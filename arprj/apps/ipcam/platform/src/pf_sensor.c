#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "hal_type.h"
#include "hal_vin_tuning_def.h"
#include "ar_combo_dev.h"
#include "ar_common.h"
#include "ar_comm_vi.h"

#include "pf_sensor.h"
#include "util_common.h"
#include "hal_clk.h"

#if defined(PROXIMA)
#define IMX307_SETTLE     0X04
#define IMX307_HDR_SETTLE     0X04  
#define OS04A10_SETTLE     0x03
#define OS04A10_HDR_SETTLE    0x03 
#define OS05A20_SETTLE     4
#define OS05A20_MCLK_SRC   CGU_OSCIN_CLK
#define OS05A20_MCLK_SRC_KHZ 24000
#define OS05A20_MCLK_ID CGU_SENSOR_MCLK2
#define OS05A20_MCLK_ID_KHZ 24000
#else
#define IMX307_SETTLE     0X18
#define IMX307_HDR_SETTLE     0X18  
#define OS04A10_SETTLE     0x18
#define OS04A10_HDR_SETTLE    0x18
#define OS05A20_SETTLE     17
#define OS05A20_MCLK_SRC CLK_SENSOR0
#define OS05A20_MCLK_SRC_KHZ 24000
#define OS05A20_MCLK_ID OS05A20_MCLK_SRC
#define OS05A20_MCLK_ID_KHZ OS05A20_MCLK_SRC_KHZ
#endif

#define MAX_FRAME_WIDTH     8192

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_SC230AI_12BIT_2M_NOWDR_ATTR =
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
            {0, 1, -1, -1, -1, -1, -1, -1},
            75,
            0x18,
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

#if 0
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
#endif

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
            150,
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OV04A10_12BIT_4M_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 2560, 1440},
    
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            100,
            OS04A10_SETTLE
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
        24000,
    },
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OV04A10_BINNING_RAW10_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 1280, 720},
    
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            100,
            0x18
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OV04A10_BINNING_RAW12_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 1280, 720},
    
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            200,
            0x18
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


STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OV04A10_10BIT_4M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 2562, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3, -1, -1, -1, -1},
            100,
            OS04A10_HDR_SETTLE
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

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX347_12BIT_4M_ATTR =
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

static STRU_COMBO_DEV_ATTR_T DVP_4VC_TP9930_1080P_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_BT1120,
    .img_rect = {0, 0, 1920, 1080},
};

static VI_DEV_ATTR_S DEV_ATTR_SC230AI_2M_BASE =
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


VI_DEV_ATTR_S DEV_ATTR_MIPI_OS04A10_BINNING_BASE = 
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
    {1280, 720},
    {
        {
            {1280, 720},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        720
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_MIPI_OS04A10_4M_BASE = 
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
    {2560, 1440},
    {
        {
            {2560, 1440},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1440
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_MIPI_OS04A10_4M_10BIT_BASE = 
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,	0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,
    
    {
        /*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,
        
        /*hsync_hfb    hsync_act	hsync_hhb*/
        {0, 		   1280,		0,
        /*vsync0_vhb vsync0_act vsync0_hhb*/
        0, 		   720, 	   0,
        /*vsync1_vhb vsync1_act vsync1_hhb*/
        0, 		   0,			 0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {2562, 1440},
    {
        {
        	{2562, 1440},
        
        },
        {
        	VI_REPHASE_MODE_NONE,
        	VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_2To1_LINE,
        1440
    },
    DATA_RATE_X1
};

static VI_DEV_ATTR_S DEV_ATTR_IMX347_4M_BASE =
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

#if 0
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
#endif

static VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR_SC230 =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_DPCM_6BITS,
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_PLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_DPCM_6BITS
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
        COMPRESS_MODE_DPCM_6BITS
    },
    AR_FALSE,
    { -1, -1}
};

#if 0
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
#endif

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
        COMPRESS_MODE_DPCM_6BITS
    },
    AR_FALSE,
    { -1, -1}
};

#if 0
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
#endif

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
        COMPRESS_MODE_DPCM_6BITS
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

VI_PIPE_ATTR_S PIPE_ATTR_OS04A10_2560x1440_RAW10_420_3DNR_RFR = 
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    2562, 1440,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_DPCM_6BITS, //reduce bw, but need to disable for tunning and app to get raw.
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_DPCM_6BITS
    },
    AR_FALSE,
    { -1, -1}

};

