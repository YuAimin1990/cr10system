#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>
#ifdef __cplusplus
};
#endif

//#define DUMP_STREAM_FILE
#define FILENAME_LEN (128)
#define STREAM_FILE_NAME   "stream_file.mp4"

AR_S32 g_videoindex[TEST_VDEC_MAX_CHN_NUM] = {[0 ... (TEST_VDEC_MAX_CHN_NUM-1)] = -1};
AVFormatContext *g_fmtCtx[TEST_VDEC_MAX_CHN_NUM] = {[0 ... (TEST_VDEC_MAX_CHN_NUM-1)] = NULL};

extern int task_done[TEST_VDEC_MAX_CHN_NUM];
typedef struct task_param {
    SAMPLE_VDEC_ATTR    attr;
    AR_S32              numFrameToDecode;
    AR_CHAR             yuvFile[FILENAME_LEN];
    AR_CHAR             streamFile[FILENAME_LEN];
    AR_S32              fps;
    AR_S32              startChnId;
    AR_S32              chnNum;
    AR_S32              saveFile;
    VB_SOURCE_E         vbSource;
} task_param;

AVFormatContext *read_mp4_info(char *mp4file, PAYLOAD_TYPE_E *entype, AR_U32 *width, AR_U32 *height, AR_S32 *fps, AR_S32 * video_index)
{
    int ret = 0;
    int index = 0;
    enum AVCodecID codec;
    AVFormatContext *fmtCtx = NULL;

    fmtCtx = avformat_alloc_context();

    ret = avformat_open_input(&fmtCtx, mp4file, NULL, NULL);
    if (ret < 0) {
        logerr("avformat_open_input %s failed, error(%s)\n", mp4file, av_err2str(ret));
        return NULL;
    }

    ret = avformat_find_stream_info(fmtCtx, NULL);
    if (ret < 0) {
        logerr("avformat_find_stream_info failed, error(%s)\n", av_err2str(ret));
        return NULL;
    }

    index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (index < 0) {
        logerr("av_find_best_stream %s failed\n", mp4file);
        return NULL;
    }

    codec = fmtCtx->streams[index]->codecpar->codec_id;
    if (codec == AV_CODEC_ID_H264)
        *entype = PT_H264;
    else if (codec == AV_CODEC_ID_HEVC)
        *entype = PT_H265;
    else {
        return NULL;
    }

    if (fmtCtx->streams[index]->avg_frame_rate.den && fmtCtx->streams[index]->avg_frame_rate.num) {
        *fps = *fps ? *fps : av_q2d(fmtCtx->streams[index]->avg_frame_rate);
    }

    *fps = *fps ? *fps : 25;
    *width  = fmtCtx->streams[index]->codecpar->width;
    *height = fmtCtx->streams[index]->codecpar->height;

    *video_index = index;
    av_dump_format(fmtCtx, 0, mp4file, 0);

    return fmtCtx;
}

