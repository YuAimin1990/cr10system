#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParseTranspose(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)

{
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_TRANSPOSE_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_TRANSPOSE_PARAM_S);
	*pOpParams = (AR_NPU_TRANSPOSE_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_TRANSPOSE_PARAM_S *pTransposeParam = (AR_NPU_TRANSPOSE_PARAM_S*) *pOpParams;

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
		JSON_PARSE_INT_ARRAY_VALUE(pTransposeParam, u32Perm, pAttributeNode, "perm");
		JSON_PARSE_INT_VALUE(pTransposeParam, u32Perm_size, pAttributeNode, "perm_size");
	}
	else
	{
		JSON_PARSE_INT_ARRAY_VALUE(pTransposeParam, u32Perm, pOperatorParamNode, "perm");
		JSON_PARSE_INT_VALUE(pTransposeParam, u32Perm_size, pOperatorParamNode, "perm_size");
	}


	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


