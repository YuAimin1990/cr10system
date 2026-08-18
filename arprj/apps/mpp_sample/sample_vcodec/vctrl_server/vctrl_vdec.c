#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

//copy from hisi sample
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "mpi_vb.h"
#include "mpi_vdec.h"
#include "mpi_venc.h"
#include "vctrl_vdec.h"
#include "vctrl_venc.h"
#include "ar_comm_vb.h"
#include "vctrl_msg.h"
#include "mpi_sys.h"
#include "ar_buffer.h"
#include "hal_vcodec_type_comm.h"
//#include "sample_comm.h"

#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>

#define DISPLAY_NUM 5
typedef struct {
    AR_U32 index;
    AR_U64 pa[DISPLAY_NUM + 1];
} SAMPLE_VDEC_INFO;
SAMPLE_VDEC_INFO g_frameInfo[VDEC_MAX_CHN_NUM] = {0};

static VB_SOURCE_E  g_enVdecVBSource  = VB_SOURCE_MODULE;

static VB_POOL g_ahPicVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};
static VB_POOL g_ahTmvVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};
static VDEC_THREAD_PARAM_S gstVdecThreadParam[VDEC_MAX_CHN_NUM] = { 0 };
static pthread_t vdecSendThreads[VDEC_MAX_CHN_NUM];
static pthread_t vdecGetThreads[VDEC_MAX_CHN_NUM];
static int task_done[VDEC_MAX_CHN_NUM] = {0};
static pthread_mutex_t gLock = PTHREAD_MUTEX_INITIALIZER;

#define AR_ALIGN1024(_x) (((_x)+0x3ff)&~0x3ff)
#define PRINTF_VDEC_CHN_STATUS(Chn, stStatus) \
do {\
    printf("\033[0;33m ---------------------------------------------------------------------------------------------------\033[0;39m\n");\
    printf("\033[0;33m chn%d, Type:%d, bStart:%d, DecodeFrames:%d, LeftPics:%d, LeftBytes:%d, LeftFrames:%d, RecvFrames:%d  \033[0;39m\n",\
        Chn,\
        stStatus.enType,\
        stStatus.bStartRecvStream,\
        stStatus.u32DecodeStreamFrames,\
        stStatus.u32LeftPics,\
        stStatus.u32LeftStreamBytes,\
        stStatus.u32LeftStreamFrames,\
        stStatus.u32RecvStreamFrames);\
    printf("\033[0;33m FormatErr:%d,    s32PicSizeErrSet:%d,  s32StreamUnsprt:%d,  s32PackErr:%d,  u32PrtclNumErrSet:%d,  s32RefErrSet:%d,  s32PicBufSizeErrSet:%d  \033[0;39m\n",\
        stStatus.stVdecDecErr.s32FormatErr,\
        stStatus.stVdecDecErr.s32PicSizeErrSet,\
        stStatus.stVdecDecErr.s32StreamUnsprt,\
        stStatus.stVdecDecErr.s32PackErr,\
        stStatus.stVdecDecErr.s32PrtclNumErrSet,\
        stStatus.stVdecDecErr.s32RefErrSet,\
        stStatus.stVdecDecErr.s32PicBufSizeErrSet);\
    printf("\033[0;33m ---------------------------------------------------------------------------------------------------\033[0;39m\n");\
}while(0)

#define SAVE_FILE_NAME(aFileName, chn, s32Cnt, cStreamName, enPixelFormat)\
do {\
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
    else if ((enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420) \
          ||(enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422) \
          ||(enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_420) \
          ||(enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_422) \
          ||(enPixelFormat == PIXEL_FORMAT_YUV_400))\
    {\
        Postfix = "yuv";\
    }\
    else\
    {\
        printf("[%s]-%d: enPixelFormat type err", __FUNCTION__, __LINE__);\
        return (AR_VOID *)(AR_FAILURE);\
    }\
    snprintf(aFileName, sizeof(aFileName), "%s_chn%d_%d.%s", cStreamName, chn, s32Cnt, Postfix);\
}while(0)

AR_S32 VCTRL_VDEC_InitVBPool(AR_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    VB_CONFIG_S stVbConf;
    AR_S32 i, j, pos=0, s32Ret;
    AR_BOOL bFindFlag;
    SAMPLE_VDEC_BUF astSampleVdecBuf[VDEC_MAX_CHN_NUM];
    VB_POOL_CONFIG_S stVbPoolCfg;

    memset(astSampleVdecBuf, 0, sizeof(SAMPLE_VDEC_BUF)*VDEC_MAX_CHN_NUM);
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    for(i = 0; i < ChnNum; i++)
    {
        if (PT_H265 == pastSampleVdec[i].enType)
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
            astSampleVdecBuf[i].u32TmvBufSize = VDEC_GetTmvBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height);
        }
        else if (PT_H264 == pastSampleVdec[i].enType)
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
            if (VIDEO_DEC_MODE_IPB == pastSampleVdec[i].stSapmleVdecVideo.enDecMode)
            {
                astSampleVdecBuf[i].u32TmvBufSize = VDEC_GetTmvBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height);
            }
        }
        else
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                    pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat, DATA_BITWIDTH_8, 0);
        }
    }

    /* PicBuffer */
    for(j=0; j<VB_MAX_COMM_POOLS; j++)
    {
        bFindFlag = AR_FALSE;
        for(i = 0; i < ChnNum; i++)
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
        for(i = 0; i < ChnNum; i++)
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

    if (VB_SOURCE_MODULE == g_enVdecVBSource)
    {
        AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
        CHECK_RET(AR_MPI_VB_SetModPoolConfig(VB_UID_VDEC, &stVbConf), "AR_MPI_VB_SetModPoolConfigig");
        s32Ret = AR_MPI_VB_InitModCommPool(VB_UID_VDEC);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VB_InitModCommPool fail for 0x%x\n", s32Ret);
            AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
            return AR_FAILURE;
        }
    }
    else if (VB_SOURCE_USER == g_enVdecVBSource)
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
                    printf("AR_MPI_VB_DestroyPool %d fail!\n",g_ahPicVbPool[i]);
                }
                g_ahPicVbPool[i] = VB_INVALID_POOLID;
            }
            if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    printf("AR_MPI_VB_DestroyPool %d fail!\n",g_ahTmvVbPool[i]);
                }
                g_ahTmvVbPool[i] = VB_INVALID_POOLID;
            }
        }
        return AR_FAILURE;
}

AR_VOID VCTRL_VDEC_ExitVBPool(AR_VOID)
{
    AR_S32 i, s32Ret;

    if (VB_SOURCE_MODULE == g_enVdecVBSource)
    {
        AR_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    }
    else if (VB_SOURCE_USER == g_enVdecVBSource)
    {
        for (i = VB_MAX_POOLS-1; i>=0; i--)
        {
            if (VB_INVALID_POOLID != g_ahPicVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahPicVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    printf("AR_MPI_VB_DestroyPool %d fail!\n",g_ahPicVbPool[i]);
                }
                g_ahPicVbPool[i] = VB_INVALID_POOLID;
            }
            if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
            {
                s32Ret = AR_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
                if (AR_SUCCESS != s32Ret)
                {
                    printf("AR_MPI_VB_DestroyPool %d fail!\n",g_ahTmvVbPool[i]);
                }
                g_ahTmvVbPool[i] = VB_INVALID_POOLID;
            }
        }
    }

    return;
}

