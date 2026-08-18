#ifndef __DRV_OSAL_LIB_LINUX_H__
#define __DRV_OSAL_LIB_LINUX_H__
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
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/page.h>
#include "hal_type.h"
#include "drv_osal_chip.h"
#include "drv_cipher_kapi.h"

#define AR_HANDLE_MAKEHANDLE(mod, privatedata, chnid)  (AR_HANDLE)( (((mod)& 0xffff) << 16) | ((((privatedata)& 0xff) << 8) ) | (((chnid) & 0xff)) )
#define AR_HANDLE_GET_MODID(handle)     (((handle) >> 16) & 0xffff)
#define AR_HANDLE_GET_PriDATA(handle)   (((handle) >> 8) & 0xff)
#define AR_HANDLE_GET_CHNID(handle)     (((handle)) & 0xff)

#define crypto_memset(s, sn, c, n)    { if (sn >= n) memset(s, c, n); else AR_LOG_ERROR("error, memset overflow\n");}
#define crypto_memcpy(s, sn, c, n)    { if (sn >= n) memcpy(s, c, n); else AR_LOG_ERROR("error, memcpy overflow\n");}

#define crypto_ioremap_nocache(addr, size)  ioremap_nocache(addr, size)
#define crypto_iounmap(addr, size)          iounmap(addr)

#define crypto_read(addr)         readl(addr)
#define crypto_write(addr, val)   writel(val, addr)

#define crypto_msleep(msec)         msleep(msec)
#define crypto_udelay(msec)         udelay(msec)

#define crypto_malloc(x)          (0 < (x) ? kzalloc((x), GFP_KERNEL) : AR_NULL)
#define crypto_free(x)            {if (AR_NULL != (x) ) kfree((x));}

#define crypto_copy_from_user(to, from, n) copy_from_user(to, from, n)
#define crypto_copy_to_user(to, from, n) copy_to_user(to, from, n)

void *crypto_calloc(size_t n, size_t size);


AR_U32 get_rand(void);

#define crypto_queue_head                          wait_queue_head_t
#define crypto_queue_init(x)                       init_waitqueue_head(x)
#define crypto_queue_wait_up(x)                    wake_up_interruptible(x)

/*
* Returns of crypto_queue_wait_timeout:
* 0 if the @condition evaluated to %false after the @timeout elapsed,
* 1 if the @condition evaluated to %true after the @timeout elapsed,
* the remaining jiffies (at least 1) if the @condition evaluated
* to %true before the @timeout elapsed, or -%ERESTARTSYS if it was
* interrupted by a signal.
*/
#define crypto_queue_wait_timeout(head, con, time) wait_event_interruptible_timeout(head, *(con), time)

#define crypto_request_irq(irq, func, name) request_irq(irq, func, IRQF_SHARED, name, (void*)name)
#define crypto_free_irq(irq, name)          free_irq(irq, (void*)name)

typedef struct semaphore                     crypto_mutex;
#define crypto_mutex_init(x)                 sema_init(x, 1)
#define crypto_mutex_lock(x)                 down_interruptible(x)
#define crypto_mutex_unlock(x)               up(x)
#define crypto_mutex_destroy(x)

#define crypto_owner                         pid_t
#define crypto_get_owner(x)                  *x = task_tgid_nr(current)

#define LOG_DEBUG 0
#define LOG_INFO 0


#define AR_PRINT(fmt...)                     printk(fmt)
#define AR_LOG_FATAL(fmt...) \
    do{ \
        printk("[FATAL-AR_CIPHER]:%s[%d]:",(AR_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
        printk("\n"); \
    }while(0)
#define AR_LOG_ERROR(fmt...) \
    do{ \
        printk("[ERROR-AR_CIPHER]:%s[%d]:",(AR_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
        printk("\n"); \
    }while(0)

#define AR_LOG_WARN(fmt...) \
    do{ \
        printk("[WARN-AR_CIPHER]:%s[%d]:",(AR_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
    }while(0)

#if LOG_INFO
#define AR_LOG_INFO(fmt...) \
    do{ \
        printk("[INFO-AR_CIPHER]:%s[%d]:\n",(AR_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
    }while(0)
#else
#define AR_LOG_INFO(fmt...) \
		do{ \
		}while(0)
#endif

#if LOG_DEBUG
#define AR_LOG_DEBUG(fmt...) \
    do{ \
        printk("[DEBUG-AR_CIPHER]:%s[%d]:",(AR_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
    }while(0)
#else
#define AR_LOG_DEBUG(fmt...) \
	do{ \
	}while(0)
#endif

#endif  /* End of #ifndef __DRV_OSAL_LIB_LINUX_H__*/
