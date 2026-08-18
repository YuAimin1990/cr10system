#include "hal_vin_log.h"
#include "mpi_sys.h"
#include "mpi_isp.h"
#include "ir_param_ctx.h"


IR_PARAM_CTX_S *CreateIrParamCtx(IR_PARAM_CTX_CFG_S *pstCfg)
{
    AR_S32 s32Ret = 0;
    IR_PARAM_CTX_S *pstCtx = NULL;
    AR_U32 u32BufSize;
    AR_U16 *pKb = NULL;
    AR_U32 i = 0;

    if (pstCfg == NULL) {
        ar_err("pstCfg is null");
        return NULL;
    }

    pstCtx = malloc(sizeof(IR_PARAM_CTX_S));
    if (pstCtx == NULL) {
        ar_err("allocate ir param ctx failed");
        return NULL;
    }

    memset(pstCtx, 0, sizeof(IR_PARAM_CTX_S));

    pstCtx->IrSnsPipe = pstCfg->IrSnsPipe;
    pstCtx->stIrSnsSize = pstCfg->stIrSnsSize;
    pstCtx->stIrSnsRoi = pstCfg->stIrSnsRoi;
    pstCtx->u32IrSnsParamSize = pstCfg->u32IrSnsParamSize;
    pstCtx->IrIspPipe = pstCfg->IrIspPipe;
    pstCtx->stIrIspSize = pstCfg->stIrIspSize;
    pstCtx->stIrIspRoi = pstCfg->stIrIspRoi;

    if (pstCtx->IrSnsPipe >= 0) {
        // Allocate ir sensor param buf
        pstCtx->pSnsParam = malloc(pstCtx->u32IrSnsParamSize);
        if (pstCtx->pSnsParam == NULL) {
            ar_err("allocate ir param ctx failed");
            goto FAIL1;
        }

        // Allocate OCC buf
        pstCtx->stOccParam.stOccBuf.u32Width = pstCtx->stIrSnsSize.u32Width;
        pstCtx->stOccParam.stOccBuf.u32Height = pstCtx->stIrSnsSize.u32Height;
        pstCtx->stOccParam.stOccBuf.u32Stride[0] = CAM_ALIGNE_TO(pstCtx->stIrSnsSize.u32Width, IR_OCC_BUF_ALIGN);
        u32BufSize = pstCtx->stOccParam.stOccBuf.u32Stride[0] * pstCtx->stIrSnsSize.u32Height;
        pstCtx->u32OccBufSize = u32BufSize + 256;

        s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstCtx->u64OccBufOriginPhy,
            &pstCtx->pOccBufOriginVirt, NULL, NULL, pstCtx->u32OccBufSize);
        if (s32Ret) {
            ar_err("allocate occ buf failed");
            goto FAIL2;
        }

        pstCtx->stOccParam.stOccBuf.u64PhyAddr[0] = CAM_ALIGNE_TO(pstCtx->u64OccBufOriginPhy, 256);
        pstCtx->stOccParam.stOccBuf.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pstCtx->pOccBufOriginVirt, 256);

        ar_always("allocate occ buf: size[%u %u] orig[0x%llx %p %u] p0[0x%llx 0x%llx %u]",
            pstCtx->stOccParam.stOccBuf.u32Width, pstCtx->stOccParam.stOccBuf.u32Height,
            pstCtx->u64OccBufOriginPhy, pstCtx->pOccBufOriginVirt, u32BufSize,
            pstCtx->stOccParam.stOccBuf.u64PhyAddr[0], pstCtx->stOccParam.stOccBuf.u64VirAddr[0],
            pstCtx->stOccParam.stOccBuf.u32Stride[0]);

        // Set default value to OCC buf
        memset((AR_VOID *)pstCtx->stOccParam.stOccBuf.u64VirAddr[0], 0x20, u32BufSize);
        s32Ret = AR_MPI_SYS_MmzFlushCache(pstCtx->u64OccBufOriginPhy,
            pstCtx->pOccBufOriginVirt, pstCtx->u32OccBufSize);
        if (s32Ret) {
            ar_err("flush occ data buffer failed");
            goto FAIL3;
        }
    }

    if (pstCtx->IrIspPipe >= 0) {
        // Allocate KB buf
        pstCtx->stKbParam.stKbBuf.u32Width = pstCtx->stIrIspSize.u32Width;
        pstCtx->stKbParam.stKbBuf.u32Height = pstCtx->stIrIspSize.u32Height;
        pstCtx->stKbParam.stKbBuf.u32Stride[0] = CAM_ALIGNE_TO(pstCtx->stKbParam.stKbBuf.u32Width * 2, IR_KB_BUF_ALIGN);
        pstCtx->stKbParam.stKbBuf.u32Stride[1] = pstCtx->stKbParam.stKbBuf.u32Stride[0];
        u32BufSize = pstCtx->stKbParam.stKbBuf.u32Stride[0] * pstCtx->stKbParam.stKbBuf.u32Height * 2;
        pstCtx->u32KbBufSize = u32BufSize + 256;

        s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstCtx->u64KbBufOriginPhy,
            &pstCtx->pKbBufOriginVirt, NULL, NULL, pstCtx->u32KbBufSize);
        if (s32Ret) {
            ar_err("allocate kb buf failed");
            goto FAIL3;
        }

        pstCtx->stKbParam.stKbBuf.u64PhyAddr[0] = CAM_ALIGNE_TO(pstCtx->u64KbBufOriginPhy, 256);
        pstCtx->stKbParam.stKbBuf.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pstCtx->pKbBufOriginVirt, 256);
        pstCtx->stKbParam.stKbBuf.u64PhyAddr[1] = pstCtx->stKbParam.stKbBuf.u64PhyAddr[0] + u32BufSize / 2;
        pstCtx->stKbParam.stKbBuf.u64VirAddr[1] = pstCtx->stKbParam.stKbBuf.u64VirAddr[0] + u32BufSize / 2;

        pstCtx->stKbParam.bKbcEn = AR_TRUE;
        pstCtx->stKbParam.bDpcEn = AR_TRUE;

        ar_always("allocate kb buf: size[%u %u] orig[0x%llx %p %u] p0[0x%llx 0x%llx %u] p1[0x%llx 0x%llx %u]",
            pstCtx->stKbParam.stKbBuf.u32Width, pstCtx->stKbParam.stKbBuf.u32Height,
            pstCtx->u64KbBufOriginPhy, pstCtx->pKbBufOriginVirt, u32BufSize,
            pstCtx->stKbParam.stKbBuf.u64PhyAddr[0], pstCtx->stKbParam.stKbBuf.u64VirAddr[0],
            pstCtx->stKbParam.stKbBuf.u32Stride[0], pstCtx->stKbParam.stKbBuf.u64PhyAddr[1],
            pstCtx->stKbParam.stKbBuf.u64VirAddr[1], pstCtx->stKbParam.stKbBuf.u32Stride[1]);

        // Set default value to KB buf
        pKb = (AR_U16 *)pstCtx->stKbParam.stKbBuf.u64VirAddr[0];
        for (i = 0; i < u32BufSize / sizeof(AR_U16); i++)
            *pKb++ = 8192;

        s32Ret = AR_MPI_SYS_MmzFlushCache(pstCtx->u64KbBufOriginPhy,
            pstCtx->pKbBufOriginVirt, pstCtx->u32KbBufSize);
        if (s32Ret) {
            ar_err("flush kb data buffer failed");
            goto FAIL4;
        }

        pstCtx->stGtmParam.u32GtmContrast = 64;
        pstCtx->stGtmParam.s32GtmBright = 0;
    }

    return pstCtx;

