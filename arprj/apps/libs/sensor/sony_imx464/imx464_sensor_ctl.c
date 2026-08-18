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


const unsigned char imx464_i2c_addr     =    0x34;        /* I2C Address of IMX464 */
const unsigned int  imx464_addr_byte    =    2;
const unsigned int  imx464_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastImx464[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunImx464BusInfo[];

int imx464_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunImx464BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	ISP_TRACE(AR_DBG_INFO, "u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx464_i2c_addr >> 1));
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

int imx464_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int imx464_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;
}


int imx464_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = imx464_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = imx464_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = imx464_data_byte;

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

    if (imx464_addr_byte == 2)
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

    if (imx464_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, imx464_addr_byte + imx464_data_byte);
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

void imx464_prog(VI_PIPE ViPipe, int *rom)
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
            imx464_write_register(ViPipe, addr, data);
        }
    }
}

void imx464_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void imx464_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define IMX464_MIRRO (0x304E)
#define IMX464_FLIP  (0x304F)

int imx464_flip_off_mirro_off(VI_PIPE ViPipe)
{
    imx464_write_register(ViPipe, IMX464_MIRRO, 0x00);

    //flip off
    imx464_write_register(ViPipe, IMX464_FLIP, 0x00);
    imx464_write_register(ViPipe, 0x3080, 0x01);
    imx464_write_register(ViPipe, 0x30AD, 0x02);
    imx464_write_register(ViPipe, 0x30B6, 0x00);
    imx464_write_register(ViPipe, 0x30B7, 0x00);
    imx464_write_register(ViPipe, 0x30D8, 0x44);
    imx464_write_register(ViPipe, 0x3116, 0x02);
    return 0;
}

int imx464_flip_on_mirro_off(VI_PIPE ViPipe)
{
    imx464_write_register(ViPipe, IMX464_MIRRO, 0x00);

    //flip on
    imx464_write_register(ViPipe, IMX464_FLIP, 0x01);
    imx464_write_register(ViPipe, 0x3080, 0xFF);
    imx464_write_register(ViPipe, 0x30AD, 0x7E);
    imx464_write_register(ViPipe, 0x30B6, 0xFF);
    imx464_write_register(ViPipe, 0x30B7, 0x01);
    imx464_write_register(ViPipe, 0x30D8, 0x45);
    imx464_write_register(ViPipe, 0x3116, 0x01);
    return 0;
}

int imx464_flip_off_mirro_on(VI_PIPE ViPipe)
{
    imx464_write_register(ViPipe, IMX464_MIRRO, 0x01);
    
    //flip off
    imx464_write_register(ViPipe, IMX464_FLIP, 0x00);
    imx464_write_register(ViPipe, 0x3080, 0x01);
    imx464_write_register(ViPipe, 0x30AD, 0x02);
    imx464_write_register(ViPipe, 0x30B6, 0x00);
    imx464_write_register(ViPipe, 0x30B7, 0x00);
    imx464_write_register(ViPipe, 0x30D8, 0x44);
    imx464_write_register(ViPipe, 0x3116, 0x02);
    return 0;
}

int imx464_flip_on_mirro_on(VI_PIPE ViPipe)
{
    imx464_write_register(ViPipe, IMX464_MIRRO, 0x01);
    
    //flip on
    imx464_write_register(ViPipe, IMX464_FLIP, 0x01);
    imx464_write_register(ViPipe, 0x3080, 0xFF);
    imx464_write_register(ViPipe, 0x30AD, 0x7E);
    imx464_write_register(ViPipe, 0x30B6, 0xFF);
    imx464_write_register(ViPipe, 0x30B7, 0x01);
    imx464_write_register(ViPipe, 0x30D8, 0x45);
    imx464_write_register(ViPipe, 0x3116, 0x01);
    return 0;
}

#define IMX464_SENSOR_4M_30FPS_LINEAR_MODE      (1)
//#define IMX464_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
#define IMX464_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
#define IMX464_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)
#define IMX464_SENSOR_4M_30FPS_2t1_WDR_MODE     (5)

//void imx464_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
//void imx464_wdr_1080p60_2to1_init(VI_PIPE ViPipe);
//void imx464_wdr_1080p120_2to1_init(VI_PIPE ViPipe);
//void imx464_wdr_720p60_2to1_init(VI_PIPE ViPipe);
//void imx464_wdr_1080p30_3to1_init(VI_PIPE ViPipe);
//void imx464_wdr_1080p120_3to1_init(VI_PIPE ViPipe);
//void imx464_wdr_720p60_3to1_init(VI_PIPE ViPipe);
void imx464_linear_4mp30_init(VI_PIPE ViPipe);
void imx464_wdr_4m_2to1_init(VI_PIPE ViPipe);


