#include "vo/interface/display_interface.h"
#include "vo/interface/dsi_mipi/dsi_reg.h"
#include "vo/interface/dsi_mipi/dphy_reg.h"
#include "vo/interface/dsi_mipi/dsi_mipi_common.h"
#include <fdtdec.h>
#include <asm/arch-ar9301/gpio.h>

#define ar_delay_us(arg) udelay(arg)

#define LANE4
//#define BACKLIGHT_PWM 

#define lcd_backlight_port               GPIO_PORT_D
#define lcd_backlight_group              GPIO_GROUP_3
#define lcd_backlight_number             4
#define lcd_backlight_pad                144
#define lcd_backlight_pad_gpio_fun_num   0

#define lcd_power_port                   GPIO_PORT_D
#define lcd_power_group                  GPIO_GROUP_1
#define lcd_power_number                 5
#define lcd_power_pad                    128
#define lcd_power_pad_gpio_fun_num       4

#ifdef  SUPPORT_STANDBY_IO
#define lcd_standby_port                 GPIO_PORT_A
#define lcd_standby_group                GPIO_GROUP_1
#define lcd_standby_number               6
#define lcd_standby_pad                  28
#define lcd_standby_pad_gpio_fun_num     2
#endif
#define lcd_rest_port                    GPIO_PORT_C
#define lcd_rest_group                   GPIO_GROUP_1
#define lcd_rest_number                  4
#define lcd_rest_pad                     42
#define lcd_rest_pad_gpio_fun_num        2
#ifdef SUPPORT_SCAN_MODE
#define lcd_updown_scan_port             GPIO_PORT_D
#define lcd_updown_scan_group            GPIO_GROUP_1
#define lcd_updown_scan_number           7
#define lcd_updown_scan_pad              26
#define lcd_updown_pad_gpio_fun_num      3
#endif
#if 0
#define lcd_shlr_port GPIO_PORT_A
#define lcd_shlr_group GPIO_GROUP_1
#define lcd_shlr_number 2
#define lcd_shlr_pad     24
#define lcd_shlr_pad_gpio_fun_num 1
#endif
//#define DISPLAY_30HZ
#define DISPLAY_50HZ
//#define DISPLAY_40HZ
//#define DISPLAY_60HZ

enum
{
    PANEL_ID_E309144 = 0x1F2138,
    PANEL_ID_HIB050H1087 = 0xB0,
};
enum
{
    PANEL_INDEX_HIB050H1087=0,
    PANEL_INDEX_E309144 = 1,
    PANEL_INDEX_MAX
};

static int panel_index = PANEL_INDEX_E309144;

static int get_lcd_cfg(char *name);

static display_interface_desc_t g_desc_E309144=
{
    .out_color_mode=COLOR_MODE_RGB888,
    .data_width=24,
    .de_en=ENABLE_SIGNAL,
    .da_en=ENABLE_SIGNAL,
    .clock_en=ENABLE_SIGNAL,
    .de_polarity=POSITIVE_ACTIVE,
    .da_polarity=POSITIVE_ACTIVE,
    .clock_polarity=POSITIVE_ACTIVE,
    .hsync_polarity=POSITIVE_ACTIVE,
    .vsync_polarity=POSITIVE_ACTIVE,
};

static display_interface_desc_t g_desc_HIB050H1087=
{
    .out_color_mode=COLOR_MODE_RGB888,
    .data_width=24,
    .de_en=ENABLE_SIGNAL,
    .da_en=ENABLE_SIGNAL,
    .clock_en=ENABLE_SIGNAL,
    .de_polarity=POSITIVE_ACTIVE,
    .da_polarity=POSITIVE_ACTIVE,
    .clock_polarity=POSITIVE_ACTIVE,
    .hsync_polarity=POSITIVE_ACTIVE,
    .vsync_polarity=POSITIVE_ACTIVE,
};