VI_PIPE_ATTR_S PIPE_ATTR_OS04A10_2560x1440_RAW12_420_3DNR_RFR = 
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    2560, 1440,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_DPCM_6BITS, //reduce bw, but need to disable for tunning and app to get raw.
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_DPCM_6BITS
    },
    AR_FALSE,
    { -1, -1}

};

VI_PIPE_ATTR_S PIPE_ATTR_OS04A10_1280x720_BINNING_RAW10_420_3DNR_RFR = 
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    1280, 720,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
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

VI_PIPE_ATTR_S PIPE_ATTR_OS04A10_1280x720_BINNING_RAW12_420_3DNR_RFR = 
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    1280, 720,
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

#if 0
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
#endif

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

#if 0
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
#endif

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

#if 0
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
#endif

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

VI_CHN_ATTR_S CHN_ATTR_2560x1440_PLANAR_420_OS04A10_LINEAR = 
{
    {2560, 1440},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


VI_CHN_ATTR_S CHN_ATTR_1280x720_PLANAR_420_OS04A10_BINNING_LINEAR = 
{
    {1280, 720},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


#ifdef THERMAL_ENABLE
//******************************************************
//ISP VIN IR SIM ATTR, 10bit
//******************************************************
#define IR_SIM_WIDTH  256
#define IR_SIM_HEIGHT 192

STRU_COMBO_DEV_ATTR_T MIPI_SENSOR_ISP_VIN_IR_SIM_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, IR_SIM_WIDTH, IR_SIM_HEIGHT},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

VI_DEV_ATTR_S DEV_ATTR_ISP_VIN_IR_SIM =
{
    VI_MODE_MEM,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            IR_SIM_WIDTH,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            IR_SIM_HEIGHT,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {IR_SIM_WIDTH, IR_SIM_HEIGHT},
    {
        {
            {IR_SIM_WIDTH , IR_SIM_HEIGHT},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        IR_SIM_HEIGHT
    },
    DATA_RATE_X1
};

VI_PIPE_ATTR_S PIPE_ATTR_ISP_VIN_IR_SIM =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    IR_SIM_WIDTH, IR_SIM_HEIGHT,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
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

VI_CHN_ATTR_S CHN_ATTR_ISP_VIN_IR_SIM =
{
    {IR_SIM_WIDTH, IR_SIM_HEIGHT},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

//******************************************************
//NK1221A ATTR
//******************************************************
STRU_COMBO_DEV_ATTR_T DVP_NK1221A_ATTR = {
    .devno = 2,
    .input_mode = INPUT_MODE_CMOS,
    .img_rect = {0, 0, 768, 292},
    .dev_power_attr = {

    }
};

VI_DEV_ATTR_S DEV_ATTR_NK1221A_BASE =
{
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
        .enVsyncNeg = VI_VSYNC_NEG_LOW,
        .enHsync = VI_HSYNC_PULSE,
        .enHsyncNeg = VI_HSYNC_NEG_LOW,
        .enVsyncValid = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_LOW,
        .enDE = VI_DE_LOW_HVSYNC,
        .enDENeg = VI_DE_NEG_HIGH,
     },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {768, 292},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {768 , 292},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 292,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_PIPE_ATTR_S PIPE_ATTR_NK1221A_RAW =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 768,
    .u32MaxH = 292,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_8BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_8,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE,
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {
        .s32SrcFrameRate = 50,
        .s32DstFrameRate = 50,
    },
    .bDiscardProPic = AR_FALSE,
};

VI_CHN_ATTR_S CHN_ATTR_NK1221A_LINEAR =
{
    .stSize = {768, 292},
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 2,
    .stFrameRate = {
        .s32SrcFrameRate = 50,
        .s32DstFrameRate = 50,
    },
};


//******************************************************
//TIMO256 ATTR
//******************************************************
STRU_COMBO_DEV_ATTR_T DVP_TIMO256_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 512, 200},
    .dev_power_attr = {
        {3,0,3},//reset
        {1,0,0},//power 5v
        {3,0,0},//power 3v6
    }
};

