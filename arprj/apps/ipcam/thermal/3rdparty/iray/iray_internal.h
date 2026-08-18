#ifndef __IRAY_INTERNAL__
#define __IRAY_INTERNAL__

#include "IRAY_RTD_API.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    IR_HANDLE                           pRtdHandle;
    IrayRtdSetIp                        RtdSetIp;
    IrayRtdSetTempFrameRate             RtdSetTempFrameRate;
    IrayRtdPutCommand                   RtdPutCmd;
    IrayRtdPutOneFrame                  RtdPutFrame;
    IrayRtdSetVideoType                 RtdSetVideoType;
    IrayRtdSetPseudoColorParams         RtdSetPseudoColorParams;

    IrayRtdGetAlarmState				RtdGetAlarmState;
    IrayRtdSetAlarmParams				RtdSetAlarmParams;	
    IrayRtdReiginCalc					RtdReiginCalc;

    IrayRtdAdjustAdcGain	        	RtdAdjustAdcGain;
    IrayRtdGetSysId	                	RtdGetSysId;
    IrayRtdCalibForeheadTemp	        RtdCalibForeheadTemp;

    IrayRtdSetForeheadTempSwitch		RtdSetForeheadTempSwitch;
    IrayRtdGetForeheadTempSwitch		RtdGetForeheadTempSwitch;
    IrayRtdSetForeheadTempParams        RtdSetForeheadTempParams;
    IrayRtdGetForeheadTempParams		RtdGetForeheadTempParams;

    IrayRtdSetBlackbodyParams			RtdSetBlackbodyParams;
    IrayRtdGetBlackbodyParams			RtdGetBlackbodyParams;

    IrayRtdSetTempCompensationParams	RtdSetTempCompensationParams;
    IrayRtdGetTempCompensationParams	RtdGetTempCompensationParams;

    IrayRtdSetNucpFrameRate             RtdSetNucpFrameRate;
    IrayRtdGetIspPicColorAttr           RtdGetIspPicColorAttr;
    IrayRtdSetIspPicColorAttr           RtdSetIspPicColorAttr;
    IrayRtdGetIsp2DDenoiseAttr          RtdGetIsp2DDenoiseAttr;
    IrayRtdSetIsp2DDenoiseAttr          RtdSetIsp2DDenoiseAttr;
    IrayRtdGetIsp3DDenoiseAttr          RtdGetIsp3DDenoiseAttr;
    IrayRtdSetIsp3DDenoiseAttr          RtdSetIsp3DDenoiseAttr;
    IrayRtdGetIspDDEAttr                RtdGetIspDDEAttr;
    IrayRtdSetIspDDEAttr                RtdSetIspDDEAttr;
    IrayRtdGetIspRVEAttr                RtdGetIspRVEAttr;
    IrayRtdSetIspRVEAttr                RtdSetIspRVEAttr;
    IrayRtdGetIspBMIAttr                RtdGetIspBMIAttr;
    IrayRtdSetIspBMIAttr                RtdSetIspBMIAttr;

    //privite
    unsigned char *swap_buffer;
}SHisiLibMgr;

#ifdef __cplusplus
};
#endif

#endif //__IRAY_INTERNAL__
