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

const unsigned char sc230ai_i2c_addr     =    0x60;        /* I2C Address of IMX290 */
const unsigned int  sc230ai_addr_byte    =    2;
const unsigned int  sc230ai_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastSc230ai[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunSc230aiBusInfo[];
int sc230ai_i2c_init(VI_PIPE ViPipe)
{
    printf("sc230ai_i2c_init ....\n");
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
        printf(AR_DBG_ERR, "Open gpioi2c_ex error!\n");
        return AR_FAILURE;
    }
#else
    int ret;

    u8DevNum =g_aunSc230aiBusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	ar_always("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        printf(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (sc230ai_i2c_addr >> 1));
    if (ret < 0)
    {
        printf(AR_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }
#endif

    return AR_SUCCESS;
}

int sc230ai_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int sc230ai_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int sc230ai_write_register(VI_PIPE ViPipe, int addr, int data)
{
    //printf("sc230ai_write_register \n");
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }
#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = sc230ai_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sc230ai_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sc230ai_data_byte;

    ret = ioctl(g_fd[ViPipe], GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf(AR_DBG_ERR, "GPIO-I2C write faild!\n");
        return ret;
    }
#else
    int idx = 0;
    int ret;
    char buf[8];
    if (sc230ai_addr_byte == 2)
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
    if (sc230ai_data_byte == 2)
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
    ret = write(g_fd[ViPipe], buf, sc230ai_addr_byte + sc230ai_data_byte);
    if (ret < 0)
    {
        printf(AR_DBG_ERR, "I2C_WRITE error!\n");
        return AR_FAILURE;
    }
#endif
    return AR_SUCCESS;
}


static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void sc230ai_prog(VI_PIPE ViPipe, int *rom)
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
            sc230ai_write_register(ViPipe, addr, data);
        }
    }
}

void sc230ai_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void sc230ai_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define IMX290_FLIP_MIRRO (0x3030)

int sc230ai_flip_off_mirro_off(VI_PIPE ViPipe)
{
    sc230ai_write_register(ViPipe, IMX290_FLIP_MIRRO, 0x00);
    return 0;
}

int sc230ai_flip_on_mirro_off(VI_PIPE ViPipe)
{
    sc230ai_write_register(ViPipe, IMX290_FLIP_MIRRO, 0x01);
    return 0;
}

int sc230ai_flip_off_mirro_on(VI_PIPE ViPipe)
{
    sc230ai_write_register(ViPipe, IMX290_FLIP_MIRRO, 0x02);
    return 0;
}

int sc230ai_flip_on_mirro_on(VI_PIPE ViPipe)
{
    sc230ai_write_register(ViPipe, IMX290_FLIP_MIRRO, 0x03);
    return 0;
}

#define IMX290_SENSOR_1080P_30FPS_LINEAR_MODE      (1)
#define IMX290_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
#define IMX290_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
#define IMX290_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)
#define IMX290_SENSOR_1080P_60FPS_LINEAR_MODE      (5)

void sc230ai_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
void sc230ai_wdr_1080p60_2to1_init(VI_PIPE ViPipe);
void sc230ai_wdr_1080p120_2to1_init(VI_PIPE ViPipe);
void sc230ai_wdr_720p60_2to1_init(VI_PIPE ViPipe);
void sc230ai_wdr_1080p30_3to1_init(VI_PIPE ViPipe);
void sc230ai_wdr_1080p120_3to1_init(VI_PIPE ViPipe);
void sc230ai_wdr_720p60_3to1_init(VI_PIPE ViPipe);
void sc230ai_linear_1080p30_init(VI_PIPE ViPipe);
void sc230ai_linear_1080p60_init(VI_PIPE ViPipe);


