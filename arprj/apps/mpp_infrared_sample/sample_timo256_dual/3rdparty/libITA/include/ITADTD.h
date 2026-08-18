/*************************************************************
Copyright (C), 2021--2022, Wuhan Guide Sensmart Tech Co., Ltd
File name   : ITADTD.h
Author      : Guide Sensmart RD. wangyan
Version     : 1.0
Date        : 2021/11/9
Description : ITA SDK data structure definition.
*************************************************************/
#ifndef ITA_DTD_H
#define ITA_DTD_H

#include "ERROR.h"
#include <stdbool.h>

typedef enum ITAMode
{
	ITA_X16,
	ITA_MCU_X16,
	ITA_Y16,
	ITA_FPGA_Y16
}ITA_MODE;

typedef enum ITARange
{
	ITA_HUMAN_BODY,		//人体测温范围
	ITA_INDUSTRY_LOW,	//工业测温常温段
	ITA_INDUSTRY_HIGH	//工业测温高温段
}ITA_RANGE;

typedef enum ITASensorValue
{
	ITA_AD_VALUE,		/* 温传AD值，int型 */
	ITA_TEMP_VALUE		/* 温传温度值，float型 */
} ITA_SENSOR_VALUE;

/*由ITA_SENSOR_VALUE决定是AD值还是温度。*/
typedef enum ITATempType
{
	ITA_SHUTTER_TEMP,	/* 快门温 */
	ITA_FPA_TEMP,		/* 焦温 */
	ITA_LENS_TEMP		/* 镜筒温 */
} ITA_TEMP_TYPE;

typedef enum ITAShutterStatus
{
	ITA_STATUS_OPEN,		//快门弹开状态。
	ITA_STATUS_CLOSE		//快门闭合状态。
}ITA_SHUTTER_STATUS;

/*根据用户设置的图像格式、缩放倍数、旋转方式，计算的结果。
* 旋转、翻转对图像和Y16数据都生效，缩放只对图像有效。
*/
typedef struct ITA_IMG_INFO {
	int imgDataLen;			//放大后的图像数据长度，单位Byte。
	int imgW;				//旋转、放大后的分辨率
	int imgH;
	int y16Len;				//数据长度，单位short。
	int w;					//旋转后的分辨率
	int h;
}ITAImgInfo;

/**
* ITA库ISP图像处理函数的输出参数。
*/
typedef struct ITA_ISP_RES {
	unsigned char *imgDst;
	short *y16Data;
	ITAImgInfo info;
}ITAISPResult;

typedef enum ITAMatrixType
{
	ITA_NORMAL_MATRIX,	//温度矩阵
	ITA_FAST_MATRIX		//快速温度矩阵，占用CPU资源相对较少，精度相对ITA_NORMAL_MATRIX降低1%以内，误差在±0.2以内。
}ITA_MATRIX_TYPE;

typedef enum ITASwitch
{
	ITA_DISABLE,		//关闭
	ITA_ENABLE			//打开
}ITA_SWITCH;

typedef enum ITADCType
{
	ITA_DETECTOR_VALID_MODE,       //0：探测器VSYNC和HSYNC高有效；1：探测器VSYNC和HSYNC低有效。默认0。参数类型：整型。
	ITA_DETECTOR_DATA_MODE,        //0：先发16位数据的高8bits；1：先发16位数据的低8bits。默认1。参数类型：整型。
	ITA_DETECTOR_FRAME_FREQUENCY,  //设置探测器帧频，范围1-30，单位fps。建议使用以下典型帧频：9、15、20、25和30。默认25。参数类型：整型。
}ITA_DC_TYPE;	//Detector control type

