#include "ir_sample_ctx.h"

static AR_S32 OfflineCfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType, AR_U32 u32Width, AR_U32 u32Height);


IR_SAMPLE_CTX_S stOfflineIrSampleCtx = {
    .stIrViGrpCfg = {
        .aViDev = {0, 1, 2, 3, 4},
        .aViPipe = {0, 1, 2, 3, 4},
        .aViChn = {0, 2, 4, 0, 0},
        .aenSnsType = {
            SONY_IMX290_MIPI_2M_30FPS_12BIT,        // visible light
            -1,                                     // ir raw
            ISP_VIN_IR_THERMAL_OFFLINE,             // ir thermal
            ISP_VIN1_IR_OFFLINE,                    // ir isp1
            ISP_VIN2_IR_OFFLINE,                    // ir isp2
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
            {640, 512},                         // ir raw
            {640, 512},                         // ir thermal
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
            {640, 512},                         // ir raw
            {640, 512},                         // ir thermal
            {640, 512},                         // ir isp1
            {640, 512},                         // ir isp2
        },
        .as32BusId = {0, 2, 0, 0, 0},
        .as32MipiDev = {0, 0, 0, 0, 0},

        .s32VisibleLightIdx = -1,
        .s32IrRawIdx = -1,
        .s32IrThrmlIdx = 2,
        .s32IrIsp1Idx = 3,
        .s32IrIsp2Idx = 4,
    },
    .u32IrSnsParamSize = 6,
    .stIrSnsRoi = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = 640,
        .u32Height = 512,
    },
    .stIrIspRoi = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = 640,
        .u32Height = 512,
    },
    .strInitSnsParamCalibFname = NULL,
    .strInitOccCalibFname = NULL,
    .strInitKbCalibFname = NULL,
    .bUseRawTempBuf = AR_FALSE,
    .bUseThrmlTempBuf = AR_TRUE,

    .bAutoCalib = AR_FALSE,
    .u32AutoCalibIntervalMs = 40000,

    .pfnRawProcess = NULL,
    .pfnCfgIrViTopology = OfflineCfgIrViTopology,
    .pfnIrCmdGetIrSnsParam = NULL,
};


static AR_S32 OfflineCfgIrViTopology(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    IR_VI_TOPOLOGY_TYPE_E enIrViTopoType, AR_U32 u32Width, AR_U32 u32Height)
{
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    switch (enIrViTopoType) {
    case IR_VI_TOPOLOGY_TYPE_0:     // ir_thermal --> ir_isp1 --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = -1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_OFFLINE;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_12BPP;
        break;

    case IR_VI_TOPOLOGY_TYPE_1:     // (ir_isp1 & ir_thermal) --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = -1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_OFFLINE_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        break;

    case IR_VI_TOPOLOGY_TYPE_2:     // ir_raw --> ir_thermal --> ir_isp1
        pstIrViGrpCfg->s32IrRawIdx = -1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_OFFLINE;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_12BPP;
        break;

    case IR_VI_TOPOLOGY_TYPE_3:     // ir_raw --> (ir_isp1 & ir_thermal)
        pstIrViGrpCfg->s32IrRawIdx = -1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_OFFLINE_WITH_THERMAL;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_14BPP;
        break;

    case IR_VI_TOPOLOGY_TYPE_4:     // ir_raw --> ir_thermal
        pstIrViGrpCfg->s32IrRawIdx = -1;
        pstIrViGrpCfg->s32IrThrmlIdx = 2;
        pstIrViGrpCfg->s32IrIsp1Idx = -1;
        pstIrViGrpCfg->s32IrIsp2Idx = -1;
        break;

    case IR_VI_TOPOLOGY_TYPE_6:     // ir_isp1 --> ir_isp2
        pstIrViGrpCfg->s32IrRawIdx = -1;
        pstIrViGrpCfg->s32IrThrmlIdx = -1;
        pstIrViGrpCfg->s32IrIsp1Idx = 3;
        pstIrViGrpCfg->s32IrIsp2Idx = 4;
        pstIrViGrpCfg->aenSnsType[pstIrViGrpCfg->s32IrIsp1Idx] = ISP_VIN1_IR_OFFLINE;
        pstIrViGrpCfg->aenInPixFmt[pstIrViGrpCfg->s32IrIsp1Idx] = PIXEL_FORMAT_RGB_BAYER_12BPP;
        break;

    default:
        ar_err("unsupported ir vi topology type[%d]", enIrViTopoType);
        return -1;
    }

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrThrmlIdx].u32Width = u32Width;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrThrmlIdx].u32Height = u32Height;
        pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrThrmlIdx] = pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrThrmlIdx];
    }

    if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width = u32Width;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height = u32Height;
        pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp1Idx] = pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx];
    }

    if (pstIrViGrpCfg->s32IrIsp2Idx >= 0) {
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp2Idx].u32Width = u32Width;
        pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp2Idx].u32Height = u32Height;
        pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp2Idx] = pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp2Idx];
    }

    pstIrSampleCtx->stIrSnsRoi.s32X = 0;
    pstIrSampleCtx->stIrSnsRoi.s32Y = 0;
    pstIrSampleCtx->stIrSnsRoi.u32Width = (u32Width == 408 ? 400 : u32Width);
    pstIrSampleCtx->stIrSnsRoi.u32Height = u32Height;

    pstIrSampleCtx->stIrIspRoi.s32X = 0;
    pstIrSampleCtx->stIrIspRoi.s32Y = 0;
    pstIrSampleCtx->stIrIspRoi.u32Width = (u32Width == 408 ? 400 : u32Width);
    pstIrSampleCtx->stIrIspRoi.u32Height = u32Height;

    return 0;
}

