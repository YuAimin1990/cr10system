#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"

#include <pthread.h>
#include <sys/prctl.h>

#include "util_profile.h"
#include "cfg_audio.h"
#include "cfg_venc.h"
#include "util_api.h"

#include "pf_audio.h"
#include "pf_drv.h"
#include "pf_mbuf.h"
#include "util_profile.h"
#include "media_fifo.h"
#include "cfg_channel.h"
#include "hal_clk.h"
#include "tp9930_audio.h"

#define AUDIO_POINT_NUM   160
#define ACODEC_FILE     "/dev/acodec"

#define AI_VOL 50

typedef struct
{
    AR_U32 ChunkID;             
    AR_U32 ChunkSize;          
    AR_U32 Format;             
    AR_U32 SubChunk1ID;        
    AR_U32 SubChunk1Size;     
    AR_U16 AudioFormat;       
    AR_U16 NumChannels;       
    AR_U32 SampleRate;      
    AR_U32 ByteRate;           
    AR_U16 BlockAlign;       
    AR_U16 BitsPerSample;    
    AR_U32 DataTag;           
    AR_U32 DataLen;			
} IPC_AUDIO_WAV_HEAD_S;

void IPC_PF_WAV_HeaderInit(IPC_AUDIO_WAV_HEAD_S * pstWavHead, AR_U32 u32DataLen, AR_U32 u32Channel, AR_U32 u32SampleRate, AR_U32 u32BitsPerSample)
{
    pstWavHead->ChunkID = 0x46464952;
    pstWavHead->ChunkSize = u32DataLen + 36;
    pstWavHead->Format = 0x45564157; 
    pstWavHead->SubChunk1ID = 0x20746d66;
    pstWavHead->SubChunk1Size = 0x10;
    pstWavHead->AudioFormat = 0x1;
    pstWavHead->BitsPerSample = u32BitsPerSample;
    pstWavHead->NumChannels = u32Channel;
    pstWavHead->SampleRate = u32SampleRate;
    pstWavHead->ByteRate = pstWavHead->BitsPerSample * pstWavHead->SampleRate * pstWavHead->NumChannels/8;
    pstWavHead->BlockAlign = 0x4;	
    pstWavHead->DataTag = 0x61746164;
    pstWavHead->DataLen = u32DataLen;
}

//static AR_U32 s_u32AudioInit = 0;
//static pthread_mutex_t s_AudioMutex = PTHREAD_MUTEX_INITIALIZER;
static AR_U32 s_u32AThreadStop = 0;
static AR_U32 g_u32AThreadSuspend = 0;
static IPC_AUDIO_THREAD_STATUS s_u32AIThreadStatus[IPC_MAX_PIPELINE_NUM] = {0};

void *pvPortMalloc(size_t xWantedSize )
{
	return malloc(xWantedSize);
}

void vPortFree( void *pv)
{
	free(pv);
}

static AUDIO_BIT_WIDTH_E IPC_PF_Audio_GetBitWidth(AR_U32 u32SampleBitWidth)
{

    if(u32SampleBitWidth == 8)
    {
        return AUDIO_BIT_WIDTH_8;
    }
    if(u32SampleBitWidth == 16)
    {
        return AUDIO_BIT_WIDTH_16;
    }
    if(u32SampleBitWidth == 24)
    {
        return AUDIO_BIT_WIDTH_24;
    }	
    if(u32SampleBitWidth == 32)
    {
        return AUDIO_BIT_WIDTH_32;
    }	
    
    return AUDIO_BIT_WIDTH_16;
}

static AUDIO_SAMPLE_RATE_E IPC_PF_Audio_GetSampleRate(AR_U32 u32SampleRate)
{
    if(u32SampleRate == 8000)
    {
        return AUDIO_SAMPLE_RATE_8000;
    }
    if(u32SampleRate == 12000)
    {
        return AUDIO_SAMPLE_RATE_12000;
    }
    if(u32SampleRate == 11025)
    {
        return AUDIO_SAMPLE_RATE_11025;
    }	
    if(u32SampleRate == 16000)
    {
        return AUDIO_SAMPLE_RATE_16000;
    }
    if(u32SampleRate == 22050)
    {
        return AUDIO_SAMPLE_RATE_22050;
    }
    if(u32SampleRate == 24000)
    {
        return AUDIO_SAMPLE_RATE_24000;
    }
    if(u32SampleRate == 32000)
    {
        return AUDIO_SAMPLE_RATE_32000;
    }
    if(u32SampleRate == 44100)
    {
        return AUDIO_SAMPLE_RATE_44100;
    }
    if(u32SampleRate == 48000)
    {
        return AUDIO_SAMPLE_RATE_48000;
    }
    if(u32SampleRate == 64000)
    {
        return AUDIO_SAMPLE_RATE_64000;
    }
    if(u32SampleRate == 96000)
    {
        return AUDIO_SAMPLE_RATE_96000;
    }
    
    return AUDIO_SAMPLE_RATE_48000;
}

AR_S32 IPC_PF_AI_SetSampleRate()
{
    return 0;
}

AR_S32 IPC_PF_AI_SetSampleBitWidth()
{
    return 0;
}


static AUDIO_SOUND_MODE_E IPC_PF_Audio_GetSoundMode(AR_U32 u32SoundMode)
{
    if(u32SoundMode == 1)
    {
        return AUDIO_SOUND_MODE_MONO;
    }
    if(u32SoundMode == 2)
    {
        return AUDIO_SOUND_MODE_STEREO;
    }
    
    return AUDIO_SOUND_MODE_STEREO;
}

AR_S32 IPC_PF_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, IPC_AUDIO_FRAME_S *pstFrm, IPC_AEC_FRAME_S *pstAecFrm)
{
    AR_S32 s32Ret = 0;
    AUDIO_FRAME_S pAiGetChnFrm = {0};
    AR_S32 aiFd = 0;
    
    if ((pstFrm == NULL)||(pstFrm->pu8AudioData[0] == NULL)) 
    {
        PRINT_ERR("[%s][%d]--->>>audio frame pointer args null\n", __func__, __LINE__);
        return -1;
    }
    
    if((pstFrm->enSoundmode == AUDIO_SOUND_MODE_STEREO) && (pstFrm->pu8AudioData[1] == NULL))
    {
        PRINT_ERR("[%s][%d]--->>>stereo mode audio frame pointer args null\n", __func__, __LINE__);
        return -1;
    }
    
    aiFd = AR_MPI_AI_GetFd(AiDevId, AiChn);
    if(aiFd <= 0)
    {
        PRINT_ERR("[%s][%d]--->>>Ai get fd failed!!!ret:%#x\n", __func__, __LINE__, aiFd);
        return -1;
    }

    s32Ret = AR_MPI_AI_GetFrame(AiDevId, AiChn , &pAiGetChnFrm, NULL, -1);
    if(s32Ret == AR_SUCCESS)
    {
        if (pstFrm->u32PerFrameBuffLen < pAiGetChnFrm.u32Len) 
        {
            PRINT_ERR("[%s][%d]--->>>buf too small:buf len=%u, data len=%u\n", __func__, __LINE__, pstFrm->u32PerFrameBuffLen, pAiGetChnFrm.u32Len);
            goto Exit;
        } 
        else 
        {
            memcpy(pstFrm->pu8AudioData[0], pAiGetChnFrm.u64VirAddr[0], pAiGetChnFrm.u32Len);
            
            if(pstFrm->enSoundmode == AUDIO_SOUND_MODE_STEREO)
            {	
                memcpy(pstFrm->pu8AudioData[1], pAiGetChnFrm.u64VirAddr[1], pAiGetChnFrm.u32Len);
            }
            pstFrm->u32PerFrameLen = pAiGetChnFrm.u32Len;
        }
    }
    else
    {
        PRINT_ERR("[%s][%d]--->>> Dev%dChn%d get frame failed!!!error:0x%x\n", __func__, __LINE__, AiDevId, AiChn, s32Ret);
        return -1;
    }

Exit:
    s32Ret = AR_MPI_AI_ReleaseFrame(AiDevId, AiChn, &pAiGetChnFrm, NULL);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>> AI Dev%d Chn%d release frame failed!!!error:%#x", __func__, __LINE__, AiDevId, AiChn, s32Ret);
        return -1;
    }
    
    return 0;
}

AR_S32 IPC_PF_TP9930AudioClkSelcet(AUDIO_SAMPLE_RATE_E enSampleRate)
{
    AR_S32 s32Ret = 0;
    ENUM_CLK_AUDIO i2sSysRate = CLK_AUDIO_RATE_8000_64000;
    
    switch(enSampleRate)
    {
         case AUDIO_SAMPLE_RATE_96000:
         case AUDIO_SAMPLE_RATE_48000:
         case AUDIO_SAMPLE_RATE_24000:
         case AUDIO_SAMPLE_RATE_12000:
             i2sSysRate = CLK_AUDIO_RATE_12000_96000;
             break;
         case AUDIO_SAMPLE_RATE_44100:
         case AUDIO_SAMPLE_RATE_22050:
         case AUDIO_SAMPLE_RATE_11025:
             i2sSysRate = CLK_AUDIO_RATE_11025_44100;
             break;
         case AUDIO_SAMPLE_RATE_64000:
         case AUDIO_SAMPLE_RATE_32000:
         case AUDIO_SAMPLE_RATE_16000:
         case AUDIO_SAMPLE_RATE_8000:
             i2sSysRate = CLK_AUDIO_RATE_8000_64000;
             break;
         default:
             printf("audio pll can not detect!\n");
             return -1;
    }
    
    s32Ret = ar_hal_audio_clk_select(i2sSysRate);
    if(s32Ret)
    {
        PRINT_ERR("audio pll set failed!\n");
        return -1;
    }
    
    return 0;
}

TP9930_SAMPLE_RATE_E IPC_PF_TP9930AudioSampleRate(AR_U32 u32SampleRate)
{
    if(u32SampleRate == 8000)
    {
        return TP9930_SAMPLE_RATE_8K;
    }
    if(u32SampleRate == 16000)
    {
        return TP9930_SAMPLE_RATE_16K;
    }
    
    return TP9930_SAMPLE_RATE_8K;
}

TP9930_BITS_E IPC_PF_TP9930AudioBitWidth(AR_U32 u32BitWidth)
{
    if(u32BitWidth == 8)
    {
        return TP9930_BITS_8;
    }
    if(u32BitWidth == 16)
    {
        return TP9930_BITS_16;
    }
    
    return TP9930_BITS_16;
}


