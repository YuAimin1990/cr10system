#include "hal_vin_log.h"
#include "ar_buffer.h"
#include "mpi_vi.h"
#include "sample_comm.h"
#include "ir_vi_group.h"


#define DEFAULT_THERMAL_CHN_ID          4


extern SAMPLE_SNS_TYPE_E g_enSnsType[];


static AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(IR_VI_GROUP_S *pstIrViGroup, AR_S32 mode)
{
    VI_DEV_PROP_S Prop = {0};

    pthread_mutex_lock(&pstIrViGroup->vinDevLock);

    if (pstIrViGroup->bVinDevOpened) {
        goto EXIT;
    }

    if (mode == 0) {
        Prop.cam_mode = VIN_CAMERA_NORMAL;
    } else if (mode == 1) {
        Prop.cam_mode = VIN_CAMERA_OFFLINE;
    } else if (mode == 2) {
        Prop.cam_mode = VIN_CMAERA_MULTI_MODE;
    } else {
        Prop.cam_mode = VIN_CAMERA_OFFLINE;
    }

    //cfg the fre
    Prop.mipi_fre_mod = 1;
    Prop.mipi_fre_hz = 100000000;
    Prop.hdr_fre_mod = 1;
    Prop.hdr_fre_hz = 300000000;
    Prop.vif_fre_mod = 1;
    Prop.vif_fre_hz = 300000000;
    Prop.isp_fre_mod = 1;
    Prop.isp_fre_hz = 300000000;

    AR_MPI_VIN_OpenDev(&Prop);
    pstIrViGroup->bVinDevOpened = AR_TRUE;

EXIT:
    pthread_mutex_unlock(&pstIrViGroup->vinDevLock);
    return;
}


IR_VI_GROUP_S *CreateIrViGroup(IR_VI_GROUP_CFG_S *pstCfg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = NULL;

    if (pstCfg == NULL) {
        ar_err("pstCfg is null");
        return NULL;
    }

    if (pstCfg->s32VisibleLightIdx >= IR_MAX_VI_CNT || pstCfg->s32IrRawIdx >= IR_MAX_VI_CNT
        || pstCfg->s32IrThrmlIdx >= IR_MAX_VI_CNT || pstCfg->s32IrIsp1Idx >= IR_MAX_VI_CNT
        || pstCfg->s32IrIsp2Idx >= IR_MAX_VI_CNT) {
        ar_err("vi index[%d %d %d %d %d] is invalid", pstCfg->s32VisibleLightIdx,
            pstCfg->s32IrRawIdx, pstCfg->s32IrThrmlIdx, pstCfg->s32IrIsp1Idx, pstCfg->s32IrIsp2Idx);
        return NULL;
    }

    if (pstCfg->s32IrIsp1Idx < 0) {
        if (pstCfg->s32IrIsp2Idx >= 0) {
            ar_err("not support that ir_isp2 open but ir_isp1 not open");
            return NULL;
        }
    }

    pstIrViGroup = malloc(sizeof(IR_VI_GROUP_S));
    if (pstIrViGroup == NULL) {
        ar_err("allocate IR_VI_GROUP_S failed");
        return NULL;
    }

    memset(pstIrViGroup, 0, sizeof(IR_VI_GROUP_S));

    s32Ret = pthread_mutex_init(&pstIrViGroup->irLock, NULL);
    if (s32Ret) {
        ar_err("initialize ir lock failed");
        goto FAIL1;
    }

    s32Ret = pthread_mutex_init(&pstIrViGroup->vlLock, NULL);
    if (s32Ret) {
        ar_err("initialize visible light lock failed");
        goto FAIL2;
    }

    s32Ret = pthread_mutex_init(&pstIrViGroup->vinDevLock, NULL);
    if (s32Ret) {
        ar_err("initialize vin dev lock failed");
        goto FAIL3;
    }

    if (pstCfg != NULL) {
        pstIrViGroup->stIrViGroupCfg = *pstCfg;
    }

    return pstIrViGroup;

FAIL3:
    pthread_mutex_destroy(&pstIrViGroup->vlLock);
FAIL2:
    pthread_mutex_destroy(&pstIrViGroup->irLock);
FAIL1:
    free(pstIrViGroup);
    return NULL;
}

AR_VOID DestroyIrViGroup(IR_VI_GROUP_S *pstIrViGroup)
{
    if (pstIrViGroup != NULL) {
        pthread_mutex_destroy(&pstIrViGroup->vinDevLock);
        pthread_mutex_destroy(&pstIrViGroup->vlLock);
        pthread_mutex_destroy(&pstIrViGroup->irLock);
        free(pstIrViGroup);
    }
}
#if 0
static AR_VOID SetIrThreadHighPriority(IR_VI_GROUP_S *pstIrViGroup)
{
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_BOOL bHighPriority = AR_FALSE;
    int policy = 0;
    struct sched_param sp;

    if (pstCfg->s32IrThrmlIdx >= 0) {
        if (pstCfg->aenSnsType[pstCfg->s32IrThrmlIdx] == ISP_VIN_IR_THERMAL_OFFLINE) {
            bHighPriority = AR_TRUE;
        }
    } else if (pstCfg->s32IrIsp1Idx >= 0) {
        if (pstCfg->aenSnsType[pstCfg->s32IrIsp1Idx] == ISP_VIN1_IR_OFFLINE
            || pstCfg->aenSnsType[pstCfg->s32IrIsp1Idx] == ISP_VIN1_IR_OFFLINE_WITH_THERMAL) {
            bHighPriority = AR_TRUE;
        }
    } else if (pstCfg->s32IrIsp2Idx >= 0) {
        if (pstCfg->aenSnsType[pstCfg->s32IrIsp2Idx] == ISP_VIN2_IR_OFFLINE) {
            bHighPriority = AR_TRUE;
        }
    }

    if (bHighPriority) {
        memset(&sp, 0, sizeof(sp));
        pthread_getschedparam(pthread_self(), &policy, &sp);
        ar_always("ir thread: policy[%d] priority[%d] RR_min_max[%d %d]",
            policy, sp.sched_priority, sched_get_priority_min(SCHED_RR),
            sched_get_priority_max(SCHED_RR));

        policy = SCHED_RR;
        memset(&sp, 0, sizeof(sp));
        sp.sched_priority = sched_get_priority_min(SCHED_RR);
        pthread_setschedparam(pthread_self(), policy, &sp);
    }
}
#endif

