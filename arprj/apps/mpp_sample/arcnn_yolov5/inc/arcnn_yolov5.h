#include<string>
#include "opencv2/opencv.hpp"
#include "hal_npu_types.h"

using namespace std;
using namespace cv;

#ifndef __ARCNN_YOLOv5_H__
#define __ARCNN_YOLOv5_H__

#define YOLOv5_POSTPROC			 1
#define MAX_NUM_DET			     100
#define MAX_NUM_REGION		     1000
#define MAX_OUTBRANCH_NUM	5
#define DUMP_MED_VALUE			 1

//#define OPT_POSTPROC				2 
#define OPT_NEON_REGION				1

#define GET_MAX(a, b)	((a) > (b) ? (a) : (b))
#define GET_MIN(a, b)	((a) < (b) ? (a) : (b))

typedef struct
{
    float xmin;
	float ymin;
	float xmax;
	float ymax;
	float score;
	int cls_id;
} YOLOv5_RSLT;

typedef struct
{
	int NetOutSize;
	int MaxFtSize;
	int FtSizes[MAX_OUTBRANCH_NUM];
	int strides[MAX_OUTBRANCH_NUM];
} MAP_SIZE;

typedef struct
{
	YOLOv5_RSLT *regionRslts;
	float *ContReshapeBuf;
	float *OutDataBuf;
	cv::Mat oriImg;
	AR_S32 *colors;
	AR_UCHAR *livecolors;
} USE_BUF;

typedef struct
{
    float conf_thr;
	float nms_thr;
	int net_w;
	int net_h;
	int img_w;
	int img_h;
	int class_num;
	USE_BUF buf;
	MAP_SIZE map_size;
	float ratio;
	int postproc_version;
} YOLOv5_PARAS;

int params_init(void *handle, YOLOv5_PARAS *params);
int yolov5_postprocess(char *NPUOutBuf, void * handle, YOLOv5_PARAS params, YOLOv5_RSLT *detRslts);
void params_uninit(YOLOv5_PARAS *params);
#endif

