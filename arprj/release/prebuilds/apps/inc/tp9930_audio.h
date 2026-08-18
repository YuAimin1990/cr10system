#ifndef __TP9930_AUDIO_H__
#define __TP9930_AUDIO_H__

typedef enum 
{
    TP9930_SAMPLE_RATE_8K = 0,
    TP9930_SAMPLE_RATE_16K = 1,
    TP9930_SAMPLE_RATE_BUTT
} TP9930_SAMPLE_RATE_E;

typedef enum
{
    TP9930_BITS_16 = 0,
    TP9930_BITS_8 = 1,
    TP9930_BITS_BUTT
} TP9930_BITS_E;

typedef enum
{
    TP9930_I2S_MODE_SLAVE,
    TP9930_I2S_MODE_MASTER,
    TP9930_I2S_MODE_BUTT
} TP9930_I2S_MODE_E;


/**
* @brief  初始化Tp9930 AIN1-AIN4通道.
* @param  u8I2CIdx I2C bus id
* @param  u8I2CSlaveAddr 芯片I2C地址
* @param  enFs 采样频率fs
* @param  enBits 位宽 16bit
* @param  enMode TP9930 I2S的工作模式：slave or master
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 TP9930_AudioCfg4Chs(AR_U8 u8I2CIdx, AR_U8 u8I2CSlaveAddr, TP9930_SAMPLE_RATE_E enFs, TP9930_BITS_E enBits, TP9930_I2S_MODE_E enMode);

/**
* @brief  调整Tp9930 AIN1-AIN4通道的增益.
* @param  u8I2CIdx I2C bus id
* @param  u8I2CSlaveAddr 芯片I2C地址
* @param  u8Chn AIN通道，取值0-3
* @param  u8Vol 范围0-15
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 TP9930_AdjustAinVolume(AR_U8 u8I2CIdx, AR_U8 u8I2CSlaveAddr, AR_U8 u8Chn, AR_U8 u8Vol);

/**
* @brief  调整Tp9930 Aout通道的增益.
* @param  u8I2CIdx I2C bus id
* @param  u8I2CSlaveAddr 芯片I2C地址
* @param  u8Vol 范围0-15：Out = in x (1 x GAIN[3] + 1/2 x GAIN[2] + 1/4 x GAIN[1] + 1/8 x GAIN[0])
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 TP9930_AdjustAoVolume(AR_U8 u8I2CIdx, AR_U8 u8I2CSlaveAddr, AR_U8 u8Vol);


#endif
