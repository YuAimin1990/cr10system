#ifndef _IR_VI_GROUP_H_
#define _IR_VI_GROUP_H_

#include <pthread.h>

#include "ar_comm_video.h"
#include "sample_comm.h"


#define IR_MAX_VI_CNT           5


typedef struct {
    VI_DEV aViDev[IR_MAX_VI_CNT];
    VI_PIPE aViPipe[IR_MAX_VI_CNT];
    VI_CHN aViChn[IR_MAX_VI_CNT];
    SAMPLE_SNS_TYPE_E aenSnsType[IR_MAX_VI_CNT];
    PIXEL_FORMAT_E aenInPixFmt[IR_MAX_VI_CNT];
    SIZE_S astInSize[IR_MAX_VI_CNT];
    PIXEL_FORMAT_E aenOutPixFmt[IR_MAX_VI_CNT];
    SIZE_S astOutSize[IR_MAX_VI_CNT];
    AR_S32 as32BusId[IR_MAX_VI_CNT];
    AR_S32 as32MipiDev[IR_MAX_VI_CNT];
    AR_S32 s32VisibleLightIdx;      // indicate the index of array aViDev/aViPipe/aViChn... ... for visible light. -1 means visible light invalid.
    AR_S32 s32IrRawIdx;             // indicate the index of array aViDev/aViPipe/aViChn... ... for ir raw. -1 means ir raw invalid.
    AR_S32 s32IrThrmlIdx;           // indicate the index of array aViDev/aViPipe/aViChn... ... for ir thermal. -1 means ir thermal invalid.
    AR_S32 s32IrIsp1Idx;            // indicate the index of array aViDev/aViPipe/aViChn... ... for ir isp1. -1 means ir isp1 invalid.
    AR_S32 s32IrIsp2Idx;            // indicate the index of array aViDev/aViPipe/aViChn... ... for ir isp2. -1 means ir isp2 invalid.

    AR_VOID *irRawOutPriv;
    AR_S32 (*pfnIrRawOutHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *irIsp1OutPriv;
    AR_S32 (*pfnIrIsp1OutHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *irIsp1DeqPriv;
    AR_S32 (*pfnIrIsp1DeqHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *irIsp2OutPriv;
    AR_S32 (*pfnIrIsp2OutHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *irIsp2DeqPriv;
    AR_S32 (*pfnIrIsp2DeqHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *irThrmlOutPriv;
    AR_S32 (*pfnIrThrmlOutHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *irThrmlDeqPriv;
    AR_S32 (*pfnIrThrmlDeqHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);

    AR_VOID *visibleLightOutPriv;
    AR_S32 (*pfnVisibleLightOutHandle)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);
} IR_VI_GROUP_CFG_S;

typedef struct {
    IR_VI_GROUP_CFG_S stIrViGroupCfg;
    SAMPLE_VI_CONFIG_S stSampleViCfgIr;
    SAMPLE_VI_CONFIG_S stSampleViCfgVl;
    AR_BOOL bIrStarted;
    AR_BOOL bVlStarted;
    AR_BOOL bVinDevOpened;
    pthread_mutex_t irLock;
    pthread_mutex_t vlLock;
    pthread_mutex_t vinDevLock;

    VI_DEV thrmlDev;
    VI_PIPE thrmlPipe;
    VI_CHN thrmlChn;
    PIXEL_FORMAT_E enThrmlInFmt;
    SIZE_S stThrmlInSize;
    PIXEL_FORMAT_E enThrmlOutFmt;
    SIZE_S stThrmlOutSize;

    volatile AR_BOOL irRawOutStart;
    pthread_t irRawOutThrd;

    volatile AR_BOOL irIsp1InDeqStart;
    pthread_t irIsp1InDeqThrd;

    volatile AR_BOOL irIsp1OutStart;
    pthread_t irIsp1OutThrd;

    volatile AR_BOOL irIsp2InDeqStart;
    pthread_t irIsp2InDeqThrd;

    volatile AR_BOOL irIsp2OutStart;
    pthread_t irIsp2OutThrd;

    volatile AR_BOOL irThrmlDeqStart;
    pthread_t irThrmlDeqThrd;

    volatile AR_BOOL irThrmlOutStart;
    pthread_t irThrmlOutThrd;

    volatile AR_BOOL visibleLightOutStart;
    pthread_t visibleLightOutThrd;
} IR_VI_GROUP_S;


IR_VI_GROUP_S *CreateIrViGroup(IR_VI_GROUP_CFG_S *pstCfg);
AR_VOID DestroyIrViGroup(IR_VI_GROUP_S *pstIrViGroup);
AR_S32 StartIrViGroupIrOnly(IR_VI_GROUP_S *pstIrViGroup);       // start ir only
AR_S32 StopIrViGroupIrOnly(IR_VI_GROUP_S *pstIrViGroup);        // stop ir only
AR_S32 StartIrViGroupVlOnly(IR_VI_GROUP_S *pstIrViGroup);       // start visible light only
AR_S32 StopIrViGroupVlOnly(IR_VI_GROUP_S *pstIrViGroup);        // stop visible light only
AR_S32 StartIrViGroup(IR_VI_GROUP_S *pstIrViGroup);             // start ir and visible light
AR_S32 StopIrViGroup(IR_VI_GROUP_S *pstIrViGroup);              // stop ir and visible light
AR_VOID GetIrViGroupVbConf(IR_VI_GROUP_S *pstIrViGroup, VB_CONFIG_S *pstVbConf);


#endif // _IR_VI_GROUP_H_