AVFormatContext *read_stream_info(char *stream_file, int* video_index, double* fps)
{
    int ret = 0;
    bool fps_flag = false;
    AVFormatContext *fmtCtx = avformat_alloc_context();

    fmtCtx->flags |= AV_CODEC_FLAG_TRUNCATED;
    if ((ret = avformat_open_input(&fmtCtx, stream_file, 0, 0)) < 0) {
        logline("\033[0;31mopen %s file failed\033[0m", stream_file);
        return NULL;
    }

    if ((ret = avformat_find_stream_info(fmtCtx, 0)) < 0) {
        logline("\033[0;31mread %s stream info failed\033[0m", stream_file);
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

AR_VOID * VCTRL_VDEC_SendStream(AR_VOID *pArgs)
{
#if 0
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    AR_BOOL bEndOfStream = AR_FALSE;
    AR_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm=NULL;
    AR_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    AR_BOOL bFindStart, bFindEnd;
    AR_U64 u64PTS = 0;
    AR_U32 u32Len, u32Start;
    AR_S32 s32Ret,  i;
    AR_U32 sendCount = 0;
    char threadName[16]={0};
    sprintf(threadName, "VdecSendStrm_%d", pstVdecThreadParam->s32ChnId);
    prctl(PR_SET_NAME, threadName, 0,0,0);

    printf("\033[0;35m chn%d send steam thread start ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    if (strlen(pstVdecThreadParam->cInFileName) != 0)
    {
        fpStrm = fopen(pstVdecThreadParam->cInFileName, "rb");
        if (fpStrm == NULL)
        {
            VCTRL_PRT("chn%d can't open file %s in send stream thread!\n",
                       pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cInFileName);
            return (AR_VOID *)(AR_FAILURE);
        }
    }
    printf("\n\033[0;36m chn%d, stream file:%s, userbufsize: %d \033[0;39m\n",
            pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cInFileName, pstVdecThreadParam->s32MinBufSize);

    pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
    if (pu8Buf == NULL)
    {
        VCTRL_PRT("chn%d can't alloc %d in send stream thread!\n",
                   pstVdecThreadParam->s32ChnId, pstVdecThreadParam->s32MinBufSize);
        fclose(fpStrm);
        return (AR_VOID *)(AR_FAILURE);
    }
    fflush(stdout);

    u64PTS = pstVdecThreadParam->u64PtsInit;
    while (1)
    {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_PAUSE)
        {
            usleep(30 * 1000);
            continue;
        }

        if (task_done[pstVdecThreadParam->s32ChnId] == 1) {
            logline("task %d done, exit thread", pstVdecThreadParam->s32ChnId);
            break;
        }

        bEndOfStream = AR_FALSE;
        bFindStart   = AR_FALSE;
        bFindEnd     = AR_FALSE;
        u32Start     = 0;
        fseek(fpStrm, s32UsedBytes, SEEK_SET);
        s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
        if (s32ReadLen == 0)
        {
            if (1 || pstVdecThreadParam->bCircleSend == AR_TRUE)
            {
                memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
                /*
                stStream.bEndOfStream = AR_TRUE;
                AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);
                */

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

            if(i>0)s32ReadLen = i - 1;
            if (bFindStart == AR_FALSE)
            {
                VCTRL_PRT("chn%d can not find H264 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
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
            for (i = 0; i < s32ReadLen-6; i++)
            {
                AR_U32 tmp = (pu8Buf[i+3]&0x7E)>>1;
                bNewPic = ( pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
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
                bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                           (tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 ||
                           ((tmp >= 0 && tmp <= 21) && (pu8Buf[i+5]&0x80) == 0x80)));

                if (bNewPic)
                {
                    bFindEnd = AR_TRUE;
                    break;
                }
            }
            if (i>0)s32ReadLen = i - 1;

            if (bFindStart == AR_FALSE)
            {
                VCTRL_PRT("chn%d can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                    pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == AR_FALSE)
            {
                s32ReadLen = i+6;
            }

        }
        else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG)
        {
            for (i = 0; i < s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    u32Start = i;
                    bFindStart = AR_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i < s32ReadLen-3; i++)
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

            for (; i < s32ReadLen-1; i++)
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
                VCTRL_PRT("chn%d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
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
        stStream.pu8Addr      = pu8Buf + u32Start;
        stStream.u32Len       = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? AR_TRUE: AR_FALSE;
        stStream.bEndOfStream = bEndOfStream;
        stStream.bDisplay     = 1;

SendAgain:
        s32Ret=AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);

        if ( (AR_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl) )
        {
            usleep(30 * 1000);
            goto SendAgain;
        }
        else
        {
            bEndOfStream = AR_FALSE;
            s32UsedBytes = s32UsedBytes +s32ReadLen + u32Start;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
            sendCount++;
            logline("chn%d sendstream success, cnt=%u", pstVdecThreadParam->s32ChnId, sendCount);
        }
        usleep(pstVdecThreadParam->s32IntervalTime);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = AR_TRUE;
    AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    printf("\033[0;35m chn%d send steam thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != AR_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (AR_VOID *)AR_SUCCESS;
#else
    VCHN_INFO* pChnInfo = NULL;
    VDEC_THREAD_PARAM_S *pstVdecThreadParam = NULL;
    AR_BOOL bEndOfStream = AR_FALSE;
    AR_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm = NULL;
    FILE *fp_dump = NULL;
    AR_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream = { 0 };
    AR_BOOL bFindStart, bFindEnd;
    AR_U64 u64PTS = 0;
    AR_U32 u32Len /*, u32Start*/;
    AR_S32 s32Ret, i;
    AR_CHAR cStreamFile[256];
    int send_frame_counter = 0;
    static AR_S32 send_count = 0;
    uint64_t tm = 0;
    int ret;
    int video_index = -1;
    double fps;
    uint64_t delay_ms = 0;
    uint64_t frame_rate_ms;
    pChnInfo =  (VCHN_INFO *)pArgs;
    pstVdecThreadParam = &gstVdecThreadParam[pChnInfo->chnId];
    if (pstVdecThreadParam->u32Fps)
        frame_rate_ms = 1000 / pstVdecThreadParam->u32Fps;
    else {
        pstVdecThreadParam->u32Fps = 25;
        frame_rate_ms = 1000 / pstVdecThreadParam->u32Fps;
    }


    prctl(PR_SET_NAME, "VideoSendStream", 0,0,0);
    snprintf(cStreamFile, sizeof(cStreamFile), pstVdecThreadParam->cInFileName);
    printf("cStreamFile=%s, cOutFile=%s\n", cStreamFile, pstVdecThreadParam->cOutFileName);
    fflush(stdout);

#if 0
    char dumpfile[128] = "dumpfile.h265";
    fp_dump = fopen(dumpfile,"wb");
#endif

    AVPacket *packet = NULL;
    AVFormatContext *fmtCtx = NULL;
    av_log_set_level(AV_LOG_QUIET);
    if (pstVdecThreadParam->enType == PT_H264 ||
        pstVdecThreadParam->enType == PT_H265) {
        fmtCtx = read_stream_info(cStreamFile, &video_index, &fps);
        if (fmtCtx == NULL) {
            VCTRL_PRT("\033[0;31mchn%d read %s stream info failed\n\033[0m",
                     pstVdecThreadParam->s32ChnId, cStreamFile);
            return (AR_VOID *)(AR_FAILURE);
        }

        if (fps)
            frame_rate_ms = 1000/fps;
        packet = av_packet_alloc();
    } else if (pstVdecThreadParam->enType == PT_MJPEG ||
               pstVdecThreadParam->enType == PT_JPEG) {
        if (cStreamFile != 0) {
            fpStrm = fopen(cStreamFile, "rb");
            if (fpStrm == NULL) {
                VCTRL_PRT("\033[0;31mchn%d can't open file %s in send stream thread, %s\n\033[0m",
                         pstVdecThreadParam->s32ChnId, cStreamFile, strerror(errno));
                return (AR_VOID *)(AR_FAILURE);
            }
        }
        VCTRL_PRT("\033[0;36m chn %d, stream file:%s, userbufsize: %d \033[0;39m\n",
                 pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cInFileName, pstVdecThreadParam->s32MinBufSize);

        pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
        if(pu8Buf == NULL) {
            VCTRL_PRT("\033[0;31mchn%d can't alloc %d in send stream thread, %s\n\033[0m",
                     pstVdecThreadParam->s32ChnId, pstVdecThreadParam->s32MinBufSize, strerror(errno));
            fclose(fpStrm);
            return (AR_VOID *)(AR_FAILURE);
        }
    }

    u64PTS = pstVdecThreadParam->u64PtsInit;
    while (1) {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP) {
            break;
        } else if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_PAUSE) {
            sleep(1);
            continue;
        }

        if (task_done[pstVdecThreadParam->s32ChnId] == 1) {
            VCTRL_PRT("task %d done, exit thread", pstVdecThreadParam->s32ChnId);
            break;
        }

        if (pstVdecThreadParam->enType == PT_H264 ||
            pstVdecThreadParam->enType == PT_H265) {
            ret = av_read_frame(fmtCtx, packet);
            if (ret != 0) { // eos
                VCTRL_PRT("replay...\n");
                av_packet_unref(packet);
                if (pstVdecThreadParam->bCircleSend == AR_TRUE) {
                    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
                    ret = av_seek_frame(fmtCtx, video_index, 0, AVSEEK_FLAG_BACKWARD);
                    if (ret < 0) {
                        avformat_close_input(&fmtCtx);
                        avformat_free_context(fmtCtx);
                        fmtCtx = read_stream_info(cStreamFile, &video_index, &fps);
                        if (fmtCtx == NULL) {
                            VCTRL_PRT("\033[0;31mchn%d read %s stream info failed\n\033[0m",
                                     pstVdecThreadParam->s32ChnId, cStreamFile);
                            return (AR_VOID *)(AR_FAILURE);
                        }

                        if (fps)
                            frame_rate_ms = 1000/fps;
                    }
                    ret = av_read_frame(fmtCtx, packet);
                } else {
                    break;
                }
            }
            stStream.u64PTS   = packet->pts == AV_NOPTS_VALUE ? u64PTS : packet->pts;
            stStream.pu8Addr  = packet->data;
            stStream.u32Len   = packet->size;
            stStream.bEndOfStream = bEndOfStream;
            stStream.bDisplay = 1;
        } else if (pstVdecThreadParam->enType == PT_MJPEG ||
            pstVdecThreadParam->enType == PT_JPEG) {
            bEndOfStream = AR_FALSE;
            bFindStart   = AR_FALSE;
            bFindEnd     = AR_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0) {
                if (pstVdecThreadParam->bCircleSend == AR_TRUE) {
                    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                } else {
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

            if (bFindStart == AR_FALSE) {
                VCTRL_PRT("\033[0;31mchn%d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n\033[0m",
                         pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            stStream.u64PTS       = u64PTS;
            stStream.pu8Addr      = pu8Buf;
            stStream.u32Len       = s32ReadLen;
            stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? AR_TRUE: AR_FALSE;
            stStream.bEndOfStream = bEndOfStream;
            stStream.bDisplay     = 1;
        }

        if (packet) {
            VCTRL_PRT("send frame %d(%d), len = %d, pts = %lld",
                     send_count, packet->flags, stStream.u32Len, stStream.u64PTS);
        }

        if (fp_dump != NULL) {
            fwrite(stStream.pu8Addr, stStream.u32Len, 1, fp_dump);
        }

SendAgain:
        tm = ar_get_linux_time();
        s32Ret = AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if ((AR_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl)) {
            pstVdecThreadParam->s32IntervalTime += 10;
            if (s32Ret != AR_ERR_VDEC_BUF_FULL)
                VCTRL_PRT("AR_MPI_VDEC_SendStream %dth faield, ret=%d send again.\n", send_frame_counter, s32Ret);
            else {
                usleep(2*1000);
            }
            goto SendAgain;
        } else {
            send_count++;
            send_frame_counter++;
            //if(send_frame_counter%10 == 0)
            //    logline("chn%d sendstream counter=%d, len=%d\n",
            //             pstVdecThreadParam->s32ChnId, send_frame_counter, stStream.u32Len);
            bEndOfStream = AR_FALSE;
            s32UsedBytes = s32UsedBytes +s32ReadLen;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
            delay_ms = ar_get_linux_time() - tm;
            delay_ms = (delay_ms < frame_rate_ms) ? (frame_rate_ms - delay_ms) : 1;
            pstVdecThreadParam->s32IntervalTime = delay_ms * 1000;
        }
        usleep(pstVdecThreadParam->s32IntervalTime);
        /*
        av_packet_unref(packet);
        */
        if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
            if (!packet && !fmtCtx)
                av_packet_unref(packet);
        }
    }

    if (fp_dump != NULL){
        fclose(fp_dump);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S));
    stStream.bEndOfStream = AR_TRUE;
    AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    if (pstVdecThreadParam->enType == PT_H264 ||
        pstVdecThreadParam->enType == PT_H265) {
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        av_packet_free(&packet);
    } else if (pstVdecThreadParam->enType == PT_MJPEG ||
               pstVdecThreadParam->enType == PT_JPEG) {
        if (pu8Buf != AR_NULL) {
            free(pu8Buf);
        }
        fclose(fpStrm);
    }

    send_count = 0;
    VCTRL_PRT("\033[0;35m chn %d send EOS and return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);

    return (AR_VOID *)AR_SUCCESS;

#endif
}

static void VCTRL_VDEC_SaveYUVFile_Linear8Bit(FILE* pfd, VIDEO_FRAME_S* pVBuf)
{
    AR_S32 s32Ret;
    AR_U8 *pVirAddr_Y = (AR_U8 *)pVBuf->u64VirAddr[0];
    AR_U8 *pVirAddr_U = (AR_U8 *)pVBuf->u64VirAddr[1];
    AR_U8 *pVirAddr_V = (AR_U8 *)pVBuf->u64VirAddr[2];

    logline("saving......Y......\n");
    fflush(stderr);
    for (int i = 0; i < pVBuf->u32Height; i++)
    {
        s32Ret = fwrite(pVirAddr_Y, pVBuf->u32Width, 1, pfd);
        if (s32Ret < 1) {
            if (ferror(pfd)) {
                perror("fwrite error");
            }

            logline("short write\n");
            return;
        }
        pVirAddr_Y += pVBuf->u32Stride[0];
    }
    fflush(pfd);

    logline("saving......U......\n");
    fflush(stderr);
    for (int i = 0; i < pVBuf->u32Height/2; i++)
    {
        s32Ret = fwrite(pVirAddr_U, pVBuf->u32Width/2, 1, pfd);
        if (s32Ret < 1) {
            if (ferror(pfd)) {
                perror("fwrite error");
            }

            logline("short write\n");
            return;
        }
        pVirAddr_U += pVBuf->u32Stride[1];
    }
    fflush(pfd);

    logline("saving......V......\n");
    fflush(stderr);
    for (int i = 0; i < pVBuf->u32Height/2; i++)
    {
        s32Ret = fwrite(pVirAddr_V, pVBuf->u32Width/2, 1, pfd);
        if (s32Ret < 1) {
            if (ferror(pfd)) {
                perror("fwrite error");
            }

            logline("short write\n");
            return;
        }
        pVirAddr_V += pVBuf->u32Stride[2];
    }
    fflush(pfd);

    fflush(stderr);
}

void VCTRL_VDEC_SaveRGBFile(FILE* fRgb, VIDEO_FRAME_S* pVB)
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
        printf("%s %d: This RGB format is not support!\n",__func__, __LINE__);
        return;
    }

    u32Stride = ALIGN_UP(u32WidthInBytes, 16);
    u32Size   = u32Stride * ALIGN_UP(pVB->u32Height, 16);

    pUserPageAddr = (AR_U8*) AR_MPI_SYS_Mmap(pVB->u64PhyAddr[0], u32Size);

    if (AR_NULL == pUserPageAddr)
    {
        printf("%s %d:AR_MPI_SYS_Mmap fail!!! u32Size=%d\n",__func__, __LINE__,u32Size);
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

AR_VOID VCTRL_VDEC_CreateEncoder(VCHN_INFO* pChnInfo)
{
    AR_S32 i;
    AR_S32 s32Ret;
    VENC_CHN VencChn = 0;
    VENC_CHN_ATTR_S stAttr = {0};
    VENC_RECV_PIC_PARAM_S stParam = {0};
    VENC_GOP_ATTR_S stGopAttr = { 0 };

    s32Ret = VCTRL_VENC_GetGopAttr(pChnInfo->enGopMode, &stGopAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = VCTRL_VENC_InitAttr(&stAttr, pChnInfo, &stGopAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("VCTRL_VENC_InitAttr[%d] faild with %#x! ===\n", VencChn, s32Ret);
        return s32Ret;
    }

    VencChn = pChnInfo->transcodingChnId;//pastSampleVdec[i].s32ChnId;
    s32Ret = AR_MPI_VENC_CreateChn(VencChn, &stAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_CreateChn [%d] faild with %x\n", VencChn, s32Ret);
        return;
    }

    stParam.s32RecvPicNum = -1;
    s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn, &stParam);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_StartRecvPic faild with %x\n", s32Ret);
        return;
    }
}

AR_VOID VCTRL_VDEC_StartEncoder(VENC_CHN VencChn, VIDEO_FRAME_INFO_S* pVFrame)
{
    AR_S32 s32Ret;
    AR_S32 index;

    pVFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    s32Ret = AR_MPI_VENC_SendFrame(VencChn, pVFrame, -1);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_SendFrame faild with %x\n", s32Ret);
    } else {
        index = g_frameInfo[VencChn].index;
        g_frameInfo[VencChn].pa[index] = pVFrame->stVFrame.u64PhyAddr[0];
        g_frameInfo[VencChn].index = (index + 1) % (DISPLAY_NUM + 1);
    }
}

/*Deprecated*/
AR_VOID VCTRL_VDEC_CmdCtrl(AR_S32 u32ChnNum,VDEC_THREAD_PARAM_S *pstVdecSend, VCHN_INFO* pChnInfo)
{
    AR_S32 i, s32Ret, s32Done_no = 0;
    VDEC_CHN_STATUS_S stStatus;
    AR_BOOL bVoPause = AR_FALSE;
    char c=0;
    struct timeval timeoutVal;
    fd_set read_fds;
    const AR_S32 stdinFd = 0;

    while(1)
    {
        //printf("\nSAMPLE_TEST:press 'e' to exit; 'p' to pause; 'r' to resume; 'q' to query; 's' to step; 'a' to add; 'd' to sub!;\n");
        for (i = 0; i < u32ChnNum; i++) {
            if (task_done[pstVdecSend->s32ChnId] == 1)
                s32Done_no++;
        }

        if (u32ChnNum == s32Done_no)
            break;

        if (pChnInfo->u32State == STATE_STOP)
            break;

        FD_ZERO(&read_fds);
        FD_SET(stdinFd, &read_fds);

        timeoutVal.tv_sec  = 3;
        timeoutVal.tv_usec = 0;//500 * 1000;
        s32Ret = select(stdinFd + 1, &read_fds, NULL, NULL, &timeoutVal);
        if (s32Ret < 0)
        {
            logline("select on stdin failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            continue;
        }

        s32Ret = read(0, (void *)(&c), 1);

        if (c == 'e') {
            for (i = 0; i < u32ChnNum; i++)
            {
                pstVdecSend[i].eThreadCtrl = THREAD_CTRL_STOP;
            }
            break;
        }
        else if (c == 'r')
        {
            if (bVoPause == AR_TRUE)
            {
                for (i = 0; i < u32ChnNum; i++)
                {
                    pstVdecSend[i].eThreadCtrl = THREAD_CTRL_START;
                }
#if 0
                for (VoLayer=0; VoLayer<VO_MAX_LAYER_NUM; VoLayer++)
                {
                    for (VoChn=0; VoChn<u32ChnNum; VoChn++)
                    {
                        s32Ret = AR_MPI_VO_ResumeChn(VoLayer, VoChn);
                        if (AR_SUCCESS != s32Ret)
                        {
                            printf("AR_MPI_VO_ResumeChn(%d, %d) fail for 0x%x!\n", VoLayer, VoChn, s32Ret);
                        }
                    }
                }
#endif
                printf("VO Resume!!!");
            }
            bVoPause = AR_FALSE;
        }
        else if (c == 'p')
        {
            if (bVoPause == AR_FALSE)
            {
                for (i = 0; i < u32ChnNum; i++)
                {
                    pstVdecSend[i].eThreadCtrl = THREAD_CTRL_PAUSE;
                }
#if 0
                for (VoLayer=0; VoLayer<VO_MAX_LAYER_NUM; VoLayer++)
                {
                    for (VoChn=0; VoChn<u32ChnNum; VoChn++)
                    {
                        s32Ret = AR_MPI_VO_PauseChn(VoLayer, VoChn);
                        if (AR_SUCCESS != s32Ret)
                        {
                            printf("AR_MPI_VO_PauseChn(%d, %d) fail for 0x%x!\n", VoLayer, VoChn, s32Ret);
                        }
                    }
                }
#endif
                printf("VO Pause...");
            }
            bVoPause = AR_TRUE;
        }

        else if (c == 'a')
        {
#if 0
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
                    printf("VO FrameRate of chnl 0 of layer %d is larger than 120.", i);
                    continue;
                }
                s32FrameRate += 10;
                AR_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                printf("VO FrameRate of chnl 0 of layer %d is set to %d.", i, s32FrameRate);
             }
#endif
        }
        else if (c == 'd')
        {
#if 0
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
                    printf("VO FrameRate of chnl 0 of layer %d is less than 10.", i);
                    continue;
                }
                s32FrameRate -= 10;
                AR_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                printf("VO FrameRate of chnl 0 of layer %d is set to %d.", i, s32FrameRate);
             }
#endif
        }
        else if (c == 's')
        {
            if (bVoPause == AR_FALSE)
            {
                printf("Firstly press 'p' to pause,then step.");
                continue;
            }
#if 0
            for (VoLayer=0; VoLayer<VO_MAX_LAYER_NUM; VoLayer++)
            {
                for (VoChn=0; VoChn<u32ChnNum; VoChn++)
                {
                    s32Ret = AR_MPI_VO_StepChn(VoLayer, VoChn);
                    if (AR_SUCCESS != s32Ret)
                    {
                        printf("AR_MPI_VO_StepChn(%d, %d) fail for 0x%x!\n", VoLayer, VoChn, s32Ret);
                    }
                }
            }
#endif
            printf("VO Step.");
        }
        else if (c == 'q')
        {
            for (i = 0; i < u32ChnNum; i++)
            {
                AR_MPI_VDEC_QueryStatus(pstVdecSend[i].s32ChnId, &stStatus);
                PRINTF_VDEC_CHN_STATUS(pstVdecSend[i].s32ChnId, stStatus);
            }
        }
    }
    return;
}

AR_VOID VCTRL_VDEC_StartSendStream(VDEC_CHN VdecChn, VCHN_INFO* pChnInfo, pthread_t *pVdecThread)
{
    pVdecThread[VdecChn] = 0;
    pthread_create(&pVdecThread[VdecChn], 0, VCTRL_VDEC_SendStream, (AR_VOID *)pChnInfo);
}

AR_VOID VCTRL_VDEC_StopSendStream(VDEC_CHN VdecChn, VCHN_INFO* pChnInfo, pthread_t *pVdecThread)
{
    VDEC_THREAD_PARAM_S *pstVdecSend = NULL;
    pstVdecSend = &gstVdecThreadParam[VdecChn];
    pstVdecSend->eThreadCtrl = THREAD_CTRL_STOP;
    if (0 != pVdecThread[VdecChn]) {
        pthread_join(pVdecThread[VdecChn], AR_NULL);
        pVdecThread[VdecChn] = 0;
    }
}

AR_VOID * VCTRL_VDEC_GetPic(AR_VOID *pArgs)
{
#if 0
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    FILE *fp = AR_NULL;
    AR_S32 s32Ret, s32Cnt = 0;
    VDEC_CHN_ATTR_S  stAttr = {0};
    VIDEO_FRAME_INFO_S stVFrame = {0};
    AR_CHAR cSaveFile[256];
    struct timeval timeoutVal;
    VDEC_CHN_STATUS_S stStatus = {0};
    AR_U32 getFrameCounter = 0, releaseCounter = 0;
    char threadName[16]={0};
    sprintf(threadName, "VdecGetPic_%d", pstVdecThreadParam->s32ChnId);

    fd_set read_fds;
    AR_S32 VdecFd = AR_MPI_VDEC_GetFd(pstVdecThreadParam->s32ChnId);
    AR_S32 maxfd = 0;

    if(maxfd <= VdecFd)
        maxfd = VdecFd;

    prctl(PR_SET_NAME, threadName, 0,0,0);

    printf("\033[0;35m chn%d get pic thread start ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    s32Ret = AR_MPI_VDEC_GetChnAttr(pstVdecThreadParam->s32ChnId, &stAttr);

    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("chn%d get chn stAttr fail for %#x!\n", pstVdecThreadParam->s32ChnId, s32Ret);
        return (AR_VOID *)(AR_FAILURE);
    }

#if 0
    if (PT_H265 != stAttr.enType && PT_H264 != stAttr.enType)
    {
        VCTRL_PRT("chn%d enType %d do not suport save file!\n", pstVdecThreadParam->s32ChnId, stAttr.enType);
        return (AR_VOID *)(AR_FAILURE);
    }
#endif

    SAVE_FILE_NAME(cSaveFile, pstVdecThreadParam->s32ChnId, 0, pstVdecThreadParam->cOutFileName, PIXEL_FORMAT_YVU_PLANAR_420);

    if (cSaveFile != 0 && fp == NULL)
    {
        fp=fopen(cSaveFile,"wa");

        if (fp==NULL)
        {
            VCTRL_PRT("chn%d can't open file %s in get picture thread!\n", pstVdecThreadParam->s32ChnId, cSaveFile);
            return (AR_VOID *)(AR_FAILURE);
        }

        printf("\033[0;34m chn%d saving yuv file:%s \033[0;39m \n",pstVdecThreadParam->s32ChnId, cSaveFile);
    }

    while(1)
    {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
        {
            break;
        }

        if (task_done[pstVdecThreadParam->s32ChnId] == 1) {
            logline("task %d done, exit thread", pstVdecThreadParam->s32ChnId);
            break;
        }

        if (s32Cnt >= pstVdecThreadParam->u32PicNum) {
            char threadName[128]={0};
            prctl(PR_GET_NAME, threadName);
            logline("thread %s job done: s32Cnt: %d vs numFrameToDecode: %d\n",
                     threadName, s32Cnt, pstVdecThreadParam->u32PicNum);
            task_done[pstVdecThreadParam->s32ChnId] = 1;
            break;
        }

#ifndef AR_FEAT_MPP_SERVICE_LIB
        FD_ZERO(&read_fds);
        FD_SET(VdecFd, &read_fds);
        timeoutVal.tv_sec  = 1;
        timeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &timeoutVal);
        if (s32Ret < 0)
        {
            logline("sample_comm_vdec.c: select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            logline("sample_comm_vdec.c: get vdec stream time out, exit thread\n");
            continue;
        }

        if (FD_ISSET(VdecFd, &read_fds))
        {
            memset(&stStatus, 0, sizeof(VDEC_CHN_STATUS_S));
            s32Ret = AR_MPI_VDEC_QueryStatus(pstVdecThreadParam->s32ChnId, &stStatus);
            //PRINTF_VDEC_CHN_STATUS(pstVdecThreadParam->s32ChnId, stStatus);
            //logline("AR_MPI_VDEC_QueryStatus ok,  stStatus.u32LeftPics=%d,u32LeftStreamFrames=%d, ret=%d\n",stStatus.u32LeftPics,stStatus.u32LeftStreamFrames, s32Ret);

            if(s32Ret != AR_SUCCESS)
            {
                logline("chn %d AR_MPI_VDEC_QueryStatus fail!!!\n",s32Ret);
                //return;
            }
            if(stStatus.u32LeftPics > 0 )
            {
                //logline("status left stream frames:u32LeftPics=%d, u32LeftStreamFrames=%d\n", stStatus.u32LeftPics, stStatus.u32LeftStreamFrames);
                //break;
            }
#else
        if (1)
        {
            while (1) {
                memset(&stStatus, 0, sizeof(VDEC_CHN_STATUS_S));
                s32Ret = AR_MPI_VDEC_QueryStatus(pstVdecThreadParam->s32ChnId, &stStatus);
                if(s32Ret != AR_SUCCESS)
                {
                    printf("\033[0;31mchn%d AR_MPI_VDEC_QueryStatus fail, ret = %d!!!\n\033[0m",
                             pstVdecThreadParam->s32ChnId, s32Ret);
                    return -1;
                }

                if(stStatus.u32LeftPics > 0 )
                {
                    break;
                }

                usleep(2 * 1000);
            }
#endif

            for(int i=0; i<stStatus.u32LeftPics; i++)
            {
                memset(&stVFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
                s32Ret=AR_MPI_VDEC_GetFrame(pstVdecThreadParam->s32ChnId, &stVFrame, pstVdecThreadParam->s32MilliSec);

                if (AR_SUCCESS == s32Ret)
                {
                    if (pstVdecThreadParam->transcoding)
                    {
                        VCTRL_VDEC_StartEncoder(pstVdecThreadParam->s32ChnId, &stVFrame);
                    }
                    else
                    {
                        s32Cnt++;

                        if ((PIXEL_FORMAT_YVU_SEMIPLANAR_420 != stVFrame.stVFrame.enPixelFormat) &&
                            (PIXEL_FORMAT_YVU_SEMIPLANAR_422 != stVFrame.stVFrame.enPixelFormat) &&
                            (PIXEL_FORMAT_YVU_PLANAR_420 != stVFrame.stVFrame.enPixelFormat) &&
                            (PIXEL_FORMAT_YVU_PLANAR_422 != stVFrame.stVFrame.enPixelFormat) &&
                            (PIXEL_FORMAT_YUV_400 != stVFrame.stVFrame.enPixelFormat))
                        {
                            VCTRL_VDEC_SaveRGBFile(fp, &stVFrame.stVFrame);
                        } else if ( (stVFrame.stVFrame.enCompressMode == COMPRESS_MODE_NONE) &&
                                 (stVFrame.stVFrame.enVideoFormat     == VIDEO_FORMAT_LINEAR) )
                        {
                            if (pstVdecThreadParam->s32SaveFile == 1)
                                VCTRL_VDEC_SaveYUVFile_Linear8Bit(fp, &stVFrame.stVFrame);
                            logline("save chn%d %dth yuv data done !\n", pstVdecThreadParam->s32ChnId, getFrameCounter);
                            getFrameCounter++;
                        }
                        else
                        {
                            VCTRL_PRT("chn%d enPixelFormat %d do not suport save file!\n", pstVdecThreadParam->s32ChnId, stVFrame.stVFrame.enPixelFormat);
                            s32Ret=AR_MPI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);

                            if (AR_SUCCESS != s32Ret) {
                                VCTRL_PRT("chn%d AR_MPI_VDEC_ReleaseFrame fail for s32Ret=0x%x!\n", pstVdecThreadParam->s32ChnId, s32Ret);
                            }

                            break;
                        }

                        s32Ret=AR_MPI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);

                        if (AR_SUCCESS != s32Ret)
                        {
                            VCTRL_PRT("chn%d: releaseframe fail", pstVdecThreadParam->s32ChnId);
                        }
                        else
                        {
                            releaseCounter++;
                        }

                        logline("chn%d: getframe: %d, releaseframe: %d, diff: %d, pa:%p\n",
                                pstVdecThreadParam->s32ChnId, s32Cnt, releaseCounter,
                                s32Cnt - releaseCounter, stVFrame.stVFrame.u64PhyAddr[0]);
                    }
                } else {
                    usleep(10 * 1000);
                }
            }
        }
    }

    if (fp != NULL)
    {
        fclose(fp);
    }
    printf("\033[0;35m chn%d get pic thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);

    return (AR_VOID *)AR_SUCCESS;
#else
    VCHN_INFO* pChnInfo = NULL;
    VDEC_THREAD_PARAM_S *pstVdecThreadParam = NULL;
    FILE *fp = AR_NULL;
    AR_S32 s32Ret, u32PictureCnt = 0, s32ErrCnt = 0;
    VDEC_CHN_ATTR_S  stAttr = { 0 };
    VIDEO_FRAME_INFO_S stVFrame = { 0 };
    AR_CHAR cSaveFile[256];
    VDEC_CHN_STATUS_S stStatus = { 0 };
    VDEC_USERDATA_S stUserData = { 0 };
    struct timeval TimeoutVal;
    int save_frame_counter = 0;
    pChnInfo =  (VCHN_INFO *)pArgs;
    pstVdecThreadParam = &gstVdecThreadParam[pChnInfo->chnId];

    prctl(PR_SET_NAME, "VdecGetPic", 0, 0, 0);

    fd_set read_fds;
    AR_S32 VdecFd = AR_MPI_VDEC_GetFd(pstVdecThreadParam->s32ChnId);
    AR_S32 maxfd = 0;

    if(maxfd <= VdecFd)
        maxfd = VdecFd;

    s32Ret = AR_MPI_VDEC_GetChnAttr(pstVdecThreadParam->s32ChnId, &stAttr);
    if (AR_SUCCESS != s32Ret) {
        logline("\033[0;31mchn%d get chn attr fail for %#x!\n\033[0m", pstVdecThreadParam->s32ChnId, s32Ret);
        return (AR_VOID *)(AR_FAILURE);
    }

    if (!pstVdecThreadParam->transcoding) {
        SAVE_FILE_NAME(cSaveFile, pstVdecThreadParam->s32ChnId, 0, pstVdecThreadParam->cOutFileName, PIXEL_FORMAT_YVU_PLANAR_420);
        logline("cSaveFile:%s, s32ChnId:%d, cStreamName:%s\n",
                cSaveFile, pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cInFileName);
        if (cSaveFile != 0 && pstVdecThreadParam->s32SaveFile == 1) {
            fp = fopen(cSaveFile,"wb");
            if (fp==NULL) {
                logline("\033[0;31mchn%d can't open file %s in get picture thread, %s\n\033[0m",
                         pstVdecThreadParam->s32ChnId, cSaveFile, strerror(errno));
                return (AR_VOID *)(AR_FAILURE);
            }
        }
    }

    while (1) {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP) {
            break;
        }

        if (task_done[pstVdecThreadParam->s32ChnId] == 1) {
            logline("task %d done, exit thread", pstVdecThreadParam->s32ChnId);
            break;
        }

        if (s32ErrCnt > 10) {
            char threadName[128]={0};
            prctl(PR_GET_NAME, threadName);
            logline("thread %s job done: u32PictureCnt: %d vs \n", threadName, u32PictureCnt);
            task_done[pstVdecThreadParam->s32ChnId] = 1;
            break;
        }

            memset(&stStatus, 0, sizeof(VDEC_CHN_STATUS_S));
            s32Ret = AR_MPI_VDEC_QueryStatus(pstVdecThreadParam->s32ChnId, &stStatus);
            if (s32Ret != AR_SUCCESS) {
                logline("\033[0;31mchn%d AR_MPI_VDEC_QueryStatus fail, ret = %d!!!\n\033[0m",
                         pstVdecThreadParam->s32ChnId, s32Ret);
                //return;
            }

            if (stStatus.u32LeftPics > 0 ) {
                //logline("status left stream frames:u32LeftPics=%d, u32LeftStreamFrames=%d\n", stStatus.u32LeftPics, stStatus.u32LeftStreamFrames);
                //break;
            }

            for (int i = 0; i < stStatus.u32LeftPics; i++) {
                memset(&stVFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
                s32Ret = AR_MPI_VDEC_GetFrame(pstVdecThreadParam->s32ChnId, &stVFrame, pstVdecThreadParam->s32MilliSec);

                if (AR_SUCCESS == s32Ret && stVFrame.stVFrame.u64PhyAddr[0]) {

                    if (pstVdecThreadParam->transcoding) {
                        VCTRL_VDEC_StartEncoder(pstVdecThreadParam->s32TranscodingChnId, &stVFrame);
                    } else {
                        ++save_frame_counter;
                        if (save_frame_counter%10 == 0)
                            logline("chanid:%d GetFrame ok, format %d, width %d, height %d, s32Ret=%d, paddr=%llx\n",
                                     pstVdecThreadParam->s32ChnId, stVFrame.stVFrame.enPixelFormat,
                                     stVFrame.stVFrame.u32Width, stVFrame.stVFrame.u32Height, s32Ret,
                                     stVFrame.stVFrame.u64PhyAddr[0]);
                        if (pstVdecThreadParam->s32SaveFile == 1)
                            VCTRL_VDEC_SaveYUVFile_Linear8Bit(fp, &stVFrame.stVFrame);

                        u32PictureCnt++;

                    }
                    s32Ret = AR_MPI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);
                    if (AR_SUCCESS != s32Ret) {
                        logline("\033[0;31mchn%d AR_MPI_VDEC_ReleaseFrame fail for s32Ret=0x%x!\n\033[0m",
                                 pstVdecThreadParam->s32ChnId, s32Ret);
                    }
                } else {
                    usleep(1000);
                }

                // get userdata
                if (pstVdecThreadParam->enType == PT_H264 || pstVdecThreadParam->enType == PT_H265) {
                    s32Ret = AR_MPI_VDEC_GetUserData(pstVdecThreadParam->s32ChnId, &stUserData, 0);
                    if (AR_SUCCESS == s32Ret && stUserData.bValid) {
                        //DisplayHex(stUserData.pu8Addr, stUserData.u32Len, "UNREGISTERED");

                        s32Ret = AR_MPI_VDEC_ReleaseUserData(pstVdecThreadParam->s32ChnId, &stUserData);
                    }
                }

                if (!pstVdecThreadParam->transcoding) {
                    if(pstVdecThreadParam->u32PicNum >=0 && u32PictureCnt >= pstVdecThreadParam->u32PicNum) {
                        //printf("done encoding, recv_count=%d\n", recv_count);
                        pstVdecThreadParam->eThreadCtrl = THREAD_CTRL_STOP;
                        pChnInfo->u32State = STATE_IDLE;
                        printf("change pChnInfo->u32State to STATE_IDLE %d\n", STATE_IDLE);
                        stop_vchnnel_work(pChnInfo->chnId);
                        break;
                    }
                }
            }
    }

    if (fp != NULL) {
        fclose(fp);
    }

    logline("\033[0;35m chn %d get pic thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);

    return (AR_VOID *)AR_SUCCESS;

#endif
}

AR_VOID VCTRL_VDEC_StartGetPic(VDEC_CHN VdecChn, VCHN_INFO* pChnInfo, pthread_t *pVdecThread)
{
    pVdecThread[VdecChn] = 0;
    pthread_create(&pVdecThread[VdecChn], 0, VCTRL_VDEC_GetPic, (AR_VOID *)pChnInfo);
}

AR_S32 VCTRL_TransCode_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream)
{
    AR_S32 i;

    for (i = 0; i < pstStream->u32PackCount; i++) {
        fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);

        fflush(pFd);
    }

    return AR_SUCCESS;
}

AR_VOID* VCTRL_VDEC_GetStream(AR_VOID* pArgs)
{
    AR_S32 s32Ret;
    AR_S32 maxfd = 0;
    AR_S32 VencFd = -1;
    fd_set read_fds;
    struct timeval timeoutVal;
    AR_CHAR fileName[128] = {'\0'};
    VCHN_INFO* pChnInfo = NULL;
    VDEC_THREAD_PARAM_S *pstVdecThreadParam = NULL;
    VENC_CHN VencChn = 0;
    VENC_CHN_STATUS_S stStat = {0};
    VENC_STREAM_S stStream = {0};
    char threadName[16]={0};
    AR_U32 u32PictureCnt = 0;
    FILE* pFd = NULL;
    pChnInfo =  (VCHN_INFO *)pArgs;
    pstVdecThreadParam = &gstVdecThreadParam[pChnInfo->transcodingChnId];
    VencChn = pstVdecThreadParam->s32TranscodingChnId;
    sprintf(threadName, "GetVencStrm_%d", pstVdecThreadParam->s32TranscodingChnId);
    prctl(PR_SET_NAME, threadName, 0, 0, 0);

    if (pstVdecThreadParam->s32SaveFile) {

        if (!pstVdecThreadParam->cOutFileName) {
            snprintf(fileName, 32, "stream_chn%d.stream", VencChn);
        } else {
            strncpy(fileName, pstVdecThreadParam->cOutFileName, 128);
        }

        pFd = fopen(fileName, "wb");
        if (!pFd) {
            VCTRL_PRT("open %s failed\n", fileName);
            return (AR_VOID *)(AR_FAILURE);;
        }

    }

    /* Set Venc Fd. */
    VencFd = AR_MPI_VENC_GetFd(VencChn);
    if (VencFd < 0) {
        VCTRL_PRT("AR_MPI_VENC_GetFd failed with %x\n", VencFd);
        return (AR_VOID *)(AR_FAILURE);;
    }

    if (maxfd <= VencFd) {
        maxfd = VencFd;
    }

    while (1) {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP) {
            break;
        }

        if (!ar_hal_sys_mpp_service_is_lib()) {
            FD_ZERO(&read_fds);
            FD_SET(VencFd, &read_fds);

            timeoutVal.tv_sec  = 2;
            timeoutVal.tv_usec = 0;
            s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &timeoutVal);
            if (s32Ret < 0) {
                VCTRL_PRT("select failed!\n");
                break;
            } else if (s32Ret == 0) {
                VCTRL_PRT("get venc stream time out, exit thread\n");
                continue;
            } else if (!FD_ISSET(VencFd, &read_fds)) {
                continue;
            }
        }

        /*******************************************************
         step 2.1 : query how many packs in one-frame stream.
        *******************************************************/
        memset(&stStream, 0, sizeof(stStream));
        s32Ret = AR_MPI_VENC_QueryStatus(VencChn, &stStat);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VENC_QueryStatus chn%d failed with %#x!\n", VencChn, s32Ret);
            break;
        }

        /*******************************************************
        step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
        if (0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames) {
            VCTRL_PRT("NOTE: Current  frame is NULL!\n");
            continue;
        }
        *******************************************************/
        if (0 == stStat.u32CurPacks) {
              VCTRL_PRT("NOTE: Current  frame is NULL!\n");
              usleep(100 * 1000);
              continue;
        }

        if (stStat.u32CurPacks > 0) {
            /*******************************************************
             step 2.3 : malloc corresponding number of pack nodes.
            *******************************************************/
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (NULL == stStream.pstPack) {
                VCTRL_PRT("malloc stream pack failed!\n");
                break;
            }

            /*******************************************************
             step 2.4 : call mpi to get one-frame stream
            *******************************************************/
            stStream.u32PackCount = stStat.u32CurPacks;
            s32Ret = AR_MPI_VENC_GetStream(VencChn, &stStream, AR_TRUE);
            if (AR_SUCCESS != s32Ret) {
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                VCTRL_PRT("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                break;
            }

            /*******************************************************
             step 2.5 : save frame to file
            *******************************************************/
            //logline("chn%d packet size:%d, getstream cnt:%d", VencChn, stStream.pstPack->u32Len - stStream.pstPack->u32Offset, cnt);
            if (pstVdecThreadParam->s32SaveFile) {
                if (pFd) {
                    VCTRL_TransCode_SaveStream(pFd, &stStream);
                }
            }
            /*
            frame.stVFrame.u64PhyAddr[0] = g_frameInfo[VencChn].pa[stStream.u32Seq % (DISPLAY_NUM + 1)];
            AR_MPI_VDEC_ReleaseFrame(0, &frame);
            */

            /*******************************************************
             step 2.6 : release stream
             *******************************************************/
            s32Ret = AR_MPI_VENC_ReleaseStream(VencChn, &stStream);
            if (AR_SUCCESS != s32Ret) {
                VCTRL_PRT("AR_MPI_VENC_ReleaseStream failed!\n");
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                break;
            }
            /*******************************************************
             step 2.7 : free pack nodes
            *******************************************************/
            free(stStream.pstPack);
            stStream.pstPack = NULL;
            u32PictureCnt ++;

            if(pstVdecThreadParam->u32PicNum >=0 && u32PictureCnt >= pstVdecThreadParam->u32PicNum) {
                //printf("done encoding, recv_count=%d\n", recv_count);
                pstVdecThreadParam->eThreadCtrl = THREAD_CTRL_STOP;
                pChnInfo->u32State = STATE_IDLE;
                printf("change pChnInfo->u32State to STATE_IDLE %d\n", STATE_IDLE);
                stop_vchnnel_work(pstVdecThreadParam->s32ChnId);
                break;
            }
        }
    } //endof fd set

    if (pFd) {
        fclose(pFd);
    }

    return (AR_VOID *)(AR_SUCCESS);
}

AR_VOID VCTRL_VDEC_StartGetStream(VDEC_CHN VdecChn, VCHN_INFO* pChnInfo, pthread_t *pVdecThread)
{
    pVdecThread[VdecChn] = 0;
    pthread_create(&pVdecThread[VdecChn], 0, VCTRL_VDEC_GetStream, (AR_VOID *)pChnInfo);
}

AR_VOID VCTRL_VDEC_StopGetStream(VDEC_CHN VdecChn, VCHN_INFO* pChnInfo, pthread_t *pVdecThread)
{
    VDEC_THREAD_PARAM_S *pstVdecGet = NULL;
    pstVdecGet = &gstVdecThreadParam[VdecChn];
    pstVdecGet->eThreadCtrl = THREAD_CTRL_STOP;

    if (0 != pVdecThread[VdecChn]) {
        pthread_join(pVdecThread[VdecChn], AR_NULL);
        pVdecThread[VdecChn] = 0;
    }
}

AR_VOID VCTRL_VDEC_StopGetPic(VDEC_CHN VdecChn, VCHN_INFO* pChnInfo, pthread_t *pVdecThread)
{
    VDEC_THREAD_PARAM_S *pstVdecGet = NULL;
    pstVdecGet = &gstVdecThreadParam[VdecChn];
    pstVdecGet->eThreadCtrl = THREAD_CTRL_STOP;

    if (0 != pVdecThread[VdecChn]) {
        pthread_join(pVdecThread[VdecChn], AR_NULL);
        pVdecThread[VdecChn] = 0;
    }
}

AR_S32 VCTRL_VDEC_Start(VCHN_INFO* pChnInfo, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    AR_S32 s32ChnId;
    VDEC_CHN_ATTR_S stChnAttr[VDEC_MAX_CHN_NUM] = {0};
    VDEC_CHN_POOL_S stPool = {0};
    VDEC_CHN_PARAM_S stChnParam = {0};

    VCTRL_COMM_CORE_Init(pChnInfo->enPayLoad, pChnInfo->processor, pChnInfo->u32CoreClock,
        pChnInfo->u32BpuClock, pChnInfo->u32JpegCoreClock);

    s32ChnId = pChnInfo->chnId;
    task_done[s32ChnId] = 0;
    stChnAttr[s32ChnId].enType           = pastSampleVdec[s32ChnId].enType;
    stChnAttr[s32ChnId].enMode           = pastSampleVdec[s32ChnId].enMode;
    stChnAttr[s32ChnId].u32PicWidth      = pastSampleVdec[s32ChnId].u32Width;
    stChnAttr[s32ChnId].u32PicHeight     = pastSampleVdec[s32ChnId].u32Height;
    stChnAttr[s32ChnId].u32FrameBufCnt   = pastSampleVdec[s32ChnId].u32FrameBufCnt;

    if (PT_H264 == pastSampleVdec[s32ChnId].enType || PT_H265 == pastSampleVdec[s32ChnId].enType) {
        stChnAttr[s32ChnId].stVdecVideoAttr.u32RefFrameNum     = pastSampleVdec[s32ChnId].stSapmleVdecVideo.u32RefFrameNum;
        stChnAttr[s32ChnId].stVdecVideoAttr.bTemporalMvpEnable = 1;
        if ((PT_H264 == pastSampleVdec[s32ChnId].enType) &&
            (VIDEO_DEC_MODE_IPB != pastSampleVdec[s32ChnId].stSapmleVdecVideo.enDecMode)) {
            stChnAttr[s32ChnId].stVdecVideoAttr.bTemporalMvpEnable = 0;
        }
        stChnAttr[s32ChnId].u32StreamBufSize = pastSampleVdec[s32ChnId].u32Width*pastSampleVdec[s32ChnId].u32Height;
        stChnAttr[s32ChnId].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[s32ChnId].enType, pastSampleVdec[s32ChnId].u32Width,
            pastSampleVdec[s32ChnId].u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[s32ChnId].stSapmleVdecVideo.enBitWidth, 0);
        stChnAttr[s32ChnId].stVdecVideoAttr.u32CmdQueueDepth = 4;
    } else if (PT_JPEG == pastSampleVdec[s32ChnId].enType || PT_MJPEG == pastSampleVdec[s32ChnId].enType) {
        stChnAttr[s32ChnId].enMode           = VIDEO_MODE_FRAME;
        /*
        stChnAttr[s32ChnId].u32StreamBufSize = AR_ALIGN1024(pastSampleVdec[s32ChnId].u32Width*pastSampleVdec[s32ChnId].u32Height);
        */
        stChnAttr[s32ChnId].u32StreamBufSize = AR_ALIGN1024(1024 * 1024 * 10);
        stChnAttr[s32ChnId].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[s32ChnId].enType, pastSampleVdec[s32ChnId].u32Width,
            pastSampleVdec[s32ChnId].u32Height, pastSampleVdec[s32ChnId].stSapmleVdecPicture.enPixelFormat, DATA_BITWIDTH_8, 0);
    } else {
        printf(stderr, "please indentify your codec type %d.\n", pastSampleVdec[s32ChnId].enType);
        return AR_FAILURE;
    }
    printf("stChnAttr[i].u32StreamBufSize = %d, stChnAttr[i].u32FrameBufSize = %d\n", stChnAttr[s32ChnId].u32StreamBufSize, stChnAttr[s32ChnId].u32FrameBufSize);

    CHECK_CHN_RET(AR_MPI_VDEC_CreateChn(s32ChnId, &stChnAttr[s32ChnId]), s32ChnId, "AR_MPI_VDEC_CreateChn");

    if (VB_SOURCE_USER == g_enVdecVBSource) {
        stPool.hPicVbPool = g_ahPicVbPool[s32ChnId];
        stPool.hTmvVbPool = g_ahTmvVbPool[s32ChnId];
        CHECK_CHN_RET(AR_MPI_VDEC_AttachVbPool(s32ChnId, &stPool), s32ChnId, "AR_MPI_VDEC_AttachVbPool");
    }

    CHECK_CHN_RET(AR_MPI_VDEC_GetChnParam(s32ChnId, &stChnParam), s32ChnId, "AR_MPI_VDEC_GetChnParam");
    if (PT_H264 == pastSampleVdec[s32ChnId].enType || PT_H265 == pastSampleVdec[s32ChnId].enType) {
        stChnParam.stVdecVideoParam.enDecMode         = pastSampleVdec[s32ChnId].stSapmleVdecVideo.enDecMode;
        stChnParam.stVdecVideoParam.enCompressMode    = COMPRESS_MODE_NONE;
        stChnParam.stVdecVideoParam.enVideoFormat     = VIDEO_FORMAT_LINEAR;
        stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DISP;
        printf("-------enDecMode %d enOutputOrder %d.\n",
                        stChnParam.stVdecVideoParam.enDecMode, stChnParam.stVdecVideoParam.enOutputOrder);
    } else {
        stChnParam.stVdecPictureParam.enPixelFormat   = pastSampleVdec[s32ChnId].stSapmleVdecPicture.enPixelFormat;
        stChnParam.stVdecPictureParam.u32Alpha        = pastSampleVdec[s32ChnId].stSapmleVdecPicture.u32Alpha;
    }
    stChnParam.u32DisplayFrameNum                     = pastSampleVdec[s32ChnId].u32DisplayFrameNum;
    CHECK_CHN_RET(AR_MPI_VDEC_SetChnParam(s32ChnId, &stChnParam), s32ChnId, "AR_MPI_VDEC_GetChnParam");

    CHECK_CHN_RET(AR_MPI_VDEC_StartRecvStream(s32ChnId), s32ChnId, "AR_MPI_VDEC_StartRecvStream");

    return AR_SUCCESS;
}


