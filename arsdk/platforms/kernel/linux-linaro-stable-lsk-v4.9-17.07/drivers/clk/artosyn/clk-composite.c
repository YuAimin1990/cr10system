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

#define ARTOSYN_FREQ_SELECT				(30)
#define ARTOSYN_FREQ_SELECT_PRE			(31)

#define ARTOSYN_FREQ_CLK_B_EN			(28)
#define ARTOSYN_FREQ_CLK_B_SEL			(24)
#define ARTOSYN_FREQ_CLK_B_DIV			(16)

#define ARTOSYN_FREQ_CLK_A_EN			(12)
#define ARTOSYN_FREQ_CLK_A_SEL			(8)
#define ARTOSYN_FREQ_CLK_A_DIV			(0)

//#define ARTOSYN_FREQ_MASK				(0xff)
//#define ARTOSYN_FREQ_UNIT				(10 * 1000 * 1000ULL)
//#define ARTOSYN_FREQ_MINI				(4 * ARTOSYN_FREQ_UNIT)
//#define ARTOSYN_PLL_DEGLITCH_AUTO		(1 << 25)

#define to_artosyn_clk_composite(_hw) container_of(_hw, struct artosyn_clk_composite, hw)

struct artosyn_clk_composite {
	struct clk_hw		hw;
	void __iomem		*base;
};

static unsigned long artosyn_composite_recalc_rate(struct clk_hw *hw,
						     unsigned long prate)
{
	struct artosyn_clk_composite *pll = to_artosyn_clk_composite(hw);
	u64 rate64 = prate;
	u32 enable, source, div, sel, sel_pre;

	sel = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_SELECT);
	if(sel) {
		sel_pre = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_SELECT_PRE);
		if(sel_pre) {
			enable = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_CLK_B_EN);
			if(!enable) {
				return 0;
			}

			source = readl_relaxed(pll->base) & (0x7 << ARTOSYN_FREQ_CLK_B_SEL);
			source >>= ARTOSYN_FREQ_CLK_B_SEL;

			div = readl_relaxed(pll->base) & (0x7f << ARTOSYN_FREQ_CLK_B_DIV);
			div >>= ARTOSYN_FREQ_CLK_B_DIV;

			rate64 = clk_hw_get_rate(clk_hw_get_parent_by_index(hw, source));
			return rate64 / (div + 1);
		} else {
			enable = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_CLK_A_EN);
			if(!enable) {
				return 0;
			}

			source = readl_relaxed(pll->base) & (0x7 << ARTOSYN_FREQ_CLK_A_SEL);
			source >>= ARTOSYN_FREQ_CLK_A_SEL;

			div = readl_relaxed(pll->base) & (0x7f << ARTOSYN_FREQ_CLK_A_DIV);
			div >>= ARTOSYN_FREQ_CLK_A_DIV;

			rate64 = clk_hw_get_rate(clk_hw_get_parent_by_index(hw, source));
			return rate64 / (div + 1);
		}
	} else {
		return clk_hw_get_rate(clk_hw_get_parent_by_index(hw, 0));
	}
}

static long artosyn_get_closed_clk(struct clk_hw *hw,
			    unsigned long drate, unsigned int * psource, unsigned int * pdiv)
{
	int i = 0;
	u32 div;
	u32 delta = 0xffffffff;
	u64 rate64 = 0;

	for(i = 0; i < clk_hw_get_num_parents(hw); ++i)
	{
		rate64 = clk_hw_get_rate(clk_hw_get_parent_by_index(hw, i));
		if(drate > rate64)
			continue;
		div = rate64 / drate;
		if(delta > (rate64 / div - drate)) {
			delta = (rate64 / div - drate);
			if(psource)
				*psource = i;
			if(pdiv)
				*pdiv = div - 1;
		}
	}

	if(0xffffffff == delta)
		return -1;

	return drate + delta;
}

static long artosyn_composite_round_rate(struct clk_hw *hw,
			    unsigned long drate, unsigned long *prate)
{
	return artosyn_get_closed_clk(hw, drate, NULL, NULL);
}

