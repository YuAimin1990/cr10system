//this is the common algorithm for IR process
//Customers may have their own process

#include "util_common.h"
#include "pf_thermal.h"
#include "hal_ge2d.h"
#include "cfg_vio.h"
#include "cfg_channel.h"
#include "cfg_vpss.h"
#include "cfg_vio.h"
#include "ar_common.h"
#include "hal_vin_log.h"
#ifndef THERMAL_TYPE
#include "ar_inf_alg.h"
#endif
#define IR_CALIB_INTERVAL_FRAMES 1000
#define IR_CALIB_WAIT_FRAMES_SHUTTER_SWITCH 8

static AR_S32 save_to_file(char * data, char * filename, int size)
{
    FILE * fp = NULL;
    AR_S32 s32Len = 0;
    AR_S32 s32Ret = 0;

    fp = fopen(filename, "wb");
    if (!fp)
    {
        PRINT_ERR("Open file %s error: %s\r\n", filename, strerror(errno));
        return NULL;
    }

    s32Ret = fwrite(data, size, 1, fp);
    if (s32Ret < 0)
    {
        PRINT_ERR("Write file %s error: %s\r\n", filename, s32Len, strerror(errno));
        fclose(fp);
        return NULL;
    }

    fclose(fp);
}

static AR_VOID * AllocAndLoadFile(AR_CHAR * achName, AR_S32 * pSize)
{
    FILE * pFile = 0;
    AR_S32 s32Len = 0;
    AR_S32 s32Ret = 0;
    AR_VOID * pBuff = 0;

    *pSize = 0;
    pFile = fopen(achName, "rb");
    if (!pFile)
    {
        PRINT_ERR("Open file %s error: %s\r\n", achName, strerror(errno));
        return NULL;
    }

    fseek(pFile, 0, SEEK_END);
    s32Len = ftell(pFile);
    rewind(pFile);

    pBuff = malloc(s32Len);
    if(!pBuff)
    {
        fclose(pFile);
        return NULL;
    }

    s32Ret = fread(pBuff, 1, s32Len, pFile);
    if (s32Ret != s32Len)
    {
        PRINT_ERR("Read file %s size %d error: %s\r\n", achName, s32Len, strerror(errno));
        fclose(pFile);
        free(pBuff);
        return NULL;
    }

    fclose(pFile);

    *pSize = s32Len;
    return pBuff;
}

#if 0
static AR_S32 DoBCalib(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData, ISP_IR_KB_PARAM_S * pstKbParams)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32RawLinePixs = 0;
    AR_U64 u64PixSum = 0;
    AR_U32 u32Mean = 0;
    AR_U16 *pRaw = NULL;
    AR_U32 u32KbLinePixs = 0;
    AR_U16 *pK = NULL;
    AR_U16 *pB = NULL;
    AR_S32 u32PixB = 0;
    AR_U32 i = 0;
    AR_U32 j = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)pPrivData;

    u32RawLinePixs = pstFrmInfo->stVFrame.u32Stride[0] / sizeof(AR_U16);
    u32KbLinePixs = pstKbParams->stKbBuf.u32Stride[0] / sizeof(AR_U16);

    u64PixSum = 0;
    pRaw = (AR_U16 *)pstFrmInfo->stVFrame.u64VirAddr[0];
    pRaw += (u32RawLinePixs * pstViWrapper->stRoi.s32Y + pstViWrapper->stRoi.s32X);

    for (i = 0; i < pstViWrapper->stRoi.u32Height; i++)
    {
        for (j = 0; j < pstViWrapper->stRoi.u32Width; j++)
        {
            u64PixSum += pRaw[j];
        }
        pRaw += u32RawLinePixs;
    }

    u32Mean = (AR_U32)(u64PixSum / (pstViWrapper->stRoi.u32Width * pstViWrapper->stRoi.u32Height));

    pRaw = (AR_U16 *)pstFrmInfo->stVFrame.u64VirAddr[0];
    pRaw += (u32RawLinePixs * pstViWrapper->stRoi.s32Y + pstViWrapper->stRoi.s32X);
    pK = (AR_U16 *)pstKbParams->stKbBuf.u64VirAddr[0];
    pB = (AR_U16 *)pstKbParams->stKbBuf.u64VirAddr[1];

    for (i = 0; i < pstViWrapper->pstNextVi->stRoi.u32Height; i++)
    {
        for (j = 0; j < pstViWrapper->pstNextVi->stRoi.u32Width; j++)
        {
            if ((pK[j] & 0x8000) == 0)
            {
                u32PixB = (AR_S32)pRaw[j] - ((AR_S32)u32Mean - 8192) * 8192 / (AR_S32)pK[j];

                if (u32PixB > 32767)
                    u32PixB = 32767;
                else if (u32PixB < -32768)
                    u32PixB = -32768;

                pB[j] = (AR_U16)u32PixB;
            }
        }

        pK += u32KbLinePixs;
        pB += u32KbLinePixs;
        pRaw += u32RawLinePixs;
    }

    //ar_always("mean[%u] k00[%u] k01[%u] k40[%u] k41[%u] b00[%u] b01[%u] b40[%u] b41[%u]",
    //u32Mean, pK[0], pK[1], pK[u32KbLinePixs*4], pK[u32KbLinePixs*4+1],
    //pB[0], pB[1], pB[u32KbLinePixs*4], pB[u32KbLinePixs*4+1]);


    return 0;
}

