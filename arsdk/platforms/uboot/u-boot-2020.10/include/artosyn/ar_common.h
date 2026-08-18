#ifndef __AR9301_COMMON_H
#define __AR9301_COMMON_H

#include <part_efi.h>
#include <part.h>
#include <u-boot/crc.h>
#include <uboot_upgrade.h>
#include <linux/mtd/mtd.h>

/*
 * spl sram layout:
 * | spl base 		| 0x17c0000
 * | cpu bootinfo   | 0x17c0000
 * | spl header     | 0x17c0040
 * | romcode(data + | 0x17c1000
 * | bss + stack)   |
 * | troot firmware | 0x17d0000
 * | sign key 		| 0x17e9800
 * | spl(text + 	| 0x17e9c00
 * | data) 			|
 * | spl dtb 		| 0x17ff400
 * | sram top 		| 0x1800000
 *
 * 0x17d0000 - 0x17e9bff (troot firmware + sign key) is reused in spl:
 * | spl bss 		| 0x17d0000
 * | spl bss limit 	| 0x17d1000
 * | gap 			|
 * | spl stack 		|
 * | spl gd (0x170) | 0x17d9a80
 * | spl heap 		| 0x17d9bf0
 * | spl heap limit | 0x17e9bf0
 *
 * uboot ddr layout:
 * | ddr base       | 0x20000000
 * | kernel         | 0x200a0000
 * | kernel dtb     | 0x22080000
 * | logo           | 0x22400000
 * | bl31           | 0x23400000
 * | console        | 0x24000000  4KB
 * | upgrade decomp | 0x24400000  the same with compressed kernel load address
 * | upgrade image  | 0x24900000
 * | gap            |
 * | uboot heap     | about ddr top - 17M
 * | uboot relocate | about ddr top - 1M
 * */

/* ahb dma cannot access secure sram, so relocate some address to ddr */
#define SPL_GPT0_ADDR 			(IMAGE_DECOMPRESS_ADDR)
#define SPL_GPT1_ADDR 			(IMAGE_DECOMPRESS_ADDR + 0x10000)
#define SPL_NAND_DATABUF_ADDR 	(IMAGE_DECOMPRESS_ADDR + 0x10000)

#define KERNEL_COMP_SIZE 		0x800000
#define KERNEL_LOAD_ADDR 		IMAGE_DECOMPRESS_ADDR
#define KERNEL_DTB_LOAD_ADDR 	0x22080000

enum config_filetype {
	CFG_FILETYPE_PARTOITION,
	CFG_FILETYPE_DDR_INIT,
	CFG_FILETYPE_PINSHARE,
};

#define ARTOSYN_MAGIC     0x41529301

struct config_header {
	unsigned int magic; /* AR9301 */
	unsigned char version;
	unsigned char file_num;
	unsigned char rsv[2];
	struct file_info {
		unsigned char file_type;
		unsigned short file_offset;
		unsigned short file_len;
		unsigned int file_checksum;
	} file_info[16];
	unsigned int total_lenght;
	unsigned int header_checksum;
} __attribute__ ((aligned(1)));

#define SPL_SIZE 	 	0x40000 /* nor and emmc */

#ifdef CONFIG_MTD_SPI_NAND
#define NAND_SPL_PAGES 		128
#define NAND_SPL_PARTS 		4
#define NAND_PES_PER_GPT 	2

struct mtd_info *artosyn_qspi_nand_init(void);
#endif

#ifdef CONFIG_SPI_FLASH
#define NOR_ROMCODE_SIZE 	0x10000
#define NOR_SPL_OFFSET 		(NOR_ROMCODE_SIZE)
#define NOR_GPT0_OFFSET 	(NOR_SPL_OFFSET + SPL_SIZE * 2)

struct udevice *artosyn_qspi_nor_init(void);
#endif

#ifdef CONFIG_MMC
struct mmc *artosyn_emmc_init(void);
#endif

static inline u32 efi_crc32(const void *buf, u32 len)
{
	return crc32(0, buf, len);
}

extern struct mtd_info *nand_mtd;
extern struct udevice *nor_dev;
extern struct blk_desc *block_dev;

void print_buf(unsigned char *buf, int len);
int artosyn_device_init(void);
int get_higher_version_gpt(unsigned int *version, unsigned char **gpt, int *which);
int artosyn_find_valid_partition(char *name, gpt_header *gpt_head, gpt_entry *gpt_pte, struct disk_partition *part, int *which);
int artosyn_get_part_valid_info(gpt_header *gpt_head, gpt_entry *gpt_pte, unsigned long long *part_info);
int gpt_validate_entries(gpt_header *gpt_h, gpt_entry *gpt_e);
int gpt_validate_header(gpt_header *gpt_h, lbaint_t lba, lbaint_t lastlba);
unsigned int nand_mtd_get_gpt0_offset(void);
int current_gpt_check(struct upgrade_ops *ops, int *gpt0_ok, int *gpt1_ok, unsigned char **gpt0, unsigned char **gpt1);
int spinand_flash_read(struct mtd_info *mtd, unsigned int offset, unsigned int size, unsigned char *addr);
int nand_mtd_write_common(struct mtd_info *mtd, unsigned int offset, size_t part_size, unsigned char *data, size_t size);
int spinand_write_env(struct mtd_info *mtd, int offset, char *data, int size);
int mtdparts_generate(char *mtdparts);
int spinor_read(void *buf, unsigned int offset, unsigned int size);
int spinand_read(void *buf, unsigned int offset, unsigned int size);

#endif
