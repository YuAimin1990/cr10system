#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "hal_sys.h"
#include "hal_icc.h"

#define ICC_MSG_ID  0xff00ff00

#pragma pack (4)
typedef struct
{
	AR_CHAR	 	s8Name[32];
	AR_U32 		u32Priority;
	AR_U32		u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32 		u32CoreID;
	AR_U64		u64MsgBodyAddr;
	AR_U32		u32MsgLen;
}AR_ICC_TASK_SEND_S;

typedef struct
{
	AR_CHAR 	s8Name[32];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U64 		u64MsgBodyAddr;
	AR_U32		u32MsgLen;
	AR_S32		s32ErrCode;
	AR_U32		u32Time;
}AR_ICC_TASK_ACK_S;

typedef struct
{
    AR_U64 u64PhyAddr;
	AR_U64 u64VirtAddr;
	AR_U32 u32Width;
    AR_U32 u32Height;
    AR_U32 u32Stride;
} AR_DSP_IMG_CHANNEL_S;

typedef struct
{
    AR_U32 u32ChannelNum;
    AR_DSP_IMG_CHANNEL_S astSrcChannels[4];
	AR_DSP_IMG_CHANNEL_S astDstChannels[4];
} AR_DSP_IMG_S;

#pragma pack ()


static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, const AR_CHAR *ps8Argv[])
{
    AR_S32 i=0;
	for(i=0;i<s32Argc;i++)
	{
         if(!strcmp(ps8Name,ps8Argv[i]))
         {
            printf("Find Pra %s @ %d ",ps8Name,i);
            return i;
         }
	}
	return 0;
}