FAIL4:
    AR_MPI_SYS_MmzFree(pstCtx->u64KbBufOriginPhy, pstCtx->pKbBufOriginVirt);
FAIL3:
    AR_MPI_SYS_MmzFree(pstCtx->u64OccBufOriginPhy, pstCtx->pOccBufOriginVirt);
FAIL2:
    free(pstCtx->pSnsParam);
FAIL1:
    free(pstCtx);
    return NULL;
}

AR_VOID DestroyIrParamCtx(IR_PARAM_CTX_S *pstCtx)
{
    if (pstCtx == NULL) {
        return;
    }

    if (pstCtx->u64KbBufOriginPhy) {
        AR_MPI_SYS_MmzFree(pstCtx->u64KbBufOriginPhy, pstCtx->pKbBufOriginVirt);
    }

    if (pstCtx->u64OccBufOriginPhy) {
        AR_MPI_SYS_MmzFree(pstCtx->u64OccBufOriginPhy, pstCtx->pOccBufOriginVirt);
    }

    if (pstCtx->pSnsParam) {
        free(pstCtx->pSnsParam);
    }

    free(pstCtx);
}

AR_S32 LoadSnsParamCalibData(IR_PARAM_CTX_S *pstCtx, AR_CHAR *strFname)
{
    AR_S32 s32Ret = 0;
    FILE *fp = NULL;
    AR_U32 u32ReadSize = 0;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (strFname == NULL) {
        ar_err("strFname is null");
        return -1;
    }

    if (pstCtx->pSnsParam == NULL) {
        ar_err("sensor param buffer is null");
        return -1;
    }

    fp = fopen(strFname, "rb");
    if (fp == NULL) {
        ar_err("open sensor calib data file [%s] failed!", strFname);
        return -1;
    }

    // Read IR params data
    u32ReadSize = fread((AR_VOID *)pstCtx->pSnsParam, 1, pstCtx->u32IrSnsParamSize, fp);
    if (u32ReadSize != pstCtx->u32IrSnsParamSize) {
        ar_err("read IR params data from file[%s] failed! size[%u] read_size[%u]",
            strFname, pstCtx->u32IrSnsParamSize, u32ReadSize);
        s32Ret = -1;
    }

    fclose(fp);

    if (s32Ret == 0) {
        pstCtx->bSnsParamValid = AR_TRUE;
    }

    return s32Ret;
}

