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
    AR_U32  u32RunLoop = 1;
	AR_U32  u32InputNum =0;
	AR_U32  u32BatchNum =0;
	AR_U32  u32InputId = 0;
	AR_U32  u32BatchId = 0;
    AR_S32 	s32Ret = 0;
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

    MyNet.DisableNPUDebug();
    MyNet.DisbleProfile();

    //Malloc NPU input/output space
    //MyNet.MallocNPUBuff();

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
			
            MyNet.EnableNPUDebug(u32Start, u32End);
            bDebug = AR_TRUE;
        }

		if(0 == strcmp(argv[2], "loop"))
        {
			if(argc >= 4)
		    {
			    u32RunLoop = atoi(argv[3]);
		    }
			MyNet.EnableProfile();
        }
    }

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);


    if(MyNet.GetMode() == ARCNN_INPUT_LIVE)
    {
		s32Ret = MyNet.StartViVo();
		if(s32Ret)
        {
            printf("Start VIVO failed ! \n");
            return -1;
        }

		//Malloc NPU input/output space
    	MyNet.MallocNPUBuff();
		
		s32Ret = MyNet.RunThread();
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

            s32Ret = MyNet.PushLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
            if(s32Ret)
            {
				//Net.DeBindVideoDev();
				free(stImg);
                MyNet.FreeNPUBuff();
                return -1;
            }

			stImg->astInputImg[0].bPreIfcProcess = AR_TRUE;

            s32Ret = MyNet.Forward(stImg, bInstant, bDebug);
            if(s32Ret)
            {
				MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
                //Net.DeBindVideoDev();
				free(stImg);
                MyNet.FreeNPUBuff();
                return -1;
            }
            MyNet.PostProcess(NULL);
			MyNet.ReleaseLiveImage(&stImg->astInputImg[0].astBatchImg[0]);
        }
		
        cout << "Live test finished." << endl;
        //MyNet.DeBindVideoDev();

    }
    else
    {
		//Malloc NPU input/output space
    	MyNet.MallocNPUBuff();
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

                for(AR_U32 x=0;x<u32RunLoop;x++)
                {
					s32Ret = MyNet.Forward(stImg, bInstant, bDebug);
                }

				MyNet.ReleaseLocalImage(stImg);
			}
	
			 if(s32Ret)
			 {
			 	 printf("Forward ret = %d\n", s32Ret);
				 break;
			 }
			 MyNet.PostProcess(file->d_name);
			 printf("\n");
    	}
	cout << "Local image test finished." << endl;
   }

exit_handler:

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

	return 0;
}
