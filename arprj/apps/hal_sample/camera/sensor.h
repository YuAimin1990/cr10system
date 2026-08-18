#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "hal_i2c.h"

typedef struct __STRU_SENSOR_OBJ_T__ STRU_SENSOR_OBJ_T;


struct __STRU_SENSOR_OBJ_T__
{
  char *name;
  int (*init)(STRU_SENSOR_OBJ_T *p_obj,int vin_fd,int pide_fd,int pipe_id,int res_index);
  int (*deinit)(STRU_SENSOR_OBJ_T *p_obj);
  int pipe_fd;
  int vin_fd;
  int pipe_id;
  int event_client_id;
  ar_os_thread_id_t event_thread;
  int event_data_size;
  int fd_mem;
  int frame_len_new;
  int current_res_index;
  int hdr_mode;
  int i2c_fd;
  int slave_addr;
};
STRU_SENSOR_OBJ_T *get_sensor_obj(char *name);
int vin_i2c_write(int fd,int slave_addr_7bit,uint32_t addr_reg,int addr_byte_num,uint32_t val,int data_byte_num);
int vin_i2c_read(int fd,int slave_addr_7bit,uint32_t addr_reg,int addr_byte_num,uint32_t *val,int data_byte_num);
#endif
