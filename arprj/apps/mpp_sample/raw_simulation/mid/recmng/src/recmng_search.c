#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

#include "recmng_search.h"

#if 0 // no search support at this point

#define _SINGLE_DEBUG_ 0
#define _DEBUG_MSG_ON_ 0
#define CAL_SERACH_TIME 0

static pthread_mutex_t g_manager_mutex = PTHREAD_MUTEX_INITIALIZER;

AR_U32 get_file_size(char *path)
{
    FILE *fp;
    AR_U32 tmp32;
    if(path == NULL)
    {
        PRINT_ERR();
        return -1;
    }

    fp = fopen(path, "rb");
    if(fp == NULL)
    {
        PRINT_ERR("fopen %s error\n", path);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    tmp32 = ftell(fp);
    fclose(fp);

    return tmp32;
}

AR_U64 get_file_size64(char *path)
{
    FILE *fp;
    AR_U64 tmp64;
    if(path == NULL)
    {
        PRINT_ERR();
        return -1;
    }

    fp = fopen(path, "rb");
    if(fp == NULL)
    {
        PRINT_ERR();
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    tmp64 = ftell(fp);
    fclose(fp);

    return tmp64;
}

static FILE_LIST *creat_empty_list(AR_U32 type, AR_U32 ch_num, AR_U64 start, AR_U64 stop)
{

    FILE_NODE *h = (FILE_NODE *)malloc(sizeof(FILE_NODE));
    if(h == NULL)
    {
        PRINT_ERR();
        return NULL;
    }
    h->next = NULL;

    FILE_LIST *list = (FILE_LIST *)malloc(sizeof(FILE_LIST));
    if(list == NULL)
    {
        PRINT_ERR();
        return NULL;
    }
    list->type = type;
    list->ch_num = ch_num;
    list->start = start;
    list->stop = stop;
    list->head = h;
    list->len = 0;
    list->index = 0;

    return list;
}

static int insert_node(FILE_LIST *list, char *file_name, char *dir_path)
{
    int i;
    FILE_NODE *p, *q;
    char dir[128];
    p = NULL;
    q = NULL;

    char file_path[128];
    memset(file_path, 0, sizeof(file_path));
    sprintf(file_path, "%s/%s", dir_path, file_name);

    struct stat file_info = {0};
    unsigned int file_size = 0;
    if(stat(file_path, &file_info) == -1) // cannot stat
        PRINT_ERR("Fail to stat %s\n", file_path);
    else
        file_size = (unsigned int)file_info.st_size;

    memset(dir, 0 ,sizeof(dir));
    i = AR_AVI_FindLastIndex(dir_path, '/');
    strcpy(dir, dir_path + i + 1);
    memset(file_path, 0, sizeof(file_path));
    sprintf(file_path, "%s/%s", dir, file_name);

    p = (FILE_NODE *)malloc(sizeof(FILE_NODE));
    if(p == NULL)
    {
        PRINT_ERR();
        return -1;
    }
    strcpy(p->path, file_path);
    p->start = AR_AVI_GetStartTimeFromName(dir, file_name);
    p->stop = AR_AVI_GetStopTimeFromName(dir, file_name);
    p->size = file_size;
    p->next = NULL;

#if _DEBUG_MSG_ON_
    PRINT_INFO("insert node to list: file_path = %s, start = %llu, stop = %llu\n",
            p->path, p->start, p->stop);
#endif

    q = list->head;

    while(q->next != NULL)
    {
        if(q->next->start < p->start)
            q = q->next;
        else if(q->next->start == p->start) //避免重复插入
        {
            free(p);
            return 0;
        }
        else
            break;
    }


    p->next = q->next;
    q->next = p;

    list->len ++;

    return 0;
}


static int insert_dir_node(FILE_LIST *list, char *file_name, char *path)
{
    FILE_NODE *p, *q;
    p = NULL;
    q = NULL;

    char file_path[128] = {0};
    if (path != NULL)
    {
        sprintf(file_path, "%s/%s", path, file_name);
    }

    char time_str[20] = {0};
    sprintf(time_str, "%s000000", file_name);
    AR_U64 tmp64 = (unsigned long long)atoll(time_str);

    p = (FILE_NODE *)malloc(sizeof(FILE_NODE));
    if(p == NULL)
    {
        PRINT_ERR();
        return -1;
    }

    if (path != NULL)
    {
        strcpy(p->path, file_path);
    }
    else
    {
        strcpy(p->path, file_name);
    }
    p->start = tmp64;
    p->stop = tmp64;
    p->next = NULL;

#if _DEBUG_MSG_ON_
    PRINT_INFO("insert node to list: file_path = %s, start = %llu, stop = %llu\n",
            p->path, p->start, p->stop);
#endif

    q = list->head;

    while(q->next != NULL)
    {
        if(q->next->start < p->start)
            q = q->next;
        else if(q->next->start == p->start) //避免重复插入
        {
            free(p);
            return 0;
        }
        else
            break;
    }


    p->next = q->next;
    q->next = p;

    list->len ++;

    return 0;
}



static int insert_snap_node(FILE_LIST *list, char *file_name, char *dir_path)
{
    int i;
    FILE_NODE *p, *q;
    char dir[128];
    p = NULL;
    q = NULL;

    char file_path[128];
    memset(file_path, 0, sizeof(file_path));
    sprintf(file_path, "%s/%s", dir_path, file_name);

    struct stat file_info = {0};
    unsigned int file_size = 0;
    if(stat(file_path, &file_info) == -1) // cannot stat
        PRINT_ERR("Fail to stat %s\n", file_path);
    else
        file_size = (unsigned int)file_info.st_size;

    memset(dir, 0 ,sizeof(dir));
    i = AR_AVI_FindLastIndex(dir_path, '/');
    strcpy(dir, dir_path + i + 1);
    memset(file_path, 0, sizeof(file_path));
    sprintf(file_path, "%s/%s", dir, file_name);

    p = (FILE_NODE *)malloc(sizeof(FILE_NODE));
    if(p == NULL)
    {
        PRINT_ERR();
        return -1;
    }
    strcpy(p->path, file_path);
    p->start = AR_AVI_GetStartTimeFromName(dir, file_name);
    p->stop = p->start;
    p->size = file_size;
    p->next = NULL;

#if _DEBUG_MSG_ON_
    PRINT_INFO("insert node to list: file_path = %s, start = %llu, stop = %llu\n",
            p->path, p->start, p->stop);
#endif

    q = list->head;

    while(q->next != NULL)
    {
        if(q->next->start < p->start)
            q = q->next;
        else if(q->next->start == p->start) //避免重复插入
        {
            free(p);
            return 0;
        }
        else
            break;
    }


    p->next = q->next;
    q->next = p;

    list->len ++;

    return 0;
}


/**********************************************************************
函数描述：判断是否查找到文件
入口参数：FILE_LIST *list: 查找文件的控制句柄
返回值：  1: 空，没有找到文件
          0: 非空，查找到了文件
**********************************************************************/
int is_empty_list(FILE_LIST *list)
{
    return NULL == list->head->next;
}

/**********************************************************************
函数描述：得到查找到的文件数目
入口参数：FILE_LIST *list: 查找文件的控制句柄
返回值：  int: 查找到的文件数目
**********************************************************************/
int get_len_list(FILE_LIST *list)
{
    if (list == NULL)
        return 0;

    int i = 0;
    FILE_NODE *p;
    p = list->head;
    while (p->next != NULL)
    {
        i++;
        p = p->next;
    }
    return i;
}

/**********************************************************************
函数描述：打印管理文件中的一次录像动作的信息
入口参数：INDEX_NODE *n: 录像动作的信息指针
返回值：  无
**********************************************************************/
void print_index_node(INDEX_NODE *n)
{
    PRINT_INFO("******** INDEX NODE *******\n");
    PRINT_INFO("ch_num = %u\n", n->ch_num);
    PRINT_INFO("start = %llu\n", n->start);
    PRINT_INFO("stop = %llu\n", n->stop);
    PRINT_INFO("******** INDEX NODE *******\n");
}

/**********************************************************************
函数描述：打印查找文件控制句柄中的单个文件信息
入口参数：FILE_NODE *n: 查找后得到的单个文件信息
返回值：  无
**********************************************************************/
void print_node(FILE_NODE *n)
{
    PRINT_INFO("******** NODE *******\n");
    PRINT_INFO("path = %s\n", n->path);
    PRINT_INFO("start = %llu\n", n->start);
    PRINT_INFO("stop = %llu\n", n->stop);
    PRINT_INFO("******** NODE *******\n");
}

/**********************************************************************
函数描述：打印查找文件控制句柄中的所有文件信息
入口参数：FILE_LIST *list: 查找文件控制句柄
返回值：  无
**********************************************************************/
void print_list(FILE_LIST *list)
{
    PRINT_INFO("\n-----------------------------------------------------------------\n");
    PRINT_INFO("print list:\n");
    PRINT_INFO("num = %u, type = %u\n", list->ch_num, list->type);
    PRINT_INFO("start = %llu, stop = %llu\n", list->start, list->stop);
    PRINT_INFO("len = %u, index = %u\n", list->len, list->index);

    FILE_NODE *q;
    q = list->head;
    while(q->next != NULL)
    {
        print_node(q->next);
        q = q->next;
    }

    PRINT_INFO("-----------------------------------------------------------------\n\n");
}

/**********************************************************************
函数描述：打印安普接口的 GK_NET_FINDDATA 信息
入口参数：GK_NET_FINDDATA *p: 查找后得到的单个文件信息
返回值：  无
**********************************************************************/
void print_finddata(GK_NET_FINDDATA *p)
{
    PRINT_INFO("\n-----------------------------------------\n");
    PRINT_INFO("print finddata:\n");
    PRINT_INFO("dwSize = %lu\n", p->dwSize);
    PRINT_INFO("csFileName = %s\n", p->csFileName);
    PRINT_INFO("dwFileType = %lu\n", p->dwFileType);

    PRINT_INFO("start = %04lu%02lu%02lu%02lu%02lu%02lu \n", p->stStartTime.dwYear,
                                                        p->stStartTime.dwMonth,
                                                        p->stStartTime.dwDay,
                                                        p->stStartTime.dwHour,
                                                        p->stStartTime.dwMinute,
                                                        p->stStartTime.dwSecond);
    PRINT_INFO("stop  = %04lu%02lu%02lu%02lu%02lu%02lu \n", p->stStopTime.dwYear,
                                                        p->stStopTime.dwMonth,
                                                        p->stStopTime.dwDay,
                                                        p->stStopTime.dwHour,
                                                        p->stStopTime.dwMinute,
                                                        p->stStopTime.dwSecond);
    PRINT_INFO("dwFileSize = %lu\n", p->dwFileSize);
    PRINT_INFO("-----------------------------------------\n");
}


static void delete_node(FILE_LIST *list, int index)
{
    if(is_empty_list(list))
    {
        PRINT_ERR();
        return;
    }

    FILE_NODE *p, *q;

    p = list->head;

    if(index >= get_len_list(list) || index < 0)
    {
        PRINT_ERR();
        return;
    }


    while(index--)
        p = p->next;

    q = p->next;
    p->next = q->next;

#if _DEBUG_MSG_ON_
    print_node(q);
    PRINT_INFO("len = %lu\n", list->len);
#endif

    q->next = NULL;
    free(q);

    list->len --;
}

/**********************************************************************
函数描述：查找文件结束
入口参数：FILE_LIST *: 查找文件的控制句柄
返回值：  无
**********************************************************************/
void search_close(FILE_LIST *list)
{
    if (list == NULL)
        return;

    while(list->len)
        delete_node(list, 0);

    list->head->next = NULL;
    free(list->head);

    list->head = NULL;
    free(list);
}

/**********************************************************************
函数描述：根据时间段查找文件
入口参数：FILE_LIST *: 查找文件的控制句柄
          FILE_NODE *node: 得到文件信息指针
返回值：  安普定义如下:
          GK_NET_FILE_SUCCESS: 获取文件信息成功
          GK_NET_NOMOREFILE: 没有更多的文件，查找结束
          GK_NET_FILE_NOFIND: 未查找到文件
**********************************************************************/
int get_file_node(FILE_LIST *list, FILE_NODE *node)
{
/*
#define GK_NET_FILE_SUCCESS 1000// 获取文件信息成功
#define GK_NET_FILE_NOFIND 1001// 未查找到文件
#define GK_NET_ISFINDING 1002// 正在查找请等待
#define GK_NET_NOMOREFILE  1003// 没有更多的文件，查找结束
#define GK_NET_FILE_EXCEPTION  1004// 查找文件时异常
*/
    if (list == NULL)
        return GK_NET_FILE_NOFIND;

    if(is_empty_list(list))
        return GK_NET_FILE_NOFIND;

    FILE_NODE *p;
    AR_U32 index;
    p = list->head;
    index = list->index;

    if(index  == list->len)
       return GK_NET_NOMOREFILE;

    while(index--)
        p = p->next;

    strcpy(node->path, p->next->path);
    node->start = p->next->start;
    node->stop = p->next->stop;
    node->size = p->next->size;

    list->index ++;
    return GK_NET_FILE_SUCCESS;
}


static int add_file_to_list(FILE_LIST *list, char *dir, MANAGER_INFO *m_info, MMC_SEARCH *p)
{
    if (p->search_type == E_SEARCH_VIDEO_DIR || p->search_type == E_SEARCH_SNAP_DIR)
    {
        PRINT_ERR("search_type = %d error.\n", p->search_type);
        return -1;
    }

    DIR *pDir;
    struct dirent *ent;
    AR_U32 num;

    AR_U64 time_start_64, time_stop_64;

    /* 当时分秒全0，且年月日同一天的时候，都在时间内 */
    int is_add = 0;
    char time_str[20] = {0};
    sprintf(time_str, "%s000000", dir);
    AR_U64 tmp64 = (unsigned long long)atoll(time_str);
    if ((p->start == tmp64) && (p->stop == tmp64))
    {
        is_add = 1;
    }

    char dir_path[128];
    memset(dir_path, 0, sizeof(dir_path));
    if (p->search_type == E_SEARCH_VIDEO_FILE)
        sprintf(dir_path, "%s/%s", GRD_SD_VIDEO_PATH, dir);
    else if (p->search_type == E_SEARCH_SNAP_FILE)
        sprintf(dir_path, "%s/%s", GRD_SD_SNAP_PATH, dir);

    pDir = opendir(dir_path);
    while((ent = readdir(pDir)) != NULL)
    {
        if(ent->d_type & DT_REG)  //判断是否为非目录
        {
            if (p->search_type == E_SEARCH_VIDEO_FILE)
            {
                if (0 == is_video_file_format(ent->d_name, p->rec_file_type))
                    continue;

                num = AR_AVI_GetChnFromName(ent->d_name);

                if(p->channel != num && p->channel != 0xFF)
                    continue;

                time_start_64 = AR_AVI_GetStartTimeFromName(dir, ent->d_name);
                time_stop_64 = AR_AVI_GetStopTimeFromName(dir, ent->d_name);
                /* 过了24点，结束时间加1天 */
                if (time_stop_64 < time_start_64) {
                    AR_U64 tmp = 0;
                    AR_AVI_U64AddSeconds(time_stop_64, 24 * 60 * 60, &tmp);
                    time_stop_64 = tmp;
                }

                if (((time_stop_64 < p->start || time_start_64 > p->stop)) && (is_add == 0))
                    continue;

                if (m_info)
                {
                    int ret = check_video_file_by_manage(m_info, p->channel, time_start_64, time_stop_64);
                    if (ret == 1)
                    {
                        insert_node(list, ent->d_name, dir_path);
                    }
                }
                else
                {
                    insert_node(list, ent->d_name, dir_path);
                }


            }
            else if (p->search_type == E_SEARCH_SNAP_FILE)
            {
                if (0 == is_snap_file_format(ent->d_name))
                    continue;

                num = AR_AVI_GetChnFromName(ent->d_name);
                if(p->channel != num && p->channel != 0xFF)
                    continue;

                time_start_64 = AR_AVI_GetStartTimeFromName(dir, ent->d_name);
                time_stop_64 = time_start_64;
                if ((time_stop_64 < p->start || time_start_64 > p->stop) && (is_add == 0))
                    continue;

                if (m_info)
                {
                    int ret = check_sanp_file_by_manage(m_info, p->channel, time_start_64);
                    if (ret == 1)
                    {
                        insert_snap_node(list, ent->d_name, dir_path);
                    }
                }
                else
                {
                    insert_snap_node(list, ent->d_name, dir_path);
                }

            }
        }
    }

	closedir(pDir);
    return 0;
}


static void search_add_to_list(FILE_LIST *list, MMC_SEARCH *p)
{
    DIR *pDir;
    struct dirent *ent;
    AR_U32 date = 0;

    AR_U32 date_start_32, date_stop_32;
    date_start_32 = (AR_U32)(p->start / 1000000); //去掉时分秒
    date_stop_32 = (AR_U32)(p->stop / 1000000);

    /* 当全0的时候，都在时间内 */
    int is_add = 0;
    if ((p->start == 0) && (p->stop == 0))
    {
        is_add = 1;
    }


    MANAGER_INFO *m_info = NULL;
    if (p->type != RECORD_TYPE_ALL)
         m_info = read_open_manage_info(p->type);

    char dir_path[128];
    memset(dir_path, 0, sizeof(dir_path));
    if (p->search_type == E_SEARCH_VIDEO_DIR || p->search_type == E_SEARCH_VIDEO_FILE)
        sprintf(dir_path, "%s", GRD_SD_VIDEO_PATH);
    else if (p->search_type == E_SEARCH_SNAP_DIR || p->search_type == E_SEARCH_SNAP_FILE)
        sprintf(dir_path, "%s", GRD_SD_SNAP_PATH);

    pDir = opendir(dir_path);
    if (pDir == NULL)
    {
        PRINT_ERR("opendir %s error\n", dir_path);
        goto end;
    }

    while((ent = readdir(pDir)) != NULL)
    {
        if(ent->d_type & DT_DIR)  //判断是否为目录
        {
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;

            if(is_date_dir_format(ent->d_name) == 0)
                continue;

            date = (AR_U32)atoi(ent->d_name);

            //if((date >= date_start_32) && (date <= date_stop_32))
            if ( ((date >= date_start_32) && (date <= date_stop_32)) \
                || (is_add == 1) )
            {

                if (p->search_type == E_SEARCH_VIDEO_DIR || p->search_type == E_SEARCH_SNAP_DIR)
                {
                    if (m_info)
                    {
                        int ret = check_dir_by_manage(m_info, p->channel, date);
                        if (ret == 1)
                        {
                            insert_dir_node(list, ent->d_name, NULL);
                        }
                    }
                    else
                    {
                        insert_dir_node(list, ent->d_name, NULL);
                    }
                }
                else
                {
                    add_file_to_list(list, ent->d_name, m_info, p);
                }
            }
        }
    }

	closedir(pDir);

end:
    if (m_info)
        read_close_manage_info(m_info);

    return;
}






/**********************************************************************
函数描述：查找当月录像天数
入口参数：u32t type: 安普定义如下
          RECORD_TYPE_SCHED   定时录像
          RECORD_TYPE_MOTION  移到侦测录像
          RECORD_TYPE_ALARM   手动报警录像
          RECORD_TYPE_MANU    手工录像
          AR_U32 ch_num: 通道号
          AR_U64 start: 起始时间
          AR_U64 stop:  结束时间
返回值：  FILE_LIST *: 返回的查找文件控制句柄
**********************************************************************/
unsigned int search_day_by_month(AR_U32 type, AR_U32 ch_num, int year, int month)
{
    char tmp[10];
    AR_U32 date_start_32, date_stop_32;

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "20%02d%02d01", year, month);
    date_start_32 = (AR_U32)atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "20%02d%02d31", year, month);
    date_stop_32 = (AR_U32)atoi(tmp);

    printf("start:%u stop:%u\n", date_start_32, date_stop_32);

    DIR *pDir;
    struct dirent *ent;
    AR_U32 date = 0;
    unsigned int mask = 0;
    int day = 0;
    pDir = opendir(GRD_SD_VIDEO_PATH);
    if(pDir==NULL)
    {
		return mask;
    }
    while((ent = readdir(pDir)) != NULL)
    {
        if(ent->d_type & DT_DIR)  //判断是否为目录
        {
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;

            if(is_date_dir_format(ent->d_name) == 0)
                continue;

            date = (AR_U32)atoi(ent->d_name);

            if((date >= date_start_32) && (date <= date_stop_32))
            {
                day = date % 100;
                printf("day: %d \n", day);
                mask |= (1 << day);
            }
        }
    }
	closedir(pDir);

	printf("==============0x%x, mask:%d\n", (unsigned int)pDir, mask);
    return mask;
}

