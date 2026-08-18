#include "hal_dbglog.h"

/**
 * @file hal_vin_log.h
 * @brief 定义vin的log模块的api
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

//#define ENABALE_ALL_LOG

#ifndef ENABALE_ALL_LOG
#define DISABLE_CND_LOG
#define DISABLE_FUN_LOG
#define DISABLE_INFO_LOG
//#define DISABLE_PRT_LOG
#define DISABLE_CTL_LOG
#define DISABLE_FORMAT_LOG
#define DISABLE_DBG_LOG
#define DISABLE_WARNING_LOG
#endif


#define AR_HAL_MOD_VIN_ERR          HAL_TAG_ID(AR_SYS_ID_VI)
#define AR_HAL_MOD_VIN_COND         HAL_TAG_ID(AR_SYS_ID_VI)
#define AR_HAL_MOD_VIN_ENTER        HAL_TAG_ID(AR_SYS_ID_VI)
#define AR_HAL_MOD_VIN_EXIT         HAL_TAG_ID(AR_SYS_ID_VI)
#define AR_HAL_MOD_VIN_INFO         HAL_TAG_ID(AR_SYS_ID_VI)
#define AR_HAL_MOD_VIN_DEBUG        HAL_TAG_ID(AR_SYS_ID_VI)
#define AR_HAL_MOD_VIN_ALWAYS       HAL_TAG_ID(AR_SYS_ID_VI)

#ifndef DISABLE_ERR_LOG
#define ar_err(fmt,...) do{\
   AR_LOG_ERR(AR_HAL_MOD_VIN_ERR, fmt, ##__VA_ARGS__);\
   }while(0)
#else
#define ar_err(fmt,...) do{}while(0)
#endif
#ifndef DISABLE_CND_LOG
#define ar_conditon(fmt, ...) do{\
	AR_LOG_DBG(AR_HAL_MOD_VIN_COND, fmt, ##__VA_ARGS__);\
   }while(0)
#else
#define ar_conditon(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_FUN_LOG
#define ar_func_enter(...) do{\
	AR_LOG_DBG(AR_HAL_MOD_VIN_ENTER, "Entry", ##__VA_ARGS__);\
   }while(0)
#define ar_func_exit(...) do{\
    AR_LOG_DBG(AR_HAL_MOD_VIN_EXIT, "Exit", ##__VA_ARGS__); \
   }while(0)
#else
#define ar_func_enter() do{}while(0)
#define ar_func_exit() do{}while(0)
#endif
#ifndef DISABLE_INFO_LOG
#define ar_info(fmt, ...) do{\
	AR_LOG_INFO(AR_HAL_MOD_VIN_INFO, fmt, ##__VA_ARGS__);\
   }while(0)
#else
#define ar_info(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_DBG_LOG
#define ar_debug(fmt, ...) do{\
	AR_LOG_DBG(AR_HAL_MOD_VIN_DEBUG, fmt, ##__VA_ARGS__);\
   }while(0)
#else
#define ar_debug(fmt, ...)  do{}while(0)
#endif
#ifndef DISABLE_PRT_LOG
#define ar_printf(fmt, ...) do{\
    AR_LOG_RAW(fmt,##__VA_ARGS__);\
   }while(0)
#else
#define ar_printf(fmt, ...) do{}while(0)
#endif
#ifndef DISABLE_ALWAYS_LOG
#define ar_always(fmt, ...)    do{\
	    AR_LOG_INFO(AR_HAL_MOD_VIN_ALWAYS, fmt, ##__VA_ARGS__);\
        }while(0)
#else
#define ar_always(fmt, ...) do{}while(0)
#endif

#ifndef DISABLE_WARNING_LOG
#define ar_warning(fmt, ...)    do{\
		AR_LOG_WARN(AR_HAL_MOD_VIN_WARN, fmt, ##__VA_ARGS__);\
		}while(0)
#else
#define ar_warning(fmt, ...) do{}while(0)
#endif
