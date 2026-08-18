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


const unsigned char imx347_i2c_addr     =    0x34;        /* I2C Address of IMX347 */
const unsigned int  imx347_addr_byte    =    2;
const unsigned int  imx347_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastImx347[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunImx347BusInfo[];

int imx347_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunImx347BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	ISP_TRACE(AR_DBG_INFO, "u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx347_i2c_addr >> 1));
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

int imx347_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int imx347_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int imx347_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = imx347_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = imx347_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = imx347_data_byte;

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

    if (imx347_addr_byte == 2)
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

    if (imx347_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, imx347_addr_byte + imx347_data_byte);
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

void imx347_prog(VI_PIPE ViPipe, int *rom)
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
            imx347_write_register(ViPipe, addr, data);
        }
    }
}

void imx347_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void imx347_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define IMX347_MIRRO (0x304E)
#define IMX347_FLIP  (0x304F)

int imx347_flip_off_mirro_off(VI_PIPE ViPipe)
{
    imx347_write_register(ViPipe, IMX347_MIRRO, 0x00);

    //flip off
    imx347_write_register(ViPipe, IMX347_FLIP, 0x00);
    imx347_write_register(ViPipe, 0x3080, 0x01);
    imx347_write_register(ViPipe, 0x30AD, 0x02);
    imx347_write_register(ViPipe, 0x30B6, 0x00);
    imx347_write_register(ViPipe, 0x30B7, 0x00);
    imx347_write_register(ViPipe, 0x30D8, 0x44);
    imx347_write_register(ViPipe, 0x3116, 0x02);
    return 0;
}

int imx347_flip_on_mirro_off(VI_PIPE ViPipe)
{
    imx347_write_register(ViPipe, IMX347_MIRRO, 0x00);

    //flip on
    imx347_write_register(ViPipe, IMX347_FLIP, 0x01);
    imx347_write_register(ViPipe, 0x3080, 0xFF);
    imx347_write_register(ViPipe, 0x30AD, 0x7E);
    imx347_write_register(ViPipe, 0x30B6, 0xFF);
    imx347_write_register(ViPipe, 0x30B7, 0x01);
    imx347_write_register(ViPipe, 0x30D8, 0x45);
    imx347_write_register(ViPipe, 0x3116, 0x01);
    return 0;
}

int imx347_flip_off_mirro_on(VI_PIPE ViPipe)
{
    imx347_write_register(ViPipe, IMX347_MIRRO, 0x01);
    
    //flip off
    imx347_write_register(ViPipe, IMX347_FLIP, 0x00);
    imx347_write_register(ViPipe, 0x3080, 0x01);
    imx347_write_register(ViPipe, 0x30AD, 0x02);
    imx347_write_register(ViPipe, 0x30B6, 0x00);
    imx347_write_register(ViPipe, 0x30B7, 0x00);
    imx347_write_register(ViPipe, 0x30D8, 0x44);
    imx347_write_register(ViPipe, 0x3116, 0x02);
    return 0;
}

int imx347_flip_on_mirro_on(VI_PIPE ViPipe)
{
    imx347_write_register(ViPipe, IMX347_MIRRO, 0x01);
    
    //flip on
    imx347_write_register(ViPipe, IMX347_FLIP, 0x01);
    imx347_write_register(ViPipe, 0x3080, 0xFF);
    imx347_write_register(ViPipe, 0x30AD, 0x7E);
    imx347_write_register(ViPipe, 0x30B6, 0xFF);
    imx347_write_register(ViPipe, 0x30B7, 0x01);
    imx347_write_register(ViPipe, 0x30D8, 0x45);
    imx347_write_register(ViPipe, 0x3116, 0x01);
    return 0;
}

#define IMX347_SENSOR_4M_30FPS_LINEAR_MODE      (1)
//#define IMX347_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
#define IMX347_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
#define IMX347_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)
#define IMX347_SENSOR_4M_30FPS_2t1_WDR_MODE     (5)

