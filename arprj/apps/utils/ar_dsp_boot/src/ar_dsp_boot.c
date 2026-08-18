#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "mpi_dsp_api.h"
#include "hal_sys.h"
#include "ar_dsp_boot.h"

AR_S32 ar_read_file_len(AR_CHAR* filepath)
{
	AR_S32 len;
	AR_S32 fd = open(filepath,O_RDONLY);
	if(fd<0)
	{
		printf("ar_read_file_size error!\r\n");
		return -1;
	}

    	len = lseek(fd,0,SEEK_END);
	close(fd);
	return len;
}


AR_S32 ar_read_file(AR_CHAR* addr,AR_CHAR *filepath)
{
	AR_S32 nbytes,nread;
	//struct stat buf;
	AR_S32 fd;
	nbytes = ar_read_file_len(filepath);

	fd = open(filepath,O_RDONLY);
	if(fd<0)
	{
	   	printf("open file error!\r\n");
	   	return -1;
	}

	nread = read(fd,addr,nbytes);
	if(nread<0)
	{
	   	printf("read error! nread %d erron %d\r\n",nread,errno );
	   	close(fd);
	   	return -1;
	}
	close(fd);
	return nread;
}

int sirius_verify_ceva(char* filepath)
{
    int map_size = ar_read_file_len(filepath);
	if(map_size <=0)
	{
        printf("%s read file lens error!\r\n",filepath);
		return -1;
	}

    AR_U64 u64ImgAddrPhy = 0;
    void * ptrImgAddrVirt = NULL;
    AR_S32 s32Ret = ar_hal_sys_mmz_alloc(&u64ImgAddrPhy, &ptrImgAddrVirt, "ar_dsp_check", NULL, sizeof(unsigned long) * map_size);
    if(s32Ret < 0)
    {
        printf("mzz malloc ar_dsp failed \r\n");
        return -1;
    }


    s32Ret= ar_read_file((char*)ptrImgAddrVirt, filepath);
    if(s32Ret < 0)
    {
        ar_hal_sys_mmz_free((AR_U64)u64ImgAddrPhy, (void *)ptrImgAddrVirt);
        printf("read ceva bin/img file failed!\r\n");
        return -1;
    }

    if( *(AR_U32*)ptrImgAddrVirt != CEVA_BIN_HEADER)
    {
    	printf("verify ceva bin file header %x != %x failed!\r\n",*(AR_U32*)ptrImgAddrVirt,CEVA_BIN_HEADER);
		ar_hal_sys_mmz_free((AR_U64)u64ImgAddrPhy, (void *)ptrImgAddrVirt);
    	return -1;
    }

	ar_hal_sys_mmz_free((AR_U64)u64ImgAddrPhy, (void *)ptrImgAddrVirt);

    return 0;
}


int sirius_boot_ceva(char* filepath,unsigned int dsp_id)
{
	int ret = 0;

	AR_MPI_DSP_PowerOn(dsp_id);
	ret = AR_MPI_DSP_LoadBin(dsp_id,filepath);
	if(ret < 0)
	{
	    AR_MPI_DSP_PowerOff(dsp_id);
		printf("dsp load bin fail!\n");
		return -1;
	}
       AR_MPI_DSP_EnableCore(dsp_id);
	printf("dsp load bin success!\r\n");

	return ret;

}
