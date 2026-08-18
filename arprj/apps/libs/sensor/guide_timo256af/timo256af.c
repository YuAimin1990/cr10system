#if !defined(__TIMO256_CMOS_H_)
#define __TIMO256_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ar_comm_sns.h"
#include "ar_comm_video.h"
#include "ar_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "hal_gpio.h"
#include "hal_i2c.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define TIMO256AF_ID 2256

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pasttimo256af[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define timo256af_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pasttimo256af[dev])
#define timo256af_SENSOR_SET_CTX(dev, pstCtx)   (g_pasttimo256af[dev] = pstCtx)
#define timo256af_SENSOR_RESET_CTX(dev)         (g_pasttimo256af[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_auntimo256afBusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = { .s8I2cDev = 3},
    [1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

static const unsigned int timo256af_i2c_addr = 0x45;

static int g_fd[ISP_MAX_PIPE_NUM] = {
    [0 ... (ISP_MAX_PIPE_NUM - 1)] = -1
};

#if 1
static int timo256af_i2c_init(VI_PIPE ViPipe)
{
    AR_U8 u8DevNum;

    if (g_fd[ViPipe] >= 0)
        return AR_SUCCESS;

#ifdef AR_GPIO_I2C
    g_fd[ViPipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[ViPipe] < 0) {
        ISP_TRACE(AR_DBG_ERR, "Open gpioi2c_ex error!\n");
        return AR_FAILURE;
    }
#else
    u8DevNum = g_auntimo256afBusInfo[ViPipe].s8I2cDev;

    g_fd[ViPipe] = ar_hal_i2c_open(u8DevNum);
    if (g_fd[ViPipe] < 0) {
        ISP_TRACE(AR_DBG_ERR, "Open i2c-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }
#endif

    return AR_SUCCESS;
}

int timo256af_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0) {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }

    return AR_FAILURE;
}

int timo256af_read_register(VI_PIPE ViPipe, unsigned int addr, unsigned int *data)
{
    return ar_hal_i2c_master_read(g_fd[ViPipe], timo256af_i2c_addr,
                                  8, (uint8_t *)&addr, 1, (uint8_t *)data, 1);
}

int timo256af_write_register(VI_PIPE ViPipe, unsigned int addr, unsigned int data)
{
    unsigned char buf[8];

    buf[0] = (unsigned char)addr;
    buf[1] = (unsigned char)data;

    return ar_hal_i2c_master_write(g_fd[ViPipe],
        timo256af_i2c_addr, 8, buf, 2);
}
#endif

static AR_S32 timo256af_power_on(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
#if 0
	int power1_gpio;
    int power2_gpio;
    int reset_gpio;

    ar_always("reset_gpio %d %d %d \n",p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);
    ar_always("power_gpio %d %d %d \n",p_dev_power_attr->power_gpio[0],p_dev_power_attr->power_gpio[1],p_dev_power_attr->power_gpio[2]);
    ar_always("common_gpio %d %d %d \n",p_dev_power_attr->common_gpio[0],p_dev_power_attr->common_gpio[1],p_dev_power_attr->common_gpio[2]);

    power1_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0], p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    power2_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->common_gpio[0],p_dev_power_attr->common_gpio[1],p_dev_power_attr->common_gpio[2]);
    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);

    printf("timo256af_power_on: power_gpio = %d %d, reset_gpio = %d\n", power1_gpio,power2_gpio,reset_gpio);

    ar_hal_gpio_export(power1_gpio);
    ar_hal_gpio_export(power2_gpio);
    ar_hal_gpio_export(reset_gpio);
    
    ar_hal_gpio_set_dir(power1_gpio, 1);     //set the gpio dir to out
    ar_hal_gpio_set_dir(power2_gpio, 1);
    ar_hal_gpio_set_dir(reset_gpio, 1);

    ar_hal_gpio_set_value(power1_gpio, 1);
    ar_hal_gpio_set_value(power2_gpio, 1);
    usleep(5000);
    ar_hal_gpio_set_value(reset_gpio, 0);
    usleep(10000);
    ar_hal_gpio_set_value(reset_gpio, 1);
