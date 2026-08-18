#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "ar_combo_dev.h"

#include "ar_common.h"
#include "sample_comm.h"
#include "hal_clk.h"


#if defined(AR9341)
#define SENSOR0_RESET_GROUP    1
#define SENSOR0_RESET_PORT      2
#define SENSOR0_RESET_NUM       1
#define SENSOR0_POWER_GROUP     1
#define SENSOR0_POWER_PORT     2
#define SENSOR0_POWER_NUM      0
#define SENSOR1_RESET_GROUP    1
#define SENSOR1_RESET_PORT    2
#define SENSOR1_RESET_NUM     3
#define SENSOR1_POWER_GROUP    1
#define SENSOR1_POWER_PORT     2
#define SENSOR1_POWER_NUM      0
#define IMX307_SETTLE     0X18
#define IMX307_HDR_SETTLE     0X18
#define OS04A10_SETTLE     0x18
#define OS04A10_HDR_SETTLE    0x18
#define SC2210_SETTLE     17
#define OS05A20_SETTLE     17
#define OS05A20_MCLK_SRC CLK_SENSOR0
#define OS05A20_MCLK_SRC_KHZ 24000
#define OS05A20_MCLK_ID OS05A20_MCLK_SRC
#define OS05A20_MCLK_ID_KHZ OS05A20_MCLK_SRC_KHZ
#define IMX464_SETTLE     0X0d

#elif defined(PROXIMA)
#define SENSOR0_RESET_GROUP    0
#define SENSOR0_RESET_PORT    6
#define SENSOR0_RESET_NUM    3
#define SENSOR0_POWER_GROUP   0
#define SENSOR0_POWER_PORT    6
#define SENSOR0_POWER_NUM   15
#define SENSOR1_RESET_GROUP   0
#define SENSOR1_RESET_PORT   6
#define SENSOR1_RESET_NUM    10
#define SENSOR1_POWER_GROUP   0
#define SENSOR1_POWER_PORT   6
#define SENSOR1_POWER_NUM   14
#define IMX307_SETTLE     0x04
#define IMX307_HDR_SETTLE     0x04
#define OS04A10_SETTLE 0x03
#define OS04A10_HDR_SETTLE    0x03
#define SC2210_SETTLE     6
#define OS05A20_SETTLE     4
#define OS05A20_MCLK_SRC   CGU_OSCIN_CLK
#define OS05A20_MCLK_SRC_KHZ 24000
#define OS05A20_MCLK_ID CGU_SENSOR_MCLK2
#define OS05A20_MCLK_ID_KHZ 24000
#define IMX464_SETTLE     0x04

#else
#define SENSOR0_RESET_GROUP    1
#define SENSOR0_RESET_PORT    2
#define SENSOR0_RESET_NUM     1
#define SENSOR0_POWER_GROUP     1
#define SENSOR0_POWER_PORT     2
#define SENSOR0_POWER_NUM    0
#define SENSOR1_RESET_GROUP    1
#define SENSOR1_RESET_PORT    2
#define SENSOR1_RESET_NUM     3
#define SENSOR1_POWER_GROUP     1
#define SENSOR1_POWER_PORT     2
#define SENSOR1_POWER_NUM    0
#define IMX307_SETTLE     0X18
#define IMX307_HDR_SETTLE     0X18
#define OS04A10_SETTLE     0x18
#define OS04A10_HDR_SETTLE    0x18
#define SC2210_SETTLE     17
#define OS05A20_SETTLE     17
#define OS05A20_MCLK_SRC CLK_SENSOR0
#define OS05A20_MCLK_SRC_KHZ 24000
#define OS05A20_MCLK_ID OS05A20_MCLK_SRC
#define OS05A20_MCLK_ID_KHZ OS05A20_MCLK_SRC_KHZ
#define IMX464_SETTLE     0X0d
#endif


#define MAX_FRAME_WIDTH     8192

SAMPLE_VI_DUMP_THREAD_INFO_S g_stViDumpRawThreadInfo;


STRU_COMBO_DEV_ATTR_T COMBO_DEV_ATTR_SC_SC2210_1080P =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 446,
    .img_rect = {0, 0, 1920, 1080},
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {4, 5, -1, -1, -1, -1, -1, -1},
            100,
            SC2210_SETTLE
        }
    },
	{
        {SENSOR0_RESET_GROUP, SENSOR0_RESET_PORT, SENSOR0_RESET_NUM}, // reset
        {SENSOR0_POWER_GROUP, SENSOR0_POWER_PORT, SENSOR0_POWER_NUM}, // power
        {-1, -1, -1}, //common
        /*
          #define CLK_SENSOR0		64
          #define CLK_SENSOR1		65
          #define CLK_SENSOR2		66
          #define CLK_SENSOR3		67
        */
        65,
        27000,
    },
};
VI_DEV_ATTR_S DEV_ATTR_SC_SC2210_1080P =
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
VI_PIPE_ATTR_S PIPE_ATTR_SC_SC2210_1080P =
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
VI_CHN_ATTR_S CHN_ATTR_SC_SC2210_1080P =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0, 0,
    0,
    {-1, -1}
};


STRU_COMBO_DEV_ATTR_T COMBO_DEV_ATTR_OV_OS05A20_2688x1944 =
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
        {SENSOR0_RESET_GROUP, SENSOR0_RESET_PORT, SENSOR0_RESET_NUM}, // reset
        {SENSOR0_POWER_GROUP, SENSOR0_POWER_PORT, SENSOR0_POWER_NUM}, // power
        {-1, -1, -1}, //common
        OS05A20_MCLK_SRC,
        OS05A20_MCLK_SRC_KHZ,
        OS05A20_MCLK_ID,
        OS05A20_MCLK_ID_KHZ,
    },
};
VI_DEV_ATTR_S DEV_ATTR_OV_OS05A20_2688x1944 =
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
VI_PIPE_ATTR_S PIPE_ATTR_OV_OS05A20_2688x1944 =
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
VI_CHN_ATTR_S CHN_ATTR_OV_OS05A20_2688x1944 =
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

STRU_COMBO_DEV_ATTR_T COMBO_DEV_ATTR_OV_OS05A20_2336x1752 =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 2336, 1752},
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
        {SENSOR0_RESET_GROUP, SENSOR0_RESET_PORT, SENSOR0_RESET_NUM}, // reset
        {SENSOR0_POWER_GROUP, SENSOR0_POWER_PORT, SENSOR0_POWER_NUM}, // power
        {-1, -1, -1}, //common
        OS05A20_MCLK_SRC,
        OS05A20_MCLK_SRC_KHZ,
        OS05A20_MCLK_ID,
        OS05A20_MCLK_ID_KHZ,
    },
};
VI_DEV_ATTR_S DEV_ATTR_OV_OS05A20_2336x1752 =
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
    {2336, 1752},
    {
        {
            {2336, 1752},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1752
    },
    DATA_RATE_X1
};
VI_PIPE_ATTR_S PIPE_ATTR_OV_OS05A20_2336x1752 =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    2336, 1752,
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
VI_CHN_ATTR_S CHN_ATTR_OV_OS05A20_2336x1752 =
{
    {2336, 1752},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0, 0,
    0,
    {-1, -1}
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
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
            100,
            IMX307_SETTLE,
        }
    },
	{
#ifdef USE_EVB_EXT
            {2,3,0},//reset
            {3,3,5},//power
#else
	    	{SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
	    	{SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
#endif
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

	//vif line buffer timing
	{
		0,
		153751,
		11852,
		1478

	},
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 460,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_2M_NOWDR_BIG_PIC_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, 1280, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_2M_WDR2TO1_LINE_BIG_PIC_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, 1184, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_48M_NOWDR_BIG_PIC_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MEM,
    .data_rate = 450,
    .img_rect = {0, 0, 4064, 6048},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN1_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 450,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1, -1, -1, -1, -1},
            100,
            IMX307_SETTLE
        }
    },
	{
	    	{SENSOR1_RESET_GROUP,SENSOR1_RESET_PORT,SENSOR1_RESET_NUM},//reset
	    	{SENSOR1_POWER_GROUP,SENSOR1_POWER_PORT,SENSOR1_POWER_NUM},//power
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_mipi2_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 2,
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
            IMX307_SETTLE
        }
    },
	{
	    	{1,3,1},//reset
	    	{1,3,0},//power
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_mipi3_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 3,
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
            IMX307_SETTLE
        }
    },
	{
	    	{1,3,3},//reset
	    	{1,3,0},//power
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


STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR =
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
            100,
			IMX307_HDR_SETTLE
        }
    },
    {
	    	{SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
	    	{SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_BIG_PIC_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 891,
    .img_rect = {0, 0, 1952, 2678},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_DOL,
            {0, 1, -1, -1, -1, -1, -1, -1},
            300,
            IMX307_HDR_SETTLE
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN1_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR =
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
            100,
            IMX307_HDR_SETTLE
        }
    },
    {
	    	{SENSOR1_RESET_GROUP,SENSOR1_RESET_PORT,SENSOR1_RESET_NUM},//reset
	    	{SENSOR1_POWER_GROUP,SENSOR1_POWER_PORT,SENSOR1_POWER_NUM},//power
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
        {1,3,1},//reset
    	{1,3,0},//power
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

static STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_WDR_VC_RAW_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1188,
    .img_rect = {0, 0, 2712, 1538},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            300,
            IMX464_SETTLE
        }
    },
    {
        {SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
        {SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
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

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN0_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR =
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
            IMX307_SETTLE
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN1_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR =
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
            {4, 5, -1, -1, -1, -1, -1, -1},
            75,
            IMX307_SETTLE
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN2_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR =
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
            {5, 7, -1, -1, -1, -1, -1, -1},
            75,
            IMX307_SETTLE
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN3_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR =
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
            {8, 10, -1, -1, -1, -1, -1, -1},
            75,
            IMX307_SETTLE
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN4_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR =
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
            {9, 11, -1, -1, -1, -1, -1, -1},
            75,
            IMX307_SETTLE
        }
    }
};


STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX377_10BIT_8M_NOWDR_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_NVP6324_2M_SP420_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1},
            75,
            0xd
        }
    }
};

STRU_COMBO_DEV_ATTR_T DVP_TP9930_BT656_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT656,
    .img_rect = {0, 0, 1920, 1080},
};

STRU_COMBO_DEV_ATTR_T DVP_TP9930_BT1120_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT1120,
    .img_rect = {0, 0, 1920, 1080},
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_NOWDR_ATTR =
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
#ifdef USE_EVB_EXT
        {2,3,0},//reset
        {3,3,5},//power
#else
    	{1,2,1},//reset
    	{1,2,0},//power
#endif
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1782,
    .img_rect = {0, 0, 2688, 1520},
    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1},
            200,
            0x0c
        }
    },
	{
#ifdef USE_EVB_EXT
        {2,3,0},//reset
        {3,3,5},//power
#else
		{SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
		{SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
#endif
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



STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_60FPS_NOWDR_ATTR =
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
#ifdef USE_EVB_EXT
        {2,3,0},//reset
        {3,3,5},//power
#else
    	{1,2,1},//reset
    	{1,2,0},//power
#endif
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

STRU_COMBO_DEV_ATTR_T MIPI_2lane_COMBO_SENSOR_GC2053_10BIT_1920_1080_ATTR =
{
	.devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 564,
    .img_rect = {0, 0, 1920, 1080},
    {
        .mipi_attr = {
            .input_data_type = DATA_TYPE_RAW_10BIT,
            .wdr_mode = AR_MIPI_WDR_MODE_NONE,
            .lane_id = {0, 1, -1, -1, -1, -1, -1, -1},
            .mipi_pix_clk = 200,
            .settle_count = 0x18,
        }
    },
    .dev_power_attr = {
		.reset_gpio = {SENSOR1_RESET_GROUP,SENSOR1_RESET_PORT,SENSOR1_RESET_NUM},//reset
	    .power_gpio = {SENSOR1_POWER_GROUP,SENSOR1_POWER_PORT,SENSOR1_POWER_NUM},//power
        .common_gpio = {-1, -1, -1},
        .mclk_src = 64,
        .mclk_k = 27000,
	},

};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN0_SENSOR_GC2093_10BIT_1080P_NOWDR_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 792,
    .img_rect = {0, 0, 1920, 1080},
    {
        .mipi_attr = {
            .input_data_type = DATA_TYPE_RAW_10BIT,
            .wdr_mode = AR_MIPI_WDR_MODE_NONE,
            .lane_id = {0, 1, -1, -1, -1, -1, -1, -1},
            .mipi_pix_clk = 198,
            .settle_count = 0x18,
        }
    },
    .dev_power_attr = {
        .reset_gpio = {1, 2, 1},
        .power_gpio = {1, 2, 0},
        .common_gpio = {-1, -1, -1},
        .mclk_src = 64,
        .mclk_k = 24000,
	},
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN0_SENSOR_GC2093_10BIT_1080P_WDR2To1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 792,
    .img_rect = {0, 0, 1922, 1080},

    {
        .mipi_attr = {
            .input_data_type = DATA_TYPE_RAW_10BIT,
            .wdr_mode = AR_MIPI_WDR_MODE_VC,
            .lane_id = {0, 1, -1, -1, -1, -1, -1, -1},
#if !defined(PROXIMA)

            .mipi_pix_clk = 198,
            .settle_count = 0x18,
#else
			.mipi_pix_clk = 100,
            .settle_count = 0x8,
#endif
        }
    },
    .dev_power_attr = {
        .reset_gpio = {1, 2, 1},
        .power_gpio = {1, 2, 0},
        .common_gpio = {-1, -1, -1},
        .mclk_src = 64,
        .mclk_k = 24000,
	},
};

STRU_COMBO_DEV_ATTR_T MIPI_2lane_CHN0_SENSOR_IMX415_12BIT_VC_WDR2To1_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 594,
    .img_rect = {0, 0, 2688, 1520},

    {
        .mipi_attr = {
            .input_data_type = DATA_TYPE_RAW_12BIT,
            .wdr_mode = AR_MIPI_WDR_MODE_VC,
            .lane_id = {0, 1, -1, -1, -1, -1, -1, -1},
#if !defined(PROXIMA)

            .mipi_pix_clk = 198,
            .settle_count = 0x18,
#else
			.mipi_pix_clk = 100,
            .settle_count = 0x8,
#endif
        }
    },
    .dev_power_attr = {
        .reset_gpio = {SENSOR0_RESET_GROUP, SENSOR0_RESET_PORT, SENSOR0_RESET_NUM}, // reset
        .power_gpio = {SENSOR0_POWER_GROUP, SENSOR0_POWER_PORT, SENSOR0_POWER_NUM}, // power
        .common_gpio = {-1, -1, -1},
        .mclk_src = 64,
        .mclk_k = 24000,
	},
};

STRU_COMBO_DEV_ATTR_T  MIPI_2lane_CHN0_SENSOR_PLECO_10BIT_1920_2892_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1400,
    .img_rect = {0, 0, 1920, 2892},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1, -1, -1, -1, -1},
            200,
            0x20,
			{0},
			{
				0,
				8,
				8,
				100
			}
        }
    },
	{
	    {1,3,1},//reset
	    {1,3,0},//power
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
	{
        0,
        0x2e0a,
        0x07e7,
        0x0060
	}
};

VI_PIPE_ATTR_S PIPE_ATTR_PLECO_1920x2880_RAW10_RFR =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    1920, 2880,
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

