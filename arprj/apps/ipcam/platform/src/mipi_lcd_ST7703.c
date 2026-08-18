
#include "ar_common.h"
#include "mpi_vo.h"
#include "hal_gpio.h"
#include "lcd.h"
#include "hal_vo.h"
#include "osal.h"
// #define BACKLIGHT_PWM

#define lcd_backlight_port PORT_D
#define lcd_backlight_group GROUP_3
#define lcd_backlight_number 4
#define lcd_backlight_pad 144
#define lcd_backlight_pad_gpio_fun_num 0

#define lcd_power_port PORT_D
#define lcd_power_group GROUP_1
#define lcd_power_number 5
#define lcd_power_pad 128
#define lcd_power_pad_gpio_fun_num 4

#define lcd_rest_port PORT_C
#define lcd_rest_group GROUP_1
#define lcd_rest_number 4
#define lcd_rest_pad 42
#define lcd_rest_pad_gpio_fun_num 2

#define LCD_0 0
#define LCD_JH080N021A0 0x7685e8
static int panel_id = LCD_0;

/* static VO_DSI_ATTR_S pstDsiCfg =
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
}; */

static VO_DSI_ATTR_S pstDsiCfg =
    {
        .u32Lane = {1, 1, 1, 1},
        .u32BitsPerPixel = 8,
        .stSyncInfo = {
            .u32Hsa = 10,
            .u32Hbp = 50,
            .u32Hact = 720,
            .u32Hfp = 50,

            .u32Vsa = 4,
            .u32Vbp = 21,
            .u32Vact = 1440,
            .u32Vfp = 17},
        .fDphyClkMhz = 206.2944,
        .u32PllFreqReg2c0 = 0x4D8F1EA2,
        .u32PllFreqReg38c = 0x2800};

static VO_DSI_ATTR_S pstDsiCfg_LCD_JH080N021A0 =
    {
        .u32Lane = {1, 1, 1, 1},
        .u32BitsPerPixel = 24,
        .stSyncInfo = {
            .u32Hsa = 21,
            .u32Hbp = 81,
            .u32Hact = 800,
            .u32Hfp = 81,

            .u32Vsa = 4,
            .u32Vbp = 10,
            .u32Vact = 1280,
            .u32Vfp = 30},
        .fDphyClkMhz = 497.61216,
        .u32PllFreqReg2c0 = 0x20274f62,
        .u32PllFreqReg38c = 0x2800};

static VO_DSI_ATTR_S pstDsiCfg_LCD_0 =
    {
        .u32Lane = {1, 1, 1, 1},
        .u32BitsPerPixel = 24,
        .stSyncInfo = {
            .u32Hsa = 9,
            .u32Hbp = 60,
            .u32Hact = 800,
            .u32Hfp = 81,

            .u32Vsa = 3,
            .u32Vbp = 10,
            .u32Vact = 1280,
            .u32Vfp = 30},
        .fDphyClkMhz = 476.28,
        .u32PllFreqReg2c0 = 0x2197fbb3,
        .u32PllFreqReg38c = 0x2800};

