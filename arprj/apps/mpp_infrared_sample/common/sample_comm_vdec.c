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

static long getTimeNowMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

VB_POOL g_ahPicVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};
VB_POOL g_ahTmvVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};

int task_done[VDEC_MAX_CHN_NUM] = {0};
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
    SAMPLE_VDEC_BUF astSampleVdecBuf[VDEC_MAX_CHN_NUM];
    VB_POOL_CONFIG_S stVbPoolCfg;

    memset(astSampleVdecBuf, 0, sizeof(SAMPLE_VDEC_BUF)*VDEC_MAX_CHN_NUM);
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

    if (VB_SOURCE_USER == pastSampleVdec[0].vbSource)
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
                logerr("AR_MPI_VB_DestroyPool %d fail!",g_ahPicVbPool[i]);
            }
            g_ahPicVbPool[i] = VB_INVALID_POOLID;
        }
        if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
        {
            s32Ret = AR_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
            if (AR_SUCCESS != s32Ret)
            {
                logerr("AR_MPI_VB_DestroyPool %d fail!",g_ahTmvVbPool[i]);
            }
            g_ahTmvVbPool[i] = VB_INVALID_POOLID;
        }
    }
    return AR_FAILURE;
}

AR_VOID SAMPLE_COMM_VDEC_ExitVBPool(SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    AR_S32 i, s32Ret;

    if (VB_SOURCE_USER == pastSampleVdec[0].vbSource)
    {
        for (i=VB_MAX_POOLS-1; i>=0; i--)
        {
            if (VB_INVALID_POOLID != g_ahPicVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahPicVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    logerr("AR_MPI_VB_DestroyPool %d fail!",g_ahPicVbPool[i]);
                }
                g_ahPicVbPool[i] = VB_INVALID_POOLID;
            }
            if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    logerr("AR_MPI_VB_DestroyPool %d fail!",g_ahTmvVbPool[i]);
                }
                g_ahTmvVbPool[i] = VB_INVALID_POOLID;
            }
        }
    }

    return;
}

#if 0
static AR_S32 SAMPLE_COMM_VDEC_SaveYUV420_File(FILE* pfd, VIDEO_FRAME_S* pVBuf)
{
    AR_S32 s32Ret = -1;
    if (pfd != NULL)
    {
        AR_U32 width = pVBuf->u32Width;
        AR_U32 height = pVBuf->u32Height;
        AR_U32 stride0 = pVBuf->u32Stride[0];
        AR_U32 stride1 = pVBuf->u32Stride[1];
        AR_U32 stride2 = pVBuf->u32Stride[2];
        AR_U32 y_size = pVBuf->u64PhyAddr[1] - pVBuf->u64PhyAddr[0];
        AR_U32 u_size = pVBuf->u64PhyAddr[2] - pVBuf->u64PhyAddr[1];
        AR_U32 v_size = u_size;
        AR_U32 size = y_size + u_size + v_size;

        char *pData = (char *)malloc(sizeof(char)*size);
        if (pData == NULL){
            logerr("malloc %d bytes failed!\n", size);
            return AR_FAILURE;
        }

        void *virt_addr = AR_MPI_SYS_Mmap(pVBuf->u64PhyAddr[0], size);
        if (virt_addr)
        {
            memcpy(pData, virt_addr, size);
            char *p = pData;
            char *y_virt_addr = p;
            logline( "saving......Y......\n");
            fflush(stderr);
            for(int i = 0; i < height; i++){
                s32Ret = fwrite(y_virt_addr, 1, width, pfd);
                if (s32Ret < width)
                    logerr("short write, %d vs %d\n", width, s32Ret);
                y_virt_addr += stride0;
            }
            fflush(pfd);
            p+=y_size;

            char *u_virt_addr = p;
            logline( "saving......U......\n");
            fflush(stderr);
            for(int i = 0; i < height/2; i++){
                s32Ret = fwrite(u_virt_addr, 1, width/2, pfd);
                if (s32Ret < width / 2)
                    logerr("short write, %d vs %d\n", width / 2, s32Ret);
                u_virt_addr += stride1;
            }
            fflush(pfd);
            p+=u_size;

            char *v_virt_addr = p;
            logline( "saving......V......\n");
            fflush(stderr);
            for(int i = 0; i < height/2; i++){
                s32Ret = fwrite(v_virt_addr, 1, width / 2, pfd);
                if (s32Ret < width / 2)
                    logerr("short write, %d vs %d\n",  width / 2, s32Ret);
                v_virt_addr += stride2;
            }
            fflush(pfd);
            logline( "save yuv data done !\n");
            fflush(stderr);
        } else {
            logerr("mapped virtual address is NULL!\n");
            return AR_FAILURE;
        }
        AR_MPI_SYS_Munmap(virt_addr, size);
        free(pData);
    }
    return AR_SUCCESS;
}

