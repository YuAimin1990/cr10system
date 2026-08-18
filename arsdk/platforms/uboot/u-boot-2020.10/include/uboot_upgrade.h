#ifndef _SPL_UPGRADE_H
#define _SPL_UPGRADE_H

#define ARTOSYN_UPGRADE_FILE_FOLDER 	("/")

/* "ARTO" */
#define 	MAGIC 					(0x4152544f)
#define 	IMAGE_LOAD_ADDR 		(IMAGE_DECOMPRESS_ADDR + COMPR_SEG_SIZE) /* temp for image space when uboot upgrade */
#define 	IMAGE_DECOMPRESS_ADDR	(0x24400000) /* address for uboot to decompress img */

#define 	COMPR_SEG_SIZE 			(0x500000) 	/* 5M */

#define	ROM_CODE_START_FLASH_OFS	(0x0)

#define	FLASH_SHIFT 				(9)
#define FLASH_BLKSZ 				(512)
#define	GPT_BUF_SZ 					(34 * FLASH_BLKSZ)

#define SDK_VERSION_SIZE 		128
#define PART_NUM_MAX 			64

/* FIXME, 64 is enough now */
#define 	AR_PART_MAX_BOOTARGS 	(PART_NUM_MAX + 1)

/* the MSB in gpt entry field reserve means whether this partition is valid */
#define PARTITION_VALID_SHIFT 	44
#define PARTITION_VALID_MASK 	(1ULL << PARTITION_VALID_SHIFT)

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

int artosyn_spl_upgrade(void);
int initr_sirius_upgrade(void);

#define MMC_PART_UDA			(0)
#define MMC_PART_BOOT_AERA_1	(1)
#define MMC_PART_BOOT_AERA_2	(2)

#define UPGRADE_UNIT 	(0x8000000) /* 128M */

#endif
