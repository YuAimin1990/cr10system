#define _GNU_SOURCE

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <pthread.h>
#include "sample_comm.h"

#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>

#define H26X_START_CODE_SIZE  4
#define DEBUG_SEQUENCE_CHANGE 1

#ifdef DEBUG_SEQUENCE_CHANGE
#include "h264_stream.h"
#include "h265_stream.h"
#endif

VB_POOL g_ahPicVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};
VB_POOL g_ahTmvVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};

int task_done[TEST_VDEC_MAX_CHN_NUM] = {0};

#define HEXDUMP_COLS 16
#define AR_ALIGN1024(_x) (((_x)+0x3ff)&~0x3ff)
#define PRINTF_VDEC_CHN_STATUS(Chn, stStatus) \
do{\
    logline("\033[0;33m ---------------------------------------------------------------------------------------------------\033[0;39m\n");\
    logline("\033[0;33m chn:%d, Type:%d, bStart:%d, DecodeFrames:%d, LeftPics:%d, LeftBytes:%d, LeftFrames:%d, RecvFrames:%d  \033[0;39m\n",\
        Chn,\
        stStatus.enType,\
        stStatus.bStartRecvStream,\
        stStatus.u32DecodeStreamFrames,\
        stStatus.u32LeftPics,\
        stStatus.u32LeftStreamBytes,\
        stStatus.u32LeftStreamFrames,\
        stStatus.u32RecvStreamFrames);\
    logline("\033[0;33m FormatErr:%d,    s32PicSizeErrSet:%d,  s32StreamUnsprt:%d,  s32PackErr:%d,  u32PrtclNumErrSet:%d,  s32RefErrSet:%d,  s32PicBufSizeErrSet:%d  \033[0;39m\n",\
        stStatus.stVdecDecErr.s32FormatErr,\
        stStatus.stVdecDecErr.s32PicSizeErrSet,\
        stStatus.stVdecDecErr.s32StreamUnsprt,\
        stStatus.stVdecDecErr.s32PackErr,\
        stStatus.stVdecDecErr.s32PrtclNumErrSet,\
        stStatus.stVdecDecErr.s32RefErrSet,\
        stStatus.stVdecDecErr.s32PicBufSizeErrSet);\
    logline("\033[0;33m ---------------------------------------------------------------------------------------------------\033[0;39m\n");\
}while(0)

#define SAVE_FILE_NAME(aFileName, chn, s32Cnt, cOutName, enPixelFormat)\
do{\
    AR_CHAR *Postfix;\
    if (enPixelFormat == PIXEL_FORMAT_RGB_565)\
    {\
        Postfix = "rgb565";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_BGR_565)\
    {\
        Postfix = "bgr565";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_RGB_888)\
    {\
        Postfix = "rgb888";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_BGR_888)\
    {\
        Postfix = "bgr888";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888)\
    {\
        Postfix = "argb8888";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_ABGR_8888)\
    {\
        Postfix = "abgr8888";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555)\
    {\
        Postfix = "argb1555";\
    }\
    else if (enPixelFormat == PIXEL_FORMAT_ABGR_1555)\
    {\
        Postfix = "abgr1555";\
    }\
    else if ((enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)||(enPixelFormat == PIXEL_FORMAT_YUV_400)||(enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_420))\
    {\
        Postfix = "yuv";\
    }\
    else if ((enPixelFormat == 0))\
    {\
        Postfix = "yuv";\
    }\
    else\
    {\
        logerr("[%s]-%d: enPixelFormat type err", __FUNCTION__, __LINE__);\
        return (AR_VOID *)(AR_FAILURE);\
    }\
    snprintf(aFileName, sizeof(aFileName), "%s.chn%d.%s", cOutName, chn, Postfix);\
}while(0)

AR_S32 SAMPLE_COMM_VDEC_InitVBPool(AR_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    VB_CONFIG_S stVbConf;
    AR_S32 i, j, pos=0, s32Ret;
    AR_BOOL bFindFlag;
    SAMPLE_VDEC_BUF astSampleVdecBuf[TEST_VDEC_MAX_CHN_NUM];
    VB_POOL_CONFIG_S stVbPoolCfg;

    memset(astSampleVdecBuf, 0, sizeof(SAMPLE_VDEC_BUF)*TEST_VDEC_MAX_CHN_NUM);
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    for(i=0; i<ChnNum; i++)
    {
        if (PT_H265 == pastSampleVdec[i].enType)
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width,
                                                                      pastSampleVdec[i].u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                                                      pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
            astSampleVdecBuf[i].u32TmvBufSize = VDEC_GetTmvBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width,
                                                                      pastSampleVdec[i].u32Height);
        }
        else if (PT_H264 == pastSampleVdec[i].enType)
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width,
                                                                      pastSampleVdec[i].u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                                                      pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
            if (VIDEO_DEC_MODE_IPB == pastSampleVdec[i].stSapmleVdecVideo.enDecMode)
            {
                astSampleVdecBuf[i].u32TmvBufSize = VDEC_GetTmvBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width,
                                                                      pastSampleVdec[i].u32Height);
            }
        }
        else
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width,
                                                                      pastSampleVdec[i].u32Height,
                                                                      pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat,
                                                                      DATA_BITWIDTH_8, 0);
        }
    }

    /* PicBuffer */
    for(j=0; j<VB_MAX_COMM_POOLS; j++)
    {
        bFindFlag = AR_FALSE;
        for(i=0; i<ChnNum; i++)
        {
            if ((AR_FALSE == bFindFlag) && (0 != astSampleVdecBuf[i].u32PicBufSize) && (AR_FALSE == astSampleVdecBuf[i].bPicBufAlloc) )
            {
                stVbConf.astCommPool[j].u64BlkSize = astSampleVdecBuf[i].u32PicBufSize;
                stVbConf.astCommPool[j].u32BlkCnt  = pastSampleVdec[i].u32FrameBufCnt;
                astSampleVdecBuf[i].bPicBufAlloc   = AR_TRUE;
                bFindFlag                          = AR_TRUE;
                pos = j;
            }

            if ((AR_TRUE == bFindFlag) && (AR_FALSE == astSampleVdecBuf[i].bPicBufAlloc)
                && (stVbConf.astCommPool[j].u64BlkSize == astSampleVdecBuf[i].u32PicBufSize) )
            {
                stVbConf.astCommPool[j].u32BlkCnt += pastSampleVdec[i].u32FrameBufCnt;
                astSampleVdecBuf[i].bPicBufAlloc   = AR_TRUE;
            }
        }
    }

    /* TmvBuffer */
    for(j=pos+1; j<VB_MAX_COMM_POOLS; j++)
    {
        bFindFlag = AR_FALSE;
        for(i=0; i<ChnNum; i++)
        {
            if ((AR_FALSE == bFindFlag) && (0 != astSampleVdecBuf[i].u32TmvBufSize) && (AR_FALSE == astSampleVdecBuf[i].bTmvBufAlloc) )
            {
                stVbConf.astCommPool[j].u64BlkSize = astSampleVdecBuf[i].u32TmvBufSize;
                stVbConf.astCommPool[j].u32BlkCnt  = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum+1;
                astSampleVdecBuf[i].bTmvBufAlloc   = AR_TRUE;
                bFindFlag                          = AR_TRUE;
                pos = j;
            }

            if ((AR_TRUE == bFindFlag) && (AR_FALSE == astSampleVdecBuf[i].bTmvBufAlloc)
                && (stVbConf.astCommPool[j].u64BlkSize == astSampleVdecBuf[i].u32TmvBufSize) )
            {
                stVbConf.astCommPool[j].u32BlkCnt += pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum+1;
                astSampleVdecBuf[i].bTmvBufAlloc   = AR_TRUE;
            }
        }
    }
    stVbConf.u32MaxPoolCnt = pos + 1;

    if (VB_SOURCE_MODULE == pastSampleVdec[0].vbSource)
    {
        AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
        CHECK_RET(AR_MPI_VB_SetModPoolConfig(VB_UID_VDEC, &stVbConf), "AR_MPI_VB_SetModPoolConfig");
        s32Ret = AR_MPI_VB_InitModCommPool(VB_UID_VDEC);
        if (AR_SUCCESS != s32Ret)
        {
            logerr("AR_MPI_VB_InitModCommPool failed, ret = %#x", s32Ret);
            AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
            return AR_FAILURE;
        }
    }
    else if (VB_SOURCE_USER == pastSampleVdec[0].vbSource)
    {
        for (i = 0; i < ChnNum; i++)
        {
            if ( (0 != astSampleVdecBuf[i].u32PicBufSize) && (0 != pastSampleVdec[i].u32FrameBufCnt))
            {
                memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
                stVbPoolCfg.u64BlkSize  = astSampleVdecBuf[i].u32PicBufSize;
                stVbPoolCfg.u32BlkCnt   = pastSampleVdec[i].u32FrameBufCnt;
                stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
                g_ahPicVbPool[i] = AR_MPI_VB_CreatePool(&stVbPoolCfg);
                if (VB_INVALID_POOLID == g_ahPicVbPool[i])
                {
                    goto fail;
                }
            }
            if (0 != astSampleVdecBuf[i].u32TmvBufSize)
            {
                memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
                stVbPoolCfg.u64BlkSize  = astSampleVdecBuf[i].u32TmvBufSize;
                stVbPoolCfg.u32BlkCnt   = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum+1;
                stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
                g_ahTmvVbPool[i] = AR_MPI_VB_CreatePool(&stVbPoolCfg);
                if (VB_INVALID_POOLID == g_ahTmvVbPool[i])
                {
                    goto fail;
                }
            }
        }
    }

    return AR_SUCCESS;

