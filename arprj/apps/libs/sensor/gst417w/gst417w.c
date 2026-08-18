#if !defined(__GST417W_CMOS_H_)
#define __GST417W_CMOS_H_

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
#include "gst417w.h"


#define GST417W_FPGA                0
#define GST417W_DUMP_REG_EN         0

#define GST417W_CALIB_TARGET        8192
#define GST417W_CALIB_PARAM_SIZE    6
#define GST417W_MIN_OCC_VALUE       0
#define GST417W_MAX_OCC_VALUE       0x3E


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


static gst417w_pipe_ctx_t g_gst417w_pipe_ctx[VI_MAX_PIPE_NUM] = {
    [0 ... VI_MAX_PIPE_NUM-1] = {
        .s8I2cDevId = -1,
        .s32I2cFd = -1,
        .s32NucFd = -1,
        .enCalibState = GST417W_CALIB_NONE,
        .au8SnsIrParam = {0xff, 0xff, 0xff, 0x0f, 0x00, 0x18},
    },
};


static gst417w_reg_t g_gst417w_setting_0[] = {
//   addr, val
    {0x7c, 0xa8},       // to page0;
    {0x02, 0x90},
    {0x03, 0x01},       // x_win_size = 400
    {0x04, 0x34},
    {0x05, 0x01},       // y_win_size = 308
    {0x06, 0x64},
    {0x07, 0x00},       // x_blank_size = 100
    {0x08, 0xec},
    {0x09, 0x01},       // y_blank_size = 492
    {0x0a, 0x00},
    {0x0b, 0x00},       // y_blank_pixel = 0
    {0x0c, 0x90},
    {0x0d, 0x01},       // frame_rate = 400 (400/16 = 25fps)

    {0x2d, 0x60},   //0x28},
    {0x2e, 0x00},
    {0x28, 0x05},   //0x03},

    //{0x2d, 0xa0},
    //{0x2e, 0x00},
    //{0x28, 0x05},

    {0x10, 0x01},

    {0x18, 0x3f},
    {0x19, 0x3f},
    {0x1a, 0x3f},
    {0x1b, 0x01},
    {0x1c, 0x00},

    {0x2f, 0x00},
    {0x30, 0x15},
    {0x31, 0x02},
    {0x32, 0x00},

    {0x01, 0x01},
};

static gst417w_reg_t g_gst417w_setting_1[] = {
    {0x7c, 0xa8},       // to page0;
    {0x02, 0x90},
    {0x03, 0x01},       // x_win_size = 400
    {0x04, 0x34},
    {0x05, 0x01},       // y_win_size = 300
    {0x06, 0x64},
    {0x07, 0x00},       // x_blank_size = 100
    {0x08, 0x58},
    {0x09, 0x02},       // y_blank_size = 600
    {0x0a, 0x00},
    {0x0b, 0x00},       // y_blank_pixel = 0
    {0x0c, 0x20},
    {0x0d, 0x03},       // frame_rate = 800 (800/16 = 50fps)
    {0x10, 0x01},

    {0x18, 0x3f},
    {0x19, 0x3f},
    {0x1a, 0x3f},
    {0x1b, 0x01},
    {0x1c, 0x00},

    {0x2f, 0x00},
    {0x30, 0x14},
    {0x31, 0x02},
    {0x32, 0x00},

    {0x01, 0x01},
};


AR_S32 gst417w_read_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 *data)
{
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];
    return ar_hal_i2c_master_read(pipe_ctx->s32I2cFd, GST417W_I2C_SLAVE_ADDR, 8, &addr, 1, data, 1);
}

AR_S32 gst417w_write_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 data)
{
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];
    AR_U8 buf[2];

    buf[0] = (AR_U8)addr;
    buf[1] = (AR_U8)data;

    return ar_hal_i2c_master_write(pipe_ctx->s32I2cFd, GST417W_I2C_SLAVE_ADDR, 8, buf, 2);
}

static AR_S32 gst417w_init_ctx(VI_PIPE ViPipe)
{
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];

    pipe_ctx->s32NucFd = ar_hal_vin_open_nuc_dev();
    if (pipe_ctx->s32NucFd < 0) {
        ar_err("pipe[%d] open nuc device failed", ViPipe);
        return -1;
    }

    return 0;
}

static void gst417w_deinit_ctx(VI_PIPE ViPipe)
{
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];

    if (pipe_ctx->s32NucFd >= 0) {
        ar_hal_vin_close_nuc_dev(pipe_ctx->s32NucFd);
        pipe_ctx->s32NucFd = -1;
    }
}

