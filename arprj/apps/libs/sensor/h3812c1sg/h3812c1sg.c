#if !defined(__H3812C1SG_CMOS_H_)
#define __H3812C1SG_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_comm_vi.h"
#include "ar_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_vi.h"
#include "mpi_sys.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_vin.h"
#include "h3812c1sg.h"
#include "osal.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define H3812C1SG_FPGA      0


static H3812C1SG_PIPE_CTX_S astH3812c1sgPipeCtx[VI_MAX_PIPE_NUM] = {
    [0 ... VI_MAX_PIPE_NUM-1] = {
        .s32NucFd = -1,
        .stNucCfg = {
            .intf_mode = NUC_INTF_MODE_HIKMICRO_3IN,
            .active_width = 444,
            .active_height = 336,
            .total_width = 640,
            .total_height = 625,
            .occ_buf_align = NUC_OCC_BUF_ALIGN_256,
            .blank_value = 0,
            .fs_head = 0xffffc7,
            .fs_head_bitnum = 24,
            .fs_head_ver_loc = 625 - 336 - 2,
            .fs_head_hor_loc = 640 - 168,     //((1536 + (18 * 4) + (24 / 3)) / 2) % 640,
            .ls_head = 0,
            .ls_head_bitnum = 24,
            .ls_head_ver_loc_start = 625 - 336 - 1,
            .ls_head_ver_loc_end = 625 - 2,
        },
        .stCfgData = {
            .cfg_data = {
                0x182C28F2, 0x4F8194F4, 0xA3F08000, 0x1E008A60,
                0x677010AA, 0xFFFFFFC4, 0x0003FFFF, 0xF4FBB000,
                0x116084B6, 0x0E208A12, 0xC0018A2B, 0x401003FF,
            },
            .bitnum = H3812C1SG_CFG_DATA_BITS,
        },
        .stIrSnsAttr = {
            .u32IrParamSize = 48,
            .enOccType = IR_SNS_OCC_TYPE_EXTERNAL,
            .enOccMonotonic = IR_SNS_OCC_MONOTONIC_INCREASE,
            .u32MinOccVal = 0,
            .u32MaxOccVal = 0x3E,
            .s32OccVerticalOffset = 0,
            .bOccCalibBySns = AR_TRUE,      // use the occ calibration method implemented by h3812c1sg lib
            .stSnsSize.u32Width = 444,
            .stSnsSize.u32Height = 336,
            .stRoiRect.s32X = 16,
            .stRoiRect.s32Y = 18,
            .stRoiRect.u32Width = 384,
            .stRoiRect.u32Height = 288,
        },
    },
};


static AR_VOID h3812c1sg_set_cfg_data(AR_U32 *cfg_data,
    AR_U32 start_bit, AR_U32 bit_num, AR_U32 *data)
{
    AR_U32 i = 0;
    AR_U32 src_word = 0;
    AR_U32 bit_of_src_word = 0;
    AR_U32 dst_word = 0;
    AR_U32 bit_of_dst_word = 0;

    for (i = 0; i < bit_num; i++) {
        src_word = i / 32;
        bit_of_src_word = i % 32;
        dst_word = start_bit / 32;
        bit_of_dst_word = start_bit % 32;

        if ((data[src_word] >> bit_of_src_word) & 0x1) {
            cfg_data[dst_word] |= (0x1 << bit_of_dst_word);
        } else {
            cfg_data[dst_word] &= ~(0x1 << bit_of_dst_word);
        }

        start_bit++;
    }
}

static AR_VOID h3812c1sg_get_cfg_data(AR_U32 *cfg_data,
    AR_U32 start_bit, AR_U32 bit_num, AR_U32 *data)
{
    AR_U32 word_num = (bit_num + 31) / 32;
    AR_U32 src_word = 0;
    AR_U32 bit_of_src_word = 0;
    AR_U32 dst_word = 0;
    AR_U32 bit_of_dst_word = 0;
    AR_U32 i = 0;

    for (i = 0; i < word_num; i++)
        data[i] = 0;

    for (i = 0; i < bit_num; i++) {
        src_word = start_bit / 32;
        bit_of_src_word = start_bit % 32;
        dst_word = i / 32;
        bit_of_dst_word = i % 32;
        data[dst_word] |= (((cfg_data[src_word] >> bit_of_src_word) & 0x1) << bit_of_dst_word);
        start_bit++;
    }
}

