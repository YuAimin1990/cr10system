#if !defined(__S5KGN2SP_CMOS_H_)
#define __S5KGN2SP_CMOS_H_

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


#define GN2_ID 2000

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastGN2[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define GN2_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastGN2[dev])
#define GN2_SENSOR_SET_CTX(dev, pstCtx)   (g_pastGN2[dev] = pstCtx)
#define GN2_SENSOR_RESET_CTX(dev)         (g_pastGN2[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_aunGN2BusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = { .s8I2cDev = 2},
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

extern const unsigned int gn2_i2c_addr;
extern unsigned int gn2_addr_byte;
extern unsigned int gn2_data_byte;

typedef struct arGN2_STATE_S
{
    AR_U8       u8Hcg;
    AR_U32      u32BRL;
    AR_U32      u32RHS1_MAX;
    AR_U32      u32RHS2_MAX;
} GN2_STATE_S;

GN2_STATE_S g_astGN2State[ISP_MAX_PIPE_NUM] = {{0}};

extern void gn2_init(VI_PIPE ViPipe);
extern void gn2_exit(VI_PIPE ViPipe);
extern void gn2_standby(VI_PIPE ViPipe);
extern void gn2_restart(VI_PIPE ViPipe);
extern int  gn2_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gn2_read_register(VI_PIPE ViPipe, int addr);
extern void gn2_default_reg_init(VI_PIPE ViPipe);
extern int gn2_flip_off_mirro_off(VI_PIPE ViPipe);
extern int gn2_flip_on_mirro_off(VI_PIPE ViPipe);
extern int gn2_flip_off_mirro_on(VI_PIPE ViPipe);
extern int gn2_flip_on_mirro_on(VI_PIPE ViPipe);
extern void gn2_cmos_stream_on(VI_PIPE ViPipe);
extern void gn2_cmos_stream_off(VI_PIPE ViPipe);
extern void gn2_trigger(VI_PIPE ViPipe, AR_S32 s32Frames);

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
#define GN2_SENSOR_4K_60FPS_LINEAR_10BIT_MODE		(1)
#define GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE		(2)
#define GN2_SENSOR_2K7_60FPS_LINEAR_10BIT_MODE		(3)
#define GN2_SENSOR_1080P_60FPS_LINEAR_10BIT_MODE	(4)

//analog gain
#define GN2_ANA_GAIN_MAX	64
#define GN2_A_GAIN_H	(0x0204)
#define GN2_A_GAIN_L	(0x0205)

//digital gain
#define GN2_DIG_GAIN_MAX	16
#define GN2_D_GAIN		(0x020d)
#define GN2_D_GAIN_GR_H	(0x020e)
#define GN2_D_GAIN_GR_L	(0x020f)
#define GN2_D_GAIN_R_H	(0x0210)
#define GN2_D_GAIN_R_L	(0x0211)
#define GN2_D_GAIN_B_H	(0x0212)
#define GN2_D_GAIN_B_L	(0x0213)
#define GN2_D_GAIN_GB_H	(0x0214)
#define GN2_D_GAIN_GB_L	(0x0215)

#define GN2_VMAX_ADDR_H	(0x0340) //VTS[15:8] total rows in on frame
#define GN2_VMAX_ADDR_L	(0x0341) //VTS[7:0]

#define GN2_EXP_TIME_M	(0x0202) //Bit[7:0] exposure_coarse[15:8] in unit of rows
#define GN2_EXP_TIME_L	(0x0203) //Bit[7:0] exposure_coarse[7:0] in unit of rows
//#define GN2_EXP_TIME_SHIFTER1	(0x0702)
//#define GN2_EXP_TIME_SHIFTER2	(0x0704)

#define GN2_VMAX_4K_FPS60_10BIT_LINEAR		(2210)
#define GN2_HMAX_4K_FPS30_10BIT_LINEAR  	(17049)
#define GN2_VMAX_4K_FPS30_10BIT_LINEAR		(3832) //3832
#define GN2_VMAX_2K7_FPS60_10BIT_LINEAR		(2658)
#define GN2_VMAX_1080P_FPS60_10BIT_LINEAR	(2539)

