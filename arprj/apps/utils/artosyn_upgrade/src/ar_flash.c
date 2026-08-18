/*
 * Artosyn program to generate spi/emmc program image
 *
 * Upgrade image layout:
 *
 * |--Header--|--Hash--|--Signature--|--Version--|--Romcode--|--bootloader--|--Uboot env--|--Partitions[]--|--Segment info[]--|--Segment data[]--|
 */

#define _LARGEFILE64_SOURCE
/* office MTD_UTILS macro, keep the same */
#define PROGRAM_NAME 	"Upgrade"

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <unistd.h>
#include <malloc.h>
#include <dirent.h>
#include <errno.h>
#include <asm/byteorder.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/blkpg.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <mtd/mtd-user.h>
#include <linux/mmc/ioctl.h>
#include <libfdt_env.h>
#include <fdt.h>
#include <libfdt.h>

#include "lzo.h"
#include "tomcrypt.h"
#include "uuid.h"
#include "crc32.h"
#include "part_efi.h"
#include "ar_flash.h"

#include <libubi.h>
#include <libmtd.h>
#include <crc32.h>
#include "mtd/mtd-abi.h"
#include "ubi-common.h"
#include "mmc.h"
#include <fdtdec.h>

/* "ARTO" */
#define MAGIC 0x4152544f

#define FLASH_SHIFT 9
#define FLASH_BLKSZ 512
#define COMPR_SEG_SIZE 0x500000 /* 5M */

#define ARCH_DMA_MINALIGN 64
#define GPT_BUF_SZ (40 * FLASH_BLKSZ)

/* Upgrade image file name and public file name */
#define UPGRADE_PUBKEY_FILE  "/etc/artosyn_upgrade_public_rsa2048.pem"

#ifndef min
	#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define ROUND_DOWN(x, align) ((x) & ~((align) - 1))

#define ROUND_UP(x, align) ((x + (align) - 1) & ~((align) - 1))

#define SDK_VERSION_SIZE 		128
#define PART_NUM_MAX 			64

/* Artosyn signed image header, 256 bytes */
struct upgrade_hdr {
	__le32 magic;
	unsigned char hdr_version;
	unsigned char compressed;
	unsigned char flashtype; /* 0:spinor 1:emmc 2:spinand */
	unsigned char part_status; /* whether partitions are changed */
	__le16 header_ext_size;
	__le16 hash_size;
	__le16 sig_size;
	__le16 sig_realsize;
	/* size of the image which lays after signature */
	__le64 img_size;
	__le32 rom_size;
	__le32 loader_size;
	__le16 partitions;
	__le16 segments;
	__le32 object_version; // object_version > depend_version
	__le32 depend_version;
	unsigned char reserve[20];
	unsigned char part_flag[PART_NUM_MAX]; /* 64 */
	unsigned char sdk_version[SDK_VERSION_SIZE]; /* 128 */
} __attribute__((packed));

/* Upgrade parition table entry */
struct part_info {
	unsigned char name[32];
	__le64 flash_offset;
	__le64 length;
	__le32 is_upgrade;
} __attribute__((packed));

/* Upgrade image segment table */
struct segment_info {
	__le64 img_offset;
	__le64 flash_offset;
	__le64 size_compress;
	__le64 size_decompress;
} __attribute__((packed));

/*
 * Maximum amount of consequtive eraseblocks which are considered as normal by
 * this utility. Otherwise it is assume that something is wrong with the flash
 * or the driver, and eraseblocks are stopped being marked as bad.
 */
#define MAX_CONSECUTIVE_BAD_BLOCKS 4

#define DEFAULT_CTRL_DEV 		"/dev/ubi_ctrl"
#define NAND_PARTITIONS_MAX 	32

enum part_status {
	PART_STATUS_NOCHANGE = 0,
	PART_STATUS_CHANGE,
	PART_STATUS_UNKNOWN
};

enum {
	FLASH_TYPE_SPINOR = 0,
	FLASH_TYPE_MMC = 1,
	FLASH_TYPE_SPINAND = 2,
};

enum {
	UPGRADE_TYPE_NOR,
	UPGRADE_TYPE_MCP,
	UPGRADE_TYPE_MMC,
	UPGRADE_TYPE_NAND,
};

struct upgrade_ops {
	unsigned char *type;
	unsigned char *disk_rom;
	unsigned char *disk_boot0;
	unsigned char *disk_boot1;
	unsigned char *disk_usr;
	unsigned long long usr_size;

	size_t gpt_size;
	size_t erasesize;

	/* Size of data that has been written to flash */
	unsigned long long writtensize;
	/* Total size of data that is going to write to flash */
	unsigned long long totalsize;
	int (*upgrade_init)(struct upgrade_ops *ops, struct upgrade_hdr *hdr);
	int (*upgrade_erase)(struct upgrade_ops *ops,
			struct part_info *parts);
	int (*upgrade_rom)(struct upgrade_ops *ops,
			unsigned char *rom, size_t size);
	int (*upgrade_bootloader)(struct upgrade_ops *ops,
			unsigned char *bootloader, size_t size);
	union {
		/* used by nor and emmc */
		int (*upgrade_segments)(struct upgrade_ops *ops,
				struct segment_info *segment, unsigned char *data);
		/* used by nand */
		int (*upgrade_segments_nand)(struct upgrade_ops *ops,
				struct segment_info *segment, unsigned char *data, struct part_info *part_info, int nrparts, bool last);
	};
	int (*upgrade_gpt)(struct upgrade_ops *ops,
			struct part_info *parts, int part);
	int (*upgrade_callback)(int state, int percent);
};

/*
 * If an eraseblock does not contain an erase counter, this value is used
 * instead of the erase counter.
 */
#define NO_EC 0xFFFFFFFF

/*
 * If an eraseblock contains a corrupted erase counter, this value is used
 * instead of the erase counter.
 */
#define CORRUPT_EC 0xFFFFFFFE

/*
 * If an eraseblock does not contain an erase counter, one of these values is
 * used.
 *
 * @EB_EMPTY: the eraseblock appeared to be empty
 * @EB_CORRUPTED: the eraseblock contains corrupted erase counter header
 * @EB_ALIEN: the eraseblock contains some non-UBI data
 * @EC_MAX: maximum allowed erase counter value
 */
enum
{
	EB_EMPTY     = 0xFFFFFFFF,
	EB_CORRUPTED = 0xFFFFFFFE,
	EB_ALIEN     = 0xFFFFFFFD,
	EB_BAD       = 0xFFFFFFFC,
	EC_MAX       = UBI_MAX_ERASECOUNTER,
};

/*
 * struct ubi_scan_info - UBI scanning information.
 * @ec: erase counters or eraseblock status for all eraseblocks
 * @mean_ec: mean erase counter
 * @ok_cnt: count of eraseblock with correct erase counter header
 * @empty_cnt: count of supposedly eraseblocks
 * @corrupted_cnt: count of eraseblocks with corrupted erase counter header
 * @alien_cnt: count of eraseblock containing non-ubi data
 * @bad_cnt: count of bad eraseblocks
 * @good_cnt: count of non-bad eraseblocks
 * @vid_hdr_offs: volume ID header offset from the found EC headers (%-1 means
 *                undefined)
 * @data_offs: data offset from the found EC headers (%-1 means undefined)
 */
struct ubi_scan_info
{
	uint32_t *ec;
	long long mean_ec;
	int ok_cnt;
	int empty_cnt;
	int corrupted_cnt;
	int alien_cnt;
	int bad_cnt;
	int good_cnt;
	int vid_hdr_offs;
	int data_offs;
};

/*
 * struct ubigen_info - libubigen information.
 * @leb_size: logical eraseblock size
 * @peb_size: size of the physical eraseblock
 * @min_io_size: minimum input/output unit size
 * @vid_hdr_offs: offset of the VID header
 * @data_offs: data offset
 * @ubi_ver: UBI version
 * @vtbl_size: volume table size
 * @max_volumes: maximum amount of volumes
 * @image_seq: UBI image sequence number
 */
struct ubigen_info
{
	int leb_size;
	int peb_size;
	int min_io_size;
	int vid_hdr_offs;
	int data_offs;
	int ubi_ver;
	int vtbl_size;
	int max_volumes;
	uint32_t image_seq;
};

/*
 * struct ubigen_vol_info - information about a volume.
 * @id: volume id
 * @type: volume type (%UBI_VID_DYNAMIC or %UBI_VID_STATIC)
 * @alignment: volume alignment
 * @data_pad: how many bytes are unused at the end of the each physical
 *            eraseblock to satisfy the requested alignment
 * @usable_leb_size: LEB size accessible for volume users
 * @name: volume name
 * @name_len: volume name length
 * @compat: compatibility of this volume (%0, %UBI_COMPAT_DELETE,
 *          %UBI_COMPAT_IGNORE, %UBI_COMPAT_PRESERVE, or %UBI_COMPAT_REJECT)
 * @used_ebs: total number of used logical eraseblocks in this volume (relevant
 *            for static volumes only)
 * @bytes: size of the volume contents in bytes (relevant for static volumes
 *         only)
 * @flags: volume flags (%UBI_VTBL_AUTORESIZE_FLG)
 */
struct ubigen_vol_info
{
	int id;
	int type;
	int alignment;
	int data_pad;
	int usable_leb_size;
	const char *name;
	int name_len;
	int compat;
	int used_ebs;
	long long bytes;
	uint8_t flags;
};

#define DEVNODE_NAME_LEN 		64
/* The variables below are set by command line arguments */
struct args {
	unsigned int yes:1;
	unsigned int quiet:1;
	unsigned int verbose:1;
	unsigned int override_ec:1;
	unsigned int novtbl:1;
	unsigned int manual_subpage;
	int subpage_size;
	int vid_hdr_offs;
	int ubi_ver;
	uint32_t image_seq;
	off_t image_sz;
	long long ec;
	const char *image;
	char node[DEVNODE_NAME_LEN];
	int node_fd;
};

/* used by this file */
#define PROGRAM   		 "ubiformat: "
#define MCP_ERASEBLOCK_CNT 		1024
#define AR_UPGRADE_DEBUG

#undef debug
#undef error
#ifdef AR_UPGRADE_DEBUG
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define error(fmt, ...) printf("[%s %d] " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#define error(fmt, ...)
#endif

#define PARTITION_VALID_SHIFT 	44
#define PARTITION_VALID_MASK 	(1ULL << PARTITION_VALID_SHIFT)

#define DUAL_PART_FLAG_SHIFT 	0
#define DUAL_PART_FLAG_MASK 	(0x01 << DUAL_PART_FLAG_SHIFT)

#define NAND_SPL_PAGES 		128
#define NAND_SPL_PARTS 		4
#define NAND_PES_PER_GPT 	2

#define PART_NAME_LEN 	16

#define VERIFY_IMAGE_UNIT 	(0xa00000) /* 10M */
#define UPGRADE_UNIT 		VERIFY_IMAGE_UNIT

static char disk_usr[PART_NAME_LEN] = {0};
static char disk_rom[PART_NAME_LEN] = {0};
static char disk_boot0[PART_NAME_LEN] = {0};
static char disk_boot1[PART_NAME_LEN] = {0};
static unsigned long long partitions_size;
struct args args;
static unsigned int object_version = 0;
static int higher_version = -1;
static char higher_valid[128] = {0};
static int develop = 1;
struct upgrade_hdr *phdr;
bool single_upgrade;
int nand_upgrade = 0;

static struct upgrade_ops nand_upgrade_ops;

typedef int (*ar_flash_cb)(int state, int percent);

static int develop_erase(struct upgrade_ops *ops);
static int do_erase_gpt(struct upgrade_ops *ops);
static int nand_mtd_create_partitions(struct part_info *part_info, int nrparts);
static int mark_bad(const struct mtd_dev_info *mtd, struct ubi_scan_info *si, int eb);
static int nand_ubidetach_mtd_delete_partitions(libmtd_t libmtd);
static int nand_get_partnr_off(libmtd_t libmtd, uint64_t offset);
static unsigned long long mtd_get_disk_size(unsigned char *disk);
static unsigned long mtd_get_disk_offset(unsigned char *disk);
static unsigned long mtd_get_disk_type(unsigned char *disk);
static int get_nand_mtd_master(char *master);

/* the code in the boot and ota is the same */
static void print_buf(unsigned char *buf, int len)
{
	int i = 0;

	for(i = 0; i < len; i++) {
		debug("%02x ", buf[i]);
		if((i + 1) % 16 == 0 || i == len - 1)
			debug("\n");
	}
}

static int vsystem(const char *command)
{
	int status, ret = 0;
	pid_t pid;

	pid = vfork();
	if (pid < 0) {
		error("vfork failed\n");
		return -1;
	} else if (pid == 0) {
		ret = execl("/bin/sh", "sh", "-c", command, (char *)0);
		if (ret < 0) {
			error("execl failed, errno = %d\n", errno);
			return -1;
		}
	}
	ret = waitpid(pid, &status, 0);
	if (ret != pid) {
		error("waitpid failed, ret = %d\n", ret);
		return -1;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status)) {
			error("exit status = %d\n", WEXITSTATUS(status));
			return -1;
		}
	} else {
		error("abort exceptly\n");
		return -1;
	}

	return 0;
}

static int rsa_load_public_key(unsigned char *path, rsa_key *key)
{
	struct stat st;
	int fd, ret = 0, len_rsa, key_sz;
	unsigned char *buf_rsa;

	/* Read rsa key file */
	ret = stat((char *)path, &st);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		return 0;
	}
	len_rsa = st.st_size;

	buf_rsa = (unsigned char *)malloc(len_rsa);
	if(!buf_rsa) {
		error("Error %d\n", __LINE__);
		return 0;
	}
	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return 0;
	}
	ret = read(fd, buf_rsa, len_rsa);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		close(fd);
		goto out;
	}
	close(fd);

	/* Import RSA key */
	ret = rsa_import(buf_rsa, len_rsa, key);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto out;
	}

	/* Signature is the same length with modulus */
	key_sz = mp_unsigned_bin_size(key->N);

	/* RSA 2048 is required in this program */
	if(key_sz != 256) {
		error("Error %d\n", __LINE__);
		goto out;
	}

out:
	free(buf_rsa);
	return ret;
}

static int rsa_verify_image(unsigned char *data, struct upgrade_hdr *hdr,
								unsigned char *hash, unsigned char *sig, ar_flash_cb cb)
{
	unsigned char pubkey_file[128] = {0};
	unsigned char buf_hash_calc[256];
	void *src, *dst;
	rsa_key key;
	int i, ret = 0;
	hash_state md;
	int percent, div;
	unsigned long mod;
	unsigned long long verified = 0;
	unsigned long data_len;

	data_len = __le64_to_cpu(hdr->img_size);
	/* Init libromcrypt */
	ltc_mp = ltm_desc;

#define VERIFY_IMAGE_UNIT 	(0xa00000) /* 10M */
	mod = data_len % VERIFY_IMAGE_UNIT;
	div = data_len / VERIFY_IMAGE_UNIT;
	sha256_init(&md);
	for (i = 0; i < div; i++) {
		ret = sha256_process(&md, &data[i * VERIFY_IMAGE_UNIT], VERIFY_IMAGE_UNIT);
		if (ret != CRYPT_OK) {
			error("error %d\n", __LINE__);
			return -1;
		}
		verified += VERIFY_IMAGE_UNIT;
		percent = verified * 100 / data_len;
		if (cb)
			cb(STAT_VERIFY_IMAGE, percent);
	}

	if (mod) {
		ret = sha256_process(&md, &data[i * VERIFY_IMAGE_UNIT], mod);
		if (ret != CRYPT_OK) {
			error("error %d\n", __LINE__);
			return -1;
		}
	}

	ret = sha256_done(&md, buf_hash_calc);
	if (ret != CRYPT_OK) {
		error("error %d\n", __LINE__);
		return -1;
	}

	for(i = 0; i < 32; i++) {
		if(buf_hash_calc[i] != hash[i]) {
			error("error %d\n", __LINE__);
			return -1;
		}
	}

	sprintf((char *)pubkey_file, "%s", UPGRADE_PUBKEY_FILE);

	ret = rsa_load_public_key(pubkey_file, &key);
	if(ret < 0) {
		error("error %d\n", __LINE__);
		return -1;
	}

	ret = mp_unsigned_bin_size(key.N);

	/* Init big number */
	mp_init(&src);
	mp_init(&dst);

	/* Convert char to big number */
	if (hdr->hdr_version > 1)
		mp_read_unsigned_bin(src, sig, __le16_to_cpu(hdr->sig_realsize));
	else
		mp_read_unsigned_bin(src, sig, 256);
	/* Do expt mode */
	mp_exptmod(src, key.e, key.N, dst);
	/* Convert big number to char */
	mp_to_unsigned_bin(dst, buf_hash_calc);

	ret = mp_unsigned_bin_size(dst);
	if (ret < 0) {
		ret = -1;
		goto out;
	}

	for(i = 0; i < ret; i++) {
		if(buf_hash_calc[i] != hash[32 - ret + i]) {
			printf("signature verify failed\n;");
			printf("stored in image:\n");
			print_buf(hash, 32);
			printf("calculated by image:\n");
			print_buf(buf_hash_calc, ret);

			ret = -1;
			goto out;
		}
	}

	if (cb && mod)
		cb(STAT_VERIFY_IMAGE, 100);
out:
	mp_clear(src);
	mp_clear(dst);
	return ret;
}

static int rsa_verify_image_ex(unsigned char *data, struct upgrade_hdr *hdr,
								unsigned char *hash, unsigned char *sig, ar_flash_cb cb, int fd)
{
	unsigned char pubkey_file[128] = {0};
	unsigned char buf_hash_calc[256];
	void *src, *dst;
	rsa_key key;
	int i, ret = 0;
	hash_state md;
	int percent, div;
	unsigned long mod;
	unsigned long long verified = 0;
	unsigned long data_len;
	int skip_len;
	char *tmp_buf;

	data_len = __le64_to_cpu(hdr->img_size);
	/* Init libromcrypt */
	ltc_mp = ltm_desc;

	skip_len = 32 + 256 + sizeof(*hdr);

	sha256_init(&md);

	ret = sha256_process(&md, data, VERIFY_IMAGE_UNIT - skip_len);
	if (ret != CRYPT_OK) {
		error("error %d\n", __LINE__);
		return -1;
	}

	verified += VERIFY_IMAGE_UNIT - skip_len;
	
	data_len -= verified;
	mod = data_len % VERIFY_IMAGE_UNIT;
	div = data_len / VERIFY_IMAGE_UNIT;

	tmp_buf = malloc(UPGRADE_UNIT);
	if (!tmp_buf) {
		printf("malloc failed\n");
		return -1;
	}

	for (i = 0; i < div; i++) {
		ret = read(fd, tmp_buf, VERIFY_IMAGE_UNIT);
		if (ret < 0) {
			printf("read failed\n");
			return -1;
		}

		ret = sha256_process(&md, tmp_buf, VERIFY_IMAGE_UNIT);
		if (ret != CRYPT_OK) {
			error("error %d\n", __LINE__);
			return -1;
		}
		verified += VERIFY_IMAGE_UNIT;
		percent = verified * 100 / data_len;
		if (cb)
			cb(STAT_VERIFY_IMAGE, percent);
	}

	if (mod) {
		ret = read(fd, tmp_buf, mod);
		if (ret < 0) {
			printf("read failed\n");
			return -1;
		}

		ret = sha256_process(&md, tmp_buf, mod);
		if (ret != CRYPT_OK) {
			error("error %d\n", __LINE__);
			return -1;
		}
	}
	ret = sha256_done(&md, buf_hash_calc);
	if (ret != CRYPT_OK) {
		error("error %d\n", __LINE__);
		return -1;
	}

	for(i = 0; i < 32; i++) {
		if(buf_hash_calc[i] != hash[i]) {
			error("error %d\n", __LINE__);
			return -1;
		}
	}

	sprintf((char *)pubkey_file, "%s", UPGRADE_PUBKEY_FILE);

	ret = rsa_load_public_key(pubkey_file, &key);
	if(ret < 0) {
		error("error %d\n", __LINE__);
		return -1;
	}

	ret = mp_unsigned_bin_size(key.N);

	/* Init big number */
	mp_init(&src);
	mp_init(&dst);

	/* Convert char to big number */
	if (hdr->hdr_version > 1)
		mp_read_unsigned_bin(src, sig, __le16_to_cpu(hdr->sig_realsize));
	else
		mp_read_unsigned_bin(src, sig, 256);
	/* Do expt mode */
	mp_exptmod(src, key.e, key.N, dst);
	/* Convert big number to char */
	mp_to_unsigned_bin(dst, buf_hash_calc);

	ret = mp_unsigned_bin_size(dst);
	if (ret < 0) {
		ret = -1;
		goto out;
	}

	for(i = 0; i < ret; i++) {
		if(buf_hash_calc[i] != hash[32 - ret + i]) {
			printf("signature verify failed\n;");
			printf("stored in image:\n");
			print_buf(hash, 32);
			printf("calculated by image:\n");
			print_buf(buf_hash_calc, ret);

			ret = -1;
			goto out;
		}
	}

	if (cb && mod)
		cb(STAT_VERIFY_IMAGE, 100);
out:
	if (tmp_buf)
		free(tmp_buf);
	mp_clear(src);
	mp_clear(dst);
	return ret;
}

static int get_part_info_offset(struct part_info *part_info, int nrparts, struct segment_info *segment)
{
	int i;

	/* avoid segments spreading over two partitions */
	for (i = 0; i < nrparts; i++) {
		if (__le64_to_cpu(segment->flash_offset) >= __le64_to_cpu(part_info[i].flash_offset)
				&& __le64_to_cpu(segment->flash_offset) + __le64_to_cpu(segment->size_decompress)
					<= __le64_to_cpu(part_info[i].flash_offset) + __le64_to_cpu(part_info[i].length)) {
			return i;
		}
	}

	return -1;
}

static int get_segment_cnt_one_part(struct part_info *part_info, struct segment_info *segment)
{
	int i;

	for (i = 0; ;i++) {
		if (__le64_to_cpu(segment[i].flash_offset) >= __le64_to_cpu(part_info->flash_offset) + __le64_to_cpu(part_info->length))
			break;
	}

	return i;
}