static void gst417w_adc_calib_auto_en(VI_PIPE ViPipe)
{
    gst417w_write_register(ViPipe, 0x7c, 0xa9);      //to page1;
    gst417w_write_register(ViPipe, 0x2f, 0x00);  //0x80);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to page0;
}

static void gst417w_adc_calib(VI_PIPE ViPipe)
{
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];
    volatile unsigned char ADC_CALIB_SIZE_USER = 19;
    //volatile unsigned char ADC_CALIB_ENABLE_FLAG;
    //volatile unsigned char flag;
    //volatile unsigned char ADC_TEST_ENABLE_MODE;
    volatile unsigned short rd_data_h = 0;
    volatile unsigned short rd_data_l = 0;
    volatile unsigned short ADC_OUT_TEMPERATURE_BUS = 0;

    gst417w_write_register(ViPipe, 0x7c, 0xa9);      //to page1;
    gst417w_write_register(ViPipe, 0x1c, 0x07);      //ADC_CALIB_ENABLE_FLAG[2:0] = 7;

    gst417w_write_register(ViPipe, 0x10, (ADC_CALIB_SIZE_USER & 0x1f));      //ADC_CALIB_SIZE_USER

    gst417w_write_register(ViPipe, 0x1D, 0x03);
    ar_delay_us(192);       //3行 64*3
    gst417w_read_register(ViPipe, 0x14, (AR_U8 *)&rd_data_l);
    gst417w_read_register(ViPipe, 0x15, (AR_U8 *)&rd_data_h);
    pipe_ctx->niossysctl.gNmiddle_first = (rd_data_h << 8) + rd_data_l;

    gst417w_write_register(ViPipe, 0x1D, 0x01);
    ar_delay_us(192);       //3行 64*3
    gst417w_read_register(ViPipe, 0x14, (AR_U8 *)&rd_data_l);
    gst417w_read_register(ViPipe, 0x15, (AR_U8 *)&rd_data_h);
    pipe_ctx->niossysctl.gNmin_first = (rd_data_h << 8) + rd_data_l;

    gst417w_write_register(ViPipe, 0x1D, 0x07);
    ar_delay_us(192);       //3行 64*3
    gst417w_read_register(ViPipe, 0x14, (AR_U8 *)&rd_data_l);
    gst417w_read_register(ViPipe, 0x15, (AR_U8 *)&rd_data_h);
    ADC_OUT_TEMPERATURE_BUS = (rd_data_h << 8) + rd_data_l;

    pipe_ctx->niossysctl.g17_Vtemp_V = ((int)ADC_OUT_TEMPERATURE_BUS - (int)pipe_ctx->niossysctl.gNmin_first)
        * 1000 / ((int)pipe_ctx->niossysctl.gNmiddle_first - (int)pipe_ctx->niossysctl.gNmin_first) + 550;
    pipe_ctx->niossysctl.Dvtemp0 = (pipe_ctx->niossysctl.g17_Vtemp_V - 1101) * 100;
    pipe_ctx->niossysctl.gTemp_first = pipe_ctx->niossysctl.Dvtemp0 / 12;

    ar_always("ADC_OUT_TEMPERATURE_BUS = %u gNmin_first = %u  gNmiddle_first = %u  g17_Vtemp_V = %d",
        ADC_OUT_TEMPERATURE_BUS, pipe_ctx->niossysctl.gNmin_first,
        pipe_ctx->niossysctl.gNmiddle_first, pipe_ctx->niossysctl.g17_Vtemp_V);

    //ADC_CALIB_NMIN_NUM_USER[7:0]
    gst417w_write_register(ViPipe, 0x08, (0x00ff & pipe_ctx->niossysctl.gNmin_first));
    //ADC_CALIB_NMIN_NUM_USER[14:8]
    gst417w_write_register(ViPipe, 0x09, (0x007f & (pipe_ctx->niossysctl.gNmin_first >> 8)));

    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to page0;
}

#if GST417W_DUMP_REG_EN
static void gst417w_dump_reg(VI_PIPE ViPipe)
{
    uint32_t addr = 0;
    uint32_t val = 0;
    uint32_t i = 0;
    uint32_t page[4] = {0xa8, 0xa9, 0xaa, 0xab};

    for (i = 0; i < 4; i++) {
        gst417w_write_register(ViPipe, 0x7c, page[i]);
        ar_always("page_%u:\n", i);
        for (addr = 0x00; addr <= 0x7f; addr++) {
            gst417w_read_register(ViPipe, addr, &val);
            ar_always("addr = 0x%02x  value = 0x%02x", addr, val);
        }
        ar_always("\n\n");
    }
}
#endif