AR_S32 VCTRL_VDEC_Stop(VCHN_INFO* pChnInfo)
{

    if (pChnInfo->transcoding) {

        CHECK_CHN_RET(AR_MPI_VENC_StopRecvFrame(pChnInfo->transcodingChnId), pChnInfo->transcodingChnId, "AR_MPI_VENC_StopRecvFrame");

        CHECK_CHN_RET(AR_MPI_VENC_DestroyChn(pChnInfo->transcodingChnId), pChnInfo->transcodingChnId, "AR_MPI_VENC_DestroyChn");

    }

    CHECK_CHN_RET(AR_MPI_VDEC_StopRecvStream(pChnInfo->chnId), pChnInfo->chnId, "AR_MPI_VDEC_StopRecvStream");

    CHECK_CHN_RET(AR_MPI_VDEC_DestroyChn(pChnInfo->chnId), pChnInfo->chnId, "AR_MPI_VDEC_DestroyChn");

    return AR_SUCCESS;
}

#if 0
AR_S32 bind_get_stream_frame(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
    AR_S32              s32Ret;
    VB_BLK block_handle;
    VB_POOL pool_handle;
    ViData * p_data;
  VIDEO_FRAME_INFO_S * pstFrameInfo = (VIDEO_FRAME_INFO_S *)pv_data;
    if (NULL == pstFrameInfo)
    {
        VCTRL_PRT("pv_data is NULL!\n");
        return -1;
    }
    p_data = (ViData *)pv_priv_data;

    VI_USERPIC_ATTR_S * pUsrPic = &p_data->stUsrPic;

    s32Ret = VCTRL_COMM_VI_Load_UserPic(p_data->pfd, pUsrPic, &p_data->stViFrameInfo);
    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("VCTRL_COMM_VI_Load_UserPic failed with %#x!\n", s32Ret);
    }
    memcpy(pstFrameInfo, &pUsrPic->unUsrPic.stUsrPicFrm, sizeof(VIDEO_FRAME_INFO_S));

    //usleep(1000 / 25 * 1000); //25fps
    sleep(1);

    return AR_SUCCESS;
}

