
#if !defined(__GC2093_CMOS_C_)
#define __GC2093_CMOS_C_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "hal_gpio.h"
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define GC2093_ID 2093

typedef struct {
    AR_U32 u32RHS1_MAX;
    AR_U32 u32BRL;
} SENSOR_STATE_S;


/****************************************************************************
 * global variables                                                         *
 ****************************************************************************/
ISP_SNS_STATE_S *g_pastGc2093[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_pastGc2093[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (g_pastGc2093[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev)       (g_pastGc2093[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_aunGc2093BusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = { .s8I2cDev = 1},
    [1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

static ISP_FSWDR_MODE_E genFSWDRMode[ISP_MAX_PIPE_NUM] =
{
    [0 ... ISP_MAX_PIPE_NUM - 1] = ISP_FSWDR_NORMAL_MODE
};

static AR_U32 gu32MaxTimeGetCnt[ISP_MAX_PIPE_NUM]   = {0};
static AR_U32 g_au32InitExposure[ISP_MAX_PIPE_NUM]  = {0};
static AR_U32 g_au32LinesPer500ms[ISP_MAX_PIPE_NUM] = {0};
static AR_U16 g_au16InitWBGain[ISP_MAX_PIPE_NUM][3] = {{0}};
static AR_U16 g_au16SampleRgain[ISP_MAX_PIPE_NUM]   = {0};
static AR_U16 g_au16SampleBgain[ISP_MAX_PIPE_NUM]   = {0};

static SENSOR_STATE_S g_astGc2093State[ISP_MAX_PIPE_NUM] = {{0}};



/****************************************************************************
 * extern                                                                   *
 ****************************************************************************/
extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

extern void sensor_init(VI_PIPE ViPipe);
extern void sensor_exit(VI_PIPE ViPipe);
extern void sensor_standby(VI_PIPE ViPipe);
extern void sensor_restart(VI_PIPE ViPipe);
extern void sensor_default_reg_init(VI_PIPE ViPipe);
extern int  sensor_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sensor_read_register(VI_PIPE ViPipe, int addr);
extern int  sensor_flip_off_mirror_off(VI_PIPE ViPipe);
extern int  sensor_flip_on_mirror_off(VI_PIPE ViPipe);
extern int  sensor_flip_off_mirror_on(VI_PIPE ViPipe);
extern int  sensor_flip_on_mirror_on(VI_PIPE ViPipe);



/****************************************************************************
 * local variables                                                          *
 ****************************************************************************/
/* Sensor register address */
#define GC2093_SHORT_EXPTIME_ADDR_H (0x0001) // Exposure_Short[13:8]
#define GC2093_SHORT_EXPTIME_ADDR_L (0x0002) // Exposure_Short[7:0]
#define GC2093_EXPTIME_ADDR_H       (0x0003) // Exposure[13:8]
#define GC2093_EXPTIME_ADDR_L       (0x0004) // Exposure[7:0]
#define GC2093_AGAIN_ADDR_H         (0x00b4) // Analog_PGA_gain[9:8]
#define GC2093_AGAIN_ADDR_L         (0x00b3) // Analog_PGA_gain[7:0]
#define GC2093_DGAIN_ADDR_H         (0x00b8) // Col_gain[11:6]
#define GC2093_DGAIN_ADDR_L         (0x00b9) // Col_gain[5:0]
#define GC2093_GAIN_ADDR_0X155      (0x0155)
#define GC2093_GAIN_ADDR_0X31D      (0x031d)
#define GC2093_GAIN_ADDR_0XC2       (0x00c2)
#define GC2093_GAIN_ADDR_0XCF       (0x00cf)
#define GC2093_GAIN_ADDR_0XD9       (0x00d9)

#define GC2093_AUTO_PREGAIN_ADDR_H  (0x00b1) // auto-pregain-sync[9:6]
#define GC2093_AUTO_PREGAIN_ADDR_L  (0x00b2) // auto-pregain[5:0]
#define GC2093_VMAX_ADDR_H          (0x0041) // Vmax[13:8]
#define GC2093_VMAX_ADDR_L          (0x0042) // Vmax[7:0]


#define SENSOR_FULL_LINES_MAX          (0x3FFF)
#define SENSOR_FULL_LINES_MAX_2TO1_WDR (0x966)  /* considering the YOUT_SIZE and bad frame */

#define SENSOR_INCREASE_LINES       (0)
#define SENSOR_VMAX_1080P30_LINEAR  (2500 + SENSOR_INCREASE_LINES)
#define SENSOR_VMAX_1080P60TO30_WDR (1219 + SENSOR_INCREASE_LINES)

#define SENSOR_1080P_30FPS_LINEAR_MODE (0)
#define SENSOR_1080P_30FPS_2T1_WDR_MODE (1)

#define SENSOR_RES_IS_1080P(w, h) ((w) <= 1920 && (h) <= 1080)
#define SENSOR_RES_IS_WDR(w, h)   ((w) <= 1952 && (h) <= 2678)

#define HIGH_8BITS(x)             (((x) & 0xff00) >> 8)
#define LOW_8BITS(x)              ((x) & 0x00ff)


//#define AR9311_SOCKET
static AR_S32 cmos_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{

#if defined(PROXIMA)
	
	#if defined(AR9311_SOCKET)
		 AR_U32 reset_val = 0, pow_val = 0, val = 0;
	   AR_MPI_ISP_GetRegister(ViPipe, 0xA10A108, &val);
	   pow_val=val;
	   pow_val = pow_val & (~(1<<8));   
	   pow_val = pow_val & (~(1<<9));
	   AR_MPI_ISP_SetRegister(ViPipe, 0xA10A108, pow_val);


	   AR_MPI_ISP_GetRegister(ViPipe, 0xA10A104, &val);
	   pow_val=val;
	   pow_val = pow_val & (~(1<<8));  
	   pow_val = pow_val & (~(1<<9));
	   AR_MPI_ISP_SetRegister(ViPipe, 0xA10A104, pow_val);

//	   usleep(1000);
	   AR_MPI_ISP_GetRegister(ViPipe, 0xA10A104, &val);
	   pow_val=val;
	   pow_val = pow_val | ((1<<8));   
	   pow_val = pow_val | ((1<<9));
	   AR_MPI_ISP_SetRegister(ViPipe, 0xA10A104, pow_val);

   		return 0;
	#else
	   AR_U32 reset_val = 0, pow_val = 0, val = 0;
	   AR_MPI_ISP_GetRegister(ViPipe, 0xA10A108, &val);
	   pow_val=val;
	   pow_val = pow_val & (~(1<<3));   
	   pow_val = pow_val & (~(1<<15));
	   AR_MPI_ISP_SetRegister(ViPipe, 0xA10A108, pow_val);


	   AR_MPI_ISP_GetRegister(ViPipe, 0xA10A104, &val);
	   pow_val=val;
	   pow_val = pow_val & (~(1<<3));  
	   pow_val = pow_val & (~(1<<15));
	   AR_MPI_ISP_SetRegister(ViPipe, 0xA10A104, pow_val);

//	   usleep(1000);
	   AR_MPI_ISP_GetRegister(ViPipe, 0xA10A104, &val);
	   pow_val=val;
	   pow_val = pow_val | ((1<<3));   
	   pow_val = pow_val | ((1<<15));
	   AR_MPI_ISP_SetRegister(ViPipe, 0xA10A104, pow_val);

	   return 0;
			
	#endif	   


#else 

    int power_gpio = 0;
    int reset_gpio = 0;

    power_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0],
        p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],
        p_dev_power_attr->reset_gpio[1], p_dev_power_attr->reset_gpio[2]);

    ar_always("cmos_power_on: power_gpio=%d, reset_gpio=%d\n", power_gpio, reset_gpio);
    ar_hal_gpio_export(power_gpio);
    ar_hal_gpio_export(reset_gpio);

    /* set the gpio dir to out */
    ar_hal_gpio_set_dir(power_gpio, 1);
    ar_hal_gpio_set_dir(reset_gpio, 1);

    /* set the gpio to low */
    ar_hal_gpio_set_value(power_gpio, 0);
    ar_hal_gpio_set_value(reset_gpio, 0);

    /* give some delay, set the gpio to low */
    //usleep(10000);
    ar_hal_gpio_set_value(power_gpio, 1);
    //usleep(10000);
    ar_hal_gpio_set_value(reset_gpio, 1);
    //usleep(10000);
#endif

    return AR_SUCCESS;
}

static AR_S32 cmos_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    int power_gpio = 0;
    int reset_gpio = 0;

    power_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0],
        p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],
        p_dev_power_attr->reset_gpio[1], p_dev_power_attr->reset_gpio[2]);

    ar_always("cmos_power_off: power_gpio=%d, reset_gpio=%d\n", power_gpio, reset_gpio);
    ar_hal_gpio_export(power_gpio);
    ar_hal_gpio_export(reset_gpio);

    /* set the gpio dir to out */
    ar_hal_gpio_set_dir(power_gpio, 1);
    ar_hal_gpio_set_dir(reset_gpio, 1);

    /* set the gpio to low */
    ar_hal_gpio_set_value(power_gpio, 0);
    ar_hal_gpio_set_value(reset_gpio, 0);

    return AR_SUCCESS;
}

