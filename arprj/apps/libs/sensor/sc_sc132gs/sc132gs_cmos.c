#if !defined(__sc132gs_CMOS_H_)
#define __sc132gs_CMOS_H_

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


#define sc132gs_ID 0x0132

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastsc132gs[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define sc132gs_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastsc132gs[dev])
#define sc132gs_SENSOR_SET_CTX(dev, pstCtx)   (g_pastsc132gs[dev] = pstCtx)
#define sc132gs_SENSOR_RESET_CTX(dev)         (g_pastsc132gs[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_aunsc132gsBusInfo[ISP_MAX_PIPE_NUM] =
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

extern const unsigned int sc132gs_i2c_addr;
extern unsigned int sc132gs_addr_byte;
extern unsigned int sc132gs_data_byte;

typedef struct arsc132gs_STATE_S
{
    AR_U8       u8Hcg;
    AR_U32      u32BRL;
    AR_U32      u32RHS1_MAX;
    AR_U32      u32RHS2_MAX;
} sc132gs_STATE_S;

sc132gs_STATE_S g_astsc132gsState[ISP_MAX_PIPE_NUM] = {{0}};

extern void sc132gs_init(VI_PIPE ViPipe);
extern void sc132gs_exit(VI_PIPE ViPipe);
extern void sc132gs_standby(VI_PIPE ViPipe);
extern void sc132gs_restart(VI_PIPE ViPipe);
extern int  sc132gs_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc132gs_read_register(VI_PIPE ViPipe, int addr);
extern void sc132gs_default_reg_init(VI_PIPE ViPipe);
extern int  sc132gs_flip_off_mirro_off(VI_PIPE ViPipe);
extern int  sc132gs_flip_off_mirro_on(VI_PIPE ViPipe);
extern int  sc132gs_flip_on_mirro_off(VI_PIPE ViPipe);
extern int  sc132gs_flip_on_mirro_on(VI_PIPE ViPipe);
extern int sc132gs_write_callback(VI_PIPE ViPipe, int addr, int data);

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
#define sc132gs_FULL_LINES_MAX  (0x7FFF) // 0xFFFF

#define sc132gs_FULL_LINES_MAX_2TO1_WDR  (0x8AA)    // considering the YOUT_SIZE and bad frame
#define sc132gs_FULL_LINES_MAX_3TO1_WDR  (0x7FC)

/*****sc132gs Register Address*****/
#define sc132gs_SHS1_ADDR  (0x3e00)
#define sc132gs_SHS2_ADDR1 (0x3e22)
#define sc132gs_SHS2_ADDR2 (0x3e04)
#define sc132gs_SHS2_ADDR3 (0x3e05)
#define sc132gs_SHS3_ADDR  (0x3e50)

#define sc132gs_ANA_GAIN_ADDR  (0x3e08)
#define sc132gs_ANA_FINE_ADDR  (0x3e09)
#define sc132gs_DIG_GAIN_ADDR  (0x3e06)
#define sc132gs_DIG_FINE_ADDR  (0x3e07)

#define sc132gs_VMAX_ADDR  (0x320e)
#define sc132gs_HMAX_ADDR  (0x320c)

#define sc132gs_RHS1_ADDR  (0x3030)
#define sc132gs_RHS2_ADDR  (0x3034)

#define sc132gs_Y_OUT_SIZE_ADDR (0x3418)
#define sc132gs_FLIP_MIRRO (0x3221)
#define sc132gs_INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/

#define sc132gs_VMAX_960P30_LINEAR  2400

//sensor fps mode
#define sc132gs_SENSOR_960P_30FPS_LINEAR_MODE      (1)


#define sc132gs_RES_IS_960x1280(w, h)      ((w) <= 960 && (h) <= 1280)

#define SENSOR_NM (0)


