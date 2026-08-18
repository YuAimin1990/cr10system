
#include "ar_common.h"
#include "mpi_vo.h"
#include "hal_gpio.h"
#include "hal_vo.h"
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include "osal.h"
#include "lcd.h"

#define GPIO_DATA_HIGH 1
#define GPIO_DATA_LOW 0

#define lcd_backlight_port PORT_A
#define lcd_backlight_group GROUP_3
#define lcd_backlight_number 7
#define lcd_backlight_pad     101
#define lcd_backlight_pad_gpio_fun_num   0


#define lcd_power_port PORT_D
#define lcd_power_group GROUP_3
#define lcd_power_number 4
#define lcd_power_pad     144
#define lcd_power_pad_gpio_fun_num 0

#define lcd_rest_port PORT_A
#define lcd_rest_group GROUP_3
#define lcd_rest_number 0
#define lcd_rest_pad     88
#define lcd_rest_pad_gpio_fun_num 1


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
}


int APP_VO_DVP_OLED_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr){
    stPubAttr->stSyncInfo.u16Hpw  = 16;
    stPubAttr->stSyncInfo.u16Hbb  = 32;
    stPubAttr->stSyncInfo.u16Hact = 1024;
    stPubAttr->stSyncInfo.u16Hfb  = 20;
    stPubAttr->stSyncInfo.u16Vpw  = 6;
    stPubAttr->stSyncInfo.u16Vbb  = 32;
    stPubAttr->stSyncInfo.u16Vact = 768;
    stPubAttr->stSyncInfo.u16Vfb  = 19;
    return 0;
}

int APP_DVP_Set_Dev_Attr(VO_DEV voDev){
    STRU_AR_HAL_VO_DEV_ATTR attr;
    int fd = ar_hal_vo_dev_open(voDev);

    ar_hal_vo_dev_get_attr(fd, &attr);
    attr.bit_count_per_channel = 8;//5;
    attr.e_sub_interface = AR_HAL_VO_DEV_SUB_INTF_BT1120;//AR_HAL_VO_DEV_SUB_INTF_LCD_24BIT;
    attr.timing_customize.de_polarity = 1;
    attr.timing_customize.da_polarity = 0;
    attr.timing_customize.clock_polarity = 0;
    attr.timing_customize.hsync_polarity = 0;
    attr.timing_customize.vsync_polarity = 0;
    ar_hal_vo_dev_set_attr(fd, &attr);
    ar_hal_vo_fd_close(fd);
    return 0;
}


#define GPIO_SPI
#ifdef GPIO_SPI

#define spi_ss_port PORT_B
#define spi_ss_group GROUP_3
#define spi_ss_number 3
#define spi_ss_pad     141
#define spi_ss_pad_gpio_fun_num 5
#define spi_ss_gpio_reg_addr 0x10730bc
#define spi_ss_gpio_value_offset 0

#define spi_sclk_port PORT_B
#define spi_sclk_group GROUP_3
#define spi_sclk_number 2
#define spi_sclk_pad     140
#define spi_sclk_pad_gpio_fun_num 5
#define spi_sclk_gpio_reg_addr 0x10730b8
#define spi_sclk_gpio_value_offset 20

#define spi_mosi_port PORT_B
#define spi_mosi_group GROUP_3
#define spi_mosi_number 5
#define spi_mosi_pad     143
#define spi_mosi_pad_gpio_fun_num 5
#define spi_mosi_gpio_reg_addr 0x10730bc
#define spi_mosi_gpio_value_offset 20

#define spi_miso_port PORT_B
#define spi_miso_group GROUP_3
#define spi_miso_number 4
#define spi_miso_pad     142
#define spi_miso_pad_gpio_fun_num 5
#define spi_miso_gpio_reg_addr 0x10730bc
#define spi_miso_gpio_value_offset 10


#define PER_WORD_BITS 8
#define SPI_SPEED 500000 //500K
#define SPI_CLOCK_PERIOD  1000000/SPI_SPEED

//#define ar_delay_us(arg) udelay(arg)

int gpio_set_func(uint32_t addr, uint32_t value, uint32_t offset)
{
    int val = read_reg32(addr);
    val |= (value << offset);
    //printf("set func: addr 0x%x value: 0x%x \n", addr, val);
    write_reg32(addr, val);
    return 0;
}


