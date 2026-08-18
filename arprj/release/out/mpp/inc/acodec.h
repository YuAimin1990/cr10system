#ifndef _ACODEC_H_
#define _ACODEC_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define IOC_TYPE_ACODEC 'A'

/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_AUDIO */
/** @{ */  /** <!-- [MPI_AUDIO] */

/**定义 I2S 接口的采样率选择*/
typedef enum arACODEC_FS_E {
	ACODEC_FS_48000 =   0x1a,
	ACODEC_FS_24000 =   0x19,
	ACODEC_FS_12000 =   0x18,

	ACODEC_FS_44100 =   0x1a,
	ACODEC_FS_22050 =   0x19,
	ACODEC_FS_11025 =   0x18,

	ACODEC_FS_32000 =   0x1a,
	ACODEC_FS_16000 =   0x19,
	ACODEC_FS_8000  =   0x18,

	ACODEC_FS_64000 =   0x1b,

	ACODEC_FS_96000 =   0x1b,

	ACODEC_FS_BUTT = 0x1c,
} ACODEC_FS_E;

/**定义内置 Audio Codec 输入选择*/
typedef enum arACODEC_MIXER_E {
	ACODEC_MIXER_IN0   = 0x0, /**<MICPGA 选择 IN0 单端输入*/
	ACODEC_MIXER_IN1   = 0x1, /**<MICPGA 选择 IN1 单端输入L*/
	ACODEC_MIXER_IN_D  = 0x2, /**<MPCPGA 选择 IN_D 差分输入*/

	ACODEC_MIXER_BUTT,
} ACODEC_MIXER_E;

/**定义内置 Audio Codec 音量控制结构体*/
typedef struct {
	unsigned int vol_ctrl;      /**<音量大小*/
	unsigned int vol_ctrl_mute; /**<静音控制*/
} ACODEC_VOL_CTRL;

/**定义内置 Audio Codec 音量控制结构体ioctl命令*/
typedef enum arACODEC_IOCTL_E {
	IOC_NR_SOFT_RESET_CTRL = 0x0,

	IOC_NR_SET_INPUT_VOL ,
	IOC_NR_SET_OUTPUT_VOL,
	IOC_NR_GET_INPUT_VOL,
	IOC_NR_GET_OUTPUT_VOL,

	IOC_NR_SET_I2S1_FS,
	IOC_NR_SET_MIXER_MIC,       /**<@note 不支持*/
	IOC_NR_SEL_DAC_CLK,         /**<@note 不支持*/
	IOC_NR_SEL_ADC_CLK,         /**<@note 不支持*/
	IOC_NR_SEL_ANA_MCLK,        /**<@note 不支持*/
	IOC_NR_SET_GAIN_MICL,       /**<@note 不支持*/
	IOC_NR_SET_GAIN_MICR,       /**<@note 不支持*/
	IOC_NR_SET_DACL_VOL,
	IOC_NR_SET_DACR_VOL,
	IOC_NR_SET_ADCL_VOL,
	IOC_NR_SET_ADCR_VOL,
	IOC_NR_SET_MICL_MUTE,
	IOC_NR_SET_MICR_MUTE,
	IOC_NR_SET_DACL_MUTE,
	IOC_NR_SET_DACR_MUTE,
	IOC_NR_BOOSTL_ENABLE,       /**<@note 不支持*/
	IOC_NR_BOOSTR_ENABLE,       /**<@note 不支持*/

	IOC_NR_GET_GAIN_MICL,       /**<@note 不支持*/
	IOC_NR_GET_GAIN_MICR,       /**<@note 不支持*/
	IOC_NR_GET_DACL_VOL,
	IOC_NR_GET_DACR_VOL,
	IOC_NR_GET_ADCL_VOL,
	IOC_NR_GET_ADCR_VOL,

	IOC_NR_SET_PD_DACL,
	IOC_NR_SET_PD_DACR,
	IOC_NR_SET_PD_ADCL,
	IOC_NR_SET_PD_ADCR,
	IOC_NR_SET_PD_LINEINL,      /**<@note 不支持*/
	IOC_NR_SET_PD_LINEINR,      /**<@note 不支持*/

	IOC_NR_SET_DAC_DE_EMPHASIS, /**<@note 不支持*/
	IOC_NR_SET_ADC_HP_FILTER,   /**<@note 不支持*/

	IOC_NR_SET_I2S1_DATAWIDTH,  /**<@note 不支持*/

	IOC_NR_DACL_SEL_TRACK,      /**<@note 不支持*/
	IOC_NR_DACR_SEL_TRACK,      /**<@note 不支持*/
	IOC_NR_ADCL_SEL_TRACK,      /**<@note 不支持*/
	IOC_NR_ADCR_SEL_TRACK,      /**<@note 不支持*/
} ACODEC_IOCTL_E;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/*reset the audio code to the default config*/
#define ACODEC_SOFT_RESET_CTRL \
	_IO(IOC_TYPE_ACODEC, IOC_NR_SOFT_RESET_CTRL)
