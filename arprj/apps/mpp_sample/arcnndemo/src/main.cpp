#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

#include "arcnn_base.h"
#include "arcnn_common.h"
#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "mpi_sysctl.h"

MPI_SYSCTL_HANDLE g_phandle = NULL;

extern "C" AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer);
extern "C" AR_S32 DumpIFCOutput(void * handle, void * pIfcOutput);

static AR_U32 gu32QuitFlag = 0;

void PrintUsage(char * name)
{
    printf("%s Usage:\n", name);
    printf("%s [json cfg file name] [option] [start_layer] [end_layer]\n", name);
    printf("option: can be [prof] or [debug] or [debugifc] or [FastBoot].\n");
    printf("        prof: enable profiling.\n");
    printf("        debug: debug mode, NPU will stop on each layer completion.\n");
	printf("        FastBoot: test for FastBoot, NPU will suspend with test_sysctl.\n");
    printf("start_layer: Used in debug mode.\n");
    printf("             NPU will dump the cbuf and runtime starting from this layer.\n");
    printf("             If you dont input this value, NPU will dump from layer 0.\n");
    printf("end_layer: Used in debug mode.\n");
    printf("           NPU will dump the cbuf and runtime stopping at this layer");
    printf("           If you dont input this value, NPU will dump till the last layer.\n");
}

AR_S32 s32NpuDone = 0;

void AR_SAMPLE_fast_boot_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    int flag = -1;
    AR_U64 u64Tic = 0;
    struct timeval tm_start;
    struct timeval tm_end;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
        SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        //while(!s32NpuDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_NPU_Suspend();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("npu suspend failed, ret:%d!\n", ret);
        else
            printf("npu suspend success!\n");
        
        flag = 1;
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
    SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
        //while(!s32NpuDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_NPU_Resume();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("npu resume failed, ret:%d!\n", ret);
        else
            printf("npu resume success!\n");
        
        flag = 0;
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");
    
    if(-1 != flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }

}


void SignalHandler(int sig)
{
    printf("Get signal %d\r\n", sig);

    gu32QuitFlag = 1;
}

