/**
 * @file hal_ge2d.h
 * @brief 定义2D图形加速器用户接口
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef _AR_HAL_GE2D_API_H_
#define _AR_HAL_GE2D_API_H_

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

//=============================================================================

// Macro definition

//=============================================================================


//=============================================================================

// Data type definition

//=============================================================================

#define AR_HAL_GE2D_DEV_NAME     "ge2d_server"
#define AR_HAL_MOD_GE2D          HAL_TAG_ID(AR_SYS_ID_GE2D)

typedef enum
{
    GE2D_SUCCUESS                     =   0,

    GE2D_ERR_INPUT_PARA               =   0x40,
    GE2D_ERR_CREATE_CLIENT,
    GE2D_ERR_DELETE_CLIENT,
    GE2D_ERR_MALLOC_FRAME,
    GE2D_ERR_GET_FRAME,
    GE2D_ERR_DEV_OPEN,
    GE2D_ERR_DEV_CLOSE,
    GE2D_ERR_LINE_NUM,
    GE2D_ERR_BMP_NUM,
    GE2D_ERR_STATE
}ENUM_GE2D_ERRNO;

#define  HAL_ERR_GE2D_INPUT_PARA                     AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_INPUT_PARA)
#define  HAL_ERR_GE2D_CREATE_CLIENT                  AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_CREATE_CLIENT)
#define  HAL_ERR_GE2D_DELETE_CLIENT                  AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_DELETE_CLIENT)
#define  HAL_ERR_GE2D_MALLOC_FRAME                   AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_MALLOC_FRAME)
#define  HAL_ERR_GE2D_GET_FRAME                      AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_GET_FRAME)
#define  HAL_ERR_GE2D_DEV_OPEN                       AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_DEV_OPEN)
#define  HAL_ERR_GE2D_DEV_CLOSE                      AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_DEV_CLOSE)
#define  HAL_ERR_GE2D_LINE_NUM                       AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_LINE_NUM)
#define  HAL_ERR_GE2D_BMP_NUM                        AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_BMP_NUM)
#define  HAL_ERR_GE2D_STATE                          AR_HAL_DEF_ERR(AR_SYS_ID_GE2D, HAL_ERR_LEVEL_ERROR, GE2D_ERR_STATE)

/**
* @note    2D图形加速器接口类型
*/
typedef enum
{
    GE2D_FORMAT_ARGB8888       = 0,
    GE2D_FORMAT_ARGB4444,
    GE2D_FORMAT_ARGB1555,
    GE2D_FORMAT_YV12,
    GE2D_FORMAT_NV12,
    GE2D_FORMAT_MONOCHROMA,
    GE2D_FORMAT_GRAY1,
    GE2D_FORMAT_GRAY4,
    GE2D_FORMAT_GRAY8,
    GE2D_FORMAT_CF50,
    GE2D_FORMAT_MAX
}ENUM_GE2D_FOMART;

/**
* @note    支持的色域类型
*/
typedef enum
{
    GE2D_CSC_BT601_TV         = 0,
    GE2D_CSC_BT601_PC,
    GE2D_CSC_BT709_TV,
    GE2D_CSC_BT709_PC,
}ENUM_GE2D_CSC;

/**
* @note    2D图形加速器参数结构，描述画布的属性
*/
typedef struct
{
    ENUM_GE2D_FOMART  e_format;       //  数据格式
    AR_S32            width;          //  宽
    AR_S32            height;         //  高
    AR_S32            y_stride;       //  亮度分量的步幅
    AR_S32            uv_stride;      //  色度分量的步幅
    ENUM_GE2D_CSC     e_csc_mode;     //  色域
}STRU_GE2D_CLIENT_PARA;

/**
* @note    命令帧开始条件参数，画布的基地址(需为物理地址)
*/
typedef struct
{
    AR_VOID  *addr_y;                 // 亮度分量的基地址
    AR_VOID  *addr_u;                 // 蓝色分量(Cr)的基地址
    AR_VOID  *addr_v;                 // 红色分量(Cb)的基地址
}STRU_GE2D_START_FRAME_PARA;

/**
* @note    画线开始条件参数
*/
typedef struct
{
    AR_S32   use_global_alfa;         // 是否使用全局透明变量           0：不使用;     1：使用
    AR_S32   alfa;                    // 本地透明变量
    AR_S32   rgb;                     // 线的颜色
}STRU_GE2D_LINE_PARA;

/**
* @note    画线参数
*/
typedef struct
{
    AR_S32  alfa_en;                  // 是否开启透明混合
    AR_S32  line_w;                   // 线宽
    AR_S32  start_x;                  // 起始点x坐标
    AR_S32  start_y;                  // 起始点y坐标
    AR_S32  end_x;                    // 结束点x坐标
    AR_S32  end_y;                    // 结束点y坐标
    AR_S32  end_line;                 // 是否为本次画线的最后一根线
}STRU_GE2D_LINE_APPEND_PARA;

