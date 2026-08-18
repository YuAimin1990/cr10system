/**
 * \file
 * \brief 描述sns相关的通用数据结构
 */

#ifndef __AR_COMM_SNS_H__
#define __AR_COMM_SNS_H__

#include "hal_type.h"
#include "ar_common.h"
#include "ar_comm_isp.h"
#include "ar_combo_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
typedef struct arISP_SNS_ATTR_INFO_S
{
    SENSOR_ID            eSensorId;
} ISP_SNS_ATTR_INFO_S;

#define WDR_MAX_FRAME    (4)
typedef struct arISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    AR_U32  u32MaxWidth;
    AR_U32  u32MaxHeight;
} ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct arISP_CMOS_SENSOR_MODE_S
{
    AR_U32  u32SensorID;
    AR_U8   u8SensorMode;
    AR_BOOL bValidDngRawFormat;
    DNG_RAW_FORMAT_S stDngRawFormat;
} ISP_CMOS_SENSOR_MODE_S;

typedef struct arISP_CMOS_DNG_COLORPARAM_S
{
    ISP_DNG_WBGAIN_S stWbGain1;/*the calibration White balance gain of colorcheker in A Light*/
    ISP_DNG_WBGAIN_S stWbGain2;/*the calibration White balance gain of colorcheker in D50 Light*/
} ISP_CMOS_DNG_COLORPARAM_S;
#define EXP_RATIO_NUM 4
typedef struct arISP_CMOS_WDR_SWITCH_ATTR_S
{
    AR_U32   au32ExpRatio[EXP_RATIO_NUM];
    AR_U32   au32ShortOffset;
    AR_U32   au32MidOffset;
    AR_U32   au32LongOffset;
    AR_U32   au32VcCnt;
    AR_U32   au32VcMask;
}ISP_CMOS_WDR_SWITCH_ATTR_S;

typedef struct arISP_CMOS_DEFAULT_S
{
    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
    ISP_CMOS_SENSOR_MODE_S           stSensorMode;
    ISP_CMOS_DNG_COLORPARAM_S        stDngColorParam;
    ISP_CMOS_WDR_SWITCH_ATTR_S       stWdrSwitchAttr;
	AR_CHAR TuningPraBinName[256];
    AR_BOOL bDvpDownSampleEn;
	AR_BOOL bFoucs;
} ISP_CMOS_DEFAULT_S;

typedef struct arISP_CMOS_SENSOR_IMAGE_MODE_S
{
    AR_U16   u16Width;
    AR_U16   u16Height;
    AR_FLOAT f32Fps;
    AR_U8    u8SnsMode;
} ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct {
    STRU_VIDEO_FORAMT_NOTITY_PRA_T video_format;
    STRU_AUDIO_FORAMT_NOTITY_PRA_T audio_format;
} STRU_AR_HDMI_FORAMT_T;

typedef struct {
    AR_U32 hdmi_plug_status; // 1: plugin, 0: unplug    
    ENUM_NOTIFY_DATA_SRC_T src_type; //0 audio ,1 video, 2 audio&&video
    AR_U32 lowdelay_src; /* ar_video_input_type_t, isp 1; vif 2 */
    STRU_AR_HDMI_FORAMT_T format;
} STRU_AR_HDMI_PLUG_STATUS_T;

/* for AR_MESSAGE_FORMAT_CHANGE */
typedef struct {
    AR_U32 audio_change;
    AR_U32 video_change;	
    ENUM_NOTIFY_DATA_SRC_T src_type; //0 audio ,1 video, 2 audio&&video
    STRU_AR_HDMI_FORAMT_T format;
} STRU_AR_HDMI_FORMAT_CHANGE_T;


typedef struct arISP_SNS_DETECT_INFO_EXT_S
{
   union{
   	   STRU_AR_HDMI_PLUG_STATUS_T plug_info;
	   STRU_AR_HDMI_FORMAT_CHANGE_T change_info;
   }hdmi_info;
   
} ISP_SNS_DETECT_INFO_EXT_S;

