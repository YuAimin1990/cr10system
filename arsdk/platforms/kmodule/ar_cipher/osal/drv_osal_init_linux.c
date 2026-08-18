#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/of_device.h>
#include "drv_osal_lib.h"
#include "drv_symc.h"
//#include "drv_hash.h"
#include "hal_type.h"
#include "hal_comm_cipher.h"
#include "drv_osal_lib_linux.h"

/************************ Internal Structure Definition *********************/

#define CIPHER_PROC_NAME    "driver/ar_cipher"

extern AR_S32 crypto_ioctl(AR_U32 cmd, AR_VOID *argp);
extern AR_S32 crypto_entry(void);
extern AR_S32 crypto_exit(void);
extern AR_S32 crypto_release(void);

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      link*/
/** @{*/  /** <!-- [link]*/

/******* proc function begin ********/
#if (1 == AR_PROC_SUPPORT)
AR_S32 symc_proc_read(struct seq_file *p, AR_VOID *v)
{
    symc_chn_status *status = AR_NULL;
    int i = 0;
    AR_S32 ret = AR_SUCCESS;

    seq_printf(p, "\n------------------------------------------"
                  "CIPHER STATUS-------------------------------"
                  "--------------------------------------------"
                  "--------------------\n");
    seq_printf(p, "Chnid   Status   Decrypt   Alg   Mode   KeyLen    "
                  "Addr in/out      KeyFrom  INT-RAW in/out  INT-EN "
                  "in/out INT_OCNTCFG    IVOUT\n");

    status = (symc_chn_status *)crypto_malloc(8 * sizeof(symc_chn_status));
    if(AR_NULL == status)
    {
        return AR_FAILURE;
    }

    crypto_memset(status, 8 * sizeof(symc_chn_status), 0, 8 * sizeof(symc_chn_status));
    for(i = 0; i < 8; i++)
    {
        status[i].id = i;
    }

    ret = drv_symc_proc_status(status);
    if(AR_SUCCESS != ret)
    {
        seq_printf(p, "CIPHER_ProcGetStatus failed!\n");
        crypto_free(status);
        status = NULL;
        return AR_FAILURE;
    }

    for(i=0; i< CRYPTO_HARD_CHANNEL_MAX; i++)
    {
        seq_printf(p, " %d       %s      %d      %s  %s    %03d    %08x/%08x   "
                      " %s           %d/%d            %d/%d        %08x     %s\n",
                        i,
                        status[i].open,
                        status[i].decrypt,
                        status[i].alg,
                        status[i].mode,
                        status[i].klen,
                        status[i].inaddr,
                        status[i].outaddr,
                        status[i].ksrc,
                        status[i].inraw,
                        status[i].outraw,
                        status[i].inten,
                        status[i].outen,
                        status[i].outintcnt,
                        status[i].iv);
    }

    crypto_free(status);
    status = NULL;

    return AR_SUCCESS;
}

static int symc_proc_open(struct inode* inode, struct file* file)
{
    return single_open(file, symc_proc_read, NULL);
}

static const struct file_operations DRV_CIPHER_ProcFops =
{
    .owner		= THIS_MODULE,
    .open		= symc_proc_open,
    .read		= seq_read,
    .llseek		= seq_lseek,
    .release	= single_release,
};

static AR_VOID symc_proc_init(AR_VOID)
{
    struct proc_dir_entry *pstProcEntry = AR_NULL;

    pstProcEntry = proc_create(CIPHER_PROC_NAME, 0, NULL, &DRV_CIPHER_ProcFops);
    if (NULL == pstProcEntry)
    {
        AR_LOG_ERROR("cipher: can't create %s.\n", CIPHER_PROC_NAME);
    }
}

static AR_VOID symc_proc_deinit(AR_VOID)
{
    remove_proc_entry(CIPHER_PROC_NAME, NULL);
}
#endif
/******* proc function end ********/


