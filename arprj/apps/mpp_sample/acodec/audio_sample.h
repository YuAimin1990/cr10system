#ifndef __AUDIO_SAMPLE_H__
#define __AUDIO_SAMPLE_H__

#include <pthread.h>

#include "ar_comm_adec.h"
#include "ar_comm_aenc.h"

#include "audio_aac_adp.h"
#include "mpi_audio.h"
#include "mpi_vb.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*******************************************************
    macro define
*******************************************************/
#define FILE_NAME_LEN               128

#define CHECK_CHN_RET(express,Chn,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, __LINE__, Ret);\
            fflush(stdout);\
            return Ret;\
        }\
    }while(0)

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
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define TLV320_FILE "/dev/tlv320aic31"
#define COLOR_RGB_RED      0xFF0000
#define COLOR_RGB_GREEN    0x00FF00
#define COLOR_RGB_BLUE     0x0000FF
#define COLOR_RGB_BLACK    0x000000
#define COLOR_RGB_YELLOW   0xFFFF00
#define COLOR_RGB_CYN      0x00ffff
#define COLOR_RGB_WHITE    0xffffff

#define SAMPLE_VO_DEV_DHD0 0                  /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1 1                  /* VO's device HD1 */
#define SAMPLE_VO_DEV_UHD  SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD   SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VHD2 2
#define SAMPLE_VO_LAYER_PIP  SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1

#define SAMPLE_AUDIO_PTNUMPERFRM   480

#define WDR_MAX_PIPE_NUM        4


#define PAUSE()  do {\
        printf("---------------press Enter key to exit!---------------\n");\
        getchar();\
    } while (0)

#define CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
        {\
            printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);\
            return AR_FAILURE;\
        }\
    }while(0)


/*******************************************************
    enum define
*******************************************************/



/*******************************************************
    function announce
*******************************************************/
AR_S32 SAMPLE_COMM_AUDIO_StartAdec(AR_S32 AdChn, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
AR_S32 SAMPLE_COMM_AUDIO_StopAdec(AR_S32 AdChn);

AR_S32 SAMPLE_COMM_AUDIO_CreatGetTrdFileAdec(AR_S32 AdChn, FILE* pAdcFd);
AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(AR_S32 AdChn);

AR_S32 SAMPLE_COMM_AUDIO_CreatSaveAdecFrameToFile(AR_S32 AdChn, FILE* pAdcFd0, FILE* pAdcFd1);
AR_S32 SAMPLE_COMM_AUDIO_DestroySaveAdecFrameToFile(AR_S32 AdChn);
AR_S32 SAMPLE_COMM_AUDIO_SetStopAdecStatus(AR_S32 AdChn);

AR_S32 SAMPLE_COMM_AUDIO_StartAenc(AR_S32 s32AencChn, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
AR_S32 SAMPLE_COMM_AUDIO_StopAenc(AR_S32 s32AencChn);

AR_S32 SAMPLE_COMM_AUDIO_CreatTrdAenc(AR_S32 AeChn, FILE* pAecFd0, FILE* pAecFd1);
AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdAenc(AR_S32 AeChn);

AR_S32 SAMPLE_COMM_AUDIO_CreatTrdAencFile(AR_S32 AeChn, FILE* pAecFd);
AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencFile(AR_S32 AeChn);
AR_S32 SAMPLE_COMM_AUDIO_SetStopAencStatus(AR_S32 AeChn);


AR_S32 SAMPLE_COMM_AUDIO_AiBindAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn);
AR_S32 SAMPLE_COMM_AUDIO_AiUnbindAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn);
AR_S32 SAMPLE_COMM_AUDIO_AencBindAdec(AR_S32 AencDev, AR_S32 AencChn, AR_S32 AdecDev, AR_S32 AdecChn);
AR_S32 SAMPLE_COMM_AUDIO_AencUnbindAdec(AR_S32 AencDev, AR_S32 AencChn, AR_S32 AdecDev, AR_S32 AdecChn);
AR_S32 SAMPLE_COMM_AUDIO_AdecBindAo(AR_S32 AdecDev, AR_S32 AdecChn, AR_S32 AoDev, AR_S32 AoChn);
AR_S32 SAMPLE_COMM_AUDIO_AdecUnbindAo(AR_S32 AdecDev, AR_S32 AdecChn, AR_S32 AoDev, AR_S32 AoChn);

AR_S32 SAMPLE_AUDIO_AiAo(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AoDev, AR_S32 AoChn);

AR_S32 SAMPLE_AUDIO_AiToFile(AR_S32 AiDev, AR_S32 AiChn, FILE* pAiFd);
AR_S32 SAMPLE_COMM_AUDIO_CreatGetTrdFileAi(AR_S32 AiDev, AR_S32 AiChn, FILE* pAiFd);
AR_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAi(AR_S32 AiDev);
AR_S32 SAMPLE_COMM_AUDIO_SetStopAiStatus(AR_S32 AiDev);

AR_S32 SAMPLE_COMM_AUDIO_StartAi(AR_S32 AiDevId, AR_S32 s32AiChnCnt, AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, AR_BOOL bResampleEn, AR_VOID* pstAiVqeAttr, AR_U32 u32AiVqeType);
AR_S32 SAMPLE_INNER_CODEC_CfgAudio(AUDIO_SAMPLE_RATE_E enSample);
AR_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S* pstAioAttr);
AR_S32 SAMPLE_COMM_AUDIO_StopAi(AR_S32 AiDevId, AR_S32 s32AiChnCnt, AR_BOOL bResampleEn, AR_BOOL bVqeEn);
AR_S32 SAMPLE_COMM_AUDIO_StartAo(AR_S32 AoDevId, AR_S32 s32AoChnCnt, AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, AR_BOOL bResampleEn);
AR_S32 SAMPLE_COMM_AUDIO_StopAo(AR_S32 AoDevId, AR_S32 s32AoChnCnt, AR_BOOL bResampleEn);

int find_param_index_by_name(char *name,int argc, char *argv[]);
int acodec_cmd_ioctl(int argc, char * argv[]);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __AUDIO_SAMPLE_H__ */
