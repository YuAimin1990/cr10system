#include <ctype.h>

#include "uvc_service2.h"
#include "uvc_sensor.h"
#include "uvc_isp.h"

#include "ar_comm_sys.h"
#include "ar_comm_vb.h"
#include "ar_comm_isp.h"
#include "ar_comm_vi.h"
#include "ar_comm_vo.h"
#include "ar_comm_venc.h"
#include "ar_comm_vdec.h"
#include "ar_comm_vpss.h"
#include "ar_comm_region.h"
#include "ar_comm_adec.h"
#include "ar_comm_aenc.h"
#include "ar_comm_ai.h"
#include "ar_comm_ao.h"
#include "ar_comm_aio.h"
#include <ar_comm_video.h>
#include "ar_defines.h"
#include "ar_comm_hdmi.h"
#include "ar_comm_venc.h"
#include "ar_combo_dev.h"
#include "ar_mipi_tx.h"
//#include "sample_comm.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_audio.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "ar_math.h"
#include "ar_sns_ctrl.h"
#include "mpi_hdmi.h"

#include "hal_sys.h"
#include "hal_gpio.h"

#include "uvc_isp.h"

/*#include "ar_video_stream.h"
#include "ar_pipeline_ctrl.h"
#include "pipeline_ctl.h"
#include "ar_lancher.h"
#include "ar_cam_src.h"*/

#include "pipeline_service.h"

#define COLOR_RGB_BLUE     0x0000FF
#define IPC_MAX_VO_WINDOW_NUM  9

typedef struct
{
    int args_num;
    char args[16][128];
}cmd_args_t;

//static IPC_STREAM_THREAD_STATUS s_astVThreadStatus[IPC_TOTAL_STREAM_NUM] = {0};
//static pthread_mutex_t g_ViMutex[8];
//static pthread_mutex_t g_VpssMutex[8 * 5];
//static pthread_mutex_t g_VencMutex[32];//we use 32 venc chn


//const char encoder_hevc[]   = {"hevc_encoder"};
//const char encoder_avc[]    = {"avc_encoder"};
//const char encoder_mjpg[]   = {"jpeg_encoder"};

void ar_uvc_get_context(void** context, uint32_t* context_size)
{
    return;
}

int uvc_find_pra_index_by_name(char *name,int argc, char *argv[])
{
    int i=0;
    for(i=0;i<argc;i++)
    {
        if(!strcmp(name,argv[i]))
        {
            return i;
        }
    }
    return -1;
}

/*static int find_cmd_pra_index_by_name(char *name,cmd_args_t *argv)
{
    int i=0;
    for(i=0;i<argv->args_num;i++)
    {
        if(!strcmp(name,argv->args[i]))
        {
            return i;
        }
    }
    return -1;
}*/

cmd_args_t *uvc_get_args(const char *cmd)
{
    cmd_args_t * args=(cmd_args_t *)malloc(sizeof(cmd_args_t));
    char *p_cmd=(char *)malloc(1024);
    char *p_str=NULL;
    char  *split_char=" ";
    char *p_free_p_cmd=p_cmd;
    memset(args,0,sizeof(cmd_args_t));
    strcpy(p_cmd,cmd);
    p_str=strtok(p_cmd,split_char);
    if(p_str)
    {
        strcpy(&args->args[args->args_num][0],p_str);
        args->args_num++;
    }

    while(p_str)
    {
        p_str=strtok(NULL,split_char);
        if(p_str)
        {
            strcpy(&args->args[args->args_num][0],p_str);
            args->args_num++;
        }
    }
    free(p_free_p_cmd);
    return args;
}

AR_VOID UVC_MPI_VIN_OpenDev(AR_S32 mode, int hdr_fre,
                                        int vif_fre, int isp_fre, int mipi_fre)
{
   VI_DEV_PROP_S Prop={0};
   if(mode==0){
      Prop.cam_mode=VIN_CAMERA_NORMAL;
   }else if(mode==1)
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }else if(mode==2)
   {
      Prop.cam_mode=VIN_CMAERA_MULTI_MODE;
   }else
   {
      Prop.cam_mode=VIN_CAMERA_OFFLINE;
   }
   //cfg the fre
   Prop.hdr_fre_mod=1;
   Prop.hdr_fre_hz=hdr_fre;
   Prop.vif_fre_mod=1;
   Prop.vif_fre_hz=vif_fre;
   Prop.isp_fre_mod=1;
   Prop.isp_fre_hz=isp_fre;
   Prop.mipi_fre_mod=1;
   Prop.mipi_fre_hz=mipi_fre;

    printf("%s %d: vif_fre = %d, isp_fre = %d mipi_fre = %d\n", __func__, __LINE__, Prop.vif_fre_hz, Prop.isp_fre_hz, Prop.mipi_fre_hz);

   AR_MPI_VIN_OpenDev(&Prop);
   return;
}

static UVC_SNS_TYPE_E UVC_VI_GetSensorType(AR_CHAR * name, AR_CHAR WDREnable, AR_CHAR u8Fps)
{
    if(strcmp(name, "imx307") == 0)
    {
        if(u8Fps > 30)
        {
            printf("No setting for %s %d\n", name, u8Fps);
            return UVC_SNS_TYPE_BUTT;
        }

        if(WDREnable)
        {
            return SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1;
        }
        else
        {
            return SONY_IMX307_MIPI_2M_30FPS_12BIT;
        }
    }

    if(strcmp(name, "imx415") == 0)
    {
        if(WDREnable)
        {
            if(u8Fps <= 30)
            {
                return SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1;
            }
            else
            {
                return SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1;
            }
        }
        else
        {
            if(u8Fps <= 30)
            {
                return SONY_IMX415_MIPI_8M_30FPS_12BIT;
            }
            else
            {
                return SONY_IMX415_MIPI_8M_60FPS_12BIT;
            }
        }
    }

    if(strcmp(name, "imx464") == 0)
    {
        if(WDREnable)
        {
            return SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1;
        }
        else
        {
            return SONY_IMX464_MIPI_4M_30FPS_12BIT;
        }
    }

    if(strcmp(name, "sc910gs") == 0)
    {
        if(WDREnable)
        {
            //return SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1;
        }
        else
        {
            return SC_SC910GS_MIPI_9M_20FPS_10BIT;
        }
    }

    if(strcmp(name, "tp9930") == 0)
    {
        if(u8Fps == 25)
        {
            return TP9930_DVP_4VC_1080P_25FPS;
        }
        else
        {
            return TP9930_DVP_4VC_1080P_30FPS;
        }
    }
    if(strcmp(name, "sc132gs") == 0){
        return SC_SC132GS_MIPI_2M_25FPS_8BIT;
    }

    printf("sensor %s not supported.\n", name);

    return UVC_SNS_TYPE_BUTT;
}

