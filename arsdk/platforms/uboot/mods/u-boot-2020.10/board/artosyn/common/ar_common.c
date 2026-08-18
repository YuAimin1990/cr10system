#include <asm/io.h>
#include <dm.h>
#include <mmc.h>
#include <mtd.h>
#include <spi_flash.h>
#include <asm/arch/boot.h>
#include <uboot_upgrade.h>
#include <artosyn/ar_common.h>
#include <part_efi.h>
#include <memalign.h>
#include <part.h>
#include <common.h>

extern int part_get_info_efi_table(int part, struct disk_partition *info, gpt_header *gpt_head, gpt_entry *gpt_pte);
extern int is_gpt_valid(struct blk_desc *dev_desc, u64 lba,
				gpt_header *pgpt_head, gpt_entry **pgpt_pte);
static int boot_mode;
struct mtd_info *nand_mtd;
struct udevice *nor_dev;
struct blk_desc *block_dev;

/* the code in the boot and ota is the same */
void print_buf(unsigned char *buf, int len)
{
	int i = 0;

	for(i = 0; i < len; i++) {
		printf("%02x ", buf[i]);
		if((i + 1) % 16 == 0 || i == len - 1)
			printf("\n");
	}
}

#ifdef CONFIG_MMC
struct mmc *artosyn_emmc_init(void)
{
	struct mmc *mmcp = NULL;
	int ret = 0;

	ret = mmc_init_device(0);
	if (ret) {
		printf("spl: could not initialize mmc. ret: %d\n", ret);
		return NULL;
	}
	mmcp = find_mmc_device(0);
	ret = mmcp ? 0 : -ENODEV;
	if (ret) {
		printf("spl: could not find mmc dev 0. ret: %d\n", ret);
		return NULL;
	}

	ret = mmc_init(mmcp);
	if (ret) {
		mmcp = NULL;
		printf("spl: mmc init failed with ret %d!\n", ret);
		return NULL;
	}

	return mmcp;
}
#endif

#if defined(CONFIG_SPI_FLASH) || defined(CONFIG_SPL_SPI_LOAD)
struct udevice *artosyn_qspi_nor_init(void)
{
	struct udevice *dev = NULL;

	if ((CONFIG_IS_ENABLED(MTD) && CONFIG_IS_ENABLED(SPI_FLASH))
			|| CONFIG_IS_ENABLED(DM_SPI_FLASH)) {
		if (uclass_get_device(UCLASS_SPI_FLASH, 0, &dev)) {
			pr_err("spi flash probe failed\n");
			return NULL;
		}
	}

	return dev;
}
#endif

#ifdef CONFIG_MTD_SPI_NAND
struct mtd_info *artosyn_qspi_nand_init(void)
{
	struct mtd_info *mtd = NULL;

	/* Ensure all devices (and their partitions) are probed */
	mtd_probe_devices();

	mtd_for_each_device(mtd) {
		if (!mtd_is_partition(mtd) && mtd->type == MTD_NANDFLASH) {
			break;
		}
	}

	if (mtd == NULL) {
		printf("qspi nand init failed\n");
	}

	return mtd;
}
#endif

