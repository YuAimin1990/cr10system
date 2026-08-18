//this is the base class
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include <numeric>
#include <arm_neon.h>
#ifdef CONFIG_BIT_32BITS
#include <errno.h>
#endif
#include "mpi_scaler_api.h"
#include "aryolov8_seg.h"
#include "mpi_scaler_api.h"
#include "mpi_ifc_api.h"

#define NCHW_OUTPUT

using namespace std;
using namespace cv;


AR_VOID getTensorId(void* handle, AR_U32 *tensorsId)
{
	AR_NPU_TENSOR_S stTensor;
	AR_U32 tensorNum = AR_MPI_NPU_GetOutputTensorNum(handle);
	
	//1x80x80x80: 0; 1x80x40x40: 1; 1x80x20x20: 2; 1x4x80x80: 3; 1x4x40x40: 4; 1x4x20x20: 5; 
	//1x32x80x80: 6; 1x32x40x40: 7; 1x32x20x20: 8; 1x32x160x160: 9;
	
	for(AR_U32 i=0; i<tensorNum; i++)
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
		else if(stTensor.u32Height == 80 && stTensor.u32Width == 80 && stTensor.u32OriChannels == 32)
			tensorsId[6] = stTensor.u32ID;
		else if(stTensor.u32Height == 40 && stTensor.u32Width == 40 && stTensor.u32OriChannels == 32)
			tensorsId[7] = stTensor.u32ID;
		else if(stTensor.u32Height == 20 && stTensor.u32Width == 20 && stTensor.u32OriChannels == 32)
			tensorsId[8] = stTensor.u32ID;
		else if(stTensor.u32Height == 160 && stTensor.u32Width == 160 && stTensor.u32OriChannels == 32)
			tensorsId[9] = stTensor.u32ID;
	}

}


AR_U32 getIndex(AR_U32 h, AR_U32 w, AR_U32 c, AR_U32 byteUnit, AR_NPU_TENSOR_S * pTensor)
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


AR_FLOAT sigmoid_op(AR_FLOAT x)
{
	return 1.0/(1.0 + exp(-x));
}


AR_FLOAT fast_exp(AR_FLOAT y)		//fast_exp
{
	AR_FLOAT d;
	*(reinterpret_cast<short*>(&d) + 0) = 0;
	*(reinterpret_cast<short*>(&d) + 1) = static_cast<short>(184*y+(16256-7));
	return d;
}


AR_FLOAT sigmoid_op_fast(AR_FLOAT x)
{
	return 1.0/(1.0 + fast_exp(-x));
}