AR_VOID * SAMPLE_COMM_VDEC_SendStreamMP4(AR_VOID *pArgs)
{
    AR_S32 s32Ret;
    AVPacket *packet;
    AR_S32 send_count = 0;
    AR_CHAR cStreamFile[256];
    VDEC_STREAM_S stStream = {0};
    AVBSFContext *bsf_ctx = NULL;
    VDEC_THREAD_PARAM_S *pstVdecThreadParam = (VDEC_THREAD_PARAM_S *)pArgs;
    AR_S32 video_index = g_videoindex[pstVdecThreadParam->s32ChnId];
    AVFormatContext *fmtCtx = g_fmtCtx[pstVdecThreadParam->s32ChnId];

    prctl(PR_SET_NAME, "VideoSendStream", 0,0,0);
    snprintf(cStreamFile, sizeof(cStreamFile), pstVdecThreadParam->cFileName);
    logline("cStreamFile=%s, cOutFile=%s\n", cStreamFile, pstVdecThreadParam->cOutName);

#ifdef DUMP_STREAM_FILE
    FILE *fp_dump = NULL;
    char dumpfile[128] = STREAM_FILE_NAME;
    fp_dump = fopen(dumpfile,"wb");
    fflush(stdout);
#endif

    if (pstVdecThreadParam->enType != PT_H264 && pstVdecThreadParam->enType != PT_H265) {
        logerr("entype not support\n");
        return (AR_VOID*)(AR_FAILURE);
    }

#if 0
    const AVBitStreamFilter *pfilter;
    if (pstVdecThreadParam->enType == PT_H264) {
        pfilter = av_bsf_get_by_name("h264_mp4toannexb");
    } else if (pstVdecThreadParam->enType == PT_H265 ) {
        pfilter = av_bsf_get_by_name("hevc_mp4toannexb");
    }

    if (pfilter == NULL) {
        logerr("get bsf failed\n");
        return (AR_VOID*)(AR_FAILURE);
    }

    s32Ret = av_bsf_alloc(pfilter, &bsf_ctx);
    if (s32Ret != 0) {
        logerr("bsf alloc failed, ret = %#lx\n", s32Ret);
        return (AR_VOID*)(AR_FAILURE);
    }
#else
    if (pstVdecThreadParam->enType == PT_H264) {
        av_bsf_list_parse_str("h264_mp4toannexb,filter_units=pass_types=1-8,extract_extradata,h264_metadata", &bsf_ctx);
    } else if (pstVdecThreadParam->enType == PT_H265) {
        av_bsf_list_parse_str("hevc_mp4toannexb,filter_units=remove_types=35|38-40,extract_extradata,hevc_metadata", &bsf_ctx);
    }
#endif

    avcodec_parameters_copy(bsf_ctx->par_in, fmtCtx->streams[video_index]->codecpar);

    s32Ret = av_bsf_init(bsf_ctx);
    if (s32Ret != 0) {
        logerr("av_bsf_init failed, error(%s)\n", av_err2str(s32Ret));
        return (AR_VOID*)(AR_FAILURE);
    }

    packet = av_packet_alloc();

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
            logline("task %d done, exit thread", pstVdecThreadParam->s32ChnId);
            break;
        }

        s32Ret = av_read_frame(fmtCtx, packet);
        if (s32Ret != 0) // end of mp4 file
        {
            av_packet_unref(packet);
            if (pstVdecThreadParam->bCircleSend == AR_TRUE)
            {
                printf("\033[0;32mreplay...\033[0;39m\n");
                memset(&stStream, 0, sizeof(VDEC_STREAM_S));
                av_seek_frame(fmtCtx, video_index, 0, AVSEEK_FLAG_BACKWARD);
                s32Ret = av_read_frame(fmtCtx, packet);
            }
            else
            {
                break;
            }
        }

        if (packet->stream_index != video_index)
            continue;

        s32Ret = av_bsf_send_packet(bsf_ctx, packet);
        if (s32Ret) {
            logerr("av_bsf_send_packet error, error(%s)\n", av_err2str(s32Ret));
            break;
        }

        s32Ret = av_bsf_receive_packet(bsf_ctx, packet);
        if (s32Ret != 0) {
            logline("av_bsf_receive_packet failed, error(%s)\n", av_err2str(s32Ret));
            continue;
        }

        stStream.u64PTS       = packet->pts;
        stStream.pu8Addr      = packet->data;
        stStream.u32Len       = packet->size;
        stStream.bEndOfFrame  = AR_TRUE;
        stStream.bEndOfStream = AR_FALSE;
        stStream.bDisplay     = AR_TRUE;
        logline("send frame %d, len = %d, pts = %lld, flag = %d\n", send_count, stStream.u32Len, stStream.u64PTS, packet->flags);

#ifdef DUMP_STREAM_FILE
        if (fp_dump != NULL) {
            fwrite(stStream.pu8Addr, stStream.u32Len, 1, fp_dump);
        }
#endif

SendAgain:
        s32Ret = AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if ((AR_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl))
        {
            if (s32Ret != AR_ERR_VDEC_BUF_FULL)
                logerr("AR_MPI_VDEC_SendStream %dth faield, ret = %#lx", send_count, s32Ret);
            else {
                usleep(2*1000);
            }
            goto SendAgain;
        }
        else
        {
            send_count++;
        }

        usleep(pstVdecThreadParam->s32IntervalTime * 1000);
        av_packet_unref(packet);
    }

    av_bsf_free(&bsf_ctx);
    av_packet_free(&packet);
    avformat_close_input(&fmtCtx);

