/*************************************************
Copyright (C), 2021--2030, GuideIR Tech. Co., Ltd.
File name	: HalAPI.h
Author		: Guide Sensmart RD
Version		: 2.0
Date		: 2021/3/9
Description	:
*************************************************/
//#include "guide_hal_api.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <stdlib.h>
#include <fcntl.h> 
#include <string.h>
#include <sys/ioctl.h> 
#include <linux/types.h> 
#include <linux/spi/spidev.h> 
//#include "ar_spi.h"

#include "ar_common.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "osal.h"


//#include "guide_internal.h"
#include "hal_gpio.h"
#include "i2c.h"

#include "ITADTD.h"

#define DEV_I2C_PATH ("/dev/i2c-2")
#define DEV_TEMP_ADC_I2C_PATH ("/dev/i2c-1")
#define DEV_TEMP_ADC_I2C_ADDR 0x48
//#define TEST_HIGH_PATH "/usrdata/guide_demo/V1C-10_High.raw"
//#define TEST_LOW_PATH "/usrdata/guide_demo/V1C-10_Low.raw"
//#define TEST_HIGH_PATH "/mnt/GHP734714E70BM067_High.raw"
//#define TEST_LOW_PATH "/tmp/GHP734714E70BM067_Low.raw"
#define TEST_HIGH_PATH "/usrdata/guide_demo/GHGH17712C606R143_High.raw"
#define TEST_LOW_PATH "/usrdata/guide_demo/GHGH17712C606R143_Low.raw"

#define KFB_PATH "/usrdata/guide_demo/KFB.raw"

#define DEV_SPI_PATH ("/dev/spidev32766.0")


#define ADC_TEMP_ENABLE
#ifdef ADC_TEMP_ENABLE
static pthread_t g_getTempThread;
static float g_tempLens = 21.5*100;
static float g_tempShutter = 22.0*100;
static float g_tempFPA = 22.5*100;
static bool g_start = false;
#endif


/*
####################################################################

    ITA SDK Hal Function, artosyn ar9341, glfeng,20220614

####################################################################
*/

#ifdef ADC_TEMP_ENABLE
static void* get_temp_thread(void *p)
{
    int ret;
    unsigned short u16AdVal = 0;
    int cur_chn = -1;
	bool ready = false;
	unsigned int fd = i2c_open(DEV_TEMP_ADC_I2C_PATH, DEV_TEMP_ADC_I2C_ADDR);
	if (fd < 0) {
		printf("temp adc i2c open failed\n");
		return NULL;
	}

	//unsigned int data = (0x80 << 8) | (0xc3 + (cur_chn << 4));
	//i2c_write_word(fd, 0x01, data);

	static unsigned int ii = 0;

    while(g_start)
    {
        if(ready)
        {
            ret = i2c_read_word(fd, 0x00, &u16AdVal);
            if(ret!=0){
                printf("temp adc i2c read failed\n");
                sleep(5);
		        continue;
            }
            u16AdVal = (u16AdVal << 8) | (u16AdVal >> 8);
            if(cur_chn == 0) //lens
            {
                //float voltage = u16AdVal * 4.09 / 0x7fff;
                g_tempLens = (12736.1304901973000 - (0.0000000040235 * u16AdVal * u16AdVal * u16AdVal) + (0.0001218402729 * u16AdVal * u16AdVal) - (1.8218076216914 * u16AdVal));
				//printf("u16AdVal == %04x, g_tempLens == %0.3f v=%.2f\n", u16AdVal, g_tempLens, voltage);
            }
            else if(cur_chn == 1) //shutter
            {
                //float voltage = u16AdVal * 4.09 / 0x7fff;
                g_tempShutter = (12736.1304901973000 - (0.0000000040235 * u16AdVal * u16AdVal * u16AdVal) + (0.0001218402729 * u16AdVal * u16AdVal) - (1.8218076216914 * u16AdVal));
				//printf("u16AdVal == %04x, g_tempShutter == %0.3f v=%.2f\n", u16AdVal, g_tempShutter, voltage);
            }
            else if(cur_chn == 2) //vtemp
            {
                //float voltage = u16AdVal * 4.09 / 0x7fff;
                g_tempFPA = 37129-2.01*u16AdVal; //120模组
				//printf("u16AdVal == %04x, g_tempFPA == %0.3f v=%.2f\n", u16AdVal, g_tempFPA, voltage);
            }
            ready = false;

            ii++;
            if(ii%10==0)
            {
                //printf("g_tempLens == %0.3f g_tempShutter == %0.3f g_tempFPA == %0.3f\n",g_tempLens,g_tempShutter,g_tempFPA);
            }
        } else {
			cur_chn = (cur_chn + 1) % 3;
			//unsigned int data = ((0xc3 + (cur_chn << 4)) << 8) | 0x80;
			unsigned int data = (0x80 << 8) | (0xc3 + (cur_chn << 4));
            ret = i2c_write_word(fd, 0x01, data);
            if(ret!=0){
                printf("temp adc i2c write failed\n");
                sleep(5);
                continue;
            }
			ready = true;
        }
        usleep(800000);
    }

    i2c_close(fd);
    return NULL;
}
#endif

