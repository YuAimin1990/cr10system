/*
 * Copyright (c) 2014, artosyn Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/mmc/host.h>
#include <linux/mmc/dw_mmc.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include "dw_mmc.h"
#include "dw_mmc-pltfm.h"

#define SDMMC_MAX_PHASES		16

#define POLESTAR_SDMMC_PLL		(0x01072088)
#define POLESTAR_SDMMC_PLL_PHASE	(0x0107208C)
#define PERIP_CLK_CTRL			(0x01070020)
#define POLESTAR_SDMMC_PB_MSC_REG00	(0x01073104)

#define PROXIMA_SDMMC0_PLL		(0x0A108088)
#define PROXIMA_SDMMC0_PLL_PHASE	(0x0A10808C)
#define PROXIMA_SDMMC1_PLL		(0x0A1080C0)
#define PROXIMA_SDMMC1_PLL_PHASE	(0x0A1080C4)
#define PROXIMA_GPIO_MSC		(0x0A10A000)
#define  PROXIMA_GPIO_MSC_SDMMC0_MASK	(1 << 3)
#define  PROXIMA_GPIO_MSC_SDMMC1_MASK	(1 << 4)

static unsigned int max_tuning_phases;
static unsigned int init_sample_phase;

struct dw_mci_artosyn_priv_data {
	void __iomem *sdmmc_pll_phase;
	void __iomem *sdmmc_pll;
	void __iomem *pd_pwr_domain;
	int phase_mask;
	int phase_current;
	int phase_slot[3];
	unsigned int pwr_domain_mask;
};

/*
 * Find out the greatest range of consecuitive selected
 * DLL clock output phases that can be used as sampling
 * setting for SD3.0 UHS-I card read operation (in SDR104
 * timing mode) or for eMMC4.5 card read operation (in HS200
 * timing mode).
 * Select the 3/4 of the range and configure the DLL with the
 * selected DLL clock output phase.
 */
static int ar_find_most_appropriate_phase(struct dw_mci_slot *slot, unsigned char *phase_table, unsigned char total_phases)
{
	int ret;
	unsigned char ranges[SDMMC_MAX_PHASES][SDMMC_MAX_PHASES] = { {0}, {0} };
	unsigned char phases_per_row[SDMMC_MAX_PHASES] = { 0 };
	int row_index = 0, col_index = 0, selected_row_index = 0, curr_max = 0;
	int i, cnt;

	memset(phases_per_row, 0, SDMMC_MAX_PHASES);
	memset(ranges, 0, SDMMC_MAX_PHASES*SDMMC_MAX_PHASES);

	for (cnt = 0; cnt < total_phases; cnt++) {
		ranges[row_index][col_index] = phase_table[cnt];
		phases_per_row[row_index] += 1;
		col_index++;

		if ((phase_table[cnt] + 1) != phase_table[cnt + 1]) {
			row_index++;
			col_index = 0;
		}
	}

	if (row_index >= max_tuning_phases)
		return -1;

	for (cnt = 0; cnt < row_index; cnt++) {
		if (phases_per_row[cnt] > curr_max) {
			curr_max = phases_per_row[cnt];
			selected_row_index = cnt;
		}
	}

	i = (curr_max) / 2;

	ret = ranges[selected_row_index][i];


	return ret;
}

static int ar_config_cm_dll_phase(struct dw_mci_slot *slot, u8 phase)
{
	int rc = 0;
	struct dw_mci *host = slot->host;
	struct dw_mci_artosyn_priv_data *priv = host->priv;

	static const u8 grey_coded_phase_table[SDMMC_MAX_PHASES] = {
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
		0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
	};

	u32 pll_val = 0;;

	/*config phase*/
	pll_val = readl(priv->sdmmc_pll_phase);
	pll_val &= ~(0x1f);
	pll_val |= grey_coded_phase_table[phase];
	writel(pll_val, priv->sdmmc_pll_phase);

	mdelay(1);

	return rc;
}