/* SPI端口初始化 */
static int lcd_spi_init()
{
    gpio_set_func(spi_sclk_gpio_reg_addr, spi_sclk_pad_gpio_fun_num, spi_sclk_gpio_value_offset);
    gpio_set_func(spi_ss_gpio_reg_addr, spi_ss_pad_gpio_fun_num, spi_ss_gpio_value_offset);
    gpio_set_func(spi_miso_gpio_reg_addr, spi_miso_pad_gpio_fun_num, spi_miso_gpio_value_offset);
    gpio_set_func(spi_mosi_gpio_reg_addr, spi_mosi_pad_gpio_fun_num, spi_mosi_gpio_value_offset);


    int gpio_num = ar_hal_gpio_name_to_num(spi_sclk_group,spi_sclk_port,spi_sclk_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);


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
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    return 0;

}

static void IPC_gpio_set_val(int grp, int port, int num, int val){
    int gpio_num = ar_hal_gpio_name_to_num(grp,port,num);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    ar_hal_gpio_set_value(gpio_num, val);
}

static int IPC_gpio_get_val(int grp, int port, int num){
    unsigned int val = 0;
    int gpio_num = ar_hal_gpio_name_to_num(grp,port,val);
    if(ar_hal_gpio_get_value(gpio_num, &val) < 0){
        //printf("")
    }
    return val;
}

static void ecx334af_spi_uninit()
{
    gpio_set_func(spi_sclk_gpio_reg_addr, 1, spi_sclk_gpio_value_offset);
    gpio_set_func(spi_ss_gpio_reg_addr, 1, spi_ss_gpio_value_offset);
    gpio_set_func(spi_miso_gpio_reg_addr, 1, spi_miso_gpio_value_offset);
    gpio_set_func(spi_mosi_gpio_reg_addr, 1, spi_mosi_gpio_value_offset);

}

/*
从设备使能
enable：为1时，使能信号有效，SS低电平
为0时，使能信号无效，SS高电平
*/
static void ecx334af_ss_enable(int enable)
{
	if (enable)
		IPC_gpio_set_val(spi_ss_group,spi_ss_port, spi_ss_number, GPIO_DATA_LOW);      //SS低电平，从设备使能有效
	else
		IPC_gpio_set_val(spi_ss_group,spi_ss_port, spi_ss_number, GPIO_DATA_HIGH);      //SS高电平，从设备使能无效
}
 /* SPI字节写 */
static void ecx334af_spi_write_byte(unsigned char b)
{
	int i, val;
    int len = 8;
	for (i=len -1; i>=0; i--) {
        IPC_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_LOW);
        if(b&(1<<i)){
            val = GPIO_DATA_HIGH;
        }else{
            val = GPIO_DATA_LOW;
        }
        IPC_gpio_set_val(spi_mosi_group,spi_mosi_port, spi_mosi_number, val);  //从高位到低位进行串行写入
		ar_delay_us(SPI_CLOCK_PERIOD);       //延时
		IPC_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_HIGH);
		ar_delay_us(SPI_CLOCK_PERIOD);
	}
}
/* SPI字节读 */
static unsigned char spi_read_byte()
{
	int i;
	unsigned char r = 0;
	for (i=0; i<8; i++) {
        IPC_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_LOW);
		ar_delay_us(SPI_CLOCK_PERIOD);       //延时
		IPC_gpio_set_val(spi_sclk_group,spi_sclk_port, spi_sclk_number, GPIO_DATA_HIGH);    // CPHA=1，在时钟的第一个跳变沿采样
		r = (r <<1) | IPC_gpio_get_val(spi_miso_group, spi_miso_port, spi_miso_number);   //从高位7到低位0进行串行读出
		ar_delay_us(SPI_CLOCK_PERIOD);
	}
    return r;
}

static int ecx334af_spi_write(unsigned char *TxBuf, unsigned char *RxBuf, int len, unsigned char cs)
{
    msb2lsb(TxBuf, len);
    //printf("txbuf[0] = 0x%x  \n", TxBuf[0]);

    ecx334af_ss_enable(1);
    for(int i = 0; i < len; i++){
        ecx334af_spi_write_byte(TxBuf[i]);
    }
    //if(cs)
        ecx334af_ss_enable(0);
    return 0;
}

#else

static int spi_fd = -1;
static int spi_mode = SPI_CPHA|SPI_CPOL;//|SPI_LSB_FIRST;;
static int spi_speed = 1.5*1000*1000;;
static int spi_bits = 8;
static const char *device = "/dev/spidev32766.0";

