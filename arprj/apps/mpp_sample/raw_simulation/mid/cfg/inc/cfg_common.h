#ifndef _CFG_COMMON_H__
#define _CFG_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "util_common.h"
#include "cJSON.h"
#include "pf_type.h"
#ifdef THERMAL_ENABLE
#include "pf_thermal.h"
#endif

//common macro used for all headers
//#define IPC_MAX_PIPELINE_NUM  8
#define IPC_MAX_PIPELINE_NUM  1

#define IPC_MAX_STREAM_PER_PIPE  1
#define IPC_MAIN_STREAM     0
#define IPC_SUB_STREAM     1

#define IPC_TOTAL_STREAM_NUM (IPC_MAX_PIPELINE_NUM * IPC_MAX_STREAM_PER_PIPE)
#define IPC_GET_ID_FROM_CHN_STREAM(ch, stream)  ((ch) * IPC_MAX_STREAM_PER_PIPE + (stream))

//Totally support 32 venc chns

#define IPC_MAX_VENC_STREAM_PER_CHN    1

#define IPC_TOTAL_VENC_STREAM_NUM (IPC_MAX_PIPELINE_NUM * IPC_MAX_VENC_STREAM_PER_CHN)
#define IPC_GET_VENC_CHN_ID(ch, stream) ((ch) * IPC_MAX_VENC_STREAM_PER_CHN + (stream))
#define IPC_GET_VENC_CHN_FROM_ID(id) ((id) / IPC_MAX_VENC_STREAM_PER_CHN)
#define IPC_GET_VENC_STREAM_FROM_ID(id) ((id) % IPC_MAX_VENC_STREAM_PER_CHN)

typedef enum {
#if 0
    CFG_RUN_MODE_SINGLE_307 = 0,
    CFG_RUN_MODE_DUAL_307 = 1,
    CFG_RUN_MODE_SINGLE_464 = 2,
    CFG_RUN_MODE_SINGLE_415 = 3,
    CFG_RUN_MODE_DVP_8X1080P_25 = 4,
    CFG_RUN_MODE_SINGLE_307_HDR = 5,
    CFG_RUN_MODE_SINGLE_464_HDR = 6,
    CFG_RUN_MODE_FOUR_307 = 7,
    CFG_RUN_MODE_DVP_8X1080P_30 = 8,
#endif
    CFG_RUN_MODE_SINGLE_SC230AI_LOW_BW = 7,
    CFG_RUN_MODE_DUAL_SC230AI_LOW_BW = 8,
    CFG_RUN_MODE_SINGLE_307_LOW_BW = 9,
    CFG_RUN_MODE_DUAL_307_LOW_BW = 10,
    CFG_RUN_MODE_SINGLE_464_LOW_BW = 11,
    CFG_RUN_MODE_SINGLE_415_LOW_BW = 12,
    CFG_RUN_MODE_DVP_8X1080P_25_LOW_BW = 13,
    CFG_RUN_MODE_SINGLE_307_HDR_LOW_BW = 14,
    CFG_RUN_MODE_SINGLE_464_HDR_LOW_BW = 15,
    CFG_RUN_MODE_FOUR_307_LOW_BW = 16,
    CFG_RUN_MODE_DVP_8X1080P_30_LOW_BW = 17,
    CFG_RUN_MODE_SINGLE_307_ARIPC = 18,
    CFG_RUN_MODE_SINGLE_415_ARIPC = 19,
    CFG_RUN_MODE_SINGLE_464_ARIPC = 20,
    CFG_RUN_MODE_SINGLE_307_HDR_ARIPC = 21,
    CFG_RUN_MODE_SINGLE_464_HDR_ARIPC = 22,
    CFG_RUN_MODE_SINGLE_530AI_30 = 23,
    CFG_RUN_MODE_SINGLE_OV04A10_25 = 24,
    CFG_RUN_MODE_SINGLE_OV04A10_25_HDR = 25,
    CFG_RUN_MODE_SINGLE_347 = 26,
    CFG_RUN_MODE_SINGLE_OV04A10_25_ARIPC = 27,
    CFG_RUN_MODE_SINGLE_OV04A10_25_HDR_ARIPC = 28,
    CFG_RUN_MODE_SINGLE_307_ARS31 = 29,
    CFG_RUN_MODE_SINGLE_307_HDR_ARS31 = 30,
    CFG_RUN_MODE_SINGLE_OV04A10_ARS31 = 31,
    CFG_RUN_MODE_SINGLE_OV04A10_HDR_ARS31 = 32,
    CFG_RUN_MODE_SINGLE_TIMO256_ARS31 = 33,
    CFG_RUN_MODE_SINGLE_GST417W_ARS31 = 34,
    CFG_RUN_MODE_307_timo256_ARS31 = 35,
    CFG_RUN_MODE_DUAL_307_ARS31 = 36,
    CFG_RUN_MODE_OV04A10_GST417W_ARS31 = 37,
    CFG_RUN_MODE_SINGLE_GST212W4_ARS31 = 38,
    CFG_RUN_MODE_OV04A10_GST212W_ARS31 = 39,
    CFG_RUN_MODE_SINGLE_OV05A20_ARS31 = 40,
    CFG_RUN_MODE_OV05A20_GST212W_ARS31 = 41,
    CFG_RUN_MODE_SINGLE_H3812C1SG_ARS31 = 42,
    CFG_RUN_MODE_307_H3812C1SG_ARS31 = 43,
    CFG_RUN_MODE_ISP_RAW_SIMU = 44,
    CFG_RUN_MODE_BUTT
} CFG_RUN_MODE_E;

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
    double min;
    double max;

    char* description;
} CFG_MAP;

typedef struct {
    AR_U16  year;
    AR_U16  month;
    AR_U16  day;
    AR_U16  hour;
    AR_U16  minute;
    AR_U16  second;
} CFG_TIME, *LPCFG_TIME;

typedef struct {
    AR_S32 startHour;
    AR_S32 startMin;
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

extern void CfgSetRunMode(CFG_RUN_MODE_E enMode);
extern CFG_RUN_MODE_E CfgGetRunMode(void);

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

#ifdef THERMAL_ENABLE
#if THERMAL_TYPE == THERMAL_TYPE_IRAY_ELF
#define CFG_DIR "/usrdata/iray_demo/ipc_cfg/"
#elif (THERMAL_TYPE == THERMAL_TYPE_GD_TIMO256)
#define CFG_DIR "/usrdata/guide_demo/ipc_cfg/"
#else
#define CFG_DIR "/usrdata/local/ipc_cfg/"
#endif
#else
//#define CFG_DIR "/usrdata/local/ipc_cfg/"
#define CFG_DIR "/usrdata/local/raw_simu_cfg/"
#endif

#ifdef __cplusplus
}
#endif
#endif /* _CFG_COMMON_H__ */

