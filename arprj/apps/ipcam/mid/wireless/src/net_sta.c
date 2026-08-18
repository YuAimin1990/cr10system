#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>


#define DISABLE_STA_HANDLE
#include "util_net.h"
#include "util_api.h"
#include "net_sta.h"
#include "net_utils.h"
#include "net_wpactl.h"
#undef	DISABLE_STA_HANDLE


#define CMD_LEN_MIN  	64
#define CMD_LEN_MAX  	128
#define MAC_LEN			6

#define MAC_EMPTY       "00:00:00:00:00:00"


#define UDHCPC_PID		"/tmp/udhcpc_wifi.pid"
#define WIFI_SCAN_ORDER_DBM  			3
//max scanning times at connecting
#define NET_STA_MAX_SCANNING_TIMES 		2

#define SUPPORT_WPS(n)			(n->private.supportWPS)
#define SUPPORT_SMARTLINK(n)	(n->private.supportSmartlink)

#ifdef NETWORK_ELIAN_SUPPORT
#ifndef SIOCIWFIRSTPRIV
#define	SIOCIWFIRSTPRIV		0x8BE0
#endif
#define SIOCIWELIAN   		(SIOCIWFIRSTPRIV + 0x17)
#endif

typedef enum {
	NET_STA_LINKWAY_CLEAN = 0,
	NET_STA_LINKWAY_NORMAL_AUTH,
	NET_STA_LINKWAY_WPS,
	NET_STA_LINKWAY_SMARTLINK,
} NET_STA_LinkWayEnumT;

typedef enum {
	NET_STA_LINKEVENT_CLEAN = 0,
	NET_STA_LINKEVENT_SUCCESS,
	NET_STA_LINKEVENT_FAIL,
	NET_STA_LINKEVENT_ASSOCIATION_FAIL,
	NET_STA_LINKEVENT_HANDSHAKE_FAIL,
	NET_STA_LINKEVENT_LINKTIMEOUT,
} NET_STA_LinkEventEnumT;

#ifndef IW_ESSID_MAX_SIZE
#define IW_ESSID_MAX_SIZE 32
#endif

//Sta private data space
typedef struct StaPrivate_s {
	s8 ifname[IFNAMSIZ];
	s8 ssid[IW_ESSID_MAX_SIZE];
	s8 mac[14];
	struct wpa_ctrl * 	cmdSocket;
	struct wpa_ctrl * 	eventSocket;
	pthread_t 			eventProcess;
	pthread_t 			smartLinkProcess;
	pthread_rwlock_t 	staRwlock;
	pthread_mutex_t  	staMutex;
	fCallBack			smartLinkCallBack;
	ELIAN_ResultT 		SmartLinkRes;
	NET_STA_LinkWayEnumT linkWay;
	NET_STA_LinkEventEnumT	 linkEvent;
	bool isHaveScanRes;
    bool isOpen;
	bool isEvent;
	u8	 smartLinkChannel;
	s8 *deviceName;
	bool supportWPS;
	bool supportSmartlink;
} StaPrivateT;

//Sta operate handle.
typedef struct StaObject_s {
	WirelessHandle		handle;
	s32 (*getWiFiInterface)(WirelessHandle, s8 **);
	s32 (*turnOnWiFi)(WirelessHandle);
	s32 (*turnOffWiFi)(WirelessHandle);
	s32 (*getCurrentNameOfConnectionPoint)(WirelessHandle, s8 **);
	s32 (*getCurrentMACOfConnectionPoint)(WirelessHandle, s8 **);
	s32 (*getCurrentConnectionStatus)(WirelessHandle);
	s32 (*getAccessPointLists)(WirelessHandle, SCAN_DataT[], s32);
	s32 (*passwordAuthToConnectAccessPoint)(WirelessHandle, const s8*, const s8*);
	s32 (*WPSPbcAuthToConnectAccessPoint)(WirelessHandle, const s8*);
	s32 (*WPSPinAuthToConnectAccessPoint)(WirelessHandle, const s8*, const s8*);
	s32 (*openOnceSmartLink)(WirelessHandle, u8, fCallBack);
	s32 (*closeCurrentSmartLink)(WirelessHandle, s32);
	s32 (*disconnect)(WirelessHandle);
	s32 (*reconnect)(WirelessHandle);
	StaPrivateT			private;
} StaObjT;

#define pRdlock(n)		pthread_rwlock_rdlock(&(n->private.staRwlock))
#define pWrlock(n)		pthread_rwlock_wrlock(&(n->private.staRwlock))
#define vRwlock(n)		pthread_rwlock_unlock(&(n->private.staRwlock))
#define pTryMutex(n)	pthread_mutex_trylock(&(n->private.staMutex))
#define pMutex(n)		pthread_mutex_lock(&(n->private.staMutex))
#define vMutex(n)		pthread_mutex_unlock(&(n->private.staMutex))

static StaObjT *getObjectByHandle(WirelessHandle handle)
{
	return (StaObjT *)handle;
}

static s32 _staProbe(WirelessHandle handle)
{
	s32 ret = 0;
	s8 *wifi_device = NULL;
	NET_WIFI_DeviceNumT device_info;
	NET_WIFI_SupportFuncT *supportFunc = NULL;

	StaObjT *staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
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
		return NET_STA_BADPARAM;
	}
	staObj->private.deviceName 	= supportFunc->deviceName;
	staObj->private.supportWPS	= supportFunc->supportWPS;
	staObj->private.supportSmartlink = supportFunc->supportSmartlink;
	return NET_STA_OK;
}



static s32 FAST_FUNC _wifi_ioctl(const s8 *dev, u32 op_code, void *parm)
{
	s32 sockfd;
	struct iwreq *wreq = NULL;

	if(dev == NULL || parm == NULL){
		LogE("bad params");
		return NET_STA_BADPARAM;
	}
	wreq = (struct iwreq *)parm;
	snprintf(wreq->ifr_ifrn.ifrn_name, sizeof(wreq->ifr_ifrn.ifrn_name), "%s", dev);
	/*create a socket to get info from ioctl*/
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		LogE("Cannot creat socket,error");
		return NET_STA_NOSUPPORT;
	}
	if (ioctl(sockfd, op_code, wreq) == -1) {
		if(SIOCGIWSCAN != op_code || errno != E2BIG)
			LogE("IOCTL [0x%x] Failed,error", op_code);
		close(sockfd);
		return NET_STA_NOSUPPORT;
	}
	close(sockfd);
	return NET_STA_OK;
}

static s32 FAST_FUNC _switchLinkWayToNormalAuth(WirelessHandle handle, u32 second)
{
	NET_STA_LinkWayEnumT oldLinkWay = NET_STA_LINKWAY_CLEAN;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	if(staObj->private.linkWay == NET_STA_LINKWAY_NORMAL_AUTH)
		return NET_STA_OK;
	LogI("current linkway: %d", staObj->private.linkWay);
	pWrlock(staObj);
	oldLinkWay = staObj->private.linkWay;
	staObj->private.linkWay = NET_STA_LINKWAY_CLEAN;
	vRwlock(staObj);
	second *= 10;
	do{
		usleep(100 * 1000);
	}while(staObj->private.linkWay == NET_STA_LINKWAY_CLEAN && (--second != 0));
	if(second == 0 && staObj->private.linkWay == NET_STA_LINKWAY_CLEAN) {
		staObj->private.linkWay = oldLinkWay;
		return NET_STA_TIMEOUT;
	}
    pWrlock(staObj);
	staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
    vRwlock(staObj);
	return NET_STA_OK;
}

static s32 getCurrentNameOfConnectionPoint(WirelessHandle handle, s8 **ssid)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(ssid)) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	wreq.u.essid.pointer = staObj->private.ssid;//if not write these codes , the program maybe wrong.
	wreq.u.essid.length = sizeof(staObj->private.ssid);
	ret = _wifi_ioctl(staObj->private.ifname, SIOCGIWESSID, &wreq);
	/*get ESSID from ioctl*/
	if (ret) {
		LogE("IOCTL SIOCGIWESSID Failed,error");
		return ret;
	}
	LogI("Get current link name: %s", ssid);
	*ssid = staObj->private.ssid;
	return NET_STA_OK;
}

static s32 SetCurrentLinkName(WirelessHandle handle, s8 *ssid, s32 len)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(ssid) || len <= 0) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	wreq.u.essid.pointer = ssid;//if not write these codes , the program maybe wrong.
	wreq.u.essid.length = len;
	ret = _wifi_ioctl(staObj->private.ifname, SIOCSIWESSID, &wreq);
	/*get ESSID from ioctl*/
	if (ret) {
		LogE("IOCTL SIOCSIWESSID Failed,error");
		return ret;
	}
	return NET_STA_OK;
}

#if 0
static s32 GetCurrentTransRate(WirelessHandle handle, s32 * const freq_mhz)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(freq_mhz)) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	ret = _wifi_ioctl(staObj->private.ifname, SIOCGIWFREQ, &wreq);
	if (ret) {
		LogE("IOCTL SIOCGIWFREQ Failed,error");
		return ret;
	}
	/* output current freq and channel*/
	if(wreq.u.freq.e){
		int i = 6;
		i -= wreq.u.freq.e;
		if(i < 0){
			*freq_mhz = wreq.u.freq.m;
			for(i = 0; i < wreq.u.freq.e - 6; i++)
				*freq_mhz *= 10;
		} else {
			*freq_mhz = wreq.u.freq.m;
			for(; i > 0; i--)
				*freq_mhz /= 10;
		}
	} else {
		*freq_mhz = wreq.u.freq.m/1000000;
	}

	return  NET_STA_OK;
}
#endif

static s32 getCurrentMACOfConnectionPoint(WirelessHandle handle, s8 **mac)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(mac)) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	ret = _wifi_ioctl(staObj->private.ifname, SIOCGIWAP, &wreq);
	if (ret) {
		LogE("IOCTL SIOCGIWAP Failed,error");
		return ret;
	}
	/* output current connecting AP MAC*/
	memcpy(staObj->private.mac, wreq.u.ap_addr.sa_data, 14);
	*mac = staObj->private.mac;
	return NET_STA_OK;
}

s32 SetCurrentLinkMAC(WirelessHandle handle, s8 mac[], s32 len)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(mac)) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	len = len < 14? len : 14;
	memcpy(wreq.u.ap_addr.sa_data, mac, len);
	ret = _wifi_ioctl(staObj->private.ifname, SIOCSIWAP, &wreq);
	if (ret) {
		LogE("IOCTL SIOCSIWAP Failed,error");
		return ret;
	}
	return NET_STA_OK;
}

static s32 FAST_FUNC _WpaCtlCmd(WirelessHandle handle, const s8 *cmd)
{
	s8 buf[2048];
	unsigned int len;
	s32 ret;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(staObj->private.cmdSocket, cmd, strlen(cmd), buf, &len, NULL);
	if (ret == -2) {
		LogE("'%s' command timed out.\n", cmd);
		return ret;
	} else if (ret < 0) {
		LogE("'%s' command failed.\n", cmd);
		return ret;
	}
    if(len >= 2 && strncmp(buf, "OK", 2) == 0)
        return 0;
    else
        return -1;
}

static s32 FAST_FUNC _wpaLinkStatus(struct wpa_ctrl *ctrl, s8 **pstatus, s32 *plen)
{
	s8 buf[2048];
	unsigned int len;
	s32 ret;
    s8 *substr = NULL;
    const s8 *cmd = "STATUS";
    const s8 *wpa_status = "wpa_state=";

	len = sizeof(buf) - 1;
    memset(buf, 0, sizeof(buf));
	ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
	if (ret == -2) {
		LogE("'%s' command timed out.\n", cmd);
		return NET_STA_TIMEOUT;
	} else if (ret < 0) {
		LogE("'%s' command failed.\n", cmd);
		return NET_STA_NOSUPPORT_VERSION;
	}

    substr = strstr(buf, wpa_status);
    if(substr != NULL){
        int len = 0;
        substr += strlen(wpa_status);
        while(substr[len] == '_' || (substr[len] <= 'Z' && substr[len] >= 'A') || (substr[len] <= '9' && substr[len] >= '0')){
            len++;
        }
        *pstatus = malloc(len+1);
        if(pstatus == NULL)
            return NET_STA_OUTOFMEMORY;
        *plen = len+1;
        memset(*pstatus, 0, len+1);
        memcpy(*pstatus, substr, len);
        return NET_STA_OK;
    }
   	return NET_STA_NOSUPPORT_VERSION;
}

static s32 FAST_FUNC _wpaBuildEmptyCfgFile(const s8 *cfg_path)
{
    s32 fd = -1;
    s8 buff[128] = {0};
    if(isNull(cfg_path))
        return NET_STA_BADPARAM;

    printf("\nbuild empty file:%s\n", cfg_path);

    /* clean configuration file */
    if(0 == access(cfg_path, F_OK)){
        printf("remove file:%s\n", cfg_path);
		remove(cfg_path);
	}
    /* open configuration file */
	fd = open(cfg_path, O_RDWR | O_CREAT, 0666);
	if(fd < 0){
		PRINT_ERR("open %s failed\n", cfg_path);
		return NET_STA_NOTOPEN;
	}
    printf("open file:%s\n", cfg_path);

    memset(buff, 0, sizeof(buff));
    sprintf(buff, "ctrl_interface=%s\r\n\
        update_config=1\r\n", WPA_CTL_INTERFACE);
    write(fd, buff, strlen(buff));
    close(fd);
    printf("close file:%s\n", cfg_path);

    return NET_STA_OK;
}

#define IW_EV_LCP_LEN	(sizeof(struct iw_event) - sizeof(union iwreq_data))
#define IW_EV_POINT_LEN	(IW_EV_LCP_LEN + sizeof(struct iw_point) - \
			 IW_EV_POINT_OFF)
#define IW_EV_POINT_OFF (((char *) &(((struct iw_point *) NULL)->length)) - \
				  (char *) NULL)
#define SEQ_MAX_VALUE 	8


static s32 FAST_FUNC _staSetNetwork(WirelessHandle handle, s32 seq,
        s32 flag, const s8 *content)
{
    s32 ret;
    s8 cmd[CMD_LEN_MAX*2] = {0};
    const s8 flag_arry[][12] ={"ssid", "psk", "key_mgmt",
        "indentity", "password"};

    if(flag < 0 || flag > WIFI_SET_MAXNUM || isNull(content))
    {
        return NET_STA_BADPARAM;
    }

	if(flag != WIFI_SET_KEY_MGMT)
	{
    	ret = snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"",
        	seq, flag_arry[flag], content);
	}
	else
	{
	 	ret = snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s",
        	seq, flag_arry[flag], content);
	}

	if (ret < 0 || (size_t) ret >= sizeof(cmd))
	{
		return -1;
	}
	cmd[sizeof(cmd) - 1] = '\0';

	return _WpaCtlCmd(handle, cmd);
}

