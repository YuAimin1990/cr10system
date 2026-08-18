// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <autoboot.h>
#include <bootretry.h>
#include <cli.h>
#include <command.h>
#include <console.h>
#include <env.h>
#include <fdtdec.h>
#include <hash.h>
#include <log.h>
#include <malloc.h>
#include <memalign.h>
#include <menu.h>
#include <post.h>
#include <time.h>
#include <linux/delay.h>
#include <u-boot/sha256.h>
#include <bootcount.h>
#include <artosyn/ar_common.h>
#include <uboot_upgrade.h>
#include <part_efi.h>
#include <part.h>
#include <search.h>
#include <asm/arch/boot.h>

DECLARE_GLOBAL_DATA_PTR;

#define MAX_DELAY_STOP_STR 32

#ifndef DEBUG_BOOTKEYS
#define DEBUG_BOOTKEYS 0
#endif
#define debug_bootkeys(fmt, args...)		\
	debug_cond(DEBUG_BOOTKEYS, fmt, ##args)

/* Stored value of bootdelay, used by autoboot_command() */
static int stored_bootdelay;
static int menukey;

#ifdef CONFIG_AUTOBOOT_ENCRYPTION
#define AUTOBOOT_STOP_STR_SHA256 CONFIG_AUTOBOOT_STOP_STR_SHA256
#else
#define AUTOBOOT_STOP_STR_SHA256 ""
#endif

#ifdef CONFIG_USE_AUTOBOOT_MENUKEY
#define AUTOBOOT_MENUKEY CONFIG_USE_AUTOBOOT_MENUKEY
#else
#define AUTOBOOT_MENUKEY 0
#endif

/*
 * Use a "constant-length" time compare function for this
 * hash compare:
 *
 * https://crackstation.net/hashing-security.htm
 */
static int slow_equals(u8 *a, u8 *b, int len)
{
	int diff = 0;
	int i;

	for (i = 0; i < len; i++)
		diff |= a[i] ^ b[i];

	return diff == 0;
}

/**
 * passwd_abort_sha256() - check for a hashed key sequence to abort booting
 *
 * This checks for the user entering a SHA256 hash within a given time.
 *
 * @etime: Timeout value ticks (stop when get_ticks() reachs this)
 * @return 0 if autoboot should continue, 1 if it should stop
 */
static int passwd_abort_sha256(uint64_t etime)
{
	const char *sha_env_str = env_get("bootstopkeysha256");
	u8 sha_env[SHA256_SUM_LEN];
	u8 *sha;
	char *presskey;
	const char *algo_name = "sha256";
	u_int presskey_len = 0;
	int abort = 0;
	int size = sizeof(sha);
	int ret;

	if (sha_env_str == NULL)
		sha_env_str = AUTOBOOT_STOP_STR_SHA256;

	/*
	 * Generate the binary value from the environment hash value
	 * so that we can compare this value with the computed hash
	 * from the user input
	 */
	ret = hash_parse_string(algo_name, sha_env_str, sha_env);
	if (ret) {
		printf("Hash %s not supported!\n", algo_name);
		return 0;
	}

	presskey = malloc_cache_aligned(MAX_DELAY_STOP_STR);
	sha = malloc_cache_aligned(SHA256_SUM_LEN);
	size = SHA256_SUM_LEN;
	/*
	 * We don't know how long the stop-string is, so we need to
	 * generate the sha256 hash upon each input character and
	 * compare the value with the one saved in the environment
	 */
	do {
		if (tstc()) {
			/* Check for input string overflow */
			if (presskey_len >= MAX_DELAY_STOP_STR) {
				free(presskey);
				free(sha);
				return 0;
			}

			presskey[presskey_len++] = getc();

			/* Calculate sha256 upon each new char */
			hash_block(algo_name, (const void *)presskey,
				   presskey_len, sha, &size);

			/* And check if sha matches saved value in env */
			if (slow_equals(sha, sha_env, SHA256_SUM_LEN))
				abort = 1;
		}
	} while (!abort && get_ticks() <= etime);

	free(presskey);
	free(sha);
	return abort;
}

/**
 * passwd_abort_key() - check for a key sequence to aborted booting
 *
 * This checks for the user entering a string within a given time.
 *
 * @etime: Timeout value ticks (stop when get_ticks() reachs this)
 * @return 0 if autoboot should continue, 1 if it should stop
 */
static int passwd_abort_key(uint64_t etime)
{
	int abort = 0;
	struct {
		char *str;
		u_int len;
		int retry;
	}
	delaykey[] = {
		{ .str = env_get("bootdelaykey"),  .retry = 1 },
		{ .str = env_get("bootstopkey"),   .retry = 0 },
	};

	char presskey[MAX_DELAY_STOP_STR];
	u_int presskey_len = 0;
	u_int presskey_max = 0;
	u_int i;

#  ifdef CONFIG_AUTOBOOT_DELAY_STR
	if (delaykey[0].str == NULL)
		delaykey[0].str = CONFIG_AUTOBOOT_DELAY_STR;
#  endif
#  ifdef CONFIG_AUTOBOOT_STOP_STR
	if (delaykey[1].str == NULL)
		delaykey[1].str = CONFIG_AUTOBOOT_STOP_STR;
#  endif

	for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i++) {
		delaykey[i].len = delaykey[i].str == NULL ?
				    0 : strlen(delaykey[i].str);
		delaykey[i].len = delaykey[i].len > MAX_DELAY_STOP_STR ?
				    MAX_DELAY_STOP_STR : delaykey[i].len;

		presskey_max = presskey_max > delaykey[i].len ?
				    presskey_max : delaykey[i].len;

		debug_bootkeys("%s key:<%s>\n",
			       delaykey[i].retry ? "delay" : "stop",
			       delaykey[i].str ? delaykey[i].str : "NULL");
	}

	/* In order to keep up with incoming data, check timeout only
	 * when catch up.
	 */
	do {
		if (tstc()) {
			if (presskey_len < presskey_max) {
				presskey[presskey_len++] = getc();
			} else {
				for (i = 0; i < presskey_max - 1; i++)
					presskey[i] = presskey[i + 1];

				presskey[i] = getc();
			}
		}

		for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i++) {
			if (delaykey[i].len > 0 &&
			    presskey_len >= delaykey[i].len &&
				memcmp(presskey + presskey_len -
					delaykey[i].len, delaykey[i].str,
					delaykey[i].len) == 0) {
					debug_bootkeys("got %skey\n",
						delaykey[i].retry ? "delay" :
						"stop");

				/* don't retry auto boot */
				if (!delaykey[i].retry)
					bootretry_dont_retry();
				abort = 1;
			}
		}
	} while (!abort && get_ticks() <= etime);

	return abort;
}

