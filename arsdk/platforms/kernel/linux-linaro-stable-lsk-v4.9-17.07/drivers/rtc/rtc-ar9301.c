/*
 * Artosyn SoC AR9301 Real Time Clock Driver
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/rtc.h>

/* RTC CSR Registers */
#define RTC_CCVR		0x00
#define RTC_CMR			0x04
#define RTC_CLR			0x08
#define RTC_CCR			0x0C
#define  RTC_CCR_IE		BIT(0)
#define  RTC_CCR_MASK		BIT(1)
#define  RTC_CCR_EN		BIT(2)
#define  RTC_CCR_WEN		BIT(3)
#define RTC_STAT		0x10
#define  RTC_STAT_BIT		BIT(0)
#define RTC_RSTAT		0x14
#define RTC_EOI			0x18
#define RTC_VER			0x1C

/* RTC RF Registers */
#define RTC_RF_CON0		0x40
#define RTC_RF_CON1		0x44
#define RTC_RF_RST_CON	0x48
#define RTC_RF_RST_CNT	0x4C
#define RTC_RF_STAT0	0x50
#define RTC_RF_PWR_CON	0x54

#define RTC_RF_CON_PWR	BIT(0)
#define RTC_RF_CON_EN	BIT(16)

struct arto_rtc_dev {
	struct rtc_device *rtc;
	struct device *dev;
	//unsigned long alarm_time;
	void __iomem *csr_base;
	struct clk *clk;
	unsigned int irq_wake;
	unsigned int irq_enabled;
	unsigned int rf_enable;
};

static int arto_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);

	rtc_time64_to_tm(readl(pdata->csr_base + RTC_CCVR), tm);
	return 0;
}

static int arto_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);

	/*
	 * NOTE: After the following write, the RTC_CCVR is only reflected
	 *       after the update cycle of 1 seconds.
	 */
	writel((u32)rtc_tm_to_time64(tm), pdata->csr_base + RTC_CLR);
	readl(pdata->csr_base + RTC_CLR); /* Force a barrier */

	return 0;
}

static int arto_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);

	/* If possible, CMR should be read here */
	rtc_time64_to_tm(readl(pdata->csr_base + RTC_CMR), &alrm->time);
	alrm->enabled = readl(pdata->csr_base + RTC_CCR) & RTC_CCR_IE;

	return 0;
}

static int arto_rtc_alarm_irq_enable(struct device *dev, u32 enabled)
{
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);
	u32 ccr;

	ccr = readl(pdata->csr_base + RTC_CCR);
	if (enabled) {
		ccr &= ~RTC_CCR_MASK;
		ccr |= RTC_CCR_IE;
	} else {
		ccr &= ~RTC_CCR_IE;
		ccr |= RTC_CCR_MASK;
	}
	writel(ccr, pdata->csr_base + RTC_CCR);

	return 0;
}

static int arto_rtc_alarm_irq_enabled(struct device *dev)
{
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);

	return readl(pdata->csr_base + RTC_CCR) & RTC_CCR_IE ? 1 : 0;
}

static int arto_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);

	writel((u32)rtc_tm_to_time64(&alrm->time), pdata->csr_base + RTC_CMR);

	arto_rtc_alarm_irq_enable(dev, alrm->enabled);

	return 0;
}

static int arto_rtc_ioctl(struct device *dev,
				unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	unsigned int val = 0;
	struct arto_rtc_dev *pdata = dev_get_drvdata(dev);

	switch (cmd) {
	case RTC_RF_CON0_SET:
		writel(RTC_RF_CON_PWR|RTC_RF_CON_EN, pdata->csr_base + RTC_RF_CON0);
		break;

	case RTC_RF_CON1_SET:
		writel(RTC_RF_CON_PWR|RTC_RF_CON_EN, pdata->csr_base + RTC_RF_CON1);
		break;

	case RTC_RF_RST_CON_SET:
		copy_from_user(&val, (const void *)arg, sizeof(unsigned int));
		writel(val, pdata->csr_base + RTC_RF_RST_CON);
		break;

	case RTC_RF_RST_CNT_SET:
		copy_from_user(&val, (const void *)arg, sizeof(unsigned int));
		writel(val, pdata->csr_base + RTC_RF_RST_CNT);
		break;

	case RTC_RF_STAT0_GET:
		val = readl(pdata->csr_base + RTC_RF_STAT0);
		copy_to_user((const void *)arg, &val, sizeof(unsigned int));
		break;

	case RTC_RF_PWR_CON_GET:
		val = readl(pdata->csr_base + RTC_RF_PWR_CON);
		copy_to_user((const void *)arg, &val, sizeof(unsigned int));
		break;

	case RTC_RF_RST_CON_GET:
		val = readl(pdata->csr_base + RTC_RF_RST_CON);
		copy_to_user((const void *)arg, &val, sizeof(unsigned int));
		break;

	case RTC_RF_RST_CNT_GET:
		val = readl(pdata->csr_base + RTC_RF_RST_CNT);
		copy_to_user((const void *)arg, &val, sizeof(unsigned int));
		break;

	default:
		ret = -ENOIOCTLCMD;
	}

	return ret;
}

static const struct rtc_class_ops arto_rtc_ops_with_rf = {
	.read_time	= arto_rtc_read_time,
	.set_time	= arto_rtc_set_time,
	.read_alarm	= arto_rtc_read_alarm,
	.set_alarm	= arto_rtc_set_alarm,
	.alarm_irq_enable = arto_rtc_alarm_irq_enable,
	.ioctl		= arto_rtc_ioctl,
};

