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

#if defined(CONFIG_ION) 
#include <linux/artosyn_ion.h>
#else
#include <linux/mman.h>
#include <osal_mmz.h>
#endif


#include "ar_scaler.h"
#include "ar_scaler_hal.h"

#define AR_ION_SYSTEM_HEAP_ID		(27)

#define AR_SCALER_ION_NAME    "ar_scaler"

struct ar_scaler_dev scaler_dev;

static int ar_scaler_proc_state_show(struct seq_file *s, void *p)
{
    unsigned long long dist = 0;
    unsigned int val = 0, val1 = 0;

    mutex_lock(&scaler_dev.lock);
    if(scaler_dev.suspended)
    {
        seq_printf(s,"Scaler has been suspended!\n");
        mutex_unlock(&scaler_dev.lock);
        return 0;
    }
    
    seq_printf(s, "--------------------Src img--------------------\n");
    seq_printf(s, "  Addr        :  0x%08x\n", readl(scaler_dev.base + 7 * 4));
    val = readl(scaler_dev.base + 9 * 4);
    seq_printf(s, "  Width       :  %d\n", val & 0x1FFF);
    seq_printf(s, "  Height      :  %d\n", (val >> 16) & 0x1FFF);
    seq_printf(s, "  Stride      :  %d\n", readl(scaler_dev.base + 9 * 4) & 0xFFFF);
    val = readl(scaler_dev.base + 11 * 4);
    val1 = readl(scaler_dev.base + 12 * 4); 
    seq_printf(s, "  Crop rect   :  [%d %d %d %d]\n", 
        val & 0x1FFF, (val >> 16) & 0x1FFF, val1 & 0x1FFF, (val1 >> 16) & 0x1FFF);
    seq_printf(s, "--------------------Dst img--------------------\n");
    seq_printf(s, "  Addr        :  0x%08x\n", readl(scaler_dev.base + 26 * 4));
    val = readl(scaler_dev.base + 17 * 4);
    seq_printf(s, "  Width       :  %d\n", val & 0x1FFF);
    seq_printf(s, "  Height      :  %d\n", (val >> 16) & 0x1FFF);
    seq_printf(s, "  Stride      :  %d\n", readl(scaler_dev.base + 27 * 4) & 0x1FFFF);
    seq_printf(s, "--------------------Ctrl reg-------------------\n");
    val = readl(scaler_dev.base + 0);
    seq_printf(s, "  Ctrl        :  0x%08x(%s)\n", val, val & 0x1 ? "Batch mode" : "Single mode");
    val = readl(scaler_dev.base + 3 * 4);
    seq_printf(s, "  Batch num   :  start %d end %d\n", (val >> 16) & 0xFFFF, val & 0xFFFF);
    seq_printf(s, "  Batch addr  :  0x%08x\n", readl(scaler_dev.base + 1 * 4));
    seq_printf(s, "  Irq mask    :  0x%08x\n", readl(scaler_dev.base + 4 * 4));
    val = readl(scaler_dev.base + 10 * 4);
    seq_printf(s, "  Interp mode :  %s\n", (val >> 5) & 0x3 ? "Bilinear":"Bicubic");
    seq_printf(s, "  PreCrop     :  %s\n", val & 0x4 ? "Enable":"Disable");
    seq_printf(s, "  Knorm       :  %d\n", (val >> 16) & 0x1F);
    seq_printf(s, "  LUT addr    :  0x%08x\n", readl(scaler_dev.base + 28 * 4));
    seq_printf(s, "  BurstCtrl   :  0x%08x\n", readl(scaler_dev.base + 16 * 4));
    seq_printf(s, "  H_Div       :  %d\n", readl(scaler_dev.base + 18 * 4));
    dist = readl(scaler_dev.base + 19 * 4) << 32 | readl(scaler_dev.base + 20 * 4);
    seq_printf(s, "  H_Dist      :  %lld\n", dist);
    seq_printf(s, "  V_Div       :  %d\n", readl(scaler_dev.base + 22 * 4));
    dist = readl(scaler_dev.base + 23 * 4) << 32 | readl(scaler_dev.base + 24 * 4);    
    seq_printf(s, "  V_Dist      :  %lld\n", dist);
    seq_printf(s, "  CurrChannel :  %d\n", readl(scaler_dev.base + 71 * 4) );

    mutex_unlock(&scaler_dev.lock);

    return 0;
}

