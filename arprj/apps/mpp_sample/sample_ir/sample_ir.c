#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "mpi_vb.h"
#include "osal.h"
#include "hal_sys.h"
#include "hal_vin.h"
#include "mpi_sysctl.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "sample_ir.h"
#include "ir_vi_group.h"
#include "ir_param_ctx.h"
#include "ir_sample_ctx.h"
#include "ir_fusion.h"


#define SAMPLE_IR_FPGA      0


#define DEFAULT_IR_GAIN_ALPHA       0.01f


typedef struct
{
    AR_S32                   chan_id;
    AR_S32                      chan_fd;
    STRU_AR_HAL_VO_RECT      pos;
}STRU_SURFACE_OBJ_T;

typedef struct
{
    STRU_SURFACE_OBJ_T       surface[24];
    //dislpay_obj
    ENUM_AR_HAL_VO_DEV_ID    e_dev_id;
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_S32                      dev_fd;
    AR_S32                      layer_fd;
}STRU_DISPLAY_OBJ_T;

typedef struct
{
    int ViPipe_isp;
    int ViChn_isp;
    int VpssGrp_inf;
} STRU_YUV_PROCESS_PARAM_T;



typedef struct {
    AR_CHAR *strSnsName;
    IR_SAMPLE_CTX_S *pstIrSampleCtx;
} IR_SNS_ENTRY_S;

typedef struct {
    AR_CHAR *strSnsName;
    SAMPLE_SNS_TYPE_E enSnsType;
    AR_S32 s32BusId;
    SIZE_S stInSize;
    SIZE_S stOutSize;
} VL_SNS_ENTRY_S;


static AR_BOOL bExit = AR_FALSE;
//static STRU_DISPLAY_OBJ_T g_vo_obj = {0};

extern IR_SAMPLE_CTX_S stGst417wIrSampleCtx;
extern IR_SAMPLE_CTX_S stH3812c1sgIrSampleCtx;
extern IR_SAMPLE_CTX_S stRtd6122cIrSampleCtx;
extern IR_SAMPLE_CTX_S stRtd3172cIrSampleCtx;
extern IR_SAMPLE_CTX_S stGst412cIrSampleCtx;
extern IR_SAMPLE_CTX_S stGst612cIrSampleCtx;
extern IR_SAMPLE_CTX_S stGst212w4IrSampleCtx;
extern IR_SAMPLE_CTX_S stOfflineIrSampleCtx;

// supported ir sensor list
static IR_SNS_ENTRY_S astIrSnsList[] = {
    {"gst417w", &stGst417wIrSampleCtx},
    {"f23", &stH3812c1sgIrSampleCtx},
    {"rtd6122c", &stRtd6122cIrSampleCtx},
    {"rtd3172c", &stRtd3172cIrSampleCtx},
    {"gst412c", &stGst412cIrSampleCtx},
    {"gst612c", &stGst612cIrSampleCtx},
    {"gst212w4", &stGst212w4IrSampleCtx},
    {"offline", &stOfflineIrSampleCtx},
};

// supported visible light sensor list
static VL_SNS_ENTRY_S astVlSnsList[] = {
    // imx307
    {
        "imx307",
        SONY_IMX290_MIPI_2M_30FPS_12BIT,    // sensor type
        1,                                  // bus id
        {1920, 1080},                       // input size
        {1920, 1080},                       // output size
    },
    // os04a10
    {
        "os04a10",
        OV_OS04A10_MIPI_4M_30FPS_12BIT,     // sensor type
        1,                                  // bus id
        {2560, 1440},                       // input size
        {1920, 1080},                       // output size
    },
    //sc2210
    {
        "sc2210",
        SC_SC2210_1080P,     // sensor type
        1,                                  // bus id
        {1920, 1080},                       // input size
        {1920, 1080},                       // output size
    },
};

static volatile AR_BOOL bIrDisplayEn = AR_FALSE;
static volatile AR_BOOL bFirstIrDynamicCalib = AR_TRUE;
static volatile AR_BOOL bIrParamInited = AR_FALSE;
static volatile AR_BOOL bEnableFusion = AR_FALSE;
static volatile AR_BOOL bVoChnEn[4] = {AR_FALSE};
static MPI_SYSCTL_HANDLE sysctlHandle = NULL;

AR_S32 ar_vo_dev_init()
{
    //AR_S32 s32Ret;
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
    SIZE_S stDevSize;
    
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.enIntfType = VO_INTF_HDMI;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P30;       // It was designed that the max display performance of ARS31/AR9311 is 1080P30 or 720P60. 1080P60 may cause display underflow.
    AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr);

    /* ENABLE VO DEV */
    AR_MPI_VO_Enable(VoDev);
    
    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = 1920;
    stDevSize.u32Height = 1080;
    
    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width  = stDevSize.u32Width;
    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;
    stLayerAttr.u32DispFrmRt = 30;
    
    AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
    
    /* ENABLE VO LAYER */
    AR_MPI_VO_EnableVideoLayer(VoLayer);
    
    return 0;
}

AR_S32 ar_vo_surface_init(AR_S32 ch, STRU_AR_HAL_VO_RECT *chan_pos)
{
    VO_CHN_ATTR_S stChnAttr = {0};
    stChnAttr.bDeflicker = AR_FALSE;
    stChnAttr.u32Priority = 0;
    stChnAttr.stRect.s32X = chan_pos->x;
    stChnAttr.stRect.s32Y = chan_pos->y;
    stChnAttr.stRect.u32Height = chan_pos->h;
    stChnAttr.stRect.u32Width = chan_pos->w;
    AR_MPI_VO_SetChnAttr(0, ch, &stChnAttr);
    AR_MPI_VO_EnableChn(0, ch);
    bVoChnEn[ch] = AR_TRUE;
    return 0;
}

AR_S32 close_display_surface(AR_S32 ch)
{
    AR_S32 VoLayer = 0;
    AR_MPI_VO_DisableChn(VoLayer, ch);
    bVoChnEn[ch] = AR_FALSE;
   return 0;
}

AR_S32 close_all_display_surface()
{
    AR_S32 VoLayer = 0;
    for(int i=0; i<4; i++)
    {
        AR_MPI_VO_DisableChn(VoLayer, i);
    }
    return 0;
}

AR_S32 close_display()
{
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    AR_MPI_VO_DisableVideoLayer(VoLayer);
    AR_MPI_VO_Disable(VoDev);
    return 0;
}

AR_VOID SAMPLE_AR_MPI_VIN_CloseDev()
{
    return;
}

AR_VOID SAMPLE_VIO_MsgInit(AR_VOID)
{
    return;
}

AR_VOID SAMPLE_VIO_MsgExit(AR_VOID)
{
    return;
}

void SAMPLE_VIO_HandleSig(AR_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        bExit = AR_TRUE;
    }
}



AR_S32 CreateRawTempBuf(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32BufSize = 0;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = NULL;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    pstIrSampleCtx->stRawTempFrame.stVFrame.u32Width = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrRawIdx].u32Width;
    pstIrSampleCtx->stRawTempFrame.stVFrame.u32Height = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrRawIdx].u32Height;
    pstIrSampleCtx->stRawTempFrame.stVFrame.u32Stride[0] = CAM_ALIGNE_TO(pstIrSampleCtx->stRawTempFrame.stVFrame.u32Width * 2, 256);
    u32BufSize = pstIrSampleCtx->stRawTempFrame.stVFrame.u32Stride[0] * pstIrSampleCtx->stRawTempFrame.stVFrame.u32Height;
    pstIrSampleCtx->u32RawTempBufSize = u32BufSize + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstIrSampleCtx->u64RawTempBufOrigPhy,
        &pstIrSampleCtx->pRawTempBufOrigVirt, NULL, NULL, pstIrSampleCtx->u32RawTempBufSize);
    if (s32Ret) {
        ar_err("allocate raw temp buf failed");
        pstIrSampleCtx->u64RawTempBufOrigPhy = 0;
        pstIrSampleCtx->pRawTempBufOrigVirt = NULL;
        return -1;
    }

    pstIrSampleCtx->stRawTempFrame.stVFrame.u64PhyAddr[0] = CAM_ALIGNE_TO(pstIrSampleCtx->u64RawTempBufOrigPhy, 256);
    pstIrSampleCtx->stRawTempFrame.stVFrame.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pstIrSampleCtx->pRawTempBufOrigVirt, 256);

    ar_always("create raw temp buf: size[%u %u] stride[%u] buf_len[%u] orig_addr[0x%llx %p] align_addr[0x%llx 0x%llx]",
        pstIrSampleCtx->stRawTempFrame.stVFrame.u32Width, pstIrSampleCtx->stRawTempFrame.stVFrame.u32Height,
        pstIrSampleCtx->stRawTempFrame.stVFrame.u32Stride[0], pstIrSampleCtx->u32RawTempBufSize,
        pstIrSampleCtx->u64RawTempBufOrigPhy, pstIrSampleCtx->pRawTempBufOrigVirt,
        pstIrSampleCtx->stRawTempFrame.stVFrame.u64PhyAddr[0],
        pstIrSampleCtx->stRawTempFrame.stVFrame.u64VirAddr[0]);

    return 0;
}

AR_S32 CreateThrmlTempBuf(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32BufSize = 0;
    AR_S32 s32IrThrmlIdx = -1;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = NULL;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        s32IrThrmlIdx = pstIrViGrpCfg->s32IrThrmlIdx;
    } else if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        s32IrThrmlIdx = pstIrViGrpCfg->s32IrIsp1Idx;
    }

    pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Width = pstIrViGrpCfg->astOutSize[s32IrThrmlIdx].u32Width;
    pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Height = pstIrViGrpCfg->astOutSize[s32IrThrmlIdx].u32Height;
    pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Stride[0] = CAM_ALIGNE_TO(pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Width * 2, 256);
    u32BufSize = pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Stride[0] * pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Height;
    pstIrSampleCtx->u32ThrmlTempBufSize = u32BufSize + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstIrSampleCtx->u64ThrmlTempBufOrigPhy,
        &pstIrSampleCtx->pThrmlTempBufOrigVirt, NULL, NULL, pstIrSampleCtx->u32ThrmlTempBufSize);
    if (s32Ret) {
        ar_err("allocate thermal temp buf failed");
        pstIrSampleCtx->u64ThrmlTempBufOrigPhy = 0;
        pstIrSampleCtx->pThrmlTempBufOrigVirt = NULL;
        return -1;
    }

    pstIrSampleCtx->stThrmlTempFrame.stVFrame.u64PhyAddr[0] = CAM_ALIGNE_TO(pstIrSampleCtx->u64ThrmlTempBufOrigPhy, 256);
    pstIrSampleCtx->stThrmlTempFrame.stVFrame.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pstIrSampleCtx->pThrmlTempBufOrigVirt, 256);

    ar_always("create thermal temp buf: size[%u %u] stride[%u] buf_len[%u] orig_addr[0x%llx %p] align_addr[0x%llx 0x%llx]",
        pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Width, pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Height,
        pstIrSampleCtx->stThrmlTempFrame.stVFrame.u32Stride[0], pstIrSampleCtx->u32ThrmlTempBufSize,
        pstIrSampleCtx->u64ThrmlTempBufOrigPhy, pstIrSampleCtx->pThrmlTempBufOrigVirt,
        pstIrSampleCtx->stThrmlTempFrame.stVFrame.u64PhyAddr[0],
        pstIrSampleCtx->stThrmlTempFrame.stVFrame.u64VirAddr[0]);

    return 0;
}

AR_S32 Create3DnrTempBuf(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32BufSize = 0;
    AR_S32 s32IrThrmlIdx = -1;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = NULL;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        s32IrThrmlIdx = pstIrViGrpCfg->s32IrThrmlIdx;
    } else if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        s32IrThrmlIdx = pstIrViGrpCfg->s32IrIsp1Idx;
    }

    if (s32IrThrmlIdx < 0) {
        ar_err("no thermal pipe");
        return -1;
    }

    pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Width = pstIrViGrpCfg->astOutSize[s32IrThrmlIdx].u32Width;
    pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Height = pstIrViGrpCfg->astOutSize[s32IrThrmlIdx].u32Height;
    pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Stride[0] = CAM_ALIGNE_TO(pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Width * 2, 256);
    u32BufSize = pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Stride[0] * pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Height;
    pstIrSampleCtx->u32Ir3DnrTempBufSize = u32BufSize + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy,
        &pstIrSampleCtx->pIr3DnrTempBufOrigVirt, NULL, NULL, pstIrSampleCtx->u32Ir3DnrTempBufSize);
    if (s32Ret) {
        ar_err("allocate ir 3dnr temp buf failed");
        pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy = 0;
        pstIrSampleCtx->pIr3DnrTempBufOrigVirt = NULL;
        return -1;
    }

    pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u64PhyAddr[0] = CAM_ALIGNE_TO(pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy, 256);
    pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pstIrSampleCtx->pIr3DnrTempBufOrigVirt, 256);

    ar_always("create ir 3dnr temp buf: size[%u %u] stride[%u] buf_len[%u] orig_addr[0x%llx %p] align_addr[0x%llx 0x%llx]",
        pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Width, pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Height,
        pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u32Stride[0], pstIrSampleCtx->u32Ir3DnrTempBufSize,
        pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy, pstIrSampleCtx->pIr3DnrTempBufOrigVirt,
        pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u64PhyAddr[0],
        pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u64VirAddr[0]);

    return 0;
}

AR_S32 Create2DnrTempBuf(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32BufSize = 0;
    AR_S32 s32IrThrmlIdx = -1;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = NULL;

    if (pstIrSampleCtx == NULL) {
        ar_err("pstIrSampleCtx is null");
        return -1;
    }

    pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        s32IrThrmlIdx = pstIrViGrpCfg->s32IrThrmlIdx;
    } else if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        s32IrThrmlIdx = pstIrViGrpCfg->s32IrIsp1Idx;
    }

    if (s32IrThrmlIdx < 0) {
        ar_err("no thermal pipe");
        return -1;
    }

    pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Width = pstIrViGrpCfg->astOutSize[s32IrThrmlIdx].u32Width;
    pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Height = pstIrViGrpCfg->astOutSize[s32IrThrmlIdx].u32Height;
    pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Stride[0] = CAM_ALIGNE_TO(pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Width * 2, 256);
    u32BufSize = pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Stride[0] * pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Height;
    pstIrSampleCtx->u32Ir2DnrTempBufSize = u32BufSize + 256;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy,
        &pstIrSampleCtx->pIr2DnrTempBufOrigVirt, NULL, NULL, pstIrSampleCtx->u32Ir2DnrTempBufSize);
    if (s32Ret) {
        ar_err("allocate ir 2dnr temp buf failed");
        pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy = 0;
        pstIrSampleCtx->pIr2DnrTempBufOrigVirt = NULL;
        return -1;
    }

    pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u64PhyAddr[0] = CAM_ALIGNE_TO(pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy, 256);
    pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pstIrSampleCtx->pIr2DnrTempBufOrigVirt, 256);

    ar_always("create ir 2dnr temp buf: size[%u %u] stride[%u] buf_len[%u] orig_addr[0x%llx %p] align_addr[0x%llx 0x%llx]",
        pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Width, pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Height,
        pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u32Stride[0], pstIrSampleCtx->u32Ir2DnrTempBufSize,
        pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy, pstIrSampleCtx->pIr2DnrTempBufOrigVirt,
        pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u64PhyAddr[0],
        pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u64VirAddr[0]);

    return 0;
}


static AR_S32 ManualShutter(IR_SAMPLE_CTX_S *pstIrSampleCtx, AR_BOOL bOpen)
{
    AR_S32 s32Ret = 0;
    AR_BOOL bAutoCalibBackup = AR_FALSE;
    VI_PIPE rawPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);

    if (pstIrSampleCtx->bIrDynamicCalibBusy) {
        ar_err("manual %s shutter failed, because there is a auto b calib task going on",
            (bOpen ? "open" : "close"));
        s32Ret = -1;
        goto EXIT;
    }

    bAutoCalibBackup = pstIrSampleCtx->bAutoCalib;
    pstIrSampleCtx->bAutoCalib = AR_FALSE;

    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

    if (bOpen) {
        s32Ret = AR_MPI_ISP_SnsSwitchShutter(rawPipe, AR_TRUE);
    } else {
        s32Ret = AR_MPI_ISP_SnsSwitchShutter(rawPipe, AR_FALSE);
    }

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);

    if (s32Ret) {
        ar_err("manual %s shutter failed", (bOpen ? "open" : "close"));
        pstIrSampleCtx->bAutoCalib = bAutoCalibBackup;
    }

EXIT:
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);
    return s32Ret;
}

