#include "vo/interface/display_interface.h"
#include "vo/interface/dsi_mipi/dsi_reg.h"
#include "vo/interface/dsi_mipi/dphy_reg.h"
#include "vo/interface/dsi_mipi/dsi_mipi_common.h"
#include <fdtdec.h>
#include <asm/arch-ar9301/gpio.h>

// #ifdef CONFIG_ARTOSYN_AR9301
#define lcd_backlight_port GPIO_PORT_D
#define lcd_backlight_group GPIO_GROUP_3
#define lcd_backlight_number 4
#define lcd_backlight_pad 144
#define lcd_backlight_pad_gpio_fun_num 0

#define lcd_power_port GPIO_PORT_D
#define lcd_power_group GPIO_GROUP_1
#define lcd_power_number 5
#define lcd_power_pad 128
#define lcd_power_pad_gpio_fun_num 4

#ifdef SUPPORT_STANDBY_IO
#define lcd_standby_port GPIO_PORT_A
#define lcd_standby_group GPIO_GROUP_1
#define lcd_standby_number 6
#define lcd_standby_pad 28
#define lcd_standby_pad_gpio_fun_num 2
#endif

#define lcd_rest_port GPIO_PORT_C
#define lcd_rest_group GPIO_GROUP_1
#define lcd_rest_number 4
#define lcd_rest_pad 42
#define lcd_rest_pad_gpio_fun_num 2

#ifdef SUPPORT_SCAN_MODE
#define lcd_updown_scan_port GPIO_PORT_D
#define lcd_updown_scan_group GPIO_GROUP_1
#define lcd_updown_scan_number 7
#define lcd_updown_scan_pad 26
#define lcd_updown_pad_gpio_fun_num 3
#endif

#ifdef CONFIG_ARTOSYN_AR9311
#define lcd_backlight_port GPIO_PORT_C
#define lcd_backlight_group GPIO_GROUP_0
#define lcd_backlight_number 13

#define lcd_power_port GPIO_PORT_C
#define lcd_power_group GPIO_GROUP_0
#define lcd_power_number 12

#define lcd_standby_port GPIO_PORT_C
#define lcd_standby_group GPIO_GROUP_0
#define lcd_standby_number 8

#define lcd_rest_port GPIO_PORT_C
#define lcd_rest_group GPIO_GROUP_0
#define lcd_rest_number 11

#define lcd_updown_scan_port GPIO_PORT_C
#define lcd_updown_scan_group GPIO_GROUP_0
#define lcd_updown_scan_number 9
#endif

#if 0
#define lcd_shlr_port GPIO_PORT_A
#define lcd_shlr_group GPIO_GROUP_1
#define lcd_shlr_number 2
#define lcd_shlr_pad 24
#define lcd_shlr_pad_gpio_fun_num 1
#endif

static int get_lcd_cfg(char *name);

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
            {720, 832, 772, 780, 1440, 1482, 1457, 1461, 60},
        },
        8,
        0.0};

static STRU_DSI_CFG g_dsi_cfg =
    {
        .dev_no = 0,
        .lane = {1, 1, 1, 1},
        .bits_per_pixel = 24,
        .sync_info = {
            .hsa = 6,
            .hbp = 39,
            .hact = 720,
            .hfp = 39,

            .vsa = 4,
            .vbp = 21,
            .vact = 1440,
            .vfp = 17},
        .dphy_clk_mhz = 443.88864,
        .pll_freq_reg_2c0 = 0x240B8A22,
        .pll_freq_reg_38c = 0x2800};

static int get_lcd_cfg_init_setting(unsigned int *val, int count)
{
    int offset = -1;
    int ret, status = -1;
    void *blob = board_fdt_blob_setup();
    offset = fdt_node_offset_by_compatible(blob, 0, "tc3587_lcd_cfg");
    if (offset != -1)
    {
        ret = fdtdec_get_int_array_count(blob, offset, "init_setting", (unsigned int *)val, count * 2);
        ar_debug("ret=%d", ret);
        if (ret)
        {
            status = ret;
        }
        else
        {
            ar_err("%d", __LINE__);
        }
    }
    else
    {
        ar_err("%d", __LINE__);
    }
    return status;
}

static int lcd_init(void)
{
    int count = 0;
    unsigned int *val = NULL;
    int ret = 0;
    ar_always("start the lcd");
    count = get_lcd_cfg("init_setting_count");
    if (count > 0)
    {
        val = ar_malloc(count * sizeof(int) * 2);
        ret = get_lcd_cfg_init_setting(val, count);
        if (ret > 0)
        {
            int i = 0;
            ret = ret / 2;
            for (i = 0; i < ret; i++)
            {
                regw(val[i * 2], val[i * 2 + 1]);
                ar_always("add=0x%x data=0x%x", val[i * 2], val[i * 2 + 1]);
            }
        }
        ar_free(val);
    }
    return 0;
}
static int standby_lcd(int on)
{
#ifdef SUPPORT_STANDBY_IO
    //    pin_share_config(lcd_standby_pad, lcd_standby_pad_gpio_fun_num);
    if (on)
    {
        gpio_set_val(lcd_standby_group, lcd_standby_port, lcd_standby_number, GPIO_DATA_HIGH);
    }
    else
    {
        gpio_set_val(lcd_standby_group, lcd_standby_port, lcd_standby_number, GPIO_DATA_LOW);
    }
    return 0;
#endif
}

