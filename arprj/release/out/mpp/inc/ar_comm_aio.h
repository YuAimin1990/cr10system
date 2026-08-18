/**
 * \file
 * \brief 描述视频输入输出相关的通用数据结构
 */

#ifndef __AR_COMM_AIO_H__
#define __AR_COMM_AIO_H__

#include "ar_common.h"
#include "hal_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/********************************Macro Definition********************************/
/** \addtogroup      MPI_AUDIO */
/** @{ */  /** <!-- [MPI_AUDIO] */

#define MAX_AUDIO_FRAME_NUM    300      /*max count of audio frame in Buffer */
#define MAX_AUDIO_POINT_BYTES  4        /*max bytes of one sample point(now 32bit max)*/

#define MAX_VOICE_POINT_NUM    480      /*max sample per frame for voice encode */

#define MAX_AUDIO_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/
#define MAX_AO_POINT_NUM       4096     /* from h3;support 4096 framelen*/
#define MIN_AUDIO_POINT_NUM    80       /*min sample per frame*/
#define MAX_AI_POINT_NUM       2048     /*max sample per frame for all encoder(aacplus:2048)*/

/*max length of audio frame by bytes, one frame contain many sample point */
#define MAX_AUDIO_FRAME_LEN    (MAX_AUDIO_POINT_BYTES*MAX_AO_POINT_NUM)

/*max length of audio stream by bytes */
#define MAX_AUDIO_STREAM_LEN   MAX_AUDIO_FRAME_LEN

#define MAX_AI_USRFRM_DEPTH     30      /*max depth of user frame buf */

#define MAX_AUDIO_FILE_PATH_LEN 256
#define MAX_AUDIO_FILE_NAME_LEN 256

/*The VQE EQ Band num.*/
#define VQE_EQ_BAND_NUM  10
#define VQE_DRC_SECNUM  5

#define AI_RECORDVQE_MASK_HPF   0x1
#define AI_RECORDVQE_MASK_RNR   0x2
#define AI_RECORDVQE_MASK_HDR   0x4
#define AI_RECORDVQE_MASK_DRC   0x8
#define AI_RECORDVQE_MASK_EQ    0x10
#define AI_RECORDVQE_MASK_AGC   0x20

#define AI_TALKVQE_MASK_HPF   0x1
#define AI_TALKVQE_MASK_AEC   0x2
#define AI_TALKVQE_MASK_AGC   0x8
#define AI_TALKVQE_MASK_EQ    0x10
#define AI_TALKVQE_MASK_ANR   0x20

#define AO_VQE_MASK_HPF     0x1
#define AO_VQE_MASK_ANR     0x2
#define AO_VQE_MASK_AGC     0x4
#define AO_VQE_MASK_EQ      0x8

/** @} */  /** <!-- ==== Macro Definition end ==== */


/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_AUDIO */
/** @{ */  /** <!-- [MPI_AUDIO] */

/**定义音频采样率*/
typedef enum arAUDIO_SAMPLE_RATE_E
{
    AUDIO_SAMPLE_RATE_8000   = 8000,    /**<8kHz 采样率*/
    AUDIO_SAMPLE_RATE_12000  = 12000,   /**<12kHz 采样率*/
    AUDIO_SAMPLE_RATE_11025  = 11025,   /**<11.025kHz 采样率*/
    AUDIO_SAMPLE_RATE_16000  = 16000,   /**<16kHz 采样率*/
    AUDIO_SAMPLE_RATE_22050  = 22050,   /**<22.05kHz 采样率*/
    AUDIO_SAMPLE_RATE_24000  = 24000,   /**<24kHz 采样率*/
    AUDIO_SAMPLE_RATE_32000  = 32000,   /**<32kHz 采样率*/
    AUDIO_SAMPLE_RATE_44100  = 44100,   /**<44.1kHz 采样率*/
    AUDIO_SAMPLE_RATE_48000  = 48000,   /**<48kHz 采样率*/
    AUDIO_SAMPLE_RATE_64000  = 64000,   /**<64kHz 采样率*/
    AUDIO_SAMPLE_RATE_96000  = 96000,   /**<96kHz 采样率*/
    AUDIO_SAMPLE_RATE_BUTT,
} AUDIO_SAMPLE_RATE_E;

