#include "pf_type.h"
#include "pf_video.h"
#include "pf_mbuf.h"

#include "ipc_video.h"
#include "ar_rtsp_server.h"
#include "ipc_net.h"

#include "ipc_rtsp.h"
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

    ret = IPC_PF_MBUF_Init();
    if (0 != ret)
    {
        PRINT_ERR("ipc_mediabuf_start failed!\n");
        return -1;
    }

    IPC_PF_VENC_StartMbufThread();

    IPC_NET_RTSP_Start();

	//2022-04-20 for ahd camera detect.
    //IPC_PF_VI_StartCameraDetect();

    return 0;
}

int IPC_AV_UnInit()
{
	IPC_NET_RTSP_Stop();
	
    IPC_PF_VENC_StopMbufThread();

    IPC_PF_MBUF_UnInit();

    IPC_PF_Destroy_Channel();

    IPC_PF_VENC_UnInit();
    IPC_PF_VO_UnInit();
    IPC_PF_VPSS_UnInit();
    IPC_PF_VI_UnInit();

	return 0;
}


