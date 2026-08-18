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
//#include "ar_ion.h"
#include "mpi_dsp_api.h"
#include "hal_dsp_api.h"
#include <assert.h>

#define  MAX_NUM_POINTS  5000

AR_S32 AR_DSP_Malloc(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size)
{
	AR_S32 s32Ret;
	s32Ret = ar_hal_sys_mmz_alloc(u64PhyAddr, pvVirtAddr, pstr_mmb, NULL, u32Size);

    	return s32Ret;
}

void AR_DSP_Free(AR_U64 u64_phy_addr, void* p_vir_addr)
{
    	ar_hal_sys_mmz_free(u64_phy_addr, p_vir_addr);
	u64_phy_addr = 0;
	p_vir_addr = NULL;
}

static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, AR_CHAR *ps8Argv[])
{
	AR_S32 i=0;
	
	for(i=0;i<s32Argc;i++)
	{
		if(!strcmp(ps8Name,ps8Argv[i]))
		{
			printf("Find Pra %s @ %d \r\n",ps8Name,i);
			return i;
		}
	}
	return 0;
}

AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{
	AR_S32 s32Ret = 0;
	AR_S32 s32Index = 0;

    s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
    if(s32Index>0 || argc==1)
    {
        printf("-input: src file name\n");
		printf("-w: src_width\n");
		printf("-h: src_height\n");
		printf("-thr: threshold\n");
        return 0;
    }

	AR_S32 src_width=640;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-w",argc,argv);
	if(s32Index>0)
	{
		src_width=atoi(argv[s32Index+1]);
		printf("src_width=%d \r\n",src_width);
	}

	AR_S32 src_height=480;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-h",argc,argv);
	if(s32Index>0)
	{
		src_height=atoi(argv[s32Index+1]);
		printf("src_height=%d \r\n",src_height);
	}

	AR_S32 threshold=75;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-thr",argc,argv);
	if(s32Index>0)
	{
		threshold=atoi(argv[s32Index+1]);
		printf("threshold=%d \r\n",threshold);
	}

    AR_CHAR s8PathIn[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-input",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8PathIn,argv[s32Index+1]);
		printf("input=%s \r\n",s8PathIn);
	}
	else{
		printf(" NO input path defined\r\n");
        return -1;
	}

	
    AR_U32 u32MMZReadSize = src_width*src_height*sizeof(int);
	AR_U64 u64PhyAddrIn = 0;
	void * pvVirtAddrIn = NULL; 
	AR_DSP_Malloc(&u64PhyAddrIn, &pvVirtAddrIn, "ArArmFast9In", u32MMZReadSize);
	if((!pvVirtAddrIn) || (u64PhyAddrIn == 0))
	{
		printf("Malloc ArArmFast9In failed.\n");
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrIn, 0 , u32MMZReadSize);
	
	FILE *pFile = fopen(s8PathIn, "rb");
	if(!pFile)
	{
		printf("fopen %s file failed\n", s8PathIn); 
		return -1;
	}
	fseek(pFile, 0 ,SEEK_END);
	AR_U32 u32ReadSize = ftell(pFile);
	printf("file %s read size = %d\n",s8PathIn, u32ReadSize);
	rewind(pFile);

	s32Ret = fread((AR_CHAR *)pvVirtAddrIn, 1, u32ReadSize, pFile);
	if(s32Ret != u32ReadSize)
	{
		printf("read file %s failed \n", s8PathIn);
		return -1;
	}
	fclose(pFile);


	AR_U32 u32Len = MAX_NUM_POINTS*sizeof(AR_U32);
	AR_U64 u64PhyAddrPX = 0;
	void * pvVirtAddrPX = NULL; 
	AR_DSP_Malloc(&u64PhyAddrPX, &pvVirtAddrPX, "ArArmFast9PX", u32Len);
	if((!pvVirtAddrPX) || (u64PhyAddrPX == 0))
	{
		printf("Malloc ArArmFast9PX failed.\n");
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrPX, 0 , u32Len);
	
	AR_U64 u64PhyAddrPY = 0;
	void * pvVirtAddrPY = NULL; 
	AR_DSP_Malloc(&u64PhyAddrPY, &pvVirtAddrPY, "ArArmFast9PY", u32Len);
	if((!pvVirtAddrPY) || (u64PhyAddrPY == 0))
	{
		printf("Malloc ArArmFast9PY failed.\n");
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrPY, 0 , u32Len);

	AR_U64 u64PhyAddrPS = 0;
	void * pvVirtAddrPS = NULL; 
	AR_DSP_Malloc(&u64PhyAddrPS, &pvVirtAddrPS, "ArArmFast9PS", u32Len);
	if((!pvVirtAddrPS) || (u64PhyAddrPS == 0))
	{
		printf("Malloc ArArmFast9PS failed.\n");
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrPS, 0 , u32Len);


	AR_U32 nms =1 ;
	AR_U32 output_list_is_external  = 1;
	AR_U32 use_grid =  1;

	AR_U32 au32Msg[12] = {0}; 
	au32Msg[0] = (AR_U32)(u64PhyAddrIn & 0x0FFFFFFFF); 
	au32Msg[1] = u64PhyAddrPX;
 	au32Msg[2] = u64PhyAddrPY;
	au32Msg[3] = u64PhyAddrPS;
	au32Msg[4] = src_width;
	au32Msg[5] = src_height;
	au32Msg[6] = threshold;
	au32Msg[7] = nms;
	au32Msg[8] = output_list_is_external;	
	au32Msg[9] = use_grid;


	printf("au32Msg[0] = %x\r\n",au32Msg[0]);
	printf("au32Msg[1] = %x\r\n",au32Msg[1]);
 	printf("au32Msg[2] = %x\r\n",au32Msg[2]);
 	printf("au32Msg[3] = %x\r\n",au32Msg[3]);
 	printf("au32Msg[4] = %x\r\n",au32Msg[4]);
	printf("au32Msg[5] = %x\r\n",au32Msg[5]);
 	printf("au32Msg[6] = %x\r\n",au32Msg[6]);
 	printf("au32Msg[7] = %x\r\n",au32Msg[7]);
 	printf("au32Msg[8] = %x\r\n",au32Msg[8]);
 	printf("au32Msg[9] = %x\r\n",au32Msg[9]);


    AR_DSP_TASK_S stTask = {0};
	strncpy((char *)stTask.s8Name, "ArArmCvFast9", 31);
	stTask.u32Priority = 0;
	stTask.u64MsgBodyAddr = (AR_U64)au32Msg;
	stTask.u32MsgLen = sizeof(au32Msg);
  
	printf("stTask.MsgBodyAddr = %x\r\n",stTask.u64MsgBodyAddr);

	s32Ret = AR_MPI_DSP_RemoteCall(0, &stTask, 1);
 	if(s32Ret < 0)
	 	printf("AR_MPI_DSP_RemoteCall return error %d\n",s32Ret); 

 
	if(pvVirtAddrIn)
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrIn);
	if(pvVirtAddrPX)
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrPX);
	if(pvVirtAddrPY)
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrPY);
	if(pvVirtAddrPS)
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrPS);

  	printf("ar arm cv finished\n");
  
	return s32Ret;
}





