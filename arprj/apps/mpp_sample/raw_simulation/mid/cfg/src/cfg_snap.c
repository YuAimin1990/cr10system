#include "cfg_snap.h"

IPC_CFG_SNAP_S g_stRunSnapCfg;

//Snap one channel only?
CFG_MAP snapTimerMap[] = {
    {"enable",            &g_stRunSnapCfg.timer_snap.enable,               CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1, NULL},
    {"scheduleTime",      &(g_stRunSnapCfg.timer_snap.scheduleTime[0][0]), CFG_DATA_TYPE_STIME, "0",   "rw", 0, 28, NULL},
    {"interval",          &g_stRunSnapCfg.timer_snap.interval,             CFG_DATA_TYPE_S32,   "10000",   "rw", 0, 1000000, NULL},
    {"nums",              &g_stRunSnapCfg.timer_snap.nums,                 CFG_DATA_TYPE_S32,   "1",   "rw", 1, 30,   NULL},
    {"pictureQuality",    &g_stRunSnapCfg.timer_snap.pictureQuality,       CFG_DATA_TYPE_S32,   "2",   "rw", 0, 4, NULL},
    {"imageSize",         &g_stRunSnapCfg.timer_snap.imageSize,            CFG_DATA_TYPE_S32,   "0",   "rw", 0, 4, "0: 1080p 1: 720p 2 D1 3：CIF，4：QCIF"},
    {"snapShotImageType", &(g_stRunSnapCfg.timer_snap.snapShotImageType),  CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1, "0 JPEG；1 bmp"},
    {"storageMode",      &(g_stRunSnapCfg.timer_snap.storagerMode),       CFG_DATA_TYPE_S32,   "0",   "rw", 0, 2, "0 local；1 ftp; 2 local|ftp"},
    {"channelID",         &(g_stRunSnapCfg.timer_snap.channelID),          CFG_DATA_TYPE_S32,   "3",   "rw", 3, 3, "channel id for snapshort"},
    {NULL,},
};

CFG_MAP snapEventMap[] = {
	{"enable",            &g_stRunSnapCfg.event_snap.enable,               CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1, NULL},
    {"interval",          &g_stRunSnapCfg.event_snap.interval,             CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1000, NULL},
    {"nums",              &g_stRunSnapCfg.event_snap.nums,                 CFG_DATA_TYPE_S32,   "1",   "rw", 1, 30,   NULL},
    {"pictureQuality",    &g_stRunSnapCfg.event_snap.pictureQuality,       CFG_DATA_TYPE_S32,   "100", "rw", 0, 100, NULL},
    {"imageSize",         &g_stRunSnapCfg.event_snap.imageSize,            CFG_DATA_TYPE_S32,   "0",   "rw", 0, 4, "0: 1080p 1: 720p 2 D1 3：CIF，4：QCIF"},
    {"snapShotImageType", &(g_stRunSnapCfg.event_snap.snapShotImageType),  CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1, "0 JPEG；1 bmp"},
    {"storageMode",      &(g_stRunSnapCfg.event_snap.storagerMode),       CFG_DATA_TYPE_S32,   "0",   "rw", 0, 2, "0 local；1 ftp; 2 local|ftp"},
    {"channelID",         &(g_stRunSnapCfg.event_snap.channelID),          CFG_DATA_TYPE_S32,   "3",   "rw", 3, 3, "channel id for snapshort"},
    {NULL,},
};

void IPC_CFG_SNAP_Print()
{
    printf("**********Timer Snap *********\n");
    CfgPrintMap(snapTimerMap);
    printf("**********Timer Snap *********\n\n");

	printf("**********Event Snap *********\n");
    CfgPrintMap(snapEventMap);
    printf("**********Event Snap *********\n\n");
}

AR_S32 IPC_CFG_SNAP_Save()
{
	cJSON *root;
    char *out;

	root = cJSON_CreateObject();//创建项目

    CfgAddCjson(root, "timerSnap", snapTimerMap);
    CfgAddCjson(root, "eventSnap", snapEventMap);

    out = cJSON_Print(root);

    AR_S32 ret = CfgWriteToFile(SNAP_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", SNAP_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);
    return 0;
}

AR_S32 IPC_CFG_SNAP_Load(CFG_RUN_MODE_E enMode)
{
	char *data = NULL;
    data = CfgReadFromFile(SNAP_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", SNAP_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    CfgParseCjson(json, "timerSnap", snapTimerMap);
    CfgParseCjson(json, "eventSnap", snapEventMap);

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_SNAP_LoadDefault();
    IPC_CFG_SNAP_Save();
    return 0;
}

AR_S32 IPC_CFG_SNAP_LoadDefault()
{
    CfgLoadDefValue(snapTimerMap);
	CfgLoadDefValue(snapEventMap);
    return 0;
}

IPC_CFG_SNAP_S * IPC_CFG_SNAP_GetParam()
{
    return &g_stRunSnapCfg;
}
