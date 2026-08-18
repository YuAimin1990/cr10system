#ifndef __AR_NPU_PLATFORM_H__
#define __AR_NPU_PLATFORM_H__


#include "ArDspPlatform.h"


//#define NPU_CFG_BASE_ADDR 0x64c00000
#define NPU_CFG_BASE_ADDR 0x01140000

#define NPU_START_REG             (NPU_CFG_BASE_ADDR + 0x00)
#define NPU_COMMAND_ADDR_REG      (NPU_CFG_BASE_ADDR + 0x04)
#define NPU_CMPLT_LAYER_NUM_REG   (NPU_CFG_BASE_ADDR + 0x08)
#define NPU_COMMAND_LEN_REG       (NPU_CFG_BASE_ADDR + 0x0C)
#define NPU_RESTART_ADDR_REG      (NPU_CFG_BASE_ADDR + 0x10)
#define NPU_IRQ_STATUS_REG        (NPU_CFG_BASE_ADDR + 0x14)
#define NPU_IRQ_NET_FRAME_ID_REG  (NPU_CFG_BASE_ADDR + 0x18)
#define NPU_PROC_NET_FRAME_ID_REG (NPU_CFG_BASE_ADDR + 0x1C)
#define NPU_IRQ_MASK_REG          (NPU_CFG_BASE_ADDR + 0x20)
#define NPU_IRQ_CLR_REG           (NPU_CFG_BASE_ADDR + 0x24)
#define NPU_AXI_QOS_REG           (NPU_CFG_BASE_ADDR + 0x28)
#define NPU_SRAM_ADDR_REG         (NPU_CFG_BASE_ADDR + 0x2C)
#define NPU_SRAM_SIZE_REG         (NPU_CFG_BASE_ADDR + 0x30)
#define NPU_RUNTIME_ADDR_REG      (NPU_CFG_BASE_ADDR + 0x34)
#define NPU_PARAMS_ADDR_REG       (NPU_CFG_BASE_ADDR + 0x38)
#define NPU_INPUT_ADDR_0_REG      (NPU_CFG_BASE_ADDR + 0x3C)
#define NPU_OUTPUT_ADDR_0_REG     (NPU_CFG_BASE_ADDR + 0xBC)
#define NPU_APB_DBG_CTRL_REG      (NPU_CFG_BASE_ADDR + 0x13C)
#define NPU_APB_DBG_VALUE_REG     (NPU_CFG_BASE_ADDR + 0x140)
#define NPU_SRAM_RME_CTRL_REG     (NPU_CFG_BASE_ADDR + 0x144)
#define NPU_CALLBACK_ID_0_REG     (NPU_CFG_BASE_ADDR + 0x148)
#define NPU_CALLBACK_ID_1_REG     (NPU_CFG_BASE_ADDR + 0x14C)
#define NPU_CALLBACK_ID_2_REG     (NPU_CFG_BASE_ADDR + 0x150)
#define NPU_CALLBACK_ID_3_REG     (NPU_CFG_BASE_ADDR + 0x154)
#define NPU_CALLBACK_ACK_ID_0_REG (NPU_CFG_BASE_ADDR + 0x158)
#define NPU_CALLBACK_ACK_ID_1_REG (NPU_CFG_BASE_ADDR + 0x15C)
#define NPU_CALLBACK_ACK_ID_2_REG (NPU_CFG_BASE_ADDR + 0x160)
#define NPU_CALLBACK_ACK_ID_3_REG (NPU_CFG_BASE_ADDR + 0x164)
#define NPU_AXI_ARCACHE_0_REG     (NPU_CFG_BASE_ADDR + 0x168)
#define NPU_AXI_ARCACHE_1_REG     (NPU_CFG_BASE_ADDR + 0x16C)
#define NPU_AXI_ARCACHE_2_REG     (NPU_CFG_BASE_ADDR + 0x170)
#define NPU_AXI_ARCACHE_3_REG     (NPU_CFG_BASE_ADDR + 0x174)
#define NPU_AXI_AWCACHE_0_REG     (NPU_CFG_BASE_ADDR + 0x178)
#define NPU_AXI_AWCACHE_1_REG     (NPU_CFG_BASE_ADDR + 0x17C)
#define NPU_AXI_AWCACHE_2_REG     (NPU_CFG_BASE_ADDR + 0x180)
#define NPU_AXI_AWCACHE_3_REG     (NPU_CFG_BASE_ADDR + 0x184)
#define NPU_CALLBACK_MASK_ARM_0_REG    (NPU_CFG_BASE_ADDR + 0x188)
#define NPU_CALLBACK_MASK_ARM_1_REG    (NPU_CFG_BASE_ADDR + 0x18C)
#define NPU_CALLBACK_MASK_ARM_2_REG    (NPU_CFG_BASE_ADDR + 0x190)
#define NPU_CALLBACK_MASK_ARM_3_REG    (NPU_CFG_BASE_ADDR + 0x194)
#define NPU_CALLBACK_MASK_CEVA_0_REG    (NPU_CFG_BASE_ADDR + 0x198)
#define NPU_CALLBACK_MASK_CEVA_1_REG    (NPU_CFG_BASE_ADDR + 0x19C)
#define NPU_CALLBACK_MASK_CEVA_2_REG    (NPU_CFG_BASE_ADDR + 0x1A0)
#define NPU_CALLBACK_MASK_CEVA_3_REG    (NPU_CFG_BASE_ADDR + 0x1A4)


