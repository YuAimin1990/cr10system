
#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"

double dmcn_im2col_bilinear(const double* src, const int data_width, const int height, const int width, double h, double w){

	int h_low = floor(h);
	int w_low = floor(w);
	int h_high = h_low + 1;
	int w_high = w_low + 1;

	double lh = h - h_low, lw = w - w_low;
	double hh = 1 - lh, hw = 1 - lw;

	double v1 = 0;
	if (h_low >= 0 && w_low >= 0)
		v1 = src[h_low * data_width + w_low];
	double v2 = 0;
	if (h_low >= 0 && w_high <= width - 1)
		v2 = src[h_low * data_width + w_high];
	double v3 = 0;
	if (h_high <= height - 1 && w_low >= 0)
		v3 = src[h_high * data_width + w_low];
	double v4 = 0;
	if (h_high <= height - 1 && w_high <= width - 1)
		v4 = src[h_high * data_width + w_high];

	double w1 = hh * hw, w2 = hh * lw, w3 = lh * hw, w4 = lh * lw;
	double val = w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4;
	return val;
}



double deformable_im2col_bilinear(const double* src, const int data_width, const int height, const int width, double h, double w){

	int h_low = floor(h), w_low = floor(w);
	int h_high, w_high;
	if (h_low >= height - 1) {
		h_high = h_low = height - 1;
		h = (double)h_low;
	}
	else {
		h_high = h_low + 1;
	}

	if (w_low >= width - 1) {
		w_high = w_low = width - 1;
		w = (double)w_low;
	}
	else {
		w_high = w_low + 1;
	}

	double lh = h - h_low, lw = w - w_low;
	double hh = 1 - lh, hw = 1 - lw;

	double v1 = src[h_low * data_width + w_low];
	double v2 = src[h_low * data_width + w_high];
	double v3 = src[h_high * data_width + w_low];
	double v4 = src[h_high * data_width + w_high];

	double w1 = hh * hw, w2 = hh * lw, w3 = lh * hw, w4 = lh * lw;
	double val = w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4;
	return val;
}



void deformable_im2col_kernel(const int n, const double* data_im, const double* data_offset, const double* data_mask,
	const int height, const int width, const int kernel_h, const int kernel_w,
	const int pad_h, const int pad_w,
	const int stride_h, const int stride_w,
	const int dilation_h, const int dilation_w,
	const int channel_per_deformable_group,
	const int height_col, const int width_col,
	double* data_col){

	for (int index = 0; index < n; index++) {
		// index index of output matrix
		const int w_col = index % width_col;
		const int h_col = (index / width_col) % height_col;
		const int c_im = (index / width_col) / height_col;
		const int c_col = c_im * kernel_h * kernel_w;

		// compute deformable group index
		const int deformable_group_index = c_im / channel_per_deformable_group;

		const int h_in = h_col * stride_h - pad_h;
		const int w_in = w_col * stride_w - pad_w;

		double* data_col_ptr = data_col + (c_col * height_col + h_col) * width_col + w_col;

		const double* data_im_ptr = data_im + (c_im * height + h_in) * width + w_in;
		const double* data_offset_ptr = data_offset + deformable_group_index * 2 * kernel_h * kernel_w * height_col * width_col;
		const double* data_mask_ptr = data_mask ? data_mask + deformable_group_index * kernel_h * kernel_w * height_col * width_col : NULL;

		for (int i = 0; i < kernel_h; ++i) {
			for (int j = 0; j < kernel_w; ++j) {
				const int data_offset_h_index = ((2 * (i * kernel_w + j)) * height_col + h_col) * width_col + w_col;
				const int data_offset_w_index = ((2 * (i * kernel_w + j) + 1) * height_col + h_col) * width_col + w_col;
				const int data_mask_hw_index = data_mask ? ((i * kernel_w + j) * height_col + h_col) * width_col + w_col : 0;
				const double offset_h = data_offset_ptr[data_offset_h_index];
				const double offset_w = data_offset_ptr[data_offset_w_index];
				const double mask = data_mask ? data_mask_ptr[data_mask_hw_index] : 0.;
				double val = 0.;
				const double h_im = h_in + i * dilation_h + offset_h;
				const double w_im = w_in + j * dilation_w + offset_w;
				if (h_im >= 0 && w_im >= 0 && h_im < height && w_im < width) {

					if (data_mask_ptr)
						val = dmcn_im2col_bilinear(data_im_ptr, width, height, width, h_im, w_im);
					else{
						const double map_h = i * dilation_h + offset_h;
						const double map_w = j * dilation_w + offset_w;
						const int cur_height = height - h_in;
						const int cur_width = width - w_in;
						val = deformable_im2col_bilinear(data_im_ptr, width, cur_height, cur_width, map_h, map_w);
					}
				}
				*data_col_ptr = data_mask ? val * mask : val;
				data_col_ptr += height_col * width_col;
			}
		}
	}
}