static AR_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstAeSnsDft->f32Fps          = 30;
    pstAeSnsDft->f32MaxFps       = 30;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq  = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = SENSOR_FULL_LINES_MAX;
    pstAeSnsDft->u32Hmax         = 2640;    //2200;

    if(g_au32LinesPer500ms[ViPipe] == 0)
    {
        pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * pstAeSnsDft->f32Fps / 2;
    }
    else
    {
        pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];
    }

    switch(pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
        {
            /* Linear mode */
            pstAeSnsDft->u32MaxAgain   = 74976;
            pstAeSnsDft->u32MinAgain   = 1024;
            pstAeSnsDft->u32MaxDgain   = 74976;
            pstAeSnsDft->u32MinDgain   = 1024;
            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            pstAeSnsDft->u32MinIntTime = 3;
            break;
        }

        case WDR_MODE_2To1_LINE:
        {
            /* Wdr mode */
            pstAeSnsDft->u32MaxAgain   = 74976;
            pstAeSnsDft->u32MinAgain   = 1024;
            pstAeSnsDft->u32MaxDgain   = 74976;
            pstAeSnsDft->u32MinDgain   = 1024;
            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 2;
            pstAeSnsDft->u32MinIntTime = 2;

            pstAeSnsDft->u32FullLinesShort = 461;
            pstAeSnsDft->u32LFMaxShortTime = 461;
            pstAeSnsDft->u32LFMinExposure  = 2;
            break;
        }
    }

    return AR_SUCCESS;
}