/**定义音频采样精度*/
typedef enum arAUDIO_BIT_WIDTH_E
{
    AUDIO_BIT_WIDTH_8   = 0,   /**<采样精度为 8bit 位宽*/
    AUDIO_BIT_WIDTH_16  = 1,   /**<采样精度为 16bit 位宽*/
    AUDIO_BIT_WIDTH_24  = 2,   /**<采样精度为 24bit 位宽*/
    AUDIO_BIT_WIDTH_32  = 3,   /**<采样精度为 32bit 位宽*/
    AUDIO_BIT_WIDTH_BUTT,
} AUDIO_BIT_WIDTH_E;

/**定义音频输入输出设备工作模式*/
typedef enum arAIO_MODE_E
{
    AIO_MODE_I2S_MASTER  = 0,   /**<I2S 主模式*/
    AIO_MODE_I2S_SLAVE,         /**<I2S 从模式*/
    AIO_MODE_PCM_SLAVE_STD,     /**<PCM 从模式（标准协议）*/
    AIO_MODE_PCM_SLAVE_NSTD,    /**<PCM 从模式（自定义协议）*/
    AIO_MODE_PCM_MASTER_STD,    /**<PCM 主模式（标准协议）*/
    AIO_MODE_PCM_MASTER_NSTD,   /**<PCM 主模式（自定义协议）*/
    AIO_MODE_BUTT
} AIO_MODE_E;
/**定义音频设备ID */
typedef enum {
    AIO_ACODEC_DEV_ID = 0,       /**<内置audio codec */
    AIO_I2S_MASTER0_ID = 1,      /**<内置I2S master0 */
    AIO_I2S_MASTER1_ID,          /**<内置I2S master1 */
#ifdef AR9341
    AIO_I2S_MASTER2_ID,          /**<内置I2S master2   只有9341支持 */
#endif
    AIO_I2S_SLAVE0_ID,           /**<内置I2S slave0 */
    AIO_I2S_SLAVE1_ID,           /**<内置I2S slave1 */
} AR_AIO_DEV_ID;

/**定义设备 I2S 对接设备类型*/
typedef enum
{
    AIO_I2STYPE_INNERCODEC = 0, /**<对接内置 CODEC*/
    AIO_I2STYPE_INNERHDMI,      /**<对接内置 HDMI*/
    AIO_I2STYPE_EXTERN,         /**<对接外接设备*/
} AIO_I2STYPE_E;

/**定义音频声道模式*/
typedef enum arAIO_SOUND_MODE_E
{
    AUDIO_SOUND_MODE_MONO   =0, /**<单声道*/
    AUDIO_SOUND_MODE_STEREO =1, /**<双声道*/
    AUDIO_SOUND_MODE_BUTT
} AUDIO_SOUND_MODE_E;

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
/**定义 G.726 编解码协议速率*/
typedef enum arG726_BPS_E
{
    G726_16K = 0,       /**<16kbps G.726*/
    G726_24K,           /**<24kbps G.726*/
    G726_32K,           /**<32kbps G.726*/
    G726_40K,           /**<40kbps G.726*/
    MEDIA_G726_16K,     /**<G726 16kbps for ASF*/
    MEDIA_G726_24K,     /**<G726 24kbps for ASF*/
    MEDIA_G726_32K,     /**<G726 32kbps for ASF*/
    MEDIA_G726_40K,     /**<G726 40kbps for ASF*/
    G726_BUTT,
} G726_BPS_E;

