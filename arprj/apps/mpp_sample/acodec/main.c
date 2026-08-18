#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#include "osal.h"
#include "mpi_dbglog.h"
#include "ar_common.h"
#include "sample_comm.h"
#include "audio_sample.h"

// use for fdk-acc libSYS
void *pvPortMalloc(size_t xWantedSize )
{
  return malloc(xWantedSize);
}
void vPortFree( void *pv)
{
  free(pv);
}
/*********  AI -> File ***********/
AR_S32 SAMPLE_AUDIO_AI_FILE(AR_VOID)
{
  AR_S32    AiDev  = 0;
  AR_S32    AiChn  = 0;
  AR_S32    s32Ret = 0;
  FILE* pwfd = NULL;
  char outFileName[32] = "Aiout.pcm";

  pwfd = fopen(outFileName, "w+");
  if (NULL == pwfd)
  {
    printf("%s: open file %s failed\n", __FUNCTION__, outFileName);
    goto AiToFile_ERR2;
  }

  s32Ret = SAMPLE_AUDIO_AiToFile(AiDev, AiChn, pwfd);
  if (s32Ret) {
      printf("AiToFile failed %d.\n", s32Ret);
  }
  printf("\nplease press twice ENTER to exit this sample\n");
  getchar();
  getchar();

  s32Ret = SAMPLE_COMM_AUDIO_SetStopAiStatus(AiDev);

  s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdFileAi(AiDev);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_DestoryTrdAencFile!\n");
  }
AiToFile_ERR1:
  SAMPLE_COMM_AUDIO_StopAi(AiDev, 1, 0, AR_FALSE);
AiToFile_ERR2:
  return s32Ret;
}

/*********  AI -> AO ***********/
AR_S32 SAMPLE_AUDIO_AI_AO(AR_VOID)
{
  AR_S32    AiDev  = 0;
  AR_S32    AiChn  = 0;

  AR_S32    AoDev  = 0;
  AR_S32    AoChn  = 0;

  return SAMPLE_AUDIO_AiAo(AiDev, AiChn, AoDev, AoChn);
}

/*********  AI -> AENC -> File ***********/
AR_S32 SAMPLE_AUDIO_AI_AENC_FILE(PAYLOAD_TYPE_E emu_pl_type)
{
  AR_S32    s32Ret;

  AR_S32    AiDev  = 0;
  AR_S32    AiChn  = 0;

  AR_S32    AencDev  = 0;
  AR_S32    AencChn  = 0;

  AIO_ATTR_S stAioAttr;
  AR_S32 s32AiChnCnt;

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

  /********** 1. AI ***************/
    /* enable AI channle */
  printf("1.Enable AI channle.\n");
  s32AiChnCnt = stAioAttr.u32ChnCnt;
  s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0, NULL, 0);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
    goto __error3;
  }

  /********** 2. AAC Encode ***************/
  printf("2.Start aenc.\n");
  s32Ret = SAMPLE_COMM_AUDIO_StartAenc(AencChn, &stAioAttr, emu_pl_type);
  if (s32Ret != AR_SUCCESS)
  {
    goto __error2;
  }

  /* config internal audio codec */
  s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
    goto __error1;
  }

  /********** 3. bind ***************/
  printf("3.Bind\n");
  s32Ret = SAMPLE_COMM_AUDIO_AiBindAenc(AiDev, AiChn, AencDev, AencChn);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_AiBindAenc failed, ret:0x%02x!\n", s32Ret);
    goto __error1;
  }

  //output file
  FILE* pwfd;
  char outFileName[32] = {0};
  if(PT_AAC == emu_pl_type){
    sprintf(outFileName, "aenc_out.%s", "aac");
  }else if(PT_G711A == emu_pl_type){
    sprintf(outFileName, "aenc_out.%s", "g711a");
  }else if(PT_G711U == emu_pl_type){
    sprintf(outFileName, "aenc_out.%s", "g711u");
  }else{
    return -1;
  }

  pwfd = fopen(outFileName, "w+");
  if (NULL == pwfd)
  {
    printf("%s: open file %s failed\n", __FUNCTION__, outFileName);
    goto __error0;
  }
  printf("open out file:\"%s\" for adec ok\n", outFileName);

  s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAencFile(AencChn, pwfd);
  if (s32Ret != AR_SUCCESS)
  {
    goto __error0;
  }

  printf("\nplease press twice ENTER to exit this sample\n");
  getchar();
  getchar();

  SAMPLE_COMM_AUDIO_SetStopAencStatus(AencChn);

  s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAencFile(AencChn);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_DestoryTrdAencFile!\n");
  }

  fclose(pwfd);