static AR_VOID cmos_fps_set(VI_PIPE ViPipe, AR_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    AR_U32 u32VMAX = SENSOR_VMAX_1080P30_LINEAR;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    switch(pstSnsState->u8ImgMode)
    {
        case SENSOR_1080P_30FPS_2T1_WDR_MODE:
        {
            if((f32Fps <= pstAeSnsDft->f32MaxFps) && (f32Fps >= 15.22))
            {
                u32VMAX = SENSOR_VMAX_1080P30_LINEAR * pstAeSnsDft->f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
            }
            else
            {
                ar_always("Not support Fps: %f\n", f32Fps);
                return;
            }

            u32VMAX = (u32VMAX > SENSOR_FULL_LINES_MAX_2TO1_WDR) ? SENSOR_FULL_LINES_MAX_2TO1_WDR : u32VMAX;
            break;
        }

        case SENSOR_1080P_30FPS_LINEAR_MODE:
        {
            if((f32Fps <= pstAeSnsDft->f32MaxFps) && (f32Fps >= 2.06))
            {
                u32VMAX = SENSOR_VMAX_1080P30_LINEAR * pstAeSnsDft->f32MaxFps / DIV_0_TO_1_FLOAT(f32Fps);
            }
            else
            {
                ar_always("Not support Fps: %f\n", f32Fps);
                return;
            }

            u32VMAX = (u32VMAX > SENSOR_FULL_LINES_MAX) ? SENSOR_FULL_LINES_MAX : u32VMAX;
            break;
        }

        default:
        {
            return;
        }
    }


    if(WDR_MODE_NONE == pstSnsState->enWDRMode)
    {
        pstSnsState->astRegsInfo[0].astI2cData[13].u32Data = LOW_8BITS(u32VMAX);
        pstSnsState->astRegsInfo[0].astI2cData[14].u32Data = HIGH_8BITS(u32VMAX);
    }
    else
    {
        AR_U32 u32Temp = u32VMAX / 2;
        pstSnsState->astRegsInfo[0].astI2cData[13].u32Data = LOW_8BITS(u32Temp);
        pstSnsState->astRegsInfo[0].astI2cData[14].u32Data = HIGH_8BITS(u32Temp);
    }

    if(WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        pstSnsState->u32FLStd   = u32VMAX;  //u32VMAX * 2 + 80;
        pstSnsState->u32FLShort = pstAeSnsDft->u32FullLinesShort;
        //ar_always("gu32FullLinesStd:%d\n",pstSnsState->u32FLStd);

        g_astGc2093State[ViPipe].u32RHS1_MAX = (u32VMAX - g_astGc2093State[ViPipe].u32BRL) * 2 - 21;
    }
    else
    {
        pstSnsState->u32FLStd = u32VMAX;

   }


    pstAeSnsDft->f32Fps           = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd  = pstSnsState->u32FLStd;
    pstAeSnsDft->u32MaxIntTime    = pstSnsState->u32FLStd - 2;
    pstSnsState->au32FL[0]        = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines     = pstSnsState->au32FL[0];

    return;
}

