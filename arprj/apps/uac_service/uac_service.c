#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include <arm_neon.h>
#include <math.h>

#include <alsa/asoundlib.h>

#include <mpi_audio.h>
#include <acodec.h>
#include <mpi_sys.h>
#include <osal_thread.h>

#include "uac_configfs.h"
#include "uac_service.h"
#include "rt5633_audio.h"

ar_uac_info * p_uac_info = NULL;

unsigned int g_run = 0;

static int find_pra_index_by_name(char *name,int argc, char *argv[])
{
    int i = 0;
    for(i = 0; i < argc; i++)
    {
        if(!strcmp(name, argv[i]))
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
    //unsigned int        size        = 0;
    ACODEC_IOCTL_E      cmd         = 0;
    ACODEC_VOL_CTRL     vol_ctl     = {0};

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    index = find_pra_index_by_name("-cmd", argc, argv);
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
                //size = sizeof(ACODEC_VOL_CTRL);
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
            //size = sizeof(ACODEC_VOL_CTRL);
            break;

        default:
            if(argv[index + 2])
            {
                ctl = atoi(argv[index + 2]);
            }
            arg = &ctl;
            //size = sizeof(int);
            break;
    }

    ret = ar_mpi_acodec_ioctl(fdAcodec, cmd, arg);
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
    p_uac_info->run = 0;
}

AR_S32 UAC_SYS_INIT(VB_CONFIG_S * st_vb_config)
{
    AR_S32 s32_ret = AR_FAILURE;

    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();

    if (NULL == st_vb_config)
    {
        printf("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    s32_ret = AR_MPI_VB_SetConfig(st_vb_config);
    if (AR_SUCCESS != s32_ret)
    {
        int i = 0;
        printf("USE user pool");
        for(i = 0; i < st_vb_config->u32MaxPoolCnt; ++i)
        {
            s32_ret = AR_MPI_VB_CreatePool(&st_vb_config->astCommPool[i]);
            if (AR_FAILURE == s32_ret)
            {
                printf("AR_MPI_VB_CreatePool failed!\n");
                return AR_FAILURE;
            }
            p_uac_info->is_usrpool = 1;
            p_uac_info->pool_id = s32_ret;
        }
    }
    else
    {
        s32_ret = AR_MPI_VB_Init();
        if (AR_SUCCESS != s32_ret)
        {
            printf("AR_MPI_VB_Init failed!\n");
            return AR_FAILURE;
        }
        p_uac_info->is_usrpool = 0;
        p_uac_info->pool_id = VB_INVALID_POOLID;
    }

    s32_ret = AR_MPI_SYS_Init();
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_SYS_Init failed, ret = %d!\n", s32_ret);
        AR_MPI_VB_Exit();
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

AR_S32 UAC_SYS_UNINIT()
{
    AR_S32 s32_ret = AR_FAILURE;
    int i = 0;

    s32_ret = AR_MPI_SYS_Exit();
    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_SYS_UnInit failed, ret = 0x%x!\n", s32_ret);
        //return AR_FAILURE;
    }

    for(i = 0; i < VB_MAX_POOLS; ++i)
    {
        AR_MPI_VB_MunmapPool(i);
    }

    if(p_uac_info->is_usrpool)
    {
        s32_ret = AR_MPI_VB_DestroyPool(p_uac_info->pool_id);
        p_uac_info->is_usrpool = 0;
        p_uac_info->pool_id = VB_INVALID_POOLID;
    }
    else
    {
        s32_ret = AR_MPI_VB_Exit();
        if (AR_SUCCESS != s32_ret)
        {
            printf("AR_MPI_VB_Exit failed, ret = 0x%x!\n", s32_ret);
            AR_MPI_VB_Exit();
            //return AR_FAILURE;
        }
    }

    return s32_ret;
}

AR_S32 UVC_AUDIO_StartAi(AUDIO_DEV AiDevId, AR_S32 s32AiChnCnt,          AIO_ATTR_S* pstAioAttr)
{
    AR_S32 i;
    AR_S32 s32Ret;

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
    }

    return AR_SUCCESS;
}

AR_S32 UVC_INNER_CODEC_CfgAudio(AUDIO_SAMPLE_RATE_E enSample)
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
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SOFT_RESET_CTRL))
    {
        printf("Reset audio codec error\n");
    }

    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_I2S1_FS, &enSample))
    {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        ret = AR_FAILURE;
    }

    input_mode = ACODEC_MIXER_IN1;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_MIXER_MIC, &input_mode))
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
        if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_INPUT_VOL, &iAcodecInputVol))
        {
            printf("%s: set acodec micin volume failed\n", __FUNCTION__);
            return AR_FAILURE;
        }

        iAcodecOutputVol = 1;
        if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_OUTPUT_VOL, &iAcodecOutputVol))
        {
            printf("%s: set acodec lineout volume failed\n", __FUNCTION__);
            return AR_FAILURE;
        }

    }

    close(fdAcodec);
    return ret;
}

 /* config codec */
 AR_S32 UVC_AUDIO_CfgAcodec(AIO_ATTR_S* pstAioAttr)
 {
     AR_S32 s32Ret = AR_SUCCESS;

     /*** INNER AUDIO CODEC ***/
     s32Ret = UVC_INNER_CODEC_CfgAudio(pstAioAttr->enSamplerate);
     if (AR_SUCCESS != s32Ret)
     {
         printf("%s:SAMPLE_INNER_CODEC_CfgAudio failed\n", __FUNCTION__);
         return s32Ret;
     }

     return AR_SUCCESS;
 }

 AR_S32 UVC_AUDIO_StopAi(AR_S32 AiDevId, AR_S32 s32AiChnCnt,
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

AR_S32 UVC_AUDIO_StartAo(AUDIO_DEV AoDevId, AR_S32 s32AoChnCnt,
                                 AIO_ATTR_S * pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, AR_BOOL bResampleEn)
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

    return AR_SUCCESS;
}

