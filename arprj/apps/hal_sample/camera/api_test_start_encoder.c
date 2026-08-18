#include <fcntl.h>
#include <getopt.h>
#include "stdio.h"
#include "stdlib.h"
#include "hal_dbglog.h"
#include "hal_venc.h"
#include "osal.h"
#include "utils_debug.h"
#include "hal_sys.h"
#include"camera.h"
#include "ar_rtsp_server.h"
//#include "rtspServ.h"
#include "hal_sys.h"

#if defined LOG_TAG
#undef      LOG_TAG
#define     LOG_TAG "test_venc"
#endif
#define ROUNDUP(x, y) (((x) + ((y) - 1)) & ~((y) - 1))

#define MAX_VIDEO_ENC_CHN   2
#define RTSP_QUEUE_SIZE 128
typedef struct video_param{
    int max_ch;//max video channle number
    int enc_type;
    int buffer_id;//video stream buffer id default 0-2
    int fps;
    char rtsp_route[512];
}VIDEO_PARAM_S;
typedef struct audio_patam{
    int enable;// 0 disable; 1 input; 2- input&output
    int enc_type;//audio enc type
    int samplerate;//audio samplerate
    int samplewidth;//audio samplewidth
    int channle_num;
}AUDIO_PATAM_S;
typedef struct rtsp_param{
    VIDEO_PARAM_S video;
    AUDIO_PATAM_S audio;
    int authenticate;
}RTSP_PARAM_S;



typedef struct
{
	AR_S32 dev_created;
	AR_S32 inst_created;
	AR_S32 dev_opened;
	AR_S32 dev_started;
	AR_S32 dev_id;
	uint8_t* bitstream_va_addr;
	uint8_t* bitstream_pa_addr;
	AR_VOID* bitstream_viraddr;
	AR_U64 bitstream_phyaddr;
	int bitstream_size;
	ar_os_thread_id_t bitstream_thread;
	int bitstream_thread_run;
	STRU_STREAM_BUFFER_INIFO_T stream_buffer_look_up[16];
	int  stream_id;
	int  client_id;
	int  fd_stream;
	int  pipe_id;
    int  w;
	int  h;
	int  fps;

	int  codec_id;
	int  codec_type;
	int  u32RtspRunFlag;
	RTSP_PARAM_S glbRtspParam;

	//queue to pass bitstream to rtsp
    STRU_AR_QUEUE_T *rtsp_queue;
	ar_signal_t signal;
    STRU_AR_QUEUE_T *rtsp_mem_queue;
}STRU_CAEMRA_RECODER_T;

STRU_CAEMRA_RECODER_T *g_p_recoder=NULL;

static STRU_CAEMRA_RECODER_T *get_camera_recoder()
{
   if(g_p_recoder==NULL)
   {
      g_p_recoder=malloc(sizeof(STRU_CAEMRA_RECODER_T));
	  memset(g_p_recoder,0,sizeof(STRU_CAEMRA_RECODER_T));
	  g_p_recoder->bitstream_size=5 * 1024 * 1024;
   }
   return g_p_recoder;
}

static AR_S32 set_param_venc(STRU_VENC_INST_ATTR *pInstAttr, AR_S32 id, ENUM_VCODEC_TYPE type, AR_S32 width, AR_S32 height, AR_S32 bitrate, AR_S32 gopsize) {
    AR_S32 ret = 0;
	  STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();

    pInstAttr->stVencAttr.eEncType = type;
    pInstAttr->stVencAttr.eformat = HAL_VCODEC_FORMAT_420;
    pInstAttr->stVencAttr.s32PicWidth = width;
    pInstAttr->stVencAttr.s32PicHeight = height;
    pInstAttr->stVencAttr.s32profile = 0;
    pInstAttr->stVencAttr.stH26xAttr.u32CmdQueueDepth = 2;

    pInstAttr->stRcAttr.s32BitRate = bitrate;
    pInstAttr->stRcAttr.s32VbvBufferSize = 1024;
    pInstAttr->stRcAttr.s32FPSNum = p_recoder->fps;
    pInstAttr->stRcAttr.s32FPSDen = 1;
    pInstAttr->stRcAttr.s32GopSize = gopsize;

    pInstAttr->stGopAttr.eGopType = GOP_IDX_IPP_SINGLE;
    return ret;
}

