
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "mpi_vb.h"
#include "hal_vb.h"
#include "osal.h"
#include "hal_gpio.h"
#include "mpi_gdc_api.h"
#ifdef AR_FEAT_EIS
#include "hal_eis_algo.h"
#include "sample_eis.h"
#endif

#include <sys/prctl.h>
#include "ar_common.h"
#include "ar_comm_video.h"
#include "sample_lcd.h"


#include "hal_sys.h"
#include "mpi_sysctl.h"

#define lcd_backlight_port PORT_B
#define lcd_backlight_group GROUP_1
#define lcd_backlight_number 0
#define lcd_backlight_pad 30
#define lcd_backlight_pad_gpio_fun_num 2

#define lcd_power_port PORT_B
#define lcd_power_group GROUP_1
#define lcd_power_number 2
#define lcd_power_pad 32
#define lcd_power_pad_gpio_fun_num 2

#define lcd_standby_port PORT_A
#define lcd_standby_group GROUP_1
#define lcd_standby_number 6
#define lcd_standby_pad 28
#define lcd_standby_pad_gpio_fun_num 2

#define lcd_rest_port PORT_B
#define lcd_rest_group GROUP_1
#define lcd_rest_number 1
#define lcd_rest_pad 31
#define lcd_rest_pad_gpio_fun_num 2

#define lcd_updown_scan_port PORT_D
#define lcd_updown_scan_group GROUP_1
#define lcd_updown_scan_number 7
#define lcd_updown_scan_pad 26
#define lcd_updown_pad_gpio_fun_num 3

static VO_DSI_ATTR_S pstDsiCfg =
    {
        .u32Lane = {1, 1, 1, 1},
        .u32BitsPerPixel = 24,
        // .stSyncInfo        = {
        //     .u32Hsa  = 72,
        //     .u32Hbp  = 162,
        //     .u32Hact = 1024,
        //     .u32Hfp  = 162,

        //     .u32Vsa  = 10,
        //     .u32Vbp  = 23,
        //     .u32Vact = 600,
        //     .u32Vfp  = 12
        // },
        // .fDphyClkMhz      = 360.3744,
        // .u32PllFreqReg2c0 = 0x2c65f509,
        // .u32PllFreqReg38c = 0x2800
        .stSyncInfo = {
            .u32Hsa = 6,
            .u32Hbp = 39,
            .u32Hact = 720,
            .u32Hfp = 39,

            .u32Vsa = 4,
            .u32Vbp = 21,
            .u32Vact = 1440,
            .u32Vfp = 17},
        .fDphyClkMhz = 443.88864,
        .u32PllFreqReg2c0 = 0x240B8A22,
        .u32PllFreqReg38c = 0x2800};

#define CHECK_RET(express, name)                                                                 \
do                                                                                           \
{                                                                                            \
    AR_S32 Ret;                                                                              \
    Ret = express;                                                                           \
    if (Ret != AR_SUCCESS)                                                                   \
    {                                                                                        \
        printf("%s failed at %s : LINE: %d with %#x!\n", name, __FUNCTION__, __LINE__, Ret); \
        AR_MPI_SYS_Exit();                                                                   \
        AR_MPI_VB_Exit();                                                                    \
        return Ret;                                                                          \
    }                                                                                        \
} while (0)

int VO_SysCall(const char *cmd, char *buff, int bufsize)
{
    FILE *fp = NULL;
    // printf("==> %s\n", cmd);

    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        printf("popen error\n");
        return -1;
    }

    int ret = 0;
    if ((buff != NULL) && (bufsize > 0))
    {
        ret = fread(buff, 1, bufsize, fp);
    }

    if (ret >= 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    pclose(fp);

    return ret;
}

int VO_GPIO_SetValue(int pin, int value)
{
    char dir_tmp[128] = {0};
    char cmd[128] = {0};
    int ret = 0;

    sprintf(dir_tmp, "/sys/class/gpio/gpio%d", pin);
    if (access(dir_tmp, F_OK) != 0)
    {
        snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/export", pin);
        VO_SysCall(cmd, NULL, 0);
    }

    snprintf(cmd, sizeof(cmd), "cat /sys/class/gpio/gpio%d/direction", pin);
    char buff[30] = {0};
    ret = VO_SysCall(cmd, buff, sizeof(buff));
    if ((ret != 0) || (strlen(buff) == 0))
    {
        printf("%s failed\n", cmd);
        return -1;
    }
    if (strncmp(buff, "in", 2) == 0)
    {
        snprintf(cmd, sizeof(cmd), "echo out > /sys/class/gpio/gpio%d/direction", pin);
        // printf("1 cmd:%s, buff:%s", cmd, buff);
        VO_SysCall(cmd, NULL, 0);
    }
    else
    {
        // printf("1 buff:%s", buff);
    }

    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value", value, pin);
    // printf("1 cmd:%s\n", cmd);
    VO_SysCall(cmd, NULL, 0);

    return 0;
}

