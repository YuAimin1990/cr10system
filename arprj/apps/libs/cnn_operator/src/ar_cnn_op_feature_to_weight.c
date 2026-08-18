#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"

//#define MEMSET_TOTAL

#define BITS_PER_CBUF	1024
#define BYTES_PER_CBUF	BITS_PER_CBUF / 8
#define BYTES_HALF_CBUF BYTES_PER_CBUF / 2

AR_U32 u32SocVersion = 1;  //1:9341  2:9311

AR_S32 printOpParamsInfo(void* pOpParams)
{
	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);
	printf("-------------- feature_to_weight params info --------------\n");
	printf("u32CdmaKernelBytesToal		: %u\n", pFeatureToWeightParam->u32CdmaKernelBytesToal);
	printf("u32CdmaKernelKBytesNorm		: %u\n", pFeatureToWeightParam->u32CdmaKernelKBytesNorm);
	printf("u32CdmaKernelKNormNum		: %u\n", pFeatureToWeightParam->u32CdmaKernelKNormNum);
	printf("u32CdmaKernelKSizeLast		: %u\n", pFeatureToWeightParam->u32CdmaKernelKSizeLast);
	printf("u32CdmaKerneLKSizeNorm		: %u\n", pFeatureToWeightParam->u32CdmaKerneLKSizeNorm);

	printf("u32Precision				: %u\n", pFeatureToWeightParam->u32Precision);
	printf("u32ConvCmacKernelKNorm		: %u\n", pFeatureToWeightParam->u32ConvCmacKernelKNorm);
	printf("u32ConvFoldingBit			: %u\n", pFeatureToWeightParam->u32ConvFoldingBit);
	printf("u32ConvFtFoldingNum			: %u\n", pFeatureToWeightParam->u32ConvFtFoldingNum);
	printf("u32ConvGroupInputChannel	: %u\n", pFeatureToWeightParam->u32ConvGroupInputChannel);
	printf("u32ConvGroupNum				: %u\n", pFeatureToWeightParam->u32ConvGroupNum);
	printf("u32ConvGroupOuutputChannel	: %u\n", pFeatureToWeightParam->u32ConvGroupOuutputChannel);
	printf("u32ConvKernelH				: %u\n", pFeatureToWeightParam->u32ConvKernelH);
	printf("u32ConvKernelW				: %u\n", pFeatureToWeightParam->u32ConvKernelW);
	printf("u32ConvKernelSplitNum		: %u\n", pFeatureToWeightParam->u32ConvKernelSplitNum);

	printf("bConvGroupPartitionEn		: %u\n", pFeatureToWeightParam->bConvGroupPartitionEn);
	printf("bConvWtPartitionEn			: %u\n", pFeatureToWeightParam->bConvWtPartitionEn);
	printf("bConvDeconvEn				: %u\n", pFeatureToWeightParam->bConvDeconvEn);
	printf("bConvDeconvOptimizeEn		: %u\n", pFeatureToWeightParam->bConvDeconvOptimizeEn);

	printf("achFtwMethod				: %s\n", pFeatureToWeightParam->achFtwMethod);
	printf("achConvMode         		: %s\n", pFeatureToWeightParam->achConvMode);

	return 0;
}

