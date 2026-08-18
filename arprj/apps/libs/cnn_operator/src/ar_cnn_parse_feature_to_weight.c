#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParseFeatureToWeight(const void* pJsonNode, AR_U32 u32JsonLen, void** pOpParams, AR_U32* pOpParamLen)
{
	printf("AR_CNN_ParseFeatureToWeight\r\n");
	if ((!pJsonNode) || (u32JsonLen == 0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if (pOpParamLen != NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_FEATURE_TO_WEIGHT_PARAM_S);
	}

	AR_U32 u32Size = sizeof(AR_NPU_FEATURE_TO_WEIGHT_PARAM_S);
	*pOpParams = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)malloc(u32Size);
	if (*pOpParams == NULL)
		return -1;
	memset(*pOpParams, 0, u32Size);
	AR_NPU_FEATURE_TO_WEIGHT_PARAM_S* pFeatureToWeightParam = (AR_NPU_FEATURE_TO_WEIGHT_PARAM_S*)*pOpParams;

	cJSON* pOperatorParamNode = (cJSON*)pJsonNode;
	cJSON * pAttributeNode = NULL;
	pAttributeNode = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "attributes");
	if(pAttributeNode)
	{
	//data type is uint32
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelBytesToal, pAttributeNode, "cdma_kernel_bytes_total");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelKBytesNorm, pAttributeNode, "cdma_kernel_k_bytes_norm");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelKNormNum, pAttributeNode, "cdma_kernel_k_norm_num");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelKSizeLast, pAttributeNode, "cdma_kernel_k_size_last");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKerneLKSizeNorm, pAttributeNode, "cdma_kernel_k_size_norm");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32Precision, pAttributeNode, "precision");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvCmacKernelKNorm, pAttributeNode, "conv_cmac_kernel_k_norm");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvFoldingBit, pAttributeNode, "conv_folding_bit");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvFtFoldingNum, pAttributeNode, "conv_ft_folding_num");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvGroupInputChannel, pAttributeNode, "conv_group_input_channel");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvGroupNum, pAttributeNode, "conv_group_num");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvGroupOuutputChannel, pAttributeNode, "conv_group_output_channel");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvKernelH, pAttributeNode, "conv_kernel_h");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvKernelW, pAttributeNode, "conv_kernel_w");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvKernelSplitNum, pAttributeNode, "conv_kernel_split_num");
	
		//data type is bool
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvGroupPartitionEn, pAttributeNode, "conv_group_partition_en");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvWtPartitionEn, pAttributeNode, "conv_wt_partition_en");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvDeconvEn, pAttributeNode, "conv_deconv_en");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvDeconvOptimizeEn, pAttributeNode, "conv_deconv_optimize_en");
		
		//dataa type is char*
		JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achFtwMethod, pAttributeNode, "ftw_method");
		JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achConvMode, pAttributeNode, "conv_mode");
		//JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achOperatorChannelMode, pOperatorParamNode, "operator_channel_mode");
		//JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achOperatorType, pOperatorParamNode, "operator_type");
		//printf("attribute f2w \r\n");
	}
	else
	{	
		//data type is uint32
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelBytesToal, pOperatorParamNode, "cdma_kernel_bytes_total");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelKBytesNorm, pOperatorParamNode, "cdma_kernel_k_bytes_norm");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelKNormNum, pOperatorParamNode, "cdma_kernel_k_norm_num");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKernelKSizeLast, pOperatorParamNode, "cdma_kernel_k_size_last");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32CdmaKerneLKSizeNorm, pOperatorParamNode, "cdma_kernel_k_size_norm");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32Precision, pOperatorParamNode, "precision");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvCmacKernelKNorm, pOperatorParamNode, "conv_cmac_kernel_k_norm");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvFoldingBit, pOperatorParamNode, "conv_folding_bit");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvFtFoldingNum, pOperatorParamNode, "conv_ft_folding_num");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvGroupInputChannel, pOperatorParamNode, "conv_group_input_channel");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvGroupNum, pOperatorParamNode, "conv_group_num");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvGroupOuutputChannel, pOperatorParamNode, "conv_group_output_channel");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvKernelH, pOperatorParamNode, "conv_kernel_h");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvKernelW, pOperatorParamNode, "conv_kernel_w");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, u32ConvKernelSplitNum, pOperatorParamNode, "conv_kernel_split_num");
	
		//data type is bool
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvGroupPartitionEn, pOperatorParamNode, "conv_group_partition_en");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvWtPartitionEn, pOperatorParamNode, "conv_wt_partition_en");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvDeconvEn, pOperatorParamNode, "conv_deconv_en");
		JSON_PARSE_INT_VALUE(pFeatureToWeightParam, bConvDeconvOptimizeEn, pOperatorParamNode, "conv_deconv_optimize_en");
		
		//dataa type is char*
		JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achFtwMethod, pOperatorParamNode, "ftw_method");
		JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achConvMode, pOperatorParamNode, "conv_mode");
		//JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achOperatorChannelMode, pOperatorParamNode, "operator_channel_mode");
		//JSON_PARSE_STR_VALUE(pFeatureToWeightParam, achOperatorType, pOperatorParamNode, "operator_type");
		//printf("no attribute f2w \r\n");
		if (!pFeatureToWeightParam->u32CdmaKernelKBytesNorm){
			cJSON * pCdmaKernelKBytesVecNode = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "cdma_kernel_k_bytes_vec");
			AR_U32 u32CdmaKernelKBytesVecArraySize = cJSON_GetArraySize(pCdmaKernelKBytesVecNode);
			pFeatureToWeightParam->u32CdmaKernelKBytesVec = (AR_U32 *)malloc(u32CdmaKernelKBytesVecArraySize * sizeof(AR_U32));

			JSON_PARSE_INT_ARRAY_VALUE(pFeatureToWeightParam, u32CdmaKernelKBytesVec, pOperatorParamNode, "cdma_kernel_k_bytes_vec");
			if(u32CdmaKernelKBytesVecArraySize == 1)
				pFeatureToWeightParam->u32CdmaKernelKBytesNorm = pFeatureToWeightParam->u32CdmaKernelKBytesVec[0];
			else if(u32CdmaKernelKBytesVecArraySize == 0){
				printf("FeatureToWeight callback parse param error!\n");
				return -1;
			}
			else{
				AR_U32 firstBytes = pFeatureToWeightParam->u32CdmaKernelKBytesVec[0];
				for(AR_U32 x = 0; x < u32CdmaKernelKBytesVecArraySize - 1; x++){
					if (firstBytes != pFeatureToWeightParam->u32CdmaKernelKBytesVec[x]){
						printf("Err: CdmaKernelKBytesVec has more than two differences and is not supported for the time being.\n");
						return -1;
					}
				}
				pFeatureToWeightParam->u32CdmaKernelKBytesNorm = firstBytes;
			}
			//for(int x = 0; x < u32CdmaKernelKBytesVecArraySize; x++)
			//	printf("************** u32CdmaKernelKBytesVec[%d] = %d **************\n", x, pFeatureToWeightParam->u32CdmaKernelKBytesVec[x]);
			free(pFeatureToWeightParam->u32CdmaKernelKBytesVec);
		}
		if (!pFeatureToWeightParam->u32CdmaKernelKSizeVec){
			cJSON * pCdmaKernelKSizeVecNode = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "cdma_kernel_k_size_vec");
			AR_U32 u32CdmaKernelKSizeVecArraySize = cJSON_GetArraySize(pCdmaKernelKSizeVecNode);
			pFeatureToWeightParam->u32CdmaKernelKSizeVec = (AR_U32 *)malloc(u32CdmaKernelKSizeVecArraySize * sizeof(AR_U32));

			JSON_PARSE_INT_ARRAY_VALUE(pFeatureToWeightParam, u32CdmaKernelKSizeVec, pOperatorParamNode, "cdma_kernel_k_size_vec");
			if(u32CdmaKernelKSizeVecArraySize == 1){
				pFeatureToWeightParam->u32CdmaKerneLKSizeNorm = pFeatureToWeightParam->u32CdmaKernelKSizeVec[0];
				pFeatureToWeightParam->u32CdmaKernelKSizeLast = 0;
				pFeatureToWeightParam->u32CdmaKernelKNormNum = 1;
			}
			else if(u32CdmaKernelKSizeVecArraySize == 0){
				printf("FeatureToWeight callback parse param error!\n");
				return -1;
			}
			else{
				AR_U32 firstSize = pFeatureToWeightParam->u32CdmaKernelKSizeVec[0];
				for(AR_U32 x = 0; x < u32CdmaKernelKSizeVecArraySize - 1; x++){
					if (firstSize != pFeatureToWeightParam->u32CdmaKernelKSizeVec[x]){
						printf("Err: CdmaKernelKSizeVec has more than two differences and is not supported for the time being.\n");
						return -1;
					}
				}
				pFeatureToWeightParam->u32CdmaKerneLKSizeNorm = firstSize;
				pFeatureToWeightParam->u32CdmaKernelKNormNum = u32CdmaKernelKSizeVecArraySize - 1;
				pFeatureToWeightParam->u32CdmaKernelKSizeLast = pFeatureToWeightParam->u32CdmaKernelKSizeVec[u32CdmaKernelKSizeVecArraySize-1];
				if(firstSize == pFeatureToWeightParam->u32CdmaKernelKSizeLast){
					pFeatureToWeightParam->u32CdmaKernelKNormNum = u32CdmaKernelKSizeVecArraySize;
					pFeatureToWeightParam->u32CdmaKernelKSizeLast = 0;
				}
			}

			//for(int x = 0; x < u32CdmaKernelKSizeVecArraySize; x++)
			//	printf("============== u32CdmaKernelKSizeVec[%d] = %d ============\n", x, pFeatureToWeightParam->u32CdmaKernelKSizeVec[x]);
			free(pFeatureToWeightParam->u32CdmaKernelKSizeVec);
		}
	}
	
	//cJSON_Delete(pOperatorParamNode);
	return 0;
}
