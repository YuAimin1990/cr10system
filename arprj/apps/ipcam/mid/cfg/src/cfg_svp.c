#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfg_common.h"
#include "cfg_svp.h"

IPC_CFG_SVP_S g_stRunSvpCfg;

CFG_MAP SvpMap[] =
{
   {"enable",			&(g_stRunSvpCfg.u32Enable),			CFG_DATA_TYPE_U32, 		"0",     						"rw", 0, 1,    NULL},
   {"net_name",      	&(g_stRunSvpCfg.achNetName),		CFG_DATA_TYPE_STRING, 	"ssd",   						"rw", 0, 128,  NULL},
   {"npu_bin",      	&(g_stRunSvpCfg.achNpuBinName),		CFG_DATA_TYPE_STRING, 	"/usrdata/local/mbssd/ssd-mobilenet_caffe.npubin",   "rw", 0, 128,  NULL},
   {"vpss_grp",			&(g_stRunSvpCfg.u32VpssGrp),		CFG_DATA_TYPE_U32, 		"1",	  						"rw", 0, 16,  NULL},
   {"vpss_chn",			&(g_stRunSvpCfg.u32VpssChn),		CFG_DATA_TYPE_U32, 		"3",	  						"rw", 0, 5,  NULL},
   {"osd_enable",		&(g_stRunSvpCfg.u32OsdEnable),		CFG_DATA_TYPE_U32, 		"1",	  						"rw", 0, 1,  NULL},
   {"osd_vpss_grp",		&(g_stRunSvpCfg.u32OsdGrp),			CFG_DATA_TYPE_U32, 		"1",	  						"rw", 0, 16,  NULL},
   {"osd_chn",			&(g_stRunSvpCfg.u32OsdChn),			CFG_DATA_TYPE_U32, 		"3",	  						"rw", 0, 5,  NULL},
   {NULL,},
};

AR_S32 IPC_CFG_SVP_LoadDefault()
{
    CfgLoadDefValue(SvpMap);

    return 0;
}

AR_S32 IPC_CFG_SVP_Save()
{
    AR_S32 ret = CfgSave(CFG_SVP_FILE, "svp", SvpMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgSave %s error.", CFG_SVP_FILE);
        return -1;
    }

    return 0;
}


AR_S32 IPC_CFG_SVP_Load(CFG_RUN_MODE_E enMode)
{
    AR_S32 ret = CfgLoad(CFG_SVP_FILE, "svp", SvpMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgLoad %s error.", CFG_SVP_FILE);
        goto err;
    }

    return 0;
err:
    IPC_CFG_SVP_LoadDefault();
    IPC_CFG_SVP_Save();
    return 0;
}


void IPC_CFG_SVP_Print()
{
    int i, j;
    printf("********** SVP *********\n");

    CfgPrintMap(SvpMap);

    printf("********** SVP *********\n\n");
}

IPC_CFG_SVP_S * IPC_CFG_SVP_GetParam()
{
    return &g_stRunSvpCfg;
}