/**定义 ADPCM 编解码协议类型*/
typedef enum arADPCM_TYPE_E
{
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */

    ADPCM_TYPE_DVI4 = 0,    /**<32kbit/s ADPCM(DVI4) */
    ADPCM_TYPE_IMA,         /**<32kbit/s ADPCM(IMA)*/
    ADPCM_TYPE_ORG_DVI4,    /**<32kbit/s ADPCM(ORG_DVI4)*/
    ADPCM_TYPE_BUTT,
} ADPCM_TYPE_E;

#define AI_EXPAND  0x01
#define AI_CUT     0x02

/**定义音频输入输出设备属性结构体*/
typedef struct arAIO_ATTR_S
{
    AUDIO_SAMPLE_RATE_E enSamplerate;   /**<@note 音频采样率,当前最高支持48000*/
    AUDIO_BIT_WIDTH_E   enBitwidth;     /**<音频采样精度（从模式下，此参数必须和音频 AD/DA 的采样精度匹配）*/
    AIO_MODE_E          enWorkmode;     /**<音频输入输出工作模式*/
    AUDIO_SOUND_MODE_E  enSoundmode;    /**<音频声道模式*/
    AR_U32              u32EXFlag;      /**<取值范围：{0, 1, 2}
                                        0：不扩展。
                                        1：扩展成 16 位，8bit 到 16bit 扩展标志
                                        2：24 位裁剪成 16 位，在外置 Codec 的场景下可能用到*/
    AR_U32              u32FrmNum;      /**<缓存帧数目[2, 300],值越大越能抗住系统loading重的时候断音的问题 */
    AR_U32              u32PtNumPerFrm; /**<每帧的采样点个数, 每个Frame=enBitwidth*u32PtNumPerFrm, Frame最大4k */
    AR_U32              u32ChnCnt;      /**<支持的通道数目*/
    AR_U32              u32ClkSel;      /**<配置 AI 设备 0 是否复用 AO 设备 0 的帧同步时钟及位流时钟,
                                        取值：0、1, 0：不复用, 1：复用*/
    AIO_I2STYPE_E       enI2sType;      /**<配置设备 I2S 类型*/
} AIO_ATTR_S;

/**定义通道参数结构体*/
typedef struct arAI_CHN_PARAM_S
{
    AR_U32 u32UsrFrmDepth; /**<音频帧缓存深度*/
} AI_CHN_PARAM_S;

/**定义音频帧结构体*/
typedef struct arAUDIO_FRAME_S
{
    AUDIO_BIT_WIDTH_E   enBitwidth;      /**<@note 音频采样精度,当前只支持16bit*/
    AUDIO_SOUND_MODE_E  enSoundmode;     /**<音频声道模式*/
    AR_U8*  u64VirAddr[2];               /**<音频帧数据虚拟地址*/
    AR_U64  u64PhyAddr[2];               /**<音频帧数据物理地址*/
    AR_U64  u64TimeStamp;                /**<音频帧时间戳, 以 µs 为单位*/
    AR_U32  u32Seq;                      /**<音频帧序号*/
    AR_U32  u32Len;                      /**<音频帧长度, 以 byte 为单位*/
    AR_U32  u32PoolId[2];                /**<音频帧缓存池 ID*/
} AUDIO_FRAME_S;

/**定义音频回声抵消参考帧信息结构体*/
typedef struct arAEC_FRAME_S
{
    AUDIO_FRAME_S   stRefFrame;    /**<回声抵消参考帧结构体*/
    AR_BOOL         bValid;        /**<参考帧有效的标志*/
    AR_BOOL         bSysBind;      /**<AI 和 AENC 是否系统绑定*/
} AEC_FRAME_S;

/**定义解码后的音频帧信息结构体*/
typedef struct arAUDIO_FRAME_INFO_S
{
    AUDIO_FRAME_S *pstFrame;/**<音频帧指针*/
    AR_U32         u32Id;   /**<音频帧的索引*/
} AUDIO_FRAME_INFO_S;

