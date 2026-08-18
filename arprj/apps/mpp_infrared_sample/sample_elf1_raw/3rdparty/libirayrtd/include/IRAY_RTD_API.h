#ifndef __IRAY_RTD_API_H__
#define __IRAY_RTD_API_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
typedef void 					*IR_HANDLE;

typedef unsigned char           IR_UCHAR;
typedef unsigned char           IR_U8;
typedef unsigned short          IR_U16;
typedef unsigned int            IR_U32;
typedef unsigned long long 		IR_U64;
typedef unsigned long           IR_UL;

typedef char                    IR_CHAR;
typedef signed char             IR_S8;
typedef short                   IR_S16;
typedef int                     IR_S32;
typedef long long				IR_S64;
typedef long                    IR_SL;
typedef double					IR_DOUBLE;

#define IR_VOID                 void

#define IR_FAILED					-1
#define IR_SUCCESS					0
#define IR_ERR_CODE_PARA_INVALID	-1001
#define IR_ERR_CODE_BUFFER_FULLED	-1002

typedef enum irayFRAME_TYPE_E
{
	IRAY_FRAME_TYPE_ORG = 0,	//14位灰阶图像数据
	IRAY_FRAME_TYPE_AGC,		//经本模块处理后的8位图像数据
	IRAY_FRAME_TYPE_NUC,
	IRAY_FRAME_TYPE_TEMP,		//温度数据
	IRAY_FRAME_TYPE_YVU422SP,
	IRAY_FRAME_TYPE_YUV422SP,
	IRAY_FRAME_TYPE_YUV422P,
	IRAY_FRAME_TYPE_YUV420P,
	IRAY_FRAME_TYPE_NUC_T,
	IRAY_FRAME_TYPE_BUTT,
}IRAY_FRAME_TYPE_E;

typedef enum irayFRAME_RATE_E
{
	IRAY_RTD_FRAME_RATE_25 = 0,
	IRAY_RTD_FRAME_RATE_30 = 1,
	IRAY_RTD_FRAME_RATE_BUTT
}IRAY_RTD_FRAME_RATE_E;

typedef struct irayFrame_INFO_S
{
	IRAY_FRAME_TYPE_E	enFrameType;	//视频帧类型
	IR_U32				u32DevID;		//红外模组ID号从0开始编号（默认0）
	IR_U32 				u32Width;		//视频分辨率宽度
	IR_U32 				u32Height;		//视频分辨率高度
	IR_U32				u32Len;			//数据长度
	IR_U32				u32Seq;			//帧序列		
	IR_U32				u32VTemp;		//for FD2
	IR_U64				u64Pts;
}IRAY_FRAME_INFO_S;

typedef struct irayPointTemp_INFO_S
{
	IR_U16		u16X;
	IR_U16		u16Y;
	IR_U16		u16Temp;
	IR_U16		u16Reserved;
}POINT_TEMP_INFO_S;

typedef struct IrayForeheadParams
{
    IR_DOUBLE      dbDtMin;	//校正参数最小值
    IR_DOUBLE      dbDtMax;	//校正参数最大值
    IR_DOUBLE      dbDtRatio;//校正系数默认1.0
}FOREHEAD_PARAMS_S;
//强光保护结构体
typedef struct IrayAlarmParams
{
    unsigned char  enableAlarm;	//1，使能强光保护，0：关闭强光保护
    IR_U32         s32AlarmThs;	//灵敏度【1，100】
    IR_U32         s32AlarmEngry;//能量值【0，100】
}ALARM_PARAMS_S;



typedef struct tgIspPicColor
{
	IR_U32 u32LumaVal;                  /* Luminance: [1 ~ 100] */
	IR_U32 u32ContrVal;                 /* Contrast: [1 ~ 100] */
#if 0
	IR_U32 u32HueVal;                   /* Hue: [1 ~ 100] */
	IR_U32 u32SatuVal;                  /* Satuature: [1 ~ 100] */
    IR_U32 u32Compen;                   /* Compensation:  [1 ~ 100] */
#endif
}IRAY_RTD_ISP_PIC_COLOR_S;

