/**
 * @file hal_acodec.h
 * @brief  hal 音频编解码数据结构
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_ACODEC_H__
#define __HAL_ACODEC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define IOC_TYPE_ACODEC 'A'

typedef enum {
    AR_ACODEC_FS_8000  =   0x1,
    AR_ACODEC_FS_11025 =   0x2,
    AR_ACODEC_FS_12000 =   0x3,
    AR_ACODEC_FS_16000 =   0x4,
    AR_ACODEC_FS_22050 =   0x5,
    AR_ACODEC_FS_24000 =   0x6,
    AR_ACODEC_FS_32000 =   0x7,
    AR_ACODEC_FS_44100 =   0x8,
    AR_ACODEC_FS_48000 =   0x9,
    AR_ACODEC_FS_64000 =   0xa,
    AR_ACODEC_FS_96000 =   0xb,

    AR_ACODEC_FS_BUTT = 0x1c,
} ENMU_AR_ACODEC_FS;

typedef enum {
    AR_ACODEC_MIXER_IN0   = 0x0,   /* 16CV500/16DV300 Unsupport IN0. */
    AR_ACODEC_MIXER_IN1   = 0x1,
    AR_ACODEC_MIXER_IN_D  = 0x2,

    AR_ACODEC_MIXER_BUTT,
} ENMU_AR_ACODEC_MIXER;

typedef enum {
    AR_ACODEC_ADC_SWITCH_LR   = 0x0,        /* {L, R}. */
    AR_ACODEC_ADC_SWITCH_RL   = 0x1,        /* {R, L}. */
    AR_ACODEC_ADC_SWITCH_REVERSE_LR  = 0x2, /* {-L, -R}. */
    AR_ACODEC_ADC_SWITCH_REVERSE_RL  = 0x3, /* {-R, -L}. */
    AR_ACODEC_ADC_SWITCH_BUTT,
} ENMU_AR_ACODEC_ADC_SWITCH;

typedef struct {
    /* volume control, 0x00~0x7e, 0x7F:mute */
    unsigned int vol_ctrl;
    /* adc/dac mute control, 1:mute, 0:unmute */
    unsigned int vol_ctrl_mute;
} STRU_AR_ACODEC_VOL_CTRL;

typedef enum {
    AR_AC_IOC_NR_SOFT_RESET_CTRL = 0x0,

    AR_AC_IOC_NR_SET_INPUT_VOL,
    AR_AC_IOC_NR_SET_OUTPUT_VOL,
    AR_AC_IOC_NR_GET_INPUT_VOL,
    AR_AC_IOC_NR_GET_OUTPUT_VOL,

    AR_AC_IOC_NR_SET_I2S1_FS,
    AR_AC_IOC_NR_SET_MIXER_MIC,
    AR_AC_IOC_NR_SEL_DAC_CLK,
    AR_AC_IOC_NR_SEL_ADC_CLK,
    AR_AC_IOC_NR_SEL_ANA_MCLK,
    AR_AC_IOC_NR_SET_GAIN_MICL,
    AR_AC_IOC_NR_SET_GAIN_MICR,
    AR_AC_IOC_NR_SET_DACL_VOL,
    AR_AC_IOC_NR_SET_DACR_VOL,
    AR_AC_IOC_NR_SET_ADCL_VOL,
    AR_AC_IOC_NR_SET_ADCR_VOL,
    AR_AC_IOC_NR_SET_MICL_MUTE,
    AR_AC_IOC_NR_SET_MICR_MUTE,
    AR_AC_IOC_NR_SET_DACL_MUTE,
    AR_AC_IOC_NR_SET_DACR_MUTE,
    AR_AC_IOC_NR_BOOSTL_ENABLE,
    AR_AC_IOC_NR_BOOSTR_ENABLE,

    AR_AC_IOC_NR_GET_GAIN_MICL,
    AR_AC_IOC_NR_GET_GAIN_MICR,
    AR_AC_IOC_NR_GET_DACL_VOL,
    AR_AC_IOC_NR_GET_DACR_VOL,
    AR_AC_IOC_NR_GET_ADCL_VOL,
    AR_AC_IOC_NR_GET_ADCR_VOL,

    AR_AC_IOC_NR_SET_PD_DACL,
    AR_AC_IOC_NR_SET_PD_DACR,
    AR_AC_IOC_NR_SET_PD_ADCL,
    AR_AC_IOC_NR_SET_PD_ADCR,
    AR_AC_IOC_NR_SET_PD_LINEINL,
    AR_AC_IOC_NR_SET_PD_LINEINR,

    AR_AC_IOC_NR_SET_DAC_DE_EMPHASIS,
    AR_AC_IOC_NR_SET_ADC_HP_FILTER,

    AR_AC_IOC_NR_SET_I2S1_DATAWIDTH,

    AR_AC_IOC_NR_DACL_SEL_TRACK,
    AR_AC_IOC_NR_DACR_SEL_TRACK,
    AR_AC_IOC_NR_ADCL_SEL_TRACK,
    AR_AC_IOC_NR_ADCR_SEL_TRACK,

    //artosyn define
    AR_AC_IOC_NR_SET_SWITCH_ADC,
    AR_AC_IOC_NR_GET_SWITCH_ADC,
} ENMU_AR_ACODEC_IOCTL;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HAL_AIO_H__ */

