#if !defined(__RTD3172C_CMOS_H_)
#define __RTD3172C_CMOS_H_

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
#include "rtd3172c.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define RTD3172C_DEBUG_INFO_EN          1


#define RTD3172C_INT_TIME_START_BIT     1
#define RTD3172C_INT_TIME_BIT_NUM       7
#define RTD3172C_RST_TIME_START_BIT     8
#define RTD3172C_RST_TIME_BIT_NUM       7
#define RTD3172C_INT_CAP_START_BIT      18
#define RTD3172C_INT_CAP_BIT_NUM        3
#define RTD3172C_GOF_START_BIT          28
#define RTD3172C_GOF_BIT_NUM            6
#define RTD3172C_NTCC_START_BIT         43
#define RTD3172C_NTCC_BIT_NUM           13
#define RTD3172C_ADC_GAIN_START_BIT     119
#define RTD3172C_ADC_GAIN_BIT_NUM       7
#define RTD3172C_TEST_START_BIT         169
#define RTD3172C_TEST_BIT_NUM           4

#define RTD3172C_TEST_MEAN_TH           16380
#define RTD3172C_CALIB_TARGET           8192

#define RTD3172C_RESET_CODE             0xFFFFFFE
#define RTD3172C_RESET_CODE_BITNUM      28

#define RTD3172C_CFG_HEAD_CODE          0x3fd5
#define RTD3172C_CFG_HEAD_CODE_BITNUM   14


static RTD3172C_PIPE_CTX_S g_rtd3172c_pipe_ctx[VI_MAX_PIPE_NUM] = {
    [0 ... VI_MAX_PIPE_NUM-1] = {
        .s32NucFd = -1,
        .stNucCfg = {
            .intf_mode = NUC_INTF_MODE_IRAY_1IN,
            .active_width = 394,
            .active_height = 298,
            .total_width = 418,
            .total_height = 586,
            .occ_buf_align = NUC_OCC_BUF_ALIGN_256,
            .blank_value = 0,
            .cfg_head_ver_loc = 586 - 298 - 1,
            .cfg_head_hor_loc = 0,
            .fs_head = 0x3fd3,
            .fs_head_bitnum = 14,
            .fs_head_ver_loc = 586 - 298 - 2,
            .fs_head_hor_loc = 0,
            .ls_head = 0x3fd7,
            .ls_head_bitnum = 14,
            .ls_head_ver_loc_start = 586 - 298 - 1,
            .ls_head_ver_loc_end = 586 - 2,
            .nuc_in_clk_phase = NUC_CLK_PHASE_0,
            .ls_delay_cycles = 144,
        },
        .stCfgData = {
            .cfg_data = {
                0x78148E8C, 0x0C07EAC2, 0x2CE38B92, 0x1E8060E7,
                0x1FC43A84, 0x000010A1, 0x00000000, 0x00000000,
                0x00000000, 0x00000000, 0x00000000, 0x00000000,
                0x00000000, 0x00000000, 0x00000000, 0x00000000,
            },
            .bitnum = RTD3172C_CFG_DATA_BITS,
        },
        .enCalibState = RTD3172C_CALIB_NONE,
    },
};


static AR_S32 rtd3172c_init_ctx(VI_PIPE ViPipe)
{
    RTD3172C_PIPE_CTX_S *pipe_ctx = &g_rtd3172c_pipe_ctx[ViPipe];

    pipe_ctx->s32NucFd = ar_hal_vin_open_nuc_dev();
    if (pipe_ctx->s32NucFd < 0) {
        ar_err("pipe[%d] open nuc device failed", ViPipe);
        return -1;
    }

    return 0;
}

static void rtd3172c_deinit_ctx(VI_PIPE ViPipe)
{
    RTD3172C_PIPE_CTX_S *pipe_ctx = &g_rtd3172c_pipe_ctx[ViPipe];

    if (pipe_ctx->s32NucFd >= 0) {
        ar_hal_vin_close_nuc_dev(pipe_ctx->s32NucFd);
        pipe_ctx->s32NucFd = -1;
    }
}

static AR_S32 rtd3172c_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;
    AR_S32 power_gpio = -1;

    power_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0],
        p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    if (power_gpio < 0) {
        ar_err("pipe[%d] get power_gpio num failed", ViPipe);
        return -1;
    }

    ret = ar_hal_gpio_export(power_gpio);
    if (ret) {
        ar_err("pipe[%d] power_gpio export failed", ViPipe);
        return ret;
    }

    ret = ar_hal_gpio_set_dir(power_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] set power_gpio output failed", ViPipe);
        return ret;
    }

    // ldo_en pull high
    ret = ar_hal_gpio_set_value(power_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] power_gpio pull high failed", ViPipe);
        return ret;
    }

    ar_delay(20);

    ret = ar_hal_vin_nuc_set_out_clk(g_rtd3172c_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 7, NUC_CLK_PHASE_0, AR_TRUE);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] enable nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(10);
    ar_always("pipe[%d] power on", ViPipe);
    return ret;
}


