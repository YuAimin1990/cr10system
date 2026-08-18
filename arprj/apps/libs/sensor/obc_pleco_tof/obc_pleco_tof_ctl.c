#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ar_comm_video.h"
#include "../ar_sns_ctrl.h"

#include "obc_pleco_i2c_ctl.h"


#ifdef AR_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ar_i2c.h"
#endif

const unsigned char pleco_i2c_addr     =    0x20;        /* I2C Address of PLECO */
const unsigned int  pleco_addr_byte    =    2;
const unsigned int  pleco_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastPleco[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunPlecoBusInfo[];

int pleco_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunPlecoBusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

    ar_always("u8DevNum=%d %s",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (pleco_i2c_addr >> 1));
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

int pleco_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int pleco_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:

    return AR_SUCCESS;

}


int pleco_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = pleco_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = pleco_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = pleco_data_byte;

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

    if (pleco_addr_byte == 2)
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

    if (pleco_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, pleco_addr_byte + pleco_data_byte);
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

void pleco_prog(VI_PIPE ViPipe, int *rom)
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
            pleco_write_register(ViPipe, addr, data);
        }
    }
}

void pleco_standby(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

void pleco_restart(VI_PIPE ViPipe)
{
    // TODO:
    return;
}

#define PLECO_FLIP_MIRRO (0x3007)

int pleco_flip_off_mirro_off(VI_PIPE ViPipe)
{
    pleco_write_register(ViPipe, PLECO_FLIP_MIRRO, 0x00);
    return 0;
}

int pleco_flip_on_mirro_off(VI_PIPE ViPipe)
{
    pleco_write_register(ViPipe, PLECO_FLIP_MIRRO, 0x01);
    return 0;
}

int pleco_flip_off_mirro_on(VI_PIPE ViPipe)
{
    pleco_write_register(ViPipe, PLECO_FLIP_MIRRO, 0x02);
    return 0;
}

int pleco_flip_on_mirro_on(VI_PIPE ViPipe)
{
    pleco_write_register(ViPipe, PLECO_FLIP_MIRRO, 0x03);
    return 0;
}

#define PLECO_SENSOR_1080P_30FPS_LINEAR_MODE      (1)
#define PLECO_SENSOR_1080P_30FPS_3t1_WDR_MODE     (2)
#define PLECO_SENSOR_1080P_30FPS_2t1_WDR_MODE     (3)
#define PLECO_SENSOR_720P_30FPS_2t1_WDR_MODE      (4)
#define PLECO_SENSOR_1080P_60FPS_LINEAR_MODE      (5)
#define PLECO_SENSOR_DUAL_SHUFFLE_MASTER_1920x2892_MODE      (6)


void pleco_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
void pleco_wdr_1080p60_2to1_init(VI_PIPE ViPipe);
void pleco_wdr_1080p120_2to1_init(VI_PIPE ViPipe);
void pleco_wdr_720p60_2to1_init(VI_PIPE ViPipe);
void pleco_wdr_1080p30_3to1_init(VI_PIPE ViPipe);
void pleco_wdr_1080p120_3to1_init(VI_PIPE ViPipe);
void pleco_wdr_720p60_3to1_init(VI_PIPE ViPipe);
void pleco_linear_1080p30_init(VI_PIPE ViPipe);
void pleco_linear_1080p60_init(VI_PIPE ViPipe);
void pleco_dual_shuffle_master_1920x2892_init(VI_PIPE ViPipe);




void pleco_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastPleco[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //ar_always("set reg[0x%x]: 0x%x \n", g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //ar_always("set reg[0x%x]: 0x%x \n", g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            pleco_write_register(ViPipe, g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void pleco_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    AR_U8            u8ImgMode;

    bInit       = g_pastPleco[ViPipe]->bInit;
    enWDRMode   = g_pastPleco[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastPleco[ViPipe]->u8ImgMode;

    pleco_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (PLECO_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* PLECO_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                pleco_wdr_1080p60_2to1_init(ViPipe);
            }
            else if (PLECO_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* PLECO_SENSOR_720P_30FPS_2t1_WDR_MODE */
            {
                pleco_wdr_720p60_2to1_init(ViPipe);
            }
            else
            {
            }
        }
        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            pleco_wdr_1080p120_3to1_init(ViPipe);
        }
        else
        {
            if (PLECO_SENSOR_1080P_30FPS_LINEAR_MODE == u8ImgMode)
            {
                pleco_linear_1080p30_init(ViPipe);
            }
            else if (PLECO_SENSOR_1080P_60FPS_LINEAR_MODE == u8ImgMode) 
            {
                pleco_linear_1080p60_init(ViPipe);
            }
			else if (PLECO_SENSOR_DUAL_SHUFFLE_MASTER_1920x2892_MODE == u8ImgMode)
			{
				//ISP_TRACE(AR_DBG_ERR, "===pleco_init : pleco_dual_shuffle_master_1920x2892_init Begin!===\n");
				printf("===pleco_init : pleco_dual_shuffle_master_1920x2892_init Begin!===\n");
				pleco_dual_shuffle_master_1920x2892_init(ViPipe);
				//ISP_TRACE(AR_DBG_ERR, "===pleco_init : pleco_dual_shuffle_master_1920x2892_init OK!===\n");
				printf("===pleco_init : pleco_dual_shuffle_master_1920x2892_init End!===\n");
			}

            else
            {
                pleco_linear_1080p30_init(ViPipe);
            }
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {
        if (WDR_MODE_2To1_LINE == enWDRMode)
        {
            if (PLECO_SENSOR_1080P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* PLECO_SENSOR_1080P_30FPS_2t1_WDR_MODE */
            {
                pleco_wdr_1080p60_2to1_init(ViPipe);
            }
            else if (PLECO_SENSOR_720P_30FPS_2t1_WDR_MODE == u8ImgMode)    /* PLECO_SENSOR_720P_30FPS_2t1_WDR_MODE */
            {
                pleco_wdr_720p60_2to1_init(ViPipe);
            }
            else
            {
            }
        }

        else if (WDR_MODE_3To1_LINE == enWDRMode)
        {
            pleco_wdr_1080p120_3to1_init(ViPipe);
        }

        else
        {
            if (PLECO_SENSOR_1080P_30FPS_LINEAR_MODE == u8ImgMode)
            {
                pleco_linear_1080p30_init(ViPipe);
            }
            else if (PLECO_SENSOR_1080P_60FPS_LINEAR_MODE == u8ImgMode) 
            {
                pleco_linear_1080p60_init(ViPipe);
            }
            else
            {
                pleco_linear_1080p30_init(ViPipe);
            }
        }
    }

    // for (i = 0; i < g_pastPleco[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     pleco_write_register(ViPipe, g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastPleco[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastPleco[ViPipe]->bInit = AR_TRUE;
    return ;
}

void pleco_trigger(VI_PIPE ViPipe)
{
	pleco_write_register (ViPipe,1,1);// streaming on
	pleco_write_register (ViPipe,1,0);
	ar_always("===pleco trigger stream on!=====\n");
}

void pleco_exit(VI_PIPE ViPipe)
{
	//pleco_write_register (ViPipe,1,1);// streaming on
	pleco_write_register (ViPipe,17,252);// streaming on
	pleco_write_register (ViPipe,17,253);
	ar_always("===pleco trigger stream off!=====\n");
    //pleco_i2c_exit(ViPipe);

    return;
}

/*
* mclk 24Mhz
* mipi data rate per lane 1.4Gbps, 2lane
* pixel size: 1920*(480*6), data id 0x2B (RAW10)
* embed size: 1920*2*6, data id 0x12
* 30fps dual-shuffle master setting
* Tx current 3.5A, laser duty compensate 0.4ns
* modulation frequency 4ns & 5ns, integration time 500us
*/

//#define USE_MIPI_800M
//#define USE_1LNAE_1440M
void pleco_dual_shuffle_master_1920x2892_init(VI_PIPE ViPipe)
{	
#ifdef USE_MIPI_800M

	pleco_write_register (ViPipe,16,0x01);
    pleco_write_register (ViPipe,580,179);
    pleco_write_register (ViPipe,38,228);
    pleco_write_register (ViPipe,39,1);
    pleco_write_register (ViPipe,740,160);
    pleco_write_register (ViPipe,741,5);
    pleco_write_register (ViPipe,784,224); //dg 
    pleco_write_register (ViPipe,785,1); //dg
    pleco_write_register (ViPipe,704,239);
    pleco_write_register (ViPipe,702,226);
    pleco_write_register (ViPipe,703,1);
    pleco_write_register (ViPipe,74,100);
    pleco_write_register (ViPipe,75,17);
    pleco_write_register (ViPipe,35,10);
    pleco_write_register (ViPipe,593,169);
    pleco_write_register (ViPipe,585,43);
    pleco_write_register (ViPipe,586,41);
	pleco_write_register (ViPipe,578,0xB2);
    pleco_write_register (ViPipe,579,0x6B);
    pleco_write_register (ViPipe,224,0);
    pleco_write_register (ViPipe,580,177);
    pleco_write_register (ViPipe,702,170);
	pleco_write_register (ViPipe,703,5);
    pleco_write_register (ViPipe,18,118);

	// 15 fps
	pleco_write_register (ViPipe,28,0x70);
    pleco_write_register (ViPipe,29,0x10);
	pleco_write_register (ViPipe,30,0x00);

	// DMF
    pleco_write_register (ViPipe,784,224);
    pleco_write_register (ViPipe,785,1);
    pleco_write_register (ViPipe,702,86);// rd_line_max = 2902, six subframe
	
    pleco_write_register (ViPipe,703,11);
    pleco_write_register (ViPipe,704,239);
    pleco_write_register (ViPipe,740,160);
    pleco_write_register (ViPipe,741,5);
    pleco_write_register (ViPipe,31,170);// frame number 10922 in DMF
	
    pleco_write_register (ViPipe,32,42);

	// 2lane 1.4Gbps
    pleco_write_register (ViPipe,62,180);
    pleco_write_register (ViPipe,66,43);// data type RAW 10
    pleco_write_register (ViPipe,576,172);// ADC 10bit
    pleco_write_register (ViPipe,735,96);// RAW10, EBD length 2400 byte
    pleco_write_register (ViPipe,736,9);

    pleco_write_register (ViPipe,705,43);// EBD pixel num = EBD length/8 -1 = 299
	pleco_write_register (ViPipe,706,12);
    pleco_write_register (ViPipe,585,43);
    pleco_write_register (ViPipe,586,217);
    pleco_write_register (ViPipe,587,88);

	// 2 lane 800Mbps
	pleco_write_register (ViPipe,636,130);
    pleco_write_register (ViPipe,62,100);
    pleco_write_register (ViPipe,745,20);
    pleco_write_register (ViPipe,746,12);	
	pleco_write_register (ViPipe,747,80);
    pleco_write_register (ViPipe,748,0);	
    pleco_write_register (ViPipe,749,12);
    pleco_write_register (ViPipe,750,30);	
	pleco_write_register (ViPipe,751,0);
    pleco_write_register (ViPipe,752,12);
	pleco_write_register (ViPipe,753,12);
    pleco_write_register (ViPipe,754,0);


	// raw10 analog config
    pleco_write_register (ViPipe,592,175);
    pleco_write_register (ViPipe,168,12);
    pleco_write_register (ViPipe,170,32);
    pleco_write_register (ViPipe,172,45);
    pleco_write_register (ViPipe,174,115);
    pleco_write_register (ViPipe,175,0);
    pleco_write_register (ViPipe,180,30);
    pleco_write_register (ViPipe,182,34);
    pleco_write_register (ViPipe,184,113);
    pleco_write_register (ViPipe,185,0);
    pleco_write_register (ViPipe,186,117);
    pleco_write_register (ViPipe,187,0);
    pleco_write_register (ViPipe,192,41);
    pleco_write_register (ViPipe,188,40);
	pleco_write_register (ViPipe,190,42);
	pleco_write_register (ViPipe,196,118);
    pleco_write_register (ViPipe,197,0);
    pleco_write_register (ViPipe,198,119);
    pleco_write_register (ViPipe,199,0);
    pleco_write_register (ViPipe,176,1);
    pleco_write_register (ViPipe,178,3);
    pleco_write_register (ViPipe,144,10);
    pleco_write_register (ViPipe,146,32);
    pleco_write_register (ViPipe,148,42);
    pleco_write_register (ViPipe,150,115);
    pleco_write_register (ViPipe,151,0);
	pleco_write_register (ViPipe,160,14);
	pleco_write_register (ViPipe,162,33);
	pleco_write_register (ViPipe,164,48);
	pleco_write_register (ViPipe,166,116);
	pleco_write_register (ViPipe,167,0);
	pleco_write_register (ViPipe,152,6);
	pleco_write_register (ViPipe,154,8);
	pleco_write_register (ViPipe,156,38);
	pleco_write_register (ViPipe,158,40);
	pleco_write_register (ViPipe,578,106);
	pleco_write_register (ViPipe,579,249);

	pleco_write_register (ViPipe,632,13);
	pleco_write_register (ViPipe,633,0);
	pleco_write_register (ViPipe,634,160);
	pleco_write_register (ViPipe,602,19);
	pleco_write_register (ViPipe,603,166);

	pleco_write_register (ViPipe,583,130);
	pleco_write_register (ViPipe,609,18);
	pleco_write_register (ViPipe,609,26);
	pleco_write_register (ViPipe,608,244);
	pleco_write_register (ViPipe,569,1);
	pleco_write_register (ViPipe,742,184);
	pleco_write_register (ViPipe,743,2);

	pleco_write_register (ViPipe,31,170);// dual freq frame num
	pleco_write_register (ViPipe,32,42);

	pleco_write_register (ViPipe,526,244);// f0 integration time
	pleco_write_register (ViPipe,527,1);
	pleco_write_register (ViPipe,528,244);// f1 integration time
	pleco_write_register (ViPipe,529,1);

	pleco_write_register (ViPipe,537,8);// f1 npulse
	pleco_write_register (ViPipe,538,1);
	pleco_write_register (ViPipe,541,216);// f1 npulse
	pleco_write_register (ViPipe,542,0);
	pleco_write_register (ViPipe,567,1); // f0
	pleco_write_register (ViPipe,568,2); // f1
	pleco_write_register (ViPipe,547,16);// f0 duty
	pleco_write_register (ViPipe,551,16);// f1 duty
	
	pleco_write_register (ViPipe,16,0x00);

	// PHX3D Laser Driver reset
	pleco_write_register (ViPipe,356,0x04);
	pleco_write_register (ViPipe,375,0x02);
	pleco_write_register (ViPipe,376,0x25);
	pleco_write_register (ViPipe,377,0x01);
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	// PHX3D Laser Driver initial
	pleco_write_register (ViPipe,356,0x07);
	pleco_write_register (ViPipe,378,0x15);
	pleco_write_register (ViPipe,379,0x00);
	pleco_write_register (ViPipe,380,0x1C);// 0x00
	pleco_write_register (ViPipe,381,0x20);// 0x01
	pleco_write_register (ViPipe,382,0x30);// 0x02
	pleco_write_register (ViPipe,383,0x10);// 0x03
	pleco_write_register (ViPipe,384,0x20);// 0x04
	pleco_write_register (ViPipe,385,0x69);// 0x05
	pleco_write_register (ViPipe,386,0xC3);// 0x06
	pleco_write_register (ViPipe,387,0x00);// 0x07 IBIAS_FIX
	pleco_write_register (ViPipe,388,0xC0);// 0x08 ISW_FIX 
	pleco_write_register (ViPipe,389,0x24);// 0x09
	pleco_write_register (ViPipe,390,0x0D);// 0x0A
	pleco_write_register (ViPipe,391,0xFF);// 0x0B
	pleco_write_register (ViPipe,392,0xD9);// 0x0C
	pleco_write_register (ViPipe,393,0xC2);// 0x0D
	pleco_write_register (ViPipe,394,0x78);// 0x0E ISW_FIX threshold
	pleco_write_register (ViPipe,395,0x01);// 0x0F
	pleco_write_register (ViPipe,396,0xDF);// 0x10
	pleco_write_register (ViPipe,397,0x00);// 0x11
	pleco_write_register (ViPipe,398,0x00);// 0x12
	pleco_write_register (ViPipe,399,0x01);// 0x13
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	pleco_write_register (ViPipe,356,0x1D);
	pleco_write_register (ViPipe,400,0x02);
	pleco_write_register (ViPipe,401,0x26);
	pleco_write_register (ViPipe,402,0x19);// 0x26 internal PD resistance
	pleco_write_register (ViPipe,403,0x31);// 0x27
	pleco_write_register (ViPipe,404,0xFF);// 0x28
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	pleco_write_register (ViPipe,356,0x22);
	pleco_write_register (ViPipe,405,0x09);
	pleco_write_register (ViPipe,406,0x2B);
	pleco_write_register (ViPipe,407,0xF1);// 0x2B
	pleco_write_register (ViPipe,408,0xF8);// 0x2C
	pleco_write_register (ViPipe,409,0x03);// 0x2D
	pleco_write_register (ViPipe,410,0x02);// 0x2E
	pleco_write_register (ViPipe,411,0x00);// 0x2F
	pleco_write_register (ViPipe,412,0x00);// 0x30 duty increase
	pleco_write_register (ViPipe,413,0x00);// 0x31 duty decrease
	pleco_write_register (ViPipe,414,0xFF);// 0x32 falling edge speed control
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	// Laser Driver ADC auto start
	pleco_write_register (ViPipe,357,0x05);
	pleco_write_register (ViPipe,359,0x20);
	pleco_write_register (ViPipe,403,0x02);
	pleco_write_register (ViPipe,404,0x13);
	pleco_write_register (ViPipe,405,0x21);
	pleco_write_register (ViPipe,365,0x01);
	pleco_write_register (ViPipe,366,0x01);

	// Laser Driver temperature auto read
	pleco_write_register (ViPipe,344,0x05);
	pleco_write_register (ViPipe,346,0x23);
	pleco_write_register (ViPipe,347,0x0B);

	// read driver IC address [0x14 - 0x24]
	pleco_write_register (ViPipe,406,0x13);
	pleco_write_register (ViPipe,407,0x94);
	pleco_write_register (ViPipe,360,0x01);
	pleco_write_register (ViPipe,361,0xFF);
	pleco_write_register (ViPipe,362,0x20);

	//pleco_write_register (ViPipe,1,1);// streaming on
	//pleco_write_register (ViPipe,1,0);


    printf("===PLECO 800M pleco_dual_shuffle_master_1920x2892_init Init OK!===\n");
#elif defined(USE_1LNAE_1440M)
		pleco_write_register (ViPipe,16,0x01);
		pleco_write_register (ViPipe,580,179);
		pleco_write_register (ViPipe,38,228);
		pleco_write_register (ViPipe,39,1);
		pleco_write_register (ViPipe,740,160);
		pleco_write_register (ViPipe,741,5);
		pleco_write_register (ViPipe,784,224); //dg 
		pleco_write_register (ViPipe,785,1); //dg
		pleco_write_register (ViPipe,704,239);
		pleco_write_register (ViPipe,702,226);
		pleco_write_register (ViPipe,703,1);
		pleco_write_register (ViPipe,74,100);
		pleco_write_register (ViPipe,75,17);
		pleco_write_register (ViPipe,35,10);
		pleco_write_register (ViPipe,593,169);
		pleco_write_register (ViPipe,585,43);
		pleco_write_register (ViPipe,586,41);
		pleco_write_register (ViPipe,578,0xB2);
		pleco_write_register (ViPipe,579,0x6B);
		pleco_write_register (ViPipe,224,0);
		pleco_write_register (ViPipe,580,177);
		pleco_write_register (ViPipe,702,170);
		pleco_write_register (ViPipe,703,5);
		pleco_write_register (ViPipe,18,118);
	
		// DMF
		pleco_write_register (ViPipe,784,224);
		pleco_write_register (ViPipe,785,1);
		pleco_write_register (ViPipe,702,86);// rd_line_max = 2902, six subframe
		
		pleco_write_register (ViPipe,703,11);
		pleco_write_register (ViPipe,704,239);
		pleco_write_register (ViPipe,740,160);
		pleco_write_register (ViPipe,741,5);
		pleco_write_register (ViPipe,31,170);// frame number 10922 in DMF
		
		pleco_write_register (ViPipe,32,42);
	
		// 1440Mbps
		pleco_write_register (ViPipe,62,180);
		pleco_write_register (ViPipe,66,43);// data type RAW 10
		pleco_write_register (ViPipe,576,172);// ADC 10bit
		pleco_write_register (ViPipe,735,96);// RAW10, EBD length 2400 byte
		pleco_write_register (ViPipe,736,9);
	
		pleco_write_register (ViPipe,705,43);// EBD pixel num = EBD length/8 -1 = 299
		pleco_write_register (ViPipe,706,12);
		pleco_write_register (ViPipe,585,43);
		pleco_write_register (ViPipe,586,217);
		pleco_write_register (ViPipe,587,88);
		
		// 1 lane
		pleco_write_register (ViPipe,65,0);
	
		// raw10 analog config
		pleco_write_register (ViPipe,592,175);
		pleco_write_register (ViPipe,168,12);
		pleco_write_register (ViPipe,170,32);
		pleco_write_register (ViPipe,172,45);
		pleco_write_register (ViPipe,174,115);
		pleco_write_register (ViPipe,175,0);
		pleco_write_register (ViPipe,180,30);
		pleco_write_register (ViPipe,182,34);
		pleco_write_register (ViPipe,184,113);
		pleco_write_register (ViPipe,185,0);
		pleco_write_register (ViPipe,186,117);
		pleco_write_register (ViPipe,187,0);
		pleco_write_register (ViPipe,192,41);
		pleco_write_register (ViPipe,188,40);
		pleco_write_register (ViPipe,190,42);
		pleco_write_register (ViPipe,196,118);
		pleco_write_register (ViPipe,197,0);
		pleco_write_register (ViPipe,198,119);
		pleco_write_register (ViPipe,199,0);
		pleco_write_register (ViPipe,176,1);
		pleco_write_register (ViPipe,178,3);
		pleco_write_register (ViPipe,144,10);
		pleco_write_register (ViPipe,146,32);
		pleco_write_register (ViPipe,148,42);
		pleco_write_register (ViPipe,150,115);
		pleco_write_register (ViPipe,151,0);
		pleco_write_register (ViPipe,160,14);
		pleco_write_register (ViPipe,162,33);
		pleco_write_register (ViPipe,164,48);
		pleco_write_register (ViPipe,166,116);
		pleco_write_register (ViPipe,167,0);
		pleco_write_register (ViPipe,152,6);
		pleco_write_register (ViPipe,154,8);
		pleco_write_register (ViPipe,156,38);
		pleco_write_register (ViPipe,158,40);
		pleco_write_register (ViPipe,578,106);
		pleco_write_register (ViPipe,579,249);
	
		pleco_write_register (ViPipe,632,13);
		pleco_write_register (ViPipe,633,0);
		pleco_write_register (ViPipe,634,160);
		pleco_write_register (ViPipe,602,19);
		pleco_write_register (ViPipe,603,166);
	
		pleco_write_register (ViPipe,583,130);
		pleco_write_register (ViPipe,609,18);
		pleco_write_register (ViPipe,609,26);
		pleco_write_register (ViPipe,608,244);
		pleco_write_register (ViPipe,569,1);
		pleco_write_register (ViPipe,742,184);
		pleco_write_register (ViPipe,743,2);
	
		pleco_write_register (ViPipe,31,170);// dual freq frame num
		pleco_write_register (ViPipe,32,42);
	
		pleco_write_register (ViPipe,526,244);// f0 integration time
		pleco_write_register (ViPipe,527,1);
		pleco_write_register (ViPipe,528,244);// f1 integration time
		pleco_write_register (ViPipe,529,1);
	
		pleco_write_register (ViPipe,537,8);// f1 npulse
		pleco_write_register (ViPipe,538,1);
		pleco_write_register (ViPipe,541,216);// f1 npulse
		pleco_write_register (ViPipe,542,0);
		pleco_write_register (ViPipe,567,1); // f0
		pleco_write_register (ViPipe,568,2); // f1
		pleco_write_register (ViPipe,547,16);// f0 duty
		pleco_write_register (ViPipe,551,16);// f1 duty
		
		pleco_write_register (ViPipe,16,0x00);
	
		// PHX3D Laser Driver reset
		pleco_write_register (ViPipe,356,0x04);
		pleco_write_register (ViPipe,375,0x02);
		pleco_write_register (ViPipe,376,0x25);
		pleco_write_register (ViPipe,377,0x01);
		pleco_write_register (ViPipe,354,0x03);
		pleco_write_register (ViPipe,354,0x00);
	
		// PHX3D Laser Driver initial
		pleco_write_register (ViPipe,356,0x07);
		pleco_write_register (ViPipe,378,0x15);
		pleco_write_register (ViPipe,379,0x00);
		pleco_write_register (ViPipe,380,0x1C);// 0x00
		pleco_write_register (ViPipe,381,0x20);// 0x01
		pleco_write_register (ViPipe,382,0x30);// 0x02
		pleco_write_register (ViPipe,383,0x10);// 0x03
		pleco_write_register (ViPipe,384,0x20);// 0x04
		pleco_write_register (ViPipe,385,0x69);// 0x05
		pleco_write_register (ViPipe,386,0xC3);// 0x06
		pleco_write_register (ViPipe,387,0x00);// 0x07 IBIAS_FIX
		pleco_write_register (ViPipe,388,0xC0);// 0x08 ISW_FIX 
		pleco_write_register (ViPipe,389,0x24);// 0x09
		pleco_write_register (ViPipe,390,0x0D);// 0x0A
		pleco_write_register (ViPipe,391,0xFF);// 0x0B
		pleco_write_register (ViPipe,392,0xD9);// 0x0C
		pleco_write_register (ViPipe,393,0xC2);// 0x0D
		pleco_write_register (ViPipe,394,0x78);// 0x0E ISW_FIX threshold
		pleco_write_register (ViPipe,395,0x01);// 0x0F
		pleco_write_register (ViPipe,396,0xDF);// 0x10
		pleco_write_register (ViPipe,397,0x00);// 0x11
		pleco_write_register (ViPipe,398,0x00);// 0x12
		pleco_write_register (ViPipe,399,0x01);// 0x13
		pleco_write_register (ViPipe,354,0x03);
		pleco_write_register (ViPipe,354,0x00);
	
		pleco_write_register (ViPipe,356,0x1D);
		pleco_write_register (ViPipe,400,0x02);
		pleco_write_register (ViPipe,401,0x26);
		pleco_write_register (ViPipe,402,0x19);// 0x26 internal PD resistance
		pleco_write_register (ViPipe,403,0x31);// 0x27
		pleco_write_register (ViPipe,404,0xFF);// 0x28
		pleco_write_register (ViPipe,354,0x03);
		pleco_write_register (ViPipe,354,0x00);
	
		pleco_write_register (ViPipe,356,0x22);
		pleco_write_register (ViPipe,405,0x09);
		pleco_write_register (ViPipe,406,0x2B);
		pleco_write_register (ViPipe,407,0xF1);// 0x2B
		pleco_write_register (ViPipe,408,0xF8);// 0x2C
		pleco_write_register (ViPipe,409,0x03);// 0x2D
		pleco_write_register (ViPipe,410,0x02);// 0x2E
		pleco_write_register (ViPipe,411,0x00);// 0x2F
		pleco_write_register (ViPipe,412,0x00);// 0x30 duty increase
		pleco_write_register (ViPipe,413,0x00);// 0x31 duty decrease
		pleco_write_register (ViPipe,414,0xFF);// 0x32 falling edge speed control
		pleco_write_register (ViPipe,354,0x03);
		pleco_write_register (ViPipe,354,0x00);
	
		// Laser Driver ADC auto start
		pleco_write_register (ViPipe,357,0x05);
		pleco_write_register (ViPipe,359,0x20);
		pleco_write_register (ViPipe,403,0x02);
		pleco_write_register (ViPipe,404,0x13);
		pleco_write_register (ViPipe,405,0x21);
		pleco_write_register (ViPipe,365,0x01);
		pleco_write_register (ViPipe,366,0x01);
	
		// Laser Driver temperature auto read
		pleco_write_register (ViPipe,344,0x05);
		pleco_write_register (ViPipe,346,0x23);
		pleco_write_register (ViPipe,347,0x0B);
	
		// read driver IC address [0x14 - 0x24]
		pleco_write_register (ViPipe,406,0x13);
		pleco_write_register (ViPipe,407,0x94);
		pleco_write_register (ViPipe,360,0x01);
		pleco_write_register (ViPipe,361,0xFF);
		pleco_write_register (ViPipe,362,0x20);
	
		//pleco_write_register (ViPipe,1,1);// streaming on
		//pleco_write_register (ViPipe,1,0);
		printf("===PLECO 1 lane 1.44G	pleco_dual_shuffle_master_1920x2892_init Init OK!===\n");
#else
	
    pleco_write_register (ViPipe,16,0x01);
    pleco_write_register (ViPipe,580,179);
    pleco_write_register (ViPipe,38,228);
    pleco_write_register (ViPipe,39,1);
    pleco_write_register (ViPipe,740,160);
    pleco_write_register (ViPipe,741,5);
    pleco_write_register (ViPipe,784,224); //dg 
    pleco_write_register (ViPipe,785,1); //dg
    pleco_write_register (ViPipe,704,239);
    pleco_write_register (ViPipe,702,226);
    pleco_write_register (ViPipe,703,1);
    pleco_write_register (ViPipe,74,100);
    pleco_write_register (ViPipe,75,17);
    pleco_write_register (ViPipe,35,10);
    pleco_write_register (ViPipe,593,169);
    pleco_write_register (ViPipe,585,43);
    pleco_write_register (ViPipe,586,41);
	pleco_write_register (ViPipe,578,0xB2);
    pleco_write_register (ViPipe,579,0x6B);
    pleco_write_register (ViPipe,224,0);
    pleco_write_register (ViPipe,580,179);
    pleco_write_register (ViPipe,702,170);
	pleco_write_register (ViPipe,703,5);
    pleco_write_register (ViPipe,18,118);

	// DMF
    pleco_write_register (ViPipe,784,224);
    pleco_write_register (ViPipe,785,1);
    pleco_write_register (ViPipe,702,86);// rd_line_max = 2902, six subframe
	
    pleco_write_register (ViPipe,703,11);
    pleco_write_register (ViPipe,704,239);
    pleco_write_register (ViPipe,740,160);
    pleco_write_register (ViPipe,741,5);
    pleco_write_register (ViPipe,31,170);// frame number 10922 in DMF
	
    pleco_write_register (ViPipe,32,42);

	// 2lane 1.4Gbps
    pleco_write_register (ViPipe,62,180);
    pleco_write_register (ViPipe,66,43);// data type RAW 10
    pleco_write_register (ViPipe,576,172);// ADC 10bit
    pleco_write_register (ViPipe,735,96);// RAW10, EBD length 2400 byte
    pleco_write_register (ViPipe,736,9);

    pleco_write_register (ViPipe,705,43);// EBD pixel num = EBD length/8 -1 = 299
	pleco_write_register (ViPipe,706,12);
    pleco_write_register (ViPipe,585,43);
    pleco_write_register (ViPipe,586,217);
    pleco_write_register (ViPipe,587,88);

	// raw10 analog config
    pleco_write_register (ViPipe,592,175);
    pleco_write_register (ViPipe,168,12);
    pleco_write_register (ViPipe,170,32);
    pleco_write_register (ViPipe,172,45);
    pleco_write_register (ViPipe,174,115);
    pleco_write_register (ViPipe,175,0);
    pleco_write_register (ViPipe,180,30);
    pleco_write_register (ViPipe,182,34);
    pleco_write_register (ViPipe,184,113);
    pleco_write_register (ViPipe,185,0);
    pleco_write_register (ViPipe,186,117);
    pleco_write_register (ViPipe,187,0);
    pleco_write_register (ViPipe,192,41);
    pleco_write_register (ViPipe,188,40);
	pleco_write_register (ViPipe,190,42);
	pleco_write_register (ViPipe,196,118);
    pleco_write_register (ViPipe,197,0);
    pleco_write_register (ViPipe,198,119);
    pleco_write_register (ViPipe,199,0);
    pleco_write_register (ViPipe,176,1);
    pleco_write_register (ViPipe,178,3);
    pleco_write_register (ViPipe,144,10);
    pleco_write_register (ViPipe,146,32);
    pleco_write_register (ViPipe,148,42);
    pleco_write_register (ViPipe,150,115);
    pleco_write_register (ViPipe,151,0);
	pleco_write_register (ViPipe,160,14);
	pleco_write_register (ViPipe,162,33);
	pleco_write_register (ViPipe,164,48);
	pleco_write_register (ViPipe,166,116);
	pleco_write_register (ViPipe,167,0);
	pleco_write_register (ViPipe,152,6);
	pleco_write_register (ViPipe,154,8);
	pleco_write_register (ViPipe,156,38);
	pleco_write_register (ViPipe,158,40);
	pleco_write_register (ViPipe,578,106);
	pleco_write_register (ViPipe,579,249);

	pleco_write_register (ViPipe,632,13);
	pleco_write_register (ViPipe,633,0);
	pleco_write_register (ViPipe,634,160);
	pleco_write_register (ViPipe,602,19);
	pleco_write_register (ViPipe,603,166);

	pleco_write_register (ViPipe,583,130);
	pleco_write_register (ViPipe,609,18);
	pleco_write_register (ViPipe,609,26);
	pleco_write_register (ViPipe,608,244);
	pleco_write_register (ViPipe,569,1);
	pleco_write_register (ViPipe,742,184);
	pleco_write_register (ViPipe,743,2);

	pleco_write_register (ViPipe,31,170);// dual freq frame num
	pleco_write_register (ViPipe,32,42);

	pleco_write_register (ViPipe,526,244);// f0 integration time
	pleco_write_register (ViPipe,527,1);
	pleco_write_register (ViPipe,528,244);// f1 integration time
	pleco_write_register (ViPipe,529,1);

	pleco_write_register (ViPipe,537,8);// f1 npulse
	pleco_write_register (ViPipe,538,1);
	pleco_write_register (ViPipe,541,216);// f1 npulse
	pleco_write_register (ViPipe,542,0);
	pleco_write_register (ViPipe,567,1); // f0
	pleco_write_register (ViPipe,568,2); // f1
	pleco_write_register (ViPipe,547,16);// f0 duty
	pleco_write_register (ViPipe,551,16);// f1 duty
	
	//pleco_write_register (ViPipe,755,0x00); //close calibration.
	//pleco_write_register (ViPipe,756,0x00); //close calibration.
	//pleco_write_register (ViPipe,757,0x00); //close calibration.	
	//pleco_write_register (ViPipe,758,0x00); //close calibration.

	pleco_write_register (ViPipe,16,0x00);

	// PHX3D Laser Driver reset
	pleco_write_register (ViPipe,356,0x04);
	pleco_write_register (ViPipe,375,0x02);
	pleco_write_register (ViPipe,376,0x25);
	pleco_write_register (ViPipe,377,0x01);
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	// PHX3D Laser Driver initial
	pleco_write_register (ViPipe,356,0x07);
	pleco_write_register (ViPipe,378,0x15);
	pleco_write_register (ViPipe,379,0x00);
	pleco_write_register (ViPipe,380,0x1C);// 0x00
	pleco_write_register (ViPipe,381,0x20);// 0x01
	pleco_write_register (ViPipe,382,0x30);// 0x02
	pleco_write_register (ViPipe,383,0x10);// 0x03
	pleco_write_register (ViPipe,384,0x20);// 0x04
	pleco_write_register (ViPipe,385,0x69);// 0x05
	pleco_write_register (ViPipe,386,0xC3);// 0x06
	pleco_write_register (ViPipe,387,0x00);// 0x07 IBIAS_FIX
	pleco_write_register (ViPipe,388,0xC0);// 0x08 ISW_FIX 
	pleco_write_register (ViPipe,389,0x24);// 0x09
	pleco_write_register (ViPipe,390,0x0D);// 0x0A
	pleco_write_register (ViPipe,391,0xFF);// 0x0B
	pleco_write_register (ViPipe,392,0xD9);// 0x0C
	pleco_write_register (ViPipe,393,0xC2);// 0x0D
	pleco_write_register (ViPipe,394,0x78);// 0x0E ISW_FIX threshold
	pleco_write_register (ViPipe,395,0x01);// 0x0F
	pleco_write_register (ViPipe,396,0xDF);// 0x10
	pleco_write_register (ViPipe,397,0x00);// 0x11
	pleco_write_register (ViPipe,398,0x00);// 0x12
	pleco_write_register (ViPipe,399,0x01);// 0x13
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	pleco_write_register (ViPipe,356,0x1D);
	pleco_write_register (ViPipe,400,0x02);
	pleco_write_register (ViPipe,401,0x26);
	pleco_write_register (ViPipe,402,0x19);// 0x26 internal PD resistance
	pleco_write_register (ViPipe,403,0x31);// 0x27
	pleco_write_register (ViPipe,404,0xFF);// 0x28
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	pleco_write_register (ViPipe,356,0x22);
	pleco_write_register (ViPipe,405,0x09);
	pleco_write_register (ViPipe,406,0x2B);
	pleco_write_register (ViPipe,407,0xF1);// 0x2B
	pleco_write_register (ViPipe,408,0xF8);// 0x2C
	pleco_write_register (ViPipe,409,0x03);// 0x2D
	pleco_write_register (ViPipe,410,0x02);// 0x2E
	pleco_write_register (ViPipe,411,0x00);// 0x2F
	pleco_write_register (ViPipe,412,0x00);// 0x30 duty increase
	pleco_write_register (ViPipe,413,0x00);// 0x31 duty decrease
	pleco_write_register (ViPipe,414,0xFF);// 0x32 falling edge speed control
	pleco_write_register (ViPipe,354,0x03);
	pleco_write_register (ViPipe,354,0x00);

	// Laser Driver ADC auto start
	pleco_write_register (ViPipe,357,0x05);
	pleco_write_register (ViPipe,359,0x20);
	pleco_write_register (ViPipe,403,0x02);
	pleco_write_register (ViPipe,404,0x13);
	pleco_write_register (ViPipe,405,0x21);
	pleco_write_register (ViPipe,365,0x01);
	pleco_write_register (ViPipe,366,0x01);

	// Laser Driver temperature auto read
	pleco_write_register (ViPipe,344,0x05);
	pleco_write_register (ViPipe,346,0x23);
	pleco_write_register (ViPipe,347,0x0B);

	// read driver IC address [0x14 - 0x24]
	pleco_write_register (ViPipe,406,0x13);
	pleco_write_register (ViPipe,407,0x94);
	pleco_write_register (ViPipe,360,0x01);
	pleco_write_register (ViPipe,361,0xFF);
	pleco_write_register (ViPipe,362,0x20);
	
	//pleco_write_register (ViPipe,1,1);// streaming on
	//pleco_write_register (ViPipe,1,0);

    ar_always("===PLECO 1.4G pleco_dual_shuffle_master_1920x2892_init Init OK!===\n");
#endif
	
    return;
}





/* 1080P30 */
void pleco_linear_1080p30_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe,0x3002,0x00);
    pleco_write_register (ViPipe,0x3005,0x01);
    pleco_write_register (ViPipe,0x3007,0x00);

    pleco_write_register (ViPipe,0x3009,0x02);

    pleco_write_register (ViPipe,0x300A,0xF0);
    pleco_write_register (ViPipe,0x3011,0x0a);

    pleco_write_register (ViPipe,0x3018,0x65); //dg 
    pleco_write_register (ViPipe,0x3019,0x04); //dg

    pleco_write_register (ViPipe,0x301c,0x30);
    pleco_write_register (ViPipe,0x301d,0x11);

    pleco_write_register (ViPipe,0x3046,0x01);
    pleco_write_register (ViPipe,0x304B,0x0a);

    pleco_write_register (ViPipe,0x305C,0x18);
    pleco_write_register (ViPipe,0x305D,0x03);
    pleco_write_register (ViPipe,0x305E,0x20);
    pleco_write_register (ViPipe,0x305F,0x01);

    pleco_write_register (ViPipe,0x309e,0x4a);
    pleco_write_register (ViPipe,0x309f,0x4a);

    pleco_write_register (ViPipe,0x311C,0x0E);
    pleco_write_register (ViPipe,0x3128,0x04);
    pleco_write_register (ViPipe,0x3129,0x00);
    pleco_write_register (ViPipe,0x313b,0x41);
    pleco_write_register (ViPipe,0x315E,0x1A);
    pleco_write_register (ViPipe,0x3164,0x1A);
    pleco_write_register (ViPipe,0x317C,0x00);
    pleco_write_register (ViPipe,0x31EC,0x0E);

    pleco_write_register (ViPipe,0x3405,0x10);

    pleco_write_register (ViPipe,0x3407,0x01);
    pleco_write_register (ViPipe,0x3414,0x0A);
    pleco_write_register (ViPipe,0x3418,0x38);
    pleco_write_register (ViPipe,0x3419,0x04);
    pleco_write_register (ViPipe,0x3441,0x0C);
    pleco_write_register (ViPipe,0x3442,0x0C);
    pleco_write_register (ViPipe,0x3443,0x01);
    pleco_write_register (ViPipe,0x3444,0x20);
    pleco_write_register (ViPipe,0x3445,0x25);

    pleco_write_register (ViPipe,0x3446,0x57);
    pleco_write_register (ViPipe,0x3447,0x00);

    pleco_write_register (ViPipe,0x3448,0x37);
    pleco_write_register (ViPipe,0x3449,0x00);

    pleco_write_register (ViPipe,0x344A,0x1F);
    pleco_write_register (ViPipe,0x344B,0x00);

    pleco_write_register (ViPipe,0x344C,0x1F);
    pleco_write_register (ViPipe,0x344D,0x00);

    pleco_write_register (ViPipe,0x344E,0x1F);
    pleco_write_register (ViPipe,0x344F,0x00);

    pleco_write_register (ViPipe,0x3450,0x77);
    pleco_write_register (ViPipe,0x3451,0x00);

    pleco_write_register (ViPipe,0x3452,0x1F);
    pleco_write_register (ViPipe,0x3453,0x00);

    pleco_write_register (ViPipe,0x3454,0x17);
    pleco_write_register (ViPipe,0x3455,0x00);

    pleco_write_register (ViPipe,0x3472,0x80);
    pleco_write_register (ViPipe,0x3473,0x07);
    //{REGISTER_OP_WRITE,0x3480,0x49,

    pleco_default_reg_init(ViPipe);

    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //pleco_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */
    ar_always("===PLECO 1080P 30fps 12bit LINE Init OK!===\n");
    return;
}


/* 1080P60 */
void pleco_linear_1080p60_init(VI_PIPE ViPipe)
{

    pleco_write_register (ViPipe,0x3002,0x00);
    pleco_write_register (ViPipe,0x3005,0x01);
    pleco_write_register (ViPipe,0x3007,0x00);

    pleco_write_register (ViPipe,0x3009,0x01); //dg for 60fps

    pleco_write_register (ViPipe,0x300A,0xF0);
    pleco_write_register (ViPipe,0x3011,0x0a);

    pleco_write_register (ViPipe,0x3018,0x65); //dg 
    pleco_write_register (ViPipe,0x3019,0x04); //dg

    pleco_write_register (ViPipe,0x301c,0x98); //dg    for 60fps
    pleco_write_register (ViPipe,0x301d,0x08); //dg

    pleco_write_register (ViPipe,0x3046,0x01);
    pleco_write_register (ViPipe,0x304B,0x0a);

    pleco_write_register (ViPipe,0x305C,0x18);
    pleco_write_register (ViPipe,0x305D,0x03);
    pleco_write_register (ViPipe,0x305E,0x20);
    pleco_write_register (ViPipe,0x305F,0x01);

    pleco_write_register (ViPipe,0x309e,0x4a);
    pleco_write_register (ViPipe,0x309f,0x4a);

    pleco_write_register (ViPipe,0x311C,0x0E);
    pleco_write_register (ViPipe,0x3128,0x04);
    pleco_write_register (ViPipe,0x3129,0x00);
    pleco_write_register (ViPipe,0x313b,0x41);
    pleco_write_register (ViPipe,0x315E,0x1A);
    pleco_write_register (ViPipe,0x3164,0x1A);
    pleco_write_register (ViPipe,0x317C,0x00);
    pleco_write_register (ViPipe,0x31EC,0x0E);

    pleco_write_register (ViPipe,0x3405,0x00); // dg for 60fps

    pleco_write_register (ViPipe,0x3407,0x01);
    pleco_write_register (ViPipe,0x3414,0x0A);
    pleco_write_register (ViPipe,0x3418,0x38);
    pleco_write_register (ViPipe,0x3419,0x04);
    pleco_write_register (ViPipe,0x3441,0x0C);
    pleco_write_register (ViPipe,0x3442,0x0C);
    pleco_write_register (ViPipe,0x3443,0x01);
    pleco_write_register (ViPipe,0x3444,0x20);
    pleco_write_register (ViPipe,0x3445,0x25);

    pleco_write_register (ViPipe,0x3446,0x77); // dg for 60fps
    pleco_write_register (ViPipe,0x3447,0x00);

    pleco_write_register (ViPipe,0x3448,0x67); // dg for 60fps
    pleco_write_register (ViPipe,0x3449,0x00);

    pleco_write_register (ViPipe,0x344A,0x47); // dg for 60fps
    pleco_write_register (ViPipe,0x344B,0x00);

    pleco_write_register (ViPipe,0x344C,0x37); // dg for 60fps
    pleco_write_register (ViPipe,0x344D,0x00);

    pleco_write_register (ViPipe,0x344E,0x3F); // dg for 60fps
    pleco_write_register (ViPipe,0x344F,0x00);

    pleco_write_register (ViPipe,0x3450,0xFF); // dg for 60fps
    pleco_write_register (ViPipe,0x3451,0x00);

    pleco_write_register (ViPipe,0x3452,0x03); // dg for 60fps
    pleco_write_register (ViPipe,0x3453,0x00);

    pleco_write_register (ViPipe,0x3454,0x37); // dg for 60fps
    pleco_write_register (ViPipe,0x3455,0x00);

    pleco_write_register (ViPipe,0x3472,0x80);
    pleco_write_register (ViPipe,0x3473,0x07);
    //{REGISTER_OP_WRITE,0x3480,0x49,

    pleco_default_reg_init(ViPipe);

    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */
    //pleco_write_register (ViPipe, 0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */

    ar_always("===PLECO 1080P 60fps 12bit LINE Init OK!===\n");
    return;
}


void pleco_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe, 0x3000, 0x01); /* standby */
    pleco_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //10bit
    pleco_write_register (ViPipe, 0x3005, 0x00);
    pleco_write_register (ViPipe, 0x3007, 0x00);
    pleco_write_register (ViPipe, 0x300a, 0x3c);
    pleco_write_register (ViPipe, 0x300c, 0x11);
    pleco_write_register (ViPipe, 0x300f, 0x00);
    pleco_write_register (ViPipe, 0x3010, 0x21);
    pleco_write_register (ViPipe, 0x3012, 0x64);
    pleco_write_register (ViPipe, 0x3016, 0x09);

    pleco_write_register (ViPipe, 0x3018, 0xA6);  /**** VMAX ****/
    pleco_write_register (ViPipe, 0x3019, 0x04);
    pleco_write_register (ViPipe, 0x301A, 0x00);

    pleco_write_register (ViPipe, 0x301C, 0xD8); /***** HMAX ****/
    pleco_write_register (ViPipe, 0x301D, 0x0F);

    pleco_write_register (ViPipe, 0x3020, 0x02);
    pleco_write_register (ViPipe, 0x3024, 0xc9);
    pleco_write_register (ViPipe, 0x3030, 0x0b);
    pleco_write_register (ViPipe, 0x3045, 0x05);
    pleco_write_register (ViPipe, 0x3046, 0x00);
    pleco_write_register (ViPipe, 0x304b, 0x0a);
    pleco_write_register (ViPipe, 0x305c, 0x18);
    pleco_write_register (ViPipe, 0x305d, 0x03);
    pleco_write_register (ViPipe, 0x305e, 0x20);
    pleco_write_register (ViPipe, 0x305f, 0x01);
    pleco_write_register (ViPipe, 0x3070, 0x02);
    pleco_write_register (ViPipe, 0x3071, 0x11);
    pleco_write_register (ViPipe, 0x309b, 0x10);
    pleco_write_register (ViPipe, 0x309c, 0x22);
    pleco_write_register (ViPipe, 0x30a2, 0x02);
    pleco_write_register (ViPipe, 0x30a6, 0x20);
    pleco_write_register (ViPipe, 0x30a8, 0x20);
    pleco_write_register (ViPipe, 0x30aa, 0x20);
    pleco_write_register (ViPipe, 0x30ac, 0x20);
    pleco_write_register (ViPipe, 0x30b0, 0x43);
    pleco_write_register (ViPipe, 0x3106, 0x11);
    pleco_write_register (ViPipe, 0x3119, 0x9e);
    pleco_write_register (ViPipe, 0x311c, 0x1e);
    pleco_write_register (ViPipe, 0x311e, 0x08);
    pleco_write_register (ViPipe, 0x3128, 0x05);
    pleco_write_register (ViPipe, 0x3129, 0x1d);
    pleco_write_register (ViPipe, 0x313d, 0x83);
    pleco_write_register (ViPipe, 0x3150, 0x03);
    pleco_write_register (ViPipe, 0x315e, 0x1a);
    pleco_write_register (ViPipe, 0x3164, 0x1a);
    pleco_write_register (ViPipe, 0x317c, 0x12);
    pleco_write_register (ViPipe, 0x317e, 0x00);
    pleco_write_register (ViPipe, 0x31ec, 0x37);
    pleco_write_register (ViPipe, 0x32b8, 0x50);
    pleco_write_register (ViPipe, 0x32b9, 0x10);
    pleco_write_register (ViPipe, 0x32ba, 0x00);
    pleco_write_register (ViPipe, 0x32bb, 0x04);
    pleco_write_register (ViPipe, 0x32c8, 0x50);
    pleco_write_register (ViPipe, 0x32c9, 0x10);
    pleco_write_register (ViPipe, 0x32ca, 0x00);
    pleco_write_register (ViPipe, 0x32cb, 0x04);
    pleco_write_register (ViPipe, 0x332c, 0xd3);
    pleco_write_register (ViPipe, 0x332d, 0x10);
    pleco_write_register (ViPipe, 0x332e, 0x0d);
    pleco_write_register (ViPipe, 0x3358, 0x06);
    pleco_write_register (ViPipe, 0x3359, 0xe1);
    pleco_write_register (ViPipe, 0x335a, 0x11);
    pleco_write_register (ViPipe, 0x3360, 0x1e);
    pleco_write_register (ViPipe, 0x3361, 0x61);
    pleco_write_register (ViPipe, 0x3362, 0x10);
    pleco_write_register (ViPipe, 0x33b0, 0x50);
    pleco_write_register (ViPipe, 0x33b2, 0x1a);
    pleco_write_register (ViPipe, 0x33b3, 0x04);

    pleco_write_register (ViPipe, 0x3418, 0xb2); /**** Y_OUT_SIZE *****/
    pleco_write_register (ViPipe, 0x3419, 0x08);

    pleco_write_register (ViPipe, 0x3441, 0x0a);
    pleco_write_register (ViPipe, 0x3442, 0x0a);
    pleco_write_register (ViPipe, 0x3444, 0x20);
    pleco_write_register (ViPipe, 0x3445, 0x25);
    pleco_write_register (ViPipe, 0x3480, 0x49);

    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco sensor 1080P15fps 10bit 2to1 WDR(30fps->15fps) init success!=====\n");

    return;

}

