#include <signal.h>
#include <pthread.h>
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"
#include "mpi_audio.h"
#include "mpi_vb.h"
#include "mpi_vo.h"
#include "mpi_sys.h"
#include "mpi_sysctl.h"
#include "acodec.h"
#include "hal_sys.h"

typedef enum {
    AR_ACODEC_DEV_ID = 0,
    AR_I2S_MASTER0_ID = 1,
    AR_I2S_MASTER1_ID,
#ifdef AR9341
    AR_I2S_MASTER2_ID,
#endif
    AR_I2S_SLAVE0_ID,
    AR_I2S_SLAVE1_ID,
} AR_AUDIO_DEV_ID;

#define CHECK_RET(express,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)
static AR_BOOL bEnable_AI = AR_FALSE;
static AR_BOOL bEnable_AO = AR_FALSE;
static AR_BOOL bEnable_AI_Bind_AO = AR_FALSE;
static AR_BOOL bEnable_AI_Send_AO = AR_FALSE;
static AR_BOOL bEnable_AI_SaveFrame = AR_FALSE;
static AR_BOOL bEnable_AO_SendFrame = AR_FALSE;
static AR_BOOL bTest_acodec_AI = AR_FALSE;
static AR_BOOL bTest_acodec_AO = AR_FALSE;

static AR_BOOL bAiExit = AR_TRUE;
static AR_BOOL bAoExit = AR_TRUE;
static AR_BOOL bAi_Send_Ao_EXIT = AR_TRUE;
static AR_BOOL bThreadRunFlag = AR_FALSE;
static AR_BOOL SuspendFlag = AR_FALSE;
static AUDIO_DEV AiDevId = AR_ACODEC_DEV_ID;
static AI_CHN AiChn = 0;
static AUDIO_DEV AoDevId = AR_ACODEC_DEV_ID;
static AO_CHN AoChn = 0;
static AUDIO_SAMPLE_RATE_E  eSampleRate = AUDIO_SAMPLE_RATE_48000;
static AUDIO_SOUND_MODE_E   eSoundMode  = AUDIO_SOUND_MODE_STEREO;
static AR_S32         s32AiVolume = 23;
static AR_S32         s32AoVolume = 5;
static AUDIO_BIT_WIDTH_E  eBitWidth = AUDIO_BIT_WIDTH_16;

static AR_U8   pu8AiOutputPath[512]= {0};
static AR_U8   pu8AoIutputPath[512]= {0};

static AR_U8  pu8AiOutputFileName[512]= {0};
static AR_U8  pu8AiOutputFileName_L[512]= {0};
static AR_U8  pu8AiOutputFileName_R[512]= {0};

static AR_U8 DefaultAIOutPutPath[] = "./";

VB_POOL vbPoolId = POOL_OWNER_COMMON;

static AR_S32 VoDev = 0, VoLayer = 0, VoChn = 0;

typedef enum
{
    E_AENC_TYPE_G711A = 0,
    E_AENC_TYPE_G711U,
    E_AENC_TYPE_G726_16,
    E_AENC_TYPE_G726_24,
    E_AENC_TYPE_G726_32,
    E_AENC_TYPE_G726_40,
    PCM,
} AencType_e;


typedef struct WAVE_FORMAT
{
    AR_S16  wFormatTag;
    AR_S16  wChannels;
    AR_U32  dwSamplesPerSec;
    AR_U32  dwAvgBytesPerSec;
    AR_S16  wBlockAlign;
    AR_S16  wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    AR_S8       chRIFF[4];
    AR_U32      dwRIFFLen;
    AR_S8       chWAVE[4];
    AR_S8       chFMT[4];
    AR_U32      dwFMTLen;
    WaveFormat_t  wave;
    AR_S8       chDATA[4];
    AR_U32      dwDATALen;
} WaveFileHeader_t;


