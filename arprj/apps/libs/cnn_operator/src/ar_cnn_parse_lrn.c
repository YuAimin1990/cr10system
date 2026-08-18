#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParseLrn(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
    printf("AR_CNN_ParseLrn\r\n");
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_LRN_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_LRN_PARAM_S);
	*pOpParams = (AR_NPU_LRN_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_LRN_PARAM_S *pLrnParam = (AR_NPU_LRN_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)pJsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dAlphaDivSize, pOperatorParamNode, "alpha_div_size");
    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dBeta, pOperatorParamNode, "beta");
    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dK, pOperatorParamNode, "k");
    JSON_PARSE_INT_VALUE(pLrnParam, u32LocalSize, pOperatorParamNode, "local_size");
    JSON_PARSE_STR_VALUE(pLrnParam, achNormRegion, pOperatorParamNode, "norm_region");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}



AR_S32 AR_CNN_ParseOnnxLrn(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
    printf("AR_CNN_ParseOnnxLrn\r\n");
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_ONNX_LRN_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_ONNX_LRN_PARAM_S);
	*pOpParams = (AR_NPU_ONNX_LRN_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_ONNX_LRN_PARAM_S *pLrnParam = (AR_NPU_ONNX_LRN_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)pJsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

	cJSON * pcbv = NULL;
	pcbv = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "callback_version");
	if(pcbv)
	{
		printf("cb version is %d \r\n",(int)(pcbv->valuedouble));
	}

	cJSON * pAttributeNode = NULL;
	pAttributeNode = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "attributes");
	if(pAttributeNode)
	{
	    printf("onnxlrn\r\n");
	    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dAlpha, pAttributeNode, "alpha");
	    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dBeta, pAttributeNode, "beta");
	    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dBias, pAttributeNode, "bias");
	    JSON_PARSE_INT_VALUE(pLrnParam, u32LocalSize, pAttributeNode, "size");
	}
	//cJSON_Delete(pOperatorParamNode);
	return 0;
}