VI_DEV_ATTR_S DEV_ATTR_IMX290_2M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_IMX290_2M_BIG_PIC_BASE =
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
    {1952, 2678},
    {
        {
            {1952 , 2678},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_2To1_LINE,
        2678
    },
    DATA_RATE_X1
};
VI_DEV_ATTR_S DEV_ATTR_ISP_VIN_0_2M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_ISP_VIN_0_2M_BIG_PIC_BASE =
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
        {0,            1280,        0,
         /*vsync0_vhb vsync0_act vsync0_hhb*/
         0,            720,        0,
         /*vsync1_vhb vsync1_act vsync1_hhb*/
         0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {1280, 1080},
    {
        {
            {1280 , 1080},

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

VI_DEV_ATTR_S DEV_ATTR_ISP_VIN_0_2M_WDR2TO1_LINE_BIG_PIC_BASE =
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
        {0,            1280,        0,
         /*vsync0_vhb vsync0_act vsync0_hhb*/
         0,            720,        0,
         /*vsync1_vhb vsync1_act vsync1_hhb*/
         0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {1184, 2678},
    {
        {
            {1184 , 2678},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2678
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_ISP_VIN_0_48M_BIG_PIC_BASE =
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
        {0,            1280,        0,
         /*vsync0_vhb vsync0_act vsync0_hhb*/
         0,            720,        0,
         /*vsync1_vhb vsync1_act vsync1_hhb*/
         0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    AR_FALSE,
    {4064, 6048},
    {
        {
            {4064 , 6048},

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

VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_WDR2TO1_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_IMX377_8M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_MIPI_SP420_2M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_TP9930_BASE =
{
    .enIntfMode = VI_MODE_BT656,
    .enWorkMode = VI_WORK_MODE_2Multiplex,
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
    .enDataRate = DATA_RATE_X2,
};

VI_DEV_ATTR_S DEV_ATTR_IMX415_8M_BASE =
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

static VI_DEV_ATTR_S DEV_ATTR_IMX464_4M_VC_RAW_BASE =
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
    {2712, 1538},
    {
        {
            {2712 , 1538},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1538
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

VI_DEV_ATTR_S DEV_ATTR_GC2093_1080P_BASE =
{
    .enIntfMode = VI_MODE_MIPI,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFFC00000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
        .enVsyncNeg = VI_VSYNC_NEG_LOW,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .stTimingBlank = {
            .u32HsyncHfb = 0,
            .u32HsyncAct = 1280,
            .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,
            .u32VsyncVact = 720,
            .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0,
            .u32VsyncVbact = 0,
            .u32VsyncVbbb = 0,
        },
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {1920, 1080},
    .stBasAttr = {
        .stSacleAttr = {
            {1920 , 1080},
        },
        .stRephaseAttr = {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE,
        }
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 1080,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_DEV_ATTR_S DEV_ATTR_GC2053_1080P_BASE =
{
    .enIntfMode = VI_MODE_MIPI,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFFC00000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
        .enVsyncNeg = VI_VSYNC_NEG_LOW,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .stTimingBlank = {
            .u32HsyncHfb = 0,
            .u32HsyncAct = 1280,
            .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,
            .u32VsyncVact = 720,
            .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0,
            .u32VsyncVbact = 0,
            .u32VsyncVbbb = 0,
        },
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {1920, 1080},
    .stBasAttr = {
        .stSacleAttr = {
            {1920 , 1080},
        },
        .stRephaseAttr = {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE,
        }
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 1080,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_DEV_ATTR_S DEV_ATTR_GC2093_1080P_WDR2To1_BASE =
{
    .enIntfMode = VI_MODE_MIPI,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFFC00000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
        .enVsyncNeg = VI_VSYNC_NEG_LOW,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .stTimingBlank = {
            .u32HsyncHfb = 0,
            .u32HsyncAct = 1280,
            .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,
            .u32VsyncVact = 720,
            .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0,
            .u32VsyncVbact = 0,
            .u32VsyncVbbb = 0,
        },
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {1922, 1080},
    .stBasAttr = {
        .stSacleAttr = {
            {1922, 1080},
        },
        .stRephaseAttr = {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE,
        }
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_2To1_LINE,
        .u32CacheLine = 1080,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_DEV_ATTR_S DEV_ATTR_IMX415_VC_WDR2To1 =
{
    .enIntfMode = VI_MODE_MIPI,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFFC00000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_YUYV,
    .stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
        .enVsyncNeg = VI_VSYNC_NEG_LOW,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,
        .stTimingBlank = {
            .u32HsyncHfb = 0,
            .u32HsyncAct = 1280,
            .u32HsyncHbb = 0,
            .u32VsyncVfb = 0,
            .u32VsyncVact = 720,
            .u32VsyncVbb = 0,
            .u32VsyncVbfb = 0,
            .u32VsyncVbact = 0,
            .u32VsyncVbbb = 0,
        },
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {2688, 1520},
    .stBasAttr = {
        .stSacleAttr = {
            {2688, 1520},
        },
        .stRephaseAttr = {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE,
        }
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_2To1_LINE,
        .u32CacheLine = 1080,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_DEV_ATTR_S DEV_ATTR_PLECO_1920_2880_BASE =
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
    {1920, 2880},
    {
        {
            {1920, 2880},

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

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420 =
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

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR =
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
        COMPRESS_MODE_DPCM_6BITS,
    },
    AR_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_PREVIEW_BIG_PIC =
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
        COMPRESS_MODE_DPCM_6BITS,
    },
    AR_FALSE,
    { -1, -1},
    0,
    0,
    1,
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_RAW_WDR2To1_BIG_PIC =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    1952, 2678,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE,
    },
    AR_FALSE,
    { -1, -1},
    0,
    0,
    1,
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR_BIG_PIC =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    1280, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_DPCM_6BITS,
    },
    AR_FALSE,
    { -1, -1},
    0,
    0,
    0,
    1,
    1,
    1,
    0,
    0,
    0,
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_WDR2TO1_LINE_BIG_PIC =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    1184, 2678,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE,
    },
    AR_FALSE,
    { -1, -1},
    0,
    0,
    0,
    1,
    1,
    1,
    0,
    0,
    0,
};

VI_PIPE_ATTR_S PIPE_ATTR_8064x6048_RAW12_420_3DNR_RFR_BIG_PIC =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_FALSE,
    4064, 6048,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    AR_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_DPCM_6BITS,
    },
    AR_FALSE,
    { -1, -1},
    0,
    0,
    0,
    1,
    1,
    1,
    0,
    0,
    0,
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_CHN0 =
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

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420 =
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

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR =
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

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR =
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

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_CHN0 =
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

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_PLANAR_420_3DNR_RFR =
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
        COMPRESS_MODE_DPCM_6BITS
    },
    AR_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_3000x3000_RAW12_420_3DNR_RFR =
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

VI_PIPE_ATTR_S PIPE_ATTR_3000x3000_RAW10_420_3DNR_RFR =
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


VI_PIPE_ATTR_S PIPE_ATTR_4000x3000_RAW12_420_3DNR_RFR =
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

VI_PIPE_ATTR_S PIPE_ATTR_4096x2160_RAW12_420_3DNR_RFR =
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

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_SP420_3DNR_RFR =
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

VI_PIPE_ATTR_S PIPE_ATTR_2688x1520_RAW12_420_3DNR_RFR =
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
    0, //vfe mode ceva hdr
};

VI_PIPE_ATTR_S PIPE_ATTR_2712x1538_RAW12_DNG =
{
    VI_PIPE_BYPASS_NONE, AR_FALSE, AR_TRUE,
    2712, 1538,
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
    0, //vfe mode ceva hdr
};



VI_PIPE_ATTR_S PIPE_ATTR_TP9930_DEFAULT =
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

VI_PIPE_ATTR_S PIPE_ATTR_GC2093_1080P_RAW10 =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 1920,
    .u32MaxH = 1080,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_10,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {-1, -1},
};

VI_PIPE_ATTR_S PIPE_ATTR_GC2053_1080P_RAW10 =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 1920,
    .u32MaxH = 1080,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_10,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {-1, -1},
};


VI_PIPE_ATTR_S PIPE_ATTR_GC2093_1080P_RAW10_WDR2To1 =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 1922,
    .u32MaxH = 1080,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_10,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .enBitWidth = DATA_BITWIDTH_8,
        .enNrRefSource = VI_NR_REF_FROM_RFR,
        .enCompressMode = COMPRESS_MODE_NONE
    },
    .bSharpenEn = AR_FALSE,
    .stFrameRate = {-1, -1},
};


VI_CHN_ATTR_S CHN_ATTR_1920x1080_422_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR_PHY_BIG_PIC =
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

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR_RAW_BIG_PIC =
{
    {1952, 2678},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR_BIG_PIC =
{
    {1024, 1080},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_WDR2TO1_LINE_BIG_PIC =
{
    {1024, 1080},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_8064x6048_420_SDR8_LINEAR_BIG_PIC =
{
    {4032, 6048},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SP_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


VI_CHN_ATTR_S CHN_ATTR_1920x1080_400_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3000x3000_422_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3000x3000_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3000x3000_400_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3840x2160_422_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3840x2160_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3840x2160_PLANAR_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_3840x2160_400_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_4000x3000_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_4096x2160_422_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_4096x2160_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_2688x1520_PLANAR_420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_2712x1538_VC_RAW =
{
    {2712, 1538},
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


VI_CHN_ATTR_S CHN_ATTR_1080P_YUV420_SDR8_LINEAR =
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

VI_CHN_ATTR_S CHN_ATTR_1920x2892_PLANAR_420_PLECO_LINEAR =
{
    {1920, 2880},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OS04A10_12BIT_4M_ATTR =
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
			OS04A10_SETTLE,
        }
    },
	{
	    	{SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
	    	{SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
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
		6050,
		8208,
		1000,
	},
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

VI_PIPE_ATTR_S PIPE_ATTR_OS04A10_2560x1440_RAW12_420_3DNR_RFR =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    2560, 1440,
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
    { -1, -1},
	0,
	0,
	0,

};

VI_CHN_ATTR_S CHN_ATTR_2560x1440_PLANAR_420_OS04A10_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OS04A10_10BIT_4M_WDR2to1_ATTR =
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
            0x18
        }
    },
	{
	    	{SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
	    	{SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
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

VI_DEV_ATTR_S DEV_ATTR_MIPI_OS04A10_4M_10BIT_BASE =
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

VI_PIPE_ATTR_S PIPE_ATTR_OS04A10_2560x1440_RAW10_420_3DNR_RFR =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, AR_FALSE,AR_FALSE,
    2562, 1440,
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_OS04A10_10BIT_4M_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 720,
    .img_rect = {0, 0, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            AR_MIPI_WDR_MODE_DOL,
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
		{SENSOR0_RESET_GROUP,SENSOR0_RESET_PORT,SENSOR0_RESET_NUM},//reset
		{SENSOR0_POWER_GROUP,SENSOR0_POWER_PORT,SENSOR0_POWER_NUM},//power
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

//iray pal interlase sensor
STRU_COMBO_DEV_ATTR_T DVP_IRAY_PAL_BT656_ATTR = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT656,
    .img_rect = {0, 0, 720, 288},
};

VI_DEV_ATTR_S DEV_ATTR_IRAY_PAL_BASE =
{
    .enIntfMode = VI_MODE_BT656,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFF000000, 0x0},
    .enScanMode = VI_SCAN_INTERLACED,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .enInputDataType = VI_DATA_TYPE_YUV,
    .bDataReverse = AR_FALSE,
    .stSize = {720, 288},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {720, 288},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 720,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_PIPE_ATTR_S PIPE_ATTR_IRAY_PAL_DEFAULT =
{
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 720,
    .u32MaxH = 288,
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

VI_CHN_ATTR_S CHN_ATTR_720x576_PLANAR_IRAY_PAL_LINEAR =
{
    {720, 288},
    PIXEL_FORMAT_YVU_PLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

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
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_14,
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

static VI_CHN_ATTR_S CHN_ATTR_GST417W_400X308 = {
    .stSize = {400, 308},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP,
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
//GST612C ATTR
//******************************************************
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_GST612C_640X520 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 640, 520},
};

static VI_DEV_ATTR_S DEV_ATTR_GST612C_640X520 = {
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
    .stSize = {640, 520},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {640, 520},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 520,
    },
    .enDataRate = DATA_RATE_X1,
    .bDvpHighLowByteSwap = AR_FALSE,
    .bIrSensor = AR_TRUE,
};

static VI_PIPE_ATTR_S PIPE_ATTR_GST612C_640X520 = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 640,
    .u32MaxH = 520,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_14,
    .bNrEn = AR_FALSE,
    .stNrAttr = {
        .enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
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
    .enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE,
};

static VI_CHN_ATTR_S CHN_ATTR_GST612C_640X520 = {
    .stSize = {640, 520},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enDynamicRange = DYNAMIC_RANGE_SDR8,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = 0,
    .bFlip = 0,
    .u32Depth = 0,
    .stFrameRate = {
        .s32SrcFrameRate = 50,
        .s32DstFrameRate = 50,
    },
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
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_14,
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
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP,
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
//RTD6122C ATTR
//******************************************************
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_RTD6122C_656X522 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 656, 522},
    .dev_power_attr = {
        .power_gpio = {0, 0, 17},       // GPIOA_17
    },
};

static VI_DEV_ATTR_S DEV_ATTR_RTD6122C_656X522 = {
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFFFF0000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .stSynCfg = {
        .enVsyncNeg = 0,
        .enHsyncNeg = 0,
        .enDE = VI_DE_PIXEL_IN,
        .enDENeg = 0,
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {656, 522},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {656, 522},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 522,
    },
    .enDataRate = DATA_RATE_X1,
    .bDvpHighLowByteSwap = AR_FALSE,
    .bIrSensor = AR_TRUE,
    .bDvpInputFromNuc = AR_TRUE,
};

static VI_PIPE_ATTR_S PIPE_ATTR_RTD6122C_656X522 = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 656,
    .u32MaxH = 522,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_14,
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

static VI_CHN_ATTR_S CHN_ATTR_RTD6122C_656X522 = {
    .stSize = {656, 522},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP,
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
//RTD3172C ATTR
//******************************************************
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_RTD3172C_394X298 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 394, 298},
    .dev_power_attr = {
        .power_gpio = {0, 0, 17},       // GPIOA_17
    },
};

static VI_DEV_ATTR_S DEV_ATTR_RTD3172C_394X298 = {
    .enIntfMode = VI_MODE_BT601,
    .enWorkMode = VI_WORK_MODE_1Multiplex,
    .au32ComponentMask = {0xFFFF0000, 0x0},
    .enScanMode = VI_SCAN_PROGRESSIVE,
    .as32AdChnId = {-1, -1, -1, -1},
    .enDataSeq = VI_DATA_SEQ_UYVY,
    .stSynCfg = {
        .enVsyncNeg = 0,
        .enHsyncNeg = 0,
        .enDE = VI_DE_PIXEL_IN,
        .enDENeg = 0,
    },
    .enInputDataType = VI_DATA_TYPE_RGB,
    .bDataReverse = AR_FALSE,
    .stSize = {394, 298},
    .stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {394, 298},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        },
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 298,
    },
    .enDataRate = DATA_RATE_X1,
    .bDvpHighLowByteSwap = AR_FALSE,
    .bIrSensor = AR_TRUE,
    .bDvpInputFromNuc = AR_TRUE,
};

static VI_PIPE_ATTR_S PIPE_ATTR_RTD3172C_394X298 = {
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .bYuvSkip = AR_FALSE,
    .bIspBypass = AR_FALSE,
    .u32MaxW = 394,
    .u32MaxH = 298,
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_14,
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

static VI_CHN_ATTR_S CHN_ATTR_RTD3172C_394X298 = {
    .stSize = {394, 298},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP,
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
//GST212W4 ATTR
//******************************************************
static STRU_COMBO_DEV_ATTR_T COMBO_ATTR_GST212W4_256X200 = {
    .devno = 0,
    .input_mode = INPUT_MODE_BT601,
    .img_rect = {0, 0, 256, 200},
    .dev_power_attr = {
        .reset_gpio = {0, 6, 0},        // GPIOG_0
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
    .enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP,
    .enCompressMode = COMPRESS_MODE_NONE,
    .enBitWidth = DATA_BITWIDTH_14,
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

static VI_CHN_ATTR_S CHN_ATTR_GST212W4_256X200 = {
    .stSize = {256, 200},
    .enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP,
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
    .enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE,
};

static VI_CHN_ATTR_S CHN_ATTR_ISP_VIN_IR_THERMAL = {
    {640, 512},
    PIXEL_FORMAT_RGB_BAYER_14BPP,
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
    .enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE,
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
    .enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE,
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

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_S5KGN2SP_10BIT_4K_30FPS_ATTR =
{
	.devno = 0,
	.input_mode = INPUT_MODE_MIPI,
	.data_rate = 1500,//1500,
	.img_rect = {0, 0, 3840, 2160},
	{
		.mipi_attr =
		{
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = AR_MIPI_WDR_MODE_NONE,
			.lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
			.mipi_pix_clk = 200,
			.settle_count = 0x10
		}
	},
	.dev_power_attr = {
		.reset_gpio = {3,3,2},//reset
		.power_gpio = {1,2,0},//power
		.common_gpio = {-1,-1,-1},//common
			/*
			 #define CLK_SENSOR0		64
			 #define CLK_SENSOR1		65
			 #define CLK_SENSOR2		66
			 #define CLK_SENSOR3		67
			   */
		.mclk_src = CLK_SENSOR0,
		.mclk_k = 24000,//24000
	},
	.line_timing = {
		.timing_mode = 1,	//0:auto;1:manual
		.de_delay = 0x66268,
		.htotal = 0xd98,
		.hstart = 0x21f
	}
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_S5KGN2SP_10BIT_4K_60FPS_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1800,
    .img_rect = {0, 0, 3840, 2160},
	{
		.mipi_attr =
		{
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = AR_MIPI_WDR_MODE_NONE,
			.lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
			.mipi_pix_clk = 300,
			.settle_count = 21
		}
	},
	.dev_power_attr = {
		.reset_gpio = {3,3,2},//reset
		.power_gpio = {1,2,0},//power
		.common_gpio = {-1,-1,-1},//common
			/*
			 #define CLK_SENSOR0		64
			 #define CLK_SENSOR1		65
			 #define CLK_SENSOR2		66
			 #define CLK_SENSOR3		67
			   */
		.mclk_src = CLK_SENSOR0,
		.mclk_k = 24000,
	},
	.line_timing = {
		.timing_mode = 1,	//0:auto;1:manual
		.de_delay =0x52005,
		.htotal = 0xafb,
		.hstart = 0x60
	}
};

STRU_COMBO_DEV_ATTR_T MIPI_4lane_CHN0_SENSOR_S5KGN2SP_10BIT_2K7_60FPS_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = 1500,
    .img_rect = {0, 0, 2704, 1520},
	{
		.mipi_attr =
		{
			.input_data_type = DATA_TYPE_RAW_10BIT,
			.wdr_mode = AR_MIPI_WDR_MODE_NONE,
			.lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
			.mipi_pix_clk = 200,
			.settle_count = 0x10
		}
	},
	.dev_power_attr = {
		.reset_gpio = {3,3,2},//reset
		.power_gpio = {1,2,0},//power
		.common_gpio = {-1,-1,-1},//common
			/*
			 #define CLK_SENSOR0		64
			 #define CLK_SENSOR1		65
			 #define CLK_SENSOR2		66
			 #define CLK_SENSOR3		67
			   */
		.mclk_src = CLK_SENSOR0,
		.mclk_k = 24000,
	},
	.line_timing = {
		.timing_mode = 1,	//0:auto;1:manual
		.de_delay = 0x5babe,
		.htotal = 0xc3f,
		.hstart = 0x187
	}
};
VI_DEV_ATTR_S DEV_ATTR_S5KGN2SP_4K_30FPS_BASE =
{
	.enIntfMode = VI_MODE_MIPI,
	.enWorkMode = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask = {0xFFC00000,    0x0},
	.enScanMode = VI_SCAN_PROGRESSIVE,
	.as32AdChnId = {-1, -1, -1, -1},
	.enDataSeq = VI_DATA_SEQ_YUYV,

	.stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
		.enVsyncNeg = VI_VSYNC_NEG_LOW,
		.enHsync = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg = VI_HSYNC_NEG_HIGH,
		.enVsyncValid = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,

        .stTimingBlank = {
        	.u32HsyncHfb = 0,
			.u32HsyncAct = 1280,
			.u32HsyncHbb = 0,
         	.u32VsyncVfb = 0,
         	.u32VsyncVact = 720,
         	.u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,
			.u32VsyncVbact = 0,
			.u32VsyncVbbb = 0,
         },
		.enDE = VI_DE_HSYNC_IN,
		.enDENeg = VI_DE_NEG_HIGH,	
    },
	.enInputDataType = VI_DATA_TYPE_RGB,
	.bDataReverse = AR_FALSE,
	.stSize = {3840, 2160},
	.stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {3840 , 2160},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        }
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 2160,
    },
    .enDataRate = DATA_RATE_X1,
};

VI_DEV_ATTR_S DEV_ATTR_S5KGN2SP_2K7_30FPS_BASE =
{
	.enIntfMode = VI_MODE_MIPI,
	.enWorkMode = VI_WORK_MODE_1Multiplex,
	.au32ComponentMask = {0xFFC00000,    0x0},
	.enScanMode = VI_SCAN_PROGRESSIVE,
	.as32AdChnId = {-1, -1, -1, -1},
	.enDataSeq = VI_DATA_SEQ_YUYV,

	.stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
		.enVsyncNeg = VI_VSYNC_NEG_LOW,
		.enHsync = VI_HSYNC_VALID_SINGNAL,
		.enHsyncNeg = VI_HSYNC_NEG_HIGH,
		.enVsyncValid = VI_VSYNC_VALID_SINGAL,
		.enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,

        .stTimingBlank = {
        	.u32HsyncHfb = 0,
			.u32HsyncAct = 1280,
			.u32HsyncHbb = 0,
         	.u32VsyncVfb = 0,
         	.u32VsyncVact = 720,
         	.u32VsyncVbb = 0,
			.u32VsyncVbfb = 0,
			.u32VsyncVbact = 0,
			.u32VsyncVbbb = 0,
         },
		.enDE = VI_DE_HSYNC_IN,
		.enDENeg = VI_DE_NEG_HIGH,	
    },
	.enInputDataType = VI_DATA_TYPE_RGB,
	.bDataReverse = AR_FALSE,
	.stSize = {2704, 1520},
	.stBasAttr = {
        .stSacleAttr = {
            .stBasSize = {2704 , 1520},
        },
        .stRephaseAttr = {
            .enHRephaseMode = VI_REPHASE_MODE_NONE,
            .enVRephaseMode = VI_REPHASE_MODE_NONE,
        }
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
        .u32CacheLine = 1520,
    },
    .enDataRate = DATA_RATE_X1,
};
VI_PIPE_ATTR_S PIPE_ATTR_S5KGN2SP_3840x2160_RAW10_420_3DNR_RFR =
{
	.enPipeBypassMode = VI_PIPE_BYPASS_NONE,
	.bYuvSkip = AR_FALSE,
	.bIspBypass = AR_FALSE,
	.u32MaxW = 3840,
	.u32MaxH = 2160,
	.enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode = COMPRESS_MODE_NONE,
	.enBitWidth = DATA_BITWIDTH_10,
	.bNrEn = AR_FALSE,
	.stNrAttr = {
	.enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth = DATA_BITWIDTH_8,
		.enNrRefSource = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn = AR_FALSE,
	.stFrameRate = { -1, -1},
	.bDiscardProPic = 0,
	.enVfeMode = 0,
	.s32Trigger = 0,
};

VI_PIPE_ATTR_S PIPE_ATTR_S5KGN2SP_2704x1520_RAW10_420_3DNR_RFR =
{
	.enPipeBypassMode = VI_PIPE_BYPASS_NONE,
	.bYuvSkip = AR_FALSE,
	.bIspBypass = AR_FALSE,
	.u32MaxW = 2704,
	.u32MaxH = 1520,
	.enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP,
	.enCompressMode = COMPRESS_MODE_NONE,
	.enBitWidth = DATA_BITWIDTH_10,
	.bNrEn = AR_FALSE,
	.stNrAttr = {
	.enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
		.enBitWidth = DATA_BITWIDTH_8,
		.enNrRefSource = VI_NR_REF_FROM_RFR,
		.enCompressMode = COMPRESS_MODE_NONE,
	},
	.bSharpenEn = AR_FALSE,
	.stFrameRate = { -1, -1},
	.bDiscardProPic = 0,
	.enVfeMode = 0,
	.s32Trigger = 0,
};
VI_CHN_ATTR_S CHN_ATTR_3840x2160_PLANAR_420_S5KGN2SP_LINEAR =
{
	.stSize = {3840, 2160},
	.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420,
	.enDynamicRange = DYNAMIC_RANGE_SDR8,
	.enVideoFormat = VIDEO_FORMAT_LINEAR,
	.enCompressMode = COMPRESS_MODE_NONE,
	.bMirror = 0,
	.bFlip = 0,
	.u32Depth = 0,
	.stFrameRate = { -1, -1}
};
	
VI_CHN_ATTR_S CHN_ATTR_2704x1520_PLANAR_420_S5KGN2SP_LINEAR =
{
	.stSize = {2704, 1520},
	.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420,
	.enDynamicRange = DYNAMIC_RANGE_SDR8,
	.enVideoFormat = VIDEO_FORMAT_LINEAR,
	.enCompressMode = COMPRESS_MODE_NONE,
	.bMirror = 0,
	.bFlip = 0,
	.u32Depth = 0,
	.stFrameRate = { -1, -1}
};

AR_BOOL IsSensorInput(SAMPLE_SNS_TYPE_E enSnsType)
{
    AR_BOOL bRet = AR_TRUE;

    switch (enSnsType)
    {
        case SAMPLE_SNS_TYPE_BUTT:
            bRet = AR_FALSE;
            break;

        default:
            break;
    }

    return bRet;
}

static input_mode_t SAMPLE_COMM_VI_GetSnsInputMode(SAMPLE_SNS_TYPE_E enSnsType)
{
    input_mode_t enInputMode;

    switch (enSnsType)
    {
        default:
            enInputMode = INPUT_MODE_MIPI;
            break;
    }

    return enInputMode;
}
AR_S32 SAMPLE_COMM_VI_GetComboAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, combo_dev_t MipiDev, STRU_COMBO_DEV_ATTR_T* pstComboAttr)
{
    switch (enSnsType)
    {
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
            if (0 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if(MipiDev==1)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN1_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }else if(MipiDev==2)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_mipi2_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));

            }else
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_mipi3_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            break;

        case ISP_VIN_0_MEM_2M_30FPS_12BIT:
			{
				ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
			}
		    break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
    {
        ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_2M_NOWDR_BIG_PIC_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
    }
    break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
    {
        ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_2M_WDR2TO1_LINE_BIG_PIC_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
    }
    break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
    {
        ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_ISPVIN0_12BIT_48M_NOWDR_BIG_PIC_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
    }
    break;
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_BIG_PIC_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
        break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (1 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN1_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }

            break;

        case SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT:
            if (0 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN0_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (1 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN1_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (2 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN2_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (3 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN3_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else if (4 == MipiDev)
            {
                ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN4_SENSOR_IMX290_SLAVE_10BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX377_10BIT_8M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_60FPS_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
			 ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN0_SENSOR_IMX415_12BIT_VC_WDR2To1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));

			break;

		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
			ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_4M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
			break;
       case NVP6324_MIPI_2M_30FPS_8BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_NVP6324_2M_SP420_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case TP9930_DVP_SINGLE_CH:
        case TP9930_DVP_MULTIPLEX_2CH:
        case TP9930_DVP_MULTIPLEX_4CH_BT656:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &DVP_TP9930_BT656_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case TP9930_DVP_MULTIPLEX_4CH:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &DVP_TP9930_BT1120_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_WDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX464_12BIT_4M_WDR_VC_RAW_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OS04A10_12BIT_4M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OS04A10_10BIT_4M_WDR2to1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_OS04A10_10BIT_4M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN0_SENSOR_GC2093_10BIT_1080P_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN0_SENSOR_GC2093_10BIT_1080P_WDR2To1_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_SC530AI_10BIT_5M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

        case IRAY_PAL_DVP_SINGLE_CH:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &DVP_IRAY_PAL_BT656_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;

		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
			ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX347_12BIT_4M_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
			break;

        case GST417W_IR_400X308:
        case GST412C_IR_400X308:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_GST417W_400X308, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 308;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 300;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_GST417W_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 308;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_GST417W:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 408;
            pstComboAttr->img_rect.height = 300;
            pstComboAttr->devno = MipiDev;
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

        case ISP_VIN1_IR_H3812C1SG:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 384;
            pstComboAttr->img_rect.height = 288;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_H3812C1SG_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 444;
            pstComboAttr->img_rect.height = 336;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_H3812C1SG:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 384;
            pstComboAttr->img_rect.height = 288;
            pstComboAttr->devno = MipiDev;
            break;

        case RTD6122C_IR_656X522:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_RTD6122C_656X522, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_RTD6122C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 656;
            pstComboAttr->img_rect.height = 522;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_RTD6122C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_RTD6122C_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 656;
            pstComboAttr->img_rect.height = 522;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_RTD6122C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->devno = MipiDev;
            break;

        case RTD3172C_IR_394X298:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_RTD3172C_394X298, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_RTD3172C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 394;
            pstComboAttr->img_rect.height = 298;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_RTD3172C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 384;
            pstComboAttr->img_rect.height = 288;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_RTD3172C_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 394;
            pstComboAttr->img_rect.height = 298;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_RTD3172C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 384;
            pstComboAttr->img_rect.height = 288;
            pstComboAttr->devno = MipiDev;
            break;

       case GST612C_IR_640X520:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_GST612C_640X520, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_GST612C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 520;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_GST612C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_GST612C_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 520;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_GST612C:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->devno = MipiDev;
            break;

        case SC_SC2210_1080P:
        {
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_DEV_ATTR_SC_SC2210_1080P, sizeof(STRU_COMBO_DEV_ATTR_T));
        }
        break;

    case OV_OS05A20_2688x1944:
    {
        ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_DEV_ATTR_OV_OS05A20_2688x1944, sizeof(STRU_COMBO_DEV_ATTR_T));
    }
    break;

    case OV_OS05A20_2336x1752:
    {
        ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &COMBO_DEV_ATTR_OV_OS05A20_2336x1752, sizeof(STRU_COMBO_DEV_ATTR_T));
    }
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

        case ISP_VIN1_IR_GST212W4:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 192;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_GST212W4_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 200;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_GST212W4:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 256;
            pstComboAttr->img_rect.height = 192;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN_IR_THERMAL_OFFLINE:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN_IR_THERMAL, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_OFFLINE:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_12BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN1_IR_OFFLINE_WITH_THERMAL:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN1_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->mipi_attr.input_data_type = DATA_TYPE_RAW_14BIT;
            pstComboAttr->devno = MipiDev;
            break;

        case ISP_VIN2_IR_OFFLINE:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T),
                &COMBO_ATTR_ISP_VIN2_IR, sizeof(STRU_COMBO_DEV_ATTR_T));
            pstComboAttr->img_rect.width = 640;
            pstComboAttr->img_rect.height = 512;
            pstComboAttr->devno = MipiDev;
            break;
    	case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
        	ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_S5KGN2SP_10BIT_4K_30FPS_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
        	break;
    	case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
        	ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_S5KGN2SP_10BIT_4K_60FPS_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
        	break;
    	case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
        	ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_S5KGN2SP_10BIT_2K7_60FPS_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
        	break;
		 case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_CHN0_SENSOR_PLECO_10BIT_1920_2892_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
            break;
		case GC_GC2053_MIPI_1080P_30FPS_10BIT:
			ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_2lane_COMBO_SENSOR_GC2053_10BIT_1920_1080_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
			break;
        default:
            SAMPLE_PRT("not support enSnsType: %d\n", enSnsType);
            ar_memcpy(pstComboAttr, sizeof(STRU_COMBO_DEV_ATTR_T), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(STRU_COMBO_DEV_ATTR_T));
    }


    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i = 0;
    AR_S32              s32ViNum = 0;
    AR_S32              s32Ret = AR_SUCCESS;
    // AR_S32              fd;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;
    STRU_COMBO_DEV_ATTR_T    stcomboDevAttr;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }
    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_GetComboAttrBySns(pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.MipiDev, &stcomboDevAttr);
        stcomboDevAttr.devno = pstViInfo->stSnsInfo.MipiDev;

		if(pstViInfo->stSnsInfo.mipi_ipi_fre>0){
		   stcomboDevAttr.mipi_attr.mipi_pix_clk=pstViInfo->stSnsInfo.mipi_ipi_fre/1000/1000;
		}

		if(pstViInfo->stSnsInfo.settle >0 )
		{
		    stcomboDevAttr.mipi_attr.settle_count=pstViInfo->stSnsInfo.settle;
		}


        if(VI_PARALLEL_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode || VI_PARALLEL_VPSS_PARALLEL == pstViInfo->stPipeInfo.enMastPipeMode)
        {
            //stcomboDevAttr.data_rate = MIPI_DATA_RATE_X2;
        }

        SAMPLE_PRT("MipiDev %d, SetMipiAttr enWDRMode: %d\n", pstViInfo->stSnsInfo.MipiDev, pstViInfo->stDevInfo.enWDRMode);

        s32Ret=AR_MPI_VI_SetComboDevAttr(&stcomboDevAttr);

        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("MIPI_SET_DEV_ATTR failed\n");
            goto EXIT;
        }
    }

EXIT:
    return s32Ret;
}

/*****************************************************************************
* function : init mipi
*****************************************************************************/
AR_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_S32              s32ViNum;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (int i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		s32Ret=AR_MPI_VI_SetMipiBindDev(pstViInfo->stDevInfo.ViDev, pstViInfo->stSnsInfo.MipiDev);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
			return AR_FAILURE;
        }
    }

    s32Ret = SAMPLE_COMM_VI_SetMipiAttr(pstViConfig);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetMipiAttr failed!\n");
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_StopMIPI(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    // AR_S32 s32Ret = AR_SUCCESS;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_SetParam(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i, j;
    AR_S32              s32ViNum;
    AR_S32              s32Ret;
    VI_PIPE             ViPipe;
    VI_VPSS_MODE_S      stVIVPSSMode;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_SYS_GetVIVPSSMode(&stVIVPSSMode);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get VI-VPSS mode Param failed with %#x!\n", s32Ret);

        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];
        ViPipe    = pstViInfo->stPipeInfo.aPipe[0];

        if (VI_OFFLINE_VPSS_ONLINE == pstViInfo->stPipeInfo.enMastPipeMode)
        {
            for (j = 0; j < VI_MAX_PIPE_NUM; j++)
            {
                stVIVPSSMode.aenMode[j] = VI_OFFLINE_VPSS_ONLINE;
            }
        }

        stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
        if (pstViInfo->stPipeInfo.bMultiPipe == AR_TRUE)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[1];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
            }

            ViPipe = pstViInfo->stPipeInfo.aPipe[2];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
            }

            ViPipe = pstViInfo->stPipeInfo.aPipe[3];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
            }
        }

        if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe))
        {
            ViPipe    = pstViInfo->stPipeInfo.aPipe[1];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stSnapInfo.enSnapPipeMode;
            }
        }
    }
    s32Ret = AR_MPI_SYS_SetVIVPSSMode(&stVIVPSSMode);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set VI-VPSS mode Param failed with %#x!\n", s32Ret);

        return AR_FAILURE;
    }

    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_VI_GetDevAttrBySns(VI_DEV ViDev, SAMPLE_SNS_TYPE_E enSnsType, AR_BOOL bDvpByteSwap, VI_DEV_ATTR_S* pstViDevAttr)
{
    switch (enSnsType)
    {
        case ISP_VIN_0_MEM_2M_30FPS_12BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_ISP_VIN_0_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
        ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_ISP_VIN_0_2M_BIG_PIC_BASE, sizeof(VI_DEV_ATTR_S));
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
        ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_ISP_VIN_0_2M_WDR2TO1_LINE_BIG_PIC_BASE, sizeof(VI_DEV_ATTR_S));
        break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_ISP_VIN_0_48M_BIG_PIC_BASE, sizeof(VI_DEV_ATTR_S));
        break;
		case SONY_IMX290_MIPI_2M_30FPS_12BIT:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
		case SONY_IMX307_MIPI_2M_25FPS_12BIT:
		case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
		case SONY_IMX290_MIPI_2M_60FPS_12BIT:
			ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
			break;

    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BIG_PIC_BASE, sizeof(VI_DEV_ATTR_S));
        pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
        break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX334_8M_WDR2TO1_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX377_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX415_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case NVP6324_MIPI_2M_30FPS_8BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MIPI_SP420_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX464_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX464_4M_VC_RAW_BASE, sizeof(VI_DEV_ATTR_S));
            break;
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
			 ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX415_VC_WDR2To1, sizeof(VI_DEV_ATTR_S));
			break;
        case TP9930_DVP_SINGLE_CH:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_TP9930_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->enIntfMode = VI_MODE_BT656;
            pstViDevAttr->enDataRate = DATA_RATE_X1;
            pstViDevAttr->enWorkMode = VI_WORK_MODE_1Multiplex;
            pstViDevAttr->au32ComponentMask[0] = 0xFF000000;
            pstViDevAttr->au32ComponentMask[1] = 0x00000000;
            break;

        case TP9930_DVP_MULTIPLEX_2CH:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_TP9930_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->enIntfMode = VI_MODE_BT656;
            pstViDevAttr->enDataRate = DATA_RATE_X2;
            pstViDevAttr->enWorkMode = VI_WORK_MODE_2Multiplex;
            pstViDevAttr->au32ComponentMask[0] = 0xFF000000;
            pstViDevAttr->au32ComponentMask[1] = 0x00000000;
            break;

        case TP9930_DVP_MULTIPLEX_4CH:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_TP9930_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->enIntfMode = VI_MODE_BT1120_STANDARD;
            pstViDevAttr->enDataRate = DATA_RATE_X2;
            pstViDevAttr->enWorkMode = VI_WORK_MODE_4Multiplex;
            pstViDevAttr->au32ComponentMask[0] = 0xFF000000;
            pstViDevAttr->au32ComponentMask[1] = 0x00FF0000;
            pstViDevAttr->bDvpHighLowByteSwap = bDvpByteSwap;
            if (bDvpByteSwap) {
                pstViDevAttr->enDataSeq = VI_DATA_SEQ_YUYV;
            }
            break;

        case TP9930_DVP_MULTIPLEX_4CH_BT656:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_TP9930_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->enIntfMode = VI_MODE_BT656;
            pstViDevAttr->enDataRate = DATA_RATE_X2;
            pstViDevAttr->enWorkMode = VI_WORK_MODE_4Multiplex;
            pstViDevAttr->au32ComponentMask[0] = 0xFF000000;
            pstViDevAttr->au32ComponentMask[1] = 0x00000000;
            pstViDevAttr->bDvpHighLowByteSwap = bDvpByteSwap;
            break;

        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MIPI_OS04A10_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MIPI_OS04A10_4M_10BIT_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_GC2093_1080P_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_GC2093_1080P_WDR2To1_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC530AI_5M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case IRAY_PAL_DVP_SINGLE_CH:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IRAY_PAL_BASE, sizeof(VI_DEV_ATTR_S));
            break;

		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
			ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX347_4M_BASE, sizeof(VI_DEV_ATTR_S));
			break;

        case GST417W_IR_400X308:
        case GST412C_IR_400X308:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_GST417W_400X308, sizeof(VI_DEV_ATTR_S));
            break;

        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
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

        case ISP_VIN1_IR_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 408;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 300;
            pstViDevAttr->stSize.u32Width = 408;
            pstViDevAttr->stSize.u32Height = 300;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 408;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 300;
            break;

        case ISP_VIN1_IR_GST417W_WITH_THERMAL:
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

        case ISP_VIN2_IR_GST417W:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 408;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 300;
            pstViDevAttr->stSize.u32Width = 408;
            pstViDevAttr->stSize.u32Height = 300;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 408;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 300;
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

        case ISP_VIN1_IR_H3812C1SG:
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

        case ISP_VIN1_IR_H3812C1SG_WITH_THERMAL:
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

        case ISP_VIN2_IR_H3812C1SG:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 384;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 288;
            pstViDevAttr->stSize.u32Width = 384;
            pstViDevAttr->stSize.u32Height = 288;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 384;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 288;
            break;

        case RTD6122C_IR_656X522:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_RTD6122C_656X522, sizeof(VI_DEV_ATTR_S));
            break;

        case ISP_VIN_IR_THERMAL_RTD6122C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 656;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 522;
            pstViDevAttr->stSize.u32Width = 656;
            pstViDevAttr->stSize.u32Height = 522;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 656;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 522;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN1_IR_RTD6122C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            break;

        case ISP_VIN1_IR_RTD6122C_WITH_THERMAL:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 656;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 522;
            pstViDevAttr->stSize.u32Width = 656;
            pstViDevAttr->stSize.u32Height = 522;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 656;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 522;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN2_IR_RTD6122C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            break;

        case RTD3172C_IR_394X298:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_RTD3172C_394X298, sizeof(VI_DEV_ATTR_S));
            break;

        case ISP_VIN_IR_THERMAL_RTD3172C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 394;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 298;
            pstViDevAttr->stSize.u32Width = 394;
            pstViDevAttr->stSize.u32Height = 298;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 394;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 298;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN1_IR_RTD3172C:
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

        case ISP_VIN1_IR_RTD3172C_WITH_THERMAL:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 394;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 298;
            pstViDevAttr->stSize.u32Width = 394;
            pstViDevAttr->stSize.u32Height = 298;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 394;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 298;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN2_IR_RTD3172C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 384;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 288;
            pstViDevAttr->stSize.u32Width = 384;
            pstViDevAttr->stSize.u32Height = 288;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 384;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 288;
            break;

        case GST612C_IR_640X520:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_GST612C_640X520, sizeof(VI_DEV_ATTR_S));
            break;

        case ISP_VIN_IR_THERMAL_GST612C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 520;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 520;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 520;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN1_IR_GST612C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            break;

        case ISP_VIN1_IR_GST612C_WITH_THERMAL:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 520;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 520;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 520;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN2_IR_GST612C:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            break;

        case SC_SC2210_1080P:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC_SC2210_1080P, sizeof(VI_DEV_ATTR_S));
            break;

    case OV_OS05A20_2688x1944:
        ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OV_OS05A20_2688x1944, sizeof(VI_DEV_ATTR_S));
        break;

    case OV_OS05A20_2336x1752:
        ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OV_OS05A20_2336x1752, sizeof(VI_DEV_ATTR_S));
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

        case ISP_VIN1_IR_GST212W4:
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

        case ISP_VIN1_IR_GST212W4_WITH_THERMAL:
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

        case ISP_VIN2_IR_GST212W4:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 256;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 192;
            pstViDevAttr->stSize.u32Width = 256;
            pstViDevAttr->stSize.u32Height = 192;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 256;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 192;
            break;

        case ISP_VIN_IR_THERMAL_OFFLINE:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN1_IR_OFFLINE:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfff00000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            break;

        case ISP_VIN1_IR_OFFLINE_WITH_THERMAL:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN1_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xfffc0000;
            pstViDevAttr->au32ComponentMask[1] = 0;
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            pstViDevAttr->bIrSensor = AR_TRUE;
            break;

        case ISP_VIN2_IR_OFFLINE:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S),
                &DEV_ATTR_ISP_VIN2_IR, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->stSynCfg.stTimingBlank.u32HsyncAct = 640;
            pstViDevAttr->stSynCfg.stTimingBlank.u32VsyncVact = 512;
            pstViDevAttr->stSize.u32Width = 640;
            pstViDevAttr->stSize.u32Height = 512;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Width = 640;
            pstViDevAttr->stBasAttr.stSacleAttr.stBasSize.u32Height = 512;
            break;

		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_PLECO_1920_2880_BASE, sizeof(VI_DEV_ATTR_S));
			break;
		case GC_GC2053_MIPI_1080P_30FPS_10BIT:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_GC2053_1080P_BASE, sizeof(VI_DEV_ATTR_S));
            break;
    	case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
        	ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_S5KGN2SP_4K_30FPS_BASE, sizeof(VI_DEV_ATTR_S));
        	break;
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
        	ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_S5KGN2SP_2K7_30FPS_BASE, sizeof(VI_DEV_ATTR_S));
        	break;
        default:
            ar_memcpy(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
    }

    return AR_SUCCESS;
}