static AR_VOID *IrRawOutThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32IrRawIdx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};
    VIDEO_FRAME_INFO_S stEnqFrameInfo = {0};

    ar_always("ir raw out thread start: pipe[%d] chn[%d] size[%u %u] fmt[%d]",
        pstCfg->aViPipe[idx], pstCfg->aViChn[idx], pstCfg->astOutSize[idx].u32Width,
        pstCfg->astOutSize[idx].u32Height, pstCfg->aenOutPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_raw_out");

    while (pstIrViGroup->irRawOutStart) {
        s32Ret = AR_MPI_VI_GetChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d]: get ir raw output frame failed",
                pstCfg->aViPipe[idx], pstCfg->aViChn[idx]);
            continue;
        }

        if (pstCfg->pfnIrRawOutHandle != NULL) {
            s32Ret = pstCfg->pfnIrRawOutHandle(&stFrameInfo, pstCfg->irRawOutPriv);
            if (s32Ret) {
                AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
                continue;
            }
        }

        if (pstIrViGroup->thrmlChn >= 0) {
            stEnqFrameInfo = stFrameInfo;
            stEnqFrameInfo.stVFrame.u32Width = pstIrViGroup->stThrmlInSize.u32Width;
            stEnqFrameInfo.stVFrame.u32Height = pstIrViGroup->stThrmlInSize.u32Height;
            stEnqFrameInfo.stVFrame.enPixelFormat = pstIrViGroup->enThrmlInFmt;

            s32Ret = AR_MPI_VI_QPipeRaw(pstIrViGroup->thrmlPipe, &stEnqFrameInfo);
            if (s32Ret) {
                ar_err("pipe[%d]: enqueue raw failed", pstIrViGroup->thrmlPipe);
                AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
            }
        } else {
            AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
        }
    }

    ar_always("ir raw out thread end");
    return NULL;
}

static AR_VOID *IrThrmlDeqThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32IrThrmlIdx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};

    ar_always("ir thermal deq thread start: pipe[%d] size[%u %u] fmt[%d]", pstCfg->aViPipe[idx],
        pstCfg->astInSize[idx].u32Width, pstCfg->astInSize[idx].u32Height, pstCfg->aenInPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_thermal_deq");

    while (pstIrViGroup->irThrmlDeqStart) {
        s32Ret = AR_MPI_VI_DqPipeRaw(pstCfg->aViPipe[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d]: ir thermal deq raw failed", pstCfg->aViPipe[idx]);
            continue;
        }

        if (pstCfg->pfnIrThrmlDeqHandle != NULL) {
            pstCfg->pfnIrThrmlDeqHandle(&stFrameInfo, pstCfg->irThrmlDeqPriv);
        }

        if (pstCfg->s32IrRawIdx >= 0) {
            AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[pstCfg->s32IrRawIdx],
                pstCfg->aViChn[pstCfg->s32IrRawIdx], &stFrameInfo);
        }
    }

    ar_always("ir isp1 in deq thread end");
    return NULL;
}

static AR_VOID *IrThrmlOutThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};
    VIDEO_FRAME_INFO_S stEnqFrameInfo = {0};

    ar_always("ir thermal out thread start: pipe[%d] chn[%d] size[%u %u] fmt[%d]",
        pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn, pstIrViGroup->stThrmlOutSize.u32Width,
        pstIrViGroup->stThrmlOutSize.u32Height, pstIrViGroup->enThrmlOutFmt);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_thermal_out");

    while (pstIrViGroup->irThrmlOutStart) {
        s32Ret = AR_MPI_VI_GetChnFrame(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn, &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d]: get ir thermal output frame failed",
                pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn);
            continue;
        }

        if (pstCfg->pfnIrThrmlOutHandle != NULL) {
            s32Ret = pstCfg->pfnIrThrmlOutHandle(&stFrameInfo, pstCfg->irThrmlOutPriv);
            if (s32Ret) {
                AR_MPI_VI_ReleaseChnFrame(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn, &stFrameInfo);
                continue;
            }
        }

        if (pstCfg->s32IrThrmlIdx >= 0 && pstCfg->s32IrIsp1Idx >= 0) {
            stEnqFrameInfo = stFrameInfo;
            stEnqFrameInfo.stVFrame.u32Width = pstCfg->astInSize[pstCfg->s32IrIsp1Idx].u32Width;
            stEnqFrameInfo.stVFrame.u32Height = pstCfg->astInSize[pstCfg->s32IrIsp1Idx].u32Height;
            stEnqFrameInfo.stVFrame.enPixelFormat = pstCfg->aenInPixFmt[pstCfg->s32IrIsp1Idx];

            s32Ret = AR_MPI_VI_QPipeRaw(pstCfg->aViPipe[pstCfg->s32IrIsp1Idx], &stEnqFrameInfo);
            if (s32Ret) {
                ar_err("pipe[%d]: enqueue thermal image failed", pstCfg->aViPipe[pstCfg->s32IrIsp1Idx]);
                AR_MPI_VI_ReleaseChnFrame(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn, &stFrameInfo);
            }
        } else {
            AR_MPI_VI_ReleaseChnFrame(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn, &stFrameInfo);
        }
    }

    ar_always("ir thermal out thread end");
    return NULL;
}