void sc230ai_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastSc230ai[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //printf("set reg[0x%x]: 0x%x \n", g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //printf("set111111111111 reg[0x%x]: 0x%x \n", g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            sc230ai_write_register(ViPipe, g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void sc230ai_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastSc230ai[ViPipe]->bInit;
    enWDRMode   = g_pastSc230ai[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastSc230ai[ViPipe]->u8ImgMode;
    //sc230ai_i2c_init(ViPipe);
    if(sc230ai_i2c_init(ViPipe) != AR_SUCCESS)
    {
        printf("sc230ai_init failed \n");
    }

    /* When sensor first init, config all registers */
    if(ViPipe == 0)
    {
        sc230ai_linear_1080p30_init(ViPipe);
    }
    else
    {
        sc230ai_linear_1080p30_init(ViPipe);
    }

    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */


    // for (i = 0; i < g_pastSc230ai[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     sc230ai_write_register(ViPipe, g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastSc230ai[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastSc230ai[ViPipe]->bInit = AR_TRUE;
    return ;
}

void sc230ai_exit(VI_PIPE ViPipe)
{
    sc230ai_i2c_exit(ViPipe);

    return;
}

/* 1080P30 */
void sc230ai_linear_1080p30_init(VI_PIPE ViPipe)
{
    sc230ai_write_register (ViPipe,0x0103, 0x01);
    sc230ai_write_register (ViPipe,0x0100, 0x00);
    sc230ai_write_register (ViPipe,0x36e9, 0x80);
    sc230ai_write_register (ViPipe,0x37f9, 0x80);
    sc230ai_write_register (ViPipe,0x301f, 0x23);
    sc230ai_write_register (ViPipe,0x3208, 0x07);
    sc230ai_write_register (ViPipe,0x3209, 0x80);
    sc230ai_write_register (ViPipe,0x320a, 0x04);
    sc230ai_write_register (ViPipe,0x320b, 0x38);
    sc230ai_write_register (ViPipe,0x320c, 0x09);
    sc230ai_write_register (ViPipe,0x320d, 0x60);
    /*VTSframerate*/
    sc230ai_write_register (ViPipe,0x320e, 0x08);
    sc230ai_write_register (ViPipe,0x320f, 0xca);

    sc230ai_write_register (ViPipe,0x3211, 0x04);
    sc230ai_write_register (ViPipe,0x3213, 0x04);

    sc230ai_write_register (ViPipe,0x3217, 0x04);
    sc230ai_write_register (ViPipe,0x322e, 0x01);
    sc230ai_write_register (ViPipe,0x3225, 0x30);
    sc230ai_write_register (ViPipe,0x3227, 0x03);
    sc230ai_write_register (ViPipe,0x3250, 0x00);
    sc230ai_write_register (ViPipe,0x3301, 0x09);
    sc230ai_write_register (ViPipe,0x3304, 0x50);
    sc230ai_write_register (ViPipe,0x3306, 0x48);
    sc230ai_write_register (ViPipe,0x3308, 0x18);
    sc230ai_write_register (ViPipe,0x3309, 0x68);
    sc230ai_write_register (ViPipe,0x330a, 0x00);
    sc230ai_write_register (ViPipe,0x330b, 0xc0);
    sc230ai_write_register (ViPipe,0x331e, 0x41);
    sc230ai_write_register (ViPipe,0x331f, 0x59);
    sc230ai_write_register (ViPipe,0x3333, 0x10);
    sc230ai_write_register (ViPipe,0x3334, 0x40);
    sc230ai_write_register (ViPipe,0x335d, 0x60);
    sc230ai_write_register (ViPipe,0x335e, 0x06);
    sc230ai_write_register (ViPipe,0x335f, 0x08);
    sc230ai_write_register (ViPipe,0x3364, 0x5e);
    sc230ai_write_register (ViPipe,0x337c, 0x02);
    sc230ai_write_register (ViPipe,0x337d, 0x0a);
    sc230ai_write_register (ViPipe,0x3390, 0x01);
    sc230ai_write_register (ViPipe,0x3391, 0x0b);
    sc230ai_write_register (ViPipe,0x3392, 0x0f);
    sc230ai_write_register (ViPipe,0x3393, 0x0c);
    sc230ai_write_register (ViPipe,0x3394, 0x0d);
    sc230ai_write_register (ViPipe,0x3395, 0x60);
    sc230ai_write_register (ViPipe,0x3396, 0x48);
    sc230ai_write_register (ViPipe,0x3397, 0x49);
    sc230ai_write_register (ViPipe,0x3398, 0x4f);
    sc230ai_write_register (ViPipe,0x3399, 0x0a);
    sc230ai_write_register (ViPipe,0x339a, 0x0f);
    sc230ai_write_register (ViPipe,0x339b, 0x14);
    sc230ai_write_register (ViPipe,0x339c, 0x60);
    sc230ai_write_register (ViPipe,0x33a2, 0x04);
    sc230ai_write_register (ViPipe,0x33af, 0x40);
    sc230ai_write_register (ViPipe,0x33b1, 0x80);
    sc230ai_write_register (ViPipe,0x33b3, 0x40);
    sc230ai_write_register (ViPipe,0x33b9, 0x0a);
    sc230ai_write_register (ViPipe,0x33f9, 0x70);
    sc230ai_write_register (ViPipe,0x33fb, 0x90);
    sc230ai_write_register (ViPipe,0x33fc, 0x4b);
    sc230ai_write_register (ViPipe,0x33fd, 0x5f);
    sc230ai_write_register (ViPipe,0x349f, 0x03);
    sc230ai_write_register (ViPipe,0x34a6, 0x4b);
    sc230ai_write_register (ViPipe,0x34a7, 0x4f);
    sc230ai_write_register (ViPipe,0x34a8, 0x30);
    sc230ai_write_register (ViPipe,0x34a9, 0x20);
    sc230ai_write_register (ViPipe,0x34aa, 0x00);
    sc230ai_write_register (ViPipe,0x34ab, 0xe0);
    sc230ai_write_register (ViPipe,0x34ac, 0x01);
    sc230ai_write_register (ViPipe,0x34ad, 0x00);
    sc230ai_write_register (ViPipe,0x34f8, 0x5f);
    sc230ai_write_register (ViPipe,0x34f9, 0x10);
    sc230ai_write_register (ViPipe,0x3630, 0xc0);
    sc230ai_write_register (ViPipe,0x3633, 0x44);
    sc230ai_write_register (ViPipe,0x3637, 0x29);
    sc230ai_write_register (ViPipe,0x363b, 0x20);
    sc230ai_write_register (ViPipe,0x3670, 0x09);
    sc230ai_write_register (ViPipe,0x3674, 0xb0);
    sc230ai_write_register (ViPipe,0x3675, 0x80);
    sc230ai_write_register (ViPipe,0x3676, 0x88);
    sc230ai_write_register (ViPipe,0x367c, 0x40);
    sc230ai_write_register (ViPipe,0x367d, 0x49);
    sc230ai_write_register (ViPipe,0x3690, 0x54);
    sc230ai_write_register (ViPipe,0x3691, 0x44);
    sc230ai_write_register (ViPipe,0x3692, 0x55);
    sc230ai_write_register (ViPipe,0x369c, 0x49);
    sc230ai_write_register (ViPipe,0x369d, 0x4f);
    sc230ai_write_register (ViPipe,0x36ae, 0x4b);
    sc230ai_write_register (ViPipe,0x36af, 0x4f);
    sc230ai_write_register (ViPipe,0x36b0, 0x87);
    sc230ai_write_register (ViPipe,0x36b1, 0x9b);
    sc230ai_write_register (ViPipe,0x36b2, 0xb7);
    sc230ai_write_register (ViPipe,0x36d0, 0x01);
    sc230ai_write_register (ViPipe,0x36ea, 0x09);
    sc230ai_write_register (ViPipe,0x36eb, 0x04);
    sc230ai_write_register (ViPipe,0x36ec, 0x0c);
    sc230ai_write_register (ViPipe,0x36ed, 0x24);
    sc230ai_write_register (ViPipe,0x370f, 0x01);
    sc230ai_write_register (ViPipe,0x3722, 0x17);
    sc230ai_write_register (ViPipe,0x3728, 0x90);
    sc230ai_write_register (ViPipe,0x37b0, 0x17);
    sc230ai_write_register (ViPipe,0x37b1, 0x17);
    sc230ai_write_register (ViPipe,0x37b2, 0x97);
    sc230ai_write_register (ViPipe,0x37b3, 0x4b);
    sc230ai_write_register (ViPipe,0x37b4, 0x4f);
    sc230ai_write_register (ViPipe,0x37fa, 0x09);
    sc230ai_write_register (ViPipe,0x37fb, 0x04);
    sc230ai_write_register (ViPipe,0x37fc, 0x00);
    sc230ai_write_register (ViPipe,0x37fd, 0x22);
    sc230ai_write_register (ViPipe,0x3901, 0x02);
    sc230ai_write_register (ViPipe,0x3902, 0xc5);
    sc230ai_write_register (ViPipe,0x3904, 0x04);
    sc230ai_write_register (ViPipe,0x3907, 0x00);
    sc230ai_write_register (ViPipe,0x3908, 0x41);
    sc230ai_write_register (ViPipe,0x3909, 0x00);
    sc230ai_write_register (ViPipe,0x390a, 0x00);
    sc230ai_write_register (ViPipe,0x391f, 0x04);
    sc230ai_write_register (ViPipe,0x3928, 0xc1);
    sc230ai_write_register (ViPipe,0x3933, 0x84);
    sc230ai_write_register (ViPipe,0x3934, 0x02);
    sc230ai_write_register (ViPipe,0x3940, 0x62);
    sc230ai_write_register (ViPipe,0x3941, 0x00);
    sc230ai_write_register (ViPipe,0x3942, 0x04);
    sc230ai_write_register (ViPipe,0x3943, 0x03);
    sc230ai_write_register (ViPipe,0x3e00, 0x00);
    sc230ai_write_register (ViPipe,0x3e01, 0x8c);
    sc230ai_write_register (ViPipe,0x3e02, 0x20);
    sc230ai_write_register (ViPipe,0x440e, 0x02);
    sc230ai_write_register (ViPipe,0x450d, 0x11);
    sc230ai_write_register (ViPipe,0x4819, 0x0a);
    sc230ai_write_register (ViPipe,0x481b, 0x06);
    sc230ai_write_register (ViPipe,0x481d, 0x16);
    sc230ai_write_register (ViPipe,0x481f, 0x05);
    sc230ai_write_register (ViPipe,0x4821, 0x0b);
    sc230ai_write_register (ViPipe,0x4823, 0x05);
    sc230ai_write_register (ViPipe,0x4825, 0x05);
    sc230ai_write_register (ViPipe,0x4827, 0x05);
    sc230ai_write_register (ViPipe,0x4829, 0x09);
    sc230ai_write_register (ViPipe,0x5010, 0x01);
    sc230ai_write_register (ViPipe,0x5787, 0x08);
    sc230ai_write_register (ViPipe,0x5788, 0x03);
    sc230ai_write_register (ViPipe,0x5789, 0x00);
    sc230ai_write_register (ViPipe,0x578a, 0x10);
    sc230ai_write_register (ViPipe,0x578b, 0x08);
    sc230ai_write_register (ViPipe,0x578c, 0x00);
    sc230ai_write_register (ViPipe,0x5790, 0x08);
    sc230ai_write_register (ViPipe,0x5791, 0x04);
    sc230ai_write_register (ViPipe,0x5792, 0x00);
    sc230ai_write_register (ViPipe,0x5793, 0x10);
    sc230ai_write_register (ViPipe,0x5794, 0x08);
    sc230ai_write_register (ViPipe,0x5795, 0x00);
    sc230ai_write_register (ViPipe,0x5799, 0x06);
    sc230ai_write_register (ViPipe,0x57ad, 0x00);
    sc230ai_write_register (ViPipe,0x5ae0, 0xfe);
    sc230ai_write_register (ViPipe,0x5ae1, 0x40);
    sc230ai_write_register (ViPipe,0x5ae2, 0x3f);
    sc230ai_write_register (ViPipe,0x5ae3, 0x38);
    sc230ai_write_register (ViPipe,0x5ae4, 0x28);
    sc230ai_write_register (ViPipe,0x5ae5, 0x3f);
    sc230ai_write_register (ViPipe,0x5ae6, 0x38);
    sc230ai_write_register (ViPipe,0x5ae7, 0x28);
    sc230ai_write_register (ViPipe,0x5ae8, 0x3f);
    sc230ai_write_register (ViPipe,0x5ae9, 0x3c);
    sc230ai_write_register (ViPipe,0x5aea, 0x2c);
    sc230ai_write_register (ViPipe,0x5aeb, 0x3f);
    sc230ai_write_register (ViPipe,0x5aec, 0x3c);
    sc230ai_write_register (ViPipe,0x5aed, 0x2c);
    sc230ai_write_register (ViPipe,0x5af4, 0x3f);
    sc230ai_write_register (ViPipe,0x5af5, 0x38);
    sc230ai_write_register (ViPipe,0x5af6, 0x28);
    sc230ai_write_register (ViPipe,0x5af7, 0x3f);
    sc230ai_write_register (ViPipe,0x5af8, 0x38);
    sc230ai_write_register (ViPipe,0x5af9, 0x28);
    sc230ai_write_register (ViPipe,0x5afa, 0x3f);
    sc230ai_write_register (ViPipe,0x5afb, 0x3c);
    sc230ai_write_register (ViPipe,0x5afc, 0x2c);
    sc230ai_write_register (ViPipe,0x5afd, 0x3f);
    sc230ai_write_register (ViPipe,0x5afe, 0x3c);
    sc230ai_write_register (ViPipe,0x5aff, 0x2c);

    ////output fsync
    //sc230ai_write_register (ViPipe,0x300a, 0x24);
    //sc230ai_write_register (ViPipe,0x3032, 0xa0);
    /*sync*/

    sc230ai_write_register (ViPipe,0x3222, 0x00);
    sc230ai_write_register (ViPipe,0x3224, 0x92);
    sc230ai_write_register (ViPipe,0x300a, 0x24);
    sc230ai_write_register (ViPipe,0x3032, 0xa0);//开输出

    sc230ai_write_register (ViPipe,0x36e9, 0x53);
    sc230ai_write_register (ViPipe,0x37f9, 0x53);

    sc230ai_write_register (ViPipe,0x0100, 0x01);
    printf("11111111===sc230ai 1080P 25fps 10bit LINE Init OK!===\n");
    return;
}





void sc230ai_linear_1080p30_init_slave(VI_PIPE ViPipe)
{

//slave
    sc230ai_write_register (ViPipe,0x0103, 0x01);
    sc230ai_write_register (ViPipe,0x0100, 0x00);
    sc230ai_write_register (ViPipe,0x36e9, 0x80);
    sc230ai_write_register (ViPipe,0x37f9, 0x80);
    sc230ai_write_register (ViPipe,0x301f, 0x24);
    sc230ai_write_register (ViPipe,0x3208, 0x07);
    sc230ai_write_register (ViPipe,0x3209, 0x80);
    sc230ai_write_register (ViPipe,0x320a, 0x04);
    sc230ai_write_register (ViPipe,0x320b, 0x38);
    sc230ai_write_register (ViPipe,0x320c, 0x09);
    sc230ai_write_register (ViPipe,0x320d, 0x60);
        /*VTSframerate*/
    sc230ai_write_register (ViPipe,0x320e, 0x0a);
    sc230ai_write_register (ViPipe,0x320f, 0x8c);

    sc230ai_write_register (ViPipe,0x3211, 0x04);
    sc230ai_write_register (ViPipe,0x3213, 0x04);
    sc230ai_write_register (ViPipe,0x3222, 0x01);
    sc230ai_write_register (ViPipe,0x3224, 0x82);
    sc230ai_write_register (ViPipe,0x3227, 0x03);
    sc230ai_write_register (ViPipe,0x3230, 0x03);
    sc230ai_write_register (ViPipe,0x3231, 0x84);
    sc230ai_write_register (ViPipe,0x3250, 0x00);
    sc230ai_write_register (ViPipe,0x3301, 0x09);
    sc230ai_write_register (ViPipe,0x3304, 0x50);
    sc230ai_write_register (ViPipe,0x3306, 0x48);
    sc230ai_write_register (ViPipe,0x3308, 0x18);
    sc230ai_write_register (ViPipe,0x3309, 0x68);
    sc230ai_write_register (ViPipe,0x330a, 0x00);
    sc230ai_write_register (ViPipe,0x330b, 0xc0);
    sc230ai_write_register (ViPipe,0x331e, 0x41);
    sc230ai_write_register (ViPipe,0x331f, 0x59);
    sc230ai_write_register (ViPipe,0x3333, 0x10);
    sc230ai_write_register (ViPipe,0x3334, 0x40);
    sc230ai_write_register (ViPipe,0x335d, 0x60);
    sc230ai_write_register (ViPipe,0x335e, 0x06);
    sc230ai_write_register (ViPipe,0x335f, 0x08);
    sc230ai_write_register (ViPipe,0x3364, 0x5e);
    sc230ai_write_register (ViPipe,0x337c, 0x02);
    sc230ai_write_register (ViPipe,0x337d, 0x0a);
    sc230ai_write_register (ViPipe,0x3390, 0x01);
    sc230ai_write_register (ViPipe,0x3391, 0x0b);
    sc230ai_write_register (ViPipe,0x3392, 0x0f);
    sc230ai_write_register (ViPipe,0x3393, 0x0c);
    sc230ai_write_register (ViPipe,0x3394, 0x0d);
    sc230ai_write_register (ViPipe,0x3395, 0x60);
    sc230ai_write_register (ViPipe,0x3396, 0x48);
    sc230ai_write_register (ViPipe,0x3397, 0x49);
    sc230ai_write_register (ViPipe,0x3398, 0x4f);
    sc230ai_write_register (ViPipe,0x3399, 0x0a);
    sc230ai_write_register (ViPipe,0x339a, 0x0f);
    sc230ai_write_register (ViPipe,0x339b, 0x14);
    sc230ai_write_register (ViPipe,0x339c, 0x60);
    sc230ai_write_register (ViPipe,0x33a2, 0x04);
    sc230ai_write_register (ViPipe,0x33af, 0x40);
    sc230ai_write_register (ViPipe,0x33b1, 0x80);
    sc230ai_write_register (ViPipe,0x33b3, 0x40);
    sc230ai_write_register (ViPipe,0x33b9, 0x0a);
    sc230ai_write_register (ViPipe,0x33f9, 0x70);
    sc230ai_write_register (ViPipe,0x33fb, 0x90);
    sc230ai_write_register (ViPipe,0x33fc, 0x4b);
    sc230ai_write_register (ViPipe,0x33fd, 0x5f);
    sc230ai_write_register (ViPipe,0x349f, 0x03);
    sc230ai_write_register (ViPipe,0x34a6, 0x4b);
    sc230ai_write_register (ViPipe,0x34a7, 0x4f);
    sc230ai_write_register (ViPipe,0x34a8, 0x30);
    sc230ai_write_register (ViPipe,0x34a9, 0x20);
    sc230ai_write_register (ViPipe,0x34aa, 0x00);
    sc230ai_write_register (ViPipe,0x34ab, 0xe0);
    sc230ai_write_register (ViPipe,0x34ac, 0x01);
    sc230ai_write_register (ViPipe,0x34ad, 0x00);
    sc230ai_write_register (ViPipe,0x34f8, 0x5f);
    sc230ai_write_register (ViPipe,0x34f9, 0x10);
    sc230ai_write_register (ViPipe,0x3630, 0xc0);
    sc230ai_write_register (ViPipe,0x3633, 0x44);
    sc230ai_write_register (ViPipe,0x3637, 0x29);
    sc230ai_write_register (ViPipe,0x363b, 0x20);
    sc230ai_write_register (ViPipe,0x3670, 0x09);
    sc230ai_write_register (ViPipe,0x3674, 0xb0);
    sc230ai_write_register (ViPipe,0x3675, 0x80);
    sc230ai_write_register (ViPipe,0x3676, 0x88);
    sc230ai_write_register (ViPipe,0x367c, 0x40);
    sc230ai_write_register (ViPipe,0x367d, 0x49);
    sc230ai_write_register (ViPipe,0x3690, 0x54);
    sc230ai_write_register (ViPipe,0x3691, 0x44);
    sc230ai_write_register (ViPipe,0x3692, 0x55);
    sc230ai_write_register (ViPipe,0x369c, 0x49);
    sc230ai_write_register (ViPipe,0x369d, 0x4f);
    sc230ai_write_register (ViPipe,0x36ae, 0x4b);
    sc230ai_write_register (ViPipe,0x36af, 0x4f);
    sc230ai_write_register (ViPipe,0x36b0, 0x87);
    sc230ai_write_register (ViPipe,0x36b1, 0x9b);
    sc230ai_write_register (ViPipe,0x36b2, 0xb7);
    sc230ai_write_register (ViPipe,0x36d0, 0x01);
    sc230ai_write_register (ViPipe,0x36ea, 0x09);
    sc230ai_write_register (ViPipe,0x36eb, 0x04);
    sc230ai_write_register (ViPipe,0x36ec, 0x0c);
    sc230ai_write_register (ViPipe,0x36ed, 0x24);
    sc230ai_write_register (ViPipe,0x370f, 0x01);
    sc230ai_write_register (ViPipe,0x3722, 0x17);
    sc230ai_write_register (ViPipe,0x3728, 0x90);
    sc230ai_write_register (ViPipe,0x37b0, 0x17);
    sc230ai_write_register (ViPipe,0x37b1, 0x17);
    sc230ai_write_register (ViPipe,0x37b2, 0x97);
    sc230ai_write_register (ViPipe,0x37b3, 0x4b);
    sc230ai_write_register (ViPipe,0x37b4, 0x4f);
    sc230ai_write_register (ViPipe,0x37fa, 0x09);
    sc230ai_write_register (ViPipe,0x37fb, 0x04);
    sc230ai_write_register (ViPipe,0x37fc, 0x00);
    sc230ai_write_register (ViPipe,0x37fd, 0x22);
    sc230ai_write_register (ViPipe,0x3901, 0x02);
    sc230ai_write_register (ViPipe,0x3902, 0xc5);
    sc230ai_write_register (ViPipe,0x3904, 0x04);
    sc230ai_write_register (ViPipe,0x3907, 0x00);
    sc230ai_write_register (ViPipe,0x3908, 0x41);
    sc230ai_write_register (ViPipe,0x3909, 0x00);
    sc230ai_write_register (ViPipe,0x390a, 0x00);
    sc230ai_write_register (ViPipe,0x391f, 0x04);
    sc230ai_write_register (ViPipe,0x3928, 0xc1);
    sc230ai_write_register (ViPipe,0x3933, 0x84);
    sc230ai_write_register (ViPipe,0x3934, 0x02);
    sc230ai_write_register (ViPipe,0x3940, 0x62);
    sc230ai_write_register (ViPipe,0x3941, 0x00);
    sc230ai_write_register (ViPipe,0x3942, 0x04);
    sc230ai_write_register (ViPipe,0x3943, 0x03);
    sc230ai_write_register (ViPipe,0x3e00, 0x00);
    sc230ai_write_register (ViPipe,0x3e01, 0x8c);
    sc230ai_write_register (ViPipe,0x3e02, 0x20);
    sc230ai_write_register (ViPipe,0x440e, 0x02);
    sc230ai_write_register (ViPipe,0x450d, 0x11);
    sc230ai_write_register (ViPipe,0x4819, 0x0a);
    sc230ai_write_register (ViPipe,0x481b, 0x06);
    sc230ai_write_register (ViPipe,0x481d, 0x16);
    sc230ai_write_register (ViPipe,0x481f, 0x05);
    sc230ai_write_register (ViPipe,0x4821, 0x0b);
    sc230ai_write_register (ViPipe,0x4823, 0x05);
    sc230ai_write_register (ViPipe,0x4825, 0x05);
    sc230ai_write_register (ViPipe,0x4827, 0x05);
    sc230ai_write_register (ViPipe,0x4829, 0x09);
    sc230ai_write_register (ViPipe,0x5010, 0x01);
    sc230ai_write_register (ViPipe,0x5787, 0x08);
    sc230ai_write_register (ViPipe,0x5788, 0x03);
    sc230ai_write_register (ViPipe,0x5789, 0x00);
    sc230ai_write_register (ViPipe,0x578a, 0x10);
    sc230ai_write_register (ViPipe,0x578b, 0x08);
    sc230ai_write_register (ViPipe,0x578c, 0x00);
    sc230ai_write_register (ViPipe,0x5790, 0x08);
    sc230ai_write_register (ViPipe,0x5791, 0x04);
    sc230ai_write_register (ViPipe,0x5792, 0x00);
    sc230ai_write_register (ViPipe,0x5793, 0x10);
    sc230ai_write_register (ViPipe,0x5794, 0x08);
    sc230ai_write_register (ViPipe,0x5795, 0x00);
    sc230ai_write_register (ViPipe,0x5799, 0x06);
    sc230ai_write_register (ViPipe,0x57ad, 0x00);
    sc230ai_write_register (ViPipe,0x5ae0, 0xfe);
    sc230ai_write_register (ViPipe,0x5ae1, 0x40);
    sc230ai_write_register (ViPipe,0x5ae2, 0x3f);
    sc230ai_write_register (ViPipe,0x5ae3, 0x38);
    sc230ai_write_register (ViPipe,0x5ae4, 0x28);
    sc230ai_write_register (ViPipe,0x5ae5, 0x3f);
    sc230ai_write_register (ViPipe,0x5ae6, 0x38);
    sc230ai_write_register (ViPipe,0x5ae7, 0x28);
    sc230ai_write_register (ViPipe,0x5ae8, 0x3f);
    sc230ai_write_register (ViPipe,0x5ae9, 0x3c);
    sc230ai_write_register (ViPipe,0x5aea, 0x2c);
    sc230ai_write_register (ViPipe,0x5aeb, 0x3f);
    sc230ai_write_register (ViPipe,0x5aec, 0x3c);
    sc230ai_write_register (ViPipe,0x5aed, 0x2c);
    sc230ai_write_register (ViPipe,0x5af4, 0x3f);
    sc230ai_write_register (ViPipe,0x5af5, 0x38);
    sc230ai_write_register (ViPipe,0x5af6, 0x28);
    sc230ai_write_register (ViPipe,0x5af7, 0x3f);
    sc230ai_write_register (ViPipe,0x5af8, 0x38);
    sc230ai_write_register (ViPipe,0x5af9, 0x28);
    sc230ai_write_register (ViPipe,0x5afa, 0x3f);
    sc230ai_write_register (ViPipe,0x5afb, 0x3c);
    sc230ai_write_register (ViPipe,0x5afc, 0x2c);
    sc230ai_write_register (ViPipe,0x5afd, 0x3f);
    sc230ai_write_register (ViPipe,0x5afe, 0x3c);
    sc230ai_write_register (ViPipe,0x5aff, 0x2c);
    /*sync*/
    sc230ai_write_register (ViPipe,0x3222, 0x01);
    //sc230ai_write_register (ViPipe,0x3225, 0x30);
    sc230ai_write_register (ViPipe,0x3224, 0x92);
    sc230ai_write_register (ViPipe,0x300a, 0x20);

    sc230ai_write_register (ViPipe,0x36e9, 0x53);
    sc230ai_write_register (ViPipe,0x37f9, 0x53);
    sc230ai_write_register (ViPipe,0x0100, 0x01);
    printf("===SC230AI 1080P 25fps 10bit SLAVE Init OK!===\n");
    return;
}


/* 1080P60 */
void sc230ai_linear_1080p60_init(VI_PIPE ViPipe)
{


    ar_always("===IMX290 1080P 60fps 12bit LINE Init OK!===\n");
    return;
}


void sc230ai_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
    
    ar_always("===Sc230ai sensor 1080P15fps 10bit 2to1 WDR(30fps->15fps) init success!=====\n");

    return;

}

