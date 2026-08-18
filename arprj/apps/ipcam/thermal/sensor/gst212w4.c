#include <stdio.h>

#include "util_common.h"
#include "pf_thermal.h"
#include "ar_comm_sns.h"
#include "gst212w4/gst212w4.h"

#define GST212W4_SNS_PARAM_SIZE 7

AR_S32 Gst212wRawProcess(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData)
{
    AR_U32 u32CycleWidth = pstFrmInfo->stVFrame.u32Width * 2 / sizeof(AR_U64);
    AR_U32 u32LineEndOffset = pstFrmInfo->stVFrame.u32Stride[0] / sizeof(AR_U64) - u32CycleWidth;
    AR_U64 *p = (AR_U64 *)pstFrmInfo->stVFrame.u64VirAddr[0];
    AR_U32 i = 0;
    AR_U32 j = 0;

    for (i = 0; i < pstFrmInfo->stVFrame.u32Height; i++) 
    {
        for (j = 0; j < u32CycleWidth; j++) 
        {
            *p = 0x3fff3fff3fff3fffULL - *p;
            p++;
        }
        p += u32LineEndOffset;
    }

    return 0;
}

//do occ calib and save to new files
AR_S32 Gst212wSnsCalib(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData)
{
#if 0
    AR_S32 s32Ret = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)pPrivData;
    GST212W4_CALIB_INFO_S stCalibInfo;
    FILE *fp = NULL;
    size_t wsize = 0;
    AR_U32 u32BuffSize = 0;
    AR_U32 u32ReadSize = 0;
    AR_U64 u64OccBuffPhyAddr = 0;
    AR_VOID * pOccBuffVirtAddr = NULL;

    if (pstFrmInfo == NULL) 
    {
        PRINT_ERR("pstFrmInfo is null");
        return -1;
    }

    //get mem for sns param and occ param
    stCalibInfo.pIrSnsParam = malloc(GST212W4_SNS_PARAM_SIZE);
    if(!stCalibInfo.pIrSnsParam)
    {
        PRINT_ERR("Malloc sns param for gst417w failed.\n");
        return -1;
    }

    stCalibInfo.enCalibType = GST212W4_CALIB_TYPE_ALL;
    stCalibInfo.pstFrmInfo = pstFrmInfo;
    stCalibInfo.stROI = pstViWrapper->stRoi;
    stCalibInfo.u32CalibFrmInterval = 8;

    s32Ret = AR_MPI_ISP_SnsCalib(pstViWrapper->ViPipe, &stCalibInfo);
    if (s32Ret || stCalibInfo.enCalibResult == GST212W4_CALIB_FAIL) 
    {
        PRINT_ERR("gst212w4 sensor calibration failed");
        free(stCalibInfo.pIrSnsParam);
        s32Ret = -1;
        return s32Ret;
    }

    //Save to file
    if (stCalibInfo.enCalibResult == GST212W4_CALIB_DONE) 
    {
        if (pstViWrapper->strSnsParamCalibFname != NULL) 
        {
            fp = fopen(pstViWrapper->strSnsParamCalibFname, "wb");
            if (fp == NULL) 
            {
                PRINT_ERR("open file[%s] failed\n", pstViWrapper->strSnsParamCalibFname);
                free(stCalibInfo.pIrSnsParam);
                return -1;
            }

            wsize = fwrite(stCalibInfo.pIrSnsParam, 1, GST212W4_SNS_PARAM_SIZE, fp);
            if (wsize != GST212W4_SNS_PARAM_SIZE) 
            {
                PRINT_ERR("write sensor ir param to file[%s] failed", pstViWrapper->strSnsParamCalibFname);
                fclose(fp);
                free(stCalibInfo.pIrSnsParam);
                return -1;
            }

            fclose(fp);
            fp = NULL;
        }
    }
    free(stCalibInfo.pIrSnsParam);
#endif

    return 0;
}

