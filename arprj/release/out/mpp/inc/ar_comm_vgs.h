#ifndef __AR_COMM_VGS_H__
#define __AR_COMM_VGS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ar_common.h"
#include "ar_comm_video.h"
#include "hal_errno.h"
#include "ar_defines.h"

/********************************Macro Definition********************************/
/** \addtogroup      MPI_VGS */
/** @{ */  /** <!-- [MPI_VGS] */
#define AR_ERR_VGS_NOBUF           AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define AR_ERR_VGS_BUF_EMPTY       AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
#define AR_ERR_VGS_NULL_PTR        AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_VGS_ILLEGAL_PARAM   AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_VGS_BUF_FULL        AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
#define AR_ERR_VGS_SYS_NOTREADY    AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define AR_ERR_VGS_NOT_SUPPORT     AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
#define AR_ERR_VGS_NOT_PERMITTED   AR_MPP_DEF_ERR(AR_ID_VGS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)

#define VGS_PRIVATE_DATA_LEN 6
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_VGS */
/** @{ */  /** <!-- [MPI_VGS] */

/** VGS句柄 */
typedef AR_S32 VGS_HANDLE;

/** 反色模式 */
typedef enum arVGS_COLOR_REVERT_MODE_E
{
    VGS_COLOR_REVERT_NONE = 0,          /**<           不反色      */
    VGS_COLOR_REVERT_RGB,               /**<@note      按RGB反色，暂不支持 */
    VGS_COLOR_REVERT_ALPHA,             /**<           按ALPHA反色 */
    VGS_COLOR_REVERT_BOTH,              /**<@note      同时反色，暂不支持 */
    VGS_COLOR_REVERT_BUTT
} VGS_COLOR_REVERT_MODE_E;

/** 反色参数 */
typedef struct arVGS_OSD_REVERT_S
{
    RECT_S                       stSrcRect;             /**<@note      指定区域反转，暂不支持。目前仅支持整图反色 */
    VGS_COLOR_REVERT_MODE_E      enColorRevertMode;     /**<@note      仅支持部分反色模式 */
} VGS_OSD_REVERT_S;

/** task参数 */
typedef struct arVGS_TASK_ATTR_S
{
    VIDEO_FRAME_INFO_S       stImgIn;                   /**< 源图像      */
    VIDEO_FRAME_INFO_S       stImgOut;                  /**< 目的图像 */
    AR_U64                   au64PrivateData[4];        /**< 私有数据 */
    AR_U32                   reserved;                  /**< 保留数据 */
} VGS_TASK_ATTR_S;

/** 画线参数 */
typedef struct arVGS_DRAW_LINE_S
{
    POINT_S                     stStartPoint;           /**< 起始坐标 */
    POINT_S                     stEndPoint;             /**< 终点坐标 */
    AR_U32                      u32Thick;               /**< 线宽       */
    AR_U32                      u32Color;               /**< 线颜色， ARGB8888 */
} VGS_DRAW_LINE_S;

/** cover类型 */
typedef enum arVGS_COVER_TYPE_E
{
    COVER_RECT = 0,             /**< 矩形cover */
    COVER_QUAD_RANGLE,          /**< 四边行cover */
    COVER_BUTT
} VGS_COVER_TYPE_E;

/** 四边形cover参数 */
typedef struct arVGS_QUADRANGLE_COVER_S
{
    AR_BOOL              bSolid;            /**< 是否实心 */
    AR_U32               u32Thick;          /**< 线宽，仅bSolid为假时有效 */
    POINT_S              stPoint[4];        /**< 四边形坐标 */
} VGS_QUADRANGLE_COVER_S;

/** cover参数 */
typedef struct arVGS_ADD_COVER_S
{
    VGS_COVER_TYPE_E                    enCoverType;    /**< Cover类型 */
    union
    {
        RECT_S                          stDstRect;      /**< 矩形参数 */
        VGS_QUADRANGLE_COVER_S          stQuadRangle;   /**< 四边形参数 */
    };

    AR_U32                              u32Color;       /**< 颜色， ARGB8888 */
} VGS_ADD_COVER_S;

/** osd参数 */
typedef struct arVGS_ADD_OSD_S
{
    RECT_S                    stRect;               /**< Osd区域大小 */
    AR_U32                    u32BgColor;           /**< 背景颜色      @note暂不支持*/
    PIXEL_FORMAT_E            enPixelFmt;           /**< 像素格式 */
    AR_U64                    u64PhyAddr;           /**< 物理地址 */
    AR_U32                    u32Stride;            /**< OSD行跨度 */
    AR_U32                    u32BgAlpha;           /**< 后景色 @note     后景仅PIXEL_FORMAT_GRAY1&4&8时生效   */
    AR_U32                    u32FgAlpha;           /**< 前景色 @note     前景仅PIXEL_FORMAT_GRAY1&4&8时生效   */
    AR_BOOL                   bOsdRevert;           /**< 是否需要反色 */
    VGS_OSD_REVERT_S          stOsdRevert;          /**< 反色参数 */
    AR_U16                    u16ColorLUT[2];
    AR_U32                    u32CloseAlpha;        /**< 关闭alpha混合 */
} VGS_ADD_OSD_S;

typedef struct arVGS_ADD_ROTATION_S
{
    ROTATION_E                enAngle;             /**< 旋转角度     */
    AR_U32                    u32CloseAlpha;        /**< 关闭alpha混合 */
} VGS_ADD_ROTATION_S;


/** 缩放参数 */
typedef enum arVGS_SCLCOEF_MODE_E
{
    VGS_SCLCOEF_NORMAL   = 0,    /**<@note     暂不支持*/
    VGS_SCLCOEF_TAP2     = 1,    /**<@note     暂不支持   */
    VGS_SCLCOEF_TAP4     = 2,    /**<@note     暂不支持   */
    VGS_SCLCOEF_TAP6     = 3,    /**<@note     暂不支持   */
    VGS_SCLCOEF_TAP8     = 4,    /**<@note     暂不支持   */
    VGS_SCLCOEF_BUTT
} VGS_SCLCOEF_MODE_E;

/** @} */  /** <!-- ==== Structure Definition End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_COMM_VGS_H__ */

