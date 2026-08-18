#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <arm_neon.h>

#include "hal_npu_types.h"
#include "ar_cnn_op_common.h"
#include "ar_cnn_channelcvt.h"

#define NHWC_TO_NCHW_NEON

void AR_NPU_FixCvtFloat(AR_CHAR* pcInput, AR_FLOAT* pfOutput, AR_U32 u32DataSize, AR_NPU_TENSOR_S * pstTensor)
{
    int data_size = u32DataSize;
    float scale_factor = pstTensor->dScaleFactor;
    int zero_point = pstTensor->s32ZeroPoint;
    int precision = pstTensor->u32Precision;
    
    for (int ind = 0; ind < data_size; ++ind)
    {
        if (precision == 16)
        {
            signed short * input = (signed short *)pcInput;
            pfOutput[ind] = (input[ind] - zero_point) * scale_factor;
        }
        else
        {
            signed char * input = (signed char *)pcInput;
            pfOutput[ind] = (input[ind] - zero_point) * scale_factor;
        }
    }
}

void AR_ConvertDump_YUV420PImg(AR_IMG_S *SrcImg, AR_CHAR *DstBuf)
{
	AR_U32 h = 0;
	AR_U32 img_h = SrcImg->u32Height;
	AR_U32 img_w = SrcImg->u32Width;
	AR_IMG_FORMAT_E imgFormat = SrcImg->enFormat;
	if(imgFormat != AR_IMG_YUV420P){
		printf("ERROR: The current interface only supports YUV420P!\n");
		return;
	}
	AR_U32 YStride = SrcImg->astChannels[0].u32Stride;
	AR_U32 UStride = SrcImg->astChannels[1].u32Stride;
	AR_U32 VStride = SrcImg->astChannels[2].u32Stride;
	AR_UINTPTR YAddr = SrcImg->astChannels[0].uptrAddrVirt;
	AR_UINTPTR UAddr = SrcImg->astChannels[1].uptrAddrVirt;
	AR_UINTPTR VAddr = SrcImg->astChannels[2].uptrAddrVirt;
	for(h = 0; h < img_h; h++){
		memcpy((void *)(DstBuf + h*img_w), (void *)(YAddr + h*YStride), img_w);
		if(h < img_h / 2){
			memcpy((void *)(DstBuf + img_h*img_w + h*img_w/2), (void *)(UAddr + h*UStride), img_w / 2);
			memcpy((void *)(DstBuf + img_h*img_w + img_h*img_w/4 + h*img_w/2), (void *)(VAddr + h*VStride), img_w / 2);
		}
	}
}

void AR_NPU_FloatCvtFix(AR_FLOAT* pfInput, AR_CHAR* pcOutput, AR_U32 u32DataSize, AR_NPU_TENSOR_S * pstTensor)
{
    int data_size = u32DataSize;
    float scale_factor = 1.0 / pstTensor->dScaleFactor;
    int precision = pstTensor->u32Precision;

    if(precision == 16)
    {
        short * quant = (short *)pcOutput;
		int tmp = 0;
		int zero_point = pstTensor->s32ZeroPoint;
        for (int i = 0; i < data_size; ++i)
        {
            tmp = (int)round(pfInput[i] * scale_factor) + zero_point;
            tmp = MIN(tmp, 32767);
            tmp = MAX(tmp, -32768);
            quant[i] = (short)tmp;
        }
    }
    else
    {
		short tmp = 0;
		short zero_point = (short)pstTensor->s32ZeroPoint;
		for (int i = 0; i < data_size; ++i)
        {
            tmp = (short)round(pfInput[i] * scale_factor) + zero_point;
            tmp = MIN(tmp, 127);
            tmp = MAX(tmp, -128);
            pcOutput[i] = (char)tmp;
        }
    }
}

void AR_NPU_Exe_NhwcCvtNchw_Fix(AR_CHAR * pInput, AR_CHAR * pOutput, AR_NPU_TENSOR_S * pTensor)
{
    if((!pInput) || (!pOutput) ||(!pTensor))
    {
        printf("AR_NPU_Exe_NhwcCvtNchw_Fix with err param\r\n");
		return;
	}
		
	int c = 0, w = 0, h = 0, k = 0;
	unsigned int width = pTensor->u32Width;
	unsigned int height = pTensor->u32Height;
	unsigned int k_size_norm = pTensor->u32KSizeNorm;
	unsigned int k_norm_num = pTensor->u32KNormNum;
	unsigned int k_step = pTensor->u32KStep;
	unsigned int row_ddr_step = pTensor->u32RowStep;
	unsigned int precision = pTensor->u32Precision;
	//unsigned int pixel_byte = precision / 8;
	unsigned int k_size_last = pTensor->u32KSizeLast;
       unsigned int k_size_last_ori = pTensor->u32OriChannels - k_size_norm * k_norm_num;
	unsigned int batch = pTensor->u32Num;
	//int channels_norm = k_size_norm * k_norm_num;
	//int channels_total = channels_norm + k_size_last;
	//unsigned int hwc_tensor_step = pTensor->u32TensorStep;
	//unsigned int chw_tensor_step = width * height * channels_total * pixel_byte;
       AR_CHAR * hwc = pInput;
       AR_CHAR * chw = pOutput;
	//the norm block

	if(precision == 16)
	{
	    AR_S16 * dst = (AR_S16 *)chw;
        AR_S16 * src = (AR_S16 *)hwc;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    src = (AR_S16 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                               {
                                   *dst++ = *(src + w * k_size_norm + c);
                               }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last_ori; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			       src = (AR_S16 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
          			for (w = 0; w < width; w++)
          			{
          				*dst++ = *(src + w * k_size_last + c);
          			}
			}
		}
	}
    else
    {
        AR_S8 * dst = (AR_S8 *)chw;
        AR_S8 * src = (AR_S8 *)hwc;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    src = (AR_S8 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                               {
                                   *dst++ = *(src + w * k_size_norm + c);
                               }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last_ori; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			       src = (AR_S8 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*dst++ = *(src + w * k_size_last + c);
				}
			}
		}
    }

}

void AR_NPU_Exe_NchwCvtNhwc_Fix(AR_CHAR * pInput, AR_CHAR * pOutput, AR_NPU_TENSOR_S * pTensor) 
{
    if((!pInput) || (!pOutput) ||(!pTensor))
    {
        printf("AR_NPU_Exe_NchwCvtNhwc_Fix with err param\r\n");
		return;
	}
    int c = 0, w = 0, h = 0, k = 0;
    unsigned int width = pTensor->u32Width;
    unsigned int height = pTensor->u32Height;
    //unsigned int output_off = pCBParams->astOutputTensor[0].u32Offset;
    unsigned int k_size_norm = pTensor->u32KSizeNorm;
    unsigned int k_step = pTensor->u32KStep;
    unsigned int row_ddr_step = pTensor->u32RowStep;
    unsigned int precision = pTensor->u32Precision;
    //unsigned int pixel_byte = precision / 8;
    unsigned int k_size_last = pTensor->u32KSizeLast;
    unsigned int k_norm_num = pTensor->u32KNormNum;
        unsigned int k_size_last_ori = pTensor->u32OriChannels - k_size_norm * k_norm_num;
    unsigned int batch = pTensor->u32Num;
    //int channels_norm = k_size_norm * k_norm_num;
    //int channels_total = channels_norm + k_size_last;
    //unsigned int hwc_tensor_step = pTensor->u32TensorStep;
    //unsigned int chw_tensor_step = width * height * channels_total * pixel_byte;
    AR_CHAR * hwc = pOutput;
    AR_CHAR * chw = pInput;
    if(precision == 16)
	{
	    AR_S16 * dst = (AR_S16 *)hwc;
        AR_S16 * src = (AR_S16 *)chw;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    dst = (AR_S16 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                    {
                        *(dst + w * k_size_norm + c) = *src++;
                    }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last_ori; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			    dst = (AR_S16 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*(dst + w * k_size_last + c) = *src++;
				}
			}
		}
	}
    else
    {
        AR_S8 * dst = (AR_S8 *)hwc;
        AR_S8 * src = (AR_S8 *)chw;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    dst = (AR_S8 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                    {
                        *(dst + w * k_size_norm + c) = *src++;
                    }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last_ori; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			    dst = (AR_S8 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*(dst + w * k_size_last + c) = *src++;
				}
			}
		}
    }

}

#ifndef NHWC_TO_NCHW_NEON
static AR_U32 AR_NPU_GetEntryIndex(AR_U32 h, AR_U32 w, AR_U32 c, AR_U32 byteUnit, AR_NPU_TENSOR_S * pTensor)
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

#endif

//for nchw tensor float cvt to fix with c code
void AR_NPU_FloatCvtFix_With_Stride(AR_FLOAT* pfInput, AR_CHAR* pcOutput, AR_NPU_TENSOR_S * pstTensor)
{
    int width = pstTensor->u32Width;
    int height = pstTensor->u32Height;
    int batch = pstTensor->u32Num;
    int channel = pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast;
	int row_step = pstTensor->u32RowStep;
	int k_step = pstTensor->u32KStep;
    double scale_factor = pstTensor->dScaleFactor;
    int zero_point = pstTensor->s32ZeroPoint;
    int precision = pstTensor->u32Precision;
    int positive_limit = (int)pow((double)2, (double)(precision - 1)) - 1;
    int negative_limit = (int)-pow((double)2, (double)(precision - 1));
    int tmp = 0;

	int k_step_in = width * height * batch;
	int k_step_out = k_step;
	int h_step_in = width;
	int h_step_out = row_step;
    if(precision == 16)
    {
        short * output = (short *)pcOutput;
		float * input = (float *)pfInput;

		for(int k = 0; k < channel; k++)
    	{
			for(int j = 0; j < batch * height; j++)
			{
				input = (float *)(pfInput + k * k_step_in + j * h_step_in);
				output = (short *)(pcOutput + k * k_step_out +j * h_step_out);
				
		        for (int i = 0; i < width; ++i)
		        {
		            tmp = (int)round(input[i] / scale_factor) + zero_point;
		            tmp = MIN(tmp, positive_limit);
		            tmp = MAX(tmp, negative_limit);
		            output[i] = (short)tmp;
		        }
			}
		}
    }
    else
    {
        char * output = (char *)pcOutput;
		float * input = (float *)pfInput;

		for(int k = 0; k < channel; k++)
    	{
			for(int j = 0; j < batch * height; j++)
			{
				input = (float *)(pfInput + k * k_step_in + j * h_step_in);
				output = (char *)(pcOutput + k * k_step_out +j * h_step_out);
				
		        for (int i = 0; i < width; ++i)
		        {
		            tmp = (int)round(input[i] / scale_factor) + zero_point;
		            tmp = MIN(tmp, positive_limit);
		            tmp = MAX(tmp, negative_limit);
		            output[i] = (short)tmp;
		        }
			}
		}
    }
}


#ifdef NHWC_TO_NCHW_NEON
//for nchw tensor float cvt to fix with neon code

//for arm arch32
#if 0
void AR_NPU_FloatCvtFix_With_Stride_Neon(AR_FLOAT * pfInput, AR_CHAR* pcOutput, AR_NPU_TENSOR_S * pstTensor)
{
    AR_U32 u32Width = pstTensor->u32Width;
    AR_U32 u32Height = pstTensor->u32Height;
    AR_U32 u32Channel = pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast;
    AR_FLOAT dScaleFactor = pstTensor->dScaleFactor;
    AR_S32 s32ZeroPoint = pstTensor->s32ZeroPoint;
    AR_U32 u32Precision = pstTensor->u32Precision;
	AR_U32 u32Batch = pstTensor->u32Num;
    AR_U32 u32RowStep = pstTensor->u32RowStep;
    AR_U32 u32KStep = pstTensor->u32KStep;

    AR_S32 positive_limit = (AR_S32)pow((AR_FLOAT)2, (AR_FLOAT)(u32Precision - 1)) - 1;
    AR_S32 negative_limit = (AR_S32)-pow((AR_FLOAT)2, (AR_FLOAT)(u32Precision - 1));
    AR_S32 s32Tmp = 0;
	int s32ChannelStepIn = u32Width * u32Height;
	int s32ChannelStepOut = u32KStep;

    int32x4_t s32ZeroPointVec = vdupq_n_s32((int32_t)s32ZeroPoint);
    float32x4_t f32Scale = vdupq_n_f32(1.0/dScaleFactor);
    if(u32Precision == 16)
    { 
    	AR_FLOAT *f32Input = (AR_FLOAT *)pfInput;
        AR_S16 *s16Output = (AR_S16 *)pcOutput;
        AR_S32 s32WidthLoop = u32Width/8;
        AR_S32 s32WidthLeft = u32Width%8;
		AR_S32 s32HeightLoop = u32Batch * u32Height/2;
        AR_S32 s32HeightLeft = u32Batch * u32Height%2;
		AR_S32 s32HeightStepIn = 2 * u32Width;
		AR_S32 s32HeightStepOut = 2 * u32RowStep;
	    for(int k = 0; k < u32Channel; k++)
	    {
            for(int j = 0; j < s32HeightLoop; j++)
            {
            	f32Input = pfInput + s32HeightStepIn * j + k * s32ChannelStepIn;
				s16Output = (AR_S16 *)(pcOutput + s32HeightStepOut * j + k * s32ChannelStepOut);
                for(int i = 0; i < s32WidthLoop; i++)
                {
                    float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
                    
                    float32x4_t f32Load0 = vld1q_f32((float32_t *)f32Input);
                    float32x4_t f32Load1 = vld1q_f32((float32_t *)f32Input + 4);
                    float32x4_t f32Load2 = vld1q_f32((float32_t *)f32Input + u32Width);
                    float32x4_t f32Load3 = vld1q_f32((float32_t *)f32Input + u32Width + 4);
                    
                    float32x4_t f32ScaleInVec0 = vmulq_f32(f32Load0, f32Scale);
                    int32x4_t s32SignSignal0 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec0), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue0 = vaddq_f32(f32ScaleInVec0, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal0)));
                    int16x4_t s16SumValueInt0 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue0), s32ZeroPointVec));
        
                    float32x4_t f32ScaleInVec1 = vmulq_f32(f32Load1, f32Scale);
                    int32x4_t s32SignSignal1 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec1), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue1 = vaddq_f32(f32ScaleInVec1, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal1)));
                    int16x4_t s16SumValueInt1 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue1), s32ZeroPointVec));
        
                    float32x4_t f32ScaleInVec2 = vmulq_f32(f32Load2, f32Scale);
                    int32x4_t s32SignSignal2 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec2), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue2 = vaddq_f32(f32ScaleInVec2, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal2)));
                    int16x4_t s16SumValueInt2 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue2), s32ZeroPointVec));
        
                    float32x4_t f32ScaleInVec3 = vmulq_f32(f32Load3, f32Scale);
                    int32x4_t s32SignSignal3 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec3), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue3 = vaddq_f32(f32ScaleInVec3, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal3)));
                    int16x4_t s16SumValueInt3 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue3), s32ZeroPointVec));
        
                    int16x8_t s16Combine0 = vcombine_s16(s16SumValueInt0, s16SumValueInt1);
                    int16x8_t s16Combine1 = vcombine_s16(s16SumValueInt2, s16SumValueInt3);
                    vst1q_s16((int16_t *)s16Output, s16Combine0);
                    vst1q_s16((int16_t *)s16Output + u32RowStep/2, s16Combine1);
        
                    s16Output += 8;
                    f32Input += 8;
                }

				for(int p = 0; p < 2; p++)
				{
                    for(int i = 0; i < s32WidthLeft; i++)
                    {
                        s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                        s32Tmp = MIN(s32Tmp, positive_limit);
                        s32Tmp = MAX(s32Tmp, negative_limit);
                        s16Output[i] = (short)s32Tmp;
                    }

					f32Input += u32Width;
					s16Output += u32RowStep/2;
				}
            }

			f32Input = pfInput + s32HeightStepIn * s32HeightLoop + k * s32ChannelStepIn;
			s16Output = (AR_S16 *)(pcOutput + s32HeightStepOut * s32HeightLoop +  k * s32ChannelStepOut);

			for(int j = 0; j < s32HeightLeft; j++)
			{
				for(int i = 0; i < u32Width; i++)
				{
					s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                    s32Tmp = MIN(s32Tmp, positive_limit);
                    s32Tmp = MAX(s32Tmp, negative_limit);
                    s16Output[i] = (short)s32Tmp;
				}

				f32Input += u32Width;
				s16Output += u32RowStep/2;
			}
	    }
    }
    else if(u32Precision == 8)
    {
        AR_FLOAT * f32Input = (AR_FLOAT *)pfInput;
        AR_CHAR * s8Output = (AR_CHAR *)pcOutput;
        AR_S32 s32WidthLoop = u32Width/8;
        AR_S32 s32WidthLeft = u32Width%8;
		AR_S32 s32HeightLoop = u32Batch * u32Height/2;
        AR_S32 s32HeightLeft = u32Batch * u32Height%2;
		AR_S32 s32HeightStepIn = 2 * u32Width;
		AR_S32 s32HeightStepOut = 2 * u32RowStep;

	    for(int k = 0; k < u32Channel; k++)
	    {
			for(int j =0; j < s32HeightLoop; j++)
			{
				f32Input = pfInput + s32HeightStepIn * j + k * s32ChannelStepIn;
				s8Output = pcOutput + s32HeightStepOut * j + k * s32ChannelStepOut;
                for(int i = 0; i < s32WidthLoop; i++)
                {
                    float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
        
                    float32x4_t f32Load0 = vld1q_f32((float32_t *)f32Input);
                    float32x4_t f32Load1 = vld1q_f32((float32_t *)f32Input + 4);
                    float32x4_t f32Load2 = vld1q_f32((float32_t *)f32Input + u32Width);
                    float32x4_t f32Load3 = vld1q_f32((float32_t *)f32Input + u32Width + 4);
                    
                    float32x4_t f32ScaleInVec0 = vmulq_f32(f32Load0, f32Scale);
                    int32x4_t s32SignSignal0 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec0), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue0 = vaddq_f32(f32ScaleInVec0, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal0)));
                    int16x4_t s16SumValueInt0 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue0), s32ZeroPointVec));
        
                   
                    float32x4_t f32ScaleInVec1 = vmulq_f32(f32Load1, f32Scale);
                    int32x4_t s32SignSignal1 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec1), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue1 = vaddq_f32(f32ScaleInVec1, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal1)));
                    int16x4_t s16SumValueInt1 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue1), s32ZeroPointVec));
        
                    float32x4_t f32ScaleInVec2 = vmulq_f32(f32Load2, f32Scale);
                    int32x4_t s32SignSignal2 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec2), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue2 = vaddq_f32(f32ScaleInVec2, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal2)));
                    int16x4_t s16SumValueInt2 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue2), s32ZeroPointVec));
        
                    
                    float32x4_t f32ScaleInVec3 = vmulq_f32(f32Load3, f32Scale);
                    int32x4_t s32SignSignal3 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec3), vdupq_n_s32(2147483648));
                    float32x4_t f32SumValue3 = vaddq_f32(f32ScaleInVec3, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal3)));
                    int16x4_t s16SumValueInt3 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue3), s32ZeroPointVec));
        
                    int8x8_t s8Out1 = vqmovn_s16(vcombine_s16(s16SumValueInt0, s16SumValueInt1));
					int8x8_t s8Out2 = vqmovn_s16(vcombine_s16(s16SumValueInt2, s16SumValueInt3));
        
                    vst1_s8((int8_t *)s8Output, s8Out1);
					vst1_s8((int8_t *)s8Output + u32RowStep, s8Out2);
        
                    s8Output += 8;
                    f32Input+= 8;
                }

				//for W left , H = 2
                for(int p = 0; p < 2; p++)
				{
                    for(int i = 0; i < s32WidthLeft; i++)
                    {
                        s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                        s32Tmp = MIN(s32Tmp, positive_limit);
                        s32Tmp = MAX(s32Tmp, negative_limit);
                        s8Output[i] = (char)s32Tmp;
                    }

					f32Input += u32Width;
					s8Output += u32RowStep;
				}
            }

 			//for H left
    		f32Input = pfInput + s32HeightStepIn * s32HeightLoop + k * s32ChannelStepIn;
    		s8Output = pcOutput + s32HeightStepOut * s32HeightLoop +  k * s32ChannelStepOut;
    
    		for(int j = 0; j < s32HeightLeft; j++)
    		{
                for(int i = 0; i < u32Width; i++)
                {
                    s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                    s32Tmp = MIN(s32Tmp, positive_limit);
                    s32Tmp = MAX(s32Tmp, negative_limit);
                    s8Output[i] = (short)s32Tmp;
                }
                f32Input += u32Width;
                s8Output += u32RowStep;
    		}
	    }	
    }
}

void AR_NPU_FloatCvtFix_Neon(AR_FLOAT * pfInput, AR_CHAR* pcOutput, AR_U32 u32DataNum, AR_NPU_TENSOR_S * pstTensor)
{
    //AR_U32 u32Width = pstTensor->u32Width;
    //AR_U32 u32Height = pstTensor->u32Height;
    //AR_U32 u32Channel = pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast;
    AR_FLOAT dScaleFactor = pstTensor->dScaleFactor;
    AR_S32 s32ZeroPoint = pstTensor->s32ZeroPoint;
    AR_U32 u32Precision = pstTensor->u32Precision;
    //AR_U32 u32Batch = pstTensor->u32Num;
    AR_U32 s32DataSize = u32DataNum;

    AR_S32 positive_limit = (AR_S32)pow((AR_FLOAT)2, (AR_FLOAT)(u32Precision - 1)) - 1;
    AR_S32 negative_limit = (AR_S32)-pow((AR_FLOAT)2, (AR_FLOAT)(u32Precision - 1));
    //AR_S32 s32Index = 0;
    AR_S32 s32Tmp = 0;

    int32x4_t s32ZeroPointVec = vdupq_n_s32((int32_t)s32ZeroPoint);
    float32x4_t f32Scale = vdupq_n_f32(1.0/dScaleFactor);
    if(u32Precision == 16)
    {
        AR_FLOAT * f32Input = (AR_FLOAT *)pfInput;
        AR_S16 * s16Output = (AR_S16 *)pcOutput;
        int loop = s32DataSize/16;
        int left = s32DataSize%16;

        for(int i = 0; i < loop; i++)
        {
            float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
            
            float32x4_t f32Load0 = vld1q_f32((float32_t *)f32Input);
            float32x4_t f32Load1 = vld1q_f32((float32_t *)f32Input + 4);
            float32x4_t f32Load2 = vld1q_f32((float32_t *)f32Input + 8);
            float32x4_t f32Load3 = vld1q_f32((float32_t *)f32Input + 12);
            
            float32x4_t f32ScaleInVec0 = vmulq_f32(f32Load0, f32Scale);
            int32x4_t s32SignSignal0 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec0), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue0 = vaddq_f32(f32ScaleInVec0, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal0)));
            int16x4_t s16SumValueInt0 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue0), s32ZeroPointVec));

            float32x4_t f32ScaleInVec1 = vmulq_f32(f32Load1, f32Scale);
            int32x4_t s32SignSignal1 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec1), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue1 = vaddq_f32(f32ScaleInVec1, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal1)));
            int16x4_t s16SumValueInt1 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue1), s32ZeroPointVec));

            float32x4_t f32ScaleInVec2 = vmulq_f32(f32Load2, f32Scale);
            int32x4_t s32SignSignal2 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec2), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue2 = vaddq_f32(f32ScaleInVec2, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal2)));
            int16x4_t s16SumValueInt2 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue2), s32ZeroPointVec));

            float32x4_t f32ScaleInVec3 = vmulq_f32(f32Load3, f32Scale);
            int32x4_t s32SignSignal3 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec3), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue3 = vaddq_f32(f32ScaleInVec3, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal3)));
            int16x4_t s16SumValueInt3 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue3), s32ZeroPointVec));

            int16x8_t s16Combine0 = vcombine_s16(s16SumValueInt0, s16SumValueInt1);
            int16x8_t s16Combine1 = vcombine_s16(s16SumValueInt2, s16SumValueInt3);
            vst1q_s16((int16_t *)s16Output, s16Combine0);
            vst1q_s16((int16_t *)s16Output + 8, s16Combine1);

            s16Output += 16;
            f32Input += 16;
        }

        for(int i = 0; i < left; i++)
        {
            s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
            s32Tmp = MIN(s32Tmp, positive_limit);
            s32Tmp = MAX(s32Tmp, negative_limit);
            s16Output[i] = (short)s32Tmp;
        }
    }
    else if(u32Precision == 8)
    {
        AR_FLOAT * f32Input = (AR_FLOAT *)pfInput;
        AR_CHAR * s8Output = (AR_CHAR *)pcOutput;
        int loop = s32DataSize/16;
        int left = s32DataSize%16;

        for(int i = 0; i < loop; i++)
        {
            float32x4_t f32RoundVal = vdupq_n_f32(0.5f);

            float32x4_t f32Load0 = vld1q_f32((float32_t *)f32Input);
            float32x4_t f32Load1 = vld1q_f32((float32_t *)f32Input + 4);
            float32x4_t f32Load2 = vld1q_f32((float32_t *)f32Input + 8);
            float32x4_t f32Load3 = vld1q_f32((float32_t *)f32Input + 12);
            
            float32x4_t f32ScaleInVec0 = vmulq_f32(f32Load0, f32Scale);
            int32x4_t s32SignSignal0 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec0), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue0 = vaddq_f32(f32ScaleInVec0, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal0)));
            int16x4_t s16SumValueInt0 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue0), s32ZeroPointVec));

           
            float32x4_t f32ScaleInVec1 = vmulq_f32(f32Load1, f32Scale);
            int32x4_t s32SignSignal1 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec1), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue1 = vaddq_f32(f32ScaleInVec1, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal1)));
            int16x4_t s16SumValueInt1 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue1), s32ZeroPointVec));

            float32x4_t f32ScaleInVec2 = vmulq_f32(f32Load2, f32Scale);
            int32x4_t s32SignSignal2 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec2), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue2 = vaddq_f32(f32ScaleInVec2, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal2)));
            int16x4_t s16SumValueInt2 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue2), s32ZeroPointVec));

            
            float32x4_t f32ScaleInVec3 = vmulq_f32(f32Load3, f32Scale);
            int32x4_t s32SignSignal3 = vandq_s32(vreinterpretq_s32_f32(f32ScaleInVec3), vdupq_n_s32(2147483648));
            float32x4_t f32SumValue3 = vaddq_f32(f32ScaleInVec3, vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal3)));
            int16x4_t s16SumValueInt3 = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(f32SumValue3), s32ZeroPointVec));

            int8x16_t s8Out = vcombine_s8(vqmovn_s16(vcombine_s16(s16SumValueInt0, s16SumValueInt1)), vqmovn_s16(vcombine_s16(s16SumValueInt2, s16SumValueInt3)));

            vst1q_s8((int8_t *)s8Output, s8Out);

            s8Output += 16;
            f32Input+= 16;
        }

        for(int i = 0; i < left; i++)
        {
            s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
            s32Tmp = MIN(s32Tmp, positive_limit);
            s32Tmp = MAX(s32Tmp, negative_limit);
            s8Output[i] = (char)s32Tmp;
        }
    }
}

