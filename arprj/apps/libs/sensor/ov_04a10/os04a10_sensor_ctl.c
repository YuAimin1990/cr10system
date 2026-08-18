#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"

#ifdef AR_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ar_i2c.h"
#endif

#define OS04A10_SENSOR_4M_25FPS_LINEAR_MODE      (1)
#define OS04A10_SENSOR_4M_25FPS_2T1_WDR_MODE     (2)
#define OS04A10_SENSOR_4M_25FPS_LINEAR_10BIT_MODE      (3)
#define OS04A10_SENSOR_BINNING_25FPS_LINEAR_10BIT_MODE (4)
#define OS04A10_SENSOR_BINNING_25FPS_LINEAR_12BIT_MODE (5)

const unsigned char os04a10_i2c_addr     =    0x6c;        /* I2C Address of OS04A10 */
const unsigned int  os04a10_addr_byte    =    2;
const unsigned int  os04a10_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastOs04a10[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunOs04a10BusInfo[];
#define OS04A10_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastOs04a10[dev])

int os04a10_i2c_init(VI_PIPE ViPipe)
{
    char acDevFile[16] = {0};
    AR_U8 u8DevNum;

    if (g_fd[ViPipe] >= 0)
    {

        return AR_SUCCESS;
    }
#ifdef AR_GPIO_I2C
    int ret;

    g_fd[ViPipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open gpioi2c_ex error!\n");
        return AR_FAILURE;
    }
#else
    int ret;

    u8DevNum =g_aunOs04a10BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	ar_always("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (os04a10_i2c_addr >> 1));
    if (ret < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }
#endif

    return AR_SUCCESS;
}

int os04a10_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int os04a10_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int os04a10_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = os04a10_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = os04a10_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = os04a10_data_byte;

    ret = ioctl(g_fd[ViPipe], GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        ISP_TRACE(AR_DBG_ERR, "GPIO-I2C write faild!\n");
        return ret;
    }
#else
    int idx = 0;
    int ret;
    char buf[8];

    if (os04a10_addr_byte == 2)
    {
        buf[idx] = (addr >> 8) & 0xff;
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    }
    else
    {
        //buf[idx] = addr & 0xff;
        //idx++;
    }

    if (os04a10_data_byte == 2)
    {
        //buf[idx] = (data >> 8) & 0xff;
        //idx++;
        //buf[idx] = data & 0xff;
        //idx++;
    }
    else
    {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[ViPipe], buf, os04a10_addr_byte + os04a10_data_byte);
    if (ret < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "I2C_WRITE error!\n");
        return AR_FAILURE;
    }

#endif
    return AR_SUCCESS;
}


static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void os04a10_prog(VI_PIPE ViPipe, int *rom)
{
    int i = 0;
    while (1)
    {
        int lookup = rom[i++];
        int addr = (lookup >> 16) & 0xFFFF;
        int data = lookup & 0xFFFF;
        if (addr == 0xFFFE)
        {
            delay_ms(data);
        }
        else if (addr == 0xFFFF)
        {
            return;
        }
        else
        {
            os04a10_write_register(ViPipe, addr, data);
        }
    }
}

void os04a10_trigger(VI_PIPE ViPipe, AR_S32 s32Frames)
{
    os04a10_write_register (ViPipe, 0x0100, 0x01); /* standby */	
  
    ar_always("===os04a10 trigger stream on!=====\n");
}

void os04a10_stream_on(VI_PIPE ViPipe, AR_S32 s32Frames)
{
    os04a10_write_register (ViPipe, 0x0100, 0x01); /* standby */	
  
    ar_always("===os04a10 stream on!=====\n");
}



void os04a10_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void os04a10_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define OS04A10_FLIP_MIRRO (0x3820)

int os04a10_flip_off_mirro_off(VI_PIPE ViPipe)
{
    os04a10_write_register(ViPipe, OS04A10_FLIP_MIRRO, 0x00);
    return 0;
}

int os04a10_flip_on_mirro_off(VI_PIPE ViPipe)
{
    os04a10_write_register(ViPipe, OS04A10_FLIP_MIRRO, 0x02);
    return 0;
}

int os04a10_flip_off_mirro_on(VI_PIPE ViPipe)
{
    os04a10_write_register(ViPipe, OS04A10_FLIP_MIRRO, 0x01);
    return 0;
}

int os04a10_flip_on_mirro_on(VI_PIPE ViPipe)
{
    os04a10_write_register(ViPipe, OS04A10_FLIP_MIRRO, 0x03);
    return 0;
}

void os04a10_hdr_4M25fps_10bit_init(VI_PIPE ViPipe);
void os04a10_linear_4Mp30_12bit_init(VI_PIPE ViPipe);
void os04a10_linear_4Mp30_10bit_init(VI_PIPE ViPipe);

void os04a10_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastOs04a10[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("update set reg[0x%x]: 0x%x \n", g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            os04a10_write_register(ViPipe, g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void os04a10_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastOs04a10[ViPipe]->bInit;
    enWDRMode   = g_pastOs04a10[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastOs04a10[ViPipe]->u8ImgMode;

    os04a10_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (OS04A10_SENSOR_4M_25FPS_2T1_WDR_MODE == u8ImgMode)
            {
                os04a10_hdr_4M25fps_10bit_init(ViPipe);
            }

        }else if (WDR_MODE_3To1_LINE == enWDRMode)
        {

        }
        else
        {
            if (OS04A10_SENSOR_4M_25FPS_LINEAR_MODE == u8ImgMode)
            {
                os04a10_linear_4Mp30_12bit_init(ViPipe);
            }
            if (OS04A10_SENSOR_4M_25FPS_LINEAR_10BIT_MODE == u8ImgMode)
            {
                os04a10_linear_4Mp30_10bit_init(ViPipe);
            }

			if (OS04A10_SENSOR_BINNING_25FPS_LINEAR_10BIT_MODE == u8ImgMode)
			{
				os04a10_linear_binning_1280x720_25fps_10bit_init(ViPipe);
			}

			if (OS04A10_SENSOR_BINNING_25FPS_LINEAR_12BIT_MODE == u8ImgMode)
			{
				os04a10_linear_binning_1280x720_25fps_12bit_init(ViPipe);
			}
        }

    }else /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            os04a10_hdr_4M25fps_10bit_init(ViPipe);

        }else if (WDR_MODE_3To1_LINE == enWDRMode)
        {

        }else
        {
            if (OS04A10_SENSOR_4M_25FPS_LINEAR_MODE == u8ImgMode)
                os04a10_linear_4Mp30_12bit_init(ViPipe);

            if (OS04A10_SENSOR_4M_25FPS_LINEAR_10BIT_MODE == u8ImgMode)
                os04a10_linear_4Mp30_10bit_init(ViPipe);

        }
    }

    // for (i = 0; i < g_pastOs04a10[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     os04a10_write_register(ViPipe, g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastOs04a10[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastOs04a10[ViPipe]->bInit = AR_TRUE;
    return ;
}

void os04a10_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    OS04A10_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);
    
	os04a10_write_register (ViPipe, 0x0100, 0x00); /* standby */
	//need clear astRegsInfo[1] for resume aec can't set new value
	memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));	
    return;
}