/* init qspi nor/qspi nand/emmc */
int artosyn_device_init(void)
{
	static int initd = 0;

	if (initd)
		return 0;

	boot_mode = get_boot_device();
#ifdef CONFIG_SPI_FLASH
#ifdef CONFIG_ARTOSYN_AR9301
	if (boot_mode == BOOT_VALUE_ROM_QSPI_NOR
			|| boot_mode == BOOT_VALUE_ROM_AUTO
			|| boot_mode == BOOT_VALUE_ROM_AUTO_BYPASS_PLL) {
#elif defined(CONFIG_ARTOSYN_AR9311)
	if (boot_mode == BOOT_VALUE_ROM_QSPI_NOR_1V8
			|| boot_mode == BOOT_VALUE_ROM_QSPI_NOR_3V3) {
#endif
		nor_dev = artosyn_qspi_nor_init();
		if (!nor_dev)
			return -ENODEV;
		goto out;
	}
#endif

#ifdef CONFIG_MTD_SPI_NAND
#ifdef CONFIG_ARTOSYN_AR9301
	if (boot_mode == BOOT_VALUE_ROM_QSPI_NAND
			|| boot_mode == BOOT_VALUE_ROM_AUTO
			|| boot_mode == BOOT_VALUE_ROM_AUTO_BYPASS_PLL) {
#elif defined(CONFIG_ARTOSYN_AR9311)
	if (boot_mode == BOOT_VALUE_ROM_QSPI_NAND_1V8
			|| boot_mode == BOOT_VALUE_ROM_QSPI_NAND_3V3) {
#endif
		nand_mtd = artosyn_qspi_nand_init();
		if (!nand_mtd)
			return -ENODEV;
		goto out;
	}
#endif

#ifdef CONFIG_MMC
#ifdef CONFIG_ARTOSYN_AR9301
	if (boot_mode == BOOT_VALUE_ROM_EMMC
			|| boot_mode == BOOT_VALUE_ROM_AUTO
			|| boot_mode == BOOT_VALUE_ROM_AUTO_BYPASS_PLL) {
#elif defined(CONFIG_ARTOSYN_AR9311)
	if (boot_mode == BOOT_VALUE_ROM_EMMC) {
#endif
		struct mmc *mmcp;
		mmcp = artosyn_emmc_init();
		if (mmcp)
			block_dev = mmc_get_blk_desc(mmcp);
		else
			return -ENODEV;
		goto out;
	}
#endif
out:
	initd = 1;

	return 0;
}

int get_higher_version_gpt(unsigned int *version, unsigned char **gpt, int *which)
{
	unsigned char *gpt0, *gpt1;
	gpt_entry *gpt_pte0, *gpt_pte1;
	gpt_header *gpt_head0, *gpt_head1;
	int ret0, ret1, ret;
	static int getted = 0;
	static unsigned int version_getted;
	static unsigned char *gpt_getted;
	static int which_getted;

	if (getted) {
		if (version)
			*version = version_getted;
		*gpt = gpt_getted;
		if (which)
			*which = which_getted;

		return 0;
	}

	ret = artosyn_device_init();
	if (ret) {
		printf("device init failed\n");
		return ret;
	}
	ret = current_gpt_check(NULL, &ret0, &ret1, &gpt0, &gpt1);
	if (ret < 0) {
		printf("current gpt check failed\n");
		return ret;
	}

	gpt_head0   = (gpt_header *)(gpt0 + FLASH_BLKSZ);
	gpt_head1   = (gpt_header *)(gpt1 + FLASH_BLKSZ);
	gpt_pte0 = (gpt_entry  *)(gpt0 + FLASH_BLKSZ * 2);
	gpt_pte1 = (gpt_entry  *)(gpt1 + FLASH_BLKSZ * 2);

	/* both ok, compare version */
	if (ret0 == 1 && ret1 == 1) {
		/* use gpt0 */
		if (__le32_to_cpu(gpt_head0->reserved1) >= __le32_to_cpu(gpt_head1->reserved1))
			ret = 0;
		else
			ret = 1;
		goto out;
	} else if (ret0 == 1 && ret1 != 1) {
		ret = 0; /* gpt0 */
		goto out;
	} else if (ret0 != 1 && ret1 == 1) {
		ret = 1; /* gpt1 */
		goto out;
	} else {
		ret = -1; /* error */
		goto out;
	}

out:
	/* return version */
	if(ret == 0) {
		if (version)
			*version = __le32_to_cpu(gpt_head0->reserved1);
		*gpt = gpt0;
		free(gpt1);
	} else if (ret == 1) {
		if (version)
			*version = __le32_to_cpu(gpt_head1->reserved1);
		*gpt = gpt1;
		free(gpt0);
	} else {
		return -1;
		if (version)
			*version = 0; /* gpt_pte has been freed while check gpt failed */
	}

	if (which)
		*which = ret;

	if (ret >=0) {
		getted = 1;
		version_getted = *version;
		gpt_getted = *gpt;
		which_getted = *which;
	}

	return 0;
}

/*
 * return value: > 0, part number, otherwise error
 */
int artosyn_find_valid_partition(char *name, gpt_header *gpt_head, gpt_entry *gpt_pte, struct disk_partition *part, int *which)
{
	int i, ret;
	unsigned int num_partition_entries;
	struct disk_partition info;
	char buf[16] = {0};

	num_partition_entries = le32_to_cpu(gpt_head->num_partition_entries);

	for (i = 1; i < num_partition_entries; i++) {
		memset(&info, 0, sizeof(struct disk_partition));
		ret = part_get_info_efi_table(i, &info, gpt_head, gpt_pte);
		if (ret)
			break;

		if ((gpt_pte[i - 1].attributes.fields.reserved & PARTITION_VALID_MASK) >> PARTITION_VALID_SHIFT) {
			if (strcmp((char *)info.name, name) == 0) {
				if (which)
					*which = 2;
				break;
			}
			sprintf(buf, "%s0", name);
			if (strcmp((char *)info.name, buf) == 0) {
				if (which)
					*which = 0;
				break;
			}
			sprintf(buf, "%s1", name);
			if (strcmp((char *)info.name, buf) == 0) {
				if (which)
					*which = 1;
				break;
			}
		}
	}

	if (i >= num_partition_entries)
		return -1;

	if (part)
		*part = info;

	return i;
}

int artosyn_get_part_valid_info(gpt_header *gpt_head, gpt_entry *gpt_pte, unsigned long long *part_info)
{
	int i, ret;
	struct disk_partition info;

	for (i = 1; i < AR_PART_MAX_BOOTARGS; i++) {
		memset(&info, 0, sizeof(struct disk_partition));
		ret = part_get_info_efi_table(i, &info, gpt_head, gpt_pte);
		if (ret) {
			break;
		}

		if (((gpt_pte[i - 1].attributes.fields.reserved & PARTITION_VALID_MASK) >> PARTITION_VALID_SHIFT) == 1)
			*part_info |= BIT(i - 1); /* one bit per partition, 1 means valid */
	}

	return 0;
}

static int artosyn_get_valid_partition(char *name, struct disk_partition *info, int *part)
{
	int ret, which;
	unsigned int version;
	unsigned char *gpt;
	gpt_entry *gpt_pte;
	gpt_header *gpt_head;

	ret = get_higher_version_gpt(&version, &gpt, &which);
	if (ret < 0) {
		printf("Both gpt error\n");
		return ret;
	}

	gpt_head  = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_pte = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	debug("gpt%d is higher valid, version: %d, ", which, version);

	ret = artosyn_find_valid_partition(name, gpt_head, gpt_pte, info, NULL);
	if (ret < 0) {
		printf("cannot find valid partition: %s\n", name);
		return ret;
	}
	if (part)
		*part = ret;
	printf("%s partition: %d\n", name, ret);

	return 0;
}

#ifdef CONFIG_SPL_BUILD
#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_PARTITION
int spl_mmc_boot_partition(const u32 boot_device)
{
	int ret, part;
#if CONFIG_IS_ENABLED(ATF)
	char *part_name[] = {"bl31", "uboot"};
#else
	char *part_name[] = {"uboot"};
#endif
	ret = artosyn_get_valid_partition(part_name[boot_device], NULL, &part);
	if (ret) {
		printf("get %s partition from gpt failed, boot from default place\n",
			part_name[boot_device]);
		goto out;
	}

	return part;
out:
	return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION;
}

int spl_mmc_read_from_valid_partition(char *part_name, lbaint_t offset,
				      lbaint_t blkcnt, void *buffer) {
	unsigned long count;
	struct disk_partition info;
	int part_num;
	int ret;

	if (!part_name) {
		printf("spl: mmc partition name missing!\n");
		ret = -EINVAL;
		goto out;
	}

	ret = artosyn_device_init();
	if (ret)
		goto out;

	ret = blk_dselect_hwpart(block_dev, 0);
	if (ret) {
		printf("spl: mmc partition 0 switch failed\n");
		goto out;
	}

	ret = artosyn_get_valid_partition(part_name, &info, &part_num);
	if (ret) {
		printf("spl: get valid partition(%s) failed\n", part_name);
		goto out;
	}

	if (blkcnt == 0)
		count = blk_dread(block_dev, info.start + offset, info.size,
				  buffer);
	else
		count = blk_dread(block_dev, info.start + offset, blkcnt,
				  buffer);
	debug("read part number %d sector %lx, count=%lu\n", part_num,
	      info.start + offset, count);
	if (count == 0) {
		ret = -EIO;
		goto out;
	}

	return count;
out:
	return ret;

}

int spl_mmc_read_from_hw_partition(int hwpart, lbaint_t offset,
				   lbaint_t blkcnt, void *buffer) {
	unsigned long count;
	int ret;

	ret = artosyn_device_init();
	if (ret)
		goto out;

	ret = blk_dselect_hwpart(block_dev, hwpart);
	if (ret) {
		printf("spl: mmc partition %d switch failed\n", hwpart);
		goto out;
	}

	count = blk_dread(block_dev, offset, blkcnt, buffer);
	debug("Read hw partition %d sector %lx, count=%lu\n", hwpart, offset,
	      count);
	if (count == 0) {
		ret = -EIO;
		goto out;
	}

	return count;
out:
	return ret;
}

int spl_mmc_write_to_hw_partition(int hwpart, lbaint_t offset,
				   lbaint_t blkcnt, void *buffer) {
	unsigned long count;
	int ret;

	ret = artosyn_device_init();
	if (ret)
		goto out;

	ret = blk_dselect_hwpart(block_dev, hwpart);
	if (ret) {
		printf("spl: mmc partition %d switch failed\n", hwpart);
		goto out;
	}

	count = blk_dwrite(block_dev, offset, blkcnt, buffer);
	debug("Write hw partition %d sector %lx, count=%lu\n", hwpart, offset,
	      count);
	if (count == 0) {
		ret = -EIO;
		goto out;
	}

	return count;
out:
	return ret;
}
#endif

#if CONFIG_SYS_SPI_U_BOOT_OFFS
unsigned int spl_spi_get_uboot_offs(struct spi_flash *flash)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("uboot", &info, NULL);
	if (ret) {
		printf("get uboot partition from gpt failed, boot from default place\n");
		goto out;
	}

	return info.start * info.blksz + NOR_GPT0_OFFSET;
out:
	return CONFIG_SYS_SPI_U_BOOT_OFFS;
}
#endif

#ifdef CONFIG_SYS_NAND_U_BOOT_OFFS
unsigned int spl_spinand_get_uboot_offs(void)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("uboot", &info, NULL);
	if (ret) {
		printf("get uboot partition from gpt failed, boot from default place\n");
		goto out;
	}

	return info.start * info.blksz;
out:
	return CONFIG_SYS_NAND_U_BOOT_OFFS;
}
#endif

#ifdef CONFIG_SYS_NAND_ATF_OFFS
unsigned int spl_spinand_get_atf_offs(void)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("bl31", &info, NULL);
	if (ret) {
		printf("get bl31 partition from gpt failed, boot from default place\n");
		goto out;
	}

	return info.start * info.blksz;
out:
	return CONFIG_SYS_NAND_ATF_OFFS;
}
#endif