__error0:
  SAMPLE_COMM_AUDIO_AiUnbindAenc(AiDev, AiChn, AencDev, AencChn);

__error1:
  s32Ret = SAMPLE_COMM_AUDIO_StopAenc(AencChn);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAenc failed!\n");
  }

__error2:
  s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, 0, AR_FALSE);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAi failed!\n");
  }

__error3:

  return s32Ret;

}

/*********  AI -> AENC -> ADEC -> AO ***********/
AR_S32 SAMPLE_AUDIO_AI_AENC_ADEC_AO(PAYLOAD_TYPE_E emu_pl_type)
{
  AR_S32    s32Ret;

  AR_S32    AiDev  = 0;
  AR_S32    AiChn  = 0;

  AR_S32    AencDev  = 0;
  AR_S32    AencChn  = 0;

  AR_S32    AdecDev  = 0;
  AR_S32    AdecChn  = 0;

  AR_S32    AoDev  = 0;
  AR_S32    AoChn  = 0;

  AIO_ATTR_S stAioAttr;
  AR_S32 s32AiChnCnt;
  AR_S32 s32AoChnCnt;

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

  /********** 1. AI ***************/
    /* enable AI channle */
  printf("1.Enable AI channle\n");
  s32AiChnCnt = stAioAttr.u32ChnCnt;
  s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0, NULL, 0);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
    goto __error4;
  }

  /********** 2. Encode ***************/
  printf("2.Start aenc.\n");
  s32Ret = SAMPLE_COMM_AUDIO_StartAenc(AencChn, &stAioAttr, emu_pl_type);
  if (s32Ret != AR_SUCCESS)
  {
    goto __error3;
  }

  /********** 3. Decode ***************/
  printf("3.Start adec.\n");
  s32Ret = SAMPLE_COMM_AUDIO_StartAdec(AdecChn,  &stAioAttr, emu_pl_type);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StartAdec failed!\n");
    goto __error2;
  }

  /********** 4. AO ***************/
  /* enable AO channle */
  printf("4.Enable AO channle\n");
  s32AoChnCnt = stAioAttr.u32ChnCnt;
  s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, 0);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
     goto __error1;
  }

  /* config internal audio codec */
  s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
    goto __error0;
  }

  /********** 5. bind ***************/
  printf("5.Bind\n");
  s32Ret = SAMPLE_COMM_AUDIO_AiBindAenc(AiDev, AiChn, AencDev, AencChn);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_AiBindAenc failed, ret:0x%02x!\n", s32Ret);
    goto __error0;
  }

  s32Ret = SAMPLE_COMM_AUDIO_AencBindAdec(AencDev, AencChn, AdecDev, AdecChn);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
    printf("SAMPLE_COMM_AUDIO_AencBindAdec failed, ret:%d!\n", s32Ret);
    goto __error_bind1;
  }

  s32Ret = SAMPLE_COMM_AUDIO_AdecBindAo(AdecDev, AdecChn, AoDev, AoChn);
  if (s32Ret != AR_SUCCESS)
  {
    //SAMPLE_DBG(s32Ret);
    printf("SAMPLE_COMM_AUDIO_AdecBindAo failed, ret:%d!\n", s32Ret);
    goto __error_bind0;
  }

  printf("\nplease press twice ENTER to exit this sample\n");
  getchar();
  getchar();

  SAMPLE_COMM_AUDIO_AdecUnbindAo(AdecDev, AdecChn, AoDev, AoChn);

