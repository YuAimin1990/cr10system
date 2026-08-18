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
#include "ar_dsp_rotate_test.h"

#include <assert.h>

static  long long get_current_time()
{
  struct  timeval    tv;    
  struct  timezone   tz;    
  long long ncurrent_time = 0x0LL;   
  gettimeofday(&tv,&tz);   
  ncurrent_time = ((long long)tv.tv_sec) * 1000000 + (long long)tv.tv_usec;
  return ncurrent_time;
}

int cvRotate_test(AR_CHAR* input_path, AR_CHAR* output_path, int src_width, int src_height, int src_stride, int flag)
{
  FILE * pFile = NULL;
  //spec for rot90
  int dst_width = src_height;
  int dst_height = src_width;
  int dst_stride = dst_width;

  //malloc mmz for src
  AR_U64 ext_src_y_pa = 0;
  void * ext_src_y_va = NULL;
  int ret = ar_hal_sys_mmz_alloc(&ext_src_y_pa, &ext_src_y_va, "rotsy", NULL, src_stride*src_height);
  if(ret)
  {
    printf(" get src y addr error!\r\n");
    return -1;
  }
  memset((AR_CHAR *)ext_src_y_va, 0, src_stride*src_height);

  AR_U64 ext_src_u_pa = 0;
  void * ext_src_u_va = NULL;
  ret = ar_hal_sys_mmz_alloc(&ext_src_u_pa, &ext_src_u_va, "rotsu", NULL, (src_stride>>1)*(src_height>>1));
  if(ret)
  {
    printf(" get src u addr error!\r\n");
    return -1;
  }
  memset((AR_CHAR *)ext_src_u_va, 0, (src_stride>>1)*(src_height>>1));

  AR_U64 ext_src_v_pa = 0;
  void * ext_src_v_va = NULL;
  ret = ar_hal_sys_mmz_alloc(&ext_src_v_pa, &ext_src_v_va, "rotsv", NULL, (src_stride>>1)*(src_height>>1));
  if(ret)
  {
    printf(" get src v addr error!\r\n");
    return -1;
  }
  memset((AR_CHAR *)ext_src_v_va, 0, (src_stride>>1)*(src_height>>1));

  //malloc mmz for dst
  AR_U64 ext_dst_y_pa = 0;
  void * ext_dst_y_va = NULL;
  ret = ar_hal_sys_mmz_alloc(&ext_dst_y_pa, &ext_dst_y_va, "rotdy", NULL, dst_stride*dst_height);
  if(ret)
  {
    printf(" get dst y addr error!\r\n");
    return -1;
  }
  memset((AR_CHAR *)ext_dst_y_va, 0, dst_stride*dst_height);

  AR_U64 ext_dst_u_pa = 0;
  void * ext_dst_u_va = NULL;
  ret = ar_hal_sys_mmz_alloc(&ext_dst_u_pa, &ext_dst_u_va, "rotdu", NULL, (dst_stride>>1)*(dst_height>>1));
  if(ret)
  {
    printf(" get dst u addr error!\r\n");
    return -1;
  }
  memset((AR_CHAR *)ext_dst_u_va, 0, (dst_stride>>1)*(dst_height>>1));

  AR_U64 ext_dst_v_pa = 0;
  void * ext_dst_v_va = NULL;
  ret = ar_hal_sys_mmz_alloc(&ext_dst_v_pa, &ext_dst_v_va, "rotdv", NULL, (dst_stride>>1)*(dst_height>>1));
  if(ret)
  {
    printf(" get dst v addr error!\r\n");
    return -1;
  }
  memset((AR_CHAR *)ext_dst_v_va, 0, (dst_stride>>1)*(dst_height>>1));

  // read input file
  FILE * fp_in = fopen(input_path, "rb");
  int num_read = fread(ext_src_y_va, 1, src_stride*src_height, fp_in);
  if (num_read !=  src_stride*src_height)
  {
    printf("read y plane failed.\n");
    fclose(fp_in);
    return -1;
  }

  num_read = fread(ext_src_u_va, 1, (src_stride>>1)*(src_height>>1), fp_in);
  if (num_read !=  (src_stride>>1)*(src_height>>1))
  {
    printf("read u plane failed.\n");
    fclose(fp_in);
    return -1;
  }

  num_read = fread(ext_src_v_va, 1, (src_stride>>1)*(src_height>>1), fp_in);
  if (num_read !=  (src_stride>>1)*(src_height>>1))
  {
    printf("read v plane failed.\n");
    fclose(fp_in);
    return -1;
  }
  fflush(fp_in);
  fclose(fp_in);


  unsigned long long start = get_current_time();

  ret = cvRotate( ext_src_y_pa, ext_src_u_pa, ext_src_v_pa, src_width,  src_height, src_stride, ext_dst_y_pa, ext_dst_u_pa,  ext_dst_v_pa, dst_width,  dst_height,  dst_stride, flag);
  if(ret < 0)
  {
	printf("cvRotate failed \r\n");
  }
  
  unsigned long long end = get_current_time();
  printf("cvRotate time consumed: %lluus\r\n", end-start); 


  

  FILE * fp_out = fopen(output_path, "wb");
  int num_write = fwrite(ext_dst_y_va, 1, dst_width*dst_height, fp_out);

  if (num_write != dst_width*dst_height) 
  {
    printf("Write file failed.\n");
    fclose(fp_out);
    return -1;
  }

  int dst_uv_stride = dst_width>>1;
  for(int i=0; i<(dst_height>>1); i++ )
  {
    int num_write = fwrite(ext_dst_u_va + i*dst_uv_stride, 1, dst_width>>1, fp_out);
    if (num_write != dst_width>>1)  {
      printf("Write file failed.\n");
      fclose(fp_out);
      return -1;
    }
  }

  for(int i=0; i<(dst_height>>1); i++ )
  {
    int num_write = fwrite(ext_dst_v_va + i*dst_uv_stride, 1, dst_width>>1, fp_out);
    if (num_write != dst_width>>1)  {
      printf("Write file failed.\n");
      fclose(fp_out);
      return -1;
    }
  }
  
  fflush(fp_out);
  fclose(fp_out); 

  if(ext_src_y_va)
		ar_hal_sys_mmz_free((AR_U64)ext_src_y_pa, (AR_VOID *)ext_src_y_va);
  if(ext_src_u_va)
		ar_hal_sys_mmz_free((AR_U64)ext_src_u_pa, (AR_VOID *)ext_src_u_va);
  if(ext_src_v_va)
		ar_hal_sys_mmz_free((AR_U64)ext_src_v_pa, (AR_VOID *)ext_src_v_va);
  if(ext_dst_y_va)
		ar_hal_sys_mmz_free((AR_U64)ext_dst_y_pa, (AR_VOID *)ext_dst_y_va);
  if(ext_dst_u_va)
		ar_hal_sys_mmz_free((AR_U64)ext_dst_u_pa, (AR_VOID *)ext_dst_u_va);
  if(ext_dst_v_va)
		ar_hal_sys_mmz_free((AR_U64)ext_dst_v_pa, (AR_VOID *)ext_dst_v_va);

  printf("ar arm rotate finished.\n");
  
	return 0;
}