static AR_VOID *IrIsp1InDeqThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32IrIsp1Idx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};

    ar_always("ir isp1 in deq thread start: pipe[%d] size[%u %u] fmt[%d]", pstCfg->aViPipe[idx],
        pstCfg->astInSize[idx].u32Width, pstCfg->astInSize[idx].u32Height, pstCfg->aenInPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_isp1_deq");

    while (pstIrViGroup->irIsp1InDeqStart) {
        s32Ret = AR_MPI_VI_DqPipeRaw(pstCfg->aViPipe[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d]: ir isp1 deq raw failed", pstCfg->aViPipe[idx]);
            continue;
        }

        if (pstCfg->pfnIrIsp1DeqHandle != NULL) {
            pstCfg->pfnIrIsp1DeqHandle(&stFrameInfo, pstCfg->irIsp1DeqPriv);
        }

        if (pstCfg->s32IrThrmlIdx >= 0) {
            AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[pstCfg->s32IrThrmlIdx],
                pstCfg->aViChn[pstCfg->s32IrThrmlIdx], &stFrameInfo);
        } else if (pstCfg->s32IrRawIdx >= 0) {
            AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[pstCfg->s32IrRawIdx],
                pstCfg->aViChn[pstCfg->s32IrRawIdx], &stFrameInfo);
        }
    }

    ar_always("ir isp1 in deq thread end");
    return NULL;
}

static AR_VOID *IrIsp1OutThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32IrIsp1Idx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};
    VIDEO_FRAME_INFO_S stEnqFrameInfo = {0};

    ar_always("ir isp1 out thread start: pipe[%d] chn[%d] size[%u %u] fmt[%d]", pstCfg->aViPipe[idx],
        pstCfg->aViChn[idx], pstCfg->astOutSize[idx].u32Width, pstCfg->astOutSize[idx].u32Height,
        pstCfg->aenOutPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_isp1_out");

    while (pstIrViGroup->irIsp1OutStart) {
        s32Ret = AR_MPI_VI_GetChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d]: get ir isp1 output frame failed",
                pstCfg->aViPipe[idx], pstCfg->aViChn[idx]);
            continue;
        }

        if (pstCfg->pfnIrIsp1OutHandle != NULL) {
            s32Ret = pstCfg->pfnIrIsp1OutHandle(&stFrameInfo, pstCfg->irIsp1OutPriv);
            if (s32Ret) {
                AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
                continue;
            }
        }

        if (pstCfg->s32IrIsp2Idx >= 0) {
            stEnqFrameInfo = stFrameInfo;
            stEnqFrameInfo.stVFrame.u32Width = pstCfg->astInSize[pstCfg->s32IrIsp2Idx].u32Width;
            stEnqFrameInfo.stVFrame.u32Height = pstCfg->astInSize[pstCfg->s32IrIsp2Idx].u32Height;
            stEnqFrameInfo.stVFrame.enPixelFormat = pstCfg->aenInPixFmt[pstCfg->s32IrIsp2Idx];

            s32Ret = AR_MPI_VI_QPipeRaw(pstCfg->aViPipe[pstCfg->s32IrIsp2Idx], &stEnqFrameInfo);
            if (s32Ret) {
                ar_err("pipe[%d]: ir isp2 enq raw failed", pstCfg->aViPipe[pstCfg->s32IrIsp2Idx]);
                AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
            }
        } else {
            AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
        }
    }

    ar_always("ir isp1 in out thread end");
    return NULL;
}

static AR_VOID *IrIsp2InDeqThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32IrIsp2Idx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};

    ar_always("ir isp2 in deq thread start: pipe[%d] size[%u %u] fmt[%d]", pstCfg->aViPipe[idx],
        pstCfg->astInSize[idx].u32Width, pstCfg->astInSize[idx].u32Height, pstCfg->aenInPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_isp2_deq");

    while (pstIrViGroup->irIsp2InDeqStart) {
        s32Ret = AR_MPI_VI_DqPipeRaw(pstCfg->aViPipe[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d]: ir isp2 deq raw failed", pstCfg->aViPipe[idx]);
            continue;
        }

        if (pstCfg->pfnIrIsp2DeqHandle != NULL) {
            pstCfg->pfnIrIsp2DeqHandle(&stFrameInfo, pstCfg->irIsp2DeqPriv);
        }

        AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[pstCfg->s32IrIsp1Idx],
            pstCfg->aViChn[pstCfg->s32IrIsp1Idx], &stFrameInfo);
    }

    ar_always("ir isp2 in deq thread end");
    return NULL;
}

