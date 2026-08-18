#ifndef __AR_DSP_TWO_STAGE_RESIZE_H__
#define __AR_DSP_TWO_STAGE_RESIZE_H__
// #include "cv_common_types.h"



int cvRotate( AR_U64 ext_src_y, AR_U64 ext_src_u, AR_U64 ext_src_v, int src_width,  int src_height, int src_stride ,AR_U64 ext_dst_y,AR_U64 ext_dst_u,  AR_U64 ext_dst_v, 
  int dst_width,  int dst_height,  int dst_stride, int flag);
#endif

