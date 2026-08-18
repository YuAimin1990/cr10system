
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


#define DISABLE_WIRELESS_HANDLE
#include "net_wifi.h"
#undef DISABLE_WIRELESS_HANDLE
#include "util_api.h"
#include "util_timer.h"

#define DETECT_TIME		60   /* detection time is second */
#define CMD_LEN_MIN  	64
#define CMD_LEN_MAX  	128
#define MAC_LEN			6

#define SUPPORT_AP(n)		(n->private.supportAP)
#define SUPPORT_STA(n)		(n->private.supportSTA)

//Sta private data space
typedef struct WirelessManagePrivate_s {
	s8 ifname[IFNAMSIZ];
	s8 mac[18];
	s8 ip[16];
	s8 mask[16];
	s8 *deviceName;
	bool supportAP;
	bool supportSTA;
	NET_WIFI_WORK_StatusEnumT workMode;
	WirelessHandle objHandle;
} WirelessManagePrivateT;

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
	WirelessManagePrivateT	private;
} WirelessManageObjT;

extern int create_wpa_conf(const char *cfg_path, const char *ssid, const char *key);

static WirelessManageObjT *getObjectByHandle(WirelessHandle handle)
{
	return (WirelessManageObjT *)handle;
}

static s32 _wirelessProbe(WirelessHandle handle)
{
	s32 ret = 0;
	s8 *wifi_device = NULL;
	NET_WIFI_DeviceNumT device_info;
	NET_WIFI_SupportFuncT *supportFunc = NULL;

	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
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
		return NET_WIRELESS_BADPARAM;
	}
	wirelessManageObj->private.deviceName 	= supportFunc->deviceName;
	wirelessManageObj->private.supportAP	= supportFunc->supportAP;
	wirelessManageObj->private.supportSTA	= supportFunc->supportSTA;
	return NET_WIRELESS_OK;
}

static s32 _upWirelessNetwork(WirelessHandle handle)
{
	s8 *cmd = NULL;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	cmd = (char *)malloc(CMD_LEN_MIN);
	if(cmd == NULL) return NET_WIRELESS_OUTOFMEMORY;
	memset(cmd, 0, CMD_LEN_MIN);
	snprintf(cmd, CMD_LEN_MIN, "%s %s up", IFCONFIG_CMD, wirelessManageObj->private.ifname);
	cmd[CMD_LEN_MIN - 1] = '\0';
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	return NET_WIRELESS_OK;
}

static s32 _downWirelessNetwork(WirelessHandle handle)
{
	s8 *cmd = NULL;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	cmd = (char *)malloc(CMD_LEN_MIN);
	if(cmd == NULL) return NET_WIRELESS_OUTOFMEMORY;
	memset(cmd, 0, CMD_LEN_MIN);
	snprintf(cmd, CMD_LEN_MIN, "%s %s down", IFCONFIG_CMD, wirelessManageObj->private.ifname);
	cmd[CMD_LEN_MIN - 1] = '\0';
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	return NET_WIRELESS_OK;
}

static s32 FAST_FUNC _ioctl(u32 op, void *params)
{
	struct ifreq ifr;
	s32 skfd;
	s32 ret = 0;
	if(isNull(params)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0)
	{
		printf("socket create error\n");
		return -1;
	}

	if ((ret = ioctl(skfd, SIOCSIFADDR, &ifr)) != 0){
		perror("SIOCSIFADDR ?");
		close(skfd);
		return	-1;
	}
	close(skfd);

	return 0;
}

static s32 FAST_FUNC _ifreqAndIoctl(const s8 *ifname, u32 op, struct ifreq *ifr,
	struct in_addr indr)
{
	struct sockaddr_in *sin;
	if(isNull(ifname) || isNull(ifr)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	memset(ifr, 0, sizeof(struct ifreq));
    strcpy(ifr->ifr_name, ifname);
    sin = (struct sockaddr_in*) &(ifr->ifr_addr);
    sin->sin_addr = indr;
    sin->sin_family = AF_INET;
	return _ioctl(op, &ifr);
}

static s32 _setWirelessIP(WirelessHandle handle,  const s8 * ipaddr)
{
	struct ifreq ifr;
	struct in_addr addr;
    s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(ipaddr)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	addr.s_addr = inet_addr(ipaddr);
    ret = _ifreqAndIoctl(wirelessManageObj->private.ifname,
		SIOCSIFADDR, &ifr, addr);
	if(ret < 0) {
		return ret;
	} else {
		strncpy(wirelessManageObj->private.ip, ipaddr, sizeof(wirelessManageObj->private.ip));
		return NET_WIRELESS_OK;
	}
}

static s8 * FAST_FUNC _getIPv4ForIfreq(struct ifreq *ifr)
{
	struct sockaddr_in *sin;
	sin = (struct sockaddr_in *)&(ifr->ifr_addr);
	return inet_ntoa(sin->sin_addr);
}

static s32 _getWirelessIP(WirelessHandle handle,  s8 ** ipaddr)
{
	struct ifreq ifr;
	const s8 *ifname = NULL;
    s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(ipaddr)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}