static AR_S32 StartIrRawCapture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    AR_S32 s32Ret = 0;

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);

    if (pstIrSampleCtx->enIrRawProcState != IR_RAW_PROCESS_STATE_NONE) {
        ar_err("start ir raw capture failed, because there is a raw process task going on");
        s32Ret = -1;
        goto EXIT;
    }

    pstIrSampleCtx->enIrRawProcState = IR_RAW_PROCESS_STATE_CAPTURE;
    pstIrSampleCtx->u32IrRawCapFrms = u32CapFrms;
    pstIrSampleCtx->bRawCapNoDummy = bNoDummy;
    strcpy(pstIrSampleCtx->achRawCapFname, strFname);

EXIT:
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);
    return s32Ret;
}

static AR_S32 StartIrThrmlCapture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);
    pstIrSampleCtx->s32IrThrmlDumpFrms = u32CapFrms;
    strcpy(pstIrSampleCtx->achThrmlDumpFname, strFname);
    pstIrSampleCtx->bThrmlDumpNoDummy = bNoDummy;
    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);
    return 0;
}

static AR_S32 StartIr3DnrCapture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);
    pstIrSampleCtx->s32Ir3DnrDumpFrms = u32CapFrms;
    strcpy(pstIrSampleCtx->ach3DnrDumpFname, strFname);
    pstIrSampleCtx->b3DnrDumpNoDummy = bNoDummy;
    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);
    return 0;
}

static AR_S32 StartIr2DnrCapture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);
    pstIrSampleCtx->s32Ir2DnrDumpFrms = u32CapFrms;
    strcpy(pstIrSampleCtx->ach2DnrDumpFname, strFname);
    pstIrSampleCtx->b2DnrDumpNoDummy = bNoDummy;
    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);
    return 0;
}

static AR_S32 StartIrGtmCapture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);
    pstIrSampleCtx->s32IrGtmDumpFrms = u32CapFrms;
    strcpy(pstIrSampleCtx->achGtmDumpFname, strFname);
    pstIrSampleCtx->bGtmDumpNoDummy = bNoDummy;
    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);
    return 0;
}

static AR_S32 StartIrIsp1Capture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    pthread_mutex_lock(&pstIrSampleCtx->irIsp1ProcLock);
    pstIrSampleCtx->s32IrIsp1DumpFrms = u32CapFrms;
    strcpy(pstIrSampleCtx->achIsp1DumpFname, strFname);
    pstIrSampleCtx->bIsp1DumpNoDummy = bNoDummy;
    pthread_mutex_unlock(&pstIrSampleCtx->irIsp1ProcLock);
    return 0;
}

static AR_S32 StartIrIsp2Capture(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32CapFrms, AR_CHAR *strFname, AR_BOOL bNoDummy)
{
    pthread_mutex_lock(&pstIrSampleCtx->irIsp2ProcLock);
    pstIrSampleCtx->s32IrIsp2DumpFrms = u32CapFrms;
    strcpy(pstIrSampleCtx->achIsp2DumpFname, strFname);
    pstIrSampleCtx->bIsp2DumpNoDummy = bNoDummy;
    pthread_mutex_unlock(&pstIrSampleCtx->irIsp2ProcLock);
    return 0;
}

static int get_time_interval_us(struct timeval t1, struct timeval t2)
{
    return ((long long)t2.tv_sec - (long long)t1.tv_sec) * 1000000 + ((long long)t2.tv_usec - (long long)t1.tv_usec);
}

AR_S32 IrDynamicCalib(IR_SAMPLE_CTX_S *pstIrSampleCtx, IR_DYNAMIC_CALIB_TYPE_E enCalibType, AR_BOOL bSwitchShutter)
{
    AR_S32 s32Ret = 0;
    VI_PIPE rawPipe = -1;
    VI_PIPE thrmlPipe = -1;
    IR_SNS_CALIB_RESULT_S stSnsCalibResult;
    IR_OCC_CALIB_RESULT_S stOccCalibResult;
    IR_B_CALIB_RESULT_S stBCalibResult;
    struct timeval stStartTime;
    struct timeval stEndTime;

    memset(&stSnsCalibResult, 0, sizeof(stSnsCalibResult));
    memset(&stOccCalibResult, 0, sizeof(stOccCalibResult));
    memset(&stBCalibResult, 0, sizeof(stBCalibResult));

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx >= 0) {
        rawPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
        thrmlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
    } else if (pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx >= 0) {
        thrmlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
    }

    if (rawPipe < 0) {
        ar_err("ir raw pipe is closed, ir dynamic calib is not supported");
        return -1;
    }

    gettimeofday(&stStartTime, NULL);

    if (bSwitchShutter && enCalibType != IR_DYNAMIC_CALIB_TYPE_NONE) {
        s32Ret = AR_MPI_ISP_SnsSwitchShutter(rawPipe, AR_FALSE);
        if (s32Ret) {
            ar_err("pipe[%d] close shutter failed", rawPipe);
            goto EXIT;
        }
    }

    if (enCalibType >= IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B) {
        usleep(80 * 1000);

        s32Ret = AR_MPI_ISP_StartIrSnsCalib(rawPipe, &pstIrSampleCtx->stIrSnsCalibCfg);
        if (s32Ret) {
            ar_err("pipe[%d] start ir sensor calib failed", rawPipe);
            goto EXIT;
        }

        //stSnsCalibResult.pCalibParam = pstIrSampleCtx->pstIrParamCtx->pSnsParam;

        while (1) {
            s32Ret = AR_MPI_ISP_GetIrSnsCalibResult(rawPipe, &stSnsCalibResult);
            if (s32Ret) {
                ar_err("pipe[%d] get ir sensor calib result failed", rawPipe);
                AR_MPI_ISP_CancelIrSnsCalib(rawPipe);
                goto EXIT;
            } else {
                if (stSnsCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                    break;
                } else if (stSnsCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                    ar_err("pipe[%d] ir sensor calib cancelled", rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                } else if (stSnsCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                    ar_err("pipe[%d] ir sensor calib failed", rawPipe);
                    AR_MPI_ISP_CancelIrSnsCalib(rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                }
            }

            usleep(40 * 1000);
        }
    }

    if (enCalibType >= IR_DYNAMIC_CALIB_TYPE_OCC_B) {
        usleep(80 * 1000);

        s32Ret = AR_MPI_ISP_StartIrOccCalib(rawPipe, &pstIrSampleCtx->stIrOccCalibCfg);
        if (s32Ret) {
            ar_err("pipe[%d] start ir occ calib failed", rawPipe);
            goto EXIT;
        }

        //stOccCalibResult.stOccBuf = pstIrSampleCtx->pstIrParamCtx->stOccParam.stOccBuf;

        while (1) {
            s32Ret = AR_MPI_ISP_GetIrOccCalibResult(rawPipe, &stOccCalibResult);
            if (s32Ret) {
                ar_err("pipe[%d] get ir occ calib result failed", rawPipe);
                AR_MPI_ISP_CancelIrOccCalib(rawPipe);
                goto EXIT;
            } else {
                if (stOccCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                    break;
                } else if (stOccCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                    ar_err("pipe[%d] ir occ calib cancelled", rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                } else if (stOccCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                    ar_err("pipe[%d] ir occ calib failed", rawPipe);
                    AR_MPI_ISP_CancelIrOccCalib(rawPipe);
                    s32Ret = -1;
                    goto EXIT;
                }
            }
        }
    }

    if (enCalibType >= IR_DYNAMIC_CALIB_TYPE_B && thrmlPipe >= 0) {
        usleep(80 * 1000);

        s32Ret = AR_MPI_ISP_StartIrBCalib(thrmlPipe, &pstIrSampleCtx->stIrBCalibCfg);
        if (s32Ret) {
            ar_err("pipe[%d] start ir b calib failed", thrmlPipe);
            goto EXIT;
        }

        //stBCalibResult.stBBuf = pstIrSampleCtx->pstIrParamCtx->stKbParam.stKbBuf;
        //stBCalibResult.stBBuf.u32Stride[0] = stBCalibResult.stBBuf.u32Stride[1];
        //stBCalibResult.stBBuf.u64PhyAddr[0] = stBCalibResult.stBBuf.u64PhyAddr[1];
        //stBCalibResult.stBBuf.u64VirAddr[0] = stBCalibResult.stBBuf.u64VirAddr[1];
        //stBCalibResult.stBBuf.u32Stride[1] = 0;
        //stBCalibResult.stBBuf.u64PhyAddr[1] = 0;
        //stBCalibResult.stBBuf.u64VirAddr[1] = 0;

        while (1) {
            s32Ret = AR_MPI_ISP_GetIrBCalibResult(thrmlPipe, &stBCalibResult);
            if (s32Ret) {
                ar_err("pipe[%d] get ir b calib result failed", thrmlPipe);
                AR_MPI_ISP_CancelIrBCalib(thrmlPipe);
                goto EXIT;
            } else {
                if (stBCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                    break;
                } else if (stBCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                    ar_err("pipe[%d] ir b calib cancelled", thrmlPipe);
                    s32Ret = -1;
                    goto EXIT;
                } else if (stBCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                    ar_err("pipe[%d] ir b calib failed", thrmlPipe);
                    AR_MPI_ISP_CancelIrBCalib(thrmlPipe);
                    s32Ret = -1;
                    goto EXIT;
                }
            }

            usleep(40 * 1000);
        }
    }

    if (bSwitchShutter && enCalibType != IR_DYNAMIC_CALIB_TYPE_NONE) {
        s32Ret = AR_MPI_ISP_SnsSwitchShutter(rawPipe, AR_TRUE);
        if (s32Ret) {
            ar_err("pipe[%d] open shutter failed", rawPipe);
            goto EXIT;
        }
    }

    gettimeofday(&stEndTime, NULL);

    ar_always("ir dynamic calib time: %d ms", get_time_interval_us(stStartTime, stEndTime) / 1000);

EXIT:
    return s32Ret;
}

static AR_S32 StartIrBCalib(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    AR_BOOL bAutoCalibBackup = AR_FALSE;

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);

    if (pstIrSampleCtx->bIrDynamicCalibBusy) {
        ar_err("start b calibration failed, because auto ir dynamic calib is going on");
        s32Ret = -1;
        pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);
        goto EXIT;
    }

    bAutoCalibBackup = pstIrSampleCtx->bAutoCalib;
    pstIrSampleCtx->bAutoCalib = AR_FALSE;
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

    s32Ret = IrDynamicCalib(pstIrSampleCtx, IR_DYNAMIC_CALIB_TYPE_B, AR_FALSE);

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
    pstIrSampleCtx->bAutoCalib = bAutoCalibBackup;
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

EXIT:
    return s32Ret;
}

#define IR_AUTO_B_CALIB_WAIT_FRMS           8

static AR_S32 StartIrAutoDynamicCalib(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
    pstIrSampleCtx->bAutoCalib = AR_TRUE;
    pstIrSampleCtx->bForceIrDynamicCalib = AR_TRUE;
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

    return 0;
}

static AR_S32 IrRawOutHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    AR_S32 s32Ret = 0;
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    AR_BOOL bUseRawTempBuf = AR_FALSE;

    if (!bIrParamInited) {
        return 0;
    }

    if (pstIrSampleCtx->bUseRawTempBuf) {
        s32Ret = ar_hal_sys_memcpy_pa(pstIrSampleCtx->stRawTempFrame.stVFrame.u64PhyAddr[0],
            pstFrameInfo->stVFrame.u64PhyAddr[0],
            pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height);
        if (s32Ret) {
            ar_err("dma copy raw failed");
            s32Ret = 0;
        } else {
            bUseRawTempBuf = AR_TRUE;
        }
    }

    if (pstIrSampleCtx->pfnRawProcess) {
        pstIrSampleCtx->pfnRawProcess(pstIrSampleCtx,
            (bUseRawTempBuf ? &pstIrSampleCtx->stRawTempFrame : pstFrameInfo));
    }

    if (bUseRawTempBuf) {
        s32Ret = AR_MPI_SYS_MmzFlushCache(pstIrSampleCtx->u64RawTempBufOrigPhy,
            pstIrSampleCtx->pRawTempBufOrigVirt, pstIrSampleCtx->u32RawTempBufSize);
        if (s32Ret) {
            ar_err("flush raw temp buffer failed");
            goto EXIT;
        }

        s32Ret = ar_hal_sys_memcpy_pa(pstFrameInfo->stVFrame.u64PhyAddr[0],
            pstIrSampleCtx->stRawTempFrame.stVFrame.u64PhyAddr[0],
            pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height);
        if (s32Ret) {
            ar_err("dma copy raw failed");
            goto EXIT;
        }
    }

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);

    switch (pstIrSampleCtx->enIrRawProcState) {
    case IR_RAW_PROCESS_STATE_CAPTURE:
        if (pstIrSampleCtx->u32IrRawCapFrms > 0 && pstIrSampleCtx->achRawCapFname[0] != '\0') {
            if (pstIrSampleCtx->pRawFp == NULL) {
                pstIrSampleCtx->pRawFp = fopen(pstIrSampleCtx->achRawCapFname, "wb");
                if (pstIrSampleCtx->pRawFp == NULL) {
                    ar_err("open %s failed", pstIrSampleCtx->achRawCapFname);
                    pstIrSampleCtx->enIrRawProcState = IR_RAW_PROCESS_STATE_NONE;
                    break;
                }
            }

            pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

            AR_VOID *pData = NULL;
            if (bUseRawTempBuf) {
                pData = (AR_VOID *)pstIrSampleCtx->stRawTempFrame.stVFrame.u64VirAddr[0];
            } else {
                pData = (AR_VOID *)pstFrameInfo->stVFrame.u64VirAddr[0];
            }

            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->bRawCapNoDummy) {
                wsize = fwrite(pData, 1,
                    pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height,
                    pstIrSampleCtx->pRawFp);
                if (wsize != pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height) {
                    ar_err("write ir raw to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->achRawCapFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrSnsRoi.s32Y * pstFrameInfo->stVFrame.u32Stride[0]
                    + pstIrSampleCtx->stIrSnsRoi.s32X * 2);

                for (i = 0; i < pstIrSampleCtx->stIrSnsRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrSnsRoi.u32Width * 2, pstIrSampleCtx->pRawFp);
                    if (wsize != pstIrSampleCtx->stIrSnsRoi.u32Width * 2) {
                        ar_err("write ir raw line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->achRawCapFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += pstFrameInfo->stVFrame.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir raw to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->achRawCapFname, wsize);
                pstIrSampleCtx->enIrRawProcState = IR_RAW_PROCESS_STATE_NONE;
                fclose(pstIrSampleCtx->pRawFp);
                pstIrSampleCtx->pRawFp = NULL;
                break;
            }

            printf("dump ir raw frame[%u]\n", pstIrSampleCtx->u32IrRawCapFrms);
            pstIrSampleCtx->u32IrRawCapFrms--;
            if (pstIrSampleCtx->u32IrRawCapFrms == 0) {
                printf("dump ir raw to file[%s] finished\n", pstIrSampleCtx->achRawCapFname);
                pstIrSampleCtx->enIrRawProcState = IR_RAW_PROCESS_STATE_NONE;
                fclose(pstIrSampleCtx->pRawFp);
                pstIrSampleCtx->pRawFp = NULL;
            }
        }
        break;

    default:
        break;
    }

    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

EXIT:
    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32RawFrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return s32Ret;
}

//#define UPDATE_AEC_OLD_METHOD

static AR_S32 IrThrmlOutHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    AR_S32 s32Ret = 0;
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    AR_S32 s32CurGain = 0;
    AR_S32 s32SetGain = 0;
    VI_PIPE thrmlPipe = -1;
    IR_ALGO_INPUT_S stIrAlgoInput;
    IR_ALGO_OUTPUT_S stIrAlgoOutput;
    AR_BOOL b3DnrOutput = AR_FALSE;
    AR_BOOL b2DnrOutput = AR_FALSE;
    AR_BOOL bGtmOutput = AR_FALSE;

    static AR_S32 s32LastGain = 0;

    static AR_U32 u32IrAlgoProcFrmCnt = 0;
#ifdef UPDATE_AEC_OLD_METHOD
    ISP_AEC_MANU_TIDY_ATTR_S stAecManuTidyAttr = {0};
#else
    static STRU_AEC_OUT_T stIrThrmlAecOut;
    static STRU_AEC_OUT_T stIrIsp1AecOut;
    static STRU_AEC_OUT_T stIrIsp2AecOut;
    static AR_BOOL bThrmalAecUpdateGet = AR_FALSE;
#endif

    // dump ir_thermal image
    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);

    if (pstIrSampleCtx->s32IrThrmlDumpFrms > 0 && pstIrSampleCtx->achThrmlDumpFname[0] != '\0') {
        if (pstIrSampleCtx->pThrmlFp == NULL) {
            pstIrSampleCtx->pThrmlFp = fopen(pstIrSampleCtx->achThrmlDumpFname, "wb");
            if (pstIrSampleCtx->pThrmlFp == NULL) {
                ar_err("open %s failed", pstIrSampleCtx->achThrmlDumpFname);
            }
        }

        if (pstIrSampleCtx->pThrmlFp) {
            pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);

            AR_VOID *pData = (AR_VOID *)pstFrameInfo->stVFrame.u64VirAddr[0];
            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->bThrmlDumpNoDummy) {
                wsize = fwrite(pData, 1,
                    pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height,
                    pstIrSampleCtx->pThrmlFp);
                if (wsize != pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height) {
                    ar_err("write ir thermal to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->achThrmlDumpFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrIspRoi.s32Y * pstFrameInfo->stVFrame.u32Stride[0]
                    + pstIrSampleCtx->stIrIspRoi.s32X * 2);

                for (i = 0; i < pstIrSampleCtx->stIrIspRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrIspRoi.u32Width * 2, pstIrSampleCtx->pThrmlFp);
                    if (wsize != pstIrSampleCtx->stIrIspRoi.u32Width * 2) {
                        ar_err("write ir thermal line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->achThrmlDumpFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += pstFrameInfo->stVFrame.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir thermal image to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->achThrmlDumpFname, wsize);
                fclose(pstIrSampleCtx->pThrmlFp);
                pstIrSampleCtx->pThrmlFp = NULL;
            } else{
                //printf("dump ir thermal image frame[%u]\n", pstIrSampleCtx->s32IrThrmlDumpFrms);
                pstIrSampleCtx->s32IrThrmlDumpFrms--;
            }

            if (pstIrSampleCtx->s32IrThrmlDumpFrms == 0) {
                printf("dump ir thermal image to file[%s] finished\n", pstIrSampleCtx->achThrmlDumpFname);
                fclose(pstIrSampleCtx->pThrmlFp);
                pstIrSampleCtx->pThrmlFp = NULL;
            }
        }
    }

    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);