AR_S32 bind_release_stream_frame(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
    ViData * p_data;
    VCTRL_VI_FRAME_INFO_S * frame_info = (VCTRL_VI_FRAME_INFO_S * )pv_data;
    if (NULL == frame_info)
    {
        VCTRL_PRT("pv_data is NULL!\n");
        return -1;
    }
    p_data = (struct sender_data *)pv_priv_data;
    VCTRL_COMM_VI_Release_UserPic(&p_data->stViFrameInfo);
    return AR_SUCCESS;
}

AR_S32 VCTRL_VI_Bind_VDEC(VI_PIPE ViPipe, VI_CHN ViChn, VDEC_CHN VdecChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VDEC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VdecChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VI-VDEC)");

    return AR_SUCCESS;
}

AR_S32 VCTRL_VI_UnBind_VDEC(VI_PIPE ViPipe, VI_CHN ViChn, VDEC_CHN VdecChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VDEC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VdecChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VI-VDEC)");

    return AR_SUCCESS;
}
#endif

#if 0
AR_S32 VCTRL_VDEC_VI_Init( VCTRL_VI_BIND_INFO * pstViInfo)
{
    AR_S32              s32Ret;
    int                     i = 0;

    STRU_SYS_BIND_SENDER * pViBind = &pstViInfo->stViBind;

    pViBind->e_mod_id = AR_SYS_ID_VI;
    pViBind->u32_max_dev_cnt = 4;
    pViBind->u32_max_chn_cnt = 3;
    pViBind->s32_flag = 0;
    pViBind->e_data_type = AR_SYS_DATA_VIDEO_FRAME;
    pViBind->give_bind_call_back = NULL;

    pViBind->get_frame_call_back = bind_get_stream_frame;
    pViBind->release_frame_call_back = bind_release_stream_frame;

    VI_USERPIC_ATTR_S stUsrPic;
    stUsrPic.enUsrPicMode = VI_USERPIC_MODE_PIC;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Width = pstViInfo->u32Width;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Height = pstViInfo->u32Height;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    ViData * pViData = &pstViInfo->stViData;

    memcpy(&pViData->stUsrPic, &stUsrPic, sizeof(stUsrPic));
    pViData->e_mod_id = AR_SYS_ID_VI;
    pViData->p_sender = pViBind;

    VCTRL_PRT("====to ar_mpi_open stream file: %s. \n", pstViInfo->inFile);
    pViData->pfd = fopen(pstViInfo->inFile, "rb");
    if (pViData->pfd == NULL)
    {
        VCTRL_PRT("ar_mpi_open file -> %s fail \n", pstViInfo->inFile);
        return AR_FAILURE;
    }

    pViBind->pv_priv_data = pViData;

    ar_hal_sys_bind_init();

  if ((s32Ret = ar_hal_sys_bind_register_sender(pViBind)) < 0)
  {
    VCTRL_PRT("bind_register_sender failed (%lx) !\n", s32Ret);
        return AR_FAILURE;
  }

    return AR_SUCCESS;
}

