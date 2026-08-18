#if !defined(__ISPINL_CMOS_H_)
#define __ISPINL_CMOS_H_

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


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define ISP_IN_L_ID 100

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastIspInL[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define ISPINL_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastIspInL[dev])
#define ISPINL_SENSOR_SET_CTX(dev, pstCtx)   (g_pastIspInL[dev] = pstCtx)
#define ISPINL_SENSOR_RESET_CTX(dev)         (g_pastIspInL[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_aunIspInLBusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = { .s8I2cDev = 1},
    [1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

static ISP_FSWDR_MODE_E genFSWDRMode[ISP_MAX_PIPE_NUM] =
{
    [0 ... ISP_MAX_PIPE_NUM - 1] = ISP_FSWDR_NORMAL_MODE
};

static AR_U32 gu32MaxTimeGetCnt[ISP_MAX_PIPE_NUM] = {0};
static AR_U32 g_au32InitExposure[ISP_MAX_PIPE_NUM]  = {0};
static AR_U32 g_au32LinesPer500ms[ISP_MAX_PIPE_NUM] = {0};

static AR_U16 g_au16InitWBGain[ISP_MAX_PIPE_NUM][3] = {{0}};
static AR_U16 g_au16SampleRgain[ISP_MAX_PIPE_NUM] = {0};
static AR_U16 g_au16SampleBgain[ISP_MAX_PIPE_NUM] = {0};

typedef struct arISPINL_STATE_S
{
    AR_U8       u8Hcg;
    AR_U32      u32BRL;
    AR_U32      u32RHS1_MAX;
    AR_U32      u32RHS2_MAX;
} ISPINL_STATE_S;

ISPINL_STATE_S g_astisp_in_LState[ISP_MAX_PIPE_NUM] = {{0}};

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
#define ISPINL_FULL_LINES_MAX  (0x3FFFF)
#define ISPINL_FULL_LINES_MAX_2TO1_WDR  (0x8AA)    // considering the YOUT_SIZE and bad frame
#define ISPINL_FULL_LINES_MAX_3TO1_WDR  (0x7FC)
#define ISPINL_HMAX (2200)
#define ISPINL_HMAX_8K_BIG_PIC (4400)

/*****IspInL Register Address*****/

#define ISPINL_VMAX_1080P30_LINEAR  (1125)
#define ISPINL_VMAX_720P60TO30_WDR  (750)
#define ISPINL_VMAX_1080P60TO30_WDR (1220)
#define ISPINL_VMAX_1080P120TO30_WDR (1125)
#define ISPINL_VMAX_1080P60TO30_WDR_BIG_PIC (1390)
#define ISPINL_VMAX_8K_5FPS_LINEAR_BIG_PIC (6666)

//sensor fps mode
#define ISPINL_SENSOR_1080P_30FPS_LINEAR_MODE      (1)
#define ISPINL_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
#define ISPINL_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
#define ISPINL_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)
#define ISPINL_SENSOR_1080P_30FPS_BIG_PIC_LINEAR_MODE      (5)
#define ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE      (6)
#define ISPINL_SENSOR_1080P_30FPS_BIG_PIC_2t1_WDR_MODE     (7)

#define ISPINL_RES_IS_1280x1080(w, h)      ((w) <= 1280 && (h) <= 1080)
#define ISPINL_RES_IS_1080P_WDR2TO1_LINE_BIG_PIC(w, h)      ((w) <= 1184 && (h) <= 2678)
#define ISPINL_RES_IS_4064x6048(w, h)      ((w) <= 4064 && (h) <= 6048)
#define ISPINL_RES_IS_720P(w, h)       ((w) <= 1280 && (h) <= 720)
#define ISPINL_RES_IS_1080P(w, h)      ((w) <= 1920 && (h) <= 1080)
#define SENSOR_NM (0)

static AR_S32 isp_in_l_cmos_power_on(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
    ar_always("%s \n",__FUNCTION__);
	return 0;
}

static AR_S32 isp_in_l_cmos_power_off(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
    ar_always("%s \n",__FUNCTION__);
	return 0;
}
static AR_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

	pstAeSnsDft->f32Fps=30;
	pstAeSnsDft->f32MaxFps=30;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = ISPINL_FULL_LINES_MAX;
	pstAeSnsDft->u32Hmax=ISPINL_HMAX;
    if(pstSnsState->u8ImgMode == ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE)
    {
        pstAeSnsDft->u32Hmax = ISPINL_HMAX_8K_BIG_PIC;
    }

    if (g_au32LinesPer500ms[ViPipe] == 0)
    {
        pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * pstAeSnsDft->f32Fps / 2;
    }
    else
    {
        pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];
    }


    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/

            pstAeSnsDft->u32MaxAgain = 2886024;
            pstAeSnsDft->u32MinAgain = 0;
            pstAeSnsDft->u32MaxDgain = 1024;
            pstAeSnsDft->u32MinDgain = 0;
            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd;
            pstAeSnsDft->u32MinIntTime = 1;
            break;
            // pstAeSnsDft->u32MaxAgain = 62564;
            // pstAeSnsDft->u32MinAgain = 1024;
            // pstAeSnsDft->u32MaxDgain = 38577;
            // pstAeSnsDft->u32MinDgain = 1024;
            // pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            // pstAeSnsDft->u32MinIntTime = 1;
            // break;

        case WDR_MODE_2To1_LINE:

            if(pstSnsState->u8ImgMode == ISPINL_SENSOR_1080P_30FPS_BIG_PIC_2t1_WDR_MODE)
            {
                pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd;
                pstAeSnsDft->u32FullLinesShort = 461;
                pstAeSnsDft->u32LFMaxShortTime = 461;
                pstAeSnsDft->u32LFMinExposure = 1;
                pstAeSnsDft->u32MinIntTime = 1;
                pstAeSnsDft->u32MaxAgain = 2886024;
                pstAeSnsDft->u32MinAgain = 0;
                pstAeSnsDft->u32MaxDgain = 1024;
                pstAeSnsDft->u32MinDgain = 0;

            }else if(pstSnsState->u8ImgMode == ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE)
            {
                pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
                pstAeSnsDft->u32MinIntTime = 2;
                pstAeSnsDft->u32MaxAgain = 62564;
                pstAeSnsDft->u32MinAgain = 1024;
                pstAeSnsDft->u32MaxDgain = 38577;
                pstAeSnsDft->u32MinDgain = 1024;
            }

		    break;

        case WDR_MODE_3To1_LINE:

            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            pstAeSnsDft->u32MinIntTime = 3;
            pstAeSnsDft->u32MaxAgain = 62564;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxDgain = 38577;
            pstAeSnsDft->u32MinDgain = 1024;

    }

    return AR_SUCCESS;
}