#ifndef UPDATE_AEC_OLD_METHOD
    if (!bThrmalAecUpdateGet) {
        if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
            s32Ret |= AR_MPI_ISP_GetAecUpdate(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx],
                &stIrThrmlAecOut);
            if (s32Ret) {
                ar_err("pipe[%d] get aec update failed",
                    pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx]);
            }
        }

        if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
            s32Ret |= AR_MPI_ISP_GetAecUpdate(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx],
                &stIrIsp1AecOut);
            if (s32Ret) {
                ar_err("pipe[%d] get aec update failed",
                    pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx]);
            }
        }

        if (pstIrViGrpCfg->s32IrIsp2Idx >= 0) {
            s32Ret |= AR_MPI_ISP_GetAecUpdate(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp2Idx],
                &stIrIsp2AecOut);
            if (s32Ret) {
                ar_err("pipe[%d] get aec update failed",
                    pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp2Idx]);
            }
        }

        if (s32Ret == 0) {
            bThrmalAecUpdateGet = AR_TRUE;
        } else {
            s32Ret = 0;
        }
    }
#endif

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        thrmlPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx];
    } else if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        thrmlPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx];
    }

    memset(&stIrAlgoInput, 0, sizeof(IR_ALGO_INPUT_S));
    stIrAlgoInput.stInputFrm = pstFrameInfo->stVFrame;
    stIrAlgoInput.stInputFrm.u32Width = pstIrSampleCtx->stIrIspRoi.u32Width;
    stIrAlgoInput.b3DnrFirstFrame = (u32IrAlgoProcFrmCnt == 0 ? AR_TRUE : AR_FALSE);
    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        stIrAlgoInput.bUseHwGtm = AR_FALSE;     // if ir_thermal is a single pipe, only sw_gtm can be used instead of hw_gtm
    } else {
        stIrAlgoInput.bUseHwGtm = AR_TRUE;      // if ir_thermal shares ir_isp1 pipe, only hw_gtm can be used
    }

    memset(&stIrAlgoOutput, 0, sizeof(stIrAlgoOutput));
    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        // if ir_thermal is a single pipe, use sw_gtm and there is gtm output frame
        // TODO: in this case, 2dnr output frame is also needed
        stIrAlgoOutput.stGtmOutput.stOutputFrm = stIrAlgoInput.stInputFrm;
        bGtmOutput = AR_TRUE;
    } else {
        // if ir_thermal shares ir_isp1 pipe, use hw_gtm and there is no gtm output frame,
        // so only get 2dnr output frame here
        stIrAlgoOutput.st2DnrOutput.stOutputFrm = stIrAlgoInput.stInputFrm;
    }

    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);
    if (pstIrSampleCtx->s32Ir3DnrDumpFrms > 0
        && pstIrSampleCtx->stIr3DnrTempFrame.stVFrame.u64PhyAddr[0]) {
        stIrAlgoOutput.st3DnrOutput.stOutputFrm = pstIrSampleCtx->stIr3DnrTempFrame.stVFrame;
        b3DnrOutput = AR_TRUE;
    }
    if (pstIrSampleCtx->s32Ir2DnrDumpFrms > 0
        && pstIrSampleCtx->stIr2DnrTempFrame.stVFrame.u64PhyAddr[0]) {
        stIrAlgoOutput.st2DnrOutput.stOutputFrm = pstIrSampleCtx->stIr2DnrTempFrame.stVFrame;
        b2DnrOutput = AR_TRUE;
    }
    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);

    s32Ret = AR_MPI_ISP_IrAlgoProcess(thrmlPipe, &stIrAlgoInput, &stIrAlgoOutput);
    if (s32Ret) {
        ar_err("thermal image ir algo process failed");
        goto EXIT;
    }

    u32IrAlgoProcFrmCnt++;

    // dump gtm output image
    pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);

    if (pstIrSampleCtx->s32IrGtmDumpFrms > 0 && pstIrSampleCtx->achGtmDumpFname[0] != '\0'
        && stIrAlgoOutput.stGtmOutput.stOutputFrm.u64VirAddr[0] && bGtmOutput) {
        if (pstIrSampleCtx->pGtmFp == NULL) {
            pstIrSampleCtx->pGtmFp = fopen(pstIrSampleCtx->achGtmDumpFname, "wb");
            if (pstIrSampleCtx->pGtmFp == NULL) {
                ar_err("open %s failed", pstIrSampleCtx->achGtmDumpFname);
            }
        }

        if (pstIrSampleCtx->pGtmFp) {
            pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);

            AR_VOID *pData = (AR_VOID *)stIrAlgoOutput.stGtmOutput.stOutputFrm.u64VirAddr[0];
            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->bGtmDumpNoDummy) {
                wsize = fwrite(pData, 1,
                    stIrAlgoOutput.stGtmOutput.stOutputFrm.u32Stride[0] * stIrAlgoOutput.stGtmOutput.stOutputFrm.u32Height,
                    pstIrSampleCtx->pGtmFp);
                if (wsize != stIrAlgoOutput.stGtmOutput.stOutputFrm.u32Stride[0] * stIrAlgoOutput.stGtmOutput.stOutputFrm.u32Height) {
                    ar_err("write ir gtm to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->achGtmDumpFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrIspRoi.s32Y * stIrAlgoOutput.stGtmOutput.stOutputFrm.u32Stride[0]
                    + pstIrSampleCtx->stIrIspRoi.s32X * 2);

                for (i = 0; i < pstIrSampleCtx->stIrIspRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrIspRoi.u32Width * 2, pstIrSampleCtx->pGtmFp);
                    if (wsize != pstIrSampleCtx->stIrIspRoi.u32Width * 2) {
                        ar_err("write ir gtm line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->achGtmDumpFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += stIrAlgoOutput.stGtmOutput.stOutputFrm.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir gtm output image to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->achGtmDumpFname, wsize);
                fclose(pstIrSampleCtx->pGtmFp);
                pstIrSampleCtx->pGtmFp = NULL;
            } else{
                //printf("dump ir gtm output image frame[%u]\n", pstIrSampleCtx->s32IrGtmDumpFrms);
                pstIrSampleCtx->s32IrGtmDumpFrms--;
            }

            if (pstIrSampleCtx->s32IrGtmDumpFrms == 0) {
                printf("dump ir gtm output image to file[%s] finished\n", pstIrSampleCtx->achGtmDumpFname);
                fclose(pstIrSampleCtx->pGtmFp);
                pstIrSampleCtx->pGtmFp = NULL;
            }
        }
    }

    if (pstIrSampleCtx->s32Ir3DnrDumpFrms > 0 &&  pstIrSampleCtx->ach3DnrDumpFname[0] != '\0'
        && stIrAlgoOutput.st3DnrOutput.stOutputFrm.u64VirAddr[0] && b3DnrOutput) {
        if (pstIrSampleCtx->p3DnrFp == NULL) {
            pstIrSampleCtx->p3DnrFp = fopen(pstIrSampleCtx->ach3DnrDumpFname, "wb");
            if (pstIrSampleCtx->p3DnrFp == NULL) {
                ar_err("open %s failed", pstIrSampleCtx->ach3DnrDumpFname);
            }
        }

        if (pstIrSampleCtx->p3DnrFp) {
            pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);

            AR_VOID *pData = (AR_VOID *)stIrAlgoOutput.st3DnrOutput.stOutputFrm.u64VirAddr[0];
            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->b3DnrDumpNoDummy) {
                wsize = fwrite(pData, 1,
                    stIrAlgoOutput.st3DnrOutput.stOutputFrm.u32Stride[0] * stIrAlgoOutput.st3DnrOutput.stOutputFrm.u32Height,
                    pstIrSampleCtx->p3DnrFp);
                if (wsize != stIrAlgoOutput.st3DnrOutput.stOutputFrm.u32Stride[0] * stIrAlgoOutput.st3DnrOutput.stOutputFrm.u32Height) {
                    ar_err("write ir 3dnr to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->ach3DnrDumpFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrIspRoi.s32Y * stIrAlgoOutput.st3DnrOutput.stOutputFrm.u32Stride[0]
                    + pstIrSampleCtx->stIrIspRoi.s32X * 2);

                for (i = 0; i < pstIrSampleCtx->stIrIspRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrIspRoi.u32Width * 2, pstIrSampleCtx->p3DnrFp);
                    if (wsize != pstIrSampleCtx->stIrIspRoi.u32Width * 2) {
                        ar_err("write ir 3dnr line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->ach3DnrDumpFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += stIrAlgoOutput.st3DnrOutput.stOutputFrm.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir 3dnr output image to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->ach3DnrDumpFname, wsize);
                fclose(pstIrSampleCtx->p3DnrFp);
                pstIrSampleCtx->p3DnrFp = NULL;
            } else{
                //printf("dump ir 3dnr output image frame[%u]\n", pstIrSampleCtx->s32Ir3DnrDumpFrms);
                pstIrSampleCtx->s32Ir3DnrDumpFrms--;
            }

            if (pstIrSampleCtx->s32Ir3DnrDumpFrms == 0) {
                printf("dump ir 3dnr output image to file[%s] finished\n", pstIrSampleCtx->ach3DnrDumpFname);
                fclose(pstIrSampleCtx->p3DnrFp);
                pstIrSampleCtx->p3DnrFp = NULL;
            }
        }
    }

    if (pstIrSampleCtx->s32Ir2DnrDumpFrms > 0 &&  pstIrSampleCtx->ach2DnrDumpFname[0] != '\0'
        && stIrAlgoOutput.st2DnrOutput.stOutputFrm.u64VirAddr[0] && b2DnrOutput) {
        if (pstIrSampleCtx->p2DnrFp == NULL) {
            pstIrSampleCtx->p2DnrFp = fopen(pstIrSampleCtx->ach2DnrDumpFname, "wb");
            if (pstIrSampleCtx->p2DnrFp == NULL) {
                ar_err("open %s failed", pstIrSampleCtx->ach2DnrDumpFname);
            }
        }

        if (pstIrSampleCtx->p2DnrFp) {
            pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);

            AR_VOID *pData = (AR_VOID *)stIrAlgoOutput.st2DnrOutput.stOutputFrm.u64VirAddr[0];
            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->b2DnrDumpNoDummy) {
                wsize = fwrite(pData, 1,
                    stIrAlgoOutput.st2DnrOutput.stOutputFrm.u32Stride[0] * stIrAlgoOutput.st2DnrOutput.stOutputFrm.u32Height,
                    pstIrSampleCtx->p2DnrFp);
                if (wsize != stIrAlgoOutput.st2DnrOutput.stOutputFrm.u32Stride[0] * stIrAlgoOutput.st2DnrOutput.stOutputFrm.u32Height) {
                    ar_err("write ir 2dnr to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->ach2DnrDumpFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrIspRoi.s32Y * stIrAlgoOutput.st2DnrOutput.stOutputFrm.u32Stride[0]
                    + pstIrSampleCtx->stIrIspRoi.s32X * 2);

                for (i = 0; i < pstIrSampleCtx->stIrIspRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrIspRoi.u32Width * 2, pstIrSampleCtx->p2DnrFp);
                    if (wsize != pstIrSampleCtx->stIrIspRoi.u32Width * 2) {
                        ar_err("write ir 2dnr line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->ach2DnrDumpFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += stIrAlgoOutput.st2DnrOutput.stOutputFrm.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irThrmlProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir 2dnr output image to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->ach2DnrDumpFname, wsize);
                fclose(pstIrSampleCtx->p2DnrFp);
                pstIrSampleCtx->p2DnrFp = NULL;
            } else{
                //printf("dump ir 2dnr output image frame[%u]\n", pstIrSampleCtx->s32Ir2DnrDumpFrms);
                pstIrSampleCtx->s32Ir2DnrDumpFrms--;
            }

            if (pstIrSampleCtx->s32Ir2DnrDumpFrms == 0) {
                printf("dump ir 2dnr output image to file[%s] finished\n", pstIrSampleCtx->ach2DnrDumpFname);
                fclose(pstIrSampleCtx->p2DnrFp);
                pstIrSampleCtx->p2DnrFp = NULL;
            }
        }
    }

    pthread_mutex_unlock(&pstIrSampleCtx->irThrmlProcLock);


#ifdef UPDATE_AEC_OLD_METHOD
    s32CurGain = (AR_S32)stIrAlgoOutput.stGtmOutput.u32HistUpperPos
        - (AR_S32)stIrAlgoOutput.stGtmOutput.u32HistLowerPos;

    stAecManuTidyAttr.stAecManu.aec_mode = 5;
    stAecManuTidyAttr.stAecManu.gain = s32CurGain;

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        s32Ret = AR_MPI_ISP_SetAecManuTidyAttr(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx],
            &stAecManuTidyAttr);
        if (s32Ret) {
            ar_err("pipe[%d] set aec manual tidy attr failed",
                pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx]);
            goto EXIT;
        }
    }

    if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        s32Ret = AR_MPI_ISP_SetAecManuTidyAttr(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx],
            &stAecManuTidyAttr);
        if (s32Ret) {
            ar_err("pipe[%d] set aec manual tidy attr failed",
                pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx]);
            goto EXIT;
        }
    }

    if (pstIrViGrpCfg->s32IrIsp2Idx >= 0) {
        s32Ret = AR_MPI_ISP_SetAecManuTidyAttr(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp2Idx],
            &stAecManuTidyAttr);
        if (s32Ret) {
            ar_err("pipe[%d] set aec manual tidy attr failed",
                pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp2Idx]);
            goto EXIT;
        }
    }
#else
    if (bThrmalAecUpdateGet) {
        s32CurGain = (AR_S32)stIrAlgoOutput.stGtmOutput.u32HistUpperPos
            - (AR_S32)stIrAlgoOutput.stGtmOutput.u32HistLowerPos;

        if (u32IrAlgoProcFrmCnt > 1) {
            s32SetGain = (AR_S32)(s32CurGain * pstIrSampleCtx->f32GainAlpha
                + s32LastGain * (1 - pstIrSampleCtx->f32GainAlpha));
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

        if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
            stIrThrmlAecOut.real_gain = s32SetGain;
            s32Ret = AR_MPI_ISP_AecUpdate(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx],
                &stIrThrmlAecOut);
            if (s32Ret) {
                ar_err("pipe[%d] aec update failed",
                    pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx]);
                s32Ret = 0;
            }
        }

        if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
            stIrIsp1AecOut.real_gain = s32SetGain;
            s32Ret = AR_MPI_ISP_AecUpdate(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx],
                &stIrIsp1AecOut);
            if (s32Ret) {
                ar_err("pipe[%d] aec update failed",
                    pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx]);
                s32Ret = 0;
            }
        }

        if (pstIrViGrpCfg->s32IrIsp2Idx >= 0) {
            stIrIsp2AecOut.real_gain = s32SetGain;
            s32Ret = AR_MPI_ISP_AecUpdate(pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp2Idx],
                &stIrIsp2AecOut);
            if (s32Ret) {
                ar_err("pipe[%d] aec update failed",
                    pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp2Idx]);
                s32Ret = 0;
            }
        }
    }
#endif

EXIT:
    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32ThrmlFrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return s32Ret;
}

AR_S32 IrThrmlDeqHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32ThrmlDeqFrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return 0;
}

