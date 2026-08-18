
#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"

AR_S32 ARCnnParseConcat(const void* JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	xDebugPrint("ARCnnParseConcat\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		printf("JsonNode or u32JsonLen err \r\n");
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

	cJSON *pOperatorParamNode = (cJSON *)JsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

    JSON_PARSE_INT_VALUE(pConcatParam, u32Axis, pOperatorParamNode, "axis");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


AR_CNN_PARSE_DECLARE(ARCnnParseConcat) = {
    .pName = "Concat",
    .pCnnParseHandler = ARCnnParseConcat,
};