static int dump_upgrade_file(struct upgrade_ops *ops, unsigned char *buf)
{
	int i;
	struct upgrade_hdr *hdr = (struct upgrade_hdr *)buf;
	struct part_info *parts;
	struct segment_info *segments;

	ops->totalsize = 0;
	ops->writtensize = 0;

	debug("\n====================upgrade bin infomation====================\n\n");
	debug("magic:            0x%x\n",   __le32_to_cpu(hdr->magic));
	debug("hdr_version:      0x%x\n",   hdr->hdr_version);
	debug("compressed:       0x%x\n",   hdr->compressed);
	debug("flashtype:        0x%x\n",   hdr->flashtype);
	debug("header_ext size:  0x%x\n",   __le16_to_cpu(hdr->header_ext_size));
	debug("hash size:        0x%x\n",   __le16_to_cpu(hdr->hash_size));
	debug("sig size:         0x%x\n",   __le16_to_cpu(hdr->sig_size));
	debug("sig_realsize:     0x%x\n",   __le16_to_cpu(hdr->sig_realsize));
	debug("img size:         0x%llx\n", __le64_to_cpu(hdr->img_size));
	debug("rom_size:         0x%x\n",   __le32_to_cpu(hdr->rom_size));
	debug("loader_size:      0x%x\n",   __le32_to_cpu(hdr->loader_size));
	debug("partitions:       0x%x\n",   __le16_to_cpu(hdr->partitions));
	debug("segments:         0x%x\n",   __le16_to_cpu(hdr->segments));
	debug("obj_version:      0x%x\n",   __le32_to_cpu(hdr->object_version));
	debug("dep_version:      0x%x\n",   __le32_to_cpu(hdr->depend_version));
	if (hdr->part_status == PART_STATUS_NOCHANGE)
		printf("part_status:      no change\n");
	else if (hdr->part_status == PART_STATUS_CHANGE)
		printf("part_status:      changed\n");
	else
		printf("part_status:      unknown\n");

	object_version = __le32_to_cpu(hdr->object_version);
	debug("\n");

	buf += sizeof(*hdr);
	buf += __le16_to_cpu(hdr->header_ext_size); /* now is 0 */
	debug("Hash:\n");
	print_buf(buf, 32);
	debug("\n");

	buf += 32;
	debug("RSA Signature:\n");
	print_buf(buf, 256);
	debug("\n");

	buf += 256;
	buf +=	__le32_to_cpu(hdr->rom_size) + __le32_to_cpu(hdr->loader_size);

	parts = (struct part_info *)buf;

	/* init here, not when declarations, because uboot command upgrade many times */
	partitions_size = 0;
	debug("Upgrade image partition table:\n");
	for(i = 0; i < __le16_to_cpu(hdr->partitions); i++) {
		debug("part %d, name %8s, length 0x%08llx\n", i, parts->name, parts->length);
		partitions_size += __le64_to_cpu(parts->length);
		parts++;
	}

	debug("\n");

	buf += sizeof(struct part_info) * __le16_to_cpu(hdr->partitions);

	ops->totalsize += __le32_to_cpu(hdr->rom_size);
	if (hdr->flashtype == FLASH_TYPE_SPINAND)
		ops->totalsize += __le32_to_cpu(hdr->loader_size) * NAND_SPL_PARTS; /* only nand has 4 spl */
	else
		ops->totalsize += __le32_to_cpu(hdr->loader_size) * 2;

	ops->totalsize += ops->gpt_size;

	segments = (struct segment_info *)buf;

	debug("Upgrade image segments:\n");
	for(i = 0; i < __le32_to_cpu(hdr->segments); i++) {
		debug("segment %03d, img offset=0x%08llx, flash offset=0x%08llx, compress size=0x%08llx, decompress size=0x%08llx\n",
				i,
				segments->img_offset, segments->flash_offset,
				segments->size_compress, segments->size_decompress);

		ops->totalsize += __le64_to_cpu(segments->size_decompress);

		segments++;
	}

	debug("\n====================upgrade bin infomation done====================\n\n\n");

	return 0;
}

static inline u32 efi_crc32(const void *buf, u32 len)
{
	return crc32(0, buf, len);
}

/*
 * size: block number of disk
 */
static int gpt_set_protective_mbr(legacy_mbr *p_mbr, unsigned long long size)
{
	memset(p_mbr, 0, sizeof(*p_mbr));

	if (p_mbr == NULL) {
		error("%s: calloc failed!\n", __func__);
		return -1;
	}

	/* Append signature */
	p_mbr->signature = MSDOS_MBR_SIGNATURE;
	p_mbr->partition_record[0].sys_ind = EFI_PMBR_OSTYPE_EFI_GPT;
	p_mbr->partition_record[0].start_sect = 1;
	p_mbr->partition_record[0].nr_sects = (u32)(size) - 1;

	return 0;
}

/*
 * because the two functions below has the same name functions
 * in part_efi.c of the boot, so change names.
 * size: block number of disk
 */
static int gpt_fill_header_upgrade(gpt_header *gpt_h, char *str_guid, unsigned long long size, size_t gpt_size)
{
	gpt_h->signature = __cpu_to_le64(GPT_HEADER_SIGNATURE);
	gpt_h->revision = __cpu_to_le32(GPT_HEADER_REVISION_V1);
	gpt_h->header_size = __cpu_to_le32(sizeof(gpt_header));
	gpt_h->my_lba = __cpu_to_le64(1);
	gpt_h->alternate_lba = __cpu_to_le64(size - 1);
	gpt_h->first_usable_lba = __cpu_to_le64(gpt_size);
	gpt_h->last_usable_lba = __cpu_to_le64(size - 34); /* based on uefi spec */
	gpt_h->partition_entry_lba = __cpu_to_le64(2);
	gpt_h->num_partition_entries = __cpu_to_le32(GPT_ENTRY_NUMBERS);
	gpt_h->sizeof_partition_entry = __cpu_to_le32(sizeof(gpt_entry));
	gpt_h->header_crc32 = 0;
	gpt_h->partition_entry_array_crc32 = 0;
	gpt_h->reserved1 = __cpu_to_le32(object_version);

	if (uuid_str_to_bin(str_guid, gpt_h->disk_guid.b, UUID_STR_FORMAT_GUID))
		return -1;

	return 0;
}

static int gpt_fill_pte_upgrade(gpt_header *gpt_h, gpt_entry *gpt_e, struct part_info *info, int part)
{
	lbaint_t offset = (lbaint_t)__le64_to_cpu(gpt_h->first_usable_lba);
	lbaint_t start = 0;
	lbaint_t last_usable_lba = (lbaint_t)__le64_to_cpu(gpt_h->last_usable_lba);
	int i, k;
	size_t efiname_len, dosname_len;
	char *str_uuid;
	unsigned char uuid[64];
	unsigned char *bin_uuid;

	for (i = 0; i < part; i++) {
		/* partition starting lba */
		if (start && (start < offset)) {
			error("Partition overlap\n");
			return -1;
		}

		if (start) {
			gpt_e[i].starting_lba = __cpu_to_le64(start);
			offset = start + (__le64_to_cpu(info->length) >> FLASH_SHIFT);
		} else {
			gpt_e[i].starting_lba = __cpu_to_le64(offset);
			offset += __le64_to_cpu(info->length) >> FLASH_SHIFT;
		}

		if (nand_upgrade) {
			if (offset > nand_upgrade_ops.usr_size) {
				error("Partitions layout exceds disk size %lx %lx\n", offset, last_usable_lba);
				return -1;
			}
		} else {
			if (offset >= last_usable_lba) {
				error("Partitions layout exceds disk size %lx %lx\n", offset, last_usable_lba);
				return -1;
			}
		}
		/* partition ending lba */
		if ((i == part - 1) && (__le64_to_cpu(info->length) == 0))
			/* extend the last partition to maximuim */
			gpt_e[i].ending_lba = last_usable_lba;
		else
			gpt_e[i].ending_lba = __cpu_to_le64(offset - 1);

		/* partition type GUID */
		efi_guid_t partition_basic_data_guid = PARTITION_BASIC_DATA_GUID;
		memcpy(gpt_e[i].partition_type_guid.b, &partition_basic_data_guid, 16);

		/*Generate GUID*/
		gen_rand_uuid_str((char *)uuid, UUID_STR_FORMAT_GUID);

		str_uuid = (char *)uuid;
		bin_uuid = gpt_e[i].unique_partition_guid.b;

		if (uuid_str_to_bin(str_uuid, bin_uuid, UUID_STR_FORMAT_STD)) {
			error("Partition no. %d: invalid guid: %s\n",
				i, str_uuid);
			return -1;
		}

		/* partition attributes */
		memset(&gpt_e[i].attributes, 0, sizeof(gpt_entry_attributes));

		/* single part */
		if (!(phdr->part_flag[i] & DUAL_PART_FLAG_MASK)) {
			gpt_e[i].attributes.fields.reserved |= (1ULL << PARTITION_VALID_SHIFT);
		} else {
			if (info->name[strlen((char *)info->name) - 1] == '0') {
				if ((info->is_upgrade && !higher_valid[i])
						|| (!info->is_upgrade && higher_valid[i])
						|| higher_version == -1) {
					gpt_e[i].attributes.fields.reserved |= (1ULL << PARTITION_VALID_SHIFT);
					printf("dual part valid partition name: %s\n", info->name);
				}
			} else {
				if (!((gpt_e[i - 1].attributes.fields.reserved & PARTITION_VALID_MASK) >> PARTITION_VALID_SHIFT)) {
					gpt_e[i].attributes.fields.reserved |= (1ULL << PARTITION_VALID_SHIFT);
					printf("dual part valid partition name: %s\n", info->name);
				}
			}
		}

		/* partition name */
		efiname_len = sizeof(gpt_e[i].partition_name) / sizeof(efi_char16_t);
		dosname_len = sizeof(info->name);

		memset(gpt_e[i].partition_name, 0, sizeof(gpt_e[i].partition_name));

		for (k = 0; k < min(dosname_len, efiname_len); k++)
			gpt_e[i].partition_name[k] = (efi_char16_t)(info->name[k]);

		debug("%s: name: %s offset[%d]: 0x%llx 0x%llx" \
		      " size[%d]: 0x%llx\n",
		      __func__, info->name, i,
		      __le64_to_cpu(gpt_e[i].starting_lba), __le64_to_cpu(gpt_e[i].ending_lba), i, info->length);
		info++;
	}

	return 0;
}


static void gpt_prepare_backup_gpt_header(gpt_header *gpt_h)
{
	uint32_t calc_crc32;
	uint64_t val;

	/* recalculate the values for the Backup GPT Header */
	val = le64_to_cpu(gpt_h->my_lba);
	gpt_h->my_lba = gpt_h->alternate_lba;
	gpt_h->alternate_lba = cpu_to_le64(val);
	gpt_h->partition_entry_lba =
			cpu_to_le64(le64_to_cpu(gpt_h->last_usable_lba) + 1);
	gpt_h->header_crc32 = 0;

	calc_crc32 = efi_crc32((const unsigned char *)gpt_h,
			       le32_to_cpu(gpt_h->header_size));
	gpt_h->header_crc32 = cpu_to_le32(calc_crc32);
}

static int gpt_validate_entries(gpt_header *gpt_h, gpt_entry *gpt_e)
{
	uint32_t calc_crc32;

	/* Check the GUID Partition Table Entry Array CRC */
	calc_crc32 = efi_crc32((const unsigned char *)gpt_e,
		__le32_to_cpu(gpt_h->num_partition_entries) *
		__le32_to_cpu(gpt_h->sizeof_partition_entry));

	if (calc_crc32 != __le32_to_cpu(gpt_h->partition_entry_array_crc32)) {
		error("%s: 0x%x != 0x%x\n",
		       "GUID Partition Table Entry Array CRC is wrong",
		       __le32_to_cpu(gpt_h->partition_entry_array_crc32),
		       calc_crc32);
		return -1;
	}

	return 0;
}

static int gpt_validate_header(gpt_header *gpt_h, lbaint_t lba, lbaint_t lastlba)
{
	uint32_t crc32_backup = 0;
	uint32_t calc_crc32;

	/* Check the GPT header signature */
	if (__le64_to_cpu(gpt_h->signature) != GPT_HEADER_SIGNATURE) {
		error("%s signature is wrong: 0x%llX != 0x%llX\n",
		       "GUID Partition Table Header",
		       __le64_to_cpu(gpt_h->signature),
		       GPT_HEADER_SIGNATURE);
		return -1;
	}

	/* Check the GUID Partition Table CRC */
	memcpy(&crc32_backup, &gpt_h->header_crc32, sizeof(crc32_backup));
	memset(&gpt_h->header_crc32, 0, sizeof(gpt_h->header_crc32));

	calc_crc32 = efi_crc32((const unsigned char *)gpt_h,
		__le32_to_cpu(gpt_h->header_size));

	memcpy(&gpt_h->header_crc32, &crc32_backup, sizeof(crc32_backup));

	if (calc_crc32 != __le32_to_cpu(crc32_backup)) {
		error("%s CRC is wrong: 0x%x != 0x%x\n",
		       "GUID Partition Table Header",
		       __le32_to_cpu(crc32_backup), calc_crc32);
		return -1;
	}

	/*
	 * Check that the my_lba entry points to the LBA that contains the GPT
	 */
	if (__le64_to_cpu(gpt_h->my_lba) != lba) {
		error("GPT: my_lba incorrect: %llX != %lx\n",
		       __le64_to_cpu(gpt_h->my_lba),
		       (unsigned long)lba);
		return -1;
	}

	return 0;
}

static unsigned char *gpt_create(unsigned long long flash_size, struct upgrade_ops *ops,
		struct part_info *info, int part)
{
	unsigned char	guid[64];
	unsigned char	*gpt;
	legacy_mbr		*p_mbr;
	gpt_header		*p_gpt;
	gpt_entry		*p_entry;
	u32 			calc_crc32;
	int 			ret = 0;
	int 			alloc_size;
	unsigned long long gpt_sector_n;

	alloc_size = ops->gpt_size;
	gpt = malloc(alloc_size);
	if(!gpt) {
		error("error %d\n", __LINE__);
		return NULL;
	}
	memset(gpt, 0, alloc_size);

	p_mbr   = (legacy_mbr *)(gpt);
	p_gpt   = (gpt_header *)(gpt + FLASH_BLKSZ);
	p_entry = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	gpt_sector_n = flash_size / FLASH_BLKSZ;
	printf("Flash gpt_sector size 0x%llx\n", gpt_sector_n);

	ret = gpt_set_protective_mbr(p_mbr, gpt_sector_n);
	if(ret < 0) {
		error("error %d\n", __LINE__);
		goto out;
	}

	/* Genearte GUID */
	gen_rand_uuid_str((char *)guid, UUID_STR_FORMAT_GUID);

	/* Fill gpt header */
	ret = gpt_fill_header_upgrade(p_gpt, (char *)guid, gpt_sector_n, ops->gpt_size / FLASH_BLKSZ);
	if(ret < 0) {
		error("error %d\n", __LINE__);
		goto out;
	}

	printf("p_gpt header 0 %llx \n", p_gpt->signature);

	/* Fill gpt entries */
	ret = gpt_fill_pte_upgrade(p_gpt, p_entry, info, part);
	if(ret < 0) {
		error("error %d\n", __LINE__);
		goto out;
	}

	/* Generate CRC for the Primary GPT Header */
	calc_crc32 = efi_crc32((const unsigned char *)p_entry,
			      __le32_to_cpu(p_gpt->num_partition_entries) *
			      __le32_to_cpu(p_gpt->sizeof_partition_entry));
	p_gpt->partition_entry_array_crc32 = __cpu_to_le32(calc_crc32);

	calc_crc32 = efi_crc32((const unsigned char *)p_gpt,
			      __le32_to_cpu(p_gpt->header_size));
	p_gpt->header_crc32 = __cpu_to_le32(calc_crc32);

	/*Validate GPT header*/
	if (gpt_validate_header(p_gpt, GPT_PRIMARY_PARTITION_TABLE_LBA, gpt_sector_n)) {
		error("validate GPT header fail \n");
		goto out;
	}

	/*Validate GPT entry*/
	if (gpt_validate_entries(p_gpt, p_entry)) {
		printf("validate GPT entry fail \n");
		goto out;
	}

	return gpt;
out:
	free(gpt);
	return NULL;
}

static int nand_get_partnr_off(libmtd_t libmtd, uint64_t offset)
{
	int i, ret;
	uint64_t off, size;
	unsigned char path[128] = {0};
	unsigned char buf[32] = {0};
	struct mtd_info mtd_info;

	if (!libmtd)
		return -EINVAL;

	ret = mtd_get_info(libmtd, &mtd_info);
	if (ret) {
		sys_errmsg("cannot get MTD information");
		return ret;
	}

	for (i = mtd_info.lowest_mtd_num; i <= mtd_info.highest_mtd_num; i++) {
		sprintf((char *)buf, "mtd%d", i);
		sprintf((char *)path, "/sys/class/mtd/%s/offset", buf);

		ret = access((const char *)path, F_OK);
		/* file exist, it's a partition */
		if (ret == 0) {
			ret = mtd_get_disk_type(buf);
			/* not nand */
			if (ret != MTD_NANDFLASH)
				continue;

			off = mtd_get_disk_offset(buf);
			if (off < 0)
				return -1;

			if (offset >= off) {
				size = mtd_get_disk_size(buf);
				if (size <= 0)
					return -1;
				if (offset < off + size) {
					ret = i;
					break;
				}
			}
		} else
			continue;
	}

	/* device is not found */
	if (i == mtd_info.highest_mtd_num + 1)
		ret = -ENODEV;

	return ret;
}

static int all_ff(const void *buf, int len)
{
	int i;
	const uint8_t *p = buf;

	for (i = 0; i < len; i++)
		if (p[i] != 0xFF)
			return 0;
	return 1;
}

/*
 * ubi_scan - scan an MTD device.
 * @mtd: information about the MTD device to scan
 * @info: the result of the scanning is returned here
 */
static int ubi_scan(struct mtd_dev_info *mtd, int fd, struct ubi_scan_info **info,
	     int verbose)
{
	int eb, v = (verbose == 2), pr = (verbose == 1);
	struct ubi_scan_info *si;
	unsigned long long sum = 0;
	int eb_cnt;
	int writesize;

	eb_cnt = mtd->eb_cnt;
	writesize = mtd->min_io_size;

	si = calloc(1, sizeof(struct ubi_scan_info));
	if (!si)
		return printf(PROGRAM "cannot allocate %zd bytes of memory\n",
				  sizeof(struct ubi_scan_info));

	si->ec = calloc(eb_cnt, sizeof(uint32_t));
	if (!si->ec) {
		printf(PROGRAM "cannot allocate %zd bytes of memory\n",
			   eb_cnt * sizeof(uint32_t));
		goto out_si;
	}

	si->vid_hdr_offs = si->data_offs = -1;

	printf("ubiscan: start scanning eraseblocks 0-%d\n", eb_cnt);
	for (eb = 0; eb < eb_cnt; eb++) {
		int ret;
		uint32_t crc;
		struct ubi_ec_hdr ech;
		unsigned long long ec;

		if (pr) {
			printf("\n" PROGRAM "scanning eraseblock %d -- %2d %% complete  \n\n",
				   eb, (eb + 1) * 100 / eb_cnt);
			fflush(stdout);
		}

		if (v)
			printf(PROGRAM "scanning eraseblock %d", eb);

		ret = mtd_is_bad(mtd, fd, eb);
		if (ret < 0) {
			printf(PROGRAM "\nmtd_block_isbad error ret=%d\n", ret);
			goto out_ec;
		}
		if (ret) {
			si->bad_cnt += 1;
			si->ec[eb] = EB_BAD;
			if (v)
				printf(": bad\n");
			continue;
		}

		ret = mtd_read(mtd, fd, eb, 0, &ech, sizeof(struct ubi_ec_hdr));
		if (ret < 0)
			goto out_ec;

		if (be32_to_cpu(ech.magic) != UBI_EC_HDR_MAGIC) {
			if (all_ff(&ech, sizeof(struct ubi_ec_hdr))) {
				si->empty_cnt += 1;
				si->ec[eb] = EB_EMPTY;
				if (v)
					printf(": empty\n");
			} else {
				si->alien_cnt += 1;
				si->ec[eb] = EB_ALIEN;
				if (v)
					printf(": alien\n");
			}
			continue;
		}

		crc = mtd_crc32(UBI_CRC32_INIT,(const u_char *)&ech, UBI_EC_HDR_SIZE_CRC);
		if (be32_to_cpu(ech.hdr_crc) != crc) {
			si->corrupted_cnt += 1;
			si->ec[eb] = EB_CORRUPTED;
			if (v)
				printf(": bad CRC %#08x, should be %#08x\n",
				       crc, be32_to_cpu(ech.hdr_crc));
			continue;
		}

		ec = be64_to_cpu(ech.ec);
		if (ec > EC_MAX) {
			if (pr)
				printf("\n");
			printf("ubiscan: erase counter in EB %d is %llu, while this "
			       "program expects them to be less than %u\n",
			       eb, ec, EC_MAX);
			goto out_ec;
		}

		if (si->vid_hdr_offs == -1) {
			si->vid_hdr_offs = be32_to_cpu(ech.vid_hdr_offset);
			si->data_offs = be32_to_cpu(ech.data_offset);
			if (si->data_offs % mtd->min_io_size) {
				if (pr)
					printf("\n");
				if (v)
					printf(": corrupted because of the below\n");
				printf("ubiscan: bad data offset %d at eraseblock %d (n"
					"of multiple of min. I/O unit size %d\n)",
					si->data_offs, eb, writesize);
				printf("ubiscan: treat eraseblock %d as corrupted\n", eb);
				si->corrupted_cnt += 1;
				si->ec[eb] = EB_CORRUPTED;
				continue;

			}
		} else {
			if ((int)be32_to_cpu(ech.vid_hdr_offset) != si->vid_hdr_offs) {
				if (pr)
					printf("\n");
				if (v)
					printf(": corrupted because of the below\n");
				printf("ubiscan: inconsistent VID header offset: was "
					"%d, but is %d in eraseblock %d\n",
					si->vid_hdr_offs,
					be32_to_cpu(ech.vid_hdr_offset), eb);
				printf("ubiscan: treat eraseblock %d as corrupted\n", eb);
				si->corrupted_cnt += 1;
				si->ec[eb] = EB_CORRUPTED;
				continue;
			}
			if ((int)be32_to_cpu(ech.data_offset) != si->data_offs) {
				if (pr)
					printf("\n");
				if (v)
					printf(": corrupted because of the below\n");
				printf("ubiscan: inconsistent data offset: was %d, but"
					" is %d in eraseblock %d\n",
					si->data_offs,
					be32_to_cpu(ech.data_offset), eb);
				printf("ubiscan: treat eraseblock %d as corrupted\n", eb);
				si->corrupted_cnt += 1;
				si->ec[eb] = EB_CORRUPTED;
				continue;
			}
		}

		si->ok_cnt += 1;
		si->ec[eb] = ec;
		if (v)
			printf(": OK, erase counter %u\n", si->ec[eb]);
	}

	if (si->ok_cnt != 0) {
		/* Calculate mean erase counter */
		for (eb = 0; eb < eb_cnt; eb++) {
			if (si->ec[eb] > EC_MAX)
				continue;
			sum += si->ec[eb];
		}
		si->mean_ec = sum / si->ok_cnt;
	}

	si->good_cnt = eb_cnt - si->bad_cnt;
	printf("ubiscan: finished, mean EC %lld, %d OK, %d corrupted, %d empty, %d "
		"alien, bad %d\n", si->mean_ec, si->ok_cnt, si->corrupted_cnt,
		si->empty_cnt, si->alien_cnt, si->bad_cnt);

	*info = si;
	if (pr)
		printf("\n");
	return 0;

out_ec:
	free(si->ec);
out_si:
	free(si);
	*info = NULL;
	return -1;
}