/*ACODEC_FS_E*/
#define ACODEC_SET_I2S1_FS \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_FS, unsigned int)

/*select the micpga's input, micin linein, or differential input(ACODEC_MIXER_E)*/
#define ACODEC_SET_MIXER_MIC \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MIXER_MIC, unsigned int)
/*analog part input volume control(left channel 0~0x1f)*/
#define ACODEC_SET_GAIN_MICL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_GAIN_MICL, unsigned int)
/*analog part input volume control(right channel 0~0x1f)*/
#define ACODEC_SET_GAIN_MICR \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_GAIN_MICR, unsigned int)
/*Output volume control(left channel) ACODEC_VOL_CTRL*/
#define ACODEC_SET_DACL_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_VOL, ACODEC_VOL_CTRL)
/*Output volume control(right channel) ACODEC_VOL_CTRL*/
#define ACODEC_SET_DACR_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_VOL, ACODEC_VOL_CTRL)
/*Input volume control(left channel) ACODEC_VOL_CTRL*/
#define ACODEC_SET_ADCL_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADCL_VOL, ACODEC_VOL_CTRL)
/*Input volume control(right channel) ACODEC_VOL_CTRL*/
#define ACODEC_SET_ADCR_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADCR_VOL, ACODEC_VOL_CTRL)
/*Input mute control(left channel), 1:mute, 0:unmute*/
#define ACODEC_SET_MICL_MUTE \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MICL_MUTE, unsigned int)
/*Input mute control(right channel), 1:mute, 0:unmute*/
#define ACODEC_SET_MICR_MUTE \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MICR_MUTE, unsigned int)
/*Output mute control(left channel), 1:mute, 0:unmute*/
#define ACODEC_SET_DACL_MUTE \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_MUTE, unsigned int)
/*Output mute control(right channel), 1:mute, 0:unmute*/
#define ACODEC_SET_DACR_MUTE \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_MUTE, unsigned int)
/*Audio AD BOOST Control, 1:on, 0:off*/
#define ACODEC_ENABLE_BOOSTL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_BOOSTL_ENABLE, unsigned int)
#define ACODEC_ENABLE_BOOSTR \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_BOOSTR_ENABLE, unsigned int)

#define ACODEC_GET_GAIN_MICL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_GAIN_MICL, unsigned int)
#define ACODEC_GET_GAIN_MICR \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_GAIN_MICR, unsigned int)
#define ACODEC_GET_DACL_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACL_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_DACR_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACR_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_ADCL_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_ADCL_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_ADCR_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_ADCR_VOL, ACODEC_VOL_CTRL)

/*set adcl power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_DACL \
        _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACL, unsigned int)
/*set adcr power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_DACR \
        _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACR, unsigned int)
/*set adcl power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_ADCL \
        _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_ADCL, unsigned int)
/*set adcr power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_ADCR \
        _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_ADCR, unsigned int)
/*set adcl power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_LINEINL \
        _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_LINEINL, unsigned int)
/*set adcr power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_LINEINR \
        _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_LINEINR, unsigned int)

/* Don't need to set, the driver will set a default value */
/*clock of dac and adc is reverse or obverse*/
#define ACODEC_SEL_DAC_CLK \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_DAC_CLK, unsigned int)
#define ACODEC_SEL_ADC_CLK \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_ADC_CLK, unsigned int)
/*clock of analog part and digital part is reverse or obverse*/
#define ACODEC_SEL_ANA_MCLK \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_ANA_MCLK, unsigned int)
#define ACODEC_SET_DAC_DE_EMPHASIS \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DAC_DE_EMPHASIS, unsigned int)
#define ACODEC_SET_ADC_HP_FILTER \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADC_HP_FILTER, unsigned int)

#define ACODEC_SET_INPUT_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_INPUT_VOL, unsigned int)
#define ACODEC_SET_OUTPUT_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_OUTPUT_VOL, unsigned int)
#define ACODEC_GET_INPUT_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_INPUT_VOL, unsigned int)
#define ACODEC_GET_OUTPUT_VOL \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_OUTPUT_VOL, unsigned int)

/* Reserved ioctl cmd */
#define ACODEC_SET_I2S1_DATAWIDTH \
	_IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_DATAWIDTH, unsigned int)

/**
\addtogroup MPI_AUDIO
 * @{
*/

/**
\brief  对内置 Audio Codec 通过 ioctl 提供对硬件设备的操作
\param[in]  fd              :audio codec描述符
\param[in]  request_code    :命令请求码
\param[in]  ...             :不定长的参数
\retval ::  0           :成功
\retval ::  other       :失败
\attention  新添加接口
\see \n
N/A
*/
int ar_mpi_acodec_ioctl(int fd, int request_code, ...);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef _ACODEC_H_ */