//void imx347_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
//void imx347_wdr_1080p60_2to1_init(VI_PIPE ViPipe);
//void imx347_wdr_1080p120_2to1_init(VI_PIPE ViPipe);
//void imx347_wdr_720p60_2to1_init(VI_PIPE ViPipe);
//void imx347_wdr_1080p30_3to1_init(VI_PIPE ViPipe);
//void imx347_wdr_1080p120_3to1_init(VI_PIPE ViPipe);
//void imx347_wdr_720p60_3to1_init(VI_PIPE ViPipe);
void imx347_linear_4mp30_init(VI_PIPE ViPipe);
void imx347_wdr_4m_2to1_init(VI_PIPE ViPipe);


void imx347_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastImx347[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            imx347_write_register(ViPipe, g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void imx347_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastImx347[ViPipe]->bInit;
    enWDRMode   = g_pastImx347[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastImx347[ViPipe]->u8ImgMode;

    imx347_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (IMX347_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX347_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                //imx347_wdr_1080p60_2to1_init(ViPipe);
            }
            else if (IMX347_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX347_SENSOR_720P_30FPS_2t1_WDR_MODE */
            {
                //imx347_wdr_720p60_2to1_init(ViPipe);
            }
            else if (IMX347_SENSOR_4M_30FPS_2t1_WDR_MODE == u8ImgMode)
            {
                imx347_wdr_4m_2to1_init(ViPipe);
            }
        }
        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            //imx347_wdr_1080p120_3to1_init(ViPipe);
        }
        else
        {
            imx347_linear_4mp25_init(ViPipe);
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (IMX347_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX347_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                //imx347_wdr_1080p60_2to1_init(ViPipe);
            }
            //else if (IMX347_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX347_SENSOR_720P_30FPS_2t1_WDR_MODE */
            //{
            //    imx347_wdr_720p60_2to1_init(ViPipe);
            //}
            //else
            //{
            //}
        }

        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            //imx347_wdr_1080p120_3to1_init(ViPipe);
        }

        else
        {
            imx347_linear_4mp25_init(ViPipe);
        }
    }

    // for (i = 0; i < g_pastImx347[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     imx347_write_register(ViPipe, g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx347[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastImx347[ViPipe]->bInit = AR_TRUE;
    return ;
}

void imx347_exit(VI_PIPE ViPipe)
{
    imx347_i2c_exit(ViPipe);

    return;
}

