#if !defined(__TIMO120_CMOS_H_)
#define __TIMO120_CMOS_H_

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


#define TIMO120_ID 1120

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pasttimo120[ISP_MAX_PIPE_NUM] = {AR_NULL};

#define timo120_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pasttimo120[dev])
#define timo120_SENSOR_SET_CTX(dev, pstCtx)   (g_pasttimo120[dev] = pstCtx)
#define timo120_SENSOR_RESET_CTX(dev)         (g_pasttimo120[dev] = AR_NULL)

ISP_SNS_COMMBUS_U g_auntimo120BusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = { .s8I2cDev = 3},
    [1 ... ISP_MAX_PIPE_NUM - 1] = { .s8I2cDev = -1}
};

static const unsigned int timo120_i2c_addr = 0x45;

static int g_fd[ISP_MAX_PIPE_NUM] = {
    [0 ... (ISP_MAX_PIPE_NUM - 1)] = -1
};

#if 1
static int timo120_i2c_init(VI_PIPE ViPipe)
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
    u8DevNum = g_auntimo120BusInfo[ViPipe].s8I2cDev;

    g_fd[ViPipe] = ar_hal_i2c_open(u8DevNum);
    if (g_fd[ViPipe] < 0) {
        ISP_TRACE(AR_DBG_ERR, "Open i2c-%u error!\n", u8DevNum);
        return AR_FAILURE;
    }
#endif

    return AR_SUCCESS;
}

int timo120_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0) {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return AR_SUCCESS;
    }

    return AR_FAILURE;
}

int timo120_read_register(VI_PIPE ViPipe, unsigned int addr, unsigned int *data)
{
    return ar_hal_i2c_master_read(g_fd[ViPipe], timo120_i2c_addr,
                                  8, (uint8_t *)&addr, 1, (uint8_t *)data, 1);
}

int timo120_write_register(VI_PIPE ViPipe, unsigned int addr, unsigned int data)
{
    unsigned char buf[8];

    buf[0] = (unsigned char)addr;
    buf[1] = (unsigned char)data;

    return ar_hal_i2c_master_write(g_fd[ViPipe],
        timo120_i2c_addr, 8, buf, 2);
}
#endif

static AR_S32 timo120_power_on(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
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
    printf("timo120_power_on enter\n");

    int gpio_avdd, gpio_vsk, gpio_TcalEn, gpio_CalDataValid, gpio_reset;
    gpio_avdd           = ar_hal_gpio_name_to_num(3,3,5);
    gpio_vsk            = ar_hal_gpio_name_to_num(2,3,0);
    gpio_TcalEn         = ar_hal_gpio_name_to_num(3,3,7);
    gpio_CalDataValid   = ar_hal_gpio_name_to_num(0,3,5);
    gpio_reset          = ar_hal_gpio_name_to_num(3,3,6);

    ar_hal_gpio_export(gpio_avdd);
    ar_hal_gpio_export(gpio_vsk);
    ar_hal_gpio_export(gpio_TcalEn);
    ar_hal_gpio_export(gpio_CalDataValid);
    ar_hal_gpio_export(gpio_reset);

    ar_hal_gpio_set_dir(gpio_avdd, 1);
    ar_hal_gpio_set_dir(gpio_vsk, 1);
    ar_hal_gpio_set_dir(gpio_TcalEn, 1);
    ar_hal_gpio_set_dir(gpio_CalDataValid, 1);
    ar_hal_gpio_set_dir(gpio_reset, 1);
    
    //power
    ar_hal_gpio_set_value(gpio_avdd, 1);
    usleep(18*1000);
    ar_hal_gpio_set_value(gpio_vsk, 1);
    usleep(10*1000);
    
    ar_hal_gpio_set_value(gpio_TcalEn, 0);
    ar_hal_gpio_set_value(gpio_CalDataValid, 0);

    //reset
    ar_hal_gpio_set_value(gpio_reset, 1);
    usleep(10000);
    ar_hal_gpio_set_value(gpio_reset, 0);

    printf("timo120_power_on exit\n");
    return 0;
}

