#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/uaccess.h>
#include <linux/pagemap.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>
#include <linux/artosyn_ion.h>

#include "ar_gdc.h"
#include "ar_gdc_hal.h"


struct AR_GDC_DEV gdc_dev;

static int ar_gdc_proc_state_show(struct seq_file *s, void *p)
{
    unsigned int val = 0, rot = 0;

    mutex_lock(&gdc_dev.lock);
    if(gdc_dev.suspended)
    {
        seq_printf(s, "GDC has been suspended!\n");
        mutex_unlock(&gdc_dev.lock);
        return 0;
    }
    
    seq_printf(s, "--------------------Src img--------------------\n");
    val = readl(gdc_dev.base + 5 * 4);
    seq_printf(s, "  Width       :  %d\n", val & 0xFFFF);
    seq_printf(s, "  Height      :  %d\n", (val >> 16) & 0xFFFF);
    seq_printf(s, "  Y addr      :  0x%08x\n", readl(gdc_dev.base + 9 * 4));
    seq_printf(s, "  Y stride    :  %d\n", readl(gdc_dev.base + 10 * 4) & 0xFFFF);
    seq_printf(s, "  U addr      :  0x%08x\n", readl(gdc_dev.base + 11 * 4));
    seq_printf(s, "  U stride    :  %d\n", readl(gdc_dev.base + 12 * 4) & 0xFFFF);
    seq_printf(s, "  V addr      :  0x%08x\n", readl(gdc_dev.base + 13 * 4));
    seq_printf(s, "  V stride    :  %d\n", readl(gdc_dev.base + 14 * 4) & 0xFFFF);
    seq_printf(s, "--------------------Dst img--------------------\n");
    val = readl(gdc_dev.base + 6 * 4);
    seq_printf(s, "  Width       :  %d\n", val & 0xFFFF);
    seq_printf(s, "  Height      :  %d\n", (val >> 16) & 0xFFFF);
    seq_printf(s, "  Y addr      :  0x%08x\n", readl(gdc_dev.base + 15 * 4));
    seq_printf(s, "  Y stride    :  %d\n", readl(gdc_dev.base + 16 * 4) & 0xFFFF);
    seq_printf(s, "  U addr      :  0x%08x\n", readl(gdc_dev.base + 17 * 4));
    seq_printf(s, "  U stride    :  %d\n", readl(gdc_dev.base + 18 * 4) & 0xFFFF);
    seq_printf(s, "  V addr      :  0x%08x\n", readl(gdc_dev.base + 19 * 4));
    seq_printf(s, "  V stride    :  %d\n", readl(gdc_dev.base + 20 * 4) & 0xFFFF);
    seq_printf(s, "--------------------Ctrl reg--------------------\n");
    seq_printf(s, "  LUT addr    :  0x%08x\n", readl(gdc_dev.base + 7 * 4));
    seq_printf(s, "  LUT stride  :  %d\n", readl(gdc_dev.base + 8 * 4) & 0xFFFF);
    val = readl(gdc_dev.base + 27 * 4);
    rot = ((val >> 2) & 0x3) * 90;
    seq_printf(s, "  Rotation    :  %d degree\n", rot);
    seq_printf(s, "  Pipe bypass :  %s\n", (val & 0x2) ? "Y":"N");
    seq_printf(s, "  Mode        :  %s\n", (val & 0x1) ? "Pipeline":"DDR");
    val = readl(gdc_dev.base + 28 * 4);
    seq_printf(s, "  CF50 luma   :\n");
    seq_printf(s, "    Enable    :  %s\n", (val & 0x1) ? "Y" : "N");
    seq_printf(s, "    Endian    :  %s\n", (val & (1<<12))? "Big" : "Little");
    seq_printf(s, "    Ratio     :  %d\n", (val >> 4) & 0xFF);
    seq_printf(s, "    Bits      :  %d\n", (val & 0x8) ? 16 : 8);
    seq_printf(s, "    Lossless  :  %s\n", (val & 0x2) ? "Y":"N");
    val = readl(gdc_dev.base + 29 * 4);
    seq_printf(s, "  CF50 chroma :\n");
    seq_printf(s, "    Enable    :  %s\n", (val & 0x1) ? "Y" : "N");
    seq_printf(s, "    Endian    :  %s\n", (val & (1<<12))? "Big" : "Little");
    seq_printf(s, "    Ratio     :  %d\n", (val >> 4) & 0xFF);
    seq_printf(s, "    Bits      :  %d\n", (val & 0x8) ? 16 : 8);
    seq_printf(s, "    Lossless  :  %s\n", (val & 0x2) ? "Y":"N");
    val = readl(gdc_dev.base + 30 * 4);
    seq_printf(s, "  AXI config  :\n");
    seq_printf(s, "    4k_split  :  %s\n", (val & (1 << 8)) ? "Y" : "N");
    seq_printf(s, "    AwCache   :  0x%x\n", (val >> 4) & 0xF);
    seq_printf(s, "    AwCache   :  0x%x\n", val & 0xF);
    seq_printf(s, "  Sram ctrl   :  0x%08x\n", readl(gdc_dev.base + 31 * 4));
    seq_printf(s, "  Clock gate  :  0x%08x\n", readl(gdc_dev.base + 32 * 4));
    seq_printf(s, "---------------------Status---------------------\n");
    val = readl(gdc_dev.base + 35 * 4);
    seq_printf(s, "  State       :  0x%x\n", readl(gdc_dev.base + 35 * 4));
    if(val & 0x4)
    {
        seq_printf(s, "    Bit[2]    :  Safety reset\n");
    }
    if(val & 0x2)
    {
        seq_printf(s, "    Bit[1]    :  Pipeline error\n");
    }
    if(val & 0x1)
    {
        seq_printf(s, "    Bit[0]    :  Busy\n");
    }
    if(!val)
    {
        seq_printf(s, "    IDLE\n");
    }
    val = readl(gdc_dev.base + 3 * 4);
    seq_printf(s, "  IrqStatus   :  0x%x\n", val);
    if(val & 0x2)
    {
        seq_printf(s, "    Bit[1]    :  Pipeline error\n");
    }
    if(val & 0x1)
    {
        seq_printf(s, "    Bit[0]    :  Done\n");
    }

	seq_printf(s, "    GDC takes %d x 0.01 ms\n", gdc_dev.tprocess);

    mutex_unlock(&gdc_dev.lock);
    return 0;
}

