/*
 * Artosyn program to generate spi/emmc program image
 *
 * Upgrade image layout:
 *
 * |--Header--|--Hash--|--Signature--|--Romcode--|--bootloader--|--Partitions[]--|--Segment info[]--|--Segment data[]--|
 */

#include <common.h>
#include <asm/arch/image.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/compat.h>
#include <linux/delay.h>
#include <linux/mtd/ubi-media.h>
#include <linux/mtd/partitions.h>
#include <linux/lzo.h>
#include <fs.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <memalign.h>
#include <part.h>
#include <usb.h>
#include <spi.h>
#include <nand.h>
#include <mmc.h>
#include <spi_flash.h>
#include <image.h>
#include <part_efi.h>
#include <console.h>
#include <malloc.h>
#include <fdtdec.h>
#include <uuid.h>
#include <mtd.h>
#include <uboot_upgrade.h>
#include <artosyn/ar_common.h>
#include <asm/arch/boot.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef CONFIG_MTD_SPI_NAND
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
#define ERASEBLOCK_CNT 		2048 /* TODO */

struct args args;
static struct upgrade_ops nand_upgrade_ops;
#endif /* end of CONFIG_MTD_SPI_NAND */

#ifdef CONFIG_MMC
static struct mmc *mmc_dev;
#endif

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
enum {
	ERR_SUCCESS    = 0,
	ERR_VERIFY     = -1,
	ERR_PROGRAM    = -2,
	ERR_ERASE      = -3,
	ERR_FLASH_TYPE = -4,
	ERR_IMG_SIZE   = -5,
	ERR_OTHER 	   = -6,
	ERR_DDR        = -7,
};

enum {
	STAT_VERIFY_IMAGE = 0,
	STAT_START = 1,
	STAT_IN_PROGRESS,
	STAT_DONE,
};

char upgradefilename[64];
bool single_upgrade;

static struct blk_desc *stor_dev = NULL; /* ? */
#ifdef CONFIG_USB_STORAGE
static int usb_stor_curr_dev = -1; /* current device */
#endif

enum part_status {
	PART_STATUS_NOCHANGE = 0,
	PART_STATUS_CHANGE,
	PART_STATUS_UNKNOWN
};

/* header extention, now is empty */
struct upgrade_hdr_ext {
	unsigned char reserve[0];
} __attribute__((packed));

enum {
	FLASH_TYPE_SPINOR = 0,
	FLASH_TYPE_MMC = 1,
	FLASH_TYPE_SPINAND = 2,
};

#define PARTITION_VALID_SHIFT 	44
#define PARTITION_VALID_MASK 	(1ULL << PARTITION_VALID_SHIFT)

#define DUAL_PART_FLAG_SHIFT 	0
#define DUAL_PART_FLAG_MASK 	(0x01 << DUAL_PART_FLAG_SHIFT)

#define PART_NAME_MAX 	16

/* reserved spade, for nand bad block table(4 blocks) and factory bad block swap(need up to 20 blocks based on winbond mcp spec) */
#define MCP_NAND_RESERVE_SIZE 	0x300000

static char disk_usr[PART_NAME_MAX] = {0};
__maybe_unused static char disk_rom[PART_NAME_MAX] = {0};
static char disk_boot0[PART_NAME_MAX] = {0};
static char disk_boot1[PART_NAME_MAX] = {0};
static unsigned long long partitions_size;
static unsigned int object_version = 0;
static int higher_version = -1;
static char higher_valid[128] = {0};
static int develop = 1;
struct upgrade_hdr *phdr;

typedef int (*ar_flash_cb)(int state, int percent);

static int fs_set_usb_dev(void);
extern int spl_verify_sw(char *image_name, unsigned char *hash, unsigned long hash_len,
		unsigned char *sig, unsigned long sig_len,
		unsigned char *img, unsigned long img_len);
extern char *do_fat_getfnamecopy(int *files_cnt, int *dirs_cnt);