static display_interface_res_infor_t g_res_infor_E309144=
{
    /*int is_fix_res_out;*/
    1,
    /*int fix_res_index;*/
    0,
    /*int res_count;*/
    1,
    /*disp_res_infor_t res[MAX_SUPPORTED_RES];*/
    {
#ifdef DISPLAY_50HZ
        {720, 868, 776, 788, 1280, 1309, 1295, 1298, 50},
#endif
#ifdef DISPLAY_30HZ
        {720, 868, 776, 788, 1280, 1309, 1295, 1298, 30},
#endif
#ifdef DISPLAY_40HZ
        {720, 868, 776, 788, 1280, 1309, 1295, 1298, 40},
#endif
#ifdef DISPLAY_60HZ

        {720, 868, 776, 788, 1280, 1309, 1295, 1298, 60},
#endif
    },
    8,
    0.0
};

static display_interface_res_infor_t g_res_infor_HIB050H1087=
{
    /*int is_fix_res_out;*/
    1,
    /*int fix_res_index;*/
    0,
    /*int res_count;*/
    1,
    /*disp_res_infor_t res[MAX_SUPPORTED_RES];*/
    {
#ifdef DISPLAY_50HZ
        {720, 820, 752, 772, 1280, 1325, 1303, 1313, 50},
#endif
#ifdef DISPLAY_60HZ
        {720, 820, 752, 772, 1280, 1325, 1303, 1313, 60},
#endif
    },
    8,
    0.0
};


static STRU_DSI_CFG g_dsi_cfg_E309144 =
{
#ifdef DISPLAY_50HZ
.dev_no         = 0,
.lane           = {1,1,1,1},
.bits_per_pixel = 24,
.sync_info      = {
                      .hsa  = 9,
                      .hbp  = 60,
                      .hact = 720,
                      .hfp  = 42,

                      .vsa  = 3,
                      .vbp  = 11,
                      .vact = 1280,
                      .vfp  = 15
                  },
 .dphy_clk_mhz     = 340.8636,
 .pll_freq_reg_2c0 = 0x2EF0896D,
 .pll_freq_reg_38c = 0x2800
#endif
#ifdef DISPLAY_30HZ
.dev_no         = 0,
.lane           = {1,1,1,1},
.bits_per_pixel = 24,
.sync_info      = {
                      .hsa  = 9,
                      .hbp  = 60,
                      .hact = 720,
                      .hfp  = 42,

                      .vsa  = 3,
                      .vbp  = 11,
                      .vact = 1280,
                      .vfp  = 15
                  },
 .dphy_clk_mhz     = 204.51816,
 .pll_freq_reg_2c0 = 0x4E3B8FB5,
 .pll_freq_reg_38c = 0x2800
#endif

#ifdef DISPLAY_40HZ
.dev_no         = 0,
.lane           = {1,1,1,1},
.bits_per_pixel = 24,
.sync_info      = {
                      .hsa  = 9,
                      .hbp  = 60,
                      .hact = 720,
                      .hfp  = 42,

                      .vsa  = 3,
                      .vbp  = 11,
                      .vact = 1280,
                      .vfp  = 15
                  },
 .dphy_clk_mhz     = 272.69088,
 .pll_freq_reg_2c0 = 0x3AACABC8,
 .pll_freq_reg_38c = 0x2800
#endif
#ifdef DISPLAY_60HZ
    .dev_no         = 0,
    .lane           = {1,1,1,1},
    .bits_per_pixel = 24,
    .sync_info      = {
                          .hsa  = 9,
                          .hbp  = 60,
                          .hact = 720,
                          .hfp  = 42,

                          .vsa  = 3,
                          .vbp  = 11,
                          .vact = 1280,
                          .vfp  = 15
                      },
     .dphy_clk_mhz     = 409.03632,
     .pll_freq_reg_2c0 = 0x271dc7da,
     .pll_freq_reg_38c = 0x2800
#endif
};

