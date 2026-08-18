/**
 * @file    hal_aacdec.h
 * @brief   aac编码数据结构和API
 * @author  Artosyn Software Team
 * @version 0.0.1
 * @date   2021/04/21
 * @license   2021-2025, Artosyn. Co., Ltd.
**/
#ifndef _HAL_AACDEC_H
#define _HAL_AACDEC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hal_type.h"


/********************************Macro Definition********************************/
/** \addtogroup      AACDEC */
/** @{ */  /** <!-- [AACDEC] */

#ifndef AR_AAC_MAX_NCHANS
#define AR_AAC_MAX_NCHANS      2
#endif
#define AR_AAC_MAX_NSAMPS      1024
#define AR_AAC_MAINBUF_SIZE    (768 * AR_AAC_MAX_NCHANS)  /**<according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3),6144 bits =  768 bytes per SCE or CCE-I,12288 bits = 1536 bytes per CPE*/

#define AR_AAC_NUM_PROFILES    3
#define AR_AAC_PROFILE_MP      0
#define AR_AAC_PROFILE_LC      1
#define AR_AAC_PROFILE_SSR     2


/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      AACDEC */
/** @{ */  /** <!-- [AACDEC] */

/**Defines AACDEC error code*/
typedef enum
{
    AR_ERR_AAC_NONE                          =   0,        /**<no decode error*/
    AR_ERR_AAC_INDATA_UNDERFLOW              =  -1,        /**<not enough input data*/
    AR_ERR_AAC_NULL_POINTER                  =  -2,        /**<null pointer*/
    AR_ERR_AAC_INVALID_ADTS_HEADER           =  -3,        /**<invalid adts header*/
    AR_ERR_AAC_INVALID_ADIF_HEADER           =  -4,        /**<invalid adif header*/
    AR_ERR_AAC_INVALID_FRAME                 =  -5,        /**<invalid frame*/
    AR_ERR_AAC_MPEG4_UNSUPPORTED             =  -6,        /**<upsupport mpeg4 format*/
    AR_ERR_AAC_CHANNEL_MAP                   =  -7,        /**<channel map error*/
    AR_ERR_AAC_SYNTAX_ELEMENT                =  -8,        /**<element error*/
    AR_ERR_AAC_DEQUANT                       =  -9,        /**<dequant error*/
    AR_ERR_AAC_STEREO_PROCESS                = -10,        /**<stereo process error*/
    AR_ERR_AAC_PNS                           = -11,        /**<pns process error*/
    AR_ERR_AAC_SHORT_BLOCK_DEINT             = -12,        /**<reserved*/
    AR_ERR_AAC_TNS                           = -13,        /**<TNS process error*/
    AR_ERR_AAC_IMDCT                         = -14,        /**<IMDCT process error*/
    AR_ERR_AAC_NCHANS_TOO_HIGH               = -15,        /**<unsupport mutil channel*/
    AR_ERR_AAC_SBR_INIT                      = -16,        /**<SBR init error*/
    AR_ERR_AAC_SBR_BITSTREAM                 = -17,        /**<SBR bitstream error*/
    AR_ERR_AAC_SBR_DATA                      = -18,        /**<SBR data error*/
    AR_ERR_AAC_SBR_PCM_FORMAT                = -19,        /**<SBR pcm data error*/
    AR_ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,        /**<unsupport SBR multi channel*/
    AR_ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,        /**<SBR invalid samplerate*/
    AR_ERR_AAC_RAWBLOCK_PARAMS               = -22,        /**<invalid RawBlock params*/
    AR_ERR_AAC_PS_INIT                       = -23,        /**<PS init error*/
    AR_ERR_AAC_CH_MAPPING                    = -24,
    AR_ERR_UNKNOWN                           = -9999,      /**<reserved*/
    AR_ERR_AAC_OUT_OF_MEMORY                 = 2,          /*!< Heap returned NULL pointer. Output buffer is invalid. */
    AR_ERR_AAC_UNKNOWN                       = 5,          /*!< Error condition is of unknown reason, or from a another module. Output buffer is invalid. */
    AR_ERR_AAC_TRANSPORT_SYNC_ERROR          = 4097,       /*!< The transport decoder had syncronisation problems. Do not exit decoding. Just feed new bitstream data. */
    AR_ERR_AAC_NOT_ENOUGH_BITS               = 4098,       /*!< The input buffer ran out of bits. */
    AR_ERR_AAC_TRANSPORT_FATAL_ERROR         = 4099,       /*!< The transport decoder occut fatal error. Reset Tranport */
    AR_ERR_AAC_INVALID_HANDLE                = 8193,       /*!< The handle passed to the function call was invalid (NULL). */
    AR_ERR_AAC_UNSUPPORTED_AOT               = 8194,       /*!< The AOT found in the configuration is not supported. */
    AR_ERR_AAC_UNSUPPORTED_FORMAT            = 8195,       /*!< The bitstream format is not supported.  */
    AR_ERR_AAC_UNSUPPORTED_ER_FORMAT         = 8196,       /*!< The error resilience tool format is not supported. */
    AR_ERR_AAC_UNSUPPORTED_EPCONFIG          = 8197,       /*!< The error protection format is not supported. */
    AR_ERR_AAC_UNSUPPORTED_MULTILAYER        = 8198,       /*!< More than one layer for AAC scalable is not supported. */
    AR_ERR_AAC_UNSUPPORTED_CHANNELCONFIG     = 8199,       /*!< The channel configuration (either number or arrangement) is not supported. */
    AR_ERR_AAC_UNSUPPORTED_SAMPLINGRATE      = 8200,       /*!< The sample rate specified in the configuration is not supported. */
    AR_ERR_AAC_INVALID_SBR_CONFIG            = 8201,       /*!< The SBR configuration is not supported. */
    AR_ERR_AAC_SET_PARAM_FAIL                = 8202,       /*!< The parameter could not be set. Either the value was out of range or the parameter does not exist. */
    AR_ERR_AAC_NEED_TO_RESTART               = 8203,       /*!< The decoder needs to be restarted, since the requiered configuration change cannot be performed. */
    AR_ERR_AAC_TRANSPORT_ERROR               = 16385,      /*!< The transport decoder encountered an unexpected error. */
    AR_ERR_AAC_PARSE_ERROR                   = 16386,      /*!< Error while parsing the bitstream. Most probably it is corrupted, or the system crashed. */
    AR_ERR_AAC_UNSUPPORTED_EXTENSION_PAYLOAD = 16387,      /*!< Error while parsing the extension payload of the bitstream. The extension payload type found is not supported. */
    AR_ERR_AAC_DECODE_FRAME_ERROR            = 16388,      /*!< The parsed bitstream value is out of range. Most probably the bitstream is corrupt, or the system crashed. */
    AR_ERR_AAC_CRC_ERROR                     = 16389,      /*!< The embedded CRC did not match. */
    AR_ERR_AAC_INVALID_CODE_BOOK             = 16390,      /*!< An invalid codebook was signalled. Most probably the bitstream is corrupt, or the system crashed. */
    AR_ERR_AAC_UNSUPPORTED_PREDICTION        = 16391,      /*!< Predictor found, but not supported in the AAC Low Complexity profile. Most probably the bitstream is corrupt, or has a wrong format. */
    AR_ERR_AAC_UNSUPPORTED_CCE               = 16392,      /*!< A CCE element was found which is not supported. Most probably the bitstream is corrupt, or has a wrong format. */
    AR_ERR_AAC_UNSUPPORTED_LFE               = 16393,      /*!< A LFE element was found which is not supported. Most probably the bitstream is corrupt, or has a wrong format. */
    AR_ERR_AAC_UNSUPPORTED_GAIN_CONTROL_DATA = 16394,      /*!< Gain control data found but not supported. Most probably the bitstream is corrupt, or has a wrong format. */
    AR_ERR_AAC_UNSUPPORTED_SBA               = 16395,      /*!< SBA found, but currently not supported in the BSAC profile. */
    AR_ERR_AAC_TNS_READ_ERROR                = 16396,      /*!< Error while reading TNS data. Most probably the bitstream is corrupt or the system crashed. */
    AR_ERR_AAC_RVLC_ERROR                    = 16397,      /*!< Error while decoding error resillient data. */
    AR_ERR_AAC_ANC_DATA_ERROR                = 32769,      /*!< Non severe error concerning the ancillary data handling. */
    AR_ERR_AAC_TOO_SMALL_ANC_BUFFER          = 32770,      /*!< The registered ancillary data buffer is too small to receive the parsed data. */
    AR_ERR_AAC_TOO_MANY_ANC_ELEMENTS         = 32771,      /*!< More than the allowed number of ancillary data elements should be written to buffer. */
} ENUM_AR_AACDEC_ERR_E;