void imx464_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastImx464[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("set reg[0x%x]: 0x%x \n", g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            imx464_write_register(ViPipe, g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void imx464_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastImx464[ViPipe]->bInit;
    enWDRMode   = g_pastImx464[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastImx464[ViPipe]->u8ImgMode;

    imx464_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (IMX464_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX464_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                //imx464_wdr_1080p60_2to1_init(ViPipe);
            }
            else if (IMX464_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX464_SENSOR_720P_30FPS_2t1_WDR_MODE */
            {
                //imx464_wdr_720p60_2to1_init(ViPipe);
            }
            else if (IMX464_SENSOR_4M_30FPS_2t1_WDR_MODE == u8ImgMode)
            {
                imx464_wdr_4m_2to1_init(ViPipe);
            }
        }
        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            //imx464_wdr_1080p120_3to1_init(ViPipe);
        }
        else
        {
            imx464_linear_4mp30_init(ViPipe);
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (IMX464_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX464_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                //imx464_wdr_1080p60_2to1_init(ViPipe);
            }
            //else if (IMX464_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* IMX464_SENSOR_720P_30FPS_2t1_WDR_MODE */
            //{
            //    imx464_wdr_720p60_2to1_init(ViPipe);
            //}
            //else
            //{
            //}
        }

        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            //imx464_wdr_1080p120_3to1_init(ViPipe);
        }

        else
        {
            imx464_linear_4mp30_init(ViPipe);
        }
    }

    // for (i = 0; i < g_pastImx464[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     imx464_write_register(ViPipe, g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx464[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastImx464[ViPipe]->bInit = AR_TRUE;
    return ;
}

void imx464_exit(VI_PIPE ViPipe)
{
    imx464_i2c_exit(ViPipe);

    return;
}

