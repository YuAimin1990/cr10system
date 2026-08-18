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

#include "ar_ifc.h"
#include "ar_ifc_hal.h"

struct ar_ifc_dev ifc_dev;

static int ar_ifc_proc_state_open(struct inode *inode, struct file *file)
{
	return single_open(file, ar_ifc_proc_state_show, inode->i_private);
}

static struct file_operations ar_ifc_proc_state_fops={
	.owner		= THIS_MODULE,
	.open		= ar_ifc_proc_state_open,
	.release	= single_release,
	.read		= seq_read,
};

static int ar_ifc_proc_create(void)
{
	struct proc_dir_entry * entry;

	ifc_dev.proc_dir = proc_mkdir("arifc", NULL);
	if(!ifc_dev.proc_dir)
	{
		printk(KERN_ERR"Create proc dir failed.\n");
		return -ENOMEM;
	}

	entry = proc_create("state", 0, ifc_dev.proc_dir, &ar_ifc_proc_state_fops);
	if(!entry)
	{
		printk(KERN_ERR"Create proc state failed.\n");
		return -ENOMEM;
	}

	return 0;
}

static int ar_ifc_proc_destroy(void)
{
    if(ifc_dev.proc_dir)
    {
        proc_remove(ifc_dev.proc_dir);
    }

    return 0;
}

static int ar_ifc_dev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ar_ifc_dev_close(struct inode *inode, struct file *file)
{
	//printk(KERN_INFO "Close is called.\n");
	return 0;
}

static ssize_t ar_ifc_dev_read(struct file *file, char __user *user, size_t len, loff_t *loff)
{
	//printk(KERN_INFO "Read is called.\n");
	return 0;
}

static int __maybe_unused ar_ifc_suspend(struct device *dev);
static int __maybe_unused ar_ifc_resume(struct device *dev);

static ssize_t ar_ifc_dev_write(struct file *file, const char __user *user, size_t len, loff_t *loff)
{
    char cmd[16] = {0};
    
    len = len > 16 ? 16 : len;
    
    if(copy_from_user(cmd, user, len))
    {
        return -EFAULT;
    }
    
    if(strncmp(cmd, "suspend", 7) == 0)
    {
        ar_ifc_suspend(NULL);
    }
    
    //Enable Auto Resume
    if(strncmp(cmd, "resume", 6) == 0)
    {
        ar_ifc_resume(NULL);
    }
        
    return len;
}