void pleco_wdr_1080p60_2to1_init(VI_PIPE ViPipe)
{
    //dol2 2lane 896Mbps, 12bits official setting
    pleco_write_register(ViPipe,0x3003,0x01);
    pleco_write_register(ViPipe,0x3002,0x01);
    pleco_write_register(ViPipe,0x3000,0x01);
    pleco_write_register(ViPipe,0x3005,0x01);
    pleco_write_register(ViPipe,0x3007,0x00);
    pleco_write_register(ViPipe,0x3009,0x01);
    pleco_write_register(ViPipe,0x300A,0xF0);
    pleco_write_register(ViPipe,0x300C,0x11);
    pleco_write_register(ViPipe,0x300F,0x00);
    pleco_write_register(ViPipe,0x3010,0x21);
    pleco_write_register(ViPipe,0x3012,0x64);
    pleco_write_register(ViPipe,0x3016,0x09);
    pleco_write_register(ViPipe,0x3018,0x46);
    pleco_write_register(ViPipe,0x3019,0x05);  //vmax 1350
    pleco_write_register(ViPipe,0x301C,0x98);
    pleco_write_register(ViPipe,0x301D,0x08);  //hmax 2200
    pleco_write_register(ViPipe,0x3020,0x02);  //sexp = 92
    pleco_write_register(ViPipe,0x3021,0x00);
    pleco_write_register(ViPipe,0x3024,0x59);  //lexp = 92*16 = 1520
    pleco_write_register(ViPipe,0x3025,0x03);
    pleco_write_register(ViPipe,0x3030,0xcd);  //rhs = 2n+5 = 95
    pleco_write_register(ViPipe,0x3031,0x01);
    pleco_write_register(ViPipe,0x3045,0x05);
    pleco_write_register(ViPipe,0x3046,0x01);
    pleco_write_register(ViPipe,0x304B,0x0A);
    pleco_write_register(ViPipe,0x305C,0x18);
    pleco_write_register(ViPipe,0x305D,0x03);
    pleco_write_register(ViPipe,0x305E,0x20);
    pleco_write_register(ViPipe,0x305F,0x01);
    pleco_write_register(ViPipe,0x3070,0x02);
    pleco_write_register(ViPipe,0x3071,0x11);
    pleco_write_register(ViPipe,0x309B,0x10);
    pleco_write_register(ViPipe,0x309C,0x22);
    pleco_write_register(ViPipe,0x30A2,0x02);
    pleco_write_register(ViPipe,0x30A6,0x20);
    pleco_write_register(ViPipe,0x30A8,0x20);
    pleco_write_register(ViPipe,0x30AA,0x20);
    pleco_write_register(ViPipe,0x30AC,0x20);
    pleco_write_register(ViPipe,0x30B0,0x43);
    pleco_write_register(ViPipe,0x3106,0x11);
    pleco_write_register(ViPipe,0x3119,0x9E);
    pleco_write_register(ViPipe,0x311C,0x1E);
    pleco_write_register(ViPipe,0x311E,0x08);
    pleco_write_register(ViPipe,0x3128,0x05);
    pleco_write_register(ViPipe,0x3129,0x00);
    pleco_write_register(ViPipe,0x313D,0x83);
    pleco_write_register(ViPipe,0x3150,0x03);
    pleco_write_register(ViPipe,0x315E,0x1A);
    pleco_write_register(ViPipe,0x3164,0x1A);
    pleco_write_register(ViPipe,0x317C,0x00);
    pleco_write_register(ViPipe,0x317E,0x00);
    pleco_write_register(ViPipe,0x31EC,0x0E);
    pleco_write_register(ViPipe,0x32B8,0x50);
    pleco_write_register(ViPipe,0x32B9,0x10);
    pleco_write_register(ViPipe,0x32BA,0x00);
    pleco_write_register(ViPipe,0x32BB,0x04);
    pleco_write_register(ViPipe,0x32C8,0x50);
    pleco_write_register(ViPipe,0x32C9,0x10);
    pleco_write_register(ViPipe,0x32CA,0x00);
    pleco_write_register(ViPipe,0x32CB,0x04);
    pleco_write_register(ViPipe,0x332C,0xD3);
    pleco_write_register(ViPipe,0x332D,0x10);
    pleco_write_register(ViPipe,0x332E,0x0D);
    pleco_write_register(ViPipe,0x3358,0x06);
    pleco_write_register(ViPipe,0x3359,0xE1);
    pleco_write_register(ViPipe,0x335A,0x11);
    pleco_write_register(ViPipe,0x3360,0x1E);
    pleco_write_register(ViPipe,0x3361,0x61);
    pleco_write_register(ViPipe,0x3362,0x10);
    pleco_write_register(ViPipe,0x33B0,0x50);
    pleco_write_register(ViPipe,0x33B2,0x1A);
    pleco_write_register(ViPipe,0x33B3,0x04);
    pleco_write_register(ViPipe,0x3405,0x00);
    pleco_write_register(ViPipe,0x3407,0x01);
    pleco_write_register(ViPipe,0x3414,0x0A);
    pleco_write_register(ViPipe,0x3415,0x00);
    pleco_write_register(ViPipe,0x3418,0x76); //pic height 2678
    pleco_write_register(ViPipe,0x3419,0x0a); //pic height
    pleco_write_register(ViPipe,0x3441,0x0C);
    pleco_write_register(ViPipe,0x3442,0x0C);
    pleco_write_register(ViPipe,0x3443,0x01);
    pleco_write_register(ViPipe,0x3444,0x20);
    pleco_write_register(ViPipe,0x3445,0x25);
    pleco_write_register(ViPipe,0x3446,0x77);
    pleco_write_register(ViPipe,0x3447,0x00);
    pleco_write_register(ViPipe,0x3448,0x67);
    pleco_write_register(ViPipe,0x3449,0x00);
    pleco_write_register(ViPipe,0x344A,0x47);
    pleco_write_register(ViPipe,0x344B,0x00);
    pleco_write_register(ViPipe,0x344C,0x37);
    pleco_write_register(ViPipe,0x344D,0x00);
    pleco_write_register(ViPipe,0x344E,0x3F);
    pleco_write_register(ViPipe,0x344F,0x00);
    pleco_write_register(ViPipe,0x3450,0xFF);
    pleco_write_register(ViPipe,0x3451,0x00);
    pleco_write_register(ViPipe,0x3452,0x3F);
    pleco_write_register(ViPipe,0x3453,0x00);
    pleco_write_register(ViPipe,0x3454,0x37);
    pleco_write_register(ViPipe,0x3472,0xA0);  //pic width 1952
    pleco_write_register(ViPipe,0x3473,0x07);  //pic width
    pleco_write_register(ViPipe,0x347B,0x23);
    pleco_write_register(ViPipe,0x3010,0x61); //set dol2 long exp gain enable
    pleco_write_register(ViPipe,0x30f0,0x64); //set dol2 shot exp gain enable


    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco sensor 1080P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    return;

}

