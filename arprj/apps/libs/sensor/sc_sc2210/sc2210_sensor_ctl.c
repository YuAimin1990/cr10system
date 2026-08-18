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

const unsigned char sc2210_i2c_addr     =    0x60;  // 0x32 for sid low  ; 0x30 for sid high   /* I2C Address of sc2210 */
const unsigned int  sc2210_addr_byte    =    2;
const unsigned int  sc2210_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastsc2210[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunsc2210BusInfo[];

int sc2210_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunsc2210BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

    printf("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (sc2210_i2c_addr >> 1));
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

int sc2210_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}
int sc2210_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:
    return AR_SUCCESS;
}


int sc2210_read_register_1(VI_PIPE ViPipe, char* addr, int addr_size, char *data, int data_len)
{
    // TODO:
    return ar_hal_i2c_master_read(g_fd[ViPipe], sc2210_i2c_addr>>1, 7, addr, addr_size, data, data_len);
}


int sc2210_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = sc2210_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sc2210_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sc2210_data_byte;

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

    if (sc2210_addr_byte == 2)
    {
        buf[idx] = (addr >> 8) & 0xff;
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    }
    else
    {
        // buf[idx] = addr & 0xff;
        // idx++;
    }

    if (sc2210_data_byte == 2)
    {
        // buf[idx] = (data >> 8) & 0xff;
        // idx++;
        // buf[idx] = data & 0xff;
        // idx++;
    }
    else
    {
        buf[idx] = data & 0xff;
        idx++;
    }
    // printf("write addr[0x%x] data : 0x%x \n", addr, buf[2]);

    ret = write(g_fd[ViPipe], buf, sc2210_addr_byte + sc2210_data_byte);
    if (ret < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "I2C_WRITE error!\n");
        return AR_FAILURE;
    }
    // char r_data[4] = {0};
    // sc2210_read_register_1(ViPipe, buf, sc2210_addr_byte, r_data, sc2210_data_byte);
    // printf("read addr[0x%x] data : 0x%x \n", addr, r_data[0]);
#endif
    return AR_SUCCESS;
}


static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void sc2210_prog(VI_PIPE ViPipe, int *rom)
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
            sc2210_write_register(ViPipe, addr, data);
        }
    }
}

void sc2210_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void sc2210_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define sc2210_FLIP_MIRRO (0x3221)

int sc2210_flip_off_mirro_off(VI_PIPE ViPipe)
{
    sc2210_write_register(ViPipe, sc2210_FLIP_MIRRO, 0x00);
    return 0;
}

int sc2210_flip_on_mirro_off(VI_PIPE ViPipe)
{
    sc2210_write_register(ViPipe, sc2210_FLIP_MIRRO, 0x60);
    return 0;
}

int sc2210_flip_off_mirro_on(VI_PIPE ViPipe)
{
    sc2210_write_register(ViPipe, sc2210_FLIP_MIRRO, 0x06);
    return 0;
}

int sc2210_flip_on_mirro_on(VI_PIPE ViPipe)
{
    sc2210_write_register(ViPipe, sc2210_FLIP_MIRRO, 0x66);
    return 0;
}

#define sc2210_SENSOR_2M_30FPS_LINEAR_MODE      (1)
#define sc2210_SENSOR_2M_50FPS_LINEAR_MODE      (2)
#define sc2210_SENSOR_2M_60FPS_LINEAR_MODE      (3)