static AR_S32 set_bitrate_param_venc(AR_S32 id, AR_S32 bitrate) {
    AR_S32 ret = 0;
    STRU_VENC_INST_ATTR inst_attr;
    ret = ar_hal_venc_get_attr(id, &inst_attr);
    inst_attr.stRcAttr.s32BitRate = bitrate;
    ret = ar_hal_venc_set_attr(id, &inst_attr);
    return ret;
}

static AR_S32 set_gopsize_param_venc(AR_S32 id, AR_S32 gopsize) {
    AR_S32 ret = 0;
    STRU_VENC_INST_ATTR inst_attr;
    ret = ar_hal_venc_get_attr(id, &inst_attr);
    inst_attr.stRcAttr.s32GopSize = gopsize;
    ret = ar_hal_venc_set_attr(id, &inst_attr);
    return ret;
}

static AR_S32 set_fps_param_venc(AR_S32 id, AR_S32 num, AR_S32 den) {
    AR_S32 ret = 0;
    STRU_VENC_INST_ATTR inst_attr;
    ret = ar_hal_venc_get_attr(id, &inst_attr);
    inst_attr.stRcAttr.s32FPSNum = num;
    inst_attr.stRcAttr.s32FPSDen = den;
    ret = ar_hal_venc_set_attr(id, &inst_attr);
    return ret;
}

static AR_S32 set_vbv_param_venc(AR_S32 id, AR_S32 vbvBufferSize) {
    AR_S32 ret = 0;
    STRU_VENC_INST_ATTR inst_attr;
    ret = ar_hal_venc_get_attr(id, &inst_attr);
    inst_attr.stRcAttr.s32VbvBufferSize = vbvBufferSize;
    ret = ar_hal_venc_set_attr(id, &inst_attr);
    return ret;
}

static AR_S32 set_intra_qp_offset_param_venc(AR_S32 id, AR_S32 qpoffset) {
    AR_S32 ret = 0;
    STRU_VENC_INST_ATTR inst_attr;
    ret = ar_hal_venc_get_attr(id, &inst_attr);
    inst_attr.stGopAttr.s32IntraQpOffset = qpoffset;
    ret = ar_hal_venc_set_attr(id, &inst_attr);
    return ret;
}