static STRU_DSI_CFG g_dsi_cfg_HIB050H1087 =
{
#ifdef DISPLAY_50HZ
        .dev_no         = 0,
        .lane           = {1,1,1,1},
        .bits_per_pixel = 24,
        .sync_info      = {
                              .hsa  = 15,
                              .hbp  = 36,
                              .hact = 720,
                              .hfp  = 24,
    
                              .vsa  = 10,
                              .vbp  = 12,
                              .vact = 1280,
                              .vfp  = 23
                          },
         .dphy_clk_mhz     = 325.95,
         .pll_freq_reg_2c0 = 0x31165833,
         .pll_freq_reg_38c = 0x2800
#endif

#ifdef DISPLAY_60HZ
    .dev_no         = 0,
    .lane           = {1,1,1,1},
    .bits_per_pixel = 24,
    .sync_info      = {
                          .hsa  = 15,
                          .hbp  = 36,
                          .hact = 720,
                          .hfp  = 24,

                          .vsa  = 10,
                          .vbp  = 12,
                          .vact = 1280,
                          .vfp  = 23
                      },
     .dphy_clk_mhz     = 391.14,
     .pll_freq_reg_2c0 = 0x28E7F42A,
     .pll_freq_reg_38c = 0x2800
#endif
};

static int standby_lcd(int on)
{
#ifdef  SUPPORT_STANDBY_IO

//    pin_share_config(lcd_standby_pad, lcd_standby_pad_gpio_fun_num);
    gpio_set_direct(lcd_standby_group,lcd_standby_port,lcd_standby_number, GPIO_DIR_OUTPUT);
    if(on){
       gpio_set_val(lcd_standby_group,lcd_standby_port, lcd_standby_number, GPIO_DATA_HIGH);
    }else
    {
       gpio_set_val(lcd_standby_group,lcd_standby_port, lcd_standby_number, GPIO_DATA_LOW);
    }
    return 0;
#endif
}

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
   //pin_share_config(lcd_backlight_pad, lcd_backlight_pad_gpio_fun_num);
   gpio_set_direct(lcd_backlight_group,lcd_backlight_port,lcd_backlight_number, GPIO_DIR_OUTPUT);
   if(on){
       gpio_set_val(lcd_backlight_group,lcd_backlight_port, lcd_backlight_number, GPIO_DATA_HIGH);
   }else
   {
       gpio_set_val(lcd_backlight_group,lcd_backlight_port, lcd_backlight_number, GPIO_DATA_LOW);
   }
#endif
   return 0;
}

static int power_lcd(int on)
{
	//pin_share_config(lcd_power_pad,lcd_power_pad_gpio_fun_num);
	gpio_set_direct(lcd_power_group,lcd_power_port,lcd_power_number, GPIO_DIR_OUTPUT);
	if(on)
	{
	   gpio_set_val(lcd_power_group,lcd_power_port, lcd_power_number, GPIO_DATA_HIGH);
	}else
	{
	   gpio_set_val(lcd_power_group,lcd_power_port, lcd_power_number, GPIO_DATA_LOW);
	}

    return 0;
}

static int reset_lcd(int delay1,int delay2 ,int delay3)
{
   //pin_share_config(lcd_rest_pad,lcd_rest_pad_gpio_fun_num);
   gpio_set_direct(lcd_rest_group,lcd_rest_port,lcd_rest_number, GPIO_DIR_OUTPUT);
   gpio_set_val(lcd_rest_group,lcd_rest_port, lcd_rest_number, GPIO_DATA_HIGH);
   ar_delay(delay1);
   gpio_set_val(lcd_rest_group,lcd_rest_port,lcd_rest_number, GPIO_DATA_LOW);
   ar_delay(delay2);
   gpio_set_val(lcd_rest_group,lcd_rest_port,lcd_rest_number, GPIO_DATA_HIGH);
   ar_delay(delay3);
   return 0;
}