/**********************************************************************
函数描述：查找当月录像天数
入口参数：u32t type: 安普定义如下
          RECORD_TYPE_SCHED   定时录像
          RECORD_TYPE_MOTION  移到侦测录像
          RECORD_TYPE_ALARM   手动报警录像
          RECORD_TYPE_MANU    手工录像
          AR_U32 ch_num: 通道号
          AR_U64 start: 起始时间
          AR_U64 stop:  结束时间
返回值：  FILE_LIST *: 返回的查找文件控制句柄
**********************************************************************/
unsigned int search_snap_day_by_month(AR_U32 type, AR_U32 ch_num, int year, int month)
{
    char tmp[10];
    AR_U32 date_start_32, date_stop_32;

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "20%02d%02d01", year, month);
    date_start_32 = (AR_U32)atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "20%02d%02d31", year, month);
    date_stop_32 = (AR_U32)atoi(tmp);

    printf("start:%u stop:%u\n", date_start_32, date_stop_32);

    DIR *pDir;
    struct dirent *ent;
    AR_U32 date = 0;
    unsigned int mask = 0;
    int day = 0;
    pDir = opendir(GRD_SD_SNAP_PATH);
    if(pDir==NULL)
    {
		return mask;
    }
    while((ent = readdir(pDir)) != NULL)
    {
        if(ent->d_type & DT_DIR)  //判断是否为目录
        {
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;

            if(is_date_dir_format(ent->d_name) == 0)
                continue;

            date = (AR_U32)atoi(ent->d_name);

            if((date >= date_start_32) && (date <= date_stop_32))
            {
                day = date % 100;
                printf("day: %d \n", day);
                mask |= (1 << day);
            }
        }
    }
	closedir(pDir);

	printf("==============0x%x, mask:%d\n", (unsigned int)pDir, mask);
    return mask;
}