void pleco_wdr_1080p30_3to1_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe, 0x3000, 0x01); /* standby */
    pleco_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //12bit
    pleco_write_register (ViPipe, 0x3007, 0x00);
    pleco_write_register (ViPipe, 0x300c, 0x21);
    pleco_write_register (ViPipe, 0x300f, 0x00);
    pleco_write_register (ViPipe, 0x3010, 0x21);
    pleco_write_register (ViPipe, 0x3012, 0x64);
    pleco_write_register (ViPipe, 0x3016, 0x09);
    pleco_write_register (ViPipe, 0x3020, 0x04);
    pleco_write_register (ViPipe, 0x3021, 0x00);
    pleco_write_register (ViPipe, 0x3024, 0xF2);
    pleco_write_register (ViPipe, 0x3025, 0x01);
    pleco_write_register (ViPipe, 0x3028, 0x57);
    pleco_write_register (ViPipe, 0x3029, 0x02);
    pleco_write_register (ViPipe, 0x3030, 0xED);
    pleco_write_register (ViPipe, 0x3031, 0x01);
    pleco_write_register (ViPipe, 0x3034, 0x30);
    pleco_write_register (ViPipe, 0x3035, 0x02);
    pleco_write_register (ViPipe, 0x3045, 0x05);
    pleco_write_register (ViPipe, 0x304b, 0x0a);
    pleco_write_register (ViPipe, 0x305c, 0x18);
    pleco_write_register (ViPipe, 0x305d, 0x03);
    pleco_write_register (ViPipe, 0x305e, 0x20);
    pleco_write_register (ViPipe, 0x305f, 0x01);
    pleco_write_register (ViPipe, 0x3070, 0x02);
    pleco_write_register (ViPipe, 0x3071, 0x11);
    pleco_write_register (ViPipe, 0x309b, 0x10);
    pleco_write_register (ViPipe, 0x309c, 0x22);
    pleco_write_register (ViPipe, 0x30a2, 0x02);
    pleco_write_register (ViPipe, 0x30a6, 0x20);
    pleco_write_register (ViPipe, 0x30a8, 0x20);
    pleco_write_register (ViPipe, 0x30aa, 0x20);
    pleco_write_register (ViPipe, 0x30ac, 0x20);
    pleco_write_register (ViPipe, 0x30b0, 0x43);
    pleco_write_register (ViPipe, 0x3106, 0x33);
    pleco_write_register (ViPipe, 0x3119, 0x9e);
    pleco_write_register (ViPipe, 0x311c, 0x1e);
    pleco_write_register (ViPipe, 0x311e, 0x08);
    pleco_write_register (ViPipe, 0x3128, 0x05);
    pleco_write_register (ViPipe, 0x313d, 0x83);
    pleco_write_register (ViPipe, 0x3150, 0x03);
    pleco_write_register (ViPipe, 0x315e, 0x1a);
    pleco_write_register (ViPipe, 0x3164, 0x1a);
    pleco_write_register (ViPipe, 0x317e, 0x00);
    pleco_write_register (ViPipe, 0x32b8, 0x50);
    pleco_write_register (ViPipe, 0x32b9, 0x10);
    pleco_write_register (ViPipe, 0x32ba, 0x00);
    pleco_write_register (ViPipe, 0x32bb, 0x04);
    pleco_write_register (ViPipe, 0x32c8, 0x50);
    pleco_write_register (ViPipe, 0x32c9, 0x10);
    pleco_write_register (ViPipe, 0x32ca, 0x00);
    pleco_write_register (ViPipe, 0x32cb, 0x04);
    pleco_write_register (ViPipe, 0x332c, 0xd3);
    pleco_write_register (ViPipe, 0x332d, 0x10);
    pleco_write_register (ViPipe, 0x332e, 0x0d);
    pleco_write_register (ViPipe, 0x3358, 0x06);
    pleco_write_register (ViPipe, 0x3359, 0xe1);
    pleco_write_register (ViPipe, 0x335a, 0x11);
    pleco_write_register (ViPipe, 0x3360, 0x1e);
    pleco_write_register (ViPipe, 0x3361, 0x61);
    pleco_write_register (ViPipe, 0x3362, 0x10);
    pleco_write_register (ViPipe, 0x33b0, 0x50);
    pleco_write_register (ViPipe, 0x33b2, 0x1a);
    pleco_write_register (ViPipe, 0x33b3, 0x04);

    pleco_write_register (ViPipe, 0x3418, 0x24); /**** Y_OUT_SIZE *****/
    pleco_write_register (ViPipe, 0x3419, 0x0F);

    pleco_write_register (ViPipe, 0x3444, 0x20);
    pleco_write_register (ViPipe, 0x3445, 0x25);
    pleco_write_register (ViPipe, 0x3480, 0x49);

    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco pleco 1080P15fps 12bit 3to1 WDR(30fps->7p5fps) init success!=====\n");

    return;

}

