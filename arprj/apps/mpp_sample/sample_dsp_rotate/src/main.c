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
#include <assert.h>
#include "ar_dsp_rotate_test.h"

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
	AR_S32 s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
	if(s32Index>0 || argc==1)
	{
	   printf("-srcfile: src file name\n");
	   printf("-dstfile: dst file name\n");
	   printf("-w: src img width\n");
	   printf("-h: src img height\n");
	   printf("-s: src img stride\n");
	   printf("-flag: 0:pingpong 1:no-pingpong\n");
	   printf("spec for: YUV420P Rot90, How to use ceva Ping-Pong Buffer \n");
	   return 0;
	}


	AR_CHAR in[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-srcfile",argc,argv);
	if(s32Index>0)
	{
	   strcpy(in,argv[s32Index+1]);
	   printf("SrcFileName=%s \r\n",in);
	}
	else{
	   printf(" NO src file defined\r\n");
	   return -1;
	}
	AR_CHAR out[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-dstfile",argc,argv);
	if(s32Index>0)
	{
	   strcpy(out,argv[s32Index+1]);
	   printf("DstFileName=%s \r\n",out);
	}
	else{
	   printf(" NO dst file defined\r\n");
	   return -1;
	}


	AR_S32 w = 1920;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-w",argc,argv);
	if(s32Index>0)
	{
	   w=atoi(argv[s32Index+1]);
	   printf("w=%d \r\n",w);
	   if(w >4096 || w <0)
	   {
		   printf("w error \r\n");
		   return -1;
	   }   
	}

	AR_S32 h = 1080;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-h",argc,argv);
	if(s32Index>0)
	{
	   h=atoi(argv[s32Index+1]);
	   printf("h=%d \r\n",h);
	   if(h >2160 || h <0)
	   {
		   printf("h error \r\n");
		   return -1;
	   }
	}

	AR_S32 s = 2048;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-s",argc,argv);
	if(s32Index>0)
	{
	   s=atoi(argv[s32Index+1]);
	   printf("s=%d \r\n",s);
	   if(s >4096 || s <0)
	   {
		   printf("s error \r\n");
		   return -1;
	   }
	}

	int flag =1; //set 1: ping-pong, set 0: no ping-pong
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-flag",argc,argv);
	if(s32Index>0)
	{
	   
	   flag =atoi(argv[s32Index+1]);
	   printf("flag=%d \r\n",flag);
	}

	cvRotate_test(in, out, w, h, s, flag);
		
	return 0;
}


