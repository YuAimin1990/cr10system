/*************************************************
Copyright (C), 2021--2030, GuideIR Tech. Co., Ltd.
File name	: HalAPI.h
Author		: Guide Sensmart RD
Version		: 2.0
Date		: 2021/3/9
Description	:
*************************************************/
//#include "guide_hal_api.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <stdlib.h>
#include <fcntl.h> 
#include <string.h>
#include <sys/ioctl.h> 
#include <linux/types.h> 
#include <linux/spi/spidev.h> 
//#include "ar_spi.h"

#include "ar_common.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "osal.h"

#include "hal_gpio.h"
#include "i2c.h"

#include "ITADTD.h"

#define DEV_I2C_PATH ("/dev/i2c-2")
#define DEV_TEMP_ADC_I2C_PATH ("/dev/i2c-1")
#define DEV_TEMP_ADC_I2C_ADDR 0x48
#define TEST_HIGH_PATH "/usrdata/guide_demo/V1C-10_High.raw"
#define TEST_LOW_PATH "/usrdata/guide_demo/V1C-10_Low.raw"
//#define TEST_HIGH_PATH "/mnt/GHP734714E70BM067_High.raw"
//#define TEST_LOW_PATH "/tmp/GHP734714E70BM067_Low.raw"
//#define TEST_HIGH_PATH "/usrdata/guide_demo/GHGH17712C606R143_High.raw"
//#define TEST_LOW_PATH "/usrdata/guide_demo/GHGH17712C606R143_Low.raw"
#define KFB_PATH "/usrdata/guide_demo/KFB.raw"

#define DEV_SPI_PATH ("/dev/spidev32766.0")

static unsigned int g_Fd = 0;

#define ADC_TEMP_ENABLE
#ifdef ADC_TEMP_ENABLE
static pthread_t g_getTempThread;
static float g_tempLens = 21.5*100;
static float g_tempShutter = 22.0*100;
static float g_tempFPA = 22.5*100;
static bool g_start = false;
#endif

/*
####################################################################

    ITA SDK Hal Function, artosyn ar9341, glfeng,20220614

####################################################################
*/

#ifdef ADC_TEMP_ENABLE
static void* get_temp_thread(void *p)
{
    int ret;
    unsigned short u16AdVal = 0;
    int cur_chn = -1;
	bool ready = false;
	unsigned int fd = i2c_open(DEV_TEMP_ADC_I2C_PATH, DEV_TEMP_ADC_I2C_ADDR);
	if (fd < 0) {
		printf("temp adc i2c open failed\n");
		return NULL;
	}

	//unsigned int data = (0x80 << 8) | (0xc3 + (cur_chn << 4));
	//i2c_write_word(fd, 0x01, data);

	static unsigned int ii = 0;

    while(g_start)
    {
        if(ready)
        {
            ret = i2c_read_word(fd, 0x00, &u16AdVal);
            if(ret!=0){
                printf("temp adc i2c read failed\n");
                sleep(5);
		        continue;
            }
            u16AdVal = (u16AdVal << 8) | (u16AdVal >> 8);
            if(cur_chn == 0) //lens
            {
                //float voltage = u16AdVal * 4.09 / 0x7fff;
                g_tempLens = (12736.1304901973000 - (0.0000000040235 * u16AdVal * u16AdVal * u16AdVal) + (0.0001218402729 * u16AdVal * u16AdVal) - (1.8218076216914 * u16AdVal));
				//printf("u16AdVal == %04x, g_tempLens == %0.3f v=%.2f\n", u16AdVal, g_tempLens, voltage);
            }
            else if(cur_chn == 1) //shutter
            {
                //float voltage = u16AdVal * 4.09 / 0x7fff;
                g_tempShutter = (12736.1304901973000 - (0.0000000040235 * u16AdVal * u16AdVal * u16AdVal) + (0.0001218402729 * u16AdVal * u16AdVal) - (1.8218076216914 * u16AdVal));
				//printf("u16AdVal == %04x, g_tempShutter == %0.3f v=%.2f\n", u16AdVal, g_tempShutter, voltage);
            }
            else if(cur_chn == 2) //vtemp
            {
                //float voltage = u16AdVal * 4.09 / 0x7fff;
                g_tempFPA = 1.25 * u16AdVal - 15800;
				//printf("u16AdVal == %04x, g_tempFPA == %0.3f v=%.2f\n", u16AdVal, g_tempFPA, voltage);
            }
            ready = false;

            ii++;
            if(ii%10==0)
            {
                //printf("g_tempLens == %0.3f g_tempShutter == %0.3f g_tempFPA == %0.3f\n",g_tempLens,g_tempShutter,g_tempFPA);
            }
        } else {
			cur_chn = (cur_chn + 1) % 3;
			//unsigned int data = ((0xc3 + (cur_chn << 4)) << 8) | 0x80;
			unsigned int data = (0x80 << 8) | (0xc3 + (cur_chn << 4));
            ret = i2c_write_word(fd, 0x01, data);
            if(ret!=0){
                printf("temp adc i2c write failed\n");
                sleep(5);
                continue;
            }
			ready = true;
        }
        usleep(800000);
    }

    i2c_close(fd);
    return NULL;
}
#endif

