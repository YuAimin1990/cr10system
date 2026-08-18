/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VI
 *  @{
 */

/**
 * @file hal_vin_type_def.h
 * @brief 定义vin的基础数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_VIN_TYPE_DEF__H__
#define __HAL_VIN_TYPE_DEF__H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <pthread.h>
#include <stdint.h>
#include "hal_type.h"
#include "hal_vin_tuning_def.h"
#include "hal_errno.h"
#include "hal_comm.h"

#define AF_STAT_WIDTH 16
#define AF_STAT_HEIGHT 9


#define CAM_ALIGNE_TO(size,num) ( (num)*( ( (size)+(num)-1)/(num) ) )

#define  DEFAULT_DDR_RW_BURST_LEN 16
#define  Y_UV_DIV_FACTOR 2
#define  DDR_RW_BYRES_PER_BURST 16
#define  DDR_RW_BITS_PER_BURST  (DDR_RW_BYRES_PER_BURST*8)
// #define  BUF_START_ADDR_ALIGN (DDR_RW_BURST*DDR_RW_BYRES_PER_BURST*Y_UV_DIV_FACTOR)
// #define  LINE_LEN_STRIDE   BUF_START_ADDR_ALIGN
#define  BUF_START_ADDR_ALIGN (512)
#define  BUF_PAD_LEN (BUF_START_ADDR_ALIGN*16)  //8k pad for every buffer

#define MATRIX_ROW_NUMBER 36
#define MATRIX_COL_NUMBER 64
#define AWB_NEARGRAY_SIZE (MATRIX_COL_NUMBER*MATRIX_ROW_NUMBER)
#define AWB_CHHIST_SIZE 64

#define TRUE 1
#define FALSE 0


typedef enum
{
	CAM_ERR_SUCCUESS=0,
    CAM_ERR_IGNORE=AR_HAL_DEF_ERR(AR_SYS_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUTT+1),
	CAM_ERR_INPUT_NULL_POINT,
	CAM_ERR_INPUT_INVALIDE,
	CAM_ERR_OUTPUT_NULL_POINT,
	CAM_ERR_INPUT_OVERFOLLOW,
	CAM_ERR_NOT_SUPPORT_IOCTL_CODE,
	CAM_ERR_HW_ERROR,
	CAM_ERR_LOW_MEM,
	CAM_ERR_LOW_CAN_NOT_FIND_ITEM,
	CAM_ERR_GET_OBJECT_FAILED,
	CAM_ERR_GET_IN_OUT_COUNT_FAILED,
	CAM_ERR_GET_CAP_FAILED,
	CAM_ERR_DATA_NOT_MATCH,
	CAM_ERR_CTL_NEED_SPECIAL_PROC,
	CAM_ERR_QUEUE_EMPTY,
	CAM_ERR_QUEUE_FULL,
	CAM_ERR_RESOURCE_USING,
	CAM_ERR_UNKNOW_ERROR,
	CAM_ERR_CAMERA_NOT_OPEND,
	CAM_ERR_OP_NOT_ALLOWED,
	CAM_ERR_MULTI_OPEN,
	CAM_ERR_IGNOR,
	CAM_ERR_SERVER_NOT_READY,
	CAM_ERR_SERVER_NO_RESOURCE,
	CAM_ERR_TIME_OUT,
	CAM_ERR_EXIT_MSG,
	CAM_ERR_HANG_UP_MSG,
}ENUM_VIN_RET_T;
typedef enum
{
   MEM_TYPE_START=1,
   MOUNT_ANGLE_START,
   STREAM_FORMAT_START,
   CAM_FACE_START,
   STREAM_TYPE_START,
   STREAM_MODE_START,
   CAM_PRA_START,
   NOTIFY_START,
   FOCUS_STATUS_START,
   HW_CONTROLLER_START,
   HW_IMG_PRO_START,
   HW_DDR_OP_MOD_START,
   DATA_INOUT_START,
   IN_OUT_MODE_START,
   INTER_TYPE_START,
   HW_IRQ_START,
   POW_TYPE_START,
   I2C_ADDR_TYPE_START,
   I2C_DATA_TYPE_START,
   I2C_COMPONENT_CTL_START,
   ISP_STATUS_START,
   ISP_FILTER_CTL_START,
   MODULE_TYPE_START,
   PORT_TYPE_START,
   SUB_MODULE_FILTER_START,
   PORT_CTL_START,
   FILTER_CTL_START,
   SENSOR_IOCTL_START,
   REGISTER_OP_START,
   SENSOR_OUT_FORMAT_START,
   LINK_MODE_START,
   SENSOR_BIT_START,
   BAYER_FORMAT,
   CAM_CTL_START,
   CAM_WORK_MODE_STAT,
   CAM_EVENT_START,
   LINK_TYPE_START,
   STREAM_STATUS_START,
   STREAM_CTL_START,
   PROCESS_MODE_START,
   REG_TYPE_START,
   HW_INTERFACE_START,
   VIF_OUTPUT_START,
   ISP_PRIV_HW_CTL_START,
   TUNING_OPS_START,
   CAM_CTL_API,
   CAM_RPC_CALL_BASE,
}ENUM_CTL_EVENT_START_T;


typedef enum
{
  MODULE_TYPE_SRC=MODULE_TYPE_START*(1<<8),
  MODULE_TYPE_LINK,
  MOUDLE_TYPE_SINK,
  MODULE_TYPE_DRVIER,
  MODULE_TYPE_ALGO,
  MODULE_TYPE_FUNC,
  MODULE_TYPE_HW_INOUT,
  MODULE_TYPE_DRVIER_ROOT,
  MODULE_TYPE_DRVIER_INTERNAL,
  MODULE_TYPE_DRVIER_LEAF,
}ENUM_MODULE_FLAG_T;


typedef enum
{
  META_TYPE_AF_STATS, //0
  META_TYPE_AEC_RRO_STATS,
  META_TYPE_AWB_GREY_STATS,
  META_TYPE_RGB_HISTOGRAM_STATUS, // 5
  META_TYPE_RAW_HISTOGRAM_STATS,
  META_TYPE_GTM_HIS_STATS,
  META_TYPE_AEC_RRO_FACE_STATS,
  META_TYPE_ISP_HDR_RRO_0_STATS, // 7
  META_TYPE_ISP_HDR_RRO_1_STATS,
  META_TYPE_ISP_HDR_AWB_STATS,
  META_TYPE_AEC_INFO, //reported by aec algo done
  META_TYPE_AWB_INFO,//reported by awb algo done
  META_TYPE_AF_INFO,//reported by af algo done
  META_TYPE_INT_REF,  //report the int ref when vsync   //13
  META_TYPE_AEC_MOTION,
  META_TYPE_AEC_GROUP,
  META_TYPE_AWB_GROUP,
  META_TYPE_AF_GROUP,
  META_TYPE_PROC_DEBUG_INFO,
  META_TYPE_LTM_STATS,
  META_TYPE_MAX,
}ENUM_META_DATA_TYPE_T;

typedef struct
{
    AR_S32 s32X;
    AR_S32 s32Y;
    AR_U32 u32Width;
    AR_U32 u32Height;
}STRU_RECT_S;


typedef struct
{
 AR_FLOAT x;
 AR_FLOAT y;
 AR_FLOAT width;
 AR_FLOAT height;
}STRU_CAM_ROI_T;


typedef struct
{
  AR_U16 x;
  AR_U16 y;
}STRU_CAM_POINT;


typedef enum
{
  MOUNT_ANGLE_0=0,
  MOUNT_ANGLE_90=90,
  MOUNT_ANGLE_180=180,
  MOUNT_ANGLE_270=270,
}ENUM_ANGLE_T;

typedef enum
{
  BAYER_RGGB,
  BAYER_BGGR,
  BAYER_GRBG,
  BAYER_GBRG,
}ENUM_BAYER_FORMAT_T;

typedef enum
{
  STREAM_FORMAT_JPG=STREAM_FORMAT_START*(1<<8),
  STREAM_FORMAT_YUV420_8BIT,
  STREAM_FORMAT_YUV420_8BIT_LEGACY,
  STREAM_FORMAT_YUV420_8BIT_CSPS,
  STREAM_FORMAT_YUV422_8BIT,  // yuyv  yuyv yuyv
  STREAM_FORMAT_YUV444_8BIT, //yuv yuv yuv
  STREAM_FORMAT_YUV420_10BIT,
  STREAM_FORMAT_YUV420_10BIT_CSPS,
  STREAM_FORMAT_YUV422_10BIT,
  STREAM_FORMAT_YUV444_10BIT,
  STREAM_FORMAT_YUV422_12BIT,
  STREAM_FORMAT_YUV444_12BIT,
  STREAM_FORMAT_META_8BIT,


  //define isp out format, 9 format
  STREAM_FORMAT_YUV400_8BIT_Plannar,
  STREAM_FORMAT_YUV420_8BIT_Plannar, //mod1
  STREAM_FORMAT_YUV422_8BIT_Plannar,//mod2
  STREAM_FORMAT_YUV444_8BIT_Plannar,//mod3
  STREAM_FORMAT_YUV422_8BIT_semiPlannar,//mod4
  STREAM_FORMAT_YUV420_8BIT_semiPlannar,//mod4
  STREAM_FORMAT_YUV420_10BIT_semiPlannar_packed,//6
  STREAM_FORMAT_YUV422_10BIT_semiPlannar_packed,//7
  STREAM_FORMAT_YUV420_10BIT_semiPlannar_unpacked,//8
  STREAM_FORMAT_YUV422_10BIT_semiPlannar_unpacked,//9
  STREAM_FORMAT_YUV444_10BIT_semiPlannar_unpacked,//10

  STREAM_FORMAT_RAW_UNPACKED_8BIT_RGGB,
  STREAM_FORMAT_RAW_UNPACKED_8BIT_BGGR,
  STREAM_FORMAT_RAW_UNPACKED_8BIT_GRBG,
  STREAM_FORMAT_RAW_UNPACKED_8BIT_GBRG,

  STREAM_FORMAT_RAW_UNPACKED_10BIT_RGGB,
  STREAM_FORMAT_RAW_UNPACKED_10BIT_BGGR,
  STREAM_FORMAT_RAW_UNPACKED_10BIT_GRBG,
  STREAM_FORMAT_RAW_UNPACKED_10BIT_GBRG,

  STREAM_FORMAT_RAW_UNPACKED_12BIT_RGGB,
  STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR,
  STREAM_FORMAT_RAW_UNPACKED_12BIT_GRBG,
  STREAM_FORMAT_RAW_UNPACKED_12BIT_GBRG,

  STREAM_FORMAT_RAW_UNPACKED_14BIT_RGGB,
  STREAM_FORMAT_RAW_UNPACKED_14BIT_BGGR,
  STREAM_FORMAT_RAW_UNPACKED_14BIT_GRBG,
  STREAM_FORMAT_RAW_UNPACKED_14BIT_GBRG,

  STREAM_FORMAT_RAW_UNPACKED_16BIT_RGGB,
  STREAM_FORMAT_RAW_UNPACKED_16BIT_BGGR,
  STREAM_FORMAT_RAW_UNPACKED_16BIT_GRBG,
  STREAM_FORMAT_RAW_UNPACKED_16BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_6BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_6BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_6BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_6BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_7BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_7BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_7BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_7BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_8BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_8BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_8BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_8BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_10BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_10BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_10BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_10BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_12BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_12BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_12BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_12BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_14BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_14BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_14BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_14BIT_GBRG,

  STREAM_FORMAT_RAW_MIPI_16BIT_RGGB,
  STREAM_FORMAT_RAW_MIPI_16BIT_BGGR,
  STREAM_FORMAT_RAW_MIPI_16BIT_GRBG,
  STREAM_FORMAT_RAW_MIPI_16BIT_GBRG,


  STREAM_FORMAT_RAW_RGB_555,
  STREAM_FORMAT_RAW_RGB_444,
  STREAM_FORMAT_RAW_RGB_565,
  STREAM_FORMAT_RAW_RGB_666,
  STREAM_FORMAT_RAW_RGB_888,
  STREAM_FORMAT_RAW_RGB_101010,
  STREAM_FORMAT_RAW_RGB_121212,
  STREAM_FORMAT_RAW_CUS1,
  STREAM_FORMAT_RAW_CUS2,

  STREAM_FORMAT_DVP_RAW6_RGGB,
  STREAM_FORMAT_DVP_RAW6_BGGR,
  STREAM_FORMAT_DVP_RAW6_GRBG,
  STREAM_FORMAT_DVP_RAW6_GBRG,
  STREAM_FORMAT_DVP_RAW7_RGGB,
  STREAM_FORMAT_DVP_RAW7_BGGR,
  STREAM_FORMAT_DVP_RAW7_GRBG,
  STREAM_FORMAT_DVP_RAW7_GBRG,
  STREAM_FORMAT_DVP_RAW8_RGGB,
  STREAM_FORMAT_DVP_RAW8_BGGR,
  STREAM_FORMAT_DVP_RAW8_GRBG,
  STREAM_FORMAT_DVP_RAW8_GBRG,
  STREAM_FORMAT_DVP_RAW10_RGGB,
  STREAM_FORMAT_DVP_RAW10_BGGR,
  STREAM_FORMAT_DVP_RAW10_GRBG,
  STREAM_FORMAT_DVP_RAW10_GBRG,
  STREAM_FORMAT_DVP_RAW12_RGGB,
  STREAM_FORMAT_DVP_RAW12_BGGR,
  STREAM_FORMAT_DVP_RAW12_GRBG,
  STREAM_FORMAT_DVP_RAW12_GBRG,
  STREAM_FORMAT_DVP_RAW14_RGGB,
  STREAM_FORMAT_DVP_RAW14_BGGR,
  STREAM_FORMAT_DVP_RAW14_GRBG,
  STREAM_FORMAT_DVP_RAW14_GBRG,
  STREAM_FORMAT_DVP_RAW16_RGGB,
  STREAM_FORMAT_DVP_RAW16_BGGR,
  STREAM_FORMAT_DVP_RAW16_GRBG,
  STREAM_FORMAT_DVP_RAW16_GBRG,

  STREAM_FORMAT_HDMI_AUDIO,

  STREAM_FORMAT_YUV420_CF50_8BIT_Plannar,
  STREAM_FORMAT_YUV420_CF50_10BIT_Plannar,

  STREAM_FORMAT_YUV420_10BIT_Plannar_packed,
  STREAM_FORMAT_YUV422_10BIT_Plannar_packed,
  STREAM_FORMAT_YUV420_10BIT_Plannar_unpacked,
  STREAM_FORMAT_YUV422_10BIT_Plannar_unpacked,
  STREAM_FORMAT_YUV444_10BIT_Plannar_unpacked,
  STREAM_FORMAT_YUV444_10BIT_Plannar_packed,
  STREAM_FORMAT_INVALID,
 }ENUM_STREAM_DATA_FORMAT_T;

typedef enum
{
 CAM_FACE_UP,
 CAM_FACE_DOWN,
 CAM_FACE_LEFT,
 CAM_FACE_RIGHT,
 CAM_FACE_FRONT,
 CAM_FACE_BACK,
 CAM_FACE_HDMI_AUDIO,
 CAM_FACE_HDMI_VIDEO,
 CAM_FACE_HDMI_DVP_VIDEO,
 CAM_FACE_EMULATOT,
}ENUM_CAM_FACE_T;


enum
{
  CORE_DEV_HW_OUT_TYPE_NORMAL_RAW, //core dev out raw for every sub camera
  CORE_DEV_HW_OUT_TYPE_NORMAL_VC_RAW, //core dev out raw by vc for every sub camera
  CORE_DEV_HW_OUT_TYPE_YUV_AND_SCALER_YUV,//core dev out two yuv, 1 for orignal size, one scaler to another size for every sub camera
  CORE_DEV_HW_OUT_TYPE_YUV, //core device out 1 yuv for every sub camera
  CORE_DEV_HW_OUT_TYPE_MAX,
};


/*
*@brief:
mount_angle	安装角度，比如 0 90 270 180
face	安装的方向，比如上下左右等，目前支持的方向如下方向
focus_supported	是否支持对焦
flash_supported	是否支持闪光灯
sensor_name[64]	本camera 所用的cmos sensor的名字，也就是dev 名字
device_type	设备的类型，比如core设备，子设备等
num_sub_module	如果当前设备是core 设备，那么这个表示这个core 设备拥有的subdev的数量，一般情况下不会大于4个，因为在9301的系统中，vc 无论是dvp 还是mipi 最大是3
p_sub_name[8][64];	存放每一个subdev的名字
core_dev_hw_out_type	Not used
sub_dev_in_core_dev_index	Not used
*/

typedef struct
{
  AR_U32 mount_angle;
  AR_U32 face;
  AR_U32 focus_supported;
  AR_U32 flash_supported;
  AR_CHAR sensor_name[64];
  AR_S32 device_type;
}STRU_CAM_INFO_T;

typedef struct
{
   AR_S32 cam_id;
   STRU_CAM_INFO_T info;
}STRU_CAM_SERV_GET_CAM_INFO_PRA_T;

typedef enum
{
 STREAM_TYPE_PREVIEW=STREAM_TYPE_START*(1<<8),
 STREAM_TYPE_SIM_PREVIEW,
 STREAM_TYPE_VIDEO,
 STREAM_TYPE_RAW_BAYER,
 STREAM_TYPE_VFE_RAW,
 STREAM_TYPE_RAW_YUV,
 STREAM_TYPE_RAW_RGB,
 STREAM_TYPE_SCALER3,
 STREAM_TYPE_SIM_VIDEO,
 STREAM_TYPE_RAW_YUV1,
 STREAM_TYPE_RAW_YUV2,
 STREAM_TYPE_RAW_YUV3,
 STREAM_TYPE_RAW_YUV4,
 STREAM_TYPE_RAW_YUV5,
 STREAM_TYPE_RAW_YUV6,
 STREAM_TYPE_RAW_YUV7,
 STREAM_TYPE_RAW_BAYER_VC0,
 STREAM_TYPE_RAW_BAYER_VC1,
 STREAM_TYPE_RAW_BAYER_VC2,
 STREAM_TYPE_RAW_BAYER_VC3,
 STREAM_TYPE_RAW_YUV_VC0,
 STREAM_TYPE_RAW_YUV_VC1,
 STREAM_TYPE_RAW_YUV_VC2,
 STREAM_TYPE_RAW_YUV_VC3,
 STREAM_TYPE_RAW_YUV_SCALER_VC0,
 STREAM_TYPE_RAW_YUV_SCALER_VC1,
 STREAM_TYPE_RAW_YUV_SCALER_VC2,
 STREAM_TYPE_RAW_YUV_SCALER_VC3,
 STREAM_TYPE_RAW_YUV_SCALER_VC4,
 STREAM_TYPE_RAW_YUV_SCALER_VC5,
 STREAM_TYPE_RAW_YUV_SCALER_VC6,
 STREAM_TYPE_RAW_YUV_SCALER_VC7,
 STREAM_TYPE_RAW_YUV_SCALER,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC0,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC1,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC2,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC3,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC4,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC5,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC6,
 STREAM_TYPE_RAW_YUV_SUB_SCALER_VC7,
 STREAM_TYPE_RAW_YUV_SUB_SCALER,
 STREAM_TYPE_SIM_IR_THERMAL,
 STREAM_TYPE_MAX,
}STRU_STREAM_TYPE_T;

typedef enum
{
	STREAM_MODE_NORMAL,
	STREAM_MODE_CEVA_EIS,
	STREAM_MODE_INTERLACE,
	STREAM_MODE_HW_EIS_LDC, //POSTROC
}ENUM_STREAM_MODE_T;


typedef enum
{
  VFE_MODE_NULL,
  VFE_MODE_CEVA_HDR,
  VFE_MODE_CEVA_LDC,
  VFE_MODE_AI_ISP_NR,
  VFE_MODE_DMA_CPY,
  VFE_MODE_MAX,
}ENUM_VFE_MODE;

//low delay for 9411
typedef enum{
    VI_CH_LOW_DELAY_OFF,
	VI_CH_LOW_DELAY_DDR,
	VI_CH_LOW_DELAY_LINEBUFFER,  /**<不支持*/
}STRU_LOW_DELAY_MODE_T;


typedef enum
{
  LOW_DELAY_SINK_HEVC_H265,
  LOW_DELAY_SINK_H264,
}ENUM_LOW_DELAY_SINK_T;


typedef enum
{
 AEC_AUTO,
 AEC_FULL_SWEEP,/**<扫描曝光表，aec 算法从曝光表的开始一直扫描到结尾，循环往复*/
 AEC_AVERGATE,/**<@note 设置到这个模式后，aec 亮度只有avergae， 亮区，暗区等不起作用*/
 AEC_CENTER_WEIGHT,/**<中心权重曝光，权重由tuning参数的权重表表示*/
 AEC_SPOT,   /**<@note 不支持*/
 AEC_MANUAL, /**<进入手动模式*/
 AEC_LOCKED,/**<锁定aec，设置后，aec被锁定，不再曝光*/
 AEC_MANUAL_UPDATE, /**<@note 不支持*/
}ENUM_AEC_MODE_T;

typedef enum
{
	AWB_D75=0,
	AWB_D65,
	AWB_D50,
	AWB_NOON,
	AWB_CW,
	AWB_TL84,
	AWB_A,
	AWB_H,
	AWB_CUST1,
	AWB_CUST2,
	AWB_AUTO,
 	AWB_MANUAL,
 	AWB_MANUAL_UPDATE,/**<@note 不支持*/
}ENUM_AWB_MODE_T;

typedef enum
{
 AF_AUTO = 0,
 AF_CAF,
 AF_MACRO,
 AF_INFINITE,
 AF_MANUAL,
 AF_MOD_MAX
}ENUM_AF_MODE_T;


typedef struct
{
	AR_S32   aec_mode;
	AR_S32   locked;
	AR_FLOAT exp_time_us_short;
	AR_FLOAT gain;
	AR_FLOAT sensor_gain;
    AR_FLOAT isp_gain1;
	AR_FLOAT isp_gain2;
	AR_FLOAT exp_time_us;
	AR_U32 line_count;
	AR_U32 exp_index;
	AR_U32 lux_index;
	AR_U32 luma_target;
	AR_U32 luma_target_short;
	AR_U32 current_luma;
    AR_FLOAT weightSum;
	AR_FLOAT luma_avg;
	AR_FLOAT lumaSum;
    AR_S32 cur_af_luma;
	AR_FLOAT gain_before_banding;
	AR_S32 line_cnt_before_banding;
	AR_FLOAT gain_after_banding;
	AR_S32 line_cnt_after_banding;
	AR_S32 hybrid_luma;
	AR_S32 default_luma_target_compensated;
	AR_S32 luma_comp_target;
	AR_S32 ae_settled;
	AR_S32 ae_settled_short;
	AR_U32 flash_status;
	AR_FLOAT dark_interpolated_thld0;
	AR_FLOAT dark_interpolated_thld1;
	AR_FLOAT dark_interpolated_thld2;
	AR_FLOAT over_exp_adjust_ratio;
	AR_U32 high_luma_region_count;
	AR_S32 high_luma_region_th;
	AR_S32 bright_discard_th;
	AR_S32 dark_discard_th;
	AR_S32 metering_type;
	AR_S32 motion;
	AR_S32 sad_for_flat_scene;
	AR_S32 run_times;
	AR_FLOAT long_exp_ration;
	AR_FLOAT max_drc_gain;
	AR_FLOAT min_drc_gain;
	AR_FLOAT max_drc_gain1;
	AR_FLOAT ration_time;
	AR_FLOAT ration_gain;
	AR_FLOAT ration_total;
	AR_FLOAT drc_gain1;
	AR_FLOAT drc_gain2;
	AR_FLOAT drc_gain;
    AR_FLOAT cur_real_gain_short;
	AR_FLOAT cur_real_gain;
	AR_S32 cur_line_cnt_short;
	AR_S32 cur_line_cnt;
	AR_S32 short_luma;
	AR_S32 short_exp_index;
	AR_FLOAT y_average[16*36];
	AR_FLOAT y_average_short[16*36];
	AR_S32 extreme_red_count;
	AR_S32 extreme_green_count;
	AR_S32 extreme_blue_count;
	AR_S32 dark_region_count;
	AR_S32 bright_region_count;
	AR_S32 settled_time_frames;
	AR_S32 weightSum_short;
	AR_S32 luma_avg_short;
	AR_S32 lumaSum_short;
	AR_S32 dark_region_count_short;
	AR_S32 bright_region_count_short;	
	float luma_avg_r;
	float luma_avg_g;
	float luma_avg_b;
}STRU_AEC_GET_INFO_T;

typedef struct
{
  ENUM_AEC_MODE_T   aec_mode;
  AR_FLOAT gain;/**<曝光的实际gain*/
  AR_FLOAT exp_time_us;/**<曝光时间，单位为us，不能大于曝光表的上限规定的时间*/
  AR_U32 luma_target;  /**<@note not used,如果是读取表示当前亮度*/
  AR_FLOAT gain_short;/**<短曝光的实际gain，hdr 模式的时候使用*/
  AR_FLOAT exp_time_us_short;/**<短曝光时间，单位为us，不能大于短曝光表的上限规定的时间，hdr模式的时候使用*/
  AR_FLOAT lux_index; /**<曝光索引，值越大，表示环境光越暗，可以用来做日夜切换的时候表示环境亮度来用*/
  AR_FLOAT luma_avg_r;/**<当前图像的R 通道的平均亮度值*/
  AR_FLOAT luma_avg_g;/**<当前图像的G 通道的平均亮度值*/
  AR_FLOAT luma_avg_b;/**<当前图像的B 通道的平均亮度值*/
}STRU_AEC_SET_INFO_T;
typedef struct
{
	AR_S32  eis_algo_state;
	AR_S32  eis_distortion_state;
}STRU_CAM_EIS_T;

typedef struct
{
  ENUM_AWB_MODE_T awb_mode;
  AR_U32 cct;   /**< 色温，当awb_mode   设置为 AWB_MANUAL的时候，是设置进去的色温值*/
  AR_FLOAT r_gain; /**< 白平衡的r g b增益，最大值为32. 当当awb_mode   设置为 AWB_MANUAL的时候生效，cct和 gain都是AWB_MANUAL的时候生效，但是一次设置只能有一个生效，
  生效的优先级是gain的优先级大于cct的优先级，当gain被设置为小于0的时候，gain 被禁止，这时候cct 被使用来设置awb*/
  AR_FLOAT g_gain; 
  AR_FLOAT b_gain;

}STRU_AWB_SET_INFO_T;

typedef struct
{
	AR_FLOAT all_sgw_rg;
	AR_FLOAT all_sgw_bg;
	AR_FLOAT cct;
	AR_S32 main_decision;
	AR_S32 sub_decsion;
	AR_FLOAT dist;
	AR_FLOAT dist_w;
	AR_FLOAT luma_cct_w;
	AR_S32   y_luma;
	AR_S32   point_type; //bit0: grey bit1: white bit2:extream_blue bit3 extream_red bit4:green
}STRU_AWB_STATS_POINT_INFO_T;


typedef struct
{
  AR_S32 awb_mode;
  AR_U32 cct;
  AR_FLOAT r_gain;
  AR_FLOAT g_gain;
  AR_FLOAT b_gain;
  AR_S32 current_lux_index;
  AR_S32 is_extreme_blue;
  AR_S32 is_extreme_green;
  AR_FLOAT ExtremeB_pec;
  AR_FLOAT GreenZone_pec;
  AR_FLOAT last_R_gain;
  AR_FLOAT last_G_gain;
  AR_FLOAT last_B_gain;
  AR_FLOAT smooth_weight;
  AR_FLOAT r_gain_adj_coef;
  AR_FLOAT b_gain_adj_coef;
  AR_FLOAT GreenZone_num;
  AR_FLOAT GreenZone_r_ave;
  AR_FLOAT GreenZone_g_ave;
  AR_FLOAT GreenZone_b_ave;
  AR_S32 ExtremeB_num;
  AR_FLOAT ExtremeB_r_ave;
  AR_FLOAT ExtremeB_g_ave;
  AR_FLOAT ExtremeB_b_ave;
  AR_FLOAT green_rg;
  AR_FLOAT green_bg;
  AR_S32 percent_cnt;
  AR_S32 sgw_cnt;
  AR_S32 rg_bg_as0_num;
  AR_S32 grey_point_num;
  AR_S32 out_line_num;
  AR_S32 block_percent_low_num;
  AR_FLOAT sgw_rg_ratio;
  AR_FLOAT sgw_bg_ratio;
  AR_S32 a_cluster;
  AR_FLOAT a_rg_ratio;
  AR_FLOAT a_bg_ratio;
  AR_FLOAT unsat_y_a_ave;
  AR_S32 h_cluster;
  AR_FLOAT h_rg_ratio;
  AR_FLOAT h_bg_ratio;
  AR_FLOAT unsat_y_h_ave;
  AR_S32 f_cluster;
  AR_FLOAT f_rg_ratio;
  AR_FLOAT f_bg_ratio;
  AR_FLOAT unsat_y_f_ave;
  AR_S32 day_cluster;
  AR_FLOAT day_rg_ratio;
  AR_FLOAT day_bg_ratio;
  AR_FLOAT unsat_y_day_ave;
  AR_FLOAT w_d;
  AR_FLOAT w_f;
  AR_FLOAT w_a;
  AR_FLOAT w_h;
  AR_FLOAT wrg_sum;
  AR_FLOAT wbg_sum;
  AR_FLOAT ave_rg_ratio;
  AR_FLOAT ave_bg_ratio;
  AR_FLOAT unsat_y_mid;
  AR_FLOAT unsat_y_max;
  AR_FLOAT unsat_y_min_threshold;
  AR_S32 valid_sample_cnt;
  AR_S32 sat_cnt;
  AR_S32 sat_a_cluster;
  AR_FLOAT sat_a_rg;
  AR_FLOAT sat_a_bg;
  AR_S32 sat_h_cluster;
  AR_FLOAT sat_h_rg;
  AR_FLOAT sat_h_bg;
  AR_S32 sat_f_cluster;
  AR_FLOAT sat_f_rg;
  AR_FLOAT sat_f_bg;
  AR_S32 sat_day_cluster;
  AR_FLOAT sat_day_rg;
  AR_FLOAT sat_day_bg;
  AR_S32 gw_init_decision;
  AR_FLOAT gw_init_decision_rg;
  AR_FLOAT gw_init_decision_bg;
  AR_S32 gw_init_decision_cct;
  AR_S32 gw_init_domain_cluster;
  AR_S32 wh_cnt;
  AR_FLOAT white_rg_ratio;
  AR_FLOAT white_bg_ratio;
  AR_S32 white_decision;
  AR_FLOAT white_dist;
  AR_FLOAT awb_sgw_cluster_dist2_max_compact_cluster;
  AR_FLOAT awb_sgw_cluster_temp_min_dist;
  AR_S32 sw_policy;
  AR_FLOAT sw_policy_rg;
  AR_FLOAT sw_policy_bg;
  AR_S32 sw_policy_decision;
  AR_S32 run_times;
  STRU_AWB_STATS_POINT_INFO_T awb_stats_point_info[AWB_NEARGRAY_SIZE];
}STRU_AWB_GET_INFO_T;

#define AF_INFO_SEARCH_POINT_NUM 1024
typedef struct
{
  AR_U32 fv;
  AR_U32 pos;
}STRU_AF_SEARCH_POINT_T;
//AF_CTL_GET_3A_INFO
typedef struct
{
    AR_U32 af_mode;
	AR_U32 algo_type;
    AR_U32 len_position;
    AR_U32 focus_value;
    AR_U32 history_pos;
    AR_U32 history_focus_value;
    AR_U32 stats_type;
	AR_U32 point_num;
	STRU_AF_SEARCH_POINT_T point[AF_INFO_SEARCH_POINT_NUM];
} STRU_AF_GET_INFO_T;


typedef struct
{
 AR_U32 af_mod;
 AR_U32 len_position;
 AR_U32 total_steps;
}STRU_AF_SET_INFO_T;

typedef struct
{
 AR_U32 af_mod; 
 AR_U32 algo_type;
 AR_U32 len_position;
 AR_U32 focus_value;
 AR_U32 stats_type;
}STRU_AF_GET_BASIC_INFO_T;

typedef struct
{
  STRU_AEC_GET_INFO_T aec_info;
  STRU_AWB_GET_INFO_T awb_info;
  STRU_AF_GET_INFO_T  af_info;
}STRU_ALGO_3A_GET_INFO_T;

typedef struct
{
    STRU_AEC_SET_INFO_T  aec_info;
    STRU_AWB_SET_INFO_T  awb_info;
	STRU_AF_GET_BASIC_INFO_T af_info;
}STRU_BASIC_3A_INFO_T;

typedef struct
{
  // AR_U32 frame_id;
  AR_U32 focus_value_sharpness[16][9];
  AR_U32 focus_value_fir[16][9];
  AR_U32 focus_value_iir[16][9];
  AR_U32 af_luma_info[16][9];
  AR_U32 af_high_num_info[16][9];
} STRU_CAM_AF_STATISTIC_PARAM_T;

typedef enum
{
   SENSOR_HDR_OFF,
   SENSOR_HDR_ON,
}ENUM_SENSOR_HDR_STATUS_T;


typedef struct
{
 AR_S32 camera_id;
 AR_S32 stats_type;
 AR_S32 info_src;       // the source of aec out information. 0 - from aec algorithm; 1 - from user. This param is used internally, user ignore it please.

 AR_S32      updated;
 AR_U32 line_count;
 AR_U32 exp_time_us;
 AR_FLOAT    real_gain;
 AR_FLOAT    sensor_gain;
 AR_FLOAT    isp_gainl;
 AR_FLOAT    isp_gain2;
 AR_FLOAT    exp_gain;
 AR_U32 exp_index;
 AR_U32 short_exp_index;
 AR_U32 lux_index;
 AR_U32 lux_value;
 AR_U32 ae_settle;
 float current_luma;
 /*
	//ration = long/short
	exp ration array:
	index 0: short exp ration  long/short
	index 1: midle exp ration  midle/short
	index 2: midle+ exp ration midle+/short
	index 3: exp ration, always is 1 short/short
 */

 AR_FLOAT    ration_gain[4];
 AR_FLOAT    ration_time[4];
 AR_FLOAT      total_raion[4];
 AR_FLOAT    drc_gain;
 AR_FLOAT    drc_gain_1; //for ceva drc gain
 AR_FLOAT    drc_gain_2;//for isp drc gain
 //the value reprent the aec input stats convert to output , please caller to malloc buffer for it
 float fps;
}STRU_AEC_OUT_T;//aec_out_put_t;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 stats_type;
  AR_S32 info_src;      // the source of awb out information. 0 - from awb algorithm; 1 - from user. This param is used internally, user ignore it please.

  AR_FLOAT r_gain;
  AR_FLOAT g_gain;
  AR_FLOAT b_gain;
  AR_FLOAT cct;
}STRU_AWB_OUT_T;//awb_out_put_t;


