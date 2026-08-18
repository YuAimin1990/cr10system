#include "guide_module_manager.h"
#include "ModuleSDK.h"

#include <stdio.h>
#include <fcntl.h>
#include<unistd.h>

#define Y16_DEBUG 1

IRDataFunc g_DataFunc = NULL;
void* g_UserData = NULL;
HANDLE_T g_Module = 0;
YuvImageCallBack g_YuvCallbackFunc = NULL;
Y16DataCallBack g_Y16CallbackFunc = NULL;

int save_y16_data_count = 0;

volatile int g_isShutterControl = 0;

int cbFuncHandle(CallBackData cbData, void * param)
{
    //printf("CallBackData %p %p\n",cbData.y16Data,cbData.yuvData);

    if(g_isShutterControl){
        g_isShutterControl--;
        //printf("drop one y16*****************\n");
        return 0;
    }
    
	if (cbData.yuvData && g_YuvCallbackFunc) {
        g_YuvCallbackFunc(cbData.yuvData, cbData.yuvLength, cbData.width, cbData.height);
	}

	if (cbData.y16Data && g_Y16CallbackFunc) {
        g_Y16CallbackFunc(cbData.y16Data, cbData.y16Length);
	}

#if Y16_DEBUG
    static int y16_fd = 0;
    if (save_y16_data_count>0)
    {
        if(!y16_fd){
            char fname[128] = {0};
            sprintf(fname,"/tmp/dump_raw_y16-%d.raw",save_y16_data_count);
            y16_fd = open(fname, O_RDWR | O_CREAT);
            if(y16_fd){
                printf("save y16 data begin...\n");
            }
        }
        if(y16_fd){
            int r = write(y16_fd, (unsigned char *)cbData.y16Data, cbData.y16Length*2);
            printf("write file, y16len=%d wlen=%d index=%d\n",cbData.y16Length,r,save_y16_data_count);
        }
        save_y16_data_count--;
    }
    else if(y16_fd>0){
        close(y16_fd);
        y16_fd = 0;
        printf("save y16 data end\n");
    }
#endif
    
    return 0;
}