static AR_S32 DoKbCalibAndApply(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData)
{
    VI_WRAPPER_S * pstViSns = (VI_WRAPPER_S *)pPrivData;
    ISP_IR_PARAMS_S stIrParams = {0};
    ISP_IR_KB_PARAM_S stKbParams = {0};
    AR_S32 s32Size = 0;
    AR_S32 s32Ret = 0;
    AR_S32 i = 0;

    stKbParams.stKbBuf.u32Width = pstViSns->pstNextVi->stOutSize.u32Width;
    stKbParams.stKbBuf.u32Height = pstViSns->pstNextVi->stOutSize.u32Height;
    stKbParams.stKbBuf.u32Stride[0] = CAM_ALIGNE_TO(stKbParams.stKbBuf.u32Width * 2, IR_KB_BUF_ALIGN);
    stKbParams.stKbBuf.u32Stride[1] = stKbParams.stKbBuf.u32Stride[0];
    s32Size = stKbParams.stKbBuf.u32Stride[0] * stKbParams.stKbBuf.u32Height * 2 + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&stKbParams.stKbBuf.u64PhyAddr[0],
            &stKbParams.stKbBuf.u64VirAddr[0], NULL, NULL, s32Size);
    if (s32Ret)
    {
        PRINT_ERR("Allocate kb buf failed\n");
        return -1;
    }

    stKbParams.stKbBuf.u64PhyAddr[1] = stKbParams.stKbBuf.u64PhyAddr[0] + stKbParams.stKbBuf.u32Stride[0] * stKbParams.stKbBuf.u32Height;
    stKbParams.stKbBuf.u64VirAddr[1] = stKbParams.stKbBuf.u64VirAddr[0] + stKbParams.stKbBuf.u32Stride[0] * stKbParams.stKbBuf.u32Height;

    stKbParams.bKbcEn = AR_TRUE;
    stKbParams.bDpcEn = AR_TRUE;

    // Set default value to KB buf
    AR_U16 * pKb = (AR_U16 *)stKbParams.stKbBuf.u64VirAddr[0];
    for (i = 0; i < s32Size / sizeof(AR_U16); i++)
    {
        *pKb++ = 8192;
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(stKbParams.stKbBuf.u64PhyAddr[0],
    stKbParams.stKbBuf.u64VirAddr[0], s32Size);
    if (s32Ret)
    {
        PRINT_ERR("Flush kb data buffer failed\n");
    }
    DoBCalib(pstFrmInfo, pPrivData, &stKbParams);
    s32Ret = AR_MPI_SYS_MmzFlushCache(stKbParams.stKbBuf.u64PhyAddr[0],
    stKbParams.stKbBuf.u64VirAddr[0], s32Size);
    if (s32Ret)
    {
        PRINT_ERR("Flush kb data buffer failed\n");
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_KB;
    stIrParams.astParamEntry[0].pParamData = &stKbParams;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(stKbParams);
    AR_MPI_ISP_SetIrParams(pstViSns->pstNextVi->ViPipe, &stIrParams);

    AR_MPI_SYS_MmzFree(stKbParams.stKbBuf.u64PhyAddr[0], stKbParams.stKbBuf.u64VirAddr[0]);

    return 0;
}

/* The process is:
 * 0. do the calib at the first beginning and apply params to isp.
 * 1. do calib every 40s during runtime.
 * 2. for each calib, steps are:
 *    2.1. Close shutter
 *    2.2. wait for 8 frames to let it stable.
 *    2.3. calib
 *    2.4. apply the calib params to isp.
 *    2.5. wait for 8 frames to let it stable.
 *    2.6. open shutter
 */
AR_S32 IrKbCalib(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData)
{
    static AR_S32 s32FrameCnt = -1;
    AR_S32 s32Ret = 0;
    static AR_BOOL bCalibFirst = AR_FALSE;

    VI_WRAPPER_S * pstViSns = (VI_WRAPPER_S *)pPrivData;

    //do calib at the beginning
    if(!bCalibFirst)
    {
        IrCloseShutter(pstViSns->ViPipe);
        DoKbCalibAndApply(pstFrmInfo, pPrivData);
        IrOpenShutter(pstViSns->ViPipe);
        bCalibFirst = AR_TRUE;
    }

    s32FrameCnt++;
    if(s32FrameCnt % IR_CALIB_INTERVAL_FRAMES == 0)
    {
        IrCloseShutter(pstViSns->ViPipe);
    }

    if(s32FrameCnt % IR_CALIB_INTERVAL_FRAMES == IR_CALIB_WAIT_FRAMES_SHUTTER_SWITCH)
    {
        DoKbCalibAndApply(pstFrmInfo, pPrivData);
    }

    if(s32FrameCnt % IR_CALIB_INTERVAL_FRAMES == IR_CALIB_WAIT_FRAMES_SHUTTER_SWITCH * 2)
    {
        IrOpenShutter(pstViSns->ViPipe);
    }

    return 0;
}
#endif

AR_S32 IrOpenShutter(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;

    s32Ret = AR_MPI_ISP_SnsSwitchShutter(ViPipe, AR_TRUE);
    if (s32Ret)
    {
        PRINT_ERR("Pipe %d open shutter failed\n", ViPipe);
    }

    //PRINT_INFO("IR shutter opened\n");
    return s32Ret;
}

AR_S32 IrCloseShutter(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;

    s32Ret = AR_MPI_ISP_SnsSwitchShutter(ViPipe, AR_FALSE);
    if (s32Ret)
    {
        PRINT_ERR("Pipe %d close shutter failed\n", ViPipe);
    }

    //PRINT_INFO("IR shutter closed\n");
    return s32Ret;
}

//set sns/occ/kb/ltm/gtm by default.
AR_S32 IrLoadAndApplyAllParam(VI_IR_GROUP_S * pstViIRGrp)
{
    AR_S32 s32Ret = 0;
    AR_S32 i = 0, cnt = 0;
    AR_S32 s32Size = 0;
    ISP_IR_PARAMS_S stIrParams = {0};
    ISP_IR_GTM_PARAM_S stGtmParam = {0};
    ISP_IR_KB_PARAM_S stKbParam = {0};
    ISP_IR_OCC_PARAM_S stOccParam = {0};
    AR_VOID * pBuff = 0;
    AR_S32 s32FileLen = 0;

    //Sns params
    stIrParams.astParamEntry[cnt].pParamData = AllocAndLoadFile(pstViIRGrp->stViSns.strInitSnsParamCalibFname, &s32Size);
    if(stIrParams.astParamEntry[cnt].pParamData)
    {
        stIrParams.astParamEntry[cnt].enParamType = IR_PARAM_TYPE_SENSOR;
        stIrParams.astParamEntry[cnt].u32ParamSize = s32Size;
        cnt++;
    }

    //occ param
    stOccParam.bIsSingleVal = AR_FALSE;
    stOccParam.stOccBuf.u32Width = pstViIRGrp->stViSns.stInSize.u32Width;
    stOccParam.stOccBuf.u32Height = pstViIRGrp->stViSns.stInSize.u32Height;
    stOccParam.stOccBuf.u32Stride[0] = CAM_ALIGNE_TO(pstViIRGrp->stViSns.stInSize.u32Width, IR_OCC_BUF_ALIGN);
    s32Size = stOccParam.stOccBuf.u32Stride[0] * stOccParam.stOccBuf.u32Height + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&stOccParam.stOccBuf.u64PhyAddr[0],
        &stOccParam.stOccBuf.u64VirAddr[0], NULL, NULL, s32Size);
    if (s32Ret)
    {
        PRINT_ERR("Allocate occ buf failed\n");
        return -1;
    }

    s32FileLen = 0;
    pBuff = AllocAndLoadFile(pstViIRGrp->stViSns.strInitOccCalibFname, &s32FileLen);
    if(pBuff)
    {
        memcpy((AR_VOID *)stOccParam.stOccBuf.u64VirAddr[0], pBuff, s32FileLen);
        s32Ret = AR_MPI_SYS_MmzFlushCache(stOccParam.stOccBuf.u64PhyAddr[0],
                  stOccParam.stOccBuf.u64VirAddr[0], s32Size);
        if (s32Ret)
        {
            PRINT_ERR("Flush occ data buffer failed\n");
            free(pBuff);
            AR_MPI_SYS_MmzFree(stOccParam.stOccBuf.u64PhyAddr[0],
                  (AR_VOID *)stOccParam.stOccBuf.u64VirAddr[0]);
            return -1;
        }

        stIrParams.astParamEntry[cnt].pParamData = &stOccParam;
        stIrParams.astParamEntry[cnt].enParamType = IR_PARAM_TYPE_OCC;
        stIrParams.astParamEntry[cnt].u32ParamSize = s32Size;
        cnt++;
    }

    if(cnt > 0)
    {
        stIrParams.u32ParamCnt = cnt;
        s32Ret = AR_MPI_ISP_SetIrParams(pstViIRGrp->stViSns.ViPipe, &stIrParams);
        if (s32Ret)
        {
            PRINT_ERR("Set ir sns params failed\n");
        }

        //for debug
        //memset((AR_VOID *)stOccParam.stOccBuf.u64VirAddr[0], 0x5a, s32FileLen);
        //AR_MPI_ISP_GetIrParams(pstViIRGrp->stViSns.ViPipe, &stIrParams);
        //save_to_file(stIrParams.astParamEntry[0].pParamData, "/tmp/sns.param", 6);
        //save_to_file(stOccParam.stOccBuf.u64VirAddr[0], "/tmp/occ.param", s32FileLen);
        for(i = 0; i < cnt; i++)
        {
            if(stIrParams.astParamEntry[i].enParamType == IR_PARAM_TYPE_SENSOR)
            {
                free(stIrParams.astParamEntry[i].pParamData);
            }
            else
            {
                free(pBuff);
                AR_MPI_SYS_MmzFree(stOccParam.stOccBuf.u64PhyAddr[0],
                  (AR_VOID *)stOccParam.stOccBuf.u64VirAddr[0]);
            }
        }
    }

    //kb and gtm params
    cnt = 0;
    memset(&stIrParams, 0, sizeof(stIrParams));
    stKbParam.stKbBuf.u32Width = pstViIRGrp->stViThermal.stOutSize.u32Width;
    stKbParam.stKbBuf.u32Height = pstViIRGrp->stViThermal.stOutSize.u32Height;
    stKbParam.stKbBuf.u32Stride[0] = CAM_ALIGNE_TO(stKbParam.stKbBuf.u32Width * 2, IR_KB_BUF_ALIGN);
    stKbParam.stKbBuf.u32Stride[1] = stKbParam.stKbBuf.u32Stride[0];
    s32Size = stKbParam.stKbBuf.u32Stride[0] * stKbParam.stKbBuf.u32Height * 2 + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&stKbParam.stKbBuf.u64PhyAddr[0],
        &stKbParam.stKbBuf.u64VirAddr[0], NULL, NULL, s32Size);
    if (s32Ret)
    {
        PRINT_ERR("Allocate kb buf failed\n");
        return -1;
    }

    stKbParam.stKbBuf.u64PhyAddr[1] = stKbParam.stKbBuf.u64PhyAddr[0] + stKbParam.stKbBuf.u32Stride[0] * stKbParam.stKbBuf.u32Height;
    stKbParam.stKbBuf.u64VirAddr[1] = stKbParam.stKbBuf.u64VirAddr[0] + stKbParam.stKbBuf.u32Stride[0] * stKbParam.stKbBuf.u32Height;

    stKbParam.bKbcEn = AR_TRUE;
    stKbParam.bDpcEn = AR_TRUE;

    s32FileLen = 0;
    pBuff = AllocAndLoadFile(pstViIRGrp->stViThermal.strInitKbCalibFname, &s32FileLen);
    if(pBuff)
    {
        memcpy((AR_VOID *)stKbParam.stKbBuf.u64VirAddr[0], pBuff, s32FileLen);
        s32Ret = AR_MPI_SYS_MmzFlushCache(stKbParam.stKbBuf.u64PhyAddr[0],
                  stKbParam.stKbBuf.u64VirAddr[0], s32Size);
        if (s32Ret)
        {
            PRINT_ERR("Flush kb data buffer failed\n");
            free(pBuff);
            AR_MPI_SYS_MmzFree(stKbParam.stKbBuf.u64PhyAddr[0],
                (AR_VOID *)stKbParam.stKbBuf.u64VirAddr[0]);
            return -1;
        }

        stIrParams.astParamEntry[cnt].pParamData = &stKbParam;
        stIrParams.astParamEntry[cnt].enParamType = IR_PARAM_TYPE_KB;
        stIrParams.astParamEntry[cnt].u32ParamSize = s32Size;
        cnt++;
    }

    stGtmParam.u32GtmContrast = 64;
    stGtmParam.s32GtmBright = 0;

    stIrParams.astParamEntry[cnt].enParamType = IR_PARAM_TYPE_GTM;
    stIrParams.astParamEntry[cnt].pParamData = &stGtmParam;
    stIrParams.astParamEntry[cnt].u32ParamSize = sizeof(stGtmParam);
    cnt++;

    stIrParams.u32ParamCnt = cnt;

    s32Ret = AR_MPI_ISP_SetIrParams(pstViIRGrp->stViThermal.ViPipe, &stIrParams);
    if (s32Ret)
    {
        PRINT_ERR("Set ir thermal params failed: 0x%x\n", s32Ret);
    }

    //for debug
    //memset((AR_VOID *)stKbParam.stKbBuf.u64VirAddr[0], 0x5a, s32FileLen);
    //AR_MPI_ISP_GetIrParams(pstViIRGrp->stViThermal.ViPipe, &stIrParams);
    //save_to_file(stKbParam.stKbBuf.u64VirAddr[0], "/tmp/kb.param", s32FileLen);

    for(i = 0; i < cnt; i++)
    {
        if(stIrParams.astParamEntry[i].enParamType == IR_PARAM_TYPE_KB)
        {
            free(pBuff);
            AR_MPI_SYS_MmzFree(stKbParam.stKbBuf.u64PhyAddr[0],
                (AR_VOID *)stKbParam.stKbBuf.u64VirAddr[0]);
        }
    }

    return s32Ret;
}

