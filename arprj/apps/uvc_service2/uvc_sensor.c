#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "hal_type.h"
#include "hal_vin_tuning_def.h"
#include "ar_combo_dev.h"
#include "ar_common.h"
#include "ar_comm_vi.h"

#include "uvc_sensor.h"
//#include "util_common.h"

#if defined(PROXIMA)
#define IMX307_SETTLE     0X04
#define IMX307_HDR_SETTLE     0X04  
#define OS04A10_SETTLE     0x03
#define OS04A10_HDR_SETTLE    0x03 
#else
#define IMX307_SETTLE     0X18
#define IMX307_HDR_SETTLE     0X18  
#define OS04A10_SETTLE     0x18
#define OS04A10_HDR_SETTLE    0x18 
#endif

#define MAX_FRAME_WIDTH     8192

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1, -1, -1, -1, -1},
            75,
            IMX307_SETTLE,
            {{0, 0, 0, 0}}, //data type
            .ipi_timing =
            {
                0,
                0,
                0,
                0
            } //mipi ipi timing
        }
    },
	{
	    	{1,2,1},//reset
	    	{1,2,0},//power
	    	{-1,-1,-1},//common
            /*
                     #define CLK_SENSOR0		64
                     #define CLK_SENSOR1		65
                     #define CLK_SENSOR2		66
                     #define CLK_SENSOR3		67
               */
	    	64,
	    	37125,
	},
	{
        0,
        0,
        0,
        0
	}
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN1_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    },
    {
        {1,2,1},//reset
    	{1,2,0},//power
    	{-1,-1,-1},//common
        /*
                 #define CLK_SENSOR0		64
                 #define CLK_SENSOR1		65
                 #define CLK_SENSOR2		66
                 #define CLK_SENSOR3		67
           */
    	64,
    	37125,

    },
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 891,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_DOL,
            {0, 1, -1, -1, -1, -1, -1, -1},
            75,
            IMX307_HDR_SETTLE,

        }
    },
    {
        {1,2,1},//reset
        {1,2,0},//power
        {-1,-1,-1},//common
    /*
             #define CLK_SENSOR0		64
             #define CLK_SENSOR1		65
             #define CLK_SENSOR2		66
             #define CLK_SENSOR3		67
       */
        64,
        37125,

    },
};

static STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN0_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN1_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {4, 5, -1, -1, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN2_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {5, 7, -1, -1, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN3_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR =
{
    .devno = 3,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {8, 10, -1, -1, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN4_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {9, 11, -1, -1, -1, -1, -1, -1}
        }
    }
};


static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX377_10BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = DATA_RATE_X1,
    .img_rect = {50, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_NVP6324_2M_SP420_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_YUV420_8BIT_NORMAL,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_30FPS_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 891,
    .img_rect = {12, 22, 3840, 2160},
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            400,
            26
        }
    },
    {
        {1,2,1},//reset
    	{1,2,0},//power
    	{-1,-1,-1},//common
        /*
                 #define CLK_SENSOR0		64
                 #define CLK_SENSOR1		65
                 #define CLK_SENSOR2		66
                 #define CLK_SENSOR3		67
           */
    	64,
    	37125,

    },
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_60FPS_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1782,
    .img_rect = {12, 22, 3840, 2160},
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            400,
            35
        }
    },
    {
        {1,2,1},//reset
    	{1,2,0},//power
    	{-1,-1,-1},//common
        /*
                 #define CLK_SENSOR0		64
                 #define CLK_SENSOR1		65
                 #define CLK_SENSOR2		66
                 #define CLK_SENSOR3		67
           */
    	64,
    	37125,

    },
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 891,
    .img_rect = {12, 10, 2688, 1520},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            200,
            0xd
        }
    },
    {
        {1,2,1},//reset
    	{1,2,0},//power
    	{-1,-1,-1},//common
        /*
                 #define CLK_SENSOR0		64
                 #define CLK_SENSOR1		65
                 #define CLK_SENSOR2		66
                 #define CLK_SENSOR3		67
           */
    	64,
    	37125,

    },
};

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_WDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1188,
    .img_rect = {12, 10, 2688, 1520},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            300,
            0xd
        }
    },
    {
        {1,2,1},//reset
    	{1,2,0},//power
    	{-1,-1,-1},//common
        /*
                 #define CLK_SENSOR0		64
                 #define CLK_SENSOR1		65
                 #define CLK_SENSOR2		66
                 #define CLK_SENSOR3		67
           */
    	64,
    	37125,

    },
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_SC910GS_10BIT_9M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 600,
    .img_rect = {0, 0, 3840, 2336},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            200,
            25
        }
    }
};

