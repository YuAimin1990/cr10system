#if !defined(__TP9930_CMOS_H_)
#define __TP9930_CMOS_H_

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
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "tp9930.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define TP9930_DUMP_REG_EN          0
#define TP9930_DETECT_DEBUG_EN      0


#define TP9930_MAX_DEV_CNT      8


#if defined(AR9341)
#define TP9930_I2C_IDX                  3
#define TP9930_0_RESET_GPIO_GROUP       3
#define TP9930_0_RESET_GPIO_PORT        1
#define TP9930_0_RESET_GPIO_NUM         5
#define TP9930_1_RESET_GPIO_GROUP       3
#define TP9930_1_RESET_GPIO_PORT        1
#define TP9930_1_RESET_GPIO_NUM         4
#elif defined(PROXIMA)
#define TP9930_I2C_IDX                  2
#define TP9930_0_RESET_GPIO_GROUP       0
#define TP9930_0_RESET_GPIO_PORT        1
#define TP9930_0_RESET_GPIO_NUM         17
#define TP9930_1_RESET_GPIO_GROUP       0
#define TP9930_1_RESET_GPIO_PORT        1
#define TP9930_1_RESET_GPIO_NUM         17
#else
#define TP9930_I2C_IDX                  3
#define TP9930_0_RESET_GPIO_GROUP       3
#define TP9930_0_RESET_GPIO_PORT        1
#define TP9930_0_RESET_GPIO_NUM         5
#define TP9930_1_RESET_GPIO_GROUP       3
#define TP9930_1_RESET_GPIO_PORT        1
#define TP9930_1_RESET_GPIO_NUM         4
#endif


// This is information about on-board tp9930 chips, which may be different based on
// different boards.
static tp9930_chip_ctx_t g_tp9930_chip_ctx[TP9930_CHIP_CNT] = {
    [0] = {
        .chip_idx = TP9930_CHIP_0,
        .i2c_idx = TP9930_I2C_IDX,
        .i2c_fd = -1,
        .i2c_slv_addr = (0x8A >> 1),
        .reset_gpio = {TP9930_0_RESET_GPIO_GROUP, TP9930_0_RESET_GPIO_PORT, TP9930_0_RESET_GPIO_NUM},
        .dvp_idx_available = ((1 << TP9930_DVP_IDX_0) | (1 << TP9930_DVP_IDX_1)),
        .dvp_idx_occupied = 0,
        .ref_cnt = 0,
        .chip_status = TP9930_STATUS_POWER_OFF,
        .power_on_ref = 0,
        .lock = PTHREAD_MUTEX_INITIALIZER,
    },
    [1] = {
        .chip_idx = TP9930_CHIP_1,
        .i2c_idx = TP9930_I2C_IDX,
        .i2c_fd = -1,
        .i2c_slv_addr = (0x88 >> 1),
        .reset_gpio = {TP9930_1_RESET_GPIO_GROUP, TP9930_1_RESET_GPIO_PORT, TP9930_1_RESET_GPIO_NUM},
        .dvp_idx_available = ((1 << TP9930_DVP_IDX_2) | (1 << TP9930_DVP_IDX_3)),
        .dvp_idx_occupied = 0,
        .ref_cnt = 0,
        .strm_ref_cnt = 0,
        .chip_status = TP9930_STATUS_POWER_OFF,
        .power_on_ref = 0,
        .lock = PTHREAD_MUTEX_INITIALIZER,
    },
};

static tp9930_dev_ctx_t g_tp9930_dev_ctx[TP9930_MAX_DEV_CNT] = {
    [0 ... TP9930_MAX_DEV_CNT-1] = {
        .ViDev = -1,
        .chip = NULL,
        .dev_of_chip = -1,
        .dvp_idx = -1,
        .ch_num = 0,
        .ch_occupied = {-1, -1, -1, -1},
        .is_bt1120 = AR_FALSE,
        .ref_cnt = 0,
        .strm_ref = 0,
        .detect_en = {AR_FALSE, AR_FALSE, AR_FALSE, AR_FALSE},
        .lock = PTHREAD_MUTEX_INITIALIZER,
    },
};

static tp9930_pipe_ctx_t g_tp9930_pipe_ctx[VI_MAX_PIPE_NUM] = {
    [0 ... VI_MAX_PIPE_NUM-1] = {
        .dev = NULL,
        .pipe_of_dev = -1,
        .ch_id = -1,
        .vc_id = -1,
        .strm_ref_cnt = 0,
    },
};


AR_S32 tp9930_read_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 *data)
{
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    return ar_hal_i2c_master_read(pipe_ctx->dev->chip->i2c_fd,
        pipe_ctx->dev->chip->i2c_slv_addr, 8, &addr, 1, data, 1);
}

AR_S32 tp9930_write_register(VI_PIPE ViPipe, AR_U8 addr, AR_U8 data)
{
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    AR_U8 buf[2];

    buf[0] = (AR_U8)addr;
    buf[1] = (AR_U8)data;

    return ar_hal_i2c_master_write(pipe_ctx->dev->chip->i2c_fd,
        pipe_ctx->dev->chip->i2c_slv_addr, 8, buf, 2);
}

