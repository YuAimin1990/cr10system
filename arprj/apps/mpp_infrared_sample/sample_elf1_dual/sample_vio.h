#ifndef __SAMPLE_VIO_H__
#define __SAMPLE_VIO_H__

#include "ar_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef SAMPLE_PRT
#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)
#endif

#ifndef PAUSE
#define PAUSE()  do {\
        printf("---------------press Enter key to exit!---------------\n");\
        getchar();\
    } while (0)
#endif

AR_VOID SAMPLE_VIO_MsgInit(AR_VOID);
AR_VOID SAMPLE_VIO_MsgExit(AR_VOID);
AR_VOID SAMPLE_AR_MPI_VIN_CloseDev();

void SAMPLE_VIO_HandleSig(AR_S32 signo);

AR_S32 SAMPLE_elf1_dual_test(AR_S32 outMode);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_H__*/