typedef enum ITABCType
{
	ITA_SHUTTER_NOW,			//打快门
	ITA_AUTO_SHUTTER,			//控制自动快门，默认打开。参数类型：ITA_SWITCH。
	ITA_SHUTTER_TIME,			//设置自动快门间隔时间，单位秒。默认30秒。参数类型：整型。如果用户设置自动快门时间，那么默认的自动快门策略不再有效。
	ITA_SINGLE_STEP_TIME,		//设置单步控制快门从闭合到打开整个过程的时间，单位ms。默认约400ms。参数类型：整型。
	ITA_STEP_DELAY_TIME,		//设置单步控制快门时弹开或闭合的延时，单位ms。默认100ms。例如USB连接的产品可以设置400ms。参数类型：整型。
	ITA_B_DELAY,                //设置采集本底延时，单位帧。默认5帧。用户可以根据需要增加延时。参数类型：整型。
	ITA_GET_SHUTTER_STATUS,		//获取快门状态，快门闭合时用户可以定格画面。参数类型：ITA_SHUTTER_STATUS。在X16使用快门策略的情况下用到。
	ITA_FIRST_NUC_FINISH,		//第一次NUC是否完成。参数类型：ITA_SWITCH。ITA_DISABLE，未完成；ITA_ENABLE，已完成。
	ITA_DO_NUC,					//手动NUC
	ITA_AUTO_GEARS,				//自动切焦温档位开关，默认打开。参数类型：ITA_SWITCH。手动切之前先关闭自动切换。
	ITA_CHANGE_GEAR,			//手动切焦温档位。参数类型：整形，>= 0。调用ITA_Version获取档位数量。手动切之前先关闭自动切换。
	ITA_SHUTTER_POLICY,			//控制自动快门和NUC策略的开关。参数类型：ITA_SWITCH。默认打开。通常在X16模式下打开，在Y16模式下关闭（FPGA产品例外）。
	ITA_CLOSE_SHUTTER,			//单步控制快门闭合，无参数。先disable ITA_SHUTTER_POLICY，再单步控制快门。
	ITA_OPEN_SHUTTER,			//单步控制快门弹开，无参数。先disable ITA_SHUTTER_POLICY，再单步控制快门。
	ITA_IMAGE_PROCESS,			//控制图像处理。参数类型：ITA_SWITCH。默认打开。在X16模式下不能关闭，在Y16模式下可以关闭。例如某类产品通过芯片ISP已经生成YUV，使用ITA仅测温。
	ITA_SET_FRAMERATE,			//设置帧率，1-25之间有效。原始帧率25fps。调整帧率可以调节CPU负载。参数类型：整型。
	ITA_GET_FRAMERATE,			//获取实际帧率。参数类型：整型。
	ITA_SET_CONSUME,			//设置执行一次图像处理消耗的平均时间，值可以参考ITA_ISP_CONSUME。参数类型：整型。单位：ms。在使用ITA_SET_FRAMERATE的情况下，该值影响实际帧率。
	ITA_NUC_REPEAT_CORRECTION,  //重复NUC纠错机制。参数类型：ITA_SWITCH。默认关闭。
	ITA_NUC_ANOTHER_SHUTTER,    //重复快门纠错机制。参数类型：ITA_SWITCH。默认关闭。
	ITA_NUC_REPEAT_THRESHOLD,   //判断NUC失效的阈值。参数类型：整形。默认400，适用于120模组。用户根据产品需要调整此值。
	ITA_AUTO_FOCUS,				//自动调焦。参数类型：整形。0：正常调焦,起点为近焦；1：正常调焦,起点为远焦；2：校准参数,起点为近焦；3：校准参数,起点为远焦。
	ITA_AF_NEAR,				//调最近焦
	ITA_AF_FAR,					//调最远焦
	ITA_STEP_NEAR,				//调整一个时间周期步长近焦
	ITA_STEP_FAR				//调整一个时间周期步长远焦
}ITA_BC_TYPE;	//Base control type

