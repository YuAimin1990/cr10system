#ifndef __ARCNN_BASE_H__
#define __ARCNN_BASE_H__

#include "mpi_npu_api.h"

#define ARCNN_LOG_ERR       1
#define ARCNN_LOG_WARNING   2
#define ARCNN_LOG_DEBUG     3
#define ARCNN_LOG_INFO      4

#define ARCNN_INPUT_LIVE   1
#define ARCNN_INPUT_LOCAL   2

#define ARCNN_MAX_PROFILE   32

#define ALIGNED_256B(x) ((x)%256 == 0 ? (x) : (((x)/256 + 1) * 256))
#define ALIGNED_32B(x) ((x)%32 == 0 ? (x) : (((x)/32 + 1) * 32))

typedef struct
{
    string strName;
    AR_U64 u64Tic;
    AR_U64 u64Toc;
} AR_PROFILE_S;

typedef AR_S32 (*PostProcessCB)(void *);

//This is the base class for AR CNN system
class CARCNN
{
public:
    CARCNN(AR_CHAR * pchCfgFile); //json file
    ~CARCNN();

    AR_S32 HalVinInit(AR_S32 DevId,AR_S32 StreamId);
    AR_U32 GetMode();
    AR_U32 IsIFCEnabled();
	AR_U32 IsCbEnabled();
    //on which video that this network runs.
    AR_S32 BindVideoDev();
    AR_S32 GetLiveImage(AR_IMG_S * pstImg);
    AR_S32 ReleaseLiveImage(AR_IMG_S * pstImg);
    AR_S32 DeBindVideoDev();

    //For local image test.
    string GetLocalTestDir();
    AR_S32 GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId);
    AR_S32 ReleaseLocalImage(AR_IMG_SET_S * pstImg);
    AR_S32 FillInputBuff(AR_CHAR * pchFileName, AR_U32 u32InputId, AR_U32 u32BatchId);

    AR_S32 Forward(AR_IMG_SET_S * pstImg,  AR_BOOL bInstant, AR_BOOL bDebug);
    AR_S32 PostProcess(void * args);
    void RegisterPostCB(PostProcessCB cbFuncPtr);

    AR_S32 Log(AR_U32 u32LogLevel, const AR_CHAR * fmt, ...);
    AR_S32 SetLogLevel(AR_U32 u32LogLevel);

    AR_S32 EnableProfile();
    AR_S32 DisbleProfile();
    AR_S32 ProfileStart(string pchName);
    AR_S32 ProfileEnd(string pchName);

    AR_S32 EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End);
    AR_S32 DisableNPUDebug();
    AR_S32 RegisterLayerDebugFunc(AR_NPU_LayerDebugFunc pDebugFunc);
    AR_S32 RegisterIFCDebugFunc(AR_NPU_IFCDebugFunc pCBFunc);
    AR_S32 RegisterCallbackFunc(AR_CHAR * chName, AR_NPU_CallbackFunc pCBFunc);
	AR_S32 SetDSPCallbackMask(AR_U32 *pu32CBId, AR_U32 u32CBNum);
	AR_S32 RegisterDSPCallback();
	AR_S32 UnRegisterDSPCallback();
	AR_S32 RegisterParseFunc(AR_CHAR * chName, AR_NPU_ParseFunc pParseFunc);

    AR_S32 MallocNPUBuff();
    AR_S32 FreeNPUBuff();
    AR_S32 MallocRuntimeBuff();
    AR_S32 FreeRuntimeBuff();

    AR_U32 GetInputNum();
    AR_U32 GetBatchNum();
	AR_U32 GetNetPerf(AR_U32);
    AR_U32 IsInitialized();
private:
    AR_NPU_CNN_DESC_S stCNNDesc;

	AR_S32 mDevId;
	AR_S32 mStreamId;
	AR_S32 mClientId;
	AR_S32 mStreamFd;
	AR_S32 mPipeId;
	AR_S32 mPipeFd;

	AR_U32 u32TotalPreTime;
	AR_U32 u32TotalFwTime;
	AR_U32 u32TotalCBTime;
	AR_U32 u32FirstPreTime;
	AR_U32 u32FirstFwTime;
	AR_U32 u32FirstCBTime;

    //handle
    void * handle;

    //by disabling it can do a pure NPU test(input is not images but tensors)
    AR_U32 u32IfcPreProcess;
	AR_U32 u32CbEnable;
    AR_U32 u32Mode;//local or live?

    //this is for npu input buffer, also known as IFC output.
    AR_MEM_S stNPUInBuff;
    //NPU final output buffer
    AR_MEM_S stNPUOutBuff;
	AR_MEM_S stNPURtBuff;

    //input image
    string strDevName;
    AR_S32 s32LiveFd;
    //stLiveImage * pstLiveImage;

    string strLocalDir;
    AR_CHAR * pchLocalImage; //buffer
    PostProcessCB cbFunc;

    AR_U32 u32InputNum; //depends on cnn model.
    AR_U32 u32BatchNum;

    AR_U32 u32DrawOSD;
    AR_U32 u32LogLevelCtrl;

    AR_U32 u32ProfileEnable;
    AR_PROFILE_S astProfile[ARCNN_MAX_PROFILE];

    AR_U32 u32Initialized;

	AR_U32 u32ImgWidth;
	AR_U32 u32ImgHeight;

	AR_U32 u32FisrtRun;

};

#endif
