#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "it_define.h"
#include "it_66021.h"
#include "config.h"
#include "it6602.h"
#include "it6602_reg.h"
#include "it66021_i2c.h"
#include "osal.h"
#include "ar_ite_hdmi_rx_app.h"
#include "ar_hdmi_rx_app.h"
#include "hal_vin_log.h"

static void IT_66021_I2CInitial(void);

extern volatile VTiming CurVTiming;
extern unsigned char HdmiI2cAddr;

ar_i2c_component ite_hdmirx_i2c_component;

void IT_Delay(uint32_t delay)
{
    ar_delay(delay);
}

struct CRT_TimingSetting_rx {
	BYTE fmt;
    WORD HActive;
    WORD VActive;
    WORD HTotal;
    WORD VTotal;
    WORD H_FBH;
    WORD H_SyncW;
    WORD H_BBH;
    WORD V_FBH;
    WORD V_SyncW;
    WORD V_BBH;
    BYTE Scan:1;
    BYTE VPolarity:1;
    BYTE HPolarity:1;
    int fps;
};

//   VDEE_L,   VDEE_H, VRS2S_L, VRS2S_H, VRS2E_L, VRS2E_H, HalfL_L, HalfL_H, VDE2S_L, VDE2S_H, HVP&Progress
static struct CRT_TimingSetting_rx TimingTable[] =
{
    //  VIC   H     V    HTotal VTotal  HFT   HSW     HBP VF VSW   VB
    {  1,  640,  480,    800,  525,   16,    96,    48, 10, 2,  33,      PROG, Vneg, Hneg, 60},// 640x480@60Hz         - CEA Mode [ 1]
    {  2,  720,  480,    858,  525,   16,    62,    60,  9, 6,  30,      PROG, Vneg, Hneg, 60},// 720x480@60Hz         - CEA Mode [ 2]
    {  3,  720,  480,    858,  525,   16,    62,    60,  9, 6,  30,      PROG, Vneg, Hneg, 60},// 720x480@60Hz         - CEA Mode [ 3]
    {  4, 1280,  720,   1650,  750,  110,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 60},// 1280x720@60Hz        - CEA Mode [ 4]
    {  5, 1920,  540,   2200,  562,   88,    44,   148,  2, 5,  15, INTERLACE, Vpos, Hpos, 60},// 1920x1080(I)@60Hz    - CEA Mode [ 5]
    {  6,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [ 6]
    {  7,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [ 7]
    {  8,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15,      PROG, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [ 8]
    {  9,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15,      PROG, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [ 9]
    { 10,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [10]
    { 11,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [11]
    { 12,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15,      PROG, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [12]
    { 13,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15,      PROG, Vneg, Hneg, 60},// 720x480(I)@60Hz      - CEA Mode [13]
    { 14, 1440,  480,   1716,  525,   32,   124,   120,  9, 6,  30,      PROG, Vneg, Hneg, 60},// 1440x480@60Hz        - CEA Mode [14]
    { 15, 1440,  480,   1716,  525,   32,   124,   120,  9, 6,  30,      PROG, Vneg, Hneg, 60},// 1440x480@60Hz        - CEA Mode [15]
    { 16, 1920, 1080,   2200, 1125,   88,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 60},// 1920x1080@60Hz       - CEA Mode [16]
    { 17,  720,  576,    864,  625,   12,    64,    68,  5, 5,  39,      PROG, Vneg, Hneg, 50},// 720x576@50Hz         - CEA Mode [17]
    { 18,  720,  576,    864,  625,   12,    64,    68,  5, 5,  39,      PROG, Vneg, Hneg, 50},// 720x576@50Hz         - CEA Mode [18]
    { 19, 1280,  720,   1980,  750,  440,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 50},// 1280x720@50Hz        - CEA Mode [19]
    { 20, 1920,  540,   2640,  562,  528,    44,   148,  2, 5,  15, INTERLACE, Vpos, Hpos, 50},// 1920x1080(I)@50Hz    - CEA Mode [20]
    { 21,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 50},// 1440x576(I)@50Hz     - CEA Mode [21]
    { 22,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 50},// 1440x576(I)@50Hz     - CEA Mode [22]
    { 23,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19,      PROG, Vneg, Hneg, 50},// 1440x288@50Hz        - CEA Mode [23]
    { 24,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19,      PROG, Vneg, Hneg, 50},// 1440x288@50Hz        - CEA Mode [24]
    { 25,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 50},// 1440x576(I)@50Hz     - CEA Mode [25]
    { 26,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 50},// 1440x576(I)@50Hz     - CEA Mode [26]
    { 27,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19,      PROG, Vneg, Hneg, 50},// 1440x288@50Hz        - CEA Mode [27]
    { 28,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19,      PROG, Vneg, Hneg, 50},// 1440x288@50Hz        - CEA Mode [28]
    { 29, 1440,  576,   1728,  625,   24,   128,   136,  5, 5,  39,      PROG, Vpos, Hneg, 50},// 1440x576@50Hz        - CEA Mode [29]
    { 30, 1440,  576,   1728,  625,   24,   128,   136,  5, 5,  39,      PROG, Vpos, Hneg, 50},// 1440x576@50Hz        - CEA Mode [30]
    { 31, 1920, 1080,   2640, 1125,  528,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 50},// 1920x1080@50Hz       - CEA Mode [31]
    { 32, 1920, 1080,   2750, 1125,  638,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 24},// 1920x1080@24Hz       - CEA Mode [32]
    { 33, 1920, 1080,   2640, 1125,  528,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 25},// 1920x1080@25Hz       - CEA Mode [33]
    { 34, 1920, 1080,   2200, 1125,   88,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 30},// 1920x1080@30Hz       - CEA Mode [34]
    { 35, 2880,  480, 1716*2,  525, 32*2, 124*2, 120*2,  9, 6,  30,      PROG, Vneg, Hneg, 60},// 2880x480@60Hz        - CEA Mode [35]
    { 36, 2880,  480, 1716*2,  525, 32*2, 124*2, 120*2,  9, 6,  30,      PROG, Vneg, Hneg, 60},// 2880x480@60Hz        - CEA Mode [36]
    { 37, 2880,  576,   3456,  625, 24*2, 128*2, 136*2,  5, 5,  39,      PROG, Vneg, Hneg, 50},// 2880x576@50Hz        - CEA Mode [37]
    { 38, 2880,  576,   3456,  625, 24*2, 128*2, 136*2,  5, 5,  39,      PROG, Vneg, Hneg, 50},// 2880x576@50Hz        - CEA Mode [38]
    { 39, 1920,  540,   2304,  625,   32,   168,   184, 23, 5,  57, INTERLACE, Vneg, Hpos, 50},// 1920x1080@50Hz       - CEA Mode [39]
    { 40, 1920,  540,   2640,  562,  528,    44,   148,  2, 5,  15, INTERLACE, Vpos, Hpos, 100},// 1920x1080(I)@100Hz   - CEA Mode [40]
    { 41, 1280,  720,   1980,  750,  440,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 100},// 1280x720@100Hz       - CEA Mode [41]
    { 42,  720,  576,    864,  625,   12,    64,    68,  5, 5,  39,      PROG, Vneg, Hneg, 100},// 720x576@100Hz        - CEA Mode [42]
    { 43,  720,  576,    864,  625,   12,    64,    68,  5, 5,  39,      PROG, Vneg, Hneg, 100},// 720x576@100Hz        - CEA Mode [43]
    { 44,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 100},// 1440x576(I)@100Hz    - CEA Mode [44]
    { 45,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 100},// 1440x576(I)@100Hz    - CEA Mode [45]
    { 46, 1920,  540,   2200,  562,   88,    44,   148,  2, 5,  15, INTERLACE, Vpos, Hpos, 120},// 1920x1080(I)@120Hz   - CEA Mode [46]
    { 47, 1280,  720,   1650,  750,  110,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 120},// 1280x720@120Hz       - CEA Mode [47]
    { 48,  720,  480,    858,  525,   16,    62,    60,  9, 6,  30,      PROG, Vneg, Hneg, 120},// 720x480@120Hz        - CEA Mode [48]
    { 49,  720,  480,    858,  525,   16,    62,    60,  9, 6,  30,      PROG, Vneg, Hneg, 120},// 720x480@120Hz        - CEA Mode [49]
    { 50,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 120},// 720x480(I)@120Hz     - CEA Mode [50]
    { 51,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 120},// 720x480(I)@120Hz     - CEA Mode [51]
    { 52,  720,  576,    864,  625,   12,    64,    68,  5, 5,  39,      PROG, Vneg, Hneg, 200},// 720x576@200Hz        - CEA Mode [52]
    { 53,  720,  576,    864,  625,   12,    64,    68,  5, 5,  39,      PROG, Vneg, Hneg, 200},// 720x576@200Hz        - CEA Mode [53]
    { 54,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 200},// 1440x576(I)@200Hz    - CEA Mode [54]
    { 55,  720,  288,    864,  312,   12,    63,    69,  2, 3,  19, INTERLACE, Vneg, Hneg, 200},// 1440x576(I)@200Hz    - CEA Mode [55]
    { 56,  720,  480,    858,  525,   16,    62,    60,  9, 6,  30,      PROG, Vneg, Hneg, 120},// 720x480@120Hz        - CEA Mode [56]
    { 57,  720,  480,    858,  525,   16,    62,    60,  9, 6,  30,      PROG, Vneg, Hneg, 120},// 720x480@120Hz        - CEA Mode [57]
    { 58,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 120},// 720x480(I)@120Hz     - CEA Mode [58]
    { 59,  720,  240,    858,  262,   19,    62,    57,  4, 3,  15, INTERLACE, Vneg, Hneg, 120},// 720x480(I)@120Hz     - CEA Mode [59]
    { 60, 1280,  720,   3300,  750, 1760,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 24},// 1280x720@24Hz        - CEA Mode [60]
    { 61, 1280,  720,   3960,  750, 2420,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 25},// 1280x720@25Hz        - CEA Mode [61]
    { 62, 1280,  720,   3300,  750, 1760,    40,   220,  5, 5,  20,      PROG, Vpos, Hpos, 30},// 1280x720@30Hz        - CEA Mode [62]
    { 63, 1920, 1080,   2200, 1125,   88,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 120},// 1920x1080@120Hz      - CEA Mode [63]
    { 64, 1920, 1080,   2640, 1125,  528,    44,   148,  4, 5,  36,      PROG, Vpos, Hpos, 100},// 1920x1080@100Hz      - CEA Mode [64]
};

#if defined (__linux__)
static uint32_t IT_66021_I2C_COMPONENT_NUM = 3;
static uint16_t it_66021_subaddr = 0x92;
static int it_66021_fd = -1;   //only the 1st I2c master be used
static unsigned char i2c_write_byte( BYTE address,BYTE offset,BYTE byteno,BYTE *p_data,BYTE device )
{
    if ((byteno > 0) && (byteno <10 )){
        uint8_t data[10] = {offset};
        memcpy(&data[1], p_data, byteno);
		if (ar_i2c_master_write(it_66021_fd, address>>1, 7, data, byteno+1) >= 0)
		{
			return 1;
		}
    }

	return 0;
}

static unsigned char i2c_read_byte( BYTE address,BYTE offset,BYTE byteno,BYTE *p_data,BYTE device )
{
    uint8_t sub_addr = offset;
    if(ar_i2c_master_read(it_66021_fd, address >> 1, 7, &sub_addr, 1, p_data, byteno) >= 0)
		return 1;
	else
		return 0;
}
int HDMIRX_i2c_init(ar_i2c_component component, uint16_t subaddr)
{
	int fd;

	IT_66021_I2C_COMPONENT_NUM = component;
	it_66021_subaddr = subaddr;

	if (it_66021_fd != -1)
		return -1;

	fd = ar_i2c_open(component);
	if (fd < 0)
	{
		printf("ar_i2c_open i2c_%d failed = %d\r\n", component, fd) ;
		return 0;
	}

	it_66021_fd = fd;
	return IT6602_Identify_Chip();
}


uint8_t IT_66021_WriteByte(uint8_t slv_addr, uint8_t sub_addr, uint8_t val)
{
    i2c_write_byte(it_66021_subaddr,sub_addr,1,&val,0);
	return 0;

}


uint8_t IT_66021_ReadByte(uint8_t slv_addr, uint8_t sub_addr)
{
    BYTE p_data;

    i2c_read_byte(it_66021_subaddr,sub_addr,1,&p_data,0);

    return p_data;
}

uint8_t IT_66021_WriteBytes(uint8_t slv_addr, uint8_t sub_addr, uint8_t byteno, uint8_t *p_data)
{
    BYTE I2C_buf[128];
    int     i;

    if(byteno>128)
    {
      ar_err("too larger bytes to write");
	  return 0;
    }
	
    for (i = 0; i < byteno; i++)
    {
        I2C_buf[i]=p_data[i];		
        i2c_write_byte(it_66021_subaddr, sub_addr++, 1, &I2C_buf[i], 0);
    }

	return 0;
}

uint8_t IT_66021_ReadBytes(uint8_t slv_addr, uint8_t sub_addr, uint8_t byteno, uint8_t *p_data)
{
    i2c_read_byte(it_66021_subaddr,sub_addr,byteno,p_data,0);
	return 0;
}
#else
int HDMIRX_i2c_init(ar_i2c_component component, uint16_t subaddr)
{
	ite_hdmirx_i2c_component = component;
	HdmiI2cAddr = subaddr;
	g_i2c.component = ite_hdmirx_i2c_component;
	g_i2c.speed = AR_I2C_STANDARD_SPEED;
	g_i2c.addr = (HdmiI2cAddr >> 1);
	g_i2c.flags = AR_I2C_SLAV_7BITS;
	ar_i2c_master_init(&g_i2c);
	return IT6602_Identify_Chip();
}


uint8_t IT_66021_WriteByte(uint8_t slv_addr, uint8_t sub_addr, uint8_t val)
{

	uint8_t wr_data[2] = {sub_addr, val};

	struct i2c_msg i2c_msg_write = {
		.buf = &wr_data[1],
		.buf_len = 1,
		.reg_addr = &wr_data[0],
		.reg_len = 1,
		.dev_addr = slv_addr >> 1,
	};

	ar_i2c_master_writebyte(&i2c_msg_write);
	return 0;

}


uint8_t IT_66021_ReadByte(uint8_t slv_addr, uint8_t sub_addr)
{
	uint8_t val[2], ret = 0;
	uint8_t addr[2] = {sub_addr, 0};
	struct i2c_msg i2c_msg_read = {
		.buf = val,
		.buf_len = 1,
		.reg_addr = addr,
		.reg_len = 1,
		.dev_addr = slv_addr >> 1,
	};
	ret = ar_i2c_master_readbyte(&i2c_msg_read);
	if (I2C_FALSE == ret){
		dbg_printf(PRN_BUFF, "read slv_addr=%x sub_addr=%x error\n",slv_addr, sub_addr);
	}
    return val[0];
}

uint8_t IT_66021_WriteBytes(uint8_t slv_addr, uint8_t sub_addr, uint8_t byteno, uint8_t *p_data)
{
	uint8_t wr_data[1024];
	memset(wr_data, 0, sizeof(wr_data));
	wr_data[0] = sub_addr;
	memcpy(&wr_data[1], p_data, byteno);

	struct i2c_msg i2c_msg_write = {
		.buf = wr_data,
		.buf_len = 1+byteno,
		.dev_addr = slv_addr >> 1,
	};

	return ar_i2c_master_writedata(&i2c_msg_write);
}

uint8_t IT_66021_ReadBytes(uint8_t slv_addr, uint8_t sub_addr, uint8_t byteno, uint8_t *p_data)
{
	uint8_t addr[2] = {sub_addr, 0};
	int8_t ret = 0;
	struct i2c_msg i2c_msg_read = {
		.buf = p_data,
		.buf_len = byteno,
		.reg_addr = addr,
		.reg_len = 1,
		.dev_addr = slv_addr >> 1,
	};

	ret = ar_i2c_master_readdata(&i2c_msg_read);
	if (I2C_FALSE == ret){
		dbg_printf(PRN_BUFF, "read slv_addr=%x sub_addr=%x error\n",slv_addr, sub_addr);
	}
	return ret;
}
#endif
void IT_66021_Set(unsigned char slv_addr, unsigned char sub_addr, unsigned char mask, unsigned char val)
{

    unsigned char tmpal = IT_66021_ReadByte(slv_addr, sub_addr);

	tmpal = ((tmpal&((~mask)&0xFF)))+(mask&val);
	IT_66021_WriteByte(slv_addr, sub_addr, tmpal);
}

int hdmi_rx_66021_get_avstate(void)
{

	struct it6602_dev_data *it6602data = get_it6602_dev_data();
	int state = HDMI_RX_UNSTABLE;

	if (VSTATE_VideoOn == it6602data->m_VState) {
		state = HDMI_RX_VIDEO_O;
    }

	if (ASTATE_AudioOn == it6602data->m_AState){
        state = HDMI_RX_AVSTABLE;
    }
	return state;
}

//static unsigned char interlace_vic[] = {5, 6, 7, 21, 22, 20};

int hdmi_rx_66021_get_vformat(struct video_format_info_t *vformat)
{
	struct it6602_dev_data *it6602data = get_it6602_dev_data();
	int ret = 0, i = 0, vic = 0xff, min=0xff, val = 0;
    if (VSTATE_VideoOn == it6602data->m_VState) {

        uint32_t u32_HTotal   = (((IT_66021_ReadByte(HdmiI2cAddr, 0x9D))&0x3F)<<8) + IT_66021_ReadByte(HdmiI2cAddr, 0x9C);
        uint32_t u32_HActive  = (((IT_66021_ReadByte(HdmiI2cAddr, 0x9F))&0x3F)<<8) + IT_66021_ReadByte(HdmiI2cAddr, 0x9E);

        uint32_t u32_VTotal   = (((IT_66021_ReadByte(HdmiI2cAddr, 0xA4))&0x0F)<<8) + IT_66021_ReadByte(HdmiI2cAddr, 0xA3);
        uint32_t u32_VActive  = (((IT_66021_ReadByte(HdmiI2cAddr, 0xA4))&0xF0)<<4) + IT_66021_ReadByte(HdmiI2cAddr, 0xA5);

        uint8_t u8_rddata = IT_66021_ReadByte(HdmiI2cAddr, 0x9A);
        uint32_t PCLK = (124*255/u8_rddata)/10;

        uint64_t u64_FrameRate = (uint64_t)(PCLK)*1000*1000;
        u64_FrameRate /= u32_HTotal;
        u64_FrameRate /= u32_VTotal;

		vformat->width = u32_HActive;
		vformat->height = u32_VActive;
		//vformat->fps_den = u32_HTotal*u32_VTotal;
        //vformat->is_interlace = 0;
        //vformat->fps_num = (PCLK)*1000*1000;

		switch (it6602data->m_bOutputVideoMode) {
			case F_MODE_RGB444:
				vformat->format = HDMI_RX_OUT_MODE_RGB444;
				break;
			case F_MODE_YUV422:
				vformat->format = HDMI_RX_OUT_MODE_YUV422;
				break;
			case F_MODE_YUV444:
				vformat->format = HDMI_RX_OUT_MODE_YUV444;
				break;
			default:
				ar_err("m_bOutputVideoMode err\n");
				break;
		}
		for (i = 0; i < ARRAY_COUNT_OF(TimingTable); i++){
			if (u32_HTotal == TimingTable[i].HTotal &&
				((u32_VTotal == TimingTable[i].VTotal) || (u32_VTotal == (TimingTable[i].VTotal+1)))) {

				if (u64_FrameRate > TimingTable[i].fps) {
					val = u64_FrameRate - TimingTable[i].fps;
				} else {
					val = TimingTable[i].fps- u64_FrameRate;
				}

				if (min > val) {
					vic = i;
					min = val;
				}
			}
		}
		if (0xff == vic) {
            vic = it6602data->VIC - 1;
		}
		vformat->fps_num = TimingTable[vic].fps;
        vformat->fps_den = 1;
        vformat->is_interlace = (TimingTable[vic].Scan==INTERLACE);
		ar_always("table index=%d, fps %u, u32_VTotal %d u32_HTotal %d.\n",vic, (uint32_t)u64_FrameRate, u32_VTotal, u32_HTotal);
#if 0
        if ( it6602data->VIC > 0 && it6602data->VIC < 65) {
            vformat->fps_num = TimingTable[it6602data->VIC-1].fps;
            vformat->fps_den = 1;
            vformat->is_interlace = (TimingTable[it6602data->VIC -1].Scan==INTERLACE);
        }
#endif
        ar_always("VIC %d, fps %u, is_interlace %u.\n",it6602data->VIC, vformat->fps_num, vformat->is_interlace);

        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

int hdmi_rx_66021_get_aformat(struct audio_format_info_t *aformat)
{
    struct it6602_dev_data *it6602data = get_it6602_dev_data();
    int ret = 0;
    if (ASTATE_AudioOn == it6602data->m_AState)
    {
		switch(it6602data->m_RxAudioCaps.SampleFreq) {
			case B_192K:
				aformat->rate = 192 *1000;
			break;
			case B_176P4K:
				aformat->rate = 1764 *100;
			break;
			case B_96K:
				aformat->rate = 96 *1000;
			break;
			case B_88P2K:
				aformat->rate = 882 *100;
			break;
			case B_48K:
				aformat->rate = 48 *1000;
			break;
			case B_44P1K:
				aformat->rate = 441 *100;
			break;
			case B_32K:
				aformat->rate = 32 *1000;
			break;
			default:
				aformat->rate = 48 *1000;
			break;
		}
		aformat->format = 16;
		aformat->channels = 2;
        ret = 1;
    } else {
		ret = 0;
    }
    return ret;
}


uint8_t IT_66021_app(void)
{
        IT6602_Interrupt();
        IT6602_fsm();
		return hdmi_rx_66021_get_avstate();
}

void HDMIRX_Initial(void)
{
    it66021_init();
}


