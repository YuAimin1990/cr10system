#ifndef __PF_OSD_H__
#define __PF_OSD_H__

#include "util_common.h"
#include "util_api.h"
#include "util_timer.h"
#include "cfg_all.h"
#include "ar_comm_video.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IPC_ALG_OSD_MAX_NUM   32
#define IPC_ALG_OSD_STR_MAX_LEN   32
typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32RGB; //RGB888
    AR_CHAR achStr[IPC_ALG_OSD_STR_MAX_LEN];
} IPC_OSD_STRING_S;
typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32W;
    AR_U32 u32H;
    AR_U32 u32RGB; //RGB888
} IPC_OSD_RECT_S;

typedef struct
{
    AR_U32 u32X0;
    AR_U32 u32Y0;
    AR_U32 u32X1;
    AR_U32 u32Y1;
    AR_U32 u32RGB;
} IPC_OSD_GEOM_S;


typedef struct
{
    PIXEL_FORMAT_E enPixelFormat;
    AR_U32 u32Width; //bitmap width
    AR_U32 u32Height; //bitmap height
    void * pData; //bitmap data, usually it's the canvas addr.
} IPC_OSD_BITMAP_S;

typedef struct{
    AR_S32 valid; //if this rgn is created or attached to some chn or not.
	AR_S32 channel;
	AR_S32 stream;
	AR_S32 x;
	AR_S32 y;
    OSD_FONT_IDX_E enFontSizeId;
    AR_CHAR text[128];
    AR_UCHAR * pDotMatrix; //dot matrix of each ascii character
    IPC_OSD_BITMAP_S stBmp;
} IPC_PF_OSD_PARAM_S;

AR_S32 IPC_PF_OSD_Init();
void IPC_PF_OSD_UnInit();

//This is for other App(AI algrithom) to draw osd on video
AR_S32 IPC_PF_OSD_DrawString(VPSS_GRP VpssGrp, AR_U32 u32StrNum, IPC_OSD_STRING_S * pastStr, AR_U32 u32ImgW, AR_U32 u32ImgH);
AR_S32 IPC_PF_OSD_DrawRect(VPSS_GRP VpssGrp, AR_U32 u32RectNum, IPC_OSD_RECT_S * pastRect, AR_U32 u32ImgW, AR_U32 u32ImgH);
AR_S32 IPC_PF_OSD_DrawGeomRect(VPSS_GRP VpssGrp, AR_U32 u32GeomNum, IPC_OSD_GEOM_S * pastGeom, AR_U32 u32ImgW, AR_U32 u32ImgH);
AR_S32 IPC_PF_SetStreamOsd(AR_U32 u32ChannelId,
		AR_U32 u32StreamId,
		AR_U32 u32ChnNameEnable,
        AR_CHAR * strChnName,
		AR_U32 u32DateTimeEnable,
		AR_U32 u32DateFormat,
		AR_U32 u32TimeFormat,
		AR_U32 u32DisplayWeek,
		AR_U32 u32RGB,
		AR_U32 u32FontSize);

AR_S32 IPC_PF_OSD_Suspend();
AR_S32 IPC_PF_OSD_Resume();

#ifdef __cplusplus
}
#endif
#endif