AR_S32 IrRawFrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    AR_S32 s32Ret = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)pPrivData;
    AR_BOOL bUseRawTempBuf = AR_FALSE;
    AR_U32 u32CycleWidth = pstFrameInfo->stVFrame.u32Width * 2 / sizeof(AR_U64);
    AR_U32 u32LineEndOffset = pstFrameInfo->stVFrame.u32Stride[0] / sizeof(AR_U64) - u32CycleWidth;
    AR_U64 *p = (AR_U64 *)pstFrameInfo->stVFrame.u64VirAddr[0];
    AR_BOOL bSnsCalibDone = AR_FALSE;
    AR_U32 i = 0;
    AR_U32 j = 0;
    AR_U32 u32Size = 0;
    VIDEO_FRAME_INFO_S stFrame = {0};

#if 1 //use raw buffer
    if (pstViWrapper->pfnSnsHandler)
    {
        pstViWrapper->pfnSnsHandler(pstFrameInfo, pstViWrapper);
    }

#else //use a cacheable buffer then cp back to raw buffer
    u32Size = CAM_ALIGNE_TO(pstViWrapper->stOutSize.u32Width * 2, 256) * pstViWrapper->stOutSize.u32Height + 256;
    if (!pstViWrapper->u64TmpCachedBuffPhyAddr && !pstViWrapper->pTmpCachedBuffVirtAddr)
    {
        s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstViWrapper->u64TmpCachedBuffPhyAddr, &pstViWrapper->pTmpCachedBuffVirtAddr, "RawBuf", NULL, u32Size);
        if (s32Ret)
        {
            PRINT_ERR("Allocate raw temp buf failed.\n");
            pstViWrapper->u64TmpCachedBuffPhyAddr = 0;
            pstViWrapper->pTmpCachedBuffVirtAddr = NULL;
            return -1;
        }
    }

    memcpy(&stFrame, pstFrameInfo, sizeof(stFrame));

    //PRINT_INFO("Frame stride %d, height %d, addr %x, u32Size %d\n",pstFrameInfo->stVFrame.u32Stride[0], pstFrameInfo->stVFrame.u32Height, pstFrameInfo->stVFrame.u64VirAddr[0], u32Size);
    s32Ret = ar_hal_sys_memcpy_pa(pstViWrapper->u64TmpCachedBuffPhyAddr, pstFrameInfo->stVFrame.u64PhyAddr[0],
        pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height);
    if (s32Ret)
    {
        PRINT_ERR("Dma copy raw to cached buf failed.\n");
        s32Ret = 0;
        //use the origin noncached buffer directly
    }
    else
    {
        stFrame.stVFrame.u64PhyAddr[0] = pstViWrapper->u64TmpCachedBuffPhyAddr;
        stFrame.stVFrame.u64VirAddr[0] = (AR_U64)pstViWrapper->pTmpCachedBuffVirtAddr;
    }

    if (pstViWrapper->pfnSnsHandler)
    {
        pstViWrapper->pfnSnsHandler(&stFrame, pstViWrapper);
    }

    //PRINT_INFO("Raw process...\n");
    //if we used cached buffer, don't forget to flush it before send to next vi
    if (stFrame.stVFrame.u64PhyAddr[0] == pstViWrapper->u64TmpCachedBuffPhyAddr)
    {
        s32Ret = AR_MPI_SYS_MmzFlushCache(pstViWrapper->u64TmpCachedBuffPhyAddr,
            pstViWrapper->pTmpCachedBuffVirtAddr, u32Size);
        if (s32Ret)
        {
            PRINT_ERR("Flush raw temp buffer failed.\n");
            goto EXIT;
        }

        s32Ret = ar_hal_sys_memcpy_pa(pstFrameInfo->stVFrame.u64PhyAddr[0],
            stFrame.stVFrame.u64PhyAddr[0],
            pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height);
        if (s32Ret)
        {
            PRINT_ERR("Dma copy back to raw buffer failed.\n");
            goto EXIT;
        }
    }
