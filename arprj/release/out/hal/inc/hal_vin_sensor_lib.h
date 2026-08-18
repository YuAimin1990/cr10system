/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VI
 *  @{
 */

#ifndef __HAL_VIN_SENSOR_LIB_H__
#define __HAL_VIN_SENSOR_LIB_H__


/**
 * @file hal_vin_sensor_lib.h
 * @brief 定义vin的sensor模块的api 及数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "math.h"
#include "hal_vin_type_def.h"

typedef struct
{
  AR_FLOAT gain;
  AR_U32 exp_time_us;
  AR_U32 line_count;
  AR_FLOAT   exp_ration_time[4];
  AR_FLOAT   exp_ration_gain[4];
  AR_S32      need_flash;
}STRU_SENSOR_AEC_UPDATE_T;

typedef enum
{
 SENSOR_IOCTL_CFG_RES=SENSOR_IOCTL_START*(1<<8),
 SENSOR_I2C_CTL_SET_EXP,
 SENSOR_IOCTL_GET_TUNING_PRA,
 SENSOR_I2C_CTL_INIT,
 SENSOR_I2C_CTL_PROBE,
 SENSOR_I2C_CTL_STREAM_ON,
 SENSOR_I2C_CTL_STREAM_OFF,
 SENSOR_I2C_CTL_VC_STREAM_ON,
 SENSOR_I2C_CTL_VC_STREAM_OFF,
 SENSOR_I2C_CTL_STREAM_FLIP_MIRROR,
 SENSOR_I2C_CTL_STREAM_SET_RES,
 SENSOR_I2C_CTL_DESTROY,
 SENSOR_I2C_CTL_DRIVER_DEF,
 SENSOR_I2C_CTL_CREATE_DONE,
 SENSOR_I2C_CTL_RUNTIME_DETECT_ON,
 SENSOR_I2C_CTL_RUNTIME_DETECT_OFF,
 SENSOR_I2C_CTL_ERR_PROC,
 SENSOR_I2C_CTL_SLAVE_STREAM_ON,
 SENSOR_I2C_CTL_SLAVE_STREAM_OFF,
 SENSOR_I2C_CTL_SET_LED,
 SENSOR_I2C_CTL_SET_IR_CUTTER,
 SENSOR_I2C_CTL_SET_HDR,
 SENSOR_I2C_CTL_SET_MASTER_OUT_SYNC_CLK,
 SENSOR_I2C_CTL_VSYNC,
 SENSOR_I2C_CTL_SET_VAR_PRA,

  //debug ctl
 SENSOR_I2C_CTL_REG_DUMP,
 SENSOR_I2C_CTL_DEBUG_WRITE,  //deprecated
 SENSOR_I2C_CTL_DEBUG_READ,  //deprecated
 SENSOR_I2C_CTL_DEBUG_PWND_LEVEL_SET,  //deprecated
 SENSOR_I2C_CTL_DEBUG_RESET_LEVEL_SET,  //deprecated


 SENSOR_BUNDLE_SET_TUNING,
 SENSOR_REGISTER_CORE_MODULE,
 SENSOR_UNREGISTER_CORE_MODULE,
 SENSOR_REGISTER_SUB_MODULE,
 SENSOR_UNREGISTER_SUB_MODULE,
 SENSOR_CTL_GET_SUB_CFG_REQ,
 SENSOR_CTL_SUB_POWER_ON_REQ,
 SENSOR_CTL_SUB_POWER_OFF_REQ,
 SENSOR_CTL_SUB_AEC_UPDATE_REQ,
 SENSOR_IOCTL_RLEASE_TUNING_PRA,


 SENSOR_I2C_CTL_PRI_START,
}ENUM_SENSOR_CTL_T;

typedef struct
{
  AR_S32 op_code;
  AR_VOID *pra;
  AR_S32 par_size;
}STRU_SENSOR_IOCLT_DRIVER_DEF_PAR_T;

typedef struct
{
  AR_S32 en;
  AR_FLOAT clk;
}STRU_MASTER_SYNC_CLK_T;


#define MIPI_MAX_CHANNEL_NUM 32

/** @short IPI Data Types */
typedef enum
{
	CSI_2_OBDATA_8		  = 0x12,
    CSI_2_YUV420_8        = 0x18,
    CSI_2_YUV420_10       = 0x19,
    CSI_2_YUV420_8_LEG    = 0x1A,
    CSI_2_YUV420_8_SHIFT  = 0x1C,
    CSI_2_YUV420_10_SHIFT = 0x1D,
    CSI_2_YUV422_8        = 0x1E,
    CSI_2_YUV422_10       = 0x1F,
    CSI_2_RGB444          = 0x20,
    CSI_2_RGB555          = 0x21,
    CSI_2_RGB565          = 0x22,
    CSI_2_RGB666          = 0x23,
    CSI_2_RGB888          = 0x24,
    CSI_2_RAW6            = 0x28,
    CSI_2_RAW7            = 0x29,
    CSI_2_RAW8            = 0x2A,
    CSI_2_RAW10           = 0x2B,
    CSI_2_RAW12           = 0x2C,
    CSI_2_RAW14           = 0x2D,
    CSI_2_RAW16           = 0x2E
}ENUM_CSI_2_DATA_TYPE_T;

typedef struct
{
  AR_U32 addr;
  AR_U32 value;
}STRU_HW_SENSOR_CLT_I2C_T;

typedef enum{
	DEVICE_MODE_MASTER,
	DEVICE_MODE_SLAVE,
	DEVICE_MODE_SLAVE_PROXY_MASTER,
	DEVICE_MODE_MAX,
}ENUM_DEVICE_MODE_T;


typedef enum{
	DEVICE_TYPE_NORMAL,
	DEVICE_TYPE_COMMON,
	DEVICE_TYPE_ISPIN,
	DEVICE_TYPE_MAX,
}ENUM_DEVICE_TYPE_T;


typedef enum
{
  GPIO_CFG_PADNUM,
  GPIO_CFG_FUNCTION,
  GPIO_CFG_GROUP,
  GPIO_CFG_PORT,
  GPIO_CFG_NUM,
  GPIO_CFG_INOUT,
}ENUM_GPIO_CFG_INDEX;


typedef struct
{
  STRU_REGISTER_SETTING_T reg_read_id[4];
  AR_U32 sensor_id[4];
  AR_U32 id_count;
  AR_S32 i2c_component_index;
  AR_S32 slave_address_7bit;
  AR_CHAR actuator[64];
  AR_CHAR eeprom[64];
  AR_CHAR flash[64];
  AR_CHAR master_sensor[64]; //when the device is slave , the name save the devices's master device
  AR_S32 need_master_slave_sel;// if set to 1 we need call the pfn_sensor_lib_set_slave_master to set the slave master
  AR_S32 device_mode;// 1: the device is slave ;0 :the device is master sensor;2 the device is master proxy, but it real is a slave sensor
  AR_S32 slave_num; //the master devie have how mush slave sensor;
  AR_CHAR slave_sensor[7][64]; //we max supported a master device have 7 slave devices
  AR_CHAR cfg_file[64]; //the device tuing cfg file
  AR_S32  focus;
}STRU_SENSOR_SLAVE_INFO_T;

typedef enum
{
  SLOT_MIPI,
  SLOT_DVP,
  SLOT_HDMI,
  SLOT_MEM,
}ENUM_SLOT_T;

typedef enum
{
  SENSOR_BAYER,
  SENSOR_YUV,
  SENSOR_RGB,
  SENSOR_HDMI_AUDIO,
}ENUM_SENSOR_T;

typedef enum
{
  LINK_MODE_DVP,
  LINK_MODE_MIPI_CSI,
  LINK_MODE_HDMI,
  LINK_MODE_MEM,
}ENUM_LINK_MODE;

typedef enum
{
 SENSOR_BIT_8_BIT=8,
 SENSOR_BIT_10_BIT=10,
 SENSOR_BIT_12_BIT=12,
 SENSOR_BIT_14_BIT=14,
 SENSOR_BIT_16_BIT=16,
}ENUM_SENSOR_BIT_T;

typedef enum
{
  YUV420,
  YUV422,
  YUV444,
}ENUM_YUV_DATA_TYPE_T;

typedef enum
{
  RGB888,
}ENUM_RGB_DATA_TYPE_T;

typedef enum
{
   FLIP_OFF_MIRROR_OFF,
   FLIP_OFF_MIRROR_ON,
   FLIP_ON_MIRROR_OFF,
   FLIP_ON_MIRROR_ON,
   FLIP_MIRRO_MAX,
}ENUM_FLIP_T;

typedef struct
{
  AR_U32 vc;//which virtual channel will be used this sensor, vc dt need according mipi spec to set
  AR_S32 dt;
  AR_U32 crop_en;
  AR_S32 out_format;
  AR_S32 bit_width;
  AR_S32 bayer;
  AR_S32 yuv;
  AR_S32 rgb;
  AR_S32 width;
  AR_S32 heigh;
  AR_S32 x;
  AR_S32 y;
  AR_S32 x_width;
  AR_S32 y_heigh;
  AR_U32 line_length;
  AR_U32 frame_length;
  AR_U32 clk_per_lane;
  AR_FLOAT fps;
  AR_S32 manual_ipi_timing;
  AR_S32 hsa;
  AR_S32 hbp;
  AR_S32 hsd;
}STRU_VC_CHANNEL_T;
typedef struct
{
 //mipi description
  AR_U32 mipi_index;
  AR_U32 mipi_lane_cout;
  AR_U32 settle_count;
}STRU_MIPI_DESC_T;

typedef enum
{
    DVP_BT601,
    DVP_BT656,
    DVP_BT1120
}ENUM_DVP_T;

typedef enum
{
	DE_MODE_DE_EN,
	DE_MODE_HSYNC,
	DE_MODE_LOW_HVSYNC,
	DE_MODE_NO_DE,
}ENUM_DE_MODE_T;

typedef enum{
   VIDEO_STABBLE_SELECT_MODE_DETECTED,
   VIDEO_STABBLE_SELECT_MODE_BYPASS,
}ENUM_VIDEO_STABBLE_SELECT_T;

typedef enum{
   DVP_POLORITY_MODE_HIGH,
   DVP_POLORITY_MODE_LOW,
}ENUM_DVP_POLORITY_MODE_T;

typedef enum{
   FIELD_SELECT_MODE_VYSNC,
   FIELD_SELECT_MODE_DE,
}ENUM_FIELD_SELECT_MODE;

typedef enum{
   BIT_WIDTH_16_BIT,
   BIT_WIDTH_8_BIT,
}ENUM_BIT_WIDTH_T;

typedef enum{
   DDR_SDR_SEL_DDR,
   DDR_SDR_SEL_SDR,
}ENUM_DDR_SDR_T;

typedef enum {
   YUV422_YUYV,
   YUV422_UYVY,
   YUV422_YVYU,
   YUV422_VYUY,
}ENUM_YUV422_T;

typedef struct
{
 AR_S32      highlow_8bit_switch;
 AR_S32      dvp_index;
 AR_S32      dvp_bit_width;
 AR_S32      dvp_ddr_sdr_mode;
 AR_S32      yuv_order;
 AR_S32      de_mode;
 AR_S32      stable_detect_mode;
 AR_S32      de_polority; // 0:same as input, 1:select polarity detect
 AR_S32      hs_polority; // 0:same as input, 1:select polarity detect
 AR_S32      vs_polority; // 0:same as input, 1:select polarity detect
 AR_S32      field_polority;
 AR_S32      filed_select_mode;
 AR_S32      dvp_type;
 AR_S32      is_interlace;
 // input_from_nuc is used by Iray ir-sensor, whose output data must be transformed into
 // dvp-format by nuc. So in this case, dvp input is from nuc.
 AR_S32      input_from_nuc;
 AR_S32      h_start;
 AR_S32      h_end;
 AR_S32      v_start;
 AR_S32      v_end;
 AR_S32      de_sel;
 AR_S32      vc_id[8];
 AR_S32      vc_num;
 AR_S32      vc_down_sample_en[8];
}STRU_DVP_DESC_T;


typedef enum
{
 HDMI_AUDIO,
 HDMI_VIDEO,
}ENUM_HDMI_T;

typedef struct
{
 AR_U32 hdmi_index;
 AR_U32 hdmi_media_type;
 AR_S32      is_interlace;
}STRU_HDMI_DESC_T;

typedef struct
{
  AR_S32 pad;
  AR_S32 func;
  AR_S32 group;
  AR_S32 port;
  AR_S32 num;
  AR_S32 in_out;
}STRU_CAM_GPIO_T;


typedef struct
{
  ENUM_SENSOR_T out_format;
  ENUM_LINK_MODE   link_mod;
  ENUM_SENSOR_BIT_T bit_width;

  ENUM_BAYER_FORMAT_T bayer;
  ENUM_YUV_DATA_TYPE_T yuv;
  ENUM_RGB_DATA_TYPE_T rgb;

  STRU_MIPI_DESC_T mipi_desc;
  STRU_DVP_DESC_T  dvp_desc;
  STRU_HDMI_DESC_T hdmi_desc;

  STRU_CAM_INFO_T cam_infor;
}STRU_SENSOR_OUT_INFOR_T;

typedef struct
{
  STRU_REGISTER_SETTING_T *p_seting;
  AR_S32 setting_num;
}STRU_REGISTER_SETTING_GROUP_T;


typedef enum
{
	SENSOR_CB_EVENT_POWER_ON,
	SENSOR_CB_EVENT_POWER_OFF,
	SENSOR_CB_EVENT_SENSOR_INIT,
	SENSOR_CB_EVENT_CFG_RES,
	SENSOR_CB_EVENT_STREAM_ON,
	SENSOR_CB_EVENT_STREAM_OFF,
	SENSOR_CB_EVENT_AEC_UPDATE,
	SENSOR_CB_EVENT_FLIP_MIRROR,
	SENSOR_CB_EVENT_CTL_SET_LED,
	SENSOR_CB_EVENT_CTL_SET_IR_CUTTER,
	SENSOR_CB_EVENT_CTL_SET_HDR,
	SENSOR_CB_EVENT_CTL_ERR_PROC,
	SENSOR_CB_EVENT_VSYNC,
	SENSOR_CB_EVENT_CLT,
	SENSOR_CB_EVENT_PRIV_CLT,
	SENSOR_CB_EVENT_TRIGGER_ON,
	SENSOR_CB_EVENT_ISP_DONE,
	SENSOR_CB_EVENT_CLT_SET_FPS_RANGE,
	SENSOR_CB_EVENT_ACTUATOR_UPDATE,
	SENSOR_CB_EVENT_GET_IR_SNS_ATTR,
	SENSOR_CB_EVENT_IR_SNS_CALIB,
	SENSOR_CB_EVENT_IR_OCC_CALIB,	
	SENSOR_CB_EVENT_VIF_DONE,	
	SENSOR_CB_EVENT_AWB_UPDATE,
	SENSOR_CB_EVENT_MAX,
}ENUM_SENSOR_CB_EVENT_T;


typedef struct
{
  STRU_MOVE_FOCUS_T  move;
}STRU_SENSOR_CB_EVENT_ACTUATOR_UPDATE_T;


typedef struct
{
   uint32_t fps_min;
   uint32_t fps_max;
   uint32_t fps_min_line_count;
   uint32_t fps_max_line_count;
}STRU_PRA_SET_FPS_RANGE_pra_t;


typedef struct
{
	AR_S32 status;
}STRU_SENSOR_CB_EVENT_CTL_SET_HDR_PRAT_T;

typedef struct
{
	AR_S32 err_code;
}STRU_SENSOR_CB_EVENT_CTL_ERR_PROC_PRA_T;


typedef struct
{
  AR_S32 camera_id;
  AR_S32 current_index;
  AR_S32 w;
  AR_S32 h;
  AR_FLOAT fps;
  AR_S32 sensor_hdr;
}STRU_SENSOR_CB_EVENT_CFG_RES_PRA_T;


typedef struct
{
  AR_S32 vc_strm_id;
  AR_S32 camera_id;
}STRU_SENSOR_CB_EVENT_INIT_PRA_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 vc_strm_id;
}STRU_SENSOR_CB_EVENT_STREAM_ON_PRA_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 vc_strm_id;
}STRU_SENSOR_CB_EVENT_STREAM_OFF_PRA_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 flip_mirror;
}STRU_SENSOR_CB_EVENT_FLIP_MIRROR_PRA_T;