static AR_S32 sc132gs_cmos_power_on(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{

	int power_gpio=ar_hal_gpio_name_to_num(0,2,0);   //c0_0
	int reset_gpio=ar_hal_gpio_name_to_num(1,2,0);   //c1_0


	printf("sc132gs_cmos_power_on  power_gpio=%d reset_gpio=%d\n",power_gpio,reset_gpio);

	//ar_hal_gpio_export(power_gpio);
	ar_hal_gpio_export(reset_gpio);


	//set the gpio dir to out
	//ar_hal_gpio_set_dir(power_gpio,1);
	ar_hal_gpio_set_dir(reset_gpio,1);

	//set the gpio to low
	//ar_hal_gpio_set_value(power_gpio,0);
    ar_hal_gpio_set_value(reset_gpio,0);

	//give some delay
    //usleep(10000);
	//ar_hal_gpio_set_value(power_gpio,1);
    usleep(10000);
    ar_hal_gpio_set_value(reset_gpio,1);
    usleep(10000);
	return 0;
}

static AR_S32 sc132gs_cmos_power_on_test(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{

	int power_gpio=ar_hal_gpio_name_to_num(1,2,0);   //c1_0


	printf("sc132gs_cmos_power_on  power_gpio=%d\n",power_gpio);

	ar_hal_gpio_export(power_gpio);

	//set the gpio dir to out
	ar_hal_gpio_set_dir(power_gpio,1);

	//set the gpio to low
	ar_hal_gpio_set_value(power_gpio,0);

	//give some delay
    usleep(30000);
	ar_hal_gpio_set_value(power_gpio,1);
    usleep(10000);
	return 0;
}

static AR_S32 sc132gs_cmos_power_off(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{

	int power_gpio=ar_hal_gpio_name_to_num(1,2,3);
	int reset_gpio=ar_hal_gpio_name_to_num(1,2,0);

	//ar_hal_gpio_export(power_gpio);
	ar_hal_gpio_export(reset_gpio);

	printf("sc132gs_cmos_power_off  power_gpio=%d reset_gpio=%d\n",power_gpio,reset_gpio);
	//set the gpio dir to out
	//ar_hal_gpio_set_dir(power_gpio,1);
	ar_hal_gpio_set_dir(reset_gpio,1);

	//set the gpio to low
	//ar_hal_gpio_set_value(power_gpio,0);
    ar_hal_gpio_set_value(reset_gpio,0);
	return 0; 
}

static AR_S32 sc132gs_cmos_power_off_test(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{

	int power_gpio=ar_hal_gpio_name_to_num(1,2,0);

	ar_hal_gpio_export(power_gpio);

	printf("sc132gs_cmos_power_off  power_gpio=%d \n",power_gpio);
	
    //set the gpio dir to out
	ar_hal_gpio_set_dir(power_gpio,1);

	//set the gpio to low
	ar_hal_gpio_set_value(power_gpio,0);
	return 0; 
}


static AR_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

	pstAeSnsDft->f32Fps=30;
	pstAeSnsDft->f32MaxFps=30;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = sc132gs_FULL_LINES_MAX;
	pstAeSnsDft->u32Hmax=1500;

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

            pstAeSnsDft->u32MaxAgain = 29232;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxDgain = 32556;
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            pstAeSnsDft->u32MinIntTime = 1;
            break;

        case WDR_MODE_2To1_LINE:

            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;
            pstAeSnsDft->u32MaxAgain = 62564;
            pstAeSnsDft->u32MinAgain = 0;
            pstAeSnsDft->u32MaxDgain = 38577;
            pstAeSnsDft->u32MinDgain = 0;
            break;

        case WDR_MODE_3To1_LINE:

            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            pstAeSnsDft->u32MinIntTime = 3;
            pstAeSnsDft->u32MaxAgain = 62564;
            pstAeSnsDft->u32MinAgain = 0;
            pstAeSnsDft->u32MaxDgain = 38577;
            pstAeSnsDft->u32MinDgain = 0;
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
    AR_U32 u32VMAX = sc132gs_VMAX_960P30_LINEAR;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    switch (pstSnsState->u8ImgMode)
    {
        case sc132gs_SENSOR_960P_30FPS_LINEAR_MODE:
            if ((f32Fps <= pstAeSnsDft->f32MaxFps) && (f32Fps >= 0.5))
            {
                u32VMAX = sc132gs_VMAX_960P30_LINEAR * pstAeSnsDft->f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
            }
            else
            {
                ISP_TRACE(AR_DBG_ERR, "Not support Fps: %f, %f\n", f32Fps, pstAeSnsDft->f32MaxFps);
                break;
            }
            u32VMAX = (u32VMAX > sc132gs_FULL_LINES_MAX) ? sc132gs_FULL_LINES_MAX : u32VMAX;
            break;

        default:
            return;
    }

    if (WDR_MODE_NONE == pstSnsState->enWDRMode)
    {
        pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = ((u32VMAX & 0xFF00) >> 8);
        pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = (u32VMAX & 0xFF);
    }
    else
    {

    }

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        pstSnsState->u32FLStd = u32VMAX * 2;
        //printf("gu32FullLinesStd:%d\n",pstSnsState->u32FLStd);

        /*
            RHS1 limitation:
            2n + 5
            RHS1 <= FSC - BRL*2 -21
            (2 * VMAX_sc132gs_1080P30_WDR - 2 * gu32BRL - 21) - (((2 * VMAX_sc132gs_1080P30_WDR - 2 * 1109 - 21) - 5) %2)
        */

        g_astsc132gsState[ViPipe].u32RHS1_MAX = (u32VMAX - g_astsc132gsState[ViPipe].u32BRL) * 2 - 21;

    }

    else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {
        pstSnsState->u32FLStd = u32VMAX * 4;

        //printf("u32VMAX:%d gu32FullLinesStd:%d\n",u32VMAX,pstSnsState->u32FLStd);

        /*
            RHS2 limitation:
            3n + 14
            RHS2 <= FSC - BRL*3 -25
        */
        g_astsc132gsState[ViPipe].u32RHS2_MAX = u32VMAX * 4 - g_astsc132gsState[ViPipe].u32BRL * 3 - 25;

    }
    else
    {
        pstSnsState->u32FLStd = u32VMAX;
    }

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];

    printf("%s %d: f32Fps %f, f32MaxFps %f, u32FLStd %d\n",__func__, __LINE__, f32Fps,pstAeSnsDft->f32MaxFps,pstSnsState->u32FLStd);

    return;

}

static AR_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, AR_U32 u32FullLines,
                                       AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	return;//need to do
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        u32FullLines = (u32FullLines > 2 * sc132gs_FULL_LINES_MAX_2TO1_WDR) ? 2 * sc132gs_FULL_LINES_MAX_2TO1_WDR : u32FullLines;
        pstSnsState->au32FL[0] = (u32FullLines >> 1) << 1;
        g_astsc132gsState[ViPipe].u32RHS1_MAX = pstSnsState->au32FL[0] - g_astsc132gsState[ViPipe].u32BRL * 2 - 21;
    }
    else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {
        u32FullLines = (u32FullLines > 4 * sc132gs_FULL_LINES_MAX_3TO1_WDR) ? 4 * sc132gs_FULL_LINES_MAX_3TO1_WDR : u32FullLines;
        pstSnsState->au32FL[0] = (u32FullLines >> 2) << 2;
        g_astsc132gsState[ViPipe].u32RHS2_MAX = pstSnsState->au32FL[0] - g_astsc132gsState[ViPipe].u32BRL * 3 - 25;
    }
    else if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
    {
        u32FullLines = (u32FullLines > sc132gs_FULL_LINES_MAX) ? sc132gs_FULL_LINES_MAX : u32FullLines;
        pstSnsState->au32FL[0] = u32FullLines;
    }
    else
    {
        u32FullLines = (u32FullLines > sc132gs_FULL_LINES_MAX) ? sc132gs_FULL_LINES_MAX : u32FullLines;
        pstSnsState->au32FL[0] = u32FullLines;
    }

    if (WDR_MODE_NONE == pstSnsState->enWDRMode)
    {
        pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = ((pstSnsState->au32FL[0] & 0xFF00) >> 8);
        pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = (pstSnsState->au32FL[0] & 0xFF);
    }
    else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
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
    }

    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - 2;

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static AR_VOID cmos_inttime_update(VI_PIPE ViPipe, AR_U32 u32IntTime)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    // static AR_BOOL bFirst[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = 1};
    AR_U32 u32Value = 0;

    // static AR_U8 u8Count[ISP_MAX_PIPE_NUM] = {0};

    // static AR_U32 u32ShortIntTime[ISP_MAX_PIPE_NUM] = {0};
    // static AR_U32 u32ShortIntTime1[ISP_MAX_PIPE_NUM] = {0};
    // static AR_U32 u32ShortIntTime2[ISP_MAX_PIPE_NUM] = {0};
    // static AR_U32 u32LongIntTime[ISP_MAX_PIPE_NUM] = {0};

    // static AR_U32 u32RHS1[ISP_MAX_PIPE_NUM]  = {0};
    // static AR_U32 u32RHS2[ISP_MAX_PIPE_NUM]  = {0};

    // static AR_U32 u32SHS1[ISP_MAX_PIPE_NUM]  = {0};
    // static AR_U32 u32SHS2[ISP_MAX_PIPE_NUM]  = {0};
    // static AR_U32 u32SHS3[ISP_MAX_PIPE_NUM]  = {0};

    // AR_U32 u32YOUTSIZE;

    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
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

		if(u32Value < 4)
            u32Value = 4;
        if(u32Value > (pstSnsState->au32FL[0] / 2 - 4))
            u32Value = pstSnsState->au32FL[0] / 2 - 4;

		u32Value = u32Value * 2;

        pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = ((u32Value >> 12) & 0x0F);
        pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = ((u32Value >> 4) & 0xFF);
        pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = ((u32Value << 4) & 0xF0);

        // bFirst[ViPipe] = AR_TRUE;
    }

    //printf("update inittime u32InitTime %d, u32Value %d\n",u32IntTime,u32Value);

    return;
}

