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

const unsigned char imx415_i2c_addr     =    0x34;        /* I2C Address of IMX415 */
const unsigned int  imx415_addr_byte    =    2;
const unsigned int  imx415_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastImx415[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunImx415BusInfo[];

int imx415_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunImx415BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	ar_always("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx415_i2c_addr >> 1));
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

int imx415_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int imx415_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int imx415_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = imx415_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = imx415_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = imx415_data_byte;

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

    if (imx415_addr_byte == 2)
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

    if (imx415_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, imx415_addr_byte + imx415_data_byte);
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

void imx415_prog(VI_PIPE ViPipe, int *rom)
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
            imx415_write_register(ViPipe, addr, data);
        }
    }
}

void imx415_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void imx415_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define IMX415_FLIP_MIRRO (0x3030)

int imx415_flip_off_mirro_off(VI_PIPE ViPipe)
{
    imx415_write_register(ViPipe, IMX415_FLIP_MIRRO, 0x00);
    return 0;
}

int imx415_flip_on_mirro_off(VI_PIPE ViPipe)
{
    imx415_write_register(ViPipe, IMX415_FLIP_MIRRO, 0x02);
    return 0;
}

int imx415_flip_off_mirro_on(VI_PIPE ViPipe)
{
    imx415_write_register(ViPipe, IMX415_FLIP_MIRRO, 0x01);
    return 0;
}

int imx415_flip_on_mirro_on(VI_PIPE ViPipe)
{
    imx415_write_register(ViPipe, IMX415_FLIP_MIRRO, 0x03);
    return 0;
}

#define IMX415_SENSOR_4K_30FPS_LINEAR_MODE      (1)
#define IMX415_SENSOR_4K_60FPS_LINEAR_MODE      (2)
#define SONY_IMX415_MIPI_HIGH_SPEED_12BIT		(3)	

// #define IMX415_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
// #define IMX415_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
// #define IMX415_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)

// void imx415_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
// void imx415_wdr_1080p60_2to1_init(VI_PIPE ViPipe);
// void imx415_wdr_1080p120_2to1_init(VI_PIPE ViPipe);
// void imx415_wdr_720p60_2to1_init(VI_PIPE ViPipe);
// void imx415_wdr_1080p30_3to1_init(VI_PIPE ViPipe);
// void imx415_wdr_1080p120_3to1_init(VI_PIPE ViPipe);
// void imx415_wdr_720p60_3to1_init(VI_PIPE ViPipe);
void imx415_linear_4kp30_init(VI_PIPE ViPipe);
void imx415_linear_4kp60_init(VI_PIPE ViPipe);
void imx415_linear_4Mp30_init(VI_PIPE ViPipe);
void imx415_wdr2to1_4M10P_init(VI_PIPE ViPipe);


void imx415_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastImx415[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            imx415_write_register(ViPipe, g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void imx415_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastImx415[ViPipe]->bInit;
    enWDRMode   = g_pastImx415[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastImx415[ViPipe]->u8ImgMode;

    imx415_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            imx415_wdr2to1_4M10P_init(ViPipe);

        }else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            // imx415_wdr_1080p120_3to1_init(ViPipe);

        }
        else
        {
            if (IMX415_SENSOR_4K_30FPS_LINEAR_MODE == u8ImgMode)
            {
                imx415_linear_4kp30_init(ViPipe);

            }else if (IMX415_SENSOR_4K_60FPS_LINEAR_MODE == u8ImgMode)
            {
                imx415_linear_4kp60_init(ViPipe);
            }
			else
			{
				imx415_linear_4Mp30_init(ViPipe);
			}

			
        }

    }else /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {

        }else if (WDR_MODE_3To1_LINE == enWDRMode)
        {

        }else
        {
            if (IMX415_SENSOR_4K_30FPS_LINEAR_MODE == u8ImgMode)
            {
                imx415_linear_4kp30_init(ViPipe);

            }else if (IMX415_SENSOR_4K_60FPS_LINEAR_MODE == u8ImgMode)
            {
                imx415_linear_4kp60_init(ViPipe);
            }
        }
    }

    // for (i = 0; i < g_pastImx415[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     imx415_write_register(ViPipe, g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx415[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastImx415[ViPipe]->bInit = AR_TRUE;
    return ;
}

void imx415_trigger(VI_PIPE ViPipe, AR_S32 s32Frames)
{
    imx415_write_register (ViPipe, 0x3000, 0x00); /* standby */	
    imx415_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx415 trigger stream on!=====\n");
}

void imx415_exit(VI_PIPE ViPipe)
{
	imx415_write_register (ViPipe, 0x3000, 0x01); /* standby */	
    imx415_write_register (ViPipe, 0x3002, 0x01); /* master mode start */
    //imx415_i2c_exit(ViPipe);
	ar_always("===Imx415 trigger stream off!=====\n");
    return;
}


