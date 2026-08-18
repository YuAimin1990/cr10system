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


#define AR9311_FIX_OSC				"fix_osc"
#define AR9311_FIX_1000M			"fix_1000m"
#define AR9311_FIX_800M				"fix_800m"
#define AR9311_FIX_666M				"fix_666m"
#define AR9311_FIX_600M				"fix_600m"
#define AR9311_FIX_400M				"fix_400m"


static struct clk *clks[INDEX_CLK(AR_CLK_END)] = {NULL};
static struct clk_onecell_data clk_data;

static const char *ar9311_cpu_sels[] = {AR9311_FIX_OSC, AR9311_FIX_1000M, AR9311_FIX_800M, AR9311_FIX_666M,
									     AR9311_FIX_600M, AR9311_FIX_400M};

static int ar9311_clocks_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device_node *np = pdev->dev.of_node;
	void __iomem *reg_base_a53;

	reg_base_a53 = of_iomap(np, 1);
	if(!reg_base_a53) {
		pr_err("%s: could not map a53 region\n", __func__);
		goto err1;
	}

	clks[INDEX_CLK(AR_CLK_OSC)] 	 = artosyn_clk_register_fixed(AR9311_FIX_OSC, 24000000);
	clks[INDEX_CLK(AR_CLK_1000M)] 	 = artosyn_clk_register_fixed(AR9311_FIX_1000M, 1000000000);
	clks[INDEX_CLK(AR_CLK_800M)] 	 = artosyn_clk_register_fixed(AR9311_FIX_800M, 800000000);
	clks[INDEX_CLK(AR_CLK_666M)] 	 = artosyn_clk_register_fixed(AR9311_FIX_666M, 666666666);
	clks[INDEX_CLK(AR_CLK_600M)] 	 = artosyn_clk_register_fixed(AR9311_FIX_600M, 600000000);
	clks[INDEX_CLK(AR_CLK_400M)] 	 = artosyn_clk_register_fixed(AR9311_FIX_400M, 400000000);

	clks[INDEX_CLK(AR_CLK_PLL_ARM)]  = artosyn_clk_register_composite("comp_a53",  ar9311_cpu_sels, ARRAY_SIZE(ar9311_cpu_sels),
										reg_base_a53,  CLK_GET_RATE_NOCACHE);

	clk_data.clks = clks;
	clk_data.clk_num = ARRAY_SIZE(clks);
	of_clk_add_provider(np, of_clk_src_onecell_get, &clk_data);

	return ret;

err1:
	return -1;
}

static const struct of_device_id ar9311_clk_of_match[] = {
        { .compatible = "artosyn,ar9311-cgu" },
        { /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, ar9311_clk_of_match);

static struct platform_driver ar9311_clk_driver = {
        .probe = ar9311_clocks_probe,
        .driver = {
                .name = "ar9311-cgu",
                /*
                 * Disable bind attributes: clocks are not removed and
                 * reloading the driver will crash or break devices.
                 */
                .suppress_bind_attrs = true,
                .of_match_table = ar9311_clk_of_match,
        },
};
module_platform_driver(ar9311_clk_driver);

MODULE_AUTHOR("kwang");
MODULE_DESCRIPTION("Artosyn AR9311 clock unit");
MODULE_LICENSE("GPL v2");