//#define FPS_15
void imx347_wdr_4m_2to1_init(VI_PIPE ViPipe)
{
	/*
IMX347LQR All-pixel scan CSI-2_4lane 37.125MHz AD:12bit Output:12bit 1188Mbps Master Mode LCG Mode DOL HDR 2frame VC 25fps Integration Time LEF:16ms SEF:1.01ms Gain:6dB
Tool ver : Ver2.0
*/
	imx347_write_register(ViPipe, 0x300C, 0x5B);  // BCWAIT_TIME[7:0]
	imx347_write_register(ViPipe, 0x300D, 0x40);  // CPWAIT_TIME[7:0]
	imx347_write_register(ViPipe, 0x3030, 0xBC);  // VMAX[19:0]
	imx347_write_register(ViPipe, 0x3031, 0x07);  // 
	imx347_write_register(ViPipe, 0x3048, 0x01);  // WDMODE[0]
	imx347_write_register(ViPipe, 0x3049, 0x01);  // WDSEL[1:0]
	imx347_write_register(ViPipe, 0x304A, 0x04);  // WD_SET1[2:0]
	imx347_write_register(ViPipe, 0x304B, 0x04);  // WD_SET2[3:0]
	imx347_write_register(ViPipe, 0x304C, 0x13);  // OPB_SIZE_V[5:0]
	imx347_write_register(ViPipe, 0x3056, 0xF2);  // Y_OUT_SIZE[12:0]
	imx347_write_register(ViPipe, 0x3057, 0x05);  // 
	imx347_write_register(ViPipe, 0x3058, 0x48);  // SHR0[19:0]
	imx347_write_register(ViPipe, 0x3059, 0x09);  // 
	imx347_write_register(ViPipe, 0x3068, 0x6D);  // RHS1[19:0]
	imx347_write_register(ViPipe, 0x30BE, 0x5E);  // -
	imx347_write_register(ViPipe, 0x30E8, 0x14);  // GAIN[10:0]
	imx347_write_register(ViPipe, 0x3110, 0x02);  // -
	imx347_write_register(ViPipe, 0x315A, 0x02);  // INCKSEL2[1:0]
	imx347_write_register(ViPipe, 0x316A, 0x7E);  // INCKSEL4[1:0]
	imx347_write_register(ViPipe, 0x31A1, 0x00);  // XVS_DRV[1:0]
	imx347_write_register(ViPipe, 0x31D7, 0x01);  // XVSMSKCNT_INT[1:0]
	imx347_write_register(ViPipe, 0x3202, 0x02);  // -
	imx347_write_register(ViPipe, 0x3288, 0x22);  // -
	imx347_write_register(ViPipe, 0x328A, 0x02);  // -
	imx347_write_register(ViPipe, 0x328C, 0xA2);  // -
	imx347_write_register(ViPipe, 0x328E, 0x22);  // -
	imx347_write_register(ViPipe, 0x3415, 0x27);  // -
	imx347_write_register(ViPipe, 0x3418, 0x27);  // -
	imx347_write_register(ViPipe, 0x3428, 0xFE);  // -
	imx347_write_register(ViPipe, 0x349E, 0x6A);  // -
	imx347_write_register(ViPipe, 0x34A2, 0x9A);  // -
	imx347_write_register(ViPipe, 0x34A4, 0x8A);  // -
	imx347_write_register(ViPipe, 0x34A6, 0x8E);  // -
	imx347_write_register(ViPipe, 0x34AA, 0xD8);  // -
	imx347_write_register(ViPipe, 0x3648, 0x01);  // -
	imx347_write_register(ViPipe, 0x3678, 0x01);  // -
	imx347_write_register(ViPipe, 0x367C, 0x69);  // -
	imx347_write_register(ViPipe, 0x367E, 0x69);  // -
	imx347_write_register(ViPipe, 0x3680, 0x69);  // -
	imx347_write_register(ViPipe, 0x3682, 0x69);  // -
	imx347_write_register(ViPipe, 0x371D, 0x05);  // -
	imx347_write_register(ViPipe, 0x375D, 0x11);  // -
	imx347_write_register(ViPipe, 0x375E, 0x43);  // -
	imx347_write_register(ViPipe, 0x375F, 0x76);  // -
	imx347_write_register(ViPipe, 0x3760, 0x07);  // -
	imx347_write_register(ViPipe, 0x3768, 0x1B);  // -
	imx347_write_register(ViPipe, 0x3769, 0x1B);  // -
	imx347_write_register(ViPipe, 0x376A, 0x1A);  // -
	imx347_write_register(ViPipe, 0x376B, 0x19);  // -
	imx347_write_register(ViPipe, 0x376C, 0x17);  // -
	imx347_write_register(ViPipe, 0x376D, 0x0F);  // -
	imx347_write_register(ViPipe, 0x376E, 0x0B);  // -
	imx347_write_register(ViPipe, 0x376F, 0x0B);  // -
	imx347_write_register(ViPipe, 0x3770, 0x0B);  // -
	imx347_write_register(ViPipe, 0x3776, 0x89);  // -
	imx347_write_register(ViPipe, 0x3777, 0x00);  // -
	imx347_write_register(ViPipe, 0x3778, 0xCA);  // -
	imx347_write_register(ViPipe, 0x3779, 0x00);  // -
	imx347_write_register(ViPipe, 0x377A, 0x45);  // -
	imx347_write_register(ViPipe, 0x377B, 0x01);  // -
	imx347_write_register(ViPipe, 0x377C, 0x56);  // -
	imx347_write_register(ViPipe, 0x377D, 0x02);  // -
	imx347_write_register(ViPipe, 0x377E, 0xFE);  // -
	imx347_write_register(ViPipe, 0x377F, 0x03);  // -
	imx347_write_register(ViPipe, 0x3780, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3781, 0x05);  // -
	imx347_write_register(ViPipe, 0x3782, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3783, 0x06);  // -
	imx347_write_register(ViPipe, 0x3784, 0x7F);  // -
	imx347_write_register(ViPipe, 0x3788, 0x1F);  // -
	imx347_write_register(ViPipe, 0x378A, 0xCA);  // -
	imx347_write_register(ViPipe, 0x378B, 0x00);  // -
	imx347_write_register(ViPipe, 0x378C, 0x45);  // -
	imx347_write_register(ViPipe, 0x378D, 0x01);  // -
	imx347_write_register(ViPipe, 0x378E, 0x56);  // -
	imx347_write_register(ViPipe, 0x378F, 0x02);  // -
	imx347_write_register(ViPipe, 0x3790, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3791, 0x03);  // -
	imx347_write_register(ViPipe, 0x3792, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3793, 0x05);  // -
	imx347_write_register(ViPipe, 0x3794, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3795, 0x06);  // -
	imx347_write_register(ViPipe, 0x3796, 0x7F);  // -
	imx347_write_register(ViPipe, 0x3798, 0xBF);  // -
    imx347_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx347_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //imx347_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ISP_TRACE(AR_DBG_INFO, "===IMX347 4M 30fps 12bit wdr Init OK!===\n");
    return;
}