static s32 FAST_FUNC _getNetWork(WirelessHandle handle, s32 seq,
    s32 flag, s8 *buf, s32 buflen)
{
    s32 ret;
    s8 cmd[CMD_LEN_MAX*2] = {0};
    const s8 flag_arry[][12] ={"ssid", "psk", "key_mgmt",
        "indentity", "password"};
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
    if(flag < 0 || flag > WIFI_SET_MAXNUM || buf == NULL)
        return NET_STA_BADPARAM;

    ret = snprintf(cmd, sizeof(cmd), "GET_NETWORK %d %s",
        seq, flag_arry[flag]);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return NET_STA_OUTOFMEMORY;
	cmd[sizeof(cmd) - 1] = '\0';

    if(staObj->private.cmdSocket == NULL)
		return NET_STA_NOTOPEN;
    memset(buf, 0, buflen);
	ret = wpa_ctrl_request(staObj->private.cmdSocket, cmd, strlen(cmd), buf, (unsigned int*)&buflen, NULL);
    buf[buflen - 1] = '\0';
	if (ret < 0 || strcmp(buf, "FAIL") == 0) {
		printf("'%s' command failed.\n", cmd);
		return NET_STA_NOSUPPORT_VERSION;
	}

	return NET_STA_OK;
}


static s32 FAST_FUNC _staAddNetwork(WirelessHandle handle)
{
    s8 buf[2048];
	unsigned int len;
	s32 ret, i;
    struct wpa_ctrl *pstWpaCtl;

   	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}

    pstWpaCtl = staObj->private.cmdSocket;
    if(pstWpaCtl == NULL)
		return NET_STA_NOTOPEN;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, "ADD_NETWORK", strlen("ADD_NETWORK"), buf, &len, NULL);
	if (ret == -2) {
		LogE("commmand timed out.");
		return NET_STA_TIMEOUT;
	} else if (ret < 0) {
		printf("command failed.\n");
		return NET_STA_NOSUPPORT_VERSION;
	}

    if(buf[0]>='0' && buf[0]<='9'){
        ret = 0;
        for(i=0; buf[i]>='0' && buf[i]<='9'; i++){
            ret *= 10;
            ret = buf[i] - '0';
        }
        return ret;
    }

    return NET_STA_NOSUPPORT;
}

#if 0
static s32 FAST_FUNC _staRemoveNetwork(WirelessHandle handle, s32 seq)
{
    int ret;
    char cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "REMOVE_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return _WpaCtlCmd(handle, cmd);
}
#endif

static s32 FAST_FUNC _staSelectNetwork(WirelessHandle handle, s32 seq)
{
    s32 ret;
    s8 cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return WIFI_FAIL;
	cmd[sizeof(cmd) - 1] = '\0';

	return _WpaCtlCmd(handle, cmd);
}

static s32 FAST_FUNC  _staEnableNetwork(WirelessHandle handle, s32 seq)
{
    s32 ret;
    s8 cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return WIFI_FAIL;
	cmd[sizeof(cmd) - 1] = '\0';

	return _WpaCtlCmd(handle, cmd);
}

static s32 FAST_FUNC _staDisableNetwork(WirelessHandle handle, s32 seq)
{
    s32 ret;
    s8 cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "DISABLE_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return _WpaCtlCmd(handle, cmd);
}

static s32 disconnect(WirelessHandle handle)
{
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
    return _WpaCtlCmd(handle, "DISCONNECT");
}


static s32 reconnect(WirelessHandle handle)
{
	s32 ret = 0;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	ret = _switchLinkWayToNormalAuth(handle, 2);
	if(ret != NET_STA_OK) {
		LogE("do not switch normal auth way, ret[%d].", ret);
		return ret;
	}
    return _WpaCtlCmd(handle, "RECONNECT");
}

static s32 FAST_FUNC _wpaGetMaxSeq(WirelessHandle handle)
{
    s8 buf[2048];
	unsigned int len;
	s32 ret, i;
    s32 max_seq = -1;
	s32 tmp_seq = 0;
    struct wpa_ctrl *pstWpaCtl = NULL;
    StaObjT *staObj =  getObjectByHandle(handle);

	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
    pstWpaCtl = staObj->private.cmdSocket;
    if(pstWpaCtl == NULL)
		return WIFI_FAIL;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, "LIST_NETWORKS", strlen("LIST_NETWORKS"), buf, &len, NULL);
	if (ret == -2) {
		LogE("command timed out.\n");
		return WIFI_TIMEOUT;
	} else if (ret < 0) {
		LogE("command failed.\n");
		return WIFI_FAIL;
	}

    for(i=0; i<sizeof(buf) && buf[i] != '\0'; i++){
        if(buf[i] == '\n'){
            tmp_seq = 0;
            i++;
            while(buf[i]>='0' && buf[i]<='9' && i < sizeof(buf)){
                tmp_seq *= 10;
                tmp_seq += buf[i++] - '0';
                if(tmp_seq > max_seq && tmp_seq < SEQ_MAX_VALUE)
                    max_seq = tmp_seq;
            }
        }
    }
    return max_seq;
}

static s32 FAST_FUNC _staFindSeq(WirelessHandle handle, const s8 *ssid)
{
    s32 i, ret;
    s8 des_ssid[128];
    s32 max_seq = -1;
	s32 len = 0;

    if(isNull(handle) || isNull(ssid))
        return NET_STA_BADPARAM;
    len = strlen(ssid);
    len += 1;
    len <<= 1;
    char *ascii_ssid = malloc(len);
    if(ascii_ssid == NULL)
        return NET_STA_OUTOFMEMORY;
    max_seq = _wpaGetMaxSeq(handle);
    memset(ascii_ssid, 0, len);
    net_chartoascii(ssid, ascii_ssid);
    for(i=0; i <= max_seq; i++)
    {
        ret = _getNetWork(handle, i,
            WIFI_SET_SSID, des_ssid, sizeof(des_ssid));
        if(ret < 0){
            free(ascii_ssid);
            return i;
        }
		if(strncmp(ssid, &des_ssid[1], (IFNAMSIZ-1)) == 0){
            free(ascii_ssid);
            return i;
        }
        if(strncmp(ascii_ssid, des_ssid, ((IFNAMSIZ<<1)-1)) == 0){
            free(ascii_ssid);
            return i;
        }
    }
    free(ascii_ssid);
    return -1;
}
#if 0
static s32 _staUdhcpc(const s8 *ifname)
{
	s32 ret = 0;
	s8 cmd[CMD_LEN_MAX] = {0};
	if(isNull(ifname))
		return NET_STA_BADPARAM;
	sprintf(cmd,"%s -b -i%s -R -p %s", UDHCPC_CMD, ifname, UDHCPC_PID);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd,"%s -2 `%s %s`", KILL_CMD, CAT_CMD, UDHCPC_PID);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	return ret;
}
#endif
static u32 FAST_FUNC freq_to_channel(s32 freq)
{
    if(freq >= 2407){
        return (freq - 2407)/5;
    }
    return freq;
}