static AR_S32 IrIsp1OutHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    // dump ir_isp1 image
    pthread_mutex_lock(&pstIrSampleCtx->irIsp1ProcLock);

    if (pstIrSampleCtx->s32IrIsp1DumpFrms > 0) {
        if (pstIrSampleCtx->pIsp1Fp == NULL) {
            pstIrSampleCtx->pIsp1Fp = fopen(pstIrSampleCtx->achIsp1DumpFname, "wb");
            if (pstIrSampleCtx->pIsp1Fp == NULL) {
                ar_err("open %s failed", pstIrSampleCtx->achIsp1DumpFname);
            }
        }

        if (pstIrSampleCtx->pIsp1Fp) {
            pthread_mutex_unlock(&pstIrSampleCtx->irIsp1ProcLock);

            AR_VOID *pData = (AR_VOID *)pstFrameInfo->stVFrame.u64VirAddr[0];
            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->bIsp1DumpNoDummy) {
                wsize = fwrite(pData, 1,
                    pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height,
                    pstIrSampleCtx->pIsp1Fp);
                if (wsize != pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height) {
                    ar_err("write ir isp1 output to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->achIsp1DumpFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrIspRoi.s32Y * pstFrameInfo->stVFrame.u32Stride[0]
                    + pstIrSampleCtx->stIrIspRoi.s32X);

                for (i = 0; i < pstIrSampleCtx->stIrIspRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrIspRoi.u32Width, pstIrSampleCtx->pIsp1Fp);
                    if (wsize != pstIrSampleCtx->stIrIspRoi.u32Width) {
                        ar_err("write ir isp1 output line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->achIsp1DumpFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += pstFrameInfo->stVFrame.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irIsp1ProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir isp1 output image to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->achIsp1DumpFname, wsize);
                fclose(pstIrSampleCtx->pIsp1Fp);
                pstIrSampleCtx->pIsp1Fp = NULL;
            } else{
                //printf("dump ir isp1 output image frame[%u]\n", pstIrSampleCtx->s32IrIsp1DumpFrms);
                pstIrSampleCtx->s32IrIsp1DumpFrms--;
            }

            if (pstIrSampleCtx->s32IrIsp1DumpFrms == 0) {
                printf("dump ir isp1 output image to file[%s] finished\n", pstIrSampleCtx->achIsp1DumpFname);
                fclose(pstIrSampleCtx->pIsp1Fp);
                pstIrSampleCtx->pIsp1Fp = NULL;
            }
        }
    }

    pthread_mutex_unlock(&pstIrSampleCtx->irIsp1ProcLock);

    if (pstIrViGrpCfg->s32IrIsp1Idx >= 0 && pstIrViGrpCfg->s32IrIsp2Idx < 0 && bIrDisplayEn) {
        VIDEO_FRAME_INFO_S stFrameInfoTemp = *pstFrameInfo;
        if (stFrameInfoTemp.stVFrame.u32Width == 408) {
            stFrameInfoTemp.stVFrame.u32Width = 400;
        }
        //send_to_fw_display(&g_vo_obj, 0, &stFrameInfoTemp);
        if(!bEnableFusion){
            AR_MPI_VO_SendFrame(0, 0, &stFrameInfoTemp, -1);
            }
        else{
            ColorMapHandle(&stFrameInfoTemp);
            AR_MPI_VO_SendFrame(0, 0, &stFrameInfoTemp, -1);
            AR_MPI_VPSS_SendFrame(0, 0, &stFrameInfoTemp, -1);
        }
        
    }

    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32Isp1FrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return 0;
}

AR_S32 IrIsp1DeqHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32Isp1DeqFrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return 0;
}

static AR_S32 IrIsp2OutHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    // dump ir_isp2 image
    pthread_mutex_lock(&pstIrSampleCtx->irIsp2ProcLock);

    if (pstIrSampleCtx->s32IrIsp2DumpFrms > 0) {
        if (pstIrSampleCtx->pIsp2Fp == NULL) {
            pstIrSampleCtx->pIsp2Fp = fopen(pstIrSampleCtx->achIsp2DumpFname, "wb");
            if (pstIrSampleCtx->pIsp2Fp == NULL) {
                ar_err("open %s failed", pstIrSampleCtx->achIsp2DumpFname);
            }
        }

        if (pstIrSampleCtx->pIsp2Fp) {
            pthread_mutex_unlock(&pstIrSampleCtx->irIsp2ProcLock);

            AR_VOID *pData = (AR_VOID *)pstFrameInfo->stVFrame.u64VirAddr[0];
            size_t wsize = 0;
            AR_BOOL bWriteSuccess = AR_TRUE;
            AR_U32 i = 0;

            if (!pstIrSampleCtx->bIsp2DumpNoDummy) {
                wsize = fwrite(pData, 1,
                    pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height,
                    pstIrSampleCtx->pIsp2Fp);
                if (wsize != pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height) {
                    ar_err("write ir isp2 output to file[%s] failed, wsize[%u]\n",
                        pstIrSampleCtx->achIsp2DumpFname, wsize);
                    bWriteSuccess = AR_FALSE;
                }
            } else {
                pData += (pstIrSampleCtx->stIrIspRoi.s32Y * pstFrameInfo->stVFrame.u32Stride[0]
                    + pstIrSampleCtx->stIrIspRoi.s32X);

                for (i = 0; i < pstIrSampleCtx->stIrIspRoi.u32Height; i++) {
                    wsize = fwrite(pData, 1, pstIrSampleCtx->stIrIspRoi.u32Width, pstIrSampleCtx->pIsp2Fp);
                    if (wsize != pstIrSampleCtx->stIrIspRoi.u32Width) {
                        ar_err("write ir isp2 output line[%u] to file[%s] failed, wsize[%u]\n",
                            i, pstIrSampleCtx->achIsp2DumpFname, wsize);
                        bWriteSuccess = AR_FALSE;
                        break;
                    }
                    pData += pstFrameInfo->stVFrame.u32Stride[0];
                }
            }

            pthread_mutex_lock(&pstIrSampleCtx->irIsp2ProcLock);

            if (!bWriteSuccess) {
                ar_err("write ir isp2 output image to file[%s] failed, wsize[%u]\n",
                    pstIrSampleCtx->achIsp2DumpFname, wsize);
                fclose(pstIrSampleCtx->pIsp2Fp);
                pstIrSampleCtx->pIsp2Fp = NULL;
            } else{
                //printf("dump ir isp2 output image frame[%u]\n", pstIrSampleCtx->s32IrIsp2DumpFrms);
                pstIrSampleCtx->s32IrIsp2DumpFrms--;
            }

            if (pstIrSampleCtx->s32IrIsp2DumpFrms == 0) {
                printf("dump ir isp2 output image to file[%s] finished\n", pstIrSampleCtx->achIsp2DumpFname);
                fclose(pstIrSampleCtx->pIsp2Fp);
                pstIrSampleCtx->pIsp2Fp = NULL;
            }
        }
    }

    pthread_mutex_unlock(&pstIrSampleCtx->irIsp2ProcLock);

    if (pstIrViGrpCfg->s32IrIsp2Idx >= 0 && bIrDisplayEn) {
        VIDEO_FRAME_INFO_S stFrameInfoTemp = *pstFrameInfo;
        if (stFrameInfoTemp.stVFrame.u32Width == 408) {
            stFrameInfoTemp.stVFrame.u32Width = 400;
        }
        //send_to_fw_display(&g_vo_obj, 0, &stFrameInfoTemp);
        AR_MPI_VO_SendFrame(0, 0, &stFrameInfoTemp, -1);
    }

    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32Isp2FrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return 0;
}

AR_S32 IrIsp2DeqHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)pPrivData;
    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
    pstIrSampleCtx->u32Isp2DeqFrmCnt++;
    pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
    return 0;
}

static AR_S32 VisibleLightOutHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    //send_to_fw_display(&g_vo_obj, 1, pstFrameInfo);
    AR_MPI_VO_SendFrame(0, 1, pstFrameInfo, -1);
    return 0;
}

AR_S32 FusionOutHandle(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData)
{
    AR_MPI_VO_SendFrame(0, 3, pstFrameInfo, -1);
    //send_to_fw_display(&g_vo_obj, 3, &visFrame);
    //int ret = send_to_fw_display(&g_vo_obj, 3, pstFrameInfo);
    //if(ret) ar_err("send vo");
    return 0;
}

static IR_SAMPLE_CTX_S *FindIrSampleCtx(AR_CHAR *strSensor)
{
    IR_SAMPLE_CTX_S *pstIrSampleCtx = NULL;
    AR_CHAR achSns1Name[16] = {'\0'};
    AR_CHAR achSns2Name[16] = {'\0'};
    AR_CHAR *pchMid = NULL;
    AR_S32 s32IrSnsIdx = 0;
    AR_U32 i = 0;

    pchMid = strchr(strSensor, '+');

    if (pchMid) {
        *pchMid = '\0';
        strcpy(achSns1Name, strSensor);
        strcpy(achSns2Name, (pchMid + 1));
    } else {
        strcpy(achSns1Name, strSensor);
    }

    printf("ir_sample sensor: %s %s\n", achSns1Name, achSns2Name); 

    for (i = 0; i < sizeof(astIrSnsList) / sizeof(IR_SNS_ENTRY_S); i++) {
        if (strcmp(achSns1Name, astIrSnsList[i].strSnsName) == 0) {
            pstIrSampleCtx = astIrSnsList[i].pstIrSampleCtx;
            s32IrSnsIdx = 1;
            break;
        }
    }

    if (pstIrSampleCtx == NULL) {
        for (i = 0; i < sizeof(astIrSnsList) / sizeof(IR_SNS_ENTRY_S); i++) {
            if (strcmp(achSns2Name, astIrSnsList[i].strSnsName) == 0) {
                pstIrSampleCtx = astIrSnsList[i].pstIrSampleCtx;
                s32IrSnsIdx = 2;
                break;
            }
        }
    }

    if (pstIrSampleCtx) {
        pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx = -1;

        if (s32IrSnsIdx == 1) {
            for (i = 0; i < sizeof(astVlSnsList) / sizeof(VL_SNS_ENTRY_S); i++) {
                if (strcmp(achSns2Name, astVlSnsList[i].strSnsName) == 0) {
                    pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx = 0;
                    pstIrSampleCtx->stIrViGrpCfg.aenSnsType[0] = astVlSnsList[i].enSnsType;
                    pstIrSampleCtx->stIrViGrpCfg.as32BusId[0] = astVlSnsList[i].s32BusId;
                    pstIrSampleCtx->stIrViGrpCfg.astInSize[0] = astVlSnsList[i].stInSize;
                    pstIrSampleCtx->stIrViGrpCfg.astOutSize[0] = astVlSnsList[i].stOutSize;
                    break;
                }
            }
        } else {
            for (i = 0; i < sizeof(astVlSnsList) / sizeof(VL_SNS_ENTRY_S); i++) {
                if (strcmp(achSns1Name, astVlSnsList[i].strSnsName) == 0) {
                    pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx = 0;
                    pstIrSampleCtx->stIrViGrpCfg.aenSnsType[0] = astVlSnsList[i].enSnsType;
                    pstIrSampleCtx->stIrViGrpCfg.as32BusId[0] = astVlSnsList[i].s32BusId;
                    pstIrSampleCtx->stIrViGrpCfg.astInSize[0] = astVlSnsList[i].stInSize;
                    pstIrSampleCtx->stIrViGrpCfg.astOutSize[0] = astVlSnsList[i].stOutSize;
                    break;
                }
            }
        }
    }

    return pstIrSampleCtx;
}

static AR_S32 IrCmdGetIrOccParam(IR_SAMPLE_CTX_S *pstIrSampleCtx, AR_CHAR *strOccFname)
{
    AR_S32 s32Ret = 0;
    AR_U64 u64OccOrigPhy = 0;
    AR_VOID *pOccOrigVirt = NULL;
    SIZE_S stOccSize = pstIrSampleCtx->stIrViGrpCfg.astInSize[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
    AR_U32 u32OccStride = CAM_ALIGNE_TO(stOccSize.u32Width, 256);
    AR_U32 u32OccBufSize = u32OccStride * stOccSize.u32Height;
    ISP_IR_OCC_PARAM_S stIrOccParam;
    ISP_IR_PARAMS_S stIrParams;
    FILE *fp = NULL;
    size_t wsize = 0;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&u64OccOrigPhy, &pOccOrigVirt, NULL, NULL, u32OccBufSize + 256);
    if (s32Ret) {
        ar_err("allocate occ buf failed");
        return s32Ret;
    }

    stIrOccParam.stOccBuf.u32Width = stOccSize.u32Width;
    stIrOccParam.stOccBuf.u32Height = stOccSize.u32Height;
    stIrOccParam.stOccBuf.u32Stride[0] = u32OccStride;
    stIrOccParam.stOccBuf.u64PhyAddr[0] = CAM_ALIGNE_TO(u64OccOrigPhy, 256);
    stIrOccParam.stOccBuf.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pOccOrigVirt, 256);

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)&stIrOccParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(stIrOccParam);

    s32Ret = AR_MPI_ISP_GetIrParams(pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx],
        &stIrParams);
    if (s32Ret) {
        ar_err("get occ param failed");
        goto EXIT1;
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(u64OccOrigPhy, pOccOrigVirt, u32OccBufSize + 256);
    if (s32Ret) {
        ar_err("flush occ buf failed");
        goto EXIT1;
    }

    fp = fopen(strOccFname, "wb");
    if (fp == NULL) {
        ar_err("open %s failed", strOccFname);
        s32Ret = -1;
        goto EXIT1;
    }

    wsize = fwrite((AR_VOID *)stIrOccParam.stOccBuf.u64VirAddr[0], 1, u32OccBufSize, fp);
    if (wsize != u32OccBufSize) {
        ar_err("write occ data to file %s failed, wsize[%u]", strOccFname, wsize);
        s32Ret = -1;
        goto EXIT2;
    }

    printf("get occ param successfully, occ data is saved to file %s\n", strOccFname);

EXIT2:
    fclose(fp);
EXIT1:
    AR_MPI_SYS_MmzFree(u64OccOrigPhy, pOccOrigVirt);
    return s32Ret;
}

static AR_S32 IrCmdGetIrKbParam(IR_SAMPLE_CTX_S *pstIrSampleCtx, AR_CHAR *strKbFname)
{
    AR_S32 s32Ret = 0;
    VI_PIPE IrIspPipe = -1;
    AR_U64 u64KbOrigPhy = 0;
    AR_VOID *pKbOrigVirt = NULL;
    SIZE_S stKbSize;
    AR_U32 u32KbStride = 0;
    AR_U32 u32KbBufSize = 0;
    ISP_IR_KB_PARAM_S stIrKbParam;
    ISP_IR_PARAMS_S stIrParams;
    FILE *fp = NULL;
    size_t wsize = 0;

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
        IrIspPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
        stKbSize = pstIrSampleCtx->stIrViGrpCfg.astOutSize[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
    } else {
        IrIspPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
        stKbSize = pstIrSampleCtx->stIrViGrpCfg.astOutSize[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
    }

    u32KbStride = CAM_ALIGNE_TO(stKbSize.u32Width * 2, 16);
    u32KbBufSize = u32KbStride * stKbSize.u32Height * 2;

    s32Ret = AR_MPI_SYS_MmzAlloc_Cached(&u64KbOrigPhy, &pKbOrigVirt, NULL, NULL, u32KbBufSize + 256);
    if (s32Ret) {
        ar_err("allocate kb buf failed");
        return s32Ret;
    }

    stIrKbParam.stKbBuf.u32Width = stKbSize.u32Width;
    stIrKbParam.stKbBuf.u32Height = stKbSize.u32Height;
    stIrKbParam.stKbBuf.u32Stride[0] = u32KbStride;
    stIrKbParam.stKbBuf.u64PhyAddr[0] = CAM_ALIGNE_TO(u64KbOrigPhy, 256);
    stIrKbParam.stKbBuf.u64VirAddr[0] = CAM_ALIGNE_TO((AR_U64)pKbOrigVirt, 256);
    stIrKbParam.stKbBuf.u32Stride[1] = u32KbStride;
    stIrKbParam.stKbBuf.u64PhyAddr[1] = stIrKbParam.stKbBuf.u64PhyAddr[0] + u32KbBufSize / 2;
    stIrKbParam.stKbBuf.u64VirAddr[1] = stIrKbParam.stKbBuf.u64VirAddr[0] + u32KbBufSize / 2;

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_KB;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)&stIrKbParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(stIrKbParam);

    s32Ret = AR_MPI_ISP_GetIrParams(IrIspPipe, &stIrParams);
    if (s32Ret) {
        ar_err("get kb param failed");
        goto EXIT1;
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(u64KbOrigPhy, pKbOrigVirt, u32KbBufSize + 256);
    if (s32Ret) {
        ar_err("flush kb buf failed");
        goto EXIT1;
    }

    fp = fopen(strKbFname, "wb");
    if (fp == NULL) {
        ar_err("open %s failed", strKbFname);
        s32Ret = -1;
        goto EXIT1;
    }

    wsize = fwrite((AR_VOID *)stIrKbParam.stKbBuf.u64VirAddr[0], 1, u32KbBufSize, fp);
    if (wsize != u32KbBufSize) {
        ar_err("write kb data to file %s failed, wsize[%u]", strKbFname, wsize);
        s32Ret = -1;
        goto EXIT2;
    }

    printf("get kb param successfully, kb data is saved to file %s, kb_en[%d] dpc_en[%d]\n",
        strKbFname, stIrKbParam.bKbcEn, stIrKbParam.bDpcEn);

EXIT2:
    fclose(fp);
EXIT1:
    AR_MPI_SYS_MmzFree(u64KbOrigPhy, pKbOrigVirt);
    return s32Ret;
}

static AR_S32 IrCmdGetIrGtmParam(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    VI_PIPE IrIspPipe = -1;
    ISP_IR_GTM_PARAM_S stIrGtmParam;
    ISP_IR_PARAMS_S stIrParams;

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
        IrIspPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
    } else {
        IrIspPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_GTM;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)&stIrGtmParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(stIrGtmParam);

    s32Ret = AR_MPI_ISP_GetIrParams(IrIspPipe, &stIrParams);
    if (s32Ret) {
        ar_err("get gtm param failed");
        goto EXIT;
    }

    printf("get gtm param successfully, contrast[%d] bright[%d]\n",
        stIrGtmParam.u32GtmContrast, stIrGtmParam.s32GtmBright);

EXIT:
    return s32Ret;
}