static AR_VOID h3812c1sg_set_rdm(VI_PIPE ViPipe, AR_U32 *rdm)
{
    h3812c1sg_set_cfg_data(astH3812c1sgPipeCtx[ViPipe].stCfgData.cfg_data, 199, 32, rdm);
}

static AR_VOID h3812c1sg_get_rdm(VI_PIPE ViPipe, AR_U32 *rdm)
{
    h3812c1sg_get_cfg_data(astH3812c1sgPipeCtx[ViPipe].stCfgData.cfg_data, 199, 32, rdm);
}

static AR_VOID h3812c1sg_set_rsm(VI_PIPE ViPipe, AR_U32 *rsm)
{
    h3812c1sg_set_cfg_data(astH3812c1sgPipeCtx[ViPipe].stCfgData.cfg_data, 164, 35, rsm);
}

static AR_VOID h3812c1sg_get_rsm(VI_PIPE ViPipe, AR_U32 *rsm)
{
    h3812c1sg_get_cfg_data(astH3812c1sgPipeCtx[ViPipe].stCfgData.cfg_data, 164, 35, rsm);
}


static AR_S32 h3812c1sg_init_ctx(VI_PIPE ViPipe)
{
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];

    pstPipeCtx->s32NucFd = ar_hal_vin_open_nuc_dev();
    if (pstPipeCtx->s32NucFd < 0) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] open nuc device failed\n", ViPipe);
        return -1;
    }

    return 0;
}

static void h3812c1sg_deinit_ctx(VI_PIPE ViPipe)
{
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];

    if (pstPipeCtx->s32NucFd >= 0) {
        ar_hal_vin_close_nuc_dev(pstPipeCtx->s32NucFd);
        pstPipeCtx->s32NucFd = -1;
    }
}

static AR_S32 h3812c1sg_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

#if H3812C1SG_FPGA
    AR_S32 s32VinFd = -1;

    s32VinFd = ar_hal_vin_open_vin_dev();
    if (s32VinFd < 0) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] open vin device failed\n", ViPipe);
        return -1;
    }

    ar_delay(100);

    ret = ar_hal_vin_write_reg(s32VinFd, 0x0b00001c, 1, 11, 11);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] vsk_en pull up failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ar_delay(100);

    ret = ar_hal_vin_write_reg(s32VinFd, 0x0b00001c, 0, 8, 8);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] reset pull down failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ar_delay(10);

    ret = ar_hal_vin_nuc_set_out_clk(astH3812c1sgPipeCtx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_500MHZ, 25, NUC_CLK_PHASE_0, AR_TRUE);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] enable nuc_out_clk failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ar_delay(10);

    ret = ar_hal_vin_write_reg(s32VinFd, 0x0b00001c, 1, 8, 8);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] reset pull up failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ar_hal_vin_close_vin_dev(s32VinFd);
    ar_delay(10);
    ar_always("pipe[%d] power on", ViPipe);
#else
    AR_S32 power_gpio = -1;
    AR_S32 reset_gpio = -1;

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

    // f23_vsk_en pull high
    ret = ar_hal_gpio_set_value(power_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] power_gpio pull high failed", ViPipe);
        return ret;
    }

    ar_delay(100);

    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],
        p_dev_power_attr->reset_gpio[1], p_dev_power_attr->reset_gpio[2]);
    if (reset_gpio < 0) {
        ar_err("pipe[%d] get reset_gpio num failed", ViPipe);
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

    // sensor_resest pull low
    ret = ar_hal_gpio_set_value(reset_gpio, 0);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull low failed", ViPipe);
        return ret;
    }

    ar_delay(10);

    ret = ar_hal_vin_nuc_set_out_clk(astH3812c1sgPipeCtx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 15, NUC_CLK_PHASE_0, AR_TRUE);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] enable nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(10);

    // sensor_resest pull high
    ret = ar_hal_gpio_set_value(reset_gpio, 1);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull high failed", ViPipe);
        return ret;
    }

    ar_delay(10);
    ar_always("pipe[%d] power on", ViPipe);

#endif

    return ret;
}

static AR_S32 h3812c1sg_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    AR_S32 ret = 0;