static void gst417w_stream_on(VI_PIPE ViPipe)
{
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];
    int i = 0;
    int try_ready_cnt = 0;
    uint32_t val = 0;

#define GST417W_RDY_TH             100
#define GST417W_RDY_ITVL_MS        5

    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to reg page0;
    gst417w_write_register(ViPipe, 0x35, 0x2c);      //复位之后配置
    gst417w_write_register(ViPipe, 0x7c, 0xa9);      //to page1;

    gst417w_adc_calib_auto_en(ViPipe);
    gst417w_write_register(ViPipe, 0x7c, 0xa9);      //to page1;
    gst417w_write_register(ViPipe, 0x42, 0x72);
    gst417w_write_register(ViPipe, 0x41, 0x05);
    gst417w_write_register(ViPipe, 0x7c, 0xab);      //to page3;
    gst417w_write_register(ViPipe, 0x0e, 0x26);
    gst417w_write_register(ViPipe, 0x13, 0x10);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to page0;

    if (pipe_ctx->stImageMode.f32Fps == 25) {
        for (i = 0; i < sizeof(g_gst417w_setting_0) / sizeof(gst417w_reg_t); i++) {
            gst417w_write_register(ViPipe, g_gst417w_setting_0[i].addr, g_gst417w_setting_0[i].val);
        }
    } else if (pipe_ctx->stImageMode.f32Fps == 50) {
        for (i = 0; i < sizeof(g_gst417w_setting_1) / sizeof(gst417w_reg_t); i++) {
            gst417w_write_register(ViPipe, g_gst417w_setting_1[i].addr, g_gst417w_setting_1[i].val);
        }
    }

    gst417w_read_register(ViPipe, 0x01, (AR_U8 *)&val);
    while ((val & 0x01) && try_ready_cnt < GST417W_RDY_TH) {
        try_ready_cnt++;
        ar_delay(GST417W_RDY_ITVL_MS);
        gst417w_read_register(ViPipe, 0x01, (AR_U8 *)&val);
    }
    ar_delay_us(20);

    ar_hal_vin_nuc_start(pipe_ctx->s32NucFd);
    ar_delay(30);

    gst417w_adc_calib(ViPipe);

    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to page0;
    gst417w_write_register(ViPipe, 0x33, 0x00);
    gst417w_write_register(ViPipe, 0x2c, 0x00);
    gst417w_write_register(ViPipe, 0x0f, 0x00);
    gst417w_write_register(ViPipe, 0x7c, 0xab);      //to page3;
    gst417w_write_register(ViPipe, 0x07, 0x20);
    gst417w_write_register(ViPipe, 0x0b, 0x88);
    gst417w_write_register(ViPipe, 0x0c, 0x00);
    gst417w_write_register(ViPipe, 0x0e, 0x26);
    gst417w_write_register(ViPipe, 0x0f, 0x48);  //0x58);
    gst417w_write_register(ViPipe, 0x10, 0xa1);
    gst417w_write_register(ViPipe, 0x13, 0x00);
    gst417w_write_register(ViPipe, 0x14, 0x00);
    gst417w_write_register(ViPipe, 0x15, 0x48);
    gst417w_write_register(ViPipe, 0x1d, 0x01);
    gst417w_write_register(ViPipe, 0x1f, 0x5a);
    gst417w_write_register(ViPipe, 0x20, 0x04);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to page0;
    gst417w_write_register(ViPipe, 0x2c, 0x05);
    gst417w_write_register(ViPipe, 0x35, 0x2c);
    gst417w_write_register(ViPipe, 0x7c, 0xa9);      //to page1;
    gst417w_write_register(ViPipe, 0x21, 0x50);
    gst417w_write_register(ViPipe, 0x24, 0x50);
    gst417w_write_register(ViPipe, 0x41, 0x57);
    gst417w_write_register(ViPipe, 0x24, 0xf0);
    gst417w_write_register(ViPipe, 0x3E, 0x06);
    gst417w_write_register(ViPipe, 0x7c, 0xab);      //to page3;
    gst417w_write_register(ViPipe, 0x20, 0x0f);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      //to page0;