VI_DEV_ATTR_S DEV_ATTR_TIMO256_BASE =
{
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .stSynCfg = {
        .enVsync = VI_VSYNC_FIELD,
        .enVsyncNeg = VI_VSYNC_NEG_HIGH,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_NORM_PULSE,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .enDE = VI_DE_LOW_HVSYNC,
        .enDENeg = VI_DE_NEG_HIGH,
     },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {512, 200},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {512 , 200},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 200,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_PIPE_ATTR_S PIPE_ATTR_TIMO256_RAW =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 512,
    .u32MaxH = 200,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_8BPP,
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
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
    .bDiscardProPic = AR_FALSE,
};

VI_CHN_ATTR_S CHN_ATTR_TIMO256_LINEAR =
{
    .stSize = {512, 200},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_8BPP,
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



//******************************************************
//ELF1 ATTR
//******************************************************
STRU_COMBO_DEV_ATTR_T DVP_ELF1_ATTR = {
    .devno = 1,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 520, 204},
    .dev_power_attr = {
        {0,2,2},//reset
        {-1,-1,-1},
        {-1,-1,-1},
    }
};

VI_DEV_ATTR_S DEV_ATTR_ELF1_BASE =
{
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .stSynCfg = {
        .enVsync = VI_VSYNC_FIELD,
        .enVsyncNeg = VI_VSYNC_NEG_HIGH,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_NORM_PULSE,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .enDE = VI_DE_LOW_HVSYNC,
        .enDENeg = VI_DE_NEG_HIGH,
     },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {520, 204},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {520 , 204},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 204,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_PIPE_ATTR_S PIPE_ATTR_ELF1_RAW =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 520,
    .u32MaxH = 204,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_8BPP,
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
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
    .bDiscardProPic = AR_FALSE,
};

VI_CHN_ATTR_S CHN_ATTR_ELF1_LINEAR =
{
    .stSize = {520, 204},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_8BPP,
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

#endif

VI_DEV_ATTR_S DEV_ATTR_SC530AI_5M_BASE =
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
    {2880, 1616},
    {
        {
            {2880, 1616},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1616
    },
    DATA_RATE_X1
};

STRU_COMBO_DEV_ATTR_T  MIPI_4lane_CHN0_SENSOR_SC530AI_10BIT_5M_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 792,
    .img_rect = {0, 2, 2880, 1616},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            200,
            0xe
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
	      27000,
	},
	{
        1,
        0x2e0a,
        0x07e7,
        0x0060
	}
};

VI_PIPE_ATTR_S PIPE_ATTR_SC530AI_2880x1620_RAW10_420_3DNR_RFR =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    2880, 1616,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
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

VI_CHN_ATTR_S CHN_ATTR_2880x1620_PLANAR_420_SC530AI_LINEAR =
{
    {2880, 1616},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

//following for IR sensors
//******************************************************
//GST417W ATTR
//******************************************************
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_GST417W_400X308 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 400, 308},
    .dev_power_attr = {
        .reset_gpio = {0, 6, 0},        // GPIOG_0
    },
};

static VI_DEV_ATTR_S DEV_ATTR_GST417W_400X308 = {
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .stSynCfg = {
        .enVsyncNeg = 1,
        .enHsyncNeg = 1,
        .enDE = VI_DE_HSYNC_IN,
        .enDENeg = 0,
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {400, 308},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {400, 308},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 308,
    },
    .enDataRate = DATA_RATE_X1,
    .bDvpHighLowByteSwap = AR_FALSE,
    .bIrSensor = AR_TRUE,
};

