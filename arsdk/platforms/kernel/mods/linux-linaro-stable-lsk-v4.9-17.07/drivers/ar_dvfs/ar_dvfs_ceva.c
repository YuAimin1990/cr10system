#include <linux/of.h>
#include <asm/io.h>
#include "ar_dvfs.h"

struct ar_dvfs_ceva_info {
	struct ar_dvfs_processor *processor;
	void *reg_freq;

	/*M Hz*/
	unsigned long max_clk;
	unsigned long min_clk;
};

static struct ar_dvfs_ceva_info ceva_info;

static int ar_dvfs_ceva_setfreq(struct ar_dvfs_processor *processor, unsigned int freq)
{
	struct ar_dvfs_ceva_info *info =
		(struct ar_dvfs_ceva_info *)processor->priv;
	int value;

	value = readl(info->reg_freq);
	value |= (freq & 0xff) << 8;
	writel(value, info->reg_freq);
}

int ar_dvfs_init_ceva(struct device_node *node, struct ar_dvfs_processor *processor)
{
	int ret;
	u32 value;

	memset(&ceva_info, 0, sizeof(ceva_info));

	ceva_info.processor = processor;

	ret = of_property_read_u32(node, "reg-freq", &value);
	if (ret < 0) {
		return ret;
	}

	ceva_info.reg_freq = ioremap(value, 4);
	if(!ceva_info.reg_freq) {
		return -ENOMEM;
	}

	ret = of_property_read_u32(node, "max-clk", &value);
	if (ret < 0) {
		return ret;
	}
	ceva_info.max_clk = value;

	printk(KERN_EMERG"max clk %d\n", ceva_info.max_clk);

	ret = of_property_read_u32(node, "min-clk", &value);
	if (ret < 0) {
		return ret;
	}
	ceva_info.min_clk = value;

	printk(KERN_EMERG"min clk %d\n", ceva_info.min_clk);

	processor->priv = (void *)&ceva_info;
	processor->set_freq = ar_dvfs_ceva_setfreq;
	return 0;
}