AR_S32 SAMPLE_COMM_VI_GetPipeAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, AR_U32 u32Width,
    AR_U32 u32Height, AR_FLOAT f32Fps, AR_U32 u32bitwidth, AR_BOOL trigger, VI_PIPE_ATTR_S* pstPipeAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enCompressMode = COMPRESS_MODE_NONE;

			if(u32bitwidth>0)
			{
			   pstPipeAttr->enBitWidth =  u32bitwidth;
			}

			if (u32Width > 0 && u32Height > 0)
			{
				pstPipeAttr->u32MaxW = u32Width;
            	pstPipeAttr->u32MaxH = u32Height;
			}
			pstPipeAttr->s32Trigger = trigger;
            break;
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
			ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
			break;
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_RAW_WDR2To1_BIG_PIC, sizeof(VI_PIPE_ATTR_S));
        pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
        pstPipeAttr->enCompressMode = COMPRESS_MODE_NONE;
        break;
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_PREVIEW_BIG_PIC, sizeof(VI_PIPE_ATTR_S));
        pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
        pstPipeAttr->enCompressMode = COMPRESS_MODE_NONE;
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR_BIG_PIC, sizeof(VI_PIPE_ATTR_S));
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_WDR2TO1_LINE_BIG_PIC, sizeof(VI_PIPE_ATTR_S));
        break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_8064x6048_RAW12_420_3DNR_RFR_BIG_PIC, sizeof(VI_PIPE_ATTR_S));
        break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            pstPipeAttr->enCompressMode = COMPRESS_MODE_NONE;
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_PLANAR_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case NVP6324_MIPI_2M_30FPS_8BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_SP420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2688x1520_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
			ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2712x1538_RAW12_DNG, sizeof(VI_PIPE_ATTR_S));
			pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
            break;
        case TP9930_DVP_SINGLE_CH:
        case TP9930_DVP_MULTIPLEX_2CH:
        case TP9930_DVP_MULTIPLEX_4CH:
        case TP9930_DVP_MULTIPLEX_4CH_BT656:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_TP9930_DEFAULT, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
            pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            break;

        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OS04A10_2560x1440_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
			pstPipeAttr->s32Trigger = trigger;
            break;

        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OS04A10_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_GC2093_1080P_RAW10, sizeof(VI_PIPE_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_GC2093_1080P_RAW10_WDR2To1, sizeof(VI_PIPE_ATTR_S));
            break;

        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_SC530AI_2880x1620_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case IRAY_PAL_DVP_SINGLE_CH:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_IRAY_PAL_DEFAULT, sizeof(VI_PIPE_ATTR_S));
            break;

        case GST417W_IR_400X308:
        case GST412C_IR_400X308:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_GST417W_400X308, sizeof(VI_PIPE_ATTR_S));
            if(GST412C_IR_400X308 == enSnsType)
            {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = 50;
                pstPipeAttr->stFrameRate.s32DstFrameRate = 50;
            }
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 308;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 300;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST417W_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 308;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_GST417W:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 408;
            pstPipeAttr->u32MaxH = 300;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case H3812C1SG_IR_444X336:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_H3812C1SG_444X336, sizeof(VI_PIPE_ATTR_S));
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 444;
            pstPipeAttr->u32MaxH = 336;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 384;
            pstPipeAttr->u32MaxH = 288;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_H3812C1SG_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 444;
            pstPipeAttr->u32MaxH = 336;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_H3812C1SG:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 384;
            pstPipeAttr->u32MaxH = 288;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case RTD6122C_IR_656X522:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_RTD6122C_656X522, sizeof(VI_PIPE_ATTR_S));
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_RTD6122C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 656;
            pstPipeAttr->u32MaxH = 522;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD6122C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 640;
            pstPipeAttr->u32MaxH = 512;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD6122C_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 656;
            pstPipeAttr->u32MaxH = 522;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_RTD6122C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 640;
            pstPipeAttr->u32MaxH = 512;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case RTD3172C_IR_394X298:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_RTD3172C_394X298, sizeof(VI_PIPE_ATTR_S));
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_RTD3172C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 394;
            pstPipeAttr->u32MaxH = 298;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD3172C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 384;
            pstPipeAttr->u32MaxH = 288;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD3172C_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 394;
            pstPipeAttr->u32MaxH = 298;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_RTD3172C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 384;
            pstPipeAttr->u32MaxH = 288;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case GST612C_IR_640X520:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_GST612C_640X520, sizeof(VI_PIPE_ATTR_S));
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_GST612C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 640;
            pstPipeAttr->u32MaxH = 520;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST612C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 640;
            pstPipeAttr->u32MaxH = 512;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST612C_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 640;
            pstPipeAttr->u32MaxH = 520;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_GST612C:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 640;
            pstPipeAttr->u32MaxH = 512;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case SC_SC2210_1080P:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_SC_SC2210_1080P, sizeof(VI_PIPE_ATTR_S));
            break;

    case OV_OS05A20_2688x1944:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OV_OS05A20_2688x1944, sizeof(VI_PIPE_ATTR_S));
        break;

    case OV_OS05A20_2336x1752:
        ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_OV_OS05A20_2336x1752, sizeof(VI_PIPE_ATTR_S));
        break;

        case GST212W4_IR_256X200:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_GST212W4_256X200, sizeof(VI_PIPE_ATTR_S));
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 200;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 192;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
	    break;

        case ISP_VIN1_IR_GST212W4_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 200;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_GST212W4:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = 256;
            pstPipeAttr->u32MaxH = 192;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_OFFLINE:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_OFFLINE:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_12;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_OFFLINE_WITH_THERMAL:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN1_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_14BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_14;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_OFFLINE:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),
                &PIPE_ATTR_ISP_VIN2_IR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
            if (f32Fps > 0) {
                pstPipeAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
                pstPipeAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            }
            break;
    	case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
        	ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_S5KGN2SP_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
        	break;
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
        	ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_S5KGN2SP_2704x1520_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
        	break;
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
			ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_PLECO_1920x2880_RAW10_RFR, sizeof(VI_PIPE_ATTR_S));
			pstPipeAttr->u32MaxW = u32Width;
            pstPipeAttr->u32MaxH = u32Height;
			pstPipeAttr->enBitWidth =  u32bitwidth;
			break;
		case GC_GC2053_MIPI_1080P_30FPS_10BIT:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_GC2053_1080P_RAW10, sizeof(VI_PIPE_ATTR_S));
            break;
        default:
            ar_memcpy(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, AR_U32 u32Width,
    AR_U32 u32Height, AR_FLOAT f32Fps, VI_CHN_ATTR_S* pstChnAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
			if (u32Width > 0 && u32Height > 0)
			{
				pstChnAttr->stSize.u32Width = u32Width;
            	pstChnAttr->stSize.u32Height = u32Height;
			}
            break;
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR_PHY_BIG_PIC, sizeof(VI_CHN_ATTR_S));
        break;
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
             ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SP_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR_BIG_PIC, sizeof(VI_CHN_ATTR_S));
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_WDR2TO1_LINE_BIG_PIC, sizeof(VI_CHN_ATTR_S));
        break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_8064x6048_420_SDR8_LINEAR_BIG_PIC, sizeof(VI_CHN_ATTR_S));
        break;
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR_RAW_BIG_PIC, sizeof(VI_CHN_ATTR_S));
        break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_PLANAR_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2688x1520_PLANAR_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2712x1538_VC_RAW, sizeof(VI_CHN_ATTR_S));
			pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            break;
        case NVP6324_MIPI_2M_30FPS_8BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case TP9930_DVP_SINGLE_CH:
        case TP9930_DVP_MULTIPLEX_2CH:
        case TP9930_DVP_MULTIPLEX_4CH:
        case TP9930_DVP_MULTIPLEX_4CH_BT656:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_1080P_YUV420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
            pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            break;

        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2560x1440_PLANAR_420_OS04A10_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2880x1620_PLANAR_420_SC530AI_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case IRAY_PAL_DVP_SINGLE_CH:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_720x576_PLANAR_IRAY_PAL_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case GST417W_IR_400X308:
		case GST412C_IR_400X308:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_GST417W_400X308, sizeof(VI_CHN_ATTR_S));
            if(GST412C_IR_400X308 == enSnsType)
            {
                pstChnAttr->stFrameRate.s32DstFrameRate = 50;
                pstChnAttr->stFrameRate.s32SrcFrameRate = 50;
            }
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST417W_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_GST417W:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 408;
            pstChnAttr->stSize.u32Height = 300;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case H3812C1SG_IR_444X336:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_H3812C1SG_444X336, sizeof(VI_CHN_ATTR_S));
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_H3812C1SG_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_H3812C1SG:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case RTD6122C_IR_656X522:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_RTD6122C_656X522, sizeof(VI_CHN_ATTR_S));
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_RTD6122C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD6122C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD6122C_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_RTD6122C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case RTD3172C_IR_394X298:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_RTD3172C_394X298, sizeof(VI_CHN_ATTR_S));
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_RTD3172C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD3172C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_RTD3172C_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_RTD3172C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 384;
            pstChnAttr->stSize.u32Height = 288;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case GST612C_IR_640X520:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                    &CHN_ATTR_GST612C_640X520, sizeof(VI_CHN_ATTR_S));
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_GST612C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST612C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST612C_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_GST612C:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 640;
            pstChnAttr->stSize.u32Height = 512;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case SC_SC2210_1080P:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_SC_SC2210_1080P, sizeof(VI_CHN_ATTR_S));
            break;

    case OV_OS05A20_2688x1944:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_OV_OS05A20_2688x1944, sizeof(VI_CHN_ATTR_S));
        break;

    case OV_OS05A20_2336x1752:
        ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_OV_OS05A20_2336x1752, sizeof(VI_CHN_ATTR_S));
        break;

        case GST212W4_IR_256X200:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_GST212W4_256X200, sizeof(VI_CHN_ATTR_S));
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_GST212W4_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_GST212W4:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = 256;
            pstChnAttr->stSize.u32Height = 192;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN_IR_THERMAL_OFFLINE:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN_IR_THERMAL, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_OFFLINE:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN1_IR_OFFLINE_WITH_THERMAL:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN1_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;

        case ISP_VIN2_IR_OFFLINE:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S),
                &CHN_ATTR_ISP_VIN2_IR, sizeof(VI_CHN_ATTR_S));
            pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            if (f32Fps > 0) {
                pstChnAttr->stFrameRate.s32DstFrameRate = (AR_S32)f32Fps;
                pstChnAttr->stFrameRate.s32SrcFrameRate = (AR_S32)f32Fps;
            }
            break;
    	case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
        	ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_PLANAR_420_S5KGN2SP_LINEAR, sizeof(VI_CHN_ATTR_S));
        	break;
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
        	ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2704x1520_PLANAR_420_S5KGN2SP_LINEAR, sizeof(VI_CHN_ATTR_S));
        	break;
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x2892_PLANAR_420_PLECO_LINEAR, sizeof(VI_CHN_ATTR_S));
			pstChnAttr->stSize.u32Width = u32Width;
            pstChnAttr->stSize.u32Height = u32Height;
            break;
		case GC_GC2053_MIPI_1080P_30FPS_10BIT:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        default:
            ar_memcpy(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32              s32Ret;
    VI_DEV              ViDev;
    SAMPLE_SNS_TYPE_E    enSnsType;
    VI_DEV_ATTR_S       stViDevAttr;

    ViDev       = pstViInfo->stDevInfo.ViDev;
    enSnsType    = pstViInfo->stSnsInfo.enSnsType;
    SAMPLE_PRT("enSnsType = %d \n", enSnsType);
    SAMPLE_COMM_VI_GetDevAttrBySns(ViDev, enSnsType, pstViInfo->stDevInfo.bDvpHighLowByteSwap, &stViDevAttr);
    stViDevAttr.stWDRAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
    if(VI_PARALLEL_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode || VI_PARALLEL_VPSS_PARALLEL == pstViInfo->stPipeInfo.enMastPipeMode)
    {
        stViDevAttr.enDataRate = DATA_RATE_X2;
    }

    s32Ret = AR_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_EnableDev(ViDev);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32 s32Ret;
    VI_DEV ViDev;

    ViDev   = pstViInfo->stDevInfo.ViDev;
    s32Ret  = AR_MPI_VI_DisableDev(ViDev);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32              i;
    AR_S32              s32PipeCnt = 0;
    AR_S32              s32Ret;
    VI_DEV_BIND_PIPE_S  stDevBindPipe = {0};

    for (i = 0; i < 4; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            stDevBindPipe.PipeId[s32PipeCnt] = pstViInfo->stPipeInfo.aPipe[i];
            s32PipeCnt++;
            stDevBindPipe.u32Num = s32PipeCnt;
        }
    }

    s32Ret = AR_MPI_VI_SetDevBindPipe(pstViInfo->stDevInfo.ViDev, &stDevBindPipe);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_SetDevBindPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return s32Ret;
}

