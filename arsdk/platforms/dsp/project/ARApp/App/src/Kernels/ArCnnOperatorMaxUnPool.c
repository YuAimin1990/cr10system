
#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"


AR_S32 ArCnnOperatorMaxUnPool(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_NPU_MAXUNPOOL_PARAM_S *pMaxUnPoolParam  = (AR_NPU_MAXUNPOOL_PARAM_S*)(pOpParams);

	const int stride_h = pMaxUnPoolParam->u32PoolStrideH;
	const int stride_w = pMaxUnPoolParam->u32PoolStrideW;
	const int kernel_h = pMaxUnPoolParam->u32PoolKernelH;
	const int kernel_w = pMaxUnPoolParam->u32PoolKernelW;
	const int pad_w = pMaxUnPoolParam->u32MaxUnPoolPadT;// config->pool_pad_t;
	const int pad_h = pMaxUnPoolParam->u32MaxUnPoolPadL;// config->pool_pad_l;

	//double* pooled_data = pIn1;
	//double* data = input_data_2;
	const int pooled_height = pCBParams->astInputTensor[0].u32Height;// config->operator_input_height[0];
	const int pooled_width = pCBParams->astInputTensor[0].u32Width;
	const int height = pCBParams->astInputTensor[1].u32Height;// config->operator_input_height[1];
	const int width = pCBParams->astInputTensor[1].u32Width;// config->operator_input_width[1];

	int batch_size = pCBParams->astInputTensor[0].u32Num;// config->batch_size[0];
	int output_channel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm
		               + pCBParams->astOutputTensor[0].u32KSizeLast;// config->operator_output_channel;

	AR_U32 u32ConvTensorStep = pCBParams->astInputTensor[1].u32TensorStep;
	AR_U32 u32ConvRowStep = pCBParams->astInputTensor[1].u32RowStep;
	AR_U32 u32ConvKSize = pCBParams->astInputTensor[1].u32KSizeNorm;
	AR_U32 u32ConvKStep = pCBParams->astInputTensor[1].u32KStep;
	AR_U32 pConv = pIn + pCBParams->astInputTensor[1].u32Offset;

	AR_U32 u32PoolTensorStep = pCBParams->astInputTensor[0].u32TensorStep;
	AR_U32 u32PoolRowStep = pCBParams->astInputTensor[0].u32RowStep;
	AR_U32 u32PoolKSize = pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32PoolKStep = pCBParams->astInputTensor[0].u32KStep;
	AR_U32 pPool = pIn + pCBParams->astInputTensor[0].u32Offset;

	AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
	AR_U32 u32OutRowStep = pCBParams->astOutputTensor[0].u32RowStep;
	AR_U32 u32OutKSize = pCBParams->astOutputTensor[0].u32KSizeNorm;
	AR_U32 u32OutKSizeLast = pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_U32 u32OutKStep = pCBParams->astOutputTensor[0].u32KStep;
	AR_U32 u32OutKNormNum = pCBParams->astOutputTensor[0].u32KNormNum;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	AR_U32 u32PixelByte = u32Precision / 8;

	AR_U32 u32ConvIdxH = 0, u32ConvIdxW = 0;
	AR_U32 u32KSize = 0;

	if (pCBParams->astInputTensor[0].u32Precision == 16)
	{
		AR_S16 * ps16Conv = NULL;
		AR_S16 * ps16Pool = NULL;
		AR_S16 * ps16Out = NULL;
		//initialize the output first, float 0 is 0 / fscale + zeropoint after quantization
		for (int i = 0; i < (u32OutKNormNum + 1) * u32OutKStep; i += 2)
		{
			*(AR_S16 *)(pOutput + i) = s32OutZeroPoint;
		}

		for (int c = 0; c < output_channel; c++)
		{
			if (c >= u32OutKSize * u32OutKNormNum) //last k
			{
				u32KSize = u32OutKSizeLast;
			}
			else
			{
				u32KSize = u32OutKSize; //normal k
			}

			for (int ph = 0; ph < pooled_height; ph++)
			{
				for (int pw = 0; pw < pooled_width; pw++)
				{
					for (int n = 0; n < batch_size; n++)
					{
						int hstart = ph * stride_h - pad_w;
						int wstart = pw * stride_w - pad_h;
						int hend = MIN(hstart + kernel_h, height);
						int wend = MIN(wstart + kernel_w, width);
						hstart = MAX(hstart, 0);
						wstart = MAX(wstart, 0);
						//const int pool_index = ph * pooled_width + pw;
						double top_max = -DBL_MAX;
						//int top_index = 0;
						for (int h = hstart; h < hend; h++)
						{
							for (int w = wstart; w < wend; w++)
							{
								ps16Conv = (AR_S16 *)(pConv + (c / u32ConvKSize) * u32ConvKStep + h * u32ConvRowStep + w * u32KSize * u32PixelByte + (c % u32ConvKSize) * u32PixelByte + n * u32ConvTensorStep);

								AR_S16 s16Data = *ps16Conv;
								//const int index = h * width + w;
								if (s16Data > top_max/*data[index] > top_max*/)
								{
									top_max = s16Data;// data[index];
									//top_index = index;
									u32ConvIdxH = h;
									u32ConvIdxW = w;
								}
							}
						}
						ps16Out = (AR_S16 *)(pOutput + (c / u32OutKSize) * u32OutKStep + u32ConvIdxH * u32OutRowStep + u32ConvIdxW * u32KSize * u32PixelByte + (c % u32OutKSize) * u32PixelByte + n * u32OutTensorStep);
						ps16Pool = (AR_S16 *)(pPool + (c / u32PoolKSize) * u32PoolKStep + ph * u32PoolRowStep + pw * u32KSize * u32PixelByte + (c % u32PoolKSize) * u32PixelByte + n * u32PoolTensorStep);
						//output_data[top_index] = pooled_data[pool_index];
						*ps16Out = *ps16Pool;
					}
				}
			}
		}
	}
	else //8bit
	{
		AR_S8 * ps8Conv = NULL;
		AR_S8 * ps8Pool = NULL;
		AR_S8 * ps8Out = NULL;

		//initialize the output first, float 0 is 0 / fscale + zeropoint after quantization
		memset((AR_S8 *)pOutput, s32OutZeroPoint, (u32OutKNormNum + 1) * u32OutKStep);

		for (int c = 0; c < output_channel; c++)
		{
			if (c >= u32OutKSize * u32OutKNormNum) //last k
			{
				u32KSize = u32OutKSizeLast;
			}
			else
			{
				u32KSize = u32OutKSize; //normal k
			}

			for (int ph = 0; ph < pooled_height; ph++)
			{
				for (int pw = 0; pw < pooled_width; pw++)
				{
					for (int n = 0; n < batch_size; n++)
					{
						int hstart = ph * stride_h - pad_w;
						int wstart = pw * stride_w - pad_h;
						int hend = MIN(hstart + kernel_h, height);
						int wend = MIN(wstart + kernel_w, width);
						hstart = MAX(hstart, 0);
						wstart = MAX(wstart, 0);
						//const int pool_index = ph * pooled_width + pw;
						double top_max = -DBL_MAX;
						//int top_index = 0;
						for (int h = hstart; h < hend; h++)
						{
							for (int w = wstart; w < wend; w++)
							{
								ps8Conv = (AR_S8 *)(pConv + (c / u32ConvKSize) * u32ConvKStep + h * u32ConvRowStep + w * u32KSize * u32PixelByte + (c % u32ConvKSize) * u32PixelByte + n * u32ConvTensorStep);

								AR_S8 s8Data = *ps8Conv;
								//const int index = h * width + w;
								if (s8Data > top_max/*data[index] > top_max*/)
								{
									top_max = s8Data;// data[index];
									//top_index = index;
									u32ConvIdxH = h;
									u32ConvIdxW = w;
								}
							}
						}
						ps8Out = (AR_S8 *)(pOutput + (c / u32OutKSize) * u32OutKStep + u32ConvIdxH * u32OutRowStep + u32ConvIdxW * u32KSize * u32PixelByte + (c % u32OutKSize) * u32PixelByte + n * u32OutTensorStep);
						ps8Pool = (AR_S8 *)(pPool + (c / u32PoolKSize) * u32PoolKStep + ph * u32PoolRowStep + pw * u32KSize * u32PixelByte + (c % u32PoolKSize) * u32PixelByte + n * u32PoolTensorStep);
						//output_data[top_index] = pooled_data[pool_index];
						*ps8Out = *ps8Pool;
					}
				}
			}
		}
	}

	return 0;
}




AR_CNN_OPERATOR_DECLARE(ArCnnOperatorMaxUnPool) = {
    .pName = "maxunpool",
    .pCnnCbHandler = ArCnnOperatorMaxUnPool,
};