/* 4kP30 */
void imx415_linear_4kp30_init(VI_PIPE ViPipe)
{
	/*
	  IMX415-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 891Mbps Master Mode 30fps Integration Time 33.277ms
	  Tool ver : Ver6.0
	*/    imx415_write_register(ViPipe, 0x3008, 0x7F);
    imx415_write_register(ViPipe, 0x300A, 0x5B);
	
    imx415_write_register(ViPipe, 0x3024, 0xCA);
    imx415_write_register(ViPipe, 0x3025, 0x08);
	
    imx415_write_register(ViPipe, 0x3028, 0x4C);
    imx415_write_register(ViPipe, 0x3029, 0x04);
    imx415_write_register(ViPipe, 0x3033, 0x05);
    imx415_write_register(ViPipe, 0x3050, 0x08);

    imx415_write_register(ViPipe, 0x30C1, 0x00);

    // normal exp/gain mod
    imx415_write_register(ViPipe, 0x302C, 0x00);
    imx415_write_register(ViPipe, 0x302D, 0x00);
    imx415_write_register(ViPipe, 0x3260, 0x01);

    imx415_write_register(ViPipe, 0x3116, 0x24);
    imx415_write_register(ViPipe, 0x311E, 0x24);
    imx415_write_register(ViPipe, 0x32D4, 0x21);
    imx415_write_register(ViPipe, 0x32EC, 0xA1);
    imx415_write_register(ViPipe, 0x3452, 0x7F);
    imx415_write_register(ViPipe, 0x3453, 0x03);
    imx415_write_register(ViPipe, 0x358A, 0x04);
    imx415_write_register(ViPipe, 0x35A1, 0x02);
    imx415_write_register(ViPipe, 0x36BC, 0x0C);
    imx415_write_register(ViPipe, 0x36CC, 0x53);
    imx415_write_register(ViPipe, 0x36CD, 0x00);
    imx415_write_register(ViPipe, 0x36CE, 0x3C);
    imx415_write_register(ViPipe, 0x36D0, 0x8C);
    imx415_write_register(ViPipe, 0x36D1, 0x00);
    imx415_write_register(ViPipe, 0x36D2, 0x71);
    imx415_write_register(ViPipe, 0x36D4, 0x3C);
    imx415_write_register(ViPipe, 0x36D6, 0x53);
    imx415_write_register(ViPipe, 0x36D7, 0x00);
    imx415_write_register(ViPipe, 0x36D8, 0x71);
    imx415_write_register(ViPipe, 0x36DA, 0x8C);
    imx415_write_register(ViPipe, 0x36DB, 0x00);
    imx415_write_register(ViPipe, 0x3724, 0x02);
    imx415_write_register(ViPipe, 0x3726, 0x02);
    imx415_write_register(ViPipe, 0x3732, 0x02);
    imx415_write_register(ViPipe, 0x3734, 0x03);
    imx415_write_register(ViPipe, 0x3736, 0x03);
    imx415_write_register(ViPipe, 0x3742, 0x03);
    imx415_write_register(ViPipe, 0x3862, 0xE0);
    imx415_write_register(ViPipe, 0x38CC, 0x30);
    imx415_write_register(ViPipe, 0x38CD, 0x2F);
    imx415_write_register(ViPipe, 0x395C, 0x0C);
    imx415_write_register(ViPipe, 0x3A42, 0xD1);
    imx415_write_register(ViPipe, 0x3A4C, 0x77);
    imx415_write_register(ViPipe, 0x3AE0, 0x02);
    imx415_write_register(ViPipe, 0x3AEC, 0x0C);
    imx415_write_register(ViPipe, 0x3B00, 0x2E);
    imx415_write_register(ViPipe, 0x3B06, 0x29);
    imx415_write_register(ViPipe, 0x3B98, 0x25);
    imx415_write_register(ViPipe, 0x3B99, 0x21);
    imx415_write_register(ViPipe, 0x3B9B, 0x13);
    imx415_write_register(ViPipe, 0x3B9C, 0x13);
    imx415_write_register(ViPipe, 0x3B9D, 0x13);
    imx415_write_register(ViPipe, 0x3B9E, 0x13);
    imx415_write_register(ViPipe, 0x3BA1, 0x00);
    imx415_write_register(ViPipe, 0x3BA2, 0x06);
    imx415_write_register(ViPipe, 0x3BA3, 0x0B);
    imx415_write_register(ViPipe, 0x3BA4, 0x10);
    imx415_write_register(ViPipe, 0x3BA5, 0x14);
    imx415_write_register(ViPipe, 0x3BA6, 0x18);
    imx415_write_register(ViPipe, 0x3BA7, 0x1A);
    imx415_write_register(ViPipe, 0x3BA8, 0x1A);
    imx415_write_register(ViPipe, 0x3BA9, 0x1A);
    imx415_write_register(ViPipe, 0x3BAC, 0xED);
    imx415_write_register(ViPipe, 0x3BAD, 0x01);
    imx415_write_register(ViPipe, 0x3BAE, 0xF6);
    imx415_write_register(ViPipe, 0x3BAF, 0x02);
    imx415_write_register(ViPipe, 0x3BB0, 0xA2);
    imx415_write_register(ViPipe, 0x3BB1, 0x03);
    imx415_write_register(ViPipe, 0x3BB2, 0xE0);
    imx415_write_register(ViPipe, 0x3BB3, 0x03);
    imx415_write_register(ViPipe, 0x3BB4, 0xE0);
    imx415_write_register(ViPipe, 0x3BB5, 0x03);
    imx415_write_register(ViPipe, 0x3BB6, 0xE0);
    imx415_write_register(ViPipe, 0x3BB7, 0x03);
    imx415_write_register(ViPipe, 0x3BB8, 0xE0);
    imx415_write_register(ViPipe, 0x3BBA, 0xE0);
    imx415_write_register(ViPipe, 0x3BBC, 0xDA);
    imx415_write_register(ViPipe, 0x3BBE, 0x88);
    imx415_write_register(ViPipe, 0x3BC0, 0x44);
    imx415_write_register(ViPipe, 0x3BC2, 0x7B);
    imx415_write_register(ViPipe, 0x3BC4, 0xA2);
    imx415_write_register(ViPipe, 0x3BC8, 0xBD);
    imx415_write_register(ViPipe, 0x3BCA, 0xBD);

    imx415_write_register(ViPipe, 0x4004, 0x48);
    imx415_write_register(ViPipe, 0x4005, 0x09);
    imx415_write_register(ViPipe, 0x400C, 0x00);
    imx415_write_register(ViPipe, 0x4018, 0x7F);
    imx415_write_register(ViPipe, 0x401A, 0x37);
    imx415_write_register(ViPipe, 0x401C, 0x37);
    imx415_write_register(ViPipe, 0x401E, 0xF7);
    imx415_write_register(ViPipe, 0x401F, 0x00);
    imx415_write_register(ViPipe, 0x4020, 0x3F);
    imx415_write_register(ViPipe, 0x4022, 0x6F);
    imx415_write_register(ViPipe, 0x4024, 0x3F);
    imx415_write_register(ViPipe, 0x4026, 0x5F);
    imx415_write_register(ViPipe, 0x4028, 0x2F);
    imx415_write_register(ViPipe, 0x4074, 0x01);
    //imx415_default_reg_init(ViPipe);
    imx415_write_register(ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx415_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
    //imx415_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ar_always("===IMX415 4K 30fps 12bit LINE Init OK!===\n");
    return;
}


/* 4kP60 */
void imx415_linear_4kp60_init(VI_PIPE ViPipe)
{
	/*
	  IMX415-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 1782Mbps Master Mode 60fps Integration Time 33.277ms
	  Tool ver : Ver6.0
	*/
	imx415_write_register(ViPipe, 0x3008, 0x7F);
	imx415_write_register(ViPipe, 0x300A, 0x5B);

	imx415_write_register(ViPipe, 0x3024, 0xca);  //vmax
	imx415_write_register(ViPipe, 0x3025, 0x08);


	imx415_write_register(ViPipe, 0x3028, 0x26);	//hmax
	imx415_write_register(ViPipe, 0x3029, 0x02);

	imx415_write_register(ViPipe, 0x3050, 0x08);
	imx415_write_register(ViPipe, 0x30C1, 0x00);
	imx415_write_register(ViPipe, 0x3116, 0x24);
	imx415_write_register(ViPipe, 0x311E, 0x24);
	imx415_write_register(ViPipe, 0x32D4, 0x21);
	imx415_write_register(ViPipe, 0x32EC, 0xA1);
	imx415_write_register(ViPipe, 0x3452, 0x7F);
	imx415_write_register(ViPipe, 0x3453, 0x03);
	imx415_write_register(ViPipe, 0x358A, 0x04);
	imx415_write_register(ViPipe, 0x35A1, 0x02);
	imx415_write_register(ViPipe, 0x36BC, 0x0C);
	imx415_write_register(ViPipe, 0x36CC, 0x53);
	imx415_write_register(ViPipe, 0x36CD, 0x00);
	imx415_write_register(ViPipe, 0x36CE, 0x3C);
	imx415_write_register(ViPipe, 0x36D0, 0x8C);
	imx415_write_register(ViPipe, 0x36D1, 0x00);
	imx415_write_register(ViPipe, 0x36D2, 0x71);
	imx415_write_register(ViPipe, 0x36D4, 0x3C);
	imx415_write_register(ViPipe, 0x36D6, 0x53);
	imx415_write_register(ViPipe, 0x36D7, 0x00);
	imx415_write_register(ViPipe, 0x36D8, 0x71);
	imx415_write_register(ViPipe, 0x36DA, 0x8C);
	imx415_write_register(ViPipe, 0x36DB, 0x00);
	imx415_write_register(ViPipe, 0x3724, 0x02);
	imx415_write_register(ViPipe, 0x3726, 0x02);
	imx415_write_register(ViPipe, 0x3732, 0x02);
	imx415_write_register(ViPipe, 0x3734, 0x03);
	imx415_write_register(ViPipe, 0x3736, 0x03);
	imx415_write_register(ViPipe, 0x3742, 0x03);
	imx415_write_register(ViPipe, 0x3862, 0xE0);
	imx415_write_register(ViPipe, 0x38CC, 0x30);
	imx415_write_register(ViPipe, 0x38CD, 0x2F);
	imx415_write_register(ViPipe, 0x395C, 0x0C);
	imx415_write_register(ViPipe, 0x3A42, 0xD1);
	imx415_write_register(ViPipe, 0x3A4C, 0x77);
	imx415_write_register(ViPipe, 0x3AE0, 0x02);
	imx415_write_register(ViPipe, 0x3AEC, 0x0C);
	imx415_write_register(ViPipe, 0x3B00, 0x2E);
	imx415_write_register(ViPipe, 0x3B06, 0x29);
	imx415_write_register(ViPipe, 0x3B98, 0x25);
	imx415_write_register(ViPipe, 0x3B99, 0x21);
	imx415_write_register(ViPipe, 0x3B9B, 0x13);
	imx415_write_register(ViPipe, 0x3B9C, 0x13);
	imx415_write_register(ViPipe, 0x3B9D, 0x13);
	imx415_write_register(ViPipe, 0x3B9E, 0x13);
	imx415_write_register(ViPipe, 0x3BA1, 0x00);
	imx415_write_register(ViPipe, 0x3BA2, 0x06);
	imx415_write_register(ViPipe, 0x3BA3, 0x0B);
	imx415_write_register(ViPipe, 0x3BA4, 0x10);
	imx415_write_register(ViPipe, 0x3BA5, 0x14);
	imx415_write_register(ViPipe, 0x3BA6, 0x18);
	imx415_write_register(ViPipe, 0x3BA7, 0x1A);
	imx415_write_register(ViPipe, 0x3BA8, 0x1A);
	imx415_write_register(ViPipe, 0x3BA9, 0x1A);
	imx415_write_register(ViPipe, 0x3BAC, 0xED);
	imx415_write_register(ViPipe, 0x3BAD, 0x01);
	imx415_write_register(ViPipe, 0x3BAE, 0xF6);
	imx415_write_register(ViPipe, 0x3BAF, 0x02);
	imx415_write_register(ViPipe, 0x3BB0, 0xA2);
	imx415_write_register(ViPipe, 0x3BB1, 0x03);
	imx415_write_register(ViPipe, 0x3BB2, 0xE0);
	imx415_write_register(ViPipe, 0x3BB3, 0x03);
	imx415_write_register(ViPipe, 0x3BB4, 0xE0);
	imx415_write_register(ViPipe, 0x3BB5, 0x03);
	imx415_write_register(ViPipe, 0x3BB6, 0xE0);
	imx415_write_register(ViPipe, 0x3BB7, 0x03);
	imx415_write_register(ViPipe, 0x3BB8, 0xE0);
	imx415_write_register(ViPipe, 0x3BBA, 0xE0);
	imx415_write_register(ViPipe, 0x3BBC, 0xDA);
	imx415_write_register(ViPipe, 0x3BBE, 0x88);
	imx415_write_register(ViPipe, 0x3BC0, 0x44);
	imx415_write_register(ViPipe, 0x3BC2, 0x7B);
	imx415_write_register(ViPipe, 0x3BC4, 0xA2);
	imx415_write_register(ViPipe, 0x3BC8, 0xBD);
	imx415_write_register(ViPipe, 0x3BCA, 0xBD);
	imx415_write_register(ViPipe, 0x4004, 0x48);
	imx415_write_register(ViPipe, 0x4005, 0x09);

    imx415_default_reg_init(ViPipe);

    imx415_write_register(ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx415_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
    //imx415_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ar_always("===IMX415 4K 60fps 12bit LINE Init OK!===\n");
    return;
}

void imx415_wdr2to1_4M10P_init(VI_PIPE ViPipe)
{
		/*
	IMX415-AAQR Window cropping 2688x1520 CSI-2_2lane 37.125Mhz AD:12bit Output:12bit 594Mbps Master Mode DOL HDR 2frame VC 7.497fps Integration Time LEF:16.018ms SEF:1.118ms Gain:6dB 
	Tool ver : Ver8.0
	*/
	imx415_write_register(ViPipe,  0x3008,  0x7F);  // BCWAIT_TIME[9:0]
	imx415_write_register(ViPipe,  0x300A,  0x5B);  // CPWAIT_TIME[9:0]
	imx415_write_register(ViPipe,  0x301C,  0x04);  // WINMODE[3:0]
	imx415_write_register(ViPipe,  0x3024,  0x8A);  // VMAX[19:0]
	imx415_write_register(ViPipe,  0x3025,  0x06);  //	
	imx415_write_register(ViPipe,  0x3028,  0x1c);  // HMAX[15:0]
	imx415_write_register(ViPipe,  0x3029,  0x17);  // 
	imx415_write_register(ViPipe,  0x302C,  0x01);  // WDMODE[1:0]
	imx415_write_register(ViPipe,  0x302D,  0x01);  // WDSEL[1:0]
	imx415_write_register(ViPipe,  0x3033,  0x07);  // SYS_MODE[3:0]
	imx415_write_register(ViPipe,  0x3040,  0x4C);  // PIX_HST[12:0]
	imx415_write_register(ViPipe,  0x3041,  0x02);  // 
	imx415_write_register(ViPipe,  0x3042,  0x80);  // PIX_HWIDTH[12:0]
	imx415_write_register(ViPipe,  0x3043,  0x0A);  // 
	imx415_write_register(ViPipe,  0x3044,  0xA0);  // PIX_VST[12:0]
	imx415_write_register(ViPipe,  0x3045,  0x02);  // 
	imx415_write_register(ViPipe,  0x3046,  0xE0);  // PIX_VWIDTH[12:0]
	imx415_write_register(ViPipe,  0x3047,  0x0B);  // 
	imx415_write_register(ViPipe,  0x3050,  0x82);  // SHR0[19:0]
	imx415_write_register(ViPipe,  0x3051,  0x0B);  // 
	imx415_write_register(ViPipe,  0x3054,  0x09);  // SHR1[19:0]
	imx415_write_register(ViPipe,  0x3090,  0x14);  // GAIN_PCG_0[8:0]
	imx415_write_register(ViPipe,  0x30C1,  0x00);  // XVS_DRV[1:0]
	imx415_write_register(ViPipe,  0x30CF,  0x01);  // XVSMSKCNT_INT[1:0]
	imx415_write_register(ViPipe,  0x3116,  0x24);  // INCKSEL2[7:0]
	imx415_write_register(ViPipe,  0x3118,  0x80);  // INCKSEL3[10:0]
	imx415_write_register(ViPipe,  0x311E,  0x24);  // INCKSEL5[7:0]
	imx415_write_register(ViPipe,  0x32D4,  0x21);  // -
	imx415_write_register(ViPipe,  0x32EC,  0xA1);  // -
	imx415_write_register(ViPipe,  0x344C,  0x2B);  // -
	imx415_write_register(ViPipe,  0x344D,  0x01);  // -
	imx415_write_register(ViPipe,  0x344E,  0xED);  // -
	imx415_write_register(ViPipe,  0x344F,  0x01);  // -
	imx415_write_register(ViPipe,  0x3450,  0xF6);  // -
	imx415_write_register(ViPipe,  0x3451,  0x02);  // -
	imx415_write_register(ViPipe,  0x3452,  0x7F);  // -
	imx415_write_register(ViPipe,  0x3453,  0x03);  // -
	imx415_write_register(ViPipe,  0x358A,  0x04);  // -
	imx415_write_register(ViPipe,  0x35A1,  0x02);  // -
	imx415_write_register(ViPipe,  0x35EC,  0x27);  // -
	imx415_write_register(ViPipe,  0x35EE,  0x8D);  // -
	imx415_write_register(ViPipe,  0x35F0,  0x8D);  // -
	imx415_write_register(ViPipe,  0x35F2,  0x29);  // -
	imx415_write_register(ViPipe,  0x36BC,  0x0C);  // -
	imx415_write_register(ViPipe,  0x36CC,  0x53);  // -
	imx415_write_register(ViPipe,  0x36CD,  0x00);  // -
	imx415_write_register(ViPipe,  0x36CE,  0x3C);  // -
	imx415_write_register(ViPipe,  0x36D0,  0x8C);  // -
	imx415_write_register(ViPipe,  0x36D1,  0x00);  // -
	imx415_write_register(ViPipe,  0x36D2,  0x71);  // -
	imx415_write_register(ViPipe,  0x36D4,  0x3C);  // -
	imx415_write_register(ViPipe,  0x36D6,  0x53);  // -
	imx415_write_register(ViPipe,  0x36D7,  0x00);  // -
	imx415_write_register(ViPipe,  0x36D8,  0x71);  // -
	imx415_write_register(ViPipe,  0x36DA,  0x8C);  // -
	imx415_write_register(ViPipe,  0x36DB,  0x00);  // -
	imx415_write_register(ViPipe,  0x3720,  0x00);  // -
	imx415_write_register(ViPipe,  0x3724,  0x02);  // -
	imx415_write_register(ViPipe,  0x3726,  0x02);  // -
	imx415_write_register(ViPipe,  0x3732,  0x02);  // -
	imx415_write_register(ViPipe,  0x3734,  0x03);  // -
	imx415_write_register(ViPipe,  0x3736,  0x03);  // -
	imx415_write_register(ViPipe,  0x3742,  0x03);  // -
	imx415_write_register(ViPipe,  0x3862,  0xE0);  // -
	imx415_write_register(ViPipe,  0x38CC,  0x30);  // -
	imx415_write_register(ViPipe,  0x38CD,  0x2F);  // -
	imx415_write_register(ViPipe,  0x395C,  0x0C);  // -
	imx415_write_register(ViPipe,  0x39A4,  0x07);  // -
	imx415_write_register(ViPipe,  0x39A8,  0x32);  // -
	imx415_write_register(ViPipe,  0x39AA,  0x32);  // -
	imx415_write_register(ViPipe,  0x39AC,  0x32);  // -
	imx415_write_register(ViPipe,  0x39AE,  0x32);  // -
	imx415_write_register(ViPipe,  0x39B0,  0x32);  // -
	imx415_write_register(ViPipe,  0x39B2,  0x2F);  // -
	imx415_write_register(ViPipe,  0x39B4,  0x2D);  // -
	imx415_write_register(ViPipe,  0x39B6,  0x28);  // -
	imx415_write_register(ViPipe,  0x39B8,  0x30);  // -
	imx415_write_register(ViPipe,  0x39BA,  0x30);  // -
	imx415_write_register(ViPipe,  0x39BC,  0x30);  // -
	imx415_write_register(ViPipe,  0x39BE,  0x30);  // -
	imx415_write_register(ViPipe,  0x39C0,  0x30);  // -
	imx415_write_register(ViPipe,  0x39C2,  0x2E);  // -
	imx415_write_register(ViPipe,  0x39C4,  0x2B);  // -
	imx415_write_register(ViPipe,  0x39C6,  0x25);  // -
	imx415_write_register(ViPipe,  0x3A42,  0xD1);  // -
	imx415_write_register(ViPipe,  0x3A4C,  0x77);  // -
	imx415_write_register(ViPipe,  0x3AE0,  0x02);  // -
	imx415_write_register(ViPipe,  0x3AEC,  0x0C);  // -
	imx415_write_register(ViPipe,  0x3B00,  0x2E);  // -
	imx415_write_register(ViPipe,  0x3B06,  0x29);  // -
	imx415_write_register(ViPipe,  0x3B98,  0x25);  // -
	imx415_write_register(ViPipe,  0x3B99,  0x21);  // -
	imx415_write_register(ViPipe,  0x3B9B,  0x13);  // -
	imx415_write_register(ViPipe,  0x3B9C,  0x13);  // -
	imx415_write_register(ViPipe,  0x3B9D,  0x13);  // -
	imx415_write_register(ViPipe,  0x3B9E,  0x13);  // -
	imx415_write_register(ViPipe,  0x3BA1,  0x00);  // -
	imx415_write_register(ViPipe,  0x3BA2,  0x06);  // -
	imx415_write_register(ViPipe,  0x3BA3,  0x0B);  // -
	imx415_write_register(ViPipe,  0x3BA4,  0x10);  // -
	imx415_write_register(ViPipe,  0x3BA5,  0x14);  // -
	imx415_write_register(ViPipe,  0x3BA6,  0x18);  // -
	imx415_write_register(ViPipe,  0x3BA7,  0x1A);  // -
	imx415_write_register(ViPipe,  0x3BA8,  0x1A);  // -
	imx415_write_register(ViPipe,  0x3BA9,  0x1A);  // -
	imx415_write_register(ViPipe,  0x3BAC,  0xED);  // -
	imx415_write_register(ViPipe,  0x3BAD,  0x01);  // -
	imx415_write_register(ViPipe,  0x3BAE,  0xF6);  // -
	imx415_write_register(ViPipe,  0x3BAF,  0x02);  // -
	imx415_write_register(ViPipe,  0x3BB0,  0xA2);  // -
	imx415_write_register(ViPipe,  0x3BB1,  0x03);  // -
	imx415_write_register(ViPipe,  0x3BB2,  0xE0);  // -
	imx415_write_register(ViPipe,  0x3BB3,  0x03);  // -
	imx415_write_register(ViPipe,  0x3BB4,  0xE0);  // -
	imx415_write_register(ViPipe,  0x3BB5,  0x03);  // -
	imx415_write_register(ViPipe,  0x3BB6,  0xE0);  // -
	imx415_write_register(ViPipe,  0x3BB7,  0x03);  // -
	imx415_write_register(ViPipe,  0x3BB8,  0xE0);  // -
	imx415_write_register(ViPipe,  0x3BBA,  0xE0);  // -
	imx415_write_register(ViPipe,  0x3BBC,  0xDA);  // -
	imx415_write_register(ViPipe,  0x3BBE,  0x88);  // -
	imx415_write_register(ViPipe,  0x3BC0,  0x44);  // -
	imx415_write_register(ViPipe,  0x3BC2,  0x7B);  // -
	imx415_write_register(ViPipe,  0x3BC4,  0xA2);  // -
	imx415_write_register(ViPipe,  0x3BC8,  0xBD);  // -
	imx415_write_register(ViPipe,  0x3BCA,  0xBD);  // -
	imx415_write_register(ViPipe,  0x301c, 0x04);   // -crop setting
	imx415_write_register(ViPipe,  0x3040, 0x28);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3042, 0x80);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3043, 0x0a);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3044, 0x28); // -crop setting
	imx415_write_register(ViPipe,  0x3046, 0xe0); // -crop setting
	imx415_write_register(ViPipe,  0x3047, 0x0B); // -crop setting
	imx415_write_register(ViPipe,  0x4001,  0x01);  // LANEMODE[2:0]
	imx415_write_register(ViPipe,  0x4004,  0x48);  // TXCLKESC_FREQ[15:0]
	imx415_write_register(ViPipe,  0x4005,  0x09);  // 
	imx415_write_register(ViPipe,  0x400C,  0x00);  // INCKSEL6
	imx415_write_register(ViPipe,  0x4018,  0x67);  // TCLKPOST[15:0]
	imx415_write_register(ViPipe,  0x401A,  0x27);  // TCLKPREPARE[15:0]
	imx415_write_register(ViPipe,  0x401C,  0x27);  // TCLKTRAIL[15:0]
	imx415_write_register(ViPipe,  0x401E,  0xB7);  // TCLKZERO[15:0]
	imx415_write_register(ViPipe,  0x401F,  0x00);  // 
	imx415_write_register(ViPipe,  0x4020,  0x2F);  // THSPREPARE[15:0]
	imx415_write_register(ViPipe,  0x4022,  0x4F);  // THSZERO[15:0]
	imx415_write_register(ViPipe,  0x4024,  0x2F);  // THSTRAIL[15:0]
	imx415_write_register(ViPipe,  0x4026,  0x47);  // THSEXIT[15:0]
	imx415_write_register(ViPipe,  0x4028,  0x27);  // TLPX[15:0]
	imx415_write_register(ViPipe,  0x4074,  0x01);  // INCKSEL7 [2:0]
	
	imx415_write_register(ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx415_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
}


