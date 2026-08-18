#ifndef __NET_WIFI_H__
#define __NET_WIFI_H__

#include "net_common.h"
#include "net_sta.h"
#include "net_ap.h"

#define NET_WIRELESS_OK                 0
#define NET_WIRELESS_BADPARAM          (NET_ERR_BADPARAM            + NET_WIRELESS_BASE)
#define NET_WIRELESS_OUTOFMEMORY       (NET_ERR_OUTOFMEMORY         + NET_WIRELESS_BASE)
#define NET_WIRELESS_ALREADINIT        (NET_ERR_ALREADINIT          + NET_WIRELESS_BASE)
#define NET_WIRELESS_NOTINIT           (NET_ERR_NOTINIT             + NET_WIRELESS_BASE)
#define NET_WIRELESS_NOSUPPORT         (NET_ERR_NOSUPPORT           + NET_WIRELESS_BASE)
#define NET_WIRELESS_TIMEOUT           (NET_ERR_TIMEOUT             + NET_WIRELESS_BASE)
#define NET_WIRELESS_DEVICEBUSY        (NET_ERR_DEVICEBUSY          + NET_WIRELESS_BASE)
#define NET_WIRELESS_INVALIDHANDLE     (NET_ERR_INVALIDHANDLE       + NET_WIRELESS_BASE)
#define NET_WIRELESS_NOSUPPORT_VERSION (NET_ERR_NOSUPPORT_VERSION   + NET_WIRELESS_BASE)
#define NET_WIRELESS_NOTOPEN           (NET_ERR_NOTOPEN             + NET_WIRELESS_BASE)
#define NET_WIRELESS_FROMDRIVER        (NET_ERR_FROMDRIVER          + NET_WIRELESS_BASE)

typedef enum
{
	NET_WIFI_NONE = 0,
	NET_WIFI_STA,
	NET_WIFI_AP,
} NET_WIFI_WORK_StatusEnumT;

typedef enum eIPC_WIFI_MODE
{
	IPC_WIFI_MODE_NONE = 0,
	IPC_WIFI_MODE_AP,
	IPC_WIFI_MODE_STA,
} E_IPC_WIFI_MODE;

#define WIFI_RUNING		(1)
#define WIFI_STOP		(0)


#ifndef DISABLE_WIRELESS_HANDLE
//Sta operate handle.
typedef struct WirelessManageObject_s {
	WirelessHandle		handle;
	s32 (*getCurrentInterface)(WirelessHandle,  s8 **);
	WirelessHandle (*getWirelessObject)(WirelessHandle);
	s32 (*setWirelessWorkMode)(WirelessHandle,  s32);
	s32 (*getWirelessWorkMode)(WirelessHandle);
	s32 (*upWirelessNetwork)(WirelessHandle);
	s32 (*downWirelessNetwork)(WirelessHandle);
	s32 (*getWirelessMAC)(WirelessHandle,  s8 **);
	s32 (*setWirelessMAC)(WirelessHandle,  const s8 *);
	s32 (*getWirelessMask)(WirelessHandle,  s8 **);
	s32 (*setWirelessMask)(WirelessHandle,  const s8 *);
	s32 (*getWirelessIP)(WirelessHandle,  s8 **);
	s32 (*setWirelessIP)(WirelessHandle,  const s8 *);
} WirelessManageObjT;
#ifdef __cplusplus
extern "C" {
#endif
extern WirelessManageObjT *CreateNewWirelessManage(const char *ifname);
extern void DestoryOldWirelessManage(WirelessManageObjT *WirelessObj);

void ipc_init_mt7601();

int ipc_get_wifi_ifname(char *name);
void ipc_interface_ctrl(const char *ifname, int on);

int ipc_is_scan_ready();
int ipc_get_scan_list(WIFI_SCAN_LIST_t *list);
void ipc_wifi_wpa_scanning();
void ipc_wifi_iwlist_scan_once();
void ipc_wifi_iwlist_scanning();

int ipc_wifi_timer_scan_start();
int ipc_wifi_timer_scan_stop();
int ipc_wifi_once_scan_start();
int ipc_wifi_once_scan_stop();

int ipc_wifi_start_ap();
int ipc_wifi_stop_ap();
int ipc_wifi_start_sta(const char* ifname, const char *ssid, const char *key);

int ipc_wifi_wpa_connect(const char* ifname, const char *ssid, const char *key, unsigned char usedhcp);

#ifdef __cplusplus
}
#endif

#endif
#endif
