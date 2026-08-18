#if !defined(__DVP_RX_COLOR_BAR__)
#define __DVP_RX_COLOR_BAR__

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "hal_gpio.h"
#include "hal_i2c.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define color_bar_SNS_ID    (6122)


static ISP_SNS_STATE_S *g_pstcolor_bar[ISP_MAX_PIPE_NUM] = {AR_NULL};
static ISP_CMOS_SENSOR_IMAGE_MODE_S g_ImageMode[ISP_MAX_PIPE_NUM]={0};

#define color_bar_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pstcolor_bar[dev])
#define color_bar_SENSOR_SET_CTX(dev, pstCtx)   (g_pstcolor_bar[dev] = pstCtx)
#define color_bar_SENSOR_RESET_CTX(dev)         (g_pstcolor_bar[dev] = AR_NULL)


static AR_S32 color_bar_power_on(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{

    return 0;
}

static AR_S32 color_bar_power_off(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{

    return 0;
}

static void color_bar_init(VI_PIPE ViPipe)
{
    return;
}

static void color_bar_exit(VI_PIPE ViPipe)
{
	AR_MPI_ISP_SetRegister(ViPipe,0x010f0070, 0x0);
    return;
}

static int vif_reg_read(int addr)
{
  int value;
  addr+=0x01100000;
  AR_MPI_ISP_GetRegister(0,addr,&value);
  return value;
}
static int vif_reg_write(int addr,int value)
{
	addr+=0x01100000;
	AR_MPI_ISP_SetRegister(0,addr,value);
	return 0;
}

static int dvp_rx_outlib_generate_pattern(VI_PIPE ViPipe,int index, int mode)
{

    int input_mode = 0;

    int w=g_ImageMode[ViPipe].u16Width;	
    int h=g_ImageMode[ViPipe].u16Height;
    //write total h, w &actvie h,w
    int value=w|(h<<16);	
    AR_MPI_ISP_SetRegister(ViPipe,0x010f0078, value);
	value=(w+200)|((h+100)<<16);	
    AR_MPI_ISP_SetRegister(ViPipe,0x010f0074, value);

    input_mode = 0x01;
    //set pattern mode and enable pattern
    if (mode == 1)
    {
        AR_MPI_ISP_SetRegister(ViPipe,0x010f0070, 0x04380331);
    }
    else
    {
        AR_MPI_ISP_SetRegister(ViPipe,0x010f0070, 0x04380731);
    }
	
    //set skip seq
    AR_MPI_ISP_GetRegister(ViPipe,0x010f0028, &value);
    value &=~(0xffff);
	value |= 0x1111;	
    AR_MPI_ISP_SetRegister(ViPipe,0x010f0028, value);
	
    //set dvp output view for pattern
    switch (index)
    {
        case 0:
        case 1:
        {
            AR_MPI_ISP_SetRegister(ViPipe,0x010f0018, 0x801 << (16 *index));
            break;
        }

        case 2:
        case 3:
        {
            AR_MPI_ISP_SetRegister(ViPipe,0x010f001c, 0x801 << (16 *(index - 2)));
            break;
        }
        case 4:
        case 5:
        {
            AR_MPI_ISP_SetRegister(ViPipe,0x010f0020, 0x801 << (16 *(index - 4)));
            break;
        }
        case 6:
        case 7:
        {
            AR_MPI_ISP_SetRegister(ViPipe,0x010f0024, 0x801 << (16 *(index - 6)));
            break;
        }
        default:
        break;

    }
	
    return CAM_ERR_SUCCUESS;
}

static void color_bar_on(VI_PIPE ViPipe)
{
	dvp_rx_outlib_generate_pattern(ViPipe,0,0);
    return;
}

static void color_bar_trigger(VI_PIPE ViPipe,int frames)
{
	//dvp_rx_outlib_generate_pattern(ViPipe,0,0);
    return;
}


static AR_S32 color_bar_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstDef);
    color_bar_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    pstDef->stSensorMode.u32SensorID = color_bar_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}

static AR_S32 color_bar_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    color_bar_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (AR_NULL == pastSnsStateCtx) {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (AR_NULL == pastSnsStateCtx) {
            ISP_TRACE(AR_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));
    color_bar_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID color_bar_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    color_bar_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    color_bar_SENSOR_RESET_CTX(ViPipe);
}


static AR_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
	g_ImageMode[ViPipe]=*pstSensorImageMode;
    return AR_SUCCESS;
}


static AR_S32 color_bar_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init     = color_bar_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit     = color_bar_exit;	
	pstSensorExpFunc->pfn_cmos_sns_stream_on   = color_bar_on;
	pstSensorExpFunc->pfn_cmos_sns_trigger     = color_bar_trigger;
    pstSensorExpFunc->pfn_cmos_set_image_mode  = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = color_bar_get_isp_default;
    pstSensorExpFunc->pfn_cmos_sns_power_on    = color_bar_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off   = color_bar_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl         = AR_NULL;	
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;

    return AR_SUCCESS;
}