void os04a10_hdr_4M25fps_10bit_init(VI_PIPE ViPipe)
{
    //@@ 0 40 OS04A10 4lane 2560x1440 2Stagger HDR 25fps
    //; Delay 50ms
    //mclk 24M, pclk 72M, mipi data rate 792Mbps, 4 lane

    os04a10_write_register(ViPipe, 0x0103, 0x01);
    os04a10_write_register(ViPipe, 0x0109, 0x01);
    os04a10_write_register(ViPipe, 0x0104, 0x02);
    
    os04a10_write_register(ViPipe, 0x0102, 0x00);
    os04a10_write_register(ViPipe, 0x0305, 0x3c); //60
    os04a10_write_register(ViPipe, 0x0306, 0x00);
    os04a10_write_register(ViPipe, 0x0307, 0x00);
    os04a10_write_register(ViPipe, 0x0308, 0x04); //
    os04a10_write_register(ViPipe, 0x030a, 0x01);
    os04a10_write_register(ViPipe, 0x0317, 0x09);
    os04a10_write_register(ViPipe, 0x0322, 0x01);
    os04a10_write_register(ViPipe, 0x0323, 0x02);
    os04a10_write_register(ViPipe, 0x0324, 0x00);
    os04a10_write_register(ViPipe, 0x0325, 0x90);
    os04a10_write_register(ViPipe, 0x0327, 0x05);
    os04a10_write_register(ViPipe, 0x0329, 0x02);
    os04a10_write_register(ViPipe, 0x032c, 0x02);
    os04a10_write_register(ViPipe, 0x032d, 0x02);
    os04a10_write_register(ViPipe, 0x032e, 0x02);
    os04a10_write_register(ViPipe, 0x300f, 0x11);
    os04a10_write_register(ViPipe, 0x3012, 0x41);
    os04a10_write_register(ViPipe, 0x3026, 0x10);
    os04a10_write_register(ViPipe, 0x3027, 0x08);
    os04a10_write_register(ViPipe, 0x302d, 0x24);
    os04a10_write_register(ViPipe, 0x3104, 0x01);
    os04a10_write_register(ViPipe, 0x3106, 0x11);
    os04a10_write_register(ViPipe, 0x3400, 0x00);
    os04a10_write_register(ViPipe, 0x3408, 0x05);
    os04a10_write_register(ViPipe, 0x340c, 0x0c);
    os04a10_write_register(ViPipe, 0x340d, 0xb0);
    os04a10_write_register(ViPipe, 0x3425, 0x51);
    os04a10_write_register(ViPipe, 0x3426, 0x10);
    os04a10_write_register(ViPipe, 0x3427, 0x14);
    os04a10_write_register(ViPipe, 0x3428, 0x10);
    os04a10_write_register(ViPipe, 0x3429, 0x10);
    os04a10_write_register(ViPipe, 0x342a, 0x10);
    os04a10_write_register(ViPipe, 0x342b, 0x04);

    os04a10_write_register(ViPipe, 0x3501, 0x02); //hcg_exposrue_coarse L exposure time

    os04a10_write_register(ViPipe, 0x3504, 0x08);

    os04a10_write_register(ViPipe, 0x3508, 0x01); //hcg real gain
    os04a10_write_register(ViPipe, 0x3509, 0x00);

    os04a10_write_register(ViPipe, 0x350a, 0x01); //hcg digital gain
    os04a10_write_register(ViPipe, 0x3544, 0x08);

    os04a10_write_register(ViPipe, 0x3548, 0x01); //lcg real gain
    os04a10_write_register(ViPipe, 0x3549, 0x00);

    os04a10_write_register(ViPipe, 0x3584, 0x08);
    os04a10_write_register(ViPipe, 0x3588, 0x01);
    os04a10_write_register(ViPipe, 0x3589, 0x00);
    os04a10_write_register(ViPipe, 0x3601, 0x70);
    os04a10_write_register(ViPipe, 0x3604, 0xe3);
    os04a10_write_register(ViPipe, 0x3605, 0x7f);
    os04a10_write_register(ViPipe, 0x3606, 0x80);
    os04a10_write_register(ViPipe, 0x3608, 0xa8);
    os04a10_write_register(ViPipe, 0x360a, 0xd0);
    os04a10_write_register(ViPipe, 0x360b, 0x08);
    os04a10_write_register(ViPipe, 0x360e, 0xc8);
    os04a10_write_register(ViPipe, 0x360f, 0x66);
    os04a10_write_register(ViPipe, 0x3610, 0x89);
    os04a10_write_register(ViPipe, 0x3611, 0x8a);
    os04a10_write_register(ViPipe, 0x3612, 0x4e);
    os04a10_write_register(ViPipe, 0x3613, 0xbd);
    os04a10_write_register(ViPipe, 0x3614, 0x9b);
    os04a10_write_register(ViPipe, 0x362a, 0x0e);
    os04a10_write_register(ViPipe, 0x362b, 0x0e);
    os04a10_write_register(ViPipe, 0x362c, 0x0e);
    os04a10_write_register(ViPipe, 0x362d, 0x0e);
    os04a10_write_register(ViPipe, 0x362e, 0x1a);
    os04a10_write_register(ViPipe, 0x362f, 0x34);
    os04a10_write_register(ViPipe, 0x3630, 0x67);
    os04a10_write_register(ViPipe, 0x3631, 0x7f);
    os04a10_write_register(ViPipe, 0x3638, 0x00);
    os04a10_write_register(ViPipe, 0x3643, 0x00);
    os04a10_write_register(ViPipe, 0x3644, 0x00);
    os04a10_write_register(ViPipe, 0x3645, 0x00);
    os04a10_write_register(ViPipe, 0x3646, 0x00);
    os04a10_write_register(ViPipe, 0x3647, 0x00);
    os04a10_write_register(ViPipe, 0x3648, 0x00);
    os04a10_write_register(ViPipe, 0x3649, 0x00);
    os04a10_write_register(ViPipe, 0x364a, 0x04);
    os04a10_write_register(ViPipe, 0x364c, 0x0e);
    os04a10_write_register(ViPipe, 0x364d, 0x0e);
    os04a10_write_register(ViPipe, 0x364e, 0x0e);
    os04a10_write_register(ViPipe, 0x364f, 0x0e);
    os04a10_write_register(ViPipe, 0x3650, 0xff);
    os04a10_write_register(ViPipe, 0x3651, 0xff);
    os04a10_write_register(ViPipe, 0x365a, 0x00);
    os04a10_write_register(ViPipe, 0x365b, 0x00);
    os04a10_write_register(ViPipe, 0x365c, 0x00);
    os04a10_write_register(ViPipe, 0x365d, 0x00);
    os04a10_write_register(ViPipe, 0x3661, 0x07);
    os04a10_write_register(ViPipe, 0x3662, 0x02);
    os04a10_write_register(ViPipe, 0x3663, 0x20);
    os04a10_write_register(ViPipe, 0x3665, 0x12);
    os04a10_write_register(ViPipe, 0x3667, 0x54);
    os04a10_write_register(ViPipe, 0x3668, 0x80);
    os04a10_write_register(ViPipe, 0x366c, 0x00);
    os04a10_write_register(ViPipe, 0x366d, 0x00);
    os04a10_write_register(ViPipe, 0x366e, 0x00);
    os04a10_write_register(ViPipe, 0x366f, 0x00);
    os04a10_write_register(ViPipe, 0x3671, 0x09);
    os04a10_write_register(ViPipe, 0x3673, 0x2a);
    os04a10_write_register(ViPipe, 0x3681, 0x80);
    os04a10_write_register(ViPipe, 0x3700, 0x2d);
    os04a10_write_register(ViPipe, 0x3701, 0x22);
    os04a10_write_register(ViPipe, 0x3702, 0x25);
    os04a10_write_register(ViPipe, 0x3703, 0x20);
    os04a10_write_register(ViPipe, 0x3705, 0x00);
    os04a10_write_register(ViPipe, 0x3706, 0x72);
    os04a10_write_register(ViPipe, 0x3707, 0x0a);
    os04a10_write_register(ViPipe, 0x3708, 0x36);
    os04a10_write_register(ViPipe, 0x3709, 0x57);
    os04a10_write_register(ViPipe, 0x370a, 0x01);
    os04a10_write_register(ViPipe, 0x370b, 0x14);
    os04a10_write_register(ViPipe, 0x3714, 0x01);
    os04a10_write_register(ViPipe, 0x3719, 0x1f);
    os04a10_write_register(ViPipe, 0x371b, 0x16);
    os04a10_write_register(ViPipe, 0x371c, 0x00);
    os04a10_write_register(ViPipe, 0x371d, 0x08);
    os04a10_write_register(ViPipe, 0x373f, 0x63);
    os04a10_write_register(ViPipe, 0x3740, 0x63);
    os04a10_write_register(ViPipe, 0x3741, 0x63);
    os04a10_write_register(ViPipe, 0x3742, 0x63);
    os04a10_write_register(ViPipe, 0x3743, 0x01);
    os04a10_write_register(ViPipe, 0x3756, 0x9d);
    os04a10_write_register(ViPipe, 0x3757, 0x9d);
    os04a10_write_register(ViPipe, 0x3762, 0x1c);
    os04a10_write_register(ViPipe, 0x376c, 0x04);
    os04a10_write_register(ViPipe, 0x3776, 0x05);
    os04a10_write_register(ViPipe, 0x3777, 0x22);
    os04a10_write_register(ViPipe, 0x3779, 0x60);
    os04a10_write_register(ViPipe, 0x377c, 0x48);
    os04a10_write_register(ViPipe, 0x3784, 0x06);
    os04a10_write_register(ViPipe, 0x3785, 0x0a);
    os04a10_write_register(ViPipe, 0x3790, 0x10);
    os04a10_write_register(ViPipe, 0x3793, 0x04);
    os04a10_write_register(ViPipe, 0x3794, 0x07);
    os04a10_write_register(ViPipe, 0x3796, 0x00);
    os04a10_write_register(ViPipe, 0x3797, 0x02);
    os04a10_write_register(ViPipe, 0x379c, 0x4d);
    os04a10_write_register(ViPipe, 0x37a1, 0x80);
    os04a10_write_register(ViPipe, 0x37bb, 0x88);
    os04a10_write_register(ViPipe, 0x37be, 0x48);
    os04a10_write_register(ViPipe, 0x37bf, 0x01);
    os04a10_write_register(ViPipe, 0x37c0, 0x01);
    os04a10_write_register(ViPipe, 0x37c4, 0x72);
    os04a10_write_register(ViPipe, 0x37c5, 0x72);
    os04a10_write_register(ViPipe, 0x37c6, 0x72);
    os04a10_write_register(ViPipe, 0x37ca, 0x21);
    os04a10_write_register(ViPipe, 0x37cc, 0x13);
    os04a10_write_register(ViPipe, 0x37cd, 0x90);
    os04a10_write_register(ViPipe, 0x37cf, 0x02);
    os04a10_write_register(ViPipe, 0x37d0, 0x00);
    os04a10_write_register(ViPipe, 0x37d1, 0x72);
    os04a10_write_register(ViPipe, 0x37d2, 0x01);
    os04a10_write_register(ViPipe, 0x37d3, 0x14);
    os04a10_write_register(ViPipe, 0x37d4, 0x00);
    os04a10_write_register(ViPipe, 0x37d5, 0x6c);
    os04a10_write_register(ViPipe, 0x37d6, 0x00);
    os04a10_write_register(ViPipe, 0x37d7, 0xf7);
    os04a10_write_register(ViPipe, 0x37d8, 0x01);
    os04a10_write_register(ViPipe, 0x37dc, 0x00);
    os04a10_write_register(ViPipe, 0x37dd, 0x00);
    os04a10_write_register(ViPipe, 0x37da, 0x00);
    os04a10_write_register(ViPipe, 0x37db, 0x00);

    os04a10_write_register(ViPipe, 0x3800, 0x00);//horizonal start 0
    os04a10_write_register(ViPipe, 0x3801, 0x00);

    os04a10_write_register(ViPipe, 0x3802, 0x00); //vertial start 0
    os04a10_write_register(ViPipe, 0x3803, 0x00);

    os04a10_write_register(ViPipe, 0x3804, 0x0a); //horizonal end 2703
    os04a10_write_register(ViPipe, 0x3805, 0x8f);

    os04a10_write_register(ViPipe, 0x3806, 0x05); //vertial end 1535
    os04a10_write_register(ViPipe, 0x3807, 0xff);

    os04a10_write_register(ViPipe, 0x3808, 0x0a); //2560
    os04a10_write_register(ViPipe, 0x3809, 0x00); //1440

    os04a10_write_register(ViPipe, 0x380a, 0x05); //1440
    os04a10_write_register(ViPipe, 0x380b, 0xa0);

    os04a10_write_register(ViPipe, 0x380c, 0x02); //hts 732 hts
    os04a10_write_register(ViPipe, 0x380d, 0xdc);

    os04a10_write_register(ViPipe, 0x380e, 0x07); //vts 1968
    os04a10_write_register(ViPipe, 0x380f, 0xb0);

    os04a10_write_register(ViPipe, 0x3811, 0x48);
    os04a10_write_register(ViPipe, 0x3813, 0x30);

    os04a10_write_register(ViPipe, 0x3814, 0x01);
    os04a10_write_register(ViPipe, 0x3815, 0x01);
    os04a10_write_register(ViPipe, 0x3816, 0x01);
    os04a10_write_register(ViPipe, 0x3817, 0x01);
    os04a10_write_register(ViPipe, 0x381c, 0x08);
    os04a10_write_register(ViPipe, 0x3820, 0x03);
    os04a10_write_register(ViPipe, 0x3821, 0x00);
    os04a10_write_register(ViPipe, 0x3822, 0x14);
    os04a10_write_register(ViPipe, 0x3823, 0x18);
    os04a10_write_register(ViPipe, 0x3826, 0x00);
    os04a10_write_register(ViPipe, 0x3827, 0x00);
    os04a10_write_register(ViPipe, 0x3833, 0x41);
    os04a10_write_register(ViPipe, 0x384c, 0x02);
    os04a10_write_register(ViPipe, 0x384d, 0xdc);
    os04a10_write_register(ViPipe, 0x3858, 0x3c);
    os04a10_write_register(ViPipe, 0x3865, 0x02);
    os04a10_write_register(ViPipe, 0x3866, 0x00);
    os04a10_write_register(ViPipe, 0x3867, 0x00);
    os04a10_write_register(ViPipe, 0x3868, 0x02);
    os04a10_write_register(ViPipe, 0x3900, 0x13);
    os04a10_write_register(ViPipe, 0x3940, 0x13);
    os04a10_write_register(ViPipe, 0x3980, 0x13);
    os04a10_write_register(ViPipe, 0x3c01, 0x11);
    os04a10_write_register(ViPipe, 0x3c05, 0x00);
    os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
    os04a10_write_register(ViPipe, 0x3c12, 0x0d);
    os04a10_write_register(ViPipe, 0x3c19, 0x00);
    os04a10_write_register(ViPipe, 0x3c21, 0x00);
    os04a10_write_register(ViPipe, 0x3c3a, 0x10);
    os04a10_write_register(ViPipe, 0x3c3b, 0x18);
    os04a10_write_register(ViPipe, 0x3c3d, 0xc6);
    os04a10_write_register(ViPipe, 0x3c55, 0xcb);
    os04a10_write_register(ViPipe, 0x3c5a, 0x55);
    os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
    os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
    os04a10_write_register(ViPipe, 0x3d8c, 0x70);
    os04a10_write_register(ViPipe, 0x3d8d, 0x10);
    os04a10_write_register(ViPipe, 0x4000, 0xf9);
    os04a10_write_register(ViPipe, 0x4001, 0xef);
    os04a10_write_register(ViPipe, 0x4004, 0x00);
    os04a10_write_register(ViPipe, 0x4005, 0x40);
    os04a10_write_register(ViPipe, 0x4008, 0x02);
    os04a10_write_register(ViPipe, 0x4009, 0x11);
    os04a10_write_register(ViPipe, 0x400a, 0x06);
    os04a10_write_register(ViPipe, 0x400b, 0x40);
    os04a10_write_register(ViPipe, 0x400e, 0x40);
    os04a10_write_register(ViPipe, 0x402e, 0x00);
    os04a10_write_register(ViPipe, 0x402f, 0x40);
    os04a10_write_register(ViPipe, 0x4030, 0x00);
    os04a10_write_register(ViPipe, 0x4031, 0x40);
    os04a10_write_register(ViPipe, 0x4032, 0x0f);
    os04a10_write_register(ViPipe, 0x4033, 0x80);
    os04a10_write_register(ViPipe, 0x4050, 0x00);
    os04a10_write_register(ViPipe, 0x4051, 0x07);
    os04a10_write_register(ViPipe, 0x4011, 0xbb);
    os04a10_write_register(ViPipe, 0x410f, 0x01);
    os04a10_write_register(ViPipe, 0x4288, 0xce);
    os04a10_write_register(ViPipe, 0x4289, 0x00);
    os04a10_write_register(ViPipe, 0x428a, 0x46);
    os04a10_write_register(ViPipe, 0x430b, 0x0f);
    os04a10_write_register(ViPipe, 0x430c, 0xfc);
    os04a10_write_register(ViPipe, 0x430d, 0x00);
    os04a10_write_register(ViPipe, 0x430e, 0x00);
    os04a10_write_register(ViPipe, 0x4314, 0x04);
    os04a10_write_register(ViPipe, 0x4500, 0x18);
    os04a10_write_register(ViPipe, 0x4501, 0x18);
    os04a10_write_register(ViPipe, 0x4503, 0x10);
    os04a10_write_register(ViPipe, 0x4504, 0x00);
    os04a10_write_register(ViPipe, 0x4506, 0x32);
    os04a10_write_register(ViPipe, 0x4507, 0x03);
    os04a10_write_register(ViPipe, 0x4601, 0x30);
    os04a10_write_register(ViPipe, 0x4603, 0x00);
    os04a10_write_register(ViPipe, 0x460a, 0x50);
    os04a10_write_register(ViPipe, 0x460c, 0x60);
    os04a10_write_register(ViPipe, 0x4640, 0x62);
    os04a10_write_register(ViPipe, 0x4646, 0xaa);
    os04a10_write_register(ViPipe, 0x4647, 0x55);
    os04a10_write_register(ViPipe, 0x4648, 0x99);
    os04a10_write_register(ViPipe, 0x4649, 0x66);
    os04a10_write_register(ViPipe, 0x464d, 0x00);
    os04a10_write_register(ViPipe, 0x4654, 0x11);
    os04a10_write_register(ViPipe, 0x4655, 0x22);
    os04a10_write_register(ViPipe, 0x4800, 0x44);
    os04a10_write_register(ViPipe, 0x480e, 0x04);
    os04a10_write_register(ViPipe, 0x4810, 0xff);
    os04a10_write_register(ViPipe, 0x4811, 0xff);
    os04a10_write_register(ViPipe, 0x4813, 0x84);
    os04a10_write_register(ViPipe, 0x481f, 0x30);
    os04a10_write_register(ViPipe, 0x4837, 0x0e);
    os04a10_write_register(ViPipe, 0x484b, 0x67);
    os04a10_write_register(ViPipe, 0x4d00, 0x4d);
    os04a10_write_register(ViPipe, 0x4d01, 0x9d);
    os04a10_write_register(ViPipe, 0x4d02, 0xb9);
    os04a10_write_register(ViPipe, 0x4d03, 0x2e);
    os04a10_write_register(ViPipe, 0x4d04, 0x4a);
    os04a10_write_register(ViPipe, 0x4d05, 0x3d);
    os04a10_write_register(ViPipe, 0x4d09, 0x4f);
    os04a10_write_register(ViPipe, 0x5000, 0x1f);
    os04a10_write_register(ViPipe, 0x5001, 0x0c);
    os04a10_write_register(ViPipe, 0x5080, 0x00);
    os04a10_write_register(ViPipe, 0x50c0, 0x00);
    os04a10_write_register(ViPipe, 0x5100, 0x00);
    os04a10_write_register(ViPipe, 0x5200, 0x00);
    os04a10_write_register(ViPipe, 0x5201, 0x00);
    os04a10_write_register(ViPipe, 0x5202, 0x03);
    os04a10_write_register(ViPipe, 0x5203, 0xff);
    os04a10_write_register(ViPipe, 0x5780, 0x53);
    os04a10_write_register(ViPipe, 0x5782, 0x18);
    os04a10_write_register(ViPipe, 0x5783, 0x3c);
    os04a10_write_register(ViPipe, 0x5786, 0x01);
    os04a10_write_register(ViPipe, 0x5788, 0x18);
    os04a10_write_register(ViPipe, 0x5789, 0x3c);
    os04a10_write_register(ViPipe, 0x5792, 0x11);
    os04a10_write_register(ViPipe, 0x5793, 0x33);
    os04a10_write_register(ViPipe, 0x5857, 0xff);
    os04a10_write_register(ViPipe, 0x5858, 0xff);
    os04a10_write_register(ViPipe, 0x5859, 0xff);
    os04a10_write_register(ViPipe, 0x58d7, 0xff);
    os04a10_write_register(ViPipe, 0x58d8, 0xff);
    os04a10_write_register(ViPipe, 0x58d9, 0xff);
    os04a10_write_register(ViPipe, 0x0100, 0x01);
    os04a10_write_register(ViPipe, 0x0100, 0x01);

    ar_always("===new setting OS04A10 4M 25fps 10bit hdr Init OK!===\n");
}


