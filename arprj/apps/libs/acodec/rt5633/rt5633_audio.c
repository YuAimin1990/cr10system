#include <stdio.h>
#include <string.h>

#include "mpi_type.h"
#include "hal_i2c.h"
#include "rt5633_audio.h"

#define RT5633_I2C_ADDR  (0x1c)
//evb board
#define I2C_MASTER_IDX   (0x0)

#define RT5633_READ_REG(reg, pdata) \
do{\
    AR_U8 u8Reg = reg; \
    AR_U8 u8Val[2] = {0}; \
    AR_S32 s32Ret = ar_hal_i2c_master_read(s32Fd, RT5633_I2C_ADDR, 8, &u8Reg , 1, &u8Val[0], 2);\
    if(s32Ret < 0)\
    {\
        printf("Read I2C 0x%x reg %x failed, error = %d\n", RT5633_I2C_ADDR, u8Reg, s32Ret);\
        ar_hal_i2c_close(s32Fd);\
        return s32Ret;\
    }\
    *pdata = ((u8Val[0] << 8) | u8Val[1]);\
}while(0)

#define RT5633_WRITE_REG(reg, data) \
do{\
    AR_U8 buf[3]; \
    buf[0] = (AR_U8)reg;\
    buf[1] = (data>>8)&0xff;\
    buf[2] = data&0xff;\
    AR_S32 s32Ret = ar_hal_i2c_master_write(s32Fd, RT5633_I2C_ADDR, 8, buf, 3);\
    if(s32Ret < 0)\
    {\
        printf("write I2C 0x%x reg %x failed, error = %d\n", RT5633_I2C_ADDR, reg, s32Ret);\
        ar_hal_i2c_close(s32Fd);\
        return s32Ret;\
    }\
}while(0)

typedef enum
{
    Codec_Master = 0,
    Codec_Slave
} CODEC_MASTER_OR_SLAVE;

static CODEC_MASTER_OR_SLAVE codec_mode = Codec_Slave;//Codec_Master;//Codec_Slave;
static int ar_rt5633_reset(void);

static AR_S32 rt5633_read_private(AR_U8 reg, AR_U8 *value)
{
    AR_S32 s32Fd = -1, ret = 0;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return s32Fd;
    }
    RT5633_WRITE_REG(ALC5633_INDEX_ADDR, reg);
    RT5633_READ_REG(ALC5633_INDEX_ADDR, value);
    ar_hal_i2c_close(s32Fd);

    return ret;
}

static void rt5633_write_private(unsigned int reg, unsigned int value)
{
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }
    RT5633_WRITE_REG(ALC5633_INDEX_ADDR, reg);
    RT5633_WRITE_REG(ALC5633_INDEX_DATA, value);
    ar_hal_i2c_close(s32Fd);
}