int arAdcTempThreadCtl(int start)
{
#ifdef ADC_TEMP_ENABLE
    if(start){
        if(!g_start){
            g_start = true;
            pthread_create(&g_getTempThread, NULL, get_temp_thread, NULL);
        }
    }
    else{
        if(g_start){
            g_start = false;
	        pthread_join(g_getTempThread, NULL);
        }
    }
#endif
    return 0;
}


/* reg function */
static int reg_ops(unsigned long addr, int wr, int data){
    void *map_base, *virt_addr;
    off_t target = addr;
    unsigned page_size, mapped_size, offset_in_page;
    unsigned width = 8 * sizeof(int);
    uint64_t read_result = 0;

    int fd = open("/dev/mem", wr ? (O_RDWR | O_SYNC) : (O_RDONLY | O_SYNC));
    mapped_size = page_size = getpagesize();
    offset_in_page = (unsigned)target & (page_size - 1);
    if (offset_in_page + width > page_size) {
            /* This access spans pages.
             * Must map two pages to make it possible: */
            mapped_size *= 2;
    }
    map_base = mmap(NULL,
                    mapped_size,
                    wr ? (PROT_READ | PROT_WRITE) : PROT_READ,
                    MAP_SHARED,
                    fd,
                    target & ~(off_t)(page_size - 1));
    if (map_base == MAP_FAILED)
        return -1;

    virt_addr = (char*)map_base + offset_in_page;
    if(wr){
        *(volatile uint32_t*)virt_addr = data;
    }else{
        read_result = *(volatile uint32_t*)virt_addr;
    }
    if (munmap(map_base, mapped_size) == -1)
        printf("unmap reg virt failed! 0x%lx\n", addr);
    close(fd);
    return read_result;

}

int read_reg32(unsigned long addr)
{
    return reg_ops(addr, 0, 0);
    
}
int write_reg32(unsigned long addr, unsigned int data)
{
    reg_ops(addr, 1, data);
    return 0;
}

int gpio_set_func(uint32_t addr, uint32_t value, uint32_t offset)
{
    int val = read_reg32(addr);
    //ar_always("set func: read addr 0x%x value: 0x%x value %d offset %d\n", addr, val, value, offset);
    int func = (val >> offset) & 0xfffffff8;
    int low = 0;
    for(int i = offset - 1; i >= 0; i--){
        int bit = ((val >> i) & 0x1);
        low = ((low << 1) | bit);
    }
    //ar_always("set func: value: 0x%x \n", func);
    val = (((func | value) << offset) | low);
    //ar_always("set func: addr 0x%x value: 0x%x \n", addr, val);
    write_reg32(addr, val);
    return 0;
}


#define GPIO_SPI
#if 1

#define GPIO_DATA_HIGH 1
#define GPIO_DATA_LOW 0

#define spi_ss_port PORT_B
#define spi_ss_group GROUP_3
#define spi_ss_number 3
#define spi_ss_pad     93
#define spi_ss_pad_gpio_fun_num 5
#define spi_ss_gpio_reg_addr 0x10730BC
#define spi_ss_gpio_value_offset 0
#define spi_ss_pad_gpio_ori_fun_num 1

