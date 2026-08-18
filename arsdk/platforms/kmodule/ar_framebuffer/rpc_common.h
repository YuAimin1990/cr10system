#ifndef __RPC_COMMON_H__

#define __RPC_COMMON_H__
#include <linux/sched.h>

//#define ENABALE_ALL_LOG
#ifndef ENABALE_ALL_LOG
#define DISABLE_CND_LOG
#define DISABLE_FUN_LOG
#define DISABLE_INFO_LOG
#define DISABLE_PRT_LOG
#define DISABLE_DBG_LOG
#define DISABLE_WARNING_LOG
#endif

#define  dbg_printf printk
#define  log_level  KERN_EMERG
#define  get_task_id()  ((unsigned long)current->pid)

#ifndef DISABLE_ERR_LOG
#define ar_err(fmt,...) do{\
    dbg_printf(log_level"[%lx][ERR] %s "fmt"\n",get_task_id(),__FUNCTION__,##__VA_ARGS__);\
    }while(0)
#else
#define ar_err(fmt,...) do{}while(0)
#endif
#ifndef DISABLE_CND_LOG
#define ar_conditon(fmt, ...) do{\
    dbg_printf(log_level"[%lx][CND] %s "fmt"\n",get_task_id(),__FUNCTION__,##__VA_ARGS__);\
    }while(0)
#else
#define ar_conditon(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_FUN_LOG
#define ar_func_enter() do{\
    dbg_printf(log_level"[%lx][FUNC] %s enter %d\n",get_task_id(),__FUNCTION__,__LINE__);\
    }while(0)
#define ar_func_exit() do{\
    dbg_printf(log_level"[%lx][FUNC] %s exit\n",get_task_id(),__FUNCTION__);\
    }while(0)
#else
#define ar_func_enter() do{}while(0)
#define ar_func_exit() do{}while(0)
#endif
#ifndef DISABLE_INFO_LOG
#define ar_info(fmt, ...) do{\
    dbg_printf(log_level"[%lx][INFO] %s "fmt"\n",get_task_id(),__FUNCTION__,##__VA_ARGS__);\
    }while(0)
#else
#define ar_info(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_DBG_LOG
#define ar_debug(fmt, ...) do{\
    dbg_printf(log_level"[%lx][DEBUG] %s "fmt"\n",get_task_id(),__FUNCTION__,##__VA_ARGS__);\
    }while(0)
#else
#define ar_debug(fmt, ...)  do{}while(0)
#endif
#ifndef DISABLE_PRT_LOG
#define ar_printf(fmt, ...) do{\
    dbg_printf(log_level""fmt"",##__VA_ARGS__);\
    }while(0)
#else
#define ar_printf(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_ALWAYS_LOG
#define ar_always(fmt, ...)    do{\
        dbg_printf(log_level"[%lx][ALWAYS] %s "fmt"\n",get_task_id(),__FUNCTION__,##__VA_ARGS__);\
        }while(0)
#else
#define ar_always(fmt, ...) do{}while(0)
#endif

#ifndef DISABLE_WARNING_LOG
#define ar_warning(fmt, ...)    do{\
        dbg_printf(log_level"[%lx][WARN] %s "fmt"\n",get_task_id(),__FUNCTION__,##__VA_ARGS__);\
        }while(0)
#else
#define ar_warning(fmt, ...) do{}while(0)
#endif


#define ar_malloc(size) kmalloc((size),GFP_KERNEL);
#define ar_free(p) kfree((p));
#endif