static long ar_ifc_dev_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int ret  = 0;
    unsigned int cache_coherency = 0;
    unsigned int security = 0;
    unsigned int frequency = 0;
    ar_ifc_ioctl_single_st single_param;
    ar_ifc_ioctl_auto_st auto_params;

   int size = _IOC_SIZE(cmd);

    switch(cmd)
    {
            case IFC_IOC_START_SINGLE:
                if(size != sizeof(ar_ifc_ioctl_single_st))
                {
                     printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                     return -EINVAL;
                }

                ret = copy_from_user(&single_param,(char __user *)args, size);
                if(ret)
                {
                     printk(KERN_ERR"copy ioctl args error.\n");
                     return -EFAULT;
                }

                ret = _ar_ifc_start_single(&ifc_dev, &single_param.ifc_param);
                if(ret == IFC_STATE_FINISH_OK)
                {
                    _ar_ifc_calculate_result(&single_param.result_param);
                    ret = copy_to_user((char __user *)args + sizeof(ar_ifc_params_st), &single_param.result_param, sizeof(ar_ifc_result_s));
                    if(ret)
                    {
                        printk(KERN_ERR"copy args to user error.\n");
                        return -EFAULT;
                    }
                    ret = IFC_STATE_FINISH_OK;
                }
                break;

            case IFC_IOC_START_AUTO:
                if(size != sizeof(ar_ifc_ioctl_auto_st))
                {
                    printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                    return -EINVAL;
                }

                ret = copy_from_user(&auto_params,(char __user *)args, size);
                if(ret)
                {
                    printk(KERN_ERR"copy ioctl args error.\n");
                    return -EFAULT;
                }

                ret = _ar_ifc_start_auto(&ifc_dev, &auto_params);
                break;
            case IFC_IOC_SET_CACHE_COHERENCY:
                if(size != sizeof(unsigned int))
                {
                    printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                    return -EINVAL;
                }

                ret = copy_from_user(&cache_coherency, (char __user *)args, size);
                if(ret)
                {
                    printk(KERN_ERR"copy ioctl args error.\n");
                    return -EFAULT;
                }
                ret = _ar_ifc_set_cache_coherency(cache_coherency);
                break;

            case IFC_IOC_SET_SECURITY:
                if(size != sizeof(unsigned int))
                {
                    printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                    return -EINVAL;
                }

                ret = copy_from_user(&security, (char __user *)args, size);
                if(ret)
                {
                    printk(KERN_ERR"copy ioctl args error.\n");
                    return -EFAULT;
                }
                ret = _ar_ifc_set_security(security);
                break;
			case IFC_IOC_SET_FREQUENCY:
                if(size != sizeof(unsigned int))
                {
                    printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                    return -EINVAL;
                }

                ret = copy_from_user(&frequency, (char __user *)args, size);
                if(ret)
                {
                    printk(KERN_ERR"copy ioctl args error.\n");
                    return -EFAULT;
                }
                ret = _ar_ifc_set_frequency(frequency);
                break;

            default:
                printk(KERN_ERR"No this ioctl: %d.\n", cmd);
                break;
	}

	return ret == IFC_STATE_FINISH_OK ? 0 : ret;
}

static struct file_operations ar_ifc_fops={
	.owner		= THIS_MODULE,
	.open		= ar_ifc_dev_open,
	.release	= ar_ifc_dev_close,
	.read		= ar_ifc_dev_read,
	.write		= ar_ifc_dev_write,
	.unlocked_ioctl = ar_ifc_dev_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl	= ar_ifc_dev_ioctl,
#endif
};

static int ar_ifc_probe(struct platform_device *pdev)
{
	int ret = 0;
  	struct resource *res = NULL;
  	unsigned int frequency = 0;
  	unsigned int control = 0;

	if(!pdev)
	{
		return -EINVAL;
	}

	memset(&ifc_dev, 0, sizeof(ifc_dev));

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res)
	{
		return -EINVAL;
	}

	if((res->end - res->start + 1) < AR_IFC_REG_SPACE_RANGE) {
		dev_err(&pdev->dev, "I/O space is too small\n");
		return -EINVAL;
	}

	ifc_dev.base = ioremap_nocache(res->start, AR_IFC_REG_SPACE_RANGE);
	if(!ifc_dev.base)
	{
		return -ENOMEM;
	}

    if (of_property_read_u32(pdev->dev.of_node, "control", &control))
	{
		control = AR_IFC_GLB_CTRL_ADDR;
	}

	ifc_dev.global_ctrl = ioremap_nocache(control, AR_IFC_GLB_SPACE_RANGE);
	if(!ifc_dev.global_ctrl)
	{
		goto fail;
	}

	if (!of_property_read_u32(pdev->dev.of_node, "frequency", &frequency))
	{
		ifc_dev.frequency = frequency;
	}
	else
	{
		ifc_dev.frequency = 0;
	}

	ifc_dev.irq = platform_get_irq(pdev, 0);
	if(ifc_dev.irq < 0)
	{
		printk(KERN_ERR"No IRQs\n");
		goto fail;
	}

	ret = request_irq(ifc_dev.irq, _ar_ifc_irq_handler, 0, "arifc", &ifc_dev);
	if(ret < 0)
	{
		printk(KERN_ERR"Request irq fail.\n");
		goto fail;
	}

	//2. register misc dev
	ifc_dev.miscdev.minor = MISC_DYNAMIC_MINOR;
	ifc_dev.miscdev.name="arifc";
	ifc_dev.miscdev.fops = &ar_ifc_fops;

	ret = misc_register(&ifc_dev.miscdev);
	if(ret < 0)
	{
		goto fail;
	}

	//Init ifc global registers, like PLL, power ctrl
	_ar_ifc_poweron(&ifc_dev);

    ar_ifc_proc_create();

    init_completion(&ifc_dev.done);
    mutex_init(&ifc_dev.lock);

	printk("IFC registered.\n");
	return 0;