static const struct rtc_class_ops arto_rtc_ops = {
	.read_time	= arto_rtc_read_time,
	.set_time	= arto_rtc_set_time,
	.read_alarm	= arto_rtc_read_alarm,
	.set_alarm	= arto_rtc_set_alarm,
	.alarm_irq_enable = arto_rtc_alarm_irq_enable,
};

static irqreturn_t arto_rtc_interrupt(int irq, void *id)
{
	struct arto_rtc_dev *pdata = (struct arto_rtc_dev *) id;

	/* Check if interrupt asserted */
	if (!(readl(pdata->csr_base + RTC_STAT) & RTC_STAT_BIT))
		return IRQ_NONE;

	/* Clear interrupt */
	readl(pdata->csr_base + RTC_EOI);

	rtc_update_irq(pdata->rtc, 1, RTC_IRQF | RTC_AF);

	return IRQ_HANDLED;
}

static int arto_rtc_probe(struct platform_device *pdev)
{
	struct arto_rtc_dev *pdata;
	struct resource *res;
	int ret;
	int irq;

	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;
	platform_set_drvdata(pdev, pdata);
	pdata->dev = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pdata->csr_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(pdata->csr_base))
		return PTR_ERR(pdata->csr_base);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "No IRQ resource\n");
		return irq;
	}
	ret = devm_request_irq(&pdev->dev, irq, arto_rtc_interrupt, 0,
				   dev_name(&pdev->dev), pdata);
	if (ret) {
		dev_err(&pdev->dev, "Could not request IRQ\n");
		return ret;
	}

	pdata->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(pdata->clk)) {
		dev_err(&pdev->dev, "Couldn't get the clock for RTC\n");
		return -ENODEV;
	}
	ret = clk_prepare_enable(pdata->clk);
	if (ret)
		return ret;

	/* Turn on the clock and the crystal */
	writel(RTC_CCR_EN, pdata->csr_base + RTC_CCR);

	ret = device_init_wakeup(&pdev->dev, 1);
	if (ret) {
		clk_disable_unprepare(pdata->clk);
		return ret;
	}

	pdata->rf_enable = device_property_read_bool(&pdev->dev,
				"arto,enable_rf");
	if(pdata->rf_enable)
	{
		pdata->rtc = devm_rtc_device_register(&pdev->dev, pdev->name,
						 &arto_rtc_ops_with_rf, THIS_MODULE);
		if (IS_ERR(pdata->rtc)) {
			clk_disable_unprepare(pdata->clk);
			return PTR_ERR(pdata->rtc);
		}
	}
	else
	{
		pdata->rtc = devm_rtc_device_register(&pdev->dev, pdev->name,
						 &arto_rtc_ops, THIS_MODULE);
		if (IS_ERR(pdata->rtc)) {
			clk_disable_unprepare(pdata->clk);
			return PTR_ERR(pdata->rtc);
		}
	}

	/* HW does not support update faster than 1 seconds */
	pdata->rtc->uie_unsupported = 1;

	return 0;
}

static int arto_rtc_remove(struct platform_device *pdev)
{
	struct arto_rtc_dev *pdata = platform_get_drvdata(pdev);

	arto_rtc_alarm_irq_enable(&pdev->dev, 0);
	device_init_wakeup(&pdev->dev, 0);
	clk_disable_unprepare(pdata->clk);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int arto_rtc_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct arto_rtc_dev *pdata = platform_get_drvdata(pdev);
	int irq;

	/*
	 * If this RTC alarm will be used for waking the system up,
	 * don't disable it of course. Else we just disable the alarm
	 * and await suspension.
	 */
	irq = platform_get_irq(pdev, 0);
	if (device_may_wakeup(&pdev->dev)) {
		if (!enable_irq_wake(irq))
			pdata->irq_wake = 1;
	} else {
		pdata->irq_enabled = arto_rtc_alarm_irq_enabled(dev);
		arto_rtc_alarm_irq_enable(dev, 0);
		clk_disable_unprepare(pdata->clk);
	}

	return 0;
}

static int arto_rtc_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct arto_rtc_dev *pdata = platform_get_drvdata(pdev);
	int irq;
	int rc;

	irq = platform_get_irq(pdev, 0);
	if (device_may_wakeup(&pdev->dev)) {
		if (pdata->irq_wake) {
			disable_irq_wake(irq);
			pdata->irq_wake = 0;
		}
	} else {
			rc = clk_prepare_enable(pdata->clk);
			if (rc) {
				dev_err(dev, "Unable to enable clock error %d\n", rc);
				return rc;
			}
			arto_rtc_alarm_irq_enable(dev, pdata->irq_enabled);
	}

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(arto_rtc_pm_ops, arto_rtc_suspend, arto_rtc_resume);

#ifdef CONFIG_OF
static const struct of_device_id artosyn_rtc_of_match[] = {
	{.compatible = "artosyn,ar9301-rtc" },
	{ }
};
MODULE_DEVICE_TABLE(of, artosyn_rtc_of_match);
#endif

static struct platform_driver artosyn_rtc_driver = {
	.probe		= arto_rtc_probe,
	.remove		= arto_rtc_remove,
	.driver		= {
		.name	= "arto-rtc",
		.pm = &arto_rtc_pm_ops,
		.of_match_table	= of_match_ptr(artosyn_rtc_of_match),
	},
};

module_platform_driver(artosyn_rtc_driver);

MODULE_AUTHOR("kaiwang <kai.wang@artosyn.cn>");
MODULE_DESCRIPTION("Artosyn SoC AR9301 RTC driver");
MODULE_LICENSE("GPL v2");
