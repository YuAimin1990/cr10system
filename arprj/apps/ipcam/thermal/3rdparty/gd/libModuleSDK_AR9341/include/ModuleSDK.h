/*************************************************
Copyright (C), 2021--2030, GuideIR Tech. Co., Ltd.
File name	: ModuleSDK.h
Author		: Guide Sensmart RD. wangyan
Version		: 2.0
Date		: 2021/3/9
Description	:
*************************************************/
#ifndef MODULE_SDK_H
#define MODULE_SDK_H

#ifdef __cplusplus
#    define MODULE_SDK_START  extern "C" {
#    define MODULE_SDK_END    };
#else
#    define MODULE_SDK_START
#    define MODULE_SDK_END
#endif

MODULE_SDK_START

#ifdef WINDOWS_PLATFORM
#    define MODULEAPI __declspec(dllexport)
#else
#    define MODULEAPI
#endif // WINDOWS_PLATFORM

#ifdef WINDOWS_PLATFORM
typedef void *				HANDLE_T;	//兼容32bit和64bit操作系统
#else
typedef unsigned long long	HANDLE_T;
#endif // WINDOWS_PLATFORM

/* general return values */
#define GUIDEIR_OK			(0)
#define GUIDEIR_ERR			(-1)
#define GUIDEIR_NULL		(0)//((VOID *)0)

typedef enum
{
    MODE_X16,
    MODE_Y16,
    MODE_ASIC_Y16,	//ASIC DVP输出Y16+YUV+参数行。
    MODE_FPGA_Y16,
    MODE_ASIC_Y16_2,	//ASIC USB输出帧头64B+Y16+参数行。
}MODE_TYPE;

typedef enum
{
    IMG_FORMAT_YUV422YUYV  = 0,  //YUV422格式YUYV
    IMG_FORMAT_YUV422UYVY  = 1,  //YUV422格式UYVY
    IMG_FORMAT_YUV422Plane = 2,  //YUV422格式YUV422Plane
    IMG_FORMAT_YVU422Plane = 3,  //YUV422格式YVU422Plane
    IMG_FORMAT_YUV420NV12  = 4,  //YUV420格式NV12
    IMG_FORMAT_YUV420NV21  = 5,  //YUV420格式NV21
    IMG_FORMAT_YUV420PLANE = 6,  //YUV420格式YUV420Plane
    IMG_FORMAT_YVU420PLANE = 7,  //YUV420格式YVU420Plane
    IMG_FORMAT_RGB888      = 20, //RGB格式图像
    IMG_FORMAT_RGBA8888    = 21, //RGBA格式图像
    IMG_FORMAT_BGR888      = 22, //BGR格式图像
    IMG_FORMAT_RGB565      = 23, //RGB565格式图像
}IMG_FORMAT;

