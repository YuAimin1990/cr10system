#if !defined(__GST612C_CMOS_H_)
#define __GST612C_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_comm_vi.h"
#include "ar_sns_ctrl.h"
#include "mpi_sys.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_vi.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_vin.h"
#include "osal.h"
#include "gst612c.h"

#define GST612C_CALIB_TARGET      8192


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


static gst612c_pipe_ctx_t g_gst612c_pipe_ctx[VI_MAX_PIPE_NUM] = {
    [0 ... VI_MAX_PIPE_NUM-1] = {
        .s8I2cDevId = -1,
        .s32I2cFd = -1,
        .s32VinFd = -1,
        .s32NucFd = -1,
        .enCalibState = GST612C_CALIB_NONE,
        .s32RaAdjCalibFrmIdx = -1,
        .s32HssdCalibFrmIdx = -1,
    },
};

/*
612c setting, 37.5M mclk, 50Hz, 640x520
开窗参数: (640+76)*(520+3)+532 @P_MC 37.5MHZ
*/
#if 0
static gst612c_reg_t g_gst612c_setting_0[] = {
//   addr, val
    {0x0c, 0x20},		//0x0c  Frame_rate[7:0]
    {0x0d, 0x03},		//0x0d  Frame_rate[11:8]

    {0x02, 0x80},       //0x02  行有效[7:0]
    {0x03, 0x02},		//0x03  行有效[8]
    {0x04, 0x08},		//0x04  场有效[7:0]
    {0x05, 0x02},		//0x05  场有效[8]
    {0x06, 0x4c},		//0x06  行消隐[7:0]
    {0x07, 0x00},		//0x07  行消隐[9:8]
    {0x08, 0x03},		//0x08  场消隐[7:0]
    {0x09, 0x00},		//0x09  场消隐[9:8]

    {0x0a, 0x14},		//0x0a  最后行场消隐点数
    {0x0b, 0x02},		//0x0b  最后行场消隐点数

    //***ADC时钟频率计算公式：AD_CLK_OUT = MC/2^pre_div * pll_m/pll_n 
    //***如果按照当前的配置计算： 14.75/2^1 * 64 / 2 = 236MHz,GST612探测器ADC时钟频率设置为236MHz左右
    {0x2F, 0x42},		//0x2F  //pre_div为寄存器低3bit（bit[2:0]）
    {0x30, 0x4B},		//0x30  //pll_m
    {0x31, 0x02},		//0x31  //pll_n
    {0x32, 0x00},		//0x32  //pll_reset
 
    {0x01, 0x01},		//0x01   芯片初始化完成
};
#endif

/*
612c setting, 37.5M mclk, 50Hz, 640x520
开窗参数: (640+76)*(520+4)+0 @P_MC 37.5MHZ
*/
static gst612c_reg_t g_gst612c_setting_1[] = {
//   addr, val
    {0x0c, 0x20},		//0x0c  Frame_rate[7:0]
    {0x0d, 0x03},		//0x0d  Frame_rate[11:8]

    {0x02, 0x80},       //0x02  行有效[7:0]
    {0x03, 0x02},		//0x03  行有效[8]
    {0x04, 0x08},		//0x04  场有效[7:0]
    {0x05, 0x02},		//0x05  场有效[8]
    {0x06, 0x4C},		//0x06  行消隐[7:0]
    {0x07, 0x00},		//0x07  行消隐[9:8]
    {0x08, 0x04},		//0x08  场消隐[7:0]
    {0x09, 0x00},		//0x09  场消隐[9:8]

    {0x0a, 0x00},		//0x0a  最后行场消隐点数
    {0x0b, 0x00},		//0x0b  最后行场消隐点数

    //***ADC时钟频率计算公式：AD_CLK_OUT = MC/2^pre_div * pll_m/pll_n 
    //***如果按照当前的配置计算： 14.75/2^1 * 64 / 2 = 236MHz,GST612探测器ADC时钟频率设置为236MHz左右
    {0x2F, 0x42},		//0x2F  //pre_div为寄存器低3bit（bit[2:0]）
    {0x30, 0x4B},		//0x30  //pll_m
    {0x31, 0x02},		//0x31  //pll_n
    {0x32, 0x00},		//0x32  //pll_reset
 
    {0x01, 0x01},		//0x01   芯片初始化完成
};

AR_S32 gst612c_read_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 *data)
{
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];
    return ar_hal_i2c_master_read(pipe_ctx->s32I2cFd, GST612C_I2C_SLAVE_ADDR, 7, &addr, 1, data, 1);
}

AR_S32 gst612c_write_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 data)
{
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];
    AR_U8 buf[2];

    buf[0] = (AR_U8)addr;
    buf[1] = (AR_U8)data;

    return ar_hal_i2c_master_write(pipe_ctx->s32I2cFd, GST612C_I2C_SLAVE_ADDR, 7, buf, 2);
}

static AR_S32 gst612c_init_ctx(VI_PIPE ViPipe)
{
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];

    pipe_ctx->s32VinFd = ar_hal_vin_open_vin_dev();
    if (pipe_ctx->s32VinFd < 0) {
        ar_err("pipe[%d] open vin device failed", ViPipe);
        return -1;
    }

    pipe_ctx->s32NucFd = ar_hal_vin_open_nuc_dev();
    if (pipe_ctx->s32NucFd < 0) {
        ar_err("pipe[%d] open nuc device failed", ViPipe);
        return -1;
    }

    return 0;
}

static void gst612c_deinit_ctx(VI_PIPE ViPipe)
{
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];

    if (pipe_ctx->s32VinFd >= 0) {
        ar_hal_vin_close_vin_dev(pipe_ctx->s32VinFd);
        pipe_ctx->s32VinFd = -1;
    }

    if (pipe_ctx->s32NucFd >= 0) {
        ar_hal_vin_close_nuc_dev(pipe_ctx->s32NucFd);
        pipe_ctx->s32NucFd = -1;
    }
}

#if 0
static void gst612c_adc_calib_auto_en(VI_PIPE ViPipe)
{
    gst612c_write_register(ViPipe, 0x7c, 0xa9);      //to page1;
    gst612c_write_register(ViPipe, 0x2f, 0x00);  //0x80);
    gst612c_write_register(ViPipe, 0x7c, 0xa8);      //to page0;
}