	ifname = wirelessManageObj->private.ifname;
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);
	ret = _ioctl(SIOCGIFADDR, &ifr);
	if(ret < 0) {
		return ret;
	} else {
		*ipaddr = _getIPv4ForIfreq(&ifr);
		if (*ipaddr == NULL) {
			LogE("network address conevert to character, error.");
			return NET_WIRELESS_BADPARAM;
		}
		strncpy(wirelessManageObj->private.ip, *ipaddr, sizeof(wirelessManageObj->private.ip));
		*ipaddr = wirelessManageObj->private.ip;
		return NET_WIRELESS_OK;
	}
}

static s32 _setWirelessNetmask(WirelessHandle handle,  const s8 *netmask)
{
	struct ifreq ifr;
	struct in_addr addr;
    s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(netmask)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	addr.s_addr = inet_addr(netmask);
	ret = _ifreqAndIoctl(wirelessManageObj->private.ifname,
		SIOCSIFNETMASK, &ifr, addr);
	if(ret < 0) {
		return ret;
	} else {
		strncpy(wirelessManageObj->private.mask, netmask,
			sizeof(wirelessManageObj->private.mask));
		return NET_WIRELESS_OK;
	}
}

static s32 _getWirelessNetmask(WirelessHandle handle, s8 **netmask)
{
	struct ifreq ifr;
	const s8 *ifname = NULL;
    s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(netmask)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}

	ifname = wirelessManageObj->private.ifname;
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);
	ret = _ioctl(SIOCGIFADDR, &ifr);
	if(ret < 0) {
		return ret;
	} else {
		*netmask = _getIPv4ForIfreq(&ifr);
		if (*netmask == NULL) {
			LogE("network address conevert to character, error.");
			return NET_WIRELESS_BADPARAM;
		}
		strncpy(wirelessManageObj->private.mask, *netmask, sizeof(wirelessManageObj->private.mask));
		*netmask = wirelessManageObj->private.mask;
		return NET_WIRELESS_OK;
	}
}

static void FAST_FUNC _getMACForIfreq(struct ifreq *ifr, s8 *mac)
{
	s8 *pmac = ifr->ifr_hwaddr.sa_data;
	sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
		pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5]);
}

static s32 _getWirelessMAC(WirelessHandle handle, s8 **mac)
{
	struct ifreq ifr;
	const s8 *ifname = NULL;
    s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(mac)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}

	ifname = wirelessManageObj->private.ifname;
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);
	ret = _ioctl(SIOCGIFHWADDR, &ifr);
	if(ret < 0) {
		return ret;
	} else {
		_getMACForIfreq(&ifr, wirelessManageObj->private.mac);
		*mac = wirelessManageObj->private.mac;
		return NET_WIRELESS_OK;
	}
}

static s32 _setMacAddress(const s8 *ifname, s8 *mac)
{
	struct ifreq ifr;
	if(isNull(ifname) || isNull(mac)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, ifname, sizeof(ifr.ifr_ifrn.ifrn_name));
    strncpy(ifr.ifr_ifru.ifru_hwaddr.sa_data, mac, sizeof(ifr.ifr_ifru.ifru_hwaddr.sa_data));
	return _ioctl(SIOCSIFHWADDR, &ifr);
}


static s32 _setWirelessMAC(WirelessHandle handle, const s8 *mac)
{
#ifndef IFHWADDRLEN
#define IFHWADDRLEN	6
#endif
	u8 macbyte[14] = {0};
    s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(mac)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	memset(macbyte, 0, sizeof(macbyte));
	net_macstrtobyte(macbyte, mac, IFHWADDRLEN);
	_setMacAddress(wirelessManageObj->private.ifname, (char *)macbyte);
	if(ret < 0) {
		return ret;
	} else {
		strncpy(wirelessManageObj->private.mac, mac,
			sizeof(wirelessManageObj->private.mac));
		return NET_WIRELESS_OK;
	}
}

static WirelessHandle getObjHandleByHandle(WirelessHandle handle)
{
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NULL;
	}
	return wirelessManageObj->private.objHandle;
}

