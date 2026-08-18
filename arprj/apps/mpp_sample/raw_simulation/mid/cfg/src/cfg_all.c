#include "cfg_all.h"
#include "util_api.h"

//All configs should be got from this struct, each one is a sensor pipeline.
//IPC_CONFIG_S g_stIPCConfig;

AR_S32 IPC_CFG_SaveAll()
{
    IPC_CFG_VIO_Save();
    IPC_CFG_VENC_Save();
    IPC_CFG_IMAGE_Save();
    IPC_CFG_RECORD_Save();
    IPC_CFG_SNAP_Save();

    return 0;
}


AR_S32 IPC_CFG_LoadAll(CFG_RUN_MODE_E enMode)
{
    IPC_CFG_VIO_Load(enMode);
    //IPC_CFG_VPSS_Load(enMode);

    IPC_CFG_VENC_Load(enMode);

    IPC_CFG_IMAGE_Load(enMode);
    IPC_CFG_RECORD_Load(enMode);
    IPC_CFG_SNAP_Load(enMode);

    return 0;
}

AR_S32 IPC_CFG_PrintAll()
{
#if 0
    //IPC_CFG_GPIO_Print();
	IPC_CFG_VIO_Print();
    IPC_CFG_VPSS_Print();
    IPC_CFG_VENC_Print();
    //IPC_CFG_AUDIO_Print();
	//IPC_CFG_NETWORK_Print();

	//IPC_CFG_USER_Print();
	//IPC_CFG_PTZ_Print();
	//IPC_CFG_ALARM_Print();
	IPC_CFG_OSD_Print();
	IPC_CFG_IMAGE_Print();
	//IPC_CFG_MD_Print();
	IPC_CFG_RECORD_Print();
	IPC_CFG_SNAP_Print();
    //IPC_CFG_SYSTEM_Print();
    //IPC_CFG_VENDOR_Print();

    //IPC_CFG_SVP_Print();
	IPC_CFG_CHANNEL_Print();
	IPC_CFG_CONTROL_Print();
#endif
    return 0;
}

AR_S32 IPC_CFG_LoadDefaultAll(CFG_RUN_MODE_E enMode)
{
    IPC_CFG_VIO_LoadDefault(enMode);
    IPC_CFG_VENC_LoadDefault(enMode);
	IPC_CFG_IMAGE_LoadDefault();
	IPC_CFG_RECORD_LoadDefault();
	IPC_CFG_SNAP_LoadDefault();

    return 0;
}

/*Remove all the json files*/
void IPC_CFG_ClearAll()
{
    char cmd[128] = {0};
	sprintf(cmd, "rm -rf %s/*.json", CFG_DIR);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    sleep(1);
}



