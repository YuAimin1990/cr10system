#include "cfg_common.h"

pthread_mutex_t g_cfg_write_mutex = PTHREAD_MUTEX_INITIALIZER;


int CfgWriteToFile(const char *filename, const char *data)
{
    pthread_mutex_lock(&g_cfg_write_mutex);
    char path[100];

    memset(path, 0, sizeof(path));
    sprintf(path, "%s%s", CFG_DIR, filename);
    PRINT_INFO("to save %s\n", path);

    FILE *fp = NULL;
    fp = fopen(path, "wb+");
    if (fp == NULL) {
        PRINT_ERR();
        pthread_mutex_unlock(&g_cfg_write_mutex);
        return -1;
    }

    int len = strlen(data);
    if (fwrite(data, 1, len, fp) != len) {
        PRINT_ERR();
        fclose(fp);
        pthread_mutex_unlock(&g_cfg_write_mutex);
        return -1;
    }

	fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&g_cfg_write_mutex);
    return 0;
}

char *CfgReadFromFile(const char *filename)
{
    char path[100];

    //目录不存在，则创建
    /*if(access(CFG_DIR, F_OK) != 0 ) {
        PRINT_INFO("to create cfg dir: %s\n", CFG_DIR);
        if((mkdir(CFG_DIR, 0777)) < 0)
        {
            PRINT_ERR("mkdir %s failed\n", CFG_DIR);
            return NULL;
        }
    }*/

    memset(path, 0, sizeof(path));
    sprintf(path, "%s%s", CFG_DIR, filename);
    PRINT_INFO("to load %s\n", path);

    FILE *fp = NULL;
    fp = fopen(path, "rb");
    if (fp == NULL) {

        return NULL;
    }

    int fileSize;
    if (0 != fseek(fp, 0, SEEK_END)) {
        fclose(fp);
        return NULL;
    }
    fileSize = ftell(fp);

    char *data = NULL;
    data = malloc(fileSize);
    if(!data) {
        PRINT_ERR();
        fclose(fp);
        return NULL;
    }
    memset(data, 0, fileSize);

    if(0 != fseek(fp, 0, SEEK_SET)) {
        PRINT_ERR();
        free(data);
        fclose(fp);
        return NULL;
    }
    if (fread(data, 1, fileSize, fp) != (fileSize)) {
        PRINT_ERR();
        free(data);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    return data;
}

char *CfgReadFromPath(const char *path)
{
    //目录不存在，则返回
    if(access(path, F_OK) != 0 ) {
        PRINT_INFO("%s not exist\n", path);
        return NULL;
    }

    PRINT_INFO("to load %s\n", path);

    FILE *fp = NULL;
    fp = fopen(path, "rb");
    if (fp == NULL) {
        PRINT_ERR("fopen %s error.", path);
        return NULL;
    }

    int fileSize;
    if (0 != fseek(fp, 0, SEEK_END)) {
        fclose(fp);
        return NULL;
    }
    fileSize = ftell(fp);

    char *data = NULL;
    data = malloc(fileSize);
    if(!data) {
        PRINT_ERR();
        fclose(fp);
        return NULL;
    }
    memset(data, 0, fileSize);

    if(0 != fseek(fp, 0, SEEK_SET)) {
        PRINT_ERR();
        free(data);
        fclose(fp);
        return NULL;
    }
    if (fread(data, 1, fileSize, fp) != (fileSize)) {
        PRINT_ERR();
        free(data);
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    return data;
}


static int CfgLoadDefValueItem(CFG_MAP *map)
{
    int tmp;
	double temp;
    switch (map->dataType) {
        case CFG_DATA_TYPE_U32:
            tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned int *)(map->dataAddress)) = (unsigned int)tmp;
            break;
        case CFG_DATA_TYPE_U16:
            tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned short *)(map->dataAddress)) = (unsigned short)tmp;
            break;
        case CFG_DATA_TYPE_U8:
            tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned char *)(map->dataAddress)) = (unsigned char)tmp;
            break;
        case CFG_DATA_TYPE_S32:
            tmp = strtol(map->defaultValue, NULL, 0);
            *((int *)(map->dataAddress)) = tmp;
            break;
        case CFG_DATA_TYPE_S16:
            tmp = strtol(map->defaultValue, NULL, 0);
            *((signed short *)(map->dataAddress)) = (signed short)tmp;
            break;
        case CFG_DATA_TYPE_S8:
            tmp = strtol(map->defaultValue, NULL, 0);
            *((signed char *)(map->dataAddress)) = (signed char)tmp;
            break;
		case CFG_DATA_TYPE_FLOAT:
			temp = strtod(map->defaultValue, NULL);
            *(( float *)(map->dataAddress)) = (float)temp;
            break;

        case CFG_DATA_TYPE_STRING:
			if (map->dataAddress && (map->max > 1)) {
				tmp = (int)map->max - 1;
                strncpy((char *)map->dataAddress, map->defaultValue, tmp);
                ((char *)map->dataAddress)[tmp] = '\0';
            } else {
                PRINT_ERR("if type is string, addr can't be null and the upper limit must greater than 1.");
            }
            break;
        case CFG_DATA_TYPE_STIME:
            {
                CFG_SCHEDTIME *stime = (CFG_SCHEDTIME *)map->dataAddress;
                int i, j;
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 4; j ++) {
                        stime->startHour = 0;
                        stime->startMin = 0;
                        stime->stopHour = 23;
                        stime->stopMin = 59;
                        stime ++;
                    }
                }
            }
            break;
        case CFG_DATA_TYPE_SLICE:
            {
                unsigned int *slice = (unsigned int *)map->dataAddress;
                int i, j;
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 3; j ++) {
                        *slice = strtoul("4294967295", NULL, 0);
                        slice ++;
                    }
                }
            }
            break;
        default:
            PRINT_ERR("unknown data type in map definition!\n");
            break;
    }

    return 0;
}