//for ar93xx platform, gpio name to gpio num
AR_S32 UVC_GPIO_Trans(AR_CHAR * strGpio, ENUM_GPIO_GROUP * pGrp, ENUM_GPIO_PORT * pPort, AR_U32 * pNum)
{
    AR_CHAR chPort = 0;
    unsigned int chGrp = 0, chNum = 0;

    if(!strGpio || !pGrp || !pPort || !pNum)
    {
        printf("GPIO trans null pointer.\n");
        return -1;
    }

    //remove space
    while(*strGpio == 0x20) {strGpio++;}

    printf("GPIO %s\n", strGpio);
    //left should be like "A1_0"
    if(strlen(strGpio) < 4)
    {
        printf("Invalid gpio: %s\n", strGpio);
        return -1;
    }

#ifdef PROXIMA
    sscanf(strGpio, "%c%u_%u", &chPort, &chGrp, &chNum);
    chPort = toupper(chPort);

    *pPort = PORT_A + chPort - 'A';
    *pGrp = chGrp;
    *pNum = chNum;
#else
    chPort = toupper(strGpio[0]);
    if(chPort < 'A' || chPort > 'D')
    {
        printf("Invalid gpio : %s, the port should be from A to D\n", strGpio);
        return -1;
    }
    *pPort = PORT_A + chPort - 'A';

    chGrp = strGpio[1];
    if(chGrp < '0' || chGrp > '3')
    {
        printf("Invalid gpio : %s, the grp should be from 0 to 3\n", strGpio);
        return -1;
    }
    *pGrp = GROUP_0 + chGrp - '0';

    chNum = strGpio[3];
    if(chNum < '0' || chNum > '7')
    {
        printf("Invalid gpio : %s, the num should be from 0 to 7\n", strGpio);
        return -1;
    }
    *pNum = chNum - '0';
#endif
    printf("GPIO port %d grp %d num %d\n", *pPort, *pGrp, *pNum);

    return 0;
}

//here to config and start mipi
//For mipi device, we set vipipe = mipi_id
//For dvp device, we use IPC_PF_PIPE_2_DVP_ID .
static AR_S32 UVC_VI_StartMipi(VI_PIPE ViPipe, UVC_SNS_TYPE_E enSensorType, AR_CHAR * strPowerGpio, AR_CHAR * strResetGpio)
{
    AR_S32 s32Ret = AR_SUCCESS;
    STRU_COMBO_DEV_ATTR_T ComboAttr={0};
    ENUM_GPIO_GROUP enGrp = 0;
    ENUM_GPIO_PORT enPort = 0;
    AR_U32 u32GpioNum = 0;

    UVC_VI_GetComboAttrBySns(enSensorType, 0, &ComboAttr);

    s32Ret = UVC_GPIO_Trans(strPowerGpio, &enGrp, &enPort, &u32GpioNum);
    if(s32Ret == 0) //else use default.
    {
        ComboAttr.dev_power_attr.power_gpio[0] = enGrp;
        ComboAttr.dev_power_attr.power_gpio[1] = enPort;
        ComboAttr.dev_power_attr.power_gpio[2] = u32GpioNum;
    }

    s32Ret = UVC_GPIO_Trans(strResetGpio, &enGrp, &enPort, &u32GpioNum);
    if(s32Ret == 0) //else use default.
    {
        ComboAttr.dev_power_attr.reset_gpio[0] = enGrp;
        ComboAttr.dev_power_attr.reset_gpio[1] = enPort;
        ComboAttr.dev_power_attr.reset_gpio[2] = u32GpioNum;
    }

    // ComboAttr.devno = ViPipe;

    s32Ret=AR_MPI_VI_SetComboDevAttr(&ComboAttr);

    if (s32Ret != AR_SUCCESS)
    {
        printf("ar_mipi_ioctl AR_MIPI_SET_DEV_ATTR fail %#x!\n", s32Ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

static AR_S32 UVC_VI_CreateSingleVi(VI_DEV ViDev, VI_PIPE ViPipe,
              UVC_SNS_TYPE_E enSensorType, WDR_MODE_E enWDRMode, ENUM_VFE_MODE enVfe,
              AR_CHAR * strPowerGpio, AR_CHAR * strResetGpio)
{
    AR_S32 s32Ret = AR_SUCCESS;
    VI_DEV_ATTR_S stViDevAttr = {0};
    VI_DEV_BIND_PIPE_S stDevBindPipe = {0};
    VI_PIPE_ATTR_S stPipeAttr = {0};
    // VI_CHN_ATTR_S stChnAttr = {0};
    // VI_CHN ViChn = 0;

    uvc_server_t    *server_l   = get_uvc_server();

    UVC_VI_GetDevAttrBySns(enSensorType, &stViDevAttr);
    stViDevAttr.stWDRAttr.enWDRMode = enWDRMode;

    stDevBindPipe.u32Num = 1;
    stDevBindPipe.PipeId[0] = ViPipe;

    s32Ret = AR_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);

    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret=UVC_VI_StartMipi(ViPipe, enSensorType, strPowerGpio, strResetGpio);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_VI_StartMipi failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_EnableDev(ViDev);

    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);

    UVC_VI_GetPipeAttrBySns(enSensorType, &stPipeAttr);
    stPipeAttr.enVfeMode = enVfe;
    stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;

    if (strcmp(server_l->sensor_name, "imx307") == 0)
    {
        stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
        stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_DISABLE;
    }
    else
    {
        stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
    }
    s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
        goto EXIT1;
    }

    return AR_SUCCESS;
/*
    UVC_VI_GetChnAttrBySns(enSensorType, &stChnAttr);
    stChnAttr.u32BufCount = 5;

    //must start channel 0 for a pipe
    s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    //enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;
    s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);

    if (s32Ret != AR_SUCCESS)
    {
        printf("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }
    return AR_SUCCESS;

EXIT2:

    s32Ret = AR_MPI_VI_StopPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }
*/

EXIT1:
    s32Ret  = AR_MPI_VI_DisableDev(ViDev);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return s32Ret;
}