static AR_U32 again_table[] =
{
1024,
1056,
1089,
1121,
1152,
1184,
1217,
1249,
1280,
1312,
1345,
1377,
1408,
1440,
1473,
1505,
1536,
1568,
1601,
1633,
1664,
1696,
1729,
1761,
1792,
1828,
1857,
1919,
1973,
2030,
2088,
2147,
2204,
2263,
2321,
2378,
2437,
2495,
2552,
2611,
2667,
2726,
2785,
2842,
2900,
2958,
3015,
3074,
3132,
3189,
3248,
3306,
3363,
3422,
3479,
3537,
3596,
3653,
3712,
3827,
3944,
4060,
4175,
4291,
4408,
4524,
4639,
4756,
4872,
4987,
5103,
5220,
5336,
5451,
5568,
5684,
5799,
5915,
6032,
6148,
6263,
6379,
6496,
6611,
6727,
6844,
6960,
7075,
7191,
7308,
7424,
7656,
7887,
8120,
8351,
8584,
8815,
9048,
9280,
9511,
9744,
9975,
10208,
10439,
10672,
10903,
11136,
11368,
11599,
11832,
12063,
12296,
12527,
12760,
12992,
13223,
13456,
13687,
13920,
14151,
14384,
14615,
14848,
15311,
15775,
16239,
16704,
17168,
17632,
18096,
18560,
19023,
19487,
19951,
20416,
20880,
21344,
21808,
22272,
22735,
23199,
23663,
24128,
24592,
25056,
25520,
25984,
26447,
26911,
27375,
27840,
28304,
28768,
29232,
};

