#ifndef _IR_SAMPLE_CTX_H_
#define _IR_SAMPLE_CTX_H_


#include "ar_comm_isp.h"
#include "ir_vi_group.h"
#include "ir_param_ctx.h"


typedef enum {
    IR_RAW_PROCESS_STATE_NONE = 0,
    IR_RAW_PROCESS_STATE_CAPTURE,
    IR_RAW_PROCESS_STATE_MANUAL_B_CALIB,
    IR_RAW_PROCESS_STATE_AUTO_B_CALIB,
} IR_RAW_PROCESS_STATE_E;

typedef enum {
    IR_VI_TOPOLOGY_TYPE_0 = 0,      // ir_raw --> ir_thermal --> ir_isp1 --> ir_isp2
    IR_VI_TOPOLOGY_TYPE_1,          // ir_raw --> (ir_isp1 + ir_thermal) --> ir_isp2
    IR_VI_TOPOLOGY_TYPE_2,          // ir_raw --> ir_thermal --> ir_isp1
    IR_VI_TOPOLOGY_TYPE_3,          // ir_raw --> (ir_isp1 + ir_thermal)
    IR_VI_TOPOLOGY_TYPE_4,          // ir_raw --> ir_thermal
    IR_VI_TOPOLOGY_TYPE_5,          // ir_raw
    IR_VI_TOPOLOGY_TYPE_6,          // ir_isp1 --> ir_isp2
} IR_VI_TOPOLOGY_TYPE_E;

typedef enum {
    IR_DYNAMIC_CALIB_TYPE_NONE = 0,
    IR_DYNAMIC_CALIB_TYPE_B,
    IR_DYNAMIC_CALIB_TYPE_OCC_B,
    IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B,
} IR_DYNAMIC_CALIB_TYPE_E;

typedef struct _IR_SAMPLE_CTX_S IR_SAMPLE_CTX_S;

struct _IR_SAMPLE_CTX_S {
    IR_VI_GROUP_CFG_S stIrViGrpCfg;
    IR_VI_GROUP_S *pstIrViGrp;
    AR_U32 u32IrSnsParamSize;
    RECT_S stIrSnsRoi;
    RECT_S stIrIspRoi;
    AR_CHAR *strInitSnsParamCalibFname;
    AR_CHAR *strInitOccCalibFname;
    AR_CHAR *strInitKbCalibFname;
    IR_PARAM_CTX_S *pstIrParamCtx;
    AR_BOOL bUseRawTempBuf;
    AR_BOOL bUseThrmlTempBuf;
    AR_BOOL bAutoCalib;
    AR_U32 u32AutoCalibIntervalMs;
    IR_SNS_CALIB_CFG_S stIrSnsCalibCfg;
    IR_OCC_CALIB_CFG_S stIrOccCalibCfg;
    IR_B_CALIB_CFG_S stIrBCalibCfg;
    IR_KB_CALIB_CFG_S stIrKBCalibCfg;
    IR_DYNAMIC_CALIB_TYPE_E enFirstIrDynamicCalibType;
    IR_DYNAMIC_CALIB_TYPE_E enIrDynamicCalibType;

    AR_S32 (*pfnRawProcess)(IR_SAMPLE_CTX_S *pstIrSampleCtx, VIDEO_FRAME_INFO_S *pstFrmInfo);
    AR_S32 (*pfnCfgIrViTopology)(IR_SAMPLE_CTX_S *pstIrSampleCtx,
        IR_VI_TOPOLOGY_TYPE_E enIrViTopoType, AR_U32 u32Width, AR_U32 u32Height);
    AR_S32 (*pfnIrCmdGetIrSnsParam)(IR_SAMPLE_CTX_S *pstIrSampleCtx);

    pthread_t irDynCalibThrd;
    AR_BOOL bIrDynCalibThrdStart;
    AR_BOOL bIrStarted;
    AR_BOOL bVlStarted;

