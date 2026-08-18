#include "stdio.h"
#include "stdlib.h"
#include <getopt.h>
#include <fcntl.h>
#include "mpi_venc.h"
#include "ar_comm_venc.h"
#include  "signal.h"
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

static AVFormatContext* g_OutFmt_Ctx;
static AVCodecContext *PAVCodecCtx;
static char pszFileName[256] = {0};
static int vi = -1;
static int STREAM_FRAME_RATE = 30;
static AR_BOOL b_First_IDR_Find = AR_FALSE;
static int ptsInc = 0;

static int AR_PDT_CheckKeyFrame_H264(H264E_NALU_TYPE_E enH264EType)
{

    AR_BOOL isIDR = 0;

    switch(enH264EType)
    {
        case H264E_NALU_IDRSLICE:
        case H264E_NALU_ISLICE:
        case H264E_NALU_SPS:
        case H264E_NALU_PPS:
            isIDR = 1;
        break;
        default:
        break;
    }

    return isIDR;
}


static int AR_PDT_CheckKeyFrame_H265(H265E_NALU_TYPE_E enH265EType)
{

    AR_BOOL isIDR = 0;

    switch(enH265EType)
    {
        case H265E_NALU_IDRSLICE:
        case H265E_NALU_ISLICE:
        case H265E_NALU_SPS:
        case H265E_NALU_PPS:
        case H265E_NALU_VPS:
            isIDR = 1;
        break;
        default:
        break;
    }

    return isIDR;
}


/* Add an output stream */
static int AR_PDT_Add_Stream(AVFormatContext *poutFmtCtx, PAYLOAD_TYPE_E enType, int width, int height, int frameRate, int gop)
{
    AVStream *pAVStream = NULL;
    enum AVCodecID eCodecId = AV_CODEC_ID_H264;
    AVCodecParameters *pCodecParams = NULL;

    if (enType == PT_H265)
        eCodecId = AV_CODEC_ID_HEVC;

    pAVStream = avformat_new_stream(poutFmtCtx, NULL);
    if (NULL == pAVStream)
    {
       printf("could not allocate stream \n");
       return -1;
    }

    pAVStream->id = poutFmtCtx->nb_streams-1;
    vi = pAVStream->index;
    pCodecParams = pAVStream->codecpar;
    printf("AVMEDIA_TYPE_VIDEO\n");
    pCodecParams->codec_id = eCodecId;
    pCodecParams->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecParams->bit_rate = 1024<<10;
    pCodecParams->width = width;
    pCodecParams->height = height;
    pCodecParams->format = AV_PIX_FMT_YUV420P;
    pCodecParams->field_order = AV_FIELD_PROGRESSIVE;

    return AR_SUCCESS;
}

int AR_PDT_CreateMp4(VENC_CHN VeChn, char *pfile, PAYLOAD_TYPE_E enType, int width, int height, int frameRate, int gop)
{
    int ret = 0; // 成功返回0，失败返回1
    AVOutputFormat *pOutFmt = NULL;

    sprintf(pszFileName,"%s_%d.mp4",pfile, VeChn);

    // avcodec_register_all();
    avformat_alloc_output_context2(&g_OutFmt_Ctx, NULL, "mp4", NULL);

    if (NULL == g_OutFmt_Ctx)
    {   //try default
        printf("Could not deduce output format from file extension: using mp4.\n");
        avformat_alloc_output_context2(&g_OutFmt_Ctx, NULL, "mp4", pszFileName);
        if (NULL == g_OutFmt_Ctx)
        {
            printf("avformat_alloc_output_context2 failed  \n");
            return -1;
        }
    }

    pOutFmt = g_OutFmt_Ctx->oformat;
    if (pOutFmt->video_codec == AV_CODEC_ID_NONE)
    {
        printf("add_stream ID =%d \n",pOutFmt->video_codec);
        goto exit_outFmt_failed;
    }

    ret = AR_PDT_Add_Stream(g_OutFmt_Ctx, enType, width, height, frameRate, gop);
    if (ret <0)
    {
        printf("AR_PDT_Add_Stream Failed \n");
        goto exit_outFmt_failed;
    }

    printf("==========Output Information==========\n");
    av_dump_format(g_OutFmt_Ctx, 0, pszFileName, 1);

    /* open the output file, if needed */
    if (!(pOutFmt->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&g_OutFmt_Ctx->pb, pszFileName, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            printf("could not open %s\n", pszFileName);
            goto exit_avio_open_failed;
        }
    }

    /* Write the stream header, if any */
    ret = avformat_write_header(g_OutFmt_Ctx, NULL);
    if (ret < 0)
    {
        printf("Error occurred when opening output file : %d\n", ret);
        goto exit_writeheader_failed;
    }

    b_First_IDR_Find = 0;
    return AR_SUCCESS;

exit_writeheader_failed:
exit_avio_open_failed:
    if (g_OutFmt_Ctx && !(g_OutFmt_Ctx->flags & AVFMT_NOFILE))
        avio_close(g_OutFmt_Ctx->pb);
exit_outFmt_failed:
    if (NULL != g_OutFmt_Ctx)
        avformat_free_context(g_OutFmt_Ctx);
    return -1;
}

