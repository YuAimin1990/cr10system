#ifndef __SSD_H__
#define __SSD_H__

#include "mpi_type.h"
#include "hal_npu_types.h"
#include "mpi_npu_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define IPC_ALG_OSD_MAX_NUM 32

typedef enum 
{
    MOBILENETSSD_300,
    MOBILENETSSD_PYTORCH_300,
    SQUEEZENETSSD_300
} SSD_NET_TYPE_E;
	
typedef struct
{
    int class_num;
    int per_box_num;	 //box numbers per anchor
    int image_width;
    int image_height;
    int per_kind_laye;
    int keep_box_max_num;
    float nms_threshold;
    float dect_threshol;
    
    //ssd
    int nms_overlap_top_num;
    int windows_num;	 //ssd windows num
    int conf_idex[6];	//ssd conf layer idex
    int conf_idex_num;
    int loc_idex[6];	//ssd loc layer idex
    int loc_idex_num;
    float delta_stds[4];
    int delta_stds_num;
    float aspect_ratios[6][4];
    int aspect_ratios_num;
    int min_size[6]; //box min size
    int min_size_num;
    int max_size[6];
    int max_size_num;
    float step[2][6];
    int step_mum;
}SSD_DECT_PARAM_S;
	
typedef struct
{
    float *values;
    int *sort;
    int *index;
    int num;
}SSD_SORT_PARAM_S;
	
typedef struct
{
    float *scores;
    int *keep;
    int num;
}SSD_NMS_PARAM_S;
	
typedef struct
{
    float * priorbox;
    int priorbox_num;
    float* scores_flat;
    int scores_flat_num;
    float* deltas_flat;
    int deltas_flat_num;
    float *softmaxScore;
    int softmaxScore_num;
    SSD_NET_TYPE_E net_type;
    SSD_DECT_PARAM_S dect_param;
    SSD_SORT_PARAM_S decodeSort;
    SSD_NMS_PARAM_S nms;
    int tensorNum;
    AR_NPU_TENSOR_S *pTensor;
} SSD_DECTION_S;
	
typedef struct {
    float x;
    float y;
    float w;
    float h;
    unsigned int classId;
    float confidence;
}SSD_DECTION_OUTPUT_S;

typedef struct {
	AR_U32 x;
	AR_U32 y;
	AR_U32 w;
	AR_U32 h;
	AR_CHAR scores[IPC_ALG_OSD_MAX_NUM];
} IPC_SSD_OUTPUT_INFO_S;

typedef struct
{
	IPC_SSD_OUTPUT_INFO_S *pstSsdInfo;
	AR_U32 u32OutNum;
}IPC_SSD_OUTPUT_S;

AR_S32 IPC_SVP_GetSsdPostProcessResult(void * handle, AR_MEM_S *stNpuOutBuffer, IPC_SSD_OUTPUT_S * pstSsdOutput);
void IPC_SVP_ReleaseSsdPostProcessResult(IPC_SSD_OUTPUT_S * pstSsdOutput);


#ifdef __cplusplus
}
#endif
#endif