int CfgLoadDefValue(CFG_MAP *mapArray)
{
    CFG_MAP *map = NULL;
    int i =0;
    while (mapArray[i].stringName != NULL) {
        map = &mapArray[i];
        CfgLoadDefValueItem(map);
        i++;
    }

    return 0;
}

static int CfgDataToCjsonByMapItem(CFG_MAP *map, cJSON *root)
{
    int tmp;
	double temp;

    switch (map->dataType) {
        case CFG_DATA_TYPE_U32:
            tmp = *((unsigned int *)(map->dataAddress));
            if (tmp > map->max || tmp < map->min)
                tmp = strtoul(map->defaultValue, NULL, 0);
            cJSON_AddNumberToObject(root, map->stringName, tmp);
            break;
        case CFG_DATA_TYPE_U16:
            tmp = *((unsigned short *)(map->dataAddress));
            if (tmp > map->max || tmp < map->min)
                tmp = strtoul(map->defaultValue, NULL, 0);
            cJSON_AddNumberToObject(root, map->stringName, tmp);
            break;
        case CFG_DATA_TYPE_U8:
            tmp = *((unsigned char *)(map->dataAddress));
            if (tmp > map->max || tmp < map->min)
                tmp = strtoul(map->defaultValue, NULL, 0);
            cJSON_AddNumberToObject(root, map->stringName, tmp);
            break;
        case CFG_DATA_TYPE_S32:
            tmp = *((signed int *)(map->dataAddress));
            if (tmp > map->max || tmp < map->min)
                tmp = strtol(map->defaultValue, NULL, 0);
            cJSON_AddNumberToObject(root, map->stringName, tmp);
            break;
        case CFG_DATA_TYPE_S16:
            tmp = *((signed short *)(map->dataAddress));
            if (tmp > map->max || tmp < map->min)
                tmp = strtol(map->defaultValue, NULL, 0);
            cJSON_AddNumberToObject(root, map->stringName, tmp);
            break;
        case CFG_DATA_TYPE_S8:
            tmp = *((signed char *)(map->dataAddress));
            if (tmp > map->max || tmp < map->min)
                tmp = strtol(map->defaultValue, NULL, 0);
            cJSON_AddNumberToObject(root, map->stringName, tmp);
            break;
		case CFG_DATA_TYPE_FLOAT:
            temp = *(( float *)(map->dataAddress));
            if (temp > map->max || temp < map->min)
                temp = strtod(map->defaultValue, NULL);
            cJSON_AddNumberToObject(root, map->stringName, temp);
            break;

        case CFG_DATA_TYPE_STRING:
            cJSON_AddStringToObject(root, map->stringName, (char *)map->dataAddress);//
            break;

        case CFG_DATA_TYPE_STIME:
            {
                CFG_SCHEDTIME *stime = (CFG_SCHEDTIME *)map->dataAddress;

                int i, j;
                char tmp[20] = {0};
                cJSON *array = cJSON_CreateArray();
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 4; j ++) {
                        sprintf(tmp, "%d:%d-%d:%d", stime->startHour, \
                                                    stime->startMin, \
                                                    stime->stopHour, \
                                                    stime->stopMin);
                        cJSON_AddItemToArray(array, cJSON_CreateString(tmp));
                        stime ++;
                    }
                }
                cJSON_AddItemToObject(root, map->stringName, array);
            }
            break;

        case CFG_DATA_TYPE_SLICE:
            {
                unsigned int *slice = (unsigned int *)map->dataAddress;

                int i, j;
                char tmp[20] = {0};
                cJSON *array = cJSON_CreateArray();
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 3; j ++) {
                        sprintf(tmp, "%u", *slice);
                        cJSON_AddItemToArray(array, cJSON_CreateString(tmp));
                        slice ++;
                    }
                }
                cJSON_AddItemToObject(root, map->stringName, array);
            }
            break;
        default:
            PRINT_ERR("unknown data type in map definition!\n");
            break;
    }

    return 0;
}

