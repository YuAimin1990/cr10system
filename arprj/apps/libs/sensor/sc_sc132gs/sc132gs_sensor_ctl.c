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

const unsigned char sc132gs_i2c_addr     =    0x60;  //0x32 for sid low  ; 0x30 for sid high   /* I2C Address of sc132gs */
const unsigned int  sc132gs_addr_byte    =    2;
const unsigned int  sc132gs_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

extern ISP_SNS_STATE_S       *g_pastsc132gs[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U      g_aunsc132gsBusInfo[];

int sc132gs_i2c_init(VI_PIPE ViPipe)
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

    u8DevNum =g_aunsc132gsBusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	printf("u8DevNum=%d %s\n",u8DevNum,acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(AR_DBG_ERR, "Open /dev/ar_i2c_drv-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (sc132gs_i2c_addr >> 1));
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

int sc132gs_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }
    return AR_FAILURE;
}

int sc132gs_read_register(VI_PIPE ViPipe, int addr)
{
    // TODO:
    if (0 > g_fd[ViPipe])
    {
        return -1;
    }

#ifdef AR_GPIO_I2C
    // i2c_data.dev_addr = sc132gs_i2c_addr;
    // i2c_data.reg_addr = addr;
    // i2c_data.addr_byte_num = sc132gs_addr_byte;
    // i2c_data.data = data;
    // i2c_data.data_byte_num = sc132gs_data_byte;

    // ret = ioctl(g_fd[ViPipe], GPIO_I2C_READ, &i2c_data);

    // if (ret)
    // {
    //     ISP_TRACE(AR_DBG_ERR, "GPIO-I2C write faild!\n");
    //     return ret;
    // }
    return -1;
#else
    int idx = 0;
    int ret;
    char buf[8];

    if (sc132gs_addr_byte == 2)
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

    // if (sc132gs_data_byte == 2)
    // {
    //     //buf[idx] = (data >> 8) & 0xff;
    //     //idx++;
    //     //buf[idx] = data & 0xff;
    //     //idx++;
    // }
    // else
    // {
    //     buf[idx] = data & 0xff;
    //     idx++;
    // }

    ret = write(g_fd[ViPipe], buf, sc132gs_addr_byte);
    if (ret <= 0)
    {
        ISP_TRACE(AR_DBG_ERR, "I2C_WRITE error!\n");
        return AR_FAILURE;
    }
    ret = read(g_fd[ViPipe], buf, sc132gs_data_byte);
    if(ret <= 0){
        printf("i2c read error\n");
        return -1;
    }
    return buf[0];

#endif
    return AR_SUCCESS;
}

int sc132gs_write_callback(VI_PIPE ViPipe, int addr, int data)
{
    return AR_SUCCESS;
}


int sc132gs_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return AR_SUCCESS;
    }

#ifdef AR_GPIO_I2C
    i2c_data.dev_addr = sc132gs_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sc132gs_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sc132gs_data_byte;

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

    if (sc132gs_addr_byte == 2)
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

    if (sc132gs_data_byte == 2)
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

    ret = write(g_fd[ViPipe], buf, sc132gs_addr_byte + sc132gs_data_byte);
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

void sc132gs_prog(VI_PIPE ViPipe, int *rom)
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
            sc132gs_write_register(ViPipe, addr, data);
        }
    }
}

void sc132gs_standby(VI_PIPE ViPipe)
{
    // TODO:
    printf("standby pipe %d\n",ViPipe);
    return;
}

