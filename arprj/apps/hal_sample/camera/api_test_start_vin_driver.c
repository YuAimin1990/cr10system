#include"camera.h"

static const STRU_SENSOR_SLOT_T slot_group[]=
{
{
   .slot_name="slot0",
   .slot_type=0,
   .mipi_index=0,
   .i2c_index=1,
   .mclk_index=0,

   #ifdef USE_EVB
   //pad fun group port num inout
   .reset_gpio={38,2,1,2,1,1},
   .power_gpio={39,2,1,2,0,1},
   #elif defined(USE_EVB_EXT)
   //pad fun group port num inout
   .reset_gpio={148,0,2,3,0,1},
   .power_gpio={145,0,3,3,5,1},
   #else
   //pad fun group port num inout
   .power_gpio={167,0,3,1,4,1},
   .reset_gpio={169,0,3,1,6,1},
   #endif
},
{
   .slot_name="slot1",
   .slot_type=0,
   .mipi_index=1,
   .mclk_index=1,
   .i2c_index=2,

   #ifdef USE_EVB
   //.power_gpio={40,2,1,2,2,1},
   .power_gpio={39,2,1,2,0,1},
   .reset_gpio={41,2,1,2,3,1},
   #elif defined(USE_EVB_EXT)
   .power_gpio={147,0,3,3,7,1},
   .reset_gpio={146,0,3,3,6,1},
   #else
   .power_gpio={170,0,3,1,7,1},
   .reset_gpio={171,0,0,3,5,1},
   #endif
},
{
   .slot_name="slot2",
   .slot_type=0,
   .mipi_index=2,
   .mclk_index=2,
   .i2c_index=3,

   #ifdef USE_EVB
   .power_gpio={46,2,1,3,0,1},
   .reset_gpio={47,2,1,3,1,1},
   #elif defined(USE_EVB_EXT)
   .power_gpio={171,0,0,3,5,1},
   .reset_gpio={166,0,3,1,3,1},
   #else
   #endif
},
{
   .slot_name="slot3",
   .slot_type=0,
   .mipi_index=3,
   .mclk_index=3,
   .i2c_index=0,

   #ifdef USE_EVB
   //.power_gpio={48,2,1,3,2,1},
   .power_gpio={46,2,1,3,0,1},
   .reset_gpio={49,2,1,3,3,1},
   #elif defined(USE_EVB_EXT)
   .power_gpio={165,0,3,1,2,1},
   .reset_gpio={140,5,3,1,2,1},
   #else
   #endif
},
#ifdef USE_EVB_EXT
{
    .slot_name = "slot_tp9930_0",
    .slot_type = 1,     // SLOT_DVP
    .mipi_index = 0,
    .mclk_index = 0,
    .i2c_index = 3,
    .reset_gpio = {143, 5, 3, 1, 5, 1},
},
{
    .slot_name = "slot_tp9930_1",
    .slot_type = 1,     // SLOT_DVP
    .mipi_index = 1,
    .mclk_index = 0,
    .i2c_index = 3,
    .reset_gpio = {143, 5, 3, 1, 5, 1},
},
{
    .slot_name = "slot_tp9930_2",
    .slot_type = 1,     // SLOT_DVP
    .mipi_index = 2,
    .mclk_index = 0,
    .i2c_index = 3,
    .reset_gpio = {142, 5, 3, 1, 4, 1},
},
{
    .slot_name = "slot_tp9930_3",
    .slot_type = 1,     // SLOT_DVP
    .mipi_index = 3,
    .mclk_index = 0,
    .i2c_index = 3,
    .reset_gpio = {142, 5, 3, 1, 4, 1},
},
#endif
{
   .slot_name="slot_common",
   .slot_type=0,
   .mipi_index=0,
   .mclk_index=0,
   .i2c_index=256,
   .reset_gpio={0,0,0,0,0,0},
   .power_gpio={0,0,0,0,0,0},
},
};


static STRU_SENSOR_DEV_T sensor_imx307_0=
{
.dev_name="cam_imx307_0",
.driver_name="cam_imx307",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot0",
},
};