AR_S32 IPC_PF_AI_DevAttachChn(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AI_CHN AiChn = 0; 
    AI_CHN_PARAM_S pstChnParam = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    s32Ret = AR_MPI_AI_SetPubAttr(AiDevId, pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev (%d) setAttr failed!ret:%#x\n", __func__, __LINE__, AiDevId, s32Ret);
        return -1;
    }
    
    s32Ret = AR_MPI_AI_Enable(AiDevId);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev (%d) Enable failed!ret:%#x\n", __func__, __LINE__, AiDevId, s32Ret);
        return -1;
    }

    for(AiChn = 0; AiChn < (pstAttr->u32ChnCnt>>pstAttr->enSoundmode); AiChn++)
    {
        s32Ret = AR_MPI_AI_EnableChn(AiDevId, AiChn);
        if (s32Ret)
        {
            PRINT_ERR("[%s][%d]--->>>AR_MPI_AI_EnableChn(%d, %d) failed with %#x\n", __func__, __LINE__, AiDevId, AiChn, s32Ret);
            return -1;
        }
        
        //no need set for binder mode
        if(pstAudioCfg->mode == 1)
        {
            pstChnParam.u32UsrFrmDepth = 6;
            s32Ret = AR_MPI_AI_SetChnParam(AiDevId, AiChn, &pstChnParam);
            if(s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("[%s][%d]--->>>AI Dev (%d) Chn (%d) setParam failed!ret:%#x\n",__func__,__LINE__, AiDevId, AiChn, s32Ret);
                return -1;
            }
        }
    }

	return 0;
}

AR_S32 IPC_PF_AI_DevDetachChn(AUDIO_DEV AiDevId)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AI_CHN AiChn = 0; 
    AIO_ATTR_S pstAttr = {0};
    
    s32Ret = AR_MPI_AI_GetPubAttr(AiDevId, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    for(AiChn = 0; AiChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AiChn++)
    {
        s32Ret = AR_MPI_AI_DisableChn(AiDevId, AiChn);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AI Chn disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }

    s32Ret = AR_MPI_AI_Disable(AiDevId);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }

	return 0;
}

AR_S32 IPC_PF_AI_Init()
{
    AR_S32 s32Ret = AR_SUCCESS;
    AUDIO_DEV AiDevId = 0;
    AIO_ATTR_S pstAttr = {0};
    TP9930_SAMPLE_RATE_E eSampleRateTp9930 = TP9930_SAMPLE_RATE_16K;
    TP9930_BITS_E eBitWidthTp9930 = TP9930_BITS_16;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    memset(&pstAttr, 0x0, sizeof(AIO_ATTR_S));
    
    //inner dev 0, for tp9930 i2s set to dev 1 & dev 3 with pin_cfg i2s0 & i2s2
    if(!strcmp(pstAudioCfg->i2sType, "tp9930"))
    {
        if(pstAudioCfg->sampleBitWidth == 8)
        {
            PRINT_ERR("[%s][%d]--->>> not support 8 bit BitWidth\n", __func__, __LINE__);
            return -1;
        }
		
        //for tp9930 with 4 channel, I2S SampleRate shouled be 4 times of tp9930
        //tp9930 every device with 4 channel, I2S with stereo mode every channel bitWidth is 2 times of tp9930
        pstAttr.enSamplerate   = IPC_PF_Audio_GetSampleRate(4 * pstAudioCfg->sampleRate);			
        pstAttr.enBitwidth     = IPC_PF_Audio_GetBitWidth(2 * pstAudioCfg->sampleBitWidth);
        pstAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
        pstAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
        pstAttr.u32ChnCnt 	   = 2;
        pstAttr.u32FrmNum      = 8;
        pstAttr.u32PtNumPerFrm = 4096 / pstAttr.u32ChnCnt / (pstAttr.enBitwidth + 1);
        pstAttr.u32EXFlag      = 0;
        pstAttr.u32ClkSel      = 0;
        pstAttr.enI2sType      = AIO_I2STYPE_EXTERN;
        AiDevId                = 1; //slave mode dev id 4/5, master mode 1/2/3
        
        s32Ret = IPC_PF_TP9930AudioClkSelcet(4 * pstAudioCfg->sampleRate);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>IPC_PF_TP9930AudioClkSelcet failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }

        eSampleRateTp9930 = IPC_PF_TP9930AudioSampleRate(pstAudioCfg->sampleRate);
        eBitWidthTp9930 = IPC_PF_TP9930AudioBitWidth(pstAudioCfg->sampleBitWidth);

		printf("eSampleRateTp9930 = %d, eBitWidthTp9930 = %d\n", eSampleRateTp9930, eBitWidthTp9930);
		
        //for tp9930, init ai1,ai2 with left channel; ai3,ai4 with right channel;
        //for tp9930, init with slave mode, 16bit bitwidth, 16k SampleRate
        s32Ret = TP9930_AudioCfg4Chs(3, 0x8A, eSampleRateTp9930, eBitWidthTp9930, TP9930_I2S_MODE_SLAVE);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930_AudioCfg4Chs failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        s32Ret = IPC_PF_AI_DevAttachChn(AiDevId, &pstAttr);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930_AudioCfg4Chs failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        //init tp9930 ai5-ai8 chn
        AiDevId = 3;
        s32Ret = TP9930_AudioCfg4Chs(3, 0x88, eSampleRateTp9930, eBitWidthTp9930, TP9930_I2S_MODE_SLAVE);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930_AudioCfg4Chs failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        s32Ret = IPC_PF_AI_DevAttachChn(AiDevId, &pstAttr);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930_AudioCfg4Chs failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }
    else
    {
        pstAttr.enSamplerate   = IPC_PF_Audio_GetSampleRate(pstAudioCfg->sampleRate);			
        pstAttr.enBitwidth     = IPC_PF_Audio_GetBitWidth(pstAudioCfg->sampleBitWidth);
        pstAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
        pstAttr.enSoundmode    = IPC_PF_Audio_GetSoundMode(pstAudioCfg->chans);
        pstAttr.u32ChnCnt      = (pstAttr.enSoundmode == AUDIO_SOUND_MODE_STEREO)?2:1;
        pstAttr.u32FrmNum      = 30;
        pstAttr.u32PtNumPerFrm = 1024;
        pstAttr.u32EXFlag      = 0;
        pstAttr.u32ClkSel      = 0;
        pstAttr.enI2sType      = AIO_I2STYPE_INNERCODEC; 
        AiDevId 			   = 0;
        
        s32Ret = IPC_PF_AI_DevAttachChn(AiDevId, &pstAttr);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930_AudioCfg4Chs failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }
  
    return 0;
}

AR_S32 IPC_PF_AI_UnInit()
{
    AR_S32 s32Ret = AR_SUCCESS;
    AUDIO_DEV AiDevId = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(!strcmp(pstAudioCfg->i2sType, "tp9930"))
    {
        AiDevId = 1;
        s32Ret = IPC_PF_AI_DevDetachChn(AiDevId);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930 IPC_PF_AI_DevDetachChn failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        AiDevId = 3;
        s32Ret = IPC_PF_AI_DevDetachChn(AiDevId);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>TP9930 IPC_PF_AI_DevDetachChn failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }
    else
    {
        s32Ret = IPC_PF_AI_DevDetachChn(AiDevId);
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>IPC_PF_AI_DevDetachChn failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }
    
    return 0;
}

AR_S32 IPC_PF_AO_DevAttachChn(AUDIO_DEV AoDevId, AIO_ATTR_S *pstAttr)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AI_CHN AoChn = 0; 
    
    s32Ret = AR_MPI_AO_SetPubAttr(AoDevId, pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AO Dev (%d) setAttr failed!ret:%#x\n", __func__, __LINE__, AoDevId, s32Ret);
        return -1;
    }
    
    s32Ret = AR_MPI_AO_Enable(AoDevId);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AO Dev (%d) Enable failed!ret:%#x\n", __func__, __LINE__, AoDevId, s32Ret);
        return -1;
    }
    
    for(AoChn = 0; AoChn < (pstAttr->u32ChnCnt>>pstAttr->enSoundmode); AoChn++)
    {
        s32Ret = AR_MPI_AO_EnableChn(AoDevId, AoChn);
        if (s32Ret)
        {
            PRINT_ERR("[%s][%d]--->>>AR_MPI_AO_EnableChn(%d, %d) failed with %#x\n", __func__, __LINE__, AoDevId, AoChn, s32Ret);
            return -1;
        }
    }
    
    return 0;
}

AR_S32 IPC_PF_AO_DevDetachChn(AUDIO_DEV AoDevId)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AI_CHN AoChn = 0; 
    AIO_ATTR_S pstAttr = {0};
    
    s32Ret = AR_MPI_AO_GetPubAttr(AoDevId, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AO Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    for(AoChn = 0; AoChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AoChn++)
    {
        s32Ret = AR_MPI_AO_DisableChn(AoDevId, AoChn);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AO Chn disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }
    
    s32Ret = AR_MPI_AO_Disable(AoDevId);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AO Dev disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    return 0;
}

//both evb and ext board use inner ao 
AR_S32 IPC_PF_AO_Init()
{
    AR_S32 s32Ret = AR_SUCCESS;
    AUDIO_DEV AoDevId = 0;
    AIO_ATTR_S pstAttr = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    memset(&pstAttr,0x0,sizeof(AIO_ATTR_S));
    
    pstAttr.enSamplerate   = IPC_PF_Audio_GetSampleRate(pstAudioCfg->sampleRate);			
    pstAttr.enBitwidth     = IPC_PF_Audio_GetBitWidth(pstAudioCfg->sampleBitWidth);
    pstAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    pstAttr.u32EXFlag      = 0;
    pstAttr.u32ClkSel      = 0;
    pstAttr.u32FrmNum      = 30;
    pstAttr.u32PtNumPerFrm = 1024;
    pstAttr.enI2sType      = AIO_I2STYPE_INNERCODEC; 
    AoDevId                = 0;
    
    if(!strcmp(pstAudioCfg->i2sType, "tp9930"))
    {
        pstAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
        pstAttr.u32ChnCnt = 1;
    }
    else
    {
        pstAttr.enSoundmode = IPC_PF_Audio_GetSoundMode(pstAudioCfg->chans);
        pstAttr.u32ChnCnt = (pstAttr.enSoundmode == AUDIO_SOUND_MODE_STEREO)?2:1;
    }
	
    s32Ret = IPC_PF_AO_DevAttachChn(AoDevId, &pstAttr);
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_AO_DevAttachChn failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
	
    return 0;
}