static AR_VOID *IrIsp2OutThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32IrIsp2Idx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};

    ar_always("ir isp2 out thread start: pipe[%d] chn[%d] size[%u %u] fmt[%d]", pstCfg->aViPipe[idx],
        pstCfg->aViChn[idx], pstCfg->astOutSize[idx].u32Width, pstCfg->astOutSize[idx].u32Height,
        pstCfg->aenOutPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "ir_isp2_out");

    while (pstIrViGroup->irIsp2OutStart) {
        s32Ret = AR_MPI_VI_GetChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d]: get ir isp2 output frame failed",
                pstCfg->aViPipe[idx], pstCfg->aViChn[idx]);
            continue;
        }

        if (pstCfg->pfnIrIsp2OutHandle != NULL) {
            s32Ret = pstCfg->pfnIrIsp2OutHandle(&stFrameInfo, pstCfg->irIsp2OutPriv);
        }

        AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
    }

    ar_always("ir isp2 in out thread end");
    return NULL;
}

static AR_VOID *VisibleLightOutThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_S *pstIrViGroup = (IR_VI_GROUP_S *)arg;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    AR_S32 idx = pstCfg->s32VisibleLightIdx;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};

    ar_always("visible light out thread start: pipe[%d] chn[%d] size[%u %u] fmt[%d]",
        pstCfg->aViPipe[idx], pstCfg->aViChn[idx], pstCfg->astOutSize[idx].u32Width,
        pstCfg->astOutSize[idx].u32Height, pstCfg->aenOutPixFmt[idx]);

    //SetIrThreadHighPriority(pstIrViGroup);
    pthread_setname_np(pthread_self(), "vl_out");

    while (pstIrViGroup->visibleLightOutStart) {
        s32Ret = AR_MPI_VI_GetChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo, 1000);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d]: get visible light output frame failed",
                pstCfg->aViPipe[idx], pstCfg->aViChn[idx]);
            continue;
        }

        if (pstCfg->pfnVisibleLightOutHandle != NULL) {
            s32Ret = pstCfg->pfnVisibleLightOutHandle(&stFrameInfo, pstCfg->visibleLightOutPriv);
        }

        AR_MPI_VI_ReleaseChnFrame(pstCfg->aViPipe[idx], pstCfg->aViChn[idx], &stFrameInfo);
    }

    ar_always("visible light out thread end");
    return NULL;
}

