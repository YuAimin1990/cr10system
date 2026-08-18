#include "pf_type.h"
#include "pf_video.h"
#include "pf_osd.h"
#include "pf_audio.h"
#include "pf_md.h"
#include "pf_od.h"
#include "pf_mbuf.h"

#include "ipc_video.h"
#include "ipc_gpio.h"
#include "ar_rtsp_server.h"
#include "ipc_net.h"

#ifdef THERMAL_ENABLE
#include "pf_thermal.h"
#endif
#include "ipc_rtsp.h"
#include "pf_vgs.h"
#include "hal_vin_type_def.h"

int IPC_AV_Init()
{
    int ret = 0;	

    ret = IPC_PF_VI_Init();
    if (0 != ret)
    {
        PRINT_ERR("IPC_PF_VI_Init failed!\n");
        return -1;
    }

#ifdef THERMAL_ENABLE
        printf("THERMAL_ENABLE defined !\n");
        IPC_PF_THERMAL_Start();
#endif
    ret = IPC_PF_VPSS_Init();
    if (0 != ret)
    {
        PRINT_ERR("IPC_PF_VPSS_Init failed!\n");
        return -1;
    }
	
    ret = IPC_PF_VO_Init();
    if (0 != ret)
    {
        PRINT_ERR("IPC_PF_VO_Init failed!\n");
        return -1;
    }

    ret = IPC_PF_VENC_Init();
    if (0 != ret)
    {
        PRINT_ERR("IPC_PF_VENC_Init failed!\n");
        return -1;
    }

    //bind pipeline : vi + vpss + vo + venc
    //After this, the pipeline should start
    ret = IPC_PF_Create_Channel();
    if(0 != ret)
    {
        PRINT_ERR("IPC_PF_Create_Channel failed!\n");
        return -1;
    }

#ifdef ENABLE_VGS
    IPC_VGS_Init();
#endif


    ret = IPC_PF_Audio_Init();
    if (0 != ret)
    {
        PRINT_ERR("IPC_PF_Audio_Init failed!\n");
        return -1;
    }

    IPC_PF_MD_Init();
	IPC_PF_OD_Init();

    ret = IPC_PF_MBUF_Init();
    if (0 != ret)
    {
        PRINT_ERR("ipc_mediabuf_start failed!\n");
        return -1;
    }

	IPC_PF_VENC_StartMbufThread();
	IPC_PF_AENC_StartMbufThread();
	IPC_PF_AI_StartMbufThread();
 
	ret = IPC_PF_OSD_Init();
	if (0 != ret)
	{
		PRINT_ERR("IPC_PF_OSD_Init failed!\n");
		return -1;
	}

	IPC_NET_RTSP_Start();

	//2022-04-20 for ahd camera detect.
    IPC_PF_VI_StartCameraDetect();

    return 0;
}

int IPC_AV_UnInit()
{
	IPC_PF_VI_StopCameraDetect();
	
	IPC_NET_RTSP_Stop();
	
    IPC_PF_OSD_UnInit();

    IPC_PF_VENC_StopMbufThread();
    IPC_PF_AENC_StopMbufThread();
	IPC_PF_AI_StopMbufThread();

    IPC_PF_MBUF_UnInit();
    IPC_PF_MD_UnInit();
    IPC_PF_OD_UnInit();

    IPC_PF_Audio_UnInit();
#ifdef ENABLE_VGS
    IPC_VGS_UnInit();
#endif

    IPC_PF_Destroy_Channel();

    IPC_PF_VENC_UnInit();
    IPC_PF_VO_UnInit();
    IPC_PF_VPSS_UnInit();
    IPC_PF_VI_UnInit();

	return 0;
}


AR_S32 IPC_AV_Suspend()
{
    IPC_PF_VI_CameraDectect_Suspend();
	IPC_PF_AENC_MbufThread_Suspend();
	IPC_PF_AI_MbufThread_Suspend();
	IPC_PF_Audio_Suspend();
    IPC_PF_VENC_MbufThread_Suspend(); 	
    IPC_PF_Destroy_Channel();
    IPC_PF_VENC_Suspend();
    IPC_PF_VO_Suspend();
	IPC_PF_OSD_Suspend();
	IPC_PF_MD_Suspend();
	IPC_PF_OD_Suspend();
#ifdef ENABLE_VGS
	IPC_VGS_Suspend();
#endif
    IPC_PF_VPSS_Suspend();	
    IPC_PF_VI_Suspend();
	IPC_PF_SubMoudle_Suspend();

	return 0;
}

AR_S32 IPC_AV_Resume()
{
	IPC_PF_SubMoudle_Resume();
	IPC_PF_VI_Resume();		
	IPC_PF_VPSS_Resume();
	IPC_PF_VENC_Resume();
	IPC_PF_MD_Resume();
	IPC_PF_OD_Resume();
#ifdef ENABLE_VGS
	IPC_VGS_Resume();
#endif	
	IPC_PF_OSD_Resume();
	IPC_PF_VO_Resume();
	IPC_PF_Create_Channel();
	IPC_PF_VENC_MbufThread_Resume();
	IPC_PF_AENC_MbufThread_Resume();
	IPC_PF_Audio_Resume();
	IPC_PF_AI_MbufThread_Resume();
	IPC_PF_VI_CameraDectect_Resume();

	return 0;
}
