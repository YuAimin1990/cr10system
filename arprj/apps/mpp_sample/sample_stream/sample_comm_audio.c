
#include "sample_comm_audio.h"
#include "ar_comm_aio.h"
#include "ar_comm_aenc.h"
#include "mpi_sys.h"
#include "hal_acodec.h"
#include "acodec.h"
#include "audio_aac_adp.h"

#include <fcntl.h>

#define ACODEC_FILE     "/dev/acodec"


AR_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV audDevId, AR_S32 s32ChnNum, AIO_ATTR_S* pstAioAttr)
{
    AR_S32 i;
    AR_S32 s32Ret;

	AI_CHN_PARAM_S stChnParam;
	
	s32Ret = AR_MPI_AI_SetPubAttr(audDevId, pstAioAttr);
    if (s32Ret)
    {
        printf("%s: AR_MPI_AI_SetPubAttr(%d) failed with %#x\n", __func__, audDevId, s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AI_Enable(audDevId);
    if (s32Ret)
    {
        printf("%s: AR_MPI_AI_Enable(%d) failed with %#x\n", __func__, audDevId, s32Ret);
        return s32Ret;
    }

    for (int i = 0; i < s32ChnNum/pstAioAttr->u32ChnCnt; i++)
    {
        s32Ret = AR_MPI_AI_EnableChn(audDevId, i);
        if (s32Ret){
            printf("%s: AR_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __func__, audDevId, i, s32Ret);
            return s32Ret;
        }
        // stChnParam.u32UsrFrmDepth = 6;
        // s32Ret = AR_MPI_AI_SetChnParam(audDevId,i,&stChnParam);
        // if(s32Ret != AR_SUCCESS){
        //     printf("AI_SetChnparam Failed 0x%x\n",s32Ret);
        // }
    }
    return AR_SUCCESS;
}

 AR_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV audioDevId, AR_S32 s32ChnNum)
 {
     AR_S32 s32Ret;

     for (int i = 0; i < s32ChnNum; i++)
     {
         s32Ret = AR_MPI_AI_DisableChn(audioDevId, i);
         if (AR_SUCCESS != s32Ret)
         {
             printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
             return s32Ret;
         }
     }

     s32Ret = AR_MPI_AI_Disable(audioDevId);
     if (AR_SUCCESS != s32Ret)
     {
         printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
         return s32Ret;
     }
     return AR_SUCCESS;
 }


AR_S32 SAMPLE_COMM_AUDIO_SetCodec(AUDIO_SAMPLE_RATE_E enSample)
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
    if (ar_mpi_acodec_ioctl(fdAcodec,IOC_NR_SET_MIXER_MIC, &input_mode, sizeof(input_mode)))
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
     s32Ret = SAMPLE_COMM_AUDIO_SetCodec(pstAioAttr->enSamplerate);
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




AR_S32 SAMPLE_STREAM_AI_SetVolume(AUDIO_SAMPLE_RATE_E enSample,AR_S32 s32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    int iAcodecInputVol = 0;
    ENMU_AR_ACODEC_MIXER input_mode = 0;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }
    if (ar_hal_acodec_ioctl(fdAcodec, AR_AC_IOC_NR_SOFT_RESET_CTRL, NULL, 0))
    {
        printf("Reset audio codec error\n");
    }

    if (ar_hal_acodec_ioctl(fdAcodec, AR_AC_IOC_NR_SET_I2S1_FS, &enSample, sizeof(enSample)))
    {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        ret = AR_FAILURE;
    }

    /******************************************************************************************
    The input volume range is [-97, +76]. Both the analog gain and digital gain are adjusted.
    A larger value indicates higher volume.
    For example, the value 76 indicates the maximum volume of 76 dB,
    and the value -97 indicates the minimum volume (muted status).
    The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
    The recommended volume range is [0, +46].
    Within this range, the noises are lowest because only the analog gain is adjusted,
    and the voice quality can be guaranteed.
    *******************************************************************************************/
    iAcodecInputVol = s32Volume;
    if (ar_hal_acodec_ioctl(fdAcodec, AR_AC_IOC_NR_SET_INPUT_VOL, &iAcodecInputVol, sizeof(iAcodecInputVol)))
    {
        printf("%s: set acodec micin volume failed\n", __FUNCTION__);
        close(fdAcodec);
        return AR_FAILURE;
    }
    close(fdAcodec);
    return ret;
}

AR_S32 SAMPLE_COMM_AUDIO_StartAenc(AENC_CHN audioEncChn, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
    AR_S32 s32Ret;
    AENC_CHN_ATTR_S stAencAttr;
    AENC_ATTR_ADPCM_S stAdpcmAenc;
    AENC_ATTR_G711_S stAencG711;
    AENC_ATTR_G726_S stAencG726;
    AENC_ATTR_LPCM_S stAencLpcm;
    AENC_ATTR_AAC_S  stAencAac = {0};     // AAC协议参数

    /* set AENC chn attr */

    stAencAttr.enType = enType;          //音频编码协议类型
    stAencAttr.u32BufSize = 30;          //音频编码协议对应的帧长
    stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm;  //音频编码缓存大小，以帧为单位

    if (PT_ADPCMA == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAdpcmAenc;
        stAdpcmAenc.enADPCMType = ADPCM_TYPE_DVI4;
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
        stAencG726.enG726bps    = MEDIA_G726_40K;
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
        stAencAac.enAACType = AAC_TYPE_AACLC;
        stAencAac.enBitRate = AAC_BPS_48K;//gs_enAacBps;//pstAioAttr->enSamplerate * 2 ;//gs_enAacBps;
        stAencAac.enBitWidth = pstAioAttr->enBitwidth;
        stAencAac.enSmpRate = pstAioAttr->enSamplerate;
        stAencAac.enSoundMode = pstAioAttr->enSoundmode;
        stAencAac.enTransType = AAC_TRANS_TYPE_ADTS;
        stAencAac.s16BandWidth = 0;
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
        return AR_FAILURE;
    }

    /* create aenc chn*/
    s32Ret = AR_MPI_AENC_CreateChn(audioEncChn, &stAencAttr);
	printf("%s: AR_MPI_AENC_CreateChn(%d) with %#x!\n", __FUNCTION__, audioEncChn, s32Ret);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: AR_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
               audioEncChn, s32Ret);
        return s32Ret;
    }

    return SAMPLE_COMM_AUDIO_CfgAcodec(pstAioAttr);
}


/******************************************************************************
* function : Stop Aenc
******************************************************************************/
AR_S32 SAMPLE_COMM_AUDIO_StopAenc(AENC_CHN audioEncChn)
{
	AR_S32 s32Ret;
    // Release all buffer from enc queue
    AUDIO_STREAM_S stAencStream;
    do{
        memset(&stAencStream, 0, sizeof(stAencStream));
        s32Ret = AR_MPI_AENC_GetStream(audioEncChn, &stAencStream, 1000);
        if(AR_SUCCESS != s32Ret){
            printf("Empty Used Pool\n");
            break;
        }
        AR_MPI_AENC_ReleaseStream(audioEncChn, &stAencStream);
    }while(1);
	s32Ret = AR_MPI_AENC_DestroyChn(audioEncChn);
	if (AR_SUCCESS != s32Ret)
	{
		printf("%s: AR_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
			audioEncChn, s32Ret);
		return s32Ret;
	}
	return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_AUDIO_BindAiAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn)
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

AR_S32 SAMPLE_COMM_AUDIO_UnbindAiAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn)
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
