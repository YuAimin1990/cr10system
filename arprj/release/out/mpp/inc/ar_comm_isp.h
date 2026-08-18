/**
 * \file
 * \brief 描述isp相关的通用数据结构
 */

#ifndef __AR_COMM_ISP_H__
#define __AR_COMM_ISP_H__

#include "hal_type.h"
#include "hal_errno.h"
#include "ar_common.h"
#include "ar_isp_debug.h"
#include "ar_comm_video.h"
#include "ar_isp_defines.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/
/*
ISP Error Code
0x40 = ISP_NOT_INIT
0x41 = ISP_MEM_NOT_INIT
0x42 = ISP_ATTR_NOT_CFG
0x43 = ISP_SNS_UNREGISTER
0x44 = ISP_INVALID_ADDR
0x45 = ISP_NOMEM
0x46 = ISP_NO_INT
*/
typedef enum arISP_ERR_CODE_E
{
    ERR_ISP_NOT_INIT                = 0x40,// ISP not init
    ERR_ISP_MEM_NOT_INIT            = 0x41,//ISP memory not init
    ERR_ISP_ATTR_NOT_CFG            = 0x42,//ISP attribute not cfg
    ERR_ISP_SNS_UNREGISTER          = 0x43,//ISP sensor unregister
    ERR_ISP_INVALID_ADDR            = 0x44,//ISP invalid address
    ERR_ISP_NOMEM                   = 0x45,//ISP nomem
    ERR_ISP_NO_INT                  = 0x46,//ISP
} ISP_ERR_CODE_E;

#define AR_ERR_ISP_NULL_PTR         AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_ISP_ILLEGAL_PARAM    AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_ISP_NOT_SUPPORT      AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)

#define AR_ERR_ISP_NOT_INIT         AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define AR_ERR_ISP_MEM_NOT_INIT     AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_MEM_NOT_INIT)
#define AR_ERR_ISP_ATTR_NOT_CFG     AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define AR_ERR_ISP_SNS_UNREGISTER   AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define AR_ERR_ISP_INVALID_ADDR     AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define AR_ERR_ISP_NOMEM            AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)
#define AR_ERR_ISP_NO_INT           AR_MPP_DEF_ERR(AR_ID_ISP, HAL_ERR_LEVEL_ERROR, ERR_ISP_NO_INT)