AR_S32 UVC_AUDIO_StopAo(AUDIO_DEV AoDevId, AR_S32 s32AoChnCnt, AR_BOOL bResampleEn)
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

static int wait_for_poll(snd_pcm_t *handle, struct pollfd *ufds, unsigned int count, int timeout)
{
    unsigned short revents;

    poll(ufds, count, timeout);
    snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
    if (revents & POLLERR)
    {
        return -EIO;
    }

    if (revents & POLLOUT)
    {
        return 0;
    }

    return -1;
}

static int xrun_recovery(snd_pcm_t *handle, int err)
{
    if (err == -EPIPE) {    /* under-run */
        err = snd_pcm_prepare(handle);
        if (err < 0)
            printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
        return 0;
    } else if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
            sleep(1);    /* wait until the suspend flag is released */
        if (err < 0) {
            err = snd_pcm_prepare(handle);
            if (err < 0)
                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
        }
        return 0;
    }
    return err;
}

static int data_interlace_u16(unsigned short * data_in_l, unsigned short * data_in_r, unsigned int data_unit_num, unsigned short * data_out)
{
    uint16x8_t    tmp[8];
    uint16x8x2_t  result[4];
    int i = 0, j = 0, tmp_len;

    tmp_len = data_unit_num & (32 - 1);

    for(i = 0, j = 0; i < tmp_len; i += 32, j += 64)
    {
        tmp[0] = vld1q_u16(data_in_l + i);
        tmp[1] = vld1q_u16(data_in_l + i + 8);
        tmp[2] = vld1q_u16(data_in_l + i + 16);
        tmp[3] = vld1q_u16(data_in_l + i + 24);
        tmp[4] = vld1q_u16(data_in_r + i);
        tmp[5] = vld1q_u16(data_in_r + i + 8);
        tmp[6] = vld1q_u16(data_in_r + i + 16);
        tmp[7] = vld1q_u16(data_in_r + i + 24);

        result[0] = vzipq_u16(tmp[0], tmp[4]);
        result[1] = vzipq_u16(tmp[1], tmp[5]);
        result[2] = vzipq_u16(tmp[2], tmp[6]);
        result[3] = vzipq_u16(tmp[3], tmp[7]);

        vst2q_u16(data_out + j, result[0]);
        vst2q_u16(data_out + j + 16, result[1]);
        vst2q_u16(data_out + j + 32, result[2]);
        vst2q_u16(data_out + j + 48, result[3]);
    }

    for(; i < data_unit_num; ++i, j += 2)
    {
        data_out[j]     = data_in_l[i];
        data_out[j + 1] = data_in_r[i];
    }

    return 0;
}