/**定义音频码流结构体*/
typedef struct arAUDIO_STREAM_S
{
    AR_U8 ATTRIBUTE *pStream;         /**<音频码流数据指针*/
    AR_U64 ATTRIBUTE u64PhyAddr;      /**<音频码流的物理地址*/
    AR_U32 u32Len;          /**<音频码流长度。以 byte 为单位*/
    AR_U64 u64TimeStamp;    /**<音频码流时间戳*/
    AR_U32 u32Seq;          /**<音频码流序号*/
} AUDIO_STREAM_S;

/**音频输出通道的数据缓存状态结构体*/
typedef struct arAO_CHN_STATE_S
{
    AR_U32                  u32ChnTotalNum;    /**<输出通道总的缓存块数*/
    AR_U32                  u32ChnFreeNum;     /**<可用的空闲缓存块数*/
    AR_U32                  u32ChnBusyNum;     /**<被占用缓存块数*/
} AO_CHN_STATE_S;

/**定义音频设备声道模式类型*/
typedef enum arAUDIO_TRACK_MODE_E
{
    AUDIO_TRACK_NORMAL      = 0, /**<正常模式，不做处理*/
    AUDIO_TRACK_BOTH_LEFT   = 1, /**<两个声道全部为左声道声音*/
    AUDIO_TRACK_BOTH_RIGHT  = 2, /**<两个声道全部为右声道声音*/
    AUDIO_TRACK_EXCHANGE    = 3, /**<左右声道数据互换，左声道为右声道声音，右声道为左声道声音*/
    AUDIO_TRACK_MIX         = 4, /**<左右两个声道输出为左右声道相加（混音）*/
    AUDIO_TRACK_LEFT_MUTE   = 5, /**<左声道静音，右声道播放原右声道声音*/
    AUDIO_TRACK_RIGHT_MUTE  = 6, /**<右声道静音，左声道播放原左声道声音*/
    AUDIO_TRACK_BOTH_MUTE   = 7, /**<左右声道均静音*/

    AUDIO_TRACK_BUTT
} AUDIO_TRACK_MODE_E;

/**定义音频输出设备淡入淡出速度类型*/
typedef enum arAUDIO_FADE_RATE_E
{
    AUDIO_FADE_RATE_1 = 0, /**<1 个采样点改变一次*/
    AUDIO_FADE_RATE_2 = 1, /**<2 个采样点改变一次*/
    AUDIO_FADE_RATE_4 = 2, /**<3 个采样点改变一次*/
    AUDIO_FADE_RATE_8   = 3, /**<4 个采样点改变一次*/
    AUDIO_FADE_RATE_16  = 4, /**<5 个采样点改变一次*/
    AUDIO_FADE_RATE_32  = 5, /**<6 个采样点改变一次*/
    AUDIO_FADE_RATE_64  = 6, /**<7 个采样点改变一次*/
    AUDIO_FADE_RATE_128 = 7, /**<8 个采样点改变一次*/

    AUDIO_FADE_RATE_BUTT
} AUDIO_FADE_RATE_E;

/**音频输出设备淡入淡出配置结构体*/
typedef struct arAUDIO_FADE_S
{
    AR_BOOL         bFade;            /**<是否开启淡入淡出功能*/
    AUDIO_FADE_RATE_E enFadeInRate;   /**<音频输出设备音量淡入速度*/
    AUDIO_FADE_RATE_E enFadeOutRate;  /**<音频输出设备音量淡出速度*/
} AUDIO_FADE_S;

