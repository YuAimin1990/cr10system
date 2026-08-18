/*
 * PWM driver for Artosyn SoCs
 */

#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/time.h>

#define ARTOSYN_PWM_NUM		(0x08)

#define REG_PWM_LOAD_COUNT	(0x00)
#define REG_PWM_LOAD_COUNT2     (0xb0)
#define REG_PWM_CTRL		(0x08)
#define ARTO_PWM_SIZE		(0x14)
#define ARTO_PWM_SIZE2          (0x04)

#define TIME_ENABLE		(0x01)
#define TIME_PWM_ENABLE		(0x01 << 3)
#define USER_DEFINED		(0x01 << 1)
#define PWM_ENABLE		(TIME_ENABLE | TIME_PWM_ENABLE | USER_DEFINED)

struct artosyn_pwm_regs{
	unsigned long on_count;
	unsigned long off_count;
	unsigned long ctrl;
};

struct artosyn_pwm_data {
	unsigned long 		default_clk_rate;
	bool 			supports_polarity;
	struct artosyn_pwm_regs regs;
	unsigned int		npwm;
};

struct artosyn_pwm_ctx {
	u32 cnt;
	u32 cnt2;
	u32 ctrl;
};

struct artosyn_pwm_chip {
	struct pwm_chip 	chip;
	struct clk 		*clk;
	void __iomem 		*base;
	const struct artosyn_pwm_data *data;
	struct artosyn_pwm_ctx	*ctx;
};

static inline struct artosyn_pwm_chip *to_artosyn_pwm_chip(struct pwm_chip *c)
{
	return container_of(c, struct artosyn_pwm_chip, chip);
}

static inline unsigned long artosyn_pwm_read_count(const struct pwm_device *pwm)
{
	struct artosyn_pwm_chip *arto_chip = to_artosyn_pwm_chip(pwm->chip);

	return readl(arto_chip->base + pwm->hwpwm * ARTO_PWM_SIZE + REG_PWM_LOAD_COUNT);
}

static inline unsigned long artosyn_pwm_read_count2(const struct pwm_device *pwm)
{
	struct artosyn_pwm_chip *arto_chip = to_artosyn_pwm_chip(pwm->chip);

	return readl(arto_chip->base + pwm->hwpwm * ARTO_PWM_SIZE2 + REG_PWM_LOAD_COUNT2);
}

static inline unsigned long artosyn_pwm_read_ctrl(const struct pwm_device *pwm)
{
	struct artosyn_pwm_chip *arto_chip = to_artosyn_pwm_chip(pwm->chip);

        return readl(arto_chip->base + pwm->hwpwm * ARTO_PWM_SIZE + REG_PWM_CTRL);
}

static inline void artosyn_pwm_wrtie_count(const struct pwm_device *pwm, unsigned int value)
{
        struct artosyn_pwm_chip *arto_chip = to_artosyn_pwm_chip(pwm->chip);

        return writel(value, arto_chip->base + pwm->hwpwm * ARTO_PWM_SIZE + REG_PWM_LOAD_COUNT);
}

static inline void artosyn_pwm_wrtie_count2(const struct pwm_device *pwm, unsigned int value)
{
        struct artosyn_pwm_chip *arto_chip = to_artosyn_pwm_chip(pwm->chip);

        return writel(value, arto_chip->base + pwm->hwpwm * ARTO_PWM_SIZE2 + REG_PWM_LOAD_COUNT2);
}

static inline void artosyn_pwm_wrtie_ctrl(const struct pwm_device *pwm, unsigned int value)
{
        struct artosyn_pwm_chip *arto_chip = to_artosyn_pwm_chip(pwm->chip);

        return writel(value, arto_chip->base + pwm->hwpwm * ARTO_PWM_SIZE + REG_PWM_CTRL);
}

static void artosyn_pwm_get_state(struct pwm_chip *chip,
				   struct pwm_device *pwm,
				   struct pwm_state *state)
{
	unsigned long clk_rate;
	struct artosyn_pwm_chip *arto_chip;
	u64 tmp;
	u32 ctrl;
	int ret;

	arto_chip = to_artosyn_pwm_chip(chip);
	if(NULL == arto_chip)
		return;