#define GN2_EXP_LINES_MAX_4K_FPS60			(2210 - 24)
#define GN2_EXP_LINES_MIN_4K_FPS60			(8)
#define GN2_EXP_LINES_MAX_4K_FPS30			(GN2_VMAX_4K_FPS30_10BIT_LINEAR - 24)
#define GN2_EXP_LINES_MIN_4K_FPS30			(8)
#define GN2_EXP_LINES_MAX_2K7_FPS60			(GN2_VMAX_2K7_FPS60_10BIT_LINEAR - 24)
#define GN2_EXP_LINES_MIN_2K7_FPS60			(8)
#define GN2_EXP_LINES_MAX_1080P_FPS60		(GN2_VMAX_1080P_FPS60_10BIT_LINEAR - 24)
#define GN2_EXP_LINES_MIN_1080P_FPS60		(8)

#define GN2_RES_IS_4K(w, h)		((w) == 3840 && (h) == 2160)
#define GN2_RES_IS_2K7(w, h)    ((w) == 2704 && (h) == 1520)
#define GN2_RES_IS_1080P(w, h)	((w) == 1920 && (h) == 1080)

/*
 pad fun group port num inout
.reset_gpio={38,2,1,2,1,1},
.power_gpio={39,2,1,2,0,1},
*/
#define CLK_SENSOR0		64
#define CLK_SENSOR1		65
#define CLK_SENSOR2		66
#define CLK_SENSOR3		67

static AR_S32 gn2_cmos_power_on(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
#if 1
	//int power_gpio = ar_hal_gpio_name_to_num(3,1,7); //for custom board
	//int reset_gpio = ar_hal_gpio_name_to_num(3,1,6);
	int power_gpio = ar_hal_gpio_name_to_num(1,2,1);
	int reset_gpio = ar_hal_gpio_name_to_num(1,2,0); //for 9341 evb
#else
    printf("s5k3p3_cmos_power_on  power_gpio=%d %d %d\n",p_dev_power_attr->power_gpio[0],p_dev_power_attr->power_gpio[1],p_dev_power_attr->power_gpio[2]);
    printf("s5k3p3_cmos_power_on  reset_gpio=%d %d %d\n",p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);
    int power_gpio=ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0],p_dev_power_attr->power_gpio[1],p_dev_power_attr->power_gpio[2]);
    int reset_gpio=ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);
#endif
	printf("gn2_cmos_power_on power_gpio=%d reset_gpio=%d\n",power_gpio,reset_gpio);
	if(p_dev_power_attr->mclk_k==SENSOR_MCLK_24M || p_dev_power_attr->mclk_k==SENSOR_MCLK_37_125M ||p_dev_power_attr->mclk_k==SENSOR_MCLK_27M || p_dev_power_attr->mclk_k==SENSOR_MCLK_74_25M)
	{
	    if(p_dev_power_attr->mclk_src>=CLK_SENSOR0 && p_dev_power_attr->mclk_src <=CLK_SENSOR3)
	    {
			printf("set the mclk mclk_index=%d mclk=%d \n",p_dev_power_attr->mclk_src,p_dev_power_attr->mclk_k);
			ar_hal_clk_set_rate(p_dev_power_attr->mclk_src,p_dev_power_attr->mclk_k);
			ar_hal_clk_enable_byid(p_dev_power_attr->mclk_src);
	    }
	}
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

static AR_S32 gn2_cmos_power_off(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
#if 1
	int power_gpio = ar_hal_gpio_name_to_num(3,1,7);
	int reset_gpio = ar_hal_gpio_name_to_num(3,1,6);
#else
    int power_gpio=ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0],p_dev_power_attr->power_gpio[1],p_dev_power_attr->power_gpio[2]);
    int reset_gpio=ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);