void AR_PDT_CloseMp4(AR_VOID)
{
    if (g_OutFmt_Ctx)
        av_write_trailer(g_OutFmt_Ctx);

    if (g_OutFmt_Ctx && !(g_OutFmt_Ctx->oformat->flags & AVFMT_NOFILE))
        avio_close(g_OutFmt_Ctx->pb);

    if (g_OutFmt_Ctx)
    {
        avformat_free_context(g_OutFmt_Ctx);
        g_OutFmt_Ctx = NULL;
    }
    printf("output: %s\n", pszFileName);
    vi = -1;
    b_First_IDR_Find = 0;
}

int AR_PDT_WriteVideo(AR_HANDLE VencHdl, VENC_STREAM_S *pstStream, PAYLOAD_TYPE_E enType)
{
    unsigned int i=0;
    unsigned char* pPackVirtAddr = NULL;
    unsigned int u32PackLen = 0;
    unsigned int u32PackOffset = 0;
    static unsigned long u64LastPts = 0;
    H264E_NALU_TYPE_E enH264EType;
    H265E_NALU_TYPE_E enH265EType;
    int ret = 0;
    AVStream *pst = NULL;
    AVPacket pkt;
    int isIDR =0;

    if(vi<0)
    {
        printf("vi less than 0 \n");
        return;
    }

    if (NULL == pstStream)
    {
        return AR_SUCCESS;
    }

    pst = g_OutFmt_Ctx->streams[vi];

    for (i = 0 ; i < pstStream->u32PackCount; i++)
    {
        pPackVirtAddr = pstStream->pstPack[i].pu8Addr;
        u32PackLen = pstStream->pstPack[i].u32Len;
        u32PackOffset = pstStream->pstPack[i].u32Offset;
        isIDR = 0;
        av_init_packet(&pkt);

        if(PT_H264 == enType)
        {
            enH264EType = pstStream->pstPack[i].DataType.enH264EType;
            isIDR = AR_PDT_CheckKeyFrame_H264(enH264EType);
        }
        else if (PT_H265 == enType)
        {
            enH265EType = pstStream->pstPack[i].DataType.enH265EType;
            isIDR = AR_PDT_CheckKeyFrame_H265(enH265EType);
        }
        else
        {
            printf("unsupport stream type!\n");
            return AR_FAILURE;
        }

        pkt.flags |=  (isIDR==1) ? AV_PKT_FLAG_KEY : 0;
        pkt.stream_index = pst->index;
        pkt.data = (unsigned char*)(pPackVirtAddr + u32PackOffset);
        pkt.size = u32PackLen - u32PackOffset;

        if (b_First_IDR_Find == 0)
        {
            if (0 == (pkt.flags & AV_PKT_FLAG_KEY))
                continue;

            b_First_IDR_Find = 1;
        }
#if 0
      // pkt.pts = av_rescale_q((ptsInc++), pst->codec->time_base,pst->time_base);
      pkt.pts = av_rescale_q((ptsInc++), PAVCodecCtx->time_base, pst->time_base);
      pkt.dts=av_rescale_q_rnd(pkt.dts, pst->time_base,pst->time_base,(enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
      pkt.duration = av_rescale_q(pkt.duration,pst->time_base, pst->time_base);
#else
        AVRational sysTimeBase = {1, 1000000};
        pkt.dts = pkt.pts = av_rescale_q(pstStream->pstPack[i].u64PTS, sysTimeBase, pst->time_base);
        if (u64LastPts) {
            pkt.duration = pkt.pts - u64LastPts;
            if (pkt.duration == 0)//first pack include header and IDR,so pts is same
                pkt.dts = pkt.pts = (pkt.pts + 1);
        } else {
            pkt.duration = 0;
        }
        u64LastPts = pkt.pts;
#endif
        pkt.pos = -1;

        ret = av_interleaved_write_frame(g_OutFmt_Ctx, &pkt);
        if (ret < 0)
        {
            printf("cannot write frame");
        }
    }
    return AR_SUCCESS;
}
