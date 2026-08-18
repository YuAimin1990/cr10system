#include "cfg_od.h"

IPC_CFG_OD_S g_stRunOdCfg;

CFG_MAP odMap[] =
{
   {"enable",			        &(g_stRunOdCfg.bEnable),				  CFG_DATA_TYPE_S32, "0",     "rw", 0, 1,     NULL},
   {"group_id",                 &(g_stRunOdCfg.u32GrpId),                 CFG_DATA_TYPE_U32, "1",     "rw", 0, 16,    NULL},
   {"channel_id",               &(g_stRunOdCfg.u32ChnId),                 CFG_DATA_TYPE_U32, "0",     "rw", 0, 5,     NULL},
   {"alarmblocknum_threshold",	&(g_stRunOdCfg.u32AlarmBlockNumThred),	  CFG_DATA_TYPE_U32, "30",	  "rw", 1, 48,    "range:1-48"},
   {NULL,},
};

AR_S32 IPC_CFG_OD_LoadDefault()
{
    CfgLoadDefValue(odMap);

    return 0;
}

AR_S32 IPC_CFG_OD_Save()
{
    AR_S32 ret = CfgSave(OD_CFG_FILE, "od", odMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgSave %s error.", OD_CFG_FILE);
        return -1;
    }

    return 0;
}


AR_S32 IPC_CFG_OD_Load(CFG_RUN_MODE_E enMode)
{
    AR_S32 ret = CfgLoad(OD_CFG_FILE, "od", odMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgLoad %s error.", OD_CFG_FILE);
        goto err;
    }

    return 0;
err:
    IPC_CFG_OD_LoadDefault();
    IPC_CFG_OD_Save();
    return 0;
}


void IPC_CFG_OD_Print()
{
    int i, j;
    printf("********** Od *********\n");

    CfgPrintMap(odMap);

    printf("********** Od *********\n\n");
}

IPC_CFG_OD_S * IPC_CFG_OD_GetParam()
{
    return &g_stRunOdCfg;
}