/**
* @note    画图像参数
*/
typedef struct
{
    AR_S32  use_global_alfa;          // 是否使用全局透明变量;            0：不使用; 1：使用
    AR_S32  alfa;                     // 本地透明变量
    ENUM_GE2D_FOMART  e_format;       // 数据格式
    AR_S32  src_x;                    // 源图像在源画布的x坐标
    AR_S32  src_y;                    // 源图像在源画布的y坐标
    AR_S32  y_stride;                 // 源画布的亮度分量步幅
    AR_S32  uv_stride;                // 源画布的色度分量步幅
    AR_VOID  *addr_y;                 // 源画布的亮度分量基地址
    AR_VOID  *addr_u;                 // 源画布的红色分量基地址
    AR_VOID  *addr_v;                 // 源画布的蓝色分量基地址
    AR_S32  dest_x;                   // 目的图像在目的画布的x坐标
    AR_S32  dest_y;                   // 目的图像在目的画布的y坐标
    AR_S32  roi_w;                    // 图像数据宽度
    AR_S32  roi_h;                    // 图像数据高度
}STRU_GE2D_BMP_PARA;

/**
* @note    图像旋转参数
*/
typedef struct
{
    AR_S32  use_global_alfa;          // 是否使用全局透明变量;            0：不使用; 1：使用
    AR_S32  alfa;                     // 本地透明变量
    ENUM_GE2D_FOMART  e_format;       // 数据格式
    AR_S32  src_x;                    // 源图像在源画布的x坐标
    AR_S32  src_y;                    // 源图像在源画布的y坐标
    AR_S32  y_stride;                 // 源画布的亮度分量步幅
    AR_S32  uv_stride;                // 源画布的色度分量步幅
    AR_VOID  *addr_y;                 // 源画布的亮度分量基地址
    AR_VOID  *addr_u;                 // 源画布的红色分量基地址
    AR_VOID  *addr_v;                 // 源画布的蓝色分量基地址
    AR_S32  dest_x;                   // 目的图像在目的画布的x坐标
    AR_S32  dest_y;                   // 目的图像在目的画布的y坐标
    AR_S32  roi_w;                    // 图像数据宽度
    AR_S32  roi_h;                    // 图像数据高度
    AR_S32  back_color;               // 背景色,需预先乘以透明量
    AR_FLOAT  degree;                 // 顺时针旋转角度
}STRU_GE2D_ROTATION_PARA;

/**
* @note    图像拉伸参数
*/
typedef struct
{
    AR_S32  use_global_alfa;          // 是否使用全局透明变量;            0：不使用; 1：使用
    AR_S32  alfa;                     // 本地透明变量
    ENUM_GE2D_FOMART  e_format;       // 数据格式
    AR_S32  src_x;                    // 源图像在源画布的x坐标
    AR_S32  src_y;                    // 源图像在源画布的y坐标
    AR_S32  src_w;                    // 源图像数据宽度
    AR_S32  src_h;                    // 源图像数据高度
    AR_S32  y_stride;                 // 源画布的亮度分量步幅
    AR_S32  uv_stride;                // 源画布的色度分量步幅
    AR_VOID  *addr_y;                 // 源画布的亮度分量基地址
    AR_VOID  *addr_u;                 // 源画布的红色分量基地址
    AR_VOID  *addr_v;                 // 源画布的蓝色分量基地址
    AR_S32  dest_x;                   // 目的图像在目的画布的x坐标
    AR_S32  dest_y;                   // 目的图像在目的画布的y坐标
    AR_S32  dest_w;                   // 目的图像数据宽度
    AR_S32  dest_h;                   // 目的图像数据高度
}STRU_GE2D_STRETCH_BLIT_PARA;

/**
* @note    马赛克参数
*/
typedef struct
{
    AR_S32  use_global_alfa;          // 是否使用全局透明变量;            0：不使用; 1：使用
    AR_S32  alfa;                     // 本地透明变量
    AR_S32  x;                        // 马赛克在源画布的x坐标
    AR_S32  y;                        // 马赛克在源画布的y坐标
    AR_S32  w;                        // 马赛克数据宽度
    AR_S32  h;                        // 马赛克数据高度
    AR_S32  level;
}STRU_GE2D_MOSAIC_PARA;