AR_VOID GetIrViGroupVbConf(IR_VI_GROUP_S *pstIrViGroup, VB_CONFIG_S *pstVbConf)
{
    VB_CONFIG_S stVbConf;
    AR_U32 u32BlkSize;
    IR_VI_GROUP_CFG_S *pstCfg = &pstIrViGroup->stIrViGroupCfg;
    VI_CHN thrmlChn = -1;
    SIZE_S stThrmlOutSize;

    if (pstCfg->s32IrThrmlIdx >= 0) {
        // If ir thermal index is valid, ir thermal will be a single pipe
        thrmlChn = pstCfg->aViChn[pstCfg->s32IrThrmlIdx];
        stThrmlOutSize = pstCfg->astOutSize[pstCfg->s32IrThrmlIdx];
    } else if (pstCfg->s32IrIsp1Idx >= 0
        && pstCfg->aenInPixFmt[pstCfg->s32IrIsp1Idx] == PIXEL_FORMAT_RGB_BAYER_14BPP) {
        // If ir thermal index is invalid, ir thermal and ir isp1 share a same pipe.
        // In this case, ir thermal output port is channel-4 by default, and isp1 output port is channel-0.
        thrmlChn = DEFAULT_THERMAL_CHN_ID;
        stThrmlOutSize = pstCfg->astOutSize[pstCfg->s32IrIsp1Idx];
    }

	/*config vb*/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

	if (pstCfg->s32IrRawIdx >= 0) {
		u32BlkSize = VI_GetRawBufferSize(pstCfg->astOutSize[pstCfg->s32IrRawIdx].u32Width,
			pstCfg->astOutSize[pstCfg->s32IrRawIdx].u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP,
			COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
		stVbConf.u32MaxPoolCnt++;
	} else if (pstCfg->s32IrThrmlIdx >= 0) {
		u32BlkSize = VI_GetRawBufferSize(pstCfg->astInSize[pstCfg->s32IrThrmlIdx].u32Width,
			pstCfg->astInSize[pstCfg->s32IrThrmlIdx].u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP,
			COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
		stVbConf.u32MaxPoolCnt++;
	} else if (pstCfg->s32IrIsp1Idx >= 0) {
		u32BlkSize = VI_GetRawBufferSize(pstCfg->astInSize[pstCfg->s32IrIsp1Idx].u32Width,
			pstCfg->astInSize[pstCfg->s32IrIsp1Idx].u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP,
			COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
		stVbConf.u32MaxPoolCnt++;
	}

	if (pstCfg->s32IrIsp1Idx >= 0) {
		u32BlkSize = COMMON_GetPicBufferSize(pstCfg->astOutSize[pstCfg->s32IrIsp1Idx].u32Width,
			pstCfg->astOutSize[pstCfg->s32IrIsp1Idx].u32Height,
			pstCfg->aenOutPixFmt[pstCfg->s32IrIsp1Idx],
			DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
		stVbConf.u32MaxPoolCnt++;

		if (pstCfg->s32IrIsp2Idx >= 0) {
			u32BlkSize = COMMON_GetPicBufferSize(pstCfg->astOutSize[pstCfg->s32IrIsp2Idx].u32Width,
				pstCfg->astOutSize[pstCfg->s32IrIsp2Idx].u32Height,
				pstCfg->aenOutPixFmt[pstCfg->s32IrIsp2Idx],
				DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
			stVbConf.u32MaxPoolCnt++;
		}
	}

	if (thrmlChn >= 0) {
		u32BlkSize = VI_GetRawBufferSize(stThrmlOutSize.u32Width, stThrmlOutSize.u32Height,
            PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
		stVbConf.u32MaxPoolCnt++;
	}

	if (pstCfg->s32VisibleLightIdx >= 0) {
		u32BlkSize = COMMON_GetPicBufferSize(pstCfg->astOutSize[pstCfg->s32VisibleLightIdx].u32Width,
			pstCfg->astOutSize[pstCfg->s32VisibleLightIdx].u32Height,
			pstCfg->aenOutPixFmt[pstCfg->s32VisibleLightIdx],
			DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5+5;
		stVbConf.u32MaxPoolCnt++;

		u32BlkSize = VI_GetRawBufferSize(pstCfg->astInSize[pstCfg->s32VisibleLightIdx].u32Width,
			pstCfg->astInSize[pstCfg->s32VisibleLightIdx].u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP,
			COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize  = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt	 = 5;
		stVbConf.u32MaxPoolCnt++;
	}

	for (int i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
		printf("ir vi group vb pool[%d]: blk_size[%llu] blk_cnt[%u]\n", i,
			stVbConf.astCommPool[i].u64BlkSize, stVbConf.astCommPool[i].u32BlkCnt);
	}

	*pstVbConf = stVbConf;
}

AR_S32 StartIrViGroupIrOnly(IR_VI_GROUP_S *pstIrViGroup)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstCfg = NULL;
    SAMPLE_VI_CONFIG_S stViConfig;
    AR_S32 idx[4] = {-1};
    AR_S32 i = 0;
    AR_S32 j = 0;

    if (pstIrViGroup == NULL) {
        ar_err("pstIrViGroup is null");
        return -1;
    }

    pthread_mutex_lock(&pstIrViGroup->irLock);

    if (pstIrViGroup->bIrStarted) {
        ar_always("IR has been started");
        goto EXIT;
    }

    pstCfg = &pstIrViGroup->stIrViGroupCfg;

    pstIrViGroup->thrmlDev = -1;
    pstIrViGroup->thrmlPipe = -1;
    pstIrViGroup->thrmlChn = -1;

    if (pstCfg->s32IrThrmlIdx >= 0) {
        // If ir thermal index is valid, ir thermal will be a single pipe
        pstIrViGroup->thrmlDev = pstCfg->aViDev[pstCfg->s32IrThrmlIdx];
        pstIrViGroup->thrmlPipe = pstCfg->aViPipe[pstCfg->s32IrThrmlIdx];
        pstIrViGroup->thrmlChn = pstCfg->aViChn[pstCfg->s32IrThrmlIdx];
        pstIrViGroup->enThrmlInFmt = pstCfg->aenInPixFmt[pstCfg->s32IrThrmlIdx];
        pstIrViGroup->stThrmlInSize = pstCfg->astInSize[pstCfg->s32IrThrmlIdx];
        pstIrViGroup->enThrmlOutFmt = pstCfg->aenOutPixFmt[pstCfg->s32IrThrmlIdx];
        pstIrViGroup->stThrmlOutSize = pstCfg->astOutSize[pstCfg->s32IrThrmlIdx];
    } else if (pstCfg->s32IrIsp1Idx >= 0
        && pstCfg->aenInPixFmt[pstCfg->s32IrIsp1Idx] == PIXEL_FORMAT_RGB_BAYER_14BPP) {
        // If ir thermal index is invalid, ir thermal and ir isp1 share a same pipe.
        // In this case, ir thermal output port is channel-4 by default, and isp1 output port is channel-0.
        pstIrViGroup->thrmlDev = pstCfg->aViDev[pstCfg->s32IrIsp1Idx];
        pstIrViGroup->thrmlPipe = pstCfg->aViPipe[pstCfg->s32IrIsp1Idx];
        pstIrViGroup->thrmlChn = DEFAULT_THERMAL_CHN_ID;
        pstIrViGroup->enThrmlInFmt = pstCfg->aenInPixFmt[pstCfg->s32IrIsp1Idx];
        pstIrViGroup->stThrmlInSize = pstCfg->astInSize[pstCfg->s32IrIsp1Idx];
        pstIrViGroup->enThrmlOutFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
        pstIrViGroup->stThrmlOutSize = pstCfg->astOutSize[pstCfg->s32IrIsp1Idx];
    }

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    idx[0] = pstCfg->s32IrIsp2Idx;      // for ir application, please open isp pipe first, and open raw pipe finally.
    idx[1] = pstCfg->s32IrIsp1Idx;
    idx[2] = pstCfg->s32IrThrmlIdx;
    idx[3] = pstCfg->s32IrRawIdx;

    j = 0;
    for (i = 0; i < 4; i++) {
        if (idx[i] < 0) {
            continue;
        }

        g_enSnsType[j] = pstCfg->aenSnsType[idx[i]];
        stViConfig.as32WorkingViId[j] = j;
        stViConfig.astViInfo[j].stSnsInfo.s32SnsId = j;
        stViConfig.astViInfo[j].stSnsInfo.s32BusId = pstCfg->as32BusId[idx[i]];
        stViConfig.astViInfo[j].stSnsInfo.MipiDev = pstCfg->as32MipiDev[idx[i]];
        stViConfig.astViInfo[j].stSnsInfo.enSnsType = pstCfg->aenSnsType[idx[i]];
        stViConfig.astViInfo[j].stDevInfo.ViDev = pstCfg->aViDev[idx[i]];
        stViConfig.astViInfo[j].stDevInfo.enWDRMode = WDR_MODE_NONE;
        stViConfig.astViInfo[j].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
        stViConfig.astViInfo[j].stPipeInfo.bMultiPipe = AR_FALSE;
        stViConfig.astViInfo[j].stPipeInfo.bVcNumCfged = AR_FALSE;
        stViConfig.astViInfo[j].stPipeInfo.aPipe[0] = pstCfg->aViPipe[idx[i]];
        stViConfig.astViInfo[j].stPipeInfo.aPipe[1] = -1;
        stViConfig.astViInfo[j].stPipeInfo.aPipe[2] = -1;
        stViConfig.astViInfo[j].stPipeInfo.aPipe[3] = -1;
        stViConfig.astViInfo[j].stPipeInfo.u32Width[0] = pstCfg->astInSize[idx[i]].u32Width;
        stViConfig.astViInfo[j].stPipeInfo.u32Height[0] = pstCfg->astInSize[idx[i]].u32Height;
        stViConfig.astViInfo[j].stChnInfo.ViChn = pstCfg->aViChn[idx[i]];
        stViConfig.astViInfo[j].stChnInfo.enPixFormat = pstCfg->aenOutPixFmt[idx[i]];
        stViConfig.astViInfo[j].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
        stViConfig.astViInfo[j].stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
        stViConfig.astViInfo[j].stChnInfo.enCompressMode = COMPRESS_MODE_NONE;
        if (i == 0) {
            stViConfig.astViInfo[j].stChnInfo.u32Width = pstCfg->astOutSize[idx[i]].u32Width;
            stViConfig.astViInfo[j].stChnInfo.u32Height = pstCfg->astOutSize[idx[i]].u32Height;
        }

        stViConfig.s32WorkingViNum++;
        j++;
    }

    pstIrViGroup->stSampleViCfgIr = stViConfig;

    SAMPLE_AR_MPI_VIN_OpenDev(pstIrViGroup, 2);

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret) {
        ar_err("start vi failed.s32Ret:0x%x !", s32Ret);
        goto FAIL1;
    }

    if (pstCfg->s32IrThrmlIdx < 0 && pstCfg->s32IrIsp1Idx >= 0
        && pstCfg->aenInPixFmt[pstCfg->s32IrIsp1Idx] == PIXEL_FORMAT_RGB_BAYER_14BPP) {
        VI_CHN_ATTR_S stThrmlChnAttr;

        s32Ret = AR_MPI_VI_GetChnAttr(pstCfg->aViPipe[pstCfg->s32IrIsp1Idx],
            pstCfg->aViChn[pstCfg->s32IrIsp1Idx], &stThrmlChnAttr);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d] get channel attr failed",
                pstCfg->aViPipe[pstCfg->s32IrIsp1Idx], pstCfg->aViChn[pstCfg->s32IrIsp1Idx]);
            goto FAIL2;
        }

        stThrmlChnAttr.enPixelFormat = pstIrViGroup->enThrmlOutFmt;

        s32Ret = AR_MPI_VI_SetChnAttr(pstIrViGroup->thrmlPipe,
            pstIrViGroup->thrmlChn, &stThrmlChnAttr);
        if (s32Ret) {
            ar_err("pipe[%d] chn[%d] set channel attr failed",
                pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn);
            goto FAIL2;
        }

        s32Ret = AR_MPI_VI_EnableChn(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn);
        if (s32Ret)
        {
            ar_err("pipe[%d] chn[%d] enable channel failed",
                pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn);
            goto FAIL2;
        }
    }

    if (pstCfg->s32IrIsp2Idx >= 0) {
        pstIrViGroup->irIsp2OutStart = AR_TRUE;
        s32Ret = pthread_create(&pstIrViGroup->irIsp2OutThrd, NULL, IrIsp2OutThread, pstIrViGroup);
        if (s32Ret) {
            ar_err("create isp2 out thread failed");
            pstIrViGroup->irIsp2OutStart = AR_FALSE;
            goto FAIL3;
        }

        if (pstCfg->s32IrIsp1Idx >= 0) {
            pstIrViGroup->irIsp2InDeqStart = AR_TRUE;
            s32Ret = pthread_create(&pstIrViGroup->irIsp2InDeqThrd, NULL, IrIsp2InDeqThread, pstIrViGroup);
            if (s32Ret) {
                ar_err("create isp2 in deq thread failed");
                pstIrViGroup->irIsp2InDeqStart = AR_FALSE;
                goto FAIL3;
            }
        }
    }

    if (pstCfg->s32IrIsp1Idx >= 0) {
        pstIrViGroup->irIsp1OutStart = AR_TRUE;
        s32Ret = pthread_create(&pstIrViGroup->irIsp1OutThrd, NULL, IrIsp1OutThread, pstIrViGroup);
        if (s32Ret) {
            ar_err("create isp1 out thread failed");
            pstIrViGroup->irIsp1OutStart = AR_FALSE;
            goto FAIL3;
        }

        if (pstCfg->s32IrThrmlIdx >= 0 || pstCfg->s32IrRawIdx >= 0) {
            pstIrViGroup->irIsp1InDeqStart = AR_TRUE;
            s32Ret = pthread_create(&pstIrViGroup->irIsp1InDeqThrd, NULL, IrIsp1InDeqThread, pstIrViGroup);
            if (s32Ret) {
                ar_err("create isp1 in deq thread failed");
                pstIrViGroup->irIsp1InDeqStart = AR_FALSE;
                goto FAIL3;
            }
        }
    }

    if (pstIrViGroup->thrmlChn >= 0) {
        pstIrViGroup->irThrmlOutStart = AR_TRUE;
        s32Ret = pthread_create(&pstIrViGroup->irThrmlOutThrd, NULL, IrThrmlOutThread, pstIrViGroup);
        if (s32Ret) {
            ar_err("create thermal out thread failed");
            pstIrViGroup->irThrmlOutStart = AR_FALSE;
            goto FAIL3;
        }

        if (pstCfg->s32IrThrmlIdx >= 0 && pstCfg->s32IrRawIdx >= 0) {
            pstIrViGroup->irThrmlDeqStart = AR_TRUE;
            s32Ret = pthread_create(&pstIrViGroup->irThrmlDeqThrd, NULL, IrThrmlDeqThread, pstIrViGroup);
            if (s32Ret) {
                ar_err("create thermal deq thread failed");
                pstIrViGroup->irThrmlDeqStart = AR_FALSE;
                goto FAIL3;
            }
        }
    }

    if (pstCfg->s32IrRawIdx >= 0) {
        pstIrViGroup->irRawOutStart = AR_TRUE;
        s32Ret = pthread_create(&pstIrViGroup->irRawOutThrd, NULL, IrRawOutThread, pstIrViGroup);
        if (s32Ret) {
            ar_err("create ir raw out thread failed");
            pstIrViGroup->irRawOutStart = AR_FALSE;
            goto FAIL3;
        }
    }

    pstIrViGroup->bIrStarted = AR_TRUE;

EXIT:
    pthread_mutex_unlock(&pstIrViGroup->irLock);
    return 0;

FAIL3:
    if (pstIrViGroup->irIsp2OutStart) {
        pstIrViGroup->irIsp2OutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp2OutThrd, NULL);
    }

    if (pstIrViGroup->irIsp1OutStart) {
        pstIrViGroup->irIsp1OutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp1OutThrd, NULL);
    }

    if (pstIrViGroup->irThrmlOutStart) {
        pstIrViGroup->irThrmlOutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irThrmlOutThrd, NULL);
    }

    if (pstIrViGroup->irRawOutStart) {
        pstIrViGroup->irRawOutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irRawOutThrd, NULL);
    }

    if (pstIrViGroup->irIsp2InDeqStart) {
        pstIrViGroup->irIsp2InDeqStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp2InDeqThrd, NULL);
    }

    if (pstIrViGroup->irIsp1InDeqStart) {
        pstIrViGroup->irIsp1InDeqStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp1InDeqThrd, NULL);
    }

    if (pstIrViGroup->irThrmlDeqStart) {
        pstIrViGroup->irThrmlDeqStart = AR_FALSE;
        pthread_join(pstIrViGroup->irThrmlDeqThrd, NULL);
    }

    if (pstCfg->s32IrThrmlIdx < 0 && pstCfg->s32IrIsp1Idx >= 0) {
        AR_MPI_VI_DisableChn(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn);
    }