#endif
	ar_hal_gpio_export(power_gpio);
	ar_hal_gpio_export(reset_gpio);

	printf("gn2_cmos_power_off  power_gpio=%d reset_gpio=%d\n",power_gpio,reset_gpio);
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
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);
	AR_U32 exp_lines_min;

    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
	if(pstSnsState->u8ImgMode==GN2_SENSOR_1080P_60FPS_LINEAR_10BIT_MODE){
    	pstAeSnsDft->u32Hmax= 1920 + 200;
		pstAeSnsDft->f32Fps = 60;
		pstAeSnsDft->f32MaxFps = 60;
		pstAeSnsDft->u32FullLinesMax = GN2_EXP_LINES_MAX_1080P_FPS60;
		exp_lines_min = GN2_EXP_LINES_MIN_1080P_FPS60;
	}else if(pstSnsState->u8ImgMode==GN2_SENSOR_2K7_60FPS_LINEAR_10BIT_MODE){
		pstAeSnsDft->u32Hmax= 2705+200;
		pstAeSnsDft->f32Fps = 60;
		pstAeSnsDft->f32MaxFps = 60;
		pstAeSnsDft->u32FullLinesMax = GN2_EXP_LINES_MAX_2K7_FPS60;
		exp_lines_min = GN2_EXP_LINES_MIN_2K7_FPS60;
	}else if(pstSnsState->u8ImgMode==GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE){
		pstAeSnsDft->u32Hmax= 3840+200;
		pstAeSnsDft->f32Fps = 30;
		pstAeSnsDft->f32MaxFps = 30;
		pstAeSnsDft->u32FullLinesMax = GN2_EXP_LINES_MAX_4K_FPS30;
		exp_lines_min = GN2_EXP_LINES_MIN_4K_FPS30;
	}else if(pstSnsState->u8ImgMode==GN2_SENSOR_4K_60FPS_LINEAR_10BIT_MODE){
		pstAeSnsDft->u32Hmax= 3840+200;
		pstAeSnsDft->f32Fps = 60;
		pstAeSnsDft->f32MaxFps = 60;
		pstAeSnsDft->u32FullLinesMax = GN2_EXP_LINES_MAX_4K_FPS60;
		exp_lines_min = GN2_EXP_LINES_MIN_4K_FPS30;
	}else{

	}

    if (g_au32LinesPer500ms[ViPipe] == 0){
        pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * pstAeSnsDft->f32Fps / 2;
    }else{
        pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];
    }

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:   /*linear mode*/
            pstAeSnsDft->u32MaxAgain = 64 * 1024;
            pstAeSnsDft->u32MinAgain = 1024;

            pstAeSnsDft->u32MaxDgain = 16 * 1024;
            pstAeSnsDft->u32MinDgain = 1024;

            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd;
            pstAeSnsDft->u32MinIntTime = exp_lines_min;
            break;
        case WDR_MODE_2To1_LINE:
            break;
        case WDR_MODE_3To1_LINE:
            break;
    }

    return AR_SUCCESS;
}

/* the function of sensor set fps */
static AR_VOID cmos_fps_set(VI_PIPE ViPipe, AR_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    AR_U32 u32VMAX = GN2_VMAX_4K_FPS30_10BIT_LINEAR;
	AR_U32 exp_lines_max = u32VMAX;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);
	if(pstSnsState->u8ImgMode==GN2_SENSOR_1080P_60FPS_LINEAR_10BIT_MODE){
		u32VMAX = GN2_VMAX_1080P_FPS60_10BIT_LINEAR; //10bit
		exp_lines_max = GN2_EXP_LINES_MAX_1080P_FPS60;
	}else if(pstSnsState->u8ImgMode==GN2_SENSOR_2K7_60FPS_LINEAR_10BIT_MODE){
		u32VMAX = GN2_VMAX_2K7_FPS60_10BIT_LINEAR;
		exp_lines_max = GN2_EXP_LINES_MAX_2K7_FPS60;
	}else if(pstSnsState->u8ImgMode==GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE){
		u32VMAX = GN2_VMAX_4K_FPS30_10BIT_LINEAR;
		exp_lines_max = GN2_EXP_LINES_MAX_4K_FPS30;
	}else if(pstSnsState->u8ImgMode==GN2_SENSOR_4K_60FPS_LINEAR_10BIT_MODE){
		u32VMAX = GN2_VMAX_4K_FPS60_10BIT_LINEAR;
		exp_lines_max = GN2_EXP_LINES_MAX_4K_FPS60;
	}else{

	}

    if (WDR_MODE_NONE == pstSnsState->enWDRMode){
        pstSnsState->astRegsInfo[0].astI2cData[13].u32Data = (u32VMAX & 0xFF);
        pstSnsState->astRegsInfo[0].astI2cData[14].u32Data = ((u32VMAX & 0xFF00) >> 8);
    }else{
		
    }

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){
		
    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){
		
    }else{
        pstSnsState->u32FLStd = exp_lines_max;	//u32VMAX - 24;
    }

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];

    return;
}

