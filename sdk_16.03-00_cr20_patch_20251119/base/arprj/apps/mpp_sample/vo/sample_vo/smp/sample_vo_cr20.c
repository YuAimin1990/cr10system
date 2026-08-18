#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/select.h>
#include <sys/time.h>

#include "sample_comm.h"
#include "ar_mipi_tx.h"
#include "../sample_vo.h"
#include "sample_vo_config.h"
#include "hal_gpio.h"
#include "hal_vo.h"
#include "osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* End of #ifdef __cplusplus */

/*********log marco*********/
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define vo_print(format, ...) ar_printf("\033[0m[INFO ]<%s: %s():%d> " format "\033[0m", __FILENAME__, __func__, __LINE__, ##__VA_ARGS__)
#define vo_error(format, ...) ar_printf("\033[0;31m[ERROR]<%s: %s():%d> " format "\033[0m", __FILENAME__, __func__, __LINE__, ##__VA_ARGS__)
/*********log marco end*********/

extern AR_BOOL is_running;

#define lcd_backlight_port PORT_D
#define lcd_backlight_group GROUP_3
#define lcd_backlight_number 4

#define lcd_power_port PORT_D
#define lcd_power_group GROUP_1
#define lcd_power_number 5

#define lcd_rest_port PORT_C
#define lcd_rest_group GROUP_1
#define lcd_rest_number 4

static int sample_vo_dsi_short_cmd(VO_DEV VoDev, uint8_t data_type, uint8_t param1, uint8_t param2, int delayms)
{
    VO_DSI_CMD_S pstCmd;

    pstCmd.u16DataType = data_type;
    pstCmd.u16CmdSize = param2 << 8 | param1;
    pstCmd.pu8Cmd = NULL;
    AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);
    ar_delay(delayms);
    return 0;
}

static int sample_vo_dsi_long_cmd(VO_DEV VoDev, uint8_t data_type, uint8_t *data, uint16_t size, int delayms)
{
    VO_DSI_CMD_S pstCmd;

    pstCmd.u16DataType = data_type;
    pstCmd.u16CmdSize = size;
    pstCmd.pu8Cmd = data;
    AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);
    ar_delay(delayms);
    return 0;
}

