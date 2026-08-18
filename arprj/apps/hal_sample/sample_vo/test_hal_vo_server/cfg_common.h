#ifndef _CFG_COMMON_H__
#define _CFG_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "util_common.h"
#include "cJSON.h"
#include "hal_type.h"

//common macro used for all headers
#define IPC_MAX_PIPELINE_NUM  8
#define IPC_MAX_STREAM_PER_PIPE  4
#define IPC_MAIN_STREAM     0
#define IPC_SUB_STREAM     1

#define IPC_TOTAL_STREAM_NUM (IPC_MAX_PIPELINE_NUM * IPC_MAX_STREAM_PER_PIPE)
#define IPC_GET_ID_FROM_CHN_STREAM(ch, stream)  ((ch) * IPC_MAX_STREAM_PER_PIPE + (stream))
#define IPC_GET_CHN_FROM_ID(id) ((id) / IPC_MAX_STREAM_PER_PIPE)
#define IPC_GET_STREAM_FROM_ID(id) ((id) % IPC_MAX_STREAM_PER_PIPE)

typedef enum {
	CFG_DATA_TYPE_U32 = 0,
	CFG_DATA_TYPE_U16,
	CFG_DATA_TYPE_U8,
	CFG_DATA_TYPE_S32,
	CFG_DATA_TYPE_S16,
	CFG_DATA_TYPE_S8,
	CFG_DATA_TYPE_FLOAT,
	//CFG_DATA_TYPE_DOUBLE,
	CFG_DATA_TYPE_STRING,
	CFG_DATA_TYPE_STIME,
	CFG_DATA_TYPE_SLICE,
} CFG_DATA_TYPE;

typedef struct
{
    char* 					stringName;
    void* 					dataAddress;
    CFG_DATA_TYPE		dataType;
    char*                   defaultValue;

    char* mode;
    double min; //下限
    double max; //上限

    char* description;
} CFG_MAP;

/*系统时间*/
typedef struct {
    AR_U16  year;
    AR_U16  month;
    AR_U16  day;
    AR_U16  hour;
    AR_U16  minute;
    AR_U16  second;
} CFG_TIME, *LPCFG_TIME;

typedef struct {
    //开始时间
    AR_S32 startHour;
    AR_S32 startMin;
    //结束时间
    AR_S32 stopHour;
    AR_S32 stopMin;
} CFG_SCHEDTIME;


extern int CfgWriteToFile(const char *filename, const char *data);
extern char *CfgReadFromFile(const char *filename);
extern char *CfgReadFromPath(const char *path);

extern int CfgPrintMap(CFG_MAP *mapArray);
extern int CfgSave(const char *filename, const char *str, CFG_MAP *mapArray);
extern int CfgLoad(const char *filename, const char *str, CFG_MAP *mapArray);
extern int CfgLoadDefValue(CFG_MAP *mapArray);

extern cJSON *CfgDataToCjsonByMap(CFG_MAP *mapArray);
extern int CfgCjsonToDataByMap(CFG_MAP *mapArray, cJSON *json);

extern int CfgAddCjson(cJSON *root, const char *str, CFG_MAP *mapArray);
extern int CfgParseCjson(cJSON *root, const char *str, CFG_MAP *mapArray);

extern int CfgGetDefByName(CFG_MAP *mapArray, const char *item_name, void *value);

extern int is_in_schedule_slice(AR_U32 *slice);
extern int is_in_schedule_timepoint(CFG_SCHEDTIME *time_point);

#define MAX_STR_LEN_16 16
#define MAX_STR_LEN_20 20
#define MAX_STR_LEN_32 32
#define MAX_STR_LEN_64 64
#define MAX_STR_LEN_128 128
#define MAX_STR_LEN_256 256

#define MAX_SYSTEM_STR_SIZE 32
#define MAX_TIME_STR_SIZE 32
#define MAX_URL_STR_SIZE 100

#define CFG_DIR ""

#ifdef __cplusplus
}
#endif
#endif /* _CFG_COMMON_H__ */

