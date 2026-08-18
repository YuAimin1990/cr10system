#include <sys/prctl.h>

#include "pf_video.h"
#include "pf_audio.h"
#include "pf_mbuf.h"
#include "pf_sensor.h"
#include "pf_drv.h"
#include "pf_isp.h"

#include "media_fifo.h"
#include "util_api.h"

#include "ar_common.h"
#include "ar_buffer.h"
#include "ar_comm_sys.h"
#include "ar_comm_vb.h"
#include "ar_comm_isp.h"
#include "ar_comm_vi.h"
#include "ar_comm_vo.h"
#include "ar_comm_venc.h"
#include "ar_comm_vdec.h"
#include "ar_comm_vpss.h"
#include "ar_comm_region.h"
#include "ar_comm_adec.h"
#include "ar_comm_aenc.h"
#include "ar_comm_ai.h"
#include "ar_comm_ao.h"
#include "ar_comm_aio.h"
#include "ar_defines.h"
#include "ar_comm_hdmi.h"
#include "ar_combo_dev.h"
#include "ar_mipi_tx.h"
//#include "sample_comm.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_audio.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "ar_math.h"
#include "ar_sns_ctrl.h"
#include "mpi_hdmi.h"
#include "mpi_gdc_api.h"
#include "mpi_scaler_api.h"
#include "mpi_ifc_api.h"

#include "hal_sys.h"
#include "hal_gpio.h"
#include "util_timer.h"
#include "util_profile.h"
#include "lcd.h"

#include "pf_vgs.h"
#include "pf_thermal.h"

//On EVB ext board, dvp-4-ch mode, we use dvp 0 and dvp 2, each one has 4 pipes:
//dvp 0: pipe 0,1,2,3
//dvp 2: pipe 4,5,6,7.
//#define IPC_PF_DVP_ID_2_PIPE(DVP_ID) ((DVP_ID) == 0 ? 0 : 1)
#define IPC_PF_PIPE_2_DVP_ID(PIPE) ((PIPE) < 4 ? 0 : 2)
#define IPC_VENC_DEFAULT_FREQUENCY_75MHZ 75000000
#define IPC_VENC_DEFAULT_FREQUENCY_150MHZ 150000000
#define IPC_VENC_DEFAULT_FREQUENCY_200MHZ 200000000
#define IPC_VENC_DEFAULT_FREQUENCY_250MHZ 250000000
#define IPC_VENC_DEFAULT_FREQUENCY_300MHZ 300000000
#define IPC_VENC_DEFAULT_FREQUENCY_360MHZ 360000000
#define IPC_VENC_DEFAULT_FREQUENCY_400MHZ 400000000
#define IPC_VENC_DEFAULT_FREQUENCY_450MHZ 450000000
#define IPC_VENC_DEFAULT_FREQUENCY_500MHZ 500000000
#define IPC_VENC_DEFAULT_FREQUENCY_600MHZ 600000000
#define IPC_VENC_DEFAULT_FREQUENCY_666MHZ 666000000

typedef struct
{
    AR_U32 u32Id; //ch x 4 + stream
    AR_U32 u32Stop;
} IPC_STREAM_THREAD_STATUS;

typedef enum
{
  CTL_START = 0,
  CTL_STOP  = 1,
} IPC_CTL_STATUS_E;

extern int g_dvp_out_mode;

static IPC_STREAM_THREAD_STATUS s_astVThreadStatus[IPC_TOTAL_STREAM_NUM] = {0};
static pthread_mutex_t g_ViMutex[IPC_MAX_PIPELINE_NUM];
pthread_mutex_t g_VpssMutex[IPC_MAX_VPSS_GRP_NEEDED * IPC_MAX_STREAM_PER_PIPE];
static pthread_mutex_t g_VencMutex[32];//we use 32 venc chn
static AR_S32 g_s32VencDevFd[IPC_TOTAL_STREAM_NUM] = {-1};
static IPC_CTL_STATUS_E s_CtrStatus = 0;

static AR_S32 g_s32VencThreadSuspend = 0;
static AR_S32 g_s32ViThreadSuspend = 0;

static AR_U32 IPC_PF_GetEncType(AR_U32 u32CfgType)
{
    if(u32CfgType == VENC_ENCODE_TYPE_H264)
    {
        return PT_H264;
    }
    if(u32CfgType == VENC_ENCODE_TYPE_H265)
    {
        return PT_H265;
    }
    if(u32CfgType == VENC_ENCODE_TYPE_MJPEG)
    {
        return PT_MJPEG;
    }
    if(u32CfgType == VENC_ENCODE_TYPE_JPEG)
    {
        return PT_JPEG;
    }

    return PT_H265;
}

static AR_U32 IPC_PF_GetRcMode(AR_U32 u32CfgType, AR_U32 u32CfgMode)
{
    if(u32CfgType == VENC_ENCODE_TYPE_H264)
    {
        if(u32CfgMode == VENC_BRCMODE_CBR)
        {
            return VENC_RC_MODE_H264CBR;
        }
        if(u32CfgMode == VENC_BRCMODE_VBR)
        {
            return VENC_RC_MODE_H264VBR;
        }
        if(u32CfgMode == VENC_BRCMODE_AVBR)
        {
            return VENC_RC_MODE_H264AVBR;
        }
        if(u32CfgMode == VENC_BRCMODE_QPMAP)
        {
            return VENC_RC_MODE_H264QPMAP;
        }
        if(u32CfgMode == VENC_BRCMODE_FIXQP)
        {
            return VENC_RC_MODE_H264FIXQP;
        }
        if(u32CfgMode == VENC_BRCMODE_QVBR)
        {
            return VENC_RC_MODE_H264QVBR;
        }
    }
    else if(u32CfgType == VENC_ENCODE_TYPE_H265)
    {
        if(u32CfgMode == VENC_BRCMODE_CBR)
        {
            return VENC_RC_MODE_H265CBR;
        }
        if(u32CfgMode == VENC_BRCMODE_VBR)
        {
            return VENC_RC_MODE_H265VBR;
        }
        if(u32CfgMode == VENC_BRCMODE_AVBR)
        {
            return VENC_RC_MODE_H265AVBR;
        }
        if(u32CfgMode == VENC_BRCMODE_QPMAP)
        {
            return VENC_RC_MODE_H265QPMAP;
        }
        if(u32CfgMode == VENC_BRCMODE_FIXQP)
        {
            return VENC_RC_MODE_H265FIXQP;
        }
        if(u32CfgMode == VENC_BRCMODE_QVBR)
        {
            return VENC_RC_MODE_H265QVBR;
        }
    }
    else
    {
        if(u32CfgMode == VENC_BRCMODE_CBR)
        {
            return VENC_RC_MODE_MJPEGCBR;
        }
        if(u32CfgMode == VENC_BRCMODE_VBR)
        {
            return VENC_RC_MODE_MJPEGVBR;
        }
        if(u32CfgMode == VENC_BRCMODE_FIXQP)
        {
            return VENC_RC_MODE_MJPEGFIXQP;
        }
    }

    return VENC_RC_MODE_BUTT;
}

static AR_U32 IPC_PF_GetViFpsForVenc(AR_U32 u32Chn, AR_U32 u32Stream)
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

    if(strcmp(pChnCfg->stChannel[u32Chn].chType, "low_bw") == 0)
    {
        if(u32Stream == 0) //main stream from vi main channel
        {
            return pVioCfg->vi[u32Chn].channel_0_fps;
        }
        else //from vi sub channel
        {
            return pVioCfg->vi[u32Chn].channel_1_fps;
        }
    }

    //basic pipeline, all stream from vi main ch
    return pVioCfg->vi[u32Chn].channel_0_fps;
}

//here we simply check if 8 ch are all enabled, if so, we set high perf to encode above 4k@60fps.
//users can judge if they need this or not.
static void IPC_PF_VENC_SetHighPerformance()
{
    AR_S32 i = 0, s32Ret = 0;
    AR_U32 u32Cnt = 0, u32ViHas4K = 0;
    VENC_PARAM_MOD_S stParam = {0};
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(pChnCfg->stChannel[i].u32Enable)
            u32Cnt++;

        //check if any vi set to 4K@60fps
        if(pVioCfg->vi[i].channel_0_enable && pVioCfg->vi[i].width >= 3840 && pVioCfg->vi[i].height >= 2160
      && pVioCfg->vi[i].fps >= 60)
        {
            u32ViHas4K = 1;
        }
    }

    if(u32Cnt == 8 || u32ViHas4K)
    {
        stParam.enVencModType = MODTYPE_H265E; //whatever 264/265, they share the clk
        s32Ret = AR_MPI_VENC_GetModParam(&stParam);
        if(s32Ret)
        {
            PRINT_ERR("Get venc mod param error! ret = %x\n", s32Ret);
            return;
        }

        //set core=600M, bpu=500M, cmdqueue=4 for ext board
        stParam.stH265eModParam.u32CoreClock = 600;
        stParam.stH265eModParam.u32BpuClock = 500;

        s32Ret = AR_MPI_VENC_SetModParam(&stParam);
        if(s32Ret)
        {
            PRINT_ERR("Set venc mod param error! ret = %x\n", s32Ret);
            return;
        }

    }

}

static void IPC_PF_VENC_SetVencFrequency()
{
    AR_S32 s32Ret = 0;
    AR_S32 i = 0, j = 0;
    VENC_PARAM_MOD_S stParam = {0};
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    AR_U32 u32FrequencyTotal = 0, u32FrequencyCore = 0, u32FrequencyBpu = 0;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }

            u32FrequencyTotal += pstVencCfg->vencStream[i][j].cfg.width
                                    * pstVencCfg->vencStream[i][j].cfg.height
                                        * pstVencCfg->vencStream[i][j].cfg.fps;

        }
    }

    if(u32FrequencyTotal <= IPC_VENC_DEFAULT_FREQUENCY_150MHZ)
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_150MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_150MHZ/1000000;//4M@6Mbps need 150M
    }
    else if(u32FrequencyTotal <= IPC_VENC_DEFAULT_FREQUENCY_200MHZ)
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_200MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_150MHZ/1000000;
    }
    else if(u32FrequencyTotal <= IPC_VENC_DEFAULT_FREQUENCY_300MHZ)
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_300MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_200MHZ/1000000;
    }
    else if(u32FrequencyTotal <= IPC_VENC_DEFAULT_FREQUENCY_400MHZ)
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_400MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_300MHZ/1000000;
    }
    else if(u32FrequencyTotal <= IPC_VENC_DEFAULT_FREQUENCY_500MHZ)
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_500MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_360MHZ/1000000;
    }
    else if(u32FrequencyTotal <= IPC_VENC_DEFAULT_FREQUENCY_600MHZ)
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_600MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_360MHZ/1000000;
    }
    else
    {
        u32FrequencyCore = IPC_VENC_DEFAULT_FREQUENCY_500MHZ/1000000;
        u32FrequencyBpu = IPC_VENC_DEFAULT_FREQUENCY_360MHZ/1000000;
    }
    stParam.enVencModType = MODTYPE_VENC;
    s32Ret = AR_MPI_VENC_GetModParam(&stParam);
    if(s32Ret)
    {
        PRINT_ERR("Get venc mod param error! ret = %x\n", s32Ret);
        return;
    }
    stParam.stVencModParam.u32VencBufferCache = 1;
    s32Ret = AR_MPI_VENC_SetModParam(&stParam);
    if(s32Ret)
    {
        PRINT_ERR("Set venc mod param error! ret = %x\n", s32Ret);
        return;
    }
    stParam.enVencModType = MODTYPE_H265E; //whatever 264/265, they share the clk
    s32Ret = AR_MPI_VENC_GetModParam(&stParam);
    if(s32Ret)
    {
        PRINT_ERR("Get MODTYPE_H265E mod param error! ret = %x\n", s32Ret);
        return;
    }
    stParam.stH265eModParam.u32CoreClock = u32FrequencyCore;
    stParam.stH265eModParam.u32BpuClock = u32FrequencyBpu;

    s32Ret = AR_MPI_VENC_SetModParam(&stParam);
    if(s32Ret)
    {
        PRINT_ERR("Set MODTYPE_H265E mod param error! ret = %x\n", s32Ret);
        return;
    }

    return;
}

static void IPC_PF_VENC_SetVencMsgPoolSize(IPC_CFG_VENC_S * pstVencCfg ) {
    AR_U32 encoder_cnt = 1; //jpeg encoder
    AR_S32 s32Ret = 0;
    AR_U32 u32VencDoneQueueSize = 0;

    for(int i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(int j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }
            //AR_U32 u32EncType = IPC_PF_GetEncType(pstVencCfg->vencStream[i][j].cfg.encodeType);
            AR_U32 u32RcMode = IPC_PF_GetRcMode(pstVencCfg->vencStream[i][j].cfg.encodeType, pstVencCfg->vencStream[i][j].cfg.brcMode);

            if(u32RcMode == VENC_RC_MODE_H264CBR)
            {
                u32VencDoneQueueSize += (2*1024*1024) * IPC_PF_GetViFpsForVenc(i,j) / (pstVencCfg->vencStream[i][j].cfg.cbrAvgBps << 7);
            }
            else if(u32RcMode == VENC_RC_MODE_H265CBR)
            {
                u32VencDoneQueueSize += (2*1024*1024) * IPC_PF_GetViFpsForVenc(i,j) / (pstVencCfg->vencStream[i][j].cfg.cbrAvgBps << 7);
            }
            else if(u32RcMode == VENC_RC_MODE_H264VBR)
            {
                u32VencDoneQueueSize += (2*1024*1024)  * IPC_PF_GetViFpsForVenc(i,j) / (pstVencCfg->vencStream[i][j].cfg.vbrMaxBps << 7);
            }
            else if(u32RcMode == VENC_RC_MODE_H265VBR)
            {
                u32VencDoneQueueSize += (2*1024*1024)  * IPC_PF_GetViFpsForVenc(i,j) / (pstVencCfg->vencStream[i][j].cfg.vbrMaxBps << 7);
            }
            else if(u32RcMode == VENC_RC_MODE_MJPEGCBR)
            {
                u32VencDoneQueueSize += (pstVencCfg->vencStream[i][j].cfg.roi_width * pstVencCfg->vencStream[i][j].cfg.roi_height)
                        * IPC_PF_GetViFpsForVenc(i,j)
                        / (pstVencCfg->vencStream[i][j].cfg.cbrAvgBps << 7);
            }
            else if(u32RcMode == VENC_RC_MODE_MJPEGVBR)
            {
                u32VencDoneQueueSize += (pstVencCfg->vencStream[i][j].cfg.roi_width * pstVencCfg->vencStream[i][j].cfg.roi_height)
                        * IPC_PF_GetViFpsForVenc(i,j)
                        / (pstVencCfg->vencStream[i][j].cfg.vbrMaxBps << 7) ;
            }

            encoder_cnt++;
        }
    }

    VENC_PARAM_MOD_S stParam = {0};
    stParam.enVencModType = MODTYPE_H265E;
    s32Ret = AR_MPI_VENC_GetModParam(&stParam);

    if(s32Ret)
    {
        PRINT_ERR("Get venc mod param error! ret = %x\n", s32Ret);
        return;
    }

    stParam.stEventModParam.u32VencIrqQueueSize   = 64;
    stParam.stEventModParam.u32VencTaskQueueSize  = encoder_cnt * 16;
    stParam.stEventModParam.u32VencDoneQueueSize  = u32VencDoneQueueSize;
    /* if enable cmdqueue need enable below flag */
    stParam.stH265eModParam.u64Flags |= AR_VIDEO_CODEC_PARAM_HAL_EXT_INSTANCE_PARALLESIM;
    /*
    stParam.stEventModParam.u32VencOutQueueSize   = 16;
    stParam.stEventModParam.u32VencEventQueueSize = 16;
    */

    s32Ret = AR_MPI_VENC_SetModParam(&stParam);

    if(s32Ret)
    {
        PRINT_ERR("Set venc mod param error! ret = %x\n", s32Ret);
    }

    return;
}

static AR_S32 waitOnFd(AR_S32 devFd, AR_U32 timeout/*in seconds*/) {
    if (devFd < 0) {
        printf("invalid dev fd: %d\n", devFd);
        return -1;
    }

    fd_set rfds;
    struct timeval tv;
    AR_S32 retval;

    FD_ZERO(&rfds);
    FD_SET(devFd, &rfds);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    retval = select(devFd + 1, &rfds, NULL, NULL, &tv);

    if (retval == -1) {
        printf("select return error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static AR_S32 venc_debug_info(AR_U32 u32Ch, VENC_STREAM_S * pstStream, IPC_VENC_CODEC_TYPE_E u32CodecType)
{
    static unsigned long last_time[10] = {0};
    static unsigned long last_seq[10] = {0};
    static unsigned long count[10] = {0};

    count[u32Ch]++;
    unsigned long cur_time = getTickCount();
    unsigned long dur_time = cur_time - last_time[u32Ch];
    if (dur_time < 1000*3)
        return 0;

    char type[20];
    switch (u32CodecType) {
    case VENC_ENCODE_TYPE_H264:
        sprintf(type, "PT_H264");
        break;
    case VENC_ENCODE_TYPE_H265:
        sprintf(type, "PT_H265");
        break;
    case VENC_ENCODE_TYPE_MJPEG:
        sprintf(type, "PT_MJPEG");
    case VENC_ENCODE_TYPE_JPEG:
        sprintf(type, "PT_JPEG");
        break;
    default:
        sprintf(type, "PT_NULL");
    }

    float fps =count[u32Ch]*1000/ dur_time;

    printf("stream[%u] %s seq=%u, fps=%.2f, size=%u\n",
            u32Ch, type, pstStream->u32Seq, fps, pstStream->pstPack->u32Len);

    last_time[u32Ch] = cur_time;
    last_seq[u32Ch] = pstStream->u32Seq;
    count[u32Ch] = 0;

    return 0;
}

AR_S32 IPC_PF_VENC_GetStream(AR_S32 s32DevFd, AR_U32 u32Id, IPC_VENC_STREAM_S *pstVideoStream)
{
    AR_S32 s32Ret;
    VENC_CHN_STATUS_S stStatus;
    VENC_STREAM_S  stStream = {0};
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    AR_U32 u32Ch = u32Id / IPC_MAX_VENC_STREAM_PER_CHN;
    AR_U32 u32Stream = u32Id % IPC_MAX_VENC_STREAM_PER_CHN;
    AR_S32 timeout = 1;

    while (1)
    {
        if(g_s32VencThreadSuspend)
        {
            usleep(500*1000);
            continue;
        }
#ifdef THERMAL_ENABLE
#else
        waitOnFd(s32DevFd, timeout);
#endif
        memset(&stStatus, 0, sizeof(VENC_CHN_STATUS_S));
        s32Ret = AR_MPI_VENC_QueryStatus(u32Id, &stStatus);
        if (s32Ret)
        {
            PRINT_ERR("dev id:%d query status failed! ret=%d\n", u32Id, s32Ret);
            return -1;
        }

        if (stStatus.u32LeftStreamFrames > 0)
        {
            //PRINT_ERR("status left stream frames:%d\n", stStatus.u32LeftStreamFrames);
            break;
        }
        usleep(20 * 1000);
    }

    //PRINT_ERR("++++Got a stream from %d ++++\n", u32Id);

    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
    if (NULL == stStream.pstPack)
    {
        PRINT_ERR("malloc memory failed!\n");
        return -1;;
    }

    stStream.u32PackCount = stStatus.u32CurPacks;
    s32Ret = AR_MPI_VENC_GetStream(u32Id, &stStream, -1);
    if (AR_SUCCESS != s32Ret)
    {
        PRINT_ERR("AR_MPI_VENC_GetStream failed with %d!\n", s32Ret);

        free(stStream.pstPack);
        return -1;
    }

    //PRINT_ERR("++++Put a stream %d to mbuf header : type %d, len %d ++++\n",u32Id, pstStream->pstPack->DataType.enH265EType, pstStream->pstPack->u32Len);
    pstVideoStream->u32CodecType = pstVencCfg->vencStream[u32Ch][u32Stream].cfg.encodeType;
    //venc_debug_info(u32Id, pstStream, pstVideoStream->u32CodecType);

    if(pstVideoStream->u32CodecType == VENC_ENCODE_TYPE_H264)
    {
        if(stStream.pstPack[0].DataType.enH264EType == H264E_NALU_IDRSLICE)
        {
            pstVideoStream->u32FrameType = IPC_FRAME_TYPE_IDR;
        }
        else if(stStream.pstPack[0].DataType.enH264EType == H264E_NALU_ISLICE)
        {
            pstVideoStream->u32FrameType = IPC_FRAME_TYPE_I;
        }
        else //if(stStream.pstPack->DataType.enH264EType == H264E_NALU_PSLICE)
        {
            pstVideoStream->u32FrameType = IPC_FRAME_TYPE_P;
        }
    }
    else if(pstVideoStream->u32CodecType == VENC_ENCODE_TYPE_H265)
    {
        if(stStream.pstPack[0].DataType.enH265EType == H265E_NALU_IDRSLICE)
        {
            pstVideoStream->u32FrameType = IPC_FRAME_TYPE_IDR;
        }
        else if(stStream.pstPack[0].DataType.enH265EType == H265E_NALU_ISLICE)
        {
            pstVideoStream->u32FrameType = IPC_FRAME_TYPE_I;
        }
        else //if(stStream.pstPack->DataType.enH264EType == H264E_NALU_PSLICE)
        {
            pstVideoStream->u32FrameType = IPC_FRAME_TYPE_P;
        }
    }
    else
    {
        PRINT_ERR("Not supported encode type: %d\n", pstVideoStream->u32CodecType);
        AR_MPI_VENC_ReleaseStream(u32Id, &stStream);
        free(stStream.pstPack);
        return -1;
    }

    //in case of multiple packs, we concat them to one buffer for Mbuf to handle it more easily.
    pstVideoStream->u64FrameNo = stStream.u32Seq;
    pstVideoStream->u64Pts = stStream.pstPack[0].u64PTS;
    pstVideoStream->u64Len = 0;
    pstVideoStream->u64Offset = 0;
    for(int i = 0; i < stStream.u32PackCount; i++)
    {
        pstVideoStream->u64Len += stStream.pstPack[i].u32Len;
    }
    pstVideoStream->u64AddrVirt = (AR_U64)malloc(pstVideoStream->u64Len);
    if(!pstVideoStream->u64AddrVirt)
    {
        PRINT_ERR("Malloc stream pack error!\n");
        AR_MPI_VENC_ReleaseStream(u32Id, &stStream);
        free(stStream.pstPack);
        return -1;
    }

    memcpy((AR_CHAR *)pstVideoStream->u64AddrVirt, stStream.pstPack[0].pu8Addr, stStream.pstPack[0].u32Len);
    for(int i = 1; i < stStream.u32PackCount; i++)
    {
        memcpy((AR_CHAR *)(pstVideoStream->u64AddrVirt + stStream.pstPack[i-1].u32Len), stStream.pstPack[i].pu8Addr, stStream.pstPack[i].u32Len);
    }

    //pstVideoStream->private_buffer = pstStream;

    AR_MPI_VENC_ReleaseStream(u32Id, &stStream);
    free(stStream.pstPack);

    //Save to file for debug
    //static int cnt = 0;
    //if(cnt++ < 300)
    if(0)
    {
        FILE * fp = NULL;
        fp = fopen("/local/venc.bin", "ab+");
        if(!fp)
        {
            return 0;
        }

        PRINT_INFO("Save frame %lld to file...\n", pstVideoStream->u64FrameNo);

        fwrite((AR_CHAR *)(pstVideoStream->u64AddrVirt + pstVideoStream->u64Offset), pstVideoStream->u64Len, 1, fp);
        fclose(fp);
    }

    return 0;
}
#if 0
AR_S32 IPC_PF_VENC_GetStream(AR_U32 u32Id, IPC_VENC_STREAM_S *pstVideoStream)
{
    AR_S32 s32Ret;
    AR_S32 s32VencFd = -1;
    struct timeval TimeoutVal;
    fd_set read_fds;

    VENC_CHN_STATUS_S stStatus;
    VENC_STREAM_S * pstStream;
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    AR_U32 u32Ch = u32Id / IPC_MAX_STREAM_PER_PIPE;
    AR_U32 u32Stream = u32Id % IPC_MAX_VENC_STREAM_PER_CHN;

    s32VencFd = AR_MPI_VENC_GetFd(u32Id);
    if (s32VencFd < 0)
    {
        PRINT_ERR("AR_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return AR_FAILURE;
    }

    FD_ZERO(&read_fds);
    FD_SET(s32VencFd, &read_fds);
    TimeoutVal.tv_sec  = 10;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret <= 0)
    {
        PRINT_ERR("Venc getstream select failed: %s!\n", strerror(errno));
        AR_MPI_VENC_CloseFd(u32Id);
        return AR_FAILURE;
    }

    if (FD_ISSET(s32VencFd, &read_fds))
    {
        s32Ret = AR_MPI_VENC_QueryStatus(u32Id, &stStatus);
        if (s32Ret)
        {
            PRINT_ERR("dev id:%d query status failed! ret=%d\n", u32Id, s32Ret);
            AR_MPI_VENC_CloseFd(u32Id);
            return -1;
        }

        if (stStatus.u32LeftStreamFrames == 0)
        {
            PRINT_ERR("venc %d no frame\n", u32Id);
            AR_MPI_VENC_CloseFd(u32Id);
            return -1;
        }

        pstStream = (VENC_STREAM_S *)malloc(sizeof(VENC_STREAM_S));
    if(!pstStream)
    {
      PRINT_ERR("Malloc failed.\n");
            AR_MPI_VENC_CloseFd(u32Id);
      return -1;
    }

    pstStream->pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
    if (NULL == pstStream->pstPack)
    {
      PRINT_ERR("malloc memory failed!\n");
            AR_MPI_VENC_CloseFd(u32Id);
      return -1;;
    }

    pstStream->u32PackCount = 1;//stStatus.u32CurPacks;
    s32Ret = AR_MPI_VENC_GetStream(u32Id, pstStream, -1);
    if (AR_SUCCESS != s32Ret)
    {
      PRINT_ERR("AR_MPI_VENC_GetStream failed with %d!\n", s32Ret);

      free(pstStream->pstPack);
      pstStream->pstPack = NULL;
            AR_MPI_VENC_CloseFd(u32Id);
      return -1;
    }
    //PRINT_ERR("++++Put a stream %d to mbuf header : type %d, len %d ++++\n",u32Id, pstStream->pstPack->DataType.enH265EType, pstStream->pstPack->u32Len);
    pstVideoStream->u32CodecType = pstVencCfg->vencStream[u32Ch][u32Stream].cfg.encodeType;
    if(pstVideoStream->u32CodecType == VENC_ENCODE_TYPE_H264)
    {
      if(pstStream->pstPack->DataType.enH264EType == H264E_NALU_IDRSLICE)
      {
        pstVideoStream->u32FrameType = IPC_FRAME_TYPE_IDR;
      }
      else if(pstStream->pstPack->DataType.enH264EType == H264E_NALU_ISLICE)
      {
        pstVideoStream->u32FrameType = IPC_FRAME_TYPE_I;
      }
      else //if(stStream.pstPack->DataType.enH264EType == H264E_NALU_PSLICE)
      {
        pstVideoStream->u32FrameType = IPC_FRAME_TYPE_P;
      }
    }
    else if(pstVideoStream->u32CodecType == VENC_ENCODE_TYPE_H265)
    {
      if(pstStream->pstPack->DataType.enH265EType == H265E_NALU_IDRSLICE)
      {
        pstVideoStream->u32FrameType = IPC_FRAME_TYPE_IDR;
      }
      else if(pstStream->pstPack->DataType.enH265EType == H265E_NALU_ISLICE)
      {
        pstVideoStream->u32FrameType = IPC_FRAME_TYPE_I;
      }
      else //if(stStream.pstPack->DataType.enH264EType == H264E_NALU_PSLICE)
      {
        pstVideoStream->u32FrameType = IPC_FRAME_TYPE_P;
      }
    }
    else
    {
      PRINT_ERR("Not supported encode type: %d\n", pstVideoStream->u32CodecType);
      AR_MPI_VENC_ReleaseStream(u32Id, pstStream);
            AR_MPI_VENC_CloseFd(u32Id);
      return -1;
    }

    pstVideoStream->u64FrameNo = pstStream->u32Seq;
    pstVideoStream->u64Pts = pstStream->pstPack->u64PTS;
    pstVideoStream->u64AddrVirt = (AR_U64)pstStream->pstPack->pu8Addr;
    pstVideoStream->u64Offset = pstStream->pstPack->u32Offset;
    pstVideoStream->u64Len = pstStream->pstPack->u32Len;
    pstVideoStream->private_buffer = pstStream;

    //Save to file for debug
    static int cnt = 0;
    //if(cnt++ < 300)
    if(0)
    {
      FILE * fp = NULL;
      fp = fopen("/local/venc.bin", "ab+");
      if(!fp)
      {
        return 0;
      }

      PRINT_ERR("Save frame %d to file...\n", pstVideoStream->u64FrameNo);

      fwrite(pstVideoStream->u64AddrVirt + pstVideoStream->u64Offset, pstVideoStream->u64Len, 1, fp);
      fclose(fp);
    }
    }

    AR_MPI_VENC_CloseFd(u32Id);

  return AR_SUCCESS;
}
#endif
AR_S32 IPC_PF_VENC_ReleaseStream(AR_U32        u32Id, IPC_VENC_STREAM_S *pstVideoStream)
{
    if(pstVideoStream->u64AddrVirt)
    {
        free((void *)pstVideoStream->u64AddrVirt);
    }

    return 0;
}

/************ static functions ***********************/
static void* IPC_PF_THREAD_ReadVStream(void *args)
{
  AR_U32 u32Id = *(AR_U32 *)args;
  AR_CHAR achName[32] = {0};
  IPC_VENC_STREAM_S stVencStream = {0};
  IPC_FRAME_HEADER_S  stFrameHeader;
  IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
  AR_S32 s32Ret = 0;
  AR_U32 u32Ch = u32Id / IPC_MAX_VENC_STREAM_PER_CHN;
  AR_U32 u32Stream = u32Id % IPC_MAX_VENC_STREAM_PER_CHN;

  sprintf(achName, "read_v%d", u32Id);
  prctl(PR_SET_NAME, achName, 0,0,0);

  while(1)
  {
    if(s_astVThreadStatus[u32Id].u32Stop)
    {
      PRINT_ERR("IPC_PF_THREAD_ReadVStream %d, exit\n", s32Ret);
      break;
    }

    if(g_s32VencThreadSuspend)
    {
        usleep(500*1000);
        continue;
    }

    //get stream
    s32Ret = IPC_PF_VENC_GetStream(g_s32VencDevFd[u32Id], u32Id, &stVencStream);
    if(s32Ret < 0)
    {
      PRINT_ERR("Get stream error %d, exit\n", s32Ret);
      return NULL;
    }

    stFrameHeader.magic = 0XF3B0A4B8;
    stFrameHeader.device_type = 0;
    stFrameHeader.frame_size = stVencStream.u64Len;
    stFrameHeader.pts = stVencStream.u64Pts;
    stFrameHeader.sec = 0;
    stFrameHeader.usec = stVencStream.u64Pts;
    stFrameHeader.frame_type = stVencStream.u32FrameType;
    stFrameHeader.frame_rate = pstVencCfg->vencStream[u32Ch][u32Stream].cfg.fps;
    stFrameHeader.width = pstVencCfg->vencStream[u32Ch][u32Stream].cfg.width;
    stFrameHeader.height = pstVencCfg->vencStream[u32Ch][u32Stream].cfg.height;
    stFrameHeader.frame_no = stVencStream.u64FrameNo;

    s32Ret = IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetVideoWriterId(u32Id), (void *)(stVencStream.u64AddrVirt + stVencStream.u64Offset), stVencStream.u64Len, &stFrameHeader);
    IPC_PF_VENC_ReleaseStream(u32Id, &stVencStream);
  }

  return NULL;
}

void IPC_PF_VENC_MbufThread_Suspend()
{
	g_s32VencThreadSuspend = 1;
	printf("IPC_PF_VENC_MbufThread_Suspend\n");
}

void IPC_PF_VENC_MbufThread_Resume()
{
	g_s32VencThreadSuspend = 0;
	printf("IPC_PF_VENC_MbufThread_Resume\n");
}



//here to config and start mipi
//For mipi device, we set vipipe = mipi_id
//For dvp device, we use IPC_PF_PIPE_2_DVP_ID .
static AR_S32 AR_VI_StartMipi(int mipiDev, IPC_SNS_TYPE_E enSensorType, AR_CHAR * strPowerGpio, AR_CHAR * strResetGpio, AR_CHAR *strCommGpio)
{
  AR_S32 s32Ret = AR_SUCCESS;
  STRU_COMBO_DEV_ATTR_T ComboAttr={0};
    ENUM_GPIO_GROUP enGrp = 0;
    ENUM_GPIO_PORT enPort = 0;
    AR_U32 u32GpioNum = 0;

  IPC_PF_VI_GetComboAttrBySns(enSensorType, 0, &ComboAttr);

    s32Ret = AR_GPIO_Trans(strPowerGpio, &enGrp, &enPort, &u32GpioNum);
    if(s32Ret == 0) //else use default.
    {
        ComboAttr.dev_power_attr.power_gpio[0] = enGrp;
        ComboAttr.dev_power_attr.power_gpio[1] = enPort;
        ComboAttr.dev_power_attr.power_gpio[2] = u32GpioNum;
    }

    s32Ret = AR_GPIO_Trans(strResetGpio, &enGrp, &enPort, &u32GpioNum);
    if(s32Ret == 0) //else use default.
    {
        ComboAttr.dev_power_attr.reset_gpio[0] = enGrp;
        ComboAttr.dev_power_attr.reset_gpio[1] = enPort;
        ComboAttr.dev_power_attr.reset_gpio[2] = u32GpioNum;
    }

    s32Ret = AR_GPIO_Trans(strCommGpio, &enGrp, &enPort, &u32GpioNum);
    if(s32Ret == 0) //else use default.
    {
        ComboAttr.dev_power_attr.common_gpio[0] = enGrp;
        ComboAttr.dev_power_attr.common_gpio[1] = enPort;
        ComboAttr.dev_power_attr.common_gpio[2] = u32GpioNum;
    }

    if(enSensorType == TP9930_DVP_4VC_1080P_25FPS || enSensorType == TP9930_DVP_4VC_1080P_30FPS)
    {
        ComboAttr.devno = IPC_PF_PIPE_2_DVP_ID(mipiDev);
    }
    else //mipi dev
    {
        ComboAttr.devno = mipiDev;
    }

    PRINT_INFO("Snstype %d input data type: %d\n", enSensorType, ComboAttr.mipi_attr.input_data_type);
  s32Ret=AR_MPI_VI_SetComboDevAttr(&ComboAttr);

  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("ar_mipi_ioctl AR_MIPI_SET_DEV_ATTR fail %#x!\n", s32Ret);
    return AR_FAILURE;
  }
  return AR_SUCCESS;
}

static IPC_SNS_TYPE_E AR_VI_GetSensorType(AR_CHAR * name, AR_CHAR WDREnable, AR_CHAR u8Fps)
{
    if(strcmp(name, "imx307") == 0)
    {
      if(u8Fps > 30)
      {
        PRINT_ERR("No setting for %s %d\n", name, u8Fps);
        return IPC_SNS_TYPE_BUTT;
      }

      if(WDREnable)
      {
        return SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1;
      }
      else
      {
        return SONY_IMX307_MIPI_2M_30FPS_12BIT;
      }
    }

    if(strcmp(name, "imx415") == 0)
    {
      if(WDREnable)
      {
        if(u8Fps <= 30)
        {
          return SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1;
        }
        else
        {
          return SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1;
        }
      }
      else
      {
        if(u8Fps <= 30)
        {
          return SONY_IMX415_MIPI_8M_30FPS_12BIT;
        }
        else
        {
          return SONY_IMX415_MIPI_8M_60FPS_12BIT;
        }
      }
    }

    if(strcmp(name, "imx464") == 0)
    {
      if(WDREnable)
      {
        return SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1;
      }
      else
      {
        return SONY_IMX464_MIPI_4M_30FPS_12BIT;
      }
    }

  if(strcmp(name, "sc910gs") == 0)
  {
    if(WDREnable)
    {
      //return SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1;
    }
    else
    {
      return SC_SC910GS_MIPI_9M_20FPS_10BIT;
    }
  }

    if(strcmp(name, "tp9930") == 0)
    {
        if(u8Fps == 25)
        {
            return TP9930_DVP_4VC_1080P_25FPS;
        }
        else
        {
            return TP9930_DVP_4VC_1080P_30FPS;
        }
    }
  if(strcmp(name, "sc530ai") == 0)
  {
    if(WDREnable)
    {
      //
    }
    else
    {
                    return SC_SC530AI_MIPI_5M_30FPS_10BIT;
    }
  }

  if(strcmp(name, "ov04a10") == 0)
    {
      if(WDREnable)
      {
        return OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1;
      }
      else
      {
        return OV_OS04A10_MIPI_4M_25FPS_12BIT;
      }
    }
#ifdef THERMAL_ENABLE
    if(strcmp(name, "isp_in") == 0)
    {
        return ISP_VIN_COMMON_INF;
    }
    if(strcmp(name, "timo256") == 0)
    {
        return TIMO256_DVP_256_IR;
    }

    if(strcmp(name, "elf1") == 0)
    {
        return ELF1_DVP_256_IR;
    }

    if(strcmp(name, "nk1221a") == 0)
    {
        return NK1221A_DVP_384_IR;
    }
#endif
    if(strcmp(name, "imx347") == 0)
    {
        return SONY_IMX347_MIPI_4M_30FPS_12BIT;
    }
    if(strcmp(name, "ar_gst417w") == 0)
    {
        return GST417W_IR_400X308;
    }
    if(strcmp(name, "ar_gst212w4") == 0)
    {
        return GST212W4_IR_256X200;
    }
    if(strcmp(name, "ov05a20") == 0)
    {
        return OV_OS05A20_2688x1944;
    }
    if(strcmp(name, "h3812c1sg") == 0)
    {
        return H3812C1SG_IR_444X336;
    }
    if(strcmp(name, "sc230ai") == 0)
    {
        return SC_SC230AI_MIPI_2M_30FPS_10BIT;
    }

    PRINT_ERR("sensor %s not supported.\n", name);

    return IPC_SNS_TYPE_BUTT;
}

AR_S32 AR_VI_CreateSingleVi(VI_DEV ViDev, VI_PIPE ViPipe, int mipiDev,
              IPC_SNS_TYPE_E enSensorType, WDR_MODE_E enWDRMode, ENUM_VFE_MODE enVfe,
              AR_CHAR * strPowerGpio, AR_CHAR * strResetGpio, AR_CHAR * strCommGpio, AR_BOOL bRawCompress)
{
  AR_S32 s32Ret = AR_SUCCESS;
  VI_DEV_ATTR_S stViDevAttr = {0};
  VI_DEV_BIND_PIPE_S stDevBindPipe = {0};
  VI_PIPE_ATTR_S stPipeAttr = {0};
  VI_CHN_ATTR_S stChnAttr = {0};

  IPC_PF_VI_GetDevAttrBySns(enSensorType, &stViDevAttr);
  stViDevAttr.stWDRAttr.enWDRMode = enWDRMode;

    if(enSensorType == TP9930_DVP_4VC_1080P_25FPS || enSensorType == TP9930_DVP_4VC_1080P_30FPS)
    {
        stDevBindPipe.u32Num = 4;
        stDevBindPipe.PipeId[0] = ViPipe;
        stDevBindPipe.PipeId[1] = ViPipe + 1;
        stDevBindPipe.PipeId[2] = ViPipe + 2;
        stDevBindPipe.PipeId[3] = ViPipe + 3;
    }
    else
    {
        stDevBindPipe.u32Num = 1;
        stDevBindPipe.PipeId[0] = ViPipe;
    }

  s32Ret = AR_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);

  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  s32Ret=AR_VI_StartMipi(mipiDev, enSensorType, strPowerGpio, strResetGpio, strCommGpio);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_VI_StartMipi failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  s32Ret = AR_MPI_VI_EnableDev(ViDev);

  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  //stDevBindPipe.u32Num = 1;
  //stDevBindPipe.PipeId[0] = ViPipe;
  s32Ret = AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);

  IPC_PF_VI_GetPipeAttrBySns(enSensorType, &stPipeAttr);
    stPipeAttr.enVfeMode = enVfe;
    if(bRawCompress)
    {
        stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
    }
    else
    {
        stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
        stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
    }

  s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
    goto EXIT1;
  }

  s32Ret = AR_MPI_VI_StartPipe(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
    goto EXIT2;
  }

  IPC_PF_VI_GetChnAttrBySns(enSensorType, &stChnAttr);
    stChnAttr.u32BufCount = 5;


  return AR_SUCCESS;

