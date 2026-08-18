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

#include "ar_common.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "pf_isp.h"
#include "ar_sns_ctrl.h"
#include "cfg_vio.h"

#define MAX_SENSOR_NUM      8
#define ISP_MAX_DEV_NUM     8
#define IPC_CHECK_RET(express,name)\
do{\
    AR_S32 s32Ret;\
    s32Ret = express;\
    if (AR_SUCCESS != s32Ret)\
    {\
        printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, s32Ret);\
        return s32Ret;\
    }\
}while(0)


extern ISP_SNS_OBJ_S stSnsIspIn0Obj;

static pthread_t    g_IspPid[ISP_MAX_DEV_NUM] = {0};
//static AR_U32       g_au32IspSnsId[ISP_MAX_DEV_NUM] = {0, 1};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC230AI_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};


ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS_WDR2TO1_LINE =
{
    {4, 0, 1920, 1080},
    {1952, 2678},
    25,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
    {4, 0, 1924, 1080},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_SLAVE_MIPI_2M_60FPS =
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
ISP_PUB_ATTR_S ISP_PUB_ATTR_TP9930_1080P =
{
    .stWndRect = {0, 0, 1920, 1080},
    .stSnsSize = {1920, 1080},
    .f32FrameRate = 25,
    .enBayer = AR_BAYER_RGGB,
    .enWDRMode = WDR_MODE_NONE,
    .u8SnsMode = 0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX464_4M_30FPS =
{
    {12, 10, 2688, 1520},
    {2712, 1538},
    30,
    AR_BAYER_RGGB,
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

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC910GS_9M_20FPS =
{
    {0, 0, 3840, 2336},
    {3840, 2336},
    20,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

#ifdef THERMAL_ENABLE
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

ISP_PUB_ATTR_S ISP_PUB_ATTR_NK1221A =
{
    .stWndRect = {0, 0, 768, 292},
    .stSnsSize = {768, 292},
    .f32FrameRate = 50,
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
#endif

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC530AI_5M_30FPS =
{
    {0, 0, 2880, 1616},
    {2880, 1616},
    30,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS04A10_4M_25FPS =
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
    {2562, 1440},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_2To1_LINE,
    0,
    {0, 0, 2562, 1440},
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS04A10_BINNING_25FPS =
{
    {0, 0, 1280, 720},
    {1280, 720},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX347_4M_30FPS =
{
    {12, 10, 2688, 1520},
    {2712, 1538},
    30,
    AR_BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    1,
    {0, 0, 2712, 1538}
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

ISP_PUB_ATTR_S ISP_PUB_ATTR_OV_OS05A20_2688x1944 =
{
    {0, 0, 2688, 1944},
    {2688, 1944},
    30,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
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

AR_S32 IPC_PF_ISP_GetIspAttrBySns(IPC_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S* pstPubAttr)
{
    SRTU_SENSOR_DEFAULT_ATTR_T stDefaultAttr = {0};
    
    PRINT_INFO("enSnsType=%d \n",enSnsType);
    switch (enSnsType)
    {
        case SC_SC230AI_MIPI_2M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC230AI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS_WDR2TO1_LINE, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX307_SLAVE_MIPI_2M_60FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_SLAVE_MIPI_2M_60FPS, sizeof(ISP_PUB_ATTR_S));
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
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1: //tmp
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_60FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1: //tmp
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX415_4K_60FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case TP9930_DVP_4VC_1080P_25FPS:
        case TP9930_DVP_4VC_1080P_30FPS:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_TP9930_1080P, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX464_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX464_4M_30FPS_WDR_2TO1, sizeof(ISP_PUB_ATTR_S));
            break;
		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC910GS_9M_20FPS, sizeof(ISP_PUB_ATTR_S));
            break;


#ifdef THERMAL_ENABLE
		case TIMO256_DVP_256_IR:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_TIMO256, sizeof(ISP_PUB_ATTR_S));
			break;
        
        case ISP_VIN_COMMON_INF:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR_SIM, sizeof(ISP_PUB_ATTR_S));
			break;

        case ELF1_DVP_256_IR:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_ELF1, sizeof(ISP_PUB_ATTR_S));
			break;
        
		case NK1221A_DVP_384_IR:
			memcpy(pstPubAttr, &ISP_PUB_ATTR_NK1221A, sizeof(ISP_PUB_ATTR_S));
			break;
#endif

	case SC_SC530AI_MIPI_5M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC530AI_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case OV_OS04A10_MIPI_4M_25FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_4M_25FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_4M_25FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;
        case OV_OS04A10_MIPI_BINNING_25FPS_10BIT:		
        case OV_OS04A10_MIPI_BINNING_25FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS04A10_BINNING_25FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SONY_IMX347_MIPI_4M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX347_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case GST417W_IR_400X308:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GST417W_400X308, sizeof(ISP_PUB_ATTR_S));           
            break;

        case ISP_VIN_IR_THERMAL_GST417W:
        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST417W:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 4;
            pstPubAttr->stWndRect.u32Width = 408;
            pstPubAttr->stWndRect.u32Height = 300;
            pstPubAttr->stSnsSize.u32Width = 408;
            pstPubAttr->stSnsSize.u32Height = 308;
            break;
        case ISP_VIN_IR_ISP0_GST417W:
        case ISP_VIN_IR_ISP1_GST417W:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 408;
            pstPubAttr->stWndRect.u32Height = 300;
            pstPubAttr->stSnsSize.u32Width = 408;
            pstPubAttr->stSnsSize.u32Height = 300;
            break;
        case GST212W4_IR_256X200:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GST212W4_256X200, sizeof(ISP_PUB_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_GST212W4:
        case ISP_VIN_IR_ISP0_WITH_THERMAL_GST212W4:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 4;
            pstPubAttr->stWndRect.u32Width = 256;
            pstPubAttr->stWndRect.u32Height = 192;
            pstPubAttr->stSnsSize.u32Width = 256 + 1;
            pstPubAttr->stSnsSize.u32Height = 200;
            break;

        case ISP_VIN_IR_ISP0_GST212W4:
        case ISP_VIN_IR_ISP1_GST212W4:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 256;
            pstPubAttr->stWndRect.u32Height = 192;
            pstPubAttr->stSnsSize.u32Width = 256;
            pstPubAttr->stSnsSize.u32Height = 192;
            break;
        case OV_OS05A20_2688x1944:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OV_OS05A20_2688x1944, sizeof(ISP_PUB_ATTR_S));
            break;
        case H3812C1SG_IR_444X336:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_H3812C1SG_444X336, sizeof(ISP_PUB_ATTR_S));
            break;
        case ISP_VIN_IR_THERMAL_H3812C1SG:
        case ISP_VIN_IR_ISP0_WITH_THERMAL_H3812C1SG:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 16;
            pstPubAttr->stWndRect.s32Y = 18;
            pstPubAttr->stWndRect.u32Width = 384;
            pstPubAttr->stWndRect.u32Height = 288;
            pstPubAttr->stSnsSize.u32Width = 444;
            pstPubAttr->stSnsSize.u32Height = 336;
            break;   
        case ISP_VIN_IR_ISP0_H3812C1SG:
        case ISP_VIN_IR_ISP1_H3812C1SG:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_ISP_VIN_IR, sizeof(ISP_PUB_ATTR_S));
            pstPubAttr->stWndRect.s32X = 0;
            pstPubAttr->stWndRect.s32Y = 0;
            pstPubAttr->stWndRect.u32Width = 384;
            pstPubAttr->stWndRect.u32Height = 288;
            pstPubAttr->stSnsSize.u32Width = 384;
            pstPubAttr->stSnsSize.u32Height = 288;
            break;
        case ISP_RAW_SIMULATION:
            if(stSnsIspIn0Obj.pfnGetDefaultAttr)
            {
                stSnsIspIn0Obj.pfnGetDefaultAttr(0, &stDefaultAttr);
                memcpy(pstPubAttr, &stDefaultAttr.stPubAttr, sizeof(*pstPubAttr));
                IPC_CFG_VIO_S * pstVio = IPC_CFG_VIO_GetParam();
                pstPubAttr->stWndRect = pstVio->vi[0].stPubAttr.stWndRect;
                pstPubAttr->stSnsSize = pstVio->vi[0].stPubAttr.stSnsSize;
                pstPubAttr->enBayer = pstVio->vi[0].stPubAttr.enBayer;
                break;
            }
            else
            {
                PRINT_ERR("Get ISP default attr failed.\n");
                return -1;
            }
            
        default:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
    }

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S* IPC_PF_ISP_GetSnsObj(VI_PIPE ViPipe, IPC_SNS_TYPE_E enSnsType)
{
    switch (enSnsType)
    {
        case ISP_RAW_SIMULATION:
            return &stSnsIspIn0Obj;
        default:
            PRINT_ERR("Only support raw simulation.\n");
            return AR_NULL;
    }
}

static void* IPC_PF_ISP_Thread(void* param)
{
    AR_S32 s32Ret;
    ISP_DEV IspDev;
    AR_CHAR szThreadName[20];
	AR_CHAR *pcInParam = (AR_CHAR *)param;

    IspDev = (ISP_DEV)(*pcInParam);

    snprintf(szThreadName, 20, "ISP%d_RUN", IspDev);
    prctl(PR_SET_NAME, szThreadName, 0,0,0);

    s32Ret = AR_MPI_ISP_Run(IspDev);

    if (AR_SUCCESS != s32Ret)
    {
        PRINT_ERR("AR_MPI_ISP_Run failed with %#x!\n", s32Ret);
    }

    return NULL;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
AR_S32 IPC_PF_ISP_Run(ISP_DEV IspDev)
{
    AR_S32 s32Ret = 0;
    pthread_attr_t* pstAttr = NULL;

    s32Ret = pthread_create(&g_IspPid[IspDev], pstAttr, IPC_PF_ISP_Thread, (AR_VOID*)&IspDev);

    if (0 != s32Ret)
    {
        PRINT_ERR("create isp running thread failed!, error: %d, %s\r\n", s32Ret, strerror(s32Ret));
        goto out;
    }
	PRINT_INFO("wait isp run completed .............");
    pthread_join(g_IspPid[IspDev],NULL);
out:

    if (NULL != pstAttr)
    {
        pthread_attr_destroy(pstAttr);
    }

    return s32Ret;
}

AR_S32 IPC_PF_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, IPC_SNS_TYPE_E enSnsType)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32    s32Ret = -1;

    pstSnsObj = IPC_PF_ISP_GetSnsObj(IspDev, enSnsType);

    if (AR_NULL == pstSnsObj)
    {
        PRINT_ERR("sensor %d not exist!\n", enSnsType);
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
            PRINT_ERR("sensor_register_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        PRINT_ERR("sensor_register_callback failed with AR_NULL!\n");
    }

    return AR_SUCCESS;
}

AR_S32 IPC_PF_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev, IPC_SNS_TYPE_E enSnsType)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32    s32Ret = -1;

    pstSnsObj = IPC_PF_ISP_GetSnsObj(IspDev, enSnsType);

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
            PRINT_ERR("sensor_unregister_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        PRINT_ERR("sensor_unregister_callback failed with AR_NULL!\n");
    }

    return AR_SUCCESS;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