static int ar_gdc_proc_state_open(struct inode *inode, struct file *file)
{
	return single_open(file, ar_gdc_proc_state_show, inode->i_private);
}

static struct file_operations ar_gdc_proc_state_fops={
	.owner		= THIS_MODULE,
	.open		= ar_gdc_proc_state_open,
	.release	= single_release,
	.read		= seq_read,
};


static int ar_gdc_proc_lut_show(struct seq_file *s, void *p)
{
	int i=0;
	int *pLutMem = (int*)gdc_dev.lutbase;
	for(i=0;i<gdc_dev.lutlen;i++)
	{
		seq_printf(s, "LutMem[%d]:	 0x%x\n", i,pLutMem[i]);
	}

    return 0;
}

static int ar_gdc_proc_lut_open(struct inode *inode, struct file *file)
{
	return single_open(file, ar_gdc_proc_lut_show, inode->i_private);
}

static struct file_operations ar_gdc_proc_lut_fops={
	.owner		= THIS_MODULE,
	.open		= ar_gdc_proc_lut_open,
	.release	= single_release,
	.read		= seq_read,
};


static int ar_gdc_proc_create(void)
{
	struct proc_dir_entry * entry;

	gdc_dev.proc_dir = proc_mkdir("argdc", NULL);
	if(!gdc_dev.proc_dir)
	{
		printk(KERN_ERR"Create proc dir failed.\n");
		return -ENOMEM;
	}

	entry = proc_create("state", 0, gdc_dev.proc_dir, &ar_gdc_proc_state_fops);
	if(!entry)
	{
		printk(KERN_ERR"Create proc state failed.\n");
		return -ENOMEM;
	}

	entry = proc_create("lut", 0, gdc_dev.proc_dir, &ar_gdc_proc_lut_fops);
	if(!entry)
	{
		printk(KERN_ERR"Create proc lut failed.\n");
		return -ENOMEM;
	}

	return 0;
}

static int ar_gdc_proc_destroy(void)
{
    if(gdc_dev.proc_dir)
    {
        proc_remove(gdc_dev.proc_dir);
    }

    return 0;
}

static int ar_gdc_dev_open(struct inode *inode, struct file *file)
{
	//printk(KERN_INFO "Open is called.\n");
	int ret = ar_gdc_isp_clk_check();
	if(ret == 0)
	{
	    printk(KERN_ERR"ISP Clk is OFF, Open Dev err!\n");
		return -1;
	}
	
	return 0;
}

static int ar_gdc_dev_close(struct inode *inode, struct file *file)
{
	//printk(KERN_INFO "Close is called.\n");
	return 0;
}

static ssize_t ar_gdc_dev_read(struct file *file, char __user *user, size_t len, loff_t *loff)
{
	//printk(KERN_INFO "Read is called.\n");
	return 0;
}

static int __maybe_unused ar_gdc_suspend(struct device *dev);
static int __maybe_unused ar_gdc_resume(struct device *dev);

