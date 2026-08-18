#ifndef __RECMNG_SEARCH_H__
#define __RECMNG_SEARCH_H__

#include "pf_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct tagINDEX_NODE
{
    AR_U32 ch_num;
    AR_U64 start;
    AR_U64 stop;
}INDEX_NODE;

typedef struct
{
    AR_S32 type;
    AR_S32 num;
    INDEX_NODE *h;
} MANAGER_INFO;

#ifndef FILE_NODE
typedef struct tagFILE_NODE
{
    AR_CHAR path[128];
    AR_U64 start;
    AR_U64 stop;
    AR_U64 size;
    struct tagFILE_NODE *next;
} FILE_NODE;
#endif

typedef struct tagFILE_LIST
{
    AR_U64 start;
    AR_U64 stop;
    AR_U32 type;
    AR_U32 ch_num;

    FILE_NODE *head;
    AR_U32 len;
    AR_U32 index;
}FILE_LIST, *LPFILE_LIST;

typedef struct tagAVI_DMS_TIME
{
    AR_U64    dwYear;
    AR_U64    dwMonth;
    AR_U64    dwDay;
    AR_U64    dwHour;
    AR_U64    dwMinute;
    AR_U64    dwSecond;
} AVI_DMS_TIME, MMC_TIME;

typedef enum
{
    E_SEARCH_VIDEO_DIR,
    E_SEARCH_SNAP_DIR,
    E_SEARCH_VIDEO_FILE,
    E_SEARCH_SNAP_FILE,
} E_SEARCH_TYPE;


typedef enum emRECORD_TYPE_E
{
    RECORD_TYPE_SCHED   = 0x000001,  //定时录像 by slice
    RECORD_TYPE_MOTION  = 0x000002,  //移到侦测录像
    RECORD_TYPE_ALARM   = 0x000004,  //报警录像
    RECORD_TYPE_CMD     = 0x000008,  //命令录像
    RECORD_TYPE_MANU    = 0x000010,  //手工录像
    RECORD_TYPE_SCHED_2 = 0x000011,  //定时录像 by period
    RECORD_TYPE_ALL     = 0xff,      //全部录像
} E_RECORD_TYPE;


typedef struct
{
    E_SEARCH_TYPE search_type;
    E_RECORD_TYPE type;
    AR_U32 channel;
    AR_U64 start;
    AR_U64 stop;
    AR_S32 rec_file_type; //0:avi 1:mov
} MMC_SEARCH;

#define IPC_NET_FILE_SUCCESS 1000
#define IPC_NET_FILE_NOFIND 1001
#define IPC_NET_ISFINDING 1002
#define IPC_NET_NOMOREFILE  1003
#define IPC_NET_FILE_EXCEPTION  1004

typedef  struct
{
    AR_U64    dwSize;
    AR_CHAR             csFileName[100];
    AR_U64    dwFileType;
    AVI_DMS_TIME     stStartTime;
    AVI_DMS_TIME     stStopTime;
    AR_U64    dwFileSize;
    AR_UCHAR    byLocked;
    AR_UCHAR    byRes[3];
}IPC_NET_FINDDATA,*LPIPC_NET_FINDDATA;
extern AR_U32 search_day_by_month(AR_U32 type, AR_U32 ch_num, AR_S32 year, AR_S32 month);
extern AR_U32 search_snap_day_by_month(AR_U32 type, AR_U32 ch_num, AR_S32 year, AR_S32 month);
extern FILE_LIST *search_by_time(MMC_SEARCH *p);
extern AR_S32 get_file_node(FILE_LIST *list, FILE_NODE *node);
extern void search_close(FILE_LIST *list);
extern AR_U32 get_file_size(AR_CHAR *path);
extern AR_U64 get_file_size64(AR_CHAR *path);
extern void print_index_node(INDEX_NODE *n);
extern void print_node(FILE_NODE *n);
extern void print_list(FILE_LIST *list);
extern void print_finddata(IPC_NET_FINDDATA *p);
extern AR_S32 is_empty_list(FILE_LIST *list);
extern AR_S32 get_len_list(FILE_LIST *list);
AR_S32 check_sanp_file_by_manage(MANAGER_INFO *m_info, AR_U32 ch_num, AR_U64 start);
AR_S32 check_video_file_by_manage(MANAGER_INFO *m_info, AR_U32 ch_num, AR_U64 start, AR_U64 stop);
AR_S32 check_dir_by_manage(MANAGER_INFO *m_info, AR_U32 ch_num, AR_U32 date);

AR_S32 mmc_update_path(AR_CHAR *path);


#ifdef __cplusplus
}
#endif
#endif



