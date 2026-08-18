#include "ir_sample_ctx.h"
#include "rtd6122c/rtd6122c.h"


static AR_S32 Rtd6122cCfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType, AR_U32 u32Width, AR_U32 u32Height);
static AR_S32 Rtd6122cIrCmdGetIrSnsParam(IR_SAMPLE_CTX_S *pstIrSampleCtx);


IR_SAMPLE_CTX_S stRtd6122cIrSampleCtx = {
    .stIrViGrpCfg = {
        .aViDev = {0, 1, 2, 3, 4},
        .aViPipe = {0, 1, 2, 3, 4},
        .aViChn = {0, 2, 4, 0, 0},
        .aenSnsType = {
            SONY_IMX290_MIPI_2M_30FPS_12BIT,        // visible light
            RTD6122C_IR_656X522,                    // ir raw
            ISP_VIN_IR_THERMAL_RTD6122C,            // ir thermal
            ISP_VIN1_IR_RTD6122C,                   // ir isp1
            ISP_VIN2_IR_RTD6122C,                   // ir isp2
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
            {656, 522},                         // ir raw
            {656, 522},                         // ir thermal
            {640, 512},                         // ir isp1
            {640, 512},                         // ir isp2
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
            {656, 522},                         // ir raw
            {640, 512},                         // ir thermal
            {640, 512},                         // ir isp1
            {640, 512},                         // ir isp2
        },
        .as32BusId = {0, 0, 0, 0, 0},
        .as32MipiDev = {0, 0, 0, 0, 0},

        .s32VisibleLightIdx = -1,
        .s32IrRawIdx = 1,
        .s32IrThrmlIdx = 2,
        .s32IrIsp1Idx = 3,
        .s32IrIsp2Idx = 4,
    },
    .u32IrSnsParamSize = 51,
    .stIrSnsRoi = {
        .s32X = 10,
        .s32Y = 8,
        .u32Width = 640,
        .u32Height = 512,
    },
    .stIrIspRoi = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = 640,
        .u32Height = 512,
    },
    .strInitSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_rtd6122c/rtd6122c_sns_calib.bin",
    .strInitOccCalibFname = "/usrdata/local/factory/tunning/cam_rtd6122c/rtd6122c_occ_calib.bin",
    .strInitKbCalibFname = "/usrdata/local/factory/tunning/cam_rtd6122c/rtd6122c_kb_calib.bin",
    .bUseRawTempBuf = AR_FALSE,
    .bUseThrmlTempBuf = AR_TRUE,

    .bAutoCalib = AR_FALSE,
    .u32AutoCalibIntervalMs = 40000,

    .stIrSnsCalibCfg = {
        .u32FrmInterval = 8,
        .u32TargetLowerLimit = 8192 - 1024,
        .u32TargetUpperLimit = 8192 + 1024,
    },

    .stIrOccCalibCfg = {
        .enCalibMethod = IR_OCC_CALIB_METHOD_BINARY_SEARCH,
        .u32OccSearchStep = 1,
        .u32FrmInterval = 8,
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

    .enFirstIrDynamicCalibType = IR_DYNAMIC_CALIB_TYPE_OCC_B,
    .enIrDynamicCalibType = IR_DYNAMIC_CALIB_TYPE_OCC_B,

    .pfnRawProcess = NULL,
    .pfnCfgIrViTopology = Rtd6122cCfgIrViTopology,
    .pfnIrCmdGetIrSnsParam = Rtd6122cIrCmdGetIrSnsParam,
};


static AR_S32 Rtd6122cCfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
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
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_RTD6122C;
        break;

    case IR_VI_TOPOLOGY_TYPE_1:     // ir_raw --> (ir_isp1 & ir_thermal) --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_RTD6122C_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = 656;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = 522;
        break;

    case IR_VI_TOPOLOGY_TYPE_2:     // ir_raw --> ir_thermal --> ir_isp1
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_RTD6122C;
        break;

    case IR_VI_TOPOLOGY_TYPE_3:     // ir_raw --> (ir_isp1 & ir_thermal)
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_RTD6122C_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = 656;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = 522;
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

static AR_S32 Rtd6122cIrCmdGetIrSnsParam(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U32 *pau32IrSnsParam = NULL;
    ISP_IR_PARAMS_S stIrParams;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    pau32IrSnsParam = (AR_U32 *)malloc(pstIrSampleCtx->u32IrSnsParamSize);
    if (pau32IrSnsParam == NULL) {
        ar_err("allocate ir_sns_param buffer failed");
        return -1;
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_SENSOR;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)pau32IrSnsParam;
    stIrParams.astParamEntry[0].u32ParamSize = pstIrSampleCtx->u32IrSnsParamSize;

    s32Ret = AR_MPI_ISP_GetIrParams(pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx], &stIrParams);
    if (s32Ret) {
        ar_err("get rtd6122c ir sns param failed");
        return -1;
    }

    printf("get rtd6122c ir sns param successfully, [0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%02x%02x%02x]\n",
        pau32IrSnsParam[0], pau32IrSnsParam[1], pau32IrSnsParam[2], pau32IrSnsParam[3],
        pau32IrSnsParam[4], pau32IrSnsParam[5], pau32IrSnsParam[6], pau32IrSnsParam[7],
        pau32IrSnsParam[8], pau32IrSnsParam[9], pau32IrSnsParam[10], pau32IrSnsParam[11],
        *((AR_U8 *)pau32IrSnsParam), *(((AR_U8 *)pau32IrSnsParam) + 1),
        *(((AR_U8 *)pau32IrSnsParam) + 2));

    free(pau32IrSnsParam);
    return 0;
}


