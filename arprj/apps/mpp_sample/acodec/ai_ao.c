#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "mpi_audio.h"
#include "acodec.h"
#include "mpi_sys.h"
#include "audio_sample.h"

#define ACODEC_FILE     "/dev/acodec"

typedef struct tagSAMPLE_AIEC_S
{
    AR_BOOL bStart;
    AR_S32 AiDev;
    AR_S32 AiChn;
    FILE* pWfd;
    pthread_t stAiPid_out;
} SAMPLE_AIEC_S;


static unsigned int g_run = 0;

AR_S32 SAMPLE_COMM_AUDIO_StartAi(AR_S32 AiDevId, AR_S32 s32AiChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, AR_BOOL bResampleEn, AR_VOID* pstAiVqeAttr, AR_U32 u32AiVqeType)
{
    AR_S32 i;
    AR_S32 s32Ret;
    AI_CHN_PARAM_S pstChnParam = { 0 };

    s32Ret = AR_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
    if (s32Ret)
    {
        printf("%s: AR_MPI_AI_SetPubAttr(%d) failed with %#x\n", __func__, AiDevId, s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AI_Enable(AiDevId);
    if (s32Ret)
    {
        printf("%s: AR_MPI_AI_Enable(%d) failed with %#x\n", __func__, AiDevId, s32Ret);
        return s32Ret;
    }

    for (i = 0; i < s32AiChnCnt>>pstAioAttr->enSoundmode; i++)
    {
        s32Ret = AR_MPI_AI_EnableChn(AiDevId, i);
        if (s32Ret)
        {
            printf("%s: AR_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __func__, AiDevId, i, s32Ret);
            return s32Ret;
        }
        pstChnParam.u32UsrFrmDepth = 6;
        s32Ret = AR_MPI_AI_SetChnParam(AiDevId, i, &pstChnParam);
        if(s32Ret != AR_SUCCESS)
        {
            printf("[%s][%d]--->>>AI Chn%d setParam failed!ret:%#x\n",__func__,__LINE__, i, s32Ret);
            return s32Ret;
        }
    }


    return AR_SUCCESS;
}

AR_S32 SAMPLE_INNER_CODEC_CfgAudio(AUDIO_SAMPLE_RATE_E enSample)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    int iAcodecInputVol = 0;
    int iAcodecOutputVol = 0;
    ACODEC_MIXER_E input_mode = 0;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SOFT_RESET_CTRL, NULL, 0))
    {
        printf("Reset audio codec error\n");
    }

    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_I2S1_FS, &enSample, sizeof(enSample)))
    {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        ret = AR_FAILURE;
    }

    input_mode = ACODEC_MIXER_IN1;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_MIXER_MIC, &input_mode, sizeof(input_mode)))
    {
        printf("%s: select acodec input_mode failed\n", __FUNCTION__);
        ret = AR_FAILURE;
    }

    if (1) /* should be 1 when micin */
    {
        /******************************************************************************************
        The input volume range is [-87, +86]. Both the analog gain and digital gain are adjusted.
        A larger value indicates higher volume.
        For example, the value 86 indicates the maximum volume of 86 dB,
        and the value -87 indicates the minimum volume (muted status).
        The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
        The recommended volume range is [+10, +56].
        Within this range, the noises are lowest because only the analog gain is adjusted,
        and the voice quality can be guaranteed.
        *******************************************************************************************/
        iAcodecInputVol = 47;
        if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_INPUT_VOL, &iAcodecInputVol, sizeof(iAcodecInputVol)))
        {
            printf("%s: set acodec micin volume failed\n", __FUNCTION__);
            return AR_FAILURE;
        }

        iAcodecOutputVol = 1;
        if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_OUTPUT_VOL, &iAcodecOutputVol, sizeof(iAcodecOutputVol)))
        {
            printf("%s: set acodec lineout volume failed\n", __FUNCTION__);
            return AR_FAILURE;
        }

    }

    close(fdAcodec);
    return ret;
}

 /* config codec */
 AR_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S* pstAioAttr)
 {
     AR_S32 s32Ret = AR_SUCCESS;
     AR_BOOL bCodecCfg = AR_FALSE;

     /*** INNER AUDIO CODEC ***/
     s32Ret = SAMPLE_INNER_CODEC_CfgAudio(pstAioAttr->enSamplerate);
     if (AR_SUCCESS != s32Ret)
     {
         printf("%s:SAMPLE_INNER_CODEC_CfgAudio failed\n", __FUNCTION__);
         return s32Ret;
     }
     bCodecCfg = AR_TRUE;

     if (!bCodecCfg)
     {
         printf("Can not find the right codec.\n");
         return AR_FALSE;
     }
     return AR_SUCCESS;
 }

 AR_S32 SAMPLE_COMM_AUDIO_StopAi(AR_S32 AiDevId, AR_S32 s32AiChnCnt,
                                 AR_BOOL bResampleEn, AR_BOOL bVqeEn)
 {
     AR_S32 i;
     AR_S32 s32Ret;

     for (i = 0; i < s32AiChnCnt; i++)
     {
         s32Ret = AR_MPI_AI_DisableChn(AiDevId, i);
         if (AR_SUCCESS != s32Ret)
         {
             printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
             return s32Ret;
         }
     }

     s32Ret = AR_MPI_AI_Disable(AiDevId);
     if (AR_SUCCESS != s32Ret)
     {
         printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
         return s32Ret;
     }

     return AR_SUCCESS;
 }

