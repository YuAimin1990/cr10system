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
#include <dirent.h>
#include "alphanum.h"


using namespace std;

#include "arcnn_base.h"
#include "arcnn_common.h"
#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"


extern "C" AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer);
extern "C" AR_S32 DumpIFCOutput(void * handle, void * pIfcOutput);

static int ar_alphasort(const struct dirent **a, const struct dirent **b)
{
	return (doj::alphanum_comp((char*)(*a)->d_name, (char*)(*b)->d_name)> 0);
}

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
	AR_U32  u32FrameId = 0;
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
	   MyNet.RegisterParseFunc((AR_CHAR *)"lrn", AR_CNN_ParseLrn);
	   MyNet.RegisterParseFunc((AR_CHAR *)"maxunpool", AR_CNN_ParseMaxUnpool);
	   MyNet.RegisterParseFunc((AR_CHAR *)"upsample", AR_CNN_ParseUpsample);
	   MyNet.RegisterParseFunc((AR_CHAR *)"deformable", AR_CNN_ParseDeformable);
	   MyNet.RegisterParseFunc((AR_CHAR *)"sevcallback", AR_CNN_ParseSevCallback);
	   MyNet.RegisterParseFunc((AR_CHAR *)"LRN", AR_CNN_ParseCustomLrn);

	   MyNet.RegisterCallbackFunc((AR_CHAR *)"lrn", AR_CNN_OperatorLrn);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"getconfidence", AR_CNN_OperatorGetConf);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"maxunpool", AR_CNN_OperatorMaxUnPool);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"reshape", AR_CNN_OperatorReshape);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"Reshape", AR_CNN_OperatorOnnxReshape);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"upsample", AR_CNN_OperatorUpsample);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"reciprocal", AR_CNN_OperatorReciprocal);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"deformable", AR_CNN_OperatorDeformable);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"sevcallback", AR_CNN_OperatorSevCallback);
	   MyNet.RegisterCallbackFunc((AR_CHAR *)"LRN", AR_CNN_OperatorCustomLrn);

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
	AR_U32 u32FrameNum = MyNet.GetFrameNum();
    printf("NetWork InputNum=%d BatchNum=%d FrameNum=%d\r\n",u32InputNum,u32BatchNum,u32FrameNum);
	AR_IMG_SET_S *stImg = (AR_IMG_SET_S *)malloc(sizeof(AR_IMG_SET_S));
	if(!stImg)
	{
	    printf("malloc stImg error \r\n");
		MyNet.FreeNPUBuff();
		closedir(dirp);
		return -1;
	}
	memset(stImg,0,sizeof(AR_IMG_SET_S));

	int scandir_num = 0;
	int scandir_count = 0;
	struct dirent **in_file;
	scandir_num = scandir(strDirPath.c_str(), &in_file, 0, ar_alphasort);
	if(scandir_num < 0)
	{
		printf("Scan local dir %s error\r\n", strDirPath.c_str());
		return -1;;
	}
	
	//while((file = readdir(dirp)) != NULL)
	while(scandir_count< scandir_num)
	{
		if(gu32QuitFlag)
			break;

		char fullname[128] = {0};
		//strcpy(fullname, strDirPath.c_str());
		//strcat(fullname, "/");
		//strcat(fullname, file->d_name);

		//currently, we don't support recursive DIR
		if(in_file[scandir_count]->d_type != DT_REG)
		{
			free(in_file[scandir_count]);
			scandir_count ++;
			continue;
		}

		snprintf(fullname, 128, "%s%s", strDirPath.c_str(), in_file[scandir_count]->d_name);
		free(in_file[scandir_count]);
		scandir_count ++;

		s32Ret = MyNet.GetLocalInput(fullname, stImg, u32InputId, u32BatchId, u32FrameId);
		if(s32Ret<0)
		{
			continue;
		}

		if(u32FrameId < u32FrameNum)
		{
			u32FrameId++;
		}

		if(u32FrameId == u32FrameNum) 
		{
			u32FrameId =0;
			u32BatchId++;
		}

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

	ofstream ofile("InFiles.bin", ios::out|ios::binary|ios::ate);
    if(ofile.is_open())
    {
    	AR_U32 s32FileSize = MyNet.GetFrameNum()*MyNet.GetYUVStep(0);
		AR_IMG_S *pstImg = &(stImg->astInputImg[0].astBatchImg[0]);
        ofile.write((AR_CHAR *)pstImg->astChannels[0].uptrAddrVirt, s32FileSize);
        ofile.close();
    }


	s32Ret = MyNet.Forward(stImg, bInstant, bDebug);

	MyNet.ReleaseLocalInput(stImg);

    printf("Forward ret = %d\n", s32Ret);

	MyNet.PostProcess();

	free(stImg);
	MyNet.FreeNPUBuff();
	closedir(dirp);
    printf("Local image test finished.\r\n");
	
	return 0;
}
