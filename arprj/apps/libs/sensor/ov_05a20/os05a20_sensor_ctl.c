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

#define OS05A20_SENSOR_2688X1944_30FPS_LINEAR_12BIT_MODE (1)
#define OS05A20_SENSOR_2336X1752_30FPS_LINEAR_12BIT_MODE (2)

const unsigned char os05a20_i2c_addr     =    0x6c;        /* I2C Address of OS05A20 */
const unsigned int  os05a20_addr_byte    =    2;
const unsigned int  os05a20_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastOs05a20[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunOs05a20BusInfo[];
#define OS05A20_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastOs05a20[dev])

int os05a20_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunOs05a20BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

    ar_always("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (os05a20_i2c_addr >> 1));
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

int os05a20_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int os05a20_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int os05a20_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = os05a20_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = os05a20_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = os05a20_data_byte;

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

    if (os05a20_addr_byte == 2)
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

    if (os05a20_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, os05a20_addr_byte + os05a20_data_byte);
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

void os05a20_prog(VI_PIPE ViPipe, int *rom)
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
            os05a20_write_register(ViPipe, addr, data);
        }
    }
}

void os05a20_trigger(VI_PIPE ViPipe, AR_S32 s32Frames)
{
    os05a20_write_register (ViPipe, 0x0100, 0x01); /* standby */

    ar_always("===os05a20 trigger stream on!=====\n");
}


void os05a20_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void os05a20_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define OS05A20_FLIP_MIRRO (0x3820)

int os05a20_flip_off_mirro_off(VI_PIPE ViPipe)
{
    os05a20_write_register(ViPipe, OS05A20_FLIP_MIRRO, 0x00);
    return 0;
}

int os05a20_flip_on_mirro_off(VI_PIPE ViPipe)
{
    os05a20_write_register(ViPipe, OS05A20_FLIP_MIRRO, 0x02);
    return 0;
}

int os05a20_flip_off_mirro_on(VI_PIPE ViPipe)
{
    os05a20_write_register(ViPipe, OS05A20_FLIP_MIRRO, 0x01);
    return 0;
}

int os05a20_flip_on_mirro_on(VI_PIPE ViPipe)
{
    os05a20_write_register(ViPipe, OS05A20_FLIP_MIRRO, 0x03);
    return 0;
}

void os05a20_linear_2688x1944_30fps_10bit_init(VI_PIPE ViPipe);