HANDLE_T GUIDE_MODEL_SDK_Init()
{
    int isDebugging = 0, debugMode = 0;
    int clock = 12000000;
    //int clock = 6000000;
    //12MHZ 的配置
	DetectorConf pConf[20] =
	{
		{ 0x02, 0x00 },
		{ 0x03, 0x01 },
		{ 0x04, 0xc8 },
		{ 0x05, 0x00 },
		{ 0x06, 0x80 },
		{ 0x07, 0x00 },
		{ 0x08, 0xa9 },
		{ 0x09, 0x01 },
		{ 0x0a, 0x00 },
		{ 0x0b, 0x00 },
		{ 0x0c, 0x90 },
		{ 0x0d, 0x01 },
		{ 0x0e, 0x00 },
		//{ 0x10, 0x05 },
		{ 0x10, 0x07 },
		{ 0x16, 0x00 },
		{ 0x2f, 0x00 },
		{ 0x30, 0x24 },
		{ 0x31, 0x02 },
		{ 0x33, 0x00 },
		{ 0x01, 0x01 },
	};

    //6MHZ的配置
    /*DetectorConf pConf[20] = {
        {0x02, 0x00},
        {0x03, 0x01},
        {0x04, 0xc8},
        {0x05, 0x00},
        {0x06, 0x30},
        {0x07, 0x00},
        {0x08, 0xc2},
        {0x09, 0x00},
        {0x0a, 0xe0},
        {0x0b, 0x00},
        {0x0c, 0x90},
        {0x0d, 0x01},
        {0x0e, 0x00},
        {0x10, 0x05},
        {0x16, 0x00},
        {0x2f, 0x00},
        {0x30, 0x3d},
        {0x31, 0x02},
        {0x33, 0x00},
        {0x01, 0x01},
    };*/

    char version[32];

    g_Module = CreateModuleInstance(MODE_X16);

    int ret;
    ret = MeasureParamsControl(g_Module, DEBUGGING_PATH, (void *)"/tmp/debug.log");
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: MeasureParamsControl \n");
    ret = MeasureParamsControl(g_Module, DEBUGGING_MODE, &debugMode);
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: MeasureParamsControl 2 \n");
    ret = MeasureParamsControl(g_Module, SET_DEBUGGING, &isDebugging);
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: MeasureParamsControl 3 \n");
    ret = SDKVersion(version, 32);
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: SDKVersion \n");
    ret = SetDetectorClock(g_Module, pConf, 20, clock);
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: SetDetectorClock \n");
    ret = RegisterImgCallBack(g_Module, cbFuncHandle, 0, 1.0);
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: RegisterImgCallBack \n");
    int format = IMG_FORMAT_YUV420PLANE;//21;
    ret = ImageParamsControl(g_Module, SET_FORMAT, &format);
    if(ret==GUIDEIR_ERR) printf("call gd sdk error: ImageParamsControl \n");

    /******************************设置测温参数******************************/
    int flag;
    flag = 0;
    MeasureParamsControl(g_Module, SET_TEMPORAL_FILTER_SWITCH, &flag);  //关闭时域滤波
    MeasureParamsControl(g_Module, SET_SPATIAL_FILTER_SWITCH, &flag);   //关闭空域滤波
    MeasureParamsControl(g_Module, SET_DRT_SWITCH, &flag);              //关闭调光
    MeasureParamsControl(g_Module, SET_BADPOINT_SWITCH, &flag);         //关闭坏点替换
    MeasureParamsControl(g_Module, SET_HORIZON_STRIPE_SWITCH, &flag);   //关闭去横纹
    MeasureParamsControl(g_Module, SET_VERTICAL_STRIPE_SWITCH, &flag);  //关闭去竖纹
    flag = 1;
    //MeasureParamsControl(g_Module, SET_BADPOINT_SWITCH, &flag);         //打开坏点替换
    //MeasureParamsControl(g_Module, SET_HORIZON_STRIPE_SWITCH, &flag);   //打开去横纹
    //MeasureParamsControl(g_Module, SET_VERTICAL_STRIPE_SWITCH, &flag);  //打开去竖纹
    
    MeasureParamsControl(g_Module, GET_TEMPORAL_FILTER_SWITCH, &flag);         
    printf("GET_TEMPORAL_FILTER_SWITCH %d\n",flag);
    MeasureParamsControl(g_Module, GET_SPATIAL_FILTER_SWITCH, &flag);
    printf("GET_SPATIAL_FILTER_SWITCH %d\n",flag);
    MeasureParamsControl(g_Module, GET_DRT_SWITCH, &flag);
    printf("GET_DRT_SWITCH %d\n",flag);
    MeasureParamsControl(g_Module, GET_BADPOINT_SWITCH, &flag);         
    printf("GET_BADPOINT_SWITCH %d\n",flag);
    MeasureParamsControl(g_Module, GET_HORIZON_STRIPE_SWITCH, &flag);
    printf("GET_HORIZON_STRIPE_SWITCH %d\n",flag);
    MeasureParamsControl(g_Module, GET_VERTICAL_STRIPE_SWITCH, &flag);
    printf("GET_VERTICAL_STRIPE_SWITCH %d\n",flag);

    if(g_Module)
        return 0;
    return -1;
}

int guide_init()
{
    GUIDE_MODEL_SDK_Init();
    return 0;
}

void guide_release()
{
    if(g_Module){
        DestroyModuleInstance(g_Module);
        g_Module = 0;
        g_YuvCallbackFunc = NULL;
    }
}

int guide_push_raw_data(unsigned char * pData, int dataLen)
{
    if (g_DataFunc) {
        //dvp source 512*200  delele head 512*4 tail 512*4
        g_DataFunc(pData+512*4, 512*192, g_UserData);
	}
    return 0;
}

void guide_set_yuv_callback(YuvImageCallBack func)
{
    g_YuvCallbackFunc = func;
}

void guide_set_y16_callback(Y16DataCallBack func)
{
	g_Y16CallbackFunc = func;
}

void save_y16_data(int count)
{
    if(save_y16_data_count==0){
        save_y16_data_count = count;
    }
}