static s32 FAST_FUNC _wpa_v19(const s8  *ifname)
{
	int ret;
	struct iwreq wreq;
    char *buff[sizeof(struct iw_range)*2];
    struct iw_range *range_raw;

	if(isNull(ifname)){
		LogE("bad params");
		return false;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
    wreq.u.data.pointer = buff;
    wreq.u.data.length= sizeof(buff);
    wreq.u.data.flags = 0;
    ret = _wifi_ioctl(ifname, SIOCGIWRANGE, &wreq);
	/*get mode from ioctl*/
	if (ret) {
		LogE("IOCTL SIOCGIWAUTH Failed,error");
		return ret;
	}
    range_raw = (struct iw_range *)buff;
    if(range_raw->we_version_compiled < 19){
        return false;
	}
	return true;
}

static void FAST_FUNC _wifi_gen_ie(GEN_IE_DataT *gen_ie, s32 ie_len, u8 *buff, s32 buff_len)
{
    s32 offset = 0;
    s32 ie_index = 0;

    if(gen_ie == NULL || buff == NULL
        || ie_len <= 0 || buff_len <=0){
        LogE("Bad params");
        return;
    }
    for(ie_index=0; (ie_index < ie_len) && (gen_ie[ie_index].use_flag > 0); ie_index++);

    if(ie_index >= ie_len){
        LogE("Bad params");
        return;
    }

    for(;offset <= (buff_len - 2) && ie_len > ie_index;offset += buff[offset+1]+2){
        if(buff[offset] == 0xdd || buff[offset] == 0x30){
            u8 *ie_buff = buff + offset;
            s32 ie_len = ie_buff[1] + 2;
            s32 index = 2;
            u8 wpa1_oui[3] = {0x00, 0x50, 0xf2};
            u8 wpa2_oui[3] = {0x00, 0x0f, 0xac};
            u8 *wpa_oui = NULL;
            s32 i;
            s32 cnt;
            s32 par_index = 0;
            s32 auth_index = 0;

            if(ie_len > buff_len)
                ie_len = buff_len;
            if(ie_buff[0] == 0x30)
            {
                if(ie_len < 4){
                    continue;
                }
                wpa_oui = wpa2_oui;
                gen_ie[ie_index].ie = WIFI_IE_WPA2;
            }
            else if(ie_buff[0] == 0xdd)
            {
                wpa_oui = wpa1_oui;
                if((ie_len < 8)
                    ||(memcmp(&ie_buff[index], wpa_oui,3) != 0)
                    ||(ie_buff[index + 3] != 0x01))
                {
                    continue;
                }
                index += 4;
                gen_ie[ie_index].ie = WIFI_IE_WPA1;
            }
            gen_ie[ie_index].version = ie_buff[index] | (ie_buff[index + 1] << 8);
            index += 2;

            /* get group chip */
            if(ie_len < (index + 4)){
                gen_ie[ie_index].group_chip = WIFI_CHIP_TKIP;
                gen_ie[ie_index].pair_chip[0] = WIFI_CHIP_TKIP;
                gen_ie[ie_index].use_flag = 1;
                ie_index++;
                continue;
            }
            if(memcmp(&ie_buff[index], wpa_oui, 3) != 0){
                gen_ie[ie_index].group_chip = WIFI_CHIP_PROPRIETARY;
            } else {
                if(ie_buff[index+3] >= WIFI_CHIP_MAXNUM)
                    gen_ie[ie_index].group_chip = WIFI_CHIP_UNKNOW;
                else
                    gen_ie[ie_index].group_chip = ie_buff[index+3] + 1;

            }
            index += 4;

            /* get pair chip */
            if(ie_len < (index + 2)){
                gen_ie[ie_index].pair_chip[0] = WIFI_CHIP_TKIP;
                gen_ie[ie_index].use_flag = 1;
                ie_index++;
                continue;
            }
            cnt = ie_buff[index] | (ie_buff[index+1] << 8);
            index += 2;
            if(ie_len  < (index + 4*cnt)){
                gen_ie[ie_index].use_flag = 1;
                ie_index++;
                continue;
            }
            for(i=0; i < cnt; i++){
                if(par_index < sizeof(gen_ie[ie_index].pair_chip)){
                    if(memcmp(&ie_buff[index], wpa_oui,3) != 0)
                        gen_ie[ie_index].pair_chip[par_index] = WIFI_CHIP_PROPRIETARY;
                    else if(ie_buff[index+3] >= WIFI_CHIP_MAXNUM)
                        gen_ie[ie_index].pair_chip[par_index] = WIFI_CHIP_UNKNOW;
                    else
                        gen_ie[ie_index].pair_chip[par_index] = ie_buff[index+3] + 1;
                }
                index += 4;
                par_index++;
            }

            /* get auth */
            if(ie_len < (index + 2)){
                gen_ie[ie_index].use_flag = 1;
                ie_index++;
                continue;
            }
            cnt = ie_buff[index] | (ie_buff[index+1] << 8);
            index += 2;
            if(ie_len  < (index + 4*cnt)){
                gen_ie[ie_index].use_flag = 1;
                ie_index++;
                continue;
            }
            for(i=0; i < cnt; i++){
                if(auth_index < sizeof(gen_ie[ie_index].auth_suite)){
                    if(memcmp(&ie_buff[index], wpa_oui,3) != 0)
                        gen_ie[ie_index].auth_suite[auth_index] = WIFI_AUTH_PROPRIETARY;
                    else if(ie_buff[index+3] >= WIFI_AUTH_MAXNUM)
                        gen_ie[ie_index].auth_suite[auth_index] = WIFI_AUTH_UNKNOW;
                    else
                        gen_ie[ie_index].auth_suite[auth_index] = ie_buff[index+3] + 1;
                }
                index += 4;
                auth_index++;
            }
            gen_ie[ie_index].use_flag = 1;
            ie_index++;
       }
    }
}


static s32 FAST_FUNC _staScanListOrderByDBM(SCAN_DataT *list, s32 len)
{
	s32 i,j;
	s32 index = 0;
	SCAN_DataT tmp;
	s32 vild_len;

	if(list == NULL || len <= 0)
		return NET_STA_BADPARAM;
	for(i=0; i<len; i++){
		if(list[i].used != 1)
			break;
	}
	vild_len = i;
	for(i=0; i < vild_len-1; i++){
		index = i;
		for(j=i+1; j < vild_len; j++){
			if((list[j].level - list[j].noise)
				>(list[index].level - list[index].noise)){
				index = j;
			}
		}
		if(index != i){
			tmp = list[i];
			list[i] = list[index];
			list[index] = tmp;
		}
	}

	return NET_STA_OK;
}

static s32 FAST_FUNC _staScanListOrder(SCAN_DataT *list, s32 len, s32 order)
{
	s32 ret = NET_STA_OK;
	/* error check */
	if(list == NULL || len <= 0)
		return NET_STA_BADPARAM;
	switch(order)
	{
		case WIFI_SCAN_ORDER_DBM:
			ret = _staScanListOrderByDBM(list, len);
			break;
		default:
			ret = NET_STA_NOSUPPORT;
			break;
	}
	return ret;
}


static s32 getAccessPointLists(WirelessHandle handle, SCAN_DataT list[], s32 len)
{
	s32 ret;
	struct iwreq wreq;
	u32 res_buf_len = IW_SCAN_MAX_DATA;
	u8 *res_buf;
	struct iw_event iwe_buf, *iwe = &iwe_buf;
	s8 *pos, *end, *custom;
	s32 count = 100;
	SCAN_DataT list_cache[50];
	SCAN_DataT * list_tmp = list_cache;
	s32 list_max = 50;
	s32 cur_num = list_max;
	s8 *ifname = NULL;

	StaObjT *staObj = getObjectByHandle(handle);
	if(isNull(staObj)){
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(list)|| len <= 0){
		LogE("bad params");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	ret = _switchLinkWayToNormalAuth(handle, 2);
	if(ret != NET_STA_OK) {
		LogE("do not switch normal auth way, ret[%d].", ret);
		return ret;
	}
	ifname = staObj->private.ifname;
	/* wait result data integrity */
	if(!staObj->private.isHaveScanRes) {
		int time = 100;
		while(time-- > 0 && !staObj->private.isHaveScanRes)
			usleep(10000);
		if(!staObj->private.isHaveScanRes) {
			LogE("get scanning info timeout.");
			return NET_STA_TIMEOUT;
		}
	}

	memset(&wreq, 0, sizeof(wreq));
	res_buf = malloc(res_buf_len);
	do{
		if (res_buf == NULL) {
			res_buf = malloc(res_buf_len);
			if (res_buf == NULL) return NET_STA_OUTOFMEMORY;
		}

		memset(&wreq, 0, sizeof(struct iwreq));
		wreq.u.data.pointer = res_buf;//if not write these codes , the program maybe wrong.
		wreq.u.data.length = res_buf_len;
		ret = _wifi_ioctl(ifname, SIOCGIWSCAN, &wreq);
	//	printf("-----------------------1\n");
		/*get IEEE from ioctl*/
		if (ret == 0) break;
		if (errno == E2BIG && res_buf_len < 65535) {
			free(res_buf);
			res_buf = NULL;
			res_buf_len *= 2;
			if (res_buf_len > 65535)
				res_buf_len = 65535; /* 16-bit length field */
			LogW("Scan results did not fit - "
				   "trying larger buffer (%lu bytes)\n",
				   (unsigned long) res_buf_len);
		} else {
			usleep(50000);
		}
		count--;
	}while(count);

	/* clean list spase */
	memset(list_cache, 0, sizeof(list_cache));
	if(count <= 0)
	{
		LogW("Scan timeout,No found wifi list");
		if(res_buf != NULL)
		{
			free(res_buf);
		}
		/* init scanning */
		memset(&wreq, 0, sizeof(wreq));
		wreq.u.data.pointer = NULL;
		wreq.u.data.length = 0;
		wreq.u.data.flags = IW_SCAN_DEFAULT;
		if ( _wifi_ioctl(ifname, SIOCSIWSCAN, &wreq) < 0) {
			LogE("ioctl[SIOCSIWSCAN]: %s",strerror(errno));
			return NET_STA_NOSUPPORT;
		}
		return NET_STA_DEVICEBUSY;
	}

	pos = (char *) res_buf;
	end = (char *) res_buf + wreq.u.data.length;
    bool version = _wpa_v19(ifname);
	while (pos + IW_EV_LCP_LEN <= end && cur_num > 0) {
		/* Event data may be unaligned, so make a local, aligned copy
		 * before processing. */
		memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);
		if (iwe->len <= IW_EV_LCP_LEN)
			break;

		custom = pos + IW_EV_POINT_LEN;
		if ((version) && (iwe->cmd == SIOCGIWESSID || iwe->cmd == SIOCGIWENCODE ||
		 iwe->cmd == IWEVGENIE || iwe->cmd == IWEVCUSTOM)) {
			/* WE-19 removed the pointer from struct iw_point */
			char *dpos = (char *) &iwe_buf.u.data.length;
			int dlen = dpos - (char *) &iwe_buf;
			memcpy(dpos, pos + IW_EV_LCP_LEN,
				  sizeof(struct iw_event) - dlen);
		} else {
			memcpy(&iwe_buf, pos, sizeof(struct iw_event));
			custom += IW_EV_POINT_OFF;
		}
		switch (iwe->cmd) {
			case SIOCGIWAP:
			{
                if(list_tmp->used == 1){
    				list_tmp++;
    				cur_num--;
                }
                list_tmp->used = 1;

				memset(list_tmp->mac, 0, sizeof(list_tmp->mac));
				memcpy(list_tmp->mac,iwe->u.ap_addr.sa_data, 14);
				break;
			}
            case SIOCGIWMODE:
            {
			    list_tmp->mode = iwe->u.mode;
            }
			break;
            case SIOCGIWNAME:
                memcpy(list_tmp->protocol, iwe->u.name, IW_ESSID_MAX_SIZE);
            break;
			case SIOCGIWESSID:
			{
				int ssid_len = iwe->u.essid.length;
				if (custom + ssid_len > end)
					break;
				if (iwe->u.essid.flags) {
				    memset(list_tmp->ssid, 0, sizeof(list_tmp->ssid));
                    if(ssid_len > 0){
    					if(ssid_len <= sizeof(list_tmp->ssid)){
                            memcpy(list_tmp->ssid, custom, ssid_len);
    					} else {
                            memcpy(list_tmp->ssid, custom, sizeof(list_tmp->ssid));
                        }
                    } else {
                        //strcpy(list->ssid.name, "Hide");
                        list_tmp->ssid[0] ='\0';
                    }
                    list_tmp->ssid[IW_ESSID_MAX_SIZE-1] = '\0';
				} else {
					list_tmp->ssid[0] ='\0';
                    //list->ssid.name_len = strlen("隐藏网络");
                }
			}break;
			case SIOCGIWFREQ:
			{
                int divi = 1000000, i;
                if(iwe->u.freq.m > 1000){
                	if (iwe->u.freq.e == 0) {
                        list_tmp->channel = freq_to_channel(iwe->u.freq.m);
                	} else if (iwe->u.freq.e > 6) {
                		list_tmp->channel = 0;
                	} else {
                    	for (i = 0; i < iwe->u.freq.e; i++)
                    		divi /= 10;
                    	list_tmp->channel = freq_to_channel(iwe->u.freq.m / divi);
                	}
                } else {
                    list_tmp->channel = iwe->u.freq.m;
                }

			}
			break;
			case IWEVQUAL:
			{
				if ((iwe->u.qual.updated & (IW_QUAL_DBM | IW_QUAL_RCPI)) ||
				    ((iwe->u.qual.level != 0))) {
				    /* Deal with quality : always a relative value */
				    if(!(iwe->u.qual.updated & IW_QUAL_QUAL_INVALID)) {
					 	list_tmp->qual = iwe->u.qual.qual;
					} else {
						list_tmp->qual = 0;
					}
					if(iwe->u.qual.updated & IW_QUAL_RCPI)
					{
						/* Deal with signal level in RCPI */
						/* RCPI = int{(Power in dBm +110)*2} for 0dbm > Power > -110dBm */
						if(!(iwe->u.qual.updated & IW_QUAL_LEVEL_INVALID))
					    {
					      list_tmp->level = (iwe->u.qual.level/2) - 110;
					    }
						/* Deal with noise level in dBm (absolute power measurement) */
						if(!(iwe->u.qual.updated & IW_QUAL_NOISE_INVALID))
						{
						  list_tmp->noise = (iwe->u.qual.noise/2) - 110;
						}
					} else {
						if(iwe->u.qual.updated & IW_QUAL_DBM)
					    {
					    	if(!(iwe->u.qual.updated & IW_QUAL_LEVEL_INVALID)) {
								if (iwe->u.qual.level >= 64)
									list_tmp->level = iwe->u.qual.level - 0x100;
								else
									list_tmp->level = iwe->u.qual.level;
					    	} else {
					    		list_tmp->level = 0;
					    	}
							if(!(iwe->u.qual.updated & IW_QUAL_NOISE_INVALID)) {
								if (iwe->u.qual.noise >= 64)
									list_tmp->noise = iwe->u.qual.noise - 0x100;
								else
									list_tmp->noise = iwe->u.qual.noise;
							} else {
								list_tmp->noise = 0;
							}
						} else {
							if(!(iwe->u.qual.updated & IW_QUAL_LEVEL_INVALID)) {
								list_tmp->level = iwe->u.qual.level;
					    	} else {
					    		list_tmp->level = 0;
					    	}
							if(!(iwe->u.qual.updated & IW_QUAL_NOISE_INVALID)) {
								list_tmp->noise = iwe->u.qual.noise;
							} else {
								list_tmp->noise = 0;
							}
						}
					}
				}else {
					list_tmp->qual= iwe->u.qual.qual;
					list_tmp->noise = iwe->u.qual.noise;
					list_tmp->level = iwe->u.qual.level;
				}
			}
			break;

			case SIOCGIWENCODE:
			{
                int i;
                unsigned char key[IW_ENCODING_TOKEN_MAX];
                unsigned char *buff = (unsigned char*)list_tmp->key;
                if(iwe->u.data.pointer){
                    //memcpy(key, iwe->u.data.pointer, iwe->u.data.length);
                }
                else
                    iwe->u.data.flags |= IW_ENCODE_NOKEY;

				if ((iwe->u.data.flags&IW_ENCODE_FLAGS) == IW_ENCODE_DISABLED){
					list_tmp->encode = WIFI_ENCRYPT_OFF;
				} else if((iwe->u.data.flags&IW_ENCODE_FLAGS) == IW_ENCODE_RESTRICTED){
					list_tmp->encode = WIFI_ENCRYPT_RESTRICTED;
				} else if((iwe->u.data.flags&IW_ENCODE_FLAGS) == IW_ENCODE_OPEN){
					list_tmp->encode = WIFI_ENCRYPT_OPEN;
				} else if((iwe->u.data.flags&IW_ENCODE_FLAGS) == IW_ENCODE_ENABLED){
				    list_tmp->encode = WIFI_ENCRYPT_ENABLED;
				} else if((iwe->u.data.flags&IW_ENCODE_FLAGS) == IW_ENCODE_TEMP){
				    list_tmp->encode = WIFI_ENCRYPT_TEMP;
				} else {
				    list_tmp->encode = WIFI_ENCRYPT_UNKNOW;
				}

                if((iwe->u.data.length*3) <= IW_ENCODING_TOKEN_MAX){
                    if(iwe->u.data.flags & IW_ENCODE_NOKEY){
                        if(iwe->u.data.length <= 0){
                            strcpy((char *)buff,"on");
                            list_tmp->encode = WIFI_ENCRYPT_ON;
                        }
                        else {
                            strcpy((char *)buff,"**");
                            buff += 2;
                            for(i=1; i < iwe->u.data.length; i++){
                                if((i&0x01) == 0){
                                    strcpy((char *)buff, "-");
                                    buff++;
                                    strcpy((char *)buff, "**");
                                    buff += 2;
                                }
                            }
                        }
                    } else {
                        sprintf((char *)buff,"%.2x", key[0]);
                        buff += 2;
                        for(i=1; i < iwe->u.data.length; i++){
                            if((i&0x01) == 0){
                                strcpy((char *)buff, "-");
                                buff++;
                                sprintf((char *)buff,"%.2x", key[i]);
                                buff += 2;
                            }
                        }
                    }
                }
			}
			break;

            case SIOCGIWRATE:
            {
                int maxrate;
                char *cust = pos + IW_EV_LCP_LEN;
                struct iw_param p;
                size_t clen;
                clen = iwe->len;

                if(cust + clen > end){
                    list_tmp->used = 1;
    				list_tmp++;
    				cur_num--;
                    break;
                }
                maxrate = 0;
                while(((ssize_t)clen) >= (ssize_t)sizeof(struct iw_param)){
                    memcpy(&p, cust, sizeof(struct iw_param));
                    if(p.value > maxrate)
                        maxrate = p.value;
                    clen -= sizeof(struct iw_param);
                    cust += sizeof(struct iw_param);
                }
                list_tmp->maxrate = maxrate / 500000;
            }
            break;
            case IWEVGENIE:
                if(iwe->u.data.length + custom > end)break;
                _wifi_gen_ie(list_tmp->gen_ie, sizeof(list_tmp->gen_ie)/sizeof(GEN_IE_DataT),
                    (unsigned char*)custom, iwe->u.data.length);
            break;

            case IWEVCUSTOM:
            {

            }break;
		}
		pos += iwe->len;
	}
	/* when cur_num < 1, out of bound */
	if(cur_num < 1)
		cur_num = 1;

	/* scan data dbm order*/
	ret = _staScanListOrder(list_cache, list_max - cur_num + 1, WIFI_SCAN_ORDER_DBM);
	if(ret < 0){
		LogW("order fail\n");
	}
	s32 vild_len = ((list_max - cur_num + 1)<len)?(list_max - cur_num + 1):len;
	memcpy(list, list_cache, sizeof(SCAN_DataT) * vild_len);
	free(res_buf);
	return WIFI_OK;
}


static s32 getCurrentConnectionStatus(WirelessHandle handle)
{
    s32 ret;
    struct wpa_ctrl *pstWpaCtl = NULL;
    s8 *status = NULL;
    s32 stlen = 0;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	if(staObj->private.cmdSocket == NULL){
		LogE("Wireless not support command socket.");
		return NET_STA_NOSUPPORT;
	}

	pstWpaCtl = staObj->private.cmdSocket;
    ret = _wpaLinkStatus(pstWpaCtl, &status, &stlen);
    if(ret < 0 || status == NULL){
        return WIFI_STATUS_ERROR;
    }
    if(strcmp(status, "COMPLETED") == 0)
        ret = WIFI_STATUS_CONNECTED;
    else if(strcmp(status, "DISCONNECTED") == 0)
        ret = WIFI_STATUS_DISCONNECTED;
    else if(strcmp(status, "INACTIVE") == 0)
        ret = WIFI_STATUS_INACTIVE;
	else if (strcmp(status, "SCANNING") == 0
		|| strcmp(status, "4WAY_HANDSHAKE") == 0
		|| strcmp(status, "GROUP_HANDSHAKE") == 0
		|| strcmp(status, "ASSOCIATED") == 0
		|| strcmp(status, "ASSOCIATING") == 0){
        ret = WIFI_STATUS_CONNECTING;
    } else {
        ret = WIFI_STATUS_UNKNOW;
    }
    /* get status, need free space */
    free(status);
    return ret;
}

static s32 getStaWorkMode(WirelessHandle handle, s32 * const mode)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(mode == NULL){
		LogE("bad params\n");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	ret = _wifi_ioctl(staObj->private.ifname, SIOCGIWMODE, &wreq);
	/*get mode from ioctl*/
	if (ret) {
		LogE("IOCTL SIOCGIWMODE Failed,error");
		return ret;
	}
	*mode = wreq.u.mode;
	return NET_STA_OK;
}

static s32 setStaWorkMode(WirelessHandle handle, s32 mode)
{
	s32 ret;
	struct iwreq wreq;
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	wreq.u.mode = mode;
	ret = _wifi_ioctl(staObj->private.ifname, SIOCSIWMODE, &wreq);
	/*set mode from ioctl*/
	if (ret) {
		LogE("IOCTL SIOCSIWMODE Failed,error");
		return ret;
	}

	return NET_STA_OK;
}

#ifdef NETWORK_ELIAN_SUPPORT
#define ELIAN_CHAR_MAX	128
static s32 FAST_FUNC _staSmartLinkIoctl(const s8* dev, s8 *cmd, s32 cmd_len)
{
	s32 ret = 0;
	struct iwreq wreq;
	if(isNull(dev) || isNull(cmd)) {
		LogE("Smart Link params error");
		return NET_STA_BADPARAM;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	wreq.u.data.pointer = cmd;//if not write these codes , the program maybe wrong.
	wreq.u.data.length = cmd_len;
	ret = _wifi_ioctl(dev, SIOCIWELIAN, &wreq);
	/*get ESSID from ioctl*/
	if (ret) {
		LogE("IOCTL SIOCGIWESSID Failed,error");
		return ret;
	}
	return NET_STA_OK;
}

static s32	_staSmartLinkStartScan(const s8* dev, u8 channels)
{
	s8 cmd[ELIAN_CHAR_MAX] = {0};
	if(isNull(dev)) {
		LogE("Smart Link params error");
		return NET_STA_BADPARAM;
	}
	if(channels == 0xff){
		snprintf(cmd, sizeof(cmd), "start");
	} else {
		snprintf(cmd, sizeof(cmd), "start_ch=%d", channels);
	}
	return _staSmartLinkIoctl(dev, cmd, strlen(cmd));
}

static s32	_staSmartLinkStopScan(const s8* dev)
{
	s8 cmd[ELIAN_CHAR_MAX] = {0};
	if(isNull(dev)) {
		LogE("Smart Link params error");
		return NET_STA_BADPARAM;
	}
	snprintf(cmd, sizeof(cmd), "stop");
	return _staSmartLinkIoctl(dev, cmd, strlen(cmd));
}

static s32	_staSmartLinkClearScanResult(const s8* dev)
{
	s8 cmd[ELIAN_CHAR_MAX] = {0};
	if(isNull(dev)) {
		LogE("Smart Link params error");
		return NET_STA_BADPARAM;
	}
	snprintf(cmd, sizeof(cmd), "clear");
	return _staSmartLinkIoctl(dev, cmd, strlen(cmd));
}

#define ELIAN_VALUE_STRING	1
#define ELIAN_VALUE_U8		2
#define ELIAN_VALUE_U32		3
static void FAST_FUNC _staSmartLinkResultStrToVaule(s8 *str, s8 *label,
	s32 type, void *buf, s32 size)
{
	s8 *cmd_cur = NULL;
	if(isNull(str) || isNull(label) || isNull(buf) || size <= 0)
		return;

	cmd_cur = strstr(str, label);
    if(cmd_cur != NULL){
        int len = 0;
        cmd_cur += strlen(label);
       	if(type == ELIAN_VALUE_U8){
			*(unsigned char *)buf = 0;
			while(*cmd_cur <= '9' && *cmd_cur >= '0'){
				*(unsigned char *)buf *= 10;
				*(unsigned char *)buf += *cmd_cur - '0';
				cmd_cur++;
			}
			printf("[ELIAN]%s%d\n",  label, *(unsigned char *)buf);
       	} else if(type == ELIAN_VALUE_U32){
			*(unsigned int *)buf = 0;
			while(*cmd_cur <= '9' && *cmd_cur >= '0'){
				*(unsigned int *)buf *= 10;
				*(unsigned int *)buf += *cmd_cur - '0';
				cmd_cur++;
			}
			printf("[ELIAN]%s%d\n",  label, *(unsigned int *)buf);
       	} else if(type == ELIAN_VALUE_STRING) {
       		char *pbuf = (char *)buf;
       		while(len < size && *cmd_cur != ',' && *cmd_cur != ' '
				&& *cmd_cur != '\0' && *cmd_cur != '\n') {
				*pbuf++ = *cmd_cur++;
				len++;
			}
			printf("[ELIAN]%s%s\n", label, (char *)buf);
       	}
    }
}

static s32	_staSmartlinkGetScanResult(const s8* dev, ELIAN_ResultT *result)
{
	s32 ret = 0;
	s8 cmd[ELIAN_CHAR_MAX] = {0};
	if(isNull(dev)) {
		LogE("Smart Link params error");
		return NET_STA_BADPARAM;
	}
	snprintf(cmd, sizeof(cmd), "result");
	ret = _staSmartLinkIoctl(dev, cmd, strlen(cmd));
	if(ret < 0)
		return ret;
	_staSmartLinkResultStrToVaule(cmd, "AM=", ELIAN_VALUE_U8,
		(void *)&(result->auth_mode), sizeof(result->auth_mode));
	_staSmartLinkResultStrToVaule(cmd, "ssid=", ELIAN_VALUE_STRING,
		(void *)&(result->ssid[0]), sizeof(result->ssid));
	_staSmartLinkResultStrToVaule(cmd, "pwd=", ELIAN_VALUE_STRING,
		(void *)&(result->pwd[0]), sizeof(result->pwd));
	_staSmartLinkResultStrToVaule(cmd, "cust_data_len=", ELIAN_VALUE_U8,
		(void *)&(result->cust_data_len), sizeof(result->cust_data_len));
	_staSmartLinkResultStrToVaule(cmd, "user=", ELIAN_VALUE_STRING,
		(void *)&(result->user[0]), sizeof(result->user));
	_staSmartLinkResultStrToVaule(cmd, "cust_data=", ELIAN_VALUE_STRING,
		(void *)&(result->cust_data[0]), sizeof(result->cust_data));

	return NET_STA_OK;
}

#if 0
static s32	_setSmartLinkScanChannel(const s8* dev, u8 channels)
{
	s8 cmd[ELIAN_CHAR_MAX] = {0};
	if(isNull(dev)) {
		LogE("Smart Link params error");
		return NET_STA_BADPARAM;
	}
	snprintf(cmd, sizeof(cmd), "set_ch=%d", channels);
	return _staSmartLinkIoctl(dev, cmd, strlen(cmd));
}
#endif

static s32 FAST_FUNC _staSmartLinkDefaultCallback(WirelessHandle handle, ELIAN_ResultT
	*result)
{

	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(result)) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	return staObj->passwordAuthToConnectAccessPoint(handle, result->ssid, result->pwd);
}

void *_staSmartLinkProccess(void *handle)
{
    //IPC_MID_UTIL_SetThreadName("sta samrt link proccess");
	StaObjT * staObj = (StaObjT *)(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NULL;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NULL;
	}
	if(staObj->private.linkWay != NET_STA_LINKWAY_SMARTLINK) {
		LogE("Wireless smartlink start status error.");
		return NULL;
    }
	Log.i("start pthread_detach(pthread_self())");
	pthread_detach(pthread_self());
	Log.i("start pthread");
	_staSmartLinkStartScan(staObj->private.ifname, staObj->private.smartLinkChannel);//use 0xff, scan all of channels
	while(staObj->private.linkWay == NET_STA_LINKWAY_SMARTLINK){
		/* running programe */
		memset(&(staObj->private.SmartLinkRes), 0, sizeof(ELIAN_ResultT));
		_staSmartlinkGetScanResult(staObj->private.ifname, &(staObj->private.SmartLinkRes));
		if(strlen(staObj->private.SmartLinkRes.ssid) > 0
			|| strlen(staObj->private.SmartLinkRes.pwd) > 0
			|| staObj->private.SmartLinkRes.cust_data_len > 0) {
			_staSmartLinkStopScan(staObj->private.ifname);//stop scanning elian, exit smartlink
			_staSmartLinkClearScanResult(staObj->private.ifname);
			break;
		}
		sleep(1);//sleep
	}
	_staSmartLinkStopScan(staObj->private.ifname);
	pWrlock(staObj);
	staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
	vRwlock(staObj);
	if(strlen(staObj->private.SmartLinkRes.ssid) > 0) {
		if(staObj->private.smartLinkCallBack != NULL) {
			staObj->private.smartLinkCallBack(staObj->handle,
				&(staObj->private.SmartLinkRes));
		} else {
			_staSmartLinkDefaultCallback(staObj->handle,
				&(staObj->private.SmartLinkRes));
		}
	}
	pthread_exit(0);
}

static s32 _staSmartLinkStart(StaObjT * staObj)
{
    s32 ret = 0;
    if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
    if(staObj->private.linkWay == NET_STA_LINKWAY_SMARTLINK) {
		LogE("Wireless smartlink already start.");
		return NET_STA_ALREADINIT;
    } else {
        ret = _switchLinkWayToNormalAuth(staObj->handle, 2);
        if(ret != NET_STA_OK) {
        	LogE("do not switch normal auth way, ret[%d].", ret);
        	return ret;
        }
    }
	pWrlock(staObj);
	staObj->private.linkWay = NET_STA_LINKWAY_SMARTLINK;//set smartlink status
	vRwlock(staObj);
	pthread_create(&(staObj->private.smartLinkProcess), NULL, _staSmartLinkProccess,(void *)staObj);
    return NET_STA_OK;
}

static s32 _staSmartLinkStop(StaObjT * staObj, s32 seconds)
{
    s32 ret = 0;
    if(isNull(staObj)){
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	if(staObj->private.linkWay == NET_STA_LINKWAY_NORMAL_AUTH) {
		LogE("Wireless smartlink already stop.");
        return NET_STA_ALREADINIT;
	}
    ret = _switchLinkWayToNormalAuth(staObj->handle, seconds);
    if(ret != NET_STA_OK) {
		LogE("do not switch normal auth way, ret[%d].", ret);
		return ret;
	}
    return NET_STA_OK;
}

#endif

static s32 FAST_FUNC _staWaitLinkEvent(WirelessHandle handle, NET_STA_LinkWayEnumT linkway)
{
	s32 ret = WIFI_OK;
	NET_STA_LinkWayEnumT way;
	s32 scanTimes = 0;

	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}
	pWrlock(staObj);
	staObj->private.linkEvent = NET_STA_LINKEVENT_CLEAN;
	staObj->private.isHaveScanRes = false;
	vRwlock(staObj);
	do{
		usleep(10000);//10 ms
		pWrlock(staObj);
		ret = staObj->private.linkEvent;
		way = staObj->private.linkWay;
		if(staObj->private.isHaveScanRes) {
			scanTimes++;
			staObj->private.isHaveScanRes = false;
		}
		vRwlock(staObj);
	}while(ret == NET_STA_LINKEVENT_CLEAN && way == linkway
		&& (scanTimes < NET_STA_MAX_SCANNING_TIMES));

	if(way != linkway) {
		LogW("Link way is change [%d] to [%d] [%d]", linkway, way, staObj->private.linkWay);
		return NET_STA_LINKEVENT_FAIL;
	} else if (scanTimes >= NET_STA_MAX_SCANNING_TIMES) {
		return NET_STA_LINKEVENT_ASSOCIATION_FAIL;
	} else {
		return ret;
	}
}

static s32 WPSPbcAuthToConnectAccessPoint(WirelessHandle handle, const s8 *bssid)
{
#ifdef NETWORK_WPS_SUPPORT
    s32 ret;
    s8 cmd[CMD_LEN_MAX] = {0};
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}

	if(isNull(bssid)){
	    ret = snprintf(cmd, sizeof(cmd), "WPS_PBC");
		if (ret < 0 || (size_t) ret >= sizeof(cmd))
			return NET_STA_OUTOFMEMORY;
	} else {
	    ret = snprintf(cmd, sizeof(cmd), "WPS_PBC %s", bssid);
		if (ret < 0 || (size_t) ret >= sizeof(cmd))
			return NET_STA_OUTOFMEMORY;
	}
	cmd[sizeof(cmd) - 1] = '\0';

	ret = pTryMutex(staObj);
	if(ret != 0) {
		LogE("Wifi is connecting.");
		return NET_STA_DEVICEBUSY;
	}
	/* switch to normal auth way */
	ret = _switchLinkWayToNormalAuth(handle, 2);
	if(ret != NET_STA_OK) {
		LogE("do not switch normal auth way, ret[%d].", ret);
		vMutex(staObj);
		return ret;
	}
	staObj->private.linkWay = NET_STA_LINKWAY_WPS;
	ret = _WpaCtlCmd(handle, cmd);
	if(ret != NET_STA_OK) {
		LogE("No device or wpa_supplicant version not support");
		staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
		vMutex(staObj);
		return NET_STA_NOSUPPORT_VERSION;
	}
	vMutex(staObj);
	ret = _staWaitLinkEvent(handle, NET_STA_LINKWAY_WPS);//120 s
	if(ret == NET_STA_LINKEVENT_SUCCESS)
		ret = WIFI_OK;
	else if (ret == NET_STA_LINKEVENT_ASSOCIATION_FAIL)
		ret = WIFI_NO_SSID;
	else if (ret == NET_STA_LINKEVENT_HANDSHAKE_FAIL)
		ret = WIFI_LINK_ERROR;
	else if (ret == NET_STA_LINKEVENT_LINKTIMEOUT)
		ret = WIFI_TIMEOUT;
	else if (ret == NET_STA_LINKEVENT_FAIL)
		ret = WIFI_FAIL;

	staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
	return ret;
#else
	return NET_STA_NOSUPPORT;
#endif
}

