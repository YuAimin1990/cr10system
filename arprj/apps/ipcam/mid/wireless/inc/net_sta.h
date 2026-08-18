#ifndef     __NET_STA_H__
#define     __NET_STA_H__

#include 	"util_net.h"
#include 	"net_common.h"


/* proc command  */
//#define WPA_SUPPLICANT_CMD	"wpa_supplicant"


typedef struct gen_ie_data_s
{
    unsigned char use_flag;/* 1, is used */
    unsigned char ie;       /* ie way */
    unsigned char group_chip;
    unsigned int version;
    unsigned char pair_chip[4];
    unsigned char auth_suite[4];
} GEN_IE_DataT;

typedef struct ssid_data_s
{
	char name[32];
	int name_len;
}SSID_DataT;

typedef struct scand_data_s
{
    int used;            /* if use = 1, is used */
	int qual;			/* signal quality */
	int noise;			/* Level + Noise are dBm */
	int level;
    unsigned int mode;  /* work mode */
	unsigned int channel;/* AP used channel */
    unsigned int maxrate; /* max reate, 802.11 rates 500000 b/s units */
	unsigned int encode;    /* AP encode way */
    char        key[32];    /* encode key */
    char        protocol[32]; /* protocol name */
	unsigned char mac[14];		/* AP mac address (string) */
	char 		ssid[32];	/* AP ssid (string) */
    GEN_IE_DataT gen_ie[4]; /* ie , Generic IE (WPA, RSN, WMM, ..)*/
}SCAN_DataT;

typedef struct elian_result {
	unsigned char auth_mode;
	unsigned char cust_data_len;
	char ssid[33];
	char pwd[65];
	char user[65];
	char cust_data[256];
} ELIAN_ResultT;

typedef void *(*fCallBack)(WirelessHandle, ELIAN_ResultT *);


#ifndef DISABLE_STA_HANDLE
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
} StaObjT;
#ifdef __cplusplus
extern "C" {
#endif
extern StaObjT *CreateNewSta(const s8 *ifname);
extern void DestoryOldSta(StaObjT *staObj);

int net_wifi_cli_scan(const WIFI_HANDLE dev);
int net_wifi_cli_scan_result(const WIFI_HANDLE dev,WIFI_SCAN_LIST_t *list, int *number);

int _staOpenWpasupplicant(const s8 *ifname);
void _staCloseWpasupplicant(void);

int net_wifi_cli_status(const WIFI_HANDLE dev);
int net_wifi_cli_disconnect(const WIFI_HANDLE dev);

#ifdef __cplusplus
}
#endif
#endif
#endif