AR_S32 IPC_PF_AO_UnInit()
{
    AR_S32 s32Ret = AR_SUCCESS;
    AUDIO_DEV AoDevId = 0;
  
    s32Ret = IPC_PF_AO_DevDetachChn(AoDevId);
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_AI_DevDetachChn failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    return 0;
}

/******************************* Encoder ****************************************/
/******************************************************************************
* function : Start Aenc
******************************************************************************/
static PAYLOAD_TYPE_E IPC_PF_AENC_GetType(AR_S32 s32CodecType)
{
    if(s32CodecType == 0)
    {
        return PT_G711A;
    }
    else if(s32CodecType == 1)
    {
        return PT_G711U;
    }
    
    return PT_G711A;
}

/******************************************************************************
* function : int Aenc
******************************************************************************/
AR_S32 IPC_PF_AENC_Init()
{
    AR_S32 s32Ret = 0;
    AR_S32 s32AencNum = 0;
    AR_S32 s32AencChn = 0;
    AENC_CHN_ATTR_S stAencAttr = {0};
    AENC_ATTR_G711_S stAencG711 = {0};
    AENC_ATTR_LPCM_S stAencLpcm = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    /* set AENC chn attr */
    stAencAttr.enType = IPC_PF_AENC_GetType(pstAudioCfg->type); 
    stAencAttr.u32BufSize = 30;  
    stAencAttr.u32PtNumPerFrm = 1024; 
    
    if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
    {
        stAencG711.enBitWidth = IPC_PF_Audio_GetBitWidth(pstAudioCfg->sampleBitWidth);
        stAencG711.enSoundMode = IPC_PF_Audio_GetSoundMode(pstAudioCfg->chans);
        stAencAttr.pValue       = &stAencG711;
    }
    else if (PT_LPCM == stAencAttr.enType)
    {
        stAencAttr.pValue = &stAencLpcm;
    }
    else
    {
        PRINT_ERR("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
        return -1;
    }

    /* create aenc chn*/
    s32AencNum = strcmp(pstAudioCfg->i2sType, "tp9930")?1:8;
    for(s32AencChn = 0; s32AencChn < s32AencNum; s32AencChn++)
    {
        s32Ret = AR_MPI_AENC_CreateChn(s32AencChn, &stAencAttr);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("%s: AR_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, s32AencChn, s32Ret);
            return -1;
        }
    }
    
    return 0;
}

/******************************************************************************
* function : uint Aenc
******************************************************************************/
AR_S32 IPC_PF_AENC_UnInit()
{
    AR_S32 s32Ret;
    AR_S32 s32AencNum = 0;
    AR_S32 s32AencChn = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    s32AencNum = strcmp(pstAudioCfg->i2sType, "tp9930")?1:8;
    for(s32AencChn = 0; s32AencChn < s32AencNum; s32AencChn++)
    {
        s32Ret = AR_MPI_AENC_DestroyChn(s32AencChn);
        if (AR_SUCCESS != s32Ret)
        {
            PRINT_ERR("%s: AR_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__, s32AencChn, s32Ret);
            return -1;
        }
    }
    
    return 0;
}

AR_S32 IPC_PF_AiBindAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn)
{
    MPP_CHN_S stSrcChn = {0};
    MPP_CHN_S stDestChn = {0};
    
    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AENC;
    stDestChn.s32DevId = AencDev;
    stDestChn.s32ChnId = AencChn;
    
    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 IPC_PF_AiUnBindAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn)
{
    MPP_CHN_S stSrcChn = {0}; 
    MPP_CHN_S stDestChn = {0};
    
    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AENC;
    stDestChn.s32DevId = AencDev;
    stDestChn.s32ChnId = AencChn;
    
    return AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

AR_S32 IPC_PF_AiBindAo(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AoDev, AR_S32 AoChn)
{
    MPP_CHN_S stSrcChn = {0}; 
    MPP_CHN_S stDestChn = {0};
    
    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;
    
    return AR_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

AR_S32 IPC_PF_AiUnBindAo(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AoDev, AR_S32 AoChn)
{
    MPP_CHN_S stSrcChn = {0};
    MPP_CHN_S stDestChn = {0};
    
    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;
    
    return AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/************ static functions ***********************/
static void* IPC_PF_THREAD_ReadAStream(void *args)
{
    AR_S32 s32Ret = 0;
    AR_U32 u32Id = 0;
    AR_CHAR achName[32] = {0};
    AENC_CHN stVencCh = 0;
    IPC_FRAME_HEADER_S	stFrameHeader = {0};
    AUDIO_STREAM_S stStream = {0};
    AR_S32 i = 0, j = 0;
    
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    sprintf(achName, "read_a%d", u32Id);
    prctl(PR_SET_NAME, achName, 0, 0, 0);

    while(1)
    {
        if(s_u32AThreadStop)
        {
            PRINT_INFO("IPC_PF_THREAD_ReadAStream exit\n");
            break;
        }
        
        //get audio stream
        s32Ret = AR_MPI_AENC_GetStream(stVencCh, &stStream, 2000);
        if (AR_SUCCESS != s32Ret )
        {
            PRINT_INFO("%s: AR_MPI_AENC_GetStream, timeout %dms with ret:%#x!\n", __FUNCTION__, 2, s32Ret);
            continue;
        }

        stFrameHeader.magic = 0XF3B0A4B8;
        stFrameHeader.device_type = 0;
        stFrameHeader.frame_size = stStream.u32Len;
        stFrameHeader.pts = stStream.u64TimeStamp;
        stFrameHeader.sec = 0;
        stFrameHeader.usec = stStream.u64TimeStamp;
        stFrameHeader.frame_rate = pstAudioCfg->sampleRate/1000;
        stFrameHeader.frame_type = IPC_FRAME_TYPE_A;
        stFrameHeader.frame_no = stStream.u32Seq;

        for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
        {
            for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
            {
                u32Id = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
                if(s_u32AThreadStop)
                {
                    break;
                }
                if(pstVencCfg->vencStream[i][j].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)stStream.pStream, stStream.u32Len, &stFrameHeader);
                }
            }
        }
        
        AR_MPI_AENC_ReleaseStream(stVencCh, &stStream);
    
    }
    
    return NULL;
}

//By default, we record the audio to video channel 0 main stream (IPC)
void IPC_PF_AENC_StartMbufThread(void)
{
    ProfileStart(__FUNCTION__);
    
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(pstAudioCfg->mode == 3)
    {
        s_u32AThreadStop = 0;
        IPC_MID_UTIL_CreateDetachThread(IPC_PF_THREAD_ReadAStream, NULL, NULL);
    }
    
    ProfileEnd(__FUNCTION__);
}

void IPC_PF_AENC_StopMbufThread(void)
{
    s_u32AThreadStop = 1;
}

/************ static functions ***********************/
static void* IPC_PF_THREAD_ReadAIStream(void *args)
{
    AR_S32 s32ChnId = *(AR_S32 *)args;
    IPC_FRAME_HEADER_S	stFrameHeader = {0};
    AR_S32 i = 0, j = 0;
    AR_S32  u32Id =0;
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    
    AR_S32 s32Ret  = 0;
    AUDIO_DEV AiDevId = 0;
    AI_CHN AiChn = 0;
    IPC_AUDIO_FRAME_S stFrm = {0};
    AUDIO_SOUND_MODE_E eSoundMode = AUDIO_SOUND_MODE_STEREO;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    eSoundMode = IPC_PF_Audio_GetSoundMode(pstAudioCfg->chans); 
    
    AR_U8 * pu8AudioStereoData =NULL;
    AR_U32  u32AudioBuffLen =0;
    
    stFrm.u32PerFrameBuffLen = 2048;
    stFrm.pu8AudioData[0] = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen);
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        stFrm.enSoundmode = AUDIO_SOUND_MODE_STEREO;
        stFrm.pu8AudioData[1] = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen);
        pu8AudioStereoData = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen*2);
    }
    
    PRINT_INFO("start AI Read thread\n");
    
    while(1)
    {
        if(s_u32AIThreadStatus[s32ChnId].u32Stop)
        {
            PRINT_INFO("IPC_PF_THREAD_ReadAIStream exit\n");
            break;
        }

        if(g_u32AThreadSuspend)
        {
        	usleep(500*1000);
			continue;
        }
        
        s32Ret = IPC_PF_AI_GetFrame(AiDevId, AiChn , &stFrm, NULL);
        if (AR_SUCCESS != s32Ret )
        {
            PRINT_INFO("%s: IPC_PF_AI_GetFrame, with ret:%#x!\n", __FUNCTION__,s32Ret);
            continue;
        }
        
        if(eSoundMode == AUDIO_SOUND_MODE_MONO)
        {
            pu8AudioStereoData = (AR_U8 *)stFrm.pu8AudioData[0];
            u32AudioBuffLen = stFrm.u32PerFrameLen;
        }
        else
        {
            memcpy(pu8AudioStereoData,stFrm.pu8AudioData[0],stFrm.u32PerFrameLen);
            memcpy(pu8AudioStereoData+stFrm.u32PerFrameLen,stFrm.pu8AudioData[1],stFrm.u32PerFrameLen);
            u32AudioBuffLen = stFrm.u32PerFrameLen*2;
        }

        stFrameHeader.magic = 0XF3B0A4B8;
        stFrameHeader.device_type = eSoundMode;
        stFrameHeader.frame_size = stFrm.u32PerFrameLen;
        stFrameHeader.pts = 0;
        stFrameHeader.sec = 0;
        stFrameHeader.usec = 0;
        stFrameHeader.frame_rate = pstAudioCfg->sampleRate/1000;
        stFrameHeader.frame_type = IPC_FRAME_TYPE_A;
        stFrameHeader.frame_no = 0;

        for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
        {
            for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
            {
                u32Id = i * IPC_MAX_VENC_STREAM_PER_CHN + j;
                if(s_u32AIThreadStatus[s32ChnId].u32Stop)
                {
                    break;
                }
                if(pstVencCfg->vencStream[i][j].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pu8AudioStereoData, u32AudioBuffLen, &stFrameHeader);
                }
            }
        }

    }
    
    free(stFrm.pu8AudioData[0]);
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        free(stFrm.pu8AudioData[1]);
        free(pu8AudioStereoData);
    }
    
    return NULL;
}