FAIL2:
    SAMPLE_COMM_VI_StopVi(&stViConfig);

FAIL1:
    pthread_mutex_unlock(&pstIrViGroup->irLock);
    return s32Ret;
}

AR_S32 StopIrViGroupIrOnly(IR_VI_GROUP_S *pstIrViGroup)
{
    IR_VI_GROUP_CFG_S *pstCfg = NULL;

    if (pstIrViGroup == NULL) {
        ar_err("pstIrViGroup is null");
        return -1;
    }

    pthread_mutex_lock(&pstIrViGroup->irLock);

    if (!pstIrViGroup->bIrStarted) {
        ar_always("IR has been stopped");
        goto EXIT;
    }

    pstCfg = &pstIrViGroup->stIrViGroupCfg;

    if (pstIrViGroup->irIsp2OutStart) {
        pstIrViGroup->irIsp2OutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp2OutThrd, NULL);
    }

    if (pstIrViGroup->irIsp1OutStart) {
        pstIrViGroup->irIsp1OutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp1OutThrd, NULL);
    }

    if (pstIrViGroup->irThrmlOutStart) {
        pstIrViGroup->irThrmlOutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irThrmlOutThrd, NULL);
    }

    if (pstIrViGroup->irRawOutStart) {
        pstIrViGroup->irRawOutStart = AR_FALSE;
        pthread_join(pstIrViGroup->irRawOutThrd, NULL);
    }

    if (pstIrViGroup->irIsp2InDeqStart) {
        pstIrViGroup->irIsp2InDeqStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp2InDeqThrd, NULL);
    }

    if (pstIrViGroup->irIsp1InDeqStart) {
        pstIrViGroup->irIsp1InDeqStart = AR_FALSE;
        pthread_join(pstIrViGroup->irIsp1InDeqThrd, NULL);
    }

    if (pstIrViGroup->irThrmlDeqStart) {
        pstIrViGroup->irThrmlDeqStart = AR_FALSE;
        pthread_join(pstIrViGroup->irThrmlDeqThrd, NULL);
    }

    if (pstIrViGroup->thrmlChn >= 0 && pstCfg->s32IrThrmlIdx < 0) {
        AR_MPI_VI_DisableChn(pstIrViGroup->thrmlPipe, pstIrViGroup->thrmlChn);
    }

    SAMPLE_COMM_VI_StopVi(&pstIrViGroup->stSampleViCfgIr);
    pstIrViGroup->bIrStarted = AR_FALSE;

