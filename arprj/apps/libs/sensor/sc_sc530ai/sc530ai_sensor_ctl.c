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

const unsigned char sc530ai_i2c_addr     =    0x60;  //0x30 for sid low  ; 0x32 for sid high   /* I2C Address of sc530ai */
const unsigned int  sc530ai_addr_byte    =    2;
const unsigned int  sc530ai_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastsc530ai[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunsc530aiBusInfo[];

int sc530ai_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunsc530aiBusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	printf("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (sc530ai_i2c_addr >> 1));
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

int sc530ai_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int sc530ai_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int sc530ai_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = sc530ai_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sc530ai_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sc530ai_data_byte;

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

    if (sc530ai_addr_byte == 2)
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

    if (sc530ai_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, sc530ai_addr_byte + sc530ai_data_byte);
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

void sc530ai_prog(VI_PIPE ViPipe, int *rom)
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
            sc530ai_write_register(ViPipe, addr, data);
        }
    }
}

void sc530ai_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void sc530ai_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define sc530ai_FLIP_MIRRO (0x3221)

int sc530ai_flip_off_mirro_off(VI_PIPE ViPipe)
{
    sc530ai_write_register(ViPipe, sc530ai_FLIP_MIRRO, 0x0);
    return 0;
}

int sc530ai_flip_on_mirro_off(VI_PIPE ViPipe)
{
    sc530ai_write_register(ViPipe, sc530ai_FLIP_MIRRO, 0x60);
    return 0;
}

int sc530ai_flip_off_mirro_on(VI_PIPE ViPipe)
{
    sc530ai_write_register(ViPipe, sc530ai_FLIP_MIRRO, 0x6);
    return 0;
}

int sc530ai_flip_on_mirro_on(VI_PIPE ViPipe)
{
    sc530ai_write_register(ViPipe, sc530ai_FLIP_MIRRO, 0x66);
    return 0;
}

#define sc530ai_SENSOR_2K_60FPS_LINEAR_MODE      (1)
#define sc530ai_SENSOR_2K_30FPS_LINEAR_MODE      (2)

void sc530ai_linear_5MP60_init(VI_PIPE ViPipe);
void sc530ai_linear_5MP30_init(VI_PIPE ViPipe);