static AR_S32 tp9930_init_ctx(VI_PIPE ViPipe)
{
    AR_S32 i = 0;
    AR_S32 j = 0;
    AR_S32 ret = AR_SUCCESS;
    VI_DEV_BIND_PIPE_S stDevBindPipe;
    VI_DEV ViDev = -1;
    AR_S32 dvp_idx = -1;
    AR_U32 vc_id = -1;
    VI_DEV_ATTR_S stDevAttr;
    tp9930_chip_ctx_t *chip_ctx = NULL;
    tp9930_pipe_ctx_t *pipe_ctx = NULL;
    AR_U8 dvp_occupy = 0;
    AR_BOOL chip_found = AR_FALSE;

    if (ViPipe < 0 || ViPipe >= VI_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];

    // find which vi_dev binded with the vi_pipe
    for (i = 0; i < TP9930_MAX_DEV_CNT; i++) {
        ret = AR_MPI_VI_GetDevBindPipe(i, &stDevBindPipe);
        if (ret != AR_SUCCESS)
            continue;

        for (j = 0; j < (AR_S32)stDevBindPipe.u32Num; j++) {
            if (stDevBindPipe.PipeId[j] == ViPipe)
                break;
        }

        if (j < (AR_S32)stDevBindPipe.u32Num)
            break;
    }

    if (i >= TP9930_MAX_DEV_CNT) {
        ISP_TRACE(AR_DBG_ERR, "find the dev binded with the pipe[%d] failed\n", ViPipe);
        return AR_ERR_VI_INVALID_DEVID;
    }

    ViDev = i;
    ISP_TRACE(AR_DBG_INFO, "find the vi_dev[%d] binded with the vi_pipe[%d]\n", ViDev, ViPipe);

    ret = AR_MPI_VI_GetDevAttr(ViDev, &stDevAttr);
    if (ret != AR_SUCCESS) {
        ISP_TRACE(AR_DBG_ERR, "get the dev[%d] attr failed\n", ViDev);
        return ret;
    }

    ret = AR_MPI_VI_GetMipiBindDev(ViDev, &dvp_idx);
    if (ret != AR_SUCCESS || dvp_idx < TP9930_CH_0 || dvp_idx >= TP9930_CH_CNT) {
        ISP_TRACE(AR_DBG_ERR, "find the dvp binded with the pipe[%d] failed\n", ViPipe);
        return ret;
    }
    ISP_TRACE(AR_DBG_INFO, "find the dvp[%d] binded with the pipe[%d]\n", dvp_idx, ViPipe);

    ret = AR_MPI_VI_GetPipeVCNumber(ViPipe, &vc_id);
    if (ret != AR_SUCCESS) {
        ISP_TRACE(AR_DBG_ERR, "get the pipe[%d] vc_number failed\n", ViPipe);
        return AR_ERR_VI_INVALID_PARA;
    }

    pipe_ctx->pipe_of_dev = j;
    pipe_ctx->dev = &g_tp9930_dev_ctx[ViDev];

    pthread_mutex_lock(&pipe_ctx->dev->lock);

    if (pipe_ctx->dev->ref_cnt == 0) {
        pipe_ctx->dev->ViDev = ViDev;

        if (stDevAttr.enIntfMode == VI_MODE_BT1120_STANDARD) {
            pipe_ctx->dev->is_bt1120 = AR_TRUE;
        } else if (stDevAttr.enIntfMode == VI_MODE_BT656) {
            pipe_ctx->dev->is_bt1120 = AR_FALSE;
        } else {
            ISP_TRACE(AR_DBG_ERR, "tp9930 doesn't support intf_mode[%d]\n", stDevAttr.enIntfMode);
            ret = AR_ERR_VI_INVALID_PARA;
            goto fail;
        }

        if (stDevAttr.enWorkMode == VI_WORK_MODE_1Multiplex) {
            pipe_ctx->dev->ch_num = 1;
        } else if (stDevAttr.enWorkMode == VI_WORK_MODE_2Multiplex) {
            pipe_ctx->dev->ch_num = 2;
        } else if (stDevAttr.enWorkMode == VI_WORK_MODE_4Multiplex) {
            pipe_ctx->dev->ch_num = 4;
        } else {
            ISP_TRACE(AR_DBG_ERR, "tp9930 doesn't support work_mode[%d]\n", stDevAttr.enWorkMode);
            ret = AR_ERR_VI_INVALID_PARA;
            goto fail;
        }

        pipe_ctx->dev->dvp_idx = dvp_idx;

        // Find which tp9930 chip binded with the vi_dev according to the dvp_idx of the vi_dev.
        for (i = 0; i < TP9930_CHIP_CNT && !chip_found; i++) {
            chip_ctx = &g_tp9930_chip_ctx[i];
            pthread_mutex_lock(&chip_ctx->lock);

            if (pipe_ctx->dev->is_bt1120) {
                // bt1120 is 16-bit-width, it occupies 2 dvp index.
                dvp_occupy = (0x3 << (dvp_idx & 0x2));
                if ((chip_ctx->dvp_idx_available & dvp_occupy) == dvp_occupy
                    && (chip_ctx->dvp_idx_occupied & dvp_occupy) == 0) {
                    chip_found = AR_TRUE;
                }
            } else {
                // bt656 is 8-bit-width, it occupies 1 dvp index.
                dvp_occupy = (0x1 << dvp_idx);
                if ((chip_ctx->dvp_idx_available & dvp_occupy) == dvp_occupy
                    && (chip_ctx->dvp_idx_occupied & dvp_occupy) == 0) {
                    chip_found = AR_TRUE;
                }
            }

            if (chip_found) {
                if (chip_ctx->ref_cnt == 0) {
                    chip_ctx->i2c_fd = ar_hal_i2c_open(chip_ctx->i2c_idx);
                    if (chip_ctx->i2c_fd < 0) {
                        ISP_TRACE(AR_DBG_ERR, "open i2c-%u failed\n", chip_ctx->i2c_idx);
                        pthread_mutex_unlock(&chip_ctx->lock);
                        chip_found = AR_FALSE;
                        break;
                    }

                    chip_ctx->chip_status = TP9930_STATUS_POWER_OFF;
                }

                chip_ctx->dvp_idx_occupied |= dvp_occupy;
                chip_ctx->ref_cnt++;
                pipe_ctx->dev->chip = chip_ctx;

                if (pipe_ctx->dev->is_bt1120) {
                    pipe_ctx->dev->dev_of_chip = 0;
                    pipe_ctx->dev->ch_occupied[0] = TP9930_CH_0;
                    pipe_ctx->dev->ch_occupied[1] = TP9930_CH_1;
                    pipe_ctx->dev->ch_occupied[2] = TP9930_CH_2;
                    pipe_ctx->dev->ch_occupied[3] = TP9930_CH_3;
                } else {
                    if (pipe_ctx->dev->ch_num == 4) {
                        pipe_ctx->dev->dev_of_chip = 0;
                        pipe_ctx->dev->ch_occupied[0] = TP9930_CH_0;
                        pipe_ctx->dev->ch_occupied[1] = TP9930_CH_1;
                        pipe_ctx->dev->ch_occupied[2] = TP9930_CH_2;
                        pipe_ctx->dev->ch_occupied[3] = TP9930_CH_3;
                    } else {
                        if (dvp_occupy < (chip_ctx->dvp_idx_available ^ dvp_occupy)) {
                            pipe_ctx->dev->dev_of_chip = 0;
                            pipe_ctx->dev->ch_occupied[0] = TP9930_CH_0;
                            if (pipe_ctx->dev->ch_num > 1) {
                                pipe_ctx->dev->ch_occupied[1] = TP9930_CH_1;
                            }
                        } else {
                            pipe_ctx->dev->dev_of_chip = 1;
                            pipe_ctx->dev->ch_occupied[0] = TP9930_CH_2;
                            if (pipe_ctx->dev->ch_num > 1) {
                                pipe_ctx->dev->ch_occupied[1] = TP9930_CH_3;
                            }
                        }
                    }
                }
            }

            pthread_mutex_unlock(&chip_ctx->lock);
        }

        if (!chip_found) {
            ISP_TRACE(AR_DBG_ERR, "find the tp9930 chip binded with the dev[%d] failed\n", ViDev);
            ret = AR_ERR_VI_NORM_UNMATCH;
            goto fail;
        }
    }

    pipe_ctx->ch_id = pipe_ctx->dev->ch_occupied[pipe_ctx->pipe_of_dev];
    pipe_ctx->vc_id = vc_id;

    pipe_ctx->dev->ref_cnt++;

    ar_always("Pipe[%d]: chip[%d] dvp_avail[0x%02x] dvp_occup[0x%02x] ref[%d] strm_ref[%d] st[%d] pwr_ref[%u]",
        ViPipe, pipe_ctx->dev->chip->chip_idx, pipe_ctx->dev->chip->dvp_idx_available,
        pipe_ctx->dev->chip->dvp_idx_occupied, pipe_ctx->dev->chip->ref_cnt,
        pipe_ctx->dev->chip->strm_ref_cnt, pipe_ctx->dev->chip->chip_status,
        pipe_ctx->dev->chip->power_on_ref);

    ar_always("Pipe[%d]: dev[%d] dev_of_chip[%d] dvp_idx[%d] ch_num[%d] bt1120[%d] ref[%d] strm_ref[%d] ch_occupy[%u %u %u %u]",
        ViPipe, pipe_ctx->dev->ViDev, pipe_ctx->dev->dev_of_chip, pipe_ctx->dev->dvp_idx,
        pipe_ctx->dev->ch_num, pipe_ctx->dev->is_bt1120, pipe_ctx->dev->ref_cnt, pipe_ctx->dev->strm_ref,
        pipe_ctx->dev->ch_occupied[0], pipe_ctx->dev->ch_occupied[1], pipe_ctx->dev->ch_occupied[2],
        pipe_ctx->dev->ch_occupied[3]);

    ar_always("Pipe[%d]: pipe_of_dev[%d] ch_id[%d] vc_id[%d] vmode[%d] strm_ref[%d]",
        ViPipe, pipe_ctx->pipe_of_dev, pipe_ctx->ch_id, pipe_ctx->vc_id, pipe_ctx->vmode, pipe_ctx->strm_ref_cnt);

    pthread_mutex_unlock(&pipe_ctx->dev->lock);
    return ret;

fail:
    pthread_mutex_unlock(&pipe_ctx->dev->lock);
    return ret;
}