#if GST417W_DUMP_REG_EN
    gst417w_dump_reg(ViPipe);
#endif
}

static AR_S32 gst417w_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

#if GST417W_FPGA
    AR_S32 vin_fd = -1;

    vin_fd = ar_hal_vin_open_vin_dev();
    if (vin_fd < 0) {
        ar_err("pipe[%d] open vin device failed", ViPipe);
        return -1;
    }

    ret = ar_hal_vin_write_reg(vin_fd, 0x0b00001c, 0, 8, 8);
    if (ret) {
        ar_err("Pipe[%d] gst417w reset pull down failed", ViPipe);
        ar_hal_vin_close_vin_dev(vin_fd);
        return ret;
    }

    ar_delay(1);

    ret = ar_hal_vin_nuc_set_out_clk(g_gst417w_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_500MHZ, 25, NUC_CLK_PHASE_180, AR_TRUE);
    if (ret) {
        ar_err("Pipe[%d] gst417w set nuc_out_clk failed", ViPipe);
        ar_hal_vin_close_vin_dev(vin_fd);
        return ret;
    }

    ar_delay(1);

    ret = ar_hal_vin_write_reg(vin_fd, 0x0b00001c, 1, 8, 8);
    if (ret) {
        ar_err("Pipe[%d] gst417w reset pull up failed", ViPipe);
        ar_hal_vin_close_vin_dev(vin_fd);
        return ret;
    }

    ar_delay(4);
    ar_hal_vin_close_vin_dev(vin_fd);
#else
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

    ret = ar_hal_vin_nuc_set_out_clk(g_gst417w_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 15, NUC_CLK_PHASE_180, AR_TRUE);
    if (ret) {
        ar_err("Pipe[%d] gst417w set nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(1);

    // sensor_reset pull high
    ret = ar_hal_gpio_set_value(reset_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull high failed", ViPipe);
        return ret;
    }

    ar_delay(4);
#endif

    ar_always("Pipe[%d] gst417w power on", ViPipe);
    return ret;
}

static AR_S32 gst417w_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

#if GST417W_FPGA
    AR_S32 vin_fd = -1;

    vin_fd = ar_hal_vin_open_vin_dev();
    if (vin_fd < 0) {
        ar_err("pipe[%d] open vin device failed", ViPipe);
        return -1;
    }

    ret = ar_hal_vin_write_reg(vin_fd, 0x0b00001c, 0, 8, 8);
    if (ret) {
        ar_err("Pipe[%d] gst417w power on failed", ViPipe);
        ar_hal_vin_close_vin_dev(vin_fd);
        return ret;
    }

    ar_delay(1);

    ret = ar_hal_vin_nuc_set_out_clk(g_gst417w_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_500MHZ, 25, NUC_CLK_PHASE_270, AR_FALSE);
    if (ret) {
        ar_err("Pipe[%d] gst417w set nuc_out_clk failed", ViPipe);
        ar_hal_vin_close_vin_dev(vin_fd);
        return ret;
    }

    ar_hal_vin_close_vin_dev(vin_fd);
#else
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

    ret = ar_hal_vin_nuc_set_out_clk(g_gst417w_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 15, NUC_CLK_PHASE_270, AR_FALSE);
    if (ret) {
        ar_err("Pipe[%d] gst417w set nuc_out_clk failed", ViPipe);
        return ret;
    }
#endif

    ar_always("Pipe[%d] gst417w power off", ViPipe);
    return ret;
}

static AR_S32 gst417w_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    ar_always("pipe[%d] w[%u] h[%u] fps[%f] sns_mode[%u]",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstSensorImageMode->u16Width != 400 || pstSensorImageMode->u16Height != 308
        || (pstSensorImageMode->f32Fps != 25 && pstSensorImageMode->f32Fps != 50)) {
        ar_err("pipe[%d] image_mode[w=%u h=%u fps=%f] invalid", ViPipe,
            pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, pstSensorImageMode->f32Fps);
        return AR_ERR_VI_INVALID_PARA;
    }

    g_gst417w_pipe_ctx[ViPipe].stImageMode = *pstSensorImageMode;

    return 0;
}