static uint16_t reg_list[] = {
    0x0103,//
	0x0100,// // SLEEP ENABLE

    
	0x36e9,// // 
	0x36f9,// // 

	0x3018,// // 

	0x3019,//
	0x301a,// //
	0x3031,//
	0x3032,// //
	0x3038,//
	0x3207,// //
	0x320c,// // line length hi 1500 hmax
	0x320d,// // line length lo
	0x320e,// // frame length hi 2880 vmax
	0x320f,// // frame length lo
	0x3250,//
	0x3251,//
	0x3252,//
	0x3253,// //1e
	0x3254,// //05
	0x3255,// adbit1 12bit
	0x3306,//
	0x330a,// //INCKSEL5
	0x330b,// //INCKSEL6
	0x330f,// //ADBIT2
	0x3314,//
	0x3315,//
	0x3317,//03 -> 01
	0x331f,//
	0x3364,// //38 -> 49
	0x3385,//
	0x3387,// 12bit raw
	0x3389,// 12bit raw
	0x33ab,// 2 lane
	0x33ac,//
	0x33b1,//
	0x33b2,//
	0x33f8,//
	0x33fa,//
	0x3409,//
	0x34f0,//
	0x34f1,//
	0x34f2,//
	0x3622,//
	0x3630,//
	0x3631,//
	0x3632,//
	0x3633,//
	0x3638,//
	0x3639,//
	0x363b,//
    0x363c,//
	0x363d,//
	0x36ea,//
	0x36fa,//
	0x36fb,//
	0x36fd,//
	0x3900,//
	0x3901,//
	0x3902,//
	0x3904,//
	0x3908,//blc 0x0f
	0x391e,//
	0x3e01,//
	0x3e02,//
	0x3e09,//
	0x3e0e,//
	0x3e14,//
	0x3e1e,//
	0x3e26,//
	0x4418,//
	0x4503,//
	0x4837,//
	0x5000,//
	0x540c,//
	0x550f,//
	0x5780,//
	0x5784,//
	0x5785,//
	0x5787,//
	0x5788,//
	0x5789,//
	0x578a,//
	0x578b,//
	0x578c,//
	0x5790,//
	0x5791,//
	0x5792,//
	0x5793,//
	0x5794,//
	0x5795,//
	0x5799,//

	0x3361,//

	0x36e9,//
	0x36f9,//

    0x3208,//
    0x3209,//
    0x320a,//
    0x320b,//

    0x3210,//
    0x3211,//
    0x3212,//
    0x3213,//
};


void sc132gs_restart(VI_PIPE ViPipe)
{
    // TODO:
    // int val;
    
    // printf("dump sc132 regs");
    
    // for(int i = 0; i < sizeof(reg_list)/sizeof(reg_list[0]); i++)
    // {
    //     val = sc132gs_read_register(ViPipe, reg_list[i]);
    //     if(val < 0){
    //         printf("reg[0x%04x]= 0x--\n",reg_list[i]);
    //     }else{
    //         printf("reg[0x%04x]= 0x%02x\n", reg_list[i], val);
    //     }
    // }
    // printf("restart pipe %d\n",ViPipe);
    return;
}

#define sc132gs_FLIP_MIRRO (0x3221)

int sc132gs_flip_off_mirro_off(VI_PIPE ViPipe)
{
    sc132gs_write_register(ViPipe, sc132gs_FLIP_MIRRO, 0x00);
    return 0;
}

int sc132gs_flip_on_mirro_off(VI_PIPE ViPipe)
{
    sc132gs_write_register(ViPipe, sc132gs_FLIP_MIRRO, 0x60);
    return 0;
}

int sc132gs_flip_off_mirro_on(VI_PIPE ViPipe)
{
    sc132gs_write_register(ViPipe, sc132gs_FLIP_MIRRO, 0x06);
    return 0;
}

int sc132gs_flip_on_mirro_on(VI_PIPE ViPipe)
{
    sc132gs_write_register(ViPipe, sc132gs_FLIP_MIRRO, 0x66);
    return 0;
}

#define sc132gs_SENSOR_9K_20FPS_LINEAR_MODE      (1)


void c132gs_linear_1080p30_init(VI_PIPE ViPipe);