void AR_NPU_FixCvtFloat_Neon(AR_CHAR* pcInput, AR_FLOAT* pfOutput, AR_U32 u32DataNum, AR_NPU_TENSOR_S* pstTensor)
{
    AR_FLOAT dScaleFactor = pstTensor->dScaleFactor;
    AR_S32 s32ZeroPoint = pstTensor->s32ZeroPoint;
    AR_U32 u32Precision = pstTensor->u32Precision;
    AR_S32 s32Ind = 0;

    float32x4_t f32Scale = vdupq_n_f32(dScaleFactor);
    int32x4_t s32ZeroPointVec= vdupq_n_s32(s32ZeroPoint);
    if(u32Precision == 16)
    {
        AR_S16 * s16Input = (AR_S16 *)pcInput;
        AR_FLOAT * f32Output = (AR_FLOAT *)pfOutput;
        
        AR_S32 s32Loop = u32DataNum/32;
        AR_S32 s32Left = u32DataNum%32;

        for(s32Ind = 0; s32Ind < s32Loop; s32Ind++)
        {
            int16x8_t s16Load0 = vld1q_s16((int16_t *)s16Input);
            int16x8_t s16Load1 = vld1q_s16((int16_t *)s16Input + 8);
            int16x8_t s16Load2 = vld1q_s16((int16_t *)s16Input + 16);
            int16x8_t s16Load3 = vld1q_s16((int16_t *)s16Input + 24);
            
            int32x4_t  s32SubZeroLow0 = vsubq_s32(vmovl_s16(vget_low_s16(s16Load0)), s32ZeroPointVec);
            float32x4_t f32OutputLow0 = vmulq_f32(vcvtq_f32_s32(s32SubZeroLow0), f32Scale);
            int32x4_t  s32SubZeroHigh0 = vsubq_s32(vmovl_s16(vget_high_s16(s16Load0)), s32ZeroPointVec);
            float32x4_t f32OutputHigh0 = vmulq_f32(vcvtq_f32_s32(s32SubZeroHigh0), f32Scale);

            int32x4_t  s32SubZeroLow1 = vsubq_s32(vmovl_s16(vget_low_s16(s16Load1)), s32ZeroPointVec);
            float32x4_t f32OutputLow1 = vmulq_f32(vcvtq_f32_s32(s32SubZeroLow1), f32Scale);
            int32x4_t  s32SubZeroHigh1 = vsubq_s32(vmovl_s16(vget_high_s16(s16Load1)), s32ZeroPointVec);
            float32x4_t f32OutputHigh1 = vmulq_f32(vcvtq_f32_s32(s32SubZeroHigh1), f32Scale);

            int32x4_t  s32SubZeroLow2 = vsubq_s32(vmovl_s16(vget_low_s16(s16Load2)), s32ZeroPointVec);
            float32x4_t f32OutputLow2 = vmulq_f32(vcvtq_f32_s32(s32SubZeroLow2), f32Scale);
            int32x4_t  s32SubZeroHigh2 = vsubq_s32(vmovl_s16(vget_high_s16(s16Load2)), s32ZeroPointVec);
            float32x4_t f32OutputHigh2 = vmulq_f32(vcvtq_f32_s32(s32SubZeroHigh2), f32Scale);

            int32x4_t  s32SubZeroLow3 = vsubq_s32(vmovl_s16(vget_low_s16(s16Load3)), s32ZeroPointVec);
            float32x4_t f32OutputLow3 = vmulq_f32(vcvtq_f32_s32(s32SubZeroLow3), f32Scale);
            int32x4_t  s32SubZeroHigh3 = vsubq_s32(vmovl_s16(vget_high_s16(s16Load3)), s32ZeroPointVec);
            float32x4_t f32OutputHigh3 = vmulq_f32(vcvtq_f32_s32(s32SubZeroHigh3), f32Scale);

            vst1q_f32((float32_t *)f32Output, f32OutputLow0);
            vst1q_f32((float32_t *)f32Output + 4, f32OutputHigh0);
            vst1q_f32((float32_t *)f32Output + 8, f32OutputLow1);
            vst1q_f32((float32_t *)f32Output + 12, f32OutputHigh1);
            vst1q_f32((float32_t *)f32Output + 16, f32OutputLow2);
            vst1q_f32((float32_t *)f32Output + 20, f32OutputHigh2);
            vst1q_f32((float32_t *)f32Output + 24, f32OutputLow3);
            vst1q_f32((float32_t *)f32Output + 28, f32OutputHigh3);

            s16Input += 32;
            f32Output += 32;
        }

        for(s32Ind = 0; s32Ind < s32Left; s32Ind++)
        {
            f32Output[s32Ind] = (s16Input[s32Ind] - s32ZeroPoint) * dScaleFactor;
        }
    }
    else if(u32Precision == 8)
    {
       AR_CHAR * s8Input = (AR_CHAR *)pcInput;
       AR_FLOAT * f32Output = (AR_FLOAT *)pfOutput;
       AR_S32 s32Loop = u32DataNum/32;
       AR_S32 s32Left = u32DataNum%32;
    
       for(s32Ind = 0; s32Ind < s32Loop; s32Ind++)
       {
           int8x16_t s8_input0 = vld1q_s8((int8_t *)s8Input);
           int8x16_t s8_input1 = vld1q_s8((int8_t *)s8Input+16);
           //first 16
           int16x8_t s16_input_low0 = vmovl_s8(vget_low_s8(s8_input0));
           int16x8_t s16_input_high0 = vmovl_s8(vget_high_s8(s8_input0));

           int32x4_t s32_cast_low_low0 = vmovl_s16(vget_low_s16(s16_input_low0));
           int32x4_t s32_cast_low_high0 = vmovl_s16(vget_high_s16(s16_input_low0));

           int32x4_t s32_cast_high_low0 = vmovl_s16(vget_low_s16(s16_input_high0));
           int32x4_t s32_cast_high_high0 = vmovl_s16(vget_high_s16(s16_input_high0));
           
           int32x4_t  subZeroVec_low_low0 = vsubq_s32(s32_cast_low_low0, s32ZeroPointVec);
           float32x4_t outputVec_low_low0 = vmulq_f32(vcvtq_f32_s32(subZeroVec_low_low0), f32Scale);

           int32x4_t  subZeroVec_low_high0 = vsubq_s32(s32_cast_low_high0, s32ZeroPointVec);
           float32x4_t outputVec_low_high0 = vmulq_f32(vcvtq_f32_s32(subZeroVec_low_high0), f32Scale);

           int32x4_t  subZeroVec_high_low0 = vsubq_s32(s32_cast_high_low0, s32ZeroPointVec);
           float32x4_t outputVec_high_low0 = vmulq_f32(vcvtq_f32_s32(subZeroVec_high_low0), f32Scale);

           int32x4_t  subZeroVec_high_high0 = vsubq_s32(s32_cast_high_high0, s32ZeroPointVec);
           float32x4_t outputVec_high_high0 = vmulq_f32(vcvtq_f32_s32(subZeroVec_high_high0), f32Scale);
            //second 16
           int16x8_t s16_input_low1 = vmovl_s8(vget_low_s8(s8_input1));
           int16x8_t s16_input_high1 = vmovl_s8(vget_high_s8(s8_input1));

           int32x4_t s32_cast_low_low1 = vmovl_s16(vget_low_s16(s16_input_low1));
           int32x4_t s32_cast_low_high1 = vmovl_s16(vget_high_s16(s16_input_low1));

           int32x4_t s32_cast_high_low1 = vmovl_s16(vget_low_s16(s16_input_high1));
           int32x4_t s32_cast_high_high1 = vmovl_s16(vget_high_s16(s16_input_high1));
           
           int32x4_t  subZeroVec_low_low1 = vsubq_s32(s32_cast_low_low1, s32ZeroPointVec);
           float32x4_t outputVec_low_low1 = vmulq_f32(vcvtq_f32_s32(subZeroVec_low_low1), f32Scale);

           int32x4_t  subZeroVec_low_high1 = vsubq_s32(s32_cast_low_high1, s32ZeroPointVec);
           float32x4_t outputVec_low_high1 = vmulq_f32(vcvtq_f32_s32(subZeroVec_low_high1), f32Scale);

           int32x4_t  subZeroVec_high_low1 = vsubq_s32(s32_cast_high_low1, s32ZeroPointVec);
           float32x4_t outputVec_high_low1 = vmulq_f32(vcvtq_f32_s32(subZeroVec_high_low1), f32Scale);

           int32x4_t  subZeroVec_high_high1 = vsubq_s32(s32_cast_high_high1, s32ZeroPointVec);
           float32x4_t outputVec_high_high1 = vmulq_f32(vcvtq_f32_s32(subZeroVec_high_high1), f32Scale);
    
           vst1q_f32((float32_t *)f32Output, outputVec_low_low0);
           vst1q_f32((float32_t *)f32Output + 4, outputVec_low_high0);
           vst1q_f32((float32_t *)f32Output + 8, outputVec_high_low0);
           vst1q_f32((float32_t *)f32Output + 12, outputVec_high_high0);
           vst1q_f32((float32_t *)f32Output + 16, outputVec_low_low1);
           vst1q_f32((float32_t *)f32Output + 20, outputVec_low_high1);
           vst1q_f32((float32_t *)f32Output + 24, outputVec_high_low1);
           vst1q_f32((float32_t *)f32Output + 28, outputVec_high_high1);
           
    
           s8Input += 32;
           f32Output += 32;
       }
    
       for(s32Ind = 0; s32Ind < s32Left; s32Ind++)
       {
           f32Output[s32Ind] = (s8Input[s32Ind] - s32ZeroPoint) * dScaleFactor;
       }
    }
}

#endif

//for arm arch64
//for nchw tensor float cvt to fix with neon code
void AR_NPU_FloatCvtFix_With_Stride_Neon(AR_FLOAT * pfInput, AR_CHAR* pcOutput, AR_NPU_TENSOR_S * pstTensor)
{
    AR_U32 u32Width = pstTensor->u32Width;
    AR_U32 u32Height = pstTensor->u32Height;
    AR_U32 u32Channel = pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast;
    AR_DOUBLE dScaleFactor = pstTensor->dScaleFactor;
    AR_S32 s32ZeroPoint = pstTensor->s32ZeroPoint;
    AR_U32 u32Precision = pstTensor->u32Precision;
    AR_U32 u32Batch = pstTensor->u32Num;
    AR_U32 u32RowStep = pstTensor->u32RowStep;
    AR_U32 u32KStep = pstTensor->u32KStep;

    AR_S32 positive_limit = (AR_S32)pow((AR_DOUBLE)2, (AR_DOUBLE)(u32Precision - 1)) - 1;
    AR_S32 negative_limit = (AR_S32)-pow((AR_DOUBLE)2, (AR_DOUBLE)(u32Precision - 1));
    AR_S32 s32Tmp = 0;
    int s32ChannelStepIn = u32Width * u32Height;
    int s32ChannelStepOut = u32KStep;
    
    int64x2_t s64ZeroPointVec = vdupq_n_s64((int64_t)s32ZeroPoint);
    float64x2_t f64Scale = vdupq_n_f64(1.0/dScaleFactor);
	
    if(u32Precision == 16)
    { 
        AR_FLOAT *f32Input = (AR_FLOAT *)pfInput;
        AR_S16 *s16Output = (AR_S16 *)pcOutput;
        AR_S32 s32WidthLoop = u32Width/4;
        AR_S32 s32WidthLeft = u32Width%4;
        AR_S32 s32HeightLoop = u32Batch * u32Height/2;
        AR_S32 s32HeightLeft = u32Batch * u32Height%2;
        AR_S32 s32HeightStepIn = 2 * u32Width;
        AR_S32 s32HeightStepOut = 2 * u32RowStep;
        for(int k = 0; k < u32Channel; k++)
        {
            for(int j = 0; j < s32HeightLoop; j++)
            {
                f32Input = pfInput + s32HeightStepIn * j + k * s32ChannelStepIn;
                s16Output = (AR_S16 *)(pcOutput + s32HeightStepOut * j + k * s32ChannelStepOut);
                for(int i = 0; i < s32WidthLoop; i++)
                {
                    float64x2_t f64RoundVal = vdupq_n_f64(0.5f);
                    
                    float64x2_t f64Load0 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input));
                    float64x2_t f64Load1 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input + 2));
                    float64x2_t f64Load2 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input + u32Width));
                    float64x2_t f64Load3 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input + u32Width + 2));
                    
                    float64x2_t f64ScaleInVec0 = vmulq_f64(f64Load0, f64Scale);
                    int64x2_t s64SignSignal0 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec0), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue0 = vaddq_f64(f64ScaleInVec0, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal0)));
                    int32x2_t s32SumValueInt0 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue0), s64ZeroPointVec));
                    
                    float64x2_t f64ScaleInVec1 = vmulq_f64(f64Load1, f64Scale);
                    int64x2_t s64SignSignal1 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec1), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue1 = vaddq_f64(f64ScaleInVec1, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal1)));
                    int32x2_t s32SumValueInt1 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue1), s64ZeroPointVec));
                    
                    float64x2_t f64ScaleInVec2 = vmulq_f64(f64Load2, f64Scale);
                    int64x2_t s64SignSignal2 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec2), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue2 = vaddq_f64(f64ScaleInVec2, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal2)));
                    int32x2_t s32SumValueInt2 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue2), s64ZeroPointVec));
                    
                    float64x2_t f64ScaleInVec3 = vmulq_f64(f64Load3, f64Scale);
                    int64x2_t s64SignSignal3 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec3), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue3 = vaddq_f64(f64ScaleInVec3, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal3)));
                    int32x2_t s32SumValueInt3 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue3), s64ZeroPointVec));
                    
                    int16x4_t s16Combine0 = vqmovn_s32(vcombine_s32(s32SumValueInt0, s32SumValueInt1));
                    int16x4_t s16Combine1 = vqmovn_s32(vcombine_s32(s32SumValueInt2, s32SumValueInt3));
                    //int16x8_t s16Combine = vcombine_s16(s16Combine0, s16Combine1);
                    vst1_s16((int16_t *)s16Output, s16Combine0);				
                    vst1_s16((int16_t *)s16Output + u32RowStep/2, s16Combine1);
                    
                    s16Output += 4;
                    f32Input += 4;
                }
                
                for(int p = 0; p < 2; p++)
                {
                    for(int i = 0; i < s32WidthLeft; i++)
                    {
                        s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                        s32Tmp = MIN(s32Tmp, positive_limit);
                        s32Tmp = MAX(s32Tmp, negative_limit);
                        s16Output[i] = (short)s32Tmp;
                    }
                    
                    f32Input += u32Width;
                    s16Output += u32RowStep/2;
                }
            }
            
            f32Input = pfInput + s32HeightStepIn * s32HeightLoop + k * s32ChannelStepIn;
            s16Output = (AR_S16 *)(pcOutput + s32HeightStepOut * s32HeightLoop +  k * s32ChannelStepOut);
            
            for(int j = 0; j < s32HeightLeft; j++)
            {
                for(int i = 0; i < u32Width; i++)
                {
                    s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                    s32Tmp = MIN(s32Tmp, positive_limit);
                    s32Tmp = MAX(s32Tmp, negative_limit);
                    s16Output[i] = (short)s32Tmp;
                }
                
                f32Input += u32Width;
                s16Output += u32RowStep/2;
            }
        }
    }
    else if(u32Precision == 8)
    {
        AR_FLOAT * f32Input = (AR_FLOAT *)pfInput;
        AR_CHAR * s8Output = (AR_CHAR *)pcOutput;
        AR_S32 s32WidthLoop = u32Width/8;
        AR_S32 s32WidthLeft = u32Width%8;
        AR_S32 s32HeightLoop = u32Batch * u32Height;
        AR_S32 s32HeightStepIn = u32Width;
        AR_S32 s32HeightStepOut = u32RowStep;
        
        for(int k = 0; k < u32Channel; k++)
        {
            for(int j =0; j < s32HeightLoop; j++)
            {
                f32Input = pfInput + s32HeightStepIn * j + k * s32ChannelStepIn;
                s8Output = pcOutput + s32HeightStepOut * j + k * s32ChannelStepOut;
                for(int i = 0; i < s32WidthLoop; i++)
                {
                    float64x2_t f64RoundVal = vdupq_n_f64(0.5f);
                    
                    float64x2_t f64Load0 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input));
                    float64x2_t f64Load1 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input + 2));
                    float64x2_t f64Load2 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input + 4));
                    float64x2_t f64Load3 = vcvt_f64_f32(vld1_f32((float32_t *)f32Input + 6));
                    
                    float64x2_t f64ScaleInVec0 = vmulq_f64(f64Load0, f64Scale);
                    int64x2_t s64SignSignal0 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec0), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue0 = vaddq_f64(f64ScaleInVec0, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal0)));
                    int32x2_t s32SumValueInt0 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue0), s64ZeroPointVec));
                    
                    float64x2_t f64ScaleInVec1 = vmulq_f64(f64Load1, f64Scale);
                    int64x2_t s64SignSignal1 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec1), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue1 = vaddq_f64(f64ScaleInVec1, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal1)));
                    int32x2_t s32SumValueInt1 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue1), s64ZeroPointVec));
                    
                    float64x2_t f64ScaleInVec2 = vmulq_f64(f64Load2, f64Scale);
                    int64x2_t s64SignSignal2 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec2), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue2 = vaddq_f64(f64ScaleInVec2, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal2)));
                    int32x2_t s32SumValueInt2 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue2), s64ZeroPointVec));
                    
                    float64x2_t f64ScaleInVec3 = vmulq_f64(f64Load3, f64Scale);
                    int64x2_t s64SignSignal3 = vandq_s64(vreinterpretq_s64_f64(f64ScaleInVec3), vdupq_n_s64((int64_t)pow(-2, 63)));
                    float64x2_t f64SumValue3 = vaddq_f64(f64ScaleInVec3, vreinterpretq_f64_s64(vorrq_s64(vreinterpretq_s64_f64(f64RoundVal), s64SignSignal3)));
                    int32x2_t s32SumValueInt3 = vqmovn_s64(vqaddq_s64(vcvtq_s64_f64(f64SumValue3), s64ZeroPointVec));
                    
                    int16x4_t s16Combine0 = vqmovn_s32(vcombine_s32(s32SumValueInt0, s32SumValueInt1));
                    int16x4_t s16Combine1 = vqmovn_s32(vcombine_s32(s32SumValueInt2, s32SumValueInt3));
                    int8x8_t s8Combine = vqmovn_s16(vcombine_s16(s16Combine0, s16Combine1));
                    vst1_s8((int8_t *)s8Output, s8Combine);
                    
                    s8Output += 8;
                    f32Input += 8;
                }
                
                //for W left , 
                for(int i = 0; i < s32WidthLeft; i++)
                {
                    s32Tmp = (int)round(f32Input[i] / dScaleFactor) + s32ZeroPoint;
                    s32Tmp = MIN(s32Tmp, positive_limit);
                    s32Tmp = MAX(s32Tmp, negative_limit);
                    s8Output[i] = (char)s32Tmp;
                }
            }
        }	
    }
}

//2x2 for Precision = 32

void AR_NPU_Matrix2x2_F32_Convert(AR_FLOAT * pTensorIn, AR_FLOAT *pTensorOut, AR_U32 u32InInterleave, AR_U32 u32OutInterleave)
{
     //load ch = 2; w = 2
    float32x2_t temp0 = vld1_f32((float32_t *)pTensorIn);
    float32x2_t temp1 = vld1_f32((float32_t *)pTensorIn + u32InInterleave);

    //1<->2
    float32x2x2_t vtrn32t0 = vtrn_f32(temp0, temp1);//0,1

    vst1_f32((float32_t *)pTensorOut, vtrn32t0.val[0]);
    vst1_f32((float32_t *)pTensorOut + u32OutInterleave, vtrn32t0.val[1]);
}

void AR_NPU_Matrix4x4_F32_Convert(AR_FLOAT * pTensorIn, AR_FLOAT *pTensorOut, AR_U32 u32InInterleave, AR_U32 u32OutInterleave)
{
    //load ch = 4; w = 4
    float32x4_t temp0 = vld1q_f32((float32_t *)pTensorIn);
    float32x4_t temp1 = vld1q_f32((float32_t *)pTensorIn + u32InInterleave);
    float32x4_t temp2 = vld1q_f32((float32_t *)pTensorIn + 2 * u32InInterleave);
    float32x4_t temp3 = vld1q_f32((float32_t *)pTensorIn + 3 * u32InInterleave);

    //1<->2
    float32x4x2_t vtrn32t0 = vtrnq_f32(temp0, temp1);//0,1
    float32x4x2_t vtrn32t1 = vtrnq_f32(temp2, temp3);//2,3

    //s16->s32
    float32x4_t vcombine32t0 = vcombine_f32(vget_low_f32(vtrn32t0.val[0]), vget_low_f32(vtrn32t1.val[0]));
    float32x4_t vcombine32t1 = vcombine_f32(vget_low_f32(vtrn32t0.val[1]), vget_low_f32(vtrn32t1.val[1]));
    float32x4_t vcombine32t2 = vcombine_f32(vget_high_f32(vtrn32t0.val[0]), vget_high_f32(vtrn32t1.val[0]));
    float32x4_t vcombine32t3 = vcombine_f32(vget_high_f32(vtrn32t0.val[1]), vget_high_f32(vtrn32t1.val[1]));

    //1<->3
    vst1q_f32((float32_t *)pTensorOut, vcombine32t0);
    vst1q_f32((float32_t *)pTensorOut + u32OutInterleave, vcombine32t1);
    vst1q_f32((float32_t *)pTensorOut + 2 * u32OutInterleave, vcombine32t2);
    vst1q_f32((float32_t *)pTensorOut + 3 * u32OutInterleave, vcombine32t3);
}

