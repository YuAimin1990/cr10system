
#include "cfg_gpio.h"

IPC_CFG_GPIO_S g_stRunGpioCfg;


CFG_MAP gpioRecMap[] ={
    {"enable",  &(g_stRunGpioCfg.rec.enable),     CFG_DATA_TYPE_S32, "0",  "rw", 0, 1,  NULL},
    {"gpio",    &(g_stRunGpioCfg.rec.gpio_name),   CFG_DATA_TYPE_STRING, "Cn_m", "rw", 1, 8,  NULL},
	{"value",   &(g_stRunGpioCfg.rec.gpio_value), CFG_DATA_TYPE_S32, "0",  "rw", 0, 1,  NULL},
    {NULL,}
};


CFG_MAP gpioLedMap[] ={
    {"enable",  &(g_stRunGpioCfg.led.enable),     CFG_DATA_TYPE_S32, "0",  "rw", 0, 1,  NULL},
    {"gpio",    &(g_stRunGpioCfg.led.gpio_name),   CFG_DATA_TYPE_STRING, "Dn_m", "rw", 1, 8,  NULL},
	{"value",   &(g_stRunGpioCfg.led.gpio_value), CFG_DATA_TYPE_S32, "0",  "rw", 0, 1,  NULL},
	{NULL,}
};

CFG_MAP gpioIrcutMap[] ={
    {"enable",  &(g_stRunGpioCfg.ircut.enable),     CFG_DATA_TYPE_S32, "1",  "rw", 0, 1,  NULL},
    {"enable_gpio",    &(g_stRunGpioCfg.ircut.enable_gpio_name),   CFG_DATA_TYPE_STRING, "C1_3", "rw", 1, 8,  NULL},
	{"fbc_gpio",   &(g_stRunGpioCfg.ircut.fbc_gpio_name), CFG_DATA_TYPE_STRING, "C1_2",  "rw", 1, 8,  NULL},
	{NULL,}
};

void IPC_CFG_GPIO_Print()
{
    printf("*************** gpio info **************\n");
    printf("rec_snap\n");
    CfgPrintMap(gpioRecMap);
    printf("\n");

    printf("led\n");
    CfgPrintMap(gpioLedMap);
    printf("\n");

    printf("ircut\n");
    CfgPrintMap(gpioIrcutMap);
    printf("\n");
    printf("*************** gpio info **************\n\n");
}

AR_S32 IPC_CFG_GPIO_Save()
{
    cJSON *root;
    char *out;

    root = cJSON_CreateObject();

    CfgAddCjson(root, "rec_snap", gpioRecMap);
    CfgAddCjson(root, "led", gpioLedMap);
    CfgAddCjson(root, "ircut", gpioIrcutMap);

    out = cJSON_Print(root);

    AR_S32 ret = CfgWriteToFile(GPIO_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", GPIO_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

//for 38x38 IPC board, ircut/led gpio is different with evb
static void IPC_CFG_GPIO_LoadIpcBoard()
{
    g_stRunGpioCfg.ircut.enable = 1;
    strcpy(g_stRunGpioCfg.ircut.enable_gpio_name, "A0_3");
    strcpy(g_stRunGpioCfg.ircut.fbc_gpio_name, "B2_5");

    g_stRunGpioCfg.led.enable = 1;
    strcpy(g_stRunGpioCfg.led.gpio_name, "D1_7");
}

//for ARS31 EVB board, ircut/led gpio is different
static void IPC_CFG_GPIO_LoadARS31_Evb()
{
#if 0
    g_stRunGpioCfg.ircut.enable = 1;
    strcpy(g_stRunGpioCfg.ircut.enable_gpio_name, "A0_3");
    strcpy(g_stRunGpioCfg.ircut.fbc_gpio_name, "B2_5");

    g_stRunGpioCfg.led.enable = 1;
    strcpy(g_stRunGpioCfg.led.gpio_name, "D1_7");
#endif
}

AR_S32 IPC_CFG_GPIO_LoadDefault(CFG_RUN_MODE_E enMode)
{
    CfgLoadDefValue(gpioRecMap);
    CfgLoadDefValue(gpioLedMap);
    CfgLoadDefValue(gpioIrcutMap);

    if(enMode == CFG_RUN_MODE_SINGLE_307_ARIPC || enMode == CFG_RUN_MODE_SINGLE_307_HDR_ARIPC
      || enMode == CFG_RUN_MODE_SINGLE_415_ARIPC || enMode == CFG_RUN_MODE_SINGLE_464_ARIPC
      || enMode == CFG_RUN_MODE_SINGLE_464_HDR_ARIPC 
      || enMode == CFG_RUN_MODE_SINGLE_OV04A10_25 || enMode == CFG_RUN_MODE_SINGLE_OV04A10_25_HDR
      || enMode == CFG_RUN_MODE_SINGLE_OV04A10_25_ARIPC || enMode == CFG_RUN_MODE_SINGLE_OV04A10_25_HDR_ARIPC)
    {
        IPC_CFG_GPIO_LoadIpcBoard();
    }

    if(enMode == CFG_RUN_MODE_307_timo256_ARS31 || enMode == CFG_RUN_MODE_SINGLE_307_ARS31 
      || enMode == CFG_RUN_MODE_SINGLE_307_HDR_ARS31 || enMode == CFG_RUN_MODE_SINGLE_OV04A10_ARS31
      || enMode == CFG_RUN_MODE_SINGLE_OV04A10_HDR_ARS31 || CFG_RUN_MODE_SINGLE_TIMO256_ARS31
      || enMode == CFG_RUN_MODE_SINGLE_GST417W_ARS31 || enMode == CFG_RUN_MODE_SINGLE_OV05A20_ARS31)
    {
        IPC_CFG_GPIO_LoadARS31_Evb();
    }

    return 0;
}

AR_S32 IPC_CFG_GPIO_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    data = CfgReadFromFile(GPIO_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", GPIO_CFG_FILE);
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

    CfgParseCjson(json, "rec_snap", gpioRecMap);
    CfgParseCjson(json, "led", gpioLedMap);
    CfgParseCjson(json, "ircut", gpioIrcutMap);

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_GPIO_LoadDefault(enMode);
    IPC_CFG_GPIO_Save();
    return 0;
}

IPC_CFG_GPIO_S * IPC_CFG_GPIO_GetParam()
{
    return &g_stRunGpioCfg;
}