typedef struct
{
    AR_S32 camera_id;
    AR_S32 stats_type;
    AR_S32 info_src;    // the source of af out information. 0 - from af algorithm; 1 - from user. This param is used internally, user ignore it please.
    AR_U32 lens_position;
	AR_U32 focus_value;
    AR_S32 dir;
    AR_S32 move_step;
    AR_S32 b_move_lens;
    // af_focus_status_t status;
    STRU_ROI_T roi;
}STRU_AF_OUT_T;//af_algo_output_t;


typedef struct
{
    AR_U32 sof_mode; // 0: auto, 1: manual,  lower priority than sof_mode in pre_pra
    AR_U32 to_trigger; // 0: not trigger sof, 1: trigger one sof
}STRU_AR_CAMERA_SOF_PRO_T;//ar_camera_sof_prop_t;

typedef struct
{
    AR_U32 trigger_mode; // 0 ,trigger run always 1 tirgger run num frames
    AR_U32 trigger_num; //how much frame to run
    AR_U32 need_sync;
}STRU_AR_CAMERA_TRIGGER_PROP_T;//ar_camera_trigger_prop_t;

typedef struct
{
    AR_U32 x;
    AR_U32 y;
    AR_U32 width;
    AR_U32 height;
}STRU_AR_CAMERA_MANUAL_CROP_PROP_T;