#define spi_sclk_port PORT_B
#define spi_sclk_group GROUP_3
#define spi_sclk_number 2
#define spi_sclk_pad     16
#define spi_sclk_pad_gpio_fun_num 5
#define spi_sclk_gpio_reg_addr 0x10730B8
#define spi_sclk_gpio_value_offset 20
#define spi_sclk_pad_gpio_ori_fun_num 1

#define spi_mosi_port PORT_B
#define spi_mosi_group GROUP_3
#define spi_mosi_number 5
#define spi_mosi_pad     15
#define spi_mosi_pad_gpio_fun_num 5
#define spi_mosi_gpio_reg_addr 0x10730BC
#define spi_mosi_gpio_value_offset 20
#define spi_mosi_pad_gpio_ori_fun_num 1


#define spi_miso_port PORT_B
#define spi_miso_group GROUP_3
#define spi_miso_number 4
#define spi_miso_pad     14
#define spi_miso_pad_gpio_fun_num 5
#define spi_miso_gpio_reg_addr 0x10730BC
#define spi_miso_gpio_value_offset 10
#define spi_miso_pad_gpio_ori_fun_num 1


#define PER_WORD_BITS 8
#define SPI_SPEED 2000 //500K
#define SPI_CLOCK_PERIOD  1000000/SPI_SPEED

/*
static void msb2lsb(unsigned char *data, int len){
    unsigned char tmp = 0;
    for(int i = 0; i < len; i++){
        tmp = ((data[i] & 0x1) << 7 | (data[i] & 0x2) << 5 |
               (data[i] & 0x4) << 3 | (data[i] & 0x8) << 1 |
               (data[i] & 0x10) >> 1 | (data[i] & 0x20) >> 3 |
               (data[i] & 0x40) >> 5 | (data[i] & 0x80) >> 7);
        //printf("tmp = 0x%x  ori = 0x%x \n", tmp, data[i]);
        data[i] = tmp;
    }
}*/

/* SPI端口初始化 */
static int gpio_spi_init_impl()
{
    //gpio_set_func(spi_sclk_gpio_reg_addr, spi_sclk_pad_gpio_fun_num, spi_sclk_gpio_value_offset);
    //gpio_set_func(spi_ss_gpio_reg_addr, spi_ss_pad_gpio_fun_num, spi_ss_gpio_value_offset);
    //gpio_set_func(spi_miso_gpio_reg_addr, spi_miso_pad_gpio_fun_num, spi_miso_gpio_value_offset);
    //gpio_set_func(spi_mosi_gpio_reg_addr, spi_mosi_pad_gpio_fun_num, spi_mosi_gpio_value_offset);


    int gpio_num = ar_hal_gpio_name_to_num(spi_sclk_group,spi_sclk_port,spi_sclk_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);


    gpio_num = ar_hal_gpio_name_to_num(spi_mosi_group,spi_mosi_port,spi_mosi_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);


    gpio_num = ar_hal_gpio_name_to_num(spi_ss_group,spi_ss_port,spi_ss_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);

    gpio_num = ar_hal_gpio_name_to_num(spi_miso_group,spi_miso_port,spi_miso_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, INTPUT);
    return 0;

}

static void guide_hal_gpio_set_val(int grp, int port, int num, int val){
    int gpio_num = ar_hal_gpio_name_to_num(grp,port,num);
    //ar_hal_gpio_export(gpio_num);
    //ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    ar_hal_gpio_set_value(gpio_num, val);

}

static int guide_hal_gpio_get_val(int grp, int port, int num){
    unsigned int val = 0;
    int gpio_num = ar_hal_gpio_name_to_num(grp,port,num);
    if(ar_hal_gpio_get_value(gpio_num, &val) < 0){
        //printf("")
    }
    return val;
}

/*
从设备使能
enable：为1时，使能信号有效，SS低电平
为0时，使能信号无效，SS高电平
*/
static void gpio_spi_ss_enable(int enable)
{
	if (enable)
		guide_hal_gpio_set_val(spi_ss_group,spi_ss_port, spi_ss_number, GPIO_DATA_LOW);      //SS低电平，从设备使能有效
	else
		guide_hal_gpio_set_val(spi_ss_group,spi_ss_port, spi_ss_number, GPIO_DATA_HIGH);      //SS高电平，从设备使能无效
}
 /* SPI字节写 */
