/*************************************************
Copyright (C), 2021--2030, GuideIR Tech. Co., Ltd.
File name	: HalAPI.h
Author		: Guide Sensmart RD
Version		: 2.0
Date		: 2021/3/9
Description	:
*************************************************/
#include "guide_hal_api.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "guide_internal.h"
#include "hal_gpio.h"

//#include "libremote_i2c.h"
//#include "guide_stream.h"
#include "i2c.h"


#define DEV_I2C_PATH ("/dev/i2c-2")
#define DEV_TEMP_ADC_I2C_PATH ("/dev/i2c-1")
#define DEV_TEMP_ADC_I2C_ADDR 0x48
#define TEST_HIGH_PATH "/usrdata/guide_demo/V1C-10_High.raw"
#define TEST_LOW_PATH "/usrdata/guide_demo/V1C-10_Low.raw"
//#define TEST_HIGH_PATH "/mnt/GHP734714E70BM067_High.raw"
//#define TEST_LOW_PATH "/tmp/GHP734714E70BM067_Low.raw"
#define KFB_PATH "/usrdata/guide_demo/KFB.raw"

static unsigned int g_Fd = 0;
//static IRDataFunc g_DataFunc = NULL;
//static void* g_UserData = NULL;
static pthread_t g_getTempThread;

//static float g_tempLens = 0.0;
//static float g_tempShutter = 0.0;
//static float g_tempFPA = 0.0;
static bool g_start = false;

//static GUIDE_S32 g_streamHandle = -1;

static int callSystem(const char* command)
{
	int status, ret = 0;
	pid_t pid;
	pid = vfork();
	if (pid < 0) {
		printf("vfork failed\n");
		return -1;
	} else if (pid == 0) {
		ret = execl("/bin/sh", "sh", "-c", command, (char *)0);
		if (ret < 0) {
			printf("execl failed\n");
			return -1;
		}
		return ret;
	} else {
		ret = waitpid(pid, &status, 0);
		if (ret != pid) {
			printf("waitpid failed, ret = %d\n", ret);
			return -1;
		}

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				printf("exit status = %d\n", WEXITSTATUS(status));
				return -1;
			}
		} else {
			printf("abort exceptly\n");
			return -1;
		}
	}
	return 0;
}

static int doSystem(const char *fmt, ...)
{
	char cmd_buf[1024];
	int ret;
	va_list args;

	va_start(args, fmt);
	vsprintf(cmd_buf, fmt, args);
	va_end(args);

    //	strcat(cmd_buf, " &2 > 1");
	ret = callSystem(cmd_buf);
    //printf("cmd: %s,ret=%d\n", cmd_buf, ret);
	return ret;
}

/**
* @brief:	增加模组数据回调前后拦截器
* @param:	HANDLE_T handle		 	实例句柄。
* @param:	PreFunc preFunc  	模组数据回调前预处理函数
* @param:	PostFunc postFunc	模组数据回调完成后处理函数
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int AddIRInterceptor(PreFunc preFunc, PostFunc postFunc)
{
	printf("Enter AddIRInterceptor\n");
	return GUIDEIR_OK;
}

#if 0
static void* get_temp_thread(void *p)
{
    unsigned int u16AdVal = 0;
    int cur_chn = -1;
	bool ready = false;
	unsigned int fd = i2c_open(DEV_TEMP_ADC_I2C_PATH, DEV_TEMP_ADC_I2C_ADDR);
	if (fd < 0) {
		printf("i2c_open error\n");
		return NULL;
	}

    while(g_start)
    {
        if(ready)
        {
            i2c_read(fd, 0x00, &u16AdVal);
            if(cur_chn == 0) //lens
            {
                g_tempLens = (12736.1304901973000 - (0.0000000040235 * u16AdVal * u16AdVal * u16AdVal) + (0.0001218402729 * u16AdVal * u16AdVal) - (1.8218076216914 * u16AdVal));
				//printf("u16AdVal == %d, g_tempLens == %0.3f\n", u16AdVal, g_tempLens);
            }
            else if(cur_chn == 1) //shutter
            {
                g_tempShutter = (12736.1304901973000 - (0.0000000040235 * u16AdVal * u16AdVal * u16AdVal) + (0.0001218402729 * u16AdVal * u16AdVal) - (1.8218076216914 * u16AdVal));
				//printf("u16AdVal == %d, g_tempShutter == %0.3f\n", u16AdVal, g_tempShutter);
            }
            else if(cur_chn == 2) //vtemp
            {
                g_tempFPA = 1.25 * u16AdVal - 15800;
				//printf("u16AdVal == %d, g_tempFPA == %0.3f\n", u16AdVal, g_tempFPA);
            }
            ready = false;
        } else {
			cur_chn = (cur_chn + 1) % 3;
			unsigned int data = ((0xc3 + (cur_chn << 4)) << 8) | 0x80;
			i2c_write(fd, 0x01, data);
			ready = true;
        }
        usleep(800000);
    }
    return NULL;
}
#endif

/**
* @brief:	注册模组数据回调函数。
* @param:	IRDataFunc dataFunc			 回调函数
* @param:	void * pUserData	         回调函数预留参数。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
/*
static void StreamCallBack(GUIDE_STREAM_TYPE_E u32Handle, GUIDE_U8 * psEncodeData, GUIDE_U32 u32Len,  void * pUserData)
{
	//printf("psEncodeData, len: %d: %x %x %x %x\n", u32Len, psEncodeData[0], psEncodeData[1], psEncodeData[2], psEncodeData[3]);
	if (g_DataFunc) {
        //dvp source 512*196  delele head 512*2 tail 512*2
        if (u32Len > 512*194) {
            g_DataFunc(psEncodeData+512*2, 512*192, g_UserData);
        }
	}
}*/