AR_VOID VCTRL_VDEC_VI_Deinit( VCTRL_VI_BIND_INFO * pstViInfo)
{
  ar_hal_sys_bind_unregister_sender(AR_SYS_ID_VI);
    ViData *     pViData = &pstViInfo->stViData;

    ar_hal_sys_bind_exit();

    if (pViData->pfd)
       fclose(pViData->pfd);
}
#endif

AR_VOID* VCTRL_VdecStartProc(AR_VOID* param)
{
    AR_S32 i;
    AR_S32 s32Ret;
    //SIZE_S stSize;
    VCHN_INFO* pChnInfo = (VCHN_INFO*)param;
    if (pChnInfo == NULL) {
        VCTRL_PRT("no valid input data\n");
        return (void *)(AR_FAILURE);
    }

    //stSize.u32Width = pChnInfo->u32Width;
    //stSize.u32Height = pChnInfo->u32Height;

    AR_U32 u32RefNum = 6;
    AR_U32 u32DisNum = DISPLAY_NUM;

    //VI_DEV   ViDev        = 0;
    //VI_PIPE  ViPipe       = 0;
    VDEC_CHN VdecChn        = pChnInfo->chnId;
    VDEC_CHN VencChn        = pChnInfo->transcodingChnId;

    SAMPLE_VDEC_ATTR pastSampleVdec[VDEC_MAX_CHN_NUM];

    //VCTRL_VI_BIND_INFO * pViInfo = &pChnInfo->stViInfo;

    pastSampleVdec[VdecChn].enType = pChnInfo->enPayLoad;
    pastSampleVdec[VdecChn].enMode = VIDEO_MODE_FRAME;
    pastSampleVdec[VdecChn].u32Width = pChnInfo->u32Width;
    pastSampleVdec[VdecChn].u32Height = pChnInfo->u32Height;
    pastSampleVdec[VdecChn].s32ChnId  = pChnInfo->chnId;
    //pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat;
    pastSampleVdec[VdecChn].stSapmleVdecVideo.enDecMode = VIDEO_DEC_MODE_IPB;
    pastSampleVdec[VdecChn].stSapmleVdecVideo.u32RefFrameNum = u32RefNum;
    pastSampleVdec[VdecChn].u32DisplayFrameNum = u32DisNum;
    pastSampleVdec[VdecChn].u32FrameBufCnt = u32RefNum + u32DisNum + 1;
    //pastSampleVdec[i].stSapmleVdecVideo.enBitWidth;
    //pastSampleVdec[i].stSapmleVdecPicture.u32Alpha;

    strcpy(gstVdecThreadParam[VdecChn].cInFileName, pChnInfo->inputFilePath);
    strcpy(gstVdecThreadParam[VdecChn].cOutFileName, pChnInfo->outputFilePath);

    gstVdecThreadParam[VdecChn].u32PicNum = pChnInfo->u32PicNum;
    gstVdecThreadParam[VdecChn].s32ChnId  = pChnInfo->chnId;
    gstVdecThreadParam[VdecChn].s32TranscodingChnId  = pChnInfo->transcodingChnId;
    gstVdecThreadParam[VdecChn].s32MinBufSize = 1 << 18;
    gstVdecThreadParam[VdecChn].u64PtsInit = 0;
    gstVdecThreadParam[VdecChn].eThreadCtrl = THREAD_CTRL_START;
    gstVdecThreadParam[VdecChn].bCircleSend = true;
    gstVdecThreadParam[VdecChn].s32StreamMode = VIDEO_MODE_FRAME;
    gstVdecThreadParam[VdecChn].enType = pChnInfo->enPayLoad;
    gstVdecThreadParam[VdecChn].enTranscodingType = pChnInfo->enTranscodingPayLoad;
    gstVdecThreadParam[VdecChn].s32MilliSec = -1;
    gstVdecThreadParam[VdecChn].s32IntervalTime = 20;
    gstVdecThreadParam[VdecChn].u64PtsIncrease = 40;
    gstVdecThreadParam[VdecChn].transcoding = pChnInfo->transcoding;
    gstVdecThreadParam[VdecChn].s32SaveFile = pChnInfo->s32SaveFile;

    if (pChnInfo->transcoding) {
        memcpy(&gstVdecThreadParam[VencChn], &gstVdecThreadParam[VdecChn], sizeof(VDEC_THREAD_PARAM_S));
    }
#if 0
    // TODO, use bind
    s32Ret = VCTRL_VDEC_VI_Init(pViInfo);
    if (s32Ret != AR_SUCCESS)
    {
        VCTRL_PRT("Init VI err for %#x!\n", s32Ret);
        goto EXIT;
    }
#endif

    logline();

   /******************************************
    start stream vdec
    ******************************************/
    s32Ret = VCTRL_VDEC_Start(pChnInfo, pastSampleVdec);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("Vdec Start failed for %#x!\n", s32Ret);
        goto EXIT_VDEC_STOP;
    }

   /******************************************
    start venc if needed
    ******************************************/
    if (pChnInfo->transcoding) {
        AR_MPI_VENC_Init();
        VCTRL_VDEC_CreateEncoder(pChnInfo);
    }

    VCTRL_VDEC_StartSendStream(VdecChn, pChnInfo, vdecSendThreads);

