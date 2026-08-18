/**
 * \file
 * \brief 系统控制休眠及唤醒相关的数据结构和接口.
 */

#ifndef __MPI_SYSCTL_H__
#define __MPI_SYSCTL_H__

#include "hal_type.h"
#include "ar_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/**
\addtogroup MPI_SYSCTL
 * @brief 系统控制提供休眠及唤醒相关接口，用于实现快速启动。
 * @{
*/
typedef void* MPI_SYSCTL_HANDLE;

/** 事件类型 */
typedef enum arSYSCTL_EVENT_E
{
    SYSCTL_EVENT_NONE,          /**<@note      无事件  */
    SYSCTL_EVENT_FAST_SUSPEND,  /**<@note      该事件已弃用 */
    SYSCTL_EVENT_FAST_RESUME,   /**<@note      该事件已弃用 */
    SYSCTL_EVENT_SUSPEND,       /**<@note      深度休眠，DDR Retention方案  */
    SYSCTL_EVENT_RESUME,        /**<@note      从深度休眠唤醒  */
    SYSCTL_EVENT_EXIT           /**<@note      sysctl退出  */
}SYSCTL_EVENT_E;

/** 工作状态 */
typedef enum arSYSCTL_STATUS_E
{
    SYSCTL_STAT_RUNNING,       /**<@note       处于工作状态  */
    SYSCTL_STAT_SUSPEND,       /**<@note       处于休眠状态  */
    SYSCTL_STAT_BUSY,          /**<@note       正在执行休眠或唤醒  */
    SYSCTL_STAT_ERROR          /**<@note       处于错误状态  */
}SYSCTL_STATUS_E;

typedef void (*SYSCTL_EVENT_CB) (SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len);

/**
* @brief      用于app注册热启动相关信息到系统中
* @param      pName             名称
*             u32Priority       优先级，该数值越低，优先级越高
*             SYSCTL_EVENT_CB   接受系统事件的回调函数
* @retval     返回app注册句柄   0表示失败， 非0表示成功
*/
MPI_SYSCTL_HANDLE AR_MPI_SYSCTL_Register(AR_CHAR *pName, AR_U32 u32Priority, SYSCTL_EVENT_CB pCallback);

/**
* @brief      用于app取消注册到系统中热启动相关信息
* @param      pHandle            注册返回的句柄
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYSCTL_Unregister(MPI_SYSCTL_HANDLE pHandle);

/**
* @brief      用于app通知系统事件已完成
* @param      pHandle            注册返回的句柄
* @param      event              事件类型
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYSCTL_Event_done(MPI_SYSCTL_HANDLE pHandle, SYSCTL_EVENT_E event);

/**
* @brief      用于向系统下发普通休眠命令（已弃用）
* @retval     0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_SYSCTL_Fast_Suspend(void);

/**
* @brief      用于向系统下普通休眠的唤醒命令（已弃用）
* @retval     0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_SYSCTL_Fast_Resume(void);

/**
* @brief      用于向系统下发深度休眠命令，DDR Retention方案
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYSCTL_Suspend();

/**
* @brief      用于向系统下发查询状态命令
* @param      e_sysctl_stat      sysctl工作状态
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYSCTL_Query_Status(SYSCTL_STATUS_E *e_sysctl_stat);

/** @} */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_SYSCTL_H__ */

