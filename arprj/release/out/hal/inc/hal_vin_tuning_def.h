/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VI
 *  @{
 */

/**
 * @file hal_vin_tuning_def.h
 * @brief 定义vin的tunning模块的api及数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_VIN_TUNING_DEF_H__
#define __HAL_VIN_TUNING_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "hal_type.h"
#include "hal_errno.h"
#include "hal_comm.h"

#define TUNING_VERSION (0X20230616)

#define BLOCK_ROW 36
#define BLOCK_COL 16
#define MAX_TRIGGER_COUNT 16
#define HIGH_TRIGGER_COUNT 8
#define MIDDLE_TRIGGER_COUNT 5
#define MIN_TRIGGER_COUNT 3
#define NO_TRIGGER_COUNT  1
#define LSC_TRIGGER_COUNT 6
#define LSC_TABLE_NUM 208
#define LSC_TUNING_NUM 300
#define AWB_LIGHT_WEIGHT_COUNT 16
#define AWB_LIGHT_WEIGHT_COUNT2 10
#define AWB_LOWLIGHT_LUT_COUNT 16
#define HDR_RATION_TALBE_SIZE 64

#define HDR_FAST_RATION_TALBE_SIZE 64

#define AF_BLOCK_ROW (16)
#define AF_BLOCK_COL (9)

typedef enum
{
 AEC_TRIGGER_GAIN,
 AEC_TRIGGER_LUX,
}ENUM_AEC_TRIGGER_MODE_T;
typedef struct
{
 AR_FLOAT start;
 AR_FLOAT end;
}STRU_TRIGGER_T;
typedef struct
{
 STRU_TRIGGER_T trig_val;
}STRU_AEC_TRIGGER_T;
typedef struct
{
 STRU_TRIGGER_T trig_val;
}STRU_AWB_TRIGGER_T;

typedef struct
{
 STRU_TRIGGER_T trig_val;
}STRU_EXP_RATION_TRIGGER_T;


typedef struct
{
 AR_S32 enable;
 AR_S32 count;
 AR_S32 trigger_mode;
 STRU_AEC_TRIGGER_T aec_trigger_lumatarget[MIN_TRIGGER_COUNT];
 AR_U32 lumtarget[MIN_TRIGGER_COUNT];
}STRU_LUMA_TARGET_PARA_T;
#define MAX_EXPOSUER_COUNT 1024
#define MAX_AEC_STATS_BLOCKS_COUNT (64*48)

typedef struct
{
  AR_U32 gain; //Q8 format, 256 as 1;
  AR_U32 line_count;//
}STRU_EXP_TABLE_T;

typedef struct
{
  AR_S32 count;//how much exposure table
  AR_S32 ev0_count;
  AR_S32 ev2_count;
  AR_S32 fix_fps_index;
  AR_S32 enable_isp_digital_gain;//if 1 enable the isp digital gain for lowlight
  AR_FLOAT max_sensor_gain;
  AR_FLOAT max_isp_gain1;
  STRU_EXP_TABLE_T exp_table[MAX_EXPOSUER_COUNT];
  AR_S32 enable_short_exp_table;
  STRU_EXP_TABLE_T exp_table_short[MAX_EXPOSUER_COUNT];
}STRU_EXP_TABLE_PARA_T;
typedef struct
{
  AR_U32 r_weight;
  AR_U32 g_weight;
  AR_U32 b_weight;
}STRU_RGB_TO_Y_WEIGHT_T;
typedef  struct
{
  AR_S32 weight_count;
  AR_U32 weight[MAX_AEC_STATS_BLOCKS_COUNT];
  AR_U32 weight_spot[MAX_AEC_STATS_BLOCKS_COUNT];
}STRU_WEIGHT_TABLE_T;

typedef enum
{
	BAYER_AEC,
	HYBRID_AEC
}ENUM_STATS_TYPE_T;

#define LUMA_TARGET_NUM 16


typedef struct
{
  AR_S32 exp_index;
  AR_S32 target;
}STRU_LUMA_TARGET_LUT_T;


typedef struct
{
	AR_U32 outdoor_luma_target_compensated;
	AR_U32 default_luma_target_compensated;
	AR_U32 lowlight_luma_target;
	AR_FLOAT outdoor_index;
	AR_FLOAT indoor_index;
	AR_U32 lowlight_start_idx;
	AR_U32 lowlight_end_idx;
	AR_U32 luma_target_short;
	AR_S32 enable_luma_target_lut;
	AR_S32 count;
	STRU_LUMA_TARGET_LUT_T luma_target[LUMA_TARGET_NUM];
}STRU_LUMA_TARGET_T;

typedef struct
{
	AR_U32 extreme_luma_target_offset;
}STRU_SNOW_DETECT_T;

typedef struct
{
	AR_U32 backlight_max_la_luma_target_offset;
}STRU_BACKLIT_DETECT_T;

typedef struct
{
	AR_U32 motion_iso_threshold;
} STRU_AEC_MOTION_DETECT_T;

typedef struct
{
	AR_S32 speed;
	AR_S32 luma_tolerance;
	AR_S32 frame_skip;
	AR_S32 fine_adjust_skip;
	AR_S32 luma_nostable_torlerance;
	AR_S32 luma_nostable_torlerance_max;
	AR_S32 settled_to_nosettle_skip_count;//we we find stable to unstable , if skip count is passed ,it still > luma_nostable_torlerance, we will start adjust the aec
} STRU_FAST_CONV_T;

typedef enum
{
	STATS_PROC_ANTIBANDING_NO,
	STATS_PROC_ANTIBANDING_50HZ,
	STATS_PROC_ANTIBANDING_60HZ,
}ENUM_STATS_PROC_ANTIBANDING_T;

typedef enum
{
	AEC_METERING_SPOT_METERING,
    AEC_METERING_CENTER_WEIGHTED,
	AEC_METERING_SIMPLE_FRAME_AVERAGE
}ENUM_AEC_METERING_MODE_T;

typedef struct {
  /* Tuning parameters */
  AR_S32 hist_target_adjust_enable;
  AR_FLOAT outdoor_max_target_adjust_ratio;
  AR_FLOAT outdoor_min_target_adjust_ratio;
  AR_FLOAT indoor_max_target_adjust_ratio;
  AR_FLOAT indoor_min_target_adjust_ratio;
  AR_FLOAT lowlight_max_target_adjust_ratio;
  AR_FLOAT lowlight_min_target_adjust_ratio;
  AR_FLOAT target_filter_factor;
  AR_FLOAT hist_sat_pct;
  AR_FLOAT hist_dark_pct;
  AR_FLOAT hist_sat_low_ref;
  AR_FLOAT hist_sat_high_ref;
  AR_FLOAT hist_dark_low_ref;
  AR_FLOAT hist_dark_high_ref;
} STRU_HIST_TARGET_T;

typedef struct {
    AR_U32 over_exp_enable;
	AR_U32 high_luma_region_threshold;
    AR_FLOAT outdoor_over_exp_adjust_ratio;
    AR_FLOAT indoor_over_exp_adjust_ratio;
    AR_FLOAT lowlight_over_exp_adjust_ratio;
    AR_FLOAT outdoor_over_exp_adjust_offset;
    AR_FLOAT indoor_over_exp_adjust_offset;
    AR_FLOAT lowlight_over_exp_adjust_offset;
    AR_U32 outdoor_over_exp_max_count;
    AR_U32 indoor_over_exp_max_count;
    AR_U32 lowlight_over_exp_max_count;
    AR_U32 outdoor_over_exp_min_count;
    AR_U32 indoor_over_exp_min_count;
    AR_U32 lowlight_over_exp_min_count;
}STRU_OVER_EXP_T;


typedef struct
{
   AR_S32   exp_index_short;
   AR_FLOAT ration0; //long/short
   AR_FLOAT ration1; //mid/short
   AR_FLOAT ration2; //mid+/short
   AR_FLOAT ration3;//short/short3
   AR_S32   luma_target;
   AR_FLOAT max_drc_gain;
   AR_FLOAT min_drc_gain;
   AR_FLOAT max_drc_gain1;//ceva drc gain max
   AR_S32   luma_settle_th;
}STRU_RATION_TABLE_T;


typedef struct
{
   AR_S32	 en_hdr_detect;
   AR_FLOAT   low_luma_hdr;
   AR_FLOAT   high_luma_hdr;
   AR_FLOAT   low_luma_normal;
   AR_FLOAT   high_luma_nornal;
   AR_S32   skip_count_hdr_detct;
   AR_S32   use_lux_index;
   AR_S32   low_lux_index_hdr;
   AR_S32   high_lux_index_hdr;
   AR_S32   low_lux_index;
   AR_S32   high_lux_index;
}STRU_HDR_DETECT_T;

typedef enum
{
    AEC_HDR_ALGO_TYPE_ORG = 0,
    AEC_HDR_ALGO_TYPE_FAST = 1,
    AEC_HDR_ALGO_TYPE_FAST_ANDVANCE = 2, //9301 long short exp stats
    AEC_HDR_ALGO_TYPE_MAX
}ENUM_AEC_HDR_ALGO_T;

typedef struct
{
    //use the value in STRU_HDR_TUNING_EXP_T

    // AR_S32 full_sweep_en;

    // AR_S32 luma_tolerance;

    //short exp adjust
    // AR_S32 short_exp_max_index;
    AR_S32 short_exp_adjust_skip_count;
    AR_U32 short_luma_interval_threshold[3];
    AR_FLOAT short_luma_speed_ratio[3];
    AR_FLOAT short_luma_speed_ratio_others;

    //ration table
    // AR_S32 ration_interp_en;
    // AR_S32 ration_talbe_size;
    // STRU_RATION_TABLE_T ration_table[HDR_FAST_RATION_TALBE_SIZE];

    //drc adjust
    AR_S32 drc_gain_adjust_skip_count;
    AR_FLOAT drc_gain_interval_threshold[3];
    AR_FLOAT drc_gain_adjust_ratio[3];
    AR_FLOAT drc_gain_adjust_ratio_others;

    //hist
    // AR_S32 use_averge_hist;
    // AR_FLOAT current_hist_weight;

    // AR_S32 over_exp_bin_th;//max 127 min 0
    // AR_S32 over_exp_per_high;//when the over exp over the value, the shortest exp is decided
    // AR_S32 over_exp_per_low;

    //deprecated

    // always enable
    // AR_S32   enable_short_exp_luma_adjust;
    // AR_S32 enble_ration_table;

    // always use advance??
    // AR_S32 enable_drc_gain_adjust;

} STRU_AEC_HDR_FAST_PARAM_T;