static VI_PIPE_ATTR_S PIPE_ATTR_GST417W_400X308 = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 400,
    .u32MaxH = 308,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_16,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE,
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
    .bDiscardProPic = AR_FALSE,
};

static VI_CHN_ATTR_S CHN_ATTR_GST417W_400X308 = {
    .stSize = {400, 308},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_16BPP,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 0,
    .stFrameRate = {
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
};

static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_GST212W4_256X200 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 256, 200},
    .dev_power_attr = {
        .reset_gpio = {0, 6, 0},		// GPIOG_0
    },
};

static VI_DEV_ATTR_S DEV_ATTR_GST212W4_256X200 = {
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .stSynCfg = {
        .enVsyncNeg = 1,
        .enHsyncNeg = 1,
        .enDE = VI_DE_LOW_HVSYNC,
        .enDENeg = 0,
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {256, 200},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {256, 200},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 200,
    },
    .enDataRate = DATA_RATE_X1,
    .bDvpHighLowByteSwap = AR_FALSE,
    .bIrSensor = AR_TRUE,
};

static VI_PIPE_ATTR_S PIPE_ATTR_GST212W4_256X200 = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 256,
    .u32MaxH = 200,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_16,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE,
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
    .bDiscardProPic = AR_FALSE,
};

static VI_CHN_ATTR_S CHN_ATTR_GST212W4_256X200 = {
    .stSize = {256, 200},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_16BPP,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 0,
    .stFrameRate = {
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
};

//******************************************************
//IR THERMAL ATTR
//****************************************************** 
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_ISP_VIN_IR_THERMAL = {
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, 640, 512},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

static VI_DEV_ATTR_S DEV_ATTR_ISP_VIN_IR_THERMAL = {
    VI_MODE_MEM,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            640,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            512,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {640, 512},
    {
        {
            {640 , 512},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        512
    },
    DATA_RATE_X1
};

static VI_PIPE_ATTR_S PIPE_ATTR_ISP_VIN_IR_THERMAL = {
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    640, 512,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1},
    .s32EnableManAecUpdate = 1,
};

static VI_CHN_ATTR_S CHN_ATTR_ISP_VIN_IR_THERMAL = {
    {640, 512},
    PIXEL_FORMAT_RGB_BAYER_16BPP,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

//******************************************************
//IR ISP1 ATTR
//****************************************************** 
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_ISP_VIN1_IR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, 640, 512},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

static VI_DEV_ATTR_S DEV_ATTR_ISP_VIN1_IR =
{
    VI_MODE_MEM,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            640,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            512,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {640, 512},
    {
        {
            {640 , 512},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        512
    },
    DATA_RATE_X1
};

static VI_PIPE_ATTR_S PIPE_ATTR_ISP_VIN1_IR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    640, 512,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    AR_FALSE,
    { -1, -1},
    .s32EnableManAecUpdate = 1,
};

static VI_CHN_ATTR_S CHN_ATTR_ISP_VIN1_IR =
{
    {640, 512},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

//******************************************************
//IR ISP2 ATTR
//****************************************************** 
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_ISP_VIN2_IR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, 640, 512},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_8BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

static VI_DEV_ATTR_S DEV_ATTR_ISP_VIN2_IR =
{
    VI_MODE_MEM,
    VI_WORK_MODE_1Multiplex,
    {0xFF000000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            640,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            512,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {640, 512},
    {
        {
            {640 , 512},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        512
    },
    DATA_RATE_X1
};

static VI_PIPE_ATTR_S PIPE_ATTR_ISP_VIN2_IR =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    640, 512,
    PIXEL_FORMAT_RGB_BAYER_8BPP,
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
    { -1, -1},
    .s32EnableManAecUpdate = 1,
};

static VI_CHN_ATTR_S CHN_ATTR_ISP_VIN2_IR =
{
    {640, 512},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static STRU_COMBO_DEV_ATTR_T COMBO_DEV_ATTR_OV_OS05A20_2688x1944 =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 2688, 1944},
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {1, 1, 1, 1, -1, -1, -1, -1},
            100,
            OS05A20_SETTLE
        }
    },
	{
        {1, 2, 1}, // reset
        {1, 2, 0}, // power
        {-1, -1, -1}, //common
        OS05A20_MCLK_SRC,
        OS05A20_MCLK_SRC_KHZ,
        OS05A20_MCLK_ID,
        OS05A20_MCLK_ID_KHZ,
    },
};

