#if !defined(__GST212W4_CMOS_H_)
#define __GST212W4_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_comm_vi.h"
#include "ar_sns_ctrl.h"
#include "mpi_sys.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_vi.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_vin.h"
#include "osal.h"
#include "gst212w4.h"


#define GST212W4_FPGA            0

#define GST212W4_DUMP_REG_EN     0

#define GST212W4_INIT_OCC_VAL       0x42
//#define GST212W4_CALIB_TARGET       8192


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


static gst212w4_pipe_ctx_t g_gst212w4_pipe_ctx[VI_MAX_PIPE_NUM] = {
    [0 ... VI_MAX_PIPE_NUM-1] = {
        .s8I2cDevId = -1,
        .s32I2cFd = -1,
        .s32NucFd = -1,
        .enCalibState = GST212W4_CALIB_NONE,
        .au8SnsIrParam = {0xff, 0xff, 0xff, 0xff, 0x1f, 0x10, 0x80},
    },
};


static gst212w4_reg_t g_gst212w4_setting[] = {
//   addr, val
    {0x7c, 0xa8},       // to page0;

    {0x02, 0x00},
    {0x03, 0x01},       // x_win_size = 256
    {0x04, 0xc8},
    {0x05, 0x00},       // y_win_size = 200
    {0x06, 0x1e},
    {0x07, 0x00},       // x_blank_size = 30
    {0x08, 0xae},
    {0x09, 0x01},       // y_blank_size = 430
    {0x0a, 0x00},
    {0x0b, 0x00},       // y_blank_pixel = 0
    {0x0c, 0x90},
    {0x0d, 0x01},       // frame_rate = 400 (400/16 = 25fps)

    {0x0e, 0x00},
    {0x10, 0x01},
    {0x16, 0x00},
    {0x20, 0x0e},

    {0x2d, 0x38},   //0x60},       // int_time = 50 us
    {0x2e, 0x00},
    {0x28, 0x04},   //0x03},       // cgain = 4

    {0x2f, 0x00},
    {0x30, 0x30},
    {0x31, 0x02},
    {0x33, 0x00},

    {0x01, 0x01},
};


AR_S32 gst212w4_read_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 *data)
{
    gst212w4_pipe_ctx_t *pipe_ctx = &g_gst212w4_pipe_ctx[ViPipe];
    return ar_hal_i2c_master_read(pipe_ctx->s32I2cFd, GST212W4_I2C_SLAVE_ADDR, 8, &addr, 1, data, 1);
}

AR_S32 gst212w4_write_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 data)
{
    gst212w4_pipe_ctx_t *pipe_ctx = &g_gst212w4_pipe_ctx[ViPipe];
    AR_U8 buf[2];

    buf[0] = (AR_U8)addr;
    buf[1] = (AR_U8)data;

    return ar_hal_i2c_master_write(pipe_ctx->s32I2cFd, GST212W4_I2C_SLAVE_ADDR, 8, buf, 2);
}

static AR_S32 gst212w4_init_ctx(VI_PIPE ViPipe)
{
    gst212w4_pipe_ctx_t *pipe_ctx = &g_gst212w4_pipe_ctx[ViPipe];

    pipe_ctx->s32NucFd = ar_hal_vin_open_nuc_dev();
    if (pipe_ctx->s32NucFd < 0) {
        ar_err("pipe[%d] open nuc device failed", ViPipe);
        return -1;
    }

    return 0;
}

static void gst212w4_deinit_ctx(VI_PIPE ViPipe)
{
    gst212w4_pipe_ctx_t *pipe_ctx = &g_gst212w4_pipe_ctx[ViPipe];

    if (pipe_ctx->s32NucFd >= 0) {
        ar_hal_vin_close_nuc_dev(pipe_ctx->s32NucFd);
        pipe_ctx->s32NucFd = -1;
    }
}

#if GST212W4_DUMP_REG_EN
static void gst212w4_dump_reg(VI_PIPE ViPipe)
{
    uint32_t addr = 0;
    uint32_t val = 0;
    uint32_t i = 0;
    uint32_t page[4] = {0xa8, 0xa9, 0xaa, 0xab};

    for (i = 0; i < 4; i++) {
        gst212w4_write_register(ViPipe, 0x7c, page[i]);
        ar_always("page_%u:\n", i);
        for (addr = 0x00; addr <= 0x7f; addr++) {
            gst212w4_read_register(ViPipe, addr, &val);
            ar_always("addr = 0x%02x  value = 0x%02x", addr, val);
        }
        ar_always("\n\n");
    }
}
#endif

