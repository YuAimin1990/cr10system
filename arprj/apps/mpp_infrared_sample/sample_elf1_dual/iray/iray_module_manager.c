#include "iray_module_manager.h"
#include "iray_internal.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 256
#define HEIGHT 192

static SHisiLibMgr g_Mgr;

YuvImageCallBack g_YuvCallbackFunc = NULL;
NucDataCallBack g_NucCallbackFunc = NULL;

int save_nuc_data_count = 0;

/*
static int64_t getSystemMilliTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t msec = tv.tv_sec;
    msec *= 1000;
    msec += tv.tv_usec/1000;
    return msec;
}*/

IR_S32 fRtdGetNUC_tFrame(IRAY_FRAME_INFO_S *pFrameInfo, IR_U32* pNUC_tFrame)
{
    return 0;
}

IR_S32 fRtdGetAgcFrame(IRAY_FRAME_INFO_S *pFrameInfo, IR_U8* pAgcFrame)
{  
    return 0;
}

IR_S32 fRtdGetTempFrame(IRAY_FRAME_INFO_S *pFrameInfo, IR_U16* pTempFrame,POINT_TEMP_INFO_S ptMaxTemp, POINT_TEMP_INFO_S ptMinTemp, IR_U16 u16AverageTemp)
{
    return 0;
}

IR_S32 fRtdGetYVUFrame(IRAY_FRAME_INFO_S *pFrameInfo, IR_U8* pYVUFrame)
{
	return 0;
}

IR_S32 fRtdGetCommand(IR_U32 u32DevID,IR_UCHAR *pData, IR_S32 s32Len)
{
    return 0;
}


IR_S32 fpCtlShutter(IR_U32 u32DevID,IR_UCHAR ON_OFF)
{
    printf("fpCtlShutter ON_OFF[%d]\n",ON_OFF);
    return 0;
}

IR_S32 fRtdGetNucFrame(IRAY_FRAME_INFO_S *pFrameInfo, IR_U8* pNucFrame)
{
    //printf("fRtdGetNucFrame %u %u %p\n",pFrameInfo->u32Width,pFrameInfo->u32Height,pNucFrame);

    if (g_NucCallbackFunc) {
        g_NucCallbackFunc((unsigned short*)pNucFrame,WIDTH*HEIGHT);
    }
    
    static int nuc_fd = 0;
    if (save_nuc_data_count>0)
    {
        if(!nuc_fd){
            char fname[128] = {0};
            sprintf(fname,"/tmp/dump_raw_nuc-%d.raw",save_nuc_data_count);
            nuc_fd = open(fname, O_RDWR | O_CREAT);
            if(nuc_fd){
                printf("save nuc data begin...\n");
            }
        }
        if(nuc_fd){
            int r = write(nuc_fd, (unsigned char *)pNucFrame, WIDTH*HEIGHT*2);
            printf("write file, nuclen=%d wlen=%d index=%d\n",WIDTH*HEIGHT*2,r,save_nuc_data_count);
        }
        save_nuc_data_count--;
    }
    else if(nuc_fd>0){
        close(nuc_fd);
        nuc_fd = 0;
        printf("save nuc data end\n");
    }
    
    return 0;
}