//4x4 for Precision = 16
void AR_NPU_Matrix4x4_S16_Convert(AR_S16 * pTensorIn, AR_S16 *pTensorOut, AR_U32 u32InInterleave, AR_U32 u32OutInterleave)
{
     //load ch = 4; w = 4
    int16x4_t temp0 = vld1_s16((int16_t *)pTensorIn);
    int16x4_t temp1 = vld1_s16((int16_t *)pTensorIn + u32InInterleave);
    int16x4_t temp2 = vld1_s16((int16_t *)pTensorIn + 2 * u32InInterleave);
    int16x4_t temp3 = vld1_s16((int16_t *)pTensorIn + 3 * u32InInterleave);

    //1<->2
    int16x4x2_t vtrn16t0 = vtrn_s16(temp0, temp1);//0,1
    int16x4x2_t vtrn16t1 = vtrn_s16(temp2, temp3);//2,3

    //s16->s32
    int32x2_t vcast32_0 = vreinterpret_s32_s16(vtrn16t0.val[0]);
    int32x2_t vcast32_1 = vreinterpret_s32_s16(vtrn16t0.val[1]);
    int32x2_t vcast32_2 = vreinterpret_s32_s16(vtrn16t1.val[0]);
    int32x2_t vcast32_3 = vreinterpret_s32_s16(vtrn16t1.val[1]);

    //1<->3
    int32x2x2_t vtrn32t0 = vtrn_s32(vcast32_0, vcast32_2);//0,2
    int32x2x2_t vtrn32t1 = vtrn_s32(vcast32_1, vcast32_3);//1,3

    //s32->s16
    int16x4_t vout16_0 = vreinterpret_s16_s32(vtrn32t0.val[0]);
    int16x4_t vout16_1 = vreinterpret_s16_s32(vtrn32t1.val[0]);
    int16x4_t vout16_2 = vreinterpret_s16_s32(vtrn32t0.val[1]);
    int16x4_t vout16_3 = vreinterpret_s16_s32(vtrn32t1.val[1]);

    vst1_s16((int16_t *)pTensorOut, vout16_0);
    vst1_s16((int16_t *)pTensorOut + u32OutInterleave, vout16_1);
    vst1_s16((int16_t *)pTensorOut + 2 * u32OutInterleave, vout16_2);
    vst1_s16((int16_t *)pTensorOut + 3 * u32OutInterleave, vout16_3);
}

// 16x16 for Precision = 16
void AR_NPU_Matrix8x8_S16_Convert(AR_S16 * pTensorIn, AR_S16 *pTensorOut, AR_U32 u32InInterleave, AR_U32 u32OutInterleave)
{
    
    //load ch = 8; w = 8
    int16x8_t temp0 = vld1q_s16((int16_t *)pTensorIn );
    int16x8_t temp1 = vld1q_s16((int16_t *)pTensorIn + u32InInterleave);
    int16x8_t temp2 = vld1q_s16((int16_t *)pTensorIn + 2 * u32InInterleave);
    int16x8_t temp3 = vld1q_s16((int16_t *)pTensorIn + 3 * u32InInterleave);
    int16x8_t temp4 = vld1q_s16((int16_t *)pTensorIn + 4 * u32InInterleave);
    int16x8_t temp5 = vld1q_s16((int16_t *)pTensorIn + 5 * u32InInterleave);
    int16x8_t temp6 = vld1q_s16((int16_t *)pTensorIn + 6 * u32InInterleave);
    int16x8_t temp7 = vld1q_s16((int16_t *)pTensorIn + 7 * u32InInterleave);

    //0<->1
    int16x8x2_t vtrn16t0 = vtrnq_s16(temp0, temp1);//0,1
    int16x8x2_t vtrn16t1 = vtrnq_s16(temp2, temp3);//2,3
    int16x8x2_t vtrn16t2 = vtrnq_s16(temp4, temp5);//4,5
    int16x8x2_t vtrn16t3 = vtrnq_s16(temp6, temp7);//6,7

    //s16->s32
    int32x4_t vcast32_0 = vreinterpretq_s32_s16(vtrn16t0.val[0]);
    int32x4_t vcast32_1 = vreinterpretq_s32_s16(vtrn16t0.val[1]);
    int32x4_t vcast32_2 = vreinterpretq_s32_s16(vtrn16t1.val[0]);
    int32x4_t vcast32_3 = vreinterpretq_s32_s16(vtrn16t1.val[1]);
    int32x4_t vcast32_4 = vreinterpretq_s32_s16(vtrn16t2.val[0]);
    int32x4_t vcast32_5 = vreinterpretq_s32_s16(vtrn16t2.val[1]);
    int32x4_t vcast32_6 = vreinterpretq_s32_s16(vtrn16t3.val[0]);
    int32x4_t vcast32_7 = vreinterpretq_s32_s16(vtrn16t3.val[1]);

    //0<->2
    int32x4x2_t vtrn32t0 = vtrnq_s32(vcast32_0, vcast32_2);//0,2
    int32x4x2_t vtrn32t1 = vtrnq_s32(vcast32_1, vcast32_3);//1,3
    int32x4x2_t vtrn32t2 = vtrnq_s32(vcast32_4, vcast32_6);//4,6
    int32x4x2_t vtrn32t3 = vtrnq_s32(vcast32_5, vcast32_7);//5,7

    int32x4_t vcombine32t0 = vcombine_s32(vget_low_s32(vtrn32t0.val[0]), vget_low_s32(vtrn32t2.val[0]));
    int32x4_t vcombine32t1 = vcombine_s32(vget_low_s32(vtrn32t1.val[0]), vget_low_s32(vtrn32t3.val[0]));
    int32x4_t vcombine32t2 = vcombine_s32(vget_low_s32(vtrn32t0.val[1]), vget_low_s32(vtrn32t2.val[1]));
    int32x4_t vcombine32t3 = vcombine_s32(vget_low_s32(vtrn32t1.val[1]), vget_low_s32(vtrn32t3.val[1]));
    int32x4_t vcombine32t4 = vcombine_s32(vget_high_s32(vtrn32t0.val[0]), vget_high_s32(vtrn32t2.val[0]));
    int32x4_t vcombine32t5 = vcombine_s32(vget_high_s32(vtrn32t1.val[0]), vget_high_s32(vtrn32t3.val[0]));
    int32x4_t vcombine32t6 = vcombine_s32(vget_high_s32(vtrn32t0.val[1]), vget_high_s32(vtrn32t2.val[1]));
    int32x4_t vcombine32t7 = vcombine_s32(vget_high_s32(vtrn32t1.val[1]), vget_high_s32(vtrn32t3.val[1]));

    //s32->s16
    int16x8_t vout16_0 = vreinterpretq_s16_s32(vcombine32t0);
    int16x8_t vout16_1 = vreinterpretq_s16_s32(vcombine32t1);
    int16x8_t vout16_2 = vreinterpretq_s16_s32(vcombine32t2);
    int16x8_t vout16_3 = vreinterpretq_s16_s32(vcombine32t3);
    int16x8_t vout16_4 = vreinterpretq_s16_s32(vcombine32t4);
    int16x8_t vout16_5 = vreinterpretq_s16_s32(vcombine32t5);
    int16x8_t vout16_6 = vreinterpretq_s16_s32(vcombine32t6);
    int16x8_t vout16_7 = vreinterpretq_s16_s32(vcombine32t7);

    vst1q_s16((int16_t *)pTensorOut, vout16_0);
    vst1q_s16((int16_t *)pTensorOut + u32OutInterleave, vout16_1);
    vst1q_s16((int16_t *)pTensorOut + 2 * u32OutInterleave, vout16_2);
    vst1q_s16((int16_t *)pTensorOut + 3 * u32OutInterleave, vout16_3);
    vst1q_s16((int16_t *)pTensorOut + 4 * u32OutInterleave, vout16_4);
    vst1q_s16((int16_t *)pTensorOut + 5 * u32OutInterleave, vout16_5);
    vst1q_s16((int16_t *)pTensorOut + 6 * u32OutInterleave, vout16_6);
    vst1q_s16((int16_t *)pTensorOut + 7 * u32OutInterleave, vout16_7);
}

// 8x8 for Precision = 8
void AR_NPU_Matrix8x8_S8_Convert(AR_CHAR * pTensorIn, AR_CHAR *pTensorOut, AR_U32 u32InInterleave, AR_U32 u32OutInterleave)
{
    //load ch = 8; w = 8
    int8x8_t temp0 = vld1_s8((int8_t *)pTensorIn );
    int8x8_t temp1 = vld1_s8((int8_t *)pTensorIn + u32InInterleave);
    int8x8_t temp2 = vld1_s8((int8_t *)pTensorIn + 2 * u32InInterleave);
    int8x8_t temp3 = vld1_s8((int8_t *)pTensorIn + 3 * u32InInterleave);
    int8x8_t temp4 = vld1_s8((int8_t *)pTensorIn + 4 * u32InInterleave);
    int8x8_t temp5 = vld1_s8((int8_t *)pTensorIn + 5 * u32InInterleave);
    int8x8_t temp6 = vld1_s8((int8_t *)pTensorIn + 6 * u32InInterleave);
    int8x8_t temp7 = vld1_s8((int8_t *)pTensorIn + 7 * u32InInterleave);

    //0<->1
    int8x8x2_t vtrn8t0 = vtrn_s8(temp0, temp1);//0,1
    int8x8x2_t vtrn8t1 = vtrn_s8(temp2, temp3);//2,3
    int8x8x2_t vtrn8t2 = vtrn_s8(temp4, temp5);//4,5
    int8x8x2_t vtrn8t3 = vtrn_s8(temp6, temp7);//6,7

    //s8->s16
    int16x4_t vcast16_0 = vreinterpret_s16_s8(vtrn8t0.val[0]);
    int16x4_t vcast16_1 = vreinterpret_s16_s8(vtrn8t0.val[1]);
    int16x4_t vcast16_2 = vreinterpret_s16_s8(vtrn8t1.val[0]);
    int16x4_t vcast16_3 = vreinterpret_s16_s8(vtrn8t1.val[1]);
    int16x4_t vcast16_4 = vreinterpret_s16_s8(vtrn8t2.val[0]);
    int16x4_t vcast16_5 = vreinterpret_s16_s8(vtrn8t2.val[1]);
    int16x4_t vcast16_6 = vreinterpret_s16_s8(vtrn8t3.val[0]);
    int16x4_t vcast16_7 = vreinterpret_s16_s8(vtrn8t3.val[1]);

    //0<->2
    int16x4x2_t vtrn16t0 = vtrn_s16(vcast16_0, vcast16_2);//0,2
    int16x4x2_t vtrn16t1 = vtrn_s16(vcast16_1, vcast16_3);//1,3
    int16x4x2_t vtrn16t2 = vtrn_s16(vcast16_4, vcast16_6);//4,6
    int16x4x2_t vtrn16t3 = vtrn_s16(vcast16_5, vcast16_7);//5,7

    //s16->s32
    int32x2_t vcast32_0 = vreinterpret_s32_s16(vtrn16t0.val[0]);
    int32x2_t vcast32_1 = vreinterpret_s32_s16(vtrn16t1.val[0]);
    int32x2_t vcast32_2 = vreinterpret_s32_s16(vtrn16t0.val[1]);
    int32x2_t vcast32_3 = vreinterpret_s32_s16(vtrn16t1.val[1]);
    int32x2_t vcast32_4 = vreinterpret_s32_s16(vtrn16t2.val[0]);
    int32x2_t vcast32_5 = vreinterpret_s32_s16(vtrn16t3.val[0]);
    int32x2_t vcast32_6 = vreinterpret_s32_s16(vtrn16t2.val[1]);
    int32x2_t vcast32_7 = vreinterpret_s32_s16(vtrn16t3.val[1]);

    //0<->4
    int32x2x2_t vtrn32t0 = vtrn_s32(vcast32_0, vcast32_4);//0,4
    int32x2x2_t vtrn32t1 = vtrn_s32(vcast32_1, vcast32_5);//1,5
    int32x2x2_t vtrn32t2 = vtrn_s32(vcast32_2, vcast32_6);//2,6
    int32x2x2_t vtrn32t3 = vtrn_s32(vcast32_3, vcast32_7);//3,7

    int8x8_t vout8_0 = vreinterpret_s8_s32(vtrn32t0.val[0]);
    int8x8_t vout8_1 = vreinterpret_s8_s32(vtrn32t1.val[0]);
    int8x8_t vout8_2 = vreinterpret_s8_s32(vtrn32t2.val[0]);
    int8x8_t vout8_3 = vreinterpret_s8_s32(vtrn32t3.val[0]);
    int8x8_t vout8_4 = vreinterpret_s8_s32(vtrn32t0.val[1]);
    int8x8_t vout8_5 = vreinterpret_s8_s32(vtrn32t1.val[1]);
    int8x8_t vout8_6 = vreinterpret_s8_s32(vtrn32t2.val[1]);
    int8x8_t vout8_7 = vreinterpret_s8_s32(vtrn32t3.val[1]);

    vst1_s8((int8_t *)pTensorOut, vout8_0);
    vst1_s8((int8_t *)pTensorOut + u32OutInterleave, vout8_1);
    vst1_s8((int8_t *)pTensorOut + 2 * u32OutInterleave, vout8_2);
    vst1_s8((int8_t *)pTensorOut + 3 * u32OutInterleave, vout8_3);
    vst1_s8((int8_t *)pTensorOut + 4 * u32OutInterleave, vout8_4);
    vst1_s8((int8_t *)pTensorOut + 5 * u32OutInterleave, vout8_5);
    vst1_s8((int8_t *)pTensorOut + 6 * u32OutInterleave, vout8_6);
    vst1_s8((int8_t *)pTensorOut + 7 * u32OutInterleave, vout8_7);
}

// 16x16 for Precision = 8
void AR_NPU_Matrix16x16_S8_Convert(AR_CHAR * pTensorIn, AR_CHAR *pTensorOut, AR_U32 u32InInterleave, AR_U32 u32OutInterleave)
{
    //load ch = 16; w = 16
    int8x16_t temp0 = vld1q_s8((int8_t *)pTensorIn );
    int8x16_t temp1 = vld1q_s8((int8_t *)pTensorIn + 1 * u32InInterleave);
    int8x16_t temp2 = vld1q_s8((int8_t *)pTensorIn + 2 * u32InInterleave);
    int8x16_t temp3 = vld1q_s8((int8_t *)pTensorIn + 3 * u32InInterleave);
    int8x16_t temp4 = vld1q_s8((int8_t *)pTensorIn + 4 * u32InInterleave);
    int8x16_t temp5 = vld1q_s8((int8_t *)pTensorIn + 5 * u32InInterleave);
    int8x16_t temp6 = vld1q_s8((int8_t *)pTensorIn + 6 * u32InInterleave);
    int8x16_t temp7 = vld1q_s8((int8_t *)pTensorIn + 7 * u32InInterleave);
	int8x16_t temp8 = vld1q_s8((int8_t *)pTensorIn + 8 * u32InInterleave);
    int8x16_t temp9 = vld1q_s8((int8_t *)pTensorIn + 9 * u32InInterleave);
    int8x16_t temp10 = vld1q_s8((int8_t *)pTensorIn + 10 * u32InInterleave);
    int8x16_t temp11 = vld1q_s8((int8_t *)pTensorIn + 11 * u32InInterleave);
    int8x16_t temp12 = vld1q_s8((int8_t *)pTensorIn + 12 * u32InInterleave);
    int8x16_t temp13 = vld1q_s8((int8_t *)pTensorIn + 13 * u32InInterleave);
    int8x16_t temp14 = vld1q_s8((int8_t *)pTensorIn + 14 * u32InInterleave);
    int8x16_t temp15 = vld1q_s8((int8_t *)pTensorIn + 15 * u32InInterleave);

    //0<->1
    int8x16x2_t vtrn8t0 = vtrnq_s8(temp0, temp1);//0,1
    int8x16x2_t vtrn8t1 = vtrnq_s8(temp2, temp3);//2,3
    int8x16x2_t vtrn8t2 = vtrnq_s8(temp4, temp5);//4,5
    int8x16x2_t vtrn8t3 = vtrnq_s8(temp6, temp7);//6,7
    int8x16x2_t vtrn8t4 = vtrnq_s8(temp8, temp9);//0,1
    int8x16x2_t vtrn8t5 = vtrnq_s8(temp10, temp11);//2,3
    int8x16x2_t vtrn8t6 = vtrnq_s8(temp12, temp13);//4,5
    int8x16x2_t vtrn8t7 = vtrnq_s8(temp14, temp15);//6,7

    //s8->s16
    int16x8_t vcast16_0 = vreinterpretq_s16_s8(vtrn8t0.val[0]);
    int16x8_t vcast16_1 = vreinterpretq_s16_s8(vtrn8t0.val[1]);
    int16x8_t vcast16_2 = vreinterpretq_s16_s8(vtrn8t1.val[0]);
    int16x8_t vcast16_3 = vreinterpretq_s16_s8(vtrn8t1.val[1]);
    int16x8_t vcast16_4 = vreinterpretq_s16_s8(vtrn8t2.val[0]);
    int16x8_t vcast16_5 = vreinterpretq_s16_s8(vtrn8t2.val[1]);
    int16x8_t vcast16_6 = vreinterpretq_s16_s8(vtrn8t3.val[0]);
    int16x8_t vcast16_7 = vreinterpretq_s16_s8(vtrn8t3.val[1]);
	int16x8_t vcast16_8 = vreinterpretq_s16_s8(vtrn8t4.val[0]);
    int16x8_t vcast16_9 = vreinterpretq_s16_s8(vtrn8t4.val[1]);
    int16x8_t vcast16_10 = vreinterpretq_s16_s8(vtrn8t5.val[0]);
    int16x8_t vcast16_11 = vreinterpretq_s16_s8(vtrn8t5.val[1]);
    int16x8_t vcast16_12 = vreinterpretq_s16_s8(vtrn8t6.val[0]);
    int16x8_t vcast16_13 = vreinterpretq_s16_s8(vtrn8t6.val[1]);
    int16x8_t vcast16_14 = vreinterpretq_s16_s8(vtrn8t7.val[0]);
    int16x8_t vcast16_15 = vreinterpretq_s16_s8(vtrn8t7.val[1]);

    //0<->2
    int16x8x2_t vtrn16t0 = vtrnq_s16(vcast16_0, vcast16_2);//0,2
    int16x8x2_t vtrn16t1 = vtrnq_s16(vcast16_1, vcast16_3);//1,3
    int16x8x2_t vtrn16t2 = vtrnq_s16(vcast16_4, vcast16_6);//4,6
    int16x8x2_t vtrn16t3 = vtrnq_s16(vcast16_5, vcast16_7);//5,7
    int16x8x2_t vtrn16t4 = vtrnq_s16(vcast16_8, vcast16_10);//8,10
    int16x8x2_t vtrn16t5 = vtrnq_s16(vcast16_9, vcast16_11);//9,11
    int16x8x2_t vtrn16t6 = vtrnq_s16(vcast16_12, vcast16_14);//12,14
    int16x8x2_t vtrn16t7 = vtrnq_s16(vcast16_13, vcast16_15);//13,15

    //s16->s32 
    int32x4_t vcast32_0 = vreinterpretq_s32_s16(vtrn16t0.val[0]); //0 
    int32x4_t vcast32_1 = vreinterpretq_s32_s16(vtrn16t1.val[0]); //1
    int32x4_t vcast32_2 = vreinterpretq_s32_s16(vtrn16t0.val[1]); //2
    int32x4_t vcast32_3 = vreinterpretq_s32_s16(vtrn16t1.val[1]); //3
    int32x4_t vcast32_4 = vreinterpretq_s32_s16(vtrn16t2.val[0]); //4
    int32x4_t vcast32_5 = vreinterpretq_s32_s16(vtrn16t3.val[0]); //5
    int32x4_t vcast32_6 = vreinterpretq_s32_s16(vtrn16t2.val[1]); //6
    int32x4_t vcast32_7 = vreinterpretq_s32_s16(vtrn16t3.val[1]); //7
	int32x4_t vcast32_8 = vreinterpretq_s32_s16(vtrn16t4.val[0]); //8
    int32x4_t vcast32_9 = vreinterpretq_s32_s16(vtrn16t5.val[0]); //9
    int32x4_t vcast32_10 = vreinterpretq_s32_s16(vtrn16t4.val[1]);//10
    int32x4_t vcast32_11 = vreinterpretq_s32_s16(vtrn16t5.val[1]);//11
    int32x4_t vcast32_12 = vreinterpretq_s32_s16(vtrn16t6.val[0]);//12
    int32x4_t vcast32_13 = vreinterpretq_s32_s16(vtrn16t7.val[0]);//13
    int32x4_t vcast32_14 = vreinterpretq_s32_s16(vtrn16t6.val[1]);//14
    int32x4_t vcast32_15 = vreinterpretq_s32_s16(vtrn16t7.val[1]);//15

    //0<->4
    int32x4x2_t vtrn32t0 = vtrnq_s32(vcast32_0, vcast32_4);//0,4
    int32x4x2_t vtrn32t1 = vtrnq_s32(vcast32_1, vcast32_5);//1,5
    int32x4x2_t vtrn32t2 = vtrnq_s32(vcast32_2, vcast32_6);//2,6
    int32x4x2_t vtrn32t3 = vtrnq_s32(vcast32_3, vcast32_7);//3,7
    int32x4x2_t vtrn32t4 = vtrnq_s32(vcast32_8, vcast32_12);//8,12
    int32x4x2_t vtrn32t5 = vtrnq_s32(vcast32_9, vcast32_13);//9,13
    int32x4x2_t vtrn32t6 = vtrnq_s32(vcast32_10, vcast32_14);//10,14
    int32x4x2_t vtrn32t7 = vtrnq_s32(vcast32_11, vcast32_15);//11,15

	//s32->s64 
	int64x2_t vcast64_0 = vreinterpretq_s64_s32(vtrn32t0.val[0]);	//0
	int64x2_t vcast64_1 = vreinterpretq_s64_s32(vtrn32t1.val[0]);	//1
	int64x2_t vcast64_2 = vreinterpretq_s64_s32(vtrn32t2.val[0]);	//2
	int64x2_t vcast64_3 = vreinterpretq_s64_s32(vtrn32t3.val[0]);	//3
	int64x2_t vcast64_4 = vreinterpretq_s64_s32(vtrn32t0.val[1]);	//4
	int64x2_t vcast64_5 = vreinterpretq_s64_s32(vtrn32t1.val[1]);	//5
	int64x2_t vcast64_6 = vreinterpretq_s64_s32(vtrn32t2.val[1]);	//6
	int64x2_t vcast64_7 = vreinterpretq_s64_s32(vtrn32t3.val[1]);	//7
	int64x2_t vcast64_8 = vreinterpretq_s64_s32(vtrn32t4.val[0]);	//8
	int64x2_t vcast64_9 = vreinterpretq_s64_s32(vtrn32t5.val[0]);	//9
	int64x2_t vcast64_10 = vreinterpretq_s64_s32(vtrn32t6.val[0]);	//10
	int64x2_t vcast64_11 = vreinterpretq_s64_s32(vtrn32t7.val[0]);	//11
	int64x2_t vcast64_12 = vreinterpretq_s64_s32(vtrn32t4.val[1]);	//12
	int64x2_t vcast64_13 = vreinterpretq_s64_s32(vtrn32t5.val[1]);	//13
	int64x2_t vcast64_14 = vreinterpretq_s64_s32(vtrn32t6.val[1]);	//14
	int64x2_t vcast64_15 = vreinterpretq_s64_s32(vtrn32t7.val[1]);	//15

	//0<-->8
	int64x2_t vtrn64t0 = vcopyq_laneq_s64(vcast64_0, 1, vcast64_8, 0);
	int64x2_t vtrn64t1 = vcopyq_laneq_s64(vcast64_1, 1, vcast64_9, 0);
	int64x2_t vtrn64t2 = vcopyq_laneq_s64(vcast64_2, 1, vcast64_10, 0);
	int64x2_t vtrn64t3 = vcopyq_laneq_s64(vcast64_3, 1, vcast64_11, 0);
	int64x2_t vtrn64t4 = vcopyq_laneq_s64(vcast64_4, 1, vcast64_12, 0);
	int64x2_t vtrn64t5 = vcopyq_laneq_s64(vcast64_5, 1, vcast64_13, 0);
	int64x2_t vtrn64t6 = vcopyq_laneq_s64(vcast64_6, 1, vcast64_14, 0);
	int64x2_t vtrn64t7 = vcopyq_laneq_s64(vcast64_7, 1, vcast64_15, 0);
	
	int64x2_t vtrn64t8 = vcopyq_laneq_s64(vcast64_8, 0, vcast64_0, 1);
	int64x2_t vtrn64t9 = vcopyq_laneq_s64(vcast64_9, 0, vcast64_1, 1);
	int64x2_t vtrn64t10 = vcopyq_laneq_s64(vcast64_10, 0, vcast64_2, 1);
	int64x2_t vtrn64t11 = vcopyq_laneq_s64(vcast64_11, 0, vcast64_3, 1);
	int64x2_t vtrn64t12 = vcopyq_laneq_s64(vcast64_12, 0, vcast64_4, 1);
	int64x2_t vtrn64t13 = vcopyq_laneq_s64(vcast64_13, 0, vcast64_5, 1);
	int64x2_t vtrn64t14 = vcopyq_laneq_s64(vcast64_14, 0, vcast64_6, 1);
	int64x2_t vtrn64t15 = vcopyq_laneq_s64(vcast64_15, 0, vcast64_7, 1);
	
	//s64 -> s8 
    int8x16_t vout8_0 = vreinterpretq_s8_s64(vtrn64t0);
    int8x16_t vout8_1 = vreinterpretq_s8_s64(vtrn64t1);
    int8x16_t vout8_2 = vreinterpretq_s8_s64(vtrn64t2);
    int8x16_t vout8_3 = vreinterpretq_s8_s64(vtrn64t3);
    int8x16_t vout8_4 = vreinterpretq_s8_s64(vtrn64t4);
    int8x16_t vout8_5 = vreinterpretq_s8_s64(vtrn64t5);
    int8x16_t vout8_6 = vreinterpretq_s8_s64(vtrn64t6);
    int8x16_t vout8_7 = vreinterpretq_s8_s64(vtrn64t7);
	int8x16_t vout8_8 = vreinterpretq_s8_s64(vtrn64t8);
    int8x16_t vout8_9 = vreinterpretq_s8_s64(vtrn64t9);
    int8x16_t vout8_10 = vreinterpretq_s8_s64(vtrn64t10);
    int8x16_t vout8_11 = vreinterpretq_s8_s64(vtrn64t11);
    int8x16_t vout8_12 = vreinterpretq_s8_s64(vtrn64t12);
    int8x16_t vout8_13 = vreinterpretq_s8_s64(vtrn64t13);
    int8x16_t vout8_14 = vreinterpretq_s8_s64(vtrn64t14);
    int8x16_t vout8_15 = vreinterpretq_s8_s64(vtrn64t15);

    vst1q_s8((int8_t *)pTensorOut, vout8_0);
    vst1q_s8((int8_t *)pTensorOut + 1 * u32OutInterleave, vout8_1);
    vst1q_s8((int8_t *)pTensorOut + 2 * u32OutInterleave, vout8_2);
    vst1q_s8((int8_t *)pTensorOut + 3 * u32OutInterleave, vout8_3);
    vst1q_s8((int8_t *)pTensorOut + 4 * u32OutInterleave, vout8_4);
    vst1q_s8((int8_t *)pTensorOut + 5 * u32OutInterleave, vout8_5);
    vst1q_s8((int8_t *)pTensorOut + 6 * u32OutInterleave, vout8_6);
    vst1q_s8((int8_t *)pTensorOut + 7 * u32OutInterleave, vout8_7);
	vst1q_s8((int8_t *)pTensorOut + 8 * u32OutInterleave, vout8_8);
    vst1q_s8((int8_t *)pTensorOut + 9 * u32OutInterleave, vout8_9);
    vst1q_s8((int8_t *)pTensorOut + 10 * u32OutInterleave, vout8_10);
    vst1q_s8((int8_t *)pTensorOut + 11 * u32OutInterleave, vout8_11);
    vst1q_s8((int8_t *)pTensorOut + 12 * u32OutInterleave, vout8_12);
    vst1q_s8((int8_t *)pTensorOut + 13 * u32OutInterleave, vout8_13);
    vst1q_s8((int8_t *)pTensorOut + 14 * u32OutInterleave, vout8_14);
	vst1q_s8((int8_t *)pTensorOut + 15 * u32OutInterleave, vout8_15);
}