int addWaveHeader(WaveFileHeader_t* tWavHead, AencType_e eAencType, AUDIO_SOUND_MODE_E eSoundMode, AUDIO_SAMPLE_RATE_E  eSampleRate, int raw_len)
{
    tWavHead->chRIFF[0] = 'R';
    tWavHead->chRIFF[1] = 'I';
    tWavHead->chRIFF[2] = 'F';
    tWavHead->chRIFF[3] = 'F';

    tWavHead->chWAVE[0] = 'W';
    tWavHead->chWAVE[1] = 'A';
    tWavHead->chWAVE[2] = 'V';
    tWavHead->chWAVE[3] = 'E';

    tWavHead->chFMT[0] = 'f';
    tWavHead->chFMT[1] = 'm';
    tWavHead->chFMT[2] = 't';
    tWavHead->chFMT[3] = 0x20;
    tWavHead->dwFMTLen = 0x10;

    if(eAencType == E_AENC_TYPE_G711A)
    {
        tWavHead->wave.wFormatTag = 0x06;
    }

    if(eAencType == E_AENC_TYPE_G711U)
    {
        tWavHead->wave.wFormatTag = 0x07;
    }

    if(eAencType == E_AENC_TYPE_G711U || eAencType == E_AENC_TYPE_G711A)
    {
        if(eSoundMode == AUDIO_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wBitsPerSample = 8;//bitWidth;g711encode出来是8bit，这里需要写死
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.wChannels) / 8;
    }
    else if(eAencType == PCM)
    {
        if(eSoundMode == AUDIO_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x1;
        tWavHead->wave.wBitsPerSample = 16; //16bit
        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample  * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
        tWavHead->wave.wBlockAlign = 1024;
    }
    else //g726
    {
    if(eSoundMode == AUDIO_SOUND_MODE_MONO)
            tWavHead->wave.wChannels = 0x01;
        else
            tWavHead->wave.wChannels = 0x02;

        tWavHead->wave.wFormatTag = 0x45;
        switch(eAencType)
        {
            case E_AENC_TYPE_G726_40:
                tWavHead->wave.wBitsPerSample = 5;
                tWavHead->wave.wBlockAlign =  5;
                break;
            case E_AENC_TYPE_G726_32:
                tWavHead->wave.wBitsPerSample = 4;
                tWavHead->wave.wBlockAlign =  4;
                break;
            case E_AENC_TYPE_G726_24:
                tWavHead->wave.wBitsPerSample = 3;
                tWavHead->wave.wBlockAlign =  3;
                break;
            case E_AENC_TYPE_G726_16:
                tWavHead->wave.wBitsPerSample = 2;
                tWavHead->wave.wBlockAlign =  2;
                break;
            default:
                printf("eAencType error:%d\n", eAencType);
                return -1;
        }

        tWavHead->wave.dwSamplesPerSec = eSampleRate;
        tWavHead->wave.dwAvgBytesPerSec = (tWavHead->wave.wBitsPerSample * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
    }

    tWavHead->chDATA[0] = 'd';
    tWavHead->chDATA[1] = 'a';
    tWavHead->chDATA[2] = 't';
    tWavHead->chDATA[3] = 'a';
    tWavHead->dwDATALen = raw_len;
    tWavHead->dwRIFFLen = raw_len + sizeof(WaveFileHeader_t) - 8;

    return 0;
}

int readWaveHeader(AR_CHAR *data)
{
}
static void GetAiFrame(FILE *pFd, FILE *pFd_L, FILE *pFd_R, AR_S32 *pTotallen, AR_S32 *pTotallen_l, AR_S32 *pTotallen_r)
{
    AR_S32 s32Ret = -1, write_offset = 0, writelen = 0, perwritelen = 0;
    AUDIO_FRAME_S pAiGetChnFrm = { 0 };

    s32Ret = AR_MPI_AI_GetFrame(AiDevId, AiChn, &pAiGetChnFrm, NULL, -1);
    if(s32Ret == AR_SUCCESS)
    {
        if(eSoundMode == AUDIO_SOUND_MODE_MONO)
        {
          if(pFd)
          {
              //need to save left+right data
              perwritelen = 16;
              while(write_offset < pAiGetChnFrm.u32Len)
              {
                  writelen = (pAiGetChnFrm.u32Len -write_offset) >perwritelen ?perwritelen:(pAiGetChnFrm.u32Len -write_offset);
                  fwrite(pAiGetChnFrm.u64VirAddr[0]+write_offset,1,writelen, pFd);
                  fflush(pFd);
                  write_offset += writelen;
              }
              *pTotallen += pAiGetChnFrm.u32Len;
          }
        }
        else
        {
          if(pFd)
          {
              //need to save left+right data
              perwritelen = 2;
              while(write_offset < pAiGetChnFrm.u32Len)
              {
                   writelen = (pAiGetChnFrm.u32Len -write_offset) >perwritelen ?perwritelen:(pAiGetChnFrm.u32Len -write_offset);
                   fwrite(pAiGetChnFrm.u64VirAddr[0]+write_offset, 1, writelen, pFd);
                   fwrite(pAiGetChnFrm.u64VirAddr[1]+write_offset, 1, writelen, pFd);
                   fflush(pFd);
                   write_offset += writelen;
              }
              *pTotallen +=pAiGetChnFrm.u32Len*2;
          }
          if(pFd_L)
          {
              fwrite(pAiGetChnFrm.u64VirAddr[0],1,pAiGetChnFrm.u32Len,pFd_L);
              fflush(pFd_L);
              *pTotallen_l += pAiGetChnFrm.u32Len;
          }
          if(pFd_R)
          {
              fwrite(pAiGetChnFrm.u64VirAddr[1],1,pAiGetChnFrm.u32Len,pFd_R);
              fflush(pFd_R);
              *pTotallen_r += pAiGetChnFrm.u32Len;
          }
      }

      s32Ret = AR_MPI_AI_ReleaseFrame(AiDevId,AiChn,&pAiGetChnFrm, NULL);
      if(s32Ret != AR_SUCCESS)
      {
          printf("AI Dev%d Chn%d release frame failed!!!error:%#x",AiDevId,AiChn,s32Ret);
      }
  }
  else
  {
       printf("Dev%dChn%d get frame failed!!!error:0x%x\n", AiDevId, AiChn, s32Ret);
  }
}
#define AI_TO_WAVE
void *Task_AiGetChnBuf(void *argv)
{
  pthread_detach(pthread_self());

  AR_S32 s32Ret  = AR_SUCCESS;
  AEC_FRAME_S  pAiGetAecFrm = { 0 };
  AUDIO_STREAM_S pAencGetStream = { 0 };
  AENC_CHN AeChn = 0;
  FILE *s32Fd = NULL;
  FILE *s32Fd_L = NULL;
  FILE *s32Fd_R = NULL;
  AR_S32 totallen = 0,totallen_l =0,totallen_r = 0;

  s32Fd = fopen(pu8AiOutputFileName,"w+");
  if(!s32Fd)
  {
    printf("open %s failed!\n",pu8AiOutputFileName);
    return NULL;
  }
  else
  {
    fseek(s32Fd, 0, SEEK_SET);
  }

  if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
  {
    s32Fd_L = fopen(pu8AiOutputFileName_L,"w+");
    if(!s32Fd_L)
    {
      printf("open %s failed!\n",pu8AiOutputFileName_L);
      fclose(s32Fd);
      return NULL;
    }
    else
    {
      fseek(s32Fd_L, 0, SEEK_SET);
    }

    s32Fd_R = fopen(pu8AiOutputFileName_R,"w+");
    if(!s32Fd_R)
    {
      printf("open %s failed!\n",pu8AiOutputFileName_R);
      fclose(s32Fd);
      fclose(s32Fd_L);
      return NULL;
    }
    else
    {
      fseek(s32Fd_R,0,SEEK_SET);
    }
  }

  AR_S32 aiFd = AR_MPI_AI_GetFd(AiDevId, AiChn);
  if(aiFd <= 0)
  {
    printf("Ai get fd failed!!!ret:%#x\n",aiFd);
    return NULL;
  }
  if(!ar_hal_sys_mpp_service_is_lib()) {
      fd_set read_fds;
      struct timeval TimeoutVal;

      //FD_ZERO(&read_fds);
      //FD_SET(aiFd, &read_fds);

      while(bAiExit == AR_FALSE)
      {
          TimeoutVal.tv_sec = 1;
          TimeoutVal.tv_usec = 0;

          FD_ZERO(&read_fds);
          FD_SET(aiFd, &read_fds);

          s32Ret = select(aiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
          if (s32Ret < 0)
          {
              break;
          }
          else if (0 == s32Ret)
          {
              printf("%s: get ai frame select time out\n", __FUNCTION__);
              break;
          }

          // For CDP Board, Left channel is Mic input, Right channel is line in
          if (FD_ISSET(aiFd, &read_fds))
          {
              GetAiFrame(s32Fd, s32Fd_L, s32Fd_R, &totallen, &totallen_l, &totallen_r);
          }
      }
  } else {
      while(bAiExit == AR_FALSE) {
          if (SuspendFlag)
              sleep(1);
          else
              GetAiFrame(s32Fd, s32Fd_L, s32Fd_R, &totallen, &totallen_l, &totallen_r);
      }
  }

  if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    printf("totallen:%d,totallen_l:%d,totallen_r:%d\n", totallen, totallen_l, totallen_r);
  else
    printf("totallen:%d\n", totallen);
#ifdef AI_TO_WAVE
  WaveFileHeader_t stWavHead;
  memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
  addWaveHeader(&stWavHead, PCM, eSoundMode, eSampleRate, totallen);
  fseek(s32Fd, 0, SEEK_SET);
  fwrite(&stWavHead, 1, sizeof(WaveFileHeader_t), s32Fd);
#endif
  fflush(s32Fd);

  if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
  {
#ifdef AI_TO_WAVE
    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
    addWaveHeader(&stWavHead, PCM, AUDIO_SOUND_MODE_MONO, eSampleRate, totallen_l);
    fseek(s32Fd_L,0,SEEK_SET);
    fwrite(&stWavHead,1,sizeof(WaveFileHeader_t),s32Fd_L);
#endif
    fflush(s32Fd_L);
#ifdef AI_TO_WAVE
    memset(&stWavHead, 0, sizeof(WaveFileHeader_t));
    addWaveHeader(&stWavHead, PCM, AUDIO_SOUND_MODE_MONO, eSampleRate, totallen_r);
    fseek(s32Fd_R,0,SEEK_SET);
    fwrite(&stWavHead,1,sizeof(WaveFileHeader_t),s32Fd_R);
#endif
    fflush(s32Fd_R);
  }

  if(s32Fd)
  {
    fclose(s32Fd);
  }

  if(aiFd > 0)
  {
  //  s32Ret = AR_MPI_AI
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

  printf("%s exit!!!\n",__func__);
  return NULL;
}


void *Task_AI_Send_AO(void *argv)
{
  pthread_detach(pthread_self());

  AR_S32 s32Ret  = AR_SUCCESS;
  AUDIO_FRAME_S pAiGetChnFrm;
  AEC_FRAME_S  pAiGetAecFrm;

  AENC_CHN AeChn = 0;
  AUDIO_STREAM_S pAencGetStream;

  memset(&pAiGetChnFrm,0x0,sizeof(AUDIO_FRAME_S));
  memset(&pAiGetAecFrm,0x0,sizeof(AEC_FRAME_S));
  memset(&pAencGetStream,0x0,sizeof(AUDIO_STREAM_S));

  AR_S32 aiFd = AR_MPI_AI_GetFd(AiDevId, AiChn);
  if(aiFd <= 0)
  {
    printf("Ai get fd failed!!!ret:%#x\n",aiFd);
    return NULL;
  }

  fd_set read_fds;
  struct timeval TimeoutVal;

  FD_ZERO(&read_fds);
  FD_SET(aiFd, &read_fds);

  while(bAi_Send_Ao_EXIT == AR_FALSE)
  {
      TimeoutVal.tv_sec = 1;
      TimeoutVal.tv_usec = 0;

      FD_ZERO(&read_fds);
      FD_SET(aiFd, &read_fds);

      s32Ret = select(aiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
      if (s32Ret < 0)
      {
          break;
      }
      else if (0 == s32Ret)
      {
          printf("%s: get ai frame select time out\n", __FUNCTION__);
          break;
      }

      if (FD_ISSET(aiFd, &read_fds))
      {
          s32Ret = AR_MPI_AI_GetFrame(AiDevId, AiChn, &pAiGetChnFrm, NULL, -1);
          if(s32Ret == AR_SUCCESS)
          {
              s32Ret = AR_MPI_AO_SendFrame(AoDevId, AoChn, &pAiGetChnFrm, 1000);
              if(s32Ret != AR_SUCCESS)
              {
                  printf("Ao Dev%d Chn%d send frame failed!!!error:%#x",AiDevId,AiChn,s32Ret);
              }

              s32Ret = AR_MPI_AI_ReleaseFrame(AiDevId, AiChn, &pAiGetChnFrm, NULL);
              if(s32Ret != AR_SUCCESS)
              {
                  printf("AI Dev%d Chn%d release frame failed!!!error:%#x",AiDevId,AiChn,s32Ret);
                  break;
              }
          }
          else
          {
              printf("Dev%dChn%d get frame failed!!!error:0x%x\n",AiDevId,AiChn, s32Ret);
          }
      }
  }

  return NULL;
}
static void *Task_aoSendFrame(void *argv)
{
    pthread_detach(pthread_self());
    AR_S32 s32Ret = AR_SUCCESS, s32raw = 0;
    FILE *s32Fd = NULL;
    AUDIO_FRAME_S pAoSendFrame;
    WaveFileHeader_t pstWaveHeader;
    VB_BLK blk;
    VB_POOL poolId;
    AR_S32 data_len = 0;
    AO_CHN_STATE_S stStatus = { 0 };

    memset(&pstWaveHeader,0x0,sizeof(WaveFileHeader_t));

    s32Fd = fopen(pu8AoIutputPath,"r+");
    if(!s32Fd)
    {
        printf("open %s failed!\n",pu8AoIutputPath);
        return NULL;
    }
    else
    {
        if(strstr(pu8AoIutputPath,".wav"))
        {
            fread(&pstWaveHeader,1,sizeof(WaveFileHeader_t),s32Fd);
            printf("**********************\n");
            printf("*** audio wav info:\n");
            switch(pstWaveHeader.wave.wFormatTag)
            {
                 case 6:
                 printf("*** audio type: G711A\n");
                 break;
                 case 7:
                 printf("*** audio type: G711U\n");
                 break;
                 case 45:
                 printf("*** audio type: G726\n");
                 break;
                 default:
                 printf("*** audio type: PCM\n");
                 break;
           }
           printf("*** SoundMode   :%d [0:MONO,1:STEREO]\n",pstWaveHeader.wave.wChannels - 1);
           printf("*** BitWidth    :%d \n",pstWaveHeader.wave.wBitsPerSample);
           printf("*** SampleRate  :%d \n",pstWaveHeader.wave.dwSamplesPerSec);
           printf("*** DataLen     :%d \n",pstWaveHeader.dwDATALen);
           printf("**********************\n");
           data_len = pstWaveHeader.dwDATALen;
        }
        else
        {
            s32raw = 1;
            fseek(s32Fd,0,SEEK_END);
            data_len = ftell(s32Fd);
            fseek(s32Fd,0,SEEK_SET);
        }
  }

  //check param

  if(!s32raw && eSampleRate != pstWaveHeader.wave.dwSamplesPerSec)
  {
    printf("Wav file samplerate not match Ao SampleRate %u,please check!!!\n", eSampleRate);
    return NULL;
  }

  AR_S32 bitwidth = 16;
  if(eBitWidth == AUDIO_BIT_WIDTH_8)
    bitwidth = 8;
  else if(eBitWidth == AUDIO_BIT_WIDTH_16)
    bitwidth = 16;
  else if(eBitWidth == AUDIO_BIT_WIDTH_24)
    bitwidth = 24;

  if(!s32raw && bitwidth != pstWaveHeader.wave.wBitsPerSample)
  {
    printf("Wav file BitWidth not match Ao BitWidth %u,please check!!!\n", eBitWidth);
    return NULL;
  }

  if(!s32raw && eSoundMode != (pstWaveHeader.wave.wChannels-1))
  {
    printf("Wav file SoundMode not match Ao SoundMode,please check!!!\n");
    if(pstWaveHeader.wave.wChannels == 0x1)
    {
      printf("file SoundMode:Mono,");
    }
    else
    {
      printf("file SoundMode:Stereo,");
    }
    if(eSoundMode == AUDIO_SOUND_MODE_MONO)
    {
      printf("Ao SoundMode:Mono\n");
    }
    else
    {
      printf("Ao SoundMode:Stereo\n");
    }
    return NULL;
  }

  while(bAoExit == AR_FALSE)
  {
    AR_U64 u64BlkSize = 1024;
    AR_U64 u64ReadSize = 0;
    AR_VOID *frameVirtAddr;
    AR_VOID *frameVirtAddr_r;
    if (SuspendFlag) {
        sleep(1);
    } else {
      blk = AR_MPI_VB_GetBlock(vbPoolId, u64BlkSize, NULL);
      if (blk == VB_INVALID_HANDLE) {
        printf("get block failed\n");
        break;
      }

      poolId = AR_MPI_VB_Handle2PoolId(blk);
      if (poolId < 0) {
        printf("get pool id failed\n");
        bAoExit = AR_TRUE;
        goto RelBlk;
      }
      s32Ret = AR_MPI_VB_MmapPool(poolId);
      if (s32Ret) {
        printf("mmap pool failed\n");
        bAoExit = AR_TRUE;
        goto RelBlk;
      }
      AR_U64 phyAddr = AR_MPI_VB_Handle2PhysAddr(blk);

      memset(&pAoSendFrame,0x0,sizeof(AUDIO_FRAME_S));

      s32Ret = AR_MPI_VB_GetBlockVirAddr(poolId,phyAddr, &frameVirtAddr);
      if (s32Ret) {
        printf("get frame virtual address failed\n");
        goto Unmap;
      }

      pAoSendFrame.u64PhyAddr[0] = phyAddr;

      if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
      {
        pAoSendFrame.u64PhyAddr[1] = pAoSendFrame.u64PhyAddr[0] + u64BlkSize/2;
        frameVirtAddr_r = frameVirtAddr + u64BlkSize/2;
      }

      u64ReadSize = u64BlkSize;
      AR_CHAR *buf = (AR_CHAR *)malloc(u64ReadSize);
      if(!buf)
      {
        printf("malloc buf failed!!!\n");
        usleep(10*1000);
        continue;
      }

      s32Ret = fread(buf,1,u64ReadSize,s32Fd);
      if(s32Ret <= 0)
      {
        if(strstr(pu8AoIutputPath,".wav"))
        {
          fseek(s32Fd,44,SEEK_SET);
        }
        else
        {
          fseek(s32Fd,0,SEEK_SET);
        }
        s32Ret = fread(buf,1,u64ReadSize,s32Fd);
      }

      if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
      {
        pAoSendFrame.u32Len = s32Ret/2;
        for(int x = 0;x <(s32Ret/4);x++)
        {
          memcpy(frameVirtAddr+x*2,buf+x*4,2);
          memcpy(frameVirtAddr_r+x*2,buf+x*4+2,2);
        }
      }
      else
      {
        pAoSendFrame.u32Len = s32Ret;
        memcpy(frameVirtAddr,buf,s32Ret);
      }

      free(buf);
      pAoSendFrame.enBitwidth = eBitWidth;
      pAoSendFrame.enSoundmode = eSoundMode;
      pAoSendFrame.u32PoolId[0] = poolId;
      pAoSendFrame.u64VirAddr[0] = frameVirtAddr;
      if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
      {
        pAoSendFrame.u32PoolId[1] = poolId;
        pAoSendFrame.u64VirAddr[1] = frameVirtAddr_r;
      }
      
      printf("[DEBUG] AO Frame: Len=%u, PhyAddr=0x%llx, PoolId=%u, VirAddr=%p\n", 
             pAoSendFrame.u32Len, pAoSendFrame.u64PhyAddr[0], pAoSendFrame.u32PoolId[0], (void*)pAoSendFrame.u64VirAddr[0]);
      
      s32Ret = AR_MPI_AO_QueryChnStat(AoDevId, AoChn, &stStatus);
      if (s32Ret) {
          printf(stderr, "AR_MPI_AO_QueryChnStat failed %d.\n", s32Ret);
      }
      //do{
      s32Ret = AR_MPI_AO_SendFrame(AoDevId, AoChn, &pAoSendFrame, 1000);
          //}while(s32Ret == AR_ERR_AO_NOBUF);
      if(s32Ret != AR_SUCCESS)
      {
          printf("[Warning]: AO SendFrame failed!!!error:%#x\n",s32Ret);
      }

      Unmap:
        s32Ret = AR_MPI_VB_MunmapPool(poolId);
        if (s32Ret) {
          printf("munmap pool failed\n");
        }
      RelBlk:
        s32Ret = AR_MPI_VB_ReleaseBlock(blk);
        if (s32Ret) {
          printf("release yuv frame block failed\n");
        }
      }
    }
    if(s32Fd)
    {
        fclose(s32Fd);
    }

    return NULL;
}


#define ACODEC_FILE     "/dev/acodec"

AR_S32 Module_AI_SetVolume(AUDIO_SAMPLE_RATE_E enSample, AR_S32 s32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL iAcodecInputVol = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
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
    iAcodecInputVol.vol_ctrl = s32Volume;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_INPUT_VOL, &iAcodecInputVol, sizeof(iAcodecInputVol)))
    {
        printf("%s: AR_AC_IOC_NR_SET_INPUT_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_INPUT_VOL success, volume=%d\n", __FUNCTION__, s32Volume);


    iAcodecInputVol.vol_ctrl = 0;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_GET_INPUT_VOL, &iAcodecInputVol, sizeof(iAcodecInputVol)))
    {
        printf("%s: AR_AC_IOC_NR_GET_INPUT_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_GET_INPUT_VOL success, volume=%d\n", __FUNCTION__, iAcodecInputVol.vol_ctrl);


END:
    close(fdAcodec);
    return ret;
}


AR_S32 Module_AI_SetVolume_L(AR_U32 u32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };
    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }
    /*
     * volume range [0,46], the lower the volume value, the louder it sounds.
     * when volume is 46, set it to mute;
     */
    vol_ctl.vol_ctrl = u32Volume;
    vol_ctl.vol_ctrl_mute = 0;
    if (vol_ctl.vol_ctrl = 46)
        vol_ctl.vol_ctrl_mute = 1;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_ADCL_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_SET_ADCL_VOL failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_ADCL_VOL success, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

    memset(&vol_ctl, 0, sizeof(ACODEC_VOL_CTRL));
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_GET_ADCL_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_GET_ADCL_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_GET_ADCL_VOL success, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AI_SetVolume_R(AR_U32 u32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    /*
     * volume range [0,46], the lower the volume value, the louder it sounds.
     * when volume is 46, set it to mute;
     */
    vol_ctl.vol_ctrl = u32Volume;
    vol_ctl.vol_ctrl_mute = 0;
    if (vol_ctl.vol_ctrl == 46)
        vol_ctl.vol_ctrl_mute = 1;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_ADCR_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_SET_ADCR_VOL failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_ADCR_VOL success, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

    memset(&vol_ctl, 0, sizeof(ACODEC_VOL_CTRL));
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_GET_ADCR_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_GET_ADCR_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_GET_ADCR_VOL success, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AI_Mute_L(AR_S32 s32Mute)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }
    vol_ctl.vol_ctrl_mute = s32Mute;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_MICL_MUTE, &vol_ctl, sizeof(ACODEC_VOL_CTRL)))
    {
        printf("%s: AR_AC_IOC_NR_SET_MICL_MUTE failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_MICL_MUTE mute=%d success.\n", __FUNCTION__, s32Mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AI_Mute_R(AR_S32 s32Mute)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }
    vol_ctl.vol_ctrl_mute = s32Mute;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_MICR_MUTE, &vol_ctl, sizeof(ACODEC_VOL_CTRL)))
    {
        printf("%s: AR_AC_IOC_NR_SET_MICR_MUTE failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_MICR_MUTE mute=%d success.\n", __FUNCTION__, s32Mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AI_PowerDown_L(AR_S32 s32Pd)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

  // s32Pd = 0, Power off; s32Pd = 1, Power on;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_PD_ADCL, &s32Pd, sizeof(s32Pd)))
    {
        printf("%s: AR_AC_IOC_NR_SET_PD_ADCL failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_PD_ADCL s32Pd=%d %s success.\n", __FUNCTION__, s32Pd, s32Pd==0 ? "Power ON" : "Power Off");

END:
    close(fdAcodec);
    return ret;
}


AR_S32 Module_AI_PowerDown_R(AR_S32 s32Pd)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

  // s32Pd = 0, Power off; s32Pd = 1, Power on;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_PD_ADCR, &s32Pd, sizeof(s32Pd)))
    {
        printf("%s: AR_AC_IOC_NR_SET_PD_ADCR failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_PD_ADCR s32Pd=%d %s success.\n", __FUNCTION__, s32Pd, s32Pd==0 ? "Power ON" : "Power Off");

END:
    close(fdAcodec);
    return ret;
}


AR_S32 test_acodec_AI()
{
  // Note: the loewer the volume value, the louder it sounds.
  while (bAoExit == AR_FALSE)
  {
    for(int volume = 46; volume >= 0; volume--){
      sleep(2);
      Module_AI_SetVolume_L(volume);  // set L value
      sleep(2);
      Module_AI_SetVolume_R(volume);  // set R value
    }

    sleep(2);
    Module_AI_Mute_L(1);  // set L mute
    sleep(2);
    Module_AI_Mute_L(0);  // set L unmute
    sleep(2);
    Module_AI_Mute_R(1);  // set R mute
    sleep(2);
    Module_AI_Mute_R(0);  // set R unmute
    sleep(2);
    Module_AI_PowerDown_L(1); // set L power off
    sleep(2);
    Module_AI_PowerDown_L(0); // set L power on
    sleep(2);
    Module_AI_PowerDown_R(1); // set R power off
    sleep(2);
    Module_AI_PowerDown_R(0); // set R power on
  }

  return 0;
}


AR_S32 Module_AO_SetVolume(AUDIO_SAMPLE_RATE_E enSample,AR_S32 s32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL iAcodecOutputVol = { 0 };
    //ENMU_AR_ACODEC_MIXER input_mode = 0;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    /******************************************************************************************
    The output volume range is [-122, +15]. Both the analog gain and digital gain are adjusted.
    A larger value indicates higher volume.
    For example, the value 14 indicates the maximum volume of 14 dB,
    and the value -122 indicates the minimum volume (muted status).
    The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
    The recommended volume range is [-2, +8],step is 3db.
    Within this range, the noises are lowest because only the analog gain is adjusted,
    and the voice quality can be guaranteed.
    *******************************************************************************************/
    iAcodecOutputVol.vol_ctrl = s32Volume;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_OUTPUT_VOL, &iAcodecOutputVol, sizeof(iAcodecOutputVol)))
    {
        printf("%s: AR_AC_IOC_NR_SET_OUTPUT_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_OUTPUT_VOL success, volume=%d\n", __FUNCTION__, s32Volume);

    iAcodecOutputVol.vol_ctrl = 0;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_GET_OUTPUT_VOL, &iAcodecOutputVol, sizeof(iAcodecOutputVol)))
    {
        printf("%s: AR_AC_IOC_NR_GET_OUTPUT_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_GET_OUTPUT_VOL success, volume=%d\n", __FUNCTION__, iAcodecOutputVol.vol_ctrl);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AO_SetVolume_L(AR_U32 u32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    /*
     * volume range [0,10], the lower the volume value, the louder it sounds.
     * when volume is 10, set it to mute;
     */
    vol_ctl.vol_ctrl = u32Volume;
    vol_ctl.vol_ctrl_mute = 0;
    if(vol_ctl.vol_ctrl == 10)
        vol_ctl.vol_ctrl_mute = 1;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_DACL_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_SET_DACL_VOL failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_DACL_VOL success, u32Volume = %d, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, u32Volume, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

    memset(&vol_ctl, 0, sizeof(ACODEC_VOL_CTRL));
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_GET_DACL_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_GET_DACL_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_GET_DACL_VOL success, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AO_SetVolume_R(AR_U32 u32Volume)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    /*
     * volume range [0,10], the lower the volume value, the louder it sounds.
     * when volume is 10, set it to mute;
     */
    vol_ctl.vol_ctrl = u32Volume;
    vol_ctl.vol_ctrl_mute = 0;
    if(vol_ctl.vol_ctrl == 10)
        vol_ctl.vol_ctrl_mute = 1;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_DACR_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_SET_DACR_VOL failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_DACR_VOL success, u32Volume = %d, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, u32Volume, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

    memset(&vol_ctl, 0, sizeof(ACODEC_VOL_CTRL));
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_GET_DACR_VOL, &vol_ctl, sizeof(vol_ctl)))
    {
        printf("%s: AR_AC_IOC_NR_GET_DACR_VOL failed\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_GET_DACR_VOL success, vol_ctrl=%d, vol_ctrl_mute=%d\n", __FUNCTION__, vol_ctl.vol_ctrl, vol_ctl.vol_ctrl_mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AO_Mute_L(AR_S32 s32Mute)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;
    ACODEC_VOL_CTRL vol_ctl = { 0 };

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_DACL_MUTE, &s32Mute, sizeof(s32Mute)))
    {
        printf("%s: AR_AC_IOC_NR_SET_DACL_MUTE failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_DACL_MUTE mute=%d success.\n", __FUNCTION__, s32Mute);
END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AO_Mute_R(AR_S32 s32Mute)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_DACR_MUTE, &s32Mute, sizeof(s32Mute)))
    {
        printf("%s: AR_AC_IOC_NR_SET_DACR_MUTE failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_DACR_MUTE mute=%d success.\n", __FUNCTION__, s32Mute);

END:
    close(fdAcodec);
    return ret;
}