EXIT2:

  s32Ret = AR_MPI_VI_StopPipe(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

EXIT1:
  s32Ret  = AR_MPI_VI_DisableDev(ViDev);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  return s32Ret;
}

static AR_S32 AR_VI_CreateSingleViDvp4Ch(VI_DEV ViDev, VI_PIPE ViPipe,
            IPC_SNS_TYPE_E enSensorType, WDR_MODE_E enWDRMode, ENUM_VFE_MODE enVfe,
            AR_CHAR * strPowerGpio, AR_CHAR * strResetGpio)
{
    AR_S32 s32Ret = AR_SUCCESS;
    VI_DEV_ATTR_S stViDevAttr = {0};
    VI_DEV_BIND_PIPE_S stDevBindPipe = {0};
    VI_PIPE_ATTR_S stPipeAttr = {0};
    VI_CHN_ATTR_S stChnAttr = {0};
    VI_CHN ViChn = 0;
    int i = 0;

    IPC_PF_VI_GetDevAttrBySns(enSensorType, &stViDevAttr);
    stViDevAttr.stWDRAttr.enWDRMode = enWDRMode;

    //for TP9930_DVP_4VC_1080P_25FPS and TP9930_DVP_4VC_1080P_30FPS
    {
        stDevBindPipe.u32Num = 4;
        stDevBindPipe.PipeId[0] = ViPipe;
        stDevBindPipe.PipeId[1] = ViPipe + 1;
        stDevBindPipe.PipeId[2] = ViPipe + 2;
        stDevBindPipe.PipeId[3] = ViPipe + 3;
    }

    s32Ret = AR_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);

    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret=AR_VI_StartMipi(ViPipe, enSensorType, strPowerGpio, strResetGpio, NULL);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_VI_StartMipi failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_EnableDev(ViDev);

    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);

    IPC_PF_VI_GetPipeAttrBySns(enSensorType, &stPipeAttr);
    stPipeAttr.enVfeMode = enVfe;

    for(i = 0; i < 4; i++)
    {
        s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("AR_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
            goto EXIT1;
        }

        s32Ret = AR_MPI_VI_SetPipeVCNumber(ViPipe, ViPipe);
        if (s32Ret != AR_SUCCESS)
        {
            AR_MPI_VI_DestroyPipe(ViPipe);
            PRINT_ERR("AR_MPI_VI_SetPipeVCNumber failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VI_StartPipe(ViPipe);
        if (s32Ret != AR_SUCCESS)
        {
            AR_MPI_VI_DestroyPipe(ViPipe);
            PRINT_ERR("AR_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        IPC_PF_VI_GetChnAttrBySns(enSensorType, &stChnAttr);
        stChnAttr.u32BufCount = 5;

        //must start channel 0 for a pipe
        s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("AR_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        //enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;
        s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);

        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("SAMPLE_COMM_VI_StartViChn failed !\n");
            goto EXIT2;
        }

        ViPipe++;
    }

    return AR_SUCCESS;

  EXIT2:

    s32Ret = AR_MPI_VI_StopPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

  EXIT1:
    s32Ret  = AR_MPI_VI_DisableDev(ViDev);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return s32Ret;
}

static AR_S32 AR_VI_CreateSingleIspDvp4Ch(VI_DEV ViDev, VI_PIPE ViPipe,
             IPC_SNS_TYPE_E enSnsType, AR_U32 u32BusId, WDR_MODE_E enWDRMode, AR_U32 u32Fps)
{
    AR_S32              i;
    //AR_BOOL             bNeedPipe;
    AR_S32              s32Ret = AR_SUCCESS;
    ISP_PUB_ATTR_S      stPubAttr;
    //VI_PIPE_ATTR_S      stPipeAttr;

    IPC_PF_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);
    stPubAttr.enWDRMode = enWDRMode;

    for(i = 0; i < 4; i++)
    {
        s32Ret = IPC_PF_ISP_Sensor_Regiter_callback(ViPipe, enSnsType);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("register sensor %d to ISP %d failed\n", ViDev, ViPipe);
            return AR_FAILURE;
        }

        s32Ret = IPC_PF_ISP_BindSns(ViPipe, enSnsType, u32BusId);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("register sensor %d bus id %d failed\n", ViDev, u32BusId);
            IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_ISP_MemInit(ViPipe);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("Init Ext memory failed with %#x!\n", s32Ret);
            IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
            return AR_FAILURE;
        }

        stPubAttr.f32FrameRate = u32Fps;
        s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("SetPubAttr failed with %#x!\n", s32Ret);
            IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_ISP_Init(ViPipe);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("ISP Init failed with %#x!\n", s32Ret);
            IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
            return AR_FAILURE;
        }

        s32Ret = IPC_PF_ISP_Run(ViPipe);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("ISP Run failed with %#x!\n", s32Ret);
            IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
            return AR_FAILURE;
        }

        //2022-04-21
        AR_MPI_ISP_StartSnsDetect(ViPipe);

        ViPipe++;
    }

    return s32Ret;
}



AR_S32 AR_VI_CreateSingleIsp(VI_DEV ViDev, VI_PIPE ViPipe,
             IPC_SNS_TYPE_E enSnsType, AR_U32 u32BusId, WDR_MODE_E enWDRMode, AR_U32 u32Fps)
{
  //AR_S32              i;
  //AR_BOOL             bNeedPipe;
  //VI_PIPE_ATTR_S      stPipeAttr;
  AR_S32              s32Ret = AR_SUCCESS;
  ISP_PUB_ATTR_S      stPubAttr;

  IPC_PF_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);
  stPubAttr.enWDRMode = enWDRMode;
  s32Ret = IPC_PF_ISP_Sensor_Regiter_callback(ViPipe, enSnsType);
  if (AR_SUCCESS != s32Ret)
  {
    PRINT_ERR("register sensor %d to ISP %d failed\n", ViDev, ViPipe);
    return AR_FAILURE;
  }

  s32Ret = IPC_PF_ISP_BindSns(ViPipe, enSnsType, u32BusId);
  if (AR_SUCCESS != s32Ret)
  {
    PRINT_ERR("register sensor %d bus id %d failed\n", ViDev, u32BusId);
    IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
    return AR_FAILURE;
  }

  s32Ret = AR_MPI_ISP_MemInit(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("Init Ext memory failed with %#x!\n", s32Ret);
    IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
    return AR_FAILURE;
  }

    stPubAttr.f32FrameRate = u32Fps;
  s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("SetPubAttr failed with %#x!\n", s32Ret);
    IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
    return AR_FAILURE;
  }

  s32Ret = AR_MPI_ISP_Init(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("ISP Init failed with %#x!\n", s32Ret);
    IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
    return AR_FAILURE;
  }

  s32Ret = IPC_PF_ISP_Run(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("ISP Run failed with %#x!\n", s32Ret);
    IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
    return AR_FAILURE;
  }

  return s32Ret;
}

AR_S32 AR_VI_StopSingleIsp(VI_PIPE ViPipe, IPC_SNS_TYPE_E enSnsType)
{
    AR_MPI_ISP_StopSnsDetect(ViPipe);
  IPC_PF_ISP_Stop(ViPipe);
  IPC_PF_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);

  return 0;
}

AR_S32 AR_VI_StopSingleVi(VI_DEV ViDev, VI_PIPE ViPipe)
{
  AR_S32 s32Ret = 0;
  s32Ret = AR_MPI_VI_StopPipe(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  s32Ret  = AR_MPI_VI_DisableDev(ViDev);

  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
    return AR_FAILURE;
  }

  return AR_SUCCESS;
}

AR_S32 AR_VI_StopSingleViDvp4Ch(VI_DEV ViDev, VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;
    s32Ret = AR_MPI_VI_StopPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    if(ViPipe == 3 || ViPipe == 7) //vi and isp only need once for every 4 channels
    {
        s32Ret  = AR_MPI_VI_DisableDev(ViDev);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }

    return AR_SUCCESS;
}


static AR_S32 AR_VO_StartHdmi()
{
  AR_HDMI_ATTR_S          stAttr;
  AR_HDMI_VIDEO_FMT_E     enVideoFmt = AR_HDMI_VIDEO_FMT_1080P_60;
  AR_HDMI_ID_E            enHdmiId    = AR_HDMI_ID_0;

  memset(&stAttr, 0, sizeof(AR_HDMI_ATTR_S));

  AR_MPI_HDMI_Init();
  AR_MPI_HDMI_Open(enHdmiId);
  AR_MPI_HDMI_GetAttr(enHdmiId, &stAttr);

  stAttr.bEnableHdmi           = AR_TRUE;
  stAttr.bEnableVideo          = AR_TRUE;
  stAttr.enVideoFmt            = enVideoFmt;
  stAttr.enVidOutMode          = AR_HDMI_VIDEO_MODE_YCBCR444;
  stAttr.enDeepColorMode = AR_HDMI_DEEP_COLOR_24BIT;

  stAttr.bxvYCCMode            = AR_FALSE;
  stAttr.enOutCscQuantization  = HDMI_QUANTIZATION_LIMITED_RANGE;

  stAttr.bEnableAudio          = AR_FALSE;
  stAttr.enSoundIntf           = AR_HDMI_SND_INTERFACE_I2S;
  stAttr.bIsMultiChannel       = AR_FALSE;

  stAttr.enBitDepth            = AR_HDMI_BIT_DEPTH_16;

  stAttr.bEnableAviInfoFrame   = AR_TRUE;
  stAttr.bEnableAudInfoFrame   = AR_TRUE;
  stAttr.bEnableSpdInfoFrame   = AR_FALSE;
  stAttr.bEnableMpegInfoFrame  = AR_FALSE;

  stAttr.bDebugFlag            = AR_FALSE;
  stAttr.bHDCPEnable           = AR_FALSE;

  stAttr.b3DEnable             = AR_FALSE;
  stAttr.enDefaultMode         = AR_HDMI_FORCE_HDMI;

  AR_MPI_HDMI_SetAttr(enHdmiId, &stAttr);
  AR_MPI_HDMI_Start(enHdmiId);

  return 0;
}

static AR_S32 AR_VO_StopHdmi()
{
  AR_HDMI_ID_E enHdmiId = AR_HDMI_ID_0;

  AR_MPI_HDMI_Stop(enHdmiId);
  AR_MPI_HDMI_Close(enHdmiId);
  AR_MPI_HDMI_DeInit();

  return AR_SUCCESS;
}

