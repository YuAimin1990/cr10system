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

const unsigned char sc910gs_i2c_addr     =    0x60;  //0x32 for sid low  ; 0x30 for sid high   /* I2C Address of sc910gs */
const unsigned int  sc910gs_addr_byte    =    2;
const unsigned int  sc910gs_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastsc910gs[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunsc910gsBusInfo[];

int sc910gs_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunsc910gsBusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	printf("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (sc910gs_i2c_addr >> 1));
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

int sc910gs_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int sc910gs_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int sc910gs_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = sc910gs_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sc910gs_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sc910gs_data_byte;

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

    if (sc910gs_addr_byte == 2)
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

    if (sc910gs_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, sc910gs_addr_byte + sc910gs_data_byte);
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

void sc910gs_prog(VI_PIPE ViPipe, int *rom)
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
            sc910gs_write_register(ViPipe, addr, data);
        }
    }
}

void sc910gs_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void sc910gs_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define sc910gs_FLIP_MIRRO (0x3221)

int sc910gs_flip_off_mirro_off(VI_PIPE ViPipe)
{
    sc910gs_write_register(ViPipe, sc910gs_FLIP_MIRRO, 0x0);
    return 0;
}

int sc910gs_flip_on_mirro_off(VI_PIPE ViPipe)
{
    sc910gs_write_register(ViPipe, sc910gs_FLIP_MIRRO, 0xe0);
    return 0;
}

int sc910gs_flip_off_mirro_on(VI_PIPE ViPipe)
{
    sc910gs_write_register(ViPipe, sc910gs_FLIP_MIRRO, 0x6);
    return 0;
}

int sc910gs_flip_on_mirro_on(VI_PIPE ViPipe)
{
    sc910gs_write_register(ViPipe, sc910gs_FLIP_MIRRO, 0xe6);
    return 0;
}

#define sc910gs_SENSOR_9K_20FPS_LINEAR_MODE      (1)


void sc910gs_linear_9M_init(VI_PIPE ViPipe);



