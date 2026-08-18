#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "ar_dsp_boot.h"
#include "mpi_dsp_api.h"

int main(int argc,char* argv[])
{

   	int ret = 0;
	//int flag = 0;
	unsigned int dsp_id = MAX_DSP_CORE;

   	if(argc < 3)
    	{
        	printf("Usage: ar_dsp_boot [0] [/mnt/ceva0.bin]  [500,600,700,800]\r\n");
		return -1;
   	}

	dsp_id = atoi(argv[1]);
	if(dsp_id != 0)
	{
		printf("Usage: ar_dsp_boot [0] [/mnt/ceva0.bin] [500,600,700,800]\r\n");
		return -1;
	}

	printf("sirius boot ceva %d ...\n",dsp_id);


	ret = sirius_verify_ceva(argv[2]);
	if(ret < 0)
	{
		printf("ar_dsp_verify_bin file %s error \r\n",argv[2]);
		return -1;
	}


	ret = sirius_boot_ceva(argv[2],dsp_id);
	if(ret < 0)
	{
		printf("sirius boot ceva %d failed...\n",dsp_id);
		return -1;
	}

    AR_MPI_DSP_Set_Status_Phyaddr(0, 0x32400000);
	unsigned int u32Frequency = 500;
	if(argc > 3)
	{
		u32Frequency = atoi(argv[3]);
		if((u32Frequency<100)||(u32Frequency>1000))
		{
	        printf("dsp freq set abnormal, default to 500MHz \r\n");
			u32Frequency = 500;
		}
	}

	AR_MPI_DSP_SetFrequency(0, u32Frequency);

	printf("sirius boot ceva[%d] to %d MHz finished...\n",dsp_id, u32Frequency);

	sleep(1);
	AR_U32 u32TickCnt = 0;
	for(int i = 0; i < 8; i++)
	{
		AR_MPI_DSP_Get_System_Timer(&u32TickCnt, i);
		printf("system timer %d cnt value %x\n", i, u32TickCnt);
	}
	
	return 0;
}