static AR_S32 AR_VO_SetLayerChnRes(AR_U32 u32Width, AR_U32 u32Height)
{
#if 0 //vo can't be disabled for some reason, will fix it later.
    AR_S32 s32Ret = 0;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    VO_CHN_ATTR_S stChnAttr = {0};
    VO_LAYER VoLayer = 0;
    VO_CHN VoChn = 0;
    s32Ret = AR_MPI_VO_DisableChn(VoLayer, VoChn);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("Disable vo chn failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("disable video layer failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("Get video layer attr failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    stLayerAttr.stDispRect.u32Width = u32Width;
    stLayerAttr.stDispRect.u32Height = u32Height;
    stLayerAttr.stImageSize.u32Width = u32Width;
    stLayerAttr.stImageSize.u32Height = u32Height;

    s32Ret = AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("Set video layer attr failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_GetChnAttr(VoLayer, VoChn, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("Get vo chn attr failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    stChnAttr.stRect.u32Width = u32Width;
    stChnAttr.stRect.u32Height = u32Height;

    s32Ret = AR_MPI_VO_SetChnAttr(VoLayer, VoChn, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("VO set attr failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("VO enable video layer failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VO_EnableChn(VoLayer, VoChn);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("VO enable chn failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }
#endif
    return AR_SUCCESS;
}

static AR_U32 IPC_PF_CheckPipelineLowBwMode(IPC_CFG_CHANNEL_S * pChnCfg)
{
    AR_S32 i = 0;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(pChnCfg->stChannel[i].u32Enable)
        {
            if(strcmp(pChnCfg->stChannel[i].chType, "low_bw") == 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;
}

/*Wrapper to specific bind apis */
MOD_ID_E IPC_PF_MOD_TRANS(IPC_MOD_ID_E enMod)
{
  switch(enMod)
  {
    case MOD_ID_SYS :
      return AR_ID_SYS;
    case MOD_ID_VB  :
      return AR_ID_VB;
    case MOD_ID_VI  :
      return AR_ID_VI;
    case MOD_ID_VO  :
      return AR_ID_VO;
    case MOD_ID_VPSS:
      return AR_ID_VPSS;
    case MOD_ID_RGN :
      return AR_ID_RGN;
    case MOD_ID_VENC:
      return AR_ID_VENC;
    case MOD_ID_VDEC:
      return AR_ID_VDEC;
    case MOD_ID_VGS :
      return AR_ID_VGS;
    case MOD_ID_AI  :
      return AR_ID_AI;
    case MOD_ID_AO  :
      return AR_ID_AO;
    case MOD_ID_AENC:
      return AR_ID_AENC;
    case MOD_ID_ADEC:
      return AR_ID_ADEC;
    default:
      return AR_ID_BUTT;
  }
}

AR_S32 IPC_PF_Bind(IPC_MPP_CHN_S * pstSrcChn, IPC_MPP_CHN_S * pstDstChn)
{
  AR_S32 s32Ret;
  MPP_CHN_S stSrc = {0};
  MPP_CHN_S stDst = {0};

  PRINT_INFO("src[%d %d %d] to dst[%d %d %d]\n",
    pstSrcChn->enModId, pstSrcChn->u32DevId, pstSrcChn->u32ChnId,
    pstDstChn->enModId, pstDstChn->u32DevId, pstDstChn->u32ChnId);

  stSrc.enModId = IPC_PF_MOD_TRANS(pstSrcChn->enModId);
  stSrc.s32ChnId = pstSrcChn->u32ChnId;
  stSrc.s32DevId = pstSrcChn->u32DevId;
  stDst.enModId = IPC_PF_MOD_TRANS(pstDstChn->enModId);
  stDst.s32ChnId = pstDstChn->u32ChnId;
  stDst.s32DevId = pstDstChn->u32DevId;

  s32Ret = AR_MPI_SYS_Bind(&stSrc, &stDst);
  if(s32Ret)
  {
    PRINT_ERR("==Sys bind error: %d enModId:(%d->%d) u32ChnId:(%d->%d) u32DevId:(%d->%d)\n", s32Ret,
        pstSrcChn->enModId, pstDstChn->enModId,
        pstSrcChn->u32ChnId, pstDstChn->u32ChnId,
        pstSrcChn->u32DevId, pstDstChn->u32DevId);
    return s32Ret;
  }
  return 0;
}
AR_S32 IPC_PF_UnBind(IPC_MPP_CHN_S * pstSrcChn, IPC_MPP_CHN_S * pstDstChn)
{
  AR_S32 s32Ret;
  MPP_CHN_S stSrc = {0};
  MPP_CHN_S stDst = {0};

  PRINT_INFO("src[%d %d %d] to dst[%d %d %d]\n",
    pstSrcChn->enModId, pstSrcChn->u32DevId, pstSrcChn->u32ChnId,
    pstDstChn->enModId, pstDstChn->u32DevId, pstDstChn->u32ChnId);

  stSrc.enModId = IPC_PF_MOD_TRANS(pstSrcChn->enModId);
  stSrc.s32ChnId = pstSrcChn->u32ChnId;
  stSrc.s32DevId = pstSrcChn->u32DevId;
  stDst.enModId = IPC_PF_MOD_TRANS(pstDstChn->enModId);
  stDst.s32ChnId = pstDstChn->u32ChnId;
  stDst.s32DevId = pstDstChn->u32DevId;

  s32Ret = AR_MPI_SYS_UnBind(&stSrc, &stDst);
  if(s32Ret)
  {
    PRINT_ERR("==Sys Unbind error: %d enModId:(%d->%d) u32ChnId:(%d->%d) u32DevId:(%d->%d)\n", s32Ret,
        pstSrcChn->enModId, pstDstChn->enModId,
        pstSrcChn->u32ChnId, pstDstChn->u32ChnId,
        pstSrcChn->u32DevId, pstDstChn->u32DevId);
    return s32Ret;
  }

  return 0;
}

/* Create IR pipeline */
/* sns->thermal->isp0->isp1--bind--vpss--bind--venc*/
AR_S32 IPC_PF_Create_IRChannel(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    //IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    //IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();
    AR_S32 s32Ret = 0;
    int i = u32Index;
    //VPSS_GRP VpssGrp = 0;

    VI_IR_GROUP_S * pstViIRGrp = IPC_PF_GetViIRGroup();

    PRINT_INFO("Create ir channel...\n");
    //while(1){usleep(1000 * 1000);}
    stVI.enModId = MOD_ID_VI;
    if(pstViIRGrp->stViIsp1.enSnsType == IPC_SNS_TYPE_BUTT)
    {
        stVI.u32DevId = pstViIRGrp->stViIsp0.ViPipe;
        stVI.u32ChnId = pstViIRGrp->stViIsp0.ViChn;
    }
    else
    {
        stVI.u32DevId = pstViIRGrp->stViIsp1.ViPipe;
        stVI.u32ChnId = pstViIRGrp->stViIsp1.ViChn;
    }

    stVpss.enModId = MOD_ID_VPSS;
    stVpss.u32DevId = u32Index * 2; //we suppose other channels are RGB sensors, which pipeline is built with 2 vpss groups
    stVpss.u32ChnId = 0;

    stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;

    s32Ret = IPC_PF_Bind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
        PRINT_ERR("Create channel %d error.\n", i);
        return s32Ret;
    }

    if(pstVencCfg->vencStream[i][0].enable)
    {
        s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
        if(s32Ret < 0)
        {
            PRINT_ERR("Create channel %d error.\n", i);
            return s32Ret;
        }
    }

    if(pstVioCfg->vo.enable && i < pstVioCfg->vo.square * pstVioCfg->vo.square)
    {
        stVo.enModId = MOD_ID_VO;
        stVo.u32DevId = 0;
        stVo.u32ChnId = i;

        s32Ret = IPC_PF_Bind(&stVpss, &stVo);
        if(s32Ret < 0)
        {
            PRINT_ERR("Create channel %d error.\n", i);
            return s32Ret;
        }
    }

    return 0;
}

AR_S32 IPC_PF_Create_BasicChannel_DVP(AR_U32 u32Index)
{
    PRINT_ERR("Not supported, please set low_bw in cfg_channel.json when running DVP 8x1080p case\n");
    return 0;
}

/* Create a low-bw mipi channel pipeline */
/*                              osd
*camera0--main---vpss0---ch0----> ---+--->venc-->mbuf-main
*    |--sub--vpss1---ch0----> ---+--->venc-->mbuf-sub
*    |             |-ch1----> ---+--->venc-->mbuf-3rd
*    |             |-ch2----> VO
*    |             |-ch3----> ALG
*    |-- raw
*/
AR_S32 IPC_PF_Create_LowBandwidthChannel(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    //IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    IPC_CFG_VPSS_S * pstVpssCfg;
    AR_S32 s32Ret = 0;
    int i = u32Index;
    VPSS_GRP VpssGrp = 0;

    //pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();
    pstVpssCfg = IPC_CFG_VPSS_GetParam();

    stVI.enModId = MOD_ID_VI;
    stVI.u32DevId = i; //vi pipe
    stVI.u32ChnId = 0; // chn 0 or 1

    //vi -- ch0 -> vpss
    VpssGrp = i * 2;
    if(pstVpssCfg->stGrpCfg[VpssGrp].bGroup)
    {
        stVpss.enModId = MOD_ID_VPSS;
        stVpss.u32DevId = VpssGrp;
        stVpss.u32ChnId = 0;
        s32Ret = IPC_PF_Bind(&stVI, &stVpss);
        if(s32Ret < 0)
        {
            PRINT_ERR("Create channel %d error.\n", i);
            return s32Ret;
        }

        //main stream
        if(pstVencCfg->vencStream[i][0].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable)
        {
#ifdef ENABLE_VGS
            printf("[%s]%d: vgs_debug --------------------> not bind vpss to venc !!!!\n", __FILE__, __LINE__);
#else

            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = VpssGrp;
            stVpss.u32ChnId = 0;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
#endif
        }
    }
    else
    {
        if(pstVencCfg->vencStream[i][0].enable)
        {
            //bind vi to venc directly for main stream
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
            s32Ret = IPC_PF_Bind(&stVI, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
    }

    //For vi sub channel
    stVI.enModId = MOD_ID_VI;
    stVI.u32DevId = i;
    stVI.u32ChnId = 1;

    VpssGrp = VpssGrp + 1;
    if(pstVpssCfg->stGrpCfg[VpssGrp].bGroup && pstVioCfg->vi[i].channel_1_enable)
    {
        stVpss.enModId = MOD_ID_VPSS;
        stVpss.u32DevId = VpssGrp;
        stVpss.u32ChnId = 0;
        s32Ret = IPC_PF_Bind(&stVI, &stVpss);
        if(s32Ret < 0)
        {
            PRINT_ERR("Create channel %d error.\n", i);
            return s32Ret;
        }

        //sub stream
        if(pstVencCfg->vencStream[i][1].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable)
        {
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = VpssGrp;
            stVpss.u32ChnId = 0;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
        //3rd stream
        if(pstVencCfg->vencStream[i][2].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[1].bEnable)
        {
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = VpssGrp;
            stVpss.u32ChnId = 1;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 2;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
        //ch 2 to VO
        if(pstVioCfg->vo.enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].bEnable)
        {
            /* for single sensor, we set layer and chn w/h same as the input,
             * so that the display will scale the buffer automatically, instead of
             * calling system scaler to do it.
             * This way can help reduce the bw and process time of a single buffer.
             */
            if(pstVioCfg->vo.square == 1)
            {
                AR_VO_SetLayerChnRes(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].u32Width, pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].u32Height);
            }
#ifdef ENABLE_VGS
            printf("[%s]%d: vgs_debug --------------------> not bind vpss to vo !!!!\n", __FILE__, __LINE__);
#else

            //vo has square x square display max, each for a video.
            if(i < pstVioCfg->vo.square * pstVioCfg->vo.square)
            {
                stVpss.enModId = MOD_ID_VPSS;
                stVpss.u32DevId = VpssGrp;
                stVpss.u32ChnId = 2;
                stVo.enModId = MOD_ID_VO;
                stVo.u32DevId = 0;
                stVo.u32ChnId = i;
                s32Ret = IPC_PF_Bind(&stVpss, &stVo);
                if(s32Ret < 0)
                {
                    PRINT_ERR("Create channel %d error.\n", i);
                    return s32Ret;
                }
            }
#endif
        }

        //ch 3 to ALG
        if(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[3].bEnable)
        {
            PRINT_INFO("Vpss %d ch 3 enabled.\n", i);
        }

    }
    else
    {
        //no vpss, just bind vi sub channel to vo/venc
        if(pstVencCfg->vencStream[i][1].enable)
        {
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
            s32Ret = IPC_PF_Bind(&stVI, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }

        if(pstVioCfg->vo.enable)
        {
            if(pstVioCfg->vo.square == 1)
            {
                AR_VO_SetLayerChnRes(pstVioCfg->vi[i].width, pstVioCfg->vi[i].height);
            }

            stVo.enModId = MOD_ID_VO;
            stVo.u32DevId = 0;
            stVo.u32ChnId = i;
            s32Ret = IPC_PF_Bind(&stVI, &stVo);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
    }

    return 0;
}

/* Create a basic channel pipeline */
/*                              osd
* camera0 ---vpss ---ch0----> ---+--->venc-->mbuf-main
*                  |-ch1----> ---+--->venc-->mbuf-sub
*                  |-ch2----> ---+--->venc-->mbuf-3rd
*                  |-ch3----> VO
*                  |-ch4----> ALG
*/
AR_S32 IPC_PF_Create_BasicChannel_MIPI(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    //IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    IPC_CFG_VPSS_S * pstVpssCfg;
    AR_S32 s32Ret = 0;
    int i = u32Index;
    //VPSS_GRP VpssGrp = 0;

    //pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();
    pstVpssCfg = IPC_CFG_VPSS_GetParam();

    //create a basic pipeline
    stVI.enModId = MOD_ID_VI;
    stVI.u32DevId = i;
    stVI.u32ChnId = 0;
#ifdef THERMAL_ENABLE
    int streams_per_ch = 1;//IPC_MAX_VENC_STREAM_PER_CHN/2;
    IPC_SNS_TYPE_E sns = AR_VI_GetSensorType(pstVioCfg->vi[i].sensor,pstVioCfg->vi[i].HDR_enable ? WDR_MODE_2To1_LINE : WDR_MODE_NONE, pstVioCfg->vi[i].fps);
    int is_thermal = IPC_PF_IsThermalSensor(sns);
#endif
    //vi -- ch0 -> vpss
    if(pstVpssCfg->stGrpCfg[i].bGroup)
    {
#ifdef THERMAL_ENABLE
        if(!is_thermal){
#endif
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = i;
            stVpss.u32ChnId = 0;
            s32Ret = IPC_PF_Bind(&stVI, &stVpss);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
#ifdef THERMAL_ENABLE
        }
#endif
        //main stream
#ifdef THERMAL_ENABLE
        if(pstVencCfg->vencStream[0][i * streams_per_ch].enable && pstVpssCfg->stGrpCfg[i].stChnCfg[0].bEnable && !is_thermal)
        {
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = i;
            stVpss.u32ChnId = 0;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * streams_per_ch;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
#else
        if(pstVencCfg->vencStream[i][0].enable && pstVpssCfg->stGrpCfg[i].stChnCfg[0].bEnable)
        {
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = i;
            stVpss.u32ChnId = 0;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }

#endif
#ifndef THERMAL_ENABLE
        //sub stream
        if(pstVencCfg->vencStream[i][1].enable && pstVpssCfg->stGrpCfg[i].stChnCfg[1].bEnable)
        {
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = i;
            stVpss.u32ChnId = 1;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
        //3rd stream
        if(pstVencCfg->vencStream[i][2].enable && pstVpssCfg->stGrpCfg[i].stChnCfg[2].bEnable)
        {
            stVpss.enModId = MOD_ID_VPSS;
            stVpss.u32DevId = i;
            stVpss.u32ChnId = 2;
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 2;
            s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
#endif
        //ch 3 to VO
        if(pstVioCfg->vo.enable && pstVpssCfg->stGrpCfg[i].stChnCfg[3].bEnable)
        {
            /* for single sensor, we set layer and chn w/h same as the input,
             * so that the display will scale the buffer automatically, instead of
             * calling system scaler to do it.
             * This way can help reduce the bw and process time of a single buffer.
             */
            if(pstVioCfg->vo.square == 1)
            {
                AR_VO_SetLayerChnRes(pstVpssCfg->stGrpCfg[i].stChnCfg[3].u32Width, pstVpssCfg->stGrpCfg[i].stChnCfg[3].u32Height);
            }
            //vo has square x square display max, each for a video.
            if(i < pstVioCfg->vo.square * pstVioCfg->vo.square)
            {
                stVpss.enModId = MOD_ID_VPSS;
                stVpss.u32DevId = i;
                stVpss.u32ChnId = 3;
                stVo.enModId = MOD_ID_VO;
                stVo.u32DevId = 0;
                stVo.u32ChnId = i;
                s32Ret = IPC_PF_Bind(&stVpss, &stVo);
                if(s32Ret < 0)
                {
                    PRINT_ERR("Create channel %d error.\n", i);
                    return s32Ret;
                }
            }
        }
        //ch 4 to ALG
        if(pstVpssCfg->stGrpCfg[i].stChnCfg[4].bEnable)
        {
            PRINT_INFO("Vpss %d ch 4 enabled.\n", i);
        }
    }
    else
    {
        //no vpss, just bind vi and venc
        if(pstVencCfg->vencStream[0][i].enable){
            stVenc.enModId = MOD_ID_VENC;
            stVenc.u32DevId = 0;
            stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
            s32Ret = IPC_PF_Bind(&stVI, &stVenc);
            if(s32Ret < 0)
            {
                PRINT_ERR("Create channel %d error.\n", i);
                return s32Ret;
            }
        }
        if(pstVioCfg->vo.enable)
        {
            if(pstVioCfg->vo.square == 1)
            {
                AR_VO_SetLayerChnRes(pstVioCfg->vi[i].width, pstVioCfg->vi[i].height);
            }

            //vo has square x square display max, each for a video.
            if(i < pstVioCfg->vo.square * pstVioCfg->vo.square)
            {
                stVo.enModId = MOD_ID_VO;
                stVo.u32DevId = 0;
                stVo.u32ChnId = i;
                s32Ret = IPC_PF_Bind(&stVI, &stVo);
                if(s32Ret < 0)
                {
                    PRINT_ERR("Create channel %d error.\n", i);
                    return s32Ret;
                }
            }
        }
    }

    return 0;
}

#if 0
AR_S32 IPC_PF_Create_LowBandwidthChannel_MIPI(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    IPC_CFG_VPSS_S * pstVpssCfg;
    AR_S32 s32Ret = 0;
    int i = u32Index;
    VPSS_GRP VpssGrp = 0;

    pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();
    pstVpssCfg = IPC_CFG_VPSS_GetParam();

    //create a basic pipeline
  stVI.enModId = MOD_ID_VI;
  stVI.u32DevId = i;
  stVI.u32ChnId = 0;

  //vi -- ch0 -> vpss
  VpssGrp = i * 2;
  if(pstVpssCfg->stGrpCfg[VpssGrp].bGroup)
  {
    stVpss.enModId = MOD_ID_VPSS;
    stVpss.u32DevId = VpssGrp;
    stVpss.u32ChnId = 0;
    s32Ret = IPC_PF_Bind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
      PRINT_ERR("Create channel %d error.\n", i);
      return s32Ret;
    }

    //main stream
    if(pstVencCfg->vencStream[i][0].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 0;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
      s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Create channel %d error.\n", i);
        return s32Ret;
      }
    }
    }
    else
    {
        //bind vi to venc directly for main stream
        stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
    s32Ret = IPC_PF_Bind(&stVI, &stVenc);
    if(s32Ret < 0)
    {
      PRINT_ERR("Create channel %d error.\n", i);
      return s32Ret;
    }
    }

    //For vi sub channel
    stVI.enModId = MOD_ID_VI;
  stVI.u32DevId = i;
  stVI.u32ChnId = 1;

    VpssGrp = VpssGrp + 1;
    if(pstVpssCfg->stGrpCfg[VpssGrp].bGroup && pstVioCfg->vi[i].channel_1_enable)
    {
        stVpss.enModId = MOD_ID_VPSS;
    stVpss.u32DevId = VpssGrp;
    stVpss.u32ChnId = 0;
    s32Ret = IPC_PF_Bind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
      PRINT_ERR("Create channel %d error.\n", i);
      return s32Ret;
    }

    //sub stream
    if(pstVencCfg->vencStream[i][1].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 0;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
      s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Create channel %d error.\n", i);
        return s32Ret;
      }
    }
    //3rd stream
    if(pstVencCfg->vencStream[i][2].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[1].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 1;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 2;
      s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Create channel %d error.\n", i);
        return s32Ret;
      }
    }
    //ch 2 to VO
    if(pstVioCfg->vo.enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].bEnable)
    {
            /* for single sensor, we set layer and chn w/h same as the input,
             * so that the display will scale the buffer automatically, instead of
             * calling system scaler to do it.
             * This way can help reduce the bw and process time of a single buffer.
             */
            if(pstVioCfg->vo.square == 1)
            {
                AR_VO_SetLayerChnRes(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].u32Width, pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].u32Height);
            }
      //vo has square x square display max, each for a video.
      if(i < pstVioCfg->vo.square * pstVioCfg->vo.square)
      {
        stVpss.enModId = MOD_ID_VPSS;
        stVpss.u32DevId = VpssGrp;
        stVpss.u32ChnId = 2;
        stVo.enModId = MOD_ID_VO;
        stVo.u32DevId = 0;
        stVo.u32ChnId = i;
        s32Ret = IPC_PF_Bind(&stVpss, &stVo);
        if(s32Ret < 0)
        {
          PRINT_ERR("Create channel %d error.\n", i);
          return s32Ret;
        }
      }
    }

    //ch 3 to ALG
    if(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[3].bEnable)
    {
      PRINT_INFO("Vpss %d ch 3 enabled.\n", i);
    }

  }
  else
  {
    //no vpss, just bind vi sub channel to vo/venc
    stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
    s32Ret = IPC_PF_Bind(&stVI, &stVenc);
    if(s32Ret < 0)
    {
      PRINT_ERR("Create channel %d error.\n", i);
      return s32Ret;
    }

    if(pstVioCfg->vo.enable)
    {
            if(pstVioCfg->vo.square == 1)
            {
                AR_VO_SetLayerChnRes(pstVioCfg->vi[i].width, pstVioCfg->vi[i].height);
            }

      stVo.enModId = MOD_ID_VO;
      stVo.u32DevId = 0;
      stVo.u32ChnId = i;
      s32Ret = IPC_PF_Bind(&stVI, &stVo);
      if(s32Ret < 0)
      {
        PRINT_ERR("Create channel %d error.\n", i);
        return s32Ret;
      }
    }
  }

    return 0;

}
#endif

AR_S32 IPC_PF_Destroy_BasicChannel_DVP(AR_U32 u32Index)
{
    return 0;
}

AR_S32 IPC_PF_Destroy_BasicChannel_MIPI(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    //IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    IPC_CFG_VPSS_S * pstVpssCfg;
    AR_S32 s32Ret = 0;
    int i = 0;

    //pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();
    pstVpssCfg = IPC_CFG_VPSS_GetParam();

    stVI.enModId = MOD_ID_VI;
    stVI.u32DevId = i;
    stVI.u32ChnId = 0;

  //vi -- ch0 -> vpss
  if(pstVpssCfg->stGrpCfg[i].bGroup)
  {
      stVpss.enModId = MOD_ID_VPSS;
        stVpss.u32DevId = i;
        stVpss.u32ChnId = 0;
    s32Ret = IPC_PF_UnBind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }
    //main stream
    if(pstVencCfg->vencStream[i][0].enable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = i;
      stVpss.u32ChnId = 0;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    //sub stream
    if(pstVencCfg->vencStream[i][1].enable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = i;
      stVpss.u32ChnId = 1;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    //3rd stream
    if(pstVencCfg->vencStream[i][2].enable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = i;
      stVpss.u32ChnId = 2;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 2;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    //ch 3 to VO
    if(pstVioCfg->vo.enable && pstVpssCfg->stGrpCfg[i].stChnCfg[3].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = i;
      stVpss.u32ChnId = 3;
      stVo.enModId = MOD_ID_VO;
      stVo.u32DevId = 0;
      stVo.u32ChnId = i;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVo);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    //ch 4 to ALG
    if(pstVpssCfg->stGrpCfg[i].stChnCfg[4].bEnable)
    {
      PRINT_ERR("Vpss %d ch 4 unbind.\n", i);
    }
  }
  else
  {
    //no vpss, just unbind vi and venc
    stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
    s32Ret = IPC_PF_UnBind(&stVI, &stVenc);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }
        if(pstVioCfg->vo.enable)
    {
      stVo.enModId = MOD_ID_VO;
      stVo.u32DevId = 0;
      stVo.u32ChnId = i;
      s32Ret = IPC_PF_UnBind(&stVI, &stVo);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
  }

    return 0;
}

AR_S32 IPC_PF_Destroy_LowBandwidthChannel(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    //IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    IPC_CFG_VPSS_S * pstVpssCfg;
    AR_S32 s32Ret = 0;
    int i = u32Index;
    VPSS_GRP VpssGrp = 0;

    //pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();
    pstVpssCfg = IPC_CFG_VPSS_GetParam();

    //create a basic pipeline
  stVI.enModId = MOD_ID_VI;
  stVI.u32DevId = i;
  stVI.u32ChnId = 0;

  //vi -- ch0 -> vpss
  VpssGrp = i * 2;
  if(pstVpssCfg->stGrpCfg[VpssGrp].bGroup)
  {
    stVpss.enModId = MOD_ID_VPSS;
    stVpss.u32DevId = VpssGrp;
    stVpss.u32ChnId = 0;
    s32Ret = IPC_PF_UnBind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }

    //main stream
    if(pstVencCfg->vencStream[i][0].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 0;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    }
    else
    {
        //bind vi to venc directly for main stream
        stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;
    s32Ret = IPC_PF_UnBind(&stVI, &stVenc);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }
    }

    //For vi sub channel
    stVI.enModId = MOD_ID_VI;
  stVI.u32DevId = i;
  stVI.u32ChnId = 1;

    VpssGrp = VpssGrp + 1;
    if(pstVpssCfg->stGrpCfg[VpssGrp].bGroup && pstVioCfg->vi[i].channel_1_enable)
    {
        stVpss.enModId = MOD_ID_VPSS;
    stVpss.u32DevId = VpssGrp;
    stVpss.u32ChnId = 0;
    s32Ret = IPC_PF_UnBind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }

    //sub stream
    if(pstVencCfg->vencStream[i][1].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[0].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 0;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    //3rd stream
    if(pstVencCfg->vencStream[i][2].enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[1].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 1;
      stVenc.enModId = MOD_ID_VENC;
      stVenc.u32DevId = 0;
      stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 2;
      s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
    //ch 2 to VO
    if(pstVioCfg->vo.enable && pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[2].bEnable)
    {
      //vo has square x square display max, each for a video.
      if(i < pstVioCfg->vo.square * pstVioCfg->vo.square)
      {
        stVpss.enModId = MOD_ID_VPSS;
        stVpss.u32DevId = VpssGrp;
        stVpss.u32ChnId = 2;
        stVo.enModId = MOD_ID_VO;
        stVo.u32DevId = 0;
        stVo.u32ChnId = i;
        s32Ret = IPC_PF_UnBind(&stVpss, &stVo);
        if(s32Ret < 0)
        {
          PRINT_ERR("Create channel %d error.\n", i);
          return s32Ret;
        }
      }
    }

    //ch 3 to ALG
    if(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[3].bEnable)
    {
      PRINT_INFO("Vpss %d ch 3 enabled.\n", i);
    }
  }
  else
  {
    //no vpss, just bind vi sub channel to vo/venc
    stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN + 1;
    s32Ret = IPC_PF_UnBind(&stVI, &stVenc);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }

    if(pstVioCfg->vo.enable)
    {
      stVo.enModId = MOD_ID_VO;
      stVo.u32DevId = 0;
      stVo.u32ChnId = i;
      s32Ret = IPC_PF_UnBind(&stVI, &stVo);
      if(s32Ret < 0)
      {
        PRINT_ERR("Destroy channel %d error.\n", i);
        return s32Ret;
      }
    }
  }

    return 0;
}

/* Destory IR pipeline */
/* sns->thermal->isp0->isp1--bind--vpss--bind--venc*/
AR_S32 IPC_PF_Destory_IRChannel(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVpss, stVenc, stVo;
    //IPC_CFG_CHANNEL_S * pstChannelCfg;
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    //IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();
    AR_S32 s32Ret = 0;
    int i = u32Index;
    //VPSS_GRP VpssGrp = 0;

    VI_IR_GROUP_S * pstViIRGrp = IPC_PF_GetViIRGroup();

    PRINT_INFO("Destory ir channel...\n");
    //while(1){usleep(1000 * 1000);}
    stVI.enModId = MOD_ID_VI;
    if(pstViIRGrp->stViIsp1.enSnsType == IPC_SNS_TYPE_BUTT)
    {
        stVI.u32DevId = pstViIRGrp->stViIsp0.ViPipe;
        stVI.u32ChnId = pstViIRGrp->stViIsp0.ViChn;
    }
    else
    {
        stVI.u32DevId = pstViIRGrp->stViIsp1.ViPipe;
        stVI.u32ChnId = pstViIRGrp->stViIsp1.ViChn;
    }

    stVpss.enModId = MOD_ID_VPSS;
    stVpss.u32DevId = u32Index * 2; //we suppose other channels are RGB sensors, which pipeline is built with 2 vpss groups
    stVpss.u32ChnId = 0;

    stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = i * IPC_MAX_VENC_STREAM_PER_CHN;

    s32Ret = IPC_PF_UnBind(&stVI, &stVpss);
    if(s32Ret < 0)
    {
        PRINT_ERR("destory channel %d error.\n", i);
        return s32Ret;
    }

    if(pstVencCfg->vencStream[i][0].enable)
    {
        s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
        if(s32Ret < 0)
        {
            PRINT_ERR("destory channel %d error.\n", i);
            return s32Ret;
        }
    }

    if(pstVioCfg->vo.enable && i < pstVioCfg->vo.square * pstVioCfg->vo.square)
    {
        stVo.enModId = MOD_ID_VO;
        stVo.u32DevId = 0;
        stVo.u32ChnId = i;

        s32Ret = IPC_PF_UnBind(&stVpss, &stVo);
        if(s32Ret < 0)
        {
            PRINT_ERR("destory channel %d error.\n", i);
            return s32Ret;
        }
    }

    return 0;
}


AR_S32 IPC_PF_Create_Channel()
{
  //IPC_MPP_CHN_S stVo;
  ProfileStart(__FUNCTION__);

  IPC_CFG_CHANNEL_S * pstChannelCfg;
  IPC_CFG_VIO_S * pstVioCfg;
  //IPC_CFG_VENC_S * pstVencCfg;
  //IPC_CFG_VPSS_S * pstVpssCfg;
  AR_S32 s32Ret = 0;
  int i = 0;

  pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
  pstVioCfg = IPC_CFG_VIO_GetParam();
  //pstVencCfg = IPC_CFG_VENC_GetParam();
  //pstVpssCfg = IPC_CFG_VPSS_GetParam();

  //how many pipelines are there, are they basic or full pipeline?
  for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
  {
    if(!pstChannelCfg->stChannel[i].u32Enable)
    {
      continue;
    }

    if(strcmp(pstChannelCfg->stChannel[i].chType, "low_bw") == 0)
    {
            s32Ret = IPC_PF_Create_LowBandwidthChannel(i);
            if(s32Ret)
            {
                PRINT_ERR("Create MIPI low bw pipeline %d failed.\n", i);
                return s32Ret;
            }
    }
    else if(strcmp(pstChannelCfg->stChannel[i].chType, "infrared") == 0)
    {
        s32Ret = IPC_PF_Create_IRChannel(i);
        if(s32Ret)
        {
            PRINT_ERR("Create IR pipeline %d failed.\n", i);
            return s32Ret;
        }
    }
    else //default
    {
        if(strcmp(pstVioCfg->vi[i].sensor, "tp9930") == 0)
            {
                s32Ret = IPC_PF_Create_BasicChannel_DVP(i);
                if(s32Ret)
                {
                    PRINT_ERR("Create DVP basic pipeline %d failed.\n", i);
                    return s32Ret;
                }
            }
            else
            {
                s32Ret = IPC_PF_Create_BasicChannel_MIPI(i);
                if(s32Ret)
                {
                    PRINT_ERR("Create MIPI basic pipeline %d failed.\n", i);
                    return s32Ret;
                }
            }
    }
  }

  s_CtrStatus = CTL_START;

  //20221201 add for ir fussion
  IPC_PF_StartIrFussion();

  ProfileEnd(__FUNCTION__);

  return 0;
}

AR_S32 IPC_PF_Destroy_Channel()
{
  //IPC_MPP_CHN_S stVo;
  IPC_CFG_CHANNEL_S * pstChannelCfg;
  IPC_CFG_VIO_S * pstVioCfg;
  //IPC_CFG_VENC_S * pstVencCfg;
  //IPC_CFG_VPSS_S * pstVpssCfg;
  //AR_S32 s32Ret = 0;
  int i = 0;

  pstChannelCfg = IPC_CFG_CHANNEL_GetParam();
  pstVioCfg = IPC_CFG_VIO_GetParam();
  //pstVencCfg = IPC_CFG_VENC_GetParam();
  //pstVpssCfg = IPC_CFG_VPSS_GetParam();

    //20221201 add for ir fussion
    IPC_PF_StopIrFussion();

  //how many pipelines are there, are they basic or full pipeline?
  for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
  {
    if(!pstChannelCfg->stChannel[i].u32Enable)
    {
      continue;
    }

    if(strcmp(pstChannelCfg->stChannel[i].chType, "low_bw") == 0)
    {

            IPC_PF_Destroy_LowBandwidthChannel(i);
    }
   else if(strcmp(pstChannelCfg->stChannel[i].chType, "infrared") == 0)
    {
        IPC_PF_Destory_IRChannel(i);
    }
        else
        {
            if(strcmp(pstVioCfg->vi[i].sensor, "tp9930") == 0)
            {
                IPC_PF_Destroy_BasicChannel_DVP(i);
            }
            else
            {
                IPC_PF_Destroy_BasicChannel_MIPI(i);
            }
        }
  }

  s_CtrStatus = CTL_STOP;

  return 0;
}

#define IPC_ALIGN(x, align) (((x) / (align) + 1) * (align))

static AR_S32 IPC_PF_VB_PoolCnt(AR_U32 u32Width, AR_U32 u32height, IPC_VB_POOL_CNT_S * pstVbPoolCnt)
{
  if(u32Width <= 720 && u32height <= 576)
  {
    pstVbPoolCnt->u32PoolD1++;
  }
  else if(u32Width <= 1280 && u32height <= 720)
  {
    pstVbPoolCnt->u32Pool720P++;
  }
  else if(u32Width <= 1920 && u32height <= 1080)
  {
    pstVbPoolCnt->u32Pool1080P++;
  }
  else if(u32Width <= 2560 && u32height <= 1920)
  {
    pstVbPoolCnt->u32Pool5M++;
  }
  else
  {
    pstVbPoolCnt->u32Pool4K++;
  }

  return 0;
}


AR_S32 IPC_PF_VB_VpssPoolCnt(VB_CONFIG_S * pstVBCfg)
{
    AR_S32 i = 0, j = 0;
    AR_U32 u32PoolId = 0;
    VPSS_GRP VpssGrp = 0;
    AR_U32 u32WidthMain = 0, u32HeightMain = 0;
    AR_U32 u32WidthSub = 0, u32HeightSub = 0;
    AR_U32 u32StreamCntMain = 0, u32StreamCntSub = 0;
    AR_U32 u32GrpVbCntExtra = 0;
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    for(i = 0;i < IPC_MAX_PIPELINE_NUM;i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }

        if((strcmp(pChnCfg->stChannel[i].chType, "low_bw") == 0) || (strcmp(pChnCfg->stChannel[i].chType, "infrared") == 0))
        {
            VpssGrp = i * 2;

            //main channel
            for(j = IPC_MAX_STREAM_PER_PIPE - 1; j >=0 ; j--)
            {
                if(pVpssCfg->stGrpCfg[VpssGrp].bGroup && pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].bEnable)
                {
                    u32WidthMain = (pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Width > u32WidthMain ? pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Width : u32WidthMain);
                    u32HeightMain = (pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Height > u32HeightMain ? pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Height : u32HeightMain);
                    u32StreamCntMain++;
#ifdef PROXIMA
                    if(pVpssCfg->stGrpCfg[VpssGrp].bLdc
                        || pVpssCfg->stGrpCfg[VpssGrp].bFlip
                        || pVpssCfg->stGrpCfg[VpssGrp].bMirror
                        || pVpssCfg->stGrpCfg[VpssGrp].bFlip
                        || (pVpssCfg->stGrpCfg[VpssGrp].stRotate.bEnable && (pVpssCfg->stGrpCfg[VpssGrp].stRotate.enAngle != VPSS_ROTATE_0)))
                    {
                        u32GrpVbCntExtra++;
                    }
#endif
                    PRINT_INFO("u32StreamCntMain %d u32GrpVbCntExtra %d w %d h %d\n", u32StreamCntMain, u32GrpVbCntExtra, u32WidthMain, u32HeightMain);
        }
            }

            //sub channel
            VpssGrp += 1;
            for(j = IPC_MAX_STREAM_PER_PIPE - 1; j >=0 ; j--)
            {
                if(pVpssCfg->stGrpCfg[VpssGrp].bGroup && pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].bEnable)
                {
                    u32WidthSub = (pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Width > u32WidthSub ? pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Width : u32WidthSub);
                    u32HeightSub = (pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Height > u32HeightSub ? pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[j].u32Height : u32HeightSub);
                    u32StreamCntSub++;
                    PRINT_INFO("u32StreamCntSub %d w %d h %d\n", u32StreamCntSub, u32WidthSub, u32HeightSub);
        }
            }
        }
        else
        {
            for(j = IPC_MAX_STREAM_PER_PIPE - 1; j >=0 ; j--)
            {
                if(pVpssCfg->stGrpCfg[i].bGroup && pVpssCfg->stGrpCfg[i].stChnCfg[j].bEnable)
                {
                    pstVBCfg->u32MaxPoolCnt++;
                    u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
                    pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(pVpssCfg->stGrpCfg[i].stChnCfg[j].u32Width, 512) * IPC_ALIGN(pVpssCfg->stGrpCfg[i].stChnCfg[j].u32Height, 64) * 3 / 2 + 8192*3;
                    pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = 5;
                    PRINT_INFO("VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
                }
            }
        }
    }

    //main
    if(u32WidthMain != 0 && u32HeightMain != 0)
    {
        pstVBCfg->u32MaxPoolCnt++;
        u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
        pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(u32WidthMain, 512) * IPC_ALIGN(u32HeightMain, 64) * 3 / 2 + 8192*3;
        pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = 3 * (u32StreamCntMain + u32GrpVbCntExtra);
        PRINT_INFO("VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
    }
    //sub
    if(u32WidthSub != 0 && u32HeightSub != 0)
    {
        pstVBCfg->u32MaxPoolCnt++;
        u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
        pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(u32WidthSub, 512) * IPC_ALIGN(u32HeightSub, 64) * 3 / 2 + 8192*3;
        pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = 3 * u32StreamCntSub;
        PRINT_INFO("VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
    }

    return 0;
}

AR_S32 IPC_PF_VB_ViPoolCnt(VB_CONFIG_S * pstVBCfg, IPC_VB_POOL_CNT_S * pstVbPoolCnt, AR_U32 u32Blk, AR_S32 s32IsRawData)
{
    AR_U32 u32PoolId = 0;

    if(!s32IsRawData)
    {
        if(pstVbPoolCnt->u32PoolD1)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(720, 512) * 576 * 3 / 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32PoolD1;
            PRINT_INFO("pool D1 VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool720P)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(1280, 512) * 720 * 3 / 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool720P;
            PRINT_INFO("pool 720P VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool1080P)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(1920, 512) * 1080 * 3 / 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool1080P;
            PRINT_INFO("pool 1080P VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool5M)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(2560, 512) * 1920 * 3 / 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool5M;
            PRINT_INFO("pool 5MP VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool4K)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(3840, 512) * 2160 * 3 / 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool4K;
            PRINT_INFO("pool 4K VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
    }
    else
    {
        if(pstVbPoolCnt->u32PoolD1)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(720, 512) * 576 * 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32PoolD1;
            PRINT_INFO("pool D1 VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool720P)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(1280, 512) * 720 * 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool720P;
            PRINT_INFO("pool 720P VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool1080P)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(1920, 512) * 1080 * 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool1080P;
            PRINT_INFO("pool 1080P VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool5M)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(2560, 512) * 1920 * 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool5M;
            PRINT_INFO("pool 5MP VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
        if(pstVbPoolCnt->u32Pool4K)
        {
            pstVBCfg->u32MaxPoolCnt++;
            u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
            pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(3840, 512) * 2160 * 2 + 8192*3;
            pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32Blk * pstVbPoolCnt->u32Pool4K;
            PRINT_INFO("pool 4K VB %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);
        }
    }

    return 0;
}

static AR_S32 IPC_PF_VB_AudioPoolCnt(VB_CONFIG_S * pstVBCfg)
{
    AR_U32 u32PoolId = 0;

    pstVBCfg->u32MaxPoolCnt++;
    u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
    pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = 4096;
    pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = 100;
    PRINT_INFO("pool audio %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);

    return 0;
}

static AR_S32 IPC_PF_VB_IrPoolCnt(VB_CONFIG_S * pstVBCfg, IPC_CFG_VIO_S * pstVioCfg, IPC_CFG_CHANNEL_S * pstChnCfg)
{
    AR_U32 u32PoolId = 0;
    AR_S32 i = 0;
    AR_U32 u32Width = 0, u32Height = 0;
    AR_U32 u32IrSnsCnt = 0;

    //find max resolution
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pstChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }

        if(strcmp(pstChnCfg->stChannel[i].chType, "infrared") == 0)
        {
            if(pstVioCfg->vi[i].width > u32Width)
            {
                u32Width = pstVioCfg->vi[i].width;
            }
            if(pstVioCfg->vi[i].height > u32Height)
            {
                u32Height = pstVioCfg->vi[i].height;
            }

            u32IrSnsCnt++;
        }
    }

    //alloc raw vb for ir_sns and ir_thermal
    pstVBCfg->u32MaxPoolCnt++;
    u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
    pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(u32Width * 2, 512) * u32Height + 3*8192; //w*2*h
    pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32IrSnsCnt * 5 * 2; //sns + thermal
    PRINT_INFO("pool IR %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);

    //alloc isp vb for ir_isp0 and ir_isp1
    pstVBCfg->u32MaxPoolCnt++;
    u32PoolId = pstVBCfg->u32MaxPoolCnt - 1;
    pstVBCfg->astCommPool[u32PoolId].u64BlkSize  = IPC_ALIGN(u32Width, 512) * u32Height * 3 / 2 + 3*8192; //w*h*1.5
    pstVBCfg->astCommPool[u32PoolId].u32BlkCnt   = u32IrSnsCnt * 5 * 2; //isp0 + isp1

    PRINT_INFO("pool IR %d size %lld cnt %d\n", u32PoolId, pstVBCfg->astCommPool[u32PoolId].u64BlkSize, pstVBCfg->astCommPool[u32PoolId].u32BlkCnt);

    return 0;
}


AR_S32 IPC_PF_SYS_Init()
{
  AR_S32 i = 0;
  AR_S32 s32Ret = 0;
  //AR_U32 u32PoolId = 0;
  VB_CONFIG_S stVbConf = {0};
  ISP_PUB_ATTR_S stPubAttr = {0};
  WDR_MODE_E enWDRMode = WDR_MODE_NONE;
  IPC_SNS_TYPE_E enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT; //ACTUALLY 25 fps on FPGA
  IPC_VB_POOL_CNT_S stCh0VbPoolCnt = {0}, stCh1VbPoolCnt = {0}, stRawVbPoolCnt = {0};

  IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
  AR_U32 u32MaxBlk = pVioCfg->vi_cfg.vb_blk_cnt;
  //AR_U32 u32MaxBlk_IR = 0;
  AR_MPI_SYS_Exit();
  AR_MPI_VB_Exit();

  //93 ISP 420 format, align 512, 8bit, VI
  for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
  {
      if(!pChnCfg->stChannel[i].u32Enable)// || strcmp(pChnCfg->stChannel[i].chType,"low_bw") != 0)
      {
          continue;
      }

      enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, enWDRMode, pVioCfg->vi[i].fps);
      if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
      {
          continue;
      }

      if(pVioCfg->vi[i].channel_1_enable)
      {
          IPC_PF_VB_PoolCnt(pVioCfg->vi[i].channel_1_width, pVioCfg->vi[i].channel_1_height, &stCh1VbPoolCnt);
      }

      //vpss and vi share this, for osd copy
      if(pVioCfg->vi[i].channel_0_enable)
      {
          IPC_PF_VB_PoolCnt(pVioCfg->vi[i].width, pVioCfg->vi[i].height, &stCh0VbPoolCnt);
      }

      if(pVioCfg->vi[i].channel_2_raw_enable)
      {
          enWDRMode = pVioCfg->vi[i].HDR_enable ? WDR_MODE_2To1_LINE : WDR_MODE_NONE;
          //enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, enWDRMode, pVioCfg->vi[i].fps);
          IPC_PF_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);
          IPC_PF_VB_PoolCnt(stPubAttr.stSnsSize.u32Width, stPubAttr.stSnsSize.u32Height, &stRawVbPoolCnt);
      }
  }

  //VI alloc vb pool for ch0,ch1,raw data ch
  IPC_PF_VB_ViPoolCnt(&stVbConf, &stCh1VbPoolCnt, u32MaxBlk, 0);//ch1
#ifdef THERMAL_ENABLE
  IPC_PF_VB_ViPoolCnt(&stVbConf, &stCh0VbPoolCnt, 10, 0);//ch0
  IPC_PF_VB_ViPoolCnt(&stVbConf, &stRawVbPoolCnt, 15, 1);//raw data if enable
#else
    IPC_PF_VB_ViPoolCnt(&stVbConf, &stCh0VbPoolCnt, u32MaxBlk, 0);//ch0
    IPC_PF_VB_ViPoolCnt(&stVbConf, &stRawVbPoolCnt, u32MaxBlk, 1);//raw data if enable
#endif

  //VPSS alloc vb pool
  IPC_PF_VB_VpssPoolCnt(&stVbConf);

    //Audio alloc vb pool
    IPC_PF_VB_AudioPoolCnt(&stVbConf);

    //alloc vb for IR sensors(4 vin dev for each sensor)
    IPC_PF_VB_IrPoolCnt(&stVbConf, pVioCfg, pChnCfg);

  s32Ret = AR_MPI_VB_SetConfig(&stVbConf);
  if (AR_SUCCESS != s32Ret)
  {
    PRINT_ERR("AR_MPI_VB_SetConf failed, ret = %d!\n", s32Ret);
    return AR_FAILURE;
  }

  s32Ret = AR_MPI_VB_Init();
  if (AR_SUCCESS != s32Ret)
  {
    PRINT_ERR("AR_MPI_VB_Init failed, ret = %d!\n", s32Ret);
    return AR_FAILURE;
  }

    s32Ret = AR_MPI_SYS_Init();
  if (AR_SUCCESS != s32Ret)
  {
    PRINT_ERR("AR_MPI_SYS_Init failed, ret = %d!\n", s32Ret);
    AR_MPI_VB_Exit();
    return AR_FAILURE;
  }

  return AR_SUCCESS;
}

AR_S32 IPC_PF_SYS_UnInit()
{
  AR_MPI_SYS_Exit();
  AR_MPI_VB_Exit();

  return AR_SUCCESS;
}

AR_VOID IPC_MPI_VIN_OpenDev(AR_S32 mode, AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq, AR_S32 mipi_freq)
{
   VI_DEV_PROP_S Prop={0};
   if(mode==0){
      Prop.cam_mode=VIN_CAMERA_NORMAL;
   }else if(mode==1)
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }else if(mode==2)
   {
      Prop.cam_mode=VIN_CMAERA_MULTI_MODE;
   }else if(mode==3)
   {
       Prop.cam_mode=VIN_CMAERA_TOOL_SIMULATION;
   }else
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }
   //cfg the fre
   Prop.hdr_fre_mod=1;
   Prop.hdr_fre_hz=hdr_freq;
   Prop.vif_fre_mod=1;
   Prop.vif_fre_hz=vif_freq;
   Prop.isp_fre_mod=1;
   Prop.isp_fre_hz=isp_freq;
   Prop.mipi_fre_mod = 1;
   Prop.mipi_fre_hz = mipi_freq;
   AR_MPI_VIN_OpenDev(&Prop);
   return;
}

/********  VI platform APIs  *****************/
AR_S32 IPC_PF_VI_Init()
{
    ProfileStart(__FUNCTION__);
    AR_S32 i = 0;
    AR_S32 s32Ret = 0;
    printf("IPC_PF_VI_Init\n");
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
    printf("IPC_PF_VI_Init line %d sensor %s\n", __LINE__, pVioCfg->vi[0].sensor);
    VI_CHN_ATTR_S      stChnAttr = {0};
    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    IPC_SNS_TYPE_E     enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT;
    AR_U32             u32BusId = 0;
    ENUM_VFE_MODE      enVfe = 0;
    
    VI_DEV  ViDev = 0;
    VI_PIPE ViPipe = 0;
    int mipiDev = 0;
    AR_U32 u32MaxBlk = pVioCfg->vi_cfg.vb_blk_cnt;
    
    //single process default dev num is 5, for dvp need set to 8
    if(ar_hal_sys_mpp_service_is_lib())
    {
        STRU_VIN_DRIVER_START_OPT_T p_opt = {0};
        AR_MPI_VIN_unload_vin_driver();
        AR_MPI_VIN_get_driver_opt(&p_opt);
        p_opt.max_dev_num = 8;
        AR_MPI_VIN_set_driver_opt(&p_opt);
        AR_MPI_VIN_load_vin_driver();
    }
    IPC_MPI_VIN_OpenDev(pVioCfg->vi_cfg.cam_mode, pVioCfg->vi_cfg.hdr_freq, pVioCfg->vi_cfg.vif_freq, pVioCfg->vi_cfg.isp_freq, pVioCfg->vi_cfg.mipi_freq);
    printf("IPC_MPI_VIN_OpenDev\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }
        
        enWDRMode = pVioCfg->vi[i].HDR_enable ? WDR_MODE_2To1_LINE : WDR_MODE_NONE;
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, enWDRMode, pVioCfg->vi[i].fps);
        if(enSnsType == IPC_SNS_TYPE_BUTT)
        {
            return -1;
        }
        //for ir thermal cameras
        if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200 || enSnsType == H3812C1SG_IR_444X336)
        {
            s32Ret = IPC_PF_CreateViIRGroup(enSnsType, i);
            if(s32Ret)
            {
                PRINT_ERR("Create VI IR group failed.\n");
                return -1;
            }
        }
        //For mipi device
        else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
        {
            ViDev = i;
            ViPipe = ViDev;
            mipiDev = pVioCfg->vi[i].mipi_id;
#ifdef THERMAL_ENABLE
            if(enSnsType == TIMO256_DVP_256_IR || enSnsType == ELF1_DVP_256_IR || enSnsType == NK1221A_DVP_384_IR){
                setRawProcessParam(enSnsType);
                setRawProcessParamRaw(ViDev, 2);
            } else if(enSnsType == ISP_VIN_COMMON_INF){
                setRawProcessParamISP(ViDev, 0);
            } else {
                setYuvProcessParamVis(ViDev, 0, ViDev, 1);
            }
#endif

            s32Ret=AR_MPI_VI_SetMipiBindDev(ViDev, mipiDev); //this must refer to the hardware design
            if(s32Ret)
            {
                PRINT_ERR("Mipi bind dev %d/%d error!\n", ViDev, mipiDev);
                return -1;
            }
      //Create VI
      u32BusId = pVioCfg->vi[i].i2c_bus;
            enVfe = pVioCfg->vi[i].vfe_mode;

      s32Ret = AR_VI_CreateSingleVi(ViDev, ViPipe, mipiDev, enSnsType, enWDRMode, enVfe,
                pVioCfg->vi[i].power_gpio, pVioCfg->vi[i].reset_gpio, pVioCfg->vi[i].comm_gpio, pVioCfg->vi[i].raw_compress_enable);
            if (AR_SUCCESS != s32Ret)
            {
                goto EXIT;
            }

            if(pVioCfg->vi[i].channel_0_enable)
            {
                IPC_PF_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
                stChnAttr.u32Depth = 1;
                stChnAttr.u32BufCount = u32MaxBlk;
                for(int cn = 0; cn < 8; cn++)
                {
                    stChnAttr.u32DepthClient[cn] = 1;
                }
                /* cf50 enable */
//                stChnAttr.enCompressMode = COMPRESS_MODE_SEG;
                s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, 0, &stChnAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    PRINT_ERR("AR_MPI_VI_SetChnAttr channel 0 failed with %#x!\n", s32Ret);
                    return AR_FAILURE;
                }

                //enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;
                s32Ret = AR_MPI_VI_EnableChn(ViPipe, 0);

                if (s32Ret != AR_SUCCESS)
                {
                    PRINT_ERR("SAMPLE_COMM_VI_StartViChn failed !\n");
                    goto EXIT;
                }
            }

            //Must enable ch0, ch1 is optional
            if(pVioCfg->vi[i].channel_1_enable)
            {
                VI_CHN  ViChn1 = 1;
                /*start vi chn1*/
                s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr); //get attr from channel 0.
                if (AR_SUCCESS != s32Ret)
                {
                    PRINT_ERR("AR_MPI_VI_GetChnAttr channel 1 failed. s32Ret: 0x%x !\n", s32Ret);
                    goto EXIT;
                }
                stChnAttr.stSize.u32Width = pVioCfg->vi[i].channel_1_width;
                stChnAttr.stSize.u32Height = pVioCfg->vi[i].channel_1_height;
                stChnAttr.u32Depth = 1;
                stChnAttr.u32BufCount = u32MaxBlk;
                for(int cn = 0; cn < 8; cn++)
                {
                    stChnAttr.u32DepthClient[cn] = 1;
                }
                s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
                if (AR_SUCCESS != s32Ret)
                {
                    PRINT_ERR("AR_MPI_VI_SetChnAttr channel 1 failed. s32Ret: 0x%x !\n", s32Ret);
                    goto EXIT;
                }
                s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
                if (AR_SUCCESS != s32Ret)
                {
                    PRINT_ERR("AR_MPI_VI_EnableChn channel 1 failed. s32Ret: 0x%x !\n", s32Ret);
                    goto EXIT;
                }
            }
            //Must enable ch0, ch2 is optional
            if(pVioCfg->vi[i].channel_2_raw_enable)
            {
                VI_CHN ViChn2 = 2;
            #ifdef THERMAL_ENABLE
                IPC_PF_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
                stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_8BPP;
            #else
                ISP_PUB_ATTR_S stPubAttr = {0};
                IPC_PF_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);

                /*start vi chn2*/
                s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr); //get attr from channel 0.
                if (AR_SUCCESS != s32Ret)
                {
                    PRINT_ERR("AR_MPI_VI_GetChnAttr channel 2 failed. s32Ret: 0x%x !\n", s32Ret);
                    goto EXIT;
                }

                stChnAttr.stSize.u32Width = stPubAttr.stSnsSize.u32Width;
                stChnAttr.stSize.u32Height = stPubAttr.stSnsSize.u32Height;
                stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
                stChnAttr.u32Depth = 1;
                stChnAttr.u32BufCount = u32MaxBlk;
                for(int cn = 0; cn < 8; cn++)
                {
                    stChnAttr.u32DepthClient[cn] = 1;
                }
            #endif
                s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn2, &stChnAttr);
                if (AR_SUCCESS != s32Ret)
                {
                    PRINT_ERR("AR_MPI_VI_SetChnAttr channel 2 raw failed. s32Ret: 0x%x !\n", s32Ret);
                    goto EXIT;
                }

                s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn2);
                if (AR_SUCCESS != s32Ret)
                {
                    PRINT_ERR("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
                    goto EXIT;
                }
            }
            s32Ret = AR_VI_CreateSingleIsp(ViDev, ViPipe, enSnsType, u32BusId, enWDRMode, pVioCfg->vi[i].channel_0_fps);
            if (AR_SUCCESS != s32Ret)
            {
                goto EXIT;
            }
            //here to load image quality setting and configure to vi/isp
            //to do
        }
        else //for DVP, only has 2 device, 4 pipes, each pipe has 3 channels
        {
            //DVP in, 4 camera for each tp9930->dvp ctrlor
            ViDev = i / 4;
            ViPipe = i;
            VI_CHN ViChn = 0;

            MIPI_DEV DvpId = IPC_PF_PIPE_2_DVP_ID(ViPipe);
            s32Ret = AR_MPI_VI_SetMipiBindDev(ViDev, DvpId);
            if(s32Ret)
            {
                PRINT_ERR("Mipi bind dev 0/0 error!\n");
                return -1;
            }

            if(pVioCfg->vi[i].channel_0_enable)
            {
                //Create VI
                u32BusId = pVioCfg->vi[i].i2c_bus;
                enVfe = pVioCfg->vi[i].vfe_mode;
                if(ViPipe == 0 || ViPipe == 4) //vi and isp only need once for every 4 channels
                {
                    AR_VI_CreateSingleViDvp4Ch(ViDev, ViPipe, enSnsType, enWDRMode, enVfe,
                        pVioCfg->vi[i].power_gpio, pVioCfg->vi[i].reset_gpio);
                    AR_VI_CreateSingleIspDvp4Ch(ViDev, ViPipe, enSnsType, u32BusId, enWDRMode, pVioCfg->vi[i].channel_0_fps);
                }
            }

            if(pVioCfg->vi[i].channel_1_enable)
            {
                //sub channel
                ViChn = 1;
                IPC_PF_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
                stChnAttr.u32BufCount = 5;
                stChnAttr.stSize.u32Width = pVioCfg->vi[i].channel_1_width;
                stChnAttr.stSize.u32Height = pVioCfg->vi[i].channel_1_height;
                stChnAttr.stFrameRate.s32SrcFrameRate = pVioCfg->vi[i].channel_1_fps;
                stChnAttr.stFrameRate.s32DstFrameRate = pVioCfg->vi[i].channel_1_fps;
                /*start chn1*/
                s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
                if (AR_SUCCESS != s32Ret)
                {
                  PRINT_ERR("AR_MPI_VI_GetChnAttr sub channel failed. s32Ret: 0x%x !\n", s32Ret);
                  goto EXIT;
                }

                s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);
                if (AR_SUCCESS != s32Ret)
                {
                  PRINT_ERR("AR_MPI_VI_EnableChn sub channel failed. s32Ret: 0x%x !\n", s32Ret);
                  goto EXIT;
                }
            }
        }
        pthread_mutex_init(&g_ViMutex[i], 0);

  }
  ProfileEnd(__FUNCTION__);

  return 0;

EXIT:
  for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
  {
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, pVioCfg->vi[i].HDR_enable, pVioCfg->vi[i].fps);
        if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200 || enSnsType == H3812C1SG_IR_444X336)
        {
            IPC_PF_DestroyViIRGroup();
            continue;
        }
        else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
        {
            ViDev = i;
            ViPipe = pVioCfg->vi[i].mipi_id;
        }
        else
        {
            //DVP in, 4 camera for each tp9930->dvp ctrlor
            ViDev = i / 4;
            ViPipe = i / 4;
        }

    if(pVioCfg->vi[i].channel_0_enable)
    {
      AR_VI_StopSingleIsp(ViPipe, enSnsType);
      AR_VI_StopSingleVi(ViDev, ViPipe);
    }
    if(pVioCfg->vi[i].channel_1_enable)
    {
      AR_MPI_VI_DisableChn(ViPipe, 1);
    }
    if(pVioCfg->vi[i].channel_2_raw_enable)
    {
      AR_MPI_VI_DisableChn(ViPipe, 2);
    }
  }

  ProfileEnd(__FUNCTION__);

  return s32Ret;
}

AR_S32 IPC_PF_VI_UnInit()
{
    int i = 0;
    VI_DEV ViDev = 0;
    VI_PIPE ViPipe = 0;
    IPC_SNS_TYPE_E enSnsType;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }

        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, pVioCfg->vi[i].HDR_enable, pVioCfg->vi[i].fps);
        if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
        {
            IPC_PF_DestroyViIRGroup();
            continue;
        }
        else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
        {
            ViDev = i;
            ViPipe = ViDev;//pVioCfg->vi[i].mipi_id;
        }
        else
        {
            //DVP in, 4 camera for each tp9930->dvp ctrlor
            ViDev = i / 4;
            ViPipe = i;
        }
        if(pVioCfg->vi[i].channel_2_raw_enable)
        {
            AR_MPI_VI_DisableChn(ViPipe, 2);
        }
        if(pVioCfg->vi[i].channel_1_enable)
        {
            AR_MPI_VI_DisableChn(ViPipe, 1);
        }
        if(pVioCfg->vi[i].channel_0_enable)
        {
            AR_MPI_VI_DisableChn(ViPipe, 0);
        }

        AR_VI_StopSingleIsp(ViPipe, enSnsType);

        if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
        {
            AR_VI_StopSingleVi(ViDev, ViPipe);
        }
        else
        {
            AR_VI_StopSingleViDvp4Ch(ViDev, ViPipe);
        }
    }

    return 0;
}

