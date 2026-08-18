#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#ifndef CONFIG_BIT_32BITS
#include <net/if.h>
#endif
#include "util_api.h"
#include "net_common.h"

//#define NO_LOCK
#define CMD_LEN_MIN  	64
#define CMD_LEN_MAX  	128
#define MAC_LEN			6

static const char dev_name_arry[2][8] = {"ra", "wlan"};


NET_WIFI_MapVenderNameT MapVenderList[] =
{
	{0x1d6b, "Linux"},//linux foundation, Hub
	{0x148f, "Ralink"},
	{0x0bda, "Realtek"},
};

NET_WIFI_MapDeviceNameT MapDeviceList[] =
{
	{{0x148f, 0x7601}, NET_WIFI_DEVICENAME_MT7601},
	{{0x0bda, 0x8179}, NET_WIFI_DEVICENAME_RTL8188},
};

/* global wifi device support function list */
NET_WIFI_SupportFuncT gWifiDeviceSupportFuncList[] = {
//mt7601 support
{
	.deviceName = NET_WIFI_DEVICENAME_MT7601,
	.supportAP 	= true,
	.supportSTA = true,
	.supportWPS = true,
	.supportSmartlink 	= true,
	.custom = {
		{NET_WIFI_LABEL_APCONFIG, NET_WIFI_VAULE_MT7601_AP},
		{NET_WIFI_LABEL_STACONFIG, NET_WIFI_VAULE_MT7601_STA},
		{NULL, NULL}
	}
},
//rtl8188 support
{
	.deviceName = NET_WIFI_DEVICENAME_RTL8188,
	.supportAP	= true,
	.supportSTA = true,
	.supportWPS = true,
	.supportSmartlink	= false,
	.custom = {
		{NULL, NULL}
	}
}
};

#if 0
static s32 FAST_FUNC _system(const s8 *cmd)
{
    FILE *fp = NULL;
    s8 buffer[CMD_LEN_MAX];

    if (NULL == (fp = popen(cmd, "r"))) {
        goto err;
    }
    if (NULL == fgets(buffer, sizeof(buffer), fp)) {
        goto err;
    }
    pclose(fp);
    return 0;
err:
	pclose(fp);
    return -1;
}
#endif

int _wifi_cli_getshellresult(char *cmd, char *buffer, int buffsize)
{
	FILE *fp = NULL;

    if (NULL == (fp = popen(cmd, "r")))
    {
        goto err;
    }
    if (NULL == fgets(buffer, buffsize, fp))
    {
        goto err;
    }
    pclose(fp);
	printf(">>%s\n", buffer);
    return 0;
err:
	pclose(fp);
    return -1;
}

static s32 FAST_FUNC _is_wifi_dev2(const s8 *ifname)
{
	int ret = -1;
	char cmd[CMD_LEN_MAX] = {0};
	if(isNull(ifname))
		return 0;
	snprintf(cmd, sizeof(cmd),"%s -a|%s -e \"s/^\\w*/&<</\" -e \"s/<<.*//\"|%s %s",
			IFCONFIG_CMD, SED_CMD, GREP_CMD, ifname);
	ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	if(ret == 0)
		return 1;
	else
		return 0;
}

/* get wifi device name, if no success this is not wifi device.*/
static s32 FAST_FUNC _is_wifi_dev(const s8 *ifname)
{
	s32 ret, s;
	struct ifreq req;
	if(ifname == NULL){
		//perror("net wifi\n");
		return 0;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0 );
	if (s < 0) {
		//perror("Cannot open socket");
		return 0;
	}
	strncpy(req.ifr_name, ifname, sizeof(req.ifr_name));
/* solution net/if.h and linux/if.h conflict */
#ifndef SIOCGIWNAME
#define SIOCGIWNAME	0x8B01		/* get name == wireless protocol */
#endif
	ret = ioctl(s, SIOCGIWNAME, &req);
	if (ret < 0) {
		close(s);
		//fprintf(stderr, "No wireless extension\n");
		return 0;
	}
	close(s);
	return 1;
}

/* get all wifi dev name, list len max is DEF_MAX_LIST=10 */
/* If the search to multiple wireless devices, defualt use frist device. */
/* if need look all wireless devices, please use net_wifi_get_wifidevlist(). */
static s32 FAST_FUNC _wifi_get_wifidev(s8 *ifname, s32 len, s32 index)
{
#define DEF_MAX_LIST 10
	s32 i=0, j=0, k = 0;
	s32 sockfd;
	s32 ret = 0;
    s8 buff[CMD_LEN_MAX] ={0};
	struct ifconf ifconf;
	u8 buf[sizeof (struct ifreq) * DEF_MAX_LIST];
	struct ifreq *ifreq;
	//init recv buff
	ifconf.ifc_len = sizeof (struct ifreq) * DEF_MAX_LIST;
	ifconf.ifc_buf = (char *)buf;

	if(ifname == NULL || len <= 0){
		printf("bad params\n");
		return -1;
	}
	memset(buf, 0, sizeof (struct ifreq) * DEF_MAX_LIST);
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket open failure!\n" );
		return -1;
	}
	ret = ioctl(sockfd, SIOCGIFCONF, &ifconf); //get all dev name
	if(ret){
		perror("[SIOCGIFCONF]");
		close(sockfd);
		return -1;
	}
	close(sockfd);
	//loop seclect dev name
	ifreq = (struct ifreq*)buf;
	for (i = 0; i < DEF_MAX_LIST; i++)
	{
		if(ifreq->ifr_flags == AF_INET)
		{
			if(_is_wifi_dev(ifreq[i].ifr_name)){
				if(j == index){
					strncpy(ifname, ifreq[i].ifr_name, len-1);
					return 1;
				} else {
					j++;
				}
			}
		}
	}

    /* if network device not enable, mannul detect device name */
    for(i=0, k=0; i < 2; i++)
    {
        for(j=0; j < 2; j++){
            memset(buff, 0, sizeof(buff));
			sprintf(buff, "%s%d",dev_name_arry[i], j);
            if(_is_wifi_dev2(buff)){
                snprintf(ifname, len, "%s%d", dev_name_arry[i], j);
                return 1;
            } else {
				k++;
			}
        }
    }

	return 0;
}