typedef enum
{
 CAM_PRA_SET_GET_FLASH_MODE=CAM_PRA_START*(1<<8),
 CAM_PRA_GET_SUPPORTED_FLASH_MODE,

 CAM_PRA_SET_GET_FOCUS_MODE,
 CAM_PRA_GET_SUPPORTEDFOCUS_MODE,

 CAM_PRA_GET_SUPPORTED_PREVIEW_SIZE,
 CAM_PRA_GET_SUPPORTED_PICTURE_SIZE,
 CAM_PRA_GET_SUPPORTED_VIDEO_SIZE,

 CAM_PRA_SET_AE_MODE,
 CAM_PRA_GET_AE_MODE,
 CAM_PRA_SET_MANUAL_AE,
 CAM_PRA_SET_AE_ROI,
 CAM_PRA_GET_AE_ROI,
 CAM_PRA_SET_AWB_MODE,
 CAM_PRA_SET_MANUAL_AWB,

 CAM_PRA_SET_IMG_BRIGHTNESS,
 CAM_PRA_SET_FACE_AEC,
 CAM_PRA_SET_IMG_CONTRAST,
 CAM_PRA_SET_IMG_SATURABILITY,
 CAM_PRA_SET_IMG_ACUTANCE,
 CAM_PRA_SET_DENOISE_3D,
 CAM_PRA_SET_DENOISE_RNR,
 CAM_PRA_SET_DENOISE_CNF,
 CAM_PRA_SET_IMG_MIRRO,
 CAM_PRA_GET_IMG_MIRRO,
 CAM_PRA_SET_IMG_FLIP,
 CAM_PRA_GET_IMG_FLIP,
 CAM_PRA_SET_AF_MODE,
 CAM_PRA_GET_AF_MODE,
 CAM_PRA_SET_MANUAL_AF,
 CAM_PRA_FOCUS,
 CAM_PRA_FOCUS_ROI,
 CAM_PRA_GET_3A_INFO,
 CAM_PRA_SET_SENCE_MODE,
 CAM_PRA_GET_SCENE_MODE,
 CAM_PRA_SET_SENCE,
 CAM_PRA_GET_SCENE,
 CAM_PRA_SET_LED,
 CAM_PRA_GET_LED,
 CAM_PRA_SET_IR_CUTTER,
 CAM_PRA_GET_IR_CUTTER,
 CAM_PRA_SET_SENSOR_HDR,
 CAM_PRA_GET_SENSOR_HDR,
 CAM_PRA_SET_HDR_AUTO_DETECT,
 CAM_PRA_GET_HDR_AUTO_DETECT,
 CAM_PRA_GET_IMG_BRIGHTNESS,
 CAM_PRA_GET_IMG_SATURABILITY,
 CAM_PRA_GET_IMG_CONTRAST,
 CAM_PRA_GET_ACUTANCE,
 CAM_PRA_GET_BASIC_3A_INFO,

 CAM_PRA_GET_CSC,
 CAM_PRA_SET_CSC,
 CAM_PRA_SET_DETAIL_ENHANCE,
 CAM_PRA_GET_DETAIL_ENHANCE,
 CAM_PRA_SET_IMG_GLOBAL_CONTRAST,
 CAM_PRA_GET_IMG_GLOBAL_CONTRAST,

 CAM_PRA_SET_FPS_RANGE,
 CAM_PRA_GET_FPS_RANGE,
 CAM_PRA_SET_EXP_LIMIT,
 CAM_PRA_GET_EXP_LIMIT,
 CAM_PRA_SET_EIS_STATE,
 CAM_PRA_GET_ANTIBANDING,
 CAM_PRA_SET_ANTIBANDING,
 CAM_PRA_SET_MOTION_DETECT,
 CAM_PRA_SET_DRC_MODE,
 CAM_PRA_GET_DRC_MODE,
 CAM_PRA_SET_HDR_PRA_MODE,
 CAM_PRA_GET_HDR_PRA_MODE,
 CAM_PRA_SET_STREAM_FPS_SUBSAMPLE_RATE, //vif can set the subsample by hw ,but isp only do it use software.we can set the  pra anyware ,but for lowdealy app, we need stop steam ,restart it
 CAM_PRA_GET_STREAM_FPS_SUBSAMPLE_RATE,
 CAM_PRA_SET_STREAM_FPS_RATE,
 CAM_PRA_GET_STREAM_FPS_RATE,
 CAM_REGISTER_STATUS_NOTIFY,
 CAM_SET_PRE_PRA,
 ISP_SET_WORK_MODE,
 CAM_SET_RAW_TO_SENSOR_SIM,
 CAM_SET_TUNINIG_REQUEST,
 CAM_SERV_GET_CAM_NUM,
 CAM_SERV_GET_CAM_INFO,
 CAM_PRA_SET_DIGITAL_ZOOM,
 CAM_PRA_GET_DIGITAL_ZOOM,
 CAM_PRA_GET_AF_STATISTIC_INFO,
 CAM_PRA_RETURN_AF_STATISTIC_BUFFER,
 CAM_PRA_ERROR_PROC,
 CAM_PRA_GET_DENOISE_3D,
 CAM_PRA_SET_DENOISE_2D,
 CAM_PRA_GET_DENOISE_2D,
 CAM_PRA_GET_SENSOR_HW_STATS,
 CAM_PRA_SET_SENSOR_HW_STATS,
 CAM_PRA_REQUSET_STREAM_BUFFER,
 CAM_PRA_Q_STREAM_BUFFER,
 CAM_PRA_DQ_STREAM_BUFFER,
 CAM_PRA_TRIGER_SENSOR_RUN,
 CAM_PRA_PREPARE_STILL_PICTURE,
 CAM_PRA_SEND_AEC_UPDATE,
 CAM_PRA_SEND_AWB_UPDATE,
 CAM_PRA_SEND_AF_UPDATE,
 CAM_PRA_SEND_VSYNC_UPDATE,
 CAM_PRA_SEND_AEC_RUN,
 CAM_PRA_SEND_AWB_RUN,
 CAM_PRA_SEND_AF_RUN,
 CAM_PRA_SET_SENSOR_VAR_PRA,
 CAM_PRA_GET_SENSOR_VAR_PRA,
 CAM_PRA_GET_EVENT,
 CAM_PRA_RELEASE_EVENT,
 CAM_PRA_REGISTER_EVENT,
 CAM_PRA_UNREGISTER_EVENT,
 CAM_PRA_CREATE_EVENT_CLIENT,
 CAM_PRA_DELETE_EVENT_CLIENT,
 CAM_PRA_TRIGGER_WITH_NUMBER,
 CAM_PRA_PRIV_CTRL,
 CAM_PRA_TRIGGER_SOF,
 CAM_PRA_SET_IMG_HUE,
 CAM_PRA_GET_IMG_HUE,
 CAM_PRA_GET_DENOISE_RNR,
 CAM_PRA_SET_IMG_LTM,
 CAM_PRA_GET_IMG_LTM,
 CAM_PRA_GET_AEC_UPDATE,
 CAM_PRA_GET_AWB_UPDATE,
 CAM_PRA_GET_AF_UPDATE,
 CAM_PRA_GET_LIMIT_INFO,
 CAM_PRA_SET_MIPI_DESKEW,
 CAM_PRA_GET_PIPE_BUFFER,
 CAM_PRA_GET_PIPE_RUN_INFO,
 CAM_PRA_SET_AWB_ROI,
 CAM_PRA_GET_AWB_ROI,
 CAM_PRA_SET_MANUAL_CROP,
 CAM_PRA_SET_IR_NUC_PARAM,
 CAM_PRA_GET_IR_NUC_PARAM,
 CAM_PRA_SET_IR_QGG_PARAM,
 CAM_PRA_GET_IR_QGG_PARAM,
 CAM_PRA_SET_IR_GTM_PARAM,
 CAM_PRA_GET_IR_GTM_PARAM,
 CAM_PRA_IR_ALGO_PROCESS,
 CAM_PRA_TUNING_PRA_UPDATE, 
 CAM_PRA_LTM_ALGO_RUN,
 CAM_PRA_LTM_ALGO_UPDATE,
 CAM_PRA_GET_IR_SNS_ATTR,
 CAM_PRA_START_IR_SNS_CALIB,
 CAM_PRA_GET_IR_SNS_CALIB_RESULT,
 CAM_PRA_CANCEL_IR_SNS_CALIB,
 CAM_PRA_START_IR_OCC_CALIB,
 CAM_PRA_GET_IR_OCC_CALIB_RESULT,
 CAM_PRA_CANCEL_IR_OCC_CALIB,
 CAM_PRA_START_IR_B_CALIB,
 CAM_PRA_GET_IR_B_CALIB_RESULT,
 CAM_PRA_CANCEL_IR_B_CALIB,
 CAM_PRA_IR_KB_CALIB,
 CAM_SERVER_STOP,
 CAM_PRA_GET_MIPI_DESKEW,
 CAM_PRA_GET_MIPI_DESKEW_STATUS,
 CAM_PRA_SET_EXP_POLICY, 
 CAM_PRA_GET_EXP_POLICY, 
 CAM_PRA_SET_BLACK_LIGHT_MODE, 
 CAM_PRA_GET_BLACK_LIGHT_MODE,
 CAM_PRA_SET_CF50_CMP_PRA,
 CAM_PRA_SET_IMG_GAMMA, 
 CAM_PRA_GET_IMG_GAMMA, 
 CAM_PRA_SET_SENSOR_OTP_PRA,
}ENUM_CAM_PRA_T;

/**定义CF50 的压缩模式，分为无损和有损两种*/
typedef enum 
{
   CF50_CMP_MODE_LOSSLESS,
   CF50_CMP_MODE_LOSS,
}ENUM_CF50_CMP_MODE_T;
   
/**定义CF50 的压缩率，只有高低两种，当压缩模式为无损压缩的时候，本参数不起作用,本压缩参数表示，把128bit 压缩到多少bit。
 用户可以直接用一个int 的值 0 --128 直接赋值enCmpRate，从而实现无极的压缩率设置*/
typedef enum
{
   CF50_CMP_RATE_HIGH=96,
   CF50_CMP_RATE_LOW=64,   
}ENUM_CF50_CMP_RATE_T;