static AR_S32 init_venc(AR_S32 id, AR_S32 num, ENUM_VCODEC_TYPE type, AR_S32 width, AR_S32 height, AR_S32 bitrate, AR_S32 gopsize) {
    AR_S32 ret = 0;
    STRU_VENC_INST_ATTR inst_attr = { 0 };
	  STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    do {
        ret = ar_hal_venc_create_dev(id);
        if (ret != 0) {
            log_tag_err("dev id:%d create failed! ret=%d", id, ret);
//            break;
        } else {
            log_tag_info("+++++dev id:%d create success!++++", id);
            p_recoder->dev_created = 1;
        }
        ret = set_param_venc(&inst_attr, id, type, width, height, bitrate, gopsize);
        if (ret != 0) {
            log_tag_err("dev id:%d set param failed! ret=%d", id, ret);
            break;
        } else {
            log_tag_info("+++++dev id%d set param success!++++", id);
        }
        ret = ar_hal_venc_create_inst(id, &inst_attr);
        if (ret != 0) {
            log_tag_err("dev id:%d create inst failed! ret=%d", id, ret);
            break;
        } else {
            log_tag_info("+++++dev id:%d create inst success!++++", id);
            p_recoder->inst_created = 1;
        }

        p_recoder->dev_id = ar_hal_venc_dev_open(id);
        ret = (p_recoder->dev_id <= 0) ? -1 : 0;
        if (ret != 0) {
            log_tag_err("dev id:%d open failed! ret=%d", id, ret);
            break;
        } else {
            log_tag_info("+++++dev id:%d open success!++++", id);
            p_recoder->dev_opened = 1;
        }

        STRU_VENC_PIC_PARAM param;
        param.s32RecvPicNum = num;
        ret = ar_hal_venc_start(id, &param);
        if (ret != 0) {
            log_tag_err("dev id:%d start failed! ret=%d", id, ret);
            break;
        } else {
            log_tag_info("+++++dev id:%d start success!++++", id);
            p_recoder->dev_started = 1;
        }
        //malloc bitstream buffer
        ar_hal_sys_mmz_alloc(&p_recoder->bitstream_phyaddr, &p_recoder->bitstream_viraddr, "bitstream_buffer", NULL, p_recoder->bitstream_size + 32);
        log_tag_info("bitstream virt addr:%p, phy addr:%lld, size:%d", p_recoder->bitstream_viraddr, p_recoder->bitstream_phyaddr, p_recoder->bitstream_size);
        p_recoder->bitstream_va_addr = (uint8_t*)ROUNDUP((uint64_t)p_recoder->bitstream_viraddr, 32);
        p_recoder->bitstream_pa_addr = (uint8_t*)ROUNDUP((uint64_t)p_recoder->bitstream_phyaddr, 32);
        if (p_recoder->bitstream_va_addr && p_recoder->bitstream_pa_addr)
           log_tag_info("alloc bitstream buffer! va:%p, pa:%p, size:%d", p_recoder->bitstream_va_addr, p_recoder->bitstream_pa_addr, p_recoder->bitstream_size);
        else {
           log_tag_err("alloc bitstream buffer failed!");
           return -1;
        }
    } while (0);
    return ret;
}

static AR_S32 reset_venc(AR_S32 id, AR_S32 num) {

	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    log_tag_info("\n\n\n---------------RESET---------------\n\n\n");
    AR_S32 ret = 0;
    ret = ar_hal_venc_stop(id);
    if (ret != 0) {
        log_tag_err("dev id:%d stop failed! ret=%d", id, ret);
    } else {
        log_tag_info("++++++dev id:%d stop success!------", id);
    }

    ret = ar_hal_venc_reset(id);
    if (ret != 0) {
        log_tag_err("dev id:%d reset failed! ret=%d", id, ret);
    } else {
        log_tag_info("++++++dev id:%d reset success!------", id);
    }

    STRU_VENC_PIC_PARAM param;
    param.s32RecvPicNum = num;
    ret = ar_hal_venc_start(id, &param);
    if (ret != 0) {
        log_tag_err("dev id:%d start failed! ret=%d", id, ret);
        p_recoder->dev_started = 0;
    } else {
        log_tag_info("+++++dev id:%d start success!++++", id);
        p_recoder->dev_started = 1;
    }
    return ret;
}

static AR_S32 uninit_venc(AR_S32 id) {

	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    AR_S32 ret = 0;
    if (p_recoder->dev_opened) {
        ret = ar_hal_venc_dev_close(id);
        if (ret != 0) {
            log_tag_err("dev id:%d destroy inst failed! ret=%d", id, ret);
        } else {
            log_tag_info("++++++dev id:%d close success!------", id);
        }
    }
    if (p_recoder->dev_started) {
        ret = ar_hal_venc_stop(id);
        if (ret != 0) {
            log_tag_err("dev id:%d stop failed! ret=%d", id, ret);
        } else {
            log_tag_info("++++++dev id:%d stop success!------", id);
        }
    }

    if (p_recoder->inst_created) {
        ret = ar_hal_venc_destory_inst(id);
        if (ret != 0) {
            log_tag_err("dev id:%d destroy inst failed! ret=%d", id, ret);
        } else {
            log_tag_info("++++++dev id:%d destroy inst success!------", id);
        }
    }

    if (p_recoder->dev_created) {
        ret = ar_hal_venc_destory_dev(id);
        if (ret != 0) {
            log_tag_err("dev id:%d destroy device failed! ret=%d", id, ret);
        } else {
            log_tag_info("++++++dev id:%d destroy device success!------", id);
        }
    }
	log_tag_info("release yuv & bitstream memory");
    if (p_recoder->bitstream_viraddr && p_recoder->bitstream_phyaddr) {
        ar_hal_sys_mmz_free(p_recoder->bitstream_phyaddr, p_recoder->bitstream_viraddr);
    }
    return ret;
}