static int get_lcd_cfg(char *name)
{
	 int offset=-1;
	 int ret,status=0;
	 unsigned int value=-1;
	 void *blob = board_fdt_blob_setup();
	 offset = fdt_node_offset_by_compatible(blob, 0, "lcd_cfg");
	 if (offset != -1)
	 {
		 ret = fdtdec_get_int_array(blob, offset, name, &value, 1);
		 ar_always("ret=%d value=%d",ret,value);
		 if(!ret)
		 {
			status=value;

		 }else
		 {
			ar_err("%d",__LINE__);
			value=0;
		 }
	 }else
	 {
		 ar_err("%d",__LINE__);
		 value=0;
	 }
	 ar_always("value=%d",value);
	 return value;

}

static int lcd_set_scan_mode()
{
#ifdef  SUPPORT_SCAN_MODE
   int value=0;
   //pin_share_config(lcd_updown_scan_pad,lcd_updown_pad_gpio_fun_num);
   gpio_set_direct(lcd_updown_scan_group,lcd_updown_scan_port,lcd_updown_scan_number, GPIO_DIR_OUTPUT);
   value=get_lcd_cfg("flip");
   gpio_set_val(lcd_updown_scan_group,lcd_updown_scan_port, lcd_updown_scan_number, value);
#endif
#if 0
   pin_share_config(lcd_shlr_pad,lcd_shlr_pad_gpio_fun_num);
   sirius_gpio_set_direction(lcd_shlr_group,lcd_shlr_port,lcd_shlr_number, GPIO_IS_OUTPUT);
   value=get_lcd_cfg("mirror");
   sirius_gpio_set_value(lcd_shlr_group,lcd_shlr_port, lcd_shlr_number, value);
#endif
   return 0;

}
#if 0
static unsigned int dsi_reg_read(unsigned int addr)
{
    return read_reg32(DSI_HOST_REG_BASE + addr);
}

