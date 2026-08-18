#ifndef __VIDEO_PACKAGING_H__
#define __VIDEO_PACKAGING_H__


#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>


#define PACKAGE_MP4 1

#define USING_PTS 1
#define DEBUG 1

#define STREAM_FRAME_RATE 25
typedef struct
{

	AVFormatContext* g_OutFmt_Ctx[VENC_MAX_CHN_NUM];	//每个通道的AVFormatContext
	int vi[VENC_MAX_CHN_NUM];	//视频流索引号
	int ai[VENC_MAX_CHN_NUM];	//音频流索引号
	AR_BOOL b_First_IDR_Find[VENC_MAX_CHN_NUM];	//第一帧是I帧标志
	long int VptsInc[VENC_MAX_CHN_NUM];	//用于视频帧递增计数
	long int AptsInc[VENC_MAX_CHN_NUM];	//音频帧递增
	AR_U64 Audio_PTS[VENC_MAX_CHN_NUM];	//音频PTS
	AR_U64 Video_PTS[VENC_MAX_CHN_NUM];	//视频PTS
	AR_U64 Afirst[VENC_MAX_CHN_NUM];	//是文件第一帧音频标志
	AR_U64 Vfirst[VENC_MAX_CHN_NUM];	//视频第一帧标志
	AR_BOOL state;
	long int moov_pos[VENC_MAX_CHN_NUM];	//moov的pos，未使用
	int moov_flags[VENC_MAX_CHN_NUM];	//moov前置标志，未使用
	int file_flags[VENC_MAX_CHN_NUM];
	char filename[VENC_MAX_CHN_NUM][1024];	//文件名
}FfmpegConf;

#ifdef ENABLE_VER4
int AR_PDT_CreateMp4(VENC_CHN VeChn, char *pfile, FfmpegConf *fc);
AR_S32 AR_PDT_WriteVideo(VENC_CHN VeChn, VENC_STREAM_S *pstStream, FfmpegConf *fc);
void AR_PDT_CloseMp4(VENC_CHN VeChn, FfmpegConf *fc);
#else
int AR_PDT_CreateMp4(VENC_CHN VeChn, char *pfile, PAYLOAD_TYPE_E enType, int width, int height, int frameRate, int gop);
int AR_PDT_WriteVideo(AR_HANDLE VencHdl, VENC_STREAM_S *pstStream, PAYLOAD_TYPE_E enType);
void AR_PDT_CloseMp4(AR_VOID);
#endif


#endif /* End of #ifndef __VIDEO_PACKAGING_H__*/