static AR_S32 IrCmdSetIrGtmParam(IR_SAMPLE_CTX_S *pstIrSampleCtx,
    AR_U32 u32Contrast, AR_S32 s32Bright)
{
    AR_S32 s32Ret = 0;
    VI_PIPE IrIspPipe = -1;
    ISP_IR_GTM_PARAM_S stIrGtmParam;
    ISP_IR_PARAMS_S stIrParams;

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
        IrIspPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
    } else {
        IrIspPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
    }

    stIrGtmParam.u32GtmContrast = u32Contrast;
    stIrGtmParam.s32GtmBright = s32Bright;

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_GTM;
    stIrParams.astParamEntry[0].pParamData = (AR_VOID *)&stIrGtmParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(stIrGtmParam);

    s32Ret = AR_MPI_ISP_SetIrParams(IrIspPipe, &stIrParams);
    if (s32Ret) {
        ar_err("get gtm param failed");
        goto EXIT;
    }

    printf("set gtm param successfully, contrast[%d] bright[%d]\n",
        stIrGtmParam.u32GtmContrast, stIrGtmParam.s32GtmBright);

EXIT:
    return s32Ret;
}

static AR_VOID *IrSnsCalibThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)arg;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrRawIdx];
    IR_SNS_CALIB_RESULT_S stSnsCalibResult;
    FILE *fp = NULL;
    AR_U32 wsize = 0;

    s32Ret = AR_MPI_ISP_StartIrSnsCalib(ViPipe, &pstIrSampleCtx->stIrSnsCalibCfg);
    if (s32Ret) {
        ar_err("start ir sensor calibration failed");
        goto EXIT;
    }

    stSnsCalibResult.pCalibParam = pstIrSampleCtx->pstIrParamCtx->pSnsParam;

    while (1) {
        s32Ret = AR_MPI_ISP_GetIrSnsCalibResult(ViPipe, &stSnsCalibResult);
        if (s32Ret) {
            ar_err("get ir sensor calibration result failed");
            goto EXIT;
        } else {
            if (stSnsCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                break;
            } else if (stSnsCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                printf("ir sensor calibration cancelled\n");
                goto EXIT;
            } else if (stSnsCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                ar_err("ir sensor calibration failed");
                goto EXIT;
            }
        }

        usleep(100 * 1000);
    }

    if (pstIrSampleCtx->achIrSnsCalibFname[0] != '\0') {
        fp = fopen(pstIrSampleCtx->achIrSnsCalibFname, "wb");
        if (fp == NULL) {
            ar_err("open file[%s] failed", pstIrSampleCtx->achIrSnsCalibFname);
            goto EXIT;
        }

        wsize = fwrite(stSnsCalibResult.pCalibParam, 1, pstIrSampleCtx->u32IrSnsParamSize, fp);
        if (wsize != pstIrSampleCtx->u32IrSnsParamSize) {
            ar_err("write sensor ir param to file[%s] failed", pstIrSampleCtx->achIrSnsCalibFname);
        } else {
            printf("ir sensor calibration success\n");
        }
    } else {
        printf("ir sensor calibration success\n");
    }

EXIT:
    if (fp) {
        fclose(fp);
    }
    return NULL;
}

static AR_S32 IrCmdCancelSnsCalib(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrRawIdx];

    s32Ret = AR_MPI_ISP_CancelIrSnsCalib(ViPipe);
    if (s32Ret) {
        ar_err("cancel ir sensor calibration failed");
    }

    return s32Ret;
}

static AR_VOID *IrOccCalibThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)arg;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrRawIdx];
    IR_OCC_CALIB_RESULT_S stOccCalibResult;
    FILE *fp = NULL;
    AR_U32 wsize = 0;

    s32Ret = AR_MPI_ISP_StartIrOccCalib(ViPipe, &pstIrSampleCtx->stIrOccCalibCfg);
    if (s32Ret) {
        ar_err("start ir occ calibration failed");
        goto EXIT;
    }

    stOccCalibResult.stOccBuf = pstIrSampleCtx->pstIrParamCtx->stOccParam.stOccBuf;

    while (1) {
        s32Ret = AR_MPI_ISP_GetIrOccCalibResult(ViPipe, &stOccCalibResult);
        if (s32Ret) {
            ar_err("get ir occ calibration result failed");
            goto EXIT;
        } else {
            if (stOccCalibResult.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
                break;
            } else if (stOccCalibResult.enCalibStatus == IR_CALIB_STATUS_CANCELLED) {
                printf("ir occ calibration cancelled\n");
                goto EXIT;
            } else if (stOccCalibResult.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
                ar_err("ir occ calibration failed");
                goto EXIT;
            }
        }

        usleep(100 * 1000);
    }

    if (pstIrSampleCtx->achIrOccCalibFname[0] != '\0' && stOccCalibResult.stOccBuf.u64VirAddr[0]) {
        fp = fopen(pstIrSampleCtx->achIrOccCalibFname, "wb");
        if (fp == NULL) {
            ar_err("open file[%s] failed", pstIrSampleCtx->achIrOccCalibFname);
            goto EXIT;
        }

        wsize = fwrite((AR_VOID *)stOccCalibResult.stOccBuf.u64VirAddr[0], 1,
            stOccCalibResult.stOccBuf.u32Stride[0] * stOccCalibResult.stOccBuf.u32Height, fp);
        if (wsize != stOccCalibResult.stOccBuf.u32Stride[0] * stOccCalibResult.stOccBuf.u32Height) {
            ar_err("write ir occ data to file[%s] failed", pstIrSampleCtx->achIrOccCalibFname);
        } else {
            printf("ir occ calibration success\n");
        }
    } else {
        printf("ir occ calibration success\n");
    }

EXIT:
    if (fp) {
        fclose(fp);
    }
    return NULL;
}

static AR_S32 IrCmdCancelOccCalib(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrRawIdx];

    s32Ret = AR_MPI_ISP_CancelIrOccCalib(ViPipe);
    if (s32Ret) {
        ar_err("cancel ir occ calibration failed");
    }

    return s32Ret;
}

static AR_VOID *IrDynamicCalibThread(AR_VOID *arg)
{
    AR_S32 s32Ret = 0;
    IR_SAMPLE_CTX_S *pstIrSampleCtx = (IR_SAMPLE_CTX_S *)arg;
    struct timeval stCurTime;

    pthread_setname_np(pthread_self(), "ir_dyn_calib");

    ar_always("ir dynamic calib thread start");

    gettimeofday(&pstIrSampleCtx->stLastIrDynamicCalibTime, NULL);

    while (pstIrSampleCtx->bIrDynCalibThrdStart) {
        gettimeofday(&stCurTime, NULL);

        if (get_time_interval_us(pstIrSampleCtx->stLastIrDynamicCalibTime, stCurTime) >= pstIrSampleCtx->u32AutoCalibIntervalMs * 1000
            || bFirstIrDynamicCalib || pstIrSampleCtx->bForceIrDynamicCalib) {
            pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
            if (pstIrSampleCtx->bAutoCalib && !pstIrSampleCtx->bIrSuspend && !pstIrSampleCtx->bIrDynamicCalibBusy) {
                pstIrSampleCtx->bIrDynamicCalibBusy = AR_TRUE;
            }
            pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

            if (pstIrSampleCtx->bIrDynamicCalibBusy) {
                s32Ret = IrDynamicCalib(pstIrSampleCtx,
                    (bFirstIrDynamicCalib ? pstIrSampleCtx->enFirstIrDynamicCalibType : pstIrSampleCtx->enIrDynamicCalibType),
                    AR_TRUE);
                if (s32Ret) {
                    ar_err("ir dynamic calib failed");
                    s32Ret = 0;
                }

                pstIrSampleCtx->stLastIrDynamicCalibTime = stCurTime;
            }

            pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
            pstIrSampleCtx->bIrDynamicCalibBusy = AR_FALSE;
            if (bFirstIrDynamicCalib) {
                bIrDisplayEn = AR_TRUE;
            }
            bFirstIrDynamicCalib = AR_FALSE;
            pstIrSampleCtx->bForceIrDynamicCalib = AR_FALSE;
            pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);
        }

        sleep(1);
    }

    ar_always("ir dynamic calib thread end");
    return NULL;
}

static AR_S32 IrCmdCancelBCalib(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    VI_PIPE ViPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx];

    s32Ret = AR_MPI_ISP_CancelIrBCalib(ViPipe);
    if (s32Ret) {
        ar_err("cancel ir b calibration failed");
    }

    return s32Ret;
}

static AR_S32 StartIr(IR_SAMPLE_CTX_S *pstIrSampleCtx, AR_BOOL bAllowIrDynCalib)
{
    AR_S32 s32Ret = 0;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;
    IR_PARAM_CTX_CFG_S stIrParamCtxCfg;
    IR_PARAM_CTX_S *pstIrParamCtx = NULL;

    if (pstIrSampleCtx->bIrStarted) {
        ar_always("ir has been started");
        return 0;
    }

    if (!bVoChnEn[0]) {
        AR_MPI_VO_EnableChn(0, 0);
        bVoChnEn[0] = AR_TRUE;
    }

    if (pstIrViGrpCfg->s32IrRawIdx >= 0) {
        stIrParamCtxCfg.IrSnsPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrRawIdx];
        stIrParamCtxCfg.stIrSnsSize = pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrRawIdx];
        stIrParamCtxCfg.stIrSnsRoi = pstIrSampleCtx->stIrSnsRoi;
        stIrParamCtxCfg.u32IrSnsParamSize = pstIrSampleCtx->u32IrSnsParamSize;
    } else {
        stIrParamCtxCfg.IrSnsPipe = -1;
    }

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
        stIrParamCtxCfg.IrIspPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx];
        stIrParamCtxCfg.stIrIspSize = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrThrmlIdx];
        stIrParamCtxCfg.stIrIspRoi = pstIrSampleCtx->stIrIspRoi;
    } else if (pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        stIrParamCtxCfg.IrIspPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx];
        stIrParamCtxCfg.stIrIspSize = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp1Idx];
        stIrParamCtxCfg.stIrIspRoi = pstIrSampleCtx->stIrIspRoi;
    } else {
        stIrParamCtxCfg.IrIspPipe = -1;
    }

    pstIrSampleCtx->pstIrParamCtx = CreateIrParamCtx(&stIrParamCtxCfg);
    if (pstIrSampleCtx->pstIrParamCtx == NULL) {
        ar_err("create ir param ctx failed");
        s32Ret = -1;
        goto FAIL1;
    }
    pstIrParamCtx = pstIrSampleCtx->pstIrParamCtx;

    if (pstIrViGrpCfg->s32IrRawIdx >= 0) {
        if (pstIrSampleCtx->strInitSnsParamCalibFname) {
            s32Ret = LoadSnsParamCalibData(pstIrParamCtx, pstIrSampleCtx->strInitSnsParamCalibFname);
            if (s32Ret) {
                ar_err("load sensor calib data [%s] failed", pstIrSampleCtx->strInitSnsParamCalibFname);
                goto FAIL2;
            }
        }

        if (pstIrSampleCtx->strInitOccCalibFname) {
            s32Ret = LoadOccCalibData(pstIrParamCtx, pstIrSampleCtx->strInitOccCalibFname);
            if (s32Ret) {
                ar_err("load occ calib data [%s] failed", pstIrSampleCtx->strInitOccCalibFname);
                goto FAIL2;
            }
        }
    }

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0 || pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        if (pstIrSampleCtx->strInitKbCalibFname) {
            s32Ret = LoadKbCalibData(pstIrParamCtx, pstIrSampleCtx->strInitKbCalibFname);
            if (s32Ret) {
                ar_err("load kb calib data [%s] failed", pstIrSampleCtx->strInitKbCalibFname);
                goto FAIL2;
            }
        }
    }

    if (pstIrViGrpCfg->s32IrRawIdx >= 0) {
        s32Ret = CreateRawTempBuf(pstIrSampleCtx);
        if (s32Ret) {
            ar_err("create raw temp buf failed");
            goto FAIL2;
        }
    }

    if (pstIrViGrpCfg->s32IrThrmlIdx >= 0 || pstIrViGrpCfg->s32IrIsp1Idx >= 0) {
        s32Ret = CreateThrmlTempBuf(pstIrSampleCtx);
        if (s32Ret) {
            ar_err("create thermal temp buf failed");
            goto FAIL3;
        }

        s32Ret = Create3DnrTempBuf(pstIrSampleCtx);
        if (s32Ret) {
            ar_err("create ir 3dnr temp buf failed");
            goto FAIL4;
        }

        s32Ret = Create2DnrTempBuf(pstIrSampleCtx);
        if (s32Ret) {
            ar_err("create ir 2dnr temp buf failed");
            goto FAIL5;
        }
    }

    s32Ret = StartIrViGroupIrOnly(pstIrSampleCtx->pstIrViGrp);
    if (s32Ret) {
        ar_err("start ir failed");
        goto FAIL6;
    }

    s32Ret = ApplyAllIrParam(pstIrParamCtx);
    if (s32Ret) {
        ar_err("apply all ir param failed");
        s32Ret = 0;
    }
    bIrParamInited = AR_TRUE;

    if (bAllowIrDynCalib) {
        pstIrSampleCtx->bIrDynCalibThrdStart = AR_TRUE;
        s32Ret = pthread_create(&pstIrSampleCtx->irDynCalibThrd, NULL, IrDynamicCalibThread, pstIrSampleCtx);
        if (s32Ret) {
            ar_err("create ir dynamic calib thread failed");
            pstIrSampleCtx->bIrDynCalibThrdStart = AR_FALSE;
            goto FAIL7;
        }
    }

    pstIrSampleCtx->bIrStarted = AR_TRUE;
    return 0;

FAIL7:
    StopIrViGroupIrOnly(pstIrSampleCtx->pstIrViGrp);
FAIL6:
    if (pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy, pstIrSampleCtx->pIr2DnrTempBufOrigVirt);
    }
FAIL5:
    if (pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy, pstIrSampleCtx->pIr3DnrTempBufOrigVirt);
    }
FAIL4:
    if (pstIrSampleCtx->u64ThrmlTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64ThrmlTempBufOrigPhy, pstIrSampleCtx->pThrmlTempBufOrigVirt);
    }
FAIL3:
    if (pstIrSampleCtx->u64RawTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64RawTempBufOrigPhy, pstIrSampleCtx->pRawTempBufOrigVirt);
    }
FAIL2:
    DestroyIrParamCtx(pstIrParamCtx);
FAIL1:
    return s32Ret;
}

static AR_S32 StopIr(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    if (!pstIrSampleCtx->bIrStarted) {
        ar_always("ir has been stopped");
        return 0;
    }

    if (pstIrSampleCtx->bIrDynCalibThrdStart) {
        pstIrSampleCtx->bIrDynCalibThrdStart = AR_FALSE;
        pthread_join(pstIrSampleCtx->irDynCalibThrd, NULL);
    }

    StopIrViGroupIrOnly(pstIrSampleCtx->pstIrViGrp);

    if (pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64Ir2DnrTempBufOrigPhy, pstIrSampleCtx->pIr2DnrTempBufOrigVirt);
    }

    if (pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64Ir3DnrTempBufOrigPhy, pstIrSampleCtx->pIr3DnrTempBufOrigVirt);
    }

    if (pstIrSampleCtx->u64ThrmlTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64ThrmlTempBufOrigPhy, pstIrSampleCtx->pThrmlTempBufOrigVirt);
    }

    if (pstIrSampleCtx->u64RawTempBufOrigPhy) {
        AR_MPI_SYS_MmzFree(pstIrSampleCtx->u64RawTempBufOrigPhy, pstIrSampleCtx->pRawTempBufOrigVirt);
    }

    DestroyIrParamCtx(pstIrSampleCtx->pstIrParamCtx);

    if (bVoChnEn[0]) {
        AR_MPI_VO_DisableChn(0, 0);
        bVoChnEn[0] = AR_FALSE;
    }

    bIrDisplayEn = AR_FALSE;
    bFirstIrDynamicCalib = AR_TRUE;
    pstIrSampleCtx->bForceIrDynamicCalib = AR_FALSE;

    pstIrSampleCtx->bIrStarted = AR_FALSE;
    return 0;
}

