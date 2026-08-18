#include "guide_module_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define Y16_DEBUG 0

void* g_UserData = NULL;
YuvImageCallBack g_YuvCallbackFunc = NULL;
Y16DataCallBack g_Y16CallbackFunc = NULL;

int save_y16_data_count = 0;

static HANDLE_T instance;
static ITARegistry registry;

static int guide_ita_sdk_init(int w, int h)
{
    gpio_spi_init();
	//ITARegistry registry;
	ITA_RESULT result;
	ITAImgInfo pInfo;
	//ITAISPResult pResult;
	ITA_SWITCH itaParam = ITA_DISABLE;

	int iparam;
	bool bparam;

	/*初始化ITA*/
	instance = ITA_Init(ITA_X16, ITA_INDUSTRY_LOW, w, h, 1);
	if (0 == instance)
	{
	    printf("ita init failed\n");
		return -1;
	}
	ITA_Debugger(instance, ITA_DEBUGGING_PATH, (void *)"/tmp/ita_debug.log");
	/*打开调试。一般情况下关闭调试开关。*/
	ITA_Debugger(instance, ITA_SET_DEBUGGING, &itaParam);
	/*注册日志和HAL函数，以及其它自定义函数。*/
	memset(&registry, 0, sizeof(ITARegistry));
	//registry.OutputLog = myPrintf;
	registry.ShutterControl = arShutterControl;
	registry.ReadPackageData = arReadPackageData;
	registry.WritePackageData = arWritePackageData;
	//registry.ReadConfig = readConfig;
	registry.SPITransmit = arSPITransmit;
	registry.SPIWrite = arSPIWrite;
	registry.GetSensorTemp = arGetSensorTemp;
	result = ITA_Register(instance, &registry);

    /*时序参数     glfeng@20220620*/
    #if 0
    iparam = 1;
	result = ITA_DetectorControl(instance, ITA_DETECTOR_VALID_MODE, &iparam);
	if(result != ITA_OK) printf("call gd sdk error: ITA_DETECTOR_VALID_MODE \n");
	//iparam = 1;
	//result = ITA_DetectorControl(instance, ITA_DETECTOR_DATA_MODE, &iparam);
	//if(result != ITA_OK) printf("call gd sdk error: ITA_DETECTOR_DATA_MODE \n");
	//iparam = 25;
	//result = ITA_DetectorControl(instance, ITA_DETECTOR_FRAME_FREQUENCY, &iparam);
	//if(result != ITA_OK) printf("call gd sdk error: ITA_DETECTOR_FRAME_FREQUENCY \n");
	#endif
	result = ITA_ConfigureDetector(instance, 5000000);
	if (result < ITA_OK)
	{
	    printf("ita failed 0\n");
	    return -1;
		//异常处理，探测器未配置成功。
	}
	/*设置图像缩放倍数，默认1.0倍。*/
	float scaleTimes = 1.0;
	ITA_ImageControl(instance, ITA_SET_SCALE, (void *)&scaleTimes);
	/*设置图像输出格式，默认ITA_RGB888。*/
	ITA_PIXEL_FORMAT format = ITA_RGB888;
	ITA_ImageControl(instance, ITA_SET_FORMAT, (void *)&format);

	/*成像参数     glfeng@20220620*/
	bparam = false;
	result = ITA_ImageControl(instance, ITA_TIME_FILTER, &bparam);
	result = ITA_ImageControl(instance, ITA_SPATIAL_FILTER, &bparam);
	result = ITA_ImageControl(instance, ITA_REMOVE_VERTICAL, &bparam);
	result = ITA_ImageControl(instance, ITA_REMOVE_HORIZONTAL, &bparam);
	result = ITA_ImageControl(instance, ITA_SHARPENING, &bparam);
	result = ITA_ImageControl(instance, ITA_DETAIL_ENHANCE, &bparam);
	result = ITA_ImageControl(instance, ITA_BLOCK_HISTOGRAM, &bparam);
	result = ITA_ImageControl(instance, ITA_GAMMA_CORRECTION, &bparam);
	result = ITA_ImageControl(instance, ITA_Y8_ADJUSTBC, &bparam);
	result = ITA_ImageControl(instance, ITA_DRT_Y8, &bparam);
	
	/*获取成像和测温参数信息*/
	ITAParamInfo info;
	ITA_GetParamInfo(instance, &info);
	/*调用ITA_PREPARE获取图像信息*/
	memset(&pInfo, 0, sizeof(ITAImgInfo));
	result = ITA_Prepare(instance, &pInfo);

	/*获取版本信息包括伪彩号范围。*/
	ITAVersionInfo pVesion;
	memset(&pVesion, 0, sizeof(ITAVersionInfo));
	ITA_Version(instance, &pVesion);
	printf("ita sdk version %s %d %d\n", pVesion.version, pVesion.totalNumberOfPalettes, pVesion.totalNumberOfGears);
	//设置图像伪彩，伪彩号要小于版本信息中的totalNumberOfPalettes。
	ITA_PALETTE_TYPE paletteIndex = ITA_IRON_RED;
	result = ITA_ImageControl(instance, ITA_SET_PALETTE, (void *)&paletteIndex);
	if (result < ITA_OK)
	{
	    printf("ita failed 1\n");
	    return -1;
		//异常处理，未设置成功。
	}

    gpio_spi_uninit();
	arAdcTempThreadCtl(1);
	return 0;
}

int guide_init(int w, int h)
{
    return guide_ita_sdk_init(w, h);
}

void guide_release()
{
    if(instance){
        ITA_Uninit(instance);
        instance = 0;
        arAdcTempThreadCtl(0);
    }
}

int guide_push_raw_data(unsigned char *srcData, int srcSize, ITAISPResult *pResult)
{
    ITA_RESULT ret;
    ret = ITA_ISP(instance, srcData, srcSize, pResult);
    if(ret!=ITA_OK)
    {
        return ret;
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
            int r = write(y16_fd, (unsigned char *)pResult->y16Data, pResult->info.y16Len*2);
            printf("write file, y16len=%d wlen=%d index=%d\n",pResult->info.y16Len,r,save_y16_data_count);
        }
        save_y16_data_count--;
    }
    else if(y16_fd>0){
        close(y16_fd);
        y16_fd = 0;
        printf("save y16 data end\n");
    }
#endif

	return ret;
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

int guide_MeasureTempPoint(short y16, float *surfaceTemp)
{
    if(!instance) return 0;
    return ITA_MeasureTempPoint(instance, y16, surfaceTemp);
}

unsigned long long guide_get_instance()
{
    return instance;
}

