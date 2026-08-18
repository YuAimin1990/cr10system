#include "cfg_record.h"

IPC_CFG_RECORD_S g_stRunRecordCfg;

CFG_MAP recordMap[IPC_MAX_PIPELINE_NUM][IPC_MAX_VENC_STREAM_PER_CHN][12] =
{
    //channel 0:
    {
        //stream 0
        {
            {"enable",           &g_stRunRecordCfg.stRecord[0][0].enable,                CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   NULL},
            {"stream_no",        &g_stRunRecordCfg.stRecord[0][0].stream_no,             CFG_DATA_TYPE_S32,   "0",   "rw", 0, 3,   "0-3"},
            {"recordMode",       &g_stRunRecordCfg.stRecord[0][0].recordMode,            CFG_DATA_TYPE_S32,   "1",   "rw", 1, 3,   "1 manual 2 sched other n/a"},
            {"recordType",       &g_stRunRecordCfg.stRecord[0][0].recordType,            CFG_DATA_TYPE_S32,   "0",   "rw", 0, 3,   "0 avi 1 mov"},
            {"scheduleTime",     &(g_stRunRecordCfg.stRecord[0][0].scheduleTime[0][0]),  CFG_DATA_TYPE_STIME, "0",   "rw", 0, 0,  NULL},
            {"scheduleSlice",    &(g_stRunRecordCfg.stRecord[0][0].scheduleSlice[0][0]), CFG_DATA_TYPE_SLICE, "4294967295", "rw", 0, 0, NULL},
            {"preRecordTime",  &g_stRunRecordCfg.stRecord[0][0].preRecordTime,        CFG_DATA_TYPE_S32,   "1",   "rw", 0, 10,  NULL},
            {"audioRecEnable", &g_stRunRecordCfg.stRecord[0][0].audioRecEnable,       CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   NULL},
            {"recAudioType",   &g_stRunRecordCfg.stRecord[0][0].recAudioType,         CFG_DATA_TYPE_S32,   "2",   "rw", 0, 3,   NULL},
            {"recordDuration",      &g_stRunRecordCfg.stRecord[0][0].recordDuration,            CFG_DATA_TYPE_S32,   "5",  "rw", 1, 1440, NULL},
            {"recycleRecord",  &g_stRunRecordCfg.stRecord[0][0].recycleRecord,        CFG_DATA_TYPE_S32,   "1",   "rw", 0, 1,   NULL},
            {NULL,}
        },
        //stream 1
        {
            {"enable",           &g_stRunRecordCfg.stRecord[0][1].enable,                CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   NULL},
            {"stream_no",        &g_stRunRecordCfg.stRecord[0][1].stream_no,             CFG_DATA_TYPE_S32,   "0",   "rw", 0, 3,   "0-3"},
            {"recordMode",       &g_stRunRecordCfg.stRecord[0][1].recordMode,            CFG_DATA_TYPE_S32,   "3",   "rw", 1, 3,   "1 manual 2 sched other n/a"},
            {"recordType",       &g_stRunRecordCfg.stRecord[0][1].recordType,            CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   "0 avi 1 mov"},
            {"scheduleTime",     &(g_stRunRecordCfg.stRecord[0][1].scheduleTime[0][0]),  CFG_DATA_TYPE_STIME, "0",   "rw", 0, 0,  NULL},
            {"scheduleSlice",    &(g_stRunRecordCfg.stRecord[0][1].scheduleSlice[0][0]), CFG_DATA_TYPE_SLICE, "4294967295", "rw", 0, 0, NULL},
            {"preRecordTime",  &g_stRunRecordCfg.stRecord[0][1].preRecordTime,        CFG_DATA_TYPE_S32,   "1",   "rw", 0, 10,  NULL},
            {"audioRecEnable", &g_stRunRecordCfg.stRecord[0][1].audioRecEnable,       CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   NULL},
            {"recAudioType",   &g_stRunRecordCfg.stRecord[0][1].recAudioType,         CFG_DATA_TYPE_S32,   "2",   "rw", 0, 3,   NULL},
            {"recordDuration",      &g_stRunRecordCfg.stRecord[0][1].recordDuration,            CFG_DATA_TYPE_S32,   "5",  "rw", 1, 1440, NULL},
            {"recycleRecord",  &g_stRunRecordCfg.stRecord[0][1].recycleRecord,        CFG_DATA_TYPE_S32,   "1",   "rw", 0, 1,   NULL},
            {NULL,}
        },
        //stream 2
        {
            {"enable",           &g_stRunRecordCfg.stRecord[0][2].enable,                CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   NULL},
            {"stream_no",        &g_stRunRecordCfg.stRecord[0][2].stream_no,             CFG_DATA_TYPE_S32,   "0",   "rw", 0, 3,   "0-3"},
            {"recordMode",       &g_stRunRecordCfg.stRecord[0][2].recordMode,            CFG_DATA_TYPE_S32,   "3",   "rw", 1, 3,   "1 manual 2 sched other n/a"},
            {"recordType",       &g_stRunRecordCfg.stRecord[0][2].recordType,            CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   "0 avi 1 mov"},
            {"scheduleTime",     &(g_stRunRecordCfg.stRecord[0][2].scheduleTime[0][0]),  CFG_DATA_TYPE_STIME, "0",   "rw", 0, 0,  NULL},
            {"scheduleSlice",    &(g_stRunRecordCfg.stRecord[0][2].scheduleSlice[0][0]), CFG_DATA_TYPE_SLICE, "4294967295", "rw", 0, 0, NULL},
            {"preRecordTime",  &g_stRunRecordCfg.stRecord[0][2].preRecordTime,        CFG_DATA_TYPE_S32,   "1",   "rw", 0, 10,  NULL},
            {"audioRecEnable", &g_stRunRecordCfg.stRecord[0][2].audioRecEnable,       CFG_DATA_TYPE_S32,   "0",   "rw", 0, 1,   NULL},
            {"recAudioType",   &g_stRunRecordCfg.stRecord[0][2].recAudioType,         CFG_DATA_TYPE_S32,   "2",   "rw", 0, 3,   NULL},
            {"recordDuration",      &g_stRunRecordCfg.stRecord[0][2].recordDuration,            CFG_DATA_TYPE_S32,   "5",  "rw", 1, 1440, NULL},
            {"recycleRecord",  &g_stRunRecordCfg.stRecord[0][2].recycleRecord,        CFG_DATA_TYPE_S32,   "1",   "rw", 0, 1,   NULL},
            {NULL,}
        }
    },
    {
    	{
    		{NULL,},
    	}
	},
    {
    	{
    		{NULL,},
    	}
	},
    {
    	{
    		{NULL,},
    	}
	},
    {
    	{
    		{NULL,},
    	}
	},
    {
    	{
    		{NULL,},
    	}
	},
    {
    	{
    		{NULL,},
    	}
	},
    {
    	{
    		{NULL,},
    	}
	},

};