/***************************************************************************
 * Watch for 'delay' seconds for autoboot stop or autoboot delay string.
 * returns: 0 -  no key string, allow autoboot 1 - got key string, abort
 */
static int abortboot_key_sequence(int bootdelay)
{
	int abort;
	uint64_t etime = endtick(bootdelay);

#  ifdef CONFIG_AUTOBOOT_PROMPT
	/*
	 * CONFIG_AUTOBOOT_PROMPT includes the %d for all boards.
	 * To print the bootdelay value upon bootup.
	 */
	printf(CONFIG_AUTOBOOT_PROMPT, bootdelay);
#  endif

	if (IS_ENABLED(CONFIG_AUTOBOOT_ENCRYPTION))
		abort = passwd_abort_sha256(etime);
	else
		abort = passwd_abort_key(etime);
	if (!abort)
		debug_bootkeys("key timeout\n");

	return abort;
}

static int abortboot_single_key(int bootdelay)
{
	int abort = 0;
	unsigned long ts;

	printf("Hit any key to stop autoboot: %2d ", bootdelay);

	/*
	 * Check if key already pressed
	 */
	if (tstc()) {	/* we got a key press	*/
		(void) getc();  /* consume input	*/
		puts("\b\b\b 0");
		abort = 1;	/* don't auto boot	*/
	}

	while ((bootdelay > 0) && (!abort)) {
		--bootdelay;
		/* delay 1000 ms */
		ts = get_timer(0);
		do {
			if (tstc()) {	/* we got a key press	*/
				int key;

				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				key = getc(); /* consume input	*/
				if (IS_ENABLED(CONFIG_USE_AUTOBOOT_MENUKEY))
					menukey = key;
				break;
			}
			udelay(10000);
		} while (!abort && get_timer(ts) < 1000);

		printf("\b\b\b%2d ", bootdelay);
	}

	putc('\n');

	return abort;
}