static int ar_scaler_proc_state_open(struct inode *inode, struct file *file)
{
	return single_open(file, ar_scaler_proc_state_show, inode->i_private);
}

static struct file_operations ar_scaler_proc_state_fops={
	.owner		= THIS_MODULE,
	.open		= ar_scaler_proc_state_open,
	.release	= single_release,
	.read		= seq_read,
};

static int ar_scaler_proc_create(void)
{
	struct proc_dir_entry * entry;

	scaler_dev.proc_dir = proc_mkdir("arscaler", NULL);
	if(!scaler_dev.proc_dir)
	{
		printk(KERN_ERR"Create proc dir failed.\n");
		return -ENOMEM;
	}

	entry = proc_create("state", 0, scaler_dev.proc_dir, &ar_scaler_proc_state_fops);
	if(!entry)
	{
		printk(KERN_ERR"Create proc state failed.\n");
		return -ENOMEM;
	}

	return 0;
}

static int ar_scaler_proc_destroy(void)
{
    if(scaler_dev.proc_dir)
    {
        proc_remove(scaler_dev.proc_dir);
    }

    return 0;
}

static int ar_scaler_dev_open(struct inode *inode, struct file *file)
{
	//1. do nothing ?

	return 0;
}

static int ar_scaler_dev_close(struct inode *inode, struct file *file)
{
	//printk(KERN_INFO "Close is called.\n");
	return 0;
}

static ssize_t ar_scaler_dev_read(struct file *file, char __user *user, size_t len, loff_t *loff)
{
	//printk(KERN_INFO "Read is called.\n");
	return 0;
}

static int __maybe_unused ar_scaler_suspend(struct device *dev);
static int __maybe_unused ar_scaler_resume(struct device *dev);

static ssize_t ar_scaler_dev_write(struct file *file, const char __user *user, size_t len, loff_t *loff)
{
    char cmd[16] = {0};
    
    len = len > 16 ? 16 : len;
    
    if(copy_from_user(cmd, user, len))
    {
        return -EFAULT;
    }
    
    if(strncmp(cmd, "suspend", 7) == 0)
    {
        ar_scaler_suspend(NULL);
    }
    
    //Enable Auto Resume
    if(strncmp(cmd, "resume", 6) == 0)
    {
        ar_scaler_resume(NULL);
    }
        
    return len;
}

static long ar_scaler_dev_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int ret  = 0;
	unsigned int frequency = 0;
    ar_scaler_auto_st * p_auto_params;
    ar_scaler_params_st single_param;

    int size = _IOC_SIZE(cmd);
    switch(cmd)
    {
        case AR_SCALER_IOC_START:
            if(size != sizeof(ar_scaler_auto_st))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                return -EINVAL;
            }
            p_auto_params = kmalloc(size, GFP_KERNEL);
            if(!p_auto_params)
            {
                printk(KERN_ERR"Kmalloc failed.\n");
                return -ENOMEM;
            }
            memset(p_auto_params, 0, size);
            ret = copy_from_user(p_auto_params,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                kfree(p_auto_params);
                return ret;
            }

            ret = ar_scaler_start_batch(&scaler_dev, p_auto_params);
            kfree(p_auto_params);
        break;

        case AR_SCALER_IOC_START_SINGLE:
            if(size != sizeof(ar_scaler_params_st))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                return -EINVAL;
            }
            ret = copy_from_user(&single_param,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                return ret;
            }

            ret = ar_scaler_start_single(&scaler_dev, &single_param);
        break;
	    case AR_SCALER_IOC_SET_FREQUENCY:
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
            ret = ar_scaler_set_frequency(frequency);
            break;
        default:
        printk(KERN_ERR"No this ioctl: %d.\n", cmd);
    }

    return ret;
}