void imx415_linear_4Mp30_init(VI_PIPE ViPipe)
{
#if 0	
	/*
	  IMX415-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 1782Mbps Master Mode 30fps Integration Time 33.277ms
	  clock pre/lane  1782/4 = 445M x 2 =890	 
	  Tool ver : Ver6.0
	*/  
	imx415_write_register(ViPipe, 0x3008, 0x7F);
    imx415_write_register(ViPipe, 0x300A, 0x5B);
	
    imx415_write_register(ViPipe, 0x3024, 0xCA);
    imx415_write_register(ViPipe, 0x3025, 0x08);
	
    imx415_write_register(ViPipe, 0x3028, 0x4C);
    imx415_write_register(ViPipe, 0x3029, 0x04);
    imx415_write_register(ViPipe, 0x3033, 0x05);
    imx415_write_register(ViPipe, 0x3050, 0x08);

    imx415_write_register(ViPipe, 0x30C1, 0x00);

    // normal exp/gain mod
    imx415_write_register(ViPipe, 0x302C, 0x00);
    imx415_write_register(ViPipe, 0x302D, 0x00);
    imx415_write_register(ViPipe, 0x3260, 0x01);

    imx415_write_register(ViPipe, 0x3116, 0x24);
    imx415_write_register(ViPipe, 0x311E, 0x24);
    imx415_write_register(ViPipe, 0x32D4, 0x21);
    imx415_write_register(ViPipe, 0x32EC, 0xA1);
    imx415_write_register(ViPipe, 0x3452, 0x7F);
    imx415_write_register(ViPipe, 0x3453, 0x03);
    imx415_write_register(ViPipe, 0x358A, 0x04);
    imx415_write_register(ViPipe, 0x35A1, 0x02);
    imx415_write_register(ViPipe, 0x36BC, 0x0C);
    imx415_write_register(ViPipe, 0x36CC, 0x53);
    imx415_write_register(ViPipe, 0x36CD, 0x00);
    imx415_write_register(ViPipe, 0x36CE, 0x3C);
    imx415_write_register(ViPipe, 0x36D0, 0x8C);
    imx415_write_register(ViPipe, 0x36D1, 0x00);
    imx415_write_register(ViPipe, 0x36D2, 0x71);
    imx415_write_register(ViPipe, 0x36D4, 0x3C);
    imx415_write_register(ViPipe, 0x36D6, 0x53);
    imx415_write_register(ViPipe, 0x36D7, 0x00);
    imx415_write_register(ViPipe, 0x36D8, 0x71);
    imx415_write_register(ViPipe, 0x36DA, 0x8C);
    imx415_write_register(ViPipe, 0x36DB, 0x00);
    imx415_write_register(ViPipe, 0x3724, 0x02);
    imx415_write_register(ViPipe, 0x3726, 0x02);
    imx415_write_register(ViPipe, 0x3732, 0x02);
    imx415_write_register(ViPipe, 0x3734, 0x03);
    imx415_write_register(ViPipe, 0x3736, 0x03);
    imx415_write_register(ViPipe, 0x3742, 0x03);
    imx415_write_register(ViPipe, 0x3862, 0xE0);
    imx415_write_register(ViPipe, 0x38CC, 0x30);
    imx415_write_register(ViPipe, 0x38CD, 0x2F);
    imx415_write_register(ViPipe, 0x395C, 0x0C);
    imx415_write_register(ViPipe, 0x3A42, 0xD1);
    imx415_write_register(ViPipe, 0x3A4C, 0x77);
    imx415_write_register(ViPipe, 0x3AE0, 0x02);
    imx415_write_register(ViPipe, 0x3AEC, 0x0C);
    imx415_write_register(ViPipe, 0x3B00, 0x2E);
    imx415_write_register(ViPipe, 0x3B06, 0x29);
    imx415_write_register(ViPipe, 0x3B98, 0x25);
    imx415_write_register(ViPipe, 0x3B99, 0x21);
    imx415_write_register(ViPipe, 0x3B9B, 0x13);
    imx415_write_register(ViPipe, 0x3B9C, 0x13);
    imx415_write_register(ViPipe, 0x3B9D, 0x13);
    imx415_write_register(ViPipe, 0x3B9E, 0x13);
    imx415_write_register(ViPipe, 0x3BA1, 0x00);
    imx415_write_register(ViPipe, 0x3BA2, 0x06);
    imx415_write_register(ViPipe, 0x3BA3, 0x0B);
    imx415_write_register(ViPipe, 0x3BA4, 0x10);
    imx415_write_register(ViPipe, 0x3BA5, 0x14);
    imx415_write_register(ViPipe, 0x3BA6, 0x18);
    imx415_write_register(ViPipe, 0x3BA7, 0x1A);
    imx415_write_register(ViPipe, 0x3BA8, 0x1A);
    imx415_write_register(ViPipe, 0x3BA9, 0x1A);
    imx415_write_register(ViPipe, 0x3BAC, 0xED);
    imx415_write_register(ViPipe, 0x3BAD, 0x01);
    imx415_write_register(ViPipe, 0x3BAE, 0xF6);
    imx415_write_register(ViPipe, 0x3BAF, 0x02);
    imx415_write_register(ViPipe, 0x3BB0, 0xA2);
    imx415_write_register(ViPipe, 0x3BB1, 0x03);
    imx415_write_register(ViPipe, 0x3BB2, 0xE0);
    imx415_write_register(ViPipe, 0x3BB3, 0x03);
    imx415_write_register(ViPipe, 0x3BB4, 0xE0);
    imx415_write_register(ViPipe, 0x3BB5, 0x03);
    imx415_write_register(ViPipe, 0x3BB6, 0xE0);
    imx415_write_register(ViPipe, 0x3BB7, 0x03);
    imx415_write_register(ViPipe, 0x3BB8, 0xE0);
    imx415_write_register(ViPipe, 0x3BBA, 0xE0);
    imx415_write_register(ViPipe, 0x3BBC, 0xDA);
    imx415_write_register(ViPipe, 0x3BBE, 0x88);
    imx415_write_register(ViPipe, 0x3BC0, 0x44);
    imx415_write_register(ViPipe, 0x3BC2, 0x7B);
    imx415_write_register(ViPipe, 0x3BC4, 0xA2);
    imx415_write_register(ViPipe, 0x3BC8, 0xBD);
    imx415_write_register(ViPipe, 0x3BCA, 0xBD);
	imx415_write_register(ViPipe,  0x301c, 0x04);   // -crop setting
	imx415_write_register(ViPipe,  0x3040, 0x28);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3042, 0x80);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3043, 0x0a);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3044, 0x28); // -crop setting
	imx415_write_register(ViPipe,  0x3046, 0xe0); // -crop setting
	imx415_write_register(ViPipe,  0x3047, 0x0B); // -crop setting
    imx415_write_register(ViPipe, 0x4004, 0x48);
    imx415_write_register(ViPipe, 0x4005, 0x09);
    imx415_write_register(ViPipe, 0x400C, 0x00);
    imx415_write_register(ViPipe, 0x4018, 0x7F);
    imx415_write_register(ViPipe, 0x401A, 0x37);
    imx415_write_register(ViPipe, 0x401C, 0x37);
    imx415_write_register(ViPipe, 0x401E, 0xF7);
    imx415_write_register(ViPipe, 0x401F, 0x00);
    imx415_write_register(ViPipe, 0x4020, 0x3F);
    imx415_write_register(ViPipe, 0x4022, 0x6F);
    imx415_write_register(ViPipe, 0x4024, 0x3F);
    imx415_write_register(ViPipe, 0x4026, 0x5F);
    imx415_write_register(ViPipe, 0x4028, 0x2F);
    imx415_write_register(ViPipe, 0x4074, 0x01);

    imx415_default_reg_init(ViPipe);