AR_VOID IPC_PF_ISP_Stop(ISP_DEV IspDev)
{
    if (g_IspPid[IspDev])
    {
        AR_MPI_ISP_Exit(IspDev);
        //IPC_PF_ISP_Sensor_UnRegiter_callback(IspDev, enSnsType);
        g_IspPid[IspDev] = 0;
    }

    return;
}

static ISP_SNS_TYPE_E IPC_PF_GetSnsBusType(IPC_SNS_TYPE_E enSnsType)
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

AR_S32 IPC_PF_ISP_BindSns(ISP_DEV IspDev, IPC_SNS_TYPE_E enSnsType, AR_S8 s8SnsDev)
{
    ISP_SNS_COMMBUS_U uSnsBusInfo;
    ISP_SNS_TYPE_E enBusType;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32 s32Ret;

    pstSnsObj = IPC_PF_ISP_GetSnsObj(IspDev, enSnsType);

    if (AR_NULL == pstSnsObj)
    {
        PRINT_ERR("sensor %d not exist!\n", enSnsType);
        return AR_FAILURE;
    }

    enBusType = IPC_PF_GetSnsBusType(enSnsType);

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
            PRINT_ERR("set sensor bus info failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        PRINT_ERR("not support set sensor bus info!\n");
        return AR_FAILURE;
    }

    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