static void dw_mci_artosyn_set_phase(struct dw_mci_slot *slot)
{
	struct dw_mci *host = slot->host;
	struct dw_mci_artosyn_priv_data *priv = host->priv;
	u32 div;
	u32 pll_val;

	if (slot->mmc->ios.timing == MMC_TIMING_LEGACY ||
	    slot->mmc->ios.timing == MMC_TIMING_MMC_HS ||
	    slot->mmc->ios.timing == MMC_TIMING_SD_HS ||
	    slot->mmc->ios.timing == MMC_TIMING_UHS_SDR12 ||
	    slot->mmc->ios.timing == MMC_TIMING_UHS_SDR25)
		priv->phase_slot[slot->id] = init_sample_phase;

	if (priv->phase_current != priv->phase_slot[slot->id])
		ar_config_cm_dll_phase(slot, priv->phase_slot[slot->id]);

	priv->phase_current = priv->phase_slot[slot->id];

	/*
	 * Config drive clock phase according to clock frequence
	 * Make up for the lack of invert function of drive clock
	 */
	if (slot->clock) {
		div = host->bus_hz / slot->clock;
		if (host->bus_hz % slot->clock && host->bus_hz > slot->clock)
			/*
			 * move the + 1 after the divide to prevent
			 * over-clocking the card.
			 */
			div += 1;

		div = (host->bus_hz != slot->clock) ? DIV_ROUND_UP(div, 2) : 0;
		pll_val = readl(priv->sdmmc_pll);
		pll_val &= ~(0x3F);
		if (div)
			pll_val |= (max_tuning_phases - 1);
		else
			pll_val |= (max_tuning_phases / 2);
		writel(pll_val, priv->sdmmc_pll);
	}
}

static int dw_mci_sirius_execute_tuning(struct dw_mci_slot *slot, u32 opcode)
{
	int tuning_seq_cnt = 3;
	u8 phase, tuned_phases[SDMMC_MAX_PHASES], tuned_phase_cnt = 0;
	int rc;
	struct dw_mci *host = slot->host;
	struct dw_mci_artosyn_priv_data *priv = host->priv;
	struct mmc_host *mmc = slot->mmc;

#ifdef FPGA_SD
	return 0;
#endif

#ifdef VELOCE_SD
	return 0;
#endif

retry:
	phase = 0;
	do {
		if (priv->phase_mask & (0x1 << phase))
			continue;

		priv->phase_slot[slot->id] = phase;

		rc = mmc_send_tuning(mmc, opcode, NULL);
		if (!rc) {
			/* Tuning is successful at this tuning point */
			tuned_phases[tuned_phase_cnt++] = phase;
			dev_info(mmc_dev(mmc), "%s: Found good phase = %d\n",
				 mmc_hostname(mmc), phase);
		} else {
			priv->phase_mask |= 0x1 << phase;
		}
	} while (++phase < max_tuning_phases);

	if (tuned_phase_cnt) {
		rc = ar_find_most_appropriate_phase(slot, tuned_phases,
						     tuned_phase_cnt);
		if (rc < 0)
			return rc;
		else
			phase = rc;

		dev_info(mmc_dev(mmc), "%s: Setting the tuning phase to %d\n",
			 mmc_hostname(mmc), phase);
		priv->phase_slot[slot->id] = phase;
		rc = 0;
	} else {
		if (--tuning_seq_cnt)
			goto retry;
		/* Tuning failed */
		dev_err(mmc_dev(mmc), "%s: No tuning point found\n",
		       mmc_hostname(mmc));
		rc = -EIO;
	}

	return rc;
}