fail:
    for (;i>=0;i--)
    {
        if (VB_INVALID_POOLID != g_ahPicVbPool[i])
        {
            s32Ret = AR_MPI_VB_DestroyPool(g_ahPicVbPool[i]);
            if (AR_SUCCESS != s32Ret)
            {
                logerr("AR_MPI_VB_DestroyPool %d failed, ret = %#x", g_ahPicVbPool[i], s32Ret);
            }
            g_ahPicVbPool[i] = VB_INVALID_POOLID;
        }

        if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
        {
            s32Ret = AR_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
            if (AR_SUCCESS != s32Ret)
            {
                logerr("AR_MPI_VB_DestroyPool %d failed, ret = %#x", g_ahTmvVbPool[i], s32Ret);
            }
            g_ahTmvVbPool[i] = VB_INVALID_POOLID;
        }
    }
    return AR_FAILURE;
}

AR_VOID SAMPLE_COMM_VDEC_ExitVBPool(SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    AR_S32 i, s32Ret;

    if (VB_SOURCE_MODULE == pastSampleVdec[0].vbSource)
    {
        AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    }
    else if (VB_SOURCE_USER == pastSampleVdec[0].vbSource)
    {
        for (i=VB_MAX_POOLS-1; i>=0; i--)
        {
            if (VB_INVALID_POOLID != g_ahPicVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahPicVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    logerr("AR_MPI_VB_DestroyPool %d failed, ret = %#x", g_ahPicVbPool[i], s32Ret);
                }
                g_ahPicVbPool[i] = VB_INVALID_POOLID;
            }

            if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    logerr("AR_MPI_VB_DestroyPool %d failed, ret = %#x", g_ahTmvVbPool[i], s32Ret);
                }
                g_ahTmvVbPool[i] = VB_INVALID_POOLID;
            }
        }
    }

    return;
}

static AR_S32 SAMPLE_COMM_VDEC_SaveYUV4xxP_File(FILE* pfd, AR_S32 s32Count, VIDEO_FRAME_S* pVBuf)
{
    AR_U32 u32ChromaPicW = 0;
    AR_U32 u32ChromaPicH = 0;
    AR_U8 *pVirAddr_Y = (AR_U8 *) pVBuf->u64VirAddr[0];
    AR_U8 *pVirAddr_U = (AR_U8 *) pVBuf->u64VirAddr[1];
    AR_U8 *pVirAddr_V = (AR_U8 *) pVBuf->u64VirAddr[2];
    PIXEL_FORMAT_E enPixelFormat = pVBuf->enPixelFormat;

    switch (enPixelFormat) {
    case PIXEL_FORMAT_YVU_PLANAR_420:
        u32ChromaPicW = pVBuf->u32Width/2;
        u32ChromaPicH = pVBuf->u32Height/2;
        break;
    case PIXEL_FORMAT_YVU_PLANAR_422:
        u32ChromaPicW = pVBuf->u32Width/2;
        u32ChromaPicH = pVBuf->u32Height;
        break;
    case PIXEL_FORMAT_YVU_PLANAR_444:
        u32ChromaPicW = pVBuf->u32Width;
        u32ChromaPicH = pVBuf->u32Height;
        break;
    default:
        logerr("invalid picture format %d\n", enPixelFormat);
        return -1;
    }

    fprintf(stderr, "saving......Y......\n");
    fflush(stderr);
    for (int i = 0; i < pVBuf->u32Height; i++)
    {
        fwrite(pVirAddr_Y, pVBuf->u32Width, 1, pfd);
        pVirAddr_Y += pVBuf->u32Stride[0];
    }
    fflush(pfd);

    fprintf(stderr, "saving......U......\n");
    fflush(stderr);
    for (int i = 0; i < u32ChromaPicH; i++)
    {
        fwrite(pVirAddr_U, u32ChromaPicW, 1, pfd);
        pVirAddr_U += pVBuf->u32Stride[1];
    }
    fflush(pfd);

    fprintf(stderr, "saving......V......\n");
    fflush(stderr);
    for (int i = 0; i < u32ChromaPicH; i++)
    {
        fwrite(pVirAddr_V, u32ChromaPicW, 1, pfd);
        pVirAddr_V += pVBuf->u32Stride[2];
    }
    fflush(pfd);

    fprintf(stderr, "save %dth yuv data done !\n", s32Count);
    fflush(stderr);

    return AR_SUCCESS;
}

#ifdef DEBUG_SEQUENCE_CHANGE
static AR_BOOL check_codec_type_changed(AR_U8* buf, AR_U32 size, PAYLOAD_TYPE_E type, PAYLOAD_TYPE_E* new_type)
{
    int i = 0;
    AR_U8* p = buf;

    if (size <= H26X_START_CODE_SIZE + 1) {
        return AR_FALSE;
    }

    do {
        if ((p[i] == 0x00 && p[i + 1] == 0x00 && p[i + 2] == 0x01 && ((p[i + 3] & 0x1f) == H264_NAL_UNIT_TYPE_SPS)) ||
            (p[i] == 0x00 && p[i + 1] == 0x00 && p[i + 2] == 0x00 && p[i + 3] == 0x01 && ((p[i + 4] & 0x1f) == H264_NAL_UNIT_TYPE_SPS))) {
            *new_type = PT_H264;
            break;
        } else if ((p[i] == 0x00 && p[i + 1] == 0x00 && p[i + 2] == 0x01 && ((p[i + 3] & 0x7e) == H265_NAL_UNIT_VPS)) ||
                   (p[i] == 0x00 && p[i + 1] == 0x00 && p[i + 2] == 0x00 && p[i + 3] == 0x01 && ((p[i + 4] & 0x7e) == H265_NAL_UNIT_VPS))) {
            *new_type = PT_H265;
            break;
        }

        i++;
    } while (i < size - H26X_START_CODE_SIZE);

    if (i == size - H26X_START_CODE_SIZE) {
        return AR_FALSE;
    }

    if (type == *new_type) {
        return AR_FALSE;
    } else {
        return AR_TRUE;
    }
}

