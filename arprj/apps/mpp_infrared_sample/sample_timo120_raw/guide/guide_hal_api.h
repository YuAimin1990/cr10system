#include "ITADTD.h"

/* common function  artosyn glfeng@20220620 */
int arAdcTempThreadCtl(int start);
void gpio_spi_init();
void gpio_spi_uninit();


/**
* @brief:   控制快门。
* @param:   int cameraID                标识设备，支持同时打开多台设备。
* @param:   ITA_SHUTTER_STATUS status   快门状态。
* @param:   void *userParam             用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arShutterControl(int cameraID, ITA_SHUTTER_STATUS status, void *userParam);

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
ITA_RESULT arGetSensorTemp(int cameraID, ITA_TEMP_TYPE eType, ITA_SENSOR_VALUE *sensorValue, int *ADValue, float *tempValue, void *userParam);

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
ITA_RESULT arReadPackageData(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam);

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
ITA_RESULT arWritePackageData(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam);

/**
* @brief:   SPI传输数据，先写入再读。Timo 120通过SPI配置探测器。
* @param:   int cameraID             标识设备，支持同时打开多台设备。
* @param:   unsigned char *writeBuf  待写入的数据缓存。
* @param:   unsigned char *readBuf   读数据的缓存。
* @param:   int bufLen               缓存长度。
* @param:   void *userParam          用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arSPITransmit(int cameraID, unsigned char *writeBuf, unsigned char *readBuf, int bufLen, void *userParam);

/**
* @brief:   SPI写数据。Timo 120通过SPI配置探测器。
* @param:   int cameraID             标识设备，支持同时打开多台设备。
* @param:   unsigned char *writeData 待写入的数据。
* @param:   int writeLen             数据长度。
* @param:   void *userParam          用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
* @return:  返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
* @see ITA_Init，camera id由用户输入。
**/
ITA_RESULT arSPIWrite(int cameraID, unsigned char *writeData, int writeLen, void *userParam);
