
#include "sample_comm_muxer.h"
#include "sample_comm_audio.h"
#include "sample_comm.h"

#include <signal.h>

#ifdef SAMPLE_PRT
#undef SAMPLE_PRT
#define SAMPLE_PRT printf
#endif

static AR_BOOL gRuningFlag = FALSE;

void *pvPortMalloc(size_t xWantedSize )
{
	return malloc(xWantedSize);
}
void vPortFree( void *pv)
{
	free(pv);
}

static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, const AR_CHAR *ps8Argv[])
{
    AR_S32 i=0;
	for(i=0;i<s32Argc;i++)
	{
         if(!strcmp(ps8Name,ps8Argv[i]))
         {
            printf("Find Pra %s @ %d \n",ps8Name,i);
            return i;
         }
	}
	return 0;
}

AR_S32 SAMPLE_STREAM_VIN_OpenDev(AR_S32 mode)
{
   VI_DEV_PROP_S Prop={0};
   if(mode==0){
      Prop.cam_mode=VIN_CAMERA_NORMAL;
   }else if(mode==1)
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }else if(mode==2)
   {
      Prop.cam_mode=VIN_CMAERA_MULTI_MODE;
   }else
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }
   //cfg the fre
   Prop.hdr_fre_mod=1;
   Prop.hdr_fre_hz=200000000;
   Prop.vif_fre_mod=1;
   Prop.vif_fre_hz=400000000;
   Prop.isp_fre_mod=1;
   Prop.isp_fre_hz=200000000;

   AR_MPI_VIN_OpenDev(&Prop);
   return AR_SUCCESS;
}

AR_S32 SAMPLE_STREAM_VIN_CloseDev()
{
	return AR_SUCCESS;
}