static void rt5633_clk_select(int use_mclk, unsigned int clk)
{
    AR_U16 value = 0;
    unsigned int value2=0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    if (use_mclk) {
        /* PLL1 power on */
        RT5633_READ_REG(ALC5633_PWR_MANAG_ADD2, &value);
        value |= PWR_PLL1_EN;
        RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD2, value);
        /* Use pll, from mclk */
        RT5633_READ_REG(ALC5633_INTER_CLK_CTRL, &value);
        value &= ~INTER_SYS_CLK_MASK;
        value |= INTER_SYS_CLK_MCLK;
        value &= ~INTER_CLK_SRC_MASK;
        value |= INTER_CLK_SRC_MCLK;
        RT5633_WRITE_REG(ALC5633_INTER_CLK_CTRL, value);
        /*
         * We consider i2s ws 48k, fs = 256, mclk = 24M
         *
         *    fout =fs * ws = 12M
         *
         *                                             n+2
         *      fout =mclk(or bclk) *  ------------
         *                                       (m+2)*(k+2)
         *    m:     0x0(bypass)
         *     n:   0x2
         *     k:   0x2
         */
         RT5633_READ_REG(ALC5633_PLL_CTRL, &value);
         RT5633_READ_REG(ALC5633_INTER_CLK_CTRL, &value2);
         switch (clk) {
         case 48000:
             printf("[I2S]: clk %d asigned to alc5633\n", clk);
             value = (0x2 << PLL1_FUNC_N_SHFT);
             value |= PLL1_FUNC_MBYPASS;
             value |= (0x2 << PLL1_FUNC_K_SHFT);
         break;
         case 44100:
             printf("[I2S]: clk %d asigned to alc5633\n", clk);
             /*
             value = (0x7 << PLL1_FUNC_N_SHFT);
             value |= PLL1_FUNC_MBYPASS;
             value |= (0x8 << PLL1_FUNC_K_SHFT);
             */
             value = (0x9 << PLL1_FUNC_N_SHFT);
             value |= PLL1_FUNC_MBYPASS;
             value |= (0x4 << PLL1_FUNC_K_SHFT);
             value2 |= INTER_CLK_PRE_DEVIDER;
         break;
         case 8000:
              printf("[I2S]: clk %d asigned to alc5633\n", clk);
              value = (0x0 << PLL1_FUNC_N_SHFT);
              value |= PLL1_FUNC_MBYPASS;
              value |= (0x4 << PLL1_FUNC_K_SHFT);
              value2 |= INTER_CLK_PRE_DEVIDER;
              break;
         default:
              printf("[I2S]: clk %d not supported in alc5633, use 48000Hz\n", clk);
              value = (0x2 << PLL1_FUNC_N_SHFT);
              value |= PLL1_FUNC_MBYPASS;
              value |= (0x2 << PLL1_FUNC_K_SHFT);
         }
         RT5633_WRITE_REG(ALC5633_INTER_CLK_CTRL, value2);
         RT5633_WRITE_REG(ALC5633_PLL_CTRL, value);
         //reg38 use default value is ok
    }else {
         /* PLL1 power on */
        RT5633_READ_REG(ALC5633_PWR_MANAG_ADD2, &value);
        value |= PWR_PLL1_EN;
        RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD2, value);

        /* Use pll, from bclk */
        RT5633_READ_REG(ALC5633_INTER_CLK_CTRL, &value);
        value &= ~INTER_SYS_CLK_MASK;
        value |= INTER_SYS_CLK_PLL;
        value &= ~INTER_CLK_SRC_MASK;
        value |= INTER_CLK_SRC_BCLK;
        RT5633_WRITE_REG(ALC5633_INTER_CLK_CTRL, value);

        /*
         * We consider i2s bclk as 3M, m=2,k=2
         *
         *                                             n+2
         *      fout =mclk(or bclk) *  ------------
         *                                       (m+2)*(k+2)
         *
         *   bclk:     3M
         *      m:     0x0(bypass)
         *       n:     0x1e
         *       k:     0x2
         *       fout: 12M
         */
        RT5633_READ_REG(ALC5633_PLL_CTRL, &value);
        switch (clk) {
        case 48000:
            printf("[I2S]: used clk %d to alc5633 pll\n", clk);
            value = (0x1e << PLL1_FUNC_N_SHFT);
            value |= PLL1_FUNC_MBYPASS;
            value |= (0x2 << PLL1_FUNC_K_SHFT);
            break;
        case 44100:
            printf("[I2S]: used clk %d to alc5633 pll\n", clk);
            value = (0x1e << PLL1_FUNC_N_SHFT);
            value |= PLL1_FUNC_MBYPASS;
            value |= (0x2 << PLL1_FUNC_K_SHFT);
            break;
        case 8000:
            printf("[I2S]: used clk %d to alc5633 pll\n", clk);
            value = (0xe << PLL1_FUNC_N_SHFT);
            value |= PLL1_FUNC_MBYPASS;
            value |= (0x0 << PLL1_FUNC_K_SHFT);
            break;
        default:
            printf("[I2S]: clk %d not supported by alc5633 pll, use 48000Hz\n", clk);
            value = (0x1e << PLL1_FUNC_N_SHFT);
            value |= PLL1_FUNC_MBYPASS;
            value |= (0x2 << PLL1_FUNC_K_SHFT);
        }
        RT5633_WRITE_REG(ALC5633_PLL_CTRL, value);
    }
    ar_hal_i2c_close(s32Fd);
}

