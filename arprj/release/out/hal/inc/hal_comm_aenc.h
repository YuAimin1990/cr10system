/**
 * @file hal_comm_aenc.h
 * @brief  hal 音频编码通用数据结构
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef  __HAL_COMM_AENC_H__
#define  __HAL_COMM_AENC_H__

#include "hal_type.h"
#include "hal_comm_aio.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define AR_AENC_MAX_CHN_NUM        32

typedef struct
{
	ENUM_AUDIO_BIT_WIDTH   enBitWidth;  /*bit width (only support 16bit)*/
	ENUM_AUDIO_SOUND_MODE  enSoundMode; /* sound mode of inferent audio frame */
	AR_U32 resv;            /*reserve item*/
}STRU_AENC_ATTR_G711;

typedef struct
{
    ENUM_G726_BPS enG726bps;
}STRU_AENC_ATTR_G726;

typedef struct
{
    ENUM_ADPCM_TYPE enADPCMType;
}STRU_AENC_ATTR_ADPCM;

typedef struct
{
    AR_U32 resv;            /*reserve item*/
}STRU_AENC_ATTR_LPCM;

typedef struct
{
    ENUM_PAYLOAD_TYPE  enType;
    AR_U32             u32MaxFrmLen;
    AR_CHAR            aszName[17];    /* encoder type,be used to print proc information */
    AR_S32             (*pfnOpenEncoder)(AR_VOID *pEncoderAttr, AR_VOID **ppEncoder); /* pEncoder is the handle to control the encoder */
    AR_S32             (*pfnEncodeFrm)(AR_VOID *pEncoder, const STRU_AUDIO_FRAME *pstData,
                        AR_U8 *pu8Outbuf,AR_U32 *pu32OutLen);
    AR_S32             (*pfnCloseEncoder)(AR_VOID *pEncoder);
} STRU_AENC_ENCODER;

typedef struct
{
    ENUM_PAYLOAD_TYPE      enType;         /*payload type ()*/
	AR_U32 				u32PtNumPerFrm;
    AR_U32              u32BufSize;      /*buf size [2~MAX_AUDIO_FRAME_NUM]*/
    AR_VOID AR_ATTRIBUTE   *pValue;  /*point to attribute of definite audio encoder*/
}STRU_AENC_CHN_ATTR;

typedef enum
{
    AR_AENC_ERR_ENCODER_ERR     = 64 ,
    AR_AENC_ERR_VQE_ERR        = 65 ,

} ENUM_AENC_ERR_CODE;


/* invlalid device ID */
#define HAL_ERR_AENC_INVALID_DEVID     AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HAL_ERR_AENC_INVALID_CHNID     AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HAL_ERR_AENC_ILLEGAL_PARAM     AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HAL_ERR_AENC_EXIST             AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/* channel unexists */
#define HAL_ERR_AENC_UNEXIST           AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* using a NULL point */
#define HAL_ERR_AENC_NULL_PTR          AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HAL_ERR_AENC_NOT_CONFIG        AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HAL_ERR_AENC_NOT_SUPPORT       AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define HAL_ERR_AENC_NOT_PERM          AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HAL_ERR_AENC_NOMEM             AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HAL_ERR_AENC_NOBUF             AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define HAL_ERR_AENC_BUF_EMPTY         AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HAL_ERR_AENC_BUF_FULL          AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HAL_ERR_AENC_SYS_NOTREADY      AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/* encoder internal err */
#define HAL_ERR_AENC_ENCODER_ERR       AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, AR_AENC_ERR_ENCODER_ERR)
/* vqe internal err */
#define HAL_ERR_AENC_VQE_ERR           AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_ERROR, AR_AENC_ERR_VQE_ERR)
/* vqe timeout err */
#define HAL_ERR_AENC_TIMEOUT_ERR	   AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_CRIT, HAL_ERR_TIMEOUT);
/* vqe busy err */
#define HAL_ERR_AENC_BUSY_ERR         AR_HAL_DEF_ERR(AR_SYS_ID_AENC, HAL_ERR_LEVEL_CRIT, HAL_ERR_BUSY);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __AR_COMM_AENC_H__*/