static void tp9930_deinit_ctx(VI_PIPE ViPipe)
{
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    tp9930_dev_ctx_t *dev_ctx = pipe_ctx->dev;
    tp9930_chip_ctx_t *chip_ctx = dev_ctx->chip;

    pthread_mutex_lock(&dev_ctx->lock);

    if (dev_ctx->ref_cnt > 0) {
        dev_ctx->ref_cnt--;

        if (dev_ctx->ref_cnt == 0) {
            pthread_mutex_lock(&chip_ctx->lock);

            if (chip_ctx->ref_cnt > 0) {
                chip_ctx->ref_cnt--;

                if (dev_ctx->is_bt1120) {
                    chip_ctx->dvp_idx_occupied &= ~(0x03 << (dev_ctx->dvp_idx & 0x2));
                } else {
                    chip_ctx->dvp_idx_occupied &= ~(0x01 << dev_ctx->dvp_idx);
                }

                if (chip_ctx->ref_cnt == 0) {
                    chip_ctx->dvp_idx_occupied = 0;
                    if (chip_ctx->i2c_fd >= 0) {
                        close(chip_ctx->i2c_fd);
                        chip_ctx->i2c_fd = -1;
                    }
                }
            }

            pthread_mutex_unlock(&chip_ctx->lock);

            dev_ctx->chip = NULL;
            dev_ctx->dev_of_chip = -1;
            dev_ctx->dvp_idx = -1;
            dev_ctx->ch_num = 0;
            dev_ctx->ch_occupied[0] = -1;
            dev_ctx->ch_occupied[1] = -1;
            dev_ctx->ch_occupied[2] = -1;
            dev_ctx->ch_occupied[3] = -1;
            dev_ctx->strm_ref = 0;
            dev_ctx->detect_en[0] = AR_FALSE;
            dev_ctx->detect_en[1] = AR_FALSE;
            dev_ctx->detect_en[2] = AR_FALSE;
            dev_ctx->detect_en[3] = AR_FALSE;
        }
    }

    pthread_mutex_unlock(&dev_ctx->lock);

    pipe_ctx->dev = NULL;
    pipe_ctx->pipe_of_dev = -1;
    pipe_ctx->ch_id = -1;
    return;
}

void tp9930_pll_reset(VI_PIPE ViPipe)
{
    AR_U8 val1 = 0;
    AR_U8 val2 = 0;
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];

    tp9930_write_register(ViPipe, 0x40, 0x00);

    //output disable
    tp9930_write_register(ViPipe, 0x4d, 0x00);
    tp9930_write_register(ViPipe, 0x4e, 0x00);

    //PLL reset
    tp9930_read_register(ViPipe, 0x44, &val1);
    tp9930_write_register(ViPipe, 0x44, val1|0x40);
    tp9930_read_register(ViPipe, 0xf4, &val2);
    tp9930_write_register(ViPipe, 0xf4, val2|0x80);
    usleep(10000);
    tp9930_write_register(ViPipe, 0x44, val1);

    tp9930_write_register(ViPipe, 0x40, 0x04);
    tp9930_write_register(ViPipe, 0x3b, 0x20);
    tp9930_write_register(ViPipe, 0x3d, 0xe0);
    tp9930_write_register(ViPipe, 0x3d, 0x60);
    tp9930_write_register(ViPipe, 0x3b, 0x25);
    tp9930_write_register(ViPipe, 0x40, 0x40);
    tp9930_write_register(ViPipe, 0x7a, 0x20);
    tp9930_write_register(ViPipe, 0x3c, 0x20);
    tp9930_write_register(ViPipe, 0x3c, 0x00);
    tp9930_write_register(ViPipe, 0x7a, 0x25);
    tp9930_write_register(ViPipe, 0x40, 0x00);

    tp9930_write_register(ViPipe, 0x43, 0x12);
    tp9930_write_register(ViPipe, 0x45, 0x09);

    if (pipe_ctx->dev->ch_num == 1 || pipe_ctx->dev->ch_num == 2) {
        // fix 148.5MHz pixel clock
        tp9930_write_register(ViPipe, 0x44, 0x07);
        tp9930_write_register(ViPipe, 0xf4, 0xa0);
        tp9930_write_register(ViPipe, 0xfa, 0x88);
        tp9930_write_register(ViPipe, 0xfb, 0x88);
    } else {
        if (pipe_ctx->vmode == TP2802_720P25V2 || pipe_ctx->vmode == TP2802_720P30V2) {
            if (pipe_ctx->dev->is_bt1120) {
                tp9930_write_register(ViPipe, 0x44, 0x17);
                tp9930_write_register(ViPipe, 0xf4, 0x00);
                tp9930_write_register(ViPipe, 0xfa, 0x99);
                tp9930_write_register(ViPipe, 0xfb, 0x99);
            } else {
                tp9930_write_register(ViPipe, 0x44, 0x07);
                tp9930_write_register(ViPipe, 0xf4, 0xa0);
                tp9930_write_register(ViPipe, 0xfa, 0x88);
                tp9930_write_register(ViPipe, 0xfb, 0x88);
            }
        } else {
            tp9930_write_register(ViPipe, 0x44, 0x07);
            tp9930_write_register(ViPipe, 0xf4, 0xa0);
            tp9930_write_register(ViPipe, 0xfa, 0x88);
            tp9930_write_register(ViPipe, 0xfb, 0x88);
        }
    }
}

