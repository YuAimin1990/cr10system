#include "vo/interface/display_interface.h"
#include "vo/interface/dsi_mipi/dsi_reg.h"
#include "vo/interface/dsi_mipi/dphy_reg.h"
#include "vo/interface/dsi_mipi/dsi_mipi_common.h"
#include <fdtdec.h>
#include <asm/arch-ar9301/gpio.h>

#define lcd_backlight_port GPIO_PORT_D
#define lcd_backlight_group GPIO_GROUP_3
#define lcd_backlight_number 4

#define lcd_power_port GPIO_PORT_D
#define lcd_power_group GPIO_GROUP_1
#define lcd_power_number 5

#define lcd_rest_port GPIO_PORT_C
#define lcd_rest_group GPIO_GROUP_1
#define lcd_rest_number 4


static display_interface_desc_t g_desc =
    {
        .out_color_mode = COLOR_MODE_RGB888,
        .data_width = 24,
        .de_en = ENABLE_SIGNAL,
        .da_en = ENABLE_SIGNAL,
        .clock_en = ENABLE_SIGNAL,
        .de_polarity = POSITIVE_ACTIVE,
        .da_polarity = POSITIVE_ACTIVE,
        .clock_polarity = POSITIVE_ACTIVE,
        .hsync_polarity = POSITIVE_ACTIVE,
        .vsync_polarity = POSITIVE_ACTIVE,
};

static display_interface_res_infor_t g_res_infor =
    {
        /*int is_fix_res_out;*/
        1,
        /*int fix_res_index;*/
        0,
        /*int res_count;*/
        1,
        /*disp_res_infor_t res[MAX_SUPPORTED_RES];*/
        {
            // {1024, 1354, 1184, 1194, 600, 636, 612, 613, 60},
            // {1024, 1352, 1284, 1292, 600, 635, 613, 623, 60},
            // {1024, 1352, 1184, 1192, 600, 645, 612, 622, 60},
            // {3840, 6040, 5040, 5640, 2160, 2250, 2190, 2220, 60},
            {400, 470, 432, 438, 960, 994, 976, 978, 60},
        },
        8,
        0.0};

static STRU_DSI_CFG g_dsi_cfg =
    {
        .dev_no = 0,
        .lane = {1, 1},
        .bits_per_pixel = 24,
        .sync_info = {
            .hsa = 9,
            .hbp = 48,
            .hact = 400,
            .hfp = 48,

            .vsa = 2,
            .vbp = 16,
            .vact = 960,
            .vfp = 16},
        .dphy_clk_mhz = 336.3696,
        .pll_freq_reg_2c0 = 0x2F9114D6,
        .pll_freq_reg_38c = 0x2800};

static int backlight_lcd(int on)
{
    if (on)
    {
        gpio_set_val(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number, GPIO_DATA_HIGH);
    }
    else
    {
        gpio_set_val(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number, GPIO_DATA_LOW);
    }
    return 0;
}

static int power_lcd(int on)
{
    if (on)
    {
        gpio_set_val(lcd_power_group, lcd_power_port, lcd_power_number, GPIO_DATA_HIGH);
    }
    else
    {
        gpio_set_val(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DATA_LOW);
    }
    return 0;
}

static int reset_gpio(int val, int delay_ms)
{
    gpio_set_val(lcd_rest_group, lcd_rest_port, lcd_rest_number, val);
    ar_delay(delay_ms);
}

static int reset_lcd(int delay1, int delay2, int delay3)
{
    reset_gpio(GPIO_DATA_HIGH, delay1);
    reset_gpio(GPIO_DATA_LOW, delay2);
    reset_gpio(GPIO_DATA_HIGH, delay3);
    return 0;
}