#else
static AR_S32 SAMPLE_COMM_VDEC_SaveYUV420_File(FILE* pfd, VIDEO_FRAME_S* pVBuf)
{
    AR_U8 *pVirAddr_Y = (AR_U8 *) pVBuf->u64VirAddr[0];
    AR_U8 *pVirAddr_U = (AR_U8 *) pVBuf->u64VirAddr[1];
    AR_U8 *pVirAddr_V = (AR_U8 *) pVBuf->u64VirAddr[2];

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
    for (int i = 0; i < pVBuf->u32Height/2; i++)
    {
        fwrite(pVirAddr_U, pVBuf->u32Width/2, 1, pfd);
        pVirAddr_U += pVBuf->u32Stride[1];
    }
    fflush(pfd);

    fprintf(stderr, "saving......V......\n");
    fflush(stderr);
    for (int i = 0; i < pVBuf->u32Height/2; i++)
    {
        fwrite(pVirAddr_V, pVBuf->u32Width/2, 1, pfd);
        pVirAddr_V += pVBuf->u32Stride[2];
    }
    fflush(pfd);

    fprintf(stderr, "save yuv data done !\n");
    fflush(stderr);

    return AR_SUCCESS;
}
#endif

#if 1
AVFormatContext *read_stream_info(char *stream_file, int* video_index, double* fps)
{
    int ret = 0;
    bool fps_flag = false;
    AVFormatContext *fmtCtx = avformat_alloc_context();

    fmtCtx->flags |= AV_CODEC_FLAG_TRUNCATED;
    if ((ret = avformat_open_input(&fmtCtx, stream_file, 0, 0)) < 0) {
        logerr("open %s file failed", stream_file);
        return NULL;
    }

    if ((ret = avformat_find_stream_info(fmtCtx, 0)) < 0) {
        logerr("read %s stream info failed", stream_file);
        return NULL;
    }

    for (int i = 0; i < fmtCtx->nb_streams; i++){
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            *video_index = i;
            break;
        }
    }

    fps_flag = fmtCtx->streams[*video_index]->avg_frame_rate.den &&
               fmtCtx->streams[*video_index]->avg_frame_rate.num;
    if (fps_flag) {
        *fps = av_q2d(fmtCtx->streams[*video_index]->avg_frame_rate);
    }

    av_dump_format(fmtCtx, 0, stream_file, 0);

    return fmtCtx;
}

