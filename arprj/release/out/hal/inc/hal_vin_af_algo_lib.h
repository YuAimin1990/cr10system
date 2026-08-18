/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VI
 *  @{
 */

/**
 * @file hal_vin_af_algo_lib.h
 * @brief 定义vin的af 模块的数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_VIN_AF_ALGO_INTERFACE_DEF_H__
#define __HAL_VIN_AF_ALGO_INTERFACE_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

///////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "hal_vin_type_def.h"

///////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

typedef enum
{
    AF_CTL_SET_AF_MODE,
    AF_CTL_SET_MANUAL_AF,
    AF_CTL_EVENT_AEC_UPDATE,
    AF_CTL_GET_3A_INFO,
    AF_CTL_GET_BASIC_3A_INFO,
    AF_CTL_TRIGGER_FOCUS,
    AF_CTL_SET_FOCUS_ROI,
    AF_CTL_SET_TUNINIG_REQUEST,
    AF_CTL_HW_SET_IN_OUT_FORMAT,
    AF_CTL_HW_SET_START,
    AF_CTL_HW_SET_STOP,
    AF_CTL_GET_LIMIT_INFO,
    AF_CTL_UPDATE,
    AF_CTL_MAX
}ENUM_AF_CTL_T;

typedef enum
{
   MOVE_NEAR,
   MOVE_FAR,
   MOVE_FIX,
}ENUM_MOVE_T;

///////////////////////////////////////////////////////////////////////////////////

// //AF_CTL_SET_AF_MODE
typedef struct
{
    AR_S32 mode;
} STRU_AF_CTL_SET_AF_MODE_INFO_T;

// //AF_CTL_SET_MANUAL_AF
typedef struct
{
    AR_S32 pos;
} STRU_AF_CTL_SET_MANUAL_AF_INFO_T;

//AF_CTL_EVENT_AEC_UPDATE
typedef struct
{
    AR_S32 aec_settled;
    AR_FLOAT lux_idx;
    AR_FLOAT cur_luma;
    AR_FLOAT cur_real_gain;
    AR_FLOAT exp_time_us;
} STRU_AF_CTL_AEC_UPDATE_INFO_T;

//AF_CTL_SET_FOCUS_ROI
typedef struct
{
    AR_FLOAT x;
    AR_FLOAT y;
    AR_FLOAT width;
    AR_FLOAT height;
} STRU_AF_CTL_SET_FOUCS_ROI_INFO_T;

//AF_CTL_HW_SET_IN_OUT_FORMAT
typedef struct
{
    AR_S32 x_offset;
    AR_S32 y_offset;
    AR_S32 image_width;
    AR_S32 image_height;
    AR_S32 block_width;
    AR_S32 block_height;
    AR_S32 stats_height;
    AR_S32 stats_width;
} STRU_AF_CTL_HW_IN_OUT_INFO_T;

//AF_CTL_HW_SET_IN_OUT_FORMAT
typedef struct
{
    AR_S32 cam_index;
} STRU_AF_CTL_HW_START_INFO_T;

///////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    STRU_AF_ALGO_LIB_AF_STATUS_DATA_T af_stats_data;
} STRU_AF_ALGO_LIB_INPUT_T;

typedef struct
{
    AR_S32 camera_id;
    AR_U32 lens_position;
	AR_S32 fv;
	ENUM_MOVE_T dir;
    AR_S32 move_step;
    AR_S32 b_move_lens;
    STRU_AF_CTL_SET_FOUCS_ROI_INFO_T roi;
} STRU_AF_ALGO_LIB_OUTPUT_T;


typedef enum
{
   AF_CB_EVENT_INIT,
   AF_CB_EVENT_EXIT,
   AF_CB_EVENT_RUN,
   AF_CB_EVENT_CTL,
   AF_CB_EVENT_MAX,
}ENUM_AF_CB_EVENT_T;

typedef struct
{
   AR_S32 camera_id;
}STRU_AF_CB_EVENT_INIT_PRA_T;

typedef struct
{
   AR_S32 camera_id;
}STRU_AF_CB_EVENT_EXIT_PRA_T;

typedef struct
{
   AR_S32 camera_id;
   STRU_AF_ALGO_LIB_INPUT_T *p_af_algo_lib_input;
   STRU_AF_ALGO_LIB_OUTPUT_T *p_af_algo_lib_output;
}STRU_AF_CB_EVENT_RUN_PRA_T;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 clt_code;
   AR_VOID *pra;
   AR_S32 ctl_size;
}STRU_AF_CB_EVENT_CTL_PRA_T;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // __AF_CORE_INTERFACE_DEF_H__