static void tp9930_set_reg_page(VI_PIPE ViPipe, tp9930_reg_page_e page)
{
    switch (page)
    {
    case TP9930_CH_1_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x00);
        break;  // VIN1 registers
    case TP9930_CH_2_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x01);
        break;  // VIN2 registers
    case TP9930_CH_3_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x02);
        break;  // VIN3 registers
    case TP9930_CH_4_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x03);
        break;  // VIN4 registers
    case TP9930_CH_ALL_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x04);
        break;  // Write All VIN1-4 registers
    case TP9930_AUDIO_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x40);
        break;  // Audio
    case TP9930_DATA_PAGE:
        tp9930_write_register(ViPipe, 0x40, 0x10);
        break;  // PTZ data
    default:
        tp9930_write_register(ViPipe, 0x40, 0x04);
        break;
    }
}

static void tp9930_set_input(VI_PIPE ViPipe)
{
    AR_U8 val = 0;
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];

    tp9930_set_reg_page(ViPipe, pipe_ctx->ch_id);

    val = 0x44;
    if (!pipe_ctx->dev->is_bt1120)
        val |= 0x88;
    if (pipe_ctx->vmode != TP2802_1080P25 && pipe_ctx->vmode != TP2802_1080P30)
        val |= 0x02;
    tp9930_write_register(ViPipe, 0x02, val);

    switch (pipe_ctx->vmode) {
    case TP2802_1080P25:
        tp9930_write_register(ViPipe, 0x0c, 0x03);
        tp9930_write_register(ViPipe, 0x0d, 0x73);
        tp9930_write_register(ViPipe, 0x15, 0x01);
        tp9930_write_register(ViPipe, 0x16, 0xed);
        tp9930_write_register(ViPipe, 0x17, 0x80);
        tp9930_write_register(ViPipe, 0x18, 0x2a);
        tp9930_write_register(ViPipe, 0x19, 0x38);
        tp9930_write_register(ViPipe, 0x1a, 0x47);
        tp9930_write_register(ViPipe, 0x1b, 0x00);
        tp9930_write_register(ViPipe, 0x1c, 0x0a);
        tp9930_write_register(ViPipe, 0x1d, 0x50);
        tp9930_write_register(ViPipe, 0x20, 0x3c);
        tp9930_write_register(ViPipe, 0x26, 0x0d);
        tp9930_write_register(ViPipe, 0x2d, 0x54);
        tp9930_write_register(ViPipe, 0x30, 0xa5);
        tp9930_write_register(ViPipe, 0x31, 0x86);
        tp9930_write_register(ViPipe, 0x32, 0xfb);
        tp9930_write_register(ViPipe, 0x33, 0x60);
        tp9930_write_register(ViPipe, 0x35, 0x05);
        tp9930_write_register(ViPipe, 0x39, 0x1c);
        break;

    case TP2802_1080P30:
        tp9930_write_register(ViPipe, 0x0c, 0x03);
        tp9930_write_register(ViPipe, 0x0d, 0x72);
        tp9930_write_register(ViPipe, 0x15, 0x01);
        tp9930_write_register(ViPipe, 0x16, 0xed);
        tp9930_write_register(ViPipe, 0x17, 0x80);
        tp9930_write_register(ViPipe, 0x18, 0x2a);
        tp9930_write_register(ViPipe, 0x19, 0x38);
        tp9930_write_register(ViPipe, 0x1a, 0x47);
        tp9930_write_register(ViPipe, 0x1b, 0x00);
        tp9930_write_register(ViPipe, 0x1c, 0x08);
        tp9930_write_register(ViPipe, 0x1d, 0x98);
        tp9930_write_register(ViPipe, 0x20, 0x38);
        tp9930_write_register(ViPipe, 0x26, 0x0d);
        tp9930_write_register(ViPipe, 0x2d, 0x54);
        tp9930_write_register(ViPipe, 0x30, 0xa5);
        tp9930_write_register(ViPipe, 0x31, 0x95);
        tp9930_write_register(ViPipe, 0x32, 0xe0);
        tp9930_write_register(ViPipe, 0x33, 0x60);
        tp9930_write_register(ViPipe, 0x35, 0x05);
        tp9930_write_register(ViPipe, 0x39, 0x1c);
        break;

    case TP2802_720P25V2:
        tp9930_write_register(ViPipe, 0x0c, 0x13);
        tp9930_write_register(ViPipe, 0x0d, 0x71);
        tp9930_write_register(ViPipe, 0x15, 0x13);
        tp9930_write_register(ViPipe, 0x16, 0x18);
        tp9930_write_register(ViPipe, 0x17, 0x00);
        tp9930_write_register(ViPipe, 0x18, 0x1b);
        tp9930_write_register(ViPipe, 0x19, 0xd0);
        tp9930_write_register(ViPipe, 0x1a, 0x25);
        tp9930_write_register(ViPipe, 0x1b, 0x00);
        tp9930_write_register(ViPipe, 0x1c, 0x07);
        tp9930_write_register(ViPipe, 0x1d, 0xbc);
        tp9930_write_register(ViPipe, 0x20, 0x40);
        tp9930_write_register(ViPipe, 0x26, 0x01);
        tp9930_write_register(ViPipe, 0x2d, 0x5a);
        tp9930_write_register(ViPipe, 0x30, 0x9e);
        tp9930_write_register(ViPipe, 0x31, 0x20);
        tp9930_write_register(ViPipe, 0x32, 0x01);
        tp9930_write_register(ViPipe, 0x33, 0x90);
        tp9930_write_register(ViPipe, 0x35, 0x25);
        tp9930_write_register(ViPipe, 0x39, 0x18);

        tp9930_write_register(ViPipe, 0x1e, 0x60);
        tp9930_write_register(ViPipe, 0x1f, 0x06);
        tp9930_write_register(ViPipe, 0x2f, 0x06);
        break;

    case TP2802_720P30V2:
        tp9930_write_register(ViPipe, 0x0c, 0x13);
        tp9930_write_register(ViPipe, 0x0d, 0x70);
        tp9930_write_register(ViPipe, 0x15, 0x13);
        tp9930_write_register(ViPipe, 0x16, 0x16);
        tp9930_write_register(ViPipe, 0x17, 0x00);
        tp9930_write_register(ViPipe, 0x18, 0x1b);
        tp9930_write_register(ViPipe, 0x19, 0xd0);
        tp9930_write_register(ViPipe, 0x1a, 0x25);
        tp9930_write_register(ViPipe, 0x1b, 0x00);
        tp9930_write_register(ViPipe, 0x1c, 0x06);
        tp9930_write_register(ViPipe, 0x1d, 0x72);
        tp9930_write_register(ViPipe, 0x20, 0x40);
        tp9930_write_register(ViPipe, 0x26, 0x01);
        tp9930_write_register(ViPipe, 0x2d, 0x5a);
        tp9930_write_register(ViPipe, 0x30, 0x9d);
        tp9930_write_register(ViPipe, 0x31, 0xca);
        tp9930_write_register(ViPipe, 0x32, 0x01);
        tp9930_write_register(ViPipe, 0x33, 0xd0);
        tp9930_write_register(ViPipe, 0x35, 0x25);
        tp9930_write_register(ViPipe, 0x39, 0x18);

        tp9930_write_register(ViPipe, 0x1e, 0x60);
        tp9930_write_register(ViPipe, 0x1f, 0x06);
        tp9930_write_register(ViPipe, 0x2f, 0x06);
        break;

    default:
        break;
    }

    tp9930_read_register(ViPipe, 0xf5, &val);
    if (pipe_ctx->vmode == TP2802_720P25V2 || pipe_ctx->vmode == TP2802_720P30V2)
        val |= (pipe_ctx->ch_id < TP9930_CH_CNT ? (1 << pipe_ctx->ch_id) : 0x0f);
    else
        val &= (pipe_ctx->ch_id < TP9930_CH_CNT ? ~(1 << pipe_ctx->ch_id) : 0xf0);
    tp9930_write_register(ViPipe, 0xf5, val);
}

