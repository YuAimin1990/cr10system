
#ifndef __AUDIO_AAC_ADP_H__
#define __AUDIO_AAC_ADP_H__

#include <stdio.h>

#include "mpi_audio.h"
#include "ar_comm_aenc.h"
#include "ar_comm_adec.h"
#include "ar_comm_aio.h"
#include "aacenc.h"
#include "aacdec.h"

/*samples per frame for AACLC and aacPlus */
#define AACLD_SAMPLES_PER_FRAME         512
#define AACLC_SAMPLES_PER_FRAME         1024
#define AACPLUS_SAMPLES_PER_FRAME       2048

/*max length of AAC stream by bytes */
#define MAX_AAC_MAINBUF_SIZE    768*2

typedef enum arAAC_TYPE_E
{
	AAC_TYPE_AACLC      = 0,            /* AAC LC */
	AAC_TYPE_EAAC       = 1,            /* eAAC  (HEAAC or AAC+  or aacPlusV1) */
	AAC_TYPE_EAACPLUS   = 2,            /* eAAC+ (AAC++ or aacPlusV2) */
	AAC_TYPE_AACLD      = 3,
	AAC_TYPE_AACELD     = 4,
	AAC_TYPE_BUTT,
} AAC_TYPE_E;

typedef enum arAAC_BPS_E
{
	AAC_BPS_8K      = 8000,
	AAC_BPS_16K     = 16000,
	AAC_BPS_22K     = 22000,
	AAC_BPS_24K     = 24000,
	AAC_BPS_32K     = 32000,
	AAC_BPS_48K     = 48000,
	AAC_BPS_64K     = 64000,
	AAC_BPS_96K     = 96000,
	AAC_BPS_128K    = 128000,
	AAC_BPS_256K    = 256000,
	AAC_BPS_320K    = 320000,
	AAC_BPS_BUTT
} AAC_BPS_E;

typedef enum arAAC_TRANS_TYPE_E
{
	AAC_TRANS_TYPE_ADTS = 0,
	AAC_TRANS_TYPE_LOAS= 1,
	AAC_TRANS_TYPE_LATM_MCP1 = 2,
	AAC_TRANS_TYPE_BUTT
}AAC_TRANS_TYPE_E;


typedef struct arAAC_FRAME_INFO_S
{
	AR_S32 s32Samplerate;   /* sample rate*/
	AR_S32 s32BitRate;                  /* bitrate */
	AR_S32 s32Profile;                  /* profile*/
	AR_S32 s32TnsUsed;                  /* TNS Tools*/
	AR_S32 s32PnsUsed;                  /* PNS Tools*/
} AAC_FRAME_INFO_S;

/*
    AAC Commendatory Parameter:
    Sampling Rate(HZ)    LC BitRate(Kbit/s)    EAAC BitRate (Kbit/s)    EAAC+ BitRate (Kbit/s)
    48000                128                    48                        32,24
    44100                128                    48                        32,24
    32000                96                    22                        16
    24000                64
    22050                64
    16000                48
*/

typedef struct arAENC_ATTR_AAC_S
{
	AAC_TYPE_E          enAACType;   /* AAC profile type */
	AAC_BPS_E           enBitRate;   /* AAC bitrate (LC:16~320, EAAC:24~128, EAAC+:16~64, AACLD:16~320, AACELD:32~320)*/
	AUDIO_SAMPLE_RATE_E enSmpRate;   /* AAC sample rate (LC:8~48, EAAC:16~48, EAAC+:16~48, AACLD:8~48, AACELD:8~48)*/
	AUDIO_BIT_WIDTH_E   enBitWidth;  /* AAC bit width (only support 16bit)*/
	AUDIO_SOUND_MODE_E  enSoundMode; /* sound mode of inferent audio frame */
	AAC_TRANS_TYPE_E    enTransType;

	AR_S16              s16BandWidth; /* targeted audio bandwidth in Hz (0 or 1000~enSmpRate/2), the default is 0*/

}AENC_ATTR_AAC_S;

typedef struct arAENC_AAC_ENCODER_S
{
	AAC_ENCODER_S       *pstAACState;
	AENC_ATTR_AAC_S     stAACAttr;
} AENC_AAC_ENCODER_S;

typedef struct arADEC_ATTR_AAC_S
{
	AAC_TRANS_TYPE_E enTransType;
}ADEC_ATTR_AAC_S;

typedef struct arADEC_AAC_DECODER_S
{
	HAACDecoder         pstAACState;
	ADEC_ATTR_AAC_S     stAACAttr;
} ADEC_AAC_DECODER_S;


AR_S32 AENC_AacInit(AR_VOID);

AR_S32 OpenAACEncoder(AR_VOID *pEncoderAttr, AR_VOID **ppEncoder);

AR_S32 EncodeAACFrm(AR_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                    AR_U8 *pu8Outbuf,  AR_U32 *pu32OutLen);

AR_S32 CloseAACEncoder(AR_VOID *pEncoder);


AR_S32 ADEC_AacInit(AR_VOID);

AR_S32 OpenAACDecoder(AR_VOID *pDecoderAttr, AR_VOID **ppDecoder);

AR_S32 DecodeAACFrm(AR_VOID *pDecoder, AR_U8 **pu8Inbuf,AR_S32 *ps32LeftByte,
                        AR_U16 *pu16Outbuf,AR_U32 *pu32OutLen,AR_U32 *pu32Chns);

AR_S32 GetAACFrmInfo(AR_VOID *pDecoder, AR_VOID *pInfo);

AR_S32 CloseAACDecoder(AR_VOID *pDecoder);

AR_S32 ResetAACDecoder(AR_VOID *pDecoder);

#endif