#if H3812C1SG_FPGA
    AR_S32 s32VinFd = -1;

    s32VinFd = ar_hal_vin_open_vin_dev();
    if (s32VinFd < 0) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] open vin device failed\n", ViPipe);
        return -1;
    }

    ret = ar_hal_vin_write_reg(s32VinFd, 0x0b00001c, 0, 8, 8);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] reset pull down failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ret = ar_hal_vin_nuc_set_out_clk(astH3812c1sgPipeCtx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_500MHZ, 25, NUC_CLK_PHASE_0, AR_FALSE);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] disable nuc_out_clk failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ar_delay(100);

    ret = ar_hal_vin_write_reg(s32VinFd, 0x0b00001c, 0, 11, 11);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] vsk_en pull down failed", ViPipe);
        ar_hal_vin_close_vin_dev(s32VinFd);
        return ret;
    }

    ar_delay(100);

    ar_hal_vin_close_vin_dev(s32VinFd);
    ar_always("pipe[%d] power off", ViPipe);
#else
    AR_S32 power_gpio = -1;
    AR_S32 reset_gpio = -1;

    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],
        p_dev_power_attr->reset_gpio[1], p_dev_power_attr->reset_gpio[2]);
    if (reset_gpio < 0) {
        ar_err("pipe[%d] get reset_gpio num failed", ViPipe);
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

    // sensor_resest pull low
    ret = ar_hal_gpio_set_value(reset_gpio, 0);
    if (ret) {
        ar_err("pipe[%d] reset_gpio pull low failed", ViPipe);
        return ret;
    }

    ret = ar_hal_vin_nuc_set_out_clk(astH3812c1sgPipeCtx[ViPipe].s32NucFd,
        NUC_OUT_CLK_SRC_600MHZ, 15, NUC_CLK_PHASE_0, AR_FALSE);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] disable nuc_out_clk failed", ViPipe);
        return ret;
    }

    ar_delay(100);

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

    // f23_vsk_en pull low
    ret = ar_hal_gpio_set_value(power_gpio, 0);
    if (ret) {
        ar_err("pipe[%d] power_gpio pull high failed", ViPipe);
        return ret;
    }

    ar_delay(10);
    ar_always("pipe[%d] power off", ViPipe);
#endif

    return ret;
}

static AR_S32 h3812c1sg_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    ISP_TRACE(AR_DBG_INFO, "pipe[%d] set sensor image mode: w[%u] h[%u] fps[%f] sns_mode[%u]\n",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe_id[%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstSensorImageMode->u16Width != 444 || pstSensorImageMode->u16Height != 336
        || (pstSensorImageMode->f32Fps != 25 && pstSensorImageMode->f32Fps != 15)) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] sns_image_mode[w=%u h=%u fps=%f] invalid\n", ViPipe,
            pstSensorImageMode->u16Width, pstSensorImageMode->u16Height, pstSensorImageMode->f32Fps);
        return AR_ERR_VI_INVALID_PARA;
    }

    astH3812c1sgPipeCtx[ViPipe].stImageMode = *pstSensorImageMode;

    return 0;
}

//static STRU_NUC_CMD_CFG_DATA_T stH3812c1sgCfgData = {
//    .cfg_data = {0x182C28F2, 0x4F8194F4, 0xA3F08000, 0x1E008A60,
//                 0x677010AA, 0xFFFFFFC4, 0x0003FFFF, 0xF4FBB000,
//                 0x116084B6, 0x0E208A12, 0xC0018A2B, 0x401003FF
//     },
//    .bitnum = 384,
//};

void h3812c1sg_init(VI_PIPE ViPipe)
{
    int ret = 0;

    if (astH3812c1sgPipeCtx[ViPipe].stImageMode.f32Fps == 25) {
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.total_height = 625;
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.fs_head_ver_loc = 625 - 336 - 2;
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.ls_head_ver_loc_start = 625 - 336 - 1;
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.ls_head_ver_loc_start = 625 - 2;
    } else if (astH3812c1sgPipeCtx[ViPipe].stImageMode.f32Fps == 15) {
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.total_height = 1042;
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.fs_head_ver_loc = 1042 - 336 - 2;
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.ls_head_ver_loc_start = 1042 - 336 - 1;
        astH3812c1sgPipeCtx[ViPipe].stNucCfg.ls_head_ver_loc_start = 1042 - 2;
    }

    ret = ar_hal_vin_nuc_cfg(astH3812c1sgPipeCtx[ViPipe].s32NucFd,
        &astH3812c1sgPipeCtx[ViPipe].stNucCfg);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] nuc cfg failed\n", ViPipe);
        return;
    }

    ret = ar_hal_vin_nuc_cmd(astH3812c1sgPipeCtx[ViPipe].s32NucFd,
        NUC_CMD_SET_CFG_DATA, &astH3812c1sgPipeCtx[ViPipe].stCfgData,
        sizeof(astH3812c1sgPipeCtx[ViPipe].stCfgData));
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] nuc set cfg data failed\n", ViPipe);
        return;
    }

    ret = ar_hal_vin_nuc_start(astH3812c1sgPipeCtx[ViPipe].s32NucFd);
    if (ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] nuc start failed\n", ViPipe);
    }
}