static s32 WPSPinAuthToConnectAccessPoint(WirelessHandle handle, const s8 *bssid, const s8 *pin)
{
#ifdef NETWORK_WPS_SUPPORT
    s32 ret;
    s8 cmd[CMD_LEN_MAX] = {0};
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}
	if(isNull(bssid)|| isNull(pin)){
		LogE("Bad params device[0x%p] bssid[0x%p] pin[0x%p]\n",
			staObj, bssid, pin);
		return NET_STA_BADPARAM;
	}

    ret = snprintf(cmd, sizeof(cmd), "WPS_PIN %s %s", bssid, pin);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return NET_STA_OUTOFMEMORY;

	ret = pTryMutex(staObj);
	if(ret != 0) {
		LogE("Wifi is connecting.");
		return NET_STA_DEVICEBUSY;
	}
	/* switch to normal auth way */
	ret = _switchLinkWayToNormalAuth(handle, 2);
	if(ret != NET_STA_OK) {
		LogE("do not switch normal auth way, ret[%d].", ret);
		vMutex(staObj);
		return ret;
	}
	staObj->private.linkWay = NET_STA_LINKWAY_WPS;
	cmd[sizeof(cmd) - 1] = '\0';
	ret = _WpaCtlCmd(handle, cmd);
	if(ret != NET_STA_OK) {
		LogE("No device or wpa_supplicant version not support\n");
		staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
		vMutex(staObj);
		return NET_STA_NOSUPPORT_VERSION;
	}
	vMutex(staObj);
	ret = _staWaitLinkEvent(handle, NET_STA_LINKWAY_WPS);//120 s
	if(ret == NET_STA_LINKEVENT_SUCCESS)
		ret = WIFI_OK;
	else if (ret == NET_STA_LINKEVENT_ASSOCIATION_FAIL)
		ret = WIFI_NO_SSID;
	else if (ret == NET_STA_LINKEVENT_HANDSHAKE_FAIL)
		ret = WIFI_LINK_ERROR;
	else if (ret == NET_STA_LINKEVENT_LINKTIMEOUT)
		ret = WIFI_TIMEOUT;
	else if (ret == NET_STA_LINKEVENT_FAIL)
		ret = WIFI_FAIL;
	staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
	return ret;
