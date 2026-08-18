#ifndef __NET_COMMON_H__
#define __NET_COMMON_H__

#include "net_types.h"
#include "net_config.h"
#include "net_log.h"
#include "net_utils.h"

//*******************************************************************//
typedef char * WIFI_HANDLE;

#define NET_WIFI_DEFAULT_BUS 		1
#define NET_WIFI_DEFAULT_DEVICENUM 	2

#define NET_WIFI_DEVICENAME_MT7601		"mt7601"
#define NET_WIFI_DEVICENAME_RTL8188		"rtl8188"

#define NET_WIFI_LABEL_APCONFIG		"APCONFIG"
#define NET_WIFI_LABEL_STACONFIG	"STACONFIG"




typedef struct {
	s8 *label;
	s8 *value;
} NET_WIFI_SupportCustomT;

typedef struct {
	s8 *deviceName;
	bool supportAP;
	bool supportSTA;
	bool supportWPS;
	bool supportSmartlink;
	NET_WIFI_SupportCustomT custom[4];
} NET_WIFI_SupportFuncT;

typedef struct {
	u32 venderID;
	u32 deviceID;
} NET_WIFI_DeviceNumT;

typedef struct {
	u32 venderID;
	s8 	venderName[12];
} NET_WIFI_MapVenderNameT;

typedef struct {
	NET_WIFI_DeviceNumT deviceNum;
	s8 				deviceName[12];
} NET_WIFI_MapDeviceNameT;

/* wireless handle */
typedef void * WirelessHandle;

#ifdef __cplusplus
extern "C" {
#endif
extern s32 net_wifi_findInterFace(s8 *buffer, s32 len);
extern s32 net_wifi_checkDriverExist(const s8 *driver_name);
extern s32 net_wifi_loadDriver(const s8 *driver_name);
extern s32 net_wifi_unloadDriver(const s8 *driver_name);
/* get usb device ID */
extern s32 net_wifi_getUsbDeviceID(NET_WIFI_DeviceNumT *device, s32 bus, s32
	deviceNum);
extern s32 net_wifi_getVenderNameByDeviceNum(NET_WIFI_DeviceNumT device, s8
	**pName);
extern s32 net_wifi_getDeviceNameByDeviceNum(NET_WIFI_DeviceNumT device, s8
	**pName);
extern NET_WIFI_SupportFuncT *net_wifi_getDeviceSupportFunc(const s8
	*deviceName);
/* get all wifi dev name */
//extern int net_wifi_get_wifidevlist(WIFI_DEV_ListT *listbuf, int list_len);

extern s32 net_wifi_check_driver_exist(const s8 *driver_name);
extern s32 net_wifi_load_driver(const s8 *driver_name);
extern s32 net_wifi_unload_driver(const s8 *driver_name);

#ifdef __cplusplus
}
#endif



#endif

