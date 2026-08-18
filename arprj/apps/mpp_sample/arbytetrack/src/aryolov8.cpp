//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>

#ifdef CONFIG_BIT_32BITS
#include <errno.h>
#endif

#include "aryolov8.h"
#include <arm_neon.h>

using namespace std;
using namespace cv;


AR_VOID getTensorId(void* handle, AR_U32 *tensorsId)
{
	AR_NPU_TENSOR_S stTensor;
	AR_U32 u32Num = AR_MPI_NPU_GetOutputTensorNum(handle);
	
	//1x80x80x80: 0; 1x80x40x40: 1; 1x80x20x20: 2; 1x4x80x80: 3; 1x4x40x40: 4; 1x4x20x20: 5
	for(AR_U32 i=0; i<u32Num; i++)
	{
		AR_MPI_NPU_GetOutputTensorParam(handle, i, &stTensor);
		if(stTensor.u32Height == 80 && stTensor.u32Width == 80 && stTensor.u32OriChannels == 80)
			tensorsId[0] = stTensor.u32ID;
		else if(stTensor.u32Height == 40 && stTensor.u32Width == 40 && stTensor.u32OriChannels == 80)
			tensorsId[1] = stTensor.u32ID;
		else if(stTensor.u32Height == 20 && stTensor.u32Width == 20 && stTensor.u32OriChannels == 80)
			tensorsId[2] = stTensor.u32ID;
		else if(stTensor.u32Height == 80 && stTensor.u32Width == 80 && stTensor.u32OriChannels == 4)
			tensorsId[3] = stTensor.u32ID;
		else if(stTensor.u32Height == 40 && stTensor.u32Width == 40 && stTensor.u32OriChannels == 4)
			tensorsId[4] = stTensor.u32ID;
		else if(stTensor.u32Height == 20 && stTensor.u32Width == 20 && stTensor.u32OriChannels == 4)
			tensorsId[5] = stTensor.u32ID;
	}
}


AR_U32 getIndex(AR_U32 h, AR_U32 w, AR_U32 c, AR_U32 byteUnit, AR_NPU_TENSOR_S *pTensor)
{
	AR_U32 tensor_k_size_norm = pTensor->u32KSizeNorm;
	AR_U32 tensor_k_ddr_step = pTensor->u32KStep;
	AR_U32 tensor_row_ddr_step = pTensor->u32RowStep;
	AR_U32 tensor_k_norm_num = pTensor->u32KNormNum;
	AR_U32 tensor_last = pTensor->u32KSizeLast;

	AR_U32 AddressOffset = (c / tensor_k_size_norm)*tensor_k_ddr_step + h*tensor_row_ddr_step;
	AR_U32 memoryMigration = (c <tensor_k_norm_num*tensor_k_size_norm) ? \
		(AddressOffset + c%tensor_k_size_norm*byteUnit + w*tensor_k_size_norm*byteUnit) : (AddressOffset + (c - tensor_k_norm_num*tensor_k_size_norm)*byteUnit + w*tensor_last*byteUnit);

	return memoryMigration / byteUnit;
}


AR_S32 cmp(const void *a, const void *b)
{
	AR_FLOAT diff = ((candidate_Result*)a)->conf - ((candidate_Result*)b)->conf;
	
	if(diff < 0)
		return 1;
	else if(diff > 0)
		return -1;
	else
		return 0;
}


AR_FLOAT cal_iou(candidate_Result& a, candidate_Result& b)
{
	AR_FLOAT a_area = (a.x_max - a.x_min + 1) * (a.y_max - a.y_min + 1);
	AR_FLOAT b_area = (b.x_max - b.x_min + 1) * (b.y_max - b.y_min + 1);
	
	AR_FLOAT left = GET_MAX(a.x_min, b.x_min);
	AR_FLOAT right = GET_MIN(a.x_max, b.x_max);
	AR_FLOAT top = GET_MAX(a.y_min, b.y_min);
	AR_FLOAT bottom = GET_MIN(a.y_max, b.y_max);
	
	AR_FLOAT width = GET_MAX(right - left + 1, 0.f);
	AR_FLOAT height = GET_MAX(bottom - top + 1, 0.f);
	
	AR_FLOAT inter_area = height * width;
	AR_FLOAT union_area = a_area + b_area - inter_area;
	AR_FLOAT iou_ = inter_area / union_area;

	return iou_;
}