static int abortboot(int bootdelay)
{
	int abort = 0;

	if (bootdelay >= 0) {
		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			abort = abortboot_key_sequence(bootdelay);
		else
			abort = abortboot_single_key(bootdelay);
	}
    
    if (secure_boot_enabled())
        abort = 0;

	if (IS_ENABLED(CONFIG_SILENT_CONSOLE) && abort)
		gd->flags &= ~GD_FLG_SILENT;

	return abort;
}

static void process_fdt_options(const void *blob)
{
#ifdef CONFIG_SYS_TEXT_BASE
	ulong addr;

	/* Add an env variable to point to a kernel payload, if available */
	addr = fdtdec_get_config_int(gd->fdt_blob, "kernel-offset", 0);
	if (addr)
		env_set_addr("kernaddr", (void *)(CONFIG_SYS_TEXT_BASE + addr));

	/* Add an env variable to point to a root disk, if available */
	addr = fdtdec_get_config_int(gd->fdt_blob, "rootdisk-offset", 0);
	if (addr)
		env_set_addr("rootaddr", (void *)(CONFIG_SYS_TEXT_BASE + addr));
#endif /* CONFIG_SYS_TEXT_BASE */
}

const char *bootdelay_process(void)
{
	char *s;
	int bootdelay;

	bootcount_inc();

	s = env_get("bootdelay");
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

	if (IS_ENABLED(CONFIG_OF_CONTROL))
		bootdelay = fdtdec_get_config_int(gd->fdt_blob, "bootdelay",
						  bootdelay);

	debug("### main_loop entered: bootdelay=%d\n\n", bootdelay);

	if (IS_ENABLED(CONFIG_AUTOBOOT_MENU_SHOW))
		bootdelay = menu_show(bootdelay);
	bootretry_init_cmd_timeout();

#ifdef CONFIG_POST
	if (gd->flags & GD_FLG_POSTFAIL) {
		s = env_get("failbootcmd");
	} else
#endif /* CONFIG_POST */
	if (bootcount_error())
		s = env_get("altbootcmd");
	else
		s = env_get("bootcmd");

	if (IS_ENABLED(CONFIG_OF_CONTROL))
		process_fdt_options(gd->fdt_blob);
	stored_bootdelay = bootdelay;

	return s;
}

/* check which kernel and dtb to use while dual partitions,
 * regenerate bootargs
 */
