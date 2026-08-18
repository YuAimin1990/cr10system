#ifndef __SAMPLE_VO_H__
#define __SAMPLE_VO_H__

#include "ar_common.h"
#include "ar_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef SAMPLE_PRT
#define SAMPLE_PRT(fmt, ...)                     \
    do {\
        printf(fmt, ##__VA_ARGS__);                  \
    }while(0)
#endif

#ifndef PAUSE
#define PAUSE()  do {\
        printf("---------------press Enter key to exit!---------------\n");\
        getchar();\
    } while (0)
#endif


void SAMPLE_VO_HandleSig(AR_S32 signo);
AR_VOID SAMPLE_VOU_SYS_Exit(void);

AR_S32 SAMPLE_VO_MIPILCD_1024_600(AR_BOOL          MutiDev);
AR_S32 SAMPLE_VO_HDMI_1080P60(RGN_TYPE_E enRgnType, AR_BOOL IsMono);
AR_S32 SAMPLE_VO_HDMI_1080I60(AR_VOID);
AR_S32 SAMPLE_VO_HDMI_1080P60_MUTI_CHAN(AR_VOID);
AR_S32 SAMPLE_VO_TEST(void);
AR_S32 SAMPLE_VO_MIPILCD_720_1440(AR_BOOL AR_FALSE);
AR_S32 SAMPLE_VO_MIPILCD_400x900(AR_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VO_H__*/