static ssize_t ar_gdc_dev_write(struct file *file, const char __user *user, size_t len, loff_t *loff)
{
    char cmd[16] = {0};
    
    len = len > 16 ? 16 : len;
    
    if(copy_from_user(cmd, user, len))
    {
        return -EFAULT;
    }
    
    if(strncmp(cmd, "suspend", 7) == 0)
    {
        ar_gdc_suspend(NULL);
    }
    
    //Enable Auto Resume
    if(strncmp(cmd, "resume", 6) == 0)
    {
        ar_gdc_resume(NULL);
    }
    
    return len;
}

static long ar_gdc_dev_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int ret  = 0;
    AR_GDC_PARAMS_S* p_gdc_params =NULL;
	unsigned int frequency = 0;

    int size = _IOC_SIZE(cmd);
    switch(cmd)
    {
        case AR_GDC_IOC_START:
            if(size != sizeof(AR_GDC_PARAMS_S))
            {
                printk("Ioctl cmd %d param mismatch.\n", cmd);
                return -EINVAL;
            }
            p_gdc_params = kmalloc(size, GFP_KERNEL);
            if(!p_gdc_params)
            {
                printk("Kmalloc failed.\n");
                return -ENOMEM;
            }
            memset(p_gdc_params, 0, size);
            ret = copy_from_user(p_gdc_params,(char __user *)args, size);
            if(ret)
            {
                printk("Copy ioctl args error.\n");
                kfree(p_gdc_params);
                return ret;
            }

            ret = ar_gdc_start_process(&gdc_dev, p_gdc_params);
            kfree(p_gdc_params);
        break;
		case AR_GDC_IOC_SET_FREQUENCY:
			if(size != sizeof(unsigned int))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                return -EINVAL;
            }

            ret = copy_from_user(&frequency, (char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                return ret;
            }
            ret = ar_gdc_set_frequency(frequency);
            break;
		break;
		case AR_GDC_IOC_GET_TIME:
            if(size != sizeof(unsigned long long))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                return ret;
            }
            ret = copy_to_user((char __user *)args, &gdc_dev.tprocess, size);
            break;
        default:
        printk("No this ioctl: %d.\n", cmd);
    }

    return ret;
}

static struct file_operations ar_gdc_fops = {
	.owner		= THIS_MODULE,
	.open		= ar_gdc_dev_open,
	.release	= ar_gdc_dev_close,
	.read		= ar_gdc_dev_read,
	.write		= ar_gdc_dev_write,
	.unlocked_ioctl = ar_gdc_dev_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl	= ar_gdc_dev_ioctl,
#endif
};

static int ar_gdc_probe(struct platform_device *pdev)
{
    int ret = 0;
    unsigned int control = 0;
    unsigned int frequency = 0;
    struct resource *res = NULL;

    if(!pdev)
    {
    	return -EINVAL;
    }

    memset(&gdc_dev, 0, sizeof(gdc_dev));

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(!res)
    {
    	return -EINVAL;
    }

    if((res->end - res->start + 1) < AR_GDC_REG_SPACE_RANGE)
    {
    	dev_err(&pdev->dev, "I/O space is too small\n");
    	return -EINVAL;
    }

    gdc_dev.base = ioremap_nocache(res->start, AR_GDC_REG_SPACE_RANGE);
    if(!gdc_dev.base)
    {
    	return -ENOMEM;
    }

    printk("gdc_dev.base Phy: %llx\r\n",res->start);
    printk("gdc_dev.base Virt: %p\r\n",gdc_dev.base);

	if (of_property_read_u32(pdev->dev.of_node, "control", &control))
	{
		control = AR_GLOBAL_CONTROL_BASE;
	}
	gdc_dev.global_ctrl = ioremap_nocache(control, AR_GLOBAL_CONTROL_RANGE);
	if(!gdc_dev.global_ctrl)
	{
		return -ENOMEM;
	}

	if (!of_property_read_u32(pdev->dev.of_node, "frequency", &frequency))
    {
        gdc_dev.frequency = frequency;
    }
    else
    {
        gdc_dev.frequency = 0;
    }
	
	if(ar_gdc_isp_clk_check() == 0)
	{
	    printk(KERN_ERR"ISP Clk is OFF!\n");
		ret = -1;
		goto fail;
	}


	gdc_dev.lutbase = vmalloc(AR_GDC_LUT_SPACE_RANGE);
    if(!gdc_dev.lutbase)
    {
        printk("gdc_dev.lutbase vmalloc failed.\n");
        return -ENOMEM;
    }
    memset(gdc_dev.lutbase, 0, AR_GDC_LUT_SPACE_RANGE);
    printk("gdc_dev.lutbase Virt: %p\r\n",gdc_dev.lutbase);

    gdc_dev.irq = platform_get_irq(pdev, 0);
    if(gdc_dev.irq < 0)
    {
    	printk(KERN_ERR"No IRQs\n");
    	goto fail;
    }

    ret = request_irq(gdc_dev.irq, ar_gdc_irq_handler, 0, "argdc", &gdc_dev);
    if(ret < 0)
    {
    	printk(KERN_ERR"Request IRQ Failed.\n");
    	goto fail;
    }

    gdc_dev.miscdev.minor = MISC_DYNAMIC_MINOR;
    gdc_dev.miscdev.name = "argdc";
    gdc_dev.miscdev.fops = &ar_gdc_fops;

    ret = misc_register(&gdc_dev.miscdev);
    if(ret < 0)
    {
    	goto fail;
    }

    ar_gdc_proc_create();

	ar_gdc_set_frequency(gdc_dev.frequency);

    ar_gdc_hw_init(&gdc_dev);

    init_completion(&gdc_dev.done);
    mutex_init(&gdc_dev.lock);


    printk("gdc_dev.freq: %d\n", gdc_dev.frequency);

    printk("GDC Registered Done.\n");
    return 0;

fail:
    if(gdc_dev.base)
    {
    	iounmap(gdc_dev.base);
    	gdc_dev.base = NULL;
    }

	if(gdc_dev.lutbase)
	{
		vfree(gdc_dev.lutbase);
	}

	if(gdc_dev.global_ctrl)
    {
        iounmap(gdc_dev.global_ctrl);
        gdc_dev.global_ctrl = NULL;
    }

    printk("GDC Register Failed.\n");
    return ret;
}

