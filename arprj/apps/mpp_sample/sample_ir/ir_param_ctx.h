#ifndef _IR_PARAM_CTX_H_
#define _IR_PARAM_CTX_H_

#include <pthread.h>
#include "ar_comm_isp.h"


#define IR_OCC_BUF_ALIGN        256
#define IR_KB_BUF_ALIGN         16
#define IR_AUTO_GTM_HIST_BINS   (1 << 14)


typedef struct {
    VI_PIPE IrSnsPipe;
    SIZE_S stIrSnsSize;
    RECT_S stIrSnsRoi;
    AR_U32 u32IrSnsParamSize;
    VI_PIPE IrIspPipe;
    SIZE_S stIrIspSize;
    RECT_S stIrIspRoi;
} IR_PARAM_CTX_CFG_S;

typedef struct {
    VI_PIPE IrSnsPipe;
    SIZE_S stIrSnsSize;
    RECT_S stIrSnsRoi;
    AR_U32 u32IrSnsParamSize;
    VI_PIPE IrIspPipe;
    SIZE_S stIrIspSize;
    RECT_S stIrIspRoi;

    AR_VOID *pSnsParam;
    AR_BOOL bSnsParamValid;
    ISP_IR_OCC_PARAM_S stOccParam;
    AR_BOOL bOccParamValid;
    ISP_IR_KB_PARAM_S stKbParam;
    AR_BOOL bKbParamValid;
    ISP_IR_GTM_PARAM_S stGtmParam;

    AR_U32 u32OccBufSize;
    AR_U64 u64OccBufOriginPhy;
    AR_VOID *pOccBufOriginVirt;

    AR_U32 u32KbBufSize;
    AR_U64 u64KbBufOriginPhy;
    AR_VOID *pKbBufOriginVirt;
} IR_PARAM_CTX_S;


IR_PARAM_CTX_S *CreateIrParamCtx(IR_PARAM_CTX_CFG_S *pstCfg);
AR_VOID DestroyIrParamCtx(IR_PARAM_CTX_S *pstCtx);
AR_S32 LoadSnsParamCalibData(IR_PARAM_CTX_S *pstCtx, AR_CHAR *strFname);
AR_S32 LoadOccCalibData(IR_PARAM_CTX_S *pstCtx, AR_CHAR *strFname);
AR_S32 LoadKbCalibData(IR_PARAM_CTX_S *pstCtx, AR_CHAR *strFname);
AR_S32 ApplyAllIrParam(IR_PARAM_CTX_S *pstCtx);
AR_S32 ApplyIrSnsParam(IR_PARAM_CTX_S *pstCtx);
AR_S32 ApplyIrOccParam(IR_PARAM_CTX_S *pstCtx);
AR_S32 ApplyIrKbParam(IR_PARAM_CTX_S *pstCtx);
AR_S32 ApplyIrGtmParam(IR_PARAM_CTX_S *pstCtx);
AR_VOID *GetIrSnsParam(IR_PARAM_CTX_S *pstCtx);
ISP_IR_OCC_PARAM_S *GetIrOccParam(IR_PARAM_CTX_S *pstCtx);
ISP_IR_KB_PARAM_S *GetIrKbParam(IR_PARAM_CTX_S *pstCtx);
ISP_IR_GTM_PARAM_S *GetIrGtmParam(IR_PARAM_CTX_S *pstCtx);
AR_S32 IrKbCalib(IR_PARAM_CTX_S *pstIrParamCtx, IR_KB_CALIB_CFG_S *pstCalibCfg,
    AR_CHAR *strLTRawFname, AR_CHAR *strHTRawFname, AR_CHAR *strKBFname);


#endif // _IR_PARAM_CTX_H_