AR_S32 Module_AO_PowerDown_L(AR_S32 s32Pd)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    // s32Pd = 0, Power off; s32Pd = 1, Power on;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_PD_DACL, &s32Pd, sizeof(s32Pd)))
    {
        printf("%s: AR_AC_IOC_NR_SET_PD_DACL failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_PD_DACL s32Pd=%d %s success.\n", __FUNCTION__, s32Pd, s32Pd==0 ? "Power ON" : "Power Off");
END:
    close(fdAcodec);
    return ret;
}


AR_S32 Module_AO_PowerDown_R(AR_S32 s32Pd)
{
    AR_S32 fdAcodec = -1;
    AR_S32 ret = AR_SUCCESS;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return AR_FAILURE;
    }

    // s32Pd = 0, Power off; s32Pd = 1, Power on;
    if (ar_mpi_acodec_ioctl(fdAcodec, IOC_NR_SET_PD_DACR, &s32Pd, sizeof(s32Pd)))
    {
        printf("%s: AR_AC_IOC_NR_SET_PD_DACR failed!\n", __FUNCTION__);
        ret = AR_FAILURE;
        goto END;
    }
    printf("%s: AR_AC_IOC_NR_SET_PD_DACR s32Pd=%d %s success.\n", __FUNCTION__, s32Pd, s32Pd==0 ? "Power ON" : "Power Off");

