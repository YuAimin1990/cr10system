/*
 * (C) Copyright 2022 Artosyn Electronics Co., Ltd
 *
 * SPDX-License-Identifier: GPL-2.0+
 */
#ifndef SDRAM_AR93XX_H
#define SDRAM_AR93XX_H

#include <common.h>
#include <clk.h>
#include <debug_uart.h>
#include <dm.h>
#include <dt-structs.h>
#include <init.h>
#include <log.h>
#include <ram.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <fdt.h>
#include <linux/libfdt.h>
#include <blk.h>
#include <mmc.h>
#include <memalign.h>
#include "dwc_ddrphy_phyinit_userCustom.h"
#include <artosyn/ar_common.h>
#include <asm/armv8/mmu.h>
#include <asm/arch/image.h>
#include <asm/spl.h>

//#define AR9301_SDRAM_DEBUG
//#define AR9301_SDRAM_DIAG

#define DEVINIT			0
#define TRAIN1D			1
#define TRAIN2D			2

#define DDR_CON12		0x0030
#define DDR_CON14		0x0038
#define DDR_CON22		0x0058
#define DDR_CON23		0x005c

#define DDR_MON_CTRL		0x0000
#define DDR_MON_PERIOD		0x0004
#define AXI_MON_CTRL		0x0000
#define AXI_MON_PERIOD		0x0004

#define DDRPHY_IMEM_OFFSET		0x00140000
#define DDRPHY_DMEM_OFFSET		0x00150000
#define DDRPHY_DIAG_DMEM_OFFSET		0x00150800
#define DDRPHY_DIAG_DATA_OFFSET		0x0015082c
#define DDRPHY_PIE_OFFSET		0x00240000

/* here is offset in emmc, in nor and nand, put it after gpt temporarily */
#define DDRPHY_FLASH_ALIGN		512
#define DDRPHY_FLASH_EMMC_HEADER_OFFSET	512 * 1024
#define DDRPHY_FLASH_NOR_HEADER_OFFSET 	0xa0000 /* romcode + spl0 + spl1 + gpt0 */
#define DDRPHY_FLASH_1D_IMEM_OFFSET	0x00040000
#define DDRPHY_FLASH_1D_DMEM_OFFSET	0x00050000
#define DDRPHY_FLASH_2D_IMEM_OFFSET	0x00060000
#define DDRPHY_FLASH_2D_DMEM_OFFSET	0x00070000
#define DDRPHY_FLASH_DIAG_IMEM_OFFSET	0x00080000
#define DDRPHY_FLASH_DIAG_DMEM_OFFSET	0x00090000

/* Data offset in RTC SRAM */
#define RET_REG_NUM_OFFSET		0x10
#define RET_STATE_OFFSET		0x14
#define RET_REG_LIST_OFFSET		0x20

#if defined(CONFIG_ARTOSYN_AR9301)
#define CEVA_PWR_CTRL		(0x010c3060)
#define CEVA_PWR_STAT		(0x010c3058)
#define CEVA_CORE_0_RGU		(0x010c1004)
#define CEVA_CORE_0_REG		(0x010c3000)
#define CEVA_SRAM_ADDR		(0x01800000)

#define PMU_BASE		(0x01075000)
#define  PMU_CTRL_OFFSET	(0x00)
#define  IP_PMU_CTRL_OFFSET	(0x04)

#define DDRPLL_DIG_REG0_ADDR	0x01072a00
#define DDRPLL_DIG_REG1_ADDR	0x01072a04
#define DDRPLL_DIG_REG_ADDR	0x01072a0c
#define DDRPLL_ANA_REG_ADDR	0x06010004
#define DDRPLL_DIG_REG_PLL_LOCK_MASK		(0x1)

#define ABB_CORE_REG6_ADDR	0x01072018
#define ABB_CORE_REG9_ADDR	0x01072024
#define SAR10_WORK_MODE		0x01072800
#define SAR10_CHAN_SEL		0x01072804
#define SAR10_DATA		    0x01072810
#define SAR10_CALIBRATION   0x0107202c

#define BL31_WARM_ENTRYPOINT	0
#define RET_DDR_BASE_ADDR	0
#define RET_REG_NUM_ADDR	0
#define RET_STATE_ADDR		0
#define RET_REG_LIST_BASE_ADDR	0

#define RET_FLASH_DATA_ADDR	0
#define RET_FLASH_DATA_SIZE	0

#elif defined(CONFIG_ARTOSYN_AR9311)
#define DDRPLL_DIG_REG0_ADDR	0x0a108400
#define DDRPLL_DIG_REG1_ADDR	0x0a108404
#define DDRPLL_DIG_REG_ADDR	0x0a108100
#define DDRPLL_ANA_REG_ADDR	0x0e010004
#define DDRPLL_DIG_REG_PLL_LOCK_MASK		(0x1 << 4)

