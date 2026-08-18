/*
* Company:	artosyn
* Author: 	weihh
* Date:	2020/12/14
*/
#include<string>
#include<vector>
#include "hal_npu_types.h"

using namespace std;
#ifndef __SSD_LEYER_H_
#define __SSD_LEYER_H_

typedef struct {
	float x;
	float y;
	float w;
	float h;
	unsigned int classId;
	float confidence;
}detection_output;

typedef enum _NET_TYPE
{
	MOBILENETSSD_300,
	MOBILENETSSD_PYTORCH_300,
	SQUEEZENETSSD_300
} NET_TYPE;

typedef struct DectParam
{
	int class_num = 0;
	int per_box_num = 0;     //box numbers per anchor
	int image_width = 0;
	int image_height = 0;
	int per_kind_layer = 0;
	int keep_box_max_num = 0;
	float nms_threshold = 0.;
	float dect_threshold = 0.;

	//ssd
	int nms_overlap_top_num = 0;
	int windows_num = 0;     //ssd windows num
	vector<int>conf_idex;   //ssd conf layer idex
	vector<int>loc_idex;    //ssd loc layer idex
	vector<float>delta_stds;
	vector<vector<float>>aspect_ratios;
	vector<int>min_size; //box min size
	vector<int>max_size;
	vector<vector<float>>step;

}DectParam;

class SSD_detection{

public:
	typedef struct SSD_NMS_Param{
		float *scores;
		int *keep;
	}SSD_NMS_Param;
	typedef struct SSD_Sort_Parm{
		float *values;
		int *sort;
		int *index;
		int num;
	}SSD_Sort_Parm;
	SSD_detection(NET_TYPE net_type, AR_NPU_TENSOR_S *outputs_tensor, int tensorNum, int class_num, float nms_threshold, float dect_threshold, int input_width, int input_height);
	~SSD_detection();
	void SSD_PostProcess(short *ssdBuff, vector<detection_output> &dets);
	void SSD_PostProcess(char *ssdBuff, vector<detection_output> &dets);
	void SSD_PostProcess(float *ssdBuff, vector<detection_output> &dets);
	void SSD_CHW_PostProcess(float *ssdBuff, vector<detection_output> &dets);
	void SSD_CHW_PostProcess(short *ssdBuff, vector<detection_output> &dets);
	void SSD_CHW_PostProcess(char *ssdBuff, vector<detection_output> &dets);
private:
	vector<vector<float>> priorbox;
	float* scores_flat;
	float* deltas_flat;
	float *softmaxScore;
	NET_TYPE net_type;
	DectParam dect_param;
	SSD_Sort_Parm decodeSort;
	SSD_NMS_Param nms;
	int tensorNum;
	AR_NPU_TENSOR_S *pTensor;
	void fixedToFloatHWCSSD(float *dst, short * src, AR_NPU_TENSOR_S &tensor, int index);
	void fixedToFloatHWCSSD(float *dst, char *src, AR_NPU_TENSOR_S &tensor, int index);
	void fixedToFloatHWCSSD(float *dst, float *src, AR_NPU_TENSOR_S &tensor, int index);
	void CHW_FloatToHWC_Float(float *dst,float *src, AR_NPU_TENSOR_S &tensor, int index);
	void CHW_ShortToHWC_Float(float *dst,short *src, AR_NPU_TENSOR_S &tensor, int index);
	void CHW_CharToHWC_Float(float *dst,char *src, AR_NPU_TENSOR_S &tensor, int index);
	void PermuteSSD(short *ssdBuff);
	void PermuteSSD(char *ssdBuff);
	void PermuteSSD(float *ssdBuff);
	void CHW_PermuteSSD(float *ssdBuff);
	void CHW_PermuteSSD(short *ssdBuff);
	void CHW_PermuteSSD(char *ssdBuff);

	int SSD_BboxDecoding(vector<detection_output> &dets);
	void GetPriorBoxesSSD();
	void SetDetectParam(int class_num, int input_width, int input_height, float nms_threshold, float dect_threshold);
	void BoxTransformSSD(float* tpbox, vector<float> &priorbox);
	int NonMaxSuppressSSD(vector<vector<float>>& boxes, int num_boxes);
	void AscendSortIndexSSD();
	void SoftmaxSSD();
};
#endif