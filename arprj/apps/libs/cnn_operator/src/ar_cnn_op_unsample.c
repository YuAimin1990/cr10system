#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"


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

AR_S32 AR_CNN_OperatorUpsample(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
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
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm
		+ pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;

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


static AR_U32  StringToCoordinateTransformationMode(AR_CHAR* coordinate_transform_mode_name)
{
    if (strcmp(coordinate_transform_mode_name ,"asymmetric") ==0){
      return E_DCNN_UPSAMPLE_ASYMMETRIC;
    }
    if (strcmp(coordinate_transform_mode_name , "pytorch_half_pixel") ==0) {
      return E_DCNN_UPSAMPLE_PYTORCH_HALF_PIXEL;
    }
    if (strcmp(coordinate_transform_mode_name , "tf_half_pixel_for_nn") ==0) {
      return E_DCNN_UPSAMPLE_TF_HALF_PIEXL_FOR_NN;
    }
    if (strcmp(coordinate_transform_mode_name , "align_corners") ==0) {
      return E_DCNN_UPSAMPLE_ALIGN_CORNERS;
    }
    if (strcmp(coordinate_transform_mode_name , "tf_crop_and_resize") ==0) {
      return E_DCNN_UPSAMPLE_TF_CROP_AND_RESIZE;
    }
    if (strcmp(coordinate_transform_mode_name , "half_pixel") ==0) {
      return E_DCNN_UPSAMPLE_HALF_PIXEL;
    }
    printf("coordinate_transform_mode:%s is not supportted!",coordinate_transform_mode_name);
  }

static AR_U32  StringToNearestMode(AR_CHAR* nearest_mode_name) {
    if (strcmp(nearest_mode_name ,"round_prefer_floor") ==0) {
      return E_DCNN_UPSAMPLE_ROUND_PREFER_FLOOR;
    } else if (strcmp(nearest_mode_name , "round_prefer_ceil") ==0) {
      return E_DCNN_UPSAMPLE_ROUND_PREFER_CEIL;
    } else if (strcmp(nearest_mode_name , "floor") ==0) {
      return E_DCNN_UPSAMPLE_FLOOR;
    } else if (strcmp(nearest_mode_name , "ceil") ==0) {
      return E_DCNN_UPSAMPLE_CEIL;
    } else if (strcmp(nearest_mode_name , "") ==0){
      return E_DCNN_UPSAMPLE_SIMPLE;
    }
    printf("nearest_mode:%s is not supported!",nearest_mode_name);
  }


AR_S32 _AR_CNN_ResizeForward(AR_FLOAT * pdIn, AR_FLOAT * pdOut, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_RESIZE_PARAM_S *pResizeParam)
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

	AR_U32 u32CoordTransMode = StringToCoordinateTransformationMode(pResizeParam->achCoordTransMode);
	AR_U32 u32NearestMode = StringToNearestMode(pResizeParam->achNearestMode);

	//printf("u32CoordTransMode %s \r\n", pResizeParam->achCoordTransMode);
	//printf("u32NearestMode %s \r\n", pResizeParam->achNearestMode);
	//printf("u32Mode %s \r\n",pResizeParam->achMode);

	if (u32OutHeight == u32InHeight && u32OutWidth == u32InWidth)
	{
		memcpy(pdOut, pdIn, u32InDataSize * sizeof(AR_FLOAT));
		return 0;
	}

	if (strcmp(pResizeParam->achMode, "nearest") == 0)
	{
		AR_S32 * ps32NearestOfs = (AR_S32 *)malloc( (u32OutHeight + u32OutWidth) * sizeof(AR_S32));

		OnnxNearestCoeffs(u32InWidth, u32OutWidth, ps32NearestOfs, u32CoordTransMode, u32NearestMode);
		OnnxNearestCoeffs(u32InHeight, u32OutHeight, ps32NearestOfs + u32OutWidth, u32CoordTransMode, u32NearestMode);

		execOnnxNearestUpsample(pdIn, pdOut, ps32NearestOfs, ps32NearestOfs + u32OutWidth,
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth);

		free(ps32NearestOfs);
	}
	//execNearestUpsample(input_data_tmp, output_data, batch_size, operator_input_channel, operator_input_height, operator_input_width, operator_output_height, operator_output_width, scale_h, scale_w);
	else if (strcmp(pResizeParam->achMode, "linear") == 0)// bilinear
	{
		AR_S32 *ps32Bilinear_input_width_mul_y = (AR_S32 *)malloc(u32OutHeight * 2 * sizeof(AR_S32));
		AR_S32 *ps32Bilinear_in_x              = (AR_S32 *)malloc(u32OutWidth * 2 * sizeof(AR_S32));
		AR_FLOAT *pdBilinear_dy                = (AR_FLOAT *)malloc(u32OutHeight * 2 * sizeof(AR_FLOAT));
		AR_FLOAT *pdBilinear_dx                = (AR_FLOAT *)malloc(u32OutWidth * 2 * sizeof(AR_FLOAT));
		AR_FLOAT *pdBilinear_ori_y             = (AR_FLOAT *)malloc(u32OutHeight * sizeof(AR_FLOAT));
		AR_FLOAT *pdBilinear_ori_x             = (AR_FLOAT *)malloc(u32OutWidth * sizeof(AR_FLOAT));
		
		OnnxLinearCoeffsX(u32InWidth, u32OutWidth, pdBilinear_ori_x, ps32Bilinear_in_x, pdBilinear_dx, u32CoordTransMode);
		OnnxLinearCoeffsY(u32InHeight, u32OutHeight, pdBilinear_ori_y, ps32Bilinear_input_width_mul_y, pdBilinear_dy, u32CoordTransMode);
		execOnnxBilinearUpsample(pdIn, pdOut, pdBilinear_ori_y, pdBilinear_ori_x, ps32Bilinear_in_x, ps32Bilinear_input_width_mul_y, pdBilinear_dx, pdBilinear_dy, 
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth);
		
		free(ps32Bilinear_input_width_mul_y);
		free(ps32Bilinear_in_x);
		free(pdBilinear_dy);
		free(pdBilinear_dx);	
		free(pdBilinear_ori_y);
		free(pdBilinear_ori_x);
	}
	else if (strcmp(pResizeParam->achMode, "cubic") == 0)// bicubic
	{
		AR_S32 CubicModeGridLength  = 4;
		AR_FLOAT *pdCubic_coeffs_y  = (AR_FLOAT *)malloc(u32OutHeight * sizeof(AR_FLOAT) * CubicModeGridLength);
		AR_FLOAT *pdCubic_coeffs_x  = (AR_FLOAT *)malloc(u32OutWidth * sizeof(AR_FLOAT) * CubicModeGridLength);
		AR_S32   *pdCubic_int_y     = (AR_S32 *)malloc(u32OutHeight * sizeof(AR_S32));
		AR_S32   *pdCubic_int_x     = (AR_S32 *)malloc(u32OutWidth * sizeof(AR_S32));

		OnnxCubicCoeffs(u32InWidth, u32OutWidth, pdCubic_coeffs_x, pdCubic_int_x, u32CoordTransMode);
		OnnxCubicCoeffs(u32InHeight, u32OutHeight, pdCubic_coeffs_y, pdCubic_int_y, u32CoordTransMode);
		execOnnxBicubicUpsample(pdIn, pdOut, pdCubic_coeffs_y, pdCubic_coeffs_x, pdCubic_int_y, pdCubic_int_x,
			u32Batch, u32InChannel, u32InHeight, u32InWidth, u32OutHeight, u32OutWidth, CubicModeGridLength);

		free(pdCubic_coeffs_y);
		free(pdCubic_coeffs_x);
		free(pdCubic_int_y);
		free(pdCubic_int_x);
	}
	else
	{
		printf("unsupported upsample type\n");
	}

	return 0;
}