void VO_7INCH_LCD_Init(void)
{
#if defined(AR9341)
    int backlight = 40;
    int power_gpio = 42;
    int standby_gpio = 38;
    int reset_gpio = 41;
    int updown_gpio = 63;
#endif

#if defined(PROXIMA)
    int backlight = 58;
    int power_gpio = 57;
    int standby_gpio = 53;
    int reset_gpio = 56;
    int updown_gpio = 54;
#endif

    printf("!!!!!!!!!!!!!!!! 7inch lcd init\n");

    VO_GPIO_SetValue(power_gpio, 1);
    VO_GPIO_SetValue(backlight, 1);

    // standby the lcd
    VO_GPIO_SetValue(updown_gpio, 0);
    VO_GPIO_SetValue(standby_gpio, 1);

    usleep(10000);

    VO_GPIO_SetValue(reset_gpio, 1);
    usleep(1000);
    VO_GPIO_SetValue(reset_gpio, 0);
    usleep(10000);
    VO_GPIO_SetValue(reset_gpio, 1);

    // final we backlight the lcd
    VO_GPIO_SetValue(backlight, 1);
    return;
}

int ar_vo_dev_init_for_mipi(STRU_DISPLAY_OBJ_T *vo_obj, int width, int height, int y_stride,
                            int uv_stride, float fps, AR_BOOL is_mipi_tx, AR_BOOL is_interlace)
{
    AR_U32 i = 0;
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    AR_S32 VoChnNum = 1;
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
    AR_U32 u32Framerate = 0;
    SIZE_S stDevSize = {0};
    printf("ar_vo_dev_init_for_mipi: width:%d, height:%d, y_stride:%d, uv_stride:%d, fps:%f, is_mipi_tx:%d, is_interlace:%d\n", width, height, y_stride, uv_stride, fps, is_mipi_tx, is_interlace);
    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = 0;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.bIop = 1;

    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    stPubAttr.stSyncInfo.bIdv = 0;
    stPubAttr.stSyncInfo.bIhs = 0;
    stPubAttr.stSyncInfo.bIvs = 0;

    /* USER SET VO FRAME RATE */
    u32Framerate = 60;
    CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");

    /* USER SET VO DEV SYNC INFO */
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = AR_TRUE;
    stPubAttr.stSyncInfo.bIop = AR_TRUE;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.u16Hpw = 96;
    stPubAttr.stSyncInfo.u16Hbb = 216;
    stPubAttr.stSyncInfo.u16Hact = 1024;
    stPubAttr.stSyncInfo.u16Hfb = 216;
    stPubAttr.stSyncInfo.u16Vpw = 10;
    stPubAttr.stSyncInfo.u16Vbb = 23;
    stPubAttr.stSyncInfo.u16Vact = 600;
    stPubAttr.stSyncInfo.u16Vfb = 12;
    stPubAttr.stSyncInfo.u16Hmid = 0;
    stPubAttr.stSyncInfo.u16Bvact = 0;
    stPubAttr.stSyncInfo.u16Bvbb = 0;
    stPubAttr.stSyncInfo.u16Bvfb = 0;
    stPubAttr.stSyncInfo.bIdv = AR_FALSE;
    stPubAttr.stSyncInfo.bIhs = AR_FALSE;
    stPubAttr.stSyncInfo.bIvs = AR_FALSE;
    stPubAttr.enIntfType = VO_INTF_MIPI;
    stPubAttr.enIntfType |= VO_INTF_LCD_24BIT;

    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

    /* USER SET VO SYNC INFO OF USER INTF */
    stUserInfo.bClkReverse = AR_TRUE;
    stUserInfo.u32DevDiv = 1;
    stUserInfo.u32PreDiv = 1;
    stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

    CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

    /* SET LCD */

    CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg), "AR_MPI_VO_Dsi_SetAttr");
    CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");
    VO_7INCH_LCD_Init();

    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = width;	  // stPubAttr.stSyncInfo.u16Hact;
    stDevSize.u32Height = height; // stPubAttr.stSyncInfo.u16Vact;

    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width = stDevSize.u32Width;

    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;

    stLayerAttr.u32DispFrmRt = 30;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");
}