/**定义音频回声抵消配置信息结构体*/
typedef struct arAI_AEC_CONFIG_S
{
    AR_BOOL bUsrMode;                             /**<是否采用用户模式, 0：自动模式, 1：用户模式, 默认为 0 关闭*/
    AR_S8 s8CngMode;                              /**<是否开启舒适噪声模式,0：关闭, 1：开启 */
    AR_S8 s8NearAllPassEnergy;                    /**<判断近端是否透传的远端能量阈值，默认为 1*/
    AR_S8 s8NearCleanSupEnergy;                   /**<近端信号强制清零的能量阈值，默认为 2*/

    AR_S16 s16DTHnlSortQTh;                       /**<单双讲判断门限值，标为 Q15，默认取 16384*/

    AR_S16 s16EchoBandLow;                       /**<语音处理频段 1，低频参数*/
    AR_S16 s16EchoBandHigh;                      /**<语音处理频段 1，高频参数*/
    AR_S16 s16EchoBandLow2;                      /**<语音处理频段 2, 低频参数*/
    AR_S16 s16EchoBandHigh2;                     /**<语音处理频段 2,高频参数*/

    AR_S16 s16ERLBand[6];                        /**<ERL(回波衰减路径)保护频段数组参数*/

    AR_S16 s16ERL[7];                            /**<ERL(回波衰减路径)频段保护值数组参数*/
    AR_S16 s16VioceProtectFreqL;                 /**<近端低频保护区域频点参数*/
    AR_S16 s16VioceProtectFreqL1;                /**<近端低频保护区域频点参数*/
    AR_S32 s32Reserved;                          /**<保留，未使用*/
} AI_AEC_CONFIG_S;

/**定义音频语音降噪功能配置信息结构体*/
typedef struct arAUDIO_ANR_CONFIG_S
{
    AR_BOOL bUsrMode;            /**<是否采用用户模式, 0：自动模式, 1：用户模式, 默认为 0 关闭*/

    AR_S16 s16NrIntensity;       /**<降噪力度配置*/
    AR_S16 s16NoiseDbThr;        /**<噪声门限配置*/
    AR_S8  s8SpProSwitch;        /**<音乐检测开关*/

    AR_S32 s32Reserved;
} AUDIO_ANR_CONFIG_S;

/**定义音频自动增益控制配置信息结构体*/
typedef struct arAUDIO_AGC_CONFIG_S
{
    AR_BOOL bUsrMode;          /**<是否采用用户模式, 0：自动模式, 1：用户模式, 默认为 0 关闭*/

    AR_S8 s8TargetLevel;       /**<目标电平*/
    AR_S8 s8NoiseFloor;        /**<噪声底线*/
    AR_S8 s8MaxGain;           /**<最大增益*/
    AR_S8 s8AdjustSpeed;       /**<调整速度*/

    AR_S8 s8ImproveSNR;        /**<提高信噪比开关*/
    AR_S8 s8UseHighPassFilt;   /**<打开高通滤波标志*/
    AR_S8 s8OutputMode;        /**<输出模式,*/
    AR_S16 s16NoiseSupSwitch;  /**<噪声抑制开关*/

    AR_S32 s32Reserved;
} AUDIO_AGC_CONFIG_S;

/**定义音频高通滤波截止频率*/
typedef enum arAUDIO_HPF_FREQ_E
{
    AUDIO_HPF_FREQ_80   = 80,    /**<80Hz*/
    AUDIO_HPF_FREQ_120  = 120,   /**<120Hz*/
    AUDIO_HPF_FREQ_150  = 150,   /**<150Hz*/
    AUDIO_HPF_FREQ_BUTT,
} AUDIO_HPF_FREQ_E;

/**定义音频高通滤波功能配置信息结构体*/
typedef struct arAUDIO_HPF_CONFIG_S
{
    AR_BOOL bUsrMode;           /**<是否采用用户模式, 0：自动模式, 1：用户模式, 默认为 0 关闭*/
    AUDIO_HPF_FREQ_E enHpfFreq; /**<高通滤波截止频率选择*/
} AUDIO_HPF_CONFIG_S;

/**定义音频录音噪声消除功能配置信息结构体*/
typedef struct arAI_RNR_CONFIG_S
{
    AR_BOOL bUsrMode;                /**<是否采用用户模式, 0：自动模式, 1：用户模式, 默认为 0 关闭*/

    AR_S32  s32NrMode;               /**<降噪模式, 0：降底噪, 1：降环境噪声, 默认值：0*/

    AR_S32 s32MaxNrLevel;           /**<最大降噪能力*/

    AR_S32  s32NoiseThresh;         /**<噪声阈值*/
} AI_RNR_CONFIG_S;