VI_IR_GROUP_S Gst212wGrpCfg = 
{
     .stViSns = 
     {
         .enSnsType = GST212W4_IR_256X200,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
         .enOutPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
         .stInSize = {256, 200},
         .stOutSize = {256, 200},
         .stRoi = {0,4,256,192},
         .s32BusId = 2,
         .strInitSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_gst212w4/gst212w4_sns_calib.bin",
         .strInitOccCalibFname = NULL,
         .strInitKbCalibFname = NULL, 
         .strSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_gst212w4/gst212w4_sns_calib_new.bin",
         .strOccCalibFname = NULL,
         .u64TmpCachedBuffPhyAddr = 0,
         .pTmpCachedBuffVirtAddr = NULL,
         .pfnSnsHandler = Gst212wRawProcess,
         .pfnFrameProcess = IrRawFrameProcess,
         .pfnThreadFunc = IrFrameProcessThreadFunc,
         .pfnThreadDeqFunc = IrDeqThreadFunc,

         .stIrSnsCalibCfg = {
             .u32FrmInterval = 4,
             .u32TargetLowerLimit = 8192 - 1024,
             .u32TargetUpperLimit = 8192 + 1024,
         },
         .stIrOccCalibCfg = {
             .u32FrmInterval = 4,
         },
         .enFirstIrDynamicCalibType = IPC_IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B,
         .enIrDynamicCalibType = IPC_IR_DYNAMIC_CALIB_TYPE_OCC_B,
         .u32DynamicCalibIntervalMs = 40 * 1000,
         .pfnIrDynamicCalibFunc = IrDynamicCalibFunc,
     },
     .stViThermal = 
     {
         .enSnsType = ISP_VIN_IR_THERMAL_GST212W4,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
         .enOutPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
         .stInSize = {256, 200},
         .stOutSize = {256, 192},
         .stRoi = {0,0,256,192},
         .s32BusId = 0,
         .strInitSnsParamCalibFname = NULL,
         .strInitOccCalibFname = NULL,
         .strInitKbCalibFname = "/usrdata/local/factory/tunning/cam_gst212w4/gst212w4_kb_calib.bin", 
         .strSnsParamCalibFname = NULL,
         .strOccCalibFname = NULL,
         .u64TmpCachedBuffPhyAddr = 0,
         .pTmpCachedBuffVirtAddr = NULL,
         .pfnSnsHandler = NULL,
         .pfnFrameProcess = IrThermalFrameProcess,
         .pfnThreadFunc = IrFrameProcessThreadFunc,
         .pfnThreadDeqFunc = IrDeqThreadFunc,

         .stIrBCalibCfg = {
             .u32GrabFrmNum = 4,
             .bMeanAsTargetVal = AR_TRUE,
         },
     },
     .stViIsp0 = 
     {
         .enSnsType = ISP_VIN_IR_ISP0_GST212W4,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP,
         .enOutPixFmt = PIXEL_FORMAT_YVU_PLANAR_420,
         .stInSize = {256, 192},
         .stOutSize = {256, 192},
         .stRoi = {0,0,256,192},
         .s32BusId = 0,
         .strInitSnsParamCalibFname = NULL,
         .strInitOccCalibFname = NULL,
         .strInitKbCalibFname = NULL, 
         .strSnsParamCalibFname = NULL,
         .strOccCalibFname = NULL,
         .u64TmpCachedBuffPhyAddr = 0,
         .pTmpCachedBuffVirtAddr = NULL,
         .pfnSnsHandler = NULL,
         .pfnFrameProcess = IrIsp0FrameProcess,
         .pfnThreadFunc = IrFrameProcessThreadFunc,
         .pfnThreadDeqFunc = IrDeqThreadFunc,
     },
#if 0 //ir go through isp two times     
     .stViIsp1 = 
     {
         .enSnsType = ISP_VIN_IR_ISP1_GST212W4,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_8BPP,
         .enOutPixFmt = PIXEL_FORMAT_YVU_PLANAR_420,
         .stInSize = {256, 192},
         .stOutSize = {256, 192},
         .stRoi = {0,0,256,192},
         .s32BusId = 0,
         .strInitSnsParamCalibFname = NULL,
         .strInitOccCalibFname = NULL,
         .strInitKbCalibFname = NULL, 
         .strSnsParamCalibFname = NULL,
         .strOccCalibFname = NULL,
         .u64TmpCachedBuffPhyAddr = 0,
         .pTmpCachedBuffVirtAddr = NULL,
         .pfnSnsHandler = NULL,
         .pfnFrameProcess = NULL,
         .pfnThreadFunc = NULL,
         .pfnThreadDeqFunc = NULL,
     }
#else
    .stViIsp1 = 
    {
        .enSnsType = IPC_SNS_TYPE_BUTT,
        .enInPixFmt = 0,
        .enOutPixFmt = 0,
        .stInSize = {0, 0},
        .stOutSize = {0, 0},
        .stRoi = {0,0,0,0},
        .s32BusId = 0,
        .strInitSnsParamCalibFname = NULL,
        .strInitOccCalibFname = NULL,
        .strInitKbCalibFname = NULL, 
        .strSnsParamCalibFname = NULL,
        .strOccCalibFname = NULL,
        .u64TmpCachedBuffPhyAddr = 0,
        .pTmpCachedBuffVirtAddr = NULL,
        .pfnSnsHandler = NULL,
        .pfnFrameProcess = NULL,
        .pfnThreadFunc = NULL,
        .pfnThreadDeqFunc = NULL,
    }
#endif
 };