	ret = clk_enable(arto_chip->clk);
	if (ret)
		return;

	clk_rate = clk_get_rate(arto_chip->clk);
	tmp = artosyn_pwm_read_count(pwm) + artosyn_pwm_read_count2(pwm);
	tmp *= NSEC_PER_SEC;

	state->period = DIV_ROUND_CLOSEST_ULL(tmp, clk_rate);

	tmp = artosyn_pwm_read_count2(pwm);
	tmp *= NSEC_PER_SEC;
	state->duty_cycle = DIV_ROUND_CLOSEST_ULL(tmp, clk_rate);

	ctrl = artosyn_pwm_read_ctrl(pwm);
	ctrl &= PWM_ENABLE;

	if(ctrl)
	{
		state->enabled = 1;
	}
	else
	{
		state->enabled = 0;
	}
#if 1
	if(!arto_chip->data->supports_polarity)
	{
		state->polarity = PWM_POLARITY_NORMAL;
	}
#endif
	clk_disable(arto_chip->clk);
}

static int artosyn_pwm_config(struct pwm_chip *chip,
                              struct pwm_device *pwm,
                              struct pwm_state *state)
{
	struct artosyn_pwm_chip *arto_chip;
	unsigned long count1, count2;
	u64 div, clk_rate;

	arto_chip = to_artosyn_pwm_chip(chip);

	clk_rate = clk_get_rate(arto_chip->clk);

	div = clk_rate * (state->period - state->duty_cycle);
	count1 = DIV_ROUND_CLOSEST_ULL(div, NSEC_PER_SEC);

	div = clk_rate * state->duty_cycle;
	count2 = DIV_ROUND_CLOSEST_ULL(div, NSEC_PER_SEC);

	artosyn_pwm_wrtie_count(pwm, count1);
	artosyn_pwm_wrtie_count2(pwm, count2);

	return 0;
}

static int artosyn_pwm_apply(struct pwm_chip *chip,
			      struct pwm_device *pwm,
			      struct pwm_state *state)
{
	//unsigned long clk_rate;
        struct artosyn_pwm_chip *arto_chip;
	struct pwm_state curstate;
	bool enabled;
	int ret;

	arto_chip = to_artosyn_pwm_chip(chip);
        if(NULL == arto_chip)
                return PTR_ERR(arto_chip);

	ret = clk_enable(arto_chip->clk);
        if (ret)
                return ret;

	pwm_get_state(pwm, &curstate);
	enabled = curstate.enabled;

	artosyn_pwm_config(chip, pwm, state);
	if(state->enabled != enabled)
	{
		unsigned long ctrl;
		ctrl = artosyn_pwm_read_ctrl(pwm);

		if(state->enabled)
		{
			ctrl |= PWM_ENABLE;
		}
		else
		{
			ctrl &= (~PWM_ENABLE);
		}

		artosyn_pwm_wrtie_ctrl(pwm, ctrl);
	}

	artosyn_pwm_get_state(chip, pwm, state);

	clk_disable(arto_chip->clk);

	return 0;
}

static const struct pwm_ops artosyn_pwm_ops = {
	.get_state 	= artosyn_pwm_get_state,
	.apply 		= artosyn_pwm_apply,
	.owner		= THIS_MODULE,
};

static const struct artosyn_pwm_data ar9201_pwm_data = {
	.default_clk_rate 	= 300000000, //300M
	.supports_polarity 	= 0,
	.npwm			= ARTOSYN_PWM_NUM,
};

static const struct artosyn_pwm_data ar9301_pwm_data = {
	.default_clk_rate 	= 150000000, //150M
	.supports_polarity 	= 0,
	.npwm			= ARTOSYN_PWM_NUM,
};

static const struct of_device_id artosyn_pwm_dt_ids[] = {
	{ .compatible = "artosyn,ar9201-pwm", .data = &ar9201_pwm_data},
	{ .compatible = "artosyn,ar9301-pwm", .data = &ar9301_pwm_data},
	{},
};