typedef struct _STRU_AACFrameInfo
{
    int bitRate;
    int nChans;                      /**<channels,range:1,2*/
    int sampRateCore;                /**<inner sample rate*/
    int sampRateOut;                 /**<output samplerate*/
    int bitsPerSample;               /**<bitwidth ,range:16*/
    int outputSamps;                 /**<output samples*/
    int profile;                     /**<profile*/
    int tnsUsed;                     /**<tns tools*/
    int pnsUsed;                     /**<pns tools*/
} STRU_AACFrameInfo;

typedef enum
{
    AR_AACDEC_ADTS = 0,
    AR_AACDEC_LOAS = 1,
    AR_AACDEC_LATM_MCP1 = 2,
} ENUM_AACDECTransportType;

typedef void* AR_HAACDecoder;

typedef struct _arAACDEC_VERSION_S
{
    AR_U8 aVersion[64];
} STRU_AACDEC_VERSION_S;


/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/** \addtogroup      AACDEC */
/** @{ */  /** <!--  [AACDEC]*/

/**
\brief Get version information.
N/A
\param[in] pVersion    :   version describe struct
\retval ::AR_SUCCESS   :   Success
\retval ::AR_FAILURE   :   pVersion is NULL, return AR_FAILURE
\see \n
N/A
*/
AR_S32 ar_aacdec_get_version(STRU_AACDEC_VERSION_S* pVersion);