static struct file_operations ar_scaler_fops = {
	.owner		= THIS_MODULE,
	.open		= ar_scaler_dev_open,
	.release	= ar_scaler_dev_close,
	.read		= ar_scaler_dev_read,
	.write		= ar_scaler_dev_write,
	.unlocked_ioctl = ar_scaler_dev_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl	= ar_scaler_dev_ioctl,
#endif
};

static int ar_scaler_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct resource *res = NULL;
    unsigned int frequency = 0;
    unsigned int control = 0;

#if defined(CONFIG_ION) 
#else
	struct mmb_info mi;
#endif
    printk("Register scaler driver 0...\n");

    if(!pdev)
    {
    	return -EINVAL;
    }

    printk("Register scaler driver...\n");
    memset(&scaler_dev, 0, sizeof(scaler_dev));

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(!res)
    {
    	return -EINVAL;
    }

    if((res->end - res->start + 1) < AR_SCALER_REG_SPACE_RANGE)
    {
    	dev_err(&pdev->dev, "I/O space is too small\n");
    	return -EINVAL;
    }

    scaler_dev.base = ioremap_nocache(res->start, AR_SCALER_REG_SPACE_RANGE);
    if(!scaler_dev.base)
    {
    	return -ENOMEM;
    }

	if (of_property_read_u32(pdev->dev.of_node, "control", &control))
	{
		control = AR_GLOBAL_CONTROL_BASE;
	}
	scaler_dev.global_ctrl = ioremap_nocache(control, AR_GLOBAL_CONTROL_RANGE);
	if(!scaler_dev.global_ctrl)
	{
		return -ENOMEM;
	}

	if (!of_property_read_u32(pdev->dev.of_node, "frequency", &frequency))
    {
    	scaler_dev.frequency = frequency;
    }
    else
    {
    	scaler_dev.frequency = 0;
    }


    scaler_dev.irq = platform_get_irq(pdev, 0);
    if(scaler_dev.irq < 0)
    {
    	printk(KERN_ERR"No IRQs\n");
    	goto fail;
    }

    printk("SCALER IRQ register.\n");

    ret = request_irq(scaler_dev.irq, ar_scaler_irq_handler, 0, "arscaler", &scaler_dev);
    if(ret < 0)
    {
    	printk(KERN_ERR"Request irq fail.\n");
    	goto fail;
    }

    printk("SCALER MISC register.\n");

    //2. register misc dev
    scaler_dev.miscdev.minor = MISC_DYNAMIC_MINOR;
    scaler_dev.miscdev.name="arscaler";
    scaler_dev.miscdev.fops = &ar_scaler_fops;

    ret = misc_register(&scaler_dev.miscdev);
    if(ret < 0)
    {
    	goto fail;
    }

#if defined(CONFIG_ION) 
    //malloc ion for lut and batch buffer
    ret = artosyn_client_create(AR_SCALER_ION_NAME);
    if (ret < 0)
    {
    	printk(KERN_ERR"creat ion failded");
    	goto fail;
    }

    printk("SCALER ION alloc.\n");

    scaler_dev.lut_ion_fd = artosyn_alloc_buffer(AR_SCALER_ION_NAME, ar_scaler_get_lut_size() + 512, 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0,
                                (void **)&scaler_dev.lut_va, (void **)&scaler_dev.lut_pa);
    if (scaler_dev.lut_ion_fd < 0 || !scaler_dev.lut_va || !scaler_dev.lut_pa)
    {
    	printk(KERN_ERR"artosyn_alloc_buffer ion failded");
    	goto fail;
    }

    scaler_dev.batch_buffer_fd = artosyn_alloc_buffer(AR_SCALER_ION_NAME, SCALER_CMD_BUFFER_SIZE_PER_CMD * AR_SCALER_MAX_BATCH * 4, 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0,
                                     (void **)&scaler_dev.batch_buffer_va, (void **)&scaler_dev.batch_buffer_pa);
    if (scaler_dev.batch_buffer_fd < 0 || !scaler_dev.batch_buffer_va || !scaler_dev.batch_buffer_pa)
    {
    	printk(KERN_ERR"artosyn_alloc_buffer ion failded");
    	goto fail;
    }