AR_VOID * SAMPLE_COMM_VDEC_SendStream(AR_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam = (VDEC_THREAD_PARAM_S *)pArgs;
    AR_BOOL bEndOfStream = AR_FALSE;
    AR_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm = NULL;
    FILE *fp_dump = NULL;
    AR_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    AR_BOOL bFindStart, bFindEnd;
    AR_U64 u64PTS = 0;
    AR_U32 u32Len /*, u32Start*/;
    AR_S32 s32Ret, i;
    AR_CHAR cStreamFile[256];
    int send_frame_counter = 0;
    uint64_t tm = 0;
    int ret;
    int video_index = -1;
    double fps;
    uint64_t delay_ms = 0;
    uint64_t frame_rate_ms;
    AVPacket *packet = NULL;
    AVFormatContext *fmtCtx = NULL;
    if (pstVdecThreadParam->u32Fps)
        frame_rate_ms = 1000/pstVdecThreadParam->u32Fps;

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
        fmtCtx = read_stream_info(cStreamFile, &video_index, &fps);
        if (fmtCtx == NULL) {
            logerr("chn%d read %s stream info failed\n",
                    pstVdecThreadParam->s32ChnId, cStreamFile);
            return (AR_VOID *)(AR_FAILURE);
        }

        if (fps)
            frame_rate_ms = 1000/fps;
        packet = av_packet_alloc();
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

        if (task_done[pstVdecThreadParam->s32ChnId] == 1)
        {
            printf("\033[0;36mchn%d exit SendStream thread\033[0;39m\n", pstVdecThreadParam->s32ChnId);
            break;
        }

        if (pstVdecThreadParam->enType == PT_H264 ||
            pstVdecThreadParam->enType == PT_H265) {
            ret = av_read_frame(fmtCtx, packet);
            if (ret != 0) // eos
            {
                printf("\033[0;32mchn%d replay...\033[0;39m\n", pstVdecThreadParam->s32ChnId);
                av_packet_unref(packet);
                if (pstVdecThreadParam->bCircleSend == AR_TRUE)
                {
                    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
                    ret = av_seek_frame(fmtCtx, video_index, 0, AVSEEK_FLAG_BACKWARD);
                    if (ret < 0) {
                        avformat_close_input(&fmtCtx);
                        avformat_free_context(fmtCtx);
                        fmtCtx = read_stream_info(cStreamFile, &video_index, &fps);
                        if (fmtCtx == NULL) {
                            logerr("chn%d read %s stream info failed\n",
                                     pstVdecThreadParam->s32ChnId, cStreamFile);
                            return (AR_VOID *)(AR_FAILURE);
                        }

                        if (fps)
                            frame_rate_ms = 1000/fps;
                    }
                    ret = av_read_frame(fmtCtx, packet);
                }
                else
                {
                    break;
                }
            }
            stStream.u64PTS   = packet->pts == AV_NOPTS_VALUE ? u64PTS : packet->pts;
            stStream.pu8Addr  = packet->data;
            stStream.u32Len   = packet->size;
            stStream.bDisplay = 1;
        }
        else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG)
        {
            bEndOfStream = AR_FALSE;
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

            if (bFindStart == AR_FALSE)
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
            stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? AR_TRUE: AR_FALSE;
            stStream.bEndOfStream = bEndOfStream;
            stStream.bDisplay     = 1;
        }

        tm = ar_get_linux_time();

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

        if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
            logline("chn%d send %d(%s) frame, len = %d, pts = %lld\n",
                     pstVdecThreadParam->s32ChnId, send_frame_counter, packet->flags ? "I" : "B/P",
                     stStream.u32Len, stStream.u64PTS);
        } else {
            logline("chn%d send frame %d, len = %d\n",
                     pstVdecThreadParam->s32ChnId, send_frame_counter, stStream.u32Len);
        }

        if (fp_dump != NULL) {
            fwrite(stStream.pu8Addr, stStream.u32Len, 1, fp_dump);
        }