AR_S32 AR_CNN_OperatorFeatureToWeight_ParamsCheck(AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);

	AR_U32 u32BitPerCbuf		= (u32SocVersion == 1)? BITS_PER_CBUF: BITS_PER_CBUF/2;
	AR_U32 u32BytesPerPixel		= pCBParams->astInputTensor[0].u32Precision / 8;
	AR_U32 u32InKSizeNorm		= pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32FoldingNumPerCbuf = u32BitPerCbuf / pFeatureToWeightParam->u32ConvFoldingBit;

	if (pCBParams->astInputTensor[0].u32Precision != pCBParams->astOutputTensor[0].u32Precision) {
		printf("unsupported: input tensor precision is not equal to output tensor precision!\n");
		return 1;
	}
	if (((AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)pOpParams)->bConvGroupPartitionEn) {
		printf("unsupported: group partition mode!\n");
		return 1;
	}
	if (((AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)pOpParams)->u32ConvGroupNum != 1) {
		printf("unsupported: group convolution!\n");
		return 1;
	}
	if (((AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)pOpParams)->u32ConvKernelH != 1 ||
		((AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)pOpParams)->u32ConvKernelW != 1) {
		printf("unsupported: conv kernel map shape is not 1x1!\n");
		return 1;
	}
	if (pFeatureToWeightParam->bConvWtPartitionEn && u32FoldingNumPerCbuf != 2) {
		printf("unsupported: cbuf not split to 2 block in wt partition mode!\n");
		return 1;
	}
	if (!pFeatureToWeightParam->bConvWtPartitionEn && u32FoldingNumPerCbuf != 1) {
		printf("uunsupported: cbuf not keep 1 block in no partition mode!\n");
		return 1;
	}
	if (pFeatureToWeightParam->bConvWtPartitionEn && u32InKSizeNorm * u32BytesPerPixel > BYTES_HALF_CBUF) {
		printf("uunsupported: input tensor k is illegal!\n");
		return 1;
	}
	if (strcmp(pFeatureToWeightParam->achConvMode, "pointwise") != 0){
		printf("uunsupported: conv mode is not pointwise!\n");
		return 1;
	}
	if (strcmp(pFeatureToWeightParam->achFtwMethod, "ftw_reshape") == 0)
	{
		if (pCBParams->astInputTensor[0].u32KSizeNorm != 1) {
			printf("uunsupported: input tensor k is not 1 which need NCHW input format!\n");
			return 1;
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorFeatureToWeight_SelfAttentionNhwcDumpWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	//print("MSG: feature_to_weight blocking-nhwc dump mode!!\n");

	AR_U32 u32InKSizeNorm		= pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InkNormNum		= pCBParams->astInputTensor[0].u32KNormNum;
	AR_U32 u32InKSizeLast		= pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32InChannel			= u32InKSizeNorm * u32InkNormNum + u32InKSizeLast;
	AR_U32 u32InWidth			= pCBParams->astInputTensor[0].u32Width;
	//AR_U32 u32Batch				= pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InKStep			= pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InRowStep			= pCBParams->astInputTensor[0].u32RowStep;
	//AR_U32 u32InTensorStep		= pCBParams->astInputTensor[0].u32TensorStep;
	AR_U32 u32BytesPerPixel		= pCBParams->astInputTensor[0].u32Precision / 8;

	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);
	AR_U32 u32OutMemBytesToTal	= pFeatureToWeightParam->u32CdmaKernelBytesToal;
	AR_U32 u32KerneLKSizeNorm	= pFeatureToWeightParam->u32CdmaKerneLKSizeNorm;
	AR_U32 u32KernelKNormNum	= pFeatureToWeightParam->u32CdmaKernelKNormNum;
	AR_U32 u32KernelKSizeLast	= pFeatureToWeightParam->u32CdmaKernelKSizeLast;
	AR_U32 u32KernelNum			= u32KerneLKSizeNorm * u32KernelKNormNum + u32KernelKSizeLast;

	AR_U32 u32CmacKNorm			= pFeatureToWeightParam->u32ConvCmacKernelKNorm;
	AR_U32 u32CmacKNormNum		= u32KernelNum / u32CmacKNorm;
	AR_U32 u32CmacKLast			= u32KernelNum - u32CmacKNorm * u32CmacKNormNum;

	AR_U32 u32FoldingNum		= pFeatureToWeightParam->u32ConvFtFoldingNum;
	AR_BOOL bWtPartitionEn		= pFeatureToWeightParam->bConvWtPartitionEn;

	AR_U32 bytes_per_cbuf  = (u32SocVersion == 1)? BYTES_PER_CBUF: BYTES_PER_CBUF/2;
	AR_U32 bytes_half_cbuf = (u32SocVersion == 1)? BYTES_HALF_CBUF: BYTES_HALF_CBUF/2;

	AR_S8* pInput  = (AR_S8*)pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_S8* pOutput = (AR_S8*)pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

#ifdef MEMSET_TOTAL
	memset((AR_S8*)pOutput, 0, u32OutMemBytesToTal);
#endif

	if (bWtPartitionEn)
	{
		AR_U32 half_cmac_k	  = u32CmacKNorm / 2;
		AR_U32 cmac_addr_step = u32FoldingNum * bytes_per_cbuf * half_cmac_k;
		AR_U32 put_kernel_num = u32CmacKNorm;
		AR_U32 perKernelCbuf  = u32FoldingNum * bytes_half_cbuf;
 		for (AR_U32 idx_n = 0; idx_n < u32CmacKNormNum + 1; ++idx_n)
		{
			if (idx_n == u32CmacKNormNum)
			{
				if (u32CmacKLast != 0)
					put_kernel_num = u32CmacKLast;
				else
					continue;
			}
			AR_S8* pOutAddr = (AR_S8*)pOutput + idx_n * cmac_addr_step;
			for (AR_U32 idx_k = 0; idx_k < put_kernel_num; ++idx_k)
			{
				AR_S8* pOutAddrTmp = pOutAddr + (idx_k % half_cmac_k) * u32FoldingNum * bytes_per_cbuf + (idx_k / half_cmac_k) * bytes_half_cbuf;
				AR_U32 curKernelIdx = idx_n * u32CmacKNorm + idx_k;
				AR_S8* pInAddr = (AR_S8*)pInput + (curKernelIdx / u32InWidth) * u32InRowStep;
				AR_U32 memCopyLen = u32InKSizeNorm * u32BytesPerPixel;
				for (AR_U32 idx_n = 0; idx_n < u32InkNormNum + 1; ++idx_n)
				{
					if (idx_n == u32InkNormNum)
						memCopyLen = u32InKSizeLast * u32BytesPerPixel;

					AR_U32 input_addr_offset = (curKernelIdx % u32InWidth) * memCopyLen;
					AR_U32 fillable_len = bytes_half_cbuf - ((idx_n * u32InKSizeNorm * u32BytesPerPixel) % bytes_half_cbuf);
					if (fillable_len > memCopyLen)
					{
						memcpy(pOutAddrTmp, pInAddr + input_addr_offset, memCopyLen * sizeof(AR_S8));
						pOutAddrTmp += memCopyLen;
					}
					else
					{
						if (fillable_len > 0)
							memcpy(pOutAddrTmp, pInAddr + input_addr_offset, fillable_len * sizeof(AR_S8));
						pOutAddrTmp += fillable_len + bytes_half_cbuf;
						if (memCopyLen - fillable_len > 0) {
							memcpy(pOutAddrTmp, pInAddr + input_addr_offset + fillable_len, (memCopyLen - fillable_len) * sizeof(AR_S8));
							pOutAddrTmp += (memCopyLen - fillable_len);
						}
					}
					pInAddr += u32InKStep;

#ifndef MEMSET_TOTAL
					if (idx_n == u32InkNormNum)
					{
						AR_U32 surplusLen = perKernelCbuf - u32InChannel * u32BytesPerPixel;
						if (surplusLen > 0 && surplusLen < bytes_half_cbuf)
							memset(pOutAddrTmp, 0, surplusLen);
					}
#endif
				}
			}
		}
#ifndef MEMSET_TOTAL
		AR_U32 step_times = u32CmacKNormNum + (u32CmacKLast == 0 ? 0 : 1);
		if (u32OutMemBytesToTal > step_times * cmac_addr_step)
		{
			AR_U32 surplusLen = u32OutMemBytesToTal - step_times * cmac_addr_step;
			memset((AR_S8*)pOutput + step_times * cmac_addr_step, 0, surplusLen);
		}
#endif
	}
	else
	{
		AR_U32 perKernelCbuf = u32FoldingNum * bytes_per_cbuf;
		for (AR_U32 idx_k = 0; idx_k < u32KernelNum; ++idx_k)
		{
			AR_S8* pOutAddr	  = pOutput + idx_k * perKernelCbuf;
			AR_S8* pInAddr	  = pInput + (idx_k / u32InWidth) * u32InRowStep;
			AR_U32 memCopyLen = u32InKSizeNorm * u32BytesPerPixel;
			for (AR_U32 idx_n = 0; idx_n < u32InkNormNum + 1; ++idx_n)
			{
				if (idx_n == u32InkNormNum)
					memCopyLen = u32InKSizeLast * u32BytesPerPixel;

				memcpy(pOutAddr, pInAddr + (idx_k % u32InWidth) * memCopyLen, memCopyLen);

				pOutAddr += memCopyLen;
				pInAddr  += u32InKStep;

#ifndef MEMSET_TOTAL
				if (idx_n == u32InkNormNum)
				{
					AR_U32 surplusLen = perKernelCbuf - u32InChannel * u32BytesPerPixel;
					if (surplusLen > 0 && surplusLen < bytes_per_cbuf)
						memset(pOutAddr, 0, surplusLen);
				}
#endif
			}
		}
#ifndef MEMSET_TOTAL
		if (u32OutMemBytesToTal > u32KernelNum * perKernelCbuf)
		{
			AR_U32 surplusLen = u32OutMemBytesToTal - u32KernelNum * perKernelCbuf;
			memset((AR_S8*)pOutput + u32KernelNum * perKernelCbuf, 0, surplusLen);
		}
#endif
	}
	
	return 0;
}