/*
Defines the working mode of ISP
0 = automatic mode
1 = manual mode
*/
typedef enum arISP_OP_TYPE_E
{
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ISP_OP_TYPE_E;

typedef enum arISP_SUS_MODE_E
{
	ISP_SUS_EXT_SHALLOW_MODE,
	ISP_SUS_SHALLOW_MODE,	
	ISP_SUS_DEEP_MODE
}ISP_SUS_MODE_E;

/*
Defines the ISP correction or detection status
0 = initial status, no calibration
1 = The static defect pixel calibration ends normally
2 = The static defect pixel calibration ends due to timeout.
*/
typedef enum arISP_STATE_E
{
    ISP_STATE_INIT     = 0,
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ISP_STATUS_E;


/*Defines the format of the input Bayer image*/
typedef enum arISP_BAYER_FORMAT_E
{
    AR_BAYER_RGGB    = 0,
    AR_BAYER_GRBG    = 1,
    AR_BAYER_GBRG    = 2,
    AR_BAYER_BGGR    = 3,
    AR_BAYER_BUTT
} ISP_BAYER_FORMAT_E;

/*Defines the bitwidth of the input Bayer image, used for lsc online calibration*/
typedef enum arISP_BAYER_RAWBIT_E
{
    BAYER_RAWBIT_8BIT    = 8,
    BAYER_RAWBIT_10BIT   = 10,
    BAYER_RAWBIT_12BIT   = 12,
    BAYER_RAWBIT_14BIT   = 14,
    BAYER_RAWBIT_16BIT   = 16,
    BAYER_RAWBIT_BUTT

} ISP_BAYER_RAWBIT_E;

typedef struct
{
  AR_S32 hblank;
  AR_S32 vblank;
}STRU_ISP_TIMING_T;

/* ISP public attribute, contains the public image attribute */
typedef struct arISP_PUB_ATTR_S
{
    RECT_S          stWndRect;      /**<RW; Start position of the cropping window, image width, and image height */
    SIZE_S          stSnsSize;      /**<RW; Width and height of the image output from the sensor*/
    AR_FLOAT        f32FrameRate;   /**<RW; Range: [0, 0xFFFF]; For frame rate */
    ISP_BAYER_FORMAT_E  enBayer;    /**<RW; Range:[0,3] ;Format:2.0;the format of the input Bayer image*/
    WDR_MODE_E      enWDRMode;      /**<RW; WDR mode select*/
    AR_U8           u8SnsMode;      /**<RW; 本模式传递到了pfn_cmos_set_image_mode(ViPipe, &stImageMode)中，驱动可以根据此模式选择分辨率等 */
    RECT_S          stWdrRect;     /**<@note 新加参数 wdr模式下的图像的crop 属性*/
	STRU_ISP_TIMING_T stTiming;    /**<@note 新加参数 控制isp 如何从ddr 读取图像*/
	//ISP_BAYER_RAWBIT_E enBayerRawBits; /*RW; how much the bits of the sensor out*/
} ISP_PUB_ATTR_S;

/*
Defines the ISP firmware status
0 = Running status
1 = Frozen status
*/
typedef enum arISP_FMW_STATE_E
{
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

/*Defines the WDR mode of the ISP*/
typedef struct arISP_WDR_MODE_S
{
    WDR_MODE_E  enWDRMode;
} ISP_WDR_MODE_S;

typedef struct arISP_INNER_STATE_INFO_S
{
   AR_S32 bResSwitchFinish;
   AR_S32 bWDRSwitchFinish;
} ISP_INNER_STATE_INFO_S;

/************AEC ***************/
/* config of statistics structs */
typedef struct arISP_AE_STATISTICS_CFG_S
{
	STRU_ROI_T roi;
} ISP_AE_STATISTICS_CFG_S;

typedef struct arISP_AE_STATISTICS_S
{
   STRU_AEC_ALOG_LIB_INPUT_T stAecStats;
} ISP_AE_STATISTICS_S;

/************ AWB*************/

/*Defines the AWB statistics configuration*/
typedef struct arISP_WB_STATISTICS_CFG_S
{
	STRU_ROI_T roi;
} ISP_WB_STATISTICS_CFG_S;

typedef struct arISP_WB_STATISTICS_S
{
   STRU_AWB_ALGO_LIB_INPUT_T stAwbStats;
}ISP_WB_STATISTICS_S;

/******************AF******************/
typedef struct arISP_FOCUS_STATISTICS_CFG_S
{
	//for af stats tuning pra update
	AR_S32 count_ae;
	AR_S32 trigger_mode;
	AR_S32 interpolation_enable;
	STRU_AEC_TRIGGER_T aec_trigger[MIN_TRIGGER_COUNT];
	STRU_AF_STATS_TUNING_T af_stats_tuning_pra[MIN_TRIGGER_COUNT];
} ISP_FOCUS_STATISTICS_CFG_S;

typedef struct arISP_AF_STATISTICS_S
{
   STRU_AF_ALGO_LIB_INPUT_T stAfStats;
} ISP_AF_STATISTICS_S;

typedef struct arISP_STATISTICS_CFG_S
{
    ISP_AE_STATISTICS_CFG_S     stAECfg;
    ISP_WB_STATISTICS_CFG_S     stWBCfg;
    ISP_FOCUS_STATISTICS_CFG_S  stFocusCfg;
} ISP_STATISTICS_CFG_S;

typedef struct arISP_INIT_ATTR_S
{
    AR_U32 u32ExpTime;
    AR_U32 u32AGain;
    AR_U32 u32DGain;
    AR_U32 u32ISPDGain;
    AR_U32 u32Exposure;
    AR_U32 u32LinesPer500ms;
    AR_U32 u32PirisFNO;
    AR_U16 u16WBRgain;
    AR_U16 u16WBGgain;
    AR_U16 u16WBBgain;
    AR_U16 u16SampleRgain;
    AR_U16 u16SampleBgain;
} ISP_INIT_ATTR_S ;

typedef struct arISP_DBG_ATTR_S
{
    AR_U32  u32Rsv;         /* H;need to add member */
} ISP_DBG_ATTR_S;

typedef struct arISP_DBG_STATUS_S
{
    AR_U32  u32FrmNumBgn;
    AR_U32  u32Rsv;         /* H;need to add member */
    AR_U32  u32FrmNumEnd;
} ISP_DBG_STATUS_S;

/*
0 = Communication between the sensor and the ISP over the I2C interface
1 = Communication between the sensor and the ISP over the SSP interface
*/
typedef enum arISP_SNS_TYPE_E
{
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,

    ISP_SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

/* sensor communication bus */
typedef union arISP_SNS_COMMBUS_U
{
    AR_S8   s8I2cDev;
    struct
    {
        AR_S8  bit4SspDev       : 4;
        AR_S8  bit4SspCs        : 4;
    } s8SspDev;
} ISP_SNS_COMMBUS_U;

typedef struct arISP_I2C_DATA_S
{
    AR_BOOL bUpdate;            /*RW; Range: [0x0, 0x1]; Format:1.0; AR_TRUE: The sensor registers are written,AR_FALSE: The sensor registers are not written*/
    AR_U8   u8DelayFrmNum;      /*RW; Number of delayed frames for the sensor register*/
    AR_U8   u8IntPos;           /*RW;Position where the configuration of the sensor register takes effect */
    AR_U8   u8DevAddr;          /*RW;Sensor device address*/
    AR_U32  u32RegAddr;         /*RW;Sensor register address*/
    AR_U32  u32AddrByteNum;     /*RW;Bit width of the sensor register address*/
    AR_U32  u32Data;            /*RW;Sensor register data*/
    AR_U32  u32DataByteNum;     /*RW;Bit width of sensor register data*/
} ISP_I2C_DATA_S;

typedef struct arISP_SSP_DATA_S
{
    AR_BOOL bUpdate;            /*RW; Range: [0x0, 0x1]; Format:1.0; AR_TRUE: The sensor registers are written,AR_FALSE: The sensor registers are not written*/
    AR_U8   u8DelayFrmNum;      /*RW; Number of delayed frames for the sensor register*/
    AR_U8   u8IntPos;           /*RW;Position where the configuration of the sensor register takes effect */
    AR_U32  u32DevAddr;         /*RW;Sensor device address*/
    AR_U32  u32DevAddrByteNum;  /*RW;Bit width of the sensor device address*/
    AR_U32  u32RegAddr;         /*RW;Sensor register address*/
    AR_U32  u32RegAddrByteNum;  /*RW;Bit width of the sensor register address*/
    AR_U32  u32Data;            /*RW;Sensor register data*/
    AR_U32  u32DataByteNum;     /*RW;Bit width of sensor register data*/
} ISP_SSP_DATA_S;

#define ISP_MAX_SNS_REGS                (32)
typedef struct arISP_SNS_REGS_INFO_S
{
    ISP_SNS_TYPE_E enSnsType;
    AR_U32  u32RegNum;              /*RW;Number of registers required when exposure results are written to the sensor. The member value cannot be dynamically changed*/
    AR_U8   u8Cfg2ValidDelayMax;    /*RW;Maximum number of delayed frames from the time when all sensor registers are configured to the
                                      time when configurations take effect, which is used to ensure the synchronization between sensor registers and ISP registers*/
    ISP_SNS_COMMBUS_U  unComBus;
    union
    {
        ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
        ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
    };

    struct
    {
        AR_BOOL bUpdate;
        AR_U8   u8DelayFrmNum;
        AR_U32  u32SlaveVsTime;      /* RW;time of vsync. Unit: inck clock cycle */
        AR_U32  u32SlaveBindDev;
    } stSlvSync;

    AR_BOOL bConfig;
} ISP_SNS_REGS_INFO_S;

typedef enum arISP_VD_TYPE_E
{
    ISP_VD_FE_START   = 0,  // isp vsync
    ISP_VD_FE_END,          // isp done
    ISP_VD_BE_END,          /**<@note 转义参数，表示vif 收到一帧数据的中断信号，vif 的这个中断信号是由sensor的vsync 信号触发的，表示的是上一帧的结束，下一帧的开始 
    ，所以当收到这个信号后，表示的下一帧的vsync， 比如启动后vin 后，第一次收到此信号表示的是第二帧的开始，也表示第一帧结束。 vif 不能发出第一帧的开始，使用的时候请注意
    本信号只有vif 工作扎ddr模式的时候可以取到，linebuffer 模式没有*/ 

    ISP_VD_BUTT
} ISP_VD_TYPE_E;

/******************************************************/
/********************* AE structs ************************/

/*
Defines the ISP FSWDR operating mode
0 = Normal FSWDR mode
1 = Long frame mode, only effective in LINE_WDR, when running in this mode FSWDR module only output the long frame data
*/
typedef enum hiISP_FSWDR_MODE_E
{
    ISP_FSWDR_NORMAL_MODE          = 0x0,
    ISP_FSWDR_LONG_FRAME_MODE      = 0x1,
    ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2,/*Auto long frame mode, only effective in LINE_WDR, When running in this mode, normal WDR and long frame mode would auto switch*/
    ISP_FSWDR_MODE_BUTT
} ISP_FSWDR_MODE_E;


typedef struct arISP_EXPOSURE_ATTR_S
{
    AR_BOOL bEnable;
    AR_U8   bManual;
    STRU_AEC_SET_INFO_T stManualExposure;
    STRU_AEC_TUNING_T   stAutoExposure;
} ISP_EXPOSURE_ATTR_S;


typedef struct arISP_EXP_INFO_S
{
	STRU_AEC_GET_INFO_T stAecInfo;
} ISP_EXP_INFO_S;

/********************* AWB structs ************************/

typedef struct arISP_WB_ATTR_S
{
    AR_BOOL bEnable;
    AR_U8   bManual;
    STRU_AWB_SET_INFO_T   stManualWb;
    STRU_AWB_TUNING_T stAutoWb;
} ISP_WB_ATTR_S;


typedef struct arISP_WB_INFO_S
{
  STRU_AWB_GET_INFO_T stAwbInfo;
} ISP_WB_INFO_S;

/********************* AF structs ************************/

typedef struct arISP_FOCUS_ATTR_S
{
    AR_BOOL bEnable;
    AR_U8	bManual;
    STRU_AF_SET_INFO_T stManualFocus;
    STRU_AF_TUNING_T  stAutoFocus;
} ISP_FOCUS_ATTR_S;

typedef struct arISP_FOCUS_INFO_S
{
   STRU_AF_GET_INFO_T stAfInfo;
} ISP_FOCUS_INFO_S;

/*
DNG cfalayout type
1 = Rectangular (or square) layout
2 = Staggered layout A: even columns are offset down by 1/2 row
3 = Staggered layout B: even columns are offset up by 1/2 row
4 = Staggered layout C: even rows are offset right by 1/2 column
5 = Staggered layout D: even rows are offset left by 1/2 column
6 = Staggered layout E: even rows are offset up by 1/2 row, even columns are offset left by 1/2 column
7 = Staggered layout F: even rows are offset up by 1/2 row, even columns are offset right by 1/2 column
8 = Staggered layout G: even rows are offset down by 1/2 row, even columns are offset left by 1/2 column
9 = Staggered layout H: even rows are offset down by 1/2 row, even columns are offset right by 1/2 column
*/
typedef enum arDNG_CFALAYOUT_TYPE_E
{
    CFALAYOUT_TYPE_RECTANGULAR = 1,
    CFALAYOUT_TYPE_A,        /*a,b,c... not support*/
    CFALAYOUT_TYPE_B,
    CFALAYOUT_TYPE_C,
    CFALAYOUT_TYPE_D,
    CFALAYOUT_TYPE_E,
    CFALAYOUT_TYPE_F,
    CFALAYOUT_TYPE_G,
    CFALAYOUT_TYPE_H,
    CFALAYOUT_TYPE_BUTT
} DNG_CFALAYOUT_TYPE_E;

typedef struct arDNG_SRATIONAL_S
{
    AR_S32 s32Numerator;   /*represents the numerator of a fraction,*/
    AR_S32 s32Denominator; /* the denominator. */
} DNG_SRATIONAL_S;

typedef struct arDNG_BLCREPEATDIM_S
{
    AR_U16 u16BlcRepeatRows;
    AR_U16 u16BlcRepeatCols;
} DNG_BLCREPEATDIM_S;

typedef struct arDNG_DEFAULTSCALE_S
{
    DNG_RATIONAL_S stDefaultScaleH;
    DNG_RATIONAL_S stDefaultScaleV;
} DNG_DEFAULTSCALE_S;

typedef struct arDNG_REPEATPATTERNDIM_S
{
    AR_U16 u16RepeatPatternDimRows;
    AR_U16 u16RepeatPatternDimCols;
} DNG_REPEATPATTERNDIM_S;

/*
Defines the structure of dng raw format.
*/
typedef struct arDNG_RAW_FORMAT_S
{
    AR_U8 u8BitsPerSample;                        /* RO;Format:8.0; Indicate the bit numbers of raw data*/
    AR_U8 au8CfaPlaneColor[CFACOLORPLANE];        /* RO;Format:8.0; Indicate the planer numbers of raw data; 0:red 1:green 2: blue*/
    DNG_CFALAYOUT_TYPE_E enCfaLayout;             /* RO;Range:[1,9]; Describes the spatial layout of the CFA*/
    DNG_BLCREPEATDIM_S stBlcRepeatDim;            /* Specifies repeat pattern size for the BlackLevel*/
    AR_U32 u32WhiteLevel;                         /* RO;Format:32.0; Indicate the WhiteLevel of the raw data*/
    DNG_DEFAULTSCALE_S stDefaultScale;            /* Specifies the default scale factors for each direction to convert the image to square pixels*/
    DNG_REPEATPATTERNDIM_S stCfaRepeatPatternDim;/* Specifies the pixel number of repeat color planer in each direction*/
    AR_U8 au8CfaPattern[ISP_BAYER_CHN];           /* RO;Format:8.0; Indicate the bayer start order; 0:red 1:green 2: blue*/
} DNG_RAW_FORMAT_S;

/*
Defines the structure of DNG WB gain used for calculate DNG colormatrix.
*/
typedef struct arISP_DNG_WBGAIN_S
{
    AR_U16 u16Rgain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for R  color channel*/
    AR_U16 u16Ggain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for G  color channel*/
    AR_U16 u16Bgain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for B  color channel*/
} ISP_DNG_WBGAIN_S;

typedef struct arISP_SATURATION_TIDY_ATTR_S
{
    AR_U32   u32Saturation;  /**<0---100*/
} ISP_SATURATION_TIDY_ATTR_S;

typedef struct arISP_BRIGHTNESS_TIDY_ATTR_S
{
    AR_U32   u32Brightness; /**<0---100*/
} ISP_BRIGHTNESS_TIDY_ATTR_S;

typedef struct arISP_CONTRAST_TIDY_ATTR_S
{
    AR_U32   u32Contrast;/**<0---100*/
} ISP_CONTRAST_TIDY_ATTR_S;

typedef struct arISP_GAMMA_TIDY_ATTR_S
{
    AR_U32   u32Gamma;/**<0---100*/
} ISP_GAMMA_TIDY_ATTR_S;


typedef struct arISP_GLOBAL_CONTRAST_TIDY_ATTR_S
{
    AR_U32   u32GlobalContrast;/**<0---100*/
} ISP_GLOBAL_CONTRAST_TIDY_ATTR_S;

typedef struct arISP_SHARPNESS_TIDY_ATTR_S
{
    AR_U32   u32Sharpness;/**<0---100*/
} ISP_SHARPNESS_TIDY_ATTR_S;

typedef struct arISP_DETAIL_ENH_TIDY_ATTR_S
{
    AR_U32   u32FlatZoneEnh;/**<平坦区细节增强：0---100*/
    AR_U32   u32DetailZoneEnh;/**<非平坦区细节增强：0---100*/
} ISP_DETAIL_ENH_TIDY_ATTR_S;

typedef struct arISP_AEC_MODE_TIDY_ATTR_S
{
    ENUM_AEC_MODE_T   u32AecMode;
} ISP_AEC_MODE_TIDY_ATTR_S;

typedef struct arISP_AEC_MANU_TIDY_ATTR_S
{
    STRU_AEC_SET_INFO_T   stAecManu;
} ISP_AEC_MANU_TIDY_ATTR_S;

typedef struct arISP_AWB_MODE_TIDY_ATTR_S
{
    ENUM_AWB_MODE_T   u32AwbMode;
} ISP_AWB_MODE_TIDY_ATTR_S;

typedef struct arISP_AWB_MANU_TIDY_ATTR_S
{
    STRU_AWB_SET_INFO_T   stAwbManu;
} ISP_AWB_MANU_TIDY_ATTR_S;

typedef struct arISP_AF_MODE_TIDY_ATTR_S
{
    AR_U32   u32AfMode;
} ISP_AF_MODE_TIDY_ATTR_S;

typedef struct arISP_AF_ROI_TIDY_ATTR_S
{
    STRU_CAM_ROI_T   stAfRoi;
} ISP_AF_ROI_TIDY_ATTR_S;

typedef struct arISP_3A_INFO_TIDY_ATTR_S
{
    STRU_BASIC_3A_INFO_T   st3aInfo;
} ISP_3A_INFO_TIDY_ATTR_S;

typedef struct arISP_3A_INFO_EXT_TIDY_ATTR_S
{
    STRU_ALGO_3A_GET_INFO_T   st3aInfoExt;
} ISP_3A_INFO_EXT_TIDY_ATTR_S;

typedef struct arISP_HUE_TIDY_ATTR_S
{
    AR_U32   u32Hue;/**<0---100*/
} ISP_HUE_TIDY_ATTR_S;

typedef struct arISP_FLIP_STATE_TIDY_ATTR_S
{
    AR_U32   u32FlipState;   /**< 0:关闭 1:使能flip*/
} ISP_FLIP_STATE_TIDY_ATTR_S;

typedef struct arISP_MIRROR_STATE_TIDY_ATTR_S
{
    AR_U32   u32MirrorState;/**< 0:关闭 1:使能mirror*/
} ISP_MIRROR_STATE_TIDY_ATTR_S;

typedef struct arISP_BANDING_STATE_TIDY_ATTR_S
{
    ENUM_ANTIBANDING_T   u32BandingState;
} ISP_BANDING_STATE_TIDY_ATTR_S;

typedef struct arISP_IR_CUTTER_STATE_TIDY_ATTR_S
{
    STRU_CAM_PRA_IR_CUTTER_PRA_T   u32IrCutterState;
} ISP_IR_CUTTER_STATE_TIDY_ATTR_S;

typedef struct arISP_LED_STATE_TIDY_ATTR_S
{
    STRU_CAM_PRA_LED_PRA_T   u32LedState;
} ISP_LED_STATE_TIDY_ATTR_S;

typedef struct arISP_AE_ROI_TIDY_ATTR_S
{
    STRU_CAM_ROI_T   stAeRoi;
} ISP_AE_ROI_TIDY_ATTR_S;

typedef struct arISP_FPS_RANGE_TIDY_ATTR_S
{
    STRU_FPS_INFO_T   stFpsRange;
} ISP_FPS_RANGE_TIDY_ATTR_S;

typedef struct arISP_EXP_LIMIT_TIDY_ATTR_S
{
    AR_U32   u32ExpLimit;
} ISP_EXP_LIMIT_TIDY_ATTR_S;

typedef struct arISP_PIPE_LIMIT_TIDY_ATTR_S
{
	STRU_CAM_PRA_GET_LIMIT_INFO_PRA_T stLimit;
} ISP_PIPE_LIMIT_TIDY_ATTR_S;

typedef struct arISP_EXP_POLICY_TIDY_ATTR_S
{
    STRU_EXP_POLIOCY_T   stExpPolicy;
} ISP_EXP_POLICY_TIDY_ATTR_S;

typedef struct arISP_BackLight_TIDY_ATTR_S
{
    STRU_BLACK_LIGHT_T   stBackLight;
} ISP_BackLight_TIDY_ATTR_S;




typedef struct arISP_FACE_AEC_TIDY_ATTR_S
{
    STRU_FACE_AEC_T   stFaceAec;
} ISP_FACE_AEC_TIDY_ATTR_S;

typedef struct arISP_ZOOM_TIDY_ATTR_S
{
    STRU_ZOOM_PRA_T   stZoom;
} ISP_ZOOM_TIDY_ATTR_S;

typedef struct arISP_CSC_TIDY_ATTR_S
{
    STRU_CSC_PRA_T   stCsc;
} ISP_CSC_TIDY_ATTR_S;

typedef struct arISP_DE3D_STRENGTH_TIDY_ATTR_S
{
    AR_U32   u32De3dStrength;/**<0---100*/
} ISP_DE3D_STRENGTH_TIDY_ATTR_S;

typedef struct arISP_RNR_STRENGTH_TIDY_ATTR_S
{
    AR_U32   u32RnrStrength;/**<0---100*/
} ISP_RNR_STRENGTH_TIDY_ATTR_S;

typedef struct arISP_DE2D_STRENGTH_TIDY_ATTR_S
{
    STRU_DENOISE_2D_PRA_T   stDe2dStrength;
} ISP_DE2D_STRENGTH_TIDY_ATTR_S;

typedef struct arISP_DEFOG_STRENGTH_TIDY_ATTR_S
{
    AR_U32   u32DefogStrength;/**<0---100*/
} ISP_DEFOG_STRENGTH_TIDY_ATTR_S;

typedef struct arISP_DWDR_STRENGTH_TIDY_ATTR_S
{
    AR_U32   u32Strength;/**<0---100*/
} ISP_DWDR_STRENGTH_TIDY_ATTR_S;


typedef struct arISP_SRC_SENSOR_STATS_TIDY_ATTR_S
{
    AR_U32   u32SrcSensorStats;
} ISP_SRC_SENSOR_STATS_TIDY_ATTR_S;
/**
   \brief 切换场景的结构描述，比如日夜切换，本api只负责切换，不负责何时切换，需要用户自己决定何时切换，本切换属于实施参数，调用即生效
*/
typedef struct arISP_SENSCE_TIDY_ATTR_S
{
    STRU_SENSE_INFO_T   stSensce;/**<场景，由场景的效果bin文件决定，用户通过本参数切换场景，比如日夜切换*/
} ISP_SENSCE_TIDY_ATTR_S;

typedef struct arISP_GAMMA_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Gamma Function*/
	AR_BOOL   bManual;
	STRU_ISP_SUB_MODULE_GAMMA_LUT_TUNING_T stAutoGamma; /*set to tuning bin, the system will select the gamma by aec gain or lux index*/
	STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA stManualGamma; /*the isp will use the gamma directly from this setting, the aec will not effect it*/
} ISP_GAMMA_ATTR_S;

typedef struct arISP_ACM_ATTR_S
{
	AR_BOOL bEnable;
	AR_BOOL bManual;
	STRU_ISP_SUB_MODULE_ACM_TUNING_T stAutoACM;
	STRU_ISP_SUB_MODULE_ACM_PRA		 stManualACM;
} ISP_ACM_ATTR_S;

typedef struct arISP_WDR_FS_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable WdrFs Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_HDR_MIX_TUNING_T stAutoWdrFs; /*set to tuning bin, the system will select the WdrFs from tuning*/
    STRU_ISP_SUB_MODULE_HDR_MIX_PRA stManualWdrFs; /*the isp will use the WdrFs directly from this setting*/
} ISP_WDR_FS_ATTR_S;

typedef struct arISP_DRC_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Drc Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_DRC_TUNING_T stAutoDrc; /*set to tuning bin, the system will select the Drc from tuning*/
    STRU_ISP_SUB_MODULE_DRC_PRA stManualDrc; /*the isp will use the Drc directly from this setting*/
} ISP_DRC_ATTR_S;

typedef struct arISP_DEHAZE_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Dehaze Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_GTM2_LUT_TUNING_T stAutoDehaze; /*set to tuning bin, the system will select the Dehaze from tuning*/
    STRU_ISP_SUB_MODULE_GTM2_LUT_PRA stManualDehaze; /*the isp will use the Dehaze directly from this setting*/
} ISP_DEHAZE_ATTR_S;