static void gst612c_adc_calib(VI_PIPE ViPipe)
{
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];
    volatile unsigned char ADC_CALIB_SIZE_USER = 19;
    //volatile unsigned char ADC_CALIB_ENABLE_FLAG;
    //volatile unsigned char flag;
    //volatile unsigned char ADC_TEST_ENABLE_MODE;
    volatile unsigned short rd_data_h = 0;
    volatile unsigned short rd_data_l = 0;
    volatile unsigned short ADC_OUT_TEMPERATURE_BUS = 0;

    gst612c_write_register(ViPipe, 0x7c, 0xa9);      //to page1;
    gst612c_write_register(ViPipe, 0x1c, 0x07);      //ADC_CALIB_ENABLE_FLAG[2:0] = 7;

    gst612c_write_register(ViPipe, 0x10, (ADC_CALIB_SIZE_USER & 0x1f));      //ADC_CALIB_SIZE_USER

    gst612c_write_register(ViPipe, 0x1D, 0x03);
    ar_delay_us(192);       //3行 64*3
    gst612c_read_register(ViPipe, 0x14, (AR_U8 *)&rd_data_l);
    gst612c_read_register(ViPipe, 0x15, (AR_U8 *)&rd_data_h);
    pipe_ctx->niossysctl.gNmiddle_first = (rd_data_h << 8) + rd_data_l;

    gst612c_write_register(ViPipe, 0x1D, 0x01);
    ar_delay_us(192);       //3行 64*3
    gst612c_read_register(ViPipe, 0x14, (AR_U8 *)&rd_data_l);
    gst612c_read_register(ViPipe, 0x15, (AR_U8 *)&rd_data_h);
    pipe_ctx->niossysctl.gNmin_first = (rd_data_h << 8) + rd_data_l;

    gst612c_write_register(ViPipe, 0x1D, 0x07);
    ar_delay_us(192);       //3行 64*3
    gst612c_read_register(ViPipe, 0x14, (AR_U8 *)&rd_data_l);
    gst612c_read_register(ViPipe, 0x15, (AR_U8 *)&rd_data_h);
    ADC_OUT_TEMPERATURE_BUS = (rd_data_h << 8) + rd_data_l;

    pipe_ctx->niossysctl.g17_Vtemp_V = ((int)ADC_OUT_TEMPERATURE_BUS - (int)pipe_ctx->niossysctl.gNmin_first)
        * 1000 / ((int)pipe_ctx->niossysctl.gNmiddle_first - (int)pipe_ctx->niossysctl.gNmin_first) + 550;
    pipe_ctx->niossysctl.Dvtemp0 = (pipe_ctx->niossysctl.g17_Vtemp_V - 1101) * 100;
    pipe_ctx->niossysctl.gTemp_first = pipe_ctx->niossysctl.Dvtemp0 / 12;

    ar_always("ADC_OUT_TEMPERATURE_BUS = %u gNmin_first = %u  gNmiddle_first = %u  g17_Vtemp_V = %d",
        ADC_OUT_TEMPERATURE_BUS, pipe_ctx->niossysctl.gNmin_first,
        pipe_ctx->niossysctl.gNmiddle_first, pipe_ctx->niossysctl.g17_Vtemp_V);

    //ADC_CALIB_NMIN_NUM_USER[7:0]
    gst612c_write_register(ViPipe, 0x08, (0x00ff & pipe_ctx->niossysctl.gNmin_first));
    //ADC_CALIB_NMIN_NUM_USER[14:8]
    gst612c_write_register(ViPipe, 0x09, (0x007f & (pipe_ctx->niossysctl.gNmin_first >> 8)));

    gst612c_write_register(ViPipe, 0x7c, 0xa8);      //to page0;
}
#endif

/***************************************************************************
    412c sensor ctl function
***************************************************************************/
static unsigned short x1_cursor, x2_cursor, sensor_version;
static void rd_reg(VI_PIPE ViPipe)
{
    volatile unsigned short rd_data_h = 0,rd_data_l = 0;
    volatile unsigned char  rd_data = 0;

    gst612c_write_register(ViPipe, 0x7c, 0xa8);
    gst612c_write_register(ViPipe, 0x01, 0x01);
    usleep(5000);
    gst612c_write_register(ViPipe, 0x7c, 0xa9);  //to page1;

    gst612c_read_register(ViPipe,0x33,(AR_U8 *)&rd_data_l);
    gst612c_read_register(ViPipe,0x34,(AR_U8 *)&rd_data_h);
    x1_cursor = ((rd_data_h & 0x07) << 8)|(rd_data_l & 0xff);
	if(x1_cursor >= 640)
	{
		x1_cursor = 0;
	}
    gst612c_read_register(ViPipe,0x35,(AR_U8 *)&rd_data_l);
    gst612c_read_register(ViPipe,0x36,(AR_U8 *)&rd_data_h);
    x2_cursor = ((rd_data_h & 0x07) << 8)|(rd_data_l & 0xff);
	if(x2_cursor >= 640)
	{
		x2_cursor = 0;
	}
	
	gst612c_read_register(ViPipe,0x32,(AR_U8 *)&rd_data);
	if((rd_data & 0xf0) == 0x30)
	{
	   sensor_version = rd_data & 0x0f;
	}
	
   gst612c_write_register(ViPipe,0x7c, 0xa8);  //to page0;
//    bc_replace(0x01,x1_cursor, 0x01,x2_cursor);
//    SetSENSOR(0x7c, 0xa8);
}

static void tblr_time(VI_PIPE ViPipe)
{
	gst612c_write_register(ViPipe,0x7c,0xa9); //to page1;
	gst612c_write_register(ViPipe,0x00,0x00);//niossysctl.tblr_time = 0;
	gst612c_write_register(ViPipe,0x7c,0xa8); //to page0;
}