/**定义CF50 的压缩属性，主要包括压缩模式，压缩率。以及osd区域，
所谓osd区域就是这个区域不会进行压缩，后续的ge2d 可以在这个区域里绘制,
如果在通道的属性中设置了cf50（seg）的压缩模式，用户必须使用api 配置压缩属性，否则压缩属性将采用默认的形式
默认的参数为上一次配置的属性，或者第一次启动后的结构清零
用户设置压缩属性建议在AR_MPI_VI_SetChnAttr 后配置
本属性同时也属于动态属性，用户可以在AR_MPI_ISP_Run 后修改压缩属性*/
typedef struct
{
    ENUM_CF50_CMP_MODE_T enMode; 
	ENUM_CF50_CMP_RATE_T enCmpRate; //用户可以直接用一个int 的值 0 --128 直接赋值enCmpRate，从而实现无极的压缩率设置
	AR_S32 s32OsdCount;/**<通道 osd 区域的个数，最多8个osd 区域，如果配置为0，关闭掉osd*/
	STRU_RECT_S stOsd[8];/**<通道osd 区域,这个区域中，不会进行cf50压缩，成员    AR_S32 s32X:区域的左上角x方向坐标，必须128字节对齐;
    AR_S32 s32Y区域的y左上角坐标，必须16字节对齐：;
    AR_U32 u32Width 区域的宽度，128 字节对齐;
    AR_U32 u32Height 区域的高度，16 字节对齐;*/
}STRU_CAM_PRA_CF50_CMP_PRA_T;

typedef struct
{
   AR_S32 cam_id;
   AR_S32 stream_id;
   STRU_CAM_PRA_CF50_CMP_PRA_T pra;
}STRU_CAM_PRA_SET_CF50_CMP_PRA_T;


/*
*@brief:
描述stream的属性，creat stream的时候必须指定的参数。
Stream的连接关系是由stream type 和stream mode 共同决定的。
Stream type 决定大的方向，决定数据从哪里出来，
Stream mode 决定具体的连接关系，那些hw 模块被连接进来，那些不需要。
本属性同时描述的stream 是以roi的形式给出的，也就是说roi 控制了stream被输出到一块buffer的某一个位置
*@param:
camera_id	Pipe id
stream_type	Stream的类型，主要定义了stream在vin 系统内部的连接顺序
mode	Stream的模式，辅助stream type 决定连接顺序
width	有效数据的像素宽度
height	有效数据的像素的高度
format	有效数据的格式
fps	app请求的Stream的帧速率
x	有效像素在整幅图像中的x 方向（水平方向）开始位置
y	有效像素在整幅图像中的y 方向（垂直方向）开始位置
total_w	Stream buffer 的总宽度
total_h	Stream buffer的总高度
stride	Buffer的行长，buffer长度=stride*total_h
*@note
Pipe属性，和 stream的属性共用决定这stream的link关系。 在没有打开手动max w h的时候，
第一个创建的stream的w h fps 决定着sensor 分辨率的选择。第二个stream的w h fps 不得大于第一个。
这是一个限制。为了摆脱这个限制，建议永远打开pipe属性的man max w h fps。
*/


typedef struct
{
	AR_S32 camera_id;
	AR_U32 stream_type;
	AR_U32 mode;
	AR_S32 width;
	AR_S32 height;
	AR_U32 format;
	AR_FLOAT fps;
	AR_S32 x;
	AR_S32 y;
	AR_S32 total_w;  //x+width<=total_w
	AR_S32 total_h;  //y+total_h<=total_h
	AR_S32 stride;
	AR_S32 skip;
	AR_S32 keep_ration;
	AR_S32 anti;
	AR_U32 hw_out_index; //use which hw out to out the stream
	AR_S32 queue_depth;	
	STRU_CAM_ROI_T roi;  /*配置isp 输出的roi*/
	STRU_LOW_DELAY_MODE_T enLowDelayMode; /*9411 low delay 模式*/
	AR_S32 s32DelayLine;  /* for 9411<通道写入多少行后，输出lowdelay 信号，默认16*/	
	ENUM_LOW_DELAY_SINK_T low_delay_sink;
	STRU_CAM_PRA_CF50_CMP_PRA_T stChCmpAttr;
	AR_S32 softlowdelay;  
}STRU_AR_STREAM_PROPERTY_T;


typedef enum
{
  BLACK_LIGHT_MODE_OFF, /**< 关闭背光补偿和强光抑制*/
  BLACK_LIGHT_COMPENSTATE, /**< 打开背光补偿，暗的地方会曝光出来，但是亮的地方会过曝*/
  STRONG_LIGHT_SUPPRESSION,/**< 打开强光抑制，亮的区域曝光出来，暗的地方欠曝*/
}ENUM_BLACK_LIGHT_MODE;

typedef struct
{
   ENUM_BLACK_LIGHT_MODE mode;  /**<此模式只在aec 测光模式为中心权重的情况下有效*/
   int strength;/**<0--100*/
}STRU_BLACK_LIGHT_T;

typedef struct
{
  int seg_num; /**<曝光的段数，每一个段都有曝光的开始和结束，有增益优先和快门优先的选择,每一个段都有开始和结束，
  上一个段的结束是下一个段的开始,当seg_num <=0 表示取消当前设置的策略，当获取策略的时候，获取到当前使用的曝光策略，如果有用户设置了曝光策略，那么返回用户的曝光策略
  如果没有用户设置，或者用户设置后又取消了，那么返回的seg_num 为小于等于0，其他成员无效 */
  float gain[8]; /**<每一个段的gain值，最大支持7个段，7个段需要8个数值来表示*/
  uint32_t shutter[8];/**<每一个段的曝光时间，最大支持7个段，7个段需要8个数值来表示,单位us*/
  int   b_gain_first[7];/**<每一个段的的曝光选择，可以选择是否快门优先或者增益优先*/
}STRU_EXP_POLIOCY_T;


typedef struct
{
   AR_S32 status;  /**<led status. 0 the led off, 1 the led on */
   AR_S32 led_current[4];   /**< led 的电流，最多支持设置4个led的电流，-1 表示这个led 没有*/
   AR_S32 on_times_ms[4];   /**< led 导通后on的时间，最多支持设置4个led的导通，-1 表示这个led 没有*/   
   AR_S32 reverted[4];
}STRU_CAM_PRA_LED_PRA_T;

typedef struct
{
   AR_S32 status;  /**<led status. 0 the ir off, 1 the ir on */
   AR_S32 reverted[4];
}STRU_CAM_PRA_IR_CUTTER_PRA_T;


typedef struct
{
	int cam_id;
	int mesh_w;
	int mesh_h;
	int stats_bin_num;
	int lut_bin_num;
	void *p_stats_phy;
	void *p_out_lut_phy;   
	void *p_out_lut_1_phy;  
}STRU_CAM_PRA_LTM_ALGO_RUN_T;

typedef struct
{
	int cam_id;
	int mesh_w;
	int mesh_h;
	int lut_bin_num;
	void *p_out_lut_phy;   
	void *p_out_lut_1_phy;
}STRU_CAM_PRA_LTM_ALGO_UPDATE_T;

/**<其中的数组0 1 2 分别表示长曝光，中曝光，短曝光。 非hdr 模式下只有0 是有效的。hdr 模式暂时不支持 */
typedef struct
{
   AR_S32 max_exp_times_us[3]; /**<曝光时间最大值，这个值来自于aec 算法，如果sensor的曝光函数做了限制，这里不能反映出来 */
   AR_S32 min_exp_times_us[3]; /**<曝光时间最小值，这个值来自于aec 算法，如果sensor的曝光函数做了限制，这里不能反映出来 */
   AR_S32 max_fps;/**暂时不支持 */
   AR_S32 min_fps;/**暂时不支持 */
   AR_FLOAT max_gain[3];/**<曝光增益最大值，这个值来自于aec 算法，如果sensor的曝光函数做了限制，这里不能反映出来 */
   AR_FLOAT min_gain[3];/**<曝光增益最小值，这个值来自于aec 算法，如果sensor的曝光函数做了限制，这里不能反映出来 */
   AR_S32 max_lens_pos;/**<暂时不支持 */
   AR_S32 min_lens_pos;/**<暂时不支持*/
}STRU_CAM_PRA_GET_LIMIT_INFO_PRA_T;


typedef struct
{
  AR_S32 stream_id; //input
  AR_S32 isp_irq[64];
  AR_S32 vif_irq[64];

  //work mode etc
  AR_S32 work_mode;
  AR_S32 sensor_fps;
  AR_FLOAT current_res;
  AR_S32 isp_used;

  AR_S32 isp_wait_on_sch;
  AR_S32 isp_wait_on_input_queue;
  AR_S32 isp_sch_trigger_count;
  AR_S32 isp_fetch_comp_count;
  AR_S32 isp_wait_on_sof;
  AR_S32 isp_wait_on_all_isp_done;  

  //for ddr mode pipe info
  AR_S32 isp_trigger_count;
  AR_S32 vif_loss_count;
  AR_S32 vif_done_count;
  AR_S32 vif_frame_count;  
  AR_S32 vif_port_num;
  AR_S32 isp_process_time_us;
  AR_S32 pipe_bandwidth;
  //if stream id >= 0, the flowing pra will be effect
  AR_FLOAT buffer_sink_fps_stats;
  AR_S32   isp_frame_count;
  AR_S32   isp_loss_frame_count;
  AR_S32   isp_vsync_count;
  AR_S32   isp_done_count;
  AR_S32   buffer_count_inqueue;
  AR_S32   port_num;
  AR_S32   ch_bandwidth;
}STRU_CAM_PRA_GET_PIPE_RUN_INFO_PRA_T;


typedef struct
{
    AR_S32 lane;
    AR_S32 val;
}STRU_CAM_PRA_MIPI_DESKEW_PRA_T;

typedef struct
{
  AR_S32 cam_id;
  AR_S32 num; //for cam id, run num frames,the the sensor will auto stop run
}STRU_CAM_PRA_TRIGER_SENSOR_RUN_PRA_T;


typedef struct
{
 AR_S32 enalbe;  
 AR_S32 limit;  
}STRU_CAM_PRA_SET_EXP_LIMIT_PRA_T;


typedef enum
{
   DENOISE_MODE_AUTO,
   DENOISE_MODE_MANUAL,
   DENOISE_MODE_EXPERT,
}ENUM_DENOISE_MODE_T;

typedef struct
{
  AR_S32 cam_id;
  AR_S32 hw_on;
}STRU_CAM_PRA_GET_SENSOR_HW_STATS_PRA_T;

typedef enum{
	NOISE_TYPE_LUMA,/**<亮度噪声*/
	NOISE_TYPE_CHROMA,/**<彩色噪声*/
}ENUM_NOISE_TYPE_T;

typedef struct
{
  ENUM_NOISE_TYPE_T denoise_type;
  AR_S32 strength;  /**<0--100*/
}STRU_CAM_PRA_DENOISE_2D_PRA_T;

typedef STRU_CAM_PRA_DENOISE_2D_PRA_T STRU_DENOISE_2D_PRA_T;

typedef struct {
    AR_U32 flat_zone_enh;       /**<平坦区细节增强：0---100*/
    AR_U32 detail_zone_enh;     /**<非平坦区细节增强：0---100*/
} STRU_DETAIL_ENH_PRA_T;

typedef enum
{
    CAM_SRC_ERROR_PROC_CODE_NONE=0,
    CAM_SRC_ERROR_PROC_CODE_STREAM_TIME_OUT,
    CAM_SRC_ERROR_PROC_CODE_MAX
}ENUM_CAM_SRC_ERR_CORE_T;


typedef enum
{
    CAM_BT709_TYPE,
    CAM_BT601_TYPE,
}ENUM_CAM_DVP_TYPE_T;

typedef struct
{
  AR_S32 type;       //BT709 or BT601
  AR_S32 full_range; //0 not full range, 1 full range
}STRU_CAM_PRA_CSC_PRA_T;

typedef STRU_CAM_PRA_CSC_PRA_T STRU_CSC_PRA_T;

#define MAX_SUB_SAMPLE_RATE_FRAMES 256
typedef struct
{
    AR_S32 stream_id;
    AR_S32 total_frames;//采样周期中断帧数
    AR_S32 sample_frames;//每个周期中的采样帧数
	AR_U32 frame_mask[MAX_SUB_SAMPLE_RATE_FRAMES/32];//控制如何在这个周期中采样，其中设置的位数必须等于sample_frames；
}STRU_CAM_PRA_STREAM_FPS_SUBSAMPLE_RATE_PRA_T;

typedef struct
{
    AR_S32 stream_id;
    AR_FLOAT fps;
}STRU_CAM_PRA_STREAM_FPS_RATE_PRA_T;

typedef struct
{
  AR_S32 stream_id;
  AR_FLOAT digital_zoom;
  AR_S32 manual;
  STRU_CAM_ROI_T roi;
}STRU_CAM_PRA_SET_STREAM_DIGITAL_ZOOM_PRA_T;

typedef STRU_CAM_PRA_SET_STREAM_DIGITAL_ZOOM_PRA_T STRU_ZOOM_PRA_T;

typedef enum
{
  HDR_MODE_FUSION,
  HDR_MODE_LONG,
  HDR_MODE_MIDDLE,
  HDR_MODE_SHORT,
  HDR_MODE_AUTO,
}ENUM_HDR_MODE_T;

typedef enum
{
   DRC_MODE_AUTO,
   DRC_MODE_OFF,
   DRC_MODE_MANU,
}ENUM_DRC_MODE_T;
typedef struct
{
  AR_FLOAT gain;
  AR_FLOAT k1;
  AR_FLOAT k2;
  AR_FLOAT k3;
}STRU_DRC_PRA_T;

typedef struct
{
  AR_S32 mode;
  STRU_DRC_PRA_T drc_pra;
}STRU_CAM_PRA_SET_DRC_MODE_PRA_T;

typedef enum{
  HDR_PRA_MODE_AUTO,
  HDR_PRA_MODE_NORMAL,
  HDR_PRA_MODE_EXPERT,
}ENUM_HDR_PRA_MODE_T;

typedef struct
{
  AR_S32 mode;
  AR_S32 max_short_exp;
  AR_FLOAT exp_ration;
  AR_FLOAT drc1_k[3];
  AR_FLOAT drc2_k[3];
}STRU_CAM_PRA_SET_HDR_PRA_MODE_PRA_T;

typedef enum
{
   CAM_PRA_ANTI_BANDING_OFF,
   CAM_PRA_ANTI_BANDING_50,
   CAM_PRA_ANTI_BANDING_60,
}ENUM_ANTIBANDING_T;


typedef struct
{
   AR_U32 fps_min;
   AR_U32 fps_max;
}STRU_CAM_PRA_SET_FPS_RANGE_PRA_T;

typedef enum
{
  LED_OFF,
  LED_LOW,
  LED_MIDDLE,
  LED_HIGH,
}ENUM_LED_CURRENT_T;

typedef enum
{
   SENSCE_MODE_AUTO,
   SENSCE_MODE_OFF,
}ENUM_SENSCE_MODE_T;

typedef enum
{
   SENSCE_NORMOL,
   SENSCE_NIGHT,
   SENSCE_BACK_LIGNT,
   SENSCE_PORTRAIT,
   SENSCE_LANDSCAPE,
   SENSCE_MOTION,
   SENSCE_FOOD,
   SENSCE_NORMOL_HDR_OFF,
}ENUM_SENSCE_T;

typedef struct
{
	AR_CHAR tuning_name[128];/**<场景的效果参数名字,必须带路径*/
}STRU_CAM_PRA_SET_SENCE_PRA_T;

typedef struct
{
	AR_S32 af_stats_data_total_horizontal_block_num;
	AR_S32 af_stats_data_total_vertical_block_num;
	AR_U32 p_af_stats_data_sharpness_addr[AF_STAT_WIDTH*AF_STAT_HEIGHT];
	AR_U32 p_af_stats_data_fir_addr[AF_STAT_WIDTH*AF_STAT_HEIGHT];
	AR_U32 p_af_stats_data_iir_addr[AF_STAT_WIDTH*AF_STAT_HEIGHT];
	AR_U32 p_af_stats_data_luma_addr[AF_STAT_WIDTH*AF_STAT_HEIGHT];
	AR_U32 p_af_stats_data_high_num_addr[AF_STAT_WIDTH*AF_STAT_HEIGHT];
} STRU_AF_ALGO_LIB_AF_STATUS_DATA_T;