static AR_S32 StartVl(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;

    if (pstIrSampleCtx->bVlStarted) {
        ar_always("visible light has been started");
        return 0;
    }

    if (!bVoChnEn[1]) {
        AR_MPI_VO_EnableChn(0, 1);
        bVoChnEn[1] = AR_TRUE;
    }

    s32Ret = StartIrViGroupVlOnly(pstIrSampleCtx->pstIrViGrp);
    if (s32Ret) {
        ar_err("start visible light failed");
    } else {
        pstIrSampleCtx->bVlStarted = AR_TRUE;
    }

    return s32Ret;
}

static AR_S32 StopVl(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    if (!pstIrSampleCtx->bVlStarted) {
        ar_always("visible light has been stopped");
        return 0;
    }

    StopIrViGroupVlOnly(pstIrSampleCtx->pstIrViGrp);

    if (bVoChnEn[1]) {
        AR_MPI_VO_DisableChn(0, 1);
        bVoChnEn[1] = AR_FALSE;
    }

    pstIrSampleCtx->bVlStarted = AR_FALSE;
    return 0;
}

static AR_S32 SuspendIr(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    ISP_SUS_MODE_E enMode = ISP_SUS_DEEP_MODE;

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
    pstIrSampleCtx->bIrSuspend = AR_TRUE;

    if (pstIrSampleCtx->bIrDynamicCalibBusy) {
        VI_PIPE rawPipe = -1;
        VI_PIPE thrmlPipe = -1;

        if (pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx >= 0) {
            rawPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
        }

        if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
            thrmlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
        } else if (pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx >= 0) {
            thrmlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
        }

        if (rawPipe >= 0) {
            s32Ret = AR_MPI_ISP_CancelIrSnsCalib(rawPipe);
            if (s32Ret) {
                ar_err("cancel ir sensor calibration failed");
                s32Ret = 0;
            }

            s32Ret = AR_MPI_ISP_CancelIrOccCalib(rawPipe);
            if (s32Ret) {
                ar_err("cancel ir occ calibration failed");
                s32Ret = 0;
            }
        }

        if (thrmlPipe >= 0) {
            s32Ret = AR_MPI_ISP_CancelIrBCalib(thrmlPipe);
            if (s32Ret) {
                ar_err("cancel ir b calibration failed");
                s32Ret = 0;
            }
        }
    }
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx >= 0) {
        VI_PIPE rawPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
        VI_CHN rawChn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];

        s32Ret = AR_MPI_VI_PauseChn(rawPipe, rawChn, enMode);
        if (s32Ret) {
            ar_err("suspend ir raw chn failed");
            goto EXIT;
        } else {
            printf("suspend ir raw chn successful\n");
        }
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
        VI_PIPE thrmlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
        VI_CHN thrmlChn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];

        s32Ret = AR_MPI_VI_PauseChn(thrmlPipe, thrmlChn, enMode);
        if (s32Ret) {
            ar_err("suspend ir thermal chn failed");
            goto EXIT;
        } else {
            printf("suspend ir thermal chn successful\n");
        }
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx >= 0) {
        VI_PIPE isp1Pipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
        VI_CHN isp1Chn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];

        s32Ret = AR_MPI_VI_PauseChn(isp1Pipe, isp1Chn, enMode);
        if (s32Ret) {
            ar_err("suspend ir isp1 chn failed");
            goto EXIT;
        } else {
            printf("suspend ir isp1 chn successful\n");
        }
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrIsp2Idx >= 0) {
        VI_PIPE isp2Pipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp2Idx];
        VI_CHN isp2Chn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp2Idx];

        s32Ret = AR_MPI_VI_PauseChn(isp2Pipe, isp2Chn, enMode);
        if (s32Ret) {
            ar_err("suspend ir isp2 chn failed");
            goto EXIT;
        } else {
            printf("suspend ir isp2 chn successful\n");
        }
    }

EXIT:
    return s32Ret;
}

AR_S32 ResumeIr(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    ISP_SUS_MODE_E enMode = ISP_SUS_DEEP_MODE;

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrIsp2Idx >= 0) {
        VI_PIPE isp2Pipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp2Idx];
        VI_CHN isp2Chn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp2Idx];
    
        s32Ret = AR_MPI_VI_RecoverChn(isp2Pipe, isp2Chn, enMode);
        if (s32Ret) {
            ar_err("resume ir isp2 chn failed");
            goto EXIT;
        } else {
            printf("resume ir isp2 chn successful\n");
        }
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx >= 0) {
        VI_PIPE isp1Pipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
        VI_CHN isp1Chn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrIsp1Idx];
    
        s32Ret = AR_MPI_VI_RecoverChn(isp1Pipe, isp1Chn, enMode);
        if (s32Ret) {
            ar_err("resume ir isp1 chn failed");
            goto EXIT;
        } else {
            printf("resume ir isp1 chn successful\n");
        }
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx >= 0) {
        VI_PIPE thrmlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
        VI_CHN thrmlChn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];

        s32Ret = AR_MPI_VI_RecoverChn(thrmlPipe, thrmlChn, enMode);
        if (s32Ret) {
            ar_err("resume ir thermal chn failed");
            goto EXIT;
        } else {
            printf("resume ir thermal chn successful\n");
        }
    }

    if (pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx >= 0) {
        VI_PIPE rawPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
        VI_CHN rawChn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];

        s32Ret = AR_MPI_VI_RecoverChn(rawPipe, rawChn, enMode);
        if (s32Ret) {
            ar_err("resume ir raw chn failed");
            goto EXIT;
        } else {
            printf("resume ir raw chn successful\n");
        }
    }

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
    if (pstIrSampleCtx->bAutoCalib) {
        pstIrSampleCtx->bIrDynamicCalibBusy = AR_TRUE;
    }
    pstIrSampleCtx->bIrSuspend = AR_FALSE;
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

    if (pstIrSampleCtx->bIrDynamicCalibBusy) {
        s32Ret = IrDynamicCalib(pstIrSampleCtx,
            (bFirstIrDynamicCalib ? pstIrSampleCtx->enFirstIrDynamicCalibType : pstIrSampleCtx->enIrDynamicCalibType),
            AR_TRUE);
        if (s32Ret) {
            ar_err("ir dynamic calib failed");
            s32Ret = 0;
        }

        gettimeofday(&pstIrSampleCtx->stLastIrDynamicCalibTime, NULL);
    }

    pthread_mutex_lock(&pstIrSampleCtx->irRawProcLock);
    pstIrSampleCtx->bIrDynamicCalibBusy = AR_FALSE;
    bFirstIrDynamicCalib = AR_FALSE;
    pstIrSampleCtx->bForceIrDynamicCalib = AR_FALSE;
    pthread_mutex_unlock(&pstIrSampleCtx->irRawProcLock);

EXIT:
    return s32Ret;
}

AR_S32 SuspendVl(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    ISP_SUS_MODE_E enMode = ISP_SUS_DEEP_MODE;

    if (pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx >= 0) {
        VI_PIPE vlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx];
        VI_CHN vlChn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx];

        s32Ret = AR_MPI_VI_PauseChn(vlPipe, vlChn, enMode);
        if (s32Ret) {
            ar_err("suspend visible light chn failed");
        } else {
            printf("suspend visible light chn successful\n");
        }
    }

    return s32Ret;
}

AR_S32 ResumeVl(IR_SAMPLE_CTX_S *pstIrSampleCtx)
{
    AR_S32 s32Ret = 0;
    ISP_SUS_MODE_E enMode = ISP_SUS_DEEP_MODE;

    if (pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx >= 0) {
        VI_PIPE vlPipe = pstIrSampleCtx->stIrViGrpCfg.aViPipe[pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx];
        VI_CHN vlChn = pstIrSampleCtx->stIrViGrpCfg.aViChn[pstIrSampleCtx->stIrViGrpCfg.s32VisibleLightIdx];

        s32Ret = AR_MPI_VI_RecoverChn(vlPipe, vlChn, enMode);
        if (s32Ret) {
            ar_err("resume visible light chn failed");
        } else {
            printf("resume visible light chn successful\n");
        }
    }

    return s32Ret;
}



#define IR_CMD_FILE_NAME        "/usrdata/ir_cmd.bin"

typedef struct {
    uint32_t cmd_cnt;
    char cmd[32][64];
} ir_cmd_t;

static ir_cmd_t ir_cmd_data;
static IR_SAMPLE_CTX_S *g_pstIrSampleCtx = NULL;

static int read_ir_cmd(const char* fname, ir_cmd_t *ir_cmd)
{
    FILE* fp = NULL;
    size_t r_size = 0;

    fp = fopen(fname, "rb");
    if (fp == NULL) {
        ar_err("open %s failed", fname);
        return -1;
    }

    r_size = fread(ir_cmd, 1, sizeof(ir_cmd_t), fp);
    if (r_size != sizeof(ir_cmd_t)) {
        ar_err("read ir cmd from %s failed", fname);
        return -1;
    }

    fclose(fp);
    return 0;
}

static AR_S32 s32SwitchIrVlFlag = 0;

