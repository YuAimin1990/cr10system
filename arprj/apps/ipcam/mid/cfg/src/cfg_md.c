#include "cfg_md.h"

IPC_CFG_MD_S g_stRunMdCfg;

CFG_MAP mdMap[] =
{

   {"enable",			&(g_stRunMdCfg.bEnable),				CFG_DATA_TYPE_S32, "0",     "rw", 0, 1,     NULL},
   {"group_id",         &(g_stRunMdCfg.u32GrpId),               CFG_DATA_TYPE_U32, "1",     "rw", 0, 16,    NULL},
   {"channel_id",       &(g_stRunMdCfg.u32ChnId),               CFG_DATA_TYPE_U32, "0",     "rw", 0, 5,     NULL},
   {"match_threshold",	&(g_stRunMdCfg.u16MatchingThr),	        CFG_DATA_TYPE_U32, "20",	"rw", 0, 100,   "range:0-255"},
   {"area_percentage",	&(g_stRunMdCfg.u32AreaPercentage), 	    CFG_DATA_TYPE_U32, "20",	"rw", 0, 100,   "range:0-100"},
   {"post_method",  	&(g_stRunMdCfg.u32PostMethod),		    CFG_DATA_TYPE_U32, "0",	    "rw", 0, 1,   	"range:0-1"},
   {NULL,},

};


AR_S32 IPC_CFG_MD_LoadDefault()
{
    CfgLoadDefValue(mdMap);

    return 0;
}

AR_S32 IPC_CFG_MD_Save()
{
    AR_S32 ret = CfgSave(MD_CFG_FILE, "md", mdMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgSave %s error.", MD_CFG_FILE);
        return -1;
    }

    return 0;
}


AR_S32 IPC_CFG_MD_Load(CFG_RUN_MODE_E enMode)
{
    AR_S32 ret = CfgLoad(MD_CFG_FILE, "md", mdMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgLoad %s error.", MD_CFG_FILE);
        goto err;
    }

    return 0;
err:
    IPC_CFG_MD_LoadDefault();
    IPC_CFG_MD_Save();
    return 0;
}


void IPC_CFG_MD_Print()
{
    int i, j;
    printf("********** Md *********\n");

    CfgPrintMap(mdMap);

    printf("********** Md *********\n\n");
}

IPC_CFG_MD_S * IPC_CFG_MD_GetParam()
{
    return &g_stRunMdCfg;
}