static AR_S32 SAMPLE_COMM_VI_ModeSwitchCreateSingleViPipe(VI_PIPE ViPipe, VI_PIPE_ATTR_S* pstPipeAttr)
{
    AR_S32 s32Ret;

    s32Ret = AR_MPI_VI_CreatePipe(ViPipe, pstPipeAttr);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return s32Ret;
}


static AR_S32 SAMPLE_COMM_VI_ModeSwitch_EnableSingleViPipe(VI_PIPE ViPipe, VI_PIPE_ATTR_S* pstPipeAttr)
{
    AR_S32 s32Ret;

    s32Ret = AR_MPI_VI_StartPipe(ViPipe);

    if (s32Ret != AR_SUCCESS)
    {
        AR_MPI_VI_DestroyPipe(ViPipe);
        SAMPLE_PRT("AR_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return s32Ret;
}


AR_S32 SAMPLE_COMM_VI_StopSingleViPipe(VI_PIPE ViPipe)
{
    AR_S32  s32Ret;

    s32Ret = AR_MPI_VI_StopPipe(ViPipe);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return s32Ret;
}


AR_S32 SAMPLE_COMM_VI_ModeSwitch_StartViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32          i, j;
    AR_S32          s32Ret = AR_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], pstViInfo->stPipeInfo.u32BitWidth[i], pstViInfo->stPipeInfo.enTrigger, &stPipeAttr);
            {
                s32Ret = SAMPLE_COMM_VI_ModeSwitchCreateSingleViPipe(ViPipe, &stPipeAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("SAMPLE_COMM_VI_StartSingleViPipe  %d failed!\n", ViPipe);
                    goto EXIT;
                }
            }

        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}