static AR_VOID SAMPLE_STREAM_GetViStream(AR_S32 VencChn, AR_BOOL* RuningFlag)
{
    AR_S32 s32Ret;
    struct timeval stTimeout;
    fd_set readFdSets;

    int vencFd = AR_MPI_VENC_GetFd(VencChn);

    while (*RuningFlag)
    {
        FD_ZERO(&readFdSets);
        FD_SET(vencFd, &readFdSets);
        
        stTimeout.tv_sec = 0;
        stTimeout.tv_usec = 100*1000;

        s32Ret = select(vencFd+1, &readFdSets, NULL, NULL, &stTimeout);
        if (s32Ret < 0)
        {
            printf("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            printf("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            if(!FD_ISSET(vencFd, &readFdSets)){
                continue;
            }
            VENC_CHN_STATUS_S stChnStatus;
            VENC_STREAM_S stVencStream;
            s32Ret = AR_MPI_VENC_QueryStatus(VencChn, &stChnStatus);
            if(0 == stChnStatus.u32CurPacks){
                printf("NOTE: Current  frame is NULL!\n");
                continue;
            }
            /*******************************************************
             step 2.3 : malloc corresponding number of pack nodes.
            *******************************************************/
            stVencStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stChnStatus.u32CurPacks);
            if (NULL == stVencStream.pstPack)
            {
                printf("malloc stream pack failed!\n");
                break;
            }

            /*******************************************************
             step 2.4 : call mpi to get one-frame stream
            *******************************************************/
            stVencStream.u32PackCount = stChnStatus.u32CurPacks;
            s32Ret = AR_MPI_VENC_GetStream(VencChn, &stVencStream, 0);
            if (AR_SUCCESS != s32Ret)
            {
                free(stVencStream.pstPack);
                stVencStream.pstPack = NULL;
                printf("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                continue;
            }
            printf("Get Venc Stream ok\n");
            s32Ret = AR_MPI_VENC_ReleaseStream(VencChn, &stVencStream);
            free(stVencStream.pstPack);
        }
    }
    // Close Fd
    return NULL;
}

#define HAVE_AUDIO 1
#define HAVE_VIDEO 1

static AR_S32 SAMPLE_STREAM_PackMp4(AR_S32 s32Argc, const AR_CHAR *ps8Argv[])
{
    AR_S32 s32Ret;
    AR_S32 s32Index;

    AR_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    AR_S32             s32WorkSnsId   = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VB_CONFIG_S        stVbConf;

    AR_S32             s32FrameRate = 0;
    SIZE_S             stSize;
    PIC_SIZE_E         enPicSize;
    PIC_SIZE_E         enEncPicSize;
    SIZE_S             stVideoSize;
    AR_U32             u32BlkSize;


    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;

    VPSS_GRP           VpssGrp        = 0;
    VPSS_GRP_ATTR_S    stVpssGrpAttr = {0};
    VPSS_CHN           VpssChn0        = VPSS_CHN0;
    VPSS_CHN           VpssChn1        = VPSS_CHN1;
    AR_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

    VENC_CHN           VencChn[1]  = {0};
    AENC_CHN           AencChn[1] = {0};
    PAYLOAD_TYPE_E     enVideoType   = PT_H264;
    PAYLOAD_TYPE_E     enAudioType   = PT_AAC;
    SAMPLE_RC_E        enRcMode    = SAMPLE_RC_CBR;
    AR_U32             u32Profile  = 0;
    VENC_GOP_ATTR_S    stGopAttr;

    SPREAD_ATTR_S      stSpreadAttr = {0};

    AUDIO_DEV audioDevId = 0;
    AR_S32 s32AudioChannel = 2;
    AIO_ATTR_S stAioAttr;

    AUDIO_SAMPLE_RATE_E enSampleRate;
    AUDIO_BIT_WIDTH_E enBitWidth = AUDIO_BIT_WIDTH_16;
    AUDIO_SOUND_MODE_E enSoundMode;


    MUXER_CONTEXT_T stMuxerContext;
    memset(&stMuxerContext, 0, sizeof(stMuxerContext));

    s32Index = SAMPLE_Find_Pra_Index_By_Name("-videoformat", s32Argc, ps8Argv);
    if(s32Index > 0){
        if(strcmp(ps8Argv[s32Index+1],"h264") == 0){
            enVideoType = PT_H264;
        }else if(strcmp(ps8Argv[s32Index+1], "h265") == 0){
            enVideoType = PT_H265;
        }else{
            printf("Invalid -videoformat option: %s\n",ps8Argv[s32Index+1]);
            return AR_FAILURE;
        }
    }
    s32Index = SAMPLE_Find_Pra_Index_By_Name("-videowidth", s32Argc, ps8Argv);
    if(s32Index > 0){
        stVideoSize.u32Width = atoi(ps8Argv[s32Index+1]);
    }else{
        stVideoSize.u32Width = 1280;
    }

    s32Index = SAMPLE_Find_Pra_Index_By_Name("-videoheight", s32Argc, ps8Argv);
    if(s32Index > 0){
        stVideoSize.u32Height = atoi(ps8Argv[s32Index+1]);
    }else{
        stVideoSize.u32Height = 720;
    }

    if(stVideoSize.u32Width == 352 && stVideoSize.u32Height == 288){
        enEncPicSize = PIC_CIF;
    }else if(stVideoSize.u32Width == 720 && stVideoSize.u32Height == 576){
        enEncPicSize = PIC_D1_PAL;
    }else if(stVideoSize.u32Width == 720 && stVideoSize.u32Height == 480){
        enEncPicSize = PIC_D1_NTSC;
    }else if(stVideoSize.u32Width == 1280 && stVideoSize.u32Height == 720){
        enEncPicSize = PIC_720P;
    }else if(stVideoSize.u32Width == 1920 && stVideoSize.u32Height == 1080){
        enEncPicSize = PIC_1080P;
    }else if(stVideoSize.u32Width == 2592 && stVideoSize.u32Height == 1520){
        enEncPicSize = PIC_2592x1520;
    }else if(stVideoSize.u32Width == 2688 && stVideoSize.u32Height == 1520){
        enEncPicSize = PIC_2688x1520;
    }else if(stVideoSize.u32Width == 3840 && stVideoSize.u32Height == 2160){
        enEncPicSize = PIC_3840x2160;
    }else if(stVideoSize.u32Width == 4096 && stVideoSize.u32Height == 2160){
        enEncPicSize = PIC_4096x2160;
    }else{
        printf("Invalid -videoformat option: %s\n",ps8Argv[s32Index+1]);
        return AR_FAILURE;
    }

    s32Index = SAMPLE_Find_Pra_Index_By_Name("-framerate", s32Argc, ps8Argv);
    if(s32Index > 0){
        s32FrameRate = atoi(ps8Argv[s32Index+1]);
    }else{
        s32FrameRate = 30;
    }

    s32Index = SAMPLE_Find_Pra_Index_By_Name("-audioformat", s32Argc, ps8Argv);
    if(s32Index > 0){
        if(strcmp(ps8Argv[s32Index+1],"aac") == 0){
            enAudioType = PT_AAC;
        }else{
            printf("Invalid -audioformat option: %s\n",ps8Argv[s32Index+1]);
            return AR_FAILURE;   
        }
    }else{
        enAudioType = PT_AAC;
    }

    s32Index = SAMPLE_Find_Pra_Index_By_Name("-audiochannel",s32Argc, ps8Argv);
    if(s32Index > 0){
        s32AudioChannel = atoi(ps8Argv[s32Index+1]);
    }else{
        s32AudioChannel = 2;
    }
    if(s32AudioChannel < 1 || s32AudioChannel > 2){
        printf("Invalid -audiochannel option: %d\n",s32AudioChannel);
        return AR_FAILURE;
    }
    if(s32AudioChannel == 1){
        enSoundMode = AUDIO_SOUND_MODE_MONO;
    }else if(s32AudioChannel == 2){
        enSoundMode = AUDIO_SOUND_MODE_STEREO;
    }
    s32Index = SAMPLE_Find_Pra_Index_By_Name("-samplerate",s32Argc, ps8Argv);
    if(s32Index > 0){
        int samplerate = atoi(ps8Argv[s32Index+1]);
        if(samplerate == 44100){
            enSampleRate = AUDIO_SAMPLE_RATE_44100;
        }else if(samplerate == 8000){
            enSampleRate = AUDIO_SAMPLE_RATE_8000;
        }else if(samplerate == 12000){
            enSampleRate = AUDIO_SAMPLE_RATE_12000;
        }else if(samplerate == 16000){
            enSampleRate = AUDIO_SAMPLE_RATE_16000;
        }else if(samplerate == 24000){
            enSampleRate = AUDIO_SAMPLE_RATE_24000;
        }else if(samplerate == 32000){
            enSampleRate = AUDIO_SAMPLE_RATE_32000;
        }else if(samplerate == 48000){
            enSampleRate = AUDIO_SAMPLE_RATE_48000;
        }else if(samplerate == 64000){
            enSampleRate = AUDIO_SAMPLE_RATE_64000;
        }else if(samplerate == 96000){
            enSampleRate = AUDIO_SAMPLE_RATE_96000;
        }else{
            printf("Invalid -samplerate option: %d\n",samplerate);
            return AR_FAILURE;
        }
    }else{
        enSampleRate = AUDIO_SAMPLE_RATE_48000;
    }

    AR_CHAR szOutputFile[256]={0};
    s32Index = SAMPLE_Find_Pra_Index_By_Name("-outputfile", s32Argc, ps8Argv);
    if(s32Index > 0){
        sprintf(szOutputFile,"%s",ps8Argv[s32Index+1]);
    }else{
        sprintf(szOutputFile, "output.mp4");
    }

    /*config vb*/
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

#if HAVE_VIDEO
    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum                                   = s32ViCnt;
    stViConfig.as32WorkingViId[0]                                = 0;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    // s32Ret |= SAMPLE_COMM_SYS_GetPicSize(enEncPicSize, &stEncSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }


    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 8;
#endif


#if HAVE_AUDIO
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = 2 * 2048 * 1080;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 4;
    stVbConf.u32MaxPoolCnt++;

	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = 2 * 1280 * 720;
	stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
    stVbConf.u32MaxPoolCnt++;

    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize  = 4096;
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt   = 100;
    stVbConf.u32MaxPoolCnt++;
#endif

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

#if HAVE_VIDEO
    s32Ret = SAMPLE_STREAM_VIN_OpenDev(0);
    if(AR_SUCCESS != s32Ret){
        SAMPLE_PRT("open vin device failed with %d\n",s32Ret);
        goto EXIT_SYS_Exit;
    }

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT_VI_CloseDev;
    }

    /*config vpss*/
    memset(&stVpssGrpAttr, 0, sizeof(stVpssGrpAttr));
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = s32FrameRate;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate    = s32FrameRate;
    stVpssGrpAttr.enDynamicRange                 = enDynamicRange;
    stVpssGrpAttr.enPixelFormat                  = enPixFormat;
    stVpssGrpAttr.enCompressMode                 = enCompressMode;
    stVpssGrpAttr.u32Depth                       = 2;
    stVpssGrpAttr.enVideoFormat                  = enVideoFormat;

    stVpssGrpAttr.u32Width                        = stVideoSize.u32Width;
    stVpssGrpAttr.u32Height                        = stVideoSize.u32Height;
    stVpssGrpAttr.u32ScaleMode                   = 1;
    stVpssGrpAttr.enChnMode                      = VPSS_CHN_MODE_USER;
    stVpssGrpAttr.stAspectRatio.enMode           = ASPECT_RATIO_NONE;

    printf("width %d,height %d\n",stVpssGrpAttr.u32Width,stVpssGrpAttr.u32Height);

    memset(&astVpssChnAttr, 0, sizeof(astVpssChnAttr));

    astVpssChnAttr[VpssChn0].u32Width                    = stVideoSize.u32Width;
    astVpssChnAttr[VpssChn0].u32Height                   = stVideoSize.u32Height;
    astVpssChnAttr[VpssChn0].enChnMode                   = VPSS_CHN_MODE_USER;
    astVpssChnAttr[VpssChn0].enCompressMode              = enCompressMode;
    astVpssChnAttr[VpssChn0].enDynamicRange              = enDynamicRange;
    astVpssChnAttr[VpssChn0].enVideoFormat               = enVideoFormat;
    astVpssChnAttr[VpssChn0].enPixelFormat               = enPixFormat;
    astVpssChnAttr[VpssChn0].stFrameRate.s32SrcFrameRate = 25;
    astVpssChnAttr[VpssChn0].stFrameRate.s32DstFrameRate = 25;
    astVpssChnAttr[VpssChn0].u32Depth                    = 2;
    astVpssChnAttr[VpssChn0].bMirror                     = AR_FALSE;
    astVpssChnAttr[VpssChn0].bFlip                       = AR_FALSE;
    astVpssChnAttr[VpssChn0].stAspectRatio.enMode        = ASPECT_RATIO_NONE;


    /*start vpss*/
    abChnEnable[0] = AR_TRUE;
    //abChnEnable[1] = AR_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VI_StopVi;
    }

    /*vi bind vpss*/
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VPSS_Stop;
    }

    /*config venc */
    memset(&stGopAttr, 0, sizeof(stGopAttr));
    stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
    stGopAttr.stNormalP.s32IPQpDelta  = -2;
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enVideoType, enEncPicSize, enRcMode, u32Profile, &stGopAttr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start venc failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VI_UnbindVpss;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn0, VencChn[0]);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT_VENC_Stop;
    }
