/*
 * TRNG - DesignWare TRNG driver
 *
 * Copyright 2022 Artosyn Microelectronics
 *
 */

#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/hw_random.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/random.h>

#define RNG_CTRL_OFFSET			0x00
#define RNG_STAT_OFFSET			0x04
#define RNG_MODE_OFFSET			0x08
#define RNG_SMODE_OFFSET		0x0C
#define RNG_IE_OFFSET			0x10
#define RNG_ISTAT_OFFSET		0x14
#define RNG_COREKIT_REL_OFFSET		0x18
#define RNG_FEATURES			0x1C
#define RNG_RAND0_OFFSET		0x20
#define RNG_RAND1_OFFSET		0x24
#define RNG_RAND2_OFFSET		0x28
#define RNG_RAND3_OFFSET		0x2C
#define RNG_RAND4_OFFSET		0x30
#define RNG_RAND5_OFFSET		0x34
#define RNG_RAND6_OFFSET		0x38
#define RNG_RAND7_OFFSET		0x3C
#define RNG_SEED0_OFFSET		0x40
#define RNG_SEED1_OFFSET		0x44
#define RNG_SEED2_OFFSET		0x48
#define RNG_SEED3_OFFSET		0x4C
#define RNG_SEED4_OFFSET		0x50
#define RNG_SEED5_OFFSET		0x54
#define RNG_SEED6_OFFSET		0x58
#define RNG_SEED7_OFFSET		0x5C
#define RNG_AUTO_RQSTS_OFFSET		0x60
#define RNG_AUTO_AGE_OFFSET		0x64
#define RNG_BUILD_CONFIG_OFFSET		0x68

#define RNG_CTRL_CMD_GEN_RAND		1
#define RNG_CTRL_CMD_RAND_RESEED	2
#define RNG_CTRL_CMD_NONCE_RESEED	3

#define RNG_MODE_R256			(3<<1)
#define RNG_NONCE_MODE			(2<<1)

#define RNG_EN_NONCE_RESEED_MODE	((0xa<<16) | (1<<2))
#define RNG_DIS_NONCE_RESEED_MODE	((0xa<<16))

#define RNG_ISTAT_SEED_DONE		(1<<1)
#define RNG_ISTAT_RAND_RDY		(1<<0)

#define to_dw_rng(p)	container_of(p, struct dw_rng, rng)

struct dw_rng {
	void __iomem *base;
	struct hwrng rng;
	u32 rand_length;
	u32 rand_offset;
};

static int dw_rng_reseed(struct dw_rng *hrng)
{
	u32 val = 0;
#if 1
	u32 seed[8];

	/* 1. Write a 1 to SMODE.NONCE_MODE in the SMODE register.*/
	writel(RNG_EN_NONCE_RESEED_MODE, hrng->base + RNG_SMODE_OFFSET);

	/* 2. Wait for the NONCE_MODE bit in the STAT register to become 1.*/
	do {
		val = readl_relaxed(hrng->base + RNG_STAT_OFFSET);
	} while ((val & RNG_NONCE_MODE) == 0);

	/* 3. Load the 255-bit nonce into SEED0 through SEED7.*/
	get_random_bytes(&seed[0], sizeof(seed));
	writel(seed[0], hrng->base + RNG_SEED0_OFFSET);
	writel(seed[1], hrng->base + RNG_SEED1_OFFSET);
	writel(seed[2], hrng->base + RNG_SEED2_OFFSET);
	writel(seed[3], hrng->base + RNG_SEED3_OFFSET);
	writel(seed[4], hrng->base + RNG_SEED4_OFFSET);
	writel(seed[5], hrng->base + RNG_SEED5_OFFSET);
	writel(seed[6], hrng->base + RNG_SEED6_OFFSET);
	writel(seed[7], hrng->base + RNG_SEED7_OFFSET);

	/*  4. Write a 3 to CMD in the CTRL register to execute the load nonce sequence.*/
	writel(RNG_CTRL_CMD_NONCE_RESEED, hrng->base + RNG_CTRL_OFFSET);

	do {
		val = readl_relaxed(hrng->base + RNG_ISTAT_OFFSET);
	} while ((val & RNG_ISTAT_RAND_RDY) == 0);

	writel(RNG_ISTAT_RAND_RDY, hrng->base + RNG_ISTAT_OFFSET);
#else
	writel(RNG_CTRL_CMD_RAND_RESEED, hrng->base + RNG_CTRL_OFFSET);

	do {
		val = readl_relaxed(hrng->base + RNG_ISTAT_OFFSET);
	} while ((val & RNG_ISTAT_SEED_DONE) == 0);

	writel(RNG_ISTAT_SEED_DONE, hrng->base + RNG_ISTAT_OFFSET);
#endif
	return 0;
}