void h3812c1sg_exit(VI_PIPE ViPipe)
{
    ar_hal_vin_nuc_stop(astH3812c1sgPipeCtx[ViPipe].s32NucFd);
}

static ISP_CMOS_DNG_COLORPARAM_S stH3812c1sgDngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 h3812c1sg_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe_id[%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = H3812C1SG_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &stH3812c1sgDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));
    strcpy(pstDef->TuningPraBinName, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}

AR_S32 h3812c1sg_get_ir_sns_attr(VI_PIPE ViPipe, IR_SNS_ATTR_S *pstIrSnsAttr)
{
    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pstIrSnsAttr == NULL) {
        ar_err("pstIrSnsAttr is null");
        return AR_ERR_VI_INVALID_PARA;
    }

    *pstIrSnsAttr = astH3812c1sgPipeCtx[ViPipe].stIrSnsAttr;
    return 0;
}

AR_S32 h3812c1sg_set_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 u32ParamSize)
{
    AR_S32 s32Ret = 0;
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = NULL;
    STRU_NUC_CMD_CFG_DATA_T stCfgData = {
        .cfg_data = {0},
        .bitnum = H3812C1SG_CFG_DATA_BITS,
    };

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || u32ParamSize != (H3812C1SG_CFG_DATA_BITS / 8)) {
        ISP_TRACE(AR_DBG_ERR, "pParamData[%p] or u32ParamSize[%u] invalid\n",
            pParamData, u32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];
    memcpy(stCfgData.cfg_data, pParamData, H3812C1SG_CFG_DATA_BITS / 8);

    s32Ret = ar_hal_vin_nuc_cmd(pstPipeCtx->s32NucFd,
        NUC_CMD_SET_CFG_DATA, &stCfgData, sizeof(stCfgData));
    if (s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "pipe[%d] nuc set cfg data failed\n", ViPipe);
        return -1;
    }

    pstPipeCtx->stCfgData = stCfgData;
    return 0;
}

static AR_U32 get_bit_one_cnt(AR_U32 u32Val)
{
    AR_U32 u32Cnt = 0;
    AR_U32 i = 0;

    for (i = 0; i < sizeof(AR_U32) * 8; i++) {
        if ((u32Val >> i) & 0x1) {
            u32Cnt++;
        }
    }

    return u32Cnt;
}

static AR_S32 h3812c1sg_start_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];
    AR_VOID *pCalibParam = pstCalibCtx->stOutput.pCalibParam;
    ISP_IR_PARAMS_S stIrParams;
    ISP_IR_OCC_PARAM_S stIrOccParam;
    AR_U32 u32InitRdm = 0xffff;

    h3812c1sg_set_rdm(ViPipe, &u32InitRdm);

    s32Ret = ar_hal_vin_nuc_cmd(pstPipeCtx->s32NucFd, NUC_CMD_SET_CFG_DATA,
        &pstPipeCtx->stCfgData, sizeof(pstPipeCtx->stCfgData));
    if (s32Ret) {
        ar_err("pipe[%d] nuc set cfg data failed", ViPipe);
        s32Ret = -1;
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

    memcpy(pCalibParam, pstPipeCtx->stCfgData.cfg_data, 48);

    pstPipeCtx->s32LowPos = 0;
    pstPipeCtx->s32HighPos = 32;
    pstPipeCtx->s32MidPos = get_bit_one_cnt(u32InitRdm);
    pstPipeCtx->u32FinalRdm = u32InitRdm;

    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
    return 0;

FAIL:
    pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    return s32Ret;
}

