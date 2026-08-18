#ifndef __ARCNN_BASE_H__
#define __ARCNN_BASE_H__

#include "mpi_npu_api.h"
#include "mpi_vpss.h"
#include "binder_ipc.h"
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

#define ARCNN_LOG_ERR       1
#define ARCNN_LOG_WARNING   2
#define ARCNN_LOG_DEBUG     3
#define ARCNN_LOG_INFO      4

#define ARCNN_INPUT_LIVE   1
#define ARCNN_INPUT_LOCAL   2

#define ARCNN_MAX_PROFILE   128

#define ALIGNED_256B(x) ((x)%256 == 0 ? (x) : (((x)/256 + 1) * 256))
#define ALIGNED_32B(x) ((x)%32 == 0 ? (x) : (((x)/32 + 1) * 32))


// typedef struct
// {
//     AR_U32 u32X;
//     AR_U32 u32Y;
//     AR_U32 u32RGB; //rgb888
//     AR_CHAR achStr[IPC_ALG_OSD_STR_MAX_LEN];
// } IPC_OSD_STRING_S;
// typedef struct
// {
//     AR_U32 u32X;
//     AR_U32 u32Y;
//     AR_U32 u32W;
//     AR_U32 u32H;
//     AR_U32 u32RGB; //rgb888
// } IPC_OSD_RECT_S;

// typedef struct
// {
//     AR_U32 u32VpssGrp; //will draw osd on all streams if 'alg_osd_enable' is set on this stream(cfg_osd.json)
//     AR_U32 u32StrNum;  //max 32
//     IPC_OSD_STRING_S stOsdStr[IPC_ALG_OSD_MAX_NUM];
//     AR_U32 u32RectNum; //max 32
//     IPC_OSD_RECT_S stOsdRect[IPC_ALG_OSD_MAX_NUM];
//     AR_U32 u32ImgWidth; //the reference img w, used to calculate real location on video channel.
//     AR_U32 u32ImgHeight;
// } IPC_CTRL_OSD_S;


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

    AR_U32 GetMode();
    AR_U32 IsIFCEnabled();
    //on which video that this network runs.
    AR_S32 BindVideoDev();
    AR_S32 GetLiveImage(AR_IMG_S * pstImg);
    AR_S32 ReleaseLiveImage(AR_IMG_S * pstImg);
    AR_S32 GetLiveImageImpl(AR_IMG_S * pstImg);
    AR_S32 ReleaseLvieImageImpl(AR_IMG_S * pstImg);
    AR_S32 DeBindVideoDev();

    //For local image test.
    string GetLocalTestDir();
    AR_S32 GetLocalImage(AR_CHAR * pchFileName, AR_IMG_S * pstImg, AR_U32 u32InputId);
    AR_S32 ReleaseLocalImage(AR_IMG_SET_S * pstImg);
    AR_S32 FillInputBuff(AR_CHAR * pchFileName, AR_U32 u32InputId, AR_U32 u32BatchId);
    void GetInputScale(AR_U32 imageWidth, AR_U32 imageHeight, AR_U32 basicWidth, AR_U32 basicHeight, float &inputScale, AR_U32& inputWidth, AR_U32 &inputHeight)
	{
        AR_U32 newInputHeight = basicHeight;
        float scale = basicHeight/(float)imageHeight;
        AR_U32 newInputWidth = imageWidth * scale;
        if(newInputWidth > basicWidth){
            newInputWidth = basicWidth;
            scale = basicWidth/(float)imageWidth;
            newInputHeight = imageHeight * scale;
        }
        inputScale = 1/scale;
        inputWidth = newInputWidth;
        inputHeight = newInputHeight;
    }

	void GetImageSize(string      filename, AR_U32& imgw, AR_U32& imgh)
    {
		int strLen = 0;
		strLen = filename.size();
		int idx=0;
		int num0=0;
		int num1=0;
		vector<int> a;
		idx = filename.find("x");	//return the location of "x"
		
		for(int i=0; i<idx; i++)
		{
			while(filename[i]>='0' && filename[i]<='9')
			{
				int j = i;
				int len=0;
				while(filename[i]>='0' && filename[i]<='9')
				{
					i++;
					len++;
				}
				
				string s0 = filename.substr(j, len);
				for(int si=0; si<len; si++)
				{
					int tmp = (int)(s0[si] - '0');
					num0 += tmp * pow(10, (len-si-1));
				}
			}
		}
		
		for(int i=idx+1; i<idx+5; i++)
		{
			while(filename[i]>='0' && filename[i]<='9')
			{
				int j = i;
				int len=0;
				while(filename[i]>='0' && filename[i]<='9')
				{
					i++;
					len++;
				}
				
				string s0 = filename.substr(j, len);
				for(int si=0; si<len; si++)
				{
					int tmp = (int)(s0[si] - '0');
					num1 += tmp * pow(10, (len-si-1));
				}
			}
		}
		imgw = num0;
		imgh = num1;

	}

    AR_S32 Forward(AR_IMG_SET_S * pstImg,  AR_BOOL bInstant, AR_BOOL bDebug);
    AR_S32 PostProcess(void * args);
    void RegisterPostCB(PostProcessCB cbFuncPtr);

    AR_S32 Log(AR_U32 u32LogLevel, const AR_CHAR * fmt, ...);
    AR_S32 SetLogLevel(AR_U32 u32LogLevel);

    AR_S32 EnableProfile();
    AR_S32 DisbleProfile();
    AR_S32 ProfileStart(AR_CHAR * pchName);
    AR_S32 ProfileEnd(AR_CHAR * pchName);

    AR_S32 EnableNPUDebug(AR_U32 u32Start, AR_U32 u32End);
    AR_S32 DisableNPUDebug();
    AR_S32 RegisterLayerDebugFunc(AR_NPU_LayerDebugFunc pDebugFunc);
    AR_S32 RegisterIFCDebugFunc(AR_NPU_IFCDebugFunc pCBFunc);
    AR_S32 RegisterCallbackFunc(AR_CHAR * chName, AR_NPU_CallbackFunc pCBFunc);
	AR_S32 RegisterParseFunc(AR_CHAR * chName, AR_NPU_ParseFunc pParseFunc);

    void ReadImageThread();
    void DrawOutputThread();

    AR_S32 MallocNPUBuff();
    AR_S32 FreeNPUBuff();

    AR_U32 GetInputNum();
    AR_U32 GetBatchNum();

    AR_U32 IsInitialized();