static AR_S32 gst212w4_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

    AR_S32 reset_gpio = -1;

    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],
        p_dev_power_attr->reset_gpio[1], p_dev_power_attr->reset_gpio[2]);
    if (reset_gpio < 0) {
        ar_err("pipe[%d] get reset gpio num failed", ViPipe);
        return -1;
    }

    ret = ar_hal_gpio_export(reset_gpio);
    if (ret) {
        ar_err("pipe[%d] reset_gpio export failed", ViPipe);
        return ret;
    }

    ret = ar_hal_gpio_set_dir(reset_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] set reset_gpio output failed", ViPipe);
        return ret;
    }

    // sensor_reset pull low
    ret = ar_hal_gpio_set_value(reset_gpio, 0);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull low failed", ViPipe);
        return ret;
    }

    ar_delay(1);

    ret = ar_hal_vin_nuc_set_out_clk(g_gst212w4_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_666MHZ, 37, NUC_CLK_PHASE_0, AR_TRUE);
    if (ret) {
        ar_err("Pipe[%d] gst212w4 set nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(1);

    // sensor_reset pull high
    ret = ar_hal_gpio_set_value(reset_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull high failed", ViPipe);
        return ret;
    }

    ar_delay(10);
    ar_always("Pipe[%d] gst212w4 power on", ViPipe);
    return ret;
}

static AR_S32 gst212w4_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

    int reset_gpio = -1;

    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],
        p_dev_power_attr->reset_gpio[1], p_dev_power_attr->reset_gpio[2]);
    if (reset_gpio < 0) {
        ar_err("pipe[%d] get reset gpio num failed", ViPipe);
        return -1;
    }

    ret = ar_hal_gpio_export(reset_gpio);
    if (ret) {
        ar_err("pipe[%d] reset_gpio export failed", ViPipe);
        return ret;
    }

    ret = ar_hal_gpio_set_dir(reset_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] set reset_gpio output failed", ViPipe);
        return ret;
    }

    // sensor_reset pull low
    ret = ar_hal_gpio_set_value(reset_gpio, 0);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull low failed", ViPipe);
        return ret;
    }

    ar_delay(1);

    ret = ar_hal_vin_nuc_set_out_clk(g_gst212w4_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_666MHZ, 37, NUC_CLK_PHASE_0, AR_FALSE);
    if (ret) {
        ar_err("Pipe[%d] gst212w4 set nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_always("Pipe[%d] gst212w4 power off", ViPipe);
    return ret;
}

static AR_S32 gst212w4_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    ar_always("pipe[%d] w[%u] h[%u] fps[%f] sns_mode[%u]",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstSensorImageMode->u16Width != 256 || pstSensorImageMode->u16Height != 200
        || (pstSensorImageMode->f32Fps != 25)) {
        ar_err("pipe[%d] image_mode[w=%u h=%u fps=%f] invalid", ViPipe,
            pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, pstSensorImageMode->f32Fps);
        return AR_ERR_VI_INVALID_PARA;
    }

    g_gst212w4_pipe_ctx[ViPipe].stImageMode = *pstSensorImageMode;

    return 0;
}

