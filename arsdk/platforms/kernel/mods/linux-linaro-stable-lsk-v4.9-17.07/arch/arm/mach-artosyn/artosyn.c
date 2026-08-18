/*
 * Device Tree support for Artosyn SoCs
 *
 * Copyright (C) 2012 Artosyn Co.,Ltd.
 *
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach/arch.h>
#define SIRIUS_CORESIGHT_TIMESTAMP_CTRL  (0x60406000)
static void __init artosyn_dt_cpufreq_init(void)
{
	platform_device_register_simple("cpufreq-dt", -1, NULL, 0);
}

static const char * const sirius_board_dt_compat[] = {
	"artosyn,sirius-demo",
	NULL,
};
static void sirius_enable_timestamp(void)
{
	u32 val = 0;
	void __iomem * mem;

	mem = ioremap(SIRIUS_CORESIGHT_TIMESTAMP_CTRL, 0x1000);
	if (!mem)
	{
		pr_info("Map coresight timestamp reg error!\n");
		return;
	}
	val = readl(mem);
	val |= 0x1; //bit 0 is enable bit
	writel(val, mem);
	isb();
}
//extern void __init sirius_reset_init(void);
static void __init sirius_timer_init(void)
{
	//hbbai, we need to enable Coresight timestamp to generate counter
	sirius_enable_timestamp();
	of_clk_init(NULL);

//	if (IS_ENABLED(CONFIG_RESET_CONTROLLER))
//		sirius_reset_init();
	clocksource_probe();
}

DT_MACHINE_START(SIRIUS_DT, "Artosyn Sirius Family")
	.init_time	= sirius_timer_init,
	.dt_compat	= sirius_board_dt_compat,
	.init_late	= artosyn_dt_cpufreq_init,
//	.smp		= smp_ops(sirius_smp_dt_ops),
//	.smp_init	= smp_init_ops(sirius_smp_init_ops),
MACHINE_END