static void bc_replace(VI_PIPE ViPipe, unsigned char bp1_adj_en, unsigned short bp1_col_position, unsigned char bp2_adj_en, unsigned short bp2_col_position)
{
	unsigned short wr_bp_data1=0,wr_bp_data2=0;
	unsigned char wr_data = 0;

	gst612c_write_register(ViPipe,0x7c, 0xa9);
	//setting bc1;
	wr_bp_data1 = bp1_col_position + 5;
	wr_data = wr_bp_data1&0x00ff;
	gst612c_write_register(ViPipe,0x0e, wr_data);
	wr_data = ((0x01&bp1_adj_en)<<7) + ((wr_bp_data1>>8)&0x03);
	gst612c_write_register(ViPipe,0x0f, wr_data);

	//setting bc2;
	wr_bp_data2 = bp2_col_position + 5;
	wr_data = wr_bp_data2&0x00ff;
	gst612c_write_register(ViPipe,0x10, wr_data);
	wr_data = ((0x01&bp2_adj_en)<<7) + ((wr_bp_data2>>8)&0x03);
	gst612c_write_register(ViPipe,0x11, wr_data);
	usleep(10);
	//to reg_mapping page0;
	gst612c_write_register(ViPipe,0x7c, 0xa8);
}

static void SetSENSOR(VI_PIPE ViPipe, AR_U8 addr, AR_U8 data)
{
    AR_S32 s32Ret;
    AR_U8 u8Rd;
    s32Ret = gst612c_write_register(ViPipe,addr,data); //to page3;
    if(s32Ret<0) {
        printf("write [0x%02x 0x%02X] ret=%d\n", addr, data, s32Ret);
        if(addr!=0x7c)
        {
            s32Ret = gst612c_read_register(ViPipe, addr, &u8Rd);
            printf("try read [0x%02x 0x%02X] ret=%d\n", addr, data, s32Ret);
        }
    }
}

static void wr_reg(VI_PIPE ViPipe)
{
    SetSENSOR(ViPipe,0x7c,0xab); //to page3;
	SetSENSOR(ViPipe,0x01,0x03);
	SetSENSOR(ViPipe,0x05,0x03);
	SetSENSOR(ViPipe,0x09,0x40);
	SetSENSOR(ViPipe,0x0a,0xc0);
	SetSENSOR(ViPipe,0x0d,0x00);
	SetSENSOR(ViPipe,0x0f,0x08);
	SetSENSOR(ViPipe,0x10,0x20);
	SetSENSOR(ViPipe,0x19,0x07);
	SetSENSOR(ViPipe,0x1B,0xFF);
	SetSENSOR(ViPipe,0x1d,0x3e); 
	SetSENSOR(ViPipe,0x1e,0xff);//RA 20210331
	SetSENSOR(ViPipe,0x20,0xff);//RA 20210331
	SetSENSOR(ViPipe,0x21,0xff);//RA 20210331
	SetSENSOR(ViPipe,0x7c,0xa9);//to page1;
    SetSENSOR(ViPipe,0x27,0x0E);//
    SetSENSOR(ViPipe,0x7c,0xa8);//to page0;
    SetSENSOR(ViPipe,0x20,0x07);//
	SetSENSOR(ViPipe,0x2c,0x02);//
	SetSENSOR(ViPipe,0x35,0x3d);

    SetSENSOR(ViPipe,0x7c,0xab); //to page3;
    SetSENSOR(ViPipe,0x06,0xff);
	SetSENSOR(ViPipe,0x11,0x20);
	SetSENSOR(ViPipe,0x12,0x07);
	SetSENSOR(ViPipe,0x18,0x27);
	SetSENSOR(ViPipe,0x7C,0xA9);//page1
	SetSENSOR(ViPipe,0x06,0x36);//
	SetSENSOR(ViPipe,0x07,0x0c);//
	SetSENSOR(ViPipe,0x7c,0xa8);//to page0;
    SetSENSOR(ViPipe,0x1f,0x0b);
	
    if(sensor_version == 3) 
	{
	   SetSENSOR(ViPipe,0x7c,0xab);//to page3;
	   SetSENSOR(ViPipe,0x18,0x24);//RD 
	   SetSENSOR(ViPipe,0x12,0x04);//RD 
	   SetSENSOR(ViPipe,0x7c,0xa9);//to page1
	   SetSENSOR(ViPipe,0x07,0x0d);
	   SetSENSOR(ViPipe,0x24,0x80);
	   SetSENSOR(ViPipe,0x3D,0x00);
	   SetSENSOR(ViPipe,0x42,0x05); 
	   SetSENSOR(ViPipe,0x7c,0xa8); 
	}
}

//**************************************************************************
//////// Rasel设置 0-8
//**************************************************************************
static void RaselSet(VI_PIPE ViPipe, AR_U8 u8Rasel)
{
	unsigned char temp_data = 0;
    if(u8Rasel > 8)
	{
		u8Rasel = 8;
	}
	gst612c_write_register(ViPipe,0x7C, 0xAB);//page3
	if(u8Rasel <= 8)//0x20
	{
		temp_data = u8Rasel;
		temp_data = 0xff >> (8 - temp_data);
		gst612c_write_register(ViPipe,0x20, temp_data); 
		gst612c_write_register(ViPipe,0x21, 0xff); 
	}
	gst612c_write_register(ViPipe,0x7C, 0xA8);//page0
}
//**************************************************************************
//////// Hssd设置 0-127
//**************************************************************************
static void HssdSet(VI_PIPE ViPipe, AR_U8 u8Hssd)
{
    gst612c_write_register(ViPipe,0x7C, 0xAB);//page3
    gst612c_write_register(ViPipe,0x09,u8Hssd & 0x7f);
    gst612c_write_register(ViPipe,0x7C, 0xA8);//page0
}

static void RaselHssdRegGet(VI_PIPE ViPipe, AR_U8 *pRasel, AR_U8 *pHssd)
{
	AR_U8 rdata = 0;
	gst612c_write_register(ViPipe,0x7C, 0xAB);//page3
	gst612c_read_register(ViPipe, 0x20, &rdata);
	*pRasel = rdata;
    gst612c_read_register(ViPipe, 0x09, &rdata);
	*pHssd = rdata&0x7f;
    gst612c_write_register(ViPipe,0x7C, 0xA8);//page0
}
static void RaselRegSet(VI_PIPE ViPipe, AR_U8 u8RaselReg)
{
	gst612c_write_register(ViPipe,0x7C, 0xAB);//page3
    gst612c_write_register(ViPipe,0x20, u8RaselReg);
	gst612c_write_register(ViPipe,0x21, 0xff);
    gst612c_write_register(ViPipe,0x7C, 0xA8);//page0
}