static s32 _setWirelessWorkMode(WirelessHandle handle, s32 mode)
{
	s32 ap_status = 0, sta_status = 0;
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(mode < NET_WIFI_NONE || mode > NET_WIFI_AP) {
		LogE("unknow mode %d", mode);
		return NET_STA_BADPARAM;
	}
	ap_status = net_wifi_check_driver_exist(DRIVER_AP);
	sta_status = net_wifi_check_driver_exist(DRIVER_STA);

	if((ap_status == 1) && (sta_status == 0))
		wirelessManageObj->private.workMode = NET_WIFI_AP;
	else if((ap_status == 0) && (sta_status == 1))
		wirelessManageObj->private.workMode = NET_WIFI_STA;
	else if((sta_status == 0) && (ap_status == 0))
		wirelessManageObj->private.workMode = NET_WIFI_NONE;
	// If the driver is the same, use default status.

	if((strlen(wirelessManageObj->private.ifname) <= 0)
		&& (wirelessManageObj->private.workMode != NET_WIFI_NONE)) {
		LogE("ifname is empty.");
		return -1;
	}

	if(mode == NET_WIFI_AP){
		if(SUPPORT_AP(wirelessManageObj)) {
			if(wirelessManageObj->private.workMode == NET_WIFI_STA) {
				StaObjT *staObj = (StaObjT *)(wirelessManageObj->getWirelessObject(handle));
				if(!isNull(staObj)) {
					staObj->turnOffWiFi(staObj->handle);
					DestoryOldSta(staObj);
					wirelessManageObj->private.objHandle = NULL;
				}
				/* If the driver is the same, is not unloaded */
				if(strcmp(DRIVER_STA, DRIVER_AP)) {
					wirelessManageObj->downWirelessNetwork(handle);
					net_wifi_unload_driver(DRIVER_STA);
				}
			}
			if(wirelessManageObj->private.workMode != NET_WIFI_AP) {
				/* If the driver is the same and already loaded, is not load */
				if(wirelessManageObj->private.workMode == NET_WIFI_STA
					&& strcmp(DRIVER_STA, DRIVER_AP)) {
					net_wifi_load_driver(DRIVER_AP);
				}
			}
			wirelessManageObj->private.objHandle = (WirelessHandle)CreateNewAp(wirelessManageObj->private.ifname);
			if(wirelessManageObj->private.objHandle == NULL) {
				wirelessManageObj->private.workMode = NET_WIFI_NONE;
				return -1;
			}
		}
		else
		{
			return NET_STA_NOSUPPORT;
		}
	}

	if(mode == NET_WIFI_STA){
		if(SUPPORT_STA(wirelessManageObj)) {
			if(wirelessManageObj->private.workMode == NET_WIFI_AP) {
				ApObjT *apObj = (ApObjT *)(wirelessManageObj->getWirelessObject(handle));
				if(!isNull(apObj)) {
					apObj->turnOffAccessPoint(apObj->handle);
					DestoryOldAp(apObj);
					wirelessManageObj->private.objHandle = NULL;
				}
				/* If the driver is the same, is not unloaded */
				if(strcmp(DRIVER_STA, DRIVER_AP)) {
					wirelessManageObj->downWirelessNetwork(handle);
					net_wifi_unload_driver(DRIVER_AP);
				}
			}
			if(wirelessManageObj->private.workMode != NET_WIFI_STA) {
				/* If the driver is the same and already loaded, is not load */
				if(wirelessManageObj->private.workMode == NET_WIFI_AP
					&& strcmp(DRIVER_STA, DRIVER_AP)) {
					net_wifi_load_driver(DRIVER_STA);
				}
			}
			wirelessManageObj->private.objHandle = (WirelessHandle)CreateNewSta(wirelessManageObj->private.ifname);
			if(wirelessManageObj->private.objHandle == NULL) {
				wirelessManageObj->private.workMode = NET_WIFI_NONE;
				return -1;
			}
			LogI("wirelessManageObj->private.objHandle :0x%p",
				wirelessManageObj->private.objHandle);
		}
		else
		{
			return NET_STA_NOSUPPORT;
		}
	}

	if(mode == NET_WIFI_NONE && wirelessManageObj->private.workMode != NET_WIFI_NONE){
		if(wirelessManageObj->private.workMode == NET_WIFI_AP) {
			ApObjT *apObj = (ApObjT *)(wirelessManageObj->getWirelessObject(handle));
			if(!isNull(apObj)) {
				DestoryOldAp(apObj);
				wirelessManageObj->private.objHandle = NULL;
			}
		} else if(wirelessManageObj->private.workMode == NET_WIFI_STA) {
			StaObjT *staObj = (StaObjT *)(wirelessManageObj->getWirelessObject(handle));
			if(!isNull(staObj)) {
				DestoryOldSta(staObj);
				wirelessManageObj->private.objHandle = NULL;
			}
		}
	}
	wirelessManageObj->private.workMode = mode;
	return 0;
}