cJSON *CfgDataToCjsonByMap(CFG_MAP *mapArray)
{
    cJSON *root;

    root = cJSON_CreateObject();
    int i =0;

    while(mapArray[i].stringName != NULL) {
        CfgDataToCjsonByMapItem(&mapArray[i], root);
        i ++;
    }

    return root;
}

static int CfgCjsonToDataByMapItem(CFG_MAP *map, cJSON *json)
{
    int tmp;
	double temp;

    switch (map->dataType) {
        case CFG_DATA_TYPE_U32:
            tmp = json->valueint;
            if (tmp > map->max || tmp < map->min)
                tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned int *)(map->dataAddress)) = (unsigned int)tmp;
            break;
        case CFG_DATA_TYPE_U16:
            tmp = json->valueint;
            if (tmp > map->max || tmp < map->min)
                tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned short *)(map->dataAddress)) = (unsigned short)tmp;
            break;
        case CFG_DATA_TYPE_U8:
            tmp = json->valueint;
            if (tmp > map->max || tmp < map->min)
                tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned char *)(map->dataAddress)) = (unsigned char)tmp;
            break;
        case CFG_DATA_TYPE_S32:
            tmp = json->valueint;
            if (tmp > map->max || tmp < map->min)
                tmp = strtol(map->defaultValue, NULL, 0);
            *((int *)(map->dataAddress)) = tmp;
            break;
        case CFG_DATA_TYPE_S16:
            tmp = json->valueint;
            if (tmp > map->max || tmp < map->min)
                tmp = strtol(map->defaultValue, NULL, 0);
            *((signed short *)(map->dataAddress)) = (signed short)tmp;
            break;
        case CFG_DATA_TYPE_S8:
            tmp = json->valueint;
            if (tmp > map->max || tmp < map->min)
                tmp = strtol(map->defaultValue, NULL, 0);
            *((signed char *)(map->dataAddress)) = (signed char)tmp;
            break;
		case CFG_DATA_TYPE_FLOAT:
            temp = json->valuedouble;
            if (temp > map->max || temp < map->min)
                temp = strtod(map->defaultValue, NULL);
            *(( float *)(map->dataAddress)) = ( float)temp;
            break;

        case CFG_DATA_TYPE_STRING:
			if (map->dataAddress && (map->max > 1)) {
				tmp = (int)map->max - 1;
                strncpy((char *)map->dataAddress, json->valuestring, tmp);
                ((char *)map->dataAddress)[tmp] = '\0';
            } else {
                PRINT_ERR("if type is string, addr can't be null and the upper limit must greater than 1.");
            }
            break;

        case CFG_DATA_TYPE_STIME:
            {
                CFG_SCHEDTIME *stime = (CFG_SCHEDTIME *)map->dataAddress;
                cJSON *tmp = NULL;
                int i, j;
                int startHour, startMin, stopHour, stopMin;
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 4; j ++) {
                        tmp = cJSON_GetArrayItem(json, i * 4 + j);
                        if (tmp) {
                            //printf("[%d][%d] %s\n", i, j, tmp->valuestring);
                            sscanf(tmp->valuestring, "%d:%d-%d:%d", &startHour, &startMin, &stopHour, &stopMin);
                            stime->startHour = startHour;
                            stime->startMin = startMin;
                            stime->stopHour = stopHour;
                            stime->stopMin = stopMin;
                            stime ++;
                        }

                    }
                }
            }
            break;

        case CFG_DATA_TYPE_SLICE:
            {
                unsigned int *slice = (unsigned int *)map->dataAddress;
                cJSON *tmp = NULL;
                int i, j;
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 3; j ++) {
                        tmp = cJSON_GetArrayItem(json, i * 3 + j);
                        if (tmp) {
                            *slice = strtoul(tmp->valuestring, NULL, 0);
                            slice ++;
                        }
                    }
                }
            }
            break;
        default:
            PRINT_ERR("unknown data type in map definition!\n");
            break;
    }

    return 0;
}