static void rt5633_set_pcm(unsigned int wordsize, unsigned int clk)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("%s Open I2C %d failed.\n", __func__, I2C_MASTER_IDX);
        return;
    }

    if (codec_mode == Codec_Master)
        /* PLL uses mclk*/
        rt5633_clk_select(1, clk);
    else
        /* PLL uses bclk */
        rt5633_clk_select(0, clk);

    RT5633_READ_REG(ALC5633_I2S_AUDIO_PORT_CTRL, &value);
    if (codec_mode == Codec_Slave)
        value |= I2S_SLV;
    else
        value &= ~(I2S_SLV);
    value &= ~I2S_WIDTH_MASK;
    switch(wordsize) {
        case 8:
            value |= I2S_WIDHT_8BIT;
        break;
        case 16:
            value |= I2S_WIDHT_16BIT;
        break;
        case 20:
            value |= I2S_WIDHT_20BIT;
        break;
        case 24:
            value |= I2S_WIDHT_24BIT;
        break;
        default:
        break;
    }
    RT5633_WRITE_REG(ALC5633_I2S_AUDIO_PORT_CTRL, value);
    ar_hal_i2c_close(s32Fd);
}

static void rt5633_power_up(void)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD1, &value);
    value |= PWR_MAIN_I2S_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD1, value);

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD3, &value);
    value |= PWR_VREF_EN;
    value |= PWR_FASTB_EN;
    value |= PWR_MAIN_BIAS_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD3, value);
    ar_hal_i2c_close(s32Fd);
}

static void rt5633_adc_enable(int volume, int mic)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD1, &value);
    value |= PWR_ADC_L_EN;
    value |= PWR_ADC_R_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD1, value);

    /* Set adc vol */
    RT5633_READ_REG(ALC5633_ADC_VOL, &value);
    if (mic & 0x1) {
        value &= ~ADC_VOL_L_MASK;
        value |= (((0xff - volume) & 0xff) << ADC_VOL_L_SHFT) & ADC_VOL_L_MASK;
    }
    if (mic & 0x2) {
        value &= ~ADC_VOL_R_MASK;
        value |= (((0xff - volume) & 0xff) << ADC_VOL_R_SHFT) & ADC_VOL_R_MASK;
    }
    RT5633_WRITE_REG(ALC5633_ADC_VOL, value);

    ar_hal_i2c_close(s32Fd);
}

static void rt5633_dac_enable(int volume)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }
    printf("enter %s, volume = 0x%x\n", __func__, volume);
    /* DAC power on */
    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD1, &value);
    value |= PWR_DAC_L_EN;
    value |= PWR_DAC_R_EN;
    value |= PWR_DAC_REF_EN;
    value |= PWR_DAC_DF2SE_L_EN;
    value |= PWR_DAC_DF2SE_R_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD1, value);

    /* DAC control */
    RT5633_READ_REG(ALC5633_DAC_CTRL, &value);
    value &= ~MU_DAC_L;
    value &= ~MU_DAC_R;
    value &= ~MU_DA_TO_MIX;
    value &= ~DAC_PRE_BOOST_MASK;
    //value |= ((volume / 5) << DAC_PRE_BOOST_SHFT) & DAC_PRE_BOOST_MASK;
    RT5633_WRITE_REG(ALC5633_DAC_CTRL, value);

    /* Set DAC volume*/
#if 1
    RT5633_READ_REG(ALC5633_DAC_VOL, &value);
    value &= ~DAC_VOL_L_MASK;
    value |= (((0xff - volume) & 0xff) << DAC_VOL_L_SHFT) & DAC_VOL_L_MASK;
    value &= ~DAC_VOL_R_MASK;
    value |= (((0xff - volume) & 0xff) << DAC_VOL_R_SHFT) & DAC_VOL_R_MASK;
    RT5633_WRITE_REG(ALC5633_DAC_VOL, value);
#endif
    ar_hal_i2c_close(s32Fd);
}

