/*
 * Artosyn Sirius GMAC MDIO interface driver
 *
 * Copyright 2017 Robin Bai <haibin.bai@artosyn.cn>
 *
 * Based on the Linux driver provided by Beyond Ethernet Controller
*/

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of_address.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#define SIRIUS_MIICOMMAND (4 * 0x0B)
#define SIRIUS_MIIADDRESS (4 * 0x0C)
#define SIRIUS_MIITX_DATA (4 * 0x0D)
#define SIRIUS_MIIRX_DATA (4 * 0x0E)
#define SIRIUS_MIISTATUS  (4 * 0x0F)

/* MII Mode Register */
#define SIRIUS_MIIMODER_CLKDIV    0x000000FF /* Clock Divider */
#define SIRIUS_MIIMODER_NOPRE     0x00000100 /* No Preamble */
#define SIRIUS_MIIMODER_RST       0x00000200 /* MIIM Reset */

/* MII Command Register */
#define SIRIUS_MIICOMMAND_SCANSTAT  0x00000001 /* Scan Status */
#define SIRIUS_MIICOMMAND_RSTAT     0x00000002 /* Read Status */
#define SIRIUS_MIICOMMAND_WCTRLDATA 0x00000004 /* Write Control Data */

/* MII Address Register */
#define SIRIUS_MIIADDRESS_FIAD    0x0000001F /* PHY Address */
#define SIRIUS_MIIADDRESS_RGAD    0x00001F00 /* RGAD Address */

/* MII Status Register */
#define SIRIUS_MIISTATUS_LINKFAIL 0x00000001 /* Link Fail */
#define SIRIUS_MIISTATUS_BUSY     0x00000002 /* MII Busy */
#define SIRIUS_MIISTATUS_INVALID   0x00000004 /* Data in MII Status Register is invalid */

struct sirius_mdio_data 
{
	void __iomem *membase;
	//TBD
};

static inline void sirius_set_miiaddress(void *baseaddr, u32 val)
{
        writel(val, baseaddr + SIRIUS_MIIADDRESS);
}

static inline void sirius_set_miitx_data(void *baseaddr, u32 val)
{
        writel(val, baseaddr + SIRIUS_MIITX_DATA);
}

static inline u32 sirius_get_miirx_data(void *baseaddr)
{
        return readl(baseaddr + SIRIUS_MIIRX_DATA);
}

static inline void sirius_set_miicommand(void *baseaddr, u32 val)
{
        writel(val, baseaddr + SIRIUS_MIICOMMAND);
}

static inline u32 sirius_get_miicommand(void *baseaddr)
{
        return readl(baseaddr + SIRIUS_MIICOMMAND);
}

static inline u32 sirius_get_miistatus(void *baseaddr)
{
        return readl(baseaddr + SIRIUS_MIISTATUS);
}

/*--------------------------------------------------[ Generic MII support ]---*/
static int sirius_mdio_read(struct mii_bus *bus, int phy_addr, int regnum)
{
        struct sirius_mdio_data * data = bus->priv;
        u32 st, cmd;

        /* Timeout 1s for this command */
        unsigned long timeout = jiffies + HZ;


        sirius_set_miiaddress(data->membase, (regnum << 8) | phy_addr);
        sirius_set_miicommand(data->membase, SIRIUS_MIICOMMAND_RSTAT);

        do {
                st = sirius_get_miistatus(data->membase);

                if(time_after(jiffies, timeout))
                {
                        /* Check mii status once more, just to
                        be sure that timeout really occourred,
                        since we are not using any IRQ locking. */
                        st = sirius_get_miistatus(data->membase);
                        if ((st & SIRIUS_MIISTATUS_BUSY) == 0)
                                break;

                        cmd = sirius_get_miicommand(data->membase);
                        printk(KERN_ERR "MDIO read timeout (cmd: 0x%08x, "\
                               "status: 0x%08x)\n", cmd, st);
                        return -EIO;
                }
        } while(st & SIRIUS_MIISTATUS_BUSY);

        st = sirius_get_miirx_data(data->membase);

	//printk("sirius mdio read: %x %x %x\n", phy_addr, regnum, st);
        return st;
}