AR_S32 UVC_VI_StopSingleVi(VI_DEV ViDev, VI_PIPE ViPipe)
{
    AR_S32 s32Ret = 0;
    s32Ret = AR_MPI_VI_StopPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VI_DestroyPipe(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret  = AR_MPI_VI_DisableDev(ViDev);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static AR_S32 UVC_VI_CreateSingleIsp(VI_DEV ViDev, VI_PIPE ViPipe,
             UVC_SNS_TYPE_E enSnsType, AR_U32 u32BusId, WDR_MODE_E enWDRMode, AR_U32 u32Fps)
{
    //AR_S32              i;
    //AR_BOOL             bNeedPipe;
    AR_S32              s32Ret = AR_SUCCESS;
    ISP_PUB_ATTR_S      stPubAttr;
    //VI_PIPE_ATTR_S      stPipeAttr;

    UVC_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);
    stPubAttr.enWDRMode = enWDRMode;

    s32Ret = UVC_ISP_Sensor_Regiter_callback(ViPipe, enSnsType);
    if (AR_SUCCESS != s32Ret)
    {
        printf("register sensor %d to ISP %d failed\n", ViDev, ViPipe);
        return AR_FAILURE;
    }

    s32Ret = UVC_ISP_BindSns(ViPipe, enSnsType, u32BusId);
    if (AR_SUCCESS != s32Ret)
    {
        printf("register sensor %d bus id %d failed\n", ViDev, u32BusId);
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        return AR_FAILURE;
    }

    s32Ret = UVC_ISP_Aelib_Callback(ViPipe);
    if (AR_SUCCESS != s32Ret)
    {
        printf("UVC_ISP_Aelib_Callback failed\n");
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        return AR_FAILURE;
    }

    s32Ret = UVC_ISP_Awblib_Callback(ViPipe);
    if (AR_SUCCESS != s32Ret)
    {
        printf("UVC_ISP_Awblib_Callback failed\n");
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        UVC_ISP_Aelib_UnCallback(ViPipe);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_ISP_MemInit(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("Init Ext memory failed with %#x!\n", s32Ret);
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        UVC_ISP_Aelib_UnCallback(ViPipe);
        UVC_ISP_Awblib_UnCallback(ViPipe);
        return AR_FAILURE;
    }

    stPubAttr.f32FrameRate = u32Fps;
    s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("SetPubAttr failed with %#x!\n", s32Ret);
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        UVC_ISP_Aelib_UnCallback(ViPipe);
        UVC_ISP_Awblib_UnCallback(ViPipe);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_ISP_Init(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("ISP Init failed with %#x!\n", s32Ret);
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        UVC_ISP_Aelib_UnCallback(ViPipe);
        UVC_ISP_Awblib_UnCallback(ViPipe);
        return AR_FAILURE;
    }

    s32Ret = UVC_ISP_Run(ViPipe);
    if (s32Ret != AR_SUCCESS)
    {
        printf("ISP Run failed with %#x!\n", s32Ret);
        UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);
        UVC_ISP_Aelib_UnCallback(ViPipe);
        UVC_ISP_Awblib_UnCallback(ViPipe);
        return AR_FAILURE;
    }

    return s32Ret;
}

AR_S32 UVC_VI_StopSingleIsp(VI_PIPE ViPipe, UVC_SNS_TYPE_E enSnsType)
{
    UVC_ISP_Stop(ViPipe);

    UVC_ISP_Awblib_UnCallback(ViPipe);
    UVC_ISP_Aelib_UnCallback(ViPipe);
    UVC_ISP_Sensor_UnRegiter_callback(ViPipe, enSnsType);

    return 0;
}

int UVC_VI_Init(void * handle,int argc, char *argv[])
{
    //int             index   = 0;
    //int             width   = 1920, height = 1080;
    int             s32Ret  = -1;

    VI_DEV          ViDev       = 0;
    VI_PIPE         ViPipe      = 0;
    VI_CHN          ViChn1      = 1;
    VI_CHN          ViChn2      = 2;

    UVC_SNS_TYPE_E  enSnsType   = SONY_IMX307_MIPI_2M_30FPS_12BIT;
    VI_CHN_ATTR_S   stChnAttr   = {0};

    uvc_server_t    *server_l   = get_uvc_server();

    ViDev = server_l->cam_index;
    ViPipe = server_l->cam_index;

    UVC_MPI_VIN_OpenDev(server_l->multi ? 2 : 0, server_l->hdr_fre, server_l->vif_fre, server_l->isp_fre, server_l->mipi_fre);

    enSnsType = UVC_VI_GetSensorType(server_l->sensor_name, server_l->wdr_mode, server_l->fps);

    /*start vi*/
    if( strcmp(server_l->sensor_name, "sc132gs") == 0)   // sc132gs mipi 1
        s32Ret = AR_MPI_VI_SetMipiBindDev(ViDev, 1); //this must refer to the hardware design
    else
        s32Ret = AR_MPI_VI_SetMipiBindDev(ViDev, ViPipe);
    if(s32Ret)
    {
        printf("Mipi bind dev %d/%d error!\n", ViDev, ViPipe);
        goto EXIT_0;
    }

    //Create VI
    s32Ret = UVC_VI_CreateSingleVi(ViDev, ViPipe, enSnsType, server_l->wdr_mode, server_l->vfe_mode,
        server_l->power_gpio, server_l->reset_gpio);
    if (AR_SUCCESS != s32Ret)
    {
        goto EXIT_0;
    }

    s32Ret = UVC_VI_CreateSingleIsp(ViDev, ViPipe, enSnsType, server_l->i2c_bus, server_l->wdr_mode, server_l->fps);
    if (AR_SUCCESS != s32Ret)
    {
        goto EXIT_1;
    }


    UVC_VI_GetChnAttrBySns(enSnsType, &stChnAttr);
    stChnAttr.u32BufCount = 5;

    if(strcmp(server_l->sensor_name, "imx307") == 0)
    {
        stChnAttr.stSize.u32Width = server_l->height[0];
        stChnAttr.stSize.u32Height = server_l->width[0];
    }
    else
    {
        stChnAttr.stSize.u32Width = server_l->width[0];
        stChnAttr.stSize.u32Height = server_l->height[0];
    }

    //must start channel 0 for a pipe
    s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_SetChnAttr pipe %d chn %d failed with %#x!\n", ViPipe, ViChn1, s32Ret);
        goto EXIT_2;
    }

    //enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;
    s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VI_EnableChn %d failed! ret = 0x%x\n", ViChn1, s32Ret);
        goto EXIT_2;
    }

    if(server_l->use_display)
    {
        /*start vi chn1*/
        s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr); //get attr from channel 0.
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT_2;
        }
        stChnAttr.stSize.u32Width = server_l->disp_w;
        stChnAttr.stSize.u32Height = server_l->disp_h;
        stChnAttr.u32Depth = 2;
        stChnAttr.u32BufCount = 5;

        s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT_2;
        }

        s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT_2;
        }
    }

    if(server_l->is_raw)
    {
        ISP_PUB_ATTR_S stPubAttr = {0};
        UVC_ISP_GetIspAttrBySns(enSnsType, &stPubAttr);

        /*start vi chn2*/
        s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr); //get attr from channel 0.
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT_3;
        }

        stChnAttr.stSize.u32Width = stPubAttr.stSnsSize.u32Width;
        stChnAttr.stSize.u32Height = stPubAttr.stSnsSize.u32Height;
        stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
        stChnAttr.u32Depth = 2;

        s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn2, &stChnAttr);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT_3;
        }

        s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn2);
        if (AR_SUCCESS != s32Ret)
        {
            printf("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
            goto EXIT_3;
        }
    }

    return 0;

EXIT_3:
    if(server_l->use_display)
        AR_MPI_VI_DisableChn(ViPipe, ViChn1);
EXIT_2:
    UVC_VI_StopSingleIsp(ViPipe, enSnsType);
EXIT_1:
    UVC_VI_StopSingleVi(ViDev, ViPipe);
EXIT_0:

    return -1;
}