#define GPIO_DATA_HIGH 1
#define GPIO_DATA_LOW 0

// LCD_VCC_EN GPIO_D1_5
#define lcd_power_port PORT_D
#define lcd_power_group GROUP_1
#define lcd_power_number 5

// LCD_RST GPIO_C1_4
#define lcd_rest_port PORT_C
#define lcd_rest_group GROUP_1
#define lcd_rest_number 4

// LCD_PWM GPIO_D3_4
#define lcd_backlight_port PORT_D
#define lcd_backlight_group GROUP_3
#define lcd_backlight_number 4

static int backlight_lcd(int on)
{
#ifdef BACKLIGHT_PWM
    printf("backlight_lcd: BACKLIGHT_PWM %d\n", on);
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
    printf("backlight_lcd: GPIO %d\n", on);
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
    printf("power_lcd: %d\n", on);
    int gpio_num = ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
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

    return 0;
}

static int reset_lcd(int delay1, int delay2, int delay3)
{
    printf("reset_lcd: %d %d %d\n", delay1, delay2, delay3);
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

static AR_S32 power_on_st7703(void)
{
    printf("power_on_st7703\n");
    power_lcd(0);
    usleep(20 * 1000); // sleep 20
    power_lcd(1);
    backlight_lcd(0);

    return 0;
}

static int SAMPLE_VO_dsi_short_cmd(VO_DEV voDev, uint8_t data_type, uint8_t param1, uint8_t param2, int delayms)
{
    VO_DSI_CMD_S pstCmd;
    printf("SAMPLE_VO_dsi_short_cmd: %d %d %d %d %d\n", voDev, data_type, param1, param2, delayms);
    pstCmd.u16DataType = data_type;
    pstCmd.u16CmdSize = param2 << 8 | param1;
    pstCmd.pu8Cmd = NULL;
    AR_MPI_VO_Dsi_Cmd(voDev, &pstCmd);
    ar_delay(delayms);
    return 0;
}

static int SAMPLE_VO_dsi_long_cmd(VO_DEV voDev, uint8_t data_type, uint8_t *data, uint16_t size, int delayms)
{
    VO_DSI_CMD_S pstCmd;

    pstCmd.u16DataType = data_type;
    pstCmd.u16CmdSize = size;
    pstCmd.pu8Cmd = data;
    AR_MPI_VO_Dsi_Cmd(voDev, &pstCmd);
    ar_delay(delayms);
    return 0;
}

static AR_S32 init_mipi_port_st7703(VO_DEV voDev)
{
    int delayms = 10;
    printf("%s: line:%d init mipi port:  LCD_ST7703\n", __FILE__, __LINE__);

    uint8_t para0[] = {0xB9, 0xF1, 0x12, 0x83};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0), delayms);

    uint8_t para1[] = {0xBA, 0x33, 0x81, 0x05, 0xF9, 0x0e, 0x0e,
                        0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x44, 0x25, 0x00, 0x91, 0x0a, 0x00, 0x00, 0x02,
                        0x4F, 0x11, 0x00, 0x00, 0x37};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1), delayms);

    uint8_t para2[] = {0xB8, 0x25, 0x22, 0x20, 0x03};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2), delayms);

    uint8_t para3[] = {0xB3, 0x10, 0x10, 0x05, 0x05, 0x03, 0xFF,
                        0x00, 0x00, 0x00, 0x00};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3), delayms);

    uint8_t para4[] = {0xC0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00,
                        0x08, 0x70, 0x00};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4), delayms);

    SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xBC, 0x4E, delayms);
    SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCC, 0x0B, delayms);
    SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB4, 0x80, delayms);

    uint8_t para8[] = {0xB2, 0xF0, 0x12, 0x30};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8), delayms);

    uint8_t para9[] = {0xE3, 0x07, 0x07, 0x0B, 0x0B, 0x03, 0x0B,
                        0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xC0, 0x10};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9), delayms);

    uint8_t para10[] = {0xC1, 0x54, 0x00, 0x1E, 0x1E, 0x77, 0xF1,
                        0xFF, 0xFF, 0xCC, 0xCC, 0x77, 0x77};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10), delayms);

    uint8_t para11[] = {0xB5, 0x0D, 0x0D};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11), delayms);

    uint8_t para12[] = {0xB6, 0x43, 0x43};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12), delayms);

    uint8_t para13[] = {0xBF, 0x02, 0x11, 0x00};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13), delayms);

    uint8_t para14[] = {0xE9, 0x82, 0x10, 0x06, 0x05, 0x9E, 0x0A,
                        0xA5, 0x12, 0x31, 0x23, 0x37, 0x83, 0x04, 0xBC,
                        0x27, 0x38, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00,
                        0x0C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x75, 0x75,
                        0x31, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x13,
                        0x88, 0x64, 0x64, 0x20, 0x88, 0x88, 0x88, 0x88,
                        0x88, 0x88, 0x02, 0x88, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14), delayms);

    uint8_t para15[] = {0xEA, 0x02, 0x21, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x46,
                        0x02, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x64,
                        0x88, 0x13, 0x57, 0x13, 0x88, 0x88, 0x88, 0x88,
                        0x88, 0x88, 0x75, 0x88, 0x23, 0x04, 0x00, 0x00,
                        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x30, 0x0A, 0xA5, 0x00, 0x00, 0x00, 0x00};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para15, sizeof(para15), delayms);

    uint8_t para16[] = {0xE0, 0x00, 0x0F, 0x1A, 0x2D, 0x32, 0x3C,
                        0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13, 0x11,
                        0x13, 0x12, 0x19, 0x00, 0x0F, 0x1A, 0x2D, 0x32,
                        0x3C, 0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13,
                        0x11, 0x13, 0x12, 0x19};
    SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para16, sizeof(para16), delayms);

    delayms = 250;
    SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11, 0x00, delayms); // Sleep Out
    delayms = 50;
    SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29, 0x00, delayms); // Display On

    printf("%s: line:%d init mipi port: LCD_ST7703 end!\n", __FILE__, __LINE__);
    return;
}

