#ifndef __RPC_RROXY_H__
#define __RPC_RROXY_H__

AR_S32 rpc_proxy_open(char* dev_name, int flags);
AR_S32 rpc_proxy_close(AR_S32 fd);
AR_S32 rpc_proxy_ioctl(AR_S32 fd, AR_S32 cmd, AR_VOID *arg);
AR_S32 rpc_proxy_init(AR_VOID);
AR_S32 rpc_proxy_exit(AR_VOID);
AR_BOOL rpc_proxy_is_inited(AR_VOID);

AR_BOOL ar_hal_sys_mpp_service_is_lib(AR_VOID);
AR_S32 ar_hal_sys_open(char* dev_name, int flags);
AR_S32 ar_hal_sys_close(AR_S32 fd);
AR_S32 ar_hal_sys_ioctl(AR_S32 fd, AR_S32 cmd, AR_VOID *arg);
void ar_set_mpp_service_master_app(const AR_BOOL is_master_app);
AR_BOOL ar_is_mpp_service_master_app(AR_VOID);

#endif