/* the function of sensor set fps */
static AR_VOID cmos_fps_set(VI_PIPE ViPipe, AR_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    // AR_U32 u32VMAX = ISPINL_VMAX_1080P30_LINEAR;
	pstAeSnsDft->f32Fps=f32Fps;
	pstAeSnsDft->f32MaxFps=f32Fps;
    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);
    return;
}

static AR_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, AR_U32 u32FullLines,
                                       AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static AR_VOID cmos_inttime_update(VI_PIPE ViPipe, AR_U32 u32IntTime)
{
}
static AR_VOID cmos_again_calc_table(VI_PIPE ViPipe, AR_U32 *pu32AgainLin, AR_U32 *pu32AgainDb)
{
}
static AR_VOID cmos_dgain_calc_table(VI_PIPE ViPipe, AR_U32 *pu32DgainLin, AR_U32 *pu32DgainDb)
{
}
static AR_VOID cmos_gains_update(VI_PIPE ViPipe, AR_U32 u32Again, AR_U32 u32Dgain)
{
}
static AR_VOID cmos_get_inttime_max(VI_PIPE ViPipe, AR_U16 u16ManRatioEnable, AR_U32 *au32Ratio, AR_U32 *au32IntTimeMax, AR_U32 *au32IntTimeMin, AR_U32 *pu32LFMaxIntTime)
{
}

/* Only used in LINE_WDR mode */
static AR_VOID cmos_ae_fswdr_attr_set(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
    CMOS_CHECK_POINTER_VOID(pstAeFSWDRAttr);

    genFSWDRMode[ViPipe] = pstAeFSWDRAttr->enFSWDRMode;
    gu32MaxTimeGetCnt[ViPipe] = 0;
}

static AR_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return AR_SUCCESS;
}
static AR_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAwbSnsDft);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);
    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    return AR_SUCCESS;
}

static AR_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return AR_SUCCESS;
}


static ISP_CMOS_DNG_COLORPARAM_S g_stDngColorParam =
{
    {378, 256, 430},
    {439, 256, 439}
};


