/*
 * (C) Copyright 2007-2011
 * Artosyn Technology Co., Ltd. <www.artosyn.com>
 *  
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
#ifdef CONFIG_ARTOSYN_AR9301
	puts("CPU:   Artosyn POLESTAR 9301\n");
#elif defined CONFIG_ARTOSYN_AR9311
	puts("CPU:   Artosyn PROXIMA 9311\n");
#endif
	return 0;
}
#endif
