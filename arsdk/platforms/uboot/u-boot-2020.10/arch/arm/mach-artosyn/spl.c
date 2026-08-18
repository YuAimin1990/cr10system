// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2020 Artosyn Electronics Co., Ltd
 */

#include <common.h>
#include <debug_uart.h>
#include <dm.h>
#include <hang.h>
#include <image.h>
#include <init.h>
#include <log.h>
#include <ram.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/cache.h>
#include <linux/bitops.h>
#include <asm/arch/boot.h>


DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_LOAD_FIT
int __weak board_fit_config_name_match(const char *name)
{
	/* Just empty function now - can't decide what to choose */
	debug("%s: %s\n", __func__, name);

	return 0;
}
#endif

#ifdef CONFIG_SPL_BUILD

void board_init_f(ulong dummy)
{
	arch_cpu_init();

	preloader_console_init();
}
#endif