void sc230ai_wdr_1080p60_2to1_init(VI_PIPE ViPipe)
{
   
    ar_always("===Sc230ai sensor 1080P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    return;

}

void sc230ai_wdr_1080p30_3to1_init(VI_PIPE ViPipe)
{
    

    ar_always("===Sc230ai sc230ai 1080P15fps 12bit 3to1 WDR(30fps->7p5fps) init success!=====\n");

    return;

}

void sc230ai_wdr_720p60_2to1_init(VI_PIPE ViPipe)
{
   
    ar_always("===Sc230ai sc230ai 720P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    return;
}

void sc230ai_wdr_720p60_3to1_init(VI_PIPE ViPipe)
{
    

    ar_always("===Sc230ai sc230ai 720P15fps 12bit 3to1 WDR(60fps->15fps) init success!=====\n");
    return;
}

void sc230ai_wdr_1080p120_2to1_init(VI_PIPE ViPipe)
{
    

    ar_always("===Sc230ai sc230ai 1080P60fps 10bit 2to1 WDR(120fps->60fps) init success!=====\n");
    return;
}

void sc230ai_wdr_1080p120_3to1_init(VI_PIPE ViPipe)
{
    

    ar_always("===Sc230ai sc230ai 1080P30fps 10bit 3to1 WDR(120fps->30fps) init success!=====\n");

    return;
}
