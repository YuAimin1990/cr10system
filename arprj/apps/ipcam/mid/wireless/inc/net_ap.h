#ifndef __NET_AP_H__
#define __NET_AP_H__

#include 	"net_types.h"
#include 	"net_config.h"
#include 	"net_common.h"

#define NET_AP_OK                  0
#define NET_AP_BADPARAM          (NET_ERR_BADPARAM            + NET_AP_BASE)
#define NET_AP_OUTOFMEMORY       (NET_ERR_OUTOFMEMORY         + NET_AP_BASE)
#define NET_AP_ALREADINIT        (NET_ERR_ALREADINIT          + NET_AP_BASE)
#define NET_AP_NOTINIT           (NET_ERR_NOTINIT             + NET_AP_BASE)
#define NET_AP_NOSUPPORT         (NET_ERR_NOSUPPORT           + NET_AP_BASE)
#define NET_AP_TIMEOUT           (NET_ERR_TIMEOUT             + NET_AP_BASE)
#define NET_AP_DEVICEBUSY        (NET_ERR_DEVICEBUSY          + NET_AP_BASE)
#define NET_AP_INVALIDHANDLE     (NET_ERR_INVALIDHANDLE       + NET_AP_BASE)
#define NET_AP_NOSUPPORT_VERSION (NET_ERR_NOSUPPORT_VERSION   + NET_AP_BASE)
#define NET_AP_NOTOPEN           (NET_ERR_NOTOPEN             + NET_AP_BASE)
#define NET_AP_FROMDRIVER        (NET_ERR_FROMDRIVER          + NET_AP_BASE)


#define HOST_OPEN		0
#define HOST_WEP		1
#define HOST_WPA_TKIP	2
#define HOST_WPA_AES	3
#define HOST_WPA2_TKIP	4
#define HOST_WPA2_AES	5

#ifndef DISABLE_AP_HANDLE
//Sta operate handle.
typedef struct ApObject_s {
	WirelessHandle		handle;
	s32 (*getWiFiInterface)(WirelessHandle, s8 **);
	s32 (*turnOnAccessPoint)(WirelessHandle, const s8*, const s8*, s32);
	s32 (*turnOffAccessPoint)(WirelessHandle);
	s32 (*startUdhcpd)(WirelessHandle);
	s32 (*stopUdhcpd)(WirelessHandle);
} ApObjT;
#ifdef __cplusplus
extern "C" {
#endif
extern ApObjT *CreateNewAp(const s8 *ifname);
extern void DestoryOldAp(ApObjT *staObj);
#ifdef __cplusplus
}
#endif
#endif


#endif