AR_S32 IPC_PF_VI_Suspend()
{
    int i = 0;
    VI_PIPE ViPipe = 0;
    IPC_SNS_TYPE_E enSnsType;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

	IPC_PF_VI_UnInit();
	
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
             continue;
        }

		enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, pVioCfg->vi[i].HDR_enable, pVioCfg->vi[i].fps);
        if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
        {
            //IPC_PF_DestroyViIRGroup();
            continue;
        }
        else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
        {
            ViPipe = i;
        }
        else
        {
            //DVP in, 4 camera for each tp9930->dvp ctrlor
            ViPipe = i;
        }
        if(pVioCfg->vi[i].channel_2_raw_enable)
        {
            AR_MPI_VI_SuspendChn(ViPipe, 2, 0);
        }
        if(pVioCfg->vi[i].channel_1_enable)
        {
            AR_MPI_VI_SuspendChn(ViPipe, 1, 0);
        }
        if(pVioCfg->vi[i].channel_0_enable)
        {
            AR_MPI_VI_SuspendChn(ViPipe, 0, 0);
        }	
    }
	
	return 0;
}

AR_S32 IPC_PF_VI_Resume()
{
    int i = 0;
    VI_PIPE ViPipe = 0;
    IPC_SNS_TYPE_E enSnsType;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
    
    IPC_PF_VI_Init();
	
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }
        
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, pVioCfg->vi[i].HDR_enable, pVioCfg->vi[i].fps);
        if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
        {
            //IPC_PF_DestroyViIRGroup();
            continue;
        }
        else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
        {
            ViPipe = i;
        }
        else
        {
            //DVP in, 4 camera for each tp9930->dvp ctrlor
            ViPipe = i;
        }
        
        if(pVioCfg->vi[i].channel_0_enable)
        {
            AR_MPI_VI_ResumeChn(ViPipe, 0, 0);
        }	
        
        if(pVioCfg->vi[i].channel_1_enable)
        {
            AR_MPI_VI_ResumeChn(ViPipe, 1, 0);
        }
        
        if(pVioCfg->vi[i].channel_2_raw_enable)
        {
            AR_MPI_VI_ResumeChn(ViPipe, 2, 0);
        }
    }

	return 0;
}


//This is only for vi ch2 (sub yuv), the main yuv and sensor res can not be changed.
AR_S32 IPC_PF_VI_SetRes(VI_PIPE ViPipe, AR_U32 u32W, AR_U32 u32H)
{
    AR_S32 s32Ret = 0;
    VI_CHN_ATTR_S      stChnAttr = {0};
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

    /*start vi chn1*/
    s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 1, &stChnAttr); //get attr from channel 1.
    if (AR_SUCCESS != s32Ret)
    {
        PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
        return -1;
    }
    stChnAttr.stSize.u32Width = u32W;
    stChnAttr.stSize.u32Height = u32H;

    s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, 1, &stChnAttr);
    if (AR_SUCCESS != s32Ret)
    {
        PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
        return -1;
    }

    pthread_mutex_lock(&g_ViMutex[ViPipe]);
    pstVioCfg->vi[ViPipe].channel_1_width = u32W;
    pstVioCfg->vi[ViPipe].channel_1_height = u32H;
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);

    return IPC_CFG_VIO_Save();
}

AR_S32 IPC_PF_VI_SetFps(VI_PIPE ViPipe, AR_U32 u32Fps)
{
  //AR_S32 s32Ret = 0;
  IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

  PRINT_INFO("These will take active after reboot!\n");
  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  if(pstVioCfg->vi[ViPipe].channel_0_enable)
  {
    pstVioCfg->vi[ViPipe].channel_0_fps = u32Fps;
  }
  if(pstVioCfg->vi[ViPipe].channel_1_enable)
  {
    pstVioCfg->vi[ViPipe].channel_1_fps = u32Fps;
  }
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return IPC_CFG_VIO_Save();
}

AR_S32 IPC_PF_VI_SetFlip(VI_PIPE ViPipe, AR_BOOL bFlip)
{
  AR_S32 s32Ret = 0;
  ISP_FLIP_STATE_TIDY_ATTR_S stFlip;
  IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

  stFlip.u32FlipState = bFlip;
  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  s32Ret = AR_MPI_ISP_SetFlipStateTidyAttr(ViPipe, &stFlip);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set sensor flip error.\n");
    return s32Ret;
  }

  pstVioCfg->vi[ViPipe].flip = bFlip;
  s32Ret = IPC_CFG_VIO_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}
AR_S32 IPC_PF_VI_SetMirror(VI_PIPE ViPipe, AR_BOOL bMirror)
{
  AR_S32  s32Ret = 0;
  ISP_MIRROR_STATE_TIDY_ATTR_S stMirror;
  IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

  stMirror.u32MirrorState = bMirror;
  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  s32Ret = AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, &stMirror);
  if(s32Ret < 0)
  {

    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set sensor mirror error.\n");
    return s32Ret;
  }

  pstVioCfg->vi[ViPipe].mirror = bMirror;
  s32Ret = IPC_CFG_VIO_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}

AR_S32 IPC_PF_VI_SetAntiFlicker(VI_PIPE ViPipe, AR_U32 u32Freq)
{
  AR_S32 s32Ret = 0;
  ISP_BANDING_STATE_TIDY_ATTR_S stBanding;
  IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

  stBanding.u32BandingState = u32Freq;
  pthread_mutex_lock(&g_ViMutex[ViPipe]);

  s32Ret = AR_MPI_ISP_SetBandingStateTidyAttr(ViPipe, &stBanding);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set antiflicker error.\n");
    return s32Ret;
  }

  pstImageCfg->stImage[ViPipe].antiFlickerFreq = u32Freq;
  s32Ret = IPC_CFG_IMAGE_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}

AR_S32 IPC_PF_VI_SetDenoise3D(VI_PIPE ViPipe, AR_U32 u32Strength)
{
    AR_S32 s32Ret = 0;
    ISP_DE3D_STRENGTH_TIDY_ATTR_S stStrength = {0};
    IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    stStrength.u32De3dStrength = u32Strength;
    s32Ret = AR_MPI_ISP_SetDe3dStrengthTidyAttr(ViPipe, &stStrength);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Set denoise error.\n");
        return s32Ret;
    }

    pstImageCfg->stImage[ViPipe].strengthDenoise3d = u32Strength;

    s32Ret = IPC_CFG_IMAGE_Save();
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    return s32Ret;
}
AR_S32 IPC_PF_VI_GetDenoise3D(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;
    ISP_DE3D_STRENGTH_TIDY_ATTR_S stStrength = {0};

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    s32Ret = AR_MPI_ISP_GetDe3dStrengthTidyAttr(ViPipe, &stStrength);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Get denoise error.\n");
        return s32Ret;
    }
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    return stStrength.u32De3dStrength;
}

AR_S32 IPC_PF_VI_SetDenoise2D(VI_PIPE ViPipe, AR_U32 u32Strength)
{
    AR_S32 s32Ret = 0;
    ISP_DE2D_STRENGTH_TIDY_ATTR_S stStrength = {0};
    IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    stStrength.stDe2dStrength.strength = u32Strength;
    s32Ret = AR_MPI_ISP_SetDe2dStrengthTidyAttr(ViPipe, &stStrength);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Set denoise error.\n");
        return s32Ret;
    }

    pstImageCfg->stImage[ViPipe].strengthDenoise2d = u32Strength;

    s32Ret = IPC_CFG_IMAGE_Save();
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    return s32Ret;
}

AR_S32 IPC_PF_VI_GetDenoise2D(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;
    ISP_DE2D_STRENGTH_TIDY_ATTR_S stStrength = {0};

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    s32Ret = AR_MPI_ISP_GetDe2dStrengthTidyAttr(ViPipe, &stStrength);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Get denoise error.\n");
        return s32Ret;
    }
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    return stStrength.stDe2dStrength.strength;
}

AR_S32 IPC_PF_VI_GetExpLimit(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;
    ISP_EXP_LIMIT_TIDY_ATTR_S stExp = {0};

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    s32Ret = AR_MPI_ISP_GetExpLimitTidyAttr(ViPipe, &stExp);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Get exp limit error.\n");
        return s32Ret;
    }
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    return stExp.u32ExpLimit;
}

AR_S32 IPC_PF_VI_SetExpLimit(VI_PIPE ViPipe, AR_U32 u32ExpLimitUs)
{
    AR_S32 s32Ret = 0;
    ISP_EXP_LIMIT_TIDY_ATTR_S stExp = {0};

    stExp.u32ExpLimit = u32ExpLimitUs;

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    s32Ret = AR_MPI_ISP_SetExpLimitTidyAttr(ViPipe, &stExp);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Set exp limit error.\n");
        return s32Ret;
    }
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    return 0;
}

AR_S32 IPC_PF_VI_SetHDR(VI_PIPE ViPipe, AR_U32 u32OnOff)
{
  //save to json, will use it next time.
  AR_S32 s32Ret = 0;
  IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

  PRINT_INFO("This HDR will take effective after reboot.\n");

  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  pstVioCfg->vi[ViPipe].HDR_enable = u32OnOff;

  //if multi sensor, don't need to change
  if(pstVioCfg->vi_cfg.cam_mode != 2)
    {
        if(u32OnOff)
        {
            pstVioCfg->vi_cfg.cam_mode = 1;
        }
        else
        {
            pstVioCfg->vi_cfg.cam_mode = 0;
        }
    }

  s32Ret = IPC_CFG_VIO_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}
AR_S32 IPC_PF_VI_SetHue(VI_PIPE ViPipe, AR_U32 u32Hue)
{
  AR_S32 s32Ret = 0;
  ISP_HUE_TIDY_ATTR_S stHue;
  IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

  stHue.u32Hue = u32Hue;
  pthread_mutex_lock(&g_ViMutex[ViPipe]);

  s32Ret = AR_MPI_ISP_SetHueTidyAttr(ViPipe, &stHue);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set hue error.\n");
    return s32Ret;
  }

  pstImageCfg->stImage[ViPipe].hue = u32Hue;
  s32Ret = IPC_CFG_IMAGE_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}

AR_S32 IPC_PF_VI_SetBrightness(VI_PIPE ViPipe, AR_U32 u32Brightness)
{
  AR_S32 s32Ret = 0;
  ISP_BRIGHTNESS_TIDY_ATTR_S stBrightness;
  IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

  stBrightness.u32Brightness = u32Brightness;
  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  s32Ret = AR_MPI_ISP_SetBrightnessTidyAttr(ViPipe, &stBrightness);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set brightness error.\n");
    return s32Ret;
  }

  pstImageCfg->stImage[ViPipe].brightness = u32Brightness;
  s32Ret = IPC_CFG_IMAGE_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;

}

AR_S32 IPC_PF_VI_SetSatueration(VI_PIPE ViPipe, AR_U32 u32Satueration)
{
  AR_S32 s32Ret = 0;
  ISP_SATURATION_TIDY_ATTR_S stSatueration;
  IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

  stSatueration.u32Saturation = u32Satueration;

  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  s32Ret = AR_MPI_ISP_SetSaturationTidyAttr(ViPipe, &stSatueration);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set satueration error.\n");
    return s32Ret;
  }

  pstImageCfg->stImage[ViPipe].saturation = u32Satueration;
  s32Ret = IPC_CFG_IMAGE_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}

AR_S32 IPC_PF_VI_SetContrast(VI_PIPE ViPipe, AR_U32 u32Contrast)
{
  AR_S32 s32Ret = 0;
  ISP_CONTRAST_TIDY_ATTR_S stContrast;
  IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

  stContrast.u32Contrast = u32Contrast;

  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  s32Ret = AR_MPI_ISP_SetContrastTidyAttr(ViPipe, &stContrast);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set contrast error.\n");
    return s32Ret;
  }

  pstImageCfg->stImage[ViPipe].contrast = u32Contrast;
  s32Ret = IPC_CFG_IMAGE_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}

AR_S32 IPC_PF_VI_SetSharpness(VI_PIPE ViPipe, AR_U32 u32Sharpness)
{
  AR_S32 s32Ret = 0;
  ISP_SHARPNESS_TIDY_ATTR_S stSharpness;
  IPC_CFG_IMAGE_S * pstImageCfg = IPC_CFG_IMAGE_GetParam();

  stSharpness.u32Sharpness = u32Sharpness;

  pthread_mutex_lock(&g_ViMutex[ViPipe]);
  s32Ret = AR_MPI_ISP_SetSharpnessTidyAttr(ViPipe, &stSharpness);
  if(s32Ret < 0)
  {
    pthread_mutex_unlock(&g_ViMutex[ViPipe]);
    PRINT_ERR("Set contrast error.\n");
    return s32Ret;
  }

  pstImageCfg->stImage[ViPipe].sharpness = u32Sharpness;
  s32Ret = IPC_CFG_IMAGE_Save();
  pthread_mutex_unlock(&g_ViMutex[ViPipe]);

  return s32Ret;
}

AR_S32 IPC_PF_VI_GetHue(VI_PIPE ViPipe)
{
  AR_S32 s32Ret = 0;
  ISP_HUE_TIDY_ATTR_S stHue = {0};

  s32Ret = AR_MPI_ISP_GetHueTidyAttr(ViPipe, &stHue);
  if(s32Ret < 0)
  {
    PRINT_ERR("Get hue error.\n");
    return s32Ret;
  }

  return stHue.u32Hue;
}

AR_S32 IPC_PF_VI_GetBrightness(VI_PIPE ViPipe)
{
  AR_S32 s32Ret = 0;
  ISP_BRIGHTNESS_TIDY_ATTR_S stBrightness;

  s32Ret = AR_MPI_ISP_GetBrightnessTidyAttr(ViPipe, &stBrightness);
  if(s32Ret < 0)
  {
    PRINT_ERR("Get brightness error.\n");
    return s32Ret;
  }

  return stBrightness.u32Brightness;
}

AR_S32 IPC_PF_VI_GetSatueration(VI_PIPE ViPipe)
{
  AR_S32 s32Ret = 0;
  ISP_SATURATION_TIDY_ATTR_S stSatueration;

  s32Ret = AR_MPI_ISP_GetSaturationTidyAttr(ViPipe, &stSatueration);
  if(s32Ret < 0)
  {
    PRINT_ERR("Get satueration error.\n");
    return s32Ret;
  }

  return stSatueration.u32Saturation;
}

AR_S32 IPC_PF_VI_GetContrast(VI_PIPE ViPipe)
{
  AR_S32 s32Ret = 0;
  ISP_CONTRAST_TIDY_ATTR_S stContrast;

  s32Ret = AR_MPI_ISP_GetContrastTidyAttr(ViPipe, &stContrast);
  if(s32Ret < 0)
  {
    PRINT_ERR("Get contrast error.\n");
    return s32Ret;
  }

  return stContrast.u32Contrast;
}

AR_S32 IPC_PF_VI_GetSharpness(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;
    ISP_SHARPNESS_TIDY_ATTR_S stSharpness;

    s32Ret = AR_MPI_ISP_GetSharpnessTidyAttr(ViPipe, &stSharpness);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get contrast error.\n");
        return s32Ret;
    }

    return stSharpness.u32Sharpness;
}

AR_S32 IPC_PF_VI_SetImgScene(VI_PIPE ViPipe, AR_U32 u32Mode, AR_CHAR * strTunningName)
{
    AR_S32 s32Ret = 0;
    ISP_SENSCE_TIDY_ATTR_S stSense = {0};
    IPC_CFG_GPIO_S * pstGpio = IPC_CFG_GPIO_GetParam();

    PRINT_INFO("Vi %d change tunning file to %s\n", ViPipe, strTunningName);
    strcpy(stSense.stSensce.tuning_name, strTunningName);
    s32Ret = AR_MPI_ISP_SetSensceTidyAttr(ViPipe, &stSense);
    if(s32Ret < 0)
    {
        PRINT_ERR("Change tunning file error.\n");
        return s32Ret;
    }

    //Turn off/on ircut and led board
    if(pstGpio->ircut.enable)
    {
        if(u32Mode == 0)//day
        {
            IPC_PF_GPIO_SetByName(pstGpio->ircut.fbc_gpio_name, OUTPUT, 0);
            IPC_PF_GPIO_SetByName(pstGpio->ircut.enable_gpio_name, OUTPUT, 0);
            usleep(50*1000);
            IPC_PF_GPIO_SetByName(pstGpio->ircut.enable_gpio_name, OUTPUT, 1);
            IPC_PF_GPIO_SetByName(pstGpio->ircut.fbc_gpio_name, OUTPUT, 1);
        }
        else //night
        {
            IPC_PF_GPIO_SetByName(pstGpio->ircut.fbc_gpio_name, OUTPUT, 1);
            IPC_PF_GPIO_SetByName(pstGpio->ircut.enable_gpio_name, OUTPUT, 0);
            usleep(50*1000);
            IPC_PF_GPIO_SetByName(pstGpio->ircut.enable_gpio_name, OUTPUT, 1);
            IPC_PF_GPIO_SetByName(pstGpio->ircut.fbc_gpio_name, OUTPUT, 1);
        }
    }

    if(pstGpio->led.enable)
    {
        if(u32Mode == 0) //day
        {
            IPC_PF_GPIO_SetByName(pstGpio->led.gpio_name, OUTPUT, 0);
        }
        else //night
        {
            IPC_PF_GPIO_SetByName(pstGpio->led.gpio_name, OUTPUT, 1);
        }
    }

    return 0;
}