static void dsi_reg_write(unsigned int addr, unsigned int data)
{
    write_reg32(DSI_HOST_REG_BASE + addr, data);
}
#endif
static void init_mipi_port(void)
{
    int temp;
    printf("%s: init mipi port \n", __FILE__);

    if(panel_index == PANEL_INDEX_E309144){
#if 1
        uint8_t para[] = {0xB9, 0xF1, 0x12, 0x83};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para, sizeof(para));
        //#20us; 
        ar_delay_us(20);

        uint8_t para1[] = { 0xBA, 0x33, 0x81, 0x05, 
                            0xF9, 0x0E, 0x0E, 0x20, 
                            0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x00, 0x44,
                            0x25, 0x00, 0x91, 0x0A,
                            0x00, 0x00, 0x02, 0x4F, 
                            0xD1, 0x00, 0x00, 0x37};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1));
        //#40us; 
        ar_delay_us(80);

        uint8_t para2[] = {0xB8, 0x25, 0x22, 0x20, 0x03};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2));
        //#30us; 
        ar_delay_us(30);

        uint8_t para3[] = {0xBF, 0x02, 0x11, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3));
        ar_delay_us(20);

        uint8_t para4[] = { 0xB3, 0x10, 0x10, 0x28, 
                            0x28, 0x03, 0xFF, 0x00, 
                            0x00, 0x00, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4));
        ar_delay_us(40);

        uint8_t para5[] = { 0xC0, 0x73, 0x73, 0x50, 
                            0x50, 0xA0, 0x00, 0x12, 
                            0x50, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5));
        ar_delay_us(40);

        uint8_t para6[] = { 0xC6, 0x82, 0x00, 0xBF, 0xFF, 0x00, 0xE0 };
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6));
        ar_delay_us(40);

        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xBC, 0x4F);
        ar_delay_us(10);

        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCC, 0x0B);
        ar_delay_us(10);

        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB4, 0x80);
        ar_delay_us(10);

        uint8_t para7[] = {0xB2, 0xC8, 0x12, 0x30};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7));
        ar_delay_us(20);

        uint8_t para8[] = { 0xE3, 0x07, 0x07, 0x0B, 
                            0x0B, 0x03, 0x0B, 0x00, 
                            0x00, 0x00, 0x00, 0xFF, 
                            0x80, 0xC0, 0x10};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8));
        ar_delay_us(50);

        uint8_t para9[] = { 0xC1, 0x83, 0x00, 0x32, 
                            0x32, 0x77, 0xF1, 0xDF, 
                            0xDF, 0x7E, 0x7E, 0x3E, 
                            0x3E};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9));
        ar_delay_us(50);

        uint8_t para10[] = {0xB5, 0x0A, 0x0A};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10));
        ar_delay_us(20);

        uint8_t para11[] = {0xB6, 0x7E, 0x7E};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11));
        ar_delay_us(20);

        uint8_t para12[] = {0xE9, 0XC8, 0x10, 0x05, 
                            0x04, 0xFE, 0x02, 0xA1, 
                            0x12, 0x31, 0x45, 0x3F, 
                            0x83, 0x12, 0xB1, 0x3B, 
                            0x2A, 0x08, 0x05, 0x00, 
                            0x00, 0x00, 0x00, 0x08, 
                            0x05, 0x00, 0x00, 0x00, 
                            0x00, 0xFF, 0x02, 0x46, 
                            0x02, 0x48, 0x68, 0x88, 
                            0x88, 0x88, 0x80, 0x88, 
                            0xFF, 0x13, 0x57, 0x13, 
                            0x58, 0x78, 0x88, 0x88, 
                            0x88, 0x81, 0x88, 0x00,
                            0x00, 0x00, 0x00, 0x00, 
                            0x12, 0xB1, 0x3B, 0x00, 
                            0x00, 0x00, 0x00, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12));
        ar_delay_us(160);

        uint8_t para13[] = { 0xEA, 0x00, 0x1A, 0x00, 
                             0x00, 0x00, 0x00, 0x00, 
                             0x00, 0x00, 0x00, 0x00, 
                             0x00, 0xFF, 0x31, 0x75, 
                             0x31, 0x18, 0x78, 0x88, 
                             0x88, 0x88, 0x85, 0x88, 
                             0xFF, 0x20, 0x64, 0x20, 
                             0x08, 0x68, 0x88, 0x88, 
                             0x88, 0x84, 0x88, 0x23, 
                             0x00, 0x00, 0x00, 0x54, 
                             0x00, 0x00, 0x00, 0x00, 
                             0x00, 0x00, 0x00, 0xC0, 
                             0x00, 0x00, 0x0C, 0x00,
                             0x00, 0x00, 0x00, 0x30, 
                             0x02, 0xA1, 0x00, 0x00, 
                             0x00, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13));

        uint8_t para14[] = {0xE0, 0x00, 0x08, 0x0E, 
                            0x23, 0x3C, 0x3F, 0x3E, 
                            0x34, 0x07, 0x0C, 0x0E, 
                            0x11, 0x13, 0x11, 0x12, 
                            0x11, 0x18, 0x00, 0x08, 
                            0x0E, 0x23, 0x3C, 0x3F, 
                            0x3E, 0x34, 0x07, 0x0C, 
                            0x0E, 0x11, 0x13, 0x11, 
                            0x12, 0x11, 0x18};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14));
        ar_delay_us(90);

        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x36, 0xc0);
        ar_delay_us(10);

        dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11);
        ar_delay(130);

        dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29);
        ar_delay_us(60);
