
#include "ar_common.h"
#include "mpi_vo.h"
#include "hal_gpio.h"
#include "lcd.h"
#include "hal_vo.h"
#include "osal.h"
//#define BACKLIGHT_PWM 

#if 0
#define lcd_backlight_port               PORT_D
#define lcd_backlight_group              GROUP_3
#define lcd_backlight_number             4
#define lcd_backlight_pad                144
#define lcd_backlight_pad_gpio_fun_num   0

#define lcd_power_port                   PORT_D
#define lcd_power_group                  GROUP_1
#define lcd_power_number                 5
#define lcd_power_pad                    128
#define lcd_power_pad_gpio_fun_num       0

#define lcd_rest_port                    PORT_C
#define lcd_rest_group                   GROUP_1
#define lcd_rest_number                  4
#define lcd_rest_pad                     42
#define lcd_rest_pad_gpio_fun_num        2

#define LCD_0 0
#define LCD_JH080N021A0  0x7685e8
static int panel_id = LCD_0;

static VO_DSI_ATTR_S pstDsiCfg =
{
    .u32Lane           = {1,1,1,1},
    .u32BitsPerPixel   = 24,
    .stSyncInfo        = {
                             .u32Hsa  = 6,
                             .u32Hbp  = 120,
                             .u32Hact = 1024,
                             .u32Hfp  = 120,

                             .u32Vsa  = 10,
                             .u32Vbp  = 23,
                             .u32Vact = 600,
                             .u32Vfp  = 12
                         },
     .fDphyClkMhz      = 320.0,
     .u32PllFreqReg2c0 = 0x32000000,
     .u32PllFreqReg38c = 0x2800
};


static VO_DSI_ATTR_S pstDsiCfg_LCD_JH080N021A0 =
{
    .u32Lane           = {1,1,1,1},
    .u32BitsPerPixel   = 24,
    .stSyncInfo        = {
                             .u32Hsa  = 21,
                             .u32Hbp  = 81,
                             .u32Hact = 800,
                             .u32Hfp  = 81,

                             .u32Vsa  = 4,
                             .u32Vbp  = 10,
                             .u32Vact = 1280,
                             .u32Vfp  = 30
                         },
     .fDphyClkMhz      = 497.61216,
     .u32PllFreqReg2c0 = 0x20274f62,
     .u32PllFreqReg38c = 0x2800
};

static VO_DSI_ATTR_S pstDsiCfg_LCD_0 =
{
    .u32Lane           = {1,1,1,1},
    .u32BitsPerPixel   = 24,
    .stSyncInfo        = {
                             .u32Hsa  = 9,
                             .u32Hbp  = 60,
                             .u32Hact = 800,
                             .u32Hfp  = 81,

                             .u32Vsa  = 3,
                             .u32Vbp  = 10,
                             .u32Vact = 1280,
                             .u32Vfp  = 30
                         },
     .fDphyClkMhz      = 476.28,
     .u32PllFreqReg2c0 = 0x2197fbb3,
     .u32PllFreqReg38c = 0x2800
};

static int backlight_lcd(int on)
{
#ifdef BACKLIGHT_PWM
	int num = 0;
	int duty = 0;

	if (on)
	{
		duty = 100;
	}

	if(pwm_export(num) < 0)
	{
		printf("PWM export failed!");
		return -1;
	}

	if(pwm_enable_set(num, 0) < 0)
	{
		printf("mode_1 disable pwm failed!\n");
		return -1;
	}

	if(pwm_period_set(num, 1000) < 0)		//1M
	{
		printf("mode_1 set period failed!\n");
		return -1;
	}

	if(pwm_duty_cycle_percent_set(num, duty) < 0)
	{
		printf("mode_1 set duty cycle percent failed!\n");
		return -1;
	}
	if(pwm_enable_set(num, 1) < 0)
	{
		printf("mode_1 enable pwm failed!\n");
		return -1;
	}
#else
   int gpio_num = ar_hal_gpio_name_to_num(lcd_backlight_group,lcd_backlight_port,lcd_backlight_number);
   ar_hal_gpio_export(gpio_num);
   ar_hal_gpio_set_dir(gpio_num, OUTPUT);
   if(on){
       ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
   }else
   {
       ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
   }
#endif
   return 0;
}