void gst417w_init(VI_PIPE ViPipe)
{
    STRU_NUC_CFG_T nuc_cfg = {
        .intf_mode = NUC_INTF_MODE_GAODE_3IN,
        .active_width = 400,
        .active_height = 308,
        .total_width = 500,
        .total_height = 800,
        .occ_buf_align = NUC_OCC_BUF_ALIGN_256,
        .blank_value = 0x20,
        .fs_head = 0xffffc1,
        .fs_head_bitnum = 24,
        .fs_head_ver_loc = 800 - 308 - 1,   //0,
        .fs_head_hor_loc = 482,
        .ls_head = 0xffffc0,
        .ls_head_bitnum = 24,
        .ls_head_ver_loc_start = 800 - 308 - 1, //0,
        .ls_head_ver_loc_end = 800 - 2, //307,
    };

    ar_hal_vin_nuc_cfg(g_gst417w_pipe_ctx[ViPipe].s32NucFd, &nuc_cfg);
    gst417w_stream_on(ViPipe);

    gst417w_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst417w_write_register(ViPipe, 0x1B, g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam[0]);
    gst417w_write_register(ViPipe, 0x1A, g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam[1]);
    gst417w_write_register(ViPipe, 0x19, g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam[2]);
    gst417w_write_register(ViPipe, 0x18, g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam[3]);
    gst417w_write_register(ViPipe, 0x17, g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam[4]);
    gst417w_write_register(ViPipe, 0x12, g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam[5]);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      // to page0;
}

void gst417w_exit(VI_PIPE ViPipe)
{
    ar_hal_vin_nuc_stop(g_gst417w_pipe_ctx[ViPipe].s32NucFd);
}

static ISP_CMOS_DNG_COLORPARAM_S g_stGst417wDngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 gst417w_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = GST417W_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &g_stGst417wDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));
    strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}

AR_S32 gst417w_get_ir_sns_attr(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstIrSnsAttr == NULL) {
        ar_err("pstIrSnsAttr is null");
        return AR_ERR_VI_INVALID_PARA;
    }

    pstIrSnsAttr->u32IrParamSize = GST417W_CALIB_PARAM_SIZE;
    pstIrSnsAttr->enOccType = IR_SNS_OCC_TYPE_EXTERNAL;
    pstIrSnsAttr->enOccMonotonic = IR_SNS_OCC_MONOTONIC_DECREASE;
    pstIrSnsAttr->u32MinOccVal = GST417W_MIN_OCC_VALUE;
    pstIrSnsAttr->u32MaxOccVal = GST417W_MAX_OCC_VALUE;
    pstIrSnsAttr->s32OccVerticalOffset = 0;
    pstIrSnsAttr->bOccCalibBySns = AR_FALSE;    // use common occ calibration method implemented by sdk
    pstIrSnsAttr->stSnsSize.u32Width = 400;
    pstIrSnsAttr->stSnsSize.u32Height = 308;
    pstIrSnsAttr->stRoiRect.s32X = 0;
    pstIrSnsAttr->stRoiRect.s32Y = 4;
    pstIrSnsAttr->stRoiRect.u32Width = 400;
    pstIrSnsAttr->stRoiRect.u32Height = 300;
    return 0;
}

AR_S32 gst417w_set_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 u32ParamSize)
{
    AR_U8 *p = NULL;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || u32ParamSize != 6) {
        ar_err("pParamData[%p] or u32ParamSize[%u] invalid",
            pParamData, u32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    p = (AR_U8 *)pParamData;

    gst417w_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst417w_write_register(ViPipe, 0x1B, p[0]);
    gst417w_write_register(ViPipe, 0x1A, p[1]);
    gst417w_write_register(ViPipe, 0x19, p[2]);
    gst417w_write_register(ViPipe, 0x18, p[3]);
    gst417w_write_register(ViPipe, 0x17, p[4]);
    gst417w_write_register(ViPipe, 0x12, p[5]);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    ar_always("pipe[%d] set sensor ir param: ra_adj[0x%02x%02x%02x%02x%02x] hssd[0x%02x]",
        ViPipe, p[0], p[1], p[2], p[3], p[4], p[5]);

    memcpy(g_gst417w_pipe_ctx[ViPipe].au8SnsIrParam, pParamData, 6);

    return 0;
}

AR_S32 gst417w_get_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 *pu32ParamSize)
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

    gst417w_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst417w_read_register(ViPipe, 0x1B, p);
    gst417w_read_register(ViPipe, 0x1A, (p + 1));
    gst417w_read_register(ViPipe, 0x19, (p + 2));
    gst417w_read_register(ViPipe, 0x18, (p + 3));
    gst417w_read_register(ViPipe, 0x17, (p + 4));
    gst417w_read_register(ViPipe, 0x12, (p + 5));
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    *pu32ParamSize = GST417W_CALIB_PARAM_SIZE;

    ar_always("pipe[%d] get sensor ir param: ra_adj[0x%02x%02x%02x%02x%02x] hssd[0x%02x]",
        ViPipe, p[0], p[1], p[2], p[3], p[4], p[5]);

    return 0;
}