typedef enum ITAMCType
{
	ITA_SET_DISTANCE,			//有效距离范围由具体产品决定。默认1.5米。参数类型：浮点型。
	ITA_SET_EMISS,				//设置发射率(工业专用)0.01-1.0，默认0.95. 参数类型：浮点型。	
	ITA_SET_TRANSMIT,			//透过率(工业专用，0.01-1, default 1)。参数类型：浮点型。
	ITA_REFLECT_TEMP,			//反射温度(工业专用)。参数类型：浮点型。
	ITA_SET_ENVIRON,			//设置环境温度。在用户不设置的情况下，内部自动计算环温。如果用户设置，则使用设置的值。参数类型：浮点型。
	ITA_GET_ENVIRON,			//获取环温。
	ITA_GET_FPA_TEMP,			//获取焦温。
	ITA_ENVIRON_CORR,			//环温修正开关。默认关闭。参数类型：ITA_SWITCH。
	ITA_LENS_CORR,				//镜筒温漂校正开关。默认打开。参数类型：ITA_SWITCH。
	ITA_DISTANCE_COMPEN,		//距离补偿开关。默认打开。参数类型：ITA_SWITCH。
	ITA_EMISS_CORR,				//发射率校正开关。默认打开。参数类型：ITA_SWITCH。
	ITA_TRANS_CORR,				//透过率校正开关。默认关闭。参数类型：ITA_SWITCH。
	ITA_LOW_LENS_CORR_K,		//实时常温档镜筒温漂修正系数。参数类型：浮点型。
	ITA_HIGH_LENS_CORR_K,		//实时高温档镜筒温漂修正系数。参数类型：浮点型。
	ITA_SHUTTER_CORR,			//快门温漂校正开关。默认关闭。参数类型：ITA_SWITCH。
	ITA_LOW_SHUTTER_CORR_K,		//常温档快门校正系数。参数类型：浮点型。
	ITA_HIGH_SHUTTER_CORR_K,	//高温档快门校正系数。参数类型：浮点型。
	ITA_SUB_AVGB,				//测温减去本底均值开关。默认关闭。参数类型：ITA_SWITCH。
	ITA_GET_AVGB,				//获取本底均值。参数类型：short。
	ITA_CENTRAL_TEMPER  		//获取滤波后的中心温。参数类型：浮点型。
}ITA_MC_TYPE;	//Measure control type

typedef enum ITAPixelFormat
{
	ITA_RGB888,			//默认像素格式
	ITA_BGR888,
	ITA_RGBA8888,
	ITA_RGB565,
	ITA_YUV422_YUYV,
	ITA_YUV422_UYVY,
	ITA_YUV422_Plane,
	ITA_YVU422_Plane,
	ITA_YUV420_YUYV,//NV12
	ITA_YUV420_UYVY,//NV21
	ITA_YUV420_Plane,
	ITA_YVU420_Plane
}ITA_PIXEL_FORMAT;

/**
* 13种伪彩：0白热，1熔岩，2铁红，3热铁，4医疗，5北极，6彩虹1，7彩虹2，8黑热，9人体筛查，10描红，11蓝热，12，绿热。
*/
typedef enum ITAPaletteType
{
	ITA_WHITE_HEAT = 0,		//默认白热
	ITA_LAVA = 1,			//熔岩
	ITA_IRON_RED = 2,		//铁红
	ITA_HOT_IRON = 3,		//热铁
	ITA_MEDICAL_TREAT = 4,	//医疗
	ITA_ARCTIC = 5,			//北极
	ITA_RAINBOW = 6,		//彩虹
	ITA_RAINBOW_EX = 7,		//彩虹2
	ITA_BLACK_HOT = 8,		//黑热
	ITA_BODY_SCREEN = 9,	//人体筛查
	ITA_RED_TINT = 10,		//描红
	ITA_BLUE_HOT = 11,		//蓝热
	ITA_GREEN_HOT = 12		//绿热
}ITA_PALETTE_TYPE;

typedef enum ITARotateType
{
	ITA_ROTATE_NONE,	//不旋转
	ITA_ROTATE_90,		//旋转90°
	ITA_ROTATE_180,
	ITA_ROTATE_270
}ITA_ROTATE_TYPE;

typedef enum ITAFlipType
{
	ITA_FLIP_NONE,			//不翻转
	ITA_FLIP_HORIZONTAL,	//水平翻转
	ITA_FLIP_VERTICAL,		//垂直翻转 
	ITA_FLIP_HOR_VER		//水平+垂直翻转
}ITA_FLIP_TYPE;

