/**
 * \file
 * \brief 描述region相关的通用数据结构
 */

#ifndef __AR_COMM_REGION_H__
#define __AR_COMM_REGION_H__

#include "ar_common.h"
#include "ar_comm_video.h"
#include "hal_errno.h"
#include "ar_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/********************************Macro Definition********************************/
/** \addtogroup      MPI_REGION */
/** @{ */  /** <!-- [MPI_REGION] */
#define RGN_COLOR_LUT_NUM 2

#define RGN_MAX_BMP_UPDATE_NUM 16

#define RGN_BATCHHANDLE_MAX 24

/* PingPong buffer change when set attr, it needs to remap memory in mpi interface */
#define AR_NOTICE_RGN_BUFFER_CHANGE  AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_NOTICE, AR_SUCCESS)

/* invlalid device ID */
#define AR_ERR_RGN_INVALID_DEVID     AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define AR_ERR_RGN_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AR_ERR_RGN_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* channel exists */
#define AR_ERR_RGN_EXIST             AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/*UN exist*/
#define AR_ERR_RGN_UNEXIST           AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* using a NULL point */
#define AR_ERR_RGN_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AR_ERR_RGN_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AR_ERR_RGN_NOT_SUPPORT       AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define AR_ERR_RGN_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define AR_ERR_RGN_NOMEM             AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define AR_ERR_RGN_NOBUF             AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define AR_ERR_RGN_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define AR_ERR_RGN_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define AR_ERR_RGN_BADADDR           AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define AR_ERR_RGN_BUSY              AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define AR_ERR_RGN_NOTREADY          AR_MPP_DEF_ERR(AR_ID_RGN, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)

/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_REGION */
/** @{ */  /** <!-- [MPI_REGION] */

/** RGN句柄 */
typedef AR_U32 RGN_HANDLE;

/** RGN句柄组 */
typedef AR_U32 RGN_HANDLEGROUP;

/** RGN类型 */
typedef enum arRGN_TYPE_E
{
    OVERLAY_RGN = 0,     /* video overlay region */
    COVER_RGN,
    COVEREX_RGN,
    OVERLAYEX_RGN,
    MOSAIC_RGN,
    RGN_BUTT
} RGN_TYPE_E;

/** 反色类型 */
typedef enum arINVERT_COLOR_MODE_E
{
    LESSTHAN_LUM_THRESH = 0,   /* the lum of the video is less than the lum threshold which is set by u32LumThresh  */
    MORETHAN_LUM_THRESH,       /* the lum of the video is more than the lum threshold which is set by u32LumThresh  */
    INVERT_COLOR_BUTT
}INVERT_COLOR_MODE_E;

/** overlay qp信息 */
typedef struct arOVERLAY_QP_INFO_S
{
    AR_BOOL     bAbsQp;
    AR_S32         s32Qp;
    AR_BOOL        bQpDisable;
}OVERLAY_QP_INFO_S;

/** overlay 反色信息 */
typedef struct arOVERLAY_INVERT_COLOR_S
{
    SIZE_S stInvColArea;                //It must be multipe of 16 but not more than 64.
    AR_U32 u32LumThresh;                //The threshold to decide whether invert the OSD's color or not.
    INVERT_COLOR_MODE_E enChgMod;
    AR_BOOL bInvColEn;                  //The switch of inverting color.
}OVERLAY_INVERT_COLOR_S;

/** overlay venc 绑定类型 */
typedef enum arATTACH_DEST_E
{
    ATTACH_JPEG_MAIN =0,
    ATTACH_JPEG_MPF0,
    ATTACH_JPEG_MPF1,
    ATTACH_JPEG_BUTT
}ATTACH_DEST_E;

/** overlay属性 */
typedef struct arOVERLAY_ATTR_S
{
    /* bitmap pixel format,now only support ARGB1555 or ARGB4444 */
    PIXEL_FORMAT_E enPixelFmt;

    /* background color, pixel format depends on "enPixelFmt" */
    AR_U32 u32BgColor;

    /* region size,W:[2,RGN_OVERLAY_MAX_WIDTH],align:2,H:[2,RGN_OVERLAY_MAX_HEIGHT],align:2 */
    SIZE_S stSize;
    AR_U32 u32CanvasNum;
}OVERLAY_ATTR_S;