static AR_S32 sample_vo_init_mipi_display_400x960(VO_DEV VoDev)
{
    vo_print("config dsi cmd\n");
    uint8_t para0[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0), 2);

    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xEF, 0x08, 2);

    uint8_t para2[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x10};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2), 2);

    uint8_t para3[] = {0xC0, 0x77, 0x00};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3), 2);

    uint8_t para4[] = {0xC1, 0x0E, 0x0C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4), 2);

    uint8_t para5[] = {0xC2, 0x07, 0x02};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para5, sizeof(para5), 2);

    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCC, 0x30, 2);

    uint8_t para7[] = {0xB0, 0x00, 0x17, 0x1F, 0x0E, 0x11, 0x06, 0x0D, 0x08, 0x07, 0x26, 0x03, 0x11, 0x0F, 0x2A, 0x31, 0x1C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para7, sizeof(para7), 8);

    uint8_t para8[] = {0xB1, 0x00, 0x17, 0x1F, 0x0D, 0x11, 0x07, 0x0C, 0x08, 0x08, 0x26, 0x04, 0x11, 0x0F, 0x2A, 0x31, 0x1C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8), 8);

    uint8_t para9[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x11};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9), 2);

    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB0, 0x5C, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB1, 0x68, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB2, 0x85, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB3, 0x80, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB5, 0x49, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB7, 0x87, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB8, 0x22, 2);

    uint8_t para17[] = {0xB9, 0x10, 0x1F};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para17, sizeof(para17), 2);

    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xBB, 0x03, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC1, 0x88, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xC2, 0x88, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xD0, 0x88, 2);

    uint8_t para22[] = {0xE0, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para22, sizeof(para22), 2);

    uint8_t para23[] = {0xE1, 0x03, 0x96, 0x05, 0x96, 0x02, 0x96, 0x04, 0x96, 0x00, 0x44, 0x44};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para23, sizeof(para23), 2);

    uint8_t para24[] = {0xE2, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para24, sizeof(para24), 2);

    uint8_t para25[] = {0xE3, 0x00, 0x00, 0x33, 0x33};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para25, sizeof(para25), 2);

    uint8_t para26[] = {0xE4, 0x44, 0x44};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para26, sizeof(para26), 2);

    uint8_t para27[] = {0xE5, 0x0B, 0xD4, 0x28, 0x8C, 0x0D, 0xD6, 0x28, 0x8C, 0x07, 0xD0, 0x28, 0x8C, 0x09, 0xD2, 0x28, 0x8C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para27, sizeof(para27), 8);

    uint8_t para28[] = {0xE6, 0x00, 0x00, 0x33, 0x33};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para28, sizeof(para28), 2);

    uint8_t para29[] = {0xE7, 0x44, 0x44};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para29, sizeof(para29), 2);

    uint8_t para30[] = {0xE8, 0x0A, 0xD5, 0x28, 0x8C, 0x0C, 0xD7, 0x28, 0x8C, 0x06, 0xD1, 0x28, 0x8C, 0x08, 0xD3, 0x28, 0x8C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para30, sizeof(para30), 8);

    uint8_t para31[] = {0xEB, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x00};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para31, sizeof(para31), 2);

    uint8_t para32[] = {0xED, 0xFF, 0x45, 0x67, 0xFC, 0x01, 0x3F, 0xAB, 0xFF, 0xFF, 0xBA, 0xF3, 0x10, 0xCF, 0x76, 0x54, 0xFF};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para32, sizeof(para32), 8);

    uint8_t para33[] = {0xEF, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para33, sizeof(para33), 2);

    uint8_t para34[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para34, sizeof(para34), 2);

    uint8_t para35[] = {0xE8, 0x00, 0x0E};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para35, sizeof(para35), 2);

    uint8_t para36[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x00};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para36, sizeof(para36), 2);

    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11, 0x00, 12);

    uint8_t para37[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para37, sizeof(para37), 2);

    uint8_t para38[] = {0xE8, 0x00, 0x0C};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para38, sizeof(para38), 2);

    uint8_t para39[] = {0xE8, 0x40, 0x00};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para39, sizeof(para39), 2);

    uint8_t para40[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x00};
    sample_vo_dsi_long_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para40, sizeof(para40), 2);

    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29, 0x00, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x36, 0x00, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x3A, 0x77, 2);
    sample_vo_dsi_short_cmd(VoDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0x35, 0x00, 2);

    ar_delay(10);
    return 0;
}

static int lcd_power(int on)
{
    int gpio_num = ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
    ar_hal_gpio_set_value(gpio_num, on);
    vo_print("lcd power %s\n", on ? "on" : "off");

    return 0;
}

static int lcd_reset(int delay1, int delay2, int delay3)
{
    int gpio_num = ar_hal_gpio_name_to_num(lcd_rest_group, lcd_rest_port, lcd_rest_number);
    ar_hal_gpio_set_value(gpio_num, 1);
    ar_delay(delay1);
    ar_hal_gpio_set_value(gpio_num, 0);
    ar_delay(delay2);
    ar_hal_gpio_set_value(gpio_num, 1);
    ar_delay(delay3);
    vo_print("lcd reset\n");

    return 0;
}

static int lcd_backlight(int on)
{
    int gpio_num = ar_hal_gpio_name_to_num(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number);
    ar_hal_gpio_set_value(gpio_num, on);
    vo_print("lcd backlight %s\n", on ? "on" : "off");

    return 0;
}