AR_S32 LoadOccCalibData(IR_PARAM_CTX_S *pstCtx, AR_CHAR *strFname)
{
    AR_S32 s32Ret = 0;
    FILE *fp = NULL;
    AR_U32 u32OccSize = 0;
    AR_U32 u32ReadSize = 0;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (strFname == NULL) {
        ar_err("strFname is null");
        return -1;
    }

    if (pstCtx->stOccParam.stOccBuf.u64VirAddr[0] == 0) {
        ar_err("occ buffer is null");
        return -1;
    }

    u32OccSize = pstCtx->stOccParam.stOccBuf.u32Stride[0] * pstCtx->stOccParam.stOccBuf.u32Height;

    fp = fopen(strFname, "rb");
    if (fp == NULL) {
        ar_err("open sensor calib data file [%s] failed!", strFname);
        return -1;
    }

    // Read OCC data
    u32ReadSize = fread((AR_VOID *)pstCtx->stOccParam.stOccBuf.u64VirAddr[0], 1, u32OccSize, fp);
    if (u32ReadSize != u32OccSize) {
        ar_err("read occ data from file[%s] failed! size[%u] read_size[%u]",
            strFname, u32OccSize, u32ReadSize);
        s32Ret = -1;
        goto EXIT;
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(pstCtx->u64OccBufOriginPhy,
        pstCtx->pOccBufOriginVirt, pstCtx->u32OccBufSize);
    if (s32Ret) {
        ar_err("flush occ data buffer failed");
        goto EXIT;
    }

    pstCtx->stOccParam.bIsSingleVal = AR_FALSE;

EXIT:
    fclose(fp);

    if (s32Ret == 0) {
        pstCtx->bOccParamValid = AR_TRUE;
    }

    return s32Ret;
}

AR_S32 LoadKbCalibData(IR_PARAM_CTX_S *pstCtx, AR_CHAR *strFname)
{
    AR_S32 s32Ret = 0;
    FILE *fp = NULL;
    AR_U32 u32KbSize = 0;
    AR_U32 u32ReadSize = 0;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (strFname == NULL) {
        ar_err("strFname is null");
        return -1;
    }

    if (pstCtx->stKbParam.stKbBuf.u64VirAddr[0] == 0) {
        ar_err("kb buffer is null");
        return -1;
    }

    u32KbSize = pstCtx->stKbParam.stKbBuf.u32Stride[0] * pstCtx->stKbParam.stKbBuf.u32Height * 2;

    fp = fopen(strFname, "rb");
    if (fp == NULL) {
        ar_err("open kb calib data file [%s] failed!", strFname);
        return -1;
    }

    // Read KB data
    u32ReadSize = fread((AR_VOID *)pstCtx->stKbParam.stKbBuf.u64VirAddr[0], 1, u32KbSize, fp);
    if (u32ReadSize != u32KbSize) {
        ar_err("read kb data from file[%s] failed! size[%u] read_size[%u]",
            strFname, u32KbSize, u32ReadSize);
        s32Ret = -1;
        goto EXIT;
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(pstCtx->u64KbBufOriginPhy,
        pstCtx->pKbBufOriginVirt, pstCtx->u32KbBufSize);
    if (s32Ret) {
        ar_err("flush kb data buffer failed");
        goto EXIT;
    }

EXIT:
    fclose(fp);

    if (s32Ret == 0) {
        pstCtx->bKbParamValid = AR_TRUE;
    }

    return s32Ret;
}

AR_S32 ApplyAllIrParam(IR_PARAM_CTX_S *pstCtx)
{
    AR_S32 s32Ret = 0;
    ISP_IR_PARAMS_S stIrParams;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (pstCtx->IrSnsPipe >= 0) {
        stIrParams.u32ParamCnt = 0;

        if (pstCtx->bSnsParamValid) {
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].enParamType = IR_PARAM_TYPE_SENSOR;
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].pParamData = pstCtx->pSnsParam;
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].u32ParamSize = pstCtx->u32IrSnsParamSize;
            stIrParams.u32ParamCnt++;
        }

        if (pstCtx->bOccParamValid) {
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].enParamType = IR_PARAM_TYPE_OCC;
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].pParamData = &pstCtx->stOccParam;
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].u32ParamSize = sizeof(pstCtx->stOccParam);
            stIrParams.u32ParamCnt++;
        }

        s32Ret = AR_MPI_ISP_SetIrParams(pstCtx->IrSnsPipe, &stIrParams);
        if (s32Ret) {
            ar_err("set ir params failed");
            goto EXIT;
        }
    }

    if (pstCtx->IrIspPipe >= 0) {
        stIrParams.u32ParamCnt = 0;

        if (pstCtx->bKbParamValid) {
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].enParamType = IR_PARAM_TYPE_KB;
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].pParamData = &pstCtx->stKbParam;
            stIrParams.astParamEntry[stIrParams.u32ParamCnt].u32ParamSize = sizeof(pstCtx->stKbParam);
            stIrParams.u32ParamCnt++;
        }

        stIrParams.astParamEntry[stIrParams.u32ParamCnt].enParamType = IR_PARAM_TYPE_GTM;
        stIrParams.astParamEntry[stIrParams.u32ParamCnt].pParamData = &pstCtx->stGtmParam;
        stIrParams.astParamEntry[stIrParams.u32ParamCnt].u32ParamSize = sizeof(pstCtx->stGtmParam);
        stIrParams.u32ParamCnt++;

        s32Ret = AR_MPI_ISP_SetIrParams(pstCtx->IrIspPipe, &stIrParams);
        if (s32Ret) {
            ar_err("set ir params failed");
            goto EXIT;
        }
    }