typedef struct arISP_LDCI_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable LDCI Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_GTM2_LUT_TUNING_T stAutoLDCI; /*set to tuning bin, the system will select the LDCI from tuning*/
    STRU_ISP_SUB_MODULE_GTM2_LUT_PRA stManualLDCI; /*the isp will use the LDCI directly from this setting*/
} ISP_LDCI_ATTR_S;

typedef struct arISP_DP_DYNAMIC_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable DpDynamic Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_DPC_TUNING_T stAutoDpDynamic; /*set to tuning bin, the system will select the DpDynamic from tuning*/
    STRU_ISP_SUB_MODULE_DPC_PRA stManualDpDynamic; /*the isp will use the DpDynamic directly from this setting*/
} ISP_DP_DYNAMIC_ATTR_S;


typedef struct arISP_SHADING_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Shading Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_LSC_TUNING_T stAutoShading; /*set to tuning bin, the system will select the Shading from tuning*/
    STRU_ISP_SUB_MODULE_LSC_PRA stManualShading; /*the isp will use the Shading directly from this setting*/
} ISP_SHADING_ATTR_S;

typedef struct arISP_HDR_SHADING_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable HdrShading Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_LSC_TUNING_T stAutoHdrShading; /*set to tuning bin, the system will select the HdrShading from tuning*/
    STRU_ISP_SUB_MODULE_LSC_PRA stManualHdrShading; /*the isp will use the HdrShading directly from this setting*/
} ISP_HDR_SHADING_ATTR_S;