int CfgCjsonToDataByMap(CFG_MAP *mapArray, cJSON *json)
{
    int i =0;
    cJSON *tmp = NULL;

    while (mapArray[i].stringName != NULL) {
        tmp = cJSON_GetObjectItem(json, mapArray[i].stringName);
        if (tmp) {
            CfgCjsonToDataByMapItem(&mapArray[i], tmp);
        } else {
            PRINT_INFO("[%s] cjson get failed.\n", mapArray[i].stringName);
            CfgLoadDefValueItem(&mapArray[i]);
        }
        i++;
    }

    return 0;
}

static void CfgPrintItem(CFG_MAP *map)
{
    switch (map->dataType) {
        case CFG_DATA_TYPE_U32:
            printf("%s : %u \n", map->stringName, *((unsigned int *)(map->dataAddress)));
            break;
        case CFG_DATA_TYPE_U16:
            printf("%s : %u \n", map->stringName, *((unsigned short *)(map->dataAddress)));
            break;
        case CFG_DATA_TYPE_U8:
            printf("%s : %u \n", map->stringName, *((unsigned char *)(map->dataAddress)));
            break;
        case CFG_DATA_TYPE_S32:
            printf("%s : %d \n", map->stringName, *((signed int *)(map->dataAddress)));
            break;
        case CFG_DATA_TYPE_S16:
            printf("%s : %d \n", map->stringName, *((signed short *)(map->dataAddress)));
            break;
        case CFG_DATA_TYPE_S8:
            printf("%s : %d \n", map->stringName, *((signed char *)(map->dataAddress)));
            break;
		case CFG_DATA_TYPE_FLOAT:
			printf("%s : %f \n", map->stringName, *(( float *)(map->dataAddress)));
            break;

        case CFG_DATA_TYPE_STRING:
            printf("%s : %s \n", map->stringName, (char *)(map->dataAddress));
            break;

        case CFG_DATA_TYPE_STIME:
            {
                printf("%s : [\n", map->stringName);
                CFG_SCHEDTIME *stime = (CFG_SCHEDTIME *)map->dataAddress;

                int i, j;
                for (i = 0; i < 7; i ++) {
                    printf("    date %d: ", i);
                    for (j = 0; j < 4; j ++) {
                        printf("%d:%d - %d:%d, ", stime->startHour, \
                                                  stime->startMin, \
                                                  stime->stopHour, \
                                                  stime->stopMin);
                        stime ++;
                    }
                    printf("\n");
                }
                printf("]\n");
            }
            break;

        case CFG_DATA_TYPE_SLICE:
            {
                printf("%s : [\n", map->stringName);
                unsigned int *slice = (unsigned int *)map->dataAddress;

                int i, j;
                for (i = 0; i < 7; i ++) {
                    printf("    date %d: ", i);
                    for (j = 0; j < 3; j ++) {
                        printf("%u, ", (*slice));
                        slice ++;
                    }
                    printf("\n");
                }
                printf("]\n");
            }
            break;
        default:
            PRINT_ERR("unknown data type in map definition!\n");
            break;
    }
}