static VI_DEV_ATTR_S DEV_ATTR_OV_OS05A20_2688x1944 =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000, 0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,
    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,
        /*hsync_hfb    hsync_act    hsync_hhb*/
        {0,            1280,        0,
         /*vsync0_vhb vsync0_act vsync0_hhb*/
         0,            720,        0,
         /*vsync1_vhb vsync1_act vsync1_hhb*/
         0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {2688, 1944},
    {
        {
            {2688, 1944},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1944
    },
    DATA_RATE_X1
};
static VI_PIPE_ATTR_S PIPE_ATTR_OV_OS05A20_2688x1944 =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    2688, 1944,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_DPCM_6BITS
    },
    AR_FALSE,
    { -1, -1}
};
static VI_CHN_ATTR_S CHN_ATTR_OV_OS05A20_2688x1944 =
{
    {2688, 1944},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0, 0,
    0,
    {-1, -1}
};


//******************************************************
//H3812C1SG ATTR
//******************************************************
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_H3812C1SG_444X336 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 444, 336},
    .dev_power_attr = {
        .reset_gpio = {0, 6, 0},        // GPIOG_0
        .power_gpio = {0, 0, 17},       // GPIOA_17
    },
};

static VI_DEV_ATTR_S DEV_ATTR_H3812C1SG_444X336 = {
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .stSynCfg = {
        .enVsyncNeg = 1,
        .enHsyncNeg = 1,
        .enDE = VI_DE_LOW_HVSYNC,
        .enDENeg = 0,
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {444, 336},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {444, 336},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 336,
    },
    .enDataRate = DATA_RATE_X1,
    .bDvpHighLowByteSwap = AR_FALSE,
    .bIrSensor = AR_TRUE,
};

static VI_PIPE_ATTR_S PIPE_ATTR_H3812C1SG_444X336 = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 444,
    .u32MaxH = 336,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_16BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_16,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE,
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
    .bDiscardProPic = AR_FALSE,
    .enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE,
};

static VI_CHN_ATTR_S CHN_ATTR_H3812C1SG_444X336 = {
    .stSize = {444, 336},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_16BPP,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 0,
    .stFrameRate = {
        .s32SrcFrameRate = 25,
        .s32DstFrameRate = 25,
    },
};