//**************************************************************************
//////// INT设置 16-240
//**************************************************************************
static void IntSet(VI_PIPE ViPipe, AR_U8 u8Int )
{
    if(u8Int > 240)
    {
        u8Int= 240;
    }
    if( u8Int < 16)
    {
        u8Int = 16;
    }

    gst612c_write_register(ViPipe,0x7C, 0xA8);///page0
    gst612c_write_register(ViPipe,0x2E, ( u8Int & 0x0F00)>>8);
    gst612c_write_register(ViPipe,0x2D, u8Int & 0x00FF);
}

//**************************************************************************
////////  Gain设置 0-7
//**************************************************************************
static void GainSet(VI_PIPE ViPipe, AR_U8 u8Gain )
{
    if( u8Gain > 7)
    {
        u8Gain = 7;
    }
    gst612c_write_register(ViPipe,0x7C, 0xA8);///page0
    gst612c_write_register(ViPipe,0x28, ( u8Gain & 0xFF));
}

static void ParamInit(VI_PIPE ViPipe)
{
    GainSet(ViPipe, 4 );
    IntSet(ViPipe, 60 );
    HssdSet(ViPipe, 64 );
    RaselSet(ViPipe, 4 );
}
/*
    412c sensor ctl function end
*/

static void gst612c_stream_on(VI_PIPE ViPipe)
{
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];
	volatile unsigned int i;
	volatile unsigned char rd_data=0;
	volatile unsigned char flag = 0;
	volatile unsigned int seed_ready_cnt = 0;
    AR_S32 s32Ret;
	
	int gpNum_rst = ar_hal_gpio_name_to_num(GROUP_0, 6, 0);

	//612c sensor ctl
	rd_reg(ViPipe);
    ar_hal_gpio_set_value(gpNum_rst, 0);//重新对探测器复位
    //ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A104, 0, 0, 0);
    usleep(5000);
    ar_hal_gpio_set_value(gpNum_rst, 1);
    //ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A104, 1, 0, 0);
    usleep(5000);
    tblr_time(ViPipe);
	printf("sensor version %u\n", sensor_version);
    #if 0
	if(sensor_version >= 2)
	{
       bc_replace(ViPipe,0x01,x1_cursor, 0x01,x2_cursor);
	}
    #endif
    bc_replace(ViPipe,0x01,x1_cursor, 0x01,x2_cursor);

	for (i = 0; i < sizeof(g_gst612c_setting_1) / sizeof(gst612c_reg_t); i++) {
		s32Ret = gst612c_write_register(ViPipe, g_gst612c_setting_1[i].addr, g_gst612c_setting_1[i].val);
        if(s32Ret<0){
            printf("write setting reg failed ret=%d addr=%x data=%x\n",s32Ret,g_gst612c_setting_1[i].addr, g_gst612c_setting_1[i].val);
        }
		usleep(10);
		s32Ret = gst612c_read_register(ViPipe, g_gst612c_setting_1[i].addr, (AR_U8 *)&rd_data);
        if(s32Ret<0){
            printf("read setting reg failed ret=%d addr=%x\n",s32Ret,g_gst612c_setting_1[i].addr);
        }
		usleep(10);
    }

	rd_data = 1;
	usleep(5000);

	//等待初始化完成
    while((rd_data == 1)&&(seed_ready_cnt < 200))
    {
        flag = gst612c_read_register(ViPipe,0x01,(AR_U8 *)&rd_data);
        if(flag<0){
            usleep(10);
            continue;
        }
	   rd_data = rd_data & 0x01;
	   seed_ready_cnt++;
    }
    printf("initial_start %d cnt=%d\n", rd_data, seed_ready_cnt);
    usleep(100);

    //探测器时序驱动使能
    ar_hal_vin_nuc_start(pipe_ctx->s32NucFd);
    usleep(10*1000);

	wr_reg(ViPipe);

	//全局寄存器初始化
	usleep(10);
	ParamInit(ViPipe);
}

static AR_S32 gst612c_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

	int gpNum_1v8 = ar_hal_gpio_name_to_num(GROUP_0, 6, 2);
	int gpNum_3v6 = ar_hal_gpio_name_to_num(GROUP_0, 6, 1);
	int gpNum_6v5 = ar_hal_gpio_name_to_num(GROUP_0, 0, 18);
	int gpNum_rst = ar_hal_gpio_name_to_num(GROUP_0, 6, 0);

	ar_hal_gpio_export(gpNum_1v8);
	ar_hal_gpio_export(gpNum_3v6);
	ar_hal_gpio_export(gpNum_6v5);
	ar_hal_gpio_export(gpNum_rst);
	ar_hal_gpio_set_dir(gpNum_1v8, OUTPUT);
	ar_hal_gpio_set_dir(gpNum_3v6, OUTPUT);
	ar_hal_gpio_set_dir(gpNum_6v5, OUTPUT);
	ar_hal_gpio_set_dir(gpNum_rst, OUTPUT);

	ar_hal_gpio_set_value(gpNum_rst, 0);

	ar_hal_gpio_set_value(gpNum_1v8, 1);
	ar_delay(1);
	ar_hal_gpio_set_value(gpNum_3v6, 1);
	ar_delay(1);
	ar_hal_gpio_set_value(gpNum_6v5, 1);
	ar_delay(10);

#if 0
	//set gpio output
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A108, 0, 2, 2);
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A108, 0, 1, 1);
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A0C0, 0, 18, 18);
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A108, 0, 0, 0);

	//set reset 0
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A104, 0, 0, 0);

	//power
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A104, 1, 2, 2);
	ar_delay(1);
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A104, 1, 1, 1);
	ar_delay(1);
	ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10A0BC, 1, 18, 18);
	ar_delay(10);