static int display_power_on(void)
{
    int power_gpio = ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
    int reset_gpio = ar_hal_gpio_name_to_num(lcd_rest_group, lcd_rest_port, lcd_rest_number);
    int backlight_gpio = ar_hal_gpio_name_to_num(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number);
    ar_hal_gpio_export(power_gpio);
    ar_hal_gpio_set_dir(power_gpio, OUTPUT);
    ar_hal_gpio_export(reset_gpio);
    ar_hal_gpio_set_dir(reset_gpio, OUTPUT);
    ar_hal_gpio_export(backlight_gpio);
    ar_hal_gpio_set_dir(backlight_gpio, OUTPUT);

    // pull down all gpio;
    lcd_backlight(0);
    ar_hal_gpio_set_value(reset_gpio, 0);
    lcd_power(0);

    ar_delay(10); // delay 10ms
    // power on
    lcd_power(1);

    return 0;
}

static AR_S32 sample_vo_dev_layer_enable(VO_DEV VoDev, VO_LAYER VoLayer)
{
    AR_S32 s32Ret = 0;
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
    AR_U32 u32Framerate = 0;
    SIZE_S stDevSize = {0};

    stPubAttr.u32BgColor = COLOR_RGB_YELLOW;
    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    u32Framerate = 60;
    s32Ret = AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_SetDevFrameRate failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = AR_TRUE;
    stPubAttr.stSyncInfo.bIop = AR_TRUE;
    stPubAttr.stSyncInfo.u8Intfb = 0;

    stPubAttr.stSyncInfo.u16Hpw = 6;
    stPubAttr.stSyncInfo.u16Hbb = 32;
    stPubAttr.stSyncInfo.u16Hact = 400;
    stPubAttr.stSyncInfo.u16Hfb = 32;

    stPubAttr.stSyncInfo.u16Vpw = 2;
    stPubAttr.stSyncInfo.u16Vbb = 16;
    stPubAttr.stSyncInfo.u16Vact = 960;
    stPubAttr.stSyncInfo.u16Vfb = 16;

    stPubAttr.stSyncInfo.u16Hmid = 0;
    stPubAttr.stSyncInfo.u16Bvact = 0;
    stPubAttr.stSyncInfo.u16Bvbb = 0;
    stPubAttr.stSyncInfo.u16Bvfb = 0;
    stPubAttr.stSyncInfo.bIdv = AR_FALSE;
    stPubAttr.stSyncInfo.bIhs = AR_FALSE;
    stPubAttr.stSyncInfo.bIvs = AR_FALSE;
    stPubAttr.enIntfType = VO_INTF_MIPI;

    s32Ret = AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_SetPubAttr failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    stUserInfo.bClkReverse = AR_TRUE;
    stUserInfo.u32DevDiv = 1;
    stUserInfo.u32PreDiv = 1;
    stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

    s32Ret = AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_SetUserIntfSyncInfo failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VO_Enable(VoDev);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_Enable failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    // lcd display power on
    display_power_on();

    VO_DSI_ATTR_S stDsiCfg =
    {
        .u32Lane = {1, 1},
        .u32BitsPerPixel = 24,
        .stSyncInfo = {
            .u32Hsa = 9,
            .u32Hbp = 48,
            .u32Hact = 400,
            .u32Hfp = 48,
            .u32Vsa = 2,
            .u32Vbp = 16,
            .u32Vact = 960,
            .u32Vfp = 16},
        .fDphyClkMhz = 336.3696,
        .u32PllFreqReg2c0 = 0x2F9114D6,
        .u32PllFreqReg38c = 0x2800
    };

    s32Ret = AR_MPI_VO_Dsi_SetAttr(VoDev, &stDsiCfg);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_Dsi_SetAttr failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    lcd_reset(15, 3, 130);
    sample_vo_init_mipi_display_400x960(VoDev);

    s32Ret = AR_MPI_VO_Dsi_Enable(VoDev);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_Dsi_Enable failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

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

    stLayerAttr.u32DispFrmRt = 60;

    vo_print("vo layer: x%d y%d w%d h%d imgW%d imgH%d fps%.2f\n",
        stLayerAttr.stDispRect.s32X, stLayerAttr.stDispRect.s32Y,
        stLayerAttr.stDispRect.u32Width, stLayerAttr.stDispRect.u32Height,
        stLayerAttr.stImageSize.u32Width, stLayerAttr.stImageSize.u32Height,
        stLayerAttr.u32DispFrmRt);

    s32Ret = AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_SetVideoLayerAttr failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    /* ENABLE VO LAYER */
    s32Ret = AR_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_EnableVideoLayer failed! ret = 0x%x\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static AR_S32 sample_vo_chn_enable(VO_LAYER VoLayer, VO_CHN VoChn, STRU_AR_HAL_VO_RECT VoChnRect)
{
    VO_CHN_ATTR_S astChnAttr = {0};

    astChnAttr.bDeflicker = AR_FALSE;
    astChnAttr.u32Priority = 0;
    astChnAttr.stRect.s32X = VoChnRect.x;
    astChnAttr.stRect.s32Y = VoChnRect.y;
    astChnAttr.stRect.u32Height = VoChnRect.h;
    astChnAttr.stRect.u32Width = VoChnRect.w;
    vo_print("vo layer %d chn %d rect: [%d %d %d %d]\n",
        VoLayer, VoChn, astChnAttr.stRect.s32X, astChnAttr.stRect.s32Y,
        astChnAttr.stRect.u32Width, astChnAttr.stRect.u32Height);

    CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, VoChn, &astChnAttr), "AR_MPI_VO_SetChnAttr");
    CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, VoChn), "AR_MPI_VO_EnableChn");

    return AR_SUCCESS;
}


