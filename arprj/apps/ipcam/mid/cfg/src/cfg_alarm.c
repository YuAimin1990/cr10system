#include "cfg_alarm.h"

IPC_CFG_ALARM_S g_stRunAlarmCfg;

static CFG_MAP alarmInMap[] = {
    {"channel",      &(g_stRunAlarmCfg.alarmIn.u32RecChn),            CFG_DATA_TYPE_S32,   "0", "rw", 0, 3,    NULL},
    {"name",    &(g_stRunAlarmCfg.alarmIn.alarmInName),        CFG_DATA_TYPE_STRING,"alarm", "rw", 1, MAX_STR_LEN_32, NULL},
    {"default_state", &(g_stRunAlarmCfg.alarmIn.defaultState),       CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, "0-low 1-high"},
    {"active_state",  &(g_stRunAlarmCfg.alarmIn.activeState),        CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, "0-low 1-high"},

    {"interval", &(g_stRunAlarmCfg.alarmIn.handle.intervalTime), CFG_DATA_TYPE_U32,  "5", "rw", 1, 120,  NULL},
    {"is_snap",      &(g_stRunAlarmCfg.alarmIn.handle.is_snap), 		CFG_DATA_TYPE_U8,   "0", "rw", 0, 1,   NULL},
	{"snap_num",	  	&(g_stRunAlarmCfg.alarmIn.handle.snapNum), 		CFG_DATA_TYPE_U32,  "1", "rw", 1, 100, NULL},
    {"is_beep",      &(g_stRunAlarmCfg.alarmIn.handle.is_beep), 		CFG_DATA_TYPE_U8,   "0", "rw", 0, 1,   NULL},
	{"beep_time",	  	&(g_stRunAlarmCfg.alarmIn.handle.beepTime),		CFG_DATA_TYPE_U32,  "3", "rw", 1, 600, NULL},
    {NULL,},
};

static CFG_MAP alarmOutMap[] = {
    {"name",  &(g_stRunAlarmCfg.alarmOut.alarmOutName),       CFG_DATA_TYPE_S8,    "0", "rw", 1, MAX_STR_LEN_32, NULL},
    {"default_state",  &(g_stRunAlarmCfg.alarmOut.defaultState),       CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, "0-low 1-high"},
    {"active_state",   &(g_stRunAlarmCfg.alarmOut.activeState),        CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, "0-low 1-high"},
    {"poweron_state",  &(g_stRunAlarmCfg.alarmOut.powerOnState),       CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, "0-pulse 1-continuous"},
    {"pulse_duration", &(g_stRunAlarmCfg.alarmOut.pulseDuration),      CFG_DATA_TYPE_S32,   "5000", "rw", 1000, 10000, NULL},
    {NULL,},
};

static CFG_MAP alarmCfgMap[] = {
	{"gpio_enable",           &(g_stRunAlarmCfg.cfg.gpio_enable),       CFG_DATA_TYPE_S8,   "0", "rw", 0, 1,    NULL},
	{"in_level",              &(g_stRunAlarmCfg.cfg.in_level),          CFG_DATA_TYPE_S8,   "0", "rw", 0, 1,    NULL},
	{"out_level",             &(g_stRunAlarmCfg.cfg.out_level),         CFG_DATA_TYPE_S8,   "0", "rw", 0, 1,    NULL},
	{"io_linkage",            &(g_stRunAlarmCfg.cfg.io_linkage),        CFG_DATA_TYPE_S8,   "0", "rw", 0, 1,    NULL},

    {"upload_interval",       &(g_stRunAlarmCfg.cfg.upload_interval),   CFG_DATA_TYPE_S8,   "0", "rw", 0, 255,    NULL},
    {"alarmbymail",           &(g_stRunAlarmCfg.cfg.alarmbymail),       CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, NULL},
    {"schedule_enable",       &(g_stRunAlarmCfg.cfg.schedule_enable),   CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, NULL},
    {"snapshot_enable",       &(g_stRunAlarmCfg.cfg.snapshot_enable),   CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, NULL},
    {"recorder_enable",       &(g_stRunAlarmCfg.cfg.recorder_enable),   CFG_DATA_TYPE_S8,    "0", "rw", 0, 1, NULL},
    {"move_perset",           &(g_stRunAlarmCfg.cfg.move_perset),       CFG_DATA_TYPE_S8,    "1", "rw", 1, 16, NULL},
    {"alarm_mute",             &(g_stRunAlarmCfg.cfg.alarmMute),         CFG_DATA_TYPE_S8,      "0", "rw", 0, 1, NULL},
    {NULL,},
};

void IPC_CFG_ALARM_Print()
{
    printf("*************** Alarm **************\n");

    printf("alarm in:\n");
    CfgPrintMap(alarmInMap);
    printf("\n");

    printf("alarm out:\n");
    CfgPrintMap(alarmOutMap);
    printf("\n");

	printf("alarm cfg:\n");
    CfgPrintMap(alarmCfgMap);
    printf("\n");

    printf("*************** Alarm **************\n\n");
}

AR_S32 IPC_CFG_ALARM_Save()
{
    cJSON *root;
    char *out;

    root = cJSON_CreateObject();

    CfgAddCjson(root, "alarmin", alarmInMap);
    CfgAddCjson(root, "alarmout", alarmOutMap);
    CfgAddCjson(root, "alarmcfg", alarmCfgMap);

    out = cJSON_Print(root);

    AR_S32 ret = CfgWriteToFile(ALARM_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", ALARM_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

AR_S32 IPC_CFG_ALARM_LoadDefault()
{
    CfgLoadDefValue(alarmInMap);
    CfgLoadDefValue(alarmOutMap);
	CfgLoadDefValue(alarmCfgMap);

    return 0;
}

AR_S32 IPC_CFG_ALARM_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    data = CfgReadFromFile(ALARM_CFG_FILE);
    if (data == NULL) {
        PRINT_INFO("load %s error, so to load default cfg param.\n", ALARM_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json){
        PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    CfgParseCjson(json, "alarmin", alarmInMap);
    CfgParseCjson(json, "alarmout", alarmOutMap);
	CfgParseCjson(json, "alarmcfg", alarmCfgMap);

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_ALARM_LoadDefault();
    IPC_CFG_ALARM_Save();
    return 0;
}

IPC_CFG_ALARM_S * IPC_CFG_ALARM_GetParam()
{
    return &g_stRunAlarmCfg;
}

