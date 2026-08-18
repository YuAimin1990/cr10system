
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"
#include "ar_i2c.h"


#define SENSOR_1080P_30FPS_LINEAR_MODE  (0)

const unsigned char gc2053_sensor_i2c_addr  = 0x7e;
const unsigned int  gc2053_sensor_addr_byte = 1;
const unsigned int  gc2053_sensor_data_byte = 1;

static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};
static AR_BOOL g_bStandby[ISP_MAX_PIPE_NUM] = {0};

extern ISP_SNS_STATE_S  *g_pastGc2053[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc2053BusInfo[];


void gc2053_sensor_linear_1080p30_init(VI_PIPE ViPipe);


static int sensor_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
    AR_U8 u8DevNum = 0;
    int   ret = 0;

    if(g_fd[ViPipe] >= 0)
    {
        return AR_SUCCESS;
    }

    u8DevNum = g_aunGc2053BusInfo[ViPipe].s8I2cDev;
	
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
    printf("u8DevNum=%d, %s\n", u8DevNum, acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);
    if(g_fd[ViPipe] < 0)
    {
        printf("Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (gc2053_sensor_i2c_addr >> 1));
    if(ret < 0)
    {
        printf("I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }
	
    return AR_SUCCESS;
}

static int sensor_i2c_exit(VI_PIPE ViPipe)
{
    if(g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
	
    return AR_FAILURE;
}

int gc2053_sensor_read_register(VI_PIPE ViPipe, int addr)
{
    return AR_SUCCESS;
}

int gc2053_sensor_write_register(VI_PIPE ViPipe, int addr, int data)
{
    int idx = 0;
    int ret = 0;

    char buf[8];

    if(0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

    if(gc2053_sensor_addr_byte == 2)
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

    if(gc2053_sensor_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, gc2053_sensor_addr_byte + gc2053_sensor_data_byte);
    if(ret < 0)
    {
        printf("I2C_WRITE error!\n");
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
            gc2053_sensor_write_register(ViPipe, addr, data);
        }
    }
}

void gc2053_sensor_standby(VI_PIPE ViPipe)
{
    //gc2053_sensor_write_register(ViPipe, 0xf2, 0x01);
    //gc2053_sensor_write_register(ViPipe, 0xfc, 0x8f);
	//printf("gc2053_debug_123456 %s %d.\r\n", __func__, __LINE__);

    g_bStandby[ViPipe] = AR_TRUE;
    return;
}

void gc2053_sensor_restart(VI_PIPE ViPipe)
{
    //gc2053_sensor_write_register(ViPipe, 0xf2, 0x00);
    //gc2053_sensor_write_register(ViPipe, 0xfc, 0x8e);
	//printf("gc2053_debug_123456 %s %d.\r\n", __func__, __LINE__);

    g_bStandby[ViPipe] = AR_FALSE;
    return;
}

int gc2053_sensor_flip_off_mirror_off(VI_PIPE ViPipe)
{
    // gc2053_sensor_write_register(ViPipe, GC1054_FLIP_MIRROR, 0x00);printf("gc2053_debug %s %d.\r\n", __func__, __LINE__);
    return AR_SUCCESS;
}

int gc2053_sensor_flip_on_mirror_off(VI_PIPE ViPipe)
{
    // gc2053_sensor_write_register(ViPipe, GC1054_FLIP_MIRROR, 0x10);printf("gc2053_debug %s %d.\r\n", __func__, __LINE__);
    return AR_SUCCESS;
}

int gc2053_sensor_flip_off_mirror_on(VI_PIPE ViPipe)
{
    // gc2053_sensor_write_register(ViPipe, GC1054_FLIP_MIRROR, 0x01);printf("gc2053_debug %s %d.\r\n", __func__, __LINE__);
    return AR_SUCCESS;
}

int gc2053_sensor_flip_on_mirror_on(VI_PIPE ViPipe)
{
    // gc2053_sensor_write_register(ViPipe, GC1054_FLIP_MIRROR, 0x11);printf("gc2053_debug %s %d.\r\n", __func__, __LINE__);
    return AR_SUCCESS;
}

void gc2053_sensor_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i = 0;

    for(i = 0; i < g_pastGc2053[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        if(g_pastGc2053[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            gc2053_sensor_write_register(ViPipe,
                g_pastGc2053[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,
                g_pastGc2053[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
	
    return;
}

void gc2053_sensor_init(VI_PIPE ViPipe)
{
    AR_BOOL    bInit     = AR_FALSE;
    WDR_MODE_E enWDRMode = WDR_MODE_NONE;
    AR_U8      u8ImgMode = 0;

    bInit     = g_pastGc2053[ViPipe]->bInit;
    enWDRMode = g_pastGc2053[ViPipe]->enWDRMode;
    u8ImgMode = g_pastGc2053[ViPipe]->u8ImgMode;

    printf("GC2053: bInit[%d] enWDRMode[%d] u8ImgMode[%u]\n", bInit, enWDRMode, (AR_U32)u8ImgMode);

    if(AR_FALSE == bInit)
    {
        sensor_i2c_init(ViPipe);
    }
    if (u8ImgMode == SENSOR_1080P_30FPS_LINEAR_MODE)
    {
        gc2053_sensor_linear_1080p30_init(ViPipe);
    }
    else
    {
        printf("gc2053_debug %s: unsupport mode %d\r\n", __func__, u8ImgMode);
    }
    
    g_pastGc2053[ViPipe]->bInit = AR_TRUE;

    return;
}

void gc2053_sensor_exit(VI_PIPE ViPipe)
{

    sensor_i2c_exit(ViPipe);
    g_bStandby[ViPipe] = AR_FALSE;
    printf("gc2053_debug %s %d.\r\n", __func__, __LINE__);

    return;
}

/*
 * window size=1920*1080 mipi@2lane
 * mclk=27M mipi_clk=594Mbps
 * pixel_line_total=2200 line_frame_total=1125
 * row_time=29.62us frame_rate=30fps
 */
void gc2053_sensor_linear_1080p30_init(VI_PIPE ViPipe)
{    
    printf("====================================================================\n");
    printf("==  gc2053 sensor 1920X1080_30fps linear init start          !    ==\n");
    printf("====================================================================\n");
#if 0 
    /****system****/
    gc2053_sensor_write_register(ViPipe,0xfe,0x80);
    gc2053_sensor_write_register(ViPipe,0xfe,0x80);
    gc2053_sensor_write_register(ViPipe,0xfe,0x80);
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0xf2,0x00);
    gc2053_sensor_write_register(ViPipe,0xf3,0x00);
    gc2053_sensor_write_register(ViPipe,0xf4,0x36);
    gc2053_sensor_write_register(ViPipe,0xf5,0xc0);
    gc2053_sensor_write_register(ViPipe,0xf6,0x44);
    gc2053_sensor_write_register(ViPipe,0xf7,0x01);
    gc2053_sensor_write_register(ViPipe,0xf8,0x63);
    gc2053_sensor_write_register(ViPipe,0xf9,0x40);
    gc2053_sensor_write_register(ViPipe,0xfc,0x8e);
    /****CISCTL & ANALOG****/
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0x87,0x18);
    gc2053_sensor_write_register(ViPipe,0xee,0x30);
    gc2053_sensor_write_register(ViPipe,0xd0,0xb7);
    gc2053_sensor_write_register(ViPipe,0x03,0x04);
    gc2053_sensor_write_register(ViPipe,0x04,0x60);
    gc2053_sensor_write_register(ViPipe,0x05,0x04);
    gc2053_sensor_write_register(ViPipe,0x06,0x4c);
    gc2053_sensor_write_register(ViPipe,0x07,0x00);
    gc2053_sensor_write_register(ViPipe,0x08,0x11);
    gc2053_sensor_write_register(ViPipe,0x09,0x00);
    gc2053_sensor_write_register(ViPipe,0x0a,0x02);
    gc2053_sensor_write_register(ViPipe,0x0b,0x00);
    gc2053_sensor_write_register(ViPipe,0x0c,0x02);
    gc2053_sensor_write_register(ViPipe,0x0d,0x04);
    gc2053_sensor_write_register(ViPipe,0x0e,0x40);
    gc2053_sensor_write_register(ViPipe,0x12,0xe2);
    gc2053_sensor_write_register(ViPipe,0x13,0x16);
    gc2053_sensor_write_register(ViPipe,0x19,0x0a);
    gc2053_sensor_write_register(ViPipe,0x21,0x1c);
    gc2053_sensor_write_register(ViPipe,0x28,0x0a);
    gc2053_sensor_write_register(ViPipe,0x29,0x24);
    gc2053_sensor_write_register(ViPipe,0x2b,0x04);
    gc2053_sensor_write_register(ViPipe,0x32,0xf8);
    gc2053_sensor_write_register(ViPipe,0x37,0x03);
    gc2053_sensor_write_register(ViPipe,0x39,0x15);
    gc2053_sensor_write_register(ViPipe,0x43,0x07);
    gc2053_sensor_write_register(ViPipe,0x44,0x40);
    gc2053_sensor_write_register(ViPipe,0x46,0x0b);
    gc2053_sensor_write_register(ViPipe,0x4b,0x20);
    gc2053_sensor_write_register(ViPipe,0x4e,0x08);
    gc2053_sensor_write_register(ViPipe,0x55,0x20);
    gc2053_sensor_write_register(ViPipe,0x66,0x05);
    gc2053_sensor_write_register(ViPipe,0x67,0x05);
    gc2053_sensor_write_register(ViPipe,0x77,0x01);
    gc2053_sensor_write_register(ViPipe,0x78,0x00);
    gc2053_sensor_write_register(ViPipe,0x7c,0x93);
    gc2053_sensor_write_register(ViPipe,0x8c,0x12);
    gc2053_sensor_write_register(ViPipe,0x8d,0x92);
    gc2053_sensor_write_register(ViPipe,0x90,0x00);
    gc2053_sensor_write_register(ViPipe,0x41,0x04);
    gc2053_sensor_write_register(ViPipe,0x42,0x65);
    gc2053_sensor_write_register(ViPipe,0x9d,0x10);
    gc2053_sensor_write_register(ViPipe,0xce,0x7c);
    gc2053_sensor_write_register(ViPipe,0xd2,0x41);
    gc2053_sensor_write_register(ViPipe,0xd3,0xdc);
    gc2053_sensor_write_register(ViPipe,0xe6,0x50);
    /*gain*/
    gc2053_sensor_write_register(ViPipe,0xb6,0xc0);
    gc2053_sensor_write_register(ViPipe,0xb0,0x70);
    gc2053_sensor_write_register(ViPipe,0xb1,0x01);
    gc2053_sensor_write_register(ViPipe,0xb2,0x00);
    gc2053_sensor_write_register(ViPipe,0xb3,0x00);
    gc2053_sensor_write_register(ViPipe,0xb4,0x00);
    gc2053_sensor_write_register(ViPipe,0xb8,0x01);
    gc2053_sensor_write_register(ViPipe,0xb9,0x00);
    /*blk*/
    gc2053_sensor_write_register(ViPipe,0x26,0x30);
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x40,0x23);
    gc2053_sensor_write_register(ViPipe,0x55,0x07);
    gc2053_sensor_write_register(ViPipe,0x60,0x40);
    gc2053_sensor_write_register(ViPipe,0xfe,0x04);
    gc2053_sensor_write_register(ViPipe,0x14,0x78);
    gc2053_sensor_write_register(ViPipe,0x15,0x78);
    gc2053_sensor_write_register(ViPipe,0x16,0x78);
    gc2053_sensor_write_register(ViPipe,0x17,0x78);
    /*window*/
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x92,0x00);
    gc2053_sensor_write_register(ViPipe,0x94,0x03);

    gc2053_sensor_write_register(ViPipe,0x95,0x04);
    gc2053_sensor_write_register(ViPipe,0x96,0x38);//1080
    gc2053_sensor_write_register(ViPipe,0x97,0x07);
    gc2053_sensor_write_register(ViPipe,0x98,0x80);//1920
    /*ISP*/
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x01,0x05);
    gc2053_sensor_write_register(ViPipe,0x02,0x89);

    gc2053_sensor_write_register(ViPipe,0x04,0x01);
    gc2053_sensor_write_register(ViPipe,0x07,0xa6);
    gc2053_sensor_write_register(ViPipe,0x08,0xa9);
    gc2053_sensor_write_register(ViPipe,0x09,0xa8);
    gc2053_sensor_write_register(ViPipe,0x0a,0xa7);
    gc2053_sensor_write_register(ViPipe,0x0b,0xff);
    gc2053_sensor_write_register(ViPipe,0x0c,0xff);
    gc2053_sensor_write_register(ViPipe,0x0f,0x00);
    gc2053_sensor_write_register(ViPipe,0x50,0x1c);
    gc2053_sensor_write_register(ViPipe,0x89,0x03);
    gc2053_sensor_write_register(ViPipe,0xfe,0x04);
    gc2053_sensor_write_register(ViPipe,0x28,0x86);
    gc2053_sensor_write_register(ViPipe,0x29,0x86);
    gc2053_sensor_write_register(ViPipe,0x2a,0x86);
    gc2053_sensor_write_register(ViPipe,0x2b,0x68);
    gc2053_sensor_write_register(ViPipe,0x2c,0x68);
    gc2053_sensor_write_register(ViPipe,0x2d,0x68);
    gc2053_sensor_write_register(ViPipe,0x2e,0x68);
    gc2053_sensor_write_register(ViPipe,0x2f,0x68);
    gc2053_sensor_write_register(ViPipe,0x30,0x4f);
    gc2053_sensor_write_register(ViPipe,0x31,0x68);
    gc2053_sensor_write_register(ViPipe,0x32,0x67);
    gc2053_sensor_write_register(ViPipe,0x33,0x66);
    gc2053_sensor_write_register(ViPipe,0x34,0x66);
    gc2053_sensor_write_register(ViPipe,0x35,0x66);
    gc2053_sensor_write_register(ViPipe,0x36,0x66);
    gc2053_sensor_write_register(ViPipe,0x37,0x66);
    gc2053_sensor_write_register(ViPipe,0x38,0x62);
    gc2053_sensor_write_register(ViPipe,0x39,0x62);
    gc2053_sensor_write_register(ViPipe,0x3a,0x62);
    gc2053_sensor_write_register(ViPipe,0x3b,0x62);
    gc2053_sensor_write_register(ViPipe,0x3c,0x62);
    gc2053_sensor_write_register(ViPipe,0x3d,0x62);
    gc2053_sensor_write_register(ViPipe,0x3e,0x62);
    gc2053_sensor_write_register(ViPipe,0x3f,0x62);
    /****DVP & MIPI****/
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x9a,0x06);
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0x7b,0x2a);
    gc2053_sensor_write_register(ViPipe,0x23,0x2d);
    gc2053_sensor_write_register(ViPipe,0xfe,0x03);
    gc2053_sensor_write_register(ViPipe,0x01,0x27);
    gc2053_sensor_write_register(ViPipe,0x02,0x5f);
    gc2053_sensor_write_register(ViPipe,0x03,0xb6);
    gc2053_sensor_write_register(ViPipe,0x12,0x80);
    gc2053_sensor_write_register(ViPipe,0x13,0x07);
    gc2053_sensor_write_register(ViPipe,0x15,0x12);
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0x3e,0x91);
#else 
    //window_size=1920*1080 mipi@2lane
    //mclk=27mhz,mipi_clk=594Mbps
    //pixel_line_total=2200,line_frame_total=1125
    //row_time=29.629us,frame_rate=30fps
    /****system****/
    gc2053_sensor_write_register(ViPipe,0xfe,0x80);
    gc2053_sensor_write_register(ViPipe,0xfe,0x80);
    gc2053_sensor_write_register(ViPipe,0xfe,0x80);
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0xf2,0x00);//[1]I2C_open_ena [0]pwd_dn
    gc2053_sensor_write_register(ViPipe,0xf3,0x00);//0f//00[3]Sdata_pad_io [2:0]Ssync_pad_io 
    gc2053_sensor_write_register(ViPipe,0xf4,0x36);//[6:4]pll_ldo_set
    gc2053_sensor_write_register(ViPipe,0xf5,0xc0);//[7]soc_mclk_enable [6]pll_ldo_en [5:4]cp_clk_sel [3:0]cp_clk_div
    gc2053_sensor_write_register(ViPipe,0xf6,0x44);//[7:3]wpllclk_div [2:0]refmp_div
    gc2053_sensor_write_register(ViPipe,0xf7,0x01);//[7]refdiv2d5_en [6]refdiv1d5_en [5:4]scaler_mode [3]refmp_enb [1]div2en [0]pllmp_en
    gc2053_sensor_write_register(ViPipe,0xf8,0x2c);//[7:0]pllmp_div
    gc2053_sensor_write_register(ViPipe,0xf9,0x42);//[7:3]rpllclk_div [2:1]pllmp_prediv [0]analog_pwc
    gc2053_sensor_write_register(ViPipe,0xfc,0x8e);
    /****CISCTL & ANALOG****/
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0x87,0x18);//[6]aec_delay_mode
    gc2053_sensor_write_register(ViPipe,0xee,0x30);//[5:4]dwen_sramen
    gc2053_sensor_write_register(ViPipe,0xd0,0xb7);//ramp_en
    gc2053_sensor_write_register(ViPipe,0x03,0x04);
    gc2053_sensor_write_register(ViPipe,0x04,0x60);
    gc2053_sensor_write_register(ViPipe,0x05,0x05);//05
    gc2053_sensor_write_register(ViPipe,0x06,0x28);//60//[11:0]hb
    gc2053_sensor_write_register(ViPipe,0x07,0x00);
    gc2053_sensor_write_register(ViPipe,0x08,0x11);//19
    gc2053_sensor_write_register(ViPipe,0x09,0x00); 
    gc2053_sensor_write_register(ViPipe,0x0a,0x02); //cisctl row start
    gc2053_sensor_write_register(ViPipe,0x0b,0x00);
    gc2053_sensor_write_register(ViPipe,0x0c,0x02); //cisctl col start
    gc2053_sensor_write_register(ViPipe,0x0d,0x04);
    gc2053_sensor_write_register(ViPipe,0x0e,0x40);
    gc2053_sensor_write_register(ViPipe,0x12,0xe2); //vsync_ahead_mode
    gc2053_sensor_write_register(ViPipe,0x13,0x16);
    gc2053_sensor_write_register(ViPipe,0x19,0x0a); //ad_pipe_num
    gc2053_sensor_write_register(ViPipe,0x21,0x1c);//eqc1fc_eqc2fc_sw
    gc2053_sensor_write_register(ViPipe,0x28,0x0a);//16//eqc2_c2clpen_sw
    gc2053_sensor_write_register(ViPipe,0x29,0x24);//eq_post_width
    gc2053_sensor_write_register(ViPipe,0x2b,0x04);//c2clpen --eqc2
    gc2053_sensor_write_register(ViPipe,0x32,0xf8); //[5]txh_en ->avdd28
    gc2053_sensor_write_register(ViPipe,0x37,0x03); //[3:2]eqc2sel=0
    gc2053_sensor_write_register(ViPipe,0x39,0x15);//17 //[3:0]rsgl

    gc2053_sensor_write_register(ViPipe,0x41,0x05);
    gc2053_sensor_write_register(ViPipe,0x42,0x46);

    gc2053_sensor_write_register(ViPipe,0x43,0x07);//vclamp
    gc2053_sensor_write_register(ViPipe,0x44,0x40); //0e//post_tx_width
    gc2053_sensor_write_register(ViPipe,0x46,0x0b); //txh——3.2v
    gc2053_sensor_write_register(ViPipe,0x4b,0x20); //rst_tx_width
    gc2053_sensor_write_register(ViPipe,0x4e,0x08); //12//ramp_t1_width
    gc2053_sensor_write_register(ViPipe,0x55,0x20); //read_tx_width_pp
    gc2053_sensor_write_register(ViPipe,0x66,0x05); //18//stspd_width_r1
    gc2053_sensor_write_register(ViPipe,0x67,0x05); //40//5//stspd_width_r
    gc2053_sensor_write_register(ViPipe,0x77,0x01); //dacin offset x31
    gc2053_sensor_write_register(ViPipe,0x78,0x00); //dacin offset
    gc2053_sensor_write_register(ViPipe,0x7c,0x93); //[1:0] co1comp
    gc2053_sensor_write_register(ViPipe,0x8c,0x12); //12 ramp_t1_ref
    gc2053_sensor_write_register(ViPipe,0x8d,0x92); //90
    gc2053_sensor_write_register(ViPipe,0x90,0x01);
    gc2053_sensor_write_register(ViPipe,0x9d,0x10);
    gc2053_sensor_write_register(ViPipe,0xce,0x7c);//70//78//[4:2]c1isel
    gc2053_sensor_write_register(ViPipe,0xd2,0x41);//[5:3]c2clamp
    gc2053_sensor_write_register(ViPipe,0xd3,0xdc);//ec//0x39[7]=0,0xd3[3]=1 rsgh=vref
    gc2053_sensor_write_register(ViPipe,0xe6,0x50);//ramps offset
    /*gain*/
    gc2053_sensor_write_register(ViPipe,0xb6,0xc0);
    gc2053_sensor_write_register(ViPipe,0xb0,0x70);
    gc2053_sensor_write_register(ViPipe,0xb1,0x01);
    gc2053_sensor_write_register(ViPipe,0xb2,0x00);
    gc2053_sensor_write_register(ViPipe,0xb3,0x00);
    gc2053_sensor_write_register(ViPipe,0xb4,0x00);
    gc2053_sensor_write_register(ViPipe,0xb8,0x01);
    gc2053_sensor_write_register(ViPipe,0xb9,0x00);
    /*blk*/
    gc2053_sensor_write_register(ViPipe,0x26,0x30);//23 //[4]写0，全n mode
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x40,0x23);
    gc2053_sensor_write_register(ViPipe,0x55,0x07);
    gc2053_sensor_write_register(ViPipe,0x60,0x40); //[7:0]WB_offset
    gc2053_sensor_write_register(ViPipe,0xfe,0x04);
    gc2053_sensor_write_register(ViPipe,0x14,0x78); //g1 ratio
    gc2053_sensor_write_register(ViPipe,0x15,0x78); //r ratio
    gc2053_sensor_write_register(ViPipe,0x16,0x78); //b ratio
    gc2053_sensor_write_register(ViPipe,0x17,0x78); //g2 ratio
    /*window*/
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x92,0x00); //win y1
    gc2053_sensor_write_register(ViPipe,0x94,0x03); //win x1
    gc2053_sensor_write_register(ViPipe,0x95,0x04);
    gc2053_sensor_write_register(ViPipe,0x96,0x38); //[10:0]out_height
    gc2053_sensor_write_register(ViPipe,0x97,0x07);
    gc2053_sensor_write_register(ViPipe,0x98,0x80); //[11:0]out_width
    /*ISP*/
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x01,0x05);//03//[3]dpc blending mode [2]noise_mode [1:0]center_choose 2b'11:median 2b'10:avg 2'b00:near
    gc2053_sensor_write_register(ViPipe,0x02,0x89); //[7:0]BFF_sram_mode
    gc2053_sensor_write_register(ViPipe,0x04,0x01); //[0]DD_en
    gc2053_sensor_write_register(ViPipe,0x07,0xa6);
    gc2053_sensor_write_register(ViPipe,0x08,0xa9);
    gc2053_sensor_write_register(ViPipe,0x09,0xa8);
    gc2053_sensor_write_register(ViPipe,0x0a,0xa7);
    gc2053_sensor_write_register(ViPipe,0x0b,0xff);
    gc2053_sensor_write_register(ViPipe,0x0c,0xff);
    gc2053_sensor_write_register(ViPipe,0x0f,0x00);
    gc2053_sensor_write_register(ViPipe,0x50,0x1c);
    gc2053_sensor_write_register(ViPipe,0x89,0x03);
    gc2053_sensor_write_register(ViPipe,0xfe,0x04);
    gc2053_sensor_write_register(ViPipe,0x28,0x86);
    gc2053_sensor_write_register(ViPipe,0x29,0x86);
    gc2053_sensor_write_register(ViPipe,0x2a,0x86);
    gc2053_sensor_write_register(ViPipe,0x2b,0x68);
    gc2053_sensor_write_register(ViPipe,0x2c,0x68);
    gc2053_sensor_write_register(ViPipe,0x2d,0x68);
    gc2053_sensor_write_register(ViPipe,0x2e,0x68);
    gc2053_sensor_write_register(ViPipe,0x2f,0x68);
    gc2053_sensor_write_register(ViPipe,0x30,0x4f);
    gc2053_sensor_write_register(ViPipe,0x31,0x68);
    gc2053_sensor_write_register(ViPipe,0x32,0x67);
    gc2053_sensor_write_register(ViPipe,0x33,0x66);
    gc2053_sensor_write_register(ViPipe,0x34,0x66);
    gc2053_sensor_write_register(ViPipe,0x35,0x66);
    gc2053_sensor_write_register(ViPipe,0x36,0x66);
    gc2053_sensor_write_register(ViPipe,0x37,0x66);
    gc2053_sensor_write_register(ViPipe,0x38,0x62);
    gc2053_sensor_write_register(ViPipe,0x39,0x62);
    gc2053_sensor_write_register(ViPipe,0x3a,0x62);
    gc2053_sensor_write_register(ViPipe,0x3b,0x62);
    gc2053_sensor_write_register(ViPipe,0x3c,0x62);
    gc2053_sensor_write_register(ViPipe,0x3d,0x62);
    gc2053_sensor_write_register(ViPipe,0x3e,0x62);
    gc2053_sensor_write_register(ViPipe,0x3f,0x62);
    /****DVP & MIPI****/
    gc2053_sensor_write_register(ViPipe,0xfe,0x01);
    gc2053_sensor_write_register(ViPipe,0x9a,0x06);//[5]OUT_gate_mode [4]hsync_delay_half_pclk [3]data_delay_half_pclk [2]vsync_polarity [1]hsync_polarity [0]pclk_out_polarity
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0x7b,0x2a);//[7:6]updn [5:4]drv_high_data [3:2]drv_low_data [1:0]drv_pclk
    gc2053_sensor_write_register(ViPipe,0x23,0x2d);//[3]rst_rc [2:1]drv_sync [0]pwd_rc
    gc2053_sensor_write_register(ViPipe,0xfe,0x03);
    gc2053_sensor_write_register(ViPipe,0x01,0x27);//20//27[6:5]clkctr [2]phy-lane1_en [1]phy-lane0_en [0]phy_clk_en
    gc2053_sensor_write_register(ViPipe,0x02,0x56);//[7:6]data1ctr [5:4]data0ctr [3:0]mipi_diff
    gc2053_sensor_write_register(ViPipe,0x03,0xb6);//b2//b6[7]clklane_p2s_sel [6:5]data0hs_ph [4]data0_delay1s [3]clkdelay1s [2]mipi_en [1:0]clkhs_ph
    gc2053_sensor_write_register(ViPipe,0x12,0x80);
    gc2053_sensor_write_register(ViPipe,0x13,0x07);//LWC
    gc2053_sensor_write_register(ViPipe,0x15,0x12);//[1:0]clk_lane_mode
    gc2053_sensor_write_register(ViPipe,0xfe,0x00);
    gc2053_sensor_write_register(ViPipe,0x3e,0x91);//40//91[7]lane_ena [6]DVPBUF_ena [5]ULPEna [4]MIPI_ena [3]mipi_set_auto_disable [2]RAW8_mode [1]ine_sync_mode [0]double_lane_en
#endif
    printf("=====================================================================\n");
    printf("== Galaxycore gc2053 sensor 1920X1080_30fps linear init success!   ==\n");
    printf("=====================================================================\n");
    return;
}


/* 1080P@30fps LINEAR; Raw:10Bit; MCLK Input: 24MHz; MIPI CLK: 594Mbps/lane; CSI-2 2Lane; */
void sensor_wdr_720p30_2to1_init(VI_PIPE ViPipe)
{
    printf("============================================================\n");
    printf("== Galaxycore gc2053 Not support HDR!  ==\n");
    printf("============================================================\n");
    return;
}

