#ifndef __ARCNN_PROCESS_H__
#define __ARCNN_PROCESS_H__

#include <string>
#include <vector>

#include "arcnn_process.h"
#include "mpi_npu_api.h"
#include "mpi_region.h"
#include "binder_ipc.h"

#include "opencv2/opencv.hpp"

struct BlobData{
	int count;
	float* data;
	int num;
	int channels;
	int height;
	int width;
	int capacity;
};

const unsigned int POSE_MAX_PEOPLE = 96;

BlobData* createBlob(int num, int channels, int height, int width);

void releaseBlob(BlobData** blob);

void hwcTochw(const char * hwc, AR_NPU_TENSOR_S * pTensor, float *chw);

int entryIndex(int h, int w, int c, int byteUnit, AR_NPU_TENSOR_S &pTensorInfo);

void computeNms(BlobData* bottom_blob, BlobData* top_blob, float threshold);

void connectBodyParts(const float* const heatMapPtr, const float* const peaksPtr,
	const cv::Size& heatMapSize, const int maxPeaks, const int interMinAboveThreshold,
	const float interThreshold, const int minSubsetCnt, const float minSubsetScore, const float scaleFactor, std::vector<float>& poseKeypoints, std::vector<int>& keypointShape);

int renderKeypointsSoc(ar_ipc_state_t* pstClient, AR_U32 u32Handle, AR_S32 u32VpssGrp, const std::vector<float>& keypoints, std::vector<int> keyshape, const float threshold, int imagewidth, int imageheight);

void renderPoseKeypointsCpu(cv::Mat& frame, const std::vector<float>& poseKeypoints, std::vector<int> keyshape,
	const float renderThreshold, float scale, const bool blendOriginalFrame = true);

#endif