static int power_lcd(int on)
{
	int gpio_num = ar_hal_gpio_name_to_num(lcd_power_group,lcd_power_port,lcd_power_number);
    ar_hal_gpio_export(gpio_num);
	ar_hal_gpio_set_dir(gpio_num, OUTPUT);
	if(on)
	{
	   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
	}else
	{
	   ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
	}

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

static int lcd_set_scan_mode()
{
   return 0;

}
//////////////////Initial  CODE///////////////////////
static void init_mipi_port(VO_DEV voDev, int panel_id)
{
    int delayms = 0;
    if(panel_id == LCD_JH080N021A0)
    {
        printf("%s: init mipi port:  LCD_JH080N021A0\n", __FILE__);
        delayms = 5;
        uint8_t para0[] = {0xE0,0xAB,0xBA};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0), delayms);

        uint8_t para1[] = {0xE1,0xBA,0xAB};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1), delayms);

        //dcs_short_write_cmd_with_1pra(0x36,0x10); //bit 4: v flip ; bit 3: rgb or bgr; bit 2: h mirror
        uint8_t para2[] = {0xB1,0x10,0x01,0x47,0xFF};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2), delayms);

        uint8_t para3[] = {0xB2,0x0A,0x1E,0x04,0x50,0x50,0x14};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3), delayms);
        
        uint8_t para4[] = {0xB3,0x56,0xD3,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4), delayms);

        // second data bit 4 H mirror, bit 5: rgb /bgr , bit 6: v flip(not ok)
        uint8_t para5[] = {0xB4,0x42,0x30,0x04};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5), delayms);
        
        //uint8_t para2_1[] = {0xB5,0x00};
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB5, 0x00, delayms);
        
        //IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, 0x36,0x14);

        uint8_t para6[] = {0xB6,0x00,0x00,0x00,0x10,0x00,0x10,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6), delayms);

        uint8_t para7[] = {
            0xB7,
            0x0E,0x00,0xFF,0x08,
            0x08,0xFF,0xFF,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7), delayms);

        uint8_t para8[] = {0xB8,0x16,0x12,0x29,0x49,0x48,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8), delayms);
        
        uint8_t para9[] = {
            0xB9,
            0x7A,0x65,0x55,0x49,
            0x46,0x38,0x3E,0x2B,
            0x47,0x48,0x49,0x66,
            0x51,0x55,0x44,0x41,
            0x32,0x20,0x06,0x7A,
            0x65,0x55,0x49,0x46,
            0x38,0x3E,0x2B,0x47,
            0x48,0x49,0x66,0x51,
            0x55,0x44,0x41,0x32,
            0x20,0x06};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9), delayms);

        uint8_t para10[] = {
            0xBA,
            0x00,0x00,0x00,0x44,
            0x24,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10), delayms);

        uint8_t para11[] = {0xBB,0x76,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11), delayms);

        uint8_t para12[] = {0xBC,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12), delayms);

        uint8_t para13[] = {0xBD,0xFF,0x00,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13), delayms);

        //uint8_t para2_2[] = {0xBE,0x00};
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xBE, 0x00, delayms);

        uint8_t para14[] = {
            0xC0,
            0x98,0xAB,0x12,0x34,
            0x88,0x88,0x44,0x44,
            0x10,0x04,0x90,0x04,
            0x3F,0x00,0x00,0xC1};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14), delayms);

        uint8_t para15[] = {0xC1, 0x17,0xD4,0x02,0x91,0x40,0x04,0x80,0x04,0x54,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para15, sizeof(para15), delayms);

        uint8_t para16[] = {0xC2, 0x37,0x08,0x08,0x89,0x08,0x11,0x22,0x30,0x44,0x80,0x18,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para16, sizeof(para16), delayms);
        
        uint8_t para17[] = {
            0xC3,
            0xA4,0x64,0x05,0x1B,
            0x1B,0x13,0x13,0x19,
            0x19,0x11,0x11,0x17,
            0x17,0x0F,0x0F,0x15,
            0x15,0x0D,0x0D,0x00,
            0x01,0x07};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para17, sizeof(para17), delayms);

        uint8_t para18[] = {
            0xC4,
            0x24,0x24,0x04,0x1A,
            0x1A,0x12,0x12,0x18,
            0x18,0x10,0x10,0x16,
            0x16,0x0E,0x0E,0x14,
            0x14,0x0C,0x0C,0x00,
            0x01,0x06};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para18, sizeof(para18), delayms);

        uint8_t para19[] = {0xC6,0x3F,0x3F};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para19, sizeof(para19), delayms);

        uint8_t para20[] = {
            0xC7,
            0x41,0x01,0x0D,0x11,
            0x09,0x15,0x19,0x4F,
            0x10,0xD7,0xCF,0x19,
            0x1B,0x1D,0x03,0x02,
            0x25,0x30,0x00,0x03,
            0xFF,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para20, sizeof(para20), delayms);

        uint8_t para21[] = {0xC8,0x61,0x00,0x31,0x40,0x54,0x16};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para21, sizeof(para21), delayms);

        uint8_t para22[] = {0xC9,0xA1,0x22,0xFF,0xC4,0x23};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para22, sizeof(para22), delayms);
        
        uint8_t para23[] = {0xCA,0x0b,0x43};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para23, sizeof(para23), delayms);

        uint8_t para24[] = {0xCC,0x2E,0x02,0x04,0x08};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para24, sizeof(para24), delayms);

        uint8_t para25[] = {0xCD, 0x0E,0x64,0x64,0x20,0x23,0x6B,0x06,0x83};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para25, sizeof(para25), delayms);

        uint8_t para26[] = {0xD0,0x07,0x10,0x80};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para26, sizeof(para26), delayms);

        uint8_t para27[] = {0xD1,0x00,0x0D,0xFF,0x0F};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para27, sizeof(para27), delayms);

        uint8_t para28[] = {0xD2,0xE3,0x2B,0x38,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para28, sizeof(para28), delayms);

        uint8_t para29[] = {0XD4, 0x00,0x01,0x00,0x0E,0x04,0x44,0x08,0x10,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para29, sizeof(para29), delayms);

        uint8_t para30[] = {0xD6,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para30, sizeof(para30), delayms);

        uint8_t para31[] = {0xD7,0x00,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para31, sizeof(para31), delayms);

        uint8_t para32[] = {0xE4,0x08,0x55,0x03};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para32, sizeof(para32), delayms);

        uint8_t para33[] = {0xE6,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para33, sizeof(para33), delayms);

        uint8_t para34[] = {0xE7,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para34, sizeof(para34), delayms);

        uint8_t para35[] = {0xE8,0xD5,0xFF,0xFF,0xFF,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para35, sizeof(para35), delayms);

        //uint8_t para2_3[] = {0xE9,0xFF};
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xE9, 0xFF, delayms);

        uint8_t para36[] = {0xF0,0x12,0x03,0x20,0x00,0xFF};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para36, sizeof(para36), delayms);

        uint8_t para37[] = {
            0xF1,
            0xA6,0xC8,0xEA,0xE6,
            0xE4,0xCC,0xE4,0xBE,
            0xF0,0xB2,0xAA,0xC7,
            0xFF,0x66,0x98,0xE3,
            0x87,0xC8,0x99,0xC8,
            0x8C,0xBE,0x96,0x91,
            0x8F,0xFF};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para37, sizeof(para37), delayms);

        //uint8_t para2_4[] = {0xF3,0x03};
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xF3, 0x03, delayms);

        uint8_t para38[] = {
            0xF4,
            0xFF,0xFE,0xFC,0xFA,
            0xF8,0xF4,0xF0,0xE8,
            0xE0,0xD0,0xC0,0xA0,
            0x80,0x7F,0x5F,0x3F,
            0x2F,0x1F,0x17,0x0F,
            0x0B,0x07,0x05,0x03,
            0x01,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para38, sizeof(para38), delayms);

        uint8_t para39[] = {
            0xF5,
            0xFF,0xFE,0xFC,0xFA,
            0xF8,0xF4,0xF0,0xE8,
            0xE0,0xD0,0xC0,0xA0,
            0x80,0x7F,0x5F,0x3F,
            0x2F,0x1F,0x17,0x0F,
            0x0B,0x07,0x05,0x03,
            0x01,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para39, sizeof(para39), delayms);

        uint8_t para40[] = {
            0xF6,
            0xFF,0xFE,0xFC,0xFA,
            0xF8,0xF4,0xF0,0xE8,
            0xE0,0xD0,0xC0,0xA0,
            0x80,0x7F,0x5F,0x3F,
            0x2F,0x1F,0x17,0x0F,
            0x0B,0x07,0x05,0x03,
            0x01,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para40, sizeof(para40), delayms);
        
        uint8_t para41[] = {0xF7,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para41, sizeof(para41), delayms);

        uint8_t para42[] = {0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para42, sizeof(para42), delayms);

        uint8_t para43[] = {0xF9,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para43, sizeof(para43), delayms);
        
        uint8_t para44[] = {
            0xFA,
            0x00,0x84,0x12,0x21,
            0x48,0x48,0x21,0x12,
            0x84,0x69,0x69,0x5A,
            0xA5,0x96,0x96,0xA5,
            0x5A,0xB7,0xDE,0xED,
            0x7B,0x7B,0xED,0xDE,
            0xB7};
       IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para44, sizeof(para44), delayms);

       // dcs_short_write_cmd_with_4pra(0xB1,0x11,0x01,0x47,0xFF);//test pattern for panel itself
        uint8_t para45[] = {
            0xFB,
            0x00,0x12,0x0F,0xFF,
            0xFF,0xFF,0x00,0x38,
            0x40,0x08,0x70,0x0B,
            0x40,0x19,0x50,0x21,
            0xC0,0x27,0x60,0x2D,
            0x00,0x00,0x0F};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para45, sizeof(para45), delayms);

        //uint8_t para2_5[] = {0x11};
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11, 0x00, delayms);
        ar_delay(150);
        //uint8_t para2_6[] = {0x29};
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29, 0x00, delayms);

        ar_delay(20);
    }
    else
    {
        printf("%s: init mipi port:  LCD_JH080N021A0\n", __FILE__);
        delayms = 2;
        uint8_t para0[] = {0xFF,0x98,0x81,0x03};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0), delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x01, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x02, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x03, 0x53, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x04, 0x53, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x05, 0x13, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x06, 0x04, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x07, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x08, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x09, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x0a, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x0b, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x0c, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x0d, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x0e, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x0f, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x10, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x11, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x12, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x13, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x14, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x15, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x16, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x17, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x18, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x19, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x1a, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x1b, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x1c, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x1d, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x1e, 0xc0, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x1f, 0x80, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x20, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x21, 0x09, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x22, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x23, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x24, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x25, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x26, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x27, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x28, 0x55, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x29, 0x03, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2a, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2b, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2c, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2d, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2e, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2f, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x30, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x31, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x32, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x33, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x34, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x35, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x36, 0x14, delayms);//0X00
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x37, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x38, 0x3C, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x39, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3a, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3b, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3c, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3d, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3e, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3f, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x40, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x41, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x42, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x43, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x44, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x50, 0x01, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x51, 0x23, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x52, 0x45, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x53, 0x67, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x54, 0x89, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x55, 0xab, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x56, 0x01, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x57, 0x23, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x58, 0x45, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x59, 0x67, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x5a, 0x89, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x5b, 0xab, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x5c, 0xcd, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x5d, 0xef, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x5e, 0x01, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x5f, 0x08, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x60, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x61, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x62, 0x0A, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x63, 0x15, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x64, 0x14, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x65, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x66, 0x11, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x67, 0x10, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x68, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x69, 0x0F, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6a, 0x0E, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6b, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6c, 0x0D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6d, 0x0C, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6e, 0x06, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6f, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x70, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x71, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x72, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x73, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x74, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x75, 0x06, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x76, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x77, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x78, 0x0A, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x79, 0x15, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7a, 0x14, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7b, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7c, 0x10, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7d, 0x11, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7e, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7f, 0x0C, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x80, 0x0D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x81, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x82, 0x0E, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x83, 0x0F, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x84, 0x08, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x85, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x86, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x87, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x88, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x89, 0x02, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x8A, 0x02, delayms);
        
        uint8_t para1[] = {0xFF, 0x98, 0x81, 0x04};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1), delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6C, 0x15, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6E, 0x30, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x6F, 0x37, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x8D, 0x1F, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x87, 0xBA, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x26, 0x76, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB2, 0xD1, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB5, 0x07, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x35, 0x17, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x33, 0x14, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x31, 0x75, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3A, 0x85, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3B, 0x98, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x38, 0x01, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x39, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x7A, 0x10, delayms);
        
        uint8_t para2[] = {0xFF, 0x98, 0x81, 0x01};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2), delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x22, 0x0a, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x31, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x50, 0xE9, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x51, 0xE4, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x53, 0x48, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x55, 0x48, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x60, 0x28, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x2E, 0xC8, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x34, 0x01, delayms);
        
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA0, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA1, 0x11, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA2, 0x1F, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA3, 0x14, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA4, 0x18, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA5, 0x2D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA6, 0x21, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA7, 0x21, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA8, 0x7D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xA9, 0x1B, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xAA, 0x25, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xAB, 0x69, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xAC, 0x1B, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xAD, 0x1A, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xAE, 0x50, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xAF, 0x24, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB0, 0x29, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB1, 0x4D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB2, 0x5A, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB3, 0x23, delayms);
        
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC0, 0x00, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC1, 0x14, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC2, 0x22, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC3, 0x14, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC4, 0x17, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC5, 0x2A, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC6, 0x1F, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC7, 0x20, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC8, 0x7D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC9, 0x1C, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCA, 0x28, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCB, 0x68, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCC, 0x1D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCD, 0x1D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCE, 0x52, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCF, 0x27, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xD0, 0x2D, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xD1, 0x4C, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xD2, 0x59, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xD3, 0x23, delayms);
        
        uint8_t para3[] = {0xFF, 0x98, 0x81, 0x02};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3), delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x04, 0x17, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x05, 0x12, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x06, 0x40, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x07, 0x0B, delayms);
        
        uint8_t para4[] = {0xFF, 0x98, 0x81, 0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4), delayms);
        
        uint8_t para5[] = {0x51, 0x0F, 0xF0};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5), delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x53, 0x2C, delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x55, 0x00, delayms);
        
        uint8_t para6[] = {0xFF, 0x98, 0x81, 0x00};
        IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6), delayms);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x35, 0x00, delayms);
        
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11, 0x00, delayms);
        ar_delay_us(120);
        dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29, 0x00, delayms);
        ar_delay_us(20);
        ar_delay(10);
    }

    return;
}

