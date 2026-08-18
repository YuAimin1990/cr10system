#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "pwm_operation.h"


int pwm_export(ENUM_PWM_PORT pwm_num)
{
	int pwm_exp_fd;
	char cmd[16] = "";

	pwm_exp_fd = open(__PATH_OF_PWM_EXPORT__, O_WRONLY);
	if(-1 == pwm_exp_fd)
	{
		printf("sysfs:%s open failed!\n", __PATH_OF_PWM_EXPORT__);
		return -1;
	}

	sprintf(cmd, "%d", pwm_num);
	if(strlen(cmd) == write(pwm_exp_fd, cmd, strlen(cmd)))
	{
		close(pwm_exp_fd);
		return pwm_num;
	}
	else if(pwm_num == pwm_unexport(pwm_num))
	{
		//printf("try open again\n");
		if(strlen(cmd) == write(pwm_exp_fd, cmd, strlen(cmd)))
		{
			close(pwm_exp_fd);
			return pwm_num;
		}
	}
	
	printf("pwm export failed!\n");
	close(pwm_exp_fd);
	return -1;
}

int pwm_period_set(ENUM_PWM_PORT pwm_num, unsigned int period_ns)
{
	int pwm_per_fd;
	char cmd[16] = "";
	char dir_path[256] = "";
	int ret;

	if(period_ns < __MIN_NS_OF_PWM__)
	{
		printf("period %dns is too small to set!\n", period_ns);
		return -1;
	}

	sprintf(cmd, "%u", period_ns);

	sprintf(dir_path, __PATH_OF_PWM_SYSFS__"pwm%d/period", pwm_num);
	pwm_per_fd = open(dir_path, O_RDWR);
	if(-1 == pwm_per_fd)
	{
		printf("sysfs:%s open failed!\n", dir_path);
		return -1;
	}

	//clear duty_cycle
	if(0 != pwm_duty_cycle_get(pwm_num) && pwm_duty_cycle_set(pwm_num, 0) < 0)
	{
		printf("pwm_period_set clear duty cycle failed!\n");
		return -1;
	}

	ret = write(pwm_per_fd, cmd, strlen(cmd));
	if(strlen(cmd) == ret)
	{
		close(pwm_per_fd);
		return 0;
	}
	else
	{
		printf("pwm_period_set write failed(%d)!\n", ret);
		close(pwm_per_fd);
		return -1;
	}
}

long long int pwm_period_get(ENUM_PWM_PORT pwm_num)
{
	int pwm_per_fd;
	char cmd[16] = "";
	char dir_path[256] = "";
	int ret;

	sprintf(dir_path, __PATH_OF_PWM_SYSFS__"pwm%d/period", pwm_num);
	pwm_per_fd = open(dir_path, O_RDWR);
	if(-1 == pwm_per_fd)
	{
		printf("sysfs:%s open failed!\n", dir_path);
		return -1;
	}

	ret = read(pwm_per_fd, cmd, sizeof(cmd));
	if(ret < 0)
	{
		printf("read failed!\n");
		close(pwm_per_fd);
		return -1;
	}

	close(pwm_per_fd);

	return strtoll(cmd, 0, 0);
}

long long int pwm_duty_cycle_get(ENUM_PWM_PORT pwm_num)
{
	int pwm_duty_fd;
	char cmd[16] = "";
	char dir_path[256] = "";
	int ret;
	
	sprintf(dir_path, __PATH_OF_PWM_SYSFS__"pwm%d/duty_cycle", pwm_num);
	pwm_duty_fd = open(dir_path, O_RDWR);
	if(-1 == pwm_duty_fd)
	{
		printf("sysfs:%s open failed!\n", dir_path);
		return -1;
	}

	ret = read(pwm_duty_fd, cmd, sizeof(cmd));
	if(ret < 0)
	{
		printf("read failed!\n");
		close(pwm_duty_fd);
		return -1;
	}

	close(pwm_duty_fd);

	return strtoll(cmd, 0, 0);
}