typedef struct
{
    AR_S32 enable;
	AR_S32 enable_hdr_luma_tartget;
	AR_S32 short_exp_max_index;
	AR_FLOAT max_exp_ration;
	AR_S32 enble_ration_table;
    AR_S32 ration_talbe_size;
	AR_S32 over_exp_per_high;//when the over exp over the value, the shortest exp is decided
	AR_S32 over_exp_per_low;
	AR_S32 adjust_step;
	AR_S32 ration_interp_en;
	STRU_RATION_TABLE_T ration_table[HDR_RATION_TALBE_SIZE];
	AR_S32 over_exp_bin_th;//max 127 min 0
	AR_S32 short_exp_skip_count;
	AR_FLOAT current_hist_weight;
	AR_S32   use_averge_hist;
	AR_S32   short_target_th;
	AR_S32   enable_short_exp_luma_adjust;
	AR_S32   full_sweep_en;
	AR_S32   enable_drc_gain_adjust;
	AR_FLOAT drc_gain_adjust_factor;
	AR_S32   use_over_exp_luma;
	STRU_HDR_DETECT_T hdr_detect;
	AR_S32  en_drc_gain_advance_adjust;
    STRU_AEC_HDR_FAST_PARAM_T aec_hdr_fast_param;
	AR_FLOAT max_mid_exp_ration;
	AR_S32 enable_over_cut_long_exp;
	AR_S32 enable_short_gain_cut_max;
}STRU_HDR_TUNING_EXP_T;


typedef struct
{
    AR_S32 enable;
	AR_S32 lux_index_low;
	AR_S32 lux_index_hight;
}STRU_NIGHT_DETECT_T;

typedef struct
{
    AR_S32 enable;
	AR_S32 abs_th;
}STRU_MOTION_DETECT_T;

typedef enum
{
   FACE_STATS_TYPE_MESH_MATCH,
   FACE_STATS_TYPE_HW,
   FACE_STATS_TYPE_SOFT,
   FACE_STATS_TYPE_MAX,
}ENUM_FACE_STATS_TYPE_T;


typedef struct
{
    AR_S32 use_face_pra;
    AR_S32 face_luma_target;
	AR_FLOAT face_weight;
    AR_S32 face_skip_count;
	AR_FLOAT filter_weight;
	AR_S32 en_face_bright_dark_region;
	AR_S32 bright_high_th;
	AR_S32 bright_low_th;
	AR_S32 dark_high_th;
	AR_S32 dark_low_th;
	AR_FLOAT bright_high_th_w;
	AR_FLOAT bright_low_th_w;
	AR_FLOAT dark_high_th_w;
	AR_FLOAT dark_low_th_w;
	AR_S32 en_face_luma_cut;
	AR_S32 face_luma_high;
	AR_S32 face_luma_low;
	AR_S32 enable_w_dec;
	AR_S32 enable_region_percent;
	AR_FLOAT region_percent_low_limit;
	AR_FLOAT region_percent_low;
	AR_FLOAT region_percent_high;
	AR_FLOAT region_percent_high_limit;
	AR_FLOAT region_percent_low_limit_w;
	AR_FLOAT region_percent_low_w;
	AR_FLOAT region_percent_high_w;
	AR_FLOAT region_percent_high_limit_w;
	#if 0
	AR_S32 enable_center_weight;
	AR_FLOAT w_3x3[3][3];
	AR_FLOAT w_5x5[5][5];
	AR_FLOAT w_7x7[3][3];
	AR_FLOAT w_9x9[5][5];
	#endif
	AR_S32 face_aec_stats_type;
}STRU_FACE_AEC_PRA_T;

typedef struct
{
  AR_FLOAT lux_index;
  AR_FLOAT extreme_color_rg_th_low_limit;
  AR_FLOAT extreme_color_rg_th_low;
  AR_FLOAT extreme_color_rg_th_high;
  AR_FLOAT extreme_color_rg_th_high_limit;
  AR_FLOAT extreme_color_bg_th_low_limit;
  AR_FLOAT extreme_color_bg_th_low;
  AR_FLOAT extreme_color_bg_th_high;
  AR_FLOAT extreme_color_bg_th_high_limit;

  AR_FLOAT extreme_color_rg_th_low_limit_w;
  AR_FLOAT extreme_color_rg_th_low_w;
  AR_FLOAT extreme_color_rg_th_high_w;
  AR_FLOAT extreme_color_rg_th_high_limit_w;
  AR_FLOAT extreme_color_bg_th_low_limit_w;
  AR_FLOAT extreme_color_bg_th_low_w;
  AR_FLOAT extreme_color_bg_th_high_w;
  AR_FLOAT extreme_color_bg_th_high_limit_w;
}STRU_EXTREAM_COLOR_PRA_T;

typedef struct
{
    AR_S32 extreme_color_en;
    AR_S32 extreme_count;
	AR_S32 inter_en;
	STRU_EXTREAM_COLOR_PRA_T extreme_color_pra[MIDDLE_TRIGGER_COUNT];
}STRU_EXTREAM_COLOR_T;

typedef struct
{
  AR_FLOAT lux_index;
  AR_FLOAT dark_th_low;
  AR_FLOAT dark_th_high;
  AR_FLOAT bright_th_low;
  AR_FLOAT bright_th_high;

  AR_FLOAT dark_th_low_w;
  AR_FLOAT dark_th_high_w;
  AR_FLOAT bright_th_low_w;
  AR_FLOAT bright_th_high_w;
}STRU_BRIGHT_DRAK_PRA_T;


typedef struct
{
    AR_S32 bright_dark_en;
    AR_S32 bright_dark_count;
	AR_S32 bright_en;
	AR_S32 dark_en;
	AR_S32 inter_en;
	STRU_BRIGHT_DRAK_PRA_T bright_dark_pra[MIDDLE_TRIGGER_COUNT];
}STRU_BRIGHT_DRAK_T;

typedef enum
{
    AEC_ALGO_TYPE_FAST_SMOOTH = 0,
    AEC_ALGO_TYPE_FAST_PLUS = 1,
    AEC_ALGO_TYPE_MAX
}ENUM_AEC_ALGO_TYPE_T;

#define AEC_FAST_PLUS_SPEED_RATIO_DEFAULT_VALUE (0.95)

typedef struct
{
    AR_S32 luma_tolerance;
    AR_S32 frame_skip;
    AR_S32 ddr_frame_skip_offset;
    AR_U32 interval_threshold[3];

    AR_FLOAT speed_ratio[3];
    AR_FLOAT speed_ratio_others;
} STRU_AEC_FAST_PLUS_PARAM_T;

typedef struct
{
    AR_S32 aec_stats_type;
	AR_U32 force_exp_forced;
	AR_FLOAT force_exp_value;
	AR_S32 preview_iso_enable;
	STRU_LUMA_TARGET_T multi_luma_target;
	STRU_SNOW_DETECT_T snow_scene_detect;
	STRU_BACKLIT_DETECT_T backlit_scene_detect;
	STRU_AEC_MOTION_DETECT_T aec_motion_iso_preview;
	AR_U32 R_WEIGHT;
	AR_U32 G_WEIGHT;
	AR_U32 B_WEIGHT;
	AR_U32 WT_Q;
	STRU_FAST_CONV_T fast_conv;
	AR_S32 metering_type;
    AR_U32 full_sweep_en;
    AR_FLOAT exposure_index_adj_step;
	AR_U32 antibanding;
	AR_FLOAT bias_table[BLOCK_ROW*BLOCK_COL]; //36*16=572
	STRU_HIST_TARGET_T hist_target;
    STRU_OVER_EXP_T over_exp_target;
	STRU_HDR_TUNING_EXP_T hdr_exp_table;
	STRU_NIGHT_DETECT_T night_detect;
	STRU_MOTION_DETECT_T motion_detect;
    STRU_FACE_AEC_PRA_T  face_ae;
	STRU_EXTREAM_COLOR_T extreme_color;
	STRU_BRIGHT_DRAK_T bright_dark;
    AR_U32 aec_algo_type;
    STRU_AEC_FAST_PLUS_PARAM_T aec_fast_plus_param;
    AR_U32 aec_hdr_algo_type;
	STRU_FAST_CONV_T fast_conv_short;
	STRU_BRIGHT_DRAK_T bright_dark_short;
	AR_S32 enable_sub_sample;
	AR_S32 w_ration;
	AR_S32 h_ration;	
	AR_FLOAT aec_weight_max;  /**<aec 各种权重的最大值，用来做权重的强度调整，默认20*/
	STRU_BRIGHT_DRAK_PRA_T black_light_compenstate;
	STRU_BRIGHT_DRAK_PRA_T strong_light_suppression;
}STRU_AEC_TUNING_PRA;

typedef struct
{
 AR_U32 version;
 AR_U32 enable;
 AR_U32 start_exp_index;
 AR_U32 start_skip_count;
 AR_S32 torlerence;//defaout as 2
 STRU_EXP_TABLE_PARA_T exp_table_para;
 STRU_AEC_TUNING_PRA pra;
}STRU_AEC_TUNING_T;

//--------------------------------awb--------------------------
#define MAX_LUMA_HYBRID_DESION_W_COUNT 16
#define MAX_DISTANCE_COUNT 16
typedef enum
{
 D75=0,
 D65,
 D50,
 NOON,
 CW,
 TL84,
 A,
 H,
 CUST1,
 CUST2,
 AWB_MAX, // 10
}ENUM_AWB_LIGHT_T;
typedef enum
{
	AWB_HYBRID_D75=0,
	AWB_HYBRID_D75_D65_1,
	AWB_HYBRID_D75_D65_2,
	AWB_HYBRID_D75_D65_3,
	AWB_HYBRID_D65,
	AWB_HYBRID_D65_D50_1,
	AWB_HYBRID_D65_D50_2,
	AWB_HYBRID_D65_D50_3,
	AWB_HYBRID_D50,
	AWB_HYBRID_NOON_LINE0,
	AWB_HYBRID_NOON_LINE1,  //10
	AWB_HYBRID_NOON_LINE2,
	AWB_HYBRID_NOON_LINE3,
	AWB_HYBRID_NOON_LINE4,
    AWB_HYBRID_D50_F1,
    AWB_HYBRID_D50_F2,
    AWB_HYBRID_D50_F3,
    AWB_HYBRID_F,
	AWB_HYBRID_CW,
	AWB_HYBRID_CW_TL841,
	AWB_HYBRID_CW_TL842, // 20
	AWB_HYBRID_CW_TL843,
	AWB_HYBRID_TL84,
	AWB_HYBRID_F_A1,
	AWB_HYBRID_F_A2,
	AWB_HYBRID_F_A3,
    AWB_HYBRID_A,
    AWB_HYBRID_A_H1,
	AWB_HYBRID_A_H2,
	AWB_HYBRID_A_H3,
    AWB_HYBRID_H, //30
    AWB_HYBRID_CUST1,
    AWB_HYBRID_CUST2,
    AWB_HYBRID_MAX //33
}ENUM_AWB_HYBRID_T;

typedef struct
{
  AR_FLOAT exp_index;
  AR_FLOAT hybrid_decision_w[AWB_HYBRID_MAX];
}STRU_HYBRID_DECISION_W_T;
typedef struct{
  AR_FLOAT rg;
  AR_FLOAT bg;
}STRU_AWB_POINT;

typedef struct{
  AR_FLOAT rg_adj;
  AR_FLOAT bg_adj;
}STRU_AWB_GAIN_ADJ;
typedef struct
{
	AR_S32 index;
  	AR_S32 D75_weight;
	AR_S32 D65_weight;
	AR_S32 D50_weight;
	AR_S32 NOON_weight;
	AR_S32 CW_weight;
	AR_S32 TL84_weight;
	AR_S32	A_weight;
	AR_S32	H_weight;
	AR_S32	custom1_weight;
	AR_S32	custom2_weight;
} STRU_AWB_LIGHT_WEIGHT_TABLE_T;