typedef struct arISP_NR_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Nr Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_RNR_TUNING_T stAutoNr; /*set to tuning bin, the system will select the Nr from tuning*/
    STRU_ISP_SUB_MODULE_RNR_PRA stManualNr; /*the isp will use the Nr directly from this setting*/
} ISP_NR_ATTR_S;

typedef struct arISP_CAC_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable CAC Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_CM2_TUNING_T stAutoCAC; /*set to tuning bin, the system will select the CAC from tuning*/
    STRU_ISP_SUB_MODULE_CM2_PRA stManualCAC; /*the isp will use the CAC directly from this setting*/
} ISP_CAC_ATTR_S;


typedef struct arISP_SHARPEN_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Sharpen Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_LEE_TUNING_T stAutoSharpen; /*set to tuning bin, the system will select the Sharpen from tuning*/
    STRU_ISP_SUB_MODULE_LEE_PRA stManualSharpen; /*the isp will use the Sharpen directly from this setting*/
} ISP_SHARPEN_ATTR_S;


typedef struct arISP_CA_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable CA Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_CM_TUNING_T stAutoCA; /*set to tuning bin, the system will select the CA from tuning*/
    STRU_ISP_SUB_MODULE_CM_PRA stManualCA; /*the isp will use the CA directly from this setting*/
} ISP_CA_ATTR_S;


