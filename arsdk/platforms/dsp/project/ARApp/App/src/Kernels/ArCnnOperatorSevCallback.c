
#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"

static void PoolCalculate(AR_DOUBLE * input_data, AR_DOUBLE * output_data, AR_S32 pad_val, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_SEVCALLBACK_PARAM_S*pSevParam)
{
	int out_c = pSevParam->u32PoolOutC;
	int out_h = pSevParam->u32PoolOutH;
	int out_w = pSevParam->u32PoolOutW;

	int in_h = pSevParam->u32PoolInH;
	int in_w = pSevParam->u32PoolInW;

	int stride_h = pSevParam->u32PoolStrideH;
	int stride_w = pSevParam->u32PoolStrideW;
	int kernel_h = pSevParam->u32PoolKernelH;
	int kernel_w = pSevParam->u32PoolKernelW;

	int pad_t = pSevParam->u32PoolPadT;
	int pad_l = pSevParam->u32PoolPadL;
	int batch_size = pCBParams->astInputTensor[0].u32Num;
	//int pool_method = pCBParams->uniOperatorParam.stSevcallbackParams.u32Pool

	if (strcmp(pSevParam->achPoolMethod, "max") == 0)
	{
		for (int c = 0; c < out_c; ++c)
		{
			for (int ph = 0; ph < out_h * batch_size; ++ph)
			{
				for (int pw = 0; pw < out_w; ++pw)
				{
					int hstart = ph * stride_h - pad_t;
					int wstart = pw * stride_w - pad_l;
					int hend = MIN(hstart + kernel_h, in_h * batch_size);
					int wend = MIN(wstart + kernel_w, in_w);
					hstart = MAX(hstart, 0);
					wstart = MAX(wstart, 0);

					int out_ind = (c * out_h * batch_size + ph) * out_w + pw;
					int  in_ind = (c * in_h * batch_size + hstart) * in_w + wstart;

					//for (int n = 0; n < batch_size; ++n)
					{
						double* input_data_tmp = input_data;// + n * out_c * in_h * in_w;
						double* output_data_tmp = output_data;// + n * out_c * out_h * out_w;

						output_data_tmp[out_ind] = input_data_tmp[in_ind];
						for (int h = hstart; h < hend; ++h)
						{
							for (int w = wstart; w < wend; ++w)
							{
								int index = (c * in_h * batch_size + h) * in_w + w;
								output_data_tmp[out_ind] = MAX(output_data_tmp[out_ind], input_data_tmp[index]);
							}
						}
					}
				}
			}
		}
	}
	else if (strcmp(pSevParam->achPoolMethod, "average") == 0)
	{
		for (int c = 0; c < out_c; ++c)
		{
			for (int ph = 0; ph < out_h * batch_size; ++ph)
			{
				for (int pw = 0; pw < out_w; ++pw)
				{
					int hstart = ph * stride_h - pad_t;
					int wstart = pw * stride_w - pad_l;
					int hend = MIN(hstart + kernel_h, in_h * batch_size + pad_t);
					int wend = MIN(wstart + kernel_w, in_w + pad_l);

					//changed by lfwang
					int pool_size;
					if (strcmp(pSevParam->achPoolAvePadMethod, "same") == 0)
						pool_size = (hend - hstart) * (wend - wstart);
					else
						pool_size = 0;

					int out_ind = (c * out_h * batch_size + ph) * out_w + pw;

					//for (int n = 0; n < batch_size; ++n)
					{
						double* input_data_tmp = input_data ;//+ n * out_c * in_h * in_w;
						double* output_data_tmp = output_data;// + n * out_c * out_h * out_w;
						double tmp = 0.;
						for (int h = hstart; h < hend; ++h)
						{
							for (int w = wstart; w < wend; ++w)
							{
								if (h < 0 || h >= in_h * batch_size || w < 0 || w >= in_w)
								{
									if (strcmp(pSevParam->achPoolAvePadMethod, "same") == 0)
										tmp += pad_val;
								}//changed by lfwang
								else
								{
									int index = (c * in_h * batch_size + h) * in_w + w;
									tmp += input_data_tmp[index];
									//added by lfwang
									if (strcmp(pSevParam->achPoolAvePadMethod, "valid") == 0)
										pool_size++;
								}
							}
						}
						output_data_tmp[out_ind] = tmp / pool_size;
					}
				}
			}
		}
	}
	else
	{
		printf("Not supported.\n");
	}
}

