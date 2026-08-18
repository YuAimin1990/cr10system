#include "ir_sample_ctx.h"
#include "gst412c/gst412c.h"


static AR_S32 Gst412cRawProcess(IR_SAMPLE_CTX_S *pstIrSampleCtx, VIDEO_FRAME_INFO_S *pstFrmInfo);
static AR_S32 Gst412cCfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType, AR_U32 u32Width, AR_U32 u32Height);
static AR_S32 Gst412cIrCmdGetIrSnsParam(IR_SAMPLE_CTX_S *pstIrSampleCtx);


IR_SAMPLE_CTX_S stGst412cIrSampleCtx = {
    .stIrViGrpCfg = {
        .aViDev = {0, 1, 2, 3, 4},
        .aViPipe = {0, 1, 2, 3, 4},
        .aViChn = {0, 2, 4, 0, 0},
        .aenSnsType = {
            SONY_IMX290_MIPI_2M_30FPS_12BIT,        // visible light
            GST412C_IR_400X308,                     // ir raw
            ISP_VIN_IR_THERMAL_GST417W,             // ir thermal
            ISP_VIN1_IR_GST417W,                    // ir isp1
            ISP_VIN2_IR_GST417W,                    // ir isp2
        },
        .aenInPixFmt = {
            PIXEL_FORMAT_RGB_BAYER_12BPP,       // visible light
            PIXEL_FORMAT_RGB_BAYER_14BPP,       // ir raw
            PIXEL_FORMAT_RGB_BAYER_14BPP,       // ir thermal
            PIXEL_FORMAT_RGB_BAYER_12BPP,       // ir isp1
            PIXEL_FORMAT_RGB_BAYER_8BPP,        // ir isp2
        },
        .astInSize = {
            {1920, 1080},                       // visible light
            {400, 308},                         // ir raw
            {408, 308},                         // ir thermal
            {408, 300},                         // ir isp1
            {408, 300},                         // ir isp2
        },
        .aenOutPixFmt = {
            PIXEL_FORMAT_YVU_PLANAR_420,        // visible light
            PIXEL_FORMAT_RGB_BAYER_14BPP,       // ir raw
            PIXEL_FORMAT_RGB_BAYER_14BPP,       // ir thermal
            PIXEL_FORMAT_YVU_PLANAR_420,        // ir isp1
            PIXEL_FORMAT_YVU_PLANAR_420,        // ir isp2
        },
        .astOutSize = {
            {640, 360},                         // visible light
            {400, 308},                         // ir raw
            {408, 300},                         // ir thermal
            {408, 300},                         // ir isp1
            {408, 300},                         // ir isp2
        },
        .as32BusId = {0, 2, 0, 0, 0},
        .as32MipiDev = {0, 0, 0, 0, 0},

        .s32VisibleLightIdx = -1,
        .s32IrRawIdx = 1,
        .s32IrThrmlIdx = 2,
        .s32IrIsp1Idx = 3,
        .s32IrIsp2Idx = 4,
    },
    .u32IrSnsParamSize = 2,
    .stIrSnsRoi = {
        .s32X = 0,
        .s32Y = 4,
        .u32Width = 400,
        .u32Height = 300,
    },
    .stIrIspRoi = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = 400,
        .u32Height = 300,
    },
    .strInitSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_gst412c/gst412c_sns_calib.bin",
    .strInitOccCalibFname = "/usrdata/local/factory/tunning/cam_gst412c/gst412c_occ_calib.bin",
    .strInitKbCalibFname = "/usrdata/local/factory/tunning/cam_gst412c/gst412c_kb_calib.bin",
    .bUseRawTempBuf = AR_FALSE,
    .bUseThrmlTempBuf = AR_TRUE,

    .bAutoCalib = AR_TRUE,
    .u32AutoCalibIntervalMs = 40000,

    .stIrSnsCalibCfg = {
        .u32FrmInterval = 4,
        .u32TargetLowerLimit = 8192 - 1024,
        .u32TargetUpperLimit = 8192 + 1024,
    },

    .stIrOccCalibCfg = {
        .enCalibMethod = IR_OCC_CALIB_METHOD_BINARY_SEARCH,
        .u32OccSearchStep = 1,
        .u32FrmInterval = 4,
        .u32TargetLowerLimit = 8192 - 512,
        .u32TargetUpperLimit = 8192 + 512,
        .enCalibCond = IR_OCC_CALIB_COND_1,
        .fPixsPercent = 99.9f,
        .u32MaxIterateTimes = 8,
    },

    .stIrBCalibCfg = {
        .u32GrabFrmNum = 4,
        .bMeanAsTargetVal = AR_TRUE,
    },

    .stIrKBCalibCfg = {
        .bLowTempMeanAsTargetVal = AR_TRUE,
        .bHighTempMeanAsTargetVal = AR_TRUE,
    },

    .enFirstIrDynamicCalibType = IR_DYNAMIC_CALIB_TYPE_B,
    .enIrDynamicCalibType = IR_DYNAMIC_CALIB_TYPE_B,


    .pfnRawProcess = Gst412cRawProcess,
    .pfnCfgIrViTopology = Gst412cCfgIrViTopology,
    .pfnIrCmdGetIrSnsParam = Gst412cIrCmdGetIrSnsParam,
};


