/**
 * \file
 * \brief 描述AAC解码相关数据结构和接口.
 */

#ifndef _AACDEC_H
#define _AACDEC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hal_type.h"


/********************************Macro Definition********************************/
/**
* @defgroup AACDEC
* @ingroup ADEC
* @brief AAC解码
* @{
*/

#ifndef AAC_MAX_NCHANS
#define AAC_MAX_NCHANS      2
#endif
#define AAC_MAX_NSAMPS      1024
#define AAC_MAINBUF_SIZE    (768 * AAC_MAX_NCHANS)  /**<according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3),6144 bits =  768 bytes per SCE or CCE-I,12288 bits = 1536 bytes per CPE*/

#define AAC_NUM_PROFILES    3
#define AAC_PROFILE_MP      0
#define AAC_PROFILE_LC      1
#define AAC_PROFILE_SSR     2

#ifndef AR_SUCCESS
#define AR_SUCCESS          0
#endif
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/**定义 AAC解码 错误码*/
typedef enum
{
    ERR_AAC_NONE                          =   0,        /**<无解码错误*/
    ERR_AAC_INDATA_UNDERFLOW              =  -1,        /**<输入数据不足*/
    ERR_AAC_NULL_POINTER                  =  -2,        /**<空指针*/
    ERR_AAC_INVALID_ADTS_HEADER           =  -3,        /**<无效adts头*/
    ERR_AAC_INVALID_ADIF_HEADER           =  -4,        /**<无效adif头*/
    ERR_AAC_INVALID_FRAME                 =  -5,        /**<无效帧*/
    ERR_AAC_MPEG4_UNSUPPORTED             =  -6,        /**<不支持mpeg4格式*/
    ERR_AAC_CHANNEL_MAP                   =  -7,        /**<通道映射错误*/
    ERR_AAC_SYNTAX_ELEMENT                =  -8,        /**<element错误*/
    ERR_AAC_DEQUANT                       =  -9,        /**<dequant错误*/
    ERR_AAC_STEREO_PROCESS                = -10,        /**<立体声处理错误*/
    ERR_AAC_PNS                           = -11,        /**<pns处理错误*/
    ERR_AAC_SHORT_BLOCK_DEINT             = -12,        /**<保留*/
    ERR_AAC_TNS                           = -13,        /**<TNS处理错误*/
    ERR_AAC_IMDCT                         = -14,        /**<IMDCT处理错误*/
    ERR_AAC_NCHANS_TOO_HIGH               = -15,        /**<不支持多通道*/
    ERR_AAC_SBR_INIT                      = -16,        /**<SBR内部错误*/
    ERR_AAC_SBR_BITSTREAM                 = -17,        /**<SBR比特流错误*/
    ERR_AAC_SBR_DATA                      = -18,        /**<SBR数据错误*/
    ERR_AAC_SBR_PCM_FORMAT                = -19,        /**<SBR pcm 数据错误*/
    ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,        /**<不支持SBR多通道*/
    ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,        /**<SBR 无效采样率*/
    ERR_AAC_RAWBLOCK_PARAMS               = -22,        /**<无效 RawBlock参数*/
    ERR_AAC_PS_INIT                       = -23,        /**<PS 内部错误*/
    ERR_AAC_CH_MAPPING                    = -24,
    ERR_UNKNOWN                           = -9999,      /**<保留*/
    ERR_AAC_OUT_OF_MEMORY                 = 2,          /*!< 堆返回空指针，输出buf无效. */
    ERR_AAC_UNKNOWN                       = 5,          /*!< 未知错误，输出buf无效 */
    ERR_AAC_TRANSPORT_SYNC_ERROR          = 4097,       /*!< 解码传输有同步问题，不退出解码，只需要传入新的比特流数据 */
    ERR_AAC_NOT_ENOUGH_BITS               = 4098,       /*!< 输入空间不足 */
    ERR_AAC_TRANSPORT_FATAL_ERROR         = 4099,       /*!< 解码传输发生致命错误，重置传输 */
    ERR_AAC_INVALID_HANDLE                = 8193,       /*!< 无效句柄 (NULL) */
    ERR_AAC_UNSUPPORTED_AOT               = 8194,       /*!< 不支持AOT */
    ERR_AAC_UNSUPPORTED_FORMAT            = 8195,       /*!< 不支持数据流格式  */
    ERR_AAC_UNSUPPORTED_ER_FORMAT         = 8196,       /*!< 不支持的错误恢复工具格式 */
    ERR_AAC_UNSUPPORTED_EPCONFIG          = 8197,       /*!< 不支持的错误保护格式 */
    ERR_AAC_UNSUPPORTED_MULTILAYER        = 8198,       /*!< 不支持多laye */
    ERR_AAC_UNSUPPORTED_CHANNELCONFIG     = 8199,       /*!< 不支持的通道配置 */
    ERR_AAC_UNSUPPORTED_SAMPLINGRATE      = 8200,       /*!< 不支持的采样率 */
    ERR_AAC_INVALID_SBR_CONFIG            = 8201,       /*!< 不支持SBR */
    ERR_AAC_SET_PARAM_FAIL                = 8202,       /*!< 设置参数失败 */
    ERR_AAC_NEED_TO_RESTART               = 8203,       /*!< 解码器需重启 */
    ERR_AAC_TRANSPORT_ERROR               = 16385,      /*!< 解码器传输错误 */
    ERR_AAC_PARSE_ERROR                   = 16386,      /*!< 解码错误 */
    ERR_AAC_UNSUPPORTED_EXTENSION_PAYLOAD = 16387,      /*!< 不支持载荷扩展 */
    ERR_AAC_DECODE_FRAME_ERROR            = 16388,      /*!< 数据流超出范围 */
    ERR_AAC_CRC_ERROR                     = 16389,      /*!< CRC不匹配 */
    ERR_AAC_INVALID_CODE_BOOK             = 16390,      /*!< 无效的code book */
    ERR_AAC_UNSUPPORTED_PREDICTION        = 16391,      /*!< 不支持的Predictor */
    ERR_AAC_UNSUPPORTED_CCE               = 16392,      /*!< 不支持CCE */
    ERR_AAC_UNSUPPORTED_LFE               = 16393,      /*!< 不支持LFE */
    ERR_AAC_UNSUPPORTED_GAIN_CONTROL_DATA = 16394,      /*!< 不支持的增益控制数据 */
    ERR_AAC_UNSUPPORTED_SBA               = 16395,      /*!< 不支持SBA */
    ERR_AAC_TNS_READ_ERROR                = 16396,      /*!< 读TNS数据错误 */
    ERR_AAC_RVLC_ERROR                    = 16397,      /*!< 解码错误的容错数据出错 */
    ERR_AAC_ANC_DATA_ERROR                = 32769,      /*!< 关于辅助数据处理的非严重错误 */
    ERR_AAC_TOO_SMALL_ANC_BUFFER          = 32770,      /*!<  buffer太小无法接收处理后的数据导致的错误 */
    ERR_AAC_TOO_MANY_ANC_ELEMENTS         = 32771,      /*!< 超出允许范围的数据元素写到buffer导致的错误 */
} AR_AACDEC_ERR_E;

