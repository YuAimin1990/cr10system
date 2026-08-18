#include "ir_sample_ctx.h"
#include "gst212w4/gst212w4.h"


typedef enum {
    GST212W4_INIT_SHUTTER_CLOSE = 0,
    GST212W4_INIT_OCC_CALIB_START,
    GST212W4_INIT_OCC_CALIB_QUERY,
    GST212W4_INIT_B_CALIB,
    GST212W4_INIT_SHUTTER_OPEN,
    GST212W4_INIT_DONE,
} GST212W4_INIT_STATE_E;


static AR_S32 Gst212w4RawProcess(IR_SAMPLE_CTX_S *pstIrSampleCtx, VIDEO_FRAME_INFO_S *pstFrmInfo);
static AR_S32 Gst212w4CfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType);
static AR_S32 Gst212w4IrCmdGetIrSnsParam(IR_SAMPLE_CTX_S *pstIrSampleCtx);


IR_SAMPLE_CTX_S stGst212w4IrSampleCtx = {
    .stIrViGrpCfg = {
        .aViDev = {0, 1, 2, 3, 4},
        .aViPipe = {0, 1, 2, 3, 4},
        .aViChn = {0, 2, 4, 0, 0},
        .aenSnsType = {
            SONY_IMX290_MIPI_2M_30FPS_12BIT,        // visible light
            GST212W4_IR_256X200,                    // ir raw
            ISP_VIN_IR_THERMAL_GST212W4,            // ir thermal
            ISP_VIN1_IR_GST212W4,                   // ir isp1
            ISP_VIN2_IR_GST212W4,                   // ir isp2
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
            {256, 200},                         // ir raw
            {256, 200},                     // ir thermal
            {256, 192},                         // ir isp1
            {256, 192},                         // ir isp2
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
            {256, 200},                         // ir raw
            {256, 192},                         // ir thermal
            {256, 192},                         // ir isp1
            {256, 192},                         // ir isp2
        },
        .as32BusId = {0, 2, 0, 0, 0},
        .as32MipiDev = {0, 0, 0, 0, 0},

        .s32VisibleLightIdx = -1,
        .s32IrRawIdx = 1,
        .s32IrThrmlIdx = 2,
        .s32IrIsp1Idx = 3,
        .s32IrIsp2Idx = 4,
    },
    .u32IrSnsParamSize = 7,
    .stIrSnsRoi = {
        .s32X = 0,
        .s32Y = 4,
        .u32Width = 256,
        .u32Height = 192,
    },
    .stIrIspRoi = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = 256,
        .u32Height = 192,
    },
    .strInitSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_gst212w4/gst212w4_sns_calib.bin",
    .strInitOccCalibFname = NULL,       // gst212w4 has internal occ, so external occ is not needed
    .strInitKbCalibFname = "/usrdata/local/factory/tunning/cam_gst212w4/gst212w4_kb_calib.bin",
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
        .u32FrmInterval = 4,
    },

    .stIrBCalibCfg = {
        .u32GrabFrmNum = 4,
        .bMeanAsTargetVal = AR_TRUE,
    },

    .stIrKBCalibCfg = {
        .bLowTempMeanAsTargetVal = AR_TRUE,
        .bHighTempMeanAsTargetVal = AR_TRUE,
    },

    .enFirstIrDynamicCalibType = IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B,
    .enIrDynamicCalibType = IR_DYNAMIC_CALIB_TYPE_OCC_B,

    .pfnRawProcess = Gst212w4RawProcess,
    .pfnCfgIrViTopology = Gst212w4CfgIrViTopology,
    .pfnIrCmdGetIrSnsParam = Gst212w4IrCmdGetIrSnsParam,
};


static AR_S32 Gst212w4RawProcess(IR_SAMPLE_CTX_S *pstIrSampleCtx, VIDEO_FRAME_INFO_S *pstFrmInfo)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32CycleWidth = pstFrmInfo->stVFrame.u32Width * 2 / sizeof(AR_U64);
    AR_U32 u32LineEndOffset = pstFrmInfo->stVFrame.u32Stride[0] / sizeof(AR_U64) - u32CycleWidth;
    AR_U64 *p = (AR_U64 *)pstFrmInfo->stVFrame.u64VirAddr[0];
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrRawIdx];
    AR_U32 i = 0;
    AR_U32 j = 0;

    for (i = 0; i < pstFrmInfo->stVFrame.u32Height; i++) {
        for (j = 0; j < u32CycleWidth; j++) {
            *p = 0x3fff3fff3fff3fffULL - *p;
            p++;
        }
        p += u32LineEndOffset;
    }

    return 0;
}

static AR_S32 Gst212w4CfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType)
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
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST212W4;
        break;

    case IR_VI_TOPOLOGY_TYPE_1:     // ir_raw --> (ir_isp1 & ir_thermal) --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST212W4_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = 256;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = 200;
        break;

    case IR_VI_TOPOLOGY_TYPE_2:     // ir_raw --> ir_thermal --> ir_isp1
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST212W4;
        break;

    case IR_VI_TOPOLOGY_TYPE_3:     // ir_raw --> (ir_isp1 & ir_thermal)
        pstIrViGrpCfg->s32IrRawIdx = 1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_GST212W4_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = 256;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = 200;
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

static AR_S32 Gst212w4IrCmdGetIrSnsParam(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U8 au8IrSnsParam[7] = {0};
    ISP_IR_PARAMS_S stIrParams;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_SENSOR;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)au8IrSnsParam;
    stIrParams.astParamEntry[0].u32ParamSize = 7;

    s32Ret = AR_MPI_ISP_GetIrParams(pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx], &stIrParams);
    if (s32Ret) {
        ar_err("get gst212w4 ir sns param failed");
        return -1;
    }

    printf("get gst212w4 ir sns param successfully, ra_adj[0x%02x%02x%02x%02x%02x] hssd[0x%02x] ad_step[0x%02x]\n",
        au8IrSnsParam[4], au8IrSnsParam[3], au8IrSnsParam[2], au8IrSnsParam[1],
        au8IrSnsParam[0], au8IrSnsParam[5], au8IrSnsParam[6]);

    return 0;
}


