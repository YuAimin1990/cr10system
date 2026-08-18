#include "camera.h"
#include "sensor.h"

extern STRU_SENSOR_OBJ_T imx307_obj;

static STRU_SENSOR_OBJ_T *obj[]=
{
   &imx307_obj,
};

STRU_SENSOR_OBJ_T *get_sensor_obj(char *name)
{
    for(int i=0;i<sizeof(obj)/sizeof(STRU_SENSOR_OBJ_T *);i++)
    {
       if(!strcmp(obj[i]->name,name))
       {
         return obj[i];
       }
    }
	return NULL;
}


int vin_i2c_write(int fd,int slave_addr_7bit,uint32_t addr_reg,int addr_byte_num,uint32_t val,int data_byte_num)
{

	uint8_t addr[4]={0};
	uint8_t data[4]={0};
	if(addr_byte_num==1)
	{
	   addr[0]=addr_reg&0xff;
	}else if(addr_byte_num==2)
	{
		addr[0]=(addr_reg>>8)&0xff;
		addr[1]=(addr_reg)&0xff;
	}else if(addr_byte_num==4)
	{
		addr[0]=(addr_reg>>24)&0xff;
		addr[1]=(addr_reg>>16)&0xff;
		addr[2]=(addr_reg>>8)&0xff;
		addr[3]=(addr_reg)&0xff;
	}

	if(data_byte_num==1)
	{
		data[0]=val&0xff;
	}else if(data_byte_num==2)
	{
		data[0]=(val>>8)&0xff;
		data[1]=(val)&0xff;
	}else if(data_byte_num==4)
	{
		data[0]=(val>>24)&0xff;
		data[1]=(val>>16)&0xff;
		data[2]=(val>>8)&0xff;
		data[3]=(val)&0xff;
	}
	uint8_t buffer[8]={0};
	int i=0;
	for(i=0;i<addr_byte_num;i++)
	{
		buffer[i]=addr[i];
	}
	for(int j=0;j<data_byte_num;j++)
	{
	   buffer[i+j]=data[j];
	}
	ar_hal_i2c_master_write(fd,slave_addr_7bit,7,buffer,addr_byte_num+data_byte_num);
	return 0;

}

int vin_i2c_read(int fd,int slave_addr_7bit,uint32_t addr_reg,int addr_byte_num,uint32_t *val,int data_byte_num)
{
	uint8_t addr[4]={0};
	uint8_t data[4]={0};

	if(addr_byte_num==1)
	{
	   addr[0]=addr_reg&0xff;
	}else if(addr_byte_num==2)
	{
		addr[0]=(addr_reg>>8)&0xff;
		addr[1]=(addr_reg)&0xff;
	}else if(addr_byte_num==4)
	{
		addr[0]=(addr_reg>>24)&0xff;
		addr[1]=(addr_reg>>16)&0xff;
		addr[2]=(addr_reg>>8)&0xff;
		addr[3]=(addr_reg)&0xff;
	}
	ar_hal_i2c_master_read(fd,slave_addr_7bit,7,addr,addr_byte_num,data,data_byte_num);

	if(data_byte_num==1)
	{
		*val=((uint32_t)data[0]);
	}else if(data_byte_num==2)
	{
		*val=((uint32_t)data[0]<<8)|((uint32_t)data[1]);
	}else if(data_byte_num==2)
	{
		*val=((uint32_t)data[0]<<24)|((uint32_t)data[1]<<16)|((uint32_t)data[2]<<8)|((uint32_t)data[3]);
	}
    return 0;
}


