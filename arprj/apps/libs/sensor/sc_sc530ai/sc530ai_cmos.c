#if !defined(__sc530ai_CMOS_H_)
#define __sc530ai_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "hal_gpio.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define sc530ai_ID 530

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastsc530ai[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define sc530ai_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastsc530ai[dev])
#define sc530ai_SENSOR_SET_CTX(dev, pstCtx)   (g_pastsc530ai[dev] = pstCtx)
#define sc530ai_SENSOR_RESET_CTX(dev)         (g_pastsc530ai[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_aunsc530aiBusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = { .s8I2cDev = 1},
    [1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

static ISP_FSWDR_MODE_E genFSWDRMode[ISP_MAX_PIPE_NUM] =
{
    [0 ... ISP_MAX_PIPE_NUM - 1] = ISP_FSWDR_NORMAL_MODE
};

static AR_U32 gu32MaxTimeGetCnt[ISP_MAX_PIPE_NUM] = {0};
static AR_U32 g_au32InitExposure[ISP_MAX_PIPE_NUM]  = {0};
static AR_U32 g_au32LinesPer500ms[ISP_MAX_PIPE_NUM] = {0};

static AR_U16 g_au16InitWBGain[ISP_MAX_PIPE_NUM][3] = {{0}};
static AR_U16 g_au16SampleRgain[ISP_MAX_PIPE_NUM] = {0};
static AR_U16 g_au16SampleBgain[ISP_MAX_PIPE_NUM] = {0};

extern const unsigned int sc530ai_i2c_addr;
extern unsigned int sc530ai_addr_byte;
extern unsigned int sc530ai_data_byte;

typedef struct arsc530ai_STATE_S
{
    AR_U8       u8Hcg;
    AR_U32      u32BRL;
    AR_U32      u32RHS1_MAX;
    AR_U32      u32RHS2_MAX;
} sc530ai_STATE_S;

sc530ai_STATE_S g_astsc530aiState[ISP_MAX_PIPE_NUM] = {{0}};

extern void sc530ai_init(VI_PIPE ViPipe);
extern void sc530ai_exit(VI_PIPE ViPipe);
extern void sc530ai_standby(VI_PIPE ViPipe);
extern void sc530ai_restart(VI_PIPE ViPipe);
extern int  sc530ai_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc530ai_read_register(VI_PIPE ViPipe, int addr);

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
#define sc530ai_FULL_LINES_MAX  (0xFFFF)
#define sc530ai_FULL_LINES_MAX_2TO1_WDR  (0x8AA)    // considering the YOUT_SIZE and bad frame
#define sc530ai_FULL_LINES_MAX_3TO1_WDR  (0x7FC)

/*****sc530ai Register Address*****/
#define sc530ai_SHS1_ADDR  (0x3e00)
#define sc530ai_ANA_GAIN_ADDR  (0x3e09)
#define sc530ai_DIG_GAIN_ADDR  (0x3e06)
#define sc530ai_DIG_FINE_ADDR  (0x3e07)
#define sc530ai_VMAX_ADDR  (0x320e)
#define sc530ai_FLIP_MIRRO (0x3221)
#define sc530ai_INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/

#define sc530ai_VMAX_5MP60_LINEAR 1650 //(2500+sc530ai_INCREASE_LINES)
#define sc530ai_VMAX_5MP30_LINEAR (1650 * 2)
//sensor fps mode
#define sc530ai_SENSOR_2K_60FPS_LINEAR_MODE      (1)
#define sc530ai_SENSOR_2K_30FPS_LINEAR_MODE      (2)

#define sc530ai_RES_IS_5M(w, h)      ((w) <= 3000 && (h) <= 2000)

#define SENSOR_NM (0)

#define MAX_ANALOG_GAIN		83559 // 3.938 * 1024
#define MAX_DIGITAL_GAIN	4033 // 81.6 * 1024
#define MAX_TOTAL_GAIN		(MAX_ANALOG_GAIN * MAX_DIGITAL_GAIN / 1024)

static AR_S32 sc530ai_cmos_power_on(VI_PIPE ViPipe)
{

	int power_gpio=ar_hal_gpio_name_to_num(1,2,0);   //c1_0
	int reset_gpio=ar_hal_gpio_name_to_num(1,2,1);   //c1_1

	printf("sc530ai_cmos_power_on  power_gpio=%d reset_gpio=%d\n",power_gpio,reset_gpio);

	ar_hal_gpio_export(power_gpio);
	ar_hal_gpio_export(reset_gpio);

	//set the gpio dir to out
	ar_hal_gpio_set_dir(power_gpio,1);
	ar_hal_gpio_set_dir(reset_gpio,1);

	//set the gpio to low
	ar_hal_gpio_set_value(power_gpio,0);
    ar_hal_gpio_set_value(reset_gpio,0);

	//give some delay
    usleep(10000);
	ar_hal_gpio_set_value(power_gpio,1);
    usleep(10000);
    ar_hal_gpio_set_value(reset_gpio,1);
    usleep(10000);
	return 0;
}

static AR_S32 sc530ai_cmos_power_off(VI_PIPE ViPipe)
{
	int power_gpio=ar_hal_gpio_name_to_num(1,2,0);
	int reset_gpio=ar_hal_gpio_name_to_num(1,2,1);

	ar_hal_gpio_export(power_gpio);
	ar_hal_gpio_export(reset_gpio);

	printf("sc530ai_cmos_power_off  power_gpio=%d reset_gpio=%d\n",power_gpio,reset_gpio);
	//set the gpio dir to out
	ar_hal_gpio_set_dir(power_gpio,1);
	ar_hal_gpio_set_dir(reset_gpio,1);

	//set the gpio to low
	ar_hal_gpio_set_value(power_gpio,0);
    ar_hal_gpio_set_value(reset_gpio,0);
	return 0;
}

static AR_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if(pstSnsState->u8ImgMode==sc530ai_SENSOR_2K_30FPS_LINEAR_MODE)
    {
       pstAeSnsDft->f32MaxFps=30;
	   pstAeSnsDft->f32Fps=30;
    }
    else if(pstSnsState->u8ImgMode==sc530ai_SENSOR_2K_60FPS_LINEAR_MODE)
    {
       pstAeSnsDft->f32MaxFps=60;
	   pstAeSnsDft->f32Fps=60;
    }

    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = sc530ai_FULL_LINES_MAX;
	pstAeSnsDft->u32Hmax=3000;

    if (g_au32LinesPer500ms[ViPipe] == 0)
    {
        pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * pstAeSnsDft->f32Fps / 2;
    }
    else
    {
        pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];
    }

    switch (pstSnsState->enWDRMode)
    {
        case WDR_MODE_NONE:   /*linear mode*/

            pstAeSnsDft->u32MaxAgain = MAX_TOTAL_GAIN;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxDgain = 1024;
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxIntTime = 2 * pstSnsState->u32FLStd - 10;
            pstAeSnsDft->u32MinIntTime = 3;
            break;

        default:
            break;
    }

    return AR_SUCCESS;
}