typedef struct
{
	AR_S32	index0_weight;
	AR_S32	outdoor_weight;
	AR_S32	inoutdoor_weight;
	AR_S32	indoor_weight;
} STRU_AWB_LIGHT_WEIGHT_TABLE2_T;

typedef struct
{
	AR_S32   lux_index;
  	AR_FLOAT green_rg_offset_adj;
  	AR_FLOAT green_bg_offset_adj;
	AR_FLOAT outlier_dist_adj;
	AR_FLOAT ref_point_rg_offset_adjust[AWB_MAX];
	AR_FLOAT ref_point_bg_offset_adjust[AWB_MAX];
} STRU_AWB_LOWLIGHT_LUT_T;

typedef struct
{
   AR_FLOAT lux_index;
   AR_FLOAT weight;
}STRU_STATS_FILTER_T;

typedef struct
{
  AR_FLOAT lux_index;
  AR_FLOAT outline_sub_decison[AWB_HYBRID_MAX];
  AR_FLOAT outline_main_decison[AWB_MAX];
}STRU_OUTLINE_ARRAY_T;

typedef struct
{
   AR_S32 count;
   STRU_OUTLINE_ARRAY_T outline_desion[16];
}STRU_AWB_OUTLINE_T;


typedef struct
{
	AR_FLOAT lux_indx;
	AR_FLOAT awb_mesh_w[36][64];
}STRU_AWB_MESH_W_T;

typedef struct
{
  AR_S32 enable;
  AR_S32 count;
  STRU_AWB_MESH_W_T awb_mesh_w[8];
}STRU_AWB_MESH_POSITION_W_TABLE_T;

typedef struct
{
	AR_S32 outdoor_midpoint;
	AR_S32 outdoor_index;
	AR_S32 inoutdoor_midpoint;
	AR_S32 indoor_index;
	AR_S32 exposure_adjustment;
	AR_FLOAT awb_mesh_stats_percent_th;
	AR_FLOAT d50_d65_weighted_sample_boundary;
	AR_FLOAT blue_sky_pec;
	AR_FLOAT blue_sky_pec_buffer;
	AR_S32 num_of_reference_point;
	STRU_AWB_OUTLINE_T outline;
	AR_FLOAT white_stat_y_threshold_low;
	AR_FLOAT white_stat_y_threshold_high;
	AR_FLOAT dominant_cluster_threshold;
	AR_FLOAT white_stat_cnt_th;
	AR_FLOAT grey_weight_day;
	AR_FLOAT white_weight_day;
	AR_FLOAT grey_weight_f;
	AR_FLOAT white_weight_f;
	AR_FLOAT grey_weight_h;
	AR_FLOAT white_weight_h;
	AR_FLOAT all_outlier_heuristic_flag;
    AR_S32 special_bayes_en;
	AR_FLOAT green_threshold;
	AR_FLOAT threshold_extreme_b_percent;
	AR_FLOAT compact_to_grey_dis;
	AR_FLOAT cluster_high_pec;
	AR_FLOAT cluster_mid_pec;
	AR_FLOAT cluster_low_pec;
	AR_S32 BitsSum;
	AR_S32 FrameNum;
	AR_S32 enable_stats_filter;
	AR_S32 stats_filter_count;
	STRU_STATS_FILTER_T input_stats_filter[5];
	AR_S32 enable_gain_filter;
	AR_S32 awb_gain_filter_count;
	STRU_STATS_FILTER_T awb_gain_filter_weight[5];
	AR_S32 enalbe_limit;
	AR_FLOAT rg_limit_max;
    AR_FLOAT rg_limit_min;
    AR_FLOAT bg_limit_max;
    AR_FLOAT bg_limit_min;
    AR_S32 default_cct;
	AR_S32 stats_filter_skip;
	AR_S32 gain_filter_skip;
	STRU_AWB_MESH_POSITION_W_TABLE_T positon_w_table;
} STRU_AWB_TUNING_PARA;
typedef struct{
	AR_U32 coef_y_r;   /* 0x64806c08 */
	AR_U32 coef_y_g;
	AR_U32 coef_y_b;
	AR_U32 coef_cb_r;
	AR_U32 coef_cb_g;
	AR_U32 coef_cb_b;
	AR_U32 coef_cr_r;
	AR_U32 coef_cr_g;
	AR_U32 coef_cr_b;
	AR_U32 coef_rsh;
	AR_U32 rrBL;
	AR_U32 grBL;
	AR_U32 gbBL;
	AR_U32 bbBL;
	AR_U32 rrGain;
	AR_U32 grGain;
	AR_U32 gbGain;
	AR_U32 bbGain;   /* 0x64806c8c */
	AR_U32 Ymax;		 /* 0x64806c94 */
	AR_U32 Ymin;
	AR_U32 Gray_m1;
	AR_U32 Gray_m2;
	AR_U32 Gray_m3;
	AR_U32 Gray_m4;
	AR_U32 Gray_c1;
	AR_U32 Gray_c2;
	AR_U32 Gray_c3;
	AR_U32 Gray_c4;
	AR_U32 Green_BGmax;
	AR_U32 Green_BGmin;
	AR_U32 Green_RGmax;
	AR_U32 Green_RGmin;
	AR_U32 Green_Rmul;
	AR_U32 Green_Bmul;
	AR_U32 ExtremeB_BGmax;
	AR_U32 ExtremeB_RGmax;
	AR_U32 ExtremeR_RGmax;
	AR_U32 ExtremeR_BGmax;  /* 0x64806ce0 */
    AR_S32 zoom;
    AR_U32 hdr_rrBL;
	AR_U32 hdr_grBL;
	AR_U32 hdr_gbBL;
	AR_U32 hdr_bbBL;
	AR_U32 hdr_rrGain;
	AR_U32 hdr_grGain;
	AR_U32 hdr_gbGain;
	AR_U32 hdr_bbGain;   /* 0x64806c8c */
}STRU_AWB_STATISTICS;

typedef struct
{
    AR_S32 count;
    AR_FLOAT dist_percent[MAX_DISTANCE_COUNT];
	AR_FLOAT distance_w[MAX_DISTANCE_COUNT]; //outline /16
}STRU_DIST_W_T;

typedef struct
{
    AR_S32 enable;
    AR_FLOAT dark_low_th;
	AR_FLOAT dark_low_th_w;
	AR_FLOAT dark_high_th;
	AR_FLOAT dark_high_th_w;
    AR_FLOAT bright_low_th;
	AR_FLOAT bright_low_th_w;
	AR_FLOAT bright_high_th;
	AR_FLOAT bright_high_th_w;
}STRU_MESH_LUMA_W_T;

typedef struct
{
  AR_S32 luma_count;
  STRU_DIST_W_T distance_w; //outline /16
  STRU_HYBRID_DECISION_W_T  hybrid_decision_w[MAX_LUMA_HYBRID_DESION_W_COUNT];
  STRU_MESH_LUMA_W_T mesh_luma;
  AR_S32 enable_no_grey_keep_settle_decison;
  AR_S32 enable_keep_no_grey_grey;
  AR_FLOAT keep_th_no_grey_to_grey; //if last grey==0, then if next grey must > keep_th_no_grey_to_grey*sgw, the grey can be used
  AR_FLOAT keep_th_grey_to_no_grey; //last grey!=0,then if next grey must < keep_th_no_grey_to_grey*sgw, the no grey can be used
  AR_S32 enable_subsample;
  AR_S32 w_ration;
  AR_S32 h_ration;
}STRU_AWB_ADVANCE_WEIGHT_AVERAGE_T;
typedef struct
{
  AR_FLOAT x;
  AR_FLOAT y;
}STRU_AWB_MESH_POINT;

typedef struct
{
   AR_S32 point_num;
   AR_S32 is_square;
   AR_FLOAT min_x;
   AR_FLOAT min_y;
   AR_FLOAT max_x;
   AR_FLOAT max_y;
   STRU_AWB_MESH_POINT point_vector[8];
   STRU_AWB_POINT ref_point;
   STRU_AWB_POINT center_point;
}STRU_AWB_MESH_REGION;

typedef struct
{
    AR_S32 lux_index;
	STRU_AWB_MESH_REGION greeen_region;
	STRU_AWB_MESH_REGION greeen_region_d;
    STRU_AWB_MESH_REGION greeen_region_t;
    STRU_AWB_MESH_REGION greeen_region_a;
}STRU_AWB_MESH_GREEN_REGION;

typedef struct
{
    AR_S32 enable;
	AR_S32 luma_count;
	AR_S32 enalbe_dta_light_inter;//if enable ,the green averae distance to center will be used to inter decision to ref point
	AR_FLOAT green_percent_th;
	STRU_AWB_MESH_GREEN_REGION awb_green_region[MAX_TRIGGER_COUNT];
}STRU_AWB_GREEN_REGION_TUNING_T;

typedef struct
{
    AR_S32 lux_index;
	AR_S32 misleading_region_count;
	STRU_AWB_MESH_REGION misleading_region[8];
}STRU_AWB_MISLEADING_REGION;

typedef struct
{
  AR_S32 enable;
  AR_S32 aec_count;
  STRU_AWB_MISLEADING_REGION misleading_region[MAX_TRIGGER_COUNT];
}STRU_AWB_MISLEADING_T;

typedef struct
{
   AR_S32 enable;
   AR_FLOAT th;
   AR_FLOAT percent_th;
   STRU_AWB_POINT ref_point;
}STRU_AWB_EXTREAM_COLOR_RED;


typedef struct
{
   AR_S32 enable;
   AR_FLOAT th;
   AR_FLOAT percent_th;
   STRU_AWB_POINT ref_point;
}STRU_AWB_EXTREAM_COLOR_BLUE;
typedef struct
{
	AR_S32 lux_index;
	STRU_AWB_GAIN_ADJ gain_adj[AWB_MAX];
}STRU_AWB_GAIN_ADJUST_WB_T;

typedef struct
{
  AR_S32 count;
  STRU_AWB_GAIN_ADJUST_WB_T gain_adj[HIGH_TRIGGER_COUNT];
}STRU_AWB_GAIN_ADJUST_LUT_WB_T;

typedef struct
{
  AR_S32 enable;
  AR_S32 lux_index;
  AR_S32 skip_frames; //every skip_frames, a scan awb clc be run
  AR_S32 frames_run_every_scan;
}STRU_AWB_SACAN_PRA_T;

typedef struct
{
   AR_S32 enable_awb_scan_mode;
   AR_S32 frames_start_scan;
   AR_S32 count;
   STRU_AWB_SACAN_PRA_T scan_pra[MIDDLE_TRIGGER_COUNT];
}STRU_AWB_SACAN_MODE_T;