/**
\brief create and initial decoder device.
N/A
\param[in] enTranType   : transport type
\retval ::HAACDecoder   : init success, return non-NULL handle.
\retval ::NULL          : init failure, return NULL
\see \n
N/A
*/
AR_HAACDecoder ar_aac_init_decoder(ENUM_AACDECTransportType enTranType);

/**
\brief destroy AAC-Decoder, free the memory.
N/A
\param[in] hAACDecoder  : AAC-Decoder handle
\see \n
N/A
*/
AR_VOID ar_aac_free_decoder(AR_HAACDecoder hAACDecoder);

/**
\brief set RawMode before decode Raw Format aac bitstream(Reserved API, unused now.)
N/A
\param[in] hAACDecoder  : AAC-Decoder handle
\param[in] nChans       : inout channels
\param[in] sampRate     : input sampelrate
\retval ::AR_FAILURE    : RESERVED API, always return AR_FAILURE.
\see \n
N/A
*/
AR_S32  ar_aac_set_raw_mode(AR_HAACDecoder hAACDecoder, AR_S32 nChans, AR_S32 sampRate);

/**
\brief look for valid AAC sync header
N/A
\param[in] hAACDecoder      : AAC-Decoder handle
\param[in/out] ppInbufPtr   : address of the pointer of start-point of the bitstream
\param[in/out] pBytesLeft   : pointer to BytesLeft that indicates bitstream numbers at input buffer
\retval ::<0                : err, always return ERR_AAC_INDATA_UNDERFLOW
\retval ::other             : Success, return number bytes of current frame
\see \n
N/A
*/
AR_S32 ar_aac_decode_find_sync_header(AR_HAACDecoder hAACDecoder, AR_U8** ppInbufPtr, AR_S32* pBytesLeft);

/**
\brief decoding AAC frame and output 1024(LC) OR 2048(HEAAC/eAAC/eAAC+) 16bit PCM samples per channel.
\param[in] hAACDecoder       : AAC-Decoder handle
\param[in] ppInbufPtr        : address of the pointer of start-point of the bitstream
\param[in/out] pBytesLeft    : pointer to BytesLeft that indicates bitstream numbers at input buffer,indicates the left bytes
\param[in] pOutPcm           : the address of the out pcm buffer,pcm data in noninterlaced fotmat: L/L/L/... R/R/R/...
\retval :: SUCCESS           : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
AR_S32  ar_aac_decode_frame(AR_HAACDecoder hAACDecoder, AR_U8** ppInbufPtr, AR_S32* pBytesLeft, AR_S16* pOutPcm);

/**
\brief get the frame information.
\param[in] hAACDecoder       : AAC-Decoder handle
\param[out] aacFrameInfo     : frame information
\retval :: AR_SUCCESS        : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
AR_S32 ar_aac_get_last_frame_info(AR_HAACDecoder hAACDecoder, STRU_AACFrameInfo* aacFrameInfo);

/**
\brief set eosflag.
\param[in] hAACDecoder       : AAC-Decoder handle
\param[in] s32Eosflag        : end flag
\retval :: AR_SUCCESS        : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
AR_S32 ar_aac_decoder_set_eos_flag(AR_HAACDecoder hAACDecoder, AR_S32 s32Eosflag);

/**
\brief flush internal codec state (after seeking, for example)
\attention NEW API
\param[in] hAACDecoder       : AAC-Decoder handle
\retval :: AR_SUCCESS        : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
AR_S32  ar_aac_flush_codec(AR_HAACDecoder hAACDecoder);

/** @} */  /** <!-- ==== API declaration end ==== */


#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#endif  /* _AACDEC_H */