AR_S32 SAMPLE_COMM_VI_ModeSwitch_EnableViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32          i, j;
    AR_S32          s32Ret = AR_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], pstViInfo->stPipeInfo.u32BitWidth[i], pstViInfo->stPipeInfo.enTrigger, &stPipeAttr);
            {
                s32Ret = SAMPLE_COMM_VI_ModeSwitch_EnableSingleViPipe(ViPipe, &stPipeAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("SAMPLE_COMM_VI_StartSingleViPipe  %d failed!\n", ViPipe);
                    goto EXIT;
                }
            }

        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StartSingleViPipe(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx)
{
    AR_S32 s32Ret = AR_SUCCESS;
    VI_PIPE ViPipe = pstViInfo->stPipeInfo.aPipe[s32PipeIdx];
    VI_PIPE_ATTR_S stPipeAttr;
    AR_S32 i = 0;

    if (ViPipe < 0 || ViPipe >= VI_MAX_PIPE_NUM) {
        SAMPLE_PRT("ViPipe[%d] is out of range[0, %d)\n", ViPipe, VI_MAX_PIPE_NUM);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    SAMPLE_PRT("ViPipe = %d, enSnsType = %d", ViPipe, pstViInfo->stSnsInfo.enSnsType);

    SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
        pstViInfo->stPipeInfo.u32Width[s32PipeIdx], pstViInfo->stPipeInfo.u32Height[s32PipeIdx],
        pstViInfo->stPipeInfo.f32Fps[s32PipeIdx], pstViInfo->stPipeInfo.u32BitWidth[s32PipeIdx], pstViInfo->stPipeInfo.enTrigger,  &stPipeAttr);

    if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe)
        && (ViPipe == pstViInfo->stSnapInfo.SnapPipe)) {
        s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
        if (s32Ret != AR_SUCCESS) {
            SAMPLE_PRT("AR_MPI_VI_CreatePipe failed: ViPipe[%s] Ret[%#x]!\n", ViPipe, s32Ret);
            goto EXIT;
        }
    } else {
        s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
        if (s32Ret != AR_SUCCESS) {
            SAMPLE_PRT("AR_MPI_VI_CreatePipe failed: ViPipe[%s] Ret[%#x]!\n", ViPipe, s32Ret);
            return AR_FAILURE;
        }

        if (AR_TRUE == pstViInfo->stPipeInfo.bVcNumCfged) {
            s32Ret = AR_MPI_VI_SetPipeVCNumber(ViPipe, pstViInfo->stPipeInfo.u32VCNum[s32PipeIdx]);
            if (s32Ret != AR_SUCCESS) {
                AR_MPI_VI_DestroyPipe(ViPipe);
                SAMPLE_PRT("AR_MPI_VI_SetPipeVCNumber failed: ViPipe[%s] Ret[%#x]!\n",
                    ViPipe, s32Ret);
                return AR_FAILURE;
            }
        }

        s32Ret = AR_MPI_VI_StartPipe(ViPipe);
        if (s32Ret != AR_SUCCESS) {
            AR_MPI_VI_DestroyPipe(ViPipe);
            SAMPLE_PRT("AR_MPI_VI_StartPipe failed: ViPipe[%s] Ret[%#x]!\n", ViPipe, s32Ret);
            return AR_FAILURE;
        }
    }

    return s32Ret;

EXIT:
    for (i = 0; i < s32PipeIdx; i++) {
        ViPipe = pstViInfo->stPipeInfo.aPipe[i];
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StartViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32          i, j;
    AR_S32          s32Ret = AR_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_PRT("ViPipe = %d, enSnsType = %d", pstViInfo->stPipeInfo.aPipe[i], pstViInfo->stSnsInfo.enSnsType);

            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], pstViInfo->stPipeInfo.u32BitWidth[i], pstViInfo->stPipeInfo.enTrigger, &stPipeAttr);

			if(pstViInfo->stPipeInfo.dpcm==1)
			{
			    stPipeAttr.enCompressMode=COMPRESS_MODE_DPCM_6BITS;
				stPipeAttr.stNrAttr.enCompressMode=COMPRESS_MODE_DPCM_6BITS;
			}else if(pstViInfo->stPipeInfo.dpcm==2)
			{
    			stPipeAttr.enCompressMode=COMPRESS_MODE_NONE;
    			stPipeAttr.stNrAttr.enCompressMode=COMPRESS_MODE_NONE;
			}

			if(stPipeAttr.enCompressMode==COMPRESS_MODE_NONE)
			{
			    stPipeAttr.enEnableNoCmp=VIN_PIPE_NOCPM_ENABLE;
			}

           if(pstViInfo->stPipeInfo.feature_mask)
           {
               stPipeAttr.u32FeatureMask=pstViInfo->stPipeInfo.feature_mask;
           }

            if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe) && (ViPipe == pstViInfo->stSnapInfo.SnapPipe))
            {
                s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
                    goto EXIT;
                }
            }
            else
            {
				SAMPLE_PRT("ViPipe = %d, attr->bitwide = %d\n", stPipeAttr.enBitWidth);
                s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
                    return AR_FAILURE;
                }

                if (AR_TRUE == pstViInfo->stPipeInfo.bVcNumCfged)
                {
                    s32Ret = AR_MPI_VI_SetPipeVCNumber(ViPipe, pstViInfo->stPipeInfo.u32VCNum[i]);
                    if (s32Ret != AR_SUCCESS)
                    {
                        AR_MPI_VI_DestroyPipe(ViPipe);
                        SAMPLE_PRT("AR_MPI_VI_SetPipeVCNumber failed with %#x!\n", s32Ret);
                        return AR_FAILURE;
                    }

					s32Ret = AR_MPI_VI_SetPipeDTNumber(ViPipe, pstViInfo->stPipeInfo.u32VCDTNum[i]);
                    if (s32Ret != AR_SUCCESS)
                    {
                        AR_MPI_VI_DestroyPipe(ViPipe);
                        SAMPLE_PRT("AR_MPI_VI_SetPipeDTNumber failed with %#x!\n", s32Ret);
                        return AR_FAILURE;
                    }
                }

                s32Ret = AR_MPI_VI_StartPipe(ViPipe);
                if (s32Ret != AR_SUCCESS)
                {
                    AR_MPI_VI_DestroyPipe(ViPipe);
                    SAMPLE_PRT("AR_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
                    return AR_FAILURE;
                }
            }

        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32  i;
    VI_PIPE ViPipe;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
        }
    }

    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_VI_ModeSwitch_StartViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32              i;
    AR_BOOL             bNeedChn;
    AR_S32              s32Ret = AR_SUCCESS;
    VI_PIPE             ViPipe;
    VI_CHN              ViChn;
    VI_CHN_ATTR_S       stChnAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn  = pstViInfo->stChnInfo.ViChn;

            SAMPLE_COMM_VI_GetChnAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], &stChnAttr);
            stChnAttr.enDynamicRange = pstViInfo->stChnInfo.enDynamicRange;
            stChnAttr.enVideoFormat  = pstViInfo->stChnInfo.enVideoFormat;
            if (pstViInfo->stPipeInfo.u32enPixFormat[i] > 0)
				stChnAttr.enPixelFormat = pstViInfo->stPipeInfo.u32enPixFormat[i];
			else
            	stChnAttr.enPixelFormat  = pstViInfo->stChnInfo.enPixFormat;
            stChnAttr.enCompressMode = pstViInfo->stChnInfo.enCompressMode;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = AR_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? AR_FALSE : AR_TRUE;
            }

            if (bNeedChn)
            {
                s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
                    return AR_FAILURE;
                }
            }
        }
    }

    return s32Ret;
}
AR_S32 SAMPLE_COMM_VI_StartViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32              i;
    AR_BOOL             bNeedChn;
    AR_S32              s32Ret = AR_SUCCESS;
    VI_PIPE             ViPipe;
    VI_CHN              ViChn;
    VI_CHN_ATTR_S       stChnAttr;
    VI_VPSS_MODE_E      enMastPipeMode=VI_OFFLINE_VPSS_OFFLINE;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn  = pstViInfo->stChnInfo.ViChn;

            SAMPLE_COMM_VI_GetChnAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], &stChnAttr);
            stChnAttr.enDynamicRange = pstViInfo->stChnInfo.enDynamicRange;
            stChnAttr.enVideoFormat  = pstViInfo->stChnInfo.enVideoFormat;
            if (pstViInfo->stPipeInfo.u32enPixFormat[i] > 0)
				stChnAttr.enPixelFormat = pstViInfo->stPipeInfo.u32enPixFormat[i];
			else
            	stChnAttr.enPixelFormat  = pstViInfo->stChnInfo.enPixFormat;
            stChnAttr.enCompressMode = pstViInfo->stChnInfo.enCompressMode;

			if(pstViInfo->stChnInfo.u32Width >0)
			{
			   stChnAttr.stSize.u32Width=pstViInfo->stChnInfo.u32Width;
			}

			if(pstViInfo->stChnInfo.u32Height >0 )
			{
			   stChnAttr.stSize.u32Height=pstViInfo->stChnInfo.u32Height;
			}

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = AR_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? AR_FALSE: AR_TRUE;
            }

            if (bNeedChn)
            {
                s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);

                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("AR_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
                    return AR_FAILURE;
                }

                enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

                if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_ONLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode)
                {
                    s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);

                    if (s32Ret != AR_SUCCESS)
                    {
                        SAMPLE_PRT("AR_MPI_VI_EnableChn failed with %#x!\n", s32Ret);
                        return AR_FAILURE;
                    }
                }else
                {
                    SAMPLE_PRT("AR_MPI_VI_EnableChn can not enable enMastPipeMode=%d\n",enMastPipeMode);
					return AR_FAILURE;
                }
            }
        }
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StopViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32              i;
    AR_BOOL             bNeedChn;
    AR_S32              s32Ret = AR_SUCCESS;
    VI_PIPE             ViPipe;
    VI_CHN              ViChn;
    VI_VPSS_MODE_E      enMastPipeMode;


    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn  = pstViInfo->stChnInfo.ViChn;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = AR_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? AR_FALSE : AR_TRUE;
            }

            if (bNeedChn)
            {
                enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

                if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_ONLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode)
                {
                    s32Ret = AR_MPI_VI_DisableChn(ViPipe, ViChn);

                    if (s32Ret != AR_SUCCESS)
                    {
                        SAMPLE_PRT("AR_MPI_VI_DisableChn failed with %#x!\n", s32Ret);
                        return AR_FAILURE;
                    }
                }
            }
        }
    }

    return s32Ret;
}

static AR_S32 SAMPLE_COMM_VI_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32 s32Ret = AR_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_StartDev(pstViInfo);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed !\n");
        return AR_FAILURE;
    }

    //we should bind pipe,then creat pipe
    s32Ret = SAMPLE_COMM_VI_BindPipeDev(pstViInfo);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_BindPipeDev failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViPipe(pstViInfo);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViChn(pstViInfo);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }

    return AR_SUCCESS;

EXIT2:
    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

EXIT1:
    SAMPLE_COMM_VI_StopDev(pstViInfo);


    return s32Ret;
}



static AR_S32 SAMPLE_COMM_ModeSwitch_VI_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32 s32Ret = AR_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_StartDev(pstViInfo);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed !\n");
        return AR_FAILURE;
    }

    //we should bind pipe,then creat pipe
    s32Ret = SAMPLE_COMM_VI_BindPipeDev(pstViInfo);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_BindPipeDev failed !\n");
        goto EXIT1;
    }


    s32Ret = SAMPLE_COMM_VI_ModeSwitch_StartViPipe(pstViInfo);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_ModeSwitch_StartViChn(pstViInfo);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }


    return AR_SUCCESS;

EXIT2:
    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

EXIT1:
    SAMPLE_COMM_VI_StopDev(pstViInfo);


    return s32Ret;
}


static AR_S32 SAMPLE_COMM_VI_StartPipe_Chn(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32 s32Ret = AR_SUCCESS;


    s32Ret = SAMPLE_COMM_VI_ModeSwitch_EnableViPipe(pstViInfo);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViChn(pstViInfo);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }

    return AR_SUCCESS;

EXIT2:
    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

EXIT1:
    SAMPLE_COMM_VI_StopDev(pstViInfo);


    return s32Ret;
}




static AR_S32 SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    SAMPLE_COMM_VI_StopViChn(pstViInfo);

    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

    SAMPLE_COMM_VI_StopDev(pstViInfo);

    return AR_SUCCESS;
}


static AR_S32 SAMPLE_COMM_VI_DestroySinglePipe_Chn(SAMPLE_VI_INFO_S* pstViInfo)
{
   SAMPLE_COMM_VI_StopViChn(pstViInfo);

   SAMPLE_COMM_VI_StopViPipe(pstViInfo);

    return AR_SUCCESS;
}



AR_S32 SAMPLE_COMM_VI_CreateVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i, j;
    AR_S32              s32ViNum;
    AR_S32              s32Ret = AR_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        s32Ret = SAMPLE_COMM_VI_CreateSingleVi(pstViInfo);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
            goto EXIT;
        }
    }

    return AR_SUCCESS;
EXIT:

    for (j = 0; j < i; j++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[j];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
    }

    return s32Ret;
}


AR_S32 SAMPLE_COMM_ModeSwitch_VI_CreateVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i, j;
    AR_S32              s32ViNum;
    AR_S32              s32Ret = AR_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        s32Ret = SAMPLE_COMM_ModeSwitch_VI_CreateSingleVi(pstViInfo);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
            goto EXIT;
        }
    }

    return AR_SUCCESS;
EXIT:

    for (j = 0; j < i; j++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[j];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_ModeSwitch_VI_StartPipe_Chn(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i, j;
    AR_S32              s32ViNum;
    AR_S32              s32Ret = AR_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        s32Ret = SAMPLE_COMM_VI_StartPipe_Chn(pstViInfo);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
            goto EXIT;
        }
    }

    return AR_SUCCESS;