int set_single_upgrade(bool flag)
{
	single_upgrade = flag;

	return 0;
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
#ifdef CONFIG_MTD_SPI_NAND
	if (hdr->flashtype == FLASH_TYPE_SPINAND)
		ops->totalsize += __le32_to_cpu(hdr->loader_size) * NAND_SPL_PARTS; /* only nand has 4 spl */
	else
#endif
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

/* size: block number of disk */
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
	gpt_h->signature = __cpu_to_le64(GPT_HEADER_SIGNATURE_UBOOT);
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

#ifdef CONFIG_MTD_SPI_NAND
		if (offset > nand_upgrade_ops.usr_size) {
			error("Partitions layout exceds disk size %lx %lx\n", offset, last_usable_lba);
			return -1;
		}
#else
		if (offset >= last_usable_lba) {
			error("Partitions layout exceds disk size %lx %lx\n", offset, last_usable_lba);
			return -1;
		}
#endif
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

static __maybe_unused void gpt_prepare_backup_gpt_header(gpt_header *gpt_h)
{
	uint32_t calc_crc32;
	uint64_t val;

	/* recalculate the values for the Backup GPT Header */
	val = le64_to_cpu(gpt_h->my_lba);
	gpt_h->my_lba = gpt_h->alternate_lba;
	gpt_h->alternate_lba = cpu_to_le64(val);
	gpt_h->partition_entry_lba = cpu_to_le64(le64_to_cpu(gpt_h->last_usable_lba) + 1);
	gpt_h->header_crc32 = 0;

	calc_crc32 = efi_crc32((const unsigned char *)gpt_h,
			       le32_to_cpu(gpt_h->header_size));
	gpt_h->header_crc32 = cpu_to_le32(calc_crc32);
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

	alloc_size = GPT_BUF_SZ;
	gpt = memalign(ARCH_DMA_MINALIGN, alloc_size);
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

#ifdef CONFIG_MTD_SPI_NAND
extern u32  crc32_le(u32 crc, unsigned char const *p, size_t len);
static u32 mtd_crc32(u32 crc, unsigned char const *p, size_t len)
{
	return crc32_le(crc, p, len);
}

static struct mtd_info *get_mtd_device_off(uint64_t off)
{
	int err = -ENODEV;
	struct mtd_info *mtd = NULL, *slave;

	list_for_each_entry(slave, &nand_mtd->partitions, node) {
		if (off >= slave->offset && off < slave->offset + slave->size) {
			mtd = slave;
			break;
		}
	}

	if (!mtd)
		return NULL;

	err = __get_mtd_device(mtd);
	if (err)
		return ERR_PTR(err);

	return mtd;
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
static int ubi_scan(struct mtd_info *mtd, struct ubi_scan_info **info, int verbose)
{
	int eb, v = (verbose == 2), pr = (verbose == 1);
	struct ubi_scan_info *si;
	unsigned long long sum = 0;
	int eb_cnt;
	int writesize;

	eb_cnt = mtd->size / mtd->erasesize;
	writesize = mtd->writesize;

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

	printf("ubiscan: start scanning eraseblocks 0-%d\n", eb_cnt - 1);
	for (eb = 0; eb < eb_cnt; eb++) {
		int ret;
		uint32_t crc;
		struct ubi_ec_hdr ech;
		unsigned long long ec;
		size_t retlen;
		loff_t off;

		off = eb * mtd->erasesize;

		if (pr) {
			printf("\n" PROGRAM "scanning eraseblock %d -- %2d %% complete  \n\n",
				   eb, (eb + 1) * 100 / eb_cnt);
		}

		if (v)
			printf(PROGRAM "scanning eraseblock %d", eb);

		ret = mtd_block_isbad(mtd, off);
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

		ret = mtd_read(mtd, off, sizeof(struct ubi_ec_hdr), &retlen, (u_char *)&ech);
		if (ret == -EUCLEAN || ret == -EBADMSG) {
			printf("\n" PROGRAM "Attention, data unclean or badmsg, ret = %d, leave really bad block process to mtd_write!!!\n", ret);
		} else if (ret) {
			printf(PROGRAM "\nmtd_read error ret = %d\n", ret);
			goto out_ec;
		}

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
			if (si->data_offs % mtd->writesize) {
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
static int ubigen_write_layout_vol(const struct ubigen_info *ui, int peb1, int peb2,
			    long long ec1, long long ec2,
			    struct ubi_vtbl_record *vtbl, struct mtd_info *mtd)
{
	int ret;
	struct ubigen_vol_info vi;
	char outbuf[128 * 1024];
	struct ubi_vid_hdr *vid_hdr;
	off_t seek;
	size_t retlen;

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

	ubigen_init_ec_hdr(ui, (struct ubi_ec_hdr *)outbuf, ec1);
	ubigen_init_vid_hdr(ui, &vi, vid_hdr, 0, NULL, 0);

	ret = mtd_write(mtd, seek, mtd->erasesize, &retlen, (u_char *)outbuf);
	if (ret) {
		printf(PROGRAM "cannot write %d bytes\n", ui->peb_size);
		goto out_free;
	}

	seek = (off_t) peb2 * ui->peb_size;
	ubigen_init_ec_hdr(ui, (struct ubi_ec_hdr *)outbuf, ec2);
	ubigen_init_vid_hdr(ui, &vi, vid_hdr, 1, NULL, 0);

	ret = mtd_write(mtd, seek, mtd->erasesize, &retlen, (u_char *)outbuf);
	if (ret) {
		printf(PROGRAM "cannot write %d bytes\n", ui->peb_size);
		goto out_free;
	}
	return 0;
out_free:
	return -1;
}

static void print_bad_eraseblocks(const struct mtd_info *mtd,
				  const struct ubi_scan_info *si)
{
	int first = 1, eb;
	int eb_cnt;

	eb_cnt = mtd->size / mtd->erasesize;
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

static int drop_ffs(const struct mtd_info *mtd, const void *buf, int len)
{
	int i;
	int writesize;

	writesize = mtd->writesize;

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
		|| !strcmp(name, "bl31")
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

static int flash_image(struct mtd_info *mtd, const struct ubigen_info *ui,
		struct ubi_scan_info *si, const char *img, size_t size, int start_eb)
{
	int img_ebs, eb, written_ebs = 0, divisor, skip_data_read = 0;
	int eb_cnt;
	int erasesize;
	struct erase_info instr;
	loff_t off;
	size_t retlen;
	int mod;

	eb_cnt = mtd->size / mtd->erasesize;
	erasesize = mtd->erasesize;
	img_ebs = size / mtd->erasesize;
	/* ubi image is block aligned, but rawimage size maybe not block aligned */
	mod = size % mtd->erasesize;
	if (mod)
		img_ebs++;

	if (!mtd || !ui || !si || !img) {
		printf("flash_image has NULL parameter\n");
		return -EINVAL;
	}

	if (img_ebs > si->good_cnt) {
		printf(PROGRAM "image is too large (%lld bytes)\n",
			  (long long)size);
		goto out_close;
	}

	if ((size % erasesize) && !part_is_rawimage(mtd->name)) {
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
		}

		if (si->ec[eb] == EB_BAD) {
			divisor += 1;
			continue;
		}

		if (args.verbose) {
			printf(PROGRAM "eraseblock %d: erase", eb);
		}

		memset(&instr, 0, sizeof(instr));
		instr.mtd = mtd;
		instr.addr = eb * mtd->erasesize;
		instr.len = mtd->erasesize;
		err = mtd_erase(mtd, &instr);
		if (err) {
			if (!args.quiet)
				printf("\n");
			printf(PROGRAM "failed to erase eraseblock %d, ret = %d\n", eb, err);

			if (err != -EIO)
				goto out_close;

			off = eb * mtd->erasesize;
			if (mtd_block_markbad(mtd, off))
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
		}

		if (!part_is_rawimage(mtd->name)) {
			err = change_ech((struct ubi_ec_hdr *)buf, ui->image_seq, ec);
			if (err) {
				printf("bad EC header at eraseblock %d of \"%s\"\n",
					   written_ebs, args.image);
				goto out_close;
			}
		}

		if (args.verbose) {
			printf(", write data\n");
		}

		/* rawimage is not block aligned, the last size is less than erasesize */
		if (mod && (written_ebs == img_ebs - 1))
			new_len = mod;
		else
			new_len = drop_ffs(mtd, buf, erasesize);

		off = eb * mtd->erasesize;
		err = mtd_write(mtd, off, new_len, &retlen, (u_char *)buf);
		if (err) {
			printf(PROGRAM "cannot write eraseblock %d\n", eb);

			if (err != -EIO)
				goto out_close;

			if (mtd_block_markbad(mtd, off))
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

static int format(struct mtd_info *mtd, const struct ubigen_info *ui, struct ubi_scan_info *si,
		  int start_eb, int novtbl)
{
	int eb, err, write_size;
	struct ubi_ec_hdr *hdr;
	struct ubi_vtbl_record *vtbl;
	int eb1 = -1, eb2 = -1;
	long long ec1 = -1, ec2 = -1;
	int eb_cnt;
	int writesize;
	struct erase_info instr;
	size_t retlen;

	eb_cnt = mtd->size / mtd->erasesize;
	writesize = mtd->writesize;
	if (!mtd || !ui || !si) {
		printf("format has NULL parameter\n");
		return -EINVAL;
	}

	write_size = UBI_EC_HDR_SIZE + mtd->writesize - 1;
	write_size /= mtd->writesize;
	write_size *= mtd->writesize;

	hdr = malloc(write_size);
	if (!hdr) {
		printf(PROGRAM "cannot allocate %d bytes of memory\n", write_size);
		return -ENOMEM;
	}
	memset(hdr, 0xFF, write_size);

	for (eb = start_eb; eb < eb_cnt; eb++) {
		long long ec;
		loff_t off = eb * mtd->erasesize;

		if (!args.quiet && !args.verbose) {
			printf("\n"PROGRAM "formatting eraseblock %d -- %2d %% complete  \n\n",
			   eb, (eb + 1 - start_eb) * 100 / (eb_cnt - start_eb));
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
		}

		memset(&instr, 0, sizeof(instr));
		instr.mtd = mtd;
		instr.addr = off;
		instr.len = mtd->erasesize;
		err = mtd_erase(mtd, &instr);
		if (err) {
			if (!args.quiet)
				printf("\n");

			printf(PROGRAM "failed to erase eraseblock %d\n", eb);
			if (err != -EIO)
				goto out_free;

			if (mtd_block_markbad(mtd, off))
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
		}

		err = mtd_write(mtd, off, write_size, &retlen, (u_char *)hdr);
		if (err) {
			if (!args.quiet && !args.verbose)
				printf("\n");

			printf(PROGRAM "cannot write EC header (%d bytes buffer) to eraseblock %d\n",
				   write_size, eb);

			if (err != -EIO) {
				if (args.subpage_size != writesize)
					printf(PROGRAM "may be sub-page size is incorrect?\n");
				goto out_free;
			}

			if (mtd_block_markbad(mtd, off))
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
					      mtd);
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
	/* FIXME: now all page == subpage */
	args.subpage_size = nand_mtd->writesize;
	args.vid_hdr_offs = nand_mtd->writesize;
	args.image_seq = get_timer(0);
}

static int ubi_prepare(struct mtd_info *mtd,
		       struct ubigen_info *ui, struct ubi_scan_info *usi, uint32_t *ec, uint32_t ec_cnt)
{
	int err;
	struct ubi_scan_info *si = NULL;
	int  verbose;
	int eb_cnt;

	if (!mtd || !ui || !usi || !ec) {
		printf("ubi_prepare has NULL parameter\n");
		return -EINVAL;
	}

	args_init();

	/* rawimage quiet */
	if (part_is_rawimage(mtd->name)) {
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

	err = ubi_scan(mtd, &si, verbose);
	if (err) {
		printf(PROGRAM "failed to scan mtd%d\n", mtd->index);
		goto out_close;
	}

	if (si->good_cnt == 0) {
		printf(PROGRAM "all %d eraseblocks are bad\n", si->bad_cnt);
		goto out_free;
	}

	if (si->good_cnt < 2 && (!args.novtbl || args.image)) {
		printf(PROGRAM "too few non-bad eraseblocks (%d) on mtd%d\n",
		       si->good_cnt, mtd->index);
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

	ubigen_info_init(ui, mtd->erasesize, mtd->writesize, mtd->writesize,
			 args.vid_hdr_offs, args.ubi_ver, args.image_seq);

	memcpy(usi, si, sizeof(*usi));
	eb_cnt = mtd->size / mtd->erasesize;
	if (ec_cnt < eb_cnt * sizeof(uint32_t)) {
		goto out_free;
	}
	memcpy(ec, si->ec, eb_cnt * sizeof(uint32_t));
	usi->ec = ec;
	ubi_scan_free(si);
	return 0;

out_free:
	ubi_scan_free(si);
out_close:
	return -1;

}

/* create partitions based on ini file info */
static int mtd_create_partitions_nand(struct part_info *part_info, int nrparts)
{
	int i, ret = 0;
	struct mtd_partition parts;

	if (!part_info || nrparts < 1)
		return -EINVAL;

	for (i = 0; i < nrparts; i++) {
		memset(&parts, 0, sizeof(parts));

		parts.name = (const char *)part_info[i].name;
		parts.size = __le64_to_cpu(part_info[i].length);
		parts.offset = __le64_to_cpu(part_info[i].flash_offset);

		ret = add_mtd_partitions(nand_mtd, &parts, 1);
		if (ret) {
			printf("add_mtd_partitions failed\n");
			break;
		}
	}

	return ret;
}

static int nand_mtd_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	struct mtd_info *mtd;
	unsigned long long size;
	unsigned int spl_size;

	/* Ensure all devices (and their partitions) are probed */
	mtd_probe_devices();

	mtd_for_each_device(mtd) {
		if (!mtd_is_partition(mtd) && mtd->type == MTD_NANDFLASH) {
			nand_mtd = mtd;
			break;
		}
	}

	if (nand_mtd == NULL) {
		printf("\n                           ^*********^                     \n");
		printf("init nand fail, please check your image or uboot config!!!\n");
		printf("\n                           ^*********^                     \n");
		return ERR_FLASH_TYPE;
	}

	ops->usr_size = nand_mtd->size;
	ops->erasesize = nand_mtd->erasesize;

	spl_size = NAND_SPL_PAGES * nand_mtd->writesize;
	spl_size *= NAND_SPL_PARTS;

	size = spl_size + ops->erasesize * NAND_PES_PER_GPT * 2;
	ops->gpt_size = size; /* gpt size include spl size in nand */
	if (ops->usr_size - size < partitions_size) {
		printf("\n                           ^*********^                                               \n");
		printf("partitions capacity: 0x%llx, available size  0x%llx (spl, config need 0x%llx)\n" \
				"but the corresponding image partitions total size: 0x%llx\n" \
				"partitions capacity is too small, please check your image!!!\n",
				ops->usr_size, ops->usr_size - size, size,  partitions_size);
		printf("\n                           ^*********^                                               \n");
		return ERR_IMG_SIZE;
	}

	return ERR_SUCCESS;
}

static int nand_mtd_erase_common(struct mtd_info *mtd, unsigned int offset, size_t part_size)
{
	int eb_cnt, eb;
	struct erase_info instr;
	loff_t off;

	/* part_size must be block aligned */
	eb_cnt = part_size / mtd->erasesize;
	eb = offset / mtd->erasesize;
	eb_cnt += eb;

	for (; eb < eb_cnt; eb++) {
		int err;

		off = eb * mtd->erasesize;
		err = mtd_block_isbad(mtd, off);
		if (err < 0) {
			printf("\nmtd_block_isbad error ret=%d\n", err);
			return ERR_OTHER;
		}
		if (err)
			continue;

		memset(&instr, 0, sizeof(instr));
		instr.mtd = mtd;
		instr.addr = off;
		instr.len = mtd->erasesize;
		err = mtd_erase(mtd, &instr);
		if (err) {
			printf("failed to erase eraseblock %d, ret = %d\n", eb, err);

			if (err != -EIO)
				return ERR_ERASE;

			off = eb * mtd->erasesize;
			if (mtd_block_markbad(mtd, off))
				return ERR_ERASE;

			continue;
		}
	}

	return ERR_SUCCESS;
}

static int nand_mtd_upgrade_bootloader(struct upgrade_ops *ops,
		unsigned char *bootloader, size_t size)
{
	int i, ret;
	unsigned int start;
	unsigned int spl_size;

	/* spl: 128 pages */
	spl_size = NAND_SPL_PAGES * nand_mtd->writesize;

	for (i = 0; i < NAND_SPL_PARTS; i++) {
		start = i * spl_size;

		debug("Erase and Write mtd bootloader%d...\n", i);
		/* if has bad blocks, it return error, just ignore it */
		nand_mtd_write_common(nand_mtd, start, spl_size, bootloader, size);
	}

	return ERR_SUCCESS;
}

static int nand_mtd_upgrade_segments(struct upgrade_ops *ops,
		struct segment_info *segment, unsigned char *data, struct part_info *part_info, int nrparts, bool last)
{
	int ret;
	uint64_t len;
	uint64_t seg_flash_offset;
	uint32_t ec_size = 0;

	static uint32_t *ec = NULL;
	static struct ubigen_info ui;
	static struct ubi_scan_info si;
	static int start_eb = 0;

	static struct mtd_info *mtd_upgrade = NULL;
	struct mtd_info *mtd_tmp = NULL;

	len = __le64_to_cpu(segment->size_decompress);
	seg_flash_offset = __le64_to_cpu(segment->flash_offset);

	/* the first entry, delete all partitions */
	if (!mtd_upgrade) {
		printf("before nand upgrade, delete all nand mtd partitions\n");
		ret = del_mtd_partitions(nand_mtd);
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
	mtd_tmp = get_mtd_device_off(seg_flash_offset);
	if (IS_ERR_OR_NULL(mtd_tmp)) {
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
		ret = mtd_create_partitions_nand(&part_info[ret], 1);
		if (ret)
			goto out_free;
		mtd_tmp = get_mtd_device_off(seg_flash_offset);
		if (IS_ERR_OR_NULL(mtd_tmp)) {
			printf("get the partition based on segment flash offset failed\n");
			goto out_free;
		}
	}

	printf("the partition mtd%d is found, flash offset = 0x%llx\n",
			mtd_tmp->index, seg_flash_offset);

	/* the first partition */
	if (!mtd_upgrade) {
		mtd_upgrade = mtd_tmp;
		start_eb = 0;
	}

	/*
	 * not the first partition, the second stage: format the remain blocks before scan
	 * the next partition
	 */
	if (mtd_upgrade && mtd_upgrade != mtd_tmp) {
		if (!part_is_rawimage(mtd_upgrade->name)) {
			printf("format the remain blocks before scan the next partition\n");
			/* format the remain erase blocks */
			ret = format(mtd_upgrade, &ui, &si, start_eb, 1);
			if (ret) {
				printf("format nand failed!!!\n");
				goto out_free;
			}
		}
		/* prepare for the next partition */
		mtd_upgrade = mtd_tmp;
		start_eb = 0;
	}

	/* a new partition, scan the whole partition */
	if (!start_eb) {
		if (ec)
			free(ec);
		ec_size = (mtd_upgrade->size / mtd_upgrade->erasesize) * sizeof(uint32_t);
		ec = malloc(ec_size);
		if (ec == NULL) {
			ec_size = 0;
		}

		memset(&ui, 0, sizeof(ui));
		memset(&si, 0, sizeof(si));
		memset(ec, 0, ec_size);
		ret = ubi_prepare(mtd_upgrade, &ui, &si, ec, ec_size);
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
	start_eb  = flash_image(mtd_upgrade, &ui, &si, (const char *)data, len, start_eb);
	if (start_eb < 0) {
		printf("flash image failed!!!\n");
		goto out_free;
	}

	if (last) {
		if (!part_is_rawimage(mtd_upgrade->name)) {
			printf("the last partition, format the remain blocks\n");
			/* format last partition remain erase blocks */
			ret = format(mtd_upgrade, &ui, &si, start_eb, 1);
			if (ret)
				goto out_free;
		}

		/* for boot upgrade test */
		start_eb = 0;
		mtd_upgrade = NULL;
		mtd_tmp = NULL;
		memset(&ui, 0, sizeof(ui));
		memset(&si, 0, sizeof(si));
		memset(ec, 0, ec_size);
		free(ec);
		ec = NULL;
		printf("nand upgrade success\n");
	}

	return 0;
out_free:
	if (ec) {
		free(ec);
		ec = NULL;
	}
	return ERR_PROGRAM;
}

static int nand_mtd_upgrade_gpt(struct upgrade_ops *ops,
			struct part_info *parts, int part)
{
	int ret;
	unsigned char *gpt;
	gpt_header *gpt_h;
	unsigned int start, size, gpt_size;

	start = nand_mtd_get_gpt0_offset();
	size = ops->usr_size - start;
	gpt = gpt_create(size, ops, parts, part);
	if (!gpt) {
		error("error %d\n", __LINE__);
		return ERR_OTHER;
	}

	gpt_size = nand_mtd->erasesize * NAND_PES_PER_GPT;
	if (higher_version == 0) {
		start += gpt_size;
		gpt_h = (gpt_header *)(gpt + FLASH_BLKSZ);
		gpt_prepare_backup_gpt_header(gpt_h);
	}
	if (higher_version == -1) {
		debug("Erase gpt1...\n");
		ret = nand_mtd_erase_common(nand_mtd, start + gpt_size, gpt_size);
		if (ret)
			return ret;
	}

	debug("Erase and Write gpt%d...\n", higher_version ? 0 : 1);
	ret = nand_mtd_write_common(nand_mtd, start, nand_mtd->erasesize * NAND_PES_PER_GPT, gpt, GPT_BUF_SZ);
	if (ret)
		return ret;

	/* only one config partition is ok */
	if (develop) {
		if (higher_version == -1)
			return ERR_SUCCESS;

		start = nand_mtd_get_gpt0_offset();
		if (higher_version == 1)
			start += gpt_size;

		debug("Erase gpt%d...\n", higher_version ? 1 : 0);
		ret = nand_mtd_erase_common(nand_mtd, start, gpt_size);
		if (ret)
			return ret;
	}

	return ERR_SUCCESS;
}

#endif /* end of CONFIG_MTD_SPI_NAND */

#ifdef CONFIG_SPI_FLASH
static int mtd_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	unsigned long long size;
	struct spi_flash *flash;

	if (uclass_get_device(UCLASS_SPI_FLASH, 0, &nor_dev)) {
		printf("\n                           ^*********^                    \n");
		printf("spi device init failed, please check your image or device!!!\n");
		printf("\n                           ^*********^                    \n");
		return ERR_FLASH_TYPE;
	}

	flash = dev_get_uclass_priv(nor_dev);
	ops->usr_size = flash->size;
	ops->erasesize = flash->erase_size;

	/* romcode + spl + config */
	size = NOR_ROMCODE_SIZE + 2 * SPL_SIZE + 2 * ops->gpt_size;
	if (ops->usr_size - size < partitions_size) {
		printf("\n                           ^*********^                                            \n");
		printf("partitions capacity: 0x%llx, available size  0x%llx (romcode, spl, config need 0x%llx)\n" \
				"but the corresponding image partitions total size: 0x%llx\n" \
				"partitions capacity is too small, please check your image!!!\n",
				ops->usr_size, ops->usr_size - size, size, partitions_size);
		printf("\n                           ^*********^                                            \n");
		return ERR_IMG_SIZE;
	}

	return ERR_SUCCESS;
}

int mtd_upgrade_erase(struct upgrade_ops *ops,
		struct part_info *parts)
{
	int ret;

	debug("upgrade erase partition %s, offset %llx, size %llx\n",
		parts->name,
		(unsigned long long)__le64_to_cpu(parts->flash_offset + NOR_GPT0_OFFSET),
		(unsigned long long)__le64_to_cpu(parts->length));

	ret = spi_flash_erase_dm(nor_dev, __le64_to_cpu(parts->flash_offset) + NOR_GPT0_OFFSET, __le64_to_cpu(parts->length));
	if(ret < 0) {
		error("error %d, %d\n", __LINE__, ret);
		return ERR_ERASE;
	}

	return ERR_SUCCESS;
}

static int mtd_upgrade_rom(struct upgrade_ops *ops,
		unsigned char *rom, size_t size)
{
	int ret;

	debug("Erase mtd romcode %s...\n", ops->disk_rom);
	ret = spi_flash_erase_dm(nor_dev, ROM_CODE_START_FLASH_OFS, size);
	if (ret < 0) {
		error("error %d, %d\n", __LINE__, ret);
		return ERR_ERASE;
	}

	debug("Write mtd romcode %s...\n", ops->disk_rom);
	ret = spi_flash_write_dm(nor_dev, ROM_CODE_START_FLASH_OFS, size, rom);
	if (ret < 0) {
		error("error %d, %d\n", __LINE__, ret);
		return ERR_PROGRAM;
	}

	return ERR_SUCCESS;
}

static int mtd_upgrade_bootloader(struct upgrade_ops *ops,
		unsigned char *bootloader, size_t size)
{
	int i, ret;
	unsigned int start;

	for (i = 0; i < 2; i++) {

		start = i == 0 ? NOR_SPL_OFFSET : (NOR_SPL_OFFSET + SPL_SIZE);
		start &= 0x0fffffff;

		debug("Erase mtd bootloader%d...\n", i);
		ret = spi_flash_erase_dm(nor_dev, start, size);
		if (ret < 0) {
			error("error %d, %d\n", __LINE__, ret);
			return ERR_ERASE;
		}

		debug("Write mtd bootloader%d...\n", i);
		ret = spi_flash_write_dm(nor_dev, start, size, bootloader);
		if (ret < 0) {
			error("error %d, %d\n", __LINE__, ret);
			return ERR_PROGRAM;
		}
	}

	return ERR_SUCCESS;
}

int mtd_upgrade_segments(struct upgrade_ops *ops,
		struct segment_info *segment, unsigned char *data)
{
	int ret;

	debug("Upgrade segment offs %llx, size %llx...\n",
		__le64_to_cpu(segment->flash_offset) + NOR_GPT0_OFFSET,
		__le64_to_cpu(segment->size_decompress));

	print_buf(data, 32);
	printf("\n");
	ret = spi_flash_write_dm(nor_dev, __le64_to_cpu(segment->flash_offset) + NOR_GPT0_OFFSET, __le64_to_cpu(segment->size_decompress), data);
	if(ret < 0) {
		error("error %d, %d\n", __LINE__, ret);
		return ERR_PROGRAM;
	}

	return ERR_SUCCESS;
}

/* develop only one gpt is ok */
static int mtd_develop_erase(struct upgrade_ops *ops)
{
	int ret;

	if (higher_version == 0) {
		debug("Erase gpt0...\n");
		ret = spi_flash_erase_dm(nor_dev, NOR_GPT0_OFFSET, ops->gpt_size);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	if (higher_version == 1) {
		debug("Erase gpt1...\n");
		ret = spi_flash_erase_dm(nor_dev, ops->usr_size - ops->gpt_size, ops->gpt_size);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static int mtd_erase_gpt(struct upgrade_ops *ops)
{
	int ret;

	if (higher_version == 1 || higher_version == -1) {
		debug("Erase gpt0...\n");
		ret = spi_flash_erase_dm(nor_dev, NOR_GPT0_OFFSET, ops->gpt_size);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	if (higher_version == 0 || higher_version == -1) {
		debug("Erase gpt1...\n");
		ret = spi_flash_erase_dm(nor_dev, ops->usr_size - ops->gpt_size, ops->gpt_size);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static int mtd_upgrade_gpt(struct upgrade_ops *ops, struct part_info *parts, int part)
{
	int ret;
	unsigned char *gpt;
	unsigned long long size;
	gpt_header		*gpt_h;
	gpt_entry		*gpt_e;

	ret = mtd_erase_gpt(ops);
	if (ret) {
		error("erase gpt fisrt failed\n");
		return ERR_OTHER;
	}

	size = ops->usr_size - (NOR_GPT0_OFFSET);
	gpt = gpt_create(size, ops, parts, part);
	if (!gpt) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	gpt_h = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_e = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	if (higher_version == 1 || higher_version == -1) {
		ret = spi_flash_write_dm(nor_dev, NOR_GPT0_OFFSET, GPT_BUF_SZ, gpt);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}
	}

	if (higher_version == 0) {
		unsigned int gpt1_offset;

		gpt_prepare_backup_gpt_header(gpt_h);

		gpt1_offset = (le64_to_cpu(gpt_h->last_usable_lba) + 1) * FLASH_BLKSZ + NOR_GPT0_OFFSET;
		/* backup gpt entry */
		ret = spi_flash_write_dm(nor_dev, gpt1_offset, gpt_h->num_partition_entries * sizeof(gpt_entry), gpt_e);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}

		/* backup gpt header */
		ret = spi_flash_write_dm(nor_dev, ops->usr_size - FLASH_BLKSZ, FLASH_BLKSZ, gpt_h);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}
	}

	if (develop) {
		ret = mtd_develop_erase(ops);
		if (ret)
			goto error_exit;
	}

	free(gpt);
	return ERR_SUCCESS;

error_exit:
	free(gpt);
	return ERR_PROGRAM;
}
#endif /* end of CONFIG_SPI_FLASH */

#ifdef CONFIG_MMC
static int mmc_upgrade_init(struct upgrade_ops *ops, struct upgrade_hdr *hdr)
{
	unsigned long long size;

	if ((mmc_dev = artosyn_emmc_init()) == NULL) {
		printf("\n                           ^*********^                    \n");
		printf("mmc device init failed, please check your image or device!!!\n");
		printf("\n                           ^*********^                    \n");
		return ERR_FLASH_TYPE;
	}

	ops->usr_size = mmc_dev->capacity;
	block_dev = mmc_get_blk_desc(mmc_dev);
	if (!block_dev) {
		printf("\n                           ^*********^                    \n");
		printf("mmc device init failed, please check your image or device!!!\n");
		printf("\n                           ^*********^                    \n");
		return ERR_FLASH_TYPE;
	}

	size = 2 * ops->gpt_size;
	if (ops->usr_size - size < partitions_size) {
		printf("\n                           ^*********^                              \n");
		printf("partitions capacity: 0x%llx, available size  0x%llx (config need 0x%llx)\n" \
				"but the corresponding image partitions total size: 0x%llx\n" \
				"partitions capacity is too small, please check your image!!!\n",
				ops->usr_size, ops->usr_size - size, size, partitions_size);
		printf("\n                           ^*********^                              \n");
		return ERR_IMG_SIZE;
	}

	return ERR_SUCCESS;
}

static int mmc_upgrade_erase(struct upgrade_ops *ops, struct part_info *parts)
{
	int ret;

	unsigned int blk_start;
	unsigned int blk_cnt;

	debug("upgrade erase partition %s, offset %llx, size %llx\n",
		parts->name,
		(unsigned long long)__le64_to_cpu(parts->flash_offset),
		(unsigned long long)__le64_to_cpu(parts->length));

	blk_start = __le64_to_cpu(parts->flash_offset) / block_dev->blksz;
	blk_cnt   = ALIGN(__le64_to_cpu(parts->length), block_dev->blksz) / block_dev->blksz;

	ret = blk_derase(block_dev, blk_start, blk_cnt);
	if(ret != blk_cnt) {
		error("error %d, %d\n", __LINE__, ret);
		return ERR_ERASE;
	}

	return ERR_SUCCESS;
}

static int mmc_upgrade_bootloader(struct upgrade_ops *ops,
		unsigned char *bootloader, size_t size)
{
	int i, ret;
	int part;
	unsigned int blk_size;

	blk_size = ALIGN(size / block_dev->blksz, mmc_dev->erase_grp_size);
	for (i = 0; i < 2; i++) {
		part = i == 0 ? MMC_PART_BOOT_AERA_1 : MMC_PART_BOOT_AERA_2;
		ret = blk_select_hwpart_devnum(IF_TYPE_MMC, 0, part);
		if (ret) {
			printf("fail select BOOT_AERA_1 \n");
			return ERR_OTHER;
		}

		debug("Erase mmc bootloader%d...\n", i);
		ret = blk_derase(block_dev, 0, blk_size);
		if (ret < 0) {
			error("error %d, %d\n", __LINE__, ret);
			return ERR_ERASE;
		}

		debug("Write mmc bootloader%d...\n", i);
		ret = blk_dwrite(block_dev, 0, blk_size, bootloader);
		if (ret < 0) {
			error("error %d, %d\n", __LINE__, ret);
			return ERR_PROGRAM;
		}
	}

	ret = blk_select_hwpart_devnum(IF_TYPE_MMC, 0, MMC_PART_UDA);
	if (ret) {
		printf("fail select BOOT_AERA_1 \n");
		return ERR_OTHER;
	}

	return ERR_SUCCESS;
}

static int mmc_upgrade_segments(struct upgrade_ops *ops,
		struct segment_info *segment, unsigned char *data)
{
	int ret;
	unsigned int blk_start;
	unsigned int blk_cnt;

	debug("Upgrade segment offs %llx, size %llx...\n",
		__le64_to_cpu(segment->flash_offset),
		__le64_to_cpu(segment->size_decompress));

	print_buf(data, 32);
	printf("\n");

	blk_start = __le64_to_cpu(segment->flash_offset) / block_dev->blksz;
	blk_cnt   = (ALIGN(segment->size_decompress, block_dev->blksz) / block_dev->blksz);
	ret = blk_dwrite(block_dev, blk_start, blk_cnt, data);
	if(ret != blk_cnt) {
		error("error %d, %d\n", __LINE__, ret);
		return ERR_PROGRAM;
	}

	return ERR_SUCCESS;
}

/* only one config partition is ok */
static int mmc_develop_erase(struct upgrade_ops *ops)
{
	int ret;
	lbaint_t gpt_blkcnt;

	/* gpt partition size */
	gpt_blkcnt = ops->gpt_size / block_dev->blksz;

	if (higher_version == 0) {
		debug("Erase gpt0...\n");
		ret = blk_derase(block_dev, 0, gpt_blkcnt);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	if (higher_version == 1) {
		debug("Erase gpt1...\n");
		ret = blk_derase(block_dev, block_dev->lba - gpt_blkcnt, gpt_blkcnt);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static int mmc_erase_gpt(struct upgrade_ops *ops)
{
	int ret;
	lbaint_t gpt_blkcnt;

	/* gpt partition size */
	gpt_blkcnt = ops->gpt_size / block_dev->blksz;

	if (higher_version == 1 || higher_version == -1) {
		debug("Erase gpt0...\n");
		ret = blk_derase(block_dev, 0, gpt_blkcnt);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	if (higher_version == 0 || higher_version == -1) {
		debug("Erase gpt1...\n");
		ret = blk_derase(block_dev, block_dev->lba - gpt_blkcnt, gpt_blkcnt);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			return ERR_ERASE;
		}
	}

	return ERR_SUCCESS;
}

static int mmc_upgrade_gpt(struct upgrade_ops *ops,
			struct part_info *parts, int part)
{
	int pte_blk_cnt, blk_cnt, ret;
	unsigned char *gpt;
	unsigned long long size;
	gpt_header		*gpt_h;
	gpt_entry		*gpt_e;

	ret = mmc_erase_gpt(ops);
	if (ret) {
		error("erase gpt fisrt failed\n");
		return ERR_OTHER;
	}

	blk_cnt = ALIGN(GPT_BUF_SZ, block_dev->blksz) / block_dev->blksz;
	size = block_dev->lba + ((nand_mtd==NULL)? 0 : (nand_mtd->size/block_dev->blksz));
	size *= block_dev->blksz;

	gpt = gpt_create(size, ops, parts, part);
	if (!gpt) {
		error("error %d\n", __LINE__);
		return ERR_PROGRAM;
	}

	gpt_h = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_e = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	pte_blk_cnt = gpt_h->num_partition_entries
					   * sizeof(gpt_entry)
					   / block_dev->blksz;

	if (higher_version == 1 || higher_version == -1) {
		ret = blk_dwrite(block_dev, 0, blk_cnt, gpt);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}
	}

	if (higher_version == 0) {
		gpt_prepare_backup_gpt_header(gpt_h);

		/* backup gpt entry */
		ret = blk_dwrite(block_dev, (lbaint_t)le64_to_cpu(gpt_h->last_usable_lba)
				   + 1, pte_blk_cnt, gpt_e);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}

		/* backup gpt header */
		ret = blk_dwrite(block_dev, (lbaint_t)le64_to_cpu(gpt_h->my_lba), 1, gpt_h);
		if (ret < 0) {
			error("error %d\n", __LINE__);
			goto error_exit;
		}
	}

	if (develop) {
		ret = mmc_develop_erase(ops);
		if (ret)
			goto error_exit;
	}

	free(gpt);
	return ERR_SUCCESS;

error_exit:
	free(gpt);
	return ERR_PROGRAM;
}
#endif /* end of CONFIG_MMC */

#ifdef CONFIG_SPI_FLASH
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
#endif

#ifdef CONFIG_MTD_SPI_NAND
static struct upgrade_ops nand_upgrade_ops = {
	.type = (unsigned char *)"nand",
	.disk_rom   = (unsigned char *)disk_rom,
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
#endif

#ifdef CONFIG_MMC
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
#endif

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
#ifndef CONFIG_SPL_BUILD
	unsigned char *hash, *data, *sig;
#endif
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

	/* Verify header */
	hdr = (struct upgrade_hdr *)img;

	if (__le32_to_cpu(hdr->magic) != MAGIC ||
		__le32_to_cpu(hdr->hash_size) != 32 ||
		__le32_to_cpu(hdr->sig_size) != 256) {
		error("error %d\n", __LINE__);
		return -1;
	}

#ifndef CONFIG_SPL_BUILD
	hash = img + sizeof(*hdr);
	sig  = hash + 32;
	data = sig + 256;
#ifdef CONFIG_ARTOSYN_AR9301
	ret = spl_verify_sw("upgrade", hash, 32, sig, 256, data, (unsigned long)(__le64_to_cpu(hdr->img_size) & 0xffffffff));
#endif
#endif
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

#ifdef CONFIG_SPI_FLASH
	if (__le32_to_cpu(hdr->rom_size) && __le32_to_cpu(hdr->rom_size) > NOR_ROMCODE_SIZE) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}
#endif

#ifdef CONFIG_MTD_SPI_NAND
	if (__le32_to_cpu(hdr->loader_size) && __le32_to_cpu(hdr->loader_size) > NAND_SPL_PAGES * nand_mtd->writesize) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}
#else
	if (__le32_to_cpu(hdr->loader_size) && __le32_to_cpu(hdr->loader_size) > SPL_SIZE) {
		debug("error %d\n", __LINE__);
		return ERR_VERIFY;
	}
#endif

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

	loff_t offset = UPGRADE_UNIT;
	unsigned int tail;

	decompress_buf = (unsigned char *)IMAGE_DECOMPRESS_ADDR;
	info = seg_info;
	tmp = NULL;
	upgrade_part = NULL;
	upgrade_offset = 0;
	n_bak = 0;
	n = __le16_to_cpu(hdr->segments);

	for(i = 0; i < n; i++) {
		/*
		 * only nor or mmc erase here, nand need scan before erasing,
		 * so do it solo
		 */
		if (strcmp((const char *)ops->type, "nand")) {
			/* find the partition based on segment flash_offset */
			index = get_part_info_offset(part_info, __le16_to_cpu(hdr->partitions), info);
			if (index < 0) {
				error("get part info based on segment flash offset failed\n");
				return ERR_PROGRAM;
			}
			tmp = &part_info[index];

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
			/* find the partition based on segment flash_offset */
			index = get_part_info_offset(part_info, hdr->partitions, info);
			if (index < 0) {
				error("get part info based on segment flash offset failed\n");
				return ERR_PROGRAM;
			}

			tmp = &part_info[index];

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
				error("error %d, ret = %d, decompress_len = 0x%lx, size_decompress = 0x%llx\n",
                                      __LINE__, ret, decompress_len, __le64_to_cpu(info->size_decompress));
				goto error;
			}

			if (strcmp((const char *)ops->type, "nand"))
				ret = ops->upgrade_segments(ops, info, decompress_buf);
			else {
				if (i == n-1)
					ret = ops->upgrade_segments_nand(ops, info, decompress_buf, part_info, hdr->partitions, true);
				else
					ret = ops->upgrade_segments_nand(ops, info, decompress_buf, part_info, hdr->partitions, false);
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
		if (!single_upgrade && offset < __le64_to_cpu(hdr->img_size) + 32 + 256 + sizeof(struct upgrade_hdr)) {
			loff_t act_read = 0;

			if (offset == UPGRADE_UNIT)
				tail = IMAGE_LOAD_ADDR + UPGRADE_UNIT - (unsigned long)compressed_buf;
			else
				tail = IMAGE_LOAD_ADDR + (UPGRADE_UNIT << 1) - (unsigned long)compressed_buf;


#define TAIL_MAX 	(0x500000)
			if (tail < TAIL_MAX) {
				unsigned char *ptmp;

				if (offset != UPGRADE_UNIT) {
					ptmp = compressed_buf;
					compressed_buf = (unsigned char *)(unsigned long long)(IMAGE_LOAD_ADDR + UPGRADE_UNIT - tail);
					memcpy(compressed_buf, ptmp, tail);
				}

				ret = fs_set_usb_dev();
				if (ret)
					return ret;

				/* use memory in IMAGE_LOAD_ADDR + UPGRADE_UNIT, so the image in the
				 * front UPGRADE_UNIT size is no change
				 */
				if (offset + UPGRADE_UNIT < __le64_to_cpu(hdr->img_size) + 32 + 256 + sizeof(struct upgrade_hdr))
					ret = fs_read(upgradefilename, IMAGE_LOAD_ADDR + UPGRADE_UNIT, offset, UPGRADE_UNIT, &act_read);
				else
					ret = fs_read(upgradefilename, IMAGE_LOAD_ADDR + UPGRADE_UNIT, offset, 0, &act_read);
				if (ret) {
					printf("boot_upgrade: read file fail %s", upgradefilename);
					return -1;
				} else
					printf("boot_upgrade: act_read=0x%llx \n", act_read);

				offset += UPGRADE_UNIT;
			}
		}
	}

	return ERR_SUCCESS;
error:
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
#ifndef CONFIG_SPL_BUILD
	char buf[512] = {0};
	char params[10][64] = {0};
	char *p_env, *p_tmp;
	unsigned char *endptr;
	int i = 0;
#endif
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

#ifndef CONFIG_SPL_BUILD
	p_env = env_get("bootargs");
	strcpy(buf, p_env);

	// get DDR config in ENV
	p_tmp = strtok(buf, " ");
	while (p_tmp != NULL && i < 10) {
		strcpy(params[i], p_tmp);
		p_tmp = strtok(NULL, " ");
		i++;
	}
	for (i = 0; i < 10; i++) {
		p_tmp = strtok(params[i], "=");
		if (p_tmp != NULL) {
			if (strcmp(p_tmp, "ddr_type") == 0) {
				p_tmp = strtok(NULL, "=");
				old_config[0] = simple_strtoul(p_tmp, (char **)&endptr, 10);
			} else if (strcmp(p_tmp, "ddr_freq") == 0) {
				p_tmp = strtok(NULL, "=");
				old_config[1] = simple_strtoul(p_tmp, (char **)&endptr, 10);
			} else if (strcmp(p_tmp, "ddr_width") == 0) {
				p_tmp = strtok(NULL, "=");
				old_config[2] = simple_strtoul(p_tmp, (char **)&endptr, 10);
			}
		}
	}
#else
	/* get DDR setting from current DTB */
	fdt_blob = (void *)gd->fdt_blob;
	if (fdt_magic(fdt_blob) != FDT_MAGIC) {
		printf("Old fdt_magic wrong %x, skip verify DDR\n",
		fdt_magic(fdt_blob));
		return 0;
	}
	dmc_node = fdt_node_offset_by_compatible(fdt_blob, 0, "artosyn,sirius-dmc");
	if (dmc_node <= 0) {
		printf("Get dmc node from old imgae fail %p\n, skip verify DDR",
			fdt_blob);

		return 0;
	}

	memset(old_config, 0, ARRAY_SIZE(old_config));
	params_cnt = fdtdec_get_int_array_count(fdt_blob, dmc_node,
				"dmc-config", (unsigned int *)old_config, ARRAY_SIZE(old_config));

	if (params_cnt % 3 != 0) {
		printf("Old dmc-config data length %d error, skip verify DDR\n",
				params_cnt);
		return 0;
	}
#endif
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

int do_upgrade(unsigned char *img, ar_flash_cb cb)
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
#ifdef CONFIG_MMC
	case FLASH_TYPE_MMC:
		debug("Flash type is mmc\n");
		ops = &mmc_upgrade_ops;
		break;
#endif
#ifdef CONFIG_SPI_FLASH
	case FLASH_TYPE_SPINOR:
		debug("Flash type is nor\n");
		ops = &nor_upgrade_ops;
		break;
#endif
#ifdef CONFIG_MTD_SPI_NAND
	case FLASH_TYPE_SPINAND:
		debug("Flash type is nand\n");
		ops = &nand_upgrade_ops;
		break;
#endif
	default:
		error("error %d, flash type in the image is wrong\n", __LINE__);
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

/*
 * the below is the upgrade prepare, the boot and ota is different.
 * boot gets the image from the udisk, but ota from the command parameter.
 */

static int fs_set_usb_dev(void)
{
	int ret, i;

	ret = fs_set_blk_dev("usb", "0", FS_TYPE_FAT);
	if (ret) {
		for (i = 0; ; ++i) {
			char cmd_string[10] = {""};
			sprintf(cmd_string, "0:%d", i + 1);
			ret = fs_set_blk_dev("usb", cmd_string, FS_TYPE_FAT);
			if (ret) {
				printf("spl_upgrade: error fs_set_blk_dev partion: %d\n", i + 1);
				if (i  >= 3)
					return -1;
			} else
				break;
		}
	}

	return ret;
}

static int do_upgrade_dev_init(int if_type)
{
	int ret = 0;

	if (if_type == IF_TYPE_USB) {
		usb_stop();

		/* usb start */
		if ((ret = usb_init()) < 0) {
			printf("Error: usb_init fail = %d", ret);
			return -1;
		}

		/* try to recognize storage devices immediately */
		usb_stor_curr_dev = usb_stor_scan(1);
		stor_dev = blk_get_devnum_by_type(IF_TYPE_USB, usb_stor_curr_dev);
		if (!stor_dev) {
			printf("Error: USB storage device not found\n");
			return -1;
		}

		ret = fs_set_usb_dev();
		if (ret)
			return ret;

		return 0;
	} else {
		/* Fixme */
		printf("Error: not support if_type");
	}

	return -1;
}

static loff_t do_upgrade_load_img(void)
{
	int    ret = 0;
	loff_t act_read = 0;
	int    find = 0;
	int filecnt, dircnt;
	char * filename;
	int cnt = 0;
	loff_t filesize;
	phys_size_t size = 0;
	phys_size_t start = 0xFFFFFFFF;
	struct upgrade_hdr *hdr;

	ret = fs_set_usb_dev();
	if (ret)
		return ret;

	ret = fs_ls(ARTOSYN_UPGRADE_FILE_FOLDER);
	char * filenames = (char *)do_fat_getfnamecopy(&filecnt, &dircnt);
	if (filecnt < 0) {
		printf("not find file = %d\n", filecnt);
		return -1;
	}

	/* find the filename */
	filename = filenames;

	printf("%d %d %d %d %d \n", filenames[0], filenames[1], filenames[2], filenames[3], filenames[4]);
	while (cnt < filecnt + dircnt && filename - filenames < 1023) {
		int len;
		while ((*filename) == 0 && filename - filenames < 1023) {
			filename ++;
		}
		cnt ++;

		len = strlen(filename);
		#if defined CONFIG_ARTOSYN_AR9301
		if (0 == memcmp(filename, "artosyn-upgrade-ar9341", strlen("artosyn-upgrade-ar9341")-1)
				&& len <= sizeof(upgradefilename) -1) {
		#elif defined CONFIG_ARTOSYN_AR9311
		if (0 == memcmp(filename, "artosyn-upgrade-ar9311", strlen("artosyn-upgrade-ar9311")-1)
				&& len <= sizeof(upgradefilename) -1) {
		#else
		if (0 == memcmp(filename, "artosyn-upgrade-ar93", strlen("artosyn-upgrade-ar93")-1)
				&& len <= sizeof(upgradefilename) -1) {
		#endif
			if (filename[len - 4] == '.' && filename[len - 3] == 'i'
					&& filename[len - 2] == 'm' && filename[len - 1] == 'g') {
				filename[len] = 0;
				find = 1;
				strcpy(upgradefilename, filename);
				printf("i = %d filename = %s \n", cnt, filename);
				break;
			}
		}

		/* move to another filename */
		filename += (len + 1);
	}

	if (find == 0) {
		printf("no upgrade file find \n");
		return -1;
	}

	ret = fs_set_usb_dev();
	if (ret)
		return ret;

	start = 0x20000000;

	if (gd->ram_size) {
		printf("Get ram size from gd: 0x%llx \n", gd->ram_size);
		size = gd->ram_size;
	} else {
		printf("Use default ram size 0x40000000\n");
		size = 0x40000000;
	}

	ret = fs_size(upgradefilename, &filesize);
	if (ret) {
		printf("boot_upgrade: read file size fail %s\n", filename);
		return -1;
	} else
		printf("boot_upgrade: filesize = %#llx\n", filesize);

	ret = fs_set_usb_dev();
	if (ret)
		return ret;

	/* if ddr size is enough, upgrade all directly */
	if (filesize > size - (IMAGE_LOAD_ADDR - start)) {
		/* read 128M once */
		ret = fs_read(upgradefilename, IMAGE_LOAD_ADDR, 0, UPGRADE_UNIT, &act_read);
		if (ret) {
			printf("boot_upgrade: read file fail %s\n", filename);
			return -1;
		} else
			printf("boot_upgrade: act_read=0x%llx \n", act_read);

		single_upgrade = false;
	} else {
		/* Perfrom file read, read all */
		ret = fs_read(upgradefilename, IMAGE_LOAD_ADDR, 0, 0, &act_read);
		if (ret) {
			printf("boot_upgrade: read file fail %s\n", filename);
			return -1;
		} else
			printf("boot_upgrade: act_read=0x%llx \n", act_read);

		single_upgrade = true;
	}

	/* check image size */
	hdr = (struct upgrade_hdr *)IMAGE_LOAD_ADDR;
	if (filesize - sizeof(struct upgrade_hdr) - 32 - 256 != __le64_to_cpu(hdr->img_size)) {
		printf("image file size error, filesize : %#llx, but image size in header : %#llx\n",
				filesize, __le64_to_cpu(hdr->img_size));
		return -1;
	}

	return act_read;
}

#ifdef CHECK_CONSOLE
static int check_console = CHECK_CONSOLE;
#else
static int check_console = 1;
#endif

int artosyn_spl_upgrade(void)
{
	int ret = 0;
	int c = 0;

	/* Check conole inuput, run upgrade only if input is 'u' */
	if (check_console) {
		if (!tstc())
			return 0;

		c = getc();
#ifndef CONFIG_SPL_BUILD
		if (c == 'u' || c == 'U')
			develop = 1;
		else if (c == 'p')
			develop = 0;
		else
			return 0;
#else
		if (c == 's' || c == 'S')
			develop = 1;
		else if (c == 'P')
			develop = 0;
		else
			return 0;
#endif
		mdelay(2000);

		while (tstc())
			(void)getc();
	}

	printf("Entering upgrade...");

	/* init device */
	ret = do_upgrade_dev_init(IF_TYPE_USB);
	if (ret < 0)
		goto fail;

	/* load img */
	ret = do_upgrade_load_img();
	if (ret < 0)
		goto fail;

do_upgrade:
	ret = do_upgrade((unsigned char *)IMAGE_LOAD_ADDR, NULL);
	if (ret == ERR_DDR && check_console) {
		/* Check conole inuput, force upgrade only if input is 'f' */
		printf("DDR change!!! reset or press 'f' to force upgrade......\n");
		while (tstc())
			(void)getc();
		do {
			if (tstc()) {
				c = getc();
				if (c == 'f' || c == 'F') {
					printf("Force upgrade......\n");
					break;
				}
			}
			mdelay(500);
		} while(true);
		artosyn_set_force_upgrade(true);
		goto do_upgrade;
	}
	artosyn_set_force_upgrade(false);
	if (ret < 0)
		goto fail;

	printf("%s upgrade finish, please reset the board...\n", c == 'u' || c == 'U' || c == 'D' ? "uboot" : "spl");
	while(1);
fail:
	printf("%s upgrade failed, please check your image or udisk and upgrade again...\n", c == 'u' || c == 'U' || c == 'D' ? "uboot" : "spl");

	return 0;
}

#ifndef CONFIG_SPL_BUILD
int initr_sirius_upgrade(void)
{
	artosyn_spl_upgrade();
	return 0;
}

static int do_artosyn_upgrade(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	bool test = false;
	int ret;

	artosyn_set_force_upgrade(false);
	if (argc == 2 && !strcmp(argv[1], "test")) {
		printf("\nuboot upgrade test start\n\n");
		test = true;
	} else if (argc == 2 && !strcmp(argv[1], "force")) {
		printf("\nuboot force upgrade\n");
		artosyn_set_force_upgrade(true);
	} else if (argc == 2 && !strcmp(argv[1], "dram")) {
		printf("\nuboot upgrade from ddr\n");
		set_single_upgrade(true);
		ret = do_upgrade((unsigned char *)IMAGE_LOAD_ADDR, NULL);
		if (ret)
			printf("uboot upgrade failed, please check your image or udisk and upgrade again...\n");
		else
			printf("uboot upgrade finish, please reset the board...\n");

		return 0;
	} else if (argc > 1)
		goto usage;

	check_console = 0;
	do {
		ret = artosyn_spl_upgrade();
		if (ret)
			break;
		if(ctrlc()) {
			printf("\nuboot upgrade test exit\n");
			break;
		}
	} while (test);
	check_console = 1;
	return 0;
usage:
	return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char artosyn_upgrade_help_text[] =
	"- no parameters, upgrade\n"
	"artosyn_upgrade test - upgrade test, CTRLC exit\n"
	"artosyn_upgrade force - force upgrade if DDR change\n"
	"artosyn_upgrade dram - upgrade from dram, you should load image to "STR(IMAGE_LOAD_ADDR)" before upgrade\n"
	"";
#endif

U_BOOT_CMD(
	artosyn_upgrade,     2,      1,      do_artosyn_upgrade,
	"artosyn upgrade", artosyn_upgrade_help_text
);
#endif