AR_S32 sample_vo_init(VO_DEV VoDev, VO_LAYER VoLayer, AR_BOOL abChnEn[],
    STRU_AR_HAL_VO_RECT aChnPosition[], AR_S32 s32ChnNum)
{
    AR_S32 s32Ret = 0;
    s32Ret = sample_vo_dev_layer_enable(VoDev, VoLayer);

    for (int i = 0; i < s32ChnNum; i++)
    {
        if (abChnEn[i])
        {
            s32Ret += sample_vo_chn_enable(VoLayer, i, aChnPosition[i]);
        }
    }

    return s32Ret;
}

AR_S32 sample_vo_deinit(VO_DEV VoDev, VO_LAYER VoLayer, AR_BOOL abChnEn[], AR_S32 s32ChnNum)
{
    AR_S32 s32Ret = 0;
    for (int i = 0; i < s32ChnNum; i++)
    {
        if(abChnEn[i])
        {
            s32Ret = AR_MPI_VO_DisableChn(VoLayer, i);
            if (s32Ret)
            {
                vo_error("AR_MPI_VO_DisableChn layer %d chn %d failed, ret = 0x%x\n", VoLayer, i);
            }
        }
    }

    s32Ret = AR_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_DisableVideoLayer layer %d failed, ret = 0x%x\n", VoLayer);
    }

    s32Ret = AR_MPI_VO_Disable(VoDev);
    if (s32Ret)
    {
        vo_error("AR_MPI_VO_Disable dev %d failed, ret = 0x%x\n", VoDev);
    }

    return s32Ret;
}