static void r5633_enable_mic(int mic, int volume)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    if (!mic)
        return;
    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD2, &value);
    /* Enable Mic bias */
    value |= PWR_MIC_BIAS_EN;
    /* Enable Mic power */
    if (mic & 0x1)
        value |= PWR_MIC1_EN;
    if (mic & 0x2)
        value |= PWR_MIC2_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD2, value);

    /* Enable Mic boost */
    RT5633_READ_REG(ALC5633_REC_MIX_CTRL, &value);
    if (mic & 0x1) {
        value &= ~MU_BST1_TO_RECMIXL;
        value &= ~MU_BST1_TO_RECMIXR;
    }
    if (mic & 0x2) {
        value &= ~MU_BST2_TO_RECMIXL;
        value &= ~MU_BST2_TO_RECMIXR;
    }
    RT5633_WRITE_REG(ALC5633_REC_MIX_CTRL, value);

    /* Set Mic vol */
    RT5633_READ_REG(ALC5633_MIC_IN_CTRL, &value);
    if (mic & 0x1) {
        value &= ~MIC1_DF_INPUT;
        value &= ~MIC1_VOL_MSK;
        value |= ((0x1f - (volume >> 3)) << MIC1_VOL_SHFT) & MIC1_VOL_MSK;
    }
    if (mic & 0x2) {
        value &= ~MIC2_DF_INPUT;
        value &= ~MIC2_VOL_MSK;
        value |= ((0x1f - (volume >> 3)) << MIC2_VOL_SHFT) & MIC2_VOL_MSK;
    }
    RT5633_WRITE_REG(ALC5633_MIC_IN_CTRL, value);

    /* Set mic  ctrl */
    RT5633_READ_REG(ALC5633_MIC_CTRL, &value);
    if (mic & 0x1) {
        value &= ~SEL_BST1_MASK;
        value |= SEL_BST_40DB << SEL_BST1_SHFT;
    }
    if (mic & 0x2) {
        value &= ~SEL_BST2_MASK;
        value |= SEL_BST_40DB << SEL_BST2_SHFT;
    }
    RT5633_WRITE_REG(ALC5633_MIC_CTRL, value);
    ar_hal_i2c_close(s32Fd);
}

static AR_S32 rt5633_enable_auxout(int volume)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }

    /* AUX Mixer power on */
    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD2, &value);
    value |= PWR_AUXMIX_L_EN;
    value |= PWR_AUXMIX_R_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD2, value);

    /* AUX Volume power on */
    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD4, &value);
    value |= PWR_AUX_L_VOL;
    value |= PWR_AUX_R_VOL;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD4, value);

    /*Enable AUX Mixer*/
    RT5633_READ_REG(ALC5633_AUX_MIX_CTRL, &value);
    value &= ~MU_DACL_AUXMIXL;
    value &= ~MU_DACR_AUXMIXR;
    RT5633_WRITE_REG(ALC5633_MIC_IN_CTRL, value);

    /*Enable AUX Mixer, Set volume*/
    RT5633_READ_REG(ALC5633_AUX_OUT_CTRL, &value);
    value &= ~MU_AXO_L;
    value &= ~MU_AXO_R;
    value &= ~MU_AXO_L_VOL_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_AXO_L_VOL_SHFT) & MU_AXO_L_VOL_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_AXO_R_VOL_SHFT) & MU_AXO_R_VOL_MASK;
    RT5633_WRITE_REG(ALC5633_AUX_OUT_CTRL, value);

    //dac route to aux out
    RT5633_READ_REG(ALC5633_AUX_MIX_CTRL, &value);
    value &= ~(MU_DACL_AUXMIXL);
    value &= ~(MU_DACR_AUXMIXR);
    RT5633_WRITE_REG(ALC5633_AUX_MIX_CTRL, value);
    ar_hal_i2c_close(s32Fd);
}

static AR_S32 rt5633_enable_hpout(int volume)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }

//depop======================================================================
    RT5633_WRITE_REG(ALC5633_DEPOP2_CTRL, 0xb000);

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD3, &value);
    value |= PWR_VREF_EN;
    value |= PWR_MAIN_BIAS_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD3, value);

    RT5633_READ_REG(ALC5633_DEPOP1_CTRL, &value);
    value |= SOFT_GENERATER;
    RT5633_WRITE_REG(ALC5633_DEPOP1_CTRL, value);

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD3, &value);
    value |= PWR_HP_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD3, value);

    RT5633_READ_REG(ALC5633_DEPOP1_CTRL, &value);
    value |= DEPOP_MODE2_HPOUT;
    RT5633_WRITE_REG(ALC5633_DEPOP1_CTRL, value);

    usleep(300);

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD3, &value);
    value |= PWR_HP_OUT;
    value |= PWR_FASTB_EN;
    value |= PWR_HP_AMP;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD3, value);

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD4, &value);
    value |= PWR_HP_L_VOL;
    value |= PWR_HP_R_VOL;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD4, value);

    RT5633_READ_REG(ALC5633_DEPOP1_CTRL, &value);
    value &= ~DEPOP_MODE2_HPOUT;
    RT5633_WRITE_REG(ALC5633_DEPOP1_CTRL, value);

    RT5633_READ_REG(ALC5633_DEPOP1_CTRL, &value);
    value |= ENABLE_SOFT_TRIGGER;
    value |= ENABLE_HPL_UNMUTE_DEPOP;
    value |= ENABLE_HPR_UNMUTE_DEPOP;
    RT5633_WRITE_REG(ALC5633_DEPOP1_CTRL, value);

    RT5633_READ_REG(ALC5633_HP_OUT_VOL, &value);
    value &= ~MU_HP_L;
    value &= ~MU_HP_R;
    RT5633_WRITE_REG(ALC5633_HP_OUT_VOL, value);

    usleep(50);
//depop end======================================================================

    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD2, &value);
    value |= PWR_HPMIX_L_EN;
    value |= PWR_HPMIX_R_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD2, value);

    RT5633_READ_REG(ALC5633_HP_MIX_CTRL, &value);
    value &= ~MU_DACL_HPMIXL;
    value &= ~MU_DACR_HPMIXR;
    RT5633_WRITE_REG(ALC5633_HP_MIX_CTRL, value);

    RT5633_READ_REG(ALC5633_SPK_HP_OUT_MIX, &value);
    value |= MU_HP_HP;
    RT5633_WRITE_REG(ALC5633_SPK_HP_OUT_MIX, value);

    RT5633_READ_REG(ALC5633_HP_OUT_VOL, &value);
    value &= ~MU_HP_VOL_L_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_HP_VOL_L_SHFT) & DAC_VOL_L_MASK;
    value &= ~MU_HP_VOL_R_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_HP_VOL_R_SHFT) & DAC_VOL_R_MASK;
    value |= SEL_HPVOL_L_HP;
    value |= SEL_HPVOL_R_HP;
    RT5633_WRITE_REG(ALC5633_HP_OUT_VOL, value);

    ar_hal_i2c_close(s32Fd);
}

/*
 * User class D
 */
static void rt5633_enable_spkout(int volume)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }
#if 0
  /* Class D power on */
  RT5633_READ_REG(i2c_component, ALC5633_PWR_MANAG_ADD1, &value);
  value |= PWR_CLSD_EN;
  RT5633_WRITE_REG(i2c_component, ALC5633_PWR_MANAG_ADD1, value);
#endif
    /* SPK Mixer power on */
    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD2, &value);
    value |= PWR_SPKMIX_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD2, value);

    /* SPK Volume power on */
    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD4, &value);
    value |= PWR_SPK_L_VOL;
    value |= PWR_SPK_R_VOL;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD4, value);

    /* Set SPK volume*/
    RT5633_READ_REG(ALC5633_SPK_OUT_CTRL, &value);
    value &= ~MU_SPK_P;
    /* Use zero, which is the maximum volume */
#if 1
    value &= ~SPK_VOL_L_MASK;
    value |= ((0x1f - (volume >> 3)) << SPK_VOL_L_SHFT) & SPK_VOL_L_MASK;
    value &= ~SPK_VOL_R_MASK;
    value |= ((0x1f - (volume >> 3)) << DAC_VOL_R_SHFT) & SPK_VOL_R_MASK;