s32 net_wifi_findInterFace(s8 *buffer, s32 len)
{
	s32 ret = 0;
	if(isNull(buffer) || len <= 0) {
		LogE("invalid handle.");
		return -1;
	}
	ret = _wifi_get_wifidev(buffer, sizeof(len), 0);
	if(ret <= 0){
	   return -1;
	}
	return 0;
}

NET_WIFI_SupportFuncT *net_wifi_getDeviceSupportFunc(const s8 *deviceName)
{
	s32 i;
	if(isNull(deviceName)) {
		LogE("getDeviceSupportFunc bad params. deviceName is NULL.");
		return NULL;
	}
	for(i = 0;
		i < sizeof(gWifiDeviceSupportFuncList)/sizeof(NET_WIFI_SupportFuncT);
		i++) {
		if(strcmp(gWifiDeviceSupportFuncList[i].deviceName, deviceName) == 0) {
			return &(gWifiDeviceSupportFuncList[i]);
		}
	}

	return NULL;
}


s32 net_wifi_getUsbDeviceID(NET_WIFI_DeviceNumT *device, s32 bus, s32
deviceNum)
{
	s32 ret = 0;
	s8 buff[32] = {0};
	s8 cmd[CMD_LEN_MAX] = {0};
	snprintf(cmd, sizeof(cmd),
		"%s |%s \"Bus %03x Device %03x\"|%s -e \"2d\" -e \"s/^.*ID //\"",
		LSUSB_CMD, GREP_CMD, bus, deviceNum, SED_CMD);

	ret = _wifi_cli_getshellresult(cmd, buff, sizeof(buff));
	if(ret != 0) {
		return ret;
	}
	sscanf(buff, "%04x:%04x", &(device->venderID), &(device->deviceID));
	return 0;
}

s32 net_wifi_getVenderNameByDeviceNum(NET_WIFI_DeviceNumT device, s8 **pName)
{
	s32 i;
	for(i = 0; i < (sizeof(MapVenderList)/sizeof(NET_WIFI_MapVenderNameT)); i++) {
		if(device.venderID == MapVenderList[i].venderID) {
			*pName = MapVenderList[i].venderName;
			return 0;
		}
	}
	return -1;
}

s32 net_wifi_getDeviceNameByDeviceNum(NET_WIFI_DeviceNumT device, s8 **pName)
{
	s32 i;
	for(i = 0; i < sizeof(MapDeviceList)/sizeof(NET_WIFI_MapDeviceNameT); i++) {
		if(device.venderID == MapDeviceList[i].deviceNum.venderID &&
			device.deviceID == MapDeviceList[i].deviceNum.deviceID) {
			*pName = MapDeviceList[i].deviceName;
			return 0;
		}
	}
	return -1;
}

static s32 _read_line(s32 fd, s8 *buf, s32 size)
{
	s32 i;
	for(i = 0; i < size; i++) {
		if(read(fd, buf+i, 1) <= 0){
			return i;
		}
		if(buf[i] == '\n')
			break;
	}
	return (i+1);
}
static s32 FAST_FUNC _wifi_check_file_iscontainstr(const s8* path, const s8 *str)
{
	s32 fd = -1;
	s8 buf[256] = {0};
	if(isNull(path) || isNull(str))
		return -1;
	fd = open(path, O_RDONLY);
	if(fd < 0){
		return -1;
	}
	while(_read_line(fd, buf, sizeof(buf)) > 0) {
		if(NULL != strstr(buf, str)){
			close(fd);
			return 1;
		}
		memset(buf, 0, sizeof(buf));
	}
	close(fd);
	return 0;
}

s32 net_wifi_check_driver_exist(const s8 *driver_name)
{
	s32 ret = 0;
	if(isNull(driver_name))
		return 0;
	ret = _wifi_check_file_iscontainstr("/proc/modules", driver_name);
	printf("file:%s ret:%d\n", "/proc/modules", ret);
	if(ret == 1)
		return 1;
	else
		return 0;
}

s32 net_wifi_load_driver(const s8 *driver_name)
{
	s32 ret = 0;
	s8 cmd[CMD_LEN_MAX] = {0};
	if(isNull(driver_name))
		return -1;
	s32 len = strlen(driver_name);
	if((len >= 3) && (strcmp(driver_name + len-3, ".ko") == 0)){
		snprintf(cmd, sizeof(cmd),"%s / -name \"%s\"|%s '2d'|%s %s",
			FIND_CMD, driver_name, SED_CMD, XARGS_CMD, INSMOD_CMD);
	} else {
		snprintf(cmd, sizeof(cmd),"%s / -name \"%s.ko\"|%s '2d'|%s %s",
			FIND_CMD, driver_name, SED_CMD, XARGS_CMD, INSMOD_CMD);
	}
	ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	return ret;
}

s32 net_wifi_unload_driver(const s8 *driver_name)
{
	s32 ret;
	s8 cmd[CMD_LEN_MAX] = {0};
	if(isNull(driver_name))
		return -1;
	snprintf(cmd, sizeof(cmd),"%s %s", RMMOD_CMD, driver_name);
	ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	return ret;
}