AR_S32 init_display_st7703(VO_DEV VoDev)
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
init_mipi_port_st7703(VoDev);
#endif

    backlight_lcd(1);
    return 0;
}

int ar_vo_dev_init_for_mipi_720_1440(STRU_DISPLAY_OBJ_T *vo_obj, int width, int height, int y_stride,
                                        int uv_stride, float fps, AR_BOOL is_mipi_tx, AR_BOOL is_interlace)
{
    AR_U32 i = 0;
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    AR_S32 VoChnNum = 1;
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
    AR_U32 u32Framerate = 0;
    SIZE_S stDevSize = {0};
    printf("ar_vo_dev_init_for_mipi_720_1440: width:%d, height:%d, y_stride:%d, uv_stride:%d, fps:%f, is_mipi_tx:%d, is_interlace:%d\n", width, height, y_stride, uv_stride, fps, is_mipi_tx, is_interlace);
    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    /* USER SET VO FRAME RATE */
    u32Framerate = 60;
    CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");

    /* USER SET VO DEV SYNC INFO */
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = AR_TRUE;
    stPubAttr.stSyncInfo.bIop = AR_TRUE;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.u16Hpw = 8;
    stPubAttr.stSyncInfo.u16Hbb = 52;
    stPubAttr.stSyncInfo.u16Hact = 720;
    stPubAttr.stSyncInfo.u16Hfb = 52;
    stPubAttr.stSyncInfo.u16Vpw = 4;
    stPubAttr.stSyncInfo.u16Vbb = 21;
    stPubAttr.stSyncInfo.u16Vact = 1440;
    stPubAttr.stSyncInfo.u16Vfb = 17;
    stPubAttr.stSyncInfo.u16Hmid = 0;
    stPubAttr.stSyncInfo.u16Bvact = 0;
    stPubAttr.stSyncInfo.u16Bvbb = 0;
    stPubAttr.stSyncInfo.u16Bvfb = 0;
    stPubAttr.stSyncInfo.bIdv = AR_FALSE;
    stPubAttr.stSyncInfo.bIhs = AR_FALSE;
    stPubAttr.stSyncInfo.bIvs = AR_FALSE;
    stPubAttr.enIntfType = VO_INTF_MIPI;

    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

    /* USER SET VO SYNC INFO OF USER INTF */
    stUserInfo.bClkReverse = AR_TRUE;
    stUserInfo.u32DevDiv = 1;
    stUserInfo.u32PreDiv = 1;
    stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

    CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

    // power on display
    power_on_st7703();
    printf("power_on_st7703 \r\n");

    /* SET LCD */
    CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg), "AR_MPI_VO_Dsi_SetAttr");
    reset_lcd(15, 3, 130);

    /* display st7703 init */
    init_display_st7703(VoDev);
    printf("init_display_st7703 \r\n");

    CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");
    // VO_7INCH_LCD_Init();

    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = stPubAttr.stSyncInfo.u16Hact;
    stDevSize.u32Height = stPubAttr.stSyncInfo.u16Vact;

    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width = stDevSize.u32Width;

    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;
    printf("x%d y%d h%d w%d imgH%d imgW%d \n", stLayerAttr.stDispRect.s32X, stLayerAttr.stDispRect.s32Y,
            stLayerAttr.stDispRect.u32Height, stLayerAttr.stDispRect.u32Width,
            stLayerAttr.stImageSize.u32Height, stLayerAttr.stImageSize.u32Width);

    stLayerAttr.u32DispFrmRt = 60;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");
}