static AR_U32 dgain_table[] =
{
1024,
1055,
1088,
1120,
1152,
1183,
1216,
1248,
1280,
1311,
1344,
1376,
1408,
1439,
1472,
1504,
1536,
1567,
1600,
1632,
1664,
1695,
1728,
1760,
1792,
1823,
1856,
1888,
1920,
1951,
1984,
2016,
2048,
2112,
2176,
2240,
2304,
2368,
2432,
2496,
2560,
2624,
2688,
2752,
2816,
2880,
2944,
3008,
3072,
3136,
3200,
3264,
3328,
3392,
3456,
3520,
3584,
3648,
3712,
3776,
3840,
3904,
3968,
4032,
4096,
4224,
4352,
4480,
4608,
4736,
4864,
4992,
5120,
5248,
5376,
5504,
5632,
5760,
5888,
6016,
6144,
6272,
6400,
6528,
6656,
6784,
6912,
7040,
7168,
7296,
7424,
7552,
7680,
7808,
7936,
8064,
8192,
8448,
8704,
8960,
9216,
9472,
9728,
9984,
10240,
10496,
10752,
11008,
11264,
11520,
11776,
12032,
12288,
12544,
12800,
13056,
13312,
13568,
13824,
14080,
14336,
14592,
14848,
15104,
15360,
15616,
15872,
16128,
16384,
16896,
17408,
17920,
18432,
18944,
19456,
19968,
20480,
20992,
21504,
22016,
22528,
23040,
23552,
24064,
24576,
25088,
25600,
26112,
26624,
27136,
27648,
28160,
28672,
29184,
29696,
30208,
30720,
31232,
31744,
32256,
};


#define COUNTOF(xs) (sizeof(xs)/sizeof((xs)[0]))

static AR_VOID cmos_again_calc_table(VI_PIPE ViPipe, AR_U32 *pu32AgainLin, AR_U32 *pu32AgainDb)
{
    int i;

    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

    if (*pu32AgainLin >= again_table[COUNTOF(again_table)-1])
    {
        *pu32AgainLin = again_table[COUNTOF(again_table)-1];
        *pu32AgainDb = COUNTOF(again_table)-1;
        return ;
    }

    for (i = 1; i < COUNTOF(again_table); i++)
    {
        if (*pu32AgainLin < again_table[i])
        {
            *pu32AgainLin = again_table[i - 1];
            *pu32AgainDb = i - 1;
            break;
        }
    }
    return;
}

