#ifndef __TESTVO_RPC_H__
#define __TESTVO_RPC_H__

#include "hal_type.h"
#include "hal_vo.h"
#include "test_vo_common.h"

typedef AR_S32 (*RPC_HANDLER)(void *, void *);
typedef struct util_rpc
{
    AR_S32 cmd_id;
    AR_S32 param_len;
    RPC_HANDLER handler;
} VOTEST_RPC_SERVICE_T;


void IPC_MID_UTIL_RPCInit();
void IPC_MID_UTIL_RPCUnInit();
void IPC_MID_UTIL_RPCReply(void * reply, void * out, AR_S32 out_len);

void votest_rpc_init(void);
void votest_rpc_deinit();

#endif
