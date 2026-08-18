
#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"



void AR_NPU_FixCvtFloat(AR_CHAR* pcInput, AR_FLOAT* pfOutput, AR_U32 u32DataSize, AR_NPU_TENSOR_S * pstTensor)
{
    //int width = pstTensor->u32Width;
    //int height = pstTensor->u32Height;
    //int batch = pstTensor->u32Num;
    //int channel = pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast;
    int data_size = u32DataSize;
    double scale_factor = pstTensor->dScaleFactor;
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

void AR_NPU_FloatCvtFix(AR_FLOAT* pfInput, AR_CHAR* pcOutput, AR_U32 u32DataSize, AR_NPU_TENSOR_S * pstTensor)
{
    //int width = pstTensor->u32Width;
    //int height = pstTensor->u32Height;
    //int batch = pstTensor->u32Num;
    //int channel = pstTensor->u32KNormNum * pstTensor->u32KSizeNorm + pstTensor->u32KSizeLast;
    int data_size = u32DataSize;
    double scale_factor = pstTensor->dScaleFactor;
    int zero_point = pstTensor->s32ZeroPoint;
    int precision = pstTensor->u32Precision;
    int positive_limit = (int)pow((double)2, (double)(precision - 1)) - 1;
    int negative_limit = (int)-pow((double)2, (double)(precision - 1));
    int tmp = 0;

    if(precision == 16)
    {
        short * quant = (short *)pcOutput;

        for (int i = 0; i < data_size; ++i)
        {
            tmp = (int)round(pfInput[i] / scale_factor) + zero_point;
            tmp = MIN(tmp, positive_limit);
            tmp = MAX(tmp, negative_limit);
            quant[i] = (short)tmp;
        }
    }
    else
    {
        for (int i = 0; i < data_size; ++i)
        {
           tmp = (int)round(pfInput[i] / scale_factor) + zero_point;
            tmp = MIN(tmp, positive_limit);
            tmp = MAX(tmp, negative_limit);
            pcOutput[i] = (char)tmp;
        }
    }
}
AR_S32 _AR_CNN_ConcatForward(AR_CHAR * pInF, AR_CHAR * pOutF, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_CONCAT_PARAM_S *pConcatParam, AR_U32 idx)
{
	AR_U32 InputTeansorNum = pCBParams->u32InputTensorNum;
	AR_U32 u32Batch = pCBParams->astOutputTensor[0].u32Num;
	AR_U32 u32Precision = pCBParams->astOutputTensor[0].u32Precision;
	AR_U32 u32PixelByte = u32Precision / 8;
	//input
	AR_U32 u32InHeight = pCBParams->astInputTensor[idx].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[idx].u32Width;
	AR_U32 u32InChannels = pCBParams->astInputTensor[idx].u32KNormNum * pCBParams->astInputTensor[idx].u32KSizeNorm + pCBParams->astInputTensor[idx].u32KSizeLast;
	AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32InSingleSize = u32InChannels * u32InChSize;
	AR_U32 u32InDataSize = u32InChannels * u32InChSize * u32Batch;

	//output
	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannels = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;
	AR_U32 u32OutSingleSize = u32OutChannels * u32OutChSize;
	AR_U32 u32OutDataSize = u32OutChannels * u32OutChSize * u32Batch;

	if (pConcatParam->u32Axis == 3)	// --> axis == 3, input data format NCHW
	{
		if(u32Precision == 16)
		{
			AR_S16* ps16In = (AR_S16 *)pInF;
			AR_S16* ps16Out = (AR_S16 *)pOutF;

			if((pCBParams->astInputTensor[idx].s32ZeroPoint != pCBParams->astOutputTensor[0].s32ZeroPoint) ||
							(pCBParams->astInputTensor[idx].dScaleFactor !=pCBParams->astOutputTensor[0].dScaleFactor))
			{
				float *CvtFloat = (float *)malloc(u32InDataSize * sizeof(float));
				memset(CvtFloat, 0, u32InDataSize * sizeof(float));

				AR_NPU_FixCvtFloat((AR_S16*)ps16In, (AR_FLOAT *)CvtFloat, u32InDataSize, &pCBParams->astInputTensor[idx]);
				AR_NPU_FloatCvtFix((AR_FLOAT *)CvtFloat, (AR_S16*)ps16In, u32InDataSize, &pCBParams->astOutputTensor[0]);
				free(CvtFloat);
			}

			int start_w = 0;
			if(idx==0)
			{
				start_w = 0;
			}
			else
			{
				for(int i=0; i<idx; i++)
				{
					start_w += pCBParams->astInputTensor[i].u32Width;
				}
			}

			for (int b = 0; b<u32Batch; b++)
			{
				for (int c =0; c<u32InChannels; c++)
				{
					for (int h =0; h<u32InHeight; h++)
					{
						for (int w =0; w<u32InWidth; w++)
						{
							ps16Out[b * u32OutSingleSize + c * u32OutChSize + h * u32OutWidth + (start_w + w)] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
						}
					}
				}
			}
		}
		else  //8bit
		{
			AR_S8* ps8In = (AR_S8 *)pInF;
			AR_S8* ps8Out = (AR_S8 *)pOutF;

			if((pCBParams->astInputTensor[idx].s32ZeroPoint != pCBParams->astOutputTensor[0].s32ZeroPoint) ||
							(pCBParams->astInputTensor[idx].dScaleFactor !=pCBParams->astOutputTensor[0].dScaleFactor))
			{
				float *CvtFloat = (float *)malloc(u32InDataSize * sizeof(float));
				memset(CvtFloat, 0, u32InDataSize * sizeof(float));

				AR_NPU_FixCvtFloat((AR_S8*)ps8In, (AR_FLOAT *)CvtFloat, u32InDataSize, &pCBParams->astInputTensor[idx]);
				AR_NPU_FloatCvtFix((AR_FLOAT *)CvtFloat, (AR_S8*)ps8In, u32InDataSize, &pCBParams->astOutputTensor[0]);
				free(CvtFloat);
			}

			int start_w = 0;
			if(idx==0)
			{
				start_w = 0;
			}
			else
			{
				for(int i=0; i<idx; i++)
				{
					start_w += pCBParams->astInputTensor[i].u32Width;
				}
			}

			for (int b = 0; b<u32Batch; b++)
			{
				for (int c =0; c<u32InChannels; c++)
				{
					for (int h =0; h<u32InHeight; h++)
					{
						for (int w =0; w<u32InWidth; w++)
						{
							ps8Out[b * u32OutSingleSize + c * u32OutChSize + h * u32OutWidth + (start_w + w)] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
						}
					}
				}
			}
  		}
	}

	if (pConcatParam->u32Axis == 2)
	{
		if(u32Precision == 16)
		{
			AR_S16* ps16In = (AR_S16 *)pInF;
			AR_S16* ps16Out = (AR_S16 *)pOutF;

			if((pCBParams->astInputTensor[idx].s32ZeroPoint != pCBParams->astOutputTensor[0].s32ZeroPoint) ||
							(pCBParams->astInputTensor[idx].dScaleFactor !=pCBParams->astOutputTensor[0].dScaleFactor))
			{
				float *CvtFloat = (float *)malloc(u32InDataSize * sizeof(float));
				memset(CvtFloat, 0, u32InDataSize * sizeof(float));

				AR_NPU_FixCvtFloat((AR_S16*)ps16In, (AR_FLOAT *)CvtFloat, u32InDataSize, &pCBParams->astInputTensor[idx]);
				AR_NPU_FloatCvtFix((AR_FLOAT *)CvtFloat, (AR_S16*)ps16In, u32InDataSize, &pCBParams->astOutputTensor[0]);
				free(CvtFloat);
			}

			int start_h = 0;
			if(idx==0)
			{
				start_h = 0;
			}
			else
			{
				for(int i=0; i<idx; i++)
				{
					start_h += pCBParams->astInputTensor[i].u32Height;
				}
			}
			for (int b=0; b<u32Batch; b++)
			{
				for (int c=0; c<u32InChannels; c++)
				{
					for (int h=0; h<u32InHeight; h++)
					{
						for (int w=0; w<u32InWidth; w++)
						{
							ps16Out[b * u32OutSingleSize + c * u32OutChSize + (start_h + h) * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
						}
					}
				}
			}
		}

		else
		{
			AR_S8* ps8In = (AR_S8 *)pInF;
			AR_S8* ps8Out = (AR_S8 *)pOutF;

			if((pCBParams->astInputTensor[idx].s32ZeroPoint != pCBParams->astOutputTensor[0].s32ZeroPoint) ||
							(pCBParams->astInputTensor[idx].dScaleFactor !=pCBParams->astOutputTensor[0].dScaleFactor))
			{
				float *CvtFloat = (float *)malloc(u32InDataSize * sizeof(float));
				memset(CvtFloat, 0, u32InDataSize * sizeof(float));

				AR_NPU_FixCvtFloat((AR_S8*)ps8In, (AR_FLOAT *)CvtFloat, u32InDataSize, &pCBParams->astInputTensor[idx]);
				AR_NPU_FloatCvtFix((AR_FLOAT *)CvtFloat, (AR_S8*)ps8In, u32InDataSize, &pCBParams->astOutputTensor[0]);
				free(CvtFloat);
			}

			int start_h = 0;
			if(idx==0)
			{
				start_h = 0;
			}
			else
			{
				for(int i=0; i<idx; i++)
				{
					start_h += pCBParams->astInputTensor[i].u32Height;
				}
			}

			for (int b=0; b<u32Batch; b++)
			{
				for (int c=0; c<u32InChannels; c++)
				{
					for (int h=0; h<u32InHeight; h++)
					{
						for (int w=0; w<u32InWidth; w++)
						{
							ps8Out[b * u32OutSingleSize + c * u32OutChSize + (start_h + h) * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
						}
					}
				}
			}
		}
	}
}

AR_S32 ArCnnOperatorConcat(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
    xDebugPrint("ArCnnOperatorConcat \n");
  	AR_NPU_CONCAT_PARAM_S *pConcatParam = (AR_NPU_CONCAT_PARAM_S*)(pOpParams);
	AR_U32 InputTeansorNum = pCBParams->u32InputTensorNum;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32Precision = pCBParams->astOutputTensor[0].u32Precision;
	AR_U32 u32PixelByte = u32Precision / 8;
	//input
	AR_U32 pInput;
	AR_U32 u32InHeight;
	AR_U32 u32InWidth;
	AR_U32 u32InChannels;
	AR_U32 u32InChSize;
	AR_U32 u32InDataSize;
	//output
	AR_U32 pOutput;
	AR_U32 u32OutHeight;
	AR_U32 u32OutWidth;
	AR_U32 u32OutChannels;
	AR_U32 u32OutChSize;
	AR_U32 u32OutDataSize;

	u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	u32OutChannels = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;

    if(0 == strcmp(pCBParams->astOutputTensor[0].achMemoryType, "output"))
    {
		pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;
    }
	else
	{
		pOutput = pIn + pCBParams->astOutputTensor[0].u32Offset;
	}

	u32OutChSize = u32OutHeight * u32OutWidth;
	u32OutDataSize = u32OutChannels * u32OutChSize * u32Batch;
	AR_CHAR *pOutNchw;
	pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32PixelByte);
	if (!pOutNchw)
	{
		printf("Malloc pOutNchw(AR_CNN_OperatorConcat) failed.\n");
		free(pOutNchw);
		return -1;
	}

	for(int idx=0; idx<InputTeansorNum; idx++)
	{
		AR_CHAR *pInNchw;
		u32InHeight = pCBParams->astInputTensor[idx].u32Height;
		u32InWidth = pCBParams->astInputTensor[idx].u32Width;
		u32InChannels = pCBParams->astInputTensor[idx].u32KNormNum * pCBParams->astInputTensor[idx].u32KSizeNorm + pCBParams->astInputTensor[idx].u32KSizeLast;
		u32Batch = pCBParams->astInputTensor[idx].u32Num;

		if(0 == strcmp(pCBParams->astInputTensor[idx].achMemoryType, "input"))
		{
			pInput = pIn + pCBParams->astInputTensor[idx].u32Offset;
		}
		else
		{
			pInput = pOut + pCBParams->astInputTensor[idx].u32Offset;
		}

		u32InChSize = u32InHeight * u32InWidth;
		u32InDataSize = u32InChannels * u32InChSize * u32Batch;

		pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32PixelByte);
		if (!pInNchw)
		{
			printf("Malloc pInNchw(AR_CNN_OperatorConcat) failed.\n");
			free(pInNchw);
			return -1;
		}

		dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[idx]);
		_AR_CNN_ConcatForward(pInNchw, pOutNchw, pCBParams, pConcatParam, idx);
		free(pInNchw);
	}
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
	free(pOutNchw);
	return 0;
}


AR_CNN_OPERATOR_DECLARE(ArCnnOperatorConcat) = {
    .pName = "Concat",
    .pCnnCbHandler = ArCnnOperatorConcat,
};