static int sirius_mdio_write(struct mii_bus *bus, int phy_addr, int regnum,
                           u16 val)
{
        struct sirius_mdio_data  * data = bus->priv;
        u32 st, cmd;

        /* Timeout 1s for this command */
        unsigned long timeout = jiffies + HZ;

	//printk("sirius mdio write: %x %x %x\n", phy_addr, regnum, val);

        sirius_set_miiaddress(data->membase, (regnum << 8) | phy_addr);
        sirius_set_miitx_data(data->membase, val);
        sirius_set_miicommand(data->membase, SIRIUS_MIICOMMAND_WCTRLDATA);

        do {
                st = sirius_get_miistatus(data->membase);

                if(time_after(jiffies, timeout))
                {
                        /* Check mii status once more, just to
                        be sure that timeout really occourred,
                        since we are not using any IRQ locking. */
                        st = sirius_get_miistatus(data->membase);
                        if ((st & SIRIUS_MIISTATUS_BUSY) == 0)
                                break;

                        cmd = sirius_get_miicommand(data->membase);
                        printk(KERN_ERR "MDIO write timeout (cmd: 0x%08x, "\
                               "status: 0x%08x)\n", cmd, st);
                        return -EIO;
                }
        } while(st & SIRIUS_MIISTATUS_BUSY);

        return 0;
}

static int sirius_mdio_reset(struct mii_bus *bus)
{
        return 0;
}

static int sirius_mdio_probe(struct platform_device *pdev)
{
        struct device_node *np = pdev->dev.of_node;
        struct mii_bus *bus;
        struct resource *res;
	struct sirius_mdio_data * data;
        int ret;
	
	pr_info("Sirius mdio probe...\n");
	bus = mdiobus_alloc_size(sizeof(*data));
	if(!bus)
		return -ENOMEM;

	bus->name = "sirius_mii_bus";
	bus->read = sirius_mdio_read;
	bus->write = sirius_mdio_write;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s-mii", dev_name(&pdev->dev));
	bus->parent = &pdev->dev;

	data = bus->priv;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	data->membase = devm_ioremap_resource(&pdev->dev, res);
	if(IS_ERR(data->membase))
	{
		ret = PTR_ERR(data->membase);
		goto err_out_free_mdiobus;
	}

	//phy-reset time > 10ms
    writel(0x0, data->membase+0x1004);
	mdelay(10);
    writel(0x1, data->membase+0x1004);

	ret = of_mdiobus_register(bus, np);
	if(ret < 0)
		goto err_out_free_mdiobus;

	platform_set_drvdata(pdev, bus);

	pr_info("sirius mdio register succussfully.\n");
	return 0;

err_out_free_mdiobus:
	mdiobus_free(bus);
	return ret;
}

static int sirius_mdio_remove(struct platform_device *pdev)
{
        struct mii_bus *bus = platform_get_drvdata(pdev);
	
	//kfree(bus->irq);
        mdiobus_unregister(bus);
        mdiobus_free(bus);

	pr_info("sirius mdio remove succussfully.\n");
        return 0;
}

static const struct of_device_id sirius_mdio_of_match[] = {
        {.compatible = "artosyn,sirius-gmac-mdio",},
		0
};
MODULE_DEVICE_TABLE(of, sirius_mdio_of_match);

static struct platform_driver sirius_mdio_driver = {
        .probe = sirius_mdio_probe,
        .remove = sirius_mdio_remove,
        .resume = NULL,
        .suspend = NULL,
        .driver = {
                .name = "sirius-mdio",
                .of_match_table = sirius_mdio_of_match,
                .owner = THIS_MODULE,
        },
};

module_platform_driver(sirius_mdio_driver);

MODULE_AUTHOR("Robin bai <haibin.bai@artosyn.cn>");
MODULE_DESCRIPTION("Artosyn Sirius GMAC MDIO driver based on beth");
MODULE_LICENSE("GPL");


