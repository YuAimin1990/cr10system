/**
 * @file hal_comm_aio.h
 * @brief  hal common audio define
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/


#ifndef __HAL_COMM_AIO_H__
#define __HAL_COMM_AIO_H__

#include "hal_errno.h"
#include "hal_type.h"
#include "hal_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*       ai                         ao
 * dev0: inner codec                inner codec
 * dev1: i2s0                       i2s0
 * dev2: i2s1                       i2s1
 * dev3: i2s2                       i2s2
 * dev4: i2s_s0                     i2s_s0
 * dev5: i2s_s1                     i2s_s1
 */
#define AR_AI_DEV_MAX_NUM          6
#define AR_AO_DEV_MIN_NUM          0
#define AR_AO_DEV_MAX_NUM          6
#define AR_AIO_MAX_NUM             6

#define AR_AI_MAX_CHN_NUM          4
#define AR_AO_MAX_CHN_NUM          4
#define AR_AO_SYSCHN_CHNID         (AR_AO_MAX_CHN_NUM - 1)

#define AR_AIO_MAX_CHN_NUM         ((AR_AO_MAX_CHN_NUM > AR_AI_MAX_CHN_NUM) ? AR_AO_MAX_CHN_NUM:AR_AI_MAX_CHN_NUM)


#define AR_MAX_AUDIO_FRAME_NUM    300       /*max count of audio frame in Buffer */
#define AR_MAX_AUDIO_POINT_BYTES  4        /*max bytes of one sample point(now 32bit max)*/

#define AR_MAX_VOICE_POINT_NUM    480      /*max sample per frame for voice encode */

#define AR_MAX_AUDIO_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/
#define AR_MAX_AO_POINT_NUM       2048     /* support 4096 framelen*/
#define AR_MIN_AUDIO_POINT_NUM    80       /*min sample per frame*/
#define AR_MAX_AI_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/

/*max length of audio frame by bytes, one frame contain many sample point */
#define AR_MAX_AUDIO_FRAME_LEN    (AR_MAX_AUDIO_POINT_BYTES*AR_MAX_AO_POINT_NUM)

/*max length of audio stream by bytes */
#define AR_MAX_AUDIO_STREAM_LEN   AR_MAX_AUDIO_FRAME_LEN

#define AR_MAX_AI_USRFRM_DEPTH     30      /*max depth of user frame buf */

#define AR_MAX_AUDIO_FILE_PATH_LEN	256
#define AR_MAX_AUDIO_FILE_NAME_LEN	256

/*The VQE EQ Band num.*/
#define AR_VQE_EQ_BAND_NUM  10
#define AR_VQE_DRC_SECNUM	5

#define AR_AI_RECORDVQE_MASK_HPF		0x1
#define AR_AI_RECORDVQE_MASK_RNR		0x2
#define AR_AI_RECORDVQE_MASK_HDR		0x4
#define AR_AI_RECORDVQE_MASK_DRC		0x8
#define AR_AI_RECORDVQE_MASK_EQ		0x10
#define AR_AI_RECORDVQE_MASK_AGC		0x20

#define AR_AI_TALKVQE_MASK_HPF		0x1
#define AR_AI_TALKVQE_MASK_AEC		0x2
#define AR_AI_TALKVQE_MASK_AGC		0x8
#define AR_AI_TALKVQE_MASK_EQ		0x10
#define AR_AI_TALKVQE_MASK_ANR		0x20

#define AR_AO_VQE_MASK_HPF			0x1
#define AR_AO_VQE_MASK_ANR			0x2
#define AR_AO_VQE_MASK_AGC			0x4
#define AR_AO_VQE_MASK_EQ			0x8