END:
    close(fdAcodec);
    return ret;
}


AR_S32 test_acodec_AO()
{
    // Note: the loewer the volume value, the louder it sounds.
    while (bAoExit == AR_FALSE)
    {
        for(int volume = 10; volume >= 0; volume--)
        {
            sleep(2);
            Module_AO_SetVolume_L(volume);  // set L value
            sleep(2);
            Module_AO_SetVolume_R(volume);  // set R value
        }

        sleep(2);
        Module_AO_Mute_L(1);  // set L mute
        sleep(2);
        Module_AO_Mute_L(0);  // set L unmute
        sleep(2);
        Module_AO_Mute_R(1);  // set R mute
        sleep(2);
        Module_AO_Mute_R(0);  // set R unmute
        sleep(2);
        Module_AO_PowerDown_L(1); // set L power off
        sleep(2);
        Module_AO_PowerDown_L(0); // set L power on
        sleep(2);
        Module_AO_PowerDown_R(1); // set R power off
        sleep(2);
        Module_AO_PowerDown_R(0); // set R power on
   }

  return 0;
}

AR_S32 Module_AI_Init()
{
  AR_S32 s32Ret = AR_SUCCESS;
  AIO_ATTR_S pstAttr = { 0 };
  AI_CHN_PARAM_S pstChnParam = { 0 };

  pstAttr.enSamplerate = eSampleRate;
  pstAttr.enBitwidth = eBitWidth;
  pstAttr.enWorkmode = AIO_MODE_I2S_MASTER;
  pstAttr.enSoundmode = eSoundMode;
  pstAttr.u32EXFlag = 0;
  pstAttr.u32FrmNum = 30;
  pstAttr.u32PtNumPerFrm = 1024;
  pstAttr.u32ChnCnt = 2;
  pstAttr.u32ClkSel = 0;
  if (AiDevId == AR_ACODEC_DEV_ID)
      pstAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
  else
      pstAttr.enI2sType = AIO_I2STYPE_EXTERN;
  if (AiDevId >= AR_I2S_SLAVE0_ID && AiDevId <= AR_I2S_SLAVE1_ID)
      pstAttr.enWorkmode = AIO_MODE_I2S_SLAVE;

  s32Ret = AR_MPI_AI_SetPubAttr(AiDevId,&pstAttr);
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>AI Dev setAttr failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }

  s32Ret = AR_MPI_AI_Enable(AiDevId);
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>AI Dev Enable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }

  s32Ret = AR_MPI_AI_EnableChn(AiDevId,  AiChn);
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>AI Chn%d Enable failed!ret:%#x\n",__func__,__LINE__,AiChn,s32Ret);
    return s32Ret;
  }
  pstChnParam.u32UsrFrmDepth = 6;
  s32Ret = AR_MPI_AI_SetChnParam(AiDevId,AiChn,&pstChnParam);
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>AI Chn setParam failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }


  return s32Ret;
}