typedef struct
{
    AR_S32  use_global_alfa;          // 是否使用全局透明变量;            0：不使用; 1：使用
    AR_S32  alfa;                     // 本地透明变量
    ENUM_GE2D_FOMART  e_format;       // 数据格式
    AR_S32  src_x;                    // 源图像在源画布的x坐标
    AR_S32  src_y;                    // 源图像在源画布的y坐标
    AR_S32  y_stride;                 // 源画布的亮度分量步幅
    AR_S32  uv_stride;                // 源画布的色度分量步幅
    AR_VOID  *addr_y;                 // 源画布的亮度分量基地址
    AR_VOID  *addr_u;                 // 源画布的红色分量基地址
    AR_VOID  *addr_v;                 // 源画布的蓝色分量基地址
    AR_S32  dest_x;                   // 目的图像在目的画布的x坐标
    AR_S32  dest_y;                   // 目的图像在目的画布的y坐标
    AR_S32  roi_w;                    // 图像数据宽度
    AR_S32  roi_h;                    // 图像数据高度
    AR_S32  color_key_high;           // color key值高门限，  格式：ARGB888
    AR_S32  color_key_low;            // color key值低门限，格式：ARGB888
}STRU_GE2D_COLOR_KEY_PARA;

typedef struct
{
    AR_S32  use_global_alfa;         // 是否使用全局透明变量;            0：不使用; 1：使用
    AR_S32  alfa;                    // 本地透明变量
    ENUM_GE2D_FOMART  e_format;      // 数据格式
    AR_S32  src_x;                   // 源图像在源画布的x坐标
    AR_S32  src_y;                   // 源图像在源画布的y坐标
    AR_S32  y_stride;                // 源画布的亮度分量步幅
    void   *addr_y;                  // 源画布的亮度分量基地址
    AR_S32  dest_x;                  // 目的图像在目的画布的x坐标
    AR_S32  dest_y;                  // 目的图像在目的画布的y坐标
    AR_S32  roi_w;                   // 图像数据宽度
    AR_S32  roi_h;                   // 图像数据高度
    AR_S32  front_color;             // 图像前景颜色
    AR_S32  back_color;              // 图像后景颜色
    AR_S32  need_revert_color;       // 是否反色
    AR_S32  th;                      // 反色阈值
    AR_S32  gray_endian;             // 当格式为gray时，字节内bit数据存放顺序
                                     // LSB:d0   1:  MSB:d0
    AR_S32  is_cached;               // 使用的内存是否cached
}STRU_GE2D_DOT_MAT_PARA;

typedef struct
{
    AR_S32      w;                  // 宽
    AR_S32      h;                  // 高
    AR_S32      x;                  // x坐标
    AR_S32      y;                  // y坐标
}STRU_GE2D_REGION_PARA;

typedef struct
{
    AR_U32      blue_sum;           // 蓝色总和
    AR_U32      green_sum;          // 绿色总和
    AR_U32      red_sum;            // 红色总和
    AR_U32      info;               // bit[0:19]  区域像素点个数
                                    // bit[20:31] 区域编号
}STRU_GE2D_REGION_STATS_RESULT;

typedef struct
{
    AR_S32                          frame_id;
    AR_U32                          region_count;
    STRU_GE2D_REGION_STATS_RESULT   stats[0];
}STRU_GE2D_REGION_STATS;

//=============================================================================

// Global function definition

//=============================================================================

/**
* @brief  打开2D图形加速器设备，获取设备句柄.
* @retval <0 失败 , 其它 句柄.
* @note
*/
AR_S32 ar_hal_ge2d_open_dev(void);

/**
* @brief  关闭2D图形加速器设备.
* @param  fd   ge2d server 句柄
* @retval 0 成功 , <0 失败.
* @note
*/
AR_S32 ar_hal_ge2d_close_dev(AR_S32 fd);


/**
* @brief  设置2D图形加速器的参数.
* @param  fd   ge2d server 句柄
* @param  para    参数结构指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_set_client_para(AR_S32 fd, STRU_GE2D_CLIENT_PARA *para);

/**
* @brief  申请一个命令帧.
* @param  fd   ge2d server 句柄
* @param  frame id   帧编号指针
* @retval >=0 成功, 其余失败.
* @note   2D图形加速器支持最多G2ED_FRAME_POOL_SIZE个命令帧，每个帧均可独立执行
*/
AR_S32 ar_hal_ge2d_malloc_frame(AR_S32 fd, AR_S32 *frame_id);

/**
* @brief  释放命令帧.
* @param  fd   ge2d server 句柄
* @param  frame_id   帧编号.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_free_frame(AR_S32 fd, AR_S32 frame_id);

/**
* @brief  执行命令帧.
* @param  client_id  客户端编号
* @param  frame_id   帧编号.
* @param  addr       画布基地址.
* @param  para       画布参数.
* @retval 0 成功 , 其它 失败.
* @note   该接口为同步接口.
*/
AR_S32 ar_hal_ge2d_process_frame(AR_S32 fd, AR_S32 frame_id,
                                           STRU_GE2D_START_FRAME_PARA *addr,
                                           STRU_GE2D_CLIENT_PARA      *para);


