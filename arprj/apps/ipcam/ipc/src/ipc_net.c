#include "util_timer.h"
#include "pf_audio.h"
#include "pf_video.h"

#include "media_fifo.h"

#include "ipc_rtsp.h"
#include "ipc_net.h"

#include "ar_g711.h"
#include "ar_rtsp_server.h"

/************************************************
 * IPC RTSP support
 * **********************************************/
static pthread_mutex_t gRtspLock = PTHREAD_MUTEX_INITIALIZER;
static RTSP_PARAM_S glbRtspParam;
static int g_u32RtspRunFlag = 0;

//static void IPC_NET_RTSP_SetDefaultParam(RTSP_PARAM_S *rtsp);

static int IPC_NET_RTSP_SetParam(RTSP_PARAM_S *rtsp_param)
{
    int i;
    int ch = 0, stream = 0;

    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

    pthread_mutex_lock(&gRtspLock);
    memset(rtsp_param, 0, sizeof(RTSP_PARAM_S));
    /////////video

	rtsp_param->video.max_ch = IPC_TOTAL_VENC_STREAM_NUM;

    for(i = 0; i < rtsp_param->video.max_ch; i ++)
    {
        ch = i / IPC_MAX_VENC_STREAM_PER_CHN;
        stream = i % IPC_MAX_VENC_STREAM_PER_CHN;

        if(VENC_ENCODE_TYPE_H264 == pstVencCfg->vencStream[ch][stream].cfg.encodeType)/*0: none, 1: H.264, 2: H265, 3: MJPEG*/
        {
            rtsp_param->video.enc_type[i] = MEDIA_CODEC_H264;
        }
        else if(VENC_ENCODE_TYPE_H265 == pstVencCfg->vencStream[ch][stream].cfg.encodeType)
        {
            rtsp_param->video.enc_type[i] = MEDIA_CODEC_H265;
        }
        else
        {
            rtsp_param->video.enc_type[i] = MEDIA_CODEC_NOT_SUPPORT;
        }

        rtsp_param->video.buffer_id[i] = i;
        rtsp_param->video.fps[i] = pstVencCfg->vencStream[ch][stream].cfg.fps;
        sprintf(rtsp_param->video.rtsp_route[i], "/ch%d/stream%d", ch, stream);
    }

    /////////audio
	#if 1 //temp disable audio
    if(pstAudioCfg->mode)
    {
        rtsp_param->audio.enable = 1;
    }

	#else
	rtsp_param->audio.enable = 0;
	#endif
    switch(pstAudioCfg->type)// 0 a-law; 1 u-law; 2-PCM
    {
        case 0:
            rtsp_param->audio.enc_type = MEDIA_CODEC_PCMA;
            break;
        case 1:
            rtsp_param->audio.enc_type = MEDIA_CODEC_PCMU;
            break;
        case 2:
           rtsp_param->audio.enc_type = MEDIA_CODEC_PCM;
            break;
        default:
            //rtsp_param->audio.enc_type = MEDIA_CODEC_NOT_SUPPORT;
            rtsp_param->audio.enc_type = MEDIA_CODEC_PCMA;
            break;
    }

    rtsp_param->audio.samplerate = pstAudioCfg->sampleRate;
    rtsp_param->audio.samplewidth = pstAudioCfg->sampleBitWidth;
    rtsp_param->audio.channle_num = pstAudioCfg->chans;
    rtsp_param->authenticate= 0;

    pthread_mutex_unlock(&gRtspLock);
    return 0;
}