static AR_VOID cmos_dgain_calc_table(VI_PIPE ViPipe, AR_U32 *pu32DgainLin, AR_U32 *pu32DgainDb)
{
    int i;

    CMOS_CHECK_POINTER_VOID(pu32DgainLin);
    CMOS_CHECK_POINTER_VOID(pu32DgainDb);

    if (*pu32DgainLin >= dgain_table[COUNTOF(dgain_table)-1])
    {
        *pu32DgainLin = dgain_table[COUNTOF(dgain_table)-1];
        *pu32DgainDb = COUNTOF(dgain_table)-1;
        return ;
    }

    for (i = 1; i < COUNTOF(dgain_table); i++)
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
    // AR_U32 u32Tmp;

    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    //printf("gains update again %d, dgain %d\n",u32Again, u32Dgain);

    if ((u32Again >= 0) && (u32Again < 26))
    {
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x03; //ana gain
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x20 + u32Again; //ana fine gain
    }
	else if ((u32Again >= 26) && (u32Again < 58))
    {
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x23;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x20 + (u32Again - 26);
    }
	else if ((u32Again >= 58) && (u32Again < 90))
    {
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x27;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x20 + (u32Again - 58);
    }
	else if ((u32Again >= 90) && (u32Again < 122))
    {
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x2f;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x20 + (u32Again - 90);
    }
	else if ((u32Again >= 122) && (u32Again < 154))
    {
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x3f;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x20 + (u32Again - 122);
    }


    if ((u32Dgain >= 0) && (u32Dgain < 32))
    {
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = 0x00; //ana gain
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 0x80 + u32Dgain*4; //ana fine gain
    }
	else if ((u32Dgain >= 32) && (u32Dgain < 64))
    {
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = 0x01;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 0x80 + (u32Dgain - 32)*4;
    }
	else if ((u32Dgain >= 64) && (u32Dgain < 96))
    {
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = 0x03;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 0x80 + (u32Dgain - 64)*4;
    }
	else if ((u32Dgain >= 96) && (u32Dgain < 128))
    {
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = 0x07;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 0x80 + (u32Dgain - 96)*4;
    }
	else if ((u32Dgain >= 128) && (u32Dgain < 160))
    {
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = 0x0f;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 0x80 + (u32Dgain - 128)*4;
    }

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
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    u32ShortTimeMinLimit = (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) ? 2 : ((WDR_MODE_3To1_LINE == pstSnsState->enWDRMode) ? 3 : 2);

    if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
    {
        u32IntTimeMaxTmp = ((pstSnsState->au32FL[0] - 2) << 6) / DIV_0_TO_1(au32Ratio[0]);
    }
    else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        /*  limitation for line base WDR

            SHS1 limitation:
            2 or more
            RHS1 - 2 or less

            SHS2 limitation:
            RHS1 + 2 or more
            FSC - 2 or less

            RHS1 Limitation
            2n + 5 (n = 0,1,2...)
            RHS1 <= FSC - BRL * 2 - 21

            short exposure time = RHS1 - (SHS1 + 1) <= RHS1 - 3
            long exposure time = FSC - (SHS2 + 1) <= FSC - (RHS1 + 3)
            ExposureShort + ExposureLong <= FSC - 6
            short exposure time <= (FSC - 6) / (ratio + 1)
        */
        if (ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode[ViPipe])
        {
            u32IntTimeMaxTmp0 = pstSnsState->au32FL[1] - 6 - pstSnsState->au32WDRIntTime[0];
            u32IntTimeMaxTmp = pstSnsState->au32FL[0] - 10;
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
            au32IntTimeMax[0] = u32IntTimeMaxTmp;
            au32IntTimeMin[0] = u32ShortTimeMinLimit;
            return;
        }
        else
        {
            u32IntTimeMaxTmp0 = ((pstSnsState->au32FL[1] - 6 - pstSnsState->au32WDRIntTime[0]) * 0x40)  / DIV_0_TO_1(au32Ratio[0]);
            u32IntTimeMaxTmp = ((pstSnsState->au32FL[0] - 6) * 0x40)  / DIV_0_TO_1(au32Ratio[0] + 0x40);
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp > (g_astsc132gsState[ViPipe].u32RHS1_MAX - 3)) ? (g_astsc132gsState[ViPipe].u32RHS1_MAX - 3) : u32IntTimeMaxTmp;
            u32IntTimeMaxTmp = (0 == u32IntTimeMaxTmp) ? 1 : u32IntTimeMaxTmp;
        }

    }
    else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
    {
        /*  limitation for DOL 3t1

            SHS1 limitation:
            3 or more
            RHS1 - 2 or less

            RHS1 Limitation
            3n + 7 (n = 0,1,2...)

            SHS2 limitation:
            RHS1 + 3 or more
            RHS2 - 2 or less

            RHS2 Limitation
            3n + 14 (n = 0,1,2...)
            RHS2 <= FSC - BRL * 3 - 25

            SHS3 limitation:
            RHS2 + 3 or more
            FSC - 2 or less

            short exposure time 1 = RHS1 - (SHS1 + 1) <= RHS1 - 4
            short exposure time 2 = RHS2 - (SHS2 + 1) <= RHS2 - (RHS1 + 4)
            short exposure time 2 <= (RHS2 - 8) / (ratio[0] + 1)
            long exposure time = FSC - (SHS3 + 1) <= FSC - (RHS2 + 4)
            short exposure time 1 + short exposure time 2 + long exposure time <= FSC -12
            short exposure time 2 <= (FSC - 12) / (ratio[0]*ratio[1] + ratio[0] + 1)
        */
        if (ISP_FSWDR_LONG_FRAME_MODE == genFSWDRMode[ViPipe])
        {
            /* when change LongFrameMode, the first 2 frames must limit the MaxIntTime to avoid flicker */
            if (gu32MaxTimeGetCnt[ViPipe] < 2)
            {
                u32IntTimeMaxTmp0 = pstSnsState->au32FL[1] - 100 - pstSnsState->au32WDRIntTime[0] - pstSnsState->au32WDRIntTime[1];
            }
            else
            {
                u32IntTimeMaxTmp0 = pstSnsState->au32FL[1] - 16 - pstSnsState->au32WDRIntTime[0] - pstSnsState->au32WDRIntTime[1];
            }
            u32IntTimeMaxTmp = pstSnsState->au32FL[0] - 24;
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
            au32IntTimeMax[0] = u32IntTimeMaxTmp;
            au32IntTimeMin[0] = u32ShortTimeMinLimit;
            gu32MaxTimeGetCnt[ViPipe]++;
            return;
        }
        else
        {
            u32IntTimeMaxTmp0 = ((pstSnsState->au32FL[1] - 16 - pstSnsState->au32WDRIntTime[0] - pstSnsState->au32WDRIntTime[1]) * 0x40 * 0x40)  / DIV_0_TO_1(au32Ratio[0] * au32Ratio[1]);
            u32IntTimeMaxTmp = ((pstSnsState->au32FL[0] - 16) * 0x40 * 0x40)  / (au32Ratio[0] * au32Ratio[1] + au32Ratio[0] * 0x40 + 0x40 * 0x40);
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp0 < u32IntTimeMaxTmp) ? u32IntTimeMaxTmp0 : u32IntTimeMaxTmp;
            u32RHS2_Max = ((g_astsc132gsState[ViPipe].u32RHS2_MAX - 8) * 0x40) / (au32Ratio[0] + 0x40);
            u32IntTimeMaxTmp = (u32IntTimeMaxTmp > u32RHS2_Max) ? (u32RHS2_Max) : u32IntTimeMaxTmp;
            u32IntTimeMaxTmp = (0 == u32IntTimeMaxTmp) ? 1 : u32IntTimeMaxTmp;
        }

    }
    else
    {
    }

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

            if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
            {
                for (i = 0x40; i <= 0xFFF; i++)
                {
                    if ((u32IntTimeMaxTmp * i >> 6) > (pstSnsState->au32FL[0] - 2))
                    {
                        //u32RatioTmp = i - 1;
                        break;
                    }
                }

                au32IntTimeMax[0] = u32IntTimeMaxTmp;
                au32IntTimeMax[1] = (pstSnsState->au32FL[0] - 2);
            }
            else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
            {
                u32RatioTmp = 0xFFF;
                au32IntTimeMax[0] = u32IntTimeMaxTmp;
                au32IntTimeMax[1] = au32IntTimeMax[0] * u32RatioTmp >> 6;
            }
            else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
            {
                for (i = 0x40; i <= 0xFFF; i++)
                {
                    if ((u32IntTimeMaxTmp + (u32IntTimeMaxTmp * i >> 6) + (u32IntTimeMaxTmp * i * i >> 12)) > (pstSnsState->au32FL[0] - 12))
                    {
                        u32RatioTmp = i - 1;
                        break;
                    }
                }
                au32IntTimeMax[0] = u32IntTimeMaxTmp;
                au32IntTimeMax[1] = au32IntTimeMax[0] * u32RatioTmp >> 6;
                au32IntTimeMax[2] = au32IntTimeMax[1] * u32RatioTmp >> 6;
            }
            else
            {
            }
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
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
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
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
		strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_sc132gs/sc132gs_tuning_preview_fam30.bin");
		break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_3To1_LINE:
		strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_sc132gs/sc132gs_tuning_preview_hdr.bin");
        break;
    }

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        pstDef->stWdrSwitchAttr.au32ExpRatio[0] = 0x40;
        pstDef->stWdrSwitchAttr.au32ExpRatio[1] = 0x40;
        pstDef->stWdrSwitchAttr.au32ExpRatio[2] = 0x40;
    }

    pstDef->stSensorMode.u32SensorID = sc132gs_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    memcpy(&pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    switch (pstSnsState->u8ImgMode)
    {
        default:
        case sc132gs_SENSOR_960P_30FPS_LINEAR_MODE:
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 8; // FIXME:
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

    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    printf("Vipipe %d, Enable %d, WDRMode %d\n",ViPipe, bEnable, pstSnsState->enWDRMode);

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
        if (sc132gs_SENSOR_960P_30FPS_LINEAR_MODE == pstSnsState->u8ImgMode)
        {
            u32FullLines_5Fps = (sc132gs_VMAX_960P30_LINEAR * 30) / 5;
        }
        else
        {
            return;
        }
    }

    //u32FullLines_5Fps = (u32FullLines_5Fps > sc132gs_FULL_LINES_MAX) ? sc132gs_FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = 4;

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        sc132gs_write_register (ViPipe, sc132gs_ANA_GAIN_ADDR, 0x00);
		sc132gs_write_register (ViPipe, sc132gs_ANA_FINE_ADDR, 0x00);
		sc132gs_write_register (ViPipe, sc132gs_DIG_GAIN_ADDR, 0x00);
		sc132gs_write_register (ViPipe, sc132gs_DIG_FINE_ADDR, 0x00);

        sc132gs_write_register (ViPipe, sc132gs_VMAX_ADDR, (u32FullLines_5Fps & 0xFF00) >> 8);
        sc132gs_write_register (ViPipe, sc132gs_VMAX_ADDR + 1, u32FullLines_5Fps & 0xFF);

        sc132gs_write_register (ViPipe, sc132gs_SHS1_ADDR, (u32MaxIntTime_5Fps >> 12) & 0x0F);
        sc132gs_write_register (ViPipe, sc132gs_SHS1_ADDR + 1,  (u32MaxIntTime_5Fps >> 4) & 0xFF);
        sc132gs_write_register (ViPipe, sc132gs_SHS1_ADDR + 2, (u32MaxIntTime_5Fps << 4) & 0xF0);

    }
    else /* setup for ISP 'normal mode' */
    {
        pstSnsState->u32FLStd = (pstSnsState->u32FLStd > 0xFFFF) ? 0xFFFF : pstSnsState->u32FLStd;
        sc132gs_write_register (ViPipe, sc132gs_VMAX_ADDR, (pstSnsState->u32FLStd & 0xFF00) >> 8);
        sc132gs_write_register (ViPipe, sc132gs_VMAX_ADDR + 1, pstSnsState->u32FLStd & 0xFF);
        pstSnsState->bSyncInit = AR_FALSE;
    }

    return;
}

