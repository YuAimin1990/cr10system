#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mpi_sys.h"
#include "audio_sample.h"

#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4  /* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_40K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

static AAC_TYPE_E     gs_enAacType = AAC_TYPE_AACLC;
static AAC_TRANS_TYPE_E gs_enAacTransType = AAC_TRANS_TYPE_ADTS;

typedef struct tagSAMPLE_ADEC_S
{
    AR_BOOL bStart;
    AR_S32 AdChn;
    FILE* prfd;
    FILE* pwfds[2];
    pthread_t stAdPid_in;
    pthread_t stAdPid_out;
} SAMPLE_ADEC_S;

typedef struct tagSAMPLE_AENC_S
{
    AR_BOOL bStart;
    pthread_t stAencPid_in;
    pthread_t stAencPid_out;
    AR_S32  AeChn;
    AR_S32  AdChn;
    FILE*    pfd[2];
    FILE*    out_pfd;
    AR_BOOL bSendAdChn;
} SAMPLE_AENC_S;

static SAMPLE_ADEC_S gs_stSampleAdec[ADEC_MAX_CHN_NUM];
static SAMPLE_AENC_S gs_stSampleAenc[AENC_MAX_CHN_NUM];


/******************************************************************************
* function : Start Adec
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_StartAdec(AR_S32 AdChn, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
  AR_S32 s32Ret;
  ADEC_CHN_ATTR_S stAdecAttr;

  ADEC_ATTR_AAC_S stAdecAac;
  AENC_ATTR_G711_S stAencG711;
  stAdecAttr.enType = enType;
  stAdecAttr.u32BufSize = 10;
  stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

  if (PT_AAC == stAdecAttr.enType)
  {
      //register aac decode
      s32Ret = ADEC_AacInit();
      if(AR_SUCCESS != s32Ret)
      {
          printf("aac init failed!\n");
          return s32Ret;
      }
      stAdecAttr.pValue = &stAdecAac;
      stAdecAttr.enMode = ADEC_MODE_STREAM;   /* aac should be stream mode */
      stAdecAac.enTransType = AAC_TRANS_TYPE_ADTS;
  }
  else if (PT_G711A == stAdecAttr.enType || PT_G711U == stAdecAttr.enType)
  {
      stAencG711.enBitWidth = pstAioAttr->enBitwidth;
      stAencG711.enSoundMode = pstAioAttr->enSoundmode;
      stAdecAttr.pValue       = &stAencG711;
  }

  /* create adec chn*/
  s32Ret = AR_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
  if (AR_SUCCESS != s32Ret)
  {
      printf("%s: AR_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, \
        AdChn, s32Ret);
      return s32Ret;
  }
  return 0;
}


/******************************************************************************
* function : Stop Adec
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_StopAdec(AR_S32 AdChn)
{
    AR_S32 s32Ret;

    s32Ret = AR_MPI_ADEC_DestroyChn(AdChn);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_ADEC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
               AdChn, s32Ret);
        return s32Ret;
    }

    return AR_SUCCESS;
}

/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AdecProc(void* parg)
{
    AR_S32 s32Ret;
    AUDIO_STREAM_S stAudioStream;
    AR_U32 u32Len = 640;
    AR_U32 u32ReadLen;
    AR_S32 s32AdecChn;
    AR_U8* pu8AudioStream = NULL;
    SAMPLE_ADEC_S* pstAdecCtl = (SAMPLE_ADEC_S*)parg;
    FILE* pfd = pstAdecCtl->prfd;
    s32AdecChn = pstAdecCtl->AdChn;

    pu8AudioStream = (AR_U8*)malloc(sizeof(AR_U8) * MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        return NULL;
    }

    while (AR_TRUE == pstAdecCtl->bStart)
    {
        /* read from file */
        stAudioStream.pStream = pu8AudioStream;
        u32ReadLen = fread(stAudioStream.pStream, 1, u32Len, pfd);
        if (u32ReadLen <= 0)
        {
            //s32Ret = ar_hal_adec_send_end_of_stream(s32AdecChn, AR_FALSE);
            s32Ret = AR_MPI_ADEC_SendEndOfStream(s32AdecChn, AR_FALSE);
            if (AR_SUCCESS != s32Ret)
            {
                printf("%s: AR_MPI_ADEC_SendEndOfStream failed!\n", __FUNCTION__);
            }
            //(AR_VOID)fseek(pfd, 0, SEEK_SET);/*read file again*/
            continue;
 //           break;
        }

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        stAudioStream.u32Len = u32ReadLen;
        s32Ret = AR_MPI_ADEC_SendStream(s32AdecChn, &stAudioStream, AR_TRUE);
        if (AR_SUCCESS != s32Ret)
        {
            printf("%s: AR_MPI_ADEC_SendStream(%d) failed with %#x!\n", \
                   __FUNCTION__, s32AdecChn, s32Ret);
            break;
        }
    }

    printf("%s: free and close!\n", __FUNCTION__);

    free(pu8AudioStream);
    pu8AudioStream = NULL;
    fclose(pfd);