/* the function of sensor set fps */
static AR_VOID cmos_fps_set(VI_PIPE ViPipe, AR_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    AR_U32 u32VMAX = sc530ai_VMAX_5MP60_LINEAR;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    switch (pstSnsState->u8ImgMode)
    {
        case sc530ai_SENSOR_2K_60FPS_LINEAR_MODE:
            if ((f32Fps <= pstAeSnsDft->f32MaxFps) && (f32Fps >= 0.5))
            {
                u32VMAX = sc530ai_VMAX_5MP60_LINEAR * pstAeSnsDft->f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support Fps: %f\n", f32Fps);
                break;
            }
            u32VMAX = (u32VMAX > sc530ai_FULL_LINES_MAX) ? sc530ai_FULL_LINES_MAX : u32VMAX;
            break;

        case sc530ai_SENSOR_2K_30FPS_LINEAR_MODE:
            if ((f32Fps <= pstAeSnsDft->f32MaxFps) && (f32Fps >= 0.5))
            {
                u32VMAX = sc530ai_VMAX_5MP30_LINEAR * pstAeSnsDft->f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support Fps: %f\n", f32Fps);
                break;
            }
            u32VMAX = (u32VMAX > sc530ai_FULL_LINES_MAX) ? sc530ai_FULL_LINES_MAX : u32VMAX;
            break;

        default:
            return;
    }

    if (WDR_MODE_NONE == pstSnsState->enWDRMode)
    {
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = ((u32VMAX & 0xFF00) >> 8);
        pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = (u32VMAX & 0xFF);
    }
    else
    {
    }

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
    }
    else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {
    }
    else
    {
        pstSnsState->u32FLStd = u32VMAX;
    }

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32MaxIntTime = 2 * pstSnsState->u32FLStd - 10;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];

    return;
}