static AR_S32 color_bar_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 color_bar_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 color_bar_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = color_bar_ctx_init(ViPipe);
    if (AR_SUCCESS != s32Ret)
        return AR_FAILURE;

    stSnsAttrInfo.eSensorId = color_bar_SNS_ID;

    s32Ret  = color_bar_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = color_bar_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = color_bar_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 color_bar_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, color_bar_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, color_bar_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, color_bar_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    color_bar_ctx_exit(ViPipe);

    return AR_SUCCESS;
}

static AR_S32 color_bar_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    return AR_SUCCESS;
}
static SRTU_SENSOR_DEFAULT_ATTR_T default_attr[]=
{
   {
       .stComboAttr=  
	   {
		   .devno = 0,
		   .input_mode = INPUT_MODE_BT1120,
		   .img_rect = {0, 0, 720, 576},
		   .dev_power_attr = {
			   .power_gpio = {1, 2, 2}, //C1_2
		   },
	   },
       .stDevAttr=
       {
		   .enIntfMode = VI_MODE_BT1120_STANDARD,
		   .enWorkMode = VI_WORK_MODE_1Multiplex,
		   .au32ComponentMask = {0xFF000000, 0x0},
		   .enScanMode = VI_SCAN_PROGRESSIVE,
		   .as32AdChnId = {-1, -1, -1, -1},
		   .enDataSeq = VI_DATA_SEQ_YUYV,
		   .enInputDataType = VI_DATA_TYPE_YUV,
		   .bDataReverse = AR_FALSE,
		   .stSize = {720, 576},
		   .stBasAttr = {
			   .stSacleAttr = {
				   .stBasSize = {720, 576},
			   },
			   .stRephaseAttr = {
				   .enHRephaseMode = VI_REPHASE_MODE_NONE,
				   .enVRephaseMode = VI_REPHASE_MODE_NONE,
			   },
		   },
		   .stWDRAttr = {
			   .enWDRMode = WDR_MODE_NONE,
			   .u32CacheLine = 576,
		   },
		   .enDataRate = DATA_RATE_X1,

       },
       .stPipeAttr=
       {
		   .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
		   .bYuvSkip = AR_FALSE,
		   .bIspBypass = AR_FALSE,
		   .u32MaxW = 720,
		   .u32MaxH = 576,
		   .enPixFmt = PIXEL_FORMAT_UYVY_PACKAGE_422,
		   .enCompressMode = COMPRESS_MODE_NONE,
		   .enBitWidth = DATA_BITWIDTH_8,
		   .bNrEn = AR_FALSE,
		   .stNrAttr = {
			   .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
			   .enBitWidth = DATA_BITWIDTH_8,
			   .enNrRefSource = VI_NR_REF_FROM_RFR,
			   .enCompressMode = COMPRESS_MODE_NONE,
		   },
		   .bSharpenEn = AR_FALSE,
		   .stFrameRate = {
			   .s32SrcFrameRate = -1,
			   .s32DstFrameRate = -1,
		   },
		   .bDiscardProPic = AR_FALSE,

       },
       .stPubAttr={
		   .stWndRect = {0, 0, 720, 576},
		   .stSnsSize = {720, 576},
		   .f32FrameRate = 50,
		   .enBayer = AR_BAYER_RGGB,
		   .enWDRMode = WDR_MODE_NONE,
		   .u8SnsMode = 0,
       },
       .stChnAttr={
			.stSize = {720, 576},
			.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420,
			.enDynamicRange = DYNAMIC_RANGE_SDR8,
			.enVideoFormat = VIDEO_FORMAT_LINEAR,
			.enCompressMode = COMPRESS_MODE_NONE,
			.bMirror = 0,
			.bFlip = 0,
			.u32Depth = 0,
			.stFrameRate = {
				.s32SrcFrameRate = -1,
				.s32DstFrameRate = -1,
			},
       },
   },   
};
static int GetDefaultAttr(AR_U8 u8SnsMode,SRTU_SENSOR_DEFAULT_ATTR_T *pstDefaultAttr)
{
    memcpy(pstDefaultAttr,&default_attr[u8SnsMode&0x7f],sizeof(SRTU_SENSOR_DEFAULT_ATTR_T));
	return 0;
}

ISP_SNS_OBJ_S stSnscolor_barObj =
{
    .pfnRegisterCallback    = color_bar_register_callback,
    .pfnUnRegisterCallback  = color_bar_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = color_bar_set_bus_info,
    .pfnSetInit             = AR_NULL,
    .pfnGetDefaultAttr      =GetDefaultAttr,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

