#include "ar_cnn_parser.h"


AR_U32 u32ConcatVer =0; //0: v1caffe 1:v2onnx
AR_S32 AR_CNN_ParseConcat(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)

{
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_CONCAT_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_CONCAT_PARAM_S);
	*pOpParams = (AR_NPU_CONCAT_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_CONCAT_PARAM_S *pConcatParam = (AR_NPU_CONCAT_PARAM_S*) *pOpParams;

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
		JSON_PARSE_INT_VALUE(pConcatParam, u32Axis, pAttributeNode, "axis");
		u32ConcatVer = 1;
		//printf("attribute axis %d \r\n",pConcatParam->u32Axis);
	}
	else
	{
		JSON_PARSE_INT_VALUE(pConcatParam, u32Axis, pOperatorParamNode, "axis");
		u32ConcatVer = 0;
		//printf("noattribute axis %d \r\n",pConcatParam->u32Axis);
	}

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


