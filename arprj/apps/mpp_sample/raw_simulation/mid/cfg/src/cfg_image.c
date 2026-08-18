#include "cfg_image.h"

IPC_CFG_IMAGE_S g_stRunImageCfg;


CFG_MAP imageMap[IPC_MAX_PIPELINE_NUM][20] =
{
    //VI 0
    {
        {"scene_mode",     &g_stRunImageCfg.stImage[0].sceneMode,     CFG_DATA_TYPE_U8, "0", "rw", 0, 2, "0 auto 1 indoor 2 outdoor"},
        {"image_style",    &g_stRunImageCfg.stImage[0].imageStyle,    CFG_DATA_TYPE_U8, "0", "rw", 0, 2, "0 nomal 1 lightness 2 bright"},
        {"infra_enable",       &g_stRunImageCfg.stImage[0].infraEnable,       CFG_DATA_TYPE_U8, "0", "rw", 0, 1,  NULL},
        {"irCut_control",  &g_stRunImageCfg.stImage[0].irCutControlMode,  CFG_DATA_TYPE_U8, "0", "rw", 0, 1, "0 hardware,  1 software"},
        {"irCut_mode",         &g_stRunImageCfg.stImage[0].irCutMode,         CFG_DATA_TYPE_U8, "1", "rw", 0, 2, "0 auto, 1 day, 2 night"},
        {"denoise2d_strength",   &g_stRunImageCfg.stImage[0].strengthDenoise2d,   CFG_DATA_TYPE_U8, "1", "rw", 0, 100, NULL},
        {"denoise3d_strength",   &g_stRunImageCfg.stImage[0].strengthDenoise3d, CFG_DATA_TYPE_U8, "50", "rw", 0, 100, NULL},
        {"lowlight_mode",    &g_stRunImageCfg.stImage[0].lowlightMode,    CFG_DATA_TYPE_U8,  "3", "rw", 0, 3, "0 close, 1 only night, 2 day-night, 3 auto"},
        {"exposure_mode",    &g_stRunImageCfg.stImage[0].exposureMode,    CFG_DATA_TYPE_U8,  "0", "rw", 0, 2, "0 - auto 1 - bright, 2 - dark"},
        {"dcIris_enable",    &g_stRunImageCfg.stImage[0].dcIrisEnable,    CFG_DATA_TYPE_U8,  "1", "rw", 0, 1, NULL},
        {"anti_flicker_freq", &g_stRunImageCfg.stImage[0].antiFlickerFreq, CFG_DATA_TYPE_U8,  "50", "rw", 50, 60, "50HZ 60HZ"},
        {"backLight_enable", &g_stRunImageCfg.stImage[0].backLightEnable, CFG_DATA_TYPE_U8,  "1", "rw", 0, 1, NULL},
        {"backLight_level",  &g_stRunImageCfg.stImage[0].backLightLevel,  CFG_DATA_TYPE_S32, "3", "rw", 0, 100, NULL},
        {"brightness",  &g_stRunImageCfg.stImage[0].brightness, CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"saturation",  &g_stRunImageCfg.stImage[0].saturation, CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"contrast",    &g_stRunImageCfg.stImage[0].contrast,   CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"sharpness",   &g_stRunImageCfg.stImage[0].sharpness,  CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"hue",         &g_stRunImageCfg.stImage[0].hue,        CFG_DATA_TYPE_S32, "50",   "rw", 0, 100, NULL},
        {NULL,}
    },
    //VI 1
    {
        {"scene_mode",     &g_stRunImageCfg.stImage[1].sceneMode,     CFG_DATA_TYPE_U8, "0", "rw", 0, 2, "0 auto 1 indoor 2 outdoor"},
        {"image_style",    &g_stRunImageCfg.stImage[1].imageStyle,    CFG_DATA_TYPE_U8, "0", "rw", 0, 2, "0 nomal 1 lightness 2 bright"},
        {"infra_enable",       &g_stRunImageCfg.stImage[1].infraEnable,       CFG_DATA_TYPE_U8, "0", "rw", 0, 1,  NULL},
        {"irCut_control",  &g_stRunImageCfg.stImage[1].irCutControlMode,  CFG_DATA_TYPE_U8, "0", "rw", 0, 1, "0 hardware,  1 software"},
        {"irCut_mode",         &g_stRunImageCfg.stImage[1].irCutMode,         CFG_DATA_TYPE_U8, "1", "rw", 0, 2, "0 auto, 1 day, 2 night"},
        {"denoise2d_strength",   &g_stRunImageCfg.stImage[1].strengthDenoise2d,   CFG_DATA_TYPE_U8, "1", "rw", 0, 100, NULL},
        {"denoise3d_strength",   &g_stRunImageCfg.stImage[1].strengthDenoise3d,   CFG_DATA_TYPE_U8, "50", "rw", 0, 100, NULL},
        {"lowlight_mode",    &g_stRunImageCfg.stImage[1].lowlightMode,    CFG_DATA_TYPE_U8,  "3", "rw", 0, 3, "0 close, 1 only night, 2 day-night, 3 auto"},
        {"exposure_mode",    &g_stRunImageCfg.stImage[1].exposureMode,    CFG_DATA_TYPE_U8,  "0", "rw", 0, 2, "0 - auto 1 - bright, 2 - dark"},
        {"dcIris_enable",    &g_stRunImageCfg.stImage[1].dcIrisEnable,    CFG_DATA_TYPE_U8,  "1", "rw", 0, 1, NULL},
        {"anti_flicker_freq", &g_stRunImageCfg.stImage[1].antiFlickerFreq, CFG_DATA_TYPE_U8,  "50", "rw", 50, 60, "50HZ 60HZ"},
        {"backLight_enable", &g_stRunImageCfg.stImage[1].backLightEnable, CFG_DATA_TYPE_U8,  "1", "rw", 0, 1, NULL},
        {"backLight_level",  &g_stRunImageCfg.stImage[1].backLightLevel,  CFG_DATA_TYPE_S32, "3", "rw", 0, 100, NULL},
        {"brightness",  &g_stRunImageCfg.stImage[1].brightness, CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"saturation",  &g_stRunImageCfg.stImage[1].saturation, CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"contrast",    &g_stRunImageCfg.stImage[1].contrast,   CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"sharpness",   &g_stRunImageCfg.stImage[1].sharpness,  CFG_DATA_TYPE_S32, "50", "rw", 0, 100, NULL},
        {"hue",         &g_stRunImageCfg.stImage[1].hue,        CFG_DATA_TYPE_S32, "50",   "rw", 0, 100, NULL},
        {NULL,}
    }
    //VI 2 - 7 to be added.

};