int data_deinterlace_u16(unsigned short * data_in, unsigned int data_unit_num, unsigned short * data_out_l, unsigned short * data_out_r)
{
    uint16x8x2_t    tmp[4];
    int i = 0, j = 0, tmp_len;

    if(data_unit_num & 0x1)
    {
        return -1;
    }

    tmp_len = data_unit_num & (64 - 1);

    for(i = 0, j = 0; i < tmp_len; i += 64, j += 32)
    {
        tmp[0] = vld2q_u16(&data_in[i]);
        tmp[1] = vld2q_u16(&data_in[i + 16]);
        tmp[2] = vld2q_u16(&data_in[i + 32]);
        tmp[3] = vld2q_u16(&data_in[i + 48]);

        vst1q_u16(&data_out_l[j], tmp[0].val[0]);
        vst1q_u16(&data_out_r[j], tmp[0].val[1]);
        vst1q_u16(&data_out_l[j + 8], tmp[1].val[0]);
        vst1q_u16(&data_out_r[j + 8], tmp[1].val[1]);
        vst1q_u16(&data_out_l[j + 16], tmp[2].val[0]);
        vst1q_u16(&data_out_r[j + 16], tmp[2].val[1]);
        vst1q_u16(&data_out_l[j + 24], tmp[3].val[0]);
        vst1q_u16(&data_out_r[j + 24], tmp[3].val[1]);
    }

    for(; i < data_unit_num; i += 2, ++j)
    {
        data_out_l[j] = data_in[i];
        data_out_r[j] = data_in[i + 1];
    }

    return 0;
}

