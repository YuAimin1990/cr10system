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

const unsigned char imx290_2_i2c_addr     =    0x34;        /* I2C Address of IMX290_2 */
const unsigned int  imx290_2_addr_byte    =    2;
const unsigned int  imx290_2_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastImx290_2[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunImx290_2BusInfo[];

int imx290_2_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunImx290_2BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	ar_always("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx290_2_i2c_addr >> 1));
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

int imx290_2_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int imx290_2_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int imx290_2_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = imx290_2_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = imx290_2_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = imx290_2_data_byte;

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

    if (imx290_2_addr_byte == 2)
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

    if (imx290_2_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, imx290_2_addr_byte + imx290_2_data_byte);
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

void imx290_2_prog(VI_PIPE ViPipe, int *rom)
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
            imx290_2_write_register(ViPipe, addr, data);
        }
    }
}

void imx290_2_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void imx290_2_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define IMX290_2_FLIP_MIRRO (0x3007)

int imx290_2_flip_off_mirro_off(VI_PIPE ViPipe)
{
    imx290_2_write_register(ViPipe, IMX290_2_FLIP_MIRRO, 0x00);
    return 0;
}

int imx290_2_flip_on_mirro_off(VI_PIPE ViPipe)
{
    imx290_2_write_register(ViPipe, IMX290_2_FLIP_MIRRO, 0x01);
    return 0;
}

int imx290_2_flip_off_mirro_on(VI_PIPE ViPipe)
{
    imx290_2_write_register(ViPipe, IMX290_2_FLIP_MIRRO, 0x02);
    return 0;
}

int imx290_2_flip_on_mirro_on(VI_PIPE ViPipe)
{
    imx290_2_write_register(ViPipe, IMX290_2_FLIP_MIRRO, 0x03);
    return 0;
}

#define IMX290_2_SENSOR_1080P_30FPS_LINEAR_MODE      (1)
#define IMX290_2_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
#define IMX290_2_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
#define IMX290_2_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)
#define IMX290_2_SENSOR_1080P_60FPS_LINEAR_MODE      (5)

void imx290_2_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
void imx290_2_wdr_1080p60_2to1_init(VI_PIPE ViPipe);
void imx290_2_wdr_1080p120_2to1_init(VI_PIPE ViPipe);
void imx290_2_wdr_720p60_2to1_init(VI_PIPE ViPipe);
void imx290_2_wdr_1080p30_3to1_init(VI_PIPE ViPipe);
void imx290_2_wdr_1080p120_3to1_init(VI_PIPE ViPipe);
void imx290_2_wdr_720p60_3to1_init(VI_PIPE ViPipe);
void imx290_2_linear_1080p30_init(VI_PIPE ViPipe);
void imx290_2_linear_1080p60_init(VI_PIPE ViPipe);


void imx290_2_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastImx290_2[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            imx290_2_write_register(ViPipe, g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void imx290_2_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastImx290_2[ViPipe]->bInit;
    enWDRMode   = g_pastImx290_2[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastImx290_2[ViPipe]->u8ImgMode;

    imx290_2_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (IMX290_2_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX290_2_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                imx290_2_wdr_1080p60_2to1_init(ViPipe);
            }
            else if (IMX290_2_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX290_2_SENSOR_720P_30FPS_2t1_WDR_MODE */
            {
                imx290_2_wdr_720p60_2to1_init(ViPipe);
            }
            else
            {
            }
        }
        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            imx290_2_wdr_1080p120_3to1_init(ViPipe);
        }
        else
        {
            if (IMX290_2_SENSOR_1080P_30FPS_LINEAR_MODE == u8ImgMode)
			{
				imx290_2_linear_1080p30_init(ViPipe);
			}
			else if (IMX290_2_SENSOR_1080P_60FPS_LINEAR_MODE == u8ImgMode) 
			{
				imx290_2_linear_1080p60_init(ViPipe);
			}
			else
			{
				imx290_2_linear_1080p30_init(ViPipe);
			}
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (IMX290_2_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX290_2_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                imx290_2_wdr_1080p60_2to1_init(ViPipe);
            }
            else if (IMX290_2_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX290_2_SENSOR_720P_30FPS_2t1_WDR_MODE */
            {
                imx290_2_wdr_720p60_2to1_init(ViPipe);
            }
            else
            {
            }
        }

        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            imx290_2_wdr_1080p120_3to1_init(ViPipe);
        }

        else
        {
            if (IMX290_2_SENSOR_1080P_30FPS_LINEAR_MODE == u8ImgMode)
			{
				imx290_2_linear_1080p30_init(ViPipe);
			}
			else if (IMX290_2_SENSOR_1080P_60FPS_LINEAR_MODE == u8ImgMode) 
			{
				imx290_2_linear_1080p60_init(ViPipe);
			}
			else
			{
				imx290_2_linear_1080p30_init(ViPipe);
			}
        }
    }

    // for (i = 0; i < g_pastImx290_2[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     imx290_2_write_register(ViPipe, g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx290_2[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastImx290_2[ViPipe]->bInit = AR_TRUE;
    return ;
}

void imx290_2_exit(VI_PIPE ViPipe)
{
    imx290_2_i2c_exit(ViPipe);

    return;
}

/* 1080P30 */
void imx290_2_linear_1080p30_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe,0x3002,0x00);
    imx290_2_write_register (ViPipe,0x3005,0x01);
    imx290_2_write_register (ViPipe,0x3007,0x00);

    imx290_2_write_register (ViPipe,0x3009,0x02);

    imx290_2_write_register (ViPipe,0x300A,0xF0);
    imx290_2_write_register (ViPipe,0x3011,0x0a);

    imx290_2_write_register (ViPipe,0x3018,0x65);	//dg 
    imx290_2_write_register (ViPipe,0x3019,0x04);	//dg

    imx290_2_write_register (ViPipe,0x301c,0x30);
    imx290_2_write_register (ViPipe,0x301d,0x11);

    imx290_2_write_register (ViPipe,0x3046,0x01);
    imx290_2_write_register (ViPipe,0x304B,0x0a);

    imx290_2_write_register (ViPipe,0x305C,0x18);
    imx290_2_write_register (ViPipe,0x305D,0x03);
    imx290_2_write_register (ViPipe,0x305E,0x20);
    imx290_2_write_register (ViPipe,0x305F,0x01);

    imx290_2_write_register (ViPipe,0x309e,0x4a);
    imx290_2_write_register (ViPipe,0x309f,0x4a);

    imx290_2_write_register (ViPipe,0x311C,0x0E);
    imx290_2_write_register (ViPipe,0x3128,0x04);
    imx290_2_write_register (ViPipe,0x3129,0x00);
    imx290_2_write_register (ViPipe,0x313b,0x41);
    imx290_2_write_register (ViPipe,0x315E,0x1A);
    imx290_2_write_register (ViPipe,0x3164,0x1A);
    imx290_2_write_register (ViPipe,0x317C,0x00);
    imx290_2_write_register (ViPipe,0x31EC,0x0E);

    imx290_2_write_register (ViPipe,0x3405,0x10);

    imx290_2_write_register (ViPipe,0x3407,0x01);
    imx290_2_write_register (ViPipe,0x3414,0x0A);
    imx290_2_write_register (ViPipe,0x3418,0x38);
    imx290_2_write_register (ViPipe,0x3419,0x04);
    imx290_2_write_register (ViPipe,0x3441,0x0C);
    imx290_2_write_register (ViPipe,0x3442,0x0C);
    imx290_2_write_register (ViPipe,0x3443,0x01);
    imx290_2_write_register (ViPipe,0x3444,0x20);
    imx290_2_write_register (ViPipe,0x3445,0x25);

    imx290_2_write_register (ViPipe,0x3446,0x57);
    imx290_2_write_register (ViPipe,0x3447,0x00);

    imx290_2_write_register (ViPipe,0x3448,0x37);
    imx290_2_write_register (ViPipe,0x3449,0x00);

    imx290_2_write_register (ViPipe,0x344A,0x1F);
    imx290_2_write_register (ViPipe,0x344B,0x00);

    imx290_2_write_register (ViPipe,0x344C,0x1F);
    imx290_2_write_register (ViPipe,0x344D,0x00);

    imx290_2_write_register (ViPipe,0x344E,0x1F);
    imx290_2_write_register (ViPipe,0x344F,0x00);

    imx290_2_write_register (ViPipe,0x3450,0x77);
    imx290_2_write_register (ViPipe,0x3451,0x00);

    imx290_2_write_register (ViPipe,0x3452,0x1F);
    imx290_2_write_register (ViPipe,0x3453,0x00);

    imx290_2_write_register (ViPipe,0x3454,0x17);
    imx290_2_write_register (ViPipe,0x3455,0x00);

    imx290_2_write_register (ViPipe,0x3472,0x80);
    imx290_2_write_register (ViPipe,0x3473,0x07);
    //{REGISTER_OP_WRITE,0x3480,0x49,

    imx290_2_default_reg_init(ViPipe);

    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //imx290_2_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ar_always("===IMX290_2 1080P 30fps 12bit LINE Init OK!===\n");
    return;
}