typedef struct tgIsp2DDeNoise
{
	IR_U32 u32Enable;
	IR_U32 u32Level; /*(0,100]*/
}IRAY_RTD_ISP_DE_NOISE_S;

typedef struct tgIspDDE
{
	IR_U32 u32Enable;
	IR_U32 u32Level; /*(0,100]*/
}IRAY_RTD_ISP_DDE_S;

typedef struct tgRegion
{
	IR_U32 x;
	IR_U32 y;
	IR_U32 width;
	IR_U32 height;
}IRAY_RTD_RECT_S;

//regional video enhancement
typedef struct tgIspRVE
{
	IR_U32 u32Enable;
	IRAY_RTD_RECT_S sRegion;
}IRAY_RTD_ISP_RVE_S;

// Brightness mutation inhibition
typedef struct tgIspBMI
{
	IR_U32 u32Enable;
}IRAY_RTD_ISP_BMI_S;

typedef struct IrayBlackbodyParams
{
	IR_U32			u32Enable;		//使能标志 1：使能，0：禁用。
	IR_U32			u32Distance;	//黑体距离单位为m(数值扩大100倍)
	IR_U32			u32Temp;		//黑体温度(单位为开尔文,数值扩大10倍)
	IRAY_RTD_RECT_S sRegion;
}IRAY_RTD_BLACKBODY_S;

typedef struct IrayTempCompensation
{
	IR_U32		u32Enable;		//使能标志 1：使能，0：禁用。
	IR_U32		u32Temp;		//温度(单位为开尔文,数值扩大10倍)
}IRAY_RTD_TEMP_COMPENSATION_S;


//#ifdef NUC_T  //nuc_t  进行单点温度数据校正时使用，目前该功能关闭
//涉及温度的变量都是开尔文单位
typedef struct tgNuc_tParams
{   
    unsigned int    nEnable;//是否使能环境变量修正模块，不使能，直接温度映射
    int             nReflect;//反射率，例如温度为25度，25*10000+2732000;
    int             nAirTemp;//环境温度，例如温度为25度，25*10000+2732000;
    unsigned int    nHumidity;//0x11 环境湿度,在新的环境补偿中，用作大气透过率（0-1）1*10000;//用作大气透过率
    unsigned int    nEmiss; //发射率0.95*10000; //发射率
    unsigned int    nDistance;//距离//1*10000;//距离,距离的单位是米，1米*10000
}NUC_t_PARAMS_S;
//#endif 

/*
* @method IrayRtdSetIp
* @function 设置IP地址
* @param 
*       pHandle             -初始化返回的句柄
*       pServerIp       	-IP地址格式“10.10.20.13”
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIp)(IR_HANDLE pHandle,IR_CHAR pServerIp[16]);

/*
* @method IrayRtdSetTempFrameRate
* @function 设置温度帧率
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       s32FrameRate       	-温度帧率[0,视频帧率]
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetTempFrameRate)(IR_HANDLE pHandle,IR_U32 u32DevID,IR_S32 s32FrameRate);


/*
* @method IrayRtdSetVideoType
* @function 设置输出视频格式
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       enVideoType       	-输出视频格式,目前支持IRAY_FRAME_TYPE_AGC,IRAY_FRAME_TYPE_NUC和IRAY_FRAME_TYPE_YVU422SP,默认为IRAY_FRAME_TYPE_AGC
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetVideoType)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_FRAME_TYPE_E enVideoType);

/*
* @method IrayRtdGetAlarmState
* @function 获取强光保护状态
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
* @return 0 强光保护未开启,1  强光保护开启
*/

typedef IR_S32 (*IrayRtdGetAlarmState)(IR_HANDLE pHandle,IR_U32 u32DevID);

/*
* @method IrayRtdSetAlarmParams
* @function 设置强光保护参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       alarmParams         -报警参数
* @return 0 设置成功  -1设置失败
*/

typedef IR_S32 (*IrayRtdSetAlarmParams)(IR_HANDLE pHandle,IR_U32 u32DevID,ALARM_PARAMS_S alarmParams);