void gst212w4_init(VI_PIPE ViPipe)
{
    volatile unsigned short ADC_CALIB_NMIDDLE_USER = 12000;
    int i = 0;
    int try_ready_cnt = 0;
    uint32_t val = 0;
    AR_U8 val8 = 0;

#define GST212W4_RDY_TH             120
#define GST212W4_RDY_ITVL_MS        5

    gst212w4_write_register(ViPipe, 0x7c, 0xa9);    // to page1;
    gst212w4_write_register(ViPipe, 0x1c, 0x04);    // ADC_CALIB_ENABLE_FLAG[2] = 1;
    val = (ADC_CALIB_NMIDDLE_USER & 0x00ff);
    gst212w4_write_register(ViPipe, 0x0b, (AR_U8)val);
    val = ((ADC_CALIB_NMIDDLE_USER >> 8) & 0x00ff);
    gst212w4_write_register(ViPipe, 0x0c, (AR_U8)val);
    gst212w4_write_register(ViPipe, 0x7c, 0xa8);            // to page0;

    for (i = 0; i < sizeof(g_gst212w4_setting) / sizeof(gst212w4_reg_t); i++) {
        gst212w4_write_register(ViPipe, g_gst212w4_setting[i].addr, g_gst212w4_setting[i].val);
    }
    ar_delay(10);

    gst212w4_read_register(ViPipe, 0x01, (AR_U8 *)&val);
    while ((val & 0x01) && try_ready_cnt < GST212W4_RDY_TH) {
        try_ready_cnt++;
        ar_delay(GST212W4_RDY_ITVL_MS);
        gst212w4_read_register(ViPipe, 0x01, (AR_U8 *)&val);
    }

    if (val & 0x01) {
        ar_err("pipe[%d] initialize failed", ViPipe);
        return;
    }

    ar_always("pipe[%d] initialize finished, try_cnt[%d]", ViPipe, try_ready_cnt);

    gst212w4_write_register(ViPipe, 0x01, 0x80);
    ar_delay_us(20);

    gst212w4_write_register(ViPipe, 0x7c, 0xA8);    // page0;
    gst212w4_write_register(ViPipe, 0x1f, 0x00);
    gst212w4_write_register(ViPipe, 0x35, 0x15);
    gst212w4_write_register(ViPipe, 0x7C, 0xAB);    // page3
    gst212w4_write_register(ViPipe, 0x20, 0xA5);
    gst212w4_write_register(ViPipe, 0x0B, 0x88);
    gst212w4_write_register(ViPipe, 0x10, 0x61);
    gst212w4_write_register(ViPipe, 0x15, 0x48);
    gst212w4_write_register(ViPipe, 0x1D, 0x05);
    gst212w4_write_register(ViPipe, 0x0E, 0x3e);
    gst212w4_write_register(ViPipe, 0x10, 0xe1);
    gst212w4_write_register(ViPipe, 0x7C, 0xA8);    // page0

    gst212w4_write_register(ViPipe, 0x2C, 0x05);
    gst212w4_write_register(ViPipe, 0x24, 0x20);
    gst212w4_write_register(ViPipe, 0x25, 0x20);

    //////////////////////////////////////////////////////////////////
    ///////*****           参数配置        ///////////////////////////
    //////////////////////////////////////////////////////////////////

    gst212w4_write_register(ViPipe, 0x7C, 0xAB);    // page3
    gst212w4_write_register(ViPipe, 0x1f, 0x60);    // 0x20);
    gst212w4_write_register(ViPipe, 0x17, 0x07);    // RA_sel
    gst212w4_write_register(ViPipe, 0x18, 0xff);
    gst212w4_write_register(ViPipe, 0x19, 0xff);
    gst212w4_write_register(ViPipe, 0x1a, 0xff);
    gst212w4_write_register(ViPipe, 0x1b, 0xff);
    gst212w4_write_register(ViPipe, 0x12, 0xEF);    // HSSD
    gst212w4_write_register(ViPipe, 0x05, 0x80);    // AD_STEP
    gst212w4_write_register(ViPipe, 0x1c, 0x02);
    gst212w4_write_register(ViPipe, 0x0c, 0x88);
     
    gst212w4_write_register(ViPipe, 0x14, 0x80);
    gst212w4_write_register(ViPipe, 0x0f, 0xda);
    gst212w4_write_register(ViPipe, 0x0a, 0x88);
    gst212w4_write_register(ViPipe, 0x07, 0x20);
    gst212w4_write_register(ViPipe, 0x13, 0x00);
    gst212w4_write_register(ViPipe, 0x7C, 0xA9);    // page1
    gst212w4_write_register(ViPipe, 0x3E, 0x80);
    gst212w4_write_register(ViPipe, 0x3F, 0x80);
    gst212w4_write_register(ViPipe, 0x40, 0x74);

    gst212w4_write_register(ViPipe, 0x57, 0x5a);
    gst212w4_write_register(ViPipe, 0x7C, 0xA8);    // page0

    // NUC相关寄存器初始化
    gst212w4_write_register(ViPipe, 0x3E, 0x00);    // 两次校准NUC时间间隔，0：间隔时间为0帧，即每一帧校准1次；1：间隔时间为1帧，2帧校准1次；以此类推//***********20210125**********//
#if 0
    /*******************************************************************
         片上非均匀性校准说明：帧频为25Hz时，需要校准20帧。
         如果需要将AD值校准到8192（16383减去探测器输出值，白热模式）左右，需要将
         粗调上限调到（8192 + 1200）= 9392;             0x24b0
         粗调下限调到（8192 - 1200）= 6992;             0x1b50
         细调上限调到（8192 + 500 ）= 8692;             0x21f4
         细调下限调到（8192 - 500 ）= 7692;             0x1e0c
    *///****************************************************************
    // 粗调上限:9583;
    gst212w4_write_register(ViPipe, 0x41, 0xb0);    //粗调上限低8bit
    gst212w4_write_register(ViPipe, 0x42, 0x24);    //粗调上限高6bit
    // 粗调下限:7183;
    gst212w4_write_register(ViPipe, 0x43, 0x50);    //粗调下限低8bit
    gst212w4_write_register(ViPipe, 0x44, 0x1b);    //粗调下限高6bit

    // 细调上限:8883;
    gst212w4_write_register(ViPipe, 0x45, 0xf4);    //细调上限低8bit
    gst212w4_write_register(ViPipe, 0x46, 0x21);    //细调上限高6bit
    // 细调下限:7883;
    gst212w4_write_register(ViPipe, 0x47, 0x0c);    //细调下限低8bit
    gst212w4_write_register(ViPipe, 0x48, 0x1e);    //细调下限高6bit
#else
    /*******************************************************************
         片上非均匀性校准说明：帧频为25Hz时，需要校准20帧。
         如果需要将AD值校准到8192（16383减去探测器输出值，白热模式）左右，需要将
         粗调上限调到（8192 + 1000）= 9192;             0x23e8
         粗调下限调到（8192 - 1000）= 7192;             0x1c18
         细调上限调到（8192 + 300） = 8492;             0x212c
         细调下限调到（8192 - 300） = 7892;             0x1ed8
    *///****************************************************************
    // 粗调上限:9192;
    gst212w4_write_register(ViPipe, 0x41, 0xe8);    //粗调上限低8bit
    gst212w4_write_register(ViPipe, 0x42, 0x23);    //粗调上限高6bit
    // 粗调下限:7192;
    gst212w4_write_register(ViPipe, 0x43, 0x18);    //粗调下限低8bit
    gst212w4_write_register(ViPipe, 0x44, 0x1c);    //粗调下限高6bit

    // 细调上限:8492;
    gst212w4_write_register(ViPipe, 0x45, 0x2c);    //细调上限低8bit
    gst212w4_write_register(ViPipe, 0x46, 0x21);    //细调上限高6bit
    // 细调下限:7892;
    gst212w4_write_register(ViPipe, 0x47, 0xd4);    //细调下限低8bit
    gst212w4_write_register(ViPipe, 0x48, 0x1e);    //细调下限高6bit
#endif

    gst212w4_write_register(ViPipe, 0x49, GST212W4_INIT_OCC_VAL);       //写入SRAM的初始值
    gst212w4_read_register(ViPipe, 0x40, (AR_U8 *)&val);
    val |= 0x10;
    gst212w4_write_register(ViPipe, 0x40, (AR_U8)val);

    try_ready_cnt = 0;
    gst212w4_read_register(ViPipe, 0x40, (AR_U8 *)&val);
    while ((val & 0x20) == 0 && try_ready_cnt < 500) {
        try_ready_cnt++;
        ar_delay(2);
        gst212w4_read_register(ViPipe, 0x40, (AR_U8 *)&val);
    }

    if ((val & 0x20) == 0) {
        ar_err("pipe[%d] initialize occ failed", ViPipe);
    }

    ar_always("pipe[%d] initialize occ finished, try_cnt[%d]", ViPipe, try_ready_cnt);

    gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst212w4_write_register(ViPipe, 0x1B, g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[0]);
    gst212w4_write_register(ViPipe, 0x1A, g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[1]);
    gst212w4_write_register(ViPipe, 0x19, g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[2]);
    gst212w4_write_register(ViPipe, 0x18, g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[3]);
    gst212w4_write_register(ViPipe, 0x17, g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[4]);
    gst212w4_read_register(ViPipe, 0x12, &val8);
    val8 &= 0xe0;
    val8 |= (g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[5] & 0x1f);
    gst212w4_write_register(ViPipe, 0x12, val8);
    gst212w4_write_register(ViPipe, 0x05, g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam[6]);
    gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;
}