#else
	return NET_STA_NOSUPPORT;
#endif

}

#if 0
static s8 * FAST_FUNC _staWpsGenPin(WirelessHandle handle, s8 *bssid)
{
#ifdef NETWORK_WPS_SUPPORT
    s32 ret;
	s8 buf[2048] = {0};
	size_t len;
    s8 cmd[CMD_LEN_MAX] = {0};
	struct wpa_ctrl *pstWpaCtl;
	s8 *pin = NULL;

	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj) || isNull(bssid))
		return NULL;

	ret = snprintf(cmd, sizeof(cmd), "WPS_PIN %s", bssid);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return NULL;
	cmd[sizeof(cmd) - 1] = '\0';

    pstWpaCtl = staObj->private.cmdSocket;
    if(pstWpaCtl == NULL)
		return NULL;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, cmd, strlen(cmd), buf, &len, NULL);
	if (ret < 0) {
		LogE("'%s' command failed.\n", cmd);
		return NULL;
	}
	len = strlen(buf);
	if(len <= 0){
		return NULL;
	}
	pin = malloc(len+1);
	if(pin == NULL) return NULL;
	memset(pin, 0, len+1);
	memcpy(pin, buf, len);

	return pin;
#else
	return NULL;
#endif
}

static s32 FAST_FUNC _staWpsReg(WirelessHandle handle, const s8 *bssid, const s8 *pin)
{
#ifdef NETWORK_WPS_SUPPORT
    int ret;
    char cmd[CMD_LEN_MAX] = {0};
	if(isNull(bssid) || isNull(pin))
		return WIFI_FAIL;

	ret = snprintf(cmd, sizeof(cmd), "WPS_REG %s %s", bssid, pin);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return WIFI_FAIL;

	cmd[sizeof(cmd) - 1] = '\0';

	return _WpaCtlCmd(handle, cmd);
#else
    return NET_STA_NOSUPPORT;
#endif
}
#endif

static s32 FAST_FUNC _staSaveCfg(WirelessHandle handle)

{
    return _WpaCtlCmd(handle, "SAVE_CONFIG");
}

/* dev is wifi device name, name is SSID of AP, passwd is link key, usedhcp is wether use udhcpc (0:no use !0:use) */
static s32 passwordAuthToConnectAccessPoint(WirelessHandle handle, const s8 *ssid, const s8 *key)
{
    s32 ret = 0;
    s32 seq = -1;

	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(ssid)){
		LogE("Bad params device[0x%p] link_data[0x%x] ssid[0x%p]\n",
			handle, 0, ssid);
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}

	ret = pTryMutex(staObj);
	if(ret != 0) {
		LogE("Wifi is connecting.");
		return NET_STA_DEVICEBUSY;
	}

	/* switch to normal auth way */
	ret = _switchLinkWayToNormalAuth(handle, 2);
	if(ret != NET_STA_OK) {
		LogE("do not switch normal auth way, ret[%d].", ret);
		goto LINKEXIT;
	}

	ret = getCurrentConnectionStatus(handle);
	if(ret == WIFI_STATUS_CONNECTED){
		if(strncmp(staObj->private.ssid, ssid,
			sizeof(staObj->private.ssid)) == 0) {
			LogW("Repeated wireless connection to \"%s\"", ssid);
			ret = WIFI_OK;
			goto LINKEXIT;
		}
		ret = disconnect(handle);
		if(ret < 0){
	        goto LINKEXIT;
		}
	}

    /* find seq of ssid, if not have seq return -1; */
    seq = _staFindSeq(handle, ssid);
    if(seq < 0){
        seq = _staAddNetwork(handle);
        if(seq < 0){
			ret = seq;
			goto LINKEXIT;
        }
    } else {
    	ret = disconnect(handle);
		if(ret < 0){
	        goto LINKEXIT;
		}
		ret = _staDisableNetwork(handle, seq);
		if(ret < 0){
	        goto LINKEXIT;
		}
    }
    ret = _staSetNetwork(handle, seq, WIFI_SET_SSID, ssid);
    if(ret < 0){
        LogE("Set network fail");
        goto LINKEXIT;
    }
    /* set psk */
	if(key != NULL && key[0] != 0){
	    ret = _staSetNetwork(handle, seq, WIFI_SET_PSK, key);
	    if(ret < 0){
	        LogE("Password is less than eight or format error");
	        goto LINKEXIT;
	    }
	}
	else
	{
	    ret = _staSetNetwork(handle, seq, WIFI_SET_KEY_MGMT, "NONE");
	    if(ret < 0){
	        LogE("Set network fail");
	       	goto LINKEXIT;
	    }
	}
    ret = _staSelectNetwork(handle, seq);
    if(ret < 0){
        goto LINKEXIT;
    }
    ret = _staEnableNetwork(handle, seq);
    if(ret < 0){
        goto LINKEXIT;
    }
    ret = reconnect(handle);
    if(ret < 0){
        goto LINKEXIT;
	}
	vMutex(staObj);

	ret = _staWaitLinkEvent(handle, NET_STA_LINKWAY_NORMAL_AUTH);//120 s
	if(ret == NET_STA_LINKEVENT_SUCCESS) {
		memset(staObj->private.ssid, 0, sizeof(staObj->private.ssid));
		strncpy(staObj->private.ssid, ssid, sizeof(staObj->private.ssid));
		ret = WIFI_OK;
	} else if (ret == NET_STA_LINKEVENT_ASSOCIATION_FAIL)
		ret = WIFI_NO_SSID;
	else if (ret == NET_STA_LINKEVENT_HANDSHAKE_FAIL)
		ret = WIFI_LINK_ERROR;
	else if (ret == NET_STA_LINKEVENT_LINKTIMEOUT)
		ret = WIFI_TIMEOUT;
	else if (ret == NET_STA_LINKEVENT_FAIL)
		ret = WIFI_FAIL;

	if(ret == WIFI_OK) {
		_staSaveCfg(handle);
	} else if(ret != WIFI_FAIL) {
		disconnect(handle);
	}
	return ret;
LINKEXIT:
	vMutex(staObj);
    return ret;
}

#define WPA_EXTRA_EVENT_WPA			"WPA:"
#define WPA_EXTRA_EVENT_AUTH		"Authentication"
#define WPA_EXTRA_EVENT_4_WAY_HEADSHAKE_FAIL	"4-Way Handshake failed"
#define WPA_EXTRA_EVENT_TIMEOUT		"time out"
#define WPA_EXTRA_EVENT_WPATIMEOUT	"wpatimeout"
#define WPA_EXTRA_CMD_SCAN			"scan"


static s32 _wpaListener(WirelessHandle handle, s8 *parm1, s8 *parm2)
{
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}
	if(isNull(parm1) || isNull(parm2)){
		LogE("wpa listener params error");
		return NET_STA_BADPARAM;
	}

	if(strcmp(parm1, WPA_EVENT_SCAN_RESULTS) == 0) {
		pWrlock(staObj);
		staObj->private.isHaveScanRes = true;
		vRwlock(staObj);
	} else if(strcmp(parm1, WPA_EXTRA_EVENT_WPATIMEOUT) == 0) {
		if(strcmp(parm2, WPA_EXTRA_CMD_SCAN) == 0) {
			struct iwreq wreq;
			/* init scanning */
			memset(&wreq, 0, sizeof(wreq));
			wreq.u.data.pointer = NULL;
			wreq.u.data.length = 0;
			wreq.u.data.flags = IW_SCAN_DEFAULT;
			/*
			pWrlock(staObj);
			staObj->private.isHaveScanRes = false;
			vRwlock(staObj);
			*/
			if ( _wifi_ioctl(staObj->private.ifname, SIOCSIWSCAN, &wreq) < 0) {
				LogE("ioctl[SIOCSIWSCAN]: %s",strerror(errno));
			}
		}
	} else if(strcmp(parm1, WPA_EXTRA_EVENT_AUTH) == 0) {
		s8 *point = strstr(parm2, WPA_EXTRA_EVENT_TIMEOUT);
		if(point != NULL) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_LINKTIMEOUT;
			vRwlock(staObj);
		}
	} else if (NET_STA_LINKWAY_NORMAL_AUTH == staObj->private.linkWay){
		if(strcmp(parm1, WPA_EVENT_CONNECTED) == 0) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_SUCCESS;
			vRwlock(staObj);
		}
		if((strcmp(parm1, WPA_EXTRA_EVENT_WPA) == 0 )
			&& (strncmp(parm2, WPA_EXTRA_EVENT_4_WAY_HEADSHAKE_FAIL,
			strlen(WPA_EXTRA_EVENT_4_WAY_HEADSHAKE_FAIL)) == 0)) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_HANDSHAKE_FAIL;
			vRwlock(staObj);
		}

	} else if (NET_STA_LINKWAY_WPS == staObj->private.linkWay){
		if(strcmp(parm1, WPS_EVENT_FAIL) == 0) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_FAIL;
			vRwlock(staObj);
		} else if(strcmp(parm1, WPS_EVENT_SUCCESS) == 0) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_SUCCESS;
			vRwlock(staObj);
		} else if(strcmp(parm1, WPS_EVENT_TIMEOUT) == 0) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_LINKTIMEOUT;
			vRwlock(staObj);
		} else if(strcmp(parm1, WPA_EVENT_EAP_FAILURE) == 0) {
			pWrlock(staObj);
			staObj->private.linkEvent = NET_STA_LINKEVENT_HANDSHAKE_FAIL;
			vRwlock(staObj);
		}
	} else if (NET_STA_LINKWAY_SMARTLINK == staObj->private.linkWay){
		//todo
	}

	return NET_STA_OK;
}

static void _staEventQuit(int signal)
{
	LogI("pthread_kill event, signal:%d", signal);
	pthread_exit(0);
}

void *_staEventProcess(void *handle)
{
	s8 buffer[256] = {0};
	u32 bufflen = 0;
	StaObjT * staObj = (StaObjT *)(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NULL;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NULL;
	}
	signal(SIGINT, _staEventQuit);
	signal(SIGQUIT, _staEventQuit);
	signal(SIGKILL, _staEventQuit);
	Log.i("start pthread_detach(pthread_self())");
	pthread_detach(pthread_self());
	Log.i("start pthread");
	s32 ret = 0;
	s8 *point = NULL;
	while(staObj->private.isEvent){
		ret = wpa_ctrl_pending(staObj->private.eventSocket);
		if(ret > 0) {
			bufflen = sizeof(buffer);
			wpa_ctrl_recv(staObj->private.eventSocket, buffer, &bufflen);
			if(bufflen <= 0)continue;
			if(bufflen < sizeof(buffer)) {
				buffer[bufflen] = '\0';
			} else {
				buffer[sizeof(buffer) - 1] = '\0';
			}
			Log.i("%s", buffer);
			point = strstr(buffer, ">");
	 		if(point != NULL) {
				bufflen = 0;
				point++;
				while(point[bufflen] != ' ' && point[bufflen] != '\n'){
					bufflen++;
				}
				point[bufflen++] = '\0';
				while(point[bufflen] == ' '){
					bufflen++;
				}
				_wpaListener(staObj->handle, point, point + bufflen);
			}
		} else {
			//once scan data.
			_wpaListener(staObj->handle, WPA_EXTRA_EVENT_WPATIMEOUT,
				WPA_EXTRA_CMD_SCAN);
		}
	}
	Log.i("exit pthread");
	pthread_exit(0);
	return NULL;
}

static s32 FAST_FUNC _staStartEvent(StaObjT * staObj)
{
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	if(staObj->private.isEvent){

		LogE("Wireless event already start.");
		return NET_STA_ALREADINIT;
	}
	pWrlock(staObj);
	staObj->private.isEvent = true;
	vRwlock(staObj);
	pthread_create(&(staObj->private.eventProcess), NULL, _staEventProcess,(void *)staObj);

	return NET_STA_OK;
}

static s32 FAST_FUNC _staStopEvent(StaObjT * staObj)
{
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	if(staObj->private.isEvent){
		pWrlock(staObj);
		staObj->private.isEvent = false;
		vRwlock(staObj);
		pthread_kill(staObj->private.eventProcess, SIGQUIT);
		return NET_STA_OK;
	} else {
		LogE("Wireless event not start.");
		return NET_STA_NOTINIT;
	}
}

int _staOpenWpasupplicant(const s8 *ifname)
{
	int ret = 0;
	s8 cmd[CMD_LEN_MAX] = {0};
	if(isNull(ifname))
		return -1;
	ret = _wpaBuildEmptyCfgFile(PASSWD_CFG);
    if(ret < 0) {
        PRINT_ERR("_wpaBuildEmptyCfgFile error\n");
        return -1;
    }

	/* check wpa_suppliant */
    if(access(WPA_SUPPLICANT_CMD, F_OK) || access(WPA_SUPPLICANT_CMD, X_OK)){
		PRINT_ERR("Is no exist <<%s>>, or is not operation permissions.", ifname);
		return -1;
	}

	/* start wpa_suppliant */
    memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd),"%s -B -Dnl80211 -i%s -c %s &", WPA_SUPPLICANT_CMD, ifname, PASSWD_CFG);
    printf("cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
	return 0;
}