typedef struct arISP_DEMOSAIC_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Demosaic Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_CFA_TUNING_T stAutoDemosaic; /*set to tuning bin, the system will select the Demosaic from tuning*/
    STRU_ISP_SUB_MODULE_CFA_PRA stManualDemosaic; /*the isp will use the Demosaic directly from this setting*/
} ISP_DEMOSAIC_ATTR_S;

typedef struct arISP_BLACK_LEVEL_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable BlackLevel Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_BLC_TUNING_T stAutoBlackLevel; /*set to tuning bin, the system will select the BlackLevel from tuning*/
    STRU_ISP_SUB_MODULE_BLC_PARA stManualBlackLevel; /*the isp will use the BlackLevel directly from this setting*/
} ISP_BLACK_LEVEL_S;


typedef struct arISP_CLUT_LUT_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable ClutLUT Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_3D_LUT_TUNING_T stAutoClutLUT; /*set to tuning bin, the system will select the ClutLUT from tuning*/
    STRU_ISP_SUB_MODULE_3D_LUT_PRA stManualClutLUT; /*the isp will use the ClutLUT directly from this setting*/
} ISP_CLUT_LUT_S;


typedef struct arISP_CSC_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Csc Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_RGB2YUV_TUNING_T stAutoCsc; /*set to tuning bin, the system will select the Csc from tuning*/
    STRU_ISP_SUB_MODULE_RGB2YUV_PRA stManualCsc; /*the isp will use the Csc directly from this setting*/
} ISP_CSC_ATTR_S;

typedef struct arDNG_IMAGE_STATIC_INFO_S
{
} DNG_IMAGE_STATIC_INFO_S;

typedef struct arISP_DNG_COLORPARAM_S
{
}ISP_DNG_COLORPARAM_S;