typedef struct
{
	AR_S32 mesh_grid_stats_w;
	AR_S32 mesh_grid_stats_h;
	AR_U32 p_mesh_grid_stats_r_sum[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_gr_sum[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_gb_sum[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_b_sum[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_r_num[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_gr_num[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_gb_num[BLOCK_COL * BLOCK_ROW];
	AR_U32 p_mesh_grid_stats_b_num[BLOCK_COL * BLOCK_ROW];
}STRU_MESH_GRID_STATS_DATA_T;


typedef struct
{
	 AR_S32 raw_hist_bin_num;
	 AR_U32 p_raw_hist_r[128];
	 AR_U32 p_raw_hist_g[128];
	 AR_U32 p_raw_hist_b[128];
}SRTU_RAW_HIST_T;

typedef struct
{
	AR_S32 rgb_his_bin_num;
	AR_U32 p_rgb_hist[128];
}SRTU_RGB_HIST_T;

typedef struct
{
   AR_S32 low_irq_ref[32];
   AR_S32 high_irq_ref[32];
}STRU_INT_STATS_T;

typedef struct
{
   AR_S32 mesh_w;
   AR_S32 mesh_h;
   AR_U16 hist_bins[256*32*32+64];
}STRU_LTM_STATS_T;


typedef struct
{
    AR_S32 mesh_grid_w;
    AR_S32 mesh_grid_h;
    AR_U32 p_mesh_grid_stats_r_sum[AWB_NEARGRAY_SIZE];
    AR_U32 p_mesh_grid_stats_g_sum[AWB_NEARGRAY_SIZE];
    AR_U32 p_mesh_grid_stats_b_sum[AWB_NEARGRAY_SIZE];
    AR_U32 p_mesh_grid_stats_num[AWB_NEARGRAY_SIZE];
}STRU_AWB_STATS_T;

typedef struct
{
	AR_S32 width;
	AR_S32 height;
	AR_U8 motion_frame[36][16];
}STRU_MOTION_FRAME_T;


typedef struct
{
   AR_S32 cam_id;
   AR_VOID *p_stats;
   AR_VOID *p_out;
}STRU_3A_ALGO_RUN_PRA_T;


typedef struct
{
    AR_CHAR cmd[128];  //debug cmd
    AR_S32  dev_id;
    AR_S32  pipe_id;
    AR_S32  ch_id;
}STRU_VIN_DEBUG_PROC_T;


typedef struct
{
   AR_S32 cam_id; /*out pra*/
   AR_S32 frame_id;/*out pra*/
   AR_S32 mesh_w;/*out pra*/
   AR_S32 mesh_h;/*out pra*/
   AR_S32 stats_bin_num;/*out pra*/
   AR_VOID *p_stats_phy; /*in pra,the addr must be malloc by user use mmz */
}STRU_LTM_STATS_PRA_T;


typedef  union
{
	 STRU_3A_ALGO_RUN_PRA_T aec_stats_group;
	 STRU_3A_ALGO_RUN_PRA_T awb_stats_group;
	 STRU_3A_ALGO_RUN_PRA_T af_stats_group;
	 STRU_VIN_DEBUG_PROC_T  proc_data;	 
	 STRU_LTM_STATS_PRA_T  ltm_stats;
}UNION_META_DATA_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 vsync_id;
  AR_S32 meta_type;
  UNION_META_DATA_T meta_data;
}STRU_META_DATA_TYPE_T;

typedef enum{
    TUNING_REQ_READ_TUNING_PRA,
    TUNING_REQ_WRITE_TUNING_PRA,
    TUNING_REQ_LOCK_TUNING_PRA,
    TUNING_REQ_UNLOCK_TUNING_PRA,
    TUNING_REQ_GET_3A_INFO,
    TUNING_REQ_LOCK_AE,
    TUNING_REQ_UNLOCK_AE,
    TUNING_REQ_SET_MANUAL_AE,
    TUNING_REQ_LOCK_AWB,
    TUNING_REQ_UNLOCK_AWB,
    TUNING_REQ_SET_MANUAL_AWB,
    TUNING_REQ_LOCK_AF,
    TUNING_REQ_UNLOCK_AF,
    TUNING_REQ_SET_MANUAL_AF,
    TUNING_REQ_SET_AF_MOD,
    TUNING_REQ_ENDEN_ISP_MODULE,
    TUNING_REQ_SENSOR_WRITE,
    TUNING_REQ_SENSOR_READ,
    TUNING_REQ_READ_CUR_ISP_PRA,
    TUNING_REQ_GET_RES_MODE,
    TUNING_REQ_SET_TRIGGER_AF,
    TUNING_REQ_GET_ISP_MODULE_STATUS,
    TUNING_REQ_SET_UPDATE_PROP,
    TUNING_REQ_GET_UPDATE_PROP,
    TUNING_REQ_SET_CUR_ISP_TUNING_PRA,
    TUNING_REQ_GET_CUR_ISP_TUNING_PRA,
    TUNING_REQ_SET_CUR_ISP_DETA_PRA,
    TUNING_REQ_GET_CUR_ISP_DETA_PRA,
    TUNING_REQ_SAVE_TUNING_PRA_TO_BIN,
    TUNING_REQ_MAX
}ENUM_REQUEST_TYPE_T;

enum
{
  TUNING_ISP_CROP_RAW,
  TUNING_ISP_BLC,
  TUNING_ISP_HDR_MIX,
  TUNING_ISP_DIGITAL_GAIN_1,
  TUNING_ISP_CAMPANDER,
  TUNING_ISP_DIGITAL_GAIN_2,
  TUNING_ISP_DPC,
  TUNING_ISP_CAC,
  TUNING_ISP_ATA,
  TUNING_ISP_RNR,
  TUNING_ISP_DECOMPANDER,
  TUNING_ISP_LSC,
  TUNING_ISP_AWB,
  TUNING_ISP_DRC,
  TUNING_ISP_GIC,
  TUNING_ISP_CFA,
  TUNING_ISP_DPP,
  TUNING_ISP_CCM1,
  TUNING_ISP_CCM2,
  TUNING_ISP_GTM1_LUT,
  TUNING_ISP_GAMMA,
  TUNING_ISP_GTM2_LUT,
  TUNING_ISP_3DLUT,
  TUNING_ISP_RGB2YUV,
  TUNING_ISP_CM,
  TUNING_ISP_CM2,
  TUNING_ISP_CNF,
  TUNING_ISP_LNR_LEE,
  TUNING_ISP_DITHER,
  TUNING_ISP_DE3D,
  TUNING_ISP_ALGO_AEC,
  TUNING_ISP_ALGO_AWB,
  TUNING_ISP_ALGO_AF,
  TUNING_ISP_VFE,
  TUNING_ISP_HDR_LSC,
  TUNING_ISP_ACM,
  TUNING_ISP_IR_LMS,
  TUNING_ISP_IR_RAW_3DNR_SW,
  TUNING_ISP_IR_RAW_2DNR_SW,
  TUNING_ISP_IR_RAW_GTM_SW,
  TUNING_ISP_SUBMODULE_MAX,
};

typedef union
{
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
	  STRU_AEC_TUNING_T aec_tuning;
	  STRU_AWB_TUNING_T awb_tuning;
	  STRU_AF_TUNING_T	af_tuning;
	  STRU_ISP_SUB_MODULE_ISP_VFE_TUNING_T isp_sub_module_isp_isp_vfe_tuning;
	  STRU_ISP_SUB_MODULE_CM2_TUNING_T isp_sub_module_cm2_tuning;
	  STRU_ISP_SUB_MODULE_LSC_TUNING_T isp_sub_module_hdr_lsc_tuning;
	  STRU_ISP_SUB_MODULE_ACM_TUNING_T isp_sub_module_acm_tuning;
	  STRU_ISP_SUB_MODULE_IR_LMS_TUNING_T isp_sub_module_ir_lms_tuning;
	  STRU_ISP_SW_ALGO_IR_RAW_3DNR_TUNING_T isp_sw_algo_ir_raw_3dnr_tuning;
      STRU_ISP_SW_ALGO_IR_RAW_2DNR_TUNING_T isp_sw_algo_ir_raw_2dnr_tuning;
	  STRU_ISP_SW_ALGO_IR_RAW_GTM_TUNING_T isp_sw_algo_ir_raw_gtm_tuning;
}UNION_TUNINIG_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 tuning_type;
   AR_S32 len;
   UNION_TUNINIG_T tuning;
}STRU_TUNING_REQ_READ_TUNING_ACK_T;

typedef union
{
	   STRU_ISP_SUB_MODULE_BLC_PARA isp_sub_module_blc_tuning;
	   STRU_ISP_SUB_MODULE_HDR_MIX_PRA isp_sub_module_hdr_mix_tuning;
	   STRU_ISP_SUB_MODULE_COMPANDER_PRA isp_sub_module_compander_tuning;
	   STRU_ISP_SUB_MODULE_DPC_PRA	isp_sub_module_dpc_tuning;
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
	   STRU_AWB_STATISTICS				awb_stats_tuning;
	   STRU_ISP_SUB_MODULE_ISP_VFE_PRA isp_sub_module_isp_isp_vfe_tuning;
	   STRU_ISP_SUB_MODULE_CM2_PRA isp_sub_module_cm2_tuning;
	   STRU_ISP_SUB_MODULE_LSC_PRA isp_sub_module_hdr_lsc_tuning;
	   STRU_ISP_SUB_MODULE_ACM_PRA isp_sub_module_acm_tuning;
	   STRU_ISP_SUB_MODULE_IR_LMS_PRA isp_sub_module_ir_lms_tuning;
	   STRU_ISP_SW_ALGO_IR_RAW_3DNR_PRA isp_sw_algo_ir_raw_3dnr_tuning;
       STRU_ISP_SW_ALGO_IR_RAW_2DNR_PRA isp_sw_algo_ir_raw_2dnr_tuning;
	   STRU_ISP_SW_ALGO_IR_RAW_GTM_PRA isp_sw_algo_ir_raw_gtm_tuning;
 }UNION_TUING_DATA_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 tuning_type;
   AR_S32 len;
   UNION_TUING_DATA_T tuning;
}STRU_TUNING_REQ_READ_CUR_ISP_ACK_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 tuning_type;
   AR_S32 len;
   UNION_TUING_DATA_T tuning;
}STRU_TUNING_REQ_WRITE_CUR_ISP_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 tuning_type;
   AR_S32 len;
   UNION_TUNINIG_T tuning;
}STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T;

typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 force_type;
}STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T;
typedef struct
{
   AR_S32 request_type;
   STRU_ALGO_3A_GET_INFO_T get_3a_info;
}STRU_TUNING_REQ_GET_3A_INFO_ACK_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_LOCK_AE_PRA_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_UNLOCK_AE_PRA_T;
typedef struct
{
   AR_S32 request_type;
   STRU_AEC_SET_INFO_T info;
}STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T;
typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_LOCK_AWB_PRA_T;
typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_UNLOCK_AWB_PRA_T;

typedef struct
{
   AR_S32 request_type;
   STRU_AWB_SET_INFO_T info;
}STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_LOCK_AF_PRA_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_UNLOCK_AF_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 pos;
}STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 af_mode;
}STRU_TUNING_REQ_SET_AF_MOD_PRA_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_TRIGGER_AF_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 isp_module;
   AR_S32 en_flag;//0 disable // 1 enable
}STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T;
#define MAX_REG_NUM 512
typedef struct
{
   AR_S32 request_type;
   AR_S32 reg_num;
   AR_U32 address[MAX_REG_NUM];
   AR_U32 data[MAX_REG_NUM];
}STRU_TUNING_REQ_SENSOR_WRITE_PRA_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 reg_num;
   AR_U32 address[MAX_REG_NUM];
   AR_U32 data[MAX_REG_NUM];
}STRU_TUNING_REQ_SENSOR_READ_ACK_T;

typedef struct
{
   AR_S32 request_type;
   AR_CHAR name[128];
}STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T;


typedef union
{
	STRU_TUNING_REQ_READ_TUNING_ACK_T read_tuning_pra;
	STRU_TUNING_REQ_WRITE_TUNING_PRA_PRA_T write_tuning_pra;
	STRU_TUNING_REQ_LOCK_TUNING_PRA_PRA_T lock_tuning_pra;
	STRU_TUNING_REQ_UNLOCK_TUNING_PRA_PRA_T unlock_tuning_pra;
	STRU_TUNING_REQ_GET_3A_INFO_ACK_T get_3a_info_pra;
	STRU_TUNING_REQ_LOCK_AE_PRA_T lock_ae_pra;
	STRU_TUNING_REQ_UNLOCK_AE_PRA_T unlock_ae_pra;
	STRU_TUNING_REQ_SET_MANUAL_AE_PRA_T set_man_ae_pra;
	STRU_TUNING_REQ_LOCK_AWB_PRA_T lock_awb_pra;
	STRU_TUNING_REQ_UNLOCK_AWB_PRA_T unlock_awb_pra;
	STRU_TUNING_REQ_SET_MANUAL_AWB_PRA_T set_awb_pra;
	STRU_TUNING_REQ_LOCK_AF_PRA_T lock_af_pra;
	STRU_TUNING_REQ_UNLOCK_AF_PRA_T unlock_af_pra;
	STRU_TUNING_REQ_SET_MANUAL_AF_PRA_T set_man_af_pra;
	STRU_TUNING_REQ_SET_AF_MOD_PRA_T set_af_mod_pra;
	STRU_TUNING_REQ_ENDEN_ISP_MODULE_PRA_T enden_isp_module_pra;
	STRU_TUNING_REQ_SENSOR_WRITE_PRA_T	  write_sensor;
	STRU_TUNING_REQ_SENSOR_READ_ACK_T	  read_sensor;
	STRU_TUNING_REQ_READ_CUR_ISP_ACK_T read_cur_isp_pra;
	STRU_TUNING_REQ_WRITE_CUR_ISP_T cur_isp_pra_wr;
	STRU_TUNING_REQ_WRITE_CUR_ISP_T cur_isp_pra_wr_deta;
	STRU_TUNING_REQ_TRIGGER_AF_PRA_T trigger_af_pra;
	STRU_TUNING_REQ_UPDATE_PROP_T  update_prop;
	STRU_TUNING_REQ_SAVE_TUNING_PRA_TO_BIN_T save;
}UNION_TUNING_REQ_DATA_T;

typedef struct
{
  AR_S32 cam_id;
  AR_S32 req_type;
  AR_S32 len;
  UNION_TUNING_REQ_DATA_T req_pra;
}STRU_CAM_SET_TUNINIG_REQUEST_PRA_T;

typedef STRU_CAM_SET_TUNINIG_REQUEST_PRA_T STRU_CAM_SET_TUNING_REQUEST_T;

/*
*@brief:描述一个buffer的pannel 类型
*@param:
buffer	指向buffer 数据的首地址
buffer_orign	指向buffer的数据的首地址的原始地址，原始地址向512字节对其后就是数据首地址
fd	Mem 对应的ion fd
va;	Mem 对应的虚拟地址
length	Mem的长度
line_buffer_len	Mem的一行的长度
used	有效使用的mem的长度=line_buffer_len*height
*/
typedef struct
{
  AR_VOID * buffer;
  AR_VOID * buffer_orign;
  AR_VOID * buffer_phy_orign;
  AR_VOID * buffer_phy_addr;
  AR_U32 length;
  AR_U32 line_buffer_len;
  AR_U32 used;
  AR_U32 offset;
  AR_U64 priv[8];       // pannel[0]'s priv[0] and priv[1] are occupied by video supplement address, please don't use them.
}STRU_PANNEL_T;