static AR_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, AR_U32 u32FullLines,
                                       AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
#if 0
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){
		
    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){

    }else if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){
		
    }else{
        u32FullLines = (u32FullLines > GN2_FULL_LINES_MAX) ? GN2_FULL_LINES_MAX : u32FullLines;
        pstSnsState->au32FL[0] = u32FullLines;
    }

    if (WDR_MODE_NONE == pstSnsState->enWDRMode){
        pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = (pstSnsState->au32FL[0] & 0xFF);
        pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = ((pstSnsState->au32FL[0] & 0xFF00) >> 8);
    }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){

    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){

    }else if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){

    }else{
		
    }

    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - 8;
#endif
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static AR_VOID cmos_inttime_update(VI_PIPE ViPipe, AR_U32 u32IntTime)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    static AR_BOOL bFirst[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = 1};
    AR_U32 u32Value = 0;
	AR_U32 exp_lines_min;
	AR_U32 epx_lines_max;

    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

	if(pstSnsState->u8ImgMode==GN2_SENSOR_1080P_60FPS_LINEAR_10BIT_MODE){
		exp_lines_min = GN2_EXP_LINES_MIN_1080P_FPS60;
		epx_lines_max = GN2_EXP_LINES_MAX_1080P_FPS60;
    }else if(pstSnsState->u8ImgMode==GN2_SENSOR_2K7_60FPS_LINEAR_10BIT_MODE){
		exp_lines_min = GN2_EXP_LINES_MIN_2K7_FPS60;
		epx_lines_max = GN2_EXP_LINES_MAX_2K7_FPS60;
    }else if(pstSnsState->u8ImgMode==GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE){
		exp_lines_min = GN2_EXP_LINES_MIN_4K_FPS30;
		epx_lines_max = GN2_EXP_LINES_MAX_4K_FPS30;
    }else{
		
		exp_lines_min = GN2_EXP_LINES_MIN_4K_FPS60;
		epx_lines_max = GN2_EXP_LINES_MAX_4K_FPS60;
	}

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){

    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){

    }else if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){

    }else{
        u32Value = u32IntTime;
        if(u32Value < exp_lines_min)
            u32Value = exp_lines_min;
        if(u32Value > epx_lines_max)
            u32Value = epx_lines_max;

        pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = (u32Value & 0x00FF);
        pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = ((u32Value & 0xFF00) >> 8);

        bFirst[ViPipe] = AR_TRUE;
    }

    return;
}

static AR_VOID cmos_again_calc_table(VI_PIPE ViPipe, AR_U32 *pu32AgainLin, AR_U32 *pu32AgainDb)
{
    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

    *pu32AgainDb = *pu32AgainLin;

    return;
}

static AR_VOID cmos_dgain_calc_table(VI_PIPE ViPipe, AR_U32 *pu32DgainLin, AR_U32 *pu32DgainDb)
{
    CMOS_CHECK_POINTER_VOID(pu32DgainLin);
    CMOS_CHECK_POINTER_VOID(pu32DgainDb);

    *pu32DgainDb = *pu32DgainLin;

    return;
}