HALAPI int RegIRDataCallBack(IRDataFunc dataFunc, void * pUserData)
{

	printf("Enter RegIRDataCallBack ******************\n");
	g_DataFunc = dataFunc;
	g_UserData = pUserData;
    //if (g_streamHandle < 0)  g_streamHandle = GUIDE_STREAM_Init(STREAM_TYPE_IR);
	//GUIDE_STREAM_Start(1, STREAM_TYPE_IR, StreamCallBack, NULL);

	//start get temp thread
	//g_start = true;
	//pthread_create(&g_getTempThread, NULL, get_temp_thread, NULL);
	return GUIDEIR_OK;
}

/**
* @brief:	注销模组数据回调。
* @param:	void * pUserData	         回调函数预留参数。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int UnRegIRDataCallBack(void * pUserData)
{
	printf("Enter UnRegIRDataCallBack\n");
	g_DataFunc = NULL;
	g_UserData = NULL;

	g_start = false;
	pthread_join(g_getTempThread, NULL);
    //GUIDE_STREAM_Stop(g_streamHandle);
	return GUIDEIR_OK;
}

extern volatile int g_isShutterControl;

/**
* @brief:	控制快门。
* @param:	bool enable        true，闭合快门。false，弹开快门。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int ShutterControl(bool enable)
{
	printf("Enter ShutterControl %d\n",enable);
    int shutter1_gpio;
    int shutter2_gpio;
    shutter1_gpio = ar_hal_gpio_name_to_num(3, 0, 1);
    shutter2_gpio = ar_hal_gpio_name_to_num(3, 0, 2);

    ar_hal_gpio_export(shutter1_gpio);
    ar_hal_gpio_export(shutter2_gpio);
    ar_hal_gpio_set_dir(shutter1_gpio, 1);
    ar_hal_gpio_set_dir(shutter2_gpio, 1);

    if(enable)
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

        g_isShutterControl = 2;
    }

    /*
	doSystem("gpio_config -e A3_1");
	doSystem("gpio_config -e A3_2");
	if (enable) { //打开快门
		doSystem("gpio_config -s A3_1 -d output -l high");
		doSystem("gpio_config -s A3_2 -d output -l low");
		usleep(20*1000);

		doSystem("gpio_config -s A3_1 -d output -l low");
		doSystem("gpio_config -s A3_2 -d output -l low");
	} else {
		doSystem("gpio_config -s A3_1 -d output -l low");
		doSystem("gpio_config -s A3_2 -d output -l high");
		usleep(20*1000);

		doSystem("gpio_config -s A3_1 -d output -l low");
		doSystem("gpio_config -s A3_2 -d output -l low");
	}*/
	return GUIDEIR_OK;
}