int IRAY_MODEL_SDK_Init()
{
    IRAY_RTD_API_INIT_ATTR_S sRtdInitAttr;
    strcpy(sRtdInitAttr.pConfigFilePath,"/usrdata/iray_demo/iraycfg");
    strcpy(sRtdInitAttr.pServerIp,"10.10.25.251");
    sRtdInitAttr.s32TempFrameRate      = 10;
    
    sRtdInitAttr.u8HasIpServer = 1;
    sRtdInitAttr.u8RtdClock = 30;
    sRtdInitAttr.enRtdFrameRate = IRAY_RTD_FRAME_RATE_25;
    sRtdInitAttr.u8IsForeheadTemp = 1;
    sRtdInitAttr.pSetIp                 = &g_Mgr.RtdSetIp;
    sRtdInitAttr.pSetTempFrameRate      = &g_Mgr.RtdSetTempFrameRate;
    sRtdInitAttr.pPutCommand            = &g_Mgr.RtdPutCmd;
    sRtdInitAttr.pPutFrame              = &g_Mgr.RtdPutFrame;
    sRtdInitAttr.pSetVideoType          = &g_Mgr.RtdSetVideoType;
    sRtdInitAttr.pSetPseudoColorParams  = &g_Mgr.RtdSetPseudoColorParams;
    
    sRtdInitAttr.pGetAlarmState         = &g_Mgr.RtdGetAlarmState;
    sRtdInitAttr.pSetAlarmParams        = &g_Mgr.RtdSetAlarmParams;
    sRtdInitAttr.pReiginCalc            = &g_Mgr.RtdReiginCalc;
    
    sRtdInitAttr.pAdjustAdcGain         = &g_Mgr.RtdAdjustAdcGain;
    sRtdInitAttr.pGetRtdSysId           = &g_Mgr.RtdGetSysId;
    sRtdInitAttr.pSetForeheadTempSwitch = &g_Mgr.RtdSetForeheadTempSwitch;
    sRtdInitAttr.pGetForeheadTempSwitch = &g_Mgr.RtdGetForeheadTempSwitch;
    sRtdInitAttr.pSetForeheadParams     = &g_Mgr.RtdSetForeheadTempParams;
    sRtdInitAttr.pGetForeheadParams     = &g_Mgr.RtdGetForeheadTempParams;
    sRtdInitAttr.pSetBlackbodyParams    = &g_Mgr.RtdSetBlackbodyParams;
    sRtdInitAttr.pGetBlackbodyParams    = &g_Mgr.RtdGetBlackbodyParams;
    sRtdInitAttr.pSetTempCompensationParams = &g_Mgr.RtdSetTempCompensationParams;
    sRtdInitAttr.pGetTempCompensationParams = &g_Mgr.RtdGetTempCompensationParams;
    sRtdInitAttr.pCalibForeheadTemp     = &g_Mgr.RtdCalibForeheadTemp;
    sRtdInitAttr.pSetNucFrameRate       = &g_Mgr.RtdSetNucpFrameRate;
    sRtdInitAttr.pGetIspPicColorAttr    = &g_Mgr.RtdGetIspPicColorAttr;
    sRtdInitAttr.pSetIspPicColorAttr    = &g_Mgr.RtdSetIspPicColorAttr;
    sRtdInitAttr.pGetIsp2DDenoiseAttr   = &g_Mgr.RtdGetIsp2DDenoiseAttr;
    sRtdInitAttr.pSetIsp2DDenoiseAttr   = &g_Mgr.RtdSetIsp2DDenoiseAttr;
    sRtdInitAttr.pGetIsp3DDenoiseAttr   = &g_Mgr.RtdGetIsp3DDenoiseAttr;
    sRtdInitAttr.pSetIsp3DDenoiseAttr   = &g_Mgr.RtdSetIsp3DDenoiseAttr;
    sRtdInitAttr.pGetIspDDEAttr         = &g_Mgr.RtdGetIspDDEAttr;
    sRtdInitAttr.pSetIspDDEAttr         = &g_Mgr.RtdSetIspDDEAttr;
    sRtdInitAttr.pGetIspRVEAttr         = &g_Mgr.RtdGetIspRVEAttr;
    sRtdInitAttr.pSetIspRVEAttr         = &g_Mgr.RtdSetIspRVEAttr;
    sRtdInitAttr.pGetIspBMIAttr         = &g_Mgr.RtdGetIspBMIAttr;
    sRtdInitAttr.pSetIspBMIAttr         = &g_Mgr.RtdSetIspBMIAttr;
    sRtdInitAttr.pGetAgcFrame           = fRtdGetAgcFrame;
    sRtdInitAttr.pGetTempFrame          = fRtdGetTempFrame;
    sRtdInitAttr.pGetCommand            = fRtdGetCommand;
    sRtdInitAttr.pCtlShutter            = fpCtlShutter;
    sRtdInitAttr.pGetYVUFrame           = fRtdGetYVUFrame;
    sRtdInitAttr.pGetNucFrame           = fRtdGetNucFrame;
    sRtdInitAttr.pGetNUC_tFrame         = fRtdGetNUC_tFrame;
    g_Mgr.pRtdHandle = NULL;
    g_Mgr.pRtdHandle = IrayRtdInit(&sRtdInitAttr);
    if (g_Mgr.pRtdHandle==NULL)
    {
        printf("IrayRtdInit error.\n");
        return -1;
    }
    g_Mgr.swap_buffer = (unsigned char *)malloc(WIDTH*HEIGHT*2);

    char sSysId[512] = {0};
    int iSysIdLen = 0;
    g_Mgr.RtdGetSysId(g_Mgr.pRtdHandle,0,sSysId,&iSysIdLen,512);
    printf("------RtdGetSysId len=%d str=[%s]\n",iSysIdLen,sSysId);

    g_Mgr.RtdSetVideoType(g_Mgr.pRtdHandle,0,IRAY_FRAME_TYPE_NUC);
    return 0;
}