void sc132gs_default_reg_init(VI_PIPE ViPipe)
{
    AR_U32 i;
    for (i = 0; i < g_pastsc132gs[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        //printf("set reg[0x%x]: 0x%x \n", g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        if (g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].bUpdate)
        {
            //printf("set reg[0x%x]: 0x%x \n", g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr,  g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
            sc132gs_write_register(ViPipe, g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
        }
    }
}

void c132gs_linear_980x1280p30_init(VI_PIPE ViPipe);

void sc132gs_init(VI_PIPE ViPipe)
{
    // WDR_MODE_E       enWDRMode;
    AR_BOOL          bInit;
    // AR_U8            u8ImgMode;

    bInit       = g_pastsc132gs[ViPipe]->bInit;
    // enWDRMode   = g_pastsc132gs[ViPipe]->enWDRMode;
    // u8ImgMode   = g_pastsc132gs[ViPipe]->u8ImgMode;

    sc132gs_i2c_init(ViPipe);

    /* When sensor first init, config all registers */
    if (AR_FALSE == bInit)
    {       
            c132gs_linear_980x1280p30_init(ViPipe);
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else
    {     
            c132gs_linear_980x1280p30_init(ViPipe);
    }
    // if(ViPipe > 0){
    //     sleep(3);
    //     // sc132gs_write_register(ViPipe,0x0103,0x01);
    //     sc132gs_write_register(ViPipe,0x0100,0x00); // ADBIT 12
    //     delay_ms(10);
    //     sc132gs_write_register (ViPipe,0x0100, 0x01);
    //     printf("reinit settings\n");
    // }

    // for (i = 0; i < g_pastsc132gs[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    // {
    //     sc132gs_write_register(ViPipe, g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastsc132gs[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    // }

    g_pastsc132gs[ViPipe]->bInit = AR_TRUE;
    return ;
}

void sc132gs_exit(VI_PIPE ViPipe)
{
    sc132gs_i2c_exit(ViPipe);

    return;
}


void c132gs_linear_980x1280p30_init(VI_PIPE ViPipe)
{
    sc132gs_write_register(ViPipe,0x0103,0x01);
	sc132gs_write_register(ViPipe,0x0100,0x00); // SLEEP ENABLE

    // PLL bypass
	sc132gs_write_register(ViPipe,0x36e9,0x80); // 
	sc132gs_write_register(ViPipe,0x36f9,0x80); // 

	sc132gs_write_register(ViPipe,0x3018,0x32); // 

	sc132gs_write_register(ViPipe,0x3019,0x0c);
	sc132gs_write_register(ViPipe,0x301a,0xb4); //
	sc132gs_write_register(ViPipe,0x3031,0x08);
	sc132gs_write_register(ViPipe,0x3032,0x60); //
	sc132gs_write_register(ViPipe,0x3038,0x44);
	sc132gs_write_register(ViPipe,0x3207,0x17); //
	sc132gs_write_register(ViPipe,0x320c,0x05); // line length hi 1500 hmax
	sc132gs_write_register(ViPipe,0x320d,0xdc); // line length lo
	sc132gs_write_register(ViPipe,0x320e,0x09); // frame length hi 2880 vmax
	sc132gs_write_register(ViPipe,0x320f,0x60); // frame length lo
	sc132gs_write_register(ViPipe,0x3250,0xcc);
	sc132gs_write_register(ViPipe,0x3251,0x02);
	sc132gs_write_register(ViPipe,0x3252,0x09);
	sc132gs_write_register(ViPipe,0x3253,0x5b); //1e
	sc132gs_write_register(ViPipe,0x3254,0x05); //05
	sc132gs_write_register(ViPipe,0x3255,0x3b);// adbit1 12bit
	sc132gs_write_register(ViPipe,0x3306,0x78);
	sc132gs_write_register(ViPipe,0x330a,0x00); //INCKSEL5
	sc132gs_write_register(ViPipe,0x330b,0xc8); //INCKSEL6
	sc132gs_write_register(ViPipe,0x330f,0x24); //ADBIT2
	sc132gs_write_register(ViPipe,0x3314,0x80);
	sc132gs_write_register(ViPipe,0x3315,0x40);
	sc132gs_write_register(ViPipe,0x3317,0xf0);//03 -> 01
	sc132gs_write_register(ViPipe,0x331f,0x12);
	sc132gs_write_register(ViPipe,0x3364,0x00); //38 -> 49
	sc132gs_write_register(ViPipe,0x3385,0x41);
	sc132gs_write_register(ViPipe,0x3387,0x41);// 12bit raw
	sc132gs_write_register(ViPipe,0x3389,0x09);// 12bit raw
	sc132gs_write_register(ViPipe,0x33ab,0x00);// 2 lane
	sc132gs_write_register(ViPipe,0x33ac,0x00);
	sc132gs_write_register(ViPipe,0x33b1,0x03);
	sc132gs_write_register(ViPipe,0x33b2,0x12);
	sc132gs_write_register(ViPipe,0x33f8,0x02);
	sc132gs_write_register(ViPipe,0x33fa,0x01);
	sc132gs_write_register(ViPipe,0x3409,0x08);
	sc132gs_write_register(ViPipe,0x34f0,0xc0);
	sc132gs_write_register(ViPipe,0x34f1,0x20);
	sc132gs_write_register(ViPipe,0x34f2,0x03);
	sc132gs_write_register(ViPipe,0x3622,0xf5);
	sc132gs_write_register(ViPipe,0x3630,0x5c);
	sc132gs_write_register(ViPipe,0x3631,0x80);
	sc132gs_write_register(ViPipe,0x3632,0xc8);
	sc132gs_write_register(ViPipe,0x3633,0x32);
	sc132gs_write_register(ViPipe,0x3638,0x2a);
	sc132gs_write_register(ViPipe,0x3639,0x07);
	sc132gs_write_register(ViPipe,0x363b,0x48);
    sc132gs_write_register(ViPipe,0x363c,0x83);
	sc132gs_write_register(ViPipe,0x363d,0x10);
	sc132gs_write_register(ViPipe,0x36ea,0x38);
	sc132gs_write_register(ViPipe,0x36fa,0x25);
	sc132gs_write_register(ViPipe,0x36fb,0x05);
	sc132gs_write_register(ViPipe,0x36fd,0x04);
	sc132gs_write_register(ViPipe,0x3900,0x11);
	sc132gs_write_register(ViPipe,0x3901,0x05);
	sc132gs_write_register(ViPipe,0x3902,0xc5);
	sc132gs_write_register(ViPipe,0x3904,0x04);
	sc132gs_write_register(ViPipe,0x3908,0x0f);//blc 0x0f
	sc132gs_write_register(ViPipe,0x391e,0x00);
	sc132gs_write_register(ViPipe,0x3e01,0x53);
	sc132gs_write_register(ViPipe,0x3e02,0xe0);
	sc132gs_write_register(ViPipe,0x3e09,0x20);
	sc132gs_write_register(ViPipe,0x3e0e,0xd2);
	sc132gs_write_register(ViPipe,0x3e14,0xb0);
	sc132gs_write_register(ViPipe,0x3e1e,0x7c);
	sc132gs_write_register(ViPipe,0x3e26,0x20);
	sc132gs_write_register(ViPipe,0x4418,0x38);
	//sc132gs_write_register(ViPipe,0x4501, 0);0},
	sc132gs_write_register(ViPipe,0x4503,0x10);
	sc132gs_write_register(ViPipe,0x4837,0x21);
	sc132gs_write_register(ViPipe,0x5000,0x0e);
	sc132gs_write_register(ViPipe,0x540c,0x51);
	sc132gs_write_register(ViPipe,0x550f,0x38);
	sc132gs_write_register(ViPipe,0x5780,0x67);
	sc132gs_write_register(ViPipe,0x5784,0x10);
	sc132gs_write_register(ViPipe,0x5785,0x06);
	sc132gs_write_register(ViPipe,0x5787,0x02);
	sc132gs_write_register(ViPipe,0x5788,0x00);
	sc132gs_write_register(ViPipe,0x5789,0x00);
	sc132gs_write_register(ViPipe,0x578a,0x02);
	sc132gs_write_register(ViPipe,0x578b,0x00);
	sc132gs_write_register(ViPipe,0x578c,0x00);
	sc132gs_write_register(ViPipe,0x5790,0x00);
	sc132gs_write_register(ViPipe,0x5791,0x00);
	sc132gs_write_register(ViPipe,0x5792,0x00);
	sc132gs_write_register(ViPipe,0x5793,0x00);
	sc132gs_write_register(ViPipe,0x5794,0x00);
	sc132gs_write_register(ViPipe,0x5795,0x00);
	sc132gs_write_register(ViPipe,0x5799,0x04);

	sc132gs_write_register(ViPipe,0x3361,0x00);

	sc132gs_write_register(ViPipe,0x36e9,0x20);
	sc132gs_write_register(ViPipe,0x36f9,0x24);

    sc132gs_write_register(ViPipe,0x3208,0x03);
    sc132gs_write_register(ViPipe,0x3209,0xc0);
    sc132gs_write_register(ViPipe,0x320a,0x05);
    sc132gs_write_register(ViPipe,0x320b,0x00);

    sc132gs_write_register(ViPipe,0x3210,0x00);
    sc132gs_write_register(ViPipe,0x3211,0x74);
    sc132gs_write_register(ViPipe,0x3212,0x00);
    sc132gs_write_register(ViPipe,0x3213,0x10);

    sc132gs_default_reg_init(ViPipe);
    
    delay_ms(10);
    sc132gs_write_register (ViPipe,0x0100, 0x01);

    //sc132gs_write_register(ViPipe,0x3021,0x03);
    // sc910gs_write_register (ViPipe,0x331f,0x02);
    // sc910gs_write_register (ViPipe,0x3385,0x25);

    printf("===sc132gs 2M 30fps 8bit LINE Init OK!===\n");
    return;
}