typedef struct arISP_COLORMATRIX_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable CCM Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_CCM1_TUNING_T stAutoCCM; /*set to tuning bin, the system will select the CCM from tuning*/
    STRU_ISP_SUB_MODULE_CCM1_PRA stManualCCM; /*the isp will use the CCM directly from this setting*/
} ISP_COLORMATRIX_ATTR_S;

typedef struct arISP_CNF_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable CNF Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_CNF_TUNING stAutoCNF; /*set to tuning bin, the system will select the CNF from tuning*/
    STRU_ISP_SUB_MODULE_CNF_PRA stManualCNF; /*the isp will use the CNF directly from this setting*/
} ISP_CNF_ATTR_S;

typedef struct arISP_IR_LMS_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable IR_LMS Function*/
    AR_BOOL   bManual;
    STRU_ISP_SUB_MODULE_IR_LMS_TUNING_T stAutoIrLms; /*set to tuning bin, the system will select the IR_LMS from tuning*/
    STRU_ISP_SUB_MODULE_IR_LMS_PRA stManualIrLms; /*the isp will use the IR_LMS directly from this setting*/
} ISP_IR_LMS_ATTR_S;

typedef struct arISP_IR_RAW_3DNR_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable IR_RAW_3DNR Function*/
    AR_BOOL   bManual;
    STRU_ISP_SW_ALGO_IR_RAW_3DNR_TUNING_T stAutoIrRaw3Dnr; /*set to tuning bin, the system will select the IR_RAW_3DNR from tuning*/
    STRU_ISP_SW_ALGO_IR_RAW_3DNR_PRA stManualIrRaw3Dnr; /*the isp will use the IR_RAW_3DNR directly from this setting*/
} ISP_IR_RAW_3DNR_ATTR_S;

typedef struct arISP_IR_RAW_2DNR_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable IR_RAW_2DNR Function*/
    AR_BOOL   bManual;
    STRU_ISP_SW_ALGO_IR_RAW_2DNR_TUNING_T stAutoIrRaw2Dnr; /*set to tuning bin, the system will select the IR_RAW_2DNR from tuning*/
    STRU_ISP_SW_ALGO_IR_RAW_2DNR_PRA stManualIrRaw2Dnr; /*the isp will use the IR_RAW_2DNR directly from this setting*/
} ISP_IR_RAW_2DNR_ATTR_S;

typedef struct arISP_IR_RAW_GTM_ATTR_S
{
    AR_BOOL   bEnable;                     /*RW; Range:[0, 1]; Format:1.0;Enable/Disable IR_RAW_GTM Function*/
    AR_BOOL   bManual;
    STRU_ISP_SW_ALGO_IR_RAW_GTM_TUNING_T stAutoIrRawGtm; /*set to tuning bin, the system will select the IR_RAW_GTM from tuning*/
    STRU_ISP_SW_ALGO_IR_RAW_GTM_PRA stManualIrRawGtm; /*the isp will use the IR_RAW_GTM directly from this setting*/
} ISP_IR_RAW_GTM_ATTR_S;


typedef struct arISP_NOTIFY_FUNC_S
{
	AR_VOID (*vsync_update)(VI_PIPE ViPipe);
	AR_VOID (*aec_update)(VI_PIPE ViPipe,STRU_AEC_OUT_T *aec_out);	
	AR_VOID (*awb_update)(VI_PIPE ViPipe,STRU_AWB_OUT_T *awb_out);
} ISP_NOTIFY_FUNC_S;

typedef struct arISP_NOTIFY_REGISTER_S
{
    ISP_NOTIFY_FUNC_S stNotify;
} ISP_NOTIFY_REGISTER_S;

typedef enum arIR_PARAM_TYPE_E
{
    IR_PARAM_TYPE_SENSOR = 0,   // ParamData type: defined by sensor; ParamSize: defined by sensor
    IR_PARAM_TYPE_OCC,          // ParamData type: ISP_IR_OCC_PARAM_S; ParamSize: sizeof(ISP_IR_OCC_PARAM_S)
    IR_PARAM_TYPE_KB,           // ParamData type: ISP_IR_KB_PARAM_S; ParamSize: sizeof(ISP_IR_KB_PARAM_S)
    IR_PARAM_TYPE_QGG,          // ParamData type: ISP_IR_QGG_PARAM_S; ParamSize: sizeof(ISP_IR_QGG_PARAM_S)
    IR_PARAM_TYPE_GTM,          // ParamData type: ISP_IR_GTM_PARAM_S; ParamSize: sizeof(ISP_IR_GTM_PARAM_S)

    IR_PARAM_TYPE_BUTT,
} IR_PARAM_TYPE_E;

typedef struct arISP_IR_OCC_PARAM_S
{
    AR_BOOL bIsSingleVal;           /**<RW; use single occ value or not. AR_TRUE: use u32SingleVal for all pixels; AR_FALSE: use stOccBuf.*/
    union
    {
        VIDEO_FRAME_S stOccBuf;     /**<RW; occ buf*/
        AR_U32 u32SingleVal;        /**<RW; single occ value*/
    };
} ISP_IR_OCC_PARAM_S;

typedef struct arISP_IR_KB_PARAM_S
{
    AR_BOOL bKbcEn;             /**<RW; kb correction enable*/
    AR_BOOL bDpcEn;             /**<RW; dead pixel correction enable*/
    VIDEO_FRAME_S stKbBuf;      /**<RW; kb buf*/
} ISP_IR_KB_PARAM_S;

typedef struct arISP_IR_QGG_PARAM_S
{
    AR_BOOL bQggEn;             /**<RW; qgg enable*/
    VIDEO_FRAME_S stQggBuf;     /**<RW; qgg buf*/
    AR_S32 s32QggStrength;
    AR_U32 u32QggCorrection;
    AR_U32 u32QggMin;
    AR_U32 u32QggMax;
} ISP_IR_QGG_PARAM_S;

typedef struct arISP_IR_GTM_PARAM_S
{
    AR_U32 u32GtmContrast;
    AR_S32 s32GtmBright;
} ISP_IR_GTM_PARAM_S;

typedef struct arISP_IR_PARAM_ENTRY_S
{
    AR_VOID *pParamData;
    AR_U32 u32ParamSize;
    IR_PARAM_TYPE_E enParamType;
} ISP_IR_PARAM_ENTRY_S;

typedef struct arISP_IR_PARAMS_S
{
    ISP_IR_PARAM_ENTRY_S astParamEntry[IR_PARAM_TYPE_BUTT];
    AR_U32 u32ParamCnt;
} ISP_IR_PARAMS_S;

typedef struct arIR_ALGO_INPUT_S
{
    VIDEO_FRAME_S stInputFrm;   /**<RW; input frame*/
    AR_BOOL b3DnrFirstFrame;    /**<RW; whether be the first 3dnr frame or not*/
    AR_BOOL bUseHwGtm;          /**<RW; whether use hardware gtm or software gtm*/
} IR_ALGO_INPUT_S;