/*
 * ubi_scan_free - free scanning information.
 * @si: scanning information to free
 */
static void ubi_scan_free(struct ubi_scan_info *si)
{
	free(si->ec);
	free(si);
}

/*
 * ubigen_info_init - initialize libubigen.
 * @ui: libubigen information
 * @peb_size: flash physical eraseblock size
 * @min_io_size: flash minimum input/output unit size
 * @subpage_size: flash sub-page, if present (has to be equivalent to
 *                @min_io_size if does not exist)
 * @vid_hdr_offs: offset of the VID header
 * @ubi_ver: UBI version
 * @image_seq: UBI image sequence number
 */
static void ubigen_info_init(struct ubigen_info *ui, int peb_size, int min_io_size,
		      int subpage_size, int vid_hdr_offs, int ubi_ver,
		      uint32_t image_seq)
{
	if (!vid_hdr_offs) {
		vid_hdr_offs = UBI_EC_HDR_SIZE + subpage_size - 1;
		vid_hdr_offs /= subpage_size;
		vid_hdr_offs *= subpage_size;
	}

	ui->peb_size = peb_size;
	ui->min_io_size = min_io_size;
	ui->vid_hdr_offs = vid_hdr_offs;
	ui->data_offs = vid_hdr_offs + UBI_VID_HDR_SIZE + min_io_size - 1;
	ui->data_offs /= min_io_size;
	ui->data_offs *= min_io_size;
	ui->leb_size = peb_size - ui->data_offs;
	ui->ubi_ver = ubi_ver;
	ui->image_seq = image_seq;

	ui->max_volumes = ui->leb_size / UBI_VTBL_RECORD_SIZE;
	if (ui->max_volumes > UBI_MAX_VOLUMES)
		ui->max_volumes = UBI_MAX_VOLUMES;
	ui->vtbl_size = ui->max_volumes * UBI_VTBL_RECORD_SIZE;
}

/*
 * ubigen_create_empty_vtbl - creates empty volume table.
 * @ui: libubigen information
 *
 * This function creates an empty volume table and returns a pointer to it in
 * case of success and %NULL in case of failure. The returned object has to be
 * freed with 'free()' call.
 */
static struct ubi_vtbl_record *ubigen_create_empty_vtbl(const struct ubigen_info *ui)
{
	struct ubi_vtbl_record *vtbl;
	int i;

	vtbl = calloc(1, ui->vtbl_size);
	if (!vtbl) {
		printf(PROGRAM "cannot allocate %d bytes of memory\n", ui->vtbl_size);
		return NULL;
	}

	for (i = 0; i < ui->max_volumes; i++) {
		uint32_t crc = mtd_crc32(UBI_CRC32_INIT, (const u_char *)&vtbl[i],
				     UBI_VTBL_RECORD_SIZE_CRC);
		vtbl[i].crc = cpu_to_be32(crc);
	}

	return vtbl;
}

/*
 * ubigen_init_ec_hdr - initialize EC header.
 * @ui: libubigen information
 * @hdr: the EC header to initialize
 * @ec: erase counter value
 */
static void ubigen_init_ec_hdr(const struct ubigen_info *ui,
		        struct ubi_ec_hdr *hdr, long long ec)
{
	uint32_t crc;

	memset(hdr, 0, sizeof(struct ubi_ec_hdr));

	hdr->magic = cpu_to_be32(UBI_EC_HDR_MAGIC);
	hdr->version = ui->ubi_ver;
	hdr->ec = cpu_to_be64(ec);
	hdr->vid_hdr_offset = cpu_to_be32(ui->vid_hdr_offs);
	hdr->data_offset = cpu_to_be32(ui->data_offs);
	hdr->image_seq = cpu_to_be32(ui->image_seq);

	crc = mtd_crc32(UBI_CRC32_INIT, (const u_char *)hdr, UBI_EC_HDR_SIZE_CRC);
	hdr->hdr_crc = cpu_to_be32(crc);
}

/*
 * ubigen_init_vid_hdr - initialize VID header.
 * @ui: libubigen information
 * @vi: volume information
 * @hdr: the VID header to initialize
 * @lnum: logical eraseblock number
 * @data: the contents of the LEB (static volumes only)
 * @data_size: amount of data in this LEB (static volumes only)
 *
 * Note, @used_ebs, @data and @data_size are ignored in case of dynamic
 * volumes.
 */
static void ubigen_init_vid_hdr(const struct ubigen_info *ui,
			 const struct ubigen_vol_info *vi,
			 struct ubi_vid_hdr *hdr, int lnum,
			 const void *data, int data_size)
{
	uint32_t crc;

	memset(hdr, 0, sizeof(struct ubi_vid_hdr));

	hdr->magic = cpu_to_be32(UBI_VID_HDR_MAGIC);
	hdr->version = ui->ubi_ver;
	hdr->vol_type = vi->type;
	hdr->vol_id = cpu_to_be32(vi->id);
	hdr->lnum = cpu_to_be32(lnum);
	hdr->data_pad = cpu_to_be32(vi->data_pad);
	hdr->compat = vi->compat;

	if (vi->type == UBI_VID_STATIC) {
		hdr->data_size = cpu_to_be32(data_size);
		hdr->used_ebs = cpu_to_be32(vi->used_ebs);
		crc = mtd_crc32(UBI_CRC32_INIT, (const u_char *)data, data_size);
		hdr->data_crc = cpu_to_be32(crc);
	}

	crc = mtd_crc32(UBI_CRC32_INIT,(const u_char *)hdr, UBI_VID_HDR_SIZE_CRC);
	hdr->hdr_crc = cpu_to_be32(crc);
}

/*
 * ubigen_write_layout_vol - write UBI layout volume
 * @ui: libubigen information
 * @peb1: physical eraseblock number to write the first volume table copy
 * @peb2: physical eraseblock number to write the second volume table copy
 * @ec1: erase counter value for @peb1
 * @ec2: erase counter value for @peb1
 * @vtbl: volume table
 * @fd: output file descriptor seeked to the proper position
 *
 * This function creates the UBI layout volume which contains 2 copies of the
 * volume table. Returns zero in case of success and %-1 in case of failure.
 */
int ubigen_write_layout_vol(const struct ubigen_info *ui, int peb1, int peb2,
			    long long ec1, long long ec2,
			    struct ubi_vtbl_record *vtbl, int fd)
{
	int ret;
	struct ubigen_vol_info vi;
	char outbuf[128 * 1024];
	struct ubi_vid_hdr *vid_hdr;
	off_t seek;

	vi.bytes = ui->leb_size * UBI_LAYOUT_VOLUME_EBS;
	vi.id = UBI_LAYOUT_VOLUME_ID;
	vi.alignment = UBI_LAYOUT_VOLUME_ALIGN;
	vi.data_pad = ui->leb_size % UBI_LAYOUT_VOLUME_ALIGN;
	vi.usable_leb_size = ui->leb_size - vi.data_pad;
	vi.data_pad = ui->leb_size - vi.usable_leb_size;
	vi.type = UBI_LAYOUT_VOLUME_TYPE;
	vi.name = UBI_LAYOUT_VOLUME_NAME;
	vi.name_len = strlen(UBI_LAYOUT_VOLUME_NAME);
	vi.compat = UBI_LAYOUT_VOLUME_COMPAT;

	memset(outbuf, 0xFF, ui->data_offs);
	vid_hdr = (struct ubi_vid_hdr *)(&outbuf[ui->vid_hdr_offs]);
	memcpy(outbuf + ui->data_offs, vtbl, ui->vtbl_size);
	memset(outbuf + ui->data_offs + ui->vtbl_size, 0xFF,
	       ui->peb_size - ui->data_offs - ui->vtbl_size);

	seek = (off_t) peb1 * ui->peb_size;
	if (lseek(fd, seek, SEEK_SET) != seek) {
		printf(PROGRAM "cannot seek output file\n");
		goto out_free;
	}

	ubigen_init_ec_hdr(ui, (struct ubi_ec_hdr *)outbuf, ec1);
	ubigen_init_vid_hdr(ui, &vi, vid_hdr, 0, NULL, 0);

	ret = write(fd, outbuf, ui->peb_size);
	if (ret != ui->peb_size) {
		printf(PROGRAM "cannot write %d bytes\n", ui->peb_size);
		goto out_free;
	}

	seek = (off_t) peb2 * ui->peb_size;
	if (lseek(fd, seek, SEEK_SET) != seek) {
		printf(PROGRAM "cannot seek output file\n");
		goto out_free;
	}
	ubigen_init_ec_hdr(ui, (struct ubi_ec_hdr *)outbuf, ec2);
	ubigen_init_vid_hdr(ui, &vi, vid_hdr, 1, NULL, 0);

	ret = write(fd, outbuf, ui->peb_size);
	if (ret != ui->peb_size) {
		printf(PROGRAM "cannot write %d bytes\n", ui->peb_size);
		goto out_free;
	}
	return 0;
out_free:
	return -1;
}

static void print_bad_eraseblocks(const struct mtd_dev_info *mtd,
				  const struct ubi_scan_info *si)
{
	int first = 1, eb;
	int eb_cnt;

	eb_cnt = mtd->eb_cnt;
	if (si->bad_cnt == 0)
		return;

	printf(PROGRAM "%d bad eraseblocks found, numbers: ", si->bad_cnt);
	for (eb = 0; eb < eb_cnt; eb++) {
		if (si->ec[eb] != EB_BAD)
			continue;
		if (first) {
			printf("%d", eb);
			first = 0;
		} else
			printf(", %d", eb);
	}
	printf("\n");
}

static int change_ech(struct ubi_ec_hdr *hdr, uint32_t image_seq,
		      long long ec)
{
	uint32_t crc;

	/* Check the EC header */
	if (be32_to_cpu(hdr->magic) != UBI_EC_HDR_MAGIC)
		return printf(PROGRAM "bad UBI magic %#08x, should be %#08x\n",
			      be32_to_cpu(hdr->magic), UBI_EC_HDR_MAGIC);

	crc = mtd_crc32(UBI_CRC32_INIT, (const u_char *)hdr, UBI_EC_HDR_SIZE_CRC);
	if (be32_to_cpu(hdr->hdr_crc) != crc)
		return printf(PROGRAM "bad CRC %#08x, should be %#08x\n",
			      crc, be32_to_cpu(hdr->hdr_crc));

	hdr->image_seq = cpu_to_be32(image_seq);
	hdr->ec = cpu_to_be64(ec);
	crc = mtd_crc32(UBI_CRC32_INIT, (const u_char *)hdr, UBI_EC_HDR_SIZE_CRC);
	hdr->hdr_crc = cpu_to_be32(crc);

	return 0;
}

static int drop_ffs(const struct mtd_dev_info *mtd, const void *buf, int len)
{
	int i;
	int writesize;

	writesize = mtd->min_io_size;

	for (i = len - 1; i >= 0; i--)
		if (((const uint8_t *)buf)[i] != 0xFF)
		      break;

	/* The resulting length must be aligned to the minimum flash I/O size */
	len = i + 1;
	len = (len + writesize - 1) / writesize;
	len *=  writesize;
	return len;
}

static bool part_is_rawimage(char *name)
{
	if (!strcmp(name, "env")
		|| !strcmp(name, "env0")
		|| !strcmp(name, "env1")
		|| !strcmp(name, "uboot")
		|| !strcmp(name, "uboot0")
		|| !strcmp(name, "uboot1")
		|| !strcmp(name, "kernel")
		|| !strcmp(name, "kernel0")
		|| !strcmp(name, "kernel1")
		|| !strcmp(name, "dtb")
		|| !strcmp(name, "dtb0")
		|| !strcmp(name, "dtb1")
		|| !strcmp(name, "vendor")
		|| !strcmp(name, "vendor0")
		|| !strcmp(name, "vendor1"))
		return true;

	return false;
}

static int flash_image(libmtd_t libmtd, const struct mtd_dev_info *mtd,
		       const struct ubigen_info *ui, struct ubi_scan_info *si,
			   const char *img, size_t size, int start_eb)
{
	int img_ebs, eb, written_ebs = 0, divisor, skip_data_read = 0;
	int eb_cnt;
	int erasesize;
	int mod;

	eb_cnt = mtd->eb_cnt;
	erasesize = mtd->eb_size;
	img_ebs = size / mtd->eb_size;
	/* ubi image is block aligned, but rawimage size maybe not block aligned */
	mod = size % mtd->eb_size;
	if (mod)
		img_ebs++;

	if (!libmtd || !mtd || !ui || !si || !img) {
		printf("flash_image has NULL parameter\n");
		return -EINVAL;
	}

	if (img_ebs > si->good_cnt) {
		printf(PROGRAM "image is too large (%lld bytes)\n",
			  (long long)size);
		goto out_close;
	}

	if (size % erasesize && !part_is_rawimage((char *)mtd->name)) {
		printf(PROGRAM "image (size %lld bytes) is not multiple of ""eraseblock size (%d bytes)\n",
				  (long long)size, erasesize);
		goto out_close;
	}

	if (args.verbose)
		printf(PROGRAM "will write %d eraseblocks\n", img_ebs);

	divisor = img_ebs;
	for (eb = start_eb; eb < eb_cnt; eb++) {
		int err, new_len;
		char buf[256 * 1024];
		long long ec;

		if (!args.quiet && !args.verbose) {
			printf("\n" PROGRAM "flashing eraseblock %d -- %2d %% complete  \n\n",
			   eb, (eb + 1 - start_eb) * 100 / divisor);
			fflush(stdout);
		}

		if (si->ec[eb] == EB_BAD) {
			divisor += 1;
			continue;
		}

		if (args.verbose) {
			printf(PROGRAM "eraseblock %d: erase", eb);
			fflush(stdout);
		}

		err = mtd_erase(libmtd, mtd, args.node_fd, eb);
		if (err) {
			if (!args.quiet)
				printf("\n");
			printf(PROGRAM "failed to erase eraseblock %d, ret = %d\n", eb, err);

			if (errno != EIO)
				goto out_close;

			if (mark_bad(mtd, si, eb))
				goto out_close;

			si->good_cnt--;
			continue;
		}

		if (!skip_data_read)
			memcpy(buf, img + written_ebs * erasesize, erasesize);

		skip_data_read = 0;

		if (args.override_ec)
			ec = args.ec;
		else if (si->ec[eb] <= EC_MAX)
			ec = si->ec[eb] + 1;
		else
			ec = si->mean_ec;

		if (args.verbose) {
			printf(", change EC to %lld", ec);
			fflush(stdout);
		}

		if (!part_is_rawimage((char *)mtd->name)) {
			err = change_ech((struct ubi_ec_hdr *)buf, ui->image_seq, ec);
			if (err) {
				printf("bad EC header at eraseblock %d of \"%s\"\n",
					   written_ebs, args.image);
				goto out_close;
			}
		}

		if (args.verbose) {
			printf(", write data\n");
			fflush(stdout);
		}

		/* rawimage is not block aligned, the last size is less than erasesize */
		if (mod && (written_ebs == img_ebs - 1)) {
			new_len = mod;
			if (mod % mtd->min_io_size) {
				memset(&buf[mod], 0xff, mtd->min_io_size - mod % mtd->min_io_size);
				new_len = mod + mtd->min_io_size - mod % mtd->min_io_size;
			}
		} else
			new_len = drop_ffs(mtd, buf, erasesize);

		err = mtd_write(libmtd, mtd, args.node_fd, eb, 0, buf, new_len,
				NULL, 0, 0);
		if (err) {
			printf(PROGRAM "cannot write eraseblock %d\n", eb);

			if (errno != EIO)
				goto out_close;

			if (mark_bad(mtd, si, eb))
				goto out_close;
			/*
			 * We have to make sure that we do not read next block
			 * of data from the input image or stdin - we have to
			 * write buf first instead.
			 */
			skip_data_read = 1;
			si->good_cnt--;
			continue;
		}
		si->good_cnt--;
		if (++written_ebs >= img_ebs)
			break;
	}

	if (!args.quiet && !args.verbose)
		printf("\n");
	return eb + 1;

out_close:
	return -1;
}

static int format(libmtd_t libmtd, const struct mtd_dev_info *mtd,
		  const struct ubigen_info *ui, struct ubi_scan_info *si,
		  int start_eb, int novtbl)
{
	int eb, err, write_size;
	struct ubi_ec_hdr *hdr;
	struct ubi_vtbl_record *vtbl;
	int eb1 = -1, eb2 = -1;
	long long ec1 = -1, ec2 = -1;
	int eb_cnt;
	int writesize;

	eb_cnt = mtd->eb_cnt;
	writesize = mtd->min_io_size;
	if (!libmtd || !mtd || !ui || !si) {
		printf("format has NULL parameter\n");
		return -EINVAL;
	}

	write_size = UBI_EC_HDR_SIZE + mtd->subpage_size - 1;
	write_size /= mtd->subpage_size;
	write_size *= mtd->subpage_size;

	hdr = malloc(write_size);
	if (!hdr) {
		printf(PROGRAM "cannot allocate %d bytes of memory\n", write_size);
		return -ENOMEM;
	}
	memset(hdr, 0xFF, write_size);

	for (eb = start_eb; eb < eb_cnt; eb++) {
		long long ec;

		if (!args.quiet && !args.verbose) {
			printf("\n"PROGRAM "formatting eraseblock %d -- %2d %% complete  \n\n",
			   eb, (eb + 1 - start_eb) * 100 / (eb_cnt - start_eb));
			fflush(stdout);
		}

		if (si->ec[eb] == EB_BAD)
			continue;

		if (args.override_ec)
			ec = args.ec;
		else if (si->ec[eb] <= EC_MAX)
			ec = si->ec[eb] + 1;
		else
			ec = si->mean_ec;
		ubigen_init_ec_hdr(ui, hdr, ec);

		if (args.verbose) {
			printf(PROGRAM "eraseblock %d: erase", eb);
			fflush(stdout);
		}

		err = mtd_erase(libmtd, mtd, args.node_fd, eb);
		if (err) {
			if (!args.quiet)
				printf("\n");

			printf(PROGRAM "failed to erase eraseblock %d\n", eb);
			if (errno != EIO)
				goto out_free;

			if (mark_bad(mtd, si, eb))
				goto out_free;
			continue;
		}

		if ((eb1 == -1 || eb2 == -1) && !novtbl) {
			if (eb1 == -1) {
				eb1 = eb;
				ec1 = ec;
			} else if (eb2 == -1) {
				eb2 = eb;
				ec2 = ec;
			}
			if (args.verbose)
				printf(", do not write EC, leave for vtbl\n");
			continue;
		}

		if (args.verbose) {
			printf(", write EC %lld\n", ec);
			fflush(stdout);
		}

		err = mtd_write(libmtd, mtd, args.node_fd, eb, 0, hdr,
				write_size, NULL, 0, 0);
		if (err) {
			if (!args.quiet && !args.verbose)
				printf("\n");

			printf(PROGRAM "cannot write EC header (%d bytes buffer) to eraseblock %d\n",
				   write_size, eb);

			if (errno != EIO) {
				if (args.subpage_size != writesize)
					printf(PROGRAM "may be sub-page size is incorrect?\n");
				goto out_free;
			}

			if (mark_bad(mtd, si, eb))
				goto out_free;

			continue;
		}
	}

	if (!args.quiet && !args.verbose)
		printf("\n");

	if (!novtbl) {
		if (eb1 == -1 || eb2 == -1) {
			printf(PROGRAM "no eraseblocks for volume table\n");
			goto out_free;
		}

		printf(PROGRAM "write volume table to eraseblocks %d and %d\n", eb1, eb2);
		vtbl = ubigen_create_empty_vtbl(ui);
		if (!vtbl)
			goto out_free;

		err = ubigen_write_layout_vol(ui, eb1, eb2, ec1,  ec2, vtbl,
					      args.node_fd);
		if (err) {
			printf(PROGRAM "cannot write layout volume\n");
			goto out_free;
		}
		free(vtbl);
	}

	free(hdr);
	return 0;

out_free:
	free(hdr);
	return -1;
}

static void args_init(void)
{
	/* image have vtbl already, so set it. */
	args.novtbl = 1;
	args.ubi_ver = 1;
	args.yes = 1;
	args.verbose = 1;
	args.quiet = 0;
	args.override_ec = 0;
	util_srand();
	args.image_seq = rand();
}

static int ubi_prepare(libmtd_t libmtd, struct mtd_dev_info *mtd,
		       struct ubigen_info *ui, struct ubi_scan_info *usi, uint32_t *ec)

{
	int err;
	struct ubi_scan_info *si = NULL;
	int  verbose;
	struct mtd_info mtd_info;
	libubi_t libubi;

	if (!libmtd || !mtd || !ui || !usi || !ec) {
		printf("ubi_prepare has NULL parameter\n");
		return -EINVAL;
	}

	args_init();

	err = mtd_get_info(libmtd, &mtd_info);
	if (err) {
		printf("cannot get MTD information\n");
		goto out_close_mtd;
	}

	err = mtd_get_dev_info(libmtd, args.node, mtd);
	if (err) {
		sys_errmsg("cannot get information about \"%s\"", args.node);
		goto out_close_mtd;
	}

	/* rawimage quiet */
	if (part_is_rawimage((char *)mtd->name)) {
		args.quiet = 1;
		args.verbose = 0;
	} else {
		args.quiet = 0;
		args.verbose = 1;
	}

	if (args.quiet)
		verbose = 0;
	else if (args.verbose)
		verbose = 2;
	else
		verbose = 1;

	if (!is_power_of_2(mtd->min_io_size)) {
		errmsg("min. I/O size is %d, but should be power of 2",
		       mtd->min_io_size);
		goto out_close_mtd;
	}

