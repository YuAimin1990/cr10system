#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils_dbglog.h"

#define GET_LOG_BUF_SIZE 512

int main(int argc, char **argv)
{
	int ret = 0;
	ret = ar_log_init();
	if(0 != ret){
		printf("log init failed!\n");
		return -1;
	}

	static char poll_get_buf[GET_LOG_BUF_SIZE];
	size_t get_log_size = 0;
	while(1){
		get_log_size = ar_log_shm_get_log(poll_get_buf, GET_LOG_BUF_SIZE);
		if (get_log_size) {
			printf("%.*s", (int)get_log_size, poll_get_buf);
		}else{
			usleep(500*1000);
		}
	}

	ar_log_close();

	return 0;
}