AR_U32 nms_proc(candidate_Result *canResult, AR_U32 u32NumCan, vector<Object>& detResult, yolov8_Param& param)
{
	AR_FLOAT iou = 0.0;
	Object object;
	
	qsort(canResult, u32NumCan, sizeof(candidate_Result), cmp);

	AR_U32 suppressed[u32NumCan] = {0};
	
	for(AR_U32 i = 0; i < u32NumCan; i++)
	{
		if(suppressed[i] == 1)
			continue;
		
		object.label 		= canResult[i].u32Cls;
		object.rect.x 		= canResult[i].x_min;
		object.rect.y 		= canResult[i].y_min;
		object.rect.width 	= canResult[i].x_max - canResult[i].x_min;
		object.rect.height 	= canResult[i].y_max - canResult[i].y_min;
		object.prob 		= canResult[i].conf;
		detResult.push_back(object);
		
		if(detResult.size() >= MAX_NUM_DET)
		{
			printf("WRNING: too many detBoxes cause buf overflow!\n");
		}
		
		for(AR_U32 j = i + 1; j < u32NumCan; j++)
		{
			if(suppressed[j] == 1)
				continue;
				
			iou = cal_iou(canResult[i], canResult[j]);
			if(iou >= param.iou_thres)
				suppressed[j] = 1;
		}

	}
	
	return 0;
}


AR_VOID xywh2xyxy(candidate_Result *canResult, AR_U32 u32NumCan, yolov8_Param& param)
{
	AR_FLOAT x=0.0, y=0.0, w=0.0, h=0.0;
	for(AR_U32 i=0; i<u32NumCan; i++)
	{
		x = canResult[i].x_min;
		y = canResult[i].y_min;
		w = canResult[i].x_max;
		h = canResult[i].y_max;
		
		canResult[i].x_min = (x - w/2) ;
		canResult[i].y_min = (y - h/2) ;
		canResult[i].x_max = (x + w/2) ;
		canResult[i].y_max = (y + h/2) ;
		
		if(canResult[i].x_min < 0)
			canResult[i].x_min = 0;
		if(canResult[i].y_min < 0)
			canResult[i].y_min = 0;
		if(canResult[i].x_max > param.net_w)
			canResult[i].x_max = param.net_w;
		if(canResult[i].y_max > param.net_h)
			canResult[i].y_max = param.net_h;
	}

}


template <typename T>
AR_VOID find_HW_loc(candidate_Result *canResult, T *npuOutBuff, AR_NPU_TENSOR_S& stTensor, AR_U32 u32NumCan, AR_U32 idx)
{
	T *locData 		= (T *)((AR_CHAR*)npuOutBuff + stTensor.u32Bank*0x2000000 + stTensor.u32Offset);
	T s32zeropoint 	= stTensor.s32ZeroPoint;
	AR_FLOAT dscale	= stTensor.dScaleFactor;
	if(sizeof(T) == 4)
	{
		s32zeropoint = 0;
		dscale		 = 1.0;
	}
	for(AR_U32 n=0; n<u32NumCan; n++)
	{
		if(canResult[n].tensorIdx == idx)
		{
			canResult[n].x_min = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 0, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			canResult[n].y_min = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 1, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			canResult[n].x_max = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 2, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			canResult[n].y_max = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 3, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
		}
	}
}