	if (!mtd_info.sysfs_supported) {
		/*
		 * Linux kernels older than 2.6.30 did not support sysfs
		 * interface, and it is impossible to find out sub-page
		 * size in these kernels. This is why users should
		 * provide -s option.
		 */
		if (args.subpage_size == 0) {
			warnmsg("your MTD system is old and it is impossible "
				"to detect sub-page size. Use -s to get rid "
				"of this warning");
			normsg("assume sub-page to be %d", mtd->subpage_size);
		} else {
			mtd->subpage_size = args.subpage_size;
			args.manual_subpage = 1;
		}
	} else if (args.subpage_size && args.subpage_size != mtd->subpage_size) {
		mtd->subpage_size = args.subpage_size;
		args.manual_subpage = 1;
	}

	if (args.manual_subpage) {
		/* Do some sanity check */
		if (args.subpage_size > mtd->min_io_size) {
			errmsg("sub-page cannot be larger than min. I/O unit");
			goto out_close_mtd;
		}

		if (mtd->min_io_size % args.subpage_size) {
			errmsg("min. I/O unit size should be multiple of "
			       "sub-page size");
			goto out_close_mtd;
		}
	}

	args.node_fd = open(args.node, O_RDWR);
	if (args.node_fd == -1) {
		sys_errmsg("cannot open \"%s\"", args.node);
		goto out_close_mtd;
	}

	/* Validate VID header offset if it was specified */
	if (args.vid_hdr_offs != 0) {
		if (args.vid_hdr_offs % 8) {
			errmsg("VID header offset has to be multiple of min. I/O unit size");
			goto out_close;
		}
		if (args.vid_hdr_offs + (int)UBI_VID_HDR_SIZE > mtd->eb_size) {
			errmsg("bad VID header offset");
			goto out_close;
		}
	}

	if (!mtd->writable) {
		errmsg("mtd%d (%s) is a read-only device", mtd->mtd_num, args.node);
		goto out_close;
	}
	/* Make sure this MTD device is not attached to UBI */
	libubi = libubi_open();
	if (libubi && strncmp(mtd->name, "userapp", 7)) {
		int ubi_dev_num;

		err = mtd_num2ubi_dev(libubi, mtd->mtd_num, &ubi_dev_num);
		if (!err) {
			errmsg("please, first detach mtd%d (%s) from ubi%d",
				   mtd->mtd_num, args.node, ubi_dev_num);

			normsg("detach mtd%d from ubi%d directly", mtd->mtd_num, ubi_dev_num);
			err = ubi_remove_dev(libubi, DEFAULT_CTRL_DEV, ubi_dev_num);
			if (err) {
				sys_errmsg("cannot remove ubi%d", ubi_dev_num);
				libubi_close(libubi);
				goto out_close;
			}
		}
		libubi_close(libubi);
	}

	if (!args.quiet) {
		normsg_cont("mtd%d (%s), size ", mtd->mtd_num, mtd->type_str);
		util_print_bytes(mtd->size, 1);
		printf(", %d eraseblocks of ", mtd->eb_cnt);
		util_print_bytes(mtd->eb_size, 1);
		printf(", min. I/O size %d bytes\n", mtd->min_io_size);
	}

	err = ubi_scan(mtd, args.node_fd, &si, verbose);
	if (err) {
		errmsg("failed to scan mtd%d (%s)", mtd->mtd_num, args.node);
		goto out_close;
	}

	if (si->good_cnt == 0) {
		printf(PROGRAM "all %d eraseblocks are bad\n", si->bad_cnt);
		goto out_free;
	}

	if (si->good_cnt < 2 && (!args.novtbl || args.image)) {
		printf(PROGRAM "too few non-bad eraseblocks (%d) on mtd%d\n",
		       si->good_cnt, mtd->mtd_num);
		goto out_free;
	}

	if (!args.quiet) {
		if (si->ok_cnt)
			printf(PROGRAM "%d eraseblocks have valid erase counter, mean value is %lld\n",
			       si->ok_cnt, si->mean_ec);
		if (si->empty_cnt)
			printf(PROGRAM "%d eraseblocks are supposedly empty\n", si->empty_cnt);
		if (si->corrupted_cnt)
			printf(PROGRAM "%d corrupted erase counters\n", si->corrupted_cnt);
		print_bad_eraseblocks(mtd, si);
	}

	if (si->alien_cnt) {
		if (!args.yes || !args.quiet)
			printf(PROGRAM "%d of %d eraseblocks contain non-UBI data\n",
				si->alien_cnt, si->good_cnt);
	}

	if (!args.override_ec && si->empty_cnt < si->good_cnt) {
		int percent = (si->ok_cnt * 100) / si->good_cnt;

		/*
		 * Make sure the majority of eraseblocks have valid
		 * erase counters.
		 */
		if (percent < 50) {
			if (!args.yes || !args.quiet) {
				printf(PROGRAM "only %d of %d eraseblocks have valid erase counter\n",
					si->ok_cnt, si->good_cnt);
				printf(PROGRAM "erase counter 0 will be used for all eraseblocks\n");
			}
			args.ec = 0;
			args.override_ec = 1;
		} else if (percent < 95) {
			if (!args.yes || !args.quiet) {
				printf(PROGRAM "only %d of %d eraseblocks have valid erase counter\n",
					si->ok_cnt, si->good_cnt);
				printf(PROGRAM "mean erase counter %lld will be used for the rest of eraseblock\n",
					   si->mean_ec);
			}
			args.ec = si->mean_ec;
			args.override_ec = 1;
		}
	}

	if (!args.quiet && args.override_ec)
		printf(PROGRAM "use erase counter %lld for all eraseblocks\n", args.ec);

	ubigen_info_init(ui, mtd->eb_size, mtd->min_io_size, mtd->subpage_size,
			 args.vid_hdr_offs, args.ubi_ver, args.image_seq);

	memcpy(usi, si, sizeof(*usi));
	memcpy(ec, si->ec, mtd->eb_cnt * sizeof(uint32_t));
	usi->ec = ec;
	ubi_scan_free(si);
	return 0;

out_free:
	ubi_scan_free(si);
out_close:
	close(args.node_fd);
out_close_mtd:
	return -1;
}

static bool nand_mtd_is_master(int mtdn)
{
	int ret;
	unsigned char path[128] = {0};
	unsigned char buf[32] = {0};

	sprintf((char *)buf, "mtd%d", mtdn);
	sprintf((char *)path, "/sys/class/mtd/%s/offset", buf);
	/* find nand mtd master */
	ret = access((const char *)path, F_OK);
	if (ret == -1) {
		ret = mtd_get_disk_type(buf);
		if (ret == MTD_NANDFLASH)
			return true;
	}

	return false;
}

static bool nand_mtd_is_factory(int mtdn)
{
	int ret, fd;
	char path[128] = {0};
	char buf[32] = {0};

	sprintf(buf, "mtd%d", mtdn);
	sprintf(path, "/sys/class/mtd/%s/name", buf);

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		sys_errmsg("cannot open MTD name");
		return false;
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 8);
	if (ret == -1) {
		sys_errmsg("cannot read MTD name");
		close(fd);
		return false;
	}

	close(fd);

	if (strncmp(buf, "factory\n", 8) == 0) {
		return true;
	}

	return false;
}

static bool nand_mtd_is_userapp(int mtdn)
{
	int ret, fd;
	char path[128] = {0};
	char buf[32] = {0};

	sprintf(buf, "mtd%d", mtdn);
	sprintf(path, "/sys/class/mtd/%s/name", buf);

	fd = open(path, O_RDONLY);
	if (fd == -1) {
		sys_errmsg("cannot open MTD name");
		return false;
	}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, 8);
	if (ret == -1) {
		sys_errmsg("cannot read MTD name");
		close(fd);
		return false;
	}

	close(fd);

	if (strncmp(buf, "userapp\n", 7) == 0) {
		return true;
	}

	return false;
}

static int nand_mtd_create_partitions(struct part_info *part_info, int nrparts)
{
	int fd, i, ret;
	struct blkpg_partition part;
	struct blkpg_ioctl_arg arg;
	char master[DEVNODE_NAME_LEN] = {0};

	ret = get_nand_mtd_master(master);
	if (ret)
		return ret;

	fd = open(master, O_RDWR | O_CLOEXEC);
	if (fd == -1 )
		sys_errmsg_die("Cannot open %s", master);

	memset(&part, 0, sizeof(part));

	memset(&arg, 0, sizeof(arg));
	arg.datalen = sizeof(part);
	arg.data = &part;

	/* do sanity check later */
	for (i = 0; i < nrparts; i++) {
		part.start = __le64_to_cpu(part_info->flash_offset);
		part.length = __le64_to_cpu(part_info->length);
		memcpy(part.devname, part_info->name, sizeof(part.devname));
		arg.op = BLKPG_ADD_PARTITION;

		if (ioctl(fd, BLKPG, &arg))
			sys_errmsg_die("Failed to issue BLKPG ioctl");

		part_info++;
	}
	close(fd);

	return 0;
}

static int nand_mtd_delete_partition(int mtdn)
{
	int fd, ret;
	struct blkpg_partition part;
	struct blkpg_ioctl_arg arg;
	char master[DEVNODE_NAME_LEN] = {0};

	ret = get_nand_mtd_master(master);
	if (ret)
		return ret;

	fd = open(master, O_RDWR | O_CLOEXEC);
	if (fd == -1 )
		sys_errmsg_die("Cannot open %s", master);

	memset(&part, 0, sizeof(part));

	memset(&arg, 0, sizeof(arg));
	arg.datalen = sizeof(part);
	arg.data = &part;

	normsg("delete_partition mtd%d\n", mtdn);

	part.pno = mtdn;
	arg.op = BLKPG_DEL_PARTITION;

	if (ioctl(fd, BLKPG, &arg))
		sys_errmsg_die("Failed to issue BLKPG ioctl");

	close(fd);
	return 0;
}

static int nand_ubidetach(int mtdn)
{
	int err;
	libubi_t libubi;

	libubi = libubi_open();
	if (libubi) {
		int ubi_dev_num;

		err = mtd_num2ubi_dev(libubi, mtdn, &ubi_dev_num);
		if (!err) {
			/* maybe leave the choice to user */
			normsg("detach mtd%d from ubi%d directly", mtdn, ubi_dev_num);
			err = ubi_remove_dev(libubi, DEFAULT_CTRL_DEV, ubi_dev_num);
			if (err) {
				sys_errmsg("cannot remove ubi%d", ubi_dev_num);
				libubi_close(libubi);
				return err;
			}
		}
		libubi_close(libubi);
	} else {
		sys_errmsg("libubi open failed");
		err = -2;
	}
	return err;
}

static int nand_ubidetach_mtd_delete_partitions(libmtd_t libmtd)
{
	int ret, i, j = 0;
	char buf[32] = {0};
	struct mtd_info mtd_info;
	struct mtd_dev_info mtd;
	int mtd_nand[NAND_PARTITIONS_MAX] = {0};

	if (!libmtd)
		return -EINVAL;

	ret = mtd_get_info(libmtd, &mtd_info);
	if (ret) {
		sys_errmsg("cannot get MTD information");
		return ret;
	}

	for (i = mtd_info.lowest_mtd_num; i <= mtd_info.highest_mtd_num; i++) {
		sprintf(buf, "mtd%d", i);
		ret = mtd_get_disk_type((unsigned char *)buf);
		/* not nand */
		if (ret != MTD_NANDFLASH)
			continue;
		/* master partition */
		if (nand_mtd_is_master(i))
			continue;
		/* factory partition */
		if (nand_mtd_is_factory(i))
			continue;

		/* userapp partition */
		if (nand_mtd_is_userapp(i))
			continue;

		mtd_nand[j++] = i;
		if (j > NAND_PARTITIONS_MAX) {
			sys_errmsg("nand mtd partition is too much, bigger then %d", NAND_PARTITIONS_MAX);
			return -EINVAL;
		}
	}

	ret = 0;
	for (i = 0; i < j; i++) {
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "/dev/mtd%d", mtd_nand[i]);
		ret = mtd_get_dev_info(libmtd, buf, &mtd);
		if (ret) {
			sys_errmsg("cannot get information about \"%s\"", buf);
			return ret;
		}

		ret = nand_ubidetach(mtd_nand[i]);
		/* -1: mtd no ubiattach, -2: open /dev/ubi_ctrl failed */
		if (ret && ret != -1 && ret != -2) {
			sys_errmsg("ubidetach mtd%d failed\n", i);
			break;
		}
		ret = nand_mtd_delete_partition(mtd_nand[i]);
		if (ret) {
			sys_errmsg("delete partition mtd%d failed\n", i);
			break;
		}
	}

	return ret;
}

static int answer_is_yes(const char *msg)
{
	return prompt(msg ? : "continue?", false);
}

/*
 * Returns %-1 if consecutive bad blocks exceeds the
 * MAX_CONSECUTIVE_BAD_BLOCKS and returns %0 otherwise.
 */
static int consecutive_bad_check(int eb)
{
	static int consecutive_bad_blocks = 1;
	static int prev_bb = -1;

	if (prev_bb == -1)
		prev_bb = eb;

	if (eb == prev_bb + 1)
		consecutive_bad_blocks += 1;
	else
		consecutive_bad_blocks = 1;

	prev_bb = eb;

	if (consecutive_bad_blocks >= MAX_CONSECUTIVE_BAD_BLOCKS) {
		if (!args.quiet)
			printf("\n");
		return errmsg("consecutive bad blocks exceed limit: %d, bad flash?",
		              MAX_CONSECUTIVE_BAD_BLOCKS);
	}

	return 0;
}

/* TODO: we should actually torture the PEB before marking it as bad */
static int mark_bad(const struct mtd_dev_info *mtd, struct ubi_scan_info *si, int eb)
{
	int err;

	if (!args.yes)
		if (!answer_is_yes("mark it as bad?"))
			return -1;

	if (!args.quiet)
		normsg_cont("marking block %d bad", eb);

	if (!args.quiet)
		printf("\n");

	if (!mtd->bb_allowed) {
		if (!args.quiet)
			printf("\n");
		return errmsg("bad blocks not supported by this flash");
	}

	err = mtd_mark_bad(mtd, args.node_fd, eb);
	if (err)
		return err;

	si->bad_cnt += 1;
	si->ec[eb] = EB_BAD;

	return consecutive_bad_check(eb);
}

static int mtd_get_mtd_info(const char *node, struct mtd_dev_info *mtd)
{
	int err;
	libmtd_t libmtd = NULL;

	libmtd = libmtd_open();
	if (!libmtd)
		return errmsg("MTD subsystem is not present");

	err = mtd_get_dev_info(libmtd, node, mtd);
	if (err) {
		sys_errmsg("cannot get information about \"%s\"", node);
		libmtd_close(libmtd);
		return -1;
	}

	libmtd_close(libmtd);

	return 0;
}

static unsigned long long mtd_get_disk_size(unsigned char *disk)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	unsigned long long size;
	int ret, fd;

	sprintf((char *)path, "/sys/class/mtd/%s/size", disk);

	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return 0;
	}

	ret = read(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto close_file;
	}
	close(fd);

	sscanf((char *)buff, "%llu", &size);
	return size;

close_file:
	close(fd);
	return 0;
}

static unsigned long mtd_get_erase_size(unsigned char *disk)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	unsigned long erasesize;
	int ret, fd;

	sprintf((char *)path, "/sys/class/mtd/%s/erasesize", disk);

	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return ERR_FLASH_TYPE;
	}

	ret = read(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto close_file;
	}
	close(fd);

	sscanf((char *)buff, "%lu", &erasesize);
	return erasesize;

close_file:
	close(fd);
	return 0;
}

static unsigned long mtd_get_disk_offset(unsigned char *disk)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	unsigned long offset;
	int ret, fd;

	sprintf((char *)path, "/sys/class/mtd/%s/offset", disk);

	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return ERR_FLASH_TYPE;
	}

	ret = read(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto close_file;
	}
	close(fd);

	sscanf((char *)buff, "%lu", &offset);
	return offset;

close_file:
	close(fd);
	return -1;
}

static unsigned long mtd_get_disk_type(unsigned char *disk)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	unsigned char type[16] = {0};
	int ret, fd;

	sprintf((char *)path, "/sys/class/mtd/%s/type", disk);

	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return ERR_FLASH_TYPE;
	}

	ret = read(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto close_file;
	}
	close(fd);

	sscanf((char *)buff, "%s", type);
	if (!strcmp((char *)type, "nor"))
		return MTD_NORFLASH;
	else if (!strcmp((char *)type, "nand"))
		return MTD_NANDFLASH;
	else if (!strcmp((char *)type, "ubi"))
		return MTD_UBIVOLUME;
	else
		return -1;

close_file:
	close(fd);
	return -1;
}

static unsigned long mtd_get_disk_name(char *disk, char *name)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	int ret, fd;

	if (!name)
		return -EINVAL;

	sprintf((char *)path, "/sys/class/mtd/%s/name", disk);

	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return ERR_FLASH_TYPE;
	}

	ret = read(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto close_file;
	}
	close(fd);

	sscanf((char *)buff, "%s", name);
	return 0;
close_file:
	close(fd);
	return -1;
}

/*
 * get the /dev/mtdN from name
 */
int get_mtd_num_name(char *mtd_name)
{
	int i, ret;
	char buf[32] = {0};
	char name[32] = {0};
	libmtd_t libmtd;
	struct mtd_info mtd_info;

	libmtd = libmtd_open();
	if (!libmtd)
		return errmsg("MTD subsystem is not present");

	ret = mtd_get_info(libmtd, &mtd_info);
	if (ret) {
		sys_errmsg("cannot get MTD information");
		return ret;
	}
	libmtd_close(libmtd);

	for (i = mtd_info.lowest_mtd_num; i <= mtd_info.highest_mtd_num; i++) {
		memset(buf, 0, sizeof(buf));
		memset(name, 0, sizeof(name));
		sprintf(buf, "mtd%d", i);

		ret = mtd_get_disk_name(buf, name);
		if (ret)
			continue;
		if (!strcmp(mtd_name, name)) {
			ret = i;
			break;
		}
	}

	/* device is not found */
	if (i == mtd_info.highest_mtd_num + 1)
		ret = -ENODEV;

	return ret;
}

static int get_nand_mtd_master(char *master)
{
	int i, ret;
	unsigned char path[128] = {0};
	unsigned char buf[32] = {0};
	libmtd_t libmtd;
	struct mtd_info mtd_info;

	libmtd = libmtd_open();
	if (!libmtd)
		return errmsg("MTD subsystem is not present");

	ret = mtd_get_info(libmtd, &mtd_info);
	if (ret) {
		sys_errmsg("cannot get MTD information");
		return ret;
	}
	libmtd_close(libmtd);

	/*
	 * now it should not have problem, beacause the first mtd number must be
	 * the master, even when create or delete mtd devices dymaitcly.
	 */
	for (i = mtd_info.lowest_mtd_num; i <= mtd_info.highest_mtd_num; i++) {
		sprintf((char *)buf, "mtd%d", i);
		sprintf((char *)path, "/sys/class/mtd/%s/offset", buf);

		/* find nand mtd master */
		ret = access((const char *)path, F_OK);
		if (ret == -1) {
			ret = mtd_get_disk_type(buf);
			if (ret == MTD_NANDFLASH) {
				memset(path, 0, sizeof(path));
				sprintf(master, "%s%s", "/dev/", buf);
				ret = 0;
				break;
			}
		}
	}

	/* device is not found */
	if (i == mtd_info.highest_mtd_num + 1)
		ret = -ENODEV;

	return ret;
}

/* if return 1: mcp, else is nor */
int get_mtd_flash_type(void)
{
	int ret;
	char buf[32];
	ret = get_nand_mtd_master(buf);

	/* if nand master is exist, it is mcp */
	if (!ret)
		ret = 1;
	else
		ret = 0;

	return ret;
}

static int nand_mtd_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	int ret;
	unsigned long long size, spl_size;
	struct mtd_dev_info mtd;

	strcpy(disk_usr, "mtd0");
	/* FIXME: master must be 0 now */
	ret = mtd_get_mtd_info("/dev/mtd0", &mtd);
	if (ret) {
		error("get mtd info failed\n");
		return ERR_OTHER;
	}

	ops->usr_size = mtd.size;
	ops->erasesize = mtd.eb_size;

	debug("Mtd flash size %llx, erase size %x...\n", ops->usr_size, ops->erasesize);

	spl_size = NAND_SPL_PAGES * mtd.min_io_size;
	spl_size *= NAND_SPL_PARTS;

	size = spl_size + ops->erasesize * NAND_PES_PER_GPT * 2;
	ops->gpt_size = size; /* gpt size include spl size in nand */
	if (ops->usr_size - size < partitions_size) {
		printf("\n                           ^*********^                                               \n");
		printf("partitions1 capacity: 0x%llx, but the corresponding image partitions total size: 0x%llx\n" \
				"partitions1 capacity is too small, please check your image!!!\n",
				ops->usr_size - size, partitions_size);
		printf("\n                           ^*********^                                               \n");
		return ERR_FLASH_TYPE;
	}

	return ERR_SUCCESS;
}

static int nand_mtd_erase_common(const char *node, int start_eb, int ebs)
{
	int eb_cnt, eb;
	int err;
	libmtd_t libmtd = NULL;
	struct mtd_dev_info *mtd = NULL;
	int node_fd = -1;

	libmtd = libmtd_open();
	if (!libmtd)
		return errmsg("MTD subsystem is not present");

	mtd = malloc(sizeof(struct mtd_dev_info));
	if (!mtd) {
		printf("malloc mtd_dev_info failed\n");
		goto out;
	}

	memset(mtd, 0, sizeof(*mtd));
	err = mtd_get_dev_info(libmtd, node, mtd);
	if (err) {
		sys_errmsg("cannot get information about \"%s\"", node);
		goto out;
	}

	node_fd = open(node, O_RDWR);
	if (node_fd < 0) {
		printf("cannot open %s\n", node);
		goto out;
	}

	eb_cnt = start_eb + ebs;

	for (eb = start_eb; eb < eb_cnt; eb++) {
		err = mtd_is_bad(mtd, node_fd, eb);
		if (err < 0) {
			printf("\nmtd_block_isbad error ret=%d\n", err);
			goto out;
		}
		if (err) {
			continue;
		}

		err = mtd_erase(libmtd, mtd, node_fd, eb);
		if (err) {
			printf("failed to erase eraseblock %d, ret = %d\n", eb, err);

			if (errno != EIO)
				goto out;

			err = mtd_mark_bad(mtd, node_fd, eb);
			if (err)
				goto out;

			continue;
		}
	}

	libmtd_close(libmtd);
	free(mtd);
	close(node_fd);
	return 0;

out:
	libmtd_close(libmtd);
	if (mtd)
		free(mtd);
	if (node_fd > 0)
		close(node_fd);

	return -1;
}