typedef enum ITAICType
{
	ITA_SET_FORMAT,			//设置成像像素格式。支持动态设置。参数类型：ITA_PIXEL_FORMAT。
	ITA_SET_PALETTE,		//设置伪彩，通过接口ITA_Version获取伪彩号范围。参数类型：ITA_PALETTE_TYPE。
	ITA_SET_SCALE,			//缩放倍数。支持图像动态实时缩放。参数类型：浮点。
	ITA_SET_CONTRAST,		//设置对比度。对比度0-511。默认255。参数类型：整形。
	ITA_SET_BRIGHTNESS,		//设置亮度。亮度0-255。默认70。参数类型：整形。
	ITA_SET_ROTATE,			//设置旋转方式，RGB和Y16数据均旋转。参数类型：ITA_ROTATE_TYPE。
	ITA_SET_FLIP,			//设置翻转，RGB和Y16数据均翻转。参数类型：ITA_FLIP_TYPE。
	ITA_SET_REVERSE,		//打开或者关闭反向处理。ITA_X16模式下默认打开。参数类型：bool。
	ITA_TIME_FILTER,		//时域滤波开关，可以关闭或者打开。参数类型：bool。
	ITA_SPATIAL_FILTER,		//空域滤波开关，降噪，可以关闭或者打开。参数类型：bool。
	ITA_REMOVE_VERTICAL,	//去竖纹，可以关闭或者打开。参数类型：bool。
	ITA_REMOVE_HORIZONTAL,	//去横纹，可以关闭或者打开。参数类型：bool。
	ITA_SHARPENING,			//锐化，可以关闭或者打开。参数类型：bool。
	ITA_DETAIL_ENHANCE,		//细节增强，可以关闭或者打开。参数类型：bool。
	ITA_BLOCK_HISTOGRAM,	//分块直方图，可以关闭或者打开。参数类型：bool。
	ITA_GAMMA_CORRECTION,	//Gamma校正，可以关闭或者打开。参数类型：bool。
	ITA_Y8_ADJUSTBC,		//Y8纠偏，可以关闭或者打开。参数类型：bool。
	ITA_DRT_Y8,				//调光，默认打开。如果关闭，那么不处理Y16转Y8，不会生成imgDst图像数据。参数类型：bool。
	ITA_TFF_STD,            //时域滤波标准差，默认值10。参数类型：整形。
	ITA_RESTRAIN_RANGE,     //调光抑制范围，默认值64。参数类型：整形。
	ITA_ISP_ACCELERATE		//图像处理加速，可以关闭或者打开。参数类型：bool。
}ITA_IC_TYPE;	//Image control type

/*Dynamic range transform type 动态范围转换类型*/
typedef enum ITADRTType
{
	ITA_DRT_LINEAR = 0,		//线性调光，默认
	ITA_DRT_PLATHE,			//平台直方图均衡
	ITA_DRT_MIX,			//混合调光
	ITA_DRT_MANUAL			//手动调光
}ITA_DRT_TYPE;

typedef struct ITA_DRT_PARAM
{
	//手动调光参数
	short manltone_maxY16;
	short manltone_minY16;
}ITADRTParam;

typedef enum ITAEqualLineType
{
	ITA_EQUAL_NONE,		//关闭等温线功能
	ITA_EQUAL_HIGH,
	ITA_EQUAL_LOW,
	ITA_EQUAL_HIGHLOW,
	ITA_EQUAL_MIDDLE
}ITA_EQUAL_LINE_TYPE;

typedef struct ITA_EQUAL_LINE_PARAM
{
	short highY16;				//高Y16
	short lowY16;				//低Y16
	int color;					//等温线的一种颜色，rgb888格式。
	int otherColor;				//等温线的另一种颜色，rgb888格式。
}ITAEqualLineParam;

typedef enum ITADebugType
{
	ITA_SET_DEBUGGING,			//设置是否开启调试。参数类型：ITA_SWITCH。ITA_DISABLE，关闭调试。ITA_ENABLE，开启调试。如果未设置日志路径，那么默认保存在程序同级目录下ITA.log。
	ITA_DEBUGGING_PATH,			//设置日志保存的完整路径。注意要在ITA_Init之后调用。必须是已存在的目录，例如/home/app/debug.log。Windows例如"D:/Tools/debug.log"。长度不要超过128。参数类型：字符串，以 \0 结束。
	ITA_DEBUGGING_MODE,			//设置日志输出模式。参数类型：整形。1，日志输出到文件，默认输出方式。2，日志输出到控制台。3，用户自定义日志函数的情况下，同时输出到文件。支持用户自定义输出日志，见ITARegistry。
	ITA_SNAPSHOT,				//拍照保存当前帧图像。如果有设置ITA_DEBUGGING_PATH，那么存储在设置的路径，否则存储在程序目录下。
	ITA_START_RECORD,			//开始录X16/Y16数据。
	ITA_STOP_RECORD,			//停止录X16/Y16数据。
	ITA_MEMORY_USAGE,			//查询动态内存使用量。参数类型：整型。单位：Byte。
	ITA_ISP_CONSUME				//查询执行一次图像处理消耗的时间。参数类型：整型。单位：ms。
}ITA_DEBUG_TYPE;	//Debug type