typedef struct _AACFrameInfo
{
    int bitRate;
    int nChans;                      /**<@note 新添加参数。通道数,范围:1,2*/
    int sampRateCore;                /**<内采样率*/
    int sampRateOut;                 /**<输出采样率*/
    int bitsPerSample;               /**<位宽 ,范围ie:16*/
    int outputSamps;                 /**<output samples*/
    int profile;                     /**< profile*/
    int tnsUsed;                     /**<tns tools*/
    int pnsUsed;                     /**<pns tools*/
} AACFrameInfo;

typedef enum
{
    AACDEC_ADTS = 0,     /**<@note 新添加参数。ADTS格式*/
    AACDEC_LOAS = 1,     /**< LOAS格式*/
    AACDEC_LATM_MCP1 = 2,  /**< MCP1格式*/
} AACDECTransportType;

typedef void* HAACDecoder;

typedef struct arAACDEC_VERSION_S
{
    AR_U8 aVersion[64];
} AACDEC_VERSION_S;


/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/**
\brief 获取AAC解码版本信息
@attention 新添加接口
\param[in] pVersion    :   版本描述结构体
\retval ::AR_SUCCESS   :   成功
\retval ::AR_FAILURE   :   pVersion为空，返回失败
\see \n
N/A
*/
AR_S32 AR_AACDEC_GetVersion(AACDEC_VERSION_S* pVersion);