void ir_cmd_signal_handler(int signum)
{
    int ret = 0;

    switch (signum) {
        case SIGUSR1: {
            ret = read_ir_cmd(IR_CMD_FILE_NAME, &ir_cmd_data);
            if (ret) {
                ar_err("read ir cmd failed");
                return;
            }

            if (strcmp(ir_cmd_data.cmd[0], "manual_shutter") == 0)
            {
                ManualShutter(g_pstIrSampleCtx, (atoi(ir_cmd_data.cmd[1]) == 0 ? AR_FALSE : AR_TRUE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_raw") == 0)
            {
                StartIrRawCapture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_thermal") == 0)
            {
                StartIrThrmlCapture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_3dnr") == 0)
            {
                StartIr3DnrCapture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_2dnr") == 0)
            {
                StartIr2DnrCapture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_gtm") == 0)
            {
                StartIrGtmCapture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_isp1") == 0)
            {
                StartIrIsp1Capture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "dump_ir_isp2") == 0)
            {
                StartIrIsp2Capture(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[2]), ir_cmd_data.cmd[1],
                    ((ir_cmd_data.cmd_cnt > 3 && strcmp(ir_cmd_data.cmd[3], "-no_dummy") == 0) ? AR_TRUE : AR_FALSE));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_vtemp") == 0)
            {
                AR_S32 vtemp = 0;
                ret = AR_MPI_ISP_GetSnsTemperature(g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx], &vtemp);
                if (ret) {
                    ar_err("get sensor temperature failed");
                } else {
                    printf("get sensor temperature: %d\n", vtemp);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_sns_calib") == 0)
            {
                if (ir_cmd_data.cmd_cnt > 1 && ir_cmd_data.cmd[1][0] != '\0') {
                    strcpy(g_pstIrSampleCtx->achIrSnsCalibFname, ir_cmd_data.cmd[1]);
                } else {
                    g_pstIrSampleCtx->achIrSnsCalibFname[0] = '\0';
                }
                ret = pthread_create(&g_pstIrSampleCtx->irCalibThrd, NULL, IrSnsCalibThread, g_pstIrSampleCtx);
                if (ret) {
                    ar_err("create ir sensor calib thread failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "cancel_ir_sns_calib") == 0)
            {
                IrCmdCancelSnsCalib(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_occ_calib") == 0)
            {
                if (ir_cmd_data.cmd_cnt > 1 && ir_cmd_data.cmd[1][0] != '\0') {
                    strcpy(g_pstIrSampleCtx->achIrOccCalibFname, ir_cmd_data.cmd[1]);
                } else {
                    g_pstIrSampleCtx->achIrOccCalibFname[0] = '\0';
                }
                ret = pthread_create(&g_pstIrSampleCtx->irCalibThrd, NULL, IrOccCalibThread, g_pstIrSampleCtx);
                if (ret) {
                    ar_err("create ir occ calib thread failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "cancel_ir_occ_calib") == 0)
            {
                IrCmdCancelOccCalib(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "load_ir_sns_calib_data") == 0)
            {
                LoadSnsParamCalibData(g_pstIrSampleCtx->pstIrParamCtx, ir_cmd_data.cmd[1]);
                ApplyIrSnsParam(g_pstIrSampleCtx->pstIrParamCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "load_ir_occ_calib_data") == 0)
            {
                LoadOccCalibData(g_pstIrSampleCtx->pstIrParamCtx, ir_cmd_data.cmd[1]);
                ApplyIrOccParam(g_pstIrSampleCtx->pstIrParamCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_kb_calib") == 0)
            {
                ret = IrKbCalib(g_pstIrSampleCtx->pstIrParamCtx, &g_pstIrSampleCtx->stIrKBCalibCfg,
                    ir_cmd_data.cmd[1], ir_cmd_data.cmd[2], ir_cmd_data.cmd[3]);
                if (ret) {
                    ar_err("kb calibration failed");
                } else {
                    printf("kb calibration finished\n");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "load_ir_kb_calib_data") == 0)
            {
                LoadKbCalibData(g_pstIrSampleCtx->pstIrParamCtx, ir_cmd_data.cmd[1]);
                ApplyIrKbParam(g_pstIrSampleCtx->pstIrParamCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_b_calib") == 0)
            {
                ret = StartIrBCalib(g_pstIrSampleCtx);
                if (ret == 0) {
                    printf("b calibration finished\n");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "cancel_ir_b_calib") == 0)
            {
                IrCmdCancelBCalib(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_dynamic_calib_auto") == 0)
            {
                StartIrAutoDynamicCalib(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_sns_param") == 0)
            {
                g_pstIrSampleCtx->pfnIrCmdGetIrSnsParam(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_occ_param") == 0)
            {
                IrCmdGetIrOccParam(g_pstIrSampleCtx, ir_cmd_data.cmd[1]);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_kb_param") == 0)
            {
                IrCmdGetIrKbParam(g_pstIrSampleCtx, ir_cmd_data.cmd[1]);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_gtm_param") == 0)
            {
                IrCmdGetIrGtmParam(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_gtm_param") == 0)
            {
                IrCmdSetIrGtmParam(g_pstIrSampleCtx, atoi(ir_cmd_data.cmd[1]),
                    atoi(ir_cmd_data.cmd[2]));
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_sns_attr") == 0)
            {
                VI_PIPE rawPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
                IR_SNS_ATTR_S stIrSnsAttr;

                ret= AR_MPI_ISP_GetIrSnsAttr(rawPipe, &stIrSnsAttr);
                ar_err("ret[%d] ir_sns_param_size[%u] occ_type[%d] occ_monotonic[%d] occ_range[%u %u] occ_vert_offset[%d] sns_size[%u %u] roi_rect[%d %d %u %u]",
                    ret, stIrSnsAttr.u32IrParamSize, stIrSnsAttr.enOccType,
                    stIrSnsAttr.enOccMonotonic, stIrSnsAttr.u32MinOccVal,
                    stIrSnsAttr.u32MaxOccVal, stIrSnsAttr.s32OccVerticalOffset,
                    stIrSnsAttr.stSnsSize.u32Width, stIrSnsAttr.stSnsSize.u32Height,
                    stIrSnsAttr.stRoiRect.s32X, stIrSnsAttr.stRoiRect.s32Y,
                    stIrSnsAttr.stRoiRect.u32Width, stIrSnsAttr.stRoiRect.u32Height);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_occ_single_val") == 0)
            {
                VI_PIPE rawPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrRawIdx];
                ISP_IR_PARAMS_S stIrParams;
                ISP_IR_OCC_PARAM_S stIrOccParam;

                stIrOccParam.bIsSingleVal = AR_TRUE;
                stIrOccParam.u32SingleVal = atoi(ir_cmd_data.cmd[1]);

                stIrParams.u32ParamCnt = 1;
                stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
                stIrParams.astParamEntry[0].pParamData = &stIrOccParam;
                stIrParams.astParamEntry[0].u32ParamSize = sizeof(stIrOccParam);

                ret = AR_MPI_ISP_SetIrParams(rawPipe, &stIrParams);
                if (ret) {
                    ar_err("set ir occ param failed");
                } else {
                    printf("set ir occ param finished\n");
                }
            }
            else if(strcmp(ir_cmd_data.cmd[0], "set_color_index") == 0)
            {
                SetColormapIndex(atoi(ir_cmd_data.cmd[1]));
            }
            else if(strcmp(ir_cmd_data.cmd[0], "set_fusion_mode") == 0)
            {
                SetFusionMode(atoi(ir_cmd_data.cmd[1]));
            }
            else if(strcmp(ir_cmd_data.cmd[0], "set_fusion_param") == 0)
            {
                SetFusionParam(atoi(ir_cmd_data.cmd[1]), atoi(ir_cmd_data.cmd[2]), atoi(ir_cmd_data.cmd[3]), atoi(ir_cmd_data.cmd[4]));
            }
            else if(strcmp(ir_cmd_data.cmd[0], "set_fusion_calib") == 0)
            {
                POINT_S srcpt[4] = {0}, dstpt[4] = {0};
                int w = atoi(ir_cmd_data.cmd[1]);
                int h = atoi(ir_cmd_data.cmd[2]);
                int argindex = 3;
                srcpt[0].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[0].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[1].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[1].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[2].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[2].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[3].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                srcpt[3].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[0].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[0].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[1].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[1].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[2].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[2].s32Y = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[3].s32X = atoi(ir_cmd_data.cmd[argindex++]);
                dstpt[3].s32Y = atoi(ir_cmd_data.cmd[argindex++]);

                SetFusionCalibPoint(w, h, srcpt, dstpt);
            }
            else if(strcmp(ir_cmd_data.cmd[0], "set_fusion_calib_enable") == 0)
            {
                int enable = atoi(ir_cmd_data.cmd[1]);
                SetFusionCalibEnable(enable);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_sharpness") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_SHARPNESS_TIDY_ATTR_S stSharpnessAttr;

                stSharpnessAttr.u32Sharpness = atoi(ir_cmd_data.cmd[1]);
                ret = AR_MPI_ISP_SetSharpnessTidyAttr(thrmlPipe, &stSharpnessAttr);
                if (ret) {
                    ar_err("set ir sharpness failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_sharpness") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_SHARPNESS_TIDY_ATTR_S stSharpnessAttr;

                ret = AR_MPI_ISP_GetSharpnessTidyAttr(thrmlPipe, &stSharpnessAttr);
                if (ret) {
                    ar_err("get ir sharpness failed");
                } else {
                    printf("ir sharpness: %u\n", stSharpnessAttr.u32Sharpness);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_3dnr_strength") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthAttr;

                stDe3dStrengthAttr.u32De3dStrength = atoi(ir_cmd_data.cmd[1]);
                ret = AR_MPI_ISP_SetDe3dStrengthTidyAttr(thrmlPipe, &stDe3dStrengthAttr);
                if (ret) {
                    ar_err("set ir de3d strength failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_3dnr_strength") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthAttr;

                ret = AR_MPI_ISP_GetDe3dStrengthTidyAttr(thrmlPipe, &stDe3dStrengthAttr);
                if (ret) {
                    ar_err("get ir de3d strength failed");
                } else {
                    printf("ir de3d strength: %u\n", stDe3dStrengthAttr.u32De3dStrength);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_2dnr_strength") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_DE2D_STRENGTH_TIDY_ATTR_S stDe2dStrengthAttr;

                stDe2dStrengthAttr.stDe2dStrength.strength = atoi(ir_cmd_data.cmd[1]);
                ret = AR_MPI_ISP_SetDe2dStrengthTidyAttr(thrmlPipe, &stDe2dStrengthAttr);
                if (ret) {
                    ar_err("set ir de2d strength failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_2dnr_strength") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_DE2D_STRENGTH_TIDY_ATTR_S stDe2dStrengthAttr;

                ret = AR_MPI_ISP_GetDe2dStrengthTidyAttr(thrmlPipe, &stDe2dStrengthAttr);
                if (ret) {
                    ar_err("get ir de2d strength failed");
                } else {
                    printf("ir de2d strength: %u\n", stDe2dStrengthAttr.stDe2dStrength.strength);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_detail_enh_strength") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_DETAIL_ENH_TIDY_ATTR_S stDetailEnhAttr;

                stDetailEnhAttr.u32FlatZoneEnh = atoi(ir_cmd_data.cmd[1]);
                stDetailEnhAttr.u32DetailZoneEnh = atoi(ir_cmd_data.cmd[2]);
                ret = AR_MPI_ISP_SetDetailEnhanceTidyAttr(thrmlPipe, &stDetailEnhAttr);
                if (ret) {
                    ar_err("set ir detail enhance strength failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_detail_enh_strength") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_DETAIL_ENH_TIDY_ATTR_S stDetailEnhAttr;

                ret = AR_MPI_ISP_GetDetailEnhanceTidyAttr(thrmlPipe, &stDetailEnhAttr);
                if (ret) {
                    ar_err("get ir detail enhance strength failed");
                } else {
                    printf("ir detail enhance strength: flat_zone[%u] detail_zone[%u]\n",
                        stDetailEnhAttr.u32FlatZoneEnh, stDetailEnhAttr.u32DetailZoneEnh);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_contrast") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_CONTRAST_TIDY_ATTR_S stContrastAttr;

                stContrastAttr.u32Contrast = atoi(ir_cmd_data.cmd[1]);
                ret = AR_MPI_ISP_SetContrastTidyAttr(thrmlPipe, &stContrastAttr);
                if (ret) {
                    ar_err("set ir contrast strength failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_contrast") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_CONTRAST_TIDY_ATTR_S stContrastAttr;

                ret = AR_MPI_ISP_GetContrastTidyAttr(thrmlPipe, &stContrastAttr);
                if (ret) {
                    ar_err("get ir contrast strength failed");
                } else {
                    printf("ir contrast strength: %u\n", stContrastAttr.u32Contrast);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_brightness") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_BRIGHTNESS_TIDY_ATTR_S stBrightnessAttr;

                stBrightnessAttr.u32Brightness = atoi(ir_cmd_data.cmd[1]);
                ret = AR_MPI_ISP_SetBrightnessTidyAttr(thrmlPipe, &stBrightnessAttr);
                if (ret) {
                    ar_err("set ir brightness strength failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_brightness") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_BRIGHTNESS_TIDY_ATTR_S stBrightnessAttr;

                ret = AR_MPI_ISP_GetBrightnessTidyAttr(thrmlPipe, &stBrightnessAttr);
                if (ret) {
                    ar_err("get ir brightness strength failed");
                } else {
                    printf("ir brightness strength: %u\n", stBrightnessAttr.u32Brightness);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_global_contrast") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_GLOBAL_CONTRAST_TIDY_ATTR_S stGlobalContrastAttr;

                stGlobalContrastAttr.u32GlobalContrast = atoi(ir_cmd_data.cmd[1]);
                ret = AR_MPI_ISP_SetGlobalContrastTidyAttr(thrmlPipe, &stGlobalContrastAttr);
                if (ret) {
                    ar_err("set ir global contrast strength failed");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_global_contrast") == 0)
            {
                VI_PIPE thrmlPipe = g_pstIrSampleCtx->stIrViGrpCfg.aViPipe[g_pstIrSampleCtx->stIrViGrpCfg.s32IrThrmlIdx];
                ISP_GLOBAL_CONTRAST_TIDY_ATTR_S stGlobalContrastAttr;

                ret = AR_MPI_ISP_GetGlobalContrastTidyAttr(thrmlPipe, &stGlobalContrastAttr);
                if (ret) {
                    ar_err("get ir global contrast strength failed");
                } else {
                    printf("ir global contrast strength: %u\n", stGlobalContrastAttr.u32GlobalContrast);
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "set_ir_gain_alpha") == 0)
            {
                g_pstIrSampleCtx->f32GainAlpha = atof(ir_cmd_data.cmd[1]);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "get_ir_gain_alpha") == 0)
            {
                printf("ir_gain_alpha[%f]\n", g_pstIrSampleCtx->f32GainAlpha);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "start_ir") == 0)
            {
                ret = StartIr(g_pstIrSampleCtx, AR_TRUE);
                if (ret) {
                    ar_err("start ir failed");
                } else {
                    printf("start ir successfully\n");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "stop_ir") == 0)
            {
                ret = StopIr(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("stop ir failed");
                } else {
                    printf("stop ir successfully\n");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "start_vl") == 0)
            {
                ret = StartVl(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("start visible light failed");
                } else {
                    printf("start visible light successfully\n");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "stop_vl") == 0)
            {
                ret = StopVl(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("stop visible light failed");
                } else {
                    printf("stop visible light successfully\n");
                }
            }
            else if (strcmp(ir_cmd_data.cmd[0], "start_all") == 0)
            {
                ret = StartIr(g_pstIrSampleCtx, AR_TRUE);
                if (ret) {
                    ar_err("start ir failed");
                    break;
                }

                ret = StartVl(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("start visible light failed");
                    break;
                }

                printf("start ir & visible light successfully\n");
            }
            else if (strcmp(ir_cmd_data.cmd[0], "stop_all") == 0)
            {
                ret = StopVl(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("stop visible light failed");
                    break;
                }

                ret = StopIr(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("stop ir failed");
                    break;
                }

                printf("stop ir & visible light successfully\n");
            }
            else if (strcmp(ir_cmd_data.cmd[0], "switch_ir_vl") == 0)
            {
                ret = StopVl(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("stop visible light failed");
                    break;
                }

                ret = StopIr(g_pstIrSampleCtx);
                if (ret) {
                    ar_err("stop ir failed");
                    break;
                }

                if (s32SwitchIrVlFlag == 0) {
                    ret = StartVl(g_pstIrSampleCtx);
                    if (ret) {
                        ar_err("start visible light failed");
                        break;
                    }

                    s32SwitchIrVlFlag = 1;
                } else if (s32SwitchIrVlFlag == 1) {
                    ret = StartIr(g_pstIrSampleCtx, AR_TRUE);
                    if (ret) {
                        ar_err("start ir failed");
                        break;
                    }

                    s32SwitchIrVlFlag = 0;
                }

                printf("stop ir & visible light successfully\n");
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_suspend") == 0)
            {
                SuspendIr(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "ir_resume") == 0)
            {
                ResumeIr(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "vl_suspend") == 0)
            {
                SuspendVl(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "vl_resume") == 0)
            {
                ResumeVl(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "all_suspend") == 0)
            {
                SuspendVl(g_pstIrSampleCtx);
                SuspendIr(g_pstIrSampleCtx);
            }
            else if (strcmp(ir_cmd_data.cmd[0], "all_resume") == 0)
            {
                ResumeVl(g_pstIrSampleCtx);
                ResumeIr(g_pstIrSampleCtx);
            }
            else
            {
                ar_err("unsupported ir_cmd[%s]", ir_cmd_data.cmd[0]);
            }
            break;
        }

        default:
            ar_err("receive unexpected signal[%d]", signum);
            break;
    }
}

void ddr_retention_suspend_and_resume_cb(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    int flag = -1;
    AR_U64 u64Tic = 0;
    struct timeval tm_start;
    struct timeval tm_end;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent || SYSCTL_EVENT_SUSPEND == eSysctlEvent) {
        gettimeofday(&tm_start, NULL);

        bIrDisplayEn = AR_FALSE;

        ret = AR_MPI_RGN_Suspend();
        ret |= AR_MPI_VO_Suspend(0);
        ret |= SuspendVl(g_pstIrSampleCtx);
        ret |= SuspendIr(g_pstIrSampleCtx);

        gettimeofday(&tm_end, NULL);

        if(ret)
            printf("suspend failed, ret:%d!\n", ret);
        else
            printf("suspend success!\n");

        flag = 1;
    } else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent || SYSCTL_EVENT_RESUME == eSysctlEvent) {
        //for gpio power level switch by set register, no need now
        AR_MPI_ISP_SetRegister(1, 0x0a10a000, 0x46);

        gettimeofday(&tm_start, NULL);

        ret = AR_MPI_VO_Resume(0);
        ret |= AR_MPI_RGN_Resume();
        ret |= ResumeVl(g_pstIrSampleCtx);
        ret |= ResumeIr(g_pstIrSampleCtx);

        bIrDisplayEn = AR_TRUE;

        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("resume failed, ret:%d!\n", ret);
        else
            printf("resume success!\n");


        flag = 0;
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    ret = AR_MPI_SYSCTL_Event_done(sysctlHandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");

    if(-1 != flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }
}

AR_S32 SAMPLE_IR(AR_CHAR *strSensor, AR_S32 s32Mode, AR_FLOAT fDispScalerRatio, AR_S32 s32Fusion,
    IR_OFFLINE_CTX_S *pstIrOfflineCtx)
{
    AR_S32 s32Ret = 0;
    IR_SAMPLE_CTX_S *pstIrSampleCtx = NULL;
    IR_VI_GROUP_S *pstIrViGrp = NULL;
    IR_VI_GROUP_CFG_S *pstIrViGrpCfg = NULL;
    STRU_AR_HAL_VO_RECT stIrVoChnRect = {0, 0, 640, 512};
    STRU_AR_HAL_VO_RECT stVlVoChnRect = {0, 0, 1920, 1080};
    VB_CONFIG_S stVbConf;
    AR_BOOL bFusion = s32Fusion==0?AR_FALSE:AR_TRUE;
    AR_BOOL bInitiallyStartVi = AR_TRUE;

    if (s32Mode >= 1000) {
        bInitiallyStartVi = AR_FALSE;
        s32Mode -= 1000;
    }

    pstIrSampleCtx = FindIrSampleCtx(strSensor);
    if (pstIrSampleCtx == NULL) {
        ar_err("sensor[%s] is not supported", strSensor);
        return -1;
    }
    g_pstIrSampleCtx = pstIrSampleCtx;

    pstIrSampleCtx->f32GainAlpha = DEFAULT_IR_GAIN_ALPHA;

    if (pstIrOfflineCtx->u32Width == 400) {
        pstIrOfflineCtx->u32Width = 408;
    }

    s32Ret = g_pstIrSampleCtx->pfnCfgIrViTopology(g_pstIrSampleCtx, s32Mode,
        pstIrOfflineCtx->u32Width, pstIrOfflineCtx->u32Height);
    if (s32Ret) {
        ar_err("configure ir vi topology[%d] failed", s32Mode);
        return -1;
    }

    pthread_mutex_init(&pstIrSampleCtx->shutterLock, NULL);
    pthread_mutex_init(&pstIrSampleCtx->irRawProcLock, NULL);
    pthread_mutex_init(&pstIrSampleCtx->irThrmlProcLock, NULL);
    pthread_mutex_init(&pstIrSampleCtx->irIsp1ProcLock, NULL);
    pthread_mutex_init(&pstIrSampleCtx->irIsp2ProcLock, NULL);
    pthread_mutex_init(&pstIrSampleCtx->frmCntLock, NULL);

    pstIrViGrpCfg = &pstIrSampleCtx->stIrViGrpCfg;

    pstIrViGrpCfg->irRawOutPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrRawOutHandle = IrRawOutHandle;

    pstIrViGrpCfg->irIsp1OutPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrIsp1OutHandle = IrIsp1OutHandle;

    pstIrViGrpCfg->irIsp1DeqPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrIsp1DeqHandle = IrIsp1DeqHandle;

    pstIrViGrpCfg->irIsp2OutPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrIsp2OutHandle = IrIsp2OutHandle;

    pstIrViGrpCfg->irIsp2DeqPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrIsp2DeqHandle = IrIsp2DeqHandle;

    pstIrViGrpCfg->irThrmlOutPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrThrmlOutHandle = IrThrmlOutHandle;

    pstIrViGrpCfg->irThrmlDeqPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnIrThrmlDeqHandle = IrThrmlDeqHandle;

    pstIrViGrpCfg->visibleLightOutPriv = (AR_VOID *)pstIrSampleCtx;
    pstIrViGrpCfg->pfnVisibleLightOutHandle = VisibleLightOutHandle;

    pstIrSampleCtx->pstIrViGrp = CreateIrViGroup(pstIrViGrpCfg);
    if (pstIrSampleCtx->pstIrViGrp == NULL) {
        ar_err("create ir vi group failed");
        s32Ret = -1;
        goto EXIT1;
    }
    pstIrViGrp = pstIrSampleCtx->pstIrViGrp;

    GetIrViGroupVbConf(pstIrViGrp, &stVbConf);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret) {
        ar_err("system init failed with %d!", s32Ret);
        goto EXIT2;
    }

    s32Ret = ar_vo_dev_init();
    if (s32Ret) {
        ar_err("ar_vo_dev_init failed");
        goto EXIT3;
    }

    if(bFusion){
        STRU_AR_HAL_VO_RECT rects[4] = {0};
        AR_U32 u32Width = 1920, u32Height = 1080, u32Square = 2, u32WndNum = 4;
        for(int i=0;i<u32WndNum;i++){
            rects[i].x = ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
            rects[i].y = ALIGN_DOWN((u32Height / u32Square) * (i / u32Square), 2);
            rects[i].w = ALIGN_DOWN(u32Width / u32Square, 2);
            rects[i].h = ALIGN_DOWN(u32Height / u32Square, 2);
        }
        ar_vo_surface_init(0, &rects[0]);
        ar_vo_surface_init(1, &rects[1]);
        ar_vo_surface_init(3, &rects[3]);
    }
    else if (pstIrViGrpCfg->s32IrIsp1Idx >= 0 || pstIrViGrpCfg->s32IrIsp2Idx >= 0) {
        if (pstIrViGrpCfg->s32IrIsp2Idx >= 0) {
            stIrVoChnRect.w = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp2Idx].u32Width;
            stIrVoChnRect.h = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp2Idx].u32Height;
        } else {
            stIrVoChnRect.w = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width;
            stIrVoChnRect.h = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height;
        }

        if (stIrVoChnRect.w == 408) {
            stIrVoChnRect.w = 400;
        }

        stIrVoChnRect.w *= fDispScalerRatio;
        stIrVoChnRect.h *= fDispScalerRatio;

        if (pstIrViGrpCfg->s32VisibleLightIdx >= 0) {
            stVlVoChnRect.x = stIrVoChnRect.w;
            stVlVoChnRect.y = 0;
            stVlVoChnRect.w = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32VisibleLightIdx].u32Width;
            stVlVoChnRect.h = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32VisibleLightIdx].u32Height;
            if (stVlVoChnRect.w > 1920 - stVlVoChnRect.x) {
                stVlVoChnRect.h = stVlVoChnRect.h * (1920 - stVlVoChnRect.x) / stVlVoChnRect.w;
                stVlVoChnRect.w = 1920 - stVlVoChnRect.x;
            }
            if (stVlVoChnRect.h > 1080) {
                stVlVoChnRect.w = stVlVoChnRect.w * 1080 / stVlVoChnRect.h;
                stVlVoChnRect.h = 1080;
            }

            ar_vo_surface_init(0, &stIrVoChnRect);
            if (s32Ret) {
                ar_err("ar_vo_surface_init failed");
                goto EXIT4;
            }

            ar_vo_surface_init(1, &stVlVoChnRect);
            if (s32Ret) {
                ar_err("ar_vo_surface_init failed");
                close_display_surface(0);
                goto EXIT4;
            }
        } else {
            ar_vo_surface_init(0, &stIrVoChnRect);
            if (s32Ret) {
                ar_err("ar_vo_surface_init failed");
                goto EXIT4;
            }
        }
    } else if (pstIrViGrpCfg->s32VisibleLightIdx >= 0) {
        stVlVoChnRect.w = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32VisibleLightIdx].u32Width;
        stVlVoChnRect.h = pstIrViGrpCfg->astOutSize[pstIrViGrpCfg->s32VisibleLightIdx].u32Height;
        if (stVlVoChnRect.w > 1920) {
            stVlVoChnRect.w = 1920;
        }
        if (stVlVoChnRect.h > 1080) {
            stVlVoChnRect.w = 1080;
        }

        s32Ret = ar_vo_surface_init(1, &stVlVoChnRect);
        if (s32Ret) {
            ar_err("ar_vo_surface_init failed");
            goto EXIT4;
        }
    }

    if (bInitiallyStartVi) {
        s32Ret = StartIr(pstIrSampleCtx, (strcmp(strSensor, "offline") == 0 ? AR_FALSE : AR_TRUE));
        if (s32Ret) {
            ar_err("start ir failed");
            goto EXIT5;
        }

        s32Ret = StartVl(pstIrSampleCtx);
        if (s32Ret) {
            ar_err("start visible light failed");
            goto EXIT6;
        }
    }

    if(bFusion){
        //check and enable vis vi chn1 for fusion
        AR_BOOL bVisCh1 = EnableVisibleLightViChn1(pstIrViGrpCfg);
        if(bVisCh1) {
            s32Ret = StartIrVpss(pstIrViGrpCfg);
            if(s32Ret!=0){
                ar_err("create ir vpss failed");
            }
            else{
                StartIrFusion(pstIrSampleCtx, FusionOutHandle);
                bEnableFusion = AR_TRUE;
            }
        }
    }

    signal(SIGUSR1, ir_cmd_signal_handler);

    sysctlHandle = AR_MPI_SYSCTL_Register("vin_ddr_retention", 0, ddr_retention_suspend_and_resume_cb);
    if (sysctlHandle == NULL) {
        ar_err("register sysctl failed");
        goto EXIT7;
    }

    if (strcmp(strSensor, "offline") != 0) {
        while (!bExit) {
            sleep(1);
        }
    } else {
        VI_PIPE sendPipe = -1;
        VIDEO_FRAME_INFO_S stSendFrame = {0};
        AR_U32 u32BlkSize = 0;
        VB_BLK vbBlk = VB_INVALID_HANDLE;
        VB_POOL vbPool = VB_INVALID_POOLID;
        FILE *fp = NULL;
        size_t rsize = 0;
        AR_U32 u32FrmCnt1 = 0;
        AR_U32 u32FrmCnt2 = 0;
        AR_BOOL bFrmProcDone = AR_FALSE;
        AR_U32 u32PollTimes = 0;
        struct timeval stCurTime;
        struct timeval stLastTime;
        AR_U32 i = 0;
        AR_U32 j = 0;
        AR_S32 k = 0;

        bIrDisplayEn = AR_TRUE;

        printf("ir offline: size[%u %u] input[%s] thermal[%s] 3dnr[%s] gtm[%s] isp1[%s] isp2[%s] circle_times[%d]\n",
            pstIrOfflineCtx->u32Width, pstIrOfflineCtx->u32Height, pstIrOfflineCtx->input_fname,
            pstIrOfflineCtx->thermal_output_fname, pstIrOfflineCtx->nr3d_output_fname,
            pstIrOfflineCtx->gtm_output_fname, pstIrOfflineCtx->isp1_output_fname,
            pstIrOfflineCtx->isp2_output_fname, pstIrOfflineCtx->s32Circle);

        if (pstIrViGrpCfg->s32IrThrmlIdx >= 0) {
            sendPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrThrmlIdx];
            u32BlkSize = VI_GetRawBufferSize(pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrThrmlIdx].u32Width,
                pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrThrmlIdx].u32Height,
                PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
        } else {
            sendPipe = pstIrViGrpCfg->aViPipe[pstIrViGrpCfg->s32IrIsp1Idx];
            u32BlkSize = VI_GetRawBufferSize(pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Width,
                pstIrViGrpCfg->astInSize[pstIrViGrpCfg->s32IrIsp1Idx].u32Height,
                PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
        }

        printf("ir offline: send raw frame buf size[%u]\n", u32BlkSize);

        vbBlk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, u32BlkSize, NULL);
        vbPool = AR_MPI_VB_Handle2PoolId(vbBlk);
        AR_MPI_VB_MmapPool(vbPool);
        stSendFrame.stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(vbBlk);
        AR_MPI_VB_GetBlockVirAddr(vbPool, stSendFrame.stVFrame.u64PhyAddr[0],
            (AR_VOID **)&stSendFrame.stVFrame.u64VirAddr[0]);

        stSendFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP;
        stSendFrame.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
        stSendFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
        stSendFrame.stVFrame.u32Width = pstIrOfflineCtx->u32Width;
        stSendFrame.stVFrame.u32Height = pstIrOfflineCtx->u32Height;
        stSendFrame.stVFrame.u32Stride[0] = CAM_ALIGNE_TO(stSendFrame.stVFrame.u32Width * 2, 256);
        stSendFrame.stVFrame.u32Len[0] = stSendFrame.stVFrame.u32Stride[0] * stSendFrame.stVFrame.u32Height;
        stSendFrame.u32PoolId = vbPool;
        stSendFrame.enModId = AR_ID_USER;

        fp = fopen(pstIrOfflineCtx->input_fname, "rb");
        if (fp == NULL) {
            ar_err("open %s failed", pstIrOfflineCtx->input_fname);
            goto EXIT8;
        }

        if (s32Mode <= IR_VI_TOPOLOGY_TYPE_4 && strcmp(pstIrOfflineCtx->thermal_output_fname, "nil")) {
            StartIrThrmlCapture(pstIrSampleCtx, 0x7fffffffUL,
                pstIrOfflineCtx->thermal_output_fname, pstIrOfflineCtx->bImgNoDummy);
        }

        if (s32Mode <= IR_VI_TOPOLOGY_TYPE_4 && strcmp(pstIrOfflineCtx->nr3d_output_fname, "nil")) {
            StartIr3DnrCapture(pstIrSampleCtx, 0x7fffffffUL,
                pstIrOfflineCtx->nr3d_output_fname, pstIrOfflineCtx->bImgNoDummy);
        }

        if (s32Mode <= IR_VI_TOPOLOGY_TYPE_4 && strcmp(pstIrOfflineCtx->nr2d_output_fname, "nil")) {
            StartIr2DnrCapture(pstIrSampleCtx, 0x7fffffffUL,
                pstIrOfflineCtx->nr2d_output_fname, pstIrOfflineCtx->bImgNoDummy);
        }

        if (s32Mode <= IR_VI_TOPOLOGY_TYPE_4 && strcmp(pstIrOfflineCtx->gtm_output_fname, "nil")) {
            StartIrGtmCapture(pstIrSampleCtx, 0x7fffffffUL,
                pstIrOfflineCtx->gtm_output_fname, pstIrOfflineCtx->bImgNoDummy);
        }

        if (strcmp(pstIrOfflineCtx->isp1_output_fname, "nil")) {
            StartIrIsp1Capture(pstIrSampleCtx, 0x7fffffffUL,
                pstIrOfflineCtx->isp1_output_fname, pstIrOfflineCtx->bImgNoDummy);
        }

        if (strcmp(pstIrOfflineCtx->isp2_output_fname, "nil")) {
            StartIrIsp2Capture(pstIrSampleCtx, 0x7fffffffUL,
                pstIrOfflineCtx->isp2_output_fname, pstIrOfflineCtx->bImgNoDummy);
        }

        gettimeofday(&stLastTime, NULL);

        if (pstIrOfflineCtx->s32Circle <= 0) {
            pstIrOfflineCtx->s32Circle = 0x7fffffff;
        }
#if 0
        {
            int policy = 0;
            struct sched_param sp;
            memset(&sp, 0, sizeof(sp));

            pthread_getschedparam(pthread_self(), &policy, &sp);
            ar_always("ir raw in thread: policy[%d] priority[%d] RR_min_max[%d %d]",
                policy, sp.sched_priority, sched_get_priority_min(SCHED_RR),
                sched_get_priority_max(SCHED_RR));

            policy = SCHED_RR;
            memset(&sp, 0, sizeof(sp));
            sp.sched_priority = sched_get_priority_min(SCHED_RR);
            pthread_setschedparam(pthread_self(), policy, &sp);
        }
#endif
        for (k = 0; k < pstIrOfflineCtx->s32Circle; k++) {
            s32Ret = fseek(fp, 0, SEEK_SET);
            if (s32Ret) {
                ar_err("seek to begin failed");
                s32Ret = 0;
                break;
            }

            while (1) {
                if (!pstIrOfflineCtx->bImgNoDummy) {
                    rsize = fread((AR_VOID *)stSendFrame.stVFrame.u64VirAddr[0], 1, u32BlkSize, fp);
                    if (rsize != u32BlkSize) {
                        //printf("send frame all finished\n");
                        usleep(20 * 1000);
                        break;
                    }
                } else {
                    AR_BOOL bSendAllDone = AR_FALSE;
                    AR_VOID *pData = (AR_VOID *)stSendFrame.stVFrame.u64VirAddr[0];
                    AR_U32 u32LineLen = (pstIrOfflineCtx->u32Width == 408 ? 400 : pstIrOfflineCtx->u32Width) * 2;
                    for (i = 0; i < pstIrOfflineCtx->u32Height; i++) {
                        rsize = fread(pData, 1, u32LineLen, fp);
                        if (rsize != u32LineLen) {
                            //printf("send frame all finished\n");
                            bSendAllDone = AR_TRUE;
                            break;
                        }

                        if (pstIrOfflineCtx->u32Width == 408) {
                            AR_U16 *pTemp = (AR_U16 *)(pData + u32LineLen);
                            for (j = 0; j < 8; j++) {
                                *pTemp = *(pTemp - 1);
                                pTemp++;
                            }
                        }

                        pData += stSendFrame.stVFrame.u32Stride[0];
                    }

                    if (bSendAllDone) {
                        usleep(20 * 1000);
                        break;
                    }
                }

                u32FrmCnt1++;

                s32Ret = AR_MPI_VI_SendPipeRaw(sendPipe, &stSendFrame, -1);
                if (s32Ret) {
                    ar_err("send frame[%u] failed", u32FrmCnt1);
                    continue;
                }

                u32FrmCnt2++;
                bFrmProcDone = AR_FALSE;
                u32PollTimes = 0;

                while (u32PollTimes < 500) {
                    pthread_mutex_lock(&pstIrSampleCtx->frmCntLock);
                    if ((s32Mode <= IR_VI_TOPOLOGY_TYPE_4 && pstIrSampleCtx->u32ThrmlFrmCnt != u32FrmCnt2)
                        || ((s32Mode <= IR_VI_TOPOLOGY_TYPE_3 || s32Mode == IR_VI_TOPOLOGY_TYPE_6) && pstIrSampleCtx->u32Isp1FrmCnt != u32FrmCnt2)
                        || ((s32Mode <= IR_VI_TOPOLOGY_TYPE_1 || s32Mode == IR_VI_TOPOLOGY_TYPE_6) && pstIrSampleCtx->u32Isp2FrmCnt != u32FrmCnt2)
                        || ((s32Mode == IR_VI_TOPOLOGY_TYPE_0 || s32Mode == IR_VI_TOPOLOGY_TYPE_2) && pstIrSampleCtx->u32Isp1DeqFrmCnt != u32FrmCnt2)
                        || ((s32Mode <= IR_VI_TOPOLOGY_TYPE_1 || s32Mode == IR_VI_TOPOLOGY_TYPE_6) && pstIrSampleCtx->u32Isp2DeqFrmCnt != u32FrmCnt2)) {
                        pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
                        u32PollTimes++;
                        usleep(20 * 1000);
                        continue;
                    } else {
                        pthread_mutex_unlock(&pstIrSampleCtx->frmCntLock);
                        bFrmProcDone = AR_TRUE;
                        break;
                    }
                }

                gettimeofday(&stCurTime, NULL);

                if (bFrmProcDone) {
                    //printf("frame[%u] process done, time[%d us]\n", u32FrmCnt1,
                    //    get_time_interval_us(stLastTime, stCurTime));
                } else {
                    printf("frame[%u] process failed or timeout\n", u32FrmCnt1);
                }

                stLastTime = stCurTime;

                usleep(20 * 1000);
            }
        }

        printf("send frame all finished\n");
        fclose(fp);
        AR_MPI_VB_MunmapPool(vbPool);
        AR_MPI_VB_ReleaseBlock(vbBlk);
    }

    if (pstIrSampleCtx->pRawFp) {
        fclose(pstIrSampleCtx->pRawFp);
    }

    if (pstIrSampleCtx->pThrmlFp) {
        fclose(pstIrSampleCtx->pThrmlFp);
    }

    if (pstIrSampleCtx->p3DnrFp) {
        fclose(pstIrSampleCtx->p3DnrFp);
    }

    if (pstIrSampleCtx->p2DnrFp) {
        fclose(pstIrSampleCtx->p2DnrFp);
    }

    if (pstIrSampleCtx->pGtmFp) {
        fclose(pstIrSampleCtx->pGtmFp);
    }

    if (pstIrSampleCtx->pIsp1Fp) {
        fclose(pstIrSampleCtx->pIsp1Fp);
    }

    if (pstIrSampleCtx->pIsp2Fp) {
        fclose(pstIrSampleCtx->pIsp2Fp);
    }

    if(bEnableFusion)
    {
        bEnableFusion = false;
        StopIrFusion();
        StopIrVpss();
        DisableVisibleLightViChn1(pstIrViGrpCfg);
    }

EXIT8:
    s32Ret = AR_MPI_SYSCTL_Unregister(sysctlHandle);
    if (s32Ret) {
        ar_err("unregister sysctl failed");
    }

EXIT7:
    StopVl(pstIrSampleCtx);

EXIT6:
    StopIr(pstIrSampleCtx);

EXIT5:
    close_all_display_surface();

EXIT4:
    close_display();

EXIT3:
    SAMPLE_COMM_SYS_Exit();

EXIT2:
    DestroyIrViGroup(pstIrViGrp);

EXIT1:
    pthread_mutex_destroy(&pstIrSampleCtx->shutterLock);
    pthread_mutex_destroy(&pstIrSampleCtx->irRawProcLock);
    pthread_mutex_destroy(&pstIrSampleCtx->irThrmlProcLock);
    pthread_mutex_destroy(&pstIrSampleCtx->irIsp1ProcLock);
    pthread_mutex_destroy(&pstIrSampleCtx->irIsp2ProcLock);
    pthread_mutex_destroy(&pstIrSampleCtx->frmCntLock);
    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