AR_S32 UpsampleCalc(AR_DOUBLE * pdIn, AR_DOUBLE * pdOut, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_SEVCALLBACK_PARAM_S*pSevParam)
{
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InHeight = pSevParam->u32UpsampleInH;
	AR_U32 u32InWidth = pSevParam->u32UpsampleInW;
	AR_U32 u32InChannel = pSevParam->u32UpsampleInC;

	AR_U32 u32OutHeight = pSevParam->u32UpsampleOutH;
	AR_U32 u32OutWidth = pSevParam->u32UpsampleOutW;

	AR_U32 u32InDataSize = u32Batch * u32InChannel * u32InHeight * u32InWidth;
	AR_U32 u32CoordTransMode = pSevParam->u32CoordTransMode;
	AR_U32 u32NearestMode = pSevParam->u32NearestMode;

	if (u32OutHeight == u32InHeight && u32OutWidth == u32InWidth)
	{
		memcpy(pdOut, pdIn, u32InDataSize * sizeof(double));
		return 0;
	}

	if (strcmp(pSevParam->achMode, "nearest") == 0)
	{
		AR_S32 * ps32NearestOfs = (AR_S32 *)malloc((u32OutHeight + u32OutWidth) * sizeof(AR_S32));

		NearestCoeffs(u32InWidth, u32OutWidth, ps32NearestOfs, u32CoordTransMode, u32NearestMode);
		NearestCoeffs(u32InHeight, u32OutHeight, ps32NearestOfs + u32OutWidth, u32CoordTransMode, u32NearestMode);

		execNearestUpsample(pdIn, pdOut, ps32NearestOfs, ps32NearestOfs + u32OutWidth,
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth);

		free(ps32NearestOfs);
	}
	//execNearestUpsample(input_data_tmp, output_data, batch_size, operator_input_channel, operator_input_height, operator_input_width, operator_output_height, operator_output_width, scale_h, scale_w);
	else if (strcmp(pSevParam->achMode, "bilinear") == 0)// bilinear
	{
		AR_S32 * ps32BilinearOfs = (AR_S32 *)malloc((u32OutHeight + u32OutWidth) * sizeof(AR_S32));
		AR_DOUBLE * pdBilinearScale = (AR_DOUBLE *)malloc((u32OutHeight + u32OutWidth) * sizeof(AR_DOUBLE) * 2);

		LinearCoeffs(u32InWidth, u32OutWidth, ps32BilinearOfs, pdBilinearScale, u32CoordTransMode);
		LinearCoeffs(u32InHeight, u32OutHeight, ps32BilinearOfs + u32OutWidth, pdBilinearScale + u32OutWidth * 2, u32CoordTransMode);

		execBilinearUpsample(pdIn, pdOut, pdBilinearScale, ps32BilinearOfs, pdBilinearScale + u32OutWidth * 2, ps32BilinearOfs + u32OutWidth,
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth);

		free(ps32BilinearOfs);
		free(pdBilinearScale);
	}
	else if (strcmp(pSevParam->achMode, "bicubic") == 0)// bicubic
	{
		AR_S32 * pBicubicOfs = (AR_S32 *)malloc((u32OutHeight + u32OutWidth) * sizeof(AR_S32));
		AR_DOUBLE * pdBicubicScale = (AR_DOUBLE *)malloc((u32OutHeight + u32OutWidth) * sizeof(AR_DOUBLE) * 4);

		CubicCoeffs(u32InWidth, u32OutWidth, pBicubicOfs, pdBicubicScale);
		CubicCoeffs(u32InHeight, u32OutHeight, pBicubicOfs + u32OutWidth, pdBicubicScale + u32OutWidth * 4);

		execBicubicUpsample(pdIn, pdOut, pdBicubicScale, pBicubicOfs, pdBicubicScale + u32OutWidth * 4, pBicubicOfs + u32OutWidth,
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth);

		free(pBicubicOfs);
		free(pdBicubicScale);
	}
	else
	{
		printf("unsupported upsample type\n");
	}

	return 0;
}