static AR_S32 cmos_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstDef);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
            if(pstSnsState->u8ImgMode == ISPINL_SENSOR_1080P_30FPS_BIG_PIC_LINEAR_MODE)
            {
                strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80_l.bin");
            }
            else if(pstSnsState->u8ImgMode == ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE)
            {
                strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80_l.bin");
            }
		break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_3To1_LINE:
            if(pstSnsState->u8ImgMode == ISPINL_SENSOR_1080P_30FPS_BIG_PIC_2t1_WDR_MODE)
            {
                strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80_hdr_l.bin");
            }
            else if(pstSnsState->u8ImgMode == ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE)
            {
                strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80_hdr_l.bin");
            }

        break;
    }

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        pstDef->stWdrSwitchAttr.au32ExpRatio[0] = 0x40;
        pstDef->stWdrSwitchAttr.au32ExpRatio[1] = 0x40;
        pstDef->stWdrSwitchAttr.au32ExpRatio[2] = 0x40;

        if(pstSnsState->u8ImgMode == ISPINL_SENSOR_1080P_30FPS_BIG_PIC_2t1_WDR_MODE)
        {
            pstDef->stWdrSwitchAttr.au32ShortOffset = 475;
            pstDef->stWdrSwitchAttr.au32MidOffset = 475;
            pstDef->stWdrSwitchAttr.au32LongOffset = 14;

        }
        else if(pstSnsState->u8ImgMode == ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE)
        {
        }
    }

    pstDef->stSensorMode.u32SensorID = ISP_IN_L_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    memcpy(&pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 12;
    pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 4095;

    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimCols = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatCols = 2;
    pstDef->stSensorMode.stDngRawFormat.enCfaLayout = CFALAYOUT_TYPE_RECTANGULAR;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[2] = 2;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[2] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[3] = 2;
    pstDef->stSensorMode.bValidDngRawFormat = AR_TRUE;

    return AR_SUCCESS;
}

static AR_VOID cmos_set_pixel_detect(VI_PIPE ViPipe, AR_BOOL bEnable)
{
}

static AR_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, AR_U8 u8Mode)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = AR_FALSE;

    switch (u8Mode)
    {
        case WDR_MODE_NONE:
            pstSnsState->enWDRMode = WDR_MODE_NONE;
            if (ISPINL_SENSOR_1080P_30FPS_BIG_PIC_LINEAR_MODE == pstSnsState->u8ImgMode)
            {
                pstSnsState->u32FLStd = ISPINL_VMAX_1080P30_LINEAR;

            }else if (ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE == pstSnsState->u8ImgMode)
            {
                pstSnsState->u32FLStd = ISPINL_VMAX_8K_5FPS_LINEAR_BIG_PIC;
            }
            ISP_TRACE(AR_DBG_ERR, "linear mode\n");
            break;
        case WDR_MODE_2To1_LINE:
            pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
            if (ISPINL_SENSOR_1080P_30FPS_2t1_WDR_MODE == pstSnsState->u8ImgMode)
            {
                pstSnsState->u32FLStd = ISPINL_VMAX_1080P60TO30_WDR * 2;
                ISP_TRACE(AR_DBG_ERR, "2to1 line WDR 1080p mode(60fps->30fps)\n");
            }
            else if (ISPINL_SENSOR_720P_30FPS_2t1_WDR_MODE == pstSnsState->u8ImgMode)
            {
                pstSnsState->u32FLStd = ISPINL_VMAX_720P60TO30_WDR * 2;
                ISP_TRACE(AR_DBG_ERR, "2to1 line WDR 720p mode(60fps->30fps)\n");
            }
            break;

        case WDR_MODE_3To1_LINE:
            pstSnsState->enWDRMode = WDR_MODE_3To1_LINE;
            pstSnsState->u32FLStd  = ISPINL_VMAX_1080P120TO30_WDR * 4;
            ISP_TRACE(AR_DBG_ERR, "3to1 line WDR 1080p mode(120fps->30fps)\n");
            break;

        default:
            ISP_TRACE(AR_DBG_ERR, "NOT support this mode!\n");
            return AR_FAILURE;
    }
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    memset(pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime));
    return AR_SUCCESS;
}

static AR_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    // AR_S32 i;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSnsRegsInfo);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);
    return AR_SUCCESS;
}

