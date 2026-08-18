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
#include "ar_cnn_operator.h"
#include "arcnn_op_common.h"

extern "C" AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer);
extern "C" AR_S32 DumpIFCOutput(void * handle, void * pIfcOutput);

static AR_U32 gu32QuitFlag = 0;

void PrintUsage(char * name)
{
    printf("%s Usage:\n", name);
    printf("%s [json cfg file name] [json cfg2 file name] [option] [start_layer] [end_layer]\n", name);
    printf("option: can be [prof] or [debug] or [debugifc].\n");
    printf("        prof: enable profiling.\n");
    printf("        debug: debug mode, NPU will stop on each layer completion.\n");
    printf("start_layer: Used in debug mode.\n");
    printf("             NPU will dump the cbuf and runtime starting from this layer.\n");
    printf("             If you dont input this value, NPU will dump from layer 0.\n");
    printf("end_layer: Used in debug mode.\n");
    printf("           NPU will dump the cbuf and runtime stopping at this layer");
    printf("           If you dont input this value, NPU will dump till the last layer.\n");
    exit(0);
}

void SignalHandler(int sig)
{
    printf("Get signal %d\r\n", sig);

    gu32QuitFlag = 1;
}

int main(int argc, char * argv[])
{
	AR_U32  u32IFCInputNum =0;
	AR_U32  u32IFCBatchNum =0;
	AR_U32  u32IFCInputId = 0;
	AR_U32  u32IFCBatchId = 0;
	void * handle = NULL;
    void * handle2 = NULL;
    AR_S32 s32Ret = 0;
    AR_S32 u32ArgOptionIdx = 2;
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
    CARCNN *MyNet1 = NULL;

    if(argc > 2 && strstr(argv[2],".json")){
        MyNet1 = new CARCNN(argv[2]);
        u32ArgOptionIdx = 3;
    }

    if(!MyNet.IsInitialized()){
        delete MyNet1;
        return -1;
    }
    if(MyNet1 && !MyNet1->IsInitialized()){
        delete MyNet1;
        return -1;
    }


    //MyNet.RegisterCallbackFunc(ArCallbackEntry);
#if 0
    MyNet.RegisterParseFunc("lrn", AR_CNN_ParseLrn);
	MyNet.RegisterParseFunc("maxunpool", AR_CNN_ParseMaxUnpool);
	MyNet.RegisterParseFunc("upsample", AR_CNN_ParseUpsample);
	MyNet.RegisterParseFunc("deformable", AR_CNN_ParseDeformable);
	MyNet.RegisterParseFunc("sevcallback", AR_CNN_ParseSevCallback);
    //usr parser json func
    MyNet.RegisterParseFunc("customLRN", AR_CNN_ParseCustomLrn);

    //cb inference func
    MyNet.RegisterCallbackFunc("lrn", AR_CNN_OperatorLrn);
    MyNet.RegisterCallbackFunc("getconfidence", AR_CNN_OperatorGetConf);
    MyNet.RegisterCallbackFunc("maxunpool", AR_CNN_OperatorMaxUnPool);
    MyNet.RegisterCallbackFunc("reshape", AR_CNN_OperatorReshape);
    MyNet.RegisterCallbackFunc("upsample", AR_CNN_OperatorUpsample);
    MyNet.RegisterCallbackFunc("reciprocal", AR_CNN_OperatorReciprocal);
    MyNet.RegisterCallbackFunc("deformable", AR_CNN_OperatorDeformable);
    MyNet.RegisterCallbackFunc("sevcallback", AR_CNN_OperatorSevCallback);
	// usr-cb inference func
	MyNet.RegisterCallbackFunc("customLRN", AR_CNN_OperatorCustomLrn);
	
    if(MyNet1){
	    MyNet1->RegisterParseFunc("lrn", AR_CNN_ParseLrn);
		MyNet1->RegisterParseFunc("maxunpool", AR_CNN_ParseMaxUnpool);
		MyNet1->RegisterParseFunc("upsample", AR_CNN_ParseUpsample);
		MyNet1->RegisterParseFunc("deformable", AR_CNN_ParseDeformable);
		MyNet1->RegisterParseFunc("sevcallback", AR_CNN_ParseSevCallback);
	    //usr parser json func
	    MyNet1->RegisterParseFunc("customLRN", AR_CNN_ParseCustomLrn);

	    //cb inference func
	    MyNet1->RegisterCallbackFunc("lrn", AR_CNN_OperatorLrn);
	    MyNet1->RegisterCallbackFunc("getconfidence", AR_CNN_OperatorGetConf);
	    MyNet1->RegisterCallbackFunc("maxunpool", AR_CNN_OperatorMaxUnPool);
	    MyNet1->RegisterCallbackFunc("reshape", AR_CNN_OperatorReshape);
	    MyNet1->RegisterCallbackFunc("upsample", AR_CNN_OperatorUpsample);
	    MyNet1->RegisterCallbackFunc("reciprocal", AR_CNN_OperatorReciprocal);
	    MyNet1->RegisterCallbackFunc("deformable", AR_CNN_OperatorDeformable);
	    MyNet1->RegisterCallbackFunc("sevcallback", AR_CNN_OperatorSevCallback);
		// usr-cb inference func
		MyNet1->RegisterCallbackFunc("customLRN", AR_CNN_OperatorCustomLrn);
    }
#endif

    MyNet.DisableNPUDebug();
    MyNet.DisbleProfile();
    if(MyNet1){
        MyNet1->DisableNPUDebug();
        MyNet1->DisbleProfile();
    }
	MyNet.MallocNPUBuff();
	if(MyNet1){
		MyNet1->MallocNPUBuff();
	}

    if(argc >= u32ArgOptionIdx+1)
    {
        if(0 == strcmp(argv[u32ArgOptionIdx+0], "prof"))
        {
            MyNet.EnableProfile();
            if(MyNet1){
                MyNet1->EnableProfile();
            }
        }

        if(0 == strcmp(argv[u32ArgOptionIdx+0], "debug"))
        {
            AR_U32 u32Start = 0, u32End = 0;
            if(argc >= u32ArgOptionIdx+2)
            {
                u32Start = atoi(argv[u32ArgOptionIdx+1]);
            }
            if(argc >= u32ArgOptionIdx+3)
            {
                u32End = atoi(argv[u32ArgOptionIdx+2]);
            }

            MyNet.RegisterIFCDebugFunc(DumpIFCOutput);
            MyNet.RegisterLayerDebugFunc(DumpLayerData);
            MyNet.EnableNPUDebug(u32Start, u32End);
            bDebug = AR_TRUE;
        }
        if(0 == strcmp(argv[u32ArgOptionIdx+0], "debugifc"))
        {
            MyNet.RegisterIFCDebugFunc(DumpIFCOutput);
            if(MyNet1){
                MyNet1->RegisterIFCDebugFunc(DumpIFCOutput);
            }
        }
    }

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);

    if(MyNet.GetMode() == ARCNN_INPUT_LIVE)
    {
        s32Ret = MyNet.BindVideoDev();
        if(s32Ret)
        {
			MyNet.FreeNPUBuff();
			if(MyNet1){
				MyNet1->FreeNPUBuff();
			}
            return -1;
        }

        while(1)
        {
            if(gu32QuitFlag)
                break;

            //get live img for single-input/single-batch
			memset(stImg,0,sizeof(AR_IMG_SET_S));
            stImg->astInputImg[0].bPreIfcProcess = AR_TRUE;
            s32Ret = MyNet.GetLiveImage(&stImg->astInputImg[0].astBatchImg[0]);

            if(s32Ret)
            {
                MyNet.DeBindVideoDev();
				free(stImg);
				MyNet.FreeNPUBuff();
				if(MyNet1){
					MyNet1->FreeNPUBuff();
				}
                return -1;
            }

            s32Ret = MyNet.Forward(stImg, bInstant, bDebug);
            if(s32Ret)
            {
                MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
                MyNet.DeBindVideoDev();
				free(stImg);
				MyNet.FreeNPUBuff();
				if(MyNet1){
					MyNet1->FreeNPUBuff();
				}
                return -1;
            }

            MyNet.PostProcess(NULL);
            if(MyNet1){
                s32Ret = MyNet1->Forward(stImg, bInstant, bDebug);
                if(s32Ret)
                {
                    MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
                    MyNet.DeBindVideoDev();
                    free(stImg);
					MyNet.FreeNPUBuff();
					if(MyNet1){
						MyNet1->FreeNPUBuff();
					}
                    return -1;
                }
                MyNet1->PostProcess(NULL);
            }
            s32Ret = MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
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
			if(MyNet1){
				MyNet1->FreeNPUBuff();
			}
            return -1;
        }
		u32IFCInputNum = MyNet.GetInputNum();;
		u32IFCBatchNum = MyNet.GetBatchNum();
        printf("NetWork : u32IFCInputNum=%d u32IFCBatchNum=%d \n", u32IFCInputNum,u32IFCBatchNum);
        while(1)
        {
            if(gu32QuitFlag)
                break;

            //get local img for multi-input/multi-batch
			memset(stImg,0,sizeof(AR_IMG_SET_S));

			u32IFCInputId = 0;
			u32IFCBatchId = 0;
			while((file = readdir(dirp)) != NULL)
			{
			     if(gu32QuitFlag)
	                break;

                 char fullname[128] = {0};
				 strcpy(fullname, strDirPath.c_str());
				 strcat(fullname, "/");
				 strcat(fullname, file->d_name);

				 if(MyNet.IsIFCEnabled())
				 {
				    stImg->astInputImg[u32IFCInputId].bPreIfcProcess = AR_TRUE;
					s32Ret = MyNet.GetLocalImage(fullname, &stImg->astInputImg[u32IFCInputId].astBatchImg[u32IFCBatchId], u32IFCInputId);
					if(s32Ret)
					{
						continue;
					}
				 }
				 else
				 {
				 	stImg->astInputImg[u32IFCInputId].bPreIfcProcess = AR_FALSE;
					s32Ret = MyNet.FillInputBuff(fullname,u32IFCInputId,u32IFCBatchId);
					if(s32Ret)
					{
						continue;
					}
				 }
				 u32IFCBatchId++;
				 if(u32IFCBatchId == u32IFCBatchNum)
				 {
				 	 u32IFCBatchId =0;
                     u32IFCInputId++;
				 }
				 if(u32IFCInputId == u32IFCInputNum)
				 {
					 break;
				 }
			}

			if(u32IFCInputId != u32IFCInputNum)
			{
				printf("Get %d Imgs, not Enough Exit!\r\n",u32IFCInputId);
				goto exit_handler;
			}

			s32Ret = MyNet.Forward(stImg, bInstant, bDebug);

            if(MyNet1){
                s32Ret |= MyNet1->Forward(stImg, bInstant, bDebug);
            }

			MyNet.ReleaseLocalImage(stImg);
            if(s32Ret)
            {
                break;
            }

            MyNet.PostProcess(file->d_name);
            if(MyNet1){
                MyNet1->PostProcess(file->d_name);
            }
        }
        cout << "Local image test finished." << endl;
    }

	exit_handler:
		MyNet.ReleaseLocalImage(stImg);
		free(stImg);
		printf("exit_handler finished\r\n");
		MyNet.FreeNPUBuff();
        if(MyNet1){
			MyNet1->FreeNPUBuff();
            delete MyNet1;
        }

    while(0)
    {
        sleep(1);
        if(gu32QuitFlag)
            break;
    }
	printf("AppEnd\r\n");

    return 0;
}