static AR_S32 h3812c1sg_rdm_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];
    AR_VOID *pCalibParam = pstCalibCtx->stOutput.pCalibParam;
    IR_SNS_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U32 rdm = 0;

    h3812c1sg_get_rdm(ViPipe, &rdm);

    ar_always("pipe[%d] rdm calib: rdm[0x%x] mean[%u]", ViPipe, rdm, pstCalibCtx->stInput.u32PixMean);

    if (pstCalibCtx->stInput.u32PixMean < pstCalibCfg->u32TargetLowerLimit) {
        pstPipeCtx->s32HighPos = pstPipeCtx->s32MidPos;
    } else if (pstCalibCtx->stInput.u32PixMean > pstCalibCfg->u32TargetUpperLimit) {
        pstPipeCtx->s32LowPos = pstPipeCtx->s32MidPos;
    } else {
        ar_always("pipe[%d] entire sensor calib done: final_rdm[0x%x]", ViPipe, rdm);
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        return 0;
    }

    if (pstPipeCtx->s32HighPos - pstPipeCtx->s32LowPos <= 1) {
        ar_err("pipe[%d] calib failed: cannot find appropriate params", ViPipe);
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
        return -1;
    } else {
        pstPipeCtx->s32MidPos = (pstPipeCtx->s32LowPos + pstPipeCtx->s32HighPos) / 2;
        rdm = 1;
        rdm <<= pstPipeCtx->s32MidPos;
        rdm -= 1;
        h3812c1sg_set_rdm(ViPipe, &rdm);

        s32Ret = ar_hal_vin_nuc_cmd(pstPipeCtx->s32NucFd, NUC_CMD_SET_CFG_DATA,
            &pstPipeCtx->stCfgData, sizeof(pstPipeCtx->stCfgData));
        if (s32Ret) {
            ar_err("pipe[%d] nuc set cfg data failed", ViPipe);
            pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
            return -1;
        } else {
            memcpy(pCalibParam, pstPipeCtx->stCfgData.cfg_data, 48);
            pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
            return 0;
        }
    }
}

