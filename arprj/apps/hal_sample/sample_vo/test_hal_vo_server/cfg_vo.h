#ifndef __CFG_VO_H__
#define __CFG_VO_H__

#include "cfg_common.h"

#define CFG_MAX_VO_CHN_NUM  9

/***********************************/
/***        vo device            ***/
/***********************************/
//遮挡区域设置
typedef struct {
    AR_S32  dev_id;                 /* ENUM_AR_HAL_VO_DEV_ID */
    AR_S32  dev_csc;                /* ENUM_AR_HAL_VO_CSC */
    AR_U32  bg_color;               /* 背景色，格式为RGB888 */
    AR_S32  interface;              /* ENUM_AR_HAL_VO_DEV_INTF */
    AR_S32  sub_interface;          /* ENUM_AR_HAL_VO_DEV_SUB_INTF */
    AR_S32  timing_template;        /* ENUM_AR_HAL_VO_DEV_TIMING_TEMPLATE*/
	AR_BOOL is_interlace;           /* interlace or not */
	AR_U32  bit_count_per_channel;  /* 图像位宽 */

	AR_S32  layer_id;               /* ENUM_AR_HAL_VO_LAYER_ID */
	AR_S32  overlayer_num;          /* overlay层数量，1使用overlay0，2使用overlay0和overlay1 */
	AR_S32  layer_csc_in;           /* ENUM_AR_HAL_VO_CSC */
	AR_S32  layer_csc_out;          /* ENUM_AR_HAL_VO_CSC */
	AR_S32  width;                  /* 视频层图像宽度 */
	AR_S32  height;                 /* 视频层图像高度 */
	AR_U32  pos_x;                  /* STRU_AR_HAL_VO_POS */
	AR_U32  pos_y;                  /* STRU_AR_HAL_VO_POS */
	AR_FLOAT fps;                    /* 视频层显示帧率 */
	AR_S32  format;                 /* ENUM_AR_HAL_VO_FMT */

	AR_S32  cursor_enable;          /* enable cursor */
	AR_S32  cursor_pos_x;           /* STRU_AR_HAL_VO_POS */
	AR_S32  cursor_pos_y;           /* STRU_AR_HAL_VO_POS */
	AR_CHAR cursorpath[256];        /* cursor图片路径 */
} VO_DEV_S;

/***********************************/
/***        vo channel           ***/
/***********************************/
typedef struct {
    AR_BOOL enable;                 /* 通道使能 0-禁用, 1-使能 */
    AR_U32  priority;               /* 通道优先级 */
	AR_U32  bg_color;               /* 通道背景色，格式为RGB888 */
	AR_S32  ch_width;               /* 通道宽度 */
	AR_S32  ch_height;              /* 通道高度 */
	AR_U32  ch_pos_x;               /* 通道起始位置x坐标 */
	AR_U32  ch_pos_y;               /* 通道起始位置y坐标 */
    AR_S32  aspect_ratio_mode;      /* ENUM_AR_HAL_VO_ASPECT_RATIO */
	AR_S32  aspect_ratio_width;     /* 幅形比视频区域宽度 */
	AR_S32  aspect_ratio_height;    /* 幅形比视频区域高度 */
	AR_U32  aspect_ratio_pos_x;     /* 幅形比视频区域起始位置x坐标 */
	AR_U32  aspect_ratio_pos_y;     /* 幅形比视频区域起始位置y坐标 */
	AR_S32  image_width;            /* 通道图像宽度 */
	AR_S32  image_height;           /* 通道图像高度 */
    AR_CHAR imagepath[256];         /* 通道图片路径 */
} VO_CHN_S;

typedef struct
{
    VO_DEV_S stVoDev;
	VO_CHN_S stVoChn[CFG_MAX_VO_CHN_NUM];
} CFG_VO_S;

AR_S32 CFG_VO_Load(AR_CHAR *cfg_file);
void CFG_VO_Print();
CFG_VO_S *CFG_VO_GetParam();

#endif