static AR_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, AR_U32 u32FullLines,
                                       AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	return; //need todo
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
    }
    else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {
    }
    else if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
    {
    }
    else
    {
        u32FullLines = (u32FullLines > sc530ai_FULL_LINES_MAX) ? sc530ai_FULL_LINES_MAX : u32FullLines;
        pstSnsState->au32FL[0] = u32FullLines;
    }

    if (WDR_MODE_NONE == pstSnsState->enWDRMode)
    {
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = ((pstSnsState->au32FL[0] & 0xFF00) >> 8);
        pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = (pstSnsState->au32FL[0] & 0xFF);
    }
    else
    {
    }

    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    pstAeSnsDft->u32MaxIntTime = 2 * pstSnsState->au32FL[0] - 10;

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static AR_VOID cmos_inttime_update(VI_PIPE ViPipe, AR_U32 u32IntTime)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    static AR_BOOL bFirst[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = 1};
    AR_U32 u32Value = 0;

    static AR_U8 u8Count[ISP_MAX_PIPE_NUM] = {0};

    static AR_U32 u32ShortIntTime[ISP_MAX_PIPE_NUM] = {0};
    static AR_U32 u32ShortIntTime1[ISP_MAX_PIPE_NUM] = {0};
    static AR_U32 u32ShortIntTime2[ISP_MAX_PIPE_NUM] = {0};
    static AR_U32 u32LongIntTime[ISP_MAX_PIPE_NUM] = {0};

    static AR_U32 u32RHS1[ISP_MAX_PIPE_NUM]  = {0};
    static AR_U32 u32RHS2[ISP_MAX_PIPE_NUM]  = {0};

    static AR_U32 u32SHS1[ISP_MAX_PIPE_NUM]  = {0};
    static AR_U32 u32SHS2[ISP_MAX_PIPE_NUM]  = {0};
    static AR_U32 u32SHS3[ISP_MAX_PIPE_NUM]  = {0};

    AR_U32 u32YOUTSIZE;

    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

	if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {

    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {

    }else if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
    {

    }else
    {
        u32Value = u32IntTime;

		if(u32Value < 3)
            u32Value = 3;

		u32Value = u32Value * 2;

        if(u32Value > (pstSnsState->au32FL[0] * 2 - 10))
            u32Value = pstSnsState->au32FL[0] * 2 - 10;

        pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = ((u32Value >> 12) & 0x0F);
        pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = ((u32Value >> 4) & 0xFF);
        pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = ((u32Value << 4) & 0xF0);

        bFirst[ViPipe] = AR_TRUE;
    }

    return;
}

#define AGAIN_TABLE_NUM 8
#define DGAIN_TABLE_NUM 64

static AR_U32 again_table[AGAIN_TABLE_NUM] =
{
    1024,2048,2612,5223,10445,20890,41780,83559
};

static AR_U32 dgain_table[DGAIN_TABLE_NUM] =
{
    1024,1056,1089,1121,1152,1184,1217,1249,
	1280,1312,1345,1377,1408,1440,1473,1505,
	1536,1568,1601,1633,1664,1696,1729,1761,
	1792,1824,1857,1889,1920,1952,1985,2017,
	2048,2113,2176,2241,2304,2369,2432,2497,
	2560,2625,2688,2753,2816,2881,2944,3009,
	3072,3137,3200,3265,3328,3393,3456,3521,
	3584,3649,3712,3777,3840,3905,3968,4033
};