static AR_S32 h3812c1sg_occ_calib(VI_PIPE ViPipe, IR_OCC_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];
    IR_OCC_CALIB_CFG_S *pstCalibCfg = &pstCalibCtx->stInput.stCalibCfg;
    AR_U32 u32Target = (pstCalibCfg->u32TargetLowerLimit + pstCalibCfg->u32TargetUpperLimit) / 2;
    AR_U32 u32FrmId = pstCalibCtx->stInput.stCalibFrm.u32FrameId;
    IR_SNS_ATTR_S *pstIrSnsAttr = &pstPipeCtx->stIrSnsAttr;
    AR_U32 u32RawLinePixs = pstCalibCtx->stInput.stCalibFrm.u32Stride[0] / sizeof(AR_U16);
    AR_U32 u32OccStride = pstCalibCtx->stOutput.stOccBuf.u32Stride[0];
    AR_U32 u32OccBufSize = u32OccStride * pstCalibCtx->stOutput.stOccBuf.u32Height;
    AR_U16 *pu16Raw = NULL;
    AR_U8 *pu8Occ = NULL;
    AR_U8 *pu8OccFlag = NULL;
    AR_U8 *pu8OccLow = NULL;
    AR_U8 *pu8OccHigh = NULL;
    AR_U16 *pu16MinPixDelta = NULL;
    AR_U8 *pu8FinalOcc = NULL;
    AR_U16 u16PixDelta = 0;
    AR_BOOL bAllDone = AR_TRUE;
    AR_U16 cur_pix0 = 0;
    AR_U8 cur_occ0 = 0;
    AR_U8 next_occ0 = 0;
    AR_U8 final_occ0 = 0;
    ISP_IR_PARAMS_S stIrParams;
    ISP_IR_OCC_PARAM_S stIrOccParam;
    AR_U32 i = 0;
    AR_U32 j = 0;

    if (u32FrmId == 0) {
        AR_U32 u32AssistBufSize = pstIrSnsAttr->stRoiRect.u32Width * pstIrSnsAttr->stRoiRect.u32Height;

        if (pstPipeCtx->pu8OccFlag) {
            free(pstPipeCtx->pu8OccFlag);
            pstPipeCtx->pu8OccFlag = NULL;
            pstPipeCtx->pu8OccLow = NULL;
            pstPipeCtx->pu8OccHigh = NULL;
            pstPipeCtx->pu16MinPixDelta = NULL;
            pstPipeCtx->pu8FinalOcc = NULL;
        }

        pstPipeCtx->pu8OccFlag = malloc(u32AssistBufSize * 5 + u32OccBufSize);
        if (pstPipeCtx->pu8OccFlag == NULL) {
            ar_err("pipe[%d] allocate h3812c1sg occ calib assist buf failed", ViPipe);
            pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
            goto CALIB_END;
        }

        pstPipeCtx->pu8OccLow = pstPipeCtx->pu8OccFlag + u32AssistBufSize;
        pstPipeCtx->pu8OccHigh = pstPipeCtx->pu8OccLow + u32AssistBufSize;
        pstPipeCtx->pu8FinalOcc = pstPipeCtx->pu8OccHigh + u32AssistBufSize;
        pstPipeCtx->pu16MinPixDelta = (AR_U16 *)(pstPipeCtx->pu8FinalOcc + u32OccBufSize);

        ar_always("pipe[%d] h3812c1sg occ calib: method[%d] step[%u] frm_intrvl[%u] target_range[%u %u] cond[%d] percent[%f] max_iterate_times[%u]",
            ViPipe, pstCalibCfg->enCalibMethod, pstCalibCfg->u32OccSearchStep, pstCalibCfg->u32FrmInterval,
            pstCalibCfg->u32TargetLowerLimit, pstCalibCfg->u32TargetUpperLimit, pstCalibCfg->enCalibCond,
            pstCalibCfg->fPixsPercent, pstCalibCfg->u32MaxIterateTimes);

        ar_always("pipe[%d] h3812c1sg occ calib: frm_w[%u] frm_h[%u] frm_stride[%u] occ_w[%u] occ_h[%u] occ_stride[%u]",
            ViPipe, pstCalibCtx->stInput.stCalibFrm.u32Width, pstCalibCtx->stInput.stCalibFrm.u32Height,
            pstCalibCtx->stInput.stCalibFrm.u32Stride[0], pstCalibCtx->stOutput.stOccBuf.u32Width,
            pstCalibCtx->stOutput.stOccBuf.u32Height, pstCalibCtx->stOutput.stOccBuf.u32Stride[0]);

        ar_always("pipe[%d] h3812c1sg occ calib: occ_flag[%p] occ_low[%p] occ_high[%p] final_occ[%p] min_pix_delta[%p]",
            ViPipe, pstPipeCtx->pu8OccFlag, pstPipeCtx->pu8OccLow, pstPipeCtx->pu8OccHigh,
            pstPipeCtx->pu8FinalOcc, pstPipeCtx->pu16MinPixDelta);

        memset(pstPipeCtx->pu8OccFlag, 0, u32AssistBufSize);
        memset(pstPipeCtx->pu8OccLow, pstIrSnsAttr->u32MinOccVal, u32AssistBufSize);
        memset(pstPipeCtx->pu8OccHigh, pstIrSnsAttr->u32MaxOccVal, u32AssistBufSize);
        memset(pstPipeCtx->pu16MinPixDelta, 0xff, u32AssistBufSize * 2);
        memcpy(pstPipeCtx->pu8FinalOcc, (AR_VOID *)pstCalibCtx->stOutput.stOccBuf.u64VirAddr[0], u32OccBufSize);
    }

    pu16Raw = (AR_U16 *)pstCalibCtx->stInput.stCalibFrm.u64VirAddr[0];
    pu16Raw += (pstIrSnsAttr->stRoiRect.s32X + pstIrSnsAttr->stRoiRect.s32Y * u32RawLinePixs);
    cur_pix0 = pu16Raw[0];

    pu8Occ = (AR_U8 *)pstCalibCtx->stOutput.stOccBuf.u64VirAddr[0];
    pu8Occ += (pstIrSnsAttr->stRoiRect.s32X + pstIrSnsAttr->stRoiRect.s32Y * u32OccStride);
    cur_occ0 = pu8Occ[0];

    pu8OccFlag = pstPipeCtx->pu8OccFlag;
    pu8OccLow = pstPipeCtx->pu8OccLow;
    pu8OccHigh = pstPipeCtx->pu8OccHigh;
    pu16MinPixDelta = pstPipeCtx->pu16MinPixDelta;
    pu8FinalOcc = pstPipeCtx->pu8FinalOcc + (pstIrSnsAttr->stRoiRect.s32X + pstIrSnsAttr->stRoiRect.s32Y * u32OccStride);

    for (i = 0; i < pstIrSnsAttr->stRoiRect.u32Height; i++) {
        for (j = 0; j < pstIrSnsAttr->stRoiRect.u32Width; j++) {
            if (pu8OccFlag[j]) {
                continue;
            }

            if (pu16Raw[j] < u32Target) {
                u16PixDelta = u32Target - pu16Raw[j];
                pu8OccLow[j] = pu8Occ[j];
            } else if (pu16Raw[j] > u32Target) {
                u16PixDelta = pu16Raw[j] - u32Target;
                pu8OccHigh[j] = pu8Occ[j];
            } else {
                u16PixDelta = 0;
            }

            if (u16PixDelta < pu16MinPixDelta[j]) {
                pu16MinPixDelta[j] = u16PixDelta;
                pu8FinalOcc[j] = pu8Occ[j];
            }

            if (u16PixDelta > 0) {
                AR_U8 occ_mid = ((AR_U32)pu8OccLow[j] + (AR_U32)pu8OccHigh[j]) / 2;
                occ_mid &= 0xfe;

                if (occ_mid != pu8OccLow[j]) {
                    pu8Occ[j] = occ_mid;
                    bAllDone = AR_FALSE;
                } else {
                    pu8OccFlag[j] = 1;
                }
            } else {
                pu8OccFlag[j] = 1;
            }
        }

        pu16Raw += u32RawLinePixs;
        pu8Occ += u32OccStride;
        pu8OccFlag += pstIrSnsAttr->stRoiRect.u32Width;
        pu8OccLow += pstIrSnsAttr->stRoiRect.u32Width;
        pu8OccHigh += pstIrSnsAttr->stRoiRect.u32Width;
        pu16MinPixDelta += pstIrSnsAttr->stRoiRect.u32Width;
        pu8FinalOcc += u32OccStride;
    }

    pu8Occ = (AR_U8 *)pstCalibCtx->stOutput.stOccBuf.u64VirAddr[0];
    pu8Occ += (pstIrSnsAttr->stRoiRect.s32X + pstIrSnsAttr->stRoiRect.s32Y * u32OccStride);
    next_occ0 = pu8Occ[0];

    pu8FinalOcc = pstPipeCtx->pu8FinalOcc + (pstIrSnsAttr->stRoiRect.s32X + pstIrSnsAttr->stRoiRect.s32Y * u32OccStride);
    final_occ0 = pu8FinalOcc[0];

    pu16MinPixDelta = pstPipeCtx->pu16MinPixDelta;

    ar_always("pipe[%d] h3812c1sg occ calib: frm_id[%d] cur_pix0[%u] cur_occ0[%u] next_occ0[%u] final_occ0[%u] min_delta[%u]",
        ViPipe, u32FrmId, cur_pix0, cur_occ0, next_occ0, final_occ0, pu16MinPixDelta[46]);

    if (bAllDone) {
        memcpy((AR_VOID *)pstCalibCtx->stOutput.stOccBuf.u64VirAddr[0], pstPipeCtx->pu8FinalOcc, u32OccBufSize);
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
    } else {
        if (pstCalibCfg->u32MaxIterateTimes > 0 && u32FrmId + 1 >= pstCalibCfg->u32MaxIterateTimes) {
            ar_always("pipe[%d] h3812c1sg occ calib fail, iteration times up to max", ViPipe);
            memcpy((AR_VOID *)pstCalibCtx->stOutput.stOccBuf.u64VirAddr[0], pstPipeCtx->pu8FinalOcc, u32OccBufSize);
            pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_SUCCESS;
        } else {
            pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_ONGOING;
        }
    }

    stIrOccParam.bIsSingleVal = AR_FALSE;
    stIrOccParam.stOccBuf = pstCalibCtx->stOutput.stOccBuf;

    stIrParams.u32ParamCnt = 1;
    stIrParams.astParamEntry[0].enParamType = IR_PARAM_TYPE_OCC;
    stIrParams.astParamEntry[0].pParamData = &stIrOccParam;
    stIrParams.astParamEntry[0].u32ParamSize = sizeof(ISP_IR_OCC_PARAM_S);

    s32Ret = AR_MPI_ISP_SetIrParams(ViPipe, &stIrParams);
    if (s32Ret) {
        ar_err("pipe[%d] set ooc failed", ViPipe);
        pstCalibCtx->stOutput.enCalibStatus = IR_CALIB_STATUS_FAIL;
    }

    if (pstCalibCtx->stOutput.enCalibStatus == IR_CALIB_STATUS_SUCCESS) {
        ar_always("pipe[%d] h3812c1sg occ calib success", ViPipe);
        goto CALIB_END;
    } else if (pstCalibCtx->stOutput.enCalibStatus != IR_CALIB_STATUS_ONGOING) {
        goto CALIB_END;
    }

    return 0;