static AR_S32 send_stream_buffer_to_venc(AR_S32 id,STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer)
{
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();

    AR_S32 ret = 0;
    STRU_VIDEO_FRAME_INFO frame;
    frame.u64AddrBs = (AR_U64)p_recoder->bitstream_pa_addr;
    frame.u64VirAddrBs = (AR_U64)p_recoder->bitstream_va_addr;
    frame.u32BsSize = p_recoder->bitstream_size;
    frame.u64AddrY = (AR_U64)p_stream_buffer->pannel[0].buffer_phy_addr;
    frame.u64AddrCb = (AR_U64)p_stream_buffer->pannel[1].buffer_phy_addr;
    frame.u64AddrCr = (AR_U64)p_stream_buffer->pannel[2].buffer_phy_addr;
    frame.u64OffsetTblAddrY = 0;
    frame.u64OffsetTblAddrCb = 0;
    frame.u64OffsetTblAddrCr = 0;
    frame.s32CbCrInterleave = 0;
    frame.u64Pts = p_stream_buffer->timestamps;
    frame.s32Endian = 0;
    frame.s32FrameIndex = p_stream_buffer->buffer_index;
    frame.eMapType = HAL_VCODEC_LINEAR_FRAME_MAP;
    frame.s32Stride = p_stream_buffer->pannel[0].line_buffer_len;
    frame.eFormat = HAL_VCODEC_FORMAT_420;
    frame.s32SrcEndFlag = 0;

	ret = ar_hal_venc_send_frame(id, &frame);
    if (ret != 0) {
        log_tag_err("send frame index:%d failed! ret=%d",ret);
    }
    return ret;
}

static void *bit_stream_loop_thread(void* arg)
{
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
	STRU_VENC_STATUS status;
    int ret=0;
	int codec_id=p_recoder->codec_id;
	while(p_recoder->bitstream_thread_run)
	{
    	while(1) {
    		ret = ar_hal_venc_query_status(codec_id, &status);
    		if (ret != 0) {
    			//log_tag_err("dev codec_id:%d query status failed! ret=%d", codec_id, ret);
    		}
    		if (status.u32LeftStreamFrames > 0) {
    			//log_tag_info("status left stream frames:%d", status.u32LeftStreamFrames);
    			break;
    		}
    		usleep(10 * 1000);
    	}
    	for (int i = 0;i < status.u32LeftStreamFrames; i++) {
    		STRU_VENC_STREAM stream;
    		ret = ar_hal_venc_get_stream(codec_id, &stream);
    		if (ret != 0) {
    			log_tag_err("dev codec_id:%d get stream failed! ret=%d", codec_id, ret);
    			break;
    		}
    		// ENUM_PIC_TYPE type = stream.stH26xStreamInfo.ePicType;
    		int offset = (uint8_t*)stream.u64BitstreamBuffers[0] - p_recoder->bitstream_pa_addr;

            if(p_recoder->rtsp_queue->get_queue_size(p_recoder->rtsp_queue)<=(RTSP_QUEUE_SIZE-8))
            {
				void *p_bit_stream=malloc(sizeof(STRU_VENC_STREAM)+stream.s32EncPicBytes[0]);
				*((STRU_VENC_STREAM *)p_bit_stream)=stream;
				memcpy((p_bit_stream+sizeof(STRU_VENC_STREAM)), p_recoder->bitstream_va_addr + offset, stream.s32EncPicBytes[0]);
				p_recoder->rtsp_queue->queue_insert(p_recoder->rtsp_queue,p_bit_stream);
				ar_signal(p_recoder->signal);
            }

			//send to net
			if(stream.s32FrameIndex>=0)
			{
			    ar_hal_vin_return_stream_buffer(p_recoder->fd_stream,p_recoder->pipe_id,p_recoder->stream_id,&p_recoder->stream_buffer_look_up[stream.s32FrameIndex]);
			}else
			{
			     ar_err("err encoder index %d",stream.s32FrameIndex);
			}
			#if 0
    		log_tag_info("\n++++write bitstream to file, current frame count:%d, addr:%p, size:%d, type:%d\n", status.u32LeftStreamFrames, p_recoder->bitstream_va_addr + offset, stream.s32EncPicByte, type);
    		log_tag_info("query status info: CalcBitRate:%d, LeftEncPics:%d, LeftPicsInQeueu:%d, LeftRecvPics:%d, LeftStreamBytes:%d, LeftStreamFrames:%d\n",
    		status.u32CalcBitRate,
    		status.u32LeftEncPics,
    		status.u32LeftPicsInQeueu,
    		status.u32LeftRecvPics,
    		status.u32LeftStreamBytes,
    		status.u32LeftStreamFrames);
			#endif
    	}
	}
	ar_osal_thread_exit();
	return 0;
}

