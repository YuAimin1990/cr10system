#ifndef __AR_NPU_HANDLE_H__
#define __AR_NPU_HANDLE_H__
#include "hal_type.h"
#include "hal_npu_types.h"
#include "hal_ifc_api.h"
#include <semaphore.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


typedef struct AR_NPU_CALLBACK_NODE_S
{
    SLIST_ENTRY(AR_NPU_CALLBACK_NODE_S) next;
    AR_CHAR achName[32];
    AR_NPU_CallbackFunc pCallbackFunc;
} AR_NPU_CALLBACK_NODE_S;

typedef SLIST_HEAD(head, AR_NPU_CALLBACK_NODE_S)  AR_NPU_CALLBACK_HEAD_S;


typedef struct AR_NPU_PARSEFUNC_NODE_S
{
    SLIST_ENTRY(AR_NPU_PARSEFUNC_NODE_S) next;
    AR_CHAR achName[32];
    AR_NPU_ParseFunc pParseFunc;
} AR_NPU_PARSEFUNC_NODE_S;

typedef SLIST_HEAD(parsehead, AR_NPU_PARSEFUNC_NODE_S)  AR_NPU_PARSEFUNC_HEAD_S;


typedef struct
{
    AR_U32 u32EnableDump;
    AR_U32 u32StartLayer;
    AR_U32 u32EndLayer;
    AR_CHAR achDumpPath[128];
} AR_NPU_DEBUG_PARAM_S;

typedef struct
{
	AR_UINTPTR u32SCUAddr;
	AR_U32     u32SCUAddrPhy;
	AR_UINTPTR u32WeightsAddr;
	AR_U32	   u32WeightsAddrPhy;
	AR_UINTPTR u32RuntimeAddr;
	AR_U32	   u32RuntimeAddrPhy;
	AR_U32     u32RuntimeSharedEn;
    #if 0
	AR_UINTPTR au32InputAddr[MAX_INPUT_NUM];
	AR_U32 au32InputAddrPhy[MAX_INPUT_NUM];
	AR_UINTPTR au32InputAddrAligned[MAX_INPUT_NUM];
	AR_U32 au32InputAddrAlignedPhy[MAX_INPUT_NUM];
	AR_UINTPTR au32OutputAddr[MAX_OUTPUT_NUM];
	AR_U32     au32OutputAddrPhy[MAX_OUTPUT_NUM];
	AR_UINTPTR au32OutputAddrAligned[MAX_OUTPUT_NUM];
	AR_U32     au32OutputAddrAlignedPhy[MAX_OUTPUT_NUM];
    #endif
	AR_UINTPTR u32SramAddrPhy;
	AR_U32 u32SCUSize;
	AR_U32 u32WeightsSize;
	AR_U32 u32RuntimeSize;
	AR_U32 u32InputSize;
	AR_U32 u32OutputSize;
	AR_U32 u32SramSize;
	AR_U32 u32TotalLayerNum;
	AR_U32 u32BatchNum; //this is for batch mode
	AR_U32 u32RGBOffset[MAX_INPUT_IMG_NUM]; //R<->G<->B offset in batch mode
	AR_U32 u32IFCTensorNum;
	AR_CHAR achIFCTensorName[MAX_INPUT_IMG_NUM][MAX_NAME_LEN];

	//For 3channel, only 1 ifc needed.
	//For 4channel, need to do twice.
	AR_NPU_IFC_PARAM_S  stIFCParam[MAX_INPUT_IMG_NUM][2];
	AR_NPU_IFCDebugFunc pIFCDebugFunc; //will be called once IFC is done.

	AR_U32 u32CBToArm; //callback to arm or dsp
	AR_U32 u32CBNum;
	AR_NPU_CB_PARAM_S   astCBParam[MAX_CB_NUM];
	AR_NPU_CALLBACK_HEAD_S * pCallbackListHead;
	AR_NPU_PARSEFUNC_HEAD_S * pParseFuncListHead;
	AR_CHAR *pCbBuff;
	AR_U32 u32CbBuffLen;

        AR_NPU_POST_CB_PARAM_S astPostCBParam[MAX_CB_NUM];
       AR_U32 u32PostCBNum;
       AR_CHAR *pPostCBBuff;
       AR_U32 u32PostCBBuffLen;
	//debug mode: when paused, will call this cb
	AR_NPU_DEBUG_PARAM_S stDebugParam;
	AR_NPU_LayerDebugFunc pLayerDebugFunc;

	//input parameters
	AR_U32 u32InputTensorNum;
	AR_NPU_TENSOR_S astInputParam[MAX_INPUT_TENSOR]; //tmp 64 input tensor num ?

	//finale output parameters
	AR_U32 u32OutputTensorNum;
	AR_NPU_TENSOR_S astOutputParam[MAX_OUTPUT_TENSOR]; //tmp 64 output tensor num

} AR_NPU_CNN_MODEL_S;


typedef struct
{
    AR_U16 u16NetworkId;
    AR_U32 u32Priority;
	AR_U32 u32NetVersion;
    AR_NPU_CNN_MODEL_S * pstCNNModel;
    pthread_t callback_thread_t;
    //for callback perf, unit 0.01ms.
    AR_U32 u32CbTime[128];
    //for ifc perf, unit 0.01ms
    AR_U32 u32NPUPreTime;
	AR_U32 u32NPURunTime;
} AR_NPU_CNN_HANDLE_S;

typedef  AR_NPU_CNN_HANDLE_S * AR_NPU_CNN_HANDLE;

typedef struct
{
    AR_NPU_CNN_HANDLE_S stHandle;
	AR_U32 u32YUVIStep[MAX_INPUT_IMG_NUM];
	AR_U32 u32IFCFrameNum;
	AR_NPU_CB_PARAM_EXT_S astCBExtParam[MAX_CB_NUM];
	AR_NPU_TENSOR_EXT_S astInputExtParam[MAX_INPUT_TENSOR]; //tmp 64 input tensor num ?
	AR_NPU_TENSOR_EXT_S astOutputExtParam[MAX_OUTPUT_TENSOR]; //tmp 64 output tensor num
	AR_MEM_S *pIn;
    AR_MEM_S *pOut;
	sem_t cbCome;
	sem_t cbDone;
	AR_U32 au32CBIDs[4];
	//new define for multi input zero mem cpy version
	AR_U32 u32BankSize[32];
	AR_CHAR acResv[524160]; //512KB - 32x4 = 524160
} AR_NPU_CNN_MODEL_EXT_S;


typedef enum
{
    NPU_IDLE = 0,
    NPU_RUNNING = 1
} AR_NPU_CURR_STATE_E;

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
