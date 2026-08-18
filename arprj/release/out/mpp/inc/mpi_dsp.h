#ifndef _AR_MPI_DSP_H_
#define _AR_MPI_DSP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "ar_dsp.h"

/*****************************************************************************
*   Prototype    : AR_MPI_SVP_DSP_LoadBin
*   Description  : Load DSP Bin
*
*   Parameters   : const AR_CHAR       *pszBinFileName       Bin file.
*                  SVP_DSP_MEM_TYPE_E  enMemType             DSP memory type.
*
*
*
*   Return Value : AR_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_LoadBin(const AR_CHAR *pszBinFileName,SVP_DSP_MEM_TYPE_E enMemType);

/*****************************************************************************
*   Prototype    : AR_MPI_SVP_DSP_EnableCore
*   Description  : Enable DSP Core
*
*   Parameters   : SVP_DSP_ID_E        enDspId               DSP Core ID.
*
*
*
*
*   Return Value : AR_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_EnableCore(SVP_DSP_ID_E enDspId);

/*****************************************************************************
*   Prototype    : AR_MPI_SVP_DSP_DisableCore
*   Description  : Disable DSP Core
*
*   Parameters   : SVP_DSP_ID_E        enDspId               DSP Core ID.
*
*
*
*
*   Return Value : AR_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_DisableCore(SVP_DSP_ID_E enDspId);

/*****************************************************************************
*   Prototype    : AR_MPI_SVP_DSP_RPC
*   Description  : Remote process control
*
*   Parameters   : SVP_DSP_HANDLE               *phHandle       Handle
*					 const SVP_DSP_MESSAGE_S      *pstMsg         Message
*                  SVP_DSP_ID_E                 enDspId         DSP Core ID.
*                  SVP_DSP_PRI_E                enPri           Priority
*
*
*
*   Return Value : AR_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_RPC(SVP_DSP_HANDLE *phHandle,const SVP_DSP_MESSAGE_S *pstMsg,
                                SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri);

/*****************************************************************************
*   Prototype    : AR_MPI_SVP_DSP_Query
*   Description  : This API is used to query the status of a called function by using the returned hHandle of the function.
                   In block mode, the system waits until the task is finish or query timeout.
                   In non-block mode, the current status is queried and no action is taken.
*   Parameters   : SVP_DSP_ID_E          enDspId          DSP Core ID.
*                  SVP_DSP_HANDLE        hHandle          Handle.
*					 AR_BOOL			    *pbFinish         Returned status
*                  AR_BOOL			    bBlock            Flag indicating the block mode or non-block mode
*   Return Value : AR_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_Query(SVP_DSP_ID_E enDspId,SVP_DSP_HANDLE hHandle,AR_BOOL *pbFinish,AR_BOOL bBlock);

/*****************************************************************************
*	Prototype	 : AR_MPI_SVP_DSP_PowerOn
*	Description  : Enable DSP Core
*
*	Parameters	 : SVP_DSP_ID_E 	   enDspId				 DSP Core ID.
*
*
*
*
*	Return Value : AR_SUCCESS: Success;Error codes: Failure.
*	Spec		 :
*
*	History:
*
*		1.	Date		 : 2017-10-17
*			Author		 :
*			Modification : Created function
*
****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_PowerOn(SVP_DSP_ID_E enDspId);

/*****************************************************************************
*	Prototype	 : AR_MPI_SVP_DSP_PowerOff
*	Description  : Disable DSP Core
*
*	Parameters	 : SVP_DSP_ID_E 	   enDspId				 DSP Core ID.
*
*
*
*
*	Return Value : AR_SUCCESS: Success;Error codes: Failure.
*	Spec		 :
*
*	History:
*
*		1.	Date		 : 2017-10-17
*			Author		 :
*			Modification : Created function
*
****************************************************************************/
AR_S32 AR_MPI_SVP_DSP_PowerOff(SVP_DSP_ID_E enDspId);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_AR_MPI_DSP_H_*/
