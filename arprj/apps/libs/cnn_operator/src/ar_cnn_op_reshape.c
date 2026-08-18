#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"


AR_S32 AR_CNN_OperatorOnnxReshape(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_U32 u32InDataSize = pCBParams->astInputTensor[0].u32Size;
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32PixelByte = u32Precision / 8;

	AR_CHAR* pNchw = (AR_CHAR*)malloc(u32InDataSize * u32PixelByte);
	if(!pNchw)
	{
		printf("malloc pNchw failed!\n");
		free(pNchw);
		return -1;
	}
	memset(pNchw, 0, u32InDataSize * u32PixelByte);

	dcnn_hwc_to_chw((AR_CHAR*)pInput, pNchw, &pCBParams->astInputTensor[0]);

	dcnn_chw_to_hwc(pNchw, (AR_CHAR*)pOutput, &pCBParams->astOutputTensor[0]);

	free(pNchw);

	return 0;
}

AR_S32 AR_CNN_OperatorReshape(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	AR_U32 u32InDataSize = pCBParams->astInputTensor[0].u32Size;
	
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32PixelByte = u32Precision / 8;

	AR_CHAR* pNchw = (AR_CHAR*)malloc(u32InDataSize * u32PixelByte);
	if(!pNchw)
	{
		printf("malloc pNchw failed!\n");
		free(pNchw);
		return -1;
	}
	memset(pNchw, 0, u32InDataSize * u32PixelByte);

	dcnn_hwc_to_chw((AR_CHAR*)pInput, pNchw, &pCBParams->astInputTensor[0]);

	dcnn_chw_to_hwc(pNchw, (AR_CHAR*)pOutput, &pCBParams->astOutputTensor[0]);

	free(pNchw);
	
	return 0;
}