#endif

    return 0;

EXIT:
    AR_MPI_SYS_MmzFree(pstViWrapper->u64TmpCachedBuffPhyAddr, pstViWrapper->pTmpCachedBuffVirtAddr);
    return 0;
}

AR_S32 IrThermalFrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    AR_S32 s32Ret = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)pPrivData;
    AR_S32 s32CurGain = 0;
    AR_S32 s32SetGain = 0;
    VI_PIPE thrmlPipe = pstViWrapper->ViPipe;
    IR_ALGO_INPUT_S stIrAlgoInput;
    IR_ALGO_OUTPUT_S stIrAlgoOutput;

    const AR_FLOAT f32GainAlpha = 0.01f;
    static AR_S32 s32LastGain = 0;

    static AR_U32 u32IrAlgoProcFrmCnt = 0;
    static STRU_AEC_OUT_T stIrThrmlAecOut;
    static STRU_AEC_OUT_T stIrIsp0AecOut;
    static STRU_AEC_OUT_T stIrIsp1AecOut;
    static AR_BOOL bThrmalAecUpdateGet = AR_FALSE;

    //PRINT_INFO("Thermal process pipe %d ...\n", pstViWrapper->ViPipe);

    if (!bThrmalAecUpdateGet)
    {
        s32Ret |= AR_MPI_ISP_GetAecUpdate(pstViWrapper->ViPipe, &stIrThrmlAecOut);
        if (s32Ret)
        {
            PRINT_ERR("pipe[%d] get aec update failed\n", pstViWrapper->ViPipe);
        }

        if (pstViWrapper->pstNextVi)
        {
            s32Ret |= AR_MPI_ISP_GetAecUpdate(pstViWrapper->pstNextVi->ViPipe, &stIrIsp0AecOut);
            if (s32Ret)
            {
                PRINT_ERR("pipe[%d] get aec update failed\n", pstViWrapper->pstNextVi->ViPipe);
            }
        }

        if (pstViWrapper->pstNextVi->pstNextVi)
        {
            s32Ret |= AR_MPI_ISP_GetAecUpdate(pstViWrapper->pstNextVi->pstNextVi->ViPipe, &stIrIsp1AecOut);
            if (s32Ret)
            {
                PRINT_ERR("pipe[%d] get aec update failed\n", pstViWrapper->pstNextVi->pstNextVi->ViPipe);
            }
        }

        if (s32Ret == 0)
        {
            bThrmalAecUpdateGet = AR_TRUE;
        }
        else
        {
            s32Ret = 0;
        }
    }

    memset(&stIrAlgoInput, 0, sizeof(IR_ALGO_INPUT_S));
    stIrAlgoInput.stInputFrm = pstFrameInfo->stVFrame;
    stIrAlgoInput.stInputFrm.u32Width = pstViWrapper->stRoi.u32Width;
    //stIrAlgoInput.stInputFrm.u32Width = pstFrameInfo->stVFrame.u32Width;
    stIrAlgoInput.b3DnrFirstFrame = (u32IrAlgoProcFrmCnt == 0 ? AR_TRUE : AR_FALSE);
    stIrAlgoInput.bUseHwGtm = AR_FALSE;     // if ir_thermal is a single pipe, only sw_gtm can be used instead of hw_gtm

    memset(&stIrAlgoOutput, 0, sizeof(stIrAlgoOutput));
    // if ir_thermal is a single pipe, use sw_gtm and there is gtm output frame
    // TODO: in this case, 2dnr output frame is also needed
    stIrAlgoOutput.stGtmOutput.stOutputFrm = stIrAlgoInput.stInputFrm;

    s32Ret = AR_MPI_ISP_IrAlgoProcess(thrmlPipe, &stIrAlgoInput, &stIrAlgoOutput);
    if (s32Ret)
    {
        PRINT_ERR("Thermal image ir algo process failed\n");
        return s32Ret;
    }

    u32IrAlgoProcFrmCnt++;

    s32CurGain = (AR_S32)stIrAlgoOutput.stGtmOutput.u32HistUpperPos
        - (AR_S32)stIrAlgoOutput.stGtmOutput.u32HistLowerPos;

    if (u32IrAlgoProcFrmCnt > 1) {
        s32SetGain = (AR_S32)(s32CurGain * f32GainAlpha + s32LastGain * (1 - f32GainAlpha));
        if (s32SetGain == s32LastGain) {
            if (s32SetGain < s32CurGain) {
                s32SetGain++;
            } else if (s32SetGain > s32CurGain) {
                s32SetGain--;
            }
        }
    } else {
        s32SetGain = s32CurGain;
    }

    s32LastGain = s32SetGain;

    stIrThrmlAecOut.real_gain = s32SetGain;
    s32Ret = AR_MPI_ISP_AecUpdate(pstViWrapper->ViPipe, &stIrThrmlAecOut);
    if (s32Ret)
    {
        PRINT_ERR("pipe[%d] aec update failed\n", pstViWrapper->ViPipe);
        s32Ret = 0;
    }

    if (pstViWrapper->pstNextVi)
    {
        stIrIsp0AecOut.real_gain = s32SetGain;
        s32Ret = AR_MPI_ISP_AecUpdate(pstViWrapper->pstNextVi->ViPipe, &stIrIsp0AecOut);
        if (s32Ret)
        {
            PRINT_ERR("Pipe[%d] aec update failed\n", pstViWrapper->pstNextVi->ViPipe);
            s32Ret = 0;
        }
    }

    if (pstViWrapper->pstNextVi->pstNextVi)
    {
        stIrIsp1AecOut.real_gain = s32SetGain;
        s32Ret = AR_MPI_ISP_AecUpdate(pstViWrapper->pstNextVi->pstNextVi->ViPipe, &stIrIsp1AecOut);
        if (s32Ret)
        {
            PRINT_ERR("pipe[%d] aec update failed\n", pstViWrapper->pstNextVi->pstNextVi->ViPipe);
            s32Ret = 0;
        }
    }

    return s32Ret;
}