static int dw_mci_sirius_parse_dt(struct dw_mci *host)
{

	struct dw_mci_artosyn_priv_data *priv;

	priv = devm_kzalloc(host->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->sdmmc_pll = devm_ioremap(host->dev, POLESTAR_SDMMC_PLL, 0x4);
	if (!priv->sdmmc_pll) {
		dev_err(host->dev, "ioremap failed for resource POLESTAR_SDMMC_PLL\n");
		priv->sdmmc_pll_phase = IOMEM_ERR_PTR(-ENOMEM);
	}

	priv->sdmmc_pll_phase = devm_ioremap(host->dev, POLESTAR_SDMMC_PLL_PHASE, 0x4);
	if (!priv->sdmmc_pll_phase) {
		dev_err(host->dev, "ioremap failed for resource POLESTAR_SDMMC_PLL_PHASE\n");
		priv->sdmmc_pll_phase = IOMEM_ERR_PTR(-ENOMEM);
	}

	priv->pd_pwr_domain = devm_ioremap(host->dev, POLESTAR_SDMMC_PB_MSC_REG00, 0x4);
	if (!priv->pd_pwr_domain) {
		dev_err(host->dev, "ioremap failed for resource POLESTAR_SDMMC_PB_MSC_REG00\n");
		priv->pd_pwr_domain = IOMEM_ERR_PTR(-ENOMEM);
	}

	host->priv = priv;

	return 0;
}

static int dw_mci_artosyn_init(struct dw_mci *host)
{
	/* It is slot 0 on artosyn SoCs */
	struct dw_mci_artosyn_priv_data *priv = host->priv;
	unsigned int clk = 0;
	unsigned int tmp = 0;
	unsigned int pll_val = 0;
	void __iomem *clk_ctrl = ioremap(PERIP_CLK_CTRL, 0x4);

	host->sdio_id0 = 0;

#ifdef FPGA_SD
	return 0;
#endif

#ifdef VELOCE_SD
	return 0;
#endif

	/*config phase 0*/
	pll_val = readl(priv->sdmmc_pll_phase);
	pll_val &= ~(0x1f);
	writel(pll_val, priv->sdmmc_pll_phase);

	dev_dbg(host->dev, "config phase 0\n");

	if(50000000 == host->bus_hz) {
		clk = 0;
	} else if(100000000 == host->bus_hz) {
		clk = 1;
	} else if(200000000 == host->bus_hz) {
		clk = 3;
	} else {
		host->bus_hz = 200000000;
		clk = 3;
	}

	writel((clk << 6), priv->sdmmc_pll);
	mdelay(2);

	if (!clk_ctrl) {
		dev_err(host->dev, "ioremap failed for resource PERIP_CLK_CTRL\n");
		return -ENOMEM;
	} else {
		tmp = readl(clk_ctrl);
		tmp &= (~ BIT(10));
		writel(tmp, clk_ctrl);
		iounmap(clk_ctrl);
	}

	mdelay(2);

	max_tuning_phases = 16;
	init_sample_phase = 6;

	return 0;
}

int dw_mci_artosyn_switch_voltage(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct dw_mci_slot *slot = mmc_priv(mmc);
	struct dw_mci *host = slot->host;
	struct dw_mci_artosyn_priv_data *priv = slot->host->priv;
	struct device_node *np;
	u32 v18 = SDMMC_UHS_18V << slot->id;
	u32 slot_bitmap_fixed_1v8;
	u32 slot_bitmap_reg;
	u32 uhs;
	u32 pb_vol;
	int ret;

	np = mmc->parent->of_node;

	if (of_property_read_u32(np, "slot-bitmap-fixed-1v8",
				 &slot_bitmap_fixed_1v8) < 0) {
		dev_dbg(mmc->parent,
			"\"slot-bitmap-fixed-1v8\" property is missing,\
			assuming normal.\n");
		slot_bitmap_fixed_1v8 = 0;
	}

	if (of_property_read_u32(np, "slot-bitmap-reg",
				 &slot_bitmap_reg) < 0) {
		dev_dbg(mmc->parent,
			"\"slot-bitmap-reg\" property is missing,\
			assuming normal.\n");
		slot_bitmap_reg = 0x1;
	}
	/*
	 * Program the voltage.  Note that some instances of dw_mmc may use
	 * the UHS_REG for this.  For other instances (like exynos) the UHS_REG
	 * does no harm but you need to set the regulator directly.  Try both.
	 */
	pb_vol = readl(priv->pd_pwr_domain);
	uhs = mci_readl(host, UHS_REG);
	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_330) {
		uhs &= ~v18;
		pb_vol &= ~v18;
	} else {
		uhs |= v18;
		pb_vol |= v18;
	}

	if (slot_bitmap_fixed_1v8 & (0x1 << slot->id)) {
		uhs |= v18;
		pb_vol |= v18;
		ios->signal_voltage = MMC_SIGNAL_VOLTAGE_180;
	}

#ifndef VELOCE_SD
	if (!IS_ERR(mmc->supply.vqmmc) &&
	    (slot_bitmap_reg & (0x1 << slot->id))) {
		ret = mmc_regulator_set_vqmmc(mmc, ios);

		if (ret) {
			dev_dbg(&mmc->class_dev,
					 "Regulator set error %d - %s V\n",
					 ret, uhs & v18 ? "1.8" : "3.3");
			return ret;
		}
	}
#endif
	mci_writel(host, UHS_REG, uhs);
	writel(pb_vol, priv->pd_pwr_domain);

	return 0;
}

