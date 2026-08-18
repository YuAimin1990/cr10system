#ifndef __AR_ADC_H__
#define __AR_ADC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum
{
	ADC_CHN_0 = 0,
	ADC_CHN_1,
	ADC_CHN_2,
	ADC_CHN_3,
	ADC_CHN_4,
	ADC_CHN_5,
	ADC_CHN_6,
	ADC_CHN_7,
	ADC_CHN_8,
	ADC_CHN_9,
	ADC_CHN_10,
	ADC_CHN_11,
	ADC_CHN_12,
	ADC_CHN_13,
	ADC_CHN_14,
	ADC_CHN_15,
	ADC_CHN_MAX
}ENUM_ADC_CHN;

int ar_hal_adc_enable();
int ar_hal_adc_disable();
int ar_hal_adc_get_raw(ENUM_ADC_CHN chn);
int ar_hal_adc_get_millivolt(ENUM_ADC_CHN chn);
//voltage = a * raw / b + c
int ar_hal_adc_get_millivolt_by_adjust(ENUM_ADC_CHN chn, int a, int b, int c);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__AR_ADC_H__
