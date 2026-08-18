/**
 * \file
 * \brief 描述gdc相关的通用数据结构
 */

#ifndef __AR_COMM_GDC_H__
#define __AR_COMM_GDC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hal_type.h"
#include "ar_common.h"
#include "hal_errno.h"
#include "ar_comm_video.h"

//#define MAKE_DWORD(high,low) (((low)&0x0000ffff)|((high)<<16))
//#define HIGH_WORD(x) (((x)&0xffff0000)>>16)
//#define LOW_WORD(x) ((x)&0x0000ffff)

/* failure caused by malloc buffer */
#define AR_ERR_GDC_NOBUF           AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define AR_ERR_GDC_BUF_EMPTY       AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
#define AR_ERR_GDC_NULL_PTR        AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_GDC_ILLEGAL_PARAM   AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_GDC_BUF_FULL        AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
#define AR_ERR_GDC_SYS_NOTREADY    AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define AR_ERR_GDC_NOT_SUPPORT     AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
#define AR_ERR_GDC_NOT_PERMITTED   AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define AR_ERR_GDC_BUSY            AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define AR_ERR_GDC_INVALID_CHNID   AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
#define AR_ERR_GDC_CHN_UNEXIST     AR_MPP_DEF_ERR(AR_ID_GDC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)




#define FISHEYE_MAX_REGION_NUM         4
#define FISHEYE_LMFCOEF_NUM          128
#define GDC_PMFCOEF_NUM            9


typedef AR_S32      GDC_HANDLE;

typedef struct arGDC_TASK_ATTR_S
{
    VIDEO_FRAME_INFO_S      stImgIn;             /* Input picture */
    VIDEO_FRAME_INFO_S      stImgOut;            /* Output picture */
    AR_U64                  au64privateData[4];  /* RW; Private data of task */
    AR_U64                  reserved;            /* RW; Debug information,state of current picture */
} GDC_TASK_ATTR_S;

/* Mount mode of device*/
typedef enum arFISHEYE_MOUNT_MODE_E
{
    FISHEYE_DESKTOP_MOUNT     = 0,        /* Desktop mount mode */
    FISHEYE_CEILING_MOUNT    = 1,        /* Ceiling mount mode */
    FISHEYE_WALL_MOUNT       = 2,        /* wall mount mode */

    FISHEYE_MOUNT_MODE_BUTT
} FISHEYE_MOUNT_MODE_E;

/* View mode of client*/
typedef enum arFISHEYE_VIEW_MODE_E
{
    FISHEYE_VIEW_360_PANORAMA   = 0,     /* 360 panorama mode of gdc correction */
    FISHEYE_VIEW_180_PANORAMA    = 1,    /* 180 panorama mode of gdc correction */
    FISHEYE_VIEW_NORMAL           = 2,     /* normal mode of gdc correction */
    FISHEYE_NO_TRANSFORMATION     = 3,     /* no gdc correction */

    FISHEYE_VIEW_MODE_BUTT
} FISHEYE_VIEW_MODE_E;

/*Fisheye region correction attribute */
typedef struct arFISHEYE_REGION_ATTR_S
{
    FISHEYE_VIEW_MODE_E     enViewMode;        /* RW; gdc view mode */
    AR_U32                     u32InRadius;    /* RW; inner radius of gdc correction region*/
    AR_U32                     u32OutRadius;   /* RW; out radius of gdc correction region*/
    AR_U32                     u32Pan;            /* RW; Range: [0, 360] */
    AR_U32                     u32Tilt;        /* RW; Range: [0, 360] */
    AR_U32                     u32HorZoom;        /* RW; Range: [1, 4095] */
    AR_U32                     u32VerZoom;        /* RW; Range: [1, 4095] */
    RECT_S                  stOutRect;         /* RW; out Imge rectangle attribute */
} FISHEYE_REGION_ATTR_S;

/*Fisheye all regions correction attribute */
typedef struct arFISHEYE_ATTR_S
{
    AR_BOOL                 bEnable;                                 /* RW; whether enable fisheye correction or not */
    AR_BOOL                 bLMF;                                    /* RW; whether gdc len's LMF coefficient is from user config or from default linear config */
    AR_BOOL                 bBgColor;                                /* RW; whether use background color or not */
    AR_U32                  u32BgColor;                              /* RW;  Range: [0,0xffffff]the background color RGB888*/

    AR_S32                  s32HorOffset;                             /* RW; Range: [-511, 511], the horizontal offset between image center and physical center of len*/
    AR_S32                  s32VerOffset;                             /* RW; Range: [-511, 511], the vertical offset between image center and physical center of len*/

    AR_U32                  u32TrapezoidCoef;                         /* RW; Range: [0, 32], strength coefficient of trapezoid correction */
    AR_S32                  s32FanStrength;                           /* RW; Range: [-760, 760], strength coefficient of fan correction */

    FISHEYE_MOUNT_MODE_E    enMountMode;                              /* RW; gdc mount mode */

    AR_U32                  u32RegionNum;                             /* RW; Range: [1, FISHEYE_MAX_REGION_NUM], gdc correction region number */
    FISHEYE_REGION_ATTR_S   astFishEyeRegionAttr[FISHEYE_MAX_REGION_NUM]; /* RW; attribution of gdc correction region */
} FISHEYE_ATTR_S;


/*Spread correction attribute */
typedef struct arSPREAD_ATTR_S
{
    AR_BOOL                 bEnable;            /* RW; whether enable spread or not */
    AR_U32                  u32SpreadCoef;      /* RW; Range: [0, 18],strength coefficient of spread correction,When spread on,ldc DistortionRatio range should be [0, 500] */
    SIZE_S                  stDestSize;         /* RW; dest size of spread*/
} SPREAD_ATTR_S;

/*Fisheye Job Config */
typedef struct arFISHEYE_JOB_CONFIG_S
{
    AR_U64                  u64LenMapPhyAddr;   /* LMF coefficient Physic Addr*/
} FISHEYE_JOB_CONFIG_S;

/*Fisheye Config */
typedef struct arFISHEYE_CONFIG_S
{
    AR_U16                  au16LMFCoef[FISHEYE_LMFCOEF_NUM];     /*RW;  LMF coefficient of gdc len */
} FISHEYE_CONFIG_S;

/*Gdc PMF Attr */
typedef struct arGDC_PMF_ATTR_S
{
    AR_S64                  as64PMFCoef[GDC_PMFCOEF_NUM];         /*W;  PMF coefficient of gdc */
} GDC_PMF_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_COMM_GDC_H__ */