#endif

    //set clk reg
    //ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10404C, 0, 24, 26);
    //ar_hal_vin_write_reg(g_gst612c_pipe_ctx[ViPipe].s32VinFd, 0xA10404C, 0x7, 16, 22);
	//set nuc clk output
    ret = ar_hal_vin_nuc_set_out_clk(g_gst612c_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 8, NUC_CLK_PHASE_180, AR_TRUE);
    if (ret) {
        ar_err("Pipe[%d] gst612c set nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(1);
    ar_always("Pipe[%d] gst612c power on", ViPipe);

	//set reset 1
    ar_hal_gpio_set_value(gpNum_rst, 1);
	ar_delay(3);
	
    return ret;
}

static AR_S32 gst612c_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

    int gpNum_1v8 = ar_hal_gpio_name_to_num(GROUP_0, 6, 2);
    int gpNum_3v6 = ar_hal_gpio_name_to_num(GROUP_0, 6, 1);
    int gpNum_6v5 = ar_hal_gpio_name_to_num(GROUP_0, 0, 18);
    int gpNum_rst = ar_hal_gpio_name_to_num(GROUP_0, 6, 0);

    ar_hal_gpio_export(gpNum_1v8);
    ar_hal_gpio_export(gpNum_3v6);
    ar_hal_gpio_export(gpNum_6v5);
    ar_hal_gpio_export(gpNum_rst);
    ar_hal_gpio_set_dir(gpNum_1v8, OUTPUT);
    ar_hal_gpio_set_dir(gpNum_3v6, OUTPUT);
    ar_hal_gpio_set_dir(gpNum_6v5, OUTPUT);
    ar_hal_gpio_set_dir(gpNum_rst, OUTPUT);

    ar_hal_gpio_set_value(gpNum_rst, 0);

    ret = ar_hal_vin_nuc_set_out_clk(g_gst612c_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 8, NUC_CLK_PHASE_180, AR_FALSE);
    if (ret) {
        ar_err("Pipe[%d] gst612c set nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_hal_gpio_set_value(gpNum_6v5, 0);
    ar_delay(1);
    ar_hal_gpio_set_value(gpNum_3v6, 0);
    ar_delay(1);
    ar_hal_gpio_set_value(gpNum_1v8, 0);
    ar_delay(10);

    ar_always("Pipe[%d] gst612c power off", ViPipe);

    return ret;
}

static AR_S32 gst612c_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    ar_always("pipe[%d] w[%u] h[%u] fps[%f] sns_mode[%u]",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstSensorImageMode->u16Width != 640 || pstSensorImageMode->u16Height != 520
        || (pstSensorImageMode->f32Fps != 25 && pstSensorImageMode->f32Fps != 50)) {
        ar_err("pipe[%d] image_mode[w=%u h=%u fps=%f] invalid", ViPipe,
            pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, pstSensorImageMode->f32Fps);
        return AR_ERR_VI_INVALID_PARA;
    }

    g_gst612c_pipe_ctx[ViPipe].stImageMode = *pstSensorImageMode;

    return 0;
}

void gst612c_init(VI_PIPE ViPipe)
{
    STRU_NUC_CFG_T nuc_cfg = {
        .intf_mode = NUC_INTF_MODE_GAODE_3IN,
        .active_width = 640,
        .active_height = 520,
        .total_width = 640+76,
        .total_height = 520+4,
        .occ_buf_align = NUC_OCC_BUF_ALIGN_256,
        .blank_value = 0x20,
        .fs_head = 0xffffc1,
        .fs_head_bitnum = 24,
        .fs_head_ver_loc = 524 - 520 - 1,   //0,
        .fs_head_hor_loc = 698,
        .ls_head = 0xffffc0,
        .ls_head_bitnum = 24,
        .ls_head_ver_loc_start = 524 - 520 - 1, //0,
        .ls_head_ver_loc_end = 524 - 2, //307,
    };

    //printf("nuc cfg %d %d %d %d\n", nuc_cfg.active_width, nuc_cfg.active_height, nuc_cfg.total_width, nuc_cfg.total_height);
    ar_hal_vin_nuc_cfg(g_gst612c_pipe_ctx[ViPipe].s32NucFd, &nuc_cfg);
    gst612c_stream_on(ViPipe);
}

void gst612c_exit(VI_PIPE ViPipe)
{
    ar_hal_vin_nuc_stop(g_gst612c_pipe_ctx[ViPipe].s32NucFd);
}

static ISP_CMOS_DNG_COLORPARAM_S g_stGst612cDngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 gst612c_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = GST612C_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &g_stGst612cDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));
    strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}

AR_S32 gst612c_get_ir_sns_attr(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstIrSnsAttr == NULL) {
        ar_err("pstIrSnsAttr is null");
        return AR_ERR_VI_INVALID_PARA;
    }

    pstIrSnsAttr->u32IrParamSize = 2;
    pstIrSnsAttr->enOccType = IR_SNS_OCC_TYPE_EXTERNAL;
    pstIrSnsAttr->enOccMonotonic = IR_SNS_OCC_MONOTONIC_DECREASE;
    pstIrSnsAttr->u32MinOccVal = 0;
    pstIrSnsAttr->u32MaxOccVal = 0x3E;
    pstIrSnsAttr->s32OccVerticalOffset = 0;
    pstIrSnsAttr->bOccCalibBySns = AR_FALSE;    // use common occ calibration method implemented by sdk
    pstIrSnsAttr->stSnsSize.u32Width = 640;
    pstIrSnsAttr->stSnsSize.u32Height = 520;
    pstIrSnsAttr->stRoiRect.s32X = 0;
    pstIrSnsAttr->stRoiRect.s32Y = 4;
    pstIrSnsAttr->stRoiRect.u32Width = 640;
    pstIrSnsAttr->stRoiRect.u32Height = 512;
    return 0;
}