static void* IPC_PF_THREAD_Tp9930ReadAIStream(void *args)
{
    AR_S32 s32ChnId = *(AR_S32 *)args;
    AR_S32 s32Ret  = 0;
    AUDIO_DEV AiDevId = 1;
    AI_CHN AiChn = 0;
    AR_S32 i = 0;
    AR_S32 u32Id =0;
    IPC_AUDIO_FRAME_S stFrm = {0};
    AUDIO_SOUND_MODE_E eSoundMode = AUDIO_SOUND_MODE_STEREO;
    IPC_FRAME_HEADER_S	stFrameHeader = {0};
    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    AR_U8 * pu8AudioStereoData =NULL;
    AR_U8 * pcAiChn0 = NULL;
    AR_U8 * pcAiChn1 = NULL;
    AR_U8 * pcAiChn2 = NULL;
    AR_U8 * pcAiChn3 = NULL;

    if(pstAudioCfg->sampleBitWidth == 8)
    {
        PRINT_ERR("tp9930 current not support 8bit sample bitwidth\n");
	    return NULL;
    }

    //master 1/3, slave 4/5
    if(s32ChnId == 0)
    {
        AiDevId = 1; 
    }
    else
    {
        AiDevId = 3;
    }
    
    stFrm.u32PerFrameBuffLen = 2048;
    stFrm.pu8AudioData[0] = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen);
    if(!stFrm.pu8AudioData[0])
    {
        PRINT_ERR("===> [%s,%d] malloc failed\n", __func__, __LINE__);
        return NULL;
    }
    
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        stFrm.enSoundmode = AUDIO_SOUND_MODE_STEREO;
        stFrm.pu8AudioData[1] = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen);
        if(!stFrm.pu8AudioData[1])
        {
            PRINT_ERR("===> [%s,%d] malloc failed\n", __func__, __LINE__);
            free(stFrm.pu8AudioData[0]);
            return NULL;
        }
    }

    pu8AudioStereoData = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen*2);
    if(!pu8AudioStereoData)
    {
        PRINT_ERR("===> [%s,%d] malloc failed\n", __func__, __LINE__);
        free(stFrm.pu8AudioData[0]);
        free(stFrm.pu8AudioData[1]);
        return NULL;
    }

    pcAiChn0 = pu8AudioStereoData;
    pcAiChn1 = pu8AudioStereoData + stFrm.u32PerFrameBuffLen/4;
    pcAiChn2 = pu8AudioStereoData + stFrm.u32PerFrameBuffLen/2;
    pcAiChn3 = pu8AudioStereoData + stFrm.u32PerFrameBuffLen*3/4;
    
    PRINT_INFO("start AI Read thread, Pipe : %d, Ai_Dev : %d, Ai_Chn : %d\n", s32ChnId, AiDevId, AiChn);

#if 0	
    FILE *s32Fd = NULL;
    char name[32] = {0};
    sprintf(name, "audio_%d_%d_%d_l_0.txt", s32ChnId, AiDevId, AiChn);
    s32Fd = fopen(name, "wb+");
    
    FILE *s32Fd1 = NULL;
    char name1[32] = {0};
    sprintf(name1, "audio_%d_%d_%d_l_1.txt", s32ChnId, AiDevId, AiChn);
    s32Fd1 = fopen(name1, "wb+");
    
    FILE *s32Fd2 = NULL;
    char name2[32] = {0};
    sprintf(name2, "audio_%d_%d_%d_r_0.txt", s32ChnId, AiDevId, AiChn);
    s32Fd2 = fopen(name2, "wb+");
    
    FILE *s32Fd3 = NULL;
    char name3[32] = {0};
    sprintf(name3, "audio_%d_%d_%d_r_1.txt", s32ChnId, AiDevId, AiChn);
    s32Fd3 = fopen(name3, "wb+");
    
    AR_S32 total = 0;
#endif	
    //for tp9930, every WS will send data form start until all 16 chn finshed 
    AR_U32 u32Stride = 0;
    AR_U32 u32ChnPerByte = pstAudioCfg->sampleBitWidth/8;
    u32Stride = (pstAudioCfg->sampleBitWidth/8) * 2 * 4;   // stride = bitWidth/8 * left_chn * total_chn
    
    while(1)
    {
        if(s_u32AIThreadStatus[s32ChnId].u32Stop)
        {
            PRINT_INFO("IPC_PF_THREAD_ReadAIStream exit\n");
            break;
        }

		if(g_u32AThreadSuspend)
        {
        	usleep(500*1000);
			continue;
        }
        
        s32Ret = IPC_PF_AI_GetFrame(AiDevId, AiChn , &stFrm, NULL);
        if (AR_SUCCESS != s32Ret )
        {
            PRINT_INFO("%s: IPC_PF_AI_GetFrame, ai_dev(%d), ai_chn(%d) with ret:%#x!\n", __func__, AiDevId, AiChn, s32Ret);
            continue;
        }

        #if 1
        for(i = 0; i < stFrm.u32PerFrameLen/u32Stride; i++)
        {
            memcpy(pcAiChn0 + i * u32ChnPerByte, stFrm.pu8AudioData[0] + i * u32Stride, u32ChnPerByte);
            memcpy(pcAiChn1 + i * u32ChnPerByte, stFrm.pu8AudioData[0] + u32ChnPerByte + i * u32Stride, u32ChnPerByte);
            memcpy(pcAiChn2 + i * u32ChnPerByte, stFrm.pu8AudioData[1] + i * u32Stride, u32ChnPerByte);
            memcpy(pcAiChn3 + i * u32ChnPerByte, stFrm.pu8AudioData[1] + u32ChnPerByte + i * u32Stride, u32ChnPerByte);
        }
        #else
        //debug for 8 bit bitwidth, rtsp with g711 transfer need 16bit
        for(i = 0; i < stFrm.u32PerFrameLen/16; i++)
        {
            memcpy(pcAiChn0 + i * u32ChnPerByte, stFrm.pu8AudioData[0] + i * 16, u32ChnPerByte);
            memcpy(pcAiChn1 + i * u32ChnPerByte, stFrm.pu8AudioData[0] + 2 + i * 16, u32ChnPerByte);
            memcpy(pcAiChn2 + i * u32ChnPerByte, stFrm.pu8AudioData[1] + i * 16, u32ChnPerByte);
            memcpy(pcAiChn3 + i * u32ChnPerByte, stFrm.pu8AudioData[1] + 2 + i * 16, u32ChnPerByte);
        }
        #endif
        
        #if 0
        total += stFrm.u32PerFrameLen/4;
        
        if(total < 1024 * 1024 * 4)
        {
            #if 1
            fwrite(pcAiChn0, 1, stFrm.u32PerFrameLen/8, s32Fd);
            fflush(s32Fd);
            fwrite(pcAiChn1, 1, stFrm.u32PerFrameLen/8, s32Fd1);
            fflush(s32Fd1);
            fwrite(pcAiChn2, 1, stFrm.u32PerFrameLen/8, s32Fd2);
            fflush(s32Fd2);
            fwrite(pcAiChn3, 1, stFrm.u32PerFrameLen/8, s32Fd3);
            fflush(s32Fd3);
            #else
            fwrite(pcAiChn0, 1, stFrm.u32PerFrameLen/16, s32Fd);
            fflush(s32Fd);
            fwrite(pcAiChn1, 1, stFrm.u32PerFrameLen/16, s32Fd1);
            fflush(s32Fd1);
            fwrite(pcAiChn2, 1, stFrm.u32PerFrameLen/16, s32Fd2);
            fflush(s32Fd2);
            fwrite(pcAiChn3, 1, stFrm.u32PerFrameLen/16, s32Fd3);
            fflush(s32Fd3);
            #endif
        }
        else
        {
            if(s32Fd && s32Fd1 && s32Fd2 && s32Fd3)
            {
                printf("write finished\n");
                fclose(s32Fd);
                s32Fd = NULL;
                fclose(s32Fd1);
                s32Fd1 = NULL;
                fclose(s32Fd2);
                s32Fd2 = NULL;
                fclose(s32Fd3);
                s32Fd3 = NULL;
            }
        }
        #endif

        stFrameHeader.magic = 0XF3B0A4B8;
        stFrameHeader.device_type = 0;
        stFrameHeader.frame_size = stFrm.u32PerFrameLen/8;
        stFrameHeader.pts = 0;
        stFrameHeader.sec = 0;
        stFrameHeader.usec = 0;
        stFrameHeader.frame_rate = pstAudioCfg->sampleRate/1000;
        stFrameHeader.frame_type = IPC_FRAME_TYPE_A;
        stFrameHeader.frame_no = 0;

        if(s32ChnId == 0)
        {
            for(i = 0; i < IPC_MAX_VENC_STREAM_PER_CHN; i++)
            {
                if(s_u32AIThreadStatus[s32ChnId].u32Stop)
                {
                    break;
                }
                
                u32Id = i;
                if(pstVencCfg->vencStream[0][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn0, stFrameHeader.frame_size, &stFrameHeader);
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN + i;
                if(pstVencCfg->vencStream[1][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn1, stFrameHeader.frame_size, &stFrameHeader);
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN * 2 + i;
                if(pstVencCfg->vencStream[2][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn2, stFrameHeader.frame_size, &stFrameHeader);
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN * 3 + i;
                if(pstVencCfg->vencStream[3][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn3, stFrameHeader.frame_size, &stFrameHeader);
                }
            }
        }
        else
        {
            for(i = 0; i < IPC_MAX_VENC_STREAM_PER_CHN; i++)
            {
                if(s_u32AIThreadStatus[s32ChnId].u32Stop)
                {
                    break;
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN * 4 + i;
                if(pstVencCfg->vencStream[4][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn0, stFrameHeader.frame_size, &stFrameHeader);
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN * 5 + i;
                if(pstVencCfg->vencStream[5][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn1, stFrameHeader.frame_size, &stFrameHeader);
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN * 6 + i;
                if(pstVencCfg->vencStream[6][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn2, stFrameHeader.frame_size, &stFrameHeader);
                }
                
                u32Id = IPC_MAX_VENC_STREAM_PER_CHN * 7 + i;
                if(pstVencCfg->vencStream[7][i].enable)
                {	
                    IPC_MID_MBUF_WriteFrame(IPC_PF_MBUF_GetAudioWriterId(u32Id), (void *)pcAiChn3, stFrameHeader.frame_size, &stFrameHeader);
                }
            }
        }
    }

    free(stFrm.pu8AudioData[0]);
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        free(stFrm.pu8AudioData[1]);
        free(pu8AudioStereoData);
    }
    
    return NULL;
}

//By default, we record the audio to video channel 0 main stream (IPC)
void IPC_PF_AI_StartMbufThread(void)
{
    ProfileStart(__FUNCTION__);
    AR_S32 i = 0;
    
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(pstAudioCfg->mode == 1)
    {
        if(strcmp(pstAudioCfg->i2sType, "tp9930"))
        {
            s_u32AIThreadStatus[i].u32Stop = 0;
            s_u32AIThreadStatus[i].u32Id = i;
            IPC_MID_UTIL_CreateDetachThread(IPC_PF_THREAD_ReadAIStream, (void *)&s_u32AIThreadStatus[i].u32Id, NULL);
        }
        else
        {
            for(i = 0; i < IPC_MAX_PIPELINE_NUM/4; i++)
            {
                s_u32AIThreadStatus[i].u32Stop = 0;
                s_u32AIThreadStatus[i].u32Id = i;
                IPC_MID_UTIL_CreateDetachThread(IPC_PF_THREAD_Tp9930ReadAIStream, (void *)&s_u32AIThreadStatus[i].u32Id, NULL);
            }
        }
    }
    
    ProfileEnd(__FUNCTION__);
}

void IPC_PF_AI_StopMbufThread(void)
{
    AR_S32 i = 0;
    
    for(i = 0; i < IPC_MAX_PIPELINE_NUM/4; i++)
    {
        s_u32AIThreadStatus[i].u32Stop = 1;
    }
}

AR_S32 IPC_PF_AI_SetVolume(AR_S32 s32AiInputVol)
{   
    AR_S32 s32Ret = 0, i = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    //for tp9930, ai volume range is 2 - 14 ((-6dB)-(6dB))
    if(!strcmp(pstAudioCfg->i2sType, "tp9930"))
    {
        for(i = 0; i < 4; i++)
        {
            s32Ret = TP9930_AdjustAinVolume(3, 0x8A, i, pstAudioCfg->inputVolume);
            if(s32Ret < 0)
            {
                PRINT_ERR("[%d][%s] TP9930_AdjustAinVolume failed, i2c_slave_addr(%x), chn(%d)!\n", __LINE__, __func__, 0x8A, i);
                return -1;
            }
            
            s32Ret = TP9930_AdjustAinVolume(3, 0x88, i, pstAudioCfg->inputVolume);
            if(s32Ret < 0)
            {
                PRINT_ERR("[%d][%s] TP9930_AdjustAinVolume failed, i2c_slave_addr(%x), chn(%d)!\n", __LINE__, __func__, 0x88, i);
                return -1;
            }
        }
    }
    else
    {
        AR_S32 fdAcodec = -1;
        fdAcodec = open(ACODEC_FILE, O_RDWR);
        if (fdAcodec < 0)
        {
            PRINT_ERR("[%d][%s]: can't open Acodec,%s\n", __LINE__, __func__, ACODEC_FILE);
            return -1;
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
        if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_INPUT_VOL, &s32AiInputVol, sizeof(s32AiInputVol)))
        {
            PRINT_ERR("[%d][%s]: set acodec micin volume failed\n", __LINE__, __func__);
            close(fdAcodec);
            return -1;
        }
        
        close(fdAcodec);
    }
	
    return 0;
}


AR_S32 IPC_PF_AO_SetVolume(AR_S32 s32AoOutputVol)
{	
    AR_S32 fdAcodec = -1; 
    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
       PRINT_ERR("[%d][%s]: can't open Acodec,%s\n", __LINE__, __func__, ACODEC_FILE);
       return -1;
    }
    
    /******************************************************************************************
    The output volume range is [-123, +14]. Both the analog gain and digital gain are adjusted.
    A larger value indicates higher volume.
    For example, the value 14 indicates the maximum volume of 14 dB,
    and the value -123 indicates the minimum volume (muted status).
    The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
    The recommended volume range is [-2, +8],step is 3db.
    Within this range, the noises are lowest because only the analog gain is adjusted,
    and the voice quality can be guaranteed.
    *******************************************************************************************/
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_OUTPUT_VOL, &s32AoOutputVol, sizeof(s32AoOutputVol)))
    {
        PRINT_ERR("[%d][%s]: set acodec micin volume failed\n", __LINE__, __func__);
        close(fdAcodec);
        return -1;
    }
    
    close(fdAcodec);
	
    return 0;
}