/* 1080P60 */
void imx290_2_linear_1080p60_init(VI_PIPE ViPipe)
{

    imx290_2_write_register (ViPipe,0x3002,0x00);
    imx290_2_write_register (ViPipe,0x3005,0x01);
    imx290_2_write_register (ViPipe,0x3007,0x00);

    imx290_2_write_register (ViPipe,0x3009,0x01);	//dg for 60fps

    imx290_2_write_register (ViPipe,0x300A,0xF0);
    imx290_2_write_register (ViPipe,0x3011,0x0a);

    imx290_2_write_register (ViPipe,0x3018,0x65);	//dg 
    imx290_2_write_register (ViPipe,0x3019,0x04);	//dg

    imx290_2_write_register (ViPipe,0x301c,0x98); //dg	for 60fps
    imx290_2_write_register (ViPipe,0x301d,0x08); //dg

    imx290_2_write_register (ViPipe,0x3046,0x01);
    imx290_2_write_register (ViPipe,0x304B,0x0a);

    imx290_2_write_register (ViPipe,0x305C,0x18);
    imx290_2_write_register (ViPipe,0x305D,0x03);
    imx290_2_write_register (ViPipe,0x305E,0x20);
    imx290_2_write_register (ViPipe,0x305F,0x01);

    imx290_2_write_register (ViPipe,0x309e,0x4a);
    imx290_2_write_register (ViPipe,0x309f,0x4a);

    imx290_2_write_register (ViPipe,0x311C,0x0E);
    imx290_2_write_register (ViPipe,0x3128,0x04);
    imx290_2_write_register (ViPipe,0x3129,0x00);
    imx290_2_write_register (ViPipe,0x313b,0x41);
    imx290_2_write_register (ViPipe,0x315E,0x1A);
    imx290_2_write_register (ViPipe,0x3164,0x1A);
    imx290_2_write_register (ViPipe,0x317C,0x00);
    imx290_2_write_register (ViPipe,0x31EC,0x0E);

    imx290_2_write_register (ViPipe,0x3405,0x00);	// dg for 60fps

    imx290_2_write_register (ViPipe,0x3407,0x01);
    imx290_2_write_register (ViPipe,0x3414,0x0A);
    imx290_2_write_register (ViPipe,0x3418,0x38);
    imx290_2_write_register (ViPipe,0x3419,0x04);
    imx290_2_write_register (ViPipe,0x3441,0x0C);
    imx290_2_write_register (ViPipe,0x3442,0x0C);
    imx290_2_write_register (ViPipe,0x3443,0x01);
    imx290_2_write_register (ViPipe,0x3444,0x20);
    imx290_2_write_register (ViPipe,0x3445,0x25);

    imx290_2_write_register (ViPipe,0x3446,0x77);	// dg for 60fps
    imx290_2_write_register (ViPipe,0x3447,0x00);

    imx290_2_write_register (ViPipe,0x3448,0x67);	// dg for 60fps
    imx290_2_write_register (ViPipe,0x3449,0x00);

    imx290_2_write_register (ViPipe,0x344A,0x47); // dg for 60fps
    imx290_2_write_register (ViPipe,0x344B,0x00);

    imx290_2_write_register (ViPipe,0x344C,0x37); // dg for 60fps
    imx290_2_write_register (ViPipe,0x344D,0x00);

    imx290_2_write_register (ViPipe,0x344E,0x3F); // dg for 60fps
    imx290_2_write_register (ViPipe,0x344F,0x00);

    imx290_2_write_register (ViPipe,0x3450,0xFF); // dg for 60fps
    imx290_2_write_register (ViPipe,0x3451,0x00);

    imx290_2_write_register (ViPipe,0x3452,0x03); // dg for 60fps
    imx290_2_write_register (ViPipe,0x3453,0x00);

    imx290_2_write_register (ViPipe,0x3454,0x37); // dg for 60fps
    imx290_2_write_register (ViPipe,0x3455,0x00);

    imx290_2_write_register (ViPipe,0x3472,0x80);
    imx290_2_write_register (ViPipe,0x3473,0x07);
    //{REGISTER_OP_WRITE,0x3480,0x49,

    imx290_2_default_reg_init(ViPipe);

    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //imx290_2_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ar_always("===IMX290_2 1080P 60fps 12bit LINE Init OK!===\n");
    return;
}


