#include <unistd.h>

#include "template_pra.h"
#include "hal_gpio.h"

STRU_AR_HAL_VO_DSI_ATTR g_dsi_cfg;

#define lcd_backlight_port               PORT_B
#define lcd_backlight_group              GROUP_1
#define lcd_backlight_number             0
#define lcd_backlight_pad                30
#define lcd_backlight_pad_gpio_fun_num   2

#define lcd_power_port                   PORT_B
#define lcd_power_group                  GROUP_1
#define lcd_power_number                 2
#define lcd_power_pad                    32
#define lcd_power_pad_gpio_fun_num       2

#define lcd_standby_port                 PORT_A
#define lcd_standby_group                GROUP_1
#define lcd_standby_number               6
#define lcd_standby_pad                  28
#define lcd_standby_pad_gpio_fun_num     2

#define lcd_rest_port                    PORT_B
#define lcd_rest_group                   GROUP_1
#define lcd_rest_number                  1
#define lcd_rest_pad                     31
#define lcd_rest_pad_gpio_fun_num        2

#define lcd_updown_scan_port             PORT_D
#define lcd_updown_scan_group            GROUP_1
#define lcd_updown_scan_number           7
#define lcd_updown_scan_pad              26
#define lcd_updown_pad_gpio_fun_num      3


void ar_vo_lcd_gpio_init(void)
{
    int backlight = ar_hal_gpio_name_to_num(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number);
    int power_gpio=ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
    int standby_gpio=ar_hal_gpio_name_to_num(lcd_standby_group, lcd_standby_port, lcd_standby_number);
    int reset_gpio=ar_hal_gpio_name_to_num(lcd_rest_group, lcd_rest_port, lcd_rest_number);
    int updown_gpio=ar_hal_gpio_name_to_num(lcd_updown_scan_group, lcd_updown_scan_port, lcd_updown_scan_number);

    printf("backlight = %d, power_gpio = %d standby_gpio=%d, reset_gpio = %d, updown_gpio=%d\n",
                    backlight, power_gpio, standby_gpio, reset_gpio, updown_gpio);

    ar_hal_gpio_export(backlight);
    ar_hal_gpio_export(power_gpio);
    ar_hal_gpio_export(standby_gpio);
    ar_hal_gpio_export(reset_gpio);
    ar_hal_gpio_export(updown_gpio);

    //set the gpio dir to out
    ar_hal_gpio_set_dir(backlight,1);
    ar_hal_gpio_set_dir(power_gpio,1);
    ar_hal_gpio_set_dir(standby_gpio,1);
    ar_hal_gpio_set_dir(reset_gpio,1);
    ar_hal_gpio_set_dir(updown_gpio,1);


    ar_hal_gpio_set_value(power_gpio,1);
    ar_hal_gpio_set_value(backlight,1);

    //standby the lcd
    ar_hal_gpio_set_value(updown_gpio,0);
    ar_hal_gpio_set_value(standby_gpio,1);

    usleep(10000);

    ar_hal_gpio_set_value(reset_gpio,1);
    usleep(1000);
    ar_hal_gpio_set_value(reset_gpio,0);
    usleep(10000);
    ar_hal_gpio_set_value(reset_gpio,1);

    //final we backlight the lcd
    ar_hal_gpio_set_value(backlight,1);

    return;
}

void set_dsi_config(ENUM_DSI_PRA_TEMPLATE dsi_pra_temp)
{
	switch (dsi_pra_temp)
		{
		case DSI_PRA_DEFAULT:
			g_dsi_cfg.dev_no = 0;

			g_dsi_cfg.lane[0] = 1;
			g_dsi_cfg.lane[1] = 1;
			g_dsi_cfg.lane[2] = 1;
			g_dsi_cfg.lane[3] = 1;

			g_dsi_cfg.bits_per_pixel = 24;

			g_dsi_cfg.sync_info.hsa = 6;
			g_dsi_cfg.sync_info.hbp = 120;
			g_dsi_cfg.sync_info.hact = 1024;
			g_dsi_cfg.sync_info.hfp = 120;

			g_dsi_cfg.sync_info.vsa = 10;
			g_dsi_cfg.sync_info.vbp = 23;
			g_dsi_cfg.sync_info.vact = 600;
			g_dsi_cfg.sync_info.vfp = 12;

			g_dsi_cfg.dphy_clk_mhz = 313.9344;
			g_dsi_cfg.pll_freq_reg_2c0 = 0x32F74FD1;
			g_dsi_cfg.pll_freq_reg_38c = 0x2800;

			break;

		default:
			printf("error dsi pra template!\n");
			break;

		}
}

void lcd_gpio_init(ENUM_LCD_GPIO_TEMPLATE lcd_gpio)
{
	switch (lcd_gpio)
		{
		case LCD_GPIO_DEFAULT:
			ar_vo_lcd_gpio_init();
			break;

		default:
			printf("error lcd gpio template!\n");
			break;

		}
}

STRU_AR_HAL_VO_DSI_ATTR* get_dsi_attr()
{
	return &g_dsi_cfg;
}

