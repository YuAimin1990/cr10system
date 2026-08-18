#include "camera.h"
#include "sensor.h"

static STRU_REGISTER_SETTING_T g_read_id_setting[]=
{
   {REGISTER_OP_READ,0x301e,0,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
   {REGISTER_OP_READ,0x31dc,0,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
};

static STRU_REGISTER_SETTING_T g_res_2lane_25fps[]=
{
	{REGISTER_OP_WRITE,0x3002,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3005,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, // ADBIT 12
	{REGISTER_OP_WRITE,0x3007,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, // v direction ; h direction
	{REGISTER_OP_WRITE,0x3009,0x02,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, // 30fps
	{REGISTER_OP_WRITE,0x300A,0xF0,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, // black level
	{REGISTER_OP_WRITE,0x3011,0x0a,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3018,0x46,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //VMAX: 1125(30fps) /1350(25fps)
	{REGISTER_OP_WRITE,0x3019,0x05,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x301c,0x30,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //HMAX: 4400
	{REGISTER_OP_WRITE,0x301d,0x11,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3046,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //
	{REGISTER_OP_WRITE,0x304B,0x0a,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, // XVS/XHS hsync/vsync
	{REGISTER_OP_WRITE,0x305C,0x18,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //INCK
	{REGISTER_OP_WRITE,0x305D,0x03,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x305E,0x20,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x305F,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x309e,0x4a,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x309f,0x4a,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x311C,0x0E,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //1e
	{REGISTER_OP_WRITE,0x3128,0x04,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //05
	{REGISTER_OP_WRITE,0x3129,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},// adbit1 12bit
	{REGISTER_OP_WRITE,0x313b,0x41,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x315E,0x1A,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //INCKSEL5
	{REGISTER_OP_WRITE,0x3164,0x1A,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //INCKSEL6
	{REGISTER_OP_WRITE,0x317C,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //ADBIT2
	{REGISTER_OP_WRITE,0x31EC,0x0E,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3405,0x10,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3407,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},//03 -> 01
	{REGISTER_OP_WRITE,0x3414,0x0A,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3418,0x38,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //38 -> 49
	{REGISTER_OP_WRITE,0x3419,0x04,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3441,0x0C,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},// 12bit raw
	{REGISTER_OP_WRITE,0x3442,0x0C,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},// 12bit raw
	{REGISTER_OP_WRITE,0x3443,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},// 2 lane
	{REGISTER_OP_WRITE,0x3444,0x20,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3445,0x25,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3446,0x57,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3447,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3448,0x37,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3449,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x344A,0x1F,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x344B,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x344C,0x1F,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x344D,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x344E,0x1F,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x344F,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3450,0x77,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3451,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3452,0x1F,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3453,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	{REGISTER_OP_WRITE,0x3454,0x17,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
    {REGISTER_OP_WRITE,0x3455,0x0,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},

	{REGISTER_OP_WRITE,0x3472,0x80,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //80 -> 9c
	{REGISTER_OP_WRITE,0x3473,0x07,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
	//{REGISTER_OP_WRITE,0x3480,0x49,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
};
// stream on off register setting
static STRU_REGISTER_SETTING_T g_stream_on[]=
{
	{REGISTER_OP_WRITE,0x3000,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},  // operating
	{REGISTER_OP_WRITE,0x3002,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},  //Master mode start

	//{REGISTER_OP_WRITE,0x3002,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //Master mode stop
	//{REGISTER_OP_WRITE,0x3000,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},	// standby
};
static STRU_REGISTER_SETTING_T g_stream_off[]=
{
    {REGISTER_OP_WRITE,0x3002,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0}, //Master mode stop
    {REGISTER_OP_WRITE,0x3000,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},  // standby

};

/*	 FLIP_OFF_MIRROR_OFF,*/
static STRU_REGISTER_SETTING_T g_flip_off_mirro_off[]=
{
  {REGISTER_OP_WRITE,0x3007,0x00,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
};
/*FLIP_OFF_MIRROR_ON*/
static STRU_REGISTER_SETTING_T g_flip_off_mirro_on[]=
{
  {REGISTER_OP_WRITE,0x3007,0x02,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
};
/*FLIP_ON_MIRROR_OFF*/
static STRU_REGISTER_SETTING_T g_flip_on_mirro_off[]=
{
  {REGISTER_OP_WRITE,0x3007,0x01,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
};
/*FLIP_ON_MIRRO_ON,*/
static STRU_REGISTER_SETTING_T g_flip_on_mirro_on[]=
{
  {REGISTER_OP_WRITE,0x3007,0x03,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
};


static STRU_SENSOR_SLAVE_INFO_T g_slave_info=
{
		/*sensor_slave_info_t *slave_infor*/
		.reg_read_id={
    		{REGISTER_OP_READ,0x301e,0,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
    		{REGISTER_OP_READ,0x31dc,0,I2C_ADDR_TYPE_UINT16,I2C_DATA_TYPE_UINT8,0,0,0,0},
		},
		/*sensor_id[4]*/
		.sensor_id={0xb2,0x0,0,0},
		/*id_count*/
		.id_count=1,
		/*i2c_component_index*/
		.i2c_component_index=0,
		/*slave_address_7bit*/
		.slave_address_7bit=0x34>>1,
};

static STRU_SENSOR_OUT_INFOR_T g_out_info=
{
		/*out_format*/
		.out_format=SENSOR_BAYER,
		/*link_mod*/
		.link_mod=LINK_MODE_MIPI_CSI,
		/*bit_width*/
		.bit_width=SENSOR_BIT_12_BIT,
		/*bayer format when out_format==SENSOR_BAYER*/
		.bayer=BAYER_RGGB,
		/*yuv format when out_format ==SENSOR_YUV */
		.yuv=YUV420,
		/*rgb format when out_format==SENSOR_RGB*/
		.rgb=RGB888,
		/*mipi desc when link_mod==LINK_MODE_MIPI_CSI */
		.mipi_desc={
		   /*mipi_index,will be changed by dts config, here give a default*/
		   .mipi_index=0,
		   /*mipi_lane_cout*/
		   .mipi_lane_cout=2,
		   /*settle count*/
		   .settle_count=1,
		 },
		 /*cam_infor */
		 .cam_infor={
			/*mount_angle*/
			.mount_angle=MOUNT_ANGLE_90,
			/*face*/
			.face=CAM_FACE_BACK,
		  },
};

STRU_SENSOR_RES_INFO_T g_sensor_res_info[]=
{
 	{
			.crop_en=0,
			.width=1920,
			.height=1080,
			.line_length=2200,
			.frame_length=1350,
			.clk_per_lane=450,
			.mipi_pix_clk=75,
			.fps=25,
			.mipi_lanes = 2,
			.x_start=0,
			.y_start=0,
			.x_width=1920,
			.y_height=1080,
			.tuning_name="/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin",
			.sensor_hdr=0,
			.hdr_width=1920,
			.hdr_height=1080,
			.aec_factor=1,
			.awb_factor=1,
			.ltm_factor=1,
    },
};

static int sensor_read_16_8(STRU_SENSOR_OBJ_T *p_obj,int addr,int *data)
{
    vin_i2c_read(p_obj->i2c_fd,p_obj->slave_addr,addr,2,(uint32_t*)data,1);
    return 0;
}
static int sensor_write_16_8(STRU_SENSOR_OBJ_T *p_obj,int addr,int data)
{
    vin_i2c_write(p_obj->i2c_fd,p_obj->slave_addr,addr,2,data,1);
    return 0;
}


static int sensor_power_on(STRU_SENSOR_OBJ_T *p_obj)
{
   ar_always("enter");
   int reset_bit=4;
   int pow_bit=0;
   ar_hal_vin_write_reg(p_obj->vin_fd,0x07890004,0,reset_bit,reset_bit);
   ar_hal_vin_write_reg(p_obj->vin_fd,0x07890004,0,pow_bit,pow_bit);

   ar_delay(100);
   ar_hal_vin_write_reg(p_obj->vin_fd,0x07890004,1,pow_bit,pow_bit);
   ar_delay(100);
   ar_hal_vin_write_reg(p_obj->vin_fd,0x07890004,1,reset_bit,reset_bit);
   ar_delay(100);
   return 0;
}
static int sensor_power_off(STRU_SENSOR_OBJ_T *p_obj)
{
   ar_always("enter");
   int reset_bit=4;
   int pow_bit=0;
   ar_hal_vin_write_reg(p_obj->vin_fd,0x07890004,0,reset_bit,reset_bit);
   ar_hal_vin_write_reg(p_obj->vin_fd,0x07890004,0,pow_bit,pow_bit);
   return 0;
}

static int probe_sensor(STRU_SENSOR_OBJ_T *p_obj)
{
    int val=0;
	sensor_power_on(p_obj);
	sensor_read_16_8(p_obj,g_read_id_setting[0].address,&val);
	sensor_power_off(p_obj);

   ar_always("val=0x%x excepted 0x%x",val,g_slave_info.sensor_id[0]);
   if(val==g_slave_info.sensor_id[0])
   {
      ar_always("probe succuess");
      return 1;
   }else
   {
      ar_err("probe fail");
      return 0;
   }
}

static int sensor_clt(STRU_SENSOR_OBJ_T *p_obj,STRU_SENSOR_CB_EVENT_CTL_PRA_T *p_ctl)
{
	AR_S32 offset_ret=0;
	void *event_data_maped=ar_hal_vin_map_phy_addr(p_obj->fd_mem,p_ctl->pra_pa,p_obj->event_data_size,&offset_ret);
	// void *event_data=event_data_maped+offset_ret;
	switch(p_ctl->ctl_code)
	{
	   default:
	   break;
	}
	ar_hal_vin_ummap_vaddr(event_data_maped,p_obj->event_data_size);
	return 0;
}

static int  set_real_gain(STRU_SENSOR_OBJ_T *p_obj, float gain_val) {
  // uint8_t gainlow=0;
  // uint8_t gainhigh=0;
  uint32_t register_gain=0;
  // int status=0;

  if( gain_val < 1.0)
      gain_val = 1.0;
  if(gain_val > 4096.0)
      gain_val = 4096.0;
#ifdef SENSOR_HCG_MODE_ENABLE
  uint32_t register_val = 0;
  static int dcg_triger=0;

  if(gain_val >= 64 && !dcg_triger)
  {
	  dcg_triger=1;
  }

  if(gain_val< 32 && dcg_triger)
  {
	  dcg_triger=0;
  }
  //max analog gain
  if(sensor_read_16_8(p_obj, 0x3009, &register_val)==0)
  {
      if(dcg_triger)
      {
         gain_val = gain_val / 2;
         register_val |= (1 << 4);
         sensor_write_16_8(p_obj, 0x3009, register_val);
      }else{
         register_val &= ~(1 << 4);
         sensor_write_16_8(p_obj, 0x3009, register_val);
      }
  }else
  {
      ar_err("i2c read err, not to wrtie hcg reg, but need change gain  value according dcg mode");
      if(dcg_triger)
      {
         gain_val = gain_val / 2;
      }else{
         gain_val=gain_val;
      }
  }
#endif

  register_gain=(uint32_t)(20*log10(gain_val)*(10/3.0));
  sensor_write_16_8(p_obj,0x3014, register_gain);
  return 0;
}

static int  set_exp(STRU_SENSOR_OBJ_T *p_obj, uint32_t exp_val) {

  uint8_t explow,expmid,exphigh;
  uint32_t frame_length_line=p_obj->frame_len_new;
  uint8_t frame_length_line_low;
  uint8_t frame_length_line_mid;
  uint8_t frame_length_line_high;
  uint32_t interage_line=0;

  if(exp_val>0xfffff)
    exp_val=0xfffff;

  if(exp_val>=frame_length_line)
  {
	 frame_length_line=exp_val+4;//set 16 as exposure margin
  }
  interage_line=frame_length_line-exp_val-1;

  if(interage_line<=4)
    interage_line=4;

  exphigh = (uint8_t) ((0x30000&interage_line)>>16);
  expmid  = (uint8_t) ((0xff00&interage_line)>>8);
  explow  = (uint8_t) (0xff&interage_line);

  frame_length_line_low=(uint8_t)(frame_length_line&0xff);
  frame_length_line_mid=(uint8_t)((frame_length_line&0xff00)>>8);
  frame_length_line_high=(uint8_t)((frame_length_line&0x30000)>>16);


  sensor_write_16_8(p_obj,0x301a, frame_length_line_high);
  sensor_write_16_8(p_obj,0x3019, frame_length_line_mid);
  sensor_write_16_8(p_obj,0x3018, frame_length_line_low);


  sensor_write_16_8(p_obj,0x3022, exphigh);
  sensor_write_16_8(p_obj,0x3021, expmid);
  sensor_write_16_8(p_obj,0x3020, explow);
  return CAM_ERR_SUCCUESS;
}
static int  set_real_gain_hdr(STRU_SENSOR_OBJ_T *p_obj, float gain_val,float *gain_ration) {

  uint32_t register_gain=0;
  float    long_gain=0;
  int status=0;
  float ration=gain_ration[0];

  uint32_t register_val=0;;
  if( gain_val < 1.0)
      gain_val = 1.0;
  if(gain_val > 4096.0)
      gain_val = 4096.0;

  long_gain=gain_val;
  gain_val=gain_val/ration;

  //ar_printf("short %d long %d ration=%d \n",float2int(gain_val),float2int(long_gain),float2int(ration));

  register_gain=(uint32_t)(20*log10(long_gain)*(10/3.0));
  sensor_write_16_8(p_obj,0x3014, register_gain);
  //register_gain=(uint32_t)(20*log10(gain_val)*(10/3.0));
  //sensor_write_16_8(sensor_lib,0x3014, register_gain);
  register_gain=(uint32_t)(20*log10(gain_val)*(10/3.0));
  sensor_write_16_8(p_obj,0x30f2, register_gain);

  if(sensor_read_16_8(p_obj, 0x3009, (int*)&register_val)==0)
  {
      register_val &= ~(1 << 4);
      sensor_write_16_8(p_obj, 0x3009, register_val);
  }

  return status;
}
static int  set_exp_hdr(STRU_SENSOR_OBJ_T *p_obj, uint32_t exp_val,float *time_ration)
{
  uint8_t explow,expmid,exphigh;
  STRU_SENSOR_RES_INFO_T *res=&g_sensor_res_info[p_obj->current_res_index];
  uint32_t frame_length_line=res->frame_length;
  uint32_t frame_length_line_short=res->frame_length_short;
  uint8_t frame_length_line_low;
  uint8_t frame_length_line_mid;
  uint8_t frame_length_line_high;
  uint32_t  interage_line=0;
  uint32_t  interage_line_long=0;
  uint32_t exp_time_long=0;
  float ration=time_ration[0];

  exp_val*=2;
  if(exp_val>=frame_length_line)
  {
      exp_val=frame_length_line;
  }
  exp_time_long=exp_val;
  exp_val=exp_val/ration;

  if(exp_val>=frame_length_line_short)
  {
     exp_val=frame_length_line_short;
  }

  ar_debug("short %d long %d time_ration=%d\n",exp_val,exp_time_long,float2int(ration));

  if(frame_length_line>=(exp_time_long+1))
  {
     interage_line_long=frame_length_line-exp_time_long-1;
  }else
  {
     interage_line_long=0;
  }

  if(interage_line_long>(frame_length_line-2))
  {
     interage_line_long=(frame_length_line-2);
  }

  if(interage_line_long<(frame_length_line_short+2))
  {
     interage_line_long=(frame_length_line_short+2);
  }

  if(frame_length_line_short>=exp_val+1)
  {
     interage_line=frame_length_line_short-exp_val-1;
  }else
  {
     interage_line=0;
  }


  if(interage_line>(frame_length_line_short-2))
  {
     interage_line=(frame_length_line_short-2);
  }

  if(interage_line<2)
  {
     interage_line=2;
  }
  ar_debug("short %d long %d \n",interage_line,interage_line_long);
  frame_length_line=frame_length_line/2;

  frame_length_line_low=(uint8_t)(frame_length_line&0xff);
  frame_length_line_mid=(uint8_t)((frame_length_line&0xff00)>>8);
  frame_length_line_high=(uint8_t)((frame_length_line&0x30000)>>16);

  sensor_write_16_8(p_obj,0x301a, frame_length_line_high);
  sensor_write_16_8(p_obj,0x3019, frame_length_line_mid);
  sensor_write_16_8(p_obj,0x3018, frame_length_line_low);

  //write short exp
  exphigh = (uint8_t) ((0x30000&interage_line)>>16);
  expmid  = (uint8_t) ((0xff00&interage_line)>>8);
  explow  = (uint8_t) (0xff&interage_line);

  sensor_write_16_8(p_obj,0x3022, exphigh);
  sensor_write_16_8(p_obj,0x3021, expmid);
  sensor_write_16_8(p_obj,0x3020, explow);

  //write long exp
  exphigh = (uint8_t) ((0x30000&interage_line_long)>>16);
  expmid  = (uint8_t) ((0xff00&interage_line_long)>>8);
  explow  = (uint8_t) (0xff&interage_line_long);

  sensor_write_16_8(p_obj,0x3026, exphigh);
  sensor_write_16_8(p_obj,0x3025, expmid);
  sensor_write_16_8(p_obj,0x3024, explow);

  return CAM_ERR_SUCCUESS;
}
static int sensor_set_hdr(STRU_SENSOR_OBJ_T *p_obj,int enable)
{
    ar_always("enable=%d",enable);
    uint32_t register_val=0;
    if(enable)
    {
		sensor_write_16_8(p_obj,0x3010,0x61);
        sensor_read_16_8(p_obj, 0x3009, (int*)&register_val);
        register_val &= ~(1 << 4);
        sensor_write_16_8(p_obj, 0x3009, register_val);

    }else
    {
        //sensor_write_16_8(sensor_lib,0x3010,0x21);
        sensor_write_16_8(p_obj,0x3010,0x61);
    }
    return 0;
}
static int  set_exp_hdr_off(STRU_SENSOR_OBJ_T *p_obj, uint32_t exp_val,float *time_ration) {
  uint8_t explow,expmid,exphigh;
  // STRU_SENSOR_RES_INFO_T *res=&g_sensor_res_info[p_obj->current_res_index];
  uint32_t frame_length_line=p_obj->frame_len_new;
  uint8_t frame_length_line_low;
  uint8_t frame_length_line_mid;
  uint8_t frame_length_line_high;
  uint32_t interage_line=0;

  if(exp_val>0xfffff)
    exp_val=0xfffff;

  if(exp_val>=frame_length_line)
  {
	  frame_length_line=exp_val+4;//set 16 as exposure margin
  }
  interage_line=frame_length_line-exp_val-1;

  if(interage_line<=4)
    interage_line=4;

  exphigh = (uint8_t) ((0x30000&interage_line)>>16);
  expmid  = (uint8_t) ((0xff00&interage_line)>>8);
  explow  = (uint8_t) (0xff&interage_line);

  frame_length_line=frame_length_line/2;
  frame_length_line_low=(uint8_t)(frame_length_line&0xff);
  frame_length_line_mid=(uint8_t)((frame_length_line&0xff00)>>8);
  frame_length_line_high=(uint8_t)((frame_length_line&0x30000)>>16);

  sensor_write_16_8(p_obj,0x301a, frame_length_line_high);
  sensor_write_16_8(p_obj,0x3019, frame_length_line_mid);
  sensor_write_16_8(p_obj,0x3018, frame_length_line_low);
  sensor_write_16_8(p_obj,0x3022, exphigh);
  sensor_write_16_8(p_obj,0x3021, expmid);
  sensor_write_16_8(p_obj,0x3020, explow);
  ar_debug("frame_length_line=%d line=%d",frame_length_line,exp_val);

  return CAM_ERR_SUCCUESS;
}

static int sensor_group_on(STRU_SENSOR_OBJ_T *p_obj)
{
   sensor_write_16_8(p_obj,0x3001, 1);
   return 0;
}
static int sensor_group_off(STRU_SENSOR_OBJ_T *p_obj)
{
   sensor_write_16_8(p_obj,0x3001, 0);
   return 0;
}

static int sensor_aec_update(STRU_SENSOR_OBJ_T *p_obj,STRU_SENSOR_CB_EVENT_AEC_UPDATE_PRA_T *p_aec_update)
{
   // int ret=0;
   // uint32_t tmp=0;
   ar_func_enter();
   if(!p_obj||!p_aec_update)
   {
	 ar_err("input pra error sensor_device=0x%p",p_obj);
	 return CAM_ERR_INPUT_NULL_POINT;
   }
    STRU_SENSOR_AEC_UPDATE_T *update=&p_aec_update->update;
	ar_debug("gain=%d line=%d",float2int(update->gain),update->line_count);
	sensor_group_on(p_obj);
	STRU_SENSOR_RES_INFO_T *current_setting=&g_sensor_res_info[p_obj->current_res_index];
	int hdr_mode=p_obj->hdr_mode;
	if(current_setting->sensor_hdr)
	{
  #if 1
		if(hdr_mode==SENSOR_HDR_ON)
		{
			set_real_gain_hdr(p_obj,update->gain,update->exp_ration_gain);
			set_exp_hdr(p_obj,update->line_count,update->exp_ration_time);
		}else
		{
			set_real_gain(p_obj,update->gain);
			set_exp_hdr_off(p_obj,update->line_count,update->exp_ration_time);
		}
 #endif
	}else{
	   set_real_gain(p_obj,update->gain);
	   set_exp(p_obj,update->line_count);
	}
	sensor_group_off(p_obj);
	return 0;
}

static void *event_loop_thread(void* arg)
{
   STRU_SENSOR_OBJ_T *p_event_pbj=(STRU_SENSOR_OBJ_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_event_pbj->vin_fd,p_opt);
   int event_data_size=p_opt->event_data_max_size;
   int fd_mem=ar_hal_vin_open_dev_mem();
   p_event_pbj->event_data_size=event_data_size;
   p_event_pbj->fd_mem=fd_mem;
   while(1)
   {
        ar_hal_vin_get_sensor_cb_event(p_event_pbj->pipe_fd,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_sensor_cb_event(p_event_pbj->pipe_fd,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
			case SENSOR_CB_EVENT_POWER_ON:
			{
				 sensor_power_on(p_event_pbj);
			}
			break;
			case SENSOR_CB_EVENT_POWER_OFF:
			{
				sensor_power_off(p_event_pbj);
			}
			break;
			case SENSOR_CB_EVENT_SENSOR_INIT:
			{
				 ar_always("SENSOR_CB_EVENT_SENSOR_INIT");
			}
			break;
			case SENSOR_CB_EVENT_CFG_RES:
			{
				 STRU_SENSOR_CB_EVENT_CFG_RES_PRA_T *res=(STRU_SENSOR_CB_EVENT_CFG_RES_PRA_T *)event_data;
				 if(res->w!=g_sensor_res_info[p_event_pbj->current_res_index].width || res->h!=g_sensor_res_info[p_event_pbj->current_res_index].height)
				 {
				    ar_err("err cfg res");
				 }else
				 {
				    ar_always("cfg res ...............");
                    for(int i=0;i<sizeof(g_res_2lane_25fps)/sizeof(g_res_2lane_25fps[0]);i++)
                    {
                       sensor_write_16_8(p_event_pbj,g_res_2lane_25fps[i].address,g_res_2lane_25fps[i].data);
                    }
				 }
			}
			break;
			case SENSOR_CB_EVENT_STREAM_ON:
			{
				for(int i=0;i<sizeof(g_stream_on)/sizeof(g_stream_on[0]);i++)
				{
				   sensor_write_16_8(p_event_pbj,g_stream_on[i].address,g_stream_on[i].data);
				}

			}
			break;
			case SENSOR_CB_EVENT_TRIGGER_ON:
			{
				for(int i=0;i<sizeof(g_stream_on)/sizeof(g_stream_on[0]);i++)
				{
				   sensor_write_16_8(p_event_pbj,g_stream_on[i].address,g_stream_on[i].data);
				}
			}
			break;
			case SENSOR_CB_EVENT_STREAM_OFF:
			{
				for(int i=0;i<sizeof(g_stream_off)/sizeof(g_stream_off[0]);i++)
				{
				   sensor_write_16_8(p_event_pbj,g_stream_off[i].address,g_stream_off[i].data);
				}
			}
			break;
			case SENSOR_CB_EVENT_AEC_UPDATE:
			{
				 STRU_SENSOR_CB_EVENT_AEC_UPDATE_PRA_T *p_aec_update=(STRU_SENSOR_CB_EVENT_AEC_UPDATE_PRA_T *)event_data;
				 sensor_aec_update(p_event_pbj,p_aec_update);
			}
			break;
			case SENSOR_CB_EVENT_FLIP_MIRROR:
			{
				  STRU_SENSOR_CB_EVENT_FLIP_MIRROR_PRA_T *filp_mirror=(STRU_SENSOR_CB_EVENT_FLIP_MIRROR_PRA_T *)event_data;
				  ar_always("filp_mirror=%d",filp_mirror->flip_mirror);
                  switch(filp_mirror->flip_mirror)
				  {
				      case FLIP_OFF_MIRROR_OFF:
					  {
					  	 	for(int i=0;i<sizeof(g_flip_off_mirro_off)/sizeof(g_flip_off_mirro_off[0]);i++)
             				{
             				   sensor_write_16_8(p_event_pbj,g_flip_off_mirro_off[i].address,g_flip_off_mirro_off[i].data);
             				}
					  }
					  break;
                      case FLIP_OFF_MIRROR_ON:
					  {
					  	    for(int i=0;i<sizeof(g_flip_off_mirro_on)/sizeof(g_flip_off_mirro_on[0]);i++)
             				{
             				   sensor_write_16_8(p_event_pbj,g_flip_off_mirro_on[i].address,g_flip_off_mirro_on[i].data);
             				}
					  }
					  break;
                      case FLIP_ON_MIRROR_OFF:
					  {
					  	   for(int i=0;i<sizeof(g_flip_on_mirro_off)/sizeof(g_flip_on_mirro_off[0]);i++)
             			   {
             				   sensor_write_16_8(p_event_pbj,g_flip_on_mirro_off[i].address,g_flip_on_mirro_off[i].data);
             			   }
					  }
					  break;
                      case FLIP_ON_MIRROR_ON:
					  {
					  	   for(int i=0;i<sizeof(g_flip_on_mirro_on)/sizeof(g_flip_on_mirro_on[0]);i++)
             			   {
             				   sensor_write_16_8(p_event_pbj,g_flip_on_mirro_on[i].address,g_flip_on_mirro_on[i].data);
             			   }
					  }
					  break;
					  default:
					  break;
                  }
			}
			break;
            case SENSOR_CB_EVENT_CTL_SET_LED:
			{
			}
			break;
            case SENSOR_CB_EVENT_CTL_SET_IR_CUTTER:
			{
			}
			break;
            case SENSOR_CB_EVENT_CTL_SET_HDR:
			{
				 STRU_SENSOR_CB_EVENT_CTL_SET_HDR_PRAT_T *hdr_pra=(STRU_SENSOR_CB_EVENT_CTL_SET_HDR_PRAT_T *)event_data;
				 ar_always("SENSOR_CB_EVENT_CTL_SET_HDR hdr_mode=%d",hdr_pra->status);
				 p_event_pbj->hdr_mode=hdr_pra->status;
			}
			break;
            case SENSOR_CB_EVENT_CTL_ERR_PROC:
			{
			}
			break;
            case SENSOR_CB_EVENT_VSYNC:
			{
			}
			break;
			case SENSOR_CB_EVENT_CLT:
			{
				 STRU_SENSOR_CB_EVENT_CTL_PRA_T *p_ctl=(STRU_SENSOR_CB_EVENT_CTL_PRA_T *)event_data;
				 sensor_clt(p_event_pbj,p_ctl);
			}
			break;
			case SENSOR_CB_EVENT_PRIV_CLT:
			{
				 ar_always("SENSOR_CB_EVENT_PRIV_CLT");
			}
			break;
		    default:
		    break;
		}
		ar_hal_vin_release_sensor_cb_event(p_event_pbj->pipe_fd,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}
static int init(STRU_SENSOR_OBJ_T *p_obj,int vin_fd,int fd_pipe,int pipe_id,int res_index)
{
    p_obj->vin_fd=vin_fd;
	p_obj->pipe_fd=fd_pipe;
	p_obj->pipe_id=pipe_id;

	p_obj->i2c_fd=ar_hal_i2c_open(g_slave_info.i2c_component_index);
	p_obj->slave_addr=g_slave_info.slave_address_7bit;

	ar_always("enter ,start probe sensor");
	 if(!probe_sensor(p_obj))
	 {
		ar_err("sensor probe err");
		return -1;
	 }

    //setup sensor info
	STRU_DEV_INFO_VAR_T *p_var_info=malloc(sizeof(STRU_DEV_INFO_VAR_T));
    p_var_info->slave_infor=g_slave_info;
	p_var_info->out_infor=g_out_info;
	p_var_info->res_info=g_sensor_res_info[res_index];
	p_var_info->res_count=1;
	p_var_info->res_index=0;
	ar_hal_vin_set_sensor_var_pra(fd_pipe,pipe_id,p_var_info);

    p_obj->frame_len_new=g_sensor_res_info[res_index].frame_length;
	p_obj->current_res_index=res_index;
	//register sensor cb
	ar_hal_vin_create_sensor_cb_event_client(fd_pipe,pipe_id,&p_obj->event_client_id);

	ar_always("event_client_id=%d",p_obj->event_client_id);
	ar_os_thread_attr_t ctrl_attr = {0};
	ctrl_attr.name = "imx307";
	ctrl_attr.stack_size = 16 * 1024;
	p_obj->event_thread=ar_osal_thread_new(event_loop_thread,p_obj, &ctrl_attr);
	//after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
	for(int i=SENSOR_CB_EVENT_POWER_ON;i<SENSOR_CB_EVENT_MAX;i++)
	{
		ar_hal_vin_register_sensor_cb_event(fd_pipe,pipe_id,p_obj->event_client_id,i);
	}
	return 0;
}

static int deinit(STRU_SENSOR_OBJ_T *p_obj)
{
	//unregister all the event
	ar_always("unregister all the event");
	for(int i=SENSOR_CB_EVENT_POWER_ON;i<SENSOR_CB_EVENT_MAX;i++)
	{
		ar_hal_vin_unregister_sensor_cb_event(p_obj->pipe_fd,p_obj->pipe_id,p_obj->event_client_id,i);
	}
	ar_osal_thread_join(p_obj->event_thread);
	ar_hal_vin_delete_sensor_cb_event_client(p_obj->pipe_fd,p_obj->pipe_id,p_obj->event_client_id);
    return 0;
}

STRU_SENSOR_OBJ_T imx307_obj=
{
	.name="imx307",
	.init=init,
	.deinit=deinit,
};