void imx290_2_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe, 0x3000, 0x01); /* standby */
    imx290_2_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //10bit
    imx290_2_write_register (ViPipe, 0x3005, 0x00);
    imx290_2_write_register (ViPipe, 0x3007, 0x00);
    imx290_2_write_register (ViPipe, 0x300a, 0x3c);
    imx290_2_write_register (ViPipe, 0x300c, 0x11);
    imx290_2_write_register (ViPipe, 0x300f, 0x00);
    imx290_2_write_register (ViPipe, 0x3010, 0x21);
    imx290_2_write_register (ViPipe, 0x3012, 0x64);
    imx290_2_write_register (ViPipe, 0x3016, 0x09);

    imx290_2_write_register (ViPipe, 0x3018, 0xA6);  /**** VMAX ****/
    imx290_2_write_register (ViPipe, 0x3019, 0x04);
    imx290_2_write_register (ViPipe, 0x301A, 0x00);

    imx290_2_write_register (ViPipe, 0x301C, 0xD8); /***** HMAX ****/
    imx290_2_write_register (ViPipe, 0x301D, 0x0F);

    imx290_2_write_register (ViPipe, 0x3020, 0x02);
    imx290_2_write_register (ViPipe, 0x3024, 0xc9);
    imx290_2_write_register (ViPipe, 0x3030, 0x0b);
    imx290_2_write_register (ViPipe, 0x3045, 0x05);
    imx290_2_write_register (ViPipe, 0x3046, 0x00);
    imx290_2_write_register (ViPipe, 0x304b, 0x0a);
    imx290_2_write_register (ViPipe, 0x305c, 0x18);
    imx290_2_write_register (ViPipe, 0x305d, 0x03);
    imx290_2_write_register (ViPipe, 0x305e, 0x20);
    imx290_2_write_register (ViPipe, 0x305f, 0x01);
    imx290_2_write_register (ViPipe, 0x3070, 0x02);
    imx290_2_write_register (ViPipe, 0x3071, 0x11);
    imx290_2_write_register (ViPipe, 0x309b, 0x10);
    imx290_2_write_register (ViPipe, 0x309c, 0x22);
    imx290_2_write_register (ViPipe, 0x30a2, 0x02);
    imx290_2_write_register (ViPipe, 0x30a6, 0x20);
    imx290_2_write_register (ViPipe, 0x30a8, 0x20);
    imx290_2_write_register (ViPipe, 0x30aa, 0x20);
    imx290_2_write_register (ViPipe, 0x30ac, 0x20);
    imx290_2_write_register (ViPipe, 0x30b0, 0x43);
    imx290_2_write_register (ViPipe, 0x3106, 0x11);
    imx290_2_write_register (ViPipe, 0x3119, 0x9e);
    imx290_2_write_register (ViPipe, 0x311c, 0x1e);
    imx290_2_write_register (ViPipe, 0x311e, 0x08);
    imx290_2_write_register (ViPipe, 0x3128, 0x05);
    imx290_2_write_register (ViPipe, 0x3129, 0x1d);
    imx290_2_write_register (ViPipe, 0x313d, 0x83);
    imx290_2_write_register (ViPipe, 0x3150, 0x03);
    imx290_2_write_register (ViPipe, 0x315e, 0x1a);
    imx290_2_write_register (ViPipe, 0x3164, 0x1a);
    imx290_2_write_register (ViPipe, 0x317c, 0x12);
    imx290_2_write_register (ViPipe, 0x317e, 0x00);
    imx290_2_write_register (ViPipe, 0x31ec, 0x37);
    imx290_2_write_register (ViPipe, 0x32b8, 0x50);
    imx290_2_write_register (ViPipe, 0x32b9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ba, 0x00);
    imx290_2_write_register (ViPipe, 0x32bb, 0x04);
    imx290_2_write_register (ViPipe, 0x32c8, 0x50);
    imx290_2_write_register (ViPipe, 0x32c9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ca, 0x00);
    imx290_2_write_register (ViPipe, 0x32cb, 0x04);
    imx290_2_write_register (ViPipe, 0x332c, 0xd3);
    imx290_2_write_register (ViPipe, 0x332d, 0x10);
    imx290_2_write_register (ViPipe, 0x332e, 0x0d);
    imx290_2_write_register (ViPipe, 0x3358, 0x06);
    imx290_2_write_register (ViPipe, 0x3359, 0xe1);
    imx290_2_write_register (ViPipe, 0x335a, 0x11);
    imx290_2_write_register (ViPipe, 0x3360, 0x1e);
    imx290_2_write_register (ViPipe, 0x3361, 0x61);
    imx290_2_write_register (ViPipe, 0x3362, 0x10);
    imx290_2_write_register (ViPipe, 0x33b0, 0x50);
    imx290_2_write_register (ViPipe, 0x33b2, 0x1a);
    imx290_2_write_register (ViPipe, 0x33b3, 0x04);

    imx290_2_write_register (ViPipe, 0x3418, 0xb2); /**** Y_OUT_SIZE *****/
    imx290_2_write_register (ViPipe, 0x3419, 0x08);

    imx290_2_write_register (ViPipe, 0x3441, 0x0a);
    imx290_2_write_register (ViPipe, 0x3442, 0x0a);
    imx290_2_write_register (ViPipe, 0x3444, 0x20);
    imx290_2_write_register (ViPipe, 0x3445, 0x25);
    imx290_2_write_register (ViPipe, 0x3480, 0x49);

    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 sensor 1080P15fps 10bit 2to1 WDR(30fps->15fps) init success!=====\n");

    return;

}

void imx290_2_wdr_1080p60_2to1_init(VI_PIPE ViPipe)
{
    //dol2 2lane 896Mbps, 12bits official setting
    imx290_2_write_register(ViPipe,0x3003,0x01);
    imx290_2_write_register(ViPipe,0x3002,0x01);
    imx290_2_write_register(ViPipe,0x3000,0x01);
    imx290_2_write_register(ViPipe,0x3005,0x01);
    imx290_2_write_register(ViPipe,0x3007,0x00);
    imx290_2_write_register(ViPipe,0x3009,0x01);
    imx290_2_write_register(ViPipe,0x300A,0xF0);
    imx290_2_write_register(ViPipe,0x300C,0x11);
    imx290_2_write_register(ViPipe,0x300F,0x00);
    imx290_2_write_register(ViPipe,0x3010,0x21);
    imx290_2_write_register(ViPipe,0x3012,0x64);
    imx290_2_write_register(ViPipe,0x3016,0x09);
    imx290_2_write_register(ViPipe,0x3018,0x46);
    imx290_2_write_register(ViPipe,0x3019,0x05);  //vmax 1350
    imx290_2_write_register(ViPipe,0x301C,0x98);
    imx290_2_write_register(ViPipe,0x301D,0x08);  //hmax 2200
    imx290_2_write_register(ViPipe,0x3020,0x02);  //sexp = 92
    imx290_2_write_register(ViPipe,0x3021,0x00);
    imx290_2_write_register(ViPipe,0x3024,0x59);  //lexp = 92*16 = 1520
    imx290_2_write_register(ViPipe,0x3025,0x03);
    imx290_2_write_register(ViPipe,0x3030,0xcd);  //rhs = 2n+5 = 95
    imx290_2_write_register(ViPipe,0x3031,0x01);
    imx290_2_write_register(ViPipe,0x3045,0x05);
    imx290_2_write_register(ViPipe,0x3046,0x01);
    imx290_2_write_register(ViPipe,0x304B,0x0A);
    imx290_2_write_register(ViPipe,0x305C,0x18);
    imx290_2_write_register(ViPipe,0x305D,0x03);
    imx290_2_write_register(ViPipe,0x305E,0x20);
    imx290_2_write_register(ViPipe,0x305F,0x01);
    imx290_2_write_register(ViPipe,0x3070,0x02);
    imx290_2_write_register(ViPipe,0x3071,0x11);
    imx290_2_write_register(ViPipe,0x309B,0x10);
    imx290_2_write_register(ViPipe,0x309C,0x22);
    imx290_2_write_register(ViPipe,0x30A2,0x02);
    imx290_2_write_register(ViPipe,0x30A6,0x20);
    imx290_2_write_register(ViPipe,0x30A8,0x20);
    imx290_2_write_register(ViPipe,0x30AA,0x20);
    imx290_2_write_register(ViPipe,0x30AC,0x20);
    imx290_2_write_register(ViPipe,0x30B0,0x43);
    imx290_2_write_register(ViPipe,0x3106,0x11);
    imx290_2_write_register(ViPipe,0x3119,0x9E);
    imx290_2_write_register(ViPipe,0x311C,0x1E);
    imx290_2_write_register(ViPipe,0x311E,0x08);
    imx290_2_write_register(ViPipe,0x3128,0x05);
    imx290_2_write_register(ViPipe,0x3129,0x00);
    imx290_2_write_register(ViPipe,0x313D,0x83);
    imx290_2_write_register(ViPipe,0x3150,0x03);
    imx290_2_write_register(ViPipe,0x315E,0x1A);
    imx290_2_write_register(ViPipe,0x3164,0x1A);
    imx290_2_write_register(ViPipe,0x317C,0x00);
    imx290_2_write_register(ViPipe,0x317E,0x00);
    imx290_2_write_register(ViPipe,0x31EC,0x0E);
    imx290_2_write_register(ViPipe,0x32B8,0x50);
    imx290_2_write_register(ViPipe,0x32B9,0x10);
    imx290_2_write_register(ViPipe,0x32BA,0x00);
    imx290_2_write_register(ViPipe,0x32BB,0x04);
    imx290_2_write_register(ViPipe,0x32C8,0x50);
    imx290_2_write_register(ViPipe,0x32C9,0x10);
    imx290_2_write_register(ViPipe,0x32CA,0x00);
    imx290_2_write_register(ViPipe,0x32CB,0x04);
    imx290_2_write_register(ViPipe,0x332C,0xD3);
    imx290_2_write_register(ViPipe,0x332D,0x10);
    imx290_2_write_register(ViPipe,0x332E,0x0D);
    imx290_2_write_register(ViPipe,0x3358,0x06);
    imx290_2_write_register(ViPipe,0x3359,0xE1);
    imx290_2_write_register(ViPipe,0x335A,0x11);
    imx290_2_write_register(ViPipe,0x3360,0x1E);
    imx290_2_write_register(ViPipe,0x3361,0x61);
    imx290_2_write_register(ViPipe,0x3362,0x10);
    imx290_2_write_register(ViPipe,0x33B0,0x50);
    imx290_2_write_register(ViPipe,0x33B2,0x1A);
    imx290_2_write_register(ViPipe,0x33B3,0x04);
    imx290_2_write_register(ViPipe,0x3405,0x00);
    imx290_2_write_register(ViPipe,0x3407,0x01);
    imx290_2_write_register(ViPipe,0x3414,0x0A);
    imx290_2_write_register(ViPipe,0x3415,0x00);
    imx290_2_write_register(ViPipe,0x3418,0x76); //pic height 2678
    imx290_2_write_register(ViPipe,0x3419,0x0a); //pic height
    imx290_2_write_register(ViPipe,0x3441,0x0C);
    imx290_2_write_register(ViPipe,0x3442,0x0C);
    imx290_2_write_register(ViPipe,0x3443,0x01);
    imx290_2_write_register(ViPipe,0x3444,0x20);
    imx290_2_write_register(ViPipe,0x3445,0x25);
    imx290_2_write_register(ViPipe,0x3446,0x77);
    imx290_2_write_register(ViPipe,0x3447,0x00);
    imx290_2_write_register(ViPipe,0x3448,0x67);
    imx290_2_write_register(ViPipe,0x3449,0x00);
    imx290_2_write_register(ViPipe,0x344A,0x47);
    imx290_2_write_register(ViPipe,0x344B,0x00);
    imx290_2_write_register(ViPipe,0x344C,0x37);
    imx290_2_write_register(ViPipe,0x344D,0x00);
    imx290_2_write_register(ViPipe,0x344E,0x3F);
    imx290_2_write_register(ViPipe,0x344F,0x00);
    imx290_2_write_register(ViPipe,0x3450,0xFF);
    imx290_2_write_register(ViPipe,0x3451,0x00);
    imx290_2_write_register(ViPipe,0x3452,0x3F);
    imx290_2_write_register(ViPipe,0x3453,0x00);
    imx290_2_write_register(ViPipe,0x3454,0x37);
    imx290_2_write_register(ViPipe,0x3472,0xA0);  //pic width 1952
    imx290_2_write_register(ViPipe,0x3473,0x07);  //pic width
    imx290_2_write_register(ViPipe,0x347B,0x23);
    imx290_2_write_register(ViPipe,0x3010,0x61); //set dol2 long exp gain enable
    imx290_2_write_register(ViPipe,0x30f0,0x64); //set dol2 shot exp gain enable


    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 sensor 1080P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    return;

}