AR_S32 gst417w_get_sns_temperature(VI_PIPE ViPipe, AR_S32 *ps32Temperature)
{
    AR_U16 dvtemp_d5 = 0;
    AR_U16 dvtemp_d6 = 0;
    AR_U16 dvtemp = 0;
    AR_S32 g17_Vtemp_V = 0;
    AR_S32 Dvtemp0 = 0;
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];

    gst417w_write_register(ViPipe, 0x7c, 0xa9);
    ar_delay_us(100);
    gst417w_read_register(ViPipe, 0x14, (AR_U8 *)(&dvtemp_d5));
    gst417w_read_register(ViPipe, 0x15, (AR_U8 *)(&dvtemp_d6));
    dvtemp = (dvtemp_d6 << 8) + dvtemp_d5;
    gst417w_write_register(ViPipe, 0x7c, 0xa8);

    g17_Vtemp_V = ((AR_S32)dvtemp - (AR_S32)pipe_ctx->niossysctl.gNmin_first)*1000
        / ((AR_S32)pipe_ctx->niossysctl.gNmiddle_first - (AR_S32)pipe_ctx->niossysctl.gNmin_first) + 1000;
    Dvtemp0 = ((100 * g17_Vtemp_V - 100 * 1900) / 1000 + 30) * 100;

    ar_always("dvtemp_d5[0x%x] dvtemp_d6[0x%x] dvtemp[%u] gNmin_first[%u] gNmiddle_first[%u] g17_Vtemp_V[%d] Dvtemp0[%d]",
        dvtemp_d5, dvtemp_d6, dvtemp, pipe_ctx->niossysctl.gNmin_first,
        pipe_ctx->niossysctl.gNmiddle_first, g17_Vtemp_V, Dvtemp0);

    *ps32Temperature = Dvtemp0;
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

static AR_S32 gst417w_start_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst417w_pipe_ctx_t *pstPipeCtx = &g_gst417w_pipe_ctx[ViPipe];
    AR_U8 *pu8CalibParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    ISP_IR_PARAMS_S stIrParams;
    ISP_IR_OCC_PARAM_S stIrOccParam;
    AR_U64 u64InitRaAdj = 0xfffff;
    AR_U8 u8InitHssd = 0x10;
    AR_U8 u8Val = 0;

    gst417w_write_register(ViPipe, 0x7c, 0xab);      // to page3;

    // initialize hssd
    gst417w_read_register(ViPipe, 0x12, &u8Val);
    gst417w_write_register(ViPipe, 0x12, ((u8Val & 0xe0) | u8InitHssd));

    // initialize ra_adj
    gst417w_write_register(ViPipe, 0x1B, (AR_U8)(u64InitRaAdj & 0xff));
    gst417w_write_register(ViPipe, 0x1A, (AR_U8)((u64InitRaAdj >> 8) & 0xff));
    gst417w_write_register(ViPipe, 0x19, (AR_U8)((u64InitRaAdj >> 16) & 0xff));
    gst417w_write_register(ViPipe, 0x18, (AR_U8)((u64InitRaAdj >> 24) & 0xff));
    gst417w_write_register(ViPipe, 0x17, (AR_U8)((u64InitRaAdj >> 32) & 0xff));

    gst417w_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    pu8CalibParam[0] = (AR_U8)(u64InitRaAdj & 0xff);
    pu8CalibParam[1] = (AR_U8)((u64InitRaAdj >> 8) & 0xff);
    pu8CalibParam[2] = (AR_U8)((u64InitRaAdj >> 16) & 0xff);
    pu8CalibParam[3] = (AR_U8)((u64InitRaAdj >> 24) & 0xff);
    pu8CalibParam[4] = (AR_U8)((u64InitRaAdj >> 32) & 0xff);
    pu8CalibParam[5] = u8InitHssd;

    memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 6);

    stIrOccParam.bIsSingleVal = AR_TRUE;
    stIrOccParam.u32SingleVal = 0x20;

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
    stIrParams.astParamEntry[0].pParamData = &stIrOccParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(ISP_IR_OCC_PARAM_S);

    s32Ret = AR_MPI_ISP_SetIrParams(ViPipe, &stIrParams);
    if (s32Ret) {
        ar_err("pipe[%d] set ooc failed", ViPipe);
        goto FAIL;
    }

    pstPipeCtx->s32LowPos = 1;
    pstPipeCtx->s32HighPos = 39;
    pstPipeCtx->s32MidPos = get_bit_one_cnt(u64InitRaAdj);
    pstPipeCtx->u16MinMeanDelta = 0xffffUL;
    pstPipeCtx->u64FinalRaAdj = u64InitRaAdj;
    pstPipeCtx->u8FinalHssd = u8InitHssd;
    pstPipeCtx->enCalibState = GST417W_CALIB_RA_ADJ;

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;