static AR_S32 Gst412cRawProcess(IR_SAMPLE_CTX_S *pstIrSampleCtx, VIDEO_FRAME_INFO_S *pstFrmInfo)
{
	#if 0
    AR_U32 u32CycleWidth = pstFrmInfo->stVFrame.u32Width * 2 / sizeof(AR_U64);
    AR_U32 u32LineEndOffset = pstFrmInfo->stVFrame.u32Stride[0] / sizeof(AR_U64) - u32CycleWidth;
    AR_U64 *p = (AR_U64 *)pstFrmInfo->stVFrame.u64VirAddr[0];
    AR_U32 i = 0;
    AR_U32 j = 0;

    for (i = 0; i < pstFrmInfo->stVFrame.u32Height; i++) {
        for (j = 0; j < u32CycleWidth; j++) {
            *p = 0x3fff3fff3fff3fffULL - *p;
            p++;
        }
        p += u32LineEndOffset;
    }
	#endif

    return 0;
}

static AR_S32 Gst412cCfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType, AR_U32 u32Width, AR_U32 u32Height)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    switch (enIrViTopoType) {
    case IR_VI_TOPOLOGY_TYPE_0:     // ir_raw --> ir_thermal --> ir_isp1 --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST417W;
        break;

    case IR_VI_TOPOLOGY_TYPE_1:     // ir_raw --> (ir_isp1 & ir_thermal) --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST417W_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = 408;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = 308;
        break;

    case IR_VI_TOPOLOGY_TYPE_2:     // ir_raw --> ir_thermal --> ir_isp1
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST417W;
        break;

    case IR_VI_TOPOLOGY_TYPE_3:     // ir_raw --> (ir_isp1 & ir_thermal)
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST417W_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = 408;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = 308;
        break;

    case IR_VI_TOPOLOGY_TYPE_4:     // ir_raw --> ir_thermal
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = -1;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        break;

    case IR_VI_TOPOLOGY_TYPE_5:     // ir_raw
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = -1;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        break;

    default:
        ar_err("unsupported ir vi topology type[%d]", enIrViTopoType);
        s32Ret = -1;
        break;
    }

    return s32Ret;
}

static AR_S32 Gst412cIrCmdGetIrSnsParam(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U8 au8IrSnsParam[2] = {0};
    ISP_IR_PARAMS_S stIrParams;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_SENSOR;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)au8IrSnsParam;
    stIrParams.astParamEntry[0].u32ParamSize = 2;

    s32Ret = AR_MPI_ISP_GetIrParams(pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx], &stIrParams);
    if (s32Ret) {
        ar_err("get gst417w ir sns param failed");
        return -1;
    }

    printf("get gst412w ir sns param successfully, ra_sel[0x%02x] hssd[0x%02x]\n",
        au8IrSnsParam[0], au8IrSnsParam[1]);

    return 0;
}