static STRU_COMBO_DEV_ATTR_T DVP_4VC_TP9930_1080P_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_BT1120,
    .img_rect = {0, 0, 1920, 1080},
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN1_SENSOR_SC132GS_8BIT_2M_NOWDR_ATTR = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 480,
    .img_rect = {120, 0, 720, 1280},
    {
        .mipi_attr = {
            .input_data_type = DATA_TYPE_RAW_8BIT,
            .wdr_mode = AR_MIPI_WDR_MODE_NONE,
            .lane_id = {0, 1, -1, -1, -1, -1, -1, -1},
            .mipi_pix_clk = 75,
            .settle_count = 0x18,
        }
    },

};

static VI_DEV_ATTR_S DEV_ATTR_IMX307_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {1920, 1080},
    {
        {
            {1920 , 1080},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};


static VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_WDR2TO1_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_IMX377_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_MIPI_SP420_2M_BASE =
{
    VI_MODE_MIPI_YUV420_NORMAL,
    VI_WORK_MODE_1Multiplex,
    {0xFF000000,    0xFF0000},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_UVUV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_YUV,
    AR_FALSE,
    {1920 , 1080},
    {
        {
            {1280 , 720},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_IMX415_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,
    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {0,            1280,        0,
         /*vsync0_vhb vsync0_act vsync0_hhb*/
         0,            720,        0,
         /*vsync1_vhb vsync1_act vsync1_hhb*/
         0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_IMX464_4M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {2688, 1520},
    {
        {
            {2688 , 1520},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1520
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_SC910GS_9M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {3840, 2336},
    {
        {
            {3840 , 2336},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2336
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_TP9930_1080P_BASE =
{
    .enIntfMode = VI_MODE_BT656,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .enInputDataType = VI_DATA_TYPE_YUV,
    .bDataReverse = AR_FALSE,
    .stSize = {1920, 1080},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {1920 , 1080},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 1080,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_DEV_ATTR_S DEV_ATTR_SC132GS_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            960,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            1280,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {720, 1280},
    {
        {
            {720, 1280},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1280
    },
    DATA_RATE_X1
};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420 =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_CHN0 =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_CHN0,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420 =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_8,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_CHN0 =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_CHN0,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3000x3000_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3000, 3000,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3000x3000_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3000, 3000,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_8,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    {-1, -1}
};


static VI_PIPE_ATTR_S PIPE_ATTR_4000x3000_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    4000, 3000,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_4096x2160_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    4096, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_SP420_3DNR_RFR =
{
    VI_PIPE_BYPASS_BE, AR_TRUE, AR_TRUE,
    1920, 1080,
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_8,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_PLANAR_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_PLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2688x1520_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    2688, 1520,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_PLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1},
    0,
    1, //vfe mode ceva hdr
};

VI_PIPE_ATTR_S PIPE_ATTR_3840x2336_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    3840, 2336,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_PLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_1080P_YUV422 =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 1920,
    .u32MaxH = 1080,
    .enPixFmt = PIXEL_FORMAT_UYVY_PACKAGE_422,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_8,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE,
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
    .bDiscardProPic = AR_FALSE,
};

VI_PIPE_ATTR_S PIPE_ATTR_SC132GS_960x1280_RAW8_420_RFR =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    720, 1280,
    PIXEL_FORMAT_RGB_BAYER_8BPP,
    COMPRESS_MODE_NONE,
    //COMPRESS_MODE_DPCM_6BITS,
    DATA_BITWIDTH_8,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE,
        //COMPRESS_MODE_DPCM_6BITS,
    },
    AR_FALSE,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_1920x1080_422_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_1920x1080_400_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YUV_400,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3000x3000_422_SDR8_LINEAR =
{
    {3000, 3000},
    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3000x3000_420_SDR8_LINEAR =
{
    {3000, 3000},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3000x3000_400_SDR8_LINEAR =
{
    {3000, 3000},
    PIXEL_FORMAT_YUV_400,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3840x2160_422_SDR8_LINEAR =
{
    {3840, 2160},
    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3840x2160_420_SDR8_LINEAR =
{
    {3840, 2160},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3840x2160_400_SDR8_LINEAR =
{
    {3840, 2160},
    PIXEL_FORMAT_YUV_400,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_4000x3000_420_SDR8_LINEAR =
{
    {4000, 3000},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_4096x2160_422_SDR8_LINEAR =
{
    {4096, 2160},
    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_4096x2160_420_SDR8_LINEAR =
{
    {4096, 2160},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_3840x2160_PLANAR_420_SDR8_LINEAR =
{
    {3840, 2160},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_2688x1520_PLANAR_420_SDR8_LINEAR =
{
    {2688, 1520},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_3840x2336_420_SDR8_LINEAR =
{
    {3840, 2336},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_1080P_YUV420_SDR8_LINEAR =
{
    .stSize = {1920, 1080},
    .enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 0,
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
};

VI_CHN_ATTR_S CHN_ATTR_960x1280_PLANAR_420_SC132GS_LINEAR =
{
    {720, 1280},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

AR_S32 UVC_VI_GetComboAttrBySns(UVC_SNS_TYPE_E enSnsType, combo_dev_t MipiDev, STRU_COMBO_DEV_ATTR_T* pstComboAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            //if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            //else
            //{
            //    printf("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            //}
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX307_SLAVE_MIPI_2M_60FPS_10BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (2 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN2_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (3 == MipiDev)
            {
                memcpy(pstComboAttr,  &MIPI_2lane_CHN3_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (4 == MipiDev)
            {
                memcpy(pstComboAttr,  &MIPI_2lane_CHN4_SENSOR_IMX307_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else
            {
                printf("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX377_10BIT_8M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

       case NVP6324_MIPI_2M_30FPS_8BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_NVP6324_2M_SP420_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
       case SONY_IMX415_MIPI_8M_30FPS_12BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_30FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
       case SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1:
            printf("Coming soon...\n");
            //tmp setting
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_30FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
       case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_60FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
       case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1:
            printf("Coming soon...\n");
            //tmp setting
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_60FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case TP9930_DVP_4VC_1080P_25FPS:
        case TP9930_DVP_4VC_1080P_30FPS:
            memcpy(pstComboAttr, &DVP_4VC_TP9930_1080P_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else
            {
                printf("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_WDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
        break;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            if (0 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_SC910GS_10BIT_9M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else
            {
                printf("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }
            break;
        case SC_SC132GS_MIPI_2M_25FPS_8BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN1_SENSOR_SC132GS_8BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
		default:
            printf("not support enSnsType: %d\n", enSnsType);
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
    }


    return AR_SUCCESS;
}

AR_S32 UVC_VI_GetDevAttrBySns(UVC_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S* pstViDevAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX307_SLAVE_MIPI_2M_60FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_WDR2TO1_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX377_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case NVP6324_MIPI_2M_30FPS_8BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_MIPI_SP420_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX415_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX415_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case TP9930_DVP_4VC_1080P_25FPS:
        case TP9930_DVP_4VC_1080P_30FPS:
            memcpy(pstViDevAttr, &DEV_ATTR_TP9930_1080P_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->enIntfMode = VI_MODE_BT1120_STANDARD;
            pstViDevAttr->enDataRate = DATA_RATE_X2;
            pstViDevAttr->enWorkMode = VI_WORK_MODE_4Multiplex;
            pstViDevAttr->au32ComponentMask[0] = 0xFF000000;
            pstViDevAttr->au32ComponentMask[1] = 0x00FF0000;
            break;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX464_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX464_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC910GS_9M_BASE , sizeof(VI_DEV_ATTR_S));
            break;
        case SC_SC132GS_MIPI_2M_25FPS_8BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC132GS_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
		default:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
    }

    return AR_SUCCESS;
}

AR_S32 UVC_VI_GetPipeAttrBySns(UVC_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S* pstPipeAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPipeAttr, &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX307_SLAVE_MIPI_2M_60FPS_10BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            pstPipeAttr->enCompressMode = COMPRESS_MODE_NONE;
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPipeAttr, &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            break;

        case NVP6324_MIPI_2M_30FPS_8BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_1920x1080_SP420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_PLANAR_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_PLANAR_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case TP9930_DVP_4VC_1080P_25FPS:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_1080P_YUV422, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->stFrameRate.s32SrcFrameRate = 25;
            pstPipeAttr->stFrameRate.s32DstFrameRate = 25;
            break;
        case TP9930_DVP_4VC_1080P_30FPS:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_1080P_YUV422, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->stFrameRate.s32SrcFrameRate = 30;
            pstPipeAttr->stFrameRate.s32DstFrameRate = 30;
            break;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT:
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPipeAttr, &PIPE_ATTR_2688x1520_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2336_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SC_SC132GS_MIPI_2M_25FPS_8BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_SC132GS_960x1280_RAW8_420_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
		default:
            memcpy(pstPipeAttr, &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
    }

    return AR_SUCCESS;
}

AR_S32 UVC_VI_GetChnAttrBySns(UVC_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S* pstChnAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX307_SLAVE_MIPI_2M_60FPS_10BIT:
            memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case NVP6324_MIPI_2M_30FPS_8BIT:
            memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_PLANAR_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_PLANAR_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case TP9930_DVP_4VC_1080P_25FPS:
            memcpy(pstChnAttr, &CHN_ATTR_1080P_YUV420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stFrameRate.s32DstFrameRate = 25;
            pstChnAttr->stFrameRate.s32SrcFrameRate = 25;
            break;
        case TP9930_DVP_4VC_1080P_30FPS:
            memcpy(pstChnAttr, &CHN_ATTR_1080P_YUV420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stFrameRate.s32DstFrameRate = 30;
            pstChnAttr->stFrameRate.s32SrcFrameRate = 30;
            break;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT:
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            memcpy(pstChnAttr, &CHN_ATTR_2688x1520_PLANAR_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2336_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SC_SC132GS_MIPI_2M_25FPS_8BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_960x1280_PLANAR_420_SC132GS_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
		default:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
    }

    return AR_SUCCESS;
}