/* We just coyp this value of payload type from RTP/RTSP definition */ //from ar_common.h
typedef enum
{
    AR_PT_PCMU          = 0,
    AR_PT_1016          = 1,
    AR_PT_G721          = 2,
    AR_PT_GSM           = 3,
    AR_PT_G723          = 4,
    AR_PT_DVI4_8K       = 5,
    AR_PT_DVI4_16K      = 6,
    AR_PT_LPC           = 7,
    AR_PT_PCMA          = 8,
    AR_PT_G722          = 9,
    AR_PT_S16BE_STEREO  = 10,
    AR_PT_S16BE_MONO    = 11,
    AR_PT_QCELP         = 12,
    AR_PT_CN            = 13,
    AR_PT_MPEGAUDIO     = 14,
    AR_PT_G728          = 15,
    AR_PT_DVI4_3        = 16,
    AR_PT_DVI4_4        = 17,
    AR_PT_G729          = 18,
    AR_PT_G711A         = 19,
    AR_PT_G711U         = 20,
    AR_PT_G726          = 21,
    AR_PT_G729A         = 22,
    AR_PT_LPCM          = 23,
    AR_PT_CelB          = 25,
    AR_PT_JPEG          = 26,
    AR_PT_CUSM          = 27,
    AR_PT_NV            = 28,
    AR_PT_PICW          = 29,
    AR_PT_CPV           = 30,
    AR_PT_H261          = 31,
    AR_PT_MPEGVIDEO     = 32,
    AR_PT_MPEG2TS       = 33,
    AR_PT_H263          = 34,
    AR_PT_SPEG          = 35,
    AR_PT_MPEG2VIDEO    = 36,
    AR_PT_AAC           = 37,
    AR_PT_WMA9STD       = 38,
    AR_PT_HEAAC         = 39,
    AR_PT_PCM_VOICE     = 40,
    AR_PT_PCM_AUDIO     = 41,
    AR_PT_MP3           = 43,
    AR_PT_ADPCMA        = 49,
    AR_PT_AEC           = 50,
    AR_PT_X_LD          = 95,
    AR_PT_H264          = 96,
    AR_PT_D_GSM_HR      = 200,
    AR_PT_D_GSM_EFR     = 201,
    AR_PT_D_L8          = 202,
    AR_PT_D_RED         = 203,
    AR_PT_D_VDVI        = 204,
    AR_PT_D_BT656       = 220,
    AR_PT_D_H263_1998   = 221,
    AR_PT_D_MP1S        = 222,
    AR_PT_D_MP2P        = 223,
    AR_PT_D_BMPEG       = 224,
    AR_PT_MP4VIDEO      = 230,
    AR_PT_MP4AUDIO      = 237,
    AR_PT_VC1           = 238,
    AR_PT_JVC_ASF       = 255,
    AR_PT_D_AVI         = 256,
    AR_PT_DIVX3         = 257,
    AR_PT_AVS             = 258,
    AR_PT_REAL8         = 259,
    AR_PT_REAL9         = 260,
    AR_PT_VP6             = 261,
    AR_PT_VP6F             = 262,
    AR_PT_VP6A             = 263,
    AR_PT_SORENSON          = 264,
    AR_PT_H265          = 265,
    AR_PT_VP8             = 266,
    AR_PT_MVC             = 267,
    AR_PT_PNG           = 268,

    AR_PT_AMR           = 1001,
    AR_PT_MJPEG         = 1002,
    AR_PT_AMRWB         = 1003,
    AR_PT_PRORES        = 1006,
    AR_PT_OPUS          = 1007,
    AR_PT_BUTT
} ENUM_PAYLOAD_TYPE;


typedef enum
{
    AR_AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate*/
    AR_AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12K samplerate*/
    AR_AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate*/
    AR_AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate*/
    AR_AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate*/
    AR_AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate*/
    AR_AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate*/
    AR_AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate*/
    AR_AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate*/
    AR_AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64K samplerate*/
    AR_AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96K samplerate*/
    AR_AUDIO_SAMPLE_RATE_BUTT,
} ENUM_AUDIO_SAMPLE_RATE;

typedef enum
{
    AR_AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    AR_AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width*/
    AR_AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width*/
    AR_AUDIO_BIT_WIDTH_32  = 3,   /* 32bit width*/
    AR_AUDIO_BIT_WIDTH_BUTT,
} ENUM_AUDIO_BIT_WIDTH;

typedef enum
{
    AR_AIO_MODE_I2S_MASTER  = 0,   /* AIO I2S master mode */
    AR_AIO_MODE_I2S_SLAVE,         /* AIO I2S slave mode */
    AR_AIO_MODE_PCM_SLAVE_STD,     /* AIO PCM slave standard mode */
    AR_AIO_MODE_PCM_SLAVE_NSTD,    /* AIO PCM slave non-standard mode */
    AR_AIO_MODE_PCM_MASTER_STD,    /* AIO PCM master standard mode */
    AR_AIO_MODE_PCM_MASTER_NSTD,   /* AIO PCM master non-standard mode */
    AR_AIO_MODE_BUTT
} ENUM_AIO_MODE;