static AR_S32 h264_extract_frame_info(h264_stream_t* parser, AR_U8* buf, AR_U32 size, AR_S32* width, AR_S32* height)
{
    AR_S32 ret = 0;
    AR_U32 consume = 0;
    AR_S32 nal_len = 0;
    AR_S32 nal_start = 0;
    AR_S32 nal_end = size;
    AR_BOOL has_nal = false;

    do {
        has_nal = h264_has_start_code(buf + consume);
        nal_len = h264_find_nal_unit(buf + consume, size - consume, &nal_start, &nal_end);
        if (nal_len > 0 || (has_nal && nal_len <= 0)) {
            ret = h264_read_nal_unit(parser, buf + consume + nal_start, nal_end - nal_start);
            if (ret < 0) {
                logline("h264 parser failed, buf %p has_nal %d start %d end %d", buf, has_nal, nal_start, nal_end);
                break;
            }
        } else {
            break;
        }

        consume += nal_end;
        switch (parser->nal->nal_unit_type) {
        case H264_NAL_UNIT_TYPE_SPS:
            break;
        case H264_NAL_UNIT_TYPE_PPS:
            *width  = (parser->sps->pic_width_in_mbs_minus1 + 1) * 16;
            *height = (parser->sps->pic_height_in_map_units_minus1 + 1) * 16;
            break;
        case H264_NAL_UNIT_TYPE_CODED_SLICE_IDR:
            break;
        default:
            break;
        }
    } while (consume < size);

    return ret;
}

static AR_S32 h265_extract_frame_info(h265_stream_t* parser, AR_U8* buf, AR_U32 size, AR_S32* width, AR_S32* height)
{
    AR_S32 ret = 0;
    AR_U32 consume = 0;
    AR_S32 nal_len = 0;
    AR_S32 nal_start = 0;
    AR_S32 nal_end = size;
    AR_BOOL has_nal = false;

    do {
        has_nal = h265_has_start_code(buf + consume);
        nal_len = h265_find_nal_unit(buf + consume, size - consume, &nal_start, &nal_end);
        if (nal_len > 0 || (has_nal && nal_len <= 0)) {
            ret = h265_read_nal_unit(parser, buf + consume + nal_start, nal_end - nal_start);
            if (ret < 0) {
                logline("h265 parser failed, buf %p has_nal %d start %d end %d", buf, has_nal, nal_start, nal_end);
                break;
            }
        } else {
            break;
        }

        switch (parser->nal->nal_unit_type) {
        case H265_NAL_UNIT_VPS:
            break;
        case H265_NAL_UNIT_SPS:
            *width  = parser->sps->pic_width_in_luma_samples;
            *height = parser->sps->pic_height_in_luma_samples;
            break;
        case H265_NAL_UNIT_PPS:
            break;
        case H265_NAL_UNIT_CODED_SLICE_IDR_W_RADL:
            break;
        default:
            break;
        }
        consume += nal_end;
    } while (consume < size);

    return ret;
}
#endif

AVFormatContext *read_stream_info(char *stream_file, int* video_index, unsigned int* fps)
{
    int ret = 0;
    AVFormatContext *fmtCtx = avformat_alloc_context();

    fmtCtx->flags |= AV_CODEC_FLAG_TRUNCATED;
    if ((ret = avformat_open_input(&fmtCtx, stream_file, 0, 0)) < 0) {
        logerr("avformat_open_input %s failed, error(%s)", stream_file, av_err2str(ret));
        return NULL;
    }

    if ((ret = avformat_find_stream_info(fmtCtx, 0)) < 0) {
        logerr("avformat_find_stream_info %s, error(%s)", stream_file, av_err2str(ret));
        return NULL;
    }

    *video_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (*video_index < 0) {
        logerr("av_find_best_stream %s failed\n", stream_file);
        return NULL;
    }

    if (fmtCtx->streams[*video_index]->avg_frame_rate.den && fmtCtx->streams[*video_index]->avg_frame_rate.num) {
        *fps = *fps ? *fps : av_q2d(fmtCtx->streams[*video_index]->avg_frame_rate);
    }

    *fps = *fps ? *fps : 25;

    av_dump_format(fmtCtx, 0, stream_file, 0);

    return fmtCtx;
}

AR_VOID * SAMPLE_COMM_VDEC_SendStream(AR_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam = (VDEC_THREAD_PARAM_S *)pArgs;
    AR_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm = NULL;
    FILE *fp_dump = NULL;
    AR_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream = {0};
    AR_BOOL bFindStart, bFindEnd;
    AR_U32 u32Len /*, u32Start*/;
    AR_S32 s32Ret, i;
    AR_CHAR cStreamFile[256];
    int send_frame_counter = 0;
    int buffer_full_counter = 0;
    int video_index = -1;
    AR_U64 u64PTS = 0;
    uint64_t send_frame_tm = 0;
    uint64_t buffer_full_tm = 0;
    uint64_t delay_ms = 0;
    uint64_t frame_rate_ms;
    AR_S32 s32Width = 0, s32Height = 0;
    AR_S32 s32NewWidth = 0, s32NewHeight = 0;
#ifdef DEBUG_SEQUENCE_CHANGE
    PAYLOAD_TYPE_E enNewType;
    PAYLOAD_TYPE_E enType = pstVdecThreadParam->enType;
    h264_stream_t* h264_parser = h264_new();
    h265_stream_t* h265_parser = h265_new();
#endif
    AVPacket *packet = NULL;
    AVBSFContext *bsfCtx = NULL;
    AVFormatContext *fmtCtx = NULL;

    prctl(PR_SET_NAME, "VideoSendStream", 0,0,0);
    snprintf(cStreamFile, sizeof(cStreamFile), pstVdecThreadParam->cFileName);
    logline("chn%d cStreamFile=%s, cOutFile=%s\n",
             pstVdecThreadParam->s32ChnId, cStreamFile, pstVdecThreadParam->cOutName);
    fflush(stdout);

#if 0
    char dumpfile[128] = "dumpfile.h265";
    fp_dump = fopen(dumpfile,"wb");
#endif

    if (pstVdecThreadParam->enType == PT_H264 ||
        pstVdecThreadParam->enType == PT_H265) {
        fmtCtx = read_stream_info(cStreamFile, &video_index, &pstVdecThreadParam->u32Fps);
        if (fmtCtx == NULL) {
            logerr("chn%d read %s stream info failed\n", pstVdecThreadParam->s32ChnId, cStreamFile);
            return (AR_VOID *)(AR_FAILURE);
        }

        frame_rate_ms = 1000 / pstVdecThreadParam->u32Fps;
        s32Width  = s32NewWidth  = fmtCtx->streams[video_index]->codecpar->width;
        s32Height = s32NewHeight = fmtCtx->streams[video_index]->codecpar->height;

        packet = av_packet_alloc();

        if (pstVdecThreadParam->enType == PT_H264) {
            av_bsf_list_parse_str("h264_mp4toannexb,filter_units=pass_types=1-8,extract_extradata,h264_metadata", &bsfCtx);
        } else if (pstVdecThreadParam->enType == PT_H265) {
            av_bsf_list_parse_str("hevc_mp4toannexb,filter_units=remove_types=35|38,extract_extradata,hevc_metadata", &bsfCtx);
        }

        avcodec_parameters_copy(bsfCtx->par_in, fmtCtx->streams[video_index]->codecpar);

        s32Ret = av_bsf_init(bsfCtx);
        if (s32Ret != 0) {
            logerr("chn%d av_bsf_init failed, error(%s)\n", pstVdecThreadParam->s32ChnId, av_err2str(s32Ret));
            return (AR_VOID*)(AR_FAILURE);
        }
    } else if (pstVdecThreadParam->enType == PT_MJPEG ||
               pstVdecThreadParam->enType == PT_JPEG) {
        if (cStreamFile != 0)
        {
            fpStrm = fopen(cStreamFile, "rb");
            if (fpStrm == NULL)
            {
                logerr("chn%d can't open file %s in send stream thread, %s\n",
                        pstVdecThreadParam->s32ChnId, cStreamFile, strerror(errno));
                return (AR_VOID *)(AR_FAILURE);
            }
        }
        logline("\033[0;36mchn%d, stream file:%s, userbufsize: %d \033[0;39m\n",
                 pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cFileName, pstVdecThreadParam->s32MinBufSize);

        pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
        if (pu8Buf == NULL)
        {
            logerr("chn%d can't alloc %d in send stream thread, %s\n",
                    pstVdecThreadParam->s32ChnId, pstVdecThreadParam->s32MinBufSize, strerror(errno));
            fclose(fpStrm);
            return (AR_VOID *)(AR_FAILURE);
        }
    }

    u64PTS = pstVdecThreadParam->u64PtsInit;
    while (1)
    {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_PAUSE)
        {
            sleep(1);
            continue;
        }

        if (task_done[pstVdecThreadParam->s32ChnId] == AR_TRUE) {
            break;
        }

        if (pstVdecThreadParam->enType == PT_H264 ||
            pstVdecThreadParam->enType == PT_H265) {
            s32Ret = av_read_frame(fmtCtx, packet);
            if (s32Ret != 0) // eos
            {
                av_packet_unref(packet);
                if (pstVdecThreadParam->bCircleSend == AR_TRUE)
                {
                    printf("\033[0;32mchn%d replay...\033[0;39m\n", pstVdecThreadParam->s32ChnId);
                    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
                    s32Ret = av_seek_frame(fmtCtx, video_index, 0, AVSEEK_FLAG_BACKWARD);
                    if (s32Ret < 0) {
                        avformat_close_input(&fmtCtx);
                        fmtCtx = read_stream_info(cStreamFile, &video_index, &pstVdecThreadParam->u32Fps);
                        if (fmtCtx == NULL) {
                            logerr("chn%d read %s stream info failed\n",
                                     pstVdecThreadParam->s32ChnId, cStreamFile);
                            return (AR_VOID *)(AR_FAILURE);
                        }

                        frame_rate_ms = 1000 / pstVdecThreadParam->u32Fps;
                    }
                    s32Ret = av_read_frame(fmtCtx, packet);
                }
                else
                {
                    break;
                }
            }

            s32Ret = av_bsf_send_packet(bsfCtx, packet);
            if (s32Ret) {
                logerr("chn%d av_bsf_send_packet error, error(%s)\n", pstVdecThreadParam->s32ChnId, av_err2str(s32Ret));
                break;
            }

            s32Ret = av_bsf_receive_packet(bsfCtx, packet);
            if (s32Ret) {
                logline("chn%d av_bsf_receive_packet failed, error(%s)\n", pstVdecThreadParam->s32ChnId, av_err2str(s32Ret));
                continue;
            }

            stStream.u64PTS   = packet->pts == AV_NOPTS_VALUE ? u64PTS : packet->pts;
            stStream.pu8Addr  = packet->data;
            stStream.u32Len   = packet->size;
            stStream.bDisplay     = AR_TRUE;
            stStream.bEndOfFrame  = AR_TRUE;
            stStream.bEndOfStream = AR_FALSE;
        }
        else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG)
        {
            bFindStart   = AR_FALSE;
            bFindEnd     = AR_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bCircleSend == AR_TRUE)
                {
                    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

            for (i=0; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    bFindStart = AR_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i<s32ReadLen-3; i++)
            {
                if ((pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0)
                {
                     u32Len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                     i += 1 + u32Len;
                }
                else
                {
                    break;
                }
            }

            for (; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD9)
                {
                    bFindEnd = AR_TRUE;
                    break;
                }
            }
            s32ReadLen = i+2;

            if (bFindStart == AR_FALSE || bFindEnd == AR_FALSE)
            {
                //logline("chn%d can not find JPEG start code, ignore! s32ReadLen %d, s32UsedBytes %d!\n",
                //         pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
                // ignore invalid frame
                s32UsedBytes = s32UsedBytes +s32ReadLen;
                continue;
            }

            stStream.u64PTS       = u64PTS;
            stStream.pu8Addr      = pu8Buf;
            stStream.u32Len       = s32ReadLen;
            stStream.bDisplay     = AR_TRUE;
            stStream.bEndOfFrame  = AR_TRUE;
            stStream.bEndOfStream = AR_FALSE;
        }

        send_frame_tm = ar_get_linux_time();

        if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
            if (pstVdecThreadParam->discardMode && send_frame_counter &&
                (send_frame_counter % pstVdecThreadParam->discardMode == 0)) {
                logwarn("chn%d discard %d(%s) frame, len = %d, pts = %lld\n",
                         pstVdecThreadParam->s32ChnId, send_frame_counter, packet->flags ? "I" : "B/P",
                         stStream.u32Len, stStream.u64PTS);
                send_frame_counter++;
                goto skip_frame;
            }
        }

        // Notice: This sequence change sample is just for reference.
        // It's better for user to implement a new parser.