static AR_VOID cmos_again_calc_table(VI_PIPE ViPipe, AR_U32 *pu32AgainLin, AR_U32 *pu32AgainDb)
{
    int i;

    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

#if 1
	*pu32AgainDb = *pu32AgainLin;
#else

    if (*pu32AgainLin >= again_table[AGAIN_TABLE_NUM - 1])
    {
        *pu32AgainLin = again_table[AGAIN_TABLE_NUM - 1];
        *pu32AgainDb = AGAIN_TABLE_NUM - 1;
        return ;
    }

    for (i = 1; i < AGAIN_TABLE_NUM; i++)
    {
        if (*pu32AgainLin < again_table[i])
        {
            *pu32AgainLin = again_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }
#endif

    return;
}

static AR_VOID cmos_dgain_calc_table(VI_PIPE ViPipe, AR_U32 *pu32DgainLin, AR_U32 *pu32DgainDb)
{
    int i;

    CMOS_CHECK_POINTER_VOID(pu32DgainLin);
    CMOS_CHECK_POINTER_VOID(pu32DgainDb);

    if (*pu32DgainLin >= dgain_table[DGAIN_TABLE_NUM - 1])
    {
        *pu32DgainLin = dgain_table[DGAIN_TABLE_NUM - 1];
        *pu32DgainDb = DGAIN_TABLE_NUM - 1;
        return ;
    }

    for (i = 1; i < DGAIN_TABLE_NUM; i++)
    {
        if (*pu32DgainLin < dgain_table[i])
        {
            *pu32DgainLin = dgain_table[i - 1];
            *pu32DgainDb = i - 1;
            break;
        }
    }

    return;
}

static AR_VOID cmos_gains_update(VI_PIPE ViPipe, AR_U32 u32Again, AR_U32 u32Dgain)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    AR_U32 u32Tmp;

    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

	float total_gain = 0, ana_gain = 0, dig_gain = 0, dig_fine_gain = 0;
	uint32_t fix_dig_fine_dgain = 0;

	if(u32Again > MAX_TOTAL_GAIN)
		total_gain = (MAX_TOTAL_GAIN / 1024.0);
	else
		total_gain = (u32Again / 1024.0);

	if(total_gain < 2.0)
	{
		ana_gain = 1.0;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x00;
	}
	else if(total_gain < 2.55)
	{
		ana_gain = 2.0;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x01;
	}
	else if(total_gain < 5.1)
	{
		ana_gain = 2.55;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x40;
	}
	else if(total_gain < 10.2)
	{
		ana_gain = 5.1;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x48;
	}
	else if(total_gain < 20.4)
	{
		ana_gain = 10.2;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x49;
	}
	else if(total_gain < 40.8)
	{
		ana_gain = 20.4;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x4b;
	}
	else if(total_gain < 81.6)
	{
		ana_gain = 40.8;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x4f;
	}
	else
	{
		ana_gain = 81.6;
		pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x5f;
	}

	dig_gain = total_gain / ana_gain;
	if (dig_gain < 2.0)
	{
		dig_fine_gain = dig_gain;
		pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x00;
	}
	else if (dig_gain < 4.0)
	{
		dig_fine_gain = dig_gain / 2;
		pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x01;
	}

	fix_dig_fine_dgain = (uint32_t)(dig_fine_gain * 128);
	fix_dig_fine_dgain &= 0xfc;

	pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = fix_dig_fine_dgain;

    return;
}