static int IPC_NET_RTSP_OPS_Describe(char *path, char *query, MediaDesc *descs, int nmemb)
{
    int i, count = 0,chn = -1;
    int audio_eable = glbRtspParam.audio.enable;

	MediaDesc *desc = NULL;
    char route[128];

    for(i = 0; i < MAX_VIDEO_ENC_CHN; i ++)
    {
        memset(route, 0, sizeof(route));
        sprintf(route, "%s",glbRtspParam.video.rtsp_route[i]);
        if(strlen(path) < strlen(route))
            continue;
        if(!(strncmp(path, route, strlen(route))))
        {
            chn = i;
            break;
        }
    }
    if(chn < 0)
        return 0;

    desc = &descs[0];
    memset(desc, 0, sizeof(MediaDesc));
    desc->media = MEDIA_TYPE_VIDEO;
    desc->codec = glbRtspParam.video.enc_type[chn];
    desc->clockrate = 90000;
    desc->record = 0;
    desc->start_ms = 10000;
    desc->end_ms = 1000000000;
    desc->multicast.enable = 0;
    strncpy(desc->multicast.ipaddr, "239.100.101.102", sizeof(desc->multicast.ipaddr) - 1);
    desc->multicast.port = 6423;
    desc->multicast.ttl = 32;
    count ++;

    if(audio_eable)
    {
        desc = &descs[1];
        memset(desc, 0, sizeof(MediaDesc));
        desc->media = MEDIA_TYPE_AUDIO;
        desc->codec = glbRtspParam.audio.enc_type;//MEDIA_CODEC_PCMA;
        desc->audio_samplerate = glbRtspParam.audio.samplerate;
        desc->audio_samplewidth = glbRtspParam.audio.samplewidth;
        desc->audio_channle_num = glbRtspParam.audio.channle_num;
        //desc->clockrate = 80000;
        desc->clockrate = glbRtspParam.audio.samplerate;
        desc->record = 0;
        desc->start_ms = 10000;
        desc->end_ms = 1000000000;
        desc->multicast.enable = 1;
        strncpy(desc->multicast.ipaddr, "239.100.101.102", sizeof(desc->multicast.ipaddr) - 1);
        desc->multicast.port = 6423;
        desc->multicast.ttl = 32;
        count ++;
    }

    return count;
}

static void *IPC_NET_RTSP_OPS_Open(char *path, char *query)
{
    int i, id;
    char route[128];
	PRINT_INFO(">>>> %s.%d: ##### path = %s, query:%s\n", __func__, __LINE__, path, query);

    //reconfig params in case of users changing venc parameters
    IPC_NET_RTSP_SetParam(&glbRtspParam);
	
    for(i = 0; i < IPC_TOTAL_STREAM_NUM; i ++)
    {
        memset(route, 0, sizeof(route));
        sprintf(route, "%s",glbRtspParam.video.rtsp_route[i]);
        if(strlen(path) < strlen(route))
            continue;
        if(!(strncmp(path, route, strlen(route))))
        {
            id = i;
            break;
        }
    }

    if(IPC_TOTAL_STREAM_NUM == i)
    {
        PRINT_ERR("rtsp path error:%s", path);
        return 0;
    }

    MEDIABUF_HANDLE handle = IPC_MID_MBUF_AddReader(id);

    if(handle)
    {
        IPC_MID_MBUF_SetNewestFrame(handle);
    }

    PRINT_INFO("rtsp add reader %d, handle = %p,\n", id, handle);

	return (void *)handle;
}

static int IPC_NET_RTSP_OPS_Close(void *handle)
{
	PRINT_INFO("%s.%d: #####\n", __func__, __LINE__);

    IPC_MID_MBUF_DelReader(handle);
    return 0;
}

static int IPC_NET_RTSP_ConvertToRtspheader(frame_head_t *dst, IPC_FRAME_HEADER_S *src)
{
    dst->device_type = src->device_type;
    dst->frame_size = src->frame_size;
    dst->frame_no = src->frame_no;
	//dst->video_reso = src->video_reso;
    dst->width = src->width;
	dst->height = src->height;
    switch (src->frame_type)
    {
        case IPC_FRAME_TYPE_I:
        case IPC_FRAME_TYPE_IDR:
            dst->frame_type = RTSP_I_FRAME;
            break;
        case IPC_FRAME_TYPE_P:
            dst->frame_type = RTSP_P_FRAME;
            break;
        case IPC_FRAME_TYPE_A:
            dst->frame_type = RTSP_A_FRAME;
            break;
        default:
            break;
    }
    dst->frame_rate = src->frame_rate;
    dst->video_standard = src->video_standard;
    dst->sec = src->sec;
    dst->usec = src->usec;
    dst->pts = src->pts;

    return 0;
}

static int IPC_NET_RTSP_OPS_ReadP(void *handle, void **buf, size_t *size, int backward,void *rtsp_header)
{
    int  ret;

    IPC_FRAME_HEADER_S header = {0};

    ret = IPC_MID_MBUF_ReadFrame((MEDIABUF_HANDLE)handle, buf, (int*)size, &header);
    if (ret < 0)
        return ret;

    IPC_NET_RTSP_ConvertToRtspheader(rtsp_header, &header);

    return (*size);
}

