#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "utils_debug.h"
#include "binder_ipc.h"

static int total_len = 0;
static char cmd_buf[1024];
static char binder_server[128];

void rtcmd_thread_func(void)
{
	ar_ipc_state_t* ipc_binder=ar_ipc_create();
	uint32_t binder_handle=0;
	if(ipc_binder){
	   binder_handle=ar_ipc_get_service(ipc_binder,binder_server);
	   if(!binder_handle)
	   {
		  printf("get binder server failed\n");
		  ar_ipc_close(ipc_binder);
		  return -1;
	   }
	}else
	{
	   printf("creat ipc failed \n");
	   return -1;
	}
	int reply=0;
    int ret = ar_ipc_send_buffer(ipc_binder, binder_handle, 0xbdc, cmd_buf,total_len, &reply, 0);
	ar_ipc_close(ipc_binder);
	return ret;
}

int main(int argc, char **argv)
{
	//char *cmd_buf;
	int len;

	if(argc < 2) {
		log_tag_err("please input the command\n");
		return -1;
	}
    strcpy(binder_server,argv[1]);
	
	for(int i=1; i<argc; i++) {
		len = strlen(argv[i]);
		strcpy((char *)&cmd_buf[total_len],argv[i]);
		total_len+=len;
		cmd_buf[total_len]=' ';
		total_len++;
	}
	cmd_buf[total_len]='\0';
	total_len++;
	printf("%s \n",cmd_buf);

	int pid = fork();
	if(pid <0 ) {
		printf("internel error\n");
	} else if (pid == 0) 
    {
		rtcmd_thread_func();
	}
}

