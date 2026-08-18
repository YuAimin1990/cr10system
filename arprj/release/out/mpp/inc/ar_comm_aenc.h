

#ifndef  __AR_COMM_AENC_H__
#define  __AR_COMM_AENC_H__

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

/**定义 G.711 编码协议属性结构体*/
typedef struct arAENC_ATTR_G711_S
{
	AUDIO_BIT_WIDTH_E   enBitWidth;  /**<@note 新添加参数, 位宽(仅支持 16bit)*/
	AUDIO_SOUND_MODE_E  enSoundMode; /**<@note 新添加参数, 音频声道模式*/
	AR_U32 resv;            /*待扩展用（目前暂未使用）*/
}AENC_ATTR_G711_S;

/**定义 G.726 编码协议属性结构体*/
typedef struct arAENC_ATTR_G726_S
{
    G726_BPS_E enG726bps; /**<G.726 协议码率*/
}AENC_ATTR_G726_S;

/**定义 ADPCM 编码协议属性结构体*/
typedef struct arAENC_ATTR_ADPCM_S
{
    ADPCM_TYPE_E enADPCMType; /**<ADPCM 类型*/
}AENC_ATTR_ADPCM_S;

/**定义 LPCM 编码协议属性结构体*/
typedef struct arAENC_ATTR_LPCM_S
{
    AR_U32 resv;            /**<待扩展用（目前暂未使用）*/
}AENC_ATTR_LPCM_S;

/**定义编码器属性结构体*/
typedef struct arAENC_ENCODER_S
{
    PAYLOAD_TYPE_E  enType; /**<编码协议类型*/
    AR_U32          u32MaxFrmLen; /**<最大码流长度*/
    AR_CHAR         aszName[17]; /**<编码器名称*/
    AR_S32          (*pfnOpenEncoder)(AR_VOID *pEncoderAttr, AR_VOID **ppEncoder); /**<打开编码器的函数指针*/
    AR_S32          (*pfnEncodeFrm)(AR_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                        AR_U8 *pu8Outbuf,AR_U32 *pu32OutLen); /**<进行编码的函数指针*/
    AR_S32          (*pfnCloseEncoder)(AR_VOID *pEncoder); /**<关闭编码器的函数指针*/
} AENC_ENCODER_S;

/**定义音频编码通道属性结构体*/
typedef struct arAENC_CHN_ATTR_S
{
    PAYLOAD_TYPE_E      enType;         /**<音频编码协议类型*/
	AR_U32 				u32PtNumPerFrm; /**<音频编码协议对应的帧长*/
    AR_U32              u32BufSize;     /**<音频编码缓存大小*/
    AR_VOID ATTRIBUTE   *pValue;        /**<具体协议属性指针*/
}AENC_CHN_ATTR_S;

/**定义编码错误类型*/
typedef enum arEN_AENC_ERR_CODE_E
{
    AENC_ERR_ENCODER_ERR     = 64 ,
    AENC_ERR_VQE_ERR        = 65 ,
} EN_AENC_ERR_CODE_E;

/** @} */  /** <!-- ==== Structure Definition End ==== */


/* invlalid device ID */
#define AR_ERR_AENC_INVALID_DEVID     AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define AR_ERR_AENC_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AR_ERR_AENC_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* channel exists */
#define AR_ERR_AENC_EXIST             AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/* channel unexists */
#define AR_ERR_AENC_UNEXIST           AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* using a NULL point */
#define AR_ERR_AENC_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AR_ERR_AENC_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AR_ERR_AENC_NOT_SUPPORT       AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define AR_ERR_AENC_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define AR_ERR_AENC_NOMEM             AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define AR_ERR_AENC_NOBUF             AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define AR_ERR_AENC_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define AR_ERR_AENC_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define AR_ERR_AENC_SYS_NOTREADY      AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/* encoder internal err */
#define AR_ERR_AENC_ENCODER_ERR       AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, AENC_ERR_ENCODER_ERR)
/* vqe internal err */
#define AR_ERR_AENC_VQE_ERR       AR_MPP_DEF_ERR(AR_ID_AENC, HAL_ERR_LEVEL_ERROR, AENC_ERR_VQE_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __AR_COMM_AENC_H__*/