EXIT:
    return s32Ret;
}

AR_S32 ApplyIrSnsParam(IR_PARAM_CTX_S *pstCtx)
{
    AR_S32 s32Ret = 0;
    ISP_IR_PARAMS_S stIrParams;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (!pstCtx->bSnsParamValid) {
        ar_err("sensor param is not valid");
        return -1;
    }

    if (pstCtx->IrSnsPipe >= 0) {
        stIrParams.u32ParamCnt = 1;

        stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_SENSOR;
        stIrParams.astParamEntry[0].pParamData = pstCtx->pSnsParam;
        stIrParams.astParamEntry[0].u32ParamSize = pstCtx->u32IrSnsParamSize;

        s32Ret = AR_MPI_ISP_SetIrParams(pstCtx->IrSnsPipe, &stIrParams);
        if (s32Ret) {
            ar_err("set ir params failed");
            goto EXIT;
        }
    }

EXIT:
    return s32Ret;
}

AR_S32 ApplyIrOccParam(IR_PARAM_CTX_S *pstCtx)
{
    AR_S32 s32Ret = 0;
    ISP_IR_PARAMS_S stIrParams;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (!pstCtx->bOccParamValid) {
        ar_err("occ param is not valid");
        return -1;
    }

    if (pstCtx->IrSnsPipe >= 0) {
        stIrParams.u32ParamCnt = 1;

        stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
        stIrParams.astParamEntry[0].pParamData = &pstCtx->stOccParam;
        stIrParams.astParamEntry[0].u32ParamSize = sizeof(pstCtx->stOccParam);

        s32Ret = AR_MPI_ISP_SetIrParams(pstCtx->IrSnsPipe, &stIrParams);
        if (s32Ret) {
            ar_err("set ir params failed");
            goto EXIT;
        }
    }

EXIT:
    return s32Ret;
}