#define ABB_CORE_REG0_ADDR	0x0a108000
#define ABB_CORE_REG6_ADDR	0x0a108018
#define ABB_CORE_REG9_ADDR	0x0a108024
#define SAR10_WORK_MODE		0x0a108200
#define SAR10_CHAN_SEL		0x0a108204
#define SAR10_DATA		0x0a108210
#define SAR10_CALIBRATION   0x0a10802c

/* Data addr in RTC SRAM */
#define BL31_WARM_ENTRYPOINT		RTC_SRAM_BASE
#define RET_REG_NUM_ADDR		(RTC_SRAM_BASE + RET_REG_NUM_OFFSET)
#define RET_STATE_ADDR			(RTC_SRAM_BASE + RET_STATE_OFFSET)
#define RET_REG_LIST_BASE_ADDR		(RTC_SRAM_BASE + RET_REG_LIST_OFFSET)
#define RET_FLASH_DATA_ADDR		(RTC_SRAM_BASE)
#define RET_FLASH_DATA_SIZE		(2048)
#endif

#define RET_FLASH_STATE_VALID		(0x12345678)
#define RET_FLASH_STATE_INVALID		(0x87654321)

#define DDRPLL_DIG_REG1_SSC_EN_SHIFT		10
#define DDRPLL_DIG_REG1_SSC_DIVN_SHIFT		12
#define DDRPLL_DIG_REG1_SSC_NUM_STEP_SHIFT	15
#define DDRPLL_DIG_REG1_SSC_KDELTA_SHIFT	18
#define DDRPLL_DIG_REG1_SSC_MODE_SHIFT		24
#define DDRPLL_DIG_REG1_SSC_EN_MASK		(0x1 << 10)
#define DDRPLL_DIG_REG1_SSC_DIVN_MASK		(0x7 << 12)
#define DDRPLL_DIG_REG1_SSC_NUM_STEP_MASK	(0x7 << 15)
#define DDRPLL_DIG_REG1_SSC_KDELTA_MASK		(0x3f << 18)
#define DDRPLL_DIG_REG1_SSC_MODE_MASK		(0x3 << 24)
#define DDRPLL_ANA_REG_LOOP_DIV_SHIFT		16
#define DDRPLL_ANA_REG_POST_DIV_SHIFT		19

#define ADC_DEFAULT_A				(2024 - 0)
#define ADC_DEFAULT_B				(0x3ff - 0x0)
#define ADC_DEFAULT_C				(0)
#define ADC_VALUE_0V3				(300)
#define ADC_VALUE_0V6				(600)
#define ADC_VALUE_0V				(0)
#define ADC_VALUE_THRES				(100)

#define AR_ADC_AUTOCALIB_VOLT       (900)//mv
#define AR_ADC_AUTOCALIB_EN         (0x1 << 3)

typedef struct artosyn_dram_params {
	unsigned int type;

	unsigned int freq0;
	unsigned int freq1;
	//unsigned int width;
	unsigned int density;

	/* RFSH_1X:0 RFSH_2X:1 others:2 */
	unsigned int refresh_mode;
	unsigned int mr1_odic;
	unsigned int mr1_rtt_nom;
	unsigned int mr4_rdpreamble;
	unsigned int mr4_wrpreamble;
	unsigned int mr5_rd_dbi;
	unsigned int mr5_wr_dbi;
	unsigned int mr5_dm;
	unsigned int mr6_dram_vref;
	unsigned int ddr_hbm;
	unsigned int clk_mode;

	/* QOS register value */
	unsigned int sched;
	unsigned int sched1;
	unsigned int perfhpr1;
	unsigned int perflpr1;
	unsigned int perfwr1;
	unsigned int pccfg;
	unsigned int pcfgr_0;
	unsigned int pcfgr_1;
	unsigned int pcfgr_2;
	unsigned int pcfgw_0;
	unsigned int pcfgw_1;
	unsigned int pcfgw_2;
	unsigned int pcfgqos0_0;
	unsigned int pcfgqos0_1;
	unsigned int pcfgqos0_2;
	unsigned int pcfgqos1_0;
	unsigned int pcfgqos1_1;
	unsigned int pcfgqos1_2;
	unsigned int pcfgwqos0_0;
	unsigned int pcfgwqos0_1;
	unsigned int pcfgwqos0_2;
	unsigned int pcfgwqos1_0;
	unsigned int pcfgwqos1_1;
	unsigned int pcfgwqos1_2;
}  __attribute__ ((packed)) artosyn_dram_params_t;

typedef struct artosyn_dphy_pll_cfgs {
	unsigned int ddr_mem_freq;
	unsigned int loop_div;
	unsigned int post_div;
	unsigned int ssc_en;
	unsigned int ssc_mode;
	unsigned int ssc_kdelta;
	unsigned int ssc_num_step;
	unsigned int ssc_divn;
} __attribute__ ((packed)) artosyn_dphy_pll_cfgs_t;

typedef struct artosyn_dphy_misc {
	unsigned int msg_misc;
	unsigned int phy_vref;
	unsigned int hdt_ctrl;
	unsigned int seq_ctrl_1d[4];
	unsigned int seq_ctrl_2d[4];
} __attribute__ ((packed)) artosyn_dphy_misc_t;