/* 4MP25 */
void os04a10_linear_4Mp30_12bit_init(VI_PIPE ViPipe)
{

    //mclk 24M, pclk 72M, mipi data rate 720Mbps, 4 lane
	 ar_always("start new setting OS04A10 4M 25fps 12bit linear Init OK!===\n");
    os04a10_write_register(ViPipe, 0x0103, 0x01);
    os04a10_write_register(ViPipe, 0x0109, 0x01);
    os04a10_write_register(ViPipe, 0x0104, 0x02);
    
    os04a10_write_register(ViPipe, 0x0102, 0x00);
    os04a10_write_register(ViPipe, 0x0305, 0x42);
    os04a10_write_register(ViPipe, 0x0306, 0x00);
    os04a10_write_register(ViPipe, 0x0307, 0x00);
    os04a10_write_register(ViPipe, 0x0308, 0x05);
    os04a10_write_register(ViPipe, 0x030a, 0x01);
    os04a10_write_register(ViPipe, 0x0317, 0x0a);
    os04a10_write_register(ViPipe, 0x0322, 0x01);
    os04a10_write_register(ViPipe, 0x0323, 0x02);
    os04a10_write_register(ViPipe, 0x0324, 0x00);
    os04a10_write_register(ViPipe, 0x0325, 0x90);
    os04a10_write_register(ViPipe, 0x0327, 0x05);
    os04a10_write_register(ViPipe, 0x0329, 0x02);
    os04a10_write_register(ViPipe, 0x032c, 0x02);
    os04a10_write_register(ViPipe, 0x032d, 0x02);
    os04a10_write_register(ViPipe, 0x032e, 0x02);
    os04a10_write_register(ViPipe, 0x300f, 0x11);
    os04a10_write_register(ViPipe, 0x3012, 0x41);
    os04a10_write_register(ViPipe, 0x3026, 0x10);
    os04a10_write_register(ViPipe, 0x3027, 0x08);
    os04a10_write_register(ViPipe, 0x302d, 0x24);
    os04a10_write_register(ViPipe, 0x3104, 0x01);
    os04a10_write_register(ViPipe, 0x3106, 0x11);
    os04a10_write_register(ViPipe, 0x3400, 0x00);
    
    os04a10_write_register(ViPipe, 0x3408, 0x05);
    os04a10_write_register(ViPipe, 0x340c, 0x0c);
    os04a10_write_register(ViPipe, 0x340d, 0xb0);
    
    os04a10_write_register(ViPipe, 0x3425, 0x51);
    os04a10_write_register(ViPipe, 0x3426, 0x10);
    os04a10_write_register(ViPipe, 0x3427, 0x14);
    os04a10_write_register(ViPipe, 0x3428, 0x10);
    os04a10_write_register(ViPipe, 0x3429, 0x10);
    os04a10_write_register(ViPipe, 0x342a, 0x10);
    os04a10_write_register(ViPipe, 0x342b, 0x04);

    os04a10_write_register(ViPipe, 0x3501, 0x02); //L exposure time

    os04a10_write_register(ViPipe, 0x3504, 0x08);
    os04a10_write_register(ViPipe, 0x3508, 0x01);
    os04a10_write_register(ViPipe, 0x3509, 0x00);
    os04a10_write_register(ViPipe, 0x350a, 0x01);

    os04a10_write_register(ViPipe, 0x3544, 0x08);
    os04a10_write_register(ViPipe, 0x3548, 0x01);
    os04a10_write_register(ViPipe, 0x3549, 0x00);
    os04a10_write_register(ViPipe, 0x3584, 0x08);
    os04a10_write_register(ViPipe, 0x3588, 0x01);
    os04a10_write_register(ViPipe, 0x3589, 0x00);
    os04a10_write_register(ViPipe, 0x3601, 0x70);
    os04a10_write_register(ViPipe, 0x3604, 0xe3);
    os04a10_write_register(ViPipe, 0x3605, 0xff);
    os04a10_write_register(ViPipe, 0x3606, 0x01);
    os04a10_write_register(ViPipe, 0x3608, 0xa8);
    os04a10_write_register(ViPipe, 0x360a, 0xd0);
    os04a10_write_register(ViPipe, 0x360b, 0x08);
    os04a10_write_register(ViPipe, 0x360e, 0xc8);
    os04a10_write_register(ViPipe, 0x360f, 0x66);
    os04a10_write_register(ViPipe, 0x3610, 0x89);
    os04a10_write_register(ViPipe, 0x3611, 0x8a);
    os04a10_write_register(ViPipe, 0x3612, 0x4e);
    os04a10_write_register(ViPipe, 0x3613, 0xbd);
    os04a10_write_register(ViPipe, 0x3614, 0x9b);
    os04a10_write_register(ViPipe, 0x362a, 0x0e);
    os04a10_write_register(ViPipe, 0x362b, 0x0e);
    os04a10_write_register(ViPipe, 0x362c, 0x0e);
    os04a10_write_register(ViPipe, 0x362d, 0x09);
    os04a10_write_register(ViPipe, 0x362e, 0x1a);
    os04a10_write_register(ViPipe, 0x362f, 0x34);
    os04a10_write_register(ViPipe, 0x3630, 0x67);
    os04a10_write_register(ViPipe, 0x3631, 0x7f);
    os04a10_write_register(ViPipe, 0x3638, 0x00);
    os04a10_write_register(ViPipe, 0x3643, 0x00);
    os04a10_write_register(ViPipe, 0x3644, 0x00);
    os04a10_write_register(ViPipe, 0x3645, 0x00);
    os04a10_write_register(ViPipe, 0x3646, 0x00);
    os04a10_write_register(ViPipe, 0x3647, 0x00);
    os04a10_write_register(ViPipe, 0x3648, 0x00);
    os04a10_write_register(ViPipe, 0x3649, 0x00);
    os04a10_write_register(ViPipe, 0x364a, 0x04);
    os04a10_write_register(ViPipe, 0x364c, 0x0e);
    os04a10_write_register(ViPipe, 0x364d, 0x0e);
    os04a10_write_register(ViPipe, 0x364e, 0x0e);
    os04a10_write_register(ViPipe, 0x364f, 0x0e);
    os04a10_write_register(ViPipe, 0x3650, 0xff);
    os04a10_write_register(ViPipe, 0x3651, 0xff);
    os04a10_write_register(ViPipe, 0x365a, 0x00);
    os04a10_write_register(ViPipe, 0x365b, 0x00);
    os04a10_write_register(ViPipe, 0x365c, 0x00);
    os04a10_write_register(ViPipe, 0x365d, 0x00);
    os04a10_write_register(ViPipe, 0x3661, 0x07);
    os04a10_write_register(ViPipe, 0x3662, 0x00);
    os04a10_write_register(ViPipe, 0x3663, 0x20);
    os04a10_write_register(ViPipe, 0x3665, 0x12);
    os04a10_write_register(ViPipe, 0x3667, 0xd4);
    os04a10_write_register(ViPipe, 0x3668, 0x80);
    os04a10_write_register(ViPipe, 0x366c, 0x00);
    os04a10_write_register(ViPipe, 0x366d, 0x00);
    os04a10_write_register(ViPipe, 0x366e, 0x00);
    os04a10_write_register(ViPipe, 0x366f, 0x00);
    os04a10_write_register(ViPipe, 0x3671, 0x08);
    os04a10_write_register(ViPipe, 0x3673, 0x2a);
    os04a10_write_register(ViPipe, 0x3681, 0x80);
    os04a10_write_register(ViPipe, 0x3700, 0x2d);
    os04a10_write_register(ViPipe, 0x3701, 0x22);
    os04a10_write_register(ViPipe, 0x3702, 0x25);
    os04a10_write_register(ViPipe, 0x3703, 0x28);
    os04a10_write_register(ViPipe, 0x3705, 0x00);
    os04a10_write_register(ViPipe, 0x3706, 0xf0);
    os04a10_write_register(ViPipe, 0x3707, 0x0a);
    os04a10_write_register(ViPipe, 0x3708, 0x36);
    os04a10_write_register(ViPipe, 0x3709, 0x57);
    os04a10_write_register(ViPipe, 0x370a, 0x03);
    os04a10_write_register(ViPipe, 0x370b, 0x15);
    os04a10_write_register(ViPipe, 0x3714, 0x01);
    os04a10_write_register(ViPipe, 0x3719, 0x24);
    os04a10_write_register(ViPipe, 0x371b, 0x1f);
    os04a10_write_register(ViPipe, 0x371c, 0x00);
    os04a10_write_register(ViPipe, 0x371d, 0x08);
    os04a10_write_register(ViPipe, 0x373f, 0x63);
    os04a10_write_register(ViPipe, 0x3740, 0x63);
    os04a10_write_register(ViPipe, 0x3741, 0x63);
    os04a10_write_register(ViPipe, 0x3742, 0x63);
    os04a10_write_register(ViPipe, 0x3743, 0x01);
    os04a10_write_register(ViPipe, 0x3756, 0xe7);
    os04a10_write_register(ViPipe, 0x3757, 0xe7);
    os04a10_write_register(ViPipe, 0x3762, 0x1c);
    //os04a10_write_register(ViPipe, 0x376c, 0x00); //hcg
    os04a10_write_register(ViPipe, 0x376c, 0x10); //lcg

    //os04a10_write_register(ViPipe, 0x376c, 0x04); //hcg
    //os04a10_write_register(ViPipe, 0x3c55, 0xcb); //hcg

    //os04a10_write_register(ViPipe, 0x376c, 0x14); //lcg
    //os04a10_write_register(ViPipe, 0x3c55, 0x08); //lcg

    os04a10_write_register(ViPipe, 0x3776, 0x05);
    os04a10_write_register(ViPipe, 0x3777, 0x22);
    os04a10_write_register(ViPipe, 0x3779, 0x60);
    os04a10_write_register(ViPipe, 0x377c, 0x48);
    os04a10_write_register(ViPipe, 0x3784, 0x06);
    os04a10_write_register(ViPipe, 0x3785, 0x0a);
    os04a10_write_register(ViPipe, 0x3790, 0x10);
    os04a10_write_register(ViPipe, 0x3793, 0x04);
    os04a10_write_register(ViPipe, 0x3794, 0x07);
    os04a10_write_register(ViPipe, 0x3796, 0x00);
    os04a10_write_register(ViPipe, 0x3797, 0x02);
    os04a10_write_register(ViPipe, 0x379c, 0x4d);
    os04a10_write_register(ViPipe, 0x37a1, 0x80);
    os04a10_write_register(ViPipe, 0x37bb, 0x88);
    os04a10_write_register(ViPipe, 0x37be, 0x48);
    os04a10_write_register(ViPipe, 0x37bf, 0x01);
    os04a10_write_register(ViPipe, 0x37c0, 0x01);
    os04a10_write_register(ViPipe, 0x37c4, 0x72);
    os04a10_write_register(ViPipe, 0x37c5, 0x72);
    os04a10_write_register(ViPipe, 0x37c6, 0x72);
    os04a10_write_register(ViPipe, 0x37ca, 0x21);
    os04a10_write_register(ViPipe, 0x37cc, 0x15);
    os04a10_write_register(ViPipe, 0x37cd, 0x90);
    os04a10_write_register(ViPipe, 0x37cf, 0x02);
    os04a10_write_register(ViPipe, 0x37d0, 0x00);
    os04a10_write_register(ViPipe, 0x37d1, 0xf0);
    os04a10_write_register(ViPipe, 0x37d2, 0x03);
    os04a10_write_register(ViPipe, 0x37d3, 0x15);
    os04a10_write_register(ViPipe, 0x37d4, 0x01);
    os04a10_write_register(ViPipe, 0x37d5, 0x00);
    os04a10_write_register(ViPipe, 0x37d6, 0x03);
    os04a10_write_register(ViPipe, 0x37d7, 0x15);
    os04a10_write_register(ViPipe, 0x37d8, 0x01);
    os04a10_write_register(ViPipe, 0x37dc, 0x00);
    os04a10_write_register(ViPipe, 0x37dd, 0x00);
    os04a10_write_register(ViPipe, 0x37da, 0x00);
    os04a10_write_register(ViPipe, 0x37db, 0x00);

    os04a10_write_register(ViPipe, 0x3800, 0x00); //horizontal start 64
    os04a10_write_register(ViPipe, 0x3801, 0x40);

    os04a10_write_register(ViPipe, 0x3802, 0x00); //vertical start 40
    os04a10_write_register(ViPipe, 0x3803, 0x28);

    os04a10_write_register(ViPipe, 0x3804, 0x0a); //horizontal end 2639
    os04a10_write_register(ViPipe, 0x3805, 0x4f);

    os04a10_write_register(ViPipe, 0x3806, 0x05); //vertisal end 1495
    os04a10_write_register(ViPipe, 0x3807, 0xd7);

    os04a10_write_register(ViPipe, 0x3808, 0x0a); //out horizonal 2560
    os04a10_write_register(ViPipe, 0x3809, 0x00);

    os04a10_write_register(ViPipe, 0x380a, 0x05); //out vertial 1440
    os04a10_write_register(ViPipe, 0x380b, 0xa0);

    os04a10_write_register(ViPipe, 0x380c, 0x05); //htotal 1476
    os04a10_write_register(ViPipe, 0x380d, 0xc4);

    os04a10_write_register(ViPipe, 0x380e, 0x07); //vtotal 1948
    os04a10_write_register(ViPipe, 0x380f, 0x9c);

    os04a10_write_register(ViPipe, 0x3811, 0x08); //isp_x_win_trunc 8

    os04a10_write_register(ViPipe, 0x3813, 0x08); //isp_y_win_trunc 8

    os04a10_write_register(ViPipe, 0x3814, 0x01);
    os04a10_write_register(ViPipe, 0x3815, 0x01);
    os04a10_write_register(ViPipe, 0x3816, 0x01);
    os04a10_write_register(ViPipe, 0x3817, 0x01);
    os04a10_write_register(ViPipe, 0x381c, 0x00);
    os04a10_write_register(ViPipe, 0x3820, 0x02);
    os04a10_write_register(ViPipe, 0x3821, 0x00);
    os04a10_write_register(ViPipe, 0x3822, 0x14);
    os04a10_write_register(ViPipe, 0x3823, 0x18);
    os04a10_write_register(ViPipe, 0x3826, 0x00);
    os04a10_write_register(ViPipe, 0x3827, 0x00);
    os04a10_write_register(ViPipe, 0x3833, 0x40);
    os04a10_write_register(ViPipe, 0x384c, 0x05);
    os04a10_write_register(ViPipe, 0x384d, 0xc4);
    os04a10_write_register(ViPipe, 0x3858, 0x3c);
    os04a10_write_register(ViPipe, 0x3865, 0x02);
    os04a10_write_register(ViPipe, 0x3866, 0x00);
    os04a10_write_register(ViPipe, 0x3867, 0x00);
    os04a10_write_register(ViPipe, 0x3868, 0x02);
    os04a10_write_register(ViPipe, 0x3900, 0x13);
    os04a10_write_register(ViPipe, 0x3940, 0x13);
    os04a10_write_register(ViPipe, 0x3980, 0x13);
    os04a10_write_register(ViPipe, 0x3c01, 0x11);
    os04a10_write_register(ViPipe, 0x3c05, 0x00);
    os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
    os04a10_write_register(ViPipe, 0x3c12, 0x0d);
    os04a10_write_register(ViPipe, 0x3c19, 0x00);
    os04a10_write_register(ViPipe, 0x3c21, 0x00);
    os04a10_write_register(ViPipe, 0x3c3a, 0x10);
    os04a10_write_register(ViPipe, 0x3c3b, 0x18);
    os04a10_write_register(ViPipe, 0x3c3d, 0xc6);

    os04a10_write_register(ViPipe, 0x3c55, 0xcb); //hcg
    //os04a10_write_register(ViPipe, 0x376c, 0x14); //lcg
    //os04a10_write_register(ViPipe, 0x3c55, 0x08); //lcg

    os04a10_write_register(ViPipe, 0x3c5a, 0xe5);
    os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
    os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
    os04a10_write_register(ViPipe, 0x3d8c, 0x70);
    os04a10_write_register(ViPipe, 0x3d8d, 0x10);
    os04a10_write_register(ViPipe, 0x4000, 0xf9);
    os04a10_write_register(ViPipe, 0x4001, 0x2f);
    os04a10_write_register(ViPipe, 0x4004, 0x00);
    os04a10_write_register(ViPipe, 0x4005, 0x80);
    os04a10_write_register(ViPipe, 0x4008, 0x02);
    os04a10_write_register(ViPipe, 0x4009, 0x11);
    os04a10_write_register(ViPipe, 0x400a, 0x03);
    os04a10_write_register(ViPipe, 0x400b, 0x27);
    os04a10_write_register(ViPipe, 0x400e, 0x40);
    os04a10_write_register(ViPipe, 0x402e, 0x00);
    os04a10_write_register(ViPipe, 0x402f, 0x80);
    os04a10_write_register(ViPipe, 0x4030, 0x00);
    os04a10_write_register(ViPipe, 0x4031, 0x80);
    os04a10_write_register(ViPipe, 0x4032, 0x9f);
    os04a10_write_register(ViPipe, 0x4033, 0x80);
    os04a10_write_register(ViPipe, 0x4050, 0x00);
    os04a10_write_register(ViPipe, 0x4051, 0x07);
    os04a10_write_register(ViPipe, 0x4011, 0xbb);
    os04a10_write_register(ViPipe, 0x410f, 0x01);
    os04a10_write_register(ViPipe, 0x4288, 0xcf);
    os04a10_write_register(ViPipe, 0x4289, 0x00);
    os04a10_write_register(ViPipe, 0x428a, 0x46);
    os04a10_write_register(ViPipe, 0x430b, 0xff);
    os04a10_write_register(ViPipe, 0x430c, 0xff);
    os04a10_write_register(ViPipe, 0x430d, 0x00);
    os04a10_write_register(ViPipe, 0x430e, 0x00);
    os04a10_write_register(ViPipe, 0x4314, 0x04);
    os04a10_write_register(ViPipe, 0x4500, 0x18);
    os04a10_write_register(ViPipe, 0x4501, 0x18);
    os04a10_write_register(ViPipe, 0x4503, 0x10);
    os04a10_write_register(ViPipe, 0x4504, 0x00);
    os04a10_write_register(ViPipe, 0x4506, 0x32);
    os04a10_write_register(ViPipe, 0x4507, 0x02);
    os04a10_write_register(ViPipe, 0x4601, 0x30);
    os04a10_write_register(ViPipe, 0x4603, 0x00);
    os04a10_write_register(ViPipe, 0x460a, 0x50);
    os04a10_write_register(ViPipe, 0x460c, 0x60);
    os04a10_write_register(ViPipe, 0x4640, 0x62);
    os04a10_write_register(ViPipe, 0x4646, 0xaa);
    os04a10_write_register(ViPipe, 0x4647, 0x55);
    os04a10_write_register(ViPipe, 0x4648, 0x99);
    os04a10_write_register(ViPipe, 0x4649, 0x66);
    os04a10_write_register(ViPipe, 0x464d, 0x00);
    os04a10_write_register(ViPipe, 0x4654, 0x11);
    os04a10_write_register(ViPipe, 0x4655, 0x22);
    os04a10_write_register(ViPipe, 0x4800, 0x44);
    os04a10_write_register(ViPipe, 0x480e, 0x00);
    os04a10_write_register(ViPipe, 0x4810, 0xff);
    os04a10_write_register(ViPipe, 0x4811, 0xff);
    os04a10_write_register(ViPipe, 0x4813, 0x00);
    os04a10_write_register(ViPipe, 0x481f, 0x30);
    os04a10_write_register(ViPipe, 0x4837, 0x14);
    os04a10_write_register(ViPipe, 0x484b, 0x27);
    os04a10_write_register(ViPipe, 0x4d00, 0x4d);
    os04a10_write_register(ViPipe, 0x4d01, 0x9d);
    os04a10_write_register(ViPipe, 0x4d02, 0xb9);
    os04a10_write_register(ViPipe, 0x4d03, 0x2e);
    os04a10_write_register(ViPipe, 0x4d04, 0x4a);
    os04a10_write_register(ViPipe, 0x4d05, 0x3d);
    os04a10_write_register(ViPipe, 0x4d09, 0x4f);
    os04a10_write_register(ViPipe, 0x5000, 0x7f);
    os04a10_write_register(ViPipe, 0x5001, 0x0d);
    os04a10_write_register(ViPipe, 0x5080, 0x00);
    os04a10_write_register(ViPipe, 0x50c0, 0x00);
    os04a10_write_register(ViPipe, 0x5100, 0x00);
    os04a10_write_register(ViPipe, 0x5200, 0x00);
    os04a10_write_register(ViPipe, 0x5201, 0x00);
    os04a10_write_register(ViPipe, 0x5202, 0x03);
    os04a10_write_register(ViPipe, 0x5203, 0xff);
    os04a10_write_register(ViPipe, 0x5780, 0x53);
    os04a10_write_register(ViPipe, 0x5782, 0x60);
    os04a10_write_register(ViPipe, 0x5783, 0xf0);
    os04a10_write_register(ViPipe, 0x5786, 0x01);
    os04a10_write_register(ViPipe, 0x5788, 0x60);
    os04a10_write_register(ViPipe, 0x5789, 0xf0);
    os04a10_write_register(ViPipe, 0x5792, 0x11);
    os04a10_write_register(ViPipe, 0x5793, 0x33);
    os04a10_write_register(ViPipe, 0x5857, 0xff);
    os04a10_write_register(ViPipe, 0x5858, 0xff);
    os04a10_write_register(ViPipe, 0x5859, 0xff);
    os04a10_write_register(ViPipe, 0x58d7, 0xff);
    os04a10_write_register(ViPipe, 0x58d8, 0xff);
    os04a10_write_register(ViPipe, 0x58d9, 0xff);
    //os04a10_write_register(ViPipe, 0x0100, 0x01);
    os04a10_write_register(ViPipe, 0x4800, 0x64);

    ar_always("===new setting OS04A10 4M 25fps 12bit linear Init OK!===\n");
    return;
}