AR_S32 ApplyIrKbParam(IR_PARAM_CTX_S *pstCtx)
{
    AR_S32 s32Ret = 0;
    ISP_IR_PARAMS_S stIrParams;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (!pstCtx->bKbParamValid) {
        ar_err("kb param is not valid");
        return -1;
    }

    if (pstCtx->IrIspPipe >= 0) {
        stIrParams.u32ParamCnt = 1;

        stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_KB;
        stIrParams.astParamEntry[0].pParamData = &pstCtx->stKbParam;
        stIrParams.astParamEntry[0].u32ParamSize = sizeof(pstCtx->stKbParam);

        s32Ret = AR_MPI_ISP_SetIrParams(pstCtx->IrIspPipe, &stIrParams);
        if (s32Ret) {
            ar_err("set ir params failed");
            goto EXIT;
        }
    }

EXIT:
    return s32Ret;
}

AR_S32 ApplyIrGtmParam(IR_PARAM_CTX_S *pstCtx)
{
    AR_S32 s32Ret = 0;
    ISP_IR_PARAMS_S stIrParams;

    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return -1;
    }

    if (pstCtx->IrIspPipe >= 0) {
        stIrParams.u32ParamCnt = 1;

        stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_GTM;
        stIrParams.astParamEntry[0].pParamData = &pstCtx->stGtmParam;
        stIrParams.astParamEntry[0].u32ParamSize = sizeof(pstCtx->stGtmParam);

        s32Ret = AR_MPI_ISP_SetIrParams(pstCtx->IrIspPipe, &stIrParams);
        if (s32Ret) {
            ar_err("set ir params failed");
            goto EXIT;
        }
    }

EXIT:
    return s32Ret;
}

AR_VOID *GetIrSnsParam(IR_PARAM_CTX_S *pstCtx)
{
    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return NULL;
    }

    return pstCtx->pSnsParam;
}

ISP_IR_OCC_PARAM_S *GetIrOccParam(IR_PARAM_CTX_S *pstCtx)
{
    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return NULL;
    }

    return &(pstCtx->stOccParam);
}