#ifdef DEBUG_SEQUENCE_CHANGE
        if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
            if (packet->flags == AV_PKT_FLAG_KEY) {
                AR_BOOL bCodcChanged = AR_FALSE;
                bCodcChanged = check_codec_type_changed(stStream.pu8Addr, stStream.u32Len, enType, &enNewType);
                if (bCodcChanged != AR_TRUE) {
                    if (pstVdecThreadParam->enType == PT_H264)
                        h264_extract_frame_info(h264_parser, stStream.pu8Addr, stStream.u32Len, &s32NewWidth, &s32NewHeight);

                    if (pstVdecThreadParam->enType == PT_H265)
                        h265_extract_frame_info(h265_parser, stStream.pu8Addr, stStream.u32Len, &s32NewWidth, &s32NewHeight);
                } else {
                    // Currently sdk don't support codec type change, app should first destroy the
                    // channel and then create channel with new codec type.
                    logerr("chn%d codec type changed, %d->%d\n", pstVdecThreadParam->s32ChnId, enType, enNewType);
                    goto decode_error;
                }
            }

            if (s32Width != s32NewWidth || s32Height != s32NewHeight) {
                logline("chn%d resolution changed, send %d(%s) frame, len = %d, pts = %lld, width = %d, height = %d\n",
                         pstVdecThreadParam->s32ChnId, send_frame_counter, packet->flags ? "I" : "B/P",
                         stStream.u32Len, stStream.u64PTS, s32NewWidth, s32NewHeight);

                s32Width  = s32NewWidth;
                s32Height = s32NewHeight;
            } else {
                logline("chn%d send %d(%s) frame, len = %d, pts = %lld, width = %d, height = %d\n",
                         pstVdecThreadParam->s32ChnId, send_frame_counter, packet->flags ? "I" : "B/P",
                         stStream.u32Len, stStream.u64PTS, s32NewWidth, s32NewHeight);
            }
        } else {
            logline("chn%d send frame %d, len = %d\n",
                     pstVdecThreadParam->s32ChnId, send_frame_counter, stStream.u32Len);
        }
#else
        if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
            logline("chn%d send %d(%s) frame, len = %d, pts = %lld\n",
                     pstVdecThreadParam->s32ChnId, send_frame_counter, packet->flags ? "I" : "B/P",
                     stStream.u32Len, stStream.u64PTS);
        } else {
            logline("chn%d send frame %d, len = %d\n",
                     pstVdecThreadParam->s32ChnId, send_frame_counter, stStream.u32Len);
        }
#endif

        if (fp_dump != NULL) {
            fwrite(stStream.pu8Addr, stStream.u32Len, 1, fp_dump);
        }

SendAgain:
        s32Ret = AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if ((AR_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl))
        {
            pstVdecThreadParam->s32IntervalTime += 10;
            if (s32Ret == AR_ERR_VDEC_NOMEM) {
                goto decode_error;
            } else if (s32Ret != AR_ERR_VDEC_BUF_FULL) {
                logerr("chn%d AR_MPI_VDEC_SendStream %dth failed, send again, ret = %x",
                        pstVdecThreadParam->s32ChnId, send_frame_counter, s32Ret);
            } else {
                usleep(2*1000);

                if (pstVdecThreadParam->s32SaveFile == AR_SAVE_YUV_NONE && pstVdecThreadParam->u32RecoverMode) {
                    if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
                        // check whether sdk stuck in buffer full
                        if (buffer_full_counter != send_frame_counter) {
                            buffer_full_tm = ar_get_linux_time();
                            buffer_full_counter = send_frame_counter;
                        } else {
                            if (ar_get_linux_time() - buffer_full_tm > 2000) {
                                logerr("chn%d stuck in buffer full over 2s", pstVdecThreadParam->s32ChnId);
#if 1
                                // do soft reset
                                AR_MPI_VDEC_ResetChn(pstVdecThreadParam->s32ChnId, false);
#else
                                // stop + hard reset + start
                                AR_MPI_VDEC_StopRecvStream(pstVdecThreadParam->s32ChnId);
                                AR_MPI_VDEC_ResetChn(pstVdecThreadParam->s32ChnId, true);
                                AR_MPI_VDEC_StartRecvStream(pstVdecThreadParam->s32ChnId);
#endif
                                buffer_full_tm = ar_get_linux_time();
                            }
                        }
                    }
                }
            }
            goto SendAgain;
        }
        else
        {
            send_frame_counter++;
            s32UsedBytes = s32UsedBytes +s32ReadLen;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
            delay_ms = ar_get_linux_time() - send_frame_tm;
            delay_ms = (delay_ms < frame_rate_ms) ? (frame_rate_ms - delay_ms) : 1;
            pstVdecThreadParam->s32IntervalTime = delay_ms * 1000;
        }