EXIT:
    pthread_mutex_unlock(&pstIrViGroup->irLock);
    return 0;
}

AR_S32 StartIrViGroupVlOnly(IR_VI_GROUP_S *pstIrViGroup)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstCfg = NULL;
    SAMPLE_VI_CONFIG_S stViConfig;

    if (pstIrViGroup == NULL) {
        ar_err("pstIrViGroup is null");
        return -1;
    }

    pthread_mutex_lock(&pstIrViGroup->vlLock);

    if (pstIrViGroup->bVlStarted) {
        ar_always("Visible light has been started");
        goto EXIT;
    }

    pstCfg = &pstIrViGroup->stIrViGroupCfg;

    if (pstCfg->s32VisibleLightIdx < 0) {
        ar_always("visible light is invalid");
        goto EXIT;
    }

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    g_enSnsType[5] = pstCfg->aenSnsType[pstCfg->s32VisibleLightIdx];
    stViConfig.as32WorkingViId[0] = 0;
    stViConfig.astViInfo[0].stSnsInfo.s32SnsId = 5;
    stViConfig.astViInfo[0].stSnsInfo.s32BusId = pstCfg->as32BusId[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stSnsInfo.MipiDev = pstCfg->as32MipiDev[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stSnsInfo.enSnsType = pstCfg->aenSnsType[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stDevInfo.ViDev = pstCfg->aViDev[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stDevInfo.enWDRMode = WDR_MODE_NONE;
    stViConfig.astViInfo[0].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[0].stPipeInfo.bMultiPipe = AR_FALSE;
    stViConfig.astViInfo[0].stPipeInfo.bVcNumCfged = AR_FALSE;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = pstCfg->aViPipe[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stPipeInfo.aPipe[1] = -1;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[2] = -1;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[3] = -1;
    stViConfig.astViInfo[0].stPipeInfo.u32Width[0] = pstCfg->astInSize[pstCfg->s32VisibleLightIdx].u32Width;
    stViConfig.astViInfo[0].stPipeInfo.u32Height[0] = pstCfg->astInSize[pstCfg->s32VisibleLightIdx].u32Height;
    stViConfig.astViInfo[0].stChnInfo.ViChn = pstCfg->aViChn[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stChnInfo.enPixFormat = pstCfg->aenOutPixFmt[pstCfg->s32VisibleLightIdx];
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stViConfig.astViInfo[0].stChnInfo.enCompressMode = COMPRESS_MODE_NONE;
    if(pstCfg->aenSnsType[pstCfg->s32VisibleLightIdx] == SONY_IMX290_MIPI_2M_30FPS_12BIT) {
        stViConfig.astViInfo[0].stPipeInfo.u32BitWidth[0] = DATA_BITWIDTH_12;
    }
    if(pstCfg->aenSnsType[pstCfg->s32VisibleLightIdx] == SC_SC2210_1080P) {
        stViConfig.astViInfo[0].stSnsInfo.settle = 3;
    }

    stViConfig.s32WorkingViNum = 1;

    pstIrViGroup->stSampleViCfgVl = stViConfig;

    SAMPLE_AR_MPI_VIN_OpenDev(pstIrViGroup, 2);

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret) {
        ar_err("start vi failed.s32Ret:0x%x !", s32Ret);
        goto FAIL1;
    }

    if (pstCfg->s32VisibleLightIdx >= 0) {
        pstIrViGroup->visibleLightOutStart = AR_TRUE;
        s32Ret = pthread_create(&pstIrViGroup->visibleLightOutThrd, NULL,VisibleLightOutThread, pstIrViGroup);
        if (s32Ret) {
            ar_err("create visible light out thread failed");
            pstIrViGroup->visibleLightOutStart = AR_FALSE;
            goto FAIL2;
        }
    }

    pstIrViGroup->bVlStarted = AR_TRUE;

EXIT:
    pthread_mutex_unlock(&pstIrViGroup->vlLock);
    return 0;

FAIL2:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
FAIL1:
    pthread_mutex_unlock(&pstIrViGroup->vlLock);
    return s32Ret;
}

AR_S32 StopIrViGroupVlOnly(IR_VI_GROUP_S *pstIrViGroup)
{
    if (pstIrViGroup == NULL) {
        ar_err("pstIrViGroup is null");
        return -1;
    }

    pthread_mutex_lock(&pstIrViGroup->vlLock);

    if (!pstIrViGroup->bVlStarted) {
        ar_always("visible light has been stopped");
        goto EXIT;
    }

    if (pstIrViGroup->visibleLightOutStart) {
        pstIrViGroup->visibleLightOutStart = AR_FALSE;
        pthread_join(pstIrViGroup->visibleLightOutThrd, NULL);
    }

    SAMPLE_COMM_VI_StopVi(&pstIrViGroup->stSampleViCfgVl);
    pstIrViGroup->bVlStarted = AR_FALSE;

EXIT:
    pthread_mutex_unlock(&pstIrViGroup->vlLock);
    return 0;
}

AR_S32 StartIrViGroup(IR_VI_GROUP_S *pstIrViGroup)
{
    AR_S32 s32Ret = 0;

    if (pstIrViGroup == NULL) {
        ar_err("pstIrViGroup is null");
        return -1;
    }

    s32Ret = StartIrViGroupIrOnly(pstIrViGroup);
    if (s32Ret) {
        ar_err("start ir failed");
        return s32Ret;
    }

    s32Ret = StartIrViGroupVlOnly(pstIrViGroup);
    if (s32Ret) {
        ar_err("start visible light failed");
        StopIrViGroupIrOnly(pstIrViGroup);
        return s32Ret;
    }

    return 0;
}

AR_S32 StopIrViGroup(IR_VI_GROUP_S *pstIrViGroup)
{
    if (pstIrViGroup == NULL) {
        ar_err("pstIrViGroup is null");
        return -1;
    }

    StopIrViGroupIrOnly(pstIrViGroup);
    StopIrViGroupVlOnly(pstIrViGroup);

    return 0;
}