AR_S32 IrIsp0FrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)pPrivData;

    return 0;
}

//If we bind vi to other modules, then we don't need app to process it.
//Or app can register this handler to do some work
AR_S32 IrIsp1FrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    return 0;
}

AR_VOID *IrFrameProcessThreadFunc(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    VI_PIPE ViPipe = 0;
    VI_CHN ViChn = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)arg;
    VIDEO_FRAME_INFO_S stFrameInfo;
    VIDEO_FRAME_INFO_S stEnqFrameInfo;
    AR_CHAR achName[16] = {0};

    ViPipe = pstViWrapper->ViPipe;
    ViChn = pstViWrapper->ViChn;
    sprintf(achName, "Ir%d%d", ViPipe, ViChn);
    prctl(PR_SET_NAME, achName, 0,0,0);

    AR_S32 policy = 0;
    struct sched_param sp = {0};
    //pthread_getschedparam(pthread_self(), &policy, &sp);
    policy = SCHED_RR;
    sp.sched_priority = sched_get_priority_min(SCHED_RR);
    pthread_setschedparam(pthread_self(), policy, &sp);

    if (!pstViWrapper->pstNextVi)
    {
        PRINT_INFO("No next vi, no need to process.\n");
        return NULL;
    }

    while (pstViWrapper->bIrfnThreadStart)
    {
        s32Ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &stFrameInfo, 1000);
        if (s32Ret)
        {
            //PRINT_ERR("pipe[%d] chn[%d]: get ir raw output frame failed", pstViWrapper->ViPipe, pstViWrapper->ViChn);
            continue;
        }

        //PRINT_INFO("Get frame from Pipe %d Chn %d...\n", ViPipe, ViChn);

        if (pstViWrapper->pfnFrameProcess != NULL)
        {
            s32Ret = pstViWrapper->pfnFrameProcess(&stFrameInfo, pstViWrapper);
            if (s32Ret)
            {
                PRINT_ERR("Process frame error!\n");
                AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &stFrameInfo);
                continue;
            }
        }

        //send to next vi
        if (pstViWrapper->pstNextVi)
        {
            stEnqFrameInfo = stFrameInfo;
            stEnqFrameInfo.stVFrame.u32Width = pstViWrapper->pstNextVi->stInSize.u32Width;
            stEnqFrameInfo.stVFrame.u32Height = pstViWrapper->pstNextVi->stInSize.u32Height;
            stEnqFrameInfo.stVFrame.enPixelFormat = pstViWrapper->pstNextVi->enInPixFmt;
            //PRINT_INFO("Send frame to pipe %d \n", pstViWrapper->pstNextVi->ViPipe);
            //s32Ret = AR_MPI_VI_SendPipeRaw(pstViWrapper->pstNextVi->ViPipe, &stEnqFrameInfo, 1000); //block way
            s32Ret = AR_MPI_VI_QPipeRaw(pstViWrapper->pstNextVi->ViPipe, &stEnqFrameInfo); //non-block way
            if (s32Ret)
            {
                PRINT_ERR("Send to pipe %d failed: 0x%x.\n",pstViWrapper->pstNextVi->ViPipe, s32Ret);
                AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &stFrameInfo);
            }
            //PRINT_INFO("Send frame to pipe %d done\n", pstViWrapper->pstNextVi->ViPipe);
        }

        //AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &stFrameInfo); //for block way only
    }

    return NULL;
}