void IPC_CFG_RECORD_Print()
{
    int i = 0, j = 0;
    printf("********** Record *********\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            printf("channel %d stream %d:\n", i, j);
            CfgPrintMap(recordMap[i][j]);
            printf("\n");
        }
    }
    printf("********** Record *********\n\n");
}

int IPC_CFG_RECORD_Save()
{
    cJSON *root, *node, *item;
    char *out;
    int i, j;
    char tmp[32] = {0};

    printf("Save record parameters...\n");

    root = cJSON_CreateObject();

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        node = cJSON_CreateObject();
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            item = CfgDataToCjsonByMap(recordMap[i][j]);
            sprintf(tmp, "stream_%d", j);
            cJSON_AddItemToObject(node, tmp, item);
        }
        printf("Channel %d record...\n", i);
        sprintf(tmp, "channel_%d", i);
        cJSON_AddItemToObject(root, tmp, node);
    }

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(RECORD_CFG_FILE, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", RECORD_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

int IPC_CFG_RECORD_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    char tmp[32] = {0};
    int i, j;

    data = CfgReadFromFile(RECORD_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", RECORD_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_INFO("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    cJSON *chn = NULL;
    cJSON *stream = NULL;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        sprintf(tmp, "channel_%d", i);
        chn = cJSON_GetObjectItem(json, tmp);
        if(!chn)
        {
            for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
            {
                CfgLoadDefValue(recordMap[i][j]);
            }
        }
        else
        {
            for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
            {
                sprintf(tmp, "stream_%d", j);
                stream = cJSON_GetObjectItem(chn, tmp);
                if(!stream)
                {
                    CfgLoadDefValue(recordMap[i][j]);
                }
                else
                {
                    CfgCjsonToDataByMap(recordMap[i][j], stream);
                }
            }
        }
    }

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_RECORD_LoadDefault();
    IPC_CFG_RECORD_Save();
    return 0;
}

int IPC_CFG_RECORD_LoadDefault()
{
    int i, j;

    printf("Enter %s \n", __FUNCTION__);

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        for(j = 0; j < IPC_MAX_VENC_STREAM_PER_CHN; j++)
        {
            CfgLoadDefValue(recordMap[i][j]);
        }
    }

    return 0;
}

IPC_CFG_RECORD_S * IPC_CFG_RECORD_GetParam()
{
    return  &g_stRunRecordCfg;
}