/** overlay绑定通道属性 */
typedef struct arOVERLAY_CHN_ATTR_S
{
    /* X:[0,OVERLAY_MAX_X_VENC],align:2,Y:[0,OVERLAY_MAX_Y_VENC],align:2 */
    POINT_S stPoint;

    /* background an foreground transparence when pixel format is ARGB1555
      * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
      * range:[0,128]
      */
    AR_U32 u32FgAlpha;

    /* background an foreground transparence when pixel format is ARGB1555
      * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
      * range:[0,128]
      */
    AR_U32 u32BgAlpha;

    AR_U32 u32Layer;   /* OVERLAY region layer range:[0,7]*/

    OVERLAY_QP_INFO_S stQpInfo;

    OVERLAY_INVERT_COLOR_S stInvertColor;

    ATTACH_DEST_E enAttachDest;

    AR_U16 u16ColorLUT[RGN_COLOR_LUT_NUM];
}OVERLAY_CHN_ATTR_S;

/** RGN形状 */
typedef enum arRGN_AREA_TYPE_E
{
    AREA_RECT = 0,
    AREA_QUAD_RANGLE,
    AREA_BUTT
} RGN_AREA_TYPE_E;

/** RGN坐标类型 */
typedef enum arRGN_COORDINATE_E
{
    RGN_ABS_COOR = 0,   /*Absolute coordinate*/
    RGN_RATIO_COOR      /*Ratio coordinate*/
}RGN_COORDINATE_E;

/** 四边形参数 */
typedef struct arRGN_QUADRANGLE_S
{
    AR_BOOL bSolid;            /* whether solid or dashed quadrangle */
    AR_U32 u32Thick;           /* Line Width of quadrangle, valid when dashed quadrangle */
    POINT_S stPoint[4];        /* points of quadrilateral */
} RGN_QUADRANGLE_S;

/** cover绑定通道属性 */
typedef struct arCOVER_CHN_ATTR_S
{
    RGN_AREA_TYPE_E     enCoverType;        /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;            /* config of rect */
        RGN_QUADRANGLE_S    stQuadRangle;      /* config of arbitary quadrilateral COVER */
    };
    AR_U32 u32Color;
    AR_U32 u32Layer;                           /* COVER region layer */
    RGN_COORDINATE_E enCoordinate;           /*ratio coordiante or abs coordinate*/
}COVER_CHN_ATTR_S;

/** cover_ex绑定通道属性 */
typedef struct arCOVEREX_CHN_ATTR_S
{
    RGN_AREA_TYPE_E     enCoverType;       /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        /* config of rect */
        RGN_QUADRANGLE_S    stQuadRangle;  /* config of arbitary quadrilateral COVER */
    };
    AR_U32 u32Color;
    AR_U32 u32Layer;   /* COVEREX region layer range */
}COVEREX_CHN_ATTR_S;

/** 马赛克块尺寸 */
typedef enum arMOSAIC_BLK_SIZE_E
{
    MOSAIC_BLK_SIZE_8 = 0,    /*block size 8*8 of MOSAIC*/
    MOSAIC_BLK_SIZE_16,       /*block size 16*16 of MOSAIC*/
    MOSAIC_BLK_SIZE_32,       /*block size 32*32 of MOSAIC*/
    MOSAIC_BLK_SIZE_64,       /*block size 64*64 of MOSAIC*/
    MOSAIC_BLK_SIZE_BUTT
}MOSAIC_BLK_SIZE_E;

/** 马赛克绑定通道属性 */
typedef struct arMOSAIC_CHN_ATTR_S
{
    RECT_S stRect;                 /*location of MOSAIC*/
    MOSAIC_BLK_SIZE_E enBlkSize;   /*block size of MOSAIC*/
    AR_U32 u32Layer;               /*MOSAIC region layer range:[0,3] */
}MOSAIC_CHN_ATTR_S;

/** overlay_ex属性 */
typedef struct arOVERLAYEX_COMM_ATTR_S
{
    PIXEL_FORMAT_E enPixelFmt;

    /* background color, pixel format depends on "enPixelFmt" */
    AR_U32 u32BgColor;

    /* region size,W:[2,RGN_OVERLAY_MAX_WIDTH],align:2,H:[2,RGN_OVERLAY_MAX_HEIGHT],align:2 */
    SIZE_S stSize;
    AR_U32 u32CanvasNum;
}OVERLAYEX_ATTR_S;

/** overlay_ex绑定通道属性 */
typedef struct arOVERLAYEX_CHN_ATTR_S
{
    /* X:[0,RGN_OVERLAY_MAX_X],align:2,Y:[0,RGN_OVERLAY_MAX_Y],align:2 */
    POINT_S stPoint;

    /* background an foreground transparence when pixel format is ARGB1555
      * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
      * range:[0,255]
      */
    AR_U32 u32FgAlpha;

    /* background an foreground transparence when pixel format is ARGB1555
      * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
      * range:[0,255]
      */
    AR_U32 u32BgAlpha;

    AR_U32 u32Layer;   /* OVERLAYEX region layer range:[0,15]*/
}OVERLAYEX_CHN_ATTR_S;