AR_S32 IPC_PF_Audio_CreateChannel(void)
{
    AR_S32 s32Ret = 0;
    AR_S32 AiDev = 0, AiChn = 0;
    AR_S32 AencDev = 0, AencChn = 0; 
    AR_S32 AoDev = 0, AoChn = 0;
    AIO_ATTR_S pstAttr = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    s32Ret = AR_MPI_AI_GetPubAttr(AiDev, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    if(pstAudioCfg->mode == 2)
    {
        for(AiChn = 0, AoChn = 0; AiChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AiChn++, AoChn++)
        {
            s32Ret = IPC_PF_AiBindAo(AiDev, AiChn, AoDev, AoChn);
            if(s32Ret)
            { 
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AiBindAo ai_dev(%d), ai_chn(%d), ao_dev(%d), ao_chn(%d) failed!!!error:%#x", 
                            __func__, __LINE__, AiDev, AiChn, AoDev, AoChn, s32Ret);
                return -1;
            }
        }
    }
    else if(pstAudioCfg->mode == 3)
    {
        for(AiChn = 0; AiChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AiChn++)
        {
            s32Ret = IPC_PF_AiBindAenc(AiDev, AiChn, AencDev, AencChn);
            if(s32Ret)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AiBindAenc failed!!!error:%#x",  __func__, __LINE__, s32Ret);
                return -1;
            }
            
            AencChn++;
        }
    }
    else
    {
        PRINT_INFO("No audio Device Need Binding!!!");
    }
    
    return 0;
}

