#ifndef __ARMTCNN_BASE_H__
#define __ARMTCNN_BASE_H_

#include "mpi_npu_api.h"
#include "armtcnn_common.h"
#include "mpi_scaler_api.h"

//typedef ar_rpc_sink_buffer_t stLiveImage;
#define ARCNN_LOG_ERR       1
#define ARCNN_LOG_WARNING   2
#define ARCNN_LOG_DEBUG     3
#define ARCNN_LOG_INFO      4

#define ARCNN_INPUT_LIVE   1
#define ARCNN_INPUT_LOCAL   2

#define ARMTCNN_MIN_FACE_SIZE  20.0
#define ARMTCNN_PYRAMID_FACTOR  0.709

#define ARMTCNN_STRIDE      2
#define ARMTCNN_CELLSIZE    12.0
#define ARMTCNN_RNET_WIDTH  24
#define ARMTCNN_RNET_HEIGHT  24
#define ARMTCNN_ONET_WIDTH   48
#define ARMTCNN_ONET_HEIGHT  48

#define ARMTCNN_POST_MODE_PNET  1
#define ARMTCNN_POST_MODE_RNET  2
#define ARMTCNN_POST_MODE_ONET  3

#define GET_BBOX_TOP(x, scale) ((AR_DOUBLE)(ARMTCNN_STRIDE * (x) + 1) / (scale))
#define GET_BBOX_BOTTOM(x, scale) ((AR_DOUBLE)(ARMTCNN_STRIDE * (x) + ARMTCNN_CELLSIZE -1 + 1) / (scale))
#define FIX_TO_FLOAT(x, zero_point, scale_factor) (((x) - (zero_point)) * (scale_factor))
#define CAP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define ALIGNED_256B(x) ((x)%256 == 0 ? (x) : (((x)/256 + 1) * 256))
#define ARCNN_MAX_PROFILE   32
typedef struct
{
    string strName;
    AR_U64 u64Tic;
    AR_U64 u64Toc;
} AR_PROFILE_S;
typedef AR_S32 (*PostProcessCB)(void *);

//This is the base class for AR CNN system
class CARMTCNN
{
public:
    CARMTCNN();
    ~CARMTCNN();
    AR_S32 Initialize(AR_CHAR * pchCfgFile); //json file

    AR_S32 Forward(AR_IMG_SET_S * pstImg, AR_BOOL bInstant, AR_BOOL bDebug);
    AR_S32 PostProcess(AR_U32 u32PostMode); //get prob score and bbox and delta
    AR_S32 Nms(AR_DOUBLE fThresh);

    AR_S32 SetScaleParam(AR_DOUBLE dScale, AR_U32 u32W, AR_U32 u32H, AR_U32 u32SrcW, AR_U32 u32SrcH);
    AR_S32 SetThreshold(AR_DOUBLE fThresh);
    AR_U32 GetBondingBoxNum();
    BONDING_BOX_S GetBondingBox();
    AR_U32 GetBatchNum();

    AR_S32 DumpOutput(AR_CHAR * pFile);
    void RegisterPostCB(PostProcessCB cbFuncPtr);

    AR_S32 Log(AR_U32 u32LogLevel, const AR_CHAR * fmt, ...);
    AR_S32 SetLogLevel(AR_U32 u32LogLevel);

    AR_S32 EnableProfile();
    AR_S32 DisableProfile();
    AR_S32 ProfileStart(string pchName);
    AR_S32 ProfileEnd(string pchName);

    AR_S32 EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End);
    AR_S32 DisableNPUDebug();
    AR_S32 RegisterLayerDebugFunc(AR_NPU_LayerDebugFunc pDebugFunc);
    AR_S32 RegisterIFCDebugFunc(AR_NPU_IFCDebugFunc pCBFunc);
    AR_S32 RegisterCallbackFunc(AR_CHAR * chName, AR_NPU_CallbackFunc pCBFunc);

    AR_S32 MallocNPUBuff();
    AR_S32 FreeNPUBuff();

    AR_U32 GetInputNum();
	AR_S32 GetRuntimeSize();
	AR_S32 SetRuntimeBuffer(AR_UINTPTR uptrVirt, AR_U64 u64Phy);

    AR_U32 IsInitialized();
	AR_U32 GetIfcPreProcess();
