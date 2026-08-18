/**
 * \file
 * \brief 描述AAC编码相关数据结构和接口.
 */

#ifndef _AACENC_H
#define _AACENC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif     /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hal_type.h"

/********************************Macro Definition********************************/
/**
* @defgroup AACENC
* @ingroup AENC
* @brief AAC编码
* @{
*/

#ifdef MONO_ONLY
#define MAX_CHANNELS 1 /**<aac编码通道数*/
#else
#define MAX_CHANNELS 2
#endif

#define AACENC_BLOCKSIZE 1024 /**<aacenc blocksize*/
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
typedef enum
{
    AACENC_OK                     = 0x0000,  /*!< 正常. */

    AACENC_INVALID_HANDLE         = 0x0020,  /*!< 无效句柄 */
    AACENC_MEMORY_ERROR           = 0x0021,  /*!< 内存分配错误 */
    AACENC_UNSUPPORTED_PARAMETER  = 0x0022,  /*!< 不支持的参数 */
    AACENC_INVALID_CONFIG         = 0x0023,  /*!< 无效的参数 */

    AACENC_INIT_ERROR             = 0x0040,  /*!< 一般初始化错误 */
    AACENC_INIT_AAC_ERROR         = 0x0041,  /*!< AAC库初始化错误 */
    AACENC_INIT_SBR_ERROR         = 0x0042,  /*!< SBR库初始化错误 */
    AACENC_INIT_TP_ERROR          = 0x0043,  /*!< Transport库初始化错误 */
    AACENC_INIT_META_ERROR        = 0x0044,  /*!< Meta data库初始化错误 */

    AACENC_ENCODE_ERROR           = 0x0060,  /*!< AAC编码错误 */

    AACENC_ENCODE_EOF             = 0x0080   /*!< 枚举结尾标识 */

} AR_AACENC_ERROR_E;

/**定义AAC编码质量*/
typedef enum
{
    AU_QualityExcellent = 0,
    AU_QualityHigh   = 1,
    AU_QualityMedium = 2,
    AU_QualityLow = 3,
} AuQuality;

/**定义AAC编码格式*/
typedef enum
{
    AACLC = 0,              /**<AAC-LC 格式*/
    EAAC = 1,               /**<HEAAC 或 AAC+  或 aacPlusV1*/
    EAACPLUS = 2,           /**<AAC++ 或 aacPlusV2*/
    AACLD = 3,              /**<AAC LD(低延迟)*/
    AACELD = 4,             /**<AAC ELD(低延迟)*/
} AuEncoderFormat;

/**定义AAC编码传输类型*/
typedef enum
{
    AACENC_ADTS = 0,
    AACENC_LOAS = 1,
    AACENC_LATM_MCP1 = 2,
} AACENCTransportType;

/**定义AAC编码配置信息*/
typedef struct
{
    AuQuality       quality;
    AuEncoderFormat coderFormat;
    AR_S16          bitsPerSample;
    AR_S32          sampleRate;    /**<音频文件采样率 */
    AR_S32          bitRate;       /**<编码比特率 比特/秒 */
    AR_S16          nChannelsIn;   /**<输入通道数 (1,2) */
    AR_S16          nChannelsOut;  /**<输出通道数 (1,2) */
    AR_S16          bandWidth;     /**<目标音频带宽 Hz */
    AACENCTransportType transtype;
} AACENC_CONFIG;

/**定义AAC编码版本信息*/
typedef struct arAACENC_VERSION_S
{
    AR_U8 aVersion[64];
} AACENC_VERSION_S;

typedef AR_U32 AAC_ENCODER_S;


/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/**
\brief 获取AAC编码版本信息
\param[in] pVersion       版本描述结构体
\retval ::AR_SUCCESS   : 成功
\retval ::AR_FAILURE          : 失败
\see \n
N/A
*/
AR_S32  AR_AACENC_GetVersion(AACENC_VERSION_S* pVersion);

/**
\brief 获取可用的默认配置
\param[in] pstConfig    配置信息结构体指针
\retval ::AR_SUCCESS   : 成功
\retval ::AR_FAILURE          : 失败
\see \n
N/A
*/
AR_S32  AACInitDefaultConfig(AACENC_CONFIG* pstConfig);

/**
\brief 打开并初始化一个新的编码实例
\param[in] phAacPlusEnc    配置信息结构体指针
\param[in] pstConfig    配置信息结构体指针
\retval ::AR_SUCCESS   : 成功
\retval ::AR_FAILURE   : 失败
\see \n
N/A
*/
AR_S32  AACEncoderOpen(AAC_ENCODER_S** phAacPlusEnc, AACENC_CONFIG* pstConfig);

/**
\brief 编码一帧数据
\param[in] hAacPlusEnc   配置信息结构体指针
\param[in] ps16PcmBuf    BLOCKSIZE*nChannels 音频采样数据,(交错)
\param[in] pu8Outbuf    输出buf指针,(必须是 6144/8*MAX_CHANNELS 字节)
\param[in] ps32NumOutBytes    number of bytes in output buffer after processing
\retval ::AR_SUCCESS   : 成功
\retval ::AR_FAILURE   : 失败
\see \n
N/A
*/
AR_S32  AACEncoderFrame(AAC_ENCODER_S* hAacPlusEnc, AR_S16* ps16PcmBuf, AR_U32 nb_pcm,
                        AR_U8* pu8Outbuf, AR_S32* ps32NumOutBytes);

/**
\brief 关闭编码器
\param[in] hAacPlusEnc    配置信息结构体指针
\retval N/A
\see \n
N/A
*/
AR_VOID AACEncoderClose (AAC_ENCODER_S* hAacPlusEnc);
/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif     /* __cpluscplus */
#endif  /* __cpluscplus */

/**
 * @}
 */

#endif  /* _AACENC_H */
