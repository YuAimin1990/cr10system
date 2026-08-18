#ifndef __UTIL_RPC_H__
#define __UTIL_RPC_H__

typedef AR_S32 (*RPC_HANDLER)(void *, void *);
typedef struct util_rpc
{
    AR_S32 cmd_id;
    AR_S32 param_len;
    RPC_HANDLER handler;
} IPC_RPC_SERVICE_T;

void IPC_MID_UTIL_RPCInit();
void IPC_MID_UTIL_RPCUnInit();
void IPC_MID_UTIL_RPCRegister(AR_S32 cmd, RPC_HANDLER handler, AR_S32 param_len);
void IPC_MID_UTIL_RPCReply(void * reply, void * out, AR_S32 out_len);

#endif