typedef enum ITACorrectType
{
	ITA_TRY_CORRECT_POINT,	//先尝试标定坏点，实时图像确认标定成功后再添加。参数类型：ITA_POINT。
	ITA_CORRECT_POINT,		//添加坏点，可以多次添加。参数类型：ITA_POINT。
	ITA_SAVE_POINTS 		//保存坏点。待坏点添加完成后，保存指定测温范围下的坏点。参数类型：ITA_RANGE。
}ITA_CORRECT_TYPE;

typedef struct ITA_POINT {
	int x;
	int y;
}ITAPoint;

typedef enum ITACollectType
{
	ITA_COLLECT_DISTANCE,		//距离作为校温参数。
	ITA_COLLECT_ENVIRON 		//环温作为校温参数。
}ITA_COLLECT_TYPE;

typedef struct ITA_COLLECT_PARAM
{
	ITA_COLLECT_TYPE type;	//参数类型
	float param;			//参数值
	float blackTemp;		//黑体温度
}ITACollectParam;

typedef struct ITA_CORRECT_PARAM
{
	int BlackBodyNumber;
	float TBlackBody[10];
	short Y16[10];
	float ErrorNoCorrect[10];
	float ErrorCorrect[10];
}ITACalibrateResult;

typedef void(*ITACollectFinish)(void *param);

typedef void(*ITAReviewFinish)(void *param);

/*校温参数有两组，一组自动校温的参数和一组手动校温的参数。*/
typedef enum ITACalibrateType
{
	ITA_NEAR_KF_AUTO,
	ITA_NEAR_B_AUTO,
	ITA_FAR_KF_AUTO,
	ITA_FAR_B_AUTO,
	ITA_NEAR_KF_MANUAL,
	ITA_NEAR_B_MANUAL,
	ITA_FAR_KF_MANUAL,
	ITA_FAR_B_MANUAL
}ITA_CALIBRATE_TYPE;

/*描述版本号和功能信息。不同的版本包含的功能可能存在差异。*/
typedef struct ITA_VERSION_INFO
{
	char version[64];				//版本号
	int totalNumberOfPalettes;		//伪彩数量
	int totalNumberOfGears;			//焦温档位数量
	char confVersion[64];
}ITAVersionInfo;

/*定义事件通知机制中的事件类型*/
typedef enum ITAEventType
{
	ITA_NUC_BEGIN,		//开始做NUC
	ITA_NUC_END,		//完成一次NUC
	ITA_SHUTTER_BEGIN,	//开始快门
	ITA_SHUTTER_END,	//完成一次打快门
	ITA_AF_BEGIN,		//开始调焦
	ITA_AF_END			//完成调焦，参数eventParam指示调焦成功或者失败。参数类型：ITA_RESULT。
}ITA_EVENT_TYPE;