static s32 turnOnWiFi(WirelessHandle handle)
{
	int ret = 0;
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(staObj->private.isOpen){
		LogE("Wireless already initialization.");
		return NET_STA_ALREADINIT;
	}
	/* check ifname whether exit */
	ret = strlen(staObj->private.ifname);
	if(ret <= 0) {
		LogE("Wireless ifname is empty.");
		return NET_STA_BADPARAM;
	}
    /* check whether init */
	staObj->private.cmdSocket = wpa_ctrl_open(staObj->private.ifname, WPA_CTL_INTERFACE);
    if(staObj->private.cmdSocket == NULL)
    {
	    ret = _staOpenWpasupplicant(staObj->private.ifname);
		if(ret < 0)
        	return ret;
		staObj->private.cmdSocket = wpa_ctrl_open(staObj->private.ifname, WPA_CTL_INTERFACE);
		if((ret < 0) && (staObj->private.cmdSocket == NULL)){
			pWrlock(staObj);
			LogE("wifi on fail");
			memset(staObj->private.ifname, 0, sizeof(staObj->private.ifname));
			staObj->private.isOpen = false;
			vRwlock(staObj);
			return ret;
		}
    }
	staObj->private.eventSocket = wpa_ctrl_open(staObj->private.ifname, WPA_CTL_INTERFACE);
	if(staObj->private.eventSocket != NULL) {
		wpa_ctrl_attach(staObj->private.eventSocket);
	}

	pWrlock(staObj);
	staObj->private.isOpen = true;//set flag of open wireless.
	staObj->private.linkWay = NET_STA_LINKWAY_NORMAL_AUTH;
	vRwlock(staObj);
	Log.i("start event");
	_staStartEvent(staObj);

	return NET_STA_OK;
}

void _staCloseWpasupplicant(void)
{
    #if 0
	s8 cmd[CMD_LEN_MAX*2] = {0};
	snprintf(cmd, sizeof(cmd), "%s|%s %s|%s -e \"2d\" -e \"s/^\\s*\\w*/&</\" "
		"-e \"s/<.*//\"|%s %s 2",
		PS_CMD, GREP_CMD, WPA_SUPPLICANT_CMD, SED_CMD, XARGS_CMD, KILL_CMD);
    printf("cmd:%s\n", cmd);
	IPC_MID_UTIL_SysCall(cmd, NULL, 0);;
    sleep(1);
    printf("\n\n");
    system("ps");
    printf("\n\n");
    #else
	s8 cmd[CMD_LEN_MAX] = {0};
	sprintf(cmd, "killall wpa_supplicant");
    printf("cmd:%s\n", cmd);
	IPC_MID_UTIL_SysCall(cmd, NULL, 0);;

    //sleep(1);
    //printf("\n\n");
    //system("ps");
    //printf("\n\n");

    #endif
}

static s32 turnOffWiFi(WirelessHandle handle)
{
    s32 ret;
    s32 mode = IW_MODE_INFRA;
	StaObjT * staObj = getObjectByHandle(handle);
	if(isNull(staObj)) {
		LogE("invalid handle.");
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_ALREADINIT;
	}
	disconnect(handle);
    if ((ret = getStaWorkMode(handle, &mode)) < 0) {
        setStaWorkMode(handle, IW_MODE_INFRA);
	}

	if (mode == IW_MODE_INFRA) {
		/*
		 * Clear the BSSID selection and set a random SSID to make sure
		 * the driver will not be trying to associate with something
		 * even if it does not understand SIOCSIWMLME commands (or
		 * tries to associate automatically after deauth/disassoc).
		 */
		s32 i;
		s8 ssid[32] = {0};
		s8 mac[14] = {0};
		SetCurrentLinkMAC(handle, mac, sizeof(mac));
		for (i = 0; i < 32; i++)
			ssid[i] = rand() & 0xFF;
		SetCurrentLinkName(handle, ssid, sizeof(ssid));
	}
	_switchLinkWayToNormalAuth(handle, 5);
	_staStopEvent(staObj);
	if(staObj->private.eventSocket != NULL) {
		wpa_ctrl_detach(staObj->private.eventSocket);
		wpa_ctrl_close(staObj->private.eventSocket);
		staObj->private.eventSocket = NULL;
	}
	if(staObj->private.cmdSocket!= NULL) {
		wpa_ctrl_close(staObj->private.cmdSocket);
		staObj->private.cmdSocket = NULL;
	}

	_staCloseWpasupplicant();
	pWrlock(staObj);
	staObj->private.isOpen = false;
	vRwlock(staObj);
	return WIFI_OK;
}

static s32 openOnceSmartLink(WirelessHandle handle, u8 channel, fCallBack callBack)
{
#ifdef NETWORK_ELIAN_SUPPORT
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	staObj->private.smartLinkChannel = channel;
	staObj->private.smartLinkCallBack = callBack;
	return _staSmartLinkStart(staObj);
#else
	return NET_STA_NOSUPPORT;
#endif
}

static s32 closeCurrentSmartLink(WirelessHandle handle, s32 seconds)
{
#ifdef NETWORK_ELIAN_SUPPORT
	s32 ret = 0;
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj) || seconds < 0){
		return NET_STA_INVALIDHANDLE;
	}
	if(!staObj->private.isOpen){
		LogE("Wireless not initialization.");
		return NET_STA_NOTINIT;
	}
	ret =  _staSmartLinkStop(staObj, seconds);
	if(ret == NET_STA_OK) {
		staObj->private.smartLinkChannel = 0xff;
		staObj->private.smartLinkCallBack = NULL;
	}
	return ret;
#else
	return NET_STA_NOSUPPORT;
#endif
}

static s32 getWiFiInterface(WirelessHandle handle, s8 **pIfname)
{
	StaObjT *staObj =  getObjectByHandle(handle);
	if(isNull(staObj)){
		return NET_STA_INVALIDHANDLE;
	}
	if(isNull(pIfname)) {
		LogE("Bad params.");
		return NET_STA_BADPARAM;
	}
	if(!staObj->private.isOpen){
		LogW("Wireless is not open.");
	}
	*pIfname = staObj->private.ifname;
	return NET_STA_OK;
}

StaObjT *CreateNewSta(const s8 *ifname)
{
	StaObjT *staObj = malloc(sizeof(StaObjT));
	if(isNull(staObj) || isNull(ifname)) {
		LogE("Memory is not enough.");
		return NULL;
	}
	memset(staObj, 0, sizeof(StaObjT));
	staObj->handle = (WirelessHandle)staObj;
	staObj->getWiFiInterface = getWiFiInterface;
	staObj->turnOnWiFi = turnOnWiFi;
	staObj->turnOffWiFi = turnOffWiFi;
	staObj->getCurrentNameOfConnectionPoint = getCurrentNameOfConnectionPoint;
	staObj->getCurrentMACOfConnectionPoint = getCurrentMACOfConnectionPoint;
	staObj->getCurrentConnectionStatus = getCurrentConnectionStatus;
	staObj->getAccessPointLists = getAccessPointLists;
	staObj->passwordAuthToConnectAccessPoint = passwordAuthToConnectAccessPoint;
	staObj->WPSPbcAuthToConnectAccessPoint = WPSPbcAuthToConnectAccessPoint;
	staObj->WPSPinAuthToConnectAccessPoint = WPSPinAuthToConnectAccessPoint;
	staObj->openOnceSmartLink = openOnceSmartLink;
	staObj->closeCurrentSmartLink = closeCurrentSmartLink;
	staObj->disconnect = disconnect;
	staObj->reconnect = reconnect;
	strncpy(staObj->private.ifname, ifname, sizeof(staObj->private.ifname));
	s32 ret = _staProbe(staObj->handle);
	if(ret != NET_STA_OK) {
		free(staObj);
		return NULL;
	}
	pthread_rwlock_init(&(staObj->private.staRwlock), NULL);
	pthread_mutex_init(&(staObj->private.staMutex), NULL);
	return staObj;
}


void DestoryOldSta(StaObjT *staObj)
{
	if(staObj == NULL) {
		return;
	}
	if(staObj->private.isOpen){
		staObj->turnOffWiFi(staObj->handle);
	}
	pthread_rwlock_destroy(&(staObj->private.staRwlock));
	pthread_mutex_destroy(&(staObj->private.staMutex));
	free(staObj);
}

#ifdef WIFI_CLIENT_TEST
//#include "net_wifi.h"

static void _staUsage(void)
{
	printf("Wifi client Usage:\n");
	printf("  wifi_sta getsta [password]      get scan lists.\n");
	printf("  wifi_sta link [ssid] [password] link AP.\n");
	printf("  wifi_sta disconnect             diconnect link.\n");
	printf("  wifi_sta reconnect              reconnect link.\n");
	printf("  wifi_sta status                 get status of link.\n");
	printf("  wifi_sta wps_pbc [bssid]        WPS pbc link.\n");
	printf("  wifi_sta wps_pin <bssid> [pin]  WPS pin link.\n");
	printf("  wifi_sta elian                  smart link.\n");
}

int main(int argc, char *argv[])
{
	int ret = 0;

	StaObjT *staObj = CreateNewSta("ra0");
	if(staObj == NULL) {
		LogE("Create new sta object fail");
		return NET_STA_NOTOPEN;
	}
	ret = staObj->turnOnWiFi(staObj->handle);//on wifi
	if(ret < 0) {
		LogE("Wifi open fail");
		return NET_STA_NOTOPEN;
	}
	if (argc >= 2 && strcmp(argv[1], "getsta") == 0){
		SCAN_DataT list[50];
//		char name[32] = "ygy2";
		char protocol[32] = {0};
		char *essid = NULL;
		char *mac = NULL;
		int frq_mhz = 0;
		int mode = 0;
		int i;
//        int auth = 0;

        char work_mode[][8] = {"auto", "adhoc", "infra", "master",
        "repeat", "second", "monitor", "mesh"};
		memset(list, 0, 50 * sizeof(SCAN_DataT));
		//net_wifi_cli_get_transprotocol(wifi_handle, protocol, sizeof(protocol));
		staObj->getCurrentNameOfConnectionPoint(staObj->handle, &essid);
		//net_wifi_cli_get_workmode(wifi_handle, &mode);
		//net_wifi_cli_get_transfreq(wifi_handle, &frq_mhz);
		staObj->getCurrentMACOfConnectionPoint(staObj->handle, &mac);
		staObj->getAccessPointLists(staObj->handle, list, 50);
        printf("#########################################\n");
		printf("##%s\t%s  ESSID:\"%s\"                 ##\n", "wifi", protocol, essid);
		printf("##\tMODE:%s  Frequency:%d.%03d GHz  Access Point:%s  ##\n", work_mode[mode], frq_mhz/1000,
			frq_mhz%1000, mac);
        char pair_str[][8] = {"UNKNOW","NONE","WEP40","TKIP","WRAP","CCMP","WEP104"};
        char auth_str[][8] = {"UNKNOW","NONE","802.1x","PSK"};
        char ie_str[][8] = {"UNKNOW","WPA1","WPA2"};
        char encode_str [][8]= {"UNKNOW","OFF","OPEN","RES","ENABLE","TEMP","ON"};
		printf("################### list #############\n");
		for(i = 0; i < 50; i++){
            int j,k;
            if(list[i].used == 0)break;
            printf("#%02d# %s\t", i, list[i].ssid);
			printf("ESSID[%s]", list[i].mac);
            printf("[%s] ", work_mode[list[i].mode]);
			printf("[%d%%]\t[%s]\t[%d] ",
			list[i].qual, (WIFI_ENCRYPT_MAXMUN >= list[i].encode) ? encode_str[list[i].encode]:"UNKNOW", list[i].channel);
            /* 802.11 rates 500000 b/s units */
            printf("\t[%d Mb/s]", list[i].maxrate/2);
            for(j=0; j < sizeof(list[i].gen_ie)/sizeof(GEN_IE_DataT);j++){
                if(list[i].gen_ie[j].use_flag){
                    printf("\tIE%d:[%s]\t[v:%d]", j,
                        list[i].gen_ie[j].ie > WIFI_IE_MAXNUM ? "UNKNOW":ie_str[list[i].gen_ie[j].ie],
                        list[i].gen_ie[j].version);
                    printf("\t#");
                    for(k=0; k < sizeof(list[i].gen_ie[j].pair_chip)/sizeof(char);k++){
                        if(list[i].gen_ie[j].pair_chip[k] == WIFI_CHIP_UNKNOW
                            ||list[i].gen_ie[j].pair_chip[k] > WIFI_CHIP_MAXNUM)break;
                        printf(" %s", pair_str[list[i].gen_ie[j].pair_chip[k]]);
                    }
                    printf("\t#");
                    for(k=0; k < sizeof(list[i].gen_ie[j].auth_suite)/sizeof(char);k++){
                        if(list[i].gen_ie[j].auth_suite[k] == WIFI_AUTH_UNKNOW
                            ||list[i].gen_ie[j].auth_suite[k] > WIFI_AUTH_MAXNUM)break;
                        printf(" %s", auth_str[list[i].gen_ie[j].auth_suite[k]]);
                    }
                }
            }
            //printf(" key:%s\n", list[i].key);
            printf("\n");
		}
		printf("######################################\n");

        int index = 0;
        if(argc == 3){
            scanf("%d", &index);
            if(index >= 0 && index < i)
            printf("index:%d ssid:%s key:%s\n", index, list[index].ssid, argv[2]);

            ret = staObj->passwordAuthToConnectAccessPoint(staObj->handle, list[index].ssid, argv[2]);
            if(ret < 0)
                printf("connect fail!! ret:%d\n",ret);
            else
                printf("connect successful!!\n");
        }
		//free(list);

	}
	else if (argc == 4 && strcmp(argv[1], "link") == 0){
		ret = staObj->passwordAuthToConnectAccessPoint(staObj->handle,  argv[2], argv[3]);
        if(ret)printf("connecting fail code[%d]\n", ret);
        else printf("connected successful!!\n");
	} else if (argc == 2 && strcmp(argv[1], "disconnect") == 0){
	    if(staObj->getCurrentConnectionStatus(staObj->handle) == WIFI_STATUS_CONNECTED){
    	    staObj->disconnect(staObj->handle);
            printf("Disconnected successful!!\n");
	    }
	} else if (argc == 2 && strcmp(argv[1], "reconnect") == 0){
	    ret = staObj->reconnect(staObj->handle);
        if(ret < 0)
            printf("reconnect fail!!\n");
        else
            printf("reconnect successful!!\n");
	} else if (argc == 2 && strcmp(argv[1], "status") == 0){
	    int ret = 0;
	    ret = staObj->getCurrentConnectionStatus(staObj->handle);
        printf("Connect Status: ");
    	if(ret == WIFI_STATUS_DISCONNECTED)
            printf("disconnected\n");
        else if(ret == WIFI_STATUS_CONNECTING)
            printf("connecting\n");
        else if(ret == WIFI_STATUS_CONNECTED)
            printf("connected\n");
        else if(ret == WIFI_STATUS_INACTIVE)
            printf("Inactive\n");
        else
            printf("error\n");

#ifdef NETWORK_WPS_SUPPORT
	} else if (argc >= 2 && strcmp(argv[1], "wps_pbc") == 0){
		if(argc == 3)
	    	ret = staObj->WPSPbcAuthToConnectAccessPoint(staObj->handle, argv[2]);
		else
			ret = staObj->WPSPbcAuthToConnectAccessPoint(staObj->handle, NULL);
        if(ret < 0)
            printf("wps_pbc fail!!\n");
        else
            printf("wps_pbc successful!!\n");
	} else if (argc >= 4 && strcmp(argv[1], "wps_pin") == 0){
	    ret = staObj->WPSPinAuthToConnectAccessPoint(staObj->handle, argv[2], argv[3]);
        if(ret < 0)
            printf("wps_pin fail!!\n");
        else
            printf("wps_pin successful!!\n");
#endif

#ifdef	NETWORK_ELIAN_SUPPORT
	} else if(argc == 2 && strcmp(argv[1], "elian") == 0) {
		staObj->openOnceSmartLink(staObj->handle, 0xff, NULL);
#endif
	} else {
		_staUsage();
	}
	s8 op;
	do{
		printf("Put enter 'q' exit:#>");
		scanf("%c", &op);
	}while(op != 'q');
    staObj->turnOffWiFi(staObj->handle);
	DestoryOldSta(staObj);
	return WIFI_OK;
}