#endif
    printf("timo256af_power_on enter\n");

    int gpio_1v8en, gpio_3v3en, gpio_vdda, gpio_vdet, gpio_reset, gpio_sleep, gpio_addrx, gpio_8841en, gpio_8841sd;
    gpio_1v8en = ar_hal_gpio_name_to_num(3,0,0);
    gpio_3v3en = ar_hal_gpio_name_to_num(1,0,0);
    gpio_vdda = ar_hal_gpio_name_to_num(3,0,1);
    gpio_vdet = ar_hal_gpio_name_to_num(3,0,2);
    gpio_reset = ar_hal_gpio_name_to_num(3,0,3);
    gpio_sleep = ar_hal_gpio_name_to_num(0,1,4);
    gpio_addrx = ar_hal_gpio_name_to_num(0,1,5);
    gpio_8841en = ar_hal_gpio_name_to_num(0,1,2);
    //gpio_8841sd = ar_hal_gpio_name_to_num(0,1,3);

    ar_hal_gpio_export(gpio_1v8en);
    ar_hal_gpio_export(gpio_3v3en);
    ar_hal_gpio_export(gpio_vdda);
    ar_hal_gpio_export(gpio_vdet);
    ar_hal_gpio_export(gpio_reset);
    ar_hal_gpio_export(gpio_sleep);
    ar_hal_gpio_export(gpio_addrx);
    ar_hal_gpio_export(gpio_8841en);
    //ar_hal_gpio_export(gpio_8841sd);

    ar_hal_gpio_set_dir(gpio_1v8en, 1);
    ar_hal_gpio_set_dir(gpio_3v3en, 1);
    ar_hal_gpio_set_dir(gpio_vdda, 1);
    ar_hal_gpio_set_dir(gpio_vdet, 1);
    ar_hal_gpio_set_dir(gpio_reset, 1);
    ar_hal_gpio_set_dir(gpio_sleep, 1);
    ar_hal_gpio_set_dir(gpio_addrx, 1);
    ar_hal_gpio_set_dir(gpio_8841en, 1);
    //ar_hal_gpio_set_dir(gpio_8841sd, 1);
    
    //power
    ar_hal_gpio_set_value(gpio_1v8en, 1);
    usleep(5000);
    ar_hal_gpio_set_value(gpio_3v3en, 1);
    ar_hal_gpio_set_value(gpio_vdda, 1);
    usleep(5000);
    ar_hal_gpio_set_value(gpio_vdet, 1);
    usleep(5000);
    
    ar_hal_gpio_set_value(gpio_sleep, 0);
    ar_hal_gpio_set_value(gpio_addrx, 1);
    ar_hal_gpio_set_value(gpio_8841en, 1);
    //ar_hal_gpio_set_value(gpio_8841sd, 1);
    usleep(5000);

    //reset
    ar_hal_gpio_set_value(gpio_reset, 0);
    usleep(10000);
    ar_hal_gpio_set_value(gpio_reset, 1);

    printf("timo256af_power_on exit\n");
    return 0;
}

static AR_S32 timo256af_power_off(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
	int power1_gpio;
    int power2_gpio;
    int reset_gpio;

    power1_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0], p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    power2_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->common_gpio[0],p_dev_power_attr->common_gpio[1],p_dev_power_attr->common_gpio[2]);
    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);

    printf("timo256af_power_off: power_gpio = %d %d, reset_gpio = %d\n", power1_gpio,power2_gpio,reset_gpio);

    ar_hal_gpio_export(power1_gpio);
    ar_hal_gpio_export(power2_gpio);
    ar_hal_gpio_export(reset_gpio);
    
    ar_hal_gpio_set_dir(power1_gpio, 1);     //set the gpio dir to out
    ar_hal_gpio_set_dir(power2_gpio, 1);
    ar_hal_gpio_set_dir(reset_gpio, 1);

    ar_hal_gpio_set_value(reset_gpio, 0);
    usleep(5000);
    ar_hal_gpio_set_value(power2_gpio, 0);
    ar_hal_gpio_set_value(power1_gpio, 0);

    return 0;
}

void timo256af_init(VI_PIPE ViPipe)
{
    //timo256af_i2c_init(ViPipe);
    return;
}

void timo256af_exit(VI_PIPE ViPipe)
{
    //timo256af_i2c_exit(ViPipe);
    return;
}


static AR_S32 timo256af_get_isp_default(VI_PIPE ViPipe,
    ISP_CMOS_DEFAULT_S *pstDef)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstDef);
    timo256af_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    pstDef->stSensorMode.u32SensorID = TIMO256AF_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static AR_S32 timo256af_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_auntimo256afBusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;
    return AR_SUCCESS;
}

static AR_S32 timo256af_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    timo256af_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (AR_NULL == pastSnsStateCtx) {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (AR_NULL == pastSnsStateCtx) {
            ISP_TRACE(AR_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));
    timo256af_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    timo256af_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    timo256af_SENSOR_RESET_CTX(ViPipe);
}

static AR_S32 timo256af_init_sensor_exp_function(
    ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = timo256af_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = timo256af_exit;
    pstSensorExpFunc->pfn_cmos_set_image_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = timo256af_get_isp_default;
    pstSensorExpFunc->pfn_cmos_sns_power_on = timo256af_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = timo256af_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;

    return AR_SUCCESS;
}

static AR_S32 timo256af_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 timo256af_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 timo256af_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = timo256af_ctx_init(ViPipe);
    if (AR_SUCCESS != s32Ret)
        return AR_FAILURE;

    stSnsAttrInfo.eSensorId = TIMO256AF_ID;

    s32Ret  = timo256af_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = timo256af_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = timo256af_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 timo256af_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, TIMO256AF_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, TIMO256AF_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, TIMO256AF_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(ViPipe);

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S stSnsTimo256afObj =
{
    .pfnRegisterCallback    = timo256af_register_callback,
    .pfnUnRegisterCallback  = timo256af_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = timo256af_set_bus_info,
    .pfnSetInit             = AR_NULL,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __TIMO256_CMOS_H_ */