AR_S32 IPC_PF_Audio_DestoryChannel(void)
{
    AR_S32 s32Ret = 0;
    AR_S32 AiDev = 0, AiChn = 0;
    AR_S32 AencDev = 0, AencChn = 0; 
    AR_S32 AoDev = 0, AoChn = 0;
	AIO_ATTR_S pstAttr = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();

    s32Ret = AR_MPI_AI_GetPubAttr(AiDev, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    if(pstAudioCfg->mode == 2)
    {
        for(AiChn = 0, AoChn = 0; AiChn < (pstAttr.u32ChnCnt >> pstAttr.enSoundmode); AiChn++, AoChn++)
        {
            s32Ret = IPC_PF_AiUnBindAo(AiDev, AiChn, AoDev, AoChn);
            if(s32Ret)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AiBindAo failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
    }
    else if(pstAudioCfg->mode == 3)
    {
        for(AiChn = 0; AiChn < (pstAttr.u32ChnCnt >> pstAttr.enSoundmode); AiChn++)
        {
            s32Ret = IPC_PF_AiUnBindAenc(AiDev, AiChn, AencDev, AencChn);
            if(s32Ret)
            {    
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AiBindAenc failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
    }
    else
    {
        PRINT_INFO("No audio Device been Bind!");
    }
    
    return 0;
}

AR_S32 IPC_PF_Audio_Init()
{
    AR_S32 s32Ret = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(!pstAudioCfg->mode)
    {
        PRINT_INFO("Audio is not enable!");
        return 0;
    }
    
    s32Ret = IPC_PF_AI_Init();
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_AI_Init failed!!!error:%#x", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    s32Ret = IPC_PF_AI_SetVolume(pstAudioCfg->inputVolume);
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_AI_SetVolume failed!!!error:%#x", __func__, __LINE__, s32Ret);
        return -1;
    }

    if(strcmp(pstAudioCfg->i2sType, "tp9930"))
    {
        if(pstAudioCfg->mode == 2)
        {
            s32Ret = IPC_PF_AO_Init();
            if(s32Ret < 0)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AO_Init failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
            
            s32Ret = IPC_PF_AO_SetVolume(pstAudioCfg->outputVolume);
            if(s32Ret < 0)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AO_SetVolume failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
        else if(pstAudioCfg->mode == 3)
        {
            s32Ret = IPC_PF_AENC_Init();
            if(s32Ret < 0)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AENC_Init failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
        
        s32Ret = IPC_PF_Audio_CreateChannel();
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>IPC_PF_Audio_CreateChannel failed!!!error:%#x", __func__, __LINE__, s32Ret);
            return -1;
        }
    }

    return 0;
}

AR_S32 IPC_PF_Audio_UnInit()
{
    AR_S32 s32Ret = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(!pstAudioCfg->mode)
    {
        PRINT_INFO("Audio is not enable!");
        return 0;
    }

    if(strcmp(pstAudioCfg->i2sType, "tp9930"))
    {
        s32Ret = IPC_PF_Audio_DestoryChannel();
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>IPC_PF_Audio_DestoryChannel failed!!!error:%#x", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        if(pstAudioCfg->mode == 2)
        {
            s32Ret = IPC_PF_AO_UnInit();
            if(s32Ret < 0)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AO_UnInit failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
        else if(pstAudioCfg->mode == 3)
        {
            s32Ret = IPC_PF_AENC_UnInit();
            if(s32Ret < 0)
            {
                PRINT_ERR("[%s][%d]--->>>IPC_PF_AENC_UnInit failed!!!error:%#x", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
    }

    s32Ret = IPC_PF_AI_UnInit();
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_AI_UnInit failed!!!error:%#x", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    return 0;
}

static AR_CHAR  pcAiOutputFileName[64]= {0};
static AR_CHAR  pcAiOutputFileName_L[64]= {0};
static AR_CHAR	pcAiOutputFileName_R[64]= {0};
IPC_AUDIO_THREAD_STATUS   stRecordExit = {0};
IPC_AUDIO_THREAD_STATUS   stPlaybackExit = {0};

void *IPC_PF_Audio_RecordThread(void *argv)
{
    //pthread_detach(pthread_self());
    
    AR_S32 s32Ret  = 0;
    AUDIO_DEV AiDevId = 0;
    AI_CHN AiChn = 0;
    IPC_AUDIO_FRAME_S stFrm = {0};
    AUDIO_SOUND_MODE_E eSoundMode = AUDIO_SOUND_MODE_STEREO;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    eSoundMode = IPC_PF_Audio_GetSoundMode(pstAudioCfg->chans);
    
    FILE *s32Fd = NULL, *s32Fd_L = NULL, *s32Fd_R = NULL;
    AR_S32 totallen = 0, totallen_l =0, totallen_r = 0;
    
    s32Fd = fopen(pcAiOutputFileName, "wb+");
    if(!s32Fd)
    {
        PRINT_ERR("open %s failed!\n", pcAiOutputFileName);
        return NULL;
    }
    else
    {
        fseek(s32Fd, 0, SEEK_SET);
    }
    
    stFrm.u32PerFrameBuffLen = 2048;
    stFrm.pu8AudioData[0] = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen);
    
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        s32Fd_L = fopen(pcAiOutputFileName_L, "wb+");
        if(!s32Fd_L)
        {
            PRINT_ERR("open %s failed!\n", pcAiOutputFileName_L);
            return NULL;
        }
        else
        {
            fseek(s32Fd_L, 0, SEEK_SET);
        }
        
        s32Fd_R = fopen(pcAiOutputFileName_R, "wb+");
        if(!s32Fd_R)
        {	
            PRINT_ERR("open %s failed!\n", pcAiOutputFileName_R);
            return NULL;
        }
        else
        {
            fseek(s32Fd_R, 0, SEEK_SET);
        }
		
        stFrm.enSoundmode = AUDIO_SOUND_MODE_STEREO;
        stFrm.pu8AudioData[1] = (AR_U8 *)malloc(stFrm.u32PerFrameBuffLen);
    }
	
	PRINT_INFO("==================start record thread=============\n");
		
    while(1)
    {
        if(stPlaybackExit.u32Stop)
        {
            PRINT_INFO("IPC_PF_AiRecordThread %d, exit\n", s32Ret);
            break;
        }
        
        s32Ret = IPC_PF_AI_GetFrame(AiDevId, AiChn , &stFrm, NULL);
        if(!s32Ret)
        {
            if(eSoundMode == AUDIO_SOUND_MODE_MONO)
            {
                if(s32Fd)
                {		
                    fwrite(stFrm.pu8AudioData[0], 1, stFrm.u32PerFrameLen, s32Fd);
                    fflush(s32Fd);					
                    totallen += stFrm.u32PerFrameLen;
                }
            }
            else
            {
                //need to save left+right data
                if(s32Fd)
                {				
                    //need to save left+right data
                    int write_offset = 0;
                    int perwritelen = 2;
                    while(write_offset < stFrm.u32PerFrameLen)
                    {
                        fwrite(stFrm.pu8AudioData[0] + write_offset, 1, perwritelen, s32Fd);
                        fwrite(stFrm.pu8AudioData[1] + write_offset, 1, perwritelen, s32Fd);
                        fflush(s32Fd);
                        write_offset += perwritelen;
                    }
                    totallen += stFrm.u32PerFrameLen*2;
                }
                    
                if(s32Fd_L)
                {
                    fwrite(stFrm.pu8AudioData[0], 1, stFrm.u32PerFrameLen, s32Fd_L);
                    fflush(s32Fd_L);
                    totallen_l += stFrm.u32PerFrameLen;
                }
                if(s32Fd_R)
                {
                    fwrite(stFrm.pu8AudioData[1], 1, stFrm.u32PerFrameLen, s32Fd_R);
                    fflush(s32Fd_R);
                    totallen_r += stFrm.u32PerFrameLen;
                }
            }
        }
        else
        {
            PRINT_ERR("Dev%dChn%d get frame failed!!!error:0x%x\n", AiDevId, AiChn, s32Ret);
        }
    }

    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
        PRINT_INFO("totallen:%d,totallen_l:%d,totallen_r:%d\n", totallen, totallen_l, totallen_r);
    else
        PRINT_INFO("totallen:%d\n", totallen);
    
    IPC_AUDIO_WAV_HEAD_S stWavHead;
    memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
    IPC_PF_WAV_HeaderInit(&stWavHead, totallen, pstAudioCfg->chans, pstAudioCfg->sampleRate, pstAudioCfg->sampleBitWidth);
    fseek(s32Fd, 0, SEEK_SET);
    fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd);
    fflush(s32Fd);

#if 1
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
        IPC_PF_WAV_HeaderInit(&stWavHead, totallen, 0x1, pstAudioCfg->sampleRate, pstAudioCfg->sampleBitWidth);
        fseek(s32Fd_L, 0, SEEK_SET);
        fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd_L);
        fflush(s32Fd_L);
        
        memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
        IPC_PF_WAV_HeaderInit(&stWavHead, totallen, 0x1, pstAudioCfg->sampleRate, pstAudioCfg->sampleBitWidth);
        fseek(s32Fd_L, 0, SEEK_SET);
        fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd_R);
        fflush(s32Fd_R);
    }
#endif

    if(s32Fd)
    {
        fclose(s32Fd);
    }
    
    free(stFrm.pu8AudioData[0]);
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        if(s32Fd_L)
        {
            fclose(s32Fd_L);
        }
        
        if(s32Fd_R)
        {
            fclose(s32Fd_R);
        }
        
        free(stFrm.pu8AudioData[1]);
    }
    
    PRINT_INFO("%s exit!!!\n", __func__);
    return NULL;
}

static void * IPC_PF_Audio_Tp9930RecordThreadFromMbuf(void *args)
{
    AR_U32 u32Id = *(AR_U32 *)args;
    AR_S32 s32Ret = 0;
    FILE *s32Fd = NULL;
    AR_S32 totallen = 0;
    
    AR_CHAR name[64] = {0};
    sprintf(name, "audio_rec_%d_%d", u32Id/IPC_MAX_VENC_STREAM_PER_CHN, u32Id % IPC_MAX_VENC_STREAM_PER_CHN);
    prctl(PR_SET_NAME, name, 0, 0, 0);
    
    s32Fd = fopen(pcAiOutputFileName, "wb+");
    if(!s32Fd)
    {
        PRINT_ERR("open %s failed!\n", pcAiOutputFileName);
        return NULL;
    }
    else
    {
        fseek(s32Fd, 0, SEEK_SET);
    }

    MEDIABUF_HANDLE hMbufReader = NULL;
    hMbufReader = IPC_MID_MBUF_AddReader(u32Id);
    if(hMbufReader == NULL)
    {
        PRINT_ERR("add_recoder_user error ");
        return NULL;
    }
    IPC_MID_MBUF_SetNewestFrame(hMbufReader);
    
    PRINT_INFO("start %s record!\n", pcAiOutputFileName);
    
    IPC_FRAME_HEADER_S header = {0};
    char *recv_buf = NULL;
    int data_length = 0;

    while (1)
    {
        if(stRecordExit.u32Stop)
        {
            PRINT_INFO("IPC_PF_AiRecordThread %d, exit\n", s32Ret);
            break;
        }
        
        recv_buf = NULL;
        s32Ret = IPC_MID_MBUF_ReadFrame(hMbufReader, (void **)&recv_buf, &data_length, &header);
        if (s32Ret < 0)
        {
            PRINT_ERR("stream%d : IPC_MID_MBUF_ReadFrame fail.\n", u32Id);
            break;
        }
        
        if (s32Ret == 0)
        {
            PRINT_INFO("read no data.\n");
            usleep(3000);
            continue;
        }

        //PRINT_INFO("header.frame_type %d recv_buf %x data_length %d \r\n",header.frame_type,recv_buf,data_length); 
        
        if(header.frame_type != IPC_FRAME_TYPE_A)
        {
            continue;
        }
        
        if(s32Fd)
        {		
            fwrite(recv_buf, 1, data_length, s32Fd);
            fflush(s32Fd);					
            totallen += data_length;
        }
    }

    PRINT_INFO("totallen:%d\n", totallen);
    
    IPC_AUDIO_WAV_HEAD_S stWavHead;
    memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
    IPC_PF_WAV_HeaderInit(&stWavHead, totallen, 0x1, 8000, 16);
    fseek(s32Fd, 0, SEEK_SET);
    fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd);
    fflush(s32Fd);
    
    if(s32Fd)
    {
        fclose(s32Fd);
    } 
    
    if(hMbufReader)
    {
        IPC_MID_MBUF_DelReader(hMbufReader);
    }
    
    PRINT_INFO("finished %s record!\n", pcAiOutputFileName);

    return NULL;
}