static const struct dw_mci_drv_data sirius_drv_data = {
	.init			= dw_mci_artosyn_init,
	.parse_dt		= dw_mci_sirius_parse_dt,
	.execute_tuning		= dw_mci_sirius_execute_tuning,
	.switch_voltage		= dw_mci_artosyn_switch_voltage,
	.set_phase		= dw_mci_artosyn_set_phase,
};

static int dw_mci_proxima_init(struct dw_mci *host)
{
	/* It is slot 0 on artosyn SoCs */
	struct dw_mci_artosyn_priv_data *priv = host->priv;
	unsigned int clk = 0;
	unsigned int pll_val = 0;

	host->sdio_id0 = 0;

#ifdef FPGA_SD
	return 0;
#endif

#ifdef VELOCE_SD
	return 0;
#endif

	/* config phase 0 */
	pll_val = readl(priv->sdmmc_pll_phase);
	pll_val &= ~(0x1f);
	writel(pll_val, priv->sdmmc_pll_phase);

	dev_dbg(host->dev, "config phase 0\n");

	if (25000000 == host->bus_hz) {
		clk = 0;
	} else if (50000000 == host->bus_hz) {
		clk = 1;
	} else if(100000000 == host->bus_hz) {
		clk = 2;
	} else if(200000000 == host->bus_hz) {
		clk = 3;
	} else {
		host->bus_hz = 200000000;
		clk = 3;
	}

	writel(clk << 6, priv->sdmmc_pll);
	mdelay(2);

	max_tuning_phases = 8;
	init_sample_phase = 2;

	return 0;
}