//#define SHOW_PATTERN
static void power_on_sequence(){
    //power on the lcd
    power_lcd(0);
    ar_delay(20);
    power_lcd(1);
    backlight_lcd(0);
    //standby the lcd
    lcd_set_scan_mode();
    ar_delay(10);

#if 1//ndef PANEL_LCD_0
        reset_lcd(15,3,130);
//    reset_lcd(15,3,130);
#else
        //reset the lcd
    
        reset_lcd(1,10,10);
        ar_delay(100);
#endif


}
static int init_lcd(VO_DEV VoDev, int panel_id)
{
#ifdef SHOW_PATTERN
    VO_DSI_CMD_S pstCmd;
    uint8_t para0[] = {0xFF,0x98,0x81,0x04};
    pstCmd.u16DataType = MIPI_DATA_TYPE_GENERIC_LONG_WRITE;
    pstCmd.u16CmdSize = sizeof(para0);
    pstCmd.pu8Cmd = para0;
    AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);

    pstCmd.u16DataType = MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA;
    pstCmd.u16CmdSize = 0x012F;
    pstCmd.pu8Cmd = NULL;
    AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);
#else
    init_mipi_port(VoDev, panel_id);
#endif

    //dsi_exit_cmd_mode();
    backlight_lcd(1);
    printf(" ---> init_interface_dev\n");

   return 0;
}