AR_S32 gst612c_set_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 u32ParamSize)
{
    AR_U8 *p = NULL;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || u32ParamSize != 2) {
        ar_err("pParamData[%p] or u32ParamSize[%u] invalid",
            pParamData, u32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    p = (AR_U8 *)pParamData;
	RaselRegSet(ViPipe, p[0]);
	HssdSet(ViPipe, p[1]);

    ar_always("pipe[%d] set sensor ir param: ra_sel[0x%02x] hssd[0x%02x]",
        ViPipe, p[0], p[1]);

    return 0;
}

AR_S32 gst612c_get_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 *pu32ParamSize)
{
    AR_U8 *p = NULL;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || pu32ParamSize == NULL) {
        ar_err("pParamData[%p] or pu32ParamSize[%p] is invalid",
            pParamData, pu32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    p = (AR_U8 *)pParamData;
	RaselHssdRegGet(ViPipe, &p[0], &p[1]);
	*pu32ParamSize = 2;

    ar_always("pipe[%d] get sensor ir param: ra_sel[0x%02x] hssd[0x%02x]",
        ViPipe, p[0], p[1]);

    return 0;
}

AR_S32 gst612c_get_sns_temperature(VI_PIPE ViPipe, AR_S32 *ps32Temperature)
{
#if 0
    AR_U16 dvtemp_d5 = 0;
    AR_U16 dvtemp_d6 = 0;
    AR_U16 dvtemp = 0;
    AR_S32 g17_Vtemp_V = 0;
    AR_S32 Dvtemp0 = 0;
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];

    gst612c_write_register(ViPipe, 0x7c, 0xa9);
    ar_delay_us(100);
    gst612c_read_register(ViPipe, 0x14, (AR_U8 *)(&dvtemp_d5));
    gst612c_read_register(ViPipe, 0x15, (AR_U8 *)(&dvtemp_d6));
    dvtemp = (dvtemp_d6 << 8) + dvtemp_d5;
    gst612c_write_register(ViPipe, 0x7c, 0xa8);

    g17_Vtemp_V = ((AR_S32)dvtemp - (AR_S32)pipe_ctx->niossysctl.gNmin_first)*1000
        / ((AR_S32)pipe_ctx->niossysctl.gNmiddle_first - (AR_S32)pipe_ctx->niossysctl.gNmin_first) + 1000;
    Dvtemp0 = ((100 * g17_Vtemp_V - 100 * 1900) / 1000 + 30) * 100;

    ar_always("dvtemp_d5[0x%x] dvtemp_d6[0x%x] dvtemp[%u] gNmin_first[%u] gNmiddle_first[%u] g17_Vtemp_V[%d] Dvtemp0[%d]",
        dvtemp_d5, dvtemp_d6, dvtemp, pipe_ctx->niossysctl.gNmin_first,
        pipe_ctx->niossysctl.gNmiddle_first, g17_Vtemp_V, Dvtemp0);

    *ps32Temperature = Dvtemp0;
#endif
    return 0;
}

static AR_S32 gst612c_start_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst612c_pipe_ctx_t *pstPipeCtx = &g_gst612c_pipe_ctx[ViPipe];
    AR_U8 *pIrSnsParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    ISP_IR_PARAMS_S stIrParams;
    ISP_IR_OCC_PARAM_S stIrOccParam;
    //AR_U8 au8InitRaAdj[5] = {0, 0, 0, 0, 0};
    //AR_U8 u8InitHssd = 0x40;
    //AR_U8 u8Val = 0;

    ParamInit(ViPipe);

    RaselHssdRegGet(ViPipe, &pIrSnsParam[0], &pIrSnsParam[1]);
    //pIrSnsParam[0] = 0x0F;
    //pIrSnsParam[1] = 0x40;
    ar_always("pram init and new reg vlue is 0x%02x 0x%02x", pIrSnsParam[0], pIrSnsParam[1]);

    stIrOccParam.bIsSingleVal = AR_TRUE;
    stIrOccParam.u32SingleVal = 0x1F;

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
    stIrParams.astParamEntry[0].pParamData = &stIrOccParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(ISP_IR_OCC_PARAM_S);

    s32Ret = AR_MPI_ISP_SetIrParams(ViPipe, &stIrParams);
    if (s32Ret) {
        ar_err("pipe[%d] set ooc failed", ViPipe);
        goto FAIL;
    }

    pstPipeCtx->enCalibState = GST612C_CALIB_RA_ADJ;
    pstPipeCtx->s32RaAdjCalibFrmIdx = -1;
    pstPipeCtx->s32HssdCalibFrmIdx = -1;
    pstPipeCtx->u16MinMeanDelta = 0xffffUL;
	pstPipeCtx->au8FinalRasel = pIrSnsParam[0];
    pstPipeCtx->u8FinalHssd = pIrSnsParam[1];

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;

FAIL:
    pstPipeCtx->enCalibState = GST612C_CALIB_NONE;
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 gst612c_ra_adj_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    gst612c_pipe_ctx_t *pstPipeCtx = &g_gst612c_pipe_ctx[ViPipe];
    AR_U8 *pIrSnsParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U32 u32CalibTarget = (pstCalibCfg->u32TargetLowerLimit + pstCalibCfg->u32TargetUpperLimit) / 2;
    AR_U16 u16Delta = 0;
    AR_BOOL bCalibDone = AR_FALSE;
    //AR_U8 u8Val = 0;

    pstPipeCtx->s32RaAdjCalibFrmIdx++;

    ar_always("ra_adj calib: frm[%d] ra_adj[0x%02x] mean[%u]",
        pstPipeCtx->s32RaAdjCalibFrmIdx, pIrSnsParam[0], pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean <= u32CalibTarget) {
        u16Delta = u32CalibTarget - pstCalibCtx->stInput.u32PixMean;
    } else {
        u16Delta = pstCalibCtx->stInput.u32PixMean - u32CalibTarget;
    }

    if (u16Delta < pstPipeCtx->u16MinMeanDelta) {
        pstPipeCtx->u16MinMeanDelta = u16Delta;
		pstPipeCtx->au8FinalRasel = pIrSnsParam[0];
    }

    if (pstPipeCtx->s32RaAdjCalibFrmIdx < 9) {
		int raSel = pstPipeCtx->s32RaAdjCalibFrmIdx;
		pIrSnsParam[0] = 0xff >> (8 - raSel);
    } else {
        bCalibDone = AR_TRUE;
        pIrSnsParam[0] = pstPipeCtx->au8FinalRasel;
    }

	RaselRegSet(ViPipe, pIrSnsParam[0]);

    if (bCalibDone) {
        ar_always("ra_adj calib done: final_ra_adj[0x%02x]", pIrSnsParam[0]);

		HssdSet(ViPipe, 64);

        pIrSnsParam[1] = 0;
        pstPipeCtx->u16MinMeanDelta = 0xffffUL;
        pstPipeCtx->enCalibState = GST612C_CALIB_HSSD;
    }

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;
}

