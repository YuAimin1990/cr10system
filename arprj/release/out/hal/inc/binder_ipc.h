/*****************************************************************************
Copyright: 2016-2021, Artosyn. Co., Ltd.
File name: ipc_server.h
Description:
Author: fei.wang@artosyn.cn
Version: v1.0
Date:2021-03-10
History:2021-03-10 : first release sdk
*****************************************************************************/
#ifndef _BINDER_IPC_H_
#define _BINDER_IPC_H_

#include <stdint.h>
#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


typedef struct {
    void* handle;
} ar_ipc_state_t;

typedef union {
    int fd;
    uint32_t handle;
    void* blob;
} ar_ipc_data_t;

// TODO 1. use ashmem to transfer blob memory
//      2. transfer fd

// create binder ipc
ar_ipc_state_t* ar_ipc_create();

// close binder ipc
void ar_ipc_close(ar_ipc_state_t* ipc);

// add service which named name
int ar_ipc_add_service(ar_ipc_state_t* ipc, const char *name, void *ptr);

// get service which named name
uint32_t ar_ipc_get_service(ar_ipc_state_t* ipc, const char *name);

// close service
int ar_ipc_close_service(ar_ipc_state_t* ipc, uint32_t handle);

// send buffer to peer client.
// [in] ipc, ipc context
// [in] handle, service handle, a proxy handle of server
// [in] cmd, used to diff different command
// [in] blob, contiguous memory which can be created as struct/string/int/...
// [in] len, buffer size
// [out] out, buffer return from peer
// [in] flags, 0->sync IPC, IPC_ONE_WAY->async IPC, IPC_ACCEPT_FDS->transmit fd
int ar_ipc_send_buffer(ar_ipc_state_t* ipc, uint32_t handle, uint32_t cmd, void* blob,
                       size_t len, void* out, uint32_t flags);

// [in] handle, addr which used to store return buffer
// [in] state, indicate whether a IPC call is success or not 0 is success.
// [in] blob, returned buffer to sender
// [in] len, returned buffer len
void ar_ipc_return_buffer(void* handle, uint32_t state, void* blob, size_t len);

// set max threads
void ar_ipc_set_maxthreads(ar_ipc_state_t* ipc, size_t maxthreads);

// create binder ipc main thread to handle reply message from driver
void ar_ipc_start_thread_pool(ar_ipc_state_t* ipc);
void ar_ipc_start_thread_pool_ext(ar_ipc_state_t* ipc,void *data);
// destroy binder ipc main thread and all derivatived threads
void ar_ipc_stop_thread_pool(ar_ipc_state_t* ipc);

/**
* @brief  处理binder 命令行的回调函数
* @retval AR_FAILURE 失败 , 其它 成功, 相关定义参看ENMU_SYS_AR_SOC_TYPE.
*/

typedef AR_S32 (*binder_cmd_process)(AR_CHAR *cmd_str,AR_S32 len,void *data);

typedef struct
{
  ar_ipc_state_t* binder;
  binder_cmd_process fun;
  AR_VOID *data;
}BINDER_CMD_T;

/**
* @brief  注册binder 命令行
* @retval NULL 失败 , 其它 成功, 
*/

AR_VOID  * ar_hal_sys_register_binder_cmd(AR_CHAR *cmd_name,binder_cmd_process fun,void *data);
/**
* @brief  反注册binder 命令行
* @retval AR_FAILURE 失败 , 其它 成功, 相关定义参看ENMU_SYS_AR_SOC_TYPE.
*/
AR_S32 ar_hal_sys_unregister_binder_cmd(AR_VOID *binder);

int binder_suspend(AR_VOID *binder);
AR_VOID * binder_resume(AR_CHAR *cmd_name, binder_cmd_process fun, AR_VOID *data);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif // _BINDER_IPC_H_