void sc530ai_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastsc530ai[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //printf("set reg[0x%x]: 0x%x \n", g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //printf("set reg[0x%x]: 0x%x \n", g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            sc530ai_write_register(ViPipe, g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void sc530ai_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastsc530ai[ViPipe]->bInit;
    enWDRMode   = g_pastsc530ai[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastsc530ai[ViPipe]->u8ImgMode;

    sc530ai_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (sc530ai_SENSOR_2K_30FPS_LINEAR_MODE == u8ImgMode)
        {
            sc530ai_linear_5MP30_init(ViPipe);
        }
        else if (sc530ai_SENSOR_2K_60FPS_LINEAR_MODE == u8ImgMode)
        {
            sc530ai_linear_5MP60_init(ViPipe);
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {
        if (sc530ai_SENSOR_2K_30FPS_LINEAR_MODE == u8ImgMode)
        {
            sc530ai_linear_5MP30_init(ViPipe);
        }
        else if (sc530ai_SENSOR_2K_60FPS_LINEAR_MODE == u8ImgMode)
        {
            sc530ai_linear_5MP60_init(ViPipe);
        }
    }

    // for (i = 0; i < g_pastsc530ai[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     sc530ai_write_register(ViPipe, g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc530ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastsc530ai[ViPipe]->bInit = AR_TRUE;
    return ;
}

void sc530ai_exit(VI_PIPE ViPipe)
{
    sc530ai_i2c_exit(ViPipe);

    return;
}

/* 1C4D 792Mbps 10bit 2880*1620 60fps */
void sc530ai_linear_5MP60_init(VI_PIPE ViPipe)
{
	sc530ai_write_register(ViPipe, 0x0103, 0x01);
	sc530ai_write_register(ViPipe, 0x0100, 0x00);
	sc530ai_write_register(ViPipe, 0x36e9, 0x80);
	sc530ai_write_register(ViPipe, 0x37f9, 0x80);
	sc530ai_write_register(ViPipe, 0x301f, 0x02);

	sc530ai_write_register(ViPipe, 0x320a, 0x06);
	sc530ai_write_register(ViPipe, 0x320b, 0x50); // 1616
	sc530ai_write_register(ViPipe, 0x3210, 0x00);
	sc530ai_write_register(ViPipe, 0x3211, 0x06); // 4+2

	sc530ai_write_register(ViPipe, 0x3250, 0x40);
	sc530ai_write_register(ViPipe, 0x3251, 0x98);
	sc530ai_write_register(ViPipe, 0x3253, 0x0c);
	sc530ai_write_register(ViPipe, 0x325f, 0x20);
	sc530ai_write_register(ViPipe, 0x3301, 0x08);
	sc530ai_write_register(ViPipe, 0x3304, 0x58);
	sc530ai_write_register(ViPipe, 0x3306, 0xa0);
	sc530ai_write_register(ViPipe, 0x3308, 0x14);
	sc530ai_write_register(ViPipe, 0x3309, 0x50);
	sc530ai_write_register(ViPipe, 0x330a, 0x01);
	sc530ai_write_register(ViPipe, 0x330b, 0x10);
	sc530ai_write_register(ViPipe, 0x330d, 0x10);
	sc530ai_write_register(ViPipe, 0x331e, 0x49);
	sc530ai_write_register(ViPipe, 0x331f, 0x41);
	sc530ai_write_register(ViPipe, 0x3333, 0x10);
	sc530ai_write_register(ViPipe, 0x335d, 0x60);
	sc530ai_write_register(ViPipe, 0x335e, 0x06);
	sc530ai_write_register(ViPipe, 0x335f, 0x08);
	sc530ai_write_register(ViPipe, 0x3364, 0x56);
	sc530ai_write_register(ViPipe, 0x3366, 0x01);
	sc530ai_write_register(ViPipe, 0x337c, 0x02);
	sc530ai_write_register(ViPipe, 0x337d, 0x0a);
	sc530ai_write_register(ViPipe, 0x3390, 0x01);
	sc530ai_write_register(ViPipe, 0x3391, 0x03);
	sc530ai_write_register(ViPipe, 0x3392, 0x07);
	sc530ai_write_register(ViPipe, 0x3393, 0x08);
	sc530ai_write_register(ViPipe, 0x3394, 0x08);
	sc530ai_write_register(ViPipe, 0x3395, 0x08);
	sc530ai_write_register(ViPipe, 0x3396, 0x48);
	sc530ai_write_register(ViPipe, 0x3397, 0x4b);
	sc530ai_write_register(ViPipe, 0x3398, 0x4f);
	sc530ai_write_register(ViPipe, 0x3399, 0x0a);
	sc530ai_write_register(ViPipe, 0x339a, 0x0a);
	sc530ai_write_register(ViPipe, 0x339b, 0x10);
	sc530ai_write_register(ViPipe, 0x339c, 0x22);
	sc530ai_write_register(ViPipe, 0x33a2, 0x04);
	sc530ai_write_register(ViPipe, 0x33ad, 0x24);
	sc530ai_write_register(ViPipe, 0x33ae, 0x38);
	sc530ai_write_register(ViPipe, 0x33af, 0x38);
	sc530ai_write_register(ViPipe, 0x33b1, 0x80);
	sc530ai_write_register(ViPipe, 0x33b2, 0x48);
	sc530ai_write_register(ViPipe, 0x33b3, 0x20);
	sc530ai_write_register(ViPipe, 0x349f, 0x02);
	sc530ai_write_register(ViPipe, 0x34a6, 0x48);
	sc530ai_write_register(ViPipe, 0x34a7, 0x4b);
	sc530ai_write_register(ViPipe, 0x34a8, 0x20);
	sc530ai_write_register(ViPipe, 0x34a9, 0x18);
	sc530ai_write_register(ViPipe, 0x34f8, 0x5f);
	sc530ai_write_register(ViPipe, 0x34f9, 0x04);
	sc530ai_write_register(ViPipe, 0x3632, 0x48);
	sc530ai_write_register(ViPipe, 0x3633, 0x32);
	sc530ai_write_register(ViPipe, 0x3637, 0x29);
	sc530ai_write_register(ViPipe, 0x3638, 0xc1);
	sc530ai_write_register(ViPipe, 0x363b, 0x20);
	sc530ai_write_register(ViPipe, 0x363d, 0x02);
	sc530ai_write_register(ViPipe, 0x3670, 0x09);
	sc530ai_write_register(ViPipe, 0x3674, 0x88);
	sc530ai_write_register(ViPipe, 0x3675, 0x88);
	sc530ai_write_register(ViPipe, 0x3676, 0x88);
	sc530ai_write_register(ViPipe, 0x367c, 0x40);
	sc530ai_write_register(ViPipe, 0x367d, 0x48);
	sc530ai_write_register(ViPipe, 0x3690, 0x33);
	sc530ai_write_register(ViPipe, 0x3691, 0x34);
	sc530ai_write_register(ViPipe, 0x3692, 0x55);
	sc530ai_write_register(ViPipe, 0x3693, 0x4b);
	sc530ai_write_register(ViPipe, 0x3694, 0x4f);
	sc530ai_write_register(ViPipe, 0x3698, 0x85);
	sc530ai_write_register(ViPipe, 0x3699, 0x8f);
	sc530ai_write_register(ViPipe, 0x369a, 0xa0);
	sc530ai_write_register(ViPipe, 0x369b, 0xc3);
	sc530ai_write_register(ViPipe, 0x36a2, 0x49);
	sc530ai_write_register(ViPipe, 0x36a3, 0x4b);
	sc530ai_write_register(ViPipe, 0x36a4, 0x4f);
	sc530ai_write_register(ViPipe, 0x36d0, 0x01);
	sc530ai_write_register(ViPipe, 0x370f, 0x01);
	sc530ai_write_register(ViPipe, 0x3722, 0x00);
	sc530ai_write_register(ViPipe, 0x3728, 0x10);
	sc530ai_write_register(ViPipe, 0x37b0, 0x03);
	sc530ai_write_register(ViPipe, 0x37b1, 0x03);
	sc530ai_write_register(ViPipe, 0x37b2, 0x83);
	sc530ai_write_register(ViPipe, 0x37b3, 0x48);
	sc530ai_write_register(ViPipe, 0x37b4, 0x4f);
	sc530ai_write_register(ViPipe, 0x3901, 0x00);
	sc530ai_write_register(ViPipe, 0x3902, 0xc5);
	sc530ai_write_register(ViPipe, 0x3904, 0x08);
	sc530ai_write_register(ViPipe, 0x3905, 0x8d);
	sc530ai_write_register(ViPipe, 0x3909, 0x00);
	sc530ai_write_register(ViPipe, 0x391d, 0x04);
	sc530ai_write_register(ViPipe, 0x3926, 0x21);
	sc530ai_write_register(ViPipe, 0x3929, 0x18);
	sc530ai_write_register(ViPipe, 0x3933, 0x82);
	sc530ai_write_register(ViPipe, 0x3934, 0x08);
	sc530ai_write_register(ViPipe, 0x3937, 0x5b);
	sc530ai_write_register(ViPipe, 0x3939, 0x00);
	sc530ai_write_register(ViPipe, 0x393a, 0x01);
	sc530ai_write_register(ViPipe, 0x39dc, 0x02);
	sc530ai_write_register(ViPipe, 0x3e01, 0xcd);
	sc530ai_write_register(ViPipe, 0x3e02, 0xa0);
	sc530ai_write_register(ViPipe, 0x440e, 0x02);
	sc530ai_write_register(ViPipe, 0x4509, 0x20);
	sc530ai_write_register(ViPipe, 0x5010, 0x10);
	sc530ai_write_register(ViPipe, 0x5799, 0x06);
	sc530ai_write_register(ViPipe, 0x57ad, 0x00);
	sc530ai_write_register(ViPipe, 0x5ae0, 0xfe);
	sc530ai_write_register(ViPipe, 0x5ae1, 0x40);
	sc530ai_write_register(ViPipe, 0x5ae2, 0x30);
	sc530ai_write_register(ViPipe, 0x5ae3, 0x2a);
	sc530ai_write_register(ViPipe, 0x5ae4, 0x24);
	sc530ai_write_register(ViPipe, 0x5ae5, 0x30);
	sc530ai_write_register(ViPipe, 0x5ae6, 0x2a);
	sc530ai_write_register(ViPipe, 0x5ae7, 0x24);
	sc530ai_write_register(ViPipe, 0x5ae8, 0x3c);
	sc530ai_write_register(ViPipe, 0x5ae9, 0x30);
	sc530ai_write_register(ViPipe, 0x5aea, 0x28);
	sc530ai_write_register(ViPipe, 0x5aeb, 0x3c);
	sc530ai_write_register(ViPipe, 0x5aec, 0x30);
	sc530ai_write_register(ViPipe, 0x5aed, 0x28);
	sc530ai_write_register(ViPipe, 0x5aee, 0xfe);
	sc530ai_write_register(ViPipe, 0x5aef, 0x40);
	sc530ai_write_register(ViPipe, 0x5af4, 0x30);
	sc530ai_write_register(ViPipe, 0x5af5, 0x2a);
	sc530ai_write_register(ViPipe, 0x5af6, 0x24);
	sc530ai_write_register(ViPipe, 0x5af7, 0x30);
	sc530ai_write_register(ViPipe, 0x5af8, 0x2a);
	sc530ai_write_register(ViPipe, 0x5af9, 0x24);
	sc530ai_write_register(ViPipe, 0x5afa, 0x3c);
	sc530ai_write_register(ViPipe, 0x5afb, 0x30);
	sc530ai_write_register(ViPipe, 0x5afc, 0x28);
	sc530ai_write_register(ViPipe, 0x5afd, 0x3c);
	sc530ai_write_register(ViPipe, 0x5afe, 0x30);
	sc530ai_write_register(ViPipe, 0x5aff, 0x28);
	sc530ai_write_register(ViPipe, 0x36e9, 0x44);
	sc530ai_write_register(ViPipe, 0x37f9, 0x44);
    sc530ai_default_reg_init(ViPipe);
    sc530ai_write_register (ViPipe,0x0100, 0x01); 

    //sc530ai_write_register (ViPipe,0x331f,0x02);
    //sc530ai_write_register (ViPipe,0x3385,0x25);

    printf("===sc530ai 5M 60fps 10bit LINE Init OK!===\n");
    return;
}

void sc530ai_linear_5MP30_init(VI_PIPE ViPipe)
{
	sc530ai_write_register(ViPipe, 0x0103, 0x01);
	sc530ai_write_register(ViPipe, 0x0100, 0x00);
	sc530ai_write_register(ViPipe, 0x36e9, 0x80);
	sc530ai_write_register(ViPipe, 0x37f9, 0x80);
	sc530ai_write_register(ViPipe, 0x301f, 0x02);

	sc530ai_write_register(ViPipe, 0x320a, 0x06);
	sc530ai_write_register(ViPipe, 0x320b, 0x50); // 1616
	sc530ai_write_register(ViPipe, 0x3210, 0x00);
	sc530ai_write_register(ViPipe, 0x3211, 0x06); // 4+2

	sc530ai_write_register(ViPipe, 0x320e, 0x0c); // 0x0672 * 2
	sc530ai_write_register(ViPipe, 0x320f, 0xe4);

	sc530ai_write_register(ViPipe, 0x3250, 0x40);
	sc530ai_write_register(ViPipe, 0x3251, 0x98);
	sc530ai_write_register(ViPipe, 0x3253, 0x0c);
	sc530ai_write_register(ViPipe, 0x325f, 0x20);
	sc530ai_write_register(ViPipe, 0x3301, 0x08);
	sc530ai_write_register(ViPipe, 0x3304, 0x58);
	sc530ai_write_register(ViPipe, 0x3306, 0xa0);
	sc530ai_write_register(ViPipe, 0x3308, 0x14);
	sc530ai_write_register(ViPipe, 0x3309, 0x50);
	sc530ai_write_register(ViPipe, 0x330a, 0x01);
	sc530ai_write_register(ViPipe, 0x330b, 0x10);
	sc530ai_write_register(ViPipe, 0x330d, 0x10);
	sc530ai_write_register(ViPipe, 0x331e, 0x49);
	sc530ai_write_register(ViPipe, 0x331f, 0x41);
	sc530ai_write_register(ViPipe, 0x3333, 0x10);
	sc530ai_write_register(ViPipe, 0x335d, 0x60);
	sc530ai_write_register(ViPipe, 0x335e, 0x06);
	sc530ai_write_register(ViPipe, 0x335f, 0x08);
	sc530ai_write_register(ViPipe, 0x3364, 0x56);
	sc530ai_write_register(ViPipe, 0x3366, 0x01);
	sc530ai_write_register(ViPipe, 0x337c, 0x02);
	sc530ai_write_register(ViPipe, 0x337d, 0x0a);
	sc530ai_write_register(ViPipe, 0x3390, 0x01);
	sc530ai_write_register(ViPipe, 0x3391, 0x03);
	sc530ai_write_register(ViPipe, 0x3392, 0x07);
	sc530ai_write_register(ViPipe, 0x3393, 0x08);
	sc530ai_write_register(ViPipe, 0x3394, 0x08);
	sc530ai_write_register(ViPipe, 0x3395, 0x08);
	sc530ai_write_register(ViPipe, 0x3396, 0x48);
	sc530ai_write_register(ViPipe, 0x3397, 0x4b);
	sc530ai_write_register(ViPipe, 0x3398, 0x4f);
	sc530ai_write_register(ViPipe, 0x3399, 0x0a);
	sc530ai_write_register(ViPipe, 0x339a, 0x0a);
	sc530ai_write_register(ViPipe, 0x339b, 0x10);
	sc530ai_write_register(ViPipe, 0x339c, 0x22);
	sc530ai_write_register(ViPipe, 0x33a2, 0x04);
	sc530ai_write_register(ViPipe, 0x33ad, 0x24);
	sc530ai_write_register(ViPipe, 0x33ae, 0x38);
	sc530ai_write_register(ViPipe, 0x33af, 0x38);
	sc530ai_write_register(ViPipe, 0x33b1, 0x80);
	sc530ai_write_register(ViPipe, 0x33b2, 0x48);
	sc530ai_write_register(ViPipe, 0x33b3, 0x20);
	sc530ai_write_register(ViPipe, 0x349f, 0x02);
	sc530ai_write_register(ViPipe, 0x34a6, 0x48);
	sc530ai_write_register(ViPipe, 0x34a7, 0x4b);
	sc530ai_write_register(ViPipe, 0x34a8, 0x20);
	sc530ai_write_register(ViPipe, 0x34a9, 0x18);
	sc530ai_write_register(ViPipe, 0x34f8, 0x5f);
	sc530ai_write_register(ViPipe, 0x34f9, 0x04);
	sc530ai_write_register(ViPipe, 0x3632, 0x48);
	sc530ai_write_register(ViPipe, 0x3633, 0x32);
	sc530ai_write_register(ViPipe, 0x3637, 0x29);
	sc530ai_write_register(ViPipe, 0x3638, 0xc1);
	sc530ai_write_register(ViPipe, 0x363b, 0x20);
	sc530ai_write_register(ViPipe, 0x363d, 0x02);
	sc530ai_write_register(ViPipe, 0x3670, 0x09);
	sc530ai_write_register(ViPipe, 0x3674, 0x88);
	sc530ai_write_register(ViPipe, 0x3675, 0x88);
	sc530ai_write_register(ViPipe, 0x3676, 0x88);
	sc530ai_write_register(ViPipe, 0x367c, 0x40);
	sc530ai_write_register(ViPipe, 0x367d, 0x48);
	sc530ai_write_register(ViPipe, 0x3690, 0x33);
	sc530ai_write_register(ViPipe, 0x3691, 0x34);
	sc530ai_write_register(ViPipe, 0x3692, 0x55);
	sc530ai_write_register(ViPipe, 0x3693, 0x4b);
	sc530ai_write_register(ViPipe, 0x3694, 0x4f);
	sc530ai_write_register(ViPipe, 0x3698, 0x85);
	sc530ai_write_register(ViPipe, 0x3699, 0x8f);
	sc530ai_write_register(ViPipe, 0x369a, 0xa0);
	sc530ai_write_register(ViPipe, 0x369b, 0xc3);
	sc530ai_write_register(ViPipe, 0x36a2, 0x49);
	sc530ai_write_register(ViPipe, 0x36a3, 0x4b);
	sc530ai_write_register(ViPipe, 0x36a4, 0x4f);
	sc530ai_write_register(ViPipe, 0x36d0, 0x01);
	sc530ai_write_register(ViPipe, 0x370f, 0x01);
	sc530ai_write_register(ViPipe, 0x3722, 0x00);
	sc530ai_write_register(ViPipe, 0x3728, 0x10);
	sc530ai_write_register(ViPipe, 0x37b0, 0x03);
	sc530ai_write_register(ViPipe, 0x37b1, 0x03);
	sc530ai_write_register(ViPipe, 0x37b2, 0x83);
	sc530ai_write_register(ViPipe, 0x37b3, 0x48);
	sc530ai_write_register(ViPipe, 0x37b4, 0x4f);
	sc530ai_write_register(ViPipe, 0x3901, 0x00);
	sc530ai_write_register(ViPipe, 0x3902, 0xc5);
	sc530ai_write_register(ViPipe, 0x3904, 0x08);
	sc530ai_write_register(ViPipe, 0x3905, 0x8d);
	sc530ai_write_register(ViPipe, 0x3909, 0x00);
	sc530ai_write_register(ViPipe, 0x391d, 0x04);
	sc530ai_write_register(ViPipe, 0x3926, 0x21);
	sc530ai_write_register(ViPipe, 0x3929, 0x18);
	sc530ai_write_register(ViPipe, 0x3933, 0x82);
	sc530ai_write_register(ViPipe, 0x3934, 0x08);
	sc530ai_write_register(ViPipe, 0x3937, 0x5b);
	sc530ai_write_register(ViPipe, 0x3939, 0x00);
	sc530ai_write_register(ViPipe, 0x393a, 0x01);
	sc530ai_write_register(ViPipe, 0x39dc, 0x02);
	sc530ai_write_register(ViPipe, 0x3e01, 0xcd);
	sc530ai_write_register(ViPipe, 0x3e02, 0xa0);
	sc530ai_write_register(ViPipe, 0x440e, 0x02);
	sc530ai_write_register(ViPipe, 0x4509, 0x20);
	sc530ai_write_register(ViPipe, 0x5010, 0x10);
	sc530ai_write_register(ViPipe, 0x5799, 0x06);
	sc530ai_write_register(ViPipe, 0x57ad, 0x00);
	sc530ai_write_register(ViPipe, 0x5ae0, 0xfe);
	sc530ai_write_register(ViPipe, 0x5ae1, 0x40);
	sc530ai_write_register(ViPipe, 0x5ae2, 0x30);
	sc530ai_write_register(ViPipe, 0x5ae3, 0x2a);
	sc530ai_write_register(ViPipe, 0x5ae4, 0x24);
	sc530ai_write_register(ViPipe, 0x5ae5, 0x30);
	sc530ai_write_register(ViPipe, 0x5ae6, 0x2a);
	sc530ai_write_register(ViPipe, 0x5ae7, 0x24);
	sc530ai_write_register(ViPipe, 0x5ae8, 0x3c);
	sc530ai_write_register(ViPipe, 0x5ae9, 0x30);
	sc530ai_write_register(ViPipe, 0x5aea, 0x28);
	sc530ai_write_register(ViPipe, 0x5aeb, 0x3c);
	sc530ai_write_register(ViPipe, 0x5aec, 0x30);
	sc530ai_write_register(ViPipe, 0x5aed, 0x28);
	sc530ai_write_register(ViPipe, 0x5aee, 0xfe);
	sc530ai_write_register(ViPipe, 0x5aef, 0x40);
	sc530ai_write_register(ViPipe, 0x5af4, 0x30);
	sc530ai_write_register(ViPipe, 0x5af5, 0x2a);
	sc530ai_write_register(ViPipe, 0x5af6, 0x24);
	sc530ai_write_register(ViPipe, 0x5af7, 0x30);
	sc530ai_write_register(ViPipe, 0x5af8, 0x2a);
	sc530ai_write_register(ViPipe, 0x5af9, 0x24);
	sc530ai_write_register(ViPipe, 0x5afa, 0x3c);
	sc530ai_write_register(ViPipe, 0x5afb, 0x30);
	sc530ai_write_register(ViPipe, 0x5afc, 0x28);
	sc530ai_write_register(ViPipe, 0x5afd, 0x3c);
	sc530ai_write_register(ViPipe, 0x5afe, 0x30);
	sc530ai_write_register(ViPipe, 0x5aff, 0x28);
	sc530ai_write_register(ViPipe, 0x36e9, 0x44);
	sc530ai_write_register(ViPipe, 0x37f9, 0x44);
    sc530ai_default_reg_init(ViPipe);
    sc530ai_write_register (ViPipe,0x0100, 0x01); 

    //sc530ai_write_register (ViPipe,0x331f,0x02);
    //sc530ai_write_register (ViPipe,0x3385,0x25);

    printf("===sc530ai 5M 30fps 10bit LINE Init OK!===\n");
    return;
}
