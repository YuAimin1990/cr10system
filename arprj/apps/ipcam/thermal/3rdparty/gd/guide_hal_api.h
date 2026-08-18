/*************************************************
Copyright (C), 2021--2030, GuideIR Tech. Co., Ltd.
File name	: HalAPI.h
Author		: Guide Sensmart RD
Version		: 2.0
Date		: 2021/3/9
Description	:
*************************************************/
#ifndef HAL_API_H
#define HAL_API_H
#include <stdbool.h>
#ifdef __cplusplus
#    define HAL_API_START  extern "C" {
#    define HAL_API_END    };
#else
#    define HAL_API_START
#    define HAL_API_END
#endif

HAL_API_START

#ifdef WINDOWS_PLATFORM
#    define HALAPI __declspec(dllexport)
#else
#    define HALAPI
#endif // WINDOWS_PLATFORM

/* general return values */
#define GUIDEIR_OK			(0)
#define GUIDEIR_ERR			(-1)

typedef enum enumTEMP_TYPE_E
{
	SHUTTER_TEMP, /* 快门温AD */
	FPA_TEMP, /* 焦温AD */
	LENS_TEMP, /* 镜筒温AD */
} HAL_TEMP_TYPE_E;

typedef void(*IRDataFunc)(unsigned char * pData, int dataLen, void * pUserData);

typedef void(*PreFunc)();
typedef void(*PostFunc)();

/**
* @brief:	增加模组数据回调前后拦截器
* @param:	HANDLE_T handle		 	实例句柄。
* @param:	PreFunc preFunc  	模组数据回调前预处理函数
* @param:	PostFunc postFunc	模组数据回调完成后处理函数
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int AddIRInterceptor(PreFunc preFunc, PostFunc postFunc);

/**
* @brief:	注册模组数据回调函数。
* @param:	IRDataFunc dataFunc			 回调函数
* @param:	void * pUserData	         回调函数预留参数。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int RegIRDataCallBack(IRDataFunc dataFunc, void * pUserData);

/**
* @brief:	注销模组数据回调。
* @param:	void * pUserData	         回调函数预留参数。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int UnRegIRDataCallBack(void * pUserData);

/**
* @brief:	控制快门。
* @param:	bool enable        true，闭合快门。false，弹开快门。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int ShutterControl(bool enable);

/**
* @brief:	获取温度传感器值。
* @param:	HAL_TEMP_TYPE_E eType	温度类型。
* @param:	float * temp			温度值。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int GetSensorTemp(HAL_TEMP_TYPE_E eType, float * temp);

/**
* @brief:	打开I2C。
* @param:	unsigned char I2CNum
* @param:	unsigned char u32DevAddr
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2COpen(unsigned char I2CNum, unsigned char u32DevAddr);

/**
* @brief:	关闭I2C。
* @param:	unsigned char I2CNum
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2CClose(unsigned char I2CNum);

/**
* @brief:	I2C读寄存器。
* @param:	unsigned char u8Addr	寄存器地址。
* @param:	unsigned char * pu8Val	读出的寄存器值。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2CRead(unsigned char u8Addr, unsigned char * pu8Val);

/**
* @brief:	I2C写寄存器。
* @param:	unsigned char u8Addr	寄存器地址。
* @param:	unsigned char u8Val		写入的寄存器值。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int I2CWrite(unsigned char u8Addr, unsigned char u8Val);

/**
* @brief:  读取数据包。
* @param:  unsigned char *buf			数据缓存
* @param:  int offset					相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size						读取的数据长度
* @param:  int range					测温范围 1，常温范围；2，高温范围。
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int ReadPackageData(unsigned char *buf, int offset, int size, int range);

/**
* @brief:  写数据包。
* @param:  unsigned char *buf			数据缓存
* @param:  int offset					相对数据包头的偏移，从偏移处开始读size长度的数据。
* @param:  int size						写的数据长度
* @param:  int range					测温范围 1，常温范围；2，高温范围。
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int WritePackageData(unsigned char *buf, int offset, int size, int range);

/**
* @brief:  读取参数。
* @param:  unsigned char *buf			参数缓存
* @param:  int len						缓存长度
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int ReadCalibrationParameters(unsigned char *buf, int len);

/**
* @brief:  保存参数。
* @param:  unsigned char *buf			参数缓存
* @param:  int len						参数长度
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int WriteCalibrationParameters(unsigned char *buf, int len);

/**
* @brief:  查询版本号。
* @param:  char *version				版本号
* @param:  int len						version缓存大小
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
HALAPI int HalVersion(char *version, int len);

HAL_API_END

#endif // !HAL_API_H