//    pstAdecCtl->bStart = AR_FALSE;
    return NULL;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_CreatGetTrdFileAdec(AR_S32 AdChn, FILE* pAdcFd)
{
    SAMPLE_ADEC_S* pstAdec = NULL;

    if (NULL == pAdcFd)
    {
        return AR_FAILURE;
    }

    pstAdec = &gs_stSampleAdec[AdChn];
    //pstAdec = &SampleAdecR;
    pstAdec->AdChn = AdChn;
    pstAdec->prfd = pAdcFd;
    pstAdec->bStart = AR_TRUE;
    pthread_create(&pstAdec->stAdPid_in, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

    return AR_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from file and send to adec
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(AR_S32 AdChn)
{
    SAMPLE_ADEC_S* pstAdec = NULL;

    pstAdec = &gs_stSampleAdec[AdChn];
    //pstAdec = &SampleAdecR;
    if (pstAdec->bStart)
    {
        pstAdec->bStart = AR_FALSE;
        //pthread_cancel(pstAdec->stAdPid);
        pthread_join(pstAdec->stAdPid_in, 0);
    }

    return AR_SUCCESS;
}

/******************************************************************************
* function : get frame from adec and save to filec
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AdecToFileProc(void* parg)
{
    AR_S32 s32Ret;

//    AR_U32 u32Len = 640;
//    AR_U32 u32ReadLen;
    AUDIO_FRAME_INFO_S audio_frame;

    SAMPLE_ADEC_S* pstAdecCtl = (SAMPLE_ADEC_S*)parg;

    FILE* pwfd0 = (pstAdecCtl->pwfds)[0];
    FILE* pwfd1 = (pstAdecCtl->pwfds)[1];
    AR_S32 s32AdecChn = pstAdecCtl->AdChn;

    while(AR_TRUE == pstAdecCtl->bStart)
    {
        /* get frame from adec chn */
        s32Ret = AR_MPI_ADEC_GetFrame(s32AdecChn, &audio_frame, AR_TRUE);
        if (AR_SUCCESS != s32Ret )
        {
            printf("%s: ar_hal_adec_get_frame(%d), failed with %#x!\n", \
                   __FUNCTION__, s32AdecChn, s32Ret);
            continue;
        }

        /* save audio frame to file */
        if (audio_frame.pstFrame->enSoundmode != AUDIO_SOUND_MODE_MONO &&
            audio_frame.pstFrame->enSoundmode != AUDIO_SOUND_MODE_STEREO) {
            return NULL;
        }
        //save as LLL...RRR..., VO as LRLRLR)
        printf("!!!! %s: adec chn(%d) fwrite a frame start\n", __FUNCTION__, s32AdecChn);
//    (AR_VOID)fwrite(audio_frame.pstFrame.pu8VirAddr[0], 1, audio_frame.pstFrame.u32Len * (audio_frame.pstFrame.enSoundmode+1), pwfd0);
        (AR_VOID)fwrite(audio_frame.pstFrame->u64VirAddr[0], 1, audio_frame.pstFrame->u32Len, pwfd0);
        (AR_VOID)fwrite(audio_frame.pstFrame->u64VirAddr[1], 1, audio_frame.pstFrame->u32Len, pwfd1);

        /* finally you must release the frame */
        AR_MPI_ADEC_ReleaseFrame(s32AdecChn,  &audio_frame);
        if (AR_SUCCESS != s32Ret)
        {
            printf("%s: ar_hal_adec_release_frame(%d), failed with %#x!\n", \
               __FUNCTION__, s32AdecChn, s32Ret);
            //pstAdecCtl->bStart = AR_FALSE;
            return NULL;
        }

        fflush(pwfd0);
        fflush(pwfd1);
    }
    fclose(pwfd0);
    fclose(pwfd1);
//    pstAdecCtl->bStart = AR_FALSE;
    return NULL;
}