static AR_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, AR_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	return; //need to do
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    u32FullLines = (u32FullLines > SENSOR_FULL_LINES_MAX) ? SENSOR_FULL_LINES_MAX : u32FullLines;
    pstSnsState->au32FL[0] = u32FullLines;

    pstSnsState->astRegsInfo[0].astI2cData[13].u32Data = LOW_8BITS(pstSnsState->au32FL[0]);
    pstSnsState->astRegsInfo[0].astI2cData[14].u32Data = HIGH_8BITS(pstSnsState->au32FL[0]);

    pstAeSnsDft->u32FullLines  = pstSnsState->au32FL[0];
    pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - 2;

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static AR_VOID cmos_inttime_update(VI_PIPE ViPipe, AR_U32 u32IntTime)
{
    static AR_BOOL bFirst[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = AR_TRUE};

    static AR_U32 u32ShortIntTime[ISP_MAX_PIPE_NUM]  = {0};
    static AR_U32 u32LongIntTime[ISP_MAX_PIPE_NUM]   = {0};

    static AR_S32 s32SHS1[ISP_MAX_PIPE_NUM]  = {0};
    static AR_S32 s32SHS2[ISP_MAX_PIPE_NUM]  = {0};

    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    AR_S32 u32Value = 0;

    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    u32Value = (u32IntTime > SENSOR_FULL_LINES_MAX) ? SENSOR_FULL_LINES_MAX : u32IntTime;
    if(WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
#if 0
        u32IntTime = u32IntTime * 2;
        if(bFirst[ViPipe])
        {
            /* short exposure */
            pstSnsState->au32WDRIntTime[0] = u32IntTime;
            u32ShortIntTime[ViPipe]        = u32IntTime;
            s32SHS1[ViPipe]                = pstSnsState->u32FLShort - u32ShortIntTime[ViPipe];
            //ar_always("sns[%d]: shtInt=%d, FLS=%d, SHS1=%d\n", ViPipe, u32ShortIntTime[ViPipe], pstSnsState->u32FLShort, s32SHS1[ViPipe]);
            if(s32SHS1[ViPipe] <= 2)
            {
                s32SHS1[ViPipe] = 2;
            }
            else if(s32SHS1[ViPipe] >= pstSnsState->u32FLShort - 2)
            {
                s32SHS1[ViPipe] = pstSnsState->u32FLShort - 2;
            }

            bFirst[ViPipe] = AR_FALSE;
        }
        else
        {
            /* long exposure */
            pstSnsState->au32WDRIntTime[1] = u32IntTime;
            u32LongIntTime[ViPipe]         = u32IntTime;
            s32SHS2[ViPipe]                = pstSnsState->au32FL[1] - u32LongIntTime[ViPipe];
            //ar_always("sns[%d]: longInt=%d, FL1=%d, SHS2=%d\n", ViPipe, u32LongIntTime[ViPipe], pstSnsState->au32FL[1], s32SHS2[ViPipe]);
            if(s32SHS2[ViPipe] <= pstSnsState->u32FLShort + 2)
            {
                /* This is not same as datasheet with >= RSH1 + 2 */
                s32SHS2[ViPipe] = pstSnsState->u32FLShort + 2;
            }
            else if(s32SHS2[ViPipe] >= pstSnsState->au32FL[1] - 2)
            {
                s32SHS2[ViPipe] =  pstSnsState->au32FL[1] - 2;
            }

            pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = LOW_8BITS(s32SHS2[ViPipe]);
            pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = HIGH_8BITS(s32SHS2[ViPipe]);

            pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = LOW_8BITS(s32SHS1[ViPipe]);
            pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = HIGH_8BITS(s32SHS1[ViPipe]);

            bFirst[ViPipe] = AR_TRUE;
        }
#else
        if (bFirst[ViPipe]) {
            u32ShortIntTime[ViPipe] = u32Value;
            bFirst[ViPipe] = AR_FALSE;
        } else {
            u32LongIntTime[ViPipe] = u32Value;
            pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = LOW_8BITS(u32LongIntTime[ViPipe]);
            pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = HIGH_8BITS(u32LongIntTime[ViPipe]);
            pstSnsState->astRegsInfo[0].astI2cData[15].u32Data = LOW_8BITS(u32ShortIntTime[ViPipe]);
            pstSnsState->astRegsInfo[0].astI2cData[16].u32Data = HIGH_8BITS(u32ShortIntTime[ViPipe]);
            bFirst[ViPipe] = AR_TRUE;
        }
#endif
    }
    else
    {
        //ar_always("sns[%d]: ln=%d, u32Value=%u\n", ViPipe, u32IntTime, u32Value);
        pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = LOW_8BITS(u32Value);
        pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = HIGH_8BITS(u32Value);
    }

    return;
}

static AR_U32 regValTable[25][9] =
{
 /* 0xb3  0xb8  0xb9 0x155 0x031d 0xc2 0xcf 0xd9 0x031d */
    {0x00, 0x01, 0x00, 0x08, 0x2d, 0x10, 0x08, 0x0a, 0x28},
    {0x10, 0x01, 0x0c, 0x08, 0x2d, 0x10, 0x08, 0x0a, 0x28},
    {0x20, 0x01, 0x1b, 0x08, 0x2d, 0x11, 0x08, 0x0c, 0x28},
    {0x30, 0x01, 0x2c, 0x08, 0x2d, 0x12, 0x08, 0x0e, 0x28},
    {0x40, 0x01, 0x3f, 0x08, 0x2d, 0x14, 0x08, 0x12, 0x28},
    {0x50, 0x02, 0x16, 0x08, 0x2d, 0x15, 0x08, 0x14, 0x28},
    {0x60, 0x02, 0x35, 0x08, 0x2d, 0x17, 0x08, 0x18, 0x28},
    {0x70, 0x03, 0x16, 0x08, 0x2d, 0x18, 0x08, 0x1a, 0x28},
    {0x80, 0x04, 0x02, 0x08, 0x2d, 0x1a, 0x08, 0x1e, 0x28},
    {0x90, 0x04, 0x31, 0x08, 0x2d, 0x1b, 0x08, 0x20, 0x28},
    {0xa0, 0x05, 0x32, 0x08, 0x2d, 0x1d, 0x08, 0x24, 0x28},
    {0xb0, 0x06, 0x35, 0x08, 0x2d, 0x1e, 0x08, 0x26, 0x28},
    {0xc0, 0x08, 0x04, 0x08, 0x2d, 0x20, 0x08, 0x2a, 0x28},
    {0x5a, 0x09, 0x19, 0x08, 0x2d, 0x1e, 0x08, 0x2a, 0x28},
    {0x83, 0x0b, 0x0f, 0x08, 0x2d, 0x1f, 0x08, 0x2a, 0x28},
    {0x93, 0x0d, 0x12, 0x08, 0x2d, 0x21, 0x08, 0x2e, 0x28},
    {0x84, 0x10, 0x00, 0x0b, 0x2d, 0x22, 0x08, 0x30, 0x28},
    {0x94, 0x12, 0x3a, 0x0b, 0x2d, 0x24, 0x08, 0x34, 0x28},
    {0x5d, 0x1a, 0x02, 0x0b, 0x2d, 0x26, 0x08, 0x34, 0x28},
    {0x9b, 0x1b, 0x20, 0x0b, 0x2d, 0x26, 0x08, 0x34, 0x28},
    {0x8c, 0x20, 0x0f, 0x0b, 0x2d, 0x26, 0x08, 0x34, 0x28},
    {0x9c, 0x26, 0x07, 0x12, 0x2d, 0x26, 0x08, 0x34, 0x28},
    {0xB6, 0x36, 0x21, 0x12, 0x2d, 0x26, 0x08, 0x34, 0x28},
    {0xad, 0x37, 0x3a, 0x12, 0x2d, 0x26, 0x08, 0x34, 0x28},
    {0xbd, 0x3d, 0x02, 0x12, 0x2d, 0x26, 0x08, 0x34, 0x28}
};