#ifdef DUMP_STREAM_FILE
    if (fp_dump != NULL) {
        fclose(fp_dump);
    }
#endif

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.u64PTS       = -1;
    stStream.pu8Addr      = NULL;
    stStream.u32Len       = 0;
    stStream.bEndOfFrame  = AR_TRUE;
    stStream.bEndOfStream = AR_TRUE;
    stStream.bDisplay     = AR_FALSE;
    AR_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    printf("\033[0;35m chn %d send EOS and return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);

    return (AR_VOID *)AR_SUCCESS;
}

AR_VOID SAMPLE_COMM_VDEC_StartSendStreamMP4(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    AR_S32 i;
    for (i = 0; i<s32ChnNum; i++)
    {
        pVdecThread[i] = 0;
        char threadName[16] = {'\0'};
        sprintf(threadName, "sendstream_%d", i);
        int ret = pthread_create(&pVdecThread[i], 0, SAMPLE_COMM_VDEC_SendStreamMP4, (AR_VOID *)&pstVdecSend[i]);

        if (ret) {
            logline("[%s:%d] pthread_create failed: %s\n", __FUNCTION__, __LINE__, strerror(errno));
            return;
        }

        ret = pthread_setname_np(pVdecThread[i], threadName);

        if (ret) {
            logline("[%s:%d] pthread_setname_np failed: %s\n", __FUNCTION__, __LINE__, strerror(ret));
        }

        memset(threadName, 0, sizeof(threadName));
        ret = pthread_getname_np(pVdecThread[i], threadName, sizeof(threadName));

        if (!ret)
            logline("threadName is %s\n", threadName);
    }
}

AR_VOID SAMPLE_VDEC_HandleSig(AR_S32 signo)
{
    if (SIGINT == signo || SIGTSTP == signo || SIGTERM == signo)
    {
        //SAMPLE_COMM_VO_HdmiStop();
        //SAMPLE_COMM_SYS_Exit();
        //printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        logline("set task_done to 1");
        for (int i = 0;i<TEST_VDEC_MAX_CHN_NUM;i++)
            task_done[i]=1;
    }

    //exit(0);
}

AR_VOID SAMPLE_VDEC_Usage(char *sPrgNm)
{
    printf("Usage : %s [OPTION]\n\n", sPrgNm);
    printf("-t, --type    video pipeline type default 1, possible values as follows:\n");
    printf("\t0:  VDEC(H264 or H265 PLAYBACK)-VPSS-VO\n");
    printf("\t1:  VDEC(H264 or H265 PLAYBACK)-SAVEYUV\n");
    printf("-i, --input   input video file path\n");
    printf("-o, --output  yuv file path default\n");
    printf("-f, --fps     set H26x decode speed, just for debug (default get fps from stream)\n");
    printf("-c, --chnid   decoder channel id [0,40), default 0\n");
    printf("-n, --num     frames to decode default %u\n", -1);
    printf("-m, --chn     channel number %u\n", 1);
    printf("-s, --source  configure vb buffer mode, (user/module/private), default private mode\n");
    printf("-l, --save    save the output file: 0:no save; 1:only half; 2:all %u\n", 0);
    printf("-H, --help    print this help\n");

    printf("example:\n");
    printf("Usage : %s -t 1 -c 0 -n -1 -l 0 -i /media/1080P.mp4 -o /media/debug.yuv\n", sPrgNm);
}

