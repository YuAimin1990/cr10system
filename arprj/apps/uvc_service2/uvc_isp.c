

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

#include "mpi_isp.h"
#include "ar_comm_isp.h"
#include "ar_sns_ctrl.h"
#include "ar_ae_comm.h"
#include "ar_awb_comm.h"

//#include "util_common.h"
#include "uvc_sensor.h"
#include "uvc_isp.h"

#define MAX_SENSOR_NUM      8
#define ISP_MAX_DEV_NUM     8
#define UVC_CHECK_RET(express,name)\
do{\
    AR_S32 s32Ret;\
    s32Ret = express;\
    if (AR_SUCCESS != s32Ret)\
    {\
        printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, s32Ret);\
        return s32Ret;\
    }\
}while(0)


extern ISP_SNS_OBJ_S stSnsImx290Obj;
/*
extern ISP_SNS_OBJ_S stSnsImx290Obj1;
extern ISP_SNS_OBJ_S stSnsImx290Obj2;
extern ISP_SNS_OBJ_S stSnsImx290Obj3;

extern ISP_SNS_OBJ_S stSnsImx415Obj;
extern ISP_SNS_OBJ_S stSnsImx464Obj;
extern ISP_SNS_OBJ_S stSnsTp9930Obj;
extern ISP_SNS_OBJ_S stSnssc910gsObj;
*/
extern ISP_SNS_OBJ_S stSnssc132gsObj;

static pthread_t    g_IspPid[ISP_MAX_DEV_NUM] = {0};
static AR_U32       g_au32IspSnsId[ISP_MAX_DEV_NUM] = {0, 1};

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

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC132GS_2M_25FPS =
{
    {120, 0, 720, 1280},
    {960, 1280},
    25,
    AR_BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

AR_S32 UVC_ISP_GetIspAttrBySns(UVC_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S* pstPubAttr)
{
    printf("enSnsType=%d \n",enSnsType);
    switch (enSnsType)
    {
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
        case SC_SC132GS_MIPI_2M_25FPS_8BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC132GS_2M_25FPS, sizeof(ISP_PUB_ATTR_S));
            // pstPubAttr->stTiming.hblank = 280;
            // pstPubAttr->stTiming.vblank = 45;
            break;
		default:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
    }

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S* UVC_ISP_GetSnsObj(VI_PIPE ViPipe, UVC_SNS_TYPE_E enSnsType)
{
    switch (enSnsType)
    {
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            if(ViPipe == 0)
            {
                return &stSnsImx290Obj;
            }
#if 0
            if(ViPipe == 1)
            {
                return &stSnsImx290Obj1;
            }
            if(ViPipe == 2)
            {
                return &stSnsImx290Obj2;
            }
            if(ViPipe == 3)
            {
                return &stSnsImx290Obj3;
            }

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT:
        case SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1:
            return &stSnsImx415Obj;
        case TP9930_DVP_4VC_1080P_25FPS:
        case TP9930_DVP_4VC_1080P_30FPS:
            return &stSnsTp9930Obj;

        case SONY_IMX464_MIPI_4M_30FPS_12BIT:
        case SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx464Obj;

		case SC_SC910GS_MIPI_9M_20FPS_10BIT:
			return &stSnssc910gsObj;
#endif
        case SC_SC132GS_MIPI_2M_25FPS_8BIT:
            return &stSnssc132gsObj;
		default:
            return AR_NULL;
    }
}

static void* UVC_ISP_Thread(void* param)
{
    AR_S32 s32Ret;
    ISP_DEV IspDev;
    AR_CHAR szThreadName[20];

    IspDev = (ISP_DEV)param;

    snprintf(szThreadName, 20, "ISP%d_RUN", IspDev);
    prctl(PR_SET_NAME, szThreadName, 0,0,0);

    s32Ret = AR_MPI_ISP_Run(IspDev);

    if (AR_SUCCESS != s32Ret)
    {
        printf("AR_MPI_ISP_Run failed with %#x!\n", s32Ret);
    }

    return NULL;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/

AR_S32 UVC_ISP_Aelib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;

    stAeLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, AR_AE_LIB_NAME, sizeof(AR_AE_LIB_NAME));
    return AR_SUCCESS;
}

