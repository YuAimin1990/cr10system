#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hal_adc.h"
#include "hal_sys.h"

#define LOG_PRINT(level, module, fmt...)\
    do {\
        printf("[%s] [%s] [%s %s():%d] ", level, module, __FILE__, __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)

#define LOG_ERR(...) LOG_PRINT("ERROR", "ADC", __VA_ARGS__);
#define LOG_INFO(...) LOG_PRINT("INFO ", "ADC", __VA_ARGS__);
#define LOG_DBG(...) LOG_PRINT("DEBUG", "ADC", __VA_ARGS__);

#define ADC_ADJUST_A            (2024 - 0)
#define ADC_ADJUST_B            (0x3ff - 0x0)
#define ADC_ADJUST_C            (0)

int usage(AR_CHAR* argv0)
{
    printf("Usage: %s <chn> [a] [b] [c]\n", argv0);
    printf("        chn: ADC channel, range [0, 7]\n");
    printf("        a b c: The parameters of the voltage adjustment calculation\n");
    return 0;
}

int adc_init()
{
    return ar_hal_adc_enable();
}

int adc_deinit()
{
    return ar_hal_adc_disable();
}

int main(AR_S32 argc, AR_CHAR* argv[])
{
    if(argc != 2 && argc != 5)
    {
        usage(argv[0]);
        return -1;
    }

    int chn = atoi(argv[1]);
    if(chn > ADC_CHN_7)
    {
        printf("chn %d out of range [0, 7]\n", chn);
        usage(argv[0]);
        return -1;
    }

    int a = 0, b = 0, c = 0;
    AR_BOOL bAdjust = AR_FALSE;
    if(argc == 5){
        a = atoi(argv[2]);
        b = atoi(argv[3]);
        c = atoi(argv[4]);
        bAdjust = AR_TRUE;
    }

    AR_S32 s32Ret = AR_FAILURE;
    s32Ret = adc_init();
    if (s32Ret == AR_SUCCESS)
    {
        LOG_INFO("ar_hal_adc_enable success.\n");
    } else {
        LOG_ERR("ar_hal_adc_enable failed!\n");
        return s32Ret;
    }

    LOG_INFO("======== ADC channel %d ========\n", chn);
    s32Ret = ar_hal_adc_get_raw(chn);
    if (s32Ret < 0)
    {
        LOG_ERR("ar_hal_adc_get_raw channel %d failed, ret=%#X\n", chn, s32Ret);
        goto END;
    } else {
        LOG_INFO("ar_hal_adc_get_raw channel %d success, raw value = %d\n", chn, s32Ret);
    }

    /*
        Note: millivolt = raw * a / b + c
    */
    s32Ret = ar_hal_adc_get_millivolt(chn);
    if (s32Ret < 0)
    {
        LOG_ERR("ar_hal_adc_get_millivolt channel %d failed, ret=%#X\n", chn, s32Ret);
        goto END;
    } else {
        LOG_INFO("ar_hal_adc_get_millivolt channel %d success, voltage = %d mV\n", chn, s32Ret);
    }

    if(bAdjust)
    {
        s32Ret = ar_hal_adc_get_millivolt_by_adjust(chn, a, b, c);
        if (s32Ret < 0)
        {
            LOG_ERR("ar_hal_adc_get_millivolt_by_adjust channel %d failed, ret=%#X\n", chn, s32Ret);
            goto END;
        } else {
            LOG_INFO("ar_hal_adc_get_millivolt_by_adjust channel %d success, a=%d b=%d c=%d, adjusted voltage = %d mV\n", chn, a, b, c, s32Ret);
        }
    } else {
        s32Ret = ar_hal_adc_get_millivolt_by_adjust(chn, ADC_ADJUST_A, ADC_ADJUST_B, ADC_ADJUST_C);
        if (s32Ret < 0)
        {
            LOG_ERR("ar_hal_adc_get_millivolt_by_adjust channel %d failed, ret=%#X\n", chn, s32Ret);
            goto END;
        } else {
            LOG_INFO("ar_hal_adc_get_millivolt_by_adjust channel %d success, default a=%d b=%d c=%d, adjusted voltage = %d mV\n", chn, ADC_ADJUST_A, ADC_ADJUST_B, ADC_ADJUST_C, s32Ret);
        }
    }

END:
    s32Ret = adc_deinit();
    if (s32Ret == AR_SUCCESS)
    {
        LOG_INFO("ar_hal_adc_disable success.\n");
    } else {
        LOG_ERR("ar_hal_adc_disable failed!\n");
    }

    return s32Ret;
}
