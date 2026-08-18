

#ifndef  __AR_COMM_ADEC_H__
#define  __AR_COMM_ADEC_H__


#include "hal_type.h"
#include "ar_common.h"
#include "ar_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_AUDIO */
/** @{ */  /** <!-- [MPI_AUDIO] */


/**定义 G.711 解码协议属性结构体*/
typedef struct arADEC_ATTR_G711_S
{
    AR_U32 resv; /**<待扩展用（目前暂未使用）*/
}ADEC_ATTR_G711_S;

/**定义 G.726 解码协议属性结构体*/
typedef struct arADEC_ATTR_G726_S
{
    G726_BPS_E enG726bps; /**G.726 协议码率*/
}ADEC_ATTR_G726_S;

/**定义 ADPCM 解码协议属性结构体*/
typedef struct arADEC_ATTR_ADPCM_S
{
    ADPCM_TYPE_E enADPCMType; /**ADPCM 类型*/
}ADEC_ATTR_ADPCM_S;

/**定义 LPCM 解码协议属性结构体*/
typedef struct arADEC_ATTR_LPCM_S
{
    AR_U32 resv; /**<待扩展用（目前暂未使用）*/
}ADEC_ATTR_LPCM_S;

/**定义解码方式*/
typedef enum arADEC_MODE_E
{
    ADEC_MODE_PACK = 0, /**<Pack方式解码*/
    ADEC_MODE_STREAM , /**<stream 方式解码*/
    ADEC_MODE_BUTT
}ADEC_MODE_E;

/**定义音频解码通道属性结构体*/
typedef struct arADEC_CH_ATTR_S
{
    PAYLOAD_TYPE_E enType; /**音频解码协议类型*/
    AR_U32         u32BufSize;  /**音频解码缓存大小*/
    ADEC_MODE_E    enMode;      /**解码方式*/
    AR_VOID ATTRIBUTE      *pValue; /**具体协议属性指针*/
}ADEC_CHN_ATTR_S;

/**定义解码器属性结构体*/
typedef struct arADEC_DECODER_S
{
    PAYLOAD_TYPE_E  enType; /**音频解码协议类型*/
    AR_CHAR         aszName[17]; /**解码器名称*/
    AR_S32          (*pfnOpenDecoder)(AR_VOID *pDecoderAttr, AR_VOID **ppDecoder); /**打开解码器的函数指针*/
    AR_S32          (*pfnDecodeFrm)(AR_VOID *pDecoder, AR_U8 **pu8Inbuf,AR_S32 *ps32LeftByte,
                        AR_U16 *pu16Outbuf,AR_U32 *pu32OutLen,AR_U32 *pu32Chns); /**进行解码的函数指针*/
    AR_S32          (*pfnGetFrmInfo)(AR_VOID *pDecoder, AR_VOID *pInfo); /**获取音频帧信息的函数指针*/
    AR_S32          (*pfnCloseDecoder)(AR_VOID *pDecoder); /**关闭解码器的函数指针*/
    AR_S32          (*pfnResetDecoder)(AR_VOID *pDecoder); /**清空缓存 buffer，复位解码器*/
} ADEC_DECODER_S;

/**定义解码错误类型*/
typedef enum arEN_ADEC_ERR_CODE_E
{
    ADEC_ERR_DECODER_ERR     = 64,
    ADEC_ERR_BUF_LACK,

} EN_ADEC_ERR_CODE_E;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/* invlalid device ID */
#define AR_ERR_ADEC_INVALID_DEVID     AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define AR_ERR_ADEC_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AR_ERR_ADEC_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* channel exists */
#define AR_ERR_ADEC_EXIST             AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/* channel unexists */
#define AR_ERR_ADEC_UNEXIST           AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* using a NULL point */
#define AR_ERR_ADEC_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AR_ERR_ADEC_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AR_ERR_ADEC_NOT_SUPPORT       AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define AR_ERR_ADEC_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define AR_ERR_ADEC_NOMEM             AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define AR_ERR_ADEC_NOBUF             AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define AR_ERR_ADEC_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define AR_ERR_ADEC_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define AR_ERR_ADEC_SYS_NOTREADY      AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/* decoder internal err */
#define AR_ERR_ADEC_DECODER_ERR       AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define AR_ERR_ADEC_BUF_LACK          AR_MPP_DEF_ERR(AR_ID_ADEC, HAL_ERR_LEVEL_ERROR, ADEC_ERR_BUF_LACK)




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __AR_COMM_ADEC_H__*/