#endif

#ifdef CONFIG_ENV_IS_IN_MMC
int mmc_get_env_addr(struct mmc *mmc, int copy, u32 *env_addr)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("env", &info, NULL);
	if (ret) {
		printf("get env partition from gpt failed, get from default place\n");
		goto out;
	}

	*env_addr = info.start * info.blksz;
	return 0;
out:
	*env_addr = CONFIG_ENV_OFFSET;
	return 0;
}
#endif

#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
unsigned int sf_get_env_addr(struct spi_flash *flash)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("env", &info, NULL);
	if (ret) {
		printf("get env partition from gpt failed, get from default place\n");
		goto out;
	}

	return info.start * info.blksz + NOR_GPT0_OFFSET;
out:
	return CONFIG_ENV_OFFSET;
}
#endif

#ifdef CONFIG_ENV_IS_IN_SPI_NAND
unsigned int spinand_get_env_addr(struct mtd_info *flash)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("env", &info, NULL);
	if (ret) {
		printf("get env partition from gpt failed, get from default place\n");
		goto out;
	}

	return info.start * info.blksz;
out:
	return CONFIG_ENV_OFFSET;
}

int spinand_write_env(struct mtd_info *mtd, int offset, char *data, int size)
{
	int ret;
	struct disk_partition info;

	ret = artosyn_get_valid_partition("env", &info, NULL);
	if (ret) {
		printf("get env partition from gpt failed, get from default place\n");
		return -1;
	}

	ret = nand_mtd_write_common(mtd, offset, info.size * info.blksz, (unsigned char *)data, size);
	if (ret) {
		printf("spi nand write env failed\n");
		return ret;
	}

	return 0;
}
#endif

