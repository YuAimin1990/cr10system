#include"camera.h"
#include "hal_gpio.h"

static int camera_test_run_help(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
    char *cmd_item="all";
	int index=camera_find_cmd_pra_index_by_name("--help",p_cmd);
	if(index>0 && p_cmd->args_num==3)
	{
		cmd_item=p_cmd->args[index+1];
	}
	for(int i=0;i< p_camera->test_case_size;i++)
	{
	   if(!strcmp(p_camera->p_test_case[i].p_name,cmd_item)||!strcmp("all",cmd_item) )
	   {
           ar_printf("%s: \n",p_camera->p_test_case[i].p_name);
     	   for(int j=0;j<MAX_HELP_STR_NUM;j++)
     	   {
     	   	   if(p_camera->p_test_case[i].p_help_string[j])
     		   {
                    ar_printf("    %s: \n",p_camera->p_test_case[i].p_help_string[j]);
     	   	   }else
     	   	   {
     	   	       break;
     	   	   }
     	   }
	   }
	}
	return 0;
}

static int camera_test_run_list_cmd(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
	for(int i=0;i< p_camera->test_case_size;i++)
	{
       ar_printf("%s: \n",p_camera->p_test_case[i].p_name);
	}
	return 0;
}


__test_camera_case__(help)=
{
   "--help",
   camera_test_run_help,
   {
      "show this help",
	  "name: eg --help --start_camera",
	  NULL,
   },
};
int camera_find_cmd_pra_index_by_name(char *name,STRU_CAMERA_CMD_T *p_cmd)
{
    int i=0;
    for(i=0;i<p_cmd->args_num;i++)
    {
         if(!strcmp(name,p_cmd->args[i]))
         {
             return i;
         }
    }
    return -1;
}

STRU_CAMERA_CMD_T *camera_construct_cmd(int argc, char *argv[])
{
    STRU_CAMERA_CMD_T *p_cmd=malloc(sizeof(STRU_CAMERA_CMD_T));
	p_cmd->args_num=argc;
    for(int i=0;i<argc;i++)
    {
		strcpy(&p_cmd->args[i][0],argv[i]);
    }
	return p_cmd;
}

int camera_send_cmd_to_remote(STRU_CAMERA_T *p_camera,char *remote_name,STRU_CAMERA_CMD_T *p_cmd)
{
	// unsigned int prio;
    char server_name[64];
	sprintf(server_name,"/%s",remote_name);
	mqd_t mqid;
	mqid=mq_open(server_name,O_CREAT | O_RDWR,0666,NULL);
	if(mqid==(mqd_t)-1)
	{
	   ar_err("start server failed");
	   return -1;
	}
	struct mq_attr attr;
	mq_getattr(mqid,&attr);
	ar_always("mq_maxmsg=%d mq_msgsize=%d name=%s",attr.mq_maxmsg,attr.mq_msgsize,server_name);
	mq_send(mqid,(const char *)p_cmd,sizeof(STRU_CAMERA_CMD_T),1);
	//wait the cmd process completed
	//mq_receive(p_camera->mqid,(char *)p_cmd,sizeof(STRU_CAMERA_CMD_T),&prio);
	mq_close(mqid);
	return 0;
}


void *msg_loop_thread(void* arg)
{
	STRU_CAMERA_T *p_camera=(STRU_CAMERA_T *)arg;
	unsigned int prio;
	STRU_CAMERA_CMD_T *p_cmd=malloc(sizeof(STRU_CAMERA_CMD_T));
    while(1)
    {
       if(mq_receive(p_camera->mqid,(char *)p_cmd,sizeof(STRU_CAMERA_CMD_T),&prio)==(mqd_t)-1)
       {
          ar_err("receive err");
		  perror("mq_receive");
		  break;
       }else
       {
           if(!strcmp(p_cmd->args[0],"--exit_msg_loop"))
           {
                ar_always("exit the msg loop");
                break;
           }
		   ar_always("current test case is %s \n",p_cmd->args[1]);
		   int i=0;
		   for(i=0;i< p_camera->test_case_size;i++)
		   {
			 if(!strcmp(p_cmd->args[1],p_camera->p_test_case[i].p_name))
			 {
				p_camera->p_test_case[i].pfn_camera_test_run(p_camera,p_cmd);
				break;
			 }
		   }
		   if(i>=p_camera->test_case_size)
		   {
			   ar_err("can not find test cmd %s \n",p_cmd->args[1]);
		   }
           //return a ack to client
		   //mq_send(p_camera->mqid,(const char *)p_cmd,sizeof(STRU_CAMERA_CMD_T),1);
       }
    }
	free(p_cmd);
	mq_close(p_camera->mqid);
	ar_osal_thread_exit();
	return NULL;
}