/*注册机制，在ITA_Init之后调用。ITA使用HAL functions完成初始化。*/
typedef struct ITA_REGISTRY
{
	/**
	* @brief:	读配置文件ITA.conf。可选，在没有配置文件的情况下ITA使用默认参数。
	* @param:	int cameraID			标识设备，支持同时打开多台设备。
	* @param:	unsigned char *buf		缓存首地址，输入参数。
	* @param:	int bufSize				缓存大小，输入参数。
	* @param:	int *length				配置文件长度，输出参数。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @note 配置文件是只读的。未经授权的修改无效。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*ReadConfig)(int cameraID, unsigned char *buf, int bufSize, int *length, void *userParam);

	/**
	* Register log function. Output logs using registered function.
	* @param:	int cameraID			标识设备，支持同时打开多台设备。
	* @param:	const char *logInfo		一行日志信息。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @note 当用户注册输出日志函数后，ITA内部不再输出日志。除非用户设置ITA_DEBUGGING_MODE为3。
	*/
	void(*OutputLog)(int cameraID, const char *logInfo, void *userParam);

	/*************************HAL接口定义**************************************************/
	/* Register HAL(Hardware Abstract Layer) functions. Users implement HAL functions 
	 * according to product requirements. Not every interface must be implemented. Please 
	 * refer to the integration documentation for which interfaces need to be implemented.*/
	/**************************************************************************************/
	/**
	* @brief:	控制快门。
	* @param:	int cameraID				标识设备，支持同时打开多台设备。
	* @param:	ITA_SHUTTER_STATUS status	快门状态。
	* @param:	void *userParam				用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*ShutterControl)(int cameraID, ITA_SHUTTER_STATUS status, void *userParam);

	/**
	* @brief:	做NUC。带MCU的产品须实现。
	* @param:	int cameraID				标识设备，支持同时打开多台设备。
	* @param:	void *userParam				用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*DoNUC)(int cameraID, void *userParam);

	/**
	* @brief:	获取温度传感器值。由ITA_SENSOR_VALUE决定输出AD值还是温度值。
	* @param:	int cameraID					标识设备，支持同时打开多台设备。输入参数。
	* @param:	ITA_TEMP_TYPE eType				温传类型。输入参数。
	* @param:	ITA_SENSOR_VALUE *sensorValue	值类型。输出参数，由用户决定。
	* @param:	int *ADValue					AD值，输出参数。当ITA_AD_VALUE==sensorValue时有效。
	* @param:	float *tempValue				温度值，输出参数。当ITA_TEMP_VALUE==sensorValue时有效。
	* @param:	void *userParam					用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*GetSensorTemp)(int cameraID, ITA_TEMP_TYPE eType, ITA_SENSOR_VALUE *sensorValue, int *ADValue, float *tempValue, void *userParam);

	/**
	* @brief:	打开I2C。Timo 256通过I2C配置探测器。
	* @param:	int cameraID				标识设备，支持同时打开多台设备。
	* @param:	unsigned char u32DevAddr
	* @param:	void *userParam				用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*I2COpen)(int cameraID, unsigned char u32DevAddr, void *userParam);

	/**
	* @brief:	关闭I2C。Timo 256通过I2C配置探测器。
	* @param:	int cameraID			标识设备，支持同时打开多台设备。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*I2CClose)(int cameraID, void *userParam);

	/**
	* @brief:	I2C读寄存器。Timo 256通过I2C配置探测器。
	* @param:	int cameraID			标识设备，支持同时打开多台设备。
	* @param:	unsigned char u8Addr	寄存器地址。
	* @param:	unsigned char * pu8Val	读出的寄存器值。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*I2CRead)(int cameraID, unsigned char u8Addr, unsigned char * pu8Val, void *userParam);

	/**
	* @brief:	I2C写寄存器。Timo 256通过I2C配置探测器。
	* @param:	int cameraID			标识设备，支持同时打开多台设备。
	* @param:	unsigned char u8Addr	寄存器地址。
	* @param:	unsigned char u8Val		写入的寄存器值。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*I2CWrite)(int cameraID, unsigned char u8Addr, unsigned char u8Val, void *userParam);

	/**
	* @brief:	SPI传输数据，先写入再读。Timo 120通过SPI配置探测器。
	* @param:	int cameraID			 标识设备，支持同时打开多台设备。
	* @param:	unsigned char *writeBuf  待写入的数据缓存。
	* @param:	unsigned char *readBuf   读数据的缓存。
	* @param:	int bufLen               缓存长度。
	* @param:	void *userParam			 用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*SPITransmit)(int cameraID, unsigned char *writeBuf, unsigned char *readBuf, int bufLen, void *userParam);

	/**
	* @brief:	SPI写数据。Timo 120通过SPI配置探测器。
	* @param:	int cameraID			 标识设备，支持同时打开多台设备。
	* @param:	unsigned char *writeData 待写入的数据。
	* @param:	int writeLen             数据长度。
	* @param:	void *userParam			 用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*SPIWrite)(int cameraID, unsigned char *writeData, int writeLen, void *userParam);

	/**
	* @brief:	模组内置Flash的GPIO口拉低，开始更新寄存器值。使用内置Flash的模组产品需要实现此函数。
	* @param:	int cameraID			 标识设备，支持同时打开多台设备。
	* @param:	void *userParam			 用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*InternalFlashPrepare)(int cameraID, void *userParam);

	/**
	* @brief:	模组内置Flash的GPIO口拉高，结束更新寄存器值。使用内置Flash的模组产品需要实现此函数。
	* @param:	int cameraID			 标识设备，支持同时打开多台设备。
	* @param:	void *userParam			 用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return:	返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*InternalFlashDone)(int cameraID, void *userParam);

	/**
	* @brief:  读数据包。
	* @param:  int cameraID			标识设备，支持同时打开多台设备。
	* @param:  unsigned char *buf	数据缓存
	* @param:  int offset			相对数据包头的偏移，从偏移处开始读size长度的数据。
	* @param:  int size				读取的数据长度
	* @param:  ITA_RANGE range		测温范围，共有3档。
	* @param:	void *userParam		用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*ReadPackageData)(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam);

	/**
	* @brief:  写数据包。
	* @param:  int cameraID			标识设备，支持同时打开多台设备。
	* @param:  unsigned char *buf	数据缓存
	* @param:  int offset			相对数据包头的偏移，从偏移处开始读size长度的数据。
	* @param:  int size				写的数据长度
	* @param:  ITA_RANGE range		测温范围，共有3档。
	* @param:	void *userParam		用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*WritePackageData)(int cameraID, unsigned char *buf, int offset, int size, ITA_RANGE range, void *userParam);

	/**
	* @brief:  读取校温参数。
	* @param:  int cameraID				标识设备，支持同时打开多台设备。
	* @param:  ITA_RANGE range			测温范围，共有3档。
	* @param:  ITA_CALIBRATE_TYPE type	参数类型。
	* @param:  float *p					参数指针。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*ReadCalibrateParam)(int cameraID, ITA_RANGE range, ITA_CALIBRATE_TYPE type, float *p, void *userParam);

	/**
	* @brief:  保存校温参数。
	* @param:  int cameraID				标识设备，支持同时打开多台设备。
	* @param:  ITA_RANGE range			测温范围，共有3档。
	* @param:  ITA_CALIBRATE_TYPE type	参数类型。
	* @param:  float value				参数值。
	* @param:	void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*WriteCalibrateParam)(int cameraID, ITA_RANGE range, ITA_CALIBRATE_TYPE type, float value, void *userParam);

	/**
	* @brief:  发送PWM脉冲信号。
	* @param:  int cameraID     标识设备，支持同时打开多台设备。
	* @param:  int dutyration	占空比，一个脉冲周期内，高电平的时间与整个周期时间的比例。
	* @param:  int engage		波形使能输出，1，输出，0，不输出。
	* @param:  void *userParam	用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*PWMDrive)(int cameraID, int dutyration, int engage, void *userParam);

	/**
	* @brief:  写EEPROM自动调焦参数。
	* @param:  int cameraID			标识设备，支持同时打开多台设备。
	* @param:  unsigned short addr	地址
	* @param:  unsigned char data	数据
	* @param:  void *userParam		用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*EEPROMWrite)(int cameraID, unsigned short addr, unsigned char data, void *userParam);

	/**
	* @brief:  读EEPROM自动调焦参数。
	* @param:  int cameraID			标识设备，支持同时打开多台设备。
	* @param:  unsigned short addr	地址
	* @param:  unsigned char *data	数据指针
	* @param:  void *userParam		用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*EEPROMRead)(int cameraID, unsigned short addr, unsigned char *data, void *userParam);

	/**
	* @brief:  事件通知机制。当有NUC、快门和AF等事件发生时，通知用户。有些事件带有参数。
	* @param:  int cameraID				标识设备，支持同时打开多台设备。
	* @param:  ITA_EVENT_TYPE eventType	事件类型
	* @param:  void *eventParam			事件参数
	* @param:  void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。用户实现事件通知函数时须立即返回，否则影响ITA运行效率。
	**/
	ITA_RESULT(*NotifyEvent)(int cameraID, ITA_EVENT_TYPE eventType, void *eventParam, void *userParam);

	/**
	* @brief:  冷热机判断等部分功能需要读写私有数据。如果产品需要此功能，那么请用户实现在存储介质上读写私有数据。
	* @param:  int cameraID				标识设备，支持同时打开多台设备。
	* @param:  unsigned char *readBuf	数据缓存
	* @param:  int readLen  			缓存长度
	* @param:  void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*ReadPrivateData)(int cameraID, unsigned char *readBuf, int readLen, void *userParam);

	/**
	* @brief:  冷热机判断等部分功能需要读写私有数据。如果产品需要此功能，那么请用户实现在存储介质上读写私有数据。
	* @param:  int cameraID				标识设备，支持同时打开多台设备。
	* @param:  unsigned char *writeBuf	数据缓存
	* @param:  int writeLen  			缓存长度
	* @param:  void *userParam			用户参数，注册时由用户传入。ITA库调用注册函数时再传给用户。
	* @return: 返回值类型：ITA_RESULT。成功，返回ITA_OK；失败，返回值<0，参考ERROR.h。
	* @see ITA_Init，camera id由用户输入。
	**/
	ITA_RESULT(*WritePrivateData)(int cameraID, unsigned char *writeBuf, int writeLen, void *userParam);

	/*用户参数，调用注册函数时再传给用户。*/
	void *userParam;
}ITARegistry;

