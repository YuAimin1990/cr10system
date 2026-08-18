#ifndef __CFG_VPSS_H__
#define __CFG_VPSS_H__

#include "cfg_common.h"

#define IPC_MAX_VPSS_GRP_NEEDED  2

typedef enum
{
    VPSS_ROTATE_0,
    VPSS_ROTATE_90,
    VPSS_ROTATE_180,
    VPSS_ROTATE_270
} VPSS_ROTATE_ANGLE_E;

typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32W;
    AR_U32 u32H;
} VPSS_RECT_S;

typedef struct
{
    AR_BOOL bEnable;
    VPSS_ROTATE_ANGLE_E enAngle;
} VPSS_ROTATE_S;

typedef struct
{
    AR_U32 u32Num;
    VPSS_RECT_S stRect[4];
} VPSS_MOSAIC_S;

typedef struct
{
    AR_U32 u32Num;
    VPSS_RECT_S stRect[4];
    AR_U32 u32ARGB8888[4];
} VPSS_COVER_S;

typedef struct
{
    AR_BOOL bEnable;
    VPSS_RECT_S stRect;
} VPSS_CROP_S;

typedef struct
{
    AR_FLOAT f32K[9];
    AR_FLOAT f32K0;
    AR_FLOAT f32K1;
    AR_FLOAT f32K2;
} VPSS_LDC_COEFF_S;

typedef struct
{
    AR_BOOL bEnable;
    AR_U32 u32Width;
    AR_U32 u32Height;
    VPSS_CROP_S stCrop;
    AR_BOOL bFlip;
    AR_BOOL bMirror;
    //VPSS_SCALE_S stScale;
    AR_U32 u32YUV2RGBEnable; //only support yuv420p to rgb planar
} VPSS_CHN_CFG_S;

typedef struct
{
    AR_BOOL bGroup;
    AR_BOOL bEis;
    AR_BOOL bLdc;
    VPSS_LDC_COEFF_S stLdcCoeff;
    VPSS_ROTATE_S stRotate;
    AR_BOOL bFlip;
    AR_BOOL bMirror;
    VPSS_MOSAIC_S stMosaic;
    VPSS_COVER_S stCover;
    VPSS_CHN_CFG_S stChnCfg[IPC_MAX_STREAM_PER_PIPE]; //Last two channels don't have mosaic and cover.
} VPSS_GRP_CFG_S;

typedef struct
{
    VPSS_GRP_CFG_S stGrpCfg[IPC_MAX_VPSS_GRP_NEEDED];
} IPC_CFG_VPSS_S;

typedef struct
{
    AR_U32 u32Width;
    AR_U32 u32Height;
} IPC_CFG_VPSS_MAX_S;

typedef struct
{
    IPC_CFG_VPSS_MAX_S stVpssMaxResource[IPC_MAX_VPSS_GRP_NEEDED][IPC_MAX_STREAM_PER_PIPE];
} IPC_VPSS_MAX_RESOURCE_S;


AR_S32 IPC_CFG_VPSS_Save();
AR_S32 IPC_CFG_VPSS_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_VPSS_LoadDefault(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_VPSS_LoadMaxResource();

void IPC_CFG_VPSS_Print();
IPC_CFG_VPSS_S * IPC_CFG_VPSS_GetParam();
IPC_VPSS_MAX_RESOURCE_S * IPC_CFG_VPSS_GetMaxResource();

#define VPSS_CFG_FILE "cfg_vpss.json"

#endif
