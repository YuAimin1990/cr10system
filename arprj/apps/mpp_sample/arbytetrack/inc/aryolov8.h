#include "mpi_npu_api.h"
#include "opencv2/opencv.hpp"
#include "BYTETracker.h"

using namespace std;
using namespace cv;

#ifndef __ARYOLOV8_H__
#define __ARYOLOV8_H__

#define MAX_NUM_DET 300
#define MAX_NUM_CAN 1000

#define GET_MAX(a, b)	((a) > (b) ? (a) : (b))
#define GET_MIN(a, b)	((a) < (b) ? (a) : (b))

typedef struct
{
	AR_U32     	useNeon;
	AR_U32 		net_w;
	AR_U32 		net_h;
	AR_U32 		img_w;
	AR_U32 		img_h;	
	AR_FLOAT   	ratio;
	AR_FLOAT 	conf_thres;
	AR_FLOAT 	iou_thres;
	cv::Mat 	oriImg;
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
}candidate_Result;

typedef struct
{
	AR_FLOAT    x_min;
	AR_FLOAT 	y_min;
	AR_FLOAT 	x_max;
	AR_FLOAT 	y_max;
	AR_FLOAT 	score;
	AR_U32  	clsId;
}detector_Result;


AR_VOID getTensorId(void* handle, AR_U32 *tesorsId);

AR_U32 getIndex(AR_U32 h, AR_U32 w, AR_U32 c, AR_U32 byteUnit, AR_NPU_TENSOR_S * pTensor);

AR_S32 cmp(const void *a, const void *b);

AR_FLOAT cal_iou(candidate_Result& a, candidate_Result& b);

AR_U32 nms_proc(candidate_Result *canResult, AR_U32 u32NumCan, vector<Object>& detResult, yolov8_Param& param);

AR_VOID xywh2xyxy(candidate_Result *canResult, AR_U32 u32NumCan, yolov8_Param& param);

AR_U32 find_HW_maxConf_neon(AR_CHAR *npuOutBuff, AR_NPU_TENSOR_S& tensor, yolov8_Param& param, candidate_Result *canResult, AR_U32 u32Idx, AR_U32 u32NumCan);

AR_U32 yolov8_postprocess(void *handle, AR_CHAR *npuOutBuff, yolov8_Param& yoloParam, vector<Object>& detResult, candidate_Result *canResult, AR_U32 *tensorsId);

#endif