static AR_S32 gst612c_hssd_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst612c_pipe_ctx_t *pstPipeCtx = &g_gst612c_pipe_ctx[ViPipe];
    AR_U8 *pIrSnsParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U32 u32CalibTarget = (pstCalibCfg->u32TargetLowerLimit + pstCalibCfg->u32TargetUpperLimit) / 2;
    AR_U16 u16Delta = 0;
    AR_BOOL bCalibDone = AR_FALSE;
    //AR_U8 u8Val = 0;

    pstPipeCtx->s32HssdCalibFrmIdx++;

    ar_always("hssd calib: frm[%d] hssd[0x%02x] mean[%u]",
        pstPipeCtx->s32HssdCalibFrmIdx, pIrSnsParam[1], pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean <= u32CalibTarget) {
        u16Delta = u32CalibTarget - pstCalibCtx->stInput.u32PixMean;
    } else {
        u16Delta = pstCalibCtx->stInput.u32PixMean - u32CalibTarget;
    }

    if (u16Delta < pstPipeCtx->u16MinMeanDelta) {
        pstPipeCtx->u16MinMeanDelta = u16Delta;
        pstPipeCtx->u8FinalHssd = pIrSnsParam[1];
    }

    if (pstPipeCtx->s32HssdCalibFrmIdx < 127) {
        pIrSnsParam[1]++;
    } else {
        bCalibDone = AR_TRUE;
        pIrSnsParam[1] = pstPipeCtx->u8FinalHssd;
    }

	HssdSet(ViPipe, pIrSnsParam[1]);

    if (bCalibDone) {
        ar_always("hssd calib done: final_hssd[0x%02x]", pIrSnsParam[1]);
        pstPipeCtx->enCalibState = GST612C_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
    } else {
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    }

    return 0;
}

static AR_S32 gst612c_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst612c_pipe_ctx_t *pstPipeCtx = &g_gst612c_pipe_ctx[ViPipe];

    if (pstCalibCtx->stInput.stCalibFrm.u32FrameId == 0) {
        s32Ret = gst612c_start_sns_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] start sensor calib failed", ViPipe);
        } else {
            ar_always("pipe[%d] sensor calib started", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST612C_CALIB_RA_ADJ) {
        s32Ret = gst612c_ra_adj_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] ra_adj calib failed", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST612C_CALIB_HSSD) {
        s32Ret = gst612c_hssd_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] hssd calib failed", ViPipe);
        }
    } else {
        s32Ret = -1;
        ar_err("pipe[%d] sensor calib failed", ViPipe);
    }

    return s32Ret;
}


#if 0
static AR_S32 gst612c_ooc_calib(VI_PIPE ViPipe, GST612C_CALIB_INFO_S *pCalibInfo)
{
    AR_S32 s32Ret = 0;
    gst612c_pipe_ctx_t *pstPipeCtx = &g_gst612c_pipe_ctx[ViPipe];
    AR_U16 *pu16Raw = NULL;
    AR_U32 u32PixStride = pCalibInfo->pstFrmInfo->stVFrame.u32Stride[0] / sizeof(AR_U16);
    AR_U8 *pu8FinalOoc = NULL;
    AR_U16 *pu16MinPixDelta = NULL;
    AR_U32 u32OocStride = pCalibInfo->pstIrOccParam->stOccBuf.u32Stride[0];
    AR_U16 u16PixDelta = 0;
    AR_U8 u8CurOocVal = 0;
    AR_BOOL bCalibDone = AR_FALSE;
    ISP_IR_PARAMS_S stIrParams;
    AR_U32 i = 0;
    AR_U32 j = 0;

    pstPipeCtx->s32OccCalibFrmIdx++;
    u8CurOocVal = (AR_U8)pstPipeCtx->s32OccCalibFrmIdx;

    pu16Raw = (AR_U16 *)pCalibInfo->pstFrmInfo->stVFrame.u64VirAddr[0];
    pu16Raw += (u32PixStride * pCalibInfo->stROI.s32Y + pCalibInfo->stROI.s32X);
    pu8FinalOoc = pstPipeCtx->pu8FinalOcc + u32OocStride * pCalibInfo->stROI.s32Y + pCalibInfo->stROI.s32X;
    pu16MinPixDelta = pstPipeCtx->pu16MinPixDelta;

    for (i = 0; i < pCalibInfo->stROI.u32Height; i++) {
        for (j = 0; j < pCalibInfo->stROI.u32Width; j++) {
            if (pu16Raw[j] <= GST612C_CALIB_TARGET) {
                u16PixDelta = GST612C_CALIB_TARGET - pu16Raw[j];
            } else {
                u16PixDelta = pu16Raw[j] - GST612C_CALIB_TARGET;
            }

            if (u16PixDelta < pu16MinPixDelta[j]) {
                pu16MinPixDelta[j] = u16PixDelta;
                pu8FinalOoc[j] = u8CurOocVal;
            }
        }

        pu16Raw += u32PixStride;
        pu8FinalOoc += u32OocStride;
        pu16MinPixDelta += pCalibInfo->stROI.u32Width;
    }

    printf("ooc calib: frm[%d]\n", pstPipeCtx->s32OccCalibFrmIdx);

    if (u8CurOocVal < 0x3E) {
        memset((AR_VOID *)pCalibInfo->pstIrOccParam->stOccBuf.u64VirAddr[0],
            u8CurOocVal + 1, u32OocStride * pCalibInfo->pstIrOccParam->stOccBuf.u32Height);
    } else {
        bCalibDone = AR_TRUE;
        memcpy((AR_VOID *)pCalibInfo->pstIrOccParam->stOccBuf.u64VirAddr[0],
            pstPipeCtx->pu8FinalOcc, u32OocStride * pCalibInfo->pstIrOccParam->stOccBuf.u32Height);
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(pCalibInfo->pstIrOccParam->stOccBuf.u64PhyAddr[0],
        (AR_VOID *)pCalibInfo->pstIrOccParam->stOccBuf.u64VirAddr[0],
        u32OocStride * pCalibInfo->pstIrOccParam->stOccBuf.u32Height);
    if (s32Ret) {
        ar_err("flush occ buf failed");
        goto FAIL;
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
    stIrParams.astParamEntry[0].pParamData = pCalibInfo->pstIrOccParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(ISP_IR_OCC_PARAM_S);

    s32Ret = AR_MPI_ISP_SetIrParams(ViPipe, &stIrParams);
    if (s32Ret) {
        ar_err("pipe[%d] set ooc failed", ViPipe);
        goto FAIL;
    }

    if (!bCalibDone) {
        pCalibInfo->enCalibResult = GST612C_CALIB_ONGOING;
    } else {
        pstPipeCtx->enCalibState = GST612C_CALIB_NONE;
        pCalibInfo->enCalibResult = GST612C_CALIB_DONE;
        printf("sensor calibration done");
    }

    return 0;

FAIL:
    pstPipeCtx->enCalibState = GST612C_CALIB_NONE;
    pCalibInfo->enCalibResult = GST612C_CALIB_FAIL;
    return s32Ret;
}
#endif

AR_S32 gst612c_switch_shutter(VI_PIPE ViPipe, AR_BOOL bOpen)
{
    AR_S32 s32Ret = 0;
    int gpNum_slp = ar_hal_gpio_name_to_num(GROUP_0, PORT_A, 17);
    int gpNum_in1 = ar_hal_gpio_name_to_num(GROUP_0, PORT_A, 19);
    int gpNum_in2 = ar_hal_gpio_name_to_num(GROUP_0, PORT_A, 20);

    ar_hal_gpio_export(gpNum_slp);
    ar_hal_gpio_export(gpNum_in1);
    ar_hal_gpio_export(gpNum_in2);
    ar_hal_gpio_set_dir(gpNum_slp, OUTPUT);
    ar_hal_gpio_set_dir(gpNum_in1, OUTPUT);
    ar_hal_gpio_set_dir(gpNum_in2, OUTPUT);

    ar_hal_gpio_set_value(gpNum_slp, 1);

    if (bOpen) {
        // shutter_gpio1 pull low
        s32Ret = ar_hal_gpio_set_value(gpNum_in1, 0);
        if (s32Ret) {
            ar_err("gpNum_in1 pull low failed");
            return s32Ret;
        }

        // shutter_gpio2 pull high
        s32Ret = ar_hal_gpio_set_value(gpNum_in2, 1);
        if (s32Ret) {
            ar_err("gpNum_in2 pull high failed");
            return s32Ret;
        }
    } else {
        // shutter_gpio1 pull low
        s32Ret = ar_hal_gpio_set_value(gpNum_in1, 1);
        if (s32Ret) {
            ar_err("gpNum_in1 pull high failed");
            return s32Ret;
        }

        // shutter_gpio2 pull high
        s32Ret = ar_hal_gpio_set_value(gpNum_in2, 0);
        if (s32Ret) {
            ar_err("gpNum_in2 pull low failed");
            return s32Ret;
        }
    }

    ar_delay(50);

    // shutter_gpio1 pull low
    s32Ret = ar_hal_gpio_set_value(gpNum_in1, 0);
    if (s32Ret) {
        ar_err("shutter_gpio1 pull low failed");
        return s32Ret;
    }

    // shutter_gpio2 pull low
    s32Ret = ar_hal_gpio_set_value(gpNum_in2, 0);
    if (s32Ret) {
        ar_err("shutter_gpio2 pull low failed");
        return s32Ret;
    }

    return 0;
}
static AR_S32 gst612c_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = gst612c_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = gst612c_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = gst612c_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = gst612c_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = gst612c_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = gst612c_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_ir_sns_attr = gst612c_get_ir_sns_attr;
    pstSensorExpFunc->pfn_cmos_set_ir_param = gst612c_set_ir_param;
    pstSensorExpFunc->pfn_cmos_get_ir_param = gst612c_get_ir_param;
    pstSensorExpFunc->pfn_cmos_get_sns_temperature = gst612c_get_sns_temperature;
    pstSensorExpFunc->pfn_cmos_sns_calib = gst612c_sns_calib;
    pstSensorExpFunc->pfn_cmos_switch_shutter = gst612c_switch_shutter;

    return AR_SUCCESS;
}

#if 0
static AR_S32 gst612c_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    pstAeSnsDft->f32Fps = 25;
	pstAeSnsDft->f32MaxFps = 25;
    pstAeSnsDft->u32FullLinesStd = 512;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = 512;
	pstAeSnsDft->u32Hmax=500;

    return AR_SUCCESS;
}
#endif