void gst212w4_exit(VI_PIPE ViPipe)
{
    gst212w4_write_register(ViPipe, 0x01, 0x00);
}

static ISP_CMOS_DNG_COLORPARAM_S g_stGst212w4DngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 gst212w4_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = GST212W4_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &g_stGst212w4DngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));
    strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}

AR_S32 gst212w4_get_ir_sns_attr(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstIrSnsAttr == NULL) {
        ar_err("pstIrSnsAttr is null");
        return AR_ERR_VI_INVALID_PARA;
    }

    pstIrSnsAttr->u32IrParamSize = 7;
    pstIrSnsAttr->enOccType = IR_SNS_OCC_TYPE_INTERNAL;
    pstIrSnsAttr->bOccCalibBySns = AR_TRUE;     // use the occ calibration method implemented by gst212w4 lib
    pstIrSnsAttr->stSnsSize.u32Width = 256;
    pstIrSnsAttr->stSnsSize.u32Height = 200;
    pstIrSnsAttr->stRoiRect.s32X = 0;
    pstIrSnsAttr->stRoiRect.s32Y = 4;
    pstIrSnsAttr->stRoiRect.u32Width = 256;
    pstIrSnsAttr->stRoiRect.u32Height = 192;
    return 0;
}

AR_S32 gst212w4_set_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 u32ParamSize)
{
    AR_U8 *p = NULL;
    AR_U8 val = 0;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || u32ParamSize != 7) {
        ar_err("pParamData[%p] or u32ParamSize[%u] invalid",
            pParamData, u32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    p = (AR_U8 *)pParamData;

    gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst212w4_write_register(ViPipe, 0x1B, p[0]);
    gst212w4_write_register(ViPipe, 0x1A, p[1]);
    gst212w4_write_register(ViPipe, 0x19, p[2]);
    gst212w4_write_register(ViPipe, 0x18, p[3]);
    gst212w4_write_register(ViPipe, 0x17, p[4]);
    gst212w4_read_register(ViPipe, 0x12, &val);
    val &= 0xe0;
    val |= (p[5] & 0x1f);
    gst212w4_write_register(ViPipe, 0x12, val);
    gst212w4_write_register(ViPipe, 0x05, p[6]);
    gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    ar_always("pipe[%d] set sensor ir param: ra_adj[0x%02x%02x%02x%02x%02x] hssd[0x%02x] ad_step[0x%02x]",
        ViPipe, p[0], p[1], p[2], p[3], p[4], p[5], p[6]);

    memcpy(g_gst212w4_pipe_ctx[ViPipe].au8SnsIrParam, pParamData, 7);

    return 0;
}

AR_S32 gst212w4_get_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 *pu32ParamSize)
{
    AR_U8 *p = NULL;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || pu32ParamSize == NULL) {
        ar_err("pParamData[%p] or pu32ParamSize[%p] is invalid",
            pParamData, pu32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    p = (AR_U8 *)pParamData;

    gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst212w4_read_register(ViPipe, 0x1B, p);
    gst212w4_read_register(ViPipe, 0x1A, (p + 1));
    gst212w4_read_register(ViPipe, 0x19, (p + 2));
    gst212w4_read_register(ViPipe, 0x18, (p + 3));
    gst212w4_read_register(ViPipe, 0x17, (p + 4));
    gst212w4_read_register(ViPipe, 0x12, (p + 5));
    p[5] &= 0x1f;
    gst212w4_read_register(ViPipe, 0x05, (p + 6));
    gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    *pu32ParamSize = 7;

    ar_always("pipe[%d] get sensor ir param: ra_adj[0x%02x%02x%02x%02x%02x] hssd[0x%02x] ad_step[0x%02x]",
        ViPipe, p[0], p[1], p[2], p[3], p[4], p[5], p[6]);

    return 0;
}

static AR_U32 get_bit_one_cnt(AR_U64 u64Val)
{
    AR_U32 u32Cnt = 0;
    AR_U32 i = 0;

    for (i = 0; i < sizeof(AR_U64) * 8; i++) {
        if ((u64Val >> i) & 0x1) {
            u32Cnt++;
        }
    }

    return u32Cnt;
}

static AR_S32 gst212w4_start_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst212w4_pipe_ctx_t *pstPipeCtx = &g_gst212w4_pipe_ctx[ViPipe];
    AR_U8 *pu8CalibParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    AR_U64 u64InitRaAdj = 0xfffff;
    AR_U8 u8InitHssd = 0xf;
    AR_U8 u8InitAdStep = 0x80;
    AR_U8 u8Val = 0;
    int try_ready_cnt = 0;

    gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;

    // initialize hssd
    gst212w4_read_register(ViPipe, 0x12, &u8Val);
    gst212w4_write_register(ViPipe, 0x12, ((u8Val & 0xe0) | u8InitHssd));

    // initialize ra_adj
    gst212w4_write_register(ViPipe, 0x1B, (AR_U8)(u64InitRaAdj & 0xff));
    gst212w4_write_register(ViPipe, 0x1A, (AR_U8)((u64InitRaAdj >> 8) & 0xff));
    gst212w4_write_register(ViPipe, 0x19, (AR_U8)((u64InitRaAdj >> 16) & 0xff));
    gst212w4_write_register(ViPipe, 0x18, (AR_U8)((u64InitRaAdj >> 24) & 0xff));
    gst212w4_write_register(ViPipe, 0x17, (AR_U8)((u64InitRaAdj >> 32) & 0xff));

    // initialize ad_step
    gst212w4_write_register(ViPipe, 0x05, u8InitAdStep);

    gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    pu8CalibParam[0] = (AR_U8)(u64InitRaAdj & 0xff);
    pu8CalibParam[1] = (AR_U8)((u64InitRaAdj >> 8) & 0xff);
    pu8CalibParam[2] = (AR_U8)((u64InitRaAdj >> 16) & 0xff);
    pu8CalibParam[3] = (AR_U8)((u64InitRaAdj >> 24) & 0xff);
    pu8CalibParam[4] = (AR_U8)((u64InitRaAdj >> 32) & 0xff);
    pu8CalibParam[5] = u8InitHssd;
    pu8CalibParam[6] = u8InitAdStep;

    memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 7);

    // initialize occ
    gst212w4_write_register(ViPipe, 0x49, GST212W4_INIT_OCC_VAL);       // 写入OCC的初始值
    gst212w4_read_register(ViPipe, 0x40, (AR_U8 *)&u8Val);
    u8Val |= 0x10;
    gst212w4_write_register(ViPipe, 0x40, (AR_U8)u8Val);

    try_ready_cnt = 0;
    gst212w4_read_register(ViPipe, 0x40, (AR_U8 *)&u8Val);
    while ((u8Val & 0x20) == 0 && try_ready_cnt < 500) {
        try_ready_cnt++;
        ar_delay(2);
        gst212w4_read_register(ViPipe, 0x40, (AR_U8 *)&u8Val);
    }

    if ((u8Val & 0x20) == 0) {
        ar_err("pipe[%d] initialize occ failed", ViPipe);
        s32Ret = -1;
        goto FAIL;
    }

    pstPipeCtx->s32LowPos = 1;
    pstPipeCtx->s32HighPos = 39;
    pstPipeCtx->s32MidPos = get_bit_one_cnt(u64InitRaAdj);
    pstPipeCtx->u16MinMeanDelta = 0xffffUL;
    pstPipeCtx->u64FinalRaAdj = u64InitRaAdj;
    pstPipeCtx->u8FinalHssd = u8InitHssd;
    pstPipeCtx->u8FinalAdStep = u8InitAdStep;
    pstPipeCtx->enCalibState = GST212W4_CALIB_RA_ADJ;

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;