static STRU_SENSOR_DEV_T sensor_imx307_1=
{
.dev_name="cam_imx307_1",
.driver_name="cam_imx307",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot1",
},
};
static STRU_SENSOR_DEV_T sensor_imx307_2=
{
.dev_name="cam_imx307_2",
.driver_name="cam_imx307",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot2",
},
};

static STRU_SENSOR_DEV_T sensor_imx307_3=
{
.dev_name="cam_imx307_3",
.driver_name="cam_imx307",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot3",
},
};

static STRU_SENSOR_DEV_T sensor_imx415_0=
{
.dev_name="cam_imx415_0",
.driver_name="cam_imx415",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=4,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot0",
},
};

static STRU_SENSOR_DEV_T sensor_imx415_1=
{
.dev_name="cam_imx415_1",
.driver_name="cam_imx415",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=4,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot2",
},
};

static STRU_SENSOR_DEV_T sensor_imx464_0=
{
.dev_name="cam_imx464_0",
.driver_name="cam_imx464",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=4,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot0",
},
};


static STRU_SENSOR_DEV_T vin_dev_0=
{
.dev_name="vin_dev_0",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_1=
{
.dev_name="vin_dev_1",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_2=
{
.dev_name="vin_dev_2",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_3=
{
.dev_name="vin_dev_3",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_4=
{
.dev_name="vin_dev_4",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_5=
{
.dev_name="vin_dev_5",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_6=
{
.dev_name="vin_dev_6",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T vin_dev_7=
{
.dev_name="vin_dev_7",
.driver_name="common_sensor",
.actuator_name="common_actuator",
.sensor_slave_addr_7bits=0x34>>1,
.e_mclk_khz=SENSOR_MCLK_37_125M,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_COMMON,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T  isp_vin_0=
{
.dev_name="isp_vin_0",
.driver_name="isp_vin",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_ISPIN,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T  isp_vin_1=
{
.dev_name="isp_vin_1",
.driver_name="isp_vin",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_ISPIN,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T  isp_vin_2=
{
.dev_name="isp_vin_2",
.driver_name="isp_vin",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_ISPIN,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};

static STRU_SENSOR_DEV_T  isp_vin_3=
{
.dev_name="isp_vin_3",
.driver_name="isp_vin",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x34>>1,
.mipi_lane_count=2,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_ISPIN,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
   .slot_name="slot_common",
},
};


static STRU_SENSOR_DEV_T sensor_hdmi_video_dvp_in=
{
.dev_name="hdmi_video_dvp_in",
.driver_name="hdmi_video_dvp_in",
.actuator_name="\0",
.sensor_slave_addr_7bits=0x92>>1,
.e_mclk_khz=SENSOR_MCLK_24M,
.mipi_lane_count=0,
.need_master_slave_sel=0,
.device_mode=DEVICE_MODE_MASTER,
.device_type=DEVICE_TYPE_NORMAL,
.slave_num=0,
.master_sensor="\0",
.slave_sensor=
{
    "\0",
    "\0",
    "\0",
    "\0",
},
.slot=
{
    .slot_name="slot_common",
},
};



static STRU_SENSOR_DEV_T sensor_tp9930_0 = {
    .dev_name = "cam_tp9930_0",
    .driver_name = "cam_tp9930",
    .actuator_name = "\0",
    .sensor_slave_addr_7bits = (0x8A >> 1),
    .e_mclk_khz = SENSOR_MCLK_27M,
    .mipi_lane_count = 0,
    .need_master_slave_sel = 0,
    .device_mode = DEVICE_MODE_MASTER,
    .device_type = DEVICE_TYPE_NORMAL,
    .slave_num = 0,
    .master_sensor = "\0",
    .slave_sensor = {"\0", "\0", "\0", "\0",},
    .slot= {
        .slot_name = "slot_tp9930_0",
    },
};

static STRU_SENSOR_DEV_T sensor_tp9930_1 = {
    .dev_name = "cam_tp9930_1",
    .driver_name = "cam_tp9930",
    .actuator_name = "\0",
    .sensor_slave_addr_7bits = (0x8A >> 1),
    .e_mclk_khz = SENSOR_MCLK_27M,
    .mipi_lane_count = 0,
    .need_master_slave_sel = 0,
    .device_mode = DEVICE_MODE_MASTER,
    .device_type = DEVICE_TYPE_NORMAL,
    .slave_num = 0,
    .master_sensor = "\0",
    .slave_sensor = {"\0", "\0", "\0", "\0",},
    .slot= {
        .slot_name = "slot_tp9930_1",
    },
};

static STRU_SENSOR_DEV_T sensor_tp9930_2 = {
    .dev_name = "cam_tp9930_2",
    .driver_name = "cam_tp9930",
    .actuator_name = "\0",
    .sensor_slave_addr_7bits = (0x88 >> 1),
    .e_mclk_khz = SENSOR_MCLK_27M,
    .mipi_lane_count = 0,
    .need_master_slave_sel = 0,
    .device_mode = DEVICE_MODE_MASTER,
    .device_type = DEVICE_TYPE_NORMAL,
    .slave_num = 0,
    .master_sensor = "\0",
    .slave_sensor= {"\0", "\0", "\0", "\0",},
    .slot= {
        .slot_name = "slot_tp9930_2",
    },
};

static STRU_SENSOR_DEV_T sensor_tp9930_3 = {
    .dev_name = "cam_tp9930_3",
    .driver_name = "cam_tp9930",
    .actuator_name = "\0",
    .sensor_slave_addr_7bits = (0x88 >> 1),
    .e_mclk_khz = SENSOR_MCLK_27M,
    .mipi_lane_count = 0,
    .need_master_slave_sel = 0,
    .device_mode = DEVICE_MODE_MASTER,
    .device_type = DEVICE_TYPE_NORMAL,
    .slave_num = 0,
    .master_sensor = "\0",
    .slave_sensor= {"\0", "\0", "\0", "\0",},
    .slot= {
        .slot_name = "slot_tp9930_3",
    },
};


static STRU_ACTUATOR_DEV_T 	g_actuator_dev[]=
{
	{
		.dev_name="common_actuator",
		.driver_name="common_actuator",
		.i2c_index=0,
	},

};
static STRU_SENSOR_DEV_T *g_p_sensor_dev_group_hal[]={&sensor_imx307_0,&sensor_imx307_1,&sensor_imx307_2,&sensor_imx307_3,
                                                      &sensor_imx415_0,&sensor_imx415_1,
                                                      &sensor_imx415_0,&sensor_imx415_1,&sensor_imx464_0,
                                                      &sensor_tp9930_0, &sensor_tp9930_1, &sensor_tp9930_2, &sensor_tp9930_3,
                                                      &sensor_hdmi_video_dvp_in,
                                                      &isp_vin_0,&vin_dev_0};

//static STRU_SENSOR_DEV_T *g_p_sensor_dev_group_hal[]={&sensor_imx464_0,&isp_vin_0,&vin_dev_0};



static STRU_SENSOR_DEV_T *g_p_sensor_dev_group_common[] = {
    &vin_dev_0,
    &vin_dev_1,
    &vin_dev_2,
    &vin_dev_3,
    &vin_dev_4,
    &vin_dev_5,
    &vin_dev_6,
    &vin_dev_7,
    &isp_vin_0,
    &isp_vin_1,
    &isp_vin_2,
    &isp_vin_3,
};

static int register_dev(int vin_fd, int is_for_mpi, int max_comm_sns_num, int max_isp_vin_num)
{
    STRU_SENSOR_DEV_T *dev=NULL;
	int find=0;

	STRU_SENSOR_DEV_T **p_sensor_dev_group_hal=NULL;
	int dev_group_num=0;
	if(is_for_mpi)
	{
	    p_sensor_dev_group_hal=g_p_sensor_dev_group_common;
		dev_group_num=sizeof(g_p_sensor_dev_group_common)/sizeof(STRU_SENSOR_DEV_T *);

        for(int i=0;i<max_comm_sns_num;i++)
    	{
    	   dev=p_sensor_dev_group_hal[i];
    	   for(int j=0;j<sizeof(slot_group)/sizeof(STRU_SENSOR_SLOT_T);j++)
    	   {
    	      if(!strcmp(slot_group[j].slot_name,dev->slot.slot_name))
    	      {
    	          dev->slot=slot_group[j];
    			  find=1;
    			  break;
    	      }
    	   }
    	   if(find){
         	   ar_always("register %s",dev->dev_name);
         	   ar_hal_vin_register_sensor_dev(vin_fd,dev);
    	   }else
    	   {
    	      ar_err("slot not find");
    		  return -1;
    	   }
    	}

        for(int i=8;i<8+max_isp_vin_num;i++)
    	{
    	   dev=p_sensor_dev_group_hal[i];
    	   for(int j=0;j<sizeof(slot_group)/sizeof(STRU_SENSOR_SLOT_T);j++)
    	   {
    	      if(!strcmp(slot_group[j].slot_name,dev->slot.slot_name))
    	      {
    	          dev->slot=slot_group[j];
    			  find=1;
    			  break;
    	      }
    	   }
    	   if(find){
         	   ar_always("register %s",dev->dev_name);
         	   ar_hal_vin_register_sensor_dev(vin_fd,dev);
    	   }else
    	   {
    	      ar_err("slot not find");
    		  return -1;
    	   }
    	}
	}else
	{
	   p_sensor_dev_group_hal=g_p_sensor_dev_group_hal;
	   dev_group_num=sizeof(g_p_sensor_dev_group_hal)/sizeof(STRU_SENSOR_DEV_T *);

        for(int i=0;i<dev_group_num;i++)
    	{
    	   dev=p_sensor_dev_group_hal[i];
    	   for(int j=0;j<sizeof(slot_group)/sizeof(STRU_SENSOR_SLOT_T);j++)
    	   {
    	      if(!strcmp(slot_group[j].slot_name,dev->slot.slot_name))
    	      {
    	          dev->slot=slot_group[j];
    			  find=1;
    			  break;
    	      }
    	   }
    	   if(find){
         	   ar_always("register %s",dev->dev_name);
         	   ar_hal_vin_register_sensor_dev(vin_fd,dev);
    	   }else
    	   {
    	      ar_err("slot not find");
    		  return -1;
    	   }
    	}
	}

    //register actuator
    STRU_ACTUATOR_DEV_T *actuator_dev=NULL;
	for(int i=0;i<sizeof(g_actuator_dev)/sizeof(STRU_ACTUATOR_DEV_T);i++)
	{
	    actuator_dev=&g_actuator_dev[i];
		ar_always("register %s",actuator_dev->dev_name);
     	ar_hal_vin_register_actuator_dev(vin_fd,actuator_dev);
	}
	return 0;
}


static int api_test_start_vin_driver(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int ret=0;
   int max_comm_sns_num = 5;
   int max_isp_vin_num = 0;
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);

   if(ar_hal_sys_mpp_service_is_lib())
	  return 0;

   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {

	     int vin_fd=ar_hal_vin_open_vin_dev();
         int count=0;
         while(vin_fd<0)
         {
            usleep(1000);
            vin_fd=ar_hal_vin_open_vin_dev();
			if(count++>5000) //wait 5s
			{
				ar_always("wait the mpp server start time out");
			    return -1;
			}
         };

         //get vin start opt
         STRU_VIN_DRIVER_START_OPT_T * p_opt=malloc(sizeof(STRU_VIN_DRIVER_START_OPT_T));

         if(!p_opt)
         {
             ar_hal_vin_close_vin_dev(vin_fd);
             return -1;
         }
		 
		 ar_always("get vin drvier start opt");
         ar_hal_vin_driver_get_start_opt(vin_fd,p_opt);
         ar_always("max_dev_num= %d ##############################",p_opt->max_dev_num);

		 index=camera_find_cmd_pra_index_by_name("-max_comm_sns_num",p_cmd);
		 if(index>0)
		 {
			max_comm_sns_num=atoi(p_cmd->args[index+1]);
			ar_always("max_comm_sns_num = %d ##############################",max_comm_sns_num);
		 }

		 index=camera_find_cmd_pra_index_by_name("-max_isp_vin_num",p_cmd);
		 if(index>0)
		 {
			max_isp_vin_num=atoi(p_cmd->args[index+1]);
			ar_always("max_isp_vin_num = %d ##############################",max_isp_vin_num);
		 }
         //set the max vin always 0;
         max_isp_vin_num=0;
         p_opt->max_dev_num = max_comm_sns_num + max_isp_vin_num;

		 p_opt->visual_isp_count=p_opt->max_dev_num;
		 index = camera_find_cmd_pra_index_by_name("-visual_isp_count", p_cmd);
		 if(index>0)
		 {
             p_opt->visual_isp_count = atoi(p_cmd->args[index+1]);
		 }

		
		 index=camera_find_cmd_pra_index_by_name("-hw_burst",p_cmd);
		 if(index>0)
		 {
			p_opt->ddr_rw_burst_len=atoi(p_cmd->args[index+1]);
		 }

		 index=camera_find_cmd_pra_index_by_name("-isp_out_hw_burst",p_cmd);
		 if(index>0)
		 {
			p_opt->isp_out_hw_burst=atoi(p_cmd->args[index+1]);
		 }
		 index=camera_find_cmd_pra_index_by_name("-share_stats_buffer",p_cmd);
		 if(index>0)
		 {
			p_opt->share_stats_buffer=atoi(p_cmd->args[index+1]);
		 }
		 index=camera_find_cmd_pra_index_by_name("-max_wh",p_cmd);
		 if(index>0)
		 {
			p_opt->max_w=atoi(p_cmd->args[index+1]);
			p_opt->max_h=atoi(p_cmd->args[index+2]);
			ar_always("max_w %d max_w %d",p_opt->max_w,p_opt->max_h);
		 }

		 index = camera_find_cmd_pra_index_by_name("-en_mem_over_dbg", p_cmd);
		 if(index>0)
		 {
             p_opt->mem_over_debug_en = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-nr3d_use_pingpong", p_cmd);
		 if(index>0)
		 {
             p_opt->nr3d_use_ping_pong_buf = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-nr3d_md_use_ping_pong_buf", p_cmd);
		 if(index>0)
		 {
             p_opt->nr3d_md_use_ping_pong_buf = atoi(p_cmd->args[index+1]);
		 }

		 index = camera_find_cmd_pra_index_by_name("-tuning_req_pool_count", p_cmd);
		 if(index>0)
		 {
             p_opt->tuning_req_pool_count = atoi(p_cmd->args[index+1]);
		 } 
		 index = camera_find_cmd_pra_index_by_name("-ltm_max_mesh_w", p_cmd);
		 if(index>0)
		 {
             p_opt->ltm_max_mesh_w = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-ltm_max_mesh_h", p_cmd);
		 if(index>0)
		 {
             p_opt->ltm_max_mesh_h = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-ltm_lut_pool_count", p_cmd);
		 if(index>0)
		 {
             p_opt->ltm_lut_pool_count = atoi(p_cmd->args[index+1]);
		 }

		 index = camera_find_cmd_pra_index_by_name("-max_stream_buffer_count", p_cmd);
		 if(index>0)
		 {
             p_opt->max_stream_buffer_count = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-vsync_update_reg", p_cmd);
		 if(index>0)
		 {
             p_opt->vsync_update_reg = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-wait_all_isp_done", p_cmd);
		 if(index>0)
		 {
             p_opt->wait_all_isp_done = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-en_invalide_hw_addr", p_cmd);
		 if(index>0)
		 {
             p_opt->en_invalide_hw_addr = atoi(p_cmd->args[index+1]);
		 } 
		 index = camera_find_cmd_pra_index_by_name("-use_stats_pingpong", p_cmd);
		 if(index>0)
		 {
             p_opt->use_stats_pingpong = atoi(p_cmd->args[index+1]);
		 }	 
		 index = camera_find_cmd_pra_index_by_name("-max_stream_per_dev", p_cmd);
		 if(index>0)
		 {
             p_opt->max_stream_per_dev = atoi(p_cmd->args[index+1]);
		 }
		 index = camera_find_cmd_pra_index_by_name("-hardware_mask", p_cmd);
		 if(index>0)
		 {
             p_opt->hardware_mask = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-filter_mask", p_cmd);
		 if(index>0)
		 {
             p_opt->filter_mask = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-isp_use_loop_mem", p_cmd);
		 if(index>0)
		 {
             p_opt->isp_use_loop_mem = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-isp_use_done_cfg_buffer", p_cmd);
		 if(index>0)
		 {
             p_opt->isp_use_done_cfg_buffer = strtoul(p_cmd->args[index+1],NULL,0);
		 }

		 index = camera_find_cmd_pra_index_by_name("-vif_out_mask", p_cmd);
		 if(index>0)
		 {
             p_opt->vif_out_mask = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-enable_mipi_irq_only_once", p_cmd);
		 if(index>0)
		 {
             p_opt->enable_mipi_irq_only_once = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-max_pipe_num", p_cmd);
		 if(index>0)
		 {
             p_opt->max_pipe_num = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-max_ch_num", p_cmd);
		 if(index>0)
		 {
             p_opt->max_ch_num = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-max_event_client", p_cmd);
		 if(index>0)
		 {
             p_opt->max_event_client = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-use_vsync_trigger_aec", p_cmd);
		 if(index>0)
		 {
             p_opt->use_vsync_trigger_aec = strtoul(p_cmd->args[index+1],NULL,0);
		 }
		 index = camera_find_cmd_pra_index_by_name("-use_isp_irq_optimization", p_cmd);
		 if(index>0)
		 {
             p_opt->use_isp_irq_optimization = strtoul(p_cmd->args[index+1],NULL,0);
		 }
         index = camera_find_cmd_pra_index_by_name("-disable_seq_queue", p_cmd);
         if(index > 0)
         {
             p_opt->disable_seq_queue = strtoul(p_cmd->args[index+1],NULL,0);
         }
		 index = camera_find_cmd_pra_index_by_name("-use_vsync_3a_clc", p_cmd);
         if(index > 0)
         {
             p_opt->use_vsync_3a_clc = strtoul(p_cmd->args[index+1],NULL,0);
         }

		 index = camera_find_cmd_pra_index_by_name("-use_dma_upate_reg", p_cmd);
         if(index > 0)
         {
             p_opt->use_dma_upate_reg = strtoul(p_cmd->args[index+1],NULL,0);
         }
		 
         index = camera_find_cmd_pra_index_by_name("-ir_algo_split_thread", p_cmd);
         if(index > 0)
         {
             p_opt->ir_algo_split_thread = strtoul(p_cmd->args[index+1],NULL,0);
         }

		 index = camera_find_cmd_pra_index_by_name("-mipi_out_mask", p_cmd);
         if(index > 0)
         {
             p_opt->mipi_out_mask = strtoul(p_cmd->args[index+1],NULL,0);
         }
		 index = camera_find_cmd_pra_index_by_name("-vif_use_ddr_done_irq", p_cmd);
         if(index > 0)
         {
             p_opt->vif_use_ddr_done_irq = strtoul(p_cmd->args[index+1],NULL,0);
         }

		 
	 ar_always("hw burst is %d isp_out_hw_burst=%d share_stats_buffer=%d",p_opt->ddr_rw_burst_len,p_opt->isp_out_hw_burst,p_opt->share_stats_buffer);
	 p_opt->vif_use_video_stable = 1;
	 index = camera_find_cmd_pra_index_by_name("-vif_use_video_stable", p_cmd);
	 if(index>0)
	 {
     		p_opt->vif_use_video_stable = strtoul(p_cmd->args[index+1],NULL,16);
	 }
	
	 index = camera_find_cmd_pra_index_by_name("-use_vsync_trigger_aec", p_cmd);
	 if(index>0)
	 {
     		p_opt->use_vsync_trigger_aec = strtoul(p_cmd->args[index+1],NULL,16);
	 } 
	 index = camera_find_cmd_pra_index_by_name("-use_mipi_only_skew_int", p_cmd);
	 if(index>0)
	 {
     		p_opt->use_mipi_only_skew_int =strtoul(p_cmd->args[index+1],NULL,0);
	 } 
	 ar_always("hw burst is %d isp_out_hw_burst=%d share_stats_buffer=%d",p_opt->ddr_rw_burst_len,p_opt->isp_out_hw_burst,p_opt->share_stats_buffer);
         ar_hal_vin_driver_set_start_opt(vin_fd,p_opt);
		 ar_always("start registger sensor dev,set the sensor detect mode the by opt");
         ar_hal_vin_driver_set_start_opt(vin_fd,p_opt);

         int is_for_mpi=0;
         index=camera_find_cmd_pra_index_by_name("-mpi",p_cmd);
		 if(index>0)
		 {
			 is_for_mpi=1;
		 }

		 ret=register_dev(vin_fd,is_for_mpi,max_comm_sns_num,max_isp_vin_num);
		 if(ret<0)
		 {
		    ar_err("register dev failed");
			ar_hal_vin_close_vin_dev(vin_fd);
			ar_free(p_opt);
			return -1;
		 }

         ret=ar_hal_vin_driver_get_start_opt(vin_fd,p_opt);
		 ar_always("register device cimpleted, dump the register dev");

		 for(int i=0;i<p_opt->sensor_dev_count;i++)
		 {
		     ar_always("i=%d ...........................................:",i);
			 ar_always("flag=%d",p_opt->sensor_dev[i].flag);
			 ar_always("dev_name=%s",p_opt->sensor_dev[i].sensor_dev.dev_name);
			 ar_always("driver_name=%s",p_opt->sensor_dev[i].sensor_dev.driver_name);
			 ar_always("actuator_name=%s",p_opt->sensor_dev[i].sensor_dev.actuator_name);
			 ar_always("sensor_slave_addr_7bits=%d",p_opt->sensor_dev[i].sensor_dev.sensor_slave_addr_7bits);
			 ar_always("mipi_lane_count=%d",p_opt->sensor_dev[i].sensor_dev.mipi_lane_count);
			 ar_always("need_master_slave_sel=%d",p_opt->sensor_dev[i].sensor_dev.need_master_slave_sel);
			 ar_always("device_mode=%d",p_opt->sensor_dev[i].sensor_dev.device_mode);
			 ar_always("device_type=%d",p_opt->sensor_dev[i].sensor_dev.device_type);
			 ar_always("slave_num=%d",p_opt->sensor_dev[i].sensor_dev.slave_num);
			 ar_always("master_sensor=%s",p_opt->sensor_dev[i].sensor_dev.master_sensor);
			 for(int j=0;j<p_opt->sensor_dev[i].sensor_dev.slave_num;j++)
			 {
				 ar_always("slave_sensor[%d]=%s",j,p_opt->sensor_dev[i].sensor_dev.slave_sensor[j]);
			 }
			 ar_always("slot_name=%s",p_opt->sensor_dev[i].sensor_dev.slot.slot_name);
			 ar_always("slot_type=%d",p_opt->sensor_dev[i].sensor_dev.slot.slot_type);
			 ar_always("mipi_index=%d",p_opt->sensor_dev[i].sensor_dev.slot.mipi_index);
			 ar_always("i2c_index=%d",p_opt->sensor_dev[i].sensor_dev.slot.i2c_index);
			 for(int j=0;j<6;j++)
			 {
				 ar_always("reset_gpio[%d]=%d",j,p_opt->sensor_dev[i].sensor_dev.slot.reset_gpio[j]);
			 }
			 for(int j=0;j<6;j++)
			 {
				 ar_always("power_gpio[%d]=%d",j,p_opt->sensor_dev[i].sensor_dev.slot.power_gpio[j]);
			 }
		 }

         int load_driver=1;
		 
		 index=camera_find_cmd_pra_index_by_name("-load_driver",p_cmd);
		 if(index >0)
		 {
		   load_driver= atoi(p_cmd->args[index+1]);
		 }
         if(load_driver)
		 {
     		 ar_always("ar_hal_vin_start_vin_driver  =============================================== ");
             ar_hal_vin_start_vin_driver(vin_fd);
     		 AR_S32  dev_num=0;
     		 ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
     		 if(ret<0)
     		 {
     		    ar_err("get dev num err");
     			return -1;
     		 }
     		 ar_always("dev_num=%d",dev_num);
     		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
			 if(p_info)
			 {
         		 for(int i=0;i<dev_num;i++)
         		 {
         		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
         			ar_always("dev id: %d ",i);
         			//dump_dev_info(p_info);
         		 }
         		 free(p_info);
			 }
         }else
		 {
		 	ar_err("note !!!!!!!!!!!!!!!!!!1 the vin driver not be load,please load it by your app\n");
		 }
		 free(p_opt);
		 ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(start_vin_driver)=
{
   "--start_vin_driver",
   api_test_start_vin_driver,
   {
      "test the api ar_hal_vin_start_vin_driver, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-mpi :use common sensor driver ,when use mpi",
	  "-load_driver [1/0]",
	  "-hw_burst [4/8/16]",
	  "-isp_out_hw_burst [4/8/16]",
	  "-share_stats_buffer [0/1]",
	  "-af_stats [0/1]",
	  "-max_comm_sns_num: set the max common sensor num the system supported",
	  "-max_wh: max input w h",
      "-en_mem_over_dbg [0/1]",      
	  "-nr3d_use_ping_pong_buf [0/1]",
	  "-nr3d_md_use_ping_pong_buf [0/1]",
	  "-tuning_req_pool_count [n]",
	  "-visual_isp_count [n]",
	  "-ltm_max_mesh_w [n]",
	  "-ltm_max_mesh_h [n]",
	  "-ltm_lut_pool_count [n]",
	  "-max_stream_buffer_count [n]",
	  "-vsync_update_reg [0/1]",
	  "-wait_all_isp_done [0/1]",
	  "-en_invalide_hw_addr [0/1]",
	  "-use_stats_pingpong [0/1]",
	  "-max_stream_per_dev [n]",	  
	  "-hardware_mask [0x] eg:0x835 for mem reduce",
	  "    note: bit[0-31], 0:i2c 1:nuc 2:eis_immu 3:sensor 4:dvp 5:mipi 6:vif 7:ceva_hdr 8:ceva_ldc 9:ai_isp 10:stats_sw 11:eis_ldc 12:isp 13:dvp_scaler",
	  "-filter_mask [0x] eg:0x147 for mem reduce",
	  "    note： bit[0-31], 0:module_sensor 1:rx 2:vif 3:vfe: 4:raw_stats 5:dvp_scaler 6:isp 7:img_proc 8:buffer_sink 9:module_ext",
	  "-isp_use_loop_mem [0/1]",	  
	  "-isp_use_done_cfg_buffer [0/1]",	  
	  "-vif_out_mask [mask ,hex format]",
	  "-enable_mipi_irq_only_once  [1/0]",
	  "-max_pipe_num  [n]",
	  "-max_ch_num  [n]",
	  "-max_event_client  [n]",
	  "-use_vsync_trigger_aec [1/0]",
	  "-use_isp_irq_optimization [0/1]",
	  "-disable_seq_queue [0/1]",
	  "-use_vsync_3a_clc [0/1]",
	  "-use_dma_upate_reg [01/]",
	  "-ir_algo_split_thread [0/1]",
	  "-mipi_out_mask [mask]",
	  "vif_use_ddr_done_irq [0/1]",
	  "-use_mipi_only_skew_int [0/1]",	
	  NULL,
	  "-vif_use_video_stable [1/0]",	
 	  NULL,
   },
};
	  
