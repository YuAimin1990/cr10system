#ifndef _CFG_SHELTER_H__
#define _CFG_SHELTER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

#define MAX_CHANNEL_ITEM_NUM 30
typedef enum
{
    FONT_4X6_ID = 1,
    FONT_5X8_ID = 2,
    FONT_5X12_ID = 3,
    FONT_6X8_ID = 4,
    FONT_6X10_ID = 5,
    FONT_7X12_ID = 6,
    FONT_8X8_ID = 7,
    FONT_8X12_ID = 8,
    FONT_8X12_CY_ID = 9,
    FONT_8X14_ID = 10,
    FONT_10X16_ID = 11,
    FONT_12X16_ID = 12,
    FONT_12X20_ID = 13,
    FONT_16X26_ID = 14,
    FONT_22X36_ID = 15,
    FONT_24X40_ID = 16,
    FONT_32X53_ID = 17,
    FONT_BUTT_ID
} OSD_FONT_IDX_E;

/***********************************/
/***         osd                 ***/
/***********************************/
typedef struct {
    AR_S32   enable;
    AR_S8    text[MAX_STR_LEN_128];
} NET_OSD_CHANNEL_NAME;

typedef struct {
    AR_S32    enable;
    AR_U8     dateFormat;
    AR_U8     dateSprtr;
    AR_U8     timeFmt;
	AR_U8     displayWeek;
} NET_OSD_DATETIME;

typedef struct {
    AR_FLOAT   x_ratio;
    AR_FLOAT   y_ratio;
    NET_OSD_CHANNEL_NAME osdChannelName;
    NET_OSD_DATETIME     osdDatetime;
    AR_CHAR    achAttachedTo[16]; //attached to "vpss" or "venc"/"vo"
    AR_U32     u32AlgOsdEnable; //Allow algrithom to draw osd on this channel
    AR_U32     u32RGB; //bit31 ~ 0: 0RGB
    OSD_FONT_IDX_E enFontSize;
} NET_OSD_INFO;

typedef struct {
    NET_OSD_INFO osdInfo[IPC_MAX_PIPELINE_NUM][IPC_MAX_STREAM_PER_PIPE];
} IPC_CFG_OSD_S;

extern AR_S32 IPC_CFG_OSD_Save();
extern AR_S32 IPC_CFG_OSD_Load(CFG_RUN_MODE_E enMode);
extern AR_S32 IPC_CFG_OSD_LoadDefault();
extern void IPC_CFG_OSD_Print();
extern IPC_CFG_OSD_S * IPC_CFG_OSD_GetParam();

#define OSD_CFG_FILE "cfg_osd.json"

#ifdef __cplusplus
}
#endif
#endif