void pleco_wdr_720p60_2to1_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe, 0x3000, 0x01); /* standby */
    pleco_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //12bit
    pleco_write_register (ViPipe, 0x3005, 0x01);
    pleco_write_register (ViPipe, 0x3007, 0x10);
    pleco_write_register (ViPipe, 0x3009, 0x01);
    pleco_write_register (ViPipe, 0x300a, 0xf0);
    pleco_write_register (ViPipe, 0x300c, 0x11);
    pleco_write_register (ViPipe, 0x300f, 0x00);
    pleco_write_register (ViPipe, 0x3010, 0x21);
    pleco_write_register (ViPipe, 0x3012, 0x64);
    pleco_write_register (ViPipe, 0x3016, 0x09);
    pleco_write_register (ViPipe, 0x3018, 0xee);
    pleco_write_register (ViPipe, 0x3019, 0x02);
    pleco_write_register (ViPipe, 0x301c, 0xe4);
    pleco_write_register (ViPipe, 0x301d, 0x0c);
    pleco_write_register (ViPipe, 0x3045, 0x05);
    pleco_write_register (ViPipe, 0x3046, 0x01);
    pleco_write_register (ViPipe, 0x304b, 0x0a);

    pleco_write_register (ViPipe, 0x305c, 0x20); //INCKSEL1
    pleco_write_register (ViPipe, 0x305d, 0x03); //INCKSEL2
    pleco_write_register (ViPipe, 0x305e, 0x20); //INCKSEL3
    pleco_write_register (ViPipe, 0x305f, 0x01); //INCKSEL4

    pleco_write_register (ViPipe, 0x3070, 0x02);
    pleco_write_register (ViPipe, 0x3071, 0x11);
    pleco_write_register (ViPipe, 0x309b, 0x10);
    pleco_write_register (ViPipe, 0x309c, 0x22);
    pleco_write_register (ViPipe, 0x30a2, 0x02);
    pleco_write_register (ViPipe, 0x30a6, 0x20);
    pleco_write_register (ViPipe, 0x30a8, 0x20);
    pleco_write_register (ViPipe, 0x30aa, 0x20);
    pleco_write_register (ViPipe, 0x30ac, 0x20);
    pleco_write_register (ViPipe, 0x30b0, 0x43);

    //Add
    pleco_write_register (ViPipe, 0x3106, 0x11);

    pleco_write_register (ViPipe, 0x3119, 0x9e);
    pleco_write_register (ViPipe, 0x311c, 0x1e);
    pleco_write_register (ViPipe, 0x311e, 0x08);
    pleco_write_register (ViPipe, 0x3128, 0x05);
    pleco_write_register (ViPipe, 0x3129, 0x00);
    pleco_write_register (ViPipe, 0x313d, 0x83);
    pleco_write_register (ViPipe, 0x3150, 0x03);
    pleco_write_register (ViPipe, 0x315e, 0x1a);
    pleco_write_register (ViPipe, 0x3164, 0x1a);
    pleco_write_register (ViPipe, 0x317c, 0x00);
    pleco_write_register (ViPipe, 0x317e, 0x00);
    pleco_write_register (ViPipe, 0x31ec, 0x00);

    pleco_write_register (ViPipe, 0x32b8, 0x50);
    pleco_write_register (ViPipe, 0x32b9, 0x10);
    pleco_write_register (ViPipe, 0x32ba, 0x00);
    pleco_write_register (ViPipe, 0x32bb, 0x04);
    pleco_write_register (ViPipe, 0x32c8, 0x50);
    pleco_write_register (ViPipe, 0x32c9, 0x10);
    pleco_write_register (ViPipe, 0x32ca, 0x00);
    pleco_write_register (ViPipe, 0x32cb, 0x04);

    pleco_write_register (ViPipe, 0x332c, 0xd3);
    pleco_write_register (ViPipe, 0x332d, 0x10);
    pleco_write_register (ViPipe, 0x332e, 0x0d);
    pleco_write_register (ViPipe, 0x3358, 0x06);
    pleco_write_register (ViPipe, 0x3359, 0xe1);
    pleco_write_register (ViPipe, 0x335a, 0x11);
    pleco_write_register (ViPipe, 0x3360, 0x1e);
    pleco_write_register (ViPipe, 0x3361, 0x61);
    pleco_write_register (ViPipe, 0x3362, 0x10);
    pleco_write_register (ViPipe, 0x33b0, 0x50);
    pleco_write_register (ViPipe, 0x33b2, 0x1a);
    pleco_write_register (ViPipe, 0x33b3, 0x04);

    pleco_write_register (ViPipe, 0x3405, 0x10);
    pleco_write_register (ViPipe, 0x3407, 0x03);
    pleco_write_register (ViPipe, 0x3414, 0x04);
    pleco_write_register (ViPipe, 0x3418, 0xc6);
    pleco_write_register (ViPipe, 0x3419, 0x05);
    pleco_write_register (ViPipe, 0x3441, 0x0c);
    pleco_write_register (ViPipe, 0x3442, 0x0c);
    pleco_write_register (ViPipe, 0x3443, 0x03);
    pleco_write_register (ViPipe, 0x3444, 0x20);
    pleco_write_register (ViPipe, 0x3445, 0x25);
    pleco_write_register (ViPipe, 0x3446, 0x4f);
    pleco_write_register (ViPipe, 0x3447, 0x00);
    pleco_write_register (ViPipe, 0x3448, 0x2f);
    pleco_write_register (ViPipe, 0x3449, 0x00);
    pleco_write_register (ViPipe, 0x344a, 0x17);
    pleco_write_register (ViPipe, 0x344b, 0x00);
    pleco_write_register (ViPipe, 0x344c, 0x17);
    pleco_write_register (ViPipe, 0x344d, 0x00);
    pleco_write_register (ViPipe, 0x344e, 0x17);
    pleco_write_register (ViPipe, 0x344f, 0x00);
    pleco_write_register (ViPipe, 0x3450, 0x57);
    pleco_write_register (ViPipe, 0x3451, 0x00);
    pleco_write_register (ViPipe, 0x3452, 0x17);
    pleco_write_register (ViPipe, 0x3453, 0x00);
    pleco_write_register (ViPipe, 0x3454, 0x17);
    pleco_write_register (ViPipe, 0x3455, 0x00);
    pleco_write_register (ViPipe, 0x3472, 0x1c);
    pleco_write_register (ViPipe, 0x3473, 0x05);
    pleco_write_register (ViPipe, 0x3480, 0x49);

    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco pleco 720P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    return;
}