private:
    AR_NPU_CNN_DESC_S stCNNDesc;
    //handle
    void * handle;

    AR_U32 u32IfcPreProcess;

    AR_U32 u32LogLevelCtrl;

    AR_U32 u32ProfileEnable;
    AR_PROFILE_S astProfile[ARCNN_MAX_PROFILE];

    AR_U32 u32Initialized;

    AR_DOUBLE fScoreThresh; //prob threshold.
    AR_DOUBLE dPnetScale; // 12/minSize  * factor^(N) where N means the pyramid level index.
    AR_U32 u32Width; //input w,h, should be same as input.json parameters
    AR_U32 u32Height;
    AR_U32 u32SrcWidth; //the original picture width
    AR_U32 u32SrcHeight;

    //this is for npu input buffer, also known as IFC output.
    AR_MEM_S stNPUInBuff;
    //NPU final output buffer
    AR_MEM_S stNPUOutBuff;

    AR_U32 u32InputNum; //depends on cnn model.
    AR_U32 u32BatchNum;

    BondingBoxList listBondingBox;

};

class CARFaceDetector
{
public:
    CARFaceDetector(AR_CHAR * pchCfgFile); //json file
    ~CARFaceDetector();

    AR_S32 Process(AR_IMG_S * pstImg,   AR_BOOL bInstant, AR_BOOL bDebug);
    AR_S32 PnetProcess(AR_IMG_S * pstImg);
    AR_S32 RnetProcess(AR_IMG_S * pstImg);
    AR_S32 OnetProcess(AR_IMG_S * pstImg);

    AR_S32 CropAndResize(AR_IMG_S * pstSrcImg, AR_HAL_SCALER_CROP_S * pstCrop, AR_IMG_S * pstDstImg, AR_U32 u32ImgNum);

    AR_U32 GetBatchNum();
    AR_U32 GetMode();
    //on which video that this network runs.
    AR_S32 BindVideoDev();
    AR_S32 GetLiveImage(AR_IMG_S * pstImg);
    AR_S32 ReleaseLiveImage(AR_IMG_S * pstImg);
    AR_S32 DeBindVideoDev();

    //For local image test.
    string GetLocalTestDir();
    AR_S32 GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg);
    AR_S32 ReleaseLocalImage(AR_IMG_S * pstImg);

    AR_S32 SetLogLevel(AR_U32 u32LogLevel);
    AR_S32 EnableProfile();
    AR_S32 DisableProfile();

    AR_S32 EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End);
    AR_S32 DisableNPUDebug();
    AR_S32 EnableIFCDebug();

    AR_S32 DrawOsd();
    AR_S32 DrawOsd(AR_CHAR * pFile);
    AR_U32 IsInitialized();

private:
    AR_U32 u32PnetNum; //depends on the scale factor
    CARMTCNN mPnet[12];
    CARMTCNN mRnet;
    CARMTCNN mOnet;

    AR_U32 u32Mode;//local or live?

    //input image
    string strDevName;
    AR_S32 s32LiveFd;
    //stLiveImage * pstLiveImage;

    string strLocalDir;
    AR_CHAR * pchLocalImage; //buffer
    PostProcessCB cbFunc;

    string strOsdDevName;
    AR_U32 u32DrawOSD;
    AR_DOUBLE fPnetNmsThresh;
    AR_DOUBLE fRnetNmsThresh;
    AR_DOUBLE fOnetNmsThresh;

    BondingBoxList listTotalBondingBox; //total bonding box
    AR_MEM_S stRuntimeSharedBuff;
	AR_S32 u32RuntimeMaxSize;

    AR_U32 u32Initialized;
};
#endif
