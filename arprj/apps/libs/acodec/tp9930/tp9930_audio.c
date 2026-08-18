#include <stdio.h>
#include <string.h>

#include "mpi_type.h"
#include "hal_i2c.h"
#include "tp9930_audio.h"

#define TP9930_READ_REG(reg, pdata) \
do{\
    AR_U8 u8Reg = reg; \
    AR_S32 s32Ret = ar_hal_i2c_master_read(s32Fd,u8Addr7bit, 8,&u8Reg , 1, pdata, 1);\
    if(s32Ret < 0)\
    {\
        printf("Read I2C 0x%x reg %x failed, error = %d\n", u8Addr7bit, u8Reg, s32Ret);\
        ar_hal_i2c_close(s32Fd);\
        return s32Ret;\
    }\
}while(0)

#define TP9930_WRITE_REG(reg, data) \
do{\
    AR_U8 buf[2]; \
    buf[0] = (AR_U8)reg;\
    buf[1] = (AR_U8)data;\
    AR_S32 s32Ret = ar_hal_i2c_master_write(s32Fd,u8Addr7bit, 8, buf, 2);\
    if(s32Ret < 0)\
    {\
        printf("write I2C 0x%x reg %x failed, error = %d\n", u8Addr7bit, reg, s32Ret);\
        ar_hal_i2c_close(s32Fd);\
        return s32Ret;\
    }\
}while(0)

AR_S32 TP9930_AudioCfg4Chs(AR_U8 u8I2CIdx, AR_U8 u8I2CSlaveAddr, TP9930_SAMPLE_RATE_E enFs, TP9930_BITS_E enBits, TP9930_I2S_MODE_E enMode)
{
    AR_S32 i = 0;
    AR_U32 u8Bank = 0;
    AR_S32 s32Fd = -1;
    AR_U8 u8Data = 0;
    AR_U8 u8Addr7bit = u8I2CSlaveAddr >> 1;
    
    
    if(enFs >= TP9930_SAMPLE_RATE_BUTT || enBits >= TP9930_BITS_BUTT)
    {
        printf("Params error: fs = %d, bits = %d\n", enFs, enBits);
        return -1;
    }
    
    s32Fd = ar_hal_i2c_open(u8I2CIdx);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", s32Fd);
        return -1;
    }

    TP9930_READ_REG(0x40, &u8Bank);
    TP9930_WRITE_REG(0x40, 0x40); //apape = 1
    
    //remove pos
    for(i = 0; i < 20; i++)
    {
        TP9930_WRITE_REG(i, 0);
    }

    //ain1,2 on left, 3,4 on right ?
    if(enBits == TP9930_BITS_16)
    {
        TP9930_WRITE_REG(0x1, 1);
        TP9930_WRITE_REG(0x4, 2);
        TP9930_WRITE_REG(0x3, 3);
        TP9930_WRITE_REG(0x2, 4);
    }
    else
    {
        TP9930_WRITE_REG(0x3, 1);
        TP9930_WRITE_REG(0x7, 2);
        TP9930_WRITE_REG(0x1, 3);
        TP9930_WRITE_REG(0x5, 4);
    }
    
    usleep(5000);
    
    //data bits and sample rate cfg
    TP9930_WRITE_REG(0x17, (enBits << 2)); //ADATR is 16bit/8bit

    if(enMode == TP9930_I2S_MODE_SLAVE)
    {
        TP9930_WRITE_REG(0x1B, enBits << 6); //ADATP is 16bit/8bit
        TP9930_WRITE_REG(0x18, enFs); //MFMODE 8k/16k Hz
        TP9930_WRITE_REG(0x19, 0x2C);
    }
    else
    {
        TP9930_WRITE_REG(0x1B, 0x1 | (enBits << 6)); //ADATP is 16bit/8bit
        TP9930_WRITE_REG(0x18, 0xC8 | enFs); //MFMODE 8k/16k Hz,
        TP9930_WRITE_REG(0x19, 0xF);
        
        if(enFs == TP9930_SAMPLE_RATE_16K)
        {
            TP9930_WRITE_REG(0x14, 0x13);
            TP9930_WRITE_REG(0x15, 0x6B);
            TP9930_WRITE_REG(0x16, 0x06);//16kHz, 136B06
        }
        else
        {
            TP9930_WRITE_REG(0x14, 0x09);
            TP9930_WRITE_REG(0x15, 0xB5);
            TP9930_WRITE_REG(0x16, 0x83);//while 8kHz, 09B583h
        }
    }

    TP9930_WRITE_REG(0x1A, 0x15); //playback audio input 
    TP9930_WRITE_REG(0x37, 0x30); //mute playback, unmute ain1-4
    TP9930_WRITE_REG(0x3D, 0x1);  //reset
    
    //resume bank
    TP9930_WRITE_REG(0x40, u8Bank); 
    
    ar_hal_i2c_close(s32Fd);
    
    return 0;
}

//0x39, 0x3a is for analog input gain
AR_S32 TP9930_AdjustAinVolume(AR_U8 u8I2CIdx, AR_U8 u8I2CSlaveAddr, AR_U8 u8Chn, AR_U8 u8Vol)
{
    AR_U32 u8Bank = 0;
    AR_S32 s32Fd = -1;
    AR_U8 u8Data = 0;
    AR_U8 u8Addr7bit = u8I2CSlaveAddr >> 1;

    if(u8Chn > 3)
    {
        printf("Invalid AIN chn: %d\n", u8Chn);
        return -1;
    }
    
    s32Fd = ar_hal_i2c_open(u8I2CIdx);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", s32Fd);
        return -1;
    }
    
    TP9930_READ_REG(0x40, &u8Bank);
    TP9930_WRITE_REG(0x40, 0x40); //apape = 1

    if(u8Chn == 0)
    {
        TP9930_READ_REG(0x39, &u8Data);
        TP9930_WRITE_REG(0x39, (u8Data & 0xF0) | u8Vol);
    }
    if(u8Chn == 1)
    {
        TP9930_READ_REG(0x39, &u8Data);
        TP9930_WRITE_REG(0x39, (u8Data & 0x0F) | (u8Vol << 4));
    }
    if(u8Chn == 2)
    {
        TP9930_READ_REG(0x3A, &u8Data);
        TP9930_WRITE_REG(0x3A, (u8Data & 0xF0) | u8Vol);
    }
    if(u8Chn == 3)
    {
        TP9930_READ_REG(0x3A, &u8Data);
        TP9930_WRITE_REG(0x3A, (u8Data & 0x0F) | (u8Vol << 4));
    }

    ar_hal_i2c_close(s32Fd);
    return 0;
}

//0x38 for DAC gain (playback volume ?)
AR_S32 TP9930_AdjustAoVolume(AR_U8 u8I2CIdx, AR_U8 u8I2CSlaveAddr, AR_U8 u8Vol)
{
    AR_U32 u8Bank = 0;
    AR_S32 s32Fd = -1;
    AR_U8 u8Data = 0;
    AR_U8 u8Addr7bit = u8I2CSlaveAddr >> 1;

    s32Fd = ar_hal_i2c_open(u8I2CIdx);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", s32Fd);
        return -1;
    }
    
    TP9930_READ_REG(0x40, &u8Bank);
    TP9930_WRITE_REG(0x40, 0x40); //apape = 1

    TP9930_READ_REG(0x38, &u8Data);
    TP9930_WRITE_REG(0x38, (u8Data & 0xF0) | u8Vol);
    
    ar_hal_i2c_close(s32Fd);
    return 0;
}