skip_frame:
        usleep(pstVdecThreadParam->s32IntervalTime);
        if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
            av_packet_unref(packet);
        }
    }

    if (fp_dump != NULL){
        fclose(fp_dump);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
    stStream.u64PTS       = -1;
    stStream.pu8Addr      = NULL;
    stStream.u32Len       = 0;
    stStream.bEndOfFrame  = AR_TRUE;
    stStream.bEndOfStream = AR_TRUE;
    stStream.bDisplay     = AR_FALSE;
    AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    if (pstVdecThreadParam->bCircleSend == AR_FALSE) {
        task_done[pstVdecThreadParam->s32ChnId] = AR_TRUE;
    }

decode_error:
    if (pstVdecThreadParam->enType == PT_H264 ||
        pstVdecThreadParam->enType == PT_H265) {
        av_bsf_free(&bsfCtx);
        av_packet_free(&packet);
        avformat_close_input(&fmtCtx);
    } else if (pstVdecThreadParam->enType == PT_MJPEG ||
               pstVdecThreadParam->enType == PT_JPEG) {
        if (pu8Buf != AR_NULL)
        {
            free(pu8Buf);
        }
        fclose(fpStrm);
    }

#ifdef DEBUG_SEQUENCE_CHANGE
    if (h264_parser) h264_free(h264_parser);
    if (h265_parser) h265_free(h265_parser);
#endif

    logline("\033[0;35mchn%d send EOS and return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);

    return (AR_VOID *)AR_SUCCESS;
}

/*
static void SAMPLE_COMM_VDEC_SaveYUVFile_Linear8Bit(FILE* pfd, VIDEO_FRAME_S* pVBuf)
{
    AR_U8* pY_map = NULL;
    AR_U8* pC_map = NULL;
    unsigned int w, h;
    AR_U8* pMemContent;
    AR_U8 *pTmpBuff=AR_NULL;
    AR_U64 phy_addr;
    AR_U32 u32Size, s32Ysize;
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    AR_U32 u32UvHeight;

    if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
    {
        s32Ysize = (pVBuf->u64PhyAddr[1] - pVBuf->u64PhyAddr[0]);
        u32Size = s32Ysize*3/2;
        u32UvHeight = pVBuf->u32Height / 2;
    }
    else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
    {
        s32Ysize = (pVBuf->u64PhyAddr[1] - pVBuf->u64PhyAddr[0]);
        u32Size = s32Ysize*2;
        u32UvHeight = pVBuf->u32Height;
    }
    else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
    {
        u32Size = s32Ysize = (pVBuf->u32Stride[0]) * ALIGN_UP(pVBuf->u32Height, 16);
        u32UvHeight = 0;
    }
    else
    {
        logerr("%s %d: This YUV format is not support!\n",__func__, __LINE__);
        return;
    }

    phy_addr = pVBuf->u64PhyAddr[0];

    pY_map = (AR_U8*) AR_MPI_SYS_Mmap(phy_addr, u32Size);
    if (AR_NULL == pY_map)
    {
        logerr("sample_comm_vdec.c: AR_MPI_SYS_Mmap for pY_map fail!!\n");
        return;
    }

    pC_map = pY_map + s32Ysize;

    fprintf(stderr, "saving......Y......");
    fflush(stderr);
    for (h = 0; h < pVBuf->u32Height; h++)
    {
        pMemContent = pY_map + h * pVBuf->u32Stride[0];
        fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
    }

    if (PIXEL_FORMAT_YUV_400 != enPixelFormat)
    {
        fflush(pfd);
        fprintf(stderr, "U......");
        fflush(stderr);

        pTmpBuff = (AR_U8 *)malloc(pVBuf->u32Stride[0]);
        if (AR_NULL == pTmpBuff)
        {
            logerr("sample_comm_vdec.c: malloc pTmpBuff (size=%d) fail!!!\n",pVBuf->u32Stride[0]);
            return;
        }
        for (h = 0; h < u32UvHeight; h++)
        {
            pMemContent = pC_map + h * pVBuf->u32Stride[1];

            pMemContent += 1;

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                pTmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(pTmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
        fflush(pfd);

        fprintf(stderr, "V......");
        fflush(stderr);
        for (h = 0; h < u32UvHeight; h++)
        {
            pMemContent = pC_map + h * pVBuf->u32Stride[1];

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                pTmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(pTmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
        free(pTmpBuff);
        pTmpBuff = AR_NULL;
    }
    fflush(pfd);

    fprintf(stderr, "done!\n");
    fflush(stderr);

    AR_MPI_SYS_Munmap(pY_map, u32Size);
    pY_map = AR_NULL;

    return;
}
*/

void SAMPLE_COMM_VDEC_SaveRGBFile(FILE* fRgb, VIDEO_FRAME_S* pVB)
{
    AR_U32 i;
    AR_U32 u32Size;
    AR_U8 *pUserPageAddr, *pTmp;
    AR_U32 u32WidthInBytes;
    AR_U32 u32Stride;

    if ( (PIXEL_FORMAT_ARGB_8888 == pVB->enPixelFormat) || (PIXEL_FORMAT_ABGR_8888 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*4;
    }
    else if ( (PIXEL_FORMAT_ARGB_1555 == pVB->enPixelFormat) || (PIXEL_FORMAT_ABGR_1555 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*2;
    }
    else if ( (PIXEL_FORMAT_RGB_888 == pVB->enPixelFormat) || (PIXEL_FORMAT_BGR_888 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*3;
    }
    else if ( (PIXEL_FORMAT_RGB_565 == pVB->enPixelFormat) || (PIXEL_FORMAT_BGR_565 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*2;
    }
    else
    {
        logerr("\033[0;31mThis RGB format is not support!\n\033[0m");
        return;
    }

    u32Stride = ALIGN_UP(u32WidthInBytes, 16);
    u32Size   = u32Stride * ALIGN_UP(pVB->u32Height, 16);

    pUserPageAddr = (AR_U8*) AR_MPI_SYS_Mmap(pVB->u64PhyAddr[0], u32Size);

    if (AR_NULL == pUserPageAddr)
    {
        logerr("\033[0;31mAR_MPI_SYS_Mmap fail!!! u32Size=%d\n\033[0m", u32Size);
        return;
    }

    fprintf(stderr, "saving......RGB..%d x %d......", pVB->u32Width, pVB->u32Height);
    fflush(stderr);

    pTmp = pUserPageAddr;
    for (i = 0; i < pVB->u32Height; i++, pTmp += u32Stride)
    {
        fwrite(pTmp, u32WidthInBytes, 1, fRgb) ;
    }
    fflush(fRgb);

    fprintf(stderr, "done!\n");
    fflush(stderr);

    AR_MPI_SYS_Munmap(pUserPageAddr, u32Size);
    pUserPageAddr = AR_NULL;

    return;
}

AR_VOID SAMPLE_COMM_VDEC_CmdCtrl(AR_S32 s32ChnNum,VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    AR_S32 i, s32Ret;
    VDEC_CHN_STATUS_S stStatus;
    AR_BOOL bVoPause = AR_FALSE;
    AR_CHAR c=0;
    VO_LAYER VoLayer;
    VO_CHN   VoChn;
    VO_VIDEO_LAYER_ATTR_S stVoAttr;
    AR_S32 s32FrameRate = 0;

    struct timeval timeoutVal;
    fd_set read_fds;
    const AR_S32 stdinFd = 0;

    logline("decoding..............");

    while(1)
    {
        if (task_done[pstVdecSend->s32ChnId] == AR_TRUE) {
            break;
        }

        FD_ZERO(&read_fds);
        FD_SET(stdinFd, &read_fds);

        timeoutVal.tv_sec  = 3;
        timeoutVal.tv_usec = 0;//500 * 1000;
        s32Ret = select(stdinFd + 1, &read_fds, NULL, NULL, &timeoutVal);
        if (s32Ret < 0)
        {
            logerr("select on stdin failed, s32Ret = %d, errno = %d(%s)\n", s32Ret, errno, strerror(errno));
            break;
        }
        else if (s32Ret == 0)
        {
            continue;
        }

        s32Ret = read(stdinFd, (void *)(&c), 1);
        if (c == 'e' || s32Ret == 0)
            break;
        else if (c == 'r')
        {
            if (bVoPause == AR_TRUE)
            {
                for (i=0; i<s32ChnNum; i++)
                {
                    pstVdecSend[i].eThreadCtrl = THREAD_CTRL_START;
                }

                for (VoLayer=0; VoLayer<VO_MAX_LAYER_NUM; VoLayer++)
                {
                    for (VoChn=0; VoChn<s32ChnNum; VoChn++)
                    {
                        s32Ret = AR_MPI_VO_ResumeChn(VoLayer, VoChn);
                        if (AR_SUCCESS != s32Ret)
                        {
                            logerr("AR_MPI_VO_ResumeChn(%d, %d) fail for 0x%x!\n", VoLayer, VoChn, s32Ret);
                        }
                    }
                }
                logline("VO Resume!!!");
            }
            bVoPause = AR_FALSE;
        }
        else if (c == 'p')
        {
            if (bVoPause == AR_FALSE)
            {
                for (i=0; i<s32ChnNum; i++)
                {
                    pstVdecSend[i].eThreadCtrl = THREAD_CTRL_PAUSE;
                }

                for (VoLayer=0; VoLayer<VO_MAX_LAYER_NUM; VoLayer++)
                {
                    for (VoChn=0; VoChn<s32ChnNum; VoChn++)
                    {
                        s32Ret = AR_MPI_VO_PauseChn(VoLayer, VoChn);
                        if (AR_SUCCESS != s32Ret)
                        {
                            logerr("AR_MPI_VO_PauseChn(%d, %d) fail for 0x%x!\n", VoLayer, VoChn, s32Ret);
                        }
                    }
                }
                logline("VO Pause...");
            }
            bVoPause = AR_TRUE;
        }

        else if (c == 'a')
        {
            for(i = 0; i < VO_MAX_LAYER_NUM; i++)
            {
                AR_MPI_VO_GetVideoLayerAttr(i, &stVoAttr);
                if (stVoAttr.u32DispFrmRt == 0)
                {
                    continue;
                }
                AR_MPI_VO_GetChnFrameRate(i, 0, &s32FrameRate);
                if (s32FrameRate >= 120)
                {
                    logline("VO FrameRate of chnl 0 of layer %d is larger than 120.", i);
                    continue;
                }
                s32FrameRate += 10;
                AR_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                logline("VO FrameRate of chnl 0 of layer %d is set to %d.", i, s32FrameRate);
             }
        }
        else if (c == 'd')
        {
            for(i = 0; i < 3; i++)
            {
                AR_MPI_VO_GetVideoLayerAttr(i, &stVoAttr);
                if (stVoAttr.u32DispFrmRt == 0)
                {
                    continue;
                }
                AR_MPI_VO_GetChnFrameRate(i, 0, &s32FrameRate);
                if (s32FrameRate < 20)
                {
                    logline("VO FrameRate of chnl 0 of layer %d is less than 10.", i);
                    continue;
                }
                s32FrameRate -= 10;
                AR_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                logline("VO FrameRate of chnl 0 of layer %d is set to %d.", i, s32FrameRate);
             }
        }
        else if (c == 's')
        {
            if (bVoPause == AR_FALSE)
            {
                logline("Firstly press 'p' to pause,then step.");
                continue;
            }

            for (VoLayer=0; VoLayer<VO_MAX_LAYER_NUM; VoLayer++)
            {
                for (VoChn=0; VoChn<s32ChnNum; VoChn++)
                {
                    s32Ret = AR_MPI_VO_StepChn(VoLayer, VoChn);
                    if (AR_SUCCESS != s32Ret)
                    {
                        logerr("AR_MPI_VO_StepChn(%d, %d) fail for 0x%x!\n", VoLayer, VoChn, s32Ret);
                    }
                }
            }
            logline("VO Step.");
        }
        else if (c == 'q')
        {
            for (i=0; i<s32ChnNum; i++)
            {
                AR_MPI_VDEC_QueryStatus(pstVdecSend[i].s32ChnId, &stStatus);
                PRINTF_VDEC_CHN_STATUS(pstVdecSend[i].s32ChnId, stStatus);
            }
        }
    }
    return;
}

AR_VOID SAMPLE_COMM_VDEC_StartSendStream(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    AR_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pVdecThread[i] = 0;
        char threadName[16] = {'\0'};
        sprintf(threadName, "sendstream_%d", i);
        int ret = pthread_create(&pVdecThread[i], 0, SAMPLE_COMM_VDEC_SendStream, (AR_VOID *)&pstVdecSend[i]);

        if (ret) {
            logerr("[%s:%d] pthread_create failed: %s\n", __FUNCTION__, __LINE__, strerror(errno));
            return;
        }

        ret = pthread_setname_np(pVdecThread[i], threadName);

        if (ret) {
            logerr("[%s:%d] pthread_setname_np failed: %s\n", __FUNCTION__, __LINE__, strerror(ret));
        }

        memset(threadName, 0, sizeof(threadName));
        ret = pthread_getname_np(pVdecThread[i], threadName, sizeof(threadName));

        if (!ret)
            logline("threadName is %s\n", threadName);
    }
}

AR_S32 SAMPLE_COMM_VDEC_StopSendStream(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    AR_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecSend[i].eThreadCtrl = THREAD_CTRL_STOP;
        CHECK_CHN_RET(AR_MPI_VDEC_StopRecvStream(pstVdecSend[i].s32ChnId), i, "AR_MPI_VDEC_StopRecvStream");
    }

    for(i=0; i<s32ChnNum; i++)
    {
        if (0 != pVdecThread[i])
        {
            pthread_join(pVdecThread[i], AR_NULL);
            pVdecThread[i] = 0;
        }
    }

    return 0;
}

static void DisplayHex(void *mem, uint32_t len, char* name)
{
    uint32_t   i, j;

    fprintf(stderr, "  addr  : 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F %s \n", name);
    for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++) {
        /* print offset */
        if (i % HEXDUMP_COLS == 0) {
            fprintf(stderr, "0x%06x: ", i);
        }

        /* print hex data */
        if (i < len) {
            fprintf(stderr, "%02x ", 0xFF & ((char*)mem)[i]);
        }
        else /* end of block, just aligning for ASCII dump */ {
            fprintf(stderr, "   ");
        }

        /* print ASCII dump */
        if (i % HEXDUMP_COLS == (HEXDUMP_COLS - 1)) {
            for(j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
                if (j >= len) { /* end of block, not really printing */
                    fprintf(stderr, " ");
                }
                else if (isprint(((char*)mem)[j])) { /* printable char */
                    fprintf(stderr, "%c", 0xFF & ((char*)mem)[j]);
                }
                else { /* other char */
                    fprintf(stderr, ".");
                }
            }
            fprintf(stderr, "\n");
        }
    }

    fprintf(stderr, "\n");
}

AR_VOID * SAMPLE_COMM_VDEC_GetPic(AR_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    FILE *fp = AR_NULL;
    AR_S32 s32Ret, s32Cnt = 0, s32ErrCnt = 0;
    VDEC_CHN_ATTR_S  stAttr = {0};
    VIDEO_FRAME_INFO_S stVFrame = {0};
    AR_CHAR cSaveFile[256];
    VDEC_CHN_STATUS_S stStatus = {0};
    VDEC_USERDATA_S stUserData = {0};
    struct timeval timeoutVal;
    int save_frame_counter = 0;

    prctl(PR_SET_NAME, "VdecGetPic", 0,0,0);

    fd_set read_fds;
    AR_S32 VdecFd = AR_MPI_VDEC_GetFd(pstVdecThreadParam->s32ChnId);
    AR_S32 maxfd = 0;

    if (maxfd <= VdecFd)
        maxfd = VdecFd;

    s32Ret = AR_MPI_VDEC_GetChnAttr(pstVdecThreadParam->s32ChnId, &stAttr);
    if (AR_SUCCESS != s32Ret)
    {
        logerr("chn%d get chn attr fail for %#x!\n", pstVdecThreadParam->s32ChnId, s32Ret);
        return (AR_VOID *)(AR_FAILURE);
    }

    if (pstVdecThreadParam->s32SaveFile == AR_SAVE_YUV_ONE_FILE) {
        SAVE_FILE_NAME(cSaveFile, pstVdecThreadParam->s32ChnId, 0, pstVdecThreadParam->cOutName, 0);
        logline("chn%d cSaveFile:%s, cStreamName:%s, numFrameToDecode:%d\n",
                 pstVdecThreadParam->s32ChnId, cSaveFile,
                 pstVdecThreadParam->cFileName, pstVdecThreadParam->numFrameToDecode);
        if (cSaveFile != 0)
        {
            fp=fopen(cSaveFile,"wb");
            if (fp==NULL)
            {
                logerr("chn%d can't open file %s in get picture thread, %s\n",
                         pstVdecThreadParam->s32ChnId, cSaveFile, strerror(errno));
                return (AR_VOID *)(AR_FAILURE);
            }
        }
    }

    while(1)
    {
        if ((pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP) &&
            (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265))
        {
            break;
        }

        if (s32Cnt >= pstVdecThreadParam->numFrameToDecode || s32ErrCnt > 10) {
            char threadName[128]={0};
            prctl(PR_GET_NAME, threadName);
            logline("chn%d thread %s job done: Count: %d vs numFrameToDecode: %d\n",
                     pstVdecThreadParam->s32ChnId, threadName, s32Cnt, pstVdecThreadParam->numFrameToDecode);
            task_done[pstVdecThreadParam->s32ChnId] = AR_TRUE;
            break;
        }

        FD_ZERO(&read_fds);
        FD_SET(VdecFd, &read_fds);

        timeoutVal.tv_sec  = 1;
        timeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &timeoutVal);
        if (s32Ret < 0)
        {
            logerr("chn%d get_pic select failed!\n", pstVdecThreadParam->s32ChnId);
            break;
        }
        else if (s32Ret == 0)
        {
            logline("chn%d get_pic select time out, wait!\n",
                     pstVdecThreadParam->s32ChnId);
            continue;
        }

        if (FD_ISSET(VdecFd, &read_fds))
        {
            memset(&stStatus, 0, sizeof(VDEC_CHN_STATUS_S));
            s32Ret = AR_MPI_VDEC_QueryStatus(pstVdecThreadParam->s32ChnId, &stStatus);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("chn%d AR_MPI_VDEC_QueryStatus failed, ret = %#x\n",
                        pstVdecThreadParam->s32ChnId, s32Ret);
                //return;
            }

#if 0
            if (stStatus.stVdecDecErr.s32FormatErr            ||
                stStatus.stVdecDecErr.s32PicSizeErrSet        ||
                stStatus.stVdecDecErr.s32StreamUnsprt         ||
                stStatus.stVdecDecErr.s32PackErr              ||
                stStatus.stVdecDecErr.s32PrtclNumErrSet       ||
                stStatus.stVdecDecErr.s32RefErrSet            ||
                stStatus.stVdecDecErr.s32PicBufSizeErrSet     ||
                stStatus.stVdecDecErr.s32StreamSizeOver       ||
                stStatus.stVdecDecErr.s32VdecStreamNotRelease) {
                logerr("chn%d AR_MPI_VDEC_QueryStatus, %d %d %d %d %d %d %d %d\n",
                        pstVdecThreadParam->s32ChnId, stStatus.stVdecDecErr.s32FormatErr, stStatus.stVdecDecErr.s32PicSizeErrSet,
                        stStatus.stVdecDecErr.s32StreamUnsprt, stStatus.stVdecDecErr.s32PackErr, stStatus.stVdecDecErr.s32PrtclNumErrSet,
                        stStatus.stVdecDecErr.s32RefErrSet, stStatus.stVdecDecErr.s32PicBufSizeErrSet, stStatus.stVdecDecErr.s32StreamSizeOver,
                        stStatus.stVdecDecErr.s32VdecStreamNotRelease);
            }
#endif

            if (stStatus.u32LeftPics == 0)
            {
                logerr("Vdec Status: LeftPics=%d, LeftStreams=%d\n",
                        stStatus.u32LeftPics, stStatus.u32LeftStreamFrames);
            }
        }

        for(int i=0; i<stStatus.u32LeftPics; i++)
        {
            memset(&stVFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
            s32Ret=AR_MPI_VDEC_GetFrame(pstVdecThreadParam->s32ChnId, &stVFrame, pstVdecThreadParam->s32MilliSec);

            if (AR_SUCCESS == s32Ret && stVFrame.stVFrame.u64PhyAddr[0])
            {
                ++save_frame_counter;
                logline("chn%d GetFrame ok, type %d, format %d, width %d, height %d, fps = %f, err_ratio = %f, paddr=%llx\n",
                         pstVdecThreadParam->s32ChnId, stVFrame.stVFrame.stExt.s32NalType, stVFrame.stVFrame.enPixelFormat,
                         stVFrame.stVFrame.u32Width, stVFrame.stVFrame.u32Height,
                         1.0 * stVFrame.stVFrame.stExt.s32RateNumerator / stVFrame.stVFrame.stExt.s32RateDenominator,
                         1.0 * stVFrame.stVFrame.stExt.s32NumOfErrMBsInDisplay / stVFrame.stVFrame.stExt.s32NumOfTotMBsInDisplay,
                         stVFrame.stVFrame.u64PhyAddr[0]);
                if (pstVdecThreadParam->s32SaveFile == AR_SAVE_YUV_ONE_FILE)
                    s32Ret = SAMPLE_COMM_VDEC_SaveYUV4xxP_File(fp, save_frame_counter, &stVFrame.stVFrame);
                else if (pstVdecThreadParam->s32SaveFile == AR_SAVE_YUV_MULTI_FILE) {
                    sprintf(cSaveFile, "%s_chn%d_%d.yuv",
                            pstVdecThreadParam->cOutName, pstVdecThreadParam->s32ChnId, save_frame_counter);
                    fp = fopen(cSaveFile, "wb");
                    if (fp) {
                        s32Ret = SAMPLE_COMM_VDEC_SaveYUV4xxP_File(fp, save_frame_counter, &stVFrame.stVFrame);
                        fclose(fp);
                        fp = NULL;
                    }
                }
                else
                    s32Ret = AR_SUCCESS;

                if (s32Ret == AR_SUCCESS){
                    s32Cnt++;
                    logline("chn%d save data success, frame count %d\n",
                             pstVdecThreadParam->s32ChnId, s32Cnt);
                }

                s32Ret=AR_MPI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);
                if (AR_SUCCESS != s32Ret)
                {
                    logerr("\033[0;31mchn%d AR_MPI_VDEC_ReleaseFrame fail for s32Ret=0x%x!\n\033[0m",
                             pstVdecThreadParam->s32ChnId, s32Ret);
                }
            }
            else
            {
                s32ErrCnt++;
                usleep(1000);
            }

            // get userdata
            if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265)
            {
                s32Ret=AR_MPI_VDEC_GetUserData(pstVdecThreadParam->s32ChnId, &stUserData, 0);
                if (AR_SUCCESS == s32Ret && stUserData.bValid)
                {
                    if (pstVdecThreadParam->s32ShowUserData)
                    {
                        DisplayHex(stUserData.pu8Addr, stUserData.u32Len, "UNREGISTERED");
                    }

                    s32Ret=AR_MPI_VDEC_ReleaseUserData(pstVdecThreadParam->s32ChnId, &stUserData);
                }
            }
        }
    }

    if (fp != NULL)
    {
        fclose(fp);
    }

    logline("\033[0;35mchn%d get pic thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);

    return (AR_VOID *)AR_SUCCESS;
}