#endif

int net_wifi_cli_scan(const WIFI_HANDLE dev)
{
#ifdef WPA_CTL_INTERFACE
    char buf[2048];
	unsigned int len;
	int ret;
    struct wpa_ctrl *pstWpaCtl;
    if(isNull(dev))
        return WIFI_FAIL;

    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)
    {
        PRINT_ERR(" wpa_ctrl_open failed\n");
        return WIFI_FAIL;
    }

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, "SCAN", strlen("SCAN"), buf, &len, NULL);
	if (ret == -2)
    {
		PRINT_ERR(" command SCAN timed out.\n");
		return WIFI_TIMEOUT;
	} else if (ret < 0)
    {
		PRINT_ERR("SCAN command failed.\n");
		return WIFI_FAIL;
	}

    buf[len] = '\0';
    if(len >= 2 && strncmp(buf, "OK", 2) == 0)
    {
		PRINT_INFO("SCAN:%s.\n", buf);
    }
    else
    {
        return WIFI_FAIL;
    }

	wpa_ctrl_close(pstWpaCtl);
    return WIFI_OK;
#endif
}


int net_wifi_cli_strtoint(char *str)
{
	char *cmd_cur = str;
    unsigned int buf = 0;


    while(*cmd_cur != '\0')
    {
        if(*cmd_cur <= '9' && *cmd_cur >= '0')
        {
            buf *= 10;
            buf += *cmd_cur - '0';
            cmd_cur++;
        }
        else
        {
            PRINT_ERR("not a number\n");
            return -1;
        }
    }

    return buf;
}


/*
wifi channel rang from 1 ~ 13,with center frequecy range :
2412k,2417,2422,2427,2432,  2437  ,3442,2447,2452,2457,  2462  ,2467,2472
each center frequency is 5M interval ,a channel range 20M between center
frequency( 2M more between two channels )
e.g
channe  l range 2401~2423  (2412)
channe  2 range 2406~2428  (2417)
.
channe  6 range 2426~2448  (2437)
.
channe 11 range 2451~2473  (2462)
.

*/
int net_wifi_cli_fre2channel(int freq)
{

    if(freq >= 2407)
    {
        return (freq - 2407)/5;
    }

    else
    {
        PRINT_ERR("frequency :%d is out of range\n",freq);
        return -1;
    }


}

/*
    return encrypt mode 0~8, -1 for error.
*/
int net_wifi_cli_encrypt(char *buf)
{

    unsigned int encMode = WPAECN_NONE;


    if(0==strncmp(buf,WPA_TKIP_ENC,strlen(WPA_TKIP_ENC)))
    {
        encMode = WPAPSK_TKIP;
    }
    else if(0==strncmp(buf,WPA_AES_ENC,strlen(WPA_AES_ENC)))
    {
        encMode = WPAPSK_AES;
    }
    else if(0==strncmp(buf,WPA2_TKIP_ENC,strlen(WPA2_TKIP_ENC)))
    {
        encMode = WPA2PSK_TKIP;
    }
    else if(0==strncmp(buf,WPA2_AES_ENC,strlen(WPA2_AES_ENC)))
    {
        encMode = WPA2PSK_AES;
    }
    else if(0==strncmp(buf,WPS_ENC,strlen(WPS_ENC)))
    {
        /* need to confirm this value .......*/
        encMode = WEP_64_ASSCI;
    }
    else
    {
        PRINT_ERR("unknow encrypt mode:%s!\n",buf);

        encMode = WPAECN_NONE;
    }


    return encMode;

}

/*
scan result like this:
bssid / frequency / signal level / flags / ssid
4c:e6:76:46:8e:e3       2427    183     [WPA-PSK-TKIP][WPS]     4-508
bc:46:99:91:81:08       2412    187     [WPA-PSK-CCMP][WPA2-PSK-CCMP]   Ipc_1

*/
int net_wifi_cli_str2scanlist(char * buff,WIFI_SCAN_LIST_t *list, int *number)
{
    char *pStart,*pEnd;
    int cnt = 0;
	char * menu="bssid / frequency / signal level / flags / ssid\n";
    char menuLen=strlen(menu);
    char clo = 0;
    //char cloLen =0;
    char tmpBuf[100];
    int chn = 0;
    pStart = pEnd = buff + menuLen;

    while(*pEnd!='\0')
    {
        if('\t'==*pEnd)
        {
            if(0==clo)//bssid
            {
				//printf("DDDDDDDD %s,%d list[%d] macAddr:0x%x ,%xstart:0x%x,end:0x%x\n",
				//  __FUNCTION__,__LINE__,cnt,list[cnt].mac,pStart,pEnd);
				int tmp = pEnd-pStart;
				strncpy(list[cnt].mac, pStart, tmp);
				list[cnt].mac[tmp] = '\0';
				//printf ("ZZ %s ZZ\n", list[cnt].mac);
            }
            else if(1==clo)//fre
            {
            	int tmp = pEnd-pStart;
                strncpy(tmpBuf,pStart,tmp);
                tmpBuf[tmp]='\0';

                //printf ("ZZ %s ZZ",tmpBuf);
                chn = net_wifi_cli_strtoint(tmpBuf);
                list[cnt].channel = net_wifi_cli_fre2channel(chn);

                //printf ("LL %d LL",list[cnt].channel);
            }
            else if(2==clo)//signel level
            {
            	int tmp = pEnd-pStart;
                strncpy(tmpBuf,pStart,tmp);
                tmpBuf[tmp]='\0';

                //printf ("ZZ %s ZZ",tmpBuf);

                list[cnt].quality = net_wifi_cli_strtoint(tmpBuf);

                //printf ("LL %d LL",list[cnt].quality);
            }
            else if(3==clo)//flags means encryption mode
            {
            	int tmp = pEnd-pStart;
                strncpy(tmpBuf,pStart,tmp);
                tmpBuf[tmp]='\0';
                //printf ("ZZ %s ZZ",tmpBuf);
                list[cnt].security = net_wifi_cli_encrypt(tmpBuf);
                //printf ("LL %d LL",list[cnt].security);
            }

            pEnd++;
            pStart = pEnd;
            clo++;
        }
        else if('\n'==*pEnd)
        {
        	int tmp = pEnd-pStart;
            strncpy(list[cnt].essid,pStart,tmp);
            list[cnt].essid[tmp] = '\0';

            //printf ("ZZ %s ZZ\n",list[cnt].essid);
            pEnd++;
            pStart = pEnd;
            clo = 0;
            list[cnt].vaild = 1;
            cnt++;
            if(cnt >=20)
            {
                break;
            }

        }
        else
        {
            pEnd++;
        }
    }
    *number = cnt;

    return 0;
}

int net_wifi_cli_sortList(WIFI_SCAN_LIST_t *list, int number)
{
    WIFI_SCAN_LIST_t tmpList;
    int i,j;
    unsigned int maxQual;
    float qual;
    for(i = 0;i < number;i++)
    {   //
        for(j=i+1;j<number;j++)
        {
            if(list[j].quality > list[i].quality)
            {
                tmpList = list[i];
                list[i] = list[j];
                list[j] = tmpList;
            }
        }
    }
    maxQual = list[0].quality;
    for(i = 0;i < number;i++)
    {   //
        qual = (float)list[i].quality/(float)maxQual ;

        //printf("xzl qual=%f ,%d\n", qual, (int)qual);
        qual = qual * 100;
        list[i].quality = (int)qual;
        //printf("xzl qual=%f ,%d\n",qual, list[i].quality);
    }

	return 0;
}

int net_wifi_cli_scan_result(const WIFI_HANDLE dev, WIFI_SCAN_LIST_t *list, int *number)
{
#ifdef WPA_CTL_INTERFACE
    char buf[40960];
	unsigned int len;
	int ret;
    struct wpa_ctrl *pstWpaCtl;
    if(isNull(dev))
        return WIFI_FAIL;

    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)
    {
        PRINT_ERR(" wpa_ctrl_open failed\n");
        return WIFI_FAIL;
    }
    //PRINT_INFO("%s open %s success\n", dev, WPA_CTL_INTERFACE);

    memset(buf, 0, sizeof(buf));
	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, "SCAN_RESULTS", strlen("SCAN_RESULTS"), buf, &len, NULL);
	if (ret == -2)
    {
		PRINT_ERR(" command SCAN_RESULTS timed out.\n");
		return WIFI_TIMEOUT;
	}
    else if (ret < 0)
	{
		PRINT_ERR("SCAN_RESULTS command failed.\n");
		return WIFI_FAIL;
	}

    if (len <= 48)
	{
		PRINT_ERR("SCAN_RESULTS no results.\n");
		return WIFI_FAIL;
	}

    printf("#########################\n");
    printf("buf len:%d, buf_len:%ld\n", len, strlen(buf));
    //printf("scan result:%s\n", buf);
    printf("#########################\n");
    wpa_ctrl_close(pstWpaCtl);

    net_wifi_cli_str2scanlist(buf, list, number);
    if (*number <= 0)
	{
		PRINT_ERR("scan list number:%d\n", *number);
		return WIFI_FAIL;
	}

    net_wifi_cli_sortList(list, *number);

    return WIFI_OK;
#endif
}

/////////////////////////////////////////////////////////


int net_wifi_cli_status(const WIFI_HANDLE dev)
{
    int ret;
    static int last_status = -1;

    if(!dev)
        return WIFI_STATUS_ERROR;

#ifdef WPA_CTL_INTERFACE
    struct wpa_ctrl *pstWpaCtl;
    char *status = NULL;
    int stlen = 0;

    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)
    {
        PRINT_ERR("wpa_ctrl_open error\n");
        return WIFI_STATUS_ERROR;
    }
    ret = wpa_get_connect_status(pstWpaCtl, &status, &stlen);
    if(ret < 0 || status == NULL){
        PRINT_ERR("wpa_get_connect_status error\n");
        wpa_ctrl_close(pstWpaCtl);
        return WIFI_STATUS_ERROR;
    }
    //fprintf(stderr, "\rstatus :%s\n",status);
	//fflush(stderr);
    if(strcmp(status, "COMPLETED") == 0)
        ret = WIFI_STATUS_CONNECTED;
    else if(strcmp(status, "DISCONNECTED") == 0)
        ret = WIFI_STATUS_DISCONNECTED;
    else if(strcmp(status, "INACTIVE") == 0)
        ret = WIFI_STATUS_INACTIVE;
	else if (strcmp(status, "SCANNING") == 0
		|| strcmp(status, "4WAY_HANDSHAKE") == 0
		|| strcmp(status, "GROUP_HANDSHAKE") == 0
		|| strcmp(status, "ASSOCIATED") == 0
		|| strcmp(status, "ASSOCIATING") == 0){
        ret = WIFI_STATUS_CONNECTING;
    } else {
        ret = WIFI_STATUS_UNKNOW;
    }
    if (last_status != ret) {
        PRINT_INFO("WiFi connect status:%s\n", status);
        last_status = ret;
    }
    /* get status, need free space */
    free(status); //todo
    wpa_ctrl_close(pstWpaCtl);
    return ret;
#else
    char mac[32];
    ret = net_wifi_cli_get_bssid(dev, mac, MAC_LEN);
    if(ret < 0)
        return WIFI_STATUS_ERROR;
    if(strcmp(mac, MAC_EMPTY) == 0) {
        ret = WIFI_STATUS_DISCONNECTED;
    } else {
        ret = WIFI_STATUS_CONNECTED;
    }

    return ret;
#endif
}

int net_wpa_ctrl_command(const WIFI_HANDLE dev, char *cmd)
{
    int ret;
    struct wpa_ctrl *pstWpaCtl;
    if(isNull(dev) || isNull(cmd))
        return WIFI_FAIL;

    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)
    {
        PRINT_ERR("wpa_ctrl_open error\n");
        return WIFI_FAIL;
    }

    ret = wpa_ctrl_command(pstWpaCtl, cmd);
    if(ret < 0){
        PRINT_ERR("wpa_ctrl_command error\n");
        wpa_ctrl_close(pstWpaCtl);
        return WIFI_FAIL;
    }
    wpa_ctrl_close(pstWpaCtl);
    return WIFI_OK;
}


int net_wifi_cli_disconnect(const WIFI_HANDLE dev)
{
#ifdef WPA_CTL_INTERFACE
    return net_wpa_ctrl_command(dev, "DISCONNECT");
#else
    return net_wifi_cli_off(dev);
#endif

}

