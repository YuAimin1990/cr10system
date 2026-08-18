#ifndef __SAMPLE_VIO_GST417W_H__
#define __SAMPLE_VIO_GST417W_H__


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


typedef struct {
    AR_U32 u32Width;
    AR_U32 u32Height;
    AR_CHAR *input_fname;
    AR_CHAR *thermal_output_fname;
    AR_CHAR *nr3d_output_fname;
    AR_CHAR *nr2d_output_fname;
    AR_CHAR *gtm_output_fname;
    AR_CHAR *isp1_output_fname;
    AR_CHAR *isp2_output_fname;
    AR_S32 s32Circle;
    AR_BOOL bImgNoDummy;
} IR_OFFLINE_CTX_S;


AR_VOID SAMPLE_VIO_MsgInit(AR_VOID);
AR_VOID SAMPLE_VIO_MsgExit(AR_VOID);
AR_VOID SAMPLE_AR_MPI_VIN_CloseDev();
void SAMPLE_VIO_HandleSig(AR_S32 signo);
AR_S32 SAMPLE_IR(AR_CHAR *strSensor, AR_S32 s32Mode, AR_FLOAT fDispScalerRatio, AR_S32 s32Fusion,
    IR_OFFLINE_CTX_S *pstIrOfflineCtx);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_GST417W_H__*/
