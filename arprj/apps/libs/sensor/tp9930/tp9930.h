#ifndef _TP9930_H_
#define _TP9930_H_


#include "ar_common.h"


#define TP9930_SNS_ID           9930


enum {
    TP9930_CHIP_0 = 0,
    TP9930_CHIP_1,
    TP9930_CHIP_CNT,
};

enum {
    TP9930_DVP_IDX_0 = 0,
    TP9930_DVP_IDX_1,
    TP9930_DVP_IDX_2,
    TP9930_DVP_IDX_3,
    TP9930_DVP_IDX_CNT,
};

enum {
    TP9930_CH_0 = 0,
    TP9930_CH_1,
    TP9930_CH_2,
    TP9930_CH_3,
    TP9930_CH_CNT,
};

typedef enum {
    TP9930_STATUS_POWER_OFF,
    TP9930_STATUS_POWER_ON,
    TP9930_STATUS_INIT,
    TP9930_STATUS_STREAM_ON,
} tp9930_chip_sts_e;

typedef enum {
    TP9930_CH_1_PAGE   = 0,
    TP9930_CH_2_PAGE   = 1,
    TP9930_CH_3_PAGE   = 2,
    TP9930_CH_4_PAGE   = 3,
    TP9930_CH_ALL_PAGE = 4,
    TP9930_DATA_PAGE   = 5,
    TP9930_AUDIO_PAGE  = 9
} tp9930_reg_page_e;

typedef enum {
    TP2802_1080P25  = 0x03,
    TP2802_1080P30  = 0x02,
    TP2802_720P25V2 = 0x0D,
    TP2802_720P30V2 = 0x0C,

    // the following modes are not supported currently
    TP2802_720P25   = 0x05,
    TP2802_720P30   = 0x04,
    TP2802_720P50   = 0x01,
    TP2802_720P60   = 0x00,
    TP2802_SD       = 0x06,
    INVALID_FORMAT  = 0x07,
    TP2802_PAL      = 0x08,
    TP2802_NTSC     = 0x09,
    TP2802_3M18     = 0x20,   // 2048x1536@18.75 for TVI
    TP2802_5M12     = 0x21,   // 2592x1944@12.5 for TVI
    TP2802_4M15     = 0x22,   // 2688x1520@15 for TVI
    TP2802_3M20     = 0x23,   // 2048x1536@20 for TVI
    TP2802_4M12     = 0x24,   // 2688x1520@12.5 for TVI
    TP2802_6M10     = 0x25,   // 3200x1800@10 for TVI
    TP2802_QHD30    = 0x26,   // 2560x1440@30 for TVI/HDA/HDC
    TP2802_QHD25    = 0x27,   // 2560x1440@25 for TVI/HDA/HDC
    TP2802_QHD15    = 0x28,   // 2560x1440@15 for HDA
    TP2802_QXGA18   = 0x29,   // 2048x1536@18 for HDA/TVI
    TP2802_QXGA30   = 0x2A,   // 2048x1536@30 for HDA
    TP2802_QXGA25   = 0x2B,   // 2048x1536@25 for HDA
    TP2802_4M30     = 0x2C,   // 2688x1520@30 for TVI(for future)
    TP2802_4M25     = 0x2D,   // 2688x1520@25 for TVI(for future)
    TP2802_5M20     = 0x2E,   // 2592x1944@20 for TVI/HDA
    TP2802_8M15     = 0x2f,   // 3840x2160@15 for TVI
    TP2802_8M12     = 0x30,   // 3840x2160@12.5 for TVI
    TP2802_1080P15  = 0x31,   // 1920x1080@15 for TVI
    TP2802_1080P60  = 0x32,   // 1920x1080@60 for TVI
    TP2802_960P30   = 0x33,   // 1280x960@30 for TVI
    TP2802_1080P20  = 0x34,   // 1920x1080@20 for TVI

    TP2802_VIDEO_MODE_UNKNOW = 0xff,
} tp9930_vmode_e;


typedef struct {
    AR_U8 addr;
    AR_U8 val;
} tp9930_reg_t;

typedef struct {
    AR_S32 chip_idx;
    AR_S32 i2c_idx;
    AR_S32 i2c_fd;
    AR_U32 i2c_slv_addr;
    AR_S32 reset_gpio[3];
    AR_U8 dvp_idx_available;
    AR_U8 dvp_idx_occupied;
    AR_S32 ref_cnt;
    AR_S32 strm_ref_cnt;
    tp9930_chip_sts_e chip_status;
    AR_U32 power_on_ref;
    VI_PIPE bind_pipe[TP9930_CH_CNT];
    pthread_mutex_t lock;
} tp9930_chip_ctx_t;

typedef struct {
    VI_DEV ViDev;
    tp9930_chip_ctx_t *chip;
    AR_S32 dev_of_chip;
    AR_S32 dvp_idx;
    AR_S32 ch_num;
    AR_U8 ch_occupied[TP9930_CH_CNT];
    AR_BOOL is_bt1120;
    AR_S32 ref_cnt;
    AR_S32 strm_ref;
    AR_BOOL detect_en[TP9930_CH_CNT];
    pthread_mutex_t lock;
} tp9930_dev_ctx_t;

typedef struct {
    tp9930_dev_ctx_t *dev;
    AR_S32 pipe_of_dev;
    AR_S32 ch_id;
    AR_S32 vc_id;
    tp9930_vmode_e vmode;
    AR_S32 strm_ref_cnt;
} tp9930_pipe_ctx_t;


static tp9930_reg_t tp9930_comm_setting[] = {
    {0x40, 0x04},
    {0x05, 0x00},
    {0x06, 0x32},
    {0x07, 0xc0},
    {0x08, 0x00},
    {0x09, 0x24},
    {0x0a, 0x48},
    {0x0b, 0xc0},
    {0x10, 0x00},
    {0x11, 0x40},
    {0x12, 0x40},
    {0x13, 0x00},
    {0x14, 0x00},
    {0x21, 0x46},
    {0x22, 0x36},
    {0x23, 0x3c},
    {0x24, 0x04},
    {0x25, 0xfe},
    {0x27, 0x2d},
    {0x28, 0x00},
    {0x29, 0x48},
    {0x2a, 0x30},
    {0x2b, 0x60},
    {0x2c, 0x3a},
    {0x2e, 0x40},
    {0x36, 0xca},
    {0x38, 0x00},
    {0x3a, 0x32},
    {0x3b, 0x26},
    {0x4f, 0x01},
    {0xf1, 0x04},
    {0xf2, 0x00},
    {0xf3, 0x00},

    {0x40, 0x00},
    {0x34, 0x00},
    {0x40, 0x01},
    {0x34, 0x00},
    {0x40, 0x02},
    {0x34, 0x00},
    {0x40, 0x03},
    {0x34, 0x00},

    {0x50, 0x00},
    {0x52, 0x00},
    {0xf6, 0x00},
    {0xf8, 0x00},

    // PCLK timing
#if defined(AR9341)
    {0xf2, 0x22},
    {0xf3, 0x22},
#elif defined(PROXIMA)
    {0xf2, 0x00},
    {0xf3, 0x00},
#else
    {0xf2, 0x22},
    {0xf3, 0x22},
#endif
};


#endif // _TP9930_H_