/******************************************************************************
* function : Create the thread to get frame from adec and save to file
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_CreatSaveAdecFrameToFile(AR_S32 AdChn, FILE* pAdcFd0, FILE* pAdcFd1)
{
    SAMPLE_ADEC_S* pstAdec = NULL;

    if (NULL == pAdcFd0 || NULL == pAdcFd1)
    {
        return AR_FAILURE;
    }

    pstAdec = &gs_stSampleAdec[AdChn];
    //pstAdec = &SampleAdecW;
    pstAdec->AdChn = AdChn;
    (pstAdec->pwfds)[0] = pAdcFd0;
    (pstAdec->pwfds)[1] = pAdcFd1;
    pstAdec->bStart = AR_TRUE;
    pthread_create(&pstAdec->stAdPid_out, 0, SAMPLE_COMM_AUDIO_AdecToFileProc, pstAdec);

    return AR_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from file and send to adec
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_DestroySaveAdecFrameToFile(AR_S32 AdChn)
{
    SAMPLE_ADEC_S* pstAdec = NULL;

    pstAdec = &gs_stSampleAdec[AdChn];
    //pstAdec = &SampleAdecW;
    if (pstAdec->bStart)
    {
        pstAdec->bStart = AR_FALSE;
        //pthread_cancel(pstAdec->stAdPid);
        pthread_join(pstAdec->stAdPid_out, 0);
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_SetStopAdecStatus(AR_S32 AdChn)
{
    gs_stSampleAdec[AdChn].bStart = AR_FALSE;
    return 0;
}

/******************************* Encoder ****************************************/


/******************************************************************************
* function : Start Aenc
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_StartAenc(AR_S32 s32AencChn, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
    AR_S32 s32Ret;
    AENC_CHN_ATTR_S stAencAttr;
    AENC_ATTR_ADPCM_S stAdpcmAenc;
    AENC_ATTR_G711_S stAencG711;
    AENC_ATTR_G726_S stAencG726;
    AENC_ATTR_LPCM_S stAencLpcm;
    AENC_ATTR_AAC_S  stAencAac;     // AAC协议参数

    /* set AENC chn attr */

    stAencAttr.enType = enType;          //音频编码协议类型
    stAencAttr.u32BufSize = 30;          //音频编码协议对应的帧长
    stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm;  //音频编码缓存大小，以帧为单位

    if (PT_ADPCMA == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAdpcmAenc;
        stAdpcmAenc.enADPCMType = AUDIO_ADPCM_TYPE;
    }
    else if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
    {
    stAencG711.enBitWidth = pstAioAttr->enBitwidth;
    stAencG711.enSoundMode = pstAioAttr->enSoundmode;
        stAencAttr.pValue       = &stAencG711;
    }
    else if (PT_G726 == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG726;
        stAencG726.enG726bps    = G726_BPS;
    }
    else if (PT_LPCM == stAencAttr.enType)
    {
        stAencAttr.pValue = &stAencLpcm;
    }
    else if (PT_AAC == stAencAttr.enType)
    {
    //register aac encode
    s32Ret = AENC_AacInit();
    if(AR_SUCCESS != s32Ret)
    {
      printf("aenc aac init failed!\n");
      return s32Ret;
    }
    printf("aenc aac init ok!\n");
        stAencAttr.pValue = &stAencAac;    //具体协议属性指针
        stAencAac.enAACType = gs_enAacType;
        stAencAac.enBitRate = AAC_BPS_48K;//gs_enAacBps;//pstAioAttr->enSamplerate * 2 ;//gs_enAacBps;
        stAencAac.enBitWidth = pstAioAttr->enBitwidth;
        stAencAac.enSmpRate = pstAioAttr->enSamplerate;
        stAencAac.enSoundMode = pstAioAttr->enSoundmode;
        stAencAac.enTransType = gs_enAacTransType;
        stAencAac.s16BandWidth = 0;
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
        return AR_FAILURE;
    }

    /* create aenc chn*/
    s32Ret = AR_MPI_AENC_CreateChn(s32AencChn, &stAencAttr);
    printf("%s: AR_MPI_AENC_CreateChn(%d) with %#x!\n", __FUNCTION__, s32AencChn, s32Ret);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
               s32AencChn, s32Ret);
        return s32Ret;
    }

    return AR_SUCCESS;
}