#if 0
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"
AR_S32 AR_CNN_OperatorOnnxReshape(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_S32 tmp = 0;
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InTensorStep = pCBParams->astInputTensor[0].u32TensorStep;
	AR_U32 u32InRowStep = pCBParams->astInputTensor[0].u32RowStep;
	AR_U32 u32InKSizeNorm = pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InKSizeLast = pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32InKStep = pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InKNormNum = pCBParams->astInputTensor[0].u32KNormNum;
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;

    AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
    AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
	AR_U32 u32OutRowStep = pCBParams->astOutputTensor[0].u32RowStep;
	AR_U32 u32OutKSizeNorm = pCBParams->astOutputTensor[0].u32KSizeNorm;
	AR_U32 u32OutKSizeLast = pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_U32 u32OutKStep = pCBParams->astOutputTensor[0].u32KStep;
	AR_U32 u32OutKNormNum = pCBParams->astOutputTensor[0].u32KNormNum;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32PixelByte = u32Precision / 8;

	AR_U32 u32InKSize = 0, u32OutKSize = 0;
	//AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;

    //AR_U32 u32InDataSize = pCBParams->astInputTensor[0].u32Size;
    AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_DOUBLE dInFactor = pCBParams->astInputTensor[0].dScaleFactor;
    //AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;

    //AR_U32 u32OutDataSize = pCBParams->astOutputTensor[0].u32Size;
    AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_DOUBLE dOutFactor = pCBParams->astOutputTensor[0].dScaleFactor;
    //AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
	AR_U32 u32ContinueMode = 0;
	AR_U32 u32ContinueCnt = 0;
	AR_CHAR achLayoutType[256]={0};
	sprintf(achLayoutType, "%s",  pCBParams->astOutputTensor[0].achLayoutType);
	if(strcmp(achLayoutType,"continue")==0)
	{
		u32ContinueMode = 1;
	}


	if (u32Precision == 16)
	{
		AR_S16 * ps16In = NULL;
		AR_S16 * ps16Out = NULL;
		int positive_limit = (int)pow((double)2, (double)(u32Precision - 1)) - 1;
	    int negative_limit = (int)-pow((double)2, (double)(u32Precision - 1));
		AR_S16 val = 0;
		AR_U32 idx = 0;

		for (int c = 0; c < u32InChannel; c++)
		{
			for (int h = 0; h < u32InHeight; h++)
			{
				for (int w = 0; w < u32InWidth; w++)
				{
					for (int b = 0; b < u32Batch; b++)
					{
						if (c >= u32InKNormNum * u32InKSizeNorm)
						{
							u32InKSize = u32InKSizeLast;
						}
						else
						{
							u32InKSize = u32InKSizeNorm;
						}

						ps16In = (AR_S16 *)(pInput + (c / u32InKSizeNorm) * u32InKStep + h * u32InRowStep + w * u32InKSize * u32PixelByte + (c % u32InKSize) * u32PixelByte + b * u32InTensorStep);
						//calculate where to put this data in output tensor
						//AR_U32 u32OutC = c * u32InChSize / u32OutChSize;
						//AR_U32 u32OutH = (c * u32InChSize) % u32OutChSize / u32OutWidth;
						//AR_U32 u32OutW = ((c * u32InChSize) % u32OutChSize) % u32OutWidth;
						AR_U32 u32OutC = idx / u32OutChSize;
						AR_U32 u32OutH = (idx % u32OutChSize )/ u32OutWidth;
						AR_U32 u32OutW = (idx % u32OutChSize )% u32OutWidth;

						if (u32OutC >= u32OutKNormNum * u32OutKSizeNorm)
						{
							u32OutKSize = u32OutKSizeLast;
						}
						else
						{
							u32OutKSize = u32OutKSizeNorm;
						}

						if(!u32ContinueMode)
						{
						    ps16Out = (AR_S16 *)(pOutput + (u32OutC / u32OutKSizeNorm) * u32OutKStep + u32OutH * u32OutRowStep + u32OutW * u32OutKSize * u32PixelByte + (u32OutC % u32OutKSize) * u32PixelByte + b * u32OutTensorStep);
						}
						else
						{
							ps16Out = (AR_S16 *)(pOutput + u32ContinueCnt*u32PixelByte);
							u32ContinueCnt++;
						}

                        double dout = (*ps16In - s32InZeroPoint) * dInFactor;
						tmp = round(dout / dOutFactor) + s32OutZeroPoint;

			            val = MIN(tmp, positive_limit);
			            val = MAX(val, negative_limit);

						*ps16Out = (AR_S16)val;
						idx++;


					}
				}
			}
		}
	}
	else//8bit
	{
		AR_S8 * ps8In = NULL;
		AR_S8 * ps8Out = NULL;
		int positive_limit = (int)pow((double)2, (double)(u32Precision - 1)) - 1;
	    int negative_limit = (int)-pow((double)2, (double)(u32Precision - 1));
		AR_S8 val = 0;
		AR_U32 idx = 0;


		for (int c = 0; c < u32InChannel; c++)
		{
			for (int h = 0; h < u32InHeight; h++)
			{
				for (int w = 0; w < u32InWidth; w++)
				{
					for (int b = 0; b < u32Batch; b++)
					{
						if (c >= u32InKNormNum * u32InKSizeNorm)
						{
							u32InKSize = u32InKSizeLast;
						}
						else
						{
							u32InKSize = u32InKSizeNorm;
						}

						ps8In = (AR_S8 *)(pInput + (c / u32InKSizeNorm) * u32InKStep + h * u32InRowStep + w * u32InKSize * u32PixelByte + (c % u32InKSize) * u32PixelByte + b * u32InTensorStep);
						//calculate where to put this data in output tensor
						//AR_U32 u32OutC = c * u32InChSize / u32OutChSize;
						//AR_U32 u32OutH = (c * u32InChSize) % u32OutChSize / u32OutWidth;
						//AR_U32 u32OutW = ((c * u32InChSize) % u32OutChSize) % u32OutWidth;
						AR_U32 u32OutC = idx / u32OutChSize;
						AR_U32 u32OutH = (idx % u32OutChSize )/ u32OutWidth;
						AR_U32 u32OutW = (idx % u32OutChSize )% u32OutWidth;
						if (u32OutC >= u32OutKNormNum * u32OutKSizeNorm)
						{
							u32OutKSize = u32OutKSizeLast;
						}
						else
						{
							u32OutKSize = u32OutKSizeNorm;
						}

						if(!u32ContinueMode)
						{
						    ps8Out = (AR_S8 *)(pOutput + (u32OutC / u32OutKSizeNorm) * u32OutKStep + u32OutH * u32OutRowStep + u32OutW * u32OutKSize * u32PixelByte + (u32OutC % u32OutKSize) * u32PixelByte + b * u32OutTensorStep);
						}
						else
						{
							ps8Out = (AR_S8 *)(pOutput + u32ContinueCnt*u32PixelByte);
							u32ContinueCnt++;
						}


					   double dout = (*ps8In - s32InZeroPoint) * dInFactor;
					   tmp = round(dout / dOutFactor) + s32OutZeroPoint;
					   val = MIN(tmp, positive_limit);
					   val = MAX(val, negative_limit);

					   *ps8Out = (AR_S8)val;
					   idx++;
					}
				}
			}
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorReshape(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InTensorStep = pCBParams->astInputTensor[0].u32TensorStep;
	AR_U32 u32InRowStep = pCBParams->astInputTensor[0].u32RowStep;
	AR_U32 u32InKSizeNorm = pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InKSizeLast = pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32InKStep = pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InKNormNum = pCBParams->astInputTensor[0].u32KNormNum;
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;

    AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
    AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
	AR_U32 u32OutRowStep = pCBParams->astOutputTensor[0].u32RowStep;
	AR_U32 u32OutKSizeNorm = pCBParams->astOutputTensor[0].u32KSizeNorm;
	AR_U32 u32OutKSizeLast = pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_U32 u32OutKStep = pCBParams->astOutputTensor[0].u32KStep;
	AR_U32 u32OutKNormNum = pCBParams->astOutputTensor[0].u32KNormNum;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32PixelByte = u32Precision / 8;

	AR_U32 u32InKSize = 0, u32OutKSize = 0;
	//AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;

	if (u32Precision == 16)
	{
		AR_S16 * ps16In = NULL;
		AR_S16 * ps16Out = NULL;
		AR_U32 idx = 0;


		for (int c = 0; c < u32InChannel; c++)
		{
			for (int h = 0; h < u32InHeight; h++)
			{
				for (int w = 0; w < u32InWidth; w++)
				{
					for (int b = 0; b < u32Batch; b++)
					{
						if (c >= u32InKNormNum * u32InKSizeNorm)
						{
							u32InKSize = u32InKSizeLast;
						}
						else
						{
							u32InKSize = u32InKSizeNorm;
						}

						ps16In = (AR_S16 *)(pInput + (c / u32InKSizeNorm) * u32InKStep + h * u32InRowStep + w * u32InKSize * u32PixelByte + (c % u32InKSize) * u32PixelByte + b * u32InTensorStep);
						//calculate where to put this data in output tensor
						//AR_U32 u32OutC = c * u32InChSize / u32OutChSize;
						//AR_U32 u32OutH = (c * u32InChSize) % u32OutChSize / u32OutWidth;
						//AR_U32 u32OutW = ((c * u32InChSize) % u32OutChSize) % u32OutWidth;
						AR_U32 u32OutC = idx / u32OutChSize;
						AR_U32 u32OutH = (idx % u32OutChSize )/ u32OutWidth;
						AR_U32 u32OutW = (idx % u32OutChSize )% u32OutWidth;

						if (u32OutC >= u32OutKNormNum * u32OutKSizeNorm)
						{
							u32OutKSize = u32OutKSizeLast;
						}
						else
						{
							u32OutKSize = u32OutKSizeNorm;
						}

						ps16Out = (AR_S16 *)(pOutput + (u32OutC / u32OutKSizeNorm) * u32OutKStep + u32OutH * u32OutRowStep + u32OutW * u32OutKSize * u32PixelByte + (u32OutC % u32OutKSize) * u32PixelByte + b * u32OutTensorStep);
						*ps16Out = *ps16In;
						idx++;
					}
				}
			}
		}
	}
	else//8bit
	{
		AR_S8 * ps8In = NULL;
		AR_S8 * ps8Out = NULL;
		AR_U32 idx = 0;


		for (int c = 0; c < u32InChannel; c++)
		{
			for (int h = 0; h < u32InHeight; h++)
			{
				for (int w = 0; w < u32InWidth; w++)
				{
					for (int b = 0; b < u32Batch; b++)
					{
						if (c >= u32InKNormNum * u32InKSizeNorm)
						{
							u32InKSize = u32InKSizeLast;
						}
						else
						{
							u32InKSize = u32InKSizeNorm;
						}

						ps8In = (AR_S8 *)(pInput + (c / u32InKSizeNorm) * u32InKStep + h * u32InRowStep + w * u32InKSize * u32PixelByte + (c % u32InKSize) * u32PixelByte + b * u32InTensorStep);
						//calculate where to put this data in output tensor
						//AR_U32 u32OutC = c * u32InChSize / u32OutChSize;
						//AR_U32 u32OutH = (c * u32InChSize) % u32OutChSize / u32OutWidth;
						//AR_U32 u32OutW = ((c * u32InChSize) % u32OutChSize) % u32OutWidth;
						AR_U32 u32OutC = idx / u32OutChSize;
						AR_U32 u32OutH = (idx % u32OutChSize )/ u32OutWidth;
						AR_U32 u32OutW = (idx % u32OutChSize )% u32OutWidth;

						if (u32OutC >= u32OutKNormNum * u32OutKSizeNorm)
						{
							u32OutKSize = u32OutKSizeLast;
						}
						else
						{
							u32OutKSize = u32OutKSizeNorm;
						}

						ps8Out = (AR_S8 *)(pOutput + (u32OutC / u32OutKSizeNorm) * u32OutKStep + u32OutH * u32OutRowStep + u32OutW * u32OutKSize * u32PixelByte + (u32OutC % u32OutKSize) * u32PixelByte + b * u32OutTensorStep);
						*ps8Out = *ps8In;
						idx++;
					}
				}
			}
		}
	}

	return 0;
}



#endif