AR_S32 AR_CNN_OperatorFeatureToWeight_SelfAttentionNchwDumpWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	//print("MSG: feature_to_weight nchw dump mode!!\n");

	//AR_U32 u32InKSizeNorm		= pCBParams->astInputTensor[0].u32KSizeNorm;
	//AR_U32 u32InkNormNum		= pCBParams->astInputTensor[0].u32KNormNum;
	//AR_U32 u32InKSizeLast		= pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32InChannel			= pCBParams->astInputTensor[0].u32OriChannels;
	AR_U32 u32InHeight			= pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth			= pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32Batch				= pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InKStep			= pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InRowStep			= pCBParams->astInputTensor[0].u32RowStep;
	//AR_U32 u32InTensorStep		= pCBParams->astInputTensor[0].u32TensorStep;
	AR_U32 u32BytesPerPixel		= pCBParams->astInputTensor[0].u32Precision / 8;

	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);
	AR_U32 u32OutMemBytesToTal	= pFeatureToWeightParam->u32CdmaKernelBytesToal;
	//AR_U32 u32KerneLKSizeNorm	= pFeatureToWeightParam->u32CdmaKerneLKSizeNorm;
	//AR_U32 u32KernelKNormNum	= pFeatureToWeightParam->u32CdmaKernelKNormNum;
	//AR_U32 u32KernelKSizeLast	= pFeatureToWeightParam->u32CdmaKernelKSizeLast;
	AR_U32 u32KernelNum			= pFeatureToWeightParam->u32CdmaKerneLKSizeNorm * pFeatureToWeightParam->u32CdmaKernelKNormNum + pFeatureToWeightParam->u32CdmaKernelKSizeLast;

	AR_U32 u32CmacKNorm			= pFeatureToWeightParam->u32ConvCmacKernelKNorm;
	AR_U32 u32CmacKNormNum		= u32KernelNum / u32CmacKNorm;
	AR_U32 u32CmacKLast			= u32KernelNum - u32CmacKNorm * u32CmacKNormNum;

	AR_U32 u32FoldingNum		= pFeatureToWeightParam->u32ConvFtFoldingNum;
	AR_BOOL bWtPartitionEn		= pFeatureToWeightParam->bConvWtPartitionEn;

	AR_U32 u32ConvKernelH = pFeatureToWeightParam->u32ConvKernelH;
	AR_U32 u32ConvKernelW = pFeatureToWeightParam->u32ConvKernelW;
	AR_U32 u32ConvInputChannel = pFeatureToWeightParam->u32ConvGroupInputChannel;

	AR_U32 bytes_per_cbuf  = (u32SocVersion == 1)? BYTES_PER_CBUF: BYTES_PER_CBUF/2;
	AR_U32 bytes_half_cbuf = (u32SocVersion == 1)? BYTES_HALF_CBUF: BYTES_HALF_CBUF/2;

	AR_S8* pInput  = (AR_S8*)pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_S8* pOutput = (AR_S8*)pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