AR_S32 UVC_ISP_Aelib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;

    stAeLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, AR_AE_LIB_NAME, sizeof(AR_AE_LIB_NAME));
    return AR_SUCCESS;
}

AR_S32 UVC_ISP_Awblib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAwbLib;

    stAwbLib.s32Id = IspDev;
    strncpy(stAwbLib.acLibName, AR_AWB_LIB_NAME, sizeof(AR_AWB_LIB_NAME));
    return AR_SUCCESS;
}

AR_S32 UVC_ISP_Awblib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAwbLib;

    stAwbLib.s32Id = IspDev;
    strncpy(stAwbLib.acLibName, AR_AWB_LIB_NAME, sizeof(AR_AWB_LIB_NAME));
    return AR_SUCCESS;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
AR_S32 UVC_ISP_Run(ISP_DEV IspDev)
{
    AR_S32 s32Ret = 0;
    pthread_attr_t* pstAttr = NULL;

    s32Ret = pthread_create(&g_IspPid[IspDev], pstAttr, UVC_ISP_Thread, (AR_VOID*)IspDev);

    if (0 != s32Ret)
    {
        printf("create isp running thread failed!, error: %d, %s\r\n", s32Ret, strerror(s32Ret));
        goto out;
    }
	printf("wait isp run completed .............\n");
    pthread_join(g_IspPid[IspDev],NULL);
out:

    if (NULL != pstAttr)
    {
        pthread_attr_destroy(pstAttr);
    }

    return s32Ret;
}

AR_S32 UVC_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, UVC_SNS_TYPE_E enSnsType)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32    s32Ret = -1;

    pstSnsObj = UVC_ISP_GetSnsObj(IspDev, enSnsType);

    if (AR_NULL == pstSnsObj)
    {
        printf("sensor %d not exist!\n", enSnsType);
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
            printf("sensor_register_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        printf("sensor_register_callback failed with AR_NULL!\n");
    }

    return AR_SUCCESS;
}

AR_S32 UVC_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev, UVC_SNS_TYPE_E enSnsType)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32    s32Ret = -1;

    pstSnsObj = UVC_ISP_GetSnsObj(IspDev, enSnsType);

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
            printf("sensor_unregister_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        printf("sensor_unregister_callback failed with AR_NULL!\n");
    }

    return AR_SUCCESS;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
AR_VOID UVC_ISP_Stop(ISP_DEV IspDev)
{
    if (g_IspPid[IspDev])
    {
        AR_MPI_ISP_Exit(IspDev);
        pthread_join(g_IspPid[IspDev], NULL);
        //UVC_ISP_Awblib_UnCallback(IspDev);
        //UVC_ISP_Aelib_UnCallback(IspDev);
        //UVC_ISP_Sensor_UnRegiter_callback(IspDev, enSnsType);
        g_IspPid[IspDev] = 0;
    }

    return;
}

static ISP_SNS_TYPE_E UVC_GetSnsBusType(UVC_SNS_TYPE_E enSnsType)
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

AR_S32 UVC_ISP_BindSns(ISP_DEV IspDev, UVC_SNS_TYPE_E enSnsType, AR_S8 s8SnsDev)
{
    ISP_SNS_COMMBUS_U uSnsBusInfo;
    ISP_SNS_TYPE_E enBusType;
    const ISP_SNS_OBJ_S* pstSnsObj;
    AR_S32 s32Ret;

    pstSnsObj = UVC_ISP_GetSnsObj(IspDev, enSnsType);

    if (AR_NULL == pstSnsObj)
    {
        printf("sensor %d not exist!\n", enSnsType);
        return AR_FAILURE;
    }

    enBusType = UVC_GetSnsBusType(enSnsType);

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
            printf("set sensor bus info failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        printf("not support set sensor bus info!\n");
        return AR_FAILURE;
    }

    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