typedef  struct
{
      AR_S32 version;
      AR_S32 enable;
      AR_S32 interpolation_enable;
      AR_S32 num_point;
      AR_S32 count_ae;
      AR_S32 trigger_mode;
      STRU_AEC_TRIGGER_T aec_trigger[MIN_TRIGGER_COUNT];
      STRU_AWB_POINT point[AWB_MAX];
      STRU_AWB_GAIN_ADJ gain_adj[AWB_MAX];
      AR_S32 awb_cct[AWB_MAX];
      AR_FLOAT distance[AWB_MAX];
      AR_S32 index_name[AWB_MAX];
      STRU_AWB_LIGHT_WEIGHT_TABLE_T light_weight_table[AWB_LIGHT_WEIGHT_COUNT];
      STRU_AWB_LIGHT_WEIGHT_TABLE2_T light_weight_table2[AWB_LIGHT_WEIGHT_COUNT2];
      STRU_AWB_LOWLIGHT_LUT_T awb_lowlight_lut[AWB_LOWLIGHT_LUT_COUNT];
      STRU_AWB_TUNING_PARA para;
      STRU_AWB_STATISTICS stats_awb[MIDDLE_TRIGGER_COUNT];
	  AR_S32 enable_distance_v1;
	  AR_S32 enable_awb_advace_average;
	  STRU_AWB_ADVANCE_WEIGHT_AVERAGE_T advance_avg;
	  AR_S32 enable_gain_adjust;
	  AR_S32 enable_lowlight_lut;
	  AR_S32 lowlight_lut_count;
	  AR_S32 force_simple_grey_word;
	  AR_S32 enable_init_point;
	  AR_S32 use_init_point_count;
      STRU_AWB_POINT init_point;
	  STRU_AWB_EXTREAM_COLOR_BLUE  extrem_color_blue;
      STRU_AWB_EXTREAM_COLOR_RED   extrem_color_red;
      STRU_AWB_MISLEADING_T   misleading;
      STRU_AWB_GREEN_REGION_TUNING_T green_region;
	  AR_S32 enable_gain_adj_lux_wb;
	  STRU_AWB_GAIN_ADJUST_LUT_WB_T gain_adj_lux_wb;
	  STRU_AWB_SACAN_MODE_T awb_scan_mode;
}STRU_AWB_TUNING_T;
//------------------------------af-------------------------------

typedef struct
{
  AR_U32 move_lens_steps;
  AR_U32 damp_value0;
  AR_U32 damp_value1;
}STRU_DAMPING_T;

typedef struct
{
  AR_S32 enable;
  AR_S32 damping_enable;
  AR_S32 init_dac;
  AR_S32 infinit_dac;
  AR_S32 hyperfocal_dac;
  AR_S32 near_dac;
  AR_S32 dmp_count;
  STRU_DAMPING_T move_to_far_damping[16];
  STRU_DAMPING_T move_to_near_damping[16];
}STRU_ACTUATOR_TUNNING_PRA_T;

typedef enum
{
 FOCUS_FULL_SWEEP,
 FOCUS_SINGLE,
}ENUM_FOCUS_ALGO_TYPE_T;

typedef struct
{
  AR_U32 code_per_step;
  AR_U32 skip_count_after_lens_move;

}STRU_FULLSWEEP_ALGO_TUNING_T;

typedef enum
{
  AF_STATS_SHARPNES=1<<0,
  AF_STATS_FIR=1<<1,
  AF_STATS_IIR=1<<2,
}ENUM_AF_STATS_T;

typedef struct
{
 AR_FLOAT x;
 AR_FLOAT y;
 AR_FLOAT width;
 AR_FLOAT height;
}STRU_ROI_T;

/** ENUM_AF_RUN_T:
 *
 * Enum to distinguish if it's camcorder or camera mode.
 *
 **/
typedef enum {
  AF_RUN_MODE_INIT,
  AF_RUN_MODE_CAMERA,
  AF_RUN_MODE_VIDEO,
  AF_RUN_MODE_SNAPSHOT
}ENUM_AF_RUN_T;

/** af_roi_type:
 *
 * Enum to indicate what type of ROI information we have received.
 *
 **/
typedef enum {
  AF_ROI_TYPE_GENERAL = 0x0, /* Default */
  AF_ROI_TYPE_FACE,          /* Face priority AF */
  AF_ROI_TYPE_TOUCH,         /* Touch-AF */
}ENUM_AF_ROI_T;

typedef enum {
  ACT_TYPE_CLOSELOOP,
  ACT_TYPE_OPENLOOP
}ENUM_ACT_TYPE_T;


//***************************************************************
//Head File:  Tuning Related
//******************************* *******************************
//                            for Scene detection
//---------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////

/** _af_tuning_sad: AF tuning parameters specific to Sum of
 *  Absolute Difference (SAD) scene detection.
 *  @enable: enable/disable sad scene detection
 *  @gain_min: minimum gain
 *  @gain_max: maximum gain
 *  @ref_gain_min: minimum referece gain
 *  @ref_gain_max: maximum reference gain
 *  @threshold_min: threshold when current gain is less than minimum gain
 *  @threshold_max: threshold when current gain is more than maximum gain.
 *  @ref_threshold_min: threshold when current gain is less than  minimum reference gain
 *  @ref_threshold_max: threshold when current gain is more than maximum reference gain
 */
typedef struct
{
 AR_S32    enable;
 AR_FLOAT gain_min;
 AR_FLOAT gain_max;
 AR_FLOAT ref_gain_min;
 AR_FLOAT ref_gain_max;
 AR_U32  threshold_min;
 AR_U32  threshold_max;
 AR_U32  ref_threshold_min;
 AR_U32  ref_threshold_max;
 AR_U32  frames_to_wait;
}STRU_AF_TUNING_SAD_T;

typedef struct
{
 AR_S32 far_zone;
 AR_S32 mid_zone;
 AR_S32 near_zone;

 AR_S32 init_pos;
 AR_S32 far_start_pos;
 AR_S32 near_start_pos;

 AR_S32 TAF_far_end;
 AR_S32 TAF_near_end;

 AR_S32 CAF_far_end;
 AR_S32 CAF_near_end;

 AR_S32 srch_rgn_1;
 AR_S32 srch_rgn_2;
 AR_S32 srch_rgn_3;

 AR_S32 fine_srch_rgn;
}STRU_SINGLE_OPTIC_T;

typedef struct
{
  AR_FLOAT thres[10];
}STRU_THRES_T;

typedef struct
{
  AR_U32  hist_dec_dec_thres;
  AR_FLOAT    drop_thres;   //fratio threshold
  STRU_THRES_T  dec_dec_3frame;
  STRU_THRES_T  inc_dec;
  STRU_THRES_T  inc_dec_3frame;
  STRU_THRES_T  dec_dec;
  STRU_THRES_T  dec_dec_noise;
  STRU_THRES_T  flat_threshold;

  AR_U32 hist_inc_dec_thres;

  AR_FLOAT    flat_dec_thres;
  AR_FLOAT    flat_inc_thres;

  AR_FLOAT    macro_thres;

}STRU_SINGLE_THRESHOLD_T;

typedef struct
{
  AR_S32   rgn_0;
  AR_S32   rgn_1;
  AR_S32   rgn_2;
  AR_S32   rgn_3;
  AR_S32   rgn_4;
}STRU_STEP_RGN_T;

typedef struct
{
  STRU_STEP_RGN_T   Prescan_low_light;
  STRU_STEP_RGN_T   Prescan_normal_light;
  STRU_STEP_RGN_T   Finescan_low_light;
  STRU_STEP_RGN_T   Finescan_normal_light;
}STRU_STEP_SIZE_TABLE_T;

typedef struct
{
 STRU_AF_TUNING_SAD_T af_par_sad;
}STRU_AF_MONIT_T;

typedef struct
{
  STRU_SINGLE_OPTIC_T optics;
  AR_S32 actuator_type;
  AR_S32 index[50];
  AR_S32  is_hys_comp_needed;
  STRU_SINGLE_THRESHOLD_T hw;
  AR_FLOAT BV_gain[10];
  unsigned short step_index_per_um;
  STRU_STEP_SIZE_TABLE_T CAF_step_table;
  STRU_STEP_SIZE_TABLE_T TAF_step_table;
  AR_S32 skip_frame[3];//0-large, 1-small, 2-others
} STRU_AF_TUNING_SINGLE_T;

#define AF_STATS_TABLE_SIZE (128*128/8/4/2) //64*128 bit
typedef struct
{
  AR_FLOAT trigger_val;
  //8 bit
  AR_S32 coef_y_b;
  AR_S32 coef_y_gb;
  AR_S32 coef_y_gr;
  AR_S32 coef_y_r;
  //
  AR_S32 fv_mod;
  AR_S32 ch_sel;
  AR_S32 zoom_en;
  AR_S32 zoom_dwn;
  AR_S32 compander_en;
  AR_S32 lowfilter_en;
  AR_S32 high_luma_th;
  //
  AR_S32 blc_b;
  AR_S32 blc_gb;
  AR_S32 blc_r;
  AR_S32 blc_gr;
  //sharp coff
  AR_S32 sharp_th;
  AR_S32 sharp_bitshift;
  AR_S32 sharp_sqr_bitshift;
  AR_S32 sharp_filter_coff[39];
  //fir
  AR_S32 fir_th;
  AR_S32 fir_bitshift;
  AR_S32 fir_sqr_bitshift;
  AR_S32 fir_filter_coff[3];
  //iir
  AR_S32 iir1_en;
  AR_S32 iir0_en;
  AR_S32 iir_bitshift_2;
  AR_S32 iir_bitshift_1;
  AR_S32 iir_th;
  AR_S32 iir_filter_coff[10];
  AR_S32 iir_sqr_bitshift;
  //gamma
  AR_U32 gamma[AF_STATS_TABLE_SIZE];
  
  float highluma_percent;
}STRU_AF_STATS_TUNING_T;


typedef struct{
  AR_S32 version;
  AR_S32 enable;
  AR_U32 infinite;
  AR_U32 near;
  AR_U32 hyperfocal;
  AR_U32 init_skip;
  AR_S32 algo_type;
  STRU_FULLSWEEP_ALGO_TUNING_T full_sweep_tuning_pra;
  AR_S32   af_stats_type;
  STRU_ROI_T roi;
  AR_FLOAT stats_weight_table[AF_BLOCK_ROW*AF_BLOCK_COL]; //16*9=144

  STRU_ACTUATOR_TUNNING_PRA_T atcuator;
  AR_U32 roi_type;/*AF_ROI_TYPE_GENERAL*/
  AR_U32 run_mode;/*AF_RUN_MODE_VIDEO*/

  STRU_AF_MONIT_T af_monit;
  STRU_AF_TUNING_SINGLE_T  single_pra;

  //for af stats tuning pra update
  AR_S32 count_ae;
  AR_S32 trigger_mode;
  AR_S32 interpolation_enable;
  STRU_AEC_TRIGGER_T aec_trigger[MIN_TRIGGER_COUNT];
  STRU_AF_STATS_TUNING_T af_stats_tuning_pra[MIN_TRIGGER_COUNT];
}STRU_AF_TUNING_T;
//---------------------------eis tuning --------------------
typedef struct{
 AR_S32 enable;
 AR_S32 eis_enable;
 AR_S32 ldc_enable;
 AR_S32 angle_th;
}STRU_EIS_TUNING_PRA_T;
//------------------------------raw_crop-----------------------------------
typedef struct
{
 AR_S32 enable;//0 disable the module, 1 enable the module
 AR_S32 interpolation_enable;
}STRU_ISP_SUB_MODULE_CROP_TUNING_T;
//------------------------------------blc------------------------------------------
typedef struct
{
 AR_U32 blc_R;
 AR_U32 blc_B;
 AR_U32 blc_GR;
 AR_U32 blc_GB;
 AR_U32 blc_gain_R;
 AR_U32 blc_gain_B;
 AR_U32 blc_gain_GR;
 AR_U32 blc_gain_GB;
}STRU_ISP_SUB_MODULE_BLC_PARA;