void os04a10_linear_4Mp30_10bit_init(VI_PIPE ViPipe)
{
    os04a10_write_register(ViPipe, 0x0102, 0x00);
    os04a10_write_register(ViPipe, 0x0305, 0x3c);
    os04a10_write_register(ViPipe, 0x0306, 0x00);
    os04a10_write_register(ViPipe, 0x0307, 0x00);
    os04a10_write_register(ViPipe, 0x0308, 0x04);
    os04a10_write_register(ViPipe, 0x030a, 0x01);
    os04a10_write_register(ViPipe, 0x0317, 0x09);
    os04a10_write_register(ViPipe, 0x0322, 0x01);
    os04a10_write_register(ViPipe, 0x0323, 0x02);
    os04a10_write_register(ViPipe, 0x0324, 0x00);
    os04a10_write_register(ViPipe, 0x0325, 0x90);
    os04a10_write_register(ViPipe, 0x0327, 0x05);
    os04a10_write_register(ViPipe, 0x0329, 0x02);
    os04a10_write_register(ViPipe, 0x032c, 0x02);
    os04a10_write_register(ViPipe, 0x032d, 0x02);
    os04a10_write_register(ViPipe, 0x032e, 0x02);
    os04a10_write_register(ViPipe, 0x300f, 0x11);
    os04a10_write_register(ViPipe, 0x3012, 0x41);
    os04a10_write_register(ViPipe, 0x3026, 0x10);
    os04a10_write_register(ViPipe, 0x3027, 0x08);
    os04a10_write_register(ViPipe, 0x302d, 0x24);
    os04a10_write_register(ViPipe, 0x3104, 0x01);
    os04a10_write_register(ViPipe, 0x3106, 0x11);
    os04a10_write_register(ViPipe, 0x3400, 0x00);
    os04a10_write_register(ViPipe, 0x3408, 0x05);
    os04a10_write_register(ViPipe, 0x340c, 0x0c);
    os04a10_write_register(ViPipe, 0x340d, 0xb0);
    os04a10_write_register(ViPipe, 0x3425, 0x51);
    os04a10_write_register(ViPipe, 0x3426, 0x10);
    os04a10_write_register(ViPipe, 0x3427, 0x14);
    os04a10_write_register(ViPipe, 0x3428, 0x10);
    os04a10_write_register(ViPipe, 0x3429, 0x10);
    os04a10_write_register(ViPipe, 0x342a, 0x10);
    os04a10_write_register(ViPipe, 0x342b, 0x04);
    os04a10_write_register(ViPipe, 0x3501, 0x02);
    os04a10_write_register(ViPipe, 0x3504, 0x08);
    os04a10_write_register(ViPipe, 0x3508, 0x01);
    os04a10_write_register(ViPipe, 0x3509, 0x00);
    os04a10_write_register(ViPipe, 0x350a, 0x01);
    os04a10_write_register(ViPipe, 0x3544, 0x08);
    os04a10_write_register(ViPipe, 0x3548, 0x01);
    os04a10_write_register(ViPipe, 0x3549, 0x00);
    os04a10_write_register(ViPipe, 0x3584, 0x08);
    os04a10_write_register(ViPipe, 0x3588, 0x01);
    os04a10_write_register(ViPipe, 0x3589, 0x00);
    os04a10_write_register(ViPipe, 0x3601, 0x70);
    os04a10_write_register(ViPipe, 0x3604, 0xe3);
    os04a10_write_register(ViPipe, 0x3605, 0x7f);
    os04a10_write_register(ViPipe, 0x3606, 0x80);
    os04a10_write_register(ViPipe, 0x3608, 0xa8);
    os04a10_write_register(ViPipe, 0x360a, 0xd0);
    os04a10_write_register(ViPipe, 0x360b, 0x08);
    os04a10_write_register(ViPipe, 0x360e, 0xc8);
    os04a10_write_register(ViPipe, 0x360f, 0x66);
    os04a10_write_register(ViPipe, 0x3610, 0x89);
    os04a10_write_register(ViPipe, 0x3611, 0x8a);
    os04a10_write_register(ViPipe, 0x3612, 0x4e);
    os04a10_write_register(ViPipe, 0x3613, 0xbd);
    os04a10_write_register(ViPipe, 0x3614, 0x9b);
    os04a10_write_register(ViPipe, 0x362a, 0x0e);
    os04a10_write_register(ViPipe, 0x362b, 0x0e);
    os04a10_write_register(ViPipe, 0x362c, 0x0e);
    os04a10_write_register(ViPipe, 0x362d, 0x0e);
    os04a10_write_register(ViPipe, 0x362e, 0x1a);
    os04a10_write_register(ViPipe, 0x362f, 0x34);
    os04a10_write_register(ViPipe, 0x3630, 0x67);
    os04a10_write_register(ViPipe, 0x3631, 0x7f);
    os04a10_write_register(ViPipe, 0x3638, 0x00);
    os04a10_write_register(ViPipe, 0x3643, 0x00);
    os04a10_write_register(ViPipe, 0x3644, 0x00);
    os04a10_write_register(ViPipe, 0x3645, 0x00);
    os04a10_write_register(ViPipe, 0x3646, 0x00);
    os04a10_write_register(ViPipe, 0x3647, 0x00);
    os04a10_write_register(ViPipe, 0x3648, 0x00);
    os04a10_write_register(ViPipe, 0x3649, 0x00);
    os04a10_write_register(ViPipe, 0x364a, 0x04);
    os04a10_write_register(ViPipe, 0x364c, 0x0e);
    os04a10_write_register(ViPipe, 0x364d, 0x0e);
    os04a10_write_register(ViPipe, 0x364e, 0x0e);
    os04a10_write_register(ViPipe, 0x364f, 0x0e);
    os04a10_write_register(ViPipe, 0x3650, 0xff);
    os04a10_write_register(ViPipe, 0x3651, 0xff);
    os04a10_write_register(ViPipe, 0x365a, 0x00);
    os04a10_write_register(ViPipe, 0x365b, 0x00);
    os04a10_write_register(ViPipe, 0x365c, 0x00);
    os04a10_write_register(ViPipe, 0x365d, 0x00);
    os04a10_write_register(ViPipe, 0x3661, 0x07);
    os04a10_write_register(ViPipe, 0x3662, 0x02);
    os04a10_write_register(ViPipe, 0x3663, 0x20);
    os04a10_write_register(ViPipe, 0x3665, 0x12);
    os04a10_write_register(ViPipe, 0x3667, 0xd4);
    os04a10_write_register(ViPipe, 0x3668, 0x80);
    os04a10_write_register(ViPipe, 0x366c, 0x00);
    os04a10_write_register(ViPipe, 0x366d, 0x00);
    os04a10_write_register(ViPipe, 0x366e, 0x00);
    os04a10_write_register(ViPipe, 0x366f, 0x00);
    os04a10_write_register(ViPipe, 0x3671, 0x08);
    os04a10_write_register(ViPipe, 0x3673, 0x2a);
    os04a10_write_register(ViPipe, 0x3681, 0x80);
    os04a10_write_register(ViPipe, 0x3700, 0x2d);
    os04a10_write_register(ViPipe, 0x3701, 0x22);
    os04a10_write_register(ViPipe, 0x3702, 0x25);
    os04a10_write_register(ViPipe, 0x3703, 0x20);
    os04a10_write_register(ViPipe, 0x3705, 0x00);
    os04a10_write_register(ViPipe, 0x3706, 0x72);
    os04a10_write_register(ViPipe, 0x3707, 0x0a);
    os04a10_write_register(ViPipe, 0x3708, 0x36);
    os04a10_write_register(ViPipe, 0x3709, 0x57);
    os04a10_write_register(ViPipe, 0x370a, 0x01);
    os04a10_write_register(ViPipe, 0x370b, 0x14);
    os04a10_write_register(ViPipe, 0x3714, 0x01);
    os04a10_write_register(ViPipe, 0x3719, 0x1f);
    os04a10_write_register(ViPipe, 0x371b, 0x16);
    os04a10_write_register(ViPipe, 0x371c, 0x00);
    os04a10_write_register(ViPipe, 0x371d, 0x08);
    os04a10_write_register(ViPipe, 0x373f, 0x63);
    os04a10_write_register(ViPipe, 0x3740, 0x63);
    os04a10_write_register(ViPipe, 0x3741, 0x63);
    os04a10_write_register(ViPipe, 0x3742, 0x63);
    os04a10_write_register(ViPipe, 0x3743, 0x01);
    os04a10_write_register(ViPipe, 0x3756, 0x9d);
    os04a10_write_register(ViPipe, 0x3757, 0x9d);
    os04a10_write_register(ViPipe, 0x3762, 0x1c);
    os04a10_write_register(ViPipe, 0x376c, 0x04);
    os04a10_write_register(ViPipe, 0x3776, 0x05);
    os04a10_write_register(ViPipe, 0x3777, 0x22);
    os04a10_write_register(ViPipe, 0x3779, 0x60);
    os04a10_write_register(ViPipe, 0x377c, 0x48);
    os04a10_write_register(ViPipe, 0x3784, 0x06);
    os04a10_write_register(ViPipe, 0x3785, 0x0a);
    os04a10_write_register(ViPipe, 0x3790, 0x10);
    os04a10_write_register(ViPipe, 0x3793, 0x04);
    os04a10_write_register(ViPipe, 0x3794, 0x07);
    os04a10_write_register(ViPipe, 0x3796, 0x00);
    os04a10_write_register(ViPipe, 0x3797, 0x02);
    os04a10_write_register(ViPipe, 0x379c, 0x4d);
    os04a10_write_register(ViPipe, 0x37a1, 0x80);
    os04a10_write_register(ViPipe, 0x37bb, 0x88);
    os04a10_write_register(ViPipe, 0x37be, 0x48);
    os04a10_write_register(ViPipe, 0x37bf, 0x01);
    os04a10_write_register(ViPipe, 0x37c0, 0x01);
    os04a10_write_register(ViPipe, 0x37c4, 0x72);
    os04a10_write_register(ViPipe, 0x37c5, 0x72);
    os04a10_write_register(ViPipe, 0x37c6, 0x72);
    os04a10_write_register(ViPipe, 0x37ca, 0x21);
    os04a10_write_register(ViPipe, 0x37cc, 0x13);
    os04a10_write_register(ViPipe, 0x37cd, 0x90);
    os04a10_write_register(ViPipe, 0x37cf, 0x02);
    os04a10_write_register(ViPipe, 0x37d0, 0x00);
    os04a10_write_register(ViPipe, 0x37d1, 0x72);
    os04a10_write_register(ViPipe, 0x37d2, 0x01);
    os04a10_write_register(ViPipe, 0x37d3, 0x14);
    os04a10_write_register(ViPipe, 0x37d4, 0x00);
    os04a10_write_register(ViPipe, 0x37d5, 0x6c);
    os04a10_write_register(ViPipe, 0x37d6, 0x00);
    os04a10_write_register(ViPipe, 0x37d7, 0xf7);
    os04a10_write_register(ViPipe, 0x37d8, 0x01);
    os04a10_write_register(ViPipe, 0x37dc, 0x00);
    os04a10_write_register(ViPipe, 0x37dd, 0x00);
    os04a10_write_register(ViPipe, 0x37da, 0x00);
    os04a10_write_register(ViPipe, 0x37db, 0x00);
    os04a10_write_register(ViPipe, 0x3800, 0x00);
    os04a10_write_register(ViPipe, 0x3801, 0x00);
    os04a10_write_register(ViPipe, 0x3802, 0x00);
    os04a10_write_register(ViPipe, 0x3803, 0x00);
    os04a10_write_register(ViPipe, 0x3804, 0x0a);
    os04a10_write_register(ViPipe, 0x3805, 0x8f);
    os04a10_write_register(ViPipe, 0x3806, 0x05);
    os04a10_write_register(ViPipe, 0x3807, 0xff);
    os04a10_write_register(ViPipe, 0x3808, 0x0a);
    os04a10_write_register(ViPipe, 0x3809, 0x00);
    os04a10_write_register(ViPipe, 0x380a, 0x05);
    os04a10_write_register(ViPipe, 0x380b, 0xa0);

    os04a10_write_register(ViPipe, 0x380c, 0x02); //732 hts
    os04a10_write_register(ViPipe, 0x380d, 0xdc);

    os04a10_write_register(ViPipe, 0x380e, 0x0f); //3898
    os04a10_write_register(ViPipe, 0x380f, 0x3a);

    os04a10_write_register(ViPipe, 0x3811, 0x40);
    os04a10_write_register(ViPipe, 0x3813, 0x28);
    os04a10_write_register(ViPipe, 0x3814, 0x01);
    os04a10_write_register(ViPipe, 0x3815, 0x01);
    os04a10_write_register(ViPipe, 0x3816, 0x01);
    os04a10_write_register(ViPipe, 0x3817, 0x01);
    os04a10_write_register(ViPipe, 0x381c, 0x00);
    os04a10_write_register(ViPipe, 0x3820, 0x02);
    os04a10_write_register(ViPipe, 0x3821, 0x00);
    os04a10_write_register(ViPipe, 0x3822, 0x14);
    os04a10_write_register(ViPipe, 0x3823, 0x18);
    os04a10_write_register(ViPipe, 0x3826, 0x00);
    os04a10_write_register(ViPipe, 0x3827, 0x00);
    os04a10_write_register(ViPipe, 0x3833, 0x40);
    os04a10_write_register(ViPipe, 0x384c, 0x02);
    os04a10_write_register(ViPipe, 0x384d, 0xdc);
    os04a10_write_register(ViPipe, 0x3858, 0x3c);
    os04a10_write_register(ViPipe, 0x3865, 0x02);
    os04a10_write_register(ViPipe, 0x3866, 0x00);
    os04a10_write_register(ViPipe, 0x3867, 0x00);
    os04a10_write_register(ViPipe, 0x3868, 0x02);
    os04a10_write_register(ViPipe, 0x3900, 0x13);
    os04a10_write_register(ViPipe, 0x3940, 0x13);
    os04a10_write_register(ViPipe, 0x3980, 0x13);
    os04a10_write_register(ViPipe, 0x3c01, 0x11);
    os04a10_write_register(ViPipe, 0x3c05, 0x00);
    os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
    os04a10_write_register(ViPipe, 0x3c12, 0x0d);
    os04a10_write_register(ViPipe, 0x3c19, 0x00);
    os04a10_write_register(ViPipe, 0x3c21, 0x00);
    os04a10_write_register(ViPipe, 0x3c3a, 0x10);
    os04a10_write_register(ViPipe, 0x3c3b, 0x18);
    os04a10_write_register(ViPipe, 0x3c3d, 0xc6);
    os04a10_write_register(ViPipe, 0x3c55, 0xcb);
    os04a10_write_register(ViPipe, 0x3c5a, 0x55);
    os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
    os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
    os04a10_write_register(ViPipe, 0x3d8c, 0x70);
    os04a10_write_register(ViPipe, 0x3d8d, 0x10);
    os04a10_write_register(ViPipe, 0x4000, 0xf9);
    os04a10_write_register(ViPipe, 0x4001, 0x2f);
    os04a10_write_register(ViPipe, 0x4004, 0x00);
    os04a10_write_register(ViPipe, 0x4005, 0x40);
    os04a10_write_register(ViPipe, 0x4008, 0x02);
    os04a10_write_register(ViPipe, 0x4009, 0x11);
    os04a10_write_register(ViPipe, 0x400a, 0x06);
    os04a10_write_register(ViPipe, 0x400b, 0x40);
    os04a10_write_register(ViPipe, 0x400e, 0x40);
    os04a10_write_register(ViPipe, 0x402e, 0x00);
    os04a10_write_register(ViPipe, 0x402f, 0x40);
    os04a10_write_register(ViPipe, 0x4030, 0x00);
    os04a10_write_register(ViPipe, 0x4031, 0x40);
    os04a10_write_register(ViPipe, 0x4032, 0x0f);
    os04a10_write_register(ViPipe, 0x4033, 0x80);
    os04a10_write_register(ViPipe, 0x4050, 0x00);
    os04a10_write_register(ViPipe, 0x4051, 0x07);
    os04a10_write_register(ViPipe, 0x4011, 0xbb);
    os04a10_write_register(ViPipe, 0x410f, 0x01);
    os04a10_write_register(ViPipe, 0x4288, 0xcf);
    os04a10_write_register(ViPipe, 0x4289, 0x00);
    os04a10_write_register(ViPipe, 0x428a, 0x46);
    os04a10_write_register(ViPipe, 0x430b, 0x0f);
    os04a10_write_register(ViPipe, 0x430c, 0xfc);
    os04a10_write_register(ViPipe, 0x430d, 0x00);
    os04a10_write_register(ViPipe, 0x430e, 0x00);
    os04a10_write_register(ViPipe, 0x4314, 0x04);
    os04a10_write_register(ViPipe, 0x4500, 0x18);
    os04a10_write_register(ViPipe, 0x4501, 0x18);
    os04a10_write_register(ViPipe, 0x4503, 0x10);
    os04a10_write_register(ViPipe, 0x4504, 0x00);
    os04a10_write_register(ViPipe, 0x4506, 0x32);
    os04a10_write_register(ViPipe, 0x4507, 0x02);
    os04a10_write_register(ViPipe, 0x4601, 0x30);
    os04a10_write_register(ViPipe, 0x4603, 0x00);
    os04a10_write_register(ViPipe, 0x460a, 0x50);
    os04a10_write_register(ViPipe, 0x460c, 0x60);
    os04a10_write_register(ViPipe, 0x4640, 0x62);
    os04a10_write_register(ViPipe, 0x4646, 0xaa);
    os04a10_write_register(ViPipe, 0x4647, 0x55);
    os04a10_write_register(ViPipe, 0x4648, 0x99);
    os04a10_write_register(ViPipe, 0x4649, 0x66);
    os04a10_write_register(ViPipe, 0x464d, 0x00);
    os04a10_write_register(ViPipe, 0x4654, 0x11);
    os04a10_write_register(ViPipe, 0x4655, 0x22);
    os04a10_write_register(ViPipe, 0x4800, 0x44);
    os04a10_write_register(ViPipe, 0x480e, 0x00);
    os04a10_write_register(ViPipe, 0x4810, 0xff);
    os04a10_write_register(ViPipe, 0x4811, 0xff);
    os04a10_write_register(ViPipe, 0x4813, 0x00);
    os04a10_write_register(ViPipe, 0x481f, 0x30);
    os04a10_write_register(ViPipe, 0x4837, 0x0e);
    os04a10_write_register(ViPipe, 0x484b, 0x27);
    os04a10_write_register(ViPipe, 0x4d00, 0x4d);
    os04a10_write_register(ViPipe, 0x4d01, 0x9d);
    os04a10_write_register(ViPipe, 0x4d02, 0xb9);
    os04a10_write_register(ViPipe, 0x4d03, 0x2e);
    os04a10_write_register(ViPipe, 0x4d04, 0x4a);
    os04a10_write_register(ViPipe, 0x4d05, 0x3d);
    os04a10_write_register(ViPipe, 0x4d09, 0x4f);
    os04a10_write_register(ViPipe, 0x5000, 0x1f);
    os04a10_write_register(ViPipe, 0x5001, 0x0d);
    os04a10_write_register(ViPipe, 0x5080, 0x00);
    os04a10_write_register(ViPipe, 0x50c0, 0x00);
    os04a10_write_register(ViPipe, 0x5100, 0x00);
    os04a10_write_register(ViPipe, 0x5200, 0x00);
    os04a10_write_register(ViPipe, 0x5201, 0x00);
    os04a10_write_register(ViPipe, 0x5202, 0x03);
    os04a10_write_register(ViPipe, 0x5203, 0xff);
    os04a10_write_register(ViPipe, 0x5780, 0x53);
    os04a10_write_register(ViPipe, 0x5782, 0x18);
    os04a10_write_register(ViPipe, 0x5783, 0x3c);
    os04a10_write_register(ViPipe, 0x5786, 0x01);
    os04a10_write_register(ViPipe, 0x5788, 0x18);
    os04a10_write_register(ViPipe, 0x5789, 0x3c);
    os04a10_write_register(ViPipe, 0x5792, 0x11);
    os04a10_write_register(ViPipe, 0x5793, 0x33);
    os04a10_write_register(ViPipe, 0x5857, 0xff);
    os04a10_write_register(ViPipe, 0x5858, 0xff);
    os04a10_write_register(ViPipe, 0x5859, 0xff);
    os04a10_write_register(ViPipe, 0x58d7, 0xff);
    os04a10_write_register(ViPipe, 0x58d8, 0xff);
    os04a10_write_register(ViPipe, 0x58d9, 0xff);
    os04a10_write_register(ViPipe, 0x0100, 0x01);
    os04a10_write_register(ViPipe, 0x0100, 0x01);

    ar_always("===new setting OS04A10 4M 25fps 10bit linear Init OK!===\n");
    return;
}