CALIB_END:
    if (pstPipeCtx->pu8OccFlag) {
        free(pstPipeCtx->pu8OccFlag);
        pstPipeCtx->pu8OccFlag = NULL;
        pstPipeCtx->pu8OccLow = NULL;
        pstPipeCtx->pu8OccHigh = NULL;
        pstPipeCtx->pu16MinPixDelta = NULL;
        pstPipeCtx->pu8FinalOcc = NULL;
    }
    return s32Ret;
}

static AR_S32 h3812c1sg_sns_calib(VI_PIPE ViPipe, IR_SNS_CALIB_CTX_S *pstCalibCtx)
{
    AR_S32 s32Ret = 0;

    if (pstCalibCtx->stInput.stCalibFrm.u32FrameId == 0) {
        s32Ret = h3812c1sg_start_sns_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] start sensor calib failed", ViPipe);
        } else {
            ar_always("pipe[%d] sensor calib started", ViPipe);
        }
    } else {
        s32Ret = h3812c1sg_rdm_calib(ViPipe, pstCalibCtx);
        if (s32Ret) {
            ar_err("pipe[%d] ra_adj calib failed", ViPipe);
        }
    }

    return s32Ret;
}


AR_S32 h3812c1sg_get_ir_param(VI_PIPE ViPipe, AR_VOID *pParamData, AR_U32 *pu32ParamSize)
{
    H3812C1SG_PIPE_CTX_S *pstPipeCtx = &astH3812c1sgPipeCtx[ViPipe];

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ar_err("the pipe id [%d] is invalid", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pParamData == NULL || pu32ParamSize == NULL) {
        ar_err("pParamData[%p] or pu32ParamSize[%p] is invalid",
            pParamData, pu32ParamSize);
        return AR_ERR_VI_INVALID_PARA;
    }

    memcpy(pParamData, pstPipeCtx->stCfgData.cfg_data, 48);
    *pu32ParamSize = 48;

    return 0;
}