int arAdcTempThreadCtl(int start)
{
#ifdef ADC_TEMP_ENABLE
    if(start){
        if(!g_start){
            g_start = true;
            pthread_create(&g_getTempThread, NULL, get_temp_thread, NULL);
        }
    }
    else{
        if(g_start){
            g_start = false;
	        pthread_join(g_getTempThread, NULL);
        }
    }
#endif
    return 0;
}

/**
* @brief:   控制快门。
* @param:   int cameraID                标识设备，支持同时打开多台设备。
* @param:   ITA_SHUTTER_STATUS status   快门状态。
* @param:   void *userParam             用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arShutterControl(int cameraID, ITA_SHUTTER_STATUS status, void *userParam)
{
    printf("camera id:%d, shutter status:%d\n", cameraID, status);
    int shutter1_gpio;
    int shutter2_gpio;
    shutter1_gpio = ar_hal_gpio_name_to_num(3, 0, 1);
    shutter2_gpio = ar_hal_gpio_name_to_num(3, 0, 2);

    ar_hal_gpio_export(shutter1_gpio);
    ar_hal_gpio_export(shutter2_gpio);
    ar_hal_gpio_set_dir(shutter1_gpio, 1);
    ar_hal_gpio_set_dir(shutter2_gpio, 1);

    if(status == ITA_STATUS_CLOSE)
    {
        ar_hal_gpio_set_value(shutter1_gpio, 1);
        ar_hal_gpio_set_value(shutter2_gpio, 0);
        usleep(20*1000);
        ar_hal_gpio_set_value(shutter1_gpio, 0);
        ar_hal_gpio_set_value(shutter2_gpio, 0);
    } else {
        ar_hal_gpio_set_value(shutter1_gpio, 0);
        ar_hal_gpio_set_value(shutter2_gpio, 1);
        usleep(20*1000);
        ar_hal_gpio_set_value(shutter1_gpio, 0);
        ar_hal_gpio_set_value(shutter2_gpio, 0);

        //g_isShutterControl = 2;
    }
	return ITA_OK;
}

/**
* @brief:   获取温度传感器值。由ITA_SENSOR_VALUE决定输出AD值还是温度值。
* @param:   int cameraID                    标识设备，支持同时打开多台设备。输入参数。
* @param:   ITA_TEMP_TYPE eType             温传类型。输入参数。
* @param:   ITA_SENSOR_VALUE *sensorValue   值类型。输出参数，由用户决定。
* @param:   int *ADValue                    AD值，输出参数。当ITA_AD_VALUE==sensorValue时有效。
* @param:   float *tempValue                温度值，输出参数。当ITA_TEMP_VALUE==sensorValue时有效。
* @param:   void *userParam                 用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arGetSensorTemp(int cameraID, ITA_TEMP_TYPE eType, ITA_SENSOR_VALUE *sensorValue, int *ADValue, float *tempValue, void *userParam)
{
    *sensorValue = ITA_TEMP_VALUE;
#ifdef ADC_TEMP_ENABLE
        switch (eType) {
            case ITA_SHUTTER_TEMP:
                *tempValue = g_tempShutter/100;
                break;
            case ITA_FPA_TEMP:
                *tempValue = g_tempFPA/100;
                break;
            case ITA_LENS_TEMP:
                *tempValue = g_tempLens/100;
                break;
            default:
                return ITA_OK;
        }
#else
	switch (eType)
	{
	case ITA_SHUTTER_TEMP:
		*tempValue = 22.0;
		break;
	case ITA_FPA_TEMP:
		*tempValue = 22.5;
		break;
	case ITA_LENS_TEMP:
		*tempValue = 21.5;
		break;
	default:
		break;
	}
#endif
	return ITA_OK;
}

/**
* @brief:  读数据包。
* @param:  int cameraID         标识设备，支持同时打开多台设备。
* @param:  unsigned char *buf   数据缓存
* @param:  int offset           相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size             读取的数据长度
* @param:  ITA_RANGE range      测温范围，共有3档。
* @param:   void *userParam     用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arReadPackageData(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam)
{
    printf("readPackageData camera id=%d, offset=%d, size=%d, range=%d\n", cameraID, offset, size, range);
    
	FILE *fp = NULL;
	if (range == ITA_INDUSTRY_LOW) {
		fp = fopen(TEST_LOW_PATH, "rb");
	}
	else if(range == ITA_INDUSTRY_HIGH)
	{
	    fp = fopen(TEST_HIGH_PATH, "rb");
	}
	else {
		//body
		return ITA_FILE_NOT_EXIST;
	}

	if (!fp) {
		printf("ReadPackageData: fopen package file error\n");
		return ITA_READ_PACKAGE_ERROR;
	}

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	if (len < offset + size) {
		fclose(fp);
		printf("ReadPackageData: file length error! %d %d\n", len, offset + size);
		return ITA_READ_PACKAGE_ERROR;
	}

	fseek(fp, offset, SEEK_SET);
	fread(buf, size, 1, fp);
	fclose(fp);

	return ITA_OK;

}

/**
* @brief:  写数据包。
* @param:  int cameraID         标识设备，支持同时打开多台设备。
* @param:  unsigned char *buf   数据缓存
* @param:  int offset           相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size             写的数据长度
* @param:  ITA_RANGE range      测温范围，共有3档。
* @param:   void *userParam     用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arWritePackageData(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam)
{
    printf("writePackageData camera id=%d, offset=%d, size=%d, range=%d\n", cameraID, offset, size, range);
    return ITA_OK;
}

/**
* @brief:	打开I2C。Timo 256通过I2C配置探测器。
* @param:	int cameraID				标识设备，支持同时打开多台设备。
* @param:	unsigned char u32DevAddr
* @param:	void *userParam				用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arI2COpen(int cameraID, unsigned char u32DevAddr, void *userParam)
{
    printf("Enter I2COpen cameraID == %d, u32DevAddr == 0x%x 7bit=0x%x\n", cameraID, u32DevAddr,u32DevAddr>>1);

 	g_Fd = i2c_open(DEV_I2C_PATH, u32DevAddr>>1);
 	if (g_Fd < 0) {
 		return ITA_I2C_ERROR;
 	}

 	return ITA_OK;
}

/**
* @brief:	关闭I2C。Timo 256通过I2C配置探测器。
* @param:	int cameraID			标识设备，支持同时打开多台设备。
* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arI2CClose(int cameraID, void *userParam)
{
	printf("Enter I2CClose cameraID == %d\n", cameraID);
 	i2c_close(g_Fd);
 	return ITA_OK;
}

/**
* @brief:	I2C读寄存器。Timo 256通过I2C配置探测器。
* @param:	int cameraID			标识设备，支持同时打开多台设备。
* @param:	unsigned char u8Addr	寄存器地址。
* @param:	unsigned char * pu8Val	读出的寄存器值。
* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arI2CRead(int cameraID, unsigned char u8Addr, unsigned char * pu8Val, void *userParam)
{
    int rc;
 	rc = i2c_read(g_Fd,u8Addr,pu8Val);
 	if(rc<0){
 		return ITA_I2C_ERROR;
 	}

 	printf("I2CRead OK, u8Addr == 0x%x, pu8Val == 0x%x\n", u8Addr, *pu8Val);
 	return ITA_OK;
}

/**
* @brief:	I2C写寄存器。Timo 256通过I2C配置探测器。
* @param:	int cameraID			标识设备，支持同时打开多台设备。
* @param:	unsigned char u8Addr	寄存器地址。
* @param:	unsigned char u8Val		写入的寄存器值。
* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arI2CWrite(int cameraID, unsigned char u8Addr, unsigned char u8Val, void *userParam)
{
    int rc;
    rc = i2c_write(g_Fd,u8Addr,u8Val);
    if(rc<0){
        return ITA_I2C_ERROR;
    }

    printf("I2CWrite OK, u8Addr == 0x%x, u8Val == 0x%x\n", u8Addr, u8Val);
    return ITA_OK;
}