void imx290_2_wdr_1080p30_3to1_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe, 0x3000, 0x01); /* standby */
    imx290_2_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //12bit
    imx290_2_write_register (ViPipe, 0x3007, 0x00);
    imx290_2_write_register (ViPipe, 0x300c, 0x21);
    imx290_2_write_register (ViPipe, 0x300f, 0x00);
    imx290_2_write_register (ViPipe, 0x3010, 0x21);
    imx290_2_write_register (ViPipe, 0x3012, 0x64);
    imx290_2_write_register (ViPipe, 0x3016, 0x09);
    imx290_2_write_register (ViPipe, 0x3020, 0x04);
    imx290_2_write_register (ViPipe, 0x3021, 0x00);
    imx290_2_write_register (ViPipe, 0x3024, 0xF2);
    imx290_2_write_register (ViPipe, 0x3025, 0x01);
    imx290_2_write_register (ViPipe, 0x3028, 0x57);
    imx290_2_write_register (ViPipe, 0x3029, 0x02);
    imx290_2_write_register (ViPipe, 0x3030, 0xED);
    imx290_2_write_register (ViPipe, 0x3031, 0x01);
    imx290_2_write_register (ViPipe, 0x3034, 0x30);
    imx290_2_write_register (ViPipe, 0x3035, 0x02);
    imx290_2_write_register (ViPipe, 0x3045, 0x05);
    imx290_2_write_register (ViPipe, 0x304b, 0x0a);
    imx290_2_write_register (ViPipe, 0x305c, 0x18);
    imx290_2_write_register (ViPipe, 0x305d, 0x03);
    imx290_2_write_register (ViPipe, 0x305e, 0x20);
    imx290_2_write_register (ViPipe, 0x305f, 0x01);
    imx290_2_write_register (ViPipe, 0x3070, 0x02);
    imx290_2_write_register (ViPipe, 0x3071, 0x11);
    imx290_2_write_register (ViPipe, 0x309b, 0x10);
    imx290_2_write_register (ViPipe, 0x309c, 0x22);
    imx290_2_write_register (ViPipe, 0x30a2, 0x02);
    imx290_2_write_register (ViPipe, 0x30a6, 0x20);
    imx290_2_write_register (ViPipe, 0x30a8, 0x20);
    imx290_2_write_register (ViPipe, 0x30aa, 0x20);
    imx290_2_write_register (ViPipe, 0x30ac, 0x20);
    imx290_2_write_register (ViPipe, 0x30b0, 0x43);
    imx290_2_write_register (ViPipe, 0x3106, 0x33);
    imx290_2_write_register (ViPipe, 0x3119, 0x9e);
    imx290_2_write_register (ViPipe, 0x311c, 0x1e);
    imx290_2_write_register (ViPipe, 0x311e, 0x08);
    imx290_2_write_register (ViPipe, 0x3128, 0x05);
    imx290_2_write_register (ViPipe, 0x313d, 0x83);
    imx290_2_write_register (ViPipe, 0x3150, 0x03);
    imx290_2_write_register (ViPipe, 0x315e, 0x1a);
    imx290_2_write_register (ViPipe, 0x3164, 0x1a);
    imx290_2_write_register (ViPipe, 0x317e, 0x00);
    imx290_2_write_register (ViPipe, 0x32b8, 0x50);
    imx290_2_write_register (ViPipe, 0x32b9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ba, 0x00);
    imx290_2_write_register (ViPipe, 0x32bb, 0x04);
    imx290_2_write_register (ViPipe, 0x32c8, 0x50);
    imx290_2_write_register (ViPipe, 0x32c9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ca, 0x00);
    imx290_2_write_register (ViPipe, 0x32cb, 0x04);
    imx290_2_write_register (ViPipe, 0x332c, 0xd3);
    imx290_2_write_register (ViPipe, 0x332d, 0x10);
    imx290_2_write_register (ViPipe, 0x332e, 0x0d);
    imx290_2_write_register (ViPipe, 0x3358, 0x06);
    imx290_2_write_register (ViPipe, 0x3359, 0xe1);
    imx290_2_write_register (ViPipe, 0x335a, 0x11);
    imx290_2_write_register (ViPipe, 0x3360, 0x1e);
    imx290_2_write_register (ViPipe, 0x3361, 0x61);
    imx290_2_write_register (ViPipe, 0x3362, 0x10);
    imx290_2_write_register (ViPipe, 0x33b0, 0x50);
    imx290_2_write_register (ViPipe, 0x33b2, 0x1a);
    imx290_2_write_register (ViPipe, 0x33b3, 0x04);

    imx290_2_write_register (ViPipe, 0x3418, 0x24); /**** Y_OUT_SIZE *****/
    imx290_2_write_register (ViPipe, 0x3419, 0x0F);

    imx290_2_write_register (ViPipe, 0x3444, 0x20);
    imx290_2_write_register (ViPipe, 0x3445, 0x25);
    imx290_2_write_register (ViPipe, 0x3480, 0x49);

    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 imx290_2 1080P15fps 12bit 3to1 WDR(30fps->7p5fps) init success!=====\n");

    return;

}