static void gpio_spi_write_byte(unsigned char b, unsigned char *prx)
{
	int i, val;
    int len = 8;
    *prx = 0;
	for (i=len -1; i>=0; i--) {
        guide_hal_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_LOW);
        if(b&(1<<i)){
            val = GPIO_DATA_HIGH;
        }else{
            val = GPIO_DATA_LOW;
        }
        guide_hal_gpio_set_val(spi_mosi_group,spi_mosi_port, spi_mosi_number, val);  //从高位到低位进行串行写入
		ar_delay_us(SPI_CLOCK_PERIOD);       //延时
		guide_hal_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_HIGH);
		*prx = (*prx <<1) | guide_hal_gpio_get_val(spi_miso_group, spi_miso_port, spi_miso_number);
		ar_delay_us(SPI_CLOCK_PERIOD);
		
	}
}
/* SPI字节读 */
static unsigned char gpio_spi_read_byte()
{
	int i;
	unsigned char r = 0;
	for (i=0; i<8; i++) {
        guide_hal_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_LOW);
		ar_delay_us(SPI_CLOCK_PERIOD);       //延时
		guide_hal_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_HIGH);    // CPHA=1，在时钟的第一个跳变沿采样
		r = (r <<1) | guide_hal_gpio_get_val(spi_miso_group, spi_miso_port, spi_miso_number);   //从高位7到低位0进行串行读出
		ar_delay_us(SPI_CLOCK_PERIOD);
	}
    return r;
}

static int gpio_spi_write_read(unsigned char *TxBuf, unsigned char *RxBuf, int len, unsigned char cs)
{
    //msb2lsb(TxBuf, len);
    //printf("txbuf[0] = 0x%x  \n", TxBuf[0]);

    gpio_spi_ss_enable(1);
    ar_delay_us(SPI_CLOCK_PERIOD);
    for(int i = 0; i < len; i++){
        gpio_spi_write_byte(TxBuf[i],RxBuf+i);
    }
    guide_hal_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_LOW);
    //if(cs)
        gpio_spi_ss_enable(0);
    return 0;
}

static void gpio_spi_uninit_impl()
{
    gpio_set_func(spi_sclk_gpio_reg_addr, spi_sclk_pad_gpio_ori_fun_num, spi_sclk_gpio_value_offset);
    gpio_set_func(spi_ss_gpio_reg_addr, spi_ss_pad_gpio_ori_fun_num, spi_ss_gpio_value_offset);
    gpio_set_func(spi_miso_gpio_reg_addr, spi_miso_pad_gpio_ori_fun_num, spi_miso_gpio_value_offset);
    gpio_set_func(spi_mosi_gpio_reg_addr, spi_mosi_pad_gpio_ori_fun_num, spi_mosi_gpio_value_offset);

}

void gpio_spi_init()
{
    gpio_spi_init_impl();
}

#define spi_shutter1_port PORT_B
#define spi_shutter1_group GROUP_3
#define spi_shutter1_number 2
#define spi_shutter1_pad     14
#define spi_shutter1_pad_gpio_fun_num 0
#define spi_shutter1_gpio_reg_addr 0x10730DC
#define spi_shutter1_gpio_value_offset 0
#define spi_shutter1_pad_gpio_ori_fun_num 1

#define spi_shutter2_port PORT_B
#define spi_shutter2_group GROUP_3
#define spi_shutter2_number 3
#define spi_shutter2_pad     14
#define spi_shutter2_pad_gpio_fun_num 0
#define spi_shutter2_gpio_reg_addr 0x10730DC
#define spi_shutter2_gpio_value_offset 10
#define spi_shutter2_pad_gpio_ori_fun_num 1


void gpio_spi_uninit()
{
    gpio_spi_uninit_impl();
    //set shutter to gpio function
    gpio_set_func(spi_shutter1_gpio_reg_addr, spi_shutter1_pad_gpio_fun_num, spi_shutter1_gpio_value_offset);
    gpio_set_func(spi_shutter2_gpio_reg_addr, spi_shutter2_pad_gpio_fun_num, spi_shutter2_gpio_value_offset);
}



#else

