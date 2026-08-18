#ifndef __CFG_NETWORK_H__
#define __CFG_NETWORK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

#define WIFI_CONNECT_LIST_MAX_NUMBER 20

/***********************************/
/***       network               ***/
/***********************************/
typedef enum {
    NET_TYPE_DHCP = 0,
    NET_TYPE_STATIC,
} NET_TYPE_E;

typedef struct {
    AR_S32  enable;
    AR_S8   netName[MAX_STR_LEN_64];
    AR_S32  ipVersion; //v4,v6
    AR_S8   mac[MAX_STR_LEN_20];
    AR_S32  dhcpIp; // 0 static, 1 dhcp
    AR_S32  upnpEnable; // 0 disable, 1 enable
    AR_S8   ip[MAX_STR_LEN_16];
    AR_S8   netmask[MAX_STR_LEN_16];
    AR_S8   gateway[MAX_STR_LEN_16];
    AR_S8   multicast[MAX_STR_LEN_16];
    AR_S32  autoDns; // 0 static, 1 dhcp
    AR_S8   dns1[MAX_STR_LEN_16];
    AR_S8   dns2[MAX_STR_LEN_16];
} NET_ETH;

/**UPNP**/
typedef struct {
    AR_U32  enable;
    AR_U32  mode;
    AR_U32  lineMode;
    AR_S8   serverIp[MAX_STR_LEN_32];
    AR_U32  dataPort;
    AR_U32  webPort;
    AR_U32  mobilePort;
    AR_U16  dataPort1;
    AR_U16  webPort1;
    AR_U16  mobilePort1;
    AR_U16  dataPortOK;
    AR_U16  webPortOK;
    AR_U16  mobilePortOK;
} UPNP_CFG;

typedef struct {
    AR_U32 enable;
    AR_S8  user[MAX_STR_LEN_64];
    AR_S8  password[MAX_STR_LEN_64];
} NET_PPPOECFG;

typedef enum {
    DDNS_DYNDNS = 0,
    DDNS_3322,
    DDNS_NIGHTOWLDVR,
    DDNS_NOIP,
    DDNS_MYEYE,
    DDNS_PEANUTHULL,
    DDNS_CHANGEIP,
    DDNS_POPDVR,
    DDNS_SKYBEST,
    DDNS_DVRTOP,
    DDNS_BUTT
} DDNS_TYPE;

typedef struct {
    AR_U32     enable;
    DDNS_TYPE type;
    AR_S8      username[MAX_STR_LEN_64];
    AR_S8      password[MAX_STR_LEN_64];
    AR_S8      domain[MAX_STR_LEN_256];
    AR_S8      address[MAX_STR_LEN_256];
    AR_S32     port;
} NET_DDNSCFG;

typedef struct {
    AR_S32   enable;
    AR_S32    attachPicture;
    AR_U8    smtpServerVerify;
    AR_U8    mailInterval;

    AR_S8    eMailUser[MAX_STR_LEN_64];
    AR_S8    eMailPass[MAX_STR_LEN_64];
    AR_S32   encryptionType;
    AR_S8    smtpServer[MAX_STR_LEN_128];
    AR_S32   smtpPort;
    AR_S8    pop3Server[MAX_STR_LEN_128];
    AR_S32   pop3Port;
    AR_S8    fromAddr[MAX_STR_LEN_64];
    AR_S8    toAddrList0[MAX_STR_LEN_64];
    AR_S8    toAddrList1[MAX_STR_LEN_64];
    AR_S8    toAddrList2[MAX_STR_LEN_64];
    AR_S8    toAddrList3[MAX_STR_LEN_64];
    AR_S8    ccAddrList0[MAX_STR_LEN_64];
    AR_S8    ccAddrList1[MAX_STR_LEN_64];
    AR_S8    ccAddrList2[MAX_STR_LEN_64];
    AR_S8    ccAddrList3[MAX_STR_LEN_64];
    AR_S8    bccAddrList0[MAX_STR_LEN_64];
    AR_S8    bccAddrList1[MAX_STR_LEN_64];
	AR_S8	  bccAddrList2[MAX_STR_LEN_64];
	AR_S8	  bccAddrList3[MAX_STR_LEN_64];
} NET_EMAIL_PARAM;

typedef struct {
    AR_S32   enable;
    AR_S8    address[MAX_STR_LEN_128];
    AR_S32   port;
    AR_S8    userName[MAX_STR_LEN_64];
    AR_S8    password[MAX_STR_LEN_64];
	AR_S8    datapath[MAX_STR_LEN_128];
	AR_S8    filename[MAX_STR_LEN_128];
} NET_FTP_PARAM;

//Need to add more settings
typedef struct {
    AR_S32 enable;
    AR_S32 mode; //0 AP, 1 stationMode, 2 NFS
    AR_S32 channel; // ["Auto","1","2","3","4","5","6","7","8","9","10","11","12","13","14"]
    AR_S32 wpaMode; //["WPA_PSK","WPA2_PSK"]
    AR_S32 dhcp; //0 static ip, 1 dhcp. only for AP mode.
    AR_U8  standard[MAX_STR_LEN_16]; //["802.11b","802.11g","802.11n","802.11bg","802.11bgn"]
    AR_U8  ssid[MAX_STR_LEN_16]; //as ap, this is the ssid. as sta, this is the ap ssid to join
    AR_U8  password[MAX_STR_LEN_16];
} NET_WIRELESS_CFG;

typedef struct {
    AR_S32            httpPort;
    AR_S32            rtspPort;   //554
} NET_PORT_CFG;

typedef struct {
    NET_ETH          lan;
    NET_WIRELESS_CFG wireless;
    UPNP_CFG         upnp;
    NET_PPPOECFG     pppoe;
    NET_DDNSCFG      ddns;
    NET_EMAIL_PARAM  email;
    NET_FTP_PARAM    ftp;
    NET_PORT_CFG     port;
} IPC_CFG_NETWORK_S;

extern AR_S32 IPC_CFG_NETWORK_Save();
extern AR_S32 IPC_CFG_NETWORK_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_NETWORK_LoadDefault();
extern void IPC_CFG_NETWORK_Print();
extern IPC_CFG_NETWORK_S * IPC_CFG_NETWORK_GetParam();

#define NETWORK_CFG_FILE "cfg_network.json"

#ifdef __cplusplus
}
#endif
#endif