/**定义音频均衡器功能配置信息结构体*/
typedef struct arAUDIO_EQ_CONFIG_S
{
    AR_S8  s8GaindB[VQE_EQ_BAND_NUM];  /**<EQ 频段增益调节*/
    AR_S32 s32Reserved;
} AUDIO_EQ_CONFIG_S;


/**定义声音质量增强的工作模式*/
typedef enum arVQE_WORKSTATE_E
{
    VQE_WORKSTATE_COMMON  = 0,   /**<一般模式*/
    VQE_WORKSTATE_MUSIC   = 1,   /**<音乐模式*/
    VQE_WORKSTATE_NOISY   = 2,   /**<噪声模式*/
} VQE_WORKSTATE_E;

/**定义录音类型*/
typedef enum arVQE_RECORD_TYPE
{
    VQE_RECORD_NORMAL        = 0,  /**<标准类型*/
    VQE_RECORD_BUTT,
} VQE_RECORD_TYPE;


/**定义CODEC 增益的函数指针 */
typedef AR_S32 (*pFuncGainCallBack)(AR_S32 s32SetGain);

/**定义音频高动态范围功能配置信息结构体*/
typedef struct arAI_HDR_CONFIG_S
{
    AR_BOOL bUsrMode;               /**<是否采用用户模式, 0：自动模式, 1：用户模式, 默认为 0*/

    AR_S32 s32MinGaindB;            /**<CODEC 允许最小配置增益*/
    AR_S32 s32MaxGaindB;            /**<CODEC 允许最大配置增益*/

    AR_S32 s32MicGaindB;            /**<CODEC 允许最大配置增益*/
    AR_S32 s32MicGainStepdB;        /**<增益调整步长*/
    pFuncGainCallBack pcallback;    /**<修改 CODEC 增益的函数指针*/
} AI_HDR_CONFIG_S;

typedef struct arAI_DRC_CONFIG_S
{
    AR_BOOL bUsrMode;   /* enable user mode or not,default 0: disable user mode,1: user mode.*/

    AR_S16  s16AttackTime;   /*time of signal change from large to small (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms*/
    AR_S16  s16ReleaseTime;  /*time of signal change from small to large (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms*/

    AR_S16 s16OldLevDb[VQE_DRC_SECNUM];  /*old curve level(dB) ,default[0, -472, -792, -960, -1280],range:[-1440, 0]ms,store from big to small,scale:Q4*/
    AR_S16 s16NewLevDb[VQE_DRC_SECNUM];  /*new curve level(dB) ,default[0, -174, -410, -608, -1021],range:[-1440, 0]ms,store from big to small,scale:Q4*/
} AI_DRC_CONFIG_S;