#ifdef MEMSET_TOTAL
	memset((AR_S8*)pOutput, 0, u32OutMemBytesToTal);
#endif

	if (bWtPartitionEn)
	{
		AR_U32 half_cmac_k	  = u32CmacKNorm / 2;
		AR_U32 cmac_addr_step = u32FoldingNum * bytes_per_cbuf * half_cmac_k;
		AR_U32 put_kernel_num = u32CmacKNorm;
		AR_U32 memCopyLen     = u32InWidth * u32BytesPerPixel;
		AR_U32 perKernelCbuf  = u32FoldingNum * bytes_half_cbuf;
 		for (AR_U32 idx_n = 0; idx_n < u32CmacKNormNum + 1; ++idx_n)
		{
			if (idx_n == u32CmacKNormNum)
			{
				if (u32CmacKLast != 0)
					put_kernel_num = u32CmacKLast;	
				else
					continue;
			}
			AR_S8* pOutAddr = pOutput + idx_n * cmac_addr_step;
			for (AR_U32 idx_k = 0; idx_k < put_kernel_num; ++idx_k)
			{
				AR_S8* pOutAddrTmp = pOutAddr + (idx_k % half_cmac_k) * u32FoldingNum * bytes_per_cbuf + (idx_k / half_cmac_k) * bytes_half_cbuf;
				AR_S8* pInputAddr = pInput + (idx_n * put_kernel_num + idx_k) * u32InKStep;
				for (AR_U32 idx_y = 0; idx_y < u32InHeight; ++idx_y)
				{
					AR_U32 fillable_len = bytes_half_cbuf - ((idx_y * memCopyLen) % bytes_half_cbuf);
					if (fillable_len > memCopyLen)
					{
						memcpy(pOutAddrTmp, pInputAddr, memCopyLen);
						pOutAddrTmp += memCopyLen;
					}
					else
					{
						if (fillable_len > 0)
							memcpy(pOutAddrTmp, pInputAddr, fillable_len);
						pOutAddrTmp += fillable_len + bytes_half_cbuf;

						AR_U32 fillableCopyLen = memCopyLen - fillable_len;
						AR_U32 memCopyNum = fillableCopyLen / bytes_half_cbuf;
						for (AR_U32 i = 0; i < memCopyNum + 1; ++i)
						{
							if (fillableCopyLen - memCopyNum * bytes_half_cbuf == 0)
								continue;
							AR_U32 copy_len = MIN((AR_U32)bytes_half_cbuf, fillableCopyLen - i * bytes_half_cbuf);
							memcpy(pOutAddrTmp, pInputAddr + fillable_len + i * bytes_half_cbuf, copy_len);
							pOutAddrTmp += (copy_len == bytes_half_cbuf ? bytes_per_cbuf : copy_len);
						}
					}
					pInputAddr += u32InRowStep;
#ifndef MEMSET_TOTAL
					if (idx_y == u32InHeight - 1)
					{
						AR_U32 surplusLen = perKernelCbuf - memCopyLen * u32InHeight;
						if (surplusLen > 0 && surplusLen < bytes_half_cbuf)
							memset(pOutAddrTmp, 0, surplusLen);
					}
#endif
				}
			}
		}
#ifndef MEMSET_TOTAL
		AR_U32 step_times = u32CmacKNormNum + (u32CmacKLast == 0 ? 0 : 1);
		if (u32OutMemBytesToTal > step_times * cmac_addr_step)
		{
			AR_U32 surplusLen = u32OutMemBytesToTal - step_times * cmac_addr_step;
			memset(pOutput + step_times * cmac_addr_step, 0, surplusLen);
		}
#endif
	}
	else
	{
		AR_U32 perKernelCbuf = u32FoldingNum * bytes_per_cbuf;
		if(u32ConvKernelH*u32ConvKernelW == 1){
			AR_U32 memCopyLen = u32InWidth * u32BytesPerPixel;
			AR_U32 u32CalHeight = (u32InWidth*u32InHeight != pFeatureToWeightParam->u32ConvGroupInputChannel 
				&& u32InHeight == pFeatureToWeightParam->u32ConvGroupOuutputChannel && 
				pCBParams->astInputTensor[0].u32OriChannels == 1) ? 1 : u32InHeight;
			for (AR_U32 idx_k = 0; idx_k < u32KernelNum; ++idx_k)
			{
				AR_S8* pOutAddr   = pOutput + idx_k * perKernelCbuf;
				AR_S8* pInputAddr = pInput + idx_k * u32InKStep;
				for (AR_U32 idx_y = 0; idx_y < u32CalHeight; ++idx_y)
				{
					memcpy(pOutAddr, pInputAddr, memCopyLen);
					pOutAddr += memCopyLen;
					pInputAddr += u32InRowStep;
#ifndef MEMSET_TOTAL
					if (idx_y == u32InHeight - 1)
					{
						AR_U32 surplusLen = perKernelCbuf - memCopyLen * u32InHeight;
						if (surplusLen > 0 && surplusLen < bytes_per_cbuf)
							memset(pOutAddr, 0, surplusLen);
					}
#endif
				}
			}
#ifndef MEMSET_TOTAL
			if (u32OutMemBytesToTal > u32KernelNum * perKernelCbuf)
			{
				AR_U32 surplusLen = u32OutMemBytesToTal - u32KernelNum * perKernelCbuf;
				memset(pOutput + u32KernelNum * perKernelCbuf, 0, surplusLen);
			}
#endif
		}
		else{
			AR_S8 *nchwInputBuf = (AR_S8 *)malloc(u32InHeight*u32InWidth*u32Batch*u32InChannel*u32BytesPerPixel);
			for(AR_U32 ah = 0; ah < u32Batch*u32InChannel*u32InHeight; ah++)
				memcpy(nchwInputBuf+ah*u32InWidth*u32BytesPerPixel, pInput+ah*u32InRowStep, u32InWidth*u32BytesPerPixel);
			AR_U32 u32KernelSize = u32ConvKernelH * u32ConvKernelW;
			for(AR_U32 idx_k = 0; idx_k < u32KernelNum; ++idx_k){
				AR_S8* pInputAddr = nchwInputBuf + idx_k*u32ConvInputChannel*u32KernelSize*u32BytesPerPixel;
				for(AR_U32 khw = 0; khw < u32KernelSize; khw++){
					AR_S8* pOutAddr   = pOutput + (idx_k * u32KernelSize + khw) * perKernelCbuf;
					for(AR_U32 kic = 0; kic < u32ConvInputChannel; kic++){
						memcpy(pOutAddr + kic*u32BytesPerPixel, pInputAddr + (khw+kic*u32KernelSize)*u32BytesPerPixel, u32BytesPerPixel);
					}
#ifndef MEMSET_TOTAL
					AR_U32 u32OneChannelKernelByte = u32ConvInputChannel*u32BytesPerPixel;
					if (u32OneChannelKernelByte < perKernelCbuf)
					{
						AR_U32 surplusLen = perKernelCbuf - u32OneChannelKernelByte;
						memset(pOutAddr + u32OneChannelKernelByte, 0, surplusLen);
					}
#endif
				}
			}
			free(nchwInputBuf);
		}
	}

	return 0;
}