AR_S32 _crypto_ioctl(struct inode *inode, struct file *file,
                       AR_U32 cmd, AR_VOID *arg)
{
    return crypto_ioctl(cmd, arg);
}

static s32 hi_cipher_open(struct inode * inode, struct file * file)
{
    if ((!capable(CAP_SYS_RAWIO)) || (!capable(CAP_SYS_ADMIN)))
        return -EPERM;
    return AR_SUCCESS;
}

static long hi_cipher_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    long ret;
    AR_U8 unCmdParam[256] = {0};

    if ((AR_NULL == ffile) || (AR_NULL == ffile->f_path.dentry) || ( 0x00 == arg))
    {
        AR_LOG_ERROR("Invalid cmd param size!\n");
        return AR_ERR_CIPHER_INVALID_POINT;
    }

    if(_IOC_SIZE(cmd) > sizeof(unCmdParam))
    {
        AR_LOG_ERROR("Invalid cmd param size %d!\n", _IOC_SIZE(cmd));
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    if (copy_from_user((AR_VOID*)unCmdParam, (void __user *)arg, _IOC_SIZE(cmd)))
    {
        AR_LOG_ERROR("copy data from user fail!\n");
        return AR_FAILURE;
    }

    ret = _crypto_ioctl(ffile->f_path.dentry->d_inode, ffile, cmd, (AR_VOID*)unCmdParam);

    if (copy_to_user((void __user *)arg, (const AR_VOID*)unCmdParam, _IOC_SIZE(cmd)))
    {
        AR_LOG_ERROR("copy data to user fail!\n");
        return AR_FAILURE;
    }

    return ret;
}

void *crypto_calloc(size_t n, size_t size)
{
    void *ptr = AR_NULL;
    ptr = crypto_malloc(n * size);
    if (AR_NULL != ptr)
    {
        crypto_memset(ptr, n * size, 0, n * size);
    }
    return ptr;
}

static s32 hi_cipher_release(struct inode * inode, struct file * file)
{
    return crypto_release();
}

static struct file_operations dev_cipher_fops=
{
    .owner            = THIS_MODULE,
    .open             = hi_cipher_open,
    .unlocked_ioctl   = hi_cipher_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl     = hi_cipher_ioctl,
#endif
    .release          = hi_cipher_release,
};

static struct miscdevice cipher_dev =
{
    .minor      = MISC_DYNAMIC_MINOR,
    .name       = UMAP_DEVNAME_CIPHER,
    .fops       = &dev_cipher_fops,
};

void *cipher_get_device(void)
{
    return (void *)cipher_dev.this_device;
}

AR_S32 CIPHER_DRV_ModInit(AR_VOID)
{
    AR_S32 ret = AR_SUCCESS;

    if (misc_register(&cipher_dev))
    {
        AR_LOG_ERROR("ERROR: could not register cipher devices\n");
        return -1;
    }

    /* dma data structure shall be initialised before being used in Kernel 4.9
     * or else call dma_set_coherent_mask/dma_alloc_coherent will return error
     */
    of_dma_configure(cipher_dev.this_device, cipher_dev.this_device->of_node);

    ret = crypto_entry();
    if (AR_SUCCESS != ret)
    {
        misc_deregister(&cipher_dev);
        return AR_FAILURE;
    }

/******* proc function begin ********/
#if (1 == AR_PROC_SUPPORT)
    symc_proc_init();
#endif
/******* proc function end ********/

//#ifdef MODULE
    AR_PRINT("Load ar_cipher.ko success.\n");
//#endif

    return AR_SUCCESS;
}

AR_VOID CIPHER_DRV_ModExit(AR_VOID)
{
/******* proc function begin ********/
#if (1 == AR_PROC_SUPPORT)
    symc_proc_deinit();
#endif
/******* proc function end ********/
    misc_deregister(&cipher_dev);
    crypto_exit();

    return ;
}

/** @}*/  /** <!-- ==== Structure Definition end ====*/