    AR_U32 u32RawTempBufSize;
    AR_U64 u64RawTempBufOrigPhy;
    AR_VOID *pRawTempBufOrigVirt;
    VIDEO_FRAME_INFO_S stRawTempFrame;

    AR_U32 u32ThrmlTempBufSize;
    AR_U64 u64ThrmlTempBufOrigPhy;
    AR_VOID *pThrmlTempBufOrigVirt;
    VIDEO_FRAME_INFO_S stThrmlTempFrame;

    pthread_mutex_t shutterLock;
    AR_BOOL bShutterOpened;

    pthread_mutex_t irRawProcLock;
    IR_RAW_PROCESS_STATE_E enIrRawProcState;
    AR_U32 u32IrRawCapFrms;
    AR_CHAR achRawCapFname[128];
    AR_BOOL bRawCapNoDummy;
    FILE *pRawFp;
    AR_U32 u32SnsCalibFrmCnt;
    AR_CHAR achSnsParamCalibFname[128];
    AR_CHAR achOccCalibFname[128];

    struct timeval stLastIrDynamicCalibTime;

    pthread_mutex_t irThrmlProcLock;
    AR_S32 s32IrThrmlDumpFrms;
    AR_CHAR achThrmlDumpFname[128];
    AR_BOOL bThrmlDumpNoDummy;
    FILE *pThrmlFp;

    AR_S32 s32Ir3DnrDumpFrms;
    AR_CHAR ach3DnrDumpFname[128];
    AR_BOOL b3DnrDumpNoDummy;
    FILE *p3DnrFp;

    AR_S32 s32Ir2DnrDumpFrms;
    AR_CHAR ach2DnrDumpFname[128];
    AR_BOOL b2DnrDumpNoDummy;
    FILE *p2DnrFp;

    AR_S32 s32IrGtmDumpFrms;
    AR_CHAR achGtmDumpFname[128];
    AR_BOOL bGtmDumpNoDummy;
    FILE *pGtmFp;

    pthread_mutex_t irIsp1ProcLock;
    AR_S32 s32IrIsp1DumpFrms;
    AR_CHAR achIsp1DumpFname[128];
    AR_BOOL bIsp1DumpNoDummy;
    FILE *pIsp1Fp;

    pthread_mutex_t irIsp2ProcLock;
    AR_S32 s32IrIsp2DumpFrms;
    AR_CHAR achIsp2DumpFname[128];
    AR_BOOL bIsp2DumpNoDummy;
    FILE *pIsp2Fp;

    pthread_mutex_t frmCntLock;
    AR_U32 u32RawFrmCnt;
    AR_U32 u32ThrmlFrmCnt;
    AR_U32 u32ThrmlDeqFrmCnt;
    AR_U32 u32Isp1FrmCnt;
    AR_U32 u32Isp1DeqFrmCnt;
    AR_U32 u32Isp2FrmCnt;
    AR_U32 u32Isp2DeqFrmCnt;

    AR_U32 u32Ir3DnrTempBufSize;
    AR_U64 u64Ir3DnrTempBufOrigPhy;
    AR_VOID *pIr3DnrTempBufOrigVirt;
    VIDEO_FRAME_INFO_S stIr3DnrTempFrame;

    AR_U32 u32Ir2DnrTempBufSize;
    AR_U64 u64Ir2DnrTempBufOrigPhy;
    AR_VOID *pIr2DnrTempBufOrigVirt;
    VIDEO_FRAME_INFO_S stIr2DnrTempFrame;

    pthread_t irCalibThrd;
    AR_CHAR achIrSnsCalibFname[128];
    AR_CHAR achIrOccCalibFname[128];

    AR_BOOL bIrDynamicCalibBusy;
    AR_BOOL bForceIrDynamicCalib;

    volatile AR_BOOL bIrSuspend;

    AR_FLOAT f32GainAlpha;
};


#endif // _IR_SAMPLE_CTX_H_