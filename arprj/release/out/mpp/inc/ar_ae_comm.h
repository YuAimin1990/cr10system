#ifndef __AR_AE_COMM_H__
#define __AR_AE_COMM_H__

#include "hal_type.h"
#include "ar_comm_isp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AR_AE_LIB_NAME "ar_ae_lib"


typedef struct arAE_SENSOR_DEFAULT_S
{

    AR_U32  u32LinesPer500ms;
    AR_U32  u32FlickerFreq;
    AR_FLOAT f32Fps;
    AR_FLOAT f32MaxFps;
	AR_U32  u32Hmax; /*unit is pix clk*/
    AR_U32  u32FullLinesStd; // vts
    AR_U32  u32FullLinesMid; //mid vts
    AR_U32  u32FullLinesShort; //short vts
    AR_U32  u32FullLinesMax;
    AR_U32  u32FullLines;
    AR_U32  u32MaxIntTime;     /* RW;unit is line */
    AR_U32  u32MinIntTime;
    AR_U32  u32MaxAgain;
    AR_U32  u32MinAgain;
    AR_U32  u32MaxDgain;
    AR_U32  u32MinDgain;
    AR_U32  u32MaxIntTimeStep;
    AR_U32  u32LFMaxShortTime;
    AR_U32  u32LFMinExposure;
    AR_BOOL bAERouteExValid;

} AE_SENSOR_DEFAULT_S;

typedef struct arAE_FSWDR_ATTR_S
{
    ISP_FSWDR_MODE_E enFSWDRMode;
} AE_FSWDR_ATTR_S;

typedef struct arAE_SENSOR_EXP_FUNC_S
{
    AR_S32(*pfn_cmos_get_ae_default)(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    /* the function of sensor set fps */
    AR_VOID(*pfn_cmos_fps_set)(VI_PIPE ViPipe, AR_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft);
    AR_VOID(*pfn_cmos_slow_framerate_set)(VI_PIPE ViPipe, AR_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    /* while isp notify ae to update sensor regs, ae call these funcs. */
    AR_VOID(*pfn_cmos_inttime_update)(VI_PIPE ViPipe, AR_U32 u32IntTime);
    AR_VOID(*pfn_cmos_gains_update)(VI_PIPE ViPipe, AR_U32 u32Again, AR_U32 u32Dgain);

    AR_VOID (*pfn_cmos_again_calc_table)(VI_PIPE ViPipe, AR_U32 *pu32AgainLin, AR_U32 *pu32AgainDb);
    AR_VOID (*pfn_cmos_dgain_calc_table)(VI_PIPE ViPipe, AR_U32 *pu32DgainLin, AR_U32 *pu32DgainDb);

    AR_VOID (*pfn_cmos_get_inttime_max)(VI_PIPE ViPipe, AR_U16 u16ManRatioEnable, AR_U32 *au32Ratio, AR_U32 *au32IntTimeMax, AR_U32 *au32IntTimeMin, AR_U32 *pu32LFMaxIntTime);

    /* long frame mode set */
    AR_VOID(*pfn_cmos_ae_fswdr_attr_set)(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr);

} AE_SENSOR_EXP_FUNC_S;

typedef struct arAE_SENSOR_REGISTER_S
{
    AE_SENSOR_EXP_FUNC_S stSnsExp;
} AE_SENSOR_REGISTER_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