static AR_VOID cmos_again_transfer(AR_FLOAT *pfAgainLin, AR_U32 *pu32AgainDb)
{
#if 0
    int i;

    CMOS_CHECK_POINTER_VOID(pfAgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

    if(*pfAgainLin > 64)
       *pfAgainLin = 64; 

    if(*pfAgainLin < 1)
       *pfAgainLin = 1; 

    AR_FLOAT fagainIn = *pfAgainLin;  
    AR_U32 intAgain = floor(fagainIn);
    AR_U32 facAgain = floor((fagainIn - intAgain)*16);

    //if(fagainIn == 16) //0x100
    //    *pu32AgainDb = 0x100; 
    //else   
        *pu32AgainDb = (intAgain & 0x0f) << 4 | facAgain; //0x10-0xff
#endif
    return;
}

static AR_VOID cmos_dgain_transfer(AR_FLOAT *pfDgainLin, AR_U32 *pu32DgainDb)
{
#if 0
    int i;

    CMOS_CHECK_POINTER_VOID(pfDgainLin);
    CMOS_CHECK_POINTER_VOID(pu32DgainDb);

    if(*pfDgainLin > 16)  
        *pfDgainLin = 16;

    if(*pfDgainLin < 1)
        *pfDgainLin = 1;

    AR_FLOAT fdgainIn = *pfDgainLin;
    
    AR_U32 intDgain = floor(fdgainIn);
    AR_U32 facDgain = floor((fdgainIn-intDgain)*1024);

    *pu32DgainDb = intDgain << 10 | facDgain;

    if(*pu32DgainDb < 1024)  //0x400-0x3fff
        *pu32DgainDb = 1024;

    if(*pu32DgainDb > 16*1024) 
        *pu32DgainDb = 16*1024;
#endif
    return;
}

static AR_VOID cmos_gains_update(VI_PIPE ViPipe, AR_U32 u32Again, AR_U32 u32Dgain)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;
    static AR_BOOL bFirst[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = 1};

    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

	int again = 0;
	int dgain = 0;
	float tmp_again = 0;
 	float tmp_dgain = 0;

	if(u32Again >= (GN2_ANA_GAIN_MAX * 1024)){
		again = (GN2_ANA_GAIN_MAX * 1024);
	}else if(u32Again <= (1 * 1024)){
		again = (1 * 1024);
	}else{
		again = u32Again;
	}

	tmp_again = (again / 1024.0) *32;   
	again = tmp_again;
	if(u32Dgain >= (GN2_DIG_GAIN_MAX * 1024)){
		dgain = (GN2_DIG_GAIN_MAX * 1024);
	}else if(u32Dgain <= (1 * 1024)){
		dgain = (1 * 1024);
	}else{
		dgain = u32Dgain;
	}

	tmp_dgain = (dgain / 1024.0) * 256;
	dgain = tmp_again;
    if (WDR_MODE_NONE == pstSnsState->enWDRMode) {
		//analog gain
        pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = ((again & 0x00FF));		//GN2_A_GAIN_L
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = ((again & 0x0F00) >> 8);//GN2_A_GAIN_H

		//digital gain
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = (0x00);					//GN2_D_GAIN
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = ((dgain & 0x00FF));		//GN2_D_GAIN_GR_L
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = ((dgain & 0x0F00) >> 8);//GN2_D_GAIN_GR_H
		pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = ((dgain & 0x00FF));		//GN2_D_GAIN_R_L
        pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = ((dgain & 0x0F00) >> 8);//GN2_D_GAIN_R_H
        pstSnsState->astRegsInfo[0].astI2cData[9].u32Data = ((dgain & 0x00FF));		//GN2_D_GAIN_B_L
		pstSnsState->astRegsInfo[0].astI2cData[10].u32Data = ((dgain & 0x0F00) >> 8);//GN2_D_GAIN_B_H
        pstSnsState->astRegsInfo[0].astI2cData[11].u32Data = ((dgain & 0x00FF));	//GN2_D_GAIN_GB_L
        pstSnsState->astRegsInfo[0].astI2cData[12].u32Data = ((dgain & 0x0F00) >> 8);//GN2_D_GAIN_GB_H

        bFirst[ViPipe] = AR_TRUE;
    } else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) {

    }

    return;
}