AR_U32 find_HW_maxConf_neon(AR_CHAR *npuOutBuff, AR_NPU_TENSOR_S& tensor, yolov8_Param& param, candidate_Result *canResult, AR_U32 u32Idx, AR_U32 u32NumCan)
{
	AR_U32 u32Cls		= 0;
	AR_U32 u32Index		= 0;
	AR_U32 u32Regbit 	= 128;
	AR_U32 u32Width 	= tensor.u32Width;
	AR_U32 u32Height 	= tensor.u32Height;
	AR_U32 u32NumInReg	= u32Regbit / tensor.u32Precision;
	AR_U32 wLoop 		= u32Width / u32NumInReg;
	AR_U32 wLeft 		= u32Width % u32NumInReg;
	AR_U32 u32Channel 	= tensor.u32OriChannels;
	AR_U32 u32ByteUint 	= tensor.u32Precision / 8;
	AR_S32 s32zeropoint = tensor.s32ZeroPoint;
	AR_DOUBLE dscale    = tensor.dScaleFactor;
	AR_FLOAT initValue 	= 0.001f;
	AR_FLOAT tempValue  = 0.0f;
	AR_CHAR* dataIn		= (AR_CHAR*)(npuOutBuff + tensor.u32Bank*0x2000000 + tensor.u32Offset);
	if(tensor.u32KSizeNorm != 1)
		printf("WARNING: only nchw format support neon! \n");
	
	if(tensor.u32Precision == 32)
	{
		AR_FLOAT *confdata = (AR_FLOAT*)dataIn;
		float32x4_t v0, v1;
		uint32x4_t vidx0, vidx1, vmask0;
		
		for(AR_U32 h=0; h<u32Height; h++)
		{
			for(AR_U32 w=0; w<wLoop; w++)
			{
				v0 = vmovq_n_f32(initValue);
				vidx0 = vmovq_n_u32(0);
				for(AR_U32 c=0; c<u32Channel; c++)
				{
					v1 = vld1q_f32((AR_FLOAT*)(confdata + getIndex(h, w*u32NumInReg, c, u32ByteUint, &tensor)));
					vidx1 = vmovq_n_u32(c);						
					vmask0 = vcgtq_f32(v1, v0);
					vidx0 = vbslq_u32(vmask0, vidx1, vidx0);
					v0 = vbslq_f32(vmask0, v1, v0);
				}
				
				for(AR_U32 i=0; i<u32NumInReg; i++)
				{
					if(v0[i] > param.conf_thres)
					{
						canResult[u32NumCan].conf = v0[i];
						canResult[u32NumCan].u32H = h;
						canResult[u32NumCan].u32W = w*u32NumInReg + i;
						canResult[u32NumCan].tensorIdx = u32Idx;
						canResult[u32NumCan].u32Cls = vidx0[i];
						u32NumCan++;
					}
				}			
			}
			if(wLeft != 0)
			{
				for(AR_U32 w=wLoop*u32NumInReg; w<wLeft; w++)
				{
					u32Cls = 0;
					tempValue = initValue;
					for(AR_U32 c=0; c<u32Channel; c++)
					{
						u32Index = getIndex(h, w, c, u32ByteUint, &tensor);
						if(confdata[u32Index] > tempValue)
						{
							tempValue = confdata[u32Index];
							u32Cls = c;
						}
					}
					if(tempValue > param.conf_thres)
					{
						canResult[u32NumCan].conf = tempValue;
						canResult[u32NumCan].u32H = h;
						canResult[u32NumCan].u32W = w;
						canResult[u32NumCan].tensorIdx = u32Idx;
						canResult[u32NumCan].u32Cls = u32Cls;
						u32NumCan++;
					}
				}
			}
		}
	}
	else if(tensor.u32Precision == 16)
	{
		AR_S16* confdata = (AR_S16*)dataIn;
		int16x8_t v0, v1;
		uint16x8_t vidx0, vidx1, vmask0;
		
		AR_S32 iThres 	  = param.conf_thres / dscale + s32zeropoint;
		AR_S16 iInitValue = initValue / dscale + s32zeropoint;
		AR_S16 iTempValue = 0;
		
		for(AR_U32 h=0; h<u32Height; h++)
		{
			for(AR_U32 w=0; w<wLoop; w++)
			{
				v0 = vmovq_n_s16(iInitValue);
				vidx0 = vmovq_n_u16(0);
				for(AR_U32 c=0; c<u32Channel; c++)
				{
					v1 = vld1q_s16((AR_S16*)(confdata + getIndex(h, w*u32NumInReg, c, u32ByteUint, &tensor)));
					vidx1 = vmovq_n_u16(c);						
					vmask0 = vcgtq_s16(v1, v0);
					vidx0 = vbslq_u16(vmask0, vidx1, vidx0);
					v0 = vbslq_s16(vmask0, v1, v0);
				}

				for(AR_U32 i=0; i<u32NumInReg; i++)
				{
					if(v0[i] > iThres)
					{
						canResult[u32NumCan].conf = (v0[i] - s32zeropoint) * dscale;
						canResult[u32NumCan].u32H = h;
						canResult[u32NumCan].u32W = w*u32NumInReg + i;
						canResult[u32NumCan].tensorIdx = u32Idx;
						canResult[u32NumCan].u32Cls = vidx0[i];
						u32NumCan++;
					}
				}			
			}
			if(wLeft != 0)
			{
				for(AR_U32 w=wLoop*u32NumInReg; w<u32Width; w++)
				{
					u32Cls = 0;
					iTempValue = iInitValue;
					for(AR_U32 c=0; c<u32Channel; c++)
					{
						u32Index = getIndex(h, w, c, u32ByteUint, &tensor);
						
						if(confdata[u32Index] > iTempValue)
						{
							iTempValue = confdata[u32Index];
							u32Cls = c;
						}
					}
					if(iTempValue > iThres)
					{
						canResult[u32NumCan].conf = (iTempValue - s32zeropoint) * dscale;
						canResult[u32NumCan].u32H = h;
						canResult[u32NumCan].u32W = w;
						canResult[u32NumCan].tensorIdx = u32Idx;
						canResult[u32NumCan].u32Cls = u32Cls;
						u32NumCan++;
					}
				}
			}
		}
	}
	else
	{
		AR_S8* confdata = (AR_S8*)dataIn;
		int8x16_t v0, v1;
		uint8x16_t vidx0, vidx1, vmask0;

		AR_S32 iThres     = param.conf_thres/dscale + s32zeropoint;
		AR_S8 iInitValue  = initValue/dscale + s32zeropoint;
		AR_S8 iTempValue  = 0;
		
		for(AR_U32 h=0; h<u32Height; h++)
		{
			for(AR_U32 w=0; w<wLoop; w++)
			{
				v0 = vmovq_n_s8(iInitValue);
				vidx0 = vmovq_n_u8(0);
				for(AR_U32 c=0; c<u32Channel; c++)
				{
					v1 = vld1q_s8((AR_S8*)(confdata + getIndex(h, w*u32NumInReg, c, u32ByteUint, &tensor)));
					vidx1 = vmovq_n_u8(c);						
					vmask0 = vcgtq_s8(v1, v0);
					vidx0 = vbslq_u8(vmask0, vidx1, vidx0);
					v0 = vbslq_s8(vmask0, v1, v0);
				}

				for(AR_U32 i=0; i<u32NumInReg; i++)
				{
					if(v0[i] > iThres)
					{
						canResult[u32NumCan].conf = (v0[i] - s32zeropoint) * dscale;
						canResult[u32NumCan].u32H = h;
						canResult[u32NumCan].u32W = w*u32NumInReg + i;
						canResult[u32NumCan].tensorIdx = u32Idx;
						canResult[u32NumCan].u32Cls = vidx0[i];
						u32NumCan++;
					}
				}			
			}
			if(wLeft != 0)
			{
				for(AR_U32 w=wLoop*u32NumInReg; w<u32Width; w++)
				{
					iTempValue = iInitValue;
					u32Cls = 0;
					for(AR_U32 c=0; c<u32Channel; c++)
					{
						u32Index = getIndex(h, w, c, u32ByteUint, &tensor);
						if(confdata[u32Index] > iTempValue)
						{
							iTempValue = confdata[u32Index];
							u32Cls = c;
						}
					}
					
					if(iTempValue > iThres)
					{
						canResult[u32NumCan].conf = (iTempValue - s32zeropoint) * dscale;
						canResult[u32NumCan].u32H = h;
						canResult[u32NumCan].u32W = w;
						canResult[u32NumCan].tensorIdx = u32Idx;
						canResult[u32NumCan].u32Cls = u32Cls;
						u32NumCan++;
					}
				}
			}
		}
	}
	
	return u32NumCan;
}