void deformable_im2col(const double* data_im, const double* data_offset, const double* data_mask, const int channels,
	const int height, const int width, const int kernel_h, const int kernel_w,
	const int pad_h, const int pad_w,
	const int stride_h, const int stride_w,
	const int dilation_h, const int dilation_w,
	const int deformable_group,
	double* data_col) {

	// We are going to launch channels * height_col * width_col kernels, each
	// kernel responsible for copying a single-channel grid.
	int height_col = (height + 2 * pad_h -
		(dilation_h * (kernel_h - 1) + 1)) / stride_h + 1;
	int width_col = (width + 2 * pad_w -
		(dilation_w * (kernel_w - 1) + 1)) / stride_w + 1;
	int num_kernels = channels * height_col * width_col;
	int channel_per_deformable_group = channels / deformable_group;

	deformable_im2col_kernel(num_kernels, data_im, data_offset, data_mask, height, width, kernel_h, kernel_w, pad_h,
		pad_w, stride_h, stride_w, dilation_h, dilation_w, channel_per_deformable_group, height_col,
		width_col, data_col);
}



AR_S32 _AR_CNN_DeformableForward(AR_DOUBLE * pdIn, AR_DOUBLE * pdOffset, AR_DOUBLE * pdMask, AR_DOUBLE * pdOut, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_DEFORMABLE_PARAM_S *pDeformableParam)
{
	int batch_size = pCBParams->astInputTensor[0].u32Num;

	int channel_in = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;
	int height_in = pCBParams->astInputTensor[0].u32Height;
	int width_in = pCBParams->astInputTensor[0].u32Width;
	//int bottom_single_size = channel_in * height_in * width_in;
	double* bottom_data = pdIn;

	//int channel_offset = pCBParams->astInputTensor[1].u32KNormNum * pCBParams->astInputTensor[1].u32KSizeNorm + pCBParams->astInputTensor[1].u32KSizeLast;
	//int height_offset = pCBParams->astInputTensor[1].u32Height;
	//int width_offset = pCBParams->astInputTensor[1].u32Width;
	//int offset_single_size = channel_offset * height_offset * width_offset;
	double* offset_data = pdOffset;

	//int channel_mask = pCBParams->astInputTensor[2].u32KNormNum * pCBParams->astInputTensor[2].u32KSizeNorm + pCBParams->astInputTensor[2].u32KSizeLast;
	//int height_mask = pCBParams->astInputTensor[2].u32Height;
	//int width_mask = pCBParams->astInputTensor[2].u32Width;
	//int mask_single_size = channel_mask * height_mask * width_mask;
	double* mask_data = pdMask;

	//int channel_out = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;
	//int height_out = pCBParams->astOutputTensor[0].u32Height;
	//int width_out = pCBParams->astOutputTensor[0].u32Width;
	//int bottom_col_single_size = channel_out * height_out * width_out;
	double* bottom_col_data = pdOut;

	int kernel_h = pDeformableParam->u32KernelH;
	int kernel_w = pDeformableParam->u32KernelW;
	int pad_t = pDeformableParam->u32PadT;
	int pad_l = pDeformableParam->u32PadL;
	int stride_h = pDeformableParam->u32StrideH;
	int stride_w = pDeformableParam->u32StrideW;
	int dilation_h = pDeformableParam->u32DilateH;
	int dilation_w = pDeformableParam->u32DilateW;
	int deformable_group = pDeformableParam->u32DeformableGroup;

	//for (int n = 0; n < batch_size; ++n) {
		double* bottom_data_ptr = bottom_data ;//+ n * bottom_single_size;
		double* offset_data_ptr = offset_data ;//+ n * offset_single_size;
		double* mask_data_ptr = pdMask ? mask_data/* + n * mask_single_size*/ : NULL;
		double* bottom_col_data_ptr = bottom_col_data ;//+ n * bottom_col_single_size;
		deformable_im2col(bottom_data_ptr, //data
			offset_data_ptr,//offset
			mask_data_ptr,//mask
			channel_in,
			height_in * batch_size, width_in, kernel_h, kernel_w,
			pad_t, pad_l, stride_h, stride_w,
			dilation_h, dilation_w, deformable_group,
			bottom_col_data_ptr); // data_col
	//}

	return 0;
}