typedef enum
{
   BUFFER_TYPE_NORMAL,
   BUFFER_TYPE_META,
}ENUM_BUFFER_TYPE_T;;

typedef enum
{
   INTERLACE_MODE_NULL,
   INTERLACE_MODE_ODD,
   INTERLACE_MODE_EVEN,
   INTERLACE_MODE_TYPE_INDICATE=0XFFFFFFFF,
}ENUM_INTERLACE_MODE_T;


/*
*@brief:stream buffer的信息的结构。表征了某个stream 上取到的buffer的信息。
*@param:

buffer_index	Stream buffer 在vin 系统中的index，这个值用户不可自行改变
必须是request stream buffer 后得到的值
frame_id	表示这个buffer的id
vsync_id	Buffer 对应的vsync 中断的计数id
current_fps	Buffer对应的stream的帧速率
buffer_type_flag	todo
lowdelay_index	标志lowdelay内部hw 翻转标志
interlace_mode	Fame 的interlace 模式，区分奇场或偶场
timestamps_vsync	Frame的vsync的时候的事件戳
timestamps	Frame的done的时候的时间戳
stats_type	如果是stats buffer，表示统计类型，比如rro af等
panel_count	Frame的panel 数目
meta_panel_index	如果大于等于0， 表示meta 数据存在于那个pannel中，比如eis 数据可能存在于pannel 3中
pannel	存放mem 描述的信息
data	Buffer的私有数据

*/

typedef struct
{
  AR_S32 buffer_index; //the index of stream_buffer_t
  AR_S32 x;
  AR_S32 y;
  AR_S32 width;
  AR_S32 height;
  AR_S32 total_width;
  AR_S32 total_height;
  AR_S32 format;
  AR_U32 frame_id;
  AR_U32 vsync_id;
  AR_FLOAT    current_fps;
  AR_U32 buffer_type_flag;
  AR_U32 lowdelay_index;
  ENUM_INTERLACE_MODE_T interlace_mode;
  AR_U64 timestamps_vsync;
  AR_U64 timestamps;
  AR_U32 stats_type;
  AR_S32 panel_count;
  AR_S32 meta_panel_index;
  STRU_PANNEL_T pannel[6];
  STRU_BASIC_3A_INFO_T basic_info;
  AR_S32 buffer_dirty;
  //this can used as priviate data
  AR_VOID *data[8];
}STRU_STREAM_BUFFER_INIFO_T;

typedef struct
{
    AR_S32 event_id;  //the event info's buffer index,user do not change it
    AR_S32 event_code;
    AR_S32 event_size;
	AR_VOID *event;
	AR_VOID *event_phy;
}STRU_AR_EVENT_INFO_T;

typedef struct
{
    AR_S32 stream_id;
    AR_S32 num;
    STRU_STREAM_BUFFER_INIFO_T buf_info[8];
}STRU_CAM_REQUSET_STREAM_BUFFER_INFO_PRA_T;

typedef struct
{
    AR_S32 stream_id;
    STRU_STREAM_BUFFER_INIFO_T buf_info;
}STRU_CAM_Q_DQ_STREAM_BUFFER_INFO_PRA_T;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 time_out_ms;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
}STRU_CAM_PRA_GET_PIPE_BUFFER_T;


//app alloc all the stream object and send to server to config it
//when config completed, some value will be filled by server.

typedef enum
{
   USR_STREAM_OFF,
   USR_STREAM_ON,
   USR_STREAM_SUSPEND,
}ENUM_USR_STREAM_T;


typedef struct
{
  AR_S32 link_num;
  AR_CHAR src_filter_name[64];
  AR_CHAR dst_filter_name[64];
  AR_S32 src_port_no;
  AR_S32 dst_port_no;
}STRU_LINK_PRA_T;

typedef enum
{
    NOTIFY_EVENT_ERR=NOTIFY_START*(1<<8),
    NOTIFY_EVENT_SHUTTER,
    NOTIFY_EVENT_FOCUS_STATUS,
    NOTIFY_EVENT_CONNECT,
    NOTIFY_EVENT_DISCONNECT,
    NOTIFY_EVENT_FORAMAT_CHANGE,
    NOTIFY_EVENT_SENSCE_CHANGED,
    NOTIFY_EVENT_UNSUPPORTED,
    NOTIFY_EVENT_STREAM_TIMEOUT,
    NOTIFY_EVENT_MIPI_ERR,
    NOTIFY_EVENT_SOF,
    NOTIFY_EVENT_AEC_UPDATE,
    NOTIFY_EVENT_AWB_UPDATE,
    NOTIFY_EVENT_AF_UPDATE,
    NOTIFY_EVENT_NOBUFFER,
    NOTIFY_EVENT_STREAM_BUFFER_CFG,
    NOTIFY_EVENT_MAX,
}notify_t;

typedef struct
{
	int camera_id;
}STRU_NOTIFY_EVENT_SHUTTER_PRA_T;

typedef struct
{
	int camera_id;
	int stream_id;
}STRU_NOTIFY_EVENT_STREAM_TIMEOUT_PRA_T;

typedef struct
{
	int camera_id;
	int stream_id;
}STRU_NOTIFY_EVENT_NOBUFFER_PRA_T;
typedef struct
{
	int camera_id;
	int stream_id;
}STRU_NOTIFY_EVENT_STREAM_BUFFER_CFG_PRA_T;

typedef struct
{
    int err_lane[4];
    int skew_val[4];
}STRU_SKEW_INFO_T;

typedef struct
{
	int camera_id;
	STRU_SKEW_INFO_T skew;
}STRU_NOTIFY_EVENT_MIPI_ERR_PRA_T;

typedef struct
{
	int camera_id;
    int err_code;
}STRU_NOTIFY_EVENT_ERR_PRA_T;

typedef enum
{
    FOCUS_STATUS_IDLE=FOCUS_STATUS_START*(1<<8),
    FOCUS_STATUS_START_FOCUS,
    FOCUS_STATUS_RUNING,
    FOCUS_STATUS_SUCUCESS,
    FOCUS_STATUS_MONITOR,
    FOCUS_STATUS_FAILED,
}ENUM_STATUS_T;

typedef struct
{
   AR_S32 camera_id;
   ENUM_STATUS_T e_focus_status;
}NOTIFY_EVENT_FOCUS_STATUS_PRA_T;

typedef struct
{
  AR_U32 camera_id;
  AR_U32 sof_count;
}STRU_NOTIFY_EVENT_SOF_PRA_T;


typedef enum
{
 DATA_HDMI_SRC_AUDIO,
 DATA_HDMI_SRC_VIDEO,
 DATA_HDMI_SRC_AUDIO_VIDEO,
 DATA_MIPI_SRC,
 DATA_DVP_SRC,
}ENUM_NOTIFY_DATA_SRC_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 sence_mask;
}STRU_NOTIFY_EVENT_SENSCE_CHANGED_PRAT;

typedef struct {
    AR_U32 format; /* refer to different color space definition in cam/ar_system */
    AR_U32 width;
    AR_U32 height;
    AR_U32 fps_num;
    AR_U32 fps_den;
	AR_U32 luma_stride;
	AR_U32 chroma_stride;
	AR_U32 is_interlace;
	AR_U32 color_mode;
	AR_U32 color_imetry;
	AR_U32 extended_color_imetry;
	AR_U32 rgb_quantization_range;
	AR_U32 ycc_quantization_range;
	AR_U32 dvp_type;//0:DVP_BT601,1:DVP_BT656,2:DVP_BT1120
	AR_U32 emb_sync;//0:sep sync 1:emb sync
	AR_U32 data_swap;//0:no swap 1:swap	
    AR_U32 matrix;
    AR_U32 origin_color_fmt;//such as fmt of hdmirx before csc in vif
}STRU_VIDEO_FORAMT_NOTITY_PRA_T;

typedef struct
{
    AR_U32 format;
    AR_U32 rate;
    AR_U32 channels;	
    AR_U32 bits_per_sample;
} STRU_AUDIO_FORAMT_NOTITY_PRA_T;


typedef struct
{
    AR_S32 camera_id;
	AR_S32 is_connected;
    ENUM_NOTIFY_DATA_SRC_T e_data_src; //0 audio ,1 video, 2 audio&&video
    STRU_VIDEO_FORAMT_NOTITY_PRA_T video_format;
    STRU_AUDIO_FORAMT_NOTITY_PRA_T audio_format;
    AR_U32 data[4];
}STRU_NOTIFY_EVENT_DATA_SRC_PRA_T;


// the roi use the 1 as max and 0 as min
//such as
/*
   {
     0.2,
     0.2,
     0.4,
     0.4,
   }
   if the picture is 100x100
   so the roi is
   20,20, 40,40
*/


typedef struct
{
   AR_S32 face_count;
   STRU_CAM_ROI_T face_roi[16]; //max 16 face
   AR_FLOAT face_weight;
   AR_S32  face_keep_frame_count;
   AR_S32  face_target;
}STRU_FACE_AEC_T;


typedef enum
{
 SING_CAMERA_LOW_DELAY=CAM_WORK_MODE_STAT*(1<<8),
 SING_CAMERA_OFFLINE,
 SING_CAMERA_NORMAL,
 DUAL_CAMERA_NORMAL,
 MUILTI_CMAERA_MODE,
 SING_CAMERA_STILL,
}ENUM_CAM_WORK_MODE_T;

/*
   pre pra is set with one structure but more pra.
   so when you set the pre pra, you need first get the last pra ,and modify some of it and
   to set. so in your app, you need save a pre pra in your app, because this pra can not be get
   by api, and the pre pra only be set before the stream is connected.
*/


typedef enum{
    //for this mode, the vif send the data to isp process uint0.
    VIF2ISP_TRANSFER_DIRECT = 0,
    VIF2ISP_TRANSFER_DIRECT_CORE1,
    //for this mode, the vif mix the two sensor data by linebylne and send to isp, isp seprate it and process it with uinit0 and unit1
    VIF2ISP_TRANSFER_DIRECT_LR,
    //the isp fetch the ddr data to isp unit0 gernerally
    VIF2ISP_TRANSFER_DDR,
    VIF2ISP_TRANSFER_DDR_CORE1,
    //the isp fetch the ddr data and the left to unit0 and right to unit2.
    VIF2ISP_TRANSFER_DDR_LR,
    VIF2ISP_TRANSFER_MAX,
}ENUM_VIF2ISP_TRANSFER_MODE_T;


typedef enum
{
   VIN_FRE_MODE_AUTO,
   VIN_FRE_MODE_MAN,
}ENUM_VIN_FRE_MODE_T;

typedef enum
{
   VIN_HW_BURST_4=4,
   VIN_HW_BURST_8=8,
   VIN_HW_BURST_16=16,
   VIN_HW_BURST_INVALIDE=0,
   VIN_HW_BURST_TYPE_INDICATTE=0XFFFFFFFF,
}ENUM_VIN_HW_BURST_T;
/*
 * @brief
cam_mode	Vin 工作模式，即isp的工作模式，主要用来设置是 online offline 及多camera 模式。当isp 工作在多camera模式的时候，isp 处于分时复用状态。
isp_used	是否使用isp，一般设置为1
vif_isp_mode	表明vif 和 isp 的关系，一遍有online offline 两种，online就是vif 的数据通过行存的形式直接给isp，也叫直联模式，linebuffer 模式， 而offline 就是vif 写到 ddr ，然后isp 主动取fetch的模式，也叫ddr模式。
vif_fre_mod	0：auto 1：由本参数控制。
vif_fre_hz	Vif hw的运行频率。单位是hz，比如500000000hz，500M
mipi_fre_mod	同上
mipi_fre_hz	同上
isp_fre_mod	同上
isp_fre_hz	同上
hdr_fre_mod	同上
hdr_fre_hz	同上
eis_fre_mod	同上
eis_fre_hz	同上
server_priv_data_len	Vin 驱动，或vin 服务的私有数据长度
server_priv_data	Vin 驱动，或vin 服务的私有数据，用户可以用来保存一些多进程数据
*/

typedef struct
{
	ENUM_CAM_WORK_MODE_T cam_mode;
	AR_U32 isp_used;
	ENUM_VIF2ISP_TRANSFER_MODE_T vif_isp_mode;
	AR_U32 eis_ldc_mode; //0: offline 1 online
	ENUM_VIN_FRE_MODE_T vif_fre_mod; //0: auto mode, 1: ctl by this cfg
	AR_S32 vif_fre_hz;
	ENUM_VIN_FRE_MODE_T mipi_fre_mod; //0: auto mode, 1: ctl by this cfg
	AR_S32 mipi_fre_hz;	
	ENUM_VIN_FRE_MODE_T isp_fre_mod; //0: auto mode, 1: ctl by this cfg
	AR_S32 isp_fre_hz;
	ENUM_VIN_FRE_MODE_T hdr_fre_mod; //0: auto mode, 1: ctl by this cfg
	AR_S32 hdr_fre_hz;
	ENUM_VIN_FRE_MODE_T eis_fre_mod; //0: auto mode, 1: ctl by this cfg
	AR_S32 eis_fre_hz;	
	ENUM_VIN_HW_BURST_T hw_burst;
	ENUM_VIN_HW_BURST_T isp_burst;
	AR_S32 server_priv_data_len;
	AR_VOID *server_priv_data;
}STRU_CAM_SERVER_GLOBAL_PRA_T;

/*
*@brief:
描述插槽的结构，所谓slot，是hw 板卡上的具体的连接器。正如pc的pci 插槽一样的概念。Slot的成员的组合定义了slot 本身，比如i2c index，gpio。等等。
Slot 也可以是虚拟的，也就是不对应任何的物理上的slot，比如定义isp in设备的slot。这个可以通过slot type来区分。
Slot 也可以是抽象的，这是因为抽象的slot 虽然不能具体的和hw 对应，但是可以通过sensor cb 函数具体化。抽象的slot 也是通过slot type 来定义的。


slot_name	Slot的名字
slot_type	Slot的类型
reset_gpio	描述reset gpio的数组，数组依次是
padnum, gpio_function_num ,group, port,num,in_out
power_gpio	同上
mipi_index	本slot的mipi index，比如mipi0 就是 0
i2c_index	I2c 的index

*/

typedef struct
{
    AR_CHAR slot_name[64];
    AR_S32 slot_type;;
    //padnum, gpio_function_num ,group, port,num,in_out
    AR_S32 reset_gpio[6];
    AR_S32 power_gpio[6];
    AR_S32 mipi_index;
    //i2c index
    AR_S32 i2c_index;
	AR_S32 mclk_index; //0 1 2 3, if -1 will use the external clk
}STRU_SENSOR_SLOT_T;


typedef enum
{
   SENSOR_DEV_HW_SENSOR_DEV,
   SENSOR_DEV_COMMON_DEV,
   SENSOR_DEV_ISP_IN_DEV,
}ENUM_SENSOR_DEV_TYPE_T;
/*
*@brief:
dev_name	本sensor 设备的名字
driver_name	本sensor 设备需要使用的驱动程序的名字
tuning_name	本sensor 设备需要使用的tuning 参数一级目录的名字
slot	本sensor 设备插在系统主板的那个插槽上
sensor_slave_addr_7bits	本sensor 设备本身的i2c的地址
need_master_slave_sel	本sensor 设备slave maser的标识 0：master 1：slave
device_mode	本设备的模式，主要是指描述core 设备，或子设备
device_type	本sensor 设备的设备模式，有sensor hw 设备，通用抽象设备，及isp in 设备
slave_num	如果本设备室core或者是master， 对应的slave 设备数量
master_sensor	如果本设备是slave 设备，那么对应的master 设备的名字
slave_sensor	如果本设备是master 设备，拥有的slave 设备的名字组
*/

