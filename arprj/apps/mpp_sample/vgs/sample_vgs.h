#ifndef __SAMPLE_VIO_H__
#define __SAMPLE_VIO_H__

#include "ar_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef SAMPLE_PRT
#define SAMPLE_PRT(fmt, ...)                     \
    do {\
        ar_always(fmt, ##__VA_ARGS__);  \
    }while(0)
#endif

#ifndef SAMPLE_ERR
#define SAMPLE_ERR(fmt, ...)                     \
            do {\
                ar_err(fmt, ##__VA_ARGS__);  \
            }while(0)
#endif

#ifndef PAUSE
#define PAUSE()  do {\
        printf("---------------press Enter key to exit!---------------\n");\
        getchar();\
    } while (0)
#endif


void SAMPLE_VGS_HandleSig(AR_S32 signo);
AR_S32 SAMPLE_VGS_Start(AR_CHAR *spFilename);
AR_S32 SAMPLE_VGS_Start_With_VI(AR_CHAR *spFilename);
AR_S32 SAMPLE_VGS_Start_Draw_Test(int test_case, AR_CHAR *spFilename);
void SAMPLE_VGS_Suspend(void);
void SAMPLE_VGS_Rusume(void);
AR_S32 SAMPLE_VGS_Vo_Init(void);
AR_S32 SAMPLE_VGS_Vo_DeInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_H__*/
