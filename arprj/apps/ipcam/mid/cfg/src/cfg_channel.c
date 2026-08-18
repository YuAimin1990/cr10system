#include "cfg_channel.h"
#include "cfg_common.h"

IPC_CFG_CHANNEL_S g_stChannelCfg = {0};

static CFG_MAP s_stChannelMap[IPC_MAX_PIPELINE_NUM][3] =
{
    //channel 0
    {
        {"enable",    &g_stChannelCfg.stChannel[0].u32Enable, CFG_DATA_TYPE_U32, "1", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[0].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 1
    {
        {"enable",    &g_stChannelCfg.stChannel[1].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[1].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 2
    {
        {"enable",    &g_stChannelCfg.stChannel[2].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[2].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 3
    {
        {"enable",    &g_stChannelCfg.stChannel[3].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[3].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 4
    {
        {"enable",    &g_stChannelCfg.stChannel[4].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[4].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 5
    {
        {"enable",    &g_stChannelCfg.stChannel[5].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[5].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 6
    {
        {"enable",    &g_stChannelCfg.stChannel[6].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[6].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    },
    //channel 7
    {
        {"enable",    &g_stChannelCfg.stChannel[7].u32Enable, CFG_DATA_TYPE_U32, "0", "rw", 0, 1, "1 enable"},
        {"type",    &g_stChannelCfg.stChannel[7].chType, CFG_DATA_TYPE_STRING, "basic", "rw", 1, MAX_STR_LEN_32, "basic/full"},
        {NULL,},
    }

};

AR_S32 IPC_CFG_CHANNEL_Load(CFG_RUN_MODE_E enMode)
{
    int i = 0;
    char tmp[32] = {0};
    char *data = NULL;

    data = CfgReadFromFile(CHANNEL_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", CHANNEL_CFG_FILE);
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

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        sprintf(tmp, "channel_%d", i);
        CfgParseCjson(json, tmp, s_stChannelMap[i]);
    }

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_CHANNEL_LoadDefault(enMode);
    IPC_CFG_CHANNEL_Save();
    return 0;
}

AR_S32 IPC_CFG_CHANNEL_Save()
{
    cJSON *root, *item;
    char *out;
    int i;
    char tmp[32] = {0};

    root = cJSON_CreateObject();

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
        item = CfgDataToCjsonByMap(s_stChannelMap[i]);
        sprintf(tmp, "channel_%d", i);
        cJSON_AddItemToObject(root, tmp, item);
    }

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(CHANNEL_CFG_FILE, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", CHANNEL_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

void IPC_CFG_CHANNEL_SetDual307()
{
    IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();

    pChCfg->stChannel[1].u32Enable = 1;
}

void IPC_CFG_CHANNEL_SetFour307()
{
    IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();

    pChCfg->stChannel[1].u32Enable = 1;
    pChCfg->stChannel[2].u32Enable = 1;
    pChCfg->stChannel[3].u32Enable = 1;
}

void IPC_CFG_CHANNEL_SetDvp8x1080P()
{
    int i = 0;
    IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        pChCfg->stChannel[i].u32Enable = 1;
    }
}

void IPC_CFG_CHANNEL_SetLowBw(AR_U32 u32ChnNum)
{
    int i = 0;
    IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();

    for(i = 0; i < u32ChnNum; i++)
    {
        pChCfg->stChannel[i].u32Enable = 1;
        strcpy(pChCfg->stChannel[i].chType, "low_bw");
    }
}

void IPC_CFG_CHANNEL_SetInfrared(AR_U32 u32ChnNum)
{
    int i = 0;
    IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();

    for(i = 0; i < u32ChnNum; i++)
    {
        pChCfg->stChannel[i].u32Enable = 1;
        strcpy(pChCfg->stChannel[i].chType, "infrared");
    }
}

void IPC_CFG_CHANNEL_SetFussion()
{
    IPC_CFG_CHANNEL_S * pChCfg = IPC_CFG_CHANNEL_GetParam();
    
    pChCfg->stChannel[0].u32Enable = 1;
    strcpy(pChCfg->stChannel[0].chType, "low_bw");

    pChCfg->stChannel[1].u32Enable = 1;
    strcpy(pChCfg->stChannel[1].chType, "infrared");
}


AR_S32 IPC_CFG_CHANNEL_LoadDefault(CFG_RUN_MODE_E enMode)
{

    int i = 0;
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        CfgLoadDefValue(s_stChannelMap[i]);
    }

    if(enMode == CFG_RUN_MODE_SINGLE_307_LOW_BW || enMode == CFG_RUN_MODE_SINGLE_307_HDR_LOW_BW
        || enMode == CFG_RUN_MODE_SINGLE_415_LOW_BW || enMode == CFG_RUN_MODE_SINGLE_464_LOW_BW
        || enMode == CFG_RUN_MODE_SINGLE_464_HDR_LOW_BW || enMode == CFG_RUN_MODE_SINGLE_307_ARIPC
        || enMode == CFG_RUN_MODE_SINGLE_307_HDR_ARIPC || enMode == CFG_RUN_MODE_SINGLE_415_ARIPC
        || enMode == CFG_RUN_MODE_SINGLE_464_ARIPC || enMode == CFG_RUN_MODE_SINGLE_464_HDR_ARIPC
		|| enMode == CFG_RUN_MODE_SINGLE_530AI_30 
		|| enMode == CFG_RUN_MODE_SINGLE_OV04A10_25 || enMode == CFG_RUN_MODE_SINGLE_OV04A10_25_HDR
		|| enMode == CFG_RUN_MODE_SINGLE_347
		|| enMode == CFG_RUN_MODE_SINGLE_OV04A10_25_ARIPC || enMode == CFG_RUN_MODE_SINGLE_OV04A10_25_HDR_ARIPC
		|| enMode == CFG_RUN_MODE_SINGLE_307_ARS31 || enMode == CFG_RUN_MODE_SINGLE_307_HDR_ARS31
		|| enMode == CFG_RUN_MODE_SINGLE_OV04A10_ARS31 || enMode == CFG_RUN_MODE_SINGLE_OV04A10_HDR_ARS31
		|| enMode == CFG_RUN_MODE_SINGLE_OV05A20_ARS31 
		|| enMode == CFG_RUN_MODE_SINGLE_SC230AI_LOW_BW)
    {
        IPC_CFG_CHANNEL_SetLowBw(1);
    }

    if(enMode == CFG_RUN_MODE_DUAL_307_LOW_BW || enMode == CFG_RUN_MODE_DUAL_307_ARS31
       || enMode == CFG_RUN_MODE_DUAL_SC230AI_LOW_BW)
    {
        IPC_CFG_CHANNEL_SetLowBw(2);
    }

    if(enMode == CFG_RUN_MODE_FOUR_307_LOW_BW)
    {
        IPC_CFG_CHANNEL_SetLowBw(4);
    }

    if(enMode == CFG_RUN_MODE_DVP_8X1080P_25_LOW_BW || enMode == CFG_RUN_MODE_DVP_8X1080P_30_LOW_BW)
    {
        IPC_CFG_CHANNEL_SetLowBw(8);
    }

    if(enMode == CFG_RUN_MODE_SINGLE_TIMO256_ARS31 || enMode == CFG_RUN_MODE_SINGLE_GST417W_ARS31
       || enMode == CFG_RUN_MODE_SINGLE_GST212W4_ARS31 || enMode == CFG_RUN_MODE_SINGLE_H3812C1SG_ARS31)
    {
        IPC_CFG_CHANNEL_SetInfrared(1);
    }
    
    if(enMode == CFG_RUN_MODE_307_timo256_ARS31 || enMode == CFG_RUN_MODE_OV04A10_GST417W_ARS31 
      || enMode == CFG_RUN_MODE_OV04A10_GST212W_ARS31 || enMode == CFG_RUN_MODE_OV05A20_GST212W_ARS31
	  || enMode == CFG_RUN_MODE_307_H3812C1SG_ARS31)
    {
        IPC_CFG_CHANNEL_SetFussion();
    }

    return 0;
}

AR_S32 IPC_CFG_CHANNEL_Print()
{
     int i = 0;
     printf("********** Channel *********\n");
     for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
     {
         CfgPrintMap(s_stChannelMap[i]);
     }
     printf("********** Channel *********\n\n");
     return 0;
}

IPC_CFG_CHANNEL_S * IPC_CFG_CHANNEL_GetParam()
{
    return &g_stChannelCfg;
}