typedef enum
{
   SENSOR_MCLK_24M=24000,
   SENSOR_MCLK_27M=27000,
   SENSOR_MCLK_37_125M=37125,
   SENSOR_MCLK_74_25M=74250,
}ENUM_SENSOR_MCLK_T;


typedef struct
{
   AR_CHAR dev_name[64];
   AR_CHAR driver_name[64];
   AR_CHAR actuator_name[64];
   STRU_SENSOR_SLOT_T slot;
   AR_S32 sensor_slave_addr_7bits;
   AR_S32 i2c_speed;
   ENUM_SENSOR_MCLK_T e_mclk_khz;
   AR_S32 mipi_lane_count;
   AR_S32 need_master_slave_sel;
   AR_S32 device_mode;
   AR_S32 device_type; //commondev  isp_indev etc
   AR_S32 slave_num;
   AR_CHAR master_sensor[64];
   AR_CHAR slave_sensor[4][64];
}STRU_SENSOR_DEV_T;

typedef struct
{
   AR_S32 flag;
   STRU_SENSOR_DEV_T sensor_dev;
}STRU_SENSOR_DEV_REGISTER_T;


typedef struct
{
  AR_CHAR dev_name[64];
  AR_CHAR driver_name[64];
  AR_S32  i2c_index;
}STRU_ACTUATOR_DEV_T;


typedef struct
{
   AR_S32 flag;
   STRU_ACTUATOR_DEV_T actuator_dev;
}STRU_ACTUATOR_DEV_REGISTER_T;


typedef struct
{
  AR_S32 ret;
  AR_S32 dac_value;
  STRU_DAMPING_T  damping;
}STRU_MOVE_FOCUS_T;


typedef struct
{
 AR_S32 awb_valid;
 AR_S32 lsc_valid;
 AR_S32 awb_gold_valid;
 AR_S32 lsc_gold_valid;

 AR_U16 awb_gold_r_gr_gb_b[4]; 
 AR_U16 awb_r_gr_gb_b[4];

 AR_U16 lsc_gold[300];  
 AR_U16 lsc[300];
}STRU_OTP_CALLIBRATION_DATA_T;

/*
*@brief:
is_init	启动选项是否被初始化
vif_buf_count	这个参数控制vif的buffer 数目，在9301的系统中，vif 送出来的数据可以直接给isp，
也可以先缓存到ddr 中，然后isp 再从isp 中取数据。直接送给isp的方式叫linebuffer 模式，direct 模式，
online 模式，后者叫ddr 模式,, offline 模式。而这个参数就是配置offline 模式下，vif 写ddr 的缓存的个数。
一般来讲，最低3 个，因为vif 自己正在写的一个，提前配置一个，送给isp 一个。在内存充裕的情况下，
建议配置5个，紧张的情况下4个，极限情况下3个。
isp_buf_count;	这个参数和vif_buf_count类似，用来设置当eis 单元为offlne 模式下情况下，本参数生效。默认配置为4个。
如何设置eis offline ，及什么情况下使用eis offline。 请参考open_prop
vfe_buf_count;	本参数在isp的pipeline 中使用vfe 的时候使用，即vfe 输出缓存的buffer 数目。
vfe 模块的使用是在soft face stats的情况下。
aec_lib_load_start 这个参数控制，是否在驱动启动的时候加载aec lib，1：的情况下，驱动加载的时候加载aec lib，
加载哪一个lib 由dts 决定，0 ：不加载aeclib。会根据用户注册的lib 进行加载
awb_lib_load_start	同上
af_lib_load_start	同上
max_sensor_sim_count	 sensor sim ，sensor 模拟设备数量，sensor sim 不是真的sensor，本身不会有hw 出图，
打开这个dev ，需要用户推一个raw图过来。主要用来模拟sensor 行为，例如构建仿真环境。或者用户打开实际的sensor 设备，
获取raw ，然后推到sensor sim的case。
max_conmmon_dev_count	 本参数设置驱动支持的最大的通用sensor dev 的个数，在这个设备下，
驱动程序本身不会操作sensor hw， 需要用户设置回调函数来进行具体sensor的绑定
max_hw_sensor_count	支持的最大的sensor 设备数。这个参数配置为max_common_dev_count+ max_sensor_sim_count+ 本身系统探测到的个数。
一个最大限制，可以配置的大一些，但不要小了。
en_invalide_hw_addr	当系统没有buffer的时候，hw 将要写buffer 的位置，1： 写到一个soc 预留的axi 地址空间，
0 写到一块开机revert的ddr 里面。对于这个参数，一般配置为0，不建议配置为1
no_buffer_addr_size	当en_invalide_hw_addr 为0的时候，保留的ddr 的空间大小。
按照系统将要使用的最大的sensor 分辨率配置。 一般配置为4k size的sensor 即 4096*2160*2
probe_dev_by_start_opt	Not used
sensor_dev_count	注册到vin的sensor设备的数目
sensor_dev[16];	保存的注册的sensor 设备
*/

typedef struct
{
  AR_S32 is_init;
  AR_U32 tuning_version;
  AR_S32 vif_buf_count;
  AR_S32 isp_buf_count;
  AR_S32 vfe_buf_count;
  AR_S32 aec_lib_load_start; // 1 : when start the server, load the aec lib with dts,   0 ,not load with dts
  AR_S32 awb_lib_load_start; // 1 : when start the server, load the aec lib with dts,   0 ,not load with dts
  AR_S32 af_lib_load_start; // 1 : when start the server, load the aec lib with dts,   0 ,not load with dts
  AR_S32 max_dev_num;        // max dev num of system .
  AR_S32 max_stream_per_dev;
  AR_S32 en_invalide_hw_addr;
  AR_S32 max_w; //when isp no buffer,the hw will write to a fix addr  
  AR_S32 max_h; //when isp no buffer,the hw will write to a fix addr
  AR_S32 sensor_dev_count;
  STRU_SENSOR_DEV_REGISTER_T sensor_dev[16];
  AR_S32 actuator_dev_count;
  STRU_ACTUATOR_DEV_REGISTER_T actuator_dev[8];
  AR_S32 event_inf_count;
  AR_S32 event_data_pool_count;
  AR_S32 event_data_max_size;
  AR_S32 tuning_req_pool_count;
  AR_S32 ddr_rw_burst_len;
  AR_S32 mem_over_debug_en;
  AR_S32 mem_over_debug_en_bug_on;
  AR_CHAR aec_algo_name[64];
  AR_CHAR af_algo_name[64];
  AR_CHAR awb_algo_name[64];
  AR_CHAR eis_algo_name[64];
  AR_S32 display_ui_mode;
  AR_S32 enable_register_mipi_irq;
  AR_S32 enable_mipi_irq_only_once;
  AR_S32 enable_mipi_reg_dump_when_mipi_err;
  AR_S32 force_af_stats;   /**<@note 16 版本后，此属性不再使用，如果只想使用af 统计，不运行系统个af，算法，请使用pipe的手动af 算法属性，用户af 算法属性，并打开tuning 中的af 功能*/
  AR_S32 dvp_rx_out_mask;
  AR_S32 dvp_scaler_out_mask;
  AR_S32 vif_out_mask;
  AR_S32 isp_out_hw_burst;
  AR_S32 use_remote_ltm_algo;
  AR_S32 share_stats_buffer;
  AR_S32 enable_hardware_irq_print;
  AR_S32 max_stream_buffer_count;  
  AR_S32 nr3d_use_ping_pong_buf;
  AR_S32 nr3d_md_use_ping_pong_buf;
  AR_S32 visual_isp_count;
  AR_S32 ltm_max_mesh_w;
  AR_S32 ltm_max_mesh_h;
  AR_S32 ltm_lut_pool_count;
  AR_S32 vsync_update_reg;
  AR_S32 wait_all_isp_done;  
  AR_S32 use_stats_pingpong;  
  AR_S32 check_ltm_stats;  
  AR_S32 hardware_mask;
  AR_S32 filter_mask;
  AR_S32 isp_use_loop_mem;  
  AR_S32 isp_use_done_cfg_buffer;
  AR_S32 max_pipe_num;
  AR_S32 max_ch_num;  
  AR_S32 max_event_client;
  AR_S32 use_vsync_trigger_aec;
  AR_S32 use_isp_irq_optimization;  
  AR_S32 use_vsync_3a_clc;
  AR_S32 mem_256_pool_size;
  AR_S32 disable_seq_queue;
  AR_S32 use_dpcm_bits_clc_stride;
  AR_S32 use_dma_upate_reg;
  AR_S32 ir_algo_split_thread;      // 1: split up ir algorithm 3dnr, 2dnr and gtm into 3 threads;  0: run ir 3dnr, 2dnr and gtm in a single thread  
  AR_S32 mipi_out_mask;             //用来屏蔽掉mipi输出，每一个bitg对应一个mipi的输出，最多8个输出
  AR_S32 vif_use_ddr_done_irq;      //是否使用vif的done中断送出帧 
  AR_S32 delay_before_isp_power_off_ms;/**<延迟时间，在行存模式下，isp poweroff 前需要加一些delay。默认值100ms。 优化启动速度是可以设置为0*/
  AR_S32 use_isp_sof_reset;
  AR_S32 all_mipi_use_same_fre;
  AR_S32 vif_use_video_stable;
  AR_S32 use_dma_flush_cache;
  AR_S32 vin_analyze_exec_time;
  AR_S32 use_sensor_init_async;
  AR_S32 use_hw_interlace;
  AR_S32 use_mipi_only_skew_int;	
}STRU_CAM_SERVER_START_OPT_T;


/*
*@brief:
描述pipe 属性的结构
1 关于lowdelay 模式： 一般来讲，当isp 把一个buffer 写到内存后才通知用户这个buffer 可以用了，而lowdealy模式是这样的一个模式，当hw 开始写这个buffer的时候，就通知用户可以使用这个buffer了。从而用户可以提前一个frame 使用buffer，故叫做lowdelay模式。显然的，这个模式下，用户需要很小心的使用，否则就会使用导致buffer使用的越界。
2 关于softeare face stats，在sdk 中，人脸的统计有两个模块，1 个是软件，1 个是hw。
  在9201 中，必须使用软件，否则会产生错误，在9301 中请使用hw
3 手动分辨率选择，如果用户对sensor的驱动程序足够了解，那么用户可以指定sensor 使用那一组分辨率setting
4 触发模式，有些sensor，可以通过控制寄存器使其出若干frames。这个feature 表示，如果进入触发模式，stream on 后，sensor 是不出图的，需要用户主动的调用相关api 取触发sensor 出图
5 用户处理raw，在某些情况下，用户想要拿到raw ，然后做一些处理，然后再发送给raw。
  这个标志表示，需要isp 发送一些事件给bind的raw dev，比如aec 事件
6 callbakc 模式和api 模式，对于数据和vin的系统通知，可以选择使用回调的模式或者api的模式，也就是异步的模式或同步的模式，但不可以同时用

*@param:
low_delay_sink	Hw lowdelay模式下，sink 硬件模块 0：h265， 1：h264 @note 不使用
sensor_hdr	是否使能sensor hdr 模式
eis_supported	是否使能eis
use_face_software_stats	使能软件face stats
need_sync	reverted
vfe_mode	reverted
man_res_mask	指定分辨率index， 第四位表示分辨率，bit4 表示使能手动选择分辨率
trigger_mode	Sensor的触发模式，0：auto 1：需要用户调用api 来触发sensor 运行
en_user_process_raw	用户是否要干预raw。
max_w;	想要选择的最大宽
max_h	想要选择的最大高
max_fps	最大fps
get_buffer_mode	0：callback 模式，1：api 模式
get_event_mode	0：callback 模式，1：api 模式

*/
typedef struct
{
  AR_U32 low_delay_sink;
  AR_U32 sensor_hdr;
  AR_U32 eis_supported;
  AR_U32 use_face_software_stats;
  AR_U32 need_sync;
  AR_U32 vfe_mode;
  AR_U32 man_res_mask;
  AR_U32 trigger_mode; //0: auto, 1 when stream on, we need call trigger to ctl the sensor to out 1 or mulit frames
  AR_U32 sof_mode; // 0: auto, 1: manual,  higher priority than sof_mode in ar_camera_sof_prop_t
  AR_S32 en_user_process_raw; //1: enable user use raw, in this mode, usr must bind the cam_id to isp_id, so isp event can auto  trigger to sensor
  AR_S32 max_w[4];  // For multi-vc cases, max_w[0]/[1]/[2]/[3] indicate vc-0/1/2/3 respectively. Otherwise, use max_w[0] only.
  AR_S32 max_h[4];  // For multi-vc cases, max_h[0]/[1]/[2]/[3] indicate vc-0/1/2/3 respectively. Otherwise, use max_h[0] only.
  AR_FLOAT max_fps[4];  // For multi-vc cases, max_fps[0]/[1]/[2]/[3] indicate vc-0/1/2/3 respectively. Otherwise, use max_fps[0] only.
  AR_S32 get_buffer_mode; //0 callbck mode 1: api_mode
  AR_S32 get_event_mode; //0 callbck mode 1: api_mode
  AR_S32 enable_man_aec_update;
  AR_S32 enable_man_awb_update;
  AR_S32 enable_man_af_update;  
  AR_S32 enable_usr_aec;
  AR_S32 enable_usr_awb;
  AR_S32 enable_usr_af;
  AR_S32 use_usr_ltm_algo;
}STRU_CAM_API_PRE_PRA_T;

typedef STRU_CAM_API_PRE_PRA_T STRU_OPEN_PROPERTY_T;

typedef struct
{
 AR_S32 cam_id;
 STRU_OPEN_PROPERTY_T pre_pra;
}STRU_CAM_SET_PRE_PRA_PRA_T;

typedef STRU_CAM_PRA_SET_SENCE_PRA_T STRU_SENSE_INFO_T;
typedef STRU_CAM_PRA_SET_FPS_RANGE_PRA_T STRU_FPS_INFO_T;
typedef STRU_CAM_PRA_SET_HDR_PRA_MODE_PRA_T STRU_HDR_PRA_MODE_T;

typedef enum
{
  I2C_ADDR_TYPE_UINT8,
  I2C_ADDR_TYPE_UINT16,
  I2C_ADDR_TYPE_UINT32,
  I2C_ADDR_TYPE_DEFAULT,//for this  add type . the i2c not need to send a gigister address to sensor or other device
  I2C_ADDR_TYPE_MAX,
}STRU_I2C_ADDRESS_TYPE_T;

typedef enum
{
  I2C_DATA_TYPE_UINT8,
  I2C_DATA_TYPE_UINT16,
  I2C_DATA_TYPE_UINT32,
  I2C_DATA_TYPE_MAX,
}STRU_I2C_DATA_TYPE_T;

typedef enum
{
 REGISTER_OP_WRITE=REGISTER_OP_START*(1<<8),
 REGISTER_OP_READ,
 REGISTER_OP_READ_AND_WIRTE_MASK_ZERO,
 REGISTER_OP_READ_AND_WIRTE_MASK_ONE,
 REGISTER_OP_WAIT_CONDITION,
}STRU_REGISTER_OP_TYPE_T;

typedef struct
{
  STRU_REGISTER_OP_TYPE_T  op_type;
  AR_U32 address;
  AR_U32 data;//this value will be set, after succuess read a id or is a value will be writed
  STRU_I2C_ADDRESS_TYPE_T addr_type;
  STRU_I2C_DATA_TYPE_T   data_type;
  AR_U32 data_mask;
  AR_U32 conditon_mask;
  AR_U32 conditon_value;
  AR_U32 delay_us;
}STRU_REGISTER_SETTING_T;