static AR_VOID cmos_get_inttime_max(VI_PIPE ViPipe, AR_U16 u16ManRatioEnable, AR_U32 *au32Ratio, AR_U32 *au32IntTimeMax, AR_U32 *au32IntTimeMin, AR_U32 *pu32LFMaxIntTime)
{
    AR_U32 i = 0;
    AR_U32 u32IntTimeMaxTmp0 = 0;
    AR_U32 u32IntTimeMaxTmp  = 0;
    AR_U32 u32RHS2_Max = 0;
    AR_U32 u32RatioTmp = 0x40;
    AR_U32 u32ShortTimeMinLimit = 0;

    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(au32Ratio);
    CMOS_CHECK_POINTER_VOID(au32IntTimeMax);
    CMOS_CHECK_POINTER_VOID(au32IntTimeMin);
    CMOS_CHECK_POINTER_VOID(pu32LFMaxIntTime);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    u32ShortTimeMinLimit = (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) ? 4 : ((WDR_MODE_3To1_LINE == pstSnsState->enWDRMode) ? 3 : 2);

    if (u32IntTimeMaxTmp >= u32ShortTimeMinLimit)
    {
        if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
        {
            au32IntTimeMax[0] = u32IntTimeMaxTmp;
            au32IntTimeMax[1] = (pstSnsState->au32FL[0] - 2);
            au32IntTimeMin[0] = 2;
            au32IntTimeMin[1] = au32IntTimeMin[0] * au32Ratio[0] >> 6;
        }
        if (IS_LINE_WDR_MODE(pstSnsState->enWDRMode))
        {
            au32IntTimeMax[0] = u32IntTimeMaxTmp;
            au32IntTimeMax[1] = au32IntTimeMax[0] * au32Ratio[0] >> 6;
            au32IntTimeMax[2] = au32IntTimeMax[1] * au32Ratio[1] >> 6;
            au32IntTimeMax[3] = au32IntTimeMax[2] * au32Ratio[2] >> 6;
            au32IntTimeMin[0] = u32ShortTimeMinLimit;
            au32IntTimeMin[1] = au32IntTimeMin[0] * au32Ratio[0] >> 6;
            au32IntTimeMin[2] = au32IntTimeMin[1] * au32Ratio[1] >> 6;
            au32IntTimeMin[3] = au32IntTimeMin[2] * au32Ratio[2] >> 6;
        }
        else
        {
        }
    }
    else
    {
        if (1 == u16ManRatioEnable)
        {
            ISP_TRACE(AR_DBG_ERR, "Manaul ExpRatio is too large!\n");
            return;
        }
        else
        {
            u32IntTimeMaxTmp = u32ShortTimeMinLimit;

            au32IntTimeMin[0] = au32IntTimeMax[0];
            au32IntTimeMin[1] = au32IntTimeMax[1];
            au32IntTimeMin[2] = au32IntTimeMax[2];
            au32IntTimeMin[3] = au32IntTimeMax[3];
        }
    }

    return;
}

/* Only used in LINE_WDR mode */
static AR_VOID cmos_ae_fswdr_attr_set(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
    CMOS_CHECK_POINTER_VOID(pstAeFSWDRAttr);

    genFSWDRMode[ViPipe] = pstAeFSWDRAttr->enFSWDRMode;
    gu32MaxTimeGetCnt[ViPipe] = 0;
}

static AR_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return AR_SUCCESS;
}

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN 0
#define GOLDEN_BGAIN 0
static AR_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAwbSnsDft);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);
    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    pstAwbSnsDft->u16GoldenRgain = GOLDEN_RGAIN;
    pstAwbSnsDft->u16GoldenBgain = GOLDEN_BGAIN;

    pstAwbSnsDft->u16SampleRgain = g_au16SampleRgain[ViPipe];
    pstAwbSnsDft->u16SampleBgain = g_au16SampleBgain[ViPipe];

    return AR_SUCCESS;
}

static AR_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return AR_SUCCESS;
}


static ISP_CMOS_DNG_COLORPARAM_S g_stDngColorParam =
{
    {378, 256, 430},
    {439, 256, 439}
};