void IPC_CFG_IMAGE_Print()
{
    int i = 0;
    printf("********** Image *********\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        CfgPrintMap(imageMap[i]);
    }
    printf("********** Image *********\n\n");
}

AR_S32 IPC_CFG_IMAGE_Save()
{
    cJSON *root, *item;
    char *out;
    int i;
    char tmp[32] = {0};

    root = cJSON_CreateObject();

    for (i = 0; i < IPC_MAX_PIPELINE_NUM; i ++)
    {
            item = CfgDataToCjsonByMap(imageMap[i]);
            sprintf(tmp, "image_vi_%d", i);
            cJSON_AddItemToObject(root, tmp, item);
    }

    out = cJSON_Print(root);

    int ret = CfgWriteToFile(IMAGE_CFG_FILE, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", IMAGE_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

AR_S32 IPC_CFG_IMAGE_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    char tmp[32] = {0};
    int i;

    data = CfgReadFromFile(IMAGE_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", IMAGE_CFG_FILE);
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

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        sprintf(tmp, "channel_%d", i);
        chn = cJSON_GetObjectItem(json, tmp);
        if(!chn)
        {
            CfgLoadDefValue(imageMap[i]);
        }
        else
        {
            CfgCjsonToDataByMap(imageMap[i], chn);
        }
    }

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_IMAGE_LoadDefault();
    IPC_CFG_IMAGE_Save();
    return 0;
}

AR_S32 IPC_CFG_IMAGE_LoadDefault()
{
    int i = 0;
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        CfgLoadDefValue(imageMap[i]);
    }

    return 0;
}

IPC_CFG_IMAGE_S * IPC_CFG_IMAGE_GetParam()
{
    return &g_stRunImageCfg;
}