/*PARAMETER_TYPE定义的参数，如果没有强调要在RegisterImgCallBack之前调用，那么请在RegisterImgCallBack之后调用，否则不生效。*/
typedef enum
{
    SHUTTER_NOW = 1,	//打快门
    SET_AUTO_SHUTTER,	//0，关闭自动快门。1，打开自动快门。默认打开自动快门。
    SET_SHUTTER_TIME,	//设置自动快门时间，单位秒。默认30秒。
    DO_NUC,
    SET_DISTANCE,		//距离0.5~2.5米。可以不设置，默认1.5米。参数类型：浮点型。
    GET_DISTANCE,
    SET_EMISS,			//设置发射率0.01-1.0，默认0.95. 参数类型：浮点型。
    GET_EMISS,
    SET_DEBUGGING,		//设置是否开启调试。参数类型：整型。0，关闭调试。1，开启调试。调试信息默认输出到日志文件debug_out.log。日志文件默认保存在程序同级目录下。
    DEBUGGING_PATH,	    //设置日志保存的完整路径。注意要在RegisterImgCallBack之前调用。必须是已存在的目录，例如/home/app/debug.log。Windows例如"D:/Tools/debug.log"。长度不要超过128。参数类型：字符串，以 \0 结束。
    DEBUGGING_MODE,		//设置日志输出模式。1，日志输出到文件，默认输出方式。2，日志输出到控制台。
    GRAB_FRAME,			//开始录X16，Y16数据。每秒录一帧。录满8MB后停止。
    SNAPSHOT,			//拍照保存当前这帧图像，存储在程序的同级目录下。
    CLOSE_SHUTTER_POLICY,	//关闭快门、NUC、切档策略。无参数。在MODE_ASIC_Y16模式下默认关闭。在RegisterImgCallBack之前调用。
    SET_TRANSMIT,		//透过率(工业专用，0.01-1, default 1)。参数类型：浮点型。
    GET_TRANSMIT,
    SET_HUMIDITY,		//湿度(0.01-1.0，默认0.6)。参数类型：浮点型。
    GET_HUMIDITY,
    SET_REFLECT_TEMP,		//反射温度(工业专用)。参数类型：浮点型。
    GET_REFLECT_TEMP,
    SET_AMBIENT_TEMP,       //设置环境温度。人体测温专用。参数类型：浮点型。
    GET_AMBIENT_TEMP,       //获取环境温度。用于人体测温。参数类型：浮点型。环境温度在用户不设置的情况下，SDK内部自动计算环温。如果用户设置，则使用设置的值。
    SET_ENVIRONMENT_CORR,	//环温修正开关。默认关闭。参数类型：整型。
    SET_LENS_CORR,		    //镜筒温漂校正开关。默认关闭。参数类型：整型。
    SET_DISTANCE_COMPEN,	//距离补偿开关。默认打开。参数类型：整型。
    SET_EMISS_CORR,			//发射率校正开关。默认打开。参数类型：整型。
    SET_TRANS_CORR,			//透过率校正开关。默认关闭。参数类型：整型。
    SET_HUMID_CORR,			//湿度校正开关。默认关闭。参数类型：整型。
    SET_LOW_LENS_CORR_K,    //实时常温档镜筒温漂修正系数。参数类型：浮点型。
    SET_HIGH_LENS_CORR_K,   //实时高温档镜筒温漂修正系数。参数类型：浮点型。
    SET_SHUTTER_CORR,	    //快门温漂校正开关。默认关闭。参数类型：整型。
    SET_LOW_SHUTTER_CORR,   //常温档快门校正系数。参数类型：浮点型。
    SET_HIGH_SHUTTER_CORR,  //高温档快门校正系数。参数类型：浮点型。
    SET_DISTANCE_CORR,       //距离校正系数。参数类型：DistanceCalibParaS*
    SET_COLDSTART_SHUTTER_TEMP, //设置冷开机快门温
    GET_COLDSTART_SHUTTER_TEMP, //获取冷开机快门温

    SET_BADPOINT_SWITCH,        //设置坏点替换开关。参数类型：整型。设置图像回调之后生效。 0：关闭，1：打开。以下相同
    GET_BADPOINT_SWITCH,        //获取坏点替换开关状态
    SET_TEMPORAL_FILTER_SWITCH, //设置时域滤波开关
    GET_TEMPORAL_FILTER_SWITCH, //获取时域滤波开关状态
    SET_SPATIAL_FILTER_SWITCH,  //设置空域滤波开关
    GET_SPATIAL_FILTER_SWITCH,  //获取空域滤波开关状态
    SET_HORIZON_STRIPE_SWITCH,  //设置去横纹开关
    GET_HORIZON_STRIPE_SWITCH,  //获取去横纹开关状态
    SET_VERTICAL_STRIPE_SWITCH, //设置去竖纹开关
    GET_VERTICAL_STRIPE_SWITCH, //获取去竖纹开关状态
    SET_DRT_SWITCH,             //设置调光开关（是否输出YUV/RGB图像）
    GET_DRT_SWITCH,             //获取调光开关状态
}PARAMETER_TYPE;

typedef enum
{
    SET_FORMAT = 1,		//设置成像格式。参数类型：整形。参数值参考IMG_FORMAT枚举；默认格式IMG_FORMAT_RGB888。在RegisterImgCallBack之后设置。
    SET_PALETTE,		//设置伪彩，范围0-11. 默认0. 一共12条伪彩，分别是：0白热，1熔岩，2铁红，3热铁，4医疗，5北极，6彩虹1，7彩虹2，8黑热，9人体筛查，10描红，11蓝热。
    GET_PALETTE,
    GET_SCALE,			//获取缩放倍数。RegisterImgCallBack时设置缩放倍数。
    SET_CONTRAST,		//设置对比度。对比度0-255。默认255. 参数类型：整形。
    GET_CONTRAST,
    SET_BRIGHTNESS,		//设置亮度。亮度0-255。默认70. 参数类型：整形。
    GET_BRIGHTNESS,
    SET_ROTATION,		//设置旋转方式，默认0。0，不旋转。1，旋转90度。2，旋转180度。3，旋转270度。
    GET_ROTATION,		//设置旋转后RGB和Y16数据均旋转。
    SET_FLIP,			//设置是否左右翻转，默认0。0，不翻转。1，左右翻转。
    GET_FLIP,			//设置翻转后RGB和Y16数据均翻转。
    SET_FRAME_RATE,		//设置帧率
    GET_FRAME_RATE,		//获取实际帧率
    CLOSE_IMAGE_PROCESS,//关闭图像处理。无参数。仅在MODE_ASIC_Y16模式下使用。在RegisterImgCallBack之前调用。
    CLOSE_YUV_PROCESS,	//关闭分离出YUV和转RGB处理。仅在MODE_ASIC_Y16模式下使用。例如，MCU只需要测温，不用处理YUV，关闭后提高效率。
}IMG_PARAM_TYPE;