FAIL:
    pstPipeCtx->enCalibState = GST417W_CALIB_NONE;
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 gst417w_ra_adj_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    gst417w_pipe_ctx_t *pstPipeCtx = &g_gst417w_pipe_ctx[ViPipe];
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
        ar_always("pipe[%d] entire sensor calib done: final_ra_adj[0x%02x%02x%02x%02x%02x] final_hssd[0x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2], pu8CalibParam[1],
            pu8CalibParam[0], pu8CalibParam[5]);
        pstPipeCtx->enCalibState = GST417W_CALIB_NONE;
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

    memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 6);

    gst417w_write_register(ViPipe, 0x7c, 0xab);      // to page3;
    gst417w_write_register(ViPipe, 0x1B, pu8CalibParam[0]);
    gst417w_write_register(ViPipe, 0x1A, pu8CalibParam[1]);
    gst417w_write_register(ViPipe, 0x19, pu8CalibParam[2]);
    gst417w_write_register(ViPipe, 0x18, pu8CalibParam[3]);
    gst417w_write_register(ViPipe, 0x17, pu8CalibParam[4]);
    gst417w_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

    if (bRaAdjCalibDone) {
        ar_always("pipe[%d] ra_adj calib done: final_ra_adj[0x%02x%02x%02x%02x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2],
            pu8CalibParam[1],pu8CalibParam[0]);
        pstPipeCtx->s32LowPos = 0;
        pstPipeCtx->s32HighPos = 0x1f;
        pstPipeCtx->u16MinMeanDelta = 0xffffUL;
        pstPipeCtx->enCalibState = GST417W_CALIB_HSSD;
    }

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;
}

static AR_S32 gst417w_hssd_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    gst417w_pipe_ctx_t *pstPipeCtx = &g_gst417w_pipe_ctx[ViPipe];
    AR_U8 *pu8CalibParam = (AR_U8 *)pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U8 u8Val = 0;

    ar_always("pipe[%d] hssd calib: hssd[0x%02x] mean[%u]",
        ViPipe, pu8CalibParam[5], pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean < pstCalibCfg->u32TargetLowerLimit) {
        pstPipeCtx->s32LowPos = pu8CalibParam[5];
    } else if (pstCalibCtx->stInput.u32PixMean > pstCalibCfg->u32TargetUpperLimit) {
        pstPipeCtx->s32HighPos = pu8CalibParam[5];
    } else {
        ar_always("pipe[%d] entire sensor calib done: final_ra_adj[0x%02x%02x%02x%02x%02x] final_hssd[0x%02x]",
            ViPipe, pu8CalibParam[4], pu8CalibParam[3], pu8CalibParam[2], pu8CalibParam[1],
            pu8CalibParam[0], pu8CalibParam[5]);
        pstPipeCtx->enCalibState = GST417W_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        return 0;
    }

    if (pstPipeCtx->s32HighPos - pstPipeCtx->s32LowPos <= 1) {
        ar_err("pipe[%d] calib failed: cannot find appropriate params", ViPipe);
        pstPipeCtx->enCalibState = GST417W_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
        return -1;
    } else {
        pu8CalibParam[5] = (pstPipeCtx->s32LowPos + pstPipeCtx->s32HighPos) / 2;

        memcpy(pstPipeCtx->au8SnsIrParam, pu8CalibParam, 6);

        gst417w_write_register(ViPipe, 0x7c, 0xab);      // to page3;
        gst417w_read_register(ViPipe, 0x12, &u8Val);
        gst417w_write_register(ViPipe, 0x12, ((u8Val & 0xe0) | pu8CalibParam[5]));
        gst417w_write_register(ViPipe, 0x7c, 0xa8);      // to page0;

        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
        return 0;
    }
}