static int artosyn_composite_set_rate(struct clk_hw *hw, unsigned long drate,
					unsigned long prate)
{
	struct artosyn_clk_composite *pll = to_artosyn_clk_composite(hw);
	u32 val = 0;
	u32 source, div, sel, sel_pre;

	if(artosyn_get_closed_clk(hw, drate, &source, &div) < 0)
		return -1;

	sel = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_SELECT);
	if(sel) {
		sel_pre = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_SELECT_PRE);
		if(sel_pre) {
			val = readl_relaxed(pll->base);
			val &= ~(0x1fff);
			val |= ((source << ARTOSYN_FREQ_CLK_A_SEL) | 
					(div << ARTOSYN_FREQ_CLK_A_DIV) |
					(1 << ARTOSYN_FREQ_CLK_A_EN));
			writel(val, pll->base);

			val &= ~(1 << ARTOSYN_FREQ_SELECT_PRE);
			writel(val, pll->base);
		} else {
			val = readl_relaxed(pll->base);
			val &= ~(0x1fff << ARTOSYN_FREQ_CLK_B_DIV);
			val |= ((source << ARTOSYN_FREQ_CLK_B_SEL) | 
					(div << ARTOSYN_FREQ_CLK_B_DIV) |
					(1 << ARTOSYN_FREQ_CLK_B_EN));
			writel(val, pll->base);

			val |= (1 << ARTOSYN_FREQ_SELECT_PRE);
			writel(val, pll->base);
		}
	} else {
		val = readl_relaxed(pll->base);
		val &= ~(0x1fff | (1 << ARTOSYN_FREQ_SELECT_PRE));
		val |= ((source << ARTOSYN_FREQ_CLK_A_SEL) | 
				(div << ARTOSYN_FREQ_CLK_A_DIV) |
				(1 << ARTOSYN_FREQ_CLK_A_EN));
		writel(val, pll->base);

		val |= (1 << ARTOSYN_FREQ_SELECT);
		writel(val, pll->base);
	}

	return 0;
}

static u8 artosyn_composite_get_parent(struct clk_hw *hw)
{
	struct artosyn_clk_composite *pll = to_artosyn_clk_composite(hw);
	u32 enable, source, sel, sel_pre;

	sel = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_SELECT);
	if(sel) {
		sel_pre = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_SELECT_PRE);
		if(sel_pre) {
			enable = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_CLK_B_EN);
			if(!enable) {
				return 0;
			}

			source = readl_relaxed(pll->base) & (0x7 << ARTOSYN_FREQ_CLK_B_SEL);
			source >>= ARTOSYN_FREQ_CLK_B_SEL;

			return source;
		} else {
			enable = readl_relaxed(pll->base) & (1 << ARTOSYN_FREQ_CLK_A_EN);
			if(!enable) {
				return 0;
			}

			source = readl_relaxed(pll->base) & (0x7 << ARTOSYN_FREQ_CLK_A_SEL);
			source >>= ARTOSYN_FREQ_CLK_A_SEL;

			return source;
		}
	} else {
		return 0;
	}
}

static const struct clk_ops artosyn_composite_clk_ops = {
	.recalc_rate = artosyn_composite_recalc_rate,
	.round_rate = artosyn_composite_round_rate,
	.set_rate = artosyn_composite_set_rate,
	.get_parent = artosyn_composite_get_parent,
};

struct clk * artosyn_clk_register_composite(const char *name,
                    const char * const *parent_names,  int num_parents,
                    void __iomem *base, u8 clk_pll_flags)
{
    struct artosyn_clk_composite 	*pll;
	struct clk 				*clk;
	struct clk_hw			*hw;
	struct clk_init_data	init;
	//int 					ret;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if(!pll)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.flags = clk_pll_flags;
	init.parent_names = parent_names;
	init.num_parents = num_parents;
	init.ops = &artosyn_composite_clk_ops;

	pll->base = base;
	pll->hw.init = &init;

	hw = &pll->hw;

	clk = clk_register(NULL, hw);
	if (IS_ERR(clk)) {
		pr_err("%s: failed to register composite clock %s : %ld\n",
			__func__, name, PTR_ERR(clk));
		goto err_pll;
	}

	return clk;
err_pll:
	kfree(pll);
	return clk;
}
EXPORT_SYMBOL_GPL(artosyn_clk_register_composite);
