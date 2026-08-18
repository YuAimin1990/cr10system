#include "simple_rtsp_srv.h"
#include <fcntl.h>
#include <getopt.h>
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "hal_dbglog.h"
//#include "hal_venc.h"
#include "osal.h"
#include "utils_debug.h"
#include "hal_sys.h"
#include "ar_rtsp_server.h"
#include "hal_sys.h"

#include "ar_comm_venc.h"
#include "mpi_venc.h"
#include "common_utils.h"

#define MAX_VIDEO_ENC_CHN   4
#define RTSP_QUEUE_SIZE 128

/*! Media buffer frame header */
typedef struct {
    AR_S32  eStreamType;
    AR_S32 s32EncPicByte;
    AR_S32 s32FrameIndex;
    AR_U64 u64Pts;
    AR_S32 ePicType;
} SMP_STREAM_FRAME_HEADER_S;

typedef struct smp_video_param{
    int enable;
    int max_ch;//max video channle number
    int enc_type;
    int buffer_id;//video stream buffer id default 0-2
    char rtsp_route[512];

    //video param
    int width;
    int height;
    int fps;

    //sys param
    STRU_AR_QUEUE_T *rtsp_queue;
	ar_signal_t signal;
    STRU_AR_QUEUE_T *rtsp_mem_queue;

    VENC_CHN venChn;
    VENC_CHN_ATTR_S stChnAttr;
    pthread_t recv_stream_thread;
    int recv_stream_run_flag;
    
}SMP_VIDEO_PARAM_S;

typedef struct smp_audio_patam{
    int enable;// 0 disable; 1 input; 2- input&output
    int enc_type;//audio enc type
    int samplerate;//audio samplerate
    int samplewidth;//audio samplewidth
    int channle_num;
}SMP_AUDIO_PATAM_S;

typedef struct smp_rtsp_param{
    SMP_VIDEO_PARAM_S video[MAX_VIDEO_ENC_CHN];
    SMP_AUDIO_PATAM_S audio;
    int authenticate;

    int u32RtspRunFlag;
}SMP_RTSP_PARAM_S;

static SMP_RTSP_PARAM_S g_smpRtspParam = {0};

//rtsp
static void rtsp_defaultParam(SMP_RTSP_PARAM_S *rtsp)
{
    int i;
    for(i=0;i<MAX_VIDEO_ENC_CHN;i++)
    {
        rtsp->video[i].max_ch = MAX_VIDEO_ENC_CHN;
        rtsp->video[i].enc_type = MEDIA_CODEC_H264;
        rtsp->video[i].buffer_id = i;
        //sprintf(rtsp->video[i].rtsp_route, "/stream%d", i);

        rtsp->video[i].width = 640;
        rtsp->video[i].height = 480;
        rtsp->video[i].fps = 30;
    }

	rtsp->audio.enable = 0;
    rtsp->audio.enc_type = MEDIA_CODEC_PCMA;
    rtsp->audio.samplerate = 8000;
    rtsp->audio.samplewidth = 16;
	rtsp->audio.channle_num = 1;

    rtsp->authenticate= 0;
}


static int set_rtsp_start_param(SMP_RTSP_PARAM_S *rtsp_param)
{
    //memset(rtsp_param, 0, sizeof(SMP_RTSP_PARAM_S));
	rtsp_defaultParam(rtsp_param);
    return 0;
}

static int get_chn_by_path(const char* path)
{
    int i, chn = -1;
    char route[128];

    for(i = 0; i < MAX_VIDEO_ENC_CHN; i++)
    {
        memset(route, 0, sizeof(route));
        sprintf(route, "%s",g_smpRtspParam.video[i].rtsp_route);
        if(strlen(path) < strlen(route))
            continue;
        if(!(strncmp(path, route, strlen(route))))
        {
            chn = i;
            break;
        }
    }
    return chn;
}