typedef struct OUT_PUT_DETECTOR_CONF {
    unsigned char addr;
    unsigned char regValue;
}DetectorConf;

typedef struct OUT_PUT_IR_DATA {
    int width;
    int height;
    unsigned char *yuvData; //由SET_FORMAT指定输出格式。
    int yuvLength;
    short *y16Data;
    int y16Length;
}CallBackData;

typedef enum
{
    CalibrateTypeNearKf = 1,
    CalibrateTypeNearB = 2,
    CalibrateTypeFarKf = 3,
    CalibrateTypeFarB = 4,
}CalibrateTempType;

typedef struct
{
    //常温档
    float         fNearKf;	//近距离kf系数
    float         fNearB ;	//近距离偏置系数
    float         fFarKf ;	//远距离kf系数
    float         fFarB  ;	//远距离偏置系数
    //高温档
    float         fNearKf2;	//近距离kf系数
    float         fNearB2 ;	//近距离偏置系数
    float         fFarKf2 ;	//远距离kf系数
    float         fFarB2  ;	//远距离偏置系数
}CalibrateTempParaS;

typedef struct
{
    float  c1;
    float  c2;
    float  c3;
    float  c4;
    float  c5;
    float  c6;
}DistanceCalibParaS; //距离校正系数

//注意：用户在实现回调函数时，最好只拷贝数据到缓存就返回。切勿做其它耗时操作，否则影响帧率。
typedef int (*ImageCallBack)(CallBackData callBackData, void *param);
typedef void(*PreHandle)();
typedef void(*PostHandle)();

/**
* @brief:	设置usb模组文件描述符。
* @param:	HANDLE_T fd	文件描述符
* @note:
**/
//MODULEAPI int SetUsbFileDescriptor(HANDLE_T fd);

/**
* @brief:	创建模组实例。
* @param:	const char *mode	模式，X16（探测器输出的原始数据）或Y16（前端图像算法处理过的数据）。如果传空，默认X16.
* @return:	返回实例句柄。
* @note:
**/
MODULEAPI HANDLE_T CreateModuleInstance(MODE_TYPE mode);

/**
* @brief:	销毁模组实例。
* @param:	HANDLE_T handle     实例句柄。
* @return:	无。
* @note:
**/
MODULEAPI void DestroyModuleInstance(HANDLE_T handle);

