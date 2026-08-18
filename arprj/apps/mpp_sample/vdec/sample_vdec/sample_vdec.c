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
#include <libavutil/avutil.h>

#include "osal.h"
#include "sample_comm.h"

#define FILENAME_LEN (128)

extern int task_done[TEST_VDEC_MAX_CHN_NUM];

typedef struct task_param {
    AR_BOOL             circleSend;
    AR_BOOL             recoverMode;
    AR_BOOL             miniBufMode;
    AR_BOOL             cacheMode;
    AR_CHAR             yuvFile[FILENAME_LEN];
    AR_CHAR             streamFile[FILENAME_LEN];
    AR_S32              fps;
    AR_S32              startChnId;
    AR_S32              discardMode;
    AR_S32              numFrameToDecode;
    AR_S32              chnId;
    AR_S32              chnNum;
    AR_S32              saveFile;
    AR_S32              showUserData;
    AR_U32              errConcealMode;
    AR_U32              errConcealUnit;
    SAMPLE_VDEC_ATTR    attr;
    VB_SOURCE_E         vbSource;
} task_param;

AR_VOID SAMPLE_VDEC_HandleSig(AR_S32 signo)
{
    if (SIGINT == signo || SIGTSTP == signo || SIGTERM == signo)
    {
        //SAMPLE_COMM_VO_HdmiStop();
        //SAMPLE_COMM_SYS_Exit();
        //printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        printf("set task_done to 1\n");
        for (int i=0;i<TEST_VDEC_MAX_CHN_NUM;i++)
            task_done[i]=1;
    }

    //exit(0);
}

AR_VOID SAMPLE_VDEC_Usage(char *sPrgNm)
{
    printf("Usage : %s [OPTION]\n\n", sPrgNm);
    printf("-t, --type    video pipeline type default 1, possible values as follows:\n");
    printf("\t0:  VDEC(H264 or H265 PLAYBACK)-VPSS-VO\n");
    //printf("\t2:  VDEC(JPEG PLAYBACK)-VPSS-VO\n");
    printf("\t1:  VDEC(H264 or H265 PLAYBACK)-SAVEYUV\n");
    printf("-i, --input   input video file path\n");
    printf("-o, --output  yuv file path\n");
    printf("-w, --width   video width default 1920\n");
    printf("-h, --height  video height default 1080\n");
    printf("-x, --scaledW scale width of picture down in luma samples. ceil8(width/8) <= scaledW <= width\n");
    printf("-y, --scaledH scale height of picture down in luma samples. ceil8(height/8) <= scaledH <= height\n");
    printf("-r, --recover configure whether to reset current channel when decoder stuck(Debug use), default 0(to save failure scene)\n");
    printf("-b, --miniBuf configure whether to save mmz & vb buffer, default 0(Sometimes, miniBuf mode will lost decoder performance)\n");
    printf("-c, --chnid   decoder channel id [0,40), default 0\n");
    printf("-C, --cache   decoder framebuffer cached or not, default disable cache\n");
    printf("-d, --debug   ffmpeg debug level, default 0\n");
    printf("-s, --source  configure vb buffer mode, (user/module/private), default private mode\n");
    printf("-D, --rand    discard frames which frame_id mode rand_num equal 0, only for H26x, default 0(not discard)\n");
    printf("-e, --codec   input video type default h265 (h264, h265, jpeg, mjpeg)\n");
    printf("-f, --format  decoder yuv format default 420p (420p 422p 444p)\n");
    printf("-F, --fps     set H26x decode speed, just for debug (default get fps from stream)\n");
    printf("-n, --num     frames to decode default %u\n", -1);
    printf("-m, --chn     channel number %u\n", 1);
    printf("-u, --userdata show userata, default not\n");
    printf("-l, --save    save the output file: 0:no save; 1:only half; 2:all; 3:one yuv per file\n");
    printf("-L, --loop    loop play\n");
    printf("-U, --err-conceal-unit     error concealment unit, default 0(conceal off)\n");
    printf("-M, --err-conceal-mode     error concealment mode, default 0(conceal off)\n");
    printf("-H, --help    print this help\n");

    printf("example:\n");
    printf("Usage : %s -t 1 -c 0 -m 1 -w 1920 -h 1080 -e h265 -n -1 -l 0 -i /media/1080P.h265 -o /media/debug.yuv\n", sPrgNm);
}