/*    if (pChnInfo->transcoding) {
        MPP_CHN_S stSrcChn;
        MPP_CHN_S stDestChn;
        AR_S32 u32ChnNum = 1;

        for (i = 0; i < u32ChnNum; i++) {
            stSrcChn.enModId   = AR_ID_VDEC;
            stSrcChn.s32DevId  = 0;
            stSrcChn.s32ChnId  = pstVdecThreadParam[i].s32ChnId;

            stDestChn.enModId  = AR_ID_VENC;
            stDestChn.s32DevId = 0;
            stDestChn.s32ChnId = pstVdecThreadParam[i].s32ChnId;

            AR_S32 bindRet = AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        }
    }
*/

#if 0
    s32Ret = VCTRL_VI_Bind_VDEC(ViPipe, ViChn, VdecChn);
    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("VCTRL_VI_Bind_VDEC failed for %#x!\n", s32Ret);
        goto EXIT_VDEC_STOP;
    }
#endif

    /******************************************
     yuv save process
    ******************************************/
    VCTRL_VDEC_StartGetPic(VdecChn, pChnInfo, vdecGetThreads);

    /******************************************
     stream save process, only used for transconding
    ******************************************/
    if (pChnInfo->transcoding) {
        VCTRL_VDEC_StartGetStream(VencChn, pChnInfo, vdecGetThreads);
    }

    pChnInfo->u32State = STATE_START;

    while(STATE_START == VCTRL_VDEC_SERVICE_STATUS(pChnInfo)) {
        sleep(1);
    }

    /******************************************
     exit process
    ******************************************/