int IPC_VO_8inch_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr){
    if(panel_id == LCD_0)
    {
        stPubAttr->stSyncInfo.u16Hpw  = 12;
        stPubAttr->stSyncInfo.u16Hbb  = 80;
        stPubAttr->stSyncInfo.u16Hact = 800;
        stPubAttr->stSyncInfo.u16Hfb  = 108;
        stPubAttr->stSyncInfo.u16Vpw  = 3;
        stPubAttr->stSyncInfo.u16Vbb  = 10;
        stPubAttr->stSyncInfo.u16Vact = 1280;
        stPubAttr->stSyncInfo.u16Vfb  = 30;
    }else{
        stPubAttr->stSyncInfo.u16Hpw  = 28;
        stPubAttr->stSyncInfo.u16Hbb  = 108;
        stPubAttr->stSyncInfo.u16Hact = 800;
        stPubAttr->stSyncInfo.u16Hfb  = 108;
        stPubAttr->stSyncInfo.u16Vpw  = 4;
        stPubAttr->stSyncInfo.u16Vbb  = 10;
        stPubAttr->stSyncInfo.u16Vact = 1280;
        stPubAttr->stSyncInfo.u16Vfb  = 30;
    }
    return 0;
}

int IPC_VO_8inch_Lcd_Init(VO_DEV VoDev)
{
    power_on_sequence();
    ar_always(" ---> lcadebug trace : %s %d\n", __FILE__, __LINE__);
    int s32Ret = AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg);
    if (s32Ret != AR_SUCCESS)
    {
        printf("set vo dev_%d dsi attr failed with %#x!\n", VoDev, s32Ret);
        return AR_FAILURE;
    }
    ar_always(" ---> lcadebug trace : %s %d\n", __FILE__, __LINE__);
    panel_id = IPC_VO_read_panel_id();
    ar_always(" ---> init_interface_dev: panel_id : 0x%x\n", panel_id);
    ar_delay(10);

    if(panel_id == LCD_JH080N021A0){
        s32Ret = AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg_LCD_JH080N021A0);
    }else{
        s32Ret = AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg_LCD_0);
    }
    ar_delay(10);
    write_reg32(0x1120034, 0x1);
    ar_delay(10);
    init_lcd(VoDev, panel_id);
    ar_delay(1000);
    s32Ret = AR_MPI_VO_Dsi_Enable(VoDev);
    if (s32Ret != AR_SUCCESS)
    {
        printf("enable vo dev_%d dsi failed with %#x!\n", VoDev, s32Ret);
        return AR_FAILURE;
    }
    
    return AR_SUCCESS;
}
#endif



