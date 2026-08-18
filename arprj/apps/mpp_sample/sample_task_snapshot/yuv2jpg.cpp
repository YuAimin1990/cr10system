#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "ar_comm_video.h"

using namespace std;

#include "opencv2/opencv.hpp"

extern "C" void yuv2jpg(VIDEO_FRAME_INFO_S *p_frame, char *out, int w, int h);
void yuv2jpg(VIDEO_FRAME_INFO_S *p_frame, char *out, int w, int h)
{
	int ylen = p_frame->stVFrame.u32Stride[0] * p_frame->stVFrame.u32Height;
	int ulen = ylen / 4;
	int vlen = ylen / 4;

	cv::Mat yuvImg(h * 3 / 2, w, CV_8UC1);
	cv::Mat jpgImg(h, w, CV_8UC1);

	memcpy(yuvImg.data, (void*)p_frame->stVFrame.u64VirAddr[0], ylen);
	memcpy(yuvImg.data + ylen, (void*)p_frame->stVFrame.u64VirAddr[1], ulen);
	memcpy(yuvImg.data + ylen + ulen, (void*)p_frame->stVFrame.u64VirAddr[2], vlen);

	cv::cvtColor(yuvImg, jpgImg, cv::COLOR_YUV2BGR_I420);
	cv::imwrite(out, jpgImg);
}
