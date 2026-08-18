

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

#include "sample_comm.h"

#define MAX_SENSOR_NUM      9
#define ISP_MAX_DEV_NUM     6

extern ISP_SNS_OBJ_S stSnsImx290Obj;
extern ISP_SNS_OBJ_S stSnsIspIn0Obj;
extern ISP_SNS_OBJ_S stSnsImx290Obj1;
extern ISP_SNS_OBJ_S stSnsTp9930Obj;
extern ISP_SNS_OBJ_S stSnsImx415Obj;
extern ISP_SNS_OBJ_S stSnsImx464Obj;
extern ISP_SNS_OBJ_S stSnsOs04a10Obj;
extern ISP_SNS_OBJ_S stSnsGc2093Obj;
extern ISP_SNS_OBJ_S stSnssc530aiObj;
extern ISP_SNS_OBJ_S stSnsIspInLObj;
extern ISP_SNS_OBJ_S stSnsIspInRObj;
extern ISP_SNS_OBJ_S stSnsMicroiiiObj;
extern ISP_SNS_OBJ_S stSnsImx347Obj;
extern ISP_SNS_OBJ_S stSnsGst417wObj;
extern ISP_SNS_OBJ_S stSnsGst412cObj;
extern ISP_SNS_OBJ_S stSnsGst612cObj;
extern ISP_SNS_OBJ_S stSnsGst212w4Obj;
extern ISP_SNS_OBJ_S stSnsH3812c1sgObj;
extern ISP_SNS_OBJ_S stSnsRtd6122cObj;
extern ISP_SNS_OBJ_S stSnsRtd3172cObj;
extern ISP_SNS_OBJ_S stSnsIspInInfThermalObj;
extern ISP_SNS_OBJ_S stSnsIspInInfObj;
extern ISP_SNS_OBJ_S stSnsIspInInfObj1;
extern ISP_SNS_OBJ_S stSnsSc2210Obj;
extern ISP_SNS_OBJ_S stSnsOs05a20Obj;
extern ISP_SNS_OBJ_S stSnsPlecoObj;
extern ISP_SNS_OBJ_S stSnsGc2053Obj;
extern ISP_SNS_OBJ_S stSnsGN2Obj;
static pthread_t    g_IspPid[ISP_MAX_DEV_NUM] = {0};
static AR_U32       g_au32IspSnsId[ISP_MAX_DEV_NUM] = {0, 1};

SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM]=
{
    SENSOR0_TYPE,
    SENSOR1_TYPE,
    SENSOR2_TYPE,
    SENSOR3_TYPE,
    SENSOR4_TYPE
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC_SC2210_1080P =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_PLECO_1920_2880_30FPS =
{
    {0, 0, 1920, 2880},
    {1920, 2880},
    30,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OV_OS05A20_2688x1944 =
{
    {0, 0, 2688, 1944},
    {2688, 1944},
    30,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OV_OS05A20_2336x1752 =
{
    {0, 0, 2336, 1752},
    {2336, 1752},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX290_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX290_2M_30FPS_VC_RAW =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW,
};


ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX290_2M_25FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};


ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS_BIG_PIC =
{
    {0, 0, 1280, 1080},
    {1280, 1080},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};
ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS_WDR2TO1_LINE_BIG_PIC =
{
    {0, 0, 1056, 1080},
    {1184, 2678},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
    {16, 0, 1060, 1080},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS_WDR2TO1_LINE_BIG_PIC_R =
{
    {0, 0, 1056, 1080},
    {1184, 2678},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
    {112, 0, 1060, 1080},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_0_48M_5FPS_BIG_PIC =
{
    {0, 0, 4064, 6048},
    {4064, 6048},
    5,
    AR_BAYER_GBRG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX290_MIPI_2M_30FPS_WDR2TO1_LINE =
{
    {4, 0, 1920, 1080},
    {1952, 2678},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
    {4, 0, 1924, 1080},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX290_MIPI_2M_30FPS_WDR2TO1_LINE_BIG_PIC =
{
    {0, 0, 1952, 2678},
    {1952, 2678},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
    {0, 0, 1952, 2678},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX290_SLAVE_MIPI_2M_60FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    60,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX334_4K_30FPS =
{
    {0, 0, 3840, 2160},
    {3840, 2160},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX334_4K_30FPS_WDR2TO1_LINE =
{
    {0, 0, 3840, 2160},
    {3840, 2160},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX377_4K_30FPS =
{
    {0, 0, 3840, 2160},
    {3850, 2160},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_TP9930_DEFAULT =
{
    .stWndRect = {0, 0, 1920, 1080},
    .stSnsSize = {1920, 1080},
    .f32FrameRate = -1,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX415_4M_HIGHT_SPEED =
{
    {0, 0, 2688, 1520},
    {2688, 1520},
    30,
    AR_BAYER_GBRG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX415_WDR_2TO1 =
{
	 {0, 0, 2688, 1520},
    {2688, 1520},
    10,
    AR_BAYER_GBRG,
    WDR_MODE_2To1_LINE,
    0,
	{0, 0, 2688, 1520}
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX415_4K_30FPS =
{
    {12, 22, 3840, 2160},
    {3864, 2192},
    30,
    AR_BAYER_GBRG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX415_4K_60FPS =
{
    {12, 22, 3840, 2160},
    {3864, 2192},
    60,
    AR_BAYER_GBRG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX464_4M_30FPS_WDR_2TO1 =
{
    {12, 10, 2688, 1520},
    {2712, 1538},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    1,
    {0, 0, 2712, 1538}
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX347_4M_30FPS =
{
    {12, 10, 2688, 1520},
    {2712, 1538},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    1,
    {0, 0, 2712, 1538}
};


ISP_PUB_ATTR_S ISP_PUB_ATTR_OS04A10_4M_30FPS =
{
    {0, 0, 2560, 1440},
    {2560, 1440},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS04A10_4M_25FPS_WDR2TO1 =
{
    {0, 0, 2562, 1440},
    {2562, 1440},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_2To1_LINE,
    0,
    {0, 0, 2562, 1440},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2093_1080P_30FPS =
{
    .stWndRect = {0, 0, 1920, 1080},
    .stSnsSize = {1920, 1080},
    .f32FrameRate = 30,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2053_1080P_30FPS =
{
    .stWndRect = {0, 0, 1920, 1080},
    .stSnsSize = {1920, 1080},
    .f32FrameRate = 30,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2093_1080P_30FPS_WDR2TO1 =
{
    .stWndRect = {0, 0, 1920, 1080},
    .stSnsSize = {1922, 1080},
    .f32FrameRate = 30,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_2To1_LINE,
    .u8SnsMode = 1,
    .stWdrRect = {0, 0, 1920, 1080},
};



ISP_PUB_ATTR_S ISP_PUB_ATTR_SC530AI_5M_30FPS =
{
    {0, 0, 2880, 1616},
    {2880, 1616},
    30,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IRAY_PAL_DEFAULT =
{
    .stWndRect = {0, 0, 720, 288},
    .stSnsSize = {720, 288},
    .f32FrameRate = 50,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GST417W_400X308 =
{
    .stWndRect = {0, 0, 400, 308},
    .stSnsSize = {400, 308},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_H3812C1SG_444X336 =
{
    .stWndRect = {0, 0, 444, 336},
    .stSnsSize = {444, 336},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_RTD6122C_656X522 =
{
    .stWndRect = {0, 0, 656, 522},
    .stSnsSize = {656, 522},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_RTD3172C_394X298 =
{
    .stWndRect = {0, 0, 394, 298},
    .stSnsSize = {394, 298},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GST412C_400X308 =
{
    .stWndRect = {0, 0, 400, 308},
    .stSnsSize = {400, 308},
    .f32FrameRate = 50,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GST612C_640X520 =
{
    .stWndRect = {0, 0, 640, 520},
    .stSnsSize = {640, 520},
    .f32FrameRate = 50,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GST212W4_256X200 =
{
    .stWndRect = {0, 0, 256, 200},
    .stSnsSize = {256, 200},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_IR =
{
    .stWndRect = {0, 0, 640, 512},
    .stSnsSize = {640, 512},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_S5KGN2SP_4K_30FPS =
{
    .stWndRect = {0, 0, 3840, 2160},
    .stSnsSize = {3840, 2160},
    .f32FrameRate = 30,
    .enBayer = AR_BAYER_GBRG,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_S5KGN2SP_4K_60FPS =
{
    .stWndRect = {0, 0, 3840, 2160},
    .stSnsSize = {3840, 2160},
    .f32FrameRate = 60,
    .enBayer = AR_BAYER_GBRG,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_S5KGN2SP_2K7_60FPS =
{
    .stWndRect = {0, 0, 2704, 1520},
    .stSnsSize = {2704, 1520},
    .f32FrameRate = 60,
    .enBayer = AR_BAYER_GBRG,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

AR_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, AR_U32 u32Width,
    AR_U32 u32Height, AR_FLOAT f32Fps, ISP_PUB_ATTR_S* pstPubAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_25FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:		
    	case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            // if f32Fps == 0, fps is 25 by default; otherwise, fps is set by f32Fps.
            if (f32Fps > 0) {
                pstPubAttr->f32FrameRate = f32Fps;
            }
			if(u32Width>0 && u32Height >0)
			{
    			pstPubAttr->stSnsSize.u32Width = u32Width;
                pstPubAttr->stSnsSize.u32Height = u32Height;
			}
            break;
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS_VC_RAW, sizeof(ISP_PUB_ATTR_S));
            // if f32Fps == 0, fps is 25 by default; otherwise, fps is set by f32Fps.
            if (f32Fps > 0) {
                pstPubAttr->f32FrameRate = f32Fps;
            }
			pstPubAttr->stSnsSize.u32Width = u32Width;
            pstPubAttr->stSnsSize.u32Height = u32Height;
			break;
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->f32FrameRate = 60;
            break;
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
			break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
        memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS_BIG_PIC, sizeof(ISP_PUB_ATTR_S));
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
        memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS_WDR2TO1_LINE_BIG_PIC, sizeof(ISP_PUB_ATTR_S));
        break;
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
        memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS_WDR2TO1_LINE_BIG_PIC_R, sizeof(ISP_PUB_ATTR_S));
        break;
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
        memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_0_48M_5FPS_BIG_PIC, sizeof(ISP_PUB_ATTR_S));
        break;
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
        memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_MIPI_2M_30FPS_WDR2TO1_LINE_BIG_PIC, sizeof(ISP_PUB_ATTR_S));
        break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_MIPI_2M_30FPS_WDR2TO1_LINE, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_SLAVE_MIPI_2M_60FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX334_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX334_4K_30FPS_WDR2TO1_LINE, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX377_MIPI_8M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX377_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
			break;
		
        case TP9930_DVP_SINGLE_CH:
        case TP9930_DVP_MULTIPLEX_2CH:
        case TP9930_DVP_MULTIPLEX_4CH:
        case TP9930_DVP_MULTIPLEX_4CH_BT656:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_TP9930_DEFAULT, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stSnsSize.u32Width = u32Width;
            pstPubAttr->stSnsSize.u32Height = u32Height;
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = u32Width;
            pstPubAttr->stWndRect.u32Height = u32Height;
            pstPubAttr->f32FrameRate = f32Fps;
            break;
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_WDR_2TO1, sizeof(ISP_PUB_ATTR_S));
			break;
		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4M_HIGHT_SPEED, sizeof(ISP_PUB_ATTR_S));
			break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_60FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX464_4M_30FPS_WDR_2TO1, sizeof(ISP_PUB_ATTR_S));
            break;

		case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_4M_25FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2093_1080P_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2093_1080P_30FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;


        case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC530AI_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case IRAY_PAL_DVP_SINGLE_CH:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IRAY_PAL_DEFAULT, sizeof(ISP_PUB_ATTR_S));
            break;

		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
			 memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX347_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
			break;	

        case GST417W_IR_400X308:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GST417W_400X308, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_GST417W:
        case ISP_VIN1_IR_GST417W_WITH_THERMAL:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 4;
            pstPubAttr->stWndRect.u32Width = 408;
            pstPubAttr->stWndRect.u32Height = 300;
            pstPubAttr->stSnsSize.u32Width = 408;
            pstPubAttr->stSnsSize.u32Height = 308;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN1_IR_GST417W:
        case ISP_VIN2_IR_GST417W:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 408;
            pstPubAttr->stWndRect.u32Height = 300;
            pstPubAttr->stSnsSize.u32Width = 408;
            pstPubAttr->stSnsSize.u32Height = 300;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case H3812C1SG_IR_444X336:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_H3812C1SG_444X336, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_H3812C1SG:
        case ISP_VIN1_IR_H3812C1SG_WITH_THERMAL:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 16;
            pstPubAttr->stWndRect.s32Y = 18;
            pstPubAttr->stWndRect.u32Width = 384;
            pstPubAttr->stWndRect.u32Height = 288;
            pstPubAttr->stSnsSize.u32Width = 444;
            pstPubAttr->stSnsSize.u32Height = 336;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN1_IR_H3812C1SG:
        case ISP_VIN2_IR_H3812C1SG:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 384;
            pstPubAttr->stWndRect.u32Height = 288;
            pstPubAttr->stSnsSize.u32Width = 384;
            pstPubAttr->stSnsSize.u32Height = 288;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case RTD6122C_IR_656X522:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_RTD6122C_656X522, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_RTD6122C:
        case ISP_VIN1_IR_RTD6122C_WITH_THERMAL:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 10;
            pstPubAttr->stWndRect.s32Y = 8;
            pstPubAttr->stWndRect.u32Width = 640;
            pstPubAttr->stWndRect.u32Height = 512;
            pstPubAttr->stSnsSize.u32Width = 656;
            pstPubAttr->stSnsSize.u32Height = 522;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN1_IR_RTD6122C:
        case ISP_VIN2_IR_RTD6122C:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 640;
            pstPubAttr->stWndRect.u32Height = 512;
            pstPubAttr->stSnsSize.u32Width = 640;
            pstPubAttr->stSnsSize.u32Height = 512;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case RTD3172C_IR_394X298:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_RTD3172C_394X298, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_RTD3172C:
        case ISP_VIN1_IR_RTD3172C_WITH_THERMAL:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 4;
            pstPubAttr->stWndRect.s32Y = 8;
            pstPubAttr->stWndRect.u32Width = 384;
            pstPubAttr->stWndRect.u32Height = 288;
            pstPubAttr->stSnsSize.u32Width = 394;
            pstPubAttr->stSnsSize.u32Height = 298;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN1_IR_RTD3172C:
        case ISP_VIN2_IR_RTD3172C:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 384;
            pstPubAttr->stWndRect.u32Height = 288;
            pstPubAttr->stSnsSize.u32Width = 384;
            pstPubAttr->stSnsSize.u32Height = 288;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case GST412C_IR_400X308:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GST412C_400X308, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case GST612C_IR_640X520:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GST612C_640X520, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_GST612C:
        case ISP_VIN1_IR_GST612C_WITH_THERMAL:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 4;
            pstPubAttr->stWndRect.u32Width = 640;
            pstPubAttr->stWndRect.u32Height = 512;
            pstPubAttr->stSnsSize.u32Width = 640;
            pstPubAttr->stSnsSize.u32Height = 520;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN1_IR_GST612C:
        case ISP_VIN2_IR_GST612C:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 640;
            pstPubAttr->stWndRect.u32Height = 512;
            pstPubAttr->stSnsSize.u32Width = 640;
            pstPubAttr->stSnsSize.u32Height = 512;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case SC_SC2210_1080P:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC_SC2210_1080P, sizeof(ISP_PUB_ATTR_S));
            break;

        case OV_OS05A20_2688x1944:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OV_OS05A20_2688x1944, sizeof(ISP_PUB_ATTR_S));
            break;

        case OV_OS05A20_2336x1752:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OV_OS05A20_2336x1752, sizeof(ISP_PUB_ATTR_S));
            break;

        case GST212W4_IR_256X200:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GST212W4_256X200, sizeof(ISP_PUB_ATTR_S));
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_GST212W4:
        case ISP_VIN1_IR_GST212W4_WITH_THERMAL:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 4;
            pstPubAttr->stWndRect.u32Width = 256;
            pstPubAttr->stWndRect.u32Height = 192;
            pstPubAttr->stSnsSize.u32Width = 256;
            pstPubAttr->stSnsSize.u32Height = 200;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN1_IR_GST212W4:
        case ISP_VIN2_IR_GST212W4:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 256;
            pstPubAttr->stWndRect.u32Height = 192;
            pstPubAttr->stSnsSize.u32Width = 256;
            pstPubAttr->stSnsSize.u32Height = 192;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

        case ISP_VIN_IR_THERMAL_OFFLINE:
        case ISP_VIN1_IR_OFFLINE_WITH_THERMAL:
        case ISP_VIN1_IR_OFFLINE:
        case ISP_VIN2_IR_OFFLINE:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = u32Width;
            pstPubAttr->stWndRect.u32Height = u32Height;
            pstPubAttr->stSnsSize.u32Width = u32Width;
            pstPubAttr->stSnsSize.u32Height = u32Height;
            if (f32Fps > 0)
                pstPubAttr->f32FrameRate = f32Fps;
            break;

		case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_S5KGN2SP_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
			break;
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_S5KGN2SP_4K_60FPS, sizeof(ISP_PUB_ATTR_S));
			break;
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_S5KGN2SP_2K7_60FPS, sizeof(ISP_PUB_ATTR_S));
			break;
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_PLECO_1920_2880_30FPS, sizeof(ISP_PUB_ATTR_S));
			pstPubAttr->stSnsSize.u32Width = u32Width;
            pstPubAttr->stSnsSize.u32Height = u32Height;
            break;
        default:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
    }

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S* SAMPLE_COMM_ISP_GetSnsObj(AR_U32 u32SnsId)
{
    SAMPLE_SNS_TYPE_E enSnsType;
    enSnsType = g_enSnsType[u32SnsId];

	SAMPLE_PRT("enSnsType %d u32SnsId=%d\n", enSnsType,u32SnsId);

    switch (enSnsType)
    {
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_60FPS_12BIT:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
		case SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW:
        {
			
            if (u32SnsId >= 1)
            {
            	SAMPLE_PRT("sensor obj stSnsImx290Obj1 \n");
                return &stSnsImx290Obj1;
            }
            else
            {
            	SAMPLE_PRT("sensor obj stSnsImx290Obj \n");
                return &stSnsImx290Obj;
            }
        }
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:{
			SAMPLE_PRT("sensor obj stSnsIspIn0Obj \n");
			return &stSnsIspIn0Obj;
		}
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_BIG_PIC:
    case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1_BIG_PIC:
    {
        SAMPLE_PRT("sensor obj stSnsImx290Obj \n");
        return &stSnsImx290Obj;
    }
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC:
    case ISP_VIN_0_MEM_2M_30FPS_12BIT_WDR2TO1_LINE_BIG_PIC_R:
    case ISP_VIN_0_MEM_48M_5FPS_12BIT_BIG_PIC:
    {
        if (u32SnsId == 1)
        {
            SAMPLE_PRT("sensor obj stSnsIspInRObj \n");
            return &stSnsIspInRObj;
        }
        else
        {
            SAMPLE_PRT("sensor obj stSnsIspInLObj \n");
            return &stSnsIspInLObj;
        }
    }
        case TP9930_DVP_SINGLE_CH:
        case TP9930_DVP_MULTIPLEX_2CH:
        case TP9930_DVP_MULTIPLEX_4CH:
        case TP9930_DVP_MULTIPLEX_4CH_BT656:
            return &stSnsTp9930Obj;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
		case SONY_IMX415_MIPI_HIGH_SPEED_12BIT:
		case SONY_IMX415_MIPI_WDR2TO1_VC_12BIT:
            return &stSnsImx415Obj;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
		case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1_VC_RAW:
			SAMPLE_PRT("sensor obj stSnsImx464Obj \n");
            return &stSnsImx464Obj;
		        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
            return &stSnsOs04a10Obj;
        case GC_GC2093_MIPI_1080P_30FPS_10BIT:
        case GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1:
            return &stSnsGc2093Obj;
         case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            return &stSnssc530aiObj;
        case IRAY_PAL_DVP_SINGLE_CH:
            return &stSnsMicroiiiObj;
		case SONY_IMX347_MIPI_4M_25FPS_12BIT:
			return &stSnsImx347Obj;
        case GST417W_IR_400X308:
            return &stSnsGst417wObj;
        case GST412C_IR_400X308:
            return &stSnsGst412cObj;
        case GST612C_IR_640X520:
            return &stSnsGst612cObj;
        case H3812C1SG_IR_444X336:
            return &stSnsH3812c1sgObj;
        case RTD6122C_IR_656X522:
            return &stSnsRtd6122cObj;
        case RTD3172C_IR_394X298:
            return &stSnsRtd3172cObj;
        case GST212W4_IR_256X200:
            return &stSnsGst212w4Obj;
        case ISP_VIN_IR_THERMAL_GST417W:
        case ISP_VIN_IR_THERMAL_H3812C1SG:
        case ISP_VIN_IR_THERMAL_RTD6122C:
        case ISP_VIN_IR_THERMAL_RTD3172C:
        case ISP_VIN_IR_THERMAL_GST612C:
        case ISP_VIN_IR_THERMAL_GST212W4:
        case ISP_VIN_IR_THERMAL_OFFLINE:
            return &stSnsIspInInfThermalObj;
        case ISP_VIN1_IR_GST417W:
        case ISP_VIN1_IR_GST417W_WITH_THERMAL:
        case ISP_VIN1_IR_H3812C1SG:
        case ISP_VIN1_IR_H3812C1SG_WITH_THERMAL:
        case ISP_VIN1_IR_RTD6122C:
        case ISP_VIN1_IR_RTD6122C_WITH_THERMAL:
        case ISP_VIN1_IR_RTD3172C:
        case ISP_VIN1_IR_RTD3172C_WITH_THERMAL:
        case ISP_VIN1_IR_GST612C:
        case ISP_VIN1_IR_GST612C_WITH_THERMAL:
        case ISP_VIN1_IR_GST212W4:
        case ISP_VIN1_IR_GST212W4_WITH_THERMAL:
        case ISP_VIN1_IR_OFFLINE:
        case ISP_VIN1_IR_OFFLINE_WITH_THERMAL:
            return &stSnsIspInInfObj;
        case ISP_VIN2_IR_GST417W:
        case ISP_VIN2_IR_H3812C1SG:
        case ISP_VIN2_IR_RTD6122C:
        case ISP_VIN2_IR_RTD3172C:
        case ISP_VIN2_IR_GST612C:
        case ISP_VIN2_IR_GST212W4:
        case ISP_VIN2_IR_OFFLINE:
            return &stSnsIspInInfObj1;
    case SC_SC2210_1080P:{
        SAMPLE_PRT("sensor obj stSnsSc2210Obj \n");
        return &stSnsSc2210Obj;
    }
    case OV_OS05A20_2336x1752:
    case OV_OS05A20_2688x1944:{
        SAMPLE_PRT("sensor obj stSnsSc2210Obj \n");
        return &stSnsOs05a20Obj;
    }
		case OB_PLECO_MIPI_1920x2880_30FPS_10BIT:
            return &stSnsPlecoObj;
		case GC_GC2053_MIPI_1080P_30FPS_10BIT:        
            return &stSnsGc2053Obj;
		case SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT:
		case SAMSUNG_S5KGN2SP_MIPI_2K7_60FPS_10BIT:
		//se SAMSUNG_S5KGN2SP_MIPI_1080P_60FPS_10BIT:
			return &stSnsGN2Obj;
        default:
            return AR_NULL;
    }
}

static void* SAMPLE_COMM_ISP_Thread(void* param)
{
    AR_S32 s32Ret;
    ISP_DEV IspDev;
    AR_CHAR szThreadName[20];

    IspDev = (ISP_DEV)((AR_S64)param);

    snprintf(szThreadName, 20, "ISP%d_RUN", IspDev);
    prctl(PR_SET_NAME, szThreadName, 0,0,0);

    SAMPLE_PRT("ISP Dev %d running !\n", IspDev);
    s32Ret = AR_MPI_ISP_Run(IspDev);

    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_ISP_Run failed with %#x!\n", s32Ret);
    }

    return NULL;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/



/******************************************************************************
* funciton : ISP Run
******************************************************************************/
AR_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev)
{
    AR_S32 s32Ret = 0;
    pthread_attr_t* pstAttr = NULL;

    s32Ret = pthread_create(&g_IspPid[IspDev], pstAttr, SAMPLE_COMM_ISP_Thread, (AR_VOID*)((AR_S64)IspDev));

    if (0 != s32Ret)
    {
        SAMPLE_PRT("create isp running thread failed!, error: %d, %s\r\n", s32Ret, strerror(s32Ret));
        goto out;
    }
	SAMPLE_PRT("wait isp run completed .............");
    pthread_join(g_IspPid[IspDev],NULL);
out:

    if (NULL != pstAttr)
    {
        pthread_attr_destroy(pstAttr);
    }

    return s32Ret;
}

AR_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, AR_U32 u32SnsId)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32    s32Ret = -1;

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("invalid sensor id: %d\n", u32SnsId);
        return AR_FAILURE;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (AR_NULL == pstSnsObj)
    {
        SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
        return AR_FAILURE;
    }

    stAeLib.s32Id = IspDev;
    stAwbLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, AR_AE_LIB_NAME, sizeof(AR_AE_LIB_NAME));
    strncpy(stAwbLib.acLibName, AR_AWB_LIB_NAME, sizeof(AR_AWB_LIB_NAME));
  //  strncpy(stAfLib.acLibName, AR_AF_LIB_NAME, sizeof(AR_AF_LIB_NAME));

    if (pstSnsObj->pfnRegisterCallback != AR_NULL)
    {
        s32Ret = pstSnsObj->pfnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("sensor_register_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("sensor_register_callback failed with AR_NULL!\n");
    }

    g_au32IspSnsId[IspDev] = u32SnsId;

    return AR_SUCCESS;
}

AR_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    AR_U32 u32SnsId;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32    s32Ret = -1;

    u32SnsId = g_au32IspSnsId[IspDev];

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("%s: invalid sensor id: %d\n", __FUNCTION__, u32SnsId);
        return AR_FAILURE;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (AR_NULL == pstSnsObj)
    {
        return AR_FAILURE;
    }

    stAeLib.s32Id = IspDev;
    stAwbLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, AR_AE_LIB_NAME, sizeof(AR_AE_LIB_NAME));
    strncpy(stAwbLib.acLibName, AR_AWB_LIB_NAME, sizeof(AR_AWB_LIB_NAME));
 //   strncpy(stAfLib.acLibName, AR_AF_LIB_NAME, sizeof(AR_AF_LIB_NAME));

    if (pstSnsObj->pfnUnRegisterCallback != AR_NULL)
    {
        s32Ret = pstSnsObj->pfnUnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("sensor_unregister_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("sensor_unregister_callback failed with AR_NULL!\n");
    }

    return AR_SUCCESS;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
AR_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev)
{
    if (g_IspPid[IspDev])
    {
        AR_MPI_ISP_Exit(IspDev);
        SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
        g_IspPid[IspDev] = 0;
    }

    return;
}

AR_VOID SAMPLE_COMM_All_ISP_Stop(AR_VOID)
{
    ISP_DEV IspDev;

    for (IspDev = 0; IspDev < ISP_MAX_DEV_NUM; IspDev++)
    {
        SAMPLE_COMM_ISP_Stop(IspDev);
    }
}

static ISP_SNS_TYPE_E SAMPLE_COMM_GetSnsBusType(SAMPLE_SNS_TYPE_E enSnsType)
{
    ISP_SNS_TYPE_E enBusType;

    switch (enSnsType)
    {
        default:
            enBusType = ISP_SNS_I2C_TYPE;
            break;
    }

    return enBusType;
}

AR_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, AR_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType, AR_S8 s8SnsDev)
{
    ISP_SNS_COMMBUS_U uSnsBusInfo;
    ISP_SNS_TYPE_E enBusType;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32 s32Ret;

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("invalid sensor id: %d\n", u32SnsId);
        return AR_FAILURE;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (AR_NULL == pstSnsObj)
    {
        SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
        return AR_FAILURE;
    }

    enBusType = SAMPLE_COMM_GetSnsBusType(enSnsType);

    if (ISP_SNS_I2C_TYPE == enBusType)
    {
        uSnsBusInfo.s8I2cDev = s8SnsDev;
    }
    else
    {
        uSnsBusInfo.s8SspDev.bit4SspDev = s8SnsDev;
        uSnsBusInfo.s8SspDev.bit4SspCs  = 0;
    }

    if (AR_NULL != pstSnsObj->pfnSetBusInfo)
    {
        s32Ret = pstSnsObj->pfnSetBusInfo(IspDev, uSnsBusInfo);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("set sensor bus info failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("not support set sensor bus info!\n");
        return AR_FAILURE;
    }

    return s32Ret;
}
AR_S32 SAMPLE_COMM_ISP_Set_Focus(ISP_DEV IspDev, AR_U32 u32SnsId,AR_S32 s32Focus)
{
  
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32 s32Ret;
    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("invalid sensor id: %d\n", u32SnsId);
        return AR_FAILURE;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (AR_NULL == pstSnsObj)
    {
        SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
        return AR_FAILURE;
    }
    if (AR_NULL != pstSnsObj->pfnSetFocus)
    {
       
        SAMPLE_PRT("s32Focus %d IspDev %d u32SnsId %d\n", s32Focus,IspDev,u32SnsId);
        s32Ret = pstSnsObj->pfnSetFocus(IspDev, s32Focus);

        if (s32Ret != AR_SUCCESS)
        {
            SAMPLE_PRT("set sensor bus info failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    return AR_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