void pleco_wdr_720p60_3to1_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe, 0x3000, 0x01); /* standby */
    pleco_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    //12bit
    pleco_write_register (ViPipe, 0x3005, 0x01);
    pleco_write_register (ViPipe, 0x3007, 0x10);
    pleco_write_register (ViPipe, 0x3009, 0x01);
    pleco_write_register (ViPipe, 0x300a, 0xf0);
    pleco_write_register (ViPipe, 0x300c, 0x31);
    pleco_write_register (ViPipe, 0x300f, 0x00);
    pleco_write_register (ViPipe, 0x3010, 0x21);
    pleco_write_register (ViPipe, 0x3012, 0x64);
    pleco_write_register (ViPipe, 0x3016, 0x09);
    pleco_write_register (ViPipe, 0x3018, 0xee);
    pleco_write_register (ViPipe, 0x3019, 0x02);
    pleco_write_register (ViPipe, 0x301c, 0xe4);
    pleco_write_register (ViPipe, 0x301d, 0x0c);
    pleco_write_register (ViPipe, 0x3045, 0x05);
    pleco_write_register (ViPipe, 0x3046, 0x01);
    pleco_write_register (ViPipe, 0x304b, 0x0a);

    pleco_write_register (ViPipe, 0x305c, 0x20); //INCKSEL1
    pleco_write_register (ViPipe, 0x305d, 0x03); //INCKSEL2
    pleco_write_register (ViPipe, 0x305e, 0x20); //INCKSEL3
    pleco_write_register (ViPipe, 0x305f, 0x01); //INCKSEL4

    pleco_write_register (ViPipe, 0x3070, 0x02);
    pleco_write_register (ViPipe, 0x3071, 0x11);
    pleco_write_register (ViPipe, 0x309b, 0x10);
    pleco_write_register (ViPipe, 0x309c, 0x22);
    pleco_write_register (ViPipe, 0x30a2, 0x02);
    pleco_write_register (ViPipe, 0x30a6, 0x20);
    pleco_write_register (ViPipe, 0x30a8, 0x20);
    pleco_write_register (ViPipe, 0x30aa, 0x20);
    pleco_write_register (ViPipe, 0x30ac, 0x20);
    pleco_write_register (ViPipe, 0x30b0, 0x43);

    //Add
    pleco_write_register (ViPipe, 0x3106, 0x33);

    pleco_write_register (ViPipe, 0x3119, 0x9e);
    pleco_write_register (ViPipe, 0x311c, 0x1e);
    pleco_write_register (ViPipe, 0x311e, 0x08);
    pleco_write_register (ViPipe, 0x3128, 0x05);
    pleco_write_register (ViPipe, 0x3129, 0x00);
    pleco_write_register (ViPipe, 0x313d, 0x83);
    pleco_write_register (ViPipe, 0x3150, 0x03);
    pleco_write_register (ViPipe, 0x315e, 0x1a);
    pleco_write_register (ViPipe, 0x3164, 0x1a);
    pleco_write_register (ViPipe, 0x317c, 0x00);
    pleco_write_register (ViPipe, 0x317e, 0x00);
    pleco_write_register (ViPipe, 0x31ec, 0x00);

    pleco_write_register (ViPipe, 0x32b8, 0x50);
    pleco_write_register (ViPipe, 0x32b9, 0x10);
    pleco_write_register (ViPipe, 0x32ba, 0x00);
    pleco_write_register (ViPipe, 0x32bb, 0x04);
    pleco_write_register (ViPipe, 0x32c8, 0x50);
    pleco_write_register (ViPipe, 0x32c9, 0x10);
    pleco_write_register (ViPipe, 0x32ca, 0x00);
    pleco_write_register (ViPipe, 0x32cb, 0x04);

    pleco_write_register (ViPipe, 0x332c, 0xd3);
    pleco_write_register (ViPipe, 0x332d, 0x10);
    pleco_write_register (ViPipe, 0x332e, 0x0d);
    pleco_write_register (ViPipe, 0x3358, 0x06);
    pleco_write_register (ViPipe, 0x3359, 0xe1);
    pleco_write_register (ViPipe, 0x335a, 0x11);
    pleco_write_register (ViPipe, 0x3360, 0x1e);
    pleco_write_register (ViPipe, 0x3361, 0x61);
    pleco_write_register (ViPipe, 0x3362, 0x10);
    pleco_write_register (ViPipe, 0x33b0, 0x50);
    pleco_write_register (ViPipe, 0x33b2, 0x1a);
    pleco_write_register (ViPipe, 0x33b3, 0x04);

    pleco_write_register (ViPipe, 0x3405, 0x10);
    pleco_write_register (ViPipe, 0x3407, 0x03);
    pleco_write_register (ViPipe, 0x3414, 0x04);
    pleco_write_register (ViPipe, 0x3418, 0xb5);
    pleco_write_register (ViPipe, 0x3419, 0x08);
    pleco_write_register (ViPipe, 0x3441, 0x0c);
    pleco_write_register (ViPipe, 0x3442, 0x0c);
    pleco_write_register (ViPipe, 0x3443, 0x03);
    pleco_write_register (ViPipe, 0x3444, 0x20);
    pleco_write_register (ViPipe, 0x3445, 0x25);
    pleco_write_register (ViPipe, 0x3446, 0x4f);
    pleco_write_register (ViPipe, 0x3447, 0x00);
    pleco_write_register (ViPipe, 0x3448, 0x2f);
    pleco_write_register (ViPipe, 0x3449, 0x00);
    pleco_write_register (ViPipe, 0x344a, 0x17);
    pleco_write_register (ViPipe, 0x344b, 0x00);
    pleco_write_register (ViPipe, 0x344c, 0x17);
    pleco_write_register (ViPipe, 0x344d, 0x00);
    pleco_write_register (ViPipe, 0x344e, 0x17);
    pleco_write_register (ViPipe, 0x344f, 0x00);
    pleco_write_register (ViPipe, 0x3450, 0x57);
    pleco_write_register (ViPipe, 0x3451, 0x00);
    pleco_write_register (ViPipe, 0x3452, 0x17);
    pleco_write_register (ViPipe, 0x3453, 0x00);
    pleco_write_register (ViPipe, 0x3454, 0x17);
    pleco_write_register (ViPipe, 0x3455, 0x00);
    pleco_write_register (ViPipe, 0x3472, 0x1c);
    pleco_write_register (ViPipe, 0x3473, 0x05);
    pleco_write_register (ViPipe, 0x3480, 0x49);

    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco pleco 720P15fps 12bit 3to1 WDR(60fps->15fps) init success!=====\n");
    return;
}