FILE_LIST *search_by_time(MMC_SEARCH *p)
{
    FILE_LIST *list = NULL;
    list = creat_empty_list(p->type, p->channel, p->start, p->stop);
    if(list == NULL)
    {
        PRINT_ERR();
        return NULL;
    }

    PRINT_INFO("after create list: num = %u, type = %u, start = %llu, stop = %llu\n",
            list->ch_num, list->type, list->start, list->stop);


    search_add_to_list(list, p);
    return list;
}

/* date不在info列表里，则返回0，存着则返回1，错误返回-1 */
int check_sanp_file_by_manage(MANAGER_INFO *m_info, AR_U32 ch_num, AR_U64 start)
{
    if (m_info == NULL)
    {
        PRINT_ERR();
        return -1;
    }
    if (m_info->num <= 0)
    {
        PRINT_ERR();
        return -1;
    }

    int i, count;

    count = m_info->num;
    INDEX_NODE *node = m_info->h;
    int check = 0;
    for(i = 0; i < count; i ++)
    {
        if((ch_num != node->ch_num) && (ch_num != 0xFF))
            continue;

        if ((start >= node->start) && (start <= node->stop))
        {
            check = 1;
            break;
        }

        node ++;
    }

    return check;
}

/* date不在info列表里，则返回0，存着则返回1，错误返回-1 */
int check_video_file_by_manage(MANAGER_INFO *m_info, AR_U32 ch_num, AR_U64 start, AR_U64 stop)
{
    if (m_info == NULL)
    {
        PRINT_ERR();
        return -1;
    }
    if (m_info->num <= 0)
    {
        PRINT_ERR();
        return -1;
    }

    int i, count;
    AR_U64 begin = 0;
    AR_U64 end = 0;
    count = m_info->num;
    INDEX_NODE *node = m_info->h;
    int check = 0;
    for(i = 0; i < count; i ++)
    {
        if((ch_num != node->ch_num) && (ch_num != 0xFF))
            continue;

        if((node->stop < start) || (stop < node->start))
            continue;

        if((start <= node->start) && (node->stop <= stop))
        {

            begin = node->start;
            end = node->stop;
            check = 1;
            break;
        }
        else if((node->start <= start) && (stop <= node->stop))
        {

            begin = start;
            end = stop;
            check = 1;
            break;
        }
        else if((node->start <= start) && (start <= node->stop))
        {

            begin = start;
            end = node->stop;
            check = 1;
            break;
        }
        else if((node->start <= stop) && (stop <= node->stop))
        {

            begin = node->start;
            end = stop;
            check = 1;
            break;
        }

        if (begin == end) {
            PRINT_INFO("begin:%llu end:%llu\n", begin, end);
        }

        node ++;
    }

    return check;
}