typedef struct
{
 AR_S32 enable;//0 disable the module, 1 enable the module
 AR_S32 interpolation_enable;
 AR_S32 count_ae;// how much trigger count will be used
 AR_S32 trigger_mode;
 STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
 STRU_ISP_SUB_MODULE_BLC_PARA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_BLC_TUNING_T;
//----------------------------------mix hdr-------------------------------------
typedef struct
{
  AR_U32 blc_short_exp;
  AR_U32 blc_mid_exp;
  AR_U32 blc_long_exp;
  AR_S32 hdr_motion_en_sm;
  AR_S32 hdr_alpha_s_th_sm;
  AR_S32 hdr_exp_value_th1_sm;
  AR_S32 hdr_th2_th1_reverse_sm;
  AR_S32 hdr_noise_sp_sm;
  AR_S32 hdr_motion_mp_sm;
  AR_S32 hdr_motion_en_sl;
  AR_S32 hdr_alpha_s_th_sl;
  AR_S32 hdr_exp_value_th1_sl;
  AR_S32 hdr_th2_th1_reverse_sl;
  AR_S32 hdr_noise_sp_sl;
  AR_S32 hdr_motion_mp_sl;
  AR_S32 hdr_md_th4_sl;
  AR_S32 hdr_md_th9_sl;
  AR_S32 hdr_md_th16_sl;
  AR_S32 hdr_md_th25_sl;
  AR_U32 noise_profile_l[129];
  AR_U32 noise_profile_m[129];
  AR_U32 noise_profile_s[129];
  //0 fusion 1 long_exp 2 shortexp 3 middle_exp
  AR_S32 hdr_policy;
  AR_S32 enable_hdr_blc;
  AR_S32 mv_fix_th;
  AR_S32 mv_fix_value;
  AR_S32 mv_fix_en;
  AR_S32 use_long_exp_fix;
  AR_S32 use_mv_fix_value_fix;
  AR_S32 enable_sexp_denoise;
  AR_S32 gaus_coef[4];
  AR_S32 clip_mode; //0 disable 1 auto 2 from tuning adjust
  AR_FLOAT pra_deta_r;
  AR_FLOAT pra_deta_g;  
  AR_FLOAT pra_deta_b;
}STRU_ISP_SUB_MODULE_HDR_MIX_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_HDR_MIX_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_HDR_MIX_TUNING_T;
//----------------------------------compander-----------------------------------
typedef struct
{

}STRU_ISP_SUB_MODULE_COMPANDER_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 count_awb;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[NO_TRIGGER_COUNT];
  STRU_AWB_TRIGGER_T awb_trigger[NO_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_COMPANDER_PRA pra[NO_TRIGGER_COUNT][NO_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_COMPANDER_TUNING_T;
//----------------------------------DPC---------------------------------------
typedef struct
{
    AR_U32 factor;
    AR_U32 dpc_mode;
    AR_U32 f_th_d;
    AR_U32 f_th_c;
    AR_U32 f_th_d2;
    AR_U32 f_th_d_dark;
    AR_U32 f_th_c_dark;
    AR_U32 f_th_d2_dark;
    AR_U32 e_th_d;
    AR_U32 e_th_c;
    AR_U32 e_th_d2;
    AR_U32 e_th_d_dark;
    AR_U32 e_th_c_dark;
    AR_U32 e_th_d2_dark;
    AR_U32 f_ratio;
    AR_U32 avg_th1;
    AR_U32 avg_th2;
    AR_U32 avg_th3;
    AR_U32 avg_th4;
    AR_U32 avg_th5;
    AR_U32 std_wt_th1;
    AR_U32 std_wt_th2;
    AR_U32 std_wt_th3;
    AR_U32 std_wt_th4;
    AR_U32 std_wt_th5;
}STRU_ISP_SUB_MODULE_DPC_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 dpc_cal_mode;
  AR_S32 trigger_mode;
  AR_S32 dpc_pra_enable[MAX_TRIGGER_COUNT];
  STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_DPC_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_DPC_TUNING_T;
//-------------------------------cac---------------------------------------------
typedef struct
{
   	AR_S32 rx_p[20];
	AR_S32 bx_p[20];
}STRU_ISP_SUB_MODULE_CAC_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 center_mode;
  AR_FLOAT center_width;
  AR_FLOAT center_heigh;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[MIDDLE_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_CAC_PRA pra[MIDDLE_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_CAC_TUNING_T;

//-----------------------------ATA----------------------------------------

typedef struct
{
}STRU_ISP_SUB_MODULE_ATA_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[MIDDLE_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_ATA_PRA pra[MIDDLE_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_ATA_TUNING_T;
//------------------------------RNR-------------------------------------------
#define RNR_LAYER_COUNT_MAX 2
#define RNR_CHANNEL_COUMT 4
#define RNR_EDGE_COUNT 2
typedef struct
{
  AR_S32 t1;
  AR_S32 t2;
  AR_S32 t3;
}STRU_ISP_SUB_MODULE_RNR_LAY;

typedef struct
{
  AR_S32 noise_profile_region[3];
  AR_S32 noise_profile_gain[4];
}STRU_PROFILE_T;

typedef struct
{
  AR_S32 edge_v[8];
  AR_S32 edge_level[9];
}STRU_EDGE_T;

typedef struct
{
	AR_S32 denoise_layer;
    AR_S32 profile_enable;
    STRU_ISP_SUB_MODULE_RNR_LAY lay[RNR_LAYER_COUNT_MAX][RNR_CHANNEL_COUMT];
    STRU_PROFILE_T profile[RNR_CHANNEL_COUMT];
    STRU_EDGE_T edge[RNR_EDGE_COUNT];
}STRU_ISP_SUB_MODULE_RNR_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 gray_mode;
  AR_S32 count_ae;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_RNR_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_RNR_TUNING_T;

//----------------------------decompander-----------------------------------
typedef struct
{

}STRU_ISP_SUB_MODULE_DECOMPANDER_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 count_awb;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[NO_TRIGGER_COUNT];
  STRU_AWB_TRIGGER_T awb_trigger[NO_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_DECOMPANDER_PRA pra[NO_TRIGGER_COUNT][NO_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_DECOMPANDER_TUNING_T;
//------------------------------------LSC------------------------------------
typedef struct
{
    AR_FLOAT    lsc_strength;
    AR_U32 lsc_man_mode;
    AR_FLOAT lsc_table[LSC_TUNING_NUM];
}STRU_ISP_SUB_MODULE_LSC_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;
  AR_S32 count_ae;
  AR_S32 count_awb;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[LSC_TRIGGER_COUNT];
  STRU_AWB_TRIGGER_T awb_trigger[LSC_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_LSC_PRA pra[LSC_TRIGGER_COUNT][LSC_TRIGGER_COUNT];  //[aec_pos][awb_pos]
  //STRU_ISP_SUB_MODULE_LSC_PRA ae_pra[MIN_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_LSC_TUNING_T;
//-------------------------------------DRC--------------------------------
#define DRC_LUT_TUINING_SIZE 257
#define DRC_LUT_REG_SIZE 256
typedef struct
{
	AR_S32 low_lut[257];
	AR_S32 high_lut[257];
	AR_S32 saturation;
	AR_S32 fl1[3][5];
	AR_S32 fl2[3][5];
	AR_S32 fl3[3][5];
	AR_FLOAT k1;
	AR_FLOAT k2;
	AR_FLOAT k3;
}STRU_ISP_SUB_MODULE_DRC_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
    AR_S32 count_ae;
	AR_S32 trigger_mode;
    AR_S32 drc_point_size;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_DRC_PRA pra[MAX_TRIGGER_COUNT];
	AR_S32 enable_auto_gen;
}STRU_ISP_SUB_MODULE_DRC_TUNING_T;
//---------------------------------GIC---------------------------------------
typedef struct
{
  AR_S32 enable;
  AR_S32 alphfa1;
  AR_S32 alphfa_red;
  AR_S32 k_filter_power;
  AR_S32 b_value;
  AR_S32 ge_thres;
  AR_S32 g_slope;
  AR_S32 max_thres;
  AR_S32 k_thres;
  AR_S32 limit_base_green;
  AR_S32 limit_base_red;
  AR_S32 limit_end_green;
  AR_S32 limit_end_red;
  AR_S32 weight_filterg;
  AR_S32 k_diff_base;
}STRU_ISP_SUB_MODULE_GIC_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MIDDLE_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_GIC_PRA pra[MIDDLE_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_GIC_TUNING_T;
//--------------------------------cfa-----------------------------------------
typedef struct
{
    AR_S32 edge_offset0;
    AR_S32 edge_offset1;
    AR_S32 edge_offset2;
    AR_S32 edge_offset3;
    AR_S32 gfilter_mid;
    AR_S32 gfilter_peak;
    AR_S32 gfilter_lrud;
    AR_S32 gfactor_1;
    AR_S32 gfactor_2;
    AR_S32 gfactor_3;
    AR_S32 gfactor_4;
    AR_S32 th1_y;
    AR_S32 th2_y;
    AR_S32 th1_ny;
    AR_S32 th2_ny;
    AR_S32 ny_correct_en;
    AR_S32 ny_weight_factor;
    AR_S32 ny_weight_factor_1minus;
    AR_S32 ahd_th;
    AR_S32 hvwt_th;
    AR_S32 flat_hvwt_factor;
    AR_S32 flat_hvwt_factor_1minus;
    AR_S32 detail_same_factor;
    AR_S32 detail_same_factor_1minus;
    AR_S32 detail_diff_factor;
    AR_S32 detail_diff_factor_1minus;
    AR_S32 ny_same_factor;
    AR_S32 ny_same_factor_1minus;
    AR_S32 ny_diff_factor;
    AR_S32 ny_diff_factor_1minus;
	AR_S32 dpp_correct;
	AR_S32 luma_detect_en;
	AR_S32 th1_very_light;
	AR_S32 th2_y_max;
	AR_S32 rgb_diff_detect_en;
	AR_S32 th1_rgb_diff;
	AR_S32 th2_rgb_diff;
	AR_S32 edge_detect_en;
	AR_S32 th1_edge;
	AR_S32 th2_edge;
	AR_S32 dpp_strength;
}STRU_ISP_SUB_MODULE_CFA_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_CFA_PRA pra[MAX_TRIGGER_COUNT];;
}STRU_ISP_SUB_MODULE_CFA_TUNING_T;

//----------------------depurple----------------------------------------------
typedef struct
{
  AR_S32 enable;
  AR_S32 min_rgb_div;
  AR_S32 max_rgb_div;
  AR_S32 max_ratio_rgb;
  AR_S32 min_ratio_rgb;
  AR_S32 mult_low1;//min_t0
  AR_S32 mult_high1;//min_t1
  AR_S32 g_low1;//g_t0
  AR_S32 g_high1;//g_t1
  AR_S32 color_rg_ratio;
  AR_S32 color_bg_ratio;
  AR_S32 edge_min_y;
  AR_S32 th_edge_low;//t0
  AR_S32 th_edge_high;//t1
  AR_S32 ration_max_median1;//rc_h_4p
  AR_S32 ration_max_dedian2;//rc_l_4p
  AR_S32 y_max;
  AR_S32 t_verylight;
  AR_S32 rgb_diff_th1;//factor_low
  AR_S32 rgb_diff_th2;//factor_high
  AR_S32 gf3x3_eps;
  AR_S32 gf3x1_eps;
  AR_S32 strength_ac;
  AR_S32 strength_ag;
  AR_S32 rgm_coring_low;
  AR_S32 rgm_coring_high;
  AR_S32 rgm_coring_slope;
  AR_S32 bgm_coring_low;
  AR_S32 bgm_coring_high;
  AR_S32 bgm_coring_slop;
  AR_S32 ratio_y_low;//th_yavg
  AR_S32 low_light_y_low_th;//y_low
  AR_S32 low_light_y_high_th;//y_high
  AR_S32 purple_cen;
  AR_S32 mangenta_cen;
  AR_S32 purple_range_high;
  AR_S32 purple_range_low;
  AR_S32 mangenta_range_high;
  AR_S32 mangenta_range_low;
  AR_S32 rgb_gb_g_ratio_max;//saturation_high
  AR_S32 saturation_highk_th;//saturation_abs_high
  AR_S32 saturation_low_th;//saturation_abs_low
  AR_S32 max_abs_rg_bg_g_ratio_hight;//saturation_ratio_high
  AR_S32 max_abs_rg_bg_g_ratio_low;//saturation_ratio_low
  AR_S32 depurple_wr;
  AR_S32 depurple_wg;
  AR_S32 depurple_wb;
}STRU_ISP_SUB_MODULE_DEPURPLE_PRA;
typedef struct
{
  AR_S32 enable;
  AR_S32 interpolation_enable;

  AR_S32 count_ae;
  // AR_S32 count_awb;
  AR_S32 trigger_mode;
  STRU_AEC_TRIGGER_T aec_trigger[MIDDLE_TRIGGER_COUNT];
  // STRU_AWB_TRIGGER_T awb_trigger[NO_TRIGGER_COUNT];
  STRU_ISP_SUB_MODULE_DEPURPLE_PRA pra[MIDDLE_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_DEPURPLE_TUNING_PRA;
//--------------------------ccm1--------------------------------------
#define CCM_AEC_TRIGGER_COUNT 5
#define CCM_AWB_TRIGGER_COUNT 7
typedef struct
{
    AR_FLOAT rr;
	AR_FLOAT rg;
	AR_FLOAT rb;
	AR_FLOAT gr;
	AR_FLOAT gg;
	AR_FLOAT gb;
	AR_FLOAT br;
	AR_FLOAT bg;
	AR_FLOAT bb;
}STRU_ISP_SUB_MODULE_CCM1_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 count_awb;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[CCM_AEC_TRIGGER_COUNT];
	STRU_AWB_TRIGGER_T awb_trigger[CCM_AWB_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_CCM1_PRA pra[CCM_AEC_TRIGGER_COUNT][CCM_AWB_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_CCM1_TUNING_T;
//-----------------------ccm2------------------------------------
typedef struct
{
    AR_FLOAT rr;
	AR_FLOAT rg;
	AR_FLOAT rb;
	AR_FLOAT gr;
	AR_FLOAT gg;
	AR_FLOAT gb;
	AR_FLOAT br;
	AR_FLOAT bg;
	AR_FLOAT bb;

}STRU_ISP_SUB_MODULE_CCM2_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 count_awb;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[CCM_AEC_TRIGGER_COUNT];
	STRU_AWB_TRIGGER_T awb_trigger[CCM_AWB_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_CCM1_PRA pra[CCM_AEC_TRIGGER_COUNT][CCM_AWB_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_CCM2_TUNING_T;
//-----------------------gtm_1_lut-------------------------------
typedef struct
{
	AR_S32 grey_max_flag;
	AR_U32 k[257];
}STRU_ISP_SUB_MODULE_GTM1_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MIN_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_GTM1_PRA pra[MIN_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_GTM1_TUNING_T;
//-------------------gamma-----------------------------
typedef struct
{
  AR_U32 k[4096];
}STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
    AR_S32 gamma_lut_size;
	STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA pra[MAX_TRIGGER_COUNT];

	//hdr exp ration trigger
	AR_FLOAT exp_ratio_en;
	AR_S32 exp_ratio_inter_en;
	AR_S32 exp_ratio_count;
	AR_FLOAT w_of_exp_ratio[MIDDLE_TRIGGER_COUNT];
	STRU_EXP_RATION_TRIGGER_T exp_ratio_trigger[MIDDLE_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA pra_exp_raion[MIDDLE_TRIGGER_COUNT];

	//
}STRU_ISP_SUB_MODULE_GAMMA_LUT_TUNING_T;
//----------------------gtm2_lut-----------------------------------
typedef struct
{
    AR_S32 enable_cdf_smooth;
    AR_FLOAT cut_ration;//0-1 1 will have no erffect to it
	AR_FLOAT weight; //current weight
	AR_S32 ration;
	AR_S32 low_cut;//0---1024
	AR_S32 enable_free_haho;
	AR_S32 filt_x;
	AR_S32 filt_y;
	AR_FLOAT PARAM_FILT;
	AR_S32 bin_meger;
	AR_S32 ltm_smooth;
	AR_S32 ltm_dehighlight;
	AR_S32 ltm_base;
	AR_FLOAT ltm_smooth_fork;
	AR_FLOAT ltm_smooth_spear;
	AR_FLOAT ltm_smooth_gamma;
	AR_FLOAT ltm_dehighlight_fork;
	AR_FLOAT ltm_dehighlight_spear;
	AR_FLOAT ltm_dehighlight_gamma;
	AR_FLOAT ltm_dark_fork;
	AR_FLOAT ltm_dark_spear;
	AR_S32   ltm_smooth_noise_erode_area;
	AR_S32   ltm_smooth_filter_area;
	AR_S32   soft_sample_rate;
}STRU_ISP_SUB_MODULE_GTM2_LUT_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_GTM2_LUT_PRA pra[MAX_TRIGGER_COUNT];
	AR_S32 enable_tuning_mesh;
	AR_S32 mesh_w;
	AR_S32 mesh_h;
}STRU_ISP_SUB_MODULE_GTM2_LUT_TUNING_T;
//-------------------------3d_lut-----------------------------
#define LUT3D_TABLE_SIZE 1230
#define LUT3D_CHANEL_NUM 3
#define LUT3D_AEC_TRIGGER_COUNT 1
#define LUT3D_AWB_TRIGGER_COUNT 1
typedef struct
{
	AR_U32 lut3d_table0[LUT3D_TABLE_SIZE][LUT3D_CHANEL_NUM];
	AR_U32 lut3d_table1[LUT3D_TABLE_SIZE][LUT3D_CHANEL_NUM];
	AR_U32 lut3d_table2[LUT3D_TABLE_SIZE][LUT3D_CHANEL_NUM];
	AR_U32 lut3d_table3[LUT3D_TABLE_SIZE][LUT3D_CHANEL_NUM];
}STRU_ISP_SUB_MODULE_3D_LUT_PRA;

typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 count_awb;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[LUT3D_AEC_TRIGGER_COUNT];
	STRU_AWB_TRIGGER_T awb_trigger[LUT3D_AWB_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_3D_LUT_PRA pra[LUT3D_AEC_TRIGGER_COUNT][LUT3D_AWB_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_3D_LUT_TUNING_T;
//---------------------------rgb_to_yuv-------------------------------------------
typedef struct
{

}STRU_ISP_SUB_MODULE_RGB2YUV_PRA;
typedef struct
{
	AR_S32 enable;
}STRU_ISP_SUB_MODULE_RGB2YUV_TUNING_T;
//----------------------------CM---------------------------------------
#define CM_AEC_TRIGGER_COUNT 5
#define CM_AWB_TRIGGER_COUNT 7

typedef struct
{
	AR_FLOAT saturation;
	AR_FLOAT hue;
}STRU_ISP_SUB_MODULE_CM_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 count_awb;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[CM_AEC_TRIGGER_COUNT];
	STRU_AWB_TRIGGER_T awb_trigger[CM_AWB_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_CM_PRA pra[CM_AEC_TRIGGER_COUNT][CM_AWB_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_CM_TUNING_T;


//---------------------------------cm2----------------------------------------------------
typedef struct
{
	AR_FLOAT saturation;
	AR_FLOAT hue;
	AR_S32 y_lo_th1;
	AR_S32 y_lo_th2;
    AR_S32 y_ar_th1;
	AR_S32 y_ar_th2;
}STRU_ISP_SUB_MODULE_CM2_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 count_awb;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[CM_AEC_TRIGGER_COUNT];
	STRU_AWB_TRIGGER_T awb_trigger[CM_AWB_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_CM2_PRA pra[CM_AEC_TRIGGER_COUNT][CM_AWB_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_CM2_TUNING_T;
//-----------------------lee sharp ------------------------------------
typedef struct
{
  AR_S32 enable_noise_level_weight_edge;
  AR_S32 enable_skin_detection;
  AR_S32 enable_shink_operation_for_haolo_side;
  AR_S32 enable_luma_weight_function;
  AR_S32 enable_mono_mode;
  AR_S32 skin_weight;
  AR_S32  bpf[4][4];
  AR_S32 hpf[3][4];
  AR_S32 strength_pos_edge;
  AR_S32 strength_neg_edge;
  AR_S32 overshoot_pos_edge;
  AR_S32 overshoot_neg_edge;
  AR_S32 colour_u_p[4];
  AR_S32 colour_u_pdes[5];
  AR_S32 colour_u_slop[5];
  AR_S32 colour_v_p[4];
  AR_S32 colour_v_pdes[5];
  AR_S32 colour_v_slop[5];
  AR_S32 conv_3x3_ration_constant;
  AR_S32 conv_1x7_ration_constant;
  AR_S32   edge_w[64];
  AR_S32 limitation_for_move_strength;
  AR_S32 adjust_strength;
  AR_S32 coef_moving_c00;
  AR_S32 coef_moving_c01;
  AR_S32 coef_moving_c10;
  AR_S32 coef_moving_c11;
  AR_S32   shink_w[64];
  AR_S32   luma_w[64];
}STRU_ISP_SUB_MODULE_LEE_PRA;

typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_LEE_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_LEE_TUNING_T;
//----------------------cnf-------------------

typedef struct
{
	AR_S32 coef_r_y;
	AR_S32 coef_r_cb;
	AR_S32 coef_r_cr;
	AR_S32 coef_g_y;
	AR_S32 coef_g_cb;
	AR_S32 coef_g_cr;
	AR_S32 coef_b_y;
	AR_S32 coef_b_cb;
	AR_S32 coef_b_cr;
	AR_S32 y_offset;
	AR_S32 cb_offset;
	AR_S32 cr_offset;
}STRU_RGBYUV_COF_T;

typedef struct
{
	AR_S32 enable;
	AR_S32 zoom_par;
	AR_S32 dn_level;
	AR_S32 lut0[256];
	AR_S32 lut1[256];
}STRU_ISP_SUB_MODULE_CNF_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
    STRU_RGBYUV_COF_T rgbyuv_cof;
	AR_S32 lut_size;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_CNF_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_CNF_TUNING;
//---------------------------3d_2d_nr---------------------------
typedef struct
{
  AR_S32 enale_3d_noise; //if0 only bm3d is enable, 1 the temple add denoise is enable
  AR_S32 nr3d_pp_en;
  AR_S32 nr3d_md_en;
  AR_S32 nr3d_npy_th1;
  AR_S32 nr3d_npy_th2;
  AR_S32 nr3d_mpy_th1;
  AR_S32 nr3d_mpy_th2;
  AR_S32 nr3d_npc_th1;
  AR_S32 nr3d_npc_th2;
  AR_S32 nr3d_npc_th3;
  AR_S32 nr3d_npc_th4;
  AR_S32 nr3d_mpc_th1;
  AR_S32 nr3d_mpc_th2;
  AR_S32 nr3d_lamda2d;
  AR_S32 nr3d_ite;
  AR_S32 nr3d_msr;
  AR_S32 nr3d_decay;
  AR_S32 nr3d_satu;
  AR_S32 nr3d_gaus_y_c11;
  AR_S32 nr3d_gaus_y_c12;
  AR_S32 nr3d_gaus_y_c13;
  AR_S32 nr3d_gaus_y_c21;
  AR_S32 nr3d_gaus_y_c23;
  AR_S32 nr3d_tauhard_th;
  AR_S32 md_blk_ave_sel;
  AR_S32 bypass_saturation_adjust;
  AR_S32 post_process_y_filter_sel;//1: med3*3 0:3*5 gaussian
  AR_S32 pixel_weight_filter_c11;
  AR_S32 pixel_weight_filter_c12;
  AR_S32 pixel_weight_filter_c13;
  AR_S32 disable_1st_media_filter;
  AR_S32 dbk_h_en;
  AR_S32 dbk_v_en;
  AR_S32 dbk_wr2ddr;
  AR_S32 dbk_bypass;
  AR_S32 dbk_h_mv_diff;
  AR_S32 dbk_h_mv_str;
  AR_S32 dbk_v_mv_diff;
  AR_S32 dbk_v_mv_str;
  AR_S32 dbk_h_yy_flat;
  AR_S32 dbk_h_yy_diff;
  AR_S32 dbk_v_yy_flat;
  AR_S32 dbk_v_yy_diff;
  AR_S32 dbk_satu;
  AR_S32 dbk_gaus_y_c11;
  AR_S32 dbk_gaus_y_c12;
  AR_S32 dbk_gaus_y_c13;
  AR_S32 dbk_gaus_y_c21;
  AR_S32 dbk_gaus_y_c23;
  AR_S32 bypass_dbk_sturation_adjust;

  AR_S32 md_yuv_sel;
  AR_S32 md_1st_media_filter_disable;
  AR_S32 md_noise_profile_slection;
  AR_S32 md_divs_seletction;
  AR_S32 md_expand_ration;
  AR_S32 md_sort_sel;
  AR_S32 noise_profile_y[64];
  AR_S32 noise_profile_cbcr[64];
  AR_S32 dbk_h_ed_en;
  AR_S32 dbk_v_ed_en;
  AR_S32 dbk_h_ed_sel;
  AR_S32 dbk_v_ed_sel;
  AR_S32 dbk_h_ed_thrd;
  AR_S32 dbk_v_ed_thrd;
}STRU_ISP_SUB_MODULE_3D_2D_NR_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_3D_2D_NR_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_3D_2D_NR_TUNING_T;
//----------------------------dithering, add some noise to image--------------------
#define DITHER_TABLE_SIZE 256
typedef struct
{
  AR_S32 enable;
  AR_FLOAT noise_gain;
  AR_S32 light_start;
  AR_S32 light_end;
  AR_CHAR dither_talbe[DITHER_TABLE_SIZE];
}STRU_ISP_SUB_MODULE_DITHERING_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MIDDLE_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_DITHERING_PRA pra[MIDDLE_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_DITHERING_TUNING;

typedef struct
{
}STRU_ISP_SUB_MODULE_EIS_LDC_PRA;

typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MIDDLE_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_EIS_LDC_PRA pra[MIDDLE_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_EIS_LDC_TUNING_T;

typedef struct
{
	AR_S32 w;
	AR_S32 h;
	AR_S32 avg_h;
	AR_S32 avg_v;
}STRU_SCALER_ANTI_T;

typedef struct
{
 STRU_SCALER_ANTI_T snr1_scaler1[16];
 STRU_SCALER_ANTI_T snr1_scaler2[16];
 STRU_SCALER_ANTI_T snr2_scaler1[16];
 STRU_SCALER_ANTI_T snr2_scaler2[16];
}STRU_ISP_SUB_MODULE_SCALER_ANTI_PRA_T;

typedef struct
{
	AR_S32 enable;
	STRU_ISP_SUB_MODULE_SCALER_ANTI_PRA_T isp_sub_module_scaler_pra;
}STRU_ISP_SUB_MODULE_SCALER_ANTI_TUNING_T;


typedef struct
{
	char net_name[128];
	double scale;
	double k_coeff[2];
	double b_coeff[3];
	int   blc[4];
}STRU_AI_ISP_PRA_T;

typedef struct
{
    STRU_ISP_SUB_MODULE_HDR_MIX_PRA hdr_mix_pra;
	STRU_AI_ISP_PRA_T ai_isp_pra;
}STRU_ISP_SUB_MODULE_ISP_VFE_PRA;

typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[MAX_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_ISP_VFE_PRA pra[MAX_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_ISP_VFE_TUNING_T;

#define ACM_AEC_TRIGGER_COUNT 5
#define ACM_AWB_TRIGGER_COUNT 7
typedef struct
{
	int s_lut[24];
	int h_lut[24];
    int v_lut[24];
}STRU_ISP_SUB_MODULE_ACM_PRA;
typedef struct
{
	AR_S32 enable;
    AR_S32 interpolation_enable;
	AR_S32 count_ae;
	AR_S32 count_awb;
	AR_S32 trigger_mode;
	STRU_AEC_TRIGGER_T aec_trigger[ACM_AEC_TRIGGER_COUNT];
	STRU_AWB_TRIGGER_T awb_trigger[ACM_AWB_TRIGGER_COUNT];
	STRU_ISP_SUB_MODULE_ACM_PRA pra[ACM_AEC_TRIGGER_COUNT][ACM_AWB_TRIGGER_COUNT];
}STRU_ISP_SUB_MODULE_ACM_TUNING_T;

#define IR_LMS_AEC_TRIGGER_COUNT    10
typedef struct
{
    AR_U32 step_vl;
    AR_U32 th_g;
} STRU_ISP_SUB_MODULE_IR_LMS_PRA;

typedef struct
{
    AR_S32 enable;
    AR_S32 interpolation_enable;
    AR_S32 count_ae;
    AR_S32 trigger_mode;
    STRU_AEC_TRIGGER_T aec_trigger[IR_LMS_AEC_TRIGGER_COUNT];
    STRU_ISP_SUB_MODULE_IR_LMS_PRA pra[IR_LMS_AEC_TRIGGER_COUNT];
} STRU_ISP_SUB_MODULE_IR_LMS_TUNING_T;

#define IR_RAW_3DNR_SW_AEC_TRIGGER_COUNT    10
typedef struct
{
    AR_U32 diff_th[5];
    AR_FLOAT mov_val[5];
    AR_FLOAT lpf_coeff[3];
    AR_FLOAT par_decay;
    AR_FLOAT blend_coeff;
} STRU_ISP_SW_ALGO_IR_RAW_3DNR_PRA;

typedef struct
{
    AR_S32 enable;
    AR_S32 interpolation_enable;
    AR_S32 count_ae;
    AR_S32 trigger_mode;
    STRU_AEC_TRIGGER_T aec_trigger[IR_RAW_3DNR_SW_AEC_TRIGGER_COUNT];
    STRU_ISP_SW_ALGO_IR_RAW_3DNR_PRA pra[IR_RAW_3DNR_SW_AEC_TRIGGER_COUNT];
} STRU_ISP_SW_ALGO_IR_RAW_3DNR_TUNING_T;

#define IR_RAW_2DNR_SW_AEC_TRIGGER_COUNT    10
typedef struct
{
    AR_FLOAT sigma0;
    AR_FLOAT sigma1;
} STRU_ISP_SW_ALGO_IR_RAW_2DNR_PRA;

typedef struct
{
    AR_S32 enable;
    AR_S32 interpolation_enable;
    AR_S32 count_ae;
    AR_S32 trigger_mode;
    STRU_AEC_TRIGGER_T aec_trigger[IR_RAW_2DNR_SW_AEC_TRIGGER_COUNT];
    STRU_ISP_SW_ALGO_IR_RAW_2DNR_PRA pra[IR_RAW_2DNR_SW_AEC_TRIGGER_COUNT];
} STRU_ISP_SW_ALGO_IR_RAW_2DNR_TUNING_T;

#define IR_RAW_GTM_SW_AEC_TRIGGER_COUNT     10
typedef struct
{
    AR_U32 simplified;          // simplified gtm algorithm
    AR_FLOAT lpf_coeff[5];      // gaussian filter coefficients
    AR_U32 lpf_dim;             // gaussian filter dimension;
    AR_FLOAT hist_lower_th;     // unit: 1/100
    AR_FLOAT hist_upper_th;     // unit: 1/100
    AR_FLOAT alpha;
    AR_S32 mix_contrast_exp;
    AR_S32 mix_bright_exp;
    AR_S32 restrain_range_th;
    AR_S32 mix_range;
    AR_S32 mid_value;
    AR_FLOAT plat_th_percent;
    AR_S32 max_contrast;
    AR_FLOAT detail_thrd[2];
    AR_FLOAT detail_lpf_coeff[3];
    AR_FLOAT dark_edge_enhance;
    AR_FLOAT bright_edge_enhance;
    AR_S32 detail_up_limit;
    AR_S32 detail_low_limit;
} STRU_ISP_SW_ALGO_IR_RAW_GTM_PRA;

typedef struct
{
    AR_S32 enable;
    AR_S32 interpolation_enable;
    AR_S32 count_ae;
    AR_S32 trigger_mode;
    STRU_AEC_TRIGGER_T aec_trigger[IR_RAW_GTM_SW_AEC_TRIGGER_COUNT];
    STRU_ISP_SW_ALGO_IR_RAW_GTM_PRA pra[IR_RAW_GTM_SW_AEC_TRIGGER_COUNT];
} STRU_ISP_SW_ALGO_IR_RAW_GTM_TUNING_T;

typedef struct
{
 AR_FLOAT aec_trigger_tolerence;
 AR_FLOAT aec_trigger_tolerence_gain;
 AR_FLOAT awb_trigger_tolerence;
 AR_S32   tuning_pra_update_ration;
}STRU_TUNING_CTL_T;


typedef struct
{
 AR_U32 isp_version;
 STRU_ISP_SUB_MODULE_CROP_TUNING_T isp_sub_module_raw_crop_tuning;
 STRU_ISP_SUB_MODULE_BLC_TUNING_T isp_sub_module_blc_tuning;
 STRU_ISP_SUB_MODULE_HDR_MIX_TUNING_T isp_sub_module_hdr_mix_tuning;
 STRU_ISP_SUB_MODULE_COMPANDER_TUNING_T isp_sub_module_compander_tuning;
 STRU_ISP_SUB_MODULE_DPC_TUNING_T isp_sub_module_dpc_tuning;
 STRU_ISP_SUB_MODULE_CAC_TUNING_T isp_sub_module_cac_tuning;
 STRU_ISP_SUB_MODULE_ATA_TUNING_T isp_sub_module_ata_tuning;
 STRU_ISP_SUB_MODULE_RNR_TUNING_T isp_sub_module_rnr_tuning;
 STRU_ISP_SUB_MODULE_DECOMPANDER_TUNING_T isp_sub_module_decompader_tuning;
 STRU_ISP_SUB_MODULE_LSC_TUNING_T isp_sub_module_lsc_tuning;
 STRU_ISP_SUB_MODULE_DRC_TUNING_T isp_sub_module_drc_tuning;
 STRU_ISP_SUB_MODULE_GIC_TUNING_T isp_sub_module_gic_tuning;
 STRU_ISP_SUB_MODULE_CFA_TUNING_T isp_sub_module_cfa_tuning;
 STRU_ISP_SUB_MODULE_DEPURPLE_TUNING_PRA isp_sub_module_depurple_tuning;
 STRU_ISP_SUB_MODULE_CCM1_TUNING_T isp_sub_module_ccm1_tuning;
 STRU_ISP_SUB_MODULE_CCM2_TUNING_T isp_sub_module_ccm2_tuning;
 STRU_ISP_SUB_MODULE_GTM1_TUNING_T isp_sub_module_gtm1_lut_tuning;
 STRU_ISP_SUB_MODULE_GAMMA_LUT_TUNING_T isp_sub_module_gamma_lut_tuning;
 STRU_ISP_SUB_MODULE_GTM2_LUT_TUNING_T isp_sub_module_gtm2_lut_tuning;
 STRU_ISP_SUB_MODULE_3D_LUT_TUNING_T isp_sub_module_3d_lut_tuning;
 STRU_ISP_SUB_MODULE_RGB2YUV_TUNING_T isp_sub_module_rgbyuv_tuning;
 STRU_ISP_SUB_MODULE_CM_TUNING_T isp_sub_module_cm_tuning;
 STRU_ISP_SUB_MODULE_LEE_TUNING_T isp_sub_module_lee_tuning;
 STRU_ISP_SUB_MODULE_CNF_TUNING isp_sub_module_cnf_tuning;
 STRU_ISP_SUB_MODULE_3D_2D_NR_TUNING_T isp_sub_module_3d_2d_nr_tuning;
 STRU_ISP_SUB_MODULE_DITHERING_TUNING isp_sub_module_dithering_tuning;
 STRU_ISP_SUB_MODULE_ISP_VFE_TUNING_T isp_sub_module_isp_isp_vfe_tuning;
 STRU_ISP_SUB_MODULE_SCALER_ANTI_TUNING_T isp_sub_module_scaler_tuning;
 STRU_ISP_SUB_MODULE_EIS_LDC_TUNING_T isp_sub_module_eis_ldc_tuning;
 STRU_ISP_SUB_MODULE_CM2_TUNING_T isp_sub_module_cm2_tuning;
 STRU_ISP_SUB_MODULE_LSC_TUNING_T isp_sub_module_hdr_lsc_tuning;
 STRU_ISP_SUB_MODULE_ACM_TUNING_T isp_sub_module_acm_tuning;
 STRU_ISP_SUB_MODULE_IR_LMS_TUNING_T isp_sub_module_ir_lms_tuning;
 STRU_ISP_SW_ALGO_IR_RAW_3DNR_TUNING_T isp_sw_algo_ir_raw_3dnr_tuning;
 STRU_ISP_SW_ALGO_IR_RAW_2DNR_TUNING_T isp_sw_algo_ir_raw_2dnr_tuning;
 STRU_ISP_SW_ALGO_IR_RAW_GTM_TUNING_T isp_sw_algo_ir_raw_gtm_tuning;
}STRU_ISP_TUNING_T;

typedef struct
{
 STRU_ISP_SUB_MODULE_BLC_PARA isp_sub_module_blc_tuning;
 STRU_ISP_SUB_MODULE_HDR_MIX_PRA isp_sub_module_hdr_mix_tuning;
 STRU_ISP_SUB_MODULE_COMPANDER_PRA isp_sub_module_compander_tuning;
 STRU_ISP_SUB_MODULE_DPC_PRA  isp_sub_module_dpc_tuning;
 STRU_ISP_SUB_MODULE_CAC_PRA isp_sub_module_cac_tuning;
 STRU_ISP_SUB_MODULE_ATA_PRA isp_sub_module_ata_tuning;
 STRU_ISP_SUB_MODULE_RNR_PRA isp_sub_module_rnr_tuning;
 STRU_ISP_SUB_MODULE_DECOMPANDER_PRA isp_sub_module_decompader_tuning;
 STRU_ISP_SUB_MODULE_LSC_PRA isp_sub_module_lsc_tuning;
 STRU_ISP_SUB_MODULE_DRC_PRA isp_sub_module_drc_tuning;
 STRU_ISP_SUB_MODULE_GIC_PRA isp_sub_module_gic_tuning;
 STRU_ISP_SUB_MODULE_CFA_PRA isp_sub_module_cfa_tuning;
 STRU_ISP_SUB_MODULE_DEPURPLE_PRA isp_sub_module_depurple_tuning;
 STRU_ISP_SUB_MODULE_CCM1_PRA isp_sub_module_ccm1_tuning;
 STRU_ISP_SUB_MODULE_CCM2_PRA isp_sub_module_ccm2_tuning;
 STRU_ISP_SUB_MODULE_GTM1_PRA isp_sub_module_gtm1_lut_tuning;
 STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA isp_sub_module_gamma_lut_tuning;
 STRU_ISP_SUB_MODULE_GTM2_LUT_PRA isp_sub_module_gtm2_lut_tuning;
 STRU_ISP_SUB_MODULE_3D_LUT_PRA isp_sub_module_3d_lut_tuning;
 STRU_ISP_SUB_MODULE_RGB2YUV_PRA isp_sub_module_rgbyuv_tuning;
 STRU_ISP_SUB_MODULE_CM_PRA isp_sub_module_cm_tuning;
 STRU_ISP_SUB_MODULE_LEE_PRA isp_sub_module_lee_tuning;
 STRU_ISP_SUB_MODULE_CNF_PRA isp_sub_module_cnf_tuning;
 STRU_ISP_SUB_MODULE_3D_2D_NR_PRA isp_sub_module_3d_2d_nr_tuning;
 STRU_ISP_SUB_MODULE_DITHERING_PRA isp_sub_module_dithering_tuning;
 STRU_AWB_STATISTICS               awb_stats_tuning;
 STRU_ISP_SUB_MODULE_ISP_VFE_PRA  isp_sub_module_isp_isp_vfe_tuning;
 STRU_ISP_SUB_MODULE_SCALER_ANTI_PRA_T  isp_sub_module_scaler_tuning;
 STRU_AF_STATS_TUNING_T            af_stats_tuning;
 STRU_ISP_SUB_MODULE_EIS_LDC_PRA  isp_sub_module_eis_ldc_tuning;
 STRU_ISP_SUB_MODULE_CM2_PRA isp_sub_module_cm2_tuning;
 STRU_ISP_SUB_MODULE_LSC_PRA isp_sub_module_hdr_lsc_tuning;
 STRU_ISP_SUB_MODULE_ACM_PRA isp_sub_module_acm_tuning;
 STRU_ISP_SUB_MODULE_IR_LMS_PRA isp_sub_module_ir_lms_tuning;
 STRU_ISP_SW_ALGO_IR_RAW_3DNR_PRA isp_sw_algo_ir_raw_3dnr_tuning;
 STRU_ISP_SW_ALGO_IR_RAW_2DNR_PRA isp_sw_algo_ir_raw_2dnr_tuning;
 STRU_ISP_SW_ALGO_IR_RAW_GTM_PRA isp_sw_algo_ir_raw_gtm_tuning;
 STRU_AEC_TUNING_T aec_tuning;
}STRU_ISP_TUNING_PRA_T;


typedef struct
{
	int request_type;
	int moudle;
	int disable_update;
	int only_force_update;
	int update_src;
	int force_update;
	int trigger_from;

	//when trigger source from user but not aec or awb
	int aec_up_index;
	int aec_down_index;
	float aec_weight_down;
	int awb_up_index;
	int awb_down_index;
	float awb_weight_down;
}STRU_TUNING_REQ_UPDATE_PROP_T;

typedef struct
{
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_blc_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_hdr_mix_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_compander_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T  isp_sub_module_dpc_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_cac_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_ata_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_rnr_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_decompader_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_lsc_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_drc_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_gic_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_cfa_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_depurple_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_ccm1_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_ccm2_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_gtm1_lut_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_gamma_lut_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_gtm2_lut_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_3d_lut_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_rgbyuv_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_cm_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_lee_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_cnf_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_3d_2d_nr_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_dithering_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T awb_stats_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_ceva_vfe_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_scaler_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T af_stats_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_eis_ldc_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_cm2_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_hdr_lsc_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_acm_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sub_module_ir_lms_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sw_algo_ir_raw_3dnr_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sw_algo_ir_raw_2dnr_prop;
 STRU_TUNING_REQ_UPDATE_PROP_T isp_sw_algo_ir_raw_gtm_prop;
} STRU_TUNING_UPDATE_PROP_T;


typedef struct
{
 AR_U32 enable;
}STRU_RRO_CONFIG_T;
typedef struct
{
 AR_S32 enable;
}STRU_RGB_MAX_CONFIG_T;
typedef struct
{
 AR_S32 enable;
}STRU_RGB_HISTO_CONFIG_T;
typedef struct
{
 AR_S32 enable;
}STRU_RAW_HISTO_CONFIG_T;

typedef struct
{
 AR_S32 enable;
}STRU_RCS_HISTO_CONFIG_T;

typedef struct
{
 AR_S32 enable;
}STRU_AWBS_HISTO_CONFIG_T;

typedef struct
{
 AR_S32 enable;
}STRU_AF_HISTO_CONFIG_T;

typedef struct
{
 STRU_RRO_CONFIG_T rro_config;
 STRU_RGB_MAX_CONFIG_T rgb_max_config;
 STRU_RGB_HISTO_CONFIG_T rgb_histo_config;
 STRU_RAW_HISTO_CONFIG_T raw_histo_config;
 STRU_RCS_HISTO_CONFIG_T rcs_config;
 STRU_AWBS_HISTO_CONFIG_T awbs_config;
 STRU_AF_HISTO_CONFIG_T af_config;
}STRU_STATS_CONFIG_T;
typedef struct
{
 AR_U32 header_version;
 STRU_TUNING_CTL_T tuning_ctl;
 STRU_ISP_TUNING_T isp_tuning;
 STRU_STATS_CONFIG_T stats_config;
 STRU_AEC_TUNING_T aec_tuning;
 STRU_AWB_TUNING_T awb_tuning;
 STRU_AF_TUNING_T  af_tuning;
 STRU_EIS_TUNING_PRA_T eis_tuning;
}STRU_ALGO_ISP_TUNING_T;

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