#define SEQ_MAX_VALUE 	8
int wpa_get_max_seq(const WIFI_HANDLE dev)
{
#ifdef WPA_CTL_INTERFACE
    char buf[2048];
	unsigned int len;
	int ret, i;
    int max_seq = -1;
    struct wpa_ctrl *pstWpaCtl;
    if(isNull(dev))
        return WIFI_FAIL;

    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)return WIFI_FAIL;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, "LIST_NETWORKS", strlen("LIST_NETWORKS"), buf, &len, NULL);
	if (ret == -2) {
		PRINT_ERR("command timed out.\n");
		return WIFI_TIMEOUT;
	} else if (ret < 0) {
		PRINT_ERR("command failed.\n");
		return WIFI_FAIL;
	}
	PRINT_INFO("buf:%s\n", buf);
    int tmp_seq = 0;
    for(i=0; i<sizeof(buf) && buf[i] != '\0'; i++){
        if(buf[i] == '\n'){
            tmp_seq = 0;
            i++;
            while(buf[i]>='0' && buf[i]<='9' && i < sizeof(buf)){
                tmp_seq *= 10;
                tmp_seq += buf[i++] - '0';
                if(tmp_seq > max_seq && tmp_seq < SEQ_MAX_VALUE)
                    max_seq = tmp_seq;
            }
        }
    }
    wpa_ctrl_close(pstWpaCtl);
    return max_seq;
#endif
    return 0;
}

int net_wifi_cli_get_network(const WIFI_HANDLE dev, int seq,
    int flag, char *buf, int buflen)
{
#ifdef WPA_CTL_INTERFACE
    int ret;
    struct wpa_ctrl *pstWpaCtl;
    char cmd[CMD_LEN_MAX*2] = {0};
    const char flag_arry[][12] ={"ssid", "psk", "key_mgmt",
        "indentity", "password", "bssid", "scan_ssid"};

    if(flag < 0 || flag > WIFI_SET_MAXNUM || buf == NULL)
        return WIFI_FAIL;

    ret = snprintf(cmd, sizeof(cmd), "GET_NETWORK %d %s",
        seq, flag_arry[flag]);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return WIFI_FAIL;
	cmd[sizeof(cmd) - 1] = '\0';
    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)return WIFI_FAIL;

    memset(buf, 0, buflen);
	ret = wpa_ctrl_request(pstWpaCtl, cmd, strlen(cmd), buf, (unsigned int*)&buflen, NULL);
    buf[buflen - 1] = '\0';
	if (ret < 0 || strcmp(buf, "FAIL") == 0) {
		PRINT_ERR("'%s' command failed.\n", cmd);
        wpa_ctrl_close(pstWpaCtl);
		return WIFI_FAIL;
	}
    wpa_ctrl_close(pstWpaCtl);

	return WIFI_OK;
#endif
}


int wifi_find_seq(const WIFI_HANDLE dev, const char *ssid)
{
    int i, ret;
    char des_ssid[128];
    int max_seq = -1;
    if(isNull(dev) || isNull(ssid))
        return WIFI_FAIL;
    int len = strlen(ssid);

    len += 1;
    len <<= 1;
    char *ascii_ssid = malloc(len);
    if(ascii_ssid == NULL)
        return WIFI_FAIL;
    max_seq = wpa_get_max_seq(dev);
    memset(ascii_ssid, 0, len);
    net_chartoascii(ssid, ascii_ssid);
	PRINT_INFO("ssid:%s \nascii ssid:%s\nmaxseq:%d\n", ssid, ascii_ssid, max_seq);

    for(i=0; i <= max_seq; i++)
    {
        ret = net_wifi_cli_get_network(dev, i,
            WIFI_SET_SSID, des_ssid, sizeof(des_ssid));
        if(ret < 0){
            free(ascii_ssid);
            return i;
        }
		PRINT_INFO("des ssid:%s\n", des_ssid);

		if(strncmp(ssid, &des_ssid[1], (IFNAMSIZ-1)) == 0){
            free(ascii_ssid);
            return i;
        }
        if(strncmp(ascii_ssid, des_ssid, ((IFNAMSIZ<<1)-1)) == 0){
            free(ascii_ssid);
            return i;
        }
    }
    free(ascii_ssid);
    return WIFI_FAIL;
}

int net_wifi_cli_disable_network(const WIFI_HANDLE dev, int seq)
{
#ifdef WPA_CTL_INTERFACE
    int ret;
    char cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "DISABLE_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return -1;
	cmd[sizeof(cmd) - 1] = '\0';

	return net_wpa_ctrl_command(dev, cmd);
#endif
}

int net_wifi_cli_add_network(const WIFI_HANDLE dev)
{
#ifdef WPA_CTL_INTERFACE
    char buf[2048];
	unsigned int len;
	int ret, i;
    struct wpa_ctrl *pstWpaCtl;
    if(isNull(dev))
        return WIFI_FAIL;

    pstWpaCtl = wpa_ctrl_open(dev, WPA_CTL_INTERFACE);
    if(pstWpaCtl == NULL)return WIFI_FAIL;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(pstWpaCtl, "ADD_NETWORK", strlen("ADD_NETWORK"), buf, &len, NULL);
	if (ret == -2) {
		printf("command timed out.\n");
		return WIFI_TIMEOUT;
	} else if (ret < 0) {
		printf("command failed.\n");
		return WIFI_FAIL;
	}

    if(buf[0]>='0' && buf[0]<='9'){
        ret = 0;
        for(i=0; buf[i]>='0' && buf[i]<='9'; i++){
            ret *= 10;
            ret = buf[i] - '0';
        }
        wpa_ctrl_close(pstWpaCtl);
        return ret;
    }

    wpa_ctrl_close(pstWpaCtl);
    return WIFI_FAIL;
#endif
	return WIFI_FAIL;
}

int net_wifi_cli_set_network(const WIFI_HANDLE dev, int seq,
        int flag, const char *content)
{
#ifdef WPA_CTL_INTERFACE
    int ret;
    char cmd[CMD_LEN_MAX*2] = {0};
    const char flag_arry[][12] ={"ssid", "psk", "key_mgmt",
        "indentity", "password", "bssid", "scan_ssid"};

    if(flag < 0 || flag > WIFI_SET_MAXNUM || content == NULL)
    {
        return WIFI_FAIL;
    }
	if(flag != WIFI_SET_KEY_MGMT
		&& flag != WIFI_SET_BSSID
		&& flag != WIFI_SET_SCAN_SSID)
	{
    	ret = snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"",
        	seq, flag_arry[flag], content);
	}
	else
	{
	 	ret = snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s",
        	seq, flag_arry[flag], content);
	}
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
	{
		return -1;
	}
	cmd[sizeof(cmd) - 1] = '\0';

	return net_wpa_ctrl_command(dev, cmd);
#endif
}

int net_wifi_cli_select_network(const WIFI_HANDLE dev, int seq)
{
#ifdef WPA_CTL_INTERFACE
    int ret;
    char cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return WIFI_FAIL;
	cmd[sizeof(cmd) - 1] = '\0';

	return net_wpa_ctrl_command(dev, cmd);
#endif
}

int net_wifi_cli_enable_network(const WIFI_HANDLE dev, int seq)
{
#ifdef WPA_CTL_INTERFACE
    int ret;
    char cmd[CMD_LEN_MIN] = {0};
    ret = snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", seq);
	if (ret < 0 || (size_t) ret >= sizeof(cmd))
		return WIFI_FAIL;
	cmd[sizeof(cmd) - 1] = '\0';

	return net_wpa_ctrl_command(dev, cmd);
#endif
}

int net_wifi_cli_reconnect(const WIFI_HANDLE dev)
{
#ifdef WPA_CTL_INTERFACE
    return net_wpa_ctrl_command(dev, "RECONNECT");
#else
    int ret;
    char cmd[128] = {0};
    if(net_wifi_cli_status(dev) == WIFI_STATUS_CONNECTED)
        net_wifi_cli_disconnect(dev);
    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd,"%s -B -i%s -c %s", WPA_SUPPLICANT_CMD, dev, PASSWD_CFG);
    ret = IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	if(usedhcp){
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd,"%s -b -i%s", UDHCPC_CMD, dev);
		IPC_MID_UTIL_SysCall(cmd, NULL, 0);
	}
#endif
}

static int _wifi_cli_udpchc(const WIFI_HANDLE dev)
{
    #if 0
	int ret = 0;
	char cmd[CMD_LEN_MAX] = {0};
	if(isNull(dev))
		return -1;
	sprintf(cmd,"%s -b -i%s -R -p %s", UDHCPC_CMD, dev, UDHCPC_PID);
    ret = IPC_MID_UTIL_ForkSysCall(cmd);
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd,"%s -2 `%s %s`", KILL_CMD, CAT_CMD, UDHCPC_PID);
    ret = IPC_MID_UTIL_ForkSysCall(cmd);
    return ret;
    #else

	char cmd[256] = {0};
	snprintf(cmd, sizeof(cmd), "%s|%s %s|%s -e \"2d\" -e \"s/^\\s*\\w*/&</\" "
		"-e \"s/<.*//\"|%s %s 2",
		PS_CMD, GREP_CMD, UDHCPC_CMD, SED_CMD, XARGS_CMD, KILL_CMD);
    printf("kill udpcpc cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

	return 0;
    #endif
}

int net_wifi_cli_get_bssid(const WIFI_HANDLE dev, char * const mac, int len)
{
	int ret;
	struct iwreq wreq;

	if(dev == NULL || mac == NULL || len == 0){
		PRINT_ERR("bad params\n");
		return WIFI_FAIL;
	}
	memset(&wreq, 0, sizeof(struct iwreq));
	ret = _wifi_ioctl(dev, SIOCGIWAP, &wreq);
	if (ret) {
		PRINT_ERR("IOCTL SIOCGIWAP Failed,error");
		return ret;
	}
	/* output current connecting AP MAC*/
	unsigned char *APaddr = (unsigned char *) wreq.u.ap_addr.sa_data;
	if(len < 3 * MAC_LEN -1)
        return WIFI_FAIL;
	else
        net_bytetomacstr(mac, APaddr, MAC_LEN);

	return WIFI_OK;
}

static int _wifi_cli_linkmsg(const WIFI_HANDLE dev, int seconds)
{
	int ret = WIFI_OK;
	int count = 0;

	sleep(3);
	seconds *= 10;
	do
	{
		usleep(300000); //sleep 100 ms
    	ret = net_wifi_cli_status(dev);
		if(ret == WIFI_STATUS_CONNECTED)//could wpa_supplicant no start connect.
		{
			break;
		}
		count++;
	}while((ret == WIFI_STATUS_CONNECTING)
		&& count < seconds); //seconds

    if(ret != WIFI_STATUS_CONNECTED){
        char mac[MAC_LEN*3] = {0};

		if(ret == WIFI_STATUS_DISCONNECTED && count < seconds)
            return WIFI_LINK_ERROR;

        ret = net_wifi_cli_get_bssid(dev, mac, sizeof(mac));
        net_wifi_cli_disconnect(dev);
        if(ret < 0)
            return WIFI_TIMEOUT;
        else if(strcmp(mac, MAC_EMPTY) == 0)
            return WIFI_NO_SSID;
		else
			return WIFI_TIMEOUT;
    }
	return WIFI_OK;
}


//net_wifi_cli_connecting("ra0", NULL,  linkInfo->linkEssid, linkInfo->linkPsd, 0);
/* dev is wifi device name, name is SSID of AP, passwd is link key, usedhcp is wether use udhcpc (0:no use !0:use) */
int net_wifi_cli_connecting(const WIFI_HANDLE dev, SCAN_DataT *link_data,
    const char *ssid, const char *key, unsigned char usedhcp)
{
    int ret = 0;
    int seq = -1;
    const char *dev_ssid = NULL;

	if(dev == NULL || (link_data == NULL && ssid == NULL) ){
		PRINT_ERR("Bad params device[0x%p] link_data[0x%p] ssid[0x%p]\n",
			dev, link_data, ssid);
		return WIFI_FAIL;
	}

    #if 1
	ret = net_wifi_cli_status(dev);
	if(ret >= 0){ // ����Ѿ����ӻ����������ӣ���Ͽ�
		ret = net_wifi_cli_disconnect(dev);
		if(ret < 0){
            PRINT_ERR("net_wifi_cli_disconnect error\n");
	        return WIFI_FAIL;
		}
		usleep(10*1000);
	}
    #endif

    if(ssid != NULL) {
        dev_ssid = ssid;
    } else {
        //dev_ssid = link_data->ssid.name;
        dev_ssid = link_data->ssid;
    }
	/* SSID string length > 0 */
	if(dev_ssid[0] == '\0'){
        PRINT_ERR("No ssid\n");
		return WIFI_NO_SSID;
	}

    printf("dev:%s   ssid:%s\n", dev, dev_ssid);
    /* find seq of ssid, if not have seq return -1; */
    seq = wifi_find_seq(dev, dev_ssid);
    if(seq >= 0){
		ret = net_wifi_cli_disable_network(dev, seq);
		if(ret < 0){
	        return WIFI_FAIL;
		}
    } else {
		seq = net_wifi_cli_add_network(dev);
	    if(seq < 0){
			return WIFI_FAIL;
	    }
    }
	PRINT_ERR("Find network seq: %d\n", seq);
	/* set wireless SSID*/
    ret = net_wifi_cli_set_network(dev, seq, WIFI_SET_SSID, dev_ssid);
    if(ret < 0){
        PRINT_ERR("Set network SSID fail\n");
        return WIFI_FAIL;
    }
	ret = net_wifi_cli_set_network(dev, seq, WIFI_SET_SCAN_SSID, "1");
    if(ret < 0){
        PRINT_ERR("Set network SCAN_SSID fail\n");
        return WIFI_FAIL;
    }
    /* set psk */
	if(key != NULL && key[0] != 0){
	    ret = net_wifi_cli_set_network(dev, seq, WIFI_SET_PSK, key);
	    if(ret < 0){
	        PRINT_ERR("Password is less than eight or format error\n");
	        return WIFI_FAIL;
	    }
	}
	else
	{
	    ret = net_wifi_cli_set_network(dev, seq, WIFI_SET_KEY_MGMT, "NONE");
	    if(ret < 0){
	        PRINT_ERR("Set network fail\n");
	        return WIFI_FAIL;
	    }
	}

    ret = net_wifi_cli_select_network(dev, seq);
    if(ret < 0){
        return WIFI_FAIL;
    }

    ret = net_wifi_cli_enable_network(dev, seq);
    if(ret < 0){
        return WIFI_FAIL;
    }

    ret = net_wifi_cli_reconnect(dev);
    if(ret < 0){
        return WIFI_FAIL;
	}

	ret = _wifi_cli_linkmsg(dev, 15);//15 second
    if(usedhcp && (ret == WIFI_OK)) {
		_wifi_cli_udpchc(dev);
	}

	if(ret != WIFI_OK) {
		net_wifi_cli_disconnect(dev);
	}
    return ret;
}