template <typename T>
AR_U32 find_HW_maxConf(T *npuOutBuff, AR_NPU_TENSOR_S& tensor, yolov8_Param& param, candidate_Result *canResult, AR_U32 u32Idx, AR_U32 u32NumCan)
{
	AR_S32 s32zeropoint = tensor.s32ZeroPoint;
	AR_DOUBLE dscale 	= tensor.dScaleFactor;
	AR_U32 u32Cls		= 0;
	AR_U32 u32Index		= 0;
	AR_U32 u32Width 	= tensor.u32Width;
	AR_U32 u32Height 	= tensor.u32Height;
	AR_U32 u32Channel 	= tensor.u32OriChannels;
	AR_U32 u32ByteUint 	= tensor.u32Precision / 8;
	AR_FLOAT init	 	= 0.001f;

	if(sizeof(T) == 4)
	{
		dscale 		 = 1;
		s32zeropoint = 0;
	}

	T initValue = init / dscale + s32zeropoint;
	T *confData	= (T *)((AR_CHAR*)npuOutBuff + tensor.u32Bank*0x2000000 + tensor.u32Offset);
	AR_FLOAT thres 	= param.conf_thres/dscale + s32zeropoint;

	for(AR_U32 h=0; h<u32Height; h++)
	{
		for(AR_U32 w=0; w<u32Width; w++)
		{
			u32Cls = 0;
			T  tempValue = initValue;
			for(AR_U32 c=0; c<u32Channel; )
			{
#ifdef NCHW_OUTPUT
				u32Index = getIndex(h, w, c, u32ByteUint, &tensor);	
				if(confData[u32Index] > tempValue)
				{
					tempValue = confData[u32Index];
					u32Cls = c;
				}
				c++;
				
#else  //blocking NHWC
				AR_U32 blockIndex = getIndex(h, w, c, u32ByteUint, &tensor);
				AR_U32 blockChanelNum = tensor.u32KSizeNorm;

				if(c >= tensor.u32KNormNum*tensor.u32KSizeNorm)
					blockChanelNum = tensor.u32KSizeLast;
				
				for(AR_U32 i=0; i<blockChanelNum; i++) {
					if(confData[blockIndex + i] > tempValue)
					{
						tempValue = confData[blockIndex + i];
						u32Cls = c+i;
					}
				}	
				c +=  blockChanelNum;
#endif
			}
			
			if(tempValue > thres)
			{
				canResult[u32NumCan].conf = (tempValue - s32zeropoint) * dscale;
				canResult[u32NumCan].u32H = h;
				canResult[u32NumCan].u32W = w;
				canResult[u32NumCan].tensorIdx = u32Idx;
				canResult[u32NumCan].u32Cls = u32Cls;
				u32NumCan++;
			}
		}
	}

	return u32NumCan;
}