/**
\brief打开并初始化一个解码设备
@attention 有差异的接口，参数数量不一样
\param[in] enTranType   : Transport 类型
\retval ::HAACDecoder   : 成功，返回值非空
\retval ::NULL          : 失败，初始化失败，返回为空
\see \n
N/A
*/
HAACDecoder AACInitDecoder(AACDECTransportType enTranType);

/**
\brief 销毁AAC解码器
\param[in] hAACDecoder  : AAC解码器句柄
\see \n
N/A
*/
AR_VOID AACFreeDecoder(HAACDecoder hAACDecoder);

/**
\brief 设置RawMode在解码Raw Format aac数据m(Reserved API, unused now.)
\param[in] hAACDecoder  : AAC解码器句柄
\param[in] nChans       : 通道号数量
\param[in] sampRate     : 输入采样率
\retval ::AR_FAILURE    : RESERVED API, always return AR_FAILURE.
\see \n
N/A
*/
AR_S32  AACSetRawMode(HAACDecoder hAACDecoder, AR_S32 nChans, AR_S32 sampRate);

/**
\brief 寻找可用的AAC同步头
\param[in] hAACDecoder      : AAC解码器句柄
\param[in/out] ppInbufPtr   : 输入数据流指针的地址
\param[in/out] pBytesLeft   : 用于指明输入数据的位流大小的指针
\retval ::<0                : 失败, 经常返回 ERR_AAC_INDATA_UNDERFLOW
\retval ::other             : 成功, 返回当前帧的字节数
\see \n
N/A
*/
AR_S32 AACDecodeFindSyncHeader(HAACDecoder hAACDecoder, AR_U8** ppInbufPtr, AR_S32* pBytesLeft);

/**
\brief 解码AAC帧，每个通道输出1024(LC) 或 2048(HEAAC/eAAC/eAAC+)16bit PCM samples
\param[in] hAACDecoder       : AAC解码器句柄
\param[in] ppInbufPtr        : 数据流指针的地址
\param[in/out] pBytesLeft    : BytesLeft指针 indicates bitstream numbers at input buffer,indicates the left bytes
\param[in] pOutPcm           : 输出cm buffer的地址,pcm 数据格式为: L/L/L/... R/R/R/...
\retval :: SUCCESS           : 成功
\retval :: ERROR_CODE        : 失败, 返回错误码
\see \n
N/A
*/
AR_S32  AACDecodeFrame(HAACDecoder hAACDecoder, AR_U8** ppInbufPtr, AR_S32* pBytesLeft, AR_S16* pOutPcm);

/**
\brief 获取帧信息
\param[in] hAACDecoder       : AAC解码器句柄
\param[out] aacFrameInfo     : 帧信息结构体指针
\retval :: AR_SUCCESS        : 成功
\retval :: ERROR_CODE        : 失败, 返回错误码
\see \n
N/A
*/
AR_S32 AACGetLastFrameInfo(HAACDecoder hAACDecoder, AACFrameInfo* aacFrameInfo);

/**
\brief 设置eos标志
\param[in] hAACDecoder       : AAC解码器句柄
\param[in] s32Eosflag        : 结束标志
\retval :: AR_SUCCESS        : 成功
\retval :: ERROR_CODE        : 失败, 返回错误码
\see \n
N/A
*/
AR_S32 AACDecoderSetEosFlag(HAACDecoder hAACDecoder, AR_S32 s32Eosflag);

/**
\brief 刷新内部编解码器状态
\param[in] hAACDecoder       : AAC解码器句柄
\retval :: AR_SUCCESS        : 成功
\retval :: ERROR_CODE        : 失败, 返回错误码
\see \n
N/A
*/
AR_S32  AACFlushCodec(HAACDecoder hAACDecoder);

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

/**
 * @}
 */

#endif  /* _AACDEC_H */