AR_S32 SAMPLE_COMM_AUDIO_StartAo(AR_S32 AoDevId, AR_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, AR_BOOL bResampleEn)
{
    AR_S32 i;
    AR_S32 s32Ret;

    s32Ret = AR_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               AoDevId, s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_AO_Enable(AoDevId);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
        return AR_FAILURE;
    }

    for (i = 0; i < s32AoChnCnt>>pstAioAttr->enSoundmode; i++)
    {
        s32Ret = AR_MPI_AO_EnableChn(AoDevId, i);
        if (AR_SUCCESS != s32Ret)
        {
            printf("%s: AR_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
            return AR_FAILURE;
        }
    }

    /*s32Ret = AR_MPI_AO_EnableChn(AoDevId, AO_SYSCHN_CHNID);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
        return AR_FAILURE;
    }*/

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_StopAo(AR_S32 AoDevId, AR_S32 s32AoChnCnt, AR_BOOL bResampleEn)
{
    AR_S32 i;
    AR_S32 s32Ret;

    for (i = 0; i < s32AoChnCnt; i++)
    {
        s32Ret = AR_MPI_AO_DisableChn(AoDevId, i);
        if (AR_SUCCESS != s32Ret)
        {
            printf("%s: AR_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    s32Ret = AR_MPI_AO_Disable(AoDevId);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_AoBindAi(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AoDev, AR_S32 AoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AoDev, AR_S32 AoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

AR_S32 SAMPLE_AUDIO_AiAo(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AoDev, AR_S32 AoChn)
{
    AR_S32 s32Ret;
    AR_S32 s32AiChnCnt;
    AR_S32 s32AoChnCnt;
    AIO_ATTR_S stAioAttr;

    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 8;
    stAioAttr.u32PtNumPerFrm = 1024;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;

    /* enable AI channle from 0~4 */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0, NULL, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AIAO_ERR3;
    }

    /* enable AO channle */
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    //stAioAttr.u32FrmNum = 8;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
         goto AIAO_ERR2;
    }

    /* config internal audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AIAO_ERR1;
    }

#if 0
    AR_S32 AiFd;
    fd_set read_fds;
    struct timeval TimeoutVal;
    FILE *fp_l = NULL;
    FILE *fp_r = NULL;
    unsigned int index = 0;
    STRU_AUDIO_FRAME stFrame;
    STRU_AI_CHN_PARAM stAiChnPara;

    fp_l = fopen("/tmp/record_l.pcm", "wb");
    fp_r = fopen("/tmp/record_r.pcm", "wb");

    s32Ret = ar_hal_ai_get_chn_param(AiDev, AiChn, &stAiChnPara);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return -1;
    }

    stAiChnPara.u32UsrFrmDepth = 8;

    s32Ret = ar_hal_ai_set_chn_param(AiDev, AiChn, &stAiChnPara);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return -1;
    }

    FD_ZERO(&read_fds);
    AiFd = ar_hal_ai_get_fd(AiDev, AiChn);
    FD_SET(AiFd, &read_fds);

    while(g_run)
    {
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);

        s32Ret = select(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            s32Ret = ar_hal_ai_get_frame(AiDev, AiChn, &stFrame, NULL, AR_FALSE);
            //s32Ret = ar_hal_ai_get_frame(AiDev, AiChn, &stFrame, NULL, -1);
            if (AR_SUCCESS != s32Ret )
            {
                printf("%s: AR_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, AiDev, AiChn, s32Ret);
                return -1;
            }

            /*if(index < 4096)
            {
                fwrite(stFrame.u64VirAddr[0], stFrame.u32Len, 1, fp_l);
                fwrite(stFrame.u64VirAddr[1], stFrame.u32Len, 1, fp_r);
            }

            printf("get aframe: bit %d, mode %d, len %u, time %llu, seq %u, pa: 0x%llx 0x%llx va %p %p data %08x %08x %08x %08x\n",
                        stFrame.enBitwidth,
                        stFrame.enSoundmode,
                        stFrame.u32Len,
                        stFrame.u64TimeStamp,
                        stFrame.u32Seq,
                        stFrame.u64PhyAddr[0],
                        stFrame.u64PhyAddr[1],
                        stFrame.u64VirAddr[0],
                        stFrame.u64VirAddr[1],
                        ((unsigned int *)stFrame.u64VirAddr[0])[0],
                        ((unsigned int *)stFrame.u64VirAddr[1])[0],
                        ((unsigned int *)stFrame.u64VirAddr[0])[1],
                        ((unsigned int *)stFrame.u64VirAddr[1])[1]);*/

            s32Ret = ar_hal_ao_send_frame(AoDev, AoChn, &stFrame, 1000);
            if (AR_SUCCESS != s32Ret )
            {
                printf("%s: ar_hal_ao_send_frame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, AoDev, AoChn, s32Ret);
                return -1;
            }

            /* finally you must release the stream */
            s32Ret = ar_hal_ai_release_frame(AiDev, AiChn, &stFrame, NULL);
            if (AR_SUCCESS != s32Ret )
            {
                printf("%s: AR_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, AiDev, AiChn, s32Ret);
                return -1;
            }

            ++index;
        }
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    fclose(fp_l);
    fclose(fp_r);
#else

    s32Ret = SAMPLE_COMM_AUDIO_AoBindAi(AiDev, AiChn, AoDev, AoChn);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AIAO_ERR1;
    }

    printf("ai(%d,%d) bind to ao(%d,%d) ok\n", AiDev, AiChn, AoDev, AoChn);

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    SAMPLE_COMM_AUDIO_AoUnbindAi(AiDev, AiChn, AoDev, AoChn);

#endif

AIAO_ERR1:

    s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
    }

AIAO_ERR2:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, 0, AR_FALSE);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
    }

AIAO_ERR3:

    return s32Ret;
}