AR_S32 IPC_PF_VI_GetComboAttrBySns(IPC_SNS_TYPE_E enSnsType, combo_dev_t MipiDev, STRU_COMBO_DEV_ATTR_T* pstComboAttr)
{
    switch (enSnsType)
    {
        case SC_SC230AI_MIPI_2M_30FPS_10BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_SC230AI_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            //if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            //else
            //{
            //    PRINT_ERR("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
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
                PRINT_ERR("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
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
            PRINT_ERR("Coming soon...\n");
            //tmp setting
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_30FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
       case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_60FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
       case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1:
            PRINT_ERR("Coming soon...\n");
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
                PRINT_ERR("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
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


#ifdef THERMAL_ENABLE
        case TIMO256_DVP_256_IR:
             ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                 &DVP_TIMO256_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
             pstComboAttr->devno = MipiDev;
             break;
        
        case ISP_VIN_COMMON_INF:
             ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                 &MIPI_SENSOR_ISP_VIN_IR_SIM_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
             pstComboAttr->devno = MipiDev;
             break;
        
        case ELF1_DVP_256_IR:
             ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                 &DVP_ELF1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
             pstComboAttr->devno = MipiDev;
             break;

         case NK1221A_DVP_384_IR:
              ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                  &DVP_NK1221A_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
              pstComboAttr->devno = MipiDev;
              break;
#endif

	 case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_SC530AI_10BIT_5M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OV04A10_12BIT_4M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OV04A10_10BIT_4M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;				
        case OV_OS04A10_MIPI_BINNING_25FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OV04A10_BINNING_RAW10_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case OV_OS04A10_MIPI_BINNING_25FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OV04A10_BINNING_RAW12_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case SONY_IMX347_MIPI_4M_30FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX347_12BIT_4M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case GST417W_IR_400X308:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_ATTR_GST417W_400X308, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;
        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 308;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;
        case ISP_VIN_IR_ISP0_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 300;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;
        case ISP_VIN_IR_ISP1_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 300;
            pstComboAttr->devno = MipiDev;
            break;
        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 308;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;
        case GST212W4_IR_256X200:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_GST212W4_256X200, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_GST212W4:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 200;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_ISP0_GST212W4:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 192;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST212W4:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 200;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_ISP1_GST212W4:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 192;
            pstComboAttr->devno = MipiDev;
            break;
        case OV_OS05A20_2688x1944:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), 
                &COMBO_DEV_ATTR_OV_OS05A20_2688x1944, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
		case H3812C1SG_IR_444X336:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_H3812C1SG_444X336, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_H3812C1SG:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 444;
            pstComboAttr->img_rect.height = 336;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_ISP0_H3812C1SG:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 384;
            pstComboAttr->img_rect.height = 288;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_ISP0_WITH_THERMAL_H3812C1SG:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 444;
            pstComboAttr->img_rect.height = 336;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_ISP1_H3812C1SG:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 384;
            pstComboAttr->img_rect.height = 288;
            pstComboAttr->devno = MipiDev;
            break;
        default:
            PRINT_ERR("not support enSnsType: %d\n", enSnsType);
            memcpy(pstComboAttr, &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
    }


    return AR_SUCCESS;
}

AR_S32 IPC_PF_VI_GetDevAttrBySns(IPC_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S* pstViDevAttr)
{
    switch (enSnsType)
    {
        case SC_SC230AI_MIPI_2M_30FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_SC230AI_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
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


#ifdef THERMAL_ENABLE
        case TIMO256_DVP_256_IR:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_TIMO256_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case ISP_VIN_COMMON_INF:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_SIM, sizeof(VI_DEV_ATTR_S));
            break;

        case ELF1_DVP_256_IR:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ELF1_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case NK1221A_DVP_384_IR:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_NK1221A_BASE, sizeof(VI_DEV_ATTR_S));
            break;
#endif

	case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC530AI_5M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OV_OS04A10_MIPI_4M_25FPS_12BIT:
           ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MIPI_OS04A10_4M_BASE, sizeof(VI_DEV_ATTR_S));
           break;
        
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
           ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MIPI_OS04A10_4M_10BIT_BASE, sizeof(VI_DEV_ATTR_S));
           break;
        case OV_OS04A10_MIPI_BINNING_25FPS_10BIT:
        case OV_OS04A10_MIPI_BINNING_25FPS_12BIT:
           ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MIPI_OS04A10_BINNING_BASE, sizeof(VI_DEV_ATTR_S));
           break;
        case SONY_IMX347_MIPI_4M_30FPS_12BIT:
           ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX347_4M_BASE, sizeof(VI_DEV_ATTR_S));
           break;   
        case GST417W_IR_400X308:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_GST417W_400X308, sizeof(VI_DEV_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 408;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 308;
            pstViDevAttr->stSize.u32Width = 408;
            pstViDevAttr->stSize.u32Height = 308;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 408;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 308;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN_IR_ISP0_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 408;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 300;
            pstViDevAttr->stSize.u32Width = 408;
            pstViDevAttr->stSize.u32Height = 300;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 408;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 300;
            break;

        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 408;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 308;
            pstViDevAttr->stSize.u32Width = 408;
            pstViDevAttr->stSize.u32Height = 308;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 408;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 308;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN_IR_ISP1_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 408;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 300;
            pstViDevAttr->stSize.u32Width = 408;
            pstViDevAttr->stSize.u32Height = 300;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 408;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 300;
            break;
        case GST212W4_IR_256X200:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_GST212W4_256X200, sizeof(VI_DEV_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_GST212W4:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 256;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 200;
            pstViDevAttr->stSize.u32Width = 256;
            pstViDevAttr->stSize.u32Height = 200;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 256;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 200;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN_IR_ISP0_GST212W4:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 256;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 192;
            pstViDevAttr->stSize.u32Width = 256;
            pstViDevAttr->stSize.u32Height = 192;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 256;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 192;
            break;

        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST212W4:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 256;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 200;
            pstViDevAttr->stSize.u32Width = 256;
            pstViDevAttr->stSize.u32Height = 200;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 256;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 200;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN_IR_ISP1_GST212W4:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 256;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 192;
            pstViDevAttr->stSize.u32Width = 256;
            pstViDevAttr->stSize.u32Height = 192;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 256;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 192;
            break;
        case OV_OS05A20_2688x1944:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), 
                &DEV_ATTR_OV_OS05A20_2688x1944, sizeof(VI_DEV_ATTR_S));
            break;
		
        case H3812C1SG_IR_444X336:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_H3812C1SG_444X336, sizeof(VI_DEV_ATTR_S));
            break;		
        case ISP_VIN_IR_THERMAL_H3812C1SG:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 444;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 336;
            pstViDevAttr->stSize.u32Width = 444;
            pstViDevAttr->stSize.u32Height = 336;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 444;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 336;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;
        case ISP_VIN_IR_ISP0_H3812C1SG:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 384;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 288;
            pstViDevAttr->stSize.u32Width = 384;
            pstViDevAttr->stSize.u32Height = 288;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 384;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 288;
            break;
        case ISP_VIN_IR_ISP0_WITH_THERMAL_H3812C1SG:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 444;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 336;
            pstViDevAttr->stSize.u32Width = 444;
            pstViDevAttr->stSize.u32Height = 336;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 444;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 336;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;
        case ISP_VIN_IR_ISP1_H3812C1SG:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 384;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 288;
            pstViDevAttr->stSize.u32Width = 384;
            pstViDevAttr->stSize.u32Height = 288;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 384;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 288;
            break;
        default:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
    }

    return AR_SUCCESS;
}