void imx290_2_wdr_720p60_2to1_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe, 0x3000, 0x01); /* standby */
    imx290_2_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //12bit
    imx290_2_write_register (ViPipe, 0x3005, 0x01);
    imx290_2_write_register (ViPipe, 0x3007, 0x10);
    imx290_2_write_register (ViPipe, 0x3009, 0x01);
    imx290_2_write_register (ViPipe, 0x300a, 0xf0);
    imx290_2_write_register (ViPipe, 0x300c, 0x11);
    imx290_2_write_register (ViPipe, 0x300f, 0x00);
    imx290_2_write_register (ViPipe, 0x3010, 0x21);
    imx290_2_write_register (ViPipe, 0x3012, 0x64);
    imx290_2_write_register (ViPipe, 0x3016, 0x09);
    imx290_2_write_register (ViPipe, 0x3018, 0xee);
    imx290_2_write_register (ViPipe, 0x3019, 0x02);
    imx290_2_write_register (ViPipe, 0x301c, 0xe4);
    imx290_2_write_register (ViPipe, 0x301d, 0x0c);
    imx290_2_write_register (ViPipe, 0x3045, 0x05);
    imx290_2_write_register (ViPipe, 0x3046, 0x01);
    imx290_2_write_register (ViPipe, 0x304b, 0x0a);

    imx290_2_write_register (ViPipe, 0x305c, 0x20); //INCKSEL1
    imx290_2_write_register (ViPipe, 0x305d, 0x03); //INCKSEL2
    imx290_2_write_register (ViPipe, 0x305e, 0x20); //INCKSEL3
    imx290_2_write_register (ViPipe, 0x305f, 0x01); //INCKSEL4

    imx290_2_write_register (ViPipe, 0x3070, 0x02);
    imx290_2_write_register (ViPipe, 0x3071, 0x11);
    imx290_2_write_register (ViPipe, 0x309b, 0x10);
    imx290_2_write_register (ViPipe, 0x309c, 0x22);
    imx290_2_write_register (ViPipe, 0x30a2, 0x02);
    imx290_2_write_register (ViPipe, 0x30a6, 0x20);
    imx290_2_write_register (ViPipe, 0x30a8, 0x20);
    imx290_2_write_register (ViPipe, 0x30aa, 0x20);
    imx290_2_write_register (ViPipe, 0x30ac, 0x20);
    imx290_2_write_register (ViPipe, 0x30b0, 0x43);

    //Add
    imx290_2_write_register (ViPipe, 0x3106, 0x11);

    imx290_2_write_register (ViPipe, 0x3119, 0x9e);
    imx290_2_write_register (ViPipe, 0x311c, 0x1e);
    imx290_2_write_register (ViPipe, 0x311e, 0x08);
    imx290_2_write_register (ViPipe, 0x3128, 0x05);
    imx290_2_write_register (ViPipe, 0x3129, 0x00);
    imx290_2_write_register (ViPipe, 0x313d, 0x83);
    imx290_2_write_register (ViPipe, 0x3150, 0x03);
    imx290_2_write_register (ViPipe, 0x315e, 0x1a);
    imx290_2_write_register (ViPipe, 0x3164, 0x1a);
    imx290_2_write_register (ViPipe, 0x317c, 0x00);
    imx290_2_write_register (ViPipe, 0x317e, 0x00);
    imx290_2_write_register (ViPipe, 0x31ec, 0x00);

    imx290_2_write_register (ViPipe, 0x32b8, 0x50);
    imx290_2_write_register (ViPipe, 0x32b9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ba, 0x00);
    imx290_2_write_register (ViPipe, 0x32bb, 0x04);
    imx290_2_write_register (ViPipe, 0x32c8, 0x50);
    imx290_2_write_register (ViPipe, 0x32c9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ca, 0x00);
    imx290_2_write_register (ViPipe, 0x32cb, 0x04);

    imx290_2_write_register (ViPipe, 0x332c, 0xd3);
    imx290_2_write_register (ViPipe, 0x332d, 0x10);
    imx290_2_write_register (ViPipe, 0x332e, 0x0d);
    imx290_2_write_register (ViPipe, 0x3358, 0x06);
    imx290_2_write_register (ViPipe, 0x3359, 0xe1);
    imx290_2_write_register (ViPipe, 0x335a, 0x11);
    imx290_2_write_register (ViPipe, 0x3360, 0x1e);
    imx290_2_write_register (ViPipe, 0x3361, 0x61);
    imx290_2_write_register (ViPipe, 0x3362, 0x10);
    imx290_2_write_register (ViPipe, 0x33b0, 0x50);
    imx290_2_write_register (ViPipe, 0x33b2, 0x1a);
    imx290_2_write_register (ViPipe, 0x33b3, 0x04);

    imx290_2_write_register (ViPipe, 0x3405, 0x10);
    imx290_2_write_register (ViPipe, 0x3407, 0x03);
    imx290_2_write_register (ViPipe, 0x3414, 0x04);
    imx290_2_write_register (ViPipe, 0x3418, 0xc6);
    imx290_2_write_register (ViPipe, 0x3419, 0x05);
    imx290_2_write_register (ViPipe, 0x3441, 0x0c);
    imx290_2_write_register (ViPipe, 0x3442, 0x0c);
    imx290_2_write_register (ViPipe, 0x3443, 0x03);
    imx290_2_write_register (ViPipe, 0x3444, 0x20);
    imx290_2_write_register (ViPipe, 0x3445, 0x25);
    imx290_2_write_register (ViPipe, 0x3446, 0x4f);
    imx290_2_write_register (ViPipe, 0x3447, 0x00);
    imx290_2_write_register (ViPipe, 0x3448, 0x2f);
    imx290_2_write_register (ViPipe, 0x3449, 0x00);
    imx290_2_write_register (ViPipe, 0x344a, 0x17);
    imx290_2_write_register (ViPipe, 0x344b, 0x00);
    imx290_2_write_register (ViPipe, 0x344c, 0x17);
    imx290_2_write_register (ViPipe, 0x344d, 0x00);
    imx290_2_write_register (ViPipe, 0x344e, 0x17);
    imx290_2_write_register (ViPipe, 0x344f, 0x00);
    imx290_2_write_register (ViPipe, 0x3450, 0x57);
    imx290_2_write_register (ViPipe, 0x3451, 0x00);
    imx290_2_write_register (ViPipe, 0x3452, 0x17);
    imx290_2_write_register (ViPipe, 0x3453, 0x00);
    imx290_2_write_register (ViPipe, 0x3454, 0x17);
    imx290_2_write_register (ViPipe, 0x3455, 0x00);
    imx290_2_write_register (ViPipe, 0x3472, 0x1c);
    imx290_2_write_register (ViPipe, 0x3473, 0x05);
    imx290_2_write_register (ViPipe, 0x3480, 0x49);

    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 imx290_2 720P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    return;
}

