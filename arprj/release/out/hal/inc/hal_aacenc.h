/**
 * @file    hal_aacenc.h
 * @brief   aac编码数据结构和API
 * @author  Artosyn Software Team
 * @version 0.0.1
 * @date   2021/04/21
 * @license   2021-2025, Artosyn. Co., Ltd.
**/

#ifndef _HAL_AACENC_H
#define _HAL_AACENC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif     /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hal_type.h"

/********************************Macro Definition********************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!-- [AACENC] */

#ifdef MONO_ONLY
#define MAX_CHANNELS 1 /**<aacenc encoder channels*/
#else
#define MAX_CHANNELS 2
#endif

#define AACENC_BLOCKSIZE 1024 /**<aacenc blocksize*/
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!-- [AACENC] */

typedef enum
{
    AR_AACENC_OK                     = 0x0000,  /*!< No error happened. All fine. */

    AR_AACENC_INVALID_HANDLE         = 0x0020,  /*!< Handle passed to function call was invalid. */
    AR_AACENC_MEMORY_ERROR           = 0x0021,  /*!< Memory allocation failed. */
    AR_AACENC_UNSUPPORTED_PARAMETER  = 0x0022,  /*!< Parameter not available. */
    AR_AACENC_INVALID_CONFIG         = 0x0023,  /*!< Configuration not provided. */

    AR_AACENC_INIT_ERROR             = 0x0040,  /*!< General initialization error. */
    AR_AACENC_INIT_AAC_ERROR         = 0x0041,  /*!< AAC library initialization error. */
    AR_AACENC_INIT_SBR_ERROR         = 0x0042,  /*!< SBR library initialization error. */
    AR_AACENC_INIT_TP_ERROR          = 0x0043,  /*!< Transport library initialization error. */
    AR_AACENC_INIT_META_ERROR        = 0x0044,  /*!< Meta data library initialization error. */

    AR_AACENC_ENCODE_ERROR           = 0x0060,  /*!< The encoding process was interrupted by an unexpected error. */

    AR_AACENC_ENCODE_EOF             = 0x0080   /*!< End of file reached. */

} ENUM_AR_AACENC_ERROR_E;

/**Defines AACENC quality*/
typedef enum
{
    AR_AU_QualityExcellent = 0,
    AR_AU_QualityHigh   = 1,
    AR_AU_QualityMedium = 2,
    AR_AU_QualityLow = 3,
} ENUM_AuQuality;

/**Defines AACENC format*/
typedef enum
{
    AR_AACLC = 0,              /**<AAC-LC format*/
    AR_EAAC = 1,               /**<HEAAC or AAC+  or aacPlusV1*/
    AR_EAACPLUS = 2,           /**<AAC++ or aacPlusV2*/
    AR_AACLD = 3,              /**<AAC LD(Low Delay)*/
    AR_AACELD = 4,             /**<AAC ELD(Low Delay)*/
} ENUM_AuEncoderFormat;

/**Defines AACENC container*/
typedef enum
{
    AR_AACENC_ADTS = 0,
    AR_AACENC_LOAS = 1,
    AR_AACENC_LATM_MCP1 = 2,
} ENUM_AACENCTransportType;

/**Defines AACENC configuration*/
typedef struct
{
    ENUM_AuQuality       quality;
    ENUM_AuEncoderFormat coderFormat;
    AR_S16          bitsPerSample;
    AR_S32          sampleRate;    /**<audio file sample rate */
    AR_S32          bitRate;       /**<encoder bit rate in bits/sec */
    AR_S16          nChannelsIn;   /**<number of channels on input (1,2) */
    AR_S16          nChannelsOut;  /**<number of channels on output (1,2) */
    AR_S16          bandWidth;     /**<targeted audio bandwidth in Hz */
    ENUM_AACENCTransportType transtype;
} STRU_AACENC_CONFIG;

/**Defines AACENC version*/
typedef struct
{
    AR_U8 aVersion[64];
} STRU_AACENC_VERSION_S;

//typedef AR_U32 AAC_ENCODER_S;

typedef AR_VOID AR_AAC_ENCODER_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/** \addtogroup      AACENC */
/** @{ */  /** <!--  [AACENC] */

/**
\brief Get version information.
N/A
\param[in] pVersion       version describe struct
\retval ::AR_SUCCESS   : Success
\retval ::AR_FAILURE          : FAILURE
\see \n
N/A
*/
AR_S32  ar_aacenc_get_version(STRU_AACENC_VERSION_S* pVersion);

/**
\brief get reasonable default configuration.
N/A
\param[in] pstConfig    pointer to an configuration information structure
\retval ::AR_SUCCESS   : Success
\retval ::AR_FAILURE          : FAILURE
\see \n
N/A
*/
AR_S32  ar_aacenc_init_default_config(STRU_AACENC_CONFIG* pstConfig);

/**
\brief allocate and initialize a new encoder instance.
N/A
\param[in] phAacPlusEnc    pointer to an configuration information structure
\param[in] pstConfig    pointer to an configuration information structure
\retval ::AR_SUCCESS   : Success
\retval ::AR_FAILURE   : FAILURE
\see \n
N/A
*/
AR_S32  ar_aac_encoder_open(AR_AAC_ENCODER_S** phAacPlusEnc, STRU_AACENC_CONFIG* pstConfig);

/**
\brief allocate and initialize a new encoder instance
N/A
\param[in] hAacPlusEnc    pointer to an configuration information structure
\param[in] ps16PcmBuf    BLOCKSIZE*nChannels audio samples,interleaved
\param[in] pu8Outbuf    pointer to output buffer,(must be 6144/8*MAX_CHANNELS bytes large)
\param[in] ps32NumOutBytes    number of bytes in output buffer after processing
\retval ::AR_SUCCESS   : Success
\retval ::AR_FAILURE   : FAILURE
\see \n
N/A
*/
AR_S32  ar_aac_encoder_frame(AR_AAC_ENCODER_S* hAacPlusEnc, AR_S16* ps16PcmBuf, AR_U32 nb_pcm,
                        AR_U8* pu8Outbuf, AR_S32* ps32NumOutBytes);

/**
\brief close encoder device.
N/A
\param[in] hAacPlusEnc    pointer to an configuration information structure
\retval N/A
\see \n
N/A
*/
AR_VOID ar_aac_encoder_close (AR_AAC_ENCODER_S* hAacPlusEnc);
/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif     /* __cpluscplus */
#endif  /* __cpluscplus */

#endif  /* _AACENC_H */