int UVC_VI_UnInit()
{
    VI_DEV          ViDev       = 0;
    VI_PIPE         ViPipe      = 0;
    VI_CHN          ViChn1      = 1;
    VI_CHN          ViChn2      = 2;

    UVC_SNS_TYPE_E  enSnsType   = SONY_IMX307_MIPI_2M_30FPS_12BIT;
    uvc_server_t    *server_l   = get_uvc_server();

    ViDev = server_l->cam_index;
    ViPipe = server_l->cam_index;
    enSnsType = UVC_VI_GetSensorType(server_l->sensor_name, server_l->wdr_mode, server_l->fps);

    if(server_l->is_raw)
        AR_MPI_VI_DisableChn(ViPipe, ViChn2);

    if(server_l->use_display)
        AR_MPI_VI_DisableChn(ViPipe, ViChn1);

    UVC_VI_StopSingleIsp(ViPipe, enSnsType);

    UVC_VI_StopSingleVi(ViDev, ViPipe);


    return 0;
}

AR_S32 UVC_VPSS_StartGrp(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S * pstVpssGrpAttr, AR_U32 u32ChnBmp, VPSS_CHN_ATTR_S * pastChnAttr)
{
    VPSS_CHN VpssChn;
    AR_S32 s32Ret;
    AR_S32 i;

    s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return AR_FAILURE;
    }

    for (i = 0; i < 5; i++)
    {
        if(u32ChnBmp & (1<<i) )
        {
            VpssChn = i;
            s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastChnAttr[VpssChn]);
            if (s32Ret != AR_SUCCESS)
            {
                printf("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }

            s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != AR_SUCCESS)
            {
                printf("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return AR_FAILURE;
            }
        }
    }

    return AR_SUCCESS;
}

