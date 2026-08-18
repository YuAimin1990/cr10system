#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParseUpsample(const void *pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
    printf("AR_CNN_ParseUpsample\r\n");
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_UPSAMPLE_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_UPSAMPLE_PARAM_S);
	*pOpParams = (AR_NPU_UPSAMPLE_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_UPSAMPLE_PARAM_S *pUpsampleParam = (AR_NPU_UPSAMPLE_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)pJsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

	JSON_PARSE_INT_VALUE(pUpsampleParam, u32CoordTransMode, pOperatorParamNode, "coordinate_transformation_mode");
	JSON_PARSE_STR_VALUE(pUpsampleParam, achMode, pOperatorParamNode, "mode");
	JSON_PARSE_INT_VALUE(pUpsampleParam, u32NearestMode, pOperatorParamNode, "nearest_mode");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


AR_S32 AR_CNN_ParseResize(const void *pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
    printf("AR_CNN_ParseResize\r\n");
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_RESIZE_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_RESIZE_PARAM_S);
	*pOpParams = (AR_NPU_RESIZE_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_RESIZE_PARAM_S *pUpsampleParam = (AR_NPU_RESIZE_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)pJsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

    
	cJSON * pAttributeNode = NULL;
	pAttributeNode = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "attributes");
	if(pAttributeNode)
	{
	    printf("OnnxResize\r\n");
		JSON_PARSE_STR_VALUE(pUpsampleParam, achCoordTransMode, pAttributeNode, "coordinate_transformation_mode");
		JSON_PARSE_STR_VALUE(pUpsampleParam, achMode, pAttributeNode, "mode");
		JSON_PARSE_STR_VALUE(pUpsampleParam, achNearestMode, pAttributeNode, "nearest_mode");
	}
	//cJSON_Delete(pOperatorParamNode);
	return 0;
}