FAIL:
    pstPipeCtx->enCalibState = GST212W4_CALIB_NONE;
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 gst212w4_ra_adj_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    gst212w4_pipe_ctx_t *pstPipeCtx = &g_gst212w4_pipe_ctx[ViPipe];
    AR_U8 *pu8CalibParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U16 u16Delta = 0;
    AR_U64 u64CurRaAdj = 0;
    AR_BOOL bRaAdjCalibDone = AR_FALSE;

    u64CurRaAdj = pu8CalibParam[0];
    u64CurRaAdj |= (((AR_U64)pu8CalibParam[1]) << 8);
    u64CurRaAdj |= (((AR_U64)pu8CalibParam[2]) << 16);
    u64CurRaAdj |= (((AR_U64)pu8CalibParam[3]) << 24);
    u64CurRaAdj |= (((AR_U64)pu8CalibParam[4]) << 32);

    ar_always("pipe[%d] ra_adj calib: ra_adj[0x%02x%02x%02x%02x%02x] mean[%u]",
        ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2], pu8CalibParam[1],
        pu8CalibParam[0], pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean < pstCalibCfg->u32TargetLowerLimit) {
        u16Delta = pstCalibCfg->u32TargetLowerLimit - pstCalibCtx->stInput.u32PixMean;
        pstPipeCtx->s32LowPos = pstPipeCtx->s32MidPos;
    } else if (pstCalibCtx->stInput.u32PixMean > pstCalibCfg->u32TargetUpperLimit) {
        u16Delta = pstCalibCtx->stInput.u32PixMean - pstCalibCfg->u32TargetUpperLimit;
        pstPipeCtx->s32HighPos = pstPipeCtx->s32MidPos;
    } else {
        ar_always("pipe[%d] entire sensor calib done: final_ra_adj[0x%02x%02x%02x%02x%02x] final_hssd[0x%02x] final_ad_step[0x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2], pu8CalibParam[1],
            pu8CalibParam[0], pu8CalibParam[5], pu8CalibParam[6]);
        pstPipeCtx->enCalibState = GST212W4_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        return 0;
    }

    if (u16Delta < pstPipeCtx->u16MinMeanDelta) {
        pstPipeCtx->u16MinMeanDelta = u16Delta;
        pstPipeCtx->u64FinalRaAdj = u64CurRaAdj;
    }

    if (pstPipeCtx->s32HighPos - pstPipeCtx->s32LowPos <= 1) {
        bRaAdjCalibDone = AR_TRUE;
        u64CurRaAdj = pstPipeCtx->u64FinalRaAdj;
    } else {
        pstPipeCtx->s32MidPos = (pstPipeCtx->s32LowPos + pstPipeCtx->s32HighPos) / 2;
        u64CurRaAdj = 1;
        u64CurRaAdj <<= pstPipeCtx->s32MidPos;
        u64CurRaAdj -= 1;
    }

    pu8CalibParam[0] = (AR_U8)(u64CurRaAdj & 0xff);
    pu8CalibParam[1] = (AR_U8)((u64CurRaAdj >> 8) & 0xff);
    pu8CalibParam[2] = (AR_U8)((u64CurRaAdj >> 16) & 0xff);
    pu8CalibParam[3] = (AR_U8)((u64CurRaAdj >> 24) & 0xff);
    pu8CalibParam[4] = (AR_U8)((u64CurRaAdj >> 32) & 0xff);

    memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 7);

    gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst212w4_write_register(ViPipe, 0x1B, pu8CalibParam[0]);
    gst212w4_write_register(ViPipe, 0x1A, pu8CalibParam[1]);
    gst212w4_write_register(ViPipe, 0x19, pu8CalibParam[2]);
    gst212w4_write_register(ViPipe, 0x18, pu8CalibParam[3]);
    gst212w4_write_register(ViPipe, 0x17, pu8CalibParam[4]);
    gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    if (bRaAdjCalibDone) {
        ar_always("pipe[%d] ra_adj calib done: final_ra_adj[0x%02x%02x%02x%02x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2],
            pu8CalibParam[1],pu8CalibParam[0]);
        pstPipeCtx->s32LowPos = 0;
        pstPipeCtx->s32HighPos = 0x1f;
        pstPipeCtx->u16MinMeanDelta = 0xffffUL;
        pstPipeCtx->enCalibState = GST212W4_CALIB_HSSD;
    }

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;
}

