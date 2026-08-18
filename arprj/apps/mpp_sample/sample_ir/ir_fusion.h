#ifndef _IR_FUSION_H_
#define _IR_FUSION_H_

#include <pthread.h>
#include "ar_inf_alg.h"
#include "ar_common.h"
#include "ir_sample_ctx.h"

typedef int (*FusionOutHandleFunc)(VIDEO_FRAME_INFO_S *pstFrameInfo,AR_VOID *pPrivData);

typedef struct {
    INF_ALGO_OBJECT_S *pstInfAlgObject;
    VI_PIPE ViPipe_vis;
    VI_CHN  ViChn_vis;
    VPSS_GRP VpssGrp_ir;
    VPSS_CHN VpssChn_ir;
    pthread_t fusion_thread;
    AR_BOOL bStart;
    AR_S32 (*pfnFusionOutHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_U64   u64ColormapTmpBufPhyAddr;
    AR_VOID* pColormapTmpBufVirAddr;
    AR_VOID* pColormapTmpBufVirAddr_u;
    AR_VOID* pColormapTmpBufVirAddr_v;
    AR_U32   u32ColormapTmpBufLen;
} IR_FUSION_S;

AR_BOOL EnableVisibleLightViChn1(IR_VI_GROUP_CFG_S *pstIrViGrpCfg);
AR_S32 DisableVisibleLightViChn1(IR_VI_GROUP_CFG_S *pstIrViGrpCfg);
AR_S32 StartIrVpss(IR_VI_GROUP_CFG_S *pstIrViGrpCfg);
AR_S32 StopIrVpss();
AR_S32 SendIrFrameToVpss(VIDEO_FRAME_INFO_S *pstIrFrame);
AR_S32 ColorMapHandle(VIDEO_FRAME_INFO_S *pstIrFrame);
AR_S32 StartIrFusion(IR_SAMPLE_CTX_S *pstIrSampleCtx, FusionOutHandleFunc outHanle);
AR_S32 StopIrFusion();

AR_S32 SetColormapIndex(int index);
AR_S32 SetFusionMode(int mode);
AR_S32 SetFusionParam(int alpha1, int alpha2, int weight, int color);
AR_S32 SetFusionCalibPoint(int w, int h, POINT_S *pSrcPt, POINT_S *pDstPt);
AR_S32 SetFusionCalibEnable(int enable);


INF_ALGO_OBJECT_S* GetInfObj();


#endif // _IR_FUSION_H_

