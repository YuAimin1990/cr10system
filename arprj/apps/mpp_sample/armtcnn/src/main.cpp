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

#include "armtcnn_base.h"
#include "armtcnn_common.h"
#include "opencv2/opencv.hpp"

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
	AR_IMG_S stImg = {0};
    AR_S32 s32Ret = 0;
    AR_BOOL bInstant = AR_TRUE; //blocked call
    AR_BOOL bDebug = AR_FALSE;

    //get json file from command line
    if(argc < 2)
    {
        PrintUsage(argv[0]);
        return -1;
    }

    CARFaceDetector FaceDetector(argv[1]);

    if(!FaceDetector.IsInitialized())
    {
        return -1;
    }

    FaceDetector.DisableNPUDebug();
    FaceDetector.DisableProfile();

    if(argc >= 3)
    {
        if(0 == strcmp(argv[2], "prof"))
        {
            FaceDetector.EnableProfile();
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

            FaceDetector.EnableNPUDebug(u32Start, u32End);
            bDebug = AR_TRUE;
        }
        if(0 == strcmp(argv[2], "debugifc"))
        {
            FaceDetector.EnableIFCDebug();
        }
    }

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);

    if(FaceDetector.GetMode()== ARCNN_INPUT_LIVE)
    {
        s32Ret = FaceDetector.BindVideoDev();
        if(s32Ret)
        {
            return -1;
        }

        while(1)
        {
            if(gu32QuitFlag)
                break;

            s32Ret = FaceDetector.GetLiveImage(&stImg);
            if(s32Ret)
            {
                FaceDetector.DeBindVideoDev();
                return -1;
            }

            s32Ret = FaceDetector.Process(&stImg, bInstant, bDebug);
            if(s32Ret)
            {
                FaceDetector.ReleaseLiveImage(&stImg);
                FaceDetector.DeBindVideoDev();
                return -1;
            }

            //save results ??
            FaceDetector.DrawOsd();
        }

        cout << "Live test finished." << endl;
        FaceDetector.DeBindVideoDev();
    }
    else
    {
        //Get dir and search for pics
        DIR * dirp;
        struct dirent * file;
        string strDirPath = FaceDetector.GetLocalTestDir();
        dirp = opendir(strDirPath.c_str());
        if(!dirp)
        {
            cout << "Error open local test path" << endl;
            return -1;
        }

        while((file = readdir(dirp)) != NULL)
        {
            if(gu32QuitFlag)
                break;

            char fullname[128] = {0};
            strcpy(fullname, strDirPath.c_str());
            strcat(fullname, "/");
            strcat(fullname, file->d_name);

            s32Ret = FaceDetector.GetLocalImage(fullname, &stImg);
            if(s32Ret)
            {
                continue;
            }

            s32Ret = FaceDetector.Process(&stImg, bInstant, bDebug);
            if(s32Ret)
            {
                FaceDetector.ReleaseLocalImage(&stImg);
                break;
            }

            FaceDetector.ReleaseLocalImage(&stImg);
            FaceDetector.DrawOsd(fullname);
        }
        cout << "Local image test finished." << endl;
    }

    while(0)
    {
        sleep(1);
        if(gu32QuitFlag)
            break;
    }

    return 0;
}