/******************************************************************************
* function : get frame from AI, and save to File
******************************************************************************/

void* SAMPLE_COMM_AUDIO_AiProc(void* parg)
{
    AR_S32 s32Ret = 0, i = 0;
    AUDIO_FRAME_S stAudioFrame = { 0 };
    AR_U32 u32WriteLen = 0;
    SAMPLE_AIEC_S* pstAiCtl = (SAMPLE_AIEC_S*)parg;
    FILE* pWd = pstAiCtl->pWfd;

    while (AR_TRUE == pstAiCtl->bStart)
    {
        s32Ret = AR_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAudioFrame, NULL, -1);
        if (s32Ret) {
            printf("ai get frame failed %d.\n", s32Ret);
        }
        for (i = 0; i < 2; i++) {
            u32WriteLen = fwrite(stAudioFrame.u64VirAddr[i], 1, (stAudioFrame.u32Len>>1), pWd);
            if (u32WriteLen != stAudioFrame.u32Len)
                printf("write audio frame%d error write %u frome %u.\n", i, u32WriteLen, (stAudioFrame.u32Len>>1));
        }
    }
    printf("%s: free and close!\n", __FUNCTION__);
    fclose(pWd);
    return NULL;
}

/******************************************************************************
* function : Create the thread to get frame from Ai and save to File
******************************************************************************/
static SAMPLE_AIEC_S pstAi;

