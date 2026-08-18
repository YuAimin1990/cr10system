#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"

AR_S32 AR_CNN_CustomPadForward(AR_DOUBLE *pInput, AR_DOUBLE *pOutput, AR_NPU_CB_PARAM_S *pCBParams, AR_U32 tensor_id)
{
	AR_NPU_CUSTOMPAD_PARAM_S *pCustomPadParam =(AR_NPU_CUSTOMPAD_PARAM_S*)(pCBParams->pOpParams);
	
	AR_U32 inBatch = pCBParams->astInputTensor[tensor_id].u32Num;
	AR_U32 inChannel = pCBParams->astInputTensor[tensor_id].u32OriChannels;
	AR_U32 inHeight = pCBParams->astInputTensor[tensor_id].u32Height;
	AR_U32 inWidth = pCBParams->astInputTensor[tensor_id].u32Width;
	AR_U32 inChnSize = inHeight * inWidth;
	AR_U32 inBatchSize = inChannel * inHeight * inWidth;

	AR_U32 outBatch = pCBParams->astOutputTensor[tensor_id].u32Num;
	AR_U32 outChannel = pCBParams->astOutputTensor[tensor_id].u32OriChannels;
	AR_U32 outHeight = pCBParams->astOutputTensor[tensor_id].u32Height;
	AR_U32 outWidth = pCBParams->astOutputTensor[tensor_id].u32Width;
	AR_U32 outChnSize = outHeight * outWidth;
	AR_U32 outBatchSize = outChannel * outHeight * outWidth;

	//do pad operator
	if(strcmp(pCustomPadParam->mode, "constant")==0)
	{
		for (int64_t b=0; b<outBatch; b++)
		{
			for (int64_t c=0; c<outChannel; c++)
			{
				for (int64_t topPh=0; topPh<pCustomPadParam->pads[2]; topPh++)
				{
					for (int64_t topPw=0; topPw<outWidth; topPw++)
					{
						pOutput[b*outBatchSize + c*outChnSize + topPh*outWidth + topPw] = pCustomPadParam->value;
					}
				}
				for (int64_t centerPh=pCustomPadParam->pads[2]; centerPh<pCustomPadParam->pads[2]+inHeight; centerPh++)
				{
					for (int64_t centerPw=0; centerPw<outWidth; centerPw++)
					{
						if(centerPw<pCustomPadParam->pads[3])
						{
							pOutput[b*outBatchSize + c*outChnSize + centerPh*outWidth + centerPw] = pCustomPadParam->value;
						}
						else if(centerPw>=pCustomPadParam->pads[3]+inWidth)	//=
						{
							pOutput[b*outBatchSize + c*outChnSize + centerPh*outWidth + centerPw] = pCustomPadParam->value;
						}
						else
						{
							pOutput[b*outBatchSize + c*outChnSize + centerPh*outWidth + centerPw] = pInput[b*inBatchSize + c*inChnSize + (centerPh-pCustomPadParam->pads[2])*inWidth + \
																										(centerPw-pCustomPadParam->pads[3])] + pCustomPadParam->add;
						}
					}
				}
				for (int64_t bottomPh = inHeight+pCustomPadParam->pads[2]; bottomPh<outHeight; bottomPh++)
				{
					for (int64_t bottomPw=0; bottomPw<outWidth; bottomPw++)
					{
						pOutput[b*outBatchSize + c*outChnSize + bottomPh*outWidth + bottomPw] = pCustomPadParam->value;
					}
				}
			}
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorCustomPad(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pCBParamsExt)
{
	AR_NPU_CB_PARAM_EXT_S *pstCBParaExt = (AR_NPU_CB_PARAM_EXT_S *)pCBParamsExt;
	AR_U32 input_tensor_num = pCBParams->u32InputTensorNum;
	AR_UINTPTR pInput;
	AR_UINTPTR pOutput;

	//input
	AR_S32 input_batch_size = pCBParams->astInputTensor[0].u32Num;
	AR_S32 input_channel = pCBParams->astInputTensor[0].u32OriChannels;
	if(!input_channel)
		input_channel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;
	AR_S32 input_height = pCBParams->astInputTensor[0].u32Height;
	AR_S32 input_width = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;
	pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_DOUBLE dInScale = pCBParams->astInputTensor[0].dScaleFactor;
	AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
	AR_U32 u32InPixelByte = u32InPrecision / 8;
	AR_U32 u32InChSize =  input_width * input_height;		
	AR_U32 u32InDataSize = input_channel * u32InChSize * input_batch_size;

	//output
	AR_S32 output_batch_size = pCBParams->astOutputTensor[0].u32Num;
	AR_S32 output_channel = pCBParams->astOutputTensor[0].u32OriChannels;
	if(!output_channel)
		output_channel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_S32 output_height = pCBParams->astOutputTensor[0].u32Height;
	AR_S32 output_width = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
	pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;
	AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 u32OutPixelByte = u32OutPrecision / 8;
	AR_U32 u32OutChSize = output_width * output_height;
	AR_U32 u32OutDataSize = output_channel * u32OutChSize * output_batch_size;

	//malloc input fix NCHW buf
	AR_CHAR *pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32InPixelByte);
	if (!pInNchw)
	{
		printf("Malloc input nchw failed.\n");
		return -1;
	}
	//malloc input double NCHW data buf
	AR_DOUBLE * pdInput = (AR_DOUBLE *)malloc(u32InDataSize * sizeof(AR_DOUBLE));
	if (!pdInput)
	{
		printf("Malloc input buffer failed.\n");
		free(pInNchw);
		return -1;
	}
	memset(pdInput, 0, u32InDataSize * sizeof(AR_DOUBLE));

	//input fix nhwc -> nchw
	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);
	//input fix to double
	dcnn_exec_scale(pInNchw, pdInput, u32InDataSize, dInScale, s32InZeroPoint, u32InPrecision);

	AR_CHAR *pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32OutPixelByte);
	if (!pOutNchw)
	{
		printf("Malloc output nchw failed.\n");
		free(pInNchw);
		free(pdInput);
		return -1;
	}
	AR_DOUBLE *pdOutput = (AR_DOUBLE *)malloc(u32OutDataSize * sizeof(AR_DOUBLE));
	if (!pdOutput)
	{
		printf("Malloc output buffer failed.\n");
		free(pInNchw);
		free(pdInput);
		free(pOutNchw);
		return -1;
	}

	AR_CNN_CustomPadForward(pdInput, pdOutput, pCBParams, 0);
	free(pInNchw);
	free(pdInput);
	
	//output double to fix
	dcnn_exec_quantization(pdOutput, pOutNchw, u32OutDataSize, dOutScale, s32OutZeroPoint, u32OutPrecision);
	//output fix nchw to nhwc
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
	
	free(pOutNchw);
	free(pdOutput);
	
	return 0;
}