//rtsp
static void rtsp_defaultParam(RTSP_PARAM_S *rtsp)
{
    memset(rtsp, 0, sizeof(RTSP_PARAM_S));
    rtsp->video.max_ch = MAX_VIDEO_ENC_CHN;
    rtsp->video.enc_type= MEDIA_CODEC_H264;
    rtsp->video.buffer_id= 0;
    rtsp->video.fps= 30;
    sprintf(rtsp->video.rtsp_route, "/stream%d", 0);

	rtsp->audio.enable = 0;
    rtsp->audio.enc_type = MEDIA_CODEC_PCMA;
    //rtsp->audio.enc_type = MEDIA_CODEC_RAW_PCM;
    rtsp->audio.samplerate = 8000;
    rtsp->audio.samplewidth = 16;
	rtsp->audio.channle_num = 1;

    rtsp->authenticate= 0;
}


static int set_rtsp_start_param(RTSP_PARAM_S *rtsp_param)
{
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    memset(rtsp_param, 0, sizeof(RTSP_PARAM_S));
	rtsp_defaultParam(rtsp_param);
	rtsp_param->video.max_ch = MAX_VIDEO_ENC_CHN;

    if(HAL_VCODEC_TYPE_AVC == p_recoder->codec_type)/*0: none, 1: H.264, 2: H265, 3: MJPEG*/
    {
        rtsp_param->video.enc_type= MEDIA_CODEC_H264;
    }
    else if(HAL_VCODEC_TYPE_HEVC == p_recoder->codec_type)
    {
        rtsp_param->video.enc_type= MEDIA_CODEC_H265;
    }
    else
    {
        rtsp_param->video.enc_type= MEDIA_CODEC_NOT_SUPPORT;
    }
    rtsp_param->video.buffer_id = 0;
    rtsp_param->video.fps= 30;
    sprintf(rtsp_param->video.rtsp_route, "/ch%d/stream%d", p_recoder->pipe_id, p_recoder->stream_id);
    return 0;
}

static int ar_rtsp_describe(char *path, char *query, MediaDesc *descs, int nmemb)
{
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    int /*i,*/ count = 0/*,chn = -1*/;
    int audio_eable = p_recoder->glbRtspParam.audio.enable;

	MediaDesc *desc = NULL;

    desc = &descs[0];
    memset(desc, 0, sizeof(MediaDesc));
    desc->media = MEDIA_TYPE_VIDEO;
    desc->codec = p_recoder->glbRtspParam.video.enc_type;
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
        desc->codec = p_recoder->glbRtspParam.audio.enc_type;//MEDIA_CODEC_PCMA;
        desc->audio_samplerate = p_recoder->glbRtspParam.audio.samplerate;
        desc->audio_samplewidth = p_recoder->glbRtspParam.audio.samplewidth;
        desc->audio_channle_num = p_recoder->glbRtspParam.audio.channle_num;
        //desc->clockrate = 80000;
        desc->clockrate = p_recoder->glbRtspParam.audio.samplerate;
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
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
	return (void *)p_recoder;
}