/******************************************************************************
* function : Stop Aenc
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_StopAenc(AR_S32 s32AencChn)
{
    AR_S32 s32Ret;

    s32Ret = AR_MPI_AENC_DestroyChn(s32AencChn);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
            s32AencChn, s32Ret);
        return s32Ret;
    }

    return AR_SUCCESS;
}

/******************************************************************************
* function : get frame from file, send it  to Aenc
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AENCProc(void* parg)
{
    SAMPLE_AENC_S* pstAeCtl = (SAMPLE_AENC_S*)parg;
    AUDIO_FRAME_S stFrame;
    AR_S32 s32Ret;
    AR_U32 u32ReadLen0;
    AR_U32 u32ReadLen1;

    FILE* chn0_fd = pstAeCtl->pfd[0];
    FILE* chn1_fd = pstAeCtl->pfd[1];

    AR_U32 u32Len = AACLC_SAMPLES_PER_FRAME * 2;
    AR_CHAR *frame0 = (AR_CHAR*)malloc(u32Len);
    AR_CHAR *frame1 = (AR_CHAR*)malloc(u32Len);

    memset(frame0, 0, u32Len);
    memset(frame1, 0, u32Len);
    memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));

    stFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
    stFrame.enSoundmode = AUDIO_SOUND_MODE_STEREO;
    stFrame.u32Len = u32Len;
    (stFrame.u64VirAddr)[0] = frame0;
    (stFrame.u64VirAddr)[1] = frame1;
    (stFrame.u32PoolId)[0] = 1;
    (stFrame.u32PoolId)[1] = 1;

    while(pstAeCtl->bStart)
    {
        if(AR_ERR_AENC_NOBUF != s32Ret)
        {
            u32ReadLen0 = fread(frame0, 1, u32Len, chn0_fd);
            u32ReadLen1 = fread(frame1, 1, u32Len, chn1_fd);
            if (u32ReadLen0 != u32Len || u32ReadLen1 != u32Len){
                printf("%s: fread0(%d) fread1(%d) but expect (%d)\n", __FUNCTION__, u32ReadLen0, u32ReadLen1, u32Len);
                break;
            }
        }
        /*** 音频编码发送数据帧是非阻塞接口，如果音频码流缓存满，则直接返回失败。***/
        /*** 该接口用于用户主动发送音频帧进行编码，如果 AENC 通道已经通过系统绑定 （ar_hal_sys_bind）接口与 AI 绑定，不需要也不建议调此接口。***/
        s32Ret = AR_MPI_AENC_SendFrame(pstAeCtl->AeChn, &stFrame, NULL);
        if (AR_SUCCESS != s32Ret)
        {
            printf("%s: ar_hal_aenc_send_frame(%d), failed with %#x!\n", \
                __FUNCTION__, pstAeCtl->AeChn, s32Ret);
        }
        /*** 如果取码流较慢，编码较快时，会导致编码缓存不足，建议加延时 ***/
        usleep(100 * 1000);
    }

    fclose(chn0_fd);
    fclose(chn1_fd);
    return NULL;
}

/******************************************************************************
* function : Create the thread to get frame from pcm file and send to aenc
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_CreatTrdAenc(AR_S32 AeChn, FILE* pAecFd0, FILE* pAecFd1)
{
    SAMPLE_AENC_S* pstAenc = NULL;

    if (NULL == pAecFd0 || NULL == pAecFd1)
    {
        return AR_FAILURE;
    }

    pstAenc = &gs_stSampleAenc[AeChn];
    pstAenc->AeChn = AeChn;
    pstAenc->AdChn = -1;
    pstAenc->bSendAdChn = AR_TRUE;
    (pstAenc->pfd)[0] = pAecFd0;
    (pstAenc->pfd)[1] = pAecFd1;
    pstAenc->bStart = AR_TRUE;
    pthread_create(&pstAenc->stAencPid_in, 0, SAMPLE_COMM_AUDIO_AENCProc, pstAenc);


    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdAenc(AR_S32 AeChn)
{
    SAMPLE_AENC_S* pstAenc = NULL;

    pstAenc = &gs_stSampleAenc[AeChn];

    if (pstAenc->bStart)
    {
        pstAenc->bStart = AR_FALSE;
        pthread_join(pstAenc->stAencPid_in, 0);
    }

    return AR_SUCCESS;
}

/******************************************************************************
* function : get stream from Aenc, send it  to Adec & save it to file
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AencProc(void* parg)
{
  AR_S32 s32Ret;
  SAMPLE_AENC_S* pstAencCtl = (SAMPLE_AENC_S*)parg;
  AUDIO_STREAM_S stStream;

  while (pstAencCtl->bStart)
  {
    /* get stream from aenc chn */
    s32Ret = AR_MPI_AENC_GetStream(pstAencCtl->AeChn, &stStream, 1000);
    if (AR_SUCCESS != s32Ret )
    {
      printf("%s: AR_MPI_AENC_GetStream(%d), timeout %dms with ret:%#x!\n", \
        __FUNCTION__, pstAencCtl->AeChn, 2, s32Ret);
      continue;
    }

    /* save audio stream to file */
    (AR_VOID)fwrite(stStream.pStream, stStream.u32Len, 1, pstAencCtl->out_pfd);

    /* finally you must release the stream */
    s32Ret = AR_MPI_AENC_ReleaseStream(pstAencCtl->AeChn, &stStream);
    if (AR_SUCCESS != s32Ret )
    {
      printf("%s: AR_MPI_AENC_ReleaseStream(%d), failed with %#x!\n", \
           __FUNCTION__, pstAencCtl->AeChn, s32Ret);
      pstAencCtl->bStart = AR_FALSE;
      continue;
    }

    fflush(pstAencCtl->out_pfd);

  }

  printf("%s: AeChn:%d, file write ok !\n", \
       __FUNCTION__, pstAencCtl->AeChn);

  return NULL;
}