/*
* @method IrayRtdSetPseudoColorParams
* @function 设置伪彩参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       nEnable       		-使能伪彩 1:使能,0:禁用
*		index				-色板索引
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetPseudoColorParams)(IR_HANDLE pHandle,IR_U32 u32DevID, int nEnable, int nIndex);

/*
* @method IrayRtdPutCommand
* @function 机芯指令透传
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pData       		-指令
*		s32Len				-长度
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdPutCommand)(IR_HANDLE pHandle, IR_U32 u32DevID,IR_UCHAR *pData, IR_S32 s32Len);

/*
* @method IrayRtdPutOneFrame
* @function 红外RAW数据(从探测器读到的数据：一个像素点14位)
* @param 
*       pHandle             -初始化返回的句柄
*		pFrameInfo			-帧头指针
*       pFrameData       	-帧内容指针
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdPutOneFrame)(IR_HANDLE pHandle, IRAY_FRAME_INFO_S *pFrameInfo, IR_UCHAR* pFrameData);

/*
* @method IrayRtdReiginCalc
* @function 针对某个点进行区域分析获取温度数据
* @param 
*       pHandle             -初始化返回的句柄
		u32DevID            -探测器ID默认为0
		data                -从nuc_t面阵里取到的某个点的nuc——t数据
		pTemp               -nuc参数

* @return 返回值：返回该像素点温度值.
	//注意得到的温度值需要除以10，得到开尔文温度。如果需要真实温度还需要开尔文温度到摄氏度的换算
*/
//#ifdef NUC_T
typedef IR_S16 (*IrayRtdReiginCalc)(IR_HANDLE pHandle,IR_U32 u32DevID,unsigned int data,NUC_t_PARAMS_S * pTemp);
//#endif