void imx290_2_wdr_720p60_3to1_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe, 0x3000, 0x01); /* standby */
    imx290_2_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //12bit
    imx290_2_write_register (ViPipe, 0x3005, 0x01);
    imx290_2_write_register (ViPipe, 0x3007, 0x10);
    imx290_2_write_register (ViPipe, 0x3009, 0x01);
    imx290_2_write_register (ViPipe, 0x300a, 0xf0);
    imx290_2_write_register (ViPipe, 0x300c, 0x31);
    imx290_2_write_register (ViPipe, 0x300f, 0x00);
    imx290_2_write_register (ViPipe, 0x3010, 0x21);
    imx290_2_write_register (ViPipe, 0x3012, 0x64);
    imx290_2_write_register (ViPipe, 0x3016, 0x09);
    imx290_2_write_register (ViPipe, 0x3018, 0xee);
    imx290_2_write_register (ViPipe, 0x3019, 0x02);
    imx290_2_write_register (ViPipe, 0x301c, 0xe4);
    imx290_2_write_register (ViPipe, 0x301d, 0x0c);
    imx290_2_write_register (ViPipe, 0x3045, 0x05);
    imx290_2_write_register (ViPipe, 0x3046, 0x01);
    imx290_2_write_register (ViPipe, 0x304b, 0x0a);

    imx290_2_write_register (ViPipe, 0x305c, 0x20); //INCKSEL1
    imx290_2_write_register (ViPipe, 0x305d, 0x03); //INCKSEL2
    imx290_2_write_register (ViPipe, 0x305e, 0x20); //INCKSEL3
    imx290_2_write_register (ViPipe, 0x305f, 0x01); //INCKSEL4

    imx290_2_write_register (ViPipe, 0x3070, 0x02);
    imx290_2_write_register (ViPipe, 0x3071, 0x11);
    imx290_2_write_register (ViPipe, 0x309b, 0x10);
    imx290_2_write_register (ViPipe, 0x309c, 0x22);
    imx290_2_write_register (ViPipe, 0x30a2, 0x02);
    imx290_2_write_register (ViPipe, 0x30a6, 0x20);
    imx290_2_write_register (ViPipe, 0x30a8, 0x20);
    imx290_2_write_register (ViPipe, 0x30aa, 0x20);
    imx290_2_write_register (ViPipe, 0x30ac, 0x20);
    imx290_2_write_register (ViPipe, 0x30b0, 0x43);

    //Add
    imx290_2_write_register (ViPipe, 0x3106, 0x33);

    imx290_2_write_register (ViPipe, 0x3119, 0x9e);
    imx290_2_write_register (ViPipe, 0x311c, 0x1e);
    imx290_2_write_register (ViPipe, 0x311e, 0x08);
    imx290_2_write_register (ViPipe, 0x3128, 0x05);
    imx290_2_write_register (ViPipe, 0x3129, 0x00);
    imx290_2_write_register (ViPipe, 0x313d, 0x83);
    imx290_2_write_register (ViPipe, 0x3150, 0x03);
    imx290_2_write_register (ViPipe, 0x315e, 0x1a);
    imx290_2_write_register (ViPipe, 0x3164, 0x1a);
    imx290_2_write_register (ViPipe, 0x317c, 0x00);
    imx290_2_write_register (ViPipe, 0x317e, 0x00);
    imx290_2_write_register (ViPipe, 0x31ec, 0x00);

    imx290_2_write_register (ViPipe, 0x32b8, 0x50);
    imx290_2_write_register (ViPipe, 0x32b9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ba, 0x00);
    imx290_2_write_register (ViPipe, 0x32bb, 0x04);
    imx290_2_write_register (ViPipe, 0x32c8, 0x50);
    imx290_2_write_register (ViPipe, 0x32c9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ca, 0x00);
    imx290_2_write_register (ViPipe, 0x32cb, 0x04);

    imx290_2_write_register (ViPipe, 0x332c, 0xd3);
    imx290_2_write_register (ViPipe, 0x332d, 0x10);
    imx290_2_write_register (ViPipe, 0x332e, 0x0d);
    imx290_2_write_register (ViPipe, 0x3358, 0x06);
    imx290_2_write_register (ViPipe, 0x3359, 0xe1);
    imx290_2_write_register (ViPipe, 0x335a, 0x11);
    imx290_2_write_register (ViPipe, 0x3360, 0x1e);
    imx290_2_write_register (ViPipe, 0x3361, 0x61);
    imx290_2_write_register (ViPipe, 0x3362, 0x10);
    imx290_2_write_register (ViPipe, 0x33b0, 0x50);
    imx290_2_write_register (ViPipe, 0x33b2, 0x1a);
    imx290_2_write_register (ViPipe, 0x33b3, 0x04);

    imx290_2_write_register (ViPipe, 0x3405, 0x10);
    imx290_2_write_register (ViPipe, 0x3407, 0x03);
    imx290_2_write_register (ViPipe, 0x3414, 0x04);
    imx290_2_write_register (ViPipe, 0x3418, 0xb5);
    imx290_2_write_register (ViPipe, 0x3419, 0x08);
    imx290_2_write_register (ViPipe, 0x3441, 0x0c);
    imx290_2_write_register (ViPipe, 0x3442, 0x0c);
    imx290_2_write_register (ViPipe, 0x3443, 0x03);
    imx290_2_write_register (ViPipe, 0x3444, 0x20);
    imx290_2_write_register (ViPipe, 0x3445, 0x25);
    imx290_2_write_register (ViPipe, 0x3446, 0x4f);
    imx290_2_write_register (ViPipe, 0x3447, 0x00);
    imx290_2_write_register (ViPipe, 0x3448, 0x2f);
    imx290_2_write_register (ViPipe, 0x3449, 0x00);
    imx290_2_write_register (ViPipe, 0x344a, 0x17);
    imx290_2_write_register (ViPipe, 0x344b, 0x00);
    imx290_2_write_register (ViPipe, 0x344c, 0x17);
    imx290_2_write_register (ViPipe, 0x344d, 0x00);
    imx290_2_write_register (ViPipe, 0x344e, 0x17);
    imx290_2_write_register (ViPipe, 0x344f, 0x00);
    imx290_2_write_register (ViPipe, 0x3450, 0x57);
    imx290_2_write_register (ViPipe, 0x3451, 0x00);
    imx290_2_write_register (ViPipe, 0x3452, 0x17);
    imx290_2_write_register (ViPipe, 0x3453, 0x00);
    imx290_2_write_register (ViPipe, 0x3454, 0x17);
    imx290_2_write_register (ViPipe, 0x3455, 0x00);
    imx290_2_write_register (ViPipe, 0x3472, 0x1c);
    imx290_2_write_register (ViPipe, 0x3473, 0x05);
    imx290_2_write_register (ViPipe, 0x3480, 0x49);

    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 imx290_2 720P15fps 12bit 3to1 WDR(60fps->15fps) init success!=====\n");
    return;
}