static int ar_rtsp_describe(char *path, char *query, MediaDesc *descs, int nmemb)
{
    int count = 0,chn = -1;
    int audio_eable = g_smpRtspParam.audio.enable;

	MediaDesc *desc = NULL;

    chn = get_chn_by_path(path);
    if(chn < 0)
        return 0;

    desc = &descs[0];
    memset(desc, 0, sizeof(MediaDesc));
    desc->media = MEDIA_TYPE_VIDEO;
    desc->codec = g_smpRtspParam.video[chn].enc_type;
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
        desc->codec = g_smpRtspParam.audio.enc_type;//MEDIA_CODEC_PCMA;
        desc->audio_samplerate = g_smpRtspParam.audio.samplerate;
        desc->audio_samplewidth = g_smpRtspParam.audio.samplewidth;
        desc->audio_channle_num = g_smpRtspParam.audio.channle_num;
        //desc->clockrate = 80000;
        desc->clockrate = g_smpRtspParam.audio.samplerate;
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

static void *rtsp_open(char *path, char *query)
{
    int chn = -1;
    chn = get_chn_by_path(path);
    if(chn < 0)
        return 0;
	else
	    return (void *)(&g_smpRtspParam.video[chn]);
}

static int rtsp_close(void *handle)
{
    return 0;
}

static int rtsp_read_p(void *handle, void **buf, size_t *size, int backward,void *rtsp_header)
{
    int  ret;
    SMP_VIDEO_PARAM_S *pVideoParam = (SMP_VIDEO_PARAM_S *)handle;
	frame_head_t *p_rtsp_header=(frame_head_t *)rtsp_header;
    void *p_bitstream=NULL;
    
	while(1)
	{
        pVideoParam->rtsp_queue->queue_pop(pVideoParam->rtsp_queue,&p_bitstream);
    	if(p_bitstream==NULL)
    	{
    	   //ar_always("wait the bit stream");
           ret = ar_signal_wait_timeout(pVideoParam->signal,50);
           if(ret!=0){
               *size = 0;
                return 0;
            }
    	}else
    	{
    	   break;
    	}
	}

	//ar_always("read the rtsp");
	SMP_STREAM_FRAME_HEADER_S *p_stream=(SMP_STREAM_FRAME_HEADER_S *)p_bitstream;

    p_rtsp_header->device_type = 0;
    p_rtsp_header->frame_size = p_stream->s32EncPicByte;
    p_rtsp_header->frame_no = p_stream->s32FrameIndex;
	//dst->video_reso = src->video_reso;
    p_rtsp_header->width = pVideoParam->width;
	p_rtsp_header->height = pVideoParam->height;

    #if 0
    if(p_stream->eStreamType==HAL_VCODEC_TYPE_HEVC || p_stream->eStreamType==HAL_VCODEC_TYPE_AVC)
    {
        switch (p_stream->ePicType)
        {
            case HAL_VCODEC_PIC_TYPE_I:
            case HAL_VCODEC_PIC_TYPE_IDR:
                p_rtsp_header->frame_type = RTSP_I_FRAME;
                break;
            case HAL_VCODEC_PIC_TYPE_P:
                p_rtsp_header->frame_type = RTSP_P_FRAME;
                break;
            default:
                break;
        }
    }
    #endif
    p_rtsp_header->frame_type = p_stream->ePicType;
    
    p_rtsp_header->frame_rate = pVideoParam->fps;
    p_rtsp_header->video_standard = 0;
    p_rtsp_header->sec = 0;
    p_rtsp_header->usec = p_stream->u64Pts;
    p_rtsp_header->pts = p_stream->u64Pts;

	if(*buf){
       memcpy(*buf,p_bitstream+sizeof(SMP_STREAM_FRAME_HEADER_S),p_stream->s32EncPicByte);
	   *size=p_stream->s32EncPicByte;
	   free(p_bitstream);
	}else
	{
		*size=p_stream->s32EncPicByte;
		*buf=p_bitstream+sizeof(SMP_STREAM_FRAME_HEADER_S);
		pVideoParam->rtsp_mem_queue->queue_insert(pVideoParam->rtsp_mem_queue,p_bitstream);
		if(pVideoParam->rtsp_mem_queue->get_queue_size(pVideoParam->rtsp_mem_queue)>=8)
		{
		      void *mem=NULL;
              pVideoParam->rtsp_mem_queue->queue_pop(pVideoParam->rtsp_mem_queue,&mem);
			  if(mem)
			  {
			     free(mem);
			  }
		}
	}
    return (*size);
}

static int rtsp_read(void *handle, void *buf, size_t size, int backward)
{
    return 0;
}

static void printf_rtsp_param(SMP_RTSP_PARAM_S *param,int chn)
{
    printf("authenticate:%d\n", param->authenticate);
    printf("video max ch:%d\n\n", param->video[chn].max_ch);
    printf("video enc_type:%d\n",param->video[chn].enc_type);
    printf("video buffer_id:%d\n", param->video[chn].buffer_id);
    printf("video width:%d\n", param->video[chn].width);
    printf("video height:%d\n", param->video[chn].height);
    printf("video fps:%d\n", param->video[chn].fps);
    printf("video rtsp_route:%s\n\n",param->video[chn].rtsp_route);
    printf("audio enable:%d\n", param->audio.enable);
    printf("audio enc_type:%d\n", param->audio.enc_type);
    printf("audio samplerate:%d\n", param->audio.samplerate);
    printf("audio samplewidth:%d\n", param->audio.samplewidth);
    printf("audio channle_num:%d\n\n", param->audio.channle_num);
}

static void updateVideoParamByAttr(SMP_VIDEO_PARAM_S *pVideoParam)
{
    if(pVideoParam->stChnAttr.stVencAttr.enType == PT_H265)
    {
        pVideoParam->enc_type = MEDIA_CODEC_H265;
        pVideoParam->fps = pVideoParam->stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate;
    }
    else if(pVideoParam->stChnAttr.stVencAttr.enType == PT_H264)
    {
        pVideoParam->enc_type = MEDIA_CODEC_H264;
        pVideoParam->fps = pVideoParam->stChnAttr.stRcAttr.stH264Cbr.u32SrcFrameRate;
    }
    else{
        printf("unsupport vect type %d\n",pVideoParam->stChnAttr.stVencAttr.enType);
    }
    pVideoParam->width = pVideoParam->stChnAttr.stVencAttr.u32PicWidth;
    pVideoParam->height = pVideoParam->stChnAttr.stVencAttr.u32PicHeight;
}

static void *rcvEncStream(void *arg);

int smp_rtsp_enable_venc_chn(int index, VENC_CHN chn, const char *route)
{
    if(index>=MAX_VIDEO_ENC_CHN)
        return -1;
    g_smpRtspParam.video[index].enable = 1;
    g_smpRtspParam.video[index].venChn = chn;
    strcpy(g_smpRtspParam.video[index].rtsp_route,route);
    return 0;
}

AR_S32 smp_rtsp_start(void)
{
    int i = 0, ret;
    RtspOps stream_ops;

    if(!g_smpRtspParam.u32RtspRunFlag)
    {
        g_smpRtspParam.u32RtspRunFlag = 1;

        set_rtsp_start_param(&g_smpRtspParam);

    	AR_RTSP_Init();
    	AR_RTSP_Setup(1, 1);

    	memset(&stream_ops, 0, sizeof(RtspOps));

    	stream_ops.open     = rtsp_open;
    	stream_ops.close    = rtsp_close;
    	stream_ops.read     = rtsp_read;
        stream_ops.read_p   = rtsp_read_p;
    	stream_ops.describe = ar_rtsp_describe;

        for(i = 0; i < MAX_VIDEO_ENC_CHN; i++)
        {
            if(!g_smpRtspParam.video[i].enable)
                continue;

            memset(&(g_smpRtspParam.video[i].stChnAttr),0,sizeof(VENC_ATTR_S));
            ret = AR_MPI_VENC_GetChnAttr(g_smpRtspParam.video[i].venChn,&(g_smpRtspParam.video[i].stChnAttr));
            if(ret!=0){
                printf("smp_rtsp_start: get chan %d attr failed\n",g_smpRtspParam.video[i].venChn);
                continue;
            }
            updateVideoParamByAttr(&(g_smpRtspParam.video[i]));
            
            AR_RTSP_AddRoute(g_smpRtspParam.video[i].rtsp_route, &stream_ops);
            g_smpRtspParam.video[i].rtsp_queue=ar_hal_sys_creat_queue(RTSP_QUEUE_SIZE,"rtsp_queue");
		    g_smpRtspParam.video[i].signal=ar_create_signal();
            g_smpRtspParam.video[i].rtsp_mem_queue=ar_hal_sys_creat_queue(16,"rtsp_mem_queue");
            printf_rtsp_param(&g_smpRtspParam,i);

            g_smpRtspParam.video[i].recv_stream_run_flag = 1;
            ret = pthread_create(&g_smpRtspParam.video[i].recv_stream_thread, NULL, rcvEncStream, &(g_smpRtspParam.video[i]));
        }

    	AR_RTSP_StartService(NULL, 554, 8081);

        printf("+++RTSP server start+++\n");
    }
    return 0;
}

void smp_rtsp_stop(void)
{
    int i;
    g_smpRtspParam.u32RtspRunFlag = 0;
    AR_RTSP_StopService();
	AR_RTSP_UnInit();
    for(i = 0; i < MAX_VIDEO_ENC_CHN; i++)
    {
        if(!g_smpRtspParam.video[i].enable)
            continue;
        g_smpRtspParam.video[i].recv_stream_run_flag = 0;
        void* retval;
        pthread_join(g_smpRtspParam.video[i].recv_stream_thread,&retval);
        while(g_smpRtspParam.video[i].rtsp_queue->get_queue_size(g_smpRtspParam.video[i].rtsp_queue)>0)
        {
            void *mem=NULL;
            g_smpRtspParam.video[i].rtsp_queue->queue_pop(g_smpRtspParam.video[i].rtsp_queue,&mem);
            if(mem!=NULL){
                free(mem);
            }
        }
        ar_hal_sys_delete_queue(&g_smpRtspParam.video[i].rtsp_queue);
	    ar_delete_signal(g_smpRtspParam.video[i].signal);
    }
}

static void push_stream_data(SMP_VIDEO_PARAM_S *pVideoParam, VENC_STREAM_S *pstStream)
{
    int i;
    if(pVideoParam->rtsp_queue->get_queue_size(pVideoParam->rtsp_queue)>=(RTSP_QUEUE_SIZE-8))
    {
        int packCount = pstStream->u32PackCount;
        while(packCount--)
        {
            void *mem=NULL;
            pVideoParam->rtsp_queue->queue_pop(pVideoParam->rtsp_queue,&mem);
            if(mem!=NULL){
                free(mem);
            }
        }
    }

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        void* pDataAddr = pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset;
        AR_U32 u32DataLen = pstStream->pstPack[i].u32Len;
        
        void *p_bit_stream=malloc(sizeof(SMP_STREAM_FRAME_HEADER_S)+u32DataLen);
    	
        SMP_STREAM_FRAME_HEADER_S * pstStreamInfo = (SMP_STREAM_FRAME_HEADER_S *)p_bit_stream;
        pstStreamInfo->eStreamType = pVideoParam->enc_type;
        pstStreamInfo->s32EncPicByte = u32DataLen;
        pstStreamInfo->s32FrameIndex = pstStream->u32Seq;
        pstStreamInfo->u64Pts = pstStream->pstPack[i].u64PTS;
        
        pstStreamInfo->ePicType = RTSP_P_FRAME;

        if(pVideoParam->enc_type == MEDIA_CODEC_H265)
        {
            if (pstStream->pstPack->DataType.enH265EType == H265E_NALU_ISLICE ||
                    pstStream->pstPack->DataType.enH265EType == H265E_NALU_IDRSLICE)
            {
                pstStreamInfo->ePicType = RTSP_I_FRAME;
            } else if (pstStream->pstPack->DataType.enH265EType == H265E_NALU_PSLICE) {
                pstStreamInfo->ePicType = RTSP_P_FRAME;
            }
        }
        else if(pVideoParam->enc_type == MEDIA_CODEC_H264)
        {
            if (pstStream->pstPack->DataType.enH264EType == H264E_NALU_ISLICE ||
                    pstStream->pstPack->DataType.enH264EType == H264E_NALU_IDRSLICE)
            {
                pstStreamInfo->ePicType = RTSP_I_FRAME;
            } else if (pstStream->pstPack->DataType.enH264EType == H264E_NALU_PSLICE) {
                pstStreamInfo->ePicType = RTSP_P_FRAME;
            }
        }
    	memcpy((p_bit_stream+sizeof(SMP_STREAM_FRAME_HEADER_S)), pDataAddr, u32DataLen);
    	pVideoParam->rtsp_queue->queue_insert(pVideoParam->rtsp_queue,p_bit_stream);
    	//ar_signal(pVideoParam->signal);
    }
    
	ar_signal(pVideoParam->signal);

}