static int 	ecx334af_spi_init()
{
    int ret=0;
    
    //printf("[%s: %d]trace : -------> %s \n", __FILE__, __LINE__, __FUNCTION__);
	spi_fd = open(device, O_RDWR);
	if (spi_fd < 0){
		printf("can't open device");
        return -1;
    }
	/** spi mode */
	ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (ret == -1){
		printf("can't set spi mode: %d \n", spi_mode);
        return -1;
    }
	ret = ioctl(spi_fd, SPI_IOC_RD_MODE, &spi_mode);
	if (ret == -1){
		printf("can't get spi mode\n");
        return -1;        
    }
	/* * bits per word */
	ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);
	if (ret == -1){
		printf("can't set bits per word");
        return -1;
    }
	ret = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits);
	if (ret == -1){
		printf("can't get bits per word");
        return -1;
    }
	/** max speed hz */
	ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1){
		printf("can't set max speed hz");
        return -1;
    }
	ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if (ret == -1){
		printf("can't get max speed hz");
        return -1;
    }

	//printf("spi mode: 0x%x\n", spi_mode);
	//printf("bits per word: %d\n", spi_bits);
	//printf("max speed: %d Hz (%d KHz)\n", spi_speed, spi_speed/1000);
    return ret;
}

struct spi_ioc2_transfer {
        __u64           tx_buf;
        __u64           rx_buf;
        __u32           len;
        __u32           tx_len;
        __u32           rx_len;
        __u32           speed_hz;

        __u16           delay_usecs;
        __u8            bits_per_word;
        __u8            cs_change;
        __u8            tx_nbits;
        __u8            rx_nbits;
        __u16           pad;

        /* If the contents of 'struct spi_ioc_transfer' ever change
         * incompatibly, then the ioctl number (currently 0) must change;
         * ioctls with constant size fields get a bit more in the way of
         * error checking than ones (like this) where that field varies.
         *
         * NOTE: struct layout is the same in 64bit and 32bit userspace.
         */
};

static int ecx334af_spi_write(const unsigned char *TxBuf, unsigned char *RxBuf, int len, unsigned char cs)
{
    msb2lsb(TxBuf, len);
    //printf("txbuf[0] = 0x%x  \n", TxBuf[0]);
    struct spi_ioc_transfer tr ={
        .tx_buf = (unsigned long) TxBuf,
        .rx_buf = (unsigned long) RxBuf,
        .len = len,
        .cs_change = cs,
        .bits_per_word = 8,
        .delay_usecs = 2,
        .speed_hz = 10000000,
    };
    int s_len = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if(s_len == tr.len) {
        return 0;
    }
    
    printf("can't send spi s message, send len : %d\n", s_len);
    return -1;

}
#endif
static int backlight_lcd(int on)
{
//    printf("[%s: %d]trace : -------> %s \n", __FILE__, __LINE__, __FUNCTION__);
   int gpio_num = ar_hal_gpio_name_to_num(lcd_backlight_group,lcd_backlight_port,lcd_backlight_number);
   ar_hal_gpio_export(gpio_num);
   ar_hal_gpio_set_dir(gpio_num, OUTPUT);
   if(on){
       ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
   }else
   {
       ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
   }

    return 0;
}

static int power_lcd(int on)
{
	int gpio_num = ar_hal_gpio_name_to_num(lcd_power_group,lcd_power_port,lcd_power_number);
    ar_hal_gpio_export(gpio_num);
	ar_hal_gpio_set_dir(gpio_num, OUTPUT);
	if(on)
	{
	   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
	}else
	{
	   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
	}


    return 0;
}
static int reset_io(int high)
{
   int gpio_num = ar_hal_gpio_name_to_num(lcd_rest_group,lcd_rest_port,lcd_rest_number);
   ar_hal_gpio_export(gpio_num);
   ar_hal_gpio_set_dir(gpio_num, OUTPUT);
   
   if(high)
       ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
   else
       ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
   return 0;
}

static int reset_lcd(int delay1,int delay2 ,int delay3)
{
   int gpio_num = ar_hal_gpio_name_to_num(lcd_rest_group,lcd_rest_port,lcd_rest_number);
   ar_hal_gpio_export(gpio_num);
   ar_hal_gpio_set_dir(gpio_num, OUTPUT);
   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
   ar_delay(delay1);
   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
   ar_delay(delay2);
   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
   ar_delay(delay3);
   return 0;
}



unsigned char lcd_set_luminance(unsigned char Value)
{
    unsigned char reg[2] = {0x1D, 0x00};                       //addr+reg0
	reg[1] = ( Value + 1 ) * 5 + 60;  //70 - 160

	/* 等场同步信号再调节屏幕亮度 */
	//while (0 != devGpioGetValue(6, 5));
    ecx334af_spi_write(reg,NULL,sizeof(reg),1);

	return 0;
}