STRU_ELMENT_T *creat_element(char *name,void *usrdata)
{
  STRU_ELMENT_T *p_element=malloc(sizeof(STRU_ELMENT_T));
  memset(p_element,0,sizeof(STRU_ELMENT_T));
  strcpy(p_element->name,name);
  p_element->obj=usrdata;
  return p_element;
}
int delete_element(STRU_ELMENT_T *p_element)
{
    if(p_element)
	{
       free(p_element);
    }
    return 0;
}
STRU_PAD_T *creat_pad(char *name,void *usrdata)
{
  STRU_PAD_T *p_pad=malloc(sizeof(STRU_PAD_T));
  memset(p_pad,0,sizeof(STRU_PAD_T));
  strcpy(p_pad->name,name);
  p_pad->obj=usrdata;
  return p_pad;
}
int delete_pad(STRU_PAD_T *p_pad)
{
   if(p_pad)
   {
       free(p_pad);
   }
   return 0;
}
int add_out_pad(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad)
{
  p_element->pad_out[p_element->out_pad_count]=p_pad;
  p_element->out_pad_count++;
  p_pad->p_element=p_element;
  return 0;
}
int add_in_pad(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad)
{
  p_element->pad_in[p_element->in_pad_count]=p_pad;
  p_element->in_pad_count++;
  p_pad->p_element=p_element;
  return 0;
}
int push_to_next(STRU_ELMENT_T *p_element,STRU_PAD_T *p_current_pad,SRTU_PAD_FRAME_T *p_frame)
{
  SRTU_PAD_FRAME_T *p_frame_free=NULL;
  if(p_element&&p_current_pad&&p_frame)
  {
     if(p_current_pad->next)
     {
        STRU_PAD_T *p_next_pad=(STRU_PAD_T *)p_current_pad->next;
		if(p_next_pad->buffer_chain){
           p_frame_free=p_next_pad->buffer_chain(p_next_pad->p_element,p_next_pad,p_frame);
           if(p_frame_free == NULL){}
		}else
		{
		   ar_err("not buffer chain");
		}
     }else
     {
       ar_err("not link");
     }
  }else
  {
     ar_err("%p %p %p",p_element,p_current_pad,p_frame);
	 return -1;
  }
  return 0;
}
int link_pad(STRU_ELMENT_T *p_src_element,int out_pad_index,STRU_ELMENT_T *p_dest_element,int in_pad_index)
{
   if(!p_src_element || (out_pad_index>(p_src_element->out_pad_count-1)))
   {
	 ar_err("err p_src_element=%p out_pad_index=%d out_pad_count=%d",p_src_element,out_pad_index,p_src_element->out_pad_count);
	 return -1;
   }
   if(p_dest_element&&(in_pad_index<p_dest_element->in_pad_count)){
       p_src_element->pad_out[out_pad_index]->next=p_dest_element->pad_in[in_pad_index];
       p_dest_element->pad_in[in_pad_index]->pairing=p_src_element->pad_out[out_pad_index];
   }else
   {
      if(p_dest_element)
      {
		  ar_err("err p_dest_element=%p pad in_pad_index=%d in_pad_count=%d",p_dest_element,in_pad_index,p_dest_element->in_pad_count);
		  return -1;
      }else
      {
         p_src_element->pad_out[out_pad_index]->next=NULL;
      }
   }
   return 0;
}