typedef enum
{
    AR_AIO_I2STYPE_INNERCODEC = 0, /* AIO I2S connect inner audio CODEC */
    AR_AIO_I2STYPE_INNERHDMI,      /* AIO I2S connect Inner HDMI */
    AR_AIO_I2STYPE_EXTERN,         /* AIO I2S connect extern hardware */
} ENUM_AIO_I2STYPE;

typedef enum
{
    AR_AUDIO_SOUND_MODE_MONO   =0,/*mono*/
    AR_AUDIO_SOUND_MODE_STEREO =1,/*stereo*/
    AR_AUDIO_SOUND_MODE_BUTT
} ENUM_AUDIO_SOUND_MODE;

/*
An example of the packing scheme for G726-32 codewords is as shown, and bit A3 is the least significant bit of the first codeword:
RTP G726-32:
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|B B B B|A A A A|D D D D|C C C C| ...
|0 1 2 3|0 1 2 3|0 1 2 3|0 1 2 3|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

MEDIA G726-32:
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|A A A A|B B B B|C C C C|D D D D| ...
|3 2 1 0|3 2 1 0|3 2 1 0|3 2 1 0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/
typedef enum
{
    AR_G726_16K = 0,       /* G726 16kbps, see RFC3551.txt  4.5.4 G726-16 */
    AR_G726_24K,           /* G726 24kbps, see RFC3551.txt  4.5.4 G726-24 */
    AR_G726_32K,           /* G726 32kbps, see RFC3551.txt  4.5.4 G726-32 */
    AR_G726_40K,           /* G726 40kbps, see RFC3551.txt  4.5.4 G726-40 */
    AR_MEDIA_G726_16K,     /* G726 16kbps for ASF ... */
    AR_MEDIA_G726_24K,     /* G726 24kbps for ASF ... */
    AR_MEDIA_G726_32K,     /* G726 32kbps for ASF ... */
    AR_MEDIA_G726_40K,     /* G726 40kbps for ASF ... */
    AR_G726_BUTT,
} ENUM_G726_BPS;

typedef enum
{
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */

    AR_ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    AR_ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    AR_ADPCM_TYPE_ORG_DVI4,
    AR_ADPCM_TYPE_BUTT,
} ENUM_ADPCM_TYPE;

#define AR_AI_EXPAND  0x01
#define AR_AI_CUT     0x02

typedef struct
{
    ENUM_AUDIO_SAMPLE_RATE  enSamplerate;   /* sample rate */
    ENUM_AUDIO_BIT_WIDTH    enBitwidth;     /* bitwidth */
    ENUM_AIO_MODE           enWorkmode;     /* master or slave mode */
    ENUM_AUDIO_SOUND_MODE   enSoundmode;    /* momo or steror */
    AR_U32                  u32EXFlag;      /* expand 8bit to 16bit,use AI_EXPAND(only valid for AI 8bit),use AI_CUT(only valid for extern Codec for 24bit) */
    AR_U32                  u32FrmNum;      /* frame num in buf[2,MAX_AUDIO_FRAME_NUM] */
    AR_U32                  u32PtNumPerFrm; /* point num per frame (80/160/240/320/480/1024/2048)
                                                (ADPCM IMA should add 1 point, AMR only support 160) */
    AR_U32                  u32ChnCnt;      /* channle number on FS, valid value:1/2/4/8 */
    AR_U32                  u32ClkSel;      /* 0: AI and AO clock is separate
                                                 1: AI and AO clock is inseparate, AI use AO's clock
                                              */
    ENUM_AIO_I2STYPE        enI2sType;      /* i2s type */
} STRU_AIO_ATTR;

typedef struct
{
    AR_U32 u32UsrFrmDepth;
} STRU_AI_CHN_PARAM;

