#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParsePermute(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)

{
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_PERMUTE_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_PERMUTE_PARAM_S);
	*pOpParams = (AR_NPU_PERMUTE_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_PERMUTE_PARAM_S *pPermuteParam = (AR_NPU_PERMUTE_PARAM_S*) *pOpParams;

	cJSON *pOperatorParamNode = (cJSON *)pJsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

    JSON_PARSE_STR_VALUE(pPermuteParam, sOrder_type, pOperatorParamNode, "order_type");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


