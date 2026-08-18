#include "ModuleSDK.h"
#include <string>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

HANDLE_T  g_Module = 0;

//处理红外原始数据，通过原始数据针对任意点，通过y16数据进行测温，
//同时也可以通过原始y16得到最高和最低的y16值，算出最高温和最低温，避免全帧测温计算最高最低温
//此处理可以单独放在独立线程中执行，防止在cbFuncHandle中执行，影响回调函数返回
void handleY16Data(short* pY16Data, const int& iLen, const int& iWidth, const int& iHeight)
{
    if (iLen != iWidth * iHeight)
    {
        std::cout << "invalid data size ,drop it!" << std::endl;
        return;
    }
    //模拟点测温
    int iPosX = 128;
    int iPosY = 96;

    if (pY16Data && g_Module)
    {
        int iPosMeasure = iWidth * iPosY + iPosX;
        if (iPosMeasure < iWidth * iHeight)
        {
            short iPosY16 = pY16Data[iPosMeasure];
            float fSurfaceT = 0.0f, fBodyT = 0.0f, fEnvTemp = 0.0f;
            MeasureParamsControl(g_Module, GET_AMBIENT_TEMP, &fEnvTemp);
            MeasureTempByY16(g_Module, iPosY16, &fSurfaceT);
            MeasureBodyTemp(g_Module, fSurfaceT, fEnvTemp, &fBodyT);
            std::cout << "Y:" << iPosY16 << " surface T:" << fSurfaceT << " body T:" << fBodyT << std::endl;
        }
    }
}

//处理红外实时视频数据，数据格式由SET_FORMAT参数设置决定，用于调用端显示使用，
//同时也可以根据实际情况，在实时画面中绘制自定义信息
//此处理可以单独放在独立线程中执行，避免在在cbFuncHandle中执行，影响回调函数返回
void handleYuvData(unsigned char* pYuvData, const int& iLen, const int& iWidth, const int& iHeight)
{
    //用户可以使用yuv422进行实时画面绘制操作
}

//注册回调红外数据回调函数
int cbFuncHandle(CallBackData cbData, void * param)
{
    static int count = 0;
    count++;
    //实时画面更新操作
    if (cbData.yuvData)
    {
        handleYuvData(cbData.yuvData, cbData.yuvLength, cbData.width, cbData.height);
    }
    //实时y16测温数据更新操作
    if (cbData.y16Data)
    {
        handleY16Data(cbData.y16Data, cbData.y16Length, cbData.width, cbData.height);
    }
    //if (count == 75)
    //{   //录制测温数据和截图
    //    MeasureParamsControl(g_Module, GRAB_FRAME, 0);
    //    MeasureParamsControl(g_Module, SNAPSHOT, 0);
    //}
    return 0;
}

void myPrintf(const char *logInfo)
{
    printf("%s", logInfo);
    return;
}

int main(int argc, char *argv[])
{
    int isDebugging = 1, debugMode = 1;
    int clock = 12000000;
    //12MHZ的配置
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
        { 0x10, 0x07 },
        { 0x16, 0x00 },
        { 0x2f, 0x00 },
        { 0x30, 0x24 },
        { 0x31, 0x02 },
        { 0x33, 0x00 },
        { 0x01, 0x01 },
    };
    //6MHZ的配置
    /*
    DetectorConf pConf[20] = {
    { 0x02, 0x00 },
    { 0x03, 0x01 },
    { 0x04, 0xc8 },
    { 0x05, 0x00 },
    { 0x06, 0x30 },
    { 0x07, 0x00 },
    { 0x08, 0xc2 },
    { 0x09, 0x00 },
    { 0x0a, 0xe0 },
    { 0x0b, 0x00 },
    { 0x0c, 0x90 },
    { 0x0d, 0x01 },
    { 0x0e, 0x00 },
    { 0x10, 0x05 },
    { 0x16, 0x00 },
    { 0x2f, 0x00 },
    { 0x30, 0x3d },
    { 0x31, 0x02 },
    { 0x33, 0x00 } ,
    { 0x01, 0x01 },
    };
    */
    /******************************初始化sdk模块******************************/
    g_Module = CreateModuleInstance(MODE_X16);
    if (!g_Module)
    {
        std::cout << "CreateModuleInstance failed" << std::endl;
        return -1;
    }
    RegisterLogFunc(g_Module, myPrintf);
    MeasureParamsControl(g_Module, DEBUGGING_PATH, (void *)"./debug.log");
    MeasureParamsControl(g_Module, DEBUGGING_MODE, &debugMode);
    MeasureParamsControl(g_Module, SET_DEBUGGING, &isDebugging);
    SetMeasureRange(g_Module, 0); //设置为人体测温档位,
    SetDetectorClock(g_Module, pConf, 20, clock);
    RegisterImgCallBack(g_Module, cbFuncHandle, NULL, 1.0);

    /******************************设置测温参数******************************/
    int flag;
    flag = 1;
    MeasureParamsControl(g_Module, SET_BADPOINT_SWITCH, &flag);         //打开坏点替换
    MeasureParamsControl(g_Module, SET_TEMPORAL_FILTER_SWITCH, &flag);  //打开时域滤波
    MeasureParamsControl(g_Module, SET_SPATIAL_FILTER_SWITCH, &flag);   //打开空域滤波
    MeasureParamsControl(g_Module, SET_DRT_SWITCH, &flag);              //打开调光
    flag = 1;
    MeasureParamsControl(g_Module, SET_HORIZON_STRIPE_SWITCH, &flag);   //关闭去横纹
    MeasureParamsControl(g_Module, SET_VERTICAL_STRIPE_SWITCH, &flag);  //关闭去竖纹

    int format = IMG_FORMAT_RGB888;
    ImageParamsControl(g_Module, SET_FORMAT, &format);

    getchar();
    //程序退出时调用
    if (g_Module)
    {
        DestroyModuleInstance(g_Module);
        g_Module = 0;
    }
    return 0;
}
