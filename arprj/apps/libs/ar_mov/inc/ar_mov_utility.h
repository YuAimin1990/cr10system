#ifndef __AR_MOV_UTILITY_H__
#define __AR_MOV_UTILITY_H__

#include "ar_mov_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void print_buf(AR_U8 *buf, AR_U32 len);
void hton_set_u32(void *pp, AR_U32 w);
void hton_set_u16(void *pp, AR_U16 w);
void hton_set_u8(void *pp, AR_U8 w);
void hton_set_u24(void *dst, void *src);
AR_S32 iframe_write_hvcc(MovWriteHandle *handle);
AR_S32 iframe_get_pps_sps(AR_U8 *buf, int len, AR_U8 *sps, AR_U32 *sps_len, AR_U8 *pps, AR_U32* pps_len);
AR_S32 iframe_get_vps_sps_pps(AR_U8 *buf, int len, AR_U8 *vps, AR_U32 *vps_len, AR_U8 *sps, AR_U32 *sps_len, AR_U8 *pps, AR_U32* pps_len);
#ifdef __cplusplus
}
#endif

#endif /* __Z_MOV_UTILITY_H__ */