static AR_S32 rtd3172c_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;
    AR_S32 power_gpio = -1;

    ret = ar_hal_vin_nuc_set_out_clk(g_rtd3172c_pipe_ctx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 7, NUC_CLK_PHASE_0, AR_FALSE);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] disable nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(20);

    power_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0],
        p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    if (power_gpio < 0) {
        ar_err("pipe[%d] get power_gpio num failed", ViPipe);
        return -1;
    }

    ret = ar_hal_gpio_export(power_gpio);
    if (ret) {
        ar_err("pipe[%d] power_gpio export failed", ViPipe);
        return ret;
    }

    ret = ar_hal_gpio_set_dir(power_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] set power_gpio output failed", ViPipe);
        return ret;
    }

    // ldo_en pull low
    ret = ar_hal_gpio_set_value(power_gpio, 0);
    if (ret) {
        ar_err("pipe[%d] power_gpio pull low failed", ViPipe);
        return ret;
    }

    ar_always("pipe[%d] power off", ViPipe);
    return ret;
}


static AR_S32 rtd3172c_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    ar_always("pipe[%d] w[%u] h[%u] fps[%f] sns_mode[%u]",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstSensorImageMode->u16Width != 394 || pstSensorImageMode->u16Height != 298
        || (pstSensorImageMode->f32Fps != 25 && pstSensorImageMode->f32Fps != 50)) {
        ar_err("pipe[%d] image_mode[w=%u h=%u fps=%f] invalid", ViPipe,
            pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, pstSensorImageMode->f32Fps);
        return AR_ERR_VI_INVALID_PARA;
    }

    g_rtd3172c_pipe_ctx[ViPipe].stImageMode = *pstSensorImageMode;

    return 0;
}

static void rtd3172c_set_smart_code(uint32_t *smart_code,
    uint32_t start_bit, uint32_t bit_num, uint32_t *value)
{
    uint32_t i = 0;
    uint32_t src_word = 0;
    uint32_t bit_of_src_word = 0;
    uint32_t dst_word = 0;
    uint32_t bit_of_dst_word = 0;

    for (i = 0; i < bit_num; i++) {
        src_word = i / 32;
        bit_of_src_word = i % 32;
        dst_word = start_bit / 32;
        bit_of_dst_word = start_bit % 32;

        if ((value[src_word] >> bit_of_src_word) & 0x1) {
            smart_code[dst_word] |= (0x1 << bit_of_dst_word);
        } else {
            smart_code[dst_word] &= ~(0x1 << bit_of_dst_word);
        }

        start_bit++;
    }
}

static void rtd3172c_get_smart_code(uint32_t *smart_code,
    uint32_t start_bit, uint32_t bit_num, uint32_t *value)
{
    uint32_t word_num = (bit_num + 31) / 32;
    uint32_t src_word = 0;
    uint32_t bit_of_src_word = 0;
    uint32_t dst_word = 0;
    uint32_t bit_of_dst_word = 0;
    uint32_t i = 0;

    for (i = 0; i < word_num; i++)
        value[i] = 0;

    for (i = 0; i < bit_num; i++) {
        src_word = start_bit / 32;
        bit_of_src_word = start_bit % 32;
        dst_word = i / 32;
        bit_of_dst_word = i % 32;
        value[dst_word] |= (((smart_code[src_word] >> bit_of_src_word) & 0x1) << bit_of_dst_word);
        start_bit++;
    }
}

static void rtd3172c_set_int_time(uint32_t *smart_code, uint32_t int_time)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_INT_TIME_START_BIT,
        RTD3172C_INT_TIME_BIT_NUM, &int_time);
}

static uint32_t rtd3172c_get_int_time(uint32_t *smart_code)
{
    uint32_t int_time;
    rtd3172c_get_smart_code(smart_code, RTD3172C_INT_TIME_START_BIT,
        RTD3172C_INT_TIME_BIT_NUM, &int_time);
    return int_time;
}

static void rtd3172c_set_rst_time(uint32_t *smart_code, uint32_t rst_time)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_RST_TIME_START_BIT,
        RTD3172C_RST_TIME_BIT_NUM, &rst_time);
}

static uint32_t rtd3172c_get_rst_time(uint32_t *smart_code)
{
    uint32_t rst_time;
    rtd3172c_get_smart_code(smart_code, RTD3172C_RST_TIME_START_BIT,
        RTD3172C_RST_TIME_BIT_NUM, &rst_time);
    return rst_time;
}