typedef struct
{
    ENUM_AUDIO_BIT_WIDTH   enBitwidth;     /*audio frame bitwidth*/
    ENUM_AUDIO_SOUND_MODE  enSoundmode;    /*audio frame momo or stereo mode*/
    AR_U8*  u64VirAddr[2];
    AR_U64  u64PhyAddr[2];
    AR_U64  u64TimeStamp;                /*audio frame timestamp*/
    AR_U32  u32Seq;                      /*audio frame seq*/
    AR_U32  u32Len;                      /*data lenth per channel in frame*/
    AR_U32  u32PoolId[2];
} STRU_AUDIO_FRAME;

typedef struct
{
    STRU_AUDIO_FRAME   stRefFrame;    /* AEC reference audio frame */
    AR_BOOL            bValid;        /* whether frame is valid */
	AR_BOOL            bSysBind;       /* whether is sysbind */
} STRU_AEC_FRAME;


typedef struct
{
    STRU_AUDIO_FRAME *pstFrame;/*frame ptr*/
    AR_U32           u32Id;   /*frame id*/
} STRU_AUDIO_FRAME_INFO;

typedef struct
{
    AR_U8  AR_ATTRIBUTE *pStream;         /* the virtual address of stream */
    AR_U64 AR_ATTRIBUTE u64PhyAddr;      /* the physics address of stream */
    AR_U32 u32Len;          /* stream lenth, by bytes */
    AR_U64 u64TimeStamp;    /* frame time stamp*/
    AR_U32 u32Seq;          /* frame seq,if stream is not a valid frame,u32Seq is 0*/
} STRU_AUDIO_STREAM;


typedef struct
{
    AR_U32                  u32ChnTotalNum;    /* total number of channel buffer */
    AR_U32                  u32ChnFreeNum;     /* free number of channel buffer */
    AR_U32                  u32ChnBusyNum;     /* busy number of channel buffer */
} STRU_AO_CHN_STATE;

typedef enum
{
    AR_AUDIO_TRACK_NORMAL      = 0,
    AR_AUDIO_TRACK_BOTH_LEFT   = 1,
    AR_AUDIO_TRACK_BOTH_RIGHT  = 2,
    AR_AUDIO_TRACK_EXCHANGE    = 3,
    AR_AUDIO_TRACK_MIX         = 4,
    AR_AUDIO_TRACK_LEFT_MUTE   = 5,
    AR_AUDIO_TRACK_RIGHT_MUTE  = 6,
    AR_AUDIO_TRACK_BOTH_MUTE   = 7,

    AR_AUDIO_TRACK_BUTT
} ENUM_AUDIO_TRACK_MODE;


typedef enum
{
    AR_AUDIO_FADE_RATE_1	= 0,
    AR_AUDIO_FADE_RATE_2	= 1,
    AR_AUDIO_FADE_RATE_4	= 2,
    AR_AUDIO_FADE_RATE_8   = 3,
    AR_AUDIO_FADE_RATE_16  = 4,
    AR_AUDIO_FADE_RATE_32  = 5,
    AR_AUDIO_FADE_RATE_64  = 6,
    AR_AUDIO_FADE_RATE_128 = 7,

    AR_AUDIO_FADE_RATE_BUTT
} ENUM_AUDIO_FADE_RATE;

typedef struct
{
    AR_BOOL         bFade;
    ENUM_AUDIO_FADE_RATE enFadeInRate;
    ENUM_AUDIO_FADE_RATE enFadeOutRate;
} STRU_AUDIO_FADE;