/**Defines the configure parameters of Record VQE.*/
typedef struct arAI_RECORDVQE_CONFIG_S
{
    AR_U32        u32OpenMask;

    AR_S32              s32WorkSampleRate;  /* Sample Rate:16KHz/48KHz*/
    AR_S32              s32FrameSample; /* VQE frame length:80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AR_S32          s32InChNum;
    AR_S32          s32OutChNum;
    VQE_RECORD_TYPE       enRecordType;

    AUDIO_HPF_CONFIG_S  stHpfCfg;
    AI_RNR_CONFIG_S     stRnrCfg;
    AI_HDR_CONFIG_S     stHdrCfg;
    AI_DRC_CONFIG_S     stDrcCfg;
    AUDIO_EQ_CONFIG_S   stEqCfg;
    AUDIO_AGC_CONFIG_S  stAgcCfg;
} AI_RECORDVQE_CONFIG_S;

/**Defines the configure parameters of Talk VQE.*/
typedef struct arAI_TALKVQE_CONFIG_S
{
    AR_U32        u32OpenMask;

    AR_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz. default: 8KHz*/
    AR_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S  stHpfCfg;
    AI_AEC_CONFIG_S     stAecCfg;
    AUDIO_ANR_CONFIG_S  stAnrCfg;
    AUDIO_AGC_CONFIG_S  stAgcCfg;
    AUDIO_EQ_CONFIG_S   stEqCfg;
} AI_TALKVQE_CONFIG_S;

typedef struct arAO_VQE_CONFIG_S
{
    AR_U32        u32OpenMask;

    AR_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz/48KHz. default: 8KHz*/
    AR_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S stHpfCfg;
    AUDIO_ANR_CONFIG_S stAnrCfg;
    AUDIO_AGC_CONFIG_S stAgcCfg;
    AUDIO_EQ_CONFIG_S  stEqCfg;
} AO_VQE_CONFIG_S;

/*Defines the configure parameters of AI saving file.*/
typedef struct arAUDIO_SAVE_FILE_INFO_S
{
    AR_BOOL     bCfg;
    AR_CHAR   aFilePath[MAX_AUDIO_FILE_PATH_LEN];
    AR_CHAR   aFileName[MAX_AUDIO_FILE_NAME_LEN];
    AR_U32    u32FileSize;  /*in KB*/
} AUDIO_SAVE_FILE_INFO_S;

/*Defines whether the file is saving or not .*/
typedef struct arAUDIO_FILE_STATUS_S
{
    AR_BOOL     bSaving;
} AUDIO_FILE_STATUS_S;

/**定义音频时钟源*/
typedef enum arAUDIO_CLKSEL_E
{
    AUDIO_CLKSEL_BASE       = 0,  /**<默认时钟源 SPLL*/
    AUDIO_CLKSEL_SPARE,           /**<备用时钟源 VPLL*/

    AUDIO_CLKSEL_BUTT,
} AUDIO_CLKSEL_E;

/**定义音频模块参数结构体*/
typedef struct arAUDIO_MOD_PARAM_S
{
    AUDIO_CLKSEL_E enClkSel;  /**<音频时钟源选择*/
} AUDIO_MOD_PARAM_S;

/**定义音频输入输出错误类型*/
typedef enum arEN_AIO_ERR_CODE_E
{
    AIO_ERR_VQE_ERR        = 65 , /*vqe error*/

} EN_AIO_ERR_CODE_E;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/* invlalid device ID */
#define AR_ERR_AI_INVALID_DEVID     AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define AR_ERR_AI_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AR_ERR_AI_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define AR_ERR_AI_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AR_ERR_AI_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AR_ERR_AI_NOT_SUPPORT       AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define AR_ERR_AI_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* the devide is not enabled  */
#define AR_ERR_AI_NOT_ENABLED       AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* failure caused by malloc memory */
#define AR_ERR_AI_NOMEM             AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define AR_ERR_AI_NOBUF             AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define AR_ERR_AI_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define AR_ERR_AI_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define AR_ERR_AI_SYS_NOTREADY      AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)

#define AR_ERR_AI_BUSY              AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
/* vqe  err */
#define AR_ERR_AI_VQE_ERR       AR_MPP_DEF_ERR(AR_ID_AI, HAL_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)

/* invlalid device ID */
#define AR_ERR_AO_INVALID_DEVID     AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define AR_ERR_AO_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AR_ERR_AO_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define AR_ERR_AO_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AR_ERR_AO_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AR_ERR_AO_NOT_SUPPORT       AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define AR_ERR_AO_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* the devide is not enabled  */
#define AR_ERR_AO_NOT_ENABLED       AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* failure caused by malloc memory */
#define AR_ERR_AO_NOMEM             AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define AR_ERR_AO_NOBUF             AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define AR_ERR_AO_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define AR_ERR_AO_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define AR_ERR_AO_SYS_NOTREADY      AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)

#define AR_ERR_AO_BUSY              AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
/* vqe  err */
#define AR_ERR_AO_VQE_ERR       AR_MPP_DEF_ERR(AR_ID_AO, HAL_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __AR_COMM_AI_H__ */