AR_S32 IPC_PF_VI_GetPipeAttrBySns(IPC_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S* pstPipeAttr)
{
    switch (enSnsType)
    {
        case SC_SC230AI_MIPI_2M_30FPS_10BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR_SC230, sizeof(VI_PIPE_ATTR_S));
            break;

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
        case SONY_IMX347_MIPI_4M_30FPS_12BIT:
            memcpy(pstPipeAttr, &PIPE_ATTR_2688x1520_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2336_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

#ifdef THERMAL_ENABLE
        case TIMO256_DVP_256_IR:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_TIMO256_RAW, sizeof(VI_PIPE_ATTR_S));
            break;
            
        case ISP_VIN_COMMON_INF:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_SIM, sizeof(VI_PIPE_ATTR_S));
            break;
            
        case ELF1_DVP_256_IR:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ELF1_RAW, sizeof(VI_PIPE_ATTR_S));
            break;
            
        case NK1221A_DVP_384_IR:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_NK1221A_RAW, sizeof(VI_PIPE_ATTR_S));
            break;
#endif

	case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_SC530AI_2880x1620_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_12BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OS04A10_2560x1440_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OS04A10_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
		case OV_OS04A10_MIPI_BINNING_25FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OS04A10_1280x720_BINNING_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
		case OV_OS04A10_MIPI_BINNING_25FPS_12BIT:
			ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OS04A10_1280x720_BINNING_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;			
        case GST417W_IR_400X308:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_GST417W_400X308, sizeof(VI_PIPE_ATTR_S)); 
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 308;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_ISP0_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 300;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 308;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_ISP1_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 300;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case GST212W4_IR_256X200:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_GST212W4_256X200, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_THERMAL_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 200;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_ISP0_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 192;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 200;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;

        case ISP_VIN_IR_ISP1_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 192;
            pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case OV_OS05A20_2688x1944:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), 
                &PIPE_ATTR_OV_OS05A20_2688x1944, sizeof(VI_PIPE_ATTR_S));
            break;
			
        case H3812C1SG_IR_444X336:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_H3812C1SG_444X336, sizeof(VI_PIPE_ATTR_S));
			pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case ISP_VIN_IR_THERMAL_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 444;
            pstPipeAttr->u32MaxH = 336;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
			pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case ISP_VIN_IR_ISP0_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 384;
            pstPipeAttr->u32MaxH = 288;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
			pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case ISP_VIN_IR_ISP0_WITH_THERMAL_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 444;
            pstPipeAttr->u32MaxH = 336;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
			pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
        case ISP_VIN_IR_ISP1_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 384;
            pstPipeAttr->u32MaxH = 288;
			pstPipeAttr->enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
            break;
		default:
            memcpy(pstPipeAttr, &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
    }

    return AR_SUCCESS;
}