__error_bind0:
  SAMPLE_COMM_AUDIO_AencUnbindAdec(AencDev, AencChn, AdecDev, AdecChn);

__error_bind1:
  SAMPLE_COMM_AUDIO_AiUnbindAenc(AiDev, AiChn, AencDev, AencChn);

__error0:
  s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, 0);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAo failed!\n");
  }

__error1:
  s32Ret = SAMPLE_COMM_AUDIO_StopAdec(AdecChn);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAdec failed!\n");
  }

__error2:
  s32Ret = SAMPLE_COMM_AUDIO_StopAenc(AencChn);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAenc failed!\n");
  }

__error3:
  s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, 0, AR_FALSE);
  if (s32Ret != AR_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAi failed!\n");
  }

__error4:

  return s32Ret;
}

AR_VOID SAMPLE_AUDIO_Usage(AR_VOID)
{
  printf("Usage1:test_mpp_audio <index> <payload_type>\n");
  printf("\tsupport payload type [0:AAC 1:G711A 2:G711U], default AAC.\n");
  printf("\tindex and its function list below:\n");
  printf("\t0:  start AI -> File \n");
  printf("\t1:  start AI -> AO \n");
  printf("\t2:  start AI -> AENC -> File \n");
  printf("\t3:  start AI -> AENC -> ADEC -> AO\n");
  printf("Usage2:test_mpp_audio -cmd <paras...>\n");
}

int main(int argc, char *argv[])
{
    AR_U32 u32Index = 0;
    PAYLOAD_TYPE_E payload_type = PT_AAC;

    if(find_param_index_by_name("-h", argc, argv) > 0 || find_param_index_by_name("--help", argc, argv) > 0)
    {
        SAMPLE_AUDIO_Usage();
        return 0;
    }

    if(find_param_index_by_name("-cmd", argc, argv) > 0)
    {
        return acodec_cmd_ioctl(argc, argv);
    }

    if (argc < 2)
    {
        SAMPLE_AUDIO_Usage();
        return AR_FAILURE;
    }

    u32Index = atoi(argv[1]);
    if (u32Index > 3)
    {
        SAMPLE_AUDIO_Usage();
        return AR_FAILURE;
    }

    if(2 < argc)
    {
        int type = atoi(argv[2]);
        if(0 == type){
            payload_type = PT_AAC;
            printf("payload_type:PT_AAC\n");
        }else if(1 == type){
            payload_type = PT_G711A;
            printf("payload_type:PT_G711A\n");
        }else if(2 == type){
            payload_type = PT_G711U;
            printf("payload_type:PT_G711U\n");
        }else{
            printf("Unsupport payload type, use default aac.\n");
        }
    }

    VB_CONFIG_S st_vb_config;
    memset(&st_vb_config, 0, sizeof(st_vb_config));

    st_vb_config.astCommPool[0].u64BlkSize = 2 * 2048 * 1080;
    st_vb_config.astCommPool[0].u32BlkCnt = 4;
    st_vb_config.astCommPool[1].u64BlkSize = 2 * 1280 * 720;
    st_vb_config.astCommPool[1].u32BlkCnt = 10;
    st_vb_config.astCommPool[2].u64BlkSize = 4096;
    st_vb_config.astCommPool[2].u32BlkCnt = 100;

    SAMPLE_COMM_SYS_Init(&st_vb_config);

    switch (u32Index)
    {
        case 0:
        {
            SAMPLE_AUDIO_AI_FILE();
            break;
        }
        case 1:
        {
            SAMPLE_AUDIO_AI_AO();
            break;
        }
        case 2:
        {
            SAMPLE_AUDIO_AI_AENC_FILE(payload_type);
            break;
        }
        case 3:
        {
            SAMPLE_AUDIO_AI_AENC_ADEC_AO(payload_type);
            break;
        }
        default:
        {
            break;
        }
    }

    SAMPLE_COMM_SYS_Exit();

    return 0;
}