AR_S32 ar_vo_dev_init(STRU_DISPLAY_OBJ_T *vo_obj,
                        AR_S32 width,
                        AR_S32 height,
                        AR_S32 y_stride,
                        AR_S32 uv_stride)
{
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    AR_S32 VoChnNum = 1;
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    SIZE_S stDevSize = {0};
    printf("ar_vo_dev_init: width:%d, height:%d, y_stride:%d, uv_stride:%d\n", width, height, y_stride, uv_stride);
    VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM] = {0};

    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = 0;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.bIop = 1;

    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    stPubAttr.stSyncInfo.bIdv = 0;
    stPubAttr.stSyncInfo.bIhs = 0;
    stPubAttr.stSyncInfo.bIvs = 0;

    stPubAttr.enIntfType = VO_INTF_MIPI; // VO_INTF_HDMI;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = width;
    stDevSize.u32Height = height;

    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width = stDevSize.u32Width;

    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;

    stLayerAttr.u32DispFrmRt = 60;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

    return AR_SUCCESS;
}

AR_S32 ar_vo_surface_init(STRU_DISPLAY_OBJ_T *vo_obj, AR_S32 ch,
                            STRU_AR_HAL_VO_RECT *chan_pos)
{
    AR_S32 VoLayer = 0;
    VO_CHN_ATTR_S astChnAttr = {0};
    printf("ar_vo_surface_init: ch:%d, x:%d, y:%d, w:%d, h:%d\n", ch, chan_pos->x, chan_pos->y, chan_pos->w, chan_pos->h);
    astChnAttr.bDeflicker = AR_FALSE;
    astChnAttr.u32Priority = 0;
    astChnAttr.stRect.s32X = chan_pos->x;
    astChnAttr.stRect.s32Y = chan_pos->y;
    astChnAttr.stRect.u32Height = chan_pos->h;
    astChnAttr.stRect.u32Width = chan_pos->w;
    CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, ch, &astChnAttr), "AR_MPI_VO_SetChnAttr");

    CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, ch), "AR_MPI_VO_EnableChn");

    return AR_SUCCESS;
}

AR_S32 send_to_fw_display(STRU_DISPLAY_OBJ_T *p_display, AR_S32 ch, VIDEO_FRAME_INFO_S *p_frame)
{
    AR_S32 s32Ret = AR_SUCCESS;
    VO_LAYER VoLayer = 0;

    p_frame->stVFrame.u64PTS = 40000;
    p_frame->stVFrame.u32TimeRef = 40000;

    s32Ret = AR_MPI_VO_SendFrame(VoLayer, ch, p_frame, 0);

    return AR_SUCCESS;
}

AR_S32 close_display_surface(STRU_DISPLAY_OBJ_T *vo_obj, AR_S32 ch)
{
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;

    /*DISABLE VO CHN*/
    CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, ch), "AR_MPI_VO_DisableChn");

    return AR_SUCCESS;
}

AR_S32 close_display(STRU_DISPLAY_OBJ_T *vo_obj)
{
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;

    /* DISABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

    /* DISABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

    return AR_SUCCESS;
}