/*****************************************************************************************************/
AR_S32 AR_CNN_OperatorFeatureToWeight_Dw2GC_GroupPartition_DumpWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_U32 u32InKSizeNorm	= pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InHeight		= pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth		= pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InKStep		= pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InRowStep		= pCBParams->astInputTensor[0].u32RowStep;

	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* opParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);

	AR_U32 u32GroupNum			= opParam->u32ConvGroupNum;
	AR_U32 u32GroupInputC		= opParam->u32ConvGroupInputChannel;
	AR_U32 u32BytesPerPixel		= opParam->u32Precision / 8;

	if (opParam->u32ConvFtFoldingNum != 1) {
		printf("ft folding num: ");
		printf(opParam->u32ConvFtFoldingNum);
		printf("\nunsupported folding num not equal to 1 in feature_to_weight reshape dw2gc group partition method\n");
		return 1;//exit(1);
	}
	if ((u32GroupInputC * 2) % u32InKSizeNorm != 0) {
		printf("k size norm is incorrect in feature_to_weight reshape dw2gc group partition method\n");
		return 1;//exit(1);
	}

	AR_U32 bytes_per_cbuf = (u32SocVersion == 1)? BYTES_PER_CBUF: BYTES_PER_CBUF/2;
	AR_U32 bytes_half_cbuf = (u32SocVersion == 1)? BYTES_HALF_CBUF: BYTES_HALF_CBUF/2;

	AR_S8* pInput = (AR_S8*)pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_S8* pOutput = (AR_S8*)pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32OutMemBytesToTal = opParam->u32CdmaKernelBytesToal;
	AR_U32 u32GroupPartNum = (u32GroupNum + 1) / 2;
	memset((AR_S8*)pOutput, 0, u32OutMemBytesToTal * u32GroupPartNum);

	AR_U32 one_kernel_step = bytes_per_cbuf * u32InHeight * u32InWidth;
	AR_U32 cmac_addr_step = u32OutMemBytesToTal;

	AR_U32 k_num_per_group = (u32GroupInputC * 2) / u32InKSizeNorm;
	for (AR_U32 idx_p = 0; idx_p < u32GroupPartNum; ++idx_p)
	{
		AR_S8* pInputAddr = pInput + idx_p * k_num_per_group * u32InKStep;
		AR_S8* pOutputAddr = pOutput + idx_p * u32OutMemBytesToTal;
		for (AR_U32 idx_n = 0; idx_n < k_num_per_group; ++idx_n)
		{
			AR_S8* pInputAddrK = pInputAddr + idx_n * u32InKStep;
			for (AR_U32 idx_h = 0; idx_h < u32InHeight; ++idx_h)
			{
				AR_S8* pInputAddrRow = pInputAddrK + idx_h * u32InRowStep;
				for (AR_U32 idx_w = 0; idx_w < u32InWidth; ++idx_w)
				{
					AR_U32 kernel_map_offset = (idx_h * u32InWidth + idx_w) * bytes_per_cbuf;
					for (AR_U32 idx_c = 0; idx_c < u32InKSizeNorm; ++idx_c)
					{
						AR_U32 kernel_c_idx = idx_n * u32InKSizeNorm + idx_c;
						AR_U32 addr_offset = kernel_c_idx * one_kernel_step;
						if (addr_offset >= cmac_addr_step)
							addr_offset = addr_offset - cmac_addr_step + bytes_half_cbuf;
						addr_offset += (kernel_c_idx % u32GroupInputC) * u32BytesPerPixel + kernel_map_offset;
						memcpy(pOutputAddr + addr_offset, pInputAddrRow, u32BytesPerPixel);
						pInputAddrRow += u32BytesPerPixel;
					}
				}
			}
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorFeatureToWeight_Dw2GC_WtPartition_DumpWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_U32 u32InKSizeNorm	= pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InHeight		= pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth		= pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InKStep		= pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InRowStep		= pCBParams->astInputTensor[0].u32RowStep;

	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* opParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);

	AR_U32 u32GroupNum			= opParam->u32ConvGroupNum;
	AR_U32 u32GroupInputC		= opParam->u32ConvGroupInputChannel;
	AR_U32 u32OutMemBytesToTal	= opParam->u32CdmaKernelBytesToal;
	AR_U32 u32BytesPerPixel		= opParam->u32Precision / 8;

	if (opParam->u32ConvFtFoldingNum != 1) {
		printf("ft folding num: ");
		printf("%u", opParam->u32ConvFtFoldingNum);
		printf("\nunsupported folding num not equal to 1 in feature_to_weight reshape dw2gc wt partition method\n");
		return 1;//exit(1);
	}
	if (u32GroupInputC % u32InKSizeNorm != 0) {
		printf("k size norm is incorrect in feature_to_weight reshape dw2gc wt partition method\n");
		return 1;//exit(1);
	}

	AR_U32 bytes_per_cbuf = (u32SocVersion == 1)? BYTES_PER_CBUF: BYTES_PER_CBUF/2;
	AR_U32 bytes_half_cbuf = (u32SocVersion == 1)? BYTES_HALF_CBUF: BYTES_HALF_CBUF/2;

	AR_S8* pInput = (AR_S8*)pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_S8* pOutput = (AR_S8*)pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	memset((AR_S8*)pOutput, 0, u32OutMemBytesToTal * u32GroupNum);

	AR_U32 half_cmac_k = opParam->u32ConvCmacKernelKNorm / 2;
	AR_U32 one_kernel_step = bytes_per_cbuf * u32InHeight * u32InWidth;
	AR_U32 cmac_addr_step = one_kernel_step * half_cmac_k;

	AR_U32 k_num_per_group = u32GroupInputC / u32InKSizeNorm;
	for (AR_U32 idx_g = 0; idx_g < u32GroupNum; ++idx_g)
	{
		AR_S8* pInputAddr = pInput + idx_g * k_num_per_group * u32InKStep;
		AR_S8* pOutputAddr = pOutput + idx_g * u32OutMemBytesToTal;
		for (AR_U32 idx_n = 0; idx_n < k_num_per_group; ++idx_n)
		{
			//u32InKStep: (cw --> u32InRowStep) * h
			AR_S8* pInputAddrK = pInputAddr + idx_n * u32InKStep;
			for (AR_U32 idx_h = 0; idx_h < u32InHeight; ++idx_h)
			{
				AR_S8* pInputAddrRow = pInputAddrK + idx_h * u32InRowStep;
				for (AR_U32 idx_w = 0; idx_w < u32InWidth; ++idx_w)
				{
					AR_U32 kernel_map_offset = (idx_h * u32InWidth + idx_w) * bytes_per_cbuf;
					for (AR_U32 idx_c = 0; idx_c < u32InKSizeNorm; ++idx_c)
					{
						AR_U32 kernel_c_idx = idx_n * u32InKSizeNorm + idx_c;
						AR_U32 addr_offset = kernel_c_idx * one_kernel_step;
						if ((addr_offset / cmac_addr_step) % 2 == 1)
							addr_offset = addr_offset - cmac_addr_step + bytes_half_cbuf;
						addr_offset += kernel_c_idx * u32BytesPerPixel + kernel_map_offset;
						memcpy(pOutputAddr + addr_offset, pInputAddrRow, u32BytesPerPixel);
						pInputAddrRow += u32BytesPerPixel;
					}
				}
			}
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorFeatureToWeight_Dw2GC_NoPartition_DumpWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_U32 u32InKSizeNorm	= pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InHeight		= pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth		= pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InKStep		= pCBParams->astInputTensor[0].u32KStep;
	AR_U32 u32InRowStep		= pCBParams->astInputTensor[0].u32RowStep;

	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* opParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);

	AR_U32 u32GroupNum			= opParam->u32ConvGroupNum;
	AR_U32 u32GroupInputC		= opParam->u32ConvGroupInputChannel;
	AR_U32 u32OutMemBytesToTal	= opParam->u32CdmaKernelBytesToal;
	AR_U32 u32BytesPerPixel		= opParam->u32Precision / 8;

	if (opParam->u32ConvFtFoldingNum != 1) {
		printf("ft folding num: ");
		printf("%u", opParam->u32ConvFtFoldingNum);
		printf("\nunsupported folding num not equal to 1 in feature_to_weight reshape dw2gc no partition method\n");
		return 1;//exit(1);
	}
	if (u32GroupInputC % u32InKSizeNorm != 0) {
		printf("k size norm is incorrect in feature_to_weight reshape dw2gc no partition method\n");
		return 1;//exit(1);
	}

	AR_U32 bytes_per_cbuf = (u32SocVersion == 1)? BYTES_PER_CBUF: BYTES_PER_CBUF/2;
	AR_U32 bytes_half_cbuf = (u32SocVersion == 1)? BYTES_HALF_CBUF: BYTES_HALF_CBUF/2;

	AR_S8* pInput = (AR_S8*)pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_S8* pOutput = (AR_S8*)pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	memset((AR_S8*)pOutput, 0, u32OutMemBytesToTal * u32GroupNum);

	AR_U32 one_kernel_step = bytes_per_cbuf * u32InHeight * u32InWidth;
	AR_U32 k_num_per_group = u32GroupInputC / u32InKSizeNorm;

	for (AR_U32 idx_g = 0; idx_g < u32GroupNum; ++idx_g)
	{
		AR_S8* pInputAddr = pInput + idx_g * k_num_per_group * u32InKStep;
		AR_S8* pOutputAddr = pOutput + idx_g * u32OutMemBytesToTal;
		for (AR_U32 idx_n = 0; idx_n < k_num_per_group; ++idx_n)
		{
			AR_S8* pInputAddrK = pInputAddr + idx_n * u32InKStep;
			for (AR_U32 idx_h = 0; idx_h < u32InHeight; ++idx_h)
			{
				AR_S8* pInputAddrRow = pInputAddrK + idx_h * u32InRowStep;
				for (AR_U32 idx_w = 0; idx_w < u32InWidth; ++idx_w)
				{
					AR_U32 kernel_map_offset = (idx_h * u32InWidth + idx_w) * bytes_per_cbuf;
					for (AR_U32 idx_c = 0; idx_c < u32InKSizeNorm; ++idx_c)
					{
						AR_U32 kernel_c_idx = idx_n * u32InKSizeNorm + idx_c;
						AR_U32 addr_offset = kernel_c_idx * one_kernel_step;
						addr_offset += kernel_c_idx * u32BytesPerPixel + kernel_map_offset;
						memcpy(pOutputAddr + addr_offset, pInputAddrRow, u32BytesPerPixel);
						pInputAddrRow += u32BytesPerPixel;
					}
				}
			}
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorFeatureToWeight_Dw2GCNhwcDumpWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);
	if (pFeatureToWeightParam->bConvGroupPartitionEn)
		AR_CNN_OperatorFeatureToWeight_Dw2GC_GroupPartition_DumpWeight(pIn, pOut, pCBParams, pOpParams);
	else if (pFeatureToWeightParam->bConvWtPartitionEn)
		AR_CNN_OperatorFeatureToWeight_Dw2GC_WtPartition_DumpWeight(pIn, pOut, pCBParams, pOpParams);
	else
		AR_CNN_OperatorFeatureToWeight_Dw2GC_NoPartition_DumpWeight(pIn, pOut, pCBParams, pOpParams);

	return 0;
}
/*****************************************************************************************************/

AR_S32 AR_CNN_OperatorFeatureToWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams)
{
	//printOpParamsInfo(pOpParams);
	//if (1 == AR_CNN_OperatorFeatureToWeight_ParamsCheck(pCBParams, pOpParams))
	//	return 0;

	u32SocVersion = AR_MPI_NPU_GetSocVersion();
	//printf("u32SocVersion %d \r\n",u32SocVersion);
	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)(pOpParams);

	//judge ftw method: reshape or transpose
	if (strcmp(pFeatureToWeightParam->achFtwMethod, "ftw_reshape") == 0)
		AR_CNN_OperatorFeatureToWeight_SelfAttentionNchwDumpWeight(pIn, pOut, pCBParams, pOpParams);
	else if (strcmp(pFeatureToWeightParam->achFtwMethod, "ftw_transpose") == 0)
		AR_CNN_OperatorFeatureToWeight_SelfAttentionNhwcDumpWeight(pIn, pOut, pCBParams, pOpParams);
	else if (strcmp(pFeatureToWeightParam->achFtwMethod, "ftw_reshape_dw2gc") == 0)
		AR_CNN_OperatorFeatureToWeight_Dw2GCNhwcDumpWeight(pIn, pOut, pCBParams, pOpParams);
	else
		printf("unsupported feature_to_weight method\n");

	return 0;
}