/*
    if (pChnInfo->transcoding) {
        MPP_CHN_S stSrcChn;
        MPP_CHN_S stDestChn;
        AR_S32 u32ChnNum = 1;

        for (i = 0; i < u32ChnNum; i++) {
            stSrcChn.enModId   = AR_ID_VDEC;
            stSrcChn.s32DevId  = 0;
            stSrcChn.s32ChnId  = pstVdecThreadParam[i].s32ChnId;

            stDestChn.enModId  = AR_ID_VENC;
            stDestChn.s32DevId = 0;
            stDestChn.s32ChnId = pstVdecThreadParam[i].s32ChnId;

            AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        }
    }
*/

    VCTRL_VDEC_StopSendStream(VdecChn, pChnInfo, vdecSendThreads);
    VCTRL_VDEC_StopGetPic(VdecChn, pChnInfo, vdecGetThreads);

    if (pChnInfo->transcoding) {
        VCTRL_VDEC_StopGetStream(VencChn, pChnInfo, vdecGetThreads);
    }

#if 0
EXIT_VDEC_UNBind:
    VCTRL_VI_UnBind_VDEC(ViPipe, ViChn, VdecChn);
#endif
EXIT_VDEC_STOP:

    VCTRL_VDEC_Stop(pChnInfo);

