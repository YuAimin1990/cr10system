//Common APIs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include <list>
//#include "ar_ion.h"
#include "hal_sys.h"
#include "mpi_npu_api.h"
#include "armtcnn_common.h"
#include "opencv2/opencv.hpp"

using namespace std;
#define IDW 0
#define IDH 1
extern int final_box[][2];

AR_S32 GetImageFileType(AR_CHAR * pchFileName)
{
    AR_CHAR * pchExt = NULL;
    AR_CHAR * pchTmp = NULL;
    AR_S32 s32Ret = 0;

    if(!pchFileName)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    //ignore "." ".." and any non-image files
    if(strcmp(pchFileName, ".") == 0 || strcmp(pchFileName, "..") == 0)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    pchExt = strrchr(pchFileName, '.');
    if(!pchExt || pchExt == pchFileName)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    //convert to lowercase
    pchTmp = pchExt;
    for(; *pchTmp; ++pchTmp) *pchTmp = tolower(*pchTmp);

    if(strcmp(pchExt, ".rgb") == 0)
    {
        s32Ret = RGB_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".bgr") == 0)
    {
        s32Ret = BGR_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".rgbd") == 0)
    {
        s32Ret = RGBD_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".jpg") == 0 || strcmp(pchExt, ".bmp") == 0 || strcmp(pchExt, ".png") == 0)
    {
        s32Ret = JPG_BMP_PNG_IMG_FILE;
    }
    else
    {
        s32Ret = INVALID_IMG_FILE;
    }

    //Need to add RGBD or RGB interleave format later..

    return s32Ret;
}

AR_S32 CalcNMS(BondingBoxList & listBBox, AR_DOUBLE fThresh, AR_U32 u32Mode)
{
    BondingBoxList::iterator it, it1;
    AR_DOUBLE fRatio = 0.0;
    //int cnt = 0, step = 0;

    printf("Calculating NMS.%f mode %d..\n", fThresh, u32Mode);

    for(it = listBBox.begin(); it != listBBox.end(); it++)
    {
        if(!it->u32Valid)
        {
            continue;
        }

        it1 = std::next(it); //next one
        while(it1 != listBBox.end())
        {
            if(!it1->u32Valid)
            {
                it1++;
                continue;
            }

            AR_DOUBLE fOverlap = 0;
            if(it1->fX0 >= it->fX1 || it1->fY0 >= it->fY1
                || it1->fX1 <= it->fX0 || it1->fY1 <= it->fY0) //no overlap at all
            {
                fOverlap = 0.0;
                fRatio = 0.0;
            }
            else
            {
                AR_DOUBLE fArea = (it->fX1 - it->fX0 + 1) * (it->fY1 - it->fY0 + 1);
                AR_DOUBLE fArea1 = (it1->fX1 - it1->fX0 + 1) * (it1->fY1 - it1->fY0 + 1);
                AR_DOUBLE fIOUX0 = MAX(it->fX0, it1->fX0);
                AR_DOUBLE fIOUY0 = MAX(it->fY0, it1->fY0);
                AR_DOUBLE fIOUX1 = MIN(it->fX1, it1->fX1);
                AR_DOUBLE fIOUY1 = MIN(it->fY1, it1->fY1);
                fOverlap = (fIOUX1 - fIOUX0 + 1) * (fIOUY1 - fIOUY0 + 1);

                if(u32Mode == ARMTCNN_NMS_MIN)
                {
                    fRatio = fOverlap / MIN(fArea, fArea1);
                }
                else //Union
                {
                    fRatio = fOverlap / (fArea + fArea1 - fOverlap);
                }

                if( (it1->u32IndexW == 134 && it1->u32IndexH == 56) ||
                    (it1->u32IndexW == 134 && it1->u32IndexH == 58))
                {
                    std::cout << "NMS orig: it: " << it->u32IndexW << " " << it->u32IndexH
                        << " score " << it->dScore <<" [ " << it->fX0 << " " << it->fY0 << " " << it->fX1 << " " << it->fY1 << " ] " << endl;
                    std::cout << "NMS delete: it1: " << it1->u32IndexW << " " << it1->u32IndexH
                        << " score " << it->dScore <<" [ " << it1->fX0 << " " << it1->fY0 << " " << it1->fX1 << " " << it1->fY1 << " ] " << endl;

                    std::cout << "Cond: ratio: " << fRatio << " area " << fArea << " area1 " << fArea1 << " iou " << fOverlap << endl;
                }
            }

            if(fRatio > fThresh) //if different score delete it.
            {
                it1->u32Valid = 0;
                //delete this box from the list
                it1 = listBBox.erase(it1);
            }
            else
            {
                it1++;
            }

            //cnt++;
            //if(cnt % 10000 == 0) printf("==Run %dx10000 times\n", cnt/10000);

        }
        //cnt = 0;
        //if(step++ % 100 == 0) printf("==step %dx100 List size: %d\n", step/100, listBBox.size());
    }

    return 0;
}