static void rtd3172c_set_int_cap(uint32_t *smart_code, uint32_t int_cap)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_INT_CAP_START_BIT,
        RTD3172C_INT_CAP_BIT_NUM, &int_cap);
}

static uint32_t rtd3172c_get_int_cap(uint32_t *smart_code)
{
    uint32_t int_cap;
    rtd3172c_get_smart_code(smart_code, RTD3172C_INT_CAP_START_BIT,
        RTD3172C_INT_CAP_BIT_NUM, &int_cap);
    return int_cap;
}

static void rtd3172c_set_gof(uint32_t *smart_code, uint32_t gof)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_GOF_START_BIT, RTD3172C_GOF_BIT_NUM, &gof);
}

static uint32_t rtd3172c_get_gof(uint32_t *smart_code)
{
    uint32_t gof;
    rtd3172c_get_smart_code(smart_code, RTD3172C_GOF_START_BIT, RTD3172C_GOF_BIT_NUM, &gof);
    return gof;
}

static void rtd3172c_set_ntcc(uint32_t *smart_code, uint32_t ntcc)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_NTCC_START_BIT, RTD3172C_NTCC_BIT_NUM, &ntcc);
}

static uint32_t rtd3172c_get_ntcc(uint32_t *smart_code)
{
    uint32_t ntcc;
    rtd3172c_get_smart_code(smart_code, RTD3172C_NTCC_START_BIT, RTD3172C_NTCC_BIT_NUM, &ntcc);
    return ntcc;
}

static void rtd3172c_set_adc_gain(uint32_t *smart_code, uint32_t adc_gain)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_ADC_GAIN_START_BIT,
        RTD3172C_ADC_GAIN_BIT_NUM, &adc_gain);
}

static uint32_t rtd3172c_get_adc_gain(uint32_t *smart_code)
{
    uint32_t adc_gain;
    rtd3172c_get_smart_code(smart_code, RTD3172C_ADC_GAIN_START_BIT,
        RTD3172C_ADC_GAIN_BIT_NUM, &adc_gain);
    return adc_gain;
}

static void rtd3172c_set_test(uint32_t *smart_code, uint32_t test)
{
    rtd3172c_set_smart_code(smart_code, RTD3172C_TEST_START_BIT, RTD3172C_TEST_BIT_NUM, &test);
}

static uint32_t rtd3172c_get_test(uint32_t *smart_code)
{
    uint32_t test;
    rtd3172c_get_smart_code(smart_code, RTD3172C_TEST_START_BIT, RTD3172C_TEST_BIT_NUM, &test);
    return test;
}

#if RTD3172C_DEBUG_INFO_EN
static void rtd3172c_dump_cfg_info(RTD3172C_PIPE_CTX_S *pipe_ctx)
{
    ar_always("int_time = 0x%x", rtd3172c_get_int_time(pipe_ctx->stCfgData.cfg_data));
    ar_always("rst_time = 0x%x", rtd3172c_get_rst_time(pipe_ctx->stCfgData.cfg_data));
    ar_always("int_cap = 0x%x", rtd3172c_get_int_cap(pipe_ctx->stCfgData.cfg_data));
    ar_always("gof = 0x%x", rtd3172c_get_gof(pipe_ctx->stCfgData.cfg_data));
    ar_always("ntcc = 0x%x", rtd3172c_get_ntcc(pipe_ctx->stCfgData.cfg_data));
    ar_always("adc_gain = 0x%x", rtd3172c_get_adc_gain(pipe_ctx->stCfgData.cfg_data));
    ar_always("test = 0x%x", rtd3172c_get_test(pipe_ctx->stCfgData.cfg_data));
}

static void rtd3172c_print_smart_code(RTD3172C_PIPE_CTX_S *pipe_ctx)
{
    uint32_t word_num = (pipe_ctx->stCfgData.bitnum + 31) / 32;
    int i = 0;

    ar_always("smart_code: bit_num[%u]", pipe_ctx->stCfgData.bitnum);
    for (i = 0; i < word_num; i++)
        ar_always("smart_code: 0x%08x", pipe_ctx->stCfgData.cfg_data[i]);
}
#endif