AR_S32 IPC_PF_VI_GetChnAttrBySns(IPC_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S* pstChnAttr)
{
    switch (enSnsType)
    {
        case SC_SC230AI_MIPI_2M_30FPS_10BIT:
            memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        
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
        case SONY_IMX347_MIPI_4M_30FPS_12BIT:
            memcpy(pstChnAttr, &CHN_ATTR_2688x1520_PLANAR_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2336_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
			
#ifdef THERMAL_ENABLE
        case TIMO256_DVP_256_IR:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_TIMO256_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case ISP_VIN_COMMON_INF:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_SIM, sizeof(VI_CHN_ATTR_S));
            break;

        case ELF1_DVP_256_IR:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ELF1_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case NK1221A_DVP_384_IR:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_NK1221A_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
#endif

	case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2880x1620_PLANAR_420_SC530AI_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
            
        case OV_OS04A10_MIPI_4M_25FPS_12BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2560x1440_PLANAR_420_OS04A10_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OV_OS04A10_MIPI_BINNING_25FPS_10BIT:
        case OV_OS04A10_MIPI_BINNING_25FPS_12BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1280x720_PLANAR_420_OS04A10_BINNING_LINEAR, sizeof(VI_CHN_ATTR_S));			
            break;
        case GST417W_IR_400X308:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_GST417W_400X308, sizeof(VI_CHN_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            break;
        case ISP_VIN_IR_ISP0_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            break;
        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            break;            
        case ISP_VIN_IR_ISP1_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            break;
        case GST212W4_IR_256X200:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_GST212W4_256X200, sizeof(VI_CHN_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            break;
        case ISP_VIN_IR_ISP0_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            break;
        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            break;
        case ISP_VIN_IR_ISP1_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            break;
        case OV_OS05A20_2688x1944:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_OV_OS05A20_2688x1944, sizeof(VI_CHN_ATTR_S));
            break;
	    
        case H3812C1SG_IR_444X336:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_H3812C1SG_444X336, sizeof(VI_CHN_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            break;
        case ISP_VIN_IR_ISP0_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            break;
        case ISP_VIN_IR_ISP0_WITH_THERMAL_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            break;
        case ISP_VIN_IR_ISP1_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            break;
        default:
            memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
    }

    return AR_SUCCESS;
}