static int IPC_NET_RTSP_OPS_Read(void *handle, void *buf, size_t size, int backward)
{
    int data_size, ret;
    char *pbuf = (char *)buf;
    frame_head_t *rtsp_header = (frame_head_t *)pbuf;
    void *data = (void *)(pbuf + sizeof(frame_head_t));
    IPC_FRAME_HEADER_S header = {0};

    if ((buf == NULL) || (handle == NULL))
    {
        PRINT_ERR("param error. %s.%d: #####\n", __func__, __LINE__);
        return -1;
    }

    data_size = size - sizeof(frame_head_t);

    ret = IPC_MID_MBUF_ReadFrame((MEDIABUF_HANDLE)handle, &data, &data_size, &header);
    if (ret < 0)
        return ret;

    IPC_NET_RTSP_ConvertToRtspheader(rtsp_header, &header);

    return (sizeof(frame_head_t) + data_size);
}

void IPC_NET_RTSP_PrintParam(RTSP_PARAM_S *param)
{
    int i;
    PRINT_INFO("authenticate:%d\n", param->authenticate);
    PRINT_INFO("video max ch:%d\n\n", param->video.max_ch);
    for(i = 0; i < MAX_VIDEO_ENC_CHN; i ++)
    {
        PRINT_INFO("video enc_type[%d]:%d\n", i, param->video.enc_type[i]);
        PRINT_INFO("video buffer_id:%d\n", param->video.buffer_id[i]);
        PRINT_INFO("video fps:%d\n", param->video.fps[i]);
        PRINT_INFO("video rtsp_route[%d]:%s\n\n", i, param->video.rtsp_route[i]);
    }
    PRINT_INFO("audio enable:%d\n", param->audio.enable);
    PRINT_INFO("audio enc_type:%d\n", param->audio.enc_type);
    PRINT_INFO("audio samplerate:%d\n", param->audio.samplerate);
    PRINT_INFO("audio samplewidth:%d\n", param->audio.samplewidth);
    PRINT_INFO("audio channle_num:%d\n\n", param->audio.channle_num);
}

#if 0
static void IPC_NET_RTSP_SetDefaultParam(RTSP_PARAM_S *rtsp)
{
    int i;

    memset(rtsp, 0, sizeof(RTSP_PARAM_S));
    rtsp->video.max_ch = MAX_VIDEO_ENC_CHN;
    for(i = 0; i < MAX_VIDEO_ENC_CHN; i ++)
    {
        rtsp->video.enc_type[i] = MEDIA_CODEC_H264;
        rtsp->video.buffer_id[i] = i;
        rtsp->video.fps[i] = 15;
        sprintf(rtsp->video.rtsp_route[i], "/stream%d", i);
    }

	rtsp->audio.enable = 1;
    rtsp->audio.enc_type = MEDIA_CODEC_PCMA;
    //rtsp->audio.enc_type = MEDIA_CODEC_RAW_PCM;
    rtsp->audio.samplerate = 8000;
    rtsp->audio.samplewidth = 16;
	rtsp->audio.channle_num = 1;

    rtsp->authenticate= 0;
}
#endif

AR_S32 IPC_NET_RTSP_Start(void)
{
    int i = 0;
    RtspOps stream_ops;

    if(!g_u32RtspRunFlag)
    {
        g_u32RtspRunFlag = 1;

        IPC_NET_RTSP_SetParam(&glbRtspParam);

    	AR_RTSP_Init();
    	AR_RTSP_Setup(1, 1);

    	memset(&stream_ops, 0, sizeof(RtspOps));

    	stream_ops.open     = IPC_NET_RTSP_OPS_Open;
    	stream_ops.close    = IPC_NET_RTSP_OPS_Close;
    	stream_ops.read     = IPC_NET_RTSP_OPS_Read;
        stream_ops.read_p   = IPC_NET_RTSP_OPS_ReadP;
    	stream_ops.describe = IPC_NET_RTSP_OPS_Describe;

        for(i = 0; i < MAX_VIDEO_ENC_CHN; i ++)
        {
            AR_RTSP_AddRoute(glbRtspParam.video.rtsp_route[i], &stream_ops);
        }

    	AR_RTSP_StartService(NULL, DFL_RTSP_PORT, DFL_HTTP_PORT);

        PRINT_INFO("+++RTSP server start+++\n");
    }
    return 0;
}

void IPC_NET_RTSP_Stop(void)
{
	if(g_u32RtspRunFlag)
	{
        g_u32RtspRunFlag = 0;
        AR_RTSP_StopService();
        AR_RTSP_UnInit();
	}
}

/************************************************
 * IPC P2P support
 * **********************************************/
AR_S32 IPC_NET_P2P_Start()
{
    return 0;
}

AR_S32 IPC_NET_P2P_Stop()
{
    return 0;
}

E_NET_STATUS IPC_NET_GetStatus()
{
    return E_NET_AP_BEGIN; //test
}

int IPC_NET_Init()
{
    return 0;
}

void IPC_NET_UnInit()
{

}


