/*
 * Artosyn DVFS driver
 */
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <asm/io.h>
#include "ar_dvfs.h"

struct artosyn_dvfs {
	struct ar_dvfs_processor processors[DVFS_PROCESSORS];
	struct timer_list timer;
	unsigned long timer_msecs;
	void *temperature_enable;
	void *temperature_value;
};

static struct artosyn_dvfs ar_dvfs;


/*
 * temerature = 0.4825 * value - 77.7 celsius
 */
void artosyn_dvfs_timer_function(unsigned long arg)
{
	unsigned int value = 0;
	unsigned int temerature;
	value = readl(ar_dvfs.temperature_value);

	temerature = (4825 * value - 777000) / 10000;

	printk(KERN_EMERG"temperature %d\n", temerature);

	/* Update timer */
	mod_timer(&ar_dvfs.timer, jiffies + msecs_to_jiffies(ar_dvfs.timer_msecs));
}

static int artosyn_dvfs_probe(struct platform_device *pdev)
{
	int i, ret, count;
	char *name = NULL;
	u32 value;
	struct device_node *dt_node = pdev->dev.of_node;
	struct device_node *node;

	memset(&ar_dvfs, 0, sizeof(ar_dvfs));

	/* Get trigger address */
	ret = of_property_read_u32(dt_node, "temperature-enable", &value);
	if(ret < 0) {
		return ret;
	}

	ar_dvfs.temperature_enable = ioremap(value, 4);
	if(!ar_dvfs.temperature_enable)
		return -EINVAL;

	printk(KERN_EMERG"temperature-enable %x %x\n", value, ar_dvfs.temperature_enable);

	ret = of_property_read_u32(dt_node, "temperature-value", &value);
	if (ret < 0) {
		return ret;
	}

	ar_dvfs.temperature_value = ioremap(value, 4);
	if(!ar_dvfs.temperature_value)
		return -EINVAL;

	printk(KERN_EMERG"temperature-value %x %x\n", value, ar_dvfs.temperature_value);

	count = of_get_available_child_count(dt_node);
	if (!count || count > DVFS_PROCESSORS) {
		return -EINVAL;
	}

	for_each_available_child_of_node(dt_node, node) {
		ret = of_property_read_string(node, "device-name", (const char **)&name);
		if(ret)
			return ret;
		if(strcmp(name, "ceva") == 0) {
			ret = ar_dvfs_init_ceva(node, &ar_dvfs.processors[AR_DVFS_DEV_CEVA]);
			if(ret)
				return ret;
		}
		else if(strcmp(name, "arm_a_linux") == 0) {
		}
		else if(strcmp(name, "arm_a_rtos") == 0) {
		}
		else if(strcmp(name, "arm_m_rtos") == 0) {
		}
		else {
		}
	}

	writel(0x1, ar_dvfs.temperature_enable);

	ret = of_property_read_u32(dt_node, "period", &value);
	if (ret < 0) {
		return ret;
	}

	ar_dvfs.timer_msecs = value;
	/* Add timer */
	ar_dvfs.timer.function = artosyn_dvfs_timer_function;
	ar_dvfs.timer.data = 0;
	ar_dvfs.timer.expires = jiffies + msecs_to_jiffies(ar_dvfs.timer_msecs);
	add_timer(&ar_dvfs.timer);
}

static int artosyn_dvfs_remove(struct platform_device *pdev)
{
	iounmap(ar_dvfs.temperature_enable);
	iounmap(ar_dvfs.temperature_value);
	del_timer(&ar_dvfs.timer);
	return 0;
}

static const struct of_device_id artosyn_dvfs_match_table[] = {
	{.compatible = "artosyn,dvfs"},
	{},
};

static struct platform_driver artosyn_dvfs_driver = {
	.probe = artosyn_dvfs_probe,
	.remove = artosyn_dvfs_remove,
	.driver = {
		.name = "dvfs-artosyn",
		.of_match_table = artosyn_dvfs_match_table,
	},
};

static int __init artosyn_dvfs_init(void)
{
	return platform_driver_register(&artosyn_dvfs_driver);
}

static void __exit artosyn_dvfs_exit(void)
{
	platform_driver_unregister(&artosyn_dvfs_driver);
}

MODULE_AUTHOR("artosyn");
MODULE_DESCRIPTION("artosyn dvfs driver");
MODULE_LICENSE("GPL");
subsys_initcall(artosyn_dvfs_init);
module_exit(artosyn_dvfs_exit);