//#define FPS_15
void imx464_wdr_4m_2to1_init(VI_PIPE ViPipe)
{

#ifdef FPS_15
    imx464_write_register(ViPipe, 0x300C, 0x5B);  // BCWAIT_TIME[7:0]
    imx464_write_register(ViPipe, 0x300D, 0x40);  // CPWAIT_TIME[7:0]
    imx464_write_register(ViPipe, 0x3034, 0xDC);  // HMAX[15:0]
    imx464_write_register(ViPipe, 0x3035, 0x05);  //
    imx464_write_register(ViPipe, 0x3048, 0x01);  // WDMODE[0]
    imx464_write_register(ViPipe, 0x3049, 0x01);  // WDSEL[1:0]
    imx464_write_register(ViPipe, 0x304A, 0x04);  // WD_SET1[2:0]
    imx464_write_register(ViPipe, 0x304B, 0x04);  // WD_SET2[3:0]
    imx464_write_register(ViPipe, 0x304C, 0x13);  // OPB_SIZE_V[5:0]
    imx464_write_register(ViPipe, 0x304D, 0x00);  // RAMPSHSWSEL
    imx464_write_register(ViPipe, 0x3058, 0xF4);  // SHR0[19:0]
    imx464_write_register(ViPipe, 0x3059, 0x0A);  //
    imx464_write_register(ViPipe, 0x3068, 0x3d);  // RHS1[19:0]:0x3d
    imx464_write_register(ViPipe, 0x3069, 0x00);
     imx464_write_register(ViPipe, 0x3200, 0x01);
    imx464_write_register(ViPipe, 0x30BE, 0x5E);  // -
    imx464_write_register(ViPipe, 0x314C, 0xC0);  // INCKSEL1[8:0]
    imx464_write_register(ViPipe, 0x315A, 0x06);  // INCKSEL2[1:0]
    imx464_write_register(ViPipe, 0x316A, 0x7E);  // INCKSEL4[1:0]
    imx464_write_register(ViPipe, 0x319E, 0x02);  // SYS_MODE[1:0]
    imx464_write_register(ViPipe, 0x31A1, 0x00);  // XVS_DRV[1:0]
    imx464_write_register(ViPipe, 0x31D7, 0x01);  // XVSMSKCNT_INT[1:0]
    imx464_write_register(ViPipe, 0x3288, 0x22);
    imx464_write_register(ViPipe, 0x328A, 0x02);
    imx464_write_register(ViPipe, 0x328C, 0xA2);
    imx464_write_register(ViPipe, 0x328E, 0x22);
    imx464_write_register(ViPipe, 0x3415, 0x27);
    imx464_write_register(ViPipe, 0x3418, 0x27);
    imx464_write_register(ViPipe, 0x3428, 0xFE);
    imx464_write_register(ViPipe, 0x349E, 0x6A);
    imx464_write_register(ViPipe, 0x34A2, 0x9A);
    imx464_write_register(ViPipe, 0x34A4, 0x8A);
    imx464_write_register(ViPipe, 0x34A6, 0x8E);
    imx464_write_register(ViPipe, 0x34AA, 0xD8);
    imx464_write_register(ViPipe, 0x35BC, 0x00);
    imx464_write_register(ViPipe, 0x35BE, 0xFF);
    imx464_write_register(ViPipe, 0x35CC, 0x1B);
    imx464_write_register(ViPipe, 0x35CD, 0x00);
    imx464_write_register(ViPipe, 0x35CE, 0x2A);
    imx464_write_register(ViPipe, 0x35CF, 0x00);
    imx464_write_register(ViPipe, 0x35DC, 0x07);
    imx464_write_register(ViPipe, 0x35DE, 0x1A);
    imx464_write_register(ViPipe, 0x35DF, 0x00);
    imx464_write_register(ViPipe, 0x35E4, 0x2B);
    imx464_write_register(ViPipe, 0x35E5, 0x00);
    imx464_write_register(ViPipe, 0x35E6, 0x07);
    imx464_write_register(ViPipe, 0x35E7, 0x01);
    imx464_write_register(ViPipe, 0x3648, 0x01);
    imx464_write_register(ViPipe, 0x3678, 0x01);
    imx464_write_register(ViPipe, 0x367C, 0x69);
    imx464_write_register(ViPipe, 0x367E, 0x69);
    imx464_write_register(ViPipe, 0x3680, 0x69);
    imx464_write_register(ViPipe, 0x3682, 0x69);
    imx464_write_register(ViPipe, 0x3718, 0x1C);
    imx464_write_register(ViPipe, 0x371D, 0x05);
    imx464_write_register(ViPipe, 0x375D, 0x11);
    imx464_write_register(ViPipe, 0x375E, 0x43);
    imx464_write_register(ViPipe, 0x375F, 0x76);
    imx464_write_register(ViPipe, 0x3760, 0x07);
    imx464_write_register(ViPipe, 0x3768, 0x1A);
    imx464_write_register(ViPipe, 0x3769, 0x1A);
    imx464_write_register(ViPipe, 0x376A, 0x19);
    imx464_write_register(ViPipe, 0x376B, 0x19);
    imx464_write_register(ViPipe, 0x376C, 0x17);
    imx464_write_register(ViPipe, 0x376D, 0x0F);
    imx464_write_register(ViPipe, 0x376E, 0x0B);
    imx464_write_register(ViPipe, 0x376F, 0x0B);
    imx464_write_register(ViPipe, 0x3770, 0x0B);
    imx464_write_register(ViPipe, 0x3776, 0x89);
    imx464_write_register(ViPipe, 0x3777, 0x00);
    imx464_write_register(ViPipe, 0x3778, 0xCA);
    imx464_write_register(ViPipe, 0x3779, 0x00);
    imx464_write_register(ViPipe, 0x377A, 0x45);
    imx464_write_register(ViPipe, 0x377B, 0x01);
    imx464_write_register(ViPipe, 0x377C, 0x56);
    imx464_write_register(ViPipe, 0x377D, 0x02);
    imx464_write_register(ViPipe, 0x377E, 0xFE);
    imx464_write_register(ViPipe, 0x377F, 0x03);
    imx464_write_register(ViPipe, 0x3780, 0xFE);
    imx464_write_register(ViPipe, 0x3781, 0x05);
    imx464_write_register(ViPipe, 0x3782, 0xFE);
    imx464_write_register(ViPipe, 0x3783, 0x06);
    imx464_write_register(ViPipe, 0x3784, 0x7F);
    imx464_write_register(ViPipe, 0x3788, 0x1F);
    imx464_write_register(ViPipe, 0x378A, 0xCA);
    imx464_write_register(ViPipe, 0x378B, 0x00);
    imx464_write_register(ViPipe, 0x378C, 0x45);
    imx464_write_register(ViPipe, 0x378D, 0x01);
    imx464_write_register(ViPipe, 0x378E, 0x56);
    imx464_write_register(ViPipe, 0x378F, 0x02);
    imx464_write_register(ViPipe, 0x3790, 0xFE);
    imx464_write_register(ViPipe, 0x3791, 0x03);
    imx464_write_register(ViPipe, 0x3792, 0xFE);
    imx464_write_register(ViPipe, 0x3793, 0x05);
    imx464_write_register(ViPipe, 0x3794, 0xFE);
    imx464_write_register(ViPipe, 0x3795, 0x06);
    imx464_write_register(ViPipe, 0x3796, 0x7F);
    imx464_write_register(ViPipe, 0x3798, 0xBF);
    imx464_write_register(ViPipe, 0x3A18, 0x7F);  // TCLKPOST[9:0]
    imx464_write_register(ViPipe, 0x3A1A, 0x37);  // TCLKPREPARE[9:0]
    imx464_write_register(ViPipe, 0x3A1C, 0x37);  // TCLKTRAIL[9:0]
    imx464_write_register(ViPipe, 0x3A1E, 0xF7);  // TCLKZERO[9:0]
    imx464_write_register(ViPipe, 0x3A1F, 0x00);  //
    imx464_write_register(ViPipe, 0x3A20, 0x3F);  // THSPREPARE[9:0]
    imx464_write_register(ViPipe, 0x3A22, 0x6F);  // THSZERO[9:0]
    imx464_write_register(ViPipe, 0x3A24, 0x3F);  // THSTRAIL[9:0]
    imx464_write_register(ViPipe, 0x3A26, 0x5F);  // THSEXIT[9:0]
    imx464_write_register(ViPipe, 0x3A28, 0x2F);  // TLPX[9:0]

#else //30fps
#if 1 //891 setting
    imx464_write_register(ViPipe, 0x300C, 0x5B);  // BCWAIT_TIME[7:0]
    imx464_write_register(ViPipe, 0x300D, 0x40);  // CPWAIT_TIME[7:0]
    imx464_write_register(ViPipe, 0x3019, 0x01);  // FDG_SEL
    imx464_write_register(ViPipe, 0x3048, 0x01);  // WDMODE[0]
    imx464_write_register(ViPipe, 0x3049, 0x01);  // WDSEL[1:0]
    imx464_write_register(ViPipe, 0x304A, 0x04);  // WD_SET1[2:0]
    imx464_write_register(ViPipe, 0x304B, 0x04);  // WD_SET2[3:0]
    imx464_write_register(ViPipe, 0x304C, 0x13);  // OPB_SIZE_V[5:0]
    imx464_write_register(ViPipe, 0x304D, 0x00);  // RAMPSHSWSEL
    imx464_write_register(ViPipe, 0x3058, 0x06);  // SHR0[19:0]
    imx464_write_register(ViPipe, 0x3059, 0x09);  //
    imx464_write_register(ViPipe, 0x3068, 0x6d);  // RHS1[19:0] //0x6d
    imx464_write_register(ViPipe, 0x3069, 0x00); //RHS1
    imx464_write_register(ViPipe, 0x30BE, 0x5E);  // -
    imx464_write_register(ViPipe, 0x30E8, 0x14);  // GAIN[10:0]
    imx464_write_register(ViPipe, 0x315A, 0x02);  // INCKSEL2[1:0]
    imx464_write_register(ViPipe, 0x316A, 0x7E);  // INCKSEL4[1:0]
    imx464_write_register(ViPipe, 0x31A1, 0x00);  // XVS_DRV[1:0]
    imx464_write_register(ViPipe, 0x31D7, 0x01);  // XVSMSKCNT_INT[1:0]
    imx464_write_register(ViPipe, 0x3288, 0x22);
    imx464_write_register(ViPipe, 0x328A, 0x02);
    imx464_write_register(ViPipe, 0x328C, 0xA2);
    imx464_write_register(ViPipe, 0x328E, 0x22);
    imx464_write_register(ViPipe, 0x3415, 0x27);
    imx464_write_register(ViPipe, 0x3418, 0x27);
    imx464_write_register(ViPipe, 0x3428, 0xFE);
    imx464_write_register(ViPipe, 0x349E, 0x6A);
    imx464_write_register(ViPipe, 0x34A2, 0x9A);
    imx464_write_register(ViPipe, 0x34A4, 0x8A);
    imx464_write_register(ViPipe, 0x34A6, 0x8E);
    imx464_write_register(ViPipe, 0x34AA, 0xD8);
    imx464_write_register(ViPipe, 0x35BC, 0x00);
    imx464_write_register(ViPipe, 0x35BE, 0xFF);
    imx464_write_register(ViPipe, 0x35CC, 0x1B);
    imx464_write_register(ViPipe, 0x35CD, 0x00);
    imx464_write_register(ViPipe, 0x35CE, 0x2A);
    imx464_write_register(ViPipe, 0x35CF, 0x00);
    imx464_write_register(ViPipe, 0x35DC, 0x07);
    imx464_write_register(ViPipe, 0x35DE, 0x1A);
    imx464_write_register(ViPipe, 0x35DF, 0x00);
    imx464_write_register(ViPipe, 0x35E4, 0x2B);
    imx464_write_register(ViPipe, 0x35E5, 0x00);
    imx464_write_register(ViPipe, 0x35E6, 0x07);
    imx464_write_register(ViPipe, 0x35E7, 0x01);
    imx464_write_register(ViPipe, 0x3648, 0x01);
    imx464_write_register(ViPipe, 0x3678, 0x01);
    imx464_write_register(ViPipe, 0x367C, 0x69);
    imx464_write_register(ViPipe, 0x367E, 0x69);
    imx464_write_register(ViPipe, 0x3680, 0x69);
    imx464_write_register(ViPipe, 0x3682, 0x69);
    imx464_write_register(ViPipe, 0x3718, 0x1C);
    imx464_write_register(ViPipe, 0x371D, 0x05);
    imx464_write_register(ViPipe, 0x375D, 0x11);
    imx464_write_register(ViPipe, 0x375E, 0x43);
    imx464_write_register(ViPipe, 0x375F, 0x76);
    imx464_write_register(ViPipe, 0x3760, 0x07);
    imx464_write_register(ViPipe, 0x3768, 0x1A);
    imx464_write_register(ViPipe, 0x3769, 0x1A);
    imx464_write_register(ViPipe, 0x376A, 0x19);
    imx464_write_register(ViPipe, 0x376B, 0x19);
    imx464_write_register(ViPipe, 0x376C, 0x17);
    imx464_write_register(ViPipe, 0x376D, 0x0F);
    imx464_write_register(ViPipe, 0x376E, 0x0B);
    imx464_write_register(ViPipe, 0x376F, 0x0B);
    imx464_write_register(ViPipe, 0x3770, 0x0B);
    imx464_write_register(ViPipe, 0x3776, 0x89);
    imx464_write_register(ViPipe, 0x3777, 0x00);
    imx464_write_register(ViPipe, 0x3778, 0xCA);
    imx464_write_register(ViPipe, 0x3779, 0x00);
    imx464_write_register(ViPipe, 0x377A, 0x45);
    imx464_write_register(ViPipe, 0x377B, 0x01);
    imx464_write_register(ViPipe, 0x377C, 0x56);
    imx464_write_register(ViPipe, 0x377D, 0x02);
    imx464_write_register(ViPipe, 0x377E, 0xFE);
    imx464_write_register(ViPipe, 0x377F, 0x03);
    imx464_write_register(ViPipe, 0x3780, 0xFE);
    imx464_write_register(ViPipe, 0x3781, 0x05);
    imx464_write_register(ViPipe, 0x3782, 0xFE);
    imx464_write_register(ViPipe, 0x3783, 0x06);
    imx464_write_register(ViPipe, 0x3784, 0x7F);
    imx464_write_register(ViPipe, 0x3788, 0x1F);
    imx464_write_register(ViPipe, 0x378A, 0xCA);
    imx464_write_register(ViPipe, 0x378B, 0x00);
    imx464_write_register(ViPipe, 0x378C, 0x45);
    imx464_write_register(ViPipe, 0x378D, 0x01);
    imx464_write_register(ViPipe, 0x378E, 0x56);
    imx464_write_register(ViPipe, 0x378F, 0x02);
    imx464_write_register(ViPipe, 0x3790, 0xFE);
    imx464_write_register(ViPipe, 0x3791, 0x03);
    imx464_write_register(ViPipe, 0x3792, 0xFE);
    imx464_write_register(ViPipe, 0x3793, 0x05);
    imx464_write_register(ViPipe, 0x3794, 0xFE);
    imx464_write_register(ViPipe, 0x3795, 0x06);
    imx464_write_register(ViPipe, 0x3796, 0x7F);
    imx464_write_register(ViPipe, 0x3798, 0xBF);
    imx464_write_register(ViPipe, 0x3200, 0x10);  // FDG_SEL
#else
      //1188mbps setting
    /*
    IMX464LQR All-pixel scan CSI-2_4lane 37.125MHz AD:12bit Output:12bit 1188Mbps Master Mode HCG Mode DOL HDR 2frame VC 30fps Integration Time LEF:10ms SEF:1.01ms Gain:6dB
    Ver5.0
    */
    imx464_write_register(ViPipe,0x300C,0x5B);  // WDMODE[0]
    imx464_write_register(ViPipe,0x300D,0x40);  // WDSEL[1:0]
    imx464_write_register(ViPipe,0x3019,0x01);  // WD_SET1[2:0]
    imx464_write_register(ViPipe,0x3048,0x01);  // WD_SET2[3:0]
    imx464_write_register(ViPipe,0x3049,0x01);  // OPB_SIZE_V[5:0]
    imx464_write_register(ViPipe,0x304A,0x04);  // RAMPSHSWSEL
    imx464_write_register(ViPipe,0x304B,0x04);  // SHR0[19:0]
    imx464_write_register(ViPipe,0x304C,0x13);  //
    imx464_write_register(ViPipe,0x304D,0x00);  // RHS1[19:0]
    imx464_write_register(ViPipe,0x3058,0x06);  // -
    imx464_write_register(ViPipe,0x3059,0x09);  // GAIN[10:0]
    imx464_write_register(ViPipe,0x3068,0x6D);  // INCKSEL2[1:0]
    imx464_write_register(ViPipe,0x30BE,0x5E);  // INCKSEL4[1:0]
    imx464_write_register(ViPipe,0x30E8,0x14);  // XVS_DRV[1:0]
    imx464_write_register(ViPipe,0x315A,0x02);  // XVSMSKCNT_INT[1:0]
    imx464_write_register(ViPipe,0x316A,0x7E);
    imx464_write_register(ViPipe,0x31A1,0x00);
    imx464_write_register(ViPipe,0x31D7,0x01);
    imx464_write_register(ViPipe,0x3288,0x22);
    imx464_write_register(ViPipe,0x328A,0x02);
    imx464_write_register(ViPipe,0x328C,0xA2);
    imx464_write_register(ViPipe,0x328E,0x22);
    imx464_write_register(ViPipe,0x3415,0x27);
    imx464_write_register(ViPipe,0x3418,0x27);
    imx464_write_register(ViPipe,0x3428,0xFE);
    imx464_write_register(ViPipe,0x349E,0x6A);
    imx464_write_register(ViPipe,0x34A2,0x9A);
    imx464_write_register(ViPipe,0x34A4,0x8A);
    imx464_write_register(ViPipe,0x34A6,0x8E);
    imx464_write_register(ViPipe,0x34AA,0xD8);
    imx464_write_register(ViPipe,0x35BC,0x00);
    imx464_write_register(ViPipe,0x35BE,0xFF);
    imx464_write_register(ViPipe,0x35CC,0x1B);
    imx464_write_register(ViPipe,0x35CD,0x00);
    imx464_write_register(ViPipe,0x35CE,0x2A);
    imx464_write_register(ViPipe,0x35CF,0x00);
    imx464_write_register(ViPipe,0x35DC,0x07);
    imx464_write_register(ViPipe,0x35DE,0x1A);
    imx464_write_register(ViPipe,0x35DF,0x00);
    imx464_write_register(ViPipe,0x35E4,0x2B);
    imx464_write_register(ViPipe,0x35E5,0x00);
    imx464_write_register(ViPipe,0x35E6,0x07);
    imx464_write_register(ViPipe,0x35E7,0x01);
    imx464_write_register(ViPipe,0x3648,0x01);
    imx464_write_register(ViPipe,0x3678,0x01);
    imx464_write_register(ViPipe,0x367C,0x69);
    imx464_write_register(ViPipe,0x367E,0x69);
    imx464_write_register(ViPipe,0x3680,0x69);
    imx464_write_register(ViPipe,0x3682,0x69);
    imx464_write_register(ViPipe,0x3718,0x1C);
    imx464_write_register(ViPipe,0x371D,0x05);
    imx464_write_register(ViPipe,0x375D,0x11);
    imx464_write_register(ViPipe,0x375E,0x43);
    imx464_write_register(ViPipe,0x375F,0x76);
    imx464_write_register(ViPipe,0x3760,0x07);
    imx464_write_register(ViPipe,0x3768,0x1A);
    imx464_write_register(ViPipe,0x3769,0x1A);
    imx464_write_register(ViPipe,0x376A,0x19);
    imx464_write_register(ViPipe,0x376B,0x19);
    imx464_write_register(ViPipe,0x376C,0x17);
    imx464_write_register(ViPipe,0x376D,0x0F);
    imx464_write_register(ViPipe,0x376E,0x0B);
    imx464_write_register(ViPipe,0x376F,0x0B);
    imx464_write_register(ViPipe,0x3770,0x0B);
    imx464_write_register(ViPipe,0x3776,0x89);
    imx464_write_register(ViPipe,0x3777,0x00);
    imx464_write_register(ViPipe,0x3778,0xCA);
    imx464_write_register(ViPipe,0x3779,0x00);
    imx464_write_register(ViPipe,0x377A,0x45);
    imx464_write_register(ViPipe,0x377B,0x01);
    imx464_write_register(ViPipe,0x377C,0x56);
    imx464_write_register(ViPipe,0x377D,0x02);
    imx464_write_register(ViPipe,0x377E,0xFE);
    imx464_write_register(ViPipe,0x377F,0x03);
    imx464_write_register(ViPipe,0x3780,0xFE);
    imx464_write_register(ViPipe,0x3781,0x05);
    imx464_write_register(ViPipe,0x3782,0xFE);
    imx464_write_register(ViPipe,0x3783,0x06);
    imx464_write_register(ViPipe,0x3784,0x7F);
    imx464_write_register(ViPipe,0x3788,0x1F);
    imx464_write_register(ViPipe,0x378A,0xCA);
    imx464_write_register(ViPipe,0x378B,0x00);
    imx464_write_register(ViPipe,0x378C,0x45);
    imx464_write_register(ViPipe,0x378D,0x01);
    imx464_write_register(ViPipe,0x378E,0x56);
    imx464_write_register(ViPipe,0x378F,0x02);
    imx464_write_register(ViPipe,0x3790,0xFE);
    imx464_write_register(ViPipe,0x3791,0x03);
    imx464_write_register(ViPipe,0x3792,0xFE);
    imx464_write_register(ViPipe,0x3793,0x05);
    imx464_write_register(ViPipe,0x3794,0xFE);
    imx464_write_register(ViPipe,0x3795,0x06);
    imx464_write_register(ViPipe,0x3796,0x7F);
    imx464_write_register(ViPipe,0x3798,0xBF);
    imx464_write_register(ViPipe, 0x3200, 0x10);
    imx464_write_register(ViPipe, 0x3034, 0xee);
    imx464_write_register(ViPipe, 0x3035, 0x02);
#endif
#endif
    imx464_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx464_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //imx464_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ISP_TRACE(AR_DBG_INFO, "===IMX464 4M 30fps 12bit wdr Init OK!===\n");
    return;
}




