#ifndef _AR_IVE_H_
#define _AR_IVE_H_

#include <stdio.h>
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
#include "ar_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OD_CHN_MAX  80

#define RESIZED_WIDTH    		320
#define RESIZED_HEIGHT   		240
#define GRID_WIDTH       		40
#define GRID_HEIGHT		 		40
#define GRID_NUM         		48

#define CANNY_LOW_THRED  		5
#define CANNY_HIGH_THRED 		20


#define GAUSSIAN_KSIZE_WIDTH  	5
#define GAUSSIAN_KSIZE_HEIGHT 	5
#define GAUSSIAN_SIGMA_X	  	1.5f
#define GAUSSIAN_SIGMA_Y      	0

/* canny thred */
#define HIST_GRAY_THRED        		150
#define HIST_PIXEL_PERCENT_THRED	80
#define CANNY_EDGE_POINT_THRED 		50
#define ALARM_BLOCK_NUM_THRED  		30

#define CANNY_CHECK_PIXELPERCENYT_IS_VALID(num) (0 <= (num) && (num) <= 100)
#define CANNY_CHECK_EDGEPOINT_IS_VALID(num)  	(0 <= (num) && (num) <= 100)
#define CANNY_CHECK_GRAY_IS_VALID(num) 			(0 <= (num) && (num) <= 255)
#define CANNY_CHECK_ALARMNUM_IS_VALID(num) 		(1 <= (num) && (num) <= 48)
#define CHECK_ODCHN_IS_VALID(num)  			    (0 <= (num) && (num) <= 79)


typedef struct ArOD_ATTR_S
{
	AR_U32 u32AlarmBlockNumThred; //GRID检测数目门限
	AR_U32 u32Width;   /*Image width*/
	AR_U32 u32Height;  /*Image height*/
    AR_U64 u64StreamPa;/*Src Stream Phy Addr*/
    void *pStreamVa;   /*Src Stream Virt Addr*/
}OD_ATTR_S;


typedef struct ArOD_ATTR_PRIV_S
{
	AR_U32 u32HistGrayThred;         //直方图亮度门限 
	AR_U32 u32HistPixelPercentThred; //直方图占比门限
	AR_U32 u32EdgePointThred;        //边缘数目门限
}OD_ATTR_PRIV_S;

/**
* @brief  IVE系统初始化.
* @param  none
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 AR_IVE_OD_Init();

/**
* @brief  IVE系统去初始化.
* @param  none
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 AR_IVE_OD_Exit();

/**
* @brief  创建IVE通道.
* @param  u32OdChn IVE通道号
* @param  pstOdAttr IVE通道属性
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 AR_IVE_OD_CreateChn(AR_U32 u32OdChn, OD_ATTR_S *pstOdAttr);

/**
* @brief  消除IVE通道.
* @param  u32OdChn IVE通道号
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVE_OD_DestroyChn(AR_U32 u32OdChn);

/**
* @brief  IVE通道附加属性设置.
* @param  u32OdChn IVE通道号
* @param  pstOdPrivAttr IVE通道附加属性
* @retval 0 成功 , 其它 失败.
* @note
*/
AR_S32 AR_IVE_OD_SetChnPrivAttr(AR_U32 u32OdChn, OD_ATTR_PRIV_S *pstOdPrivAttr);

/**
* @brief  IVE通道附加属性获取.
* @param  u32OdChn IVE通道号
* @param  pstOdPrivAttr IVE通道附加属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVE_OD_GetChnPrivAttr(AR_U32 u32OdChn, OD_ATTR_PRIV_S *pstOdPrivAttr);

/**
* @brief  IVE处理.
* @param  u32OdChn IVE通道号
* @param  pstOdAttr IVE通道属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVE_OD_Process(AR_U32 u32OdChn, OD_ATTR_S *pstOdAttr);


#ifdef __cplusplus
}
#endif

#endif



