#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParsePad(const void *pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_PAD_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_PAD_PARAM_S);
	*pOpParams = (AR_NPU_PAD_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_PAD_PARAM_S *pPadParam = (AR_NPU_PAD_PARAM_S*) *pOpParams;

	cJSON *pOperatorParamNode = (cJSON *)pJsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

	cJSON *cbVersionNode = cJSON_GetObjectItemCaseSensitive(pOperatorParamNode, "callback_version");
	
	if((int)cbVersionNode->valuedouble == 1){
		JSON_PARSE_INT_VALUE(pPadParam, pad_t, pOperatorParamNode, "pad_t");
		JSON_PARSE_INT_VALUE(pPadParam, pad_r, pOperatorParamNode, "pad_r");
		JSON_PARSE_INT_VALUE(pPadParam, pad_l, pOperatorParamNode, "pad_l");
		JSON_PARSE_INT_VALUE(pPadParam, pad_b, pOperatorParamNode, "pad_b");
		JSON_PARSE_STR_VALUE(pPadParam, achPadMode, pOperatorParamNode, "type");
		if(strcmp(pPadParam->achPadMode, "constant") == 0)
			JSON_PARSE_DOUBLE_VALUE(pPadParam, val, pOperatorParamNode, "val");
		pPadParam->u32PadOpVer = 0; 
	}
	else if((int)cbVersionNode->valuedouble > 1 && (int)cbVersionNode->valuedouble < 200){
		cJSON * pAttributeNode = NULL;
		pAttributeNode = cJSON_GetObjectItem(pOperatorParamNode, "attributes");
		if(pAttributeNode == NULL || cJSON_IsNull(pAttributeNode))
			strcpy(pPadParam->achPadMode, "constant");
		else
			JSON_PARSE_STR_VALUE(pPadParam, achPadMode, pAttributeNode, "mode");
		cJSON *pConstInputsNode = cJSON_GetObjectItem(pOperatorParamNode, "const_inputs");
		pPadParam->u32PadOpVer = 1;
		cJSON *pSubItemTensorNode = cJSON_GetArrayItem(pConstInputsNode, 0);
		JSON_PARSE_INT_ARRAY_VALUE(pPadParam, u32PadTensor, pSubItemTensorNode, "weight_data");
		pPadParam->pad_t = pPadParam->u32PadTensor[2];
		pPadParam->pad_l = pPadParam->u32PadTensor[3];
		pPadParam->pad_b = pPadParam->u32PadTensor[6];
		pPadParam->pad_r = pPadParam->u32PadTensor[7];
		if(pPadParam->u32PadTensor[0] || pPadParam->u32PadTensor[1] || pPadParam->u32PadTensor[4] || pPadParam->u32PadTensor[5]){
			printf("SDK does not support Pad Callback calculation in channel or batch direction!\n");
			return -1;
		}
		if(strcmp(pPadParam->achPadMode, "constant") == 0){
			pSubItemTensorNode = cJSON_GetArrayItem(pConstInputsNode, 1);
			if(pSubItemTensorNode != NULL){
				cJSON *pConstValNode = cJSON_GetObjectItem(pSubItemTensorNode, "weight_data");
				AR_U32 u32ConstValNum = cJSON_GetArraySize(pConstValNode);
				if(u32ConstValNum > 1){
					printf("SDK only supports that the pad's constant is the same value!\n");
					return -1;
				}
				cJSON *pValueNode = cJSON_GetArrayItem(pConstValNode, 0);
				pPadParam->val = pValueNode->valuedouble;
			}
			else{
				pPadParam->val = 0;
			}
		}
	}
	else{
		printf("When the callback_version >= 200, please use onnxruntime method to execute the Pad callback operator!\n");
		return -1;
	}
	//cJSON_Delete(pOperatorParamNode);
	return 0;
}



