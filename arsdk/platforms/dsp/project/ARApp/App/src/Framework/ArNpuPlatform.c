#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Uart.h"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "ArTasksSchedule.h"
#include "ArNpuPlatform.h"
#include "ArCnnParseDeclare.h"

//#include "ArCevaArtSimulator.h"

extern QueueHandle_t NPUIntxQueue;
extern NpuSignal_t NPUAlgSignal;
uint64_t ulNpuIrqCnt = 0;

uint64_t ulGetArNpuIrqCnt(void)
{
	return ulNpuIrqCnt;
}


int32_t xArNetParamListInit(AR_LIST_t* list_head)
{
	if(!list_head)
		return AR_FAILURE;
	vArInitListHead(list_head);
	return AR_SUCCESS;
}

int32_t xArNetParamListAdd(AR_LIST_t* list_head, AR_NPU_LIST_ITEMS_st* xNetParam)
{
    AR_NETPARAM_LIST_st* new_node = NULL;
    if(!list_head || !xNetParam)
        return AR_FAILURE;
    new_node =malloc(sizeof(AR_NETPARAM_LIST_st));
    if(!new_node)
        return AR_FAILURE;
    memset(new_node, 0x0, sizeof(AR_NETPARAM_LIST_st));
    new_node->xNetParam = xNetParam;
    vArListAddTail(&new_node->list, list_head);
    return AR_SUCCESS;
}


void vArNetParamListRemoveByNetParams(AR_LIST_t * list_head, AR_U32 u32NetworkId,AR_U32 au32InputAddr,AR_U32 au32OutputAddr)
{
    AR_NETPARAM_LIST_st* node = NULL;
    AR_LIST_t *pos_meta;
    if(!list_head )
        return;
	ar_list_for_each(pos_meta, list_head) {
		node = ar_list_entry(pos_meta, AR_NETPARAM_LIST_st, list);
        if(node && node->xNetParam && (node->xNetParam->u32NetworkId== u32NetworkId)) {
			//if((node->xNetParam->au32InputAddr== au32InputAddr) && (node->xNetParam->au32OutputAddr== au32OutputAddr))
			{
	            vArListDel(&node->list);
				for(int i=0;i<MAX_CB_NUM;i++)
				{
					if(node->xNetParam->astCBParam[i].pOpParams)
					{
						free(node->xNetParam->astCBParam[i].pOpParams);
					}
				}
	            free(node->xNetParam);
	            free(node);
	            break;
			}
        }
	}
    return;
}


AR_NETPARAM_LIST_st* pxArNetParamListGetByNetWorkId(AR_LIST_t * list_head, AR_U32 u32NetworkId)
{
    AR_NETPARAM_LIST_st* node = NULL;
    AR_LIST_t *pos_meta;
    if(!list_head)
        return NULL;
	ar_list_for_each(pos_meta, list_head) {
		node = ar_list_entry(pos_meta, AR_NETPARAM_LIST_st, list);
        if(node && node->xNetParam && (node->xNetParam->u32NetworkId==u32NetworkId)) {
            return node;
        }
	}
    return NULL;
}


int32_t xArNetParamListIsExistByNetWorkId(AR_LIST_t * list_head, AR_U32 u32NetworkId)
{
    AR_NETPARAM_LIST_st* node = NULL;
    AR_LIST_t *pos_meta;
    if(!list_head)
        return 0;
	ar_list_for_each(pos_meta, list_head) {
		node = ar_list_entry(pos_meta, AR_NETPARAM_LIST_st, list);
        if(node && node->xNetParam && (node->xNetParam->u32NetworkId==u32NetworkId))
        {
            return 1;
            break;
        }
		return 0;
	}
    return 0;
}

void vArNetParamListRemoveAll(AR_LIST_t* list_head)
{
    AR_NETPARAM_LIST_st* node = NULL, *t_tmp = NULL;
    if(!list_head)
        return;
    ar_list_for_each_entry_safe(node, t_tmp, list_head, list) {
        if(node && node->xNetParam) {
            vArListDel(&node->list);
			for(int i=0;i<MAX_CB_NUM;i++)
			{
				if(node->xNetParam->astCBParam[i].pOpParams)
				{
					free(node->xNetParam->astCBParam[i].pOpParams);
				}
			}
            free(node->xNetParam);
            free(node);
        }
	}
    return;
}

ArCnnParseDeclare_t * xArCnnParseLookUpName(char* pName)
{
	ArCnnParseDeclare_t *entry = NULL;
	ArCnnParseDeclare_t *start = xArCnnParseEntryStart(ArCnnParseDeclare_t);
	const int32_t n_entry = xArCnnParseEntryCount(ArCnnParseDeclare_t);
	for (entry = start; entry != start + n_entry; entry++)
	{
		if (!strcmp(pName, entry->pName))
			return entry;
	}
	xDebugPrint("ParseHandler %s not register!\r\n",pName);

	return NULL;
}