/**
* @brief:	获取温度传感器值。
* @param:	HAL_TEMP_TYPE_E eType	温度类型。
* @param:	float * temp			温度值。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/

HALAPI int GetSensorTemp(HAL_TEMP_TYPE_E eType, float * temp)
{
#if 0
	switch (eType) {
		case SHUTTER_TEMP:
			*temp = g_tempShutter;
			break;
		case FPA_TEMP:
			*temp = g_tempFPA;
			break;
		case LENS_TEMP:
			*temp = g_tempLens;
			break;
		default:
			return GUIDEIR_ERR;
	}
#else
	static bool isFirstTemp = true;
	switch (eType) {
		case SHUTTER_TEMP:
			if (isFirstTemp) {
				*temp = 30.7;
				isFirstTemp = false;
			}
			*temp = 35.5;
			break;
		case FPA_TEMP:
			*temp = 31.5;
			break;
		case LENS_TEMP:
			*temp = 36.6;
			break;
		default:
			break;
	}
#endif
	return GUIDEIR_OK;
}

/**
* @brief:	打开I2C。
* @param:	unsigned char I2CNum
* @param:	unsigned char u32DevAddr
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2COpen(unsigned char I2CNum, unsigned char u32DevAddr)
{
	printf("Enter I2COpen I2CNum == %d, u32DevAddr == 0x%x 7bit=0x%x\n", I2CNum, u32DevAddr,u32DevAddr>>1);
	
	g_Fd = i2c_open(DEV_I2C_PATH, u32DevAddr>>1);
	if (g_Fd < 0) {
		return GUIDEIR_ERR;
	}

	return GUIDEIR_OK;
}

/**
* @brief:	关闭I2C。
* @param:	unsigned char I2CNum
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2CClose(unsigned char I2CNum)
{
	printf("Enter I2CClose I2CNum == %d\n", I2CNum);
	i2c_close(g_Fd);
	//i2c_stop(&g_I2cDev);
	return GUIDEIR_OK;
}

/**
* @brief:	I2C读寄存器。
* @param:	unsigned char u8Addr	寄存器地址。
* @param:	unsigned char * pu8Val	读出的寄存器值。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2CRead(unsigned char u8Addr, unsigned char * pu8Val)
{
	int rc;
	rc = i2c_read(g_Fd,u8Addr,pu8Val);
	if(rc<0){
		return GUIDEIR_ERR;
	}

	printf("I2CRead OK, u8Addr == 0x%x, pu8Val == 0x%x\n", u8Addr, *pu8Val);
	return GUIDEIR_OK;
}

/**
* @brief:	I2C写寄存器。
* @param:	unsigned char u8Addr	寄存器地址。
* @param:	unsigned char u8Val		写入的寄存器值。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2CWrite(unsigned char u8Addr, unsigned char u8Val)
{

	//printf("I2CWrite OK, u8Addr == 0x%x, u8Val == 0x%x\n", u8Addr, u8Val);

	int rc;
	//rc=i2c_write_reg(&g_I2cDev,u8Addr,u8Val);
	rc = i2c_write(g_Fd,u8Addr,u8Val);
	if(rc<0){
		return GUIDEIR_ERR;
	}

	printf("I2CWrite OK, u8Addr == 0x%x, u8Val == 0x%x\n", u8Addr, u8Val);
	return GUIDEIR_OK;
}

/**
* @brief:  读取数据包。
* @param:  unsigned char *buf			数据缓存
* @param:  int offset					相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size						读取的数据长度
* @param:  int range					测温范围 1，常温范围；2，高温范围。
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int ReadPackageData(unsigned char *buf, int offset, int size, int range)
{
	//printf("Entern ReadPackageData offset == %d, size == %d, range == %d\n");
	FILE *fp = NULL;
	if (range == 1) {
		fp = fopen(TEST_LOW_PATH, "rb");
	} else {
		fp = fopen(TEST_HIGH_PATH, "rb");
	}

	if (!fp) {
		printf("ReadPackageData: fopen package file error\n");
		return GUIDEIR_ERR;
	}

	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	if (len < offset + size) {
		fclose(fp);
		printf("ReadPackageData: file length error! %d %d\n", len, offset + size);
		return GUIDEIR_ERR;
	}

	fseek(fp, offset, SEEK_SET);
	fread(buf, size, 1, fp);
	fclose(fp);

	return GUIDEIR_OK;
}

/**
* @brief:  写数据包。
* @param:  unsigned char *buf			数据缓存
* @param:  int offset					相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size						写的数据长度
* @param:  int range					测温范围 1，常温范围；2，高温范围。
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int WritePackageData(unsigned char *buf, int offset, int size, int range)
{
	printf("Entern WritePackageData\n");
	return GUIDEIR_OK;
}

/**
* @brief:  读取参数。
* @param:  unsigned char *buf			参数缓存
* @param:  int len						缓存长度
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int ReadCalibrationParameters(unsigned char *buf, int len)
{
	printf("Entern ReadCalibrationParameters\n");
	FILE *fp = fopen(KFB_PATH, "rb");
	if (!fp) {
		printf("ReadCalibrationParameters: fopen package file error\n");
		return GUIDEIR_ERR;
	}

	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	if (length < len) {
		fclose(fp);
		printf("ReadCalibrationParameters: file length error! %d %d\n", length, len);
		return GUIDEIR_ERR;
	}

	fseek(fp, 0, SEEK_SET);
	fread(buf, len, 1, fp);
	fclose(fp);

	return GUIDEIR_OK;
}

/**
* @brief:  保存参数。
* @param:  unsigned char *buf			参数缓存
* @param:  int len						参数长度
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int WriteCalibrationParameters(unsigned char *buf, int len)
{
	printf("Entern WriteCalibrationParameters\n");
	return GUIDEIR_OK;
}

/**
* @brief:  查询版本号。
* @param:  char *version				版本号
* @param:  int len						version缓存大小
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int HalVersion(char *version, int len)
{
	printf("Entern HalVersion\n");
	return GUIDEIR_OK;
}