AR_S32 SAMPLE_H26X_VDEC_VPSS_VO(task_param *param)
{
    VB_CONFIG_S stVbConfig;
    AR_S32 i, s32Ret = AR_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[TEST_VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize = {0};
    VO_LAYER VoLayer = {0};
    AR_U32 u32VdecChnNum, VpssGrpNum;
    VPSS_GRP VpssGrp = 0;
    pthread_t   VdecThread[2*TEST_VDEC_MAX_CHN_NUM];
    PIC_SIZE_E enDispPicSize = {0};
    SAMPLE_VDEC_ATTR astSampleVdec[TEST_VDEC_MAX_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    SAMPLE_VO_CONFIG_S stVoConfig = {0};
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    AR_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    VO_INTF_SYNC_E enIntfSync = {0};
    VO_INTF_TYPE_E enVoIntfType = {0};

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/

    enDispPicSize = PIC_1080P;
    enIntfSync    = VO_OUTPUT_1080P60;
    enVoIntfType  = VO_INTF_HDMI;

    if (access(param->streamFile, R_OK)) {
        logerr("streamFile %s is not accessible: %s, exit\n", param->streamFile, strerror(errno));
        return -1;
    }

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if (s32Ret != AR_SUCCESS)
    {
        logerr("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    if (param->vbSource == VB_SOURCE_MODULE) {
        memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
        stVbConfig.u32MaxPoolCnt             = 1;
        stVbConfig.astCommPool[0].u32BlkCnt  = 20*u32VdecChnNum;
#if 0
        stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
#else
         // For some resolutions, using size by API COMMON_GetPicBufferSize is not enough for VDEC
         stVbConfig.astCommPool[0].u64BlkSize = VDEC_GetPicBufferSize(param->attr.enType, param->attr.u32Width,
                                                param->attr.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                                DATA_BITWIDTH_8, 0);
#endif
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
        if (s32Ret != AR_SUCCESS)
        {
            logerr("init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        s32Ret = AR_MPI_SYS_Init();
        if (AR_SUCCESS != s32Ret)
        {
            logerr("init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    }


    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for (i = 0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = param->attr.enType;//PT_H265;
        astSampleVdec[i].u32Width                         = param->attr.u32Width;//1920;
        astSampleVdec[i].u32Height                        = param->attr.u32Height;//1080;
        astSampleVdec[i].vbSource                         = param->vbSource;
        astSampleVdec[i].s32ChnId                         = i + param->startChnId;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IPB;
        astSampleVdec[i].stSapmleVdecVideo.enOutputOrder  = VIDEO_OUTPUT_ORDER_DISP;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth = 4;
        astSampleVdec[i].u32DisplayFrameNum               = 9;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = (param->attr.enType == PT_H265 || param->attr.enType == PT_H264) ? 6 : 0;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }

    if (param->vbSource == VB_SOURCE_USER || param->vbSource == VB_SOURCE_MODULE) {
        s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
        if (s32Ret != AR_SUCCESS)
        {
            logerr("init mod common vb fail for %#x!\n", s32Ret);
            goto END2;
        }
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if (s32Ret != AR_SUCCESS)
    {
        logerr("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

    //ENABLE_VPSS
    if (param->vbSource == VB_SOURCE_MODULE) {
        /************************************************
        step4:  start VPSS
        *************************************************/
        //--------xy
        //stVpssGrpAttr.u32MaxW = 2688;
        //stVpssGrpAttr.u32MaxH = 2160;

        stVpssGrpAttr.u32Width = param->attr.u32Width;
        stVpssGrpAttr.u32Height = param->attr.u32Height;
        //--------xy

        stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
        stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
        stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
        stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_PLANAR_420;


        //--------xy
        //stVpssGrpAttr.bNrEn   = AR_FALSE;
        //--------xy

        memset(abChnEnable, 0, sizeof(abChnEnable));
        abChnEnable[1] = AR_TRUE;
        astVpssChnAttr[1].u32Width                    = stDispSize.u32Width;
        astVpssChnAttr[1].u32Height                   = stDispSize.u32Height;
        astVpssChnAttr[1].enChnMode                   = VPSS_CHN_MODE_USER;
        astVpssChnAttr[1].enCompressMode              = COMPRESS_MODE_NONE;
        astVpssChnAttr[1].enDynamicRange              = DYNAMIC_RANGE_SDR8;
        astVpssChnAttr[1].enPixelFormat               = PIXEL_FORMAT_YVU_PLANAR_420;
        astVpssChnAttr[1].stFrameRate.s32SrcFrameRate = -1;
        astVpssChnAttr[1].stFrameRate.s32DstFrameRate = -1;
        astVpssChnAttr[1].u32Depth                    = 2;
        astVpssChnAttr[1].bMirror                     = AR_FALSE;
        astVpssChnAttr[1].bFlip                       = AR_FALSE;
        astVpssChnAttr[1].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
        astVpssChnAttr[1].enVideoFormat               = VIDEO_FORMAT_LINEAR;
        for (i = 0; i<u32VdecChnNum; i++)
        {
            VpssGrp = i;
            s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &abChnEnable[0], &stVpssGrpAttr, &astVpssChnAttr[0]);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("start VPSS fail for %#x!\n", s32Ret);
                goto END4;
            }
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        printf("vpss is disabled\n");
    }

    /************************************************
    step5:  start VO
    *************************************************/
    stVoConfig.VoDev                 = SAMPLE_VO_DEV_UHD;
    stVoConfig.enVoIntfType          = enVoIntfType;
    stVoConfig.enIntfSync            = enIntfSync;
    stVoConfig.enPicSize             = enDispPicSize;
    stVoConfig.u32BgColor            = COLOR_RGB_BLUE;
    stVoConfig.u32DisBufLen          = 3;
    stVoConfig.enDstDynamicRange     = DYNAMIC_RANGE_SDR8;
    stVoConfig.enVoMode              = VO_MODE_1MUX;
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_PLANAR_420;
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_PLANAR_420;
    stVoConfig.stDispRect.s32X       = 0;
    stVoConfig.stDispRect.s32Y       = 0;
    stVoConfig.stDispRect.u32Width   = stDispSize.u32Width;
    stVoConfig.stDispRect.u32Height  = stDispSize.u32Height;
    stVoConfig.stImageSize.u32Width  = stDispSize.u32Width;
    stVoConfig.stImageSize.u32Height = stDispSize.u32Height;
    stVoConfig.enVoPartMode          = VO_PART_MODE_SINGLE;

    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (s32Ret != AR_SUCCESS)
    {
        logerr("start VO fail for %#x!\n", s32Ret);
        goto END5;
    }

    //ENABLE_VPSS
    if (param->vbSource == VB_SOURCE_MODULE) {
        /************************************************
        step6:  VDEC bind VPSS
        *************************************************/
        for (i = 0; i<u32VdecChnNum; i++)
        {
            s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(param->startChnId, i);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("vdec bind vpss fail for %#x!\n", s32Ret);
                goto END6;
            }
        }

        /************************************************
        step7:  VPSS bind VO
        *************************************************/
        VoLayer = stVoConfig.VoDev;
        for (i = 0; i<VpssGrpNum; i++)
        {
            s32Ret = SAMPLE_COMM_VPSS_Bind_VO(i, 1, VoLayer, i);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("vpss bind vo fail for %#x!\n", s32Ret);
                goto END7;
            }
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        VoLayer = stVoConfig.VoDev;
        for (i = 0; i<u32VdecChnNum; i++)
        {
            s32Ret = SAMPLE_COMM_VDEC_Bind_VO(param->startChnId, VoLayer, i);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("vdec bind vo fail for %#x!\n", s32Ret);
                goto END6;
            }
        }
    }

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for (i = 0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), param->streamFile);
        snprintf(stVdecSend[i].cOutName, sizeof(stVdecSend[i].cOutName), param->yuvFile);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = param->startChnId;
        stVdecSend[i].s32IntervalTime = param->fps > 0 ? 1000 / param->fps : 33; //ms
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = AR_TRUE;
        stVdecSend[i].s32MilliSec     = 0;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
        stVdecSend[i].numFrameToDecode= param->numFrameToDecode;
    }

    SAMPLE_COMM_VDEC_StartSendStreamMP4(u32VdecChnNum, &stVdecSend[0], VdecThread);
    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);
END7:
    for (i = 0; i<VpssGrpNum; i++)
    {
        //ENABLE_VPSS
        if (param->vbSource == VB_SOURCE_MODULE) {
            s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("vpss unbind vo fail for %#x!\n", s32Ret);
            }
        }
    }

END6:
    for (i = 0; i<u32VdecChnNum; i++)
    {
        //ENABLE_VPSS
        if (param->vbSource == VB_SOURCE_MODULE) {
            s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(param->startChnId, i);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("vdec unbind vpss fail for %#x!\n", s32Ret);
            }
        } else if (param->vbSource == VB_SOURCE_USER ||
                   param->vbSource == VB_SOURCE_PRIVATE) {
            s32Ret = SAMPLE_COMM_VDEC_UnBind_VO(param->startChnId, VoLayer, i);
            if (s32Ret != AR_SUCCESS)
            {
                logerr("vdec[%d] unbind vo[%d] fail for %#x!\n", i, i, s32Ret);
            }
        }
    }

END5:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

END4:
    for (i = VpssGrp; i >= 0; i--)
    {
        VpssGrp = i;
        if (param->vbSource == VB_SOURCE_MODULE) {
            SAMPLE_COMM_VPSS_Stop(VpssGrp, &abChnEnable[0]);
        }
    }
END3:
    SAMPLE_COMM_VDEC_Stop(param->chnNum, &astSampleVdec[0]);

END2:
    if (param->vbSource == VB_SOURCE_USER || param->vbSource == VB_SOURCE_MODULE) {
        SAMPLE_COMM_VDEC_ExitVBPool(&astSampleVdec[0]);
    }

END1:
    if (param->vbSource == VB_SOURCE_MODULE) {
        SAMPLE_COMM_SYS_Exit();
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        AR_MPI_SYS_Exit();
    }

    return s32Ret;
}

AR_S32 SAMPLE_STREAM_VDEC_To_YUV(task_param *param)
{
    VB_CONFIG_S stVbConfig;
    AR_S32 i, s32Ret = AR_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[2*TEST_VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize;
    /*VO_LAYER VoLayer;*/
    AR_U32 u32VdecChnNum;//, VpssGrpNum = 0;
    /*VPSS_GRP VpssGrp;*/
    pthread_t   VdecThread[2*TEST_VDEC_MAX_CHN_NUM];

    PIC_SIZE_E enDispPicSize;
    SAMPLE_VDEC_ATTR astSampleVdec[2*TEST_VDEC_MAX_CHN_NUM];
    /*
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM];
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    AR_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    VO_INTF_SYNC_E enIntfSync;
    VO_INTF_TYPE_E enVoIntfType;
    */

    u32VdecChnNum = param->chnNum;
    //VpssGrpNum    = u32VdecChnNum;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/

    enDispPicSize = PIC_1080P;
    /*
    enIntfSync    = VO_OUTPUT_1080P60;
    enVoIntfType  = VO_INTF_HDMI;
    */


    if (access(param->streamFile, R_OK)) {
        logerr("streamFile %s is not accessible: %s, exit\n", param->streamFile, strerror(errno));
        return -1;
    }

    /*
    FILE *tmpfp = fopen(param->yuvFile, "w");
    if (!tmpfp) {
        logerr("yuvFile %s cannot be opened for writing: %s, exit\n", param->yuvFile, strerror(errno));
    } else
        fclose(tmpfp);
    */

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if (s32Ret != AR_SUCCESS)
    {
        logerr("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    if (param->vbSource == VB_SOURCE_MODULE) {
        memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
        stVbConfig.u32MaxPoolCnt             = 1;
        stVbConfig.astCommPool[0].u32BlkCnt  = 20*u32VdecChnNum;
        stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
        if (s32Ret != AR_SUCCESS)
        {
            logerr("init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        s32Ret = AR_MPI_SYS_Init();
        if (AR_SUCCESS != s32Ret)
        {
            logerr("init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for (i = 0; i<u32VdecChnNum; i++)
    {
        logline("param->attr.enMode = %d", param->attr.enMode);
        astSampleVdec[i].enType                           = param->attr.enType;//PT_H265;
        astSampleVdec[i].u32Width                         = param->attr.u32Width;//1920;
        astSampleVdec[i].u32Height                        = param->attr.u32Height;//1080;
        astSampleVdec[i].vbSource                         = param->vbSource;
        astSampleVdec[i].s32ChnId                         = i + param->startChnId;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IPB;
        astSampleVdec[i].stSapmleVdecVideo.enOutputOrder  = VIDEO_OUTPUT_ORDER_DISP;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth = 4;
        astSampleVdec[i].u32DisplayFrameNum               = (param->attr.enType == PT_H265 || param->attr.enType == PT_H264) ? 6 : 3;
        astSampleVdec[i].u32ScaledWidth                   = param->attr.u32ScaledWidth;
        astSampleVdec[i].u32ScaledHeight                  = param->attr.u32ScaledHeight;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = (param->attr.enType == PT_H265 || param->attr.enType == PT_H264) ? 6 : 0;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }

    if (param->vbSource == VB_SOURCE_USER || param->vbSource == VB_SOURCE_MODULE) {
        s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
        if (s32Ret != AR_SUCCESS)
        {
            logerr("init mod common vb fail for %#x!\n", s32Ret);
            goto END2;
        }
    }

    /************************************************
    step3:  start VDEC
    *************************************************/

    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if (s32Ret != AR_SUCCESS)
    {
        logerr("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

    /************************************************
    step8:  send stream to VDEC
    ************************************************/
    for (i = 0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), param->streamFile);
        snprintf(stVdecSend[i].cOutName, sizeof(stVdecSend[i].cOutName), "%s_%i", param->yuvFile, i);
        if (param->saveFile == 2)
            stVdecSend[i].s32SaveFile = 1;
        else if (param->saveFile == 0)
            stVdecSend[i].s32SaveFile = 0;
        else if (i%2 == 0)
            stVdecSend[i].s32SaveFile = 1;
        else
            stVdecSend[i].s32SaveFile = 0;
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i + param->startChnId;
        stVdecSend[i].s32IntervalTime = param->fps > 0 ? 1000 / param->fps : 33; //ms
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = AR_TRUE;
        stVdecSend[i].s32MilliSec     = 10*1000;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
        stVdecSend[i].numFrameToDecode= param->numFrameToDecode;
    }

    SAMPLE_COMM_VDEC_StartSendStreamMP4(u32VdecChnNum, &stVdecSend[0], VdecThread);

    sleep(1);

    SAMPLE_COMM_VDEC_StartGetPic(u32VdecChnNum, &stVdecSend[0], VdecThread);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    sleep(1);

    SAMPLE_COMM_VDEC_StopGetPic(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

END3:
    SAMPLE_COMM_VDEC_Stop(param->chnNum, &astSampleVdec[0]);

END2:
    if (param->vbSource == VB_SOURCE_USER || param->vbSource == VB_SOURCE_MODULE) {
        SAMPLE_COMM_VDEC_ExitVBPool(&astSampleVdec[0]);
    }

END1:
    if (param->vbSource == VB_SOURCE_MODULE) {
       SAMPLE_COMM_SYS_Exit();
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        AR_MPI_SYS_Exit();
    }

    return s32Ret;
}
static struct option long_options[] = {
    {"type",          required_argument, NULL, 't'},
    {"input",         required_argument, NULL, 'i'},
    {"output",        required_argument, NULL, 'o'},
    {"num",           required_argument, NULL, 'n'},
    {"fps",           required_argument, NULL, 'f'},
    {"source of vb",  required_argument, NULL, 's'},
    {"chnid",         required_argument, NULL, 'c'},
    {"help",          no_argument,       NULL, 'H'},
    {NULL, 0, NULL,   0}
};

/******************************************************************************
* function    : main()
* Description : video vdec sample
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 file_len = 0;
    AR_S32 s32Ret = AR_SUCCESS, s32Pipeline = 1, c = 0;
    task_param param = {0};
    param.numFrameToDecode = -1;
    param.chnNum = 1;
    param.attr.enType = PT_H265;
    param.attr.u32Width = 1920;
    param.attr.u32Height = 1080;
    param.attr.u32ScaledWidth = 0;
    param.attr.u32ScaledHeight = 0;
    param.vbSource = VB_SOURCE_PRIVATE;
    param.saveFile = 1;
    ar_log_shm_set_enabled(1);

    while ((c = getopt_long(argc, argv, ":t:i:o:f:n:m:s:c:l:a", long_options, NULL)) != EOF) {
        switch (c) {
        case 't':
            s32Pipeline = atoi(optarg);
            break;
        case 'i':
            file_len = strlen(optarg) >= FILENAME_LEN - 1 ? FILENAME_LEN - 1 : strlen(optarg);
            strncpy(param.streamFile, optarg, file_len);
            param.streamFile[file_len] = '\0';
            break;
        case 'o':
            file_len = strlen(optarg) >= FILENAME_LEN - 1 ? FILENAME_LEN -1 : strlen(optarg);
            strncpy(param.yuvFile, optarg, file_len);
            param.yuvFile[file_len] = '\0';
            break;
        case 'n':
            param.numFrameToDecode = atoi(optarg);
            printf("numFrameToDecode=%d\n", param.numFrameToDecode);
            break;
        case 'm':
            param.chnNum = atoi(optarg);
            printf("chnNum=%d\n", param.chnNum);
            break;
        case 'f':
            param.fps = atoi(optarg);
            printf("fps=%d\n", param.fps);
            break;
        case 'l':
            param.saveFile = atoi(optarg);
            printf("save file type=%d\n", param.saveFile);
            break;
        case 'c':
            param.startChnId = atoi(optarg);
            printf("startChnId=%d\n", param.startChnId);
            break;
        case 's':
            if (!strncmp(optarg, "user", 4)) {
                printf("vb source set to user\n");
                param.vbSource = VB_SOURCE_USER;
            } else if (!strncmp(optarg, "module", 6)) {
                printf("vb source set to module\n");
                param.vbSource = VB_SOURCE_MODULE;
            } else if (!strncmp(optarg, "private", 7)) {
                printf("vb source set to private\n");
                param.vbSource = VB_SOURCE_PRIVATE;
            } else {
                printf("unknown vb source, defaults to private\n");
                param.vbSource = VB_SOURCE_PRIVATE;
            }
            break;
        case 'H':
        default:
            /* ignore unknown options */
            SAMPLE_VDEC_Usage(argv[0]);
            exit(0);
            break;
        }
    }

    if (param.streamFile[0] == '\0') {
        printf("you should specify a mp4 file to play\n");
        SAMPLE_VDEC_Usage(argv[0]);
        return AR_FAILURE;
    }

    if (argc < 2)
    {
        SAMPLE_VDEC_Usage(argv[0]);
        return AR_FAILURE;
    }

    for (int i = param.startChnId; i < param.chnNum + param.startChnId; i++) {
        AR_S32 video_index = -1;
        g_fmtCtx[i] = read_mp4_info(param.streamFile, &param.attr.enType, &param.attr.u32Width, &param.attr.u32Height, &param.fps, &video_index);
        if (g_fmtCtx[i] == NULL || video_index == -1) {
            printf("read mp4 file %s failed\n", param.streamFile);
            return AR_FAILURE;
        }

        g_videoindex[i] = video_index;
        printf("width = %d, height = %d, codec = %d, fps = %d\n", param.attr.u32Width, param.attr.u32Height, param.attr.enType, param.fps);
    }

    s32Ret = ar_log_init();
    if (s32Ret)
    {
        printf("log init failed\n");
        return s32Ret;
    }

    signal(SIGINT, SAMPLE_VDEC_HandleSig);
    signal(SIGTERM, SAMPLE_VDEC_HandleSig);

    /******************************************
     choose the case
    ******************************************/
    switch (s32Pipeline)
    {
        case 0:
        {
            s32Ret = SAMPLE_H26X_VDEC_VPSS_VO(&param);
            break;
        }
        case 1:
        {
            s32Ret = SAMPLE_STREAM_VDEC_To_YUV(&param);
            break;
        }
        default :
        {
            SAMPLE_VDEC_Usage(argv[0]);
            s32Ret = AR_FAILURE;
            break;
        }
    }

    if (AR_SUCCESS == s32Ret)
    {
        printf("program exit normally!\n");
    }
    else
    {
        printf("program exit abnormally!\n");
    }

    return s32Ret;
}
