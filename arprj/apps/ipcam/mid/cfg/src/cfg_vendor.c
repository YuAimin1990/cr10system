#include "cfg_vendor.h"

IPC_CFG_VENDOR_S g_stRunVendorCfg;

CFG_MAP vendorMap[] = {
    {"name",             &g_stRunVendorCfg.name,      CFG_DATA_TYPE_STRING, "artosyn", "rw", 0, MAX_STR_LEN_128, "vendor name"},
    {NULL,},
};

void IPC_CFG_VENDOR_Print()
{
    printf("********** Vendor *********\n");
    CfgPrintMap(vendorMap);
    printf("********** Vendor *********\n\n");
}

AR_S32 IPC_CFG_VENDOR_Save()
{
    AR_S32 ret = CfgSave(VENDOR_CFG_FILE, "vendor", vendorMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgSave %s error.", VENDOR_CFG_FILE);
        return -1;
    }

    return 0;
}

AR_S32 IPC_CFG_VENDOR_Load(CFG_RUN_MODE_E enMode)
{
    AR_S32 ret = CfgLoad(VENDOR_CFG_FILE, "vendor", vendorMap);
    if (ret != 0)
    {
        PRINT_ERR("CfgLoad %s error.", VENDOR_CFG_FILE);
        goto err;
    }

    return 0;
err:
    IPC_CFG_VENDOR_LoadDefault();
    IPC_CFG_VENDOR_Save();
    return 0;
}

AR_S32 IPC_CFG_VENDOR_LoadDefault()
{
    CfgLoadDefValue(vendorMap);

    return 0;
}

IPC_CFG_VENDOR_S * IPC_CFG_VENDOR_GetParam()
{
    return &g_stRunVendorCfg;
}