/*
* @method IrayRtdAdjustAdcGain
* @function 校正Adc gain, 提高测温精度
*			在各个测温范围下校正一次即可(模块内部会判断当前的测温范围)
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdAdjustAdcGain)(IR_HANDLE pHandle,IR_U32 u32DevID);

/*
* @method IrayRtdGetSysId
* @function 获取探测器sys id
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pData				-sys id
*		*s32Len				-pData 长度	
*		s32MaxLen			-调用者为pData分配的空间大小
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetSysId)(IR_HANDLE pHandle,IR_U32 u32DevID,IR_CHAR *pData, IR_S32 *s32Len,IR_S32 s32MaxLen);

/*
* @method IrayRtdSetForeheadTempSwitch
* @function 设置额温映射开关
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		u32Switch			-开关操作[1:开;0:关]
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetForeheadTempSwitch)(IR_HANDLE pHandle,IR_U32 u32DevID,IR_U32 u32Switch);


/*
* @method IrayRtdGetForeheadTempSwitch
* @function 获取额温映射开关
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		u32Switch			-开关操作[1:开;0:关]
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetForeheadTempSwitch)(IR_HANDLE pHandle,IR_U32 u32DevID,IR_U32 *u32Switch);

/*
* @method IrayRtdSetForeheadTempParams
* @function 设置额温映射参数
* @function 设置人体测温参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pstParams			-参数
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetForeheadTempParams)(IR_HANDLE pHandle,IR_U32 u32DevID,FOREHEAD_PARAMS_S *pstParams);

/*
* @method IrayRtdSetForeheadTempParams
* @function 获取额温映射参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pstParams			-参数
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetForeheadTempParams)(IR_HANDLE pHandle,IR_U32 u32DevID,FOREHEAD_PARAMS_S *pstParams);

/*
* @method IrayRtdCalibForeheadTemp
* @function 标定人体测温,在25摄氏度温度调用此接口
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdCalibForeheadTemp)(IR_HANDLE pHandle,IR_U32 u32DevID);

/*
* @method IrayRtdSetBlackbodyParams
* @function 设置黑体校正参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pstParams			-黑体参数
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetBlackbodyParams)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_BLACKBODY_S *pstParams);


/*
* @method IrayRtdSetBlackbodyParams
* @function 获取黑体校正参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pstParams			-黑体参数
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetBlackbodyParams)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_BLACKBODY_S *pstParams);


/*
* @method IrayRtdSetBlackbodyParams
* @function 设置温度补偿参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pstParams			-黑体参数
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetTempCompensationParams)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_TEMP_COMPENSATION_S *pstParams);


/*
* @method IrayRtdSetBlackbodyParams
* @function 获取温度补偿参数
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*		pstParams			-黑体参数
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetTempCompensationParams)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_TEMP_COMPENSATION_S *pstParams);

/*
* @method IrayRtdSetNucpFrameRate
* @function 设置NUC(用于高温检测)输出帖率
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       s32FrameRate       	-温度帧率[0,视频帧率]
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetNucpFrameRate)(IR_HANDLE pHandle,IR_U32 u32DevID,IR_S32 s32FrameRate);


/*
* @method IrayRtdGetIspPicColorAttr
* @function 获取视频色彩属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspPicColorAttr  -视频色彩属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetIspPicColorAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_PIC_COLOR_S *pstIspPicColorAttr);

/*
* @method IrayRtdSetIspPicColorAttr
* @function 获取视频色彩属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspPicColorAttr  -视频色彩属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIspPicColorAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_PIC_COLOR_S *pstIspPicColorAttr);


/*
* @method IrayRtdGetIsp2DDenoiseAttr
* @function 获取2D去噪属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIsp2DDenoiseAttr  -2D去噪属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetIsp2DDenoiseAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_DE_NOISE_S *pstIsp2DDenoiseAttr);

/*
* @method IrayRtdSetIsp2DDenoiseAttr
* @function 设置2D去噪属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIsp2DDenoiseAttr  -2D去噪属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIsp2DDenoiseAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_DE_NOISE_S *pstIsp2DDenoiseAttr);


/*
* @method IrayRtdGetIsp3DDenoiseAttr
* @function 获取3D去噪属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIsp2DDenoiseAttr  -3D去噪属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetIsp3DDenoiseAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_DE_NOISE_S *pstIsp2DDenoiseAttr);

/*
* @method IrayRtdSetIsp2DDenoiseAttr
* @function 设置3D去噪属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIsp2DDenoiseAttr  -3D去噪属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIsp3DDenoiseAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_DE_NOISE_S *pstIsp2DDenoiseAttr);

/*
* @method IrayRtdGetIspDDEAttr
* @function 获取细节增强DDE属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspDDEAttr  		-细节增强DDE属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetIspDDEAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_DDE_S *pstIspDDEAttr);

/*
* @method IrayRtdSetIspDDEAttr
* @function 设置细节增强DDE属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspDDEAttr  		-细节增强DDE属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIspDDEAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_DDE_S *pstIspDDEAttr);

/*
* @method IrayRtdGetIspRVEAttr
* @function 获取区域视频增强属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspRVEAttr  		-区域视频增强属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetIspRVEAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_RVE_S *pstIspRVEAttr);

/*
* @method IrayRtdSetIspRVEAttr
* @function 设置区域视频增强属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspRVEAttr  		-区域视频增强属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIspRVEAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_RVE_S *pstIspRVEAttr);

/*
* @method IrayRtdGetIspBMIAttr
* @function 获取亮度突变抑制属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspRVEAttr  		-亮度突变抑制属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdGetIspBMIAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_BMI_S *pstIspBMIAttr);

/*
* @method IrayRtdSetIspBMIAttr
* @function 设置亮度突变抑制属性
* @param 
*       pHandle             -初始化返回的句柄
*		u32DevID			-探测器ID默认为0
*       pstIspBMIAttr  		-亮度突变抑制属性
* @return 0 success,-1 error.
*/
typedef IR_S32 (*IrayRtdSetIspBMIAttr)(IR_HANDLE pHandle,IR_U32 u32DevID,IRAY_RTD_ISP_BMI_S *pstIspBMIAttr);