/**Defines the configure parameters of AEC.*/
typedef struct
{
    AR_BOOL bUsrMode;                             /* mode 0: auto mode 1: mannual.*/
    AR_S8 s8CngMode;                              /* cozy noisy mode: 0 close, 1 open, recommend 1*/
    AR_S8 s8NearAllPassEnergy;                    /* the far-end energy threshold for judging whether unvarnished transmission: 0 -59dBm0, 1 -49dBm0, 2 -39dBm0, recommend 1 */
    AR_S8 s8NearCleanSupEnergy;                   /* the energy threshold for compelling reset of near-end signal: 0 12dB, 1 15dB, 2 18dB, recommend 2 */

    AR_S16 s16DTHnlSortQTh;                       /* the threshold of judging single or double talk, recommend 16384, [0, 32767] */

    AR_S16 s16EchoBandLow;                       /* voice processing band1, low frequency parameter, [1, 63) for 8k, [1, 127) for 16k, recommend 10 */
    AR_S16 s16EchoBandHigh;                      /* voice processing band1, high frequency parameter, (s16EchoBandLow, 63] for 8k, (s16EchoBandLow, 127] for 16k, recommend 41 */
                                                   /* s16EchoBandHigh must be greater than s16EchoBandLow */
    AR_S16 s16EchoBandLow2;                      /* voice processing band2, low frequency parameter, [1, 63) for 8k, [1, 127) for 16k, recommend 47 */
    AR_S16 s16EchoBandHigh2;                     /* voice processing band2, high frequency parameter, (s16EchoBandLow2, 63] for 8k, (s16EchoBandLow2, 127] for 16k, recommend 72 */
                                                   /* s16EchoBandHigh2 must be greater than s16EchoBandLow2 */

    AR_S16 s16ERLBand[6];                        /* ERL protect area, [1, 63] for 8k, [1, 127] for 16k, frequency band calculated by s16ERLBand * 62.5 */
                                                  /* besides, s16ERLBand[n+1] should be greater than s16ERLBand[n] */
    AR_S16 s16ERL[7];                            /* ERL protect value of ERL protect area, the smaller its value, the more strength its protect ability,[0, 18]*/
    AR_S16 s16VioceProtectFreqL;                 /* protect area of near-end low frequency, [1, 63) for 8k, [1, 127) for 16k, recommend 3 */
    AR_S16 s16VioceProtectFreqL1;                /* protect area of near-end low frequency1, (s16VioceProtectFreqL, 63] for 8k, (s16VioceProtectFreqL, 127] for 16k, recommend 6 */
    AR_S32 s32Reserved;                          /* s16VioceProtectFreqL1 must be greater than s16VioceProtectFreqL */
}STRU_AI_AEC_CONFIG ;

/**Defines the configure parameters of ANR.*/
typedef struct
{
    AR_BOOL bUsrMode;   /* mode 0: auto, mode 1: manual.*/

    AR_S16 s16NrIntensity;       /* noise reduce intensity, range: [0, 25] */
    AR_S16 s16NoiseDbThr;        /* noise threshold, range: [30, 60] */
    AR_S8  s8SpProSwitch;        /* switch for music probe, range: [0:close, 1:open] */

    AR_S32 s32Reserved;
} STRU_AUDIO_ANR_CONFIG;

/**Defines the configure parameters of AGC.*/
typedef struct
{
    AR_BOOL bUsrMode;          /* mode 0: auto, mode 1: manual.*/

    AR_S8 s8TargetLevel;       /* target voltage level, range: [-40, -1]dB */
    AR_S8 s8NoiseFloor;        /* noise floor, range: TalkVqe/AoVqe[-65, -20]dB, RecordVqe[-50, -20]dB */
    AR_S8 s8MaxGain;           /* max gain, range: [0, 30]dB */
    AR_S8 s8AdjustSpeed;       /* adjustable speed, range: [0, 10]dB/s */

    AR_S8 s8ImproveSNR;        /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
    AR_S8 s8UseHighPassFilt;   /* switch for using high pass filt, range: [0:close, 1:80Hz, 2:120Hz, 3:150:Hz, 4:300Hz: 5:500Hz] */
    AR_S8 s8OutputMode;        /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
    AR_S16 s16NoiseSupSwitch;  /* switch for noise suppression, range: [0:close, 1:open] */


    AR_S32 s32Reserved;
} STRU_AUDIO_AGC_CONFIG ;

/**Defines the configure parameters of HPF.*/
typedef enum
{
    AR_AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    AR_AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    AR_AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    AR_AUDIO_HPF_FREQ_BUTT,
} ENUM_AUDIO_HPF_FREQ;

typedef struct
{
    AR_BOOL bUsrMode;   /* mode 0: auto mode 1: mannual.*/
    ENUM_AUDIO_HPF_FREQ enHpfFreq; /*freq to be processed,value:HiFiVqe/TalkVqe/AoVqe(80Hz,120Hz,150Hz), RecordVqe(80Hz)*/
} STRU_AUDIO_HPF_CONFIG;