AR_S32 AR_CNN_OperatorResize(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
    //printf("AR_CNN_OperatorResize \r\n");
	AR_NPU_RESIZE_PARAM_S *pResizeParam =(AR_NPU_RESIZE_PARAM_S*)(pOpParams);

	//printf("pUpsampleParam->achCoordTransMode %s \r\n",pResizeParam->achCoordTransMode);
	///printf("pUpsampleParam->achMode %s \r\n",pResizeParam->achMode);
	//printf("pUpsampleParam->achNearestMode %s \r\n",pResizeParam->achNearestMode);

	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;
    AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;
	AR_DOUBLE dInScale = pCBParams->astInputTensor[0].dScaleFactor;
	AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;

	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm
		+ pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

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

	AR_FLOAT * pdInput = (AR_FLOAT *)malloc(u32InDataSize * sizeof(AR_FLOAT));
	if (!pdInput)
	{
		printf("Malloc input buffer failed.\n");
		free(pInNchw);
		return -1;
	}
	AR_FLOAT * pdOut = (AR_FLOAT *)malloc(u32OutDataSize * sizeof(AR_FLOAT));
	if (!pdOut)
	{
		printf("Malloc output buffer failed.\n");
		free(pInNchw);
		free(pdInput);
		return -1;
	}

	memset(pdInput, 0, u32InDataSize * sizeof(AR_FLOAT));
	memset(pdOut, 0, u32OutDataSize * sizeof(AR_FLOAT));
	//nhwc -> nchw
	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);

	//fix to float
	dcnn_exec_scale_onnx(pInNchw, pdInput, u32InDataSize, (float)dInScale, s32InZeroPoint, u32InPrecision);

	//calculate coeffs and do upsample
	_AR_CNN_ResizeForward(pdInput, pdOut, pCBParams, pResizeParam);

	//quantization
	dcnn_exec_quantization_onnx(pdOut, pOutNchw, u32OutDataSize, (float)dOutScale, s32OutZeroPoint, u32OutPrecision);
	//dcnn_exec_limit(pOutNchw, u32OutDataSize, (int)pow((double)2, (double)(u32Precision - 1)) - 1, (int)-pow((double)2, (double)(u32Precision - 1)), u32Precision);

	//nchw -> nhwc
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
	free(pInNchw);
	free(pOutNchw);
	free(pdInput);
	free(pdOut);

	return 0;
}