void * UVC_AUDIO_Ai(void* arg)
{
    AR_S32              s32Ret;
    AR_S32              s32AiChnCnt;
    AIO_ATTR_S          stAioAttr;

    AR_S32              AiDev = 0;
    AR_S32              AiChn = 0;
    AR_S32              AiFd;

    unsigned char       *buffer = NULL;
    unsigned int        index = 0;
    unsigned int        data_len = 0;

    unsigned int        latency         = 0;
    int                 count           = 0;
    struct pollfd       *ufds           = NULL;
    snd_pcm_sframes_t    frames;

    fd_set              read_fds;
    struct timeval      TimeoutVal;

    AUDIO_FRAME_S       stFrame;
    AI_CHN_PARAM_S      stAiChnPara;


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

    buffer = (unsigned char *)malloc(stAioAttr.u32PtNumPerFrm * stAioAttr.u32ChnCnt * (stAioAttr.enBitwidth + 1));
    if(NULL == buffer)
    {
        printf("buffer malloc failed!\n");
        return NULL;
    }

    /* enable AI channle */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = UVC_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr);
    if (s32Ret != AR_SUCCESS)
    {
        goto AI_ERR1;
    }

    /* config internal audio codec */
    s32Ret = UVC_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != AR_SUCCESS)
    {
        goto AI_ERR2;
    }

    if ((s32Ret = snd_pcm_open(&p_uac_info->in_handle, UAC_DEVICE_PATH, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    //if ((err = snd_pcm_open(&p_uac_info->in_handle, UAC_DEVICE_PATH, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0)
    {
        printf("Playback open error: %s\n", snd_strerror(s32Ret));
        goto AI_ERR2;
    }

    latency = stAioAttr.u32PtNumPerFrm * 1000000 / stAioAttr.enSamplerate;
    if ((s32Ret = snd_pcm_set_params(p_uac_info->in_handle,
                                  SND_PCM_FORMAT_S16,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  stAioAttr.u32ChnCnt,
                                  stAioAttr.enSamplerate,
                                  1,
                                  latency)) < 0)/* latency */
    {
        printf("Playback open error: %s\n", snd_strerror(s32Ret));
        goto AI_ERR3;
    }

    printf("latency %u us\n", latency);

    count = snd_pcm_poll_descriptors_count(p_uac_info->in_handle);
    if (count <= 0)
    {
        printf("Invalid poll descriptors count\n");
        goto AI_ERR3;
    }

    ufds = (struct pollfd *)malloc(sizeof(struct pollfd) * count);
    if (ufds == NULL)
    {
        printf("No enough memory\n");
        goto AI_ERR3;
    }

    if ((s32Ret = snd_pcm_poll_descriptors(p_uac_info->in_handle, ufds, count)) < 0)
    {
        printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(s32Ret));
        goto AI_ERR4;
    }

    s32Ret = AR_MPI_AI_GetChnParam(AiDev, AiChn, &stAiChnPara);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        goto AI_ERR4;
    }

    stAiChnPara.u32UsrFrmDepth = 8;

    s32Ret = AR_MPI_AI_SetChnParam(AiDev, AiChn, &stAiChnPara);
    if (AR_SUCCESS != s32Ret)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        goto AI_ERR4;
    }

    FD_ZERO(&read_fds);
    AiFd = AR_MPI_AI_GetFd(AiDev, AiChn);
    FD_SET(AiFd, &read_fds);

    while(p_uac_info->run)
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
            s32Ret = AR_MPI_AI_GetFrame(AiDev, AiChn, &stFrame, NULL, 1000);
            if (AR_SUCCESS != s32Ret )
            {
                printf("%s: AR_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, AiDev, AiChn, s32Ret);
                break;
            }

            data_len = stFrame.u32Len / (stFrame.enBitwidth + 1);
            data_interlace_u16((AR_U16 *)stFrame.u64VirAddr[0], (AR_U16 *)stFrame.u64VirAddr[1], data_len, (AR_U16 *)buffer);

            s32Ret = wait_for_poll(p_uac_info->in_handle, ufds, count, stAioAttr.u32PtNumPerFrm * 1000 / stAioAttr.enSamplerate / 2);
            if(s32Ret >= 0 || -EIO == s32Ret)
            {
try_again:
                frames = snd_pcm_writei(p_uac_info->in_handle, (void *)buffer, data_len);
                if (frames < 0)
                {
                    frames = snd_pcm_recover(p_uac_info->in_handle, frames, 0);
                    if(frames >= 0)
                    {
                        printf("try_again\n");
                        goto try_again;
                    }
                }

                if (frames < 0)
                {
                    printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
                    //break;
                }

                if (frames > 0 && frames < (long)data_len)
                    printf("Short write (expected %li, wrote %li)\n", (long)data_len, frames);
            }
            else if(s32Ret < 0)
            {
                //printf("ret %d\n", ret);
                if (snd_pcm_state(p_uac_info->in_handle) == SND_PCM_STATE_XRUN ||
                    snd_pcm_state(p_uac_info->in_handle) == SND_PCM_STATE_SUSPENDED)
                {
                    s32Ret = snd_pcm_state(p_uac_info->in_handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
                    if (xrun_recovery(p_uac_info->in_handle, s32Ret) < 0)
                    {
                        printf("Write error: %s\n", snd_strerror(s32Ret));
                        AR_MPI_AI_ReleaseFrame(AiDev, AiChn, &stFrame, NULL);
                        break;
                    }
                }
                else
                {
                    //printf("Wait for poll failed\n");
                    //return err;
                }
            }

            /* finally you must release the stream */
            s32Ret = AR_MPI_AI_ReleaseFrame(AiDev, AiChn, &stFrame, NULL);
            if (AR_SUCCESS != s32Ret )
            {
                printf("%s: AR_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, AiDev, AiChn, s32Ret);
                break;
            }

            ++index;
        }
    }

AI_ERR4:
    free(ufds);

AI_ERR3:
    snd_pcm_close(p_uac_info->in_handle);

AI_ERR2:
    s32Ret |= UVC_AUDIO_StopAi(AiDev, s32AiChnCnt, 0, AR_FALSE);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
    }

AI_ERR1:
    free(buffer);

    ar_thread_exit();

    return NULL;
}