static AR_S32 gst212w4_hssd_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    gst212w4_pipe_ctx_t *pstPipeCtx = &g_gst212w4_pipe_ctx[ViPipe];
    AR_U8 *pu8CalibParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U16 u16Delta = 0;
    AR_BOOL bHssdCalibDone = AR_FALSE;
    AR_U8 u8Val = 0;

    ar_always("pipe[%d] hssd calib: hssd[0x%02x] mean[%u]",
        ViPipe, pu8CalibParam[5], pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean < pstCalibCfg->u32TargetLowerLimit) {
        u16Delta = pstCalibCfg->u32TargetLowerLimit - pstCalibCtx->stInput.u32PixMean;
        pstPipeCtx->s32LowPos = pu8CalibParam[5];
    } else if (pstCalibCtx->stInput.u32PixMean > pstCalibCfg->u32TargetUpperLimit) {
        u16Delta = pstCalibCtx->stInput.u32PixMean - pstCalibCfg->u32TargetUpperLimit;
        pstPipeCtx->s32HighPos = pu8CalibParam[5];
    } else {
        ar_always("pipe[%d] entire sensor calib done: final_ra_adj[0x%02x%02x%02x%02x%02x] final_hssd[0x%02x] final_ad_step[0x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2], pu8CalibParam[1],
            pu8CalibParam[0], pu8CalibParam[5], pu8CalibParam[6]);
        pstPipeCtx->enCalibState = GST212W4_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        return 0;
    }

    if (u16Delta < pstPipeCtx->u16MinMeanDelta) {
        pstPipeCtx->u16MinMeanDelta = u16Delta;
        pstPipeCtx->u8FinalHssd = pu8CalibParam[5];
    }

    if (pstPipeCtx->s32HighPos - pstPipeCtx->s32LowPos <= 1) {
        bHssdCalibDone = AR_TRUE;
        pu8CalibParam[5] = pstPipeCtx->u8FinalHssd;
    } else {
        pu8CalibParam[5] = (pstPipeCtx->s32LowPos + pstPipeCtx->s32HighPos) / 2;
    }

    memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 7);

    gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst212w4_read_register(ViPipe, 0x12, &u8Val);
    gst212w4_write_register(ViPipe, 0x12, ((u8Val & 0xe0) | pu8CalibParam[5]));
    gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    if (bHssdCalibDone) {
        ar_always("pipe[%d] hssd calib done: final_hssd[0x%02x]", ViPipe, pu8CalibParam[5]);
        pstPipeCtx->s32LowPos = 5;
        pstPipeCtx->s32HighPos = 250;
        pstPipeCtx->u16MinMeanDelta = 0xffffUL;
        pstPipeCtx->enCalibState = GST212W4_CALIB_AD_STEP;
    }

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;
}

