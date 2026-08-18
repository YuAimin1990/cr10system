#include <stdio.h>
#include "util_common.h"
#include "pf_thermal.h"
#include "ar_comm_sns.h"
#include "mpi_sys.h"
#include "mpi_isp.h"
#include "h3812c1sg/h3812c1sg.h"

#define H3812C1SG_SNS_PARAM_SIZE 48

//do occ calib and save to new files
AR_S32 H3812c1sgSnsCalib(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData)
{
#if 0
    AR_S32 s32Ret = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)pPrivData;
    H3812C1SG_CALIB_INFO_S stCalibInfo;
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
    stCalibInfo.pIrSnsParam = malloc(H3812C1SG_SNS_PARAM_SIZE);
    if(!stCalibInfo.pIrSnsParam)
    {
        PRINT_ERR("Malloc sns param for H3812c1sg failed.\n");
        return -1;
    }

    stCalibInfo.pstIrOccParam = (ISP_IR_OCC_PARAM_S *)malloc(sizeof(ISP_IR_OCC_PARAM_S));
    if(!stCalibInfo.pstIrOccParam)
    {
        PRINT_ERR("Malloc OCC param for H3812c1sg failed.\n");
        goto FAIL1;
    }

    stCalibInfo.pstIrOccParam->stOccBuf.u32Width = pstViWrapper->stInSize.u32Width;
    stCalibInfo.pstIrOccParam->stOccBuf.u32Height = pstViWrapper->stInSize.u32Height;
    
    stCalibInfo.pstIrOccParam->stOccBuf.u32Stride[0] = CAM_ALIGNE_TO(pstViWrapper->stInSize.u32Width, 256);
    u32BuffSize = stCalibInfo.pstIrOccParam->stOccBuf.u32Stride[0] * pstViWrapper->stInSize.u32Height + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&u64OccBuffPhyAddr, &pOccBuffVirtAddr, "OccBuff", NULL, u32BuffSize);
    if (s32Ret) 
    {
        PRINT_ERR("Allocate occ buf failed\n");
        goto FAIL2;
    }

    stCalibInfo.pstIrOccParam->stOccBuf.u64PhyAddr[0] = u64OccBuffPhyAddr;
    stCalibInfo.pstIrOccParam->stOccBuf.u64VirAddr[0] = (AR_U64)pOccBuffVirtAddr;

    //LOAD INIT params
    fp = fopen(pstViWrapper->strInitSnsParamCalibFname, "rb");
    if (fp) 
    {
        u32ReadSize = fread((AR_VOID *)stCalibInfo.pIrSnsParam, 1, H3812C1SG_SNS_PARAM_SIZE, fp);
        if (u32ReadSize != H3812C1SG_SNS_PARAM_SIZE) 
        {
            PRINT_INFO("Read sns init param %s error, will use default.\n", pstViWrapper->strInitSnsParamCalibFname);
        }
        fclose(fp);
    }

    fp = fopen(pstViWrapper->strInitOccCalibFname, "rb");
    if (fp) 
    {
        u32ReadSize = fread(pOccBuffVirtAddr, 1, u32BuffSize, fp);
        if (u32ReadSize != u32BuffSize) 
        {
            PRINT_INFO("Read occ init param error, will use default.\n");
        }
        fclose(fp);
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(u64OccBuffPhyAddr, pOccBuffVirtAddr, u32BuffSize);
    if (s32Ret) 
    {
        PRINT_ERR("Flush occ data buffer failed\n");
        goto FAIL3;
    }
    
    stCalibInfo.pstFrmInfo = pstFrmInfo;
    stCalibInfo.stROI = pstViWrapper->stRoi;

    s32Ret = AR_MPI_ISP_SnsCalib(pstViWrapper->ViPipe, &stCalibInfo);
    if (s32Ret || stCalibInfo.enCalibResult == H3812C1SG_CALIB_FAIL) 
    {
        PRINT_ERR("H3812c1sg sensor calibration failed\n");
        s32Ret = -1;
        goto FAIL3;
    }

    //Save to file
    if (stCalibInfo.enCalibResult == H3812C1SG_CALIB_DONE) 
    {
        if (pstViWrapper->strSnsParamCalibFname != NULL) 
        {
            fp = fopen(pstViWrapper->strSnsParamCalibFname, "wb");
            if (fp == NULL) 
            {
                PRINT_ERR("open file[%s] failed\n", pstViWrapper->strSnsParamCalibFname);
                goto FAIL3;
            }

            wsize = fwrite(stCalibInfo.pIrSnsParam, 1, H3812C1SG_SNS_PARAM_SIZE, fp);
            if (wsize != H3812C1SG_SNS_PARAM_SIZE) 
            {
                PRINT_ERR("write sensor ir param to file[%s] failed", pstViWrapper->strSnsParamCalibFname);
                fclose(fp);
                goto FAIL3;
            }

            fclose(fp);
            fp = NULL;
        }

        if (pstViWrapper->strOccCalibFname != NULL) 
        {
            fp = fopen(pstViWrapper->strOccCalibFname, "wb");
            if (fp == NULL) 
            {
                PRINT_ERR("open file[%s] failed", pstViWrapper->strOccCalibFname);
                goto FAIL3;
            }

            wsize = fwrite((AR_VOID *)stCalibInfo.pstIrOccParam->stOccBuf.u64VirAddr[0], 1, u32BuffSize, fp);
            if (wsize != u32BuffSize) 
            {
                PRINT_ERR("write ooc data to file[%s] failed", pstViWrapper->strOccCalibFname);
                fclose(fp);
                goto FAIL3;
            }

            fclose(fp);
            fp = NULL;
        }
    } 

FAIL3:
    AR_MPI_SYS_MmzFree(u64OccBuffPhyAddr, pOccBuffVirtAddr);
FAIL2:
    free(stCalibInfo.pstIrOccParam);
FAIL1:
    free(stCalibInfo.pIrSnsParam);
#endif

    return 0;
}