AR_S32 UVC_VPSS_StopGrp(VPSS_GRP VpssGrp, AR_U32 u32ChnBmp)
{
    AR_S32 i;
    AR_S32 s32Ret = AR_SUCCESS;
    VPSS_CHN VpssChn;

    for (i = 0; i < 5; i++)
    {
        if(u32ChnBmp & (1<<i))
        {
            VpssChn = i;
            s32Ret = AR_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
            if (s32Ret != AR_SUCCESS)
            {
                printf("failed with %#x!\n", s32Ret);
                return AR_FAILURE;
            }
        }
    }

    s32Ret = AR_MPI_VPSS_StopGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        printf("failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != AR_SUCCESS)
    {
        printf("failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}


AR_S32 UVC_VPSS_Init(void * handle,int argc, char *argv[])
{
    VPSS_GRP_ATTR_S    stVpssGrpAttr = {0};
    AR_U32             u32ChnBmp = 0;
    VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    AR_S32 s32Ret = 0;
    AR_U32 i = 0;

    uvc_server_t    *server_l = get_uvc_server();

    /*config vpss*/
    stVpssGrpAttr.enDynamicRange                    = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat                     = PIXEL_FORMAT_YVU_PLANAR_420;
    stVpssGrpAttr.enCompressMode                    = COMPRESS_MODE_NONE;
    stVpssGrpAttr.enVideoFormat                     = VIDEO_FORMAT_LINEAR;
    stVpssGrpAttr.u32Depth                          = 2;
    stVpssGrpAttr.stAspectRatio.enMode              = ASPECT_RATIO_NONE;
    stVpssGrpAttr.u32ScaleMode                      = 0;
    stVpssGrpAttr.enChnMode                         = VPSS_CHN_MODE_USER;

    //set grp input
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate       = server_l->fps;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate       = server_l->fps;
    stVpssGrpAttr.u32Width                          = server_l->width[0];
    stVpssGrpAttr.u32Height                         = server_l->height[0];

    stVpssGrpAttr.bFlip                             = 0;
    stVpssGrpAttr.bMirror                           = 0;
    stVpssGrpAttr.u32ScaleMode                      = 1; //1 bilinear, 0 bicubic
    stVpssGrpAttr.enRotation                        = strcmp(server_l->sensor_name, "imx307") == 0 ? ROTATION_90 : ROTATION_0; // cam0 imx307 rotate 90, cam1 sc132gs do not rotate

    u32ChnBmp = 0;
    //chn 0 for uvc stream
    //chn 1 for alg
    for(i = 0; i < 2; i++)
    {
        astVpssChnAttr[i].bFlip = 0;
        astVpssChnAttr[i].bMirror = 0;
        astVpssChnAttr[i].enChnMode = VPSS_CHN_MODE_USER;
        astVpssChnAttr[i].u32Width = server_l->width[0];
        astVpssChnAttr[i].u32Height = server_l->height[0];
        astVpssChnAttr[i].enCompressMode = COMPRESS_MODE_NONE;
        astVpssChnAttr[i].enDynamicRange = DYNAMIC_RANGE_SDR8;
        astVpssChnAttr[i].enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
        astVpssChnAttr[i].enVideoFormat = VIDEO_FORMAT_LINEAR;
        astVpssChnAttr[i].stAspectRatio.enMode = ASPECT_RATIO_NONE;
        astVpssChnAttr[i].stFrameRate.s32SrcFrameRate = stVpssGrpAttr.stFrameRate.s32SrcFrameRate;
        astVpssChnAttr[i].stFrameRate.s32DstFrameRate = stVpssGrpAttr.stFrameRate.s32DstFrameRate;
        astVpssChnAttr[i].u32Depth = 2;
        astVpssChnAttr[i].u32ScaleMode = 1; //1 bilinear mode

        u32ChnBmp |= (1 << i);

        //pthread_mutex_init(&g_VpssMutex[i], 0);
    }

    astVpssChnAttr[1].u32Width = server_l->width[1];
    astVpssChnAttr[1].u32Height = server_l->height[1];

    s32Ret = UVC_VPSS_StartGrp(server_l->cam_index, &stVpssGrpAttr, u32ChnBmp, astVpssChnAttr);
    if(s32Ret)
    {
        printf("Start vpss grp %d failed.\n", i);
        return s32Ret;
    }

    return 0;
}

AR_S32 UVC_VPSS_UnInit()
{
    AR_S32          s32Ret      = 0;
    AR_U32          i           = 0;
    AR_U32          u32ChnBmp   = 0;
    uvc_server_t   *server_l    = get_uvc_server();

    u32ChnBmp = 0;
    for(i = 0; i < 2; i++)
    {
        u32ChnBmp |= (1<<i);
    }

    s32Ret = UVC_VPSS_StopGrp(server_l->cam_index, u32ChnBmp);
    if(s32Ret)
    {
        printf("Stop vpss grp %d fail.\n", server_l->cam_index);
        return s32Ret;
    }

    return 0;
}

static AR_U32 UVC_GetEncType(AR_U32       fcc)
{
    if(fcc == V4L2_PIX_FMT_H264)
    {
        return PT_H264;
    }
    if(fcc == V4L2_PIX_FMT_HEVC)
    {
        return PT_H265;
    }
    if(fcc == V4L2_PIX_FMT_MJPEG)
    {
        return PT_MJPEG;
    }

    return PT_H265;
}

AR_S32 UVC_VENC_Init(void * handle,int argc, char *argv[])
{
    AR_S32 s32Ret = 0;
    AR_S32 vencChn = 0;
    AR_U32 u32EncType = 0;
    VENC_CHN_ATTR_S stChnAttr;
    VENC_PARAM_MOD_S stParamMod = { 0 };
    VENC_RECV_PIC_PARAM_S stParam;

    uvc_server_t    *server_l = get_uvc_server();

    memset(&stChnAttr, 0, sizeof(stChnAttr));

    u32EncType = UVC_GetEncType(server_l->dev->fcc);

    stChnAttr.stVencAttr.enType = u32EncType;

    stChnAttr.stVencAttr.u32PicWidth = server_l->width[0];
    stChnAttr.stVencAttr.u32PicHeight = server_l->height[0];
    stChnAttr.stVencAttr.u32MaxPicWidth = server_l->width[0];
    stChnAttr.stVencAttr.u32MaxPicHeight = server_l->height[0];
    stChnAttr.stVencAttr.u32BufSize = server_l->g_max_buffer_size * BUFFER_COUNT;//server_l->width[0] * server_l->height[0] * 2;
    stChnAttr.stVencAttr.bByFrame = AR_TRUE;
    stChnAttr.stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    if(stChnAttr.stVencAttr.enType == PT_H264)
    {
        stChnAttr.stVencAttr.stAttrH264e.u32CmdQueueDepth = 2;

        stChnAttr.stRcAttr.stH264Cbr.u32SrcFrameRate = server_l->fps;
        stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = server_l->fps;
        stChnAttr.stRcAttr.stH264Cbr.u32BitRate = __DEFAULT_H264_BITRATE__;
        stChnAttr.stRcAttr.stH264Cbr.u32Gop = server_l->fps;
        stChnAttr.stRcAttr.stH264Cbr.u32StatTime = 30;

        stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
        stParamMod.enVencModType = MODTYPE_H264E;
        AR_MPI_VENC_GetModParam(&stParamMod);
        //printf("264 clock %d %d\n",
        //    stParamMod.stH264eModParam.u32CoreClock, stParamMod.stH264eModParam.u32BpuClock);
        stParamMod.stH264eModParam.u32CoreClock = server_l->h26x_core_fre / 1000000;
        stParamMod.stH264eModParam.u32BpuClock = server_l->h26x_bpu_fre / 1000000;
        AR_MPI_VENC_SetModParam(&stParamMod);
    }
    else if(stChnAttr.stVencAttr.enType == PT_H265)
    {
        stChnAttr.stVencAttr.stAttrH265e.u32CmdQueueDepth = 2;

        stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = server_l->fps;
        stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = server_l->fps;
        stChnAttr.stRcAttr.stH265Cbr.u32BitRate = __DEFAULT_HEVC_BITRATE__;
        stChnAttr.stRcAttr.stH265Cbr.u32Gop = server_l->fps;
        stChnAttr.stRcAttr.stH265Cbr.u32StatTime = 30;

        stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
        stParamMod.enVencModType = MODTYPE_H265E;
        AR_MPI_VENC_GetModParam(&stParamMod);
        //printf("265 clock %d %d\n",
        //    stParamMod.stH265eModParam.u32CoreClock, stParamMod.stH265eModParam.u32BpuClock);
        stParamMod.stH265eModParam.u32CoreClock = server_l->h26x_core_fre / 1000000;
        stParamMod.stH265eModParam.u32BpuClock = server_l->h26x_bpu_fre / 1000000;
        AR_MPI_VENC_SetModParam(&stParamMod);
    }
    else if(stChnAttr.stVencAttr.enType == PT_MJPEG)
    {
        stChnAttr.stRcAttr.stMjpegCbr.u32SrcFrameRate = server_l->fps;
        stChnAttr.stRcAttr.stMjpegCbr.fr32DstFrameRate = server_l->fps;
        stChnAttr.stRcAttr.stMjpegCbr.u32BitRate = __DEFAULT_MJPG_BITRATE__;
        stChnAttr.stRcAttr.stMjpegCbr.u32StatTime = 1;

        stChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
        stParamMod.enVencModType = MODTYPE_JPEGE;
        AR_MPI_VENC_GetModParam(&stParamMod);
        //printf("jpeg clock %d\n",
        //    stParamMod.stJpegeModParam.u32CoreClock);
        stParamMod.stJpegeModParam.u32CoreClock = server_l->jpeg_core_fre / 1000000;
        AR_MPI_VENC_SetModParam(&stParamMod);
    }

    stChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    stChnAttr.stGopAttr.stNormalP.s32IPQpDelta = -2;

    vencChn = server_l->cam_index;
    s32Ret = AR_MPI_VENC_CreateChn(vencChn, &stChnAttr);
    if (s32Ret)
    {
        printf("create venc channel failed: %d\n", s32Ret);
        return s32Ret;
    }
    if(stChnAttr.stVencAttr.enType == PT_MJPEG)
    {
        VENC_RC_PARAM_S stMjpegRcParam = {0};
        s32Ret = AR_MPI_VENC_GetRcParam(vencChn, &stMjpegRcParam);
        if(0 == s32Ret)
        {
            int qp = 30;
            stMjpegRcParam.s32FirstFrameStartQp = qp;
            stMjpegRcParam.stParamMjpegCbr.u32MaxQfactor = 99;// set best quality for mjpeg
            stMjpegRcParam.stParamMjpegCbr.u32MinQfactor = qp;// set worst quality for mjpeg
            s32Ret = AR_MPI_VENC_SetRcParam(vencChn, &stMjpegRcParam);
            if(s32Ret)
            {
                printf("AR_MPI_VENC_SetRcParam failed! ret %d\n", s32Ret);
            }
            printf("mjpeg maxqp %d minqp %d.\n", stMjpegRcParam.stParamMjpegCbr.u32MaxQfactor, stMjpegRcParam.stParamMjpegCbr.u32MinQfactor);
        }
        else
        {
            printf("AR_MPI_VENC_GetRcParam failed! ret %d\n", s32Ret);
        }
    }
    stParam.s32RecvPicNum = -1;

    s32Ret = AR_MPI_VENC_StartRecvFrame(vencChn, &stParam);
    if (s32Ret != 0)
    {
        printf("dev id:%d start failed! ret=%d\n", vencChn, s32Ret);
        return s32Ret;
    }

    //pthread_mutex_init(&g_VencMutex[vencChn], 0);

    return 0;
}

AR_S32 UVC_VENC_UnInit()
{
    AR_S32 s32Ret = 0;
    int vencChn = 0;
    int i=0;
    VENC_CHN_STATUS_S pstStatus;
    VENC_STREAM_S    *pstStream = NULL;

    uvc_server_t    *server_l = get_uvc_server();

    vencChn = server_l->cam_index;

    //pthread_mutex_destroy(&g_VencMutex[vencChn]);
    s32Ret = AR_MPI_VENC_StopRecvFrame(vencChn);
    if (s32Ret != 0)
    {
        printf("dev id:%d stop failed! ret=%d\n", vencChn, s32Ret);
        return s32Ret;
    }

    //clean venc stream; please check!
    s32Ret = AR_MPI_VENC_QueryStatus(vencChn, &pstStatus);
    pstStream = (VENC_STREAM_S *)malloc(sizeof(VENC_STREAM_S));
    if(!pstStream)
    {
        printf("Malloc failed.\n");
        goto FAILED_0;
    }

    pstStream->pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));
    if (NULL == pstStream->pstPack)
    {
        printf("malloc memory failed!\n");
        goto FAILED_1;
    }
    pstStream->u32PackCount = 1;

    for(i = 0; i < pstStatus.u32LeftStreamFrames; i++)
    {
        s32Ret = AR_MPI_VENC_GetStream(vencChn, pstStream, -1);
        if(0 == s32Ret)
            AR_MPI_VENC_ReleaseStream(vencChn, pstStream);
        s32Ret = AR_MPI_VENC_QueryStatus(vencChn, &pstStatus);
    }

    free(pstStream->pstPack);
    pstStream->pstPack = NULL;
FAILED_1:
    free(pstStream);
FAILED_0:

    s32Ret = AR_MPI_VENC_DestroyChn(vencChn);
    if (s32Ret)
    {
        printf("destroy channel failed, vencChn:%d s32Ret:%02x\n", vencChn, s32Ret);
        return s32Ret;
    }

    return 0;
}

static AR_S32 AR_VO_StartHdmi()
{
    AR_HDMI_ATTR_S          stAttr;
    AR_HDMI_VIDEO_FMT_E     enVideoFmt = AR_HDMI_VIDEO_FMT_1080P_60;
    AR_HDMI_ID_E            enHdmiId    = AR_HDMI_ID_0;

    memset(&stAttr, 0, sizeof(AR_HDMI_ATTR_S));

    AR_MPI_HDMI_Init();
    AR_MPI_HDMI_Open(enHdmiId);
    AR_MPI_HDMI_GetAttr(enHdmiId, &stAttr);

    stAttr.bEnableHdmi           = AR_TRUE;
    stAttr.bEnableVideo          = AR_TRUE;
    stAttr.enVideoFmt            = enVideoFmt;
    stAttr.enVidOutMode          = AR_HDMI_VIDEO_MODE_YCBCR444;
    stAttr.enDeepColorMode = AR_HDMI_DEEP_COLOR_24BIT;

    stAttr.bxvYCCMode            = AR_FALSE;
    stAttr.enOutCscQuantization  = HDMI_QUANTIZATION_LIMITED_RANGE;

    stAttr.bEnableAudio          = AR_FALSE;
    stAttr.enSoundIntf           = AR_HDMI_SND_INTERFACE_I2S;
    stAttr.bIsMultiChannel       = AR_FALSE;

    stAttr.enBitDepth            = AR_HDMI_BIT_DEPTH_16;

    stAttr.bEnableAviInfoFrame   = AR_TRUE;
    stAttr.bEnableAudInfoFrame   = AR_TRUE;
    stAttr.bEnableSpdInfoFrame   = AR_FALSE;
    stAttr.bEnableMpegInfoFrame  = AR_FALSE;

    stAttr.bDebugFlag            = AR_FALSE;
    stAttr.bHDCPEnable           = AR_FALSE;

    stAttr.b3DEnable             = AR_FALSE;
    stAttr.enDefaultMode         = AR_HDMI_FORCE_HDMI;

    AR_MPI_HDMI_SetAttr(enHdmiId, &stAttr);
    AR_MPI_HDMI_Start(enHdmiId);

    return 0;
}

static AR_S32 AR_VO_StopHdmi()
{
    AR_HDMI_ID_E enHdmiId = AR_HDMI_ID_0;

    AR_MPI_HDMI_Stop(enHdmiId);
    AR_MPI_HDMI_Close(enHdmiId);
    AR_MPI_HDMI_DeInit();

    return AR_SUCCESS;
}


AR_S32 UVC_VO_Init(void * handle,int argc, char *argv[])
{
    AR_S32 s32Ret = AR_SUCCESS;
    VO_DEV VoDev;
    VO_PUB_ATTR_S  stPubAttr = {0};
    uvc_server_t    *server_l = get_uvc_server();

    if(!server_l->use_display)
    {
        return 0;
    }

    //0. set and enable dev
    VoDev = 0;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    stPubAttr.enIntfType = VO_INTF_HDMI;
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;

    s32Ret = AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("set vo pub failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    s32Ret = AR_MPI_VO_Enable(VoDev);
    if (s32Ret != AR_SUCCESS)
    {
        printf("enable vo failed with %#x!\n", s32Ret);
        return AR_FAILURE;
    }

    //1. set layer
    VO_LAYER VoLayer = 0;
    VO_VIDEO_LAYER_ATTR_S  stLayerAttr    = {0};
    VO_CSC_S               stLayerCsc     = {0};

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Width = server_l->disp_w;
    stLayerAttr.stDispRect.u32Height = server_l->disp_h;
    stLayerAttr.u32DispFrmRt = server_l->disp_fps;
    stLayerAttr.bClusterMode     = AR_FALSE;
    stLayerAttr.bDoubleFrame    = AR_FALSE;
    stLayerAttr.enPixFormat       = PIXEL_FORMAT_YVU_PLANAR_420;
    stLayerAttr.stImageSize.u32Width = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;

    s32Ret = AR_MPI_VO_SetDisplayBufLen(VoLayer, 3);
    if (AR_SUCCESS != s32Ret)
    {
        printf("AR_MPI_VO_SetDisplayBufLen failed with %#x!\n",s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_SetVideoLayerPartitionMode(VoLayer, VO_PART_MODE_MULTI);
    if (AR_SUCCESS != s32Ret)
    {
        printf("AR_MPI_VO_SetVideoLayerPartitionMode failed!\n");
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != AR_SUCCESS)
    {
        printf("Set video layer attr failed with %#x!\n", s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    stLayerCsc.enCscMatrix = VO_CSC_MATRIX_BT601_TO_RGB_PC;
    s32Ret = AR_MPI_VO_SetVideoLayerCSC(VoLayer, &stLayerCsc);
    if (s32Ret != AR_SUCCESS)
    {
        printf("Set video layer csc failed with %#x!\n", s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    s32Ret = AR_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != AR_SUCCESS)
    {
        printf("vo enable layer failed with %#x!\n", s32Ret);
        AR_MPI_VO_Disable(VoDev);
        return s32Ret;
    }

    //2. Set Chn, we set 9 mux by defaut
    VO_CHN_ATTR_S stChnAttr = {0};
    AR_S32 i = 0;
    AR_U32 u32Width = stLayerAttr.stImageSize.u32Width;
    AR_U32 u32Height = stLayerAttr.stImageSize.u32Height;
    //How many square do you want to display ? each square for a video.
    AR_U32 u32Square = server_l->disp_squ;

    stChnAttr.u32Priority       = 0;
    stChnAttr.bDeflicker        = AR_FALSE;

    for(i = 0; i < u32Square * u32Square; i++)
    {
        stChnAttr.stRect.s32X       = ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
        stChnAttr.stRect.s32Y       = ALIGN_DOWN((u32Height / u32Square) * (i / u32Square), 2);
        stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width / u32Square, 2);
        stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Square, 2);

        s32Ret = AR_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != AR_SUCCESS)
        {
            printf("VO set attr failed with %#x!\n", s32Ret);
            AR_MPI_VO_DisableVideoLayer(VoLayer);
            AR_MPI_VO_Disable(VoDev);
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VO_EnableChn(VoLayer, i);
        if (s32Ret != AR_SUCCESS)
        {
            printf("VO enable chn failed with %#x!\n", s32Ret);
            AR_MPI_VO_DisableVideoLayer(VoLayer);
            AR_MPI_VO_Disable(VoDev);
            return AR_FAILURE;
        }
    }

    //3. start hdmi device
    /******************************
    * Note : do this after vo device started.
    ********************************/
    AR_VO_StartHdmi();

    return 0;
}

AR_S32 UVC_VO_UnInit()
{
    AR_S32 i = 0;
    uvc_server_t    *server_l = get_uvc_server();

    if(!server_l->use_display)
    {
        return 0;
    }

    AR_VO_StopHdmi();

    for(i = 0; i < IPC_MAX_VO_WINDOW_NUM; i++)
    {
        AR_MPI_VO_DisableChn(0, i);
    }

    AR_MPI_VO_DisableVideoLayer(0);
    AR_MPI_VO_Disable(0);

    return 0;
}

AR_S32 UVC_Create_Channel(void * handle,int argc, char *argv[])
{
    MPP_CHN_S stVI, stVpss, stVenc, stVo;
    AR_S32 s32Ret = 0;
    uvc_server_t    *server_l = get_uvc_server();

    if(0 == server_l->is_raw)
    {
        //create a basic pipeline
        stVI.enModId = AR_ID_VI;
        stVI.s32DevId = server_l->cam_index;
        stVI.s32ChnId = 1;

        //main stream vi -- ch1 -> vpss
        stVpss.enModId = AR_ID_VPSS;
        stVpss.s32DevId = server_l->cam_index;
        stVpss.s32ChnId = 0;
        s32Ret = AR_MPI_SYS_Bind(&stVI, &stVpss);
        if(s32Ret < 0)
        {
            printf("Create Mod %d channel %d error 0x%x.\n", AR_ID_VI, 0, s32Ret);
            return s32Ret;
        }
    }

    if(V4L2_PIX_FMT_H264 == server_l->dev->fcc ||
       V4L2_PIX_FMT_HEVC == server_l->dev->fcc ||
       V4L2_PIX_FMT_MJPEG == server_l->dev->fcc)
    {
        stVpss.enModId = AR_ID_VPSS;
        stVpss.s32DevId = server_l->cam_index;
        stVpss.s32ChnId = 0;
        stVenc.enModId = AR_ID_VENC;
        stVenc.s32DevId = 0;
        stVenc.s32ChnId = server_l->cam_index;
        s32Ret = AR_MPI_SYS_Bind(&stVpss, &stVenc);
        if(s32Ret < 0)
        {
            printf("Create Mod %d channel %d error 0x%x.\n", AR_ID_VPSS, 0, s32Ret);
            return s32Ret;
        }
    }

    //sub stream for alg may not use in uvc service

    //vi -- ch1 -> vo
    if(server_l->use_display)
    {
        stVpss.enModId = AR_ID_VI;
        stVpss.s32DevId = server_l->cam_index;
        stVpss.s32ChnId = 1;
        stVo.enModId = AR_ID_VO;
        stVo.s32DevId = 0;
        stVo.s32ChnId = server_l->cam_index;
        s32Ret = AR_MPI_SYS_Bind(&stVpss, &stVo);
        if(s32Ret < 0)
        {
            printf("Create Mod %d channel %d error 0x%x.\n", AR_ID_VI, 1, s32Ret);
            return s32Ret;
        }
    }

    return 0;
}

AR_S32 UVC_Destroy_Channel()
{
    MPP_CHN_S stVI, stVpss, stVenc, stVo;
    AR_S32 s32Ret = 0;
    //int i = 0;
    uvc_server_t    *server_l = get_uvc_server();

    if(server_l->use_display)
    {
        stVpss.enModId = AR_ID_VI;
        stVpss.s32DevId = server_l->cam_index;
        stVpss.s32ChnId = 1;
        stVo.enModId = AR_ID_VO;
        stVo.s32DevId = 0;
        stVo.s32ChnId = server_l->cam_index;
        s32Ret = AR_MPI_SYS_UnBind(&stVpss, &stVo);
        if(s32Ret < 0)
        {
            printf("Destroy %d channel %d error ret %x.\n", AR_ID_VI, 1, s32Ret);
            return s32Ret;
        }
    }

    if(V4L2_PIX_FMT_H264 == server_l->dev->fcc ||
       V4L2_PIX_FMT_HEVC == server_l->dev->fcc ||
       V4L2_PIX_FMT_MJPEG == server_l->dev->fcc)
    {
        stVpss.enModId = AR_ID_VPSS;
        stVpss.s32DevId = server_l->cam_index;
        stVpss.s32ChnId = 0;
        stVenc.enModId = AR_ID_VENC;
        stVenc.s32DevId = 0;
        stVenc.s32ChnId = server_l->cam_index;
        s32Ret = AR_MPI_SYS_UnBind(&stVpss, &stVenc);
        if(s32Ret < 0)
        {
            printf("Destroy %d channel %d error ret %x.\n", AR_ID_VPSS, 0, s32Ret);
            return s32Ret;
        }
    }

    if(0 == server_l->is_raw)
    {
        //destroy a basic pipeline
        stVI.enModId = AR_ID_VI;
        stVI.s32DevId = server_l->cam_index;
        stVI.s32ChnId = 1;

        //main stream vi -- ch0 -> vpss
        stVpss.enModId = AR_ID_VPSS;
        stVpss.s32DevId = server_l->cam_index;
        stVpss.s32ChnId = 0;
        s32Ret = AR_MPI_SYS_UnBind(&stVI, &stVpss);
        if(s32Ret < 0)
        {
            printf("Destroy %d channel %d error ret %x.\n", AR_ID_VI, 0, s32Ret);
            return s32Ret;
        }
    }

    return 0;
}

void ar_uvc_init(void * handle,int argc, char *argv[])
{
    uvc_server_t    *server_l = get_uvc_server();

    UVC_VI_Init(handle, argc, argv);

    if(0 == server_l->is_raw)
        UVC_VPSS_Init(handle, argc, argv);

    if(V4L2_PIX_FMT_H264 == server_l->dev->fcc ||
       V4L2_PIX_FMT_HEVC == server_l->dev->fcc ||
       V4L2_PIX_FMT_MJPEG == server_l->dev->fcc)
        UVC_VENC_Init(handle, argc, argv);

    UVC_VO_Init(handle, argc, argv);

    UVC_Create_Channel(handle, argc, argv);

}

void ar_uvc_deinit(void * handle)
{
    uvc_server_t       *server_l        = get_uvc_server();

    UVC_Destroy_Channel();

    UVC_VO_UnInit();

    if(V4L2_PIX_FMT_H264 == server_l->dev->fcc ||
       V4L2_PIX_FMT_HEVC == server_l->dev->fcc ||
       V4L2_PIX_FMT_MJPEG == server_l->dev->fcc)
        UVC_VENC_UnInit();

    if(0 == server_l->is_raw)
        UVC_VPSS_UnInit();

    UVC_VI_UnInit();
}

int get_fcc_index(struct uvc_format_info *format, int fcc)
{
    int ret = -1, i = 0;
    for(i = 0; i < g_uvc_formats_num; ++i)
    {
        if(format[i].fcc == fcc)
            return i;
    }
    return ret;
}

int uvc_pipeline_set_format(unsigned int width, unsigned int height, unsigned int fps, unsigned int fcc)
{
    int ret = 0;
    uvc_server_t               *server_l    = get_uvc_server();
    unsigned int                fcc_index;
    struct uvc_format_opt      *format_opt;

    fcc_index = server_l->dev->fcc_index;
    format_opt  = g_uvc_formats[fcc_index].data->format_opt;

    if(server_l->dev->fcc != fcc)
    {
        char *p2 = (char *)&fcc;

        printf("change format to %c%c%c%c(%d x %d @%d fps)\n", p2[0], p2[1], p2[2], p2[3], width, height, fps);

        format_opt->format_deinit(g_uvc_formats[fcc_index].data, server_l->handle);
        server_l->dev->fcc = fcc;
        server_l->fps = fps;
        server_l->width[0] = width;
        server_l->height[0] = height;
        server_l->dev->fcc_index = get_fcc_index(g_uvc_formats, fcc);
        fcc_index = server_l->dev->fcc_index;
        format_opt  = g_uvc_formats[fcc_index].data->format_opt;

        format_opt->format_init(g_uvc_formats[fcc_index].data, server_l->handle, server_l->argc, server_l->argv);
    }
    else
    {
        format_opt->format_deinit(g_uvc_formats[fcc_index].data, server_l->handle);
        server_l->fps = fps;
        server_l->width[0] = width;
        server_l->height[0] = height;
        format_opt->format_init(g_uvc_formats[fcc_index].data, server_l->handle, server_l->argc, server_l->argv);
    }
#if 0
    ar_uvc_get_context((void**)&context, &context_size);

    pipe_fd = server_l->pipe_line_fd;
    if(pipe_fd < 0 || NULL == server_l->handle)
    {
        log_err("uvc_pipeline_set_format check failed!(%d %p)\n", pipe_fd, server_l->handle);
        return ret;
    }

    if(server_l->is_hdr)
    {
        fps=25;
    }

    if(server_l->is_raw)
    {
#if 0
        log_always("%s res0 w=%d h=%d", __func__, width,height);
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 3),AR_COMMON_META_VIDEO_WIDTH_NAME,&width,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 3),AR_COMMON_META_VIDEO_HEIGHT_NAME,&height,sizeof(int));

        //set fps
        log_always("%s fps=%d", __func__, fps);
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 3),AR_COMMON_META_VIDEO_FPS_NUM_NAME,&fps,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 3),AR_COMMON_META_VIDEO_FPS_DEN_NAME,&den_num,sizeof(int));
#endif
    }
    else
    {
        ret = ar_pipeline_terminal_stream(server_l->handle, (ar_pipeline_t)context->pipeline.element);

        if(server_l->dev->fcc != fcc)
        {
            unsigned int                fcc_index;
            struct uvc_format_opt     * format_opt;
            char *p2 = (char *)&fcc;

            fcc_index = server_l->dev->fcc_index;
            format_opt  = uvc_formats[fcc_index].data->format_opt;

            printf("change format to %c%c%c%c(%d x %d @%d fps)\n", p2[0], p2[1], p2[2], p2[3], width, height, fps);

            format_opt->format_deinit(uvc_formats[fcc_index].data, server_l->handle);

            server_l->dev->fcc = fcc;
            server_l->dev->fcc_index = get_fcc_index(uvc_formats, fcc);
            fcc_index = server_l->dev->fcc_index;
            format_opt  = uvc_formats[fcc_index].data->format_opt;

            format_opt->format_init(uvc_formats[fcc_index].data, server_l->handle, server_l->argc, server_l->argv);
        }
        else
        {
            ar_element_modify_property_value(server_l->handle,context->cam_src.element,AR_VIDEO_CAM_SET_ID,&server_l->cam_index,sizeof(server_l->cam_index));
        }

        //if(V4L2_PIX_FMT_NV12 == fcc || V4L2_PIX_FMT_YUYV == fcc)
        if(V4L2_PIX_FMT_H264 == fcc || V4L2_PIX_FMT_HEVC == fcc)
        {
            struct framebased_priv_data * priv_data = uvc_formats[server_l->dev->fcc_index].data->priv;
            priv_data->i_frame_for_start = 0;
            if(NULL != server_l->dev && V4L2_PIX_FMT_HEVC == fcc)
            {
                uint32_t bitrate = __DEFAULT_HEVC_BITRATE__;
                ar_element_modify_property_value(server_l->handle, context->hevc_enc.element,AR_VIDEO_CODEC_META_BITRATE, &bitrate, sizeof(uint32_t));
            }

            if(NULL != server_l->dev && V4L2_PIX_FMT_H264 == fcc)
            {
                uint32_t bitrate = __DEFAULT_H264_BITRATE__;
                ar_element_modify_property_value(server_l->handle, context->avc_enc.element,AR_VIDEO_CODEC_META_BITRATE, &bitrate, sizeof(uint32_t));
            }
        }

        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 1),AR_COMMON_META_VIDEO_WIDTH_NAME,&width,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 1),AR_COMMON_META_VIDEO_HEIGHT_NAME,&height,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 0),AR_COMMON_META_VIDEO_FPS_NUM_NAME,&fps,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 0),AR_COMMON_META_VIDEO_FPS_DEN_NAME,&den_num,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 1),AR_COMMON_META_VIDEO_FPS_NUM_NAME,&fps,sizeof(int));
        ar_pad_modify_meta_value(server_l->handle,ar_element_get_src_pad_by_idx(server_l->handle,context->cam_src.element, 1),AR_COMMON_META_VIDEO_FPS_DEN_NAME,&den_num,sizeof(int));

        ret = ar_pipeline_start_stream(server_l->handle, (ar_pipeline_t)context->pipeline.element);
    }
#endif
    return ret;
}