void rtd3172c_init(VI_PIPE ViPipe)
{
    int ret = 0;
    RTD3172C_PIPE_CTX_S *pipe_ctx = &g_rtd3172c_pipe_ctx[ViPipe];
    uint32_t cfg_head = RTD3172C_CFG_HEAD_CODE;
    STRU_NUC_CMD_RESET_CODE_T reset_code;
    STRU_NUC_CMD_CFG_DATA_T cfg_data;
    STRU_NUC_CMD_IN_CLK_CALIB_T clk_calib;

    if (pipe_ctx->stCfgData.bitnum != RTD3172C_CFG_DATA_BITS + RTD3172C_CFG_HEAD_CODE_BITNUM) {
        rtd3172c_set_smart_code(pipe_ctx->stCfgData.cfg_data, RTD3172C_CFG_DATA_BITS,
            RTD3172C_CFG_HEAD_CODE_BITNUM, &cfg_head);
        pipe_ctx->stCfgData.bitnum = RTD3172C_CFG_DATA_BITS + RTD3172C_CFG_HEAD_CODE_BITNUM;
    }

#if RTD3172C_DEBUG_INFO_EN
    rtd3172c_print_smart_code(pipe_ctx);
    rtd3172c_dump_cfg_info(pipe_ctx);
#endif

    ret = ar_hal_vin_nuc_cfg(pipe_ctx->s32NucFd, &pipe_ctx->stNucCfg);
    if (ret) {
        ar_err("pipe[%d] nuc cfg failed", ViPipe);
        return;
    }

    reset_code.reset_code = RTD3172C_RESET_CODE;
    reset_code.bitnum = RTD3172C_RESET_CODE_BITNUM;
    ret = ar_hal_vin_nuc_cmd(pipe_ctx->s32NucFd, NUC_CMD_RESET, &reset_code, sizeof(reset_code));
    if (ret) {
        ar_err("pipe[%d] reset failed", ViPipe);
        return;
    }

    cfg_data.cfg_data[0] = pipe_ctx->stNucCfg.ls_head;
    cfg_data.bitnum = pipe_ctx->stNucCfg.ls_head_bitnum;
    ret = ar_hal_vin_nuc_cmd(pipe_ctx->s32NucFd, NUC_CMD_CFG_SW_MODE, &cfg_data, sizeof(cfg_data));
    if (ret) {
        ar_err("pipe[%d] send ls_head failed", ViPipe);
        return;
    }

    ret = ar_hal_vin_nuc_cmd(pipe_ctx->s32NucFd, NUC_CMD_IN_CLK_CALIB, &clk_calib, sizeof(clk_calib));
    if (ret) {
        ar_err("pipe[%d] nuc calibration failed", ViPipe);
        return;
    }

    ar_always("pipe[%d] nuc input clock calibrate successfully, clk_phase[%d] ls_delay_cycles[%u]",
        ViPipe, clk_calib.nuc_in_clk_phase, clk_calib.ls_delay_cycles);

    ret = ar_hal_vin_nuc_cmd(pipe_ctx->s32NucFd, NUC_CMD_CFG_SW_MODE,
        &pipe_ctx->stCfgData, sizeof(pipe_ctx->stCfgData));
    if (ret) {
        ar_err("pipe[%d] nuc set cfg data failed", ViPipe);
        return;
    }

    ret = ar_hal_vin_nuc_start(pipe_ctx->s32NucFd);
    if (ret) {
        ar_err("pipe[%d] nuc start failed", ViPipe);
    }
}


void rtd3172c_exit(VI_PIPE ViPipe)
{
    ar_hal_vin_nuc_stop(g_rtd3172c_pipe_ctx[ViPipe].s32NucFd);
}

static ISP_CMOS_DNG_COLORPARAM_S g_stRtd3172cDngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 rtd3172c_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = RTD3172C_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &g_stRtd3172cDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));
    strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}

AR_S32 rtd3172c_get_ir_sns_attr(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstIrSnsAttr == NULL) {
        ar_err("pstIrSnsAttr is null");
        return AR_ERR_VI_INVALID_PARA;
    }

    pstIrSnsAttr->u32IrParamSize = RTD3172C_CFG_DATA_BYTES;
    pstIrSnsAttr->enOccType = IR_SNS_OCC_TYPE_EXTERNAL;
    pstIrSnsAttr->enOccMonotonic = IR_SNS_OCC_MONOTONIC_DECREASE;
    pstIrSnsAttr->u32MinOccVal = 0;
    pstIrSnsAttr->u32MaxOccVal = 0x3E;
    pstIrSnsAttr->s32OccVerticalOffset = 3;
    pstIrSnsAttr->bOccCalibBySns = AR_FALSE;    // use common occ calibration method implemented by sdk
    pstIrSnsAttr->stSnsSize.u32Width = 394;
    pstIrSnsAttr->stSnsSize.u32Height = 298;
    pstIrSnsAttr->stRoiRect.s32X = 4;
    pstIrSnsAttr->stRoiRect.s32Y = 8;
    pstIrSnsAttr->stRoiRect.u32Width = 384;
    pstIrSnsAttr->stRoiRect.u32Height = 288;
    return 0;
}

