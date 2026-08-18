#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <linux/if_ether.h>
#include <netinet/in.h>

#include "net_common.h"
#include "net_utils.h"

#define DISABLE_AP_HANDLE
#include "net_ap.h"
#undef DISABLE_AP_HANDLE
#include "util_api.h"
#include "net_types.h"
#include "net_log.h"

#define CMD_LEN_MIN  	64
#define CMD_LEN_MAX  	128

#define DEFAULT_HOST_SSID	"IPC_AP[NONE]"
//#define HOST_CFG "/tmp/hostapd.conf"

//lable of  change ap configure file
#define LABEL_SSID			"SSID"
#define LABLE_HIDE_SSID		"HideSSID"
#define LABEL_AUTH_MODE		"AuthMode"
#define LABEL_ENCRYP_TYPE	"EncrypType"
#define LABEL_PASSWORD		"WPAPSK"
#define LABEL_WEP_KEY		"Key1Str"


//params of hide ssid
#ifdef LABLE_HIDE_SSID
#define VALUE_HIDE_SSID_OFF		0
#define VALUE_HIDE_SSID_ON		1
#endif

//auth mode
#ifdef LABEL_AUTH_MODE
#define VALUE_AUTH_MODE_OPEN	"OPEN"
#define VALUE_AUTH_MODE_SHARED	"SHARED"
#define VALUE_AUTH_MODE_WPAPSK	"WPAPSK"
#define VALUE_AUTH_MODE_WPA2PSK	"WPA2PSK"
#define VALUE_AUTH_MODE_WPANONE	"WPANONE"
#endif

//Encryp type
#ifdef LABEL_ENCRYP_TYPE
#define VALUE_ENCRYP_TYPE_NONE	"NONE"
#define VALUE_ENCRYP_TYPE_WEP	"WEP"
#define VALUE_ENCRYP_TYPE_TKIP	"TKIP"
#define VALUE_ENCRYP_TYPE_AES	"AES"
#endif

//AP private data space
typedef struct ApPrivate_s {
	s8 ifname[IFNAMSIZ];
	s8 ssid[IW_ESSID_MAX_SIZE];
	s32 encryption;
	pthread_rwlock_t 	apRwlock;
	pthread_mutex_t  	apMutex;
    bool isOpen;
	s8 *deviceName;
	s8 *APConfig;
} ApPrivateT;

//AP operate handle.
typedef struct ApObject_s {
	WirelessHandle		handle;
	s32 (*getWiFiInterface)(WirelessHandle, s8 **);
	s32 (*turnOnAccessPoint)(WirelessHandle, const s8*, const s8*, s32);
	s32 (*turnOffAccessPoint)(WirelessHandle);
	s32 (*startUdhcpd)(WirelessHandle);
	s32 (*stopUdhcpd)(WirelessHandle);
	ApPrivateT		private;
} ApObjT;

#define pTryMutex(n)	pthread_mutex_trylock(&(n->private.apMutex))
#define pMutex(n)		pthread_mutex_lock(&(n->private.apMutex))
#define vMutex(n)		pthread_mutex_unlock(&(n->private.apMutex))

static ApObjT *getObjectByHandle(WirelessHandle handle)
{
	return (ApObjT *)handle;
}

static s32 _apProbe(WirelessHandle handle)
{
	s32 ret = 0, i;
	s8 *wifi_device = NULL;
	NET_WIFI_DeviceNumT device_info;
	NET_WIFI_SupportFuncT *supportFunc = NULL;
	NET_WIFI_SupportCustomT *pCustom = NULL;

	ApObjT *apObj = getObjectByHandle(handle);
	if(isNull(apObj)) {
		LogE("invalid handle.");
		return NET_AP_INVALIDHANDLE;
	}
	memset(&device_info, 0, sizeof(NET_WIFI_DeviceNumT));
	ret = net_wifi_getUsbDeviceID(&device_info,
		NET_WIFI_DEFAULT_BUS, NET_WIFI_DEFAULT_DEVICENUM);
	if(ret < 0) {
		LogE("getUsbDeviceID is failed");
		return ret;
	}
	ret = net_wifi_getDeviceNameByDeviceNum(device_info, &wifi_device);
	if(ret < 0 || wifi_device == NULL) {
		LogE("getDeviceNameByDeviceNum is failed");
		return ret;
	}
	supportFunc = net_wifi_getDeviceSupportFunc(wifi_device);
	if(supportFunc == NULL) {
		LogE("No device.");
		return NET_AP_BADPARAM;
	}
	apObj->private.deviceName 	= supportFunc->deviceName;
	//get AP configure into custom.
	pCustom = supportFunc->custom;
	for(i = 0; i < sizeof(supportFunc->custom)/sizeof(NET_WIFI_SupportCustomT);
		i++, pCustom++) {
			if(pCustom->label == NULL)
				break;
			if(strcmp(pCustom->label, NET_WIFI_LABEL_APCONFIG) == 0) {
				apObj->private.APConfig = pCustom->value;
				break;
			}
	}
	return NET_AP_OK;
}