AR_S32 AR_NPU_Exe_Long_Channel_NhwcCvtNchw_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    //AR_U32 u32Byte = sizeof(AR_FLOAT);
    AR_S32 i = 0, j = 0;

    AR_U32 u32ChStep = u32KSizeNorm;
    AR_S32 s32WLoop = u32W/2;
    AR_S32 s32WLeft = u32W%2;
    AR_S32 s32ChLoop = u32ChStep/2;
    //AR_S32 s32ChLeft = u32ChStep%2;

    //caculate the normal tensor blocks
    for(k =0; k < u32KNormNum; k++)
    {
        for(j = 0; j < s32ChLoop; j++)
        {
            if(!s32WLeft)
            {
            	for(b = 0;b < u32B;b++)
            	{
	                for(h = 0; h < u32H; h++)
	                {
	                    AR_FLOAT * pTensorIn = (AR_FLOAT *)(pInput + j * 2 + (k * u32KStep + h * u32RowStep + b * u32RowStep * u32H)/sizeof(float));
	                    AR_FLOAT * pTensorOut = (AR_FLOAT *)(pOutput + (h * u32W + j * 2 * u32W * u32H + k * u32W * u32H * u32KSizeNorm + b * u32H *u32W * u32KSizeNorm));
	                    for(w = 0; w < s32WLoop; w++)
	                    {
	                        AR_NPU_Matrix2x2_F32_Convert(pTensorIn + w * 2 * u32ChStep, pTensorOut + w * 2, u32KSizeNorm, u32W*u32H);
	                    }
	                }
            	}
            }
            else
            {
            	for(b = 0;b < u32B;b++)
            	{
	                for(h = 0; h < u32H; h++)
	                {
	                    AR_FLOAT * pTensorIn = (AR_FLOAT *)(pInput + j * 2 + (k * u32KStep + h * u32RowStep+ b * u32RowStep * u32H)/sizeof(float));
	                    AR_FLOAT * pTensorOut = (AR_FLOAT *)(pOutput + (h * u32W + j * 2 * u32W * u32H + k * u32W * u32H * u32KSizeNorm + b * u32H *u32W * u32KSizeNorm));
	                    for(w = 0; w < s32WLoop; w++)
	                    {
	                        AR_NPU_Matrix2x2_F32_Convert(pTensorIn + w * 2 * u32ChStep, pTensorOut + w * 2, u32KSizeNorm, u32W*u32H);
	                    }

	                    //W left
	                    AR_FLOAT * pWInLeft = pTensorIn + 2 * s32WLoop * u32KSizeNorm;
	                    AR_FLOAT * pWOutLeft = pTensorOut + 2 * s32WLoop;
	                    AR_FLOAT * pWInLeftTemp = pWInLeft;
	                    for(int w1 = 0; w1 < s32WLeft; w1++)
	                    {
	                       for( i = 0; i < 2; i++)
	                       {
	                            *(pWOutLeft + i * u32W * u32H) = *pWInLeftTemp ++;
	                       }
	                       pWOutLeft++;
	                       pWInLeft += u32KSizeNorm;
	                       pWInLeftTemp = pWInLeft;
	                    }
	                }
            	}
            }
        }
    }

    //caculate the last block
    AR_S32 s32LastChLoop = u32KSizeLastOri/2;
    //AR_S32 s32LastChLeft = u32KSizeLastOri%2;

    for(k = 0; k < s32LastChLoop; k++)
    {
        if(!s32WLeft)
        {
        	for(b = 0;b < u32B;b++)
            {
	            for (h = 0; h < u32H; h++)
	            {

	                AR_FLOAT * pInLast = (AR_FLOAT *)(pInput+ (u32KNormNum * u32KStep + h * u32RowStep + b * u32RowStep * u32H)/(sizeof(float)) + 2 * k);
	                AR_FLOAT * pOutLast = (AR_FLOAT *)(pOutput + (k * 2 * u32W * u32H + u32KNormNum * u32KSizeNorm * u32W * u32H  + h * u32W + b * u32H *u32W * u32KSizeNorm));
	                for(w = 0; w < s32WLoop; w++)
	                {
	                    AR_NPU_Matrix2x2_F32_Convert(pInLast + w * 2 * u32KSizeLast, pOutLast + w * 2, u32KSizeLast, u32W*u32H);
	                }
	            }
        	}
        }
        else
        {
        	for(b = 0;b < u32B;b++)
            {
	            for (h = 0; h < u32H; h++)
	            {
	                AR_FLOAT * pInLast = (AR_FLOAT *)(pInput+ (u32KNormNum * u32KStep + h * u32RowStep + b * u32RowStep * u32H)/(sizeof(float)) + 2 * k);
	                AR_FLOAT * pOutLast = (AR_FLOAT *)(pOutput + (k * 2 * u32W * u32H + u32KNormNum * u32KSizeNorm * u32W * u32H  + h * u32W + b * u32H *u32W * u32KSizeNorm));
	                for(w = 0; w < s32WLoop; w++)
	                {
	                    AR_NPU_Matrix2x2_F32_Convert(pInLast + w * 2 * u32KSizeLast, pOutLast + w * 2, u32KSizeLast, u32W*u32H);
	                }

	                //W left
	                AR_FLOAT * pWInLeft = pInLast + 2 * s32WLoop * u32KSizeLast;
	                AR_FLOAT * pWOutLeft = pOutLast + 2 * s32WLoop;
	                AR_FLOAT * pWInLeftTemp = pWInLeft;
	                for(int w1 = 0; w1 < s32WLeft; w1++)
	                {
	                   for( i = 0; i < 2; i++)
	                   {
	                        *(pWOutLeft + i * u32W * u32H) = *pWInLeftTemp ++;
	                   }
	                   pWOutLeft++;
	                   pWInLeft += u32KSizeLast;
	                   pWInLeftTemp = pWInLeft;
	                }
	            }
        	}
        }
   }

   return 0;
}

void AR_NPU_Exe_One_Channel_NhwcCvtNchw_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    //AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    //AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    //AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_FLOAT * f32Input = pInput;
    AR_FLOAT * f32Output = pOutput;


    for(k = 0; k < u32KNormNum; k++)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(f32Output, f32Input, u32W* sizeof(AR_FLOAT));

            f32Input += u32RowStep/sizeof(AR_FLOAT);
            f32Output += u32W;
        }
    }

    if(u32KSizeLastOri)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(f32Output, f32Input, u32W* sizeof(AR_FLOAT));

            f32Input += u32RowStep/sizeof(AR_FLOAT);
            f32Output += u32W;
        }
    }  
}

AR_S32 AR_NPU_Exe_NhwcCvtNchw_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 u32KSizeNormal = pTensor->u32KSizeNorm;

    if(u32KSizeNormal == 1)
    {
        AR_NPU_Exe_One_Channel_NhwcCvtNchw_F32_Neon( pInput, pOutput, pTensor);
    }
    else
    {
        AR_NPU_Exe_Long_Channel_NhwcCvtNchw_F32_Neon( pInput, pOutput, pTensor);
    }

    return 0;
}

void AR_NPU_Exe_One_Channel_NhwcCvtNchw_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    //AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    //AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    //AR_S32 i = 0, j = 0;
    AR_CHAR * s8Input = pInput;
    AR_CHAR * s8Output = pOutput;
    AR_U32 u32HStep = u32W * u32Byte;

    for(k = 0; k < u32KNormNum; k++)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(s8Output, s8Input, u32HStep);

            s8Input += u32RowStep;
            s8Output += u32HStep;
        }
    }

    if(u32KSizeLastOri)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(s8Output, s8Input, u32HStep);

            s8Input += u32RowStep;
            s8Output += u32HStep;
        }
    }
}