static int ar_gdc_remove(struct platform_device *pdev)
{
    ar_gdc_poweroff(&gdc_dev);
    if(gdc_dev.base)
    {
    	iounmap(gdc_dev.base);
    	gdc_dev.base = NULL;
    }
	if(gdc_dev.lutbase)
	{
		vfree(gdc_dev.lutbase);
	}

    ar_gdc_proc_destroy();
    free_irq(gdc_dev.irq, &gdc_dev);
    misc_deregister(&gdc_dev.miscdev);

    printk("GDC module has been unloaded.\n");

    return 0;
}

//Only need to wait until the last task finishs
static int __maybe_unused ar_gdc_suspend(struct device *dev)
{
    mutex_lock(&gdc_dev.lock);
    if(gdc_dev.suspended)
    {
        printk(KERN_INFO"GDC suspended already.\n");
        mutex_unlock(&gdc_dev.lock);
        return 0;
    }
    ar_gdc_poweroff(&gdc_dev);
    printk(KERN_INFO"GDC suspended.\n");
    gdc_dev.suspended = 1;
    mutex_unlock(&gdc_dev.lock);
    
    return 0;
}

static int __maybe_unused ar_gdc_resume(struct device *dev)
{
    mutex_lock(&gdc_dev.lock);
    if(!gdc_dev.suspended)
    {
        printk(KERN_INFO"GDC is alive, no need to resume.\n");
        mutex_unlock(&gdc_dev.lock);
        return 0;
    }
    ar_gdc_hw_init(&gdc_dev);
    gdc_dev.suspended = 0;
    printk(KERN_INFO"GDC resumed.\n");
    mutex_unlock(&gdc_dev.lock);

    return 0;
}

static const struct dev_pm_ops ar_gdc_pm_ops = {
        .suspend        = ar_gdc_suspend,
        .resume         = ar_gdc_resume,
};

static const struct of_device_id ar_gdc_match[] = {
	{.compatible = "artosyn,gdc"},
	{},
};

MODULE_DEVICE_TABLE(of, ar_gdc_match);

static struct platform_driver ar_gdc_platform_driver = {
	.probe = ar_gdc_probe,
	.remove = ar_gdc_remove,
	.driver = {
		.name = "artosyn gdc",
		.of_match_table = ar_gdc_match,
		.pm = &ar_gdc_pm_ops,
	},
};

static int __init ar_gdc_dev_init(void)
{
    printk(KERN_INFO"GDC Init Start...\n");
	return platform_driver_register(&ar_gdc_platform_driver);
}

static void __exit ar_gdc_dev_exit(void)
{
    printk(KERN_INFO"GDC Deinit Start...\n");
	platform_driver_unregister(&ar_gdc_platform_driver);
}

static int eis_debug_level = 1;
module_param(eis_debug_level, int, S_IRUGO);
module_init(ar_gdc_dev_init);
module_exit(ar_gdc_dev_exit);

MODULE_AUTHOR("Artosyn");
MODULE_DESCRIPTION("Artosyn GDC Driver");
MODULE_LICENSE("GPL v2");