/* 4MP30 */
void imx464_linear_4mp30_init(VI_PIPE ViPipe)
{
    /*
    IMX464LQR All-pixel scan CSI-2_4lane 37.125MHz AD:12bit Output:12bit 891Mbps Master Mode LCG Mode 30fps Integration Time 10ms Gain:6dB
    Ver3.0
    */

    imx464_write_register (ViPipe, 0x300C, 0x5B); // BCWAIT_TIME[7:0]
    imx464_write_register (ViPipe, 0x300D, 0x40); // CPWAIT_TIME[7:0]
    imx464_write_register (ViPipe, 0x3034, 0xDC); // HMAX[15:0]
    imx464_write_register (ViPipe, 0x3035, 0x05); //
    imx464_write_register (ViPipe, 0x3058, 0x83); // SHR0[19:0]
    imx464_write_register (ViPipe, 0x3059, 0x04); //
    imx464_write_register (ViPipe, 0x30BE, 0x5E); // -
    imx464_write_register (ViPipe, 0x30E8, 0x14); // GAIN[10:0]
    imx464_write_register (ViPipe, 0x3110, 0x02); // -
    imx464_write_register (ViPipe, 0x314C, 0xC0); // INCKSEL1[8:0]
    imx464_write_register (ViPipe, 0x315A, 0x06); // INCKSEL2[1:0]
    imx464_write_register (ViPipe, 0x316A, 0x7E); // INCKSEL4[1:0]
    imx464_write_register (ViPipe, 0x319E, 0x02); // SYS_MODE[1:0]
    imx464_write_register (ViPipe, 0x31A1, 0x00); // XVS_DRV[1:0]
    imx464_write_register (ViPipe, 0x3288, 0x22);
    imx464_write_register (ViPipe, 0x328A, 0x02);
    imx464_write_register (ViPipe, 0x328C, 0xA2);
    imx464_write_register (ViPipe, 0x328E, 0x22);
    imx464_write_register (ViPipe, 0x3415, 0x27);
    imx464_write_register (ViPipe, 0x3418, 0x27);
    imx464_write_register (ViPipe, 0x3428, 0xFE);
    imx464_write_register (ViPipe, 0x349E, 0x6A);
    imx464_write_register (ViPipe, 0x34A2, 0x9A);
    imx464_write_register (ViPipe, 0x34A4, 0x8A);
    imx464_write_register (ViPipe, 0x34A6, 0x8E);
    imx464_write_register (ViPipe, 0x34AA, 0xD8);
    imx464_write_register (ViPipe, 0x35BC, 0x00);
    imx464_write_register (ViPipe, 0x35BE, 0xFF);
    imx464_write_register (ViPipe, 0x35CC, 0x1B);
    imx464_write_register (ViPipe, 0x35CD, 0x00);
    imx464_write_register (ViPipe, 0x35CE, 0x2A);
    imx464_write_register (ViPipe, 0x35CF, 0x00);
    imx464_write_register (ViPipe, 0x35DC, 0x07);
    imx464_write_register (ViPipe, 0x35DE, 0x1A);
    imx464_write_register (ViPipe, 0x35DF, 0x00);
    imx464_write_register (ViPipe, 0x35E4, 0x2B);
    imx464_write_register (ViPipe, 0x35E5, 0x00);
    imx464_write_register (ViPipe, 0x35E6, 0x07);
    imx464_write_register (ViPipe, 0x35E7, 0x01);
    imx464_write_register (ViPipe, 0x3648, 0x01);
    imx464_write_register (ViPipe, 0x3678, 0x01);
    imx464_write_register (ViPipe, 0x367C, 0x69);
    imx464_write_register (ViPipe, 0x367E, 0x69);
    imx464_write_register (ViPipe, 0x3680, 0x69);
    imx464_write_register (ViPipe, 0x3682, 0x69);
    imx464_write_register (ViPipe, 0x3718, 0x1C);
    imx464_write_register (ViPipe, 0x371D, 0x05);
    imx464_write_register (ViPipe, 0x375D, 0x11);
    imx464_write_register (ViPipe, 0x375E, 0x43);
    imx464_write_register (ViPipe, 0x375F, 0x76);
    imx464_write_register (ViPipe, 0x3760, 0x07);
    imx464_write_register (ViPipe, 0x3768, 0x1B);
    imx464_write_register (ViPipe, 0x3769, 0x1B);
    imx464_write_register (ViPipe, 0x376A, 0x1A);
    imx464_write_register (ViPipe, 0x376B, 0x19);
    imx464_write_register (ViPipe, 0x376C, 0x17);
    imx464_write_register (ViPipe, 0x376D, 0x0F);
    imx464_write_register (ViPipe, 0x376E, 0x0B);
    imx464_write_register (ViPipe, 0x376F, 0x0B);
    imx464_write_register (ViPipe, 0x3770, 0x0B);
    imx464_write_register (ViPipe, 0x3776, 0x89);
    imx464_write_register (ViPipe, 0x3777, 0x00);
    imx464_write_register (ViPipe, 0x3778, 0xCA);
    imx464_write_register (ViPipe, 0x3779, 0x00);
    imx464_write_register (ViPipe, 0x377A, 0x45);
    imx464_write_register (ViPipe, 0x377B, 0x01);
    imx464_write_register (ViPipe, 0x377C, 0x56);
    imx464_write_register (ViPipe, 0x377D, 0x02);
    imx464_write_register (ViPipe, 0x377E, 0xFE);
    imx464_write_register (ViPipe, 0x377F, 0x03);
    imx464_write_register (ViPipe, 0x3780, 0xFE);
    imx464_write_register (ViPipe, 0x3781, 0x05);
    imx464_write_register (ViPipe, 0x3782, 0xFE);
    imx464_write_register (ViPipe, 0x3783, 0x06);
    imx464_write_register (ViPipe, 0x3784, 0x7F);
    imx464_write_register (ViPipe, 0x3788, 0x1F);
    imx464_write_register (ViPipe, 0x378A, 0xCA);
    imx464_write_register (ViPipe, 0x378B, 0x00);
    imx464_write_register (ViPipe, 0x378C, 0x45);
    imx464_write_register (ViPipe, 0x378D, 0x01);
    imx464_write_register (ViPipe, 0x378E, 0x56);
    imx464_write_register (ViPipe, 0x378F, 0x02);
    imx464_write_register (ViPipe, 0x3790, 0xFE);
    imx464_write_register (ViPipe, 0x3791, 0x03);
    imx464_write_register (ViPipe, 0x3792, 0xFE);
    imx464_write_register (ViPipe, 0x3793, 0x05);
    imx464_write_register (ViPipe, 0x3794, 0xFE);
    imx464_write_register (ViPipe, 0x3795, 0x06);
    imx464_write_register (ViPipe, 0x3796, 0x7F);
    imx464_write_register (ViPipe, 0x3798, 0xBF);
    imx464_write_register (ViPipe, 0x3A18, 0x7F); // TCLKPOST[9:0]
    imx464_write_register (ViPipe, 0x3A1A, 0x37); // TCLKPREPARE[9:0]
    imx464_write_register (ViPipe, 0x3A1C, 0x37); // TCLKTRAIL[9:0]
    imx464_write_register (ViPipe, 0x3A1E, 0xF7); // TCLKZERO[9:0]
    imx464_write_register (ViPipe, 0x3A1F, 0x00);
    imx464_write_register (ViPipe, 0x3A20, 0x3F); // THSPREPARE[9:0]
    imx464_write_register (ViPipe, 0x3A22, 0x6F); // THSZERO[9:0]
    imx464_write_register (ViPipe, 0x3A24, 0x3F); // THSTRAIL[9:0]
    imx464_write_register (ViPipe, 0x3A26, 0x5F); // THSEXIT[9:0]
    imx464_write_register (ViPipe, 0x3A28, 0x2F); // TLPX[9:0]

    imx464_default_reg_init(ViPipe);

    imx464_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    imx464_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //imx464_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ISP_TRACE(AR_DBG_INFO, "===IMX464 4M 30fps 12bit LINE Init OK!===\n");
    return;
}