AR_S32 SAMPLE_H26X_VDEC_VPSS_VO(task_param *param)
{
    VB_CONFIG_S stVbConfig = {0};
    AR_S32 i, s32Ret = AR_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[TEST_VDEC_MAX_CHN_NUM] = {0};
    SIZE_S stDispSize = {0};
    VO_LAYER VoLayer = {0};
    AR_U32 u32VdecChnNum, VpssGrpNum;
    VPSS_GRP VpssGrp = 0;
    pthread_t VdecThread[2*TEST_VDEC_MAX_CHN_NUM];
    PIC_SIZE_E enDispPicSize = {0};
    SAMPLE_VDEC_ATTR astSampleVdec[TEST_VDEC_MAX_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    SAMPLE_VO_CONFIG_S stVoConfig = {0};
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    AR_BOOL abChnEnable[VPSS_MAX_CHN_NUM] = {0};
    VO_INTF_SYNC_E enIntfSync = {0};
    VO_INTF_TYPE_E enVoIntfType = {0};

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/

#if 0
    enDispPicSize = PIC_3840x2160;
    enIntfSync    = VO_OUTPUT_3840x2160_30;
#else
    enDispPicSize = PIC_1080P;
    enIntfSync    = VO_OUTPUT_1080P60;
#endif
    enVoIntfType  = VO_INTF_HDMI;

    if (access(param->streamFile, R_OK)) {
        printf("streamFile %s is not accessible: %s, exit\n", param->streamFile, strerror(errno));
        return -1;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: sys get pic size failed for %#x!\n", s32Ret);
        goto END1;
    }

    if (param->vbSource == VB_SOURCE_MODULE) {
        memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
        stVbConfig.u32MaxPoolCnt             = 1;
        stVbConfig.astCommPool[0].u32BlkCnt  = 20*u32VdecChnNum;
#if 0
        stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                    PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
#else
        // For some resolutions, using size by API COMMON_GetPicBufferSize is not enough for VDEC
        stVbConfig.astCommPool[0].u64BlkSize = VDEC_GetPicBufferSize(param->attr.enType, param->attr.u32Width,
                                               param->attr.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                               DATA_BITWIDTH_8, 0);
#endif
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        s32Ret = AR_MPI_SYS_Init();
        if (AR_SUCCESS != s32Ret)
        {
            printf("sample_vdec.c: init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = param->attr.enType;
        astSampleVdec[i].u32Width                         = param->attr.u32Width;
        astSampleVdec[i].u32Height                        = param->attr.u32Height;
        astSampleVdec[i].vbSource                         = param->vbSource;
        astSampleVdec[i].s32ChnId                         = i + param->startChnId;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].u32ScaledWidth                   = param->attr.u32ScaledWidth;
        astSampleVdec[i].u32ScaledHeight                  = param->attr.u32ScaledHeight;
        astSampleVdec[i].u32CacheMode                     = param->cacheMode;

        if (param->attr.enType == PT_H265 || param->attr.enType == PT_H264) {
            // For VDEC->VO, to gain better performance, u32DisplayFrameNum should be greater than delay + 4
            astSampleVdec[i].u32DisplayFrameNum               = param->attr.u32Width > 1280 ? 15 : 14;
            astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IPB;
            astSampleVdec[i].stSapmleVdecVideo.enOutputOrder  = VIDEO_OUTPUT_ORDER_DISP;
            astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
            astSampleVdec[i].stSapmleVdecVideo.errConcealMode = param->errConcealMode;
            astSampleVdec[i].stSapmleVdecVideo.errConcealUnit = param->errConcealUnit;
            astSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth = 4;
            astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 16;
            astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
        } else {
            astSampleVdec[i].u32DisplayFrameNum                = 3;
            astSampleVdec[i].u32FrameBufCnt                    = 4;
            astSampleVdec[i].stSapmleVdecPicture.enPixelFormat = param->attr.stSapmleVdecPicture.enPixelFormat;
        }
    }

    if (param->vbSource == VB_SOURCE_USER || param->vbSource == VB_SOURCE_MODULE) {
        s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: init mod common vb fail for %#x!\n", s32Ret);
            goto END2;
        }
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

    // ENABLE_VPSS
    if (param->vbSource == VB_SOURCE_MODULE) {
        /************************************************
        step4:  start VPSS
        *************************************************/
        printf("vpss is enabled\n");
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
        for(i=0; i<u32VdecChnNum; i++)
        {
            VpssGrp = i;
            s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &abChnEnable[0], &stVpssGrpAttr, &astVpssChnAttr[0]);
            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: start VPSS fail for %#x!\n", s32Ret);
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
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VO fail for %#x!\n", s32Ret);
        goto END5;
    }

    // ENABLE_VPSS
    if (param->vbSource == VB_SOURCE_MODULE) {
        /************************************************
        step6:  VDEC bind VPSS
        *************************************************/
        for(i=0; i<u32VdecChnNum; i++)
        {
            s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(param->startChnId, i);
            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: vdec bind vpss fail for %#x!\n", s32Ret);
                goto END6;
            }
        }

        /************************************************
        step7:  VPSS bind VO
        *************************************************/
        VoLayer = stVoConfig.VoDev;
        for(i=0; i<VpssGrpNum; i++)
        {
            s32Ret = SAMPLE_COMM_VPSS_Bind_VO(i, 1, VoLayer, i);
            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: vpss bind vo fail for %#x!\n", s32Ret);
                goto END7;
            }
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        VoLayer = stVoConfig.VoDev;
        for(i=0; i<u32VdecChnNum; i++)
        {
            s32Ret = SAMPLE_COMM_VDEC_Bind_VO(param->startChnId, VoLayer, i);

            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: vdec bind vo fail for %#x!\n", s32Ret);
                goto END6;
            }
        }
    }

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), param->streamFile);
        snprintf(stVdecSend[i].cOutName, sizeof(stVdecSend[i].cOutName), param->yuvFile);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = param->startChnId;
        stVdecSend[i].u32Fps          = param->fps;
        stVdecSend[i].s32ChnId        = i + param->startChnId;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 40;
        stVdecSend[i].s32IntervalTime = 1000*40;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = AR_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
        stVdecSend[i].numFrameToDecode= -1;
    }

    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], VdecThread);
    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);
    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);