#else
// alloc buffer from mmz
   memset(&mi, 0, sizeof(mi));
   mi.flags = MAP_SHARED;
   mi.prot = PROT_READ | PROT_WRITE;
   mi.align = ALIGN16KB;

   //lut
   mi.size = ar_scaler_get_lut_size() + 512;//in_len;
   strncpy(mi.mmb_name, "scaler_lut_mmz", HIL_MMB_NAME_LEN - 1);
    scaler_dev.scaler_lut_mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
										 mi.gfp, mi.mmz_name, mi.order);
   if (!scaler_dev.scaler_lut_mmb)
   {
	   printk(KERN_ERR"alloc MMB faild");
	   return -ENOMEM;
   }
   scaler_dev.lut_pa = hil_mmb_phys(scaler_dev.scaler_lut_mmb);
   scaler_dev.lut_va = (uint64_t)hil_mmb_map2kern(scaler_dev.scaler_lut_mmb);
   if (!scaler_dev.lut_va)
   {
	   printk(KERN_ERR"map2kern failed");
	   hil_mmb_free(scaler_dev.scaler_lut_mmb);
	   return -ENOMEM;
   }

   //batch_buffer
   mi.size = SCALER_CMD_BUFFER_SIZE_PER_CMD * AR_SCALER_MAX_BATCH * 4;
   strncpy(mi.mmb_name, "batch_mmz", HIL_MMB_NAME_LEN - 1);
   scaler_dev.batch_buffer_mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
										 mi.gfp, mi.mmz_name, mi.order);
   if (!scaler_dev.batch_buffer_mmb)
   {
	   printk(KERN_ERR"alloc MMB faild");
	   hil_mmb_unmap(scaler_dev.scaler_lut_mmb);
	   hil_mmb_free(scaler_dev.scaler_lut_mmb);
	   return -ENOMEM;
   }
   scaler_dev.batch_buffer_pa = hil_mmb_phys(scaler_dev.batch_buffer_mmb);
   scaler_dev.batch_buffer_va = (uint64_t)hil_mmb_map2kern(scaler_dev.batch_buffer_mmb);
   if (!scaler_dev.batch_buffer_va)
   {
	   printk(KERN_ERR"map2kern failed");
	   hil_mmb_unmap(scaler_dev.scaler_lut_mmb);
	   hil_mmb_free(scaler_dev.scaler_lut_mmb);
	   hil_mmb_free(scaler_dev.batch_buffer_mmb);
	   return -ENOMEM;
   }
#endif


    printk("SCALER proc register.\n");

    ar_scaler_proc_create();

    ar_scaler_hw_init(&scaler_dev);

    init_completion(&scaler_dev.done);
    mutex_init(&scaler_dev.lock);

    printk("SCALER registered.\n");
    return 0;

fail:
    if(scaler_dev.base)
    {
    	iounmap(scaler_dev.base);
    	scaler_dev.base = NULL;
    }

	if(scaler_dev.global_ctrl)
    {
        iounmap(scaler_dev.global_ctrl);
        scaler_dev.global_ctrl = NULL;
    }

#if defined(CONFIG_ION) 

    if(scaler_dev.lut_ion_fd)
    {
        artosyn_free_buffer(AR_SCALER_ION_NAME, scaler_dev.lut_ion_fd);
    }

    if(scaler_dev.batch_buffer_fd)
    {
        artosyn_free_buffer(AR_SCALER_ION_NAME, scaler_dev.batch_buffer_fd);
        artosyn_destroy_client(AR_SCALER_ION_NAME);
    }
#else
	if(scaler_dev.scaler_lut_mmb)
	{
		hil_mmb_unmap(scaler_dev.scaler_lut_mmb);
		hil_mmb_free(scaler_dev.scaler_lut_mmb);
	}
	if(scaler_dev.batch_buffer_mmb)
	{
		hil_mmb_unmap(scaler_dev.batch_buffer_mmb);
		hil_mmb_free(scaler_dev.batch_buffer_mmb);
	}