static AR_VOID cmos_get_inttime_max(VI_PIPE ViPipe, AR_U16 u16ManRatioEnable, AR_U32 *au32Ratio, AR_U32 *au32IntTimeMax, AR_U32 *au32IntTimeMin, AR_U32 *pu32LFMaxIntTime)
{
    AR_U32 u32IntTimeMaxTmp  = 0;
    AR_U32 u32ShortTimeMinLimit = 0;

    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(au32Ratio);
    CMOS_CHECK_POINTER_VOID(au32IntTimeMax);
    CMOS_CHECK_POINTER_VOID(au32IntTimeMin);
    CMOS_CHECK_POINTER_VOID(pu32LFMaxIntTime);
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    u32ShortTimeMinLimit = (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode) ? 2 : ((WDR_MODE_3To1_LINE == pstSnsState->enWDRMode) ? 3 : 2);

    if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){
		
    }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){
		
    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){
		
    }else{
		
    }

    if (u32IntTimeMaxTmp >= u32ShortTimeMinLimit){

    }else{
        if (1 == u16ManRatioEnable){
            ISP_TRACE(AR_DBG_ERR, "Manaul ExpRatio is too large!\n");
            return;
        }else{
            u32IntTimeMaxTmp = u32ShortTimeMinLimit;

            if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){

            }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){

            }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){

            }else{
				
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
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
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
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
			strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_gn2/gn2_tuning_preview.bin");
			break;
        case WDR_MODE_2To1_LINE:
        case WDR_MODE_3To1_LINE:
			strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_gn2/gn2_tuning_preview_hdr.bin");
        	break;
    }

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){
		
    }

    pstDef->stSensorMode.u32SensorID = GN2_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    memcpy(&pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    switch (pstSnsState->u8ImgMode)
    {
    	case GN2_SENSOR_4K_60FPS_LINEAR_10BIT_MODE:
    	case GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE:
        case GN2_SENSOR_2K7_60FPS_LINEAR_10BIT_MODE:
		case GN2_SENSOR_1080P_60FPS_LINEAR_10BIT_MODE:
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 12;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 4095;
            break;
        default:
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
#if 0
    AR_U32 u32FullLines_5Fps, u32MaxIntTime_5Fps;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){
        return;
    }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){
        return;
    }else{
        if (GN2_SENSOR_4K_60FPS_LINEAR_10BIT_MODE == pstSnsState->u8ImgMode ||
			GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE == pstSnsState->u8ImgMode){
            u32FullLines_5Fps = (GN2_VMAX_4K_FPS30_10BIT_LINEAR * 30) / 5;
        }else if(GN2_SENSOR_27K_60FPS_LINEAR_10BIT_MODE == pstSnsState->u8ImgMode){
            u32FullLines_5Fps = (GN2_VMAX_2K7_FPS60_10BIT_LINEAR * 30) / 5;
		}else if(GN2_SENSOR_1080P_60FPS_LINEAR_10BIT_MODE == pstSnsState->u8ImgMode){
				u32FullLines_5Fps = (GN2_VMAX_1080P_FPS60_10BIT_LINEAR * 30) / 5;
        }else{
            return;
        }
    }

    //u32FullLines_5Fps = (u32FullLines_5Fps > OV13B10_FULL_LINES_MAX) ? OV13B10_FULL_LINES_MAX : u32FullLines_5Fps;
    u32MaxIntTime_5Fps = 4;

    if (bEnable){ /* setup for ISP pixel calibration mode */

    }else{ /* setup for ISP 'normal mode' */
    
    }
#endif
    return;
}