AR_S32 ArCnnOperatorSevCallback(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
    AR_NPU_SEVCALLBACK_PARAM_S*pSevParam = (AR_NPU_SEVCALLBACK_PARAM_S*) (pOpParams);
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;
    AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
    AR_DOUBLE dInScale = pCBParams->astInputTensor[0].dScaleFactor;
    AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm
		+ pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;
	AR_U32 u32PixelByte = u32Precision / 8;

	AR_U32 u32InDataSize = u32InChannel * u32InHeight * u32InWidth * u32Batch;
	AR_U32 u32OutDataSize = u32OutChannel * u32OutHeight * u32OutWidth * u32Batch;

	AR_U32 u32PoolOutC = pSevParam->u32PoolOutC;
	AR_U32 u32PoolOutH = pSevParam->u32PoolOutH;
	AR_U32 u32PoolOutW = pSevParam->u32PoolOutW;

	AR_U32 u32UpsampleOutC = pSevParam->u32UpsampleOutC;
	AR_U32 u32UpsampleOutH = pSevParam->u32UpsampleOutH;
	AR_U32 u32UpsampleOutW = pSevParam->u32UpsampleOutW;

	AR_CHAR * pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32PixelByte);
	if (!pInNchw)
	{
		printf("Malloc input nchw failed.\n");
		return -1;
	}

	AR_CHAR * pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32PixelByte);
	if (!pOutNchw)
	{
		printf("Malloc output nchw failed.\n");
		free(pInNchw);
		return -1;
	}

	AR_DOUBLE * pdInput = (AR_DOUBLE *)malloc(u32InDataSize * sizeof(AR_DOUBLE));
	if (!pdInput)
	{
		printf("Malloc double input failed.\n");
		free(pInNchw);
		free(pOutNchw);
		return -1;
	}

	AR_DOUBLE * pdOutput = (AR_DOUBLE *)malloc(u32OutDataSize * sizeof(AR_DOUBLE));
	if (!pdOutput)
	{
		printf("Malloc double output failed.\n");
		free(pInNchw);
		free(pOutNchw);
		free(pdInput);
		return -1;
	}

	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);
	dcnn_exec_scale(pInNchw, pdInput, u32InDataSize, dInScale, s32InZeroPoint, u32Precision);

	//first do pool
	if (strcmp(pSevParam->achLayerType[0], "pool") == 0)
	{
		AR_U32 u32MidSize = u32PoolOutC * u32PoolOutH * u32PoolOutW;
		AR_DOUBLE * pdMid = (AR_DOUBLE *)malloc(u32MidSize * sizeof(AR_DOUBLE));
		if (!pdMid)
		{
			printf("Malloc double mid failed.\n");
			free(pdInput);
			free(pdOutput);
			free(pInNchw);
			free(pOutNchw);
			return -1;
		}

		//first pool, second upsample
		PoolCalculate(pdInput, pdMid, 0, pCBParams, pSevParam);
		//dcnn_exec_scale(pMid, pdMid, u32MidSize, dInScale, s32InZeroPoint, u32Precision);

		UpsampleCalc(pdMid, pdOutput, pCBParams, pSevParam); //could not use cbparams because this routine will use upsample params but here we only have sevcallback params

		free(pdMid);
	}
	else
	{
		//first upsample, second pool
		AR_U32 u32MidSize = u32UpsampleOutC * u32UpsampleOutH * u32UpsampleOutW;
		AR_DOUBLE * pdMid = (AR_DOUBLE *)malloc(u32MidSize * sizeof(AR_DOUBLE));
		if (!pdMid)
		{
			printf("Malloc double mid failed.\n");
			free(pdInput);
			free(pdOutput);
			free(pInNchw);
			free(pOutNchw);
			return -1;
		}

		UpsampleCalc(pdInput, pdMid, pCBParams, pSevParam);

		PoolCalculate(pdMid, pdOutput, 0, pCBParams, pSevParam);

		free(pdMid);
	}

	dcnn_exec_quantization(pdOutput, pOutNchw, u32OutDataSize, dOutScale, s32OutZeroPoint, u32Precision);
	//dcnn_exec_limit(pOutNchw, u32OutDataSize, (int)pow((float)2, (float)(u32Precision - 1)) - 1, (int)-pow((float)2, (float)(u32Precision - 1)), u32Precision);
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);

	free(pInNchw);
	free(pOutNchw);
	free(pdInput);
	free(pdOutput);

	return 0;
}

AR_CNN_OPERATOR_DECLARE(ArCnnOperatorSevCallback) = {
    .pName = "sercallback",
    .pCnnCbHandler = ArCnnOperatorSevCallback,
};