int iray_init()
{
    IRAY_MODEL_SDK_Init();
    
    if(g_Mgr.pRtdHandle)
        return 0;
    return -1;
}

void iray_release()
{
    if(g_Mgr.pRtdHandle){
        IrayRtdUnit(g_Mgr.pRtdHandle);
        g_Mgr.pRtdHandle = NULL;
        free(g_Mgr.swap_buffer);
        g_YuvCallbackFunc = NULL;
    }
}

int iray_push_raw_data(unsigned char * pData, int dataLen)
{
    if(!g_Mgr.pRtdHandle)
        return -1;
    static IR_U32 frame_index = 0;
    //int i;

    //unsigned char *swap_buffer = g_Mgr.swap_buffer;

    IRAY_FRAME_INFO_S sRtdFrame;
	sRtdFrame.enFrameType = IRAY_FRAME_TYPE_ORG;
	sRtdFrame.u32DevID = 0;
	sRtdFrame.u32Width = WIDTH;
	sRtdFrame.u32Height = HEIGHT;
	sRtdFrame.u32Len = WIDTH*HEIGHT*sizeof(IR_U16);
	unsigned char *p=(unsigned char *)pData+WIDTH*2*8;
	unsigned char *q=(unsigned char *)pData+WIDTH*2*202;
	//unsigned short v_temp = (q[256*201*2+2*2]<<8)+(q[256*201*2+2*2+1]);
	unsigned short v_temp = (q[3]<<8)+(q[4]);
	sRtdFrame.u32VTemp = v_temp;
	sRtdFrame.u64Pts = 0;
	sRtdFrame.u32Seq = frame_index++;
    /*
	unsigned short *ori = (unsigned short *)p;
	unsigned short *dst = (unsigned short *)swap_buffer;

	for (i=0;i<WIDTH*HEIGHT;i++) {
		dst[i] = (ori[i] << 8) | (ori[i] >> 8);
	}*/

    g_Mgr.RtdPutFrame(g_Mgr.pRtdHandle,&sRtdFrame,(IR_UCHAR*)p);

    return 0;
}

void iray_set_yuv_callback(YuvImageCallBack func)
{
    g_YuvCallbackFunc = func;
}

void iray_set_nuc_callback(NucDataCallBack func)
{
	g_NucCallbackFunc = func;
}

void save_nuc_data(int count)
{
    if(save_nuc_data_count==0){
        save_nuc_data_count = count;
    }
}