END7:
    // ENABLE_VPSS
    if (param->vbSource == VB_SOURCE_MODULE) {
        for(i=0; i<VpssGrpNum; i++)
        {
            s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: vpss unbind vo fail for %#x!\n", s32Ret);
            }
        }
    }

END6:
    for(i=0; i<u32VdecChnNum; i++)
    {
        // ENABLE_VPSS
        if (param->vbSource == VB_SOURCE_MODULE) {
            s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(param->startChnId, i);
            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: vdec unbind vpss fail for %#x!\n", s32Ret);
            }
        } else if (param->vbSource == VB_SOURCE_USER ||
                   param->vbSource == VB_SOURCE_PRIVATE) {
            s32Ret = SAMPLE_COMM_VDEC_UnBind_VO(param->startChnId, VoLayer, i);
            if(s32Ret != AR_SUCCESS)
            {
                printf("sample_vdec.c: vdec[%d] unbind vo[%d] fail for %#x!\n", i, i, s32Ret);
            }
        }
    }

END5:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

END4:
    // ENABLE_VPSS
    if (param->vbSource == VB_SOURCE_MODULE) {
        for(i = VpssGrp; i >= 0; i--)
        {
            VpssGrp = i;
            SAMPLE_COMM_VPSS_Stop(VpssGrp, &abChnEnable[0]);
        }
    }
END3:
    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum, &astSampleVdec[0]);

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