AR_VOID * IrDeqThreadFunc(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    VI_PIPE ViPipe = 0;
    VI_CHN ViChn = 0;
    VI_WRAPPER_S * pstViWrapper = (VI_WRAPPER_S *)arg;
    VIDEO_FRAME_INFO_S stFrameInfo;
    AR_CHAR achName[16] = {0};

    ViPipe = pstViWrapper->ViPipe;
    ViChn = pstViWrapper->ViChn;
    sprintf(achName, "IrDq%d%d", ViPipe, ViChn);
    prctl(PR_SET_NAME, achName, 0,0,0);
    AR_S32 policy = 0;
    struct sched_param sp = {0};
    //pthread_getschedparam(pthread_self(), &policy, &sp);
    policy = SCHED_RR;
    sp.sched_priority = sched_get_priority_min(SCHED_RR);
    pthread_setschedparam(pthread_self(), policy, &sp);

    while(pstViWrapper->bIrDeThreadStart)
    {
        s32Ret = AR_MPI_VI_DqPipeRaw(pstViWrapper->pstNextVi->ViPipe, &stFrameInfo, 1000);
        if (s32Ret)
        {
            //PRINT_ERR("Pipe[%d]: dequeue frame failed\n", pstViWrapper->pstNextVi->ViPipe);
            continue;
        }

        AR_MPI_VI_ReleaseChnFrame(pstViWrapper->ViPipe, pstViWrapper->ViChn, &stFrameInfo);
    }
}


static int get_time_interval_us(struct timeval t1, struct timeval t2)
{
    return ((long long)t2.tv_sec - (long long)t1.tv_sec) * 1000000 + ((long long)t2.tv_usec - (long long)t1.tv_usec);
}