//spi cs
#define spi_ss_port PORT_B
#define spi_ss_group GROUP_3
#define spi_ss_number 3
#define spi_ss_pad     93
#define spi_ss_pad_gpio_fun_num 5
#define spi_ss_gpio_reg_addr 0x10730BC
#define spi_ss_gpio_value_offset 0
#define spi_ss_pad_gpio_ori_fun_num 1
//gpio same cs
#define spi_shutter2_port PORT_B
#define spi_shutter2_group GROUP_3
#define spi_shutter2_number 3
#define spi_shutter2_pad     14
#define spi_shutter2_pad_gpio_fun_num 0
#define spi_shutter2_gpio_reg_addr 0x10730DC
#define spi_shutter2_gpio_value_offset 10
#define spi_shutter2_pad_gpio_ori_fun_num 2

void gpio_spi_init()
{
    //nop
    gpio_set_func(spi_ss_gpio_reg_addr, spi_ss_pad_gpio_fun_num, spi_ss_gpio_value_offset);
    gpio_set_func(spi_shutter2_gpio_reg_addr, spi_shutter2_pad_gpio_ori_fun_num, spi_shutter2_gpio_value_offset);
}

void gpio_spi_uninit()
{
    //resolve conflict of gpio B3_3
    //gpio -> spi cs
    gpio_set_func(spi_ss_gpio_reg_addr, spi_ss_pad_gpio_ori_fun_num, spi_ss_gpio_value_offset);
    //non gpio -> gpio
    gpio_set_func(spi_shutter2_gpio_reg_addr, spi_shutter2_pad_gpio_fun_num, spi_shutter2_gpio_value_offset);
}

static int gSPIFd = -1;
static uint32_t gSpiSpeed = 10*1000*1000;

int ar_spi_open(int wr)
{
    int ret = 0; 
    int fd;
    uint32_t mode = 0; 
    uint8_t bits = 8; 
    uint32_t speed = gSpiSpeed; 
    uint16_t delay; 
    fd = open(DEV_SPI_PATH, O_RDWR);
    if (fd < 0) 
    {
        printf("open %s failed\n", DEV_SPI_PATH);
        return -1;
    }

    //if(wr==1) mode |= SPI_CPHA; 
 
    /*
     * spi mode //设置spi设备模式
     */ 
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);    //写模式 
    if (ret == -1) 
    {
        printf("ioctl SPI_IOC_WR_MODE failed\n");
        return -1;
    }
 
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);    //读模式 
    if (ret == -1) 
    {
        printf("ioctl SPI_IOC_RD_MODE failed\n");
        return -1;
    }
 
    /*
     * bits per word    //设置每个字含多少位
     */ 
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);   //写 每个字含多少位 
    if (ret == -1) 
    {
        printf("ioctl SPI_IOC_WR_BITS_PER_WORD failed\n");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);   //读 每个字含多少位 
    if (ret == -1) 
    {
        printf("ioctl SPI_IOC_RD_BITS_PER_WORD failed\n");
        return -1;
    }
 
    /*
     * max speed hz     //设置速率
     */ 
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);   //写速率 
    if (ret == -1) 
    {
        printf("ioctl SPI_IOC_WR_MAX_SPEED_HZ failed\n");
        return -1;
    }
 
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);   //读速率 
    if (ret == -1) 
    {
        printf("ioctl SPI_IOC_RD_MAX_SPEED_HZ failed\n");
        return -1;
    }

    //打印模式,每字多少位和速率信息 
    //printf("spi mode: %d\n", mode); 
    //printf("bits per word: %d\n", bits); 
    //printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000); 
    
    gSPIFd = fd;
    return gSPIFd;
}

int ar_spi_close()
{
    if(gSPIFd<0)
        return -1;

    close(gSPIFd);
    gSPIFd = -1;
    return 0;
}