static int dw_rng_gen_rand(struct dw_rng *hrng)
{
	u32 val = 0;

	writel(RNG_CTRL_CMD_GEN_RAND, hrng->base + RNG_CTRL_OFFSET);

	do {
		cpu_relax();
		val = readl_relaxed(hrng->base + RNG_ISTAT_OFFSET);
	} while ((val & RNG_ISTAT_RAND_RDY) == 0);

	writel(RNG_ISTAT_RAND_RDY, hrng->base + RNG_ISTAT_OFFSET);
	hrng->rand_offset = 0;

	return 0;
}

static int dw_rng_init(struct hwrng *rng)
{
	struct dw_rng *hrng = to_dw_rng(rng);
	u32 val = 0;

	val = readl_relaxed(hrng->base + RNG_MODE_OFFSET);
	if (val & RNG_MODE_R256)
		hrng->rand_length = 256/8;
	else
		hrng->rand_length = 128/8;

	dw_rng_reseed(hrng);

	return 0;
}

static int dw_rng_read(struct hwrng *rng, void *buf, size_t max, bool wait)
{
	struct dw_rng *hrng = to_dw_rng(rng);
	u32 *data = buf;
	int retval = 0;

	while (max >= sizeof(u32)) {
		if ((hrng->rand_offset == hrng->rand_length)) {
			dw_rng_gen_rand(hrng);
		}

		*(u32 *)data = readl_relaxed(hrng->base + RNG_RAND0_OFFSET + hrng->rand_offset);
		retval += sizeof(u32);
		max -= sizeof(u32);
		hrng->rand_offset += sizeof(u32);
		data++;
	}
	return retval;
}

static int dw_rng_probe(struct platform_device *pdev)
{
	struct dw_rng *rng;
	struct resource *res;
	int ret;

	rng = devm_kzalloc(&pdev->dev, sizeof(*rng), GFP_KERNEL);
	if (!rng)
		return -ENOMEM;

	platform_set_drvdata(pdev, rng);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	rng->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(rng->base))
		return PTR_ERR(rng->base);

	rng->rng.name = pdev->name;
	rng->rng.init = dw_rng_init;
	rng->rng.read = dw_rng_read;

	ret = devm_hwrng_register(&pdev->dev, &rng->rng);
	if (ret) {
		dev_err(&pdev->dev, "failed to register hwrng\n");
		return ret;
	}

	return 0;
}

#ifdef CONFIG_PM
static int dw_rng_suspend(struct device *dev)
{
	return 0;
}

static int dw_rng_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct dw_rng *hrng = platform_get_drvdata(pdev);
	dw_rng_reseed(hrng);
	return 0;
}

static const struct dev_pm_ops dw_rng_pm_ops = {
	.suspend	= dw_rng_suspend,
	.resume		= dw_rng_resume,
};
#endif /* CONFIG_PM */


static const struct of_device_id dw_rng_dt_ids[] = {
	{ .compatible = "snps,dw-rng" },
	{ }
};
MODULE_DEVICE_TABLE(of, dw_rng_dt_ids);

static struct platform_driver dw_rng_driver = {
	.probe		= dw_rng_probe,
	.driver		= {
		.name	= "dw-rng",
#ifdef CONFIG_PM
		.pm	= &dw_rng_pm_ops,
#endif /* CONFIG_PM */
		.of_match_table = of_match_ptr(dw_rng_dt_ids),
	},
};

module_platform_driver(dw_rng_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xin.chen@artosyn.cn");
MODULE_DESCRIPTION("DesignWare random number generator driver");