static AR_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, AR_U8 u8Mode)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = AR_FALSE;

    switch (u8Mode)
    {
        case WDR_MODE_NONE:
            pstSnsState->enWDRMode = WDR_MODE_NONE;
            ISP_TRACE(AR_DBG_ERR, "linear mode\n");
            break;

        case WDR_MODE_2To1_FRAME:
            pstSnsState->enWDRMode = WDR_MODE_2To1_FRAME;
            ISP_TRACE(AR_DBG_ERR, "2to1 half-rate frame WDR mode\n");
            break;

        case WDR_MODE_2To1_LINE:
           pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
           ISP_TRACE(AR_DBG_ERR, "2to1 half-rate line WDR mode\n");
            break;

        case WDR_MODE_3To1_LINE:
            pstSnsState->enWDRMode = WDR_MODE_3To1_LINE;
            ISP_TRACE(AR_DBG_ERR, "3to1 line WDR 1080p mode(120fps->30fps)\n");
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
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if ((AR_FALSE == pstSnsState->bSyncInit) || (AR_FALSE == pstSnsRegsInfo->bConfig))
    {
        pstSnsState->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
        pstSnsState->astRegsInfo[0].unComBus.s8I2cDev = g_aunsc132gsBusInfo[ViPipe].s8I2cDev;
        pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        pstSnsState->astRegsInfo[0].u32RegNum = 9;
        printf("I2C Device %d\n", pstSnsState->astRegsInfo[0].unComBus.s8I2cDev);

        if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].u32RegNum += 3;
            pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        }

        else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].u32RegNum += 8;
            pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        }

        else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].u32RegNum += 14;
            pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        }


        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr = sc132gs_i2c_addr;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = sc132gs_addr_byte;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = sc132gs_data_byte;
        }
        
        // pstSnsState->astRegsInfo[0].astI2cData[0].bUpdate = AR_FALSE;
        // pstSnsState->astRegsInfo[0].astI2cData[0].u8DevAddr = sc132gs_i2c_addr;
        // pstSnsState->astRegsInfo[0].astI2cData[0].u32AddrByteNum = sc132gs_addr_byte;
        // pstSnsState->astRegsInfo[0].astI2cData[0].u32DataByteNum = sc132gs_data_byte;

        //Linear Mode Regs
        pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr = sc132gs_SHS1_ADDR;
        pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr = sc132gs_SHS1_ADDR + 1;
        pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr = sc132gs_SHS1_ADDR + 2;

        pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum = 2;       //make shutter and gain effective at the same time
        pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr = sc132gs_ANA_GAIN_ADDR;  //gain
        pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr = sc132gs_ANA_FINE_ADDR;
		pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = sc132gs_DIG_GAIN_ADDR;
		//pstSnsState->astRegsInfo[0].astI2cData[5].u32Data =
		pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 2;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = sc132gs_DIG_FINE_ADDR;

        pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = sc132gs_VMAX_ADDR;
        pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = sc132gs_VMAX_ADDR + 1;

        if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = sc132gs_SHS1_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = sc132gs_SHS1_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = sc132gs_SHS1_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = sc132gs_VMAX_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr = sc132gs_VMAX_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr = sc132gs_VMAX_ADDR + 2;

        }
        //DOL 2t1 Mode Regs
        else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 0;

            pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = sc132gs_SHS2_ADDR1;
            pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = sc132gs_SHS2_ADDR2;
            pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = sc132gs_SHS2_ADDR3;

            pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = sc132gs_VMAX_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr = sc132gs_VMAX_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr = sc132gs_VMAX_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[11].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[11].u32RegAddr = sc132gs_RHS1_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[12].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[12].u32RegAddr = sc132gs_RHS1_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[13].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[13].u32RegAddr = sc132gs_RHS1_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[14].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[14].u32RegAddr = sc132gs_Y_OUT_SIZE_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[15].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[15].u32RegAddr = sc132gs_Y_OUT_SIZE_ADDR + 1;

        }

        //DOL 3t1 Mode Regs
        else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 0;

            pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = sc132gs_SHS2_ADDR1;
            pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = sc132gs_SHS2_ADDR2;
            pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = sc132gs_SHS2_ADDR3;

            pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = sc132gs_VMAX_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr = sc132gs_VMAX_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr = sc132gs_VMAX_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[11].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[11].u32RegAddr = sc132gs_RHS1_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[12].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[12].u32RegAddr = sc132gs_RHS1_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[13].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[13].u32RegAddr = sc132gs_RHS1_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[14].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[14].u32RegAddr = sc132gs_RHS2_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[15].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[15].u32RegAddr = sc132gs_RHS2_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[16].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[16].u32RegAddr = sc132gs_RHS2_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[17].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[17].u32RegAddr = sc132gs_SHS3_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[18].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[18].u32RegAddr = sc132gs_SHS3_ADDR + 1;
            pstSnsState->astRegsInfo[0].astI2cData[19].u8DelayFrmNum = 0;
            pstSnsState->astRegsInfo[0].astI2cData[19].u32RegAddr = sc132gs_SHS3_ADDR + 2;

            pstSnsState->astRegsInfo[0].astI2cData[20].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[20].u32RegAddr = sc132gs_Y_OUT_SIZE_ADDR;
            pstSnsState->astRegsInfo[0].astI2cData[21].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[21].u32RegAddr = sc132gs_Y_OUT_SIZE_ADDR + 1;

        }

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
    if(pstSnsRegsInfo->bConfig) sc132gs_default_reg_init(ViPipe);
    return AR_SUCCESS;
}