int gpt_validate_entries(gpt_header *gpt_h, gpt_entry *gpt_e)
{
	uint32_t calc_crc32;

	/* Check the GUID Partition Table Entry Array CRC */
	calc_crc32 = efi_crc32((const unsigned char *)gpt_e,
		__le32_to_cpu(gpt_h->num_partition_entries) *
		__le32_to_cpu(gpt_h->sizeof_partition_entry));

	if (calc_crc32 != __le32_to_cpu(gpt_h->partition_entry_array_crc32)) {
		debug("%s: 0x%x != 0x%x\n",
		       "GUID Partition Table Entry Array CRC is wrong",
		       __le32_to_cpu(gpt_h->partition_entry_array_crc32),
		       calc_crc32);
		return -1;
	}

	return 0;
}

int gpt_validate_header(gpt_header *gpt_h, lbaint_t lba, lbaint_t lastlba)
{
	uint32_t crc32_backup = 0;
	uint32_t calc_crc32;

	/* Check the GPT header signature */
	if (__le64_to_cpu(gpt_h->signature) != GPT_HEADER_SIGNATURE_UBOOT) {
		debug("%s signature is wrong: 0x%llX != 0x%llX\n",
		       "GUID Partition Table Header",
		       __le64_to_cpu(gpt_h->signature),
		       GPT_HEADER_SIGNATURE_UBOOT);
		return -1;
	}

	/* Check the GUID Partition Table CRC */
	memcpy(&crc32_backup, &gpt_h->header_crc32, sizeof(crc32_backup));
	memset(&gpt_h->header_crc32, 0, sizeof(gpt_h->header_crc32));

	calc_crc32 = efi_crc32((const unsigned char *)gpt_h,
		__le32_to_cpu(gpt_h->header_size));

	memcpy(&gpt_h->header_crc32, &crc32_backup, sizeof(crc32_backup));

	if (calc_crc32 != __le32_to_cpu(crc32_backup)) {
		printf("%s CRC is wrong: 0x%x != 0x%x\n",
		       "GUID Partition Table Header",
		       __le32_to_cpu(crc32_backup), calc_crc32);
		return -1;
	}

	/*
	 * Check that the my_lba entry points to the LBA that contains the GPT
	 */
	if (__le64_to_cpu(gpt_h->my_lba) != lba) {
		printf("GPT: my_lba incorrect: %llX != %lx\n",
		       __le64_to_cpu(gpt_h->my_lba),
		       (unsigned long)lba);
		return -1;
	}

	return 0;
}