static void tp9930_set_vc_id(VI_PIPE ViPipe)
{
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    tp9930_write_register(ViPipe, 0x40, pipe_ctx->ch_id);
    tp9930_write_register(ViPipe, 0x34, (0x10 | pipe_ctx->vc_id));
}

static void tp9930_set_output(VI_PIPE ViPipe)
{
    AR_U8 val = 0;
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    tp9930_dev_ctx_t *dev_ctx = pipe_ctx->dev;
    AR_S32 ch_id = pipe_ctx->ch_id;

    switch (pipe_ctx->dev->ch_num) {
    case 1:
        if (pipe_ctx->vmode == TP2802_720P25V2 || pipe_ctx->vmode == TP2802_720P30V2) {
            tp9930_read_register(ViPipe, 0x46, &val);
            tp9930_write_register(ViPipe, 0x46, (AR_U8)(val | (0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x47, &val);
            tp9930_write_register(ViPipe, 0x47, (AR_U8)(val | (0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x49, &val);
            tp9930_write_register(ViPipe, 0x49, (AR_U8)(val | (0x1 << ch_id)));
        } else {
            tp9930_read_register(ViPipe, 0x46, &val);
            tp9930_write_register(ViPipe, 0x46, (AR_U8)(val & ~(0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x47, &val);
            tp9930_write_register(ViPipe, 0x47, (AR_U8)(val & ~(0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x49, &val);
            tp9930_write_register(ViPipe, 0x49, (AR_U8)(val & ~(0x1 << ch_id)));
        }

        if ((pipe_ctx->dev->dvp_idx & 0x1) == 0) {
            tp9930_write_register(ViPipe, 0x50, (AR_U8)((ch_id << 4) | ch_id));
            tp9930_write_register(ViPipe, 0xf6, (AR_U8)((ch_id << 4) | ch_id));
        } else {
            tp9930_write_register(ViPipe, 0x52, (AR_U8)((ch_id << 4) | ch_id));
            tp9930_write_register(ViPipe, 0xf8, (AR_U8)((ch_id << 4) | ch_id));
        }
        break;

    case 2:
        if (pipe_ctx->vmode == TP2802_720P25V2 || pipe_ctx->vmode == TP2802_720P30V2) {
            tp9930_read_register(ViPipe, 0x46, &val);
            tp9930_write_register(ViPipe, 0x46, (AR_U8)(val | (0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x47, &val);
            tp9930_write_register(ViPipe, 0x47, (AR_U8)(val | (0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x49, &val);
            tp9930_write_register(ViPipe, 0x49, (AR_U8)(val | (0x1 << ch_id)));
        } else {
            tp9930_read_register(ViPipe, 0x46, &val);
            tp9930_write_register(ViPipe, 0x46, (AR_U8)(val & ~(0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x47, &val);
            tp9930_write_register(ViPipe, 0x47, (AR_U8)(val & ~(0x1 << ch_id)));
            tp9930_read_register(ViPipe, 0x49, &val);
            tp9930_write_register(ViPipe, 0x49, (AR_U8)(val & ~(0x1 << ch_id)));
        }

        if ((pipe_ctx->dev->dvp_idx & 0x1) == 0) {
            tp9930_write_register(ViPipe, 0x50, 0x00);
            if (dev_ctx->strm_ref & (1 << (pipe_ctx->pipe_of_dev ^ 0x1))) {
                tp9930_read_register(ViPipe, 0xf6, &val);
                val &= ~(0x0f << (pipe_ctx->pipe_of_dev * 4));
                val |= (ch_id << (pipe_ctx->pipe_of_dev * 4));
                tp9930_write_register(ViPipe, 0xf6, val);
            } else {
                tp9930_write_register(ViPipe, 0xf6, (AR_U8)((ch_id << 4) | ch_id));
            }
        } else {
            tp9930_write_register(ViPipe, 0x52, 0x00);
            if (dev_ctx->strm_ref & (1 << (pipe_ctx->pipe_of_dev ^ 0x1))) {
                tp9930_read_register(ViPipe, 0xf8, &val);
                val &= ~(0x0f << (pipe_ctx->pipe_of_dev * 4));
                val |= (ch_id << (pipe_ctx->pipe_of_dev * 4));
                tp9930_write_register(ViPipe, 0xf8, val);
            } else {
                tp9930_write_register(ViPipe, 0xf8, (AR_U8)((ch_id << 4) | ch_id));
            }
        }
        break;

    case 4:
        if (pipe_ctx->dev->chip->chip_status == TP9930_STATUS_INIT) {
            tp9930_write_register(ViPipe, 0x46, 0x00);
            tp9930_write_register(ViPipe, 0x47, 0x00);
            tp9930_write_register(ViPipe, 0x49, 0x00);

            if (pipe_ctx->dev->is_bt1120) {
                tp9930_write_register(ViPipe, 0x50, 0xb2);
                tp9930_write_register(ViPipe, 0x52, 0xf6);
                tp9930_write_register(ViPipe, 0xf6, 0x10);
                tp9930_write_register(ViPipe, 0xf8, 0x54);
            } else {
                tp9930_write_register(ViPipe, 0x50, 0xb2);
                tp9930_write_register(ViPipe, 0x52, 0x00);
                tp9930_write_register(ViPipe, 0xf6, 0x10);
                tp9930_write_register(ViPipe, 0xf8, 0x00);
            }
        }
        break;

    default:
        break;
    }

    tp9930_read_register(ViPipe, 0xf5, &val);
    val |= 0xf0;
    tp9930_write_register(ViPipe, 0xf5, val);
}

#if TP9930_DUMP_REG_EN
static void tp9930_dump_reg(VI_PIPE ViPipe)
{
    AR_S32 i = 0;
    AR_U32 j = 0;
    AR_U8 val = 0;

    ISP_TRACE(AR_DBG_INFO, "\ntp9930 dump registers start\n");

    for(i = 0; i < 4; i++) {
        tp9930_set_reg_page(ViPipe, i);
        ISP_TRACE(AR_DBG_INFO, "CH_%d PAGE:\n", i);
        for(j = 0; j < 0x100; j++) {
            tp9930_read_register(ViPipe, j, &val);
            ISP_TRACE(AR_DBG_INFO, "\taddr = 0x%02x val = 0x%02x", (AR_U8)j, val);
        }
        ISP_TRACE(AR_DBG_INFO, "\n");
    }

    ISP_TRACE(AR_DBG_INFO, "\ntp9930 dump registers end\n");
}
#endif

static AR_VOID tp9930_power_on_internal(tp9930_chip_ctx_t *chip_ctx)
{
    AR_S32 reset_gpio = 0;

    if (chip_ctx->power_on_ref == 0) {
        reset_gpio = ar_hal_gpio_name_to_num(chip_ctx->reset_gpio[0],
            chip_ctx->reset_gpio[1], chip_ctx->reset_gpio[2]);
        ar_hal_gpio_export(reset_gpio);
        ar_hal_gpio_set_dir(reset_gpio, 1);     //set the gpio dir to out
        ar_hal_gpio_set_value(reset_gpio, 0);
        usleep(5000);
        ar_hal_gpio_set_value(reset_gpio, 1);
        usleep(5000);
        chip_ctx->chip_status = TP9930_STATUS_POWER_ON;
    }

    chip_ctx->power_on_ref++;
}

static AR_VOID tp9930_power_off_internal(tp9930_chip_ctx_t *chip_ctx)
{
    AR_S32 reset_gpio = 0;

    chip_ctx->power_on_ref--;

    if (chip_ctx->power_on_ref > 0) {
        ISP_TRACE(AR_DBG_INFO, "the tp9930 chip[%d] is still used", chip_ctx->chip_idx);
    } else {
        reset_gpio = ar_hal_gpio_name_to_num(chip_ctx->reset_gpio[0],
            chip_ctx->reset_gpio[1], chip_ctx->reset_gpio[2]);
        ar_hal_gpio_export(reset_gpio);
        ar_hal_gpio_set_dir(reset_gpio, 1);     //set the gpio dir to out
        ar_hal_gpio_set_value(reset_gpio, 0);
        chip_ctx->chip_status = TP9930_STATUS_POWER_OFF;
    }
}

static AR_S32 tp9930_power_on(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    tp9930_dev_ctx_t *dev_ctx = g_tp9930_pipe_ctx[ViPipe].dev;
    tp9930_chip_ctx_t *chip_ctx = NULL;

    pthread_mutex_lock(&dev_ctx->lock);
    chip_ctx = dev_ctx->chip;
    pthread_mutex_lock(&chip_ctx->lock);

    tp9930_power_on_internal(chip_ctx);

    pthread_mutex_unlock(&chip_ctx->lock);
    pthread_mutex_unlock(&dev_ctx->lock);
    return 0;
}

static AR_S32 tp9930_power_off(VI_PIPE ViPipe, ar_dev_power_attr_t *p_dev_power_attr)
{
    tp9930_dev_ctx_t *dev_ctx = g_tp9930_pipe_ctx[ViPipe].dev;
    tp9930_chip_ctx_t *chip_ctx = NULL;

    pthread_mutex_lock(&dev_ctx->lock);
    chip_ctx = dev_ctx->chip;
    pthread_mutex_lock(&chip_ctx->lock);

    tp9930_power_off_internal(chip_ctx);

    pthread_mutex_unlock(&chip_ctx->lock);
    pthread_mutex_unlock(&dev_ctx->lock);
    return 0;
}

AR_S32 tp9930_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    tp9930_pipe_ctx_t *pipe_ctx = NULL;

    ISP_TRACE(AR_DBG_INFO, "pipe[%d] w[%u] h[%u] fps[%f] sns_mode[%u]\n",
        ViPipe, pstSensorImageMode->u16Width, pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps, (AR_U32)pstSensorImageMode->u8SnsMode);

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];

    if (pstSensorImageMode->u16Width == 1920 && pstSensorImageMode->u16Height == 1080) {
        if (pstSensorImageMode->f32Fps == 25)
            pipe_ctx->vmode = TP2802_1080P25;
        else if (pstSensorImageMode->f32Fps == 30)
            pipe_ctx->vmode = TP2802_1080P30;
        else
            return -1;
    } else if (pstSensorImageMode->u16Width == 1280 && pstSensorImageMode->u16Height == 720) {
        if (pstSensorImageMode->f32Fps == 25)
            pipe_ctx->vmode = TP2802_720P25V2;
        else if (pstSensorImageMode->f32Fps == 30)
            pipe_ctx->vmode = TP2802_720P30V2;
        else
            return -1;
    } else {
        return -1;
    }

    return 0;
}

void tp9930_init(VI_PIPE ViPipe)
{
    AR_U8 val1 = 0;
    AR_U8 val2 = 0;
    AR_S32 i = 0;
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    tp9930_dev_ctx_t *dev_ctx = pipe_ctx->dev;
    tp9930_chip_ctx_t *chip_ctx = dev_ctx->chip;

    pthread_mutex_lock(&dev_ctx->lock);
    pthread_mutex_lock(&chip_ctx->lock);

    if (pipe_ctx->strm_ref_cnt == 0) {
        if (chip_ctx->chip_status == TP9930_STATUS_POWER_ON) {
            tp9930_pll_reset(ViPipe);
            for (i = 0; i < sizeof(tp9930_comm_setting)/sizeof(tp9930_reg_t); i++)
                tp9930_write_register(ViPipe, tp9930_comm_setting[i].addr, tp9930_comm_setting[i].val);
            chip_ctx->chip_status = TP9930_STATUS_INIT;
        }

        if (pipe_ctx->strm_ref_cnt == 0) {
            tp9930_set_input(ViPipe);
            tp9930_set_vc_id(ViPipe);
            tp9930_set_output(ViPipe);
        }

        if (chip_ctx->chip_status == TP9930_STATUS_INIT) {
            //PLL reset
            tp9930_read_register(ViPipe, 0x44, &val1);
            tp9930_write_register(ViPipe, 0x44, (val1 | 0x40));
            tp9930_read_register(ViPipe, 0xf4, &val2);
            tp9930_write_register(ViPipe, 0xf4, (val2 | 0x80));
            usleep(10000);
            tp9930_write_register(ViPipe, 0x44, val1);

            chip_ctx->chip_status = TP9930_STATUS_STREAM_ON;
        }

        if (dev_ctx->strm_ref == 0) {
            if (dev_ctx->ch_num == 4) {
                if (dev_ctx->is_bt1120) {
                    tp9930_write_register(ViPipe, 0x4d, 0x07);
                    tp9930_write_register(ViPipe, 0x4e, 0x05);
                } else {
                    tp9930_write_register(ViPipe, 0x4d, 0x03);
                    tp9930_write_register(ViPipe, 0x4e, 0x01);
                }
            } else {
                if (dev_ctx->dev_of_chip == 0) {
                    tp9930_read_register(ViPipe, 0x4d, &val1);
                    val1 |= 0x03;
                    tp9930_write_register(ViPipe, 0x4d, val1);
                    tp9930_read_register(ViPipe, 0x4e, &val1);
                    val1 |= 0x01;
                    tp9930_write_register(ViPipe, 0x4e, val1);
                } else if (dev_ctx->dev_of_chip == 1) {
                    tp9930_read_register(ViPipe, 0x4d, &val1);
                    val1 |= 0x04;
                    tp9930_write_register(ViPipe, 0x4d, val1);
                    tp9930_read_register(ViPipe, 0x4e, &val1);
                    val1 |= 0x04;
                    tp9930_write_register(ViPipe, 0x4e, val1);
                }
            }
        }

#if TP9930_DUMP_REG_EN
        tp9930_dump_reg(ViPipe);
#endif
    } else {
        ISP_TRACE(AR_DBG_INFO, "the tp9930 pipe[%d] is streaming on\n");
    }

    pipe_ctx->strm_ref_cnt++;
    dev_ctx->strm_ref |= (1 << pipe_ctx->pipe_of_dev);
    chip_ctx->strm_ref_cnt++;

    pthread_mutex_unlock(&chip_ctx->lock);
    pthread_mutex_unlock(&dev_ctx->lock);

    return;
}

void tp9930_exit(VI_PIPE ViPipe)
{
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    tp9930_dev_ctx_t *dev_ctx = pipe_ctx->dev;
    tp9930_chip_ctx_t *chip_ctx = dev_ctx->chip;
    AR_U8 val = 0;

    pthread_mutex_lock(&dev_ctx->lock);
    pthread_mutex_lock(&chip_ctx->lock);

    if (pipe_ctx->strm_ref_cnt > 0) {
        pipe_ctx->strm_ref_cnt--;
        if (pipe_ctx->strm_ref_cnt == 0) {
            dev_ctx->strm_ref &= ~(1 << pipe_ctx->pipe_of_dev);
            if (dev_ctx->strm_ref == 0) {
                if (dev_ctx->ch_num == 4) {
                    tp9930_write_register(ViPipe, 0x4d, 0x00);
                    tp9930_write_register(ViPipe, 0x4e, 0x00);
                } else {
                    if (dev_ctx->dev_of_chip == 0) {
                        tp9930_read_register(ViPipe, 0x4d, &val);
                        val &= ~0x03;
                        tp9930_write_register(ViPipe, 0x4d, val);
                        tp9930_read_register(ViPipe, 0x4e, &val);
                        val &= ~0x01;
                        tp9930_write_register(ViPipe, 0x4e, val);
                    } else if (dev_ctx->dev_of_chip == 1) {
                        tp9930_read_register(ViPipe, 0x4d, &val);
                        val &= ~0x04;
                        tp9930_write_register(ViPipe, 0x4d, val);
                        tp9930_read_register(ViPipe, 0x4e, &val);
                        val &= ~0x04;
                        tp9930_write_register(ViPipe, 0x4e, val);
                    }
                }
            }
        }

        chip_ctx->strm_ref_cnt--;
        if (chip_ctx->strm_ref_cnt == 0) {
            chip_ctx->chip_status = TP9930_STATUS_INIT;
        }
    } else {
        ISP_TRACE(AR_DBG_ERR, "the tp9930 pipe[%d] was streamed off before\n");
    }

    pthread_mutex_unlock(&chip_ctx->lock);
    pthread_mutex_unlock(&dev_ctx->lock);

    return;
}

static ISP_CMOS_DNG_COLORPARAM_S g_stTp9930DngColorParam = {
    .stWbGain1 = {256, 256, 256},
    .stWbGain2 = {256, 256, 256},
};

static AR_S32 tp9930_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    tp9930_pipe_ctx_t *pipe_ctx = NULL;
    tp9930_dev_ctx_t *dev_ctx = NULL;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    dev_ctx = pipe_ctx->dev;

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->stSensorMode.u32SensorID = TP9930_SNS_ID;
    pstDef->stSensorMode.u8SensorMode = 0;
    memcpy(&pstDef->stDngColorParam, &g_stTp9930DngColorParam,
        sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    if (pipe_ctx->vmode == TP2802_720P25V2 || pipe_ctx->vmode == TP2802_720P30V2) {
        pstDef->bDvpDownSampleEn = ((!dev_ctx->is_bt1120 && dev_ctx->ch_num == 4) ? AR_FALSE : AR_TRUE);
    } else {
        pstDef->bDvpDownSampleEn = AR_FALSE;
    }

    return AR_SUCCESS;
}

#define TP9930_VIDEO_LOSS               0x80
#define TP9930_VIDEO_DETECTED           0x08
#define TP9930_VIDEO_PROGRESSIVE        0x02

#define TP9930_DETECT_STATUS_MASK       0x0f
#define TP9930_DETECT_1080P30           0x0a
#define TP9930_DETECT_1080P25           0x0b
#define TP9930_DETECT_720P30            0x0c
#define TP9930_DETECT_720P25            0x0d

static AR_S32 tp9930_start_detect(VI_PIPE ViPipe)
{
    tp9930_dev_ctx_t *dev_ctx = g_tp9930_pipe_ctx[ViPipe].dev;
    tp9930_chip_ctx_t *chip_ctx = NULL;
    int i = 0;

    pthread_mutex_lock(&dev_ctx->lock);
    chip_ctx = dev_ctx->chip;
    pthread_mutex_lock(&chip_ctx->lock);

    tp9930_power_on_internal(chip_ctx);
    if (chip_ctx->chip_status == TP9930_STATUS_POWER_ON) {
        tp9930_pll_reset(ViPipe);
        for (i = 0; i < sizeof(tp9930_comm_setting)/sizeof(tp9930_reg_t); i++)
            tp9930_write_register(ViPipe, tp9930_comm_setting[i].addr, tp9930_comm_setting[i].val);
        chip_ctx->chip_status = TP9930_STATUS_INIT;
    }
    dev_ctx->detect_en[g_tp9930_pipe_ctx[ViPipe].pipe_of_dev] = AR_TRUE;

    pthread_mutex_unlock(&chip_ctx->lock);
    pthread_mutex_unlock(&dev_ctx->lock);
    return 0;
}

static AR_S32 tp9930_stop_detect(VI_PIPE ViPipe)
{
    tp9930_dev_ctx_t *dev_ctx = g_tp9930_pipe_ctx[ViPipe].dev;
    tp9930_chip_ctx_t *chip_ctx = NULL;

    pthread_mutex_lock(&dev_ctx->lock);
    chip_ctx = dev_ctx->chip;
    pthread_mutex_lock(&chip_ctx->lock);

    tp9930_power_off_internal(chip_ctx);
    dev_ctx->detect_en[g_tp9930_pipe_ctx[ViPipe].pipe_of_dev] = AR_FALSE;

    pthread_mutex_unlock(&chip_ctx->lock);
    pthread_mutex_unlock(&dev_ctx->lock);
    return 0;
}

static AR_S32 tp9930_get_detect_info(VI_PIPE ViPipe, ISP_SNS_DETECT_INFO_S *pstInfo)
{
    AR_S32 ret = 0;
    tp9930_pipe_ctx_t *pipe_ctx = &g_tp9930_pipe_ctx[ViPipe];
    tp9930_dev_ctx_t *dev_ctx = pipe_ctx->dev;
    tp9930_chip_ctx_t *chip_ctx = NULL;
    AR_U8 input_status = 0;
    AR_U8 detect_status = 0;

    if (pstInfo == NULL) {
        ISP_TRACE(AR_DBG_ERR, "Pipe[%d] pstInfo is null\n", ViPipe);
        return AR_ERR_VI_INVALID_NULL_PTR;
    }

    pthread_mutex_lock(&dev_ctx->lock);
    chip_ctx = dev_ctx->chip;
    if (!dev_ctx->detect_en[pipe_ctx->pipe_of_dev]) {
        ret = AR_ERR_VI_NOT_PERM;
        goto EXIT;
    }

    pthread_mutex_lock(&chip_ctx->lock);

    tp9930_set_reg_page(ViPipe, pipe_ctx->ch_id);
    tp9930_read_register(ViPipe, 0x01, &input_status);
    tp9930_read_register(ViPipe, 0x03, &detect_status);
    detect_status &= TP9930_DETECT_STATUS_MASK;

    pstInfo->enFormat = PIXEL_FORMAT_UYVY_PACKAGE_422;

    if ((input_status & TP9930_VIDEO_LOSS) == 0 && (input_status & TP9930_VIDEO_DETECTED) != 0) {
        pstInfo->bIsInterlace = ((input_status & TP9930_VIDEO_PROGRESSIVE) ? AR_FALSE : AR_TRUE);

        switch (detect_status) {
        case TP9930_DETECT_1080P30:
        case TP9930_DETECT_1080P25:
            pstInfo->bIsConnected = AR_TRUE;
            pstInfo->u16Width = 1920;
            pstInfo->u16Height = 1080;
            pstInfo->f32Fps = (detect_status == TP9930_DETECT_1080P30 ? 30 : 25);
            break;

        case TP9930_DETECT_720P30:
        case TP9930_DETECT_720P25:
            pstInfo->bIsConnected = AR_TRUE;
            pstInfo->u16Width = 1280;
            pstInfo->u16Height = 720;
            pstInfo->f32Fps = (detect_status == TP9930_DETECT_720P30 ? 30 : 25);
            break;

        default:
            pstInfo->bIsConnected = AR_FALSE;
            break;
        }
    } else {
        pstInfo->bIsConnected = AR_FALSE;
    }

    pthread_mutex_unlock(&chip_ctx->lock);

EXIT:
    pthread_mutex_unlock(&dev_ctx->lock);
    return ret;
}

static AR_S32 tp9930_sensor_ctl(VI_PIPE ViPipe, ISP_CMOS_SENSOR_CTL *pSensorCtl)
{
    AR_S32 ret = 0;

    if (ViPipe < 0 || ViPipe >= ISP_MAX_PIPE_NUM) {
        ISP_TRACE(AR_DBG_ERR, "the pipe id [%d] is invalid\n", ViPipe);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    if (pSensorCtl == NULL) {
         ISP_TRACE(AR_DBG_ERR, "null pointer of pSensorCtl\n");
         return AR_ERR_VI_INVALID_NULL_PTR;
    }

    switch (pSensorCtl->u8CtlCode) {
    case SENSOR_CTL_START_DETECT:
        ret = tp9930_start_detect(ViPipe);
        break;

    case SENSOR_CTL_STOP_DETECT:
        ret = tp9930_stop_detect(ViPipe);
        break;

    case SENSOR_CTL_GET_DETECT_INFO:
        ret = tp9930_get_detect_info(ViPipe, (ISP_SNS_DETECT_INFO_S *)(pSensorCtl->pCtlData));
        break;

    default:
        ISP_TRACE(AR_DBG_ERR, "error ctl code\n");
        ret = AR_FAILURE;
        break;
    }

    return ret;
}

static AR_S32 tp9930_init_sensor_exp_function( ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = tp9930_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = tp9930_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = AR_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = tp9930_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = tp9930_get_isp_default;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info = AR_NULL;
    pstSensorExpFunc->pfn_cmos_sns_power_on = tp9930_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = tp9930_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = tp9930_sensor_ctl;

    return AR_SUCCESS;
}

static AR_S32 tp9930_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 tp9930_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}


static AR_S32 tp9930_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    return AR_SUCCESS;
}

static AR_S32 tp9930_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = tp9930_init_ctx(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        ISP_TRACE(AR_DBG_ERR, "init ctx failed, pipe[%d] ret[%d]\n", ViPipe, s32Ret);
        return s32Ret;
    }

    stSnsAttrInfo.eSensorId = TP9930_SNS_ID;
    s32Ret  = tp9930_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = tp9930_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = tp9930_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 tp9930_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, TP9930_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, TP9930_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, TP9930_SNS_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    tp9930_deinit_ctx(ViPipe);
    return AR_SUCCESS;
}


ISP_SNS_OBJ_S stSnsTp9930Obj = {
    .pfnRegisterCallback    = tp9930_register_callback,
    .pfnUnRegisterCallback  = tp9930_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = tp9930_set_bus_info,
    .pfnSetInit             = AR_NULL,
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __TP9930_CMOS_H_ */