#endif
    value &= ~SPK_SOURCE_MASK;
    value |= SPK_SOURCE_D_RN;
    RT5633_WRITE_REG(ALC5633_SPK_OUT_CTRL, value);

    /* SPK Amplify control */
    RT5633_READ_REG(ALC5633_SPK_AMP_CTRL, &value);
    value |= ABD_MOD_D;
    RT5633_WRITE_REG(ALC5633_SPK_AMP_CTRL, value);

    /* Enable SPK Mixer */
    RT5633_READ_REG(ALC5633_SPK_MIX_CTRL, &value);
    value &= ~SPK_VOL_SEL_MASK;
    value |= SPK_VOL_SEL_SPK;
    value &= ~MU_DACL_TO_SPKMIX;
    value &= ~MU_DACR_TO_SPKMIX;
    RT5633_WRITE_REG(ALC5633_SPK_MIX_CTRL, value);
    ar_hal_i2c_close(s32Fd);
}

static void rt5633_dump_regs(void)
{
    unsigned int i, value=0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    for (i = 0; i < 0x80; i++) {
        RT5633_READ_REG(i, &value);
        printf("rt5633 reg %02x=0x%04x\n", i, value);
    }

    ar_hal_i2c_close(s32Fd);
}


static void rt5633_set_dac_mute_unmute(unsigned int left,unsigned int right)
{
    printf("enter %s, left = %d,right = %d\n", __func__, left,right);
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    RT5633_READ_REG(ALC5633_DAC_CTRL, &value);
    if (left)
        value &= ~MU_DAC_L;
    else
        value |= MU_DAC_L;
    if (right)
        value &= ~MU_DAC_R;
    else
        value |= MU_DAC_R;
    RT5633_WRITE_REG(ALC5633_DAC_CTRL, value);
    ar_hal_i2c_close(s32Fd);

}

static void rt5633_get_dac_mute_unmute(unsigned int* left, unsigned int* right)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return;
    }

    RT5633_READ_REG(ALC5633_DAC_CTRL, &value);
    *left = value & MU_DAC_L ? 0:1;
    *right = value & MU_DAC_R ? 0:1;

    printf("enter %s, left = %d,right = %d\n", __func__, *left,*right);
    ar_hal_i2c_close(s32Fd);

}

#if 0
/*
 * This is a whole register configuration for dac->speaker class D,
 */
int alc5633_spk_regs[] = {
  /*0x0         0x1         0x2         0x3        0x4        0x5         0x6         0x7         0x8        0x9         0xa         0xb        0xc          0xd        0xe         0xf*/
  0x0001, 0x0000, 0x0000, 0x0000, 0x8080, 0x0000, 0xc080, 0x0000, 0x0808, 0x0000, 0x0808, 0x0000, 0x0000, 0x0000, 0x1010, 0x0000,
  0x0808, 0x0000, 0x0000, 0x0000, 0x7f7f, 0xfc00, 0x0000, 0x0000, 0x3f3f, 0x0000, 0x3f3f, 0x0000, 0x08fc, 0x0000, 0x8000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x3100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x8000, 0x0000, 0x0000, 0x0000, 0x2000, 0x0000, 0xa7c0, 0x0003, 0xe000, 0x0000, 0xc000, 0x0000,
  0x0c00, 0x0000, 0x6000, 0x0000, 0x1ea0, 0x0000, 0x140d, 0x0000, 0x0280, 0x0000, 0x079a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x40c0, 0x0000, 0x0000, 0x0000, 0x3000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0489, 0x5ffe, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0206, 0x0000, 0x2000, 0x0000, 0x0553, 0x0000, 0x007f, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x010c, 0x0000, 0x3f00, 0x0000, 0x0002, 0x0000, 0x10ec, 0x0000, 0x6179, 0x0000,
};
#endif