void dump_dev_info(STRU_SENSOR_DEV_INFO_T *p_info)
{
	ar_printf("mount_angle=%d\n",p_info->mount_angle);
	ar_printf("face=%d\n",p_info->face);
	ar_printf("focus_supported=%d\n",p_info->focus_supported);
	ar_printf("flash_supported=%d\n",p_info->flash_supported);
	ar_printf("sensor_name=%s\n",p_info->sensor_name);
	ar_printf("device_type=%d\n",p_info->device_type);
}

static STRU_AR_HAL_VO_DSI_ATTR g_dsi_cfg =
{
    .dev_no         = 0,
    .lane           = {1,1,1,1},
    .bits_per_pixel = 24,
    .sync_info      = {
                          .hsa  = 6,
                          .hbp  = 120,
                          .hact = 1024,
                          .hfp  = 120,

                          .vsa  = 10,
                          .vbp  = 23,
                          .vact = 600,
                          .vfp  = 12
                      },
     .dphy_clk_mhz     = 313.9344,
     .pll_freq_reg_2c0 = 0x32F74FD1,
     .pll_freq_reg_38c = 0x2800
};

#define lcd_backlight_port               PORT_B
#define lcd_backlight_group              GROUP_1
#define lcd_backlight_number             0
#define lcd_backlight_pad                30
#define lcd_backlight_pad_gpio_fun_num   2

#define lcd_power_port                   PORT_B
#define lcd_power_group                  GROUP_1
#define lcd_power_number                 2
#define lcd_power_pad                    32
#define lcd_power_pad_gpio_fun_num       2

#define lcd_standby_port                 PORT_A
#define lcd_standby_group                GROUP_1
#define lcd_standby_number               6
#define lcd_standby_pad                  28
#define lcd_standby_pad_gpio_fun_num     2

#define lcd_rest_port                    PORT_B
#define lcd_rest_group                   GROUP_1
#define lcd_rest_number                  1
#define lcd_rest_pad                     31
#define lcd_rest_pad_gpio_fun_num        2

#define lcd_updown_scan_port             PORT_D
#define lcd_updown_scan_group            GROUP_1
#define lcd_updown_scan_number           7
#define lcd_updown_scan_pad              26
#define lcd_updown_pad_gpio_fun_num      3

void ar_vo_lcd_gpio_int(void)
{
    int backlight = ar_hal_gpio_name_to_num(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number);
    int power_gpio=ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
    int standby_gpio=ar_hal_gpio_name_to_num(lcd_standby_group, lcd_standby_port, lcd_standby_number);
    int reset_gpio=ar_hal_gpio_name_to_num(lcd_rest_group, lcd_rest_port, lcd_rest_number);
    int updown_gpio=ar_hal_gpio_name_to_num(lcd_updown_scan_group, lcd_updown_scan_port, lcd_updown_scan_number);

    ar_always("backlight = %d, power_gpio = %d standby_gpio=%d, reset_gpio = %d, updown_gpio=%d",
                    backlight, power_gpio, standby_gpio, reset_gpio, updown_gpio);

    ar_hal_gpio_export(backlight);
    ar_hal_gpio_export(power_gpio);
    ar_hal_gpio_export(standby_gpio);
    ar_hal_gpio_export(reset_gpio);
    ar_hal_gpio_export(updown_gpio);

    //set the gpio dir to out
    ar_hal_gpio_set_dir(backlight,1);
    ar_hal_gpio_set_dir(power_gpio,1);
    ar_hal_gpio_set_dir(standby_gpio,1);
    ar_hal_gpio_set_dir(reset_gpio,1);
    ar_hal_gpio_set_dir(updown_gpio,1);


    ar_hal_gpio_set_value(power_gpio,1);
    ar_hal_gpio_set_value(backlight,1);

    //standby the lcd
    ar_hal_gpio_set_value(updown_gpio,0);
    ar_hal_gpio_set_value(standby_gpio,1);

    usleep(10000);


    ar_hal_gpio_set_value(reset_gpio,1);
    usleep(1000);
    ar_hal_gpio_set_value(reset_gpio,0);
    usleep(10000);
    ar_hal_gpio_set_value(reset_gpio,1);

    //final we backlight the lcd
    ar_hal_gpio_set_value(backlight,1);

	return;
}