#ifdef CONFIG_SPI_FLASH
static int mtd_gpt_read(unsigned char *buf, int which)
{
	int ret;
	struct spi_flash *flash = dev_get_uclass_priv(nor_dev);

	if (which == 0) {
		/* gpt0 */
		ret = spi_flash_read_dm(nor_dev, NOR_GPT0_OFFSET, GPT_BUF_SZ, buf);
		if (ret < 0) {
			printf("read gpt failed\n");
			return -1;
		}
	} else if (which == 1) {
		/* gpt1 header, no mbr */
		ret = spi_flash_read_dm(nor_dev, flash->size - FLASH_BLKSZ, FLASH_BLKSZ, buf + FLASH_BLKSZ);
		if (ret < 0) {
			printf("read gpt failed\n");
			return -1;
		}

		/* gpt1 entry */
		ret = spi_flash_read_dm(nor_dev, flash->size - FLASH_BLKSZ * 33, FLASH_BLKSZ * 32, buf + 2 * FLASH_BLKSZ);
		if (ret < 0) {
			printf("read gpt failed\n");
			return -1;
		}
	}

	return 0;
}
#endif

#if defined(CONFIG_MTD_SPI_NAND) || defined(CONFIG_SPL_SPINAND_SUPPORT)
unsigned int nand_mtd_get_gpt0_offset(void)
{
	unsigned int spl_size;

	spl_size = NAND_SPL_PAGES * nand_mtd->writesize;

	return spl_size * NAND_SPL_PARTS;
}