AR_S32 rtd3172c_set_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 u32ParamSize)
{
    AR_S32 s32Ret = 0;
    STRU_NUC_CMD_CFG_DATA_T stCfgData = {
        .cfg_data = {0},
        .bitnum = RTD3172C_CFG_DATA_BITS + RTD3172C_CFG_HEAD_CODE_BITNUM,
    };
    uint32_t cfg_head = RTD3172C_CFG_HEAD_CODE;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || u32ParamSize != RTD3172C_CFG_DATA_BYTES) {
        ar_err("pParamData[%p] or u32ParamSize[%u] invalid", pParamData, u32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    memcpy(stCfgData.cfg_data, pParamData, u32ParamSize);
    rtd3172c_set_smart_code(stCfgData.cfg_data, RTD3172C_CFG_DATA_BITS,
        RTD3172C_CFG_HEAD_CODE_BITNUM, &cfg_head);

    s32Ret = ar_hal_vin_nuc_cmd(g_rtd3172c_pipe_ctx[ViPipe].s32NucFd,
        NUC_CMD_CFG_HW_MODE, &stCfgData, sizeof(stCfgData));
    if (s32Ret) {
        ar_err("pipe[%d] nuc set cfg data failed\n", ViPipe);
        return -1;
    }

    g_rtd3172c_pipe_ctx[ViPipe].stCfgData = stCfgData;

#if RTD3172C_DEBUG_INFO_EN
    rtd3172c_print_smart_code(&g_rtd3172c_pipe_ctx[ViPipe]);
    rtd3172c_dump_cfg_info(&g_rtd3172c_pipe_ctx[ViPipe]);
#endif

    return 0;
}

AR_S32 rtd3172c_get_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 *pu32ParamSize)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || pu32ParamSize == NULL) {
        ar_err("pParamData[%p] or pu32ParamSize[%p] is invalid",
            pParamData, pu32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    memcpy(pParamData, g_rtd3172c_pipe_ctx[ViPipe].stCfgData.cfg_data, RTD3172C_CFG_DATA_BYTES);
    *pu32ParamSize = RTD3172C_CFG_DATA_BYTES;

    return 0;
}

static AR_S32 rtd3172c_start_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    RTD3172C_PIPE_CTX_S *pstPipeCtx = &g_rtd3172c_pipe_ctx[ViPipe];
    AR_VOID *pCalibParam = pstCalibCtx->stOutput.pCalibParam;
    ISP_IR_PARAMS_S stIrParams;
    ISP_IR_OCC_PARAM_S stIrOccParam;

    rtd3172c_set_test(pstPipeCtx->stCfgData.cfg_data, 0x5);
    rtd3172c_set_adc_gain(pstPipeCtx->stCfgData.cfg_data, 0x60);
    rtd3172c_set_ntcc(pstPipeCtx->stCfgData.cfg_data, 0x1000);

    s32Ret = ar_hal_vin_nuc_cmd(pstPipeCtx->s32NucFd, NUC_CMD_CFG_HW_MODE,
        &pstPipeCtx->stCfgData, sizeof(pstPipeCtx->stCfgData));
    if (s32Ret) {
        ar_err("pipe[%d] nuc set cfg data failed", ViPipe);
        goto FAIL;
    }

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

    memcpy(pCalibParam, pstPipeCtx->stCfgData.cfg_data, RTD3172C_CFG_DATA_BYTES);

    pstPipeCtx->enCalibState = RTD3172C_CALIB_ADC_GAIN;
    pstPipeCtx->s32AdcGainCalibFrmIdx = -1;
    pstPipeCtx->u32FinalAdcGain = rtd3172c_get_adc_gain(pstPipeCtx->stCfgData.cfg_data);
    pstPipeCtx->u32FinalNtcc = rtd3172c_get_ntcc(pstPipeCtx->stCfgData.cfg_data);

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;

FAIL:
    pstPipeCtx->enCalibState = RTD3172C_CALIB_NONE;
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 rtd3172c_adc_gain_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    RTD3172C_PIPE_CTX_S *pstPipeCtx = &g_rtd3172c_pipe_ctx[ViPipe];
    AR_VOID *pCalibParam = pstCalibCtx->stOutput.pCalibParam;
    AR_U16 *pu16Raw = NULL;
    AR_U32 u32LinePixs = pstCalibCtx->stInput.stCalibFrm.u32Stride[0] / sizeof(AR_U16);
    AR_U32 u32PixSum = 0;
    AR_U16 u16PixMean = 0;
    AR_U32 u32CurAdcGain = 0;
    AR_BOOL bAdcGainCalibDone = AR_FALSE;
    AR_U32 i = 0;

    static AR_U32 u32LastAdcGain = 0;
    static AR_U32 u32LastPixMean = 0;

    pstPipeCtx->s32AdcGainCalibFrmIdx++;

    u32CurAdcGain = rtd3172c_get_adc_gain(pstPipeCtx->stCfgData.cfg_data);

    pu16Raw = (AR_U16 *)pstCalibCtx->stInput.stCalibFrm.u64VirAddr[0];
    pu16Raw += (u32LinePixs * 8 + 393);
    for (i = 0; i < 288; i++) {
        u32PixSum += *pu16Raw;
        pu16Raw += u32LinePixs;
    }
    u16PixMean = (AR_U16)(u32PixSum / 288);

    ar_always("pipe[%d] calib: adc_gain[0x%02x] test_column_mean[%u]",
        ViPipe, u32CurAdcGain, u16PixMean);

    if (pstPipeCtx->s32AdcGainCalibFrmIdx > 0) {
        if (u32LastPixMean < RTD3172C_TEST_MEAN_TH && u16PixMean >= RTD3172C_TEST_MEAN_TH) {
            ar_always("pipe[%d] adc_gain calib done: final_adc_gain[0x%02x]", ViPipe, u32LastAdcGain);
            u32CurAdcGain = u32LastAdcGain;
            bAdcGainCalibDone = AR_TRUE;
        } else if (u32LastPixMean >= RTD3172C_TEST_MEAN_TH && u16PixMean < RTD3172C_TEST_MEAN_TH) {
            ar_always("pipe[%d] adc_gain calib done: final_adc_gain[0x%02x]", ViPipe, u32CurAdcGain);
            bAdcGainCalibDone = AR_TRUE;
        }
    }

    if (!bAdcGainCalibDone) {
        if (u16PixMean < RTD3172C_TEST_MEAN_TH && u32CurAdcGain == 0x7f) {
            ar_always("adc_gain is max now");
            ar_always("pipe[%d] adc_gain calib done: final_adc_gain[0x%02x]", ViPipe, u32CurAdcGain);
            bAdcGainCalibDone = AR_TRUE;
        } else if (u16PixMean >= RTD3172C_TEST_MEAN_TH && u32CurAdcGain == 0) {
            ar_always("adc_gain is min now");
            ar_always("pipe[%d] adc_gain calib done: final_adc_gain[0x%02x]", ViPipe, u32CurAdcGain);
            bAdcGainCalibDone = AR_TRUE;
        }
    }

    if (!bAdcGainCalibDone) {
        u32LastAdcGain = u32CurAdcGain;
        u32LastPixMean = u16PixMean;

        if (u16PixMean < RTD3172C_TEST_MEAN_TH)
            u32CurAdcGain++;
        else
            u32CurAdcGain--;

        rtd3172c_set_adc_gain(pstPipeCtx->stCfgData.cfg_data, u32CurAdcGain);
    } else {
        pstPipeCtx->u32FinalAdcGain = u32CurAdcGain;
        rtd3172c_set_adc_gain(pstPipeCtx->stCfgData.cfg_data, u32CurAdcGain);

        pstPipeCtx->s32LowPos = 0;
        pstPipeCtx->s32HighPos = 0x1fff;
        pstPipeCtx->enCalibState = RTD3172C_CALIB_DC;
    }

    s32Ret = ar_hal_vin_nuc_cmd(pstPipeCtx->s32NucFd, NUC_CMD_CFG_HW_MODE,
        &pstPipeCtx->stCfgData, sizeof(pstPipeCtx->stCfgData));
    if (s32Ret) {
        ar_err("pipe[%d] nuc set cfg data failed", ViPipe);
        goto FAIL;
    }

    memcpy(pCalibParam, pstPipeCtx->stCfgData.cfg_data, RTD3172C_CFG_DATA_BYTES);

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;

FAIL:
    pstPipeCtx->enCalibState = RTD3172C_CALIB_NONE;
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 rtd3172c_dc_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    RTD3172C_PIPE_CTX_S *pstPipeCtx = &g_rtd3172c_pipe_ctx[ViPipe];
    AR_VOID *pCalibParam = pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U32 u32CurAdcGain = 0;
    AR_U32 u32CurNtcc = 0;

    u32CurAdcGain = rtd3172c_get_adc_gain(pstPipeCtx->stCfgData.cfg_data);
    u32CurNtcc = rtd3172c_get_ntcc(pstPipeCtx->stCfgData.cfg_data);

    ar_always("pipe[%d] dc calib: ntcc[0x%04x] mean[%u]",
        ViPipe, u32CurNtcc, pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean < pstCalibCfg->u32TargetLowerLimit) {
        pstPipeCtx->s32HighPos = u32CurNtcc;
    } else if (pstCalibCtx->stInput.u32PixMean > pstCalibCfg->u32TargetUpperLimit) {
        pstPipeCtx->s32LowPos = u32CurNtcc;
    } else {
        ar_always("pipe[%d] entire sensor calib done: final_adc_gain[0x%02x] final_ntcc[0x%04x]",
            ViPipe, u32CurAdcGain, u32CurNtcc);
        pstPipeCtx->enCalibState = RTD3172C_CALIB_NONE;
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        return 0;
    }

    if (pstPipeCtx->s32HighPos - pstPipeCtx->s32LowPos <= 1) {
        ar_err("pipe[%d] calib failed: cannot find appropriate params", ViPipe);
        s32Ret = -1;
        goto FAIL;
    } else {
        u32CurNtcc = (pstPipeCtx->s32LowPos + pstPipeCtx->s32HighPos) / 2;
        rtd3172c_set_ntcc(pstPipeCtx->stCfgData.cfg_data, u32CurNtcc);

        s32Ret = ar_hal_vin_nuc_cmd(pstPipeCtx->s32NucFd, NUC_CMD_CFG_HW_MODE,
            &pstPipeCtx->stCfgData, sizeof(pstPipeCtx->stCfgData));
        if (s32Ret) {
            ar_err("pipe[%d] nuc set cfg data failed", ViPipe);
            s32Ret = -1;
            goto FAIL;
        }

        memcpy(pCalibParam, pstPipeCtx->stCfgData.cfg_data, RTD3172C_CFG_DATA_BYTES);

        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
        return 0;
    }

FAIL:
    pstPipeCtx->enCalibState = RTD3172C_CALIB_NONE;
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 rtd3172c_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    RTD3172C_PIPE_CTX_S *pstPipeCtx = &g_rtd3172c_pipe_ctx[ViPipe];

    if (pstCalibCtx->stInput.stCalibFrm.u32FrameId == 0) {
        s32Ret = rtd3172c_start_sns_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] start sensor calib failed", ViPipe);
        } else {
            ar_always("pipe[%d] sensor calib started", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == RTD3172C_CALIB_ADC_GAIN) {
        s32Ret = rtd3172c_adc_gain_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] adc_gain calib failed", ViPipe);
        }
    } else if (pstPipeCtx->enCalibState == RTD3172C_CALIB_DC) {
        s32Ret = rtd3172c_dc_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] dc calib failed", ViPipe);
        }
    } else {
        s32Ret = -1;
        ar_err("pipe[%d] sensor calib failed", ViPipe);
    }

    return s32Ret;
}

