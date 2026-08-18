#ifndef __ARCNN_BASE_H__
#define __ARCNN_BASE_H__

#include "mpi_npu_api.h"

#define ARCNN_LOG_ERR       1
#define ARCNN_LOG_WARNING   2
#define ARCNN_LOG_DEBUG     3
#define ARCNN_LOG_INFO      4

#define ARCNN_MAX_PROFILE   32

//#define ALIGNED_256B(x) ((x)%256 == 0 ? (x) : (((x)/256 + 1) * 256))
//#define ALIGNED_32B(x) ((x)%32 == 0 ? (x) : (((x)/32 + 1) * 32))

typedef struct
{
    string strName;
    AR_U64 u64Tic;
    AR_U64 u64Toc;
} AR_PROFILE_S;

typedef struct
{
    AR_U32 u32TensorInPhy;
    AR_U32 u32TensorOutPhy;
    AR_U32 u32Height;
    AR_U32 u32KStep;
    AR_U32 u32KNormNum;
    AR_U32 u32KSizeLast;
    AR_U32 u32KSizeNorm;
    AR_U32 u32Num;
    AR_U32 u32OriChannels;
    AR_U32 u32Precision;
    AR_U32 u32RowStep;
    AR_U32 u32TensorStep; //used for batch mode
    AR_U32 u32Width;
    AR_S32 s32ZeroPoint;
    AR_DOUBLE dScaleFactor; 
} AR_DSP_PRE_MSG_S;

typedef AR_S32 (*PostProcessCB)(void *);


class CARCNN
{
public:
    CARCNN(AR_CHAR * pchCfgFile);
    ~CARCNN();

    AR_U32 IsIFCEnabled(AR_U32 u32InputId);
	AR_U32 IsCbEnabled();

    string GetLocalTestDir();
    AR_S32 GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg);
    AR_S32 ReleaseLocalImage(AR_IMG_SET_S * pstImg);
	AR_S32 ReleaseLocalInput(AR_IMG_SET_S * pstImg);
    AR_S32 FillInputBuff(AR_CHAR * pchFileName, AR_U32 u32InputId, AR_U32 u32BatchId);
	AR_S32 GetLocalInput(AR_CHAR * pchFileName, AR_IMG_SET_S *stImg, AR_U32 u32BatchId);

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
	AR_S32 RegisterParseFunc(AR_CHAR * chName, AR_NPU_ParseFunc pParseFunc);

    AR_S32 MallocNPUBuff();
    AR_S32 FreeNPUBuff();

    AR_U32 GetInputNum();
    AR_U32 GetBatchNum();

    AR_U32 IsInitialized();
private:
    AR_NPU_CNN_DESC_S stCNNDesc;

    void * handle;

    AR_U32 u32IfcPreProcess[MAX_INPUT_IMG_NUM];
	AR_U32 u32CbEnable;

    AR_MEM_S stNPUInBuff;
    AR_MEM_S stNPUOutBuff;

    string strLocalDir;
    AR_CHAR * pchLocalImage;
    PostProcessCB cbFunc;

    AR_U32 u32InputNum;
    AR_U32 u32BatchNum;

    AR_U32 u32LogLevelCtrl;

    AR_U32 u32ProfileEnable;
    AR_PROFILE_S astProfile[ARCNN_MAX_PROFILE];

    AR_U32 u32Initialized;

	AR_U32 u32ImgWidth;
	AR_U32 u32ImgHeight;

};

#endif