static AR_U32 analog_gain_table[25] =
{
    1024, 1216, 1456, 1712, 2000, 2352, 2832, 3376, 3968, 4752,
    5696, 6800, 8064, 9584, 11344, 13376, 15648, 18448, 26352, 26416,
    30960, 36672, 51824, 63344, 74976
};

static AR_VOID cmos_again_calc_table(VI_PIPE ViPipe, AR_U32 *pu32AgainLin, AR_U32 *pu32AgainDb)
{
    AR_S32 again = 0;
    AR_S32 i     = 0;
    AR_U32 digital_gain = 0;

    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);


    //ar_always("sns pp=%d, agLin=%d, agDb=%d\n", ViPipe, *pu32AgainLin, *pu32AgainDb);
    again = *pu32AgainLin;
    if(again >= analog_gain_table[24])
    {
        //*pu32AgainLin = analog_gain_table[24];
        *pu32AgainDb  = 24;
    }
    else
    {
        for(i = 1; i < 25; i++)
        {
            if(again < analog_gain_table[i])
            {
                //*pu32AgainLin = analog_gain_table[i - 1];
                *pu32AgainDb  = i - 1;
                break;
            }
        }
    }

    digital_gain = (*pu32AgainLin) * 64 / analog_gain_table[*pu32AgainDb];
    *pu32AgainLin = analog_gain_table[*pu32AgainDb] * digital_gain / 64;
    *pu32AgainDb |= (digital_gain << 16);

    return;
}

static AR_VOID cmos_gains_update(VI_PIPE ViPipe, AR_U32 u32Again, AR_U32 u32Dgain)
{
    static AR_BOOL bFirst[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = 1};

    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    //AR_U8 u8DgainHigh = 1;
    //AR_U8 u8DgainLow  = 0;
    AR_U32 u32SnsDgain = 0;

    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    //{
    //    ISP_TRACE(AR_DBG_ERR, "ViPipe[%d] u32Again[%u] u32Dgain[%u]\n", ViPipe, u32Again, u32Dgain);
    //}

    u32SnsDgain = ((u32Again >> 16) & 0xffff);
    u32Again &= 0xffff;

    //ar_always("sns pp=%d, ag=%d, dg=%d\n", ViPipe, u32Again, u32Dgain);
    if(pstSnsState->enWDRMode == WDR_MODE_2To1_LINE)
    {
        if(bFirst[ViPipe])
        {
            /* short gain */
            bFirst[ViPipe] = AR_FALSE;
            //pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = u8DgainHigh;
            //pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = u8DgainLow;
        }
        else
        {
            /* long gain */
            bFirst[ViPipe] = AR_TRUE;
            //pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = u8DgainHigh;
            //pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = u8DgainLow;
            pstSnsState->astRegsInfo[0].astI2cData[2].u32Data  = regValTable[u32Again][0];
            pstSnsState->astRegsInfo[0].astI2cData[3].u32Data  = regValTable[u32Again][1];
            pstSnsState->astRegsInfo[0].astI2cData[4].u32Data  = regValTable[u32Again][2];
            pstSnsState->astRegsInfo[0].astI2cData[5].u32Data  = regValTable[u32Again][3];
            pstSnsState->astRegsInfo[0].astI2cData[6].u32Data  = regValTable[u32Again][4];
            pstSnsState->astRegsInfo[0].astI2cData[7].u32Data  = regValTable[u32Again][5];
            pstSnsState->astRegsInfo[0].astI2cData[8].u32Data  = regValTable[u32Again][6];
            pstSnsState->astRegsInfo[0].astI2cData[9].u32Data  = regValTable[u32Again][7];
            pstSnsState->astRegsInfo[0].astI2cData[10].u32Data = regValTable[u32Again][8];
            pstSnsState->astRegsInfo[0].astI2cData[11].u32Data = ((u32SnsDgain & 0x3f) << 2);
            pstSnsState->astRegsInfo[0].astI2cData[12].u32Data = (u32SnsDgain >> 6);
        }
    }
    else if(pstSnsState->enWDRMode == WDR_MODE_NONE)
    {
        //u8DgainHigh = (u32Dgain >> 6) & 0x0f;
        //u8DgainLow  = (u32Dgain & 0x3f) << 2;
        pstSnsState->astRegsInfo[0].astI2cData[2].u32Data  = regValTable[u32Again][0];
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data  = regValTable[u32Again][1];
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data  = regValTable[u32Again][2];
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data  = regValTable[u32Again][3];
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data  = regValTable[u32Again][4];
        pstSnsState->astRegsInfo[0].astI2cData[7].u32Data  = regValTable[u32Again][5];
        pstSnsState->astRegsInfo[0].astI2cData[8].u32Data  = regValTable[u32Again][6];
        pstSnsState->astRegsInfo[0].astI2cData[9].u32Data  = regValTable[u32Again][7];
        pstSnsState->astRegsInfo[0].astI2cData[10].u32Data = regValTable[u32Again][8];
        pstSnsState->astRegsInfo[0].astI2cData[11].u32Data = ((u32SnsDgain & 0x3f) << 2);
        pstSnsState->astRegsInfo[0].astI2cData[12].u32Data = (u32SnsDgain >> 6);
    }

    return;
}

