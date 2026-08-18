

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
extern ISP_SNS_OBJ_S stSnsIspInInfObj;
extern ISP_SNS_OBJ_S stSnsIspInInfObj1;
extern ISP_SNS_OBJ_S stSnsTimo120Obj;
extern ISP_SNS_OBJ_S stSnsTimo256Obj;
extern ISP_SNS_OBJ_S stSnsTimo256afObj;
extern ISP_SNS_OBJ_S stSnsElf1Obj;

static pthread_t    g_IspPid[ISP_MAX_DEV_NUM] = {0};
static AR_U32       g_au32IspSnsId[ISP_MAX_DEV_NUM] = {0, 1};

SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM]=
{
    SENSOR0_TYPE,
    SENSOR1_TYPE,
    SENSOR2_TYPE,
    SENSOR3_TYPE,
    SENSOR4_TYPE,
    SENSOR5_TYPE,
    SENSOR6_TYPE,
    SENSOR7_TYPE
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
    {0, 0, 2560, 1440},
    {2560, 1440},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_2To1_LINE,
    0,
    {0, 0, 2560, 1440},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_TIMO256 =
{
    .stWndRect = {0, 0, 512, 200},
    .stSnsSize = {512, 200},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_ELF1 =
{
    .stWndRect = {0, 0, 520, 204},
    .stSnsSize = {520, 204},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_PLUG617 =
{
    .stWndRect = {0, 0, 640, 512},
    .stSnsSize = {1280, 512},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_TIMO120 =
{
    .stWndRect = {0, 0, 120*2, 90+2},
    .stSnsSize = {120*2, 90+2},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};


#define IR_SIM_WIDTH  256
#define IR_SIM_HEIGHT 192
ISP_PUB_ATTR_S ISP_PUB_ATTR_ISP_VIN_IR_SIM =
{
    .stWndRect = {0, 0, IR_SIM_WIDTH, IR_SIM_HEIGHT},
    .stSnsSize = {IR_SIM_WIDTH, IR_SIM_HEIGHT},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
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
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX290_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
		case ISP_VIN_0_MEM_2M_30FPS_12BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_0_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
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
            memcpy(pstPubAttr, &ISP_PUB_ATTR_TP9930_DEFAULT, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stSnsSize.u32Width = u32Width;
            pstPubAttr->stSnsSize.u32Height = u32Height;
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = u32Width;
            pstPubAttr->stWndRect.u32Height = u32Height;
            pstPubAttr->f32FrameRate = f32Fps;
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_60FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX464_4M_30FPS_WDR_2TO1, sizeof(ISP_PUB_ATTR_S));
            break;
			
		case OV_OS04A10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_4M_25FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;

		case TIMO256_DVP_256_IR:
		case TIMO256AF_DVP_256_IR:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_TIMO256, sizeof(ISP_PUB_ATTR_S));
			break;
        
        case ISP_VIN_COMMON_INF:
        case ISP_VIN_COMMON_INF_120x90:
        case ISP_VIN_COMMON_INF_384x288:
        case ISP_VIN_COMMON_INF_400x300:
        case ISP_VIN_COMMON_INF_640x512:
        case ISP_VIN_COMMON_INF_1280x1024:
        case ISP_VIN_COMMON_INF_12BIT:
        case ISP_VIN_COMMON_INF_120x90_12BIT:
        case ISP_VIN_COMMON_INF_384x288_12BIT:
        case ISP_VIN_COMMON_INF_400x300_12BIT:
        case ISP_VIN_COMMON_INF_640x512_12BIT:
        case ISP_VIN_COMMON_INF_1280x1024_12BIT:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR_SIM, sizeof(ISP_PUB_ATTR_S));
            if(enSnsType == ISP_VIN_COMMON_INF_640x512 || enSnsType == ISP_VIN_COMMON_INF_640x512_12BIT)
            {
                pstPubAttr->stWndRect.u32Width = 640;
                pstPubAttr->stWndRect.u32Height = 512;
                pstPubAttr->stSnsSize.u32Width = 640;
                pstPubAttr->stSnsSize.u32Height = 512;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF_384x288 || enSnsType == ISP_VIN_COMMON_INF_384x288_12BIT)
            {
                pstPubAttr->stWndRect.u32Width = 384;
                pstPubAttr->stWndRect.u32Height = 288;
                pstPubAttr->stSnsSize.u32Width = 384;
                pstPubAttr->stSnsSize.u32Height = 288;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF_400x300 || enSnsType == ISP_VIN_COMMON_INF_400x300_12BIT)
            {
                pstPubAttr->stWndRect.u32Width = 408;
                pstPubAttr->stWndRect.u32Height = 304;
                pstPubAttr->stSnsSize.u32Width = 408;
                pstPubAttr->stSnsSize.u32Height = 304;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF_1280x1024 || enSnsType == ISP_VIN_COMMON_INF_1280x1024_12BIT)
            {
                pstPubAttr->stWndRect.u32Width = 1280;
                pstPubAttr->stWndRect.u32Height = 1024;
                pstPubAttr->stSnsSize.u32Width = 1280;
                pstPubAttr->stSnsSize.u32Height = 1024;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF_120x90 || enSnsType == ISP_VIN_COMMON_INF_120x90_12BIT)
            {
                pstPubAttr->stWndRect.u32Width = 192;
                pstPubAttr->stWndRect.u32Height = 144;
                pstPubAttr->stSnsSize.u32Width = 192;
                pstPubAttr->stSnsSize.u32Height = 144;
            }
            if(f32Fps>0) pstPubAttr->f32FrameRate = f32Fps;
            printf("isp vin , config fps: %f , pub fps: %f\n",f32Fps, pstPubAttr->f32FrameRate);
			break;

		case ISP_VIN_COMMON_INF1:
		case ISP_VIN_COMMON_INF1_120x90:
        case ISP_VIN_COMMON_INF1_384x288:
        case ISP_VIN_COMMON_INF1_400x300:
        case ISP_VIN_COMMON_INF1_640x512:
        case ISP_VIN_COMMON_INF1_1280x1024:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR_SIM, sizeof(ISP_PUB_ATTR_S));
            if(enSnsType == ISP_VIN_COMMON_INF1_640x512)
            {
                pstPubAttr->stWndRect.u32Width = 640;
                pstPubAttr->stWndRect.u32Height = 512;
                pstPubAttr->stSnsSize.u32Width = 640;
                pstPubAttr->stSnsSize.u32Height = 512;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF1_384x288)
            {
                pstPubAttr->stWndRect.u32Width = 384;
                pstPubAttr->stWndRect.u32Height = 288;
                pstPubAttr->stSnsSize.u32Width = 384;
                pstPubAttr->stSnsSize.u32Height = 288;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF1_400x300)
            {
                pstPubAttr->stWndRect.u32Width = 408;
                pstPubAttr->stWndRect.u32Height = 304;
                pstPubAttr->stSnsSize.u32Width = 408;
                pstPubAttr->stSnsSize.u32Height = 304;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF1_1280x1024)
            {
                pstPubAttr->stWndRect.u32Width = 1280;
                pstPubAttr->stWndRect.u32Height = 1024;
                pstPubAttr->stSnsSize.u32Width = 1280;
                pstPubAttr->stSnsSize.u32Height = 1024;
            }
            else if(enSnsType == ISP_VIN_COMMON_INF1_120x90)
            {
                pstPubAttr->stWndRect.u32Width = 192;
                pstPubAttr->stWndRect.u32Height = 144;
                pstPubAttr->stSnsSize.u32Width = 192;
                pstPubAttr->stSnsSize.u32Height = 144;
            }
            if(f32Fps>0) pstPubAttr->f32FrameRate = f32Fps;
            printf("isp vin 1 , config fps: %f , pub fps: %f\n",f32Fps, pstPubAttr->f32FrameRate);
            break;

        case ELF1_DVP_256_IR:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_ELF1, sizeof(ISP_PUB_ATTR_S));
			break;

        case PLUG617_DVP_640x512_IR:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_PLUG617, sizeof(ISP_PUB_ATTR_S));
            break;

        case TIMO120_DVP_120x90_IR:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_TIMO120, sizeof(ISP_PUB_ATTR_S));
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
        case SONY_IMX307_MIPI_2M_25FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP:
        {
			
            if (u32SnsId >=1)
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
        case TP9930_DVP_SINGLE_CH:
        case TP9930_DVP_MULTIPLEX_2CH:
        case TP9930_DVP_MULTIPLEX_4CH:
            return &stSnsTp9930Obj;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            return &stSnsImx415Obj;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx464Obj;
		        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_10BIT:
            return &stSnsOs04a10Obj;
		case TIMO256_DVP_256_IR:
            return &stSnsTimo256Obj;
        case TIMO256AF_DVP_256_IR:
            return &stSnsTimo256afObj;
        case ISP_VIN_COMMON_INF:
        case ISP_VIN_COMMON_INF_120x90:
        case ISP_VIN_COMMON_INF_384x288:
        case ISP_VIN_COMMON_INF_400x300:
        case ISP_VIN_COMMON_INF_640x512:
        case ISP_VIN_COMMON_INF_1280x1024:
        case ISP_VIN_COMMON_INF_12BIT:
        case ISP_VIN_COMMON_INF_120x90_12BIT:
        case ISP_VIN_COMMON_INF_384x288_12BIT:
        case ISP_VIN_COMMON_INF_400x300_12BIT:
        case ISP_VIN_COMMON_INF_640x512_12BIT:
        case ISP_VIN_COMMON_INF_1280x1024_12BIT:
            return &stSnsIspInInfObj;
        case ISP_VIN_COMMON_INF1:
        case ISP_VIN_COMMON_INF1_120x90:
        case ISP_VIN_COMMON_INF1_384x288:
        case ISP_VIN_COMMON_INF1_400x300:
        case ISP_VIN_COMMON_INF1_640x512:
        case ISP_VIN_COMMON_INF1_1280x1024:
            return &stSnsIspInInfObj1;
        case ELF1_DVP_256_IR:
            return &stSnsElf1Obj;
        case PLUG617_DVP_640x512_IR:
            return &stSnsElf1Obj;
        case TIMO120_DVP_120x90_IR:
            return &stSnsTimo120Obj;
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