AR_S32 SAMPLE_VO_MIPILCD_400x900(AR_VOID)
{
    AR_S32 ret = 0;
    VO_DEV VoDev = 0;
    VO_LAYER VoLayer = 0;
    AR_BOOL abVoChnEn[MAX_VO_CHN_NUM] = {0};
    STRU_AR_HAL_VO_RECT VoChnRect[MAX_VO_CHN_NUM] = {0};
    AR_S32 VoChnNum = 1;
    AR_U32 VoAlignNum = 512;

    /* config vb and init sys */
    VB_CONFIG_S stVbConf = {0};
    AR_U32 u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, VoAlignNum);
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
    stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
    stVbConf.u32MaxPoolCnt++;
    CHECK_RET(SAMPLE_COMM_SYS_Init(&stVbConf), "SAMPLE_COMM_SYS_Init");

    abVoChnEn[0] = AR_TRUE;
    VoChnRect[0].x = 0;
    VoChnRect[0].y = 0;
    VoChnRect[0].w = 400;
    VoChnRect[0].h = 960;
    sample_vo_init(VoDev, VoLayer, abVoChnEn, VoChnRect, VoChnNum);

    // request vb blk for frame
    AR_U64 u64BlkSize = CAM_ALIGNE_TO(VoChnRect[0].w, VoAlignNum) * VoChnRect[0].h * 3;
    VB_BLK VbBlkHandle =  AR_MPI_VB_GetBlock(POOL_OWNER_COMMON, u64BlkSize, NULL);
    if (VbBlkHandle == VB_INVALID_HANDLE) {
        vo_error("AR_MPI_VB_GetBlock pool %d size %ld failed!\n", POOL_OWNER_COMMON, u64BlkSize);
        return -1;
    }

    vo_print("get vb blk success! blk size %d\n", u64BlkSize);

    VB_POOL VbPoolId = AR_MPI_VB_Handle2PoolId(VbBlkHandle);
    if (VbPoolId < 0)
    {
        vo_error("AR_MPI_VB_Handle2PoolId %d failed!\n", VbBlkHandle);
        AR_MPI_VB_ReleaseBlock(VbBlkHandle);
        return -1;
    }

    AR_U64 u64PhyAddr = AR_MPI_VB_Handle2PhysAddr(VbBlkHandle);
    if (u64PhyAddr == 0)
    {
        vo_error("AR_MPI_VB_Handle2PhysAddr %d failed!\n", VbBlkHandle);
        AR_MPI_VB_ReleaseBlock(VbBlkHandle);
        return -1;
    }

    // mmap to virtual addr
    ret = AR_MPI_VB_MmapPool(VbPoolId);
    if (ret)
    {
        vo_error("AR_MPI_VB_MmapPool %d failed!\n", VbPoolId);
        AR_MPI_VB_ReleaseBlock(VbBlkHandle);
        return -1;
    }

    AR_VOID *pVirAddr = NULL;
    ret = AR_MPI_VB_GetBlockVirAddr(VbPoolId, u64PhyAddr, (AR_VOID **)&pVirAddr);
    if (ret)
    {
        vo_error("AR_MPI_VB_GetBlockVirAddr %d 0x%x failed!\n", VbPoolId, u64PhyAddr);
        AR_MPI_VB_MunmapPool(VbPoolId);
        AR_MPI_VB_ReleaseBlock(VbBlkHandle);
        return -1;
    }

    VIDEO_FRAME_INFO_S *pstFrameInfo = malloc(sizeof(VIDEO_FRAME_INFO_S));
    pstFrameInfo->u32PoolId = VbPoolId;
    pstFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    pstFrameInfo->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    pstFrameInfo->stVFrame.u32Width = VoChnRect[0].w;
    pstFrameInfo->stVFrame.u32Height = VoChnRect[0].h;
    pstFrameInfo->stVFrame.u32Stride[0] = CAM_ALIGNE_TO(VoChnRect[0].w, VoAlignNum);
    pstFrameInfo->stVFrame.u32Stride[1] = CAM_ALIGNE_TO(VoChnRect[0].w, VoAlignNum) / 2;
    pstFrameInfo->stVFrame.u32Stride[2] = CAM_ALIGNE_TO(VoChnRect[0].w, VoAlignNum) / 2;
    pstFrameInfo->stVFrame.u32Len[0] = pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height;
    pstFrameInfo->stVFrame.u32Len[1] = pstFrameInfo->stVFrame.u32Stride[1] * pstFrameInfo->stVFrame.u32Height / 2;
    pstFrameInfo->stVFrame.u32Len[2] = pstFrameInfo->stVFrame.u32Stride[2] * pstFrameInfo->stVFrame.u32Height / 2;
    pstFrameInfo->stVFrame.u64PhyAddr[0] = u64PhyAddr;
    pstFrameInfo->stVFrame.u64PhyAddr[1] = pstFrameInfo->stVFrame.u64PhyAddr[0] + pstFrameInfo->stVFrame.u32Len[0];
    pstFrameInfo->stVFrame.u64PhyAddr[2] = pstFrameInfo->stVFrame.u64PhyAddr[1] + pstFrameInfo->stVFrame.u32Len[1];
    pstFrameInfo->stVFrame.u64VirAddr[0] = (AR_U64)pVirAddr;
    pstFrameInfo->stVFrame.u64VirAddr[1] = pstFrameInfo->stVFrame.u64VirAddr[0] + pstFrameInfo->stVFrame.u32Len[1];
    pstFrameInfo->stVFrame.u64VirAddr[2] = pstFrameInfo->stVFrame.u64VirAddr[1] + pstFrameInfo->stVFrame.u32Len[2];

    vo_print("frame phy addr[0x%llx 0x%llx 0x%llx], vir addr [0x%llx 0x%llx 0x%llx]\n",
        pstFrameInfo->stVFrame.u64PhyAddr[0], pstFrameInfo->stVFrame.u64PhyAddr[1], pstFrameInfo->stVFrame.u64PhyAddr[2],
        pstFrameInfo->stVFrame.u64VirAddr[0], pstFrameInfo->stVFrame.u64VirAddr[1], pstFrameInfo->stVFrame.u64VirAddr[2]);

    vo_print("frame info: pool id = %d, fmt = %d, compress mode = %d, w %d h %d stride[%d %d %d], len [%d %d %d]\n", 
        pstFrameInfo->u32PoolId, pstFrameInfo->stVFrame.enPixelFormat, pstFrameInfo->stVFrame.enCompressMode,
        pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height,
        pstFrameInfo->stVFrame.u32Stride[0], pstFrameInfo->stVFrame.u32Stride[1], pstFrameInfo->stVFrame.u32Stride[2],
        pstFrameInfo->stVFrame.u32Len[0], pstFrameInfo->stVFrame.u32Len[1], pstFrameInfo->stVFrame.u32Len[2]);

    // read yuv to frame buffer
    char yuv_name[64] = "/usrdata/test_rc_400x960_stride512.yuv";
    FILE *fp = fopen(yuv_name, "rb");
    if (!fp)
    {
        vo_error("open %s failed!\n", yuv_name);
        ret = -1;
        goto END;
    }

    AR_S32 frame_len = pstFrameInfo->stVFrame.u32Len[0] + pstFrameInfo->stVFrame.u32Len[1] + pstFrameInfo->stVFrame.u32Len[2];
    ret = fread((AR_VOID *)pstFrameInfo->stVFrame.u64VirAddr[0], 1, frame_len, fp);
    vo_print("read %s %d bytes done\n", yuv_name, ret);

    vo_print("start send frame to vo...\n");
    AR_BOOL bVoFirstFrame = AR_TRUE;
    while (is_running)
    {
        ret = AR_MPI_VO_SendFrame(VoLayer, 0, pstFrameInfo, 300);
        if (ret)
        {
            vo_error("AR_MPI_VO_SendFrame %d %d failed! ret = 0x%x\n", VoLayer, 0, ret);
        }
        ar_usleep(20*1000);
        if (bVoFirstFrame)
        {
            lcd_backlight(1);
            bVoFirstFrame = AR_FALSE;
        }
    }

END:
    if(pstFrameInfo) free(pstFrameInfo);
    if(fp) fclose(fp);
    AR_MPI_VB_MunmapPool(VbPoolId);
    AR_MPI_VB_ReleaseBlock(VbBlkHandle);
    sample_vo_deinit(VoDev, VoLayer, abVoChnEn, VoChnNum);
    SAMPLE_COMM_SYS_Exit();

    return ret;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */