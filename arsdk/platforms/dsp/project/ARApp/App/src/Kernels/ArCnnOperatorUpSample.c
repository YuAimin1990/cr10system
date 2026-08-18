

#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"

AR_S32 _AR_CNN_UpsampleForward(AR_DOUBLE * pdIn, AR_DOUBLE * pdOut, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_UPSAMPLE_PARAM_S *pUpsampleParam)
{
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;

	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	//AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm
	//	+ pCBParams->astOutputTensor[0].u32KSizeLast;

	AR_U32 u32InDataSize = u32Batch * u32InChannel * u32InHeight * u32InWidth;
	AR_U32 u32CoordTransMode = pUpsampleParam->u32CoordTransMode;
	AR_U32 u32NearestMode = pUpsampleParam->u32NearestMode;

	if (u32OutHeight == u32InHeight && u32OutWidth == u32InWidth)
	{
		memcpy(pdOut, pdIn, u32InDataSize * sizeof(AR_DOUBLE));
		return 0;
	}

	if (strcmp(pUpsampleParam->achMode, "nearest") == 0)
	{
		AR_S32 * ps32NearestOfs = (AR_S32 *)malloc( (u32OutHeight + u32OutWidth) * sizeof(AR_S32));

		NearestCoeffs(u32InWidth, u32OutWidth, ps32NearestOfs, u32CoordTransMode, u32NearestMode);
		NearestCoeffs(u32InHeight, u32OutHeight, ps32NearestOfs + u32OutWidth, u32CoordTransMode, u32NearestMode);

		execNearestUpsample(pdIn, pdOut, ps32NearestOfs, ps32NearestOfs + u32OutWidth,
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth);

		free(ps32NearestOfs);
	}
	//execNearestUpsample(input_data_tmp, output_data, batch_size, operator_input_channel, operator_input_height, operator_input_width, operator_output_height, operator_output_width, scale_h, scale_w);
	else if (strcmp(pUpsampleParam->achMode, "bilinear") == 0)// bilinear
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
	else if (strcmp(pUpsampleParam->achMode, "bicubic") == 0)// bicubic
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

AR_S32 ArCnnOperatorUpSample(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_NPU_UPSAMPLE_PARAM_S *pUpsampleParam =(AR_NPU_UPSAMPLE_PARAM_S*)(pOpParams);

	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;
    AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;
	AR_DOUBLE dInScale = pCBParams->astInputTensor[0].dScaleFactor;
	AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm
		+ pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
	AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;





	AR_U32 u32InPixelByte = u32InPrecision / 8;
	AR_U32 u32OutPixelByte = u32OutPrecision / 8;

	AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;

	AR_U32 u32InDataSize = u32InChannel * u32InChSize * u32Batch;
	AR_U32 u32OutDataSize = u32OutChannel * u32OutChSize * u32Batch;

	AR_CHAR * pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32InPixelByte);
	if (!pInNchw)
	{
		printf("Malloc input nchw failed.\n");
		return -1;
	}

	AR_CHAR * pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32OutPixelByte);
	if (!pOutNchw)
	{
		printf("Malloc output nchw failed.\n");
		free(pInNchw);
		return -1;
	}

	AR_DOUBLE * pdInput = (AR_DOUBLE *)malloc(u32InDataSize * sizeof(AR_DOUBLE));
	if (!pdInput)
	{
		printf("Malloc input buffer failed.\n");
		free(pInNchw);
		return -1;
	}
	AR_DOUBLE * pdOut = (AR_DOUBLE *)malloc(u32OutDataSize * sizeof(AR_DOUBLE));
	if (!pdOut)
	{
		printf("Malloc output buffer failed.\n");
		free(pInNchw);
		free(pdInput);
		return -1;
	}

	memset(pdInput, 0, u32InDataSize * sizeof(AR_DOUBLE));
	memset(pdOut, 0, u32OutDataSize * sizeof(AR_DOUBLE));
	//nhwc -> nchw
	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);

	//fix to float
	dcnn_exec_scale(pInNchw, pdInput, u32InDataSize, dInScale, s32InZeroPoint, u32InPrecision);

	//calculate coeffs and do upsample
	_AR_CNN_UpsampleForward(pdInput, pdOut, pCBParams, pUpsampleParam);

	//quantization
	dcnn_exec_quantization(pdOut, pOutNchw, u32OutDataSize, dOutScale, s32OutZeroPoint, u32OutPrecision);
	//dcnn_exec_limit(pOutNchw, u32OutDataSize, (int)pow((double)2, (double)(u32Precision - 1)) - 1, (int)-pow((double)2, (double)(u32Precision - 1)), u32Precision);

	//nchw -> nhwc
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
	free(pInNchw);
	free(pOutNchw);
	free(pdInput);
	free(pdOut);

	return 0;
}

AR_CNN_OPERATOR_DECLARE(ArCnnOperatorUpSample) = {
    .pName = "ArCnnOperatorUpSample",
    .pCnnCbHandler = ArCnnOperatorUpSample,
};