AR_VOID SAMPLE_COMM_VDEC_StartGetPic(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread)
{
    AR_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pVdecThread[i+TEST_VDEC_MAX_CHN_NUM] = 0;

        pthread_create(&pVdecThread[i+TEST_VDEC_MAX_CHN_NUM], 0, SAMPLE_COMM_VDEC_GetPic, (AR_VOID *)&(pstVdecGet[i]));
    }
}

AR_VOID SAMPLE_COMM_VDEC_StopGetPic(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread)
{
    AR_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecGet[i].eThreadCtrl = THREAD_CTRL_STOP;
        if (0 != pVdecThread[i+TEST_VDEC_MAX_CHN_NUM])
        {
            pthread_join(pVdecThread[i+TEST_VDEC_MAX_CHN_NUM], AR_NULL);
            pVdecThread[i+TEST_VDEC_MAX_CHN_NUM] = 0;
        }
    }
}

AR_S32 SAMPLE_COMM_VDEC_Start(AR_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    AR_S32  i;
    VDEC_CHN_ATTR_S stChnAttr[TEST_VDEC_MAX_CHN_NUM];
    VDEC_CHN_POOL_S stPool;
    VDEC_CHN_PARAM_S stChnParam = {0};
    VDEC_MOD_PARAM_S stModParam = {0};

    CHECK_RET(AR_MPI_VDEC_GetModParam(&stModParam), "AR_MPI_VDEC_GetModParam");

    stModParam.enVdecVBSource = pastSampleVdec[0].vbSource;
#ifdef PROXIMA
    stModParam.stVideoModParam.u32CoreClock = 300;
    stModParam.stPictureModParam.u32CoreClock = 300;
#else
    stModParam.stVideoModParam.u32CoreClock = 500;
    stModParam.stVideoModParam.u32BpuClock = 360;
    stModParam.stPictureModParam.u32CoreClock = 500;
#endif
    stModParam.stEventModParam.u32VdecIrqQueueSize = 256;
    stModParam.stEventModParam.u32VdecTaskQueueSize = 256;
    stModParam.stEventModParam.u32VdecDoneQueueSize = 256;
    stModParam.stEventModParam.u32VdecOutQueueSize = 16;
    stModParam.stEventModParam.u32VdecEventQueueSize = 128;

    CHECK_RET(AR_MPI_VDEC_SetModParam(&stModParam), "AR_MPI_VDEC_SetModParam");

    for(i=0; i<s32ChnNum; i++)
    {
        task_done[i] = AR_FALSE;
        stChnAttr[i].enType           = pastSampleVdec[i].enType;
        stChnAttr[i].enMode           = pastSampleVdec[i].enMode;
        stChnAttr[i].u32PicWidth      = pastSampleVdec[i].u32Width;
        stChnAttr[i].u32PicHeight     = pastSampleVdec[i].u32Height;
        stChnAttr[i].u32FrameBufCnt   = pastSampleVdec[i].u32FrameBufCnt;

        if (PT_H264 == pastSampleVdec[i].enType || PT_H265 == pastSampleVdec[i].enType)
        {
            stChnAttr[i].stVdecVideoAttr.u32RefFrameNum     = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum;
            stChnAttr[i].stVdecVideoAttr.u32CmdQueueDepth   = pastSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth;
            stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 1;
            if ((PT_H264 == pastSampleVdec[i].enType) && (VIDEO_DEC_MODE_IPB != pastSampleVdec[i].stSapmleVdecVideo.enDecMode))
            {
                stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 0;
            }
            stChnAttr[i].u32StreamBufSize = pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height > 1 << 15 ?
                                            pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height : 1 << 16;
            stChnAttr[i].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
        }
        else if (PT_JPEG == pastSampleVdec[i].enType || PT_MJPEG == pastSampleVdec[i].enType)
        {
            stChnAttr[i].enMode           = VIDEO_MODE_FRAME;
            stChnAttr[i].u32StreamBufSize = 10 * 1024 * 1024;//AR_ALIGN1024(pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height) > 1 << 15 ?
                                            //AR_ALIGN1024(pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height) : 1 << 16;
            stChnAttr[i].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat, DATA_BITWIDTH_8, 0);
        }
        CHECK_CHN_RET(AR_MPI_VDEC_CreateChn(pastSampleVdec[i].s32ChnId, &stChnAttr[i]), i, "AR_MPI_VDEC_CreateChn");

        if (VB_SOURCE_USER == pastSampleVdec[i].vbSource)
        {
            stPool.hPicVbPool = g_ahPicVbPool[i];
            stPool.hTmvVbPool = g_ahTmvVbPool[i];
            CHECK_CHN_RET(AR_MPI_VDEC_AttachVbPool(pastSampleVdec[i].s32ChnId, &stPool), i, "AR_MPI_VDEC_AttachVbPool");
        }

        CHECK_CHN_RET(AR_MPI_VDEC_GetChnParam(pastSampleVdec[i].s32ChnId, &stChnParam), i, "AR_MPI_VDEC_GetChnParam");
        if (PT_H264 == pastSampleVdec[i].enType || PT_H265 == pastSampleVdec[i].enType)
        {
            stChnParam.stVdecVideoParam.enDecMode         = pastSampleVdec[i].stSapmleVdecVideo.enDecMode;
            stChnParam.stVdecVideoParam.enCompressMode    = COMPRESS_MODE_NONE;
            stChnParam.stVdecVideoParam.enVideoFormat     = VIDEO_FORMAT_TILE_64x16;
            stChnParam.stVdecVideoParam.enOutputOrder     = pastSampleVdec[i].stSapmleVdecVideo.enOutputOrder;
            stChnParam.stVdecVideoParam.enErrConcealMode  = pastSampleVdec[i].stSapmleVdecVideo.errConcealMode;
            stChnParam.stVdecVideoParam.enErrConcealUnit  = pastSampleVdec[i].stSapmleVdecVideo.errConcealUnit;
        }
        else
        {
            stChnParam.stVdecPictureParam.enPixelFormat   = pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat;
            stChnParam.stVdecPictureParam.u32Alpha        = pastSampleVdec[i].stSapmleVdecPicture.u32Alpha;
        }

        stChnParam.bEnableVbCache                         = pastSampleVdec[i].u32CacheMode;
        stChnParam.bEnableUserData                        = pastSampleVdec[i].bEnableUserData;
        stChnParam.u32MiniBufMode                         = pastSampleVdec[i].u32MiniBufMode;
        stChnParam.u32DisplayFrameNum                     = pastSampleVdec[i].u32DisplayFrameNum;
        stChnParam.u32ScaledWidth                         = pastSampleVdec[i].u32ScaledWidth;
        stChnParam.u32ScaledHeight                        = pastSampleVdec[i].u32ScaledHeight;
        CHECK_CHN_RET(AR_MPI_VDEC_SetChnParam(pastSampleVdec[i].s32ChnId, &stChnParam), i, "AR_MPI_VDEC_SetChnParam");

        CHECK_CHN_RET(AR_MPI_VDEC_StartRecvStream(pastSampleVdec[i].s32ChnId), i, "AR_MPI_VDEC_StartRecvStream");
    }

    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_VDEC_Stop(AR_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    AR_S32 i;

    for(i=0; i<s32ChnNum; i++)
    {
        if (VB_SOURCE_USER == pastSampleVdec[i].vbSource)
            CHECK_CHN_RET(AR_MPI_VDEC_DetachVbPool(pastSampleVdec[i].s32ChnId), i, "AR_MPI_VDEC_DetachVbPool");

        CHECK_CHN_RET(AR_MPI_VDEC_DestroyChn(pastSampleVdec[i].s32ChnId), i, "AR_MPI_VDEC_DestroyChn");
    }

    return AR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
