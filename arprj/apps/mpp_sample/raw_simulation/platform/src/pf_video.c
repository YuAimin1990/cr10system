#include <sys/prctl.h>

#include "pf_video.h"
#include "pf_mbuf.h"
#include "pf_sensor.h"
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

#include "hal_sys.h"
#include "hal_gpio.h"
#include "util_timer.h"
#include "util_profile.h"

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
static pthread_mutex_t g_VpssMutex[IPC_MAX_VPSS_GRP_NEEDED * IPC_MAX_STREAM_PER_PIPE];
static pthread_mutex_t g_VencMutex[32];//we use 32 venc chn
static IPC_CTL_STATUS_E s_CtrStatus = 0;

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

    //basic pipeline, all stream from vi main ch
    return 0;//pVioCfg->vi[u32Chn].channel_0_fps;
}

//here we simply check if 8 ch are all enabled, if so, we set high perf to encode above 4k@60fps.
//users can judge if they need this or not.
static void IPC_PF_VENC_SetHighPerformance()
{
    AR_S32 i = 0, s32Ret = 0;
    AR_U32 u32Cnt = 0, u32ViHas4K = 0;
    VENC_PARAM_MOD_S stParam = {0};
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        u32Cnt++;

        //check if any vi set to 4K@60fps
        //if(pVioCfg->vi[i].channel_0_enable && pVioCfg->vi[i].width >= 3840 && pVioCfg->vi[i].height >= 2160
        // && pVioCfg->vi[i].fps >= 60)
       if(pVioCfg->vi[i].stChnAttr.stSize.u32Width >= 3840 && pVioCfg->vi[i].stChnAttr.stSize.u32Height >= 2160)
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

		//set core=600M, bpu=450M, cmdqueue=2 for ext board
        stParam.stH265eModParam.u32CoreClock = 600;
        stParam.stH265eModParam.u32BpuClock = 600;

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

    stParam.enVencModType = MODTYPE_H265E; //whatever 264/265, they share the clk
    s32Ret = AR_MPI_VENC_GetModParam(&stParam);
    if(s32Ret)
    {
        PRINT_ERR("Get venc mod param error! ret = %x\n", s32Ret);
        return;
    }

    stParam.stH265eModParam.u32CoreClock = u32FrequencyCore;
    stParam.stH265eModParam.u32BpuClock = u32FrequencyBpu;

    s32Ret = AR_MPI_VENC_SetModParam(&stParam);
    if(s32Ret)
    {
        PRINT_ERR("Set venc mod param error! ret = %x\n", s32Ret);
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
            AR_U32 u32EncType = IPC_PF_GetEncType(pstVencCfg->vencStream[i][j].cfg.encodeType);
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
    stParam.stEventModParam.u32VencTaskQueueSize  = encoder_cnt * 10;
    stParam.stEventModParam.u32VencDoneQueueSize  = u32VencDoneQueueSize;
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

static AR_S32 waitOnFd(AR_S32 devFd, AR_U32 timeout/*in seconds*/) 
{
    if (devFd < 0) 
    {
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

    if (retval == -1) 
    {
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
    switch (u32CodecType) 
    {
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

AR_S32 IPC_PF_VENC_GetStream(AR_U32 u32Id, IPC_VENC_STREAM_S *pstVideoStream)
{
    AR_S32 s32Ret;
    VENC_CHN_STATUS_S stStatus;
    VENC_STREAM_S  stStream = {0};
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    AR_U32 u32Ch = u32Id / IPC_MAX_VENC_STREAM_PER_CHN;
    AR_U32 u32Stream = u32Id % IPC_MAX_VENC_STREAM_PER_CHN;
    AR_S32 devFd = AR_MPI_VENC_GetFd(u32Id);
    AR_S32 timeout = 1;

    while (1)
    {

#ifdef THERMAL_ENABLE
#else
		if(!ar_hal_sys_mpp_service_is_lib())
	        waitOnFd(devFd, timeout);
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

    memcpy(pstVideoStream->u64AddrVirt, stStream.pstPack[0].pu8Addr, stStream.pstPack[0].u32Len);
    for(int i = 1; i < stStream.u32PackCount; i++)
    {
        memcpy(pstVideoStream->u64AddrVirt + stStream.pstPack[i-1].u32Len, stStream.pstPack[i].pu8Addr, stStream.pstPack[i].u32Len);
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

AR_S32 IPC_PF_VENC_ReleaseStream(AR_U32        u32Id, IPC_VENC_STREAM_S *pstVideoStream)
{
    if(pstVideoStream->u64AddrVirt)
    {
        free(pstVideoStream->u64AddrVirt);
    }

    return 0;
}

/************ static functions ***********************/
static void* IPC_PF_THREAD_ReadVStream(void *args)
{
	AR_U32 u32Id = *(AR_U32 *)args;
	AR_CHAR achName[32] = {0};
	IPC_VENC_STREAM_S stVencStream = {0};
	IPC_FRAME_HEADER_S	stFrameHeader;
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

		//get stream
		s32Ret = IPC_PF_VENC_GetStream(u32Id, &stVencStream);
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

    //FIXME

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
    if(strcmp(name, "raw_simu") == 0)
    {
        return ISP_RAW_SIMULATION;
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

/* Create a basic channel pipeline */
/*                              
*     isp -----------ch0---------------->venc-->mbuf-main
*                  |-ch1----> sub    |-->VO
*                  |-ch2----> raw
*/
AR_S32 IPC_PF_Create_BasicChannel(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVenc, stVo;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    AR_S32 s32Ret = 0;
    int i = u32Index;

    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();

    //create a basic pipeline
    stVI.enModId = MOD_ID_VI;
    stVI.u32DevId = i;
    stVI.u32ChnId = 0;

    //no vpss, just bind vi and venc
    if(pstVencCfg->vencStream[0][i].enable)
    {
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

    return 0;
}

AR_S32 IPC_PF_Destroy_BasicChannel(AR_U32 u32Index)
{
    IPC_MPP_CHN_S stVI, stVenc, stVo;
    IPC_CFG_VIO_S * pstVioCfg;
    IPC_CFG_VENC_S * pstVencCfg;
    AR_S32 s32Ret = 0;
    int i = 0;

    pstVioCfg = IPC_CFG_VIO_GetParam();
    pstVencCfg = IPC_CFG_VENC_GetParam();

    stVI.enModId = MOD_ID_VI;
    stVI.u32DevId = i;
    stVI.u32ChnId = 0;

    //no vpss, just unbind vi and venc
    if(pstVencCfg->vencStream[0][i].enable)
    {
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

    return 0;
}

AR_S32 IPC_PF_Create_Channel()
{
    ProfileStart(__FUNCTION__);
    
    IPC_CFG_VIO_S * pstVioCfg;
    AR_S32 s32Ret = 0;
    int i = 0;
    
    pstVioCfg = IPC_CFG_VIO_GetParam();
    
    //how many pipelines are there, are they basic or full pipeline?
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        s32Ret = IPC_PF_Create_BasicChannel(i);
        if(s32Ret)
        {
            PRINT_ERR("Create MIPI basic pipeline %d failed.\n", i);
            return s32Ret;
        }
    }
    
    s_CtrStatus = CTL_START;
    
    ProfileEnd(__FUNCTION__);
    
    return 0;
}

AR_S32 IPC_PF_Destroy_Channel()
{
    int i = 0;
    
    //how many pipelines are there, are they basic or full pipeline?
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        IPC_PF_Destroy_BasicChannel(i);
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

AR_S32 IPC_PF_SYS_Init()
{
	AR_S32 i = 0;
	AR_S32 s32Ret = 0;
	VB_CONFIG_S stVbConf = {0};
	ISP_PUB_ATTR_S stPubAttr = {0};
	WDR_MODE_E enWDRMode = WDR_MODE_NONE;
	IPC_SNS_TYPE_E enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT; //ACTUALLY 25 fps on FPGA
	IPC_VB_POOL_CNT_S stCh0VbPoolCnt = {0}, stCh1VbPoolCnt = {0}, stRawVbPoolCnt = {0};

	IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
	AR_U32 u32MaxBlk = pVioCfg->vi_cfg.vb_blk_cnt;
	AR_MPI_SYS_Exit();
	AR_MPI_VB_Exit();

	//93 ISP 420 format, align 512, 8bit, VI
	for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
	{
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, enWDRMode, pVioCfg->vi[i].fps);
                
        if(pVioCfg->vi[i].channel_1_enable)
        {
            IPC_PF_VB_PoolCnt(pVioCfg->vi[i].channel_1_width, pVioCfg->vi[i].channel_1_height, &stCh1VbPoolCnt);
        }

        //vpss and vi share this, for osd copy
        //if(pVioCfg->vi[i].channel_0_enable)
        {
            IPC_PF_VB_PoolCnt(pVioCfg->vi[i].stChnAttr.stSize.u32Width, pVioCfg->vi[i].stChnAttr.stSize.u32Height, &stCh0VbPoolCnt);
        }

        if(pVioCfg->vi[i].channel_2_raw_enable)
        {
            enWDRMode = WDR_MODE_NONE;
            //enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, enWDRMode, pVioCfg->vi[i].fps);
            IPC_PF_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);

            IPC_PF_VB_PoolCnt(stPubAttr.stSnsSize.u32Width, stPubAttr.stSnsSize.u32Height, &stRawVbPoolCnt);
        }
    }
    
    //VI alloc vb pool for ch0,ch1,raw data ch
    IPC_PF_VB_ViPoolCnt(&stVbConf, &stCh1VbPoolCnt, u32MaxBlk, 0);//ch1

    IPC_PF_VB_ViPoolCnt(&stVbConf, &stCh0VbPoolCnt, u32MaxBlk, 0);//ch0
    IPC_PF_VB_ViPoolCnt(&stVbConf, &stRawVbPoolCnt, u32MaxBlk, 1);//raw data if enable

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
	IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

	VI_CHN_ATTR_S      stChnAttr = {0};
	WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
	IPC_SNS_TYPE_E     enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT;
	AR_U32             u32BusId = 0;
    ENUM_VFE_MODE      enVfe = 0;

    VI_DEV  ViDev = 0;
    VI_PIPE ViPipe = 0;
    int mipiDev = 0;
    AR_U32 u32MaxBlk = pVioCfg->vi_cfg.vb_blk_cnt;

	IPC_MPI_VIN_OpenDev(pVioCfg->vi_cfg.cam_mode, pVioCfg->vi_cfg.hdr_freq, pVioCfg->vi_cfg.vif_freq, pVioCfg->vi_cfg.isp_freq, pVioCfg->vi_cfg.mipi_freq);

	for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        enWDRMode =  WDR_MODE_NONE;
        
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, enWDRMode, pVioCfg->vi[i].fps);
        if(enSnsType == IPC_SNS_TYPE_BUTT)
        {
            return -1;
        }

        //For mipi device
        ViDev = i;
        ViPipe = ViDev;
        mipiDev = pVioCfg->vi[i].mipi_id;

        s32Ret=AR_MPI_VI_SetMipiBindDev(ViDev, mipiDev); //this must refer to the hardware design
        if(s32Ret)
        {
            PRINT_ERR("Mipi bind dev %d/%d error!\n", ViDev, mipiDev);
            return -1;
        }
        
        //Create VI
        s32Ret = AR_VI_CreateSingleVi(ViDev, ViPipe, mipiDev, enSnsType, enWDRMode, enVfe,
            NULL, NULL, NULL, 0);
        if (AR_SUCCESS != s32Ret)
        {
            goto EXIT;
        }
        
        //if(pVioCfg->vi[i].channel_0_enable)
        {
            IPC_PF_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
            stChnAttr.u32Depth = 1;
            stChnAttr.u32BufCount = u32MaxBlk;
            for(int cn = 0; cn < 8; cn++)
            {
                stChnAttr.u32DepthClient[cn] = 1;
            }

            s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, 0, &stChnAttr);
            if (s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("AR_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
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
                PRINT_ERR("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
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
        if(pVioCfg->vi[i].channel_2_raw_enable)
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
            stChnAttr.u32Depth = 1;
            stChnAttr.u32BufCount = u32MaxBlk;
            for(int cn = 0; cn < 8; cn++)
            {
                stChnAttr.u32DepthClient[cn] = 1;
            }

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
        s32Ret = AR_VI_CreateSingleIsp(ViDev, ViPipe, enSnsType, u32BusId, enWDRMode, pVioCfg->vi[i].fps);
        if (AR_SUCCESS != s32Ret)
        {
            goto EXIT;
        }

        pthread_mutex_init(&g_ViMutex[i], 0);

	}
	ProfileEnd(__FUNCTION__);

	return 0;

EXIT:
	for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
	{
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor,0, pVioCfg->vi[i].fps);
        
        ViDev = i;
        ViPipe = pVioCfg->vi[i].mipi_id;

        //if(pVioCfg->vi[i].channel_0_enable)
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

	for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
	{
        enSnsType = AR_VI_GetSensorType(pVioCfg->vi[i].sensor, 0, pVioCfg->vi[i].fps);
        
        ViDev = i;
        ViPipe = ViDev;//pVioCfg->vi[i].mipi_id;
        
        if(pVioCfg->vi[i].channel_2_raw_enable)
        {
            AR_MPI_VI_DisableChn(ViPipe, 2);
        }
        if(pVioCfg->vi[i].channel_1_enable)
        {
            AR_MPI_VI_DisableChn(ViPipe, 1);
        }
        //if(pVioCfg->vi[i].channel_0_enable)
        {
            AR_MPI_VI_DisableChn(ViPipe, 0);
        }

        AR_VI_StopSingleIsp(ViPipe, enSnsType);

        AR_VI_StopSingleVi(ViDev, ViPipe);
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

	//pstVioCfg->vi[ViPipe].flip = bFlip;
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

	//pstVioCfg->vi[ViPipe].mirror = bMirror;
	s32Ret = IPC_CFG_VIO_Save();
	pthread_mutex_unlock(&g_ViMutex[ViPipe]);

	return s32Ret;
}

AR_S32 IPC_PF_VI_SetAntiFlicker(VI_PIPE ViPipe, AR_U32 u32Freq)
{
	AR_S32 s32Ret = 0;
	ISP_BANDING_STATE_TIDY_ATTR_S stBanding;

	stBanding.u32BandingState = u32Freq;
	pthread_mutex_lock(&g_ViMutex[ViPipe]);

	s32Ret = AR_MPI_ISP_SetBandingStateTidyAttr(ViPipe, &stBanding);
	if(s32Ret < 0)
	{
		pthread_mutex_unlock(&g_ViMutex[ViPipe]);
		PRINT_ERR("Set antiflicker error.\n");
		return s32Ret;
	}

	pthread_mutex_unlock(&g_ViMutex[ViPipe]);

	return s32Ret;
}

AR_S32 IPC_PF_VI_SetDenoise3D(VI_PIPE ViPipe, AR_U32 u32Strength)
{
    AR_S32 s32Ret = 0;
    ISP_DE3D_STRENGTH_TIDY_ATTR_S stStrength = {0};

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    stStrength.u32De3dStrength = u32Strength;
    s32Ret = AR_MPI_ISP_SetDe3dStrengthTidyAttr(ViPipe, &stStrength);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Set denoise error.\n");
        return s32Ret;
    }

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

    pthread_mutex_lock(&g_ViMutex[ViPipe]);

    stStrength.stDe2dStrength.strength = u32Strength;
    s32Ret = AR_MPI_ISP_SetDe2dStrengthTidyAttr(ViPipe, &stStrength);
    if(s32Ret < 0)
    {
        pthread_mutex_unlock(&g_ViMutex[ViPipe]);
        PRINT_ERR("Set denoise error.\n");
        return s32Ret;
    }

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
	//pstVioCfg->vi[ViPipe].HDR_enable = u32OnOff;

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

	stHue.u32Hue = u32Hue;
	pthread_mutex_lock(&g_ViMutex[ViPipe]);

	s32Ret = AR_MPI_ISP_SetHueTidyAttr(ViPipe, &stHue);
	if(s32Ret < 0)
	{
		pthread_mutex_unlock(&g_ViMutex[ViPipe]);
		PRINT_ERR("Set hue error.\n");
		return s32Ret;
	}

	pthread_mutex_unlock(&g_ViMutex[ViPipe]);

	return s32Ret;
}

AR_S32 IPC_PF_VI_SetBrightness(VI_PIPE ViPipe, AR_U32 u32Brightness)
{
	AR_S32 s32Ret = 0;
	ISP_BRIGHTNESS_TIDY_ATTR_S stBrightness;

	stBrightness.u32Brightness = u32Brightness;
	pthread_mutex_lock(&g_ViMutex[ViPipe]);
	s32Ret = AR_MPI_ISP_SetBrightnessTidyAttr(ViPipe, &stBrightness);
	if(s32Ret < 0)
	{
		pthread_mutex_unlock(&g_ViMutex[ViPipe]);
		PRINT_ERR("Set brightness error.\n");
		return s32Ret;
	}

	pthread_mutex_unlock(&g_ViMutex[ViPipe]);

	return s32Ret;

}

AR_S32 IPC_PF_VI_SetSatueration(VI_PIPE ViPipe, AR_U32 u32Satueration)
{
	AR_S32 s32Ret = 0;
	ISP_SATURATION_TIDY_ATTR_S stSatueration;

	stSatueration.u32Saturation = u32Satueration;

	pthread_mutex_lock(&g_ViMutex[ViPipe]);
	s32Ret = AR_MPI_ISP_SetSaturationTidyAttr(ViPipe, &stSatueration);
	if(s32Ret < 0)
	{
		pthread_mutex_unlock(&g_ViMutex[ViPipe]);
		PRINT_ERR("Set satueration error.\n");
		return s32Ret;
	}

	pthread_mutex_unlock(&g_ViMutex[ViPipe]);

	return s32Ret;
}

AR_S32 IPC_PF_VI_SetContrast(VI_PIPE ViPipe, AR_U32 u32Contrast)
{
	AR_S32 s32Ret = 0;
	ISP_CONTRAST_TIDY_ATTR_S stContrast;

	stContrast.u32Contrast = u32Contrast;

	pthread_mutex_lock(&g_ViMutex[ViPipe]);
	s32Ret = AR_MPI_ISP_SetContrastTidyAttr(ViPipe, &stContrast);
	if(s32Ret < 0)
	{
		pthread_mutex_unlock(&g_ViMutex[ViPipe]);
		PRINT_ERR("Set contrast error.\n");
		return s32Ret;
	}

	pthread_mutex_unlock(&g_ViMutex[ViPipe]);

	return s32Ret;
}

AR_S32 IPC_PF_VI_SetSharpness(VI_PIPE ViPipe, AR_U32 u32Sharpness)
{
	AR_S32 s32Ret = 0;
	ISP_SHARPNESS_TIDY_ATTR_S stSharpness;

	stSharpness.u32Sharpness = u32Sharpness;

	pthread_mutex_lock(&g_ViMutex[ViPipe]);
	s32Ret = AR_MPI_ISP_SetSharpnessTidyAttr(ViPipe, &stSharpness);
	if(s32Ret < 0)
	{
		pthread_mutex_unlock(&g_ViMutex[ViPipe]);
		PRINT_ERR("Set contrast error.\n");
		return s32Ret;
	}

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
    else
    {
        PRINT_ERR("Raw simulation only supports HDMI as VO\n");
        return -1;
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
    
    stLayerAttr.u32DispFrmRt = pstVioCfg->vo.fps;
    stLayerCsc.enCscMatrix = VO_CSC_MATRIX_BT601_TO_RGB_PC;
    s32Ret = AR_MPI_VO_SetVideoLayerCSC(VoLayer, &stLayerCsc);
    if (s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("Set video layer csc failed with %#x!\n", s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
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

    for(i = 0; i < IPC_MAX_VO_WINDOW_NUM; i++)
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
    ProfileEnd(__FUNCTION__);

    return 0;
}
AR_S32 IPC_PF_VPSS_UnInit()
{
    return 0;
}
AR_S32 IPC_PF_VPSS_SetRotate(AR_U32 u32GrpId, VPSS_ROTATE_S * pstRotate)
{
    return 0;
}

AR_S32 IPC_PF_VPSS_SetFlip(AR_U32 u32GrpId, AR_S32 u32ChnId, AR_U32 u32FlipEnable)
{
	AR_S32 s32Ret = 0;
	VPSS_GRP_ATTR_S stGrpAttr = {0};
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
	VPSS_GRP_ATTR_S stGrpAttr = {0};
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
    return 0;
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
    	s32Ret = AR_MPI_VENC_StopRecvFrame(vencChn);
        if (s32Ret != 0)
        {
            PRINT_ERR("dev id:%d stop failed! ret=%d\n", vencChn, s32Ret);
            return s32Ret;
        }

        s32Ret = AR_MPI_VENC_DestroyChn(vencChn);
        if (s32Ret)
        {
            PRINT_ERR("create channel failed\n");
            return s32Ret;
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
        enWDRMode = WDR_MODE_NONE;
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
            stChnAttr.stVencAttr.u32BufSize = 2*1024*1024;//base on max bitrate
            stChnAttr.stVencAttr.bByFrame = AR_TRUE;
            stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            if(stChnAttr.stVencAttr.enType == PT_H264)
            {
                 if(enSnsType == TP9930_DVP_4VC_1080P_25FPS || enSnsType == TP9930_DVP_4VC_1080P_30FPS)
                 {
                     stChnAttr.stVencAttr.stAttrH264e.u32CmdQueueDepth = 3;
                 }
                 else
                 {
                     stChnAttr.stVencAttr.stAttrH264e.u32CmdQueueDepth = 2;
                 }
            }
            else if(stChnAttr.stVencAttr.enType == PT_H265)
            {
                if(enSnsType == TP9930_DVP_4VC_1080P_25FPS || enSnsType == TP9930_DVP_4VC_1080P_30FPS)
                {
                    stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 3;
                }
                else
                {
                    stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;
                }
            }
            else if(stChnAttr.stVencAttr.enType == PT_JPEG)
            {
                stChnAttr.stVencAttr.u32BufSize = stChnAttr.stVencAttr.u32PicWidth * stChnAttr.stVencAttr.u32PicHeight;//base on pic size
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
                    stChnAttr.stVencAttr.u32BufSize = stChnAttr.stRcAttr.stH264Cbr.u32BitRate * 1000 * 5 /*stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate*/ / 8;
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
        stChnAttr.stVencAttr.u32BufSize = 1024 * 1024;//pstVencCfg->vencStreamSnap.cfg.width * pstVencCfg->vencStreamSnap.cfg.height;
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
        pthread_mutex_init(&g_VencMutex[vencChn], 0);
    }

	ProfileEnd(__FUNCTION__);

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
    VENC_RC_PARAM_S stRcParam = {0};

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
    struct timeval TimeoutVal;
    fd_set read_fds;
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
    
    /******************************************
    step 1:  Cfg venc ch attr (ch 24)
    ******************************************/
    VencChn = IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN; //ch 24 for jpg
    u32Stream = (VpssGrp% 2 == 0 ? 0 : (VpssChn == 0 ? 1:2));
    pthread_mutex_lock(&g_VencMutex[VencChn]);
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
    s32VencFd = AR_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        PRINT_ERR("AR_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        goto fail;
    }
    
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

        if(!ar_hal_sys_mpp_service_is_lib())
        {
            FD_ZERO(&read_fds);
            FD_SET(s32VencFd, &read_fds);
            TimeoutVal.tv_sec  = 10;
            TimeoutVal.tv_usec = 0;
            s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
            if (s32Ret <= 0)
            {
                PRINT_ERR("snap select failed!\n");
                goto fail;
            }
            
            if (!FD_ISSET(s32VencFd, &read_fds))
            {
                PRINT_ERR("snap fd set check failed!\n");
                goto fail;
            }
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
    pthread_mutex_unlock(&g_VencMutex[VencChn]);

    return AR_SUCCESS;
}

AR_S32 IPC_PF_VI_Ctl_Start(AR_U32 u32PipelineNum, AR_U32 u32DevId, AR_U32 u32ChnId)
{
    pthread_mutex_init(&g_ViMutex[u32PipelineNum], 0);    
    return 0;
}

AR_S32 IPC_PF_VI_Ctl(AR_U32 u32PipelineNum, AR_U32 u32DevId, AR_U32 u32ChnId, AR_U32 u32IsStop)
{
    return 0;
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
			AR_S32 i = 0;
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
    s_CtrStatus = CTL_START;
    return 0;
}

AR_S32 IPC_PF_CONTROL_Stop()
{
    s_CtrStatus = CTL_STOP;
    return 0;
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
        PRINT_ERR("Get mmz for usrpic failed.\n");
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