typedef STRU_CAM_INFO_T STRU_SENSOR_DEV_INFO_T;
typedef STRU_CAM_SERVER_START_OPT_T STRU_VIN_DRIVER_START_OPT_T;
typedef STRU_CAM_SERVER_GLOBAL_PRA_T STRU_VIN_DRIVER_PUBLIC_PROPERTY_T;
typedef STRU_OPEN_PROPERTY_T STRU_VIN_PIPE_PROPERTY_T;
typedef STRU_CAM_SET_TUNING_REQUEST_T STRU_PIPE_SET_TUNING_REQUEST_T;


typedef struct
{
  void *remote_pa;
  void *remote_va;
  void *local_va;
  int  len;
}STRU_VIN_MEM_T;

typedef struct {
    AR_BOOL kb_en;
    AR_BOOL dpc_en;
    STRU_STREAM_BUFFER_INIFO_T kb_buf;
} STRU_CAM_PRA_SET_IR_NUC_PARAM_PRA_T;

typedef struct {
    AR_BOOL qgg_en;
    AR_S32 qgg_strength;
    AR_U32 qgg_correction;
    AR_U32 qgg_min;
    AR_U32 qgg_max;
    STRU_STREAM_BUFFER_INIFO_T qgg_buf;
} STRU_CAM_PRA_SET_IR_QGG_PARAM_PRA_T;

typedef struct {
    AR_U32 gtm_contrast;
    AR_S32 gtm_bright;
} STRU_CAM_PRA_SET_IR_GTM_PARAM_PRA_T;


typedef struct
{
    STRU_STREAM_BUFFER_INIFO_T input_frm;
    AR_S32 ir_3dnr_first_frame;
    AR_S32 use_hw_gtm;
} STRU_IR_ALGO_INPUT_T;

typedef struct
{
    STRU_STREAM_BUFFER_INIFO_T output_frm;
} STRU_IR_ALGO_3DNR_OUTPUT_T;

typedef struct
{
    STRU_STREAM_BUFFER_INIFO_T output_frm;
} STRU_IR_ALGO_2DNR_OUTPUT_T;

typedef struct
{
    STRU_STREAM_BUFFER_INIFO_T output_frm;
    AR_U32 hist_upper_pos;
    AR_U32 hist_lower_pos;
} STRU_IR_ALGO_GTM_OUTPUT_T;

typedef struct
{
    STRU_IR_ALGO_3DNR_OUTPUT_T output_3dnr;
    STRU_IR_ALGO_2DNR_OUTPUT_T output_2dnr;
    STRU_IR_ALGO_GTM_OUTPUT_T output_gtm;
} STRU_IR_ALGO_OUTPUT_T;

typedef struct {
    STRU_IR_ALGO_INPUT_T input;
    STRU_IR_ALGO_OUTPUT_T output;
} STRU_CAM_PRA_IR_ALGO_PROCESS_PRA_T;

typedef enum {
    HAL_IR_SNS_OCC_TYPE_EXTERNAL = 0,   /**<外部occ。occ数据由外部发送给sensor。比如高德gst417w就是外部occ。*/
    HAL_IR_SNS_OCC_TYPE_INTERNAL,       /**<内部occ。occ数据在sensor内部，不需要外部发送。比如高德gst212w就是内部occ。*/

    HAL_IR_SNS_OCC_TYPE_BUTT,
} ENUM_HAL_IR_SNS_OCC_TYPE_T;

typedef enum {
    HAL_IR_SNS_OCC_MONOTONIC_DECREASE = 0,  /**<occ单调递减。occ值越大，对应的像素值越小。*/
    HAL_IR_SNS_OCC_MONOTONIC_INCREASE,      /**<occ单调递增。occ值越大，对应的像素值越大。*/

    HAL_IR_SNS_OCC_MONOTONIC_BUTT,
} ENUM_HAL_IR_SNS_OCC_MONOTONIC_T;

typedef struct {
    AR_U32 ir_param_size;                           /**<红外sensor标定参数size，单位为字节。*/
    ENUM_HAL_IR_SNS_OCC_TYPE_T occ_type;            /**<occ类型。*/
    ENUM_HAL_IR_SNS_OCC_MONOTONIC_T occ_monotonic;  /**<occ单调性。*/
    AR_U32 min_occ_val;                             /**<occ最小值。*/
    AR_U32 max_occ_val;                             /**<occ最大值。*/
    AR_S32 occ_vertical_offset;                     /**<occ竖直方向偏移。大部分sensor可设置0。但是某些sensor（比如艾睿）的occ与raw图在竖直方向上有偏差。比如occ的第5行对应raw图的第8行，偏差3行，这时要将s32OccVerticalOffset设置为3。*/
    AR_BOOL occ_calib_by_sns;                       /**<是否由sensor驱动来做occ标定。如果为AR_FALSE，则采用sdk实现的通用occ标定方法；如果为AR_TRUE，则采用sensor驱动实现的occ标定方法。如果sensor使用内部occ，不管bOccCalibBySns如何设置，只能采用sensor驱动实现的occ标定方法。*/
    AR_U32 sns_width;                               /**<sensor输出raw图的原始宽度。*/
    AR_U32 sns_height;                              /**<sensor输出raw图的原始高度。*/
    AR_S32 roi_left;                                /**<sensor输出raw图的有效区域起始点横坐标。*/
    AR_S32 roi_top;                                 /**<sensor输出raw图的有效区域起始点纵坐标。*/
    AR_U32 roi_width;                               /**<sensor输出raw图的有效区域宽度。*/
    AR_U32 roi_height;                              /**<sensor输出raw图的有效区域高度。*/
} STRU_CAM_PRA_IR_SNS_ATTR_T;

typedef enum {
    HAL_IR_CALIB_STATUS_UNKNOWN = 0,    /**<标定状态未知。*/
    HAL_IR_CALIB_STATUS_ONGOING,        /**<标定进行中。*/
    HAL_IR_CALIB_STATUS_SUCCESS,        /**<标定成功。*/
    HAL_IR_CALIB_STATUS_FAIL,           /**<标定失败。*/
    HAL_IR_CALIB_STATUS_CANCELLED,      /**<标定被取消。*/

    HAL_IR_CALIB_STATUS_BUTT,
} ENUM_HAL_IR_CALIB_STATUS_T;

typedef struct {
    AR_U32 frm_interval;            /**<红外sensor参数标定帧间隔，表示隔多少帧迭代一次。*/
    AR_U32 target_lower_limit;      /**<红外sensor参数标定目标范围下限值。*/
    AR_U32 target_upper_limit;      /**<红外sensor参数标定目标范围上限值。*/
    AR_VOID *user_cfg;              /**<红外sensor参数标定用户自定义配置参数。*/
    AR_U32 user_cfg_size;           /**<红外sensor参数标定用户自定义配置参数size。*/
} STRU_CAM_PRA_IR_SNS_CALIB_CFG_T;

typedef struct {
    ENUM_HAL_IR_CALIB_STATUS_T calib_status;    /**<红外sensor参数标定状态。*/
    AR_VOID *p_calib_param;                     /**<RW; 标定后的红外sensor参数buf。只有标定成功，才能获取到标定后的红外sensor参数。如果不需要获取标定后的红外sensor参数，p_calib_param可指定为NULL。*/
} STRU_CAM_PRA_IR_SNS_CALIB_RESULT_T;

typedef enum {
    HAL_IR_OCC_CALIB_METHOD_STEP_SEARCH,        /**<固定步长搜索*/
    HAL_IR_OCC_CALIB_METHOD_BINARY_SEARCH,      /**<二分法搜索*/

    HAL_IR_OCC_CALIB_METHOD_BUTT,
} HAL_IR_OCC_CALIB_METHOD_E;

typedef enum {
    HAL_IR_OCC_CALIB_COND_0,    /**<occ标定完成条件0。如果落在目标区间的像素点数的百分比超过一个阈值（fPixsPercent），则标定完成。*/
    HAL_IR_OCC_CALIB_COND_1,    /**<occ标定完成条件1。对每一个像素点，寻找让它最接近目标区间中点的occ。可保证尽可能多的像素点落在目标区间，也可保证最好的像素均匀性，但标定耗时较长。*/

    HAL_IR_OCC_CALIB_COND_BUTT,
} HAL_IR_OCC_CALIB_COND_E;

typedef struct {
    HAL_IR_OCC_CALIB_METHOD_E calib_method;     /**<occ标定方法。*/
    AR_U32 occ_search_step;                     /**<occ搜索步长，仅当采用固定步长搜索方法时才有效。*/
    AR_U32 frm_interval;                        /**<occ标定帧间隔，表示隔多少帧迭代一次occ。*/
    AR_U32 target_lower_limit;                  /**<occ标定目标范围下限值*/
    AR_U32 target_upper_limit;                  /**<occ标定目标范围上限值*/
    HAL_IR_OCC_CALIB_COND_E calib_cond;         /**<occ标定完成条件。*/
    AR_FLOAT pixs_percent;                      /**<点数百分比阈值，仅当完成条件为IR_OCC_CALIB_COND_0时才有效。如果落在目标区间的像素点数百分比超过fPixsPercent，则标定完成。*/
    AR_U32 max_iterate_times;                   /**<occ迭代次数上限。如果occ迭代次数超过这个上限还没完成，则直接停止这次occ标定，并且认为标定失败。*/
} STRU_CAM_PRA_IR_OCC_CALIB_CFG_T;

typedef struct {
    ENUM_HAL_IR_CALIB_STATUS_T calib_status;    /**<occ标定状态。*/
    STRU_STREAM_BUFFER_INIFO_T occ_buf;         /**<标定后的occ数据buf。只有标定成功，才能获取到标定后的occ数据。如果不需要获取标定后的occ数据，可指定一个空buf。*/
} STRU_CAM_PRA_IR_OCC_CALIB_RESULT_T;

typedef struct {
    AR_U32 grab_frm_num;      /**<做b标定需要抓取的帧数。对抓取的帧做叠加平均后再做b标定，以减小噪声干扰。*/
    AR_BOOL mean_as_target_val;     /**<是否以图像平均值作为b标定的目标值。AR_TRUE: 以图像平均值作为目标值；AR_FALSE: 由u32TargetVal指定目标值。*/
    AR_U32 target_val;        /**<b标定的目标值，当bMeanAsTargetVal为AR_FALSE时有效。*/
} STRU_CAM_PRA_IR_B_CALIB_CFG_T;

typedef struct {
    ENUM_HAL_IR_CALIB_STATUS_T calib_status;    /**<b标定状态。*/
    STRU_STREAM_BUFFER_INIFO_T b_buf;           /**<标定后的b数据buf。只有标定成功，才能获取到标定后的b数据。如果不需要获取标定后的b数据，可指定一个空buf。*/
} STRU_CAM_PRA_IR_B_CALIB_RESULT_T;

typedef struct {
    AR_BOOL lowtemp_mean_as_target_val;         /**<是否以低温图像平均值作为KB标定的低温目标值。AR_TRUE: 以图像平均值作为目标值；AR_FALSE: 由u32LowTempTargetVal指定目标值。*/
    AR_U32 lowtemp_target_val;                  /**<KB标定的低温目标值，仅当bLowTempMeanAsTargetVal为AR_FALSE时才有效。*/
    AR_BOOL hightemp_mean_as_target_val;        /**<是否以高温图像平均值作为KB标定的高温目标值。AR_TRUE: 以图像平均值作为目标值；AR_FALSE: 由u32HighTempTargetVal指定目标值。*/
    AR_U32 hightemp_target_val;                 /**<KB标定的高温目标值，仅当bHighTempMeanAsTargetVal为AR_FALSE时才有效。*/
    STRU_STREAM_BUFFER_INIFO_T lowtemp_frm;     /**<低温raw图。*/
    STRU_STREAM_BUFFER_INIFO_T hightemp_frm;    /**<高温raw图。*/
    STRU_STREAM_BUFFER_INIFO_T kb_buf;          /**<标定后的KB数据buffer。如不需要得到标定后的KB数据，可给一个空buffer。*/
} STRU_CAM_PRA_IR_KB_CALIB_CTX_T;


#define NUC_CFG_MAX_LEN     16
#define NUC_OUT_CLK_DIV_MIN 1
#define NUC_OUT_CLK_DIV_MAX 128

typedef enum {
    NUC_INTF_MODE_GAODE_3IN = 0,
    NUC_INTF_MODE_HIKMICRO_3IN,
    NUC_INTF_MODE_IRAY_1IN,
    NUC_INTF_MODE_IRAY_2IN,
    NUC_INTF_MODE_BUTT,
} ENUM_NUC_INTF_MODE_T;

typedef enum {
    NUC_CLK_PHASE_0 = 0,
    NUC_CLK_PHASE_90,
    NUC_CLK_PHASE_180,
    NUC_CLK_PHASE_270,
    NUC_CLK_PHASE_BUTT,
} ENUM_NUC_CLK_PHASE_T;

typedef enum {
    NUC_OCC_BUF_ALIGN_256 = 0,      // ooc buf row length is 256-byte aligned
    NUC_OCC_BUF_ALIGN_128,          // ooc buf row length is 128-byte aligned
    NUC_OCC_BUF_ALIGN_BUTT,
} ENUM_NUC_OCC_BUF_ALIGN_T;

typedef enum {
    NUC_CMD_RESET = 0,              // data type: STRU_NUC_CMD_RESET_CODE_T
    NUC_CMD_SET_CFG_DATA,           // data type: STRU_NUC_CMD_CFG_DATA_T
    NUC_CMD_CFG_SW_MODE,            // data type: STRU_NUC_CMD_CFG_DATA_T
    NUC_CMD_CFG_HW_MODE,            // data type: STRU_NUC_CMD_CFG_DATA_T
    NUC_CMD_IN_CLK_CALIB,           // data type: STRU_NUC_CMD_IN_CLK_CALIB_T
    NUC_CMD_BUTT,
} ENUM_NUC_CMD_T;

typedef enum {
    NUC_OUT_CLK_SRC_500MHZ = 0,
    NUC_OUT_CLK_SRC_600MHZ,
    NUC_OUT_CLK_SRC_666MHZ,
    NUC_OUT_CLK_SRC_BUTT,
} ENUM_NUC_OUT_CLK_SRC_T;

typedef struct {
    AR_U32 reset_code;
    AR_U32 bitnum;
} STRU_NUC_CMD_RESET_CODE_T;

typedef struct {
    AR_U32 cfg_data[NUC_CFG_MAX_LEN];
    AR_U32 bitnum;
} STRU_NUC_CMD_CFG_DATA_T;

typedef struct {
    ENUM_NUC_CLK_PHASE_T nuc_in_clk_phase;
    AR_U32 ls_delay_cycles;
} STRU_NUC_CMD_IN_CLK_CALIB_T;

typedef struct {
    ENUM_NUC_INTF_MODE_T intf_mode;
    AR_U32 active_width;
    AR_U32 active_height;
    AR_U32 total_width;
    AR_U32 total_height;
    ENUM_NUC_OCC_BUF_ALIGN_T occ_buf_align;
    AR_U8 blank_value;
    AR_U16 cfg_head_ver_loc;
    AR_U16 cfg_head_hor_loc;
    AR_U32 fs_head;
    AR_U32 fs_head_bitnum;
    AR_U16 fs_head_ver_loc;
    AR_U16 fs_head_hor_loc;
    AR_U32 ls_head;
    AR_U32 ls_head_bitnum;
    AR_U16 ls_head_ver_loc_start;
    AR_U16 ls_head_ver_loc_end;
    ENUM_NUC_CLK_PHASE_T nuc_in_clk_phase;
    AR_U32 ls_delay_cycles;
} STRU_NUC_CFG_T;

typedef struct {
    AR_BOOL is_single_val;
    union
    {
        STRU_STREAM_BUFFER_INIFO_T occ_buf;
        AR_U32 single_val;
    };
} STRU_NUC_OCC_PARAM_T;

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
