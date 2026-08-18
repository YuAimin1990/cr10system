// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2011 OMICRON electronics GmbH
 *
 * based on drivers/mtd/nand/raw/nand_spl_load.c
 *
 * Copyright (C) 2011
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 */

#include <common.h>
#include <image.h>
#include <log.h>
#include <spi.h>
#include <spi_flash.h>
#include <errno.h>
#include <spl.h>
#include <artosyn/ar_common.h>
#include <asm/arch/image.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_OS_BOOT
/*
 * Load the kernel, check for a valid header we can parse, and if found load
 * the kernel and then device tree.
 */
static int spi_load_image_os(struct spl_image_info *spl_image,
			     struct mtd_info *flash,
			     struct image_header *header)
{
	int err;

	/* Read for a header, parse or error out. */
	spinand_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS, sizeof(*header),
		       (void *)header);

	if (image_get_magic(header) != IH_MAGIC)
		return -1;

	err = spl_parse_image_header(spl_image, header);
	if (err)
		return err;

	spinand_flash_read(flash, CONFIG_SYS_SPI_KERNEL_OFFS,
		       spl_image->size, (void *)spl_image->load_addr);

	/* Read device tree. */
	spinand_flash_read(flash, CONFIG_SYS_SPI_ARGS_OFFS,
		       CONFIG_SYS_SPI_ARGS_SIZE,
		       (void *)CONFIG_SYS_SPL_ARGS_ADDR);

	return 0;
}
#endif

static ulong spl_spi_fit_read(struct spl_load_info *load, ulong sector,
			      ulong count, void *buf)
{
	struct mtd_info *flash = load->dev;
	ulong ret;

	ret = spinand_flash_read(flash, sector, count, buf);
	if (!ret)
		return count;
	else
		return 0;
}

unsigned int __weak spl_spinand_get_uboot_offs(void)
{
	return CONFIG_SYS_NAND_U_BOOT_OFFS;
}

unsigned int __weak spl_spinand_get_atf_offs(void)
{
	return CONFIG_SYS_NAND_ATF_OFFS;
}

static int spl_spinand_load(struct spl_image_info *spl_image,
			    struct spl_boot_device *bootdev,
			    char *image_name,
			    unsigned int payload_offs)
{
	int err = 0;
	struct mtd_info *flash;
	struct image_header *header;
	struct sirius_image_header *ar_header;
	unsigned char * hash_buffer = NULL;
	unsigned char * signature_buffer = NULL;

	/*
	 * Load U-Boot image from SPI flash into RAM
	 * In DM mode: defaults speed and mode will be
	 * taken from DT when available
	 */
	flash = artosyn_qspi_nand_init();
	if (!flash) {
		return -ENODEV;
	}

	ar_header = spl_get_load_buffer(-0x1000, sizeof(*header));
	err = spinand_flash_read(flash, payload_offs, 512, (void *)ar_header);
	if (err) {
		debug("%s: Failed to read from SPINAND flash (err=%d)\n",
			  __func__, err);
		return err;
	}

	if(0x4152544f != ar_header->magic) {
		printf("%s[%d]: magic error!\n", __FUNCTION__, __LINE__);
		hang();
	}
//	ar_header = (struct sirius_image_header *)spl_get_load_buffer(-(sizeof(*header) + sizeof(*ar_header) + ar_header->hash_size + ar_header->sig_len), sizeof(*header));
//
//	err = spinand_flash_read(flash, payload_offs, 512, (void *)ar_header);
//	if (err) {
//		debug("%s: Failed to read from SPINAND flash (err=%d)\n",
//			  __func__, err);
//		return err;
//	}

	hash_buffer = ((unsigned char *)ar_header) + sizeof(*ar_header);
	signature_buffer = hash_buffer + ar_header->hash_size;
	header = (struct image_header *)(signature_buffer + ar_header->sig_len);

#if CONFIG_IS_ENABLED(OF_CONTROL) && !CONFIG_IS_ENABLED(OF_PLATDATA)
	payload_offs = fdtdec_get_config_int(gd->fdt_blob,
					     "u-boot,spl-payload-offset",
					     payload_offs);
#endif

#ifdef CONFIG_SPL_OS_BOOT
	if (spl_start_uboot() || spi_load_image_os(spl_image, flash, header))
#endif
	{
		if (IS_ENABLED(CONFIG_SPL_LOAD_FIT_FULL) &&
		    image_get_magic(header) == FDT_MAGIC) {
			err = spinand_flash_read(flash, payload_offs,
					     roundup(fdt_totalsize(header), 4),
					     (void *)CONFIG_SYS_LOAD_ADDR);
			if (err)
				return err;
			err = spl_parse_image_header(spl_image,
					(struct image_header *)CONFIG_SYS_LOAD_ADDR);
		} else if (IS_ENABLED(CONFIG_SPL_LOAD_FIT) &&
			   image_get_magic(header) == FDT_MAGIC) {
			struct spl_load_info load;

			debug("Found FIT\n");
			load.dev = flash;
			load.priv = NULL;
			load.filename = NULL;
			load.bl_len = 1;
			load.read = spl_spi_fit_read;
			err = spl_load_simple_fit(spl_image, &load,
						  payload_offs,
						  header);
		} else if (IS_ENABLED(CONFIG_SPL_LOAD_IMX_CONTAINER)) {
			struct spl_load_info load;

			load.dev = flash;
			load.priv = NULL;
			load.filename = NULL;
			load.bl_len = 1;
			load.read = spl_spi_fit_read;

			err = spl_load_imx_container(spl_image, &load,
						     payload_offs);
		} else {
			err = spl_parse_image_header(spl_image, header);
			if (err)
				return err;
			err = spinand_flash_read(flash, payload_offs + sizeof(*ar_header) + ar_header->hash_size + ar_header->sig_len,
					     ar_header->img_len,
					     (void *)spl_image->load_addr);
		}
	}

	if(secure_boot_enabled()) {
		if(0 != spl_verify_sw(image_name, (unsigned char *)hash_buffer, ar_header->hash_size,
				(unsigned char *)signature_buffer, ar_header->sig_len,
				(unsigned char *)header, ar_header->img_len)) {
			printf("%s[%d]: verify %s error!\n", __FUNCTION__, __LINE__, image_name);
			return -1;
		}
	}

	return err;
}

/*
 * The main entry for SPI booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from SPI into SDRAM and starts it from there.
 */
static int spl_spinand_load_image(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev)
{
	spl_spinand_load(spl_image, bootdev, "uboot", spl_spinand_get_uboot_offs());
#if CONFIG_IS_ENABLED(ATF)
	//load uboot too if atf enabled
	spl_spinand_load(spl_image, bootdev, "bl31", spl_spinand_get_atf_offs());
#endif

	return 0;
}

/* Use priorty 1 so that boards can override this */
SPL_LOAD_IMAGE_METHOD("SPINAND", 1, BOOT_DEVICE_SPINAND, spl_spinand_load_image);