static void init_mipi_port(void)
{
    printf("init mipi port: LCD_ST7701\n");

    uint8_t para0[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0));
    ar_delay(2);

    uint8_t para1[] = {0xEF, 0x08};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para0));
    ar_delay(2);

    uint8_t para2[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x10};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2));
    ar_delay(2);

    uint8_t para3[] = {0xC0, 0x77, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3));
    ar_delay(2);

    uint8_t para4[] = {0xC1, 0x0E, 0x0C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4));
    ar_delay(2);

    uint8_t para5[] = {0xC2, 0x07, 0x02};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5));
    ar_delay(2);

    uint8_t para6[] = {0xCC, 0x30};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6));
    ar_delay(2);

    uint8_t para7[] = {0xB0, 0x00, 0x17, 0x1F, 0x0E, 0x11, 0x06, 0x0D, 0x08, 0x07, 0x26, 0x03, 0x11, 0x0F, 0x2A, 0x31, 0x1C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7));
    ar_delay(8);

    uint8_t para8[] = {0xB1, 0x00, 0x17, 0x1F, 0x0D, 0x11, 0x07, 0x0C, 0x08, 0x08, 0x26, 0x04, 0x11, 0x0F, 0x2A, 0x31, 0x1C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8));
    ar_delay(8);

    uint8_t para9[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x11};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9));
    ar_delay(2);

    uint8_t para10[] = {0xB0, 0x5C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10));
    ar_delay(2);

    uint8_t para11[] = {0xB1, 0x68};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11));
    ar_delay(2);

    uint8_t para12[] = {0xB2, 0x85};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12));
    ar_delay(2);

    uint8_t para13[] = {0xB3, 0x80};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13));
    ar_delay(2);

    uint8_t para14[] = {0xB5, 0x49};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14));
    ar_delay(2);

    uint8_t para15[] = {0xB7, 0x87};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para15, sizeof(para15));
    ar_delay(2);

    uint8_t para16[] = {0xB8, 0x22};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para16, sizeof(para16));
    ar_delay(2);

    uint8_t para17[] = {0xB9, 0x10, 0x1F};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para17, sizeof(para17));
    ar_delay(2);

    uint8_t para18[] = {0xBB, 0x03};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para18, sizeof(para18));
    ar_delay(2);

    uint8_t para19[] = {0xC1, 0x88};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para19, sizeof(para19));
    ar_delay(2);

    uint8_t para20[] = {0xC2, 0x88};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para20, sizeof(para20));
    ar_delay(2);

    uint8_t para21[] = {0xD0, 0x88};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para21, sizeof(para21));
    ar_delay(2);

    uint8_t para22[] = {0xE0, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para22, sizeof(para22));
    ar_delay(2);

    uint8_t para23[] = {0xE1, 0x03, 0x96, 0x05, 0x96, 0x02, 0x96, 0x04, 0x96, 0x00, 0x44, 0x44};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para23, sizeof(para23));
    ar_delay(2);

    uint8_t para24[] = {0xE2, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para24, sizeof(para24));
    ar_delay(2);

    uint8_t para25[] = {0xE3, 0x00, 0x00, 0x33, 0x33};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para25, sizeof(para25));
    ar_delay(2);

    uint8_t para26[] = {0xE4, 0x44, 0x44};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para26, sizeof(para26));
    ar_delay(2);

    uint8_t para27[] = {0xE5, 0x0B, 0xD4, 0x28, 0x8C, 0x0D, 0xD6, 0x28, 0x8C, 0x07, 0xD0, 0x28, 0x8C, 0x09, 0xD2, 0x28, 0x8C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para27, sizeof(para27));
    ar_delay(8);

    uint8_t para28[] = {0xE6, 0x00, 0x00, 0x33, 0x33};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para28, sizeof(para28));
    ar_delay(2);

    uint8_t para29[] = {0xE7, 0x44, 0x44};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para29, sizeof(para29));
    ar_delay(2);

    uint8_t para30[] = {0xE8, 0x0A, 0xD5, 0x28, 0x8C, 0x0C, 0xD7, 0x28, 0x8C, 0x06, 0xD1, 0x28, 0x8C, 0x08, 0xD3, 0x28, 0x8C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para30, sizeof(para30));
    ar_delay(8);

    uint8_t para31[] = {0xEB, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para31, sizeof(para31));
    ar_delay(2);

    uint8_t para32[] = {0xED, 0xFF, 0x45, 0x67, 0xFC, 0x01, 0x3F, 0xAB, 0xFF, 0xFF, 0xBA, 0xF3, 0x10, 0xCF, 0x76, 0x54, 0xFF};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para32, sizeof(para32));
    ar_delay(8);

    uint8_t para33[] = {0xEF, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para33, sizeof(para33));
    ar_delay(2);

    uint8_t para34[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para34, sizeof(para34));
    ar_delay(2);

    uint8_t para35[] = {0xE8, 0x00, 0x0E};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para35, sizeof(para35));
    ar_delay(2);

    uint8_t para36[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para36, sizeof(para36));
    ar_delay(2);

    dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11);
    ar_delay(12);

    uint8_t para37[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para37, sizeof(para37));
    ar_delay(2);

    uint8_t para38[] = {0xE8, 0x00, 0x0C};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para38, sizeof(para38));
    ar_delay(2);

    uint8_t para39[] = {0xE8, 0x40, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para39, sizeof(para39));
    ar_delay(2);

    uint8_t para40[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para40, sizeof(para40));
    ar_delay(2);

    dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29);
    ar_delay(2);

    uint8_t para41[] = {0x36, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para41, sizeof(para41));
    ar_delay(2);

    uint8_t para42[] = {0x3A, 0x77};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para42, sizeof(para42));
    ar_delay(2);

    uint8_t para43[] = {0x35, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para43, sizeof(para43));
    ar_delay(2);

    return;
}

