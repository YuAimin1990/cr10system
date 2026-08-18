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

static AR_U32 gu32QuitFlag = 0;

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

void SignalHandler(int sig)
{
    printf("Get signal %d\r\n", sig);

    gu32QuitFlag = 1;
}

int main(int argc, char * argv[])
{
    AR_S32 s32Ret = 0;
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
	   MyNet.RegisterParseFunc((AR_CHAR *)"lrn", AR_CNN_ParseLrn);
	   MyNet.RegisterParseFunc((AR_CHAR *)"maxunpool", AR_CNN_ParseMaxUnpool);
	   MyNet.RegisterParseFunc((AR_CHAR *)"upsample", AR_CNN_ParseUpsample);
	   MyNet.RegisterParseFunc((AR_CHAR *)"deformable", AR_CNN_ParseDeformable);
	   MyNet.RegisterParseFunc((AR_CHAR *)"sevcallback", AR_CNN_ParseSevCallback);
	   MyNet.RegisterParseFunc((AR_CHAR *)"feature_to_weight", AR_CNN_ParseFeatureToWeight);
	   //usr parser json func
	   MyNet.RegisterParseFunc((AR_CHAR *)"LRN", AR_CNN_ParseCustomLrn);

	   //cb inference func
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"lrn", AR_CNN_OperatorLrn);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"getconfidence", AR_CNN_OperatorGetConf);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"maxunpool", AR_CNN_OperatorMaxUnPool);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"reshape", AR_CNN_OperatorReshape);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"Reshape", AR_CNN_OperatorOnnxReshape);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"upsample", AR_CNN_OperatorUpsample);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"reciprocal", AR_CNN_OperatorReciprocal);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"deformable", AR_CNN_OperatorDeformable);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"sevcallback", AR_CNN_OperatorSevCallback);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"feature_to_weight", AR_CNN_OperatorFeatureToWeight);
	   // usr-cb inference func
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"LRN", AR_CNN_OperatorCustomLrn);

    }

    MyNet.DisableNPUDebug();
    MyNet.DisbleProfile();

    //Malloc NPU input/output space
    MyNet.MallocNPUBuff();

    MyNet.EnableProfile();

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
	
	if(MyNet.GetMode() == ARCNN_INPUT_LIVE){
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

			memset(stImg,0,sizeof(AR_IMG_SET_S));

            s32Ret = MyNet.GetLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
            if(s32Ret < 0)
            {
                MyNet.DeBindVideoDev();
                MyNet.FreeNPUBuff();
				free(stImg);
                return -1;
            }
			stImg->astInputImg[0].bPreIfcProcess=AR_TRUE;

            s32Ret = MyNet.Forward(stImg, bInstant, bDebug);
            if(s32Ret)
            {
                MyNet.DeBindVideoDev();
                MyNet.FreeNPUBuff();
				free(stImg);
                return -1;
            }
			MyNet.PostProcess();
        }

        cout << "Live test finished." << endl;
        MyNet.DeBindVideoDev();
	}
	else{
	    //Get dir and search for pics
	    DIR * dirp;
		DIR * dirpEx;
	    struct dirent * file;
	    string strDirPath = MyNet.GetLocalTestDir();
	    dirp = opendir(strDirPath.c_str());
	    if(!dirp)
	    {
	        cout << "Error open local test path" << endl;
	        MyNet.FreeNPUBuff();
	        return -1;
	    }

	    //get local img
		memset(stImg,0,sizeof(AR_IMG_SET_S));

		while((file = readdir(dirp)) != NULL)
		{
			if(gu32QuitFlag)
				break;

			char fullname[128] = {0};
			strcpy(fullname, strDirPath.c_str());
			if((strcmp(file->d_name, ".")==0) ||(strcmp(file->d_name, "..")==0))
				continue;
			strcat(fullname, "/");
			strcat(fullname, file->d_name);
			MyNet.SetFileName(file->d_name);
			stImg->astInputImg[0].bPreIfcProcess = AR_TRUE;
			dirpEx = opendir(fullname);
			if(dirpEx != NULL){
				printf("WRNING: The path '%s' is not image, please check it!\n", fullname);
				closedir(dirpEx);
				continue;
			}
			s32Ret = MyNet.GetLocalImage(fullname, &stImg->astInputImg[0].astBatchImg[0], 0);
			if(s32Ret > 0)
			{
				MyNet.Forward(stImg, bInstant, bDebug);
				MyNet.PostProcess();
				continue;
			}
			else
				break;
		}
		cout << "Local image test finished." << endl;
		closedir(dirp);

		free(stImg);
		printf("exit_handler finished\r\n");
	}
	printf("AppEnd\r\n");
	MyNet.FreeNPUBuff();

	return 0;
}
