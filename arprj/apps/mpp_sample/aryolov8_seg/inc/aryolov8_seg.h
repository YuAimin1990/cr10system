#include "mpi_npu_api.h"
#include "opencv2/opencv.hpp"
#include "mpi_vgs.h"
#include <arm_neon.h>
#include "mpi_scaler_api.h"
#include "mpi_ifc_api.h"

using namespace std;
using namespace cv;

#ifndef __ARYOLOV8SEG_H__
#define __ARYOLOV8SEG_H__

#define MAX_NUM_DET 300
#define MAX_NUM_CAN 1000

#define GET_MAX(a, b)	((a) > (b) ? (a) : (b))
#define GET_MIN(a, b)	((a) < (b) ? (a) : (b))

typedef struct
{
	AR_UCHAR r[21] = {128, 0,	 128, 0,   128, 0,	 128, 64,  192, 64,  192, 64,  192, 64,  192, 0,  128, 0,   128, 0,	  128};
	AR_UCHAR g[21] = {0,   128,  128, 0,   0,   128, 128, 0,   0,   128, 128, 0,   0,   128, 128, 64, 64,  192, 192, 64,  64};
	AR_UCHAR b[21] = {0,   0,	 0,	  128, 128, 128, 128, 0,   0,   0,   0,	  128, 128, 128, 128, 0,  0,   0,   0,   128, 12};
}seg_color;


typedef struct
{
	AR_U32     	useNeon;
	AR_U32 		net_w;
	AR_U32 		net_h;
	AR_U32 		img_w;
	AR_U32 		img_h;
	AR_U32      mask_w;
	AR_U32      mask_h;
	AR_FLOAT   	ratio;
	AR_FLOAT 	conf_thres;
	AR_FLOAT 	iou_thres;
	cv::Mat 	oriImg;
	seg_color   color;
	
}yolov8_Param;

typedef struct
{
	AR_U32 		u32W = 0;
	AR_U32 		u32H = 0;
	AR_U32 		tensorIdx = 0;
	AR_U32 		u32Cls = 0;
	AR_FLOAT 	conf  = 0.0;
	AR_FLOAT 	x_min = 0.0;
	AR_FLOAT 	y_min = 0.0;
	AR_FLOAT 	x_max = 0.0;
	AR_FLOAT 	y_max = 0.0;
	AR_FLOAT    mask[32];
	AR_FLOAT    area = 0.0f;
}candidate_Result;


AR_VOID getTensorId(void* handle, AR_U32 *tensorsId);

AR_U32 getIndex(AR_U32 h, AR_U32 w, AR_U32 c, AR_U32 byteUnit, AR_NPU_TENSOR_S * pTensor);

AR_S32 cmp(const void *a, const void *b);

AR_FLOAT cal_iou(candidate_Result& a, candidate_Result& b);

AR_U32 nms_proc(candidate_Result *canResult, AR_U32 u32NumCan, candidate_Result *detResult, yolov8_Param& param);

AR_FLOAT sigmoid_op(AR_FLOAT x);

AR_FLOAT sigmoid_op_fast(AR_FLOAT x);

AR_U32 create_mask_neon(AR_CHAR* dataPtr, AR_U32* downsample, AR_FLOAT dscale, AR_S32 zeropoint, AR_NPU_TENSOR_S& maskTensor, AR_MEM_S& sMask, AR_FLOAT* mask, AR_U32 cls);

AR_VOID xywh2xyxy(candidate_Result *canResult, AR_U32 u32NumCan, yolov8_Param& param);

AR_U32 find_HW_maxConf_neon(AR_CHAR *npuOutBuff, AR_NPU_TENSOR_S tensor, yolov8_Param param, candidate_Result *canResult, AR_U32 u32Idx, AR_U32 u32NumCan);

AR_U32 yolov8_seg_process(void *handle, AR_CHAR *npuOutBuff, yolov8_Param& yoloParam, candidate_Result *postResult, AR_U32 *tensorsId, AR_MEM_S& argbuff);


#endif