/**
* @brief:	设置探测器时钟参数。
* @param:	HANDLE_T handle     实例句柄。
* @param:	DetectorConf *pConf	探测器配置信息。参考示例代码或者探测器手册。
* @param:	int num				DetectorConf数量。
* @param:	int clock			探测器时钟参数，单位HZ。例如12000000HZ。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int SetDetectorClock(HANDLE_T handle, DetectorConf *pConf, int num, int clock);

/**
* @brief:	增加成像回调前后拦截器，主要用于成像回调存在独立线程中，可以通过preHandle和postHandle通知应用层分配和释放资源。用于Android平台。
* @param:	HANDLE_T handle		 	实例句柄。
* @param:	PreHandle preHandle  	成像数据回调前预处理函数
* @param:	PostHandle postHandle	成像数据回调完成后处理函数
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int AddImgInterceptor(HANDLE_T handle, PreHandle preHandle, PostHandle postHandle);

/**
* @brief:	注册成像回调函数，并设置缩放倍数。软件实现缩放会占用一定CPU，建议取到原始图像后利用硬件缩放。用户在实现回调函数时要注意，最好只拷贝数据到缓存就返回。切勿做其它耗时操作，否则影响帧率。
* @param:	HANDLE_T handle				 实例句柄。
* @param:	ImageCallBack imageCallBack  图像回调函数。
* @param:	void *param			         回调函数预留参数。
* @param:	float scaleTime 	         缩放倍数。设置1，不缩放。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int RegisterImgCallBack(HANDLE_T handle, ImageCallBack imageCallBack, void *param, float scaleTime);

/**
* @brief:	根据Y16值计算表面温度。
* @param:	HANDLE_T handle     实例句柄。
* @param:	short y16           Y16值。
* @param:	float *surfaceTemp  输出表面温度。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureTempByY16(HANDLE_T handle, short y16, float *surfaceTemp);

/**
* @brief: 计算人体温度值。
* @param: HANDLE_T handle     实例句柄。
* @param: float surfaceTemp   体表温度。
* @param: float envirTemp     环境温度。可以通过GET_AMBIENT_TEMP获取SDK计算的环温。注意受整机散热方面的影响，内部计算的环温与实际值有偏差，一般需要减去偏移量得到实际环温。
* @param: float *bodyTemp     输出人体温度。
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureBodyTemp(HANDLE_T handle, float surfaceTemp, float envirTemp, float *bodyTemp);

/**
* @brief:	计算温度矩阵。
* @param:	HANDLE_T handle     实例句柄。
* @param:	short* y16Array     Y16矩阵。
* @param:	x,y,w,h				目标矩形区域
* @param:	float *tempMatrix   输出温度矩阵。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureTempMatrix(HANDLE_T handle, short* y16Array, int x, int y, int w, int h, float *tempMatrix);

/**
* @brief:	计算温度矩阵，带距离参数。
* @param:	HANDLE_T handle     实例句柄。
* @param:	short* y16Array     Y16矩阵。
* @param:	float distance		距离参数
* @param:	x,y,w,h				目标矩形区域
* @param:	float *tempMatrix   输出温度矩阵。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureTempMatrixEx(HANDLE_T handle, short* y16Array, float distance, int x, int y, int w, int h, float *tempMatrix);

/**
* @brief:	设置测温参数。
* @param:	HANDLE_T handle				实例句柄。
* @param:	CMD_PARAMETER_TYPE type		参数类型。
* @param:	void *param					参数值，传地址。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int MeasureParamsControl(HANDLE_T handle, PARAMETER_TYPE type, void *param);

/**
* @brief:	设置成像参数。
* @param:	HANDLE_T handle				实例句柄。
* @param:	IMG_PARAM_TYPE type		    参数类型。
* @param:	void *param					参数值，传地址。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int ImageParamsControl(HANDLE_T handle, IMG_PARAM_TYPE type, void *param);

/**
* @brief:  设置测温范围。0表示人体测温，1表示工业测温常温段（默认值），2表示工业测温高温段。
* @param:  HANDLE_T handle	实例句柄。
* @param:  int range		测温范围。0表示人体测温，1表示工业测温常温段，2表示工业测温高温段。
* @return: 成功返回GUIDEIR_OK。如果失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int SetMeasureRange(HANDLE_T handle, int range);

/**
* @brief:	设置调光方式。
* @param:	HANDLE_T handle				实例句柄。
* @param:	int type					0，默认的调光方式；1，手动调光。
* @param:	float maxTemp				最高温度，手动调光时有效。
* @param:	float minTemp				最低温度，手动调光时有效。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int SetDimmingType(HANDLE_T handle, int type, float maxTemp, float minTemp);

//下面是日志模块的接口。
typedef void(*OutputFunc)(const char *logInfo);

/**
* @brief:  注册自定义日志函数。当用户注册日志函数后，SDK会使用注册的日志函数。同时，如果SDK当前使用输出日志到文件的方式，仍然有效。如果使用输出日志到控制台方式，则无效。
* @param:  HANDLE_T handle	实例句柄。
* @param:  OutputFunc func	自定义的日志输出函数。
* @return: 成功返回GUIDEIR_OK。如果失败返回GUIDEIR_ERR。
* @note:   SDK日志默认有两种输出方式：输出到文件或者控制台，具体参考DEBUGGING_MODE。
**/
MODULEAPI int RegisterLogFunc(HANDLE_T handle, OutputFunc func);