ISP_IR_KB_PARAM_S *GetIrKbParam(IR_PARAM_CTX_S *pstCtx)
{
    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return NULL;
    }

    return &(pstCtx->stKbParam);
}

ISP_IR_GTM_PARAM_S *GetIrGtmParam(IR_PARAM_CTX_S *pstCtx)
{
    if (pstCtx == NULL) {
        ar_err("pstCtx is null");
        return NULL;
    }

    return &(pstCtx->stGtmParam);
}

static AR_S32 ReadRawFrame(AR_CHAR *strFname, VIDEO_FRAME_S *pstFrm)
{
    AR_S32 s32Ret = 0;
    FILE *fp = NULL;
    AR_U32 u32RawLinePixs = 0;
    AR_U32 u32RawSize = 0;
    AR_U32 u32FrmCnt = 0;
    AR_U16 *pu16RawTempBuf = NULL;
    AR_U32 *pu32RawSumBuf = NULL;
    AR_U32 u32WSize = 0;
    AR_U32 i = 0;
    AR_U32 j = 0;

    u32RawLinePixs = pstFrm->u32Stride[0] / sizeof(AR_U16);
    u32RawSize = pstFrm->u32Stride[0] * pstFrm->u32Height;

    pu16RawTempBuf = malloc(u32RawSize * 3);
    if (pu16RawTempBuf == NULL) {
        ar_err("allocate raw temp buf failed");
        s32Ret = -1;
        goto EXIT;
    }

    pu32RawSumBuf = (AR_U32 *)(pu16RawTempBuf + u32RawSize / sizeof(AR_U16));

    fp = fopen(strFname, "rb");
    if (fp == NULL) {
        ar_err("open raw file %s failed", strFname);
        s32Ret = -1;
        goto EXIT;
    }

    u32FrmCnt = 0;
    memset(pu32RawSumBuf, 0, u32RawSize * 2);

    while (1) {
        u32WSize = fread(pu16RawTempBuf, 1, u32RawSize, fp);
        if (u32WSize != u32RawSize) {
            break;
        }

        u32FrmCnt++;

        AR_U16 *pSrc = pu16RawTempBuf;
        AR_U32 *pDst = pu32RawSumBuf;
        for (i = 0; i < pstFrm->u32Height; i++) {
            for (j = 0; j < pstFrm->u32Width; j++) {
                pDst[j] += pSrc[j];
            }
            pSrc += u32RawLinePixs;
            pDst += u32RawLinePixs;
        }
    }

    fclose(fp);

    if (u32FrmCnt > 0) {
        AR_U32 *pSrc = pu32RawSumBuf;
        AR_U16 *pDst = (AR_U16 *)pstFrm->u64VirAddr[0];
        for (i = 0; i < pstFrm->u32Height; i++) {
            for (j = 0; j < pstFrm->u32Width; j++) {
                pDst[j] = pSrc[j] / u32FrmCnt;
            }
            pSrc += u32RawLinePixs;
            pDst += u32RawLinePixs;
        }
    } else {
        ar_err("read raw file %s failed", strFname);
        s32Ret = -1;
        goto EXIT;
    }

EXIT:
    if (pu16RawTempBuf) {
        free(pu16RawTempBuf);
    }
    return s32Ret;
}

