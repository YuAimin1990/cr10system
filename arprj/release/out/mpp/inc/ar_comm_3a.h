/**
 * \file
 * \brief 描述3a相关的通用数据结构
 */

#ifndef __AR_COMM_3A_H__
#define __AR_COMM_3A_H__

#include "ar_common.h"
#include "ar_comm_isp.h"
#include "ar_comm_sns.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define IS_LINEAR_MODE(mode)      (WDR_MODE_NONE == (mode))
#define IS_WDR_MODE(mode)         ((!IS_LINEAR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_HDR_MODE(mode)         (((mode) > DYNAMIC_RANGE_SDR10) && ((mode) < DYNAMIC_RANGE_BUTT))
#define IS_BUILT_IN_WDR_MODE(mode) (WDR_MODE_BUILT_IN == (mode))
#define IS_FS_WDR_MODE(mode)      ((!IS_LINEAR_MODE(mode))&& (!IS_BUILT_IN_WDR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_2to1_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_2To1_LINE == (mode))  || (WDR_MODE_QUDRA == (mode)))
#define IS_3to1_WDR_MODE(mode)    ((WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_LINE == (mode)))
#define IS_4to1_WDR_MODE(mode)    ((WDR_MODE_4To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_LINE == (mode)))
#define IS_FULL_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)))
#define IS_HALF_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME == (mode)))
#define IS_LINE_WDR_MODE(mode)    ((WDR_MODE_2To1_LINE == (mode)) || (WDR_MODE_3To1_LINE == (mode)) || (WDR_MODE_4To1_LINE == (mode)) || (WDR_MODE_QUDRA == (mode)))

#define IS_STITCH_MAIN_PIPE(ViPipe,MainPipe) ((ViPipe) == (MainPipe))
#define IS_OFFLINE_MODE(mode)    (ISP_MODE_RUNNING_OFFLINE    == (mode))
#define IS_ONLINE_MODE(mode)     (ISP_MODE_RUNNING_ONLINE     == (mode))
#define IS_SIDEBYSIDE_MODE(mode) (ISP_MODE_RUNNING_SIDEBYSIDE == (mode))
#define IS_STRIPING_MODE(mode)   (ISP_MODE_RUNNING_STRIPING   == (mode))

/******************* algo **********************************/
#define ALG_LIB_NAME_SIZE_MAX   (20)
typedef struct arALG_LIB_S
{
	AR_S32	s32Id;
	AR_CHAR acLibName[ALG_LIB_NAME_SIZE_MAX];
} ALG_LIB_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__AR_COMM_SNS_H__ */
