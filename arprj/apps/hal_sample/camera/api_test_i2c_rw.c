#include"camera.h"
#include "sensor.h"

static int api_test_i2c_rw(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
       index=camera_find_cmd_pra_index_by_name("-r",p_cmd);
	   if(index>0)
	   {
          int i2c_index=strtoul(p_cmd->args[index+1],NULL,0);
		  int  slave_addr_7bit=strtoul(p_cmd->args[index+2],NULL,0);
		  uint32_t  addr_reg=strtoul(p_cmd->args[index+3],NULL,0);
		  int  addr_byte_num=strtoul(p_cmd->args[index+4],NULL,0);
		  uint32_t val=0;
		  int  data_byte_num=strtoul(p_cmd->args[index+5],NULL,0);
		  int fd=ar_hal_i2c_open(i2c_index);
		  if(fd>0)
		  {
			  vin_i2c_read(fd,slave_addr_7bit,addr_reg,addr_byte_num,&val,data_byte_num);
			  ar_always("read i2c dev %d with 7bit_addr=0x%x  0x%x=0x%x",i2c_index,slave_addr_7bit,addr_reg,val);
              ar_hal_i2c_close(fd);
		  }else
		  {
		     ar_err("err open i2c dev i2c_index=%d",i2c_index);
		  }
	   }
	   index=camera_find_cmd_pra_index_by_name("-w",p_cmd);
	   if(index>0)
	   {
          int i2c_index=strtoul(p_cmd->args[index+1],NULL,0);
		  int  slave_addr_7bit=strtoul(p_cmd->args[index+2],NULL,0);
		  uint32_t  addr_reg=strtoul(p_cmd->args[index+3],NULL,0);
		  int  addr_byte_num=strtoul(p_cmd->args[index+4],NULL,0);
		  uint32_t val=strtoul(p_cmd->args[index+5],NULL,0);
		  int  data_byte_num=strtoul(p_cmd->args[index+6],NULL,0);
		  int fd=ar_hal_i2c_open(i2c_index);
		  if(fd>0)
		  {
		      vin_i2c_write(fd,slave_addr_7bit,addr_reg,addr_byte_num,val,data_byte_num);
			  ar_printf("write i2c dev %d with 7bit_addr=0x%x  0x%x=0x%x\n",i2c_index,slave_addr_7bit,addr_reg,val);
              ar_hal_i2c_close(fd);
		  }else
		  {
		     ar_err("err open i2c dev i2c_index=%d",i2c_index);
		  }
	   }
   }
   return 0;
}

__test_camera_case__(i2c_rw)=
{
   "--i2c_rw",
   api_test_i2c_rw,
   {
      "test i2c read and write or assist debug, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-r [i2c_index] [slave_addr_7bit] [addr] [addr_byte_num:1/2] [data_byte_num:1/2/4]",
	  "-w [i2c_index] [slave_addr_7bit] [addr] [addr_byte_num:1/2] [data] [data_byte_num:/1/2/4]",
	  NULL,
   },
};