//rec to square
AR_S32 ReRectangle(BondingBoxList::iterator    stBBox)
{
    AR_DOUBLE fW = stBBox->fX1 - stBBox->fX0 + 1;
    AR_DOUBLE fH = stBBox->fY1 - stBBox->fY0 + 1;
    AR_DOUBLE fL = MAX(fW, fH);
    AR_DOUBLE fX0 = stBBox->fX0 + fW / 2 - fL / 2;
    AR_DOUBLE fY0 = stBBox->fY0 + fH / 2 - fL / 2;
    //fX0 = fX0 < 0 ? 0 : fX0;
    //fY0 = fY0 < 0 ? 0 : fY0;
    stBBox->fX0 = fX0;
    stBBox->fY0 = fY0;
    stBBox->fX1 = fX0 + fL - 1;
    stBBox->fY1 = fY0 + fL - 1;

    return 0;
}

AR_S32 BBoxRegression(BondingBoxList::iterator stBBox)
{
    AR_DOUBLE fBoxW = stBBox->fX1 - stBBox->fX0 + 1;
    AR_DOUBLE fBoxH = stBBox->fY1 - stBBox->fY0 + 1;
    stBBox->fX0 = stBBox->fX0 + (stBBox->fDx0 * fBoxW);
    stBBox->fY0 = stBBox->fY0 + (stBBox->fDy0 * fBoxH);
    stBBox->fX1 = stBBox->fX1 + (stBBox->fDx1 * fBoxW);
    stBBox->fY1 = stBBox->fY1 + (stBBox->fDy1 * fBoxW);

    return 0;
}

void SaveRGBImg(AR_CHAR * pR, AR_CHAR * pG, AR_CHAR * pB, AR_U32 u32W, AR_U32 u32H, AR_CHAR * pName)
{
    cv::Mat img;
    cv::Mat R(u32H, u32W, CV_8UC1, pR);
    cv::Mat G(u32H, u32W, CV_8UC1, pG);
    cv::Mat B(u32H, u32W, CV_8UC1, pB);

    std::vector<cv::Mat> channels{B, G, R};

    cv::merge(channels, img);

    cv::imwrite(pName, img);
}
#define ARMTCNN_STRIDE      2
#define ARMTCNN_CELLSIZE    12.0f

AR_DOUBLE getBboxTop(AR_U32 u32Idx, AR_DOUBLE dScale)
{
    return (ARMTCNN_STRIDE * u32Idx + 1) / dScale;
}
AR_DOUBLE getBboxBottom(AR_U32 u32Idx, AR_DOUBLE dScale)
{
    return (ARMTCNN_STRIDE * u32Idx + ARMTCNN_CELLSIZE -1 + 1) / dScale;
}

#if 0
void FixToFloatScale(char* input_data, double* output_data, long long data_size, double scale_factor, int zero_point, int precision)
{
	for (long long ind = 0; ind < data_size; ++ind)
	{
		if (precision == 16)
		{
		    signed short * input = (signed short *)input_data;
            output_data[ind] = (input[ind] - zero_point) * scale_factor;
		}
		else
		{
    		signed char * input = (signed char *)input_data;
		    output_data[ind] = (input[ind] - zero_point) * scale_factor;
		}
	}
}

void GetConfidence(double* input_data, double* output_data, int map_size, int channel_size, int batch_size)
{
	for (int n = 0; n < batch_size; ++n)
	{
		double* input_cur = input_data + n * channel_size * map_size;
		double* output_cur = output_data + n * channel_size * map_size;
		double* sum_data = new double[map_size];
		memset(sum_data, 0., map_size * sizeof(double));

		for (int i = 0; i < map_size; ++i)
		{
			for (int c = 0; c < channel_size; ++c)
				sum_data[i] += input_cur[c * map_size + i];
		}

		for (int i = 0; i < map_size * channel_size; ++i)
			output_cur[i] = input_cur[i] / sum_data[i % map_size];
	}

}

#endif


AR_S32 GetMMZMemory(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size)
{
	AR_S32 s32Ret;
	s32Ret = ar_hal_sys_mmz_alloc(u64PhyAddr, pvVirtAddr, pstr_mmb, NULL, u32Size);
    	return s32Ret;
}

void ReleaseMMZMemory(AR_U64 u64PhyAddr, void* pvVirtAddr)
{
    	ar_hal_sys_mmz_free(u64PhyAddr, pvVirtAddr);
	u64PhyAddr = 0;
	pvVirtAddr = NULL;
}