#define NPU_IRQ_DONE_BIT          (0x1 << 4)
#define NPU_IRQ_HANG_BIT          (0x1 << 5)
#define NPU_IRQ_CB_BIT            (0x1 << 6)
#define NPU_IRQ_PAUSE_BIT         (0x1 << 7)

#define NPU_IRQ_CEVA_DONE_BIT          (0x1 << 12)
#define NPU_IRQ_CEVA_HANG_BIT          (0x1 << 13)
#define NPU_IRQ_CEVA_CB_BIT            (0x1 << 14)
#define NPU_IRQ_CEVA_PAUSE_BIT         (0x1 << 15)

#define NPU_IRQ_DONE_CLR          (0x1 << 4)
#define NPU_IRQ_HANG_CLR          (0x1 << 5)
#define NPU_IRQ_PAUSE_CLR         (0x1 << 7)


#define MAX_NETWORK_NUM 128
#define MAX_CB_NUM 128
#define MAX_NAME_LEN 256



typedef struct
{
    AR_U32 u32ID;
    AR_U32 u32Bank;
    AR_U32 u32Offset;
    AR_U32 u32Height;
    AR_U32 u32KStep;
    AR_U32 u32KNormNum;
    AR_U32 u32KSizeLast;
    AR_U32 u32KSizeNorm;
    AR_CHAR achName[MAX_NAME_LEN];
    AR_CHAR achType[MAX_NAME_LEN];
    AR_U32 u32Num;
    AR_U32 u32OriChannels;
    AR_U32 u32OriFrameSize;
    AR_U32 u32Precision;
    AR_U32 u32RowStep;
    AR_U32 u32TensorStep; //used for batch mode
    AR_DOUBLE dScaleFactor;
    AR_U32 u32Size;
    AR_U32 u32Width;
    AR_S32 s32ZeroPoint;
	AR_CHAR achLayoutType[MAX_NAME_LEN];
	AR_CHAR achMemoryType[MAX_NAME_LEN];
} AR_NPU_TENSOR_S;


typedef struct
{
    AR_U32 u32Id;
    AR_U32 u32InputTensorNum;
    AR_NPU_TENSOR_S astInputTensor[8]; //tmp 8
    AR_U32 u32OutputTensorNum;
    AR_NPU_TENSOR_S astOutputTensor[8];
    AR_CHAR achOperatorName[MAX_NAME_LEN];
    AR_CHAR achOperatorType[MAX_NAME_LEN];
	void* pOpParams;
	AR_U32 u32OpParamsLen;
} AR_NPU_CB_PARAM_S;


typedef struct
{
	AR_U32 u32NetworkId;
	AR_U32 au32InputAddr;
	AR_U32 au32OutputAddr;
	AR_NPU_CB_PARAM_S astCBParam[MAX_CB_NUM];
} AR_NPU_LIST_ITEMS_st;


typedef struct {
    AR_LIST_t list;
    AR_NPU_LIST_ITEMS_st* xNetParam;
} AR_NETPARAM_LIST_st;

int32_t xArNetParamListInit(AR_LIST_t* list_head);
int32_t xArNetParamListAdd(AR_LIST_t* list_head, AR_NPU_LIST_ITEMS_st* xNetParam);
void vArNetParamListRemoveByNetParams(AR_LIST_t * list_head, AR_U32 u32NetworkId,AR_U32 au32InputAddr,AR_U32 au32OutputAddr);
AR_NETPARAM_LIST_st* pxArNetParamListGetByNetWorkId(AR_LIST_t * list_head, AR_U32 u32NetworkId);
int32_t xArNetParamListIsExistByNetWorkId(AR_LIST_t* list_head,  AR_U32 u32NetworkId);
void vArNetParamListRemoveAll(AR_LIST_t* list_head);


typedef struct
{
    AR_U32 u32NetworkId;
	AR_U32 au32CBAckIDs[4];
} AR_NPU_IRQ_PARAMS_st;



#define JSON_PARSE_INT_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        structptr->member = pNode->valueint;\
    }\
}while(0)

#define JSON_PARSE_DOUBLE_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        structptr->member = pNode->valuedouble;\
    }\
}while(0)

#define JSON_PARSE_STR_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        strcpy(structptr->member, pNode->valuestring);\
    }\
}while(0)

#define JSON_PARSE_STR_ARRAY_VALUE(structptr, member, node, name) \
do{\
	cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
	if(pNode)\
	{\
		AR_U32 u32ArraySize = cJSON_GetArraySize(pNode);\
		for(int idx = 0; idx < u32ArraySize; idx++)\
		{\
			cJSON * pSub = cJSON_GetArrayItem(pNode, idx);\
			if (!pSub) continue;\
			strcpy(structptr->member[idx], pSub->valuestring); \
		}\
	}\
}while(0)

#define JSON_PARSE_INT_ARRAY_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        AR_U32 u32ArraySize = cJSON_GetArraySize(pNode);\
        for(int idx = 0; idx < u32ArraySize; idx++)\
        {\
            cJSON * pSub = cJSON_GetArrayItem(pNode, idx);\
            if (!pSub) continue;\
            structptr->member[idx]= pSub->valueint; \
        }\
    }\
}while(0)


AR_S32 xArNpuParseCBFile(AR_CHAR * pu8FilePhyAddr, AR_NPU_CB_PARAM_S * pstCB, AR_U32 * pu32CBNum);
void vPortint1NPUHandler();

#endif //__AR_NPU_PLATFORM__

