#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include "hal_icc.h"


#define  get_thread_id()  ((uint32_t)pthread_self())
#define  MSG_CEVA_WITH_LINUX_FOPS_0  0xff001234
#define  FILE_NAME_SIZE 4096


typedef enum{
	FAILED = 0,
	OPEN_FILE,
	GET_FILE_LEN,
	READ_FILE,
	WRITE_FILE,
	CLOSE_FILE,
}memssage_type;


static unsigned int get_file_size(int fd)
{
	struct stat stbuf;
	fstat(fd, &stbuf);
	return stbuf.st_size;
}



void ceva_file_operation_process(void * arg)
{
	int  fd_write,file_fd,flag,recv = -1;
	unsigned int file_name_address,file_size,file_address,read_size,write_size;
	unsigned char *file_name,*buf_address,*map_address;

	int msg_id;
	int dst_core_id;
	int fd_mem;

	if(!arg){
		printf("arg is NULL!! line:%d\n", __LINE__);
		return;
	}
	int core_id = *(int *)arg;
	if(core_id == 0){
		msg_id = MSG_CEVA_WITH_LINUX_FOPS_0 + core_id;
		dst_core_id = ICC_CORE_CEVA_0 + core_id;
		printf("CEVA file ops task, core %d msg id %x\n", core_id, msg_id);
		fd_mem=open("/dev/mem", O_RDWR | O_SYNC);
	}
	else{
		printf("core is error! line:%d\n", __LINE__);
		return;
	}

	if(fd_mem < 0){
		printf("open /dev/mem fail line:%d\n", __LINE__);
		return;
	}

	int fd = ar_hal_icc_open_dev();
	if(fd < 0) {
		printf("open icc fail %d\n", __LINE__);
		return;
	}
	int ret = ar_hal_icc_register_msgid(fd, msg_id);
	if(ret < 0) {
		printf("recv register msg id fail\n");
		return;
	}

	//tell ceva that we're ready for file ops
    unsigned int msg[6];
	msg[0]='R';
	ar_hal_icc_send(fd, (unsigned char *)&msg, sizeof(msg), msg_id, dst_core_id);
	
	while(1)
	{
		memset(msg, 0, sizeof(msg));
		ret=ar_hal_icc_receive(fd, (unsigned char *)msg, sizeof(msg), msg_id);
		if(ret < 0)
		{
			//printf("receive msg id %d fail\n", msg_id);
			//return 0;
		}
		else
		{   
		    //printf("[%x] file ops=%d\n",get_thread_id(),msg[0]);
			switch(msg[0])
			{
			case OPEN_FILE:
				file_name_address=msg[2];
				file_name_address = file_name_address & (~(FILE_NAME_SIZE - 1));
				map_address = mmap(NULL,FILE_NAME_SIZE*2,PROT_READ | PROT_WRITE,MAP_SHARED,fd_mem,file_name_address);
				file_name=map_address+(msg[2] & (FILE_NAME_SIZE - 1));//for data alignment
				flag=msg[3];
				file_fd=open(file_name,flag);
				msg[2]=file_fd;
				//printf("[%x] OPen file:%s fd %d\n", get_thread_id(),file_name,file_fd);
				ar_hal_icc_send(fd, (unsigned char *)msg, sizeof(unsigned int)*6, msg_id, dst_core_id);
				munmap(map_address,FILE_NAME_SIZE*2);
				break;
			case GET_FILE_LEN:
				file_fd=msg[2];
				msg[2]=get_file_size(file_fd);
				//printf("[%x]Get file len: fd %d len=%d\n", get_thread_id(),file_fd,msg[2]);
				ar_hal_icc_send(fd, (unsigned char *)msg, sizeof(unsigned int)*6, msg_id, dst_core_id);
				break;
			case READ_FILE:
				file_size = msg[3];
				file_address = msg[4];
				map_address = mmap(NULL,(file_size/4096+2)*4096,PROT_READ | PROT_WRITE,MAP_SHARED,fd_mem,file_address&(~(FILE_NAME_SIZE - 1)));
				buf_address=map_address+(file_address & (FILE_NAME_SIZE - 1));
				file_fd = msg[2];
				if (file_fd > 0)
					read_size = read(file_fd,buf_address,file_size);
				msg[2] = read_size;

				//printf("[%x]Read file: fd %d read_size=%d req_size=%d\n",get_thread_id(), file_fd,read_size,file_size);
				ar_hal_icc_send(fd, (unsigned char *)msg, sizeof(unsigned int)*6, msg_id, dst_core_id);
				munmap(map_address,(file_size/4096+2)*4096);
				break;
			case WRITE_FILE:
				file_size = msg[3];
				file_address = msg[4];
				map_address = mmap(NULL,(file_size/4096+2)*4096,PROT_READ | PROT_WRITE,MAP_SHARED,fd_mem,file_address&(~(FILE_NAME_SIZE - 1)));

				buf_address=map_address+(file_address & (FILE_NAME_SIZE - 1));
				file_fd = msg[2];
				if (file_fd > 0)
					write_size = write(file_fd,buf_address,file_size);
				msg[2] = write_size;

				//printf("[%x]write file: fd %d write_size=%d,req_size=%d",get_thread_id(),file_fd,write_size,file_size);
				ar_hal_icc_send(fd, (unsigned char *)msg, sizeof(unsigned int)*6, msg_id, dst_core_id);
				munmap(map_address,(file_size/4096+2)*4096);
				close(fd_write);
				break;
			case CLOSE_FILE:
				file_fd = msg[2];
				msg[2] = close(file_fd);
				//printf("[%x]close file: fd %d\n",get_thread_id(), file_fd);
				ar_hal_icc_send(fd, (unsigned char *)msg, sizeof(unsigned int)*6, msg_id, dst_core_id);
				file_fd = -1;
				break;

			default:
				//printf("receive error\n");
				break;
			}
		}
	}

}

int main(int argc, char *argv[])
{
	pthread_t thread;
	pthread_attr_t attr;
	cpu_set_t cpu_info;
	int core_id = 0;

	pthread_attr_init(&attr);
	CPU_ZERO(&cpu_info);
	CPU_SET(0, &cpu_info);

	if(0 != pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpu_info))
	{
		printf("Set thread 1 affinity failed!\n");
		return -1;
	}


	if(0 != pthread_create(&thread, NULL, ceva_file_operation_process, &core_id))
	{
		printf("Create thread %d failed!\n", core_id);
		return -1;
	}
	printf("ceva file operation thread %d create\n", core_id);


	while(1)
	{
		sleep(1);
	}
}
