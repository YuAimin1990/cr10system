#include "cfg_user.h"
#include <unistd.h>
/*******This is not checked yet, 2021-06-07 ********/

IPC_CFG_USER_S g_stRunUserCfg;
IPC_CFG_BLACK_USER_S g_stRunBlackUserCfg;

static CFG_MAP adminMap[] = {
    {"enable",    &(g_stRunUserCfg.user[0].enable),    CFG_DATA_TYPE_S32,    "1",     "rw", 0, 1,              NULL},
    {"userName",  &(g_stRunUserCfg.user[0].userName),  CFG_DATA_TYPE_STRING, "admin", "rw", 1, MAX_STR_LEN_32, NULL},
    {"password",  &(g_stRunUserCfg.user[0].password),  CFG_DATA_TYPE_STRING, "admin", "rw", 1, MAX_STR_LEN_32, NULL},
    {"userRight", &(g_stRunUserCfg.user[0].userRight), CFG_DATA_TYPE_U32,    "2", "rw", 0,  2,          NULL},
    {"sid",       &(g_stRunUserCfg.user[0].sid),       CFG_DATA_TYPE_S32,    "-1", "rw", -1,  10000,          NULL},
    {NULL,},
};

void IPC_CFG_USER_Print()
{
    printf("*************** User **************\n");

    //CfgPrintMap(adminMap);

    AR_S32 i;
    for (i = 0; i < MAX_USER_NUM; i ++) {
        if (g_stRunUserCfg.user[i].enable) {
            printf("user%d -- name: %s, password:%s, right:%u, sid:%d \n", i,
                g_stRunUserCfg.user[i].userName,
                g_stRunUserCfg.user[i].password,
                g_stRunUserCfg.user[i].userRight,
                g_stRunUserCfg.user[i].sid);
        }
    }

    printf("*************** User **************\n\n");
}