EXIT:

    for (j = 0; j < i; j++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[j];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySinglePipe_Chn(pstViInfo);
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32            i;
    AR_S32            s32ViNum;
    SAMPLE_VI_INFO_S* pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_StartSingleIsp(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx)
{
    AR_BOOL             bNeedPipe;
    AR_S32              s32Ret = AR_SUCCESS;
    VI_PIPE             ViPipe = pstViInfo->stPipeInfo.aPipe[s32PipeIdx];
    AR_U32              u32SnsId;
    ISP_PUB_ATTR_S      stPubAttr;
    VI_PIPE_ATTR_S      stPipeAttr;

    if (ViPipe < 0 || ViPipe >= VI_MAX_PIPE_NUM) {
        SAMPLE_PRT("ViPipe[%d] is out of range[0, %d)\n", ViPipe, VI_MAX_PIPE_NUM);
        return AR_ERR_VI_INVALID_PIPEID;
    }

    SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[0], pstViInfo->stPipeInfo.u32Height[0],
                pstViInfo->stPipeInfo.f32Fps[0],  pstViInfo->stPipeInfo.u32BitWidth[0], pstViInfo->stPipeInfo.enTrigger, &stPipeAttr);

    if (VI_PIPE_BYPASS_BE == stPipeAttr.enPipeBypassMode) {
        return AR_SUCCESS;
    }

    u32SnsId    = pstViInfo->stSnsInfo.s32SnsId;

    SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType,
        pstViInfo->stPipeInfo.u32Width[s32PipeIdx], pstViInfo->stPipeInfo.u32Height[s32PipeIdx],
        pstViInfo->stPipeInfo.f32Fps[s32PipeIdx], &stPubAttr);
    stPubAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;

    if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode) {
        bNeedPipe = AR_TRUE;
    } else {
        bNeedPipe = (s32PipeIdx > 0) ? AR_FALSE : AR_TRUE;
    }

    if (AR_TRUE != bNeedPipe) {
        return AR_SUCCESS;
    }

    s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId);
    if (AR_SUCCESS != s32Ret) {
        SAMPLE_PRT("register sensor %d to ISP %d failed\n", u32SnsId, ViPipe);
        SAMPLE_COMM_ISP_Stop(ViPipe);
        return AR_FAILURE;
    }

    if(((pstViInfo->stSnapInfo.bDoublePipe) && (pstViInfo->stSnapInfo.SnapPipe == ViPipe))
        || (pstViInfo->stPipeInfo.bMultiPipe && s32PipeIdx > 0)) {
        s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, -1);
        if (AR_SUCCESS != s32Ret) {
            SAMPLE_PRT("register sensor %d bus id %d failed\n", u32SnsId, pstViInfo->stSnsInfo.s32BusId);
            SAMPLE_COMM_ISP_Stop(ViPipe);
            return AR_FAILURE;
        }
    } else {
        s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.s32BusId);
        if (AR_SUCCESS != s32Ret) {
            SAMPLE_PRT("register sensor %d bus id %d failed\n", u32SnsId, pstViInfo->stSnsInfo.s32BusId);
            SAMPLE_COMM_ISP_Stop(ViPipe);
            return AR_FAILURE;
        }
    }


    s32Ret = AR_MPI_ISP_MemInit(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        SAMPLE_PRT("Init Ext memory failed with %#x!\n", s32Ret);
        SAMPLE_COMM_ISP_Stop(ViPipe);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
    if (s32Ret != AR_SUCCESS) {
        SAMPLE_PRT("SetPubAttr failed with %#x!\n", s32Ret);
        SAMPLE_COMM_ISP_Stop(ViPipe);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_ISP_Init(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        SAMPLE_PRT("ISP Init failed with %#x!\n", s32Ret);
        SAMPLE_COMM_ISP_Stop(ViPipe);
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);
    if (s32Ret != AR_SUCCESS) {
        SAMPLE_PRT("ISP Run failed with %#x!\n", s32Ret);
        SAMPLE_COMM_ISP_Stop(ViPipe);
        return AR_FAILURE;
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32              i = 0;
    AR_BOOL             bNeedPipe;
    AR_S32              s32Ret = AR_SUCCESS;
    VI_PIPE             ViPipe;
    AR_U32              u32SnsId;
    ISP_PUB_ATTR_S      stPubAttr;
    VI_PIPE_ATTR_S      stPipeAttr;

    SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], pstViInfo->stPipeInfo.u32BitWidth[i], pstViInfo->stPipeInfo.enTrigger, &stPipeAttr);

    if (VI_PIPE_BYPASS_BE == stPipeAttr.enPipeBypassMode)
    {
        return AR_SUCCESS;
    }

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe      = pstViInfo->stPipeInfo.aPipe[i];
            u32SnsId    = pstViInfo->stSnsInfo.s32SnsId;

            SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                pstViInfo->stPipeInfo.f32Fps[i], &stPubAttr);
            stPubAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedPipe = AR_TRUE;
            }
            else
            {
                bNeedPipe = (i > 0) ? AR_FALSE : AR_TRUE;
            }

            if (AR_TRUE != bNeedPipe)
            {
                continue;
            }

            s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId);

            if (AR_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("register sensor %d to ISP %d failed\n", u32SnsId, ViPipe);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return AR_FAILURE;
            }

            if(((pstViInfo->stSnapInfo.bDoublePipe) && (pstViInfo->stSnapInfo.SnapPipe == ViPipe))
                || (pstViInfo->stPipeInfo.bMultiPipe && i > 0))
            {
                s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.s32BusId);

                if (AR_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("register sensor %d bus id %d failed\n", u32SnsId, pstViInfo->stSnsInfo.s32BusId);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return AR_FAILURE;
                }
            }
            else
            {
                s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.s32BusId);

                if (AR_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("register sensor %d bus id %d failed\n", u32SnsId, pstViInfo->stSnsInfo.s32BusId);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return AR_FAILURE;
                }
            }

			SAMPLE_COMM_ISP_Set_Focus(ViPipe,u32SnsId,pstViInfo->stSnsInfo.s32FocusModule);


            s32Ret = AR_MPI_ISP_MemInit(ViPipe);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("Init Ext memory failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return AR_FAILURE;
            }

            s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("SetPubAttr failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return AR_FAILURE;
            }

            s32Ret = AR_MPI_ISP_Init(ViPipe);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("ISP Init failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return AR_FAILURE;
            }

            s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("ISP Run failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return AR_FAILURE;
            }
        }
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StopIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    AR_S32  i;
    AR_BOOL bNeedPipe;
    VI_PIPE ViPipe;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe    = pstViInfo->stPipeInfo.aPipe[i];

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedPipe = AR_TRUE;
            }
            else
            {
                bNeedPipe = (i > 0) ? AR_FALSE : AR_TRUE;
            }

            if (AR_TRUE != bNeedPipe)
            {
                continue;
            }

            SAMPLE_COMM_ISP_Stop(ViPipe);
        }
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i;
    AR_S32              s32ViNum;
    AR_S32              s32Ret = AR_SUCCESS;

    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StartIsp failed !\n");
            return AR_FAILURE;
        }



    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_DestroyIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i;
    AR_S32              s32ViNum;
    AR_S32              s32Ret = AR_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];



        s32Ret = SAMPLE_COMM_VI_StopIsp(pstViInfo);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopIsp failed !\n");
            return AR_FAILURE;
        }
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32 s32Ret = AR_SUCCESS;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfig);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed!\n");
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed!\n");
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_CreateVi(pstViConfig);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateVi failed!\n");
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_CreateIsp(pstViConfig);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_COMM_VI_DestroyVi(pstViConfig);
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp failed!\n");
        return AR_FAILURE;
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_BindSensor(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx)
{
    AR_BOOL             bNeedPipe = AR_FALSE;
    AR_S32              s32Ret = AR_SUCCESS;
    VI_PIPE             ViPipe = pstViInfo->stPipeInfo.aPipe[s32PipeIdx];
    AR_U32              u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
    VI_PIPE_ATTR_S      stPipeAttr;

    SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
        pstViInfo->stPipeInfo.u32Width[0], pstViInfo->stPipeInfo.u32Height[0],
        pstViInfo->stPipeInfo.f32Fps[0], pstViInfo->stPipeInfo.u32BitWidth[0], pstViInfo->stPipeInfo.enTrigger,&stPipeAttr);

    if (VI_PIPE_BYPASS_BE == stPipeAttr.enPipeBypassMode) {
        return AR_SUCCESS;
    }

    if (ViPipe < 0 || ViPipe >= VI_MAX_PIPE_NUM) {
        SAMPLE_PRT("ViPipe[%d] invalid\n", ViPipe);
        return AR_FAILURE;
    }

    if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode) {
        bNeedPipe = AR_TRUE;
    } else {
        bNeedPipe = (s32PipeIdx > 0 ? AR_FALSE : AR_TRUE);
    }

    if (AR_TRUE != bNeedPipe) {
        return AR_SUCCESS;
    }

    s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId);
    if (AR_SUCCESS != s32Ret) {
        SAMPLE_PRT("register sensor %d to ISP %d failed\n", u32SnsId, ViPipe);
        goto FAIL1;
    }

    if(((pstViInfo->stSnapInfo.bDoublePipe) && (pstViInfo->stSnapInfo.SnapPipe == ViPipe))
        || (pstViInfo->stPipeInfo.bMultiPipe && s32PipeIdx > 0)) {
        s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, -1);
        if (AR_SUCCESS != s32Ret) {
            SAMPLE_PRT("register sensor %d bus id %d failed\n",
                u32SnsId, pstViInfo->stSnsInfo.s32BusId);
            goto FAIL2;
        }
    } else {
        s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType,
            pstViInfo->stSnsInfo.s32BusId);
        if (AR_SUCCESS != s32Ret) {
            SAMPLE_PRT("register sensor %d bus id %d failed\n",
                u32SnsId, pstViInfo->stSnsInfo.s32BusId);
            goto FAIL2;
        }
    }

    return s32Ret;

FAIL2:
    SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ViPipe);
FAIL1:
    return s32Ret;
}

AR_VOID SAMPLE_COMM_VI_UnbindSensor(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx)
{
    VI_PIPE             ViPipe = pstViInfo->stPipeInfo.aPipe[s32PipeIdx];
    AR_BOOL             bNeedPipe;
    VI_PIPE_ATTR_S      stPipeAttr;

    SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType,
        pstViInfo->stPipeInfo.u32Width[0], pstViInfo->stPipeInfo.u32Height[0],
        pstViInfo->stPipeInfo.f32Fps[0], pstViInfo->stPipeInfo.u32BitWidth[0], pstViInfo->stPipeInfo.enTrigger, &stPipeAttr);

	if (VI_PIPE_BYPASS_BE == stPipeAttr.enPipeBypassMode) {
        return;
    }

    if (ViPipe < 0 || ViPipe >= VI_MAX_PIPE_NUM) {
        SAMPLE_PRT("ViPipe[%d] invalid\n", ViPipe);
        return;
    }

    if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode) {
        bNeedPipe = AR_TRUE;
    } else {
        bNeedPipe = (s32PipeIdx > 0 ? AR_FALSE : AR_TRUE);
    }

    if (AR_TRUE != bNeedPipe) {
        return;
    }

    SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ViPipe);

    return;
}

AR_S32 SAMPLE_COMM_VI_StartVi_Step1(SAMPLE_VI_CONFIG_S* pstViConfig)
{
	AR_S32 s32Ret = AR_SUCCESS;

	if (!pstViConfig)
	{
		SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
		return AR_FAILURE;
	}

	s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfig);
	if (s32Ret != AR_SUCCESS)
	{
		SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed!\n");
		return AR_FAILURE;
	}

	s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
	if (s32Ret != AR_SUCCESS)
	{
		SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed!\n");
		return AR_FAILURE;
	}

	s32Ret = SAMPLE_COMM_VI_CreateVi(pstViConfig);
	if (s32Ret != AR_SUCCESS)
	{
		SAMPLE_PRT("SAMPLE_COMM_VI_CreateVi failed!\n");
		return AR_FAILURE;
	}
	return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StartVi_Step2(SAMPLE_VI_CONFIG_S* pstViConfig)
{
	AR_S32 s32Ret = AR_SUCCESS;

	if (!pstViConfig)
	{
		SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
		return AR_FAILURE;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(pstViConfig);
	if (s32Ret != AR_SUCCESS)
	{
		SAMPLE_COMM_VI_DestroyVi(pstViConfig);
		SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp failed!\n");
		return AR_FAILURE;
	}
	return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32 s32Ret = AR_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_DestroyIsp(pstViConfig);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DestroyIsp failed !\n");
        return AR_FAILURE;
    }

#if 0
    else
    {
        if (AR_FALSE == pstViConfig->bSwitch)
        {
            s32Ret = SAMPLE_COMM_VI_DestroyIsp(pstViConfig);

            if (s32Ret != AR_SUCCESS)
            {
                SAMPLE_PRT("SAMPLE_COMM_VI_DestroyIsp failed !\n");
                return AR_FAILURE;
            }
        }
    }
#endif

    s32Ret = SAMPLE_COMM_VI_DestroyVi(pstViConfig);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DestroyVi failed !\n");
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_StopMIPI(pstViConfig);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StopMIPI failed !\n");
        return AR_FAILURE;
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_SwitchISPMode(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32              i;
    AR_S32              s32ViNum;
    AR_S32              s32Ret = AR_SUCCESS;
    AR_BOOL             bNeedPipe;
    VI_PIPE             ViPipe = 0;
    ISP_PUB_ATTR_S      stPubAttr;
    SAMPLE_VI_INFO_S*   pstViInfo = AR_NULL;
    ISP_INNER_STATE_INFO_S stInnerStateInfo;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return AR_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
        {
            if ( pstViInfo->stPipeInfo.aPipe[i] >= 0 && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
            {

                SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType,
                    pstViInfo->stPipeInfo.u32Width[i], pstViInfo->stPipeInfo.u32Height[i],
                    pstViInfo->stPipeInfo.f32Fps[i], &stPubAttr);

                stPubAttr.enWDRMode =  pstViInfo->stDevInfo.enWDRMode;

                SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp enWDRMode is %d!\n", stPubAttr.enWDRMode);

                if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode )
                {
                    bNeedPipe = AR_TRUE;
                }
                else
                {
                    bNeedPipe = (i > 0) ? AR_FALSE : AR_TRUE;
                }

                if (AR_TRUE != bNeedPipe)
                {
                    continue;
                }

                ViPipe = pstViInfo->stPipeInfo.aPipe[i];

                s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
                if (s32Ret != AR_SUCCESS)
                {
                    SAMPLE_PRT("SetPubAttr failed with %#x!\n", s32Ret);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return AR_FAILURE;
                }

            }
        }
    }

    while (1)
    {
        AR_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
        if ((AR_TRUE == stInnerStateInfo.bResSwitchFinish)
            || (AR_TRUE == stInnerStateInfo.bWDRSwitchFinish))
        {
            SAMPLE_PRT("Switch finish!\n");
            break;
        }
        usleep(1000);
    }

    SAMPLE_COMM_VI_StartPipe_Chn(pstViInfo);

    return AR_SUCCESS;
}


AR_S32  SAMPLE_COMM_VI_SwitchMode_StopVI(SAMPLE_VI_CONFIG_S* pstViConfigSrc)
{

    AR_S32              s32Ret = AR_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_DestroyVi(pstViConfigSrc);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DestroyVi failed !\n");
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_StopMIPI(pstViConfigSrc);

    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StopMIPI failed !\n");
        return AR_FAILURE;
    }

    return AR_SUCCESS;

}


AR_S32  SAMPLE_COMM_VI_SwitchMode(SAMPLE_VI_CONFIG_S* pstViConfigDes)
{

    AR_S32 s32Ret = AR_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfigDes);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed!\n");
        return AR_FAILURE;
    }

    /*   create vi without enable chn and enable pipe. */
    s32Ret =  SAMPLE_COMM_ModeSwitch_VI_CreateVi(pstViConfigDes);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateVi failed!\n");
        return AR_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_SwitchISPMode(pstViConfigDes);
    if (s32Ret != AR_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_ModeSwitch_VI_CreateIsp!\n");
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}


int SAMPLE_COMM_VI_ExitMpp( int s32poolId)
{
    if (s32poolId < 0)
    {
        if (AR_MPI_SYS_Exit())
        {
            SAMPLE_PRT("sys exit fail\n");
            return -1;
        }

        if (AR_MPI_VB_Exit())
        {
            SAMPLE_PRT("vb exit fail\n");
            return -1;
        }

        return -1;
    }

    return 0;
}

/******************************************************************************
* funciton : Get enWDRMode by diffrent sensor
******************************************************************************/
AR_S32 SAMPLE_COMM_VI_GetWDRModeBySensor(SAMPLE_SNS_TYPE_E enMode, WDR_MODE_E* penWDRMode)
{
    AR_S32 s32Ret = AR_SUCCESS;

    if (!penWDRMode)
    {
        return AR_FAILURE;
    }

    switch (enMode)
    {
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            *penWDRMode = WDR_MODE_2To1_LINE;
            break;

        default:
            *penWDRMode = WDR_MODE_NONE;
            break;
    }

    return s32Ret;
}

/******************************************************************************
* funciton : Get Pipe by diffrent sensor
******************************************************************************/
AR_S32 SAMPLE_COMM_VI_GetPipeBySensor(SAMPLE_SNS_TYPE_E enMode, SAMPLE_PIPE_INFO_S* pstPipeInfo)
{
    AR_S32 s32Ret = AR_SUCCESS;

    if (!pstPipeInfo)
    {
        return AR_FAILURE;
    }

    switch (enMode)
    {
        default:
            pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;
            pstPipeInfo->aPipe[0] = 0;
            pstPipeInfo->aPipe[1] = 1;
            pstPipeInfo->aPipe[2] = -1;
            pstPipeInfo->aPipe[3] = -1;
            break;
    }

    return s32Ret;
}

/******************************************************************************
* funciton : Get enSize by diffrent sensor
******************************************************************************/
AR_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E* penSize)
{
    AR_S32 s32Ret = AR_SUCCESS;

    if (!penSize)
    {
        return AR_FAILURE;
    }

    switch (enMode)
    {
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        case NVP6324_MIPI_2M_30FPS_8BIT:
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
            *penSize = PIC_1080P;
            break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
        *penSize = PIC_1080P_BIG_PIC;
        break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        *penSize = PIC_8K_BIG_PIC;
        break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            *penSize = PIC_3840x2160;
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
            *penSize = PIC_2688x1520;
            break;
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
			*penSize = PIC_2712x1538;
			break;
		case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            break;
        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            break;
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
            *penSize = PIC_1920x2880;
            break;
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
            *penSize = PIC_3840x2160;
            break;
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
			//*penSize = PIC_2704x1520;
			break;
		case GC_GC2053_MIPI_1080P_30FPS_10BIT:
			*penSize = PIC_1080P;
			break;
        default:
            *penSize = PIC_3840x2160;
            break;
    }

    return s32Ret;
}



/******************************************************************************
* funciton : Get enSize by diffrent sensor
******************************************************************************/
AR_S32 SAMPLE_COMM_VI_GetFrameRateBySensor(SAMPLE_SNS_TYPE_E enMode, AR_U32* pu32FrameRate)
{
    AR_S32 s32Ret = AR_SUCCESS;

    if (!pu32FrameRate)
    {
        return AR_FAILURE;
    }

    switch (enMode)
    {
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
            *pu32FrameRate = 25;
            break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
            *pu32FrameRate = 30;
            break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        *pu32FrameRate = 5;
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
			*pu32FrameRate = 25;
            break;
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
            *pu32FrameRate = 60;
            break;
        default:
            *pu32FrameRate = 30;
            break;
    }
    return s32Ret;
}