static AR_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    AR_U8 u8SensorImageMode = 0;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = AR_FALSE;

    if (pstSensorImageMode->f32Fps <= 30)
    {
        if (WDR_MODE_NONE == pstSnsState->enWDRMode)
        {
            if(ISPINL_RES_IS_1280x1080(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = ISPINL_SENSOR_1080P_30FPS_BIG_PIC_LINEAR_MODE;
            }
            else if (ISPINL_RES_IS_4064x6048(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = ISPINL_SENSOR_8K_5FPS_BIG_PIC_LINEAR_MODE;
                pstSnsState->u32FLStd = ISPINL_VMAX_8K_5FPS_LINEAR_BIG_PIC;
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
        {
            if (ISPINL_RES_IS_720P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = ISPINL_SENSOR_720P_30FPS_2t1_WDR_MODE;
            }
            else if (ISPINL_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = ISPINL_SENSOR_1080P_30FPS_2t1_WDR_MODE;
            }
            else if (ISPINL_RES_IS_1080P_WDR2TO1_LINE_BIG_PIC(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = ISPINL_SENSOR_1080P_30FPS_BIG_PIC_2t1_WDR_MODE;
                pstSnsState->u32FLStd = ISPINL_VMAX_1080P60TO30_WDR_BIG_PIC * 2;
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }
        else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
        {
            if (ISPINL_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = ISPINL_SENSOR_1080P_30FPS_3t1_WDR_MODE;
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }
        else
        {
            ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                      pstSensorImageMode->u16Width,
                      pstSensorImageMode->u16Height,
                      pstSensorImageMode->f32Fps,
                      pstSnsState->enWDRMode);
            return AR_FAILURE;
        }
    }
    else
    {
    }

    if ((AR_TRUE == pstSnsState->bInit) && (u8SensorImageMode == pstSnsState->u8ImgMode))
    {
        /* Don't need to switch SensorImageMode */
        return AR_FAILURE;
    }

    pstSnsState->u8ImgMode = u8SensorImageMode;

    return AR_SUCCESS;
}

static AR_VOID sensor_global_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    ISPINL_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit = AR_FALSE;
    pstSnsState->bSyncInit = AR_FALSE;
    pstSnsState->u8ImgMode = ISPINL_SENSOR_1080P_30FPS_LINEAR_MODE;
    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->u32FLStd = ISPINL_VMAX_1080P30_LINEAR;
    pstSnsState->au32FL[0] = ISPINL_VMAX_1080P30_LINEAR;
    pstSnsState->au32FL[1] = ISPINL_VMAX_1080P30_LINEAR;

    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
}

static AR_S32 isp_in_l_cmos_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
{
    if (!pSensorCtl)
    {
         ISP_TRACE(AR_DBG_ERR, "sensor ctl failed!\n");
         return AR_ERR_ISP_INVALID_ADDR;
    }
    return 0;

}

static void isp_in_l_init(VI_PIPE ViPipe)
{
   ar_always("%s \n",__FUNCTION__);
   return;
}

static void isp_in_l_exit(VI_PIPE ViPipe)
{
    ar_always("%s \n",__FUNCTION__);
    return;
}

static AR_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);

    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = isp_in_l_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = isp_in_l_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;

    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;
    pstSensorExpFunc->pfn_cmos_sns_power_on = isp_in_l_cmos_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = isp_in_l_cmos_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = isp_in_l_cmos_sensor_ctl;
    return AR_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static AR_S32 isp_in_l_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_aunIspInLBusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

    return AR_SUCCESS;
}

static AR_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    ISPINL_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (AR_NULL == pastSnsStateCtx)
    {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (AR_NULL == pastSnsStateCtx)
        {
            ISP_TRACE(AR_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

    ISPINL_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    ISPINL_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    ISPINL_SENSOR_RESET_CTX(ViPipe);
}

static AR_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = sensor_ctx_init(ViPipe);

    if (AR_SUCCESS != s32Ret)
    {
        return AR_FAILURE;
    }

    stSnsAttrInfo.eSensorId = ISP_IN_L_ID;

    s32Ret  = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);

    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);

    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }
    s32Ret  = cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }
    return AR_SUCCESS;
}

static AR_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, ISP_IN_L_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, ISP_IN_L_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, ISP_IN_L_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(ViPipe);

    return AR_SUCCESS;
}

static AR_S32 sensor_set_init(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr)
{
    CMOS_CHECK_POINTER(pstInitAttr);

    g_au32InitExposure[ViPipe] = pstInitAttr->u32Exposure;
    g_au32LinesPer500ms[ViPipe] = pstInitAttr->u32LinesPer500ms;
    g_au16InitWBGain[ViPipe][0] = pstInitAttr->u16WBRgain;
    g_au16InitWBGain[ViPipe][1] = pstInitAttr->u16WBGgain;
    g_au16InitWBGain[ViPipe][2] = pstInitAttr->u16WBBgain;
    g_au16SampleRgain[ViPipe] = pstInitAttr->u16SampleRgain;
    g_au16SampleBgain[ViPipe] = pstInitAttr->u16SampleBgain;

    return AR_SUCCESS;
}
static void isp_in_l_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

static void isp_in_l_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}
static int isp_in_l_read_register(VI_PIPE ViPipe, int addr)
{
    return AR_SUCCESS;
}
static int isp_in_l_write_register(VI_PIPE ViPipe, int addr, int data)
{
    return AR_SUCCESS;
}

ISP_SNS_OBJ_S stSnsIspInLObj =
{
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = isp_in_l_standby,
    .pfnRestart             = isp_in_l_restart,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = isp_in_l_write_register,
    .pfnReadReg             = isp_in_l_read_register,
    .pfnSetBusInfo          = isp_in_l_set_bus_info,
    .pfnSetInit             = sensor_set_init
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __ISPINL_CMOS_H_ */