inline float32x4_t fast_sig_neon(float32x4_t y) //fast_exp_neon
{
	float32x4_t vMul, vAdd, vShift, vExpOut, v1;
	v1   = vdupq_n_f32(1.0f);
	vMul = vdupq_n_f32(-1.4426950409);
	vAdd = vdupq_n_f32(126.94201519);
	vShift = vdupq_n_f32(1<<23);

	vExpOut = vreinterpretq_f32_u32(vcvtq_u32_f32(vmulq_f32(vmlaq_f32(vAdd, vMul, y), vShift)));
	
	return vdivq_f32(v1, vaddq_f32(vExpOut, v1));
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
	a.area = a_area;
	
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


AR_U32 nms_proc(candidate_Result *canResult, AR_U32 numCan, candidate_Result *detResult, yolov8_Param& param)
{
	AR_FLOAT iou = 0.0;
	AR_U32 u32KeepNum = 0;
	
	qsort(canResult, numCan, sizeof(candidate_Result), cmp);

	AR_U32 suppressed[numCan] = {0};
	
	for(AR_U32 i = 0; i < numCan; i++)
	{
		if(suppressed[i] == 1)
			continue;
		
		detResult[u32KeepNum].u32Cls = canResult[i].u32Cls;
		detResult[u32KeepNum].x_min = canResult[i].x_min;
		detResult[u32KeepNum].y_min = canResult[i].y_min;
		detResult[u32KeepNum].x_max = canResult[i].x_max;
		detResult[u32KeepNum].y_max = canResult[i].y_max;
		detResult[u32KeepNum].conf = canResult[i].conf;
		detResult[u32KeepNum].u32H = canResult[i].u32H;
		detResult[u32KeepNum].u32W = canResult[i].u32W;
		detResult[u32KeepNum].tensorIdx = canResult[i].tensorIdx;
		u32KeepNum++;
		
		if(u32KeepNum >= MAX_NUM_DET)
		{
			printf("WRNING: too many detBoxes cause buf overflow!\n");
		}
		
		for(AR_U32 j = i + 1; j < numCan; j++)
		{
			if(suppressed[j] == 1)
				continue;
				
			iou = cal_iou(canResult[i], canResult[j]);
			if(iou >= param.iou_thres)
				suppressed[j] = 1;
		}

	}
	
	return u32KeepNum;
}


AR_VOID xywh2xyxy(candidate_Result *canResult, AR_U32 numCan, yolov8_Param& param)
{
	AR_FLOAT x=0.0, y=0.0, w=0.0, h=0.0;
	
	for(AR_U32 i=0; i<numCan; i++)
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
AR_VOID find_HW_loc(candidate_Result *canResult, T *npuOutBuff, AR_NPU_TENSOR_S stTensor, AR_U32 numCan, AR_U32 idx)
{
	T *locData = (T *)((AR_CHAR*)npuOutBuff + stTensor.u32Bank*0x2000000 + stTensor.u32Offset);
	T s32zeropoint = stTensor.s32ZeroPoint;
	AR_FLOAT dscale		= stTensor.dScaleFactor;
	
	if(sizeof(T) == 4)
	{
		s32zeropoint = 0;
		dscale		 = 1.0;
	}
	for(AR_U32 n=0; n<numCan; n++)
	{
		if(canResult[n].tensorIdx == idx)
		{
			canResult[n].x_min = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 0, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			canResult[n].y_min = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 1, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			canResult[n].x_max = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 2, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			canResult[n].y_max = (locData[getIndex(canResult[n].u32H, canResult[n].u32W, 3, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
		}
		else
			continue;
	}
}


template <typename T>
AR_VOID find_HW_mask(candidate_Result *detResult_cls, T *npuOutBuff, AR_NPU_TENSOR_S stTensor, AR_U32 objNum, AR_U32 idx)
{
	T *maskData = (T *)((AR_CHAR*)npuOutBuff + stTensor.u32Bank*0x2000000 + stTensor.u32Offset);
	T s32zeropoint 	= stTensor.s32ZeroPoint;
	AR_FLOAT dscale	= stTensor.dScaleFactor;
	if(sizeof(T) == 4)
	{
		s32zeropoint = 0;
		dscale		 = 1.0;
	}
	
	for(AR_U32 n=0; n<objNum; n++)
	{
		if(detResult_cls[n].tensorIdx == idx)
		{
			for(int c=0; c<32; c++)
			{
				detResult_cls[n].mask[c] = (maskData[getIndex(detResult_cls[n].u32H, detResult_cls[n].u32W, c, sizeof(T), &stTensor)] - s32zeropoint)*dscale;
			}
		}
		else
			continue;
	}
}


AR_U32 create_mask_neon(AR_CHAR* dataPtr, AR_U32* downsample, AR_FLOAT dscale, AR_S32 zeropoint, AR_NPU_TENSOR_S& maskTensor, AR_MEM_S& sMask, AR_FLOAT* mask, AR_U32 cls)
{
	float32x4_t f32Output0 	= vdupq_n_f32(0.0f);
	float32x4_t f32Output1 	= vdupq_n_f32(0.0f);
	float32x4_t vScale 		= vdupq_n_f32(dscale);
	int32x4_t   vZerop 		= vdupq_n_s32(zeropoint);
	
	if(maskTensor.u32KSizeNorm != 1)
		printf("WARNING: only nchw format support neon! \n");

	if(maskTensor.u32Precision == 8)
	{
		AR_S8* maskData = (AR_S8*)dataPtr;
		for(AR_U32 h=downsample[1]; h<downsample[3]; h++)
		{
			AR_U32 wLoop = (downsample[2] - downsample[0]) / 8;
			for(AR_U32 w=0; w<wLoop; w++)
			{
				for(AR_U32 c=0; c<maskTensor.u32OriChannels; c++)
				{
					float32x4_t vMaskc = vmulq_f32(vdupq_n_f32(mask[c]), vScale);
					int8x8_t  data = vld1_s8((AR_S8 *)(maskData + c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + downsample[0] + w*8));
					
					int32x4_t  s32SubZero0 = vsubq_s32(vmovl_s16(vget_low_s16(vmovl_s8(data))), vZerop);
					int32x4_t  s32SubZero1 = vsubq_s32(vmovl_s16(vget_high_s16(vmovl_s8(data))), vZerop);
		            f32Output0 = vaddq_f32(vmulq_f32(vcvtq_f32_s32(s32SubZero0), vMaskc), f32Output0);
					f32Output1 = vaddq_f32(vmulq_f32(vcvtq_f32_s32(s32SubZero1), vMaskc), f32Output1);
				}
																			
				f32Output0 = fast_sig_neon(f32Output0);
				f32Output1 = fast_sig_neon(f32Output1);

				for(AR_U32 i=0; i<4; i++)
				{
					if((AR_UCHAR)(f32Output0[i] * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0]+ w*8 + i)*3 + 0])
					{
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + i)*3 + 0] = (AR_UCHAR)(f32Output0[i] * 255);
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + i)*3 + 1] = cls;
					}
					if((AR_UCHAR)(f32Output1[i] * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0]+ w*8 + 4 + i)*3 + 0])
					{
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + 4 + i)*3 + 0] = (AR_UCHAR)(f32Output1[i] * 255);
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + 4 + i)*3 + 1] = cls;
					}
				}
			}
			for(AR_U32 w=downsample[0]+wLoop*8; w<downsample[2]; w++)
			{
				AR_FLOAT temp0 = 0.0f;
				for(AR_U32 c=0; c<maskTensor.u32OriChannels; c++)
				{
					temp0 += (maskData[c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + w] - zeropoint) *dscale* mask[c];
					c++;
				}
				AR_FLOAT sigout = sigmoid_op_fast(temp0);
				if((AR_UCHAR)(sigout * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0])
				{
					((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0] = (AR_UCHAR)(sigout * 255);
					((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 1] = cls;
				}
			}
		}
	}
	else if(maskTensor.u32Precision == 16)
	{
		AR_S16* maskData = (AR_S16*)dataPtr;
		for(AR_U32 h=downsample[1]; h<downsample[3]; h++)
		{
			AR_U32 wLoop = (downsample[2] - downsample[0]) / 8;
			for(AR_U32 w=0; w<wLoop; w++)
			{
				for(AR_U32 c=0; c<maskTensor.u32OriChannels; c++)
				{
					float32x4_t vMaskc = vmulq_f32(vdupq_n_f32(mask[c]), vScale);
					int16x8_t  data = vld1q_s16((AR_S16 *)(maskData + c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + downsample[0] + w*8));
					
					int32x4_t  s32SubZero0 = vsubq_s32(vmovl_s16(vget_low_s16(data)), vZerop);
					int32x4_t  s32SubZero1 = vsubq_s32(vmovl_s16(vget_high_s16(data)), vZerop);
		            f32Output0 = vaddq_f32(vmulq_f32(vcvtq_f32_s32(s32SubZero0), vMaskc), f32Output0);
					f32Output1 = vaddq_f32(vmulq_f32(vcvtq_f32_s32(s32SubZero1), vMaskc), f32Output1);
				}
																			
				f32Output0 = fast_sig_neon(f32Output0);
				f32Output1 = fast_sig_neon(f32Output1);

				for(AR_U32 i=0; i<4; i++)
				{
					if((AR_UCHAR)(f32Output0[i] * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0]+ w*8 + i)*3 + 0])
					{
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + i)*3 + 0] = (AR_UCHAR)(f32Output0[i] * 255);
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + i)*3 + 1] = cls;
					}
					if((AR_UCHAR)(f32Output1[i] * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0]+ w*8 + 4 + i)*3 + 0])
					{
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + 4 + i)*3 + 0] = (AR_UCHAR)(f32Output1[i] * 255);
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*8 + 4 + i)*3 + 1] = cls;
					}
				}
			}
			for(AR_U32 w=downsample[0]+wLoop*8; w<downsample[2]; w++)
			{
				AR_FLOAT temp0 = 0.0f;
				for(int c=0; c<maskTensor.u32OriChannels; c++)
				{
					temp0 += (maskData[c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + w] - zeropoint) *dscale* mask[c];
					c++;
				}
				AR_FLOAT sigout = sigmoid_op_fast(temp0);
				if((AR_UCHAR)(sigout * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0])
				{
					((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0] = (AR_UCHAR)(sigout * 255);
					((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 1] = cls;
				}
			}
		}
	}
	else if(maskTensor.u32Precision == 32)
	{
		AR_FLOAT* maskData = (AR_FLOAT*)dataPtr;
		for(AR_U32 h=downsample[1]; h<downsample[3]; h++)
		{
			AR_U32 wLoop = (downsample[2] - downsample[0]) / 4;
			for(AR_U32 w=0; w<wLoop; w++)
			{
				for(AR_U32 c=0; c<maskTensor.u32OriChannels; c++)
				{
					float32x4_t vMaskc = vdupq_n_f32(mask[c]);
					float32x4_t  data  = vld1q_f32((AR_FLOAT *)(maskData + c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + downsample[0] + w*4));
					
		            f32Output0 = vaddq_f32(vmulq_f32(data, vMaskc), f32Output0);
				}													
				f32Output0 = fast_sig_neon(f32Output0);

				for(AR_U32 i=0; i<4; i++)
				{
					if((AR_UCHAR)(f32Output0[i] * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0]+ w*4 + i)*3 + 0])
					{
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*4 + i)*3 + 0] = (AR_UCHAR)(f32Output0[i] * 255);
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + (downsample[0] + w*4 + i)*3 + 1] = cls;
					}
				}
			}
			for(AR_U32 w=downsample[0]+wLoop*4; w<downsample[2]; w++)
			{
				AR_FLOAT temp0 = 0.0f;
				for(AR_U32 c=0; c<maskTensor.u32OriChannels; c++)
				{
					temp0 += (maskData[c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + w] - zeropoint) *dscale* mask[c];
					c++;
				}
				AR_FLOAT sigout = sigmoid_op_fast(temp0);
				if((AR_UCHAR)(sigout * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0])
				{
					((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0] = (AR_UCHAR)(sigout * 255);
					((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 1] = cls;
				}
			}
		}		
	}
	return 0;
}