int CfgPrintMap(CFG_MAP *mapArray)
{
    int i =0;
    CFG_MAP *map = NULL;
    while (mapArray[i].stringName != NULL) {
        map = &mapArray[i];
        CfgPrintItem(map);
        i++;
    }

    return 0;
}

int CfgParseCjson(cJSON *root, const char *str, CFG_MAP *mapArray)
{
    cJSON *item = NULL;
    item = cJSON_GetObjectItem(root, str);
    if (!item) {
        //PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
    CfgCjsonToDataByMap(mapArray, item);
    return 0;
}

int CfgLoad(const char *filename, const char *str, CFG_MAP *mapArray)
{
    char *data = NULL;
    data = CfgReadFromFile(filename);
    if (data == NULL) {
        //从配置文件读取失败，则使用默认参数
        PRINT_INFO("load %s error, so to load default cfg param.\n", filename);
        goto err2;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        //从配置文件解析cjson失败，则使用默认参数
        PRINT_INFO("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err2;
    }

    CfgParseCjson(json, str, mapArray);

    cJSON_Delete(json);
    free(data);
    return 0;

err2:
    CfgLoadDefValue(mapArray);
    CfgSave(filename,str, mapArray);
    return 0;
}


int CfgAddCjson(cJSON *root, const char *str, CFG_MAP *mapArray)
{
    cJSON *item = NULL;
    item = CfgDataToCjsonByMap(mapArray);
    cJSON_AddItemToObject(root, str, item);

    return 0;
}

int CfgSave(const char *filename, const char *str, CFG_MAP *mapArray)
{
    int ret = 0;
    char *out;
    cJSON *root;

    root = cJSON_CreateObject();//创建项目

    CfgAddCjson(root, str, mapArray);

    out = cJSON_Print(root);
    ret = CfgWriteToFile(filename, out);
    if (ret != 0) {
        PRINT_ERR("CfgSave %s error.", filename);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

int CfgGetDefByName(CFG_MAP *mapArray, const char *item_name, void *value)
{
    if ((value == NULL) || (item_name == NULL) || (mapArray == NULL)) {
        PRINT_ERR("param error!\n");
        return -1;
    }

    CFG_MAP *map = NULL;
    int i =0;
    while (mapArray[i].stringName != NULL) {
        map = &mapArray[i];
        if (strcmp(map->stringName, item_name) == 0)
            break;
        i++;
    }
    if (mapArray[i].stringName == NULL)
        return -1;

    int tmp;
	double temp;
    switch (map->dataType) {
        case CFG_DATA_TYPE_U32:
            tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned int *)value) = (unsigned int)tmp;
            break;
        case CFG_DATA_TYPE_U16:
            tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned short *)value) = (unsigned short)tmp;
            break;
        case CFG_DATA_TYPE_U8:
            tmp = strtoul(map->defaultValue, NULL, 0);
            *((unsigned char *)value) = (unsigned char)tmp;
            break;
        case CFG_DATA_TYPE_S32:
            tmp = strtol(map->defaultValue, NULL, 0);
            *((int *)value) = tmp;
            break;
        case CFG_DATA_TYPE_S16:
            tmp = strtol(map->defaultValue, NULL, 0);
            *((signed short *)value) = (signed short)tmp;
            break;
        case CFG_DATA_TYPE_S8:
            tmp = strtol(map->defaultValue, NULL, 0);
            *((signed char *)value) = (signed char)tmp;
            break;

		case CFG_DATA_TYPE_FLOAT:
            temp = strtod(map->defaultValue, NULL);
            *(( float *)value) = ( float)temp;
            break;

        case CFG_DATA_TYPE_STRING:
			if (map->max > 1) {
				tmp = (int)map->max - 1;
                strncpy((char *)value, map->defaultValue, tmp);
                ((char *)value)[tmp] = '\0';
            } else {
                PRINT_ERR("if type is string, addr can't be null and the upper limit must greater than 1.");
            }
            break;
        case CFG_DATA_TYPE_STIME:
            {
                CFG_SCHEDTIME *stime = (CFG_SCHEDTIME *)value;
                int i, j;
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 4; j ++) {
                        stime->startHour = 0;
                        stime->startMin = 0;
                        stime->stopHour = 23;
                        stime->stopMin = 59;
                        stime ++;
                    }
                }
            }
            break;
        case CFG_DATA_TYPE_SLICE:
            {
                unsigned int *slice = (unsigned int *)value;
                int i, j;
                for (i = 0; i < 7; i ++) {
                    for (j = 0; j < 3; j ++) {
                        *slice = strtoul("4294967295", NULL, 0);
                        slice ++;
                    }
                }
            }
            break;
        default:
            PRINT_ERR("unknown data type in map definition!\n");
            break;
    }

    return 0;
}

