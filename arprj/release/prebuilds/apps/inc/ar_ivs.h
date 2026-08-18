#ifndef _AR_IVS_H_
#define _AR_IVS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "ar_common.h"
#include "hal_npu_types.h"
#include "vibe-background-sequential.h"


#define MD_CHN_MAX  80
#define CHECK_SAMPLENUM_IS_VALID(num)  			(0 <= (num) && (num) <= 100)
#define CHECK_MATCHNUM_IS_VALID(num)  			(0 <= (num) && (num) <= 100)
#define CHECK_MATCHTHR_IS_VALID(num)  			(0 <= (num) && (num) <= 255)
#define CHECK_UPDATEFACTOR_IS_VALID(num)	    (2 <= (num) && (num) <= 64)
#define CHECK_WIDTH_IS_VALID(num)  				(0 <= (num) && (num) <= 3840)
#define CHECK_HEIGHT_IS_VALID(num)  			(0 <= (num) && (num) <= 2160)
#define CHECK_MDCHN_IS_VALID(num)  			    (0 <= (num) && (num) <= 79)
#define CHECK_CCLMODE_IS_VALID(num)  			(0 <= (num) && (num) <= 1)

#define MD_MAX_REGION_NUM 254

typedef enum ArMD_ALG_MODE_E
{
	 MD_ALG_MODE_BG = 0x0,/*Base on background image*/
	 MD_ALG_MODE_REF = 0x1,/*Base on reference image*/
	 MD_ALG_MODE_BUTT
}MD_ALG_MODE_E;

typedef enum ArIVE_SAD_MODE_E
{
	IVE_SAD_MODE_MB_4X4 = 0x0, /*4x4*/
	IVE_SAD_MODE_MB_8X8 = 0x1, /*8x8*/
	IVE_SAD_MODE_MB_16X16 = 0x2, /*16x16*/
	IVE_SAD_MODE_BUTT
}IVE_SAD_MODE_E;

typedef enum ArIVE_SAD_OUT_CTRL_E
{
	IVE_SAD_OUT_CTRL_16BIT_BOTH= 0x0, /*Output 16 bit sad and thresh*/
	IVE_SAD_OUT_CTRL_8BIT_BOTH = 0x1, /*Output 8 bit sad and thresh*/
	IVE_SAD_OUT_CTRL_16BIT_SAD = 0x2, /*Output 16 bit sad*/
	IVE_SAD_OUT_CTRL_8BIT_SAD = 0x3, /*Output 8 bit sad*/
	IVE_SAD_OUT_CTRL_THRESH = 0x4, /*Output thresh,16 bits sad */
	IVE_SAD_OUT_CTRL_BUTT
}IVE_SAD_OUT_CTRL_E;


typedef enum ArIVE_CCL_MODE_E
{
	IVE_CCL_MODE_4C = 0x0,/*4-connectivity*/
	IVE_CCL_MODE_8C = 0x1,/*8-connectivity*/
	IVE_CCL_MODE_BUTT
}IVE_CCL_MODE_E;

typedef struct ArIVE_CCL_CTRL_S
{
	 IVE_CCL_MODE_E enMode; /*Mode*/
	 AR_U16 u16InitAreaThr; /*Init threshold of region area*/
	 AR_U16 u16Step; /*Increase area step for once*/
}IVE_CCL_CTRL_S;


typedef struct ArIVE_ADD_CTRL_S
{
	AR_U16 u0q16X; /*x of "xA+yB"*/
	AR_U16 u0q16Y; /*y of "xA+yB"*/
}IVE_ADD_CTRL_S;