typedef struct irayRTD_API_INIT_ATTR
{
	IR_U8	u8HasIpServer;			//是否使用网络方式接收处理机芯指令1:是,0：否
	IR_CHAR	pServerIp[16];			//ip地址
	IR_S32	s32TempFrameRate;		//温度帧率
	IR_CHAR	pConfigFilePath[128];	//本模块所需配置文件存放路径如"/root/app_data"(注意不要"/"结尾)
	IR_U8 	u8RtdClock;				//探测器参考时钟单位为MHz
	IRAY_RTD_FRAME_RATE_E enRtdFrameRate;//配置探测器输出视频帧率
	IR_U8 	u8IsForeheadTemp;			//是否用于人体测温1:是,0：否
	/*
	* @method pGetAgcFrame
	* @function 返回视频AGC数据
	* @param 
	*		pFrameInfo			-帧头指针
	*       pAgcFrame       	-帧内容指针
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pGetAgcFrame)(IRAY_FRAME_INFO_S *pFrameInfo, IR_U8* pAgcFrame); //获取本模块处理后的视频数据

	/*
	* @method pGetYVUFrame
	* @function 返回视频YVU数据
	* @param 
	*		pFrameInfo			-帧头指针
	*       pYVUFrame       	-帧内容指针
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pGetYVUFrame)(IRAY_FRAME_INFO_S *pFrameInfo, IR_U8* pYVUFrame); //获取本模块处理后的视频数据

	/*
	* @method pGetNucFrame
	* @function 返回视频Nuc数据
	* @param 
	*		pFrameInfo			-帧头指针
	*       pNucFrame       	-帧内容指针
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pGetNucFrame)(IRAY_FRAME_INFO_S *pFrameInfo, IR_U8* pNucFrame); //获取本模块处理后的视频数据

	/*
	* @method pGetTempFrame
	* @function 返回温度数据
	* @param 
	*		pFrameInfo			-帧头指针
	*       pTempFrame       	-帧内容指针
	*		ptMaxTemp			-最大温度
	*		ptMinTemp			-最小温度
	*		u16AverageTemp		-平均温度
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pGetTempFrame)(IRAY_FRAME_INFO_S *pFrameInfo, IR_U16* pTempFrame,POINT_TEMP_INFO_S ptMaxTemp, POINT_TEMP_INFO_S ptMinTemp, IR_U16 u16AverageTemp); //获取本模块处理后的视频数据



	/*
	* @method pGetNUC_tFrame
	* @function 获取一帧nuc_t数据，需要注册该函数
	* @param 
	*		pFrameInfo			-帧头指针
	*       pTempFrame       	-帧内容指针
	*		pNUC_tFrame			-一帧的nuc_t的数据，每帧都会返回，实时的
	* @return 0 success,-1 error.
	*/
//#ifdef NUC_T
	IR_S32 (*pGetNUC_tFrame)(IRAY_FRAME_INFO_S *pFrameInfo, IR_U32* pNUC_tFrame); //获取本模块处理后的nuc_t 找机会放开20210531
//#endif
	/*
	* @method pGetCommand
	* @function 返回机芯指令
	* @param 
	*		u32DevID			-探测器ID(默认为0)
	*       pData       		-指令内容指针
	*		s32Len				-指令长度
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pGetCommand)(IR_U32 u32DevID,IR_UCHAR *pData, IR_S32 s32Len);

	/*
	* @method pCtlShutter
	* @function 控制快门
	* @param 
	*		u32DevID			-探测器ID(默认为0)
	*       ON_OFF       		-1:快门闭合,0：快门打开
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pCtlShutter)(IR_U32 u32DevID,IR_UCHAR ON_OFF);

	/*
	* @method pResetRtd
	* @function reset探测器(外部reset信号拉低20ms后再拉高)
	* @param 
	*		u32DevID			-探测器ID(默认为0)
	* @return 0 success,-1 error.
	*/
	IR_S32 (*pResetRtd)(IR_U32 u32DevID);

	IrayRtdSetIp						*pSetIp;			//初始化后如遇IP修改请调用本接口把Ip传给本模块
	IrayRtdSetTempFrameRate 			*pSetTempFrameRate; //设置温度帧率
	IrayRtdSetVideoType					*pSetVideoType;		//设置输出视频格式
	IrayRtdSetPseudoColorParams 		*pSetPseudoColorParams;//设置伪彩参数
	IrayRtdPutCommand 					*pPutCommand; 		//向本模块发送机芯透传指令
	IrayRtdPutOneFrame 					*pPutFrame; 		//向本模块发送灰度视频数据
	IrayRtdGetAlarmState                *pGetAlarmState;//获取本模块的强光保护状态
	IrayRtdSetAlarmParams               *pSetAlarmParams;//设置强光保护状态
	//#ifdef NUC_T
	IrayRtdReiginCalc                   *pReiginCalc;//单点的区域修正
	//#endif
	IrayRtdAdjustAdcGain				*pAdjustAdcGain;	//校正ADC Gain
	IrayRtdGetSysId						*pGetRtdSysId;		//获取探测器sysId
	IrayRtdGetForeheadTempSwitch		*pGetForeheadTempSwitch; //设置额温映射开关
	IrayRtdSetForeheadTempSwitch		*pSetForeheadTempSwitch; //获取额温映射开关
	IrayRtdCalibForeheadTemp			*pCalibForeheadTemp; //人体测温标定
	IrayRtdSetForeheadTempParams 		*pSetForeheadParams;//设置额温映射参数
	IrayRtdGetForeheadTempParams		*pGetForeheadParams;//获取额温映射参数
	IrayRtdSetBlackbodyParams			*pSetBlackbodyParams;//设置黑体校正参数
	IrayRtdGetBlackbodyParams			*pGetBlackbodyParams;//获取黑体校正参数
	IrayRtdSetTempCompensationParams 	*pSetTempCompensationParams;//设置温度补偿参数
	IrayRtdGetTempCompensationParams 	*pGetTempCompensationParams;//设置温度补偿参数
	IrayRtdSetNucpFrameRate				*pSetNucFrameRate;	//设置NUC输出帧率
	/* ISP API*/
	IrayRtdGetIspPicColorAttr		*pGetIspPicColorAttr; //获取Isp 图像色彩属性(亮度、对比度)
	IrayRtdGetIspPicColorAttr		*pSetIspPicColorAttr; //设置Isp 图像色彩属性(亮度、对比度)

	IrayRtdGetIsp2DDenoiseAttr		*pGetIsp2DDenoiseAttr; //获取2D降噪属性
	IrayRtdSetIsp2DDenoiseAttr		*pSetIsp2DDenoiseAttr; //设置2D降噪属性

	IrayRtdGetIsp3DDenoiseAttr		*pGetIsp3DDenoiseAttr; //获取3D降噪属性
	IrayRtdSetIsp3DDenoiseAttr		*pSetIsp3DDenoiseAttr; //设置3D降噪属性

	IrayRtdGetIspDDEAttr			*pGetIspDDEAttr;		//获取图像细节增强属性
	IrayRtdSetIspDDEAttr			*pSetIspDDEAttr;		//设置图像细节增强属性

	IrayRtdGetIspRVEAttr			*pGetIspRVEAttr;		//获取区域视频增强属性
	IrayRtdSetIspRVEAttr			*pSetIspRVEAttr;		//设置区域视频增强属性

	IrayRtdGetIspBMIAttr			*pGetIspBMIAttr;		//获取亮度突变抑制属性
	IrayRtdSetIspBMIAttr			*pSetIspBMIAttr;		//设置亮度突变抑制属性
}IRAY_RTD_API_INIT_ATTR_S;

/*
* @method IrayRtdGetVersion
* @function 获取版本信息
* @param 
* @return IR_CHAR*.
*/
extern IR_CHAR* IrayRtdGetVersion();

/*
* @method IrayRtdInit
* @function 初始化
* @param 
*		pInitAttr		-初始化结构体指令
* @return IR_HANDLE,-error时该值为NULL.
*/
extern IR_HANDLE IrayRtdInit(IRAY_RTD_API_INIT_ATTR_S *pInitAttr);

/*
* @method IrayRtdUnit
* @function 返初始化
* @param 
*		pHandle			-初始化返回的句柄
* @return 无.
*/
extern IR_VOID IrayRtdUnit(IR_HANDLE pHandle);


/*
*	备注信息
*	1. 探测器I2C地址默认是"/dev/i2c-0"，设备地址为0x48,
*	2. 快门GPIO，默认快门+使用GPIO4-4，快门-使用GPIO4-5
*	以上硬件确认后请及时告知IRAY重新修改编译该库
*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
