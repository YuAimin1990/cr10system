#ifndef __ARCNN_BASE_H__
#define __ARCNN_BASE_H__
#include "binder_ipc.h"
#include "mpi_vpss.h"
#include "mpi_npu_api.h"
#include "arcnn_yolov5.h"

#define IPC_RPC_CMD_DRAW_VPSS_OSD (0x76540000+37)
#define IPC_ALG_OSD_MAX_NUM   32
#define IPC_ALG_OSD_STR_MAX_LEN   32

#define ARCNN_LOG_ERR       	1
#define ARCNN_LOG_WARNING 	      2
#define ARCNN_LOG_DEBUG 	    3
#define ARCNN_LOG_INFO	    4

#define ARCNN_INPUT_LIVE	   1
#define ARCNN_INPUT_LOCAL	   2

#define ARCNN_MAX_PROFILE   	32

#define ALIGNED_256B(x) ((x)%256 == 0 ? (x) : (((x)/256 + 1) * 256))
#define ALIGNED_32B(x) ((x)%32 == 0 ? (x) : (((x)/32 + 1) * 32))
#define ALIGNED_16B(x) ((x)%16 == 0 ? (x) : (((x)/16 + 1) * 16))
//#define ALIGNED_NB(x, n) ((x)%n == 0 ? (x) : (((x)/n + 1) * n))

typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32RGB; //rgb888
    AR_CHAR achStr[IPC_ALG_OSD_STR_MAX_LEN];
} IPC_OSD_STRING_S;

typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32W;
    AR_U32 u32H;
    AR_U32 u32RGB; //rgb888
} IPC_OSD_RECT_S;

typedef struct
{
    AR_U32 u32VpssGrp; //will draw osd on all streams if 'alg_osd_enable' is set on this stream(cfg_osd.json)
    AR_U32 u32StrNum;  //max 32
    IPC_OSD_STRING_S stOsdStr[IPC_ALG_OSD_MAX_NUM];
    AR_U32 u32RectNum; //max 32
    IPC_OSD_RECT_S stOsdRect[IPC_ALG_OSD_MAX_NUM];
    AR_U32 u32ImgWidth; //the reference img w, used to calculate real location on video channel.
    AR_U32 u32ImgHeight;
} IPC_CTRL_OSD_S;


typedef struct arBINDER_RET_S
{
	AR_CHAR s8Name[14];
	AR_U32 u32Ret;
} BINDER_RET_S;

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
    AR_S32 DeBindVideoDev();

    //For local image test.
    string GetLocalTestDir();
    AR_S32 GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputID);
    AR_S32 FillInputBuff(AR_CHAR * pchFileName, AR_U32 u32InputId, AR_U32 u32BatchId);

    AR_S32 Forward(AR_IMG_SET_S * pstImg,  AR_BOOL bInstant, AR_BOOL bDebug);
    AR_S32 PostProcess();
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
	void SetFileName(AR_CHAR *value);

    AR_U32 GetInputNum();
    AR_U32 GetBatchNum();

    AR_U32 IsInitialized();
private:
    AR_NPU_CNN_DESC_S stCNNDesc;

	AR_S32 mDevId;
	AR_S32 mStreamId;
	AR_S32 mClientId;
	AR_S32 mStreamFd;
	AR_S32 mPipeId;
	AR_S32 mPipeFd;

    //handle
    void * handle;

    //live mode settings
	AR_S32 VpssGrp;
	AR_S32 VpssChn;
	VIDEO_FRAME_INFO_S VFrameInfo;

	//osd device
	ar_ipc_state_t* pstIpcClient;
    AR_U32 u32ClientHandle;


    //by disabling it can do a pure NPU test(input is not images but tensors)
    AR_U32 u32IfcPreProcess;
	AR_U32 u32CbEnable;
    AR_U32 u32Mode;//local or live?

    //this is for npu input buffer, also known as IFC output.
    AR_MEM_S stNPUInBuff;
    //NPU final output buffer
    AR_MEM_S stNPUOutBuff;

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

	YOLOv5_RSLT detRslts[MAX_NUM_DET];
	YOLOv5_PARAS yolov5_params = {0};
	AR_NPU_IFC_PARAM_S stIFCParam = {0};
	AR_S32 s32FileSize;	
	AR_MEM_S stPchbuff;
	AR_CHAR rsltDir[128];
	AR_CHAR *testFileName;
	AR_U32 u32FileType;
	AR_U32 InFlag;
};

#endif