#if 0
AR_S32 SAMPLE_H264_VDEC_VPSS_VO(task_param *param)
{
    VB_CONFIG_S stVbConfig;
    AR_S32 i, s32Ret = AR_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[TEST_VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize;
    VO_LAYER VoLayer;
    AR_U32 u32VdecChnNum, VpssGrpNum;
    VPSS_GRP VpssGrp = 0;
    pthread_t   VdecThread[2*TEST_VDEC_MAX_CHN_NUM];
    PIC_SIZE_E enDispPicSize;
    SAMPLE_VDEC_ATTR astSampleVdec[TEST_VDEC_MAX_CHN_NUM];
    /*
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM];
    */
    SAMPLE_VO_CONFIG_S stVoConfig;
    /*
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    AR_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    */
    VO_INTF_SYNC_E enIntfSync;
    VO_INTF_TYPE_E enVoIntfType;

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
    enDispPicSize = PIC_1080P;
    enIntfSync    = VO_OUTPUT_1080P60;
    enVoIntfType  = VO_INTF_HDMI;
    if (access(param->streamFile, R_OK)) {
        printf("streamFile %s is not accessible: %s, exit\n", param->streamFile, strerror(errno));
        return -1;
    }

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 1;
    stVbConfig.astCommPool[0].u32BlkCnt  = 10*u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = param->attr.enType;//PT_H264;
        astSampleVdec[i].u32Width                         = param->attr.u32Width;//1920;
        astSampleVdec[i].u32Height                        = param->attr.u32Height;//1080;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IPB;
        astSampleVdec[i].stSapmleVdecVideo.enOutputOrder  = VIDEO_OUTPUT_ORDER_DEC;
        astSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth = 4;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].u32DisplayFrameNum               = 3;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = (param->attr.enType == PT_H265 || param->attr.enType == PT_H264) ? 6 : 0;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

#if ENABLE_VPSS
    /************************************************
    step4:  start VPSS
    *************************************************/
    //--------xy
    //stVpssGrpAttr.u32MaxW = 2688;
    //stVpssGrpAttr.u32MaxH = 2160;

    stVpssGrpAttr.u32Width = 2688;
    stVpssGrpAttr.u32Height = 2160;
    //--------xy

    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

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
    astVpssChnAttr[1].enPixelFormat               = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    astVpssChnAttr[1].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[1].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[1].u32Depth                    = 0;
    astVpssChnAttr[1].bMirror                     = AR_FALSE;
    astVpssChnAttr[1].bFlip                       = AR_FALSE;
    astVpssChnAttr[1].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
    astVpssChnAttr[1].enVideoFormat               = VIDEO_FORMAT_LINEAR;

    for(i=0; i<u32VdecChnNum; i++)
    {
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &abChnEnable[0], &stVpssGrpAttr, &astVpssChnAttr[0]);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: start VPSS fail for %#x!\n", s32Ret);
            goto END4;
        }
    }
#else
    printf("vpss is disabled\n");
#endif

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
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_PLANAR_420;
    stVoConfig.stDispRect.s32X       = 0;
    stVoConfig.stDispRect.s32Y       = 0;
    stVoConfig.stDispRect.u32Width   = stDispSize.u32Width;
    stVoConfig.stDispRect.u32Height  = stDispSize.u32Height;
    stVoConfig.stImageSize.u32Width  = stDispSize.u32Width;
    stVoConfig.stImageSize.u32Height = stDispSize.u32Height;
    stVoConfig.enVoPartMode          = VO_PART_MODE_SINGLE;
    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VO fail for %#x!\n", s32Ret);
        goto END5;
    }

#if ENABLE_VPSS
    /************************************************
    step6:  VDEC bind VPSS
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(i, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vdec bind vpss fail for %#x!\n", s32Ret);
            goto END6;
        }
    }

    /************************************************
    step7:  VPSS bind VO
    *************************************************/
    VoLayer = stVoConfig.VoDev;
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_Bind_VO(i, 1, VoLayer, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vpss bind vo fail for %#x!\n", s32Ret);
            goto END7;
        }
    }
#else
    VoLayer = stVoConfig.VoDev;
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_Bind_VO(i, VoLayer, i);

        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vdec bind vo fail for %#x!\n", s32Ret);
            goto END6;
        }
    }
#endif

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), param->streamFile);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].s32IntervalTime = 1000*33;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 40;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = AR_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }

    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], VdecThread);
    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);
/*
END7:
*/
    for(i=0; i<VpssGrpNum; i++)
    {
#if ENABLE_VPSS
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vpss unbind vo fail for %#x!\n", s32Ret);
        }
#endif
    }

END6:
    for(i=0; i<u32VdecChnNum; i++)
    {
#if ENABLE_VPSS
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vdec unbind vpss fail for %#x!\n", s32Ret);
        }
#else
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VO(i, VoLayer, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vdec[%d] unbind vo[%d] fail for %#x!\n", i, i, s32Ret);
        }