static AR_VOID cmos_get_inttime_max(VI_PIPE ViPipe, AR_U16 u16ManRatioEnable, AR_U32 *au32Ratio,
    AR_U32 *au32IntTimeMax, AR_U32 *au32IntTimeMin, AR_U32 *pu32LFMaxIntTime)
{
    return;
}

/* Only used in LINE_WDR mode */
static AR_VOID cmos_ae_fswdr_attr_set(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
    CMOS_CHECK_POINTER_VOID(pstAeFSWDRAttr);

    genFSWDRMode[ViPipe]      = pstAeFSWDRAttr->enFSWDRMode;
    gu32MaxTimeGetCnt[ViPipe] = 0;

    return;
}

static AR_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default     = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set            = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    pstExpFuncs->pfn_cmos_inttime_update     = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update       = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table   = cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table   = AR_NULL;
    pstExpFuncs->pfn_cmos_get_inttime_max    = cmos_get_inttime_max;
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set  = cmos_ae_fswdr_attr_set;

    return AR_SUCCESS;
}

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN 0
#define GOLDEN_BGAIN 0
static AR_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstAwbSnsDft);
    SENSOR_GET_CTX(ViPipe, pstSnsState);
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
    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    switch(pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
        {
            strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_gc2093/gc2093_tuning_preview.bin");
            break;
        }

        case WDR_MODE_2To1_LINE:
        {
            strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_gc2093/gc2093_tuning_preview_hdr.bin");
            break;
        }
    }

    if(WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {
        pstDef->stWdrSwitchAttr.au32ExpRatio[0] = 0x40;
        pstDef->stWdrSwitchAttr.au32ExpRatio[1] = 0x40;
        pstDef->stWdrSwitchAttr.au32ExpRatio[2] = 0x40;
        pstDef->stWdrSwitchAttr.au32VcCnt = 2;
        pstDef->stWdrSwitchAttr.au32VcMask = 0x03;
        //pstDef->stWdrSwitchAttr.au32ShortOffset = 475;
        //pstDef->stWdrSwitchAttr.au32MidOffset   = 475;
        //pstDef->stWdrSwitchAttr.au32LongOffset  = 14;
    }

    pstDef->stSensorMode.u32SensorID  = GC2093_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;
    memcpy(&pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    switch(pstSnsState->u8ImgMode)
    {
        default:
        case SENSOR_1080P_30FPS_LINEAR_MODE:
        {
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 12;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel   = 4095;
            break;
        }

        case SENSOR_1080P_30FPS_2T1_WDR_MODE:
        {
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 10;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel   = 1023;
            break;
        }
    }

    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Numerator   = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Numerator   = 1;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimCols = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatCols = 2;
    pstDef->stSensorMode.stDngRawFormat.enCfaLayout         = CFALAYOUT_TYPE_RECTANGULAR;
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
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    return;
}

static AR_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, AR_U8 u8Mode)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = AR_FALSE;

    switch(u8Mode)
    {
        case WDR_MODE_NONE:
        {
            pstSnsState->enWDRMode = WDR_MODE_NONE;
            ISP_TRACE(AR_DBG_INFO,"cmos_set_wdr_mode: linear mode\n");
            break;
        }

        case WDR_MODE_2To1_LINE:
        {
            pstSnsState->enWDRMode = WDR_MODE_2To1_LINE;
            ISP_TRACE(AR_DBG_INFO,"cmos_set_wdr_mode: 2to1 line WDR 1080p mode(60fps->30fps)\n");
            break;
        }

        default:
        {
            ISP_TRACE(AR_DBG_INFO,"NOT support this mode!\n");
            return AR_FAILURE;
        }
    }

    memset(pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime));
    return AR_SUCCESS;
}