AR_S32 xArNpuParseCBFile(AR_CHAR * pu8FilePhyAddr, AR_NPU_CB_PARAM_S * pstCB, AR_U32 * pu32CBNum)
{
    AR_CHAR * pchBuff = pu8FilePhyAddr;
    AR_U32 i = 0;
    AR_S32 ret =0;

    cJSON * pJson = NULL;
    cJSON * pNum = NULL;
    cJSON * pCB = NULL;

    pJson = cJSON_Parse(pchBuff);
    if(!pJson)
    {
        xDebugPrint("Parse cb json file failed.\n");
        return -1;
    }

    pNum = cJSON_GetObjectItemCaseSensitive(pJson, "callback_num");
    if(!pNum || pNum->valueint == 0)
    {
        return 0;
    }

    *pu32CBNum = pNum->valueint;

    for(i = 0; i < pNum->valueint; i++)
    {
        char tmp[64] = {0};
        sprintf(tmp, "callback_%d", i);

        pCB = cJSON_GetObjectItemCaseSensitive(pJson, tmp);
        if(!pCB)
        {
            xDebugPrint("Parse cb node failed.\n");
            return -1;
        }

        JSON_PARSE_INT_VALUE(pstCB, u32Id, pCB, "callback_id");
        JSON_PARSE_INT_VALUE(pstCB, u32InputTensorNum, pCB, "input_tensor_number");
        JSON_PARSE_INT_VALUE(pstCB, u32OutputTensorNum, pCB, "output_tensor_number");
        JSON_PARSE_STR_VALUE(pstCB, achOperatorName, pCB, "operator_name");
        JSON_PARSE_STR_VALUE(pstCB, achOperatorType, pCB, "operator_type");

        //0.Operator Parameters Parse
        cJSON * pOperatorParamNode = NULL;
        pOperatorParamNode = cJSON_GetObjectItemCaseSensitive(pCB, "operator_param");
        if(pOperatorParamNode)
        {
		     ArCnnParseDeclare_t* entry=NULL;
		     entry=xArCnnParseLookUpName(pstCB->achOperatorType);
		     if(entry)
			 {
				 ret=entry->pCnnParseHandler((void*)pOperatorParamNode, 1, &pstCB->pOpParams, &pstCB->u32OpParamsLen);
				 if(ret<0)
				 {
					   xDebugPrint("pCnnParseHandler failed!\r\n");
				 }
			 }
        }

		//1.Tensors Parameters Parse
        cJSON * pTensorNode = NULL;
        char tensor[64] = {0};
        for(int j = 0; j < pstCB->u32InputTensorNum; j++)
        {
            sprintf(tensor, "input_tensor_%d", j);

            pTensorNode = cJSON_GetObjectItemCaseSensitive(pCB, tensor);
            if(!pTensorNode)
            {
                return -1;
            }

            AR_NPU_TENSOR_S * pInputTensor = &pstCB->astInputTensor[j];

            JSON_PARSE_INT_VALUE(pInputTensor, u32ID, pTensorNode, "tensor_id");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Bank, pTensorNode, "tensor_bank");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Offset, pTensorNode, "tensor_bank_offset");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Height, pTensorNode, "tensor_height");
            JSON_PARSE_INT_VALUE(pInputTensor, u32KStep, pTensorNode, "tensor_k_ddr_step");
            JSON_PARSE_INT_VALUE(pInputTensor, u32KNormNum, pTensorNode, "tensor_k_norm_num");
            JSON_PARSE_INT_VALUE(pInputTensor, u32KSizeLast, pTensorNode, "tensor_k_size_last");
            JSON_PARSE_INT_VALUE(pInputTensor, u32KSizeNorm, pTensorNode, "tensor_k_size_norm");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Num, pTensorNode, "tensor_number");
			JSON_PARSE_INT_VALUE(pInputTensor, u32OriChannels, pTensorNode, "tensor_ori_channels");
            JSON_PARSE_INT_VALUE(pInputTensor, u32OriFrameSize, pTensorNode, "tensor_ori_frame_size");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Precision, pTensorNode, "tensor_precision");
            JSON_PARSE_INT_VALUE(pInputTensor, u32RowStep, pTensorNode, "tensor_row_ddr_step");
            JSON_PARSE_DOUBLE_VALUE(pInputTensor, dScaleFactor, pTensorNode, "tensor_scale_factor");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Size, pTensorNode, "tensor_size");
			JSON_PARSE_INT_VALUE(pInputTensor, u32TensorStep, pTensorNode, "tensor_step");
            JSON_PARSE_INT_VALUE(pInputTensor, u32Width, pTensorNode, "tensor_width");
            JSON_PARSE_INT_VALUE(pInputTensor, s32ZeroPoint, pTensorNode, "tensor_zero_point");
            JSON_PARSE_STR_VALUE(pInputTensor, achName, pTensorNode, "tensor_name");
            JSON_PARSE_STR_VALUE(pInputTensor, achType, pTensorNode, "tensor_type");
            JSON_PARSE_STR_VALUE(pInputTensor, achMemoryType, pTensorNode, "tensor_memory_type");
        }

        for(int j = 0; j < pstCB->u32OutputTensorNum; j++)
        {
            sprintf(tensor, "output_tensor_%d", j);

            pTensorNode = cJSON_GetObjectItemCaseSensitive(pCB, tensor);
            if(!pTensorNode)
            {
                return -1;
            }

            AR_NPU_TENSOR_S * pOutputTensor = &pstCB->astOutputTensor[j];

            JSON_PARSE_INT_VALUE(pOutputTensor, u32ID, pTensorNode, "tensor_id");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Bank, pTensorNode, "tensor_bank");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Offset, pTensorNode, "tensor_bank_offset");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Height, pTensorNode, "tensor_height");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32KStep, pTensorNode, "tensor_k_ddr_step");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32KNormNum, pTensorNode, "tensor_k_norm_num");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32KSizeLast, pTensorNode, "tensor_k_size_last");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32KSizeNorm, pTensorNode, "tensor_k_size_norm");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Num, pTensorNode, "tensor_number");
			JSON_PARSE_INT_VALUE(pOutputTensor, u32OriChannels, pTensorNode, "tensor_ori_channels");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32OriFrameSize, pTensorNode, "tensor_ori_frame_size");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Precision, pTensorNode, "tensor_precision");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32RowStep, pTensorNode, "tensor_row_ddr_step");
            JSON_PARSE_DOUBLE_VALUE(pOutputTensor, dScaleFactor, pTensorNode, "tensor_scale_factor");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Size, pTensorNode, "tensor_size");
			JSON_PARSE_INT_VALUE(pOutputTensor, u32TensorStep, pTensorNode, "tensor_step");
            JSON_PARSE_INT_VALUE(pOutputTensor, u32Width, pTensorNode, "tensor_width");
            JSON_PARSE_INT_VALUE(pOutputTensor, s32ZeroPoint, pTensorNode, "tensor_zero_point");
            JSON_PARSE_STR_VALUE(pOutputTensor, achName, pTensorNode, "tensor_name");
			JSON_PARSE_STR_VALUE(pOutputTensor, achType, pTensorNode, "tensor_type");
			JSON_PARSE_STR_VALUE(pOutputTensor, achMemoryType, pTensorNode, "tensor_memory_type");
        }

        pstCB++;
    }

    cJSON_Delete(pJson);
    return 0;
}