static s32 FAST_FUNC net_ap_up(WirelessHandle handle)
{
	//s32 ret = 0;
	s8 *cmd = NULL;
	ApObjT *apObj = getObjectByHandle(handle);
	if(isNull(apObj)) {
		LogE("invalid handle.");
		return NET_AP_INVALIDHANDLE;
	}
	cmd = (char *)malloc(CMD_LEN_MIN);
	if(cmd == NULL)
		return NET_AP_OUTOFMEMORY;
	memset(cmd, 0, CMD_LEN_MIN);
	snprintf(cmd, CMD_LEN_MIN, "%s %s up", IFCONFIG_CMD, apObj->private.ifname);
	cmd[CMD_LEN_MIN - 1] = '\0';
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	return NET_AP_OK;
}

static s32 FAST_FUNC net_ap_down(WirelessHandle handle)
{
	//s32 ret = 0;
	s8 *cmd = NULL;
	ApObjT *apObj = getObjectByHandle(handle);
	if(isNull(apObj)) {
		LogE("invalid handle.");
		return NET_AP_INVALIDHANDLE;
	}
	cmd = (char *)malloc(CMD_LEN_MIN);
	if(cmd == NULL)
		return NET_AP_OUTOFMEMORY;
	memset(cmd, 0, CMD_LEN_MIN);
	snprintf(cmd, CMD_LEN_MIN, "%s %s down", IFCONFIG_CMD, apObj->private.ifname);
	cmd[CMD_LEN_MIN - 1] = '\0';
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	return NET_AP_OK;
}

static s32 FAST_FUNC _apEditCfgFile(const s8 *label, const s8 *value)
{
	s32 ret = 0;
	s8 cmd[CMD_LEN_MAX] = {0};
	if(isNull(value) || isNull(label)) {
		LogE("Bad parm");
		return NET_AP_BADPARAM;
	}
	if(access(DRIVER_AP_CFG, F_OK)) {
		LogE("File:%s is no exist", DRIVER_AP_CFG);
		return NET_AP_BADPARAM;
	}
	snprintf(cmd, sizeof(cmd),"%s -i \"s/^%s=.*$/%s=%s/\" %s",
		SED_CMD, label, label, value, DRIVER_AP_CFG);

	ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	if(ret < 0){
		perror("Net wifi edit configure file:");
	}
	return ret;
}

#define LABEL_UDHCPD_IF		"interface"
static s32 FAST_FUNC _apEditUdhcpdCfgFile(const s8 *label, const s8 *value)
{
	s32 ret = 0;
	s8 cmd[CMD_LEN_MAX] = {0};
	if(isNull(value) || isNull(label)) {
		LogE("Bad parm");
		return NET_AP_BADPARAM;
	}
	if(access(UDHCPD_CFG, F_OK)) {
		LogE("File:%s is no exist", UDHCPD_CFG);
		return NET_AP_BADPARAM;
	}
	snprintf(cmd, sizeof(cmd),"%s -i \"s/^%s.*$/%s %s/\" %s",
		SED_CMD, label, label, value, UDHCPD_CFG);

	ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	if(ret < 0){
		perror("Net wifi edit configure file:");
	}
	return ret;
}

static s32 FAST_FUNC _apSetInterface(const s8 *ifname)
{

	if(isNull(ifname))
		return NET_AP_BADPARAM;
	return _apEditUdhcpdCfgFile(LABEL_UDHCPD_IF, ifname);
}

static s32 FAST_FUNC _apSetSSID(const s8 *ssid)
{
	if(isNull(ssid))
		return _apEditCfgFile(LABEL_SSID, DEFAULT_HOST_SSID);
	else
		return _apEditCfgFile(LABEL_SSID, ssid);
}

