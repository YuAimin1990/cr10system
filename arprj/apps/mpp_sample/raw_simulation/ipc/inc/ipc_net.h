#ifndef __IPC_NET_H__
#define __IPC_NET_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	char    essid[32];
	char    passd[32];
	int     encMode;
	int     valid;
} NET_LINK_INFO;

typedef enum {
	E_NET_WAIT = 1,
    E_NET_AP_BEGIN,
    E_NET_AP_END,
    E_NET_STA_BEGIN,
    E_NET_STA_END,
    E_NET_STA_NFS,
    E_NET_STA_TEST,
} E_NET_STATUS;

int IPC_NET_Init();
void IPC_NET_UnInit();
AR_S32 IPC_NET_P2P_Start();
AR_S32 IPC_NET_P2P_Stop();
E_NET_STATUS IPC_NET_GetStatus();
void IPC_WIFI_Config();
AR_S32 IPC_NET_RTSP_Start(void);
void IPC_NET_RTSP_Stop(void);

#ifdef __cplusplus
};
#endif
#endif