int ar_spi_trans(uint8_t *txBuf, uint8_t *rxBuf,__u8 cs, int buf_len)
{
    int ret;

    if(gSPIFd<0)
        return -1;

    struct spi_ioc_transfer tr = {  //声明并初始化spi_ioc_transfer结构体 
        .tx_buf = (unsigned long)txBuf, 
        .rx_buf = (unsigned long)rxBuf, 
        .len = buf_len, 
        .delay_usecs = 0, 
        .speed_hz = gSpiSpeed, 
        .bits_per_word = 8, 
        .cs_change = cs,
        //.tx_nbits = buf_len*8,
        //.rx_nbits = buf_len*8,
    };

    ret = ioctl(gSPIFd, SPI_IOC_MESSAGE(1), &tr);   //ioctl默认操作,传输数据 
    if (ret != tr.len) 
    {
        printf("ioctl SPI_IOC_MESSAGE failed\n");
        return -1;
    }

    return 0;
}

#endif

/**
* @brief:   控制快门。
* @param:   int cameraID                标识设备，支持同时打开多台设备。
* @param:   ITA_SHUTTER_STATUS status   快门状态。
* @param:   void *userParam             用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arShutterControl(int cameraID, ITA_SHUTTER_STATUS status, void *userParam)
{
    int shutter1_gpio;
    int shutter2_gpio;
    shutter1_gpio = ar_hal_gpio_name_to_num(3, 1, 3);
    shutter2_gpio = ar_hal_gpio_name_to_num(3, 1, 2);

    ar_hal_gpio_export(shutter1_gpio);
    ar_hal_gpio_export(shutter2_gpio);
    ar_hal_gpio_set_dir(shutter1_gpio, 1);
    ar_hal_gpio_set_dir(shutter2_gpio, 1);

    printf("camera id:%d, shutter status:%d\n", cameraID, status);
	switch (status)
	{
	case ITA_STATUS_CLOSE:
	    ar_hal_gpio_set_value(shutter1_gpio, 1);
        ar_hal_gpio_set_value(shutter2_gpio, 0);
        usleep(20*1000);
        ar_hal_gpio_set_value(shutter1_gpio, 0);
        ar_hal_gpio_set_value(shutter2_gpio, 0);
		break;
	case ITA_STATUS_OPEN:
	    ar_hal_gpio_set_value(shutter1_gpio, 0);
        ar_hal_gpio_set_value(shutter2_gpio, 1);
        usleep(20*1000);
        ar_hal_gpio_set_value(shutter1_gpio, 0);
        ar_hal_gpio_set_value(shutter2_gpio, 0);
		break;
	default:
		break;
	}
	return ITA_OK;
}

/**
* @brief:   获取温度传感器值。由ITA_SENSOR_VALUE决定输出AD值还是温度值。
* @param:   int cameraID                    标识设备，支持同时打开多台设备。输入参数。
* @param:   ITA_TEMP_TYPE eType             温传类型。输入参数。
* @param:   ITA_SENSOR_VALUE *sensorValue   值类型。输出参数，由用户决定。
* @param:   int *ADValue                    AD值，输出参数。当ITA_AD_VALUE==sensorValue时有效。
* @param:   float *tempValue                温度值，输出参数。当ITA_TEMP_VALUE==sensorValue时有效。
* @param:   void *userParam                 用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arGetSensorTemp(int cameraID, ITA_TEMP_TYPE eType, ITA_SENSOR_VALUE *sensorValue, int *ADValue, float *tempValue, void *userParam)
{
    *sensorValue = ITA_TEMP_VALUE;
#ifdef ADC_TEMP_ENABLE
    switch (eType) {
        case ITA_SHUTTER_TEMP:
            *tempValue = g_tempShutter/100;
            break;
        case ITA_FPA_TEMP:
            *tempValue = g_tempFPA/100;
            break;
        case ITA_LENS_TEMP:
            *tempValue = g_tempLens/100;
            break;
        default:
            return ITA_OK;
    }
#else
    switch (eType)
    {
    case ITA_SHUTTER_TEMP:
        *tempValue = 22.0;
        break;
    case ITA_FPA_TEMP:
        *tempValue = 22.5;
        break;
    case ITA_LENS_TEMP:
        *tempValue = 21.5;
        break;
    default:
        break;
    }
#endif
    return ITA_OK;

}

/**
* @brief:  读数据包。
* @param:  int cameraID         标识设备，支持同时打开多台设备。
* @param:  unsigned char *buf   数据缓存
* @param:  int offset           相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size             读取的数据长度
* @param:  ITA_RANGE range      测温范围，共有3档。
* @param:   void *userParam     用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arReadPackageData(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam)
{
    printf("readPackageData camera id=%d, offset=%d, size=%d, range=%d\n", cameraID, offset, size, range);
    
	FILE *fp = NULL;
	if (range == ITA_INDUSTRY_LOW) {
		fp = fopen(TEST_LOW_PATH, "rb");
	}
	else if(range == ITA_INDUSTRY_HIGH)
	{
	    fp = fopen(TEST_HIGH_PATH, "rb");
	}
	else {
		//body
		return ITA_FILE_NOT_EXIST;
	}

	if (!fp) {
		printf("ReadPackageData: fopen package file error\n");
		return ITA_READ_PACKAGE_ERROR;
	}

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	if (len < offset + size) {
		fclose(fp);
		printf("ReadPackageData: file length error! %d %d\n", len, offset + size);
		return ITA_READ_PACKAGE_ERROR;
	}

	fseek(fp, offset, SEEK_SET);
	fread(buf, size, 1, fp);
	fclose(fp);

	return ITA_OK;

}

/**
* @brief:  写数据包。
* @param:  int cameraID         标识设备，支持同时打开多台设备。
* @param:  unsigned char *buf   数据缓存
* @param:  int offset           相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size             写的数据长度
* @param:  ITA_RANGE range      测温范围，共有3档。
* @param:   void *userParam     用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arWritePackageData(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam)
{
    printf("writePackageData camera id=%d, offset=%d, size=%d, range=%d\n", cameraID, offset, size, range);
    return ITA_OK;
}

/**
* @brief:   SPI传输数据，先写入再读。Timo 120通过SPI配置探测器。
* @param:   int cameraID             标识设备，支持同时打开多台设备。
* @param:   unsigned char *writeBuf  待写入的数据缓存。
* @param:   unsigned char *readBuf   读数据的缓存。
* @param:   int bufLen               缓存长度。
* @param:   void *userParam          用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arSPITransmit(int cameraID, unsigned char *writeBuf, unsigned char *readBuf, int bufLen, void *userParam)
{
    printf("arSPITransmit camera id=%d, writeBuf=%p, readBuf=%p, bufLen=%d tx: %02x %02x %02x %02x\n", cameraID, writeBuf, readBuf, bufLen,
        writeBuf[0], writeBuf[1],writeBuf[2], writeBuf[3]);

    #if 0
    int ret;
    if(gSPIFd<0)
    {
        ar_spi_open(0);
    }
    
    if(gSPIFd<0)
    {
        return ITA_SPI_ERROR;
    }

    ret = ar_spi_trans(writeBuf, readBuf ,0, bufLen);
    if(ret<0)
    {
        printf("ar_spi_trans failed\n");
        return ITA_OK;
    }
    ar_spi_close();
    #else
    gpio_spi_write_read(writeBuf,readBuf,bufLen,0);
    #endif
    
    printf("spi rx: %02x %02x %02x %02x\n", readBuf[0], readBuf[1],readBuf[2], readBuf[3]);
    return ITA_OK;
}

/**
* @brief:   SPI写数据。Timo 120通过SPI配置探测器。
* @param:   int cameraID             标识设备，支持同时打开多台设备。
* @param:   unsigned char *writeData 待写入的数据。
* @param:   int writeLen             数据长度。
* @param:   void *userParam          用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arSPIWrite(int cameraID, unsigned char *writeData, int writeLen, void *userParam)
{
    printf("arSPIWrite camera id=%d, writeBuf=%p, bufLen=%d  tx: %02x %02x %02x %02x\n", cameraID, writeData, writeLen,
            writeData[0],writeData[1],writeData[2],writeData[3]);
    
    #if 0
    int ret;
    if(gSPIFd<0) ar_spi_open(0);
    if(gSPIFd<0)
    {
        return ITA_SPI_ERROR;
    }
    
    ret = ar_spi_trans(writeData, NULL,0, writeLen);

    ar_spi_close();
    
    if(ret<0)
    {
        printf("ar_spi_trans failed\n");
        return ITA_OK;
    }
    #else
    unsigned char rxBuf[4] = {0};
    gpio_spi_write_read(writeData,rxBuf,writeLen,1);
    usleep(1000*20);
    #endif
    return ITA_OK;
}