#if 0
EXIT_VI_STOP:
    VCTRL_VDEC_VI_Deinit(pViInfo);
#endif

    return param;
}

AR_S32 VCTRL_VDEC_SERVICE_START(VCHN_INFO* pChnInfo)
{
    printf("Start Vdec chn%d, u32State %d!\n",pChnInfo->chnId, pChnInfo->u32State);
    return pthread_create(&pChnInfo->ptVdecStartTid, 0, VCTRL_VdecStartProc, (AR_VOID*)pChnInfo);
}

AR_S32 VCTRL_VDEC_SERVICE_STOP(VCHN_INFO* pChnInfo)
{
    printf("Stop Vdec chn%d, u32State %d!\n",pChnInfo->chnId, pChnInfo->u32State);
    if (pChnInfo->u32State == STATE_START) {
        pChnInfo->u32State = STATE_STOP;
        pthread_join(pChnInfo->ptVdecStartTid, 0);
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VDEC_SERVICE_SET(VCHN_INFO* pChnInfo)
{
    AR_S32 s32Ret;
    if (pChnInfo == NULL) {
        VCTRL_PRT("no valid input data\n");
        return AR_FAILURE;
    }
    printf("Set Vdec chn%d\n", pChnInfo->chnId);

    if (pChnInfo->enUpdate) {
        VDEC_CHN_ATTR_S stChnAttr;
        s32Ret = AR_MPI_VDEC_GetChnAttr(pChnInfo->chnId,&stChnAttr);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VDEC_GetChnAttr with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        if (pChnInfo->enUpdate & UPDATE_BITRATE)
            // TODO, stChnAttr.stRcAttr.enRcMode = pChnInfo->enRcMode;

        s32Ret = AR_MPI_VDEC_SetChnAttr(pChnInfo->chnId,&stChnAttr);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VDEC_SetChnAttr with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }

    return AR_SUCCESS;
}

AR_S32 VCTRL_VDEC_SERVICE_STATUS(VCHN_INFO* pChnInfo)
{
    if (pChnInfo == NULL) {
        VCTRL_PRT("no valid input data\n");
        return AR_FAILURE;
    }
    return pChnInfo->u32State;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
