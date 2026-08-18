#ifndef __MID_UTIL_NET_H__
#define __MID_UTIL_NET_H__

#include "util_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_WIFI_NAME   32


//0: none  (wifi enable)
//1: wep 64 assii (wifi enable)
//2: wep 64 hex (wifi enable)
//3: wep 128 assii (wifi enable)
//4: wep 128 hex (wifi enable)
//5: WPAPSK-TKIP
//6: WPAPSK-AES
//7: WPA2PSK-TKIP
//8: WPA2PSK-AES
#define WPAECN_NONE     0
#define WEP_64_ASSCI    1
#define WEP_64_HEX      2
#define WEP_128_ASSCI   3
#define WEP_128_HEX     4
#define WPAPSK_TKIP     5
#define WPAPSK_AES      6   
#define WPA2PSK_TKIP    7
#define WPA2PSK_AES     8

#define WPA_AES_ENC "[WPA-PSK-CCMP]"
#define WPA_TKIP_ENC "[WPA-PSK-TKIP+CCMP]"
#define WPA2_AES_ENC "[WPA2-PSK-CCMP]"
#define WPA2_TKIP_ENC "[WPA2-PSK-TKIP+CCMP]"
#define WPS_ENC "[WPS]"


#define IWLIST_CMD	"iwlist"

typedef enum
{
    WIFI_ENC_NONE = 0,
    WIFI_ENC_WEP_64_ASSII,
    WIFI_ENC_WEP_64_HEX,
    WIFI_ENC_WEP_128_ASSII,
    WIFI_ENC_WEP_128_HEX,
    WIFI_ENC_WPAPSK_TKIP,
    WIFI_ENC_WPAPSK_AES,
    WIFI_ENC_WPA2PSK_TKIP,
    WIFI_ENC_WPA2PSK_AES
}WIFI_ENC_MODE;

typedef struct
{
    int				vaild;
    char            essid[MAX_WIFI_NAME];
    int             quality;    /* signal quality */
    char            mac[MAX_WIFI_NAME];
    WIFI_ENC_MODE   security;
    				//0: none  (wifi enable)
    				//1: wep 64 assii (wifi enable)
    				//2: wep 64 hex (wifi enable)
    				//3: wep 128 assii (wifi enable)
    				//4: wep 128 hex (wifi enable)
    				//5: WPAPSK-TKIP
    				//6: WPAPSK-AES
    				//7: WPA2PSK-TKIP
    				//8: WPA2PSK-AES
    int             channel;
}WIFI_SCAN_LIST_t;

typedef struct tagNETCAM_WIFI_SCAN_LIST
{
    int count;
    WIFI_SCAN_LIST_t list[20];
} NETCAM_WIFI_SCAN_LIST;


#define _PATH_PROCNET_ROUTE        "/proc/net/route"
#define _PATH_DNS_CONT             "/etc/resolv.conf"
#define PACKET_SIZE     4096  

int IPC_MID_UTIL_GetIP(const char *adapter_name, char *addr);
int IPC_MID_UTIL_GetMac(const char *adapter_name, char *mac);
int IPC_MID_UTIL_GetGateway(const char *adapter_name, char *gateway);
int IPC_MID_UTIL_GetNetmask(const char *adapter_name, char *netmask);
int IPC_MID_UTIL_GetDns(char *dns1, char *dns2);
int IPC_MID_UTIL_Ping( char *ips, int timeout);
int IPC_MID_UTIL_WifiScan(const char *adapter_name, NETCAM_WIFI_SCAN_LIST *plist);
int IPC_MID_UTIL_WifiScanAP(const char* dev, NETCAM_WIFI_SCAN_LIST *plist);
void IPC_MID_UTIL_PrintAPList(NETCAM_WIFI_SCAN_LIST *plist);
void IPC_MID_UTIL_SortAPList(NETCAM_WIFI_SCAN_LIST *plist);

#ifdef __cplusplus
}
#endif

#endif /* _UTI_NET_H__ */