typedef struct
{
  AR_S32 camera_id;
  AR_S32 vc_strm_id;
}STRU_SENSOR_CB_EVENT_POWER_ON_PRA_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 vc_strm_id;
}STRU_SENSOR_CB_EVENT_POWER_OFF_PRA_T;

typedef struct
{
  AR_S32 camera_id;
}STRU_SENSOR_CB_EVENT_VSYNC_PRA_T;

typedef struct
{
  AR_S32 camera_id;
}STRU_SENSOR_CB_EVENT_ISP_DONE_PRA_T;


typedef struct
{
  AR_S32 camera_id;
  AR_S32 ctl_code;
  AR_S32 ctl_size;
  AR_VOID * pra;
  AR_VOID * pra_pa;
}STRU_SENSOR_CB_EVENT_CTL_PRA_T;

typedef struct
{
  AR_S32 camera_id;
  AR_S32 ctl_code;
  AR_S32 ctl_size;
  AR_VOID * pra;
  AR_VOID * pra_pa;
}STRU_SENSOR_CB_EVENT_PRIV_CTL_PRA_T;


typedef struct
{
  AR_S32 camera_id;
  STRU_SENSOR_AEC_UPDATE_T update;
  STRU_AEC_OUT_T  aec_out;
}STRU_SENSOR_CB_EVENT_AEC_UPDATE_PRA_T;