static AR_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    AR_S32 i = 0;

    CMOS_CHECK_POINTER(pstSnsRegsInfo);
    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if((AR_FALSE == pstSnsState->bSyncInit) || (AR_FALSE == pstSnsRegsInfo->bConfig))
    {
        pstSnsState->astRegsInfo[0].enSnsType           = ISP_SNS_I2C_TYPE;
        pstSnsState->astRegsInfo[0].unComBus.s8I2cDev   = g_aunGc2093BusInfo[ViPipe].s8I2cDev;
        pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 3;
        pstSnsState->astRegsInfo[0].u32RegNum           = 15;

        if(WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
        {
            pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 3;
            pstSnsState->astRegsInfo[0].u32RegNum          += 2;    //4;
        }

        for(i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate        = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr      = sensor_i2c_addr;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = sensor_data_byte;
        }


        /* Linear mode regs */
        pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr     = GC2093_EXPTIME_ADDR_L;
        pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr     = GC2093_EXPTIME_ADDR_H;

        pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr     = GC2093_AGAIN_ADDR_L;
        pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr     = GC2093_DGAIN_ADDR_H;
        pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr     = GC2093_DGAIN_ADDR_L;
        pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr     = GC2093_GAIN_ADDR_0X155;
        pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr     = GC2093_GAIN_ADDR_0X31D;
        pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr     = GC2093_GAIN_ADDR_0XC2;
        pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr     = GC2093_GAIN_ADDR_0XCF;
        pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum  = 1;
        pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr     = GC2093_GAIN_ADDR_0XD9;
        pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr    = GC2093_GAIN_ADDR_0X31D;
        pstSnsState->astRegsInfo[0].astI2cData[11].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[11].u32RegAddr    = GC2093_AUTO_PREGAIN_ADDR_L;
        pstSnsState->astRegsInfo[0].astI2cData[12].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[12].u32RegAddr    = GC2093_AUTO_PREGAIN_ADDR_H;

        pstSnsState->astRegsInfo[0].astI2cData[13].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[13].u32RegAddr    = GC2093_VMAX_ADDR_L;
        pstSnsState->astRegsInfo[0].astI2cData[14].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[14].u32RegAddr    = GC2093_VMAX_ADDR_H;


        /* WDR mode regs */
        if(WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
        {
#if 0
            pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr     = GC2093_EXPTIME_ADDR_L;
            pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr     = GC2093_EXPTIME_ADDR_H;

            pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr     = GC2093_AGAIN_ADDR_H;
            pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr     = GC2093_AGAIN_ADDR_L;

            pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr     = GC2093_DGAIN_ADDR_L;
            pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr     = GC2093_DGAIN_ADDR_H;

            pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr     = GC2093_AUTO_PREGAIN_ADDR_L;
            pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum  = 1;
            pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr     = GC2093_AUTO_PREGAIN_ADDR_H;

            pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum  = 0;
            pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr     = GC2093_VMAX_ADDR_L;
            pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum  = 0;
            pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr     = GC2093_VMAX_ADDR_H;
#else
            pstSnsState->astRegsInfo[0].astI2cData[15].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[15].u32RegAddr    = GC2093_SHORT_EXPTIME_ADDR_L;
            pstSnsState->astRegsInfo[0].astI2cData[16].u8DelayFrmNum = 1;
            pstSnsState->astRegsInfo[0].astI2cData[16].u32RegAddr    = GC2093_SHORT_EXPTIME_ADDR_H;
#endif
        }

        pstSnsState->bSyncInit = AR_TRUE;
    }
    else
    {
        for(i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum - 2; i++)
        {
            if(pstSnsState->astRegsInfo[0].astI2cData[i].u32Data == pstSnsState->astRegsInfo[1].astI2cData[i].u32Data)
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_FALSE;
            }
            else
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = AR_TRUE;
            }
        }

        if((pstSnsState->astRegsInfo[0].astI2cData[11].bUpdate == AR_TRUE)
            || (pstSnsState->astRegsInfo[0].astI2cData[12].bUpdate == AR_TRUE))
        {
            pstSnsState->astRegsInfo[0].astI2cData[11].bUpdate = AR_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[12].bUpdate = AR_TRUE;
        }

        pstSnsState->astRegsInfo[0].astI2cData[13].bUpdate = AR_TRUE;
        pstSnsState->astRegsInfo[0].astI2cData[14].bUpdate = AR_TRUE;
    }

	pstSnsState->astRegsInfo[0].bConfig=pstSnsRegsInfo->bConfig;
    memcpy(pstSnsRegsInfo, &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&pstSnsState->astRegsInfo[1], &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));

    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];

    /* Set real register for aec */
	
    if(pstSnsRegsInfo->bConfig) sensor_default_reg_init(ViPipe);

    return AR_SUCCESS;
}

