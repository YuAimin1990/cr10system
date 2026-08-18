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
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/poll.h>

#include "ar_npu.h"
#include "ar_npu_hal.h"

#define AR_NPU_DEFAULT_DMA_CTRL   0xF //ch 3 sec noncachable
#define AR_NPU_DEFAULT_SEC_CTRL   1 //default 1 sec mode
#define AR_NPU_DEFAULT_CB_DSP     0
#define AR_NPU_DEFAULT_CB_ARM     1
//#define NPU_WAIT_CB_DEFAULT_TIMEOUT  (1000 * HZ)


static int npu_debug_level = 1;
int npu_sram_size = NPU_SRAM_DEF_SIZE;

struct ar_npu_dev npu_dev;
struct task_struct * g_normal_thread;
struct task_struct * g_high_thread;

static int ar_npu_dev_open(struct inode *inode, struct file *file)
{
    //1. do nothing ?
    //printk(KERN_INFO "Open is called.\n");

    return 0;
}

static int ar_npu_dev_close(struct inode *inode, struct file *file)
{
    //printk(KERN_INFO "Close is called.\n");
    return 0;
}

static ssize_t ar_npu_dev_read(struct file *file, char __user *user, size_t len, loff_t *loff)
{
    //printk(KERN_INFO "Read is called.\n");
    return 0;
}

static int __maybe_unused ar_npu_suspend(struct device *dev);
static int __maybe_unused ar_npu_resume(struct device *dev);

//Only for test commands: reset
static ssize_t ar_npu_dev_write(struct file *file, const char __user *user, size_t len, loff_t *loff)
{
    char cmd[16] = {0};

    len = len > 16 ? 16 : len;

    if(copy_from_user(cmd, user, len))
    {
        return -EFAULT;
    }

    if(strncmp(cmd, "reset", 5) == 0)
    {
        _ar_npu_stop_and_reset();
    }

    //Enable Auto Resume
    if(strncmp(cmd, "ear", 3) == 0)
    {
        //todo
    }

    //Disable Auto Resume
    if(strncmp(cmd, "dar", 4) == 0)
    {
        //todo
    }

    if(strncmp(cmd, "suspend", 7) == 0)
    {
        ar_npu_suspend(NULL);
    }
    
    if(strncmp(cmd, "resume", 6) == 0)
    {
        ar_npu_resume(NULL);
    }

    return len;
}

typedef union
{
    AR_NPU_IOCTL_CFG_S cfg;
    AR_NPU_IOCTL_HANG_S hang;
    //AR_NPU_IOCTL_RESUME_S resume;
    AR_NPU_IOCTL_STATUS_S status;
    AR_NPU_IOCTL_CB_S     cb;
    AR_NPU_IOCTL_CB_DONE_S cb_done;
    AR_NPU_IOCTL_DEBUG_S  debug;
    AR_NPU_IOCTL_USAGE_S  usage;
    AR_NPU_IOCTL_IO_ADDR_S io_addr;
	AR_NPU_IOCTL_CBUF_ADDR_S cbuf_addr;
} AR_NPU_IOCTL_U;