#endif

#if HAVE_AUDIO
    memset(&stAioAttr,0, sizeof(stAioAttr));
	stAioAttr.enSamplerate = enSampleRate;			
	stAioAttr.enBitwidth = enBitWidth;
	stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	stAioAttr.enSoundmode = enSoundMode;
	stAioAttr.u32EXFlag = 0;
	stAioAttr.u32FrmNum = 8;
	stAioAttr.u32PtNumPerFrm = 1024;
    if(enSoundMode == AUDIO_SOUND_MODE_MONO){
        stAioAttr.u32ChnCnt = 1;
    }else if(enSoundMode == AUDIO_SOUND_MODE_STEREO){
        stAioAttr.u32ChnCnt = 2;
    }
	stAioAttr.u32ClkSel = 0;
	stAioAttr.enI2sType = AIO_I2STYPE_INNERCODEC;

    s32Ret = SAMPLE_COMM_AUDIO_StartAi(audioDevId, s32AudioChannel, &stAioAttr);
    if(s32Ret != AR_SUCCESS){
        SAMPLE_PRT("Start ai failed. s32Ret: 0x%x !n", s32Ret);
        #if HAVE_VIDEO
        goto EXIT_VPSS_UnbindEnc;
        #else
        goto EXIT_SYS_Exit;
        #endif
    }

    s32Ret = SAMPLE_COMM_AUDIO_StartAenc(AencChn[0], &stAioAttr, enAudioType);
    if(s32Ret != AR_SUCCESS){
        SAMPLE_PRT("Start aenc failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT_AUDIO_StopVi;
    }

    //TODO:
    s32Ret = SAMPLE_COMM_AUDIO_BindAiAenc(audioDevId, 0, 0, AencChn[0]);
    if(s32Ret){
        SAMPLE_PRT("Bind ai aenc failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT_AUDIO_StopAenc;
    }
#endif

    s32Ret = SAMPLE_COMM_MUXER_InitMuxer(&stMuxerContext, szOutputFile);
    if(s32Ret != AR_SUCCESS){
        SAMPLE_PRT("Muxer init failed. s32Ret: 0x%x !n", s32Ret);
        #if HAVE_AUDIO
        goto EXIT_AUDIO_UnbindAiAenc;
        #elif HAVE_VIDEO1
        goto EXIT_VPSS_UnbindEnc;
        #endif
    }

#if HAVE_VIDEO
    s32Ret = SAMPLE_COMM_MUXER_AddVideoStream(&stMuxerContext, VencChn[0], enVideoType, stVideoSize.u32Width, stVideoSize.u32Height, 2*1024*1024, NULL, 0);
    if(s32Ret != AR_SUCCESS){
        SAMPLE_PRT("Add video stream failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT_MUXER_StopPack;
    }
#endif

#if HAVE_AUDIO
    s32Ret = SAMPLE_COMM_MUXER_AddAudioStream(&stMuxerContext, AencChn[0], enAudioType, enSampleRate, enSoundMode, enBitWidth, NULL, 0);
    if(s32Ret != AR_SUCCESS){
        SAMPLE_PRT("Add audio stream failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT_MUXER_StopPack;
    }
#endif

    s32Ret = SAMPLE_COMM_MUXER_StartPack(&stMuxerContext);
    if(s32Ret != AR_SUCCESS){
        SAMPLE_PRT("start packet s32Ret: 0x%x !n", s32Ret);
        goto EXIT_MUXER_StopPack;
    }
    
    gRuningFlag = AR_TRUE;

    while(gRuningFlag){
        // VIDEO_FRAME_INFO_S videoFrame;
        // s32Ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &videoFrame, 10000);
        // printf("Get VI Frame Ret %d\n",s32Ret);
        // AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &videoFrame);

        // s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn0, &videoFrame, 10000);
        // printf("Get VPSS Frame Ret %d\n",s32Ret);
        // AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn0, &videoFrame);

        //SAMPLE_STREAM_GetViStream(VencChn[0],&gRuningFlag);
        sleep(1);
    }

EXIT_MUXER_StopPack:
    s32Ret = SAMPLE_COMM_MUXER_StopPack(&stMuxerContext);
    if(s32Ret != AR_SUCCESS){
    }

#if HAVE_AUDIO
EXIT_AUDIO_UnbindAiAenc:
    s32Ret = SAMPLE_COMM_AUDIO_UnbindAiAenc(audioDevId, 0, 0, AencChn[0]);

EXIT_AUDIO_StopAenc:
    s32Ret = SAMPLE_COMM_AUDIO_StopAenc(AencChn[0]);

EXIT_AUDIO_StopVi:
    s32Ret = SAMPLE_COMM_AUDIO_StopAi(audioDevId, s32AudioChannel);
#endif

#if HAVE_VIDEO
EXIT_VPSS_UnbindEnc:
    s32Ret = SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn0, VencChn[0]);

EXIT_VENC_Stop:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);

EXIT_VI_UnbindVpss:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

EXIT_VPSS_Stop:
    s32Ret = SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

EXIT_VI_StopVi:
    s32Ret = SAMPLE_COMM_VI_StopVi(&stViConfig);

EXIT_VI_CloseDev:
    s32Ret = SAMPLE_STREAM_VIN_CloseDev();
#endif

EXIT_SYS_Exit:
    SAMPLE_COMM_SYS_Exit();

    return AR_SUCCESS;
}



void SAMPLE_STREAM_Usage(char *szName)
{
    printf("Example: %s -videoformat h264 -videowidth 1280 -videoheight 720 -framerate 30 -audioformat aac -audiochannel 2 -samplerate 44100 -outputfile output.mp4\n", szName);
    printf("-function: \n");
    printf("-videoformat: video encode format, h264,h265, default h264\n");
    printf("-videowidth: video encode width, default 1280\n");
    printf("-videoheight: video encode height, default 720");
    printf("-framerate: video framerate, default 30\n");
    printf("-audioformat: audio encode format, aac default aac\n");
    printf("-audiochannel: audio channel number, 1 or 2, default 2\n");
    printf("-samplerate: audio sampling rate, default 44100\n");
    printf("-outputfile: output file, default output.mp4\n");
    printf("-help: print usage\n");
    exit(0);
}

void SAMPLE_STREAM_HandleSig(AR_S32 signo)
{
    if (SIGINT == signo) {
        gRuningFlag = AR_FALSE;
        SAMPLE_PRT("Receive INTERRUPT signal, Exit!\n");
    }
}

/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_FAILURE;
    AR_S32 s32Index;

    signal(SIGINT, &SAMPLE_STREAM_HandleSig);

    s32Index = SAMPLE_Find_Pra_Index_By_Name("-help",argc,argv);
    if(argc < 2 || s32Index > 0){
        SAMPLE_STREAM_Usage(argv[0]);
    }

    s32Ret = SAMPLE_STREAM_PackMp4(argc, argv);

    if (s32Ret == AR_SUCCESS) {
        printf("%s exit success!\n", argv[0]);
    } else {
        printf("%s exit abnormally!\n", argv[0]);
    }

    return s32Ret;
}