typedef struct ITA_PARAM_INFO
{
	ITA_RANGE range;
	int fpaGear; //焦温档位
	bool isAutoGear; //自动切档开关
	bool isReverse; //是否反向的开关
	bool isSubAvgB; //减去本底均值的开关
	bool isDebug; //调试开关
	ITA_DRT_TYPE drtType; //调光方式
	ITA_EQUAL_LINE_TYPE equalLineType; //等温线类型
	int frameRate; //用户设置的帧率
	float distance;
	float emiss;
	float transmit;
	float reflectTemp;
	ITA_SWITCH environCorr;
	ITA_SWITCH lensCorr;
	ITA_SWITCH distanceCompen;
	ITA_SWITCH emissCorr;
	ITA_SWITCH transCorr;
	float lowLensCorrK;
	float highLensCorrK;
	ITA_SWITCH shutterCorr;
	float lowShutterCorrK;
	float highShutterCorrK;
	ITA_PIXEL_FORMAT format;
	ITA_PALETTE_TYPE palette;
	float scale;
	int contrast;
	int brightness;
	ITA_ROTATE_TYPE rotate;
	ITA_FLIP_TYPE flip;
	bool isTimeFilter;	//时域滤波开关，可以关闭或者打开。
	bool isSpatialFilter;	//空域滤波开关，降噪，可以关闭或者打开。
	bool isRemoveVertical;//去竖纹，可以关闭或者打开。
	bool isRemoveHorizontal;//去横纹，可以关闭或者打开。
	bool isSharpening;	//锐化，可以关闭或者打开。
	bool isDetailEnhance;	//细节增强，可以关闭或者打开。
	bool isBlockHistogram;//分块直方图，可以关闭或者打开。
	bool isGammaCorrection;//Gamma校正，可以关闭或者打开。
	bool isY8AdjustBC;	//Y8纠偏，可以关闭或者打开。
	int tffStd;	//时域滤波标准差
	int restrainRange; //调光抑制范围
	float nearKFAuto;
	float nearBAuto;
	float farKFAuto;
	float farBAuto;
	float nearKFManual;
	float nearBManual;
	float farKFManual;
	float farBManual;
	float orinalShutterTemp;//开机快门温度
	float currentShutterTemp;//当前打快门时的快门温度
	float realTimeShutterTemp;//实时快门温度
	float realTimeLensTemp;//实时镜筒温度
	float currentLensTemp;//当前打快门时的镜筒温度
	float realTimeFpaTemp;//实时焦平面温度
}ITAParamInfo;

/*后处理函数，主要用于Android平台线程中通过postHandle通知应用层释放资源。*/
typedef void(*PostHandle)();

#endif // !ITA_DTD_H