typedef struct
{
  AR_S32 camera_id;
  STRU_AWB_OUT_T  awb_out;
}STRU_SENSOR_CB_EVENT_AWB_UPDATE_PRA_T;


typedef struct
{
	AR_S32 camera_id;
	AR_S32 vc_strm_id;
	AR_S32 trigger_num;
}STRU_SENSOR_CB_EVENT_TRIGGER_ON_PRA_T;

typedef struct {
    AR_S32 ret;
    STRU_CAM_PRA_IR_SNS_ATTR_T ir_sns_attr;
} STRU_SENSOR_CB_EVENT_GET_IR_SNS_ATTR_PRA_T;

typedef struct {
    STRU_CAM_PRA_IR_SNS_CALIB_CFG_T calib_cfg;
    AR_U32 frm_id;
    ENUM_STREAM_DATA_FORMAT_T frm_format;
    AR_U32 frm_width;
    AR_U32 frm_height;
    AR_U32 frm_stride;
    AR_VOID *frm_phy_addr;
    AR_U32 pix_mean;
    AR_U32 calib_param_size;
    ENUM_HAL_IR_CALIB_STATUS_T calib_status;
    AR_VOID *p_calib_param;
} STRU_SENSOR_CB_EVENT_IR_SNS_CALIB_PRA_T;