static AR_S32 gst212w4_ad_step_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    gst212w4_pipe_ctx_t *pstPipeCtx = &g_gst212w4_pipe_ctx[ViPipe];
    AR_U8 *pu8CalibParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;

    ar_always("pipe[%d] ad_step calib: ad_step[0x%02x] mean[%u]",
        ViPipe, pu8CalibParam[6], pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean < pstCalibCfg->u32TargetLowerLimit) {
        pstPipeCtx->s32HighPos = pu8CalibParam[6];
    } else if (pstCalibCtx->stInput.u32PixMean > pstCalibCfg->u32TargetUpperLimit) {
        pstPipeCtx->s32LowPos = pu8CalibParam[6];
    } else {
        ar_always("pipe[%d] entire sensor calib done: final_ra_adj[0x%02x%02x%02x%02x%02x] final_hssd[0x%02x] final_ad_step[0x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2], pu8CalibParam[1],
            pu8CalibParam[0], pu8CalibParam[5], pu8CalibParam[6]);
        pstPipeCtx->enCalibState = GST212W4_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        return 0;
    }

    if (pstPipeCtx->s32HighPos - pstPipeCtx->s32LowPos <= 1) {
        ar_err("pipe[%d] calib failed: cannot find appropriate params", ViPipe);
        pstPipeCtx->enCalibState = GST212W4_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
        return -1;
    } else {
        pu8CalibParam[6] = (pstPipeCtx->s32LowPos + pstPipeCtx->s32HighPos) / 2;

        memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 7);

        gst212w4_write_register(ViPipe, 0x7c, 0xab);      // to page3;
        gst212w4_write_register(ViPipe, 0x05, pu8CalibParam[6]);
        gst212w4_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
        return 0;
    }
}

static AR_S32 gst212w4_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst212w4_pipe_ctx_t *pstPipeCtx = &g_gst212w4_pipe_ctx[ViPipe];

    if (pstCalibCtx->stInput.stCalibFrm.u32FrameId == 0) {
        s32Ret = gst212w4_start_sns_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] start sensor calib failed", ViPipe);
        } else {
            ar_always("pipe[%d] sensor calib started", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST212W4_CALIB_RA_ADJ) {
        s32Ret = gst212w4_ra_adj_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] ra_adj calib failed", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST212W4_CALIB_HSSD) {
        s32Ret = gst212w4_hssd_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] hssd calib failed", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST212W4_CALIB_AD_STEP) {
        s32Ret = gst212w4_ad_step_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] ad_step calib failed", ViPipe);
        }
    } else {
        s32Ret = -1;
        ar_err("pipe[%d] sensor calib failed", ViPipe);
    }

    return s32Ret;
}

static AR_S32 gst212w4_ooc_calib(VI_PIPE ViPipe, IR_OCC_CALIB_CTX_S *pstCalibCtx)
{
    AR_U32 u32FrmCnt = pstCalibCtx->stInput.stCalibFrm.u32FrameId * pstCalibCtx->stInput.stCalibCfg.u32FrmInterval;
    AR_U8 u8Val = 0;

    if (u32FrmCnt == 0) {
        gst212w4_write_register(ViPipe, 0x7c, 0xa8);
        gst212w4_read_register(ViPipe, 0x40, &u8Val);
        gst212w4_write_register(ViPipe, 0x40, (u8Val | 0x03));
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    } else if (u32FrmCnt >= 20) {
        gst212w4_write_register(ViPipe, 0x7c, 0xa8);
        gst212w4_read_register(ViPipe, 0x40, &u8Val);
        gst212w4_write_register(ViPipe, 0x40, (u8Val & 0xfe));
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
    } else {
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    }

    ar_always("pipe[%d] occ calib: frm_cnt[%d] calib_status[%d]", ViPipe,
        u32FrmCnt, pstCalibCtx->stOutput.enCalibStatus);
    if (pstCalibCtx->stOutput.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
        ar_always("pipe[%d] occ calib success", ViPipe);
    }

    return 0;
}