static void *rcvEncStream(void *arg)
{
	AR_S32 s32Ret;
	VENC_CHN_STATUS_S stStatus;
	VENC_STREAM_S stStream;
    SMP_VIDEO_PARAM_S *pVideoParam = (SMP_VIDEO_PARAM_S*)arg;
    int id = pVideoParam->venChn;

	while (pVideoParam->recv_stream_run_flag) {
		while (1) {
			s32Ret = AR_MPI_VENC_QueryStatus(id, &stStatus);
			if (s32Ret) {
				printf("dev id:%d query status failed! ret=%d\n", id, s32Ret);
				goto out;
			}
			if (stStatus.u32LeftStreamFrames > 0) {
				//printf("status left stream frames:%d\n", stStatus.u32LeftStreamFrames);
				break;
			}
			usleep(10 * 1000);
		}

		stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
		if (NULL == stStream.pstPack) {
			printf("malloc memory failed!\n");
			goto out;
		}
		stStream.u32PackCount = stStatus.u32CurPacks;
		s32Ret = AR_MPI_VENC_GetStream(id, &stStream, -1);
		if (AR_SUCCESS != s32Ret)
		{
			printf("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

			free(stStream.pstPack);
			stStream.pstPack = NULL;
			goto out;
		}

        push_stream_data(pVideoParam, &stStream);
		
		/*******************************************************
		 release stream
		 *******************************************************/
		s32Ret = AR_MPI_VENC_ReleaseStream(id, &stStream);
		if (AR_SUCCESS != s32Ret)
		{
			printf("AR_MPI_VENC_ReleaseStream failed!\n");
			free(stStream.pstPack);
			stStream.pstPack = NULL;
			goto out;
		}

		/*******************************************************
		 free pack nodes
		*******************************************************/
		free(stStream.pstPack);
		stStream.pstPack = NULL;
    }

out:
	return NULL;
}


