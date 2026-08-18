/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VI
 *  @{
 */

/**
 * @file hal_vin_aec_algo_lib.h
 * @brief 定义vin的aec 模块的数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_VIN_AEC_ALGO_LIB_H__
#define __HAL_VIN_AEC_ALGO_LIB_H__

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

#define MAX_FACE_STATS_COUNT 16

typedef enum
{
  AEC_CTL_HW_START,
  AEC_CTL_HW_STOP,
  AEC_CTL_HW_PICK_RES,
  AEC_CTL_HW_SET_IN_OUT_FORMAT,
  AEC_CTL_UPDATE_TUNING_PRTA,
  AEC_CTL_GET_IMG_BRIGHTNESS,
  AEC_CTL_SET_IMG_BRIGHTNESS,
  AEC_CTL_SET_FACE_AEC,
  AEC_CTL_SET_AE_MODE,
  AEC_CTL_GET_AE_MODE,
  AEC_CTL_SET_MANUAL_AE,
  AEC_CTL_SET_SENCE_MODE,
  AEC_CTL_GET_SCENE_MODE,
  AEC_CTL_CAM_PRA_SET_SENSOR_HDR,
  AEC_CTL_CAM_PRA_GET_SENSOR_HDR,
  AEC_CTL_CAM_PRA_SET_HDR_AUTO_DETECT,
  AEC_CTL_CAM_PRA_GET_HDR_AUTO_DETECT,
  AEC_CTL_CAM_PRA_SET_HDR_PRA_MODE,
  AEC_CTL_CAM_PRA_GET_HDR_PRA_MODE,
  AEC_CTL_CAM_PRA_SET_FPS_RANGE,
  AEC_CTL_CAM_PRA_SET_EXP_LIMIT,
  AEC_CTL_CAM_PRA_GET_EXP_LIMIT,
  AEC_CTL_CAM_PRA_SET_ANTIBANDING,
  AEC_CTL_CAM_PRA_GET_ANTIBANDING,
  AEC_CTL_CAM_PRA_SET_MOTION_DETECT,
  AEC_CTL_CAM_PRA_GET_3A_INFO,
  AEC_CTL_CAM_PRA_GET_BASIC_3A_INFO,
  AEC_CTL_EVENT_AWB_UPDATE,
  AEC_CTL_SET_DEBUG_INFO,
  AEC_CTL_GET_AEC_EXT_INFO,
  AEC_CTL_GET_LIMIT_INFO,
  AEC_CTL_UPDATE,
  AEC_CTL_SET_EXP_POLICY,
  AEC_CTL_GET_EXP_POLICY,    
  AEC_CTL_CAM_PRA_SET_BLACK_LIGHT_MODE, 
  AEC_CTL_CAM_PRA_GET_BLACK_LIGHT_MODE,
}ENUM_AEC_CTL_T;

typedef struct
{
    AR_S32 is_init;
	STRU_ALGO_ISP_TUNING_T *p_algo_tuning;
    STRU_ISP_TUNING_PRA_T	*p_isp_tuning_pra;
}STRU_AEC_CLT_UPDATE_TUNING_PRA_T;

typedef struct
{
   AR_S32 x;
   AR_S32 y;
   AR_S32 width;
   AR_S32 height;
}STRU_AEC_ROI_T;

typedef struct
{
	AR_S32 pattern;
	AR_S32 x_offset;
	AR_S32 y_offset;
	AR_S32 ver_s;
	AR_S32 hor_s;
	AR_S32 x_skip;
	AR_S32 y_skip;
	AR_S32 image_width;
	AR_S32 image_height;
	AR_S32 block_width;
	AR_S32 block_height;
	AR_S32 th_s;

	//add for face aec
	AR_S32 is_face_stats;
	AR_S32 data_x;
	AR_S32 data_y;
	AR_S32 data_w;
	AR_S32 data_h;
} STRU_STATS_CFG_INFO_T;


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
}STRU_AEC_ALGO_RES_INFO_T;


typedef struct{
  AR_U32 camera_id;
  AR_U32 frame_id;
  AR_U64 pts_us;
  STRU_MESH_GRID_STATS_DATA_T stats_data;
  AR_S32 raw_hist_bin_num;
  AR_U32 p_raw_hist_r[128];
  AR_U32 p_raw_hist_g[128];
  AR_U32 p_raw_hist_b[128];
  AR_S32 rgb_his_bin_num;
  AR_U32 p_rgb_hist[128];
  //face aec stats ,software stats
  AR_S32 face_aec_stats_w;
  AR_S32 face_aec_stats_h;
  AR_U32 p_face_aec_mesh_grid_stats[16*16];
  //hdr stats, revert for 9301 isp
  //to do later
  STRU_MESH_GRID_STATS_DATA_T hdr_rro1_stats_data;
  STRU_MESH_GRID_STATS_DATA_T hdr_rro2_stats_data;
  STRU_MESH_GRID_STATS_DATA_T hw_face_aec_stats_data;
  AR_VOID *priv;//other type input data for extend
}STRU_AEC_ALOG_LIB_INPUT_T;

typedef enum{
	LONG_SHORT_EXP_RATION,
	MIDDLE_SHORT_EXP_RATION,
	MIDDLEPLUS_SHORT_EXP_RATION,
	SHORT_SHORT_EXP_RATION,
}ENUM_EXP_RATION_INDEX_T;


typedef struct{
  AR_S32 camera_id;
  AR_S32	   updated;
  AR_U32 line_count;
  AR_U32 exp_time_us;
  AR_FLOAT    real_gain; //will trigger the tuning pra update
  AR_FLOAT    sensor_gain;//this gain will write to the sensor
  AR_FLOAT    isp_gainl;
  AR_FLOAT    isp_gain2;
  AR_FLOAT    exp_gain;
  AR_U32 exp_index;
  AR_U32 lux_index;
  AR_U32 lux_value;
  AR_U32 ae_settle;
  AR_U32 current_luma;

  //hdr output
  AR_U32 short_exp_index;
  /*
     //ration = long/short
     exp ration array:
     index 0: short exp ration  long/short
     index 1: midle exp ration  midle/short
     index 2: midle+ exp ration midle+/short
     index 3: exp ration, always is 1 short/short
  */
  AR_FLOAT	 ration_gain[4];
  AR_FLOAT	 ration_time[4];
  AR_FLOAT	 total_raion[4];
  AR_FLOAT	 drc_gain;
  AR_FLOAT	 drc_gain_1; //for ceva drc gain
  AR_FLOAT	 drc_gain_2;//for isp drc gain
  AR_FLOAT fps;
}STRU_AEC_ALGO_LIB_OUTPUT_T;