static void * IPC_PF_Audio_RecordThreadFromMbuf()
{
    AR_S32 s32Ret = 0;
	AR_U32 i = 0, j = 0, u32Id = -1;
	IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            if(pstVencCfg->vencStream[i][j].enable)
            {
            	u32Id = IPC_GET_VENC_CHN_ID(i, j);
				break;	
            }

        }
		if(u32Id >=0)
		{
			break;
		}
    }
	
	AR_CHAR name[64] = {0};
	sprintf(name, "audio_rec_%d_%d", u32Id/IPC_MAX_VENC_STREAM_PER_CHN, u32Id % IPC_MAX_VENC_STREAM_PER_CHN);
	prctl(PR_SET_NAME, name, 0, 0, 0);

	AUDIO_SOUND_MODE_E eSoundMode = AUDIO_SOUND_MODE_STEREO;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    eSoundMode = IPC_PF_Audio_GetSoundMode(pstAudioCfg->chans);
	FILE *s32Fd = NULL, *s32Fd_L = NULL, *s32Fd_R = NULL;
    AR_S32 totallen = 0, totallen_l =0, totallen_r = 0;
    
    s32Fd = fopen(pcAiOutputFileName, "wb+");
    if(!s32Fd)
    {
        PRINT_ERR("open %s failed!\n", pcAiOutputFileName);
        return NULL;
    }
    else
    {
        fseek(s32Fd, 0, SEEK_SET);
    }
    
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
        s32Fd_L = fopen(pcAiOutputFileName_L, "wb+");
        if(!s32Fd_L)
        {
            PRINT_ERR("open %s failed!\n", pcAiOutputFileName_L);
            return NULL;
        }
        else
        {
            fseek(s32Fd_L, 0, SEEK_SET);
        }
    
        s32Fd_R = fopen(pcAiOutputFileName_R, "wb+");
        if(!s32Fd_R)
        {	
            PRINT_ERR("open %s failed!\n", pcAiOutputFileName_R);
            return NULL;
        }
        else
        {
            fseek(s32Fd_R, 0, SEEK_SET);
        }       
    }
	
	MEDIABUF_HANDLE hMbufReader = NULL;
	hMbufReader = IPC_MID_MBUF_AddReader(u32Id);
	if(hMbufReader == NULL)
	{
		PRINT_ERR("add_recoder_user error ");
		return NULL;
	}
	IPC_MID_MBUF_SetNewestFrame(hMbufReader);


    IPC_FRAME_HEADER_S header = {0};
    char *recv_buf = NULL;
    int data_length = 0;

	while (1)
    {
        if(stRecordExit.u32Stop)
        {
            PRINT_INFO("IPC_PF_AiRecordThread %d, exit\n", s32Ret);
            break;
        }
		
        recv_buf = NULL;
        s32Ret = IPC_MID_MBUF_ReadFrame(hMbufReader, (void **)&recv_buf, &data_length, &header);
        if (s32Ret < 0)
        {
			PRINT_ERR("stream%d : IPC_MID_MBUF_ReadFrame fail.\n", u32Id);
    		break;
        }
        
        if (s32Ret == 0)
        {
            PRINT_INFO("read no data.\n");
            usleep(3000);
            continue;
        }

		//PRINT_INFO("header.frame_type %d recv_buf %x data_length %d \r\n",header.frame_type,recv_buf,data_length); 

		if(header.frame_type != IPC_FRAME_TYPE_A)
		{
		   continue;
		}
		
		if(eSoundMode == AUDIO_SOUND_MODE_MONO)
		{
			if(s32Fd)
			{		
				fwrite(recv_buf, 1, data_length, s32Fd);
				fflush(s32Fd);					
				totallen += data_length;
			}
		}
		else
		{
			//need to save left+right data
		    char* pl = recv_buf;
			char* pr = recv_buf + data_length/2;
			if(s32Fd)
			{				
				//need to save left+right data
				int write_offset = 0;
				int perwritelen = 2;
				while(write_offset < data_length/2)
				{
					fwrite(pl + write_offset, 1, perwritelen, s32Fd);
					fwrite(pr + write_offset, 1, perwritelen, s32Fd);
					fflush(s32Fd);
					write_offset += perwritelen;
				}
				totallen += data_length;
			}
				
			if(s32Fd_L)
			{
				fwrite(pl, 1, data_length/2, s32Fd_L);
				fflush(s32Fd_L);
				totallen_l += data_length/2;
			}
			if(s32Fd_R)
			{
				fwrite(pr, 1, data_length/2, s32Fd_R);
				fflush(s32Fd_R);
				totallen_r += data_length/2;
			}
		}

	}

	if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
		PRINT_INFO("totallen:%d,totallen_l:%d,totallen_r:%d\n", totallen, totallen_l, totallen_r);
	else
		PRINT_INFO("totallen:%d\n", totallen);

	IPC_AUDIO_WAV_HEAD_S stWavHead;
	memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
	IPC_PF_WAV_HeaderInit(&stWavHead, totallen, pstAudioCfg->chans, pstAudioCfg->sampleRate, pstAudioCfg->sampleBitWidth);
	fseek(s32Fd, 0, SEEK_SET);
	fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd);
	fflush(s32Fd);

	if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
	{
		memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
		IPC_PF_WAV_HeaderInit(&stWavHead, totallen, 0x1, pstAudioCfg->sampleRate, pstAudioCfg->sampleBitWidth);
		fseek(s32Fd_L, 0, SEEK_SET);
		fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd_L);
		fflush(s32Fd_L);
		
		memset(&stWavHead, 0, sizeof(IPC_AUDIO_WAV_HEAD_S));
		IPC_PF_WAV_HeaderInit(&stWavHead, totallen, 0x1, pstAudioCfg->sampleRate, pstAudioCfg->sampleBitWidth);
		fseek(s32Fd_L, 0, SEEK_SET);
		fwrite(&stWavHead, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd_R);
		fflush(s32Fd_R);
	}

	if(s32Fd)
	{
		fclose(s32Fd);
	} 
	
	if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
	{
		if(s32Fd_L)
		{
			fclose(s32Fd_L);
		}
		if(s32Fd_R)
		{
			fclose(s32Fd_R);
		}
	}

	if(hMbufReader)
	{
		IPC_MID_MBUF_DelReader(hMbufReader);
	}

    return NULL;
}

AR_S32 IPC_PF_Audio_RecordStart(AR_CHAR * pcPath, AR_U32 u32StreamId)
{  
    //create record file 
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
	
    sprintf(pcAiOutputFileName, "%s_Audio.wav", pcPath);
    sprintf(pcAiOutputFileName_L, "%s_Audio_L.wav", pcPath);
    sprintf(pcAiOutputFileName_R, "%s_Audio_R.wav", pcPath);
    
    #if 0	
    AR_S32 s32Ret = 0;
    AR_S32 AiDev = 0, AiChn = 0;
    AR_S32 AencDev = 0, AencChn = 0; 
    AR_S32 AoDev = 0, AoChn = 0;

    if(pstAudioCfg->mode == 2)
    {
        IPC_PF_AiUnBindAo(AiDev, AiChn, AoDev, AoChn);
    }
    else if(pstAudioCfg->mode == 3)
    {
        IPC_PF_AENC_StopMbufThread();
        
        IPC_PF_AiUnBindAenc(AiDev, AiChn, AencDev, AencChn);
    }
    #endif
    
    sleep(2);
    stRecordExit.u32Stop = 0;
	stRecordExit.u32Id = u32StreamId;

	if(strcmp(pstAudioCfg->i2sType, "tp9930"))
	{
        IPC_MID_UTIL_CreateDetachThread(IPC_PF_Audio_RecordThreadFromMbuf, NULL, NULL);
	}
	else
	{
		IPC_MID_UTIL_CreateDetachThread(IPC_PF_Audio_Tp9930RecordThreadFromMbuf, &stRecordExit.u32Id, NULL);
	}
    
    return 0;
}

AR_S32 IPC_PF_Audio_RecordStop()
{  
    stRecordExit.u32Stop = 1;
    sleep(2);
    
    #if 0	
    AR_S32 s32Ret = 0;
    AR_S32 AiDev = 0, AiChn = 0;
    AR_S32 AoDev = 0, AoChn = 0;
    AR_S32 AencDev = 0, AencChn = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
	
    if(pstAudioCfg->mode == 2)
    {
        IPC_PF_AiBindAo(AiDev, AiChn, AoDev, AoChn);
    }
    else if(pstAudioCfg->mode == 3)
    {
        IPC_PF_AiBindAenc(AiDev, AiChn, AencDev, AencChn);
        IPC_PF_AENC_StartMbufThread();
    }
    #endif
    
    PRINT_INFO("stop audio record thread success!\n");
    return 0;
}

void * IPC_PF_Audio_PlayBackThread(void *argv)
{
    AR_S32 s32Ret  = AR_SUCCESS;
    AR_CHAR pcPathName[64] = {0};
    AUDIO_DEV AoDevId = 0;
    AO_CHN AoChn = 0;
    AUDIO_FRAME_S pAoSendFrame = {0};
    IPC_AUDIO_WAV_HEAD_S pstWaveHeader = {0};
    AUDIO_SOUND_MODE_E eSoundMode = AUDIO_SOUND_MODE_STEREO;
    FILE *s32Fd = NULL;
    
    // create VB Pool
    VB_POOL vbPoolId = -1;
    VB_POOL_CONFIG_S stVbPoolCfg;
    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
    stVbPoolCfg.u64BlkSize = 1024 * 10;
    stVbPoolCfg.u32BlkCnt = 10;
    vbPoolId = AR_MPI_VB_CreatePool(&stVbPoolCfg);
    if(vbPoolId < 0)
    {
        PRINT_ERR("[%s][%d]--->>>Create VB Pool failed!\n", __func__, __LINE__);
        stPlaybackExit.u32Stop = 1;
        return NULL;
    }
	
    PRINT_INFO("Create VB Pool success, pool id = %d\n", vbPoolId);
    strcpy(pcPathName, (AR_CHAR *)argv);
    s32Fd = fopen(pcPathName, "rb+");
    if(!s32Fd)
    {	
        PRINT_ERR("open %s failed!\n",pcPathName);
		goto DestoryPool;
        return NULL;
    }
    else
    {
        if(strstr(pcPathName,".wav"))
        {
            fread(&pstWaveHeader, 1, sizeof(IPC_AUDIO_WAV_HEAD_S), s32Fd);
            PRINT_INFO("**********************\n");
            PRINT_INFO("*** audio wav info:\n");
            PRINT_INFO("*** audio type: PCM\n");
            eSoundMode = IPC_PF_Audio_GetSoundMode(pstWaveHeader.NumChannels);
            PRINT_INFO("*** SoundMode   :%d [0:MONO,1:STEREO]\n", eSoundMode);
            PRINT_INFO("*** BitWidth    :%d \n",pstWaveHeader.BitsPerSample);
            PRINT_INFO("*** SampleRate  :%d \n",pstWaveHeader.SampleRate);
            PRINT_INFO("*** DataLen     :%d \n",pstWaveHeader.DataLen);
            PRINT_INFO("**********************\n");
        }
        else
        {
            eSoundMode = AUDIO_SOUND_MODE_STEREO;
            fseek(s32Fd, 0, SEEK_END);
            fseek(s32Fd, 0, SEEK_SET);
        }
    }

    AR_U64 u64BlkSize = 512;
    VB_BLK blk = 0;;
    AR_VOID *frameVirtAddr = NULL;
    VB_POOL poolId = -1;
    
    blk = AR_MPI_VB_GetBlock(vbPoolId, u64BlkSize, NULL);
    if (blk == VB_INVALID_HANDLE) 
    {
        printf("get block failed\n");
        goto CloseFd;
    }
    
    poolId = AR_MPI_VB_Handle2PoolId(blk);
    if (poolId < 0) {
        PRINT_ERR("get pool id failed\n");
        stPlaybackExit.u32Stop = 1;
        goto RelBlk;
    }
    s32Ret = AR_MPI_VB_MmapPool(poolId);
    if (s32Ret) {
        PRINT_ERR("mmap pool failed\n");
        stPlaybackExit.u32Stop = 1;
        goto RelBlk;
    }
    
    AR_U64 phyAddr = AR_MPI_VB_Handle2PhysAddr(blk);
    s32Ret = AR_MPI_VB_GetBlockVirAddr(poolId, phyAddr, &frameVirtAddr);
    if (s32Ret) 
    {
        PRINT_ERR("get frame virtual address failed\n");
        stPlaybackExit.u32Stop = 1;
        goto Unmap;
    }
    
    AR_U64 u64ReadSize = 0;
    while(1)
    {
        if(stPlaybackExit.u32Stop)
        {
            PRINT_INFO("IPC_PF_AoRecordThread %d, exit\n", s32Ret);
            break;
        }
		
        memset(&pAoSendFrame, 0x0, sizeof(AUDIO_FRAME_S));
        pAoSendFrame.u64PhyAddr[0] = phyAddr;
        pAoSendFrame.u32Len = u64BlkSize;
        
        memset(frameVirtAddr, 0x0, u64BlkSize);
        if(eSoundMode == AUDIO_SOUND_MODE_MONO)
            u64ReadSize = u64BlkSize;
        else
            u64ReadSize = u64BlkSize*2;

        AR_CHAR *buf = (AR_CHAR *)malloc(u64ReadSize);
        if(!buf)
        {
            printf("malloc buf failed!!!\n");
            usleep(10*1000);
            continue;
        }

        s32Ret = fread(buf, 1, u64ReadSize, s32Fd);
        if(s32Ret <= 0)
        {
            if(strstr(pcPathName,".wav"))
            {
                fseek(s32Fd, 44, SEEK_SET);
            }
            else
            {
                fseek(s32Fd, 0, SEEK_SET);
            }
            s32Ret = fread(buf, 1, u64ReadSize, s32Fd);
        }

        if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
        {
            pAoSendFrame.u32Len = s32Ret/2;
            for(int x = 0;x < (s32Ret/4); x++)
            {
                memcpy(frameVirtAddr+x*2, buf+x*4, 2);
            }
        }
        else
        {
            pAoSendFrame.u32Len = s32Ret;
            memcpy(frameVirtAddr, buf, s32Ret);
        }

		free(buf);

        pAoSendFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
        pAoSendFrame.enSoundmode = eSoundMode;	
        pAoSendFrame.u32PoolId[0] = poolId;
        pAoSendFrame.u64VirAddr[0] = frameVirtAddr;
        if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
        {
            pAoSendFrame.u32PoolId[1] = poolId;
            pAoSendFrame.u64PhyAddr[1] = pAoSendFrame.u64PhyAddr[0];
            pAoSendFrame.u64VirAddr[1] = pAoSendFrame.u64VirAddr[0];
        }

        s32Ret = AR_MPI_AO_SendFrame(AoDevId, AoChn, &pAoSendFrame, 1000);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[Warning]: AO SendFrame failed!!!error:%#x\n",s32Ret);
            break;
        }
	}

    AR_MPI_AO_ClearChnBuf(AoDevId, AoChn);