/*
int 8, float output format, o3 compile
find_HW_maxConf_neon only support output format: NCHW, timecost 11ms
find_HW_maxConf support: NCHW cost 28ms; Blocking NHWC cost 8ms
*/
AR_U32 yolov8_postprocess(void *handle, AR_CHAR *npuOutBuff, yolov8_Param& yoloParam, vector<Object>& detResult, candidate_Result *canResult, AR_U32 *tensorsId)
{
	AR_U32 u32NumCan 	= 0;
	AR_NPU_TENSOR_S stTensor = {0};

	//getTensorId(handle, tensorsId);
	
    for(AR_U32 i=0; i <3; i++)
    {		
		AR_MPI_NPU_GetOutputTensorParamById(handle, tensorsId[i], &stTensor);
		
		if(yoloParam.useNeon)
			u32NumCan = find_HW_maxConf_neon((AR_CHAR*)npuOutBuff, stTensor, yoloParam, canResult, i, u32NumCan);
		else{
				if(stTensor.u32Precision == 32)
					u32NumCan = find_HW_maxConf((AR_FLOAT*)npuOutBuff, stTensor, yoloParam, canResult, i, u32NumCan);
				else if(stTensor.u32Precision == 16)
					u32NumCan = find_HW_maxConf((AR_S16*)npuOutBuff, stTensor, yoloParam, canResult, i, u32NumCan);
				else
					u32NumCan = find_HW_maxConf((AR_S8*)npuOutBuff, stTensor, yoloParam, canResult, i, u32NumCan);
			}
    }
	
	if(u32NumCan != 0)
	{
		for(AR_U32 i=0; i<3; i++)
		{
			AR_MPI_NPU_GetOutputTensorParamById(handle, tensorsId[i+3], &stTensor);
			
			if(stTensor.u32Precision == 32)
				find_HW_loc(canResult, (AR_FLOAT*)npuOutBuff, stTensor, u32NumCan, i);
			else if(stTensor.u32Precision == 16)
				find_HW_loc(canResult, (AR_S16*)npuOutBuff, stTensor, u32NumCan, i);
			else
				find_HW_loc(canResult, (AR_S8*)npuOutBuff, stTensor, u32NumCan, i);
		}
	}
	
	xywh2xyxy(canResult, u32NumCan, yoloParam);
	
	nms_proc(canResult, u32NumCan, detResult, yoloParam);
	
	return 0;
}



