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

#define AR_SET_REG_BITS(reg,val,start_bit,end_bit) (reg)=((val)<<(start_bit))| ((reg)&~((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))
#define AR_GET_REG_BITS(reg,start_bit,end_bit) (((reg)&((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))>>(start_bit))

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

void usage()
{
	printf("usage:\n");
	printf("--help: show this help \n");
	printf("--name [name]: file name of dump file \n");
	printf("--addr [phy_addr offset len]:give the dump phy addr offset and len \n");
	printf("--reg_WRITE_APB: `WRITE_APB(32b, 32b)\n");
	printf("--reg:\n");
	printf("--reg_phy_addr: show register with register num addr and data\n");
	printf("--reg_rd_num [base,num]: show register with register base and reg num\n");
	printf("--reg_rd [addr start end]: show register with register addr\n");
	printf("--reg_wr_num [base,num,data]: write register with register base and reg num\n");
	printf("--reg_wr [addr,data,start end]: write register with register addr\n");
	printf("--dump_write_reg32:write_reg32(add,data)\n");
}

int main(int argc, char **argv)
{
    char name[64] = {0};
	FILE *fd=NULL;
	int fd_mm;
	char *addr=NULL;
	uint32_t phy_addr=0;
	uint32_t len=0;
	uint32_t add_offset=0;
	int page_num=0;
	uint32_t start =0;
	uint32_t offset=0;
	uint32_t data=0;
	int ret=0;
	int index=0;
	int start_bit=0,end_bit=0;
	index=find_pra_index_by_name("--help",argc,argv);
    if((index >0) || (argc ==1))
    {
	   usage();
	   return 0;
    }

	index=find_pra_index_by_name("--read_file",argc,argv);
    if(index >0)
    {
	   int fd_read = open(argv[index+1], O_RDONLY);
       printf("read %s %d \n",argv[index+1],fd_read);
	   if(fd_read>0)
	   {
	     close(fd_read);
	   }
	   return 0;
    }

	index=find_pra_index_by_name("--name",argc,argv);
	if(index>0)
	{
	   snprintf(name, 63, "%s", argv[index+1]);
	}else{
	   sprintf(name,"/mnt/%s","dump.bin");
	}

	index=find_pra_index_by_name("--addr",argc,argv);
	if(index>0)
	{
	    if(argc < (index+1+3))
	    {
	       printf("[err] :--addr need 3 pra !!!!!\n \n");
		   usage();
		   return 0;
	    }
		phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
		add_offset=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
		len=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
		printf("dump_mem: %s 0x%x 0x%x 0x%x......\n",name,phy_addr,add_offset,len);
	}else
	{
		index=find_pra_index_by_name("--reg_rd_num",argc,argv);
        if(index>0)
        {
			phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
			add_offset=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
			add_offset*=4;
			len=128;
        }else{
			index=find_pra_index_by_name("--reg_rd",argc,argv);
			if(index>0)
			{
				phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
				add_offset=0;
				len=128;
                start_bit=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
				end_bit=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
			}else{
				index=find_pra_index_by_name("--reg_wr",argc,argv);
				if(index>0)
				{
					phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
					data=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
					start_bit=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
					end_bit=strtoul(argv[index+4],NULL,check_data(argv[index+4]));
					add_offset=0;
					len=128;
				}else{
				       index=find_pra_index_by_name("--reg_wr_num",argc,argv);
                       if(index>0)
                       {
                  			phy_addr=strtoul(argv[index+1],NULL,check_data(argv[index+1]));
                  			add_offset=strtoul(argv[index+2],NULL,check_data(argv[index+2]));
                  			add_offset*=4;
							data=strtoul(argv[index+3],NULL,check_data(argv[index+3]));
                  			len=128;
                       }else{
					        usage();
							return 0;
                      }
				}
			}
        }
	}
    phy_addr+=add_offset;

    fd= fopen(name, "wb");
	if(!fd){
		printf("err open file %s \n",name);
		return -1;
	}
	fd_mm = open("/dev/mem", O_RDWR|O_SYNC);
    if(fd_mm<0)
	{
	   printf("err open dev/mem \n");
	   fclose(fd);
	   return -1;
	}
	start=phy_addr & ~(PAGE_SIZE-1);
	offset=phy_addr-start;
	page_num=GET_PAGE_COUNT(len);
	addr = (char *)mmap(NULL, page_num*PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd_mm,start);
    if(addr==NULL)
    {
       printf("err map addr 0x%x",phy_addr);
	   fclose(fd);
	   close(fd_mm);
	   return -1;
    }

    //printf("phy_addr=0x%x start=0x%x offset=%d addr=%p len=%d\n",phy_addr,start,offset,addr,len);

	char *addr_dest=malloc(page_num*PAGE_SIZE);
	char *addr_src=(char *)(addr+offset);

    for(int i=0;i<len;i++)
    {
       addr_dest[i]=addr_src[i];
    }

    ret = fwrite((void *)(addr_dest), 4, len>>2, fd);
	if(ret<=0)
	{
	   printf("write file err\n");
	}else
	{
	   //printf("success dump ok !!!!\n");
	}
	free(addr_dest);
    //dump_reg with format
	{
	   uint32_t value=0;
	   index=find_pra_index_by_name("--reg_WRITE_APB",argc,argv);
       if(index>0)
       {
            int i=0;
            uint32_t *p_addr=(uint32_t *)(addr+offset);
            for(i=0;i<len/4;i++)
            {
               value=p_addr[i];
               printf("`WRITE_APB(`32'h%08x, 32'h%08x);\n",phy_addr+4*i, value);
            }
       }
	   index=find_pra_index_by_name("--reg",argc,argv);
	   if(index>0)
	   {
	        int i=0;
			uint32_t *p_addr=(uint32_t *)(addr+offset);
            for(i=0;i<len/4;i++)
            {
               value=p_addr[i];
               printf("%08d(0x%08x):0x%08x\n",i,i,value);
            }
	   }
	   index=find_pra_index_by_name("--reg_phy_addr",argc,argv);
	   if(index>0)
	   {
	        int i=0;
			uint32_t *p_addr=(uint32_t *)(addr+offset);
            for(i=0;i<len/4;i++)
            {
                value=p_addr[i];
				printf("%08d(0x%08x):0x%08x:0x%08x\n",i,i,phy_addr+4*i,value);
            }
	   }

	   index=find_pra_index_by_name("--dump_write_reg32",argc,argv);
	   if(index>0)
	   {
	        int i=0;
			uint32_t *p_addr=(uint32_t *)(addr+offset);
            for(i=0;i<len/4;i++)
            {
               value=p_addr[i];
               printf("write_reg32(0x%08x,0x%08x);\n",phy_addr+4*i,value);
            }
	   }

	   index=find_pra_index_by_name("--reg_rd_num",argc,argv);
	   if(index)
	   {
		   uint32_t *p_addr=(uint32_t *)(addr+offset);
		   value=p_addr[0];
		   printf("0x%08x:0x%08x\n",phy_addr,value);
	   }
	   index=find_pra_index_by_name("--reg_rd",argc,argv);
	   if(index)
	   {
		   uint32_t *p_addr=(uint32_t *)(addr+offset);
		   value=p_addr[0];
		   printf("0x%08x:0x%08x\n",phy_addr,AR_GET_REG_BITS(value,start_bit,end_bit));
	   }
	   index=find_pra_index_by_name("--reg_wr_num",argc,argv);
	   if(index>0)
	   {
		  uint32_t *p_addr=(uint32_t *)(addr+offset);
		  p_addr[0]=data;
	   }
	   index=find_pra_index_by_name("--reg_wr",argc,argv);
	   if(index>0)
	   {
		  uint32_t *p_addr=(uint32_t *)(addr+offset);
		  value=p_addr[0];
		  AR_SET_REG_BITS(value,data,start_bit,end_bit);
		  printf("0x%08x write 0x%08x or [%d--%d write 0x%x]\n",phy_addr,value,start_bit,end_bit,AR_GET_REG_BITS(value,start_bit,end_bit));
	   }
	}
	munmap(addr, len);
	fclose(fd);
	close(fd_mm);
	return 0;
}