Unmap:
    s32Ret = AR_MPI_VB_MunmapPool(poolId);
    if (s32Ret) {
        PRINT_ERR("munmap pool failed\n");
    }
RelBlk:
    s32Ret = AR_MPI_VB_ReleaseBlock(blk);
    if (s32Ret) {
        PRINT_ERR("release yuv frame block failed\n");
    }
    
CloseFd:	
    if(s32Fd)
    {
        fclose(s32Fd);
    }
DestoryPool:
    s32Ret = AR_MPI_VB_DestroyPool(vbPoolId);
    if (s32Ret) {
        PRINT_ERR("destory vb pool failed\n");
    }

    PRINT_INFO("play %s finished\n", pcPathName);	
    return NULL;
}

AR_S32 IPC_PF_Audio_RecordPlayStart(AR_CHAR * pcPath)
{
#if 0  
    AR_S32 s32Ret = 0;
    AR_S32 AiDev = 0, AiChn = 0;
    AR_S32 AencDev = 0, AencChn = 0; 
    AR_S32 AoDev = 0, AoChn = 0;
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();

    if(pstAudioCfg->mode == 2)
    {
        s32Ret = IPC_PF_AiUnBindAo(AiDev, AiChn, AoDev, AoChn);
        if(s32Ret)
        {
            PRINT_ERR("IPC_PF_AiBindAo failed!!!error:%#x", s32Ret);
            return -1;
        }
    }
    else
    {
        IPC_PF_AO_Init();
    }
#endif	

	IPC_PF_AO_Init();
	IPC_PF_AO_SetVolume(8);

    sleep(1);
    stPlaybackExit.u32Stop = 0;
    IPC_MID_UTIL_CreateDetachThread(IPC_PF_Audio_PlayBackThread, pcPath, NULL);

	PRINT_INFO("play audio record!!!!");
    return 0;
}

AR_S32 IPC_PF_Audio_RecordPlayStop()
{
    stPlaybackExit.u32Stop = 1;
    PRINT_INFO("stop audio record play thread");

#if 0
	AR_S32 s32Ret = 0;
	AR_S32 AiDev = 0, AiChn = 0;
	AR_S32 AencDev = 0, AencChn = 0; 
	AR_S32 AoDev = 0, AoChn = 0;
	IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
	
    if(pstAudioCfg->mode == 2)
    {
        s32Ret = IPC_PF_AiBindAo(AiDev, AiChn, AoDev, AoChn);
        if(s32Ret)
        { 
            PRINT_ERR("IPC_PF_AiBindAo failed!!!error:%#x", s32Ret);
            return -1;
        }
    }
    else
    {
        IPC_PF_AO_UnInit();
    }
#else
	IPC_PF_AO_UnInit();
#endif

	return 0;
}

AR_S32 IPC_PF_Audio_Suspend()
{
    AR_S32 s32Ret = 0;
    AUDIO_DEV AiDevId = 0;
    AI_CHN AiChn = 0;
    AUDIO_DEV AoDevId = 0;
    AO_CHN AoChn = 0;
    AIO_ATTR_S pstAttr = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(!pstAudioCfg->mode)
    {
        PRINT_INFO("Audio is not enable!");
        return 0;
    }

    s32Ret = IPC_PF_Audio_DestoryChannel();
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_Audio_DestoryChannel failed!!!error:%#x", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    if(pstAudioCfg->mode == 2)
    {
        s32Ret = AR_MPI_AO_GetPubAttr(AoDevId, &pstAttr);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AO Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        for(AoChn = 0; AoChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AoChn++)
        {
            s32Ret = AR_MPI_AO_DisableChn(AoDevId, AoChn);
            if(s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("[%s][%d]--->>>AO Chn disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
                return -1;
            }
            
            s32Ret = AR_MPI_AO_SuspendChn(AoDevId, AoChn);
            if(s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("[%s][%d]--->>>AO Chn Suspend failed!ret:%#x\n", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
    }
    else if(pstAudioCfg->mode == 3)
    {
        s32Ret = IPC_PF_AENC_UnInit();
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>IPC_PF_AENC_UnInit failed!!!error:%#x", __func__, __LINE__, s32Ret);
            return -1;
        }
    }

    s32Ret = AR_MPI_AI_GetPubAttr(AiDevId, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    for(AiChn = 0; AiChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AiChn++)
    {
        s32Ret = AR_MPI_AI_DisableChn(AiDevId, AiChn);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AI Chn disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        s32Ret = AR_MPI_AI_SuspendChn(AiDevId, AiChn);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AI Chn suspend failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }

	return 0;
}

AR_S32 IPC_PF_Audio_Resume()
{
	AR_S32 s32Ret = 0;
    AUDIO_DEV AiDevId = 0;
    AI_CHN AiChn = 0;
    AUDIO_DEV AoDevId = 0;
    AO_CHN AoChn = 0;
    AIO_ATTR_S pstAttr = {0};
    IPC_CFG_AUDIO_S * pstAudioCfg = IPC_CFG_AUDIO_GetParam();
    
    if(!pstAudioCfg->mode)
    {
        PRINT_INFO("Audio is not enable!");
        return 0;
    }

    if(pstAudioCfg->mode == 2)
    {
        s32Ret = AR_MPI_AO_GetPubAttr(AoDevId, &pstAttr);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AO Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        for(AoChn = 0; AoChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AoChn++)
        {
            s32Ret = AR_MPI_AO_EnableChn(AoDevId, AoChn);
            if(s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("[%s][%d]--->>>AO Chn disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
                return -1;
            }
            
            s32Ret = AR_MPI_AO_ResumeChn(AoDevId, AoChn);
            if(s32Ret != AR_SUCCESS)
            {
                PRINT_ERR("[%s][%d]--->>>AO Chn Suspend failed!ret:%#x\n", __func__, __LINE__, s32Ret);
                return -1;
            }
        }
    }
    else if(pstAudioCfg->mode == 3)
    {
        s32Ret = IPC_PF_AENC_Init();
        if(s32Ret < 0)
        {
            PRINT_ERR("[%s][%d]--->>>IPC_PF_AENC_UnInit failed!!!error:%#x", __func__, __LINE__, s32Ret);
            return -1;
        }
    }

    s32Ret = AR_MPI_AI_GetPubAttr(AiDevId, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        PRINT_ERR("[%s][%d]--->>>AI Dev GetAttr failed!ret:%#x\n", __func__, __LINE__, s32Ret);
        return -1;
    }
    
    for(AiChn = 0; AiChn < (pstAttr.u32ChnCnt>>pstAttr.enSoundmode); AiChn++)
    {
        s32Ret = AR_MPI_AI_EnableChn(AiDevId, AiChn);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AI Chn disable failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
        
        s32Ret = AR_MPI_AI_ResumeChn(AiDevId, AiChn);
        if(s32Ret != AR_SUCCESS)
        {
            PRINT_ERR("[%s][%d]--->>>AI Chn suspend failed!ret:%#x\n", __func__, __LINE__, s32Ret);
            return -1;
        }
    }

	s32Ret = IPC_PF_Audio_CreateChannel();
    if(s32Ret < 0)
    {
        PRINT_ERR("[%s][%d]--->>>IPC_PF_Audio_CreateChannel failed!!!error:%#x", __func__, __LINE__, s32Ret);
        return -1;
    }

	return 0;
}

void IPC_PF_AENC_MbufThread_Suspend()
{
	g_u32AThreadSuspend = 1;
	printf("IPC_PF_AENC_MbufThread_Suspend\n");
}

void IPC_PF_AENC_MbufThread_Resume()
{
	g_u32AThreadSuspend = 0;
	printf("IPC_PF_AENC_MbufThread_Resume\n");
}

void IPC_PF_AI_MbufThread_Suspend()
{
	g_u32AThreadSuspend = 1;
	printf("IPC_PF_AI_MbufThread_Suspend\n");
}

void IPC_PF_AI_MbufThread_Resume()
{
	g_u32AThreadSuspend = 0;
	printf("IPC_PF_AI_MbufThread_Resume\n");
}


