#ifndef     __NET_CONFIG_H__ 
#define     __NET_CONFIG_H__

/* proc command  */
#if 0
#define IFCONFIG_CMD	"/sbin/ifconfig"
#define REOUTE_CMD		"/sbin/route"
#define WPA_SUPPLICANT_CMD	"/usr/local/bin/wpa_supplicant"
#define UDHCPC_CMD		"/sbin/udhcpc"
#define UDHCPD_CMD		"/sbin/udhcpd"
#define SED_CMD			"/bin/sed"
#define INSMOD_CMD		"/sbin/insmod"
#define RMMOD_CMD		"/sbin/rmmod"
#define XARGS_CMD		"/usr/bin/xargs"
#define FIND_CMD		"/usr/bin/find"
#define LSMOD_CMD		"/sbin/lsmod"
#define GREP_CMD		"/bin/grep"
#define CAT_CMD			"/bin/cat"
#define KILL_CMD		"/bin/kill"
#define PS_CMD			"/bin/ps"
#define LSUSB_CMD		"/usr/bin/lsusb"
#else
#define IFCONFIG_CMD	"ifconfig"
#define REOUTE_CMD		"route"
#define WPA_SUPPLICANT_CMD	"wpa_supplicant"
#define UDHCPC_CMD		"udhcpc"
#define UDHCPD_CMD		"udhcpd"
#define SED_CMD			"sed"
#define INSMOD_CMD		"insmod"
#define RMMOD_CMD		"rmmod"
#define XARGS_CMD		"xargs"
#define FIND_CMD		"find"
#define LSMOD_CMD		"lsmod"
#define GREP_CMD		"grep"
#define CAT_CMD			"cat"
#define KILL_CMD		"kill"
#define PS_CMD			"ps"
#define LSUSB_CMD		"lsusb"
#define WPA_CLI_CMD	    "wpa_cli"
#endif


/* define driver name */
#define DRIVER_MT7601U	1 //use mt7601u
#define DRIVER_RT3070	0
//add to

/* define whether use STA */
#define NETWORK_STA
/* define whether use AP */
//#define NETWORK_AP

#ifdef 	NETWORK_STA
#define NETWORK_ELIAN_SUPPORT	//support smart link
#define NETWORK_WPS_SUPPORT		//support WPS
#endif

#ifdef 	DRIVER_MT7601U
#define MT7601_INAME              "wlan0"
#define DRIVER_PATH               "/ko"
#define	DRIVER_MT7601_STA		  "mt7601Usta"
#define DRIVER_MT7601_AP		  "mt7601Usta"
#define	DRIVER_STA		          DRIVER_MT7601_STA
#define DRIVER_AP		          DRIVER_MT7601_AP
#define DRIVER_MT7601_AP_DEF_IP   "192.168.234.1"
#define DRIVER_MT7601_STA_CFG	  "/opt/custom/wifi/MT7601USTA.dat.dat"
#define DRIVER_MT7601_AP_CFG	  "/opt/custom/wifi/RT2870AP.dat"
#define HOSTAPD_CFG               "/opt/custom/wifi/hostapd.conf"
//#define DRIVER_STA_CFG	      "/etc/Wireless/RT2870STA/RT2870STA_7601.dat"
//#define DRIVER_AP_CFG	          "/etc/Wireless/RT2870AP/RT2870AP.dat"
#define DRIVER_STA_CFG            DRIVER_MT7601_STA_CFG
#define DRIVER_AP_CFG             DRIVER_MT7601_AP_CFG

#define UDHCPD_CFG_PATH         "/opt/custom/wifi/udhcpd.conf"
//#define WPA_CTL_INTERFACE       "/tmp/wpa_supplicant"
//#define PASSWD_CFG 		        "/tmp/wpa.conf"
#define WPA_CTL_INTERFACE       "/var/run/wpa_supplicant"
#define PASSWD_CFG 		        "/opt/custom/wifi/wpa.conf"

//#define NET_WIFI_VAULE_MT7601_AP	"/etc/Wireless/RT2870AP/RT2870AP.dat"
//#define NET_WIFI_VAULE_MT7601_STA	"/etc/Wireless/RT2870STA/RT2870STA_7601.dat"
#define NET_WIFI_VAULE_MT7601_AP	DRIVER_MT7601_AP_CFG
#define NET_WIFI_VAULE_MT7601_STA	DRIVER_MT7601_STA_CFG

#elif	DRIVER_RT3070
#define	DRIVER_STA		"rt3070sta"
#define DRIVER_STA_CFG	"/etc/Wireless/RT2870STA/RT2870STA.dat"
#define DRIVER_AP		"rt3070ap"
#define DRIVER_AP_CFG	"/etc/Wireless/RT2870AP/RT2870AP.dat"
#endif

#define UDHCPD_CFG		"/etc/udhcpd.conf"

#endif