AR_S32 IrKbCalib(IR_PARAM_CTX_S *pstIrParamCtx, IR_KB_CALIB_CFG_S *pstCalibCfg,
    AR_CHAR *strLTRawFname, AR_CHAR *strHTRawFname, AR_CHAR *strKBFname)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32RawStride = 0;
    AR_U32 u32RawLinePixs = 0;
    AR_U32 u32RawSize = 0;
    VIDEO_FRAME_S stLtRaw;
    VIDEO_FRAME_S stHtRaw;
    AR_U64 u64PhyOrig = 0;
    AR_VOID *pVirtOrig = NULL;
    AR_U32 u32KBSize = 0;
    FILE *fp = NULL;
    size_t wsize = 0;

    if (pstIrParamCtx == NULL) {
        ar_err("pstIrParamCtx is null");
        return -1;
    }

    if (pstCalibCfg == NULL) {
        ar_err("pstCalibCfg is null");
        return -1;
    }

    if (strLTRawFname == NULL) {
        ar_err("strLTRawFname is null");
        return -1;
    }

    if (strHTRawFname == NULL) {
        ar_err("strHTRawFname is null");
        return -1;
    }

    u32RawStride = CAM_ALIGNE_TO(pstIrParamCtx->stIrSnsSize.u32Width * sizeof(AR_U16), 256);
    u32RawLinePixs = u32RawStride / sizeof(AR_U16);
    u32RawSize = u32RawStride * pstIrParamCtx->stIrSnsSize.u32Height;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&u64PhyOrig, &pVirtOrig,
        NULL, NULL, u32RawSize * 2 + 256);
    if (s32Ret) {
        ar_err("allocate occ buf failed");
        goto EXIT1;
    }

    stLtRaw.u32Width = pstIrParamCtx->stIrSnsSize.u32Width;
    stLtRaw.u32Height = pstIrParamCtx->stIrSnsSize.u32Height;
    stLtRaw.u32Stride[0] = u32RawStride;
    stLtRaw.u64PhyAddr[0] = CAM_ALIGNE_TO(u64PhyOrig, 256);
    stLtRaw.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pVirtOrig, 256);

    stHtRaw.u32Width = pstIrParamCtx->stIrSnsSize.u32Width;
    stHtRaw.u32Height = pstIrParamCtx->stIrSnsSize.u32Height;
    stHtRaw.u32Stride[0] = u32RawStride;
    stHtRaw.u64PhyAddr[0] = stLtRaw.u64PhyAddr[0] + u32RawSize;
    stHtRaw.u64VirAddr[0] = stLtRaw.u64VirAddr[0] + u32RawSize;

    // Load low temperature raw data
    s32Ret = ReadRawFrame(strLTRawFname, &stLtRaw);
    if (s32Ret) {
        ar_err("read low temperature raw frame failed");
        goto EXIT2;
    }

    // Load high temperature raw data
    s32Ret = ReadRawFrame(strHTRawFname, &stHtRaw);
    if (s32Ret) {
        ar_err("read high temperature raw frame failed");
        goto EXIT2;
    }

    s32Ret = AR_MPI_ISP_IrKBCalib(pstIrParamCtx->IrIspPipe, pstCalibCfg,
        &stLtRaw, &stHtRaw, &pstIrParamCtx->stKbParam.stKbBuf);
    if (s32Ret) {
        ar_err("read high temperature raw frame failed");
        goto EXIT2;
    }

    if (strKBFname) {
        fp = fopen(strKBFname, "wb");
        if (fp == NULL) {
            ar_err("open kb data file %s failed", strKBFname);
            s32Ret = -1;
            goto EXIT2;
        }

        u32KBSize = pstIrParamCtx->stKbParam.stKbBuf.u32Stride[0] * pstIrParamCtx->stKbParam.stKbBuf.u32Height;

        wsize = fwrite((AR_VOID *)pstIrParamCtx->stKbParam.stKbBuf.u64VirAddr[0], 1, u32KBSize, fp);
        if (wsize != u32KBSize) {
            ar_err("write k data to file %s failed", strKBFname);
            s32Ret = -1;
            goto EXIT3;
        }

        wsize = fwrite((AR_VOID *)pstIrParamCtx->stKbParam.stKbBuf.u64VirAddr[1], 1, u32KBSize, fp);
        if (wsize != u32KBSize) {
            ar_err("write b data to file %s failed", strKBFname);
            s32Ret = -1;
            goto EXIT3;
        }
    }

EXIT3:
    if (fp) {
        fclose(fp);
    }
EXIT2:
    AR_MPI_SYS_MmzFree(u64PhyOrig, pVirtOrig);
EXIT1:
    return s32Ret;
}