typedef struct ArMD_ATTR_S
{
	 MD_ALG_MODE_E enAlgMode; /*Md algorithm mode*/
	 IVE_SAD_MODE_E enSadMode; /*Sad mode*/
	 IVE_SAD_OUT_CTRL_E enSadOutCtrl; /*Sad output ctrl*/
	 AR_U16 u16Width; /*Image width*/
	 AR_U16 u16Height; /*Image height*/
	 AR_U16 u16MatchingThr; /*Matching thresh*/
	 IVE_CCL_CTRL_S stCclCtrl; /*Ccl ctrl*/
	 IVE_ADD_CTRL_S stAddCtrl; /*Add ctrl*/
}MD_ATTR_S;

typedef struct ArMD_ATTR_EX_S
{
	AR_U32 u32NumberOfSamples;		 // 背景模型样本集像素个数,ref 20
	AR_U32 u32MatchingNumber;		 // 最小匹配像素个数阈值, ref 2  should <= u32NumberOfSamples
	AR_U32 u32UpdateFactor; 		 // 子采样概率，决定背景更新的概率 ref 16

}MD_ATTR_EX_S;

typedef struct ArIVE_REGION_S
{
	 AR_U32 u32Area; /*Represented by the pixel number*/
	 AR_U16 u16Left; /*Circumscribed rectangle left border*/
	 AR_U16 u16Right; /*Circumscribed rectangle right border*/
	 AR_U16 u16Top; /*Circumscribed rectangle top border*/
	 AR_U16 u16Bottom; /*Circumscribed rectangle bottom border*/
}IVE_REGION_S;


typedef struct ArMD_BLOB_S
{
	 AR_U16 u16CurAreaThr; /*Threshold of the result regions' area*/
	 AR_S8 s8LabelStatus; /*-1: Labeled failed ; 0: Labeled successfully*/
	 AR_U8 u8RegionNum; /*Number of valid region, non-continuous stored*/
	 IVE_REGION_S astRegion[MD_MAX_REGION_NUM]; /*Valid regions with
	'u32Area>0' and 'label = ArrayIndex+1'*/
}MD_BLOB_S;


/**
* @brief  IVS系统初始化.
* @param  none
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_Init();


/**
* @brief  IVS系统去初始化.
* @param  none
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_Exit();
/**
* @brief  创建IVS通道.
* @param  u32MdChn IVS通道号
* @param  pstMdAttr IVS通道属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_CreateChn(AR_U32 u32MdChn, MD_ATTR_S *pstMdAttr);
/**
* @brief  消除IVS通道.
* @param  u32MdChn IVS通道号
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_DestroyChn(AR_U32 u32MdChn);
/**
* @brief  IVS通道属性设置.
* @param  u32MdChn IVS通道号
* @param  pstMdAttr IVS通道属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_SetChnAttr(AR_U32 u32MdChn, MD_ATTR_S *pstMdAttr);
/**
* @brief  IVS通道属性获取.
* @param  u32MdChn IVS通道号
* @param  pstMdAttr IVS通道属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_GetChnAttr(AR_U32 u32MdChn, MD_ATTR_S *pstMdAttr);
/**
* @brief  IVS通道附加属性设置.
* @param  u32MdChn IVS通道号
* @param  pstMdAttr IVS通道附加属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_SetChnAttrEx(AR_U32 u32MdChn, MD_ATTR_EX_S *pstMdAttrEx);
/**
* @brief  IVS附加属性获取.
* @param  u32MdChn IVS通道号
* @param  pstMdAttr IVS通道附加属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_GetChnAttrEx(AR_U32 u32MdChn, MD_ATTR_EX_S *pstMdAttrEx);
/**
* @brief  IVS处理.
* @param  u32MdChn IVS通道号
* @param  pstSrc IVS源图
* @param  pstSegmapOut IVS二值图输出
* @param  pstSrc IVS Blob属性
* @retval 0 成功 , 其它 失败.
* @note
*/

AR_S32 AR_IVS_MD_Process(AR_U32 u32MdChn, AR_IMG_S *pSrc,AR_IMG_S *pstSegmapOut,MD_BLOB_S*pstBlob);


#ifdef __cplusplus
}
#endif


#endif
