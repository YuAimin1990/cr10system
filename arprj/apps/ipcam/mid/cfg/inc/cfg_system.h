#ifndef _CFG_SYSTEM_H__
#define _CFG_SYSTEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

/******************************************/
/*** device_info, time, zone, dst, NTP  ***/

/******************************************/

#define INVALID_TIEM_ZONE  0XFFFF

typedef enum {
    PAL = 0,
    NTSC,
} VIDEO_STANDARD_E;

typedef struct {
    AR_CHAR    deviceName[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    manufacturer[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    deviceType[MAX_SYSTEM_STR_SIZE];
    AR_S32   sensorType; // 0: IMX222; 1 OV9710 2 ...
    AR_S32   languageType;
    VIDEO_STANDARD_E videoType; //pal, ntsc
    AR_S32   isUpdateAPSSid;

    AR_CHAR    serial_0[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    serial_1[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    serial_2[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    serial_3[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    softwareVersion[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    softwareBuildDate[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    firmwareVersion[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    firmwareReleaseDate[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    hardwareVersion[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    hardwareBuildDate[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    webVersion[MAX_SYSTEM_STR_SIZE];
    AR_CHAR    webBuildDate[MAX_SYSTEM_STR_SIZE];

	AR_S32   updateSoftware;
} NET_DEVICE_INFO;

typedef struct  {
    AR_U32 month;
    AR_U32 weekNo;
    AR_U32 weekDate;
    AR_U32 hour;
    AR_U32 min;
} NET_TIMEPOINT;

typedef enum {
    GMT_NEG_12 = -12,
    GMT_NEG_11 = -11,
    GMT_NEG_10 = -10,
    GMT_NEG_9  = -9,
    GMT_NEG_8  = -8,
    GMT_NEG_7  = -7,
    GMT_NEG_6  = -6,
    GMT_NEG_5  = -5,
    GMT_NEG_4  = -4,
    GMT_NEG_3  = -3,
    GMT_NEG_2  = -2,
    GMT_NEG_1  = -1,
    GMT_0      = 0,
    GMT_POS_1  = 1,
    GMT_POS_2  = 2,
    GMT_POS_3  = 3,
    GMT_POS_4  = 4,
    GMT_POS_5  = 5,
    GMT_POS_6  = 6,
    GMT_POS_7  = 7,
    GMT_POS_8  = 8,
    GMT_POS_9  = 9,
    GMT_POS_10 = 10,
    GMT_POS_11 = 11,
    GMT_POS_12 = 12,
}TimeZone_E;

typedef struct {
    AR_S32     timezone;   //-720, +720

} NET_ZONE;

typedef struct {
    AR_S32    enableDST;
    AR_S32    dSTBias;
    AR_S8     beginTime[MAX_TIME_STR_SIZE];
    AR_S8     endTime[MAX_TIME_STR_SIZE];
} NET_DST;

typedef struct {
    AR_S32   enable;
    AR_S8    serverDomain[MAX_URL_STR_SIZE];
} NTP_CFG;

typedef struct {
    AR_U8      enable;
    AR_U8      index;
    AR_U8      hour;
    AR_U8      minute;
    AR_U8      second;
} MAINTAIN_CFG;

typedef struct {
    AR_U8      wdt_off;
    //other debug options
} DEBUG_CFG;


typedef struct {
    NET_DEVICE_INFO deviceInfo;
    NET_ZONE timezoneCfg;
    NET_DST netDstCfg;
    NTP_CFG ntpCfg;
	MAINTAIN_CFG maintainCfg;
    DEBUG_CFG debugCfg;
} IPC_CFG_SYSTEM_S;

AR_S32 IPC_CFG_SYSTEM_Save();
AR_S32 IPC_CFG_SYSTEM_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_SYSTEM_LoadDefault();
void IPC_CFG_SYSTEM_Print();
IPC_CFG_SYSTEM_S * IPC_CFG_SYSTEM_GetParam();

#define SYSTEM_INFO_FILE "/root/sys_info"
#define SYSTEM_CFG_FILE "cfg_system.json"

#define P2PID_CFG_PATH "/mnt/sd_card/p2p.txt"
#define P2PID_TMP_CFG_PATH "/mnt/sd_card/p2p_tmp.txt"
#define P2PID_BAK_CFG_PATH "/mnt/sd_card/p2p_bak.txt"

#ifdef __cplusplus
}
#endif
#endif