static int nand_mtd_read_common(int fd, struct mtd_dev_info *mtd, int start_eb, int ebs, char *data, size_t size)
{
	int eb, eb_cnt;
	int ret, new_len;

	eb_cnt = start_eb + ebs;
	for (eb = start_eb; eb < eb_cnt; eb++) {
		ret = mtd_is_bad(mtd, fd, eb);
		if (ret < 0) {
			printf("\nmtd_block_isbad error ret=%d\n", ret);
			goto out;
		}
		if (ret) {
			continue;
		}

		if (size > mtd->eb_size)
			new_len = mtd->eb_size;
		else
			new_len = size;

		ret = mtd_read(mtd, fd, eb, 0, data, new_len);
		if (ret < 0) {
			printf("mtd read failed, ret = %d\n", ret);
			goto out;
		}

		size -= new_len;
		data += new_len;
		if (!size)
			break;
	}

	if (size) {
		printf("read data failed, maybe mtd part is too small or bad blocks is too much\n");
		goto out;
	}

	return 0;

out:
	return -1;
}

/* include erase */
static int nand_mtd_write_common(const char *node, int start_eb, int ebs, const char *img, size_t size)
{
	int img_ebs, eb, written_ebs = 0;
	int eb_cnt;
	int erasesize;
	int mod;
	int err;
	libmtd_t libmtd = NULL;
	struct mtd_dev_info *mtd = NULL;
	int node_fd = -1;

	libmtd = libmtd_open();
	if (!libmtd)
		return errmsg("MTD subsystem is not present");

	mtd = malloc(sizeof(struct mtd_dev_info));
	if (!mtd) {
		printf("malloc mtd_dev_info failed\n");
		goto out;
	}

	memset(mtd, 0, sizeof(*mtd));
	err = mtd_get_dev_info(libmtd, node, mtd);
	if (err) {
		sys_errmsg("cannot get information about \"%s\"", node);
		goto out;
	}

	node_fd = open(node, O_RDWR);
	if (node_fd < 0) {
		printf("cannot open %s\n", node);
		goto out;
	}

	eb_cnt = start_eb + ebs;
	erasesize = mtd->eb_size;
	img_ebs = size / mtd->eb_size;
	/* ubi image is block aligned, but rawimage size maybe not block aligned */
	mod = size % mtd->eb_size;
	if (mod)
		img_ebs++;

	if (img_ebs > ebs) {
		printf("image is too large (%lld bytes)\n",
			  (long long)size);
		goto out;
	}

	for (eb = start_eb; eb < eb_cnt; eb++) {
		int new_len;

		err = mtd_is_bad(mtd, node_fd, eb);
		if (err < 0) {
			printf("\nmtd_block_isbad error ret=%d\n", err);
			goto out;
		}
		if (err) {
			continue;
		}

		err = mtd_erase(libmtd, mtd, node_fd, eb);
		if (err) {
			printf("failed to erase eraseblock %d, ret = %d\n", eb, err);

			if (errno != EIO)
				goto out;

			err = mtd_mark_bad(mtd, node_fd, eb);
			if (err)
				goto out;

			continue;
		}

		if (size > erasesize)
			new_len = erasesize;
		else
			new_len = size;

		err = mtd_write(libmtd, mtd, node_fd, eb, 0, img, new_len,
				NULL, 0, 0);
		if (err) {
			printf("cannot write eraseblock %d\n", eb);

			if (errno != EIO)
				goto out;

			err = mtd_mark_bad(mtd, node_fd, eb);
			if (err)
				goto out;

			continue;
		}
		img += new_len;
		size -= new_len;
		if (++written_ebs >= img_ebs)
			break;
	}

	if (size) {
		printf("mtd part is too small or has too many bad blocks\n");
		goto out;
	}

	libmtd_close(libmtd);
	free(mtd);
	close(node_fd);
	return 0;

out:
	libmtd_close(libmtd);
	if (mtd)
		free(mtd);
	if (node_fd > 0)
		close(node_fd);

	return -1;
}

static int nand_mtd_upgrade_bootloader(struct upgrade_ops *ops,
		unsigned char *bootloader, size_t size)
{
	int i, ret;
	unsigned int start;
	unsigned int spl_size;
	struct mtd_dev_info mtd;

	ret = mtd_get_mtd_info("/dev/mtd0", &mtd);
	if (ret) {
		error("get mtd info failed\n");
		return ERR_OTHER;
	}
	/* spl: 128 pages */
	spl_size = NAND_SPL_PAGES * mtd.min_io_size;

	for (i = 0; i < NAND_SPL_PARTS; i++) {
		start = i * spl_size;

		debug("Erase and Write mtd bootloader%d...\n", i);
		/* if has bad blocks, it return error, just ignore it */
		nand_mtd_write_common("/dev/mtd0", start / mtd.eb_size, spl_size / mtd.eb_size, bootloader, size);
	}

	return ERR_SUCCESS;
}

static int nand_mtd_upgrade_segments(struct upgrade_ops *ops,
		struct segment_info *segment, unsigned char *data, struct part_info *part_info, int nrparts, bool last)
{
	int ret;
	uint64_t len;
	uint64_t seg_flash_offset;

	static struct ubigen_info ui;
	static struct ubi_scan_info si;
	static uint32_t ec[MCP_ERASEBLOCK_CNT];
	static int start_eb = 0;
	static libmtd_t libmtd = NULL;
	static struct mtd_dev_info mtd;
	static int partnr_upgrade = -1;
	int partnr_tmp = -1;

	len = __le64_to_cpu(segment->size_decompress);
	seg_flash_offset = __le64_to_cpu(segment->flash_offset);

	/* the first entry, delete all partitions */
	if (partnr_upgrade == -1) {
		libmtd = libmtd_open();
		if (!libmtd)
			return errmsg("MTD subsystem is not present");
		printf("before nand upgrade, ubidetach and delete all nand mtd partitions\n");
		ret = nand_ubidetach_mtd_delete_partitions(libmtd);
		if (ret) {
			printf("delete partitions failed\n");
			return ERR_PROGRAM;
		}
	}

	printf("\nsearch the mtd partition by flash offset = 0x%llx\n", seg_flash_offset);
	/*
	 * every entry, find the partition by the segment flash offset,
	 * beacause delete all partitions before upgrade, so create the
	 * partition when flash the first segment of the partition.
	 */
	partnr_tmp = nand_get_partnr_off(libmtd, seg_flash_offset);
	if (partnr_tmp < 0) {
		/* find the partition info based on the segment flash offset */
		ret = get_part_info_offset(part_info, nrparts, segment);
		if (ret < 0) {
			error("get part info based on segment flash offset failed\n");
			goto out_free;
		}

		printf("the partition is not found, create it...\n" \
				"partition: name = %s, start = 0x%llx, size = 0x%llx\n",
				part_info[ret].name, __le64_to_cpu(part_info[ret].flash_offset),
				__le64_to_cpu(part_info[ret].length));
		/* create one partition */
		ret = nand_mtd_create_partitions(&part_info[ret], 1);
		if (ret)
			goto out_free;
		partnr_tmp = nand_get_partnr_off(libmtd, seg_flash_offset);
		if (partnr_tmp < 0) {
			printf("get the partition based on segment flash offset failed\n");
			goto out_free;
		}
	}

	printf("the partition mtd%d is found, flash offset = 0x%llx\n",
			partnr_tmp, seg_flash_offset);

	/* the first partition */
	if (partnr_upgrade == -1) {
		partnr_upgrade = partnr_tmp;
		memset(args.node, 0, sizeof(args.node));
		sprintf(args.node, "/dev/mtd%d", partnr_tmp);
		start_eb = 0;
	}

	/*
	 * not the first partition, the second stage: format the remain blocks before scan
	 * the next partition
	 */
	if (partnr_upgrade > 0 && partnr_upgrade != partnr_tmp) {
		printf("format the remain blocks before scan the next partition\n");
		/* format the remain erase blocks */
		ret = format(libmtd, &mtd, &ui, &si, start_eb, 1);
		if (ret)
			goto out_free;

		/* prepare for the next partition */
		partnr_upgrade = partnr_tmp;
		memset(args.node, 0, sizeof(args.node));
		sprintf(args.node, "/dev/mtd%d", partnr_tmp);
		start_eb = 0;

		close(args.node_fd);
		args.node_fd = -1;
	}

	/* a new partition, scan the whole partition */
	if (!start_eb) {
		memset(&ui, 0, sizeof(ui));
		memset(&si, 0, sizeof(si));
		memset(ec, 0, sizeof(ec));
		memset(&mtd, 0, sizeof(mtd));
		ret = ubi_prepare(libmtd, &mtd, &ui, &si, ec);
		if (ret) {
			printf("ubi_prepare failed!!!\n");
			goto out_free;
		}
	}

	print_buf(data, 32);
	printf("\n");
	/*
	 * every partition upgrade include two stages, the first: flash
	 * image, the second: format the ramian blocks if possible. here
	 * is the first stage: flash image only, based on the segment,
	 * leave the remain blocks format to the second stage.
	 */
	start_eb  = flash_image(libmtd, &mtd, &ui, &si, (const char *)data, len, start_eb);
	if (start_eb < 0) {
		printf("flash image failed!!!\n");
		goto out_free;
	}

	if (last) {
		printf("the last partition, format the remain blocks\n");
		/* format last partition remain erase blocks */
		ret = format(libmtd, &mtd, &ui, &si, start_eb, 1);
		if (ret)
			goto out_free;

		close(args.node_fd);
		libmtd_close(libmtd);
		printf("nand upgrade success\n");
	}

	return 0;
out_free:
	if (args.node_fd)
		close(args.node_fd);
	libmtd_close(libmtd);
	return ERR_PROGRAM;
}

static int nand_mtd_upgrade_gpt(struct upgrade_ops *ops,
			struct part_info *parts, int part)
{
	int ret;
	unsigned char *gpt;
	gpt_header *gpt_h;
	unsigned int start, size, gpt_size;
	struct mtd_dev_info mtd;

	ret = mtd_get_mtd_info("/dev/mtd0", &mtd);
	if (ret) {
		error("get mtd info failed\n");
		return ERR_OTHER;
	}

	start = NAND_SPL_PAGES * mtd.min_io_size * NAND_SPL_PARTS;
	size = ops->usr_size - start;
	gpt = gpt_create(size, ops, parts, part);
	if (!gpt) {
		error("error %d\n", __LINE__);
		return ERR_OTHER;
	}

	gpt_size = mtd.eb_size * NAND_PES_PER_GPT;
	if (higher_version == 0) {
		start += gpt_size;
		gpt_h = (gpt_header *)(gpt + FLASH_BLKSZ);
		gpt_prepare_backup_gpt_header(gpt_h);
	}
	if (higher_version == -1) {
		debug("Erase gpt1...\n");
		ret = nand_mtd_erase_common("/dev/mtd0", (start + gpt_size) / mtd.eb_size, gpt_size / mtd.eb_size);
		if (ret)
			return ret;
	}

	debug("Erase and Write gpt%d...\n", higher_version ? 0 : 1);
	ret = nand_mtd_write_common("/dev/mtd0", start / mtd.eb_size, mtd.eb_size * NAND_PES_PER_GPT, gpt, GPT_BUF_SZ);
	if (ret)
		return ret;

	/* only one config partition is ok */
	if (develop) {
		if (higher_version == -1)
			return ERR_SUCCESS;

		start = NAND_SPL_PAGES * mtd.min_io_size * NAND_SPL_PARTS;
		if (higher_version == 1)
			start += gpt_size;

		debug("Erase gpt%d...\n", higher_version ? 1 : 0);
		ret = nand_mtd_erase_common("/dev/mtd0", start / mtd.eb_size, gpt_size / mtd.eb_size);
		if (ret)
			return ret;
	}

	return ERR_SUCCESS;
}

static int mtd_erase_segment(unsigned char *disk, off_t offs, size_t size, size_t erasesize)
{
	int ret = 0, fd;
	size_t part_size;
	unsigned char name[32] = {0};
	erase_info_t info;

	/* Get partition size */
	part_size = mtd_get_disk_size(disk);

	if(offs + size > part_size) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}

	/* Address shoulde be aligned to erase size */
	if(offs & (erasesize - 1)) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}

	sprintf((char *)name, "/dev/%s", disk);

	fd = open((char *)name, O_RDWR | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}

	size = ROUND_UP(size, erasesize);

	debug("Erase segment %s, offset %lx, size %lx...\n",
		disk, (unsigned long)offs, (unsigned long)size);

	info.start  = offs;
	info.length = size;

	ret = ioctl(fd, MEMERASE, &info);
	if(ret < 0) {
		error("erase error\n");
		ret = ERR_ERASE;
	}

	close(fd);
	return ret;
}

static int mtd_erase_partition(unsigned char *disk)
{
	int ret = 0, fd;
	size_t part_size;
	unsigned char dev[32] = {0};
	erase_info_t info;

	/* Get partition size */
	part_size = mtd_get_disk_size(disk);
	if (part_size == 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}

	sprintf((char *)dev, "/dev/%s", disk);

	fd = open((char *)dev, O_RDWR | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}

	debug("Erase mtd part %s, size %x...\n", dev, part_size);

	info.start  = 0;
	info.length = part_size;

	ret = ioctl(fd, MEMERASE, &info);
	if(ret < 0) {
		error("erase error\n");
		goto close_file;
	}

	close(fd);
	return ERR_SUCCESS;

close_file:
	close(fd);
	return ERR_ERASE;
}

static int mtd_write_segment(unsigned char *disk, unsigned char *img, off64_t offs, size_t size)
{
	int fd, ret = 0;
	size_t part_size;
	unsigned char dev[32] = {0};

	/* Get partition size */
	part_size = mtd_get_disk_size(disk);

	if(offs + size > part_size) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_PROGRAM;
	}

	sprintf((char *)dev, "/dev/%s", disk);

	fd = open((char *)dev, O_RDWR | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_PROGRAM;
	}

	ret = lseek64(fd, offs, SEEK_SET);
	if(ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		goto error;
	}

	ret = write(fd, img, size);
	if(ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		goto error;
	}

	close(fd);
	return ERR_SUCCESS;

error:
	return ERR_PROGRAM;
}

static int mtd_write_partition(unsigned char *partition, unsigned char *img, size_t size)
{
	int fd, ret = 0;
	unsigned char dev[32] = {0};

	sprintf((char *)dev, "/dev/%s", partition);

	debug("Write mtd part %s...\n", dev);

	fd = open((char *)dev, O_RDWR);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_PROGRAM;
	}

	ret = write(fd, img, size);
	if(ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		goto error;
	}

	close(fd);
	return ERR_SUCCESS;

error:
	close(fd);
	return ERR_PROGRAM;
}

static int mtd_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	unsigned long long size;
	unsigned long erasesize;

	/* master must be mtd0 */
	strcpy(disk_usr, "mtd0");
	strcpy(disk_rom, "mtd1");
	strcpy(disk_boot0, "mtd2");
	strcpy(disk_boot1, "mtd3");

	size = mtd_get_disk_size(ops->disk_usr);
	if(size == 0) {
		error("error %d, %d\n", __LINE__, errno);
		return ERR_FLASH_TYPE;
	}

	erasesize = mtd_get_erase_size(ops->disk_usr);
	if(erasesize == 0) {
		error("error %d, %d\n", __LINE__, errno);
		return ERR_FLASH_TYPE;
	}

	ops->usr_size = size;
	ops->erasesize = erasesize;
	debug("Mtd flash size %llx, erase size %x...\n", ops->usr_size, ops->erasesize);

	size = 0;
	size += mtd_get_disk_size(ops->disk_rom);
	size += mtd_get_disk_size(ops->disk_boot0) * 2;
	size += 2 * ops->gpt_size;

	if (ops->usr_size - size < partitions_size) {
		printf("\n                           ^*********^                                            \n");
		printf("partitions capacity: 0x%llx, available size  0x%llx (romcode, spl, gpt need 0x%llx)\n" \
				"but the corresponding image partitions total size: 0x%llx\n" \
				"partitions capacity is too small, please check your image!!!\n",
				ops->usr_size, ops->usr_size - size, size, partitions_size);
		printf("\n                           ^*********^                                            \n");
		return ERR_FLASH_TYPE;
	}

	return ERR_SUCCESS;
}

int mtd_upgrade_erase(struct upgrade_ops *ops,
		struct part_info *parts)
{
	int ret;
	off_t offs = 0;

	offs += mtd_get_disk_size(ops->disk_rom);
	offs += mtd_get_disk_size(ops->disk_boot0) * 2;

	debug("upgrade erase partition %s, offset %llx, size %llx\n",
		parts->name,
		(unsigned long long)__le64_to_cpu(parts->flash_offset + offs),
		(unsigned long long)__le64_to_cpu(parts->length));

	ret = mtd_erase_segment(ops->disk_usr, offs +
		__le64_to_cpu(parts->flash_offset),
		__le64_to_cpu(parts->length),
		ops->erasesize);
	if(ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}
	return ERR_SUCCESS;
}

static int mtd_upgrade_rom(struct upgrade_ops *ops,
		unsigned char *rom, size_t size)
{
	int ret;

	debug("Erase mtd romcode %s...\n", ops->disk_rom);
	ret = mtd_erase_partition(ops->disk_rom);
	if (ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_ERASE;
	}

	debug("Write mtd romcode %s...\n", ops->disk_rom);
	ret = mtd_write_partition(ops->disk_rom, rom, size);
	if (ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_PROGRAM;
	}
	return ERR_SUCCESS;
}

static int mtd_upgrade_bootloader(struct upgrade_ops *ops,
		unsigned char *bootloader, size_t size)
{
	int i, ret;

	size_t disk_size;
	unsigned char *disk;

	for(i = 0; i < 2; i++) {
		disk = i == 0 ? ops->disk_boot0 : ops->disk_boot1;

		disk_size = mtd_get_disk_size(disk);
		if(disk_size < size) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			return ERR_PROGRAM;
		}

		debug("Erase mtd bootloader%d...\n", i);

		ret =  mtd_erase_partition(disk);
		if (ret) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			return ERR_ERASE;
		}

		debug("Write mtd bootloader%d...\n", i);

		ret = mtd_write_partition(disk, bootloader, size);
		if (ret < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			return ERR_PROGRAM;
		}
	}

	return ERR_SUCCESS;
}

int mtd_upgrade_segments(struct upgrade_ops *ops,
		struct segment_info *segment, unsigned char *data)
{
	int ret;
	off_t offs = 0;

	debug("Upgrade segment offs %llx, size %llx...\n",
		__le64_to_cpu(segment->flash_offset),
		__le64_to_cpu(segment->size_decompress));

	print_buf(data, 32);
	printf("\n");
	offs += mtd_get_disk_size(ops->disk_rom);
	offs += mtd_get_disk_size(ops->disk_boot0) * 2;

	ret = mtd_write_segment(ops->disk_usr, data, offs +
			__le64_to_cpu(segment->flash_offset),
			__le64_to_cpu(segment->size_decompress));
	if(ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_PROGRAM;
	}
	return ERR_SUCCESS;
}

static int mtd_upgrade_gpt(struct upgrade_ops *ops,
			struct part_info *parts, int part)
{
	int pte_size, ret;
	unsigned char *gpt;
	unsigned long long size;
	gpt_header		*gpt_h;
	gpt_entry		*gpt_e;
	off_t offs = 0;

	ret = do_erase_gpt(ops);
	if (ret) {
		error("erase gpt fisrt failed\n");
		return ERR_OTHER;
	}

	offs += mtd_get_disk_size(ops->disk_rom);
	offs += mtd_get_disk_size(ops->disk_boot0) * 2;
	size = ops->usr_size - offs;

	gpt = gpt_create(size, ops, parts, part);
	if (!gpt) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	gpt_h = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_e = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	pte_size = gpt_h->num_partition_entries * sizeof(gpt_entry);

	if (higher_version == 1 || higher_version == -1) {
		ret = mtd_write_segment(ops->disk_usr, gpt, offs, ops->gpt_size);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}
	}

	if (higher_version == 0) {
		gpt_prepare_backup_gpt_header(gpt_h);

		offs = ops->usr_size - FLASH_BLKSZ - pte_size;

		/* backup gpt entry */
		ret = mtd_write_segment(ops->disk_usr, (unsigned char *)gpt_e, offs, pte_size);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}

		/* backup gpt header */
		ret = mtd_write_segment(ops->disk_usr, (unsigned char *)gpt_h,
				ops->usr_size - FLASH_BLKSZ, FLASH_BLKSZ);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}
	}

	if (develop) {
		ret = develop_erase(ops);
		if (ret)
			goto error_exit;
	}

	/* FIXME: double free */
	//free(gpt);
	return ERR_SUCCESS;

error_exit:
	free(gpt);
	return ERR_PROGRAM;
}

static unsigned long long mmc_get_disk_size(unsigned char *partition)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	unsigned long long size;
	int ret, fd;

	sprintf((char *)path, "/sys/block/%s/size", partition);

	fd = open((char *)path, O_RDONLY);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return 0;
	}

	ret = read(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
		goto err_read;
	}

	sscanf((char *)buff, "%llu", &size);
	close(fd);
	return size;

err_read:
	close(fd);
	return 0;
}

static int mmc_set_disk_ro(unsigned char *disk)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	int ret = 0, fd;

	sprintf((char *)path, "/sys/block/%s/force_ro", disk);

	fd = open((char *)path, O_RDWR);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return -1;
	}

	sprintf((char *)buff, "%llu", 1LLU);

	ret = write(fd, buff, 32);
	if(ret < 0) {
		error("Error %d\n", __LINE__);
	}

	close(fd);
	return ret;
}

static int mmc_set_disk_rw(unsigned char *disk)
{
	unsigned char path[128] = {0};
	unsigned char buff[32] = {0};
	int ret = 0, fd;

	sprintf((char *)path, "/sys/block/%s/force_ro", disk);

	fd = open((char *)path, O_RDWR);
	if(fd < 0) {
		error("Error %d\n", __LINE__);
		return -1;
	}

	sprintf((char *)buff, "%llu", 0LLU);

	ret = write(fd, buff, 32);
	if(ret < 0) {
		error("Error %d, %d, %d\n", __LINE__, ret, errno);
		return -1;
	}

	close(fd);
	return 0;
}