static AR_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    AR_U8 u8SensorImageMode = 0;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = AR_FALSE;

    if (pstSensorImageMode->f32Fps <= 30)
    {
        if (WDR_MODE_NONE == pstSnsState->enWDRMode)
        {
            if (sc132gs_RES_IS_960x1280(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode = sc132gs_SENSOR_960P_30FPS_LINEAR_MODE;
                pstSnsState->u32FLStd = sc132gs_VMAX_960P30_LINEAR;
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

    sc132gs_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit = AR_FALSE;
    pstSnsState->bSyncInit = AR_FALSE;
    pstSnsState->u8ImgMode = sc132gs_SENSOR_960P_30FPS_LINEAR_MODE;
    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->u32FLStd = sc132gs_VMAX_960P30_LINEAR;
    pstSnsState->au32FL[0] = sc132gs_VMAX_960P30_LINEAR;
    pstSnsState->au32FL[1] = sc132gs_VMAX_960P30_LINEAR;

    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
}

static AR_S32 sc132gs_cmos_set_mirror_flip(VI_PIPE ViPipe, int mode)
{
    AR_S32 ret = 0;
    //ISP_TRACE(AR_DBG_ERR, "filp_mirror=%d\n", mode);
    switch (mode)
    {
        case ISP_SNS_NORMAL:
        {
            ret = sc132gs_flip_off_mirro_off(ViPipe);
            break;
        }
        case ISP_SNS_MIRROR:
        {
            ret = sc132gs_flip_off_mirro_on(ViPipe);
            break;
        }
        case ISP_SNS_FLIP:
        {
            ret = sc132gs_flip_on_mirro_off(ViPipe);
            break;
        }
        case ISP_SNS_MIRROR_FLIP:
        {
            ret = sc132gs_flip_on_mirro_on(ViPipe);
            break;
        }

        default:
        break;

    }

    return ret;

}

static AR_S32 sc132gs_cmos_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
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
            ret = sc132gs_cmos_set_mirror_flip(ViPipe, mode);
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

    pstSensorExpFunc->pfn_cmos_sensor_init = sc132gs_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = sc132gs_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;

    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;
    pstSensorExpFunc->pfn_cmos_sns_power_on = sc132gs_cmos_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = sc132gs_cmos_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = sc132gs_cmos_sensor_ctl;
    return AR_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static AR_S32 sc132gs_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_aunsc132gsBusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

    return AR_SUCCESS;
}

static AR_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    sc132gs_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

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

    sc132gs_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    sc132gs_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    sc132gs_SENSOR_RESET_CTX(ViPipe);
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

    stSnsAttrInfo.eSensorId = sc132gs_ID;

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

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, sc132gs_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, sc132gs_ID);
    if (AR_SUCCESS != s32Ret)
    {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, sc132gs_ID);
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

ISP_SNS_OBJ_S stSnssc132gsObj =
{
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = sc132gs_standby,
    .pfnRestart             = sc132gs_restart,
    .pfnMirrorFlip          = sc132gs_cmos_set_mirror_flip,
    .pfnWriteReg            = sc132gs_write_register,
    .pfnReadReg             = sc132gs_read_register,
    .pfnSetBusInfo          = sc132gs_set_bus_info,
    .pfnSetInit             = sensor_set_init
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __sc132gs_CMOS_H_ */
