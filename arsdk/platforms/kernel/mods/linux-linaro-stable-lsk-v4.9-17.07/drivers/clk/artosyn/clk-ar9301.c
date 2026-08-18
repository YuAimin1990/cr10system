#include <dt-bindings/clock/ar_clk.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "clk.h"

static struct clk *clks[INDEX_CLK(AR_CLK_END)] = {NULL};

static struct clk_onecell_data clk_data;

static int ar9301_clocks_probe(struct platform_device *pdev)
{
    int ret = 0;
	void __iomem *reg_base_dla;
	void __iomem *reg_base_ceva;
	void __iomem *reg_base_a53;
	struct device_node *np = pdev->dev.of_node;
	
	reg_base_dla = of_iomap(np, 1);
	if(!reg_base_dla) {
		pr_err("%s: could not map dla region\n", __func__);
		goto err1;
	}

	reg_base_ceva = of_iomap(np, 2);
	if(!reg_base_ceva) {
		pr_err("%s: could not map ceva region\n", __func__);
		goto err2;
	}

	reg_base_a53 = of_iomap(np, 3);
	if(!reg_base_a53) {
		pr_err("%s: could not map a53 region\n", __func__);
		goto err3;
	}

	clks[INDEX_CLK(AR_CLK_20M)] 	 = artosyn_clk_register_fixed("fix_20M", 20000000);
	clks[INDEX_CLK(AR_CLK_PLL_DLA)]  = artosyn_clk_register_pll("pll_dla",  "fix_20M", reg_base_dla,  CLK_GET_RATE_NOCACHE);
	clks[INDEX_CLK(AR_CLK_PLL_CEVA)] = artosyn_clk_register_pll("pll_ceva", "fix_20M", reg_base_ceva, CLK_GET_RATE_NOCACHE);
	clks[INDEX_CLK(AR_CLK_PLL_ARM)]  = artosyn_clk_register_pll("pll_a53",  "fix_20M", reg_base_a53,  CLK_GET_RATE_NOCACHE);


	clk_data.clks = clks;
	clk_data.clk_num = ARRAY_SIZE(clks);
	of_clk_add_provider(np, of_clk_src_onecell_get, &clk_data);

	printk(KERN_INFO "%s %d success!\n", __func__, __LINE__);
	
    return ret;

err3:
	iounmap(reg_base_ceva);
err2:
	iounmap(reg_base_dla);
err1:
	return -1;
}

static const struct of_device_id ar9301_clk_of_match[] = {
        { .compatible = "artosyn,ar9301-cgu" },
        { /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, ar9301_clk_of_match);

static struct platform_driver ar9301_clk_driver = {
        .probe = ar9301_clocks_probe,
        .driver = {
                .name = "ar9301-cgu",
                /*
                 * Disable bind attributes: clocks are not removed and
                 * reloading the driver will crash or break devices.
                 */
                .suppress_bind_attrs = true,
                .of_match_table = ar9301_clk_of_match,
        },
};
module_platform_driver(ar9301_clk_driver);

MODULE_AUTHOR("kwang");
MODULE_DESCRIPTION("Artosyn AR9301 clock unit");
MODULE_LICENSE("GPL v2");