void imx290_2_wdr_1080p120_2to1_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe, 0x3000, 0x01); /* standby */
    imx290_2_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    imx290_2_write_register (ViPipe, 0x3005, 0x00);
    imx290_2_write_register (ViPipe, 0x3007, 0x00);
    imx290_2_write_register (ViPipe, 0x3009, 0x00);
    imx290_2_write_register (ViPipe, 0x300a, 0x3c);
    imx290_2_write_register (ViPipe, 0x300c, 0x11);
    imx290_2_write_register (ViPipe, 0x300f, 0x00);
    imx290_2_write_register (ViPipe, 0x3010, 0x21);
    imx290_2_write_register (ViPipe, 0x3012, 0x64);
    imx290_2_write_register (ViPipe, 0x3016, 0x09);
    imx290_2_write_register (ViPipe, 0x3018, 0x65);
    imx290_2_write_register (ViPipe, 0x3019, 0x04);
    imx290_2_write_register (ViPipe, 0x301c, 0xF6);
    imx290_2_write_register (ViPipe, 0x301d, 0x03);
    imx290_2_write_register (ViPipe, 0x3020, 0x02);
    imx290_2_write_register (ViPipe, 0x3024, 0xc9);
    imx290_2_write_register (ViPipe, 0x3030, 0x0b);
    imx290_2_write_register (ViPipe, 0x3045, 0x05);
    imx290_2_write_register (ViPipe, 0x3046, 0x00);
    imx290_2_write_register (ViPipe, 0x304b, 0x0a);
    imx290_2_write_register (ViPipe, 0x305c, 0x18);
    imx290_2_write_register (ViPipe, 0x305d, 0x03);
    imx290_2_write_register (ViPipe, 0x305e, 0x20);
    imx290_2_write_register (ViPipe, 0x305f, 0x01);
    imx290_2_write_register (ViPipe, 0x3070, 0x02);
    imx290_2_write_register (ViPipe, 0x3071, 0x11);
    imx290_2_write_register (ViPipe, 0x309b, 0x10);
    imx290_2_write_register (ViPipe, 0x309c, 0x22);
    imx290_2_write_register (ViPipe, 0x30a2, 0x02);
    imx290_2_write_register (ViPipe, 0x30a6, 0x20);
    imx290_2_write_register (ViPipe, 0x30a8, 0x20);
    imx290_2_write_register (ViPipe, 0x30aa, 0x20);
    imx290_2_write_register (ViPipe, 0x30ac, 0x20);
    imx290_2_write_register (ViPipe, 0x30b0, 0x43);
    imx290_2_write_register (ViPipe, 0x3106, 0x11);
    imx290_2_write_register (ViPipe, 0x3119, 0x9e);
    imx290_2_write_register (ViPipe, 0x311c, 0x1e);
    imx290_2_write_register (ViPipe, 0x311e, 0x08);
    imx290_2_write_register (ViPipe, 0x3128, 0x05);
    imx290_2_write_register (ViPipe, 0x3129, 0x1d);
    imx290_2_write_register (ViPipe, 0x313d, 0x83);
    imx290_2_write_register (ViPipe, 0x3150, 0x03);
    imx290_2_write_register (ViPipe, 0x315e, 0x1a);
    imx290_2_write_register (ViPipe, 0x3164, 0x1a);
    imx290_2_write_register (ViPipe, 0x317c, 0x12);
    imx290_2_write_register (ViPipe, 0x317e, 0x00);
    imx290_2_write_register (ViPipe, 0x31ec, 0x37);

    imx290_2_write_register (ViPipe, 0x32b8, 0x50);
    imx290_2_write_register (ViPipe, 0x32b9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ba, 0x00);
    imx290_2_write_register (ViPipe, 0x32bb, 0x04);
    imx290_2_write_register (ViPipe, 0x32c8, 0x50);
    imx290_2_write_register (ViPipe, 0x32c9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ca, 0x00);
    imx290_2_write_register (ViPipe, 0x32cb, 0x04);

    imx290_2_write_register (ViPipe, 0x332c, 0xd3);
    imx290_2_write_register (ViPipe, 0x332d, 0x10);
    imx290_2_write_register (ViPipe, 0x332e, 0x0d);
    imx290_2_write_register (ViPipe, 0x3358, 0x06);
    imx290_2_write_register (ViPipe, 0x3359, 0xe1);
    imx290_2_write_register (ViPipe, 0x335a, 0x11);
    imx290_2_write_register (ViPipe, 0x3360, 0x1e);
    imx290_2_write_register (ViPipe, 0x3361, 0x61);
    imx290_2_write_register (ViPipe, 0x3362, 0x10);
    imx290_2_write_register (ViPipe, 0x33b0, 0x50);
    imx290_2_write_register (ViPipe, 0x33b2, 0x1a);
    imx290_2_write_register (ViPipe, 0x33b3, 0x04);

    imx290_2_write_register (ViPipe, 0x3405, 0x00);
    imx290_2_write_register (ViPipe, 0x3407, 0x03);
    imx290_2_write_register (ViPipe, 0x3414, 0x0a);
    imx290_2_write_register (ViPipe, 0x3418, 0xb2);
    imx290_2_write_register (ViPipe, 0x3419, 0x08);
    imx290_2_write_register (ViPipe, 0x3441, 0x0a);
    imx290_2_write_register (ViPipe, 0x3442, 0x0a);
    imx290_2_write_register (ViPipe, 0x3443, 0x03);
    imx290_2_write_register (ViPipe, 0x3444, 0x20);
    imx290_2_write_register (ViPipe, 0x3445, 0x25);
    imx290_2_write_register (ViPipe, 0x3446, 0x77);
    imx290_2_write_register (ViPipe, 0x3447, 0x00);
    imx290_2_write_register (ViPipe, 0x3448, 0x67);
    imx290_2_write_register (ViPipe, 0x3449, 0x00);
    imx290_2_write_register (ViPipe, 0x344a, 0x47);
    imx290_2_write_register (ViPipe, 0x344b, 0x00);
    imx290_2_write_register (ViPipe, 0x344c, 0x37);
    imx290_2_write_register (ViPipe, 0x344d, 0x00);
    imx290_2_write_register (ViPipe, 0x344e, 0x3f);
    imx290_2_write_register (ViPipe, 0x344f, 0x00);
    imx290_2_write_register (ViPipe, 0x3450, 0xff);
    imx290_2_write_register (ViPipe, 0x3451, 0x00);
    imx290_2_write_register (ViPipe, 0x3452, 0x3f);
    imx290_2_write_register (ViPipe, 0x3453, 0x00);
    imx290_2_write_register (ViPipe, 0x3454, 0x37);
    imx290_2_write_register (ViPipe, 0x3455, 0x00);
    imx290_2_write_register (ViPipe, 0x3472, 0x9c);
    imx290_2_write_register (ViPipe, 0x3473, 0x07);
    imx290_2_write_register (ViPipe, 0x3480, 0x49);

    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 imx290_2 1080P60fps 10bit 2to1 WDR(120fps->60fps) init success!=====\n");
    return;
}