static AR_S32 timo120_power_off(VI_PIPE ViPipe,ar_dev_power_attr_t  *p_dev_power_attr)
{
#if 0
	int power1_gpio;
    int power2_gpio;
    int reset_gpio;

    power1_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->power_gpio[0], p_dev_power_attr->power_gpio[1], p_dev_power_attr->power_gpio[2]);
    power2_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->common_gpio[0],p_dev_power_attr->common_gpio[1],p_dev_power_attr->common_gpio[2]);
    reset_gpio = ar_hal_gpio_name_to_num(p_dev_power_attr->reset_gpio[0],p_dev_power_attr->reset_gpio[1],p_dev_power_attr->reset_gpio[2]);

    printf("timo120_power_off: power_gpio = %d %d, reset_gpio = %d\n", power1_gpio,power2_gpio,reset_gpio);

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
#endif

    printf("timo120_power_off enter\n");

    int gpio_avdd, gpio_vsk, gpio_TcalEn, gpio_CalDataValid, gpio_reset;
    gpio_avdd           = ar_hal_gpio_name_to_num(3,3,5);
    gpio_vsk            = ar_hal_gpio_name_to_num(2,3,0);
    gpio_TcalEn         = ar_hal_gpio_name_to_num(3,3,7);
    gpio_CalDataValid   = ar_hal_gpio_name_to_num(0,3,5);
    gpio_reset          = ar_hal_gpio_name_to_num(3,3,6);

    ar_hal_gpio_export(gpio_avdd);
    ar_hal_gpio_export(gpio_vsk);
    ar_hal_gpio_export(gpio_TcalEn);
    ar_hal_gpio_export(gpio_CalDataValid);
    ar_hal_gpio_export(gpio_reset);

    ar_hal_gpio_set_dir(gpio_avdd, 1);
    ar_hal_gpio_set_dir(gpio_vsk, 1);
    ar_hal_gpio_set_dir(gpio_TcalEn, 1);
    ar_hal_gpio_set_dir(gpio_CalDataValid, 1);
    ar_hal_gpio_set_dir(gpio_reset, 1);
    
    //power
    ar_hal_gpio_set_value(gpio_vsk, 0);
    usleep(10*1000);
    ar_hal_gpio_set_value(gpio_avdd, 0);

    printf("timo120_power_off exit\n");
    return 0;
}

void timo120_init(VI_PIPE ViPipe)
{
    //timo120_i2c_init(ViPipe);
    return;
}

void timo120_exit(VI_PIPE ViPipe)
{
    //timo120_i2c_exit(ViPipe);
    return;
}


static AR_S32 timo120_get_isp_default(VI_PIPE ViPipe,
    ISP_CMOS_DEFAULT_S *pstDef)
{
    ISP_SNS_STATE_S *pstSnsState = AR_NULL;

    CMOS_CHECK_POINTER(pstDef);
    timo120_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    pstDef->stSensorMode.u32SensorID = TIMO120_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;

    strcpy(pstDef->TuningPraBinName,"/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");

    return AR_SUCCESS;
}


/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static AR_S32 timo120_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    g_auntimo120BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;
    return AR_SUCCESS;
}

static AR_S32 timo120_ctx_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    timo120_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (AR_NULL == pastSnsStateCtx) {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (AR_NULL == pastSnsStateCtx) {
            ISP_TRACE(AR_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return AR_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));
    timo120_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return AR_SUCCESS;
}

static AR_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = AR_NULL;

    timo120_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    timo120_SENSOR_RESET_CTX(ViPipe);
}

static AR_S32 timo120_init_sensor_exp_function(
    ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    CMOS_CHECK_POINTER(pstSensorExpFunc);
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = timo120_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit = timo120_exit;
    pstSensorExpFunc->pfn_cmos_set_image_mode = AR_NULL;
    pstSensorExpFunc->pfn_cmos_get_isp_default = timo120_get_isp_default;
    pstSensorExpFunc->pfn_cmos_sns_power_on = timo120_power_on;
    pstSensorExpFunc->pfn_cmos_sns_power_off = timo120_power_off;
    pstSensorExpFunc->pfn_cmos_sns_ctl = AR_NULL;

    return AR_SUCCESS;
}

static AR_S32 timo120_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 timo120_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    CMOS_CHECK_POINTER(pstExpFuncs);
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));
    return AR_SUCCESS;
}

static AR_S32 timo120_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = timo120_ctx_init(ViPipe);
    if (AR_SUCCESS != s32Ret)
        return AR_FAILURE;

    stSnsAttrInfo.eSensorId = TIMO120_ID;

    s32Ret  = timo120_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= AR_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = timo120_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= AR_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = timo120_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= AR_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor register callback function to awb lib failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

static AR_S32 timo120_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    AR_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = AR_MPI_ISP_SensorUnRegCallBack(ViPipe, TIMO120_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, TIMO120_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = AR_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, TIMO120_ID);
    if (AR_SUCCESS != s32Ret) {
        ISP_TRACE(AR_DBG_ERR, "sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(ViPipe);

    return AR_SUCCESS;
}

ISP_SNS_OBJ_S stSnsTimo120Obj =
{
    .pfnRegisterCallback    = timo120_register_callback,
    .pfnUnRegisterCallback  = timo120_unregister_callback,
    .pfnStandby             = AR_NULL,
    .pfnRestart             = AR_NULL,
    .pfnMirrorFlip          = AR_NULL,
    .pfnWriteReg            = AR_NULL,
    .pfnReadReg             = AR_NULL,
    .pfnSetBusInfo          = timo120_set_bus_info,
    .pfnSetInit             = AR_NULL,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __TIMO120_CMOS_H_ */