static int 	lcd_init()
{
    //printf("[%s: %d]trace : -------> %s \n", __FILE__, __LINE__, __FUNCTION__);

    //reg : addr+data
    unsigned char reg0[2] = {0x00, 0x00};                       //addr+reg0
    unsigned char reg1_67[104]={
        0x01,0x00,0xC0,0x00,0x5F,0x08,0x00,0x00,                // addr+reg0~7
        0x04,0x56,0x00,0x00,0x00,0x00,0x00,0x00,                // reg8~f
        0x10,0x00,0x00,0x00,0x00,0xC0,0x40,0x40,                // reg10~17
        0x80,0x40,0x40,0x40,0x0A,0x0/*oled_bright*/,0x63,0x11,    // reg18~1f
        0x30,//0x30,         //H_ACT_U[7:0]                                     reg20     *
        0x34,//0x34,         //H_ACT_U[8]--0--V_ACT_D[9:8]-||-0--H_ACT_D[10:8]  reg21     *
        0x30,//0x30,         //H_ACT_D[7:0]                                     reg22     *
        0x26,//0x26,         //V_ACT_U[7:0]                                     reg23     *
        0x26,//0x26,         //V_ACT_D[7:0]                                     reg24     *
        0x00,0x00,0x00,                                         // reg25~27
        0x40,//0x40,         //0--DE_D[10:8]	||  0--DE_U[10:8]		        reg28
        0x10,//0x26,         //DE_U[7:0]                                        reg29
        0x30,//0x3A,         //DE_D[7:0]                                        reg2a
        0x00,0x00,0x00,0x1C,0x1D,                               // reg2b~2f
        0x33,0x46,0x47,0xBE,0x01,0x04,0x30,0x05,                // reg30~37
        0x4E,0x0A,0x99,0x03,0xA1,0x00,0x01,0x98,                // reg38~3f
        0x00,0x01,0x6C,0x00,0x6C,0x05,0x50,0x00,                // reg40~47
        0x06,0x5B,0x2A,0x37,0x00,0x3C,0x0A,0x00,                // reg48~4f
        0x00,0x00,0x00,0x02,0xE7,0x00,0x00,0x00,                // reg50~57
        0x00,0x00,0x00,0x4F,0x4D,0x00,0x00,0x00,                // reg58~5f
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30                 // reg60~67
    };

    int u8Luminance = 100;

    reg1_67[29] = u8Luminance;
    unsigned char data_buffer2[2]={0x00,0x0f};
    unsigned char data_buffer3[2]={0x04,0x1F};
    unsigned char data_buffer4[3]={0x5b,0x04,0x04};
    unsigned char data_buffer5[3]={0x5b,0x00,0x00};
    unsigned char data_buffer6[2]={0x53,0x00};
    unsigned char data_buffer7[2]={0x03,0x00};

    // 3 Turned Power-Save mode and allocate more than 10msec period and then perform the serial setting ��
    ecx334af_spi_write(reg0,NULL, sizeof(reg0), 1);
    usleep(10*1000);
    ecx334af_spi_write(reg1_67,NULL, sizeof(reg1_67), 0);
    // 4 Set Power-Saving mode OFF (serial setting ��)
    ecx334af_spi_write(data_buffer2,NULL, 2, 1);
	usleep(100000);//4V
    // 5 Perform the serial setting �� later than 4V(Vertical) after serial setting �� completion.
    ecx334af_spi_write( data_buffer3,NULL, 2, 0);
    usleep(3000);
    // 6 Perform the serial setting �� later than 3ms after serial setting �� completion
    ecx334af_spi_write(data_buffer4,NULL, 3, 1);
	usleep(50000);//2V
    // 7 Perform the serial setting �� later than 2V(Vertical) after serial setting �� completion
    ecx334af_spi_write(data_buffer5,NULL, 3, 1);
    // 8 Perform the serial setting �� after serial setting �� completion
    ecx334af_spi_write(data_buffer6,NULL, 2, 1);
	usleep(25000);//1V
    // 9 Perform the serial setting �� later than 1V(Vertical) after the serial setting �� completion.
    ecx334af_spi_write(data_buffer7,NULL, 2, 1);
    //printf("cfg write Success !\n");

     return 0;
}
static int power_on_sequence()
{
    //printf("[%s: %d]trace : -------> %s \n", __FILE__, __LINE__, __FUNCTION__);
    
	power_lcd(0);
    usleep(20000);
    reset_io(0);

	power_lcd(1);
    usleep(2000);
	reset_io(1);
    usleep(5000);
    backlight_lcd(1);
    usleep(10000);

   return 0;
}

int APP_VO_DVP_OLED_Lcd_Init(VO_DEV VoDev)
{
	if(lcd_spi_init() < 0)
        return -1;

    power_on_sequence();

    lcd_init();

	//backlight_lcd(1);
    return 0;
}