fail:
    if(ifc_dev.base)
    {
    	iounmap(ifc_dev.base);
    	ifc_dev.base = NULL;
    }

    if(ifc_dev.global_ctrl)
    {
    	iounmap(ifc_dev.global_ctrl);
    	ifc_dev.global_ctrl = NULL;
    }

	return ret;
}

static int ar_ifc_remove(struct platform_device *pdev)
{
    _ar_ifc_poweroff(&ifc_dev);

    if(ifc_dev.base)
    {
    	iounmap(ifc_dev.base);
    	ifc_dev.base = NULL;
    }

    if(ifc_dev.global_ctrl)
    {
    	iounmap(ifc_dev.global_ctrl);
    	ifc_dev.global_ctrl = NULL;
    }

    ar_ifc_proc_destroy();
    free_irq(ifc_dev.irq, &ifc_dev);
	misc_deregister(&ifc_dev.miscdev);


    printk("IFC module has been unloaded.\n");

	return 0;
}

//Only need to wait until the last task finishs
static int __maybe_unused ar_ifc_suspend(struct device *dev)
{
    mutex_lock(&ifc_dev.lock);
    if(ifc_dev.suspended)
    {
        printk(KERN_INFO"IFC suspended already.\n");
        mutex_unlock(&ifc_dev.lock);
        return 0;
    }
    _ar_ifc_poweroff(&ifc_dev);
    ifc_dev.suspended = 1;
    printk(KERN_INFO "IFC suspended.\n");
    mutex_unlock(&ifc_dev.lock);
    return 0;
}

static int __maybe_unused ar_ifc_resume(struct device *dev)
{
    mutex_lock(&ifc_dev.lock);
    if(!ifc_dev.suspended)
    {
        printk(KERN_INFO"IFC is alive, no need to resume.\n");
        mutex_unlock(&ifc_dev.lock);
        return 0;
    }
    _ar_ifc_poweron(&ifc_dev);
    ifc_dev.suspended = 0;
    printk(KERN_INFO "IFC resumed.\n");    
    mutex_unlock(&ifc_dev.lock);

    return 0;
}

static const struct dev_pm_ops ar_ifc_pm_ops = {
        .suspend        = ar_ifc_suspend,
        .resume         = ar_ifc_resume,
};

static const struct of_device_id ar_ifc_match[] = {
	{.compatible = "artosyn,ifc"},
	{},
};

MODULE_DEVICE_TABLE(of, ar_ifc_match);

static struct platform_driver ar_ifc_platform_driver = {
	.probe = ar_ifc_probe,
	.remove = ar_ifc_remove,
	.driver = {
		.name = "artosyn ifc",
		.of_match_table = ar_ifc_match,
		.pm = &ar_ifc_pm_ops,
	},
};

static int __init ar_ifc_dev_init(void)
{
	return platform_driver_register(&ar_ifc_platform_driver);
}

static void __exit ar_ifc_dev_exit(void)
{
	platform_driver_unregister(&ar_ifc_platform_driver);
}

static int ifc_debug_level = 1;
module_param(ifc_debug_level, int, S_IRUGO);
module_init(ar_ifc_dev_init);
module_exit(ar_ifc_dev_exit);

MODULE_AUTHOR("artosyn");
MODULE_DESCRIPTION("Artosyn IFC driver");
MODULE_LICENSE("GPL");

