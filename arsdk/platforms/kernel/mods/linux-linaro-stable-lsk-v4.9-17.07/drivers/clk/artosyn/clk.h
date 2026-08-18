#ifndef __MACH_ARTO_CLK_H
#define __MACH_ARTO_CLK_H

#include <linux/clk-provider.h>

static inline struct clk *artosyn_clk_register_fixed(const char *name, int rate)
{
	return clk_register_fixed_rate(NULL, name, NULL, 0, rate);
}

struct clk * artosyn_clk_register_pll(const char *name,
                    const char *parent_name, void __iomem *base,
                    u8 clk_pll_flags);

struct clk * artosyn_clk_register_composite(const char *name,
                    const char * const *parent_names,  int num_parents,
                    void __iomem *base, u8 clk_pll_flags);

#endif