AR_S32 SAMPLE_COMM_AUDIO_CreatGetTrdFileAi(AR_S32 AiDev, AR_S32 AiChn, FILE* pAiFd)
{
    if (NULL == pAiFd)
    {
        return AR_FAILURE;
    }
    pstAi.AiDev = AiDev;
    pstAi.AiChn = AiChn;
    pstAi.pWfd = pAiFd;
    pstAi.bStart = AR_TRUE;
    pthread_create(&pstAi.stAiPid_out, 0, SAMPLE_COMM_AUDIO_AiProc, &pstAi);

    return AR_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get frame from Ai and save to File
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAi(AR_S32 AiDev)
{

    if (pstAi.bStart)
    {
        pstAi.bStart = AR_FALSE;
        //pthread_cancel(pstAdec->stAdPid);
        pthread_join(pstAi.stAiPid_out, 0);
    }

    return AR_SUCCESS;
}
AR_S32 SAMPLE_COMM_AUDIO_SetStopAiStatus(AR_S32 AiDev)
{
    pstAi.bStart = AR_FALSE;
    return 0;
}

AR_S32 SAMPLE_AUDIO_AiToFile(AR_S32 AiDev, AR_S32 AiChn, FILE* pAiFd)
{
    AR_S32 s32Ret = 0;
    AR_S32 s32AiChnCnt = 1;
    AIO_ATTR_S stAioAttr = { 0 };

    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 8;
    stAioAttr.u32PtNumPerFrm = 1024;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
    /* for i2s 8 channel is avaliable */
    if (AiDev > 0)
        s32AiChnCnt = 8;
    /* enable AI channle */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0, NULL, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AiToFile_ERR3;
    }
    /* config internal audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AiToFile_ERR2;
    }
    s32Ret = SAMPLE_COMM_AUDIO_CreatGetTrdFileAi(AiDev, 0, pAiFd);
    if (s32Ret == AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AiToFile_ERR2;
    }
AiToFile_ERR1:
    s32Ret |= SAMPLE_COMM_AUDIO_DestoryTrdFileAi(AiDev);
AiToFile_ERR2:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, 0, AR_FALSE);
AiToFile_ERR3:
    return s32Ret;
}
AR_S32 SAMPLE_AUDIO_Ao(AR_VOID)
{
    AR_S32 s32Ret = -1;
    AR_S32 s32AoChnCnt;
    AR_S32      AoChn = 0;
    AIO_ATTR_S stAioAttr;
    AR_S32   AoDev = 0;

    FILE    *fp = NULL;
    int     file_len = 0;
    int     buffer_len = 0;

    unsigned int index = 0;
    AUDIO_FRAME_S stFrame;
    unsigned short * audio_data = NULL;
    unsigned short * audio_data_interlace = NULL;
    unsigned char  * p_l = NULL;
    unsigned char  * p_r = NULL;

    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 8;
    stAioAttr.u32PtNumPerFrm = 1024;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;

    buffer_len = (stAioAttr.u32PtNumPerFrm * (stAioAttr.enBitwidth + 1));

    fp = fopen("/tmp/record.pcm", "rb");
    if(!fp)
    {
        printf("%s: file open failed\n", __FUNCTION__);
        goto AIAO_ERR2;
    }

    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    audio_data = (unsigned short *)malloc(file_len);
    audio_data_interlace = (unsigned short *)malloc(file_len);

    fread(audio_data, file_len, 1, fp);
    printf("file len %d\n", file_len);

    for(int i = 0; i < (file_len / 2); i += 2)
    {
        audio_data_interlace[i / 2] = audio_data[i];
        audio_data_interlace[i / 2 + file_len / 2 / 2] = audio_data[i + 1];
    }

    p_l = (unsigned char *)&audio_data_interlace[index * buffer_len];
    p_r = (unsigned char *)&audio_data_interlace[file_len / 2 / 2];

    /* enable AO channle */
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
         goto AIAO_ERR2;
    }

    /* config internal audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
        goto AIAO_ERR1;
    }

    while(g_run)
    {
        VB_BLK block_handle[2];
        VB_POOL pool_id[2];
        unsigned char * data[2];

        if(index * buffer_len >= (file_len / 2))
        {
            printf("\ncurrent len %d file len %d %d\n", index * buffer_len, file_len, (file_len / 2));
            break;
        }

        block_handle[0] = AR_MPI_VB_GetBlock(-1, buffer_len, NULL);
        if(block_handle[0] == VB_INVALID_HANDLE)
        {
            break;
        }

        block_handle[1] = AR_MPI_VB_GetBlock(-1, buffer_len, NULL);
        if(block_handle[1] == VB_INVALID_HANDLE)
        {
            break;
        }

        pool_id[0] = AR_MPI_VB_Handle2PoolId(block_handle[0]);
        pool_id[1] = AR_MPI_VB_Handle2PoolId(block_handle[1]);

        AR_MPI_VB_MmapPool(pool_id[0]);
        AR_MPI_VB_MmapPool(pool_id[1]);

        AR_MPI_VB_GetBlockVirAddr(pool_id[0], AR_MPI_VB_Handle2PhysAddr(block_handle[0]), (void **)(&data[0]));
        AR_MPI_VB_GetBlockVirAddr(pool_id[1], AR_MPI_VB_Handle2PhysAddr(block_handle[1]), (void **)(&data[1]));

        memcpy(data[0], &p_l[index * buffer_len], buffer_len);
        memcpy(data[1], &p_r[index * buffer_len], buffer_len);

        stFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
        stFrame.enSoundmode = AUDIO_SOUND_MODE_STEREO;

        stFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(block_handle[0]);
        stFrame.u64PhyAddr[1] = AR_MPI_VB_Handle2PhysAddr(block_handle[1]);
        stFrame.u64VirAddr[0] = data[0];
        stFrame.u64VirAddr[1] = data[1];
        stFrame.u64TimeStamp = 0;
        stFrame.u32Seq = index;
        stFrame.u32Len = buffer_len;
        stFrame.u32PoolId[0] = pool_id[0];
        stFrame.u32PoolId[1] = pool_id[1];

        /*printf("get aframe: bit %d, mode %d, len %u, time %llu, seq %u, pa: 0x%llx 0x%llx va %p %p data %08x %08x %08x %08x\n",
                    stFrame.enBitwidth,
                    stFrame.enSoundmode,
                    stFrame.u32Len,
                    stFrame.u64TimeStamp,
                    stFrame.u32Seq,
                    stFrame.u64PhyAddr[0],
                    stFrame.u64PhyAddr[1],
                    stFrame.u64VirAddr[0],
                    stFrame.u64VirAddr[1],
                    ((unsigned int *)stFrame.u64VirAddr[0])[0],
                    ((unsigned int *)stFrame.u64VirAddr[1])[0],
                    ((unsigned int *)stFrame.u64VirAddr[0])[1],
                    ((unsigned int *)stFrame.u64VirAddr[1])[1]);*/

        s32Ret = AR_MPI_AO_SendFrame(AoDev, AoChn, &stFrame, -1);
        if (AR_SUCCESS != s32Ret )
        {
            printf("%s: ar_hal_ao_send_frame(%d, %d), failed with %#x!\n", \
                   __FUNCTION__, AoDev, AoChn, s32Ret);
            return -1;
        }

        AR_MPI_VB_ReleaseBlock(block_handle[0]);
        AR_MPI_VB_ReleaseBlock(block_handle[1]);

        ++index;
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