void AR_NPU_Exe_Two_Channel_NhwcCvtNchw_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_S32 i = 0;

    AR_U32 u32DstHStep = u32W * u32Byte;
    //AR_U32 u32DstChStep = u32W * u32H * u32Byte;
    AR_U32 u32DstBatchStep = u32W * u32H * u32KSizeNorm* u32Byte;
    AR_U32 u32DstKStep = u32B * u32W * u32H * u32KSizeNorm* u32Byte;
    if(pTensor->u32Precision == 16)
    {
        if(u32W < 8)
        {
            AR_U32 u32WLoop = u32W/4;
            AR_U32 u32WLeft = u32W%4;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x4x2_t s16In = vld2_s16((int16_t *)(s16pIn + w * 4 * u32KSizeNorm));
                           vst1_s16((int16_t *)(s16pOut + w * 4), s16In.val[0]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + u32H * u32W), s16In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (4 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (4 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
                
                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%2;
                if(u32LastLeft)
                {
                    AR_S16 * pInLastChLeft = (AR_S16 *)(pInput+ u32KNormNum * u32KStep );
                    AR_S16 * pOutLastChLeft = (AR_S16 *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);

                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            
                            for(w = 0; w < u32W; w++)
                            {
                                AR_S16 * pInLastLeft = (AR_S16 *)(pInLastChLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_S16 * pOutLastLeft = (AR_S16 *)(pOutLastChLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w);
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }  
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x4x2_t s16In = vld2_s16((int16_t *)(s16pIn + w * 4 * u32KSizeLastOri));
                           vst1_s16((int16_t *)(s16pOut + w * 4), s16In.val[0]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + u32H * u32W), s16In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (4 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (4 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x8x2_t s16In = vld2q_s16((int16_t *)(s16pIn + w * 8 * u32KSizeNorm));
                           vst1q_s16((int16_t *)(s16pOut + w * 8), s16In.val[0]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + u32H * u32W), s16In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;
                
                AR_U32 u32LastLeft = u32KSizeLastOri%2;
                if(u32LastLeft)
                {
                    AR_S16 * pInLastChLeft = (AR_S16 *)(pInput+ u32KNormNum * u32KStep );
                    AR_S16 * pOutLastChLeft = (AR_S16 *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);

                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            
                            for(w = 0; w < u32W; w++)
                            {
                                AR_S16 * pInLastLeft = (AR_S16 *)(pInLastChLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_S16 * pOutLastLeft = (AR_S16 *)(pOutLastChLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w);
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }  
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x8x2_t s16In = vld2q_s16((int16_t *)(s16pIn + w * 8 * u32KSizeLastOri));
                           vst1q_s16((int16_t *)(s16pOut + w * 8), s16In.val[0]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + u32H * u32W), s16In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
    }   
    else if(pTensor->u32Precision == 8)
    {
        if(u32W < 16)
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x8x2_t s8In = vld2_s8((int8_t *)(s8pIn + w * 8 * u32KSizeNorm));
                           vst1_s8((int8_t *)(s8pOut + w * 8), s8In.val[0]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + u32H * u32W), s8In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%2;
                if(u32LastLeft)
                {
                    AR_CHAR * pInChLastLeft = (AR_CHAR *)(pInput+ u32KNormNum * u32KStep);
                    AR_CHAR * pOutChLastLeft = (AR_CHAR *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            for(w = 0; w < u32W; w++)
                            {
                                AR_CHAR * pInLastLeft = (AR_CHAR *)(pInChLastLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_CHAR * pOutLastLeft = (AR_CHAR *)pOutChLastLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w;
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x8x2_t s8In = vld2_s8((int8_t *)(s8pIn + w * 8 * u32KSizeLastOri));
                           vst1_s8((int8_t *)(s8pOut + w * 8), s8In.val[0]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + u32H * u32W), s8In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/16;
            AR_U32 u32WLeft = u32W%16;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x16x2_t s8In = vld2q_s8((int8_t *)(s8pIn + w * 16 * u32KSizeNorm));
                           vst1q_s8((int8_t *)(s8pOut + w * 16), s8In.val[0]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + u32H * u32W), s8In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (16 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (16 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%2;
                if(u32LastLeft)
                {
                    AR_CHAR * pInChLastLeft = (AR_CHAR *)(pInput+ u32KNormNum * u32KStep);
                    AR_CHAR * pOutChLastLeft = (AR_CHAR *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            for(w = 0; w < u32W; w++)
                            {
                                AR_CHAR * pInLastLeft = (AR_CHAR *)(pInChLastLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_CHAR * pOutLastLeft = (AR_CHAR *)pOutChLastLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w;
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x16x2_t s8In = vld2q_s8((int8_t *)(s8pIn + w * 16 * u32KSizeLastOri));
                           vst1q_s8((int8_t *)(s8pOut + w * 16), s8In.val[0]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + u32H * u32W), s8In.val[1]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (16 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (16 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 2; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
    }
}

void AR_NPU_Exe_Four_Channel_NhwcCvtNchw_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_S32 i = 0;

    AR_U32 u32DstHStep = u32W * u32Byte;
    //AR_U32 u32DstChStep = u32W * u32H * u32Byte;
    AR_U32 u32DstBatchStep = u32W * u32H * u32KSizeNorm* u32Byte;
    AR_U32 u32DstKStep = u32B * u32W * u32H * u32KSizeNorm* u32Byte;
    if(pTensor->u32Precision == 16)
    {
        if(u32W < 8)
        {
            AR_U32 u32WLoop = u32W/4;
            AR_U32 u32WLeft = u32W%4;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x4x4_t s16In = vld4_s16((int16_t *)(s16pIn + w * 4 * u32KSizeNorm));
                           vst1_s16((int16_t *)(s16pOut + w * 4), s16In.val[0]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + u32H * u32W), s16In.val[1]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + 2 * u32H * u32W), s16In.val[2]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + 3 * u32H * u32W), s16In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (4 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (4 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%4;
                if(u32LastLeft)
                {
                    AR_S16 * pInLastChLeft = (AR_S16 *)(pInput+ u32KNormNum * u32KStep );
                    AR_S16 * pOutLastChLeft = (AR_S16 *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);

                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            
                            for(w = 0; w < u32W; w++)
                            {
                                AR_S16 * pInLastLeft = (AR_S16 *)(pInLastChLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_S16 * pOutLastLeft = (AR_S16 *)(pOutLastChLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w);
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }  
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x4x4_t s16In = vld4_s16((int16_t *)(s16pIn + w * 4 * u32KSizeNorm));
                           vst1_s16((int16_t *)(s16pOut + w * 4), s16In.val[0]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + u32H * u32W), s16In.val[1]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + 2 * u32H * u32W), s16In.val[2]);
                           vst1_s16((int16_t *)(s16pOut + w * 4 + 3 * u32H * u32W), s16In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (4 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (4 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x8x4_t s16In = vld4q_s16((int16_t *)(s16pIn + w * 8 * u32KSizeNorm));
                           vst1q_s16((int16_t *)(s16pOut + w * 8), s16In.val[0]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + u32H * u32W), s16In.val[1]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + 2 * u32H * u32W), s16In.val[2]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + 3 * u32H * u32W), s16In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%4;
                if(u32LastLeft)
                {
                    AR_S16 * pInLastChLeft = (AR_S16 *)(pInput+ u32KNormNum * u32KStep );
                    AR_S16 * pOutLastChLeft = (AR_S16 *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);

                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            
                            for(w = 0; w < u32W; w++)
                            {
                                AR_S16 * pInLastLeft = (AR_S16 *)(pInLastChLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_S16 * pOutLastLeft = (AR_S16 *)(pOutLastChLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w);
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }  
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_S16 * s16pIn = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_S16 * s16pOut = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int16x8x4_t s16In = vld4q_s16((int16_t *)(s16pIn + w * 8 * u32KSizeNorm));
                           vst1q_s16((int16_t *)(s16pOut + w * 8), s16In.val[0]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + u32H * u32W), s16In.val[1]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + 2 * u32H * u32W), s16In.val[2]);
                           vst1q_s16((int16_t *)(s16pOut + w * 8 + 3 * u32H * u32W), s16In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_S16 * s16pInLeft = (AR_S16 *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_S16 * s16pOutLeft = (AR_S16 *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s16pOutLeft = *s16pInLeft++;
                                s16pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
    }   
    else if(pTensor->u32Precision == 8)
    {
        if(u32W < 16)
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x8x4_t s8In = vld4_s8((int8_t *)(s8pIn + w * 8 * u32KSizeNorm));
                           vst1_s8((int8_t *)(s8pOut + w * 8), s8In.val[0]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + u32H * u32W), s8In.val[1]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + 2 * u32H * u32W), s8In.val[2]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + 3 * u32H * u32W), s8In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%4;
                if(u32LastLeft)
                {
                    AR_CHAR * pInChLastLeft = (AR_CHAR *)(pInput+ u32KNormNum * u32KStep);
                    AR_CHAR * pOutChLastLeft = (AR_CHAR *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            for(w = 0; w < u32W; w++)
                            {
                                AR_CHAR * pInLastLeft = (AR_CHAR *)(pInChLastLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_CHAR * pOutLastLeft = (AR_CHAR *)pOutChLastLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w;
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x8x4_t s8In = vld4_s8((int8_t *)(s8pIn + w * 8 * u32KSizeNorm));
                           vst1_s8((int8_t *)(s8pOut + w * 8), s8In.val[0]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + u32H * u32W), s8In.val[1]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + 2 * u32H * u32W), s8In.val[2]);
                           vst1_s8((int8_t *)(s8pOut + w * 8 + 3 * u32H * u32W), s8In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (8 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput +u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (8 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/16;
            AR_U32 u32WLeft = u32W%16;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32SrcKOffset = k * u32KStep;
                    AR_U32 u32DstKOffset = k * u32DstKStep;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x16x4_t s8In = vld4q_s8((int8_t *)(s8pIn + w * 16 * u32KSizeNorm));
                           vst1q_s8((int8_t *)(s8pOut + w * 16), s8In.val[0]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + u32H * u32W), s8In.val[1]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + 2 * u32H * u32W), s8In.val[2]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + 3 * u32H * u32W), s8In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKOffset + h * u32RowStep + (16 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKOffset + h * u32DstHStep + (16 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }

                AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
                AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;

                AR_U32 u32LastLeft = u32KSizeLastOri%4;
                if(u32LastLeft)
                {
                    AR_CHAR * pInChLastLeft = (AR_CHAR *)(pInput+ u32KNormNum * u32KStep);
                    AR_CHAR * pOutChLastLeft = (AR_CHAR *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H) * u32Byte);
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_U32 u32SrcHOffset = h * u32RowStep;
                            AR_U32 u32DstHOffset = h * u32W * u32Byte;
                            for(w = 0; w < u32W; w++)
                            {
                                AR_CHAR * pInLastLeft = (AR_CHAR *)(pInChLastLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                                AR_CHAR * pOutLastLeft = (AR_CHAR *)pOutChLastLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w;
                                for(i = 0; i < u32LastLeft; i++)
                                {
                                    *pOutLastLeft = *pInLastLeft++;
                                    pOutLastLeft += u32H *u32W;
                                }
                            }
                        }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_CHAR * s8pIn = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep);
                        for(w = 0; w < u32WLoop; w++)
                       {
                           int8x16x4_t s8In = vld4q_s8((int8_t *)(s8pIn + w * 16 * u32KSizeNorm));
                           vst1q_s8((int8_t *)(s8pOut + w * 16), s8In.val[0]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + u32H * u32W), s8In.val[1]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + 2 * u32H * u32W), s8In.val[2]);
                           vst1q_s8((int8_t *)(s8pOut + w * 16 + 3 * u32H * u32W), s8In.val[3]);
                       }

                       for(w = 0; w < u32WLeft; w++)
                       {
                           AR_CHAR * s8pInLeft = (AR_CHAR *)(pInput + u32SrcBatchOffset + u32SrcKLastOffset + h * u32RowStep + (16 * u32WLoop + w) * u32KSizeNorm * u32Byte );
                           AR_CHAR * s8pOutLeft = (AR_CHAR *)(pOutput + u32DstBatchOffset + u32DstKLastOffset + h * u32DstHStep + (16 * u32WLoop + w) * u32Byte);
                           for(i = 0; i < 4; i++)
                           {
                                *s8pOutLeft = *s8pInLeft++;
                                s8pOutLeft += u32W * u32H;
                           }
                       }
                    }
                }
            }
        }
    }
}

AR_S32 AR_NPU_Exe_Long_Channel_NhwcCvtNchw_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_S32 i = 0, j = 0;

    AR_U32 u32DstHStep = u32W * u32Byte;
    //AR_U32 u32DstChStep = u32W * u32H * u32Byte;
    AR_U32 u32DstBatchStep = u32W * u32H * u32KSizeNorm* u32Byte;
    AR_U32 u32DstKStep = u32B * u32W * u32H * u32KSizeNorm* u32Byte;
    if(pTensor->u32Precision == 16)
    {
        AR_U32 u32ChStep = u32KSizeNorm;
        AR_S32 s32WLoop = u32W/8;
        AR_S32 s32WLeft = u32W%8;
        AR_S32 s32ChLoop = u32ChStep/8;
        //AR_S32 s32ChLeft = u32ChStep%8;

        //printf("u32KSizeNorm = %d, s32WLoop = %d, s32WLeft = %d\n", u32KSizeNorm, s32WLoop, s32WLeft);
        //printf("s32ChLoop = %d, s32ChLeft = %d, s32Ksizelast = %d\n", s32ChLoop, s32ChLeft, u32KSizeLastOri);
        
        //caculate the normal tensor blocks
        for(k =0; k < u32KNormNum; k++)
        {
            AR_U32 u32SrcKOffset = k * u32KStep;
            AR_U32 u32DstKOffset = k * u32DstKStep;
            for(j = 0; j < s32ChLoop; j++)
            {
                AR_U32 u32SrcChOffset = j * 8 * u32Byte;
                AR_U32 u32DstChOffset = j * 8 * u32W * u32H * u32Byte;
                if(!s32WLeft)
                {
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for(h = 0; h < u32H; h++)
                        {
                            AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32SrcChOffset + u32SrcKOffset + u32SrcBatchOffset + h * u32RowStep);
                            AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32DstChOffset + u32DstKOffset + u32DstBatchOffset  + h * u32W * u32Byte);
                            for(w = 0; w < s32WLoop; w++)
                            {
                                AR_NPU_Matrix8x8_S16_Convert(pTensorIn + w * 8 * u32ChStep, pTensorOut + w * 8, u32KSizeNorm, u32W*u32H);
                            }
                        }
                    }
                }
                else//test
                {
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for(h = 0; h < u32H; h++)
                        {
                            AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32SrcChOffset + u32SrcKOffset + u32SrcBatchOffset + h * u32RowStep);
                            AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32DstChOffset + u32DstKOffset + u32DstBatchOffset  + h * u32W * u32Byte);
                            for(w = 0; w < s32WLoop; w++)
                            {
                                AR_NPU_Matrix8x8_S16_Convert(pTensorIn + w * 8 * u32ChStep, pTensorOut + w * 8, u32KSizeNorm, u32W*u32H);
                            }

                            //W left
                            AR_S16 * pWInLeft = pTensorIn + 8 * s32WLoop * u32KSizeNorm;
                            AR_S16 * pWOutLeft = pTensorOut + 8 * s32WLoop;
                            AR_S16 * pWInLeftTemp = pWInLeft;
                            for(int w1 = 0; w1 < s32WLeft; w1++)
                            {
                               for( i = 0; i < 8; i++)
                               {
                                    *(pWOutLeft + i * u32W * u32H) = *pWInLeftTemp ++;
                               }
                               pWOutLeft++;
                               pWInLeft += u32KSizeNorm;
                               pWInLeftTemp = pWInLeft;
                            }
                        }
                    }
                }
            }
        }

        //caculate the last block
        AR_S32 s32LastChLoop = u32KSizeLastOri/8;
        AR_S32 s32LastChLeft = u32KSizeLastOri%8;

        AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
        AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;
        //printf("s32LastChLoop = %d, s32LastChLeft = %d\n", s32LastChLoop, s32LastChLeft);
        for(k = 0; k < s32LastChLoop; k++)
        {
            AR_U32 u32SrcChOffset = 8 * k * u32Byte;
            AR_U32 u32DstChOffset = k * 8 * u32W * u32H * u32Byte;
            if(!s32WLeft)
            {
                for(b = 0;b < u32B;b++)
                {
                    AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                    AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                    for (h = 0; h < u32H; h++)
                    {
                        AR_S16 * pInLast = (AR_S16 *)(pInput+ u32SrcKLastOffset + u32SrcChOffset + u32SrcBatchOffset + h * u32RowStep);
                        AR_S16 * pOutLast = (AR_S16 *)(pOutput + u32DstKLastOffset +u32DstChOffset + u32DstBatchOffset  + h * u32DstHStep);
                        for(w = 0; w < s32WLoop; w++)
                        {
                            AR_NPU_Matrix8x8_S16_Convert(pInLast + w * 8 * u32KSizeLast, pOutLast + w * 8, u32KSizeLast, u32W*u32H);
                        }
                    }
                }
            }
            else//test
            {
                for(b = 0;b < u32B;b++)
                {
                    AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                    AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                    for (h = 0; h < u32H; h++)
                    {
                        AR_S16 * pInLast = (AR_S16 *)(pInput+ u32SrcKLastOffset + u32SrcChOffset + u32SrcBatchOffset + h * u32RowStep);
                        AR_S16 * pOutLast = (AR_S16 *)(pOutput + u32DstKLastOffset +u32DstChOffset + u32DstBatchOffset  + h * u32DstHStep);
                        for(w = 0; w < s32WLoop; w++)
                        {
                            AR_NPU_Matrix8x8_S16_Convert(pInLast + w * 8 * u32KSizeLast, pOutLast + w * 8, u32KSizeLast, u32W*u32H);
                        }

                        //W left
                        AR_S16 * pWInLeft = pInLast + 8 * s32WLoop * u32KSizeLast;
                        AR_S16 * pWOutLeft = pOutLast + 8 * s32WLoop;
                        AR_S16 * pWInLeftTemp = pWInLeft;
                        for(int w1 = 0; w1 < s32WLeft; w1++)
                        {
                           for( i = 0; i < 8; i++)
                           {
                                *(pWOutLeft + i * u32W * u32H) = *pWInLeftTemp ++;
                           }
                           pWOutLeft++;
                           pWInLeft += u32KSizeLast;
                           pWInLeftTemp = pWInLeft;
                        }
                    }
                }
            }
        }

        if(s32LastChLeft)//test
        {
            AR_S16 * pInLastChLeft = (AR_S16 *)(pInput+ u32KNormNum * u32KStep + s32LastChLoop * 8 * u32Byte);
            AR_S16 * pOutLastChLeft = (AR_S16 *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H + s32LastChLoop * 8 * u32W * u32H) * u32Byte);

            for(b = 0;b < u32B;b++)
            {
                AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                for (h = 0; h < u32H; h++)
                {
                    AR_U32 u32SrcHOffset = h * u32RowStep;
                    AR_U32 u32DstHOffset = h * u32W * u32Byte;
                    
                    for(w = 0; w < u32W; w++)
                    {
                        AR_S16 * pInLastLeft = (AR_S16 *)(pInLastChLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                        AR_S16 * pOutLastLeft = (AR_S16 *)(pOutLastChLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w);
                        for(i = 0; i < s32LastChLeft; i++)
                        {
                            *pOutLastLeft = *pInLastLeft++;
                            pOutLastLeft += u32H *u32W;
                        }
                    }
                }
            }           
        } 
   
    }
    else if(pTensor->u32Precision == 8)
    {
            AR_U32 u32ChStep = u32KSizeNorm;
            AR_S32 s32WLoop = u32W/8;
            AR_S32 s32WLeft = u32W%8;
            AR_S32 s32ChLoop = u32ChStep/8;
            //AR_S32 s32ChLeft = u32ChStep%8;

            //printf("u32KSizeNorm = %d, s32WLoop = %d, s32WLeft = %d\n", u32KSizeNorm, s32WLoop, s32WLeft);
            //printf("s32ChLoop = %d, s32ChLeft = %d, s32Ksizelast = %d\n", s32ChLoop, s32ChLeft, u32KSizeLastOri);
             for(k =0; k < u32KNormNum; k++)
            {
                AR_U32 u32SrcKOffset = k * u32KStep;
                AR_U32 u32DstKOffset = k * u32DstKStep;
                for(j = 0; j < s32ChLoop; j++)
                {
                    AR_U32 u32SrcChOffset = j * 8 * u32Byte;
                    AR_U32 u32DstChOffset = j * 8 * u32W * u32H * u32Byte;
                    if(!s32WLeft)//test
                    {
                        for(b = 0;b < u32B;b++)
                        {
                            AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                            AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                            for(h = 0; h < u32H; h++)
                            {
                                AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32SrcChOffset + u32SrcKOffset + u32SrcBatchOffset + h * u32RowStep);
                                AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32DstChOffset + u32DstKOffset + u32DstBatchOffset  + h * u32W * u32Byte);
                                for(w = 0; w < s32WLoop; w++)
                                {
                                    AR_NPU_Matrix8x8_S8_Convert(pTensorIn + w * 8 * u32ChStep, pTensorOut + w * 8, u32KSizeNorm, u32W*u32H);
                                }
                            }
                        }
                    }
                    else
                    {
                        for(b = 0;b < u32B;b++)
                        {
                            AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                            AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                            for(h = 0; h < u32H; h++)
                            {
                                AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32SrcChOffset + u32SrcKOffset + u32SrcBatchOffset + h * u32RowStep);
                                AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32DstChOffset + u32DstKOffset + u32DstBatchOffset  + h * u32W * u32Byte);
                                for(w = 0; w < s32WLoop; w++)
                                {
                                    AR_NPU_Matrix8x8_S8_Convert(pTensorIn + w * 8 * u32ChStep, pTensorOut + w * 8, u32KSizeNorm, u32W*u32H);
                                }

                                //W left
                                AR_CHAR * pWInLeft = pTensorIn + 8 * s32WLoop * u32KSizeNorm;
                                AR_CHAR * pWOutLeft = pTensorOut + 8 * s32WLoop;
                                AR_CHAR * pWInLeftTemp = pWInLeft;
                                for(int w1 = 0; w1 < s32WLeft; w1++)
                                {
                                   for( i = 0; i < 8; i++)
                                   {
                                        *(pWOutLeft + i * u32W * u32H) = *pWInLeftTemp ++;
                                   }
                                   pWOutLeft++;
                                   pWInLeft += u32KSizeNorm;
                                   pWInLeftTemp = pWInLeft;
                                }
                            }
                        }
                    }
                }
            }

            AR_S32 s32LastChLoop = u32KSizeLastOri/8;
            AR_S32 s32LastChLeft = u32KSizeLastOri%8;
            //printf("s32LastChLoop = %d, s32LastChLeft = %d\n", s32LastChLoop, s32LastChLeft);
            AR_U32 u32SrcKLastOffset = u32KNormNum * u32KStep;
            AR_U32 u32DstKLastOffset = u32KNormNum * u32DstKStep;
            for(k = 0; k < s32LastChLoop; k++)
            {
                AR_U32 u32SrcChOffset = 8 * k * u32Byte;
                AR_U32 u32DstChOffset = k * 8 * u32W * u32H * u32Byte;
                if(!s32WLeft)//test
                {
                    for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_CHAR * pInLast = (AR_CHAR *)(pInput+ u32SrcKLastOffset + u32SrcChOffset + u32SrcBatchOffset + h * u32RowStep);
                            AR_CHAR * pOutLast = (AR_CHAR *)(pOutput + u32DstKLastOffset +u32DstChOffset + u32DstBatchOffset  + h * u32DstHStep);
                            for(w = 0; w < s32WLoop; w++)
                            {
                                AR_NPU_Matrix8x8_S8_Convert(pInLast + w * 8 * u32KSizeLast, pOutLast + w * 8, u32KSizeLast, u32W*u32H);
                            }
                        }
                    }
                }
                else
                {
                   for(b = 0;b < u32B;b++)
                    {
                        AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                        AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                        for (h = 0; h < u32H; h++)
                        {
                            AR_CHAR * pInLast = (AR_CHAR *)(pInput+ u32SrcKLastOffset + u32SrcChOffset + u32SrcBatchOffset + h * u32RowStep);
                            AR_CHAR * pOutLast = (AR_CHAR *)(pOutput + u32DstKLastOffset +u32DstChOffset + u32DstBatchOffset  + h * u32DstHStep);
                            for(w = 0; w < s32WLoop; w++)
                            {
                                AR_NPU_Matrix8x8_S8_Convert(pInLast + w * 8 * u32KSizeLast, pOutLast + w * 8, u32KSizeLast, u32W*u32H);
                            }

                            //W left
                            AR_CHAR * pWInLeft = pInLast + 8 * s32WLoop * u32KSizeLast;
                            AR_CHAR * pWOutLeft = pOutLast + 8 * s32WLoop;
                            AR_CHAR * pWInLeftTemp = pWInLeft;
                            for(int w1 = 0; w1 < s32WLeft; w1++)
                            {
                               for( i = 0; i < 8; i++)
                               {
                                    *(pWOutLeft + i * u32W * u32H) = *pWInLeftTemp ++;
                               }
                               pWOutLeft++;
                               pWInLeft += u32KSizeLast;
                               pWInLeftTemp = pWInLeft;
                            }
                        }
                    }
                }
            }
            if(s32LastChLeft)
            {
                AR_CHAR * pInChLastLeft = (AR_CHAR *)(pInput+ u32KNormNum * u32KStep + s32LastChLoop * 8 * u32Byte);
                AR_CHAR * pOutChLastLeft = (AR_CHAR *)(pOutput + (u32KNormNum * u32KSizeNorm * u32W * u32H + s32LastChLoop * 8 * u32W * u32H) * u32Byte);
                //printf("pInput = %p, pOutput = %p\n", pInput, pOutput);
                //printf("pInLastLeft = %p, pOutLastLeft = %p\n", pInLastLeft, pOutLastLeft);
                for(b = 0;b < u32B;b++)
                {
                    AR_U32 u32SrcBatchOffset = b * u32OutTensorStep;
                    AR_U32 u32DstBatchOffset = b * u32DstBatchStep;
                    for (h = 0; h < u32H; h++)
                    {
                        AR_U32 u32SrcHOffset = h * u32RowStep;
                        AR_U32 u32DstHOffset = h * u32W * u32Byte;
                        for(w = 0; w < u32W; w++)
                        {
                            AR_CHAR * pInLastLeft = (AR_CHAR *)(pInChLastLeft + (u32SrcBatchOffset + u32SrcHOffset)/u32Byte + w * u32KSizeLast);
                            AR_CHAR * pOutLastLeft = (AR_CHAR *)(pOutChLastLeft + (u32DstBatchOffset + u32DstHOffset)/u32Byte + w);
                            //printf("pInLastLeft = %p, pInLastLeft = %p\n", pInLastLeft, pInLastLeft);
                            //printf("pOutLastLeft = %p, pOutLastLeft = %p\n", pOutLastLeft, pOutLastLeft);
                            for(i = 0; i < s32LastChLeft; i++)
                            {
                                *pOutLastLeft = *pInLastLeft++;
                                pOutLastLeft += u32H *u32W;
                            }
                        }
                    }
                }
            }
    }

	return 0;
}

void AR_NPU_Exe_NhwcCvtNchw_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    if((!pInput) || (!pOutput) ||(!pTensor))
    {
        printf("AR_NPU_Exe_NhwcCvtNchw_Fix_Neon with err param\r\n");
		return;
	}
		
    AR_U32 u32KSizeNormal = pTensor->u32KSizeNorm;

    //printf("AR_NPU_Exe_NhwcCvtNchw_Fix_Neon\n");
    //printf("u32KSizeNormal = %d, u32W = %d,u32H = %d\n", u32KSizeNormal, pTensor->u32Width, pTensor->u32Height);
    //printf("u32Knormal_num = %d\n", pTensor->u32KNormNum);
    //printf("u32KSizeLast = %d\n", pTensor->u32KSizeLast);

    if(u32KSizeNormal == 1)
    {
        AR_NPU_Exe_One_Channel_NhwcCvtNchw_Fix_Neon(pInput, pOutput, pTensor);
    }
    else if(u32KSizeNormal == 2)
    {
        AR_NPU_Exe_Two_Channel_NhwcCvtNchw_Fix_Neon(pInput, pOutput, pTensor);
    }
    else if(u32KSizeNormal == 4)
    {
        AR_NPU_Exe_Four_Channel_NhwcCvtNchw_Fix_Neon(pInput, pOutput, pTensor);
    }
    else
    {
        AR_NPU_Exe_Long_Channel_NhwcCvtNchw_Fix_Neon(pInput, pOutput, pTensor);
    }

	return;
}

void AR_NPU_Exe_One_Channel_NchwCvtNhwc_F32_Neon(AR_FLOAT * pTensorInput, AR_FLOAT * pNhwcOutput, AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    //AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    //AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = sizeof(AR_FLOAT);
    //AR_S32 i = 0, j = 0;
    AR_FLOAT * f32Input = pTensorInput;
    AR_FLOAT * f32Output = pNhwcOutput;


    for(k = 0; k < u32KNormNum; k++)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(f32Output, f32Input, u32W* u32Byte);

            f32Input += u32W;
            f32Output += u32RowStep/u32Byte;
        }
    }

    if(u32KSizeLastOri)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(f32Output, f32Input, u32W* u32Byte);

            f32Input += u32W;
            f32Output += u32RowStep/u32Byte;
        }
    }  
}

AR_S32 AR_NPU_Exe_Two_Channel_NchwCvtNhwc_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = sizeof(AR_FLOAT);
    AR_S32 i = 0;

    //AR_U32 u32ChStep = u32KSizeNorm;
    AR_S32 u32WLoop = u32W/4;
    AR_S32 u32WLeft = u32W%4;

    AR_U32 u32InChannelStep = u32H * u32W;
    AR_U32 u32InKStep = u32H * u32W * u32KSizeNorm * u32B;
    AR_U32 u32InBatchStep = u32H * u32W * u32KSizeNorm;
    //caculate the normal tensor blocks
     for(b = 0;b < u32B;b++)
    {
        AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
        AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
        for(k =0; k < u32KNormNum; k++)
        {
            AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
            AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                AR_FLOAT * pTensorIn = (AR_FLOAT *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                AR_FLOAT * pTensorOut = (AR_FLOAT *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                float32x4x2_t f32Input;
                for(w = 0; w < u32WLoop; w++)
                {
                    f32Input.val[0] = vld1q_f32(pTensorIn + w * 4);
                    f32Input.val[1] = vld1q_f32(pTensorIn + w * 4 + u32InChannelStep);
                    vst2q_f32(pTensorOut + w * u32KSizeNorm, f32Input);
                }

                if(u32WLeft)
                {
                    //W left
                    AR_FLOAT * pWInLeft = pTensorOut + 4 * u32WLoop;
                    AR_FLOAT * pWOutLeft = pTensorIn + 4 * u32WLoop * u32KSizeNorm;
                    for(int w1 = 0; w1 < u32WLeft; w1++)
                    {
                       AR_FLOAT * pWInLeftTemp = pWInLeft + w;
                       AR_FLOAT * pWOutLeftTemp = pWOutLeft + w * u32KSizeNorm;
                       for( i = 0; i < u32KSizeNorm; i++)
                       {
                            *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                       }
                    }
                }
            } 
        }

        AR_FLOAT * f32LastIn = (AR_FLOAT *)(pInput + u32KSizeNorm * u32InKStep + b * u32InBatchStep);
        AR_FLOAT * f32LastOut = (AR_FLOAT *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
        //AR_U32 u32LastLoop = u32KSizeLastOri/2;
        AR_U32 u32LastLeft = u32KSizeLastOri%2;

        //caculate the last block
        if((!u32LastLeft) && u32KSizeLastOri)
        {
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                AR_FLOAT * f32pIn = (AR_FLOAT *)(f32LastIn + u32HInStep);
                AR_FLOAT * f32pOut = (AR_FLOAT *)(f32LastOut + u32HOutStep);
                float32x4x2_t f32In;
                for(w = 0; w < u32WLoop; w++)
               {
                    f32In.val[0] = vld1q_f32(f32pIn + w * 4);
                    f32In.val[1] = vld1q_f32(f32pIn + w * 4 + u32InChannelStep);
                    vst2q_f32(f32pOut + w * u32KSizeNorm, f32In);
               }
    
               if(u32WLeft)
               {
                   //W left
                   AR_FLOAT * pWInLeft = f32pIn + 4 * u32WLoop;
                   AR_FLOAT * pWOutLeft = f32pOut + u32HOutStep + 4 * u32WLoop * u32KSizeNorm;
                   for(int w1 = 0; w1 < u32WLeft; w1++)
                   {
                      AR_FLOAT * pWInLeftTemp = pWInLeft + w;
                      AR_FLOAT * pWOutLeftTemp = pWOutLeft + w * u32KSizeLast;
                      for(c = 0; c < u32KSizeLastOri; c++)
                      {
                           *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                      }
                   }
               }
            }
        }
        else
        {
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                AR_FLOAT * f32pIn = (AR_FLOAT *)(f32LastIn + u32HInStep);
                AR_FLOAT * f32pOut = (AR_FLOAT *)(f32LastOut + u32HOutStep);
                for(w = 0; w < u32W; w++)
                {
                    AR_FLOAT * pWInLeftTemp = f32pIn + w;
                    AR_FLOAT * pWOutLeftTemp = f32pOut + w * u32KSizeLast;
                    for(c = 0; c < u32KSizeLastOri; c++)
                    {
                        *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                    }
                }
            }
        }
    }

	return 0;
}

AR_S32 AR_NPU_Exe_Four_Channel_NchwCvtNhwc_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = sizeof(AR_FLOAT);
    AR_S32 i = 0;

    //AR_U32 u32ChStep = u32KSizeNorm;
    AR_S32 u32WLoop = u32W/4;
    AR_S32 u32WLeft = u32W%4;

    AR_U32 u32InChannelStep = u32H * u32W;
    AR_U32 u32InKStep = u32H * u32W * u32KSizeNorm * u32B;
    AR_U32 u32InBatchStep = u32H * u32W * u32KSizeNorm;
    //caculate the normal tensor blocks
     for(b = 0;b < u32B;b++)
    {
        AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
        AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
        for(k =0; k < u32KNormNum; k++)
        {
            AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
            AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                AR_FLOAT * pTensorIn = (AR_FLOAT *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                AR_FLOAT * pTensorOut = (AR_FLOAT *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                float32x4x4_t f32Input;
                for(w = 0; w < u32WLoop; w++)
                {
                    f32Input.val[0] = vld1q_f32(pTensorIn + w * 4);
                    f32Input.val[1] = vld1q_f32(pTensorIn + w * 4 + u32InChannelStep);
                    f32Input.val[2] = vld1q_f32(pTensorIn + w * 4 + 2 * u32InChannelStep);
                    f32Input.val[3] = vld1q_f32(pTensorIn + w * 4 + 3 * u32InChannelStep);
                    vst4q_f32(pTensorOut + w * 4 * u32KSizeNorm, f32Input);
                }

                if(u32WLeft)
                {
                    //W left
                    AR_FLOAT * pWInLeft = pTensorOut + 4 * u32WLoop;
                    AR_FLOAT * pWOutLeft = pTensorIn + 4 * u32WLoop * u32KSizeNorm;
                    for(int w1 = 0; w1 < u32WLeft; w1++)
                    {
                       AR_FLOAT * pWInLeftTemp = pWInLeft + w;
                       AR_FLOAT * pWOutLeftTemp = pWOutLeft + w * u32KSizeNorm;
                       for( i = 0; i < u32KSizeNorm; i++)
                       {
                            *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                       }
                    }
                }
            } 
        }

        AR_FLOAT * f32LastIn = (AR_FLOAT *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
        AR_FLOAT * f32LastOut = (AR_FLOAT *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
        //AR_U32 u32LastLoop = u32KSizeLastOri/4;
        AR_U32 u32LastLeft = u32KSizeLastOri%4;

        //printf("f32LastIn = %x, f32LastOut = %x\n", f32LastIn, f32LastOut);
        //printf("pInput = %x, pOutput = %x\n", pInput, pOutput);
        //caculate the last block
        if((!u32LastLeft) && u32KSizeLastOri)
        {
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                AR_FLOAT * f32pIn = (AR_FLOAT *)(f32LastIn + u32HInStep);
                AR_FLOAT * f32pOut = (AR_FLOAT *)(f32LastOut + u32HOutStep);
                float32x4x4_t f32In;
                for(w = 0; w < u32WLoop; w++)
               {
                    f32In.val[0] = vld1q_f32(f32pIn + w * 4);
                    f32In.val[1] = vld1q_f32(f32pIn + w * 4 + u32InChannelStep);
                    f32In.val[2] = vld1q_f32(f32pIn + w * 4 + 2 * u32InChannelStep);
                    f32In.val[3] = vld1q_f32(f32pIn + w * 4 + 3 * u32InChannelStep);
                    vst4q_f32(f32pOut + w * 4 * u32KSizeNorm, f32In);
               }
    
               if(u32WLeft)
               {
                   //W left
                   AR_FLOAT * pWInLeft = f32pIn + 4 * u32WLoop;
                   AR_FLOAT * pWOutLeft = f32pOut + u32HOutStep + 4 * u32WLoop * u32KSizeNorm;
                   for(int w1 = 0; w1 < u32WLeft; w1++)
                   {
                      AR_FLOAT * pWInLeftTemp = pWInLeft + w;
                      AR_FLOAT * pWOutLeftTemp = pWOutLeft + w * u32KSizeLast;
                      for(c = 0; c < u32KSizeLastOri; c++)
                      {
                           *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                      }
                   }
               }
            }
        }
        else
        {
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                AR_FLOAT * f32pIn = (AR_FLOAT *)(f32LastIn + u32HInStep);
                AR_FLOAT * f32pOut = (AR_FLOAT *)(f32LastOut + u32HOutStep);
                for(w = 0; w < u32W; w++)
                {
                    AR_FLOAT * pWInLeftTemp = f32pIn + w;
                    AR_FLOAT * pWOutLeftTemp = f32pOut + w * u32KSizeLast;
                    for(c = 0; c < u32KSizeLastOri; c++)
                    {
                        *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                    }
                }
            }
        }
    }

	return 0;
}