int is_in_schedule_slice(AR_U32 *slice)
{
    long ts = time(NULL);
    struct tm tt = {0};
    struct tm *pTm = localtime_r(&ts, &tt);

    unsigned int tmp;
    if (pTm->tm_hour < 8) {
        //tmp = g_stRunMdCfg.scheduleSlice[pTm->tm_wday][0];
        tmp = *(slice + pTm->tm_wday * 3 + 0);
        pTm->tm_hour -= 0;
    } else if (pTm->tm_hour > 15) {
        //tmp = g_stRunMdCfg.scheduleSlice[pTm->tm_wday][2];
        tmp = *(slice + pTm->tm_wday * 3 + 2);
        pTm->tm_hour -= 16;
    } else {
        //tmp = g_stRunMdCfg.scheduleSlice[pTm->tm_wday][1];
        tmp = *(slice + pTm->tm_wday * 3 + 1);
        pTm->tm_hour -= 8;
    }
    //PRINT_INFO("slice:%u\n", tmp);

    int mask1 = -1;
    if (pTm->tm_min < 15)
        mask1 = 0;
    if ((pTm->tm_min >= 15) && (pTm->tm_min < 30))
        mask1 = 1;
    if ((pTm->tm_min >= 30) && (pTm->tm_min < 45))
        mask1 = 2;
    if ((pTm->tm_min >= 45) && (pTm->tm_min < 60))
        mask1 = 3;

    int mask2 = pTm->tm_hour * 4 + mask1;
    //PRINT_INFO("mask:%d\n", mask2);

    int ret = (((1 << mask2) & tmp) == 0) ? 0 : 1;
    //PRINT_INFO("ret:%d\n", ret);

    return ret;
}

int timepoint_to_u64(CFG_SCHEDTIME *pSchedTime, unsigned long long *start_64, unsigned long long *stop_64)
{
    char str[30];
    memset(str, 0, sizeof(str));
    sprintf(str, "%02d%02d", pSchedTime->startHour, pSchedTime->startMin);
    *start_64 = atoll(str);

    memset(str, 0, sizeof(str));
    sprintf(str, "%02d%02d", pSchedTime->stopHour, pSchedTime->stopMin);
    *stop_64 = atoll(str);

    return 0;
}


int is_in_schedule_timepoint(CFG_SCHEDTIME *time_point)
{
    long ts = time(NULL);
    struct tm tt = {0};
    struct tm *pTm = localtime_r(&ts, &tt);

    char str[30];
    memset(str, 0, sizeof(str));
    sprintf(str, "%02d%02d", pTm->tm_hour, pTm->tm_min);
    unsigned long long now_64 = atoll(str);

    int i;
    int ret = 0;
    unsigned long long start_64 = 0;
    unsigned long long stop_64 = 0;
    CFG_SCHEDTIME *tPoint = NULL;
    for (i = 0; i < 4; i ++) {
        tPoint = (time_point + pTm->tm_wday * 4 + i);
        timepoint_to_u64(tPoint, &start_64, &stop_64);
        if ((now_64 >= start_64) && (now_64 <= stop_64)) {
            ret = 1;
            break;
        }
    }

    return ret;
}