static int mmc_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	int ret, n;
	unsigned long long size;

	ret = access("/dev/mmcblk1boot0", F_OK);
	if (!ret)
		n = 1;
	else {
		ret = access("/dev/mmcblk0boot0", F_OK);
		if (!ret)
			n = 0;
		else
			n = -1;
	}

	if (n == -1) {
		error("\n                           ^*********^                     \n");
		error("mmc device is not found, please check your image or device!!!\n");
		error("\n                           ^*********^                     \n");
		return ERR_FLASH_TYPE;
	}

	sprintf(disk_usr, "mmcblk%d", n);
	sprintf(disk_boot0, "mmcblk%dboot0", n);
	sprintf(disk_boot1, "mmcblk%dboot1", n);

	size = mmc_get_disk_size(ops->disk_usr);
	if(size == 0) {
		error("error %d, %d\n", __LINE__, errno);
		return ERR_FLASH_TYPE;
	}

	ops->usr_size = size * FLASH_BLKSZ;
	debug("mmc disk size %llx\n", (unsigned long long)ops->usr_size);

	size = 2 * ops->gpt_size;
	if (ops->usr_size - size < partitions_size) {
		printf("\n                           ^*********^                              \n");
		printf("partitions0 capacity: 0x%llx, available size  0x%llx (gpt need 0x%llx)\n" \
				"but the corresponding image partitions total size: 0x%llx\n" \
				"partitions0 capacity is too small, please check your image!!!\n",
				ops->usr_size, ops->usr_size - size, size, partitions_size);
		printf("\n                           ^*********^                              \n");
		return ERR_FLASH_TYPE;
	}
	return ERR_SUCCESS;
}

static int mmc_upgrade_erase(struct upgrade_ops *ops, struct part_info *parts)
{
	int ret, fd, len;
	off64_t offs;
	unsigned long long length;
	unsigned int start, size;
	struct mmc_ioc_multi_cmd *mioc;
	struct mmc_ioc_cmd *ioc;
	char dev[128] = {0};

	debug("upgrade erase partition %s, offset %llx, size %llx\n",
		parts->name,
		(unsigned long long)__le64_to_cpu(parts->flash_offset),
		(unsigned long long)__le64_to_cpu(parts->length));

	offs = __le64_to_cpu(parts->flash_offset);
	length = __le64_to_cpu(parts->length);
	start = offs / FLASH_BLKSZ;
	size = length / FLASH_BLKSZ;

	sprintf(dev, "/dev/%s", ops->disk_usr);

	fd = open(dev, O_WRONLY | O_SYNC);
	if(fd < 0) {
		error("error %d, %d\n", __LINE__, errno);
		return ERR_ERASE;
	}

	len = sizeof(struct mmc_ioc_multi_cmd) + 3 * sizeof(struct mmc_ioc_cmd);
	mioc = malloc(len);
	if (!mioc) {
		error("out of memory\n");
		return ERR_ERASE;
	}
	memset(mioc, 0, len);

	mioc->num_of_cmds = 3;

	ioc = &mioc->cmds[0];
	ioc->opcode = MMC_ERASE_GROUP_START;
	ioc->arg = start;
	ioc->blksz = FLASH_BLKSZ;
	ioc->flags = MMC_RSP_R1 | MMC_CMD_AC;

	ioc = &mioc->cmds[1];
	ioc->opcode = MMC_ERASE_GROUP_END;
	ioc->arg = start + size -1;
	ioc->blksz = FLASH_BLKSZ;
	ioc->flags = MMC_RSP_R1 | MMC_CMD_AC;

	ioc = &mioc->cmds[2];
	ioc->opcode = MMC_ERASE;
	ioc->arg = 0x80000000; /* 0x00000001 is ok too */
	ioc->blksz = FLASH_BLKSZ;
	ioc->flags = MMC_RSP_R1B | MMC_CMD_AC;

	ret = ioctl(fd, MMC_IOC_MULTI_CMD, mioc);
	if (ret < 0) {
		printf("ioctl error, ret = %d, errno = %d\n", ret, errno);
		close(fd);
		free(mioc);
		return ERR_ERASE;
	}

	close(fd);
	free(mioc);

	return ERR_SUCCESS;
}

static int mmc_upgrade_bootloader(struct upgrade_ops *ops,
			unsigned char *rom, size_t size)
{
	int i, fd, ret;
	unsigned char dev[128] = {0};
	unsigned char *disk;

	vsystem("sync");
	vsystem("echo 3 > /proc/sys/vm/drop_caches");

	for(i = 0; i < 2; i++) {
		disk = i == 0 ? ops->disk_boot0 : ops->disk_boot1;

		sprintf((char *)dev, "/dev/%s", disk);

		ret = mmc_set_disk_rw(disk);
		if(ret < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			return ERR_PROGRAM;
		}

		fd = open((char *)dev, O_WRONLY | O_SYNC);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			return ERR_PROGRAM;
		}

		ret = write(fd, rom, size);
		if(ret < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto err_write;
		}

		ret = mmc_set_disk_ro(disk);
		if(ret < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			return ERR_PROGRAM;
		}

		close(fd);
	}

	return ERR_SUCCESS;

err_write:
	close(fd);
	mmc_set_disk_ro(disk);
	return ERR_PROGRAM;
}

static int mmc_upgrade_segments(struct upgrade_ops *ops,
		struct segment_info *segment, unsigned char *data)
{
	int ret = 0, fd;
	off64_t offs, offs_seek;
	size_t size;
	char dev[128] = {0};

	vsystem("sync");
	vsystem("echo 3 > /proc/sys/vm/drop_caches");

	sprintf(dev, "/dev/%s", ops->disk_usr);

	fd = open(dev, O_WRONLY | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_PROGRAM;
	}

	offs = __le64_to_cpu(segment->flash_offset);
	size = __le64_to_cpu(segment->size_decompress);

	offs_seek = lseek64(fd, offs, SEEK_SET);
	if(offs != offs_seek) {
		error("error %d, %#llx, %d\n", __LINE__, offs_seek, errno);
		goto error_exit;
	}

	ret = write(fd, data, size);
	if(ret < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		goto error_exit;
	}

	close(fd);

	return ERR_SUCCESS;

error_exit:
	close(fd);
	return ERR_PROGRAM;
}

int mmc_upgrade_gpt(struct upgrade_ops *ops,
			struct part_info *parts, int part)
{
	int fd, pte_size, ret = 0;
	unsigned char *gpt;
	char dev[128] = {0};
	gpt_header		*gpt_h;
	gpt_entry		*gpt_e;
	unsigned long long offs, offs_seek;

	ret = do_erase_gpt(ops);
	if (ret) {
		error("erase gpt fisrt failed\n");
		return ERR_OTHER;
	}

	gpt = gpt_create(ops->usr_size, ops, parts, part);
	if(!gpt) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	sprintf(dev, "/dev/%s", ops->disk_usr);

	gpt_h = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_e = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	pte_size = gpt_h->num_partition_entries * sizeof(gpt_entry);

	fd = open(dev, O_WRONLY | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		goto error_open;
	}

	if (higher_version == 1 || higher_version == -1) {
		/* No need to lseek */
		ret = write(fd, gpt, ops->gpt_size);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}
	}

	if (higher_version == 0) {
		gpt_prepare_backup_gpt_header(gpt_h);

		offs = ops->usr_size - FLASH_BLKSZ - pte_size;

		offs_seek = lseek64(fd, offs, SEEK_SET);
		if(offs != offs_seek) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}

		/* backup gpt entry */
		ret = write(fd, (char *)gpt_e, pte_size);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}

		offs = ops->usr_size - FLASH_BLKSZ;
		offs_seek = lseek64(fd, offs, SEEK_SET);
		if(offs != offs_seek) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}

		/* backup gpt header */
		ret = write(fd, (char *)gpt_h, FLASH_BLKSZ);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}
	}

	if (develop) {
		ret = develop_erase(ops);
		if (ret)
			goto error_write;
	}

	close(fd);
	free(gpt);
	return ERR_SUCCESS;

error_write:
	close(fd);
error_open:
	free(gpt);
	return ERR_PROGRAM;
}

static int gpt_read(struct upgrade_ops *ops, unsigned char *buf, size_t size, int which)
{
	int fd, ret = 0;
	unsigned char dev[32] = {0};
	off64_t offs = 0, offs_seek;
	struct mtd_dev_info mtd;

	/* gpt0 */
	if (which == 0) {
		/* nor or mmc? */
		if (strcmp((char *)ops->type, "nor") == 0) {
			offs += mtd_get_disk_size(ops->disk_rom);
			offs += mtd_get_disk_size(ops->disk_boot0) * 2;
		} else if (strcmp((char *)ops->type, "mmc") == 0) {
			offs = 0;
		} else if (strcmp((char *)ops->type, "nand") == 0) {
			ret = mtd_get_mtd_info("/dev/mtd0", &mtd);
			if (ret) {
				error("get mtd info failed\n");
				return ERR_OTHER;
			}
			offs = NAND_SPL_PAGES * mtd.min_io_size * NAND_SPL_PARTS;
		} else
			return ERR_FLASH_TYPE;
	} else if (which == 1) { /* gpt1 */
		if (strcmp((char *)ops->type, "nor") == 0 || strcmp((char *)ops->type, "mmc") == 0) {
			offs = ops->usr_size - FLASH_BLKSZ;
		} else if (strcmp((char *)ops->type, "nand") == 0) {
			ret = mtd_get_mtd_info("/dev/mtd0", &mtd);
			if (ret) {
				error("get mtd info failed\n");
				return ERR_OTHER;
			}
			offs = NAND_SPL_PAGES * mtd.min_io_size * NAND_SPL_PARTS + mtd.eb_size * NAND_PES_PER_GPT;
		} else
			return ERR_FLASH_TYPE;
	} else
		return ERR_OTHER;

	sprintf((char *)dev, "/dev/%s", ops->disk_usr);

	fd = open((char *)dev, O_RDWR | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return ERR_OTHER;
	}

	if (strcmp((char *)ops->type, "nand") == 0) {
		ret = nand_mtd_read_common(fd, &mtd, offs / mtd.eb_size, NAND_PES_PER_GPT, buf, size);
		if (ret) {
			close(fd);
			return ERR_OTHER;
		}
	} else {
		offs_seek = lseek64(fd, offs, SEEK_SET);
		if (offs != offs_seek) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			close(fd);
			return ERR_OTHER;
		}

		if (which == 0)
			ret = read(fd, buf, size);
		else
			ret = read(fd, buf + FLASH_BLKSZ, FLASH_BLKSZ); /* gpt1 header, no mbr */
		if (ret < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			close(fd);
			return ERR_OTHER;
		}

		/* gpt1 entry */
		if (which == 1) {
			ret = lseek64(fd, ops->usr_size - 33 * FLASH_BLKSZ, SEEK_SET);
			if (offs != offs_seek) {
				error("error %d, %d, %d\n", __LINE__, ret, errno);
				close(fd);
				return ERR_OTHER;
			}

			ret = read(fd, buf + 2 * FLASH_BLKSZ, 32 * FLASH_BLKSZ);
			if (ret < 0) {
				error("error %d, %d, %d\n", __LINE__, ret, errno);
				close(fd);
				return ERR_OTHER;
			}
		}
	}

	close(fd);
	return ERR_SUCCESS;
}