#endif
    printk("Scaler register failed.\n");
    return ret;
}

static int ar_scaler_remove(struct platform_device *pdev)
{
    ar_scaler_poweroff(&scaler_dev);
    
    if(scaler_dev.base)
    {
    	iounmap(scaler_dev.base);
    	scaler_dev.base = NULL;
    }

    ar_scaler_proc_destroy();
    free_irq(scaler_dev.irq, &scaler_dev);
    misc_deregister(&scaler_dev.miscdev);

#if defined(CONFIG_ION) 

    if(scaler_dev.lut_ion_fd)
    {
        artosyn_free_buffer(AR_SCALER_ION_NAME, scaler_dev.lut_ion_fd);
    }

    if(scaler_dev.batch_buffer_fd)
    {
        artosyn_free_buffer(AR_SCALER_ION_NAME, scaler_dev.batch_buffer_fd);
        artosyn_destroy_client(AR_SCALER_ION_NAME);
    }

#else

   if(scaler_dev.scaler_lut_mmb)
   {
	   hil_mmb_unmap(scaler_dev.scaler_lut_mmb);
	   hil_mmb_free(scaler_dev.scaler_lut_mmb);
   }
   if(scaler_dev.batch_buffer_mmb)
   {
	   hil_mmb_unmap(scaler_dev.batch_buffer_mmb);
	   hil_mmb_free(scaler_dev.batch_buffer_mmb);
   }
#endif

    printk("SCALER module has been unloaded.\n");

    return 0;
}

//Only need to wait until the last task finishs
static int __maybe_unused ar_scaler_suspend(struct device *dev)
{
    mutex_lock(&scaler_dev.lock);
    if(scaler_dev.suspended)
    {
        printk(KERN_INFO"Scaler suspended already.\n");
        mutex_unlock(&scaler_dev.lock);
        return 0;
    }
    ar_scaler_poweroff(&scaler_dev);
    scaler_dev.suspended = 1;
    printk(KERN_INFO "Scaler suspended.\n");
    mutex_unlock(&scaler_dev.lock);
    
    return 0;
}

static int __maybe_unused ar_scaler_resume(struct device *dev)
{
    mutex_lock(&scaler_dev.lock);
    if(!scaler_dev.suspended)
    {
        printk(KERN_INFO"Scaler is alive, no need to resume.\n");
        mutex_unlock(&scaler_dev.lock);
        return 0;
    }
    ar_scaler_hw_init(&scaler_dev);
    printk(KERN_INFO "Scaler resumed.\n");
    scaler_dev.suspended = 0;
    mutex_unlock(&scaler_dev.lock);
    
    return 0;
}

static const struct dev_pm_ops ar_scaler_pm_ops = {
        .suspend        = ar_scaler_suspend,
        .resume         = ar_scaler_resume,
};

static const struct of_device_id ar_scaler_match[] = {
	{.compatible = "artosyn,scaler"},
	{},
};

MODULE_DEVICE_TABLE(of, ar_scaler_match);

static struct platform_driver ar_scaler_platform_driver = {
	.probe = ar_scaler_probe,
	.remove = ar_scaler_remove,
	.driver = {
		.name = "artosyn scaler",
		.of_match_table = ar_scaler_match,
		.pm = &ar_scaler_pm_ops,
	},
};

static int __init ar_scaler_dev_init(void)
{
    printk(KERN_ERR"Scaler init start...\n");
	return platform_driver_register(&ar_scaler_platform_driver);
}

static void __exit ar_scaler_dev_exit(void)
{
    printk(KERN_ERR"Scaler deinit start...\n");
	platform_driver_unregister(&ar_scaler_platform_driver);
}

static int scaler_debug_level = 1;
module_param(scaler_debug_level, int, S_IRUGO);
module_init(ar_scaler_dev_init);
module_exit(ar_scaler_dev_exit);

MODULE_AUTHOR("artosyn");
MODULE_DESCRIPTION("artosyn scaler driver");
MODULE_LICENSE("GPL v2");