typedef struct {
    STRU_CAM_PRA_IR_OCC_CALIB_CFG_T calib_cfg;
    AR_U32 frm_id;
    ENUM_STREAM_DATA_FORMAT_T frm_format;
    AR_U32 frm_width;
    AR_U32 frm_height;
    AR_U32 frm_stride;
    AR_VOID *frm_phy_addr;
    AR_U32 pix_mean;
    AR_U32 occ_stride;
    AR_VOID *occ_phy_addr;
    ENUM_HAL_IR_CALIB_STATUS_T calib_status;
} STRU_SENSOR_CB_EVENT_IR_OCC_CALIB_PRA_T;

typedef struct
{
      AR_U32 crop_en;
      AR_U32 width;
      AR_U32 height;
      AR_U32 line_length;
      AR_U32 frame_length;
      AR_U32 frame_length_short; //the shortest
      AR_U32 frame_length_short1;//the short1
      AR_U32 clk_per_lane;
      AR_U32 mipi_pix_clk;
	  AR_U32 mipi_lanes;
	  AR_U32 settle_count;
      AR_FLOAT    fps;

      AR_U32 x_start;
      AR_U32 y_start;
      AR_U32 x_width;
      AR_U32 y_height;

      AR_U32 vc_mask;
      AR_S32 vc_count;
      STRU_VC_CHANNEL_T vc_info[4];

      AR_S32 manual_ipi_timing;
      AR_S32 hsa;
      AR_S32 hbp;
      AR_S32 hsd;

      AR_S32 enable_bayer;
      AR_S32 bayer_format;
      //hdr config
      AR_U32 sensor_hdr;
	  AR_S32 hdr_x_start;
	  AR_S32 hdr_y_start;
      AR_U32 hdr_width;
      AR_U32 hdr_height;
      AR_U32 hdr_frame_count;
      AR_U32 bit_depth;
      AR_S32	   long_offset;
      AR_S32	   mid_offset;
      AR_S32	   short_offset;
      AR_S32	   vif_x_start;
      AR_S32	   vif_y_start_l;
      AR_S32	   vif_y_start_m;
      AR_S32	   vif_y_start_s;
	  AR_S32	   vif_width;
	  AR_S32	   vif_height;
      AR_S32	   hdr_ebd_word_pos;
      AR_S32	   hdr_ebd_byte_pos;
      AR_S32	   hdr_fid_pos;
      AR_S32	   hdr_set_pos;
      AR_S32	   hdr_ebd_type;
      AR_S32	   hdr_fid0_pos;
      AR_S32	   hdr_fid1_pos;
      AR_S32	   hdr_fid2_pos;
      AR_S32	   hdr_obid_active_pos;
      AR_S32	   hdr_sid_pos;

      AR_U32 vif_line_buffer_manual_enable; // enable/disable manual mode of vif line buffering
      AR_U32 vif_line_buffer_de_delay; //rise edge of vsync to rise edge of first HRef;
      AR_U32 vif_line_buffer_htotal; //the length between two consecutive hsync(0~0xFFFF)
      AR_U32 vif_line_buffer_hstart; //the length between hysnc received to send-start;

      AR_S32 aec_factor;
      AR_S32 awb_factor;
      AR_S32 ltm_factor;
      //when 4k, the vif can not wrtite encoder frame to ddr, so need vif bypass mode to do a buffer, when in this case ,enable the cfg
      //but the vif bypass have only support one hdr, when have two hdr, another hdr can not set	it.
      // this is a 9301 hw limit bug, but hw designer not will fix it duce to schdule.
      // when a hdr sensor mipi fre is higher than vif_fre/3, the feature must be opened.
      // if we have two hdr sensor which mipi more than vif_fre/3 , the project must reject. please tell your pm, this is not supported
      AR_S32 use_vif_hdr_bypass;
      AR_S32 use_encoder_dpcm;
      AR_S32 dpcm_out_bits;
	  AR_S32 force_dng;
	  AR_CHAR tuning_name[128];
	  AR_S32  use_nr3d_encoder;
	  AR_S32  ir_sensor;	  
      AR_S32 hblank;
      AR_S32 vblank;
}STRU_SENSOR_RES_INFO_T;


typedef struct
{
	STRU_SENSOR_SLAVE_INFO_T slave_infor;
	STRU_SENSOR_OUT_INFOR_T	out_infor;
	AR_S32 res_index; // -1: if get ,the res will not effect, but will return res_count;  > =0; if get ,will get the res info, if set ,will set the res info
	AR_S32 res_count; //res count ,the
	STRU_SENSOR_RES_INFO_T res_info;
}STRU_CAMERA_INFO_VAR_PRA_T;

typedef STRU_CAMERA_INFO_VAR_PRA_T STRU_DEV_INFO_VAR_T;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