AR_S32 IPC_PF_VI_Get3aInfoTidyAttr(VI_PIPE ViPipe, IPC_VI_VIDEO_3A_PARAM_S * pst3aInfo)
{
    AR_S32 s32Ret = 0;
    ISP_3A_INFO_EXT_TIDY_ATTR_S pst3aInfoTidyAttr = {0};

    s32Ret = AR_MPI_ISP_Get3aInfoExtTidyAttr(ViPipe, &pst3aInfoTidyAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get 3A Info error.\n");
        return s32Ret;
    }

    pst3aInfo->s32AecMode = pst3aInfoTidyAttr.st3aInfoExt.aec_info.aec_mode;
    pst3aInfo->f32AecGain= pst3aInfoTidyAttr.st3aInfoExt.aec_info.gain;
    pst3aInfo->f32AecExpTimeUs = pst3aInfoTidyAttr.st3aInfoExt.aec_info.exp_time_us;
    pst3aInfo->u32AecLumaTarget = pst3aInfoTidyAttr.st3aInfoExt.aec_info.luma_target;
    pst3aInfo->f32AecExpTimeUsShort = pst3aInfoTidyAttr.st3aInfoExt.aec_info.exp_time_us_short;
    pst3aInfo->u32ExpIndex = pst3aInfoTidyAttr.st3aInfoExt.aec_info.exp_index;
    pst3aInfo->u32LuxIndex = pst3aInfoTidyAttr.st3aInfoExt.aec_info.lux_index;
    pst3aInfo->s32AwbMode = pst3aInfoTidyAttr.st3aInfoExt.awb_info.awb_mode;
    pst3aInfo->u32AwbCct = pst3aInfoTidyAttr.st3aInfoExt.awb_info.cct;
    pst3aInfo->f32AwbRGain = pst3aInfoTidyAttr.st3aInfoExt.awb_info.r_gain;
    pst3aInfo->f32AwbGGain = pst3aInfoTidyAttr.st3aInfoExt.awb_info.g_gain;
    pst3aInfo->f32AwbBGain = pst3aInfoTidyAttr.st3aInfoExt.awb_info.b_gain;
    pst3aInfo->u32AfLenPosition = pst3aInfoTidyAttr.st3aInfoExt.af_info.len_position;
    pst3aInfo->u32AfFocusValue = pst3aInfoTidyAttr.st3aInfoExt.af_info.focus_value;

    return 0;
}


/********  VO platform APIs  *****************/
#define IPC_MAX_VO_WINDOW_NUM  9
#define COLOR_RGB_BLUE     0x0000FF

int IPC_PF_VO_7INCH_LCD_Init(VO_DEV VoDev)
{
    int backlight    = 40;
    int power_gpio   = 42;
    int standby_gpio = 38;
    int reset_gpio   = 41;
    int updown_gpio  = 63;

    PRINT_INFO("7inch lcd init\n");

    IPC_PF_GPIO_SetValue(power_gpio,1);
    IPC_PF_GPIO_SetValue(backlight,1);

    //standby the lcd
    IPC_PF_GPIO_SetValue(updown_gpio,0);
    IPC_PF_GPIO_SetValue(standby_gpio,1);

    usleep(10000);

    IPC_PF_GPIO_SetValue(reset_gpio,1);
    usleep(1000);
    IPC_PF_GPIO_SetValue(reset_gpio,0);
    usleep(10000);
    IPC_PF_GPIO_SetValue(reset_gpio,1);

    //final we backlight the lcd
    IPC_PF_GPIO_SetValue(backlight,1);

    static VO_DSI_ATTR_S pstDsiCfg =
    {
        .u32Lane           = {1,1,1,1},
        .u32BitsPerPixel   = 24,
        .stSyncInfo        = {
                                 .u32Hsa  = 72,
                                 .u32Hbp  = 162,
                                 .u32Hact = 1024,
                                 .u32Hfp  = 162,

                                 .u32Vsa  = 10,
                                 .u32Vbp  = 23,
                                 .u32Vact = 600,
                                 .u32Vfp  = 12
                             },
         .fDphyClkMhz      = 360.3744,
         .u32PllFreqReg2c0 = 0x2c65f509,
         .u32PllFreqReg38c = 0x2800
    };

    int s32Ret = AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("set vo dev_%d dsi attr failed with %#x!\n", VoDev, s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VO_Dsi_Enable(VoDev);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("enable vo dev_%d dsi failed with %#x!\n", VoDev, s32Ret);
        return AR_FAILURE;
    }
  return AR_SUCCESS;
}

AR_S32 IPC_PF_VO_Bind(int bindMod, int devId, int chId, int voChn)
{
    IPC_MPP_CHN_S stMod, stVo;
    IPC_CFG_VIO_S * pstVioCfg;
    AR_S32 s32Ret = 0;
    pstVioCfg = IPC_CFG_VIO_GetParam();

    if(pstVioCfg->vo.enable)
    {
        stMod.enModId = bindMod;
        stMod.u32DevId = devId;
        stMod.u32ChnId = chId;
        stVo.enModId = MOD_ID_VO;
        stVo.u32DevId = 0;
        stVo.u32ChnId = voChn;
        s32Ret = IPC_PF_Bind(&stMod, &stVo);
        if(s32Ret < 0)
        {
            printf("IPC_PF_VO_SHOW error.\n");
            return s32Ret;
        }
    }

    return 0;
}

AR_S32 IPC_PF_VO_UnBind(int bindMod, int devId, int chId, int voChn)
{
    IPC_MPP_CHN_S stMod, stVo;
    IPC_CFG_VIO_S * pstVioCfg;
    AR_S32 s32Ret = 0;

    pstVioCfg = IPC_CFG_VIO_GetParam();

    if(pstVioCfg->vo.enable)
    {
        stMod.enModId = bindMod;
        stMod.u32DevId = devId;
        stMod.u32ChnId = chId;
        stVo.enModId = MOD_ID_VO;
        stVo.u32DevId = 0;
        stVo.u32ChnId = voChn;
        s32Ret = IPC_PF_UnBind(&stMod, &stVo);
        if(s32Ret < 0)
        {
            printf("IPC_PF_VO_SHOW error.\n");
            return s32Ret;
        }
    }

    return 0;
}


AR_S32 IPC_PF_VO_Init()
{
    ProfileStart(__FUNCTION__);

    AR_S32 s32Ret = AR_SUCCESS;
    VO_DEV VoDev;
    VO_PUB_ATTR_S  stPubAttr = {0};
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

    if(!pstVioCfg->vo.enable)
    {
        return 0;
    }

    //0. set and enable dev
    VoDev = 0;
    if(strcmp(pstVioCfg->vo.type, "hdmi") == 0)
    {
        //for small pics, use 720x480 disp.
        if(pstVioCfg->vo.width <= 720 && pstVioCfg->vo.height <= 480)
        {
            stPubAttr.enIntfSync = VO_OUTPUT_480P60;
        }
        else
        {
            if(pstVioCfg->vo.fps == 30)
            {
                stPubAttr.enIntfSync = VO_OUTPUT_1080P30;
            }
            else //default
            {
                stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
            }
        }

        stPubAttr.enIntfType = VO_INTF_HDMI;
    }
    else if(strcmp(pstVioCfg->vo.type, "hdmi_interlace") == 0)
    {
        stPubAttr.enIntfSync = VO_OUTPUT_1080I60;
        stPubAttr.enIntfType = VO_INTF_HDMI;
    }
    else if(strcmp(pstVioCfg->vo.type, "mipi") == 0 || strcmp(pstVioCfg->vo.type, "dvp") == 0)
    {
        s32Ret = AR_MPI_VO_SetDevFrameRate(VoDev, pstVioCfg->vo.fps);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("set vo framerate failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        stPubAttr.enIntfSync = VO_OUTPUT_USER;
        stPubAttr.stSyncInfo.bSynm = AR_TRUE;
        stPubAttr.stSyncInfo.bIop  = AR_TRUE;
        stPubAttr.stSyncInfo.u8Intfb = 0;
        IPC_VO_Get_Display_Param(&stPubAttr, pstVioCfg->vo.lcd_id);

        stPubAttr.stSyncInfo.u16Hmid  = 0;
        stPubAttr.stSyncInfo.u16Bvact = 0;
        stPubAttr.stSyncInfo.u16Bvbb  = 0;
        stPubAttr.stSyncInfo.u16Bvfb  = 0;
        stPubAttr.stSyncInfo.bIdv = AR_FALSE;
        stPubAttr.stSyncInfo.bIhs = AR_FALSE;
        stPubAttr.stSyncInfo.bIvs = AR_FALSE;
        if(strcmp(pstVioCfg->vo.type, "mipi") == 0)
            stPubAttr.enIntfType = VO_INTF_MIPI;
        else if(strcmp(pstVioCfg->vo.type, "dvp") == 0)
            stPubAttr.enIntfType = g_dvp_out_mode;//VO_INTF_BT1120;//VO_INTF_LCD_16BIT;
    }
    else if(strcmp(pstVioCfg->vo.type, "dvp2803") == 0)
    {
        s32Ret = AR_MPI_VO_SetDevFrameRate(VoDev, 30);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("set vo framerate failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        stPubAttr.enIntfSync = VO_OUTPUT_USER;
        stPubAttr.stSyncInfo.bSynm = AR_FALSE;
        stPubAttr.stSyncInfo.bIop  = AR_TRUE;
        stPubAttr.stSyncInfo.u8Intfb = 0;

        stPubAttr.stSyncInfo.u16Hpw  = 280;
        stPubAttr.stSyncInfo.u16Hbb  = 0;
        stPubAttr.stSyncInfo.u16Hact = 1920;
        stPubAttr.stSyncInfo.u16Hfb  = 0;
        stPubAttr.stSyncInfo.u16Vpw  = 45;
        stPubAttr.stSyncInfo.u16Vbb  = 0;
        stPubAttr.stSyncInfo.u16Vact = 1080;
        stPubAttr.stSyncInfo.u16Vfb  = 0;

        stPubAttr.stSyncInfo.u16Hmid  = 0;
        stPubAttr.stSyncInfo.u16Bvact = 0;
        stPubAttr.stSyncInfo.u16Bvbb  = 0;
        stPubAttr.stSyncInfo.u16Bvfb  = 0;
        stPubAttr.stSyncInfo.bIdv = AR_FALSE;
        stPubAttr.stSyncInfo.bIhs = AR_FALSE;
        stPubAttr.stSyncInfo.bIvs = AR_FALSE;
        stPubAttr.enIntfType = VO_INTF_BT1120;

    	tp28xx_dvp_init();
    }

    stPubAttr.u32BgColor = COLOR_RGB_BLUE;

    s32Ret = AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("set vo pub failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VO_Enable(VoDev);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("enable vo failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    if(!strcmp(pstVioCfg->vo.type, "mipi"))//mipi lcd
    {

        printf("init lcd :%d, in else mode \n", pstVioCfg->vo.lcd_id);
        s32Ret = IPC_VO_Lcd_Init(VoDev, pstVioCfg->vo.lcd_id);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("init lcd failed with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        IPC_VO_Get_Display_Param(&stPubAttr, pstVioCfg->vo.lcd_id);
    }

    //1. set layer
    VO_LAYER VoLayer = 0;
    VO_VIDEO_LAYER_ATTR_S  stLayerAttr    = {0};
    VO_CSC_S               stLayerCsc     = {0};

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Width = pstVioCfg->vo.width;
    stLayerAttr.stDispRect.u32Height = pstVioCfg->vo.height;
    stLayerAttr.bClusterMode     = AR_FALSE;
    stLayerAttr.bDoubleFrame    = AR_FALSE;
    stLayerAttr.enPixFormat       = PIXEL_FORMAT_YVU_PLANAR_420;
    stLayerAttr.stImageSize.u32Width = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    if(!strcmp(pstVioCfg->vo.type, "dvp2803"))
    {
      stLayerAttr.u32DispFrmRt = 30;
    }
    else
    {
        stLayerAttr.u32DispFrmRt = pstVioCfg->vo.fps;
        stLayerCsc.enCscMatrix = VO_CSC_MATRIX_BT601_TO_RGB_PC;
        s32Ret = AR_MPI_VO_SetVideoLayerCSC(VoLayer, &stLayerCsc);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("Set video layer csc failed with %#x!\n", s32Ret);
            AR_MPI_VO_Disable(VoDev);
            return s32Ret;
        }
    }

    s32Ret = AR_MPI_VO_SetDisplayBufLen(VoLayer, 3);
    if (AR_SUCCESS != s32Ret)
    {
        PRINT_ERR("AR_MPI_VO_SetDisplayBufLen failed with %#x!\n",s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_SetVideoLayerPartitionMode(VoLayer, VO_PART_MODE_MULTI);
    if (AR_SUCCESS != s32Ret)
    {
        PRINT_ERR("AR_MPI_VO_SetVideoLayerPartitionMode failed!\n");
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("Set video layer attr failed with %#x!\n", s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("vo enable layer failed with %#x!\n", s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    //2. Set Chn, we set 9 mux by defaut
    VO_CHN_ATTR_S stChnAttr = {0};
    AR_S32 i = 0;
    AR_U32 u32Width = stLayerAttr.stImageSize.u32Width;
    AR_U32 u32Height = stLayerAttr.stImageSize.u32Height;
    //How many square do you want to display ? each square for a video.
    AR_U32 u32Square = pstVioCfg->vo.square;

    stChnAttr.u32Priority       = 0;
    stChnAttr.bDeflicker        = AR_FALSE;

    for(i = 0; i < u32Square * u32Square; i++)
    {
        stChnAttr.stRect.s32X       = ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
        stChnAttr.stRect.s32Y       = ALIGN_DOWN((u32Height / u32Square) * (i / u32Square), 2);
        stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width / u32Square, 2);
        stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Square, 2);

        s32Ret = AR_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("VO set attr failed with %#x!\n", s32Ret);
            AR_MPI_VO_DisableVideoLayer(VoLayer);
            AR_MPI_VO_Disable(VoDev);
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VO_EnableChn(VoLayer, i);
        if (s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("VO enable chn failed with %#x!\n", s32Ret);
            AR_MPI_VO_DisableVideoLayer(VoLayer);
            AR_MPI_VO_Disable(VoDev);
            return AR_FAILURE;
        }
    }

    //3. start hdmi device
    /******************************
    * Note : do this after vo device started.
    ********************************/
    if(VO_INTF_HDMI == stPubAttr.enIntfType)
    {
        AR_VO_StartHdmi();
    }

    ProfileEnd(__FUNCTION__);

    return 0;
}

AR_S32 IPC_PF_VO_FullScreen_Process(AR_U32 u32VoChnId, AR_U32 u32Enable)
{
    ProfileStart(__FUNCTION__);

  AR_S32 i = 0;
  AR_S32 s32Ret = AR_SUCCESS;
  IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();
  IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
  if(!pstVioCfg->vo.enable)
  {
    return 0;
  }

  AR_U32 u32Width = pstVioCfg->vo.width;
  AR_U32 u32Height = pstVioCfg->vo.height;
  AR_U32 u32Square = pstVioCfg->vo.square;
  IPC_MPP_CHN_S stVpss,  stVo;
  VPSS_GRP VpssGrp = 0;
  VO_LAYER VoLayer = 0;
  VO_CHN_ATTR_S stChnAttr = {0};
    stChnAttr.u32Priority  = 0;
    stChnAttr.bDeflicker   = AR_FALSE;

  //full screen on
    if(u32Enable)
  {
      stChnAttr.u32Priority       = 1;
    stChnAttr.stRect.s32X   = 0;
    stChnAttr.stRect.s32Y   = 0;
    stChnAttr.stRect.u32Width = u32Width;
    stChnAttr.stRect.u32Height  = u32Height;

    s32Ret = AR_MPI_VO_SetChnAttr(VoLayer, u32VoChnId, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
      PRINT_ERR("VO set attr failed with %#x!\n", s32Ret);
      return AR_FAILURE;
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
      {
          if((i == u32VoChnId) ||
         (!pChCfg->stChannel[i].u32Enable))
        {
                continue;
      }

          VpssGrp = i * 2 + 1;
          stVpss.enModId = MOD_ID_VPSS;
          stVpss.u32DevId = VpssGrp;
          stVpss.u32ChnId = 2;
          stVo.enModId = MOD_ID_VO;
          stVo.u32DevId = 0;
          stVo.u32ChnId = i;
          s32Ret = IPC_PF_UnBind(&stVpss, &stVo);
          if(s32Ret < 0)
          {
              PRINT_ERR("unbind channel %d error.\n", i);
              return s32Ret;
          }
    }

    for(i = 0; i < u32Square*u32Square; i++)
    {
        if(i == u32VoChnId)
        {
                continue;
      }

      //if(!pChCfg->stChannel[i].u32Enable)
        //{
                //continue;
      //}

      s32Ret = AR_MPI_VO_DisableChn(VoLayer, i);
      if (s32Ret != AR_SUCCESS)
      {
        PRINT_ERR("VO disable chn[%d] failed with %#x!\n", i,s32Ret);
        return AR_FAILURE;
      }
      }

  }
  //full screen off
  else
  {
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if((i == u32VoChnId) ||
         (!pChCfg->stChannel[i].u32Enable))
        {
                continue;
      }

      VpssGrp = i * 2 + 1;
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = 2;
      stVo.enModId = MOD_ID_VO;
      stVo.u32DevId = 0;
      stVo.u32ChnId = i;
      s32Ret = IPC_PF_Bind(&stVpss, &stVo);
      if(s32Ret < 0)
      {
        PRINT_ERR("Create channel %d error.\n", i);
        return s32Ret;
      }
    }


    for(i = 0; i < u32Square*u32Square; i++)
    {
        if(i == u32VoChnId)
        {
                continue;
      }

      //if(!pChCfg->stChannel[i].u32Enable)
        //{
                //continue;
      //}

      s32Ret = AR_MPI_VO_EnableChn(VoLayer, i);
      if (s32Ret != AR_SUCCESS)
      {
        PRINT_ERR("VO enable chn failed with %#x!\n", s32Ret);
        return AR_FAILURE;
      }
    }

    stChnAttr.u32Priority       = 0;
    stChnAttr.stRect.s32X   = ALIGN_DOWN((u32Width / u32Square) * (u32VoChnId % u32Square), 2);
    stChnAttr.stRect.s32Y   = ALIGN_DOWN((u32Height / u32Square) * (u32VoChnId / u32Square), 2);
    stChnAttr.stRect.u32Width = ALIGN_DOWN(u32Width / u32Square, 2);
    stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Square, 2);

    s32Ret = AR_MPI_VO_SetChnAttr(VoLayer, u32VoChnId, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
      PRINT_ERR("VO set attr failed with %#x!\n", s32Ret);
      return AR_FAILURE;
    }

  }

    ProfileEnd(__FUNCTION__);
    return 0;
}


AR_S32 IPC_PF_VO_UnInit()
{
    AR_S32 i = 0;
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

    if(!pstVioCfg->vo.enable)
    {
        return 0;
    }

    if(strcmp(pstVioCfg->vo.type, "hdmi") == 0)
    {
        AR_VO_StopHdmi();
    }

    for(i = 0; i < pstVioCfg->vo.square * pstVioCfg->vo.square; i++)
    {
        AR_MPI_VO_DisableChn(0, i);
    }

    AR_MPI_VO_DisableVideoLayer(0);
    AR_MPI_VO_Disable(0);

    return 0;
}

AR_S32 IPC_PF_VO_Reset(){
    PRINT_INFO("vo reset time trace start: %d!\n", timer_get_ms());
    IPC_PF_VO_UnBind(MOD_ID_VPSS, 0, 3, 0);
    IPC_PF_VO_UnInit();
    IPC_PF_VO_Init();
    IPC_PF_VO_Bind(MOD_ID_VPSS, 0, 3, 0);
    PRINT_INFO("vo reset time trace end: %d!\n", timer_get_ms());
    return 0;
}

AR_S32 IPC_PF_VO_Suspend()
{
	AR_S32 s32Ret = 0;
	
    s32Ret = IPC_PF_VO_UnInit();
	if(s32Ret < 0)
	{
		printf("vo uninit failed while vo suspend.\n");
		return -1;
	}

	s32Ret = AR_MPI_VO_Suspend(0);
	if(s32Ret < 0)
	{
		printf("vo suspend failed.\n");
		return -1;
	}

	return 0;
}

AR_S32 IPC_PF_VO_Resume()
{
	AR_S32 s32Ret = 0;
	
    s32Ret = IPC_PF_VO_Init();
	if(s32Ret < 0)
	if(s32Ret < 0)
	{
		printf("vo init failed while vo resume.\n");
		return -1;
	}

	s32Ret = AR_MPI_VO_Resume(0);
	if(s32Ret < 0)
	{
		printf("vo resume failed.\n");
		return -1;
	}

	return 0;
}

/********  VPSS platform APIs  *****************/
AR_S32 IPC_PF_VPSS_StartGrp(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S * pstVpssGrpAttr, AR_U32 u32ChnBmp, VPSS_CHN_ATTR_S * pastChnAttr)
{
    VPSS_CHN VpssChn;
    AR_S32 s32Ret;
    AR_S32 j;

    s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return AR_FAILURE;
    }

    for (j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
    {
        if(u32ChnBmp & (1<<j) )
        {
            VpssChn = j;
            s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastChnAttr[VpssChn]);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }

            s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }
        }
    }

    return AR_SUCCESS;

}

AR_S32 IPC_PF_VPSS_StopGrp(VPSS_GRP VpssGrp, AR_U32 u32ChnBmp)
{
    AR_S32 j;
    AR_S32 s32Ret = AR_SUCCESS;
    VPSS_CHN VpssChn;

    for (j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
    {
        if(u32ChnBmp & (1<<j))
        {
            VpssChn = j;
            s32Ret = AR_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("failed with %#x!\n", s32Ret);
                return AR_FAILURE;
            }
        }
    }

    s32Ret = AR_MPI_VPSS_StopGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

/* call MPP APIs to set vpss attributes */
AR_S32 IPC_PF_VPSS_Init()
{
    ProfileStart(__FUNCTION__);

    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

    VPSS_GRP_ATTR_S    stVpssGrpAttr = {0};
    AR_U32             u32ChnBmp = 0;
    VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    VPSS_CROP_INFO_S   stCrop = {0};
    AR_S32 s32Ret = 0;
    AR_U32 i = 0, j = 0;
    AR_U32 u32LowBw = 0;

    //since we don't support mixed "basic" and "low_bw" pipeline in this demo, so we just judge from one channel.
    u32LowBw = IPC_PF_CheckPipelineLowBwMode(pChnCfg);

    for(i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i++)
    {
        if(!pVpssCfg->stGrpCfg[i].bGroup)
        {
            continue;
        }

        memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
        memset(astVpssChnAttr, 0, sizeof(astVpssChnAttr));
        /*config vpss*/
        stVpssGrpAttr.enDynamicRange                 = DYNAMIC_RANGE_SDR8;
        stVpssGrpAttr.enPixelFormat                  = PIXEL_FORMAT_YVU_PLANAR_420;
        stVpssGrpAttr.enCompressMode = COMPRESS_MODE_NONE;
        stVpssGrpAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
        stVpssGrpAttr.u32Depth = 1;
        stVpssGrpAttr.stAspectRatio.enMode = ASPECT_RATIO_NONE;
        stVpssGrpAttr.u32ScaleMode = 0;
        stVpssGrpAttr.enChnMode = VPSS_CHN_MODE_USER;

        //set grp same as input(here is vi)
        if(u32LowBw)
        {
            if(i % 2 == 0) //this grp should bind to vi main ch
            {
                stVpssGrpAttr.stFrameRate.s32SrcFrameRate = pVioCfg->vi[i/2].channel_0_fps;
                stVpssGrpAttr.stFrameRate.s32DstFrameRate = pVioCfg->vi[i/2].channel_0_fps;
                if(pVpssCfg->stGrpCfg[i].stRotate.bEnable &&
                    (pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_90 ||
                        pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_270))
                {
                    stVpssGrpAttr.u32Width = pVioCfg->vi[i/2].height;
                    stVpssGrpAttr.u32Height = pVioCfg->vi[i/2].width;
                }
                else
                {
                    stVpssGrpAttr.u32Width = pVioCfg->vi[i/2].width;
                    stVpssGrpAttr.u32Height = pVioCfg->vi[i/2].height;
                }
            }
            else //bind to vi sub ch
            {
                stVpssGrpAttr.stFrameRate.s32SrcFrameRate = pVioCfg->vi[i/2].channel_1_fps;
                stVpssGrpAttr.stFrameRate.s32DstFrameRate = pVioCfg->vi[i/2].channel_1_fps;
                if(pVpssCfg->stGrpCfg[i].stRotate.bEnable &&
                    (pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_90 ||
                    pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_270))
                {
                    stVpssGrpAttr.u32Width = pVioCfg->vi[i/2].channel_1_height;
                    stVpssGrpAttr.u32Height = pVioCfg->vi[i/2].channel_1_width;
                }
                else
                {
                    stVpssGrpAttr.u32Width = pVioCfg->vi[i/2].channel_1_width;
                    stVpssGrpAttr.u32Height = pVioCfg->vi[i/2].channel_1_height;
                }
            }
        }
        else
        {
            stVpssGrpAttr.stFrameRate.s32SrcFrameRate = pVioCfg->vi[i].channel_0_fps;
            stVpssGrpAttr.stFrameRate.s32DstFrameRate = pVioCfg->vi[i].channel_0_fps;
            stVpssGrpAttr.u32Width = pVioCfg->vi[i].width;
            stVpssGrpAttr.u32Height = pVioCfg->vi[i].height;
        }

        stVpssGrpAttr.bFlip = pVpssCfg->stGrpCfg[i].bFlip;
        stVpssGrpAttr.bMirror = pVpssCfg->stGrpCfg[i].bMirror;
        stVpssGrpAttr.u32ScaleMode = 1; //1 bilinear, 0 bicubic
        if(pVpssCfg->stGrpCfg[i].stRotate.bEnable)
        {
            if(pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_0)
            {
                stVpssGrpAttr.enRotation = ROTATION_0;
            }
            if(pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_90)
            {
                stVpssGrpAttr.enRotation = ROTATION_90;
            }
            if(pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_180)
            {
                stVpssGrpAttr.enRotation = ROTATION_180;
            }
            if(pVpssCfg->stGrpCfg[i].stRotate.enAngle == VPSS_ROTATE_270)
            {
                stVpssGrpAttr.enRotation = ROTATION_270;
            }
        }

        if(pVpssCfg->stGrpCfg[i].bLdc)
        {
            PRINT_INFO("LDC enabled, please make sure the LDC parameters match your camera!\n");
            stVpssGrpAttr.stLdcAttr.bEnable = AR_TRUE;
            stVpssGrpAttr.stLdcAttr.stAttr.ldc_k0 = pVpssCfg->stGrpCfg[i].stLdcCoeff.f32K0;
            stVpssGrpAttr.stLdcAttr.stAttr.ldc_k1 = pVpssCfg->stGrpCfg[i].stLdcCoeff.f32K1;
            stVpssGrpAttr.stLdcAttr.stAttr.ldc_k2 = pVpssCfg->stGrpCfg[i].stLdcCoeff.f32K2;
            memcpy(stVpssGrpAttr.stLdcAttr.stAttr.k, pVpssCfg->stGrpCfg[i].stLdcCoeff.f32K, sizeof(stVpssGrpAttr.stLdcAttr.stAttr.k));
        }

        int cover = 0;
        for(cover = 0; cover < pVpssCfg->stGrpCfg[i].stCover.u32Num; cover++)
        {
            //To do
        }

        int mosaic = 0;
        for(mosaic = 0; mosaic < pVpssCfg->stGrpCfg[i].stMosaic.u32Num; mosaic++)
        {
            //to do
        }

        u32ChnBmp = 0;
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            if(!pVpssCfg->stGrpCfg[i].stChnCfg[j].bEnable)
            {
                continue;
            }

            astVpssChnAttr[j].bFlip = pVpssCfg->stGrpCfg[i].stChnCfg[j].bFlip;
            astVpssChnAttr[j].bMirror = pVpssCfg->stGrpCfg[i].stChnCfg[j].bMirror;
            astVpssChnAttr[j].enChnMode = VPSS_CHN_MODE_USER;
            astVpssChnAttr[j].u32Width = pVpssCfg->stGrpCfg[i].stChnCfg[j].u32Width;
            astVpssChnAttr[j].u32Height = pVpssCfg->stGrpCfg[i].stChnCfg[j].u32Height;
            astVpssChnAttr[j].enCompressMode = COMPRESS_MODE_NONE;
            astVpssChnAttr[j].enDynamicRange = DYNAMIC_RANGE_SDR8;
            astVpssChnAttr[j].enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            astVpssChnAttr[j].enVideoFormat = VIDEO_FORMAT_LINEAR;
            astVpssChnAttr[j].stAspectRatio.enMode = ASPECT_RATIO_NONE;
            astVpssChnAttr[j].stFrameRate.s32SrcFrameRate = stVpssGrpAttr.stFrameRate.s32SrcFrameRate;
            astVpssChnAttr[j].stFrameRate.s32DstFrameRate = stVpssGrpAttr.stFrameRate.s32DstFrameRate;
            astVpssChnAttr[j].u32Depth = 1;
            astVpssChnAttr[j].u32ScaleMode = 1; //1 bilinear mode

            u32ChnBmp |= (1 << j);

            pthread_mutex_init(&g_VpssMutex[i * IPC_MAX_STREAM_PER_PIPE + j], 0);
        }

        s32Ret = IPC_PF_VPSS_StartGrp(i, &stVpssGrpAttr, u32ChnBmp, astVpssChnAttr);
        if(s32Ret)
        {
            PRINT_ERR("Start vpss grp %d failed.\n", i);
            return s32Ret;
        }

        //set crop attr
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            if(!pVpssCfg->stGrpCfg[i].stChnCfg[j].bEnable)
            {
                continue;
            }

            if(!pVpssCfg->stGrpCfg[i].stChnCfg[j].stCrop.bEnable)
            {
                continue;
            }

            stCrop.bEnable = pVpssCfg->stGrpCfg[i].stChnCfg[j].stCrop.bEnable;
            stCrop.enCropCoordinate = VPSS_CROP_ABS_COOR;
            stCrop.stCropRect.s32X = pVpssCfg->stGrpCfg[i].stChnCfg[j].stCrop.stRect.u32X;
            stCrop.stCropRect.s32Y = pVpssCfg->stGrpCfg[i].stChnCfg[j].stCrop.stRect.u32Y;
            stCrop.stCropRect.u32Width = pVpssCfg->stGrpCfg[i].stChnCfg[j].stCrop.stRect.u32W;
            stCrop.stCropRect.u32Height = pVpssCfg->stGrpCfg[i].stChnCfg[j].stCrop.stRect.u32H;

            s32Ret = AR_MPI_VPSS_SetChnCrop(i, j, &stCrop);
            if(s32Ret)
            {
                PRINT_ERR("Set vpss grp %d chn %d crop attr failed.\n", i, j);
                return s32Ret;
            }
        }
    }

    ProfileEnd(__FUNCTION__);

    return 0;
}
AR_S32 IPC_PF_VPSS_UnInit()
{
    AR_S32 s32Ret = 0;
    AR_U32 i = 0, j = 0;
    AR_U32 u32ChnBmp = 0;
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();

    for(i = 0; i < IPC_MAX_VPSS_GRP_NEEDED; i++)
    {
        if(!pVpssCfg->stGrpCfg[i].bGroup)
        {
            continue;
        }

        u32ChnBmp = 0;
        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            if(pVpssCfg->stGrpCfg[i].stChnCfg[j].bEnable)
            {
                u32ChnBmp |= (1<<j);
            }
        }

        s32Ret = IPC_PF_VPSS_StopGrp(i, u32ChnBmp);
        if(s32Ret)
        {
            PRINT_ERR("Stop vpss grp %d fail.\n", i);
            return s32Ret;
        }
    }

    return 0;
}

AR_S32 IPC_PF_VPSS_Suspend()
{
	AR_S32 s32Ret = 0;

	s32Ret = IPC_PF_VPSS_UnInit();
	if(s32Ret < 0)
    {
        printf("vpss uninit failed while suspend .\n");
        return -1;
    }
	
    s32Ret = AR_MPI_VPSS_Suspend();
    if(s32Ret < 0)
    {
        printf("vpss suspend failed.\n");
        return -1;
    }

	return 0;
}

AR_S32 IPC_PF_VPSS_Resume()
{
	AR_S32 s32Ret = 0;

	s32Ret = IPC_PF_VPSS_Init();
	if(s32Ret < 0)
    {
        printf("vpss init failed while suspend .\n");
        return -1;
    }
	
    s32Ret = AR_MPI_VPSS_Resume();
    if(s32Ret)
    {
        printf("vpss resume failed.\n");
        return -1;
    }

	return 0;
}


AR_S32 IPC_PF_VPSS_SetRotate(AR_U32 u32GrpId, VPSS_ROTATE_S * pstRotate)
{
    return 0;
}

AR_S32 IPC_PF_VPSS_SetFlip(AR_U32 u32GrpId, AR_S32 u32ChnId, AR_U32 u32FlipEnable)
{
  AR_S32 s32Ret = 0;
  //VPSS_GRP_ATTR_S stGrpAttr = {0};
  VPSS_CHN_ATTR_S stChnAttr = {0};

  pthread_mutex_lock(&g_VpssMutex[u32GrpId * IPC_MAX_STREAM_PER_PIPE + u32ChnId]);
  s32Ret = AR_MPI_VPSS_GetChnAttr(u32GrpId, u32ChnId, &stChnAttr);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VPSS_GetChnAttr groupId %d channel %d failed!\n", u32GrpId, u32ChnId);
    goto END;
  }

  stChnAttr.bFlip = u32FlipEnable;
  s32Ret = AR_MPI_VPSS_SetChnAttr(u32GrpId, u32ChnId, &stChnAttr);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VPSS_SetChnAttr groupId %d channelId %d flip %d failed!\n", u32GrpId, u32ChnId, stChnAttr.bFlip);
    goto END;
  }
  PRINT_INFO("AR_MPI_VPSS_SetChnAttr groupId %d channelId %d flip %d success.\n", u32GrpId, u32ChnId, stChnAttr.bFlip);

END:
  pthread_mutex_unlock(&g_VpssMutex[u32GrpId * IPC_MAX_STREAM_PER_PIPE + u32ChnId]);
  return s32Ret;

}
AR_S32 IPC_PF_VPSS_SetMirror(AR_U32 u32GrpId, AR_S32 u32ChnId, AR_U32 u32MirrorEnable)
{
  AR_S32 s32Ret = 0;
  //VPSS_GRP_ATTR_S stGrpAttr = {0};
  VPSS_CHN_ATTR_S stChnAttr = {0};

  pthread_mutex_lock(&g_VpssMutex[u32GrpId * IPC_MAX_STREAM_PER_PIPE + u32ChnId]);

  s32Ret = AR_MPI_VPSS_GetChnAttr(u32GrpId, u32ChnId, &stChnAttr);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VPSS_GetChnAttr groupId %d channel %d failed!\n", u32GrpId, u32ChnId);
    goto END;
  }
  stChnAttr.bMirror = u32MirrorEnable;
  s32Ret = AR_MPI_VPSS_SetChnAttr(u32GrpId, u32ChnId, &stChnAttr);
  if (s32Ret != AR_SUCCESS)
  {
    PRINT_ERR("AR_MPI_VPSS_SetChnAttr groupId %d channelId %d mirror %d failed!\n", u32GrpId, u32ChnId, stChnAttr.bMirror);
    goto END;
  }
  PRINT_INFO("AR_MPI_VPSS_SetChnAttr groupId %d channelId %d mirror %d success.\n", u32GrpId, u32ChnId, stChnAttr.bMirror);

END:
  pthread_mutex_unlock(&g_VpssMutex[u32GrpId * IPC_MAX_STREAM_PER_PIPE + u32ChnId]);
  return s32Ret;

}
AR_S32 IPC_PF_VPSS_SetEis(AR_U32 u32GrpId, AR_U32 u32Enable)
{
    return 0;
}
AR_S32 IPC_PF_VPSS_SetLdc(AR_U32 u32GrpId, AR_U32 u32Enable)
{
    return 0;
}
AR_S32 IPC_PF_VPSS_SetMosaic(AR_U32 u32GrpId, VPSS_MOSAIC_S * pstMosaic, AR_U32 u32Num)
{
    return 0;
}
AR_S32 IPC_PF_VPSS_SetCover(AR_U32 u32GrpId, VPSS_MOSAIC_S * pstCover, AR_U32 u32Num)
{
    return 0;
}

AR_S32 IPC_PF_VPSS_SetCrop(AR_U32 u32GrpId, AR_U32 u32ChnId, VPSS_CROP_S * pstCrop)
{
    return 0;
}

AR_S32 IPC_PF_VPSS_SetRes(AR_U32 u32GrpId, AR_U32 u32ChnId, AR_U32 u32Width, AR_U32 u32Height)
{
    AR_S32 s32Ret = 0;
    VPSS_CHN_ATTR_S stChnAttr = {0};
    IPC_CFG_VPSS_S *pstVpssCfg = IPC_CFG_VPSS_GetParam();

    PRINT_INFO("IPC_PF_VPSS_SetRes, u32GrpId=%d, u32ChnId=%d\n", u32GrpId, u32ChnId);

    s32Ret = AR_MPI_VPSS_GetChnAttr(u32GrpId, u32ChnId, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
      PRINT_ERR("AR_MPI_VPSS_GetChnAttr failed with %#x\n", s32Ret);
      return AR_FAILURE;
    }

    stChnAttr.u32Width = u32Width;
    stChnAttr.u32Height = u32Height;

    s32Ret = AR_MPI_VPSS_SetChnAttr(u32GrpId, u32ChnId, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
      PRINT_ERR("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
      return AR_FAILURE;
    }

    pthread_mutex_lock(&g_VpssMutex[u32GrpId * IPC_MAX_STREAM_PER_PIPE + u32ChnId]);
    pstVpssCfg->stGrpCfg[u32GrpId].stChnCfg[u32ChnId].u32Width = u32Width;
    pstVpssCfg->stGrpCfg[u32GrpId].stChnCfg[u32ChnId].u32Height = u32Height;
    pthread_mutex_unlock(&g_VpssMutex[u32GrpId * IPC_MAX_STREAM_PER_PIPE + u32ChnId]);

    IPC_CFG_VPSS_Save();

    return 0;
}

AR_S32 IPC_PF_VPSS_SetYUV2RGB(AR_U32 u32GrpId, AR_U32 u32ChnId, AR_U32 u32Enable)
{
    return 0;
}

/********  VENC platform APIs  *****************/
AR_S32 IPC_PF_VENC_SetRes(AR_U32 pipeNum, AR_U32 u32ChnId, AR_U32 u32Width, AR_U32 u32Height)
{
    AR_S32 s32Ret = 0, vencChn = 0;
    VENC_CHN_ATTR_S stChnAttr = {0};
    IPC_CFG_VENC_S *pstVencCfg = IPC_CFG_VENC_GetParam();

    vencChn = pipeNum * IPC_MAX_VENC_STREAM_PER_CHN + u32ChnId;

    s32Ret = AR_MPI_VENC_GetChnAttr(vencChn, &stChnAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get Venc chn attr failed.\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_VENC_StopRecvFrame(vencChn);
    if (s32Ret != 0)
    {
        PRINT_ERR("dev id:%d stop failed! ret=%d\n", vencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_VENC_DestroyChn(vencChn);
    if (s32Ret)
    {
        PRINT_ERR("destroy channel failed, vencChn:%d s32Ret:%02x\n", vencChn, s32Ret);
        return s32Ret;
    }

    stChnAttr.stVencAttr.u32PicWidth = u32Width;
    stChnAttr.stVencAttr.u32PicHeight = u32Height;
    stChnAttr.stVencAttr.u32MaxPicWidth = u32Width;
    stChnAttr.stVencAttr.u32MaxPicHeight = u32Height;

    s32Ret = AR_MPI_VENC_CreateChn(vencChn, &stChnAttr);
    if (s32Ret)
    {
      PRINT_ERR("create venc channel failed: %d\n", s32Ret);
      return s32Ret;
    }

    VENC_RECV_PIC_PARAM_S stParam;
    stParam.s32RecvPicNum = -1;

    s32Ret = AR_MPI_VENC_StartRecvFrame(vencChn, &stParam);
    if (s32Ret != 0)
    {
      PRINT_ERR("dev id:%d start failed! ret=%d\n", vencChn, s32Ret);
      return s32Ret;
    }

    pthread_mutex_lock(&g_VencMutex[vencChn]);
    pstVencCfg->vencStream[pipeNum][u32ChnId].cfg.width = u32Width;
    pstVencCfg->vencStream[pipeNum][u32ChnId].cfg.height = u32Height;
    pstVencCfg->vencStream[pipeNum][u32ChnId].cfg.roi_width = u32Width;
    pstVencCfg->vencStream[pipeNum][u32ChnId].cfg.roi_height = u32Height;
    pthread_mutex_unlock(&g_VencMutex[vencChn]);

    IPC_CFG_VENC_Save();

    return 0;
}

//dynamic set stream resource
AR_S32 IPC_PF_Stream_SetRes(AR_U32 u32VpssGrpId, AR_U32 u32VpssChnId, AR_U32 u32Width, AR_U32 u32Height)
{
    AR_S32 s32PipeNum = 0,s32Stream = 0;
    AR_S32 i = 0, s32SubChannelSum = 0;
    AR_S32 s32Ret = 0, vencChn = 0;
    IPC_MPP_CHN_S stVpss, stVenc, stVi;
    IPC_CFG_VENC_S *pstVencCfg = IPC_CFG_VENC_GetParam();
    IPC_CFG_VPSS_S *pstVpssCfg = IPC_CFG_VPSS_GetParam();
    IPC_VI_MAX_RESOURCE_S *pstMaxVioCfg = IPC_CFG_VIO_GetMaxResource();
    IPC_VPSS_MAX_RESOURCE_S *pstMaxVpssCfg = IPC_CFG_VPSS_GetMaxResource();
    IPC_VENC_MAX_RESOURCE_S *pstMaxVencCfg = IPC_CFG_VENC_GetMaxResource();

    s32PipeNum = u32VpssGrpId/2;
    s32Stream = (u32VpssGrpId%2 == 0 ? 0 : (u32VpssChnId == 0 ? 1:2));
    vencChn = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN + s32Stream;

    if(!pstVencCfg->vencStream[s32PipeNum][s32Stream].enable)
    {
        PRINT_ERR("vpss[%d %d] not bind with venc[%d %d]\n", u32VpssGrpId, u32VpssChnId, s32PipeNum, s32Stream);
        return -1;
    }

  //adjust vpss w/h with default configs
    if(pstMaxVencCfg->stVpssMaxResource[s32PipeNum][s32Stream].u32Width < u32Width ||
          pstMaxVencCfg->stVpssMaxResource[s32PipeNum][s32Stream].u32Height < u32Height)
    {
        PRINT_ERR("IPC_PF_Stream_SetRes not support upscale with max resource\n");
        return -1;
    }

    //stop venc receive thread
    s_astVThreadStatus[vencChn].u32Stop = 1;
    usleep(500 * 1000);

    //if change with sub stream and only one stream bind with vpss, change vin w/h
    if(u32VpssGrpId%2 != 0)
    {
        for(i = 0; i < IPC_MAX_STREAM_PER_PIPE;i++)
        {
            if(pstVpssCfg->stGrpCfg[u32VpssGrpId].stChnCfg[i].bEnable)
            {
                s32SubChannelSum++;
            }
        }
        if(s32SubChannelSum == 1)
        {
            if(pstMaxVioCfg->stViMaxResource[s32PipeNum].channel_1_width >= u32Width ||
                pstMaxVioCfg->stViMaxResource[s32PipeNum].channel_1_height >= u32Height)
            {
                IPC_PF_VI_SetRes(s32PipeNum, u32Width, u32Height);
            }
            else
            {
                PRINT_ERR("vi scale resource is large than the max resource \n");
                s32Ret = -1;
                goto EXIT;
            }
        }
    }
    else
    {
        if(!pstVpssCfg->stGrpCfg[u32VpssGrpId].bGroup || !pstVpssCfg->stGrpCfg[u32VpssGrpId].stChnCfg[u32VpssChnId].bEnable)
        {
            PRINT_ERR("the main stream bind vi with venc dont support change resource \n");
            s32Ret = -1;
            goto EXIT;
        }
    }

    //ubind vpss or vi with venc
    if(pstVpssCfg->stGrpCfg[u32VpssGrpId].bGroup && pstVpssCfg->stGrpCfg[u32VpssGrpId].stChnCfg[u32VpssChnId].bEnable)
    {
      if(pstMaxVpssCfg->stVpssMaxResource[u32VpssGrpId][u32VpssChnId].u32Width < u32Width ||
          pstMaxVpssCfg->stVpssMaxResource[u32VpssGrpId][u32VpssChnId].u32Height < u32Height)
        {
       PRINT_ERR("the stream bind venc with vpss large than the max vpss setting \n");
             s32Ret = -1;
             goto EXIT;
    }
        stVpss.enModId = MOD_ID_VPSS;
        stVpss.u32DevId = u32VpssGrpId;
        stVenc.enModId = MOD_ID_VENC;
        stVenc.u32DevId = 0;

        if(s32Stream == 0)
        {
            stVpss.u32ChnId = 0;
            stVenc.u32ChnId = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN;
        }
        else if(s32Stream == 1)
        {
            stVpss.u32ChnId = 0;
            stVenc.u32ChnId = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN + 1;
        }
        else
        {
            stVpss.u32ChnId = 1;
            stVenc.u32ChnId = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN + 2;
        }

        s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
        if(s32Ret < 0)
        {
            PRINT_ERR("unbind channel %d error.\n", s32Ret);
            goto EXIT;;
        }

        s32Ret = IPC_PF_VPSS_SetRes(u32VpssGrpId, u32VpssChnId,u32Width, u32Height);
        if(s32Ret < 0)
        {
            PRINT_ERR("IPC_PF_VPSS_SetRes %d error.\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        stVi.enModId = MOD_ID_VI;
        stVi.u32DevId = s32PipeNum;
        stVenc.enModId = MOD_ID_VENC;
        stVenc.u32DevId = 0;

        if(s32Stream == 0)
        {
            stVi.u32ChnId = 0;
            stVenc.u32ChnId = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN;
        }
        else if(s32Stream == 1)
        {
            stVi.u32ChnId = 1;
            stVenc.u32ChnId = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN + 1;
        }
        else
        {
            stVi.u32ChnId = 1;
            stVenc.u32ChnId = s32PipeNum * IPC_MAX_VENC_STREAM_PER_CHN + 2;
        }

        s32Ret = IPC_PF_UnBind(&stVi, &stVenc);
        if(s32Ret < 0)
        {
            PRINT_ERR("unbind channel %d error.\n", s32Ret);
            return s32Ret;
        }
    }

    s32Ret = IPC_PF_VENC_SetRes(s32PipeNum, s32Stream, u32Width, u32Height);
    if(s32Ret < 0)
    {
        PRINT_ERR("IPC_PF_VENC_SetRes %d error.\n", s32Ret);
        return s32Ret;
    }

    //bind vpss/vi with venc
    if(pstVpssCfg->stGrpCfg[u32VpssGrpId].bGroup && pstVpssCfg->stGrpCfg[u32VpssGrpId].stChnCfg[u32VpssChnId].bEnable)
    {
        s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
    }
    else
    {
        s32Ret = IPC_PF_Bind(&stVi, &stVenc);
    }
    if(s32Ret < 0)
    {
        PRINT_ERR("IPC_PF_Bind channel %d error.\n", s32Ret);
        return s32Ret;
    }

EXIT:
    //start venc receive thread
    s_astVThreadStatus[vencChn].u32Id = s32Stream;
    s_astVThreadStatus[vencChn].u32Stop = 0;
    s32Ret = IPC_MID_UTIL_CreateDetachThread(IPC_PF_THREAD_ReadVStream, (void *)&s_astVThreadStatus[vencChn].u32Id, NULL);
    if(s32Ret < 0)
    {
        PRINT_ERR("create read venc failed !\n");
        return -1;
    }

    return s32Ret;
}

AR_S32 IPC_PF_VENC_UnInit()
{
    AR_S32 s32Ret = 0;
    int i = 0, j = 0, vencChn = 0;
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

    //snapshot chn
    if(pstVencCfg->vencStreamSnap.enable)
    {
        vencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN;

        pthread_mutex_destroy(&g_VencMutex[vencChn]);
        s32Ret = AR_MPI_VENC_DestroyChn(vencChn);
        if (s32Ret)
        {
            PRINT_ERR("create channel failed\n");
            return s32Ret;
        }
        
        AR_MPI_VENC_CloseFd(vencChn);
		g_s32VencDevFd[vencChn] = -1;
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }

            pthread_mutex_destroy(&g_VencMutex[vencChn]);
            vencChn = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
            s32Ret = AR_MPI_VENC_StopRecvFrame(vencChn);
            if (s32Ret != 0)
            {
                PRINT_ERR("dev id:%d stop failed! ret=%d\n", vencChn, s32Ret);
                return s32Ret;
            }
        }
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
       for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
       {
           if(!pstVencCfg->vencStream[i][j].enable)
           {
               continue;
           }

           vencChn = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
           s32Ret = AR_MPI_VENC_DestroyChn(vencChn);
           if (s32Ret)
           {
               PRINT_ERR("destroy channel failed, vencChn:%d s32Ret:%02x\n", vencChn, s32Ret);
               return s32Ret;
           }

           AR_MPI_VENC_CloseFd(vencChn);
           g_s32VencDevFd[vencChn] = -1;
       }
    }

    return 0;
}

AR_S32 IPC_PF_VENC_Init()
{
    ProfileStart(__FUNCTION__);

    AR_S32 s32Ret = 0;
    int i = 0, j = 0, vencChn = 0;
    AR_U32 u32EncType = 0, u32RcMode = 0;
    VENC_CHN_ATTR_S stChnAttr = {0};
    VENC_RC_PARAM_S stRcParam = {0};
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    WDR_MODE_E         enWDRMode = WDR_MODE_NONE;
    IPC_SNS_TYPE_E     enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT;

    IPC_PF_VENC_SetVencFrequency();
    IPC_PF_VENC_SetHighPerformance();
    IPC_PF_VENC_SetVencMsgPoolSize(pstVencCfg);

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        enWDRMode = pstVioCfg->vi[i].HDR_enable ? WDR_MODE_2To1_LINE : WDR_MODE_NONE;
        enSnsType = AR_VI_GetSensorType(pstVioCfg->vi[i].sensor, enWDRMode, pstVioCfg->vi[i].fps);

        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }

            memset(&stChnAttr, 0, sizeof(stChnAttr));

            u32EncType = IPC_PF_GetEncType(pstVencCfg->vencStream[i][j].cfg.encodeType);
            u32RcMode = IPC_PF_GetRcMode(pstVencCfg->vencStream[i][j].cfg.encodeType, pstVencCfg->vencStream[i][j].cfg.brcMode);

            stChnAttr.stVencAttr.enType = u32EncType;

            stChnAttr.stVencAttr.u32PicWidth = pstVencCfg->vencStream[i][j].cfg.roi_width;
            stChnAttr.stVencAttr.u32PicHeight = pstVencCfg->vencStream[i][j].cfg.roi_height;
            stChnAttr.stVencAttr.u32MaxPicWidth = pstVencCfg->vencStream[i][j].cfg.width;
            stChnAttr.stVencAttr.u32MaxPicHeight = pstVencCfg->vencStream[i][j].cfg.height;
            stChnAttr.stVencAttr.u32BufSize = 2*1024*1024 + (stChnAttr.stVencAttr.u32PicWidth * stChnAttr.stVencAttr.u32PicHeight * 3)/2;//base on max bitrate
            stChnAttr.stVencAttr.bByFrame = AR_TRUE;
            stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            if(stChnAttr.stVencAttr.enType == PT_H264)
            {
                 if(enSnsType == TP9930_DVP_4VC_1080P_25FPS || enSnsType == TP9930_DVP_4VC_1080P_30FPS)
                 {
                     stChnAttr.stVencAttr.stAttrH264e.u32CmdQueueDepth = 4;
                 }
                 else
                 {
                     stChnAttr.stVencAttr.stAttrH264e.u32CmdQueueDepth = 2;
                 }
                 stChnAttr.stVencAttr.stAttrH264e.u8KeyFrameSizeMultiplier = 3;
                 stChnAttr.stVencAttr.stAttrH264e.u8NonKeyFrameSizeMultiplier = 2;
                 /* cf50 enable */
                 //stChnAttr.stVencAttr.stAttrH264e.s32Cframe50Enable = 0;
                 //stChnAttr.stVencAttr.stAttrH264e.s32Cframe50Tx16Y = 50;
                 //stChnAttr.stVencAttr.stAttrH264e.s32Cframe50Tx16C = 50;
                 //stChnAttr.stVencAttr.stAttrH264e.s32Cframe50LosslessEnable = 1;
            }
            else if(stChnAttr.stVencAttr.enType == PT_H265)
            {
                if(enSnsType == TP9930_DVP_4VC_1080P_25FPS || enSnsType == TP9930_DVP_4VC_1080P_30FPS)
                {
                    stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 4;
                }
                else
                {
                    stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;
                }
                stChnAttr.stVencAttr.stAttrH265e.u8KeyFrameSizeMultiplier = 3;
                stChnAttr.stVencAttr.stAttrH265e.u8NonKeyFrameSizeMultiplier = 2;
                /* cf50 enable */
                stChnAttr.stVencAttr.stAttrH265e.s32Cframe50Enable = 0;
                stChnAttr.stVencAttr.stAttrH265e.s32Cframe50Tx16Y = 50;
                stChnAttr.stVencAttr.stAttrH265e.s32Cframe50Tx16C = 50;
                //stChnAttr.stVencAttr.stAttrH265e.s32Cframe50LosslessEnable = 1;
            }
            else if(stChnAttr.stVencAttr.enType == PT_JPEG)
            {
                stChnAttr.stVencAttr.u32BufSize = stChnAttr.stVencAttr.u32PicWidth * stChnAttr.stVencAttr.u32PicHeight*3/2;//base on pic size
                stChnAttr.stVencAttr.stAttrJpege.bSupportDCF = AR_FALSE;
                stChnAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum = 0;
                stChnAttr.stVencAttr.stAttrJpege.enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
            }

            stChnAttr.stRcAttr.enRcMode = u32RcMode;

            if(u32RcMode == VENC_RC_MODE_H264CBR)
            {
                stChnAttr.stRcAttr.stH264Cbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(i,j);//pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH264Cbr.u32BitRate = pstVencCfg->vencStream[i][j].cfg.cbrAvgBps;
                stChnAttr.stRcAttr.stH264Cbr.u32Gop = pstVencCfg->vencStream[i][j].cfg.gop;
                stChnAttr.stRcAttr.stH264Cbr.u32StatTime = pstVencCfg->vencStream[i][j].cfg.statTime;

                if (stChnAttr.stVencAttr.u32BufSize < stChnAttr.stRcAttr.stH264Cbr.u32BitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate*/ / 8)
                    stChnAttr.stVencAttr.u32BufSize = stChnAttr.stRcAttr.stH264Cbr.u32BitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate*/ / 8;
            }
            else if(u32RcMode == VENC_RC_MODE_H265CBR)
            {
                stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(i,j);//pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH265Cbr.u32BitRate = pstVencCfg->vencStream[i][j].cfg.cbrAvgBps;
                stChnAttr.stRcAttr.stH265Cbr.u32Gop = pstVencCfg->vencStream[i][j].cfg.gop;
                stChnAttr.stRcAttr.stH265Cbr.u32StatTime = pstVencCfg->vencStream[i][j].cfg.statTime;

                if (stChnAttr.stVencAttr.u32BufSize < stChnAttr.stRcAttr.stH265Cbr.u32BitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate*/ / 8)
                    stChnAttr.stVencAttr.u32BufSize = stChnAttr.stRcAttr.stH265Cbr.u32BitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate*/ / 8;
            }
            else if(u32RcMode == VENC_RC_MODE_H264VBR)
            {
                stChnAttr.stRcAttr.stH264Vbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(i,j);//pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate = pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate = pstVencCfg->vencStream[i][j].cfg.vbrMaxBps;
                stChnAttr.stRcAttr.stH264Vbr.u32MinBitRate = pstVencCfg->vencStream[i][j].cfg.vbrMinBps;
                stChnAttr.stRcAttr.stH264Vbr.u32Gop = pstVencCfg->vencStream[i][j].cfg.gop;
                stChnAttr.stRcAttr.stH264Vbr.u32StatTime = pstVencCfg->vencStream[i][j].cfg.statTime;

                if (stChnAttr.stVencAttr.u32BufSize < stChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate*/ / 8)
                    stChnAttr.stVencAttr.u32BufSize = stChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate*/ / 8;
            }
            else if(u32RcMode == VENC_RC_MODE_H265VBR)
            {
                stChnAttr.stRcAttr.stH265Vbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(i,j);//pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH265Vbr.fr32DstFrameRate = pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate = pstVencCfg->vencStream[i][j].cfg.vbrMaxBps;
                stChnAttr.stRcAttr.stH264Vbr.u32MinBitRate = pstVencCfg->vencStream[i][j].cfg.vbrMinBps;
                stChnAttr.stRcAttr.stH265Vbr.u32Gop = pstVencCfg->vencStream[i][j].cfg.gop;
                stChnAttr.stRcAttr.stH265Vbr.u32StatTime = pstVencCfg->vencStream[i][j].cfg.statTime;
                //PRINT_ERR("vbr fps %d, maxbit %d gop %d\n", stChnAttr.stRcAttr.stH265Vbr.u32SrcFrameRate,
                //    stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate, stChnAttr.stRcAttr.stH265Vbr.u32Gop);
                if (stChnAttr.stVencAttr.u32BufSize < stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH265Vbr.fr32DstFrameRate*/ / 8)
                    stChnAttr.stVencAttr.u32BufSize = stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH265Vbr.fr32DstFrameRate*/ / 8;
            }
            else if(u32RcMode == VENC_RC_MODE_MJPEGCBR)
            {
                stChnAttr.stRcAttr.stMjpegCbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(i,j);//pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stMjpegCbr.fr32DstFrameRate = pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stMjpegCbr.u32BitRate = pstVencCfg->vencStream[i][j].cfg.cbrAvgBps;
                stChnAttr.stRcAttr.stMjpegCbr.u32StatTime = pstVencCfg->vencStream[i][j].cfg.statTime;
            }
            else if(u32RcMode == VENC_RC_MODE_MJPEGVBR)
            {
                stChnAttr.stRcAttr.stMjpegVbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(i,j);//pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stMjpegVbr.fr32DstFrameRate = pstVencCfg->vencStream[i][j].cfg.fps;
                stChnAttr.stRcAttr.stMjpegVbr.u32MaxBitRate = pstVencCfg->vencStream[i][j].cfg.vbrMaxBps;
                stChnAttr.stRcAttr.stMjpegVbr.u32StatTime = pstVencCfg->vencStream[i][j].cfg.statTime;
            }
            else
            {
                PRINT_ERR("Not support the RC mode at the moment: %d\n", u32RcMode);
                //return -1;
            }

            stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
            stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;

            vencChn = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
            s32Ret = AR_MPI_VENC_CreateChn(vencChn, &stChnAttr);
            if (s32Ret)
            {
                PRINT_ERR("create venc channel failed: %d\n", s32Ret);
                return s32Ret;
            }

          s32Ret = AR_MPI_VENC_GetRcParam(vencChn, &stRcParam);
          if (s32Ret)
          {
              PRINT_ERR("Get venc rc param error: %d\n", s32Ret);
              return s32Ret;
          }

            if(u32RcMode == VENC_RC_MODE_H264CBR)
            {
                stRcParam.stParamH264Cbr.u32MaxIQp = pstVencCfg->vencStream[i][j].cfg.qpMaxI;
                stRcParam.stParamH264Cbr.u32MaxPQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH264Cbr.u32MaxBQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH264Cbr.u32MinIQp = pstVencCfg->vencStream[i][j].cfg.qpMinI;
                stRcParam.stParamH264Cbr.u32MinPQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
                stRcParam.stParamH264Cbr.u32MinBQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
            }
            if(u32RcMode == VENC_RC_MODE_H265CBR)
            {
                stRcParam.stParamH265Cbr.u32MaxIQp = pstVencCfg->vencStream[i][j].cfg.qpMaxI;
                stRcParam.stParamH265Cbr.u32MaxPQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH265Cbr.u32MaxBQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH265Cbr.u32MinIQp = pstVencCfg->vencStream[i][j].cfg.qpMinI;
                stRcParam.stParamH265Cbr.u32MinPQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
                stRcParam.stParamH265Cbr.u32MinBQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
            }
            if(u32RcMode == VENC_RC_MODE_H264VBR)
            {
                stRcParam.stParamH264Vbr.u32MaxIQp = pstVencCfg->vencStream[i][j].cfg.qpMaxI;
                stRcParam.stParamH264Vbr.u32MaxPQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH264Vbr.u32MaxBQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH264Vbr.u32MinIQp = pstVencCfg->vencStream[i][j].cfg.qpMinI;
                stRcParam.stParamH264Vbr.u32MinPQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
                stRcParam.stParamH264Vbr.u32MinBQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
            }
            if(u32RcMode == VENC_RC_MODE_H265VBR)
            {
                stRcParam.stParamH265Vbr.u32MaxIQp = pstVencCfg->vencStream[i][j].cfg.qpMaxI;
                stRcParam.stParamH265Vbr.u32MaxPQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH265Vbr.u32MaxBQp = pstVencCfg->vencStream[i][j].cfg.qpMaxP;
                stRcParam.stParamH265Vbr.u32MinIQp = pstVencCfg->vencStream[i][j].cfg.qpMinI;
                stRcParam.stParamH265Vbr.u32MinPQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
                stRcParam.stParamH265Vbr.u32MinBQp = pstVencCfg->vencStream[i][j].cfg.qpMinP;
            }

            s32Ret = AR_MPI_VENC_SetRcParam(vencChn, &stRcParam);
            if(s32Ret)
            {
                PRINT_ERR("Set venc rc param error: %d\n", s32Ret);
                return s32Ret;
            }

            g_s32VencDevFd[vencChn] = AR_MPI_VENC_GetFd(vencChn);
            if(g_s32VencDevFd[vencChn] < 0)
            {
                PRINT_ERR("Get venc Fd error: %d\n", s32Ret);
                return -1;
            }
            pthread_mutex_init(&g_VencMutex[vencChn], 0);

        }
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }

            vencChn = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
            VENC_RECV_PIC_PARAM_S stParam;
            stParam.s32RecvPicNum = -1;

            s32Ret = AR_MPI_VENC_StartRecvFrame(vencChn, &stParam);
            if (s32Ret != 0)
            {
                PRINT_ERR("dev id:%d start failed! ret=%d\n", vencChn, s32Ret);
                return s32Ret;
            }
        }
    }

  //create snapshot venc channel
  if(pstVencCfg->vencStreamSnap.enable)
    {
        memset(&stChnAttr, 0, sizeof(stChnAttr));

        u32EncType = IPC_PF_GetEncType(pstVencCfg->vencStreamSnap.cfg.encodeType);
        u32RcMode = IPC_PF_GetRcMode(pstVencCfg->vencStreamSnap.cfg.encodeType, pstVencCfg->vencStreamSnap.cfg.brcMode);

        if(u32EncType != PT_MJPEG && u32EncType != PT_JPEG)
        {
          PRINT_ERR("create JPEG/MJPEG venc channel failed! u32EncType=%d\n", u32EncType);
          return -1;
        }
        stChnAttr.stVencAttr.enType = u32EncType;
        stChnAttr.stVencAttr.u32PicWidth = pstVencCfg->vencStreamSnap.cfg.width;
        stChnAttr.stVencAttr.u32PicHeight = pstVencCfg->vencStreamSnap.cfg.height;
        stChnAttr.stVencAttr.u32MaxPicWidth = pstVencCfg->vencStreamSnap.cfg.width;
        stChnAttr.stVencAttr.u32MaxPicHeight = pstVencCfg->vencStreamSnap.cfg.height;
        stChnAttr.stVencAttr.u32BufSize = 1024 * 1024 + (pstVencCfg->vencStreamSnap.cfg.width * pstVencCfg->vencStreamSnap.cfg.height * 3)/2;
        stChnAttr.stVencAttr.bByFrame = AR_TRUE;
        stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;

        if(stChnAttr.stVencAttr.enType == PT_JPEG)
        {
            stChnAttr.stVencAttr.stAttrJpege.bSupportDCF = AR_FALSE;
            stChnAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum = 0;
            stChnAttr.stVencAttr.stAttrJpege.enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
        }

      stChnAttr.stRcAttr.enRcMode = u32RcMode;

        if(u32RcMode == VENC_RC_MODE_MJPEGCBR)
        {
            stChnAttr.stRcAttr.stMjpegCbr.u32SrcFrameRate = pstVencCfg->vencStreamSnap.cfg.fps;
            stChnAttr.stRcAttr.stMjpegCbr.fr32DstFrameRate = pstVencCfg->vencStreamSnap.cfg.fps;
          stChnAttr.stRcAttr.stMjpegCbr.u32BitRate = pstVencCfg->vencStreamSnap.cfg.cbrAvgBps;
            stChnAttr.stRcAttr.stMjpegCbr.u32StatTime = 30;
        }
        else if(u32RcMode == VENC_RC_MODE_MJPEGVBR)
        {
            stChnAttr.stRcAttr.stMjpegVbr.u32SrcFrameRate = pstVencCfg->vencStreamSnap.cfg.fps;
            stChnAttr.stRcAttr.stMjpegVbr.fr32DstFrameRate = pstVencCfg->vencStreamSnap.cfg.fps;
          stChnAttr.stRcAttr.stMjpegVbr.u32MaxBitRate = pstVencCfg->vencStreamSnap.cfg.vbrMaxBps;
            stChnAttr.stRcAttr.stMjpegVbr.u32StatTime = 30;
        }
        else
        {
            PRINT_ERR("Not support the RC mode at the moment: %d\n", u32RcMode);
            //return -1;
        }

      stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
      stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;

        vencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN;
      s32Ret = AR_MPI_VENC_CreateChn(vencChn, &stChnAttr);
      if (s32Ret)
        {
        PRINT_ERR("create venc channel failed: %d\n", s32Ret);
        return s32Ret;
      }

        #if 0
        VENC_RECV_PIC_PARAM_S stParam;
        stParam.s32RecvPicNum = -1;

        s32Ret = AR_MPI_VENC_StartRecvFrame(vencChn, &stParam);
        if (s32Ret != 0)
        {
            PRINT_ERR("dev id:%d start failed! ret=%d\n", vencChn, s32Ret);
            return s32Ret;
        }
        #endif

        g_s32VencDevFd[vencChn] = AR_MPI_VENC_GetFd(vencChn);
        if(g_s32VencDevFd[vencChn] < 0)
        {
            PRINT_ERR("Get venc Fd error: %d\n", s32Ret);
            return -1;
        }
        pthread_mutex_init(&g_VencMutex[vencChn], 0);
    }

  ProfileEnd(__FUNCTION__);

    return 0;
}

AR_S32 IPC_PF_VENC_Suspend()
{
	AR_S32 s32Ret = 0;
	
	int i = 0, j = 0, vencChn = 0;
	IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
	
	s32Ret = IPC_PF_VENC_UnInit();
	if(s32Ret < 0)
	{
		printf("venc uninit failed while venc suspend\n");
		return -1;
	}

	//snapshot chn
    if(pstVencCfg->vencStreamSnap.enable)
    {
        vencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN;
        s32Ret = AR_MPI_VENC_Suspend(vencChn);
        if(s32Ret < 0)
        {
            printf("AR_MPI_VENC_Suspend chn %d failed \n", vencChn);
            return -1;
        }
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }
            
            vencChn = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
            s32Ret = AR_MPI_VENC_Suspend(vencChn);
            if(s32Ret < 0)
            {
                printf("AR_MPI_VENC_Suspend chn %d failed \n", vencChn);
                return -1;
            }
        }
    }

	return 0;
}

AR_S32 IPC_PF_VENC_Resume()
{
	AR_S32 s32Ret = 0;
	
	int i = 0, j = 0, vencChn = 0;
	IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
	
	s32Ret = IPC_PF_VENC_Init();
	if(s32Ret < 0)
	{
		printf("venc uninit failed while venc suspend\n");
		return -1;
	}

	//snapshot chn
    if(pstVencCfg->vencStreamSnap.enable)
    {
        vencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN;
        s32Ret = AR_MPI_VENC_Resume(vencChn);
        if(s32Ret < 0)
        {
            printf("AR_MPI_VENC_Suspend chn %d failed \n", vencChn);
            return -1;
        }
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(!pstVencCfg->vencStream[i][j].enable)
            {
                continue;
            }
            
            vencChn = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
            s32Ret = AR_MPI_VENC_Resume(vencChn);
            if(s32Ret < 0)
            {
                printf("AR_MPI_VENC_Suspend chn %d failed \n", vencChn);
                return -1;
            }
        }
    }

    return 0;
}

AR_S32 IPC_PF_VENC_RequestIDR(VENC_CHN VeChn)
{
    return AR_MPI_VENC_RequestIDR(VeChn, AR_TRUE);
}

AR_S32 IPC_PF_VENC_Color2Gray(VENC_CHN VeChn, AR_U32 bColor2Grey) {return 0;}
AR_S32 IPC_PF_VENC_GetVideoParam(VENC_CHN VeChn, IPC_VENC_VIDEO_PARAM_S *p)
{
    AR_S32 s32Ret = 0;
    VENC_CHN_ATTR_S stChnAttr = {0};
    //VENC_RC_PARAM_S stRcParam = {0};

    s32Ret = AR_MPI_VENC_GetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get Venc chn attr failed.\n");
        return s32Ret;
    }

    p->u32CodecType = stChnAttr.stVencAttr.enType == PT_H264 ? VENC_ENCODE_TYPE_H264 : VENC_ENCODE_TYPE_H265;
    p->width = stChnAttr.stVencAttr.u32PicWidth;
    p->height = stChnAttr.stVencAttr.u32PicHeight;
    p->s32IPQpDelta = stChnAttr.stGopAttr.stNormalP.s32IPQpDelta;

    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR)
    {
        p->fps = stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate;
        p->Kbps = stChnAttr.stRcAttr.stH264Cbr.u32BitRate;
        p->rc_mode = VENC_BRCMODE_CBR;
        p->gop = stChnAttr.stRcAttr.stH264Cbr.u32Gop;
        p->u32StatTime = stChnAttr.stRcAttr.stH264Cbr.u32StatTime;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
    {
        p->fps = stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate;
        p->Kbps = stChnAttr.stRcAttr.stH265Cbr.u32BitRate;
        p->rc_mode = VENC_BRCMODE_CBR;
        p->gop = stChnAttr.stRcAttr.stH265Cbr.u32Gop;
        p->u32StatTime = stChnAttr.stRcAttr.stH265Cbr.u32StatTime;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264VBR)
    {
        p->fps = stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate;
        p->Kbps = 0;
        p->max_Kbps = stChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate;
        p->min_Kbps = stChnAttr.stRcAttr.stH264Vbr.u32MinBitRate;
        p->rc_mode = VENC_BRCMODE_VBR;
        p->gop = stChnAttr.stRcAttr.stH264Vbr.u32Gop;
        p->u32StatTime = stChnAttr.stRcAttr.stH264Vbr.u32StatTime;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265VBR)
    {
        p->fps = stChnAttr.stRcAttr.stH265Vbr.fr32DstFrameRate;
        p->Kbps = 0;
        p->max_Kbps = stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate;
        p->min_Kbps = stChnAttr.stRcAttr.stH265Vbr.u32MinBitRate;
        p->rc_mode = VENC_BRCMODE_VBR;
        p->gop = stChnAttr.stRcAttr.stH265Vbr.u32Gop;
        p->u32StatTime = stChnAttr.stRcAttr.stH265Cbr.u32StatTime;
    }
    else
    {
        PRINT_ERR("Not supported the rcmode: %d, do it later.\n", stChnAttr.stRcAttr.enRcMode);
        return AR_FAILURE;
    }

    return 0;
}

#define IPC_ENC_TYPE_STRING(x) ((x) == PT_H264 ? "H264" : ( (x) == PT_H265 ? "H265" : "OTHER"))
AR_S32 IPC_PF_VENC_SetVideoParam(VENC_CHN VeChn, IPC_VENC_VIDEO_PARAM_S *p)
{
    AR_S32 s32Ret = 0;
    VENC_CHN_ATTR_S stChnAttr = {0};
    VENC_H265_VUI_S stH265Vui = {0};
    VENC_H264_VUI_S stH264Vui = {0};
    //VENC_RC_PARAM_S stRcParam = {0};
    //VENC_RC_MODE_E enRcMode = 0;
    AR_U32 u32Ch = VeChn / IPC_MAX_VENC_STREAM_PER_CHN;
    AR_U32 u32Stream = VeChn % IPC_MAX_VENC_STREAM_PER_CHN;
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

    s32Ret = AR_MPI_VENC_GetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get Venc chn attr failed.\n");
        return s32Ret;
    }

    PAYLOAD_TYPE_E enCfgType = IPC_PF_GetEncType(p->u32CodecType);
    if(stChnAttr.stVencAttr.enType != enCfgType)
    {
        PRINT_ERR("Codec type change from %s to %s, will take effective after reboot.\n",
            IPC_ENC_TYPE_STRING(stChnAttr.stVencAttr.enType), IPC_ENC_TYPE_STRING(enCfgType));

        goto SAVE_CFG;
    }

    stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = p->s32IPQpDelta;

    stChnAttr.stRcAttr.enRcMode = IPC_PF_GetRcMode(p->u32CodecType, p->rc_mode);
    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR)
    {
        stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = p->fps;
        stChnAttr.stRcAttr.stH264Cbr.u32Gop = p->gop;
        stChnAttr.stRcAttr.stH264Cbr.u32BitRate = p->Kbps; //Unit Kbps;
        stChnAttr.stRcAttr.stH264Cbr.u32StatTime = p->u32StatTime;
        stChnAttr.stVencAttr.enType = PT_H264;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
    {
        stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = p->fps;
        stChnAttr.stRcAttr.stH265Cbr.u32Gop = p->gop;
        stChnAttr.stRcAttr.stH265Cbr.u32BitRate = p->Kbps;
        stChnAttr.stRcAttr.stH265Cbr.u32StatTime = p->u32StatTime;
        stChnAttr.stVencAttr.enType = PT_H265;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264VBR)
    {
        stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate = p->fps;
        stChnAttr.stRcAttr.stH264Vbr.u32Gop = p->gop;
        stChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate = p->max_Kbps; //Unit Kbps;
        stChnAttr.stRcAttr.stH264Vbr.u32MinBitRate = p->min_Kbps;
        stChnAttr.stRcAttr.stH264Vbr.u32StatTime = p->u32StatTime;

        stChnAttr.stVencAttr.enType = PT_H264;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265VBR)
    {
        stChnAttr.stRcAttr.stH265Vbr.fr32DstFrameRate = p->fps;
        stChnAttr.stRcAttr.stH265Vbr.u32Gop = p->gop;
        stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate = p->max_Kbps; //Unit Kbps;
        stChnAttr.stRcAttr.stH265Vbr.u32MinBitRate = p->min_Kbps;
        stChnAttr.stRcAttr.stH265Vbr.u32StatTime = p->u32StatTime;

        stChnAttr.stVencAttr.enType = PT_H265;
    }
    else
    {
        PRINT_ERR("Not support the RC mode at the moment: %d\n", stChnAttr.stRcAttr.enRcMode);
        return -1;
    }

    stChnAttr.stVencAttr.u32PicWidth = p->width;
    stChnAttr.stVencAttr.u32PicHeight = p->height;

    s32Ret = AR_MPI_VENC_SetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Set Venc chn attr failed.\n");
        return s32Ret;
    }

    if (stChnAttr.stVencAttr.enType == PT_H265) {
        s32Ret = AR_MPI_VENC_GetH265Vui(VeChn, &stH265Vui);
        if (s32Ret < 0)
        {
            printf("AR_MPI_VENC_GetH265Vui error.\n");
            return s32Ret;
        }

        stH265Vui.stVuiTimeInfo.num_units_in_tick = 1;
        stH265Vui.stVuiTimeInfo.time_scale = p->fps;
        s32Ret = AR_MPI_VENC_SetH265Vui(VeChn, &stH265Vui);
        if(s32Ret < 0)
        {
            printf("AR_MPI_VENC_SetH265Vui error.\n");
            return s32Ret;
        }
    } else if (stChnAttr.stVencAttr.enType == PT_H264) {
        s32Ret = AR_MPI_VENC_GetH264Vui(VeChn, &stH264Vui);
        if (s32Ret < 0)
        {
            printf("AR_MPI_VENC_GetH264Vui error.\n");
            return s32Ret;
        }

        stH264Vui.stVuiTimeInfo.num_units_in_tick = 1;
        stH264Vui.stVuiTimeInfo.time_scale = p->fps * 2;
        s32Ret = AR_MPI_VENC_SetH264Vui(VeChn, &stH264Vui);
        if(s32Ret < 0)
        {
            printf("AR_MPI_VENC_SetH264Vui error.\n");
            return s32Ret;
        }
    } else {
        printf("not support codec");
    }

SAVE_CFG:
    pthread_mutex_lock(&g_VencMutex[VeChn]);
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.encodeType = p->u32CodecType;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.brcMode = p->rc_mode;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.cbrAvgBps = p->Kbps;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.vbrMaxBps = p->max_Kbps;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.vbrMinBps = p->min_Kbps;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.gop = p->gop;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.fps = p->fps;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.width = p->width;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.height = p->height;

    s32Ret = IPC_CFG_VENC_Save();
    pthread_mutex_unlock(&g_VencMutex[VeChn]);
    return s32Ret;
}

AR_S32 IPC_PF_VENC_GetRcParam(VENC_CHN VeChn, IPC_VENC_RC_PARAM_S *pRc)
{
    AR_S32 s32Ret = 0;
    VENC_CHN_ATTR_S stChnAttr = {0};
    VENC_RC_PARAM_S stRcParam = {0};

    s32Ret = AR_MPI_VENC_GetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get Venc chn attr failed.\n");
        return s32Ret;
    }
    s32Ret = AR_MPI_VENC_GetRcParam(VeChn, &stRcParam);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get Venc chn rc param failed.\n");
        return s32Ret;
    }

    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR)
    {
        pRc->u32MinIQp = stRcParam.stParamH264Cbr.u32MinIQp;
        pRc->u32MaxIQp = stRcParam.stParamH264Cbr.u32MaxIQp;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
    {
        pRc->u32MinIQp = stRcParam.stParamH265Cbr.u32MinIQp;
        pRc->u32MaxIQp = stRcParam.stParamH265Cbr.u32MaxIQp;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264VBR)
    {
        pRc->u32MinIQp = stRcParam.stParamH264Vbr.u32MinIQp;
        pRc->u32MinPQp = stRcParam.stParamH264Vbr.u32MinPQp;
        pRc->u32MaxIQp = stRcParam.stParamH264Vbr.u32MaxIQp;
        pRc->u32MaxPQp = stRcParam.stParamH264Vbr.u32MaxPQp;
    }
    else if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265VBR)
    {
        pRc->u32MinIQp = stRcParam.stParamH265Vbr.u32MinIQp;
        pRc->u32MinPQp = stRcParam.stParamH265Vbr.u32MinPQp;
        pRc->u32MaxIQp = stRcParam.stParamH265Vbr.u32MaxIQp;
        pRc->u32MaxPQp = stRcParam.stParamH265Vbr.u32MaxPQp;
    }
    else
    {
        PRINT_ERR("Rcmode: %d, not in VBR.\n", stChnAttr.stRcAttr.enRcMode);
    }

    return 0;
}

AR_S32 IPC_PF_VENC_SetRcParam(VENC_CHN VeChn, IPC_VENC_RC_PARAM_S * pRc)
{
    AR_S32 s32Ret = 0;
    VENC_CHN_ATTR_S stChnAttr = {0};
    VENC_RC_PARAM_S stRcParam = {0};
    //VENC_RC_MODE_E enRcMode = 0;
    AR_U32 u32Ch = VeChn / IPC_MAX_VENC_STREAM_PER_CHN;
    AR_U32 u32Stream = VeChn % IPC_MAX_VENC_STREAM_PER_CHN;
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

    s32Ret = AR_MPI_VENC_GetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Get Venc chn attr failed.\n");
        return s32Ret;
    }

    //set rc param: min and max Qp
    s32Ret = AR_MPI_VENC_GetRcParam(VeChn, &stRcParam);
    if(s32Ret)
    {
        PRINT_ERR("Get rc param error: %d\n", s32Ret);
        return s32Ret;
    }

    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR)
    {
        stRcParam.stParamH264Cbr.u32MinIQp = pRc->u32MinIQp;
        stRcParam.stParamH264Cbr.u32MaxIQp = pRc->u32MaxIQp;
    }
    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
    {
        stRcParam.stParamH265Cbr.u32MinIQp = pRc->u32MinIQp;
        stRcParam.stParamH265Cbr.u32MaxIQp = pRc->u32MaxIQp;
    }
    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265VBR)
    {
        stRcParam.stParamH265Vbr.u32MinIQp = pRc->u32MinIQp;
        stRcParam.stParamH265Vbr.u32MinPQp = pRc->u32MinPQp;
        stRcParam.stParamH265Vbr.u32MinBQp = pRc->u32MinPQp;
        stRcParam.stParamH265Vbr.u32MaxIQp = pRc->u32MaxIQp;
        stRcParam.stParamH265Vbr.u32MaxPQp = pRc->u32MaxPQp;
        stRcParam.stParamH265Vbr.u32MaxBQp = pRc->u32MaxPQp;
    }
    if(stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264VBR)
    {
        stRcParam.stParamH264Vbr.u32MinIQp = pRc->u32MinIQp;
        stRcParam.stParamH264Vbr.u32MinPQp = pRc->u32MinPQp;
        stRcParam.stParamH264Vbr.u32MinBQp = pRc->u32MinPQp;
        stRcParam.stParamH264Vbr.u32MaxIQp = pRc->u32MaxIQp;
        stRcParam.stParamH264Vbr.u32MaxPQp = pRc->u32MaxPQp;
        stRcParam.stParamH264Vbr.u32MaxBQp = pRc->u32MaxPQp;
    }

    s32Ret = AR_MPI_VENC_SetRcParam(VeChn, &stRcParam);
    if(s32Ret)
    {
        PRINT_ERR("Set venc rc param failed: %d\n", s32Ret);
        return s32Ret;
    }

    pthread_mutex_lock(&g_VencMutex[VeChn]);

    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.qpMinI = pRc->u32MinIQp;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.qpMinP = pRc->u32MinPQp;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.qpMaxI = pRc->u32MaxIQp;
    pstVencCfg->vencStream[u32Ch][u32Stream].cfg.qpMaxP = pRc->u32MaxPQp;

    s32Ret = IPC_CFG_VENC_Save();
    pthread_mutex_unlock(&g_VencMutex[VeChn]);
    return s32Ret;
}
AR_S32 IPC_PF_VENC_IsInit()
{
    return 0;
}
//use frame id or timestamp to check it's alive or not.
AR_S32 IPC_PF_VENC_CheckAlive()
{
    return 0;
}

AR_S32 IPC_PF_VENC_SetJpegQuality(AR_U32 ch, AR_U32 quality){return 0;}

//check which pipeline is on, and create a new thread for each stream.
AR_S32 IPC_PF_VENC_StartMbufThread(void)
{
    ProfileStart(__FUNCTION__);

    AR_S32 i = 0, j = 0, u32Stream = 0;

    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            u32Stream = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
            if(pstVencCfg->vencStream[i][j].enable)
            {
                s_astVThreadStatus[u32Stream].u32Id = u32Stream;
                s_astVThreadStatus[u32Stream].u32Stop = 0;
                IPC_MID_UTIL_CreateDetachThread(IPC_PF_THREAD_ReadVStream, (void *)&s_astVThreadStatus[u32Stream].u32Id, NULL);
            }
        }
    }

    ProfileEnd(__FUNCTION__);

    return 0;
}
AR_S32 IPC_PF_VENC_StopMbufThread(void)
{
    AR_S32 i = 0;

    for(i = 0; i < IPC_TOTAL_VENC_STREAM_NUM; i++)
    {
        s_astVThreadStatus[i].u32Stop = 1;
    }

    //wait they exit.
    usleep(500 * 1000);

  return 0;
}
AR_S32 IPC_PF_VENC_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream)
{
    AR_S32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);

        fflush(pFd);
    }

    return AR_SUCCESS;
}

//The third stream for each channel is for snap for the time being. It has been created in IPC_PF_VENC_Init
AR_S32 IPC_PF_VENC_SnapProcess(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 SnapCnt, AR_CHAR * pSaveDir)
{
    //struct timeval TimeoutVal;
    //fd_set read_fds;
    AR_S32 s32VencFd;
    AR_S32 s32Ret;
    AR_U32 u32Stream;
    AR_U32 i;

    //VENC_RECV_PIC_PARAM_S  stRecvParam = {0};
    VENC_CHN_STATUS_S stStat = {0};
    VENC_STREAM_S stStream = {0};
    VENC_CHN_ATTR_S stChnAttr = {0};
    //IPC_MPP_CHN_S stVpss = {0};
    //IPC_MPP_CHN_S stVi = {0};
    // IPC_MPP_CHN_S stVenc = {0};
    VENC_CHN VencChn = 0;
    VI_CHN ViChn = 0;

    //IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();
    //IPC_CFG_VIO_S *pstVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pstChannelCfg = IPC_CFG_CHANNEL_GetParam();

    if(!pstChannelCfg->stChannel[VpssGrp/2].u32Enable)
    {
        PRINT_ERR("No vi channel %d, snap failed\n", VpssGrp/2);
        return -1;
    }

    /******************************************
    step 1:  Cfg venc ch attr (ch 24)
    ******************************************/
    VencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN; //ch 24 for jpg
    u32Stream = (VpssGrp% 2 == 0 ? 0 : (VpssChn == 0 ? 1:2));
    pthread_mutex_lock(&g_VencMutex[VencChn]);
	s32VencFd = g_s32VencDevFd[VencChn];
    s32Ret = AR_MPI_VENC_GetChnAttr(VencChn, &stChnAttr);
    if(s32Ret)
    {
        PRINT_ERR("Get venc %d attr failed.\n", VencChn);
        pthread_mutex_unlock(&g_VencMutex[VencChn]);
        return s32Ret;
    }

    stChnAttr.stRcAttr.stMjpegCbr.u32SrcFrameRate = IPC_PF_GetViFpsForVenc(VpssGrp/2,u32Stream);
    stChnAttr.stVencAttr.u32MaxPicHeight = pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Height;
    stChnAttr.stVencAttr.u32MaxPicWidth = pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Width;
    stChnAttr.stVencAttr.u32PicHeight = pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Height;
    stChnAttr.stVencAttr.u32PicWidth = pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Width;
    //stChnAttr.stVencAttr.u32BufSize = 50*1024*1024;//SnapCnt * stChnAttr.stVencAttr.u32PicWidth * stChnAttr.stVencAttr.u32PicHeight * 3/2;
    s32Ret = AR_MPI_VENC_SetChnAttr(VencChn, &stChnAttr);
    if(s32Ret)
    {
        PRINT_ERR("Set venc %d attr failed.\n", VencChn);
        pthread_mutex_unlock(&g_VencMutex[VencChn]);
        return s32Ret;
    }

    s32Ret = AR_MPI_VENC_GetChnAttr(VencChn, &stChnAttr);
    if(s32Ret)
    {
        PRINT_ERR("Get venc %d attr failed.\n", VencChn);
        pthread_mutex_unlock(&g_VencMutex[VencChn]);
        return s32Ret;
    }

    VENC_RECV_PIC_PARAM_S stParam;
    stParam.s32RecvPicNum = SnapCnt;

    s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn, &stParam);
    if (s32Ret != 0)
    {
        PRINT_ERR("dev id:%d start failed! ret=%d\n", VencChn, s32Ret);
        return s32Ret;
    }

#if 0
  /******************************************
     step 2:  bind Vpss or vi ch to jpeg codec(ch 24)
    ******************************************/
  stVenc.enModId = MOD_ID_VENC;
    stVenc.u32DevId = 0;
    stVenc.u32ChnId = VencChn;
  if(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].bEnable)
    {
      stVpss.enModId = MOD_ID_VPSS;
      stVpss.u32DevId = VpssGrp;
      stVpss.u32ChnId = VpssChn;
      s32Ret = IPC_PF_Bind(&stVpss, &stVenc);
  }
  else
  {
    stVi.enModId = MOD_ID_VI;
      stVi.u32DevId = VpssGrp/2;
      stVi.u32ChnId = VpssChn;
      s32Ret = IPC_PF_Bind(&stVi, &stVenc);
  }
    if(s32Ret)
    {
        PRINT_ERR("Bind for jpg failed.\n");
        pthread_mutex_unlock(&g_VencMutex[VencChn]);
        return -1;
    }
#endif

    /******************************************
    step 3:  recv picture
    ******************************************/
    for(i=0; i<SnapCnt; i++)
    {
        VIDEO_FRAME_INFO_S stVideoFrame = {0};

        if((pstVpssCfg->stGrpCfg[VpssGrp].bGroup) && (pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].bEnable))
        {
            s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stVideoFrame, -1);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VPSS_GetChnFrame failed with %#x!\n", s32Ret);
                goto fail;
            }

            s32Ret = AR_MPI_VENC_SendFrame(VencChn, &stVideoFrame, -1);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VENC_SendFrame failed with %#x!\n", s32Ret);
                AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stVideoFrame);
                goto fail;
            }

            AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stVideoFrame);
        }
        else
        {
            if(VpssGrp%2 == 0)
            {
                ViChn = 0;
            }
            else
            {
                ViChn = 1;
            }

            s32Ret = AR_MPI_VI_GetChnFrame(VpssGrp/2, ViChn, &stVideoFrame, -1);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VI_GetChnFrame failed with %#x!\n", s32Ret);
                goto fail;
            }

            s32Ret = AR_MPI_VENC_SendFrame(VencChn, &stVideoFrame, -1);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VENC_SendFrame failed with %#x!\n", s32Ret);
                AR_MPI_VI_ReleaseChnFrame(VpssGrp/2, ViChn, &stVideoFrame);
                goto fail;
            }

            AR_MPI_VI_ReleaseChnFrame(VpssGrp/2, ViChn, &stVideoFrame);
        }

        s32Ret = waitOnFd(s32VencFd, 10);
        if (s32Ret < 0)
        {
            PRINT_ERR("waitOnFd timeout with %#x!\n", s32Ret);
            goto fail;
        }

        while(1)
        {
            s32Ret = AR_MPI_VENC_QueryStatus(VencChn, &stStat);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VENC_QueryStatus failed with %#x!\n", s32Ret);
                goto fail;
            }

            if (stStat.u32LeftStreamFrames > 0)
            {
                break;
            }
            usleep(20 * 1000);
        }

        if (0 == stStat.u32CurPacks)
        {
            PRINT_ERR("NOTE: Current  frame is NULL!\n");
            goto fail;
        }

        stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
        if (NULL == stStream.pstPack)
        {
            PRINT_ERR("malloc memory failed!\n");
            goto fail;
        }

        stStream.u32PackCount = stStat.u32CurPacks;
        s32Ret = AR_MPI_VENC_GetStream(VencChn, &stStream, -1);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
            free(stStream.pstPack);
            stStream.pstPack = NULL;
            goto fail;
        }

        //printf("====> stStream.pstPack[0].u64PhyAddr = %lx\n", stStream.pstPack[0].u64PhyAddr);
        char acFile[128]    = {0};
        FILE* pFile;

        snprintf(acFile, 128, "%s/snap_%d_%d_%d_%lld.jpg",pSaveDir, VpssGrp, VpssChn, i, stStream.pstPack[0].u64PTS);
        pFile = fopen(acFile, "wb");
        if (pFile == NULL)
        {
            PRINT_ERR("open file err\n");
            free(stStream.pstPack);
            stStream.pstPack = NULL;
            goto fail;
        }

        s32Ret = IPC_PF_VENC_SaveStream(pFile, &stStream);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("save snap picture failed!\n");
            free(stStream.pstPack);
            stStream.pstPack = NULL;
            fclose(pFile);
            goto fail;
        }
        fclose(pFile);

        s32Ret = AR_MPI_VENC_ReleaseStream(VencChn, &stStream);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("AR_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
            free(stStream.pstPack);
            stStream.pstPack = NULL;
            goto fail;
        }

        free(stStream.pstPack);
        stStream.pstPack = NULL;
    }

    /******************************************
     step 4:  stop recv picture
    ******************************************/
fail:

#if 0
    //unbind
    if(pstVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].bEnable)
    {
    s32Ret = IPC_PF_UnBind(&stVpss, &stVenc);
  }
  else
  {
      s32Ret = IPC_PF_UnBind(&stVi, &stVenc);
  }
    if(s32Ret)
    {
        PRINT_ERR("Unbind jpeg ch failed.\n");
        pthread_mutex_unlock(&g_VencMutex[VencChn]);
        return AR_FAILURE;
    }
#endif
    s32Ret = AR_MPI_VENC_StopRecvFrame(VencChn);
    if (s32Ret != 0)
    {
        PRINT_ERR("dev id:%d stop failed! ret=%d\n", VencChn, s32Ret);
        return s32Ret;
    }
    pthread_mutex_unlock(&g_VencMutex[VencChn]);

    return AR_SUCCESS;
}