AR_VOID SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    AR_S32 i;

	ar_memset(pstViConfig, sizeof(SAMPLE_VI_CONFIG_S), 0, sizeof(SAMPLE_VI_CONFIG_S));

    for (i = 0; i < VI_MAX_DEV_NUM; i++)
    {
        pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
        pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
        pstViConfig->astViInfo[i].stSnsInfo.MipiDev  = i;
        ar_memset(&pstViConfig->astViInfo[i].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
        pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = AR_FALSE;
        pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = AR_FALSE;
		pstViConfig->astViInfo[i].stPipeInfo.u32BitWidth[i] = DATA_BITWIDTH_12;
    }

    /*single sensor using I2C,  s32BusId must be set to 1
      single sensor using spi,  s32BusId must be set to 0
      if add sensor using spi into SAMPLE_SNS_TYPE_E, please add code branch with sensor type*/
    pstViConfig->astViInfo[0].stSnsInfo.s32BusId = 1;
    pstViConfig->astViInfo[1].stSnsInfo.s32BusId = 2;
    pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SENSOR0_TYPE;
    pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SENSOR1_TYPE;
    pstViConfig->astViInfo[2].stSnsInfo.enSnsType = SENSOR2_TYPE;
    pstViConfig->astViInfo[3].stSnsInfo.enSnsType = SENSOR3_TYPE;
    pstViConfig->astViInfo[4].stSnsInfo.enSnsType = SENSOR4_TYPE;
}

combo_dev_t SAMPLE_COMM_VI_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, AR_S32 s32SnsIdx)
{
    combo_dev_t dev = 0;

    switch (enMode)
    {
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
    case SC_SC2210_1080P:
    case OV_OS05A20_2688x1944:
    case OV_OS05A20_2336x1752:
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
            if (0 == s32SnsIdx)
            {
                dev = 0;
            }
            else if (1 == s32SnsIdx)
            {
                dev = 2;
            }
            else if (2 == s32SnsIdx)
            {
                dev = 4;
            }
            break;

        default:
            dev = 0;
            break;
    }

    return dev;
}

AR_S32 SAMPLE_COMM_VI_ConvertBitPixel(AR_U8 *pu8Data, AR_U32 u32DataNum, AR_U32 u32BitWidth, AR_U16 *pu16OutData)
{
    AR_S32 i, u32Tmp, s32OutCnt;
    AR_U32 u32Val;
    AR_U64 u64Val;
    AR_U8 *pu8Tmp = pu8Data;

    s32OutCnt = 0;
    switch(u32BitWidth)
    {
    case 10:
        {
            /* 4 pixels consist of 5 bytes  */
            u32Tmp = u32DataNum / 4;

            for (i = 0; i < u32Tmp; i++)
            {
                /* byte4 byte3 byte2 byte1 byte0 */
                pu8Tmp = pu8Data + 5 * i;
                u64Val = pu8Tmp[0] + ((AR_U32)pu8Tmp[1] << 8) + ((AR_U32)pu8Tmp[2] << 16) +
                         ((AR_U32)pu8Tmp[3] << 24) + ((AR_U64)pu8Tmp[4] << 32);

                pu16OutData[s32OutCnt++] = u64Val & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 10) & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 20) & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 30) & 0x3ff;
            }
        }
        break;
    case 12:
        {
            /* 2 pixels consist of 3 bytes  */
            u32Tmp = u32DataNum / 2;

            for (i = 0; i < u32Tmp; i++)
            {
                /* byte2 byte1 byte0 */
                pu8Tmp = pu8Data + 3 * i;
                u32Val = pu8Tmp[0] + (pu8Tmp[1] << 8) + (pu8Tmp[2] << 16);
                pu16OutData[s32OutCnt++] = u32Val & 0xfff;
                pu16OutData[s32OutCnt++] = (u32Val >> 12) & 0xfff;
            }
        }
        break;
    case 14:
        {
            /* 4 pixels consist of 7 bytes  */
            u32Tmp = u32DataNum / 4;

            for (i = 0; i < u32Tmp; i++)
            {
                pu8Tmp = pu8Data + 7 * i;
                u64Val = pu8Tmp[0] + ((AR_U32)pu8Tmp[1] << 8) + ((AR_U32)pu8Tmp[2] << 16) +
                         ((AR_U32)pu8Tmp[3] << 24) + ((AR_U64)pu8Tmp[4] << 32) +
                         ((AR_U64)pu8Tmp[5] << 40) + ((AR_U64)pu8Tmp[6] << 48);

                pu16OutData[s32OutCnt++] = u64Val & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 14) & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 28) & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 42) & 0x3fff;
            }
        }
        break;
    default:
        SAMPLE_PRT("unsuport bitWidth: %d\n", u32BitWidth);
        return AR_FAILURE;
        break;
    }

    return s32OutCnt;
}

static AR_S32 SAMPLE_COMM_VI_BitWidth2PixelFormat(AR_U32 u32Nbit, PIXEL_FORMAT_E *penPixelFormat)
{
    PIXEL_FORMAT_E enPixelFormat;

    if (8 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_8BPP;
    }
    else if (10 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_10BPP;
    }
    else if (12 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
    }
    else if (14 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP;
    }
    else if (16 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_16BPP;
    }
    else
    {
        return AR_FAILURE;
    }

    *penPixelFormat = enPixelFormat;
    return AR_SUCCESS;
}


AR_S32 SAMPLE_COMM_VI_SaveUncompressRaw(VIDEO_FRAME_S* pVBuf, AR_U32 u32Nbit, FILE* pfd)
{
    AR_U32  u32Height;
    AR_U64  u64PhyAddr;
    AR_U64  u64Size;
    AR_U8*  pu8VirAddr;
    AR_U16 *pu16Data = NULL;
    AR_U8  *pu8Data;
    PIXEL_FORMAT_E enPixelFormat = PIXEL_FORMAT_BUTT;

    SAMPLE_COMM_VI_BitWidth2PixelFormat(u32Nbit, &enPixelFormat);
    if (enPixelFormat != pVBuf->enPixelFormat)
    {
        SAMPLE_PRT("invalid pixel format:%d, u32Nbit: %d\n", pVBuf->enPixelFormat, u32Nbit);

        return AR_FAILURE;
    }

    u64Size = (pVBuf->u32Stride[0]) * ((AR_U64)pVBuf->u32Height);
    u64PhyAddr = pVBuf->u64PhyAddr[0];


    pu8VirAddr = (AR_U8*) AR_MPI_SYS_Mmap(u64PhyAddr, u64Size);
    if (NULL == pu8VirAddr)
    {
        SAMPLE_PRT("AR_MPI_SYS_Mmap fail !\n");

        return AR_FAILURE;
    }

    pu8Data = pu8VirAddr;
    if ((8 != u32Nbit) && (16 != u32Nbit))
    {
        pu16Data = (AR_U16*)malloc(pVBuf->u32Width * 2U);
        if (NULL == pu16Data)
        {
            SAMPLE_PRT("alloc memory failed\n");

            AR_MPI_SYS_Munmap(pu8VirAddr, u64Size);
            pu8VirAddr = NULL;
            return AR_FAILURE;
        }
    }

    /* save Y ----------------------------------------------------------------*/
    SAMPLE_PRT("saving......dump data......u32Stride[0]: %d, width: %d\n", pVBuf->u32Stride[0], pVBuf->u32Width);

    for (u32Height = 0; u32Height < pVBuf->u32Height; u32Height++)
    {
        if (8 == u32Nbit)
        {
            fwrite(pu8Data, pVBuf->u32Width, 1, pfd);
        }
        else if (16 == u32Nbit)
        {
            fwrite(pu8Data, pVBuf->u32Width, 2, pfd);
            fflush(pfd);
        }
        else
        {
            SAMPLE_COMM_VI_ConvertBitPixel(pu8Data, pVBuf->u32Width, u32Nbit, pu16Data);
            fwrite(pu16Data, pVBuf->u32Width, 2, pfd);
        }
        pu8Data += pVBuf->u32Stride[0];
    }
    fflush(pfd);

    SAMPLE_PRT("done u32TimeRef: %d!\n", pVBuf->u32TimeRef);

    if (NULL != pu16Data)
    {
        free(pu16Data);
    }
    AR_MPI_SYS_Munmap(pu8VirAddr, u64Size);
    pu8VirAddr = NULL;

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_SaveCompressedRaw(VIDEO_FRAME_S* pVBuf, AR_U32 u32Nbit, FILE* pfd)
{
    AR_U32  u32Height;
    AR_U64  u64PhyAddr;
    AR_U64  u64Size;
    AR_U32  u32DataSize;
    AR_U16  u16HeadData = 0x0;
    AR_U8*  pu8VirAddr;
    AR_U8  *pu8Data;
    PIXEL_FORMAT_E enPixelFormat = PIXEL_FORMAT_BUTT;

    SAMPLE_COMM_VI_BitWidth2PixelFormat(u32Nbit, &enPixelFormat);
    if (enPixelFormat != pVBuf->enPixelFormat)
    {
        SAMPLE_PRT("invalid pixel format:%d, u32Nbit: %d\n", pVBuf->enPixelFormat, u32Nbit);

        return AR_FAILURE;
    }

    u64Size = (pVBuf->u32Stride[0]) * ((AR_U64)pVBuf->u32Height);
    u64PhyAddr = pVBuf->u64PhyAddr[0];


    pu8VirAddr = (AR_U8*) AR_MPI_SYS_Mmap(u64PhyAddr, u64Size);
    if (NULL == pu8VirAddr)
    {
        SAMPLE_PRT("AR_MPI_SYS_Mmap fail !\n");

        return AR_FAILURE;
    }

    pu8Data = pu8VirAddr;

    /* save Y ----------------------------------------------------------------*/
    SAMPLE_PRT("saving......dump data......u32Stride[0]: %d, width: %d\n", pVBuf->u32Stride[0], pVBuf->u32Width);

    for (u32Height = 0; u32Height < pVBuf->u32Height; u32Height++)
    {
        u16HeadData = *(AR_U16*)pu8Data;

        u32DataSize =  (u16HeadData + 1) * 16;

        fwrite(pu8Data, u32DataSize, 1, pfd);

        pu8Data += pVBuf->u32Stride[0];
    }

    fflush(pfd);

    SAMPLE_PRT("done u32TimeRef: %d!\n", pVBuf->u32TimeRef);

    AR_MPI_SYS_Munmap(pu8VirAddr, u64Size);
    pu8VirAddr = NULL;

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_VI_SaveRaw(VIDEO_FRAME_S* pVBuf, AR_U32 u32Nbit, FILE* pfd)
{
    if(COMPRESS_MODE_NONE == pVBuf->enCompressMode)
    {
        return SAMPLE_COMM_VI_SaveUncompressRaw(pVBuf, u32Nbit, pfd);
    }
    else
    {
        return SAMPLE_COMM_VI_SaveCompressedRaw(pVBuf, u32Nbit, pfd);
    }

}
AR_U32 SAMPLE_COMM_VI_PixelFormat2BitWidth(PIXEL_FORMAT_E  enPixelFormat)
{
    switch (enPixelFormat)
    {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16;

        default:
            return 0;
    }

}

char* SAMPLE_COMM_VI_CompressMode2String(COMPRESS_MODE_E enCompressMode)
{
    if(COMPRESS_MODE_NONE == enCompressMode)
    {
        return "COMPRESS_MODE_NONE";
    }
    else if(COMPRESS_MODE_DPCM_6BITS == enCompressMode)
    {
        return "COMPRESS_MODE_DPCM_6BITS";
    }
    else if(COMPRESS_MODE_SEG == enCompressMode)
    {
        return "COMPRESS_MODE_SEG";
    }
    else if(COMPRESS_MODE_DPCM_8BITS == enCompressMode)
    {
        return "COMPRESS_MODE_DPCM_8BITS";
    }
	else if(COMPRESS_MODE_DPCM_10BITS == enCompressMode)
    {
        return "COMPRESS_MODE_DPCM_8BITS";
    }else
    {
        return "CMP_XXX";
    }
}

int SAMPLE_COMM_VI_SaveCompressParam(VI_CMP_PARAM_S* pCmpParam, FILE* pfd)
{
    fwrite(pCmpParam, sizeof(VI_CMP_PARAM_S), 1, pfd);
    fflush(pfd);
    return AR_SUCCESS;
}

AR_VOID* SAMPLE_COMM_VI_DumpRaw(AR_VOID* arg)
{
    AR_S32      s32Ret;
    VI_PIPE     ViPipe;
    AR_S32      s32Cnt;
    AR_S32      s32DumpCnt = 0;
    AR_U32      u32Width;
    AR_U32      u32Height;
    AR_U32      u32BitWidth;
    FILE*       pfile;
    AR_S32      s32MilliSec = 2000;
    AR_CHAR     name[256] = {0};
    VIDEO_FRAME_INFO_S stVideoFrame;
    VI_CMP_PARAM_S stCmpPara;
    AR_CHAR szThreadName[20];
    SAMPLE_VI_DUMP_THREAD_INFO_S* pstViDumpRawThreadInfo = AR_NULL;

    if (NULL == arg)
    {
        SAMPLE_PRT("arg is NULL\n");
        return NULL;
    }

    pstViDumpRawThreadInfo = (SAMPLE_VI_DUMP_THREAD_INFO_S*)arg;

    ViPipe = pstViDumpRawThreadInfo->ViPipe;
    s32Cnt = pstViDumpRawThreadInfo->s32Cnt;

    if (s32Cnt < 1)
    {
        SAMPLE_PRT("You really want to dump %d frame? That is impossible !\n", s32Cnt);
        return NULL;
    }

    snprintf(szThreadName, 20, "VI_PIPE%d_DUMP_RAW", ViPipe);
    prctl(PR_SET_NAME, szThreadName, 0,0,0);

    s32Ret = AR_MPI_VI_GetPipeFrame(ViPipe, &stVideoFrame, s32MilliSec);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VI_GetPipeFrame failed with %#x!\n", s32Ret);
        return AR_NULL;
    }

    s32Ret = AR_MPI_VI_ReleasePipeFrame(ViPipe, &stVideoFrame);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VI_ReleasePipeFrame failed with %#x!\n", s32Ret);
        return AR_NULL;
    }

    u32Width    = stVideoFrame.stVFrame.u32Width;
    u32Height   = stVideoFrame.stVFrame.u32Height;
    u32BitWidth = SAMPLE_COMM_VI_PixelFormat2BitWidth(stVideoFrame.stVFrame.enPixelFormat);

    snprintf(name, sizeof(name),"./data/%s_pipe%d_w%d_h%d_%dbits_%s_%d.raw",
        pstViDumpRawThreadInfo->aszName, ViPipe, u32Width, u32Height, u32BitWidth,
        SAMPLE_COMM_VI_CompressMode2String(stVideoFrame.stVFrame.enCompressMode), s32Cnt);
    pfile = fopen(name, "ab");

    if (NULL == pfile)
    {
        SAMPLE_PRT("ar_mipi_open file %s fail !\n", name);
        return AR_NULL;
    }

    if(COMPRESS_MODE_NONE != stVideoFrame.stVFrame.enCompressMode)
    {
        if(AR_SUCCESS != AR_MPI_VI_GetPipeCmpParam(ViPipe, &stCmpPara))
        {
            SAMPLE_PRT("AR_MPI_VI_GetPipeCmpParam failed with %#x!\n", s32Ret);
            goto end;
        }

        SAMPLE_COMM_VI_SaveCompressParam(&stCmpPara, pfile);
    }

    while ((AR_TRUE == g_stViDumpRawThreadInfo.bDump) && (s32DumpCnt < s32Cnt))
    {
        s32Ret = AR_MPI_VI_GetPipeFrame(ViPipe, &stVideoFrame, s32MilliSec);

        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("AR_MPI_VI_GetPipeFrame failed with %#x!\n", s32Ret);
            goto end;
        }

        s32Ret = SAMPLE_COMM_VI_SaveRaw(&stVideoFrame.stVFrame, u32BitWidth, pfile);

        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_SaveRaw failed with %#x!\n", s32Ret);
            goto end;
        }

        s32DumpCnt++;

        s32Ret = AR_MPI_VI_ReleasePipeFrame(ViPipe, &stVideoFrame);

        if (AR_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("AR_MPI_VI_ReleasePipeFrame failed with %#x!\n", s32Ret);
            goto end;
        }
    }

end:
    fclose(pfile);
    pfile = AR_NULL;
    return AR_NULL;
}

AR_S32 SAMPLE_COMM_VI_StartDumpRawThread(VI_PIPE ViPipe, AR_S32 s32Cnt, const AR_CHAR* pzsName)
{
    AR_S32 s32Ret;
    pthread_attr_t* pattr = NULL;

    g_stViDumpRawThreadInfo.ViPipe = ViPipe;
    g_stViDumpRawThreadInfo.s32Cnt = s32Cnt;
    g_stViDumpRawThreadInfo.bDump  = AR_TRUE;
    ar_memcpy(g_stViDumpRawThreadInfo.aszName, sizeof(g_stViDumpRawThreadInfo.aszName), pzsName, sizeof(g_stViDumpRawThreadInfo.aszName));

    s32Ret = pthread_create(&g_stViDumpRawThreadInfo.ThreadId, pattr, SAMPLE_COMM_VI_DumpRaw, (AR_VOID*)&g_stViDumpRawThreadInfo);

    if (0 != s32Ret)
    {
        SAMPLE_PRT("create GetViFrame thread failed! %s\n", strerror(s32Ret));
        goto out;
    }

out:

    if (NULL != pattr)
    {
        pthread_attr_destroy(pattr);
    }

    return s32Ret;

}

AR_S32 SAMPLE_COMM_VI_StopDumpRawThread(AR_VOID)
{
    if (AR_FALSE != g_stViDumpRawThreadInfo.bDump)
    {
        g_stViDumpRawThreadInfo.bDump  = AR_FALSE;
        pthread_join(g_stViDumpRawThreadInfo.ThreadId, NULL);
    }

    return AR_SUCCESS;
}



static AR_U32 SAMPLE_VI_PixelFormat2BitWidth(VIDEO_FRAME_S *pstVFrame)
{
    switch (pstVFrame->enPixelFormat)
    {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16;

        case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case PIXEL_FORMAT_YUV_400:
            if (pstVFrame->enDynamicRange == DYNAMIC_RANGE_SDR8)
            {
                return 8;
            }
            else
            {
                return 10;
            }

        default:
            return 0;
    }

}

