#include "cfg_system.h"
#include <ctype.h>
/*******This is not checked yet, 2021-06-07 ********/

IPC_CFG_SYSTEM_S g_stRunSystemCfg;

CFG_MAP deviceInfoMap[] = {
    {"deviceName",        &(g_stRunSystemCfg.deviceInfo.deviceName),        CFG_DATA_TYPE_STRING, "IPCAM",    "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"description",       &(g_stRunSystemCfg.deviceInfo.manufacturer),      CFG_DATA_TYPE_STRING, "JH",       "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"deviceType",        &(g_stRunSystemCfg.deviceInfo.deviceType),        CFG_DATA_TYPE_STRING, "IPC_B1",   "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"sensorType",        &(g_stRunSystemCfg.deviceInfo.sensorType),        CFG_DATA_TYPE_S32,    "0",        "rw", 0, 10, "0: IMX222; 1 OV9710 2 ..."},
    {"languageType",      &(g_stRunSystemCfg.deviceInfo.languageType),      CFG_DATA_TYPE_S32,    "1",        "rw", 0, 18, "0: chiness; 1 english 2 ..."},
    {"videoType",         &(g_stRunSystemCfg.deviceInfo.videoType),         CFG_DATA_TYPE_S32,    "0",        "rw", 0, 1,  "0 pal, 1 ntsc"},
    {"isUpdateAPSSid",    &(g_stRunSystemCfg.deviceInfo.isUpdateAPSSid),    CFG_DATA_TYPE_S32,    "1",        "rw", 0, 1, NULL},

    {"serial_0",            &(g_stRunSystemCfg.deviceInfo.serial_0),            CFG_DATA_TYPE_STRING,  "JOY-000836-HYTWE",  "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"serial_1",            &(g_stRunSystemCfg.deviceInfo.serial_1),            CFG_DATA_TYPE_STRING,  "LPURNQ",            "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"serial_2",            &(g_stRunSystemCfg.deviceInfo.serial_2),            CFG_DATA_TYPE_STRING,  "0",   "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"serial_3",            &(g_stRunSystemCfg.deviceInfo.serial_3),            CFG_DATA_TYPE_STRING,  "0",   "rw", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"softwareVersion",     &(g_stRunSystemCfg.deviceInfo.softwareVersion),     CFG_DATA_TYPE_STRING,  "S-1.230.586", "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"softwareBuildDate",   &(g_stRunSystemCfg.deviceInfo.softwareBuildDate),   CFG_DATA_TYPE_STRING,  "20190808",    "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"firmwareVersion",     &(g_stRunSystemCfg.deviceInfo.firmwareVersion),     CFG_DATA_TYPE_STRING,  "F-1.0",       "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"firmwareReleaseDate", &(g_stRunSystemCfg.deviceInfo.firmwareReleaseDate), CFG_DATA_TYPE_STRING,  "20150928",    "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"hardwareVersion",     &(g_stRunSystemCfg.deviceInfo.hardwareVersion),     CFG_DATA_TYPE_STRING,  "H-1.1.1",     "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"hardwareBuildDate",   &(g_stRunSystemCfg.deviceInfo.hardwareBuildDate),   CFG_DATA_TYPE_STRING,  "20150928",    "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"webVersion",          &(g_stRunSystemCfg.deviceInfo.webVersion),          CFG_DATA_TYPE_STRING,  "W-1.0",       "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
    {"webBuildDate",        &(g_stRunSystemCfg.deviceInfo.webBuildDate),        CFG_DATA_TYPE_STRING,  "20150928", "ro", 1, MAX_SYSTEM_STR_SIZE, NULL},
	{"updateSoftware",   &(g_stRunSystemCfg.deviceInfo.updateSoftware),   CFG_DATA_TYPE_S32,    "0",        "rw", 0, 10, "0 not support, 1 support, 2 ready"},

	{NULL,},
};

CFG_MAP timezoneCfgMap[] = {
    {"timeZone", &(g_stRunSystemCfg.timezoneCfg.timezone), CFG_DATA_TYPE_S32, "0", "rw", -720, 720,  NULL},
    {NULL,},
};

CFG_MAP netDstCfgCfgMap[] = {
    {"netDstCfg.enableDST",  &(g_stRunSystemCfg.netDstCfg.enableDST),  CFG_DATA_TYPE_S32,     "0",         "rw", 0, 1,  NULL},
    {"netDstCfg.dSTBias",    &(g_stRunSystemCfg.netDstCfg.dSTBias),    CFG_DATA_TYPE_S32,     "0",         "rw", 0, 360,  NULL},
    {"netDstCfg.beginTime",  &(g_stRunSystemCfg.netDstCfg.beginTime),  CFG_DATA_TYPE_STRING,  "2015.8.30", "rw", 1, MAX_TIME_STR_SIZE,  NULL},
    {"netDstCfg.endTime",    &(g_stRunSystemCfg.netDstCfg.endTime),    CFG_DATA_TYPE_STRING,  "2015.8.30", "rw", 1, MAX_TIME_STR_SIZE,  NULL},
    {NULL,},
};

CFG_MAP ntpCfgMap[] = {
    {"ntpCfg_enable",        &(g_stRunSystemCfg.ntpCfg.enable),        CFG_DATA_TYPE_S32,     "1",               "rw", 0, 1,  NULL},
    {"ntpCfg_serverDomain",  &(g_stRunSystemCfg.ntpCfg.serverDomain),  CFG_DATA_TYPE_STRING,  "time-a.nist.gov", "rw", 1, MAX_URL_STR_SIZE, NULL},
    {NULL,},
};


CFG_MAP maintainCfgMap[] = {
    {"enable",   &(g_stRunSystemCfg.maintainCfg.enable),  CFG_DATA_TYPE_U8,    "0",  "rw", 0, 1,  NULL},
    {"index",    &(g_stRunSystemCfg.maintainCfg.index),   CFG_DATA_TYPE_U8,    "2",  "rw", 0, 7,  NULL},
    {"hour",     &(g_stRunSystemCfg.maintainCfg.hour),    CFG_DATA_TYPE_U8,    "3",  "rw", 0, 23,   NULL},
    {"minute",   &(g_stRunSystemCfg.maintainCfg.minute),  CFG_DATA_TYPE_U8,    "0",  "rw", 0, 59,   NULL},
    {"second",   &(g_stRunSystemCfg.maintainCfg.second),  CFG_DATA_TYPE_U8,    "0",  "rw", 0, 59,   NULL},
    {NULL,},
};

void IPC_CFG_SYSTEM_Print()
{
    printf("*************** System **************\n");

    printf("device info:\n");
    CfgPrintMap(deviceInfoMap);
    printf("\n");

    printf("timezone:\n");
    CfgPrintMap(timezoneCfgMap);
    printf("\n");

    printf("netDST:\n");
    CfgPrintMap(netDstCfgCfgMap);
    printf("\n");

    printf("ntp:\n");
    CfgPrintMap(ntpCfgMap);
    printf("\n");

	printf("maintain:\n");
    CfgPrintMap(maintainCfgMap);
    printf("\n");

    printf("*************** System **************\n\n");
}

AR_S32 IPC_CFG_SYSTEM_Save()
{
    cJSON *root;
    char *out;

    root = cJSON_CreateObject();

    CfgAddCjson(root, "deviceinfo", deviceInfoMap);
    CfgAddCjson(root, "timezone", timezoneCfgMap);
    CfgAddCjson(root, "netDST", netDstCfgCfgMap);
    CfgAddCjson(root, "NTP", ntpCfgMap);
	CfgAddCjson(root, "Maintain", maintainCfgMap);

    out = cJSON_Print(root);

    AR_S32 ret = CfgWriteToFile(SYSTEM_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", SYSTEM_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

AR_S32 IPC_CFG_SYSTEM_LoadDefault()
{
    CfgLoadDefValue(deviceInfoMap);
    CfgLoadDefValue(timezoneCfgMap);
    CfgLoadDefValue(netDstCfgCfgMap);
    CfgLoadDefValue(ntpCfgMap);
	CfgLoadDefValue(maintainCfgMap);

    return 0;
}

AR_S32 IPC_CFG_SYSTEM_Load(CFG_RUN_MODE_E enMode)
{
    int ret = 0;
    char *data = NULL;
    data = CfgReadFromFile(SYSTEM_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", SYSTEM_CFG_FILE);
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

    ret = CfgParseCjson(json, "deviceinfo", deviceInfoMap);
    if(ret < 0)
    {
        CfgLoadDefValue(deviceInfoMap);
    }
    ret = CfgParseCjson(json, "timezone", timezoneCfgMap);
    if(ret < 0)
    {
        CfgLoadDefValue(timezoneCfgMap);
    }
    ret = CfgParseCjson(json, "netDST", netDstCfgCfgMap);
    if(ret < 0)
    {
        CfgLoadDefValue(netDstCfgCfgMap);
    }
    ret = CfgParseCjson(json, "NTP", ntpCfgMap);
    if(ret < 0)
    {
        CfgLoadDefValue(ntpCfgMap);
    }
	ret = CfgParseCjson(json, "Maintain", maintainCfgMap);
    if(ret < 0)
    {
        CfgLoadDefValue(maintainCfgMap);
    }

    cJSON_Delete(json);
    free(data);

    return 0;

err:
    IPC_CFG_SYSTEM_LoadDefault();
    IPC_CFG_SYSTEM_Save();
    return 0;
}


cJSON *SystemCfgGetNTPJsonSting()
{
     cJSON *json = CfgDataToCjsonByMap(ntpCfgMap);
    return json;
}


char *SytemCfgGetCjsonString()
{
    cJSON *json = NULL;
    char *buf;
    json = CfgDataToCjsonByMap(deviceInfoMap);
    buf = cJSON_Print(json);
    cJSON_Delete(json);

    return buf;
}

static char *id_tmp0 = "JOY-000836-HYTWE";
static char *id_tmp1 = "LPURNQ";

/* 1:used; 0/-1:not used */
static AR_S32 check_p2p_id(char *src)
{
    char *pBuf = src;
    char *pBegin = NULL;
    char tmp[100] = {0};

    pBegin = strstr(pBuf, " ");
    if(!pBegin)
    {
        PRINT_ERR("%s no space\n", src);
        return -1;
    }

    pBegin += 1;
    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, pBegin, 1);

    AR_S32 enable = atoi(tmp);

    if (enable == 1) { //used
        return 1;
    } else {
        return 0;
    }

}

#if 0
static AR_S32 to_patch_p2p_id(char *src, char *dst)
{
    char *pBuf = src;
    char *pBegin = NULL;
    char tmp[100] = {0};

    pBegin = strstr(pBuf, " ");
    if(!pBegin)
    {
        PRINT_ERR();
        return -1;
    }
    AR_S32 id_len = pBegin - pBuf;
    memset(tmp, 0, sizeof(tmp));
    strncpy(dst, pBuf, id_len);

    pBegin += 1;
    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, pBegin, 1);

    AR_S32 enable = atoi(tmp);

    if (enable == 1) { //used
        PRINT_INFO("%s used\n", dst);
        return 1;
    } else {
        PRINT_INFO("%s not used\n", dst);
        *pBegin = '1';
        return 0;
    }
}
#endif

static AR_S32 bak_old_p2pid(char *id0, char *id1)
{
    char id[100] = {0};
    sprintf(id, "%s,%s", id0, id1);

    FILE *fp = fopen(P2PID_BAK_CFG_PATH, "rb");
    if (fp == NULL) {
        PRINT_ERR();
        return -1;
    }

    char *ptr = NULL;
    char tmp[100] = {0};
    AR_S32 is_exist = 0;
    while (!feof(fp)) {
        memset(tmp, 0, sizeof(tmp));
        ptr = fgets(tmp, sizeof(tmp), fp);
        if(ptr <= 0) {
            //PRINT_ERR();
            continue;
        }

        if (strncmp(tmp, id, strlen(id)) == 0) {
            PRINT_INFO("%s is exist\n", id);
            is_exist = 1;
            break;
        }
    }

    if (is_exist == 0) {
        PRINT_INFO("to bak p2p id:%s\n", id);
        fseek(fp, 0, SEEK_END);
        char id_tmp[100] = {0};
        sprintf(id_tmp, "%s\n", id);
        fwrite(id_tmp, 1, strlen(id_tmp), fp);
        fflush(fp);
    }

    fclose(fp);

    return 0;
}

static AR_S32 update_systme_p2p(char *id)
{
    char *pBuf = id;
    char *pBegin = NULL;
    //char tmp[100] = {0};
    AR_S32 len1 = 0;
    AR_S32 len2 = 0;
    pBegin = strstr(pBuf, ",");
    if(!pBegin)
    {
        PRINT_ERR();
        return -1;
    }
    //len1 = pBegin - pBuf;
    len1 = strlen(id_tmp0);
    len2 = strlen(id_tmp1);

    if ((strlen(g_stRunSystemCfg.deviceInfo.serial_0) == 16) && (strlen(g_stRunSystemCfg.deviceInfo.serial_1) == 6)) {
        bak_old_p2pid(g_stRunSystemCfg.deviceInfo.serial_0, g_stRunSystemCfg.deviceInfo.serial_1);
    }

    memset(g_stRunSystemCfg.deviceInfo.serial_0, 0, sizeof(g_stRunSystemCfg.deviceInfo.serial_0));
    strncpy(g_stRunSystemCfg.deviceInfo.serial_0, pBuf, len1);

    pBegin += 1;

    memset(g_stRunSystemCfg.deviceInfo.serial_1, 0, sizeof(g_stRunSystemCfg.deviceInfo.serial_1));
    strncpy(g_stRunSystemCfg.deviceInfo.serial_1, pBegin, len2);

    PRINT_INFO("use p2p_id:%s %s\n", g_stRunSystemCfg.deviceInfo.serial_0, g_stRunSystemCfg.deviceInfo.serial_1);

    IPC_CFG_SYSTEM_Save();
    return 0;
}

static void change_id(char *src, char *dst)
{
    char tmp[100] = {0};
    strncpy(tmp, src, strlen(id_tmp0) + strlen(id_tmp1) + 1);

    sprintf(dst, "%s 1\n", tmp);
}

AR_S32 P2PIDLoadFromSd()
{
    //目录不存在，则返回
    if(access(P2PID_CFG_PATH, F_OK) != 0 ) {
        PRINT_INFO("%s not exist\n", P2PID_CFG_PATH);
        return -1;
    }

    PRINT_INFO("to load %s\n", P2PID_CFG_PATH);

    FILE *fp = NULL;
    FILE *fp2 = NULL;
    fp = fopen(P2PID_CFG_PATH, "rb+");
    if (fp == NULL) {
        PRINT_ERR("fopen %s error.", P2PID_CFG_PATH);
        goto err;
    }

    fp2 = fopen(P2PID_TMP_CFG_PATH, "wb+");
    if (fp2 == NULL) {
        PRINT_ERR("fopen %s error.", P2PID_TMP_CFG_PATH);
        goto err;
    }

    char *ptr = NULL;
    char tmp[1024] = {0};
    char dst[1024] = {0};
    AR_S32 ret = 0;
    AR_S32 load_id_ok = 0;
    //AR_S32 offset1 = 0;
    //AR_S32 offset2 = 0;
    while (!feof(fp)) {

        //offset1 = ftell(fp);
        memset(tmp, 0, sizeof(tmp));
        ptr = fgets(tmp, sizeof(tmp), fp);
        if(ptr <= 0) {
            //PRINT_ERR();
            continue;
        }

        if (load_id_ok) {
            fputs(tmp, fp2);
            fflush(fp2);
        } else {
            ret = check_p2p_id(tmp);
            if (ret == 1) {
                fputs(tmp, fp2);
                fflush(fp2);

                continue;
            } else {
                update_systme_p2p(tmp);
                load_id_ok = 1;

                change_id(tmp, dst);
                fputs(dst, fp2);
                fflush(fp2);
                continue;
            }
        }

        #if 0
        //offset2 = ftell(fp);
        //printf("offset1=%d, offset2=%d \n", offset1, offset2);

        memset(dst, 0, sizeof(dst));
        ret = to_patch_p2p_id(tmp, dst);
        if (ret != 0) {
            continue;
        } else {
            // 1 to update id.txt
            fseek(fp, offset1, SEEK_SET);
            //printf("len=%d, tmp:%s", strlen(tmp), tmp);
            fwrite(tmp, 1, strlen(tmp), fp);
            fflush(fp);

            // to update system.cjson
            update_systme_p2p(dst);

            load_id_ok = 1;
            break;
        }
        #endif
    }

    if (fp) {
        fclose(fp);
    }

    if (fp2) {
        fclose(fp2);
    }

    // led twinkle to show load p2p id ok
    if (load_id_ok)
    {
        remove(P2PID_CFG_PATH);//删除原文件
        rename(P2PID_TMP_CFG_PATH, P2PID_CFG_PATH);//将临时文件名改为原文件名

    }
    else
    {
        remove(P2PID_TMP_CFG_PATH); //tmp文件为空
    }

    return 0;

err:
    if (fp)
    {
        fclose(fp);
    }

    if (fp2)
    {
        fclose(fp2);
    }

    return -1;
}

IPC_CFG_SYSTEM_S * IPC_CFG_SYSTEM_GetParam()
{
    return &g_stRunSystemCfg;
}