AR_S32 AR_NPU_Exe_Long_Channel_NchwCvtNhwc_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput, AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = sizeof(AR_FLOAT);
    AR_S32 i = 0;

    //printf("u32H = %d, u32W = %d, u32Precison =%d, u32KSizeNormal = %d\n", u32H, u32W, u32Byte, u32KSizeNorm);
    //printf("u32KNormalNum = %d,u32KLastSize = %d\n", u32KNormNum, u32KSizeLast);
    AR_U32 u32InChannelStep = u32H * u32W;
    AR_U32 u32InKStep = u32H * u32W * u32KSizeNorm * u32B;
    AR_U32 u32InBatchStep = u32H * u32W * u32KSizeNorm;
    AR_S32 s32WLoop = u32W/4;
    AR_S32 s32WLeft = u32W%4;
    AR_S32 s32ChLoop = u32KSizeNorm/4;
    //AR_S32 s32ChLeft = u32KSizeNorm%4;
    
    //caculate the normal tensor blocks
    for(b = 0;b < u32B;b++)
    {
        AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
        AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
        for(k =0; k < u32KNormNum; k++)
        {
            AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
            AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                for(w = 0; w < s32WLoop; w++)
                {
                    AR_U32 u32WInOffset = 4 * w;
                    AR_U32 u32WOutOffset = 4 * w * u32KSizeNorm;
                    AR_FLOAT * pTensorIn = (AR_FLOAT *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep + u32WInOffset);
                    AR_FLOAT * pTensorOut = (AR_FLOAT *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + u32WOutOffset);
                    for(c = 0; c < s32ChLoop; c++)
                    {
                        AR_NPU_Matrix4x4_F32_Convert(pTensorIn + c * 4 * u32InChannelStep, pTensorOut + c * 4, u32InChannelStep, u32KSizeNorm);
                    }
                }
                if(s32WLeft)
                {
                    //W left
                    AR_FLOAT * pWInLeft = (AR_FLOAT *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep + 4 * s32WLoop);
                    AR_FLOAT * pWOutLeft = (AR_FLOAT *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + 4 * s32WLoop * u32KSizeNorm);
                    for(int w1 = 0; w1 < s32WLeft; w1++)
                    {
                       AR_FLOAT * pWInLeftTemp = pWInLeft + w;
                       AR_FLOAT * pWOutLeftTemp = pWOutLeft + w * u32KSizeNorm;
                       for( i = 0; i < u32KSizeNorm; i++)
                       {
                            *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                       }
                    }
                }
            }
        }
    
        //caculate the last block
        AR_FLOAT * f32LastIn = (AR_FLOAT *)(pInput + u32KSizeNorm * u32InKStep + b * u32InBatchStep);
        AR_FLOAT * f32LastOut = (AR_FLOAT *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
        AR_S32 s32LastChLoop = u32KSizeLastOri/4;
        AR_S32 s32LastChLeft = u32KSizeLastOri%4;
        
        for(h = 0; h < u32H; h++)
        {
            AR_U32 u32HInStep = h * u32W;
            AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
            for(w = 0; w < s32WLoop; w++)
            {
                AR_U32 u32WInOffset = 4 * w;
                AR_U32 u32WOutOffset = 4 * w * u32KSizeNorm;
                AR_FLOAT * pTensorIn = (AR_FLOAT *)(f32LastIn +u32HInStep + u32WInOffset);
                AR_FLOAT * pTensorOut = (AR_FLOAT *)(f32LastOut + u32HOutStep + u32WOutOffset);
                for(c = 0; c < s32LastChLoop; c++)
                {
                    AR_NPU_Matrix4x4_F32_Convert(pTensorIn + c * 4 * u32InChannelStep, pTensorOut + c * 4, u32InChannelStep, u32KSizeLast);
                }

                if(s32LastChLeft)
                {
                    AR_FLOAT * pTensorLastIn = pTensorIn + 4 * u32InChannelStep * s32LastChLoop;
                    AR_FLOAT * pTensorLastOut = pTensorOut + 4 * s32LastChLoop;
                    for(c = 0; c < s32LastChLeft; c++)
                    {
                        *pTensorLastOut++ = *(pTensorLastIn + i * u32InChannelStep);
                    }
                }
            }
    
           if(s32WLeft)
           {
               //W left
               AR_FLOAT * pWInLeft = (AR_FLOAT *)(f32LastIn +u32HInStep + 4 * s32WLoop);
               AR_FLOAT * pWOutLeft = (AR_FLOAT *)(f32LastOut + u32HOutStep + 4 * s32WLoop * u32KSizeNorm);
               for(int w1 = 0; w1 < s32WLeft; w1++)
               {
                  AR_FLOAT * pWInLeftTemp = pWInLeft + w;
                  AR_FLOAT * pWOutLeftTemp = pWOutLeft + w * u32KSizeLast;
                  for( i = 0; i < u32KSizeLastOri; i++)
                  {
                       *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                  }
               }
           }
        }
    }

	return 0;
}


void AR_NPU_Exe_NchwCvtNhwc_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput, AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 u32KSizeNormal = pTensor->u32KSizeNorm;

    //printf("u32W = %d, u32H = %d, u32KSizeNormal = %d, u32KSizeLast = %d\n",
    //            pTensor->u32Width, pTensor->u32Height, pTensor->u32KSizeNorm, pTensor->u32KSizeLast);
    //printf("u32KNormalNum = %d\n", pTensor->u32KNormNum);
    if(u32KSizeNormal == 1)
    {
        AR_NPU_Exe_One_Channel_NchwCvtNhwc_F32_Neon(pInput, pOutput, pTensor);
    }
    else if(u32KSizeNormal == 2)
    {
        AR_NPU_Exe_Two_Channel_NchwCvtNhwc_F32_Neon(pInput, pOutput, pTensor);
    }
    else if(u32KSizeNormal == 4)
    {
        AR_NPU_Exe_Four_Channel_NchwCvtNhwc_F32_Neon(pInput, pOutput, pTensor);
    }
    else
    {
        AR_NPU_Exe_Long_Channel_NchwCvtNhwc_F32_Neon(pInput, pOutput, pTensor);
    }

}


void AR_NPU_Exe_One_Channel_NchwCvtNhwc_Fix_Neon(AR_CHAR * pTensorInput, AR_CHAR * pNhwcOutput, AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    //AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    //AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    //AR_S32 i = 0, j = 0;
    
    AR_CHAR * s8Input = pTensorInput;
    AR_CHAR * s8Output = pNhwcOutput;

    AR_U32 u32HStep = u32W * u32Byte;
    for(k = 0; k < u32KNormNum; k++)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(s8Output, s8Input, u32HStep);

            s8Input += u32HStep;
            s8Output += u32RowStep;
        }
    }

    if(u32KSizeLastOri)
    {
        for(h = 0; h < u32B * u32H; h++)
        {
            memcpy(s8Output, s8Input, u32HStep);

            s8Input += u32HStep;
            s8Output += u32RowStep;
        }
    }
   
}