#else
        uint8_t para[] = {0xb9, 0xf1, 0x12, 0x83};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para, sizeof(para));
        //#20us; 
        ar_delay_us(20);
        
        uint8_t para1[] = {0xba, 0x33, 0x81, 0x05,
                           0xf9, 0x0e, 0x0e, 0x20,
                           0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x44,
                           0x25, 0x00, 0x91, 0x0a,
                           0x00, 0x00, 0x02, 0x4f,
                           0xd1, 0x00, 0x00, 0x37};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1));
        //#40us; 
        ar_delay_us(80);
        
        uint8_t para2[] = {0xb8, 0x25, 0x22, 0x20,
                           0x03};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2));
        //#30us; 
        ar_delay_us(30);
        
        uint8_t para3[] = {0xbf, 0x02, 0x11, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3));
        ar_delay_us(20);
        
        uint8_t para4[] = {0xB3, 0x0C, 0x10, 0x0a,
                           0x50, 0x03, 0xff, 0x00,
                           0x00, 0x00, 0x00, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4));
        ar_delay_us(40);
        
        uint8_t para5[] = {0xC0, 0x73, 0x73, 0x50,
                           0x50, 0x00, 0x00, 0x08,
                           0x70, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5));
        ar_delay_us(40);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xbc, 0x46);
        ar_delay_us(10);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xcc, 0x0b);
        ar_delay_us(10);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB4, 0x80);
        ar_delay_us(10);
        
        uint8_t para6[] = {0xB2, 0xC8, 0x02, 0x30};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6));
        ar_delay_us(20);
        
        uint8_t para7[] = {0xE3, 0x07, 0x07, 0x0B,
                           0x0B, 0x03, 0x0B, 0x00,
                           0x00, 0x00, 0x00, 0xff,
                           0x80, 0xc0, 0x10};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7));
        ar_delay_us(50);
        
        uint8_t para8[] = {0xC1, 0x83, 0x00, 0x1e,
                           0x1e, 0x77, 0xf3, 0xcf,
                           0xcf, 0x7f, 0x7f, 0x57,
                           0x57};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8));
        ar_delay_us(50);
        
        uint8_t para9[] = {0xB5, 0x07, 0x07};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9));
        ar_delay_us(20);
        
        uint8_t para10[] = {0xB6, 0x70, 0x70};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10));
        ar_delay_us(20);
        
        uint8_t para11[] = {0xe9, 0xc2, 0x10, 0x05, 
                          0x04, 0xfe, 0x02, 0x91, 
                          0x12, 0x31, 0x45, 0x3f, 
                          0x83, 0x12, 0xa1, 0x3b,
                          0x2a, 0x08, 0x05, 0x00,
                          0x00, 0x00, 0x00, 0x08, 
                          0x05, 0x00, 0x00, 0x00,
                          0x00, 0xff, 0x02, 0x46,
                          0x02, 0x48, 0x68, 0x88,
                          0x88, 0x88, 0x80, 0x88, 
                          0xff, 0x13, 0x57, 0x13,
                          0x58, 0x78, 0x88, 0x88,
                          0x88, 0x81, 0x88, 0x00,
                          0x00, 0x00, 0x00, 0x00,
                          0x12, 0xb1, 0x3b, 0x00,
                          0x00, 0x00, 0x00, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11));
        ar_delay_us(160);

        uint8_t para12[] = {0xea, 0x00, 0x1a, 0x00, 
                          0x00, 0x00, 0x00, 0x00, 
                          0x00, 0x00, 0x00, 0x00, 
                          0x00, 0x00, 0x31, 0x75, 
                          0x31, 0x18, 0x78, 0x88, 
                          0x88, 0x88, 0x85, 0x88, 
                          0xff, 0x20, 0x64, 0x20, 
                          0x08, 0x68, 0x88, 0x88, 
                          0x88, 0x84, 0x88, 0x20, 
                          0x10, 0x00, 0x00, 0x54, 
                          0x00, 0x00, 0x00, 0x00, 
                          0x00, 0x00, 0x00, 0xC0, 
                          0x00, 0x00, 0x0c, 0x00,
                          0x00, 0x00, 0x00, 0x30,
                          0x02, 0xA1, 0x00, 0x00,
                          0x00, 0x00};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12));

        uint8_t para13[] = {0xe0, 0x00, 0x05, 0x08, 
                          0x24, 0x35, 0x3f, 0x37, 
                          0x2f, 0x07, 0x0c, 0x0c, 
                          0x11, 0x13, 0x11, 0x13, 
                          0x10, 0x17, 0x00, 0x05, 
                          0x08, 0x24, 0x35, 0x3f, 
                          0x37, 0x2f, 0x07, 0x0c, 
                          0x0c, 0x11, 0x13, 0x11, 
                          0x13, 0x10, 0x17};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13));
        ar_delay_us(90);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x36, 0xc0);
        ar_delay_us(10);
        
        dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11);
        ar_delay(130);
        
        dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29);
        ar_delay_us(60);