static s32 FAST_FUNC _apSetAuthMode(const s8 *mode)
{
	if(isNull(mode))
		return NET_AP_BADPARAM;
	else
		return _apEditCfgFile(LABEL_AUTH_MODE, mode);
}

static s32 FAST_FUNC _apSetEncrypType(const s8 *encryp_type)
{
	if(isNull(encryp_type))
		return NET_AP_BADPARAM;
	else
		return _apEditCfgFile(LABEL_ENCRYP_TYPE, encryp_type);
}

static s32 FAST_FUNC _apSetPassword(const s8 *key)
{
	if(isNull(key))
		return NET_AP_BADPARAM;
	else
		return _apEditCfgFile(LABEL_PASSWORD, key);
}

static s32 FAST_FUNC _apSetWEPKey(const s8 *key)
{
	if(isNull(key))
		return NET_AP_BADPARAM;
	else
		return _apEditCfgFile(LABEL_WEP_KEY, key);
}

static void FAST_FUNC _apSetUdhcpd(void)
{
    IPC_MID_UTIL_SysCall("killall udhcpd", NULL, 0);
    IPC_MID_UTIL_SysCall("udhcpd", NULL, 0);
}

static s32 FAST_FUNC _apSetEncrypMethod(s32 encryp_method)
{
	switch(encryp_method)
	{
		case HOST_OPEN:
			_apSetAuthMode(VALUE_AUTH_MODE_OPEN);
			_apSetEncrypType(VALUE_ENCRYP_TYPE_NONE);
			break;
		case HOST_WEP:
			_apSetAuthMode(VALUE_AUTH_MODE_SHARED);
			_apSetEncrypType(VALUE_ENCRYP_TYPE_WEP);
			break;
		case HOST_WPA_TKIP:
			_apSetAuthMode(VALUE_AUTH_MODE_WPAPSK);
			_apSetEncrypType(VALUE_ENCRYP_TYPE_TKIP);
			break;
		case HOST_WPA_AES:
			_apSetAuthMode(VALUE_AUTH_MODE_WPAPSK);
			_apSetEncrypType(VALUE_ENCRYP_TYPE_AES);
			break;
		case HOST_WPA2_TKIP:
			_apSetAuthMode(VALUE_AUTH_MODE_WPA2PSK);
			_apSetEncrypType(VALUE_ENCRYP_TYPE_TKIP);
			break;
		case HOST_WPA2_AES:
			_apSetAuthMode(VALUE_AUTH_MODE_WPA2PSK);
			_apSetEncrypType(VALUE_ENCRYP_TYPE_AES);
			break;
		default:
			return NET_AP_NOSUPPORT;
	}
	return NET_AP_OK;
}

static void FAST_FUNC _apReloadConfigure(WirelessHandle handle)
{
	net_ap_down(handle);
	net_ap_up(handle);
}

static s32 turnOnAccessPoint(WirelessHandle handle, const s8 *ssid,
	const s8 *passwd, s32 encryp_method)
{
	s32 ret = 0;
	ApObjT *apObj =  getObjectByHandle(handle);
	if(isNull(apObj)){
		return NET_AP_INVALIDHANDLE;
	}
	if(!apObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_AP_NOTINIT;
	}
	if(isNull(passwd) && (encryp_method != HOST_OPEN)){
		LogE("Bad params\n");
		return NET_AP_BADPARAM;
	}
	ret = pTryMutex(apObj);
	if(ret < 0)
		return NET_AP_DEVICEBUSY;
	ret = _apSetSSID(ssid);//set ssid
	if(ret < 0)
		goto exit;

	ret = _apSetEncrypMethod(encryp_method);//set encryp  method
	if(ret < 0)
		goto exit;
	//set password
	if(!isNull(passwd)) {
		if(encryp_method == HOST_WEP) {
			_apSetWEPKey(passwd);
		} else {
			_apSetPassword(passwd);
		}
	}
	_apReloadConfigure(apObj->handle);
exit:
	vMutex(apObj);
	return ret;
}

static s32 startUdhcpd(WirelessHandle handle)
{
	ApObjT *apObj =  getObjectByHandle(handle);
	if(isNull(apObj)){
		return NET_AP_INVALIDHANDLE;
	}
	if(!apObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_AP_NOTINIT;
	}
	s32 ret = _apSetInterface(apObj->private.ifname);
	_apSetUdhcpd();
	return ret;
}

static s32 stopUdhcpd(WirelessHandle handle)
{
	ApObjT *apObj =  getObjectByHandle(handle);
	if(isNull(apObj)){
		return NET_AP_INVALIDHANDLE;
	}
	if(apObj->private.isOpen){
		LogW("Wireless AP is already close.");
	}
    IPC_MID_UTIL_SysCall("killall udhcpd", NULL, 0);

	return NET_AP_OK;
}


static s32 turnOffAccessPoint(WirelessHandle handle)
{
	ApObjT *apObj =  getObjectByHandle(handle);
	if(isNull(apObj)){
		return NET_AP_INVALIDHANDLE;
	}
	if(!apObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_AP_NOTINIT;
	}
	pMutex(apObj);
	apObj->private.isOpen = false;//set flag of open wireless.
	vMutex(apObj);
	return net_ap_down(apObj->handle);
}

static s32 getWiFiInterface(WirelessHandle handle, s8 **pIfname)
{
	ApObjT *apObj =  getObjectByHandle(handle);
	if(isNull(apObj)){
		return NET_AP_INVALIDHANDLE;
	}
	if(isNull(pIfname)) {
		LogE("Bad params.");
		return NET_AP_BADPARAM;
	}
	if(!apObj->private.isOpen){
		LogW("Wireless not open.");
	}
	*pIfname = apObj->private.ifname;
	return NET_AP_OK;
}

ApObjT *CreateNewAp(const s8 *ifname)
{
	ApObjT *apObj = malloc(sizeof(ApObjT));
	if(isNull(apObj) || isNull(ifname)) {
		LogE("Memory is not enough.");
		return NULL;
	}
	memset(apObj, 0, sizeof(ApObjT));
	apObj->handle = (WirelessHandle)apObj;
	apObj->getWiFiInterface = getWiFiInterface;
	apObj->turnOnAccessPoint = turnOnAccessPoint;
	apObj->turnOffAccessPoint = turnOffAccessPoint;
	apObj->startUdhcpd = startUdhcpd;
	apObj->stopUdhcpd = stopUdhcpd;
	strncpy(apObj->private.ifname, ifname, sizeof(apObj->private.ifname));
	//s32 ret = _staProbe(apObj->handle);
	s32 ret = _apProbe(apObj->handle);
	if(ret != NET_AP_OK) {
		free(apObj);
		return NULL;
	}
	pthread_mutex_init(&(apObj->private.apMutex), NULL);
	return apObj;
}

void DestoryOldAp(ApObjT *apObj)
{
	if(isNull(apObj))
		return;
	if(apObj->private.isOpen){
		apObj->turnOffAccessPoint(apObj->handle);
	}
	pthread_mutex_destroy(&(apObj->private.apMutex));
	free(apObj);
}

#ifdef WIFI_HOST_TEST
static void net_wifi_usage(void)
{
	printf("Wifi host Usage:\n");
	printf("  ./wifi_ap start [ssid] [key]\n");
	printf("\t==>start build AP.\n");
	printf("  ./wifi_ap stop\n");
	printf("\t==>destroy AP.\n");
}

int main(int argc, char *argv[])
{
	int ret;
	ApObjT *apObj = CreateNewAp("ra0");
	if(apObj == NULL) {
		LogE("Create new sta object fail");
		return NET_STA_NOTOPEN;
	}
	ret = apObj->On(apObj->handle);//on wifi
	if(ret < 0) {
		LogE("Wifi open fail");
		return NET_STA_NOTOPEN;
	}
	if(argc >= 2 && strcmp(argv[1], "start") == 0){
		if(argc == 3){
			printf("passwd:%s\n", argv[2]);
			apObj->turnOnAccessPoint(apObj->handle, NULL, argv[2], HOST_WPA2_AES);
			apObj->startUdhcpd(apObj->handle);
		} else if(argc > 3) {
			printf("ssid:%s\n", argv[2]);
			printf("passwd:%s\n", argv[3]);
			apObj->turnOnAccessPoint(apObj->handle, argv[2], argv[2], HOST_WPA2_AES);
			apObj->startUdhcpd(apObj->handle);
		}
	} else if( argc == 2 && strcmp(argv[1], "stop") == 0){
		apObj->stopUdhcpd(apObj->handle);
		apObj->turnOffAccessPoint(apObj->handle);
	} else {
		net_wifi_usage();
	}
	DestoryOldAp(apObj);
}
#endif