SendAgain:
        s32Ret=AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if ((AR_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl))
        {
            pstVdecThreadParam->s32IntervalTime += 10;
            if (s32Ret != AR_ERR_VDEC_BUF_FULL)
                logerr("chn%d AR_MPI_VDEC_SendStream %dth faield, ret=%d send again.\n",
                        pstVdecThreadParam->s32ChnId, send_frame_counter, s32Ret);
            else {
                usleep(2*1000);
            }
            goto SendAgain;
        }
        else
        {
            send_frame_counter++;
            bEndOfStream = AR_FALSE;
            s32UsedBytes = s32UsedBytes +s32ReadLen;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
            delay_ms = ar_get_linux_time() - tm;
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

    if (pstVdecThreadParam->enType == PT_H264 ||
        pstVdecThreadParam->enType == PT_H265) {
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        av_packet_free(&packet);
    } else if (pstVdecThreadParam->enType == PT_MJPEG ||
               pstVdecThreadParam->enType == PT_JPEG) {
        if (pu8Buf != AR_NULL)
        {
            free(pu8Buf);
        }
        fclose(fpStrm);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
    stStream.u64PTS       = u64PTS;
    stStream.bEndOfStream = AR_TRUE;
    AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    if (pstVdecThreadParam->bCircleSend == AR_FALSE) {
        task_done[pstVdecThreadParam->s32ChnId] = true;
    }

    logline("\033[0;35mchn%d send EOS and return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);

    return (AR_VOID *)AR_SUCCESS;
}
#else
AR_VOID * SAMPLE_COMM_VDEC_SendStream(AR_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    AR_BOOL bEndOfStream = AR_FALSE;
    AR_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm=NULL;
    FILE *fp_dump = NULL;
    AR_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    AR_BOOL bFindStart, bFindEnd;
    AR_U64 u64PTS = 0;
    AR_U32 u32Len/*, u32Start*/;
    AR_S32 s32Ret,  i;
    AR_CHAR cStreamFile[256];
    int send_frame_counter = 0;
    uint64_t tm = 0;
    uint64_t delay_ms = 0;
    uint64_t frame_rate_ms;
    frame_rate_ms = 1000/pstVdecThreadParam->u32Fps;

    prctl(PR_SET_NAME, "VideoSendStream", 0,0,0);

    snprintf(cStreamFile, sizeof(cStreamFile), pstVdecThreadParam->cFileName);

    logline("chn%d cStreamFile=%s, cOutFile=%s\n",
             pstVdecThreadParam->s32ChnId, cStreamFile, pstVdecThreadParam->cOutName);
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
    fflush(stdout);
#if 0
    char dumpfile[128] = "dumpfile.h265";
    fp_dump = fopen(dumpfile,"wb");
#endif
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

        if (task_done[pstVdecThreadParam->s32ChnId] == 1) {
            printf("\033[0;36mchn%d exit SendStream thread\033[0;39m\n", pstVdecThreadParam->s32ChnId);
            break;
        }

        bEndOfStream = AR_FALSE;
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

        if (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME && pstVdecThreadParam->enType == PT_H264)
        {
            for (i=0; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                   (((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                    (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)))
                {
                    bFindStart = AR_TRUE;
                    i += 8;
                    break;
                }
            }

            for (; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                   (tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
                    ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                    (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)))
                {
                    bFindEnd = AR_TRUE;
                    break;
                }
            }

            if (i>0)s32ReadLen = i - 1;
            if (bFindStart == AR_FALSE)
            {
                logerr("chn%d can not find H264 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == AR_FALSE)
            {
                s32ReadLen = i+8;
            }

        }
        else if (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME
            && pstVdecThreadParam->enType == PT_H265)
        {
            AR_BOOL  bNewPic = AR_FALSE;

#if 1 // Here we put PPS/SPS in the first frame
            for (i = 0; i < s32ReadLen-6; i++)
            {
                AR_U32 tmp = (pu8Buf[i+3]&0x7E)>>1;
                bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
                            && (tmp >= 0 && tmp <= 21) && ((pu8Buf[i+5]&0x80) == 0x80) );

                if (bNewPic)
                {
                    bFindStart = AR_TRUE;
                    i += 6;
                    break;
                }
            }

            for (; i < s32ReadLen-6; i++)
            {
                AR_U32 tmp = (pu8Buf[i+3]&0x7E)>>1;
                bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
                            &&( tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 || ((tmp >= 0 && tmp <= 21) && (pu8Buf[i+5]&0x80) == 0x80) )
                             );

                if (bNewPic)
                {
                    bFindEnd = AR_TRUE;
                    break;
                }
            }
            if (i>0)s32ReadLen = i - 1;

            if (bFindStart == AR_FALSE)
            {
                logerr("chn%d can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == AR_FALSE)
            {
                s32ReadLen = i+6;
            }
#else // extractor NAL
            for (i=0; i<s32ReadLen-6; i++) {
                if (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 0 && pu8Buf[i+3] == 1 && pu8Buf[i+5] == 1) {
                    AR_U32 nalType = (pu8Buf[i + 4] & 0x7E) >> 1;

                    //logline("found nalType %d at %#x\n", nalType, i);

                    if ((nalType >= 0 && nalType <= 21) || nalType == 39 || nalType == 40) {
                        bFindStart = AR_TRUE;
                        //logline("found start nalu at %#x, type=%d\n", i, nalType);
                        i += 6;
                        break;
                    }

                    i += 6;
                } else if (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+4] == 1) {
                    AR_U32 nalType = (pu8Buf[i + 3] & 0x7E) >> 1;

                    //logline("found nalType %d at %#x\n", nalType, i);

                    if ((nalType >= 0 && nalType <= 21) || nalType == 39 || nalType == 40) {
                        bFindStart = AR_TRUE;
                        //logline("found start nalu at %#x, type=%d\n", i, nalType);
                        i += 5;
                        break;
                    }

                    i += 5;
                }
            }

            for (; i<s32ReadLen-6; i++) {
                AR_U32 tmp = (pu8Buf[i + 4] & 0x7E) >> 1;

                bNewPic = ( pu8Buf[i+0] == 0 &&
                            pu8Buf[i+1] == 0 &&
                            pu8Buf[i+2] == 0 &&
                            pu8Buf[i+3] == 1 &&
                            (   tmp == 32/*VPS*/ ||
                                tmp == 33/*SPS*/ ||
                                tmp == 34/*PPS*/ ||
                                tmp == 39/*Prefix SEI*/ ||
                                tmp == 40/*Suffix SEI*/ ||
                                ( (tmp >= 0 && tmp <= 21) && (pu8Buf[i+5] == 1) )
                            )
                          );

                if (bNewPic) {
                    //logline("found end nalu at %#x, type=%d\n", i, tmp);
                    bFindEnd = AR_TRUE;
                    break;
                }
            }
            if (i>0)s32ReadLen = i;

            if (bFindStart == AR_FALSE)
            {
                logline("sample_comm_vdec.c: chn %d can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                    pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == AR_FALSE)
            {
                s32ReadLen = i+6;
            }
#endif
        }
        else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG)
        {
            for (i=0; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    /*
                    u32Start = i;
                    */
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

            if (bFindStart == AR_FALSE)
            {
                logerr("chn%d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
        }
        else
        {
            if ((s32ReadLen != 0) && (s32ReadLen < pstVdecThreadParam->s32MinBufSize))
            {
                bEndOfStream = AR_TRUE;
            }
        }

        stStream.u64PTS       = u64PTS;
        stStream.pu8Addr      = pu8Buf;
        stStream.u32Len       = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? AR_TRUE: AR_FALSE;
        stStream.bEndOfStream = bEndOfStream;
        stStream.bDisplay     = 1;

        if (fp_dump != NULL){
            fwrite(stStream.pu8Addr, stStream.u32Len, 1, fp_dump);
        }

SendAgain:
        tm = ar_get_linux_time();
        s32Ret=AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if ( (AR_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl) )
        {
            pstVdecThreadParam->s32IntervalTime += 10;
            if (s32Ret != AR_ERR_VDEC_BUF_FULL)
                logerr("chn%d AR_MPI_VDEC_SendStream %dth faield, ret=%d send again.\n",
                        pstVdecThreadParam->s32ChnId, send_frame_counter, s32Ret);
            else {
                usleep(2*1000);
            }
            goto SendAgain;
        }
        else
        {
            send_frame_counter++;
            if (send_frame_counter%10 == 0)
                logline("chd%d sendstream counter=%d, len=%d\n",
                         pstVdecThreadParam->s32ChnId, send_frame_counter, stStream.u32Len);
            bEndOfStream = AR_FALSE;
            s32UsedBytes = s32UsedBytes +s32ReadLen;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
            delay_ms = ar_get_linux_time() - tm;  /* impossible overflow*/
            delay_ms = (delay_ms < frame_rate_ms) ? (frame_rate_ms - delay_ms) : 1;
            pstVdecThreadParam->s32IntervalTime = delay_ms * 1000;
        }

        usleep(pstVdecThreadParam->s32IntervalTime);
    }

    if (fp_dump != NULL){
        fclose(fp_dump);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = AR_TRUE;
    AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    logline("\033[0;35mchn%d send EOS and return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != AR_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (AR_VOID *)AR_SUCCESS;
}
#endif

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
    char c=0;
    VO_LAYER VoLayer;
    VO_CHN     VoChn;
    VO_VIDEO_LAYER_ATTR_S stVoAttr;
    AR_S32 s32FrameRate = 0;

    struct timeval TimeoutVal;
    fd_set read_fds;
    const AR_S32 stdinFd = 0;

    logline("decoding..............");

    while(1)
    {
        if (task_done[pstVdecSend->s32ChnId] == 1)
            break;

        FD_ZERO(&read_fds);
        FD_SET(stdinFd, &read_fds);

        TimeoutVal.tv_sec  = 3;
        TimeoutVal.tv_usec = 0;//500 * 1000;
        s32Ret = select(stdinFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
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

AR_VOID SAMPLE_COMM_VDEC_StopSendStream(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    AR_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecSend[i].eThreadCtrl = THREAD_CTRL_STOP;
        CHECK_CHN_RET(AR_MPI_VDEC_StopRecvStream(pstVdecSend[i].s32ChnId), i, "AR_MPI_VDEC_StopRecvStream");
        if (0 != pVdecThread[i])
        {
            pthread_join(pVdecThread[i], AR_NULL);
            pVdecThread[i] = 0;
        }
    }
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
    struct timeval TimeoutVal;
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
        logline("chn%d cSaveFile:%s, s32ChnId:%d, cStreamName:%s, numFrameToDecode:%d\n",
                 pstVdecThreadParam->s32ChnId, cSaveFile, pstVdecThreadParam->s32ChnId,
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
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
        {
            break;
        }

        if (task_done[pstVdecThreadParam->s32ChnId] == 1) {
            printf("\033[0;36mchn%d exit GetPic thread\033[0;39m\n", pstVdecThreadParam->s32ChnId);
            break;
        }

        if (s32Cnt >= pstVdecThreadParam->numFrameToDecode || s32ErrCnt > 10) {
            char threadName[128]={0};
            prctl(PR_GET_NAME, threadName);
            logline("chn%d thread %s job done: s32Cnt: %d vs numFrameToDecode: %d\n",
                     pstVdecThreadParam->s32ChnId, threadName, s32Cnt, pstVdecThreadParam->numFrameToDecode);
            task_done[pstVdecThreadParam->s32ChnId] = 1;
            break;
        }

        FD_ZERO(&read_fds);
        FD_SET(VdecFd, &read_fds);

        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
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
                logerr("chn%d AR_MPI_VDEC_QueryStatus fail, ret = %d!!!\n",
                        pstVdecThreadParam->s32ChnId, s32Ret);
                //return;
            }
            if (stStatus.u32LeftPics == 0 )
            {
                logerr("status left stream frames:u32LeftPics=%d, u32LeftStreamFrames=%d\n",
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
                logline("chn%d GetFrame ok, format %d, width %d, height %d, paddr=%llx\n",
                         pstVdecThreadParam->s32ChnId, stVFrame.stVFrame.enPixelFormat,
                         stVFrame.stVFrame.u32Width, stVFrame.stVFrame.u32Height,
                         stVFrame.stVFrame.u64PhyAddr[0]);
                if (pstVdecThreadParam->s32SaveFile == AR_SAVE_YUV_ONE_FILE)
                    s32Ret = SAMPLE_COMM_VDEC_SaveYUV420_File(fp, &stVFrame.stVFrame);
                else if (pstVdecThreadParam->s32SaveFile == AR_SAVE_YUV_MULTI_FILE) {
                    sprintf(cSaveFile, "%s_chn%d_%d.yuv",
                            pstVdecThreadParam->cOutName, pstVdecThreadParam->s32ChnId, save_frame_counter);
                    fp = fopen(cSaveFile, "wb");
                    if (fp) {
                        s32Ret = SAMPLE_COMM_VDEC_SaveYUV420_File(fp, &stVFrame.stVFrame);
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
                if (AR_SUCCESS == s32Ret && stUserData.bValid && pstVdecThreadParam->s32ShowUserData)
                {
                    DisplayHex(stUserData.pu8Addr, stUserData.u32Len, "UNREGISTERED");

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
        pVdecThread[i+VDEC_MAX_CHN_NUM] = 0;

        pthread_create(&pVdecThread[i+VDEC_MAX_CHN_NUM], 0, SAMPLE_COMM_VDEC_GetPic, (AR_VOID *)&(pstVdecGet[i]));
    }
}

AR_VOID SAMPLE_COMM_VDEC_StopGetPic(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread)
{
    AR_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecGet[i].eThreadCtrl = THREAD_CTRL_STOP;
        if (0 != pVdecThread[i+VDEC_MAX_CHN_NUM])
        {
            pthread_join(pVdecThread[i+VDEC_MAX_CHN_NUM], AR_NULL);
            pVdecThread[i+VDEC_MAX_CHN_NUM] = 0;
        }
    }
}

AR_S32 SAMPLE_COMM_VDEC_Start(AR_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    AR_S32  i;
    VDEC_CHN_ATTR_S stChnAttr[VDEC_MAX_CHN_NUM];
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
    CHECK_RET(AR_MPI_VDEC_SetModParam(&stModParam), "AR_MPI_VDEC_SetModParam");

    for(i=0; i<s32ChnNum; i++)
    {
        task_done[i] = 0;
        stChnAttr[i].enType           = pastSampleVdec[i].enType;
        stChnAttr[i].enMode           = pastSampleVdec[i].enMode;
        stChnAttr[i].u32PicWidth      = pastSampleVdec[i].u32Width;
        stChnAttr[i].u32PicHeight     = pastSampleVdec[i].u32Height;
        stChnAttr[i].u32FrameBufCnt   = pastSampleVdec[i].u32FrameBufCnt;

        if (PT_H264 == pastSampleVdec[i].enType || PT_H265 == pastSampleVdec[i].enType)
        {
            stChnAttr[i].stVdecVideoAttr.u32RefFrameNum     = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum;
            stChnAttr[i].stVdecVideoAttr.u32CmdQueueDepth     = pastSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth;
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
            stChnAttr[i].u32StreamBufSize = AR_ALIGN1024(pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height) > 1 << 15 ?
                                            AR_ALIGN1024(pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height) : 1 << 16;
            stChnAttr[i].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat, DATA_BITWIDTH_8, 0);
        }
        stChnAttr[i].stVdecVideoAttr.u32CmdQueueDepth = pastSampleVdec->stSapmleVdecVideo.u32CmdQueueDepth;
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
        }
        else
        {
            stChnParam.stVdecPictureParam.enPixelFormat   = pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat;
            stChnParam.stVdecPictureParam.u32Alpha        = pastSampleVdec[i].stSapmleVdecPicture.u32Alpha;
        }
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
        CHECK_CHN_RET(AR_MPI_VDEC_DestroyChn(pastSampleVdec[i].s32ChnId), i, "AR_MPI_VDEC_DestroyChn");
    }

    return AR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