#endif
    }
    else if(panel_index == PANEL_INDEX_HIB050H1087){
        uint8_t para[] = {0xb9, 0xff, 0x83, 0x94};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para, sizeof(para));
        //#20us; 
        ar_delay_us(20);
        
        uint8_t para1[] = {0xb1, 0x50, 0x15, 0x75,
                           0x09, 0x32, 0x44, 0x71,
                           0x31, 0x55, 0x2f};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1));
        //#40us; 
        ar_delay_us(40);
        
        uint8_t para2[] = {0xba, 0x63, 0x03, 0x68, 
                           0x6b, 0xb2, 0xc0};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2));
        //#30us; 
        ar_delay_us(30);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xd2, 0x88);
        //#10us; 
        ar_delay_us(10);

        uint8_t para3[] = {0xb2, 0x00, 0x80, 0x64,
                           0x10, 0x07};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3));
        //#30us; 
        ar_delay_us(30);
        
        uint8_t para4[] = {0xb4, 0x01, 0x74, 0x01,
                           0x74, 0x01, 0x74, 0x01,
                           0x0c, 0x86, 0x75, 0x00,
                           0x3f, 0x01, 0x74, 0x01,
                           0x74, 0x01, 0x74, 0x01,
                           0x0c, 0x86};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4));
        //#70us;
        ar_delay_us(70);
        
        uint8_t para5[] = {0xD3, 0x00, 0x00, 0x07,
                           0x07, 0x40, 0x1e, 0x08,
                           0x00, 0x32, 0x10, 0x08,
                           0x00, 0x08, 0x54, 0x15,
                           0x10, 0x05, 0x04, 0x02,
                           0x12, 0x10, 0x05, 0x07,
                           0x23, 0x23, 0x0c, 0x0c,
                           0x27, 0x10, 0x07, 0x07,
                           0x10, 0x40};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5));
        //#100us;
        ar_delay_us(100);
        
        uint8_t para6[] = {0xD5, 0x19, 0x19, 0x18,
                           0x18, 0x1b, 0x1b, 0x1a,
                           0x1a, 0x04, 0x05, 0x06,
                           0x07, 0x00, 0x01, 0x02,
                           0x03, 0x20, 0x21, 0x18,
                           0x18, 0x22, 0x23, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6));
        //#130us; 
        ar_delay_us(130);
        
        uint8_t para7[] = {0xD6, 0x18, 0x18, 0x19,
                           0x19, 0x1b, 0x1b, 0x1a,
                           0x1a, 0x03, 0x02, 0x01,
                           0x00, 0x07, 0x06, 0x05,
                           0x04, 0x23, 0x22, 0x18,
                           0x18, 0x21, 0x20, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18, 0x18, 0x18, 0x18,
                           0x18};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7));
        //#130us; 
        ar_delay_us(130);
        
        uint8_t para8[] = {0xe0, 0x00, 0x03, 0x09,
                           0x11, 0x11, 0x14, 0x18,
                           0x16, 0x2e, 0x3d, 0x4d,
                           0x4d, 0x58, 0x6c, 0x72,
                           0x78, 0x88, 0x8b, 0x86,
                           0xa4, 0xb2, 0x58, 0x55,
                           0x59, 0x5b, 0x5d, 0x60,
                           0x64, 0x7f, 0x00, 0x03,
                           0x09, 0x0f, 0x11, 0x14,
                           0x18, 0x16, 0x2e, 0x3d,
                           0x4d, 0x4d, 0x58, 0x6d,
                           0x73, 0x78, 0x88, 0x8b,
                           0x87, 0xa5, 0xb2, 0x58,
                           0x55, 0x58, 0x5b, 0x5d,
                           0x61, 0x65, 0x7f};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8));
        //#160us;
        ar_delay_us(160);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xcc, 0x0b);
        //#10us; 
        ar_delay_us(10);

        uint8_t para9[] = {0xc0, 0x1f, 0x31};
        dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9));
        //#10us; 
        ar_delay_us(20);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xd4, 0x02);
        //#10us; 
        ar_delay_us(10);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xbd, 0x01);
        //#10us; 
        ar_delay_us(10);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xb1, 0x0);
        //#10us; 
        ar_delay_us(10);    
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xbd, 0x00);
        //#10us; 
        ar_delay_us(10);
        
        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xc6, 0xef);
        //#10us; 
        ar_delay_us(10);
        
        dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11);
        //#10us; 
        ar_delay_us(10);
        
        dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29);

        dsi_short_cmd_2pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x36, 0x03);
        //#10us; 
        //#20ms;
        ar_delay_us(10);

    }
    //#150us;
    ar_delay_us(150);

    ar_delay(1);
    ar_trace_line();
    return;
}