static AR_S32 cmos_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstDef);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
		strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_sc530ai/sc530ai_tuning_preview.bin");
		break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_3To1_LINE:
		strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_sc530ai/sc530ai_tuning_preview_hdr.bin");
        break;
    }

    pstDef->stSensorMode.u32SensorID = sc530ai_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    memcpy(&pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    switch (pstSnsState->u8ImgMode)
    {
        default:
        case sc530ai_SENSOR_2K_60FPS_LINEAR_MODE:
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 10;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 1023;

            break;
    }

    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimCols = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatCols = 2;
    pstDef->stSensorMode.stDngRawFormat.enCfaLayout = CFALAYOUT_TYPE_RECTANGULAR;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[2] = 2;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[2] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[3] = 2;
    pstDef->stSensorMode.bValidDngRawFormat = AR_TRUE;

    return AR_SUCCESS;
}

static AR_VOID cmos_set_pixel_detect(VI_PIPE ViPipe, AR_BOOL bEnable)
{

    AR_U32 u32FullLines_5Fps, u32MaxIntTime_5Fps;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        return;
    }

    else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {
        return;
    }

    else
    {
        if (sc530ai_SENSOR_2K_60FPS_LINEAR_MODE == pstSnsState->u8ImgMode)
        {
            u32FullLines_5Fps = (sc530ai_VMAX_5MP60_LINEAR * 60) / 5;
        }
        else if (sc530ai_SENSOR_2K_30FPS_LINEAR_MODE == pstSnsState->u8ImgMode)
        {
            u32FullLines_5Fps = (sc530ai_VMAX_5MP30_LINEAR * 30) / 5;
        }
        else
        {
            return;
        }
    }

    //u32FullLines_5Fps = (u32FullLines_5Fps > sc530ai_FULL_LINES_MAX) ? sc530ai_FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = 4;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sc530ai_write_register (ViPipe, sc530ai_ANA_GAIN_ADDR, 0x00);
		sc530ai_write_register (ViPipe, sc530ai_DIG_GAIN_ADDR, 0x00);
		sc530ai_write_register (ViPipe, sc530ai_DIG_FINE_ADDR, 0x80);

        sc530ai_write_register (ViPipe, sc530ai_VMAX_ADDR, (u32FullLines_5Fps & 0xFF00) >> 8);
        sc530ai_write_register (ViPipe, sc530ai_VMAX_ADDR + 1, u32FullLines_5Fps & 0xFF);

        sc530ai_write_register (ViPipe, sc530ai_SHS1_ADDR, (u32MaxIntTime_5Fps >> 12) & 0x0F);
        sc530ai_write_register (ViPipe, sc530ai_SHS1_ADDR + 1,  (u32MaxIntTime_5Fps >> 4) & 0xFF);
        sc530ai_write_register (ViPipe, sc530ai_SHS1_ADDR + 2, (u32MaxIntTime_5Fps << 4) & 0xF0);

    }
    else /* setup for ISP 'normal mode' */
    {
        pstSnsState->u32FLStd = (pstSnsState->u32FLStd > 0xFFFF) ? 0xFFFF : pstSnsState->u32FLStd;
        sc530ai_write_register (ViPipe, sc530ai_VMAX_ADDR, (pstSnsState->u32FLStd & 0xFF00) >> 8);
        sc530ai_write_register (ViPipe, sc530ai_VMAX_ADDR + 1, pstSnsState->u32FLStd & 0xFF);
        pstSnsState->bSyncInit = AR_FALSE;
    }

    return;
}

static AR_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, AR_U8 u8Mode)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = AR_FALSE;

    switch (u8Mode)
    {
        case WDR_MODE_NONE:
            pstSnsState->enWDRMode = WDR_MODE_NONE;
            ISP_TRACE(AR_DBG_ERR, "linear mode\n");
            break;

        default:
            ISP_TRACE(AR_DBG_ERR, "NOT support this mode!\n");
            return AR_FAILURE;
    }

    memset(pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime));
    return AR_SUCCESS;
}