static AR_U32 SAMPLE_VI_GetRawStride(PIXEL_FORMAT_E enPixelFormat, AR_U32 u32Width, AR_U32 u32ByteAlign)
{
    AR_U32 u32Stride = 0;

    if (PIXEL_FORMAT_RGB_BAYER_16BPP == enPixelFormat)
    {
       u32Stride = ALIGN_UP(u32Width*2, 16);
    }
    else if (PIXEL_FORMAT_RGB_BAYER_12BPP == enPixelFormat)
    {
        if (1 == u32ByteAlign)
        {
            u32Stride = (u32Width * 12 + 127) / 128 * 128 / 8;
        }
        else
        {
            u32Stride = (u32Width * 12 ) / 8;

            if (0 == ((u32Width * 12) % 8))
            {
                u32Stride = (u32Width * 12 ) / 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
            else
            {
                u32Stride = (u32Width * 12 ) / 8 + 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
        }
    }
    else if (PIXEL_FORMAT_RGB_BAYER_10BPP == enPixelFormat)
    {
        u32Stride = (u32Width * 10 + 127) / 128 * 128 / 8;
        if (1 == u32ByteAlign)
        {
            u32Stride = (u32Width * 10 + 127) / 128 * 128 / 8;
        }
        else
        {
            u32Stride = (u32Width * 10 ) / 8;

            if (0 == ((u32Width * 10 ) % 8))
            {
                u32Stride = (u32Width * 10 ) / 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
            else
            {
                u32Stride = (u32Width * 10 ) / 8 + 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
        }
    }
    else if (PIXEL_FORMAT_RGB_BAYER_8BPP == enPixelFormat)
    {
        u32Stride = (u32Width * 8 + 127) / 128 * 128 / 8;
    }

    return u32Stride;
}

AR_S32 SAMPLE_VI_GetFrameBlkInfo(SAMPLE_VI_FRAME_CONFIG_S *pstFrmCfg, AR_S32 s32FrmCnt, SAMPLE_VI_FRAME_INFO_S *pastViFrameInfo)
{
    AR_U32 i = 0;
    AR_U32 u32Stride;
    AR_U32 u32LStride;
    AR_U32 u32CStride;
    AR_U32 u32LumaSize = 0;
    AR_U32 u32ChrmSize = 0;
    AR_U32 u32Size;
    AR_U64 u64PhyAddr;
    AR_U8 *pVirAddr;
    VB_POOL u32PoolId;
    VB_BLK VbBlk;
    PIXEL_FORMAT_E enPixelFormat;
    VB_POOL_CONFIG_S stVbPoolCfg;

    enPixelFormat = pstFrmCfg->enPixelFormat;

    if (enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422)
    {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 2;
    }
    else if (enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)
    {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 3 / 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 4;
    }
    else if (enPixelFormat == PIXEL_FORMAT_YUV_400)
    {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height;
        u32LumaSize = u32Size;
        u32ChrmSize = 0;
    }
    else
    {
        u32Stride = SAMPLE_VI_GetRawStride(enPixelFormat, pstFrmCfg->u32Width, pstFrmCfg->u32ByteAlign);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height;
    }

    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
    stVbPoolCfg.u64BlkSize  = u32Size;
    stVbPoolCfg.u32BlkCnt   = s32FrmCnt;
    stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
    u32PoolId = AR_MPI_VB_CreatePool(&stVbPoolCfg);
    if (VB_INVALID_POOLID == u32PoolId)
    {
        SAMPLE_PRT("AR_MPI_VB_CreatePool failed!\n");
        return AR_FAILURE;
    }

    for (i = 0; i < s32FrmCnt; i++)
    {
        VbBlk = AR_MPI_VB_GetBlock(u32PoolId, u32Size, AR_NULL);
        if (VB_INVALID_HANDLE == VbBlk)
        {
            SAMPLE_PRT("AR_MPI_VB_GetBlock err! size:%d\n", u32Size);
            return AR_FAILURE;
        }

        u64PhyAddr = AR_MPI_VB_Handle2PhysAddr(VbBlk);
        if (0 == u64PhyAddr)
        {
            SAMPLE_PRT("AR_MPI_VB_Handle2PhysAddr err!\n");
            return AR_FAILURE;
        }

        pVirAddr = (AR_U8 *)AR_MPI_SYS_Mmap(u64PhyAddr, u32Size);
        if (NULL == pVirAddr)
        {
            SAMPLE_PRT("AR_MPI_SYS_Mmap err!\n");
            return AR_FAILURE;
        }

        pastViFrameInfo[i].stVideoFrameInfo.u32PoolId = u32PoolId;
        pastViFrameInfo[i].stVideoFrameInfo.enModId = AR_ID_VI;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[0]   = u64PhyAddr;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[1]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[2]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[1] + u32ChrmSize;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0]   = (AR_U64)(AR_U64)pVirAddr;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[1]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0] + u32LumaSize;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[2]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[1] + u32ChrmSize;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[0]    = u32LStride;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[1]    = u32CStride;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[2]    = u32CStride;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Width        = pstFrmCfg->u32Width;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Height       = pstFrmCfg->u32Height;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enPixelFormat   = pstFrmCfg->enPixelFormat;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enVideoFormat   = pstFrmCfg->enVideoFormat;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enCompressMode  = pstFrmCfg->enCompressMode;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enField         = VIDEO_FIELD_FRAME;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enDynamicRange  = DYNAMIC_RANGE_SDR8;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enColorGamut    = COLOR_GAMUT_BT709;

        pastViFrameInfo[i].VbBlk   = VbBlk;
        pastViFrameInfo[i].u32Size = u32Size;
    }

    return AR_SUCCESS;
}

AR_S32 SAMPLE_VI_COMM_ReleaseFrameBlkInfo(AR_S32 s32FrmCnt, SAMPLE_VI_FRAME_INFO_S *pastViFrameInfo)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_U32 i;
    VB_POOL u32PoolId;
    VB_BLK VbBlk;
    AR_U32 u32Size;

    for (i = 0; i < s32FrmCnt; i++)
    {
        VbBlk = pastViFrameInfo[i].VbBlk;
        s32Ret = AR_MPI_VB_ReleaseBlock(VbBlk);
        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("AR_MPI_VB_ReleaseBlock block 0x%x failure\n", VbBlk);
        }

        u32Size = pastViFrameInfo[i].u32Size;
        s32Ret = AR_MPI_SYS_Munmap((AR_VOID*)(AR_U64)pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0], u32Size);
        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("AR_MPI_SYS_Munmap failure!\n");
        }
    }

    u32PoolId = pastViFrameInfo[0].stVideoFrameInfo.u32PoolId;
    AR_MPI_VB_DestroyPool(u32PoolId);

    return AR_SUCCESS;
}

AR_S32 SAMPLE_VI_PlanToSemi(AR_U8* pY, AR_S32 yStride,
                           AR_U8* pU, AR_S32 uStride,
                           AR_U8* pV, AR_S32 vStride,
                           AR_S32 picWidth, AR_S32 picHeight)
{
    AR_S32 i;
    AR_U8 *pTmpU, *ptu;
    AR_U8 *pTmpV, *ptv;

    AR_S32 s32HafW = uStride >> 1 ;
    AR_S32 s32HafH = picHeight >> 1 ;
    AR_S32 s32Size = s32HafW * s32HafH;

    pTmpU = ar_malloc(s32Size);
    ptu = pTmpU;
    pTmpV = ar_malloc(s32Size);
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    ar_free(ptu);
    ar_free(ptv);

    return AR_SUCCESS;
}

AR_S32 SAMPLE_VI_PlanToSemi_422(AR_U8* pY, AR_S32 yStride,
                               AR_U8* pU, AR_S32 uStride,
                               AR_U8* pV, AR_S32 vStride,
                               AR_S32 picWidth, AR_S32 picHeight)
{
    AR_S32 i;
    AR_U8 *pTmpU, *ptu;
    AR_U8 *pTmpV, *ptv;
    AR_S32 s32HafW = uStride >> 1;
    AR_S32 s32HafH = picHeight;
    AR_S32 s32Size = s32HafW * s32HafH;

    pTmpU = ar_malloc(s32Size);
    ptu = pTmpU;
    pTmpV = ar_malloc(s32Size);
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    ar_free(ptu);
    ar_free(ptv);

    return AR_SUCCESS;
}

AR_VOID SAMPLE_VI_ReadOneFrame_400( FILE* fp, AR_U8* pY, AR_U8* pU, AR_U8* pV,
                                   AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2)
{
    AR_U8 *pDst;
    AR_U32 u32Row;

    pDst = pY;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("file is EOF!\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

AR_VOID SAMPLE_VI_ReadOneFrame( FILE* fp, AR_U8* pY, AR_U8* pU, AR_U8* pV,
                               AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2)
{
    AR_U8 *pDst;
    AR_U32 u32Row;
    pDst = pY;

    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }
    pDst = pU;
    for (u32Row = 0; u32Row < height / 2; u32Row++)
    {
        fread(pDst, width / 2, 1, fp);
        pDst += stride2;
    }

    pDst = pV;
    for (u32Row = 0; u32Row < height / 2; u32Row++)
    {
        fread(pDst, width / 2, 1, fp);
        pDst += stride2;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("file is EOF!\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

AR_VOID SAMPLE_VI_ReadOneFrame_422( FILE* fp, AR_U8* pY, AR_U8* pU, AR_U8* pV,
                                   AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2)
{
    AR_U8 *pDst;
    AR_U32 u32Row;
    pDst = pY;

    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    pDst = pU;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width >> 1, 1, fp);
        pDst += stride2;
    }

    pDst = pV;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width >> 1, 1, fp);
        pDst += stride2;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("the End-of-file is reached\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

static AR_VOID SAMPLE_VI_COMM_ReadYuvFile(FILE *pfd, VIDEO_FRAME_INFO_S *pstVideoFrameInfo)
{
    if (pstVideoFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422)
    {
        SAMPLE_VI_ReadOneFrame_422(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                                   (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                                   pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                                   pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);

        sleep(1);

        SAMPLE_VI_PlanToSemi_422((AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0], pstVideoFrameInfo->stVFrame.u32Stride[0],
                                 (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], pstVideoFrameInfo->stVFrame.u32Stride[1],
                                 (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2], pstVideoFrameInfo->stVFrame.u32Stride[1],
                                 pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height);
    }
    else if (pstVideoFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)
    {
        SAMPLE_VI_ReadOneFrame(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                               (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                               pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                               pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);

        sleep(1);

        SAMPLE_VI_PlanToSemi((AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0], pstVideoFrameInfo->stVFrame.u32Stride[0],
                             (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], pstVideoFrameInfo->stVFrame.u32Stride[1],
                             (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2], pstVideoFrameInfo->stVFrame.u32Stride[1],
                             pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height);
    }
    else
    {
        SAMPLE_VI_ReadOneFrame_400(pfd, (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                                   (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                                   pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                                   pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);
    }
}

AR_S32 SAMPLE_COMM_VI_Load_UserPic(const char *pszYuvFile, VI_USERPIC_ATTR_S *pstUsrPic, SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo)
{
    FILE *pfd;
    SAMPLE_VI_FRAME_CONFIG_S stFrmCfg;
    AR_S32 s32Ret;

    stFrmCfg.u32Width = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.u32Width;
    stFrmCfg.u32Height = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.u32Height;
    stFrmCfg.u32ByteAlign = 0;
    stFrmCfg.enPixelFormat = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat;
    stFrmCfg.enCompressMode = COMPRESS_MODE_NONE;
    stFrmCfg.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stFrmCfg.enDynamicRange = DYNAMIC_RANGE_SDR8;

    s32Ret = SAMPLE_VI_GetFrameBlkInfo(&stFrmCfg, 1, pstViFrameInfo);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_VI_GetFrameBlkInfo failed!\n");
        return s32Ret;
    }

    memcpy(&pstUsrPic->unUsrPic.stUsrPicFrm, &pstViFrameInfo->stVideoFrameInfo, sizeof(VIDEO_FRAME_INFO_S));

    printf("====to ar_mipi_open YUV file: %s. \n", pszYuvFile);
    pfd = fopen(pszYuvFile, "rb");
    if (!pfd)
    {
        SAMPLE_PRT("ar_mipi_open file -> %s fail \n", pszYuvFile);
        goto EXIT;
    }

    SAMPLE_VI_COMM_ReadYuvFile(pfd, &pstUsrPic->unUsrPic.stUsrPicFrm);

    fclose(pfd);

    return AR_SUCCESS;

EXIT:
    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);
    return s32Ret;
}

AR_VOID SAMPLE_COMM_VI_Release_UserPic(SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo)
{
    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);
}

AR_S32 SAMPLE_COMM_VI_StopSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
	 AR_S32 			 s32Ret = AR_SUCCESS;

	 if (!pstViInfo)
	 {
		 SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
		 return AR_FAILURE;
	 }

	 s32Ret = SAMPLE_COMM_VI_StopIsp(pstViInfo);

	 if (s32Ret != AR_SUCCESS)
	 {
		 SAMPLE_PRT("SAMPLE_COMM_VI_StopIsp failed !\n");
		 return AR_FAILURE;
	 }

	 s32Ret = SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);

	 if (s32Ret != AR_SUCCESS)
	 {
		 SAMPLE_PRT("SAMPLE_COMM_VI_DestroyVi failed !\n");
		 return AR_FAILURE;
	 }

	 return s32Ret;
}

AR_S32 SAMPLE_COMM_VI_StartSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
	 AR_S32 			 s32Ret = AR_SUCCESS;

	 if (!pstViInfo)
	 {
		 SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
		 return AR_FAILURE;
	 }

	 //1.SAMPLE_COMM_VI_StartMIPI
	 s32Ret=AR_MPI_VI_SetMipiBindDev(pstViInfo->stDevInfo.ViDev, pstViInfo->stSnsInfo.MipiDev);

	 if (s32Ret != AR_SUCCESS)
	 {
		 SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
		return AR_FAILURE;
	 }

	 //SAMPLE_COMM_VI_SetMipiAttr
	 STRU_COMBO_DEV_ATTR_T	  stcomboDevAttr;
	 SAMPLE_COMM_VI_GetComboAttrBySns(pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.MipiDev, &stcomboDevAttr);
	 stcomboDevAttr.devno = pstViInfo->stSnsInfo.MipiDev;

	if(pstViInfo->stSnsInfo.mipi_ipi_fre>0){
	   stcomboDevAttr.mipi_attr.mipi_pix_clk=pstViInfo->stSnsInfo.mipi_ipi_fre/1000/1000;		   
	}
	if(pstViInfo->stSnsInfo.settle >0 )
	{
		stcomboDevAttr.mipi_attr.settle_count=pstViInfo->stSnsInfo.settle;
	}
	 if(VI_PARALLEL_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode || VI_PARALLEL_VPSS_PARALLEL == pstViInfo->stPipeInfo.enMastPipeMode)
	 {
		 //stcomboDevAttr.data_rate = MIPI_DATA_RATE_X2;
	 }
	 SAMPLE_PRT("MipiDev %d, SetMipiAttr enWDRMode: %d\n", pstViInfo->stSnsInfo.MipiDev, pstViInfo->stDevInfo.enWDRMode);

	 s32Ret=AR_MPI_VI_SetComboDevAttr(&stcomboDevAttr);

	 if (AR_SUCCESS != s32Ret)
	 {
		 SAMPLE_PRT("MIPI_SET_DEV_ATTR failed\n");
		return AR_FAILURE;
	 }

	 //2.SAMPLE_COMM_VI_SetParam 
	 VI_VPSS_MODE_S 	 stVIVPSSMode;
	 s32Ret = AR_MPI_SYS_GetVIVPSSMode(&stVIVPSSMode);

	 if (AR_SUCCESS != s32Ret)
	 {
		 SAMPLE_PRT("Get VI-VPSS mode Param failed with %#x!\n", s32Ret);

		 return AR_FAILURE;
	 }

	 VI_PIPE ViPipe    = pstViInfo->stPipeInfo.aPipe[0];
	 if (VI_OFFLINE_VPSS_ONLINE == pstViInfo->stPipeInfo.enMastPipeMode)
	 {
		 for (AR_S32 j = 0; j < VI_MAX_PIPE_NUM; j++)
		 {
			 stVIVPSSMode.aenMode[j] = VI_OFFLINE_VPSS_ONLINE;
		 }
	 }

	 stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
	 if (pstViInfo->stPipeInfo.bMultiPipe == AR_TRUE)
	 {
		 ViPipe = pstViInfo->stPipeInfo.aPipe[1];
		 if(ViPipe != -1)
		 {
			 stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
		 }

		 ViPipe = pstViInfo->stPipeInfo.aPipe[2];
		 if(ViPipe != -1)
		 {
			 stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
		 }

		 ViPipe = pstViInfo->stPipeInfo.aPipe[3];
		 if(ViPipe != -1)
		 {
			 stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
		 }
	 }

	 if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe))
	 {
		 ViPipe    = pstViInfo->stPipeInfo.aPipe[1];
		 if(ViPipe != -1)
		 {
			 stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stSnapInfo.enSnapPipeMode;
		 }
	 }
	 s32Ret = AR_MPI_SYS_SetVIVPSSMode(&stVIVPSSMode);

	 if (AR_SUCCESS != s32Ret)
	 {
		 SAMPLE_PRT("Set VI-VPSS mode Param failed with %#x!\n", s32Ret);

		 return AR_FAILURE;
	 }

	 //3.SAMPLE_COMM_VI_CreateVi
	 s32Ret = SAMPLE_COMM_VI_CreateSingleVi(pstViInfo);

	 if (s32Ret != AR_SUCCESS)
	 {
		 SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
		return AR_FAILURE;
	 }

	 //4.SAMPLE_COMM_VI_CreateIsp
	 s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);

	 if (s32Ret != AR_SUCCESS)
	 {
		 SAMPLE_PRT("SAMPLE_COMM_VI_StartIsp failed !\n");
		 return AR_FAILURE;
	 }

	 return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
