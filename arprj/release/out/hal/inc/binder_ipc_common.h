/*****************************************************************************
Copyright: 2016-2021, Artosyn. Co., Ltd.
File name: ipc_server.h
Description:
Author: fei.wang@artosyn.cn
Version: v1.0
Date:2021-03-26
History:2021-03-26 : first release sdk
*****************************************************************************/

#ifndef _BINDER_IPC_COMMON_H_
#define _BINDER_IPC_COMMON_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

enum ipc_flags {
    IPC_ONE_WAY       = 0x01, /* this is a one-way call: async, no return */
    IPC_ROOT_OBJECT   = 0x04, /* contents are the component's root object */
    IPC_STATUS_CODE   = 0x08, /* contents are a 32-bit status code */
    IPC_ACCEPT_FDS    = 0x10, /* allow replies with file descriptors */
    IPC_ACCEPT_ASHMEM = 0x40, /* contents are ashmem */
};

typedef struct {
    void* context;
    void* func;
    bool is_main;
} ar_binder_thread_args_t;

typedef int (*ar_service_handler_t)(uint32_t cmd,
                                    void* blob,
                                    size_t len,
                                    void* reply);

typedef int (*ar_service_handler_ext_t)(uint32_t cmd,
                                    void* blob,
                                    size_t len,
                                    void* reply,void *data);


typedef int (*ar_service_callback_t)(uint32_t cmd,
                                     void* blob,
                                     size_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //_BINDER_IPC_COMMON_H_