AR_S32 h3812c1sg_switch_shutter(VI_PIPE ViPipe, AR_BOOL bOpen)
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


static AR_S32 h3812c1sg_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = h3812c1sg_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = h3812c1sg_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = h3812c1sg_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = h3812c1sg_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = h3812c1sg_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = h3812c1sg_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_ir_sns_attr = h3812c1sg_get_ir_sns_attr;
    pstSensorExpFunc->pfn_cmos_set_ir_param = h3812c1sg_set_ir_param;
    pstSensorExpFunc->pfn_cmos_get_ir_param = h3812c1sg_get_ir_param;
    pstSensorExpFunc->pfn_cmos_get_sns_temperature = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_calib = h3812c1sg_sns_calib;
    pstSensorExpFunc->pfn_cmos_ir_occ_calib = h3812c1sg_occ_calib;
    pstSensorExpFunc->pfn_cmos_switch_shutter = h3812c1sg_switch_shutter;

    return AR_SUCCESS;
}

#if 0
static AR_S32 h3812c1sg_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    pstAeSnsDft->f32Fps = 25;   //g_astImx290ImageMode[ViPipe].f32Fps;
	pstAeSnsDft->f32MaxFps = 25;    //(pstSnsState->u8ImgMode == IMX290_SENSOR_1080P_30FPS_LINEAR_MODE ? 30 : g_astImx290ImageMode[ViPipe].f32Fps);
    pstAeSnsDft->u32FullLinesStd = 512; //pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;
    pstAeSnsDft->u32FullLinesMax = 512; //IMX290_FULL_LINES_MAX;
	pstAeSnsDft->u32Hmax=500;  //2640;

    return AR_SUCCESS;
}
#endif

static AR_S32 h3812c1sg_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    //pstExpFuncs->pfn_cmos_get_ae_default = h3812c1sg_get_ae_default;
    return AR_SUCCESS;
}

static AR_S32 h3812c1sg_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 h3812c1sg_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    return AR_SUCCESS;
}

static AR_S32 h3812c1sg_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = h3812c1sg_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ISP_TRACE(AR_DBG_ERR, "init ctx failed, pipe[%d] ret[%d]\n", ViPipe, s32Ret);
        return s32Ret;
    }

    stSnsAttrInfo.eSensorId = H3812C1SG_SNS_ID;
    s32Ret  = h3812c1sg_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = h3812c1sg_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = h3812c1sg_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 h3812c1sg_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, H3812C1SG_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, H3812C1SG_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, H3812C1SG_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    h3812c1sg_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}


ISP_SNS_OBJ_S stSnsH3812c1sgObj = {
    .pfnRegisterCallback    = h3812c1sg_register_callback,
    .pfnUnRegisterCallback  = h3812c1sg_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = h3812c1sg_set_bus_info,
    .pfnSetInit             = AR_NULL,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __H3812C1SG_CMOS_H_ */