void pleco_wdr_1080p120_2to1_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe, 0x3000, 0x01); /* standby */
    pleco_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    pleco_write_register (ViPipe, 0x3005, 0x00);
    pleco_write_register (ViPipe, 0x3007, 0x00);
    pleco_write_register (ViPipe, 0x3009, 0x00);
    pleco_write_register (ViPipe, 0x300a, 0x3c);
    pleco_write_register (ViPipe, 0x300c, 0x11);
    pleco_write_register (ViPipe, 0x300f, 0x00);
    pleco_write_register (ViPipe, 0x3010, 0x21);
    pleco_write_register (ViPipe, 0x3012, 0x64);
    pleco_write_register (ViPipe, 0x3016, 0x09);
    pleco_write_register (ViPipe, 0x3018, 0x65);
    pleco_write_register (ViPipe, 0x3019, 0x04);
    pleco_write_register (ViPipe, 0x301c, 0xF6);
    pleco_write_register (ViPipe, 0x301d, 0x03);
    pleco_write_register (ViPipe, 0x3020, 0x02);
    pleco_write_register (ViPipe, 0x3024, 0xc9);
    pleco_write_register (ViPipe, 0x3030, 0x0b);
    pleco_write_register (ViPipe, 0x3045, 0x05);
    pleco_write_register (ViPipe, 0x3046, 0x00);
    pleco_write_register (ViPipe, 0x304b, 0x0a);
    pleco_write_register (ViPipe, 0x305c, 0x18);
    pleco_write_register (ViPipe, 0x305d, 0x03);
    pleco_write_register (ViPipe, 0x305e, 0x20);
    pleco_write_register (ViPipe, 0x305f, 0x01);
    pleco_write_register (ViPipe, 0x3070, 0x02);
    pleco_write_register (ViPipe, 0x3071, 0x11);
    pleco_write_register (ViPipe, 0x309b, 0x10);
    pleco_write_register (ViPipe, 0x309c, 0x22);
    pleco_write_register (ViPipe, 0x30a2, 0x02);
    pleco_write_register (ViPipe, 0x30a6, 0x20);
    pleco_write_register (ViPipe, 0x30a8, 0x20);
    pleco_write_register (ViPipe, 0x30aa, 0x20);
    pleco_write_register (ViPipe, 0x30ac, 0x20);
    pleco_write_register (ViPipe, 0x30b0, 0x43);
    pleco_write_register (ViPipe, 0x3106, 0x11);
    pleco_write_register (ViPipe, 0x3119, 0x9e);
    pleco_write_register (ViPipe, 0x311c, 0x1e);
    pleco_write_register (ViPipe, 0x311e, 0x08);
    pleco_write_register (ViPipe, 0x3128, 0x05);
    pleco_write_register (ViPipe, 0x3129, 0x1d);
    pleco_write_register (ViPipe, 0x313d, 0x83);
    pleco_write_register (ViPipe, 0x3150, 0x03);
    pleco_write_register (ViPipe, 0x315e, 0x1a);
    pleco_write_register (ViPipe, 0x3164, 0x1a);
    pleco_write_register (ViPipe, 0x317c, 0x12);
    pleco_write_register (ViPipe, 0x317e, 0x00);
    pleco_write_register (ViPipe, 0x31ec, 0x37);

    pleco_write_register (ViPipe, 0x32b8, 0x50);
    pleco_write_register (ViPipe, 0x32b9, 0x10);
    pleco_write_register (ViPipe, 0x32ba, 0x00);
    pleco_write_register (ViPipe, 0x32bb, 0x04);
    pleco_write_register (ViPipe, 0x32c8, 0x50);
    pleco_write_register (ViPipe, 0x32c9, 0x10);
    pleco_write_register (ViPipe, 0x32ca, 0x00);
    pleco_write_register (ViPipe, 0x32cb, 0x04);

    pleco_write_register (ViPipe, 0x332c, 0xd3);
    pleco_write_register (ViPipe, 0x332d, 0x10);
    pleco_write_register (ViPipe, 0x332e, 0x0d);
    pleco_write_register (ViPipe, 0x3358, 0x06);
    pleco_write_register (ViPipe, 0x3359, 0xe1);
    pleco_write_register (ViPipe, 0x335a, 0x11);
    pleco_write_register (ViPipe, 0x3360, 0x1e);
    pleco_write_register (ViPipe, 0x3361, 0x61);
    pleco_write_register (ViPipe, 0x3362, 0x10);
    pleco_write_register (ViPipe, 0x33b0, 0x50);
    pleco_write_register (ViPipe, 0x33b2, 0x1a);
    pleco_write_register (ViPipe, 0x33b3, 0x04);

    pleco_write_register (ViPipe, 0x3405, 0x00);
    pleco_write_register (ViPipe, 0x3407, 0x03);
    pleco_write_register (ViPipe, 0x3414, 0x0a);
    pleco_write_register (ViPipe, 0x3418, 0xb2);
    pleco_write_register (ViPipe, 0x3419, 0x08);
    pleco_write_register (ViPipe, 0x3441, 0x0a);
    pleco_write_register (ViPipe, 0x3442, 0x0a);
    pleco_write_register (ViPipe, 0x3443, 0x03);
    pleco_write_register (ViPipe, 0x3444, 0x20);
    pleco_write_register (ViPipe, 0x3445, 0x25);
    pleco_write_register (ViPipe, 0x3446, 0x77);
    pleco_write_register (ViPipe, 0x3447, 0x00);
    pleco_write_register (ViPipe, 0x3448, 0x67);
    pleco_write_register (ViPipe, 0x3449, 0x00);
    pleco_write_register (ViPipe, 0x344a, 0x47);
    pleco_write_register (ViPipe, 0x344b, 0x00);
    pleco_write_register (ViPipe, 0x344c, 0x37);
    pleco_write_register (ViPipe, 0x344d, 0x00);
    pleco_write_register (ViPipe, 0x344e, 0x3f);
    pleco_write_register (ViPipe, 0x344f, 0x00);
    pleco_write_register (ViPipe, 0x3450, 0xff);
    pleco_write_register (ViPipe, 0x3451, 0x00);
    pleco_write_register (ViPipe, 0x3452, 0x3f);
    pleco_write_register (ViPipe, 0x3453, 0x00);
    pleco_write_register (ViPipe, 0x3454, 0x37);
    pleco_write_register (ViPipe, 0x3455, 0x00);
    pleco_write_register (ViPipe, 0x3472, 0x9c);
    pleco_write_register (ViPipe, 0x3473, 0x07);
    pleco_write_register (ViPipe, 0x3480, 0x49);

    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco pleco 1080P60fps 10bit 2to1 WDR(120fps->60fps) init success!=====\n");
    return;
}