static int dw_mci_proxima_parse_dt(struct dw_mci *host)
{

	struct dw_mci_artosyn_priv_data *priv;
	size_t pll_addr, phase_addr, pwr_domain_addr;
	struct device *dev = host->dev;
	struct device_node *np = dev->of_node;

	priv = devm_kzalloc(host->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	if (of_device_is_compatible(np, "dwmmc0")) {
		pll_addr = PROXIMA_SDMMC0_PLL;
		phase_addr = PROXIMA_SDMMC0_PLL_PHASE;
		priv->pwr_domain_mask = PROXIMA_GPIO_MSC_SDMMC0_MASK;
	} else if (of_device_is_compatible(np, "dwmmc1")){
		pll_addr = PROXIMA_SDMMC1_PLL;
		phase_addr = PROXIMA_SDMMC1_PLL_PHASE;
		priv->pwr_domain_mask = PROXIMA_GPIO_MSC_SDMMC1_MASK;
	} else {
		dev_err(host->dev, "unknow dwmmc compatible index\n");
		return -ENOMEM;
	}
	pwr_domain_addr = PROXIMA_GPIO_MSC;

	priv->sdmmc_pll = devm_ioremap(host->dev, pll_addr, 0x4);
	if (!priv->sdmmc_pll) {
		dev_err(host->dev, "ioremap failed for resource pll_addr\n");
		priv->sdmmc_pll_phase = IOMEM_ERR_PTR(-ENOMEM);
	}

	priv->sdmmc_pll_phase = devm_ioremap(host->dev, phase_addr, 0x4);
	if (!priv->sdmmc_pll_phase) {
		dev_err(host->dev, "ioremap failed for resource phase_addr\n");
		priv->sdmmc_pll_phase = IOMEM_ERR_PTR(-ENOMEM);
	}

	priv->pd_pwr_domain = devm_ioremap(host->dev, pwr_domain_addr, 0x4);
	if (!priv->pd_pwr_domain) {
		dev_err(host->dev, "ioremap failed for resource pwr_domain_addr\n");
		priv->pd_pwr_domain = IOMEM_ERR_PTR(-ENOMEM);
	}

	host->priv = priv;

	return 0;
}

int dw_mci_proxima_switch_voltage(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct dw_mci_slot *slot = mmc_priv(mmc);
	struct dw_mci *host = slot->host;
	struct dw_mci_artosyn_priv_data *priv = slot->host->priv;
	struct device_node *np;
	u32 slot_bitmap_fixed_1v8;
	u32 v18 = SDMMC_UHS_18V << slot->id;
	u32 uhs;
	u32 pb_pwr;
	int ret;

	np = mmc->parent->of_node;

	if (of_property_read_u32(np, "slot-bitmap-fixed-1v8",
				 &slot_bitmap_fixed_1v8) < 0) {
		dev_dbg(mmc->parent,
			"\"slot-bitmap-fixed-1v8\" property is missing,\
			assuming normal.\n");
		slot_bitmap_fixed_1v8 = 0;
	}
	/*
	 * Program the voltage.  Note that some instances of dw_mmc may use
	 * the UHS_REG for this.  For other instances (like exynos) the UHS_REG
	 * does no harm but you need to set the regulator directly.  Try both.
	 */
	pb_pwr = readl(priv->pd_pwr_domain);
	uhs = mci_readl(host, UHS_REG);
	if (ios->signal_voltage == MMC_SIGNAL_VOLTAGE_330) {
		uhs &= ~v18;
		pb_pwr &= ~(priv->pwr_domain_mask);
	} else {
		uhs |= v18;
		pb_pwr |= priv->pwr_domain_mask;
	}

	if (slot_bitmap_fixed_1v8 & (0x1 << slot->id)) {
		uhs |= v18;
		pb_pwr |= priv->pwr_domain_mask;
		ios->signal_voltage = MMC_SIGNAL_VOLTAGE_180;
	}

#ifndef VELOCE_SD
	if (!IS_ERR(mmc->supply.vqmmc)) {
		ret = mmc_regulator_set_vqmmc(mmc, ios);

		if (ret) {
			dev_dbg(&mmc->class_dev,
					 "Regulator set error %d - %s V\n",
					 ret, uhs & v18 ? "1.8" : "3.3");
			return ret;
		}
	}
#endif

	mci_writel(host, UHS_REG, uhs);
	writel(pb_pwr, priv->pd_pwr_domain);

	return 0;
}

static const struct dw_mci_drv_data proxima_drv_data = {
	.init			= dw_mci_proxima_init,
	.parse_dt		= dw_mci_proxima_parse_dt,
	.execute_tuning		= dw_mci_sirius_execute_tuning,
	.switch_voltage		= dw_mci_proxima_switch_voltage,
	.set_phase		= dw_mci_artosyn_set_phase,
};

static const struct of_device_id dw_mci_artosyn_match[] = {
	{ .compatible = "artosyn,sirius-dw-mshc",
		.data = &sirius_drv_data },
	{ .compatible = "artosyn,proxima-dw-mshc",
		.data = &proxima_drv_data },
	{},
};
MODULE_DEVICE_TABLE(of, dw_mci_artosyn_match);

static int dw_mci_artosyn_probe(struct platform_device *pdev)
{
	const struct dw_mci_drv_data *drv_data;
	const struct of_device_id *match;

	if (!pdev->dev.of_node)
		return -ENODEV;

	match = of_match_node(dw_mci_artosyn_match, pdev->dev.of_node);
	drv_data = match->data;

	return dw_mci_pltfm_register(pdev, drv_data);
}

static struct platform_driver dw_mci_artosyn_pltfm_driver = {
	.probe		= dw_mci_artosyn_probe,
	.remove		= dw_mci_pltfm_remove,
	.driver		= {
		.name		= "dwmmc_artosyn",
		.of_match_table	= dw_mci_artosyn_match,
		.pm		= &dw_mci_pltfm_pmops,
	},
};

module_platform_driver(dw_mci_artosyn_pltfm_driver);

MODULE_AUTHOR("Artosyn Software <min.zhao@artosyn.cn>");
MODULE_DESCRIPTION("artosyn Specific DW-MSHC Driver Extension");
MODULE_ALIAS("platform:dwmmc_artosyn");
MODULE_LICENSE("GPL v2");