typedef struct
{
    AR_BOOL bUsrMode;                /* mode 0: auto, mode 1: mannual.*/

    AR_S32  s32NrMode;               /*mode 0: floor noise; 1:ambient noise */

    AR_S32 s32MaxNrLevel;           /*max NR level range:[2,20]dB*/

    AR_S32  s32NoiseThresh;         /*noise threshold, range:[-80, -20]*/
} STRU_AI_RNR_CONFIG;

typedef struct
{
    AR_S8  s8GaindB[AR_VQE_EQ_BAND_NUM];   /*EQ band, include 100,200,250,350,500,800,1.2k,2.5k,4k,8k in turn,
    										range:TalkVqe/AoVqe[-100, 20], RecordVqe[-50, 20]*/
    AR_S32 s32Reserved;
} STRU_AUDIO_EQ_CONFIG;


/**Defines the configure parameters of UPVQE work state.*/
typedef enum
{
    AR_VQE_WORKSTATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    AR_VQE_WORKSTATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    AR_VQE_WORKSTATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} ENUM_VQE_WORKSTATE;

/**Defines record type*/
typedef enum
{
    AR_VQE_RECORD_NORMAL        = 0,  /*<double micphone recording. */
    AR_VQE_RECORD_BUTT,
} ENUM_VQE_RECORD_TYPE;


/* HDR Set CODEC GAIN Function Handle type */
typedef AR_S32 (*pFuncGainCallBack)(AR_S32 s32SetGain);

typedef struct
{
    AR_BOOL bUsrMode;               /* mode 0: auto mode 1: mannual.*/

    AR_S32 s32MinGaindB;            /* the minimum of MIC(AI) CODEC gain, [0, 120]*/
    AR_S32 s32MaxGaindB;            /* the maximum of MIC(AI) CODEC gain, [0, 120]*/

    AR_S32 s32MicGaindB;            /* the current gain of MIC(AI) CODEC,[s32MinGaindB, s32MaxGaindB]*/
    AR_S32 s32MicGainStepdB;        /* the step size of gain adjustment, [1, 3], recommemd 2 */
    pFuncGainCallBack pcallback;    /* the callback function pointer of CODEC gain adjustment */
} STRU_AI_HDR_CONFIG;

typedef struct
{
    AR_BOOL bUsrMode;   /* enable user mode or not,default 0: disable user mode,1: user mode.*/

    AR_S16  s16AttackTime;   /*time of signal change from large to small (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms*/
    AR_S16  s16ReleaseTime;  /*time of signal change from small to large (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms*/

    AR_S16 s16OldLevDb[AR_VQE_DRC_SECNUM];  /*old curve level(dB) ,default[0, -472, -792, -960, -1280],range:[-1440, 0]ms,store from big to small,scale:Q4*/
    AR_S16 s16NewLevDb[AR_VQE_DRC_SECNUM];  /*new curve level(dB) ,default[0, -174, -410, -608, -1021],range:[-1440, 0]ms,store from big to small,scale:Q4*/
} STRU_AI_DRC_CONFIG;


/**Defines the configure parameters of Record VQE.*/
typedef struct
{
	AR_U32				u32OpenMask;

    AR_S32              s32WorkSampleRate;  /* Sample Rate:16KHz/48KHz*/
    AR_S32              s32FrameSample; /* VQE frame length:80-4096 */
    ENUM_VQE_WORKSTATE  enWorkstate;

	AR_S32 				  s32InChNum;
	AR_S32				  s32OutChNum;
	ENUM_VQE_RECORD_TYPE  enRecordType;

	STRU_AUDIO_HPF_CONFIG  stHpfCfg;
    STRU_AI_RNR_CONFIG     stRnrCfg;
    STRU_AI_HDR_CONFIG     stHdrCfg;
	STRU_AI_DRC_CONFIG     stDrcCfg;
	STRU_AUDIO_EQ_CONFIG   stEqCfg;
	STRU_AUDIO_AGC_CONFIG  stAgcCfg;
} STRU_AI_RECORDVQE_CONFIG;

/**Defines the configure parameters of Talk VQE.*/
typedef struct
{
    AR_U32				u32OpenMask;

    AR_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz. default: 8KHz*/
    AR_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    ENUM_VQE_WORKSTATE     enWorkstate;

    STRU_AUDIO_HPF_CONFIG  stHpfCfg;
 	STRU_AI_AEC_CONFIG     stAecCfg;
    STRU_AUDIO_ANR_CONFIG  stAnrCfg;
    STRU_AUDIO_AGC_CONFIG  stAgcCfg;
    STRU_AUDIO_EQ_CONFIG   stEqCfg;
} STRU_AI_TALKVQE_CONFIG;

