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


extern "C" AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer);
extern "C" AR_S32 DumpIFCOutput(void * handle, void * pIfcOutput);


static AR_U32 gu32QuitFlag = 0;
void SignalHandler(int sig)
{
    printf("Get signal %d\r\n", sig);
    gu32QuitFlag = 1;
}

void PrintUsage(char * name)
{
    printf("%s Usage:\n", name);
    printf("%s [json cfg file name] [option] [start_layer] [end_layer]\n", name);
    printf("option: can be [prof] or [debug] or [debugifc].\n");
    printf("        prof: enable profiling.\n");
    printf("        debug: debug mode, NPU will stop on each layer completion.\n");
    printf("start_layer: Used in debug mode.\n");
    printf("             NPU will dump the cbuf and runtime starting from this layer.\n");
    printf("             If you dont input this value, NPU will dump from layer 0.\n");
    printf("end_layer: Used in debug mode.\n");
    printf("           NPU will dump the cbuf and runtime stopping at this layer");
    printf("           If you dont input this value, NPU will dump till the last layer.\n");
}

int main(int argc, char * argv[])
{
	AR_S32	s32Ret = 0;
	AR_U32  u32InputId = 0;
	AR_U32  u32BatchId = 0;
    AR_BOOL bInstant = AR_TRUE;
    AR_BOOL bDebug = AR_FALSE;


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
	   MyNet.RegisterParseFunc((AR_CHAR *)"lrn", AR_CNN_ParseLrn);
	   MyNet.RegisterParseFunc((AR_CHAR *)"LRN", AR_CNN_ParseOnnxLrn);
	   MyNet.RegisterParseFunc((AR_CHAR *)"maxunpool", AR_CNN_ParseMaxUnpool);
	   MyNet.RegisterParseFunc((AR_CHAR *)"upsample", AR_CNN_ParseUpsample);
	   MyNet.RegisterParseFunc((AR_CHAR *)"deformable", AR_CNN_ParseDeformable);
	   MyNet.RegisterParseFunc((AR_CHAR *)"sevcallback", AR_CNN_ParseSevCallback);
	   MyNet.RegisterParseFunc((AR_CHAR *)"feature_to_weight", AR_CNN_ParseFeatureToWeight);
	   MyNet.RegisterParseFunc((AR_CHAR *)"pad", AR_CNN_ParsePad);
	   MyNet.RegisterParseFunc((AR_CHAR *)"permute", AR_CNN_ParsePermute);
	   MyNet.RegisterParseFunc((AR_CHAR *)"Transpose", AR_CNN_ParseTranspose);
	   MyNet.RegisterParseFunc((AR_CHAR *)"Concat", AR_CNN_ParseConcat);
	   MyNet.RegisterParseFunc((AR_CHAR *)"art_add", AR_CNN_ParseCustomAdd);
	   //usr parser json func
	   MyNet.RegisterParseFunc((AR_CHAR *)"Custom-LRN", AR_CNN_ParseCustomLrn);

	   //cb inference func
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"lrn", AR_CNN_OperatorLrn);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"LRN", AR_CNN_OperatorOnnxLrn);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"getconfidence", AR_CNN_OperatorGetConf);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"maxunpool", AR_CNN_OperatorMaxUnPool);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"reshape", AR_CNN_OperatorReshape);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"Reshape", AR_CNN_OperatorOnnxReshape);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"upsample", AR_CNN_OperatorUpsample);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"reciprocal", AR_CNN_OperatorReciprocal);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"deformable", AR_CNN_OperatorDeformable);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"sevcallback", AR_CNN_OperatorSevCallback);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"feature_to_weight", AR_CNN_OperatorFeatureToWeight);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"pad", AR_CNN_OperatorPad);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"permute", AR_CNN_OperatorPermute);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"Transpose", AR_CNN_OperatorTranspose);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"Concat", AR_CNN_OperatorConcat);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"art_add", AR_CNN_OperatorCustomAdd);
	   // usr-cb inference func
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"Custom-LRN", AR_CNN_OperatorCustomLrn);
    }

    MyNet.DisableNPUDebug();
    MyNet.DisbleProfile();
    MyNet.MallocNPUBuff();

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
    }

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);


    DIR * dirp;
    struct dirent * file;
    string strDirPath = MyNet.GetLocalTestDir();
    dirp = opendir(strDirPath.c_str());
    if(!dirp)
    {
        printf("Error open local test path \r\n");
        MyNet.FreeNPUBuff();
        return -1;
    }

	AR_U32 u32InputNum = MyNet.GetInputNum();
	AR_U32 u32BatchNum = MyNet.GetBatchNum();
    printf("NetWork InputNum=%d BatchNum=%d\r\n",u32InputNum,u32BatchNum);
	if(!u32BatchNum)
	{
		u32BatchNum = 1;
	}
	AR_IMG_SET_S *stImg = (AR_IMG_SET_S *)malloc(sizeof(AR_IMG_SET_S));
	if(!stImg)
	{
	    printf("malloc stImg error \r\n");
		MyNet.FreeNPUBuff();
		return -1;
	}
    while(1)
    {
        if(gu32QuitFlag)
            break;

		memset(stImg,0,sizeof(AR_IMG_SET_S));
		u32InputId = 0;

		while((file = readdir(dirp)) != NULL)
		{
			if(gu32QuitFlag)
				break;

			char fullname[128] = {0};
			strcpy(fullname, strDirPath.c_str());
			strcat(fullname, "/");
			strcat(fullname, file->d_name);

			s32Ret = MyNet.GetLocalInput(fullname, stImg, 0);
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
			printf("Get %d files, not match input tensor num, exit!\r\n",u32InputId);
			MyNet.ReleaseLocalInput(stImg);
            break;
		}

		s32Ret = MyNet.Forward(stImg, bInstant, bDebug);

		MyNet.ReleaseLocalInput(stImg);

		if(s32Ret)
		{
		 	 printf("Forward ret = %d\n", s32Ret);
			 break;
		}
		MyNet.PostProcess(NULL);
	}

	free(stImg);
	MyNet.FreeNPUBuff();
    printf("Local image test finished.\r\n");
	return 0;
}