static AR_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, AR_U8 u8Mode)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = AR_FALSE;

    switch (u8Mode)
    {
        case WDR_MODE_NONE:
            pstSnsState->enWDRMode = WDR_MODE_NONE;
            ISP_TRACE(AR_DBG_INFO, "linear mode\n");
            break;
        case WDR_MODE_2To1_LINE:
            pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
            ISP_TRACE(AR_DBG_INFO, "2to1 half-rate line WDR mode\n");
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
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if ((AR_FALSE == pstSnsState->bSyncInit) || (AR_FALSE == pstSnsRegsInfo->bConfig)){
        pstSnsState->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
        pstSnsState->astRegsInfo[0].unComBus.s8I2cDev = g_aunGN2BusInfo[ViPipe].s8I2cDev;
        pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 0;
        pstSnsState->astRegsInfo[0].u32RegNum = 15;

        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr = gn2_i2c_addr;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = gn2_addr_byte;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = 2;//8bit
        }

        //Linear Mode Regs
        pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr = GN2_EXP_TIME_L;

        pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr = GN2_EXP_TIME_M;

        pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 0; 	//again
        pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr = GN2_A_GAIN_L; 

        pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum = 0;       
        pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr = GN2_A_GAIN_H;

        pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum = 0;	//dgain
        pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr = GN2_D_GAIN;

        pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = GN2_D_GAIN_GR_L;

        pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = GN2_D_GAIN_GR_H;

        pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = GN2_D_GAIN_R_L;

        pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = GN2_D_GAIN_R_H;

        pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr = GN2_D_GAIN_B_L;

        pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr = GN2_D_GAIN_B_H;

        pstSnsState->astRegsInfo[0].astI2cData[11].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[11].u32RegAddr = GN2_D_GAIN_GB_L;

        pstSnsState->astRegsInfo[0].astI2cData[12].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[12].u32RegAddr = GN2_D_GAIN_GB_H;

        pstSnsState->astRegsInfo[0].astI2cData[13].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[13].u32RegAddr = GN2_VMAX_ADDR_L;

        pstSnsState->astRegsInfo[0].astI2cData[14].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[14].u32RegAddr = GN2_VMAX_ADDR_H;
        if ( WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){

        }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){ //DOL 2t1 Mode Regs
 
        }else if (WDR_MODE_3To1_LINE == pstSnsState->enWDRMode){ //DOL 3t1 Mode Regs

        }

        pstSnsState->bSyncInit = AR_TRUE;

        ISP_TRACE(AR_DBG_ERR, "u32RegNum[%u]\n", pstSnsState->astRegsInfo[0].u32RegNum);
    }else{
        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            if (pstSnsState->astRegsInfo[0].astI2cData[i].u32Data == pstSnsState->astRegsInfo[1].astI2cData[i].u32Data){
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_FALSE;
            }else{
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_TRUE;
            }
        }

        if (WDR_MODE_2To1_FRAME == pstSnsState->enWDRMode){

        }
    }
	pstSnsState->astRegsInfo[0].bConfig = pstSnsRegsInfo->bConfig;

    memcpy(pstSnsRegsInfo, &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&pstSnsState->astRegsInfo[1], &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));

    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];

    //set real register for aec
    gn2_default_reg_init(ViPipe);
    return AR_SUCCESS;
}

static AR_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    AR_U8 u8SensorImageMode = 0;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = AR_FALSE;

    if (pstSensorImageMode->f32Fps <= 30){
        if (WDR_MODE_NONE == pstSnsState->enWDRMode){
			if (GN2_RES_IS_4K(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)){
                u8SensorImageMode = GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE;  //10bit
                ISP_TRACE(AR_DBG_ERR, "u8SensorImageMode:%d\n",u8SensorImageMode);
                pstSnsState->u32FLStd = GN2_EXP_LINES_MAX_4K_FPS30;
            }else{
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){

        }else{
            ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                      pstSensorImageMode->u16Width,
                      pstSensorImageMode->u16Height,
                      pstSensorImageMode->f32Fps,
                      pstSnsState->enWDRMode);
            return AR_FAILURE;
        }
    }else{	//60fps
        if (WDR_MODE_NONE == pstSnsState->enWDRMode){
			if (GN2_RES_IS_4K(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)){
                u8SensorImageMode = GN2_SENSOR_4K_60FPS_LINEAR_10BIT_MODE;  //10bit
                ISP_TRACE(AR_DBG_ERR, "u8SensorImageMode:%d\n",u8SensorImageMode);
                pstSnsState->u32FLStd = GN2_EXP_LINES_MAX_4K_FPS60;
            }else if(GN2_RES_IS_2K7(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height)){
                u8SensorImageMode = GN2_SENSOR_2K7_60FPS_LINEAR_10BIT_MODE;  //10bit
                ISP_TRACE(AR_DBG_ERR, "u8SensorImageMode:%d\n",u8SensorImageMode);
                pstSnsState->u32FLStd = GN2_EXP_LINES_MAX_2K7_FPS60;

			}else{
                ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                          pstSensorImageMode->u16Width,
                          pstSensorImageMode->u16Height,
                          pstSensorImageMode->f32Fps,
                          pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }else if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode){

        }else{
            ISP_TRACE(AR_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                      pstSensorImageMode->u16Width,
                      pstSensorImageMode->u16Height,
                      pstSensorImageMode->f32Fps,
                      pstSnsState->enWDRMode);
            return AR_FAILURE;
        }    
    }

    if ((AR_TRUE == pstSnsState->bInit) && (u8SensorImageMode == pstSnsState->u8ImgMode)){
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

    GN2_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit 		= AR_FALSE;
    pstSnsState->bSyncInit	= AR_FALSE;
    pstSnsState->u8ImgMode	= GN2_SENSOR_4K_30FPS_LINEAR_10BIT_MODE;
    pstSnsState->enWDRMode	= WDR_MODE_NONE;
    pstSnsState->u32FLStd	= GN2_EXP_LINES_MAX_4K_FPS30;
    pstSnsState->au32FL[0]	= GN2_EXP_LINES_MAX_4K_FPS30;
    pstSnsState->au32FL[1]	= GN2_EXP_LINES_MAX_4K_FPS30;

    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
}