void AR_NPU_Exe_Two_Channel_NchwCvtNhwc_Fix_Neon(AR_CHAR * pTensorInput, AR_CHAR * pNhwcOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_S32 i = 0;

    AR_U32 u32InChannelStep = u32H * u32W;
    AR_U32 u32InKStep = u32H * u32W * u32KSizeNorm * u32B;
    AR_U32 u32InBatchStep = u32H * u32W * u32KSizeNorm;
    if(pTensor->u32Precision == 16)
    {
        AR_S16 * pInput = (AR_S16 *)pTensorInput; 
        AR_S16 * pOutput = (AR_S16 *)pNhwcOutput;
        
        if(u32W < 8)
        {
            AR_U32 u32WLoop = u32W/4;
            AR_U32 u32WLeft = u32W%4;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int16x4x2_t s16In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s16In.val[0] = vld1_s16(pTensorIn + w * 4);
                            s16In.val[1] = vld1_s16(pTensorIn + w * 4 + u32InChannelStep);
                            vst2_s16(pTensorOut + w * 4 * u32KSizeNorm, s16In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_S16 * pWInLeft = pTensorIn + 4 * u32WLoop;
                            AR_S16 * pWOutLeft = pTensorOut + 4 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_S16 * pWInLeftTemp = pWInLeft + w1;
                               AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_S16 * s16LastIn = (AR_S16 *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_S16 * s16LastOut = (AR_S16 *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/2;
                AR_U32 u32LastLeft = u32KSizeLastOri%2;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        int16x4x2_t s16In;
                        for(w = 0; w < u32WLoop; w++)
                       {
                            s16In.val[0] = vld1_s16(s16pIn + w * 4);
                            s16In.val[1] = vld1_s16(s16pIn + w * 4 + u32InChannelStep);
                            vst2_s16(s16pOut + w * 4 * u32KSizeNorm, s16In);
                       }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_S16 * pWInLeft = s16pIn + 4 * u32WLoop;
                           AR_S16 * pWOutLeft = s16pOut + 4 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_S16 * pWInLeftTemp = pWInLeft + w1;
                              AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_S16 * pWInLeftTemp = s16pIn + w;
                            AR_S16 * pWOutLeftTemp = s16pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int16x8x2_t s16In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s16In.val[0] = vld1q_s16(pTensorIn + w * 8);
                            s16In.val[1] = vld1q_s16(pTensorIn + w * 8 + u32InChannelStep);
                            vst2q_s16(pTensorOut + w * 8 * u32KSizeNorm, s16In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_S16 * pWInLeft = pTensorIn + 8 * u32WLoop;
                            AR_S16 * pWOutLeft = pTensorOut + 8 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_S16 * pWInLeftTemp = pWInLeft + w1;
                               AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_S16 * s16LastIn = (AR_S16 *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_S16 * s16LastOut = (AR_S16 *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/2;
                AR_U32 u32LastLeft = u32KSizeLastOri%2;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        int16x8x2_t s16In;
                        for(w = 0; w < u32WLoop; w++)
                       {
                            s16In.val[0] = vld1q_s16(s16pIn + w * 8);
                            s16In.val[1] = vld1q_s16(s16pIn + w * 8 + u32InChannelStep);
                            vst2q_s16(s16pOut + w * 8 * u32KSizeNorm, s16In);
                       }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_S16 * pWInLeft = s16pIn + 8 * u32WLoop;
                           AR_S16 * pWOutLeft = s16pOut + u32HOutStep + 8 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_S16 * pWInLeftTemp = pWInLeft + w1;
                              AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_S16 * pWInLeftTemp = s16pIn + w;
                            AR_S16 * pWOutLeftTemp = s16pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
    }   
    else if(pTensor->u32Precision == 8)
    {
        AR_CHAR * pInput = pTensorInput;
        AR_CHAR * pOutput = pNhwcOutput;
        if(u32W < 16)
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int8x8x2_t s8In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s8In.val[0] = vld1_s8((int8_t *)pTensorIn + w * 8);
                            s8In.val[1] = vld1_s8((int8_t *)pTensorIn + w * 8 + u32InChannelStep);
                            vst2_s8((int8_t *)pTensorOut + w * 8 * u32KSizeNorm, s8In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_CHAR * pWInLeft = pTensorIn + 8 * u32WLoop;
                            AR_CHAR * pWOutLeft = pTensorOut + 8 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                               AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_CHAR * s16LastIn = (AR_CHAR *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_CHAR * s16LastOut = (AR_CHAR *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/2;
                AR_U32 u32LastLeft = u32KSizeLastOri%2;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s16pIn = (AR_CHAR *)(s16LastIn + u32HInStep);
                        AR_CHAR * s16pOut = (AR_CHAR *)(s16LastOut + u32HOutStep);
                        int8x8x2_t s8In;
                        for(w = 0; w < u32WLoop; w++)
                       {
                            s8In.val[0] = vld1_s8((int8_t *)s16pIn + w * 8);
                            s8In.val[1] = vld1_s8((int8_t *)s16pIn + w * 8 + u32InChannelStep);
                            vst2_s8((int8_t *)s16pOut + w * 8 * u32KSizeNorm, s8In);
                       }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_CHAR * pWInLeft = s16pIn + 8 * u32WLoop;
                           AR_CHAR * pWOutLeft = s16pOut + 8 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                              AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s16pIn = (AR_CHAR *)(s16LastIn + u32HInStep);
                        AR_CHAR * s16pOut = (AR_CHAR *)(s16LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_CHAR * pWInLeftTemp = s16pIn + w;
                            AR_CHAR * pWOutLeftTemp = s16pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/16;
            AR_U32 u32WLeft = u32W%16;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int8x16x2_t s8In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s8In.val[0] = vld1q_s8((int8_t *)pTensorIn + w * 16);
                            s8In.val[1] = vld1q_s8((int8_t *)pTensorIn + w * 16 + u32InChannelStep);
                            vst2q_s8((int8_t *)pTensorOut + w * 16 *u32KSizeNorm, s8In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_CHAR * pWInLeft = pTensorIn + 16 * u32WLoop;
                            AR_CHAR * pWOutLeft = pTensorOut + 16 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                               AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_CHAR * s16LastIn = (AR_CHAR *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_CHAR * s16LastOut = (AR_CHAR *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/2;
                AR_U32 u32LastLeft = u32KSizeLastOri%2;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s16pIn = (AR_CHAR *)(s16LastIn + u32HInStep);
                        AR_CHAR * s16pOut = (AR_CHAR *)(s16LastOut + u32HOutStep);
                        int8x16x2_t s8In;
                        for(w = 0; w < u32WLoop; w++)
                       {
                            s8In.val[0] = vld1q_s8((int8_t *)s16pIn + w * 16);
                            s8In.val[1] = vld1q_s8((int8_t *)s16pIn + w * 16 + u32InChannelStep);
                            vst2q_s8((int8_t *)s16pOut + w * 16 * u32KSizeNorm, s8In);
                       }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_CHAR * pWInLeft = s16pIn + 16 * u32WLoop;
                           AR_CHAR * pWOutLeft = s16pOut + 16 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                              AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s16pIn = (AR_CHAR *)(s16LastIn + u32HInStep);
                        AR_CHAR * s16pOut = (AR_CHAR *)(s16LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_CHAR * pWInLeftTemp = s16pIn + w;
                            AR_CHAR * pWOutLeftTemp = s16pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
    }
}

void AR_NPU_Exe_Four_Channel_NchwCvtNhwc_Fix_Neon(AR_CHAR * pTensorInput, AR_CHAR * pNhwcOutput, AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_S32 i = 0;

    AR_U32 u32InChannelStep = u32H * u32W;
    AR_U32 u32InKStep = u32H * u32W * u32KSizeNorm * u32B;
    AR_U32 u32InBatchStep = u32H * u32W * u32KSizeNorm;
    if(pTensor->u32Precision == 16)
    {
        AR_S16 * pInput = (AR_S16 *)pTensorInput;
        AR_S16 * pOutput = (AR_S16 *)pNhwcOutput;
        if(u32W < 8)
        {
            AR_U32 u32WLoop = u32W/4;
            AR_U32 u32WLeft = u32W%4;
            for(b = 0;b < u32B;b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k =0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                        AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                        int16x4x4_t s16Input;
                        for(w = 0; w < u32WLoop; w++)
                        {
                            s16Input.val[0] = vld1_s16(pTensorIn + w * 4);
                            s16Input.val[1] = vld1_s16(pTensorIn + w * 4 + u32InChannelStep);
                            s16Input.val[2] = vld1_s16(pTensorIn + w * 4 + 2 * u32InChannelStep);
                            s16Input.val[3] = vld1_s16(pTensorIn + w * 4 + 3 * u32InChannelStep);
                            vst4_s16(pTensorOut + w * 4 * u32KSizeNorm, s16Input);
                        }

                        if(u32WLeft)
                        {
                            //W left
                            AR_S16 * pWInLeft = pTensorIn + 4 * u32WLoop;
                            AR_S16 * pWOutLeft = pTensorOut + 4 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_S16 * pWInLeftTemp = pWInLeft + w1;
                               AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    } 
                }

                AR_S16 * s16LastIn = (AR_S16 *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_S16 * s16LastOut = (AR_S16 *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/4;
                AR_U32 u32LastLeft = u32KSizeLastOri%4;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        int16x4x4_t s16In;
                        for(w = 0; w < u32WLoop; w++)
                       {
                            s16In.val[0] = vld1_s16(s16pIn + w * 4);
                            s16In.val[1] = vld1_s16(s16pIn + w * 4 + u32InChannelStep);
                            s16In.val[2] = vld1_s16(s16pIn + w * 4 + 2 * u32InChannelStep);
                            s16In.val[3] = vld1_s16(s16pIn + w * 4 + 3 * u32InChannelStep);
                            vst4_s16(s16pOut + w * 4 * u32KSizeNorm, s16In);
                       }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_S16 * pWInLeft = s16pIn + 4 * u32WLoop;
                           AR_S16 * pWOutLeft = s16pOut + u32HOutStep + 4 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_S16 * pWInLeftTemp = pWInLeft + w1;
                              AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_S16 * pWInLeftTemp = s16pIn + w;
                            AR_S16 * pWOutLeftTemp = s16pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            //printf("u32WLoop = %d, u32WLeft = %d\n",u32WLoop, u32WLeft);
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int16x8x4_t s16In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s16In.val[0] = vld1q_s16(pTensorIn + w * 8);
                            s16In.val[1] = vld1q_s16(pTensorIn + w * 8 + u32InChannelStep);
                            s16In.val[2] = vld1q_s16(pTensorIn + w * 8 + 2 * u32InChannelStep);
                            s16In.val[3] = vld1q_s16(pTensorIn + w * 8 + 3 * u32InChannelStep);
                            vst4q_s16(pTensorOut + w * 8 * u32KSizeNorm, s16In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_S16 * pWInLeft = pTensorIn + 8 * u32WLoop;
                            AR_S16 * pWOutLeft = pTensorOut + 8 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_S16 * pWInLeftTemp = pWInLeft + w1;
                               AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_S16 * s16LastIn = (AR_S16 *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_S16 * s16LastOut = (AR_S16 *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/4;
                AR_U32 u32LastLeft = u32KSizeLastOri%4;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        int16x8x4_t s16In;
                        for(w = 0; w < u32WLoop; w++)
                        {
                           s16In.val[0] = vld1q_s16(s16pIn + w * 8);
                           s16In.val[1] = vld1q_s16(s16pIn + w * 8 + u32InChannelStep);
                           s16In.val[2] = vld1q_s16(s16pIn + w * 8 + 2 * u32InChannelStep);
                           s16In.val[3] = vld1q_s16(s16pIn + w * 8 + 3 * u32InChannelStep);
                           vst4q_s16(s16pOut + w * 8 * u32KSizeLast, s16In);
                        }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_S16 * pWInLeft = s16pIn + 8 * u32WLoop;
                           AR_S16 * pWOutLeft = s16pOut + u32HOutStep + 8 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_S16 * pWInLeftTemp = pWInLeft + w1;
                              AR_S16 * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_S16 * s16pIn = (AR_S16 *)(s16LastIn + u32HInStep);
                        AR_S16 * s16pOut = (AR_S16 *)(s16LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_S16 * pWInLeftTemp = s16pIn + w;
                            AR_S16 * pWOutLeftTemp = s16pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
    }   
    else if(pTensor->u32Precision == 8)
    {
        AR_CHAR * pInput = (AR_CHAR *)pTensorInput;
        AR_CHAR * pOutput = (AR_CHAR *)pNhwcOutput;
        if(u32W < 16)
        {
            AR_U32 u32WLoop = u32W/8;
            AR_U32 u32WLeft = u32W%8;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int8x8x4_t s8In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s8In.val[0] = vld1_s8((int8_t *)pTensorIn + w * 8);
                            s8In.val[1] = vld1_s8((int8_t *)pTensorIn + w * 8 + u32InChannelStep);
                            s8In.val[2] = vld1_s8((int8_t *)pTensorIn + w * 8 + 2 * u32InChannelStep);
                            s8In.val[3] = vld1_s8((int8_t *)pTensorIn + w * 8 + 3 * u32InChannelStep);
                            vst4_s8((int8_t *)pTensorOut + w * 8 * u32KSizeNorm, s8In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_CHAR * pWInLeft = pTensorIn + 8 * u32WLoop;
                            AR_CHAR * pWOutLeft = pTensorOut + 8 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                               AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_CHAR * s8LastIn = (AR_CHAR *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_CHAR * s8LastOut = (AR_CHAR *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/4;
                AR_U32 u32LastLeft = u32KSizeLastOri%4;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s8pIn = (AR_CHAR *)(s8LastIn + u32HInStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(s8LastOut + u32HOutStep);
                        int8x8x4_t s8In;
                        for(w = 0; w < u32WLoop; w++)
                        {
                           s8In.val[0] = vld1_s8((int8_t *)s8pIn + w * 8);
                           s8In.val[1] = vld1_s8((int8_t *)s8pIn + w * 8 + u32InChannelStep);
                           s8In.val[2] = vld1_s8((int8_t *)s8pIn + w * 8 + 2 * u32InChannelStep);
                           s8In.val[3] = vld1_s8((int8_t *)s8pIn + w * 8 + 3 * u32InChannelStep);
                           vst4_s8((int8_t *)s8pOut + w * 8 * u32KSizeNorm, s8In);
                        }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_CHAR * pWInLeft = s8pIn + 8 * u32WLoop;
                           AR_CHAR * pWOutLeft = s8pOut + 8 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                              AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s8pIn = (AR_CHAR *)(s8LastIn + u32HInStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(s8LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_CHAR * pWInLeftTemp = s8pIn + w;
                            AR_CHAR * pWOutLeftTemp = s8pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32WLoop = u32W/16;
            AR_U32 u32WLeft = u32W%16;
            for(b = 0; b < u32B; b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k = 0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                         AR_U32 u32HInStep = h * u32W;
                         AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                         AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32KInOffset +  u32BatchInStep +u32HInStep);
                         AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep);
                         int8x16x4_t s8In;
                         for(w = 0; w < u32WLoop; w++)
                         {
                            s8In.val[0] = vld1q_s8((int8_t *)pTensorIn + w * 16);
                            s8In.val[1] = vld1q_s8((int8_t *)pTensorIn + w * 16 + u32InChannelStep);
                            s8In.val[2] = vld1q_s8((int8_t *)pTensorIn + w * 16 + 2 * u32InChannelStep);
                            s8In.val[3] = vld1q_s8((int8_t *)pTensorIn + w * 16 + 3 * u32InChannelStep);
                            vst4q_s8((int8_t *)pTensorOut + w * 16 * u32KSizeNorm, s8In);
                         }

                        if(u32WLeft)
                        {
                            //W left
                            AR_CHAR * pWInLeft = pTensorIn + 16 * u32WLoop;
                            AR_CHAR * pWOutLeft = pTensorOut + 16 * u32WLoop * u32KSizeNorm;
                            for(int w1 = 0; w1 < u32WLeft; w1++)
                            {
                               AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                               AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeNorm;
                               for( i = 0; i < u32KSizeNorm; i++)
                               {
                                    *pWOutLeftTemp++ = *(pWInLeftTemp + i * u32InChannelStep);
                               }
                            }
                        }
                    }
                }
                
                AR_CHAR * s8LastIn = (AR_CHAR *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_CHAR * s8LastOut = (AR_CHAR *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                //AR_U32 u32LastLoop = u32KSizeLastOri/4;
                AR_U32 u32LastLeft = u32KSizeLastOri%4;

                //caculate the last block
                if((!u32LastLeft) && u32KSizeLastOri)
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s8pIn = (AR_CHAR *)(s8LastIn + u32HInStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(s8LastOut + u32HOutStep);
                        int8x16x4_t s8In;
                        for(w = 0; w < u32WLoop; w++)
                        {
                           s8In.val[0] = vld1q_s8((int8_t *)s8pIn + w * 16);
                           s8In.val[1] = vld1q_s8((int8_t *)s8pIn + w * 16 + u32InChannelStep);
                           s8In.val[2] = vld1q_s8((int8_t *)s8pIn + w * 16 + 2 * u32InChannelStep);
                           s8In.val[3] = vld1q_s8((int8_t *)s8pIn + w * 16 + 3 * u32InChannelStep);
                           vst4q_s8((int8_t *)s8pOut + w * 16 * u32KSizeNorm, s8In);
                        }
            
                       if(u32WLeft)
                       {
                           //W left
                           AR_CHAR * pWInLeft = s8pIn + 16 * u32WLoop;
                           AR_CHAR * pWOutLeft = s8pOut + 16 * u32WLoop * u32KSizeNorm;
                           for(int w1 = 0; w1 < u32WLeft; w1++)
                           {
                              AR_CHAR * pWInLeftTemp = pWInLeft + w1;
                              AR_CHAR * pWOutLeftTemp = pWOutLeft + w1 * u32KSizeLast;
                              for(c = 0; c < u32KSizeLastOri; c++)
                              {
                                   *pWOutLeftTemp++ = *(pWInLeftTemp + c * u32InChannelStep);
                              }
                           }
                       }
                    }
                }
                else
                {
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        AR_CHAR * s8pIn = (AR_CHAR *)(s8LastIn + u32HInStep);
                        AR_CHAR * s8pOut = (AR_CHAR *)(s8LastOut + u32HOutStep);
                        for(w = 0; w < u32W; w++)
                        {
                            AR_CHAR * pWInLeftTemp = s8pIn + w;
                            AR_CHAR * pWOutLeftTemp = s8pOut + w * u32KSizeLast;
                            for(c = 0; c < u32KSizeLastOri;c++)
                            {
                                *pWOutLeftTemp++  = *(pWInLeftTemp + c * u32InChannelStep);
                            }
                        }
                    }
                }
            }
        }
    }

}

void AR_NPU_Exe_Long_Channel_NchwCvtNhwc_Fix_Neon(AR_CHAR * pTensorInput, AR_CHAR * pNhwcOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_S32 i = 0, j = 0;

    //printf("u32H = %d, u32W = %d, u32Precison =%d, u32KSizeNormal = %d\n", u32H, u32W, u32Byte, u32KSizeNorm);
    //printf("u32KNormalNum = %d,u32KLastSize = %d\n", u32KNormNum, u32KSizeLast);
    AR_U32 u32InChannelStep = u32H * u32W;
    AR_U32 u32InKStep = u32H * u32W * u32KSizeNorm * u32B;
    AR_U32 u32InBatchStep = u32H * u32W * u32KSizeNorm;
    if(pTensor->u32Precision == 16)
    {
        AR_S16 * pInput = (AR_S16 *)pTensorInput;
        AR_S16 * pOutput = (AR_S16 *)pNhwcOutput;
        if(u32W < 8)
        {
            AR_U32 u32ChStep = u32KSizeNorm;
            AR_S32 s32WLoop = u32W/4;
            AR_S32 s32WLeft = u32W%4;
            AR_S32 s32ChLoop = u32ChStep/4;
            //AR_S32 s32ChLeft = u32ChStep%4;
     
            //caculate the normal tensor blocks
            for(b = 0;b < u32B;b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k =0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        for(w = 0; w < s32WLoop; w++)
                        {
                            AR_U32 u32WInOffset = w * 4;
                            AR_U32 u32WOutOffset = w * 4 * u32KSizeNorm;
                            AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32KInOffset + u32BatchInStep + u32HInStep + u32WInOffset);
                            AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + u32WOutOffset);
                            for(c = 0; c < s32ChLoop; c++)
                            {
                                AR_NPU_Matrix4x4_S16_Convert(pTensorIn + c * 4 * u32InChannelStep, pTensorOut + c * 4, u32InChannelStep, u32KSizeNorm);
                            }
                        }
                        if(s32WLeft)
                        {
                            for(i = 0; i < s32WLeft; i++)
                            {
                                AR_S16 * pTensorLeftIn = (AR_S16 *)(pInput + u32KInOffset + u32BatchInStep + u32HInStep + s32WLoop * 4 + i);
                                AR_S16 * pTensorLeftOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + (s32WLoop * 4 + i) * u32KSizeNorm );
                                for(c = 0; c < u32KSizeNorm; c++)
                                {
                                    *pTensorLeftOut++ = *pTensorLeftIn;
                                     pTensorLeftIn += u32InChannelStep;
                                }
                            }
                        }
                    }
                }
        
                //caculate the last block
                AR_S16 * s16LastIn = (AR_S16 *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_S16 * s16LastOut = (AR_S16 *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                AR_S32 s32LastChLoop = u32KSizeLastOri/4;
                AR_S32 s32LastChLeft = u32KSizeLastOri%4;
                
                for(h = 0; h < u32H; h++)
                {
                    AR_U32 u32HInStep = h * u32W;
                    AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                    for(w = 0; w < s32WLoop; w++)
                    {
                        AR_U32 u32WInOffset = w * 4;
                        AR_U32 u32WOutOffset = w * 4 * u32KSizeLast;
                        AR_S16 * pTensorIn = (AR_S16 *)(s16LastIn + u32HInStep + u32WInOffset);
                        AR_S16 * pTensorOut = (AR_S16 *)(s16LastOut + u32HOutStep + u32WOutOffset);
                        for(c = 0; c < s32LastChLoop; c++)
                        {
                            AR_NPU_Matrix4x4_S16_Convert(pTensorIn + c * 4 * u32InChannelStep, pTensorOut + c * 4, u32InChannelStep, u32KSizeLast);
                        }

                        if(s32LastChLeft)
                        {
                             AR_S16 * pTensorLastLeftIn = (AR_S16 *)pTensorIn + 4 * s32LastChLoop * u32InChannelStep;
                             AR_S16 * pTensorLastLeftOut = (AR_S16 *)pTensorOut + 4 * s32LastChLoop;
                             for(c = 0; c < s32LastChLeft; c++)
                             {
                                AR_S16 * s16TempIn = (AR_S16 *)pTensorLastLeftIn + c * u32InChannelStep;
                                AR_S16 * s16TempOut = (AR_S16 *)pTensorLastLeftOut + c;
                                for(j = 0; j < 4; j++)
                                {
                                   *s16TempOut  = *s16TempIn++;
                                   s16TempOut += j * u32KSizeLast;
                                }
                             }
                        }
  
                    }
   
                    if(s32WLeft)
                    {
                        for(i = 0; i < s32WLeft; i++)
                        {
                            AR_S16 * pTensorLeftIn = (AR_S16 *)(s16LastIn + u32HInStep + s32WLoop * 4 + i);
                            AR_S16 * pTensorLeftOut = (AR_S16 *)(s16LastOut + u32HOutStep + (s32WLoop * 4 + i) * u32KSizeLast);
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pTensorLeftOut++ = *pTensorLeftIn;
                                 pTensorLeftIn += u32InChannelStep;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            AR_U32 u32ChStep = u32KSizeNorm;
            AR_S32 s32WLoop = u32W/8;
            AR_S32 s32WLeft = u32W%8;
            AR_S32 s32ChLoop = u32ChStep/8;
            //AR_S32 s32ChLeft = u32ChStep%8;
     
            //caculate the normal tensor blocks
            for(b = 0;b < u32B;b++)
            {
                AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
                AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
                for(k =0; k < u32KNormNum; k++)
                {
                    AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                    AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                    for(h = 0; h < u32H; h++)
                    {
                        AR_U32 u32HInStep = h * u32W;
                        AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                        for(w = 0; w < s32WLoop; w++)
                        {
                            AR_U32 u32WInOffset = w * 8;
                            AR_U32 u32WOutOffset = w * 8 * u32KSizeNorm;
                            AR_S16 * pTensorIn = (AR_S16 *)(pInput + u32KInOffset + u32BatchInStep + u32HInStep + u32WInOffset);
                            AR_S16 * pTensorOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + u32WOutOffset);
                            for(c = 0; c < s32ChLoop; c++)
                            {
                                AR_NPU_Matrix8x8_S16_Convert(pTensorIn + c * 8 * u32InChannelStep, pTensorOut + c * 8, u32InChannelStep, u32KSizeNorm);
                            }
                        }
                        if(s32WLeft)
                        {
                            for(i = 0; i < s32WLeft; i++)
                            {
                                AR_S16 * pTensorLeftIn = (AR_S16 *)(pInput + u32KInOffset + u32BatchInStep + u32HInStep + s32WLoop * 8 + i);
                                AR_S16 * pTensorLeftOut = (AR_S16 *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + (s32WLoop * 8 + i) * u32KSizeNorm );
                                for(c = 0; c < u32KSizeNorm; c++)
                                {
                                    *pTensorLeftOut++ = *pTensorLeftIn;
                                     pTensorLeftIn += u32InChannelStep;
                                }
                            }
                        }
                    }
                }

                //caculate the last block
                AR_S16 * s16LastIn = (AR_S16 *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
                AR_S16 * s16LastOut = (AR_S16 *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
                AR_S32 s32LastChLoop = u32KSizeLastOri/8;
                AR_S32 s32LastChLeft = u32KSizeLastOri%8;
                
                for(h = 0; h < u32H; h++)
                {
                    AR_U32 u32HInStep = h * u32W;
                    AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                    for(w = 0; w < s32WLoop; w++)
                    {
                        AR_U32 u32WInOffset = w * 8;
                        AR_U32 u32WOutOffset = w * 8 * u32KSizeLast;
                        AR_S16 * pTensorIn = (AR_S16 *)(s16LastIn + u32HInStep + u32WInOffset);
                        AR_S16 * pTensorOut = (AR_S16 *)(s16LastOut + u32HOutStep + u32WOutOffset);
                        for(c = 0; c < s32LastChLoop; c++)
                        {
                            AR_NPU_Matrix8x8_S16_Convert(pTensorIn + c * 8 * u32InChannelStep, pTensorOut + c * 8, u32InChannelStep, u32KSizeLast);
                        }
                        if(s32LastChLeft)
                        {
                             AR_S16 * pTensorLastLeftIn = (AR_S16 *)pTensorIn + 8 * s32LastChLoop * u32InChannelStep;
                             AR_S16 * pTensorLastLeftOut = (AR_S16 *)pTensorOut + 8 * s32LastChLoop;
                             for(c = 0; c < s32LastChLeft; c++)
                             {
                                AR_S16 * s16TempIn = (AR_S16 *)pTensorLastLeftIn + c * u32InChannelStep;
                                AR_S16 * s16TempOut = (AR_S16 *)pTensorLastLeftOut + c;
                                for(j = 0; j < 8; j++)
                                {
                                   *s16TempOut  = *s16TempIn++;
                                   s16TempOut += u32KSizeLast;
                                }
                             }
                        }
                    }

                    if(s32WLeft)
                    {
                        for(i = 0; i < s32WLeft; i++)
                        {
                            AR_S16 * pTensorLeftIn = (AR_S16 *)(s16LastIn + u32HInStep + s32WLoop * 8 + i);
                            AR_S16 * pTensorLeftOut = (AR_S16 *)(s16LastOut + u32HOutStep + (s32WLoop * 8 + i) * u32KSizeLast);
                            for(c = 0; c < u32KSizeLastOri; c++)
                            {
                                *pTensorLeftOut++ = *pTensorLeftIn;
                                 pTensorLeftIn += u32InChannelStep;
                            }
                        }
                    }
                }
            }
        }
    }
    else if(pTensor->u32Precision == 8)
    {
        AR_CHAR * pInput = (AR_CHAR *)pTensorInput;
        AR_CHAR * pOutput = (AR_CHAR *)pNhwcOutput;
        
        AR_U32 u32ChStep = u32KSizeNorm;
        AR_S32 s32WLoop = u32W/8;
        AR_S32 s32WLeft = u32W%8;
        AR_S32 s32ChLoop = u32ChStep/8;
        //AR_S32 s32ChLeft = u32ChStep%8;
 
        //caculate the normal tensor blocks
        for(b = 0;b < u32B;b++)
        {
            AR_U32 u32BatchInStep = b * u32H *u32W * u32KSizeNorm;
            AR_U32 u32BatchOutStep = b * u32OutTensorStep/u32Byte;
            for(k =0; k < u32KNormNum; k++)
            {
                AR_U32 u32KInOffset = k * u32W * u32H * u32KSizeNorm * u32B;
                AR_U32 u32KOutOffset = k * u32KStep/u32Byte;
                for(h = 0; h < u32H; h++)
                {
                    AR_U32 u32HInStep = h * u32W;
                    AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                    for(w = 0; w < s32WLoop; w++)
                    {
                        AR_U32 u32WInOffset = w * 8;
                        AR_U32 u32WOutOffset = w * 8 * u32KSizeNorm;
                        AR_CHAR * pTensorIn = (AR_CHAR *)(pInput + u32KInOffset + u32BatchInStep + u32HInStep + u32WInOffset);
                        AR_CHAR * pTensorOut = (AR_CHAR *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + u32WOutOffset);
                        for(c = 0; c < s32ChLoop; c++)
                        {
                            AR_NPU_Matrix8x8_S8_Convert(pTensorIn + c * 8 * u32InChannelStep, pTensorOut + c * 8, u32InChannelStep, u32KSizeNorm);
                        }
                    }
                    if(s32WLeft)
                    {
                        for(i = 0; i < s32WLeft; i++)
                        {
                            AR_CHAR * pTensorLeftIn = (AR_CHAR *)(pInput + u32KInOffset + u32BatchInStep + u32HInStep + s32WLoop * 8 + i);
                            AR_CHAR * pTensorLeftOut = (AR_CHAR *)(pOutput + u32KOutOffset + u32BatchOutStep + u32HOutStep + (s32WLoop * 8 + i) * u32KSizeNorm );
                            for(c = 0; c < u32KSizeNorm; c++)
                            {
                                *pTensorLeftOut++ = *pTensorLeftIn;
                                 pTensorLeftIn += u32InChannelStep;
                            }
                        }
                    }
                }
            }

            //caculate the last block
            AR_CHAR * s8LastIn = (AR_CHAR *)(pInput + u32KNormNum * u32InKStep + b * u32InBatchStep);
            AR_CHAR * s8LastOut = (AR_CHAR *)(pOutput + u32KNormNum * u32KStep/u32Byte + b * u32OutTensorStep/u32Byte);
            AR_S32 s32LastChLoop = u32KSizeLastOri/8;
            AR_S32 s32LastChLeft = u32KSizeLastOri%8;
            
            for(h = 0; h < u32H; h++)
            {
                AR_U32 u32HInStep = h * u32W;
                AR_U32 u32HOutStep = h * u32RowStep/u32Byte;
                for(w = 0; w < s32WLoop; w++)
                {
                    AR_U32 u32WInOffset = w * 8;
                    AR_U32 u32WOutOffset = w * 8 * u32KSizeLast;
                    AR_CHAR * pTensorIn = (AR_CHAR *)(s8LastIn + u32HInStep + u32WInOffset);
                    AR_CHAR * pTensorOut = (AR_CHAR *)(s8LastOut + u32HOutStep + u32WOutOffset);
                    for(c = 0; c < s32LastChLoop; c++)
                    {
                        AR_NPU_Matrix8x8_S8_Convert(pTensorIn + c * 8 * u32InChannelStep, pTensorOut + c * 8, u32InChannelStep, u32KSizeLast);
                    }

                    if(s32LastChLeft)
                    {
                        AR_CHAR * pTensorLastLeftIn = (AR_CHAR *)pTensorIn + 8 * s32LastChLoop * u32InChannelStep;
                        AR_CHAR * pTensorLastLeftOut = (AR_CHAR *)pTensorOut + 8 * s32LastChLoop;
                        for(c = 0; c < s32LastChLeft; c++)
                        {
                            AR_CHAR * s8TempIn = (AR_CHAR *)(pTensorLastLeftIn + c * u32InChannelStep);
                            AR_CHAR * s8TempOut = (AR_CHAR *)(pTensorLastLeftOut + c);
                            for(j = 0; j < 8; j++)
                            {
                                *s8TempOut  = *s8TempIn++;
                                s8TempOut +=  u32KSizeLast;
                            }
                         }
                    }
                }

                if(s32WLeft)
                {
                    for(i = 0; i < s32WLeft; i++)
                    {
                        AR_CHAR * pTensorLeftIn = (AR_CHAR *)(s8LastIn + u32HInStep + s32WLoop * 8 + i);
                        AR_CHAR * pTensorLeftOut = (AR_CHAR *)(s8LastOut + u32HOutStep + (s32WLoop * 8 + i) * u32KSizeLast);
                        for(c = 0; c < u32KSizeLastOri; c++)
                        {
                            *pTensorLeftOut++ = *pTensorLeftIn;
                             pTensorLeftIn += u32InChannelStep;
                        }
                    }
                }
            }
        }
    }
}

void AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    if((!pInput) || (!pOutput) ||(!pTensor))
    {
        printf("AR_NPU_Exe_NchwCvtNhwc_Fix_Neon with err param\r\n");
		return;
	}
	
    AR_U32 u32KSizeNormal = pTensor->u32KSizeNorm;

    //printf("AR_NPU_Exe_NchwCvtNhwc_Fix_Neon\n");
    //printf("u32KnormalNum = %d, u32KSizeNormal = %d\n", pTensor->u32KNormNum, u32KSizeNormal);
    //printf("u32W = %d, u32H =%d\n", pTensor->u32Width, pTensor->u32Height);
    //printf("u32KLastSize = %d\n", pTensor->u32KSizeLast);
    //printf("u32Precision = %d\n", pTensor->u32Precision);
    if(u32KSizeNormal == 1)
    {
        AR_NPU_Exe_One_Channel_NchwCvtNhwc_Fix_Neon(pInput, pOutput, pTensor);
    }
    else if(u32KSizeNormal == 2)
    {
        AR_NPU_Exe_Two_Channel_NchwCvtNhwc_Fix_Neon(pInput, pOutput, pTensor);
    }
    else if(u32KSizeNormal == 4)
    {
        AR_NPU_Exe_Four_Channel_NchwCvtNhwc_Fix_Neon(pInput, pOutput, pTensor);
    }
    else
    {
        AR_NPU_Exe_Long_Channel_NchwCvtNhwc_Fix_Neon(pInput, pOutput, pTensor);
    }
}

//with nhwc->nchw
AR_S32 AR_NPU_Exe_One_Channel_Softmax_Fix_NhwcCvtNchw_Neon(AR_CHAR * pInput, AR_FLOAT * pOutput, AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 c, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    //AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    AR_U32 u32KStep = pTensor->u32KStep;
    //AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    //AR_FLOAT dScaleFactor = pTensor->dScaleFactor;
    AR_S32 s32ZeroPoint = pTensor->s32ZeroPoint;
    //AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    AR_U32 u32Byte = pTensor->u32Precision/8;

    if(u32H == 1 && u32W == 1)
    {
        if(pTensor->u32Precision == 16)
        {
            AR_S16 * s16In = (AR_S16 *)pInput;
            AR_FLOAT * f32Out = (AR_FLOAT *)pOutput;
            AR_FLOAT fSum = 0.0;
            AR_U32 u32ChLoop = u32KSizeNorm/4;
            AR_U32 u32ChLeft = u32KSizeNorm%4;
            AR_U32 u32ChLastLoop = u32KSizeLastOri/4;
            AR_U32 u32ChLastLeft = u32KSizeLastOri%4;

            int32x4_t s32_zero_point = vdupq_n_s32(s32ZeroPoint);

            AR_FLOAT fSumLeft = 0.0;
            int32x4_t s32_sum = vdupq_n_s32(0);

            for(k = 0; k < u32KNormNum; k++)
            {
                for(c = 0; c < u32ChLoop; c++)
                {
                   int16x4_t s16_temp = vld1_s16((int16_t *)(pInput + 4 * c * u32Byte + k * u32KStep));
                   int32x4_t s32_cast = vsubq_s32(vmovl_s16(s16_temp), s32_zero_point);
                   s32_sum = vaddq_s32(s32_sum, s32_cast);
                }

                s16In = (AR_S16 *)(pInput + 4 * c * u32Byte + k * u32KStep);
                for(c = 0; c < u32ChLeft; c++)
                {
                    fSumLeft += (*s16In - s32ZeroPoint);
                    s16In++;
                }
            }

            for(c = 0; c < u32ChLastLoop; c++)
            {
               int16x4_t s16_temp = vld1_s16((int16_t *)(pInput + 4 * c * u32Byte + u32KNormNum * u32KStep));
               int32x4_t s32_cast = vsubq_s32(vmovl_s16(s16_temp), s32_zero_point) ;
               s32_sum = vaddq_s32(s32_sum, s32_cast);
            }

            s16In = (AR_S16 *)(pInput + 4 * c * u32Byte + u32KNormNum * u32KStep);
            for(c = 0; c < u32ChLastLeft; c++)
            {
                fSumLeft += (*s16In - s32ZeroPoint);
                s16In++;
            }

            fSum = fSumLeft + vgetq_lane_s32(s32_sum, 0) + vgetq_lane_s32(s32_sum, 1) + vgetq_lane_s32(s32_sum, 2) + vgetq_lane_s32(s32_sum, 3);

            float32x4_t f32_sum = vdupq_n_f32(1.0/fSum);
            for(k = 0; k < u32KNormNum; k++)
            {
                for(c = 0; c < u32ChLoop; c++)
                {
                       int32x4_t s32_input = vmovl_s16(vld1_s16((int16_t *)(pInput + 4 * c * u32Byte + k * u32KStep)));
                       float32x4_t f32_temp = vcvtq_f32_s32(vsubq_s32(s32_input, s32_zero_point));
                       float32x4_t result = vmulq_f32( f32_temp, f32_sum);
                       vst1q_f32((float32_t *)f32Out, result);
                       f32Out += 4;
                }

                s16In = (AR_S16 *)(pInput + 4 * c * u32Byte + k * u32KStep);
                for(c = 0; c < u32ChLeft; c++)
                {
                    *f32Out++ = (*s16In - s32ZeroPoint)/fSum;
                    s16In++;
                }
            }

            for(c = 0; c < u32ChLastLoop; c++)
            {
                   int32x4_t s32_input = vmovl_s16(vld1_s16((int16_t *)(pInput + 4 * c * u32Byte + u32KNormNum * u32KStep)));
                   float32x4_t f32_temp = vcvtq_f32_s32(vsubq_s32(s32_input, s32_zero_point));
                   float32x4_t result = vmulq_f32( f32_temp, f32_sum);
                   vst1q_f32((float32_t *)f32Out, result);
                   f32Out += 4;
            }

            s16In = (AR_S16 *)(pInput + 4 * c * u32Byte + u32KNormNum * u32KStep);
            for(c = 0; c < u32ChLastLeft; c++)
            {
                *f32Out++ = (*s16In - s32ZeroPoint)/fSum;
                s16In++;
            }
        }
        else
        {
            AR_CHAR * s8In = (AR_CHAR *)pInput;
            AR_FLOAT * f32Out = (AR_FLOAT *)pOutput;
            AR_FLOAT fSum = 0.0;
            AR_U32 u32ChLoop = u32KSizeNorm/8;
            AR_U32 u32ChLeft = u32KSizeNorm%8;
            AR_U32 u32ChLastLoop = u32KSizeLastOri/8;
            AR_U32 u32ChLastLeft = u32KSizeLastOri%8;

            int32x4_t s32_zero_point = vdupq_n_s32(s32ZeroPoint);

            if(u32H == 1 && u32W == 1)
            {
                AR_FLOAT fSumLeft = 0.0;
                int32x4_t s32_sum = vdupq_n_s32(0);

                for(k = 0; k < u32KNormNum; k++)
                {
                    for(c = 0; c < u32ChLoop; c++)
                    {
                       int8x8_t s8_temp = vld1_s8((int8_t *)(pInput + 8 * c * u32Byte + k * u32KStep));
                       int16x8_t s16_cast = vmovl_s8(s8_temp);
                       int16x4_t s16_cast_low = vget_low_s16(s16_cast);
                       int16x4_t s16_cast_high = vget_high_s16(s16_cast);
                       int32x4_t s32_cast_low = vsubq_s32(vmovl_s16(s16_cast_low), s32_zero_point);
                       int32x4_t s32_cast_high = vsubq_s32(vmovl_s16(s16_cast_high), s32_zero_point);

                       s32_sum = vaddq_s32(vaddq_s32(s32_sum, s32_cast_low), s32_cast_high);
                    }

                    s8In = (AR_CHAR *)(pInput + 8 * c * u32Byte + k * u32KStep);
                    for(c = 0; c < u32ChLeft; c++)
                    {
                        fSumLeft += (*s8In - s32ZeroPoint);
                        s8In++;
                    }
                }

                for(c = 0; c < u32ChLastLoop; c++)
                {
                   int8x8_t s8_temp = vld1_s8((int8_t *)(pInput + 8 * c * u32Byte + u32KNormNum * u32KStep));
                   int16x8_t s16_cast = vmovl_s8(s8_temp);
                   int16x4_t s16_cast_low = vget_low_s16(s16_cast);
                   int16x4_t s16_cast_high = vget_high_s16(s16_cast);
                   int32x4_t s32_cast_low = vsubq_s32(vmovl_s16(s16_cast_low), s32_zero_point);
                   int32x4_t s32_cast_high = vsubq_s32(vmovl_s16(s16_cast_high), s32_zero_point);

                   s32_sum = vaddq_s32(vaddq_s32(s32_sum, s32_cast_low), s32_cast_high);
                }

                s8In = (AR_CHAR *)(pInput + 8 * c * u32Byte + u32KNormNum * u32KStep);
                for(c = 0; c < u32ChLastLeft; c++)
                {
                    fSumLeft += (*s8In - s32ZeroPoint);
                    s8In++;
                }

                fSum = fSumLeft + vgetq_lane_s32(s32_sum, 0) + vgetq_lane_s32(s32_sum, 1) + vgetq_lane_s32(s32_sum, 2) + vgetq_lane_s32(s32_sum, 3);

                float32x4_t f32_sum = vdupq_n_f32(1.0/fSum);
                for(k = 0; k < u32KNormNum; k++)
                {
                    for(c = 0; c < u32ChLoop; c++)
                    {
                        int8x8_t s8_temp = vld1_s8((int8_t *)(pInput + 8 * c * u32Byte + k * u32KStep));
                        int16x8_t s16_cast = vmovl_s8(s8_temp);
                        int16x4_t s16_cast_low = vget_low_s16(s16_cast);
                        int16x4_t s16_cast_high = vget_high_s16(s16_cast);
                        int32x4_t s32_cast_low = vsubq_s32(vmovl_s16(s16_cast_low), s32_zero_point);
                        int32x4_t s32_cast_high = vsubq_s32(vmovl_s16(s16_cast_high), s32_zero_point);
                        float32x4_t f32_cast_low = vcvtq_f32_s32(s32_cast_low);
                        float32x4_t f32_cast_high = vcvtq_f32_s32(s32_cast_high);

                        float32x4_t result_low = vmulq_f32( f32_cast_low, f32_sum);
                        float32x4_t result_high = vmulq_f32( f32_cast_high, f32_sum);

                        vst1q_f32((float32_t *)f32Out, result_low);
                        vst1q_f32((float32_t *)f32Out + 4, result_high);
                        f32Out += 8;
                    }

                    s8In = (AR_CHAR *)(pInput + 8 * c * u32Byte + k * u32KStep);
                    for(c = 0; c < u32ChLeft; c++)
                    {
                        *f32Out++ = (*s8In - s32ZeroPoint)/fSum;
                        s8In++;
                    }
                }

                for(c = 0; c < u32ChLastLoop; c++)
                {
                    int8x8_t s8_temp = vld1_s8((int8_t *)(pInput + 8 * c * u32Byte + u32KNormNum * u32KStep));
                    int16x8_t s16_cast = vmovl_s8(s8_temp);
                    int16x4_t s16_cast_low = vget_low_s16(s16_cast);
                    int16x4_t s16_cast_high = vget_high_s16(s16_cast);
                    int32x4_t s32_cast_low = vsubq_s32(vmovl_s16(s16_cast_low), s32_zero_point);
                    int32x4_t s32_cast_high = vsubq_s32(vmovl_s16(s16_cast_high), s32_zero_point);
                    float32x4_t f32_cast_low = vcvtq_f32_s32(s32_cast_low);
                    float32x4_t f32_cast_high = vcvtq_f32_s32(s32_cast_high);

                    float32x4_t result_low = vmulq_f32( f32_cast_low, f32_sum);
                    float32x4_t result_high = vmulq_f32( f32_cast_high, f32_sum);

                    vst1q_f32((float32_t *)f32Out, result_low);
                    vst1q_f32((float32_t *)f32Out + 4, result_high);
                    f32Out += 8;
                }

                s8In = (AR_CHAR *)(pInput + 8 * c * u32Byte + u32KNormNum * u32KStep);
                for(c = 0; c < u32ChLastLeft; c++)
                {
                    *f32Out++ = (*s8In - s32ZeroPoint)/fSum;
                    s8In++;
                }
            }
        }
    }

    return 0;
}

AR_S32 AR_NPU_Exe_Softmax_F32_NhwcCvtNchw_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    //AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KSizeLastOri = pTensor->u32OriChannels - u32KSizeNorm * u32KNormNum;
    //AR_U32 u32KStep = pTensor->u32KStep;
    //AR_U32 u32RowStep = pTensor->u32RowStep;
    //AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    //AR_U32 u32Byte = pTensor->u32Precision/8;
    AR_FLOAT * pInC = pInput;
    AR_FLOAT * pOutC = pOutput;

    //w=1,h=1 for classfication
    //w=m,h=n for detection
    if(u32H == 1 && u32W == 1)
    {
        AR_FLOAT fSum = 0.0;
        AR_U32 u32ChLoop = pTensor->u32OriChannels/4;
        AR_U32 u32ChLeft = pTensor->u32OriChannels%4;

        float32x4_t f32_sum = vdupq_n_f32(0.0);

        for(c = 0; c < u32ChLoop; c++)
        {
               float32x4_t f32_temp = vld1q_f32((float32_t *)pInC);
               f32_sum = vaddq_f32(f32_sum, f32_temp);
               pInC += 4;
        }
        fSum = vgetq_lane_f32(f32_sum, 0) + vgetq_lane_f32(f32_sum, 1) + vgetq_lane_f32(f32_sum, 2) + vgetq_lane_f32(f32_sum, 3);
        for(c = 0; c < u32ChLeft; c++)
        {
            fSum += *pInC++;
        }

        pInC = pInput;
        pOutC = pOutput;
        f32_sum = vdupq_n_f32(1.0/fSum);
        
        for(c = 0; c < u32ChLoop; c++)
        {
               float32x4_t f32_temp = vld1q_f32((float32_t *)pInC);
               float32x4_t result = vmulq_f32( f32_temp, f32_sum);
               vst1q_f32((float32_t *)pOutC, result);
               pInC += 4;
               pOutC += 4;
        }

        for(c = 0; c < u32ChLeft; c++)
        {
            *pOutC++ = *pInC++/fSum;
        }
    }
    else
    {
        AR_U32 u32WLoop = u32W/4;
        AR_U32 u32WLeft =  u32W%4;
        AR_U32 u32BatchStep = u32W * u32H * u32KSizeNorm;

        if(u32WLeft)
        {
            u32WLoop += 1;
        }

        for(b = 0; b < u32B; b++)
        {
            if (!u32WLeft)
            {
                for(h = 0; h < u32H; h++)
                {
                    for(w = 0; w < u32WLoop; w++)
                    {
                        float32x4_t sum = vdupq_n_f32(0.0);
                        pInC = pInput + b * u32W * u32H * u32KSizeNorm + h * u32W + w * 4;
                        pOutC = pOutput + b * u32W * u32H * u32KSizeNorm + h * u32W + w * 4;

                        for(k = 0; k < u32KNormNum; k++)
                        {
                            for(c = 0; c < u32KSizeNorm; c++)
                            {
                                float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + k * u32BatchStep * u32B));
                                sum = vaddq_f32(sum, load);
                            }
                        }

                        for(c = 0; c < u32KSizeLastOri; c++)
                        {
                                float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B));
                                sum = vaddq_f32(sum, load);
                        }

                        float32_t f32_sum0 = vgetq_lane_f32(sum, 0);
                        float32_t f32_sum1 = vgetq_lane_f32(sum, 1);
                        float32_t f32_sum2 = vgetq_lane_f32(sum, 2);
                        float32_t f32_sum3 = vgetq_lane_f32(sum, 3);

                        float32x4_t f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum0), f32_sum_recpe, 0);
                        f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum1), f32_sum_recpe, 1);
                        f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum2), f32_sum_recpe, 2);
                        f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum3), f32_sum_recpe, 3);

                        for(k = 0; k < u32KNormNum; k++)
                        {
                            for(c = 0; c < u32KSizeNorm; c++)
                            {
                                float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + k * u32BatchStep * u32B));
                                //float32x4_t f32_sum_recpe = vrecpeq_f32(sum);
                                float32x4_t result = vmulq_f32( load, f32_sum_recpe);
                                vst1q_f32((float32_t *)(pOutC + c * u32W * u32H + k * u32BatchStep * u32B), result);
                            }
                        }

                        for(c = 0; c < u32KSizeLastOri; c++)
                        {
                            float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B));
                            float32x4_t result = vmulq_f32( load, f32_sum_recpe);
                            vst1q_f32((float32_t *)(pOutC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B), result);
                        }
                    }
                }
            }
            else
            {
                for(h = 0; h < u32H -1; h++)
                {
                    for(w = 0; w < u32WLoop; w++)
                    {
                        float32x4_t sum = vdupq_n_f32(0.0);
                        pInC = pInput + b * u32W * u32H * u32KSizeNorm + h * u32W + w * 4;
                        pOutC = pOutput + b * u32W * u32H * u32KSizeNorm + h * u32W + w * 4;

                        for(k = 0; k < u32KNormNum; k++)
                        {
                            for(c = 0; c < u32KSizeNorm; c++)
                            {
                                float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + k * u32BatchStep * u32B));
                                sum = vaddq_f32(sum, load);
                            }
                        }

                        for(c = 0; c < u32KSizeLastOri; c++)
                        {
                            float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B));
                            sum = vaddq_f32(sum, load);
                        }

                        float32_t f32_sum0 = vgetq_lane_f32(sum, 0);
                        float32_t f32_sum1 = vgetq_lane_f32(sum, 1);
                        float32_t f32_sum2 = vgetq_lane_f32(sum, 2);
                        float32_t f32_sum3 = vgetq_lane_f32(sum, 3);

                        float32x4_t f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum0), f32_sum_recpe, 0);
                        f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum1), f32_sum_recpe, 1);
                        f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum2), f32_sum_recpe, 2);
                        f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum3), f32_sum_recpe, 3);

                        for(k = 0; k < u32KNormNum; k++)
                        {
                            for(c = 0; c < u32KSizeNorm; c++)
                            {
                                float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + k * u32BatchStep * u32B));
                                float32x4_t result = vmulq_f32( load, f32_sum_recpe);
                                vst1q_f32((float32_t *)(pOutC + c * u32W * u32H + k * u32BatchStep * u32B), result);
                            }
                        }

                        for(c = 0; c < u32KSizeLastOri; c++)
                        {
                            float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B));
                            float32x4_t result = vmulq_f32( load, f32_sum_recpe);
                            vst1q_f32((float32_t *)(pOutC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B), result);
                        }
                    }
                }

                for(w = 0; w < u32WLoop - 1; w++)
                {
                    float32x4_t sum = vdupq_n_f32(0.0);
                    pInC = pInput + b * u32W * u32H * u32KSizeNorm + h * u32W + w * 4;
                    pOutC = pOutput + b * u32W * u32H * u32KSizeNorm + h * u32W + w * 4;

                    for(k = 0; k < u32KNormNum; k++)
                    {
                        for(c = 0; c < u32KSizeNorm; c++)
                        {
                            float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + k * u32BatchStep * u32B));
                            sum = vaddq_f32(sum, load);
                        }
                    }

                    float32_t f32_sum0 = vgetq_lane_f32(sum, 0);
                    float32_t f32_sum1 = vgetq_lane_f32(sum, 1);
                    float32_t f32_sum2 = vgetq_lane_f32(sum, 2);
                    float32_t f32_sum3 = vgetq_lane_f32(sum, 3);

                    float32x4_t f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum0), f32_sum_recpe, 0);
                    f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum1), f32_sum_recpe, 1);
                    f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum2), f32_sum_recpe, 2);
                    f32_sum_recpe = vsetq_lane_f32((1.0/f32_sum3), f32_sum_recpe, 3);

                    for(c = 0; c < u32KSizeLastOri; c++)
                    {
                        float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B));
                        sum = vaddq_f32(sum, load);
                    }

                    for(k = 0; k < u32KNormNum; k++)
                    {
                        for(c = 0; c < u32KSizeNorm; c++)
                        {
                            float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + k * u32BatchStep * u32B));
                            float32x4_t result = vmulq_f32( load, f32_sum_recpe);
                            vst1q_f32((float32_t *)(pOutC + c * u32W * u32H + k * u32BatchStep * u32B), result);
                        }
                    }

                    for(c = 0; c < u32KSizeLastOri; c++)
                    {
                        float32x4_t load = vld1q_f32((float32_t *)(pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B));
                        float32x4_t result = vmulq_f32( load, f32_sum_recpe);
                        vst1q_f32((float32_t *)(pOutC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B), result);
                    }
                }

                for(AR_S32 w1 = 0; w1 < u32WLeft; w1++)
                {
                    AR_FLOAT dSum = 0;
                    for(k = 0; k < u32KNormNum; k++)
                    {
                        for(c = 0; c < u32KSizeNorm; c++)
                        {
                             AR_FLOAT * pIn = pInC + c * u32W * u32H + k * u32BatchStep * u32B + 4 * (u32WLoop - 1) + w1;
                             dSum += *pIn;
                        }
                    }
                    for(c = 0; c < u32KSizeLastOri; c++)
                    {
                         AR_FLOAT * pIn = pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B + 4 * (u32WLoop - 1) + w1;
                         dSum += *pIn;
                    }

                    for(k = 0; k < u32KNormNum; k++)
                    {
                        for(c = 0; c < u32KSizeNorm; c++)
                        {
                             AR_FLOAT * pIn = pInC + c * u32W * u32H + k * u32BatchStep * u32B + 4 * (u32WLoop - 1) + w1;
                             AR_FLOAT * pOut = pOutC + c * u32W * u32H + k * u32BatchStep * u32B + 4 * (u32WLoop - 1) + w1;
                             *pOut = *pIn/dSum;
                        }
                    }
                    for(c = 0; c < u32KSizeLastOri; c++)
                    {
                         AR_FLOAT * pIn = pInC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B + 4 * (u32WLoop - 1) + w1;
                         AR_FLOAT * pOut = pOutC + c * u32W * u32H + u32KNormNum * u32BatchStep * u32B + 4 * (u32WLoop - 1) + w1;
                         *pOut = *pIn/dSum;
                    }
                }
            }
        }
    }

	return 0;
}
#else
AR_S32 AR_NPU_Exe_Softmax_F32_NhwcCvtNchw(AR_FLOAT * pInput, AR_FLOAT * pOutput,AR_NPU_TENSOR_S * pTensor)
{
    AR_U32 h, w, c, b, k;
    AR_U32 u32K = 0;
    AR_U32 u32KSizeNorm = pTensor->u32KSizeNorm;
    AR_U32 u32KNormNum = pTensor->u32KNormNum;
    AR_U32 u32KSizeLast = pTensor->u32KSizeLast;
    AR_U32 u32KStep = pTensor->u32KStep;
    AR_U32 u32RowStep = pTensor->u32RowStep;
    AR_U32 u32OutTensorStep = pTensor->u32TensorStep;
    AR_U32 u32B = pTensor->u32Num;
    AR_U32 u32H = pTensor->u32Height;
    AR_U32 u32W = pTensor->u32Width;
    AR_U32 u32C = u32KNormNum * u32KSizeNorm + u32KSizeLast;
    AR_U32 u32Byte = pTensor->u32Precision/8;

    AR_FLOAT * ps32Val = (AR_FLOAT *)malloc(u32C * sizeof(AR_FLOAT));
	if(!ps32Val)
	{
		printf("AR_NPU_Exe_Softmax_F32_NhwcCvtNchw malloc ps32Val failed!\n");
		free(ps32Val);
		return -1;
	}
	memset(ps32Val, 0, u32C * sizeof(AR_FLOAT));
    AR_FLOAT * pInC = pInput;
    AR_FLOAT * pOutC = pOutput;
    for(b = 0; b < u32B; b++)
    {
        for(h = 0; h < u32H; h++)
        {
            //normally they are in the same cacheline
            for(w = 0; w < u32W; w++)
            {
                AR_FLOAT dSum = 0.0;
                AR_FLOAT * pVal = ps32Val;

                for(k = 0; k < pTensor->u32KNormNum; k++)
                {
                    pInC = pInput + (b * u32OutTensorStep + k * u32KStep + h * u32RowStep )/u32Byte + w * u32KSizeNorm;
                    for(c = 0; c < u32KSizeNorm; c++)
                    {
                        *pVal = *pInC++;
                        dSum += *pVal;
                        pVal++;
                    }
                }

                if(u32KSizeLast)
                {
                    pInC = pInput + (b * u32OutTensorStep + u32KStep * u32KNormNum + h * u32RowStep)/u32Byte + w * u32KSizeNorm;
                    for(c = 0; c < u32KSizeLast; c++)
                    {
                        *pVal = *pInC++;
                        dSum += *pVal;
                        pVal++;
                    }
                }

                pVal = ps32Val;
                for(k = 0; k < u32KNormNum; k++)
                {
                    for(c = 0; c < u32KSizeNorm; c++)
                    {
                        pOutC = pOutput + (b * u32KSizeNorm * u32W * u32H + k * u32W * u32H * u32KSizeNorm * u32B + u32W * u32H * c + h * u32W + w);
                        *pOutC = *pVal++/dSum;
                    }
                }

                if(u32KSizeLast)
                {
                    for(c = 0; c < u32KSizeLast; c++)
                    {
                        pOutC = pOutput + (b * u32KSizeNorm * u32W * u32H + u32KNormNum * u32W * u32H * u32KSizeNorm * u32B + u32W * u32H * c + h * u32W + w);
                        *pOutC = *pVal++/dSum;
                    }
                }
            }
        }
    }
    free(ps32Val);

}