/******************************************************************************
* function : Create the thread to get stream from aenc and save to file
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_CreatTrdAencFile(AR_S32 AeChn, FILE* pAecFd)
{
  SAMPLE_AENC_S* pstAenc = NULL;

  if (NULL == pAecFd)
  {
    return AR_FAILURE;
  }

  pstAenc = &gs_stSampleAenc[AeChn];
  pstAenc->AeChn = AeChn;
  pstAenc->bSendAdChn = AR_TRUE;
  pstAenc->out_pfd = pAecFd;
  pstAenc->bStart = AR_TRUE;
  pthread_create(&pstAenc->stAencPid_out, 0, SAMPLE_COMM_AUDIO_AencProc, pstAenc);

  return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencFile(AR_S32 AeChn)
{
  SAMPLE_AENC_S* pstAenc = NULL;

  pstAenc = &gs_stSampleAenc[AeChn];

  if (pstAenc->bStart)
  {
    pstAenc->bStart = AR_FALSE;
  }

  pthread_join(pstAenc->stAencPid_out, 0);

  return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_SetStopAencStatus(AR_S32 AeChn)
{
  gs_stSampleAenc[AeChn].bStart = AR_FALSE;
  return 0;
}




/******************************************************************************
* function : bind
******************************************************************************/

AR_S32 SAMPLE_COMM_AUDIO_AiBindAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AENC;
    stDestChn.s32DevId = AencDev;
    stDestChn.s32ChnId = AencChn;

    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_COMM_AUDIO_AiUnbindAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AENC;
    stDestChn.s32DevId = AencDev;
    stDestChn.s32ChnId = AencChn;

    return AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}


AR_S32 SAMPLE_COMM_AUDIO_AencBindAdec(AR_S32 AencDev, AR_S32 AencChn, AR_S32 AdecDev, AR_S32 AdecChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_AENC;
    stSrcChn.s32ChnId = AencChn;
    stSrcChn.s32DevId = AencDev;
    stDestChn.enModId = AR_ID_ADEC;
    stDestChn.s32DevId = AdecDev;
    stDestChn.s32ChnId = AdecChn;

    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_COMM_AUDIO_AencUnbindAdec(AR_S32 AencDev, AR_S32 AencChn, AR_S32 AdecDev, AR_S32 AdecChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_AENC;
    stSrcChn.s32ChnId = AencChn;
    stSrcChn.s32DevId = AencDev;
    stDestChn.enModId = AR_ID_ADEC;
    stDestChn.s32DevId = AdecDev;
    stDestChn.s32ChnId = AdecChn;

    return AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_COMM_AUDIO_AdecBindAo(AR_S32 AdecDev, AR_S32 AdecChn, AR_S32 AoDev, AR_S32 AoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_ADEC;
    stSrcChn.s32ChnId = AdecChn;
    stSrcChn.s32DevId = AdecDev;
    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_COMM_AUDIO_AdecUnbindAo(AR_S32 AdecDev, AR_S32 AdecChn, AR_S32 AoDev, AR_S32 AoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_ADEC;
    stSrcChn.s32ChnId = AdecChn;
    stSrcChn.s32DevId = AdecDev;
    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