/**
* @brief:  查询SDK版本号。
* @param:  char *version				版本号
* @param:  int len						version缓存大小
* @return: 成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int SDKVersion(char *version, int len);

//以下是自动校温接口，详细的校温过程请参考校温流程说明文档
typedef struct
{
    int nBlackBodyNumber;		//校温温度点个数，最大十个
    float TBlackBody[10];		//采集的各个温度点温度值
    short Y16[10];				//各个温度点采集的Y16
    bool isNormalTemp;       //true: 为常温环境; false 为低温环境 23℃下常温  10℃环温低温
}CorrectTempPara;

typedef void(*CollectFinishCallBack)(int y16Value, void *param);
/**
* @brief:  开始采集数据。完成后用回调函数通知，用户需在回调函数中记录采集的温度点和对应的Y16到CorrectTempPara中
* @param:  HANDLE_T handle	实例句柄。
* @param:  CollectFinishCallBack cb 回调函数。
* @param:  void *param  用户私有参数，回传到回调函数之中。
* @return: 如果失败返回GUIDEIR_ERR。如果成功返回GUIDEIR_OK。
* @note:
**/
MODULEAPI int StartCollectTemp(HANDLE_T handle, CollectFinishCallBack cb, void *param);

/**
* @brief:  开始自动校温。
* @param:  HANDLE_T handle	实例句柄。
* @param:  param  采集数据
* @param:  float *Kf 输出校温参数
* @param:  float *B  输出校温参数
* @return: 如果失败返回GUIDEIR_ERR。如果成功返回GUIDEIR_OK。
* @note:
**/
MODULEAPI int StartCalibrateTemp(HANDLE_T handle, CorrectTempPara param, float *Kf, float *B);

/**
* @brief:  对当前档位手动校温。
* @param:  HANDLE_T handle	实例句柄。
* @param:  int type 参数类型，取自枚举CalibrateTempType。
* @param:  float p 参数值
* @return: 如果失败返回GUIDEIR_ERR。如果成功返回GUIDEIR_OK。
* @note:
**/
MODULEAPI int CalibrateTempByUser(HANDLE_T handle, int type, float p);

//温度复核回调接口
typedef void(*RecheckTempFinishCallBack)(bool result, void *param);

/**
* @brief:  开始温度复核，完成后用回调函数通知。
* @param:  HANDLE_T handle	实例句柄。
* @param:  float temper 复核温度点温度。
* @param:  RecheckTempFinishCallBack cb 回调函数。
* @param:  void *param  用户私有参数，回传到回调函数之中。
* @return: 如果失败返回GUIDEIR_ERR。如果成功返回GUIDEIR_OK。
* @note:
**/
MODULEAPI int StartRecheckTemp(HANDLE_T handle, float temper, RecheckTempFinishCallBack cb, void *param);

//以下是校坏点接口
typedef enum
{
    CORRECT_POINTS = 1,	//添加坏点，参数类型为GUIDE_POINT。
    SAVE_POINTS,		//保存坏点。参数为空。保存后之前在缓存中添加的坏点会被清掉。
}CMD_CORRECT_TYPE;

typedef struct OUT_PUT_POINT {
    int x;
    int y;
}GUIDE_POINT;

/**
* @brief:	校坏点。
* @param:	HANDLE_T handle				实例句柄。
* @param:	CORRECT_TYPE type			参数类型。
* @param:	void *param					参数值，传地址。
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int CorrectBadPoints(HANDLE_T handle, CMD_CORRECT_TYPE type, void *param);

/**
* @brief:	获取当前测温参数
* @param:	HANDLE_T handle				实例句柄。
* @return:	返回校温参数结构体CalibrateTempParaS
* @note:
**/
MODULEAPI CalibrateTempParaS GetCurrentCalibrateTempMtParams(HANDLE_T handle);

/**
* @brief:	恢复出厂设置
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int restoreModuleFactoryDefault(HANDLE_T handle);

/**
* @brief:	保存设置
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int saveModuleSettings(HANDLE_T handle);

/**
* @brief:	通过伪彩数据获取当前红外图像数据，只支持图像格式为RGB888
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI bool getImage(HANDLE_T handle, unsigned char *pImage, int *pPalette);

/**
* @brief:	添加自定义伪彩数据
* @param:	HANDLE_T handle				实例句柄。
* @param:	int count					自定义伪彩数据总个数。
* @param:	unsigned char* data			自定义伪彩数据
* @return:	成功返回GUIDEIR_OK。失败返回GUIDEIR_ERR。
* @note:
**/
MODULEAPI int addCustomPalette(HANDLE_T handle, int count, unsigned char* data);


MODULE_SDK_END

#endif // !MODULE_SDK_H