private:
    AR_NPU_CNN_DESC_S stCNNDesc;

    ar_ipc_state_t* pstIpcClient;
    uint32_t u32ClientHandle;
    VPSS_GRP s32VpssGrp;
    VPSS_CHN s32VpssChn;
    VPSS_CHN s32OsdChn;

    //handle
    void * handle;

    bool bContinued;

    AR_U32 u32IfcPreProcess;
    AR_U32 u32Mode;//local or live?

    //this is for npu input buffer, also known as IFC output.
    AR_MEM_S stNPUInBuff;
    //NPU final output buffer
    AR_MEM_S stNPUOutBuff;

    //input image
    string strDevName;
    #if 0
    ar_picture_handle hLiveHandle;
    AR_S32 s32LiveFd;
    ar_picture_buffer_t *pstLiveFrame;
    stLiveImage * pstLiveImage;
    #endif
    
    AR_IMG_S *pstSwapImage;
    AR_IMG_S *stpSrcImgs;
	AR_IMG_S *stpDstImgs;
    AR_IMG_S *pendingImage;
    AR_IMG_S *currentImage;
    std::queue<AR_IMG_S*> imageQueue;

    AR_U32 u32ImageWidth;
    AR_U32 u32ImageHeight;


    AR_U32 u32InputWidth;
    AR_U32 u32InputHeight;
    float  f32InputScale;
    AR_U32 u32ScaleWidth;
    AR_U32 u32ScaleHeight;

    string strCurrentImage;

    std::thread readerThread;
    std::mutex imageMutex;

    std::thread displayThread;
    std::queue<std::pair<AR_NPU_TENSOR_S,AR_CHAR*>> tensorQueue;
    std::mutex tensorMutex;

    string strLocalDir;
    AR_CHAR * pchLocalImage; //buffer
    PostProcessCB cbFunc;
	
    AR_U32 u32InputNum; //depends on cnn model.
    AR_U32 u32BatchNum;

    int s32CompFd;
    int s32CanvasId;
    AR_U32 u32DrawOSD;
    AR_U32 u32LogLevelCtrl;
    string strOsdName;

    AR_U32 u32ProfileEnable;
    AR_PROFILE_S astProfile[ARCNN_MAX_PROFILE];

    AR_U32 u32Initialized;
};

#endif