int spinand_flash_read(struct mtd_info *mtd, unsigned int offset, unsigned int size, unsigned char *addr)
{
	int ret, remaining;
	size_t len, retlen;
	loff_t off;

	remaining = size;

	/* Search for the first good block after the given offset */
	off = offset;
	while (mtd_block_isbad(mtd, off))
		off += mtd->erasesize;

	/* Loop over the pages to do the actual read/write */
	while (remaining) {
		/* Skip the block if it is bad */
		if (mtd_block_isbad(mtd, off)) {
			off += mtd->erasesize;
			continue;
		}
		if (remaining >= mtd->erasesize)
			len = mtd->erasesize;
		else
			len = remaining;

		ret = mtd_read(mtd, off, len, &retlen, addr);
		if (ret < 0 || len != retlen) {
			printf("%s[%d] error\n", __func__, __LINE__);
			return ret;
		}
		remaining -= len;
		addr += len;
		off += mtd->erasesize;
	}

	return 0;
}

static int nand_mtd_gpt_read(unsigned char *buf, int which)
{
	int ret;
	unsigned int start;

	start = nand_mtd_get_gpt0_offset();
	if (which)
		start += nand_mtd->erasesize * NAND_PES_PER_GPT;

	ret = spinand_flash_read(nand_mtd, start, GPT_BUF_SZ, buf);
	if (ret)
		return ret;

	return 0;
}
#endif

#ifdef CONFIG_MMC
static int mmc_gpt_read(unsigned char *buf, int which)
{
	int ret;
	unsigned int blk_cnt;

	blk_cnt = ALIGN(GPT_BUF_SZ, block_dev->blksz) / block_dev->blksz;

	if (which == 0) {
		/* gpt0 */
		ret = blk_dread(block_dev, 0, blk_cnt, buf);
		if (ret < 0) {
			printf("read gpt failed\n");
			return -1;
		}
	} else if (which == 1) {
		/* gpt1 header, no mbr */
		ret = blk_dread(block_dev, block_dev->lba - 1, 1, buf + block_dev->blksz);
		if (ret < 0) {
			printf("read gpt failed\n");
			return -1;
		}

		/* gpt1 entry */
		ret = blk_dread(block_dev, block_dev->lba - 33, 32, buf + 2 * block_dev->blksz);
		if (ret < 0) {
			printf("read gpt failed\n");
			return -1;
		}
	}

	return 0;
}
#endif