static int init_interface_dev(void)
{
    int ret = 0;
    // power down all gpio first
    gpio_set_direct(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number, GPIO_DIR_OUTPUT);
    gpio_set_val(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number, GPIO_DATA_LOW);

    gpio_set_direct(lcd_power_group, lcd_power_port, lcd_power_number, GPIO_DIR_OUTPUT);
    gpio_set_val(lcd_power_group, lcd_power_port, lcd_power_number, GPIO_DATA_LOW);

    gpio_set_direct(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DIR_OUTPUT);
    gpio_set_val(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DATA_LOW);

    ar_delay(10);

    power_lcd(1);
    backlight_lcd(0);

    reset_lcd(1, 10, 10);
    ar_delay(10);

    ret = dsi_set_timing(&g_dsi_cfg);
    if (ret != 0)
    {
        ar_err("dsi_init failed ret=%d", ret);
        goto End;
    }
    ar_delay(10);

    init_mipi_port();
    dsi_exit_cmd_mode();

    ar_delay(20);
    ar_trace_line();
End:
    return ret;
}
static int get_display_interface_desc_dev(display_interface_desc_t *desc)
{
    ar_trace_line();

    if (desc)
    {
        *desc = g_desc;
    }
    else
    {
        ar_err("null input");
    }
    return 0;
}
static int get_display_interface_res_infor_dev(display_interface_res_infor_t *res_infor)
{
    ar_trace_line();

    if (res_infor)
    {
        *res_infor = g_res_infor;
    }
    else
    {
        ar_err("null input");
    }
    return 0;
}

static int set_display_interface_res_infor_dev(disp_res_infor_t *res_infor)
{
    ar_always("---> set_display_interface_res_infor_dev");

    ar_delay(10);
    // final we backlight the lcd
    printf("backlight_lcd up \n");
    backlight_lcd(1);
    return 0;
}

display_intgerface_ops_t mipi_dev_st7701 =
    {
        .interface_index = MIPI_DEV_ST7701,
        .init_display_interface = init_interface_dev,
        .get_display_interface_desc = get_display_interface_desc_dev,
        .get_display_interface_res_infor = get_display_interface_res_infor_dev,
        .set_display_interface_res_infor = set_display_interface_res_infor_dev,
};