static AR_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    AR_S32 i;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSnsRegsInfo);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if ((AR_FALSE == pstSnsState->bSyncInit) || (AR_FALSE == pstSnsRegsInfo->bConfig))
    {
        pstSnsState->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
        pstSnsState->astRegsInfo[0].unComBus.s8I2cDev = g_aunsc530aiBusInfo[ViPipe].s8I2cDev;
        pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        pstSnsState->astRegsInfo[0].u32RegNum = 8;

        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr = sc530ai_i2c_addr;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = sc530ai_addr_byte;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = sc530ai_data_byte;
        }

        //Linear Mode Regs
        pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr = sc530ai_SHS1_ADDR;
        pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr = sc530ai_SHS1_ADDR + 1;
        pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr = sc530ai_SHS1_ADDR + 2;

        pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum = 2;       //make shutter and gain effective at the same time
        pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr = sc530ai_ANA_GAIN_ADDR;  //gain
		pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr = sc530ai_DIG_GAIN_ADDR;
		pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = sc530ai_DIG_FINE_ADDR;

        pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = sc530ai_VMAX_ADDR;
        pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = sc530ai_VMAX_ADDR + 1;

        pstSnsState->bSyncInit = AR_TRUE;
    }
    else
    {
        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            if (pstSnsState->astRegsInfo[0].astI2cData[i].u32Data == pstSnsState->astRegsInfo[1].astI2cData[i].u32Data)
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_FALSE;
            }

            else
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_TRUE;
            }
        }

        if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].astI2cData[0].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[1].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[2].bUpdate = AR_TRUE;

            pstSnsState->astRegsInfo[0].astI2cData[5].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[6].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[7].bUpdate = AR_TRUE;
        }
    }

	pstSnsState->astRegsInfo[0].bConfig=pstSnsRegsInfo->bConfig;
    memcpy(pstSnsRegsInfo, &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&pstSnsState->astRegsInfo[1], &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));

    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    //set real register for aec
    if(pstSnsRegsInfo->bConfig)
	{
    	sc530ai_default_reg_init(ViPipe);
	}
    return AR_SUCCESS;
}

static AR_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    AR_U8 u8SensorImageMode = 0;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = AR_FALSE;

    if (pstSensorImageMode->f32Fps <= 30)
    {
        if (WDR_MODE_NONE == pstSnsState->enWDRMode)
        {
            if (sc530ai_RES_IS_5M(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = sc530ai_SENSOR_2K_30FPS_LINEAR_MODE;
                pstSnsState->u32FLStd = sc530ai_VMAX_5MP30_LINEAR;
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }
        else
        {
            ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                      pstSensorImageMode->u16Width,
                      pstSensorImageMode->u16Height,
                      pstSensorImageMode->f32Fps,
                      pstSnsState->enWDRMode);
            return AR_FAILURE;
        }
    }
    else if (pstSensorImageMode->f32Fps <= 60)
    {
        if (WDR_MODE_NONE == pstSnsState->enWDRMode)
        {
            if (sc530ai_RES_IS_5M(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = sc530ai_SENSOR_2K_60FPS_LINEAR_MODE;
                pstSnsState->u32FLStd = sc530ai_VMAX_5MP60_LINEAR;
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }
        else
        {
            ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                      pstSensorImageMode->u16Width,
                      pstSensorImageMode->u16Height,
                      pstSensorImageMode->f32Fps,
                      pstSnsState->enWDRMode);
            return AR_FAILURE;
        }
    }
    else
    {
    }

    if ((AR_TRUE == pstSnsState->bInit) && (u8SensorImageMode == pstSnsState->u8ImgMode))
    {
        /* Don't need to switch SensorImageMode */
        return AR_FAILURE;
    }

    pstSnsState->u8ImgMode = u8SensorImageMode;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];

    return AR_SUCCESS;
}

static AR_VOID sensor_global_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    sc530ai_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit = AR_FALSE;
    pstSnsState->bSyncInit = AR_FALSE;
    pstSnsState->u8ImgMode = sc530ai_SENSOR_2K_60FPS_LINEAR_MODE;
    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->u32FLStd = sc530ai_VMAX_5MP60_LINEAR;
    pstSnsState->au32FL[0] = sc530ai_VMAX_5MP60_LINEAR;
    pstSnsState->au32FL[1] = sc530ai_VMAX_5MP60_LINEAR;

    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
}

