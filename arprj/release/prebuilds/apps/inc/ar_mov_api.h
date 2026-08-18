#ifndef __AR_MOV_API_H__
#define __AR_MOV_API_H__

#include "ar_mov_def.h"

#ifdef __cplusplus
extern "C" {
#endif

AR_S32 AR_MOV_Init(MovWriteHandle *handle, MovWriteInitParam *param);
AR_S32 AR_MOV_WriteOpen(MovWriteHandle *handle);
AR_S32 AR_MOV_WriteClose(MovWriteHandle *handle);
AR_S32 AR_MOV_WriteFrame(AR_CHAR *frame, AR_S32 frame_len, AR_S32 is_key, MovWriteHandle *handle, AR_U32 time_ms);
AR_S32 AR_MOV_WriteHead(MovWriteHandle *w_handle);
AR_S32 AR_MOV_WriteTail(MovWriteHandle *handle);

void AR_MOV_ClearWriteBuff();
void AR_MOV_PrintHandle(MovWriteHandle *handle);

MovPBHandle *AR_MOV_ReadPbHandleCreate(char *filename);
AR_S32 AR_MOV_ReadPbHandleDestory(MovPBHandle *pPBHandle);
AR_S32 AR_MOV_ReadGetVideoFrame(MovPBHandle *pPBHandle);
AR_S32 AR_MOV_ReadGetAudioFrame(MovPBHandle *pPBHandle);


#ifdef __cplusplus
}
#endif

#endif /* __Z_MOV_API_H__ */