void pleco_wdr_1080p120_3to1_init(VI_PIPE ViPipe)
{
    pleco_write_register (ViPipe, 0x3000, 0x01); /* standby */
    pleco_write_register (ViPipe, 0x3002, 0x01); /* XTMSTA */

    pleco_write_register (ViPipe, 0x3005, 0x00);
    pleco_write_register (ViPipe, 0x3007, 0x00);
    pleco_write_register (ViPipe, 0x3009, 0x00);
    pleco_write_register (ViPipe, 0x300a, 0x3c);
    pleco_write_register (ViPipe, 0x300c, 0x21);
    pleco_write_register (ViPipe, 0x300f, 0x00);
    pleco_write_register (ViPipe, 0x3010, 0x21);
    pleco_write_register (ViPipe, 0x3012, 0x64);
    pleco_write_register (ViPipe, 0x3016, 0x09);
    pleco_write_register (ViPipe, 0x3018, 0x65);
    pleco_write_register (ViPipe, 0x3019, 0x04);
    pleco_write_register (ViPipe, 0x301c, 0x4c);
    pleco_write_register (ViPipe, 0x301d, 0x04);

#if 1
    pleco_write_register (ViPipe, 0x3020, 0x04); /* SHS1 */
    pleco_write_register (ViPipe, 0x3021, 0x00);
    //pleco_write_register (ViPipe,0x3022, 0x00);
    pleco_write_register (ViPipe, 0x3024, 0xf2); /* SHS2 */
    pleco_write_register (ViPipe, 0x3025, 0x01);
    //pleco_write_register (ViPipe,0x3025, 0x00);
    pleco_write_register (ViPipe, 0x3028, 0x57); /* SHS3 */
    pleco_write_register (ViPipe, 0x3029, 0x02);
    //pleco_write_register (ViPipe,0x302A, 0x00);
    pleco_write_register (ViPipe, 0x3030, 0xed); /* RHS1 */
    pleco_write_register (ViPipe, 0x3031, 0x01);
    pleco_write_register (ViPipe, 0x3034, 0x30); /* RHS2 */
    pleco_write_register (ViPipe, 0x3035, 0x02);
#else
    pleco_write_register (ViPipe, 0x3020, 0x04);
    pleco_write_register (ViPipe, 0x3024, 0x89);
    pleco_write_register (ViPipe, 0x3028, 0x93);
    pleco_write_register (ViPipe, 0x3029, 0x01);
    pleco_write_register (ViPipe, 0x3030, 0x85);
    pleco_write_register (ViPipe, 0x3034, 0x92);
#endif

    pleco_write_register (ViPipe, 0x3045, 0x05);
    pleco_write_register (ViPipe, 0x3046, 0x00);
    pleco_write_register (ViPipe, 0x304b, 0x0a);
    pleco_write_register (ViPipe, 0x305c, 0x18);
    pleco_write_register (ViPipe, 0x305d, 0x03);
    pleco_write_register (ViPipe, 0x305e, 0x20);
    pleco_write_register (ViPipe, 0x305f, 0x01);
    pleco_write_register (ViPipe, 0x3070, 0x02);
    pleco_write_register (ViPipe, 0x3071, 0x11);
    pleco_write_register (ViPipe, 0x309b, 0x10);
    pleco_write_register (ViPipe, 0x309c, 0x22);
    pleco_write_register (ViPipe, 0x30a2, 0x02);
    pleco_write_register (ViPipe, 0x30a6, 0x20);
    pleco_write_register (ViPipe, 0x30a8, 0x20);
    pleco_write_register (ViPipe, 0x30aa, 0x20);
    pleco_write_register (ViPipe, 0x30ac, 0x20);
    pleco_write_register (ViPipe, 0x30b0, 0x43);
    pleco_write_register (ViPipe, 0x3106, 0x33);
    pleco_write_register (ViPipe, 0x3119, 0x9e);
    pleco_write_register (ViPipe, 0x311c, 0x1e);
    pleco_write_register (ViPipe, 0x311e, 0x08);
    pleco_write_register (ViPipe, 0x3128, 0x05);
    pleco_write_register (ViPipe, 0x3129, 0x1d);
    pleco_write_register (ViPipe, 0x313d, 0x83);
    pleco_write_register (ViPipe, 0x3150, 0x03);
    pleco_write_register (ViPipe, 0x315e, 0x1a);
    pleco_write_register (ViPipe, 0x3164, 0x1a);
    pleco_write_register (ViPipe, 0x317c, 0x12);
    pleco_write_register (ViPipe, 0x317e, 0x00);
    pleco_write_register (ViPipe, 0x31ec, 0x37);

    pleco_write_register (ViPipe, 0x32b8, 0x50);
    pleco_write_register (ViPipe, 0x32b9, 0x10);
    pleco_write_register (ViPipe, 0x32ba, 0x00);
    pleco_write_register (ViPipe, 0x32bb, 0x04);
    pleco_write_register (ViPipe, 0x32c8, 0x50);
    pleco_write_register (ViPipe, 0x32c9, 0x10);
    pleco_write_register (ViPipe, 0x32ca, 0x00);
    pleco_write_register (ViPipe, 0x32cb, 0x04);

    pleco_write_register (ViPipe, 0x332c, 0xd3);
    pleco_write_register (ViPipe, 0x332d, 0x10);
    pleco_write_register (ViPipe, 0x332e, 0x0d);
    pleco_write_register (ViPipe, 0x3358, 0x06);
    pleco_write_register (ViPipe, 0x3359, 0xe1);
    pleco_write_register (ViPipe, 0x335a, 0x11);
    pleco_write_register (ViPipe, 0x3360, 0x1e);
    pleco_write_register (ViPipe, 0x3361, 0x61);
    pleco_write_register (ViPipe, 0x3362, 0x10);
    pleco_write_register (ViPipe, 0x33b0, 0x50);
    pleco_write_register (ViPipe, 0x33b2, 0x1a);
    pleco_write_register (ViPipe, 0x33b3, 0x04);

    pleco_write_register (ViPipe, 0x3405, 0x00);
    pleco_write_register (ViPipe, 0x3407, 0x03);
    pleco_write_register (ViPipe, 0x3414, 0x0a);
    pleco_write_register (ViPipe, 0x3418, 0x55);
    pleco_write_register (ViPipe, 0x3419, 0x11);
    pleco_write_register (ViPipe, 0x3441, 0x0a);
    pleco_write_register (ViPipe, 0x3442, 0x0a);
    pleco_write_register (ViPipe, 0x3443, 0x03);
    pleco_write_register (ViPipe, 0x3444, 0x20);
    pleco_write_register (ViPipe, 0x3445, 0x25);
    pleco_write_register (ViPipe, 0x3446, 0x77);
    pleco_write_register (ViPipe, 0x3447, 0x00);
    pleco_write_register (ViPipe, 0x3448, 0x67);
    pleco_write_register (ViPipe, 0x3449, 0x00);
    pleco_write_register (ViPipe, 0x344a, 0x47);
    pleco_write_register (ViPipe, 0x344b, 0x00);
    pleco_write_register (ViPipe, 0x344c, 0x37);
    pleco_write_register (ViPipe, 0x344d, 0x00);
    pleco_write_register (ViPipe, 0x344e, 0x3f);
    pleco_write_register (ViPipe, 0x344f, 0x00);
    pleco_write_register (ViPipe, 0x3450, 0xff);
    pleco_write_register (ViPipe, 0x3451, 0x00);
    pleco_write_register (ViPipe, 0x3452, 0x3f);
    pleco_write_register (ViPipe, 0x3453, 0x00);
    pleco_write_register (ViPipe, 0x3454, 0x37);
    pleco_write_register (ViPipe, 0x3455, 0x00);
    pleco_write_register (ViPipe, 0x3472, 0x9c);
    pleco_write_register (ViPipe, 0x3473, 0x07);
    pleco_write_register (ViPipe, 0x3480, 0x49);

    pleco_default_reg_init(ViPipe);
    pleco_write_register (ViPipe, 0x3000, 0x00); /* standby */
    delay_ms(20);
    pleco_write_register (ViPipe, 0x3002, 0x00); /* master mode start */

    ar_always("===Pleco pleco 1080P30fps 10bit 3to1 WDR(120fps->30fps) init success!=====\n");

    return;
}