AR_S32 Module_AI_DeInit()
{
  AR_S32 s32Ret = AR_SUCCESS;

  s32Ret = AR_MPI_AI_DisableChn(AiDevId, AiChn);
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>AI Chn disable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }

  s32Ret = AR_MPI_AI_Disable(AiDevId);
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>AI Dev disable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }

  return s32Ret;
}
AR_VOID SAMPLE_VO_GetUserLayerAttr(VO_VIDEO_LAYER_ATTR_S *pstLayerAttr,SIZE_S  *pstDevSize)
{
    pstLayerAttr->bClusterMode = AR_FALSE;
    pstLayerAttr->bDoubleFrame = AR_FALSE;
    pstLayerAttr->enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    pstLayerAttr->enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    pstLayerAttr->stDispRect.s32X = 0;
    pstLayerAttr->stDispRect.s32Y = 0;
    pstLayerAttr->stDispRect.u32Height = pstDevSize->u32Height;
    pstLayerAttr->stDispRect.u32Width  = pstDevSize->u32Width;

    pstLayerAttr->stImageSize.u32Height = pstDevSize->u32Height;
    pstLayerAttr->stImageSize.u32Width = pstDevSize->u32Width;

    return;
}

static AR_S32 SIMPLE_VO_HDMI_1080P60(void)
{
    AR_S32 width = 1920;
    AR_S32 height = 1080;
    VO_CHN_ATTR_S astChnAttr = {0};
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};

    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = 0x00FF00;//COLOR_RGB_BLUE;
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = 0;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.bIop = 1;

    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    stPubAttr.stSyncInfo.bIdv = 0;
    stPubAttr.stSyncInfo.bIhs = 0;
    stPubAttr.stSyncInfo.bIvs = 0;

    stPubAttr.enIntfType = VO_INTF_HDMI;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");


    /*SET VO LAYER ATTR*/
    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;

    stLayerAttr.stDispRect.u32Width  = width;
    stLayerAttr.stDispRect.u32Height = height;

    stLayerAttr.stImageSize.u32Width = width;
    stLayerAttr.stImageSize.u32Height = height;

    stLayerAttr.u32DispFrmRt = 30;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

    /* SET AND ENABLE VO CHN */
    astChnAttr.bDeflicker = AR_FALSE;
    astChnAttr.u32Priority = 0;
    astChnAttr.stRect.u32Height = 640;
    astChnAttr.stRect.u32Width = 480;
    CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, 0, &astChnAttr), "AR_MPI_VO_SetChnAttr");
    // missing enable channel, overlay will enable dummy data
    //CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, 0), "AR_MPI_VO_EnableChn");

    return AR_SUCCESS;
}

