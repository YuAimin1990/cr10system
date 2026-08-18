#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "pf_type.h"
#include "binder_ipc.h"

#include "util_rpc.h"

#define IPC_MAX_RPC_CMD     0x1000

static pthread_mutex_t s_RpcMutex = PTHREAD_MUTEX_INITIALIZER;
static IPC_RPC_SERVICE_T s_RpcServiceTable[IPC_MAX_RPC_CMD] = {0};
static ar_ipc_state_t * s_ar_ipc = NULL;

static AR_S32 ipc_service_handler(uint32_t cmd, ar_ipc_data_t * data, unsigned len, void * reply)
{
    AR_S32 i = 0;
    for(i = 0; i < IPC_MAX_RPC_CMD; i++)
    {
        if(s_RpcServiceTable[i].cmd_id == cmd)
        {
            if(s_RpcServiceTable[i].handler && len == s_RpcServiceTable[i].param_len)
            {
                return s_RpcServiceTable[i].handler(data->blob, reply);
            }
        }
    }

    return 0;
}

void IPC_MID_UTIL_RPCInit()
{
    AR_S32 ret = 0;
    pthread_mutex_lock(&s_RpcMutex);
    if(s_ar_ipc)
    {
        pthread_mutex_unlock(&s_RpcMutex);
        return;
    }

    s_ar_ipc = ar_ipc_create();
    ret = ar_ipc_add_service(s_ar_ipc, "ipc_ctrl", ipc_service_handler);
    if(ret)
    {
        printf("Add service error.\n");
        pthread_mutex_unlock(&s_RpcMutex);
        return;
    }

    ar_ipc_start_thread_pool(s_ar_ipc);

    pthread_mutex_unlock(&s_RpcMutex);
}

void IPC_MID_UTIL_RPCRegister(AR_S32 cmd, RPC_HANDLER handler, AR_S32 param_len)
{
    AR_S32 i = 0;

    pthread_mutex_lock(&s_RpcMutex);

    for(i = 0; i < IPC_MAX_RPC_CMD; i++)
    {
        if(s_RpcServiceTable[i].cmd_id == cmd)
        {
            printf("The cmd %x has been registered already.\n", cmd);
            pthread_mutex_unlock(&s_RpcMutex);
            return;
        }
        else if(s_RpcServiceTable[i].cmd_id == 0)
        {
            s_RpcServiceTable[i].cmd_id = cmd;
            s_RpcServiceTable[i].param_len = param_len;
            s_RpcServiceTable[i].handler = handler;
            break;
        }
        else
        {
            continue;
        }
    }

    if(i ==  IPC_MAX_RPC_CMD)
    {
        printf("Has reached max num of service. register failed.\n");
    }

    pthread_mutex_unlock(&s_RpcMutex);
}

void IPC_MID_UTIL_RPCUnInit()
{
    ar_ipc_stop_thread_pool(s_ar_ipc);

    ar_ipc_close(s_ar_ipc);
	s_ar_ipc = NULL;

	memset(s_RpcServiceTable, 0 , IPC_MAX_RPC_CMD * sizeof(IPC_RPC_SERVICE_T));
}

void IPC_MID_UTIL_RPCReply(void * reply, void * out, AR_S32 out_len)
{
    uint32_t state = 0;
    ar_ipc_return_buffer(reply, state, out, out_len);
}