typedef struct arIR_ALGO_3DNR_OUTPUT_S
{
    VIDEO_FRAME_S stOutputFrm;  /**<RW; ir 3dnr output frame. If not needed, make u64PhyAddr[0] of stOutputFrm zero.*/
} IR_ALGO_3DNR_OUTPUT_S;

typedef struct arIR_ALGO_2DNR_OUTPUT_S
{
    VIDEO_FRAME_S stOutputFrm;  /**<RW; ir 2dnr output frame. If not needed, make u64PhyAddr[0] of stOutputFrm zero.*/
} IR_ALGO_2DNR_OUTPUT_S;

typedef struct arIR_ALGO_GTM_OUTPUT_S
{
    VIDEO_FRAME_S stOutputFrm;  /**<RW; ir gtm output frame. It will be ignroed when bUseHwGtm is true. If not needed, make u64PhyAddr[0] of stOutputFrm zero.*/
    AR_U32 u32HistUpperPos;     /**<RW; histogram analyze information. */
    AR_U32 u32HistLowerPos;     /**<RW; histogram analyze information. Usually switch isp tuning params according (u32HistUpperPos - u32HistLowerPos). */
} IR_ALGO_GTM_OUTPUT_S;

typedef struct arIR_ALGO_OUTPUT_S
{
    IR_ALGO_3DNR_OUTPUT_S st3DnrOutput;     /**<RW; ir 3dnr output*/
    IR_ALGO_2DNR_OUTPUT_S st2DnrOutput;     /**<RW; ir 2dnr output*/
    IR_ALGO_GTM_OUTPUT_S stGtmOutput;       /**<RW; ir gtm output*/
} IR_ALGO_OUTPUT_S;


typedef enum arIR_SNS_OCC_TYPE_E
{
    IR_SNS_OCC_TYPE_EXTERNAL = 0,       /**<外部occ。occ数据由外部发送给sensor。比如高德gst417w就是外部occ。*/
    IR_SNS_OCC_TYPE_INTERNAL,           /**<内部occ。occ数据在sensor内部，不需要从外部发送。比如高德gst212w就是内部occ。*/

    IR_SNS_OCC_TYPE_BUTT,
} IR_SNS_OCC_TYPE_E;

typedef enum arIR_SNS_OCC_MONOTONIC_E
{
    IR_SNS_OCC_MONOTONIC_DECREASE = 0,  /**<occ单调递减。occ值越大，对应的像素值越小。*/
    IR_SNS_OCC_MONOTONIC_INCREASE,      /**<occ单调递增。occ值越大，对应的像素值越大。*/

    IR_SNS_OCC_MONOTONIC_BUTT,
} IR_SNS_OCC_MONOTONIC_E;

typedef struct arIR_SNS_ATTR_S
{
    AR_U32 u32IrParamSize;                  /**<RW; 红外sensor标定参数size，单位为字节。每款sensor自行决定哪些参数需要标定。*/
    IR_SNS_OCC_TYPE_E enOccType;            /**<RW; occ类型。*/
    IR_SNS_OCC_MONOTONIC_E enOccMonotonic;  /**<RW; occ单调性。*/
    AR_U32 u32MinOccVal;                    /**<RW; occ最小值。*/
    AR_U32 u32MaxOccVal;                    /**<RW; occ最大值。*/
    AR_S32 s32OccVerticalOffset;            /**<RW; occ竖直方向偏差。大部分sensor可设置0。但是某些sensor（比如艾睿）的occ与raw图在竖直方向上有偏差。比如occ的第5行对应raw图的第8行，偏差3行，这时要将s32OccVerticalOffset设置为3。*/
    AR_BOOL bOccCalibBySns;                 /**<RW; 是否由sensor驱动来做occ标定。如果为AR_FALSE，则采用sdk实现的通用occ标定方法；如果为AR_TRUE，则采用sensor驱动实现的occ标定方法。如果sensor使用内部occ，不管bOccCalibBySns如何设置，只能采用sensor驱动实现的occ标定方法。*/
    SIZE_S stSnsSize;                       /**<RW; sensor输出raw图的原始尺寸。*/
    RECT_S stRoiRect;                       /**<RW; sensor输出raw图的有效区域。*/
} IR_SNS_ATTR_S;


typedef enum arIR_CALIB_STATUS_E
{
    IR_CALIB_STATUS_UNKNOWN = 0,        /**<标定状态未知。*/
    IR_CALIB_STATUS_ONGOING,            /**<标定进行中。*/
    IR_CALIB_STATUS_SUCCESS,            /**<标定成功。*/
    IR_CALIB_STATUS_FAIL,               /**<标定失败。*/
    IR_CALIB_STATUS_CANCELLED,          /**<标定被取消。*/

    IR_CALIB_STATUS_BUTT,
} IR_CALIB_STATUS_E;

typedef struct arIR_SNS_CALIB_CFG_S
{
    AR_U32 u32FrmInterval;              /**<RW; 红外sensor参数标定帧间隔，表示隔多少帧迭代一次。*/
    AR_U32 u32TargetLowerLimit;         /**<RW; 红外sensor参数标定目标范围下限值。*/
    AR_U32 u32TargetUpperLimit;         /**<RW; 红外sensor参数标定目标范围上限值。*/
    AR_VOID *pUserCfg;                  /**<RW; 红外sensor参数标定用户自定义配置参数。*/
    AR_U32 u32UserCfgSize;              /**<RW; 红外sensor参数标定用户自定义配置参数size。*/
} IR_SNS_CALIB_CFG_S;

typedef struct arIR_SNS_CALIB_RESULT_S
{
    IR_CALIB_STATUS_E enCalibStatus;    /**<RW; 红外sensor参数标定状态。*/
    AR_VOID *pCalibParam;               /**<RW; 标定后的红外sensor参数buf。只有标定成功，才能获取到标定后的红外sensor参数。如果不需要获取标定后的红外sensor参数，pCalibParam可指定为NULL。*/
} IR_SNS_CALIB_RESULT_S;

typedef struct arIR_SNS_CALIB_INPUT_S
{
    IR_SNS_CALIB_CFG_S stCalibCfg;      /**<RW; 红外sensor参数标定配置。*/
    VIDEO_FRAME_S stCalibFrm;           /**<RW; 当前帧数据buf。*/
    AR_U32 u32PixMean;                  /**<RW; 当前帧平均值。*/
} IR_SNS_CALIB_INPUT_S;