typedef struct artosyn_vendor_header {
	unsigned int magic;
	unsigned int version;
	unsigned int header_checksum;
	unsigned int imem_offset_1d;
	unsigned int imem_size_1d;
	unsigned int imem_sum_1d;
	unsigned int dmem_offset_1d;
	unsigned int dmem_size_1d;
	unsigned int dmem_sum_1d;
	unsigned int imem_offset_2d;
	unsigned int imem_size_2d;
	unsigned int imem_sum_2d;
	unsigned int dmem_offset_2d;
	unsigned int dmem_size_2d;
	unsigned int dmem_sum_2d;
	unsigned int imem_offset_diag;
	unsigned int imem_size_diag;
	unsigned int imem_sum_diag;
	unsigned int dmem_offset_diag;
	unsigned int dmem_size_diag;
	unsigned int dmem_sum_diag;
	unsigned int ddr3_imem_offset_1d;
	unsigned int ddr3_imem_size_1d;
	unsigned int ddr3_imem_sum_1d;
	unsigned int ddr3_dmem_offset_1d;
	unsigned int ddr3_dmem_size_1d;
	unsigned int ddr3_dmem_sum_1d;
} __attribute__ ((packed)) artosyn_vendor_header_t;

struct artosyn_dmc_plat {
	struct regmap *map;
};

const char *user_modify_name[5] = {"user-modify0",
				   "user-modify1",
				   "user-modify2",
				   "user-modify3",
				   "user-modify4"};

#ifdef CONFIG_SPL_BUILD
typedef struct artosyn_dram_configs {
	// config params base on input params from dts
	unsigned int tck_freq;
	unsigned int tcl;
	unsigned int trcd;
	unsigned int trp;
	unsigned int tcwl;

	unsigned int trefi;
	unsigned int trfc;
	unsigned int clk_mode;
	unsigned int wr_preamble;
	unsigned int rd_preamble;
} __attribute__ ((packed)) artosyn_dram_configs_t;;

typedef struct artosyn_dram_ctrl {
	// dfi calculate result base on config params
	unsigned int dfi_t_ctrl_delay;
	unsigned int dfi_t_rddata_en;
	unsigned int dfi_tphy_wrdata;
	unsigned int dfi_tphy_wrlat;
	unsigned int dfi_tphy_rdcslat;
	unsigned int dfi_tphy_wrcslat;
	unsigned int dfi_t_wrdata_delay;
	unsigned int dfi_t_dram_clk_enable;
	unsigned int dfi_t_dram_clk_disable;

	// time calculate result base on config params
	unsigned int t_rfc_nom_x1_x32;
	unsigned int t_rfc_min;

	unsigned int t_wl;
	unsigned int t_rl;
	unsigned int t_wr;
	unsigned int t_wr2pre;
	unsigned int t_faw;
	unsigned int t_ras_min;
	unsigned int t_ras_max;
	unsigned int t_xp;
	unsigned int t_rd2pre;
	unsigned int t_rc;
	unsigned int t_rd2wr;
	unsigned int t_wr2rd;
	unsigned int t_wr2rd_s;
	unsigned int t_mrw;
	unsigned int t_mrd;
	unsigned int t_mod;
	unsigned int t_rcd;
	unsigned int t_ccd_l;
	unsigned int t_ccd_s;
	unsigned int t_rrd_l;
	unsigned int t_rrd_s;
	unsigned int t_rp;
	unsigned int t_cksrx;
	unsigned int t_cksre;
	unsigned int t_cke;
	unsigned int t_ckesr;
	unsigned int t_ckpde;
	unsigned int t_ckpdx;
	unsigned int t_xs_fast_x32;
	unsigned int t_xs_abort_x32;
	unsigned int t_xs_x32;
	unsigned int t_xs_dll_x32;
	unsigned int t_sync_gear;
	unsigned int t_cmd_gear;
	unsigned int t_gear_setup;
	unsigned int t_gear_hold;
	unsigned int t_post_mpsm_x32;
	unsigned int t_mpx_lh;
	unsigned int t_mpx_s;
	unsigned int t_ckmpe;
	unsigned int t_cmdcke;
	unsigned int t_mrd_pda;
	unsigned int wr_preamble;
	unsigned int rd_preamble;
	unsigned int clk_mode;

	unsigned int t_zqinitl;
	unsigned int t_zqoper;
	unsigned int t_zqcs;

	// mode register calculate result base on config params
	unsigned int mr0_reg;
	unsigned int mr1_reg;
	unsigned int mr2_reg;
	unsigned int mr3_reg;
	unsigned int mr4_reg;
	unsigned int mr5_reg;
	unsigned int mr6_reg;

	unsigned int wr_odt_hold;
	unsigned int wr_odt_delay;
	unsigned int rd_odt_hold;
	unsigned int rd_odt_delay;
} __attribute__ ((packed)) ar9301_dram_ctrl_t;
#endif

#endif