#endif
    }

END5:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

/*
END4:
*/
    for(i = VpssGrp; i >= 0; i--)
    {
        VpssGrp = i;
#if ENABLE_VPSS
        SAMPLE_COMM_VPSS_Stop(VpssGrp, &abChnEnable[0]);
#endif
    }
END3:
    SAMPLE_COMM_VDEC_Stop(param->startChnId, &astSampleVdec[0]);

END2:
    SAMPLE_COMM_VDEC_ExitVBPool(&astSampleVdec[0]);

END1:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

AR_S32 SAMPLE_JPEG_VDEC_VPSS_VO(AR_VOID)
{
    VB_CONFIG_S stVbConfig;
    AR_S32 i, s32Ret = AR_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[TEST_VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize;
    VO_LAYER VoLayer;
    AR_U32 u32VdecChnNum, VpssGrpNum;
    VPSS_GRP VpssGrp;
    pthread_t   VdecThread[2*TEST_VDEC_MAX_CHN_NUM];
    PIC_SIZE_E enDispPicSize;
    SAMPLE_VDEC_ATTR astSampleVdec[TEST_VDEC_MAX_CHN_NUM];
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM];
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    AR_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    VO_INTF_SYNC_E enIntfSync;
    VO_INTF_TYPE_E enVoIntfType;

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
  enDispPicSize = PIC_1080P;
  enIntfSync    = VO_OUTPUT_1080P60;
  enVoIntfType  = VO_INTF_HDMI;

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 1;
    stVbConfig.astCommPool[0].u32BlkCnt  = 10*u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                            = PT_JPEG;
        astSampleVdec[i].u32Width                          = 1920;
        astSampleVdec[i].u32Height                         = 1080;
        astSampleVdec[i].enMode                            = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        astSampleVdec[i].stSapmleVdecPicture.u32Alpha      = 255;
        astSampleVdec[i].u32DisplayFrameNum                = 3;
        astSampleVdec[i].u32FrameBufCnt                    = astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

    /************************************************
    step4:  start VPSS
    *************************************************/
    //--------xy
    //stVpssGrpAttr.u32MaxW = 2688;
    //stVpssGrpAttr.u32MaxH = 2160;

    stVpssGrpAttr.u32Width = 2688;
    stVpssGrpAttr.u32Height = 2160;

    //--------xy

    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

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
    astVpssChnAttr[1].enPixelFormat               = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    astVpssChnAttr[1].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[1].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[1].u32Depth                    = 0;
    astVpssChnAttr[1].bMirror                     = AR_FALSE;
    astVpssChnAttr[1].bFlip                       = AR_FALSE;
    astVpssChnAttr[1].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
    astVpssChnAttr[1].enVideoFormat               = VIDEO_FORMAT_LINEAR;

    for(i=0; i<u32VdecChnNum; i++)
    {
        VpssGrp = i;
        s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &abChnEnable[0], &stVpssGrpAttr, &astVpssChnAttr[0]);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: start VPSS fail for %#x!\n", s32Ret);
            goto END4;
        }
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
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVoConfig.stDispRect.s32X       = 0;
    stVoConfig.stDispRect.s32Y       = 0;
    stVoConfig.stDispRect.u32Width   = stDispSize.u32Width;
    stVoConfig.stDispRect.u32Height  = stDispSize.u32Height;
    stVoConfig.stImageSize.u32Width  = stDispSize.u32Width;
    stVoConfig.stImageSize.u32Height = stDispSize.u32Height;
    stVoConfig.enVoPartMode          = VO_PART_MODE_SINGLE;
    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VO fail for %#x!\n", s32Ret);
        goto END5;
    }

    /************************************************
    step6:  VDEC bind VPSS
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(i, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vdec bind vpss fail for %#x!\n", s32Ret);
            goto END6;
        }
    }

    /************************************************
    step7:  VPSS bind VO
    *************************************************/
    VoLayer = stVoConfig.VoDev;
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_Bind_VO(i, 1, VoLayer, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vpss bind vo fail for %#x!\n", s32Ret);
            goto END7;
        }
    }

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), param->streamFile);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].s32IntervalTime = 1000*33;
        stVdecSend[i].u64PtsIncrease  = 40;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = AR_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }

    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], VdecThread);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

