#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "mpi_dsp_api.h"
#include "hal_dsp_api.h"
#include "ar_dsp_rotate.h"
#include <assert.h>

typedef struct
{
  unsigned int y;
  int y_width;
  int y_height;
  int y_stride;

  unsigned int u;
  int u_width;
  int u_height;
  int u_stride;

  unsigned int v;
  int v_width;
  int v_height;
  int v_stride;
} IMG_MULTI_PLANAR_s;


typedef struct
{
  IMG_MULTI_PLANAR_s src;
  IMG_MULTI_PLANAR_s dst;
  int pingpong_flag;    //0: non-pingpong  1:pingpong
} SPECIFIC_ROTATION_MSG_PARAM_st;


int cvRotate( AR_U64 ext_src_y, AR_U64 ext_src_u, AR_U64 ext_src_v, int src_width,  int src_height, int src_stride ,AR_U64 ext_dst_y,AR_U64 ext_dst_u,  AR_U64 ext_dst_v, 
  int dst_width,  int dst_height,  int dst_stride, int flag)
{
  SPECIFIC_ROTATION_MSG_PARAM_st param; 
  memset(&param, 0, sizeof(SPECIFIC_ROTATION_MSG_PARAM_st));

  param.src.y = (AR_U32)(ext_src_y & 0xFFFFFFFF);
  param.src.y_width = src_width;
  param.src.y_height = src_height;
  param.src.y_stride = src_stride;
  param.src.u = (AR_U32)(ext_src_u & 0xFFFFFFFF);
  param.src.u_width = src_width>>1;
  param.src.u_height = src_height>>1;
  param.src.u_stride = src_stride>>1;
  param.src.v = (AR_U32)(ext_src_v & 0xFFFFFFFF);;
  param.src.v_width = src_width>>1;
  param.src.v_height = src_height>>1;
  param.src.v_stride = src_stride>>1;

  //for dst
  int dst_uv_stride = dst_width>>1;
  param.dst.y = (AR_U32)(ext_dst_y & 0xFFFFFFFF);
  param.dst.y_width = dst_width;
  param.dst.y_height = dst_height;
  param.dst.y_stride = dst_width;
  param.dst.u = (AR_U32)(ext_dst_u & 0xFFFFFFFF);
  param.dst.u_width = dst_width>>1;
  param.dst.u_height = dst_height>>1;
  param.dst.u_stride = dst_uv_stride;
  param.dst.v = (AR_U32)(ext_dst_v & 0xFFFFFFFF);
  param.dst.v_width = dst_width>>1;
  param.dst.v_height = dst_height>>1;
  param.dst.v_stride = dst_uv_stride;
  param.pingpong_flag = flag;

  #if 0
  printf("param.src.y = %x \r\n",param.src.y);
  printf("param.src.y_width = %d \r\n",param.src.y_width);
  printf("param.src.y_height = %d \r\n",param.src.y_height);
  printf("param.src.y_stride = %d \r\n",param.src.y_stride);
  printf("param.src.u = %x \r\n",param.src.u);
  printf("param.src.u_width = %d \r\n",param.src.u_width);
  printf("param.src.u_height = %d \r\n",param.src.u_height);
  printf("param.src.u_stride = %d \r\n",param.src.u_stride);
  printf("param.src.v = %x \r\n",param.src.v);
  printf("param.src.v_width = %d \r\n",param.src.v_width);
  printf("param.src.v_height = %d \r\n",param.src.v_height);
  printf("param.src.v_stride = %d \r\n",param.src.v_stride);

  printf("param.dst.y = %x \r\n",param.dst.y);
  printf("param.dst.y_width = %d \r\n",param.dst.y_width);
  printf("param.dst.y_height = %d \r\n",param.dst.y_height);
  printf("param.dst.y_stride = %d \r\n",param.dst.y_stride);
  printf("param.dst.u = %x \r\n",param.src.u);
  printf("param.dst.u_width = %d \r\n",param.dst.u_width);
  printf("param.dst.u_height = %d \r\n",param.dst.u_height);
  printf("param.dst.u_stride = %d \r\n",param.dst.u_stride);
  printf("param.dst.v = %x \r\n",param.src.v);
  printf("param.dst.v_width = %d \r\n",param.dst.v_width);
  printf("param.dst.v_height = %d \r\n",param.dst.v_height);
  printf("param.dst.v_stride = %d \r\n",param.dst.v_stride);
  #endif

  AR_DSP_TASK_S stTask = {0};

  strncpy((char *)stTask.s8Name, "ArArmCvRotate", 31);//ArArmCvApp
  stTask.u32Priority = 0;
  stTask.u64MsgBodyAddr = (AR_U64)(&param);
  stTask.u32MsgLen = sizeof(param);
  
  printf("stTask.u32MsgLen = %d\r\n",stTask.u32MsgLen);
  AR_S32 s32Ret = 0;
  s32Ret = AR_MPI_DSP_RemoteCall(0, &stTask, 1);//block-mode call

  if(s32Ret < 0)
    return -1;

  return 0;
 
}