#if 0
static AR_S32 rtd3172c_ooc_calib(VI_PIPE ViPipe, RTD3172C_CALIB_INFO_S *pCalibInfo)
{
    AR_S32 s32Ret = 0;
    RTD3172C_PIPE_CTX_S *pstPipeCtx = &g_rtd3172c_pipe_ctx[ViPipe];
    AR_U16 *pu16Raw = NULL;
    AR_U32 u32PixStride = pCalibInfo->pstFrmInfo->stVFrame.u32Stride[0] / sizeof(AR_U16);
    AR_U8 *pu8FinalOoc = NULL;
    AR_U16 *pu16MinPixDelta = NULL;
    AR_U32 u32OocStride = pCalibInfo->pstIrOccParam->stOccBuf.u32Stride[0];
    AR_U16 u16PixDelta = 0;
    AR_U8 u8CurOocVal = 0;
    AR_BOOL bCalibDone = AR_FALSE;
    ISP_IR_PARAMS_S stIrParams;
    AR_U32 i = 0;
    AR_U32 j = 0;

    pstPipeCtx->s32OccCalibFrmIdx++;
    u8CurOocVal = (AR_U8)pstPipeCtx->s32OccCalibFrmIdx;

    pu16Raw = (AR_U16 *)pCalibInfo->pstFrmInfo->stVFrame.u64VirAddr[0];
    pu16Raw += (u32PixStride * pCalibInfo->stROI.s32Y + pCalibInfo->stROI.s32X);
    pu8FinalOoc = pstPipeCtx->pu8FinalOcc + u32OocStride * (pCalibInfo->stROI.s32Y - 3) + pCalibInfo->stROI.s32X;
    pu16MinPixDelta = pstPipeCtx->pu16MinPixDelta;

    for (i = 0; i < pCalibInfo->stROI.u32Height; i++) {
        for (j = 0; j < pCalibInfo->stROI.u32Width; j++) {
            if (pu16Raw[j] <= RTD3172C_CALIB_TARGET) {
                u16PixDelta = RTD3172C_CALIB_TARGET - pu16Raw[j];
            } else {
                u16PixDelta = pu16Raw[j] - RTD3172C_CALIB_TARGET;
            }

            if (u16PixDelta < pu16MinPixDelta[j]) {
                pu16MinPixDelta[j] = u16PixDelta;
                pu8FinalOoc[j] = u8CurOocVal;
            }
        }

        pu16Raw += u32PixStride;
        pu8FinalOoc += u32OocStride;
        pu16MinPixDelta += pCalibInfo->stROI.u32Width;
    }

    printf("ooc calib: frm[%d]\n", pstPipeCtx->s32OccCalibFrmIdx);

    if (u8CurOocVal < 0x3E) {
        memset((AR_VOID *)pCalibInfo->pstIrOccParam->stOccBuf.u64VirAddr[0],
            u8CurOocVal + 1, u32OocStride * pCalibInfo->pstIrOccParam->stOccBuf.u32Height);
    } else {
        bCalibDone = AR_TRUE;
        memcpy((AR_VOID *)pCalibInfo->pstIrOccParam->stOccBuf.u64VirAddr[0],
            pstPipeCtx->pu8FinalOcc, u32OocStride * pCalibInfo->pstIrOccParam->stOccBuf.u32Height);
    }

    s32Ret = AR_MPI_SYS_MmzFlushCache(pCalibInfo->pstIrOccParam->stOccBuf.u64PhyAddr[0],
        (AR_VOID *)pCalibInfo->pstIrOccParam->stOccBuf.u64VirAddr[0],
        u32OocStride * pCalibInfo->pstIrOccParam->stOccBuf.u32Height);
    if (s32Ret) {
        ar_err("flush occ buf failed");
        goto FAIL;
    }

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
    stIrParams.astParamEntry[0].pParamData = pCalibInfo->pstIrOccParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(ISP_IR_OCC_PARAM_S);
    s32Ret = AR_MPI_ISP_SetIrParams(ViPipe, &stIrParams);
    if (s32Ret) {
        ar_err("pipe[%d] set ooc failed", ViPipe);
        goto FAIL;
    }

    if (!bCalibDone) {
        pCalibInfo->enCalibResult = RTD3172C_CALIB_ONGOING;
    } else {
        pstPipeCtx->enCalibState = RTD3172C_CALIB_NONE;
        pCalibInfo->enCalibResult = RTD3172C_CALIB_DONE;
        printf("sensor calibration done");
    }

    return 0;

FAIL:
    pstPipeCtx->enCalibState = RTD3172C_CALIB_NONE;
    pCalibInfo->enCalibResult = RTD3172C_CALIB_FAIL;
    return s32Ret;
}
#endif