void sc2210_linear_2M_30fps_init(VI_PIPE ViPipe);
void sc2210_linear_2M_50fps_init(VI_PIPE ViPipe);
void sc2210_linear_2M_60fps_init(VI_PIPE ViPipe);
void sc2210_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastsc2210[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        // printf("set reg[0x%x]: 0x%x \n", g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            // printf("set reg[0x%x]: 0x%x \n", g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            sc2210_write_register(ViPipe, g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void sc2210_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastsc2210[ViPipe]->bInit;
    enWDRMode   = g_pastsc2210[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastsc2210[ViPipe]->u8ImgMode;

    sc2210_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (u8ImgMode == sc2210_SENSOR_2M_30FPS_LINEAR_MODE)
            sc2210_linear_2M_30fps_init(ViPipe);
        else if (u8ImgMode == sc2210_SENSOR_2M_50FPS_LINEAR_MODE)
            sc2210_linear_2M_50fps_init(ViPipe);
        else if (u8ImgMode == sc2210_SENSOR_2M_60FPS_LINEAR_MODE)
            sc2210_linear_2M_60fps_init(ViPipe);
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {
        if (u8ImgMode == sc2210_SENSOR_2M_30FPS_LINEAR_MODE)
            sc2210_linear_2M_30fps_init(ViPipe);
        else if (u8ImgMode == sc2210_SENSOR_2M_50FPS_LINEAR_MODE)
            sc2210_linear_2M_50fps_init(ViPipe);
        else if (u8ImgMode == sc2210_SENSOR_2M_60FPS_LINEAR_MODE)
            sc2210_linear_2M_60fps_init(ViPipe);
    }

    // for (i = 0; i < g_pastsc2210[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     sc2210_write_register(ViPipe, g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc2210[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastsc2210[ViPipe]->bInit = AR_TRUE;
    return ;
}

void sc2210_exit(VI_PIPE ViPipe)
{
    sc2210_i2c_exit(ViPipe);

    return;
}


/* 1C2D 446Mbps 12bit 1920*1080 30fps */
void sc2210_linear_2M_30fps_init(VI_PIPE ViPipe)
{
    sc2210_write_register (ViPipe, 0x0103, 0x01);
    sc2210_write_register (ViPipe, 0x0100, 0x00);
    sc2210_write_register (ViPipe, 0x36e9, 0x80);
    sc2210_write_register (ViPipe, 0x36f9, 0x80);
    sc2210_write_register (ViPipe, 0x3001, 0x07);
    sc2210_write_register (ViPipe, 0x3002, 0xc0);
    sc2210_write_register (ViPipe, 0x300a, 0x2c);
    sc2210_write_register (ViPipe, 0x300f, 0x00);
    sc2210_write_register (ViPipe, 0x3018, 0x33);
    sc2210_write_register (ViPipe, 0x3019, 0x0c);
    sc2210_write_register (ViPipe, 0x301f, 0x02);
    sc2210_write_register (ViPipe, 0x3031, 0x0c);
    sc2210_write_register (ViPipe, 0x3033, 0x20);
    sc2210_write_register (ViPipe, 0x3038, 0x22);
    sc2210_write_register (ViPipe, 0x3106, 0x81);
    sc2210_write_register (ViPipe, 0x3201, 0x04);
    sc2210_write_register (ViPipe, 0x3203, 0x04);
    sc2210_write_register (ViPipe, 0x3204, 0x07);
    sc2210_write_register (ViPipe, 0x3205, 0x8b);
    sc2210_write_register (ViPipe, 0x3206, 0x04);
    sc2210_write_register (ViPipe, 0x3207, 0x43);
    sc2210_write_register (ViPipe, 0x320c, 0x04);
    sc2210_write_register (ViPipe, 0x320d, 0x4c); // line length
    sc2210_write_register (ViPipe, 0x320e, 0x04);
    sc2210_write_register (ViPipe, 0x320f, 0x65); // frame length
    sc2210_write_register (ViPipe, 0x3211, 0x04);
    sc2210_write_register (ViPipe, 0x3213, 0x04);
    sc2210_write_register (ViPipe, 0x3231, 0x02);
    sc2210_write_register (ViPipe, 0x3253, 0x04);
    sc2210_write_register (ViPipe, 0x3301, 0x0a);
    sc2210_write_register (ViPipe, 0x3302, 0x10);
    sc2210_write_register (ViPipe, 0x3304, 0x48);
    sc2210_write_register (ViPipe, 0x3305, 0x00);
    sc2210_write_register (ViPipe, 0x3306, 0x68);
    sc2210_write_register (ViPipe, 0x3308, 0x20);
    sc2210_write_register (ViPipe, 0x3309, 0x98);
    sc2210_write_register (ViPipe, 0x330a, 0x00);
    sc2210_write_register (ViPipe, 0x330b, 0xe8);
    sc2210_write_register (ViPipe, 0x330e, 0x68);
    sc2210_write_register (ViPipe, 0x3314, 0x92);
    sc2210_write_register (ViPipe, 0x331e, 0x41);
    sc2210_write_register (ViPipe, 0x3000, 0xc0);
    sc2210_write_register (ViPipe, 0x331f, 0x91);
    sc2210_write_register (ViPipe, 0x334c, 0x10);
    sc2210_write_register (ViPipe, 0x335d, 0x60);
    sc2210_write_register (ViPipe, 0x335e, 0x02);
    sc2210_write_register (ViPipe, 0x335f, 0x06);
    sc2210_write_register (ViPipe, 0x3364, 0x16);
    sc2210_write_register (ViPipe, 0x3366, 0x92);
    sc2210_write_register (ViPipe, 0x3367, 0x10);
    sc2210_write_register (ViPipe, 0x3368, 0x04);
    sc2210_write_register (ViPipe, 0x3369, 0x00);
    sc2210_write_register (ViPipe, 0x336a, 0x00);
    sc2210_write_register (ViPipe, 0x336b, 0x00);
    sc2210_write_register (ViPipe, 0x336d, 0x03);
    sc2210_write_register (ViPipe, 0x337c, 0x08);
    sc2210_write_register (ViPipe, 0x337d, 0x0e);
    sc2210_write_register (ViPipe, 0x337f, 0x33);
    sc2210_write_register (ViPipe, 0x3390, 0x10);
    sc2210_write_register (ViPipe, 0x3391, 0x30);
    sc2210_write_register (ViPipe, 0x3392, 0x40);
    sc2210_write_register (ViPipe, 0x3393, 0x0a);
    sc2210_write_register (ViPipe, 0x3394, 0x0a);
    sc2210_write_register (ViPipe, 0x3395, 0x0a);
    sc2210_write_register (ViPipe, 0x3396, 0x08);
    sc2210_write_register (ViPipe, 0x3397, 0x30);
    sc2210_write_register (ViPipe, 0x3398, 0x3f);
    sc2210_write_register (ViPipe, 0x3399, 0x50);
    sc2210_write_register (ViPipe, 0x339a, 0x50);
    sc2210_write_register (ViPipe, 0x339b, 0x50);
    sc2210_write_register (ViPipe, 0x339c, 0x50);
    sc2210_write_register (ViPipe, 0x33a2, 0x0a);
    sc2210_write_register (ViPipe, 0x33b9, 0x0e);
    sc2210_write_register (ViPipe, 0x33e1, 0x08);
    sc2210_write_register (ViPipe, 0x33e2, 0x18);
    sc2210_write_register (ViPipe, 0x33e3, 0x18);
    sc2210_write_register (ViPipe, 0x33e4, 0x18);
    sc2210_write_register (ViPipe, 0x33e5, 0x10);
    sc2210_write_register (ViPipe, 0x33e6, 0x06);
    sc2210_write_register (ViPipe, 0x33e7, 0x02);
    sc2210_write_register (ViPipe, 0x33e8, 0x18);
    sc2210_write_register (ViPipe, 0x33e9, 0x10);
    sc2210_write_register (ViPipe, 0x33ea, 0x0c);
    sc2210_write_register (ViPipe, 0x33eb, 0x10);
    sc2210_write_register (ViPipe, 0x33ec, 0x04);
    sc2210_write_register (ViPipe, 0x33ed, 0x02);
    sc2210_write_register (ViPipe, 0x33ee, 0xa0);
    sc2210_write_register (ViPipe, 0x33ef, 0x08);
    sc2210_write_register (ViPipe, 0x33f4, 0x18);
    sc2210_write_register (ViPipe, 0x33f5, 0x10);
    sc2210_write_register (ViPipe, 0x33f6, 0x0c);
    sc2210_write_register (ViPipe, 0x33f7, 0x10);
    sc2210_write_register (ViPipe, 0x33f8, 0x06);
    sc2210_write_register (ViPipe, 0x33f9, 0x02);
    sc2210_write_register (ViPipe, 0x33fa, 0x18);
    sc2210_write_register (ViPipe, 0x33fb, 0x10);
    sc2210_write_register (ViPipe, 0x33fc, 0x0c);
    sc2210_write_register (ViPipe, 0x33fd, 0x10);
    sc2210_write_register (ViPipe, 0x33fe, 0x04);
    sc2210_write_register (ViPipe, 0x33ff, 0x02);
    sc2210_write_register (ViPipe, 0x360f, 0x01);
    sc2210_write_register (ViPipe, 0x3622, 0xf7);
    sc2210_write_register (ViPipe, 0x3625, 0x0a);
    sc2210_write_register (ViPipe, 0x3627, 0x02);
    sc2210_write_register (ViPipe, 0x3630, 0xa2);
    sc2210_write_register (ViPipe, 0x3631, 0x00);
    sc2210_write_register (ViPipe, 0x3632, 0xd8);
    sc2210_write_register (ViPipe, 0x3633, 0x33);
    sc2210_write_register (ViPipe, 0x3635, 0x20);
    sc2210_write_register (ViPipe, 0x3638, 0x24);
    sc2210_write_register (ViPipe, 0x363a, 0x80);
    sc2210_write_register (ViPipe, 0x363b, 0x02);
    sc2210_write_register (ViPipe, 0x363e, 0x22);
    sc2210_write_register (ViPipe, 0x3670, 0x40);
    sc2210_write_register (ViPipe, 0x3671, 0xf7);
    sc2210_write_register (ViPipe, 0x3672, 0xf7);
    sc2210_write_register (ViPipe, 0x3673, 0x07);
    sc2210_write_register (ViPipe, 0x367a, 0x40);
    sc2210_write_register (ViPipe, 0x367b, 0x7f);
    sc2210_write_register (ViPipe, 0x36b5, 0x40);
    sc2210_write_register (ViPipe, 0x36b6, 0x7f);
    sc2210_write_register (ViPipe, 0x36c0, 0x80);
    sc2210_write_register (ViPipe, 0x36c1, 0x9f);
    sc2210_write_register (ViPipe, 0x36c2, 0x9f);
    sc2210_write_register (ViPipe, 0x36cc, 0x22);
    sc2210_write_register (ViPipe, 0x36cd, 0x23);
    sc2210_write_register (ViPipe, 0x36ce, 0x30);
    sc2210_write_register (ViPipe, 0x36d0, 0x20);
    sc2210_write_register (ViPipe, 0x36d1, 0x40);
    sc2210_write_register (ViPipe, 0x36d2, 0x7f);
    sc2210_write_register (ViPipe, 0x36ea, 0x35);
    sc2210_write_register (ViPipe, 0x36eb, 0x0f);
    sc2210_write_register (ViPipe, 0x36ec, 0x13);
    sc2210_write_register (ViPipe, 0x36ed, 0x14);
    sc2210_write_register (ViPipe, 0x36fa, 0x35);
    sc2210_write_register (ViPipe, 0x36fb, 0x1b);
    sc2210_write_register (ViPipe, 0x36fc, 0x10);
    sc2210_write_register (ViPipe, 0x36fd, 0x14);
    sc2210_write_register (ViPipe, 0x3905, 0xd8);
    sc2210_write_register (ViPipe, 0x3907, 0x01);
    sc2210_write_register (ViPipe, 0x3908, 0x11);
    sc2210_write_register (ViPipe, 0x391b, 0x83);
    sc2210_write_register (ViPipe, 0x391f, 0x00);
    sc2210_write_register (ViPipe, 0x3933, 0x28);
    sc2210_write_register (ViPipe, 0x3934, 0xa6);
    sc2210_write_register (ViPipe, 0x3940, 0x70);
    sc2210_write_register (ViPipe, 0x3942, 0x08);
    sc2210_write_register (ViPipe, 0x3943, 0xbc);
    sc2210_write_register (ViPipe, 0x3958, 0x02);
    sc2210_write_register (ViPipe, 0x3959, 0x04);
    sc2210_write_register (ViPipe, 0x3980, 0x61);
    sc2210_write_register (ViPipe, 0x3987, 0x0b);
    sc2210_write_register (ViPipe, 0x3990, 0x00);
    sc2210_write_register (ViPipe, 0x3991, 0x00);
    sc2210_write_register (ViPipe, 0x3992, 0x00);
    sc2210_write_register (ViPipe, 0x3993, 0x00);
    sc2210_write_register (ViPipe, 0x3994, 0x00);
    sc2210_write_register (ViPipe, 0x3995, 0x00);
    sc2210_write_register (ViPipe, 0x3996, 0x00);
    sc2210_write_register (ViPipe, 0x3997, 0x00);
    sc2210_write_register (ViPipe, 0x3998, 0x00);
    sc2210_write_register (ViPipe, 0x3999, 0x00);
    sc2210_write_register (ViPipe, 0x399a, 0x00);
    sc2210_write_register (ViPipe, 0x399b, 0x00);
    sc2210_write_register (ViPipe, 0x399c, 0x00);
    sc2210_write_register (ViPipe, 0x399d, 0x00);
    sc2210_write_register (ViPipe, 0x399e, 0x00);
    sc2210_write_register (ViPipe, 0x399f, 0x00);
    sc2210_write_register (ViPipe, 0x39a0, 0x00);
    sc2210_write_register (ViPipe, 0x39a1, 0x00);
    sc2210_write_register (ViPipe, 0x39a2, 0x03);
    sc2210_write_register (ViPipe, 0x39a3, 0x30);
    sc2210_write_register (ViPipe, 0x39a4, 0x03);
    sc2210_write_register (ViPipe, 0x39a5, 0x60);
    sc2210_write_register (ViPipe, 0x39a6, 0x03);
    sc2210_write_register (ViPipe, 0x39a7, 0xa0);
    sc2210_write_register (ViPipe, 0x39a8, 0x03);
    sc2210_write_register (ViPipe, 0x39a9, 0xb0);
    sc2210_write_register (ViPipe, 0x39aa, 0x00);
    sc2210_write_register (ViPipe, 0x39ab, 0x00);
    sc2210_write_register (ViPipe, 0x39ac, 0x00);
    sc2210_write_register (ViPipe, 0x39ad, 0x20);
    sc2210_write_register (ViPipe, 0x39ae, 0x00);
    sc2210_write_register (ViPipe, 0x39af, 0x40);
    sc2210_write_register (ViPipe, 0x39b0, 0x00);
    sc2210_write_register (ViPipe, 0x39b1, 0x60);
    sc2210_write_register (ViPipe, 0x39b2, 0x00);
    sc2210_write_register (ViPipe, 0x39b3, 0x00);
    sc2210_write_register (ViPipe, 0x39b4, 0x08);
    sc2210_write_register (ViPipe, 0x39b5, 0x14);
    sc2210_write_register (ViPipe, 0x39b6, 0x20);
    sc2210_write_register (ViPipe, 0x39b7, 0x38);
    sc2210_write_register (ViPipe, 0x39b8, 0x38);
    sc2210_write_register (ViPipe, 0x39b9, 0x20);
    sc2210_write_register (ViPipe, 0x39ba, 0x14);
    sc2210_write_register (ViPipe, 0x39bb, 0x08);
    sc2210_write_register (ViPipe, 0x39bc, 0x08);
    sc2210_write_register (ViPipe, 0x39bd, 0x10);
    sc2210_write_register (ViPipe, 0x39be, 0x20);
    sc2210_write_register (ViPipe, 0x39bf, 0x30);
    sc2210_write_register (ViPipe, 0x39c0, 0x30);
    sc2210_write_register (ViPipe, 0x39c1, 0x20);
    sc2210_write_register (ViPipe, 0x39c2, 0x10);
    sc2210_write_register (ViPipe, 0x39c3, 0x08);
    sc2210_write_register (ViPipe, 0x39c4, 0x00);
    sc2210_write_register (ViPipe, 0x39c5, 0x80);
    sc2210_write_register (ViPipe, 0x39c6, 0x00);
    sc2210_write_register (ViPipe, 0x39c7, 0x80);
    sc2210_write_register (ViPipe, 0x39c8, 0x00);
    sc2210_write_register (ViPipe, 0x39c9, 0x00);
    sc2210_write_register (ViPipe, 0x39ca, 0x80);
    sc2210_write_register (ViPipe, 0x39cb, 0x00);
    sc2210_write_register (ViPipe, 0x39cc, 0x00);
    sc2210_write_register (ViPipe, 0x39cd, 0x00);
    sc2210_write_register (ViPipe, 0x39ce, 0x00);
    sc2210_write_register (ViPipe, 0x39cf, 0x00);
    sc2210_write_register (ViPipe, 0x39d0, 0x00);
    sc2210_write_register (ViPipe, 0x39d1, 0x00);
    sc2210_write_register (ViPipe, 0x39e2, 0x05);
    sc2210_write_register (ViPipe, 0x39e3, 0xeb);
    sc2210_write_register (ViPipe, 0x39e4, 0x07);
    sc2210_write_register (ViPipe, 0x39e5, 0xb6);
    sc2210_write_register (ViPipe, 0x39e6, 0x00);
    sc2210_write_register (ViPipe, 0x39e7, 0x3a);
    sc2210_write_register (ViPipe, 0x39e8, 0x3f);
    sc2210_write_register (ViPipe, 0x39e9, 0xb7);
    sc2210_write_register (ViPipe, 0x39ea, 0x02);
    sc2210_write_register (ViPipe, 0x39eb, 0x4f);
    sc2210_write_register (ViPipe, 0x39ec, 0x08);
    sc2210_write_register (ViPipe, 0x39ed, 0x00);
    sc2210_write_register (ViPipe, 0x3e01, 0x46);
    sc2210_write_register (ViPipe, 0x3e02, 0x10);
    // 0x3e03 default 0b
#ifdef ENABLE_GAIN_03_MODE
    sc2210_write_register (ViPipe, 0x3e03, 0x03);
#else
    sc2210_write_register (ViPipe, 0x3e03, 0x0b);
#endif
    sc2210_write_register (ViPipe, 0x3e09, 0x40);
    sc2210_write_register (ViPipe, 0x3e14, 0x31);
    sc2210_write_register (ViPipe, 0x3e1b, 0x3a);
    sc2210_write_register (ViPipe, 0x3e26, 0x40);
    sc2210_write_register (ViPipe, 0x4401, 0x1a);
    sc2210_write_register (ViPipe, 0x4407, 0xc0);
    sc2210_write_register (ViPipe, 0x4418, 0x34);
    sc2210_write_register (ViPipe, 0x4500, 0x18);
    sc2210_write_register (ViPipe, 0x4501, 0xb4);
    sc2210_write_register (ViPipe, 0x4509, 0x20);
    sc2210_write_register (ViPipe, 0x4603, 0x00);
    sc2210_write_register (ViPipe, 0x4800, 0x24);
    sc2210_write_register (ViPipe, 0x4837, 0x24);
    sc2210_write_register (ViPipe, 0x5000, 0x0e);
    sc2210_write_register (ViPipe, 0x550f, 0x20);
    sc2210_write_register (ViPipe, 0x36e9, 0x21);
    sc2210_write_register (ViPipe, 0x36f9, 0x21);
    sc2210_default_reg_init(ViPipe);
    sc2210_write_register (ViPipe, 0x0100, 0x01);

    printf("===sc2210 2M 30fps 12bit LINE Init OK!===\n");
    return;
}

/* 1C2D 864Mbps 12bit 1920*1080 50fps */
void sc2210_linear_2M_50fps_init(VI_PIPE ViPipe)
{
    sc2210_write_register (ViPipe, 0x0103, 0x01);
    sc2210_write_register (ViPipe, 0x0100, 0x00);
    sc2210_write_register (ViPipe, 0x36e9, 0x80);
    sc2210_write_register (ViPipe, 0x36f9, 0x80);
    sc2210_write_register (ViPipe, 0x3001, 0x07);
    sc2210_write_register (ViPipe, 0x3002, 0xc0);
    sc2210_write_register (ViPipe, 0x300a, 0x2c);
    sc2210_write_register (ViPipe, 0x300f, 0x00);
    sc2210_write_register (ViPipe, 0x3018, 0x33);
    sc2210_write_register (ViPipe, 0x3019, 0x0c);
    sc2210_write_register (ViPipe, 0x301f, 0x01);
    sc2210_write_register (ViPipe, 0x3031, 0x0c);
    sc2210_write_register (ViPipe, 0x3033, 0x20);
    sc2210_write_register (ViPipe, 0x3038, 0x22);
    sc2210_write_register (ViPipe, 0x3106, 0x81);
    sc2210_write_register (ViPipe, 0x3201, 0x04);
    sc2210_write_register (ViPipe, 0x3203, 0x04);
    sc2210_write_register (ViPipe, 0x3204, 0x07);
    sc2210_write_register (ViPipe, 0x3205, 0x8b);
    sc2210_write_register (ViPipe, 0x3206, 0x04);
    sc2210_write_register (ViPipe, 0x3207, 0x43);
    sc2210_write_register (ViPipe, 0x320c, 0x04);
    sc2210_write_register (ViPipe, 0x320d, 0x37); // line length
    sc2210_write_register (ViPipe, 0x320e, 0x05);
    sc2210_write_register (ViPipe, 0x320f, 0x36); // frame length
    sc2210_write_register (ViPipe, 0x3211, 0x04);
    sc2210_write_register (ViPipe, 0x3213, 0x04);
    sc2210_write_register (ViPipe, 0x3231, 0x02);
    sc2210_write_register (ViPipe, 0x3253, 0x04);
    sc2210_write_register (ViPipe, 0x3301, 0x0a);
    sc2210_write_register (ViPipe, 0x3302, 0x10);
    sc2210_write_register (ViPipe, 0x3304, 0x58);
    sc2210_write_register (ViPipe, 0x3305, 0x00);
    sc2210_write_register (ViPipe, 0x3306, 0xb0);
    sc2210_write_register (ViPipe, 0x3308, 0x20);
    sc2210_write_register (ViPipe, 0x3309, 0x98);
    sc2210_write_register (ViPipe, 0x330a, 0x01);
    sc2210_write_register (ViPipe, 0x330b, 0x68);
    sc2210_write_register (ViPipe, 0x330e, 0x48);
    sc2210_write_register (ViPipe, 0x3314, 0x92);
    sc2210_write_register (ViPipe, 0x331e, 0x49);
    sc2210_write_register (ViPipe, 0x3000, 0xc0);
    sc2210_write_register (ViPipe, 0x331f, 0x89);
    sc2210_write_register (ViPipe, 0x334c, 0x10);
    sc2210_write_register (ViPipe, 0x335d, 0x60);
    sc2210_write_register (ViPipe, 0x335e, 0x02);
    sc2210_write_register (ViPipe, 0x335f, 0x06);
    sc2210_write_register (ViPipe, 0x3364, 0x16);
    sc2210_write_register (ViPipe, 0x3366, 0x92);
    sc2210_write_register (ViPipe, 0x3367, 0x10);
    sc2210_write_register (ViPipe, 0x3368, 0x04);
    sc2210_write_register (ViPipe, 0x3369, 0x00);
    sc2210_write_register (ViPipe, 0x336a, 0x00);
    sc2210_write_register (ViPipe, 0x336b, 0x00);
    sc2210_write_register (ViPipe, 0x336d, 0x03);
    sc2210_write_register (ViPipe, 0x337c, 0x08);
    sc2210_write_register (ViPipe, 0x337d, 0x0e);
    sc2210_write_register (ViPipe, 0x337f, 0x33);
    sc2210_write_register (ViPipe, 0x3390, 0x10);
    sc2210_write_register (ViPipe, 0x3391, 0x30);
    sc2210_write_register (ViPipe, 0x3392, 0x40);
    sc2210_write_register (ViPipe, 0x3393, 0x0a);
    sc2210_write_register (ViPipe, 0x3394, 0x0a);
    sc2210_write_register (ViPipe, 0x3395, 0x0a);
    sc2210_write_register (ViPipe, 0x3396, 0x08);
    sc2210_write_register (ViPipe, 0x3397, 0x30);
    sc2210_write_register (ViPipe, 0x3398, 0x3f);
    sc2210_write_register (ViPipe, 0x3399, 0x30);
    sc2210_write_register (ViPipe, 0x339a, 0x30);
    sc2210_write_register (ViPipe, 0x339b, 0x30);
    sc2210_write_register (ViPipe, 0x339c, 0x30);
    sc2210_write_register (ViPipe, 0x33a2, 0x0a);
    sc2210_write_register (ViPipe, 0x33b9, 0x0e);
    sc2210_write_register (ViPipe, 0x33e1, 0x08);
    sc2210_write_register (ViPipe, 0x33e2, 0x18);
    sc2210_write_register (ViPipe, 0x33e3, 0x18);
    sc2210_write_register (ViPipe, 0x33e4, 0x18);
    sc2210_write_register (ViPipe, 0x33e5, 0x10);
    sc2210_write_register (ViPipe, 0x33e6, 0x06);
    sc2210_write_register (ViPipe, 0x33e7, 0x02);
    sc2210_write_register (ViPipe, 0x33e8, 0x18);
    sc2210_write_register (ViPipe, 0x33e9, 0x10);
    sc2210_write_register (ViPipe, 0x33ea, 0x0c);
    sc2210_write_register (ViPipe, 0x33eb, 0x10);
    sc2210_write_register (ViPipe, 0x33ec, 0x04);
    sc2210_write_register (ViPipe, 0x33ed, 0x02);
    sc2210_write_register (ViPipe, 0x33ee, 0xa0);
    sc2210_write_register (ViPipe, 0x33ef, 0x08);
    sc2210_write_register (ViPipe, 0x33f4, 0x18);
    sc2210_write_register (ViPipe, 0x33f5, 0x10);
    sc2210_write_register (ViPipe, 0x33f6, 0x0c);
    sc2210_write_register (ViPipe, 0x33f7, 0x10);
    sc2210_write_register (ViPipe, 0x33f8, 0x06);
    sc2210_write_register (ViPipe, 0x33f9, 0x02);
    sc2210_write_register (ViPipe, 0x33fa, 0x18);
    sc2210_write_register (ViPipe, 0x33fb, 0x10);
    sc2210_write_register (ViPipe, 0x33fc, 0x0c);
    sc2210_write_register (ViPipe, 0x33fd, 0x10);
    sc2210_write_register (ViPipe, 0x33fe, 0x04);
    sc2210_write_register (ViPipe, 0x33ff, 0x02);
    sc2210_write_register (ViPipe, 0x360f, 0x01);
    sc2210_write_register (ViPipe, 0x3622, 0xf7);
    sc2210_write_register (ViPipe, 0x3625, 0x0a);
    sc2210_write_register (ViPipe, 0x3627, 0x02);
    sc2210_write_register (ViPipe, 0x3630, 0xa2);
    sc2210_write_register (ViPipe, 0x3631, 0x00);
    sc2210_write_register (ViPipe, 0x3632, 0xd8);
    sc2210_write_register (ViPipe, 0x3633, 0x43);
    sc2210_write_register (ViPipe, 0x3635, 0x20);
    sc2210_write_register (ViPipe, 0x3638, 0x24);
    sc2210_write_register (ViPipe, 0x363a, 0x80);
    sc2210_write_register (ViPipe, 0x363b, 0x02);
    sc2210_write_register (ViPipe, 0x363e, 0x22);
    sc2210_write_register (ViPipe, 0x3670, 0x48);
    sc2210_write_register (ViPipe, 0x3671, 0xf7);
    sc2210_write_register (ViPipe, 0x3672, 0xf7);
    sc2210_write_register (ViPipe, 0x3673, 0x07);
    sc2210_write_register (ViPipe, 0x367a, 0x40);
    sc2210_write_register (ViPipe, 0x367b, 0x7f);
    sc2210_write_register (ViPipe, 0x3690, 0x42);
    sc2210_write_register (ViPipe, 0x3691, 0x43);
    sc2210_write_register (ViPipe, 0x3692, 0x54);
    sc2210_write_register (ViPipe, 0x369c, 0x40);
    sc2210_write_register (ViPipe, 0x369d, 0x7f);
    sc2210_write_register (ViPipe, 0x36b5, 0x40);
    sc2210_write_register (ViPipe, 0x36b6, 0x7f);
    sc2210_write_register (ViPipe, 0x36c0, 0x80);
    sc2210_write_register (ViPipe, 0x36c1, 0x9f);
    sc2210_write_register (ViPipe, 0x36c2, 0x9f);
    sc2210_write_register (ViPipe, 0x36cc, 0x20);
    sc2210_write_register (ViPipe, 0x36cd, 0x20);
    sc2210_write_register (ViPipe, 0x36ce, 0x30);
    sc2210_write_register (ViPipe, 0x36d0, 0x20);
    sc2210_write_register (ViPipe, 0x36d1, 0x40);
    sc2210_write_register (ViPipe, 0x36d2, 0x7f);
    sc2210_write_register (ViPipe, 0x36ea, 0x38);
    sc2210_write_register (ViPipe, 0x36eb, 0x0e);
    sc2210_write_register (ViPipe, 0x36ec, 0x03);
    sc2210_write_register (ViPipe, 0x36ed, 0x14);
    sc2210_write_register (ViPipe, 0x36fa, 0x3a);
    sc2210_write_register (ViPipe, 0x36fb, 0x15);
    sc2210_write_register (ViPipe, 0x36fc, 0x01);
    sc2210_write_register (ViPipe, 0x36fd, 0x14);
    sc2210_write_register (ViPipe, 0x3905, 0xd8);
    sc2210_write_register (ViPipe, 0x3907, 0x01);
    sc2210_write_register (ViPipe, 0x3908, 0x11);
    sc2210_write_register (ViPipe, 0x391b, 0x83);
    sc2210_write_register (ViPipe, 0x391f, 0x00);
    sc2210_write_register (ViPipe, 0x3933, 0x28);
    sc2210_write_register (ViPipe, 0x3934, 0xa6);
    sc2210_write_register (ViPipe, 0x3940, 0x70);
    sc2210_write_register (ViPipe, 0x3942, 0x08);
    sc2210_write_register (ViPipe, 0x3943, 0xbc);
    sc2210_write_register (ViPipe, 0x3958, 0x02);
    sc2210_write_register (ViPipe, 0x3959, 0x04);
    sc2210_write_register (ViPipe, 0x3980, 0x61);
    sc2210_write_register (ViPipe, 0x3987, 0x0b);
    sc2210_write_register (ViPipe, 0x3990, 0x00);
    sc2210_write_register (ViPipe, 0x3991, 0x00);
    sc2210_write_register (ViPipe, 0x3992, 0x00);
    sc2210_write_register (ViPipe, 0x3993, 0x00);
    sc2210_write_register (ViPipe, 0x3994, 0x00);
    sc2210_write_register (ViPipe, 0x3995, 0x00);
    sc2210_write_register (ViPipe, 0x3996, 0x00);
    sc2210_write_register (ViPipe, 0x3997, 0x00);
    sc2210_write_register (ViPipe, 0x3998, 0x00);
    sc2210_write_register (ViPipe, 0x3999, 0x00);
    sc2210_write_register (ViPipe, 0x399a, 0x00);
    sc2210_write_register (ViPipe, 0x399b, 0x00);
    sc2210_write_register (ViPipe, 0x399c, 0x00);
    sc2210_write_register (ViPipe, 0x399d, 0x00);
    sc2210_write_register (ViPipe, 0x399e, 0x00);
    sc2210_write_register (ViPipe, 0x399f, 0x00);
    sc2210_write_register (ViPipe, 0x39a0, 0x00);
    sc2210_write_register (ViPipe, 0x39a1, 0x00);
    sc2210_write_register (ViPipe, 0x39a2, 0x03);
    sc2210_write_register (ViPipe, 0x39a3, 0x30);
    sc2210_write_register (ViPipe, 0x39a4, 0x03);
    sc2210_write_register (ViPipe, 0x39a5, 0x60);
    sc2210_write_register (ViPipe, 0x39a6, 0x03);
    sc2210_write_register (ViPipe, 0x39a7, 0xa0);
    sc2210_write_register (ViPipe, 0x39a8, 0x03);
    sc2210_write_register (ViPipe, 0x39a9, 0xb0);
    sc2210_write_register (ViPipe, 0x39aa, 0x00);
    sc2210_write_register (ViPipe, 0x39ab, 0x00);
    sc2210_write_register (ViPipe, 0x39ac, 0x00);
    sc2210_write_register (ViPipe, 0x39ad, 0x20);
    sc2210_write_register (ViPipe, 0x39ae, 0x00);
    sc2210_write_register (ViPipe, 0x39af, 0x40);
    sc2210_write_register (ViPipe, 0x39b0, 0x00);
    sc2210_write_register (ViPipe, 0x39b1, 0x60);
    sc2210_write_register (ViPipe, 0x39b2, 0x00);
    sc2210_write_register (ViPipe, 0x39b3, 0x00);
    sc2210_write_register (ViPipe, 0x39b4, 0x08);
    sc2210_write_register (ViPipe, 0x39b5, 0x14);
    sc2210_write_register (ViPipe, 0x39b6, 0x20);
    sc2210_write_register (ViPipe, 0x39b7, 0x38);
    sc2210_write_register (ViPipe, 0x39b8, 0x38);
    sc2210_write_register (ViPipe, 0x39b9, 0x20);
    sc2210_write_register (ViPipe, 0x39ba, 0x14);
    sc2210_write_register (ViPipe, 0x39bb, 0x08);
    sc2210_write_register (ViPipe, 0x39bc, 0x08);
    sc2210_write_register (ViPipe, 0x39bd, 0x10);
    sc2210_write_register (ViPipe, 0x39be, 0x20);
    sc2210_write_register (ViPipe, 0x39bf, 0x30);
    sc2210_write_register (ViPipe, 0x39c0, 0x30);
    sc2210_write_register (ViPipe, 0x39c1, 0x20);
    sc2210_write_register (ViPipe, 0x39c2, 0x10);
    sc2210_write_register (ViPipe, 0x39c3, 0x08);
    sc2210_write_register (ViPipe, 0x39c4, 0x00);
    sc2210_write_register (ViPipe, 0x39c5, 0x80);
    sc2210_write_register (ViPipe, 0x39c6, 0x00);
    sc2210_write_register (ViPipe, 0x39c7, 0x80);
    sc2210_write_register (ViPipe, 0x39c8, 0x00);
    sc2210_write_register (ViPipe, 0x39c9, 0x00);
    sc2210_write_register (ViPipe, 0x39ca, 0x80);
    sc2210_write_register (ViPipe, 0x39cb, 0x00);
    sc2210_write_register (ViPipe, 0x39cc, 0x00);
    sc2210_write_register (ViPipe, 0x39cd, 0x00);
    sc2210_write_register (ViPipe, 0x39ce, 0x00);
    sc2210_write_register (ViPipe, 0x39cf, 0x00);
    sc2210_write_register (ViPipe, 0x39d0, 0x00);
    sc2210_write_register (ViPipe, 0x39d1, 0x00);
    sc2210_write_register (ViPipe, 0x39e2, 0x05);
    sc2210_write_register (ViPipe, 0x39e3, 0xeb);
    sc2210_write_register (ViPipe, 0x39e4, 0x07);
    sc2210_write_register (ViPipe, 0x39e5, 0xb6);
    sc2210_write_register (ViPipe, 0x39e6, 0x00);
    sc2210_write_register (ViPipe, 0x39e7, 0x3a);
    sc2210_write_register (ViPipe, 0x39e8, 0x3f);
    sc2210_write_register (ViPipe, 0x39e9, 0xb7);
    sc2210_write_register (ViPipe, 0x39ea, 0x02);
    sc2210_write_register (ViPipe, 0x39eb, 0x4f);
    sc2210_write_register (ViPipe, 0x39ec, 0x08);
    sc2210_write_register (ViPipe, 0x39ed, 0x00);
    sc2210_write_register (ViPipe, 0x3e00, 0x00);
    sc2210_write_register (ViPipe, 0x3e01, 0x45);
    sc2210_write_register (ViPipe, 0x3e02, 0x40);
    // 0x3e03 default 0b
#ifdef ENABLE_GAIN_03_MODE
    sc2210_write_register (ViPipe, 0x3e03, 0x03);
#else
    sc2210_write_register (ViPipe, 0x3e03, 0x0b);
#endif
    sc2210_write_register (ViPipe, 0x3e06, 0x00);
    sc2210_write_register (ViPipe, 0x3e07, 0x80);
    sc2210_write_register (ViPipe, 0x3e08, 0x03);
    sc2210_write_register (ViPipe, 0x3e09, 0x40);
    sc2210_write_register (ViPipe, 0x3e14, 0x31);
    sc2210_write_register (ViPipe, 0x3e1b, 0x3a);
    sc2210_write_register (ViPipe, 0x3e26, 0x40);
    sc2210_write_register (ViPipe, 0x3f08, 0x08);
    sc2210_write_register (ViPipe, 0x4401, 0x1a);
    sc2210_write_register (ViPipe, 0x4407, 0xc0);
    sc2210_write_register (ViPipe, 0x4418, 0x34);
    sc2210_write_register (ViPipe, 0x4500, 0x18);
    sc2210_write_register (ViPipe, 0x4501, 0xb4);
    sc2210_write_register (ViPipe, 0x4509, 0x20);
    sc2210_write_register (ViPipe, 0x4603, 0x00);
    sc2210_write_register (ViPipe, 0x4800, 0x24);
    sc2210_write_register (ViPipe, 0x4837, 0x13);
    sc2210_write_register (ViPipe, 0x5000, 0x0e);
    sc2210_write_register (ViPipe, 0x550f, 0x20);
    sc2210_write_register (ViPipe, 0x36e9, 0x24);
    sc2210_write_register (ViPipe, 0x36f9, 0x14);
    sc2210_default_reg_init(ViPipe);
    sc2210_write_register (ViPipe, 0x0100, 0x01);

    printf("===sc2210 2M 50fps 12bit LINE Init OK!===\n");
    return;
}

void sc2210_linear_2M_60fps_init(VI_PIPE ViPipe)
{
#if 1
    sc2210_write_register (ViPipe, 0x0103, 0x01);
    sc2210_write_register (ViPipe, 0x0100, 0x00);
    sc2210_write_register (ViPipe, 0x36e9, 0x80);
    sc2210_write_register (ViPipe, 0x36f9, 0x80);
    sc2210_write_register (ViPipe, 0x3001, 0x07);
    sc2210_write_register (ViPipe, 0x3002, 0xc0);
    sc2210_write_register (ViPipe, 0x300a, 0x2c);
    sc2210_write_register (ViPipe, 0x300f, 0x00);
    sc2210_write_register (ViPipe, 0x3018, 0x33);
    sc2210_write_register (ViPipe, 0x3019, 0x0c);
    sc2210_write_register (ViPipe, 0x301f, 0x01);
    sc2210_write_register (ViPipe, 0x3031, 0x0c);
    sc2210_write_register (ViPipe, 0x3033, 0x20);
    sc2210_write_register (ViPipe, 0x3038, 0x22);
    sc2210_write_register (ViPipe, 0x3106, 0x81);
    sc2210_write_register (ViPipe, 0x3201, 0x04);
    sc2210_write_register (ViPipe, 0x3203, 0x04);
    sc2210_write_register (ViPipe, 0x3204, 0x07);
    sc2210_write_register (ViPipe, 0x3205, 0x8b);
    sc2210_write_register (ViPipe, 0x3206, 0x04);
    sc2210_write_register (ViPipe, 0x3207, 0x43);
    sc2210_write_register (ViPipe, 0x320c, 0x04);
    sc2210_write_register (ViPipe, 0x320d, 0x37);
    sc2210_write_register (ViPipe, 0x320e, 0x04);
    sc2210_write_register (ViPipe, 0x320f, 0x58);
    sc2210_write_register (ViPipe, 0x3211, 0x04);
    sc2210_write_register (ViPipe, 0x3213, 0x04);
    sc2210_write_register (ViPipe, 0x3231, 0x02);
    sc2210_write_register (ViPipe, 0x3253, 0x04);
    sc2210_write_register (ViPipe, 0x3301, 0x0a);
    sc2210_write_register (ViPipe, 0x3302, 0x10);
    sc2210_write_register (ViPipe, 0x3304, 0x58);
    sc2210_write_register (ViPipe, 0x3305, 0x00);
    sc2210_write_register (ViPipe, 0x3306, 0xb0);
    sc2210_write_register (ViPipe, 0x3308, 0x20);
    sc2210_write_register (ViPipe, 0x3309, 0x98);
    sc2210_write_register (ViPipe, 0x330a, 0x01);
    sc2210_write_register (ViPipe, 0x330b, 0x68);
    sc2210_write_register (ViPipe, 0x330e, 0x48);
    sc2210_write_register (ViPipe, 0x3314, 0x92);
    sc2210_write_register (ViPipe, 0x331e, 0x49);
    sc2210_write_register (ViPipe, 0x3000, 0xc0);
    sc2210_write_register (ViPipe, 0x331f, 0x89);
    sc2210_write_register (ViPipe, 0x334c, 0x10);
    sc2210_write_register (ViPipe, 0x335d, 0x60);
    sc2210_write_register (ViPipe, 0x335e, 0x02);
    sc2210_write_register (ViPipe, 0x335f, 0x06);
    sc2210_write_register (ViPipe, 0x3364, 0x16);
    sc2210_write_register (ViPipe, 0x3366, 0x92);
    sc2210_write_register (ViPipe, 0x3367, 0x10);
    sc2210_write_register (ViPipe, 0x3368, 0x04);
    sc2210_write_register (ViPipe, 0x3369, 0x00);
    sc2210_write_register (ViPipe, 0x336a, 0x00);
    sc2210_write_register (ViPipe, 0x336b, 0x00);
    sc2210_write_register (ViPipe, 0x336d, 0x03);
    sc2210_write_register (ViPipe, 0x337c, 0x08);
    sc2210_write_register (ViPipe, 0x337d, 0x0e);
    sc2210_write_register (ViPipe, 0x337f, 0x33);
    sc2210_write_register (ViPipe, 0x3390, 0x10);
    sc2210_write_register (ViPipe, 0x3391, 0x30);
    sc2210_write_register (ViPipe, 0x3392, 0x40);
    sc2210_write_register (ViPipe, 0x3393, 0x0a);
    sc2210_write_register (ViPipe, 0x3394, 0x0a);
    sc2210_write_register (ViPipe, 0x3395, 0x0a);
    sc2210_write_register (ViPipe, 0x3396, 0x08);
    sc2210_write_register (ViPipe, 0x3397, 0x30);
    sc2210_write_register (ViPipe, 0x3398, 0x3f);
    sc2210_write_register (ViPipe, 0x3399, 0x30);
    sc2210_write_register (ViPipe, 0x339a, 0x30);
    sc2210_write_register (ViPipe, 0x339b, 0x30);
    sc2210_write_register (ViPipe, 0x339c, 0x30);
    sc2210_write_register (ViPipe, 0x33a2, 0x0a);
    sc2210_write_register (ViPipe, 0x33b9, 0x0e);
    sc2210_write_register (ViPipe, 0x33e1, 0x08);
    sc2210_write_register (ViPipe, 0x33e2, 0x18);
    sc2210_write_register (ViPipe, 0x33e3, 0x18);
    sc2210_write_register (ViPipe, 0x33e4, 0x18);
    sc2210_write_register (ViPipe, 0x33e5, 0x10);
    sc2210_write_register (ViPipe, 0x33e6, 0x06);
    sc2210_write_register (ViPipe, 0x33e7, 0x02);
    sc2210_write_register (ViPipe, 0x33e8, 0x18);
    sc2210_write_register (ViPipe, 0x33e9, 0x10);
    sc2210_write_register (ViPipe, 0x33ea, 0x0c);
    sc2210_write_register (ViPipe, 0x33eb, 0x10);
    sc2210_write_register (ViPipe, 0x33ec, 0x04);
    sc2210_write_register (ViPipe, 0x33ed, 0x02);
    sc2210_write_register (ViPipe, 0x33ee, 0xa0);
    sc2210_write_register (ViPipe, 0x33ef, 0x08);
    sc2210_write_register (ViPipe, 0x33f4, 0x18);
    sc2210_write_register (ViPipe, 0x33f5, 0x10);
    sc2210_write_register (ViPipe, 0x33f6, 0x0c);
    sc2210_write_register (ViPipe, 0x33f7, 0x10);
    sc2210_write_register (ViPipe, 0x33f8, 0x06);
    sc2210_write_register (ViPipe, 0x33f9, 0x02);
    sc2210_write_register (ViPipe, 0x33fa, 0x18);
    sc2210_write_register (ViPipe, 0x33fb, 0x10);
    sc2210_write_register (ViPipe, 0x33fc, 0x0c);
    sc2210_write_register (ViPipe, 0x33fd, 0x10);
    sc2210_write_register (ViPipe, 0x33fe, 0x04);
    sc2210_write_register (ViPipe, 0x33ff, 0x02);
    sc2210_write_register (ViPipe, 0x360f, 0x01);
    sc2210_write_register (ViPipe, 0x3622, 0xf7);
    sc2210_write_register (ViPipe, 0x3625, 0x0a);
    sc2210_write_register (ViPipe, 0x3627, 0x02);
    sc2210_write_register (ViPipe, 0x3630, 0xa2);
    sc2210_write_register (ViPipe, 0x3631, 0x00);
    sc2210_write_register (ViPipe, 0x3632, 0xd8);
    sc2210_write_register (ViPipe, 0x3633, 0x43);
    sc2210_write_register (ViPipe, 0x3635, 0x20);
    sc2210_write_register (ViPipe, 0x3638, 0x24);
    sc2210_write_register (ViPipe, 0x363a, 0x80);
    sc2210_write_register (ViPipe, 0x363b, 0x02);
    sc2210_write_register (ViPipe, 0x363e, 0x22);
    sc2210_write_register (ViPipe, 0x3670, 0x48);
    sc2210_write_register (ViPipe, 0x3671, 0xf7);
    sc2210_write_register (ViPipe, 0x3672, 0xf7);
    sc2210_write_register (ViPipe, 0x3673, 0x07);
    sc2210_write_register (ViPipe, 0x367a, 0x40);
    sc2210_write_register (ViPipe, 0x367b, 0x7f);
    sc2210_write_register (ViPipe, 0x3690, 0x42);
    sc2210_write_register (ViPipe, 0x3691, 0x43);
    sc2210_write_register (ViPipe, 0x3692, 0x54);
    sc2210_write_register (ViPipe, 0x369c, 0x40);
    sc2210_write_register (ViPipe, 0x369d, 0x7f);
    sc2210_write_register (ViPipe, 0x36b5, 0x40);
    sc2210_write_register (ViPipe, 0x36b6, 0x7f);
    sc2210_write_register (ViPipe, 0x36c0, 0x80);
    sc2210_write_register (ViPipe, 0x36c1, 0x9f);
    sc2210_write_register (ViPipe, 0x36c2, 0x9f);
    sc2210_write_register (ViPipe, 0x36cc, 0x20);
    sc2210_write_register (ViPipe, 0x36cd, 0x20);
    sc2210_write_register (ViPipe, 0x36ce, 0x30);
    sc2210_write_register (ViPipe, 0x36d0, 0x20);
    sc2210_write_register (ViPipe, 0x36d1, 0x40);
    sc2210_write_register (ViPipe, 0x36d2, 0x7f);
    sc2210_write_register (ViPipe, 0x36ea, 0x38);
    sc2210_write_register (ViPipe, 0x36eb, 0x0e);
    sc2210_write_register (ViPipe, 0x36ec, 0x03);
    sc2210_write_register (ViPipe, 0x36ed, 0x14);
    sc2210_write_register (ViPipe, 0x36fa, 0x3a);
    sc2210_write_register (ViPipe, 0x36fb, 0x15);
    sc2210_write_register (ViPipe, 0x36fc, 0x01);
    sc2210_write_register (ViPipe, 0x36fd, 0x14);
    sc2210_write_register (ViPipe, 0x3905, 0xd8);
    sc2210_write_register (ViPipe, 0x3907, 0x01);
    sc2210_write_register (ViPipe, 0x3908, 0x11);
    sc2210_write_register (ViPipe, 0x391b, 0x83);
    sc2210_write_register (ViPipe, 0x391f, 0x00);
    sc2210_write_register (ViPipe, 0x3933, 0x28);
    sc2210_write_register (ViPipe, 0x3934, 0xa6);
    sc2210_write_register (ViPipe, 0x3940, 0x70);
    sc2210_write_register (ViPipe, 0x3942, 0x08);
    sc2210_write_register (ViPipe, 0x3943, 0xbc);
    sc2210_write_register (ViPipe, 0x3958, 0x02);
    sc2210_write_register (ViPipe, 0x3959, 0x04);
    sc2210_write_register (ViPipe, 0x3980, 0x61);
    sc2210_write_register (ViPipe, 0x3987, 0x0b);
    sc2210_write_register (ViPipe, 0x3990, 0x00);
    sc2210_write_register (ViPipe, 0x3991, 0x00);
    sc2210_write_register (ViPipe, 0x3992, 0x00);
    sc2210_write_register (ViPipe, 0x3993, 0x00);
    sc2210_write_register (ViPipe, 0x3994, 0x00);
    sc2210_write_register (ViPipe, 0x3995, 0x00);
    sc2210_write_register (ViPipe, 0x3996, 0x00);
    sc2210_write_register (ViPipe, 0x3997, 0x00);
    sc2210_write_register (ViPipe, 0x3998, 0x00);
    sc2210_write_register (ViPipe, 0x3999, 0x00);
    sc2210_write_register (ViPipe, 0x399a, 0x00);
    sc2210_write_register (ViPipe, 0x399b, 0x00);
    sc2210_write_register (ViPipe, 0x399c, 0x00);
    sc2210_write_register (ViPipe, 0x399d, 0x00);
    sc2210_write_register (ViPipe, 0x399e, 0x00);
    sc2210_write_register (ViPipe, 0x399f, 0x00);
    sc2210_write_register (ViPipe, 0x39a0, 0x00);
    sc2210_write_register (ViPipe, 0x39a1, 0x00);
    sc2210_write_register (ViPipe, 0x39a2, 0x03);
    sc2210_write_register (ViPipe, 0x39a3, 0x30);
    sc2210_write_register (ViPipe, 0x39a4, 0x03);
    sc2210_write_register (ViPipe, 0x39a5, 0x60);
    sc2210_write_register (ViPipe, 0x39a6, 0x03);
    sc2210_write_register (ViPipe, 0x39a7, 0xa0);
    sc2210_write_register (ViPipe, 0x39a8, 0x03);
    sc2210_write_register (ViPipe, 0x39a9, 0xb0);
    sc2210_write_register (ViPipe, 0x39aa, 0x00);
    sc2210_write_register (ViPipe, 0x39ab, 0x00);
    sc2210_write_register (ViPipe, 0x39ac, 0x00);
    sc2210_write_register (ViPipe, 0x39ad, 0x20);
    sc2210_write_register (ViPipe, 0x39ae, 0x00);
    sc2210_write_register (ViPipe, 0x39af, 0x40);
    sc2210_write_register (ViPipe, 0x39b0, 0x00);
    sc2210_write_register (ViPipe, 0x39b1, 0x60);
    sc2210_write_register (ViPipe, 0x39b2, 0x00);
    sc2210_write_register (ViPipe, 0x39b3, 0x00);
    sc2210_write_register (ViPipe, 0x39b4, 0x08);
    sc2210_write_register (ViPipe, 0x39b5, 0x14);
    sc2210_write_register (ViPipe, 0x39b6, 0x20);
    sc2210_write_register (ViPipe, 0x39b7, 0x38);
    sc2210_write_register (ViPipe, 0x39b8, 0x38);
    sc2210_write_register (ViPipe, 0x39b9, 0x20);
    sc2210_write_register (ViPipe, 0x39ba, 0x14);
    sc2210_write_register (ViPipe, 0x39bb, 0x08);
    sc2210_write_register (ViPipe, 0x39bc, 0x08);
    sc2210_write_register (ViPipe, 0x39bd, 0x10);
    sc2210_write_register (ViPipe, 0x39be, 0x20);
    sc2210_write_register (ViPipe, 0x39bf, 0x30);
    sc2210_write_register (ViPipe, 0x39c0, 0x30);
    sc2210_write_register (ViPipe, 0x39c1, 0x20);
    sc2210_write_register (ViPipe, 0x39c2, 0x10);
    sc2210_write_register (ViPipe, 0x39c3, 0x08);
    sc2210_write_register (ViPipe, 0x39c4, 0x00);
    sc2210_write_register (ViPipe, 0x39c5, 0x80);
    sc2210_write_register (ViPipe, 0x39c6, 0x00);
    sc2210_write_register (ViPipe, 0x39c7, 0x80);
    sc2210_write_register (ViPipe, 0x39c8, 0x00);
    sc2210_write_register (ViPipe, 0x39c9, 0x00);
    sc2210_write_register (ViPipe, 0x39ca, 0x80);
    sc2210_write_register (ViPipe, 0x39cb, 0x00);
    sc2210_write_register (ViPipe, 0x39cc, 0x00);
    sc2210_write_register (ViPipe, 0x39cd, 0x00);
    sc2210_write_register (ViPipe, 0x39ce, 0x00);
    sc2210_write_register (ViPipe, 0x39cf, 0x00);
    sc2210_write_register (ViPipe, 0x39d0, 0x00);
    sc2210_write_register (ViPipe, 0x39d1, 0x00);
    sc2210_write_register (ViPipe, 0x39e2, 0x05);
    sc2210_write_register (ViPipe, 0x39e3, 0xeb);
    sc2210_write_register (ViPipe, 0x39e4, 0x07);
    sc2210_write_register (ViPipe, 0x39e5, 0xb6);
    sc2210_write_register (ViPipe, 0x39e6, 0x00);
    sc2210_write_register (ViPipe, 0x39e7, 0x3a);
    sc2210_write_register (ViPipe, 0x39e8, 0x3f);
    sc2210_write_register (ViPipe, 0x39e9, 0xb7);
    sc2210_write_register (ViPipe, 0x39ea, 0x02);
    sc2210_write_register (ViPipe, 0x39eb, 0x4f);
    sc2210_write_register (ViPipe, 0x39ec, 0x08);
    sc2210_write_register (ViPipe, 0x39ed, 0x00);
    sc2210_write_register (ViPipe, 0x3e00, 0x00);
    sc2210_write_register (ViPipe, 0x3e01, 0x45);
    sc2210_write_register (ViPipe, 0x3e02, 0x40);
    sc2210_write_register (ViPipe, 0x3e03, 0x0b);
    sc2210_write_register (ViPipe, 0x3e06, 0x00);
    sc2210_write_register (ViPipe, 0x3e07, 0x80);
    sc2210_write_register (ViPipe, 0x3e08, 0x03);
    sc2210_write_register (ViPipe, 0x3e09, 0x40);
    sc2210_write_register (ViPipe, 0x3e14, 0x31);
    sc2210_write_register (ViPipe, 0x3e1b, 0x3a);
    sc2210_write_register (ViPipe, 0x3e26, 0x40);
    sc2210_write_register (ViPipe, 0x3f08, 0x08);
    sc2210_write_register (ViPipe, 0x4401, 0x1a);
    sc2210_write_register (ViPipe, 0x4407, 0xc0);
    sc2210_write_register (ViPipe, 0x4418, 0x34);
    sc2210_write_register (ViPipe, 0x4500, 0x18);
    sc2210_write_register (ViPipe, 0x4501, 0xb4);
    sc2210_write_register (ViPipe, 0x4509, 0x20);
    sc2210_write_register (ViPipe, 0x4603, 0x00);
    sc2210_write_register (ViPipe, 0x4800, 0x24);
    sc2210_write_register (ViPipe, 0x4837, 0x13);
    sc2210_write_register (ViPipe, 0x5000, 0x0e);
    sc2210_write_register (ViPipe, 0x550f, 0x20);
    sc2210_write_register (ViPipe, 0x36e9, 0x24);
    sc2210_write_register (ViPipe, 0x36f9, 0x14);
    sc2210_write_register (ViPipe, 0x0100, 0x01);
#else
    sc2210_write_register (ViPipe, 0x0103, 0x01);
    sc2210_write_register (ViPipe, 0x0100, 0x00);
    sc2210_write_register (ViPipe, 0x36e9, 0x80);
    sc2210_write_register (ViPipe, 0x36f9, 0x80);
    sc2210_write_register (ViPipe, 0x3001, 0x07);
    sc2210_write_register (ViPipe, 0x3002, 0xc0);
    sc2210_write_register (ViPipe, 0x300a, 0x2c);
    sc2210_write_register (ViPipe, 0x300f, 0x00);
    sc2210_write_register (ViPipe, 0x3018, 0x33);
    sc2210_write_register (ViPipe, 0x3019, 0x0c);
    sc2210_write_register (ViPipe, 0x301f, 0x99);
    sc2210_write_register (ViPipe, 0x3031, 0x0c);
    sc2210_write_register (ViPipe, 0x3033, 0x20);
    sc2210_write_register (ViPipe, 0x3038, 0x22);
    sc2210_write_register (ViPipe, 0x3106, 0x01);
    sc2210_write_register (ViPipe, 0x3200, 0x00);
    sc2210_write_register (ViPipe, 0x3201, 0x00);
    sc2210_write_register (ViPipe, 0x3202, 0x00);
    sc2210_write_register (ViPipe, 0x3203, 0x00);
    sc2210_write_register (ViPipe, 0x3204, 0x07);
    sc2210_write_register (ViPipe, 0x3205, 0x8f);
    sc2210_write_register (ViPipe, 0x3206, 0x04);
    sc2210_write_register (ViPipe, 0x3207, 0x47);
    sc2210_write_register (ViPipe, 0x3208, 0x07);
    sc2210_write_register (ViPipe, 0x3209, 0x90);
    sc2210_write_register (ViPipe, 0x320a, 0x04);
    sc2210_write_register (ViPipe, 0x320b, 0x40);
    sc2210_write_register (ViPipe, 0x320c, 0x04);
    sc2210_write_register (ViPipe, 0x320d, 0x4c);
    sc2210_write_register (ViPipe, 0x320e, 0x04);
    sc2210_write_register (ViPipe, 0x320f, 0x65);
    sc2210_write_register (ViPipe, 0x3210, 0x00);
    sc2210_write_register (ViPipe, 0x3211, 0x00);
    sc2210_write_register (ViPipe, 0x3212, 0x00);
    sc2210_write_register (ViPipe, 0x3213, 0x04);
    sc2210_write_register (ViPipe, 0x3231, 0x02);
    sc2210_write_register (ViPipe, 0x3253, 0x04);
    sc2210_write_register (ViPipe, 0x3301, 0x0a);
    sc2210_write_register (ViPipe, 0x3302, 0x10);
    sc2210_write_register (ViPipe, 0x3304, 0x58);
    sc2210_write_register (ViPipe, 0x3305, 0x00);
    sc2210_write_register (ViPipe, 0x3000, 0xc0);
    sc2210_write_register (ViPipe, 0x3306, 0xb0);
    sc2210_write_register (ViPipe, 0x3308, 0x20);
    sc2210_write_register (ViPipe, 0x3309, 0x98);
    sc2210_write_register (ViPipe, 0x330a, 0x01);
    sc2210_write_register (ViPipe, 0x330b, 0x68);
    sc2210_write_register (ViPipe, 0x330e, 0x48);
    sc2210_write_register (ViPipe, 0x3314, 0x92);
    sc2210_write_register (ViPipe, 0x331e, 0x49);
    sc2210_write_register (ViPipe, 0x331f, 0x89);
    sc2210_write_register (ViPipe, 0x334c, 0x10);
    sc2210_write_register (ViPipe, 0x335d, 0x60);
    sc2210_write_register (ViPipe, 0x335e, 0x02);
    sc2210_write_register (ViPipe, 0x335f, 0x06);
    sc2210_write_register (ViPipe, 0x3364, 0x16);
    sc2210_write_register (ViPipe, 0x3366, 0x92);
    sc2210_write_register (ViPipe, 0x3367, 0x10);
    sc2210_write_register (ViPipe, 0x3368, 0x04);
    sc2210_write_register (ViPipe, 0x3369, 0x00);
    sc2210_write_register (ViPipe, 0x336a, 0x00);
    sc2210_write_register (ViPipe, 0x336b, 0x00);
    sc2210_write_register (ViPipe, 0x336d, 0x03);
    sc2210_write_register (ViPipe, 0x337c, 0x08);
    sc2210_write_register (ViPipe, 0x337d, 0x0e);
    sc2210_write_register (ViPipe, 0x337f, 0x33);
    sc2210_write_register (ViPipe, 0x3390, 0x10);
    sc2210_write_register (ViPipe, 0x3391, 0x30);
    sc2210_write_register (ViPipe, 0x3392, 0x40);
    sc2210_write_register (ViPipe, 0x3393, 0x0a);
    sc2210_write_register (ViPipe, 0x3394, 0x0a);
    sc2210_write_register (ViPipe, 0x3395, 0x0a);
    sc2210_write_register (ViPipe, 0x3396, 0x08);
    sc2210_write_register (ViPipe, 0x3397, 0x30);
    sc2210_write_register (ViPipe, 0x3398, 0x3f);
    sc2210_write_register (ViPipe, 0x3399, 0x30);
    sc2210_write_register (ViPipe, 0x339a, 0x30);
    sc2210_write_register (ViPipe, 0x339b, 0x30);
    sc2210_write_register (ViPipe, 0x339c, 0x30);
    sc2210_write_register (ViPipe, 0x33a2, 0x0a);
    sc2210_write_register (ViPipe, 0x33b9, 0x0e);
    sc2210_write_register (ViPipe, 0x33e1, 0x08);
    sc2210_write_register (ViPipe, 0x33e2, 0x18);
    sc2210_write_register (ViPipe, 0x33e3, 0x18);
    sc2210_write_register (ViPipe, 0x33e4, 0x18);
    sc2210_write_register (ViPipe, 0x33e5, 0x10);
    sc2210_write_register (ViPipe, 0x33e6, 0x06);
    sc2210_write_register (ViPipe, 0x33e7, 0x02);
    sc2210_write_register (ViPipe, 0x33e8, 0x18);
    sc2210_write_register (ViPipe, 0x33e9, 0x10);
    sc2210_write_register (ViPipe, 0x33ea, 0x0c);
    sc2210_write_register (ViPipe, 0x33eb, 0x10);
    sc2210_write_register (ViPipe, 0x33ec, 0x04);
    sc2210_write_register (ViPipe, 0x33ed, 0x02);
    sc2210_write_register (ViPipe, 0x33ee, 0xa0);
    sc2210_write_register (ViPipe, 0x33ef, 0x08);
    sc2210_write_register (ViPipe, 0x33f4, 0x18);
    sc2210_write_register (ViPipe, 0x33f5, 0x10);
    sc2210_write_register (ViPipe, 0x33f6, 0x0c);
    sc2210_write_register (ViPipe, 0x33f7, 0x10);
    sc2210_write_register (ViPipe, 0x33f8, 0x06);
    sc2210_write_register (ViPipe, 0x33f9, 0x02);
    sc2210_write_register (ViPipe, 0x33fa, 0x18);
    sc2210_write_register (ViPipe, 0x33fb, 0x10);
    sc2210_write_register (ViPipe, 0x33fc, 0x0c);
    sc2210_write_register (ViPipe, 0x33fd, 0x10);
    sc2210_write_register (ViPipe, 0x33fe, 0x04);
    sc2210_write_register (ViPipe, 0x33ff, 0x02);
    sc2210_write_register (ViPipe, 0x360f, 0x01);
    sc2210_write_register (ViPipe, 0x3622, 0xf7);
    sc2210_write_register (ViPipe, 0x3625, 0x0a);
    sc2210_write_register (ViPipe, 0x3627, 0x02);
    sc2210_write_register (ViPipe, 0x3630, 0xa2);
    sc2210_write_register (ViPipe, 0x3631, 0x00);
    sc2210_write_register (ViPipe, 0x3632, 0xd8);
    sc2210_write_register (ViPipe, 0x3633, 0x43);
    sc2210_write_register (ViPipe, 0x3635, 0x20);
    sc2210_write_register (ViPipe, 0x3638, 0x24);
    sc2210_write_register (ViPipe, 0x363a, 0x80);
    sc2210_write_register (ViPipe, 0x363b, 0x02);
    sc2210_write_register (ViPipe, 0x363e, 0x22);
    sc2210_write_register (ViPipe, 0x3670, 0x48);
    sc2210_write_register (ViPipe, 0x3671, 0xf7);
    sc2210_write_register (ViPipe, 0x3672, 0xf7);
    sc2210_write_register (ViPipe, 0x3673, 0x07);
    sc2210_write_register (ViPipe, 0x367a, 0x40);
    sc2210_write_register (ViPipe, 0x367b, 0x7f);
    sc2210_write_register (ViPipe, 0x3690, 0x42);
    sc2210_write_register (ViPipe, 0x3691, 0x43);
    sc2210_write_register (ViPipe, 0x3692, 0x54);
    sc2210_write_register (ViPipe, 0x369c, 0x40);
    sc2210_write_register (ViPipe, 0x369d, 0x7f);
    sc2210_write_register (ViPipe, 0x36b5, 0x40);
    sc2210_write_register (ViPipe, 0x36b6, 0x7f);
    sc2210_write_register (ViPipe, 0x36c0, 0x80);
    sc2210_write_register (ViPipe, 0x36c1, 0x9f);
    sc2210_write_register (ViPipe, 0x36c2, 0x9f);
    sc2210_write_register (ViPipe, 0x36cc, 0x20);
    sc2210_write_register (ViPipe, 0x36cd, 0x20);
    sc2210_write_register (ViPipe, 0x36ce, 0x30);
    sc2210_write_register (ViPipe, 0x36d0, 0x20);
    sc2210_write_register (ViPipe, 0x36d1, 0x40);
    sc2210_write_register (ViPipe, 0x36d2, 0x7f);
    sc2210_write_register (ViPipe, 0x36ea, 0x38);
    sc2210_write_register (ViPipe, 0x36eb, 0x0e);
    sc2210_write_register (ViPipe, 0x36ec, 0x03);
    sc2210_write_register (ViPipe, 0x36ed, 0x14);
    sc2210_write_register (ViPipe, 0x36fa, 0x2a);
    sc2210_write_register (ViPipe, 0x36fb, 0x15);
    sc2210_write_register (ViPipe, 0x36fc, 0x10);
    sc2210_write_register (ViPipe, 0x36fd, 0x34);
    sc2210_write_register (ViPipe, 0x3905, 0xd8);
    sc2210_write_register (ViPipe, 0x3907, 0x01);
    sc2210_write_register (ViPipe, 0x3908, 0x11);
    sc2210_write_register (ViPipe, 0x391b, 0x83);
    sc2210_write_register (ViPipe, 0x391f, 0x00);
    sc2210_write_register (ViPipe, 0x3933, 0x28);
    sc2210_write_register (ViPipe, 0x3934, 0xa6);
    sc2210_write_register (ViPipe, 0x3940, 0x70);
    sc2210_write_register (ViPipe, 0x3942, 0x08);
    sc2210_write_register (ViPipe, 0x3943, 0xbc);
    sc2210_write_register (ViPipe, 0x3958, 0x02);
    sc2210_write_register (ViPipe, 0x3959, 0x04);
    sc2210_write_register (ViPipe, 0x3980, 0x61);
    sc2210_write_register (ViPipe, 0x3987, 0x0b);
    sc2210_write_register (ViPipe, 0x3990, 0x00);
    sc2210_write_register (ViPipe, 0x3991, 0x00);
    sc2210_write_register (ViPipe, 0x3992, 0x00);
    sc2210_write_register (ViPipe, 0x3993, 0x00);
    sc2210_write_register (ViPipe, 0x3994, 0x00);
    sc2210_write_register (ViPipe, 0x3995, 0x00);
    sc2210_write_register (ViPipe, 0x3996, 0x00);
    sc2210_write_register (ViPipe, 0x3997, 0x00);
    sc2210_write_register (ViPipe, 0x3998, 0x00);
    sc2210_write_register (ViPipe, 0x3999, 0x00);
    sc2210_write_register (ViPipe, 0x399a, 0x00);
    sc2210_write_register (ViPipe, 0x399b, 0x00);
    sc2210_write_register (ViPipe, 0x399c, 0x00);
    sc2210_write_register (ViPipe, 0x399d, 0x00);
    sc2210_write_register (ViPipe, 0x399e, 0x00);
    sc2210_write_register (ViPipe, 0x399f, 0x00);
    sc2210_write_register (ViPipe, 0x39a0, 0x00);
    sc2210_write_register (ViPipe, 0x39a1, 0x00);
    sc2210_write_register (ViPipe, 0x39a2, 0x03);
    sc2210_write_register (ViPipe, 0x39a3, 0x30);
    sc2210_write_register (ViPipe, 0x39a4, 0x03);
    sc2210_write_register (ViPipe, 0x39a5, 0x60);
    sc2210_write_register (ViPipe, 0x39a6, 0x03);
    sc2210_write_register (ViPipe, 0x39a7, 0xa0);
    sc2210_write_register (ViPipe, 0x39a8, 0x03);
    sc2210_write_register (ViPipe, 0x39a9, 0xb0);
    sc2210_write_register (ViPipe, 0x39aa, 0x00);
    sc2210_write_register (ViPipe, 0x39ab, 0x00);
    sc2210_write_register (ViPipe, 0x39ac, 0x00);
    sc2210_write_register (ViPipe, 0x39ad, 0x20);
    sc2210_write_register (ViPipe, 0x39ae, 0x00);
    sc2210_write_register (ViPipe, 0x39af, 0x40);
    sc2210_write_register (ViPipe, 0x39b0, 0x00);
    sc2210_write_register (ViPipe, 0x39b1, 0x60);
    sc2210_write_register (ViPipe, 0x39b2, 0x00);
    sc2210_write_register (ViPipe, 0x39b3, 0x00);
    sc2210_write_register (ViPipe, 0x39b4, 0x08);
    sc2210_write_register (ViPipe, 0x39b5, 0x14);
    sc2210_write_register (ViPipe, 0x39b6, 0x20);
    sc2210_write_register (ViPipe, 0x39b7, 0x38);
    sc2210_write_register (ViPipe, 0x39b8, 0x38);
    sc2210_write_register (ViPipe, 0x39b9, 0x20);
    sc2210_write_register (ViPipe, 0x39ba, 0x14);
    sc2210_write_register (ViPipe, 0x39bb, 0x08);
    sc2210_write_register (ViPipe, 0x39bc, 0x08);
    sc2210_write_register (ViPipe, 0x39bd, 0x10);
    sc2210_write_register (ViPipe, 0x39be, 0x20);
    sc2210_write_register (ViPipe, 0x39bf, 0x30);
    sc2210_write_register (ViPipe, 0x39c0, 0x30);
    sc2210_write_register (ViPipe, 0x39c1, 0x20);
    sc2210_write_register (ViPipe, 0x39c2, 0x10);
    sc2210_write_register (ViPipe, 0x39c3, 0x08);
    sc2210_write_register (ViPipe, 0x39c4, 0x00);
    sc2210_write_register (ViPipe, 0x39c5, 0x80);
    sc2210_write_register (ViPipe, 0x39c6, 0x00);
    sc2210_write_register (ViPipe, 0x39c7, 0x80);
    sc2210_write_register (ViPipe, 0x39c8, 0x00);
    sc2210_write_register (ViPipe, 0x39c9, 0x00);
    sc2210_write_register (ViPipe, 0x39ca, 0x80);
    sc2210_write_register (ViPipe, 0x39cb, 0x00);
    sc2210_write_register (ViPipe, 0x39cc, 0x00);
    sc2210_write_register (ViPipe, 0x39cd, 0x00);
    sc2210_write_register (ViPipe, 0x39ce, 0x00);
    sc2210_write_register (ViPipe, 0x39cf, 0x00);
    sc2210_write_register (ViPipe, 0x39d0, 0x00);
    sc2210_write_register (ViPipe, 0x39d1, 0x00);
    sc2210_write_register (ViPipe, 0x39e2, 0x05);
    sc2210_write_register (ViPipe, 0x39e3, 0xeb);
    sc2210_write_register (ViPipe, 0x39e4, 0x07);
    sc2210_write_register (ViPipe, 0x39e5, 0xb6);
    sc2210_write_register (ViPipe, 0x39e6, 0x00);
    sc2210_write_register (ViPipe, 0x39e7, 0x3a);
    sc2210_write_register (ViPipe, 0x39e8, 0x3f);
    sc2210_write_register (ViPipe, 0x39e9, 0xb7);
    sc2210_write_register (ViPipe, 0x39ea, 0x02);
    sc2210_write_register (ViPipe, 0x39eb, 0x4f);
    sc2210_write_register (ViPipe, 0x39ec, 0x08);
    sc2210_write_register (ViPipe, 0x39ed, 0x00);
    sc2210_write_register (ViPipe, 0x3e00, 0x00);
    sc2210_write_register (ViPipe, 0x3e01, 0x46);
    sc2210_write_register (ViPipe, 0x3e02, 0x10);
    sc2210_write_register (ViPipe, 0x3e03, 0x0b);
    sc2210_write_register (ViPipe, 0x3e06, 0x00);
    sc2210_write_register (ViPipe, 0x3e07, 0x80);
    sc2210_write_register (ViPipe, 0x3e08, 0x03);
    sc2210_write_register (ViPipe, 0x3e09, 0x40);
    sc2210_write_register (ViPipe, 0x3e14, 0x31);
    sc2210_write_register (ViPipe, 0x3e1b, 0x3a);
    sc2210_write_register (ViPipe, 0x3e26, 0x40);
    sc2210_write_register (ViPipe, 0x3f08, 0x08);
    sc2210_write_register (ViPipe, 0x4401, 0x1a);
    sc2210_write_register (ViPipe, 0x4407, 0xc0);
    sc2210_write_register (ViPipe, 0x4418, 0x34);
    sc2210_write_register (ViPipe, 0x4500, 0x18);
    sc2210_write_register (ViPipe, 0x4501, 0xb4);
    sc2210_write_register (ViPipe, 0x4509, 0x20);
    sc2210_write_register (ViPipe, 0x4603, 0x00);
    sc2210_write_register (ViPipe, 0x4800, 0x24);
    sc2210_write_register (ViPipe, 0x4837, 0x13);
    sc2210_write_register (ViPipe, 0x5000, 0x0e);
    sc2210_write_register (ViPipe, 0x550f, 0x20);
    sc2210_write_register (ViPipe, 0x36e9, 0x24);
    sc2210_write_register (ViPipe, 0x36f9, 0x50);
    sc2210_write_register (ViPipe, 0x0100, 0x01);
#endif
    printf("===sc2210 2M 60fps 12bit LINE Init OK!===\n");
    return;
}