VI_IR_GROUP_S H3812c1sgGrpCfg = 
{
     .stViSns = 
     {
         .enSnsType = H3812C1SG_IR_444X336,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
         .enOutPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
         .stInSize = {444, 336},
         .stOutSize = {444, 336},
         .stRoi = {16, 18, 384, 288},
         .s32BusId = 0,
         .strInitSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_h3812c1sg/h3812c1sg_sns_calib.bin",
         .strInitOccCalibFname = "/usrdata/local/factory/tunning/cam_h3812c1sg/h3812c1sg_occ_calib.bin",
         .strInitKbCalibFname = NULL, 
         .strSnsParamCalibFname = "/usrdata/local/factory/tunning/cam_h3812c1sg/h3812c1sg_sns_calib_new.bin",
         .strOccCalibFname = "/usrdata/local/factory/tunning/cam_h3812c1sg/h3812c1sg_sns_calib_new.bin",
         .u64TmpCachedBuffPhyAddr = 0,
         .pTmpCachedBuffVirtAddr = NULL,
         .pfnSnsHandler = NULL,
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
             .u32TargetLowerLimit = 8192 - 512,
             .u32TargetUpperLimit = 8192 + 512,
             .u32MaxIterateTimes = 8,
         },
         .enFirstIrDynamicCalibType = IPC_IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B,
         .enIrDynamicCalibType = IPC_IR_DYNAMIC_CALIB_TYPE_OCC_B,
         .u32DynamicCalibIntervalMs = 40 * 1000,
         .pfnIrDynamicCalibFunc = IrDynamicCalibFunc,
     },
     .stViThermal = 
     {
         .enSnsType = ISP_VIN_IR_THERMAL_H3812C1SG,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
         .enOutPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
         .stInSize = {444, 336},
         .stOutSize = {384, 288},
         .stRoi = {0, 0, 384, 288},
         .s32BusId = 0,
         .strInitSnsParamCalibFname = NULL,
         .strInitOccCalibFname = NULL,
         .strInitKbCalibFname = "/usrdata/local/factory/tunning/cam_h3812c1sg/h3812c1sg_kb_calib.bin", 
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
         .enSnsType = ISP_VIN_IR_ISP0_H3812C1SG,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP,
         .enOutPixFmt = PIXEL_FORMAT_YVU_PLANAR_420,
         .stInSize = {384, 288},
         .stOutSize = {384, 288},
         .stRoi = {0, 0, 384, 288},
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
         .enSnsType = ISP_VIN_IR_ISP1_H3812C1SG,
         .enInPixFmt = PIXEL_FORMAT_RGB_BAYER_8BPP,
         .enOutPixFmt = PIXEL_FORMAT_YVU_PLANAR_420,
         .stInSize = {384, 288},
         .stOutSize = {384, 288},
         .stRoi = {0, 0, 384, 288},
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