static AR_S32 IrDynamicCalib(VI_WRAPPER_S *pstViSnsWrapper, IPC_IR_DYNAMIC_CALIB_TYPE_E enCalibType)
{
    AR_S32 s32Ret = 0;
    VI_PIPE rawPipe = -1;
    VI_PIPE thrmlPipe = -1;
    IR_SNS_CALIB_RESULT_S stSnsCalibResult;
    IR_OCC_CALIB_RESULT_S stOccCalibResult;
    IR_B_CALIB_RESULT_S stBCalibResult;
    //struct timeval stStartTime;
    //struct timeval stEndTime;

    if (pstViSnsWrapper == NULL) {
        PRINT_ERR("pstViSnsWrapper is null\n");
        return -1;
    }

    memset(&stSnsCalibResult, 0, sizeof(stSnsCalibResult));
    memset(&stOccCalibResult, 0, sizeof(stOccCalibResult));
    memset(&stBCalibResult, 0, sizeof(stBCalibResult));

    rawPipe = pstViSnsWrapper->ViPipe;
    if (pstViSnsWrapper->pstNextVi) {
        thrmlPipe = pstViSnsWrapper->pstNextVi->ViPipe;
    }

    if (rawPipe < 0) {
        PRINT_ERR("ir raw pipe is invalid, ir dynamic calib is not supported\n");
        return -1;
    }

    //gettimeofday(&stStartTime, NULL);

    s32Ret = IrCloseShutter(rawPipe);
    if (s32Ret) {
        PRINT_ERR("pipe[%d] close shutter failed\n", rawPipe);
        goto EXIT;
    }

    if (enCalibType >= IPC_IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B) {
        usleep(80 * 1000);

        s32Ret = AR_MPI_ISP_StartIrSnsCalib(rawPipe, &pstViSnsWrapper->stIrSnsCalibCfg);
        if (s32Ret) {
            PRINT_ERR("pipe[%d] start ir sensor calib failed\n", rawPipe);
            goto EXIT;
        }

        while (1) {
            s32Ret = AR_MPI_ISP_GetIrSnsCalibResult(rawPipe, &stSnsCalibResult);
            if (s32Ret) {
                PRINT_ERR("pipe[%d] get ir sensor calib result failed\n", rawPipe);
                AR_MPI_ISP_CancelIrSnsCalib(rawPipe);
                goto EXIT;
            } else {
                if (stSnsCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                    break;
                } else if (stSnsCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                    PRINT_ERR("pipe[%d] ir sensor calib cancelled\n", rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                } else if (stSnsCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                    PRINT_ERR("pipe[%d] ir sensor calib failed\n", rawPipe);
                    AR_MPI_ISP_CancelIrSnsCalib(rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                }
            }

            usleep(40 * 1000);
        }
    }

    if (enCalibType >= IPC_IR_DYNAMIC_CALIB_TYPE_OCC_B) {
        usleep(80 * 1000);

        s32Ret = AR_MPI_ISP_StartIrOccCalib(rawPipe, &pstViSnsWrapper->stIrOccCalibCfg);
        if (s32Ret) {
            PRINT_ERR("pipe[%d] start ir occ calib failed\n", rawPipe);
            goto EXIT;
        }

        while (1) {
            s32Ret = AR_MPI_ISP_GetIrOccCalibResult(rawPipe, &stOccCalibResult);
            if (s32Ret) {
                PRINT_ERR("pipe[%d] get ir occ calib result failed\n", rawPipe);
                AR_MPI_ISP_CancelIrOccCalib(rawPipe);
                goto EXIT;
            } else {
                if (stOccCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                    break;
                } else if (stOccCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                    PRINT_ERR("pipe[%d] ir occ calib cancelled\n", rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                } else if (stOccCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                    PRINT_ERR("pipe[%d] ir occ calib failed\n", rawPipe);
                    AR_MPI_ISP_CancelIrOccCalib(rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                }
            }
        }
    }

    if (enCalibType >= IPC_IR_DYNAMIC_CALIB_TYPE_B && thrmlPipe >= 0) {
        usleep(80 * 1000);

        s32Ret = AR_MPI_ISP_StartIrBCalib(thrmlPipe, &pstViSnsWrapper->pstNextVi->stIrBCalibCfg);
        if (s32Ret) {
            PRINT_ERR("pipe[%d] start ir b calib failed\n", thrmlPipe);
            goto EXIT;
        }

        while (1) {
            s32Ret = AR_MPI_ISP_GetIrBCalibResult(thrmlPipe, &stBCalibResult);
            if (s32Ret) {
                PRINT_ERR("pipe[%d] get ir b calib result failed\n", thrmlPipe);
                AR_MPI_ISP_CancelIrBCalib(thrmlPipe);
                goto EXIT;
            } else {
                if (stBCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                    break;
                } else if (stBCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                    PRINT_ERR("pipe[%d] ir b calib cancelled\n", thrmlPipe);
                    s32Ret = -1;
                    goto EXIT;
                } else if (stBCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                    PRINT_ERR("pipe[%d] ir b calib failed\n", thrmlPipe);
                    AR_MPI_ISP_CancelIrBCalib(thrmlPipe);
                    s32Ret = -1;
                    goto EXIT;
                }
            }

            usleep(40 * 1000);
        }
    }

    s32Ret = IrOpenShutter(rawPipe);
    if (s32Ret) {
        PRINT_ERR("pipe[%d] open shutter failed\n", rawPipe);
        goto EXIT;
    }

    //gettimeofday(&stEndTime, NULL);
    //PRINT_INFO("ir dynamic calib time: %d ms\n", get_time_interval_us(stStartTime, stEndTime) / 1000);

EXIT:
    return s32Ret;
}

AR_VOID *IrDynamicCalibFunc(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    VI_WRAPPER_S *pstViSnsWrapper = (VI_WRAPPER_S *)arg;
    AR_BOOL bFirstCalib = AR_TRUE;
    struct timeval stLastDynamicCalibTime;
    struct timeval stCurTime;

    PRINT_INFO("ir dynamic calib thread start\n");

    gettimeofday(&stLastDynamicCalibTime, NULL);

    while (pstViSnsWrapper->bIrDynamicCalibThreadStart) {
        gettimeofday(&stCurTime, NULL);

        if (get_time_interval_us(stLastDynamicCalibTime, stCurTime) >= pstViSnsWrapper->u32DynamicCalibIntervalMs * 1000
            || bFirstCalib) {
            s32Ret = IrDynamicCalib(pstViSnsWrapper,
                (bFirstCalib ? pstViSnsWrapper->enFirstIrDynamicCalibType : pstViSnsWrapper->enIrDynamicCalibType));
            if (s32Ret) {
                ar_err("ir dynamic calib failed");
            }

            stLastDynamicCalibTime = stCurTime;
            bFirstCalib = AR_FALSE;
        }

        sleep(1);
    }

    PRINT_INFO("ir dynamic calib thread end\n");
    return NULL;
}


/* This is only to show how to use ge2d to fuss two yuv imgs.
 * There's no align between two imgs since we use two independent cameras which vision are totally different.
 * 0. get frame from vpss grp 1 chn 2 for rgb, same size with vo chn
 * 1. get frame from vpss grp 2 chn 0 for IR.
 * 2. fuss ir img and rgb img to the rgb buffer(grp 1 chn 2 is reserved for fussion).
 * 3. send to vo chn
 * Note: get frame from vpss and vpss chn can be set to vo chn size.
 *       vpss may do rotate/flip/mirror, so get frame from vpss instead of vi chn.
*/
#ifndef THERMAL_TYPE

extern INF_ALGO_OBJECT_S* g_pstInfAlgObject_ars31;
extern STRU_MMZ_YUV_BUFFER_T gColorMapTmpBuf;
extern AR_BOOL bFussionRun;

AR_S32 ColorMapHandle(VIDEO_FRAME_INFO_S *pstIrFrame);

AR_VOID * IrFussionThreadFunc(AR_VOID *arg)
{
    AR_S32 s32Ret = 0; 
    AR_S32 s32Ge2dFd = 0;
    AR_S32 s32Ge2dFrame = 0;
    AR_S32 s32Cnt = 0;

    VPSS_GRP VpssGrpRgb = 1;
    VPSS_CHN VpssChnRgb = 3;
    VPSS_GRP VpssGrpIr = 2;
    VPSS_CHN VpssChnIr = 0;
    VO_LAYER VoLayer = 0;
    VO_CHN   VoChn = 2; 

    VIDEO_FRAME_INFO_S stFrameInfoRgb = {0};
    VIDEO_FRAME_INFO_S stFrameInfoIr = {0};
    STRU_GE2D_CLIENT_PARA stGe2dClientPara = {0};
    STRU_GE2D_STRETCH_BLIT_PARA stGe2dStretchPara = {0};
    STRU_GE2D_START_FRAME_PARA stGe2dFramePara = {0};
    
    IPC_CFG_CHANNEL_S * pstChnCfg = IPC_CFG_CHANNEL_GetParam();
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();
    
    PRINT_INFO("Enter fussion task.\n");
    //0. check if both rgb cameras and ir cameras exist.
    if(strcmp(pstChnCfg->stChannel[0].chType, "low_bw") == 0 
      && strcmp(pstChnCfg->stChannel[1].chType, "infrared") == 0)
    {
        bFussionRun = AR_TRUE;
    }
    else
    {
        PRINT_INFO("Not in rgb+ir mode.\n");
        return NULL;
    }

    if(!pstVpssCfg->stGrpCfg[VpssGrpRgb].bGroup 
        || !pstVpssCfg->stGrpCfg[VpssGrpRgb].stChnCfg[VpssChnRgb].bEnable)
    {
        PRINT_INFO("No RGB chn for fussion, quit the fussion process...\n");
        return NULL;
    }
    
    if(!pstVioCfg->vo.enable)
    {
        PRINT_INFO("VO is not enabled, quit the fussion process...\n");
        return NULL;
    }
    
    //wait a while, maybe vpss has not started yet.
    while(s32Cnt++ < 100)
    {
        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrpRgb, VpssChnRgb, &stFrameInfoRgb, 1000);
        if(s32Ret)
        {
            //PRINT_INFO("Get vpss 1/3 failed.\n");
            usleep(30*1000);
            continue;
        }

        break;
    }

    if(s32Cnt >= 100)
    {
        PRINT_INFO("vpss grp 1 chn 3 not enabled, quit the fussion process..\n");
        return NULL;
    }

    VIDEO_FRAME_INFO_S visFrame = stFrameInfoRgb;
    AR_MPI_VPSS_ReleaseChnFrame(VpssGrpRgb, VpssChnRgb, &stFrameInfoRgb);

    PRINT_INFO("inf algo object %p",g_pstInfAlgObject_ars31);
    if(!g_pstInfAlgObject_ars31)
    {
        PRINT_ERR("inf alg object is null");
        return NULL;
    }
    s32Ret =  g_pstInfAlgObject_ars31->fusion_init(g_pstInfAlgObject_ars31,
                    visFrame.stVFrame.u32Width,visFrame.stVFrame.u32Height,
                    visFrame.stVFrame.u32Stride[0],visFrame.stVFrame.u32Stride[1]);
    if(s32Ret!=0){
        PRINT_ERR("fusion_init failed");
        return NULL;
    }
    
    while(bFussionRun)
    {
        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrpRgb, VpssChnRgb, &stFrameInfoRgb, 1000);
        if(s32Ret)
        {
            continue;
        }

        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrpIr, VpssChnIr, &stFrameInfoIr, 1000);
        if(s32Ret)
        {
            AR_MPI_VPSS_ReleaseChnFrame(VpssGrpRgb, VpssChnRgb, &stFrameInfoRgb);
            continue;
        }

        //ColorMapHandle(&stFrameInfoIr);

        s32Ret = g_pstInfAlgObject_ars31->fusion_process(g_pstInfAlgObject_ars31,&stFrameInfoRgb,&stFrameInfoIr);
        if(s32Ret==0){
            AR_MPI_VO_SendFrame(VoLayer, VoChn, &stFrameInfoRgb, 1000);
        }

        AR_MPI_VPSS_ReleaseChnFrame(VpssGrpRgb, VpssChnRgb, &stFrameInfoRgb);
        AR_MPI_VPSS_ReleaseChnFrame(VpssGrpIr, VpssChnIr, &stFrameInfoIr);
    }

    g_pstInfAlgObject_ars31->fusion_release(g_pstInfAlgObject_ars31);

    return NULL;
}

AR_S32 ColorMapHandle(VIDEO_FRAME_INFO_S *pstIrFrame)
{
    AR_S32 s32Ret;

    if(NULL ==g_pstInfAlgObject_ars31)
    {
        ar_err("inf alg not init");
        return -1;
    }

#if 1
    if(gColorMapTmpBuf.u64PhyAddr == 0){
        //malloc cacheable mmz buffer
        int y_offset = pstIrFrame->stVFrame.u64PhyAddr[1] - pstIrFrame->stVFrame.u64PhyAddr[0];
        int u_offset = pstIrFrame->stVFrame.u64PhyAddr[2] - pstIrFrame->stVFrame.u64PhyAddr[1];
        int all_len = y_offset+u_offset*2;

        s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&gColorMapTmpBuf.u64PhyAddr, &gColorMapTmpBuf.pVirAddr, 
                NULL, NULL, all_len);
        if(s32Ret!=0){
            ar_err("mmz malloc failed");
            gColorMapTmpBuf.u64PhyAddr = 0;
            return -1;
        }
        gColorMapTmpBuf.pVirAddr_u = gColorMapTmpBuf.pVirAddr+y_offset;
        gColorMapTmpBuf.pVirAddr_v = gColorMapTmpBuf.pVirAddr_u+u_offset;
        gColorMapTmpBuf.u32BufLen = all_len;
    }

    ar_hal_sys_memcpy_pa(gColorMapTmpBuf.u64PhyAddr, pstIrFrame->stVFrame.u64PhyAddr[0], gColorMapTmpBuf.u32BufLen);
    void* y_vaddr = gColorMapTmpBuf.pVirAddr;
    void* u_vaddr = gColorMapTmpBuf.pVirAddr_u;
    void* v_vaddr = gColorMapTmpBuf.pVirAddr_v;

    //color map
    s32Ret = g_pstInfAlgObject_ars31->mapping_color_process(g_pstInfAlgObject_ars31,
        y_vaddr, u_vaddr, v_vaddr,
        pstIrFrame->stVFrame.u32Width, pstIrFrame->stVFrame.u32Height,
        pstIrFrame->stVFrame.u32Stride[0], pstIrFrame->stVFrame.u32Stride[1], pstIrFrame->stVFrame.u32Stride[2]);
    if(s32Ret!=0){
        ar_err("color map failed");
        return -1;
    }
    
    ar_hal_sys_mmz_flush_cache_pa(gColorMapTmpBuf.u64PhyAddr, gColorMapTmpBuf.u32BufLen);
    ar_hal_sys_memcpy_pa(pstIrFrame->stVFrame.u64PhyAddr[0], gColorMapTmpBuf.u64PhyAddr, gColorMapTmpBuf.u32BufLen);
#else
    void* y_vaddr = (void*)pstIrFrame->stVFrame.u64VirAddr[0];
    void* u_vaddr = (void*)pstIrFrame->stVFrame.u64VirAddr[1];
    void* v_vaddr = (void*)pstIrFrame->stVFrame.u64VirAddr[2];

    //color map
    s32Ret = g_pstInfAlgObject_ars31->mapping_color_process(g_pstInfAlgObject_ars31,
        y_vaddr, u_vaddr, v_vaddr,
        pstIrFrame->stVFrame.u32Width, pstIrFrame->stVFrame.u32Height,
        pstIrFrame->stVFrame.u32Stride[0], pstIrFrame->stVFrame.u32Stride[1], pstIrFrame->stVFrame.u32Stride[2]);
    if(s32Ret!=0){
        ar_err("color map failed");
        return -1;
    }
#endif

    return 0;
}
#endif //end THERMAL_TYPE