AR_S32 ArCnnOperatorDeformable(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_NPU_DEFORMABLE_PARAM_S *pDeformableParam = (AR_NPU_DEFORMABLE_PARAM_S*) (pOpParams);

	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm
		+ pCBParams->astInputTensor[0].u32KSizeLast;
    AR_DOUBLE dInScale = pCBParams->astInputTensor[0].dScaleFactor;
    AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;
	AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

	AR_U32 u32OffsetHeight = pCBParams->astInputTensor[1].u32Height;
	AR_U32 u32OffsetWidth = pCBParams->astInputTensor[1].u32Width;
	AR_U32 u32OffsetChannel = pCBParams->astInputTensor[1].u32KNormNum * pCBParams->astInputTensor[1].u32KSizeNorm
		+ pCBParams->astInputTensor[1].u32KSizeLast;
    AR_DOUBLE dOffScale = pCBParams->astInputTensor[1].dScaleFactor;
    AR_S32 s32OffZeroPoint = pCBParams->astInputTensor[1].s32ZeroPoint;
    AR_U32 pOffset = pIn + pCBParams->astInputTensor[1].u32Offset;

	AR_U32 u32MaskHeight = pCBParams->astInputTensor[2].u32Height;
	AR_U32 u32MaskWidth = pCBParams->astInputTensor[2].u32Width;
	AR_U32 u32MaskChannel = pCBParams->astInputTensor[2].u32KNormNum * pCBParams->astInputTensor[2].u32KSizeNorm
		+ pCBParams->astInputTensor[2].u32KSizeLast;
	AR_U32 pMask = pIn + pCBParams->astInputTensor[2].u32Offset;

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

	AR_U32 u32InDataSize = u32InChannel * u32InHeight * u32InWidth * u32Batch;
	AR_U32 u32OffsetDataSize = u32OffsetChannel * u32OffsetHeight * u32OffsetWidth * u32Batch;
	AR_U32 u32MaskDataSize = u32MaskChannel * u32MaskHeight * u32MaskWidth * u32Batch;
	AR_U32 u32OutDataSize = u32OutChannel * u32OutHeight * u32OutWidth * u32Batch;

	AR_CHAR * pInNchw = NULL;
	AR_CHAR * pOffNchw = NULL;
	AR_CHAR * pOutNchw = NULL;
	AR_DOUBLE * pdInput = NULL;
	AR_DOUBLE * pdOffset = NULL;
	AR_DOUBLE * pdOut = NULL;
	AR_DOUBLE * pdMask = NULL;

	pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32InPixelByte);
	if (!pInNchw)
	{
		printf("Malloc input nchw failed.\n");
		goto free_exit;
	}

	pOffNchw = (AR_CHAR *)malloc(u32OffsetDataSize * u32InPixelByte);
	if (!pOffNchw)
	{
		printf("Malloc offset nchw failed.\n");
		goto free_exit;
	}

	pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32OutPixelByte);
	if (!pOutNchw)
	{
		printf("Malloc output nchw failed.\n");
		goto free_exit;
	}

	pdInput = (AR_DOUBLE *)malloc(u32InDataSize * sizeof(AR_DOUBLE));
	if (!pdInput)
	{
		printf("Malloc input offset buffer failed.\n");
		goto free_exit;
	}

	pdOffset = (AR_DOUBLE *)malloc(u32OffsetDataSize * sizeof(AR_DOUBLE));
	if (!pdOffset)
	{
		printf("Malloc offset double buffer failed.\n");
		goto free_exit;
	}

	pdOut = (AR_DOUBLE *)malloc(u32OutDataSize * sizeof(AR_DOUBLE));
	if (!pdOut)
	{
		printf("Malloc output double buffer failed.\n");
		goto free_exit;
	}

	memset(pdInput, 0, u32InDataSize * sizeof(AR_DOUBLE));
	memset(pdOffset, 0, u32OffsetDataSize * sizeof(AR_DOUBLE));
	memset(pdOut, 0, u32OutDataSize * sizeof(AR_DOUBLE));

	//nhwc -> nchw
	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);
	dcnn_hwc_to_chw((AR_CHAR *)pOffset, pOffNchw, &pCBParams->astInputTensor[1]);

	//fix to double
	dcnn_exec_scale(pInNchw, pdInput, u32InDataSize, dInScale, s32InZeroPoint, u32InPrecision);
	dcnn_exec_scale(pOffNchw, pdOffset, u32OffsetDataSize, dOffScale, s32OffZeroPoint, u32InPrecision);

	if (u32MaskDataSize)
	{
		AR_CHAR * pMaskNchw = (AR_CHAR *)malloc(u32MaskDataSize * u32InPixelByte);
		if (!pMaskNchw)
		{
			printf("Malloc mask nchw failed.\n");
			goto free_exit;
		}

		pdMask = (AR_DOUBLE *)malloc(u32MaskDataSize * sizeof(AR_DOUBLE));
		if (!pdMask)
		{
			printf("Malloc mask double failed.\n");
			goto free_exit;
		}

		AR_DOUBLE dMaskScale = pCBParams->astInputTensor[2].dScaleFactor;
		AR_S32 s32MaskZeroPoint = pCBParams->astInputTensor[2].s32ZeroPoint;

		dcnn_hwc_to_chw((AR_CHAR *)pMask, pMaskNchw, &pCBParams->astInputTensor[2]);
		dcnn_exec_scale(pMaskNchw, pdMask, u32MaskDataSize, dMaskScale, s32MaskZeroPoint, u32InPrecision);
		free(pMaskNchw);
	}

	_AR_CNN_DeformableForward(pdInput,pdOffset, pdMask, pdOut, pCBParams, pDeformableParam);

	//quantization
	dcnn_exec_quantization(pdOut, pOutNchw, u32OutDataSize, dOutScale, s32OutZeroPoint, u32OutPrecision);
	//dcnn_exec_limit(pOutNchw, u32OutDataSize, (int)pow((double)2, (double)(u32Precision - 1)) - 1, (int)-pow((double)2, (double)(u32Precision - 1)), u32Precision);

	//nchw -> nhwc
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);

free_exit:
	if (pInNchw)
		free(pInNchw);
	if (pOffNchw)
		free(pOffNchw);
	if (pOutNchw)
		free(pOutNchw);
	if (pdInput)
		free(pdInput);
	if (pdOffset)
		free(pdOffset);
	if (pdOut)
		free(pdOut);
	if (pdMask)
		free(pdMask);

	return 0;
}



AR_CNN_OPERATOR_DECLARE(ArCnnOperatorDeformable) = {
    .pName = "deformable",
    .pCnnCbHandler = ArCnnOperatorDeformable,
};