void os05a20_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastOs05a20[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("update set reg[0x%x]: 0x%x \n", g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            os05a20_write_register(ViPipe, g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void os05a20_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastOs05a20[ViPipe]->bInit;
    enWDRMode   = g_pastOs05a20[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastOs05a20[ViPipe]->u8ImgMode;

    os05a20_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {

        }else if (WDR_MODE_3To1_LINE == enWDRMode)
        {

        }
        else
        {
            if (OS05A20_SENSOR_2688X1944_30FPS_LINEAR_12BIT_MODE == u8ImgMode)
            {
                os05a20_linear_2688x1944_30fps_10bit_init(ViPipe);

            }else if (OS05A20_SENSOR_2336X1752_30FPS_LINEAR_12BIT_MODE == u8ImgMode)
            {
                os05a20_linear_2336x1752_30fps_10bit_init(ViPipe);
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
            if (OS05A20_SENSOR_2688X1944_30FPS_LINEAR_12BIT_MODE == u8ImgMode)
                os05a20_linear_2688x1944_30fps_10bit_init(ViPipe);
            else if (OS05A20_SENSOR_2336X1752_30FPS_LINEAR_12BIT_MODE == u8ImgMode)
                os05a20_linear_2336x1752_30fps_10bit_init(ViPipe);
        }
    }

    // for (i = 0; i < g_pastOs05a20[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     os05a20_write_register(ViPipe, g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastOs05a20[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastOs05a20[ViPipe]->bInit = AR_TRUE;
    return ;
}

void os05a20_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    OS05A20_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    os05a20_write_register (ViPipe, 0x0100, 0x00); /* standby */
    //need clear astRegsInfo[1] for resume aec can't set new value
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
    return;
}

void os05a20_linear_2688x1944_30fps_10bit_init(VI_PIPE ViPipe)
{
    /* 2688x1944 30fps 4lane */
    os05a20_write_register(ViPipe, 0x0103, 0x01);
    usleep(10 * 1000);
    os05a20_write_register(ViPipe, 0x0303, 0x01);
    os05a20_write_register(ViPipe, 0x0305, 0x44);
    os05a20_write_register(ViPipe, 0x0306, 0x00);
    os05a20_write_register(ViPipe, 0x0308, 0x03);
    os05a20_write_register(ViPipe, 0x0309, 0x04);
    os05a20_write_register(ViPipe, 0x030c, 0x01);
    os05a20_write_register(ViPipe, 0x0322, 0x01);
    os05a20_write_register(ViPipe, 0x032a, 0x00);
    os05a20_write_register(ViPipe, 0x0325, 0x48);
    os05a20_write_register(ViPipe, 0x300f, 0x11);
    os05a20_write_register(ViPipe, 0x3010, 0x01);
    os05a20_write_register(ViPipe, 0x3012, 0x41);
    os05a20_write_register(ViPipe, 0x3016, 0xf0);
    os05a20_write_register(ViPipe, 0x301e, 0x98);
    os05a20_write_register(ViPipe, 0x3031, 0xa9);
    os05a20_write_register(ViPipe, 0x3103, 0x00);
    os05a20_write_register(ViPipe, 0x3104, 0x01);
    os05a20_write_register(ViPipe, 0x3106, 0x10);
    os05a20_write_register(ViPipe, 0x3501, 0x09);
    os05a20_write_register(ViPipe, 0x3502, 0x9c);
    os05a20_write_register(ViPipe, 0x3505, 0x83);
    os05a20_write_register(ViPipe, 0x3508, 0x02);
    os05a20_write_register(ViPipe, 0x3509, 0x00);
    os05a20_write_register(ViPipe, 0x350a, 0x04);
    os05a20_write_register(ViPipe, 0x350b, 0x00);
    os05a20_write_register(ViPipe, 0x350c, 0x00);
    os05a20_write_register(ViPipe, 0x350d, 0x80);
    os05a20_write_register(ViPipe, 0x350e, 0x04);
    os05a20_write_register(ViPipe, 0x350f, 0x00);
    os05a20_write_register(ViPipe, 0x3600, 0x00);
    os05a20_write_register(ViPipe, 0x3626, 0xff);
    os05a20_write_register(ViPipe, 0x3605, 0x50);
    os05a20_write_register(ViPipe, 0x3609, 0xb5);
    os05a20_write_register(ViPipe, 0x3610, 0x69);
    os05a20_write_register(ViPipe, 0x360c, 0x01);
    os05a20_write_register(ViPipe, 0x3628, 0xa4);
    os05a20_write_register(ViPipe, 0x362d, 0x10);
    os05a20_write_register(ViPipe, 0x3660, 0x43);
    os05a20_write_register(ViPipe, 0x3661, 0x06);
    os05a20_write_register(ViPipe, 0x3662, 0x00);
    os05a20_write_register(ViPipe, 0x3663, 0x28);
    os05a20_write_register(ViPipe, 0x3664, 0x0d);
    os05a20_write_register(ViPipe, 0x366a, 0x38);
    os05a20_write_register(ViPipe, 0x366b, 0xa0);
    os05a20_write_register(ViPipe, 0x366d, 0x00);
    os05a20_write_register(ViPipe, 0x366e, 0x00);
    os05a20_write_register(ViPipe, 0x3680, 0x00);
    os05a20_write_register(ViPipe, 0x3621, 0x81);
    os05a20_write_register(ViPipe, 0x3634, 0x31);
    os05a20_write_register(ViPipe, 0x3620, 0x20);
    os05a20_write_register(ViPipe, 0x362a, 0xd0);
    os05a20_write_register(ViPipe, 0x362e, 0xb0);
    os05a20_write_register(ViPipe, 0x3701, 0x0f);
    os05a20_write_register(ViPipe, 0x3737, 0x02);
    os05a20_write_register(ViPipe, 0x3741, 0x04);
    os05a20_write_register(ViPipe, 0x373c, 0x0f);
    os05a20_write_register(ViPipe, 0x373b, 0x02);
    os05a20_write_register(ViPipe, 0x3705, 0x00);
    os05a20_write_register(ViPipe, 0x3706, 0x50);
    os05a20_write_register(ViPipe, 0x370a, 0x00);
    os05a20_write_register(ViPipe, 0x370b, 0xe4);
    os05a20_write_register(ViPipe, 0x3709, 0x4a);
    os05a20_write_register(ViPipe, 0x3714, 0x21);
    os05a20_write_register(ViPipe, 0x371c, 0x00);
    os05a20_write_register(ViPipe, 0x371d, 0x08);
    os05a20_write_register(ViPipe, 0x375e, 0x0b);
    os05a20_write_register(ViPipe, 0x3776, 0x10);
    os05a20_write_register(ViPipe, 0x3781, 0x02);
    os05a20_write_register(ViPipe, 0x3782, 0x04);
    os05a20_write_register(ViPipe, 0x3783, 0x02);
    os05a20_write_register(ViPipe, 0x3784, 0x08);
    os05a20_write_register(ViPipe, 0x3785, 0x08);
    os05a20_write_register(ViPipe, 0x3788, 0x01);
    os05a20_write_register(ViPipe, 0x3789, 0x01);
    os05a20_write_register(ViPipe, 0x3797, 0x04);
    os05a20_write_register(ViPipe, 0x3800, 0x00);
    os05a20_write_register(ViPipe, 0x3801, 0x00);
    os05a20_write_register(ViPipe, 0x3802, 0x00);
    os05a20_write_register(ViPipe, 0x3803, 0x0c);
    os05a20_write_register(ViPipe, 0x3804, 0x0e);
    os05a20_write_register(ViPipe, 0x3805, 0xff);
    os05a20_write_register(ViPipe, 0x3806, 0x08);
    os05a20_write_register(ViPipe, 0x3807, 0x6f);
    os05a20_write_register(ViPipe, 0x3808, 0x0a);
    os05a20_write_register(ViPipe, 0x3809, 0x80);
    os05a20_write_register(ViPipe, 0x380a, 0x07);
    os05a20_write_register(ViPipe, 0x380b, 0x98);
    os05a20_write_register(ViPipe, 0x380c, 0x02);
    os05a20_write_register(ViPipe, 0x380d, 0xd0);
    os05a20_write_register(ViPipe, 0x380e, 0x09);
    os05a20_write_register(ViPipe, 0x380f, 0xc0);
    os05a20_write_register(ViPipe, 0x3813, 0x04);
    os05a20_write_register(ViPipe, 0x3814, 0x01);
    os05a20_write_register(ViPipe, 0x3815, 0x01);
    os05a20_write_register(ViPipe, 0x3816, 0x01);
    os05a20_write_register(ViPipe, 0x3817, 0x01);
    os05a20_write_register(ViPipe, 0x381c, 0x00);
    os05a20_write_register(ViPipe, 0x3820, 0x00);
    os05a20_write_register(ViPipe, 0x3821, 0x04);
    os05a20_write_register(ViPipe, 0x3832, 0x02);
    os05a20_write_register(ViPipe, 0x383c, 0x48);
    os05a20_write_register(ViPipe, 0x383d, 0xff);
    os05a20_write_register(ViPipe, 0x3843, 0x20);
    os05a20_write_register(ViPipe, 0x382d, 0x08);
    os05a20_write_register(ViPipe, 0x3d85, 0x0b);
    os05a20_write_register(ViPipe, 0x3d84, 0x40);
    os05a20_write_register(ViPipe, 0x3d8c, 0x63);
    os05a20_write_register(ViPipe, 0x3d8d, 0x00);
    os05a20_write_register(ViPipe, 0x4000, 0x78);
    os05a20_write_register(ViPipe, 0x4001, 0x2b);
    os05a20_write_register(ViPipe, 0x4005, 0x40);
    os05a20_write_register(ViPipe, 0x4028, 0x0f);
    os05a20_write_register(ViPipe, 0x400a, 0x01);
    os05a20_write_register(ViPipe, 0x4010, 0x12);
    os05a20_write_register(ViPipe, 0x4008, 0x02);
    os05a20_write_register(ViPipe, 0x4009, 0x0d);
    os05a20_write_register(ViPipe, 0x401a, 0x58);
    os05a20_write_register(ViPipe, 0x4050, 0x00);
    os05a20_write_register(ViPipe, 0x4051, 0x01);
    os05a20_write_register(ViPipe, 0x4052, 0x00);
    os05a20_write_register(ViPipe, 0x4053, 0x80);
    os05a20_write_register(ViPipe, 0x4054, 0x00);
    os05a20_write_register(ViPipe, 0x4055, 0x80);
    os05a20_write_register(ViPipe, 0x4056, 0x00);
    os05a20_write_register(ViPipe, 0x4057, 0x80);
    os05a20_write_register(ViPipe, 0x4058, 0x00);
    os05a20_write_register(ViPipe, 0x4059, 0x80);
    os05a20_write_register(ViPipe, 0x430b, 0xff);
    os05a20_write_register(ViPipe, 0x430c, 0xff);
    os05a20_write_register(ViPipe, 0x430d, 0x00);
    os05a20_write_register(ViPipe, 0x430e, 0x00);
    os05a20_write_register(ViPipe, 0x4501, 0x18);
    os05a20_write_register(ViPipe, 0x4502, 0x00);
    os05a20_write_register(ViPipe, 0x4643, 0x00);
    os05a20_write_register(ViPipe, 0x4640, 0x01);
    os05a20_write_register(ViPipe, 0x4641, 0x04);
    os05a20_write_register(ViPipe, 0x480e, 0x00);
    os05a20_write_register(ViPipe, 0x4813, 0x90);
    os05a20_write_register(ViPipe, 0x4815, 0x2b);
    os05a20_write_register(ViPipe, 0x4817, 0x04);
    os05a20_write_register(ViPipe, 0x4837, 0x1d);
    os05a20_write_register(ViPipe, 0x4860, 0x00);
    os05a20_write_register(ViPipe, 0x5000, 0xc9);
    os05a20_write_register(ViPipe, 0x5001, 0x41);
    os05a20_write_register(ViPipe, 0x5211, 0x03);
    os05a20_write_register(ViPipe, 0x5291, 0x03);
    os05a20_write_register(ViPipe, 0x5004, 0x40);
    os05a20_write_register(ViPipe, 0x5005, 0x00);
    os05a20_write_register(ViPipe, 0x5180, 0x00);
    os05a20_write_register(ViPipe, 0x5181, 0x10);
    os05a20_write_register(ViPipe, 0x5182, 0x0f);
    os05a20_write_register(ViPipe, 0x5183, 0xff);
    os05a20_write_register(ViPipe, 0x580b, 0x03);
    os05a20_write_register(ViPipe, 0x4700, 0x2b);
    os05a20_write_register(ViPipe, 0x4e00, 0x2b);
    os05a20_write_register(ViPipe, 0x0100, 0x01);

    ar_always("===new setting OS05A20 2688x1944@30fps 10bit init OK!===\n");
}

void os05a20_linear_2336x1752_30fps_10bit_init(VI_PIPE ViPipe)
{
    /* 2336x1752 30fps 4lane */
    os05a20_write_register(ViPipe, 0x0103, 0x01);
    usleep(10 * 1000);
    os05a20_write_register(ViPipe, 0x0303, 0x01);
    os05a20_write_register(ViPipe, 0x0305, 0x44);
    os05a20_write_register(ViPipe, 0x0306, 0x00);
    os05a20_write_register(ViPipe, 0x0308, 0x03);
    os05a20_write_register(ViPipe, 0x0309, 0x04);
    os05a20_write_register(ViPipe, 0x030c, 0x01);
    os05a20_write_register(ViPipe, 0x0322, 0x01);
    os05a20_write_register(ViPipe, 0x032a, 0x00);
    os05a20_write_register(ViPipe, 0x0325, 0x48);
    os05a20_write_register(ViPipe, 0x300f, 0x11);
    os05a20_write_register(ViPipe, 0x3010, 0x01);
    os05a20_write_register(ViPipe, 0x3012, 0x41);
    os05a20_write_register(ViPipe, 0x3016, 0xf0);
    os05a20_write_register(ViPipe, 0x301e, 0x98);
    os05a20_write_register(ViPipe, 0x3031, 0xa9);
    os05a20_write_register(ViPipe, 0x3103, 0x00);
    os05a20_write_register(ViPipe, 0x3104, 0x01);
    os05a20_write_register(ViPipe, 0x3106, 0x10);
    os05a20_write_register(ViPipe, 0x3501, 0x09);
    os05a20_write_register(ViPipe, 0x3502, 0x9c);
    os05a20_write_register(ViPipe, 0x3505, 0x83);
    os05a20_write_register(ViPipe, 0x3508, 0x02);
    os05a20_write_register(ViPipe, 0x3509, 0x00);
    os05a20_write_register(ViPipe, 0x350a, 0x04);
    os05a20_write_register(ViPipe, 0x350b, 0x00);
    os05a20_write_register(ViPipe, 0x350c, 0x00);
    os05a20_write_register(ViPipe, 0x350d, 0x80);
    os05a20_write_register(ViPipe, 0x350e, 0x04);
    os05a20_write_register(ViPipe, 0x350f, 0x00);
    os05a20_write_register(ViPipe, 0x3600, 0x00);
    os05a20_write_register(ViPipe, 0x3626, 0xff);
    os05a20_write_register(ViPipe, 0x3605, 0x50);
    os05a20_write_register(ViPipe, 0x3609, 0xb5);
    os05a20_write_register(ViPipe, 0x3610, 0x69);
    os05a20_write_register(ViPipe, 0x360c, 0x01);
    os05a20_write_register(ViPipe, 0x3628, 0xa4);
    os05a20_write_register(ViPipe, 0x362d, 0x10);
    os05a20_write_register(ViPipe, 0x3660, 0x43);
    os05a20_write_register(ViPipe, 0x3661, 0x06);
    os05a20_write_register(ViPipe, 0x3662, 0x00);
    os05a20_write_register(ViPipe, 0x3663, 0x28);
    os05a20_write_register(ViPipe, 0x3664, 0x0d);
    os05a20_write_register(ViPipe, 0x366a, 0x38);
    os05a20_write_register(ViPipe, 0x366b, 0xa0);
    os05a20_write_register(ViPipe, 0x366d, 0x00);
    os05a20_write_register(ViPipe, 0x366e, 0x00);
    os05a20_write_register(ViPipe, 0x3680, 0x00);
    os05a20_write_register(ViPipe, 0x3621, 0x81);
    os05a20_write_register(ViPipe, 0x3634, 0x31);
    os05a20_write_register(ViPipe, 0x3620, 0x20);
    os05a20_write_register(ViPipe, 0x362a, 0xd0);
    os05a20_write_register(ViPipe, 0x362e, 0xb0);
    os05a20_write_register(ViPipe, 0x3701, 0x0f);
    os05a20_write_register(ViPipe, 0x3737, 0x02);
    os05a20_write_register(ViPipe, 0x3741, 0x04);
    os05a20_write_register(ViPipe, 0x373c, 0x0f);
    os05a20_write_register(ViPipe, 0x373b, 0x02);
    os05a20_write_register(ViPipe, 0x3705, 0x00);
    os05a20_write_register(ViPipe, 0x3706, 0x50);
    os05a20_write_register(ViPipe, 0x370a, 0x00);
    os05a20_write_register(ViPipe, 0x370b, 0xe4);
    os05a20_write_register(ViPipe, 0x3709, 0x4a);
    os05a20_write_register(ViPipe, 0x3714, 0x21);
    os05a20_write_register(ViPipe, 0x371c, 0x00);
    os05a20_write_register(ViPipe, 0x371d, 0x08);
    os05a20_write_register(ViPipe, 0x375e, 0x0b);
    os05a20_write_register(ViPipe, 0x3776, 0x10);
    os05a20_write_register(ViPipe, 0x3781, 0x02);
    os05a20_write_register(ViPipe, 0x3782, 0x04);
    os05a20_write_register(ViPipe, 0x3783, 0x02);
    os05a20_write_register(ViPipe, 0x3784, 0x08);
    os05a20_write_register(ViPipe, 0x3785, 0x08);
    os05a20_write_register(ViPipe, 0x3788, 0x01);
    os05a20_write_register(ViPipe, 0x3789, 0x01);
    os05a20_write_register(ViPipe, 0x3797, 0x04);
    os05a20_write_register(ViPipe, 0x3800, 0x00);
    os05a20_write_register(ViPipe, 0x3801, 0x00);
    os05a20_write_register(ViPipe, 0x3802, 0x00);
    os05a20_write_register(ViPipe, 0x3803, 0x0c);
    os05a20_write_register(ViPipe, 0x3804, 0x0e);
    os05a20_write_register(ViPipe, 0x3805, 0xff);
    os05a20_write_register(ViPipe, 0x3806, 0x08);
    os05a20_write_register(ViPipe, 0x3807, 0x6f);
    os05a20_write_register(ViPipe, 0x3808, 0x09);
    os05a20_write_register(ViPipe, 0x3809, 0x20);
    os05a20_write_register(ViPipe, 0x380a, 0x06);
    os05a20_write_register(ViPipe, 0x380b, 0xd8);
    os05a20_write_register(ViPipe, 0x380c, 0x02);
    os05a20_write_register(ViPipe, 0x380d, 0xd0);
    os05a20_write_register(ViPipe, 0x380e, 0x09);
    os05a20_write_register(ViPipe, 0x380f, 0xc0);
    os05a20_write_register(ViPipe, 0x3810, 0x00);
    os05a20_write_register(ViPipe, 0x3811, 0xb0);
    os05a20_write_register(ViPipe, 0x3812, 0x00);
    os05a20_write_register(ViPipe, 0x3813, 0x64);
    os05a20_write_register(ViPipe, 0x3814, 0x01);
    os05a20_write_register(ViPipe, 0x3815, 0x01);
    os05a20_write_register(ViPipe, 0x3816, 0x01);
    os05a20_write_register(ViPipe, 0x3817, 0x01);
    os05a20_write_register(ViPipe, 0x381c, 0x00);
    os05a20_write_register(ViPipe, 0x3820, 0x00);
    os05a20_write_register(ViPipe, 0x3821, 0x04);
    os05a20_write_register(ViPipe, 0x3832, 0x02);
    os05a20_write_register(ViPipe, 0x383c, 0x48);
    os05a20_write_register(ViPipe, 0x383d, 0xff);
    os05a20_write_register(ViPipe, 0x3843, 0x20);
    os05a20_write_register(ViPipe, 0x382d, 0x08);
    os05a20_write_register(ViPipe, 0x3d85, 0x0b);
    os05a20_write_register(ViPipe, 0x3d84, 0x40);
    os05a20_write_register(ViPipe, 0x3d8c, 0x63);
    os05a20_write_register(ViPipe, 0x3d8d, 0x00);
    os05a20_write_register(ViPipe, 0x4000, 0x78);
    os05a20_write_register(ViPipe, 0x4001, 0x2b);
    os05a20_write_register(ViPipe, 0x4005, 0x40);
    os05a20_write_register(ViPipe, 0x4028, 0x0f);
    os05a20_write_register(ViPipe, 0x400a, 0x01);
    os05a20_write_register(ViPipe, 0x4010, 0x12);
    os05a20_write_register(ViPipe, 0x4008, 0x02);
    os05a20_write_register(ViPipe, 0x4009, 0x0d);
    os05a20_write_register(ViPipe, 0x401a, 0x58);
    os05a20_write_register(ViPipe, 0x4050, 0x00);
    os05a20_write_register(ViPipe, 0x4051, 0x01);
    os05a20_write_register(ViPipe, 0x4052, 0x00);
    os05a20_write_register(ViPipe, 0x4053, 0x80);
    os05a20_write_register(ViPipe, 0x4054, 0x00);
    os05a20_write_register(ViPipe, 0x4055, 0x80);
    os05a20_write_register(ViPipe, 0x4056, 0x00);
    os05a20_write_register(ViPipe, 0x4057, 0x80);
    os05a20_write_register(ViPipe, 0x4058, 0x00);
    os05a20_write_register(ViPipe, 0x4059, 0x80);
    os05a20_write_register(ViPipe, 0x430b, 0xff);
    os05a20_write_register(ViPipe, 0x430c, 0xff);
    os05a20_write_register(ViPipe, 0x430d, 0x00);
    os05a20_write_register(ViPipe, 0x430e, 0x00);
    os05a20_write_register(ViPipe, 0x4501, 0x18);
    os05a20_write_register(ViPipe, 0x4502, 0x00);
    os05a20_write_register(ViPipe, 0x4643, 0x00);
    os05a20_write_register(ViPipe, 0x4640, 0x01);
    os05a20_write_register(ViPipe, 0x4641, 0x04);
    os05a20_write_register(ViPipe, 0x480e, 0x00);
    os05a20_write_register(ViPipe, 0x4813, 0x90);
    os05a20_write_register(ViPipe, 0x4815, 0x2b);
    os05a20_write_register(ViPipe, 0x4817, 0x04);
    os05a20_write_register(ViPipe, 0x4837, 0x1d);
    os05a20_write_register(ViPipe, 0x4860, 0x00);
    os05a20_write_register(ViPipe, 0x5000, 0xc9);
    os05a20_write_register(ViPipe, 0x5001, 0x41);
    os05a20_write_register(ViPipe, 0x5211, 0x03);
    os05a20_write_register(ViPipe, 0x5291, 0x03);
    os05a20_write_register(ViPipe, 0x5004, 0x40);
    os05a20_write_register(ViPipe, 0x5005, 0x00);
    os05a20_write_register(ViPipe, 0x5180, 0x00);
    os05a20_write_register(ViPipe, 0x5181, 0x10);
    os05a20_write_register(ViPipe, 0x5182, 0x0f);
    os05a20_write_register(ViPipe, 0x5183, 0xff);
    os05a20_write_register(ViPipe, 0x580b, 0x03);
    os05a20_write_register(ViPipe, 0x4700, 0x2b);
    os05a20_write_register(ViPipe, 0x4e00, 0x2b);
    os05a20_write_register(ViPipe, 0x0100, 0x01);

    ar_always("===new setting OS05A20 2336x1752@30fps 10bit init OK!===\n");
}