static int backlight_lcd(int on)
{
#ifdef BACKLIGHT_PWM
    int num = 0;
    int duty = 0;

    if (on)
    {
        duty = 100;
    }

    if (pwm_export(num) < 0)
    {
        printf("PWM export failed!");
        return -1;
    }

    if (pwm_enable_set(num, 0) < 0)
    {
        printf("mode_1 disable pwm failed!\n");
        return -1;
    }

    if (pwm_period_set(num, 1000) < 0) // 1M
    {
        printf("mode_1 set period failed!\n");
        return -1;
    }

    if (pwm_duty_cycle_percent_set(num, duty) < 0)
    {
        printf("mode_1 set duty cycle percent failed!\n");
        return -1;
    }
    if (pwm_enable_set(num, 1) < 0)
    {
        printf("mode_1 enable pwm failed!\n");
        return -1;
    }
#else
    int gpio_num = ar_hal_gpio_name_to_num(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    if (on)
    {
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
    }
    else
    {
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
    }
#endif
    return 0;
}

static int power_lcd(int on)
{
    int gpio_num = ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    if (on)
    {
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
    }
    else
    {
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
    }

    return 0;
}

static int reset_lcd(int delay1, int delay2, int delay3)
{
    int gpio_num = ar_hal_gpio_name_to_num(lcd_rest_group, lcd_rest_port, lcd_rest_number);
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
static void init_mipi_port(VO_DEV voDev)
{
    int delayms = 0;

    printf("%s: line:%d init mipi port:  LCD_ST7703\n", __FILE__, __LINE__);
    delayms = 5;
    uint8_t para0[] = {0xB9, 0xF1, 0x12, 0x83};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0), delayms);
    ar_delay(20);

    uint8_t para1[] = {0xBA, 0x33, 0x81, 0x05, 0xF9, 0x0e, 0x0e,
                       0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x44, 0x25, 0x00, 0x91, 0x0a, 0x00, 0x00, 0x02,
                       0x4F, 0x11, 0x00, 0x00, 0x37};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1), delayms);
    ar_delay(80);

    uint8_t para2[] = {0xB8, 0x75, 0x22, 0x20, 0x03};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2), delayms);
    ar_delay(20);

    uint8_t para3[] = {0xB3, 0x10, 0x10, 0x05, 0x05, 0x03, 0xFF,
                       0x00, 0x00, 0x00, 0x00};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3), delayms);
    ar_delay(40);

    uint8_t para4[] = {0xC0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00,
                       0x08, 0x70, 0x00};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4), delayms);
    ar_delay(40);

    uint8_t para5[] = {0xBC, 0x4E};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5), delayms);
    ar_delay(20);

    uint8_t para6[] = {0xCC, 0x0B};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para6, sizeof(para6), delayms);
    ar_delay(20);

    uint8_t para7[] = {0xB4, 0x80};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7), delayms);
    ar_delay(20);

    uint8_t para8[] = {0xB2, 0xF0, 0x12, 0x30};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8), delayms);
    ar_delay(20);

    uint8_t para9[] = {0xE3, 0x07, 0x07, 0x0B, 0x0B, 0x03, 0x0B,
                       0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xC0, 0x10};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9), delayms);
    ar_delay(40);

    uint8_t para10[] = {0xC1, 0x54, 0x00, 0x1E, 0x1E, 0x77, 0xF1,
                        0xFF, 0xFF, 0xCC, 0xCC, 0x77, 0x77};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10), delayms);
    ar_delay(40);

    uint8_t para11[] = {0xB5, 0x0D, 0x0D};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11), delayms);
    ar_delay(20);

    uint8_t para12[] = {0xB6, 0x43, 0x43};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12), delayms);
    ar_delay(20);

    uint8_t para13[] = {0xBF, 0x02, 0x11, 0x00};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13), delayms);
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
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14), delayms);
    ar_delay(180);

    uint8_t para15[] = {0xEA, 0x02, 0x21, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x46,
                        0x02, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x64,
                        0x88, 0x13, 0x57, 0x13, 0x88, 0x88, 0x88, 0x88,
                        0x88, 0x88, 0x75, 0x88, 0x23, 0x04, 0x00, 0x00,
                        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x30, 0x0A, 0xA5, 0x00, 0x00, 0x00, 0x00};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para15, sizeof(para15), delayms);
    ar_delay(160);

    uint8_t para16[] = {0xE0, 0x00, 0x0F, 0x1A, 0x2D, 0x32, 0x3C,
                        0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13, 0x11,
                        0x13, 0x12, 0x19, 0x00, 0x0F, 0x1A, 0x2D, 0x32,
                        0x3C, 0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13,
                        0x11, 0x13, 0x12, 0x19};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para16, sizeof(para16), delayms);
    ar_delay(100);

    uint8_t para17[] = {0x36, 0xc0};
    IPC_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para17, sizeof(para7), delayms);
    ar_delay(50);

    // dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA,0x11, delayms); //Sleep Out
    // ar_delay(50);

    // dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA,0x29, delayms); //Display On
    // ar_delay(200);

    return;
}

// #define SHOW_PATTERN
static void power_on_sequence()
{
    // power on the lcd
    power_lcd(0);
    ar_delay(20);
    power_lcd(1);
    backlight_lcd(0);
    // standby the lcd
    //  lcd_set_scan_mode();
    ar_delay(10);

#if 1 // ndef PANEL_LCD_0
    reset_lcd(15, 3, 130);
//    reset_lcd(15,3,130);
#else
    // reset the lcd

    reset_lcd(1, 10, 10);
    ar_delay(100);
#endif
}

static int init_lcd(VO_DEV VoDev, int panel_id)
{
#ifdef SHOW_PATTERN
    VO_DSI_CMD_S pstCmd;
    uint8_t para0[] = {0xFF, 0x98, 0x81, 0x04};
    pstCmd.u16DataType = MIPI_DATA_TYPE_GENERIC_LONG_WRITE;
    pstCmd.u16CmdSize = sizeof(para0);
    pstCmd.pu8Cmd = para0;
    AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);

    pstCmd.u16DataType = MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA;
    pstCmd.u16CmdSize = 0x012F;
    pstCmd.pu8Cmd = NULL;
    AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);
#else
    init_mipi_port(VoDev);
    printf(" line:%d init_mipi_port\n", __LINE__);
#endif
    // dsi_exit_cmd_mode();
    //  backlight_lcd(1);
    printf(" ---> init_interface_dev\n");

    return 0;
}

int IPC_VO_8inch_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr)
{
    if (panel_id == LCD_0)
    {
        printf("panel_id %d !\n", panel_id);
        stPubAttr->stSyncInfo.u16Hpw = 40;
        stPubAttr->stSyncInfo.u16Hbb = 200;
        stPubAttr->stSyncInfo.u16Hact = 720;
        stPubAttr->stSyncInfo.u16Hfb = 200;
        stPubAttr->stSyncInfo.u16Vpw = 4;
        stPubAttr->stSyncInfo.u16Vbb = 21;
        stPubAttr->stSyncInfo.u16Vact = 1440;
        stPubAttr->stSyncInfo.u16Vfb = 17;
    }
    else
    {
        stPubAttr->stSyncInfo.u16Hpw = 28;
        stPubAttr->stSyncInfo.u16Hbb = 108;
        stPubAttr->stSyncInfo.u16Hact = 800;
        stPubAttr->stSyncInfo.u16Hfb = 108;
        stPubAttr->stSyncInfo.u16Vpw = 4;
        stPubAttr->stSyncInfo.u16Vbb = 10;
        stPubAttr->stSyncInfo.u16Vact = 1280;
        stPubAttr->stSyncInfo.u16Vfb = 30;
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

    if (panel_id == LCD_JH080N021A0)
    {
        s32Ret = AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg_LCD_JH080N021A0);
    }
    else
    {
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