static int init_interface_dev()
{
    //power on the lcd
    power_lcd(1);
    backlight_lcd(0);
    //standby the lcd
    lcd_set_scan_mode();
    standby_lcd(1);
    ar_delay(10);

    //reset the lcd
    reset_lcd(1,10,10);

    ar_delay(100);

    int ret = 0;
    ar_delay(100);
    ret = dsi_init();
    if (ret != 0)
    {
        ar_err("dsi_init ret=%d", ret);
        goto End;
    }

    int panel_id = read_panel_id();
    if(panel_id != -1){
        if(panel_id == PANEL_ID_E309144){
            panel_index = PANEL_INDEX_E309144;
        }
        else if(panel_id == PANEL_ID_HIB050H1087){
            panel_index = PANEL_INDEX_HIB050H1087;
        }
    }
    
    if(panel_index == PANEL_INDEX_HIB050H1087)
        ret = dsi_set_timing(&g_dsi_cfg_HIB050H1087);
    else if(panel_index == PANEL_INDEX_E309144)
        ret = dsi_set_timing(&g_dsi_cfg_E309144);
    else
        ret = dsi_set_timing(&g_dsi_cfg_E309144);
    
    if (ret != 0)
    {
       ar_err("dsi_init ret=%d", ret);
       goto End;
    }
    ar_delay(10);
    init_mipi_port();

    dsi_exit_cmd_mode();
    ar_always(" ---> init_interface_dev");

   End:
   return 0;
}
static int get_display_interface_desc_dev(display_interface_desc_t *desc)
{
    ar_trace_line();

    if(desc)
	{
        if(panel_index == PANEL_INDEX_E309144)
            *desc=g_desc_E309144;
        else if(panel_index == PANEL_INDEX_HIB050H1087)
            *desc=g_desc_HIB050H1087;
        else
            *desc=g_desc_E309144;
    }else{
       ar_err("null input");
    }
	return 0;
}
static int get_display_interface_res_infor_dev(display_interface_res_infor_t *res_infor)
{
    ar_trace_line();

    if(res_infor)
	{
	    if(panel_index == PANEL_INDEX_E309144)
            *res_infor=g_res_infor_E309144;
        else if(panel_index == PANEL_INDEX_HIB050H1087)
            *res_infor=g_res_infor_HIB050H1087;
        else
            *res_infor=g_res_infor_E309144;
    }else{
       ar_err("null input");
    }
	return 0;
}

static int set_display_interface_res_infor_dev(disp_res_infor_t *res_infor)
{
    ar_always("---> set_display_interface_res_infor_dev");

    ar_delay(10);
    //final we backlight the lcd
	backlight_lcd(1);
	return 0;
}

display_intgerface_ops_t mipi_dev_ZK_5INCH =
{
   .interface_index=MIPI_DEV_ZK_5INCH,
   .init_display_interface=init_interface_dev,
   .get_display_interface_desc=get_display_interface_desc_dev,
   .get_display_interface_res_infor=get_display_interface_res_infor_dev,
   .set_display_interface_res_infor=set_display_interface_res_infor_dev,
};