/* 4MP30 */
void imx347_linear_4mp25_init(VI_PIPE ViPipe)
{
    /*
	IMX347LQR All-pixel scan CSI-2_4lane 37.125MHz AD:12bit Output:12bit 1188Mbps Master Mode LCG Mode 25fps Integration Time 10ms Gain:6dB
	Tool ver : Ver2.0
	*/
	imx347_write_register(ViPipe, 0x300C, 0x5B);  // BCWAIT_TIME[7:0]
	imx347_write_register(ViPipe, 0x300D, 0x40);  // CPWAIT_TIME[7:0]
	imx347_write_register(ViPipe, 0x3030, 0x78);  // VMAX[19:0]
	imx347_write_register(ViPipe, 0x3031, 0x0F);  // 
	imx347_write_register(ViPipe, 0x3058, 0x9A);  // SHR0[19:0]
	imx347_write_register(ViPipe, 0x3059, 0x0B);  // 
	imx347_write_register(ViPipe, 0x30BE, 0x5E);  // -
	imx347_write_register(ViPipe, 0x30E8, 0x14);  // GAIN[10:0]
	imx347_write_register(ViPipe, 0x3110, 0x02);  // -
	imx347_write_register(ViPipe, 0x315A, 0x02);  // INCKSEL2[1:0]
	imx347_write_register(ViPipe, 0x316A, 0x7E);  // INCKSEL4[1:0]
	imx347_write_register(ViPipe, 0x31A1, 0x00);  // XVS_DRV[1:0]
	imx347_write_register(ViPipe, 0x3202, 0x02);  // -
	imx347_write_register(ViPipe, 0x3288, 0x22);  // -
	imx347_write_register(ViPipe, 0x328A, 0x02);  // -
	imx347_write_register(ViPipe, 0x328C, 0xA2);  // -
	imx347_write_register(ViPipe, 0x328E, 0x22);  // -
	imx347_write_register(ViPipe, 0x3415, 0x27);  // -
	imx347_write_register(ViPipe, 0x3418, 0x27);  // -
	imx347_write_register(ViPipe, 0x3428, 0xFE);  // -
	imx347_write_register(ViPipe, 0x349E, 0x6A);  // -
	imx347_write_register(ViPipe, 0x34A2, 0x9A);  // -
	imx347_write_register(ViPipe, 0x34A4, 0x8A);  // -
	imx347_write_register(ViPipe, 0x34A6, 0x8E);  // -
	imx347_write_register(ViPipe, 0x34AA, 0xD8);  // -
	imx347_write_register(ViPipe, 0x3648, 0x01);  // -
	imx347_write_register(ViPipe, 0x3678, 0x01);  // -
	imx347_write_register(ViPipe, 0x367C, 0x69);  // -
	imx347_write_register(ViPipe, 0x367E, 0x69);  // -
	imx347_write_register(ViPipe, 0x3680, 0x69);  // -
	imx347_write_register(ViPipe, 0x3682, 0x69);  // -
	imx347_write_register(ViPipe, 0x371D, 0x05);  // -
	imx347_write_register(ViPipe, 0x375D, 0x11);  // -
	imx347_write_register(ViPipe, 0x375E, 0x43);  // -
	imx347_write_register(ViPipe, 0x375F, 0x76);  // -
	imx347_write_register(ViPipe, 0x3760, 0x07);  // -
	imx347_write_register(ViPipe, 0x3768, 0x1B);  // -
	imx347_write_register(ViPipe, 0x3769, 0x1B);  // -
	imx347_write_register(ViPipe, 0x376A, 0x1A);  // -
	imx347_write_register(ViPipe, 0x376B, 0x19);  // -
	imx347_write_register(ViPipe, 0x376C, 0x17);  // -
	imx347_write_register(ViPipe, 0x376D, 0x0F);  // -
	imx347_write_register(ViPipe, 0x376E, 0x0B);  // -
	imx347_write_register(ViPipe, 0x376F, 0x0B);  // -
	imx347_write_register(ViPipe, 0x3770, 0x0B);  // -
	imx347_write_register(ViPipe, 0x3776, 0x89);  // -
	imx347_write_register(ViPipe, 0x3777, 0x00);  // -
	imx347_write_register(ViPipe, 0x3778, 0xCA);  // -
	imx347_write_register(ViPipe, 0x3779, 0x00);  // -
	imx347_write_register(ViPipe, 0x377A, 0x45);  // -
	imx347_write_register(ViPipe, 0x377B, 0x01);  // -
	imx347_write_register(ViPipe, 0x377C, 0x56);  // -
	imx347_write_register(ViPipe, 0x377D, 0x02);  // -
	imx347_write_register(ViPipe, 0x377E, 0xFE);  // -
	imx347_write_register(ViPipe, 0x377F, 0x03);  // -
	imx347_write_register(ViPipe, 0x3780, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3781, 0x05);  // -
	imx347_write_register(ViPipe, 0x3782, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3783, 0x06);  // -
	imx347_write_register(ViPipe, 0x3784, 0x7F);  // -
	imx347_write_register(ViPipe, 0x3788, 0x1F);  // -
	imx347_write_register(ViPipe, 0x378A, 0xCA);  // -
	imx347_write_register(ViPipe, 0x378B, 0x00);  // -
	imx347_write_register(ViPipe, 0x378C, 0x45);  // -
	imx347_write_register(ViPipe, 0x378D, 0x01);  // -
	imx347_write_register(ViPipe, 0x378E, 0x56);  // -
	imx347_write_register(ViPipe, 0x378F, 0x02);  // -
	imx347_write_register(ViPipe, 0x3790, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3791, 0x03);  // -
	imx347_write_register(ViPipe, 0x3792, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3793, 0x05);  // -
	imx347_write_register(ViPipe, 0x3794, 0xFE);  // -
	imx347_write_register(ViPipe, 0x3795, 0x06);  // -
	imx347_write_register(ViPipe, 0x3796, 0x7F);  // -
	imx347_write_register(ViPipe, 0x3798, 0xBF);  // -
    
    imx347_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx347_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //imx347_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ISP_TRACE(AR_DBG_INFO, "===IMX347 4M 30fps 12bit LINE Init OK!===\n");
    return;
}