template <typename T>
AR_U32 process_mask(T* npuOut, candidate_Result *detResult, yolov8_Param& yoloParam, AR_U32 objNum, AR_NPU_TENSOR_S& maskTensor, AR_MEM_S& argbBuff)
{
	AR_S32 ret;
	AR_MEM_S sMask, bMask;
	sMask.u64Len = maskTensor.u32Height * maskTensor.u32Width * 3;
	bMask.u64Len = yoloParam.mask_h* yoloParam.mask_w * 3;
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"sMask", &sMask);
	ret = AR_MPI_NPU_MallocBuff((AR_CHAR*)"bMask", &bMask);
	
	memset((void*)sMask.u64VirtAddr, 0, sMask.u64Len);
	memset((void*)bMask.u64VirtAddr, 0, bMask.u64Len);

	AR_DOUBLE dscale = maskTensor.dScaleFactor;
	T zeropoint 	 = maskTensor.s32ZeroPoint;
	T *maskData = (T *)((AR_CHAR*)npuOut + maskTensor.u32Bank*0x2000000 + maskTensor.u32Offset);

	if(sizeof(T) == 4)
	{
		zeropoint = 0;
		dscale = 1;
	}
	
	AR_U32 downsample[4] = {0};
	for(AR_U32 n=0; n<objNum; n++)
	{
		downsample[0] = (AR_U32)(detResult[n].x_min / 4);
		downsample[1] = (AR_U32)(detResult[n].y_min / 4);
		downsample[2] = (AR_U32)(detResult[n].x_max / 4);
		downsample[3] = (AR_U32)(detResult[n].y_max / 4);
		if(!yoloParam.useNeon)
		{
			for(AR_U32 h=downsample[1]; h<downsample[3]; h++)
			{
				for(AR_U32 w=downsample[0]; w<downsample[2]; w++)
				{
					AR_FLOAT temp0 = 0.0f;
					for(AR_U32 c=0; c<maskTensor.u32OriChannels; )
					{
					#ifdef NCHW_OUTPUT
						temp0 += (maskData[c*maskTensor.u32Height*maskTensor.u32Width + h*maskTensor.u32Width + w] - zeropoint) *dscale* detResult[n].mask[c];
						c++;
					#else	
						AR_U32 blockIndex  = getIndex(h, w, c, sizeof(T), &maskTensor);
						AR_U32 blockChnNum = maskTensor.u32KSizeNorm;

						if(c >= maskTensor.u32KNormNum*maskTensor.u32KSizeNorm)
							blockChnNum = maskTensor.u32KSizeLast;

						for(AR_U32 i=0; i<blockChnNum; i++)
						{
							temp0 += (maskData[blockIndex + i] - zeropoint) *dscale* detResult[n].mask[c + i];
						}
						c += blockChnNum;
					#endif
					}
					AR_FLOAT sigout = sigmoid_op_fast(temp0);
					if((AR_UCHAR)(sigout * 255) > ((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0])
					{
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 0] = (AR_UCHAR)(sigout * 255);
						((AR_UCHAR *)sMask.u64VirtAddr)[h*maskTensor.u32Width*3 + w*3 + 1] = detResult[n].u32Cls;
					}
				}
			}
		}
		else
		{
			create_mask_neon((AR_CHAR*)maskData, downsample, dscale, zeropoint, maskTensor, sMask, detResult[n].mask, detResult[n].u32Cls);
		}
	}
	

	AR_IMG_FORMAT_E FrameFmt = AR_IMG_RGB_INTLV;		
	AR_IMG_S SigImg;	
	AR_IMG_S SigOutImg;	
	SigImg.u32FrameId = 1;
	SigImg.enFormat = FrameFmt;
	SigImg.u32Width = maskTensor.u32Width;			//160x160
	SigImg.u32Height = maskTensor.u32Height;
	SigImg.u32ChannelNum = 1;
	SigImg.astChannels[0].u32Stride = maskTensor.u32Width*3;
	SigImg.astChannels[0].u32AddrPhy = (AR_U32)sMask.u64PhyAddr;

	SigOutImg.u32FrameId = 1;
	SigOutImg.enFormat = FrameFmt;
	SigOutImg.u32Width = yoloParam.mask_w;			//640x640 or mask_h*mask_w
	SigOutImg.u32Height = yoloParam.mask_h;
	SigOutImg.u32ChannelNum = 1;
	SigOutImg.astChannels[0].u32Stride = yoloParam.mask_w*3;
	SigOutImg.astChannels[0].u32AddrPhy = (AR_U32)bMask.u64PhyAddr;

	AR_HAL_SCALER_CROP_S stCrop;
	stCrop.u32X = (int)((yoloParam.net_w-yoloParam.mask_w)*maskTensor.u32Width/(2*yoloParam.net_w));
	stCrop.u32Y = (int)((yoloParam.net_h-yoloParam.mask_h)*maskTensor.u32Height/(2*yoloParam.net_h));
	stCrop.u32W = maskTensor.u32Width - 2*stCrop.u32X;
	stCrop.u32H = maskTensor.u32Height - 2*stCrop.u32Y;

	ret = AR_MPI_SCALER_CropResizeRatio(&SigImg, &stCrop, &SigOutImg, 1, SCALER_MODE_BILINEAR, AR_IMG_ALIGN_TO_MID);
	if(ret < 0)
	{
		printf("AR_MPI_SCALER_CropResizeRatio error: %d.\n", ret);
		return -1;
	}

	for(AR_U32 n=0; n<objNum; n++)
	{
		downsample[0] = AR_U32(detResult[n].x_min - (yoloParam.net_w-yoloParam.mask_w)/2);	
		downsample[1] = AR_U32(detResult[n].y_min - (yoloParam.net_h-yoloParam.mask_h)/2);
		downsample[2] = AR_U32(detResult[n].x_max - (yoloParam.net_w-yoloParam.mask_w)/2);
		downsample[3] = AR_U32(detResult[n].y_max - (yoloParam.net_h-yoloParam.mask_h)/2);

		AR_U32 u32NumInReg = 128 / 8;
		AR_U32 u32WLoop = (downsample[2] - downsample[0]) / u32NumInReg;

		uint8x16x3_t vIn0;
		uint8x16x4_t vRGBA;
		uint8x16_t vMaskCol, vMaskCls, vMask;
		
		vRGBA.val[0] = vdupq_n_u8(yoloParam.color.r[detResult[n].u32Cls % 21]);
		vRGBA.val[1] = vdupq_n_u8(yoloParam.color.g[detResult[n].u32Cls % 21]);
		vRGBA.val[2] = vdupq_n_u8(yoloParam.color.b[detResult[n].u32Cls % 21]);
		vRGBA.val[3] = vdupq_n_u8(255);
		
		for(AR_U32 h=downsample[1]; h<downsample[3]; h++)
		{
			for(AR_U32 w=downsample[0]; w<downsample[0]+u32WLoop*u32NumInReg; w+=u32NumInReg)
			{
				vIn0 	 = vld3q_u8((AR_UCHAR*)bMask.u64VirtAddr + (h*yoloParam.mask_w + w)*3);
				vMaskCol = vshrq_n_u8(vcgtq_u8(vIn0.val[0], vdupq_n_u8(127)), 7);
				vMaskCls = vshrq_n_u8(vceqq_u8(vIn0.val[1], vdupq_n_u8(detResult[n].u32Cls)), 7);
				vMask    = vmulq_u8(vMaskCls, vMaskCol);

				if(vaddvq_u8(vMask) == u32NumInReg)
				{
					vst4q_u8((uint8_t *)(argbBuff.u64VirtAddr+h*yoloParam.mask_w*4 + w*4), vRGBA);
				}
				else if(vaddvq_u8(vMask) == 0)
					continue;
				else
				{
					for(AR_U32 i=0; i<u32NumInReg; i++)
					{
						if(vMask[i] == 0)
							continue;
						((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + (w+i)*4 + 0] = AR_UCHAR(yoloParam.color.r[detResult[n].u32Cls % 21]);
						((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + (w+i)*4 + 1] = AR_UCHAR(yoloParam.color.g[detResult[n].u32Cls % 21]);
						((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + (w+i)*4 + 2] = AR_UCHAR(yoloParam.color.b[detResult[n].u32Cls % 21]);
						((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + (w+i)*4 + 3] = AR_UCHAR(255);

					}
				}
			}
			
			for(AR_U32 w=downsample[0]+u32WLoop*u32NumInReg; w<downsample[2]; w++)			
			{
				if(((AR_UCHAR*)bMask.u64VirtAddr)[h*yoloParam.mask_w*3 + w*3 + 0] > 127 && ((AR_UCHAR*)bMask.u64VirtAddr)[h*yoloParam.mask_w*3 + w*3 + 1]==detResult[n].u32Cls)
				{
					((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + w*4 + 0] = AR_UCHAR(yoloParam.color.r[detResult[n].u32Cls % 21]);
					((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + w*4 + 1] = AR_UCHAR(yoloParam.color.g[detResult[n].u32Cls % 21]);
					((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + w*4 + 2] = AR_UCHAR(yoloParam.color.b[detResult[n].u32Cls % 21]);
					((AR_UCHAR*)argbBuff.u64VirtAddr)[h*yoloParam.mask_w*4 + w*4 + 3] = AR_UCHAR(255);
				}
			}
		}
	}

	AR_MPI_NPU_FreeBuff(&sMask);
	AR_MPI_NPU_FreeBuff(&bMask);
	return 0;
}


AR_U32 find_HW_maxConf_neon(AR_CHAR *npuOutBuff, AR_NPU_TENSOR_S tensor, yolov8_Param param, candidate_Result *canResult, AR_U32 idx, AR_U32 numCan)
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
						canResult[numCan].conf = v0[i];
						canResult[numCan].u32H = h;
						canResult[numCan].u32W = w*u32NumInReg + i;
						canResult[numCan].tensorIdx = idx;
						canResult[numCan].u32Cls = vidx0[i];
						numCan++;
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
						canResult[numCan].conf = tempValue;
						canResult[numCan].u32H = h;
						canResult[numCan].u32W = w;
						canResult[numCan].tensorIdx = idx;
						canResult[numCan].u32Cls = u32Cls;
						numCan++;
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
		
		AR_S32 iThres 	   = param.conf_thres / dscale + s32zeropoint;
		AR_S16 iInitValue  = initValue / dscale + s32zeropoint;
		AR_S16 iTempValue  = 0;
		
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
						canResult[numCan].conf = (v0[i] - s32zeropoint) * dscale;
						canResult[numCan].u32H = h;
						canResult[numCan].u32W = w*u32NumInReg + i;
						canResult[numCan].tensorIdx = idx;
						canResult[numCan].u32Cls = vidx0[i];
						numCan++;
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
						canResult[numCan].conf = (iTempValue - s32zeropoint) * dscale;
						canResult[numCan].u32H = h;
						canResult[numCan].u32W = w;
						canResult[numCan].tensorIdx = idx;
						canResult[numCan].u32Cls = u32Cls;
						numCan++;
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

		AR_S32 iThres    = param.conf_thres/dscale + s32zeropoint;
		AR_S8 iInitValue = initValue/dscale + s32zeropoint;
		AR_S8 iTempValue = 0;
		
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
						canResult[numCan].conf = (v0[i] - s32zeropoint) * dscale;
						canResult[numCan].u32H = h;
						canResult[numCan].u32W = w*u32NumInReg + i;
						canResult[numCan].tensorIdx = idx;
						canResult[numCan].u32Cls = vidx0[i];
						numCan++;
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
						canResult[numCan].conf = (iTempValue - s32zeropoint) * dscale;
						canResult[numCan].u32H = h;
						canResult[numCan].u32W = w;
						canResult[numCan].tensorIdx = idx;
						canResult[numCan].u32Cls = u32Cls;
						numCan++;
					}
				}
			}
		}
	}
	
	return numCan;
}