AR_S32 Module_AO_Init()
{
    AR_S32 s32Ret = AR_SUCCESS;
    AIO_ATTR_S pstAttr;

    memset(&pstAttr,0x0,sizeof(AIO_ATTR_S));
    pstAttr.enSamplerate = eSampleRate;
    pstAttr.enBitwidth = eBitWidth;
    pstAttr.enWorkmode = AIO_MODE_I2S_MASTER;
    pstAttr.enSoundmode = eSoundMode;
    pstAttr.u32EXFlag = 0;
    pstAttr.u32FrmNum = 8;
    pstAttr.u32PtNumPerFrm = 2048;
    pstAttr.u32ChnCnt = 2;
    pstAttr.u32ClkSel = 0;
    if (AoDevId == AR_ACODEC_DEV_ID)
        pstAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
    else
        pstAttr.enI2sType = AIO_I2STYPE_EXTERN;
    if (AoDevId >= AR_I2S_SLAVE0_ID && AoDevId <= AR_I2S_SLAVE1_ID)
        pstAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
    s32Ret = AR_MPI_AO_SetPubAttr(AoDevId, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Dev setParam failed!ret:%#x\n",__func__,__LINE__,s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AO_Enable(AoDevId);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Dev Enable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AO_EnableChn(AoDevId, AoChn);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Chn%d Enable failed!ret:%#x\n",__func__,__LINE__,AoChn,s32Ret);
        return s32Ret;
    }

    if (AoDevId != AR_ACODEC_DEV_ID)
        SIMPLE_VO_HDMI_1080P60();    //i2s output to ite66121

    return s32Ret;
}

AR_S32 Module_AO_DeInit()
{
    AR_S32 s32Ret = AR_SUCCESS;

    s32Ret = AR_MPI_AO_DisableChn(AoDevId, AoChn);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Chn Disable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AO_Disable(AoDevId);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Dhn Disable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
        return s32Ret;
    }
    AR_MPI_VO_DisableVideoLayer(VoLayer);
    /* DISABLE VO DEV */
    AR_MPI_VO_Disable(VoDev);

    return s32Ret;
}


void display_help()
{
  printf("----- audio all test -----\n");
  printf(" -I : Enable AI\n");
  printf(" -i : AI output path\n");
  printf(" -O : Enable AO\n");
  printf(" -o : AO input path\n");
  printf(" -D : Ai or AO device id[0:acodec, 1:i2sm0,2:i2sm1,3:i2sm2,4:i2ss0,5:i2ss1]\n");
  printf(" -c : Ai or AO channel id(0~3)\n");
  printf(" -m : Sound Mode [0:mono,1:STEREO]\n");
  printf(" -s : Samplerate [8000,12000,16000,24000,32000,44100,48000]\n");
  printf(" -b : Bit width [0:8,1:16,2:24,3:32]\n");
  printf(" -B : AI Bind AO\n");
  printf(" -S : AI Send AO\n");
  printf(" -v : AI volume  recommended ramge:[0,46]\n");
  printf(" -V : AO volume  recommended range:[-2,8]\n");
  printf(" -t : test AI acodec\n");
  printf(" -T : test AO acodec\n");
  printf(" -h : call help\n");
  printf("--------------------------\n");
}


AR_S32 check_param()
{
  AR_S32 CheckPass = AR_FAILURE;

  if(bEnable_AI)
  {
    bEnable_AI_SaveFrame = AR_TRUE;
  }

  if(bEnable_AO)
  {
    bEnable_AO_SendFrame = AR_TRUE;
  }

  if(bEnable_AI_Bind_AO || bEnable_AI_Send_AO)
  {
    bEnable_AI = AR_TRUE;
    bEnable_AO = AR_TRUE;
    AoDevId = AiDevId;
    AoChn = AiChn;
    bEnable_AI_SaveFrame = AR_FALSE;
    bEnable_AO_SendFrame = AR_FALSE;
  }

  if (bEnable_AI_Bind_AO)
  {
    printf("[Tips]:Will use AI bind AO mode!!!\n");
    bEnable_AI_Send_AO = AR_FALSE;
  }

  if(bEnable_AI_SaveFrame)
  {
    AR_U8 tmp[512] = {0};
    memset(tmp,0x0,sizeof(tmp));

    if(pu8AiOutputPath == NULL)
    {
      printf("[Tips]:AI Output path is NULL,will use default path!!!\n");
      //pu8AiOutputPath = DefaultAIOutPutPath;
      strcat(pu8AiOutputPath,DefaultAIOutPutPath);
    }

    if('/' == pu8AiOutputPath[strlen(pu8AiOutputPath) - 1])
    {
      sprintf(tmp,"%s",pu8AiOutputPath);
    }
    else
    {
      sprintf(tmp,"%s/",pu8AiOutputPath);
    }
    strcat(pu8AiOutputFileName,tmp);

    memset(tmp,0x0,sizeof(tmp));
    sprintf(tmp,"Dev0_Chn0_");
    strcat(pu8AiOutputFileName,tmp);

    memset(tmp,0x0,sizeof(tmp));
    sprintf(tmp,"%dK_",eSampleRate/1000);
    strcat(pu8AiOutputFileName,tmp);

    memset(tmp,0x0,sizeof(tmp));
    sprintf(tmp,"%dbit_",(eBitWidth+1)*8);
    strcat(pu8AiOutputFileName,tmp);

    memset(tmp,0x0,sizeof(tmp));
    if(eSoundMode == AUDIO_SOUND_MODE_MONO)
    {
      sprintf(tmp,"MONO");
    }
    else
    {
      sprintf(tmp,"STEREO");
    }
    strcat(pu8AiOutputFileName,tmp);

    sprintf(pu8AiOutputFileName_L,"%s_L",pu8AiOutputFileName);
    sprintf(pu8AiOutputFileName_R,"%s_R",pu8AiOutputFileName);

    memset(tmp,0x0,sizeof(tmp));
    #ifdef AI_TO_WAVE
    sprintf(tmp,".wav");
    #else
    sprintf(tmp,".pcm");
    #endif
    strcat(pu8AiOutputFileName,tmp);
    strcat(pu8AiOutputFileName_L,tmp);
    strcat(pu8AiOutputFileName_R,tmp);
    printf("AI Save frame file name:%s\n",pu8AiOutputFileName);
    if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
    {
      printf("AI Save frame left file name:%s\n",pu8AiOutputFileName_L);
      printf("AI Save frame right file name:%s\n",pu8AiOutputFileName_R);
    }
  }

  if(bEnable_AO_SendFrame)
  {
    if(pu8AoIutputPath == NULL)
    {
      printf("[Tips]:AO Iutput path invalid,please input again!!!\n");
      display_help();
      return AR_FAILURE;
    }
  }
  if ((AiDevId == AR_ACODEC_DEV_ID && AiChn != 0) || (AoDevId == AR_ACODEC_DEV_ID && AoChn != 0))
  {
      printf("[Tips]:Ai or Ao Devid=0 only channel 0 available!!!\n");
      return AR_FAILURE;
  }
  if ((AiDevId != AR_ACODEC_DEV_ID && AiChn > 3) || (AoDevId != AR_ACODEC_DEV_ID && AoChn > 3))
  {
      printf("[Tips]:Ai or Ao I2s only 4 channeles available!!!\n");
      return AR_FAILURE;
  }
  if((!bEnable_AI) && (!bEnable_AO) && (!bEnable_AI_Bind_AO) &&(!bEnable_AI_SaveFrame) &&(!bEnable_AI_Send_AO)&&(!bEnable_AO_SendFrame))
    return AR_FAILURE;

  sleep(1);
  return AR_SUCCESS;
}