static AR_S32 rtd3172c_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = rtd3172c_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = rtd3172c_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = rtd3172c_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = rtd3172c_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = rtd3172c_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = rtd3172c_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_ir_sns_attr = rtd3172c_get_ir_sns_attr;
    pstSensorExpFunc->pfn_cmos_set_ir_param = rtd3172c_set_ir_param;
    pstSensorExpFunc->pfn_cmos_get_ir_param = rtd3172c_get_ir_param;
    pstSensorExpFunc->pfn_cmos_get_sns_temperature = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_calib = rtd3172c_sns_calib;
    pstSensorExpFunc->pfn_cmos_switch_shutter = AR_NULL;

    return AR_SUCCESS;
}

#if 0
static AR_S32 rtd3172c_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
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

static AR_S32 rtd3172c_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    //pstExpFuncs->pfn_cmos_get_ae_default = rtd3172c_get_ae_default;
    return AR_SUCCESS;
}

static AR_S32 rtd3172c_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 rtd3172c_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    return 0;
}

static AR_S32 rtd3172c_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = rtd3172c_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ar_err("init ctx failed, pipe[%d] ret[%d]", ViPipe, s32Ret);
        return s32Ret;
    }

    stSnsAttrInfo.eSensorId = RTD3172C_SNS_ID;
    s32Ret  = rtd3172c_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function failed!");
        return s32Ret;
    }

    s32Ret  = rtd3172c_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret  = rtd3172c_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor register callback function to awb lib failed!");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 rtd3172c_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, RTD3172C_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, RTD3172C_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to ae lib failed!");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, RTD3172C_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ar_err("sensor unregister callback function to awb lib failed!");
        return s32Ret;
    }

    rtd3172c_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}


ISP_SNS_OBJ_S stSnsRtd3172cObj = {
    .pfnRegisterCallback    = rtd3172c_register_callback,
    .pfnUnRegisterCallback  = rtd3172c_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = rtd3172c_set_bus_info,
    .pfnSetInit             = AR_NULL,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __RTD3172C_CMOS_H_ */

