/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VO
 *  @{
 */


/**
 * @file hal_vo.h
 * @brief 定义视频输出设备用户接口
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/04/27
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef _AR_HAL_VO_API_H_
#define _AR_HAL_VO_API_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


//=============================================================================

// Include files

//=============================================================================
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "hal_type.h"
#include "hal_errno.h"

//=============================================================================

// Macro definition

//=============================================================================

#define AR_HAL_MOD_VO          HAL_TAG_ID(AR_SYS_ID_VO)
#define AR_HAL_VO_SUCCESS      0

typedef enum HAL_VO_ERRNO
{
    AR_ERR_VO_HAL_MODE                = 0x40,
    AR_ERR_VO_NO_MEM,
    AR_ERR_VO_CLOSE_FD,
    AR_ERR_VO_PARA,
    AR_ERR_VO_NOT_SUPPORT,
    AR_ERR_VO_FORMAT_NOT_SUPPORT,
    AR_ERR_VO_TIMING_NOT_SUPPORT,
    AR_ERR_VO_INTF_NOT_SUPPORT,
    AR_ERR_VO_CSC_NOT_SUPPORT,
    AR_ERR_VO_INTF_CONFLICT,
    AR_ERR_VO_SUB_HAS_ENABLED,
    AR_ERR_VO_SUB_NOT_ENABLE,
    AR_ERR_VO_SUB_CLEAR,

    AR_ERR_VO_DEV_OPEN                = 0x100,
    AR_ERR_VO_DEV_CLOSE,
    AR_ERR_VO_DEV_FD,
    AR_ERR_VO_DEV_ID,
    AR_ERR_VO_DEV_TIMING_TEMPLATE,
    AR_ERR_VO_DEV_CURSOR_HAS_ENABLED,
    AR_ERR_VO_DEV_CURSOR_NOT_ENABLE,

    AR_ERR_VO_DEV_NOT_CONFIG,
    AR_ERR_VO_DEV_NOT_ENABLE,
    AR_ERR_VO_DEV_HAS_ENABLED,
    AR_ERR_VO_DEV_HAS_BINDED,
    AR_ERR_VO_DEV_NOT_BINDED,
    AR_ERR_VO_DEV_HDMI_CREATE_THREAD,
    AR_ERR_VO_DEV_HDMI_DESTORY_THREAD,
    AR_ERR_VO_DEV_NOT_SUSPEND,
    AR_ERR_VO_DEV_HAS_SUSPENDED,

    AR_ERR_VO_LAYER_ID                = 0x200,
    AR_ERR_VO_LAYER_OPEN,
    AR_ERR_VO_LAYER_CLOSE,
    AR_ERR_VO_LAYER_NOT_ENABLE,
    AR_ERR_VO_LAYER_HAS_ENABLED,
    AR_ERR_VO_LAYER_POSITION,
    AR_ERR_VO_LAYER_GE2D_OPR,
    AR_ERR_VO_LAYER_TIMEOUT,

    AR_ERR_VO_CHN_HAS_ENABLED         = 0x300,
    AR_ERR_VO_CHN_NOT_ENABLE,
    AR_ERR_VO_CHN_OPEN,
    AR_ERR_VO_CHN_NOT_CONFIG,
    AR_ERR_VO_CHN_NOT_ALLOC,
    AR_ERR_VO_CHN_CREATE,
    AR_ERR_VO_CHN_DELETE,
    AR_ERR_VO_CHN_GET,
    AR_ERR_VO_CHN_ID,
    AR_ERR_VO_CHN_SET_ATTR,
    AR_ERR_VO_CHN_ENQUEUE,
    AR_ERR_VO_CHN_DEQUEUE,
    AR_ERR_VO_CHN_QUEUE_NOT_EMPTY,
    AR_ERR_VO_CHN_QUEUE_EMPTY,
    AR_ERR_VO_CHN_TIMEOUT,
    AR_ERR_VO_CHN_MEM_ADD_REF,
    AR_ERR_VO_CHN_MEM_SUB_REF,
    AR_ERR_VO_CHN_EXSIT,
    AR_ERR_VO_CHN_THREAD_CREATE,
    AR_ERR_VO_CHN_CLEAR_BUF,
    AR_ERR_VO_CHN_PAUSE,
    AR_ERR_VO_CHN_IN_SUSPEND,

    AR_ERR_VO_FUSION_GET,

    AR_ERR_VO_CCD_INVALID_PAT,
    AR_ERR_VO_CCD_INVALID_POS,

    AR_ERR_VO_WAIT_TIMEOUT,

    AR_ERR_VO_DSI_CMD_SIZE         = 0x400,


    AR_ERR_VO_MAX
}ENUM_HAL_VO_ERRNO;

#define  HAL_ERR_VO_HAL_MODE                     AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_HAL_MODE)
#define  HAL_ERR_VO_NO_MEM                       AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_NO_MEM)
#define  HAL_ERR_VO_CLOSE_FD                     AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CLOSE_FD)
#define  HAL_ERR_VO_PARA                         AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_PARA)
#define  HAL_ERR_VO_NOT_SUPPORT                  AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_NOT_SUPPORT)
#define  HAL_ERR_VO_FORMAT_NOT_SUPPORT           AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_FORMAT_NOT_SUPPORT)
#define  HAL_ERR_VO_TIMING_NOT_SUPPORT           AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_TIMING_NOT_SUPPORT)
#define  HAL_ERR_VO_INTF_NOT_SUPPORT             AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_INTF_NOT_SUPPORT)
#define  HAL_ERR_VO_CSC_NOT_SUPPORT              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CSC_NOT_SUPPORT)
#define  HAL_ERR_VO_INTF_CONFLICT                AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_INTF_CONFLICT)
#define  HAL_ERR_VO_SUB_HAS_ENABLED              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_SUB_HAS_ENABLED)
#define  HAL_ERR_VO_SUB_NOT_ENABLE               AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_SUB_NOT_ENABLE)
#define  HAL_ERR_VO_SUB_CLEAR                    AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_SUB_CLEAR)

#define  HAL_ERR_VO_DEV_OPEN                     AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_OPEN)
#define  HAL_ERR_VO_DEV_CLOSE                    AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_CLOSE)
#define  HAL_ERR_VO_DEV_FD                       AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_FD)
#define  HAL_ERR_VO_DEV_ID                       AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_ID)
#define  HAL_ERR_VO_DEV_CURSOR_HAS_ENABLED       AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_CURSOR_HAS_ENABLED)
#define  HAL_ERR_VO_DEV_CURSOR_NOT_ENABLE        AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_CURSOR_NOT_ENABLE)
#define  HAL_ERR_VO_DEV_TIMING_TEMPLATE          AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_TIMING_TEMPLATE)
#define  HAL_ERR_VO_DEV_NOT_ENABLE               AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_NOT_ENABLE)
#define  HAL_ERR_VO_DEV_HAS_ENABLED              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_HAS_ENABLED)
#define  HAL_ERR_VO_DEV_HDMI_CREATE_THREAD       AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_HDMI_CREATE_THREAD)
#define  HAL_ERR_VO_DEV_HDMI_DESTORY_THREAD      AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_HDMI_DESTORY_THREAD)
#define  HAL_ERR_VO_DEV_NOT_SUSPEND              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_NOT_SUSPEND)
#define  HAL_ERR_VO_DEV_HAS_SUSPENDED            AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DEV_HAS_SUSPENDED)

#define  HAL_ERR_VO_LAYER_ID                     AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_ID)
#define  HAL_ERR_VO_LAYER_OPEN                   AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_OPEN)
#define  HAL_ERR_VO_LAYER_CLOSE                  AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_CLOSE)
#define  HAL_ERR_VO_LAYER_NOT_ENABLE             AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_NOT_ENABLE)
#define  HAL_ERR_VO_LAYER_HAS_ENABLED            AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_HAS_ENABLED)
#define  HAL_ERR_VO_LAYER_POSITION               AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_POSITION)
#define  HAL_ERR_VO_LAYER_GE2D_OPR               AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_GE2D_OPR)
#define  HAL_ERR_VO_LAYER_TIMEOUT                AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_LAYER_TIMEOUT)


#define  HAL_ERR_VO_CHN_CREATE                   AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_CREATE)
#define  HAL_ERR_VO_CHN_DELETE                   AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_DELETE)
#define  HAL_ERR_VO_CHN_ID                       AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_ID)
#define  HAL_ERR_VO_CHN_NOT_ENABLE               AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_NOT_ENABLE)
#define  HAL_ERR_VO_CHN_HAS_ENABLED              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_HAS_ENABLED)
#define  HAL_ERR_VO_CHN_OPEN                     AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_OPEN)
#define  HAL_ERR_VO_CHN_GET                      AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_GET)
#define  HAL_ERR_VO_CHN_SET_ATTR                 AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_SET_ATTR)
#define  HAL_ERR_VO_CHN_ENQUEUE                  AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_ENQUEUE)
#define  HAL_ERR_VO_CHN_DEQUEUE                  AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_DEQUEUE)
#define  HAL_ERR_VO_CHN_TIMEOUT                  AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_TIMEOUT)
#define  HAL_ERR_VO_CHN_QUEUE_NOT_EMPTY          AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_QUEUE_NOT_EMPTY)
#define  HAL_ERR_VO_CHN_QUEUE_EMPTY              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_QUEUE_EMPTY)
#define  HAL_ERR_VO_CHN_MEM_ADD_REF              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_MEM_ADD_REF)
#define  HAL_ERR_VO_CHN_MEM_SUB_REF              AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_MEM_SUB_REF)
#define  HAL_ERR_VO_CHN_EXSIT                    AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_EXSIT)
#define  HAL_ERR_VO_CHN_THREAD_CREATE            AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_THREAD_CREATE)
#define  HAL_ERR_VO_CHN_CLEAR_BUF                AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_CLEAR_BUF)
#define  HAL_ERR_VO_CHN_PAUSE                    AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_PAUSE)
#define  HAL_ERR_VO_CHN_IN_SUSPEND               AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_CHN_IN_SUSPEND)

#define  HAL_ERR_VO_FUSION_GET                   AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_FUSION_GET)

#define  HAL_ERR_VO_DSI_CMD_SIZE                 AR_HAL_DEF_ERR(AR_SYS_ID_VO, HAL_ERR_LEVEL_ERROR, AR_ERR_VO_DSI_CMD_SIZE)


/**
* @note  最大通道数
*/
#define  MAX_VO_CHN_NUM                       25

/**
* @note  鼠标文件名最大长度
*/
#define  MAX_VO_CURSOR_FILE_NAME              64

/**
* @note  最大面板数
*/
#define  MAX_VO_PANNEL_NUM                    3

/**
* @note  dsi最大lane数量
*/
#define   MAX_LANE_NUM           4

#define   MAX_FIFO_DEPTH         8


//=============================================================================

// Data type definition

//=============================================================================

/**
* @note  视频层id
*/
typedef enum
{
    AR_HAL_VO_LAYER_ID_VIDEO_0,
    AR_HAL_VO_LAYER_ID_OVERLAY_0,
    AR_HAL_VO_LAYER_ID_OVERLAY_1,
    AR_HAL_VO_LAYER_ID_MAX,
}ENUM_AR_HAL_VO_LAYER_ID;

/**
* @note  视频输出设备id，目前仅支持AR_HAL_VO_DEV_ID_HD0
*/
typedef enum
{
    AR_HAL_VO_DEV_ID_HD0,
    AR_HAL_VO_DEV_ID_MAX,
}ENUM_AR_HAL_VO_DEV_ID;

/**
* @note   vo原生视频输出接口.AR93xx支持AR_HAL_VO_DEV_INTF_DVP和AR_HAL_VO_DEV_INTF_MIPI
*/
typedef enum
{
    AR_HAL_VO_DEV_INTF_NONE           = 0x00,            // 无
    AR_HAL_VO_DEV_INTF_DVP            = 0x01,            // DVP接口
    AR_HAL_VO_DEV_INTF_DP             = 0x02,            // DP接口
    AR_HAL_VO_DEV_INTF_MIPI           = 0x04,            // MIPI-DSI接口
}ENUM_AR_HAL_VO_DEV_INTF;

/**
* @note   DVP外接接口芯片拓展接口或外接显示驱动芯片驱动屏幕.
*/
typedef enum
{
    /* DVP外接接口类型 */
    AR_HAL_VO_DEV_SUB_INTF_NONE               = 0,
    AR_HAL_VO_DEV_SUB_INTF_VGA                = 0x01,
    AR_HAL_VO_DEV_SUB_INTF_HDMI               = 0x02,
    AR_HAL_VO_DEV_SUB_INTF_BT656              = 0x04,
    AR_HAL_VO_DEV_SUB_INTF_BT1120             = 0x08,
    AR_HAL_VO_DEV_SUB_INTF_LCD_16BIT          = 0x10,
    AR_HAL_VO_DEV_SUB_INTF_LCD_24BIT          = 0x20,
    AR_HAL_VO_DEV_SUB_INTF_MAX,
}ENUM_AR_HAL_VO_DEV_SUB_INTF;

/**
* @note   接口时序典型模板.
*/
typedef enum
{
    AR_HAL_VO_OUTPUT_PAL = 0,               /* PAL */
    AR_HAL_VO_OUTPUT_NTSC,                  /* NTSC */

    AR_HAL_VO_OUTPUT_1080P24,               /* 1920 x 1080 at 24 Hz. */
    AR_HAL_VO_OUTPUT_1080P25,               /* 1920 x 1080 at 25 Hz. */
    AR_HAL_VO_OUTPUT_1080P30,               /* 1920 x 1080 at 30 Hz. */
    AR_HAL_VO_OUTPUT_1080P50,               /* 1920 x 1080 at 50 Hz. */
    AR_HAL_VO_OUTPUT_1080P60,               /* 1920 x 1080 at 60 Hz. */
    AR_HAL_VO_OUTPUT_1080I48,               /* 1920 x 1080 at 48 Hz. */
    AR_HAL_VO_OUTPUT_1080I50,               /* 1920 x 1080 at 50 Hz. */
    AR_HAL_VO_OUTPUT_1080I60,               /* 1920 x 1080 at 60 Hz. */

    AR_HAL_VO_OUTPUT_720P25,                /* 1280 x  720 at 25 Hz. */
    AR_HAL_VO_OUTPUT_720P30,                /* 1280 x  720 at 30 Hz. */
    AR_HAL_VO_OUTPUT_720P50,                /* 1280 x  720 at 50 Hz. */
    AR_HAL_VO_OUTPUT_720P60,                /* 1280 x  720 at 60 Hz. */

    AR_HAL_VO_OUTPUT_576P50,                /* 720  x  576 at 50 Hz. */
    AR_HAL_VO_OUTPUT_480P60,                /* 720  x  480 at 60 Hz. */

    AR_HAL_VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz */
    AR_HAL_VO_OUTPUT_1024x600_60,           /* 1024 x 600 at 60 Hz */
    AR_HAL_VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz*/
    AR_HAL_VO_OUTPUT_1280x768_60,           /* 1280*768@60Hz VGA@60Hz*/
    AR_HAL_VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz*/
    AR_HAL_VO_OUTPUT_3840x2160_60,          /* 3840x2160_60 */
    AR_HAL_VO_OUTPUT_800x480_60,            /* 800 x 480 at 60 Hz. */
    AR_HAL_VO_OUTPUT_USER                   /* 自定义时序. */
}ENUM_AR_HAL_VO_DEV_TIMING_TEMPLATE;

/**
* @note   视频层使用的像素格式.
*/
typedef enum
{
    AR_HAL_VO_FMT_ARGB1555,         // argb1555
    AR_HAL_VO_FMT_RGB565,           // rgb565
    AR_HAL_VO_FMT_ARGB8888,         // argb8888
    AR_HAL_VO_FMT_YV12              // yv12
}ENUM_AR_HAL_VO_FMT;

/**
* @note   视频层支持的色域范围.
*/
typedef enum
{
    AR_HAL_VO_CSC_BT709_LIMIT,      // bt.709 limited
    AR_HAL_VO_CSC_BT709_FULL,       // bt.709 full
    AR_HAL_VO_CSC_BT601_LIMIT,      // bt.601 limited
    AR_HAL_VO_CSC_BT601_FULL,       // bt.601 full
    AR_HAL_VO_CSC_DEFAULT           // default
}ENUM_AR_HAL_VO_CSC;

typedef enum
{
    AR_HAL_VO_CHN_STATE_DISABLE         =  0,
    AR_HAL_VO_CHN_STATE_PLAYING,
    AR_HAL_VO_CHN_STATE_PAUSE,
    AR_HAL_VO_CHN_STATE_STEP,
}ENUM_AR_HAL_VO_CHN_STATE;

/**
* @note   显示固定内置图像.
*/
typedef enum
{
    AR_HAL_VO_IMAGE_NULL  ,           // 不显示固定图像
    AR_HAL_VO_IMAGE_UBOOT_LOGO,       // 显示uboot启动图像，若无uboot启动图像，则显示start_logo.yuv文件
    AR_HAL_VO_IMAGE_START_LOGO,       // 显示start_logo.yuv图像，若文件不存在，则显示背景色
    AR_HAL_VO_IMAGE_BACKGROUND        // 显示background.yuv图像，若文件不存在，则显示背景色
}ENUM_AR_HAL_VO_IMAGE;

typedef enum {
    AR_HAL_ROT_NONE,                  // 原图
    AR_HAL_ROT_FLIP_X,                // 上下翻转
    AR_HAL_ROT_FLIP_Y,                // 左右翻转
    AR_HAL_ROT_MAX
}ENUM_AR_HAL_VO_ROTATION;

typedef enum {
    AR_HAL_DPI_DEFAULT,
    AR_HAL_DPI_16_BITS,
    AR_HAL_DPI_24_BITS,
    AR_HAL_DPI_30_BITS,
    AR_HAL_DPI_MAX
}ENUM_AR_HAL_VO_DPI_FORMAT;

typedef enum {
    AR_HAL_DPI_COLOR_MODE_DEFAULT,
    AR_HAL_DPI_COLOR_MODE_YUV,
    AR_HAL_DPI_COLOR_MODE_RGB565,
    AR_HAL_DPI_COLOR_MODE_RGB888,
    AR_HAL_DPI_COLOR_MODE_MAX,
}ENUM_AR_HAL_VO_DPI_COLOR_MODE;

typedef enum
{
   AR_HAL_SYNC_MODE_EXTERNAL,
   AR_HAL_SYNC_MODE_INTERNAL,
}ENUM_AR_HAL_VO_SYNC_MODE;

typedef enum
{
    AR_HAL_PIN_MODE_BT1120_DEFAULT         =  0,
    AR_HAL_PIN_MODE_BT1120_CBY_CRY         =  1,
    AR_HAL_PIN_MODE_BT1120_YCB_YCR         =  2,
    AR_HAL_PIN_MODE_BT1120_CRY_CBY         =  3,
    AR_HAL_PIN_MODE_BT1120_YCR_YCB         =  4,
}ENUM_AR_HAL_PIN_MODE_BT1120;

typedef enum
{
    AR_HAL_PIN_MODE_BT656_DEFAULT          =  0,
    AR_HAL_PIN_MODE_BT656_YCR_YCB_H        =  1,
    AR_HAL_PIN_MODE_BT656_YCB_YCR_H        =  2,
    AR_HAL_PIN_MODE_BT656_CRY_CBY_H        =  3,
    AR_HAL_PIN_MODE_BT656_CBY_CRY_H        =  4,

    AR_HAL_PIN_MODE_BT656_YCB_YCR_L        =  5,
    AR_HAL_PIN_MODE_BT656_YCR_YCB_L        =  6,
    AR_HAL_PIN_MODE_BT656_CRY_CBY_L        =  7,
    AR_HAL_PIN_MODE_BT656_CBY_CRY_L        =  8,
}ENUM_AR_HAL_PIN_MODE_BT656;

typedef enum
{
    AR_HAL_PIN_MODE_RGB888_DEFAULT         =  0,
    AR_HAL_PIN_MODE_RGB888_GRB             =  1,
    AR_HAL_PIN_MODE_RGB888_RGB             =  2,
    AR_HAL_PIN_MODE_RGB888_BRG             =  3,
    AR_HAL_PIN_MODE_RGB888_RBG             =  4,
    AR_HAL_PIN_MODE_RGB888_BGR             =  5,
    AR_HAL_PIN_MODE_RGB888_GBR             =  6
}ENUM_AR_HAL_PIN_MODE_RGB888;

typedef enum
{
    AR_HAL_PIN_MODE_RGB565_DEFAULT         =  0,
    AR_HAL_PIN_MODE_RGB565_GRB             =  1,
    AR_HAL_PIN_MODE_RGB565_BGR             =  2
}ENUM_AR_HAL_PIN_MODE_RGB565;

typedef enum
{
    AR_HAL_VO_HDMI_MODE_DEFAULT         =  0,
    AR_HAL_VO_HDMI_MODE_USER,
}ENUM_AR_HAL_VO_HDMI_MODE;

typedef union
{
    ENUM_AR_HAL_PIN_MODE_BT656   bt656;
    ENUM_AR_HAL_PIN_MODE_BT1120  bt1120;
    ENUM_AR_HAL_PIN_MODE_RGB888  rgb888;
    ENUM_AR_HAL_PIN_MODE_RGB565  rgb565;
}ENUM_AR_HAL_PIN_MODE;


/**
* @note   矩阵区域描述结构
*/
typedef struct
{
    AR_S32  x;         // 起始位置x坐标
    AR_S32  y;         // 起始位置y坐标
    AR_S32  w;         // 宽度
    AR_S32  h;         // 高度
}STRU_AR_HAL_VO_RECT;

/**
* @note   帧数据指针
*/
typedef struct
{
    AR_VOID   *buffer;          // 虚拟地址，对齐后
    AR_VOID   *buffer_orign;    // 虚拟地址，对齐前，用于释放
    AR_VOID   *buffer_pa;       // 物理地址，对齐后
    AR_VOID   *buffer_pa_orign; // 物理地址，对齐前，用于释放
    AR_U32     length;          // 数据内容长度
 }STRU_AR_HAL_VO_PANNEL;

/**
* @note   定义视频图像帧结构
*/
typedef struct
{
    AR_U32                 frame_id;                     //  图像帧id
    ENUM_AR_HAL_VO_FMT     format;                       //  图像帧数据格式
    AR_S32                 frame_width;                  //  图像帧宽
    AR_S32                 frame_height;                 //  图像帧高
    STRU_AR_HAL_VO_RECT    roi;                          //  显示区域
    AR_S32                 luma_stride;                  //  y步幅
    AR_S32                 chroma_stride;                //  uv步幅
    AR_U32                 pannel_num;
    STRU_AR_HAL_VO_PANNEL  pannel[MAX_VO_PANNEL_NUM];    //  数据存放指针 当format = AR_HAL_VO_FMT_YV12时
                                                         //  当format为rgb时，仅使用pannel[0]
                                                         //  当format为yuv时，pannel[0]存放y
                                                         //                   pannel[1]存放u
                                                         //                   pannel[2]存放v
    AR_S32                 interlace_filed_flag;         //  帧interlace标志，0：非interlace帧;
                                                         //                   1:顶场帧;
                                                         //                   2:底场帧
    AR_U64                 pts;                          //  时间戳
    AR_VOID                *usr_data;                    //  存放用户数据
    AR_VOID                *priv_data;                   //  请勿填冲
}STRU_AR_HAL_VO_DISP_BUF;

/**
* @note   用户定制的显示时序.
*/
typedef struct
{
    AR_BOOL   interlace_mod;         // 0：  p模式       1：i模式
    AR_S32    hpw;                   // 水平同步脉冲宽度
    AR_S32    hbp;                   // 水平消隐后肩宽度
    AR_S32    hdp;                   // 水平有效显示宽度
    AR_S32    hfp;                   // 水平消隐前肩宽度
    AR_S32    vpw;                   // 垂直同步脉冲宽度
    AR_S32    vbp;                   // 垂直消隐后肩宽度
    AR_S32    vdp;                   // 垂直有效显示宽度
    AR_S32    vfp;                   // 垂直消隐前肩宽度
    AR_FLOAT  fps;                   // 帧率
    AR_BOOL   de_polarity;           // de信号极性       0：高电平有效 1:低电平有效
    AR_BOOL   da_polarity;           // 数据信号极性 0：高电平有效 1:低电平有效
    AR_BOOL   clock_polarity;        // 时钟信号极性 0：高电平有效 1:低电平有效
    AR_BOOL   hsync_polarity;        // 水平同步信号极性 0：高电平有效 1:低电平有效
    AR_BOOL   vsync_polarity;        // 垂直同步信号极性 0：高电平有效 1:低电平有效
}STRU_AR_HAL_VO_SYNC_INFO;

/**
* @note   当e_interface设置为AR_HAL_VO_DEV_INTF_DVP，e_sub_interface设置为AR_HAL_VO_DEV_SUB_INTF_HDMI，
          则sdk默认启动it66121的驱动。
*/
typedef struct
{
    ENUM_AR_HAL_VO_HDMI_MODE           mode;                     // AR_HAL_VO_HDMI_MODE_DEFAULT：将使用evb的配置，i2c=3，gpio_group=1, gpio_port=1,gpio_pin=2;
                                                                 // AR_HAL_VO_HDMI_MODE_USER:    i2c, gpio_group, gpio_port, gpio_pin自定义
    AR_U8                              i2c_no;                   // i2c编号
    AR_U8                              i2c_addr;                 // i2c地址
    AR_U8                              gpio_group;               // gpio group编号
    AR_U8                              gpio_port;                // gpio port 编号
    AR_U8                              gpio_pin;                 // gpio_pin编号
    AR_U8                              reserved[3];              // 保留
}STRU_AR_HAL_VO_HDMI_INFO;

/** VO时钟相位信息选择 */
typedef enum
{
    ENUM_AR_VO_CLK_PHASE_DEFAULT,                  /**<@note     default值*/
    ENUM_AR_VO_CLK_PHASE0,                         /**<@note     VO的时钟相位为0度*/
    ENUM_AR_VO_CLK_PHASE90,                        /**<@note     VO的时钟相位为90度，ar9341不支持*/
    ENUM_AR_VO_CLK_PHASE180,                       /**<@note     VO的时钟相位为180度*/
    ENUM_AR_VO_CLK_PHASE270,                       /**<@note     VO的时钟相位为270度, ar9341不支持*/
} ENUM_AR_VO_CLK_PHASE;

/** 时钟配置方式 */
typedef enum
{
    ENUM_AR_VO_CLK_AUTO,                           /**<@note     VO的时钟源自动选择*/
    ENUM_AR_VO_CLK_MANUAL,                         /**<@note     用户使用hal层接口进行配置*/
} ENUM_AR_HAL_CLK_CONTROL;
typedef enum
{
    ENUM_AR_VO_IRQ_TYPE_INTERVAL_LINE      = 1 << 0,
    ENUM_AR_VO_IRQ_TYPE_FIX_LINE           = 1 << 1,
    ENUM_AR_VO_IRQ_TYPE_WRITE_BACK         = 1 << 2,
    ENUM_AR_VO_IRQ_TYPE_WB_LOAD            = 1 << 3,
    ENUM_AR_VO_IRQ_TYPE_DE_DMA_FRAME_DONE  = 1 << 4,
    ENUM_AR_VO_IRQ_TYPE_VSYNC_START        = 1 << 5,
    ENUM_AR_VO_IRQ_TYPE_FRAME_DONE         = 1 << 6
}ENUM_AR_HAL_VO_SUBSCRIBE_TYPE;
typedef struct
{
    AR_U32                  src_frameid;
    AR_U32                  src_pts;
}STRU_AR_HAL_VO_FRAME_DONE_PARA_S;

typedef union
{
    STRU_AR_HAL_VO_FRAME_DONE_PARA_S     frame_done_para;
}STRU_AR_HAL_VO_SUBSCRIBE_EXT_PARA_S;

typedef struct
{
    AR_U32                               irq_type;
    AR_U64                               irq_time_ns;
    STRU_AR_HAL_VO_SUBSCRIBE_EXT_PARA_S  ext_para;
} STRU_AR_HAL_VO_SUBSCRIBE_INFO;

typedef enum
{
    AR_HAL_VO_LOWDELAY_MODE_NULL              =  0,
    AR_HAL_VO_LOWDELAY_MODE_NORMAL            =  1,
}ENUM_AR_HAL_VO_LOWDELAY_MODE;

typedef struct
{
    AR_U64       step_time_ps;
    AR_U64       threshold_us;
    AR_FLOAT     init_diff;
}STRU_AR_HAL_VO_FRE_ADJUST;

typedef struct
{
    ENUM_AR_HAL_VO_LOWDELAY_MODE              enMode;
    STRU_AR_HAL_VO_FRE_ADJUST                 stFreAdjust;
}STRU_AR_HAL_VO_LOWDELAY;

/**
* @note   视频输出设备属性.
*/
typedef struct
{
    AR_U32                              bg_color;                // 背景色，格式为RGB888
    STRU_AR_HAL_VO_SYNC_INFO            timing_customize;        // 客户定制时序
    ENUM_AR_HAL_VO_DEV_INTF             e_interface;             // 输出接口类型，93xx仅支持DVP和MIPI
    ENUM_AR_HAL_VO_DEV_SUB_INTF         e_sub_interface;         // 当e_interface为AR_HAL_VO_DEV_INTF_DVP时，此参数有效
    ENUM_AR_HAL_VO_DEV_TIMING_TEMPLATE  e_timing_template;       // 时序模板
    AR_U32                              bit_count_per_channel ;  // 废弃
    ENUM_AR_HAL_VO_DPI_FORMAT           dpi_format;              // DPI输出位宽
    ENUM_AR_HAL_VO_DPI_COLOR_MODE       color_mode;              // 输出格式
    ENUM_AR_HAL_VO_SYNC_MODE            sync_mode;               // 同步模式
    ENUM_AR_HAL_PIN_MODE                pin_mode;                // 输出线序
    STRU_AR_HAL_VO_HDMI_INFO            hdmi_info;               // hdmi输出参数
    ENUM_AR_HAL_CLK_CONTROL             clk_control;             // 时钟控制
    ENUM_AR_VO_CLK_PHASE                clk_phase;               // clk相位
}STRU_AR_HAL_VO_DEV_ATTR;

/**
* @note   鼠标位置坐标.
*/
typedef struct
{
    AR_U32    x;              // x坐标
    AR_U32    y;              // y坐标
}STRU_AR_HAL_VO_POS;

/* FIFO队列，用来缓存数据(帧可能有抖动)。
 * 每次送显的时候，先检查FIFO队列，如果连续多个周期FIFO队列的长度都大于阈值，
 * 则主动丢弃FIFO中的历史数据，以实现时延可以快速减小到预期值。*/
typedef struct
{
    AR_BOOL         bEnable;
    AR_U32          u32Depth;
    AR_U32          u32ClearCycle;
    AR_U32          u32Threshold;
} STRU_AR_HAL_VO_FIFO_INFO;

/**
* @note   视频层属性.
*/
typedef struct
{
    AR_S32                   width;               // 宽度
    AR_S32                   height;              // 高度
    AR_S32                   luma_stride;         // y步幅
    AR_S32                   chroma_stride;       // uv步幅
    ENUM_AR_HAL_VO_FMT       format;              // 视频层图像格式
    AR_FLOAT                 fps;                 // 显示帧率
    ENUM_AR_HAL_VO_ROTATION  rotation;            //
    STRU_AR_HAL_VO_FIFO_INFO fifo_info;
}STRU_AR_HAL_VO_LAYER_ATTR;

/**
* @note   视频层CSC.
*/
typedef struct
{
    ENUM_AR_HAL_VO_CSC    input;   // 视频层输入CSC
    ENUM_AR_HAL_VO_CSC    output;  // 视频层输出CSC
}STRU_AR_HAL_VO_LAYER_CSC;

/**
* @note   视频通道属性.
*/
typedef struct
{
    AR_U32    priority;            // 通道优先级
    STRU_AR_HAL_VO_RECT   rect;     // 通道位置
}STRU_AR_HAL_VO_CHN_ATTR;

/**
* @note   通道幅形比.
*/
typedef enum
{
    AR_HAL_VO_ASPECT_RATIO_NONE,        /* 无幅形比，拉伸显示 */
    AR_HAL_VO_ASPECT_RATIO_AUTO,        /* 自动模式，按照帧显示*/
    AR_HAL_VO_ASPECT_RATIO_MANUAL,      /* 手动模式 */
    AR_HAL_VO_ASPECT_RATIO_MAX
}ENUM_AR_HAL_VO_ASPECT_RATIO;

/**
* @note   定义幅形比信息.
*/
typedef struct STRU_ASPECT_RATIO_S
{
    ENUM_AR_HAL_VO_ASPECT_RATIO   e_mode;          /* 幅形比类型 */
    STRU_AR_HAL_VO_RECT           st_videoRect;    /* 幅形比视频区域 */
    AR_U32                        u32_bgColor;     /* 背景颜色, RGB 888 */
} STRU_AR_HAL_VO_ASPECT_RATIO_S;

/**
* @note   通道参数.
*/
typedef struct
{
    STRU_AR_HAL_VO_ASPECT_RATIO_S st_AspectRatio;  /*  幅形比信息 */
}STRU_AR_VO_CHN_PARAM_S;

typedef AR_VOID* AR_HAL_VO_DEV_HANDLE;
typedef AR_VOID* AR_HAL_VO_LAYER_HANDLE;

enum
{
    MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_0_PARA     = 0x03,
    MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA     = 0x13,
    MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA     = 0x23,

    MIPI_DATA_TYPE_GENERIC_SHORT_READ_0_PARA      = 0x04,
    MIPI_DATA_TYPE_GENERIC_SHORT_READ_1_PARA      = 0x14,
    MIPI_DATA_TYPE_GENERIC_SHORT_READ_2_PARA      = 0x24,

    MIPI_DATA_TYPE_DCS_SHORT_WRITE_0_PARA         = 0x05,
    MIPI_DATA_TYPE_DCS_SHORT_WRITE_1_PARA         = 0x15,

    MIPI_DATA_TYPE_DCS_SHORT_READ_0_PARA          = 0x06,

    MIPI_DATA_TYPE_SET_MAX_READ_SIZE              = 0x37,

    MIPI_DATA_TYPE_NULL_PACKET                    = 0x09,
    MIPI_DATA_TYPE_BLANK_PACKET                   = 0x19,
    MIPI_DATA_TYPE_GENERIC_LONG_WRITE             = 0x29,
    MIPI_DATA_TYPE_DCS_LONG_WRITE                 = 0x39,

    MIPI_DATA_TYPE_PACKED_PIXEL_RGB565            = 0x0E,
    MIPI_DATA_TYPE_PACKED_PIXEL_RGB666            = 0x1E,
    MIPI_DATA_TYPE_LOOSELY_PACKED_PIXEL_RGB666    = 0x2E,
    MIPI_DATA_TYPE_PACKED_PIXEL_RGB888            = 0x3E
};

typedef struct
{
    AR_U32  hsa;          // 水平同步脉冲宽度
    AR_U32  hbp;          // 水平消隐后肩宽度
    AR_U32  hact;         // 水平有效显示宽度
    AR_U32  hfp;          // 水平消隐前肩宽度

    AR_U32  vsa;          // 垂直同步脉冲宽度
    AR_U32  vbp;          // 垂直消隐后肩宽度
    AR_U32  vact;         // 垂直有效显示宽度
    AR_U32  vfp;          // 垂直消隐前肩宽度
}STRU_DSI_SYNC_INFO;

typedef struct
{
    AR_U32         dev_no;               //   dsi设备号，目前为0
    AR_U32         lane[MAX_LANE_NUM];   //   lane使能，1为使能 , 0为禁用
    AR_U32         bits_per_pixel;       //   像素位宽
    STRU_DSI_SYNC_INFO   sync_info;      //   时序
    AR_FLOAT       dphy_clk_mhz;         //   DPhy时钟
    AR_U32         pll_freq_reg_2c0;     //   Dphy时钟寄存器值
    AR_U32         pll_freq_reg_38c;     //   Dphy时钟寄存器值
}STRU_AR_HAL_VO_DSI_ATTR;

typedef struct
{
    AR_U32         dev_no;               //   dsi设备号
    AR_U16         data_type;            //   dsi数据类型
    AR_U16         cmd_size;             //   当发送短命令时，需将cmd填NULL，cmd_size[0:7]作为短命令的参数1
                                         //   cmd_size[8:15]作为短命令的参数2
                                         //   当发送长命令时，cmd指向命令内容，cmd_size填入命令长度
    AR_U8         *cmd;
}STRU_AR_HAL_VO_DSI_CMD;

typedef struct
{
    AR_U32         dev_no;               //   dsi设备号
    AR_U16         data_type;            //   dsi数据类型
    AR_U16         data_size;            //   data指向内容，data_size填入需读取数据的长度
    AR_U32         para;                 //   [0:7]作为短命令的参数1
                                         //   [8:15]作为短命令的参数2
    AR_U8         *data;                 //   内容存放指针
}STRU_AR_HAL_VO_DSI_READ;


//=============================================================================

// Global function definition

//=============================================================================

AR_S32 ar_hal_vo_dev_open(ENUM_AR_HAL_VO_DEV_ID e_dev_id);
AR_S32 ar_hal_vo_fd_close(AR_S32 fd);

/**
* @brief  使能视频输出设备.
* @param  e_dev_id  视频输出设备号.
* @retval 0 成功 , 其它 失败.
* @note   在使能设备前，需要调用ar_hal_vo_dev_attr_set()配置输出设备的属性.
*/
AR_S32 ar_hal_vo_dev_enable(AR_S32 fd);

/**
* @brief  禁用视频输出设备.
* @param  e_dev_id  视频输出设备号.
* @retval 0 成功 , 其它 失败.
* @note   在禁用设备前需禁用相关视频层.
*/
AR_S32 ar_hal_vo_dev_disable(AR_S32 fd);

/**
* @brief  配置视频输出设备的属性.
* @param  e_dev_id  视频输出设备号.
* @param  attr  视频输出设备属性结构体指针.
* @retval 0 成功 , 其它 失败.
* @note   视频输出设备属性需在ar_hal_vo_dev_enable()前设置
* @note   视频输出设备属性的使用说明请参考STRU_AR_HAL_VO_DEV_ATTR
*/
AR_S32 ar_hal_vo_dev_set_attr(AR_S32 fd, STRU_AR_HAL_VO_DEV_ATTR *attr);

/**
* @brief  获取视频输出设备的属性.
* @param  e_dev_id  视频输出设备号.
* @param  attr  视频输出设备属性结构体指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_get_attr(AR_S32 fd, STRU_AR_HAL_VO_DEV_ATTR *attr);


/**
* @brief  显示固定图片为背景.
* @param  e_dev_id  视频输出设备号.
* @param  e_image   固定图片编号
* @retval 0 成功 , 其它 失败.
* @note   当设置为固定图片后，所有入队列的显示buffer将不被显示，建议用户将所有buffer出列
*/
AR_S32 ar_hal_vo_dev_set_background(AR_S32 fd, ENUM_AR_HAL_VO_IMAGE e_image);

/**
* @brief  配置Lowdelay属性.
* @param  fd       设备句柄.
* @param  lowdelay 属性指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_set_lowdelay(AR_S32 fd, STRU_AR_HAL_VO_LOWDELAY *lowdelay);

/**
* @brief  获取Lowdelay属性.
* @param  fd       设备句柄.
* @param  lowdelay 属性指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_get_lowdelay(AR_S32 fd, STRU_AR_HAL_VO_LOWDELAY *lowdelay);

/**
* @brief  设置输出设备csc色域.
* @param  e_dev_id  视频输出设备号.
* @param  e_csc     csc转换矩阵.
* @retval 0 成功 , 其它 失败.
* @note   该接口将会设置该视频设备输出的色域
*/
AR_S32 ar_hal_vo_dev_set_csc(AR_S32 fd, ENUM_AR_HAL_VO_CSC *e_csc);

/**
* @brief  获取输出设备csc色域.
* @param  e_dev_id  视频输出设备号.
* @param  e_csc     csc转换矩阵指针.
* @retval 0 sucess , others failed.
* @note   查询该视频设备输出的色域
*/
AR_S32 ar_hal_vo_dev_get_csc(AR_S32 fd, ENUM_AR_HAL_VO_CSC *e_csc);

/**
* @brief  加载鼠标图标文件.
* @param  e_dev_id  视频输出设备号.
* @param  file_name  图标文件.
* @retval 0 成功 , 其它 失败.
* @note   鼠标文件格式为ARGB888
*/
AR_S32 ar_hal_vo_dev_cfg_cursor_file(AR_S32 fd, AR_CHAR *file_name);

/**
* @brief  使能鼠标显示.
* @param  e_dev_id  视频输出设备号.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_enable_cursor(AR_S32 fd);

/**
* @brief  禁用鼠标显示.
* @param  e_dev_id  视频输出设备号.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_disable_cursor(AR_S32 fd);

/**
* @brief  设置dsi相关属性.
* @param  fd    设备句柄.
* @param  attr  dsi接口属性.
* @retval 0 成功 , 其它 失败.
* @note   调用该接口后，dsi将复位，需调用ar_hal_vo_dsi_enable使dsi进入video模式
*/
AR_S32 ar_hal_vo_dsi_set_attr(AR_S32 fd, STRU_AR_HAL_VO_DSI_ATTR *attr);

/**
* @brief  发送dsi命令.
* @param  fd    设备句柄.
* @param  vo_dsi_cmd   命令参数指针.
* @retval 0 成功 , 其它 失败.
* @note   当发送短命令时，需将vo_dsi_cmd->cmd填NULL，cmd_size[0:7]作为短命令的参数1
          cmd_size[8:15]作为短命令的参数2
          当发送长命令时，vo_dsi_cmd->cmd指向命令内容，cmd_size填入命令长度
*/
AR_S32 ar_hal_vo_dsi_cmd(AR_S32 fd, STRU_AR_HAL_VO_DSI_CMD *vo_dsi_cmd);

/**
* @brief  使能dsi设备.
* @param  fd    设备句柄.
* @retval 0 成功 , 其它 失败.
* @note   调用该接口将使dsi进入video模式，如需重新进入cmd模式，需重新调用ar_hal_vo_dsi_set_attr
*/
AR_S32 ar_hal_vo_dsi_enable(AR_S32 fd);


/**
* @brief  通过dsi读取数据.
* @param  fd    设备句柄.
* @param  vo_dsi_read   数据参数指针.
* @retval 0 成功 , 其它 失败.
* @note   vo_dsi_cmd->data指向数据读取胡内容，data_size填入读取长度
*/
AR_S32 ar_hal_vo_dsi_read(AR_S32 fd, STRU_AR_HAL_VO_DSI_READ *vo_dsi_read);


/**
* @brief  设置鼠标坐标.
* @param  e_dev_id  视频输出设备号.
* @param  pos  x,y坐标.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_set_cursor_pos(AR_S32 fd, STRU_AR_HAL_VO_POS *pos);

/**
* @brief  获取鼠标坐标.
* @param  e_dev_id  视频输出设备号.
* @param  pos  x,y坐标.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_dev_get_cursor_pos(AR_S32 fd, STRU_AR_HAL_VO_POS *pos);


/**
* @brief  配置VO订阅中断信息属性(动态接口)
* @param[in]  fd    设备句柄.
* @param[out] sub_type       属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_vo_subscribe_register(AR_S32 fd, AR_U32 *sub_type);

/**
* @brief  获取订阅信息
* @param[in]  fd    设备句柄.
* @param[out] s_sub_info       属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_vo_get_subscribe_info(AR_S32 fd, STRU_AR_HAL_VO_SUBSCRIBE_INFO *s_sub_info);

/**
* @brief  更新订阅中断信息类型(动态接口)
* @param[in]  fd    设备句柄.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_vo_subscribe_clear(AR_S32 fd);

/**
* @brief  复位抖动统计直方图
* @param[in]  fd    设备句柄.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_vo_reset_skewing_histogram(AR_S32 fd);

/**
* @brief  获取抖动统计直方图
* @param[in]  fd    设备句柄.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_vo_get_skewing_histogram(AR_S32 fd, AR_U64 *pstSkewing, int count);


AR_S32 ar_hal_vo_layer_open(ENUM_AR_HAL_VO_LAYER_ID e_layer_id);

/**
* @brief  使能视频层
* @param  e_layer_id  视频层编号
* @retval 0 成功 , 其它 失败.
* @note   使能前需保证视频层已配置属性
*/
AR_S32 ar_hal_vo_layer_enable(AR_S32 fd);


/**
* @brief  禁用视频层
* @param  e_layer_id  视频层编号
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_layer_disable(AR_S32 fd);

/**
* @brief  设置视频层属性.
* @param  e_layer_id  视频层编号
* @param  attr        视频层属性，详细请看STRU_AR_HAL_VO_LAYER_ATTR描述.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_layer_set_attr(AR_S32 fd, STRU_AR_HAL_VO_LAYER_ATTR *attr);

/**
* @brief  获取视频层属性.
* @param  e_layer_id  视频层编号
* @param  attr        视频层属性，详细请看STRU_AR_HAL_VO_LAYER_ATTR描述.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_layer_get_attr(AR_S32 fd, STRU_AR_HAL_VO_LAYER_ATTR *attr);

/**
* @brief  设置视频层csc色域.
* @param  e_layer_id  视频层编号
* @param  e_csc     csc转换矩阵.
* @retval 0 成功 , 其它 失败.
* @note   该接口将会设置向视频层输入数据的色域
*/
AR_S32 ar_hal_vo_layer_set_csc(AR_S32 fd, STRU_AR_HAL_VO_LAYER_CSC *csc);

/**
* @brief  获取视频层csc色域.
* @param  e_layer_id  视频层编号
* @param  e_csc     csc转换矩阵指针.
* @retval 0 sucess , others failed.
* @note   查询该视频层输入的色域
*/
AR_S32 ar_hal_vo_layer_get_csc(AR_S32 fd, STRU_AR_HAL_VO_LAYER_CSC *csc);

/**
* @brief  捕获指定视频层的图像.
* @param  e_layer_id  视频层编号
* @param  buffer  获取输出图像数据信息的指针.
* @param  milli_sec  超时时间.
* @retval 0 成功 , 其它 失败.
* @note   使用完成后，用户需使用ar_hal_vo_layer_release_disp_buf进行释放
*/
AR_S32 ar_hal_vo_layer_get_frame(AR_S32 fd,  STRU_AR_HAL_VO_DISP_BUF *buffer, AR_S32 milli_sec);

/**
* @brief  释放图像数据.
* @param  e_layer_id  视频层编号
* @param  buffer  释放的输出图像数据信息的指针.
* @param  milli_sec  超时时间.
* @retval 0 成功 , 其它 失败.
* @note   由于获取的是vo内部使用的buffer，因此请尽快释放该，否则vo功能可能会不正常
*/
AR_S32 ar_hal_vo_layer_release_frame(AR_S32 fd, STRU_AR_HAL_VO_DISP_BUF *buffer, AR_S32 milli_sec);

/**
* @brief  设置视频层位置.
* @param  e_layer_id  视频层编号
* @param  pos  x,y坐标.
* @retval 0 成功 , 其它 失败.
* @note   AR_HAL_VO_LAYER_ID_VIDEO_0    不支持
          AR_HAL_VO_LAYER_ID_OVERLAY_0  支持
*/
AR_S32 ar_hal_vo_layer_set_pos(AR_S32 fd, STRU_AR_HAL_VO_POS *pos);

/**
* @brief  获取视频层位置.
* @param  e_layer_id  视频层编号
* @param  pos  x,y坐标.
* @retval 0 成功 , 其它 失败.
* @note   AR_HAL_VO_LAYER_ID_VIDEO_0    不支持
          AR_HAL_VO_LAYER_ID_OVERLAY_0  支持
*/
AR_S32 ar_hal_vo_layer_get_pos(AR_S32 fd, STRU_AR_HAL_VO_POS *pos);

AR_S32 ar_hal_vo_chn_open(ENUM_AR_HAL_VO_LAYER_ID e_layer_id, AR_U32 chn_id);


/**
* @brief  启用指定的视频输出通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note   请确认指定通道已配置属性和参数
*/
AR_S32 ar_hal_vo_chn_enable(AR_S32 fd);

/**
* @brief  禁用指定的视频输出通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_disable(AR_S32 fd);

/**
* @brief  配置通道的属性.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  attr        通道属性指针
* @retval 0 成功 , 其它 失败.
* @note   属性中优先级值越大，优先级越高，当各通道区域有重叠时，高优先级通道的图像覆盖
          低优先级的
*/
AR_S32 ar_hal_vo_chn_set_attr(AR_S32 fd, STRU_AR_HAL_VO_CHN_ATTR *attr);

/**
* @brief  查询通道的属性.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  attr        通道属性指针
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_get_attr(AR_S32 fd, STRU_AR_HAL_VO_CHN_ATTR *attr);


/**
* @brief  设置通道参数.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  para        通道参数指针
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_set_para(AR_S32 fd, STRU_AR_VO_CHN_PARAM_S *para);


/**
* @brief  获取通道参数.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  para        通道参数指针
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_get_para(AR_S32 fd, STRU_AR_VO_CHN_PARAM_S *para);

/**
* @brief  设置通道的显示区域.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  pos         通道显示区域
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_set_position(AR_S32 fd, STRU_AR_HAL_VO_RECT *pos);

/**
* @brief  获取通道的显示区域.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  pos         通道显示区域
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_get_position(AR_S32 fd, STRU_AR_HAL_VO_RECT *pos);


/**
* @brief  释放指定通道的图像.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  buffer      获取输出图像数据信息的指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_release_frame(AR_S32 fd, STRU_AR_HAL_VO_DISP_BUF *buffer);

/**
* @brief  往通道推送图像数据.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  buffer  视频数据帧指针.
* @retval 0 成功 , 其它 失败.
* @note   调用前须确保通道已经使能。
          用户需自行启线程调用ar_hal_vo_chn_wait_frame_ok()等待显示完成
          再调用ar_hal_vo_chn_dequeue()回收数据帧进行处理
*/
AR_S32 ar_hal_vo_chn_send_frame(AR_S32 fd, STRU_AR_HAL_VO_DISP_BUF *buffer, AR_S32 milli_sec);

/**
* @brief  从通道回收帧.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @param  buffer  视频数据帧指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_get_frame(AR_S32 fd, STRU_AR_HAL_VO_DISP_BUF *buffer, AR_S32 milli_sec);

/**
* @brief  清除指定通道的frame.
* @param  fd          通道fd
* @param  clr_all     清除标志      true：清除所有视频帧， false: 保留一帧
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_clear_frame(AR_S32 fd, AR_BOOL clr_all);


/**
* @brief  暂停指定的通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_pause(AR_S32 fd);

/**
* @brief  恢复指定的通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_resume(AR_S32 fd);

/**
* @brief  单帧播放指定的通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_step(AR_S32 fd);

/**
* @brief  显示指定的通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_show(AR_S32 fd);

/**
* @brief  隐藏指定的通道.
* @param  e_layer_id  视频层编号
* @param  chan_id     通道编号 范围:       [0, MAX_VO_CHN_NUM]
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_vo_chn_hide(AR_S32 fd);

/**
* @brief   发送suspend命令
* @param   fd  设备fd.
* @retval 0 成功 , 其它 失败.
*/
AR_S32  ar_hal_vo_suspend(AR_S32 fd);

/**
* @brief    发送resume命令
* @param   fd  设备fd.
* @retval 0 成功 , 其它 失败.
*/
AR_S32  ar_hal_vo_resume(AR_S32 fd);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
