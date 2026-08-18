#ifndef __HAL_VIN_AWB_ALGO_LIB_H__
#define __HAL_VIN_AWB_ALGO_LIB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "math.h"
#include "hal_vin_tuning_def.h"
#include "hal_vin_type_def.h"

/**
 * @file hal_vin_awb_algo_lib.h
 * @brief 定义vin的awb 模块的数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

typedef enum
{
  AWB_CTL_HW_START,
  AWB_CTL_HW_STOP,
  AWB_CTL_HW_PICK_RES,
  AWB_CTL_HW_SET_IN_OUT_FORMAT,
  AWB_CTL_UPDATE_TUNING_PRTA,
  AWB_CTL_EVENT_AEC_UPDATE,
  AWB_CTL_SET_DEBUG_INFO,
  AWB_CTL_CAM_PRA_SET_AWB_MODE,
  AWB_CTL_CAM_PRA_SET_MANUAL_AWB,
  AWB_CTL_CAM_PRA_GET_BASIC_3A_INFO,
  AWB_CTL_CAM_PRA_GET_3A_INFO,
  AWB_CTL_CAM_GET_OUT_EXT,
  AWB_CTL_UPDATE,
}ENUM_AWB_CTL_T;

typedef struct
{
	AR_S32 ImgWidth;
	AR_S32 ImgHeight;
	AR_S32 parZoom;
	AR_S32 ver_s;
	AR_S32 hor_s;
	AR_S32 BitsSum;
	AR_S32 pattern;
	AR_S32 X_offset;
	AR_S32 Y_offset;
	AR_S32 roi_width;
	AR_S32 roi_height;
	AR_S32 Block_width;
	AR_S32 Block_height;
	AR_S32 X_skip;
	AR_S32 Y_skip;
}STRU_AWB_STATS_CFG_T;


typedef struct
{
  AR_S32 frame_id;  
  AR_U64 pts_us;
  AR_S32 mesh_grid_w;
  AR_S32 mesh_grid_h;
  AR_U32 p_mesh_grid_stats_r_sum[AWB_NEARGRAY_SIZE];
  AR_U32 p_mesh_grid_stats_g_sum[AWB_NEARGRAY_SIZE];
  AR_U32 p_mesh_grid_stats_b_sum[AWB_NEARGRAY_SIZE];
  AR_U32 p_mesh_grid_stats_num[AWB_NEARGRAY_SIZE];
}STRU_AWB_ALGO_LIB_INPUT_T;

typedef struct
{
  AR_FLOAT r_gain;
  AR_FLOAT g_gain;
  AR_FLOAT b_gain;
  AR_FLOAT cct;
}STRU_AWB_ALGO_LIB_OUTPUT_T;

typedef struct
{
    AR_S32 is_init;
	STRU_ALGO_ISP_TUNING_T *p_algo_tuning;
    STRU_ISP_TUNING_PRA_T	*p_isp_tuning_pra;
}STRU_AWB_CTL_UPDATE_TUNING_PRA_T;


typedef struct
{
    STRU_CAM_API_PRE_PRA_T pre_pra;
	AR_U32 preview_fps;
	AR_U32 preview_linesPerFrame;
	AR_U16 pixel_clock_per_line;
	AR_U32 pixel_clock;
	AR_U32 sensor_hdr;
	AR_U32 hdr_frame_count;
	AR_U32 new_preview_fps;
	AR_U32 new_preview_linesPerFrame;
	//reverted by artosyn algo, not care for third algo
    AR_S32 vif_isp_mode;
}STRU_AWB_ALGO_RES_INF0_T;


typedef struct
{
  AR_FLOAT lux_index;
  AR_S32   ae_settle;
  AR_FLOAT current_luma;
}STRU_AWB_ALGO_LIB_UPDATE_T;


typedef enum
{
   AWB_CB_EVENT_INIT,
   AWB_CB_EVENT_EXIT,
   AWB_CB_EVENT_RUN,
   AWB_CB_EVENT_CTL,
   AWB_CB_EVENT_MAX,
}ENUM_CB_EVENT_T;

typedef struct
{
   AR_S32 camera_id;
}STRU_AWB_CB_EVENT_INIT_PRA_T;

typedef struct
{
   AR_S32 camera_id;
}STRU_AWB_CB_EVENT_EXIT_PRA_T;

typedef struct
{
   AR_S32 camera_id;
   STRU_AWB_ALGO_LIB_INPUT_T *p_awb_algo_lib_input;
   STRU_AWB_ALGO_LIB_OUTPUT_T *p_awb_algo_lib_output;
}STRU_AWB_CB_EVENT_RUN_PRA_T;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 clt_code;
   AR_VOID *pra;
   AR_S32 ctl_size;
}STRU_AWB_CB_EVENT_CTL_PRA_T;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