static AR_S32 SAMPLE_DSP_MMZ_Buffer_Request(AR_DSP_IMG_S *pstDspImg)
{
	AR_S32 s32Ret = -1;
	if(!pstDspImg)
	{
		printf("pstDspImg is NULL\r\n");
		return -1;
	}

    /*******************SRC Y******************************/
	int u32YSrcSize = (pstDspImg->astSrcChannels[0].u32Stride+2)*(pstDspImg->astSrcChannels[0].u32Height+2); 
	AR_U64 u64YSrcPa = 0;
	void * pvYSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YSrcPa, &pvYSrcVa, "YSrc", NULL, u32YSrcSize);
	if(s32Ret)
	{
		printf(" get src y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYSrcVa, 128, u32YSrcSize);
	pstDspImg->astSrcChannels[0].u64VirtAddr= (AR_UINTPTR)pvYSrcVa;
	pstDspImg->astSrcChannels[0].u64PhyAddr= (AR_UINTPTR)u64YSrcPa;

    int u32USrcSize=0;
    if(pstDspImg->u32ChannelNum ==3)
    {
        /*******************SRC U******************************/
		int u32USrcSize = (pstDspImg->astSrcChannels[1].u32Stride+2)*(pstDspImg->astSrcChannels[1].u32Height+2);
		AR_U64 u64USrcPa = 0;
		void * pvUSrcVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64USrcPa, &pvUSrcVa, "USrc", NULL, u32USrcSize);
		if(s32Ret)
		{
			printf(" get src u addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvUSrcVa,128,u32USrcSize);
		pstDspImg->astSrcChannels[1].u64VirtAddr= (AR_UINTPTR)pvUSrcVa;
	    pstDspImg->astSrcChannels[1].u64PhyAddr= (AR_UINTPTR)u64USrcPa;

		/*******************SRC V******************************/
		AR_U64 u64VSrcPa = 0;
		void * pvVSrcVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64VSrcPa, &pvVSrcVa, "VSrc", NULL, u32USrcSize);
		if(s32Ret)
		{
			printf(" get src v addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvVSrcVa,128,u32USrcSize);
		pstDspImg->astSrcChannels[2].u64VirtAddr= (AR_UINTPTR)pvVSrcVa;
	    pstDspImg->astSrcChannels[2].u64PhyAddr= (AR_UINTPTR)u64VSrcPa;
    }

	/*******************DST Y******************************/
	int u32YDstSize = pstDspImg->astSrcChannels[0].u32Stride*pstDspImg->astSrcChannels[0].u32Height; 
	AR_U64 u64YDstPa = 0;
	void * pvYDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YDstPa, &pvYDstVa, "YDst", NULL, u32YDstSize);
	if(s32Ret)
	{
		printf(" get dst y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYDstVa,128,u32YDstSize);
	pstDspImg->astDstChannels[0].u64VirtAddr= (AR_UINTPTR)pvYDstVa;
	pstDspImg->astDstChannels[0].u64PhyAddr= (AR_UINTPTR)u64YDstPa;

	if(pstDspImg->u32ChannelNum ==3)
    {

		/*******************DST U******************************/
		int u32UDstSize = pstDspImg->astSrcChannels[1].u32Stride*pstDspImg->astSrcChannels[1].u32Height; 
		AR_U64 u64UDstPa = 0;
		void * pvUDstVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64UDstPa, &pvUDstVa, "UDst", NULL, u32UDstSize);
		if(s32Ret)
		{
			printf(" get dst u addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvUDstVa,128,u32USrcSize);
		pstDspImg->astDstChannels[1].u64VirtAddr= (AR_UINTPTR)pvUDstVa;
		pstDspImg->astDstChannels[1].u64PhyAddr= (AR_UINTPTR)u64UDstPa;

		/*******************DST V******************************/
		AR_U64 u64VDstPa = 0;
		void * pvVDstVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64VDstPa, &pvVDstVa, "VDst", NULL, u32UDstSize);
		if(s32Ret)
		{
			printf(" get dst_v addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvVDstVa,128,u32USrcSize);
		pstDspImg->astDstChannels[2].u64VirtAddr= (AR_UINTPTR)pvVDstVa;
		pstDspImg->astDstChannels[2].u64PhyAddr= (AR_UINTPTR)u64VDstPa;
	}
	return 0;

}

static AR_S32 SAMPLE_DSP_MMZ_Buffer_Release(AR_DSP_IMG_S *pstDspImg)
{
    if(!pstDspImg)
    {
    	printf("pstDspImg is NULL\r\n");
		return -1;
    }

	if(pstDspImg->astSrcChannels[0].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astSrcChannels[0].u64PhyAddr, (AR_VOID *)pstDspImg->astSrcChannels[0].u64VirtAddr);

	if(pstDspImg->astSrcChannels[1].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astSrcChannels[1].u64PhyAddr, (AR_VOID *)pstDspImg->astSrcChannels[1].u64VirtAddr);

	if(pstDspImg->astSrcChannels[2].u64VirtAddr)
   	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astSrcChannels[2].u64PhyAddr, (AR_VOID *)pstDspImg->astSrcChannels[2].u64VirtAddr);

	if(pstDspImg->astDstChannels[0].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astDstChannels[0].u64PhyAddr, (AR_VOID *)pstDspImg->astDstChannels[0].u64VirtAddr);

	if(pstDspImg->astDstChannels[1].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astDstChannels[1].u64PhyAddr, (AR_VOID *)pstDspImg->astDstChannels[1].u64VirtAddr);

	if(pstDspImg->astDstChannels[2].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astDstChannels[2].u64PhyAddr, (AR_VOID *)pstDspImg->astDstChannels[2].u64VirtAddr);

	return 0;
}


static AR_S32 gu32QuitFlag = 0;
void SignalHandler(AR_S32 sig)
{
    printf("Get signal %d\r\n", sig);
    gu32QuitFlag = 1;
}



AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{
    AR_S32 s32Ret = 0;
/*********************Args Parser***********************/

    AR_S32 s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
    if(s32Index>0 || argc==1)
    {
        printf("-n: loop num\n");
        return 0;
    }


	AR_S32 s32SrcFormat = 0;	
	AR_S32 s32SrcWidth = 1920;
	AR_S32 s32SrcHeight = 1080;
	AR_S32 s32SrcStride = 1920;
	AR_S32 u32Loop =10;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-n",argc,argv);
	if(s32Index>0)
	{
		u32Loop=atoi(argv[s32Index+1]);
		printf("u32Loop=%d \r\n",u32Loop);
		if(u32Loop <0)
		{
			printf("u32Loop error \r\n");
			return -1;
		}
	}

	static AR_S32 fd_icc = -1;
	fd_icc = ar_hal_icc_open_dev();
	if (fd_icc < 0) {
		printf("open icc fail\n");
		return -1;
	}
	s32Ret = ar_hal_icc_register_msgid(fd_icc, ICC_MSG_ID);
	if(s32Ret < 0) {
		printf("register msg id %x fail\n", ICC_MSG_ID);
		return -1;
	}

    AR_DSP_IMG_S stDspImg = {};
    if(s32SrcFormat ==0)  //yuv420p
    {
         stDspImg.u32ChannelNum = 3; 
		 stDspImg.astSrcChannels[0].u32Width = s32SrcWidth;
		 stDspImg.astSrcChannels[0].u32Height = s32SrcHeight;
		 stDspImg.astSrcChannels[0].u32Stride = s32SrcStride;
		 stDspImg.astSrcChannels[1].u32Width = s32SrcWidth/2;
		 stDspImg.astSrcChannels[1].u32Height = s32SrcHeight/2;
		 stDspImg.astSrcChannels[1].u32Stride = s32SrcStride/2;
		 stDspImg.astSrcChannels[2].u32Width = s32SrcWidth/2;
		 stDspImg.astSrcChannels[2].u32Height = s32SrcHeight/2;
		 stDspImg.astSrcChannels[2].u32Stride = s32SrcStride/2;
		 memcpy(stDspImg.astDstChannels,stDspImg.astSrcChannels,sizeof(AR_DSP_IMG_CHANNEL_S)*4);
	}
	else   //yuv400p
	{
	     stDspImg.u32ChannelNum = 1;
		 stDspImg.astSrcChannels[0].u32Width = s32SrcWidth;
		 stDspImg.astSrcChannels[0].u32Height = s32SrcHeight;
		 stDspImg.astSrcChannels[0].u32Stride = s32SrcStride;
		 memcpy(stDspImg.astDstChannels,stDspImg.astSrcChannels,sizeof(AR_DSP_IMG_CHANNEL_S)*4);
	}
	
	s32Ret = SAMPLE_DSP_MMZ_Buffer_Request(&stDspImg);
	if(s32Ret < 0)
	{
		goto exit_handler;
		printf("AR_MPI_DSP_RemoteCall fail\r\n");
	}

	
    AR_S32 runcnt =0;
    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
	while(1)
	{
		if(gu32QuitFlag)
			break;
		if(runcnt >= u32Loop)
				break;

		AR_ICC_TASK_SEND_S stSendTask ={0};
		strncpy((AR_CHAR *)stSendTask.s8Name, "ArArmCalcGauss3x3", 31);
		stSendTask.u32Priority = 0;
		stSendTask.u64MsgBodyAddr = (AR_U64)&stDspImg;
		stSendTask.u32MsgLen = sizeof(stDspImg);

		
		struct timeval tm;
        gettimeofday(&tm, NULL);
        AR_U64 u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;
		
		AR_U32 u32HeaderLen = sizeof(AR_ICC_TASK_SEND_S);
		AR_U32 u32SenderLen = u32HeaderLen + stSendTask.u32MsgLen;
		if(u32SenderLen > 1024){
			printf("THE ARGs Size is Larger Than Allowed\n");
			goto exit_handler;
		}
		AR_CHAR* pSendBuffer = (AR_CHAR*)malloc(u32SenderLen);
		if(!pSendBuffer)
		{
			printf("IccBuff malloc failed!\n");
			goto exit_handler;
		}
		memcpy((AR_CHAR*)pSendBuffer, (AR_CHAR*)&stSendTask, u32HeaderLen);
		memcpy((AR_CHAR*)(pSendBuffer + u32HeaderLen), (AR_CHAR*)stSendTask.u64MsgBodyAddr, stSendTask.u32MsgLen);
		
		s32Ret = ar_hal_icc_send(fd_icc, (AR_UCHAR *)pSendBuffer, u32SenderLen,
									ICC_MSG_ID, ICC_CORE_CEVA_0);
		if(s32Ret < 0)
		{
			printf("icc send fail\n");
			free(pSendBuffer);
			goto exit_handler;
		}


		AR_ICC_TASK_ACK_S stRecvTask;
		s32Ret = ar_hal_icc_receive(fd_icc, (AR_UCHAR *)&stRecvTask,
			sizeof(stRecvTask), ICC_MSG_ID);
		if(s32Ret < 0) {
			printf("icc receive fail\n");
			goto exit_handler;
		}

		free(pSendBuffer);
		gettimeofday(&tm, NULL);
		AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;

		printf("[%d] %s taskes %f ms\r\n",runcnt, stRecvTask.s8Name,(u64Toc-u64Tic)*1.0/1000);

		runcnt++;
	}

exit_handler:

    ar_hal_icc_close_dev(fd_icc);
	ar_hal_icc_unregister_msgid(fd_icc,ICC_MSG_ID);
	SAMPLE_DSP_MMZ_Buffer_Release(&stDspImg);


	printf("test icc finished\r\n");

    return 0;

}