static AR_S32 gst417w_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    gst417w_pipe_ctx_t *pstPipeCtx = &g_gst417w_pipe_ctx[ViPipe];

    if (pstCalibCtx->stInput.stCalibFrm.u32FrameId == 0) {
        s32Ret = gst417w_start_sns_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] start sensor calib failed", ViPipe);
        } else {
            ar_always("pipe[%d] sensor calib started", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST417W_CALIB_RA_ADJ) {
        s32Ret = gst417w_ra_adj_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] ra_adj calib failed", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == GST417W_CALIB_HSSD) {
        s32Ret = gst417w_hssd_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] hssd calib failed", ViPipe);
        }
    } else {
        s32Ret = -1;
        ar_err("pipe[%d] sensor calib failed", ViPipe);
    }

    return s32Ret;
}

AR_S32 gst417w_switch_shutter(VI_PIPE ViPipe, AR_BOOL bOpen)
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
        // shutter_gpio1 pull low
        s32Ret = ar_hal_gpio_set_value(shutter_gpio1, 1);
        if (s32Ret) {
            ar_err("shutter_gpio1 pull low failed");
            return s32Ret;
        }

        // shutter_gpio2 pull high
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

    // shutter_gpio2 pull high
    s32Ret = ar_hal_gpio_set_value(shutter_gpio2, 0);
    if (s32Ret) {
        ar_err("shutter_gpio2 pull high failed");
        return s32Ret;
    }

    return 0;
}

static AR_S32 gst417w_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = gst417w_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = gst417w_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = gst417w_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = gst417w_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = gst417w_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = gst417w_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_ir_sns_attr = gst417w_get_ir_sns_attr;
    pstSensorExpFunc->pfn_cmos_set_ir_param = gst417w_set_ir_param;
    pstSensorExpFunc->pfn_cmos_get_ir_param = gst417w_get_ir_param;
    pstSensorExpFunc->pfn_cmos_get_sns_temperature = gst417w_get_sns_temperature;
    pstSensorExpFunc->pfn_cmos_sns_calib = gst417w_sns_calib;
    pstSensorExpFunc->pfn_cmos_switch_shutter = gst417w_switch_shutter;

    return AR_SUCCESS;
}

#if 0
static AR_S32 gst417w_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    pstAeSnsDft->f32Fps = 25;
	pstAeSnsDft->f32MaxFps = 25;
    pstAeSnsDft->u32FullLinesStd = 512;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = 512;
	pstAeSnsDft->u32Hmax=500;

    return AR_SUCCESS;
}
#endif

static AR_S32 gst417w_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    //pstExpFuncs->pfn_cmos_get_ae_default = gst417w_get_ae_default;
    return AR_SUCCESS;
}

static AR_S32 gst417w_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 gst417w_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    AR_S32 ret = 0;
    gst417w_pipe_ctx_t *pipe_ctx = &g_gst417w_pipe_ctx[ViPipe];

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

static AR_S32 gst417w_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = gst417w_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ar_err("init ctx failed, pipe[%d] ret[%d]", ViPipe, s32Ret);
        return s32Ret;
    }

    stSnsAttrInfo.eSensorId = GST417W_SNS_ID;
    s32Ret  = gst417w_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function failed!");
        return s32Ret;
    }

    s32Ret  = gst417w_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret  = gst417w_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to awb lib failed!");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 gst417w_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, GST417W_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, GST417W_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, GST417W_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to awb lib failed!");
        return s32Ret;
    }

    gst417w_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}

static AR_S32 gst417w_read(VI_PIPE ViPipe, AR_S32 s32Addr)
{
    AR_U8 data = 0;
    gst417w_read_register(ViPipe, (AR_U8)s32Addr, &data);
    return (AR_S32)data;
}

static AR_S32 gst417w_write(VI_PIPE ViPipe, AR_S32 s32Addr, AR_S32 s32Data)
{
    return gst417w_write_register(ViPipe, (AR_U8)s32Addr, (AR_U8)s32Data);
}


ISP_SNS_OBJ_S stSnsGst417wObj = {
    .pfnRegisterCallback    = gst417w_register_callback,
    .pfnUnRegisterCallback  = gst417w_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = gst417w_write,
    .pfnReadReg             = gst417w_read,
    .pfnSetBusInfo          = gst417w_set_bus_info,
    .pfnSetInit             = AR_NULL,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __GST417W_CMOS_H_ */
