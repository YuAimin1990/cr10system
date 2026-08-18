#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <asm/memory.h>
#include <asm/io.h>
#include <linux/kdebug.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/profile.h>

#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <dt-bindings/pwm/pwm.h>

int old_api = 0;
module_param(old_api, int, 0644);

struct pwm_device *pwm[10] = {NULL};

static int led_pwm_probe(struct platform_device *pdev)
{
	int ret = 0;
	int i = 0;
	char pwm_name[16] = {""};
	struct device *dev = &pdev->dev;
	printk("led_pwm_probe \n");

	for(i = 0; i < 10; ++i)
	{
		sprintf(pwm_name, "pwm%d", i);
		printk(KERN_ERR "pwm %d %s!\n", i, pwm_name);
		if(!old_api)
		{
			pwm[i] = devm_pwm_get(dev, pwm_name);
			printk(KERN_ERR "pwm %d %s %p!\n", i, pwm_name, pwm[i]);
			if (IS_ERR(pwm[i]))
				printk(KERN_ERR "pwm %d get failed!", i);
			else 
			{
				pwm_config(pwm[i], 500000000, 1000000000);
				ret = pwm_enable(pwm[i]);
			}
		}
		else
		{
			pwm[i] = pwm_request(i, pwm_name);
			printk(KERN_ERR "pwm: %d %s %p!\n", i, pwm_name, pwm[i]);
			if (IS_ERR(pwm[i]))
				printk(KERN_ERR "pwm %d get failed!", i);
			else 
			{
				pwm_config(pwm[i], 500000000, 1000000000);
				ret = pwm_enable(pwm[i]);
			}	
		}
	}

    return ret;
}

static int led_pwm_remove(struct platform_device *pdev)
{
	int i = 0;
	struct device *dev = &pdev->dev;
	printk("led_pwm_remove \n");

	for(i = 0; i < 10; ++i)
	{
		if(!old_api)
		{
			if(pwm[i] && !IS_ERR(pwm[i]))
			{
				pwm_disable(pwm[i]);
				devm_pwm_put(dev, pwm[i]);
			}
		}
		else
		{
			if(pwm[i] && !IS_ERR(pwm[i]))
			{
				pwm_disable(pwm[i]);
				pwm_free(pwm[i]);
			}
		}
	}
    return 0;
}


static const struct of_device_id of_pwm_leds_match[] = {
	{.compatible = "artosyn,test-pwm"},
	{},
};

static struct platform_driver led_pwm_driver = {
	.probe          = led_pwm_probe,
	.remove         = led_pwm_remove,
	.driver         = {
		.name   = "test_pwm",
		.of_match_table = of_pwm_leds_match,
	},
};

static int __init pwm_leds_platform_driver_init(void)
{
	int DriverState;
	DriverState = platform_driver_register(&led_pwm_driver);
	return 0;
}

static void __exit pwm_leds_platform_driver_exit(void)
{
	printk(KERN_ERR " pwm_leds_exit\n");
	platform_driver_unregister(&led_pwm_driver);
}

module_init(pwm_leds_platform_driver_init);
module_exit(pwm_leds_platform_driver_exit);

MODULE_AUTHOR("kwang");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