void imx290_2_wdr_1080p120_3to1_init(VI_PIPE ViPipe)
{
    imx290_2_write_register (ViPipe, 0x3000, 0x01); /* standby */
    imx290_2_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    imx290_2_write_register (ViPipe, 0x3005, 0x00);
    imx290_2_write_register (ViPipe, 0x3007, 0x00);
    imx290_2_write_register (ViPipe, 0x3009, 0x00);
    imx290_2_write_register (ViPipe, 0x300a, 0x3c);
    imx290_2_write_register (ViPipe, 0x300c, 0x21);
    imx290_2_write_register (ViPipe, 0x300f, 0x00);
    imx290_2_write_register (ViPipe, 0x3010, 0x21);
    imx290_2_write_register (ViPipe, 0x3012, 0x64);
    imx290_2_write_register (ViPipe, 0x3016, 0x09);
    imx290_2_write_register (ViPipe, 0x3018, 0x65);
    imx290_2_write_register (ViPipe, 0x3019, 0x04);
    imx290_2_write_register (ViPipe, 0x301c, 0x4c);
    imx290_2_write_register (ViPipe, 0x301d, 0x04);

#if 1
    imx290_2_write_register (ViPipe, 0x3020, 0x04); /* SHS1 */
    imx290_2_write_register (ViPipe, 0x3021, 0x00);
    //imx290_2_write_register (ViPipe,0x3022, 0x00);
    imx290_2_write_register (ViPipe, 0x3024, 0xf2); /* SHS2 */
    imx290_2_write_register (ViPipe, 0x3025, 0x01);
    //imx290_2_write_register (ViPipe,0x3025, 0x00);
    imx290_2_write_register (ViPipe, 0x3028, 0x57); /* SHS3 */
    imx290_2_write_register (ViPipe, 0x3029, 0x02);
    //imx290_2_write_register (ViPipe,0x302A, 0x00);
    imx290_2_write_register (ViPipe, 0x3030, 0xed); /* RHS1 */
    imx290_2_write_register (ViPipe, 0x3031, 0x01);
    imx290_2_write_register (ViPipe, 0x3034, 0x30); /* RHS2 */
    imx290_2_write_register (ViPipe, 0x3035, 0x02);
#else
    imx290_2_write_register (ViPipe, 0x3020, 0x04);
    imx290_2_write_register (ViPipe, 0x3024, 0x89);
    imx290_2_write_register (ViPipe, 0x3028, 0x93);
    imx290_2_write_register (ViPipe, 0x3029, 0x01);
    imx290_2_write_register (ViPipe, 0x3030, 0x85);
    imx290_2_write_register (ViPipe, 0x3034, 0x92);
#endif

    imx290_2_write_register (ViPipe, 0x3045, 0x05);
    imx290_2_write_register (ViPipe, 0x3046, 0x00);
    imx290_2_write_register (ViPipe, 0x304b, 0x0a);
    imx290_2_write_register (ViPipe, 0x305c, 0x18);
    imx290_2_write_register (ViPipe, 0x305d, 0x03);
    imx290_2_write_register (ViPipe, 0x305e, 0x20);
    imx290_2_write_register (ViPipe, 0x305f, 0x01);
    imx290_2_write_register (ViPipe, 0x3070, 0x02);
    imx290_2_write_register (ViPipe, 0x3071, 0x11);
    imx290_2_write_register (ViPipe, 0x309b, 0x10);
    imx290_2_write_register (ViPipe, 0x309c, 0x22);
    imx290_2_write_register (ViPipe, 0x30a2, 0x02);
    imx290_2_write_register (ViPipe, 0x30a6, 0x20);
    imx290_2_write_register (ViPipe, 0x30a8, 0x20);
    imx290_2_write_register (ViPipe, 0x30aa, 0x20);
    imx290_2_write_register (ViPipe, 0x30ac, 0x20);
    imx290_2_write_register (ViPipe, 0x30b0, 0x43);
    imx290_2_write_register (ViPipe, 0x3106, 0x33);
    imx290_2_write_register (ViPipe, 0x3119, 0x9e);
    imx290_2_write_register (ViPipe, 0x311c, 0x1e);
    imx290_2_write_register (ViPipe, 0x311e, 0x08);
    imx290_2_write_register (ViPipe, 0x3128, 0x05);
    imx290_2_write_register (ViPipe, 0x3129, 0x1d);
    imx290_2_write_register (ViPipe, 0x313d, 0x83);
    imx290_2_write_register (ViPipe, 0x3150, 0x03);
    imx290_2_write_register (ViPipe, 0x315e, 0x1a);
    imx290_2_write_register (ViPipe, 0x3164, 0x1a);
    imx290_2_write_register (ViPipe, 0x317c, 0x12);
    imx290_2_write_register (ViPipe, 0x317e, 0x00);
    imx290_2_write_register (ViPipe, 0x31ec, 0x37);

    imx290_2_write_register (ViPipe, 0x32b8, 0x50);
    imx290_2_write_register (ViPipe, 0x32b9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ba, 0x00);
    imx290_2_write_register (ViPipe, 0x32bb, 0x04);
    imx290_2_write_register (ViPipe, 0x32c8, 0x50);
    imx290_2_write_register (ViPipe, 0x32c9, 0x10);
    imx290_2_write_register (ViPipe, 0x32ca, 0x00);
    imx290_2_write_register (ViPipe, 0x32cb, 0x04);

    imx290_2_write_register (ViPipe, 0x332c, 0xd3);
    imx290_2_write_register (ViPipe, 0x332d, 0x10);
    imx290_2_write_register (ViPipe, 0x332e, 0x0d);
    imx290_2_write_register (ViPipe, 0x3358, 0x06);
    imx290_2_write_register (ViPipe, 0x3359, 0xe1);
    imx290_2_write_register (ViPipe, 0x335a, 0x11);
    imx290_2_write_register (ViPipe, 0x3360, 0x1e);
    imx290_2_write_register (ViPipe, 0x3361, 0x61);
    imx290_2_write_register (ViPipe, 0x3362, 0x10);
    imx290_2_write_register (ViPipe, 0x33b0, 0x50);
    imx290_2_write_register (ViPipe, 0x33b2, 0x1a);
    imx290_2_write_register (ViPipe, 0x33b3, 0x04);

    imx290_2_write_register (ViPipe, 0x3405, 0x00);
    imx290_2_write_register (ViPipe, 0x3407, 0x03);
    imx290_2_write_register (ViPipe, 0x3414, 0x0a);
    imx290_2_write_register (ViPipe, 0x3418, 0x55);
    imx290_2_write_register (ViPipe, 0x3419, 0x11);
    imx290_2_write_register (ViPipe, 0x3441, 0x0a);
    imx290_2_write_register (ViPipe, 0x3442, 0x0a);
    imx290_2_write_register (ViPipe, 0x3443, 0x03);
    imx290_2_write_register (ViPipe, 0x3444, 0x20);
    imx290_2_write_register (ViPipe, 0x3445, 0x25);
    imx290_2_write_register (ViPipe, 0x3446, 0x77);
    imx290_2_write_register (ViPipe, 0x3447, 0x00);
    imx290_2_write_register (ViPipe, 0x3448, 0x67);
    imx290_2_write_register (ViPipe, 0x3449, 0x00);
    imx290_2_write_register (ViPipe, 0x344a, 0x47);
    imx290_2_write_register (ViPipe, 0x344b, 0x00);
    imx290_2_write_register (ViPipe, 0x344c, 0x37);
    imx290_2_write_register (ViPipe, 0x344d, 0x00);
    imx290_2_write_register (ViPipe, 0x344e, 0x3f);
    imx290_2_write_register (ViPipe, 0x344f, 0x00);
    imx290_2_write_register (ViPipe, 0x3450, 0xff);
    imx290_2_write_register (ViPipe, 0x3451, 0x00);
    imx290_2_write_register (ViPipe, 0x3452, 0x3f);
    imx290_2_write_register (ViPipe, 0x3453, 0x00);
    imx290_2_write_register (ViPipe, 0x3454, 0x37);
    imx290_2_write_register (ViPipe, 0x3455, 0x00);
    imx290_2_write_register (ViPipe, 0x3472, 0x9c);
    imx290_2_write_register (ViPipe, 0x3473, 0x07);
    imx290_2_write_register (ViPipe, 0x3480, 0x49);

    imx290_2_default_reg_init(ViPipe);
    imx290_2_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx290_2_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Imx290_2 imx290_2 1080P30fps 10bit 3to1 WDR(120fps->30fps) init success!=====\n");

    return;
}