/* date不在info列表里，则返回0，存着则返回1，错误返回-1 */
int check_dir_by_manage(MANAGER_INFO *m_info, AR_U32 ch_num, AR_U32 date)
{
    if (m_info == NULL)
    {
        PRINT_ERR();
        return -1;
    }
    if (m_info->num <= 0)
    {
        PRINT_ERR();
        return -1;
    }

    int i;
    INDEX_NODE *node = m_info->h;

    int check = 0;
    for(i = 0; i < m_info->num; i ++)
    {
        if((ch_num != node->ch_num) && (ch_num != 0xFF))
            continue;

        AR_U32 date_start_32, date_stop_32;
        date_start_32 = (AR_U32)(node->start / 1000000); //去掉时分秒
        date_stop_32 = (AR_U32)(node->stop / 1000000);

        if ((date >= date_start_32) && (date <= date_stop_32))
        {
            check = 1;
            break;
        }

        node ++;
    }

    return check;
}

// -1:error,   0:avi,  1:jpg
int mmc_check_file_type(char *path)
{
    int ret = -1;
    int i;
    char suffix[10] = {0};
    i = AR_AVI_FindLastIndex(path, '.');

    strcpy(suffix, path + i + 1);

    if (strncmp(suffix, "avi", 3) == 0) {
        ret = 0;
    } else if (strncmp(suffix, "jpg", 3) == 0) {
        ret = 1;
    } else {
        ret = -1;
    }

    return ret;
}

int mmc_update_path(char *path)
{
    int type = mmc_check_file_type(path);
    char tmp[128] = {0};

    if (type == 0) {
        sprintf(tmp, "%s/%s", GRD_SD_VIDEO_PATH, path);
        strcpy(path, tmp);
        path[strlen(tmp)] = '\0';
    } else if (type == 1) {
        sprintf(tmp, "%s/%s", GRD_SD_SNAP_PATH, path);
        strcpy(path, tmp);
        path[strlen(tmp)] = '\0';
    } else {
        PRINT_ERR("type:%d error\n", type);
        return -1;
    }

    return 0;
}


#endif //if 0