/**
* @brief  添加命令帧的开始条件.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @retval 0 成功 , 其它 失败.
* @note   命令帧可执行多项操作，一个完整的命令帧需以ar_hal_ge2d_add_start()开始，
          以ar_hal_ge2d_add_end()结束。各项操作需在两个接口之间
          典型调用顺序:
          ar_hal_ge2d_open_dev                         =>
          ar_hal_ge2d_malloc_frame                     =>
          ar_hal_ge2d_add_start                        =>
          job1 : ar_hal_ge2d_stretch_blit              =>
          job2 : ar_hal_ge2d_dot_matrix                =>
          job n: ...........                           =>
          ar_hal_ge2d_add_end                          =>
          ar_hal_ge2d_process_frame                    =>
          ar_hal_ge2d_free_frame                       =>
          ar_hal_ge2d_close_dev                        =>
*/
AR_S32 ar_hal_ge2d_add_start(AR_S32 fd,  AR_S32 frame_id);

/**
* @brief  添加命令帧的结束条件.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_add_end(AR_S32 fd, AR_S32 frame_id);

/**
* @brief  添加画线的起始条件
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  line       画线起始参数结构指针.
* @retval 0 成功 , 其它 失败.
* @note   支持批量画线
          2D图形加速器批量画线的接口调用顺序:
          ar_hal_ge2d_draw_line_start   =>
          ar_hal_ge2d_append_line 1     =>
          ar_hal_ge2d_append_line 2     =>
                   .........            =>
          ar_hal_ge2d_append_line n     =>
          ar_hal_ge2d_add_line_end      =>
*/
AR_S32 ar_hal_ge2d_add_line_start(AR_S32 fd, AR_S32 frame_id,
                                            STRU_GE2D_LINE_PARA  *line);

/**
* @brief  添加画线.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  line       画线参数结构指针.
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_append_line(AR_S32 fd, AR_S32 frame_id,
                                        STRU_GE2D_LINE_APPEND_PARA  *line);

/**
* @brief  添加画线的结束条件
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_ge2d_add_line_end(AR_S32 fd, AR_S32 frame_id);

/**
* @brief  添加图像.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  bmp        图像参数结构指针.
* @retval 0 成功 ,   其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_add_bmp(AR_S32 fd, AR_S32 frame_id, STRU_GE2D_BMP_PARA  *bmp);

/**
* @brief  添加旋转效果.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  rotation   旋转参数结构指针.
* @retval 0 成功 ,   其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_add_rotation(AR_S32 fd, AR_S32 frame_id,
                                        STRU_GE2D_ROTATION_PARA  *rotation);

/**
* @brief  添加马赛克效果.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  mosaic     马赛克参数结构指针.
* @retval 0 成功 ,   其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_add_mosaic(AR_S32 fd, AR_S32 frame_id,
                                        STRU_GE2D_MOSAIC_PARA  *mosaic);

/**
* @brief  添加color key.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  color_key  color key参数结构指针.
* @retval 0 成功 ,   其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_add_color_key(AR_S32 fd, AR_S32 frame_id,
                                         STRU_GE2D_COLOR_KEY_PARA  *color_key);

/**
* @brief  添加拉伸操作.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  stretch    拉伸参数结构指针.
* @retval 0 成功 ,   其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_stretch_blit(AR_S32 fd, AR_S32 frame_id,
                                         STRU_GE2D_STRETCH_BLIT_PARA  *stretch);

/**
* @brief  添加字符点阵.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  dot_matrix 字符点阵结构指针.
* @retval 0 成功 ,   其它 失败.
* @note
*/
AR_S32 ar_hal_ge2d_dot_matrix(AR_S32 fd, AR_S32 frame_id,
                                         STRU_GE2D_DOT_MAT_PARA  *dot_matrix);

/**
* @brief  添加指定区域的颜色统计.
* @param  fd         ge2d句柄
* @param  frame_id   帧编号.
* @param  region     区域结构指针.
* @retval 0 成功 ,   其它 失败.
* @note   目前一个frame最多能增加32个统计区域
*/
AR_S32 ar_hal_ge2d_add_region_stats(AR_S32 fd, AR_S32 frame_id,
                                                STRU_GE2D_REGION_PARA  *region);


/**
* @brief  获取区域的颜色统计.
* @param  fd         ge2d server 句柄
* @param  stats      区域统计结构指针.
* @retval 0 成功 ,   其它 失败.
* @note   接口会根据stats->region_count的数量拷贝统计数据到stats->stats,
          用户需确保预留了有足够的内存，否则会内存越界！！！
*/
AR_S32 ar_hal_ge2d_get_region_stats(AR_S32 fd, STRU_GE2D_REGION_STATS *stats);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
