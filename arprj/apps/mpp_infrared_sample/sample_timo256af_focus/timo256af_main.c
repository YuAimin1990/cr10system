#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>

#include "pwm_operation.h"

static ENUM_PWM_PORT PWM_PORT_IN2 = PWM_4;
static ENUM_PWM_PORT PWM_PORT_SD =  PWM_5;

int mode_0(ENUM_PWM_PORT pwm_num, unsigned int period, unsigned int duty)
{
	if(pwm_enable_set(pwm_num, 0) < 0)
	{
		printf("mode_0 disable pwm failed!\n");
		return -1;
	}

	if(pwm_period_set(pwm_num, period) < 0)
	{
		printf("mode_0 set period failed!\n");
		return -1;
	}

	if(pwm_duty_cycle_set(pwm_num, duty) < 0)
	{
		printf("mode_0 set duty cycle failed!\n");
		return -1;
	}

	if(pwm_enable_set(pwm_num, 1) < 0)
	{
		printf("mode_0 enable pwm failed!\n");
		return -1;
	}
	return 0;
}

int mode_1(ENUM_PWM_PORT pwm_num, unsigned int period, unsigned char percent)
{
	if(pwm_enable_set(pwm_num, 0) < 0)
	{
		printf("mode_1 disable pwm failed!\n");
		return -1;
	}

	if(pwm_period_set(pwm_num, period) < 0)
	{
		printf("mode_1 set period failed!\n");
		return -1;
	}

	if(pwm_duty_cycle_percent_set(pwm_num, percent) < 0)
	{
		printf("mode_1 set duty cycle percent failed!\n");
		return -1;
	}

	if(pwm_enable_set(pwm_num, 1) < 0)
	{
		printf("mode_1 enable pwm failed!\n");
		return -1;
	}
	return 0;
}

unsigned char g_run_flag = 1;

//capture exit signal
static void sighandler(int signo)
{
	if (signo == SIGINT || signo == SIGQUIT || signo == SIGKILL || signo == SIGTERM)
	{
		g_run_flag = 0;
	}
}

int mode_2(ENUM_PWM_PORT pwm_num, unsigned int period)
{
	unsigned char percent = 0;
	while(g_run_flag)
	{
		mode_1(pwm_num, period, percent);
		percent = ((percent + 1) % 100);
		usleep(1000 * 1000);
	}

	return 0;
}

static void usage(const char *argv0)
{
	printf("Usage: %s [options]\n", argv0);
	printf("Available options are\n");
	printf(" -m N                      Select mode, 0 is far, 1 is close\n");
	printf(" -n N                      Select driver motor count, >=1\n");
}

int set_8841sd_out(const unsigned char enable)
{
    int ret;
    if(pwm_export(PWM_PORT_SD) < 0)
	{
		printf("PWM_PORT_SD export failed!\n");
		return -1;
	}

    if(enable)
        ret = mode_0(PWM_PORT_SD, 40*1000*1000, 20*1000*1000);
    else
        ret = pwm_enable_set(PWM_PORT_SD, 0);

    return ret;
}

int set_8841in2_out(const unsigned char enable,unsigned char percent)
{
    int ret;
    if(pwm_export(PWM_PORT_IN2) < 0)
	{
		printf("PWM_PORT_IN2 export failed!\n");
		return -1;
	}

    if(enable)
        ret = mode_1(PWM_PORT_IN2, 8000, percent);
    else
        ret = pwm_enable_set(PWM_PORT_IN2, 0);

    return ret;
}


int main(int argc , char *argv[])
{
	int 		mode = 0;
	int         drv_cnt = 1;
	unsigned char percent = 25;
	int		    opt = 0;
	int aret[4] = {0};
	struct sigaction    action;

	while ((opt = getopt(argc, argv, "m:n:h")) != -1)
	{
		switch (opt)
		{
			case 'm':
				mode = atoi(optarg);
				if(mode < 0 || mode > 3)
				{
					printf("mode out of range!\n");
					return -1;
				}
				break;
		    case 'n':
				drv_cnt = atoi(optarg);
				if(drv_cnt <= 0)
				{
					printf("count out of range!\n");
					return -1;
				}
				break;
			case 'h':
				usage(argv[0]);
				return 0;
			default :
				usage(argv[0]);
				return 0;
		}
	}

	action.sa_handler = sighandler;
	action.sa_flags = 0;

    sigaction (SIGINT, &action, NULL);
    sigaction (SIGKILL, &action, NULL);
    sigaction (SIGTERM, &action, NULL);

    printf("pwm port, sd=%d in2=%d\n",PWM_PORT_SD,PWM_PORT_IN2);
    if(pwm_export(PWM_PORT_SD) < 0)
	{
		printf("PWM_PORT_SD export failed!\n");
		return -1;
	}
	
	if(pwm_export(PWM_PORT_IN2) < 0)
	{
		printf("PWM_PORT_IN2 export failed!\n");
		return -1;
	}

	set_8841sd_out(0);

	if(0 == mode)
	{
	    printf("far focal, cnt=%d\n",drv_cnt);
	    percent = 25;
	}
	else if(1 == mode)
	{
	    printf("close focal, cnt=%d\n",drv_cnt);
	    percent = 75;
	}
	else
	{
		printf("no test mode match!\n");	
	}

	printf("wait..\n");
	aret[0] = set_8841in2_out(1,percent);
	aret[1] = set_8841sd_out(1);

	//usleep(drv_cnt * 40 * 1000);
	while(drv_cnt-- && g_run_flag)
	{
	    usleep(40 * 1000);
	}

	aret[2] = set_8841sd_out(0);
	aret[3] = set_8841in2_out(0,percent);
	printf("end return= %d %d %d %d\n",aret[0],aret[1],aret[2],aret[3]);

//EXIT:
    pwm_unexport(PWM_PORT_SD);
    pwm_unexport(PWM_PORT_IN2);
	return 0;
}
