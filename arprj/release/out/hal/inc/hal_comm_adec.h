/**
 * @file hal_comm_adec.h
 * @brief  hal 音频解码通用数据结构
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef  __HAL_COMM_ADEC_H__
#define  __HAL_COMM_ADEC_H__


#include "hal_type.h"
//#include "ar_common.h"
#include "hal_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define AR_ADEC_MAX_CHN_NUM        32

typedef struct
{
	ENUM_AUDIO_BIT_WIDTH   enBitWidth;  /*bit width (only support 16bit)*/
	ENUM_AUDIO_SOUND_MODE  enSoundMode; /* sound mode of inferent audio frame */
	AR_U32 resv;            /*reserve item*/
}STRU_ADEC_ATTR_G711;

typedef struct
{
    ENUM_G726_BPS enG726bps;
}STRU_ADEC_ATTR_G726;

typedef struct
{
    ENUM_ADPCM_TYPE enADPCMType;
}STRU_ADEC_ATTR_ADPCM;

typedef struct
{
    AR_U32 resv;
}STRU_ADEC_ATTR_LPCM;

typedef enum
{
    AR_ADEC_MODE_PACK = 0,/*require input is valid dec pack(a
                        complete frame encode result),
                        e.g.the stream get from AENC is a
                        valid dec pack, the stream know actually
                        pack len from file is also a dec pack.
                        this mode is high-performative*/
    AR_ADEC_MODE_STREAM ,/*input is stream,low-performative,
                        if you couldn't find out whether a stream is
                        vaild dec pack,you could use
                        this mode*/
    AR_ADEC_MODE_BUTT
}ENUM_ADEC_MODE;

typedef struct
{
    ENUM_PAYLOAD_TYPE enType;
    AR_U32            u32BufSize;  /*buf size[2~MAX_AUDIO_FRAME_NUM]*/
    ENUM_ADEC_MODE    enMode;      /*decode mode*/
    AR_VOID AR_ATTRIBUTE      *pValue;
}STRU_ADEC_CHN_ATTR;

typedef struct
{
    ENUM_PAYLOAD_TYPE  enType;
    AR_CHAR         aszName[17];
    AR_S32          (*pfnOpenDecoder)(AR_VOID *pDecoderAttr, AR_VOID **ppDecoder); /*struct ppDecoder is packed by user,user malloc and free memory for this struct */
    AR_S32          (*pfnDecodeFrm)(AR_VOID *pDecoder, AR_U8 **pu8Inbuf, AR_S32 *ps32LeftByte,
                        AR_U16 *pu16Outbuf, AR_U32 *pu32OutLen, AR_U32 *pu32Chns);
    AR_S32          (*pfnGetFrmInfo)(AR_VOID *pDecoder, AR_VOID *pInfo);
    AR_S32          (*pfnCloseDecoder)(AR_VOID *pDecoder);
    AR_S32          (*pfnResetDecoder)(AR_VOID *pDecoder);
} STRU_ADEC_DECODER;

typedef enum
{
    AR_ADEC_ERR_DECODER_ERR     = 64,
    AR_ADEC_ERR_BUF_LACK,

} ENUM_EN_ADEC_ERR_CODE;


/* invlalid device ID */
#define HAL_ERR_ADEC_INVALID_DEVID     AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HAL_ERR_ADEC_INVALID_CHNID     AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HAL_ERR_ADEC_ILLEGAL_PARAM     AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HAL_ERR_ADEC_EXIST             AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/* channel unexists */
#define HAL_ERR_ADEC_UNEXIST           AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* using a NULL point */
#define HAL_ERR_ADEC_NULL_PTR          AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HAL_ERR_ADEC_NOT_CONFIG        AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HAL_ERR_ADEC_NOT_SUPPORT       AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HAL_ERR_ADEC_NOT_PERM          AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HAL_ERR_ADEC_NOMEM             AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HAL_ERR_ADEC_NOBUF             AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define HAL_ERR_ADEC_BUF_EMPTY         AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HAL_ERR_ADEC_BUF_FULL          AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HAL_ERR_ADEC_SYS_NOTREADY      AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/* decoder internal err */
#define HAL_ERR_ADEC_DECODER_ERR       AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, AR_ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define HAL_ERR_ADEC_BUF_LACK          AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_ERROR, AR_ADEC_ERR_BUF_LACK)
/* timeout err */
#define HAL_ERR_ADEC_TIMEOUT_ERR	   AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_CRIT, HAL_ERR_TIMEOUT);
/* busy err */
#define HAL_ERR_ADEC_BUSY_ERR         AR_HAL_DEF_ERR(AR_SYS_ID_ADEC, HAL_ERR_LEVEL_CRIT, HAL_ERR_BUSY);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __AR_COMM_ADEC_H__*/