typedef struct arIR_SNS_CALIB_OUTPUT_S
{
    IR_CALIB_STATUS_E enCalibStatus;    /**<RW; 红外sensor参数标定状态。*/
    AR_VOID *pCalibParam;               /**<RW; 当前红外sensor参数buf。该buf由sdk分配。如果IR_SNS_ATTR_S中u32IrParamSize为0，那么pCalibParam为NULL。*/
} IR_SNS_CALIB_OUTPUT_S;

typedef struct arIR_SNS_CALIB_CTX_S
{
    IR_SNS_CALIB_INPUT_S stInput;       /**<RW; 红外sensor参数标定上下文输入。*/
    IR_SNS_CALIB_OUTPUT_S stOutput;     /**<RW; 红外sensor参数标定上下文输出。*/
} IR_SNS_CALIB_CTX_S;

typedef enum arIR_OCC_CALIB_METHOD_E
{
    IR_OCC_CALIB_METHOD_STEP_SEARCH,    /**<固定步长搜索*/
    IR_OCC_CALIB_METHOD_BINARY_SEARCH,  /**<二分法搜索*/

    IR_OCC_CALIB_METHOD_BUTT,
} IR_OCC_CALIB_METHOD_E;

typedef enum arIR_OCC_CALIB_COND_E
{
    IR_OCC_CALIB_COND_0,    /**<occ标定完成条件0。如果落在目标区间的像素点数的百分比超过阈值fPixsPercent，则标定完成。*/
    IR_OCC_CALIB_COND_1,    /**<occ标定完成条件1。对每一个像素点，寻找让它最接近目标区间中点的occ。可保证尽可能多的像素点落在目标区间，也可保证最好的像素均匀性，但标定耗时较长。*/

    IR_OCC_CALIB_COND_BUTT,
} IR_OCC_CALIB_COND_E;

typedef struct arIR_OCC_CALIB_CFG_S
{
    IR_OCC_CALIB_METHOD_E enCalibMethod;        /**<RW; occ标定方法。*/
    AR_U32 u32OccSearchStep;                    /**<RW; occ搜索步长，仅当采用固定步长搜索方法时才有效。*/
    AR_U32 u32FrmInterval;                      /**<RW; occ标定帧间隔，表示隔多少帧迭代一次。*/
    AR_U32 u32TargetLowerLimit;                 /**<RW; occ标定目标范围下限值*/
    AR_U32 u32TargetUpperLimit;                 /**<RW; occ标定目标范围上限值*/
    IR_OCC_CALIB_COND_E enCalibCond;            /**<RW; occ标定完成条件。*/
    AR_FLOAT fPixsPercent;                      /**<RW; 点数百分比阈值，仅当完成条件为IR_OCC_CALIB_COND_0时才有效。如果落在目标区间的像素点数百分比超过fPixsPercent，则标定完成。*/
    AR_U32 u32MaxIterateTimes;                  /**<RW; occ迭代次数上限。如果迭代次数超过这个上限还没完成occ标定，则直接停止这次occ标定，并且认为标定失败。*/
} IR_OCC_CALIB_CFG_S;

typedef struct arIR_OCC_CALIB_RESULT_S
{
    IR_CALIB_STATUS_E enCalibStatus;    /**<RW; occ标定状态。*/
    VIDEO_FRAME_S stOccBuf;             /**<RW; 标定后的occ数据buf。只有标定成功，才能获取到标定后的occ数据。如果不需要获取标定后的occ数据，可指定一个空buf。*/
} IR_OCC_CALIB_RESULT_S;

typedef struct arIR_OCC_CALIB_INPUT_S
{
    IR_OCC_CALIB_CFG_S stCalibCfg;      /**<RW; occ标定配置。*/
    VIDEO_FRAME_S stCalibFrm;           /**<RW; 当前帧数据buf。*/
    AR_U32 u32PixMean;                  /**<RW; 当前帧平均值。*/
} IR_OCC_CALIB_INPUT_S;

typedef struct arIR_OCC_CALIB_OUTPUT_S
{
    IR_CALIB_STATUS_E enCalibStatus;    /**<RW; occ标定状态。*/
    VIDEO_FRAME_S stOccBuf;             /**<RW; 当前occ数据buf，仅当sensor使用外部occ才有效。该buf由sdk分配。如果sensor使用内部occ，则该buf为空。*/
} IR_OCC_CALIB_OUTPUT_S;

typedef struct arIR_OCC_CALIB_CTX_S
{
    IR_OCC_CALIB_INPUT_S stInput;       /**<RW; occ标定上下文输入。*/
    IR_OCC_CALIB_OUTPUT_S stOutput;     /**<RW; occ标定上下文输出。*/
} IR_OCC_CALIB_CTX_S;

typedef struct arIR_B_CALIB_CFG_S
{
    AR_U32 u32GrabFrmNum;               /**<RW; 做b标定需要抓取的帧数。对抓取的帧做叠加平均后再做b标定，以减小噪声干扰。*/
    AR_BOOL bMeanAsTargetVal;           /**<RW; 是否以图像平均值作为b标定的目标值。AR_TRUE: 以图像平均值作为目标值；AR_FALSE: 由u32TargetVal指定目标值。*/
    AR_U32 u32TargetVal;                /**<RW; b标定的目标值，仅当bMeanAsTargetVal为AR_FALSE时才有效。*/
} IR_B_CALIB_CFG_S;

typedef struct arIR_B_CALIB_RESULT_S
{
    IR_CALIB_STATUS_E enCalibStatus;    /**<RW; b标定状态。*/
    VIDEO_FRAME_S stBBuf;               /**<RW; 标定后的b数据buf。只有标定成功，才能获取到标定后的b数据。如果不需要获取标定后的b数据，可指定一个空buf。*/
} IR_B_CALIB_RESULT_S;

typedef struct arIR_KB_CALIB_CFG_S
{
    AR_BOOL bLowTempMeanAsTargetVal;    /**<RW; 是否以低温raw图平均值作为KB标定的低温目标值。AR_TRUE: 以raw图平均值作为目标值；AR_FALSE: 由u32LowTempTargetVal指定目标值。*/
    AR_U32 u32LowTempTargetVal;         /**<RW; KB标定的低温目标值，仅当bLowTempMeanAsTargetVal为AR_FALSE时才有效。*/
    AR_BOOL bHighTempMeanAsTargetVal;   /**<RW; 是否以高温raw图平均值作为KB标定的高温目标值。AR_TRUE: 以raw平均值作为目标值；AR_FALSE: 由u32HighTempTargetVal指定目标值。*/
    AR_U32 u32HighTempTargetVal;        /**<RW; KB标定的高温目标值，仅当bHighTempMeanAsTargetVal为AR_FALSE时才有效。*/
} IR_KB_CALIB_CFG_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __AR_COMM_ISP_H__ */
