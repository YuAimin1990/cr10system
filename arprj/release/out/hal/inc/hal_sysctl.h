#ifndef __HAL_SYSCTL_H__
#define __HAL_SYSCTL_H__

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "hal_type.h"
#include "ar_sysctl_drv.h"

typedef void* AR_SYSCTL_HANDLE;

typedef void (*ar_sysctl_event_cb) (ENUM_AR_SYSCTL_EVENT e_sysctl_event, void *p_data, AR_S32 s32_len);

/**
\brief      用于app注册热启动相关信息到系统中
\retval     0 成功 , 其它 失败.
*/
AR_SYSCTL_HANDLE ar_hal_sysctl_register( AR_CHAR *p_name, AR_U32 u32_priority, ar_sysctl_event_cb p_callback);

/**
\brief      用于app取消注册到系统中热启动相关信息
\retval     0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sysctl_unregister(AR_SYSCTL_HANDLE p_handle);

/**
\brief      用于app通知系统事件已完成
\retval     0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sysctl_event_done(AR_SYSCTL_HANDLE p_handle, ENUM_AR_SYSCTL_EVENT e_event);

/**
\brief      用于向系统下发普通休眠命令
\retval     0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sysctl_fast_suspend();

/**
\brief      用于向系统下发普通休眠的唤醒命令
\retval     0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sysctl_fast_resume();

/**
\brief      用于向系统下发深度休眠命令，DDR Retention方案
\retval     0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sysctl_suspend();

/**
\brief      用于向系统下发查询状态命令
\retval     0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sysctl_query_status(ENUM_AR_SYSCTL_STATUS *sysctl_stat);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HAL_SYSCTL_H__ */