static int rtsp_close(void *handle)
{
    return 0;
}

static int rtsp_read_p(void *handle, void **buf, size_t *size, int backward,void *rtsp_header)
{
    // int  ret;
	frame_head_t *p_rtsp_header=(frame_head_t *)rtsp_header;
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    void *p_bitstream=NULL;
	while(1)
	{
        p_recoder->rtsp_queue->queue_pop(p_recoder->rtsp_queue,&p_bitstream);
    	if(p_bitstream==NULL)
    	{
    	   //ar_always("wait the bit stream");
           ar_signal_wait(p_recoder->signal);
    	}else
    	{
    	   break;
    	}
	}

	//ar_always("read the rtsp");
	STRU_VENC_STREAM *p_stream=(STRU_VENC_STREAM *)p_bitstream;

    p_rtsp_header->device_type = 0;
    p_rtsp_header->frame_size = p_stream->s32EncPicBytes[0];
    p_rtsp_header->frame_no = p_stream->s32FrameIndex;
	//dst->video_reso = src->video_reso;
    p_rtsp_header->width = p_recoder->w;
	p_rtsp_header->height = p_recoder->h;


    if(p_stream->eStreamType==HAL_VCODEC_TYPE_HEVC || p_stream->eStreamType==HAL_VCODEC_TYPE_AVC)
    {
        switch (p_stream->stH26xStreamInfo.ePicType)
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
    p_rtsp_header->frame_rate = p_recoder->fps;
    p_rtsp_header->video_standard = 0;
    p_rtsp_header->sec = 0;
    p_rtsp_header->usec = p_stream->u64Pts;
    p_rtsp_header->pts = p_stream->u64Pts;

	if(*buf){
       memcpy(*buf,p_bitstream+sizeof(STRU_VENC_STREAM),p_stream->s32EncPicBytes[0]);
	   *size=p_stream->s32EncPicBytes[0];
	   free(p_bitstream);
	}else
	{
		*size=p_stream->s32EncPicBytes[0];
		*buf=p_bitstream+sizeof(STRU_VENC_STREAM);
		p_recoder->rtsp_mem_queue->queue_insert(p_recoder->rtsp_mem_queue,p_bitstream);
		if(p_recoder->rtsp_mem_queue->get_queue_size(p_recoder->rtsp_mem_queue)>=8)
		{
		      void *mem=NULL;
              p_recoder->rtsp_mem_queue->queue_pop(p_recoder->rtsp_mem_queue,&mem);
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

void printf_rtsp_param(RTSP_PARAM_S *param)
{
    printf("authenticate:%d\n", param->authenticate);
    printf("video max ch:%d\n\n", param->video.max_ch);
    printf("video enc_type:%d\n",param->video.enc_type);
    printf("video buffer_id:%d\n", param->video.buffer_id);
    printf("video fps:%d\n", param->video.fps);
    printf("video rtsp_route:%s\n\n",param->video.rtsp_route);
    printf("audio enable:%d\n", param->audio.enable);
    printf("audio enc_type:%d\n", param->audio.enc_type);
    printf("audio samplerate:%d\n", param->audio.samplerate);
    printf("audio samplewidth:%d\n", param->audio.samplewidth);
    printf("audio channle_num:%d\n\n", param->audio.channle_num);
}
static AR_S32 rtsp_start(void)
{
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    // int i = 0;
    RtspOps stream_ops;

    if(!p_recoder->u32RtspRunFlag)
    {
        p_recoder->u32RtspRunFlag = 1;

        set_rtsp_start_param(&p_recoder->glbRtspParam);

    	AR_RTSP_Init();
    	AR_RTSP_Setup(1, 1);

    	memset(&stream_ops, 0, sizeof(RtspOps));

    	stream_ops.open     = rtsp_open;
    	stream_ops.close    = rtsp_close;
    	stream_ops.read     = rtsp_read;
        stream_ops.read_p   = rtsp_read_p;
    	stream_ops.describe = ar_rtsp_describe;

        AR_RTSP_AddRoute(p_recoder->glbRtspParam.video.rtsp_route, &stream_ops);

    	AR_RTSP_StartService(NULL, 554, 8081);

		p_recoder->rtsp_queue=ar_creat_queue(RTSP_QUEUE_SIZE,"rtsp_queue");
		p_recoder->signal=ar_create_signal();


		p_recoder->rtsp_mem_queue=ar_creat_queue(16,"rtsp_queue");

		printf_rtsp_param(&p_recoder->glbRtspParam);

        printf("+++RTSP server start+++\n");
    }
    return 0;
}

static void rtsp_stop(void)
{
	STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();
    p_recoder->u32RtspRunFlag = 0;
    AR_RTSP_StopService();
	AR_RTSP_UnInit();
	ar_delete_queue(&p_recoder->rtsp_queue);
	ar_delete_signal(p_recoder->signal);
}

static int api_test_start_recoder(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   STRU_CAEMRA_RECODER_T *p_recoder=get_camera_recoder();

   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 AR_S32  dev_num=0;
		 int ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
		 ar_always("dev_num=%d",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
			dump_dev_info(p_info);
		 }
		 free(p_info);
         int dev_id=0;
		 index=camera_find_cmd_pra_index_by_name("-dev_id",p_cmd);
		 if(index>0)
		 {
			 dev_id=atoi(p_cmd->args[index+1]);
		 }
		 AR_S32 bitrate = 4096;
		 index=camera_find_cmd_pra_index_by_name("-bitrate",p_cmd);
		 if(index>0)
		 {
			 bitrate=atoi(p_cmd->args[index+1]);
		 }
		 AR_S32 gopsize = 60;
		 index=camera_find_cmd_pra_index_by_name("-gopsize",p_cmd);
		 if(index>0)
		 {
			 gopsize=atoi(p_cmd->args[index+1]);
		 }
		 int opend=0;
		 ret=ar_hal_vin_dev_is_opend(vin_fd,dev_id,&opend);
		 if(ret<0 || opend)
		 {
			ar_always("the device have started ret=%d opend=%d",ret,opend);
			if(opend)
			{
			   int pipe_id=0;
			   ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(vin_fd,dev_id,&pipe_id);
			   ar_always("dev id =%d pipe_id=%d",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0)
			   {
				  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				  if(fd_pipe>=0)
				  {
					  int num=0;
					  int stream_id[8];
                      ar_hal_vin_get_all_stream(fd_pipe,pipe_id,stream_id,&num);
					  ar_always("stream num %d",num);
					  index=camera_find_cmd_pra_index_by_name("-stream_id",p_cmd);
					  if(index>=0){
					  	  //to dump stream buffer
					  	  int stream_id=atoi(p_cmd->args[index+1]);
						  int num=1;
						  index=camera_find_cmd_pra_index_by_name("-num",p_cmd);
						  if(index>=0)
						  {
						     num=atoi(p_cmd->args[index+1]);
						  }

						  //start a thread to receive bitstream
						  ar_os_thread_attr_t ctrl_attr = {0};
						  ctrl_attr.name = "stream";
						  ctrl_attr.stack_size = 16 * 1024;
						  p_recoder->bitstream_thread_run=1;
						  p_recoder->bitstream_thread=ar_osal_thread_new(bit_stream_loop_thread, p_recoder, &ctrl_attr);

						  //open the stream fd
						  int fd_stream=ar_hal_vin_open_stream_dev(pipe_id,stream_id);
						  int client_id=0;
						  ar_hal_vin_create_stream_client(fd_pipe,pipe_id,stream_id,&client_id);
						  int loop_num=num+1;
						  p_recoder->stream_id=stream_id;
						  p_recoder->client_id=client_id;
						  p_recoder->fd_stream=fd_stream;
						  p_recoder->pipe_id=pipe_id;
						  ar_always("dump_stream %d client id=%d fd_stream=%d pipe_id=%d",stream_id,client_id,fd_stream,pipe_id);

						  STRU_AR_STREAM_PROPERTY_T prop;
						  ar_hal_vin_get_stream_property(fd_pipe,pipe_id,stream_id,&prop);
                          //init the encoder
						  AR_S32 codec_id = 0;
						  ENUM_VCODEC_TYPE type = HAL_VCODEC_TYPE_HEVC;
						  p_recoder->codec_id=codec_id;
						  p_recoder->codec_type=type;
						  p_recoder->w=prop.total_w;
						  p_recoder->h=prop.total_h;
						  p_recoder->fps=prop.fps;

                          ar_always("p_recoder->fps=%d",p_recoder->fps);

						  init_venc(codec_id, -1, type, prop.total_w, prop.total_h, bitrate, gopsize);
						  rtsp_start();

						  while(loop_num>0)
						  {
						       STRU_STREAM_BUFFER_INIFO_T stream_buffer;
                               ret=ar_hal_vin_get_stream_buffer(fd_stream,pipe_id,stream_id,client_id,&stream_buffer,-1);

                               if(ret<0)
                               {
                                  break;
                               }

							   p_recoder->stream_buffer_look_up[stream_buffer.buffer_index]=stream_buffer;

                                //recorder
                                ret=send_stream_buffer_to_venc(p_recoder->codec_id,&stream_buffer);
                                if(ret!=0){
							       ar_hal_vin_return_stream_buffer(fd_stream,pipe_id,stream_id,&stream_buffer);
                                }
                               if(num>0)
							   {
							      loop_num--;
                               }
						  }
						  ar_hal_vin_delete_stream_client(fd_pipe,pipe_id,stream_id,client_id);
						  ar_hal_vin_close_stream_dev(fd_stream);
						  p_recoder->bitstream_thread_run=0;
						  ar_osal_thread_join(p_recoder->bitstream_thread);
						  rtsp_stop();
						  uninit_venc(p_recoder->codec_id);

					  }else
					  {
					     ar_always("the dev %d have following stream ...",dev_id);
						 for(int i=0;i<num;i++)
						 {
						     STRU_AR_STREAM_PROPERTY_T prop;
                             ar_hal_vin_get_stream_property(fd_pipe,pipe_id,i,&prop);
				             ar_always("stream id = %d =================",i);
							 ar_always("camera_id=%d",prop.camera_id);
							 ar_always("stream_type=%d",prop.stream_type);
							 ar_always("mode=%d",prop.mode);
							 ar_always("width=%d",prop.width);
							 ar_always("height=%d",prop.height);
							 ar_always("format=%d",prop.format);
							 ar_always("fps=%f",prop.fps);
							 ar_always("x=%d",prop.x);
							 ar_always("y=%d",prop.y);
							 ar_always("total_w=%d",prop.total_w);
							 ar_always("total_h=%d",prop.total_h);
							 ar_always("stride=%d",prop.stride);
						 }

					  }
				  }else
				  {
				      ar_err("open fd pipe failed");
				  }
				  ar_hal_vin_close_pipe_dev(fd_pipe);
			   }
			   else
			   {
			      ar_err("can not get bind pipid by devid");
			   }
			}
		  }else
		  {
			  ar_err("the dev not opend, can not do this test, so open the dev, to do later");
		  }
		 ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(start_recoder)=
{
   "--start_recoder",
   api_test_start_recoder,
   {
      "test the api ar_hal_vin_get_dev_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-stream_id [id]:",
	  "-num [num]: how much frame will be recorded",
	  "-name [name]: the dump file name. such  as isp_out, not suggust record.hevc etc",
	  "-codec_type [h264/h256/jpeg]",
	  "-codec_id [id]",
	  "-bitrate [khz]",
	  "-gopsize [size]",
	  NULL,
   },
};