void os04a10_linear_binning_1280x720_25fps_10bit_init(VI_PIPE ViPipe)
{
	
	//OS04A10 4lane 1280x720 RGBBin2 120fps 

	os04a10_write_register(ViPipe, 0x0103, 0x01);
	os04a10_write_register(ViPipe, 0x0109, 0x01);
	os04a10_write_register(ViPipe, 0x0104, 0x02);
	os04a10_write_register(ViPipe, 0x0102, 0x00);
	os04a10_write_register(ViPipe, 0x0305, 0x3c);
	os04a10_write_register(ViPipe, 0x0306, 0x00);
	os04a10_write_register(ViPipe, 0x0307, 0x00);
	os04a10_write_register(ViPipe, 0x0308, 0x04);
	os04a10_write_register(ViPipe, 0x030a, 0x01);
	os04a10_write_register(ViPipe, 0x0317, 0x09);
	os04a10_write_register(ViPipe, 0x0322, 0x01);
	os04a10_write_register(ViPipe, 0x0323, 0x02);
	os04a10_write_register(ViPipe, 0x0324, 0x00);
	os04a10_write_register(ViPipe, 0x0325, 0x90);
	os04a10_write_register(ViPipe, 0x0327, 0x05);
	os04a10_write_register(ViPipe, 0x0329, 0x02);
	os04a10_write_register(ViPipe, 0x032c, 0x02);
	os04a10_write_register(ViPipe, 0x032d, 0x02);
	os04a10_write_register(ViPipe, 0x032e, 0x02);
	os04a10_write_register(ViPipe, 0x300f, 0x11);
	os04a10_write_register(ViPipe, 0x3012, 0x41);
	os04a10_write_register(ViPipe, 0x3026, 0x10);
	os04a10_write_register(ViPipe, 0x3027, 0x08);
	os04a10_write_register(ViPipe, 0x302d, 0x24);
	os04a10_write_register(ViPipe, 0x3104, 0x01);
	os04a10_write_register(ViPipe, 0x3106, 0x11);
	os04a10_write_register(ViPipe, 0x3400, 0x00);
	os04a10_write_register(ViPipe, 0x3408, 0x05);
	os04a10_write_register(ViPipe, 0x340c, 0x0c);
	os04a10_write_register(ViPipe, 0x340d, 0xb0);
	os04a10_write_register(ViPipe, 0x3425, 0x51);
	os04a10_write_register(ViPipe, 0x3426, 0x10);
	os04a10_write_register(ViPipe, 0x3427, 0x14);
	os04a10_write_register(ViPipe, 0x3428, 0x10);
	os04a10_write_register(ViPipe, 0x3429, 0x10);
	os04a10_write_register(ViPipe, 0x342a, 0x10);
	os04a10_write_register(ViPipe, 0x342b, 0x04);
	os04a10_write_register(ViPipe, 0x3501, 0x02);
	os04a10_write_register(ViPipe, 0x3504, 0x08);
	os04a10_write_register(ViPipe, 0x3508, 0x01);
	os04a10_write_register(ViPipe, 0x3509, 0x00);
	os04a10_write_register(ViPipe, 0x350a, 0x01);
	os04a10_write_register(ViPipe, 0x3544, 0x08);
	os04a10_write_register(ViPipe, 0x3548, 0x01);
	os04a10_write_register(ViPipe, 0x3549, 0x00);
	os04a10_write_register(ViPipe, 0x3584, 0x08);
	os04a10_write_register(ViPipe, 0x3588, 0x01);
	os04a10_write_register(ViPipe, 0x3589, 0x00);
	os04a10_write_register(ViPipe, 0x3601, 0x70);
	os04a10_write_register(ViPipe, 0x3604, 0xe3);
	os04a10_write_register(ViPipe, 0x3605, 0x7f);
	os04a10_write_register(ViPipe, 0x3606, 0x80);
	os04a10_write_register(ViPipe, 0x3608, 0xa8);
	os04a10_write_register(ViPipe, 0x360a, 0xd0);
	os04a10_write_register(ViPipe, 0x360b, 0x08);
	os04a10_write_register(ViPipe, 0x360e, 0xc8);
	os04a10_write_register(ViPipe, 0x360f, 0x66);
	os04a10_write_register(ViPipe, 0x3610, 0x89);
	os04a10_write_register(ViPipe, 0x3611, 0x8a);
	os04a10_write_register(ViPipe, 0x3612, 0x4e);
	os04a10_write_register(ViPipe, 0x3613, 0xbd);
	os04a10_write_register(ViPipe, 0x3614, 0x9b);
	os04a10_write_register(ViPipe, 0x362a, 0x0e);
	os04a10_write_register(ViPipe, 0x362b, 0x0e);
	os04a10_write_register(ViPipe, 0x362c, 0x0e);
	os04a10_write_register(ViPipe, 0x362d, 0x0e);
	os04a10_write_register(ViPipe, 0x362e, 0x1a);
	os04a10_write_register(ViPipe, 0x362f, 0x34);
	os04a10_write_register(ViPipe, 0x3630, 0x67);
	os04a10_write_register(ViPipe, 0x3631, 0x7f);
	os04a10_write_register(ViPipe, 0x3638, 0x00);
	os04a10_write_register(ViPipe, 0x3643, 0x00);
	os04a10_write_register(ViPipe, 0x3644, 0x00);
	os04a10_write_register(ViPipe, 0x3645, 0x00);
	os04a10_write_register(ViPipe, 0x3646, 0x00);
	os04a10_write_register(ViPipe, 0x3647, 0x00);
	os04a10_write_register(ViPipe, 0x3648, 0x00);
	os04a10_write_register(ViPipe, 0x3649, 0x00);
	os04a10_write_register(ViPipe, 0x364a, 0x04);
	os04a10_write_register(ViPipe, 0x364c, 0x0e);
	os04a10_write_register(ViPipe, 0x364d, 0x0e);
	os04a10_write_register(ViPipe, 0x364e, 0x0e);
	os04a10_write_register(ViPipe, 0x364f, 0x0e);
	os04a10_write_register(ViPipe, 0x3650, 0xff);
	os04a10_write_register(ViPipe, 0x3651, 0xff);
	os04a10_write_register(ViPipe, 0x365a, 0x00);
	os04a10_write_register(ViPipe, 0x365b, 0x00);
	os04a10_write_register(ViPipe, 0x365c, 0x00);
	os04a10_write_register(ViPipe, 0x365d, 0x00);
	os04a10_write_register(ViPipe, 0x3661, 0x07);
	os04a10_write_register(ViPipe, 0x3662, 0x02);
	os04a10_write_register(ViPipe, 0x3663, 0x20);
	os04a10_write_register(ViPipe, 0x3665, 0x12);
	os04a10_write_register(ViPipe, 0x3667, 0xd4);
	os04a10_write_register(ViPipe, 0x3668, 0x80);
	os04a10_write_register(ViPipe, 0x366c, 0x00);
	os04a10_write_register(ViPipe, 0x366d, 0x00);
	os04a10_write_register(ViPipe, 0x366e, 0x00);
	os04a10_write_register(ViPipe, 0x366f, 0x00);
	os04a10_write_register(ViPipe, 0x3671, 0x08);
	os04a10_write_register(ViPipe, 0x3673, 0x2a);
	os04a10_write_register(ViPipe, 0x3681, 0x80);
	os04a10_write_register(ViPipe, 0x3700, 0x2d);
	os04a10_write_register(ViPipe, 0x3701, 0x22);
	os04a10_write_register(ViPipe, 0x3702, 0x25);
	os04a10_write_register(ViPipe, 0x3703, 0x20);
	os04a10_write_register(ViPipe, 0x3705, 0x00);
	os04a10_write_register(ViPipe, 0x3706, 0x72);
	os04a10_write_register(ViPipe, 0x3707, 0x0a);
	os04a10_write_register(ViPipe, 0x3708, 0x36);
	os04a10_write_register(ViPipe, 0x3709, 0x57);
	os04a10_write_register(ViPipe, 0x370a, 0x01);
	os04a10_write_register(ViPipe, 0x370b, 0x14);
	os04a10_write_register(ViPipe, 0x3714, 0x04);
	os04a10_write_register(ViPipe, 0x3719, 0x1f);
	os04a10_write_register(ViPipe, 0x371b, 0x16);
	os04a10_write_register(ViPipe, 0x371c, 0x00);
	os04a10_write_register(ViPipe, 0x371d, 0x08);
	os04a10_write_register(ViPipe, 0x373f, 0x63);
	os04a10_write_register(ViPipe, 0x3740, 0x63);
	os04a10_write_register(ViPipe, 0x3741, 0x63);
	os04a10_write_register(ViPipe, 0x3742, 0x63);
	os04a10_write_register(ViPipe, 0x3743, 0x01);
	os04a10_write_register(ViPipe, 0x3756, 0x9d);
	os04a10_write_register(ViPipe, 0x3757, 0x9d);
	os04a10_write_register(ViPipe, 0x3762, 0x1c);
	os04a10_write_register(ViPipe, 0x376c, 0x04);
	os04a10_write_register(ViPipe, 0x3776, 0x05);
	os04a10_write_register(ViPipe, 0x3777, 0x22);
	os04a10_write_register(ViPipe, 0x3779, 0x60);
	os04a10_write_register(ViPipe, 0x377c, 0x48);
	os04a10_write_register(ViPipe, 0x3784, 0x06);
	os04a10_write_register(ViPipe, 0x3785, 0x0a);
	os04a10_write_register(ViPipe, 0x3790, 0x10);
	os04a10_write_register(ViPipe, 0x3793, 0x04);
	os04a10_write_register(ViPipe, 0x3794, 0x07);
	os04a10_write_register(ViPipe, 0x3796, 0x00);
	os04a10_write_register(ViPipe, 0x3797, 0x02);
	os04a10_write_register(ViPipe, 0x379c, 0x4d);
	os04a10_write_register(ViPipe, 0x37a1, 0x80);
	os04a10_write_register(ViPipe, 0x37bb, 0x88);
	os04a10_write_register(ViPipe, 0x37be, 0x48);
	os04a10_write_register(ViPipe, 0x37bf, 0x01);
	os04a10_write_register(ViPipe, 0x37c0, 0x01);
	os04a10_write_register(ViPipe, 0x37c4, 0x72);
	os04a10_write_register(ViPipe, 0x37c5, 0x72);
	os04a10_write_register(ViPipe, 0x37c6, 0x72);
	os04a10_write_register(ViPipe, 0x37ca, 0x21);
	os04a10_write_register(ViPipe, 0x37cc, 0x13);
	os04a10_write_register(ViPipe, 0x37cd, 0x90);
	os04a10_write_register(ViPipe, 0x37cf, 0x04);
	os04a10_write_register(ViPipe, 0x37d0, 0x00);
	os04a10_write_register(ViPipe, 0x37d1, 0x72);
	os04a10_write_register(ViPipe, 0x37d2, 0x01);
	os04a10_write_register(ViPipe, 0x37d3, 0x14);
	os04a10_write_register(ViPipe, 0x37d4, 0x00);
	os04a10_write_register(ViPipe, 0x37d5, 0x6c);
	os04a10_write_register(ViPipe, 0x37d6, 0x00);
	os04a10_write_register(ViPipe, 0x37d7, 0xf7);
	os04a10_write_register(ViPipe, 0x37d8, 0x01);
	os04a10_write_register(ViPipe, 0x37dc, 0x00);
	os04a10_write_register(ViPipe, 0x37dd, 0x00);
	os04a10_write_register(ViPipe, 0x37da, 0x00);
	os04a10_write_register(ViPipe, 0x37db, 0x00);
	os04a10_write_register(ViPipe, 0x3800, 0x00);
	os04a10_write_register(ViPipe, 0x3801, 0x40);
	os04a10_write_register(ViPipe, 0x3802, 0x00);
	os04a10_write_register(ViPipe, 0x3803, 0x2c);
	os04a10_write_register(ViPipe, 0x3804, 0x0a);
	os04a10_write_register(ViPipe, 0x3805, 0x4f);
	os04a10_write_register(ViPipe, 0x3806, 0x05);
	os04a10_write_register(ViPipe, 0x3807, 0xd3);
	os04a10_write_register(ViPipe, 0x3808, 0x05);
	os04a10_write_register(ViPipe, 0x3809, 0x00);
	os04a10_write_register(ViPipe, 0x380a, 0x02);
	os04a10_write_register(ViPipe, 0x380b, 0xd0);
	os04a10_write_register(ViPipe, 0x380c, 0x02);
	os04a10_write_register(ViPipe, 0x380d, 0xdc);
	os04a10_write_register(ViPipe, 0x380e, 0x03);
	os04a10_write_register(ViPipe, 0x380f, 0x2c);
	os04a10_write_register(ViPipe, 0x3811, 0x04);
	os04a10_write_register(ViPipe, 0x3813, 0x02);
	os04a10_write_register(ViPipe, 0x3814, 0x03);
	os04a10_write_register(ViPipe, 0x3815, 0x01);
	os04a10_write_register(ViPipe, 0x3816, 0x03);
	os04a10_write_register(ViPipe, 0x3817, 0x01);
	os04a10_write_register(ViPipe, 0x381c, 0x00);
	os04a10_write_register(ViPipe, 0x3820, 0x02);
	os04a10_write_register(ViPipe, 0x3821, 0x09);
	os04a10_write_register(ViPipe, 0x3822, 0x14);
	os04a10_write_register(ViPipe, 0x3823, 0x18);
	os04a10_write_register(ViPipe, 0x3826, 0x00);
	os04a10_write_register(ViPipe, 0x3827, 0x00);
	os04a10_write_register(ViPipe, 0x3833, 0x40);
	os04a10_write_register(ViPipe, 0x384c, 0x02);
	os04a10_write_register(ViPipe, 0x384d, 0xdc);
	os04a10_write_register(ViPipe, 0x3858, 0x3c);
	os04a10_write_register(ViPipe, 0x3865, 0x02);
	os04a10_write_register(ViPipe, 0x3866, 0x00);
	os04a10_write_register(ViPipe, 0x3867, 0x00);
	os04a10_write_register(ViPipe, 0x3868, 0x02);
	os04a10_write_register(ViPipe, 0x3900, 0x13);
	os04a10_write_register(ViPipe, 0x3940, 0x13);
	os04a10_write_register(ViPipe, 0x3980, 0x13);
	os04a10_write_register(ViPipe, 0x3c01, 0x11);
	os04a10_write_register(ViPipe, 0x3c05, 0x00);
	os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
	os04a10_write_register(ViPipe, 0x3c12, 0x0d);
	os04a10_write_register(ViPipe, 0x3c19, 0x00);
	os04a10_write_register(ViPipe, 0x3c21, 0x00);
	os04a10_write_register(ViPipe, 0x3c3a, 0x10);
	os04a10_write_register(ViPipe, 0x3c3b, 0x18);
	os04a10_write_register(ViPipe, 0x3c3d, 0xc6);
	os04a10_write_register(ViPipe, 0x3c55, 0xcb);
	os04a10_write_register(ViPipe, 0x3c5a, 0x55);
	os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
	os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
	os04a10_write_register(ViPipe, 0x3d8c, 0x70);
	os04a10_write_register(ViPipe, 0x3d8d, 0x10);
	os04a10_write_register(ViPipe, 0x4000, 0xf9);
	os04a10_write_register(ViPipe, 0x4001, 0x2f);
	os04a10_write_register(ViPipe, 0x4004, 0x00);
	os04a10_write_register(ViPipe, 0x4005, 0x40);
	os04a10_write_register(ViPipe, 0x4008, 0x02);
	os04a10_write_register(ViPipe, 0x4009, 0x07);
	os04a10_write_register(ViPipe, 0x400a, 0x06);
	os04a10_write_register(ViPipe, 0x400b, 0x40);
	os04a10_write_register(ViPipe, 0x400e, 0x40);
	os04a10_write_register(ViPipe, 0x402e, 0x00);
	os04a10_write_register(ViPipe, 0x402f, 0x40);
	os04a10_write_register(ViPipe, 0x4030, 0x00);
	os04a10_write_register(ViPipe, 0x4031, 0x40);
	os04a10_write_register(ViPipe, 0x4032, 0x0f);
	os04a10_write_register(ViPipe, 0x4033, 0x80);
	os04a10_write_register(ViPipe, 0x4050, 0x00);
	os04a10_write_register(ViPipe, 0x4051, 0x03);
	os04a10_write_register(ViPipe, 0x4011, 0xbb);
	os04a10_write_register(ViPipe, 0x410f, 0x01);
	os04a10_write_register(ViPipe, 0x4288, 0xcf);
	os04a10_write_register(ViPipe, 0x4289, 0x00);
	os04a10_write_register(ViPipe, 0x428a, 0x46);
	os04a10_write_register(ViPipe, 0x430b, 0x0f);
	os04a10_write_register(ViPipe, 0x430c, 0xfc);
	os04a10_write_register(ViPipe, 0x430d, 0x00);
	os04a10_write_register(ViPipe, 0x430e, 0x00);
	os04a10_write_register(ViPipe, 0x4314, 0x04);
	os04a10_write_register(ViPipe, 0x4500, 0x18);
	os04a10_write_register(ViPipe, 0x4501, 0x18);
	os04a10_write_register(ViPipe, 0x4503, 0x10);
	os04a10_write_register(ViPipe, 0x4504, 0x00);
	os04a10_write_register(ViPipe, 0x4506, 0x32);
	os04a10_write_register(ViPipe, 0x4507, 0x02);
	os04a10_write_register(ViPipe, 0x4601, 0x50);
	os04a10_write_register(ViPipe, 0x4603, 0x01);
	os04a10_write_register(ViPipe, 0x460a, 0x50);
	os04a10_write_register(ViPipe, 0x460c, 0x50);
	os04a10_write_register(ViPipe, 0x4640, 0x62);
	os04a10_write_register(ViPipe, 0x4646, 0xaa);
	os04a10_write_register(ViPipe, 0x4647, 0x55);
	os04a10_write_register(ViPipe, 0x4648, 0x99);
	os04a10_write_register(ViPipe, 0x4649, 0x66);
	os04a10_write_register(ViPipe, 0x464d, 0x00);
	os04a10_write_register(ViPipe, 0x4654, 0x11);
	os04a10_write_register(ViPipe, 0x4655, 0x22);
	os04a10_write_register(ViPipe, 0x4800, 0x44);
	os04a10_write_register(ViPipe, 0x480e, 0x00);
	os04a10_write_register(ViPipe, 0x4810, 0xff);
	os04a10_write_register(ViPipe, 0x4811, 0xff);
	os04a10_write_register(ViPipe, 0x4813, 0x00);
	os04a10_write_register(ViPipe, 0x481f, 0x30);
	os04a10_write_register(ViPipe, 0x4837, 0x0e);
	os04a10_write_register(ViPipe, 0x484b, 0x27);
	os04a10_write_register(ViPipe, 0x4d00, 0x4d);
	os04a10_write_register(ViPipe, 0x4d01, 0x9d);
	os04a10_write_register(ViPipe, 0x4d02, 0xb9);
	os04a10_write_register(ViPipe, 0x4d03, 0x2e);
	os04a10_write_register(ViPipe, 0x4d04, 0x4a);
	os04a10_write_register(ViPipe, 0x4d05, 0x3d);
	os04a10_write_register(ViPipe, 0x4d09, 0x4f);
	os04a10_write_register(ViPipe, 0x5000, 0x1f);
	os04a10_write_register(ViPipe, 0x5001, 0x0d);
	os04a10_write_register(ViPipe, 0x5080, 0x00);
	os04a10_write_register(ViPipe, 0x50c0, 0x00);
	os04a10_write_register(ViPipe, 0x5100, 0x00);
	os04a10_write_register(ViPipe, 0x5200, 0x00);
	os04a10_write_register(ViPipe, 0x5201, 0x00);
	os04a10_write_register(ViPipe, 0x5202, 0x03);
	os04a10_write_register(ViPipe, 0x5203, 0xff);
	os04a10_write_register(ViPipe, 0x5780, 0x53);
	os04a10_write_register(ViPipe, 0x5782, 0x18);
	os04a10_write_register(ViPipe, 0x5783, 0x3c);
	os04a10_write_register(ViPipe, 0x5786, 0x01);
	os04a10_write_register(ViPipe, 0x5788, 0x18);
	os04a10_write_register(ViPipe, 0x5789, 0x3c);
	os04a10_write_register(ViPipe, 0x5792, 0x11);
	os04a10_write_register(ViPipe, 0x5793, 0x33);
	os04a10_write_register(ViPipe, 0x5857, 0xff);
	os04a10_write_register(ViPipe, 0x5858, 0xff);
	os04a10_write_register(ViPipe, 0x5859, 0xff);
	os04a10_write_register(ViPipe, 0x58d7, 0xff);
	os04a10_write_register(ViPipe, 0x58d8, 0xff);
	os04a10_write_register(ViPipe, 0x58d9, 0xff);
	os04a10_write_register(ViPipe, 0x0100, 0x01);
	os04a10_write_register(ViPipe, 0x0100, 0x01);
	ar_always("===new setting OS04A10 bining 25fps 10bit init OK!===\n");

}