void sc910gs_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastsc910gs[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //printf("set reg[0x%x]: 0x%x \n", g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //printf("set reg[0x%x]: 0x%x \n", g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            sc910gs_write_register(ViPipe, g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void sc910gs_init(VI_PIPE ViPipe)
{
    // WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    // AR_U8            u8ImgMode;

    bInit       = g_pastsc910gs[ViPipe]->bInit;
    // enWDRMode   = g_pastsc910gs[ViPipe]->enWDRMode;
    // u8ImgMode   = g_pastsc910gs[ViPipe]->u8ImgMode;

    sc910gs_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {       
            sc910gs_linear_9M_init(ViPipe);
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {     
            sc910gs_linear_9M_init(ViPipe);
    }

    // for (i = 0; i < g_pastsc910gs[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     sc910gs_write_register(ViPipe, g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc910gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastsc910gs[ViPipe]->bInit = AR_TRUE;
    return ;
}

void sc910gs_exit(VI_PIPE ViPipe)
{
    sc910gs_i2c_exit(ViPipe);

    return;
}


/* 1C4D 600Mbps 10bit 3840*2336 20fps */
void sc910gs_linear_9M_init(VI_PIPE ViPipe)
{
    sc910gs_write_register (ViPipe,0x0103,0x01);
    sc910gs_write_register (ViPipe,0x0100,0x00);
    sc910gs_write_register (ViPipe,0x36e9,0x80);
    sc910gs_write_register (ViPipe,0x37f9,0x80);
    sc910gs_write_register (ViPipe,0x36ea,0x05);
    sc910gs_write_register (ViPipe,0x36eb,0x0d);
    sc910gs_write_register (ViPipe,0x36ec,0x03);
    sc910gs_write_register (ViPipe,0x36ed,0x21);
    sc910gs_write_register (ViPipe,0x37fa,0x05);
    sc910gs_write_register (ViPipe,0x37fb,0x35);
    sc910gs_write_register (ViPipe,0x37fc,0x11);
    sc910gs_write_register (ViPipe,0x37fd,0x34);
    sc910gs_write_register (ViPipe,0x36e9,0x24);
    sc910gs_write_register (ViPipe,0x37f9,0x24);
    sc910gs_write_register (ViPipe,0x3018,0x72);
    sc910gs_write_register (ViPipe,0x3019,0xf0);
    sc910gs_write_register (ViPipe,0x301f,0x36);
    sc910gs_write_register (ViPipe,0x3033,0xa2);
    sc910gs_write_register (ViPipe,0x3202,0x00);
    sc910gs_write_register (ViPipe,0x3203,0x0c);
    sc910gs_write_register (ViPipe,0x3206,0x09);
    sc910gs_write_register (ViPipe,0x3207,0x33);
    sc910gs_write_register (ViPipe,0x320c,0x02);
    sc910gs_write_register (ViPipe,0x320d,0x58);
    sc910gs_write_register (ViPipe,0x3213,0x04);
    sc910gs_write_register (ViPipe,0x3271,0x1b);
    sc910gs_write_register (ViPipe,0x3273,0x1f);
    sc910gs_write_register (ViPipe,0x3275,0x1b);
    sc910gs_write_register (ViPipe,0x3277,0x1f);
    sc910gs_write_register (ViPipe,0x3306,0xa0);
    sc910gs_write_register (ViPipe,0x3308,0x20);
    sc910gs_write_register (ViPipe,0x330a,0x01);
    sc910gs_write_register (ViPipe,0x330b,0x10);
    sc910gs_write_register (ViPipe,0x3314,0xf0);
    sc910gs_write_register (ViPipe,0x3315,0x20);
    sc910gs_write_register (ViPipe,0x3317,0xb0);
    sc910gs_write_register (ViPipe,0x331f,0x02);
    sc910gs_write_register (ViPipe,0x3320,0xc1);
    sc910gs_write_register (ViPipe,0x3323,0x02);
    sc910gs_write_register (ViPipe,0x3328,0xfb);
    sc910gs_write_register (ViPipe,0x3364,0x0a);
    sc910gs_write_register (ViPipe,0x3366,0x04);
    sc910gs_write_register (ViPipe,0x3385,0x25);
    sc910gs_write_register (ViPipe,0x3387,0x6d);
    sc910gs_write_register (ViPipe,0x33ef,0x05);
    sc910gs_write_register (ViPipe,0x33f8,0x02);
    sc910gs_write_register (ViPipe,0x33fa,0x00);
    sc910gs_write_register (ViPipe,0x3410,0xb0);
    sc910gs_write_register (ViPipe,0x34f2,0x0f);
    sc910gs_write_register (ViPipe,0x3630,0xa4);
    sc910gs_write_register (ViPipe,0x3637,0x0f);
    sc910gs_write_register (ViPipe,0x363b,0x08);
    sc910gs_write_register (ViPipe,0x363c,0x07);
    sc910gs_write_register (ViPipe,0x363d,0x07);
    sc910gs_write_register (ViPipe,0x363e,0x67);
    sc910gs_write_register (ViPipe,0x363f,0x07);
    sc910gs_write_register (ViPipe,0x3648,0x99);
    sc910gs_write_register (ViPipe,0x364e,0x02);
    sc910gs_write_register (ViPipe,0x3654,0x00);
    sc910gs_write_register (ViPipe,0x365c,0x00);
    sc910gs_write_register (ViPipe,0x3727,0x07);
    sc910gs_write_register (ViPipe,0x372d,0x00);
    sc910gs_write_register (ViPipe,0x3731,0x00);
    sc910gs_write_register (ViPipe,0x3732,0x00);
    sc910gs_write_register (ViPipe,0x3733,0x08);
    sc910gs_write_register (ViPipe,0x3904,0x18);
    sc910gs_write_register (ViPipe,0x3905,0x2c);
    sc910gs_write_register (ViPipe,0x3907,0x02);
    sc910gs_write_register (ViPipe,0x391d,0x04);
    sc910gs_write_register (ViPipe,0x391f,0x19);
    sc910gs_write_register (ViPipe,0x3926,0x21);
    sc910gs_write_register (ViPipe,0x3927,0x01);
    sc910gs_write_register (ViPipe,0x3950,0x18);
    sc910gs_write_register (ViPipe,0x3e01,0x4d);
    sc910gs_write_register (ViPipe,0x3e02,0xe0);
    sc910gs_write_register (ViPipe,0x3e06,0x00);
    sc910gs_write_register (ViPipe,0x3e08,0x03);
    sc910gs_write_register (ViPipe,0x3e09,0x40);
    sc910gs_write_register (ViPipe,0x4350,0x00);
    sc910gs_write_register (ViPipe,0x4351,0x00);
    sc910gs_write_register (ViPipe,0x4353,0x37);
    sc910gs_write_register (ViPipe,0x4356,0x12);
    sc910gs_write_register (ViPipe,0x4361,0xb0);
    sc910gs_write_register (ViPipe,0x4366,0x1e);
    sc910gs_write_register (ViPipe,0x440e,0x02);
    sc910gs_write_register (ViPipe,0x4509,0x41);
    sc910gs_write_register (ViPipe,0x451c,0x01);
    sc910gs_write_register (ViPipe,0x451d,0x12);
    sc910gs_write_register (ViPipe,0x4800,0x24);
    sc910gs_write_register (ViPipe,0x4837,0x1b);
    sc910gs_write_register (ViPipe,0x4900,0x24);
    sc910gs_write_register (ViPipe,0x4937,0x14);
    sc910gs_write_register (ViPipe,0x5000,0x0e);
    sc910gs_write_register (ViPipe,0x5799,0x00);
    sc910gs_write_register (ViPipe,0x5928,0x03);
    sc910gs_write_register (ViPipe,0x59e0,0xc8);
    sc910gs_write_register (ViPipe,0x59e1,0x1c);
    sc910gs_write_register (ViPipe,0x59e2,0x10);
    sc910gs_write_register (ViPipe,0x59e3,0x08);
    sc910gs_write_register (ViPipe,0x59e4,0x00);
    sc910gs_write_register (ViPipe,0x59e5,0x10);
    sc910gs_write_register (ViPipe,0x59e6,0x08);
    sc910gs_write_register (ViPipe,0x59e7,0x00);
    sc910gs_write_register (ViPipe,0x59e8,0x18);
    sc910gs_write_register (ViPipe,0x59e9,0x0c);
    sc910gs_write_register (ViPipe,0x59ea,0x04);
    sc910gs_write_register (ViPipe,0x59eb,0x18);
    sc910gs_write_register (ViPipe,0x59ec,0x0c);
    sc910gs_write_register (ViPipe,0x59ed,0x04);
    sc910gs_write_register (ViPipe,0x59ee,0xc8);
    sc910gs_write_register (ViPipe,0x59ef,0x1c);
    sc910gs_write_register (ViPipe,0x59f4,0x10);
    sc910gs_write_register (ViPipe,0x59f5,0x08);
    sc910gs_write_register (ViPipe,0x59f6,0x00);
    sc910gs_write_register (ViPipe,0x59f7,0x10);
    sc910gs_write_register (ViPipe,0x59f8,0x08);
    sc910gs_write_register (ViPipe,0x59f9,0x00);
    sc910gs_write_register (ViPipe,0x59fa,0x18);
    sc910gs_write_register (ViPipe,0x59fb,0x0c);
    sc910gs_write_register (ViPipe,0x59fc,0x04);
    sc910gs_write_register (ViPipe,0x59fd,0x18);
    sc910gs_write_register (ViPipe,0x59fe,0x0c);
    sc910gs_write_register (ViPipe,0x59ff,0x04);
    sc910gs_default_reg_init(ViPipe);
    sc910gs_write_register (ViPipe,0x0100, 0x01); 
    //sc910gs_write_register (ViPipe,0x331f,0x12);
    //sc910gs_write_register (ViPipe,0x3385,0x1d);

    sc910gs_write_register (ViPipe,0x331f,0x02);
    sc910gs_write_register (ViPipe,0x3385,0x25);
    //delay_ms(20);
    //sc910gs_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //sc910gs_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    printf("===sc910gs 9M 20fps 10bit LINE Init OK!===\n");
    return;
}