template <typename T>
AR_U32 find_HW_maxConf(T *npuOutBuff, AR_NPU_TENSOR_S& tensor, yolov8_Param& param, candidate_Result *canResult, AR_U32 idx, AR_U32 numCan)
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

	T initValue 	= init / dscale + s32zeropoint;
	T *confData		= (T *)((AR_CHAR*)npuOutBuff + tensor.u32Bank*0x2000000 + tensor.u32Offset);
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
				canResult[numCan].conf = (tempValue - s32zeropoint) * dscale;
				canResult[numCan].u32H = h;
				canResult[numCan].u32W = w;
				canResult[numCan].tensorIdx = idx;
				canResult[numCan].u32Cls = u32Cls;
				numCan++;
			}
		}
	}

	return numCan;
}


/*
int 8, float output format, o3 compile
detect the object time cost:
find_HW_maxConf_neon only support output as NCHW format: timecost 11ms
find_HW_maxConf support: NCHW cost 28ms; Blocking NHWC cost 8ms
*/
AR_U32 yolov8_seg_process(void *handle, AR_CHAR *npuOutBuff, yolov8_Param& yoloParam, candidate_Result *postResult, AR_U32 *tensorsId, AR_MEM_S& argbBuff)
{
	AR_U32 numCan 	= 0;
	AR_U32 objNum 	= 0;
	AR_NPU_TENSOR_S stTensor = {0};
	candidate_Result canResult[MAX_NUM_CAN];
	memset((void*)argbBuff.u64VirtAddr, 0, argbBuff.u64Len);
	
    for(AR_U32 i=0; i <3; i++)
    {		
		AR_MPI_NPU_GetOutputTensorParamById(handle, tensorsId[i], &stTensor);
		if(yoloParam.useNeon)
			numCan = find_HW_maxConf_neon((AR_CHAR*)npuOutBuff, stTensor, yoloParam, canResult, i, numCan);
		else
		{
			if(stTensor.u32Precision == 32)
				numCan = find_HW_maxConf((AR_FLOAT*)npuOutBuff, stTensor, yoloParam, canResult, i, numCan);
			else if(stTensor.u32Precision == 16)
				numCan = find_HW_maxConf((AR_S16*)npuOutBuff, stTensor, yoloParam, canResult, i, numCan);
			else
				numCan = find_HW_maxConf((AR_S8*)npuOutBuff, stTensor, yoloParam, canResult, i, numCan);
		}
    }
	
	if(numCan != 0)
	{
		for(AR_U32 i=0; i<3; i++)
		{
			AR_MPI_NPU_GetOutputTensorParamById(handle, tensorsId[i+3], &stTensor);
			
			if(stTensor.u32Precision == 32)
				find_HW_loc(canResult, (AR_FLOAT*)npuOutBuff, stTensor, numCan, i);
			else if(stTensor.u32Precision == 16)
				find_HW_loc(canResult, (AR_S16*)npuOutBuff, stTensor, numCan, i);
			else
				find_HW_loc(canResult, (AR_S8*)npuOutBuff, stTensor, numCan, i);
		}
		
		xywh2xyxy(canResult, numCan, yoloParam);
		
		objNum = nms_proc(canResult, numCan, postResult, yoloParam);
	}

	if(objNum != 0)
	{
		for(AR_U32 i=0; i<3; i++)
		{
			AR_MPI_NPU_GetOutputTensorParamById(handle, tensorsId[i+6], &stTensor);
			
			if(stTensor.u32Precision == 32)
				find_HW_mask(postResult, (AR_FLOAT*)npuOutBuff, stTensor, objNum, i);
			else if(stTensor.u32Precision == 16)
				find_HW_mask(postResult, (AR_S16*)npuOutBuff, stTensor, objNum, i);
			else
				find_HW_mask(postResult, (AR_S8*)npuOutBuff, stTensor, numCan, i);
		}
		
		AR_MPI_NPU_GetOutputTensorParamById(handle, tensorsId[9], &stTensor);
			
		if(stTensor.u32Precision == 32)
			process_mask((AR_FLOAT*)npuOutBuff, postResult, yoloParam, objNum, stTensor, argbBuff);
		else if(stTensor.u32Precision == 16)
			process_mask((AR_S16*)npuOutBuff, postResult, yoloParam, objNum, stTensor, argbBuff);
		else if(stTensor.u32Precision == 8)
			process_mask((AR_S8*)npuOutBuff, postResult, yoloParam, objNum, stTensor, argbBuff);		
	}

	return objNum;
}


