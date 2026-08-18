#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hal_wdt.h"


void print_usage(char * name)
{
	printf("\r\n");
	printf("Usage: %s [options]\r\n", name);
	printf("Options:\r\n");
	printf("-t: set wdt timeout(s) defualt: 10s   0: mean system restart\r\n");

	printf("Examples:\r\n");
	printf(" %s -t 10\r\n", name);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int timeout = 10;
	int opt = 0;
	int cur_timeout = 0;

	if(argc < 2)
	{
		print_usage(argv[0]);
		return -1;
	}

	while((opt = getopt(argc, argv, "t:")) != -1)
	{
		switch(opt)
		{
			case 't':
				timeout = atoi(optarg);
				break;

			default:
				print_usage(argv[0]);
				break;
		}
	}

	ret = ar_hal_wdt_start();
	if (ret) {
		fprintf(stderr, "watchdog start error \n");
		exit(EXIT_FAILURE);
	}

#if 1
	if (!timeout) {
		printf("system restart by wdt \n");
        timeout = 1;
		ar_hal_wdt_set_timeout_threshhold(&timeout);
		return 0;
	}
#endif

	fprintf(stdout, "get wdt timeout %d s \n", timeout);

	ret = ar_hal_wdt_get_timeout_threshhold(&cur_timeout);
	if (ret == 0)
		fprintf(stdout, "get wdt timeout %d s \n", cur_timeout);
	else
		fprintf(stderr, "get wdt timeout ret = %d \n", ret);

	ret = ar_hal_wdt_set_timeout_threshhold(&timeout);
	if (ret == 0)
		fprintf(stdout, "set wdt timeout %d s \n", timeout);
	else
		fprintf(stderr, "set wdt timeout ret = %d \n", ret);

	ret = ar_hal_wdt_get_timeout_threshhold(&cur_timeout);
	if (ret == 0)
		fprintf(stdout, "get wdt timeout %d s \n", cur_timeout);
	else
		fprintf(stderr, "get wdt timeout ret = %d \n", ret);

	while (1) {
		ret = ar_hal_wdt_feed();
		sleep(1);
	}

	return ret;
}
