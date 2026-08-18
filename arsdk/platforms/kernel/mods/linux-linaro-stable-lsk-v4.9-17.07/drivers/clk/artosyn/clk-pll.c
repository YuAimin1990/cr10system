#include <linux/bitfield.h>
//#include <linux/bits.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/slab.h>
#include <linux/jiffies.h>

#include "clk.h"

#define ARTOSYN_FREQ_OFFSET				(8)
#define ARTOSYN_FREQ_MASK				(0xff)
#define ARTOSYN_FREQ_UNIT				(10 * 1000 * 1000ULL)
#define ARTOSYN_FREQ_MINI				(4 * ARTOSYN_FREQ_UNIT)
#define ARTOSYN_PLL_DEGLITCH_AUTO		(1 << 25)

#define to_artosyn_clk_pll(_hw) container_of(_hw, struct artosyn_clk_pll, hw)

struct ar9301_a53_cgu {
	u32 cgu_pll_config;
	u32 cgu_pll_ctrl_1;
	u32 cgu_pll_ctrl_2;
	u32 cgu_trt_config;
	u32 cgu_trt_ctrl;
	u32 cgu_pvt_data;
	u32 cgu_pvt_ctrl;
	u32 cgu_ls_ctrl;
	u32 standby_status;
	u32 rm_ctrl;
	u32 cgu_reserved1;
	u32 cgu_reserved2;
	u32 global_0;
	u32 global_1;
	u32 global_2;
	u32 global_3;
};

struct artosyn_clk_pll {
	struct clk_hw		hw;
	void __iomem		*base;
	
};

static unsigned long artosyn_pll_recalc_rate(struct clk_hw *hw,
						     unsigned long prate)
{
	struct artosyn_clk_pll *pll = to_artosyn_clk_pll(hw);
	u64 rate64 = prate;

	rate64 = readl_relaxed(pll->base) & (ARTOSYN_FREQ_MASK << ARTOSYN_FREQ_OFFSET);
	rate64 >>= ARTOSYN_FREQ_OFFSET;
	rate64 *= ARTOSYN_FREQ_UNIT;

	return (unsigned long)rate64;
}

static long artosyn_pll_round_rate(struct clk_hw *hw,
			    unsigned long drate, unsigned long *prate)
{
	//struct artosyn_clk_pll *pll = to_artosyn_clk_pll(hw);

	drate /= ARTOSYN_FREQ_UNIT;
	drate &= ARTOSYN_FREQ_MASK;
	drate *= ARTOSYN_FREQ_UNIT;

	if(drate < ARTOSYN_FREQ_MINI)
		drate = ARTOSYN_FREQ_MINI;

	return drate ;
}

static void reg_set_bit(unsigned long address, unsigned int bit_value)
{
	unsigned int val;
	val = readl_relaxed(address);
	val |= bit_value;
	writel_relaxed(val, address);
}

static void reg_clr_bit(unsigned long address, unsigned int bit_value)
{
	unsigned int val;
	val = readl_relaxed(address);
	val &= (~bit_value);
	writel_relaxed(val, address);
}

static int artosyn_pll_set_rate(struct clk_hw *hw, unsigned long drate,
					unsigned long prate)
{
	struct artosyn_clk_pll *pll = to_artosyn_clk_pll(hw);
	u32	val = 0;
	struct ar9301_a53_cgu *a53_cgu = pll->base;
	uint32_t tmp_freq = 0;
	int i = 0;

	pr_debug("%s: changing %s to %lu with a parent rate of %lu\n",
		 __func__, __clk_get_name(hw->clk), drate, prate);

	drate = artosyn_pll_round_rate(hw, drate, &prate);

	// Switch to backup vco
	reg_set_bit(&a53_cgu->cgu_pll_config, (0x1 << 26));
	reg_set_bit(&a53_cgu->cgu_pll_config, (0x1 << 27));
	reg_set_bit(&a53_cgu->cgu_reserved1, (0xf));
	udelay(5);

	reg_set_bit(&a53_cgu->cgu_pll_config, (0x1 << 24));
	udelay(5);

	// Switch to 900M first and then higher freq to avoid vctrl pulse
	tmp_freq = drate <= 900000000 ? drate : 900000000;

	val = (readl_relaxed(&a53_cgu->cgu_pll_config) & 0xffff00ff);
	val |= (tmp_freq / ARTOSYN_FREQ_UNIT) << 8;
	writel_relaxed(val, &a53_cgu->cgu_pll_config);

	reg_clr_bit(&a53_cgu->cgu_pll_config, (0x1));
	udelay(5);

	//wait for 900M lock
	for(i = 0; i < 10; i++) {
		if (readl_relaxed(&a53_cgu->cgu_pll_ctrl_1) & 0x1)
			break;

		if (i == 9) {
		    printk(KERN_INFO "lock failed, i = %d freq:%d\n", i, tmp_freq);
            return -1;
		}
		udelay(10);
	}
	udelay(5);

	//Switch to higher freq starting from 900M
	if (drate - tmp_freq > 0) {
		val = (readl_relaxed(&a53_cgu->cgu_pll_config) & 0xffff00ff);
		val |= (drate / ARTOSYN_FREQ_UNIT) << 8;
		writel_relaxed(val, &a53_cgu->cgu_pll_config);

		reg_clr_bit(&a53_cgu->cgu_pll_config, (0x1));
		udelay(5);

		for(i = 0; i < 10; i++) {
			if (readl_relaxed(&a53_cgu->cgu_pll_ctrl_1) & 0x1)
				break;

			if (i == 9) {
			    printk(KERN_INFO "lock failed, i = %d freq:%d\n", i, drate);
	            return -1;
			}
			udelay(10);
		}
    }
	//printf("pll lock succeed to %d MHz\n", drate / 1000000);

	//switch back to master vco
	reg_clr_bit(&a53_cgu->cgu_pll_config, (0x1 << 24));
	reg_clr_bit(&a53_cgu->cgu_pll_config, (0x1 << 26));

	return 0;
}

static const struct clk_ops artosyn_pll_clk_ops = {
	.recalc_rate = artosyn_pll_recalc_rate,
	.round_rate = artosyn_pll_round_rate,
	.set_rate = artosyn_pll_set_rate,
};

struct clk * artosyn_clk_register_pll(const char *name,
                    const char *parent_name, void __iomem *base,
                    u8 clk_pll_flags)
{
    struct artosyn_clk_pll 	*pll;
	struct clk 				*clk;
	struct clk_hw			*hw;
	struct clk_init_data	init;
	//int 					ret;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if(!pll)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.flags = clk_pll_flags;
	init.parent_names = &parent_name;
	init.num_parents = 1;
	init.ops = &artosyn_pll_clk_ops;

	pll->base = base;
	pll->hw.init = &init;

	hw = &pll->hw;

	clk = clk_register(NULL, hw);
	if (IS_ERR(clk)) {
		pr_err("%s: failed to register pll clock %s : %ld\n",
			__func__, name, PTR_ERR(clk));
		goto err_pll;
	}

	return clk;
err_pll:
	kfree(pll);
	return clk;
}
EXPORT_SYMBOL_GPL(artosyn_clk_register_pll);