int ar_vo_dev_init(STRU_DISPLAY_OBJ_T *vo_obj,int width,int height,int y_stride,
                            int uv_stride,float fps, AR_BOOL is_mipi_tx, AR_BOOL is_interlace)
{
    STRU_AR_HAL_VO_DEV_ATTR    dev_attr = {0};
    STRU_AR_HAL_VO_LAYER_ATTR  layer_attr = {0};
    // STRU_AR_HAL_VO_CHN_ATTR    chan_attr = {0};
    int                        ret;

    ar_hal_log_init();

    vo_obj->e_dev_id = AR_HAL_VO_DEV_ID_HD0;
    vo_obj->e_layer_id = AR_HAL_VO_LAYER_ID_VIDEO_0;

    ar_always("dev_id = %d, layer_id = %d fps=%f, is_mipi_tx=%d",
                   vo_obj->e_dev_id, vo_obj->e_layer_id,fps, is_mipi_tx);
    ar_always("width = %d, height = %d, y_stride = %d, uv_stride = %d",
                   width, height, y_stride, uv_stride);


    /* init vo dev */
    vo_obj->dev_fd = ar_hal_vo_dev_open(vo_obj->e_dev_id);
    if (vo_obj->dev_fd < 0)
    {
        ret = HAL_ERR_VO_DEV_OPEN;
        goto End;
    }

    if (is_mipi_tx)
    {
        dev_attr.bg_color = 0x808080;
        dev_attr.e_interface = AR_HAL_VO_DEV_INTF_MIPI;
        dev_attr.e_sub_interface = AR_HAL_VO_DEV_SUB_INTF_NONE;
        dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_USER;
        dev_attr.timing_customize.de_polarity = AR_FALSE;
        dev_attr.timing_customize.da_polarity = AR_FALSE;
        dev_attr.timing_customize.clock_polarity = AR_FALSE;
        dev_attr.timing_customize.hsync_polarity = AR_FALSE;
        dev_attr.timing_customize.vsync_polarity = AR_FALSE;
        dev_attr.timing_customize.fps = 60;
        dev_attr.timing_customize.interlace_mod = AR_FALSE;
        dev_attr.timing_customize.hpw = 8;
        dev_attr.timing_customize.hbp = 160;
        dev_attr.timing_customize.hdp = 1024;
        dev_attr.timing_customize.hfp = 160;
        dev_attr.timing_customize.vpw = 10;
        dev_attr.timing_customize.vbp = 23;
        dev_attr.timing_customize.vdp = 600;
        dev_attr.timing_customize.vfp = 12;
    }
    else
    {
        dev_attr.bg_color = 0x808080;
        dev_attr.e_interface = AR_HAL_VO_DEV_INTF_DVP;
        dev_attr.e_sub_interface = AR_HAL_VO_DEV_SUB_INTF_HDMI;
        if (is_interlace)
            dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_1080I60;
        else
            dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_1080P60;
    }

    dev_attr.bit_count_per_channel = 8;
    ret = ar_hal_vo_dev_set_attr(vo_obj->dev_fd, &dev_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_dev_enable(vo_obj->dev_fd);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    if (is_mipi_tx)
    {
        ret = ar_hal_vo_dsi_set_attr(vo_obj->dev_fd, &g_dsi_cfg);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

        #if 0
        STRU_AR_HAL_VO_DSI_CMD   cmd;
        uint8_t  cmdstr[10] = {0,1,2,3,4,5 ,6,7,8,9};
        cmd.dev_no = 0;
        cmd.data_type = MIPI_DATA_TYPE_DCS_LONG_WRITE;
        cmd.cmd_size = 10;
        cmd.cmd = cmdstr;

        ret = ar_hal_vo_dsi_cmd(vo_obj->dev_fd, &cmd);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

        STRU_AR_HAL_VO_DSI_READ   cmd;
        uint8_t  cmdstr[10] = {0};
        cmd.dev_no = 0;
        cmd.data_type = MIPI_DATA_TYPE_GENERIC_SHORT_READ_1_PARA;
        cmd.para = 0x04;
        cmd.data_size = 4;
        cmd.data = cmdstr;

        ret = ar_hal_vo_dsi_read(vo_obj->dev_fd, &cmd);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

        #endif
        ret = ar_hal_vo_dsi_enable(vo_obj->dev_fd);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;

        ar_vo_lcd_gpio_int();
    }

    /* init vo layer */
    vo_obj->layer_fd = ar_hal_vo_layer_open(vo_obj->e_layer_id);
    if (vo_obj->layer_fd < 0)
    {
        ret = HAL_ERR_VO_LAYER_OPEN;
        goto End;
    }
    layer_attr.width = width;
    layer_attr.height = height;
    layer_attr.luma_stride = y_stride;
    layer_attr.chroma_stride = uv_stride;
    layer_attr.format = AR_HAL_VO_FMT_YV12;
    layer_attr.fps = fps;
    ret = ar_hal_vo_layer_set_attr(vo_obj->layer_fd,  &layer_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_layer_enable(vo_obj->layer_fd);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        goto End;
    }
	End:
	 if (ret != AR_HAL_VO_SUCCESS)
		 ar_err("ret = 0x%x", ret);

	 return ret;
}

//disalbe layer and display dev
int ar_vo_dev_deinit(STRU_DISPLAY_OBJ_T *vo_obj)
{

    if(vo_obj->layer_fd>=0){
       ar_hal_vo_layer_disable(vo_obj->layer_fd);
	   ar_hal_vo_fd_close(vo_obj->layer_fd);
    }

    if(vo_obj->dev_fd>=0){
	    ar_hal_vo_dev_disable(vo_obj->dev_fd);
		ar_hal_vo_fd_close(vo_obj->dev_fd);
    }
	return 0;
}

int ar_vo_surface_init(STRU_SURFACE_OBJ_T *vo_obj,
                          STRU_AR_HAL_VO_RECT *chan_pos)
{
    STRU_AR_HAL_VO_CHN_ATTR    chan_attr;
    int                        ret;

    ar_always("ch id=%d",vo_obj->chan_id);
    /* init vo channel */
    vo_obj->chan_fd = ar_hal_vo_chn_open(vo_obj->e_layer_id, vo_obj->chan_id);
    if (vo_obj->chan_fd < 0)
    {
        ret = HAL_ERR_VO_CHN_OPEN;
        goto End;
    }

    chan_attr.priority = 0;
    chan_attr.rect = *chan_pos;

    ret = ar_hal_vo_chn_set_attr(vo_obj->chan_fd, &chan_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_chn_enable(vo_obj->chan_fd);
    if (ret != AR_HAL_VO_SUCCESS)
       goto End;

   End:
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);

    return ret;
}

int main (int argc, char *argv[])
{

   ar_log_init();

   STRU_CAMERA_T *p_camera=malloc(sizeof(STRU_CAMERA_T));
   memset(p_camera,0,sizeof(STRU_CAMERA_T));
   p_camera->p_test_case=__test_camera_case__start(STRU_CAMERA_TEST_CASE);
   p_camera->test_case_size=__test_camera_case__count(STRU_CAMERA_TEST_CASE);

   ar_always("p_test_case=%p test_case_size=%d",p_camera->p_test_case,p_camera->test_case_size);
   STRU_CAMERA_CMD_T *p_cmd=camera_construct_cmd(argc,argv);

   if(argc<2)
   {
      ar_err("err: no test cmd");
	  camera_test_run_list_cmd(p_camera,p_cmd);
      free(p_camera);
      return -1;
   }
   ar_always("current test case is %s \n",argv[1]);
   int i=0;
   for(i=0;i< p_camera->test_case_size;i++)
   {
     if(!strcmp(argv[1],p_camera->p_test_case[i].p_name))
     {
        p_camera->p_test_case[i].pfn_camera_test_run(p_camera,p_cmd);
		break;
     }
   }
   if(i>=p_camera->test_case_size)
   {
       ar_err("can not find test cmd %s \n",argv[1]);
   }
   free(p_cmd);
}