int pwm_duty_cycle_set(ENUM_PWM_PORT pwm_num, unsigned int duty_ns)
{
	int pwm_duty_fd;
	char cmd[16] = "";
	char dir_path[256] = "";
	int ret;

	if(duty_ns < __MIN_NS_OF_PWM__)
	{
		printf("duty cycle %uns is too small to set!\n", duty_ns);
		return -1;
	}

	if(duty_ns > pwm_period_get(pwm_num))
	{
		printf("duty cycle %uns can not bigger than period %lldns!\n", duty_ns, pwm_period_get(pwm_num));
		return -1;
	}

	sprintf(cmd, "%u", duty_ns);

	sprintf(dir_path, __PATH_OF_PWM_SYSFS__"pwm%d/duty_cycle", pwm_num);
	pwm_duty_fd = open(dir_path, O_RDWR);
	if(-1 == pwm_duty_fd)
	{
		printf("sysfs:%s open failed!\n", dir_path);
		return -1;
	}

	ret = write(pwm_duty_fd, cmd, strlen(cmd));
	if(strlen(cmd) == ret)
	{
		close(pwm_duty_fd);
		return 0;
	}
	else
	{
		printf("pwm_duty_cycle_set write failed(%d)!\n", ret);
		close(pwm_duty_fd);
		return -1;
	}
}

int pwm_duty_cycle_percent_set(ENUM_PWM_PORT pwm_num, unsigned char percent)
{
	long long period_ns;
	unsigned int duty_ns;

	if(percent > 100)
	{
		printf("percent(%u) can not bigger than 100!\n", percent);
		return -1;
	}

	period_ns = pwm_period_get(pwm_num);
	period_ns = (period_ns * percent / 100);

	duty_ns = (unsigned int)period_ns;
	return pwm_duty_cycle_set(pwm_num, duty_ns);
}

unsigned int pwm_enable_get(ENUM_PWM_PORT pwm_num)
{
	int pwm_enable_fd;
	char cmd[16] = "";
	char dir_path[256] = "";
	int ret;

	sprintf(dir_path, __PATH_OF_PWM_SYSFS__"pwm%d/enable", pwm_num);
	pwm_enable_fd = open(dir_path, O_RDWR);
	if(-1 == pwm_enable_fd)
	{
		printf("sysfs:%s open failed!\n", dir_path);
		return -1;
	}

	ret = read(pwm_enable_fd, cmd, sizeof(cmd));
	if(ret < 0)
	{
		printf("read failed!\n");
		close(pwm_enable_fd);
		return -1;
	}

	close(pwm_enable_fd);
	return strtoul(cmd, 0, 0);
}

int pwm_enable_set(ENUM_PWM_PORT pwm_num, unsigned int enable)
{
	int pwm_enable_fd;
	char cmd[16] = "";
	char dir_path[256] = "";
	int ret;

	if(enable == 0)
	{
		sprintf(cmd, "0");
	}
	else
	{
		enable = 1;
		sprintf(cmd, "1");
	}

	if(enable == pwm_enable_get(pwm_num))
		return 0;

	sprintf(dir_path, __PATH_OF_PWM_SYSFS__"pwm%d/enable", pwm_num);
	pwm_enable_fd = open(dir_path, O_RDWR);
	if(-1 == pwm_enable_fd)
	{
		printf("sysfs:%s open failed!\n", dir_path);
		return -1;
	}	

	ret = write(pwm_enable_fd, cmd, strlen(cmd));
	if(strlen(cmd) == ret)
	{
		close(pwm_enable_fd);
		return 0;
	}
	else
	{
		printf("pwm_enable_set write failed(%d)!\n", ret);
		close(pwm_enable_fd);
		return -1;
	}
}

int pwm_unexport(ENUM_PWM_PORT pwm_num)
{
	int pwm_unexp_fd;
	char cmd[16] = "";

	pwm_unexp_fd = open(__PATH_OF_PWM_UNEXPORT__, O_WRONLY);
	if(-1 == pwm_unexp_fd)
	{
		printf("sysfs:%s open failed!\n", __PATH_OF_PWM_UNEXPORT__);
		return -1;
	}

	sprintf(cmd, "%d", pwm_num);
	if(strlen(cmd) == write(pwm_unexp_fd, cmd, strlen(cmd)))
	{
		close(pwm_unexp_fd);
		return pwm_num;
	}
	else
	{
		close(pwm_unexp_fd);
		return -1;
	}
}