static long ar_npu_dev_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int ret  = 0;
    unsigned short network_id = 0;
    unsigned int security = 0;
    unsigned int frequency = 0;
    unsigned int cache_enable = 0;
	unsigned int timeout = 0;
	int soc_version = -1;
    struct timespec npu_time;

    AR_NPU_IOCTL_U * p_uniParam = kmalloc(sizeof(AR_NPU_IOCTL_U), GFP_KERNEL);

    int size = _IOC_SIZE(cmd);

    switch(cmd)
    {
        case NPU_IOC_START:
            if(size != sizeof(AR_NPU_IOCTL_CFG_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&p_uniParam->cfg,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_start(&p_uniParam->cfg);
            break;
        case NPU_IOC_HANG:
            if(size != sizeof(AR_NPU_IOCTL_HANG_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
            }

            ret = copy_from_user(&p_uniParam->hang,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_hang(&p_uniParam->hang);
            if(ret == 0)
            {
                ret = copy_to_user((char __user *)args, &p_uniParam->hang, size);
            }
            break;
#if 0
       case NPU_IOC_RESUME:
        if(size != sizeof(AR_NPU_IOCTL_RESUME_S))
        {
           printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
           return -EINVAL;
        }

        ret = copy_from_user(&resume,(char __user *)args, size);
        if(ret)
        {
          printk(KERN_ERR"copy ioctl args error.\n");
          return ret;
        }

        ret = _ar_npu_resume(&resume);
        break;
#endif
        case NPU_IOC_QUERY:
            if(size != sizeof(AR_NPU_IOCTL_STATUS_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&p_uniParam->status,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_query(&p_uniParam->status);
            if(ret == 0)
            {
                ret = copy_to_user((char __user *)args, &p_uniParam->status, size);
                if(ret)
                {
                    printk("Query: copy to user left %d, size %d\n", ret, size);
                    goto fail;
                }
            }
            break;
        case NPU_IOC_CB_REG:
            if(size != sizeof(AR_NPU_IOCTL_CB_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&p_uniParam->cb,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_register_cb(&p_uniParam->cb);
            break;
        case NPU_IOC_DSP_CB_REG:
            if(size != sizeof(AR_NPU_IOCTL_CB_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&p_uniParam->cb,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_register_dsp_cb(&p_uniParam->cb);
            break;
        case NPU_IOC_CB_DONE:
            if(size != sizeof(AR_NPU_IOCTL_CB_DONE_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
            }

            ret = copy_from_user(&p_uniParam->cb_done,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_cb_done(&p_uniParam->cb_done);
            break;
        case NPU_IOC_DBG_CONTINUE:
            if(size != sizeof(AR_NPU_IOCTL_DEBUG_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&p_uniParam->debug,(char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }

            ret = _ar_npu_debug_continue(&p_uniParam->debug);
            break;
        case NPU_IOC_SW_RESET:
            ret = _ar_npu_sw_reset();
            break;
        case NPU_IOC_CLEAN_RESOURCE:
            if(size != sizeof(unsigned short))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&network_id, (char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }
            ret = _ar_npu_clean_resource(network_id);
            break;
        case NPU_IOC_SET_SECURITY:
            if(size != sizeof(unsigned int))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&security, (char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }
            ret = _ar_npu_set_security(security);
            break;
        case NPU_IOC_SET_FREQUENCY:
            if(size != sizeof(unsigned int))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&frequency, (char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }
            ret = _ar_npu_set_frequency(frequency);
            break;
        case NPU_IOC_QUERY_USAGE:
            if(size != sizeof(AR_NPU_IOCTL_USAGE_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ktime_get_real_ts64(&npu_time);
            p_uniParam->usage.u64TimeTotal = (npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000) - npu_dev.time_start;
            p_uniParam->usage.u64TimeRunning = npu_dev.time_running;
            printk("usage.u64TimeTotal: %lld, usage.u64TimeRunning = %lld\n", p_uniParam->usage.u64TimeTotal ,p_uniParam->usage.u64TimeRunning);
            ret = copy_to_user((char __user *)args, &p_uniParam->usage, size);
            break;
        case NPU_IOC_GET_IO_ADDR:
            if(size != sizeof(AR_NPU_IOCTL_IO_ADDR_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            _ar_npu_get_io_addr(&p_uniParam->io_addr);
            ret = copy_to_user((char __user *)args, &p_uniParam->io_addr, size);
            break;
		case NPU_IOC_GET_CBUF_ADDR:
            if(size != sizeof(AR_NPU_IOCTL_CBUF_ADDR_S))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            _ar_npu_get_cbuf_addr(&p_uniParam->cbuf_addr);
            ret = copy_to_user((char __user *)args, &p_uniParam->cbuf_addr, size);
            break;
        case NPU_IOC_ALLOC_NET_ID:
            ret = _ar_npu_alloc_net_id(&network_id);
            if(!ret)
            {
                ret = copy_to_user((char __user *)args, &network_id, size);
            }
            break;
        case NPU_IOC_SET_NET_ID:
            ret = copy_from_user(&network_id, (char __user *)args, size);
			if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }
            ret = _ar_npu_set_net_id(network_id);
            break;
        case NPU_IOC_SET_CACHE_COHERENCY:
            if(size != sizeof(unsigned int))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&cache_enable, (char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }
            ret = _ar_npu_set_cache_coherency(cache_enable);
            break;
		case NPU_IOC_GET_SOC_VERSION:
            if(size != sizeof(int))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }
            soc_version = AR_SOC_VERSION;
            ret = copy_to_user((char __user *)args, &soc_version, size);
            break;
		case NPU_IOC_SET_TIMEOUT:
            if(size != sizeof(unsigned int))
            {
                printk(KERN_ERR"ioctl cmd %d param mismatch.\n", cmd);
                ret = -EINVAL;
                goto fail;
            }

            ret = copy_from_user(&timeout, (char __user *)args, size);
            if(ret)
            {
                printk(KERN_ERR"copy ioctl args error.\n");
                goto fail;
            }
            ret = _ar_npu_set_timeout(timeout);
            break;
        default:
            printk(KERN_ERR"NPU: No this ioctl: %x.\n", cmd);
            break;
    }

fail:
    kfree(p_uniParam);
    return ret;
}

static unsigned int ar_npu_dev_poll(struct file * file, struct poll_table_struct * poll_table)
{
    unsigned int mask = 0;
    poll_wait(file, &npu_dev.wait_queue, poll_table);
    //debug process cannot be interrupted by other network, otherwise it's hard to go back.
    //if(_ar_npu_is_pausing_or_waitingcb())
    if(npu_dev.cb_come || npu_dev.debug_come)
    {
        mask |= POLLIN | POLLRDNORM;
        //printk("Poll npu is pausing or waiting cb\n");
    }
    return mask;
}

static int ar_npu_proc_state_open(struct inode *inode, struct file *file)
{
    return single_open(file, ar_npu_proc_state_show, inode->i_private);
}

static struct file_operations ar_npu_proc_state_fops={
    .owner       = THIS_MODULE,
    .open        = ar_npu_proc_state_open,
    .release     = single_release,
    .read        = seq_read,
};
static int ar_npu_proc_fifo_open(struct inode *inode, struct file *file)
{
    return single_open(file, ar_npu_proc_fifo_show, inode->i_private);
}

static struct file_operations ar_npu_proc_fifo_fops={
    .owner       = THIS_MODULE,
    .open        = ar_npu_proc_fifo_open,
    .release     = single_release,
    .read        = seq_read,
};
static int ar_npu_proc_result_open(struct inode *inode, struct file *file)
{
    return single_open(file, ar_npu_proc_result_show, inode->i_private);
}

static struct file_operations ar_npu_proc_result_fops={
    .owner       = THIS_MODULE,
    .open        = ar_npu_proc_result_open,
    .release     = single_release,
    .read        = seq_read,
};

static int ar_npu_proc_usage_open(struct inode *inode, struct file *file)
{
    return single_open(file, ar_npu_proc_usage_show, inode->i_private);
}

static struct file_operations ar_npu_proc_usage_fops={
    .owner       = THIS_MODULE,
    .open        = ar_npu_proc_usage_open,
    .release     = single_release,
    .read        = seq_read,
};

static int ar_npu_proc_create(void)
{
    struct proc_dir_entry * entry;

    npu_dev.proc_dir = proc_mkdir("arnpu", NULL);
    if(!npu_dev.proc_dir)
    {
        printk(KERN_ERR"Create proc dir failed.\n");
        return -ENOMEM;
    }

    entry = proc_create("state", 0, npu_dev.proc_dir, &ar_npu_proc_state_fops);
    if(!entry)
    {
        printk(KERN_ERR"Create proc state failed.\n");
        return -ENOMEM;
    }

    entry = proc_create("fifo", 0, npu_dev.proc_dir, &ar_npu_proc_fifo_fops);
    if(!entry)
    {
        printk(KERN_ERR"Create proc fifo failed.\n");
        return -ENOMEM;
    }

    entry = proc_create("result", 0, npu_dev.proc_dir, &ar_npu_proc_result_fops);
    if(!entry)
    {
        printk(KERN_ERR"Create proc result failed.\n");
        return -ENOMEM;
    }

    entry = proc_create("usage", 0, npu_dev.proc_dir, &ar_npu_proc_usage_fops);
    if(!entry)
    {
        printk(KERN_ERR"Create proc usage failed.\n");
        return -ENOMEM;
    }

    return 0;
}

static int ar_npu_proc_destroy(void)
{
    //this will remove all the sub tree.
    if(npu_dev.proc_dir)
    {
        proc_remove(npu_dev.proc_dir);
    }

    return 0;
}

static struct file_operations ar_npu_fops={
    .owner      = THIS_MODULE,
    .open       = ar_npu_dev_open,
    .release    = ar_npu_dev_close,
    .read       = ar_npu_dev_read,
    .write      = ar_npu_dev_write,
    .unlocked_ioctl = ar_npu_dev_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl	= ar_npu_dev_ioctl,
#endif
    .poll       = ar_npu_dev_poll,
};

static int ar_npu_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct resource *res = NULL;
    unsigned int frequency = 0;
    unsigned int dma_ctrl = 0;
    unsigned int npu_sec_ctrl = 0;
    unsigned int cb_to_arm = 0;
    unsigned int control = 0;

    npu_sram_size = npu_sram_size > (NPU_SRAM_DEF_SIZE + NPU_SRAM_CEVA_SIZE) ? \
    (NPU_SRAM_DEF_SIZE + NPU_SRAM_CEVA_SIZE):npu_sram_size;

    struct sched_param normal_priority, high_priority;
    struct timespec time_start;
    if(!pdev)
    {
        return -EINVAL;
    }

    memset(&npu_dev, 0, sizeof(npu_dev));
    printk("=NPU device driver:\n");
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(!res)
    {
        return -EINVAL;
    }

    if((res->end - res->start + 1) < NPU_REG_SPACE_RANGE) {
        dev_err(&pdev->dev, "I/O space is too small\n");
        return -EINVAL;
    }

    npu_dev.base = ioremap_nocache(res->start, NPU_REG_SPACE_RANGE);
    if(!npu_dev.base)
    {
        return -ENOMEM;
    }

    if (of_property_read_u32(pdev->dev.of_node, "control", &control))
    {
        control = AR_GLOBAL_CONTROL_BASE;
    }

    npu_dev.global_ctrl = ioremap_nocache(control, AR_GLOBAL_CONTROL_RANGE);
    if(!npu_dev.global_ctrl)
    {
        goto fail;
    }

    if (!of_property_read_u32(pdev->dev.of_node, "frequency", &frequency))
    {
        npu_dev.frequency = frequency;
    }
    else
    {
        npu_dev.frequency = 0;
    }

    if (!of_property_read_u32(pdev->dev.of_node, "dma_control", &dma_ctrl))
    {
        npu_dev.dma_ctrl = dma_ctrl;
    }
    else
    {
        npu_dev.dma_ctrl = AR_NPU_DEFAULT_DMA_CTRL;//ch 0 nonsec no cache
    }

    if (!of_property_read_u32(pdev->dev.of_node, "npu_sec_control", &npu_sec_ctrl))
    {
        npu_dev.npu_sec_ctrl = npu_sec_ctrl;
    }
    else
    {
        npu_dev.npu_sec_ctrl = AR_NPU_DEFAULT_SEC_CTRL;//default  1 sec
    }

    if (!of_property_read_u32(pdev->dev.of_node, "cb_to_arm", &cb_to_arm))
    {
        npu_dev.cb_to_arm = cb_to_arm;
    }
    else
    {
        npu_dev.cb_to_arm = AR_NPU_DEFAULT_CB_DSP;
    }

    npu_dev.irq = platform_get_irq(pdev, 0);
    if(npu_dev.irq < 0)
    {
        printk(KERN_ERR"No IRQs\n");
        goto fail;
    }

    printk("=base: %p %llx\n", npu_dev.base, res->start);
    printk("=ctrl: %p %x\n", npu_dev.global_ctrl, control);
    printk("=sec_ctrl: %d\n", npu_dev.npu_sec_ctrl);
    printk("=dma_ctrl: %d\n", npu_dev.dma_ctrl);
    printk("=freq: %d\n", npu_dev.frequency);
    printk("=cb_to_arm: %d\n", npu_dev.cb_to_arm);
    printk("=irq: %d\n", npu_dev.irq);

    ret = request_irq(npu_dev.irq, _ar_npu_irq_handler, 0, "arnpu", &npu_dev);
    if(ret < 0)
    {
        printk(KERN_ERR"Request irq fail.\n");
        goto fail;
    }

    g_normal_thread = kthread_run(_ar_npu_normal_thread, &npu_dev, "npu_normal_thread");
    g_high_thread = kthread_run(_ar_npu_high_thread, &npu_dev, "npu_high_thread");
    if(!g_normal_thread || !g_high_thread)
    {
        printk(KERN_ERR"Create NPU thread error!\n");
        goto fail;
    }

    //Set priority, 1 min, 99 max.
    normal_priority.sched_priority = 5;
    high_priority.sched_priority = 1;
    sched_setscheduler(g_normal_thread, SCHED_FIFO, &normal_priority);
    sched_setscheduler(g_high_thread, SCHED_FIFO, &high_priority);

    init_waitqueue_head(&npu_dev.wait_queue);
    init_completion(&npu_dev.idle_wait);
#ifdef PROXIMA
    complete(&npu_dev.idle_wait);
#endif
    _ar_npu_status_init();

    npu_dev.cb_come =0;
    npu_dev.debug_come = 0;

    ktime_get_real_ts64(&time_start);
    npu_dev.time_start = time_start.tv_sec * 100000 + time_start.tv_nsec/10000;
    npu_dev.time_running = 0;
    npu_dev.time_tic = time_start.tv_sec * 100000 + time_start.tv_nsec/10000;
    npu_dev.time_toc = time_start.tv_sec * 100000 + time_start.tv_nsec/10000;

     ar_npu_proc_create();
     //Init DLA global registers, like PLL, power ctrl
    ret = _ar_npu_poweron();
	if(ret)
	{
		printk(KERN_ERR"Poweron NPU error!\n");
        goto fail;
	}

    //2. register misc dev
    npu_dev.miscdev.minor = MISC_DYNAMIC_MINOR;
    npu_dev.miscdev.name = "arnpu";
    npu_dev.miscdev.fops = &ar_npu_fops;

    ret = misc_register(&npu_dev.miscdev);
    if(ret < 0)
    {
        goto fail;
    }

    mutex_init(&npu_dev.lock);

    return 0;

fail:
    if(npu_dev.base)
    {
        iounmap(npu_dev.base);
        npu_dev.base = NULL;
    }

    if(npu_dev.global_ctrl)
    {
        iounmap(npu_dev.global_ctrl);
        npu_dev.global_ctrl = NULL;
    }

    if(g_normal_thread)
    {
        kthread_stop(g_normal_thread);
    }
    if(g_high_thread)
    {
        kthread_stop(g_high_thread);
    }

    return ret;
}

static int ar_npu_remove(struct platform_device *pdev)
{
    _ar_npu_poweroff();

    if(npu_dev.base)
    {
        iounmap(npu_dev.base);
        npu_dev.base = NULL;
    }

    if(npu_dev.global_ctrl)
    {
        iounmap(npu_dev.global_ctrl);
        npu_dev.global_ctrl = NULL;
    }

    printk("Stopping NPU threads...\n");

    if(g_normal_thread)
    {
        kthread_stop(g_normal_thread);
    }
    if(g_high_thread)
    {
        kthread_stop(g_high_thread);
    }

    ar_npu_proc_destroy();
    free_irq(npu_dev.irq, &npu_dev);
    misc_deregister(&npu_dev.miscdev);

    printk("NPU module has been unloaded.\n");

    return 0;
}

//Only need to wait until the last task finishs
static int __maybe_unused ar_npu_suspend(struct device *dev)
{
    unsigned int status = 0;
    unsigned int timeout = 0;
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        printk(KERN_INFO"NPU suspended already.\n");
        mutex_unlock(&npu_dev.lock);
        return 0;
    }
    
    //check fifo is empty(no need) and npu is idle    
    while(1)
    {
        status = readl(npu_dev.base + NPU_IRQ_STATUS_REG);
        /*kfifo_is_empty(&g_npu_normal_fifo) && kfifo_is_empty(&g_npu_high_fifo)*/
        if(NPU_CURRENT_STATE_IDLE == ( ( status >> 28 ) & 0x0F ))
        {
            break;
        }

        msleep(5);   
        if(timeout++ > 1000) //5s
        {
            mutex_unlock(&npu_dev.lock);
            
            printk(KERN_INFO "NPU suspend timeout.\n");
            return -ETIME;
        }
    }

    _ar_npu_poweroff();

    npu_dev.suspended = 1;
    printk(KERN_INFO "NPU suspended.\n");
    mutex_unlock(&npu_dev.lock);
    
    return 0;
}

static int __maybe_unused ar_npu_resume(struct device *dev)
{
    mutex_lock(&npu_dev.lock);
    if(!npu_dev.suspended)
    {
        printk(KERN_INFO"NPU is alive, no need to resume.\n");
        mutex_unlock(&npu_dev.lock);
        return 0;
    }
    
    _ar_npu_poweron();
    npu_dev.suspended = 0;
    printk(KERN_INFO "NPU resumed.\n");
    mutex_unlock(&npu_dev.lock);
    
    return 0;
}

static const struct dev_pm_ops ar_npu_pm_ops = {
        .suspend        = ar_npu_suspend,
        .resume         = ar_npu_resume,
};

static const struct of_device_id ar_npu_match[] = {
    {.compatible = "artosyn,npu"},
    {},
};

MODULE_DEVICE_TABLE(of, ar_npu_match);

static struct platform_driver ar_npu_platform_driver = {
    .probe = ar_npu_probe,
    .remove = ar_npu_remove,
    .driver = {
        .name = "artosyn npu",
        .of_match_table = ar_npu_match,
        .pm = &ar_npu_pm_ops,
    },
};


static int __init ar_npu_dev_init(void)
{
    return platform_driver_register(&ar_npu_platform_driver);
}

static void __exit ar_npu_dev_exit(void)
{
    platform_driver_unregister(&ar_npu_platform_driver);
}

module_param(npu_debug_level, int, S_IRUGO);
module_param(npu_sram_size, int, S_IRUGO);

module_init(ar_npu_dev_init);
module_exit(ar_npu_dev_exit);

MODULE_AUTHOR("artosyn");
MODULE_DESCRIPTION("ar NPU driver");
MODULE_LICENSE("GPL v2");