static int artosyn_pwm_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;
	struct artosyn_pwm_chip *arto_chip;
	struct resource *r;
	int ret;

	//printk(KERN_ERR "pwm probe start!\n");

	id = of_match_device(artosyn_pwm_dt_ids, &pdev->dev);
	if (!id)
	{
		printk(KERN_ERR "pwm NO match device!\n");
		return -EINVAL;
	}

	arto_chip = devm_kzalloc(&pdev->dev, sizeof(*arto_chip), GFP_KERNEL);
	if (arto_chip == NULL)
		return -ENOMEM;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	arto_chip->base = devm_ioremap_resource(&pdev->dev, r);
	if (IS_ERR(arto_chip->base))
		return PTR_ERR(arto_chip->base);

	arto_chip->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(arto_chip->clk))
		return PTR_ERR(arto_chip->clk);

	ret = clk_prepare_enable(arto_chip->clk);
	if(ret)
		return ret;

	platform_set_drvdata(pdev, arto_chip);

	arto_chip->data = id->data;
	arto_chip->chip.dev = &pdev->dev;
	arto_chip->chip.ops = &artosyn_pwm_ops;
	arto_chip->chip.base = -1;
	arto_chip->chip.npwm = ((const struct artosyn_pwm_data *)id->data)->npwm;

#ifdef CONFIG_PM_SLEEP
	arto_chip->ctx = devm_kzalloc(&pdev->dev, 
		sizeof(struct artosyn_pwm_ctx) * arto_chip->chip.npwm, GFP_KERNEL);
	if(NULL == arto_chip->ctx)
		return -ENOMEM;
#endif

	ret = pwmchip_add(&arto_chip->chip);
	if (ret < 0) {
		clk_unprepare(arto_chip->clk);
		dev_err(&pdev->dev, "pwmchip_add() failed: %d\n", ret);
	}

	printk(KERN_INFO "pwm probe success!\n");

	return ret;
}


static int artosyn_pwm_remove(struct platform_device *pdev)
{
	struct artosyn_pwm_chip *arto_chip;

	arto_chip = platform_get_drvdata(pdev);
	if(NULL == arto_chip)
		return -EINVAL;

	return pwmchip_remove(&arto_chip->chip);
}

#ifdef CONFIG_PM_SLEEP
static int artosyn_pwm_suspend(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct artosyn_pwm_chip *arto_chip = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < arto_chip->chip.npwm; i++) {
		if (arto_chip->chip.pwms[i].state.enabled) {
			dev_err(dev, "PWM %u in use by consumer (%s)\n",
				i, arto_chip->chip.pwms[i].label);
			return -EBUSY;
		}
		arto_chip->ctx[i].cnt = readl(arto_chip->base + i * ARTO_PWM_SIZE + REG_PWM_LOAD_COUNT);
		arto_chip->ctx[i].cnt2 = readl(arto_chip->base + i * ARTO_PWM_SIZE2 + REG_PWM_LOAD_COUNT2);
		arto_chip->ctx[i].ctrl = readl(arto_chip->base + i * ARTO_PWM_SIZE + REG_PWM_CTRL);
	}

	return 0;
}

static int artosyn_pwm_resume(struct device *dev)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct artosyn_pwm_chip *arto_chip = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < arto_chip->chip.npwm; i++) {
		writel(arto_chip->ctx[i].cnt, arto_chip->base + i * ARTO_PWM_SIZE + REG_PWM_LOAD_COUNT);
		writel(arto_chip->ctx[i].cnt2, arto_chip->base + i * ARTO_PWM_SIZE2 + REG_PWM_LOAD_COUNT2);
		writel(arto_chip->ctx[i].ctrl, arto_chip->base + i * ARTO_PWM_SIZE + REG_PWM_CTRL);
	}

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(artosyn_pwm_pm_ops, artosyn_pwm_suspend, artosyn_pwm_resume);


static struct platform_driver artosyn_pwm_driver = {
	.driver = {
		.name = "artosyn-pwm",
		.pm = &artosyn_pwm_pm_ops,
		.of_match_table = artosyn_pwm_dt_ids,
	},
	.probe = artosyn_pwm_probe,
	.remove = artosyn_pwm_remove,
};
module_platform_driver(artosyn_pwm_driver);

MODULE_AUTHOR("kaiwang <kai.wang@artosyn.cn>");
MODULE_DESCRIPTION("artosyn SoC PWM driver");
MODULE_LICENSE("GPL v2");