static int backlight_lcd(int on)
{
    //   pin_share_config(lcd_backlight_pad, lcd_backlight_pad_gpio_fun_num);
    //  gpio_set_direct(lcd_backlight_group,lcd_backlight_port,lcd_backlight_number, GPIO_DIR_OUTPUT);
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
    // pin_share_config(lcd_power_pad,lcd_power_pad_gpio_fun_num);
    //	gpio_set_direct(lcd_power_group,lcd_power_port,lcd_power_number, GPIO_DIR_OUTPUT);
    if (on)
    {
        gpio_set_val(lcd_power_group, lcd_power_port, lcd_power_number, GPIO_DATA_HIGH);
    }
    else
    {
        gpio_set_val(lcd_power_group, lcd_power_port, lcd_power_number, GPIO_DATA_LOW);
    }
    return 0;
}

static int reset_lcd(int delay1, int delay2, int delay3)
{
    // pin_share_config(lcd_rest_pad,lcd_rest_pad_gpio_fun_num);
    //   gpio_set_direct(lcd_rest_group,lcd_rest_port,lcd_rest_number, GPIO_DIR_OUTPUT);
    gpio_set_val(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DATA_HIGH);
    ar_delay(delay1);
    gpio_set_val(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DATA_LOW);
    ar_delay(delay2);
    gpio_set_val(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DATA_HIGH);
    ar_delay(delay3);
    return 0;
}

static int get_lcd_cfg(char *name)
{
    int offset = -1;
    int ret, status = 0;
    unsigned int value = -1;
    void *blob = board_fdt_blob_setup();
    offset = fdt_node_offset_by_compatible(blob, 0, "lcd_cfg");
    if (offset != -1)
    {
        ret = fdtdec_get_int_array(blob, offset, name, &value, 1);
        ar_always("ret=%d value=%d", ret, value);
        if (!ret)
        {
            status = value;
        }
        else
        {
            ar_err("%d", __LINE__);
            value = 0;
        }
    }
    else
    {
        ar_err("%d", __LINE__);
        value = 0;
    }
    ar_always("value=%d", value);
    return value;
}

static int lcd_set_scan_mode(void)
{
#ifdef SUPPORT_SCAN_MODE
    int value = 0;
    // pin_share_config(lcd_updown_scan_pad,lcd_updown_pad_gpio_fun_num);
    gpio_set_direct(lcd_updown_scan_group, lcd_updown_scan_port, lcd_updown_scan_number, GPIO_DIR_OUTPUT);
    // value=get_lcd_cfg("flip");
    gpio_set_val(lcd_updown_scan_group, lcd_updown_scan_port, lcd_updown_scan_number, value);
#endif
#if 0
   pin_share_config(lcd_shlr_pad,lcd_shlr_pad_gpio_fun_num);
   sirius_gpio_set_direction(lcd_shlr_group,lcd_shlr_port,lcd_shlr_number, GPIO_IS_OUTPUT);
   value=get_lcd_cfg("mirror");
   sirius_gpio_set_value(lcd_shlr_group,lcd_shlr_port, lcd_shlr_number, value);
#endif
    return 0;
}