void vPortint1NPUHandler()
{

	long woken=0;
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;


	AR_NPU_IRQ_PARAMS_st xNpuIrqParams = {0};
	AR_U32 u32rqStatus= *(unsigned int *)(NPU_IRQ_STATUS_REG);

	xNpuIrqParams.u32NetworkId= *(unsigned int *)(NPU_IRQ_NET_FRAME_ID_REG) & 0x0000FFFF;
    xNpuIrqParams.au32CBAckIDs[0] = xReadReg32(NPU_CALLBACK_ID_0_REG);
    xNpuIrqParams.au32CBAckIDs[1] = xReadReg32(NPU_CALLBACK_ID_1_REG);
    xNpuIrqParams.au32CBAckIDs[2] = xReadReg32(NPU_CALLBACK_ID_2_REG);
    xNpuIrqParams.au32CBAckIDs[3] = xReadReg32(NPU_CALLBACK_ID_3_REG);

	//write back to clear interrupt,  it is not ack
    vWriteReg32(NPU_CALLBACK_ID_0_REG, xNpuIrqParams.au32CBAckIDs[0]);
    vWriteReg32(NPU_CALLBACK_ID_1_REG, xNpuIrqParams.au32CBAckIDs[1]);
    vWriteReg32(NPU_CALLBACK_ID_2_REG, xNpuIrqParams.au32CBAckIDs[2]);
    vWriteReg32(NPU_CALLBACK_ID_3_REG, xNpuIrqParams.au32CBAckIDs[3]);

	//not necessary. because ceva only care about dlacb interrupt, no need to master dla
	/**********************************************************
	//clear irq
	  val = readl(p_dla_dev->base + NPU_IRQ_CLR_REG);
	  val |= (g_dla_status[i].irq_status & 0xFF00); //bit 15:8 is for ceva intr
	  writel(val, p_dla_dev->base + NPU_IRQ_CLR_REG);

	************************************************************/

    if(u32rqStatus & NPU_IRQ_CEVA_CB_BIT)
    {
		xQueueSendFromISR(NPUIntxQueue, &xNpuIrqParams,  &xHigherPriorityTaskWoken);
		xNpuSignalIrq(NPUAlgSignal,&woken);
    }
	ulNpuIrqCnt++;
	//portYIELD_FROM_ISR();

}