static AR_S32 gst612c_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    //pstExpFuncs->pfn_cmos_get_ae_default = gst612c_get_ae_default;
    return AR_SUCCESS;
}

static AR_S32 gst612c_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 gst612c_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    AR_S32 ret = 0;
    gst612c_pipe_ctx_t *pipe_ctx = &g_gst612c_pipe_ctx[ViPipe];

    pipe_ctx->s8I2cDevId = unSNSBusInfo.s8I2cDev;

    if (pipe_ctx->s32I2cFd >= 0) {
        close(pipe_ctx->s32I2cFd);
        pipe_ctx->s32I2cFd = -1;
    }

    pipe_ctx->s32I2cFd = ar_hal_i2c_open(pipe_ctx->s8I2cDevId);
    if (pipe_ctx->s32I2cFd < 0) {
        ar_err("pipe[%d] open i2c-%u failed", ViPipe, pipe_ctx->s8I2cDevId);
        ret = -1;
    }

    return ret;
}

static AR_S32 gst612c_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = gst612c_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ar_err("init ctx failed, pipe[%d] ret[%d]", ViPipe, s32Ret);
        return s32Ret;
    }

    stSnsAttrInfo.eSensorId = GST612C_SNS_ID;
    s32Ret  = gst612c_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function failed!");
        return s32Ret;
    }

    s32Ret  = gst612c_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret  = gst612c_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to awb lib failed!");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 gst612c_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, GST612C_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, GST612C_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, GST612C_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to awb lib failed!");
        return s32Ret;
    }

    gst612c_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}

static AR_S32 gst612c_read(VI_PIPE ViPipe, AR_S32 s32Addr)
{
    AR_U8 data = 0;
    gst612c_read_register(ViPipe, (AR_U8)s32Addr, &data);
    return (AR_S32)data;
}

static AR_S32 gst612c_write(VI_PIPE ViPipe, AR_S32 s32Addr, AR_S32 s32Data)
{
    return gst612c_write_register(ViPipe, (AR_U8)s32Addr, (AR_U8)s32Data);
}


ISP_SNS_OBJ_S stSnsGst612cObj = {
    .pfnRegisterCallback    = gst612c_register_callback,
    .pfnUnRegisterCallback  = gst612c_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = gst612c_write,
    .pfnReadReg             = gst612c_read,
    .pfnSetBusInfo          = gst612c_set_bus_info,
    .pfnSetInit             = AR_NULL,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __GST612C_CMOS_H_ */