AR_S32 IPC_CFG_USER_Save()
{
    cJSON *root;
    char *out;

    root = cJSON_CreateObject();
    AR_S32 i = 0;
    for (i = 0; i < MAX_USER_NUM; i ++) {
        if (g_stRunUserCfg.user[i].enable == 1) {
            char cjson_name[10] = {0};
            sprintf(cjson_name, "user_%d", i);
            adminMap[0].dataAddress = &(g_stRunUserCfg.user[i].enable);
            adminMap[1].dataAddress = &(g_stRunUserCfg.user[i].userName);
            adminMap[2].dataAddress = &(g_stRunUserCfg.user[i].password);
            adminMap[3].dataAddress = &(g_stRunUserCfg.user[i].userRight);
            adminMap[4].dataAddress = &(g_stRunUserCfg.user[i].sid);
            CfgAddCjson(root, cjson_name, adminMap);
        }
    }

    adminMap[0].dataAddress = &(g_stRunUserCfg.user[0].enable);
    adminMap[1].dataAddress = &(g_stRunUserCfg.user[0].userName);
    adminMap[2].dataAddress = &(g_stRunUserCfg.user[0].password);
    adminMap[3].dataAddress = &(g_stRunUserCfg.user[0].userRight);
    adminMap[4].dataAddress = &(g_stRunUserCfg.user[0].sid);

    out = cJSON_Print(root);

    AR_S32 ret = CfgWriteToFile(USER_CFG_FILE, out);
    if (ret != 0) {
        PRINT_ERR("CfgWriteToFile %s error.", USER_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

AR_S32 IPC_CFG_USER_LoadDefault()
{
    AR_S32 i;
    for (i = 0; i < MAX_USER_NUM; i ++) {
        g_stRunUserCfg.user[i].enable = 0;
        memset(g_stRunUserCfg.user[i].userName, 0, sizeof(g_stRunUserCfg.user[i].userName));
        memset(g_stRunUserCfg.user[i].password, 0, sizeof(g_stRunUserCfg.user[i].password));
        g_stRunUserCfg.user[i].userRight = 0;
        g_stRunUserCfg.user[i].sid = -1;
    }

    adminMap[0].dataAddress = &(g_stRunUserCfg.user[0].enable);
    adminMap[1].dataAddress = &(g_stRunUserCfg.user[0].userName);
    adminMap[2].dataAddress = &(g_stRunUserCfg.user[0].password);
    adminMap[3].dataAddress = &(g_stRunUserCfg.user[0].userRight);
    adminMap[4].dataAddress = &(g_stRunUserCfg.user[0].sid);
    CfgLoadDefValue(adminMap);
    return 0;
}


AR_S32 IPC_CFG_USER_Load(CFG_RUN_MODE_E enMode)
{
    char *data = NULL;
    /* add by qqw */
    #if 0
    UserCfgLoadJson();
    #endif
    data = CfgReadFromFile(USER_CFG_FILE);
    if (data == NULL) {
        PRINT_INFO("load %s error, so to load default cfg param.\n", USER_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json){
        PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    AR_S32 i = 0;
    for (i = 0; i < MAX_USER_NUM; i ++) {
            char cjson_name[10] = {0};
            sprintf(cjson_name, "user_%d", i);
            adminMap[0].dataAddress = &(g_stRunUserCfg.user[i].enable);
            adminMap[1].dataAddress = &(g_stRunUserCfg.user[i].userName);
            adminMap[2].dataAddress = &(g_stRunUserCfg.user[i].password);
            adminMap[3].dataAddress = &(g_stRunUserCfg.user[i].userRight);
            adminMap[4].dataAddress = &(g_stRunUserCfg.user[i].sid);

            CfgParseCjson(json, cjson_name, adminMap);
    }

    adminMap[0].dataAddress = &(g_stRunUserCfg.user[0].enable);
    adminMap[1].dataAddress = &(g_stRunUserCfg.user[0].userName);
    adminMap[2].dataAddress = &(g_stRunUserCfg.user[0].password);
    adminMap[3].dataAddress = &(g_stRunUserCfg.user[0].userRight);
    adminMap[4].dataAddress = &(g_stRunUserCfg.user[0].sid);

    cJSON_Delete(json);
    free(data);
    return 0;

err:
    IPC_CFG_USER_LoadDefault();
    IPC_CFG_USER_Save();
    return 0;
}

AR_S32 UserIsExist(const char *userName)
{
    AR_S32 i;
    for (i = 0; i < MAX_USER_NUM; i ++) {
        if (g_stRunUserCfg.user[i].enable == 1) {
            if (0 == strcmp(g_stRunUserCfg.user[i].userName, userName))
                return 1;
        }
    }

    return 0;
}

AR_S32 UserAdd(NET_USER_INFO *pUser)
{
    AR_S32 is_exist = UserIsExist(pUser->userName);
    if (is_exist == 1)
        return 1;

    AR_S32 i;
    for (i = 0; i < MAX_USER_NUM; i ++) {
        if (g_stRunUserCfg.user[i].enable == 0) {
            strcpy(g_stRunUserCfg.user[i].userName, pUser->userName);
            strcpy(g_stRunUserCfg.user[i].password, pUser->password);
            g_stRunUserCfg.user[i].userRight = pUser->userRight;
            g_stRunUserCfg.user[i].sid = pUser->sid;
            g_stRunUserCfg.user[i].enable = 1;
            return 0;
        }
    }
    return -1;
}

AR_S32 UserClean()
{
    AR_S32 i;
    for (i = 0; i < MAX_USER_NUM; i ++) {
        g_stRunUserCfg.user[i].enable = 0;
        memset(g_stRunUserCfg.user[i].userName, 0, sizeof(g_stRunUserCfg.user[i].userName));
        memset(g_stRunUserCfg.user[i].password, 0, sizeof(g_stRunUserCfg.user[i].password));
        g_stRunUserCfg.user[i].userRight = 0;
        g_stRunUserCfg.user[i].sid = -1;
    }
    return -1;
}

IPC_CFG_USER_S * IPC_CFG_USER_GetParam()
{
    return &g_stRunUserCfg;
}

static pthread_mutex_t g_black_user_mutex = PTHREAD_MUTEX_INITIALIZER;

AR_S32 IPC_CFG_BLACK_USER_Save()
{
    FILE *fp;
    AR_S32 count, ret;

    char path[128];
    memset(path, 0 ,sizeof(path));
    sprintf(path, "%s", BLACK_USER_CFG_FILE);

    pthread_mutex_lock(&g_black_user_mutex);

    fp = fopen(path, "wb+");
    if(NULL == fp)
    {
        PRINT_ERR();
        goto err1;
    }

    count = g_stRunBlackUserCfg.num;
    fseek(fp, 0, SEEK_SET);
    ret = fwrite(&count, sizeof(AR_S32), 1, fp);
    if(ret != 1)
    {
        PRINT_ERR();
        goto err2;
    }

    BLACK_USER_NODE *p = g_stRunBlackUserCfg.h;
    BLACK_USER_NODE *pr = NULL;

    AR_S32 write_err = 0;
    while (p != NULL) {
        pr = p;
        p = p->pNext;

        ret = fwrite(pr->user.name, sizeof(BLACK_USER), 1, fp);
        if (ret != 1) {
            PRINT_ERR();
            write_err = 1;
            break;
        }
        fflush(fp);
    }

    if (write_err == 1) {
        goto err2;
    }

    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&g_black_user_mutex);
    return 0;

err2:
    fclose(fp);
err1:
    pthread_mutex_unlock(&g_black_user_mutex);
    return -1;
}


/* 添加成功返回0， 已经存在返回1，添加失败返回-1 */
AR_S32 BlackUserAdd(BLACK_USER *pUser)
{
    if (!pUser) {
        PRINT_ERR("param error\n");
        return -1;
    }

    BLACK_USER_NODE *q = (BLACK_USER_NODE *)malloc(sizeof(BLACK_USER_NODE));
    if (q == NULL) {
        PRINT_ERR("malloc error\n");
        return -1;
    }
    memset(q->user.name, 0, sizeof(q->user.name));
    strncpy(q->user.name, pUser->name, sizeof(q->user.name));
    q->pNext = NULL;

    if (g_stRunBlackUserCfg.h == NULL) {
        g_stRunBlackUserCfg.h = q;
        g_stRunBlackUserCfg.num ++;
        return 0;
    } else {
        AR_S32 is_exist = 0;
        BLACK_USER_NODE *p = g_stRunBlackUserCfg.h;
        BLACK_USER_NODE *pr = NULL;
        while (p != NULL ) {
            if (strcmp(q->user.name, p->user.name) == 0) {
                is_exist = 1;
                break;
            }
            pr = p;
            p = p->pNext;
        }

        if (is_exist == 1) {
            PRINT_INFO("black user:%s exist\n", q->user.name);
            free(q);
            return 1; //已经存在
        } else {
            pr->pNext = q;
            g_stRunBlackUserCfg.num ++;
            return 0;
        }
    }
}

/* 删除成功返回0， 不存在返回1，删除失败返回-1 */
AR_S32 BlackUserDel(BLACK_USER *pUser)
{
    if (!pUser) {
        PRINT_ERR("param error\n");
        return -1;
    }

    if (g_stRunBlackUserCfg.h == NULL) {
        PRINT_ERR("black user head is null\n");
        return -1;
    }

    BLACK_USER_NODE *p = g_stRunBlackUserCfg.h;
    BLACK_USER_NODE *pr = NULL;

    if (strcmp(pUser->name, p->user.name) == 0) {
        pr = p->pNext;
        free(p);
        g_stRunBlackUserCfg.num --;
        g_stRunBlackUserCfg.h = pr;
        return 0;
    }

    AR_S32 del_ok = 0;
    while (p->pNext != NULL ) {
        if (strcmp(pUser->name, p->pNext->user.name) == 0) {
            pr = p->pNext;
            p->pNext = pr->pNext;

            pr->pNext = NULL;
            free(pr);
            g_stRunBlackUserCfg.num --;
            del_ok = 1;
            break;
        } else {
            p = p->pNext;
        }
    }

    if (del_ok) {
        return 0;
    } else {
        return 1; //不存在
    }
}

/* 如果在黑名单中返回0，如果不在返回1，错误返回-1 */
AR_S32 BlackUserCheck(BLACK_USER *pUser)
{
    if (!pUser) {
        PRINT_ERR("param error\n");
        return -1;
    }

    AR_S32 is_exist = 0;
    BLACK_USER_NODE *p = g_stRunBlackUserCfg.h;
    while (p != NULL ) {
        if (strcmp(pUser->name, p->user.name) == 0) {
            is_exist = 1;
            break;
        }
        p = p->pNext;
    }

    if (is_exist) {
        return 0;
    } else {
        return 1;
    }
}
void IPC_CFG_BLACK_USER_Print()
{
    printf("black user, num = %d\n", g_stRunBlackUserCfg.num);

    BLACK_USER_NODE *p = g_stRunBlackUserCfg.h;
    AR_S32 index = 0;
    while (p != NULL ) {
        printf("user%d, name:%s\n", index, p->user.name);
        index ++;
        p = p->pNext;
    }
}

AR_S32 IPC_CFG_BLACK_USER_Open(CFG_RUN_MODE_E enMode)
{
    g_stRunBlackUserCfg.num = 0;
    g_stRunBlackUserCfg.h = NULL;

    FILE *fp;
    AR_S32 count, ret;

    char path[128];
    memset(path, 0, sizeof(path));
    sprintf(path, "%s", BLACK_USER_CFG_FILE);

    if(access(path, F_OK) != 0 ) {
        PRINT_INFO("%s not exist\n", path);
        return -1;
    }

    pthread_mutex_lock(&g_black_user_mutex);
    fp = fopen(path, "rb");
    if(fp == NULL)
    {
        PRINT_ERR();
        goto err4;
    }
    fseek(fp, 0, SEEK_SET);
    ret = fread(&count, sizeof(AR_S32), 1, fp);
    if ( (ret != 1) || (count <= 0) )
    {
        PRINT_ERR();
        goto err3;
    }
    g_stRunBlackUserCfg.num = count;

    BLACK_USER *pUser = (BLACK_USER *)malloc(sizeof(BLACK_USER) * count);
    if(pUser == NULL)
    {
        PRINT_ERR();
        goto err2;
    }

    ret = fread(pUser, sizeof(BLACK_USER) * count, 1, fp);
    if (ret != 1)
    {
        PRINT_ERR();
        goto err1;
    }

    fclose(fp);

    BLACK_USER *p = pUser;
    while (count --) {
        BlackUserAdd(p);
        p ++;
    }

    free(pUser);

    pthread_mutex_unlock(&g_black_user_mutex);
    return 0;

err1:
    free(pUser);
err2:
    g_stRunBlackUserCfg.num = 0;
err3:
    fclose(fp);
err4:
    pthread_mutex_unlock(&g_black_user_mutex);
    return -1;
}

void IPC_CFG_BLACK_USER_Close()
{
    pthread_mutex_lock(&g_black_user_mutex);
    BLACK_USER_NODE *p = g_stRunBlackUserCfg.h;
    BLACK_USER_NODE *pr = NULL;

    while (p != NULL) {
        pr = p;
        p = p->pNext;
        pr->pNext = NULL;
        free(pr);
    }

    g_stRunBlackUserCfg.num = 0;
    g_stRunBlackUserCfg.h = NULL;
    pthread_mutex_unlock(&g_black_user_mutex);
}