static void signal_func(int signo, siginfo_t *info, void *data)
{
    printf("recv SIGINT signo=%d\n", signo);
    bThreadRunFlag = AR_FALSE;
}
static MPI_SYSCTL_HANDLE pHandle = NULL;
static void sysctl_call_back_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int s32Ret = 0;
    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent){
        printf("sample audio fast suspend!\n");
        usleep(1*1000);
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent || SYSCTL_EVENT_RESUME == eSysctlEvent){
        AI_CHN_PARAM_S pstChnParam = { 0 };
        printf("sample audio fast resume!\n");
        if (bEnable_AI) {
            s32Ret = Module_AI_Init();
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>Module_AI_Init failed!ret:%#x\n",__func__,__LINE__,s32Ret);
                return s32Ret;
            }
        }
        if (bEnable_AO) {
            s32Ret = AR_MPI_RGN_Resume();
            s32Ret = AR_MPI_VO_Resume(VoDev);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>VO Dev0 Resume failed!ret:%#x\n",__func__,__LINE__,s32Ret);
                return s32Ret;
            }
            s32Ret = Module_AO_Init();
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>Module_AO_Init failed!ret:%#x\n",__func__,__LINE__,s32Ret);
                return s32Ret;
            }
        }
        SuspendFlag = AR_FALSE;
    }
    else if(SYSCTL_EVENT_SUSPEND == eSysctlEvent){
        printf("sample audio suspend!\n");
        SuspendFlag = AR_TRUE;
        sleep(1);//wait ai get frame done
        if (bEnable_AI) {
            s32Ret = AR_MPI_AI_DisableChn(AiDevId,  AiChn);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>AI Chn%d Disable failed!ret:%#x\n",__func__,__LINE__,AiChn,s32Ret);
                return s32Ret;
            }
            s32Ret = AR_MPI_AI_SuspendChn(AiDevId,  AiChn);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>AI Chn%d Suspend failed!ret:%#x\n",__func__,__LINE__,AiChn,s32Ret);
                return s32Ret;
            }
            s32Ret = AR_MPI_AI_Disable(AiDevId);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>AI Dev%d Disable failed!ret:%#x\n", __func__, __LINE__, AiDevId, s32Ret);
                return s32Ret;
            }
        }
        if (bEnable_AO) {
            AR_MPI_VO_DisableChn(VoLayer, 0);
            AR_MPI_VO_DisableVideoLayer(VoLayer);
            AR_MPI_VO_Disable(VoDev);
            s32Ret = AR_MPI_VO_Suspend(VoDev);
            s32Ret = AR_MPI_RGN_Suspend();
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>VO Dev0 Suspend failed!ret:%#x\n", __func__, __LINE__, s32Ret);
                return s32Ret;
            }
            s32Ret = AR_MPI_AO_DisableChn(AoDevId,  AoChn);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>AO Chn%d Disable failed!ret:%#x\n", __func__, __LINE__, AoChn, s32Ret);
                return s32Ret;
            }
            s32Ret = AR_MPI_AO_SuspendChn(AoDevId,  AoChn);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>AO Chn%d Suspend failed!ret:%#x\n", __func__, __LINE__, AoChn, s32Ret);
                return s32Ret;
            }
            s32Ret = AR_MPI_AO_Disable(AoDevId);
            if (s32Ret != AR_SUCCESS)
            {
                printf("[%s][%d]--->>>AO Dev%d Disable failed!ret:%#x\n", __func__, __LINE__, AoDevId, s32Ret);
                return s32Ret;
            }
        }
    }

    s32Ret = AR_MPI_SYSCTL_Event_done(pHandle, eSysctlEvent);
    if(s32Ret)
        printf("sample audio event %d SYSCTL_Event_done fail!\n", eSysctlEvent);
    else
        printf("sample audio event %d SYSCTL_Event_done!\n", eSysctlEvent);
}

