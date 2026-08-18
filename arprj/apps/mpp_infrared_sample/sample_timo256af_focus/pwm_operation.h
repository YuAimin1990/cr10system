#include <stdio.h>
#include <stdlib.h>

#define __PATH_OF_PWM_SYSFS__		"/sys/class/pwm/pwmchip0/"
#define __PATH_OF_PWM_EXPORT__		__PATH_OF_PWM_SYSFS__"export"
#define __PATH_OF_PWM_UNEXPORT__	__PATH_OF_PWM_SYSFS__"unexport"

#define __MIN_NS_OF_PWM__		(0)//(1000000000/60000000)
#define __MAX_NS_OF_PWM__		(1 << 32 - 1)

typedef enum
{
	PWM_0 = 0,
	PWM_1,
	PWM_2,
	PWM_3,
	PWM_4,
	PWM_5,
	PWM_6,
	PWM_7,
	PWM_MAX
}ENUM_PWM_PORT;

int pwm_export(ENUM_PWM_PORT pwm_num);
int pwm_period_set(ENUM_PWM_PORT pwm_num, unsigned int period_ns);
long long int pwm_period_get(ENUM_PWM_PORT pwm_num);
long long int pwm_duty_cycle_get(ENUM_PWM_PORT pwm_num);
int pwm_duty_cycle_set(ENUM_PWM_PORT pwm_num, unsigned int duty_ns);
int pwm_duty_cycle_percent_set(ENUM_PWM_PORT pwm_num, unsigned char percent);
int pwm_enable_set(ENUM_PWM_PORT pwm_num, unsigned int enable);
int pwm_unexport(ENUM_PWM_PORT pwm_num);