#endif

//call ARM to do softmax
AR_S32 AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon(AR_CHAR* pcNpuOutput, AR_CHAR * pcSoftmaxOutput, AR_NPU_TENSOR_S* pstTensor)
{
    AR_CHAR *InputAddr = pcNpuOutput;
    AR_FLOAT *OutputAddr = (AR_FLOAT *)pcSoftmaxOutput;
    //AR_FLOAT * fSoftIn = NULL;
    AR_U32 u32DataNum = 0;
    AR_U32 u32PerByte = pstTensor->u32Precision/8;

    if((!pcNpuOutput) ||(!pcSoftmaxOutput) || (!pstTensor))
    {
        printf("AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon Input With Error Param\n");
        return -1;
    }
 
    u32DataNum = pstTensor->u32Height * pstTensor->u32Width * (pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast);
#ifdef NHWC_TO_NCHW_NEON
    AR_FLOAT * fSoftIn = (AR_FLOAT *)malloc(u32DataNum * sizeof(AR_FLOAT));
	if(!fSoftIn)
	{
		printf("AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon malloc fSoftIn failed!\n");
		free(fSoftIn);
		return -1;
	}
    memset(fSoftIn, 0, u32DataNum * sizeof(AR_FLOAT));

    if(!strcmp(pstTensor->achLayoutType, "integer"))
    {
        if((pstTensor->u32Width == 1) && (pstTensor->u32Height == 1))
        {
            AR_NPU_Exe_One_Channel_Softmax_Fix_NhwcCvtNchw_Neon(InputAddr, OutputAddr, pstTensor);
            free(fSoftIn);
            return 0;
        }
        else
        {
            AR_CHAR * pcSoftTemp = (AR_CHAR *)malloc(u32DataNum * u32PerByte);
			if(!pcSoftTemp)
			{
				printf("AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon malloc pcSoftTemp failed!\n");
				free(pcSoftTemp);
				free(fSoftIn);
				return -1;
			}
            memset(pcSoftTemp, 0, u32DataNum * u32PerByte);
            AR_NPU_Exe_NhwcCvtNchw_Fix_Neon(InputAddr , pcSoftTemp, pstTensor);
            AR_NPU_FixCvtFloat(pcSoftTemp, fSoftIn, u32DataNum, pstTensor);
            free(pcSoftTemp);
        }
    }
    else if(!strcmp(pstTensor->achLayoutType, "float"))
    {
       AR_NPU_Exe_NhwcCvtNchw_F32_Neon((AR_FLOAT *)InputAddr, fSoftIn, pstTensor);
    }
    AR_NPU_Exe_Softmax_F32_NhwcCvtNchw_Neon(fSoftIn, (AR_FLOAT *)OutputAddr, pstTensor);
    free(fSoftIn);
#else
    //do softmax
    if(!strcmp(pstTensor->achLayoutType, "integer"))
    {
        fSoftIn = (AR_FLOAT *)malloc(u32DataNum * sizeof(AR_FLOAT));
		if(!fSoftIn)
		{
			printf("AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon malloc fSoftIn failed!\n");
			free(fSoftIn);
			return -1;
		}
    	memset(fSoftIn, 0, u32DataNum * sizeof(AR_FLOAT));
        AR_NPU_FixCvtFloat(InputAddr, fSoftIn, u32DataNum, pstTensor);
        AR_NPU_Exe_Softmax_F32_NhwcCvtNchw(fSoftIn, (AR_FLOAT *)OutputAddr, pstTensor);
        free(fSoftIn);
    }
    else if(!strcmp(pstTensor->achLayoutType, "float"))
    {
       AR_NPU_Exe_Softmax_F32_NhwcCvtNchw((AR_FLOAT *) InputAddr, (AR_FLOAT *)OutputAddr, pstTensor);
    } 
#endif

    return 0;
}

static void AR_NPU_F32_NhwcCvtNchw(AR_FLOAT *pDstBuff, AR_FLOAT * pSrcBuff, AR_NPU_TENSOR_S * pTensor)
{
#ifdef NHWC_TO_NCHW_NEON
        AR_NPU_Exe_NhwcCvtNchw_F32_Neon( pSrcBuff, pDstBuff, pTensor);
#else
	AR_U32 channel = pTensor->u32OriChannels;
	AR_U32 width = pTensor->u32Width;
	AR_U32 height = pTensor->u32Height;
	AR_U32 index_transform;
	for (AR_U32 c = 0; c < channel; c++)
	{
		for(AR_U32 h = 0; h < height; h++)
		{
			for (AR_U32 w = 0; w < width; w++)
			{
				AR_U32 index_transform = AR_NPU_GetEntryIndex(h, w, c, sizeof(AR_FLOAT), pTensor);
				pDstBuff[c*width*height + h*width + w] = pSrcBuff[index_transform];
			}
		}
	}
#endif
}

static void AR_NPU_S16_NhwcCvtNchw(AR_S16*pDstBuff, AR_S16 * pSrcBuff, AR_NPU_TENSOR_S *pTensor)
{

#ifdef NHWC_TO_NCHW_NEON
       AR_NPU_Exe_NhwcCvtNchw_Fix_Neon((AR_CHAR *)pSrcBuff, (AR_CHAR *)pDstBuff, pTensor);
#else
	AR_U32 channel = pTensor->u32OriChannels;
	AR_U32 width = pTensor->u32Width;
	AR_U32 height = pTensor->u32Height;
	AR_U32 index_transform;

	for (AR_U32 c = 0; c < channel; c++)
	{
		for(AR_U32 h = 0; h < height; h++)
		{
			for (AR_U32 w = 0; w < width; w++)
			{
				AR_U32 index_transform = AR_NPU_GetEntryIndex(h, w, c, sizeof(AR_S16), pTensor);
				pDstBuff[c*width*height + h*width + w] = pSrcBuff[index_transform];
			}
		}
	}
#endif
}

static void AR_NPU_S8_NhwcCvtNchw(AR_CHAR*pDstBuff, AR_CHAR * pSrcBuff, AR_NPU_TENSOR_S *pTensor)
{
#ifdef NHWC_TO_NCHW_NEON
       AR_NPU_Exe_NhwcCvtNchw_Fix_Neon((AR_CHAR *)pSrcBuff, (AR_CHAR *)pDstBuff, pTensor);
#else
	AR_U32 channel = pTensor->u32OriChannels;
	AR_U32 width = pTensor->u32Width;
	AR_U32 height = pTensor->u32Height;
	AR_U32 index_transform;
	for (AR_U32 c = 0; c < channel; c++)
	{
		for(AR_U32 h = 0; h < height; h++)
		{
			for (AR_U32 w = 0; w < width; w++)
			{
				AR_U32 index_transform = AR_NPU_GetEntryIndex(h, w, c, sizeof(AR_CHAR), pTensor);
				pDstBuff[c*width*height + h*width + w] = pSrcBuff[index_transform];
			}
		}
	}
#endif
}

//int step = 0;
void AR_NPU_Outbuff_NhwcCvtNchw(AR_CHAR* pcNpuOutput, AR_CHAR* pcNchwOutput, AR_NPU_TENSOR_S* pstTensor)
{
    //int ret =0;
    //AR_U32 ByteUnit =0;
    //AR_U32 OutBuffSize =0;
    AR_NPU_TENSOR_S * stOutputTensor  = pstTensor;
	
    //printf("stOutputTensor.achLayoutType = %s\n", stOutputTensor.achLayoutType);
    //printf("u32Precision= %d,\n", stOutputTensor.u32Precision);
    //printf("u32KNormNum = %d,\n", stOutputTensor.u32KNormNum);
    //printf("u32KSizeNorm = %d,\n", stOutputTensor.u32KSizeNorm);
    //printf("s32Ksizelast = %d\n", stOutputTensor.u32KSizeLast);
    //printf("u32Width = %d, u32Height = %d\n", stOutputTensor.u32Width, stOutputTensor.u32Height);
    if(strcmp(stOutputTensor->achLayoutType, "float")==0)
    {
        AR_NPU_F32_NhwcCvtNchw((AR_FLOAT *)pcNchwOutput, (AR_FLOAT *)pcNpuOutput, stOutputTensor);
    }
    else if((strcmp(stOutputTensor->achLayoutType,"integer")==0) && (stOutputTensor->u32Precision ==16))
    {
        AR_NPU_S16_NhwcCvtNchw((AR_S16 *)pcNchwOutput, (AR_S16 *)pcNpuOutput, stOutputTensor);  
    }
    else
    { 
        AR_NPU_S8_NhwcCvtNchw((AR_CHAR*)pcNchwOutput, (AR_CHAR *)pcNpuOutput, stOutputTensor);
    }

    //FILE * fp = NULL;
    //char name[32] = {0};
    //sprintf(name, "tensor_neon%d.txt", step);
    //step++;
    //fp = fopen(name, "wb+");
    // fwrite(outputBuff, 1, OutBuffSize, fp);
    //fflush(fp);
    //fclose(fp);
    
}

