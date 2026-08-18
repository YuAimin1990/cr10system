#ifndef      __NET_TYPES_H__ 
#define     __NET_TYPES_H__

#define     VERSIONS    1.0

/*################ define data types ####################*/
#ifndef s32
#define s32 	int
#endif
#ifndef u32
#define u32 	unsigned int
#endif
#ifndef s16
#define s16 	short
#endif
#ifndef u16
#define u16 	unsigned short
#endif
#ifndef s8
#define s8 		char
#endif
#ifndef u8
#define u8 		unsigned char
#endif
#ifndef bool
#define bool 	unsigned char
#endif

#ifndef true
#define true    1
#endif
#ifndef false
#define false   0
#endif

#define isNull(n) ((n) == NULL)
#define nMAX(n, m) ((n)>(m)?(n):(m))
#define nMIN(n, m) ((n)<(m)?(n):(m))
#define sMAX(n, m) (strcmp((n), (m))>0?(n):(m))
#define sMIN(n, m) (strcmp((n), (m))<0?(n):(m))


#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
		((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif


#if __GNUC_PREREQ(3,0) && defined(i386) /* || defined(__x86_64__)? */
/* stdcall makes callee to pop arguments from stack, not caller */
# define FAST_FUNC __attribute__((regparm(3),stdcall))
/* #elif ... - add your favorite arch today! */
#else
# define FAST_FUNC
#endif


/**************error table******************/
#define NET_OK                      0
#define NET_ERR_BADPARAM            -1 
#define NET_ERR_OUTOFMEMORY         -2 
#define NET_ERR_ALREADINIT          -3 
#define NET_ERR_NOTINIT             -4 
#define NET_ERR_NOSUPPORT           -5
#define NET_ERR_TIMEOUT             -6
#define NET_ERR_DEVICEBUSY          -7
#define NET_ERR_INVALIDHANDLE       -8
#define NET_ERR_NOSUPPORT_VERSION   -9
#define NET_ERR_NOTOPEN             -10
#define NET_ERR_FROMDRIVER          -11 

/******************error code base*******************/
#define NET_WIRELESS_BASE	-1000
#define NET_STA_BASE        -2000
#define NET_AP_BASE         -3000


#define NET_STA_OK                  0
#define NET_STA_BADPARAM          (NET_ERR_BADPARAM            + NET_STA_BASE)
#define NET_STA_OUTOFMEMORY       (NET_ERR_OUTOFMEMORY         + NET_STA_BASE)
#define NET_STA_ALREADINIT        (NET_ERR_ALREADINIT          + NET_STA_BASE)
#define NET_STA_NOTINIT           (NET_ERR_NOTINIT             + NET_STA_BASE)
#define NET_STA_NOSUPPORT         (NET_ERR_NOSUPPORT           + NET_STA_BASE)
#define NET_STA_TIMEOUT           (NET_ERR_TIMEOUT             + NET_STA_BASE)
#define NET_STA_DEVICEBUSY        (NET_ERR_DEVICEBUSY          + NET_STA_BASE)
#define NET_STA_INVALIDHANDLE     (NET_ERR_INVALIDHANDLE       + NET_STA_BASE)
#define NET_STA_NOSUPPORT_VERSION (NET_ERR_NOSUPPORT_VERSION   + NET_STA_BASE)
#define NET_STA_NOTOPEN           (NET_ERR_NOTOPEN             + NET_STA_BASE)
#define NET_STA_FROMDRIVER        (NET_ERR_FROMDRIVER          + NET_STA_BASE)

/* encode way */
#define WIFI_ENCRYPT_UNKNOW         0
#define WIFI_ENCRYPT_OFF            1
#define WIFI_ENCRYPT_OPEN           2
#define WIFI_ENCRYPT_RESTRICTED     3
#define WIFI_ENCRYPT_ENABLED        4
#define WIFI_ENCRYPT_TEMP           5
#define WIFI_ENCRYPT_ON             6
#define WIFI_ENCRYPT_MAXMUN         6


/* Modes of operation */
#define WIFI_MODE_AUTO	0	/* Let the driver decides */
#define WIFI_MODE_ADHOC	1	/* Single cell network */
#define WIFI_MODE_INFRA	2	/* Multi cell network, roaming, ... */
#define WIFI_MODE_MASTER	3	/* Synchronisation master or Access Point */
#define WIFI_MODE_REPEAT	4	/* Wireless Repeater (forwarder) */
#define WIFI_MODE_SECOND	5	/* Secondary master/repeater (backup) */
#define WIFI_MODE_MONITOR	6	/* Passive monitor (listen only) */
#define WIFI_MODE_MESH	7	/* Mesh (IEEE 802.11s) network */

/* group_chip || pair_chip */
#define WIFI_CHIP_UNKNOW          0
#define WIFI_CHIP_NONE            1
#define WIFI_CHIP_WEP_40          2
#define WIFI_CHIP_TKIP            3
#define WIFI_CHIP_WRAP            4
#define WIFI_CHIP_CCMP            5
#define WIFI_CHIP_WEP_104         6
#define WIFI_CHIP_PROPRIETARY     7
#define WIFI_CHIP_MAXNUM          7


/* auth_suite   */
#define WIFI_AUTH_UNKNOW            0
#define WIFI_AUTH_NONE              1
#define WIFI_AUTH_802_1_X           2
#define WIFI_AUTH_PSK               3
#define WIFI_AUTH_PROPRIETARY       4
#define WIFI_AUTH_MAXNUM            4


/* ie */
#define WIFI_IE_UNKNOW              0
#define WIFI_IE_WPA1                1
#define WIFI_IE_WPA2                2
#define WIFI_IE_MAXNUM              2

/*  wifi connect status  */
#define WIFI_STATUS_ERROR           -1  //error
#define WIFI_STATUS_DISCONNECTED    0   //disconnect
#define WIFI_STATUS_CONNECTING      1   //connecting
#define WIFI_STATUS_CONNECTED       2   //connect complete
#define WIFI_STATUS_INACTIVE        3   //network is no active
#define WIFI_STATUS_UNKNOW        	4	//get status is unknow

/* control command resulte*/
#define WIFI_OK             0
#define WIFI_FAIL           -1      //function fail
#define WIFI_TIMEOUT        -2      //Link tmeout
#define WIFI_NO_SSID        -3      //no ssid
#define WIFI_LINK_ERROR		-4      //link error



/* set network flags */
#define WIFI_SET_SSID       0
#define WIFI_SET_PSK        1
#define WIFI_SET_KEY_MGMT   2
#define WIFI_SET_INDENT     3
#define WIFI_SET_PASSWORD   4
#define WIFI_SET_BSSID   	5
#define WIFI_SET_SCAN_SSID	6
#define WIFI_SET_MAXNUM     6


/*################ define normal function ###############*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
