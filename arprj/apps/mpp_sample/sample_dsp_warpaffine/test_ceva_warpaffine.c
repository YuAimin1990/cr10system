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
        printf("-output: dst file name\n");
        return 0;
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


  	AR_CHAR s8PathOut[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-output",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8PathOut,argv[s32Index+1]);
		printf("output0=%s \r\n",s8PathOut);
	}
	else{
		printf(" NO output0 path defined\r\n");
        return -1;
	}

	AR_U32 bit_depth = 8;	
	AR_U32 ele_size=1;
	AR_U32 src_width=1920;
    AR_U32 src_height=1080;
	AR_U32 dst_width=1080;
	AR_U32 dst_height=1920;
	AR_U32 roi_top_left_x=20;
	AR_U32 roi_top_left_y = 40;
	AR_U32 roi_width = 1000;
	AR_U32 roi_height = 1880;
	AR_U32 fill_val = 128;
 
	AR_U64 u64PhyM = 0;
	void * pvVirtPhyM = NULL;  
	AR_DSP_Malloc(&u64PhyM, &pvVirtPhyM, "M", 6*sizeof(float));//ArmCvIn
	if((!pvVirtPhyM) || (u64PhyM == 0))
	{
		printf("Malloc M failed.\n");
		return -1;
	}

	float *M= (float*)pvVirtPhyM;
 	M[0] = 0;
	M[1] = 1;
	M[2] = 0;
	M[3] = -1;
	M[4] = 0;
	M[5] = 1080;
  
/*******************Input*****************/
	FILE *pFile = fopen(s8PathIn, "rb");
	if(!pFile)
	{
		printf("fopen %s file failed\n", s8PathIn);
		AR_DSP_Free(u64PhyM, pvVirtPhyM);  
		return -1;
	}

	fseek(pFile, 0 ,SEEK_END);
	AR_U32 u32ReadSize = ftell(pFile);
	printf("file %s read size = %d\n",s8PathIn, u32ReadSize);
	rewind(pFile);

	AR_U64 u64PhyAddrIn = 0;
	void * pvVirtAddrIn = NULL; 
	AR_DSP_Malloc(&u64PhyAddrIn, &pvVirtAddrIn, "ArArmCvIn", u32ReadSize);//ArmCvIn
	if((!pvVirtAddrIn) || (u64PhyAddrIn == 0))
	{
		printf("Malloc WarpIn failed.\n");
		AR_DSP_Free(u64PhyM, pvVirtPhyM);  
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrIn, 0 , u32ReadSize);

	s32Ret = fread((AR_CHAR *)pvVirtAddrIn, 1, u32ReadSize, pFile);
	if(s32Ret != u32ReadSize)
	{
		printf("read file %s failed \n", s8PathIn);
		AR_DSP_Free(u64PhyM, pvVirtPhyM);
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrIn);
		return -1;
	}
	fclose(pFile);

	//	for (int j = 0; j <100; j++)
	//		printf("pvVirtAddrIn[%d] =%d: \r\n", j, *((unsigned char*)pvVirtAddrIn+j));

    
/*********************Output setting**********************/
	AR_U64 u64PhyAddrOut = 0;
	void * pvVirtAddrOut = NULL;  
	AR_U32 u32WriteSize =  u32ReadSize;  
	AR_DSP_Malloc(&u64PhyAddrOut, &pvVirtAddrOut, "ArCvout", u32WriteSize);//
	if((!pvVirtAddrOut) || (u64PhyAddrOut == 0))
	{
		printf("Malloc ArCvout failed.\n");
		AR_DSP_Free(u64PhyM, pvVirtPhyM);
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrIn);
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrOut, 0 , u32WriteSize);

	AR_U32 au32Msg[14] = {0}; 
	au32Msg[0] = (AR_U32)(u64PhyAddrIn & 0x0FFFFFFFF); 
 	au32Msg[1] = (AR_U32)(u64PhyAddrOut & 0x0FFFFFFFF);
	au32Msg[2] = src_width;
	au32Msg[3] = src_height;
	au32Msg[4] = dst_width;
	au32Msg[5] = dst_height;
	au32Msg[6] = roi_top_left_x;	
	au32Msg[7] = roi_top_left_y;
  au32Msg[8] = bit_depth;
  au32Msg[9] = ele_size;
	au32Msg[10] = roi_width;	
	au32Msg[11] = roi_height;
	au32Msg[12] = fill_val;
	au32Msg[13] =  (AR_U32)(u64PhyM & 0x0FFFFFFFF);

	printf("au32Msg[0] = %x\r\n",au32Msg[0]);
	printf("au32Msg[1] = %x\r\n",au32Msg[1]);
 	printf("au32Msg[2] = %d\r\n",au32Msg[2]);
 	printf("au32Msg[3] = %d\r\n",au32Msg[3]);
 	printf("au32Msg[4] = %d\r\n",au32Msg[4]);
	printf("au32Msg[5] = %d\r\n",au32Msg[5]);
 	printf("au32Msg[6] = %d\r\n",au32Msg[6]);
 	printf("au32Msg[7] = %d\r\n",au32Msg[7]);
 	printf("au32Msg[8] = %x\r\n",au32Msg[8]);
 	printf("au32Msg[9] = %d\r\n",au32Msg[9]);
 	printf("au32Msg[10] = %d\r\n",au32Msg[10]);
	printf("au32Msg[11] = %d\r\n",au32Msg[11]);
 	printf("au32Msg[12] = %d\r\n",au32Msg[12]);


    AR_DSP_TASK_S stTask = {0};
	strncpy((char *)stTask.s8Name, "ArArmCvWarpAffine", 31);//ArArmCvApp
	stTask.u32Priority = 0;
	stTask.u64MsgBodyAddr = (AR_U64)au32Msg;
	stTask.u32MsgLen =14 * sizeof(AR_U32);
  
	printf("stTask.MsgBodyAddr = %llu\r\n",stTask.u64MsgBodyAddr);

	s32Ret = AR_MPI_DSP_RemoteCall(0, &stTask, 1);
 	if(s32Ret < 0)
	 	printf("AR_MPI_DSP_RemoteCall return error %d\n",s32Ret); 

 /***********************Store output****************************/
 	FILE * fp = NULL;
	fp = fopen(s8PathOut, "wb+");
	if(!fp)
	{
		printf("Open file %s failed.\n", s8PathOut); 
		return -1;
	}

	s32Ret = fwrite((unsigned char*)pvVirtAddrOut, 1, u32WriteSize, fp);
	if(s32Ret != u32WriteSize)
	{
	 	 printf("write file %s failed \n", s8PathOut);
	}
	fflush(fp);
	fclose(fp); 

 /*******************************************************************/

	if(pvVirtAddrIn)
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrIn);
	if(pvVirtAddrOut)
		AR_DSP_Free(u64PhyAddrOut, pvVirtAddrOut); 
	if(pvVirtPhyM)
		AR_DSP_Free(u64PhyM, pvVirtPhyM);    

  	printf("ar arm cv finished\n");
  
	return s32Ret;
}