typedef struct arISP_SNS_DETECT_INFO_S
{
    AR_U16 u16Width;
    AR_U16 u16Height;
    AR_FLOAT f32Fps;
    PIXEL_FORMAT_E enFormat;
    AR_BOOL bIsInterlace;
    AR_BOOL bIsConnected;	
    AR_BOOL bIsChanged;
    ISP_SNS_DETECT_INFO_EXT_S stExtInfo;
    AR_U32 au32Data[64];
} ISP_SNS_DETECT_INFO_S;

typedef struct arISP_CMOS_SENSOR_CTL_S
{
    AR_U8 u8CtlCode;
    AR_VOID *pCtlData;
} ISP_CMOS_SENSOR_CTL;

typedef STRU_MOVE_FOCUS_T ISP_SNS_MOVE_FOUCS_S;

enum
{
   SENSOR_POWER_ON,
   SENSOR_POWER_OFF,
   SENSOR_SENSOR_INIT,
   SENSOR_CFG_RES,
   SENSOR_STREAM_ON,
   SENSOR_STREAM_OFF,
   SENSOR_AEC_UPDATE,
   SENSOR_FLIP_MIRROR,
   SENSOR_CTL_SET_LED,
   SENSOR_CTL_SET_IR_CUTTER,
   SENSOR_CTL_SET_HDR,
   SENSOR_CTL_ERR_PROC,
   SENSOR_VSYNC,
   SENSOR_CLT,
   SENSOR_PRIV_CLT,
   SENSOR_TRIGGER_ON,
   SENSOR_CTL_START_DETECT,
   SENSOR_CTL_STOP_DETECT,
   SENSOR_CTL_GET_DETECT_INFO,
   SENSOR_MAX,
};

typedef struct arISP_SENSOR_EXP_FUNC_S
{
    AR_VOID(*pfn_cmos_sensor_init)(VI_PIPE ViPipe);
    AR_VOID(*pfn_cmos_sensor_exit)(VI_PIPE ViPipe);
    AR_VOID(*pfn_cmos_sensor_global_init)(VI_PIPE ViPipe);
    AR_S32 (*pfn_cmos_set_image_mode)(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    AR_S32 (*pfn_cmos_set_wdr_mode)(VI_PIPE ViPipe, AR_U8 u8Mode);

    /* the algs get data which is associated with sensor, except 3a */
    AR_S32(*pfn_cmos_get_isp_default)(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef);
    AR_S32(*pfn_cmos_get_sns_reg_info)(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

    /* the function of sensor set pixel detect */
    AR_VOID(*pfn_cmos_set_pixel_detect)(VI_PIPE ViPipe, AR_BOOL bEnable);
    AR_S32(*pfn_cmos_sns_power_on)(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr);
    AR_S32(*pfn_cmos_sns_power_off)(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr);
    AR_S32(*pfn_cmos_sns_ctl)(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl);
	AR_S32(*pfn_cmos_sns_move_focus)(VI_PIPE ViPipe,ISP_SNS_MOVE_FOUCS_S *move);	
	AR_S32(*pfn_cmos_sns_trigger)(VI_PIPE ViPipe,AR_S32 s32Frames);
    AR_S32 (*pfn_cmos_get_ir_sns_attr)(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr);
    AR_S32 (*pfn_cmos_set_ir_param)(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 u32ParamSize);
    AR_S32 (*pfn_cmos_get_ir_param)(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 *pu32ParamSize);
    AR_S32 (*pfn_cmos_get_sns_temperature)(VI_PIPE ViPipe, AR_S32 *ps32Temperature);
    AR_S32 (*pfn_cmos_sns_calib)(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx);
    AR_S32 (*pfn_cmos_ir_occ_calib)(VI_PIPE ViPipe, IR_OCC_CALIB_CTX_S *pstCalibCtx);
    AR_S32 (*pfn_cmos_switch_shutter)(VI_PIPE ViPipe, AR_BOOL bOpen);	
	AR_S32(*pfn_cmos_sns_stream_on)(VI_PIPE ViPipe);	
	AR_S32(*pfn_cmos_sns_get_otp_data)(VI_PIPE ViPipe,STRU_OTP_CALLIBRATION_DATA_T *stOtpData);
	
} ISP_SENSOR_EXP_FUNC_S;

typedef struct arISP_SENSOR_REGISTER_S
{
    ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__AR_COMM_SNS_H__ */