#else
	/*
	  IMX415-AAQR All-pixel scan CSI-2_4lane 37.125Mhz AD:12bit Output:12bit 1782Mbps Master Mode 60fps Integration Time 16.277ms
	  Tool ver : Ver6.0
	*/
	imx415_write_register(ViPipe, 0x3008, 0x7F);
	imx415_write_register(ViPipe, 0x300A, 0x5B);

	imx415_write_register(ViPipe, 0x3024, 0xca);  //vmax
	imx415_write_register(ViPipe, 0x3025, 0x08);


	imx415_write_register(ViPipe, 0x3028, 0x26);	//hmax
	imx415_write_register(ViPipe, 0x3029, 0x02);

	imx415_write_register(ViPipe, 0x3050, 0x08);
	imx415_write_register(ViPipe, 0x30C1, 0x00);
	imx415_write_register(ViPipe, 0x3116, 0x24);
	imx415_write_register(ViPipe, 0x311E, 0x24);
	imx415_write_register(ViPipe, 0x32D4, 0x21);
	imx415_write_register(ViPipe, 0x32EC, 0xA1);
	imx415_write_register(ViPipe, 0x3452, 0x7F);
	imx415_write_register(ViPipe, 0x3453, 0x03);
	imx415_write_register(ViPipe, 0x358A, 0x04);
	imx415_write_register(ViPipe, 0x35A1, 0x02);
	imx415_write_register(ViPipe, 0x36BC, 0x0C);
	imx415_write_register(ViPipe, 0x36CC, 0x53);
	imx415_write_register(ViPipe, 0x36CD, 0x00);
	imx415_write_register(ViPipe, 0x36CE, 0x3C);
	imx415_write_register(ViPipe, 0x36D0, 0x8C);
	imx415_write_register(ViPipe, 0x36D1, 0x00);
	imx415_write_register(ViPipe, 0x36D2, 0x71);
	imx415_write_register(ViPipe, 0x36D4, 0x3C);
	imx415_write_register(ViPipe, 0x36D6, 0x53);
	imx415_write_register(ViPipe, 0x36D7, 0x00);
	imx415_write_register(ViPipe, 0x36D8, 0x71);
	imx415_write_register(ViPipe, 0x36DA, 0x8C);
	imx415_write_register(ViPipe, 0x36DB, 0x00);
	imx415_write_register(ViPipe, 0x3724, 0x02);
	imx415_write_register(ViPipe, 0x3726, 0x02);
	imx415_write_register(ViPipe, 0x3732, 0x02);
	imx415_write_register(ViPipe, 0x3734, 0x03);
	imx415_write_register(ViPipe, 0x3736, 0x03);
	imx415_write_register(ViPipe, 0x3742, 0x03);
	imx415_write_register(ViPipe, 0x3862, 0xE0);
	imx415_write_register(ViPipe, 0x38CC, 0x30);
	imx415_write_register(ViPipe, 0x38CD, 0x2F);
	imx415_write_register(ViPipe, 0x395C, 0x0C);
	imx415_write_register(ViPipe, 0x3A42, 0xD1);
	imx415_write_register(ViPipe, 0x3A4C, 0x77);
	imx415_write_register(ViPipe, 0x3AE0, 0x02);
	imx415_write_register(ViPipe, 0x3AEC, 0x0C);
	imx415_write_register(ViPipe, 0x3B00, 0x2E);
	imx415_write_register(ViPipe, 0x3B06, 0x29);
	imx415_write_register(ViPipe, 0x3B98, 0x25);
	imx415_write_register(ViPipe, 0x3B99, 0x21);
	imx415_write_register(ViPipe, 0x3B9B, 0x13);
	imx415_write_register(ViPipe, 0x3B9C, 0x13);
	imx415_write_register(ViPipe, 0x3B9D, 0x13);
	imx415_write_register(ViPipe, 0x3B9E, 0x13);
	imx415_write_register(ViPipe, 0x3BA1, 0x00);
	imx415_write_register(ViPipe, 0x3BA2, 0x06);
	imx415_write_register(ViPipe, 0x3BA3, 0x0B);
	imx415_write_register(ViPipe, 0x3BA4, 0x10);
	imx415_write_register(ViPipe, 0x3BA5, 0x14);
	imx415_write_register(ViPipe, 0x3BA6, 0x18);
	imx415_write_register(ViPipe, 0x3BA7, 0x1A);
	imx415_write_register(ViPipe, 0x3BA8, 0x1A);
	imx415_write_register(ViPipe, 0x3BA9, 0x1A);
	imx415_write_register(ViPipe, 0x3BAC, 0xED);
	imx415_write_register(ViPipe, 0x3BAD, 0x01);
	imx415_write_register(ViPipe, 0x3BAE, 0xF6);
	imx415_write_register(ViPipe, 0x3BAF, 0x02);
	imx415_write_register(ViPipe, 0x3BB0, 0xA2);
	imx415_write_register(ViPipe, 0x3BB1, 0x03);
	imx415_write_register(ViPipe, 0x3BB2, 0xE0);
	imx415_write_register(ViPipe, 0x3BB3, 0x03);
	imx415_write_register(ViPipe, 0x3BB4, 0xE0);
	imx415_write_register(ViPipe, 0x3BB5, 0x03);
	imx415_write_register(ViPipe, 0x3BB6, 0xE0);
	imx415_write_register(ViPipe, 0x3BB7, 0x03);
	imx415_write_register(ViPipe, 0x3BB8, 0xE0);
	imx415_write_register(ViPipe, 0x3BBA, 0xE0);
	imx415_write_register(ViPipe, 0x3BBC, 0xDA);
	imx415_write_register(ViPipe, 0x3BBE, 0x88);
	imx415_write_register(ViPipe, 0x3BC0, 0x44);
	imx415_write_register(ViPipe, 0x3BC2, 0x7B);
	imx415_write_register(ViPipe, 0x3BC4, 0xA2);
	imx415_write_register(ViPipe, 0x3BC8, 0xBD);
	imx415_write_register(ViPipe, 0x3BCA, 0xBD);
	imx415_write_register(ViPipe,  0x301c, 0x04);   // -crop setting
	imx415_write_register(ViPipe,  0x3040, 0x28);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3042, 0x80);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3043, 0x0a);   // -crop setting // -crop setting
	imx415_write_register(ViPipe,  0x3044, 0x28); // -crop setting
	imx415_write_register(ViPipe,  0x3046, 0xe0); // -crop setting
	imx415_write_register(ViPipe,  0x3047, 0x0B); // -crop setting
	imx415_write_register(ViPipe, 0x4004, 0x48);
	imx415_write_register(ViPipe, 0x4005, 0x09);	
	imx415_write_register(ViPipe, 0x4000, 0x11);	
	//imx415_write_register(ViPipe, 0x4022, 0xff);
	//imx415_default_reg_init(ViPipe);
#endif
    //imx415_write_register(ViPipe, 0x3000, 0x00); /* standby */
   // delay_ms(20);
   // imx415_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
	ar_always("===IMX415 4M 30fps 12bit LINE Hight speed Init OK!===\n");	
	return;
}