typedef struct
{
   AR_FLOAT r_gain;
   AR_FLOAT b_gain;
}STRU_AEC_ALGO_AWB_UPDATE_T;


typedef struct
{
   AR_U32 fps_min;
   AR_U32 fps_max;
   AR_U32 fps_min_line_count;
   AR_U32 fps_max_line_count;
}STRU_AEC_LIB_SET_FPS_INFOR_T;


typedef enum
{
   AEC_CB_EVENT_INIT,
   AEC_CB_EVENT_EXIT,
   AEC_CB_EVENT_RUN,
   AEC_CB_EVENT_CTL,
   AEC_CB_EVENT_MAX,
}ENUM_AEC_CB_EVENT_T;

typedef struct
{
   AR_S32 camera_id;
}STRU_AEC_CB_EVENT_INIT_PRA_T;

typedef struct
{
   AR_S32 camera_id;
}STRU_AEC_CB_EVENT_EXIT_PRA_T;

typedef struct
{
   AR_S32 camera_id;
   STRU_AEC_ALOG_LIB_INPUT_T *p_aec_algo_lib_input;
   STRU_AEC_ALGO_LIB_OUTPUT_T *p_aec_algo_lib_output;
}STRU_AEC_CB_EVENT_RUN_PRA_T;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 clt_code;
   AR_VOID *pra;
   AR_S32 ctl_size;
}STRU_AEC_CB_EVENT_CTL_PRA_T;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