static AR_S32 gn2_cmos_set_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
    AR_S32 ret = 0;
    //ISP_TRACE(AR_DBG_ERR, "filp_mirror=%d\n", mode);
    switch (eSnsMirrorFlip)
    {
        case ISP_SNS_NORMAL:
            ret = gn2_flip_off_mirro_off(ViPipe);
            break;
        case ISP_SNS_MIRROR:
            ret = gn2_flip_off_mirro_on(ViPipe);
            break;
        case ISP_SNS_FLIP:
            ret = gn2_flip_on_mirro_off(ViPipe);
            break;
        case ISP_SNS_MIRROR_FLIP:
            ret = gn2_flip_on_mirro_on(ViPipe);
            break;
        default:
        	break;
    }

    return ret;
}

static AR_S32 gn2_cmos_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
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
            ret = gn2_cmos_set_mirror_flip(ViPipe, mode);
            break;
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

    pstSensorExpFunc->pfn_cmos_sensor_init = gn2_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = gn2_cmos_stream_off;
	//pstSensorExpFunc->pfn_cmos_sns_trigger = gn2_trigger;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;

    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;
    pstSensorExpFunc->pfn_cmos_sns_power_on = gn2_cmos_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = gn2_cmos_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = gn2_cmos_sensor_ctl;
	//pstSensorExpFunc->pfn_cmos_sns_stream_on = gn2_cmos_stream_on;
	
    return AR_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static AR_S32 gn2_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_aunGN2BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

    return AR_SUCCESS;
}

static AR_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    GN2_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (AR_NULL == pastSnsStateCtx){
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (AR_NULL == pastSnsStateCtx){
            ISP_TRACE(AR_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));
    GN2_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    GN2_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    GN2_SENSOR_RESET_CTX(ViPipe);
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

    if (AR_SUCCESS != s32Ret){
        return AR_FAILURE;
    }

    stSnsAttrInfo.eSensorId = GN2_ID;

    s32Ret  = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);

    if (AR_SUCCESS != s32Ret){
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);

    if (AR_SUCCESS != s32Ret){
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);

    if (AR_SUCCESS != s32Ret){
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

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, GN2_ID);
    if (AR_SUCCESS != s32Ret){
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, GN2_ID);
    if (AR_SUCCESS != s32Ret){
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, GN2_ID);
    if (AR_SUCCESS != s32Ret){
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

ISP_SNS_OBJ_S stSnsGN2Obj =
{
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = gn2_standby,
    .pfnRestart             = gn2_restart,
    .pfnMirrorFlip          = gn2_cmos_set_mirror_flip,
    .pfnWriteReg            = gn2_write_register,
    .pfnReadReg             = gn2_read_register,
    .pfnSetBusInfo          = gn2_set_bus_info,
    .pfnSetInit             = sensor_set_init
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __S5KGN2SP_CMOS_H_ */