AR_S32 gst212w4_switch_shutter(VI_PIPE ViPipe, AR_BOOL bOpen)
{
    AR_S32 s32Ret = 0;
    int shutter_gpio1 = -1;
    int shutter_gpio2 = -1;

    shutter_gpio1 = ar_hal_gpio_name_to_num(0, 0, 19);
    if (shutter_gpio1 < 0) {
        ar_err("get shutter_gpio1 num failed");
        return -1;
    }

    s32Ret = ar_hal_gpio_export(shutter_gpio1);
    if (s32Ret) {
        ar_err("shutter_gpio1 export failed");
        return s32Ret;
    }

    s32Ret = ar_hal_gpio_set_dir(shutter_gpio1, 1);
    if (s32Ret) {
        ar_err("set shutter_gpio1 output failed");
        return s32Ret;
    }

    shutter_gpio2 = ar_hal_gpio_name_to_num(0, 0, 20);
    if (shutter_gpio2 < 0) {
        ar_err("get shutter_gpio2 num failed");
        return -1;
    }

    s32Ret = ar_hal_gpio_export(shutter_gpio2);
    if (s32Ret) {
        ar_err("shutter_gpio2 export failed");
        return s32Ret;
    }

    s32Ret = ar_hal_gpio_set_dir(shutter_gpio2, 1);
    if (s32Ret) {
        ar_err("set shutter_gpio2 output failed");
        return s32Ret;
    }

    if (bOpen) {
        // shutter_gpio1 pull low
        s32Ret = ar_hal_gpio_set_value(shutter_gpio1, 0);
        if (s32Ret) {
            ar_err("shutter_gpio1 pull low failed");
            return s32Ret;
        }

        // shutter_gpio2 pull high
        s32Ret = ar_hal_gpio_set_value(shutter_gpio2, 1);
        if (s32Ret) {
            ar_err("shutter_gpio2 pull high failed");
            return s32Ret;
        }
    } else {
        // shutter_gpio1 pull high
        s32Ret = ar_hal_gpio_set_value(shutter_gpio1, 1);
        if (s32Ret) {
            ar_err("shutter_gpio1 pull low failed");
            return s32Ret;
        }

        // shutter_gpio2 pull low
        s32Ret = ar_hal_gpio_set_value(shutter_gpio2, 0);
        if (s32Ret) {
            ar_err("shutter_gpio2 pull high failed");
            return s32Ret;
        }
    }

    ar_delay(50);

    // shutter_gpio1 pull low
    s32Ret = ar_hal_gpio_set_value(shutter_gpio1, 0);
    if (s32Ret) {
        ar_err("shutter_gpio1 pull low failed");
        return s32Ret;
    }

    // shutter_gpio2 pull low
    s32Ret = ar_hal_gpio_set_value(shutter_gpio2, 0);
    if (s32Ret) {
        ar_err("shutter_gpio2 pull high failed");
        return s32Ret;
    }

    return 0;
}

static AR_S32 gst212w4_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = gst212w4_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = gst212w4_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = gst212w4_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = gst212w4_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = gst212w4_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = gst212w4_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_ir_sns_attr = gst212w4_get_ir_sns_attr;
    pstSensorExpFunc->pfn_cmos_set_ir_param = gst212w4_set_ir_param;
    pstSensorExpFunc->pfn_cmos_get_ir_param = gst212w4_get_ir_param;
    pstSensorExpFunc->pfn_cmos_get_sns_temperature = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_calib = gst212w4_sns_calib;
    pstSensorExpFunc->pfn_cmos_ir_occ_calib = gst212w4_ooc_calib;
    pstSensorExpFunc->pfn_cmos_switch_shutter = gst212w4_switch_shutter;

    return AR_SUCCESS;
}

static AR_S32 gst212w4_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 gst212w4_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 gst212w4_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    AR_S32 ret = 0;
    gst212w4_pipe_ctx_t *pipe_ctx = &g_gst212w4_pipe_ctx[ViPipe];

    pipe_ctx->s8I2cDevId = unSNSBusInfo.s8I2cDev;

    if (pipe_ctx->s32I2cFd >= 0) {
        close(pipe_ctx->s32I2cFd);
        pipe_ctx->s32I2cFd = -1;
    }

    pipe_ctx->s32I2cFd = ar_hal_i2c_open(pipe_ctx->s8I2cDevId);
    if (pipe_ctx->s32I2cFd < 0) {
        ar_err("pipe[%d] open i2c-%u failed", ViPipe, pipe_ctx->s8I2cDevId);
        ret = -1;
    }

    return ret;
}

static AR_S32 gst212w4_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = gst212w4_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ar_err("init ctx failed, pipe[%d] ret[%d]", ViPipe, s32Ret);
        return s32Ret;
    }

    stSnsAttrInfo.eSensorId = GST212W4_SNS_ID;
    s32Ret  = gst212w4_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function failed!");
        return s32Ret;
    }

    s32Ret  = gst212w4_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret  = gst212w4_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to awb lib failed!");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 gst212w4_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, GST212W4_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, GST212W4_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, GST212W4_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to awb lib failed!");
        return s32Ret;
    }

    gst212w4_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}

#if 0
static AR_S32 gst212w4_read(VI_PIPE ViPipe, AR_S32 s32Addr)
{
    AR_U8 data = 0;
    gst212w4_read_register(ViPipe, (AR_U8)s32Addr, &data);
    return (AR_S32)data;
}

static AR_S32 gst212w4_write(VI_PIPE ViPipe, AR_S32 s32Addr, AR_S32 s32Data)
{
    return gst212w4_write_register(ViPipe, (AR_U8)s32Addr, (AR_U8)s32Data);
}
#endif


ISP_SNS_OBJ_S stSnsGst212w4Obj = {
    .pfnRegisterCallback    = gst212w4_register_callback,
    .pfnUnRegisterCallback  = gst212w4_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,  //gst212w4_write,
    .pfnReadReg             = AR_NULL,  //gst212w4_read,
    .pfnSetBusInfo          = gst212w4_set_bus_info,
    .pfnSetInit             = AR_NULL,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __GST212W4_CMOS_H_ */