static s32 _getWirelessWorkMode(WirelessHandle handle)
{
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	return wirelessManageObj->private.workMode;
}

static s32 _getCurrentInface(WirelessHandle handle, s8 **pIfname)
{
	WirelessManageObjT *wirelessManageObj = getObjectByHandle(handle);
	if(isNull(wirelessManageObj)) {
		LogE("invalid handle.");
		return NET_WIRELESS_INVALIDHANDLE;
	}
	if(isNull(pIfname)) {
		LogE("bad params.");
		return NET_WIRELESS_BADPARAM;
	}
	*pIfname = wirelessManageObj->private.ifname;
	return NET_WIRELESS_OK;
}

WirelessManageObjT *CreateNewWirelessManage(const char *ifname)
{
	s32 ret = 0;
	WirelessManageObjT *wirelessManageObj = malloc(sizeof(WirelessManageObjT));
	if(wirelessManageObj == NULL) {
		LogE("Memory is not enough.");
		return NULL;
	}
	memset(wirelessManageObj, 0, sizeof(WirelessManageObjT));
	wirelessManageObj->handle = (WirelessHandle)wirelessManageObj;
	wirelessManageObj->getCurrentInterface 	= _getCurrentInface;
	wirelessManageObj->getWirelessObject	= getObjHandleByHandle;
	wirelessManageObj->upWirelessNetwork 	= _upWirelessNetwork;
	wirelessManageObj->downWirelessNetwork 	= _downWirelessNetwork;
	wirelessManageObj->setWirelessWorkMode  = _setWirelessWorkMode;
	wirelessManageObj->getWirelessWorkMode  = _getWirelessWorkMode;
	wirelessManageObj->getWirelessMAC		= _getWirelessMAC;
	wirelessManageObj->setWirelessMAC		= _setWirelessMAC;
	wirelessManageObj->getWirelessMask		= _getWirelessNetmask;
	wirelessManageObj->setWirelessMask		= _setWirelessNetmask;
	wirelessManageObj->getWirelessIP		= _getWirelessIP;
	wirelessManageObj->setWirelessIP		= _setWirelessIP;
	wirelessManageObj->private.workMode 	= NET_WIFI_NONE;
	wirelessManageObj->private.objHandle 	= NULL;
	if(isNull(ifname)) {
		ret = net_wifi_findInterFace(wirelessManageObj->private.ifname,
			sizeof(wirelessManageObj->private.ifname));
		if(ret < 0) {
			free(wirelessManageObj);
			return NULL;
		}
	} else {
 		strncpy(wirelessManageObj->private.ifname, ifname, sizeof(wirelessManageObj->private.ifname));
	}
	//wireless probe
	ret = _wirelessProbe(wirelessManageObj->handle);
	if(ret != NET_WIRELESS_OK) {
		free(wirelessManageObj);
		return NULL;
	}
	return wirelessManageObj;
}

void DestoryOldWirelessManage(WirelessManageObjT *wirelessManageObj)
{
	if(isNull(wirelessManageObj)) {
		LogE("bad params.");
		return;
	}
	if(wirelessManageObj->private.workMode == NET_WIFI_STA) {
        DestoryOldSta((StaObjT *)wirelessManageObj->private.objHandle);
	} else if(wirelessManageObj->private.workMode == NET_WIFI_AP) {
		DestoryOldAp((ApObjT *)wirelessManageObj->private.objHandle);
    }
    free(wirelessManageObj);
}


static int is_get_list = 0;
static int scan_count = 0;
static WIFI_SCAN_LIST_t g_scan_list[20];

static char *ap_driver = NULL;
static char *sta_driver = NULL;
static char *ifname = NULL;
static E_IPC_WIFI_MODE g_wifi_mode = IPC_WIFI_MODE_NONE;

int ipc_get_wifi_ifname(char *name)
{
    strncpy(name, ifname, strlen(ifname));
    return 0;
}

void ipc_interface_ctrl(const char *ifname, int on)
{
	char cmd[128] = {0};
    memset(cmd, 0, sizeof(cmd));
    if (on)
	    snprintf(cmd, sizeof(cmd), "ifconfig %s up", ifname);
    else
        snprintf(cmd, sizeof(cmd), "ifconfig %s down", ifname);
    printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
}


int ipc_wifi_start_wpa_supplicant(const char *dev, const char *driver_name, unsigned char usedhcp, const char *ssid, const char *key, int create_file)
{
    if (create_file) {
        int ret;
    	ret = create_wpa_conf(PASSWD_CFG, ssid, key);
        if(ret < 0) {
            PRINT_ERR("create_wpa_conf error\n");
            return -1;
        }
    }

    char cmd[128] = {0};

    /* start wpa_suppliant */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -B -D%s -i%s -c %s &", WPA_SUPPLICANT_CMD, driver_name, dev, PASSWD_CFG);
    PRINT_INFO("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    if(usedhcp){
        memset(cmd, 0, sizeof(cmd));
        //sprintf(cmd,"%s -b -i%s", UDHCPC_CMD, dev);
        //udhcpc -i wlan0 -b -A 8
        sprintf(cmd, "%s -i %s &", UDHCPC_CMD, dev);
        PRINT_INFO("cmd:%s\n", cmd);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    }

    return 0;
}

static int ipc_wpa_cli_connect(const char *dev, const char *ssid, const char *key)
{
    int ret = 0;
    char cmd[128];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s add_network", WPA_CLI_CMD, dev);
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s set_network 0 ssid '\"%s\"'", WPA_CLI_CMD, dev, ssid);
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    if (key) {
        snprintf(cmd, sizeof(cmd),"%s -i%s set_network 0 psk '\"%s\"'", WPA_CLI_CMD, dev, key);
    } else {
        //wpa_cli -i wlan0 set_network 0 key_mgmt NONE
        snprintf(cmd, sizeof(cmd),"%s -i%s set_network 0 key_mgmt NONE", WPA_CLI_CMD, dev);
    }
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s set_network 0 priority 2", WPA_CLI_CMD, dev);
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s set_network 0 scan_ssid 1", WPA_CLI_CMD, dev);
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s enable_network 0", WPA_CLI_CMD, dev);
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s select_network 0", WPA_CLI_CMD, dev);
    //PRINT_INFO("cmd:%s\n", cmd);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    return 0;
}

int ipc_wpa_cli_save(const char *dev)
{
    int ret = 0;
    char cmd[128];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -i%s save_config", WPA_CLI_CMD, dev);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    if (ret < 0) {
        PRINT_ERR();
        return -1;
    }

    return 0;
}

int ipc_wifi_start_wpa_supplicant2(const char *dev, const char *driver_name, unsigned char usedhcp, const char *ssid, const char *key, int create_file)
{
    if (create_file) {
        int ret;
    	ret = create_wpa_conf(PASSWD_CFG, NULL, NULL);
        if(ret < 0) {
            PRINT_ERR("create_wpa_conf error\n");
            return -1;
        }
    }

    char cmd[128] = {0};

    /* start wpa_suppliant */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),"%s -B -D%s -i%s -c %s &", WPA_SUPPLICANT_CMD, driver_name, dev, PASSWD_CFG);
    //PRINT_INFO("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    #if 0
    sleep(1);
    system("ifconfig");
    sleep(1);
    system("ps");
    sleep(1);
    #else
    sleep(2);
    #endif

    int ret = ipc_wpa_cli_connect(dev, ssid, key);
    if (ret < 0) {
        PRINT_ERR("ipc_wpa_cli_connect error\n");
        return -1;
    }

    if(usedhcp){
        memset(cmd, 0, sizeof(cmd));

        //udhcpc -b -i wlan0 -A 8
        //udhcpc -b -i wlan0 -q ( –q:获得续约后退出)
        //udhcpc -i wlan0 &
        sprintf(cmd, "%s -b -i %s -q", UDHCPC_CMD, dev);
        //PRINT_INFO("cmd:%s\n", cmd);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    }

    return 0;
}


int ipc_wifi_start_ap()
{
    char cmd[128] = {0};

    snprintf(cmd, sizeof(cmd), "ifconfig %s %s up", ifname, DRIVER_MT7601_AP_DEF_IP);
    printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "hostapd %s &", HOSTAPD_CFG);
    printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    //udhcpd /opt/custom/wifi/udhcpd.conf &
    //killall hostapd ifconfig wlan0 up
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "udhcpd %s &", UDHCPD_CFG_PATH);
    printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    g_wifi_mode = IPC_WIFI_MODE_AP;

	return 0;
}


int ipc_wifi_stop_ap()
{
    IPC_MID_UTIL_Killall("udhcpd");

    IPC_MID_UTIL_Killall("hostapd");

    char cmd[128] = {0};
	snprintf(cmd, sizeof(cmd), "ifconfig %s down", ifname);
	printf("cmd:%s\n", cmd);
	IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    sleep(1);

	g_wifi_mode = IPC_WIFI_MODE_NONE;

	return 0;
}

static pthread_mutex_t m_ap_list = PTHREAD_MUTEX_INITIALIZER;

int ipc_is_scan_ready()
{
    pthread_mutex_lock(&m_ap_list);
    if (is_get_list == 1 && scan_count > 0) {
        pthread_mutex_unlock(&m_ap_list);
        return 1;
    } else {
        pthread_mutex_unlock(&m_ap_list);
        return 0;
    }

}

int ipc_get_scan_list(WIFI_SCAN_LIST_t *list)
{
    pthread_mutex_lock(&m_ap_list);
    int count = 0;
    if (is_get_list == 1 && scan_count > 0) {
        memcpy(list, g_scan_list, sizeof(g_scan_list));
        count = scan_count;
    }  else {
        count = 0;
    }
    pthread_mutex_unlock(&m_ap_list);
    return count;
}

#if 0
void ipc_wifi_wpa_scanning()
{
	int ret;
	WIFI_SCAN_LIST_t list[20];
	int count = 20;

    if (ipc_is_scan_ready() == 1)
    {
        //PRINT_ERR("already get scan list!\n");
        return;
    }

    int i;
    int try_counts = 5;
    for(i = 0; i < try_counts; i ++)
    {
        #if 0
        printf("wpa_cli -p /tmp/wpa_supplicant/ -i ra0 scan\n");
        ret = net_wifi_cli_scan(ifname);
        if(ret !=0)
        {
            PRINT_ERR("wifi scan failed!\n");
            return;
        }
        #endif

        //printf("call wpa_cli -p /var/run/wpa_supplicant/ -i ra0 scan_results\n");
        //system("wpa_cli -p /var/run/wpa_supplicant/ -i ra0 scan_results\n");

        memset(list, 0, sizeof(list));
        ret = net_wifi_cli_scan_result(ifname, list, &count);
        if (ret !=0 || count <= 0)
        {
            PRINT_ERR("net_wifi_cli_scan_result failed!, try=%d\n", i);
            sleep(1);
            continue;
        }

        printf("wpa scan_results, count=%d, try=%d\n", count, i);

#if 1
        printf("index mac                 channel   quality  security     ssid\n");
        int j;
        for (j = 0; j < count; j ++)
        {
            printf("%2d  %s      %2d      %3d      %2d      %s     \n",
                j,
                list[j].mac,
                list[j].channel,
                list[j].quality,
                list[j].security,
                list[j].essid);
        }
#endif
        pthread_mutex_lock(&m_ap_list);
        memcpy(g_scan_list, list, sizeof(list));
        is_get_list = 1;
        scan_count = count;
        pthread_mutex_unlock(&m_ap_list);

        ret = net_wifi_cli_status(ifname);
        PRINT_INFO("after scan results, status=%d\n", ret);
        break;
    }

	return;
}
#endif

static int scan_times = 0;

void ipc_wifi_iwlist_scan_once()
{
	int ret;
	NETCAM_WIFI_SCAN_LIST st;
    int i;
    int try_counts = 5;
    memset(&st, 0, sizeof(st));
    for(i = 0; i < try_counts; i ++)
    {
        //ret = uti_wifi_scan_ap(ifname, &st);
        ret = IPC_MID_UTIL_WifiScanAP(ifname, &st);
        if (ret !=0 || st.count <= 0)
        {
            PRINT_ERR("net_wifi_cli_scan_result failed!, try=%d\n", i);
            sleep(1);
            continue;
        }
        //printf("wpa scan_results, count=%d, try=%d\n", st.count, i);

        //IPC_MID_UTIL_PrintAPList(&st);
        IPC_MID_UTIL_SortAPList(&st);
        //IPC_MID_UTIL_PrintAPList(&st);

        pthread_mutex_lock(&m_ap_list);
        memcpy(g_scan_list, st.list, sizeof(st.list));
        is_get_list = 1;
        scan_count = st.count;
        scan_times ++;
        pthread_mutex_unlock(&m_ap_list);
        break;
    }

    return;
}


void ipc_wifi_iwlist_scanning()
{
    #if 0
    if (ipc_is_scan_ready() == 1)
    {
        //PRINT_ERR("already get scan list!\n");
        return;
    }
    #endif

    /* iwlist 次数多了会出错 */
    if ((scan_times > 2) && (scan_count > 0))
    {
        return;
    }

    ipc_wifi_iwlist_scan_once();

	return;
}


int ipc_wifi_timer_scan_start()
{
    pthread_mutex_lock(&m_ap_list);
    //ipc_wifi_wpa_connect_no_ssid(ifname);
    memset(g_scan_list, 0, sizeof(g_scan_list));
    is_get_list = 0;
    scan_count = 0;
    pthread_mutex_unlock(&m_ap_list);
    IPC_MID_UTIL_TIMER_AddTask(ipc_wifi_iwlist_scanning, 8, TIMER_TRUE, TIMER_FALSE);  // 4*250ms = 1000ms

    return 0;
}

int ipc_wifi_timer_scan_stop()
{
    IPC_MID_UTIL_TIMER_DelTask(ipc_wifi_iwlist_scanning);
    //ipc_wifi_wpa_disconnect(ifname);
    return 0;
}


#if 0
int ipc_wifi_once_scan_start()
{
    pthread_mutex_lock(&m_ap_list);
    //ipc_wifi_wpa_connect_no_ssid(ifname);
    memset(g_scan_list, 0, sizeof(g_scan_list));
    is_get_list = 0;
    scan_count = 0;
    pthread_mutex_unlock(&m_ap_list);
    ipc_wifi_iwlist_scanning();

    return 0;
}


int ipc_wifi_once_scan_stop()
{
    //ipc_wifi_wpa_disconnect(ifname);
    return 0;
}

#endif


int create_wpa_conf(const char *cfg_path, const char *ssid, const char *key)
{
    int fd = -1;
    char buff[10240] = {0};
    if(cfg_path == NULL)
    {
        PRINT_ERR("param error\n");
        return -1;
    }

    /* clean configuration file */
    if(0 == access(cfg_path, F_OK)){
        PRINT_INFO("remove file:%s\n", cfg_path);
		remove(cfg_path);
	}
    /* open configuration file */
	fd = open(cfg_path, O_RDWR | O_CREAT, 0666);
	if(fd < 0){
		PRINT_ERR("open %s failed\n", cfg_path);
		return -1;
	}
    PRINT_INFO("open file:%s\n", cfg_path);

    memset(buff, 0, sizeof(buff));
    char *p = buff;

    if (ssid) {
        sprintf(p, "ctrl_interface=%s\r\n", WPA_CTL_INTERFACE);
    	p += strlen(p);
    	sprintf(p, "update_config=1\r\n\r\n");
    	p += strlen(p);
    	sprintf(p, "network={\r\n");
    	p += strlen(p);
    	sprintf(p, "  ssid=\"%s\"\r\n", ssid);
    	p += strlen(p);
    	sprintf(p, "  psk=\"%s\"\r\n", key);
    	p += strlen(p);
        #if 0
    	sprintf(p, "  key_mgmt=WPA-PSK\r\n");
    	p += strlen(p);
    	sprintf(p, "  proto=WPA\r\n");
    	p += strlen(p);
        #endif
    	sprintf(p, "}\r\n");
        p += strlen(p);
    } else {
        sprintf(p, "ctrl_interface=%s\r\n", WPA_CTL_INTERFACE);
    	p += strlen(p);
    	sprintf(p, "update_config=1\r\n\r\n");
    	p += strlen(p);
    }

    write(fd, buff, strlen(buff));
    //printf("buff:%s\n", buff);
    close(fd);
    PRINT_INFO("close file:%s, buf_len=%ld, len=%ld\n", cfg_path, strlen(buff), p - buff);

    return 0;

}

int ipc_wifi_wpa_disconnect(const char *ifname)
{
    net_wifi_cli_disconnect((const WIFI_HANDLE)ifname);

    IPC_MID_UTIL_Killall("wpa_supplicant");

    return 0;
}

int ipc_wifi_stop_sta()
{
    IPC_MID_UTIL_Killall("udhcpc");

    ipc_wifi_wpa_disconnect(ifname);

    char cmd[128] = {0};
    snprintf(cmd, sizeof(cmd), "ifconfig %s down", ifname);
    printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    g_wifi_mode = IPC_WIFI_MODE_NONE;

    return 0;
}

int ipc_wifi_wpa_connect(const char* ifname, const char *ssid, const char *key, unsigned char usedhcp)
{
	if(isNull(ifname))
		return -1;

	int ret = 0;
    #if 0
    ret = net_wifi_cli_disconnect(ifname);
    if(ret < 0){
        PRINT_ERR("net_wifi_cli_disconnect error\n");
        //return -1;
    }

    ret = net_wifi_cli_status(ifname);
    PRINT_INFO("before start wpa_supplicant, status=%d\n", ret);
    #endif

    /* start wpa_suppliant */
    ipc_wifi_start_wpa_supplicant(ifname, "nl80211", usedhcp, ssid, key, 1);
    sleep(1);

    int try_times = 0;
    while (1) {
        ret = net_wifi_cli_status((const WIFI_HANDLE)ifname);
        //PRINT_INFO("after start wpa_supplicant, status=%d\n", ret);
    	if (ret <= 0) {
            PRINT_ERR("net_wifi_cli_status error=%d, try_times=%d\n", ret, try_times);
            try_times ++;
            if (try_times == 10) {
                PRINT_ERR("can not connect net, try %d times, ssid:%s, key:%s\n", try_times, ssid, key);
                return -1;
            }

            ipc_wifi_stop_sta();
    		//ipc_interface_ctrl(ifname, 0);
            usleep(200*1000);
            ipc_interface_ctrl(ifname, 1);
            usleep(200*1000);
            ipc_wifi_start_wpa_supplicant(ifname, "nl80211", usedhcp, ssid, key, 0);
            usleep(400*1000);

    	} else if (ret == WIFI_STATUS_CONNECTED){
    	    PRINT_INFO("WIFI_STATUS_CONNECTED\n");
    	    g_wifi_mode = IPC_WIFI_MODE_STA;
            break;
        } else {
            PRINT_ERR("net_wifi_cli_status error=%d, try_times=%d\n", ret, try_times);
            try_times ++;
            if (try_times == 10) {
                PRINT_ERR("can not connect net, try %d time, ssid:%s, key:%s\n", try_times, ssid, key);
                return -1;
            }
            sleep(1);
        }
    }

    return 0;
}

int ipc_wifi_wpa_connect2(const char* ifname, const char *ssid, const char *key, unsigned char usedhcp)
{
	if(isNull(ifname))
		return -1;

	int ret = 0;
    #if 0
    ret = net_wifi_cli_disconnect(ifname);
    if(ret < 0){
        PRINT_ERR("net_wifi_cli_disconnect error\n");
        //return -1;
    }

    ret = net_wifi_cli_status(ifname);
    PRINT_INFO("before start wpa_supplicant, status=%d\n", ret);
    #endif

    /* start wpa_suppliant */
    ipc_wifi_start_wpa_supplicant2(ifname, "nl80211", usedhcp, ssid, key, 1);
    //sleep(1);

    int try_times = 0;
    while (1) {
        ret = net_wifi_cli_status((const WIFI_HANDLE)ifname);
        //PRINT_INFO("after start wpa_supplicant, status=%d\n", ret);
    	if (ret <= 0) {
            PRINT_ERR("net_wifi_cli_status error=%d, try_times=%d\n", ret, try_times);
            try_times ++;
            if (try_times == 10) {
                PRINT_ERR("can not connect net, try %d times, ssid:%s, key:%s\n", try_times, ssid, key);
                return -1;
            }

            ipc_wifi_stop_sta();
    		//ipc_interface_ctrl(ifname, 0);
            usleep(200*1000);
            ipc_interface_ctrl(ifname, 1);
            usleep(200*1000);
            ipc_wifi_start_wpa_supplicant2(ifname, "nl80211", usedhcp, ssid, key, 0);
            usleep(400*1000);

    	} else if (ret == WIFI_STATUS_CONNECTED){
    	    PRINT_INFO("WIFI_STATUS_CONNECTED\n");
    	    g_wifi_mode = IPC_WIFI_MODE_STA;
            break;
        } else {
            PRINT_ERR("net_wifi_cli_status error=%d, try_times=%d\n", ret, try_times);
            try_times ++;
            if (try_times == 10) {
                PRINT_ERR("can not connect net, try %d time, ssid:%s, key:%s\n", try_times, ssid, key);
                return -1;
            }
            sleep(1);
        }
    }

    return 0;
}

int ipc_wifi_start_sta(const char* ifname, const char *ssid, const char *key)
{
	char cmd[128] = {0};

	snprintf(cmd, sizeof(cmd), "ifconfig %s up", ifname);
	printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    int ret = ipc_wifi_wpa_connect(ifname, ssid, key, 1);
    if (ret < 0)
    {
        PRINT_ERR("ipc_wifi_wpa_connect failed\n");
        return -1;
    }

	return 0;
}

void ipc_init_mt7601()
{
	ap_driver = DRIVER_MT7601_AP;
	sta_driver = DRIVER_MT7601_STA;
	ifname = MT7601_INAME;
}