static AR_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    AR_U8 u8SensorImageMode = 0;
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = AR_FALSE;

    if(pstSensorImageMode->f32Fps <= 30)
    {
        if(WDR_MODE_NONE == pstSnsState->enWDRMode)
        {
            if(SENSOR_RES_IS_1080P(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode     = SENSOR_1080P_30FPS_LINEAR_MODE;
                pstSnsState->u32FLStd = SENSOR_VMAX_1080P30_LINEAR;
            }
            else
            {
                ar_always("%d: Not support! Width:%d, Height:%d, ImageMode:%d, SensorState:%d\n",
                    __LINE__, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, u8SensorImageMode,
                    pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }
        else if(WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
        {
            if(SENSOR_RES_IS_WDR(pstSensorImageMode->u16Width, pstSensorImageMode->u16Height))
            {
                u8SensorImageMode               = SENSOR_1080P_30FPS_2T1_WDR_MODE;
                pstSnsState->u32FLStd           = SENSOR_VMAX_1080P30_LINEAR;
                g_astGc2093State[ViPipe].u32BRL = 1109;
            }
            else
            {
                ar_always("%d: Not support! Width:%d, Height:%d, ImageMode:%d, SensorState:%d\n",
                    __LINE__, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, u8SensorImageMode,
                    pstSnsState->enWDRMode);
                return AR_FAILURE;
            }
        }
        else
        {
            ar_always("%d: Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
                __LINE__, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, pstSensorImageMode->f32Fps,
                pstSnsState->enWDRMode);
            return AR_FAILURE;
        }
    }

    if((AR_TRUE == pstSnsState->bInit) && (u8SensorImageMode == pstSnsState->u8ImgMode))
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

    SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit     = AR_FALSE;
    pstSnsState->bSyncInit = AR_FALSE;
    pstSnsState->u8ImgMode = SENSOR_1080P_30FPS_LINEAR_MODE;
    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->u32FLStd  = SENSOR_VMAX_1080P30_LINEAR;
    pstSnsState->au32FL[0] = SENSOR_VMAX_1080P30_LINEAR;
    pstSnsState->au32FL[1] = SENSOR_VMAX_1080P30_LINEAR;

    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
}

static AR_S32 cmos_set_mirror_flip(VI_PIPE ViPipe, int mode)
{
    AR_S32 ret = 0;

    ar_always("cmos_set_mirror_flip: mode=%d\n", mode);
    switch(mode)
    {
        case ISP_SNS_NORMAL:
        {
            ret = sensor_flip_off_mirror_off(ViPipe);
            break;
        }

        case ISP_SNS_MIRROR:
        {
            ret = sensor_flip_off_mirror_on(ViPipe);
            break;
        }

        case ISP_SNS_FLIP:
        {
            ret = sensor_flip_on_mirror_off(ViPipe);
            break;
        }

        case ISP_SNS_MIRROR_FLIP:
        {
            ret = sensor_flip_on_mirror_on(ViPipe);
            break;
        }

        default:
        {
        break;
        }
    }

    return ret;
}

static AR_S32 cmos_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
{
    AR_U8  u8CtlCode = 0;
    AR_S32 ret       = 0;
    AR_S32 mode      = 0;

    if(!pSensorCtl)
    {
         ar_always("sensor ctl failed!\n");
         return AR_ERR_ISP_INVALID_ADDR;
    }

    mode      = *(AR_S32 *)pSensorCtl->pCtlData;
    u8CtlCode = pSensorCtl->u8CtlCode;
    switch(u8CtlCode)
    {
        case SENSOR_FLIP_MIRROR:
        {
            ret = cmos_set_mirror_flip(ViPipe, mode);
            break;
        }

        default:
        {
            ar_always("error ctl code\n");
            ret = AR_FAILURE;
            break;
        }
    }

    return ret;
}

static AR_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);

    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));
    pstSensorExpFunc->pfn_cmos_sensor_init        = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit        = sensor_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode     = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode       = cmos_set_wdr_mode;

    pstSensorExpFunc->pfn_cmos_get_isp_default  = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

    pstSensorExpFunc->pfn_cmos_sns_power_on  = cmos_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = cmos_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl       = cmos_sensor_ctl;

    return AR_SUCCESS;
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
static AR_S32 sensor_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_aunGc2093BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

    return AR_SUCCESS;
}

static AR_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if(AR_NULL == pastSnsStateCtx)
    {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if(AR_NULL == pastSnsStateCtx)
        {
            ar_always("Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

    SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    SENSOR_RESET_CTX(ViPipe);
}

static AR_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret = AR_FAILURE;

    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = sensor_ctx_init(ViPipe);
    if(AR_SUCCESS != s32Ret)
    {
        return AR_FAILURE;
    }

    stSnsAttrInfo.eSensorId = GC2093_ID;

    s32Ret  = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if(AR_SUCCESS != s32Ret)
    {
        ar_always("sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if(AR_SUCCESS != s32Ret)
    {
        ar_always("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if(AR_SUCCESS != s32Ret)
    {
        ar_always("sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret = AR_FAILURE;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, GC2093_ID);
    if(AR_SUCCESS != s32Ret)
    {
        ar_always("sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, GC2093_ID);
    if(AR_SUCCESS != s32Ret)
    {
        ar_always("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, GC2093_ID);
    if(AR_SUCCESS != s32Ret)
    {
        ar_always("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(ViPipe);

    return AR_SUCCESS;
}

static AR_S32 sensor_set_init(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr)
{
    CMOS_CHECK_POINTER(pstInitAttr);

    g_au32InitExposure[ViPipe]  = pstInitAttr->u32Exposure;
    g_au32LinesPer500ms[ViPipe] = pstInitAttr->u32LinesPer500ms;
    g_au16InitWBGain[ViPipe][0] = pstInitAttr->u16WBRgain;
    g_au16InitWBGain[ViPipe][1] = pstInitAttr->u16WBGgain;
    g_au16InitWBGain[ViPipe][2] = pstInitAttr->u16WBBgain;
    g_au16SampleRgain[ViPipe]   = pstInitAttr->u16SampleRgain;
    g_au16SampleBgain[ViPipe]   = pstInitAttr->u16SampleBgain;

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S stSnsGc2093Obj =
{
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = sensor_standby,
    .pfnRestart             = sensor_restart,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = sensor_write_register,
    .pfnReadReg             = sensor_read_register,
    .pfnSetBusInfo          = sensor_set_bus_info,
    .pfnSetInit             = sensor_set_init
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __GC2093_CMOS_C_ */