int ar_rt5633_play(    int resolution, int clk, int volume)
{
#if 0
    ar_rt5633_reset();
    rt5633_power_up();
    rt5633_dac_enable(volume);
    //rt5633_enable_auxout(volume);
    //rt5633_enable_hpout(volume);
    rt5633_enable_spkout(volume);
    rt5633_set_pcm(resolution, clk);

#else
    /*
     * This is the minimal configuration for dac->spk classD
     */
    AR_S32 s32Fd = -1;
    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }
    RT5633_WRITE_REG(0x02, 0x0000);
    RT5633_WRITE_REG(0x03, 0x0000);
    RT5633_WRITE_REG(0x0e, 0x1010);
    RT5633_WRITE_REG(0x1c, 0x08fc);
    RT5633_WRITE_REG(0x1e, 0x8000);
    RT5633_WRITE_REG(0x34, 0x8000);
    RT5633_WRITE_REG(0x3a, 0xa7c0);
    RT5633_WRITE_REG(0x3b, 0x0003);
    RT5633_WRITE_REG(0x3c, 0xe000);
    RT5633_WRITE_REG(0x3e, 0xc000);
    RT5633_WRITE_REG(0x42, 0x6000);
    RT5633_WRITE_REG(0x44, 0x1ea0);
    ar_hal_i2c_close(s32Fd);
    rt5633_set_pcm(resolution, clk);
#endif

    return 0;
}

/*
 * mic_bitmap:
 * bit0: mic1
 * bit1: mic2
 */
int ar_rt5633_record(    int resolution, int clk, int mic_bitmap, int mic_vol)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }

    rt5633_power_up();
    rt5633_adc_enable(mic_vol, mic_bitmap);
    r5633_enable_mic(mic_bitmap, mic_vol);
  //rt5633_set_pcm(i2c_component, resolution, clk);
//power on line and rec
    RT5633_READ_REG(0x3b, &value);
    value |= (0xf << 8);
    RT5633_WRITE_REG(0x3b, value);
//line route to rec
    RT5633_READ_REG(0x14, &value);
    value &= ~(1 << 11);
    RT5633_WRITE_REG(0x14, value);
    RT5633_READ_REG(0x14, &value);
    value &= ~(1 << 3);
    RT5633_WRITE_REG(0x14, value);
    ar_hal_i2c_close(s32Fd);

    //rt5633_dump_regs();
    return 0;
}

static int ar_rt5633_reset(void)
{
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }

    /* Write any value*/
    RT5633_WRITE_REG(ALC5633_RESET, 0x0);
    ar_hal_i2c_close(s32Fd);
    return 0;
}

static int ar_rt5633_enable_depop(void)
{
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }
    /* Class D power on */
    RT5633_READ_REG(ALC5633_PWR_MANAG_ADD1, &value);
    value |= PWR_CLSD_EN;
    RT5633_WRITE_REG(ALC5633_PWR_MANAG_ADD1, value);
    ar_hal_i2c_close(s32Fd);

    return 0;
}

static int ar_rt5633_set_play_vol(int volume)
{
    AR_U16 value = 0;
    printf("enter %s, vol = %d\n", __func__, volume);
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }

    RT5633_READ_REG(ALC5633_DAC_VOL, &value);
    value &= ~DAC_VOL_L_MASK;
    value |= (((0xff - volume) & 0xff) << DAC_VOL_L_SHFT) & DAC_VOL_L_MASK;
    value &= ~DAC_VOL_R_MASK;
    value |= (((0xff - volume) & 0xff) << DAC_VOL_R_SHFT) & DAC_VOL_R_MASK;
    RT5633_WRITE_REG(ALC5633_DAC_VOL, value);

    RT5633_READ_REG(ALC5633_AUX_OUT_CTRL, &value);
    value &= ~MU_AXO_L_VOL_MASK;
    value &= ~MU_AXO_R_VOL_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_AXO_L_VOL_SHFT) & MU_AXO_L_VOL_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_AXO_R_VOL_SHFT) & MU_AXO_R_VOL_MASK;
    RT5633_WRITE_REG(ALC5633_AUX_OUT_CTRL, value);

    RT5633_READ_REG(ALC5633_HP_OUT_VOL, &value);
    value &= ~MU_HP_VOL_L_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_HP_VOL_L_SHFT) & DAC_VOL_L_MASK;
    value &= ~MU_HP_VOL_R_MASK;
    value |= ((0x1f - (volume >> 3)) << MU_HP_VOL_R_SHFT) & DAC_VOL_R_MASK;
    RT5633_WRITE_REG(ALC5633_HP_OUT_VOL, value);

    RT5633_READ_REG(ALC5633_SPK_OUT_CTRL, &value);
    value &= ~SPK_VOL_L_MASK;
    value |= ((0x1f - (volume >> 3)) << SPK_VOL_L_SHFT) & SPK_VOL_L_MASK;
    value &= ~SPK_VOL_R_MASK;
    value |= ((0x1f - (volume >> 3)) << DAC_VOL_R_SHFT) & SPK_VOL_R_MASK;
    RT5633_WRITE_REG(ALC5633_SPK_OUT_CTRL, value);
    ar_hal_i2c_close(s32Fd);

    return 0;
}