static AR_S32 sc530ai_cmos_set_mirror_flip(VI_PIPE ViPipe, int mode)
{
    AR_S32 ret = 0;
    //ISP_TRACE(AR_DBG_ERR, "filp_mirror=%d\n", mode);
    switch (mode)
    {
        case ISP_SNS_NORMAL:
        {
            ret = sc530ai_flip_off_mirro_off(ViPipe);
            break;
        }
        case ISP_SNS_MIRROR:
        {
            ret = sc530ai_flip_off_mirro_on(ViPipe);
            break;
        }
        case ISP_SNS_FLIP:
        {
            ret = sc530ai_flip_on_mirro_off(ViPipe);
            break;
        }
        case ISP_SNS_MIRROR_FLIP:
        {
            ret = sc530ai_flip_on_mirro_on(ViPipe);
            break;
        }

        default:
        break;

    }

    return ret;
}

static AR_S32 sc530ai_cmos_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
{
    if (!pSensorCtl)
    {
         ISP_TRACE(AR_DBG_ERR, "sensor ctl failed!\n");
         return AR_ERR_ISP_INVALID_ADDR;
    }
    AR_U8 u8CtlCode = 0;
    AR_S32 ret = 0;
    AR_S32 mode = *(AR_S32 *)pSensorCtl->pCtlData;
    u8CtlCode = pSensorCtl->u8CtlCode;

    switch(u8CtlCode)
    {
        case SENSOR_FLIP_MIRROR:
        {
            ret = sc530ai_cmos_set_mirror_flip(ViPipe, mode);
            break;
        }
        default:
        ISP_TRACE(AR_DBG_ERR, "error ctl code\n");
        ret = AR_FAILURE;
        break;
    }

    return ret;
}

static AR_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);

    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = sc530ai_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = sc530ai_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;

    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;
    pstSensorExpFunc->pfn_cmos_sns_power_on = sc530ai_cmos_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = sc530ai_cmos_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = sc530ai_cmos_sensor_ctl;
    return AR_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static AR_S32 sc530ai_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_aunsc530aiBusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

    return AR_SUCCESS;
}

static AR_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    sc530ai_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (AR_NULL == pastSnsStateCtx)
    {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (AR_NULL == pastSnsStateCtx)
        {
            ISP_TRACE(AR_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

    sc530ai_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    sc530ai_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    sc530ai_SENSOR_RESET_CTX(ViPipe);
}

static AR_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = sensor_ctx_init(ViPipe);

    if (AR_SUCCESS != s32Ret)
    {
        return AR_FAILURE;
    }

    stSnsAttrInfo.eSensorId = sc530ai_ID;

    s32Ret  = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);

    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);

    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);

    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, sc530ai_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, sc530ai_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, sc530ai_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(ViPipe);

    return AR_SUCCESS;
}

static AR_S32 sensor_set_init(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr)
{
    CMOS_CHECK_POINTER(pstInitAttr);

    g_au32InitExposure[ViPipe] = pstInitAttr->u32Exposure;
    g_au32LinesPer500ms[ViPipe] = pstInitAttr->u32LinesPer500ms;
    g_au16InitWBGain[ViPipe][0] = pstInitAttr->u16WBRgain;
    g_au16InitWBGain[ViPipe][1] = pstInitAttr->u16WBGgain;
    g_au16InitWBGain[ViPipe][2] = pstInitAttr->u16WBBgain;
    g_au16SampleRgain[ViPipe] = pstInitAttr->u16SampleRgain;
    g_au16SampleBgain[ViPipe] = pstInitAttr->u16SampleBgain;

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S stSnssc530aiObj =
{
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = sc530ai_standby,
    .pfnRestart             = sc530ai_restart,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = sc530ai_write_register,
    .pfnReadReg             = sc530ai_read_register,
    .pfnSetBusInfo          = sc530ai_set_bus_info,
    .pfnSetInit             = sensor_set_init
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __sc530ai_CMOS_H_ */