void os04a10_linear_binning_1280x720_25fps_12bit_init(VI_PIPE ViPipe)
{	
	os04a10_write_register(ViPipe, 0x0103, 0x01);
	os04a10_write_register(ViPipe, 0x0109, 0x01);
	os04a10_write_register(ViPipe, 0x0104, 0x02);
	os04a10_write_register(ViPipe, 0x0102, 0x00);
	os04a10_write_register(ViPipe, 0x0305, 0x6c);
	os04a10_write_register(ViPipe, 0x0306, 0x00);
	os04a10_write_register(ViPipe, 0x0308, 0x05);
	os04a10_write_register(ViPipe, 0x030a, 0x01);
	os04a10_write_register(ViPipe, 0x0317, 0x0a);
	os04a10_write_register(ViPipe, 0x0322, 0x01);
	os04a10_write_register(ViPipe, 0x0323, 0x02);
	os04a10_write_register(ViPipe, 0x0324, 0x00);
	os04a10_write_register(ViPipe, 0x0325, 0xd8);
	os04a10_write_register(ViPipe, 0x0327, 0x05);
	os04a10_write_register(ViPipe, 0x0329, 0x01);
	os04a10_write_register(ViPipe, 0x032c, 0x02);
	os04a10_write_register(ViPipe, 0x300f, 0x11);
	os04a10_write_register(ViPipe, 0x3012, 0x41);
	os04a10_write_register(ViPipe, 0x3026, 0x10);
	os04a10_write_register(ViPipe, 0x3027, 0x08);
	os04a10_write_register(ViPipe, 0x302d, 0x24);
	os04a10_write_register(ViPipe, 0x3106, 0x01);
	os04a10_write_register(ViPipe, 0x3400, 0x00);
	os04a10_write_register(ViPipe, 0x3408, 0x05);
	os04a10_write_register(ViPipe, 0x340c, 0x0c);
	os04a10_write_register(ViPipe, 0x340d, 0xb0);
	os04a10_write_register(ViPipe, 0x3425, 0x51);
	os04a10_write_register(ViPipe, 0x3426, 0x10);
	os04a10_write_register(ViPipe, 0x3427, 0x14);
	os04a10_write_register(ViPipe, 0x3428, 0x10);
	os04a10_write_register(ViPipe, 0x3429, 0x10);
	os04a10_write_register(ViPipe, 0x342a, 0x10);
	os04a10_write_register(ViPipe, 0x342b, 0x04);
	os04a10_write_register(ViPipe, 0x3501, 0x02);
	os04a10_write_register(ViPipe, 0x3504, 0x08);
	os04a10_write_register(ViPipe, 0x3508, 0x01);
	os04a10_write_register(ViPipe, 0x3509, 0x00);
	os04a10_write_register(ViPipe, 0x350a, 0x01);
	os04a10_write_register(ViPipe, 0x3542, 0x10);
	os04a10_write_register(ViPipe, 0x3544, 0x08);
	os04a10_write_register(ViPipe, 0x3548, 0x01);
	os04a10_write_register(ViPipe, 0x3549, 0x00);
	os04a10_write_register(ViPipe, 0x3582, 0x02);
	os04a10_write_register(ViPipe, 0x3584, 0x08);
	os04a10_write_register(ViPipe, 0x3588, 0x01);
	os04a10_write_register(ViPipe, 0x3589, 0x00);
	os04a10_write_register(ViPipe, 0x3601, 0x70);
	os04a10_write_register(ViPipe, 0x3604, 0xe3);
	os04a10_write_register(ViPipe, 0x3603, 0xe5);
	os04a10_write_register(ViPipe, 0x3605, 0xff);
	os04a10_write_register(ViPipe, 0x3606, 0x01);
	os04a10_write_register(ViPipe, 0x3608, 0xa8);
	os04a10_write_register(ViPipe, 0x360a, 0xd0);
	os04a10_write_register(ViPipe, 0x360b, 0x18);
	os04a10_write_register(ViPipe, 0x360e, 0xc8);
	os04a10_write_register(ViPipe, 0x360f, 0x66);
	os04a10_write_register(ViPipe, 0x3610, 0x89);
	os04a10_write_register(ViPipe, 0x3611, 0x8a);
	os04a10_write_register(ViPipe, 0x3612, 0x4e);
	os04a10_write_register(ViPipe, 0x3613, 0xbd);
	os04a10_write_register(ViPipe, 0x3614, 0x9b);
	os04a10_write_register(ViPipe, 0x3623, 0x08);
	os04a10_write_register(ViPipe, 0x362a, 0x0e);
	os04a10_write_register(ViPipe, 0x362b, 0x0e);
	os04a10_write_register(ViPipe, 0x362c, 0x0e);
	os04a10_write_register(ViPipe, 0x362d, 0x09);
	os04a10_write_register(ViPipe, 0x362e, 0x07);
	os04a10_write_register(ViPipe, 0x362f, 0x0f);
	os04a10_write_register(ViPipe, 0x3630, 0x1f);
	os04a10_write_register(ViPipe, 0x3631, 0x40);
	os04a10_write_register(ViPipe, 0x3638, 0x00);
	os04a10_write_register(ViPipe, 0x3643, 0x00);
	os04a10_write_register(ViPipe, 0x3644, 0x00);
	os04a10_write_register(ViPipe, 0x3645, 0x00);
	os04a10_write_register(ViPipe, 0x3646, 0x00);
	os04a10_write_register(ViPipe, 0x3647, 0x00);
	os04a10_write_register(ViPipe, 0x3648, 0x00);
	os04a10_write_register(ViPipe, 0x3649, 0x00);
	os04a10_write_register(ViPipe, 0x364a, 0x04);
	os04a10_write_register(ViPipe, 0x364c, 0x0e);
	os04a10_write_register(ViPipe, 0x364d, 0x0e);
	os04a10_write_register(ViPipe, 0x364e, 0x0e);
	os04a10_write_register(ViPipe, 0x364f, 0x0e);
	os04a10_write_register(ViPipe, 0x3650, 0xff);
	os04a10_write_register(ViPipe, 0x3651, 0xff);
	os04a10_write_register(ViPipe, 0x365a, 0x00);
	os04a10_write_register(ViPipe, 0x365b, 0x00);
	os04a10_write_register(ViPipe, 0x365c, 0x00);
	os04a10_write_register(ViPipe, 0x365d, 0x00);
	os04a10_write_register(ViPipe, 0x3661, 0x07);
	os04a10_write_register(ViPipe, 0x3662, 0x00);
	os04a10_write_register(ViPipe, 0x3663, 0x20);
	os04a10_write_register(ViPipe, 0x3665, 0x12);
	os04a10_write_register(ViPipe, 0x3667, 0xd4);
	os04a10_write_register(ViPipe, 0x3668, 0x80);
	os04a10_write_register(ViPipe, 0x366c, 0x00);
	os04a10_write_register(ViPipe, 0x366d, 0x00);
	os04a10_write_register(ViPipe, 0x366e, 0x00);
	os04a10_write_register(ViPipe, 0x366f, 0x00);
	os04a10_write_register(ViPipe, 0x3671, 0x08);
	os04a10_write_register(ViPipe, 0x3673, 0x2a);
	os04a10_write_register(ViPipe, 0x3681, 0x80);
	os04a10_write_register(ViPipe, 0x3700, 0x2d);
	os04a10_write_register(ViPipe, 0x3701, 0x22);
	os04a10_write_register(ViPipe, 0x3702, 0x25);
	os04a10_write_register(ViPipe, 0x3703, 0x28);
	os04a10_write_register(ViPipe, 0x3705, 0x00);
	os04a10_write_register(ViPipe, 0x3706, 0xa8);
	os04a10_write_register(ViPipe, 0x3707, 0x0a);
	os04a10_write_register(ViPipe, 0x3708, 0x36);
	os04a10_write_register(ViPipe, 0x3709, 0x57);
	os04a10_write_register(ViPipe, 0x370a, 0x02);
	os04a10_write_register(ViPipe, 0x370b, 0x18);
	os04a10_write_register(ViPipe, 0x3714, 0x04);
	os04a10_write_register(ViPipe, 0x371b, 0x16);
	os04a10_write_register(ViPipe, 0x371c, 0x00);
	os04a10_write_register(ViPipe, 0x371d, 0x08);
	os04a10_write_register(ViPipe, 0x373f, 0x4c);
	os04a10_write_register(ViPipe, 0x3740, 0x4c);
	os04a10_write_register(ViPipe, 0x3741, 0x4c);
	os04a10_write_register(ViPipe, 0x3742, 0x63);
	os04a10_write_register(ViPipe, 0x3756, 0xe6);
	os04a10_write_register(ViPipe, 0x3757, 0xe6);
	os04a10_write_register(ViPipe, 0x3762, 0x1d);
	os04a10_write_register(ViPipe, 0x376c, 0x00);
	os04a10_write_register(ViPipe, 0x3776, 0x05);
	os04a10_write_register(ViPipe, 0x3777, 0x22);
	os04a10_write_register(ViPipe, 0x3779, 0x60);
	os04a10_write_register(ViPipe, 0x377c, 0x48);
	os04a10_write_register(ViPipe, 0x3793, 0x04);
	os04a10_write_register(ViPipe, 0x3794, 0x07);
	os04a10_write_register(ViPipe, 0x379c, 0x4d);
	os04a10_write_register(ViPipe, 0x3784, 0x06);
	os04a10_write_register(ViPipe, 0x3785, 0x0a);
	os04a10_write_register(ViPipe, 0x37c4, 0x72);
	os04a10_write_register(ViPipe, 0x37c5, 0x72);
	os04a10_write_register(ViPipe, 0x37c6, 0x72);
	os04a10_write_register(ViPipe, 0x37d0, 0x00);
	os04a10_write_register(ViPipe, 0x37d1, 0xa8);
	os04a10_write_register(ViPipe, 0x37d2, 0x02);
	os04a10_write_register(ViPipe, 0x37d3, 0x18);
	os04a10_write_register(ViPipe, 0x37d4, 0x00);
	os04a10_write_register(ViPipe, 0x37d5, 0x6c);
	os04a10_write_register(ViPipe, 0x37d6, 0x00);
	os04a10_write_register(ViPipe, 0x37d7, 0xf7);
	os04a10_write_register(ViPipe, 0x37d8, 0x01);
	os04a10_write_register(ViPipe, 0x37da, 0x00);
	os04a10_write_register(ViPipe, 0x37db, 0x00);
	os04a10_write_register(ViPipe, 0x37dc, 0x00);
	os04a10_write_register(ViPipe, 0x37dd, 0x00);
	os04a10_write_register(ViPipe, 0x3790, 0x10);
	os04a10_write_register(ViPipe, 0x3793, 0x04);
	os04a10_write_register(ViPipe, 0x3794, 0x07);
	os04a10_write_register(ViPipe, 0x3796, 0x00);
	os04a10_write_register(ViPipe, 0x3797, 0x02);
	os04a10_write_register(ViPipe, 0x37a1, 0x80);
	os04a10_write_register(ViPipe, 0x37bb, 0x88);
	os04a10_write_register(ViPipe, 0x37bd, 0x01);
	os04a10_write_register(ViPipe, 0x37be, 0x48);
	os04a10_write_register(ViPipe, 0x37bf, 0x01);
	os04a10_write_register(ViPipe, 0x37c0, 0x01);
	os04a10_write_register(ViPipe, 0x37ca, 0x21);
	os04a10_write_register(ViPipe, 0x37cc, 0x13);
	os04a10_write_register(ViPipe, 0x37cd, 0x90);
	os04a10_write_register(ViPipe, 0x37cf, 0x04);
	os04a10_write_register(ViPipe, 0x37ec, 0x00);
	os04a10_write_register(ViPipe, 0x37ed, 0x00);
	os04a10_write_register(ViPipe, 0x3800, 0x00);
	os04a10_write_register(ViPipe, 0x3801, 0x00);
	os04a10_write_register(ViPipe, 0x3802, 0x00);
	os04a10_write_register(ViPipe, 0x3803, 0x00);
	os04a10_write_register(ViPipe, 0x3804, 0x0a);
	os04a10_write_register(ViPipe, 0x3805, 0x0f);
	os04a10_write_register(ViPipe, 0x3806, 0x05);
	os04a10_write_register(ViPipe, 0x3807, 0xaf);
	os04a10_write_register(ViPipe, 0x3808, 0x05);
	os04a10_write_register(ViPipe, 0x3809, 0x00);
	os04a10_write_register(ViPipe, 0x380a, 0x02);
	os04a10_write_register(ViPipe, 0x380b, 0xd0);
	os04a10_write_register(ViPipe, 0x380c, 0x05);
	os04a10_write_register(ViPipe, 0x380d, 0xc4);
	os04a10_write_register(ViPipe, 0x380e, 0x03);
	os04a10_write_register(ViPipe, 0x380f, 0x2c);
	os04a10_write_register(ViPipe, 0x3811, 0x04);
	os04a10_write_register(ViPipe, 0x3813, 0x04);
	os04a10_write_register(ViPipe, 0x3814, 0x03);
	os04a10_write_register(ViPipe, 0x3815, 0x01);
	os04a10_write_register(ViPipe, 0x3816, 0x03);
	os04a10_write_register(ViPipe, 0x3817, 0x01);
	os04a10_write_register(ViPipe, 0x381c, 0x00);
	os04a10_write_register(ViPipe, 0x3820, 0x02);
	os04a10_write_register(ViPipe, 0x3821, 0x09);
	os04a10_write_register(ViPipe, 0x3822, 0x14);
	os04a10_write_register(ViPipe, 0x3823, 0x18);
	os04a10_write_register(ViPipe, 0x3826, 0x00);
	os04a10_write_register(ViPipe, 0x3827, 0x00);
	os04a10_write_register(ViPipe, 0x3833, 0x40);
	os04a10_write_register(ViPipe, 0x384c, 0x05);
	os04a10_write_register(ViPipe, 0x384d, 0xc4);
	os04a10_write_register(ViPipe, 0x3858, 0x3c);
	os04a10_write_register(ViPipe, 0x3865, 0x02);
	os04a10_write_register(ViPipe, 0x3866, 0x00);
	os04a10_write_register(ViPipe, 0x3867, 0x00);
	os04a10_write_register(ViPipe, 0x3868, 0x02);
	os04a10_write_register(ViPipe, 0x3900, 0x13);
	os04a10_write_register(ViPipe, 0x3940, 0x13);
	os04a10_write_register(ViPipe, 0x3980, 0x13);
	os04a10_write_register(ViPipe, 0x3c01, 0x11);
	os04a10_write_register(ViPipe, 0x3c05, 0x00);
	os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
	os04a10_write_register(ViPipe, 0x3c12, 0x0d);
	os04a10_write_register(ViPipe, 0x3c19, 0x00);
	os04a10_write_register(ViPipe, 0x3c21, 0x00);
	os04a10_write_register(ViPipe, 0x3c3a, 0x50);
	os04a10_write_register(ViPipe, 0x3c3b, 0x18);
	os04a10_write_register(ViPipe, 0x3c3d, 0xc6);
	os04a10_write_register(ViPipe, 0x3c55, 0xcb);
	os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
	os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
	os04a10_write_register(ViPipe, 0x3ce0, 0x00);
	os04a10_write_register(ViPipe, 0x3ce1, 0x00);
	os04a10_write_register(ViPipe, 0x3ce2, 0x00);
	os04a10_write_register(ViPipe, 0x3ce3, 0x00);
	os04a10_write_register(ViPipe, 0x3d8c, 0x70);
	os04a10_write_register(ViPipe, 0x3d8d, 0x10);
	os04a10_write_register(ViPipe, 0x4001, 0x2f);
	os04a10_write_register(ViPipe, 0x4004, 0x01);
	os04a10_write_register(ViPipe, 0x4005, 0x00);
	os04a10_write_register(ViPipe, 0x4008, 0x02);
	os04a10_write_register(ViPipe, 0x4009, 0x07);
	os04a10_write_register(ViPipe, 0x400a, 0x03);
	os04a10_write_register(ViPipe, 0x400b, 0x3f);
	os04a10_write_register(ViPipe, 0x400e, 0x40);
	os04a10_write_register(ViPipe, 0x4011, 0xbb);
	os04a10_write_register(ViPipe, 0x402e, 0x01);
	os04a10_write_register(ViPipe, 0x402f, 0x00);
	os04a10_write_register(ViPipe, 0x4030, 0x01);
	os04a10_write_register(ViPipe, 0x4031, 0x00);
	os04a10_write_register(ViPipe, 0x4032, 0x9f);
	os04a10_write_register(ViPipe, 0x4033, 0x80);
	os04a10_write_register(ViPipe, 0x4050, 0x00);
	os04a10_write_register(ViPipe, 0x4051, 0x03);
	os04a10_write_register(ViPipe, 0x405e, 0x20);
	os04a10_write_register(ViPipe, 0x410f, 0x01);
	os04a10_write_register(ViPipe, 0x4288, 0xcf);
	os04a10_write_register(ViPipe, 0x4289, 0x00);
	os04a10_write_register(ViPipe, 0x428a, 0x46);
	os04a10_write_register(ViPipe, 0x430b, 0xff);
	os04a10_write_register(ViPipe, 0x430c, 0xff);
	os04a10_write_register(ViPipe, 0x430d, 0x00);
	os04a10_write_register(ViPipe, 0x430e, 0x00);
	os04a10_write_register(ViPipe, 0x4314, 0x04);
	os04a10_write_register(ViPipe, 0x4500, 0x18);
	os04a10_write_register(ViPipe, 0x4501, 0x18);
	os04a10_write_register(ViPipe, 0x4503, 0x10);
	os04a10_write_register(ViPipe, 0x4504, 0x00);
	os04a10_write_register(ViPipe, 0x4506, 0x32);
	os04a10_write_register(ViPipe, 0x4507, 0x02);
	os04a10_write_register(ViPipe, 0x4601, 0x30);
	os04a10_write_register(ViPipe, 0x4603, 0x00);
	os04a10_write_register(ViPipe, 0x460a, 0x50);
	os04a10_write_register(ViPipe, 0x460c, 0x60);
	os04a10_write_register(ViPipe, 0x4640, 0x62);
	os04a10_write_register(ViPipe, 0x4646, 0xaa);
	os04a10_write_register(ViPipe, 0x4647, 0x55);
	os04a10_write_register(ViPipe, 0x4648, 0x99);
	os04a10_write_register(ViPipe, 0x4649, 0x66);
	os04a10_write_register(ViPipe, 0x464d, 0x00);
	os04a10_write_register(ViPipe, 0x4654, 0x11);
	os04a10_write_register(ViPipe, 0x4655, 0x22);
	os04a10_write_register(ViPipe, 0x4800, 0x04);
	os04a10_write_register(ViPipe, 0x480e, 0x00);
	os04a10_write_register(ViPipe, 0x4810, 0xff);
	os04a10_write_register(ViPipe, 0x4811, 0xff);
	os04a10_write_register(ViPipe, 0x4813, 0x00);
	os04a10_write_register(ViPipe, 0x481f, 0x30);
	os04a10_write_register(ViPipe, 0x4837, 0x0c);
	os04a10_write_register(ViPipe, 0x484b, 0x27);
	os04a10_write_register(ViPipe, 0x4d00, 0x4d);
	os04a10_write_register(ViPipe, 0x4d01, 0x9d);
	os04a10_write_register(ViPipe, 0x4d02, 0xb9);
	os04a10_write_register(ViPipe, 0x4d03, 0x2e);
	os04a10_write_register(ViPipe, 0x4d04, 0x4a);
	os04a10_write_register(ViPipe, 0x4d05, 0x3d);
	os04a10_write_register(ViPipe, 0x4d09, 0x4f);
	os04a10_write_register(ViPipe, 0x5000, 0x1f);
	os04a10_write_register(ViPipe, 0x5001, 0x0d);
	os04a10_write_register(ViPipe, 0x5080, 0x00);
	os04a10_write_register(ViPipe, 0x50c0, 0x00);
	os04a10_write_register(ViPipe, 0x5100, 0x00);
	os04a10_write_register(ViPipe, 0x5200, 0x00);
	os04a10_write_register(ViPipe, 0x5201, 0x00);
	os04a10_write_register(ViPipe, 0x5202, 0x03);
	os04a10_write_register(ViPipe, 0x5203, 0xff);
	os04a10_write_register(ViPipe, 0x5780, 0x53);
	os04a10_write_register(ViPipe, 0x5782, 0x60);
	os04a10_write_register(ViPipe, 0x5783, 0xf0);
	os04a10_write_register(ViPipe, 0x5786, 0x01);
	os04a10_write_register(ViPipe, 0x5788, 0x60);
	os04a10_write_register(ViPipe, 0x5789, 0xf0);
	os04a10_write_register(ViPipe, 0x5792, 0x11);
	os04a10_write_register(ViPipe, 0x5793, 0x33);
	os04a10_write_register(ViPipe, 0x5857, 0x00);
	os04a10_write_register(ViPipe, 0x5858, 0x00);
	os04a10_write_register(ViPipe, 0x5859, 0x00);
	os04a10_write_register(ViPipe, 0x58d7, 0x00);
	os04a10_write_register(ViPipe, 0x58d8, 0x00);
	os04a10_write_register(ViPipe, 0x58d9, 0x00);
	os04a10_write_register(ViPipe, 0x0100, 0x01);
	
	ar_always("===new setting OS04A10 bining 25fps 12bit init OK!===\n");

}

