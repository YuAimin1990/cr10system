#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define PAGE_SIZE 4096

#define GET_PAGE_COUNT(size) (size/4096+2)


static int find_pra_index_by_name(char *name,int argc, char *argv[])
{
    int i=0;
	for(i=0;i<argc;i++)
	{
         if(!strcmp(name,argv[i]))
         {
             return i;
         }
	}
	return 0;
}

static int check_data(char *number)
{
   if(number[0]=='0'&&(number[1]=='x'||number[1]=='X'))
   {
        return 16;
   }else
   {
       return 10;
   }
}

int main(int argc, char **argv)
{
    char name[64] = { 0 };
	FILE *fd=NULL;
	int fd_mm;
	void *addr=NULL;
	uint32_t phy_addr=0;
	uint32_t len=0;
	uint32_t file_len=0;
	int page_num=0;
	uint32_t start =0;
	uint32_t offset=0;
	int ret=0;
	int index=0;
	int val=0;
	index=find_pra_index_by_name("--help",argc,argv);
	if(index>0)
    {
       printf("--help:show this help\n");
	   printf("--memset dst_addr offset val size, eg: --memset 0x80000000 0 0x5c 0x1000\n");
	   printf("--memcpy dst_addr offset src_addr offset size, eg: --memcpy 0x80000000 0 0x90000000 0 0x1000\n");
	   printf("--loadmem file.bin src_addr offset max_size, eg: --loadmem load.bin 0x90000000 0 0x1000\n");
	   return -1;
    }
    index=find_pra_index_by_name("--loadmem",argc,argv);
	if(index>0)
	{
	    int addr_offset=0;
     	snprintf(name, 63, "/mnt/%s", argv[index+1]);
     	phy_addr=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
     	addr_offset=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
		len=strtoul(argv[index+4],NULL,check_data(argv[index+4]));

        phy_addr+=addr_offset;
     	printf("load_mem: %s 0x%x 0x%x 0x%x ......\n",name,phy_addr,addr_offset,len);
         fd= fopen(name, "ab");
     	if(!fd){
     		printf("err open file %s \n",name);
     		return -1;
     	}
         fseek(fd, 0 , SEEK_END);
         file_len = ftell(fd);
         fseek(fd, 0 , SEEK_SET);
     	 fd_mm = open("/dev/mem", O_RDWR | O_SYNC);
         if(fd_mm<0)
     	{
     	   printf("err open dev/mem \n");
     	   fclose(fd);
     	   return -1;
     	}
     	start=phy_addr & ~(PAGE_SIZE-1);
     	offset=phy_addr-start;
     	page_num=GET_PAGE_COUNT(len);
     	addr = mmap(NULL, page_num*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd_mm,start);
         if(addr==NULL)
         {
            printf("err map addr 0x%x",phy_addr);
     	   fclose(fd);
     	   close(fd_mm);
     	   return -1;
         }
         len = (file_len>len)?len:file_len;
         ret = fread(addr+offset, 4, len>>2, fd);
     	if(ret<=0)
     	{
     	   printf("write file err\n");
     	}else
     	{
     	   printf("success dump ok !!!!");
     	}
     	munmap(addr, len);
     	fclose(fd);
     	close(fd_mm);
	}
	index=find_pra_index_by_name("--memset",argc,argv);
	if(index>0)
	{
	    int addr_offset=0;
     	phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
     	addr_offset=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
		val=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
		len=strtoul(argv[index+4],NULL,check_data(argv[index+4]));
        phy_addr+=addr_offset;
		fd_mm = open("/dev/mem", O_RDWR | O_SYNC);
        if(fd_mm<0)
     	{
     	   printf("err open dev/mem \n");
     	   if(fd)
	     	   fclose(fd);
     	   return -1;
     	}
		start=phy_addr & ~(PAGE_SIZE-1);
     	offset=phy_addr-start;
     	page_num=GET_PAGE_COUNT(len);
     	addr = mmap(NULL, page_num*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd_mm,start);
         if(addr==NULL)
         {
           printf("err map addr 0x%x",phy_addr);
     	   if(fd)
	     	   fclose(fd);
     	   close(fd_mm);
     	   return -1;
        }
		memset(addr+offset,val,len);
		munmap(addr, len);
		if(fd)
			fclose(fd);
     	close(fd_mm);
	}
    index=find_pra_index_by_name("--memcpy",argc,argv);
	if(index>0)
	{
	    int addr_offset=0;
		int phy_addr_src=0;
		int addr_offset_src=0;
		int offset_src=0;
		void *addr_src=NULL;
     	phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
     	addr_offset=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
     	phy_addr_src=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
     	addr_offset_src=strtoul(argv[index+4],NULL,check_data(argv[index+4]));
		len=strtoul(argv[index+5],NULL,check_data(argv[index+5]));
        phy_addr+=addr_offset;
		phy_addr_src+=addr_offset_src;
		fd_mm = open("/dev/mem", O_RDWR | O_SYNC);
        if(fd_mm<0)
     	{
     	   printf("err open dev/mem \n");
     	   if(fd)
	     	   fclose(fd);
     	   return -1;
     	}
		start=phy_addr & ~(PAGE_SIZE-1);
     	offset=phy_addr-start;
     	page_num=GET_PAGE_COUNT(len);
     	addr = mmap(NULL, page_num*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd_mm,start);
        if(addr==NULL)
        {
           printf("err map addr 0x%x",phy_addr);
     	   if(fd)
	     	   fclose(fd);
     	   close(fd_mm);
     	   return -1;
        }

		start=phy_addr_src & ~(PAGE_SIZE-1);
     	offset_src=phy_addr_src-start;
     	page_num=GET_PAGE_COUNT(len);
     	addr_src = mmap(NULL, page_num*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd_mm,start);
        if(addr_src==NULL)
        {
           printf("err map addr 0x%x",phy_addr);
     	   if(fd)
	     	   fclose(fd);
     	   close(fd_mm);
     	   return -1;
        }
		memcpy(addr+offset,addr_src+offset_src,len);
		munmap(addr, len);
		munmap(addr_src, len);
		if(fd)
			fclose(fd);
     	close(fd_mm);
	}
	return 0;
}