AIAO_ERR1:

    s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
    }

    free(audio_data);
    free(audio_data_interlace);

    s32Ret = 0;

AIAO_ERR2:


    return s32Ret;
}

int find_param_index_by_name(char *name,int argc, char *argv[])
{
    int i=0;
    for(i=0;i<argc;i++)
    {
        if(!strcmp(name,argv[i]))
        {
            return i;
        }
    }
    return -1;
}

int acodec_cmd_ioctl(int argc, char * argv[])
{
    int                 fdAcodec    = -1;
    int                 ret         = AR_SUCCESS;
    int                 index       = 0;
    void                *arg        = NULL;
    int                 ctl         = 0;
    unsigned int        size        = 0;
    ACODEC_IOCTL_E        cmd         = 0;
    ACODEC_VOL_CTRL     vol_ctl     = {0};

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    index = find_param_index_by_name("-cmd", argc, argv);
    if(NULL == argv[index + 1])
    {
        printf("%s: must input ioctl index\n", __FUNCTION__);
        return AR_FAILURE;
    }

    cmd = atoi(argv[index + 1]);
    if(cmd < IOC_NR_SOFT_RESET_CTRL || cmd > IOC_NR_ADCR_SEL_TRACK)
    {
        printf("%s: cmd %d out of index!\n", __FUNCTION__, cmd );
        return AR_FAILURE;
    }

    switch(cmd)
    {
        case IOC_NR_SOFT_RESET_CTRL:
            break;

        case IOC_NR_SET_DACL_VOL:
        case IOC_NR_SET_DACR_VOL:
        case IOC_NR_SET_ADCL_VOL:
        case IOC_NR_SET_ADCR_VOL:
            if(argv[index + 2] && argv[index + 3])
            {
                vol_ctl.vol_ctrl =  atoi(argv[index + 2]);
                vol_ctl.vol_ctrl_mute =  atoi(argv[index + 3]);
                arg = &vol_ctl;
                size = sizeof(ACODEC_VOL_CTRL);
            }
            else
            {
                printf("%s: cmd %d input format error!\n", __FUNCTION__, cmd);
                return -1;
            }
            break;
        case IOC_NR_GET_DACL_VOL:
        case IOC_NR_GET_DACR_VOL:
        case IOC_NR_GET_ADCL_VOL:
        case IOC_NR_GET_ADCR_VOL:
            arg = &vol_ctl;
            size = sizeof(ACODEC_VOL_CTRL);
            break;

        default:
            if(argv[index + 2])
            {
                ctl = atoi(argv[index + 2]);
            }
            arg = &ctl;
            size = sizeof(int);
            break;
    }

    ret = ar_mpi_acodec_ioctl(fdAcodec, cmd, arg, size);
    if(ret >= 0)
    {
            switch(cmd)
            {
                case IOC_NR_SOFT_RESET_CTRL:
                    break;

                case IOC_NR_SET_DACL_VOL:
                case IOC_NR_SET_DACR_VOL:
                case IOC_NR_SET_ADCL_VOL:
                case IOC_NR_SET_ADCR_VOL:
                    break;
                case IOC_NR_GET_DACL_VOL:
                case IOC_NR_GET_DACR_VOL:
                case IOC_NR_GET_ADCL_VOL:
                case IOC_NR_GET_ADCR_VOL:
                    printf("result: vol_ctl %d %d\n", vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);
                    break;

                default:
                    if(!argv[index + 2])
                    {
                        printf("result: ctl %d\n", ctl);
                    }
                    break;
            }

    }
    else
    {
        printf("%s: cmd %d not support or execute failed!\n", __FUNCTION__, cmd);
    }

    close(fdAcodec);
    return ret;
}


//capture exit signal
static void sighandler(int signo)
{
    g_run = 0;
}