AR_S32 IPC_PF_VI_Ctl_Start(AR_U32 u32PipelineNum, AR_U32 u32DevId, AR_U32 u32ChnId)
{
  AR_S32 s32Ret = 0;
  IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

  VI_CHN_ATTR_S      stChnAttr = {0};
  WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
  IPC_SNS_TYPE_E     enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT;
  AR_U32             u32BusId = 0;
  ENUM_VFE_MODE      enVfe = 0;

  VI_DEV  ViDev = 0;
  VI_PIPE ViPipe = 0;
    int mipiDev = 0;
  IPC_MPI_VIN_OpenDev(pVioCfg->vi_cfg.cam_mode, pVioCfg->vi_cfg.hdr_freq, pVioCfg->vi_cfg.vif_freq, pVioCfg->vi_cfg.isp_freq, pVioCfg->vi_cfg.mipi_freq);


  if(!pChnCfg->stChannel[u32PipelineNum].u32Enable)
  {
    return -1;
  }

  enWDRMode = pVioCfg->vi[u32PipelineNum].HDR_enable ? WDR_MODE_2To1_LINE : WDR_MODE_NONE;

  enSnsType = AR_VI_GetSensorType(pVioCfg->vi[u32PipelineNum].sensor, enWDRMode, pVioCfg->vi[u32PipelineNum].fps);
  if(enSnsType == IPC_SNS_TYPE_BUTT)
  {
    return -1;
  }

  //For mipi device
  if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
  {
      IPC_PF_CreateViIRGroup(enSnsType, u32DevId);
  }
  else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
  {
    ViDev = u32PipelineNum;
    ViPipe = ViDev;
    mipiDev = pVioCfg->vi[u32PipelineNum].mipi_id;

    s32Ret=AR_MPI_VI_SetMipiBindDev(ViDev, mipiDev); //this must refer to the hardware design
    if(s32Ret)
    {
      PRINT_ERR("Mipi bind dev %d/%d error!\n", ViDev, ViPipe);
      return -1;
    }

    if(0 == u32ChnId)
    {
      if(pVioCfg->vi[u32PipelineNum].channel_0_enable)
      {
        //Create VI
        u32BusId = pVioCfg->vi[u32PipelineNum].i2c_bus;
        enVfe = pVioCfg->vi[u32PipelineNum].vfe_mode;

        s32Ret = AR_VI_CreateSingleVi(ViDev, ViPipe, mipiDev, enSnsType, enWDRMode, enVfe,
        pVioCfg->vi[u32PipelineNum].power_gpio, pVioCfg->vi[u32PipelineNum].reset_gpio, NULL, pVioCfg->vi[u32PipelineNum].raw_compress_enable);
        if (AR_SUCCESS != s32Ret)
        {
          goto EXIT;
        }
        //s32Ret = AR_VI_CreateSingleIsp(ViDev, ViPipe, enSnsType, u32BusId, enWDRMode, pVioCfg->vi[u32PipelineNum].channel_0_fps);
        //if (AR_SUCCESS != s32Ret)
        //{
        //  goto EXIT;
        //}

        //Must enable ch0, ch1 is optional
        if(pVioCfg->vi[u32PipelineNum].channel_1_enable)
        {
          VI_CHN  ViChn1 = 1;

          /*start vi chn1*/
          s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr); //get attr from channel 0.
          if (AR_SUCCESS != s32Ret)
          {
            PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT;
          }
          stChnAttr.stSize.u32Width = pVioCfg->vi[u32PipelineNum].channel_1_width;
          stChnAttr.stSize.u32Height = pVioCfg->vi[u32PipelineNum].channel_1_height;
          stChnAttr.u32Depth = 2;
          stChnAttr.u32BufCount = 5;

          s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
          if (AR_SUCCESS != s32Ret)
          {
            PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT;
          }

          s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
          if (AR_SUCCESS != s32Ret)
          {
            PRINT_ERR("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT;
          }
        }
        //Must enable ch0, ch2 is optional
        if(pVioCfg->vi[u32PipelineNum].channel_2_raw_enable)
        {
          VI_CHN ViChn2 = 2;
          ISP_PUB_ATTR_S stPubAttr = {0};
          IPC_PF_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);

          /*start vi chn2*/
          s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr); //get attr from channel 0.
          if (AR_SUCCESS != s32Ret)
          {
            PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT;
          }

          stChnAttr.stSize.u32Width = stPubAttr.stSnsSize.u32Width;
          stChnAttr.stSize.u32Height = stPubAttr.stSnsSize.u32Height;
          stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
          stChnAttr.u32Depth = 2;

          s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn2, &stChnAttr);
          if (AR_SUCCESS != s32Ret)
          {
            PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT;
          }

          s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn2);
          if (AR_SUCCESS != s32Ret)
          {
            PRINT_ERR("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT;
          }
        }

        //here to load image quality setting and configure to vi/isp
        //to do
      }
    }
  }
  else //for DVP, only has 2 device, 4 pipes, each pipe has 3 channels
  {
    //DVP in, 4 camera for each tp9930->dvp ctrlor
    ViDev = u32PipelineNum / 4;
    ViPipe = u32PipelineNum;
    VI_CHN ViChn = 0;

    MIPI_DEV DvpId = IPC_PF_PIPE_2_DVP_ID(ViPipe);
    s32Ret = AR_MPI_VI_SetMipiBindDev(ViDev, DvpId);
    if(s32Ret)
    {
      PRINT_ERR("Mipi bind dev 0/0 error!\n");
      return -1;
    }

    if(0 == u32ChnId)
    {
      if(pVioCfg->vi[u32PipelineNum].channel_0_enable)
      {
        //Create VI
        u32BusId = pVioCfg->vi[u32PipelineNum].i2c_bus;
        enVfe = pVioCfg->vi[u32PipelineNum].vfe_mode;

        if(ViPipe == 0 || ViPipe == 4) //vi and isp only need once for every 4 channels
        {
          AR_VI_CreateSingleViDvp4Ch(ViDev, ViPipe, enSnsType, enWDRMode, enVfe,
          pVioCfg->vi[u32PipelineNum].power_gpio, pVioCfg->vi[u32PipelineNum].reset_gpio);
          AR_VI_CreateSingleIspDvp4Ch(ViDev, ViPipe, enSnsType, u32BusId, enWDRMode, pVioCfg->vi[u32PipelineNum].channel_0_fps);
        }
      }
    }

    if(1 == u32ChnId)
    {
      if(pVioCfg->vi[u32PipelineNum].channel_1_enable)
      {
        //sub channel
        ViChn = 1;
        IPC_PF_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
        stChnAttr.u32BufCount = 5;
        stChnAttr.stSize.u32Width = pVioCfg->vi[u32PipelineNum].channel_1_width;
        stChnAttr.stSize.u32Height = pVioCfg->vi[u32PipelineNum].channel_1_height;
        stChnAttr.stFrameRate.s32SrcFrameRate = pVioCfg->vi[u32PipelineNum].channel_1_fps;
        stChnAttr.stFrameRate.s32DstFrameRate = pVioCfg->vi[u32PipelineNum].channel_1_fps;
        /*start chn1*/
        s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
        if(AR_SUCCESS != s32Ret)
        {
          PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
          goto EXIT;
        }

        s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);
        if (AR_SUCCESS != s32Ret)
        {
          PRINT_ERR("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
          goto EXIT;
        }
      }
    }
  }

  pthread_mutex_init(&g_ViMutex[u32PipelineNum], 0);

  return 0;

EXIT:
  enSnsType = AR_VI_GetSensorType(pVioCfg->vi[u32PipelineNum].sensor, pVioCfg->vi[u32PipelineNum].HDR_enable, pVioCfg->vi[u32PipelineNum].fps);
  if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
  {
      IPC_PF_DestroyViIRGroup();
  }
  else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
  {
    ViDev = u32PipelineNum;
    ViPipe = pVioCfg->vi[u32PipelineNum].mipi_id;
  }
  else
  {
    //DVP in, 4 camera for each tp9930->dvp ctrlor
    ViDev = u32PipelineNum / 4;
    ViPipe = u32PipelineNum / 4;
  }

  if(0 == u32ChnId)
  {
    if(pVioCfg->vi[u32PipelineNum].channel_0_enable)
    {
      //AR_VI_StopSingleIsp(ViPipe, enSnsType);
      //AR_VI_StopSingleVi(ViDev, ViPipe);
      AR_MPI_VI_DisableChn(ViPipe, 0);
    }
  }

  if(1 == u32ChnId)
  {
    if(pVioCfg->vi[u32PipelineNum].channel_1_enable)
    {
      AR_MPI_VI_DisableChn(ViPipe, 1);
    }
  }

  if(2 == u32ChnId)
  {
    if(pVioCfg->vi[u32PipelineNum].channel_2_raw_enable)
    {
      AR_MPI_VI_DisableChn(ViPipe, 2);
    }
  }

  return s32Ret;
}

AR_S32 IPC_PF_VI_Ctl(AR_U32 u32PipelineNum, AR_U32 u32DevId, AR_U32 u32ChnId, AR_U32 u32IsStop)
{
  AR_S32 s32Ret = 0;
  VI_PIPE ViPipe = 0;
  IPC_SNS_TYPE_E enSnsType;
  IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

  enSnsType = AR_VI_GetSensorType(pVioCfg->vi[u32PipelineNum].sensor, pVioCfg->vi[u32PipelineNum].HDR_enable, pVioCfg->vi[u32PipelineNum].fps);

    //there are for vi for one IR camera
  if(enSnsType == GST417W_IR_400X308 || enSnsType == GST212W4_IR_256X200)
  {
      VI_IR_GROUP_S * pstViIRGrp = IPC_PF_GetViIRGroup();
      if(u32IsStop)
      {
          s32Ret = AR_MPI_VI_DisableChn(pstViIRGrp->stViSns.ViPipe, pstViIRGrp->stViSns.ViChn);
          s32Ret |= AR_MPI_VI_DisableChn(pstViIRGrp->stViThermal.ViPipe, pstViIRGrp->stViThermal.ViChn);
          s32Ret |= AR_MPI_VI_DisableChn(pstViIRGrp->stViIsp0.ViPipe, pstViIRGrp->stViIsp0.ViChn);
          if(pstViIRGrp->stViIsp1.enSnsType != IPC_SNS_TYPE_BUTT)
          {
              s32Ret |= AR_MPI_VI_DisableChn(pstViIRGrp->stViIsp1.ViPipe, pstViIRGrp->stViIsp1.ViChn);
          }
      }
      else
      {
          s32Ret = AR_MPI_VI_EnableChn(pstViIRGrp->stViSns.ViPipe, pstViIRGrp->stViSns.ViChn);
          s32Ret |= AR_MPI_VI_EnableChn(pstViIRGrp->stViThermal.ViPipe, pstViIRGrp->stViThermal.ViChn);
          s32Ret |= AR_MPI_VI_EnableChn(pstViIRGrp->stViIsp0.ViPipe, pstViIRGrp->stViIsp0.ViChn);
          if(pstViIRGrp->stViIsp1.enSnsType != IPC_SNS_TYPE_BUTT)
          {
              s32Ret |= AR_MPI_VI_EnableChn(pstViIRGrp->stViIsp1.ViPipe, pstViIRGrp->stViIsp1.ViChn);
          }
      }
  }
  else if(enSnsType != TP9930_DVP_4VC_1080P_25FPS && enSnsType != TP9930_DVP_4VC_1080P_30FPS)
  {
    ViPipe = pVioCfg->vi[u32PipelineNum].mipi_id;
  }
  else
  {
    //DVP in, 4 camera for each tp9930->dvp ctrlor
    ViPipe = u32PipelineNum;
  }

  if(0 == u32ChnId)
  {
    if(pVioCfg->vi[u32PipelineNum].channel_0_enable)
    {
      //AR_VI_StopSingleIsp(ViPipe, enSnsType);
      //AR_VI_StopSingleVi(ViDev, ViPipe);
      if(u32IsStop)
      {
        s32Ret = AR_MPI_VI_DisableChn(ViPipe, 0);
      }
      else
      {
        s32Ret = AR_MPI_VI_EnableChn(ViPipe, 0);
      }
    }
  }
  else if(1 == u32ChnId)
  {
    if(pVioCfg->vi[u32PipelineNum].channel_1_enable)
    {
      if(u32IsStop)
      {
        s32Ret = AR_MPI_VI_DisableChn(ViPipe, 1);
      }
      else
      {
        s32Ret = AR_MPI_VI_EnableChn(ViPipe, 1);
      }
    }
  }
  else if(2 == u32ChnId)
  {
    if(pVioCfg->vi[u32PipelineNum].channel_2_raw_enable)
    {
      if(u32IsStop)
      {
        s32Ret = AR_MPI_VI_DisableChn(ViPipe, 2);
      }
      else
      {
        s32Ret = AR_MPI_VI_EnableChn(ViPipe, 2);
      }
    }
  }
  else
  {
    PRINT_ERR("Wrong VI ChnId:%d\n", u32ChnId);
    s32Ret = -1;
  }

  return s32Ret;
}

AR_S32 IPC_PF_CONTROL_Process_Start(AR_U32 u32PipelineNum, IPC_MPP_CHN_S* stMppChn)
{
  AR_S32 s32Ret = -1;

  printf("======= Process_Start mode_id:%d =======\n", stMppChn->enModId);
  switch(stMppChn->enModId)
  {
    case MOD_ID_VI:
    {
      s32Ret = IPC_PF_VI_Ctl(u32PipelineNum, stMppChn->u32DevId, stMppChn->u32ChnId, 0);
      break;
    }

    case MOD_ID_VO:
    {
      AR_S32 s32Ret = AR_SUCCESS;
      VO_DEV VoDev = 0;
      VO_PUB_ATTR_S  stPubAttr = {0};
      IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

      if(!pstVioCfg->vo.enable)
      {
        break;
      }

      if(pstVioCfg->vo.square == 1)
      {
          s32Ret = AR_MPI_VO_Enable(VoDev);
          if (s32Ret != AR_SUCCESS)
          {
            PRINT_ERR("enable vo failed with %#x!\n", s32Ret);
            break;
          }

          if(strcmp(pstVioCfg->vo.type, "hdmi"))//mipi lcd
          {
            s32Ret = AR_MPI_VO_Dsi_Enable(VoDev);
            if (s32Ret != AR_SUCCESS)
            {
              PRINT_ERR("enable vo dev_%d dsi failed with %#x!\n", VoDev, s32Ret);
              break;
            }
          }

          //1. set layer
          VO_LAYER VoLayer = 0;
          s32Ret = AR_MPI_VO_EnableVideoLayer(VoLayer);
          if (s32Ret != AR_SUCCESS)
          {
            PRINT_ERR("vo enable layer failed with %#x!\n", s32Ret);
            AR_MPI_VO_Disable(VoDev);
            break;
          }

        s32Ret = AR_MPI_VO_EnableChn(VoLayer, 0);
        if (s32Ret != AR_SUCCESS)
        {
          PRINT_ERR("VO enable chn failed with %#x!\n", s32Ret);
          AR_MPI_VO_DisableVideoLayer(VoLayer);
          AR_MPI_VO_Disable(VoDev);
          break;
        }

          //3. start hdmi device
          /******************************
          * Note : do this after vo device started.
          ********************************/
          if(VO_INTF_HDMI == stPubAttr.enIntfType)
          {
            AR_VO_StartHdmi();
          }
      }
      else
      {
        AR_MPI_VO_EnableChn(0, stMppChn->u32ChnId);
      }
      s32Ret = 0;

      break;
    }

    case MOD_ID_VPSS:
    {
      s32Ret = AR_MPI_VPSS_EnableChn(stMppChn->u32DevId, stMppChn->u32ChnId);
      if (s32Ret != AR_SUCCESS)
      {
        PRINT_ERR("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
        break;
      }

      s32Ret = 0;

      break;
    }

    case MOD_ID_VENC:
    {
      IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
      int vencChn = 0;
      int vencStream = 0;

      vencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN;
      if(vencChn == stMppChn->u32ChnId)
      {
        //create snapshot venc channel
          if(pstVencCfg->vencStreamSnap.enable)
          {
            VENC_RECV_PIC_PARAM_S stParam;
            stParam.s32RecvPicNum = -1;

            s32Ret = AR_MPI_VENC_StartRecvFrame(vencChn, &stParam);
            if (s32Ret != 0)
            {
              PRINT_ERR("dev id:%d start failed! ret=%d\n", vencChn, s32Ret);
              break;
            }
          }
      }
      else
      {
          vencStream = stMppChn->u32ChnId - u32PipelineNum * IPC_MAX_VENC_STREAM_PER_CHN;
          if(!pstVencCfg->vencStream[u32PipelineNum][vencStream].enable)
          {
            break;
          }

          VENC_RECV_PIC_PARAM_S stParam;
          stParam.s32RecvPicNum = -1;
          vencChn = stMppChn->u32ChnId;

          s32Ret = AR_MPI_VENC_StartRecvFrame(vencChn, &stParam);
          if (s32Ret != 0)
          {
            PRINT_ERR("dev id:%d start failed! ret=%d\n", vencChn, s32Ret);
            break;
          }
      }

      s32Ret = 0;

      break;
    }

    case MOD_ID_VDEC:
    {
      s32Ret = 0;
      break;
    }

    default:
      PRINT_ERR("Unsupport mode id %d error.\n", stMppChn->enModId);
  }

//err:

  return s32Ret;
}

AR_S32 IPC_PF_CONTROL_Process_Stop(AR_U32 u32PipelineNum, IPC_MPP_CHN_S* stMppChn)
{
  AR_S32 s32Ret = -1;
  printf("======= Process_Stop mode_id:%d =======\n", stMppChn->enModId);

  switch(stMppChn->enModId)
  {
    case MOD_ID_VI:
    {
      s32Ret = IPC_PF_VI_Ctl(u32PipelineNum, stMppChn->u32DevId, stMppChn->u32ChnId, 1);
      break;
    }

    case MOD_ID_VO:
    {
      IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();

      if(!pstVioCfg->vo.enable)
      {
        break;
      }

      if(pstVioCfg->vo.square == 1)
      {
          if(strcmp(pstVioCfg->vo.type, "hdmi") == 0)
        {
          AR_VO_StopHdmi();
        }

        AR_MPI_VO_DisableChn(0, 0);
        AR_MPI_VO_DisableVideoLayer(0);
          AR_MPI_VO_Disable(0);
      }
      else
      {
        AR_MPI_VO_DisableChn(0, stMppChn->u32ChnId);
      }

      #if 0
      if(strcmp(pstVioCfg->vo.type, "hdmi") == 0)
      {
        AR_VO_StopHdmi();
      }

      AR_MPI_VO_DisableVideoLayer(0);
      AR_MPI_VO_Disable(0);
      #endif

      s32Ret = 0;

      break;
    }

    case MOD_ID_VPSS:
    {
      s32Ret = AR_MPI_VPSS_DisableChn(stMppChn->u32DevId, stMppChn->u32ChnId);
      if (s32Ret != AR_SUCCESS)
      {
        PRINT_ERR("failed with %#x!\n", s32Ret);
        break;
      }

      s32Ret = 0;

      break;
    }

    case MOD_ID_VENC:
    {
      int vencChn = 0;
      int vencStream = 0;
      IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

      vencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN;
      if(stMppChn->u32ChnId == vencChn)
      {
          //snapshot chn
          if(pstVencCfg->vencStreamSnap.enable)
          {
            s32Ret = AR_MPI_VENC_StopRecvFrame(vencChn);
            if (s32Ret != 0)
            {
              PRINT_ERR("dev id:%d stop failed! ret=%d\n", vencChn, s32Ret);
              break;
            }

          }
      }
      else
      {
          vencStream = stMppChn->u32ChnId - u32PipelineNum * IPC_MAX_VENC_STREAM_PER_CHN;
          if(!pstVencCfg->vencStream[u32PipelineNum][vencStream].enable)
          {
            break;
          }

          vencChn = stMppChn->u32ChnId;

          s32Ret = AR_MPI_VENC_StopRecvFrame(vencChn);
          if (s32Ret != 0)
          {
            PRINT_ERR("dev id:%d stop failed! ret=%d\n", vencChn, s32Ret);
            break;
          }
      }
      s32Ret = 0;

      break;
    }

    case MOD_ID_VDEC:
    {
      s32Ret = 0;

      break;
    }

    default:
      PRINT_ERR("Unsupport mode id %d error.\n", stMppChn->enModId);

  }

//err:

  return s32Ret;
}

AR_S32 IPC_PF_CONTROL_Start()
{

  IPC_CFG_CONTROL_S * pstCtrolCfg;
  IPC_MPP_CHN_S stSrc, stDst;
  AR_S32 i, s32Ret;

  if(CTL_STOP != s_CtrStatus)
  {
    printf("Already in start status!\n");
    return -1;
  }

  pstCtrolCfg = IPC_CFG_CONTROL_GetParam();

  //1. enable;
  for(i=0; i<IPC_MAX_CONTRL_ELE_NEEDED; ++i)
  {
    if(!pstCtrolCfg->stCtrCfg[i].bEnable)
      continue;

    stSrc.enModId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ModId;
    stSrc.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32DevId;
    stSrc.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ChnId;

    stDst.enModId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ModId;
    stDst.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32DevId;
    stDst.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ChnId;

    if(pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32Status) //src enable
    {
      s32Ret = IPC_PF_CONTROL_Process_Start(pstCtrolCfg->stCtrCfg[i].u32PipelineNum, &stSrc);
    }

    if(pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32Status) //dst enable
    {
      s32Ret = IPC_PF_CONTROL_Process_Start(pstCtrolCfg->stCtrCfg[i].u32PipelineNum, &stDst);
    }
  }

  //2. bind;
  for(i=0; i<IPC_MAX_CONTRL_ELE_NEEDED; ++i)
  {
    if(!pstCtrolCfg->stCtrCfg[i].bEnable)
      continue;

    stSrc.enModId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ModId;
    stSrc.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32DevId;
    stSrc.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ChnId;

    stDst.enModId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ModId;
    stDst.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32DevId;
    stDst.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ChnId;

    s32Ret = IPC_PF_Bind(&stSrc, &stDst);
    if(s32Ret < 0)
    {
      PRINT_ERR("IPC_PF_CONTROL_Start %d error.\n", i);
      return s32Ret;
    }
  }

  s_CtrStatus = CTL_START;

  return 0;
}

AR_S32 IPC_PF_CONTROL_Stop()
{
  IPC_CFG_CONTROL_S * pstCtrolCfg;
  IPC_MPP_CHN_S stSrc, stDst;
  AR_S32 i;
  AR_S32 s32Ret = 0;

  pstCtrolCfg = IPC_CFG_CONTROL_GetParam();

  if(CTL_START != s_CtrStatus)
  {
    PRINT_ERR("Already in stop status!\n");
    return -1;
  }

  //1. unbind;
  for(i=0; i<IPC_MAX_CONTRL_ELE_NEEDED; ++i)
  {
    if(!pstCtrolCfg->stCtrCfg[i].bEnable)
      continue;

    stSrc.enModId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ModId;
    stSrc.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32DevId;
    stSrc.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ChnId;

    stDst.enModId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ModId;
    stDst.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32DevId;
    stDst.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ChnId;

    s32Ret = IPC_PF_UnBind(&stSrc, &stDst);
    if(s32Ret < 0)
    {
      PRINT_ERR("Destroy channel %d error.\n", i);
      return s32Ret;
    }
  }

  //2. disable;
  for(i=0; i<IPC_MAX_CONTRL_ELE_NEEDED; ++i)
  {
    if(!pstCtrolCfg->stCtrCfg[i].bEnable)
      continue;

    stSrc.enModId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ModId;
    stSrc.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32DevId;
    stSrc.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32ChnId;

    stDst.enModId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ModId;
    stDst.u32DevId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32DevId;
    stDst.u32ChnId = pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32ChnId;

    if(pstCtrolCfg->stCtrCfg[i].sCtrDstEle.u32Status) //dst disable
    {
      s32Ret = IPC_PF_CONTROL_Process_Stop(pstCtrolCfg->stCtrCfg[i].u32PipelineNum, &stDst);
    }

    if(pstCtrolCfg->stCtrCfg[i].sCtrSrcEle.u32Status) //src disable
    {
      s32Ret = IPC_PF_CONTROL_Process_Stop(pstCtrolCfg->stCtrCfg[i].u32PipelineNum, &stSrc);
    }

  }

  s_CtrStatus = CTL_STOP;

  return s32Ret;
}

static AR_U32 g_u32DetectThreadRun = 0;
#define AHD_CAM_DETECT_STABLE_CNT  5 //Detect 5 times for cam to be stable.
#define AHD_NO_SIGNAL_PIC_FILE  "/usrdata/local/nosignal.yuv"
#define AHD_NO_SIGNAL_PIC_W  1920
#define AHD_NO_SIGNAL_PIC_H  1080
#define AHD_NO_SIGNAL_PIC_S  1920 //stride

static AR_S32 IPC_PF_LoadUserPic(AR_CHAR * pstrPicName, VI_USERPIC_ATTR_S * pstUsrPicAttr)
{
    AR_S32 s32Ret = 0, s32Size = 0;
    AR_U32 u32Width = AHD_NO_SIGNAL_PIC_W;
    AR_U32 u32Height = AHD_NO_SIGNAL_PIC_H;
    AR_U32 u32LumaSize = 0, u32ChrmSize = 0;
    AR_U32 u32LStride = 0, u32CStride = 0;
    AR_U64 u64PhyAddr = 0;
    AR_U8 * pVirAddr = 0;

    s32Size = AHD_NO_SIGNAL_PIC_S * AHD_NO_SIGNAL_PIC_H * 3 / 2;
    u32LStride = AHD_NO_SIGNAL_PIC_S;
    u32CStride = AHD_NO_SIGNAL_PIC_S / 2;
    u32LumaSize = u32LStride * AHD_NO_SIGNAL_PIC_H;
    u32ChrmSize = u32CStride * AHD_NO_SIGNAL_PIC_H / 2;

    //Get phy mem
    s32Ret = AR_MPI_SYS_MmzAlloc(&u64PhyAddr, &pVirAddr, "UsrPic", NULL, s32Size);
    if(s32Ret < 0)
    {
        PRINT_ERR(
"Get mmz for usrpic failed.\n");
        return -1;
    }

    pstUsrPicAttr->enUsrPicMode = VI_USERPIC_MODE_PIC;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.enModId = AR_ID_VI;

    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[0]   = u64PhyAddr;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[1]   = pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[0] + u32LumaSize;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[2]   = pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[1] + u32ChrmSize;

    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64VirAddr[0]   = (AR_U64)(AR_U64)pVirAddr;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64VirAddr[1]   = pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64VirAddr[0] + u32LumaSize;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64VirAddr[2]   = pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64VirAddr[1] + u32ChrmSize;

    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Stride[0]    = u32LStride;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Stride[1]    = u32CStride;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Stride[2]    = u32CStride;

    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Width        = u32Width;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Height       = u32Height;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat   = PIXEL_FORMAT_YVU_PLANAR_420;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.enVideoFormat   = VIDEO_FORMAT_LINEAR;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.enCompressMode  = COMPRESS_MODE_NONE;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.enField         = VIDEO_FIELD_FRAME;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.enDynamicRange  = DYNAMIC_RANGE_SDR8;
    pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.enColorGamut    = COLOR_GAMUT_BT709;

    s32Ret = IPC_MID_UTIL_ReadFile(pstrPicName, s32Size, pVirAddr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Read file %s error!\n", pstrPicName);
        AR_MPI_SYS_MmzFree(u64PhyAddr, pVirAddr);
        return -1;
    }

    return 0;
}

static AR_S32 IPC_PF_ReleaseUserPic(VI_USERPIC_ATTR_S * pstUsrPicAttr)
{
    return AR_MPI_SYS_MmzFree(pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[0], (AR_VOID *)pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64VirAddr[0]);
}

static AR_S32 IPC_PF_EnableNoSignalPic(VI_PIPE ViPipe, VI_USERPIC_ATTR_S * pstUsrPicAttr)
{
    AR_S32 s32Ret = 0;

    //PRINT_INFO("UserPic: %dx%d,luma stride %d, chrom stride %d, phy: 0x%x\n",
    //pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Width,
    //pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Height,
    //pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Stride[0],
    //pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u32Stride[1],
    //pstUsrPicAttr->unUsrPic.stUsrPicFrm.stVFrame.u64PhyAddr[0]);

    s32Ret = AR_MPI_VI_SetUserPic(ViPipe, pstUsrPicAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("Set user pic failed.\n");
        return -1;
    }

    s32Ret = AR_MPI_VI_EnableUserPic(ViPipe);
    if(s32Ret < 0)
    {
        PRINT_ERR("Enable user pic failed.\n");
        return -1;
    }

    return 0;
}
static AR_S32 IPC_PF_DisableNoSignalPic(VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;

    //PRINT_INFO("AHD %d disable usr pic.\n", ViPipe);
    s32Ret = AR_MPI_VI_DisableUserPic(ViPipe);
    return s32Ret;
}

static void* IPC_PF_THREAD_DetectCameraStatus(void *args)
{
    AR_CHAR achName[32] = {0};
    AR_S32 s32Ret = 0;
    AR_S32 i = 0;
    AR_S32 as32Cnt[IPC_MAX_PIPELINE_NUM] = {0};
    ISP_SNS_DETECT_INFO_S * pastPrevSnsInfo = NULL;
    ISP_SNS_DETECT_INFO_S * pastCurrSnsInfo = NULL;
    IPC_CFG_CHANNEL_S * pstChnCfg = IPC_CFG_CHANNEL_GetParam();
    IPC_CFG_VIO_S * pstVioCfg = IPC_CFG_VIO_GetParam();
    VI_USERPIC_ATTR_S stUsrPicAttr = {0};
    AR_BOOL bActionDone[IPC_MAX_PIPELINE_NUM] = {0};

    strcpy(achName, "videtect");
    prctl(PR_SET_NAME, achName, 0,0,0);
    s32Ret = IPC_PF_LoadUserPic(AHD_NO_SIGNAL_PIC_FILE, &stUsrPicAttr);
    if(s32Ret < 0)
    {
        return NULL;
    }

    pastCurrSnsInfo = (ISP_SNS_DETECT_INFO_S *)malloc(IPC_MAX_PIPELINE_NUM * sizeof(ISP_SNS_DETECT_INFO_S));
    if(!pastCurrSnsInfo)
    {
        PRINT_ERR("Malloc failed.\n");
        return NULL;
    }
    pastPrevSnsInfo = (ISP_SNS_DETECT_INFO_S *)malloc(IPC_MAX_PIPELINE_NUM * sizeof(ISP_SNS_DETECT_INFO_S));
    if(!pastPrevSnsInfo)
    {
        PRINT_ERR("Malloc failed.\n");
        free(pastCurrSnsInfo);
        return NULL;
    }

    while(1)
    {
        if(!g_u32DetectThreadRun)
        {
            break;
        }

        if(g_s32ViThreadSuspend)
        {
            usleep(500*1000);
            continue;
        }

        //detect all AHD pipelines for camera hotplug
        for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
        {
            if(!pstChnCfg->stChannel[i].u32Enable)
            {
                continue;
            }

            if(strcmp(pstVioCfg->vi[i].sensor, "tp9930"))
            {
                //MIPI sensor doesn't support hotplug
                continue;
            }

            s32Ret = AR_MPI_ISP_GetSnsDetectInfo(i, &pastCurrSnsInfo[i]);
            if(s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("Get sensor info failed.\n");
                continue;
            }

            if(pastCurrSnsInfo[i].bIsConnected == pastPrevSnsInfo[i].bIsConnected && bActionDone[i])
            {
                continue;
            }

            as32Cnt[i]++;

            //PRINT_INFO("AHD %d isconn: %d, cnt %d\n", i, pastCurrSnsInfo[i].bIsConnected, as32Cnt[i]);

            if(as32Cnt[i] >= AHD_CAM_DETECT_STABLE_CNT)
            {
                if(pastCurrSnsInfo[i].bIsConnected)
                {
                    PRINT_INFO("AHD %d camera is connected: %dx%d@%dfps.\n",
                        i, pastCurrSnsInfo[i].u16Width, pastCurrSnsInfo[i].u16Height, (AR_S32)pastCurrSnsInfo[i].f32Fps);
                    //Remove "no signal" pic, here we don't care if cam match with user setting, this is totally the user's logic.
                    IPC_PF_DisableNoSignalPic(i);
                    memcpy(&pastPrevSnsInfo[i], &pastCurrSnsInfo[i], sizeof(ISP_SNS_DETECT_INFO_S));
                    if(pastCurrSnsInfo[i].u16Width != pstVioCfg->vi[i].width
                      || pastCurrSnsInfo[i].u16Height !=  pstVioCfg->vi[i].height
                      || (AR_U32)pastCurrSnsInfo[i].f32Fps != pstVioCfg->vi[i].fps)
                    {
                        PRINT_ERR("Your cfg [%dx%d@%dfps] does not match real cam [%dx%d@%dfps]!\n",
                            pstVioCfg->vi[i].width, pstVioCfg->vi[i].height, pstVioCfg->vi[i].fps,
                            pastCurrSnsInfo[i].u16Width, pastCurrSnsInfo[i].u16Height, (AR_S32)pastCurrSnsInfo[i].f32Fps);
                    }
                }
                else
                {
                    PRINT_INFO("AHD %d camera is disconnected.", i);
                    IPC_PF_EnableNoSignalPic(i, &stUsrPicAttr);
                    pastPrevSnsInfo[i].bIsConnected = AR_FALSE;
                    //Any other operations can be added here
                    //E.g. stop record/trigger an alarm/light a LED or whatever
                }

                as32Cnt[i] = 0;
                bActionDone[i] = AR_TRUE;
            }
        }

        usleep(100*1000);
    }

    IPC_PF_ReleaseUserPic(&stUsrPicAttr);

    free(pastCurrSnsInfo);
    free(pastPrevSnsInfo);

    return NULL;
}
AR_S32 IPC_PF_VI_StartCameraDetect()
{
    AR_S32 s32Ret = 0;
    AR_S32 i = 0;
    AR_BOOL bNeed = AR_FALSE;
    IPC_CFG_VIO_S * pstVioCfg = NULL;

    if(g_u32DetectThreadRun)
    {
        return 0;
    }

    //if there's any AHD camera
    pstVioCfg = IPC_CFG_VIO_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(strcmp(pstVioCfg->vi[i].sensor, "tp9930") == 0)
        {
            bNeed = AR_TRUE;
            break;
        }
    }

    if(!bNeed)
    {
        return 0;
    }

    s32Ret = IPC_MID_UTIL_CreateDetachThread(IPC_PF_THREAD_DetectCameraStatus, NULL, NULL);
    if(s32Ret)
    {
        return -1;
    }

    g_u32DetectThreadRun = 1;
    return 0;
}

void IPC_PF_VI_StopCameraDetect()
{
    g_u32DetectThreadRun = 0;
}

void IPC_PF_VI_CameraDectect_Suspend()
{
	g_s32ViThreadSuspend = 1;
	printf("IPC_PF_VI_CameraDectect_Suspend\n");
}

void IPC_PF_VI_CameraDectect_Resume()
{
	g_s32ViThreadSuspend = 0;
	printf("IPC_PF_VI_CameraDectect_Resume\n");
}

void IPC_PF_SubMoudle_Suspend()
{
	AR_MPI_SCALER_Suspend();
	AR_MPI_IFC_Suspend();
	AR_MPI_GDC_Suspend();
	printf("sub moudle scaler/ifc/gdc suspend. \n");
}

void IPC_PF_SubMoudle_Resume()
{
	AR_MPI_SCALER_Resume();
	AR_MPI_IFC_Resume();
	AR_MPI_GDC_Resume();
	printf("sub moudle scaler/ifc/gdc resume. \n");
}