void * UVC_AUDIO_Ao(void* arg)
{
    AR_S32      s32Ret = -1;
    AR_S32      s32AoChnCnt;
    AR_S32      AoChn = 0;
    AIO_ATTR_S  stAioAttr;
    AR_S32      AoDev = AR_ACODEC_DEV_ID;

    int     data_unit_num = 0;
    int     buffer_len = 0;

    unsigned int index = 0;
    unsigned int latency = 0;
    AUDIO_FRAME_S stFrame;
    //int sample_rate_adjust = AUDIO_SAMPLE_RATE_48000;

    unsigned char       *buffer = NULL;

    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 8;
    stAioAttr.u32PtNumPerFrm = 1024;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    if (AoDev == AR_ACODEC_DEV_ID)
        stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
    else
        stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
    buffer_len = (stAioAttr.u32PtNumPerFrm * stAioAttr.u32ChnCnt * (stAioAttr.enBitwidth + 1));
    data_unit_num = stAioAttr.u32PtNumPerFrm;

    buffer = (unsigned char *)malloc(buffer_len);
    if(NULL == buffer)
    {
        printf("buffer malloc failed!\n");
        return NULL;
    }

    /* enable AO channle */
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = UVC_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0);
    if (s32Ret != AR_SUCCESS)
    {
         goto AO_ERR1;
    }
    if (stAioAttr.enI2sType  == AIO_I2STYPE_EXTERN) {
        ar_rt5633_play(16, 48000, 30);
        ar_rt5633_record(16, 48000, 3, 20);
    } else {
        /* config internal audio codec */
        s32Ret = UVC_AUDIO_CfgAcodec(&stAioAttr);
        if (s32Ret != AR_SUCCESS)
             goto AO_ERR2;
    }

    if ((s32Ret = snd_pcm_open(&p_uac_info->out_handle, UAC_DEVICE_PATH, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        printf("Capture open error: %s\n", snd_strerror(s32Ret));
        goto AO_ERR2;
    }

    latency = stAioAttr.u32PtNumPerFrm * 1000000 / stAioAttr.enSamplerate;
    if ((s32Ret = snd_pcm_set_params(p_uac_info->out_handle,
                                  SND_PCM_FORMAT_S16,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  stAioAttr.u32ChnCnt,
                                  stAioAttr.enSamplerate,
                                  1,
                                  latency)) < 0)/* latency */
    {
        printf("Capture set param error: %s\n", snd_strerror(s32Ret));
        goto AO_ERR3;
    }

    while(p_uac_info->run)
    {
        VB_BLK block_handle[2];
        VB_POOL pool_id[2];
        unsigned char * data[2];
        snd_pcm_sframes_t frames = 0;

        frames = snd_pcm_readi(p_uac_info->out_handle, buffer, data_unit_num);
        if(frames < 0)
        {
            frames = snd_pcm_recover(p_uac_info->out_handle, frames, 0);
            if(frames >= 0)
            {
                //printf("try_again\n");
                continue;//break;
            }
            else
            {
                printf("get failed! %ld:%s\n", frames, snd_strerror(frames));
                continue;
            }
        }
        block_handle[0] = AR_MPI_VB_GetBlock(p_uac_info->pool_id, (frames * (stAioAttr.enBitwidth + 1)), NULL);
        if(block_handle[0] == VB_INVALID_HANDLE)
        {
            printf("get block 0 from pool 0x%x failed\n", p_uac_info->pool_id);
            break;
        }

        block_handle[1] = AR_MPI_VB_GetBlock(p_uac_info->pool_id, frames * (stAioAttr.enBitwidth + 1), NULL);
        if(block_handle[1] == VB_INVALID_HANDLE)
        {
            printf("get block 1 failed\n");
            break;
        }

        pool_id[0] = AR_MPI_VB_Handle2PoolId(block_handle[0]);
        pool_id[1] = AR_MPI_VB_Handle2PoolId(block_handle[1]);

        AR_MPI_VB_MmapPool(pool_id[0]);
        AR_MPI_VB_MmapPool(pool_id[1]);

        AR_MPI_VB_GetBlockVirAddr(pool_id[0], AR_MPI_VB_Handle2PhysAddr(block_handle[0]), (void **)(&data[0]));
        AR_MPI_VB_GetBlockVirAddr(pool_id[1], AR_MPI_VB_Handle2PhysAddr(block_handle[1]), (void **)(&data[1]));

        data_deinterlace_u16((unsigned short *)buffer,
                            frames * stAioAttr.u32ChnCnt,
                            (unsigned short *)data[0], (unsigned short *)data[1]);

        stFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
        stFrame.enSoundmode = AUDIO_SOUND_MODE_STEREO;

        stFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(block_handle[0]);
        stFrame.u64PhyAddr[1] = AR_MPI_VB_Handle2PhysAddr(block_handle[1]);
        stFrame.u64VirAddr[0] = data[0];
        stFrame.u64VirAddr[1] = data[1];
        stFrame.u64TimeStamp = 0;
        stFrame.u32Seq = index;
        stFrame.u32Len = frames * (stAioAttr.enBitwidth + 1);
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
        if (AR_SUCCESS != s32Ret)
        {
            printf("%s: ar_hal_ao_send_frame(%d, %d), failed with %#x!\n", \
                   __FUNCTION__, AoDev, AoChn, s32Ret);
            AR_MPI_VB_ReleaseBlock(block_handle[0]);
            AR_MPI_VB_ReleaseBlock(block_handle[1]);
            break;
        }

        AR_MPI_VB_ReleaseBlock(block_handle[0]);
        AR_MPI_VB_ReleaseBlock(block_handle[1]);

        ++index;
    }

AO_ERR3:
    snd_pcm_close(p_uac_info->out_handle);
AO_ERR2:
    s32Ret |= UVC_AUDIO_StopAo(AoDev, s32AoChnCnt, 0);
    if (s32Ret != AR_SUCCESS)
    {
        //SAMPLE_DBG(s32Ret);
    }
    s32Ret = 0;
AO_ERR1:
    free(buffer);
    ar_thread_exit();

    return NULL;
}


int main(int argc, char ** argv)
{
    VB_CONFIG_S             st_vb_config;
    struct sigaction        action;
    AR_S32                  s32_ret;
    ar_thread_id_t          ai_thread_id = NULL;
    ar_thread_id_t          ao_thread_id = NULL;

    if(find_pra_index_by_name("-cmd", argc, argv) > 0)
    {
        return acodec_cmd_ioctl(argc, argv);
    }

    p_uac_info = (ar_uac_info *)malloc(sizeof(ar_uac_info));
    p_uac_info->run = 1;
    p_uac_info->pool_id = -1;

    action.sa_flags                 = 0;
    action.sa_handler               = sighandler;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGKILL, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    memset(&st_vb_config, 0 , sizeof(st_vb_config));

    st_vb_config.u32MaxPoolCnt = 1;
    st_vb_config.astCommPool[0].u64BlkSize = 4096;
    st_vb_config.astCommPool[0].u32BlkCnt = 100;

    s32_ret = UAC_SYS_INIT(&st_vb_config);
    if(AR_FAILURE == s32_ret)
    {
        goto ERROR_0;
    }
    //epin
    if(getPlayMask(0))
    {
        ai_thread_id = ar_thread_new(UVC_AUDIO_Ai, NULL, 0, NULL);
        if(AR_NULL == ai_thread_id)
        {
            goto ERROR_1;
        }
    }

    //epout
    if(getCaptureMask(0))
    {
        ao_thread_id = ar_thread_new(UVC_AUDIO_Ao, NULL, 0, NULL);
        if(AR_NULL == ao_thread_id)
        {
            goto ERROR_1;
        }
    }

    if(getPlayMask(0))
    {
        ar_thread_terminate(ai_thread_id);
        ar_thread_join(ai_thread_id);
    }

    if(getCaptureMask(0))
    {
        ar_thread_terminate(ao_thread_id);
        ar_thread_join(ao_thread_id);
    }

ERROR_1:
    printf(stderr, "sys uninit exit.\n");
    UAC_SYS_UNINIT(&st_vb_config);
ERROR_0:
    return 0;
}