AR_S32 main(AR_S32 argc,char *argv[])
{
  AR_S32 s32Ret = AR_SUCCESS;
  struct sigaction sa = { 0 };
  VB_CONFIG_S pstVbConfig = { 0 };
  VB_POOL_CONFIG_S pstVbPoolConfig = { 0 };
  AR_S32 s32Opt = 0;
  void *status;
  AR_S32 s32SocType = 0;
  while((s32Opt = getopt(argc,argv,"Ii:Oo:Ss:D:c:m:v:V:Bb:tTh"))!= -1)
  {
    switch(s32Opt)
    {
      case 'I'://Enable AI
        bEnable_AI = AR_TRUE;
        break;
      case 'i'://AI output path
        memcpy(pu8AiOutputPath,optarg,strlen(optarg));
        break;
      case 'O'://Enable AO
        bEnable_AO = AR_TRUE;
        break;
      case 'o'://AO input path
        memcpy(pu8AoIutputPath,optarg,strlen(optarg));
        break;
      case 's'://Sample Rate
        eSampleRate = (AUDIO_SAMPLE_RATE_E)atoi(optarg);
        break;
      case 'D'://Dev id
        if (bEnable_AI)
            AiDevId = atoi(optarg);
        else
            AoDevId = atoi(optarg);
        break;
      case 'c'://Channel id
        if (bEnable_AI)
            AiChn = atoi(optarg);
        else
            AoChn = atoi(optarg);
        break;
      case 'b'://bitwidth
        eBitWidth = (AUDIO_BIT_WIDTH_E)atoi(optarg);
        break;
      case 'm'://Sound Mode
        printf("mode:%d\n",atoi(optarg));
        eSoundMode = (AUDIO_SOUND_MODE_E)atoi(optarg);
        break;
      case 'B'://AI Bind Ao
        bEnable_AI_Bind_AO = AR_TRUE;
        break;
      case 'S'://AI send frame to AO
        bEnable_AI_Send_AO = AR_TRUE;
        break;
      case 'v'://AI volume
        s32AiVolume = (AR_S32)atoi(optarg);
        break;
      case 'V':
        s32AoVolume = (AR_S32)atoi(optarg);
        break;
      case 't': // test AI acodec
        bTest_acodec_AI = AR_TRUE;
        break;
      case 'T': // test AO acodec
        bTest_acodec_AO = AR_TRUE;
        break;
      case 'h':
      default:
        display_help();
        return;
    }
  }
  s32Ret = check_param();
  if(s32Ret != AR_SUCCESS)
  {
    printf("Check param failed,please input again!!!\n");
    display_help();
    return s32Ret;
  }
  sa.sa_sigaction = signal_func;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  pstVbConfig.astCommPool[0].u64BlkSize = 1024*4;
  pstVbConfig.astCommPool[0].u32BlkCnt = 20;
  
  printf("[DEBUG] VB Config: BlkSize=%llu, BlkCnt=%u\n", pstVbConfig.astCommPool[0].u64BlkSize, pstVbConfig.astCommPool[0].u32BlkCnt);

  s32Ret = AR_MPI_VB_SetConfig(&pstVbConfig);
  if(s32Ret != AR_SUCCESS)
  {
      printf("[%s][%d]--->>>VB set common config failed will create user pool !ret:%#x\n",__func__, __LINE__, s32Ret);
      pstVbPoolConfig.u64BlkSize  = 1024*4;
      pstVbPoolConfig.u32BlkCnt   = 20;
      pstVbPoolConfig.enRemapMode = VB_REMAP_MODE_NONE;
      vbPoolId = AR_MPI_VB_CreatePool(&pstVbPoolConfig);
      if (VB_INVALID_POOLID == vbPoolId)
      {
          printf("[%s][%d]--->>>VB create user pool failed !ret:%#x\n", __func__, __LINE__, s32Ret);
          return s32Ret;
      }
  }
  s32Ret = AR_MPI_VB_Init();
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>VB Init failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }

  s32Ret = AR_MPI_SYS_Init();
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>SYS Init failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    goto EXIT;
  }

  if(bEnable_AI)
  {
    s32Ret = Module_AI_Init();
    if(s32Ret != AR_SUCCESS)
    {
      printf("[%s][%d]--->>>AI Init failed!ret:%#x\n",__func__,__LINE__,s32Ret);
      goto EXIT1;
    }
    Module_AI_SetVolume(eSampleRate, s32AiVolume);
  }

  if(bEnable_AO)
  {
    s32Ret = Module_AO_Init();
    if(s32Ret != AR_SUCCESS)
    {
      printf("[%s][%d]--->>>AO Init failed!ret:%#x\n",__func__,__LINE__,s32Ret);
      goto EXIT1;
    }
    Module_AO_SetVolume(eSampleRate, s32AoVolume);
  }

  if(bEnable_AI_Bind_AO)
  {
    MPP_CHN_S pstSrcChn;
    MPP_CHN_S pstDestChn;
    pstSrcChn.enModId = AR_ID_AI;
    pstSrcChn.s32DevId = AiDevId;
    pstSrcChn.s32ChnId = AiChn;

    pstDestChn.enModId = AR_ID_AO;
    pstDestChn.s32DevId = AoDevId;
    pstDestChn.s32ChnId = AoChn;
    s32Ret = AR_MPI_SYS_Bind(&pstSrcChn,&pstDestChn);
    if(s32Ret != AR_SUCCESS)
    {
      printf("[%s][%d]--->>>AI AO Bind failed!ret:%#x\n",__func__,__LINE__,s32Ret);
      goto EXIT2;
    }
  }

  bThreadRunFlag = AR_TRUE;
  pthread_t pth_ai_send_ao;
  if(bEnable_AI_Send_AO)
  {
    bAi_Send_Ao_EXIT = AR_FALSE;
    pthread_create(&pth_ai_send_ao,NULL,Task_AI_Send_AO,NULL);
  }

  pthread_t pth_ai_save_frame;
  if(bEnable_AI_SaveFrame)
  {
    bAiExit = AR_FALSE;
    pthread_create(&pth_ai_save_frame,NULL,Task_AiGetChnBuf,NULL);
  }

  pthread_t pth_test_acodec_AI;
  if(bTest_acodec_AI)
  {
    pthread_create(&pth_test_acodec_AI,NULL,test_acodec_AI,NULL);
  }

  pthread_t pth_ao_send_frame;
  if(bEnable_AO_SendFrame)
  {
    bAoExit = AR_FALSE;
    pthread_create(&pth_ao_send_frame,NULL,Task_aoSendFrame,NULL);
  }

  pthread_t pth_test_acodec_AO;
  if(bTest_acodec_AO)
  {
    pthread_create(&pth_test_acodec_AO,NULL,test_acodec_AO,NULL);
  }
  s32SocType = ar_hal_sys_get_soc_id();
  if (s32SocType == ARS_31 || s32SocType == AR_9311) {
      pHandle = AR_MPI_SYSCTL_Register("audio_sample", 0, &sysctl_call_back_func);
      if (!pHandle) {
          printf("audio sample register sysctl failed retention not support!\n");
          return -1;
      }
  }
  printf("Input 'kill pid' or Press 'Ctrl + c' to exit!!!\n");
  while(1)
  {
      if (bThreadRunFlag == AR_FALSE)
          break;
      else
          sleep(1);
  }
  if(bEnable_AO_SendFrame)
  {
    bAoExit = AR_TRUE;
  }

  if(bEnable_AI_SaveFrame)
  {
    bAiExit = AR_TRUE;
  }


  if(bEnable_AI_Send_AO)
  {
    bAi_Send_Ao_EXIT = AR_TRUE;
  }

  if(bEnable_AI_Bind_AO)
  {
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = AR_ID_AI;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = 0;

    stDestChn.enModId = AR_ID_AO;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = 0;
    s32Ret = AR_MPI_SYS_UnBind(&stSrcChn,&stDestChn);
  }

  usleep(300*1000);
EXIT2:
  if(bEnable_AI)
  {
    printf("[Info][%s][%d]--->>>Module_AI_DeInit!!!\n",__func__,__LINE__);
    s32Ret = Module_AI_DeInit();
    if(s32Ret != AR_SUCCESS)
    {
      printf("[%s][%d]--->>>AI DeInit failed!ret:%#x\n",__func__,__LINE__,s32Ret);
      return s32Ret;
    }
  }

  if(bEnable_AO)
  {
    printf("[Info][%s][%d]--->>>Module_AO_DeInit!!!\n",__func__,__LINE__);
    s32Ret = Module_AO_DeInit();
    if(s32Ret != AR_SUCCESS)
    {
      printf("[%s][%d]--->>>AO DeInit failed!ret:%#x\n",__func__,__LINE__,s32Ret);
      return s32Ret;
    }
  }
  if(vbPoolId > POOL_OWNER_COMMON)
  {
    s32Ret = AR_MPI_VB_DestroyPool(vbPoolId);
    if(s32Ret != AR_SUCCESS)
    {
      printf("[%s][%d]--->>>VB Destroy failed!ret:%#x\n", __func__, __LINE__, s32Ret);
      return s32Ret;
    }
  }
EXIT1:
  printf("[Info][%s][%d]--->>>AR_MPI_SYS_Exit!!!\n", __func__, __LINE__);
  s32Ret = AR_MPI_SYS_Exit();
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>SYS Exit failed!ret:%#x\n", __func__, __LINE__, s32Ret);
    return s32Ret;
  }
EXIT:
  s32Ret = AR_MPI_VB_Exit();
  if(s32Ret != AR_SUCCESS)
  {
    printf("[%s][%d]--->>>VB Exit failed!ret:%#x\n",__func__,__LINE__,s32Ret);
    return s32Ret;
  }
  return s32Ret;
}