static char *regenerate_bootinfo(const char *s)
{
	int ret;
	char *buf;
	unsigned int version;
	int kernel = -1, dtb = -1, len;
	char *str = NULL, *s1 = NULL, *s2 = NULL;
	unsigned char *gpt;
	gpt_entry *gpt_pte;
	gpt_header *gpt_head;
	struct disk_partition kinfo, dinfo;
	int boot_mode;
	unsigned long long part_info = 0;
	int rootfs_pn;

	buf = malloc(1024);
	if (!buf) {
		printf("out of memory\n");
		return NULL;
	}

	str = env_get("bootargs");
	if (str == NULL) {
		printf("Warning: bootargs is not found\n");
		goto out1;
	}
	strcpy(buf, str);
	s2 = str;
	str = NULL;

	ret = get_higher_version_gpt(&version, &gpt, NULL);
	if (ret < 0) {
		printf("Both gpt error, boot failed\n");
		goto out1;
	}

	gpt_head  = (gpt_header *)(gpt + FLASH_BLKSZ);
	gpt_pte = (gpt_entry  *)(gpt + FLASH_BLKSZ * 2);

	ret = artosyn_find_valid_partition("kernel", gpt_head, gpt_pte, &kinfo, &kernel);
	if (ret < 0) {
		printf("cannot find valid kernel, boot failed\n");
		goto out1;
	} else {
		if (kernel == 2)
			printf("kernel, ");
		else
			printf("kernel%d, ", kernel);
	}

	ret = artosyn_find_valid_partition("dtb", gpt_head, gpt_pte, &dinfo, &dtb);
	if (ret < 0) {
		printf("cannot find valid dtb, boot failed\n");
		goto out1;
	} else {
		if (dtb == 2)
			printf("dtb, ");
		else
			printf("dtb%d, ", dtb);
	}

	rootfs_pn = artosyn_find_valid_partition("userapp", gpt_head, gpt_pte, NULL, NULL);
	if (rootfs_pn < 0) {
		printf("cannot find valid rootfs, boot failed\n");
		goto out1;
	}

	ret = artosyn_get_part_valid_info(gpt_head, gpt_pte, &part_info);
	if (ret < 0) {
		printf("parse valid partitions error, boot failed\n");
		goto out1;
	} else
		printf("part info: %#llx\n", part_info);
	
	free(gpt);
	gpt_pte = NULL;

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
		/* find rootfs */
		s1 = strstr(s2, "mtdblock");
		if (!s1)
			goto out3;

		memset(&buf[s1 - s2 + 8], 0, 5);
		/* spl0 - spl1, gpt0 gpt1 */
		sprintf(&buf[s1 - s2 + 8], "%d", rootfs_pn + 4);

		if (s1[9] == ' ') {
			strcat(buf, &s1[9]);
		} else
			strcat(buf, &s1[10]);
#define BOOTCMD_LEN 	256
		str = malloc(BOOTCMD_LEN);
		if (!str) {
			printf("out of memory\n");
			goto out1;
		}
		memset(str, 0, BOOTCMD_LEN);
		sprintf(str, "setenv kernel_comp_addr %#x; setenv kernel_comp_size %#x; sf probe; sf read %#x %#x %#x; sf read %#x %#x %#x; bootm %#x - %#x",
				KERNEL_LOAD_ADDR, KERNEL_COMP_SIZE, KERNEL_LOAD_ADDR, kinfo.start * kinfo.blksz + NOR_GPT0_OFFSET, kinfo.size * kinfo.blksz,
				KERNEL_DTB_LOAD_ADDR, dinfo.start * dinfo.blksz + NOR_GPT0_OFFSET, dinfo.size * dinfo.blksz,
				KERNEL_LOAD_ADDR, KERNEL_DTB_LOAD_ADDR);

		goto out3;
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
		/* find rootfs */
		s1 = strstr(s2, "ubi.mtd=");
		if (!s1)
			goto out3;

		memset(&buf[s1 - s2 + 8], 0, 5);
		/* spl0 - spl3, gpt0 gpt1 */
		sprintf(&buf[s1 - s2 + 8], "%d", rootfs_pn + 6);

		if (s1[9] == ' ') {
			strcat(buf, &s1[9]);
		} else
			strcat(buf, &s1[10]);

#define BOOTCMD_LEN 	256
		str = malloc(BOOTCMD_LEN);
		if (!str) {
			printf("out of memory\n");
			goto out1;
		}
		memset(str, 0, BOOTCMD_LEN);
		sprintf(str, "setenv kernel_comp_addr %#x; setenv kernel_comp_size %#x; mtd read spi-nand0 %#x %#x %#x; mtd read spi-nand0 %#x %#x %#x; bootm %#x - %#x",
				KERNEL_LOAD_ADDR, KERNEL_COMP_SIZE, KERNEL_LOAD_ADDR, kinfo.start * kinfo.blksz, kinfo.size * kinfo.blksz,
				KERNEL_DTB_LOAD_ADDR, dinfo.start * dinfo.blksz, dinfo.size * dinfo.blksz,
				KERNEL_LOAD_ADDR, KERNEL_DTB_LOAD_ADDR);

		goto out3;
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
		/* find rootfs */
		s1 = strstr(s2, "mmcblk0p");
		if (!s1)
			goto out3;

		memset(&buf[s1 - s2 + 8], 0, 5);
		sprintf(&buf[s1 - s2 + 8], "%d", rootfs_pn);

		if (s1[9] == ' ') {
			strcat(buf, &s1[9]);
		} else
			strcat(buf, &s1[10]);

		printf("buf: %s\n", buf);
		/* both are single partition */
		if (kernel == 2 && dtb == 2)
			goto out3;

		len = strlen(s);
		/* actually strlen(str) >= strlen(s) + 2 is ok */
		str = malloc(len + 5);
		if (!str) {
			printf("out of memory\n");
			goto out2;
		}
		memset(str, 0, len + 5);

		if (kernel != 2) {
			s1 = strstr(s, " kernel ");
			if (!s1)
				goto out2;
		}

		if (dtb != 2) {
			s2 = strstr(s, " dtb ");
			if (!s2)
				goto out2;
		}

		/* kernel is dual partitions */
		if (s1 && !s2) {
			memcpy(str, s, s1 - s);
			sprintf(&str[s1 - s], " kernel%d ", kernel);
			strcpy(&str[s1 - s + 9], s1 + 8);
		}

		/* dtb is dual partitions */
		if (!s1 && s2) {
			memcpy(str, s, s2 - s);
			sprintf(&str[s2 - s], " dtb%d ", dtb);
			strcpy(&str[s2 - s + 6], s2 + 5);
		}

		/* both are dual partitions */
		if (s1 && s2) {
			/* kernel is before dtb in strings */
			if (s1 < s2) {
				memcpy(str, s, s1 - s + 7);
				sprintf(&str[s1 - s + 7], "%d", kernel);
				strncpy(&str[s1 - s + 8], s1 + 7, s2 - s1);
				sprintf(&str[s2 - s + 5], "%d", dtb);
				strcpy(&str[s2 - s + 6], s2 + 4);
			} else {
				memcpy(str, s, s2 - s + 4);
				sprintf(&str[s2 - s + 4], "%d", dtb);
				strncpy(&str[s2 - s + 5], s2 + 4, s1 - s2);
				sprintf(&str[s1 - s + 8], "%d", kernel);
				strcpy(&str[s1 - s + 9], s1 + 7);
			}
		}

		strcat(buf, " gpt");

	}