int current_gpt_check(struct upgrade_ops *ops, int *gpt0_ok, int *gpt1_ok, unsigned char **gpt0, unsigned char **gpt1)
{
	int ret = 0;
	gpt_header		*p_gpt0, *p_gpt1;
	gpt_entry		*p_entry0, *p_entry1;

#ifdef CONFIG_SPL_BUILD
	*gpt0 = (unsigned char *)SPL_GPT0_ADDR;
	*gpt1 = (unsigned char *)SPL_GPT1_ADDR;
#else
	*gpt0 = memalign(ARCH_DMA_MINALIGN, GPT_BUF_SZ);
	if (!*gpt0) {
		debug("alloc gpt size = %d, failed\n", GPT_BUF_SZ);
		return -1;
	}

	*gpt1 = memalign(ARCH_DMA_MINALIGN, GPT_BUF_SZ);
	if (!*gpt1) {
		free(*gpt0);
		debug("alloc gpt size = %d, failed\n", GPT_BUF_SZ);
		return -1;
	}
#endif

	memset(*gpt0, 0, GPT_BUF_SZ);
	memset(*gpt1, 0, GPT_BUF_SZ);

	if (ops) {
		if (strcmp((char *)ops->type, "nor") == 0)
#ifdef CONFIG_SPI_FLASH
			ret = mtd_gpt_read(*gpt0, 0);
#else
			ret = -1;
#endif
		else if (strcmp((char *)ops->type, "nand") == 0)
#ifdef CONFIG_MTD_SPI_NAND
			ret = nand_mtd_gpt_read(*gpt0, 0);
#else
			ret = -1;
#endif
		else if (strcmp((char *)ops->type, "mmc") == 0)
#ifdef CONFIG_MMC
			ret = mmc_gpt_read(*gpt0, 0);
#else
			ret = -1;
#endif
		else {
			printf("image type is error\n");
			goto out;
		}
		if (ret < 0) {
			printf("read gpt0 failed\n");
			goto out;
		}

		if (strcmp((char *)ops->type, "nor") == 0)
#ifdef CONFIG_SPI_FLASH
			ret = mtd_gpt_read(*gpt1, 1);
#else
			ret = -1;
#endif
		else if (strcmp((char *)ops->type, "nand") == 0)
#ifdef CONFIG_MTD_SPI_NAND
			ret = nand_mtd_gpt_read(*gpt1, 1);
#else
			ret = -1;
#endif
		else if (strcmp((char *)ops->type, "mmc") == 0)
#ifdef CONFIG_MMC
			ret = mmc_gpt_read(*gpt1, 1);
#else
			ret = -1;
#endif
		else {
			printf("image type is error\n");
			goto out;
		}
		if (ret < 0) {
			printf("read gpt1 failed\n");
			goto out;
		}
	} else {
		int boot_mode = get_boot_device();
#ifdef CONFIG_SPI_FLASH
#ifdef CONFIG_ARTOSYN_AR9301
		if (boot_mode == BOOT_VALUE_ROM_QSPI_NOR
				|| boot_mode == BOOT_VALUE_ROM_AUTO
				|| boot_mode == BOOT_VALUE_ROM_AUTO_BYPASS_PLL) {
#elif defined(CONFIG_ARTOSYN_AR9311)
		if (boot_mode == BOOT_VALUE_ROM_QSPI_NOR_1V8
				|| boot_mode == BOOT_VALUE_ROM_QSPI_NOR_3V3) {
#endif
			ret = mtd_gpt_read(*gpt0, 0);
			if (ret < 0) {
				return ret;
			}
			ret = mtd_gpt_read(*gpt1, 1);
			if (ret < 0) {
				return ret;
			}
			goto out1;
		}
#endif
#ifdef CONFIG_MTD_SPI_NAND
#ifdef CONFIG_ARTOSYN_AR9301
		if (boot_mode == BOOT_VALUE_ROM_QSPI_NAND
				|| boot_mode == BOOT_VALUE_ROM_AUTO
				|| boot_mode == BOOT_VALUE_ROM_AUTO_BYPASS_PLL) {
#elif defined(CONFIG_ARTOSYN_AR9311)
		if (boot_mode == BOOT_VALUE_ROM_QSPI_NAND_1V8
				|| boot_mode == BOOT_VALUE_ROM_QSPI_NAND_3V3) {
#endif
			ret = nand_mtd_gpt_read(*gpt0, 0);
			if (ret < 0) {
				return ret;
			}
			ret = nand_mtd_gpt_read(*gpt1, 1);
			if (ret < 0) {
				return ret;
			}
			goto out1;
		}
#endif
#ifdef CONFIG_MMC
#ifdef CONFIG_ARTOSYN_AR9301
		if (boot_mode == BOOT_VALUE_ROM_EMMC
				|| boot_mode == BOOT_VALUE_ROM_AUTO
				|| boot_mode == BOOT_VALUE_ROM_AUTO_BYPASS_PLL) {
#elif defined(CONFIG_ARTOSYN_AR9311)
		if (boot_mode == BOOT_VALUE_ROM_EMMC) {
#endif
			ret = mmc_gpt_read(*gpt0, 0);
			if (ret < 0) {
				return ret;
			}
			ret = mmc_gpt_read(*gpt1, 1);
			if (ret < 0) {
				return ret;
			}
			goto out1;
		}
#endif
	}
out1:
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

#ifndef CONFIG_SPL_BUILD
/* include erase */
int nand_mtd_write_common(struct mtd_info *mtd, unsigned int offset, size_t part_size, unsigned char *data, size_t size)
{
	int img_ebs, eb, written_ebs = 0;
	int eb_cnt;
	int erasesize;
	struct erase_info instr;
	loff_t off;
	size_t retlen;

	eb_cnt = part_size / mtd->erasesize;
	erasesize = mtd->erasesize;
	img_ebs = size / mtd->erasesize;
	eb = offset / erasesize;
	eb_cnt += eb;

	/* ubi image is block aligned, but rawimage size maybe not block aligned */
	if (size % mtd->erasesize)
		img_ebs++;
	for (; eb < eb_cnt; eb++) {
		int err, new_len;

		off = eb * mtd->erasesize;
		err = mtd_block_isbad(mtd, off);
		if (err < 0) {
			printf("\nmtd_block_isbad error ret=%d\n", err);
			return -6; /* ERR_OTHER */
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
				return -3; /* ERR_ERASE */

			off = eb * mtd->erasesize;
			if (mtd_block_markbad(mtd, off))
				return -3; /* ERR_ERASE */

			continue;
		}

		if (size > erasesize)
			new_len = erasesize;
		else
			new_len = size;

		err = mtd_write(mtd, off, new_len, &retlen, (u_char *)data);
		if (err) {
			printf("cannot write eraseblock %d\n", eb);

			if (err != -EIO)
				return -3; /* ERR_ERASE */

			if (mtd_block_markbad(mtd, off))
				return -3; /* ERR_ERASE */

			continue;
		}
		data += new_len;
		size -= new_len;
		if (++written_ebs >= img_ebs)
			break;
	}

	if (size) {
		printf("mtd part is too small or has too many bad blocks\n");
		return -6;
	}

	return 0;
}
#endif

#if defined(CONFIG_SPI_FLASH) || defined(CONFIG_MTD_SPI_NAND)
int mtdparts_generate(char *mtdparts)
{
	int ret, i;
	unsigned char *gpt;
	gpt_entry *gpt_pte;
	gpt_header *gpt_head;
	struct disk_partition info;

	if (!mtdparts)
		return -1;

	memset(mtdparts, 0, strlen(mtdparts));
#ifdef CONFIG_SPI_FLASH
	if (nor_dev) {
		strcpy(mtdparts, " mtdparts=artosyn_qspi:64k@0(romcode),256k(spl0),256k(spl1),64k(gpt)");
		goto out;
	}
#endif
#ifdef CONFIG_MTD_SPI_NAND
	if (nand_mtd) {
		if (nand_mtd->writesize == 2048)
			i = 256;
		else if (nand_mtd->writesize == 4096)
			i = 512;
		else if (nand_mtd->writesize == 8192)
			i = 1024;
		else {
			printf("nand page size bigger than 8KB is not supported now\n");
			return -1;
		}

		sprintf(mtdparts, " mtdparts=spi32766.1:%dk@0(spl0),%dk(spl1),%dk(spl2),%dk(spl3),%dk(gpt0),%dk(gpt1)",
				i, i, i, i, i / 2 * NAND_PES_PER_GPT , i / 2 * NAND_PES_PER_GPT);

		goto out;
	}
#endif

out:
	ret = get_higher_version_gpt(NULL, &gpt, NULL);
	if (ret < 0) {
		printf("Both gpt error\n");
		return ret;
	}

	gpt_head  = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_pte = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	for (i = 1; i < AR_PART_MAX_BOOTARGS; i++) {
		lbaint_t size;
		char tmp[64];

		memset(&info, 0, sizeof(struct disk_partition));
		ret = part_get_info_efi_table(i, &info, gpt_head, gpt_pte);
		if (ret) {
			break;
		}

		size = info.size * info.blksz;
		if ( (size & 0xfffff) == 0) {//unit:M
			sprintf(tmp, ",%dM(%s)", (size >> 20), info.name);
		} else if ((size & 0x3ff) == 0) { //unit:k
			sprintf(tmp, ",%dK(%s)", (size >> 10), info.name);
		} else {
			printf("error size \n");
		}
		strcat(mtdparts, tmp);
	}

	return 0;
}
#endif

#ifdef CONFIG_SPI_FLASH
int spinor_read(void *buf, unsigned int offset, unsigned int size)
{
	int ret;

	ret = artosyn_device_init();
	if (ret) {
		printf("device init failed\n");
		return ret;
	}

	ret = spi_flash_read_dm(nor_dev, offset, size, buf);
	if (ret < 0) {
		printf("spinor read failed, offset: %#x, size %#x\n", offset, size);
		return ret;
	}

	return 0;
}
#endif

#ifdef CONFIG_MTD_SPI_NAND
int spinand_read(void *buf, unsigned int offset, unsigned int size)
{
	int ret;

	ret = artosyn_device_init();
	if (ret) {
		printf("device init failed\n");
		return ret;
	}

	ret = spinand_flash_read(nand_mtd, offset, size, buf);
	if (ret) {
		printf("spinand read failed, offset: %#x, size %#x\n", offset, size);
		return ret;
	}

	return 0;
}
#endif