END7:
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vpss unbind vo fail for %#x!\n", s32Ret);
        }
    }

END6:
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: vdec unbind vpss fail for %#x!\n", s32Ret);
        }
    }

END5:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

END4:
    for(i = VpssGrp; i >= 0; i--)
    {
        VpssGrp = i;
        SAMPLE_COMM_VPSS_Stop(VpssGrp, &abChnEnable[0]);
    }

END3:
    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum, &astSampleVdec[0]);

END2:
    SAMPLE_COMM_VDEC_ExitVBPool(&astSampleVdec[0]);

END1:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}
#endif

AR_S32 SAMPLE_STREAM_VDEC_To_YUV(task_param *param)
{
    VB_CONFIG_S stVbConfig;
    AR_S32 i, s32Ret = AR_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[TEST_VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize;
    /*VO_LAYER VoLayer;*/
    AR_U32 u32VdecChnNum;//, VpssGrpNum = 0;
    /*VPSS_GRP VpssGrp;*/
    pthread_t   VdecThread[2*TEST_VDEC_MAX_CHN_NUM];

    PIC_SIZE_E enDispPicSize;
    SAMPLE_VDEC_ATTR astSampleVdec[TEST_VDEC_MAX_CHN_NUM];
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
        printf("streamFile %s is not accessible: %s, exit\n", param->streamFile, strerror(errno));
        return -1;
    }

    /*
    FILE *tmpfp = fopen(param->yuvFile, "w");
    if (!tmpfp) {
        printf("yuvFile %s cannot be opened for writing: %s, exit\n", param->yuvFile, strerror(errno));
    } else
        fclose(tmpfp);
    */

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    if (param->vbSource == VB_SOURCE_MODULE) {
        memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
        stVbConfig.u32MaxPoolCnt             = 1;
        stVbConfig.astCommPool[0].u32BlkCnt  = 30*u32VdecChnNum;
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
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    } else if (param->vbSource == VB_SOURCE_USER ||
               param->vbSource == VB_SOURCE_PRIVATE) {
        s32Ret = AR_MPI_SYS_Init();
        if (AR_SUCCESS != s32Ret)
        {
            printf("sample_vdec.c: init sys fail for %#x!\n", s32Ret);
            goto END1;
        }
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = param->attr.enType;//PT_H265;
        astSampleVdec[i].u32Width                         = param->attr.u32Width;//1920;
        astSampleVdec[i].u32Height                        = param->attr.u32Height;//1080;
        astSampleVdec[i].vbSource                         = param->vbSource;
        astSampleVdec[i].s32ChnId                         = i + param->startChnId;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].u32ScaledWidth                   = param->attr.u32ScaledWidth;
        astSampleVdec[i].u32ScaledHeight                  = param->attr.u32ScaledHeight;
        astSampleVdec[i].bEnableUserData                  = param->showUserData;
        astSampleVdec[i].u32MiniBufMode                   = param->miniBufMode;
        astSampleVdec[i].u32CacheMode                     = param->cacheMode;

        if (param->attr.enType == PT_H265 || param->attr.enType == PT_H264) {
            // For ORDER_DISP, u32DisplayFrameNum should great than delay + 3 to gain performance
            astSampleVdec[i].u32DisplayFrameNum               = 8;
            astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IPB;
            astSampleVdec[i].stSapmleVdecVideo.enOutputOrder  = VIDEO_OUTPUT_ORDER_DISP;
            astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
            astSampleVdec[i].stSapmleVdecVideo.errConcealMode = param->errConcealMode;
            astSampleVdec[i].stSapmleVdecVideo.errConcealUnit = param->errConcealUnit;
            astSampleVdec[i].stSapmleVdecVideo.u32CmdQueueDepth = param->miniBufMode ? 1 : 4;
            astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 7;
            astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
        } else {
            astSampleVdec[i].u32DisplayFrameNum                = 3;
            astSampleVdec[i].u32FrameBufCnt                    = 4;
            astSampleVdec[i].stSapmleVdecPicture.enPixelFormat = param->attr.stSapmleVdecPicture.enPixelFormat;
        }
    }

    if (param->vbSource == VB_SOURCE_USER || param->vbSource == VB_SOURCE_MODULE) {
        s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
        if(s32Ret != AR_SUCCESS)
        {
            printf("sample_vdec.c: init mod common vb fail for %#x!\n", s32Ret);
            goto END2;
        }
    }

    /************************************************
    step3:  start VDEC
    *************************************************/

    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != AR_SUCCESS)
    {
        printf("sample_vdec.c: start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

    /************************************************
    step8:  send stream to VDEC
    ************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), param->streamFile);
        snprintf(stVdecSend[i].cOutName, sizeof(stVdecSend[i].cOutName), "%s_%d", param->yuvFile, i);
        if (param->saveFile == 2)
            stVdecSend[i].s32SaveFile   = AR_SAVE_YUV_ONE_FILE;
        else if (param->saveFile == 3)
            stVdecSend[i].s32SaveFile   = AR_SAVE_YUV_MULTI_FILE;
        else if(param->saveFile == 0)
            stVdecSend[i].s32SaveFile   = AR_SAVE_YUV_NONE;
        else if(i%2 == 0)
            stVdecSend[i].s32SaveFile   = AR_SAVE_YUV_ONE_FILE;
        else
            stVdecSend[i].s32SaveFile   = AR_SAVE_YUV_NONE;
        stVdecSend[i].u32RecoverMode    = param->recoverMode;
        stVdecSend[i].s32ShowUserData   = param->showUserData;
        stVdecSend[i].enType            = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode     = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId          = i + param->startChnId;
        stVdecSend[i].u32Fps            = param->fps;
        stVdecSend[i].s32IntervalTime   = 1000;
        stVdecSend[i].u64PtsInit        = 0;
        stVdecSend[i].u64PtsIncrease    = 40;
        stVdecSend[i].eThreadCtrl       = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend       = param->circleSend;
        stVdecSend[i].s32MilliSec       = 2000;
        stVdecSend[i].s32MinBufSize     = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
        stVdecSend[i].numFrameToDecode  = param->numFrameToDecode;
        stVdecSend[i].discardMode       = param->discardMode;
    }

    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], VdecThread);

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
    {"width",         required_argument, NULL, 'w'},
    {"height",        required_argument, NULL, 'h'},
    {"scaled_width",  required_argument, NULL, 'x'},
    {"scaled_height", required_argument, NULL, 'y'},
    {"source of vb",  required_argument, NULL, 's'},
    {"encoding",      required_argument, NULL, 'e'},
    {"chnid",         required_argument, NULL, 'c'},
    {"debug",         required_argument, NULL, 'd'},
    {"save",          required_argument, NULL, 'l'},
    {"loop",          required_argument, NULL, 'L'},
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
    AR_S32 i = 0;

    task_param param = {0};
    memset(&param, 0, sizeof(task_param));
    param.numFrameToDecode = -1;
    param.cacheMode   = 0;
    param.discardMode = 0;
    param.miniBufMode = 0;
    param.recoverMode = false;
    param.fps    = 0;
    param.chnNum = 1;
    param.errConcealMode = 0;
    param.errConcealUnit = 0;
    param.attr.enType = PT_H265;
    param.attr.u32Width = 1920;
    param.attr.u32Height = 1080;
    param.attr.u32ScaledWidth = 0;
    param.attr.u32ScaledHeight = 0;
    param.vbSource = VB_SOURCE_PRIVATE;
    param.showUserData = 0;
    param.saveFile = 2;
    param.circleSend = true;
    param.attr.stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    av_log_set_level(AV_LOG_QUIET);
    ar_log_shm_set_enabled(0);

    while ((c = getopt_long(argc, argv, ":t:i:o:n:m:M:w:u:U:h:x:y:b:c:C:d:D:r:s:e:f:F:l:L:a", long_options, NULL)) != -1) {
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
        case 'F':
            param.fps = atoi(optarg);
            break;
        case 'f':
            if (!strncmp(optarg, "420p", 4)) {
                printf("yuv format=420p\n");
                param.attr.stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
            } else if (!strncmp(optarg, "422p", 4)) {
                printf("yuv format=422p\n");
                param.attr.stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_422;
            } else if (!strncmp(optarg, "444p", 4)) {
                printf("yuv format=444p\n");
                param.attr.stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_444;
            }
            break;
        case 'D':
            if (1 == atoi(optarg)) {
#if 0
                // coverity error
                srand((unsigned)time(NULL));
                i = rand()%25;
                while (i == 0 || i == 1) {
                    i = rand()%25;
                }
#else
                // CID_10618
                i = ar_get_timestamp() % 25;
#endif
                param.discardMode = i;
                printf("discard Mode=%d, discard all frames which mode %d is 0, except first frame\n",
                        param.discardMode, param.discardMode);
            } else {
                printf("discard Mode=0, not discard any frames\n");
            }
            break;
        case 'M':
            param.errConcealMode = atoi(optarg);
            printf("error conceal Mode=%d\n", param.errConcealMode);
            break;
        case 'U':
            param.errConcealUnit = atoi(optarg);
            printf("error conceal Unit=%d\n", param.errConcealUnit);
            break;
        case 'm':
            param.chnNum = atoi(optarg);
            printf("chnNum=%d\n", param.chnNum);
            break;
        case 'l':
            param.saveFile = atoi(optarg);
            printf("save file type=%d\n", param.saveFile);
            break;
        case 'u':
            param.showUserData = atoi(optarg);
            printf("show userdata=%d\n", param.showUserData);
            break;
        case 'L':
            param.circleSend = atoi(optarg);
            printf("loop play=%d\n", param.circleSend);
            break;
        case 'w':
            param.attr.u32Width = atoi(optarg);
            printf("width=%d\n", param.attr.u32Width);
            break;
        case 'h':
            param.attr.u32Height = atoi(optarg);
            printf("height=%d\n", param.attr.u32Height);
            break;
        case 'x':
            param.attr.u32ScaledWidth = atoi(optarg);
            printf("scaled width=%d\n", param.attr.u32ScaledWidth);
            break;
        case 'y':
            param.attr.u32ScaledHeight = atoi(optarg);
            printf("scaled height=%d\n", param.attr.u32ScaledHeight);
            break;
        case 'e':
            if (!strncmp(optarg, "h264", 4)) {
                param.attr.enType = PT_H264;
                printf("codec type: h264\n");
            } else if (!strncmp(optarg, "h265", 4)) {
                param.attr.enType = PT_H265;
                printf("codec type: h265\n");
            } else if (!strncmp(optarg, "jpeg", 4)) {
                param.attr.enType = PT_JPEG;
                printf("codec type: jpeg\n");
            } else if (!strncmp(optarg, "mjpeg", 5)) {
                param.attr.enType = PT_MJPEG;
                printf("codec type: mjpeg\n");
            } else {
                printf("unknwon codec type:%s", optarg);
                return AR_FAILURE;
            }
            break;
        case 'b':
            param.miniBufMode = atoi(optarg);
            printf("miniBufMode=%d\n", param.miniBufMode);
            break;
        case 'c':
            param.startChnId = atoi(optarg);
            printf("startChnId=%d\n", param.startChnId);
            break;
        case 'C':
            param.cacheMode = atoi(optarg);
            printf("cacheMode=%d\n", param.cacheMode);
            break;
        case 'd':
            if (atoi(optarg))
                av_log_set_level(AV_LOG_INFO);
            else
                av_log_set_level(AV_LOG_QUIET);
            break;
        case 'r':
            param.recoverMode = atoi(optarg) ? true : false;
            printf("recoverMode=%d\n", param.recoverMode);
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

    if (param.attr.enType == PT_H264 || param.attr.enType == PT_H265)
    {
        printf("h26x only support YUV420p format\n");
        param.attr.stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    }

    if (argc < 2)
    {
        SAMPLE_VDEC_Usage(argv[0]);
        return AR_FAILURE;
    }

    s32Ret = ar_log_init();
    if(s32Ret)
    {
        printf("log int failed\n");
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
            printf("sample_vdec.c: the index is invaild!\n");
            SAMPLE_VDEC_Usage(argv[0]);
            s32Ret = AR_FAILURE;
            break;
        }
    }

    if (AR_SUCCESS == s32Ret)
    {
        printf("sample_vdec.c: program exit normally!\n");
    }
    else
    {
        printf("sample_vdec.c: program exit abnormally!\n");
    }

    return s32Ret;
}