static int ar_rt5633_set_record_vol(int volume)
{
    printf("enter %s, vol = %d\n", __func__, volume);
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if (s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }
    /* Set adc vol */
    RT5633_READ_REG(ALC5633_ADC_VOL, &value);
    value &= ~ADC_VOL_L_MASK;
    value |= (((0xff - volume) & 0xff) << ADC_VOL_L_SHFT) & ADC_VOL_L_MASK;
    value &= ~ADC_VOL_R_MASK;
    value |= (((0xff - volume) & 0xff) << ADC_VOL_R_SHFT) & ADC_VOL_R_MASK;
    RT5633_WRITE_REG(ALC5633_ADC_VOL, value);


    /* Set Mic vol */
    RT5633_READ_REG(ALC5633_MIC_IN_CTRL, &value);
    value &= ~MIC1_DF_INPUT;
    value &= ~MIC1_VOL_MSK;
    value |= ((0x1f - (volume >> 3)) << MIC1_VOL_SHFT) & MIC1_VOL_MSK;
    value &= ~MIC2_DF_INPUT;
    value &= ~MIC2_VOL_MSK;
    value |= ((0x1f - (volume >> 3)) << MIC2_VOL_SHFT) & MIC2_VOL_MSK;
    RT5633_WRITE_REG(ALC5633_MIC_IN_CTRL, value);

    ar_hal_i2c_close(s32Fd);

    return 0;
}

static int ar_rt5633_set_sample_rate(int sample_rate)
{
    printf("enter %s, sample_rate = %d\n", __func__, sample_rate);
    if(codec_mode == Codec_Master)
        /* PLL uses mclk*/
        rt5633_clk_select(1, sample_rate);
    else
        /* PLL uses bclk */
        rt5633_clk_select(0, sample_rate);
    return 0;
}

static int ar_rt5633_set_bit_per_sample(int bit_per_sample)
{
    printf("enter %s, bit_per_sample = %d\n", __func__, bit_per_sample);
    AR_U16 value = 0;
    AR_S32 s32Fd = -1;

    s32Fd = ar_hal_i2c_open(I2C_MASTER_IDX);
    if(s32Fd < 0)
    {
        printf("Open I2C %d failed.\n", I2C_MASTER_IDX);
        return -1;
    }
    RT5633_READ_REG(ALC5633_I2S_AUDIO_PORT_CTRL, &value);
    if(codec_mode == Codec_Slave)
        value |= I2S_SLV;
    else
        value &= ~(I2S_SLV);
    value &= ~I2S_WIDTH_MASK;
        switch(bit_per_sample) {
        case 8:
            value |= I2S_WIDHT_8BIT;
        break;
        case 16:
            value |= I2S_WIDHT_16BIT;
        break;
        case 20:
            value |= I2S_WIDHT_20BIT;
        break;
        case 24:
            value |= I2S_WIDHT_24BIT;
        break;
        default:
        break;
    }
    RT5633_WRITE_REG(ALC5633_I2S_AUDIO_PORT_CTRL, value);

    ar_hal_i2c_close(s32Fd);
    return 0;
}




int ar_rt5633_init(int audio_node)
{
    unsigned char i2c_slaveaddr;
    int codec_i2s_mode;
    /*
    * Don't init twice
    */

    printf("codec i2s mode = %d\n", codec_mode);
    printf("I2C_slave addr = 0x%02x\n", RT5633_I2C_ADDR);

    ar_rt5633_reset();

    return 0;
}