#endif

out3:
#if defined(CONFIG_SPI_FLASH) || defined(CONFIG_MTD_SPI_NAND)
	mtdparts_generate(&buf[strlen(buf)]);
#endif
	/* for compatibility, do not add partitions info to bootargs */
	if (version)
		sprintf(buf, "%s part_info=%llu", buf, part_info);

	env_set((const char *)"bootargs", buf);
	free(buf);
	if (str)
		printf("Regenerate: %s\n", str);

	return str;
out2:
	free(str);
	str = NULL;
out1:
	free(buf);

	return str;
}

void autoboot_command(const char *s)
{
	debug("### main_loop: bootcmd=\"%s\"\n", s ? s : "<UNDEFINED>");

	if (stored_bootdelay != -1 && s && !abortboot(stored_bootdelay)) {
		bool lock;
		int prev;

		lock = IS_ENABLED(CONFIG_AUTOBOOT_KEYED) &&
			!IS_ENABLED(CONFIG_AUTOBOOT_KEYED_CTRLC);
		if (lock)
			prev = disable_ctrlc(1); /* disable Ctrl-C checking */

		/* add valid partitions info to bootargs, get valid kernel and dtb */
		const char *str = regenerate_bootinfo(s);
		if (!str)
			run_command_list(s, -1, 0);
		else
			run_command_list(str, -1, 0);

		if (lock)
			disable_ctrlc(prev);	/* restore Ctrl-C checking */
	}

	if (IS_ENABLED(CONFIG_USE_AUTOBOOT_MENUKEY) &&
	    menukey == AUTOBOOT_MENUKEY) {
		s = env_get("menucmd");
		if (s)
			run_command_list(s, -1, 0);
	}
}