/** RGN属性 */
typedef union arRGN_ATTR_U
{
    OVERLAY_ATTR_S      stOverlay;      /* attribute of overlay region */
    OVERLAYEX_ATTR_S    stOverlayEx;    /* attribute of overlayex region */
} RGN_ATTR_U;

/** RGN绑定通道属性 */
typedef union arRGN_CHN_ATTR_U
{
    OVERLAY_CHN_ATTR_S      stOverlayChn;      /* attribute of overlay region */
    COVER_CHN_ATTR_S        stCoverChn;        /* attribute of cover region */
    COVEREX_CHN_ATTR_S      stCoverExChn;      /* attribute of coverex region */
    OVERLAYEX_CHN_ATTR_S    stOverlayExChn;    /* attribute of overlayex region */
    MOSAIC_CHN_ATTR_S       stMosaicChn;       /* attribute of mosic region */
} RGN_CHN_ATTR_U;

/** RGN属性 */
typedef struct arRGN_ATTR_S
{
    RGN_TYPE_E enType;  /* region type */
    RGN_ATTR_U unAttr;  /* region attribute */
} RGN_ATTR_S;

/** 通道属性 */
typedef struct arRGN_CHN_ATTR_S
{
    AR_BOOL           bShow;
    RGN_TYPE_E        enType;     /* region type */
    RGN_CHN_ATTR_U    unChnAttr;  /* region attribute */
} RGN_CHN_ATTR_S;

/** 通道参数 */
typedef struct arRGN_CHN_PARA_S
{
    AR_BOOL           bCopyBufEnable;
} RGN_CHN_PARA_S;


/** 输出信息，暂MPI其他模块内部使用 */
typedef struct arRGN_DRAW_INFO_OUT_S
{
    AR_U32             QPNum;
    OVERLAY_QP_INFO_S *QPInfo;
}RGN_DRAW_INFO_OUT_S;

/** BMP更新信息 */
typedef struct arRGN_BMP_UPDATE_S
{
    POINT_S             stPoint;
    BITMAP_S            stBmp;
    AR_U32              u32Stride;
} RGN_BMP_UPDATE_S;

/** 批量BMP更新信息 */
typedef struct arRGN_BMP_UPDATE_CFG_S
{
    AR_U32              u32BmpCnt;
    RGN_BMP_UPDATE_S    astBmpUpdate[RGN_MAX_BMP_UPDATE_NUM];
} RGN_BMP_UPDATE_CFG_S;

/** 画布信息 */
typedef struct arRGN_CANVAS_INFO_S
{
    AR_U64         u64PhyAddr;
    AR_U64         u64VirtAddr;
    SIZE_S         stSize;
    AR_U32         u32Stride;
    PIXEL_FORMAT_E enPixelFmt;
} RGN_CANVAS_INFO_S;

/** RGN画图输入参数，暂MPI其他模块内部使用 */
typedef struct arRGN_DRAW_PARA_IN_S
{
    VIDEO_FRAME_INFO_S *pstFrameInfoIn;
    AR_U64              u64RgnMask;
}RGN_DRAW_PARA_IN_S;

/** RGN画图输出参数，暂MPI其他模块内部使用 */
typedef struct arRGN_DRAW_PARA_OUT_S
{
    VIDEO_FRAME_INFO_S    stFrameInfoOut;
    AR_BOOL               IsCopy;
    RGN_DRAW_INFO_OUT_S  *pstInfo;
}RGN_DRAW_PARA_OUT_S;

/** RGN画图注册钩子，暂MPI其他模块内部使用 */
typedef AR_S32 (*AR_MPI_FUNC_RGN_DRAW_CB)(const MPP_CHN_S     *pstChn,
                                                  RGN_DRAW_PARA_IN_S  *pstParaIn,
                                                  RGN_DRAW_PARA_OUT_S *pstParaOut);

/** RGN画图注销钩子，暂MPI其他模块内部使用 */
typedef AR_S32 (*AR_MPI_FUNC_RGN_RELEASE_FRAME_CB)(VIDEO_FRAME_INFO_S *pstFrameInfo);

/** RGN画图注销结构，暂MPI其他模块内部使用 */
typedef struct {
    MOD_ID_E enModId;
    AR_S32 (*attach_call_back)(AR_MPI_FUNC_RGN_DRAW_CB draw_cb, AR_MPI_FUNC_RGN_RELEASE_FRAME_CB release_cb);
    AR_S32 (*detach_call_back)(AR_VOID);
} RGN_REGISTER_CB_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AR_COMM_REGION_H__ */