static int gpt_erase(struct upgrade_ops *ops, int which)
{
	int ret, mtd = 0;
	off64_t offs = 0;
	struct part_info part = {0};

	/* nor or mmc? */
	if (strncmp((char *)ops->disk_usr, "mtd", 3) == 0) {
		mtd = 1;
	} else if (strncmp((char *)ops->disk_usr, "mmcblk", 6) == 0)
		offs = 0;
	else
		return ERR_FLASH_TYPE;

	/* gpt0 */
	if (which == 0) {
		if (mtd) {
			offs += mtd_get_disk_size(ops->disk_rom);
			offs += mtd_get_disk_size(ops->disk_boot0) * 2;
		} else {
			part.flash_offset = 0;
			part.length = __cpu_to_le64(ops->gpt_size);
		}
	} else if (which == 1) { 	/* gpt1 */
		if (mtd)
			offs = ops->usr_size - ops->gpt_size;
		else {
			part.flash_offset = __cpu_to_le64(ops->usr_size - ops->gpt_size);
			part.length = __cpu_to_le64(ops->gpt_size);
		}
	} else
		return ERR_OTHER;

	if (mtd) {
		ret = mtd_erase_segment(ops->disk_usr, offs,
				ops->gpt_size, ops->erasesize);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	} else {
		ret = mmc_upgrade_erase(ops, &part);
		if (ret) {
			error("mmc erase gpt failed\n");
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static int develop_erase(struct upgrade_ops *ops)
{
	int ret;

	if (higher_version == 0) {
		debug("Erase gpt0...\n");
		ret = gpt_erase(ops, 0);
		if (ret)
			return ERR_ERASE;
	}

	if (higher_version == 1) {
		debug("Erase gpt1...\n");
		ret = gpt_erase(ops, 1);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static struct upgrade_ops nor_upgrade_ops = {
	.type = (unsigned char *)"nor",
	.disk_rom   = (unsigned char *)disk_rom,
	.disk_boot0 = (unsigned char *)disk_boot0,
	.disk_boot1 = (unsigned char *)disk_boot1,
	.disk_usr  = (unsigned char *)disk_usr,
	.usr_size  = 0,
	.gpt_size   = 0x80 * FLASH_BLKSZ,/* 64KByte */

	.upgrade_init       = mtd_upgrade_init,
	.upgrade_erase      = mtd_upgrade_erase,
	.upgrade_rom        = mtd_upgrade_rom,
	.upgrade_bootloader = mtd_upgrade_bootloader,
	.upgrade_segments   = mtd_upgrade_segments,
	.upgrade_gpt     = mtd_upgrade_gpt,
	.upgrade_callback   = NULL,
};

static struct upgrade_ops nand_upgrade_ops = {
	.type = (unsigned char *)"nand",
	.disk_rom   = NULL,
	.disk_boot0 = (unsigned char *)disk_boot0,
	.disk_boot1 = (unsigned char *)disk_boot1,
	.disk_usr  = (unsigned char *)disk_usr,
	.usr_size  = 0,
	.gpt_size   = 0x100 * FLASH_BLKSZ,/* 128KByte */

	.upgrade_init       = nand_mtd_upgrade_init,
	.upgrade_erase      = NULL,
	.upgrade_rom        = NULL,
	.upgrade_bootloader = nand_mtd_upgrade_bootloader,
	.upgrade_segments_nand  = nand_mtd_upgrade_segments,
	.upgrade_gpt       = nand_mtd_upgrade_gpt,
	.upgrade_callback   = NULL,
};

static struct upgrade_ops mmc_upgrade_ops = {
	.type = (unsigned char *)"mmc",
	.disk_rom   = NULL,
	.disk_boot0 = (unsigned char *)disk_boot0,
	.disk_boot1 = (unsigned char *)disk_boot1,
	.disk_usr  = (unsigned char *)disk_usr,
	.usr_size  = 0,
	.gpt_size   = 0x400 * FLASH_BLKSZ, /* 512KByte */

	.upgrade_init       = mmc_upgrade_init,
	.upgrade_erase      = mmc_upgrade_erase,
	.upgrade_rom        = NULL,
	.upgrade_bootloader = mmc_upgrade_bootloader,
	.upgrade_segments   = mmc_upgrade_segments,
	.upgrade_gpt     = mmc_upgrade_gpt,
	.upgrade_callback   = NULL,
};

static int current_gpt_check(struct upgrade_ops *ops, int *gpt0_ok, int *gpt1_ok, unsigned char **gpt0, unsigned char **gpt1)
{
	int ret = 0;
	gpt_header		*p_gpt0, *p_gpt1;
	gpt_entry		*p_entry0, *p_entry1;

	*gpt0 = malloc(GPT_BUF_SZ);
	if (!*gpt0) {
		debug("alloc gpt size = %d, failed\n", GPT_BUF_SZ);
		return -1;
	}

	*gpt1 = malloc(GPT_BUF_SZ);
	if (!*gpt1) {
		free(*gpt0);
		debug("alloc gpt size = %d, failed\n", GPT_BUF_SZ);
		return -1;
	}

	memset(*gpt0, 0, GPT_BUF_SZ);
	memset(*gpt1, 0, GPT_BUF_SZ);

	ret = gpt_read(ops, *gpt0, GPT_BUF_SZ, 0);
	if (ret < 0) {
		error("read gpt failed\n");
		goto out;
	}

	ret = gpt_read(ops, *gpt1, GPT_BUF_SZ, 1);
	if (ret < 0) {
		error("read gpt failed\n");
		goto out;
	}

	p_gpt0   = (gpt_header *)(*gpt0 + FLASH_BLKSZ);
	p_entry0 = (gpt_entry  *)(*gpt0 + FLASH_BLKSZ * 2);

	/*Validate GPT0 header and entry */
	if (!gpt_validate_header(p_gpt0, GPT_PRIMARY_PARTITION_TABLE_LBA, __le64_to_cpu(p_gpt0->last_usable_lba))
			&& !gpt_validate_entries(p_gpt0, p_entry0)) {
		*gpt0_ok = 1;
	}

	p_gpt1   = (gpt_header *)(*gpt1 + FLASH_BLKSZ);
	p_entry1 = (gpt_entry  *)(*gpt1 + FLASH_BLKSZ * 2);

	/*Validate GPT1 header and entry */
	if (!gpt_validate_header(p_gpt1, __le64_to_cpu(p_gpt1->last_usable_lba) + 33, __le64_to_cpu(p_gpt1->last_usable_lba))
			&& !gpt_validate_entries(p_gpt1, p_entry1)) {
		*gpt1_ok = 1;
	}

	return 0;
out:
	free(*gpt0);
	free(*gpt1);

	return ret;
}

static bool do_upgrade_sanity_check(struct upgrade_ops *ops, struct upgrade_hdr *hdr, struct part_info *part_tbl)
{
	int i, ret;
	unsigned char	*gpt0, *gpt1;
	gpt_header		*p_gpt0, *p_gpt1;
	gpt_entry		*p_entry;
	int gpt0_ok = 0, gpt1_ok = 0;
	bool upgrade = false;
	int parts;
	unsigned int version;
	uint64_t new_offset;
	bool upgrade_all = true;
	bool partinfo_changed = false;

	ret = current_gpt_check(ops, &gpt0_ok, &gpt1_ok, &gpt0, &gpt1);
	if (ret < 0) {
		error("gpt check failed\n");
		return false;
	}

	p_gpt0   = (gpt_header *)(gpt0 + FLASH_BLKSZ);
	p_gpt1   = (gpt_header *)(gpt1 + FLASH_BLKSZ);

	/* both ok */
	if (gpt0_ok && gpt1_ok) {
		if (__le32_to_cpu(p_gpt0->reserved1) >= __le32_to_cpu(p_gpt1->reserved1)) {
			/* depend_version is only exsit in partial upgrade */
			if (__le32_to_cpu(hdr->depend_version)
					&& __le32_to_cpu(hdr->depend_version) != __le32_to_cpu(p_gpt0->reserved1)
					&& !develop) {
				printf("\n*********************************ERROR**********************************\n\n");
				printf("partial upgrade depend version must equal to current higher version,\n" \
						"depend version: %u\n" \
						"current higher version: %u\n",
						__le32_to_cpu(hdr->depend_version),
						__le32_to_cpu(p_gpt0->reserved1));
				printf("\n************************************************************************\n");

				goto out;
			}

			higher_version = 0;
		} else {
			if (__le32_to_cpu(hdr->depend_version)
					&& __le32_to_cpu(hdr->depend_version) != __le32_to_cpu(p_gpt1->reserved1)
					&& !develop) {
				printf("\n*********************************ERROR**********************************\n\n");
				printf("partial upgrade depend version must equal to current higher version,\n" \
						"depend version: %u\n" \
						"current higher version: %u\n",
						__le32_to_cpu(hdr->depend_version),
						__le32_to_cpu(p_gpt1->reserved1));
				printf("\n************************************************************************\n");

				goto out;
			}

			higher_version = 1;
		}
	} else if (gpt0_ok) {
		if (__le32_to_cpu(hdr->depend_version)
				&& __le32_to_cpu(hdr->depend_version) != __le32_to_cpu(p_gpt0->reserved1)
				&& !develop) {
			printf("\n*********************************ERROR**********************************\n\n");
			printf("partial upgrade depend version must equal to current higher version,\n" \
					"depend version: %u\n" \
					"current higher version: %u\n",
					__le32_to_cpu(hdr->depend_version),
					__le32_to_cpu(p_gpt0->reserved1));
			printf("\n************************************************************************\n");

			goto out;
		}

		higher_version = 0;
	} else if (gpt1_ok) {
		if (__le32_to_cpu(hdr->depend_version)
				&& __le32_to_cpu(hdr->depend_version) != __le32_to_cpu(p_gpt1->reserved1)
				&& !develop) {
			printf("\n*********************************ERROR**********************************\n\n");
			printf("partial upgrade depend version must equal to current higher version,\n" \
					"depend version: %u\n" \
					"current higher version: %u\n",
					__le32_to_cpu(hdr->depend_version),
					__le32_to_cpu(p_gpt1->reserved1));
			printf("\n************************************************************************\n");

			goto out;
		}

		higher_version = 1;
	} else
		higher_version = -1; /* upgrade gpt0 */

	if (higher_version == 0)
		version = __le32_to_cpu(p_gpt0->reserved1);
	else if (higher_version == 1)
		version = __le32_to_cpu(p_gpt1->reserved1);
	else
		version = 0;

	/* compare current version and object version */
	if (version && __le32_to_cpu(hdr->object_version) <= version && !develop) {
		printf("\n*********************************ERROR**********************************\n\n");
		printf("upgrade object version (%#x) <= current version(%#x)\n",
				__le32_to_cpu(hdr->object_version), version);
		printf("\n************************************************************************\n");
		goto out;
	}

	parts = hdr->partitions;

	/* single partitions exist? */
	for (i = 0; i < parts; i++) {
		if (!(hdr->part_flag[i] & DUAL_PART_FLAG_MASK) && part_tbl[i].is_upgrade) {
			printf("\n*********************************WARNNING*******************************\n\n");
			printf("single partitions exsit and will be upgraded, you must not power off while upgrade,\n" \
					"otherwise maybe upgrade fail\n");
			printf("\n************************************************************************\n");
			break;
		}
	}

	if (higher_version == 0)
		p_entry = (gpt_entry *)(gpt0 + FLASH_BLKSZ * 2);
	else if (higher_version == 1)
		p_entry = (gpt_entry *)(gpt1 + FLASH_BLKSZ * 2);
	else {
		/* upgrade gpt0 */
		upgrade = true;
		goto out; /* do not check whether partitions change */
	}
	/* check whether partitions info changed or not */
	for (i = 0; i < parts; i++) {
		int efiname_len, dosname_len, j;
		char name[32];

		/* for backwards compatibility, partitions valid information is 0 while upgrade
		 * use old upgrade program that header version < 4
		 */
		if ((higher_version == 0 && __le32_to_cpu(p_gpt0->reserved1) == 0)
				|| (higher_version == 1 && __le32_to_cpu(p_gpt1->reserved1) == 0)) {
				if (!(hdr->part_flag[i] & DUAL_PART_FLAG_MASK)
						|| (hdr->part_flag[i] & DUAL_PART_FLAG_MASK && part_tbl[i].name[strlen((char *)part_tbl[i].name) - 1] == '0'))
					higher_valid[i] = 1;
		} else
			higher_valid[i] = (char)((p_entry[i].attributes.fields.reserved & PARTITION_VALID_MASK) >> PARTITION_VALID_SHIFT);

		/* upgrade partially */
		if (!part_tbl[i].is_upgrade
				&& (!(hdr->part_flag[i] & DUAL_PART_FLAG_MASK) || (hdr->part_flag[i] & DUAL_PART_FLAG_MASK && part_tbl[i].name[strlen((char *)part_tbl[i].name) - 1] == '0'))) {
			upgrade_all = false;
		}
		/* partition name */
		efiname_len = sizeof(p_entry[i].partition_name) / sizeof(efi_char16_t);
		dosname_len = sizeof(part_tbl[i].name);

		memset(name, 0, sizeof(name));
		for (j = 0; j < min(dosname_len, efiname_len); j++)
			name[j] = (char)p_entry[i].partition_name[j];

		/* partition name is different */
		if (!partinfo_changed && strcmp(name, (char *)part_tbl[i].name)) {
			partinfo_changed = true;
			printf("\n*********************************WARNNING*******************************\n\n");
			debug("the image and gpt partitions information is changed\n");
			debug("the first different partition information(%s):\n" \
					"part_tbl[%d], size         = 	0x%llx, flash_offset = 	0x%llx\n" \
					" p_entry[%d], starting_lba = 	0x%llx, ending_lba   = 	0x%llx\n",
					part_tbl[i].name, i, part_tbl[i].length, part_tbl[i].flash_offset,
					i, p_entry[i].starting_lba, p_entry[i].ending_lba);
			printf("\n************************************************************************\n");
		}

		new_offset = part_tbl[i].flash_offset;
		/* partition flash_offset or size is different */
		if (!partinfo_changed && (p_entry[i].starting_lba * 512 != new_offset
				|| (p_entry[i].ending_lba + 1) * 512 != new_offset + part_tbl[i].length)) {
			partinfo_changed = true;
			printf("\n*********************************WARNNING*******************************\n\n");
			debug("the image and gpt partitions information is changed\n");
			debug("the first different partition information(%s):\n" \
					"part_tbl[%d], size         = 	0x%llx, flash_offset = 	0x%llx\n" \
					" p_entry[%d], starting_lba = 	0x%llx, ending_lba   = 	0x%llx\n",
					part_tbl[i].name, i, part_tbl[i].length, part_tbl[i].flash_offset,
					i, p_entry[i].starting_lba, p_entry[i].ending_lba);
			printf("\n************************************************************************\n");
		}
	}

	/* partitions infomation is changed */
	if (partinfo_changed) {
		printf("\n*********************************WARNNING*******************************\n\n");
		if (upgrade_all)
			debug("all partitions will be upgraded\n");
		else
			debug("upgrade partially, you should upgrade all partitions," \
					"please make sure you known what are you doing!!!\n"); /* sometime is ok, eg: factory */

		debug("you must not power off while upgrade, otherwise at your own risk!!!\n");
		printf("\n************************************************************************\n");

		/* upgrade gpt0 if partition info changed */
		higher_version = -1;
		memset(higher_valid, 0, sizeof(higher_valid));
	}
	/* check partitions valid rule: only one partition is valid if this
	 * partition is dual part
	 */
	if (higher_version != -1) {
		for (i = 0; i < parts; i++) {
			if (hdr->part_flag[i] & DUAL_PART_FLAG_MASK) {
				if ((higher_valid[i] && higher_valid[i + 1])
						|| (!higher_valid[i] && !higher_valid[i + 1])) {
					printf("check partitions valid rule failed\n" \
							"partition: %s\n", part_tbl[i].name);
					print_buf((unsigned char *)higher_valid, parts);
					goto out;
				} else
					i++;
			} else {
				if (!higher_valid[i]) {
					printf("check partitions valid rule failed\n" \
							"partition: %s\n", part_tbl[i].name);
					print_buf((unsigned char *)higher_valid, parts);
					goto out;
				}
			}
		}

		printf("\ngpt0 is %s, gpt1 is %s\n", gpt0_ok ? "ok" : "error", gpt1_ok ? "ok" : "error");
		printf("\nupgrade gpt%d, ", higher_version ? 0 : 1);
		if (gpt0_ok)
			printf("gpt0 version: %u ", __le32_to_cpu(p_gpt0->reserved1));
		if (gpt1_ok)
			printf("gpt1 version: %u ", __le32_to_cpu(p_gpt1->reserved1));
		printf("\nobject version: %u, depend version: %u\n\n",
				__le32_to_cpu(hdr->object_version),
				__le32_to_cpu(hdr->depend_version));

		print_buf((unsigned char *)higher_valid, parts);
	}

	upgrade = true;
out:
	free(gpt0);
	free(gpt1);
	return upgrade;
}

static int do_erase_gpt(struct upgrade_ops *ops)
{
	int ret;

	if (higher_version == 1 || higher_version == -1) {
		debug("Erase gpt0...\n");
		ret = gpt_erase(ops, 0);
		if (ret)
			return ERR_ERASE;
	}

	if (higher_version == 0 || higher_version == -1) {
		debug("Erase gpt1...\n");
		ret = gpt_erase(ops, 1);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static int do_upgrade_report(struct upgrade_ops *ops, size_t size)
{
	int percent;

	if(!ops->upgrade_callback)
		return ERR_SUCCESS;

	ops->writtensize += size;

	percent = ops->writtensize * 100 / ops->totalsize;
	if(percent > 100)
		percent = 100;

	return ops->upgrade_callback(STAT_IN_PROGRESS, percent);
}

static int do_verify_image(unsigned char *img, ar_flash_cb cb)
{
	unsigned char *hash, *data, *sig;
	int ret = 0;
	struct upgrade_hdr *hdr;
	unsigned char *hdr_ext;
	unsigned char *romcode;
	unsigned char *bootloader;
	struct part_info *partitions;
	struct segment_info * segments;
	int i, m, n;
	int index_tmp, index;
	unsigned long long img_size;

	debug("Verify upgrade image...\n");

	vsystem("sync");
	vsystem("echo 3 > /proc/sys/vm/drop_caches");
	/* Verify header */
	hdr = (struct upgrade_hdr *)img;

	if (__le32_to_cpu(hdr->magic) != MAGIC ||
		__le32_to_cpu(hdr->hash_size) != 32 ||
		__le32_to_cpu(hdr->sig_size) != 256) {
		error("error %d\n", __LINE__);
		return -1;
	}

	hash = img + sizeof(*hdr);
	sig  = hash + 32;
	data = sig + 256;

	ret = rsa_verify_image(data, hdr, hash, sig, cb);
	if (ret < 0) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}

	hdr         = (struct upgrade_hdr *)img;
	hdr_ext     = (unsigned char *)img + sizeof(*hdr);
	romcode     = hdr_ext + 32 + 256 + __le16_to_cpu(hdr->header_ext_size);
	bootloader  = romcode + __le32_to_cpu(hdr->rom_size);
	partitions  = (struct part_info *)(bootloader + __le32_to_cpu(hdr->loader_size));
	segments    = (struct segment_info *)(partitions + hdr->partitions);

	/* FIXME */
#define NOR_ROMCODE_SIZE 	(0x10000)
#define SPL_SIZE (0x40000)

	if (__le32_to_cpu(hdr->rom_size) && __le32_to_cpu(hdr->rom_size) > NOR_ROMCODE_SIZE) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}

	if (__le32_to_cpu(hdr->loader_size) && __le32_to_cpu(hdr->loader_size) > SPL_SIZE) {
		if (hdr->flashtype == FLASH_TYPE_MMC || hdr->flashtype == FLASH_TYPE_SPINOR) {
			debug("error %d\n", __LINE__);
			return ERR_VERIFY;
		}
	}

	m = __le16_to_cpu(hdr->partitions);
	n = __le16_to_cpu(hdr->segments);

	index = -1;
	index_tmp = -1;
	img_size = 0;
	for (i = 0; i < n; i++) {
		index_tmp = get_part_info_offset(partitions, m, &segments[i]);
		if (index_tmp < 0) {
			error("get part info based on segment flash offset failed\n");
			return ERR_VERIFY;
		}

		/* the first partition */
		if (index == -1) {
			index = index_tmp;
		}

		/* the next partition */
		if (index != index_tmp) {
			if (img_size > __le64_to_cpu(partitions[index].length)) {
				printf("image is too big, partition name: %s\n", partitions[index].name);
				return ERR_VERIFY;
			}
			index = index_tmp;
			img_size = 0;
		}

		img_size += __le64_to_cpu(segments[i].size_decompress);
	}

	/* the last partition */
	if (img_size > __le64_to_cpu(partitions[index].length)) {
		printf("image is too big, partition name: %s\n", partitions[index].name);
		return ERR_VERIFY;
	}

	vsystem("sync");
	vsystem("echo 3 > /proc/sys/vm/drop_caches");

	debug("Verify upgrade image success\n");

	return ERR_SUCCESS;
}

static int do_verify_image_ex(unsigned char *img, ar_flash_cb cb, int fd)
{
	unsigned char *hash, *data, *sig;
	int ret = 0;
	struct upgrade_hdr *hdr;
	unsigned char *hdr_ext;
	unsigned char *romcode;
	unsigned char *bootloader;
	struct part_info *partitions;
	struct segment_info * segments;
	int i, m, n;
	int index_tmp, index;
	unsigned long long img_size;

	debug("Verify upgrade image...\n");

	vsystem("sync");
	vsystem("echo 3 > /proc/sys/vm/drop_caches");

	/* Verify header */
	hdr = (struct upgrade_hdr *)img;

	if (__le32_to_cpu(hdr->magic) != MAGIC ||
		__le32_to_cpu(hdr->hash_size) != 32 ||
		__le32_to_cpu(hdr->sig_size) != 256) {
		error("error %d\n", __LINE__);
		return -1;
	}

	hash = img + sizeof(*hdr);
	sig  = hash + 32;
	data = sig + 256;

	if (single_upgrade)
		ret = rsa_verify_image(data, hdr, hash, sig, cb);
	else
		ret = rsa_verify_image_ex(data, hdr, hash, sig, cb, fd);
	if (ret < 0) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}

	hdr         = (struct upgrade_hdr *)img;
	hdr_ext     = (unsigned char *)img + sizeof(*hdr);
	romcode     = hdr_ext + 32 + 256 + __le16_to_cpu(hdr->header_ext_size);
	bootloader  = romcode + __le32_to_cpu(hdr->rom_size);
	partitions  = (struct part_info *)(bootloader + __le32_to_cpu(hdr->loader_size));
	segments    = (struct segment_info *)(partitions + hdr->partitions);

	/* FIXME */
#define NOR_ROMCODE_SIZE 	(0x10000)
#define SPL_SIZE (0x40000)

	if (__le32_to_cpu(hdr->rom_size) && __le32_to_cpu(hdr->rom_size) > NOR_ROMCODE_SIZE) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}

	if (__le32_to_cpu(hdr->loader_size) && __le32_to_cpu(hdr->loader_size) > SPL_SIZE) {
		if (hdr->flashtype == FLASH_TYPE_MMC || hdr->flashtype == FLASH_TYPE_SPINOR) {
			debug("error %d\n", __LINE__);
			return ERR_VERIFY;
		}
	}

	m = __le16_to_cpu(hdr->partitions);
	n = __le16_to_cpu(hdr->segments);

	index = -1;
	index_tmp = -1;
	img_size = 0;
	for (i = 0; i < n; i++) {
		index_tmp = get_part_info_offset(partitions, m, &segments[i]);
		if (index_tmp < 0) {
			error("get part info based on segment flash offset failed\n");
			return ERR_VERIFY;
		}

		/* the first partition */
		if (index == -1) {
			index = index_tmp;
		}

		/* the next partition */
		if (index != index_tmp) {
			if (img_size > __le64_to_cpu(partitions[index].length)) {
				printf("image is too big, partition name: %s\n", partitions[index].name);
				return ERR_VERIFY;
			}
			index = index_tmp;
			img_size = 0;
		}

		img_size += __le64_to_cpu(segments[i].size_decompress);
	}

	/* the last partition */
	if (img_size > __le64_to_cpu(partitions[index].length)) {
		printf("image is too big, partition name: %s\n", partitions[index].name);
		return ERR_VERIFY;
	}

	vsystem("sync");
	vsystem("echo 3 > /proc/sys/vm/drop_caches");

	debug("Verify upgrade image success\n");

	return ERR_SUCCESS;
}

static int do_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	/* Notify upgrade start */
	if(ops->upgrade_callback)
		ops->upgrade_callback(STAT_START, 0);

	if (ops->upgrade_init)
		return ops->upgrade_init(ops, hdr);

	return ERR_SUCCESS;
}

static int do_upgrade_rom(struct upgrade_ops *ops, unsigned char *romcode, size_t size)
{
	int ret;

	if (!ops->upgrade_rom)
		return ERR_SUCCESS;

	ret = ops->upgrade_rom(ops, romcode, size);
	if(ret != ERR_SUCCESS)
		return ret;

	do_upgrade_report(ops, size);

	return ERR_SUCCESS;
}

static int do_upgrade_bootloader(struct upgrade_ops *ops, unsigned char *bootloader, size_t size)
{
	int ret;

	ret = ops->upgrade_bootloader(ops, bootloader, size);
	if(ret != ERR_SUCCESS)
		return ret;

	do_upgrade_report(ops, size * 2);

	return ERR_SUCCESS;
}

static int do_upgrade_segments(struct upgrade_ops *ops, struct upgrade_hdr *hdr,
				struct part_info *part_info, struct segment_info *seg_info, char *segments)
{
	int i, j, n, n_bak, index, ret = 0;
	unsigned char *compressed_buf = (unsigned char *)segments;
	unsigned char *decompress_buf;
	size_t decompress_len = COMPR_SEG_SIZE;
	struct segment_info *info;
	struct part_info *tmp, *upgrade_part;
	unsigned long long upgrade_offset;

	decompress_buf = malloc(COMPR_SEG_SIZE);
	if(!decompress_buf) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	i = __le16_to_cpu(hdr->segments) * sizeof(struct segment_info);
	info = (struct segment_info *)malloc(i);
	if (!info) {
		error("error %d, out of memory\n", __LINE__);
		return ERR_PROGRAM;
	}
	memcpy(info, seg_info, i);

	tmp = NULL;
	upgrade_part = NULL;
	upgrade_offset = 0;
	n_bak = 0;
	n = __le16_to_cpu(hdr->segments);

	for(i = 0; i < n; i++) {
		/* find the partition based on segment flash_offset */
		index = get_part_info_offset(part_info, __le16_to_cpu(hdr->partitions), info);
		if (index < 0) {
			error("get part info based on segment flash offset failed\n");
			return ERR_PROGRAM;
		}
		tmp = &part_info[index];

		/*
		 * only nor or mmc erase here, nand need scan before erasing,
		 * so do it solo
		 */
		if (strcmp((const char *)ops->type, "nand")) {
			/* erase only once */
			if (!upgrade_part || tmp != upgrade_part) {
				upgrade_part = tmp;
				if (upgrade_part->is_upgrade != 0 && ops->upgrade_erase) {
					/* since header version 4, upgrade one part once */
					if (hdr->part_flag[index] & DUAL_PART_FLAG_MASK
							&& higher_valid[index]) {
						/* upgrade part1 while part0 is higher version valid */
						ret = ops->upgrade_erase(ops, upgrade_part + 1);
						if (ret) {
							error("error %d, %d, %d\n", __LINE__, ret, errno);
							return ret;
						}

						/* segment.flash_offset need add part lengh if upgrade
						 * the second part
						 */
						upgrade_offset = __le64_to_cpu(upgrade_part->length);
						/* get the count of segments in current upgrade partition */
						n_bak = get_segment_cnt_one_part(upgrade_part, info);
						/* only upgrade part1 */
						for (j = 0; j < n_bak; j++)
							info[j].flash_offset = __cpu_to_le64(__le64_to_cpu(info[j].flash_offset) + upgrade_offset);
					} else {
						ret = ops->upgrade_erase(ops, upgrade_part);
						if (ret) {
							error("error %d, %d, %d\n", __LINE__, ret, errno);
							return ret;
						}
					}
				}
			}
		} else {
			/* new part */
			if (!upgrade_part || tmp != upgrade_part) {
				upgrade_part = tmp;
				/* is_upgrade is 0 in part1 */
				if (upgrade_part->is_upgrade != 0) {
					/* dual part and higher valid */
					if ((hdr->part_flag[index] & DUAL_PART_FLAG_MASK)
							&& higher_valid[index]) {
						/* segment.flash_offset need add part lengh if upgrade
						 * part1
						 */
						upgrade_offset = __le64_to_cpu(upgrade_part->length);
						/* get the count of segments in current upgrade partition */
						n_bak = get_segment_cnt_one_part(upgrade_part, info);
						/* only upgrade part1 */
						for (j = 0; j < n_bak; j++)
							info[j].flash_offset = __cpu_to_le64(__le64_to_cpu(info[j].flash_offset) + upgrade_offset);
					}
				}
			}
		}

		if(hdr->compressed) {
			decompress_len = COMPR_SEG_SIZE;

			/* Decompress and check buffer */
			ret = lzo1x_decompress_safe(compressed_buf, __le64_to_cpu(info->size_compress),
					decompress_buf, &decompress_len);
			if(ret != 0 || decompress_len != __le64_to_cpu(info->size_decompress)) {
				error("error %d, ret = %d, decompress_len = 0x%x, size_decompress = 0x%llx\n",
                                      __LINE__, ret, decompress_len, __le64_to_cpu(info->size_decompress));
				goto error;
			}

			if (strcmp((const char *)ops->type, "nand"))
				ret = ops->upgrade_segments(ops, info, decompress_buf);
			else {
				if (i == n-1)
					ret = ops->upgrade_segments_nand(ops, info, decompress_buf, part_info, __le64_to_cpu(hdr->partitions), true);
				else
					ret = ops->upgrade_segments_nand(ops, info, decompress_buf, part_info, __le16_to_cpu(hdr->partitions), false);
			}
			if (ret < 0) {
				error("error %d\n", __LINE__);
				goto error;
			}

			do_upgrade_report(ops, info->size_decompress);
		} else {
			error("non-compressed image is not supported now\n");
		}

		/* Next segment */
		compressed_buf += __le64_to_cpu(info->size_compress);
		info++;
	}

	free(decompress_buf);
	return ERR_SUCCESS;

error:
	free(decompress_buf);
	return ERR_PROGRAM;
}

static int do_upgrade_segments_ex(struct upgrade_ops *ops, struct upgrade_hdr *hdr,
				struct part_info *part_info, struct segment_info *seg_info, char *segments, int fd, unsigned int seg_offs)
{
	int i, j, n, n_bak, index, ret = 0;
	unsigned char *compressed_buf = (unsigned char *)segments;
	unsigned char *decompress_buf;
	size_t decompress_len = COMPR_SEG_SIZE;
	struct segment_info *info;
	struct part_info *tmp, *upgrade_part;
	unsigned long long upgrade_offset;
	char *tmp_buf;

	decompress_buf = malloc(COMPR_SEG_SIZE);
	if(!decompress_buf) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	tmp_buf = malloc(UPGRADE_UNIT);
	if (!tmp_buf) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	ret = lseek(fd, seg_offs, SEEK_SET);
	if (ret < 0) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	i = __le16_to_cpu(hdr->segments) * sizeof(struct segment_info);
	info = (struct segment_info *)malloc(i);
	if (!info) {
		error("error %d, out of memory\n", __LINE__);
		return ERR_PROGRAM;
	}
	memcpy(info, seg_info, i);

	tmp = NULL;
	upgrade_part = NULL;
	upgrade_offset = 0;
	n_bak = 0;
	n = __le16_to_cpu(hdr->segments);

	for(i = 0; i < n; i++) {
		/* find the partition based on segment flash_offset */
		index = get_part_info_offset(part_info, __le16_to_cpu(hdr->partitions), info);
		if (index < 0) {
			error("get part info based on segment flash offset failed\n");
			return ERR_PROGRAM;
		}
		tmp = &part_info[index];

		/*
		 * only nor or mmc erase here, nand need scan before erasing,
		 * so do it solo
		 */
		if (strcmp((const char *)ops->type, "nand")) {
			/* erase only once */
			if (!upgrade_part || tmp != upgrade_part) {
				upgrade_part = tmp;
				if (upgrade_part->is_upgrade != 0 && ops->upgrade_erase) {
					/* since header version 4, upgrade one part once */
					if (hdr->part_flag[index] & DUAL_PART_FLAG_MASK
							&& higher_valid[index]) {
						/* upgrade part1 while part0 is higher version valid */
						ret = ops->upgrade_erase(ops, upgrade_part + 1);
						if (ret) {
							error("error %d, %d, %d\n", __LINE__, ret, errno);
							return ret;
						}

						/* segment.flash_offset need add part lengh if upgrade
						 * the second part
						 */
						upgrade_offset = __le64_to_cpu(upgrade_part->length);
						/* get the count of segments in current upgrade partition */
						n_bak = get_segment_cnt_one_part(upgrade_part, info);
						/* only upgrade part1 */
						for (j = 0; j < n_bak; j++)
							info[j].flash_offset = __cpu_to_le64(__le64_to_cpu(info[j].flash_offset) + upgrade_offset);
					} else {
						ret = ops->upgrade_erase(ops, upgrade_part);
						if (ret) {
							error("error %d, %d, %d\n", __LINE__, ret, errno);
							return ret;
						}
					}
				}
			}
		} else {
			/* new part */
			if (!upgrade_part || tmp != upgrade_part) {
				upgrade_part = tmp;
				/* is_upgrade is 0 in part1 */
				if (upgrade_part->is_upgrade != 0) {
					/* dual part and higher valid */
					if ((hdr->part_flag[index] & DUAL_PART_FLAG_MASK)
							&& higher_valid[index]) {
						/* segment.flash_offset need add part lengh if upgrade
						 * part1
						 */
						upgrade_offset = __le64_to_cpu(upgrade_part->length);
						/* get the count of segments in current upgrade partition */
						n_bak = get_segment_cnt_one_part(upgrade_part, info);
						/* only upgrade part1 */
						for (j = 0; j < n_bak; j++)
							info[j].flash_offset = __cpu_to_le64(__le64_to_cpu(info[j].flash_offset) + upgrade_offset);
					}
				}
			}
		}

		if(hdr->compressed) {
			decompress_len = COMPR_SEG_SIZE;

			ret = read(fd, tmp_buf, __le64_to_cpu(info->size_compress));
			if (ret < 0) {
				error("error %d\n", __LINE__);
				goto error;
			}
			/* Decompress and check buffer */
			ret = lzo1x_decompress_safe(tmp_buf, __le64_to_cpu(info->size_compress),
					decompress_buf, &decompress_len);
			if(ret != 0 || decompress_len != __le64_to_cpu(info->size_decompress)) {
				error("error %d, ret = %d, decompress_len = 0x%x, size_decompress = 0x%llx\n",
                                      __LINE__, ret, decompress_len, __le64_to_cpu(info->size_decompress));
				goto error;
			}

			if (strcmp((const char *)ops->type, "nand"))
				ret = ops->upgrade_segments(ops, info, decompress_buf);
			else {
				if (i == n-1)
					ret = ops->upgrade_segments_nand(ops, info, decompress_buf, part_info, __le64_to_cpu(hdr->partitions), true);
				else
					ret = ops->upgrade_segments_nand(ops, info, decompress_buf, part_info, __le16_to_cpu(hdr->partitions), false);
			}
			if (ret < 0) {
				error("error %d\n", __LINE__);
				goto error;
			}

			do_upgrade_report(ops, info->size_decompress);
		} else {
			error("non-compressed image is not supported now\n");
		}

		/* Next segment */
		info++;
	}

	free(tmp_buf);
	free(decompress_buf);
	return ERR_SUCCESS;

error:
	free(decompress_buf);
	return ERR_PROGRAM;
}

static int do_upgrade_gpt(struct upgrade_ops *ops, struct part_info *parts, int part)
{
	int ret;

	ret = ops->upgrade_gpt(ops, parts, part);
	if(ret != ERR_SUCCESS)
		return ret;

	do_upgrade_report(ops, ops->gpt_size);

	return ERR_SUCCESS;
}

static bool force_upgrade = false;

void artosyn_set_force_upgrade(bool force)
{
	force_upgrade = force;
}

static int do_verify_ddr_setting(unsigned char *bootloader)
{
	struct spl_header *header = 0;
	int old_config[3], new_config[3], params_cnt;
	int dmc_node;
	void *fdt_blob;
	int fd;
	char *buf, *param_name, *param_val;
	char *p;
	int ret;

	/* get DDR setting from new DTB */
	header = (struct spl_header *)bootloader;
	fdt_blob = bootloader + header->spl_dtb_offset;

	if (fdt_magic(fdt_blob) != FDT_MAGIC) {
		error("New image fdt_magic wrong %x, skip verify DDR\n", fdt_magic(fdt_blob));
		return 0;
	}

	dmc_node = fdt_node_offset_by_compatible(fdt_blob, 0, "artosyn,sirius-dmc");
	if (dmc_node <= 0) {
		error("Get dmc node from new image fail %p\n", fdt_blob);
		return -1;
	}

	memset(new_config, 0, ARRAY_SIZE(new_config));
	params_cnt = fdtdec_get_int_array_count(fdt_blob, dmc_node,
				"dmc-config", (unsigned int *)new_config, ARRAY_SIZE(new_config));

	if (params_cnt % 3 != 0) {
		error("New dmc-config data length %d error\n", params_cnt);
		return -1;
	}

	/* get DDR setting from cmdline */
	fd = open("/proc/cmdline", O_RDONLY);
	if (fd == -1) {
		error("open /proc/cmdline failed, errno = %d\n", errno);
		return ERR_OTHER;
	}
	buf = malloc(1024);
	memset(buf, 0, 1024);
	ret = read(fd, buf, 1024);
	close(fd);
	if (ret == -1) {
		error("read /proc/cmdline failed, errno = %d\n", errno);
		free(buf);
		return ERR_OTHER;
	}

	if (!strlen(buf)) {
		printf("cat /proc/cmdline failed!\n");
		free(buf);
		return -1;
	}
	param_name = malloc(512);
	param_val = malloc(512);
	memset(param_name, 0, 512);
	memset(param_val, 0, 512);

	p = strtok(buf, " ");
	while (p != NULL) {
		sscanf(p, "%[a-z|_]=%[0-9]", param_name, param_val);
		if (strcmp(param_name, "ddr_type") == 0)
			sscanf(param_val, "%d", &old_config[0]);
		else if (strcmp(param_name, "ddr_freq") == 0)
			sscanf(param_val, "%d", &old_config[1]);
		else if (strcmp(param_name, "ddr_width") == 0)
			sscanf(param_val, "%d", &old_config[2]);
		p = strtok(NULL, " ");
	}
	free(param_name);
	free(param_val);
	free(buf);

	if (!old_config[0] || !old_config[1] || !old_config[2]) {
		printf("Old DDR config is unknown! skip verify DDR\n");
		return 0;
	}

	printf("Old DDR type %d frequence %d width %d\n"
		"New DDR type %d frequence %d width %d\n",
		old_config[0], old_config[1], old_config[2],
		new_config[0], new_config[1], new_config[2]);

	if (old_config[0] != new_config[0] || old_config[1] != new_config[1]
		|| old_config[2] != new_config[2]) {

		if (force_upgrade) {
			printf("DDR change!!! force upgrade......\n");
			return 0;
		} else {
			return -1;
		}
	}

	return 0;
}

static int do_upgrade(unsigned char *img, ar_flash_cb cb)
{
	int ret = 0;
	struct upgrade_hdr *hdr;
	unsigned char *hdr_ext;
	unsigned char *romcode;
	unsigned char *bootloader;
	struct part_info *partitions;
	struct segment_info *segments;
	struct upgrade_ops *ops;

	ret = do_verify_image(img, cb);
	if(ret < 0) {
		error("\nVerify upgrade image fail %d!\n", __LINE__);
		return ERR_VERIFY;
	}

	hdr         = (struct upgrade_hdr *)img;
	phdr 		= hdr;
	hdr_ext     = (unsigned char *)img + sizeof(*hdr);
	romcode     = hdr_ext + 32 + 256 + __le16_to_cpu(hdr->header_ext_size);
	bootloader  = romcode + __le32_to_cpu(hdr->rom_size);
	partitions  = (struct part_info *)(bootloader + __le32_to_cpu(hdr->loader_size));
	segments	= (struct segment_info *)(partitions + hdr->partitions);
	img         = (unsigned char *)(segments + hdr->segments);

	if (__le32_to_cpu(hdr->loader_size)) {
		ret = do_verify_ddr_setting(bootloader);
		if (ret < 0) {
			error("\nVerify ddr setting fail %d!\n", __LINE__);
			return ERR_DDR;
		}
	}

	switch(hdr->flashtype) {
	case FLASH_TYPE_MMC:
		debug("Flash type is mmc\n");
		ops = &mmc_upgrade_ops;
		break;
	case FLASH_TYPE_SPINOR:
		debug("Flash type is nor\n");
		ops = &nor_upgrade_ops;
		break;
	case FLASH_TYPE_SPINAND:
		debug("Flash type is nand\n");
		ops = &nand_upgrade_ops;
		nand_upgrade = 1;
		break;
	default:
		error("error %d, %d\n", __LINE__, ret);
		return ERR_FLASH_TYPE;
	}

	ops->upgrade_callback = cb;
	dump_upgrade_file(ops, (unsigned char *)hdr);
	ret = do_upgrade_init(ops, hdr);
	if(ret < 0)
		return ret;
	/*
	 * beacause reading gpt must be after nor/mmc init in boot,
	 * so sanity check here.
	 */
	debug("sanity check partitions information before really upgrade\n");
	ret = do_upgrade_sanity_check(ops, hdr, partitions);
	if (!ret) {
		error("sanity check failed, exit upgrade\n");
		return ERR_OTHER;
	}

	if(__le32_to_cpu(hdr->rom_size)) {
		ret = do_upgrade_rom(ops, romcode, __le32_to_cpu(hdr->rom_size));
		if(ret < 0)
			return ret;
	}

	if(__le32_to_cpu(hdr->loader_size)) {
		ret = do_upgrade_bootloader(ops, bootloader, __le32_to_cpu(hdr->loader_size));
		if(ret < 0)
			return ret;
	}

	ret = do_upgrade_segments(ops, hdr, partitions, segments, (char *)img);
	if(ret < 0)
		return ret;

	ret = do_upgrade_gpt(ops, partitions, __le16_to_cpu(hdr->partitions));
	if(ret < 0)
		return ret;

	if(ops->upgrade_callback)
		ops->upgrade_callback(STAT_DONE, 100);

	return ERR_SUCCESS;
}

static int do_upgrade_ex(char *filename, ar_flash_cb cb)
{
	int ret = 0, rdlen, fd;
	struct upgrade_hdr *hdr;
	unsigned char *hdr_ext;
	unsigned char *romcode;
	unsigned char *bootloader;
	struct part_info *partitions;
	struct segment_info *segments;
	struct upgrade_ops *ops;
	struct stat st;
	char *img;
	unsigned int seg_offs;

	ret = stat(filename, &st);
	if(ret < 0) {
		printf("error %d, %d, %d\n", __LINE__, ret, errno);
		return -1;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("open failed\n");
		return -1;
	}

	if (st.st_size > UPGRADE_UNIT) {
		rdlen = UPGRADE_UNIT;
		single_upgrade = false;
	} else {
		rdlen = st.st_size;
		single_upgrade = true;
	}
	img = malloc(rdlen);
	if (!img) {
		printf("malloc failed\n");
		return -1;
	}
	ret = read(fd, img, rdlen);
	if (ret < 0) {
		printf("read failed\n");
		return -1;
	}

	ret = do_verify_image_ex(img, cb, fd);
	if(ret < 0) {
		error("\nVerify upgrade image fail %d!\n", __LINE__);
		return ERR_VERIFY;
	}

	hdr         = (struct upgrade_hdr *)img;
	phdr 		= hdr;
	hdr_ext     = (unsigned char *)img + sizeof(*hdr);
	romcode     = hdr_ext + 32 + 256 + __le16_to_cpu(hdr->header_ext_size);
	bootloader  = romcode + __le32_to_cpu(hdr->rom_size);
	partitions  = (struct part_info *)(bootloader + __le32_to_cpu(hdr->loader_size));
	segments	= (struct segment_info *)(partitions + hdr->partitions);
	seg_offs    = (char *)(segments + hdr->segments) - img;
	img         = (unsigned char *)(segments + hdr->segments);

	if (__le32_to_cpu(hdr->loader_size)) {
		ret = do_verify_ddr_setting(bootloader);
		if (ret < 0) {
			error("\nVerify ddr setting fail %d!\n", __LINE__);
			return ERR_DDR;
		}
	}

	switch(hdr->flashtype) {
	case FLASH_TYPE_MMC:
		debug("Flash type is mmc\n");
		ops = &mmc_upgrade_ops;
		break;
	case FLASH_TYPE_SPINOR:
		debug("Flash type is nor\n");
		ops = &nor_upgrade_ops;
		break;
	case FLASH_TYPE_SPINAND:
		debug("Flash type is nand\n");
		ops = &nand_upgrade_ops;
		nand_upgrade = 1;
		break;
	default:
		error("error %d, %d\n", __LINE__, ret);
		return ERR_FLASH_TYPE;
	}

	ops->upgrade_callback = cb;
	dump_upgrade_file(ops, (unsigned char *)hdr);
	ret = do_upgrade_init(ops, hdr);
	if(ret < 0)
		return ret;
	/*
	 * beacause reading gpt must be after nor/mmc init in boot,
	 * so sanity check here.
	 */
	debug("sanity check partitions information before really upgrade\n");
	ret = do_upgrade_sanity_check(ops, hdr, partitions);
	if (!ret) {
		error("sanity check failed, exit upgrade\n");
		return ERR_OTHER;
	}

	if(__le32_to_cpu(hdr->rom_size)) {
		ret = do_upgrade_rom(ops, romcode, __le32_to_cpu(hdr->rom_size));
		if(ret < 0)
			return ret;
	}

	if(__le32_to_cpu(hdr->loader_size)) {
		ret = do_upgrade_bootloader(ops, bootloader, __le32_to_cpu(hdr->loader_size));
		if(ret < 0)
			return ret;
	}

	if (single_upgrade)
		ret = do_upgrade_segments(ops, hdr, partitions, segments, (char *)img);
	else
		ret = do_upgrade_segments_ex(ops, hdr, partitions, segments, (char *)img, fd, seg_offs);
	if(ret < 0)
		return ret;

	ret = do_upgrade_gpt(ops, partitions, __le16_to_cpu(hdr->partitions));
	if(ret < 0)
		return ret;

	if(ops->upgrade_callback)
		ops->upgrade_callback(STAT_DONE, 100);

	close(fd);
	return ERR_SUCCESS;
}

int ar_upgrade_prepare(void)
{
	int fd, ret = 0;
	long long mem_avail;
	char buf[256] = {0};
	char path[32] = "/tmp/ota-prepare.sh";
	char *p;

	ret = access(path, F_OK);
	if (ret) {
		strcpy(path, "/etc/ota-prepare.sh");
		ret = access(path, F_OK);
		if (ret) {
			error("ota-prepare.sh is not exsit\n");
			return ERR_OTHER;
		}
	}

	ret = vsystem(path);
	if (ret)
		return ERR_OTHER;

	/* get available memory size */
	fd = open("/proc/meminfo", O_RDONLY);
	if (fd == -1) {
		error("errno = %d\n", errno);
		return ERR_OTHER;
	}

	/* 120 is enough */
	ret = read(fd, buf, 120);
	if (ret == -1) {
		printf("errno = %d\n", errno);
		close(fd);
		return ERR_OTHER;
	}

	close(fd);
	p = strstr(buf, "MemAvailable:");
	if (!p) {
		printf("MemAvailable is not found\n");
		return ERR_OTHER;
	}

	p = strchr(p, ':');
	p++;
	mem_avail = atoll(p);
	mem_avail *= 1024;
/* FIXME demanded memory size */
#define MEM_MIN 	(0xa00000) 	/* 10M */
	if (mem_avail < MEM_MIN) {
		printf("the available memory is too small, demand = %#x, mem_avail = %#llx\n",
				MEM_MIN, mem_avail);
		return ERR_OTHER;
	}

	return ERR_SUCCESS;
}

int ar_upgrade(char *img, int len, ar_flash_cb cb)
{
	int ret;

	ret = do_upgrade((unsigned char *)img, cb);
	if(ret < 0) {
		error("\nUpgrade upgrade image fail %d!\n", __LINE__);
		return ret;
	}

	/* Flush file cache */
	system("sync");

	debug("\nUpgrade image done!\n");

	return ret;
}

int ar_upgrade_ex(char *filename, ar_flash_cb cb)
{
	int ret;

	ret = do_upgrade_ex(filename, cb);
	if(ret < 0) {
		error("\nUpgrade upgrade image fail %d!\n", __LINE__);
		return ret;
	}

	/* Flush file cache */
	system("sync");

	debug("\nUpgrade image done!\n");

	return ret;
}

int ar_flash_program(char *img, int len, ar_flash_cb cb)
{
	int ret;

	ret = ar_upgrade_prepare();
	if (ret) {
		error("ota prepare failed, ret = %d\n", ret);
		return ERR_OTHER;
	}

	ret = do_upgrade((unsigned char *)img, cb);
	if(ret < 0) {
		error("\nUpgrade upgrade image fail %d!\n", __LINE__);
		return ret;
	}

	/* Flush file cache */
	system("sync");

	debug("\nUpgrade image done!\n");

	return ret;
}

int ar_enter_clean_system(ar_flash_cb cb)
{
	int ret;

	ret = access("/etc/clean-system-enter.sh", F_OK);
	if (ret) {
		error("/etc/clean-system-enter.sh is not exsit\n");
		return ERR_OTHER;
	}

	if (cb) {
		ret = cb(STAT_ENTER_CLEAN_SYSTEM, 0);
		if (ret) {
			error("callback failed\n");
			return ret;
		}
	}

	ret = vsystem("/etc/clean-system-enter.sh");
	if (ret) {
		/* notify failed */
		if (cb) {
			ret = cb(STAT_ENTER_CLEAN_SYSTEM_FAILED, 0);
			if (ret) {
				error("callback failed\n");
				return ret;
			}
		}
		return ret;
	}

	return ERR_SUCCESS;
}

int ar_enter_clean_system_ext(ar_flash_cb cb, char *params)
{
	int ret;
	char tmp[128] = {0};

	ret = access("/etc/clean-system-enter.sh", F_OK);
	if (ret) {
		error("/etc/clean-system-enter.sh is not exsit\n");
		return ERR_OTHER;
	}

	if (cb) {
		ret = cb(STAT_ENTER_CLEAN_SYSTEM, 0);
		if (ret) {
			error("callback failed\n");
			return ret;
		}
	}

	sprintf(tmp, "/etc/clean-system-enter.sh %s", params);
	ret = vsystem(tmp);
	if (ret) {
		/* notify failed */
		if (cb) {
			ret = cb(STAT_ENTER_CLEAN_SYSTEM_FAILED, 0);
			if (ret) {
				error("callback failed\n");
				return ret;
			}
		}
		return ret;
	}

	return ERR_SUCCESS;
}

/* FIXME this limit image size 2G, parameters len should be uint64_t */
int ar_flash_get_sdkversion(char *img, int len, char *sdkver)
{
	struct upgrade_hdr *hdr = (struct upgrade_hdr *)img;

	strcpy(sdkver, (const char *)hdr->sdk_version);

	return 0;
}

/* set upgrade mode: develop or product */
void ar_upgrade_product(bool product)
{
	if (product)
		develop = 0;
	else
		develop = 1;
}

static int is_pte_valid(gpt_entry * pte)
{
	efi_guid_t unused_guid;

	if (!pte) {
		printf("%s: Invalid Argument(s)\n", __func__);
		return 0;
	}

	/* Only one validation for now:
	 * The GUID Partition Type != Unused Entry (ALL-ZERO)
	 */
	memset(unused_guid.b, 0, sizeof(unused_guid.b));

	if (memcmp(pte->partition_type_guid.b, unused_guid.b,
		sizeof(unused_guid.b)) == 0) {
		return 0;
	} else {
		return 1;
	}
}

int ar_emmc_last_partition_resize(void)
{
	int i, ret, pte_size, fd;
	unsigned char	*gpt0, *gpt1, *gpt;
	gpt_header		*p_gpt0, *p_gpt1, *p_gpt;
	gpt_entry		*p_entry;
	int gpt0_ok = 0, gpt1_ok = 0;
	unsigned int calc_crc32;
	unsigned long long offs, offs_seek;
	char dev[128] = {0};
	struct upgrade_ops *ops = &mmc_upgrade_ops;

	ret = mmc_upgrade_init(ops, NULL);
	if (ret)
		return ret;


	ret = current_gpt_check(ops, &gpt0_ok, &gpt1_ok, &gpt0, &gpt1);
	if (ret < 0) {
		error("gpt check failed\n");
		return ret;
	}

	p_gpt0   = (gpt_header *)(gpt0 + FLASH_BLKSZ);
	p_gpt1   = (gpt_header *)(gpt1 + FLASH_BLKSZ);

	/* both ok */
	if (gpt0_ok && gpt1_ok) {
		if (__le32_to_cpu(p_gpt0->reserved1) >= __le32_to_cpu(p_gpt1->reserved1)) {
			higher_version = 0;
			p_gpt = p_gpt0;
			gpt = gpt0;
		} else {
			higher_version = 1;
			p_gpt = p_gpt1;
			gpt = gpt1;
		}
	} else if (gpt0_ok) {
		higher_version = 0;
		p_gpt = p_gpt0;
		gpt = gpt0;
	} else if (gpt1_ok) {
		higher_version = 1;
		p_gpt = p_gpt1;
		gpt = gpt1;
	} else {
		free(gpt0);
		free(gpt1);
		return -1;
	}

	p_entry = (gpt_entry *)(gpt + FLASH_BLKSZ * 2);
	for (i = 0; i < p_gpt->num_partition_entries; i++) {
		if (!is_pte_valid(&p_entry[i]))
			break;
	}

	p_gpt->last_usable_lba = __cpu_to_le64(ops->usr_size / FLASH_BLKSZ - 34);
	p_entry[i - 1].ending_lba = p_gpt->last_usable_lba;

	/* Generate CRC for the Primary GPT Header */
	calc_crc32 = efi_crc32((const unsigned char *)p_entry,
			      __le32_to_cpu(p_gpt->num_partition_entries) *
			      __le32_to_cpu(p_gpt->sizeof_partition_entry));
	p_gpt->partition_entry_array_crc32 = __cpu_to_le32(calc_crc32);

	p_gpt->header_crc32 = 0;
	calc_crc32 = efi_crc32((const unsigned char *)p_gpt,
			      __le32_to_cpu(p_gpt->header_size));
	p_gpt->header_crc32 = __cpu_to_le32(calc_crc32);

	pte_size = p_gpt->num_partition_entries * sizeof(gpt_entry);

	sprintf(dev, "/dev/%s", ops->disk_usr);
	fd = open(dev, O_WRONLY | O_SYNC);
	if(fd < 0) {
		error("error %d, %d, %d\n", __LINE__, ret, errno);
		return -1;
	}

	if (higher_version == 0) {
		/* No need to lseek */
		ret = write(fd, gpt, ops->gpt_size);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			close(fd);
			goto error_write;
		}
	}

	if (higher_version == 1) {
		offs = ops->usr_size - FLASH_BLKSZ - pte_size;

		offs_seek = lseek64(fd, offs, SEEK_SET);
		if(offs != offs_seek) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}

		/* backup gpt entry */
		ret = write(fd, (char *)p_entry, pte_size);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}

		offs = ops->usr_size - FLASH_BLKSZ;
		offs_seek = lseek64(fd, offs, SEEK_SET);
		if(offs != offs_seek) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}

		/* backup gpt header */
		ret = write(fd, (char *)p_gpt, FLASH_BLKSZ);
		if(fd < 0) {
			error("error %d, %d, %d\n", __LINE__, ret, errno);
			goto error_write;
		}
	}

	close(fd);
	free(gpt0);
	free(gpt1);

	return 0;
error_write:
	close(fd);

	return -1;
}