int main(int argc, char * argv[])
{
	AR_MPI_NPU_SetLogLevel(0);
   
    AR_U32  u32RunLoop = 1;
	AR_U32  u32InputNum =0;
	AR_U32  u32BatchNum =0;
	AR_U32  u32InputId = 0;
	AR_U32  u32BatchId = 0;
    AR_S32 s32Ret = 0;
    AR_S32 s32FastBoot = 0;
    AR_BOOL bInstant = AR_TRUE; //blocked call
    AR_BOOL bDebug = AR_FALSE;
	AR_IMG_SET_S *stImg = (AR_IMG_SET_S *)malloc(sizeof(AR_IMG_SET_S));

    //get json file from command line
    if(argc < 2)
    {
        PrintUsage(argv[0]);
        return -1;
    }

    CARCNN MyNet(argv[1]);


    if(!MyNet.IsInitialized())
    {
        return -1;
    }

    if(MyNet.IsCbEnabled())
	{
		// cb parser func
		/*************before ArtStudio-v0.8 caffe framework************************/
	   	MyNet.RegisterParseFunc((AR_CHAR *)"lrn", AR_CNN_ParseLrn);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"maxunpool", AR_CNN_ParseMaxUnpool);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"upsample", AR_CNN_ParseUpsample);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"deformable", AR_CNN_ParseDeformable);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"sevcallback", AR_CNN_ParseSevCallback);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"pad", AR_CNN_ParsePad);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"permute", AR_CNN_ParsePermute);
		/**************************************************************************/
		/*all ArtStudio version onnx framework, ArtStudio-v0.8 and after Art.Studio-v0.8 caffe framework*/
	   	MyNet.RegisterParseFunc((AR_CHAR *)"LRN", AR_CNN_ParseOnnxLrn);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"Pad", AR_CNN_ParsePad);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"Transpose", AR_CNN_ParseTranspose);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"Concat", AR_CNN_ParseConcat);
		MyNet.RegisterParseFunc((AR_CHAR *)"Resize", AR_CNN_ParseResize);
		/************************************************************************************************/
	   	/*******************customer-defined operator**************************************/
	   	MyNet.RegisterParseFunc((AR_CHAR *)"art_add", AR_CNN_ParseCustomAdd);
	   	MyNet.RegisterParseFunc((AR_CHAR *)"Custom-LRN", AR_CNN_ParseCustomLrn);
		/**********************************************************************************/
		/******************MatMul convert Convolution insert operator**********************/
	    MyNet.RegisterParseFunc((AR_CHAR *)"feature_to_weight", AR_CNN_ParseFeatureToWeight);
		/**********************************************************************************/

		//cb inference func
	    /********************before ArtStudio-v0.8 caffe framework****************************/
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"lrn", AR_CNN_OperatorLrn);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"maxunpool", AR_CNN_OperatorMaxUnPool);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"reshape", AR_CNN_OperatorReshape);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"upsample", AR_CNN_OperatorUpsample);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"reciprocal", AR_CNN_OperatorReciprocal);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"deformable", AR_CNN_OperatorDeformable);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"sevcallback", AR_CNN_OperatorSevCallback);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"pad", AR_CNN_OperatorPad);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"permute", AR_CNN_OperatorPermute);
		/**************************************************************************************/
		/*all ArtStudio version onnx framework, ArtStudio-v0.8 and after Art.Studio-v0.8 caffe framework*/
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"LRN", AR_CNN_OperatorOnnxLrn);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"Reshape", AR_CNN_OperatorOnnxReshape);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"Pad", AR_CNN_OperatorPad);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"Transpose", AR_CNN_OperatorTranspose);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"Concat", AR_CNN_OperatorConcat);
		MyNet.RegisterCallbackFunc((AR_CHAR *)"Resize", AR_CNN_OperatorResize);
		/************************************************************************************************/
		/***************************customer-defined operator****************************/
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"Custom-LRN", AR_CNN_OperatorCustomLrn);
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"art_add", AR_CNN_OperatorCustomAdd);
		/********************************************************************************/
	    /************************MatMul convert Convolution insert operator***********************/
	    MyNet.RegisterCallbackFunc((AR_CHAR *)"feature_to_weight", AR_CNN_OperatorFeatureToWeight);
		/*****************************************************************************************/
    }


	//AR_U32 au32CBIds[1]={0};
	//AR_U32 u32CBNum = 1;
	//MyNet.SetDSPCallbackMask(au32CBIds, u32CBNum);
	//MyNet.RegisterDSPCallback();

    MyNet.DisableNPUDebug();
    MyNet.DisbleProfile();

    //Malloc NPU input/output space
    MyNet.MallocNPUBuff();
    MyNet.MallocRuntimeBuff();

    if(argc >= 3)
    {
        if(0 == strcmp(argv[2], "prof"))
        {
            MyNet.EnableProfile();
        }

        if(0 == strcmp(argv[2], "debug"))
        {
            AR_U32 u32Start = 0, u32End = 0;
            if(argc >= 4)
            {
                u32Start = atoi(argv[3]);
            }
            if(argc >= 5)
            {
                u32End = atoi(argv[4]);
            }

            MyNet.RegisterIFCDebugFunc(DumpIFCOutput);
            MyNet.RegisterLayerDebugFunc(DumpLayerData);
            MyNet.EnableNPUDebug(u32Start, u32End);
            bDebug = AR_TRUE;
        }
        if(0 == strcmp(argv[2], "debugifc"))
        {
            MyNet.RegisterIFCDebugFunc(DumpIFCOutput);
        }

		if(0 == strcmp(argv[2], "loop"))
        {
			if(argc >= 4)
		    {
			    u32RunLoop = atoi(argv[3]);
		    }
			MyNet.EnableProfile();
        }

    if(0 == strcmp(argv[2], "FastBoot"))
    {
        s32FastBoot = 1;
    }
    /*** register in app to sysctl ***/
    if(s32FastBoot){
        g_phandle = AR_MPI_SYSCTL_Register((AR_CHAR *)argv[0], 0, &AR_SAMPLE_fast_boot_func);
        if(!g_phandle){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("register sysctl %s success!\n", argv[0]);
    }

    }

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);

    if(MyNet.GetMode() == ARCNN_INPUT_LIVE)
    {
        cout << "Live mode is only to show how the code is organized, not supported in this sample." << endl;
        return 0;

        s32Ret = MyNet.BindVideoDev();
        if(s32Ret)
        {
            MyNet.FreeNPUBuff();
            return -1;
        }

        while(1)
        {
            if(gu32QuitFlag)
                break;

            //get live img for single-input/single-batch
			memset(stImg,0,sizeof(AR_IMG_SET_S));

            s32Ret = MyNet.GetLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
            if(s32Ret)
            {
                MyNet.DeBindVideoDev();
				free(stImg);
                MyNet.FreeNPUBuff();
                return -1;
            }
			stImg->astInputImg[0].bPreIfcProcess=AR_TRUE;

            s32Ret = MyNet.Forward(stImg, bInstant, bDebug);
            if(s32Ret)
            {
                MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
                MyNet.DeBindVideoDev();
				free(stImg);
                MyNet.FreeNPUBuff();
                return -1;
            }
            MyNet.PostProcess(NULL);
			MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
        }

        cout << "Live test finished." << endl;
        MyNet.DeBindVideoDev();

    }
    else
    {
        //Get dir and search for pics
        DIR * dirp;
        struct dirent * file;
        string strDirPath = MyNet.GetLocalTestDir();
        dirp = opendir(strDirPath.c_str());
        if(!dirp)
        {
            cout << "Error open local test path" << endl;
            MyNet.FreeNPUBuff();
            return -1;
        }
		u32InputNum = MyNet.GetInputNum();
		u32BatchNum = MyNet.GetBatchNum();
        printf("NetWork InputNum=%d BatchNum=%d\r\n",u32InputNum,u32BatchNum);
        while(1)
        {
            if(gu32QuitFlag)
                break;

            //get local img for multi-input/multi-batch
			memset(stImg,0,sizeof(AR_IMG_SET_S));

			u32InputId = 0;
			u32BatchId = 0;
			if(MyNet.IsIFCEnabled())
			{
				while((file = readdir(dirp)) != NULL)
				{
					if(gu32QuitFlag)
						break;

					char fullname[128] = {0};
					strcpy(fullname, strDirPath.c_str());
					strcat(fullname, "/");
					strcat(fullname, file->d_name);


					stImg->astInputImg[u32InputId].bPreIfcProcess = AR_TRUE;
					s32Ret = MyNet.GetLocalImage(fullname, &stImg->astInputImg[u32InputId].astBatchImg[u32BatchId],u32InputId);
					if(s32Ret)
					{
						continue;
					}

					u32BatchId++;
					if(u32BatchId == u32BatchNum)
					{
						u32BatchId =0;
						u32InputId++;
					}
					
					if(u32InputId == u32InputNum)
					{
						break;
					}
				}

				if(u32InputId != u32InputNum)
				{
					printf("Get %d Imgs, not Enough Exit!\r\n",u32InputId);
					goto exit_handler;
				}

                if(s32FastBoot){
                    while(!gu32QuitFlag){
                        //usleep(1000*1000);
                        //s32NpuDone = 0;
                        s32Ret = MyNet.Forward(NULL, bInstant, bDebug);
                        //s32NpuDone = 1;
                        if(s32Ret != 0){
                            printf("npu Forward error!\n");
                        }
                    }
                }
                else{
                    for(int x=0;x<u32RunLoop;x++)
                    {
                        s32Ret = MyNet.Forward(stImg, bInstant, bDebug);
                    }
                    MyNet.GetNetPerf(u32RunLoop);
                }
                MyNet.ReleaseLocalImage(stImg);
			}
			 else
			 {
			 	while((file = readdir(dirp)) != NULL)
				{
					if(gu32QuitFlag)
						break;

					char fullname[128] = {0};
					strcpy(fullname, strDirPath.c_str());
					strcat(fullname, "/");
					strcat(fullname, file->d_name);
				 	stImg->astInputImg[u32InputId].bPreIfcProcess = AR_FALSE;
					s32Ret = MyNet.FillInputBuff(fullname, u32InputId, u32BatchId);
					if(s32Ret)
					{
						continue;
					}

					u32InputId++;
					if(u32InputId == u32InputNum)
					{
						break;
					}
			 	}

				if(u32InputId != u32InputNum)
				{
					printf("Get %d Imgs, not Enough Exit!\r\n",u32InputId);
					goto exit_handler;
				}

				
				for(int x=0;x<u32RunLoop;x++)
                {
					s32Ret = MyNet.Forward(NULL, bInstant, bDebug);
				}
				MyNet.GetNetPerf(u32RunLoop);
			 }
			 if(s32Ret)
			 {
			 	 printf("Forward ret = %d\n", s32Ret);
				 break;
			 }
			 MyNet.PostProcess(file->d_name);
    	}
	cout << "Local image test finished." << endl;
   }

exit_handler:

    /*** unregister in app to sysctl ***/
    if(s32FastBoot){
        s32Ret = AR_MPI_SYSCTL_Unregister(g_phandle);
        if(s32Ret < 0){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("unregister sysctl %s success!\n", argv[0]);
    }

    MyNet.ReleaseLocalImage(stImg);

	free(stImg);
	printf("exit_handler finished\r\n");

	while(0)
	{
	    sleep(1);
	    if(gu32QuitFlag)
	        break;
	}
	printf("AppEnd\r\n");
	MyNet.FreeNPUBuff();

   	MyNet.FreeRuntimeBuff();
	
	//MyNet.UnRegisterDSPCallback();
	return 0;
}