static void init_mipi_port(void)
{
    printf("%s: line:%d init mipi port:  LCD_ST7703\n", __FILE__, __LINE__);

    uint8_t para0[] = {0xB9, 0xF1, 0x12, 0x83};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0));
    ar_delay(20);
    uint8_t para1[] = {0xBA, 0x33, 0x81, 0x05, 0xF9, 0x0e, 0x0e,
                       0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x44, 0x25, 0x00, 0x91, 0x0a, 0x00, 0x00, 0x02,
                       0x4F, 0x11, 0x00, 0x00, 0x37};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1));
    ar_delay(80);

    uint8_t para2[] = {0xB8, 0x25, 0x22, 0x20, 0x03};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2));
    ar_delay(20);

    uint8_t para3[] = {0xB3, 0x10, 0x10, 0x05, 0x05, 0x03, 0xFF,
                       0x00, 0x00, 0x00, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3));
    ar_delay(40);

    uint8_t para4[] = {0xC0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00,
                       0x08, 0x70, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4));
    ar_delay(40);

    uint8_t para5[] = {0xBC, 0x4E};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5));
    ar_delay(20);

    uint8_t para6[] = {0xCC, 0x0B};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6));
    ar_delay(20);

    uint8_t para7[] = {0xB4, 0x80};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7));
    ar_delay(20);

    uint8_t para8[] = {0xB2, 0xF0, 0x12, 0x30};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8));
    ar_delay(20);

    uint8_t para9[] = {0xE3, 0x07, 0x07, 0x0B, 0x0B, 0x03, 0x0B,
                       0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xC0, 0x10};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9));
    ar_delay(40);

    uint8_t para10[] = {0xC1, 0x54, 0x00, 0x1E, 0x1E, 0x77, 0xF1,
                        0xFF, 0xFF, 0xCC, 0xCC, 0x77, 0x77};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10));
    ar_delay(40);

    uint8_t para11[] = {0xB5, 0x0D, 0x0D};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11));
    ar_delay(20);

    uint8_t para12[] = {0xB6, 0x43, 0x43};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12));
    ar_delay(20);

    uint8_t para13[] = {0xBF, 0x02, 0x11, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13));
    ar_delay(20);

    uint8_t para14[] = {0xE9, 0x82, 0x10, 0x06, 0x05, 0x9E, 0x0A,
                        0xA5, 0x12, 0x31, 0x23, 0x37, 0x83, 0x04, 0xBC,
                        0x27, 0x38, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00,
                        0x0C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x75, 0x75,
                        0x31, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x13,
                        0x88, 0x64, 0x64, 0x20, 0x88, 0x88, 0x88, 0x88,
                        0x88, 0x88, 0x02, 0x88, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14));
    ar_delay(180);

    uint8_t para15[] = {0xEA, 0x02, 0x21, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x46,
                        0x02, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x64,
                        0x88, 0x13, 0x57, 0x13, 0x88, 0x88, 0x88, 0x88,
                        0x88, 0x88, 0x75, 0x88, 0x23, 0x04, 0x00, 0x00,
                        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x30, 0x0A, 0xA5, 0x00, 0x00, 0x00, 0x00};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para15, sizeof(para15));
    ar_delay(160);

    uint8_t para16[] = {0xE0, 0x00, 0x0F, 0x1A, 0x2D, 0x32, 0x3C,
                        0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13, 0x11,
                        0x13, 0x12, 0x19, 0x00, 0x0F, 0x1A, 0x2D, 0x32,
                        0x3C, 0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13,
                        0x11, 0x13, 0x12, 0x19};
    dsi_long_cmd(MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para16, sizeof(para16));
    ar_delay(100);

    dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11); // Sleep Out
    ar_delay(150);

    dsi_short_cmd_1pra(MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29); // Display On
    ar_delay(100);
    return;
}

static int init_interface_dev(void)
{
    int ret = 0;

    // gpio_set_direct(lcd_standby_group,lcd_standby_port,lcd_standby_number, GPIO_DIR_OUTPUT);
    gpio_set_direct(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number, GPIO_DIR_OUTPUT);
    gpio_set_direct(lcd_power_group, lcd_power_port, lcd_power_number, GPIO_DIR_OUTPUT);
    gpio_set_direct(lcd_rest_group, lcd_rest_port, lcd_rest_number, GPIO_DIR_OUTPUT);
    // gpio_set_direct(lcd_updown_scan_group,lcd_updown_scan_port,lcd_updown_scan_number, GPIO_DIR_OUTPUT);

    power_lcd(0);
    ar_always("poweron_lcd");
    // power on the lcd
    power_lcd(1);
    backlight_lcd(0);

    ar_always("reset_lcd");
    // reset the lcd
    reset_lcd(1, 10, 10);
    ar_delay(10);

    // ret = dsi_init();
    // if (ret != 0)
    // {
    //     ar_err("dsi_init ret=%d", ret);
    //     goto End;
    // }

    ret = dsi_init();
    if (ret != 0)
    {
        ar_err("dsi_init failed ret=%d", ret);
        goto End;
    }
    ar_always("dsi_set_timing");
    ret = dsi_set_timing(&g_dsi_cfg);
    if (ret != 0)
    {
        ar_err("dsi_init failed ret=%d", ret);
        goto End;
    }
    ar_delay(10);
    init_mipi_port();
    dsi_exit_cmd_mode();

    // //standby the lcd
    // lcd_set_scan_mode();
    // standby_lcd(1);
    // ar_delay(10);

    read_panel_id();
    ar_delay(10);
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
    backlight_lcd(1);
    return 0;
}

display_intgerface_ops_t mipi_dev_st7703 =
    {
        .interface_index = MIPI_DEV_ST7703,
        .init_display_interface = init_interface_dev,
        .get_display_interface_desc = get_display_interface_desc_dev,
        .get_display_interface_res_infor = get_display_interface_res_infor_dev,
        .set_display_interface_res_infor = set_display_interface_res_infor_dev,
};