typedef struct
{
    AR_U32				u32OpenMask;

    AR_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz/48KHz. default: 8KHz*/
    AR_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    ENUM_VQE_WORKSTATE     enWorkstate;

    STRU_AUDIO_HPF_CONFIG stHpfCfg;
    STRU_AUDIO_ANR_CONFIG stAnrCfg;
    STRU_AUDIO_AGC_CONFIG stAgcCfg;
    STRU_AUDIO_EQ_CONFIG  stEqCfg;
} STRU_AO_VQE_CONFIG;

/*Defines the configure parameters of AI saving file.*/
typedef struct
{
    AR_BOOL     bCfg;
    AR_CHAR  	aFilePath[AR_MAX_AUDIO_FILE_PATH_LEN];
	AR_CHAR  	aFileName[AR_MAX_AUDIO_FILE_NAME_LEN];
    AR_U32 		u32FileSize;  /*in KB*/
} STRU_AUDIO_SAVE_FILE_INFO;

/*Defines whether the file is saving or not .*/
typedef struct
{
    AR_BOOL     bSaving;
} STRU_AUDIO_FILE_STATUS;

/**
* @note 定义音频模块时钟源类型
*/
typedef enum
{
    AR_AUDIO_CLKSEL_BASE       = 0,  /*<Audio base clk. */
    AR_AUDIO_CLKSEL_SPARE,           /*<Audio spare clk. */

    AR_AUDIO_CLKSEL_BUTT,
} ENUM_AUDIO_CLKSEL;

/**
* @note 音频模块参数结构体。
*/
typedef struct
{
    ENUM_AUDIO_CLKSEL enClkSel;
} STRU_AUDIO_MOD_PARAM;

typedef enum
{
    AR_AIO_ERR_VQE_ERR        = 65 , /*vqe error*/

} ENUM_EN_AIO_ERR_CODE;


/* invlalid device ID */
#define HAL_ERR_AI_INVALID_DEVID     AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HAL_ERR_AI_INVALID_CHNID     AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HAL_ERR_AI_ILLEGAL_PARAM     AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HAL_ERR_AI_NULL_PTR          AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HAL_ERR_AI_NOT_CONFIG        AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HAL_ERR_AI_NOT_SUPPORT       AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HAL_ERR_AI_NOT_PERM          AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* the devide is not enabled  */
#define HAL_ERR_AI_NOT_ENABLED       AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* failure caused by malloc memory */
#define HAL_ERR_AI_NOMEM             AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HAL_ERR_AI_NOBUF             AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define HAL_ERR_AI_BUF_EMPTY         AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HAL_ERR_AI_BUF_FULL          AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HAL_ERR_AI_SYS_NOTREADY      AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)

#define HAL_ERR_AI_BUSY              AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
/* vqe  err */
#define HAL_ERR_AI_VQE_ERR       AR_HAL_DEF_ERR(AR_SYS_ID_AI, HAL_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)

/* invlalid device ID */
#define HAL_ERR_AO_INVALID_DEVID     AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define HAL_ERR_AO_INVALID_CHNID     AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HAL_ERR_AO_ILLEGAL_PARAM     AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HAL_ERR_AO_NULL_PTR          AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HAL_ERR_AO_NOT_CONFIG        AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HAL_ERR_AO_NOT_SUPPORT       AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HAL_ERR_AO_NOT_PERM          AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* the devide is not enabled  */
#define HAL_ERR_AO_NOT_ENABLED       AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* failure caused by malloc memory */
#define HAL_ERR_AO_NOMEM             AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HAL_ERR_AO_NOBUF             AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define HAL_ERR_AO_BUF_EMPTY         AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HAL_ERR_AO_BUF_FULL          AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HAL_ERR_AO_SYS_NOTREADY      AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)

#define HAL_ERR_AO_BUSY              AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
/* vqe  err */
#define HAL_ERR_AO_VQE_ERR       AR_HAL_DEF_ERR(AR_SYS_ID_AO, HAL_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HAL_COMM_AI_H__ */

