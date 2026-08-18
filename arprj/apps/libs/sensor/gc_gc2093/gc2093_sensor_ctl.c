
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"
#include "ar_i2c.h"


#define GC2093_FLIP_MIRROR              (0x0017)
#define SENSOR_1080P_30FPS_LINEAR_MODE  (0)
#define SENSOR_1080P_30FPS_2T1_WDR_MODE (1)

const unsigned char sensor_i2c_addr  = 0x6e;  /* I2C Address of GC2093: 0x6e or 0x7e */
const unsigned int  sensor_addr_byte = 2;
const unsigned int  sensor_data_byte = 1;

static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};
static AR_BOOL g_bStandby[ISP_MAX_PIPE_NUM] = {0};

extern ISP_SNS_STATE_S  *g_pastGc2093[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc2093BusInfo[];


void sensor_linear_1080p30_init(VI_PIPE ViPipe);
void sensor_wdr_1080p30_2to1_init(VI_PIPE ViPipe);


int sensor_i2c_init(VI_PIPE ViPipe)
{
    char acDevFile[16] = {0};
    AR_U8 u8DevNum = 0;
    int   ret = 0;

    if(g_fd[ViPipe] >= 0)
    {
        return AR_SUCCESS;
    }

    u8DevNum = g_aunGc2093BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
    ar_always("u8DevNum=%d, %s\n", u8DevNum, acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);
    if(g_fd[ViPipe] < 0)
    {
        ar_always("Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (sensor_i2c_addr >> 1));
    if(ret < 0)
    {
        ar_always("I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }

    return AR_SUCCESS;
}

int sensor_i2c_exit(VI_PIPE ViPipe)
{
    if(g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }

    return AR_FAILURE;
}

int sensor_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:
    return AR_SUCCESS;
}

int sensor_write_register(VI_PIPE ViPipe, int addr, int data)
{
    int idx = 0;
    int ret = 0;

    char buf[8];

    if(0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

    if(sensor_addr_byte == 2)
    {
        buf[idx] = (addr >> 8) & 0xff;
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    }
    else
    {
        buf[idx] = addr & 0xff;
        idx++;
    }

    if(sensor_data_byte == 2)
    {
        buf[idx] = (data >> 8) & 0xff;
        idx++;
        buf[idx] = data & 0xff;
        idx++;
    }
    else
    {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[ViPipe], buf, sensor_addr_byte + sensor_data_byte);
    if(ret < 0)
    {
        ar_always("I2C_WRITE error!\n");
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void sensor_prog(VI_PIPE ViPipe, int *rom)
{
    int i = 0;

    while(1)
    {
        int lookup = rom[i++];
        int addr = (lookup >> 16) & 0xFFFF;
        int data = lookup & 0xFFFF;
        if(addr == 0xFFFE)
        {
            delay_ms(data);
        }
        else if(addr == 0xFFFF)
        {
            return;
        }
        else
        {
            sensor_write_register(ViPipe, addr, data);
        }
    }
}

void sensor_standby(VI_PIPE ViPipe)
{
    //sensor_write_register(ViPipe, 0xf2, 0x01);
    //sensor_write_register(ViPipe, 0xfc, 0x8f);

    g_bStandby[ViPipe] = AR_TRUE;
    return;
}

void sensor_restart(VI_PIPE ViPipe)
{
    //sensor_write_register(ViPipe, 0xf2, 0x00);
    //sensor_write_register(ViPipe, 0xfc, 0x8e);

    g_bStandby[ViPipe] = AR_FALSE;
    return;
}

int sensor_flip_off_mirror_off(VI_PIPE ViPipe)
{
    sensor_write_register(ViPipe, GC2093_FLIP_MIRROR, 0x80);
    return AR_SUCCESS;
}

int sensor_flip_on_mirror_off(VI_PIPE ViPipe)
{
    sensor_write_register(ViPipe, GC2093_FLIP_MIRROR, 0x82);
    return AR_SUCCESS;
}

int sensor_flip_off_mirror_on(VI_PIPE ViPipe)
{
    sensor_write_register(ViPipe, GC2093_FLIP_MIRROR, 0x81);
    return AR_SUCCESS;
}

int sensor_flip_on_mirror_on(VI_PIPE ViPipe)
{
    sensor_write_register(ViPipe, GC2093_FLIP_MIRROR, 0x83);
    return AR_SUCCESS;
}

void sensor_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i = 0;

    for(i = 0; i < g_pastGc2093[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        if(g_pastGc2093[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            sensor_write_register(ViPipe,
                g_pastGc2093[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,
                g_pastGc2093[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }

    return;
}

void sensor_init(VI_PIPE ViPipe)
{
    AR_BOOL    bInit     = AR_FALSE;
    WDR_MODE_E enWDRMode = WDR_MODE_NONE;
    AR_U8      u8ImgMode = 0;

    bInit     = g_pastGc2093[ViPipe]->bInit;
    enWDRMode = g_pastGc2093[ViPipe]->enWDRMode;
    u8ImgMode = g_pastGc2093[ViPipe]->u8ImgMode;

    ar_always("GC2093: bInit[%d] enWDRMode[%d] u8ImgMode[%u]\n", bInit, enWDRMode, (AR_U32)u8ImgMode);

    if(AR_FALSE == bInit)
    {
        sensor_i2c_init(ViPipe);
    }

    if(WDR_MODE_2To1_LINE == enWDRMode)
    {
        if(SENSOR_1080P_30FPS_2T1_WDR_MODE == u8ImgMode)
        {
            sensor_wdr_1080p30_2to1_init(ViPipe);
        }
    }
    else
    {
        sensor_linear_1080p30_init(ViPipe);
    }

    g_pastGc2093[ViPipe]->bInit = AR_TRUE;

    return;
}

void sensor_exit(VI_PIPE ViPipe)
{
    sensor_i2c_exit(ViPipe);
    g_bStandby[ViPipe] = AR_FALSE;
	g_pastGc2093[ViPipe]->bInit = AR_FALSE;
    return;
}


/* 1080P@30fps LINEAR; Raw:10Bit; MCLK Input: 24MHz; MIPI CLK: 792Mbps/lane; CSI-2 2Lane; */
void sensor_linear_1080p30_init(VI_PIPE ViPipe)
{
    /* system */
    sensor_write_register(ViPipe, 0x03fe, 0xf0);
    sensor_write_register(ViPipe, 0x03fe, 0xf0);
    sensor_write_register(ViPipe, 0x03fe, 0xf0);
    sensor_write_register(ViPipe, 0x03fe, 0x00);
    sensor_write_register(ViPipe, 0x03f2, 0x00);
    sensor_write_register(ViPipe, 0x03f3, 0x00);
    sensor_write_register(ViPipe, 0x03f4, 0x36);
    sensor_write_register(ViPipe, 0x03f5, 0xc0);
    sensor_write_register(ViPipe, 0x03f6, 0x0B);
    sensor_write_register(ViPipe, 0x03f7, 0x01);
    sensor_write_register(ViPipe, 0x03f8, 0x63);
    sensor_write_register(ViPipe, 0x03f9, 0x40);
    sensor_write_register(ViPipe, 0x03fc, 0x8e);

    /* CISCTL & ANALOG */
    sensor_write_register(ViPipe, 0x0087, 0x18);
    sensor_write_register(ViPipe, 0x00ee, 0x30);
    sensor_write_register(ViPipe, 0x00d0, 0xbf);
    sensor_write_register(ViPipe, 0x01a0, 0x00);
    sensor_write_register(ViPipe, 0x01a4, 0x40);
    sensor_write_register(ViPipe, 0x01a5, 0x40);
    sensor_write_register(ViPipe, 0x01a6, 0x40);
    sensor_write_register(ViPipe, 0x01af, 0x09);
    sensor_write_register(ViPipe, 0x0001, 0x00);
    sensor_write_register(ViPipe, 0x0002, 0x02);
    sensor_write_register(ViPipe, 0x0003, 0x04);
    sensor_write_register(ViPipe, 0x0004, 0x02);

    sensor_write_register(ViPipe, 0x0005, 0x02);
    sensor_write_register(ViPipe, 0x0006, 0x94);

    sensor_write_register(ViPipe, 0x0007, 0x00);
    sensor_write_register(ViPipe, 0x0008, 0x11);
    sensor_write_register(ViPipe, 0x0009, 0x00);
    sensor_write_register(ViPipe, 0x000a, 0x02);
    sensor_write_register(ViPipe, 0x000b, 0x00);
    sensor_write_register(ViPipe, 0x000c, 0x04);
    sensor_write_register(ViPipe, 0x000d, 0x04);
    sensor_write_register(ViPipe, 0x000e, 0x40);
    sensor_write_register(ViPipe, 0x000f, 0x07);
    sensor_write_register(ViPipe, 0x0010, 0x8c);
    sensor_write_register(ViPipe, 0x0013, 0x15);
    sensor_write_register(ViPipe, 0x0019, 0x0c);

    sensor_write_register(ViPipe, 0x0041, 0x09);
    sensor_write_register(ViPipe, 0x0042, 0xc4);
    sensor_write_register(ViPipe, 0x0053, 0x60);
    sensor_write_register(ViPipe, 0x008d, 0x92);
    sensor_write_register(ViPipe, 0x0090, 0x00);
    sensor_write_register(ViPipe, 0x00c7, 0xe1);
    sensor_write_register(ViPipe, 0x001b, 0x73);
    sensor_write_register(ViPipe, 0x0028, 0x0d);
    sensor_write_register(ViPipe, 0x0029, 0x24);
    sensor_write_register(ViPipe, 0x002b, 0x04);
    sensor_write_register(ViPipe, 0x002e, 0x23);
    sensor_write_register(ViPipe, 0x0037, 0x03);
    sensor_write_register(ViPipe, 0x0043, 0x04);
    sensor_write_register(ViPipe, 0x0044, 0x28);
    sensor_write_register(ViPipe, 0x004a, 0x01);
    sensor_write_register(ViPipe, 0x004b, 0x20);
    sensor_write_register(ViPipe, 0x0055, 0x28);
    sensor_write_register(ViPipe, 0x0066, 0x3f);
    sensor_write_register(ViPipe, 0x0068, 0x3f);
    sensor_write_register(ViPipe, 0x006b, 0x44);
    sensor_write_register(ViPipe, 0x0077, 0x00);
    sensor_write_register(ViPipe, 0x0078, 0x20);
    sensor_write_register(ViPipe, 0x007c, 0xa1);
    sensor_write_register(ViPipe, 0x00ce, 0x7c);
    sensor_write_register(ViPipe, 0x00d3, 0xd4);
    sensor_write_register(ViPipe, 0x00e6, 0x50);

    /* gain */
    sensor_write_register(ViPipe, 0x00b6, 0xc0);
    sensor_write_register(ViPipe, 0x00b0, 0x68); //0x60

    /* isp */
    sensor_write_register(ViPipe, 0x0101, 0x0c);
    sensor_write_register(ViPipe, 0x0102, 0x89);
    sensor_write_register(ViPipe, 0x0104, 0x01);

    sensor_write_register(ViPipe, 0x010f, 0x00);
    sensor_write_register(ViPipe, 0x0158, 0x00);

    /* dark sun */
    sensor_write_register(ViPipe, 0x0123, 0x08);
    sensor_write_register(ViPipe, 0x0123, 0x00);
    sensor_write_register(ViPipe, 0x0120, 0x01);
    sensor_write_register(ViPipe, 0x0121, 0x04);
    sensor_write_register(ViPipe, 0x0122, 0xd8);
    sensor_write_register(ViPipe, 0x0124, 0x03);
    sensor_write_register(ViPipe, 0x0125, 0xff);
    sensor_write_register(ViPipe, 0x001a, 0x8c);
    sensor_write_register(ViPipe, 0x00c6, 0xe0);

    /* blk */
    sensor_write_register(ViPipe, 0x0026, 0x30);
    sensor_write_register(ViPipe, 0x0142, 0x00);
    sensor_write_register(ViPipe, 0x0149, 0x1e);
    sensor_write_register(ViPipe, 0x014a, 0x0f);
    sensor_write_register(ViPipe, 0x014b, 0x00);
    sensor_write_register(ViPipe, 0x0155, 0x07);
    sensor_write_register(ViPipe, 0x0414, 0x78);
    sensor_write_register(ViPipe, 0x0415, 0x78);
    sensor_write_register(ViPipe, 0x0416, 0x78);
    sensor_write_register(ViPipe, 0x0417, 0x78);
    sensor_write_register(ViPipe, 0x0454, 0x78);
    sensor_write_register(ViPipe, 0x0455, 0x78);
    sensor_write_register(ViPipe, 0x0456, 0x78);
    sensor_write_register(ViPipe, 0x0457, 0x78);
    sensor_write_register(ViPipe, 0x04e0, 0x18);

    /* window */
    sensor_write_register(ViPipe, 0x0192, 0x02);
    sensor_write_register(ViPipe, 0x0194, 0x03);
    sensor_write_register(ViPipe, 0x0195, 0x04);
    sensor_write_register(ViPipe, 0x0196, 0x38);
    sensor_write_register(ViPipe, 0x0197, 0x07);
    sensor_write_register(ViPipe, 0x0198, 0x80);

    /* DVP & MIPI */
    sensor_write_register(ViPipe, 0x019a, 0x06);
    sensor_write_register(ViPipe, 0x007b, 0x2a);
    sensor_write_register(ViPipe, 0x0023, 0x2d);
    sensor_write_register(ViPipe, 0x0201, 0x27);
    sensor_write_register(ViPipe, 0x0202, 0x56);
    sensor_write_register(ViPipe, 0x0203, 0xb6);
    sensor_write_register(ViPipe, 0x0212, 0x80);
    sensor_write_register(ViPipe, 0x0213, 0x07);
    sensor_write_register(ViPipe, 0x0215, 0x10);
    sensor_write_register(ViPipe, 0x003e, 0x91);

    ar_always("===============================================================\n");
    ar_always("== Galaxycore gc2093 sensor 1080P30fps linear init success!  ==\n");
    ar_always("===============================================================\n");
    return;
}


/* 1080P@30fps Line-based WDR; Raw:10Bit; MCLK Input: 24MHz; MIPI CLK: 792Mbps/lane; CSI-2 2Lane; */
void sensor_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
    /* system */
    sensor_write_register(ViPipe, 0x03fe, 0xf0);
    sensor_write_register(ViPipe, 0x03fe, 0xf0);
    sensor_write_register(ViPipe, 0x03fe, 0xf0);
    sensor_write_register(ViPipe, 0x03fe, 0x00);
    sensor_write_register(ViPipe, 0x03f2, 0x00);
    sensor_write_register(ViPipe, 0x03f3, 0x00);
    sensor_write_register(ViPipe, 0x03f4, 0x36);
    sensor_write_register(ViPipe, 0x03f5, 0xc0);
    sensor_write_register(ViPipe, 0x03f6, 0x0B);
    sensor_write_register(ViPipe, 0x03f7, 0x01);
    sensor_write_register(ViPipe, 0x03f8, 0x63);
    sensor_write_register(ViPipe, 0x03f9, 0x40);
    sensor_write_register(ViPipe, 0x03fc, 0x8e);

    /* CISCTL & ANALOG */
    sensor_write_register(ViPipe, 0x0087, 0x18);
    sensor_write_register(ViPipe, 0x00ee, 0x30);
    sensor_write_register(ViPipe, 0x00d0, 0xbf);
    sensor_write_register(ViPipe, 0x01a0, 0x00);
    sensor_write_register(ViPipe, 0x01a4, 0x40);
    sensor_write_register(ViPipe, 0x01a5, 0x40);
    sensor_write_register(ViPipe, 0x01a6, 0x40);
    sensor_write_register(ViPipe, 0x01af, 0x09);
    sensor_write_register(ViPipe, 0x0001, 0x00);
    sensor_write_register(ViPipe, 0x0002, 0x02);
    sensor_write_register(ViPipe, 0x0003, 0x04);
    sensor_write_register(ViPipe, 0x0004, 0x02);
    sensor_write_register(ViPipe, 0x0005, 0x02);
    sensor_write_register(ViPipe, 0x0006, 0x94);
    sensor_write_register(ViPipe, 0x0007, 0x00);
    sensor_write_register(ViPipe, 0x0008, 0x11);
    sensor_write_register(ViPipe, 0x0009, 0x00);
    sensor_write_register(ViPipe, 0x000a, 0x02);
    sensor_write_register(ViPipe, 0x000b, 0x00);
    sensor_write_register(ViPipe, 0x000c, 0x04);
    sensor_write_register(ViPipe, 0x000d, 0x04);
    sensor_write_register(ViPipe, 0x000e, 0x40);
    sensor_write_register(ViPipe, 0x000f, 0x07);
    sensor_write_register(ViPipe, 0x0010, 0x8c);
    sensor_write_register(ViPipe, 0x0013, 0x15);
    sensor_write_register(ViPipe, 0x0019, 0x0c);
    sensor_write_register(ViPipe, 0x0041, 0x04);
    sensor_write_register(ViPipe, 0x0042, 0xE2);
    sensor_write_register(ViPipe, 0x0053, 0x60);
    sensor_write_register(ViPipe, 0x008d, 0x92);
    sensor_write_register(ViPipe, 0x0090, 0x00);
    sensor_write_register(ViPipe, 0x00c7, 0xe1);
    sensor_write_register(ViPipe, 0x001b, 0x73);
    sensor_write_register(ViPipe, 0x0028, 0x0d);
    sensor_write_register(ViPipe, 0x0029, 0x24);
    sensor_write_register(ViPipe, 0x002b, 0x04);
    sensor_write_register(ViPipe, 0x002e, 0x23);
    sensor_write_register(ViPipe, 0x0037, 0x03);
    sensor_write_register(ViPipe, 0x0043, 0x04);
    sensor_write_register(ViPipe, 0x0044, 0x28);
    sensor_write_register(ViPipe, 0x004a, 0x01);
    sensor_write_register(ViPipe, 0x004b, 0x20);
    sensor_write_register(ViPipe, 0x0055, 0x28);
    sensor_write_register(ViPipe, 0x0066, 0x3f);
    sensor_write_register(ViPipe, 0x0068, 0x3f);
    sensor_write_register(ViPipe, 0x006b, 0x44);
    sensor_write_register(ViPipe, 0x0077, 0x00);
    sensor_write_register(ViPipe, 0x0078, 0x20);
    sensor_write_register(ViPipe, 0x007c, 0xa1);
    sensor_write_register(ViPipe, 0x00ce, 0x7c);
    sensor_write_register(ViPipe, 0x00d3, 0xd4);
    sensor_write_register(ViPipe, 0x00e6, 0x50);

    /* gain */
    sensor_write_register(ViPipe, 0x00b6, 0xc0);
    sensor_write_register(ViPipe, 0x00b0, 0x68);

    /* isp */
    sensor_write_register(ViPipe, 0x0101, 0x0c);
    sensor_write_register(ViPipe, 0x0102, 0x89);
    sensor_write_register(ViPipe, 0x0104, 0x01);
    sensor_write_register(ViPipe, 0x010e, 0x01);
    sensor_write_register(ViPipe, 0x010f, 0x00);
    sensor_write_register(ViPipe, 0x0158, 0x00);

    /* dark sun */
    sensor_write_register(ViPipe, 0x0123, 0x08);
    sensor_write_register(ViPipe, 0x0123, 0x00);
    sensor_write_register(ViPipe, 0x0120, 0x01);
    sensor_write_register(ViPipe, 0x0121, 0x04);
    sensor_write_register(ViPipe, 0x0122, 0xd8);
    sensor_write_register(ViPipe, 0x0124, 0x03);
    sensor_write_register(ViPipe, 0x0125, 0xff);
    sensor_write_register(ViPipe, 0x001a, 0x8c);
    sensor_write_register(ViPipe, 0x00c6, 0xe0);

    /* blk */
    sensor_write_register(ViPipe, 0x0026, 0x30);
    sensor_write_register(ViPipe, 0x0142, 0x00);
    sensor_write_register(ViPipe, 0x0149, 0x1e);
    sensor_write_register(ViPipe, 0x014a, 0x0f);
    sensor_write_register(ViPipe, 0x014b, 0x00);
    sensor_write_register(ViPipe, 0x0155, 0x07);
    sensor_write_register(ViPipe, 0x0414, 0x78);
    sensor_write_register(ViPipe, 0x0415, 0x78);
    sensor_write_register(ViPipe, 0x0416, 0x78);
    sensor_write_register(ViPipe, 0x0417, 0x78);
    sensor_write_register(ViPipe, 0x0454, 0x78);
    sensor_write_register(ViPipe, 0x0455, 0x78);
    sensor_write_register(ViPipe, 0x0456, 0x78);
    sensor_write_register(ViPipe, 0x0457, 0x78);
    sensor_write_register(ViPipe, 0x04e0, 0x18);

    /* window */
    sensor_write_register(ViPipe, 0x0192, 0x02);
    sensor_write_register(ViPipe, 0x0194, 0x03);
    sensor_write_register(ViPipe, 0x0195, 0x04);
    sensor_write_register(ViPipe, 0x0196, 0x38);
    sensor_write_register(ViPipe, 0x0197, 0x07);
    sensor_write_register(ViPipe, 0x0198, 0x82);    //0x80);

    /* DVP & MIPI */
    sensor_write_register(ViPipe, 0x019a, 0x06);
    sensor_write_register(ViPipe, 0x007b, 0x2a);
    sensor_write_register(ViPipe, 0x0023, 0x2d);
    sensor_write_register(ViPipe, 0x0201, 0x27);
    sensor_write_register(ViPipe, 0x0202, 0x56);
    sensor_write_register(ViPipe, 0x0203, 0xb6); //try 0xce or 0x8e
    sensor_write_register(ViPipe, 0x0212, 0x80);
    sensor_write_register(ViPipe, 0x0213, 0x07);
    sensor_write_register(ViPipe, 0x0215, 0x10);
    sensor_write_register(ViPipe, 0x003e, 0x91);

    /* HDR EN */
    sensor_write_register(ViPipe, 0x0027, 0x71);
    sensor_write_register(ViPipe, 0x0215, 0x92);
    sensor_write_register(ViPipe, 0x024d, 0x01);

    ar_always("============================================================\n");
    ar_always("== Galaxycore gc2093 sensor 1080P30fps wdr init success!  ==\n");
    ar_always("============================================================\n");
    return;
}

