#include"camera.h"

typedef struct
{
   int sub_module;
   char *sub_module_name;
}STRU_SUB_MODULE_LIST_T;


#define __SUB_MODULE__(sub_module) {sub_module,#sub_module}

static STRU_SUB_MODULE_LIST_T g_list[]=
{
	__SUB_MODULE__(TUNING_ISP_CROP_RAW),
	__SUB_MODULE__(TUNING_ISP_BLC),
	__SUB_MODULE__(TUNING_ISP_HDR_MIX),
	__SUB_MODULE__(TUNING_ISP_CAMPANDER),
	__SUB_MODULE__(TUNING_ISP_DPC),
	__SUB_MODULE__(TUNING_ISP_CAC),
	__SUB_MODULE__(TUNING_ISP_ATA),
	__SUB_MODULE__(TUNING_ISP_RNR),
	__SUB_MODULE__(TUNING_ISP_DECOMPANDER),
	__SUB_MODULE__(TUNING_ISP_LSC),
	__SUB_MODULE__(TUNING_ISP_DRC),
	__SUB_MODULE__(TUNING_ISP_GIC),
	__SUB_MODULE__(TUNING_ISP_CFA),
	__SUB_MODULE__(TUNING_ISP_DPP),
	__SUB_MODULE__(TUNING_ISP_CCM1),
	__SUB_MODULE__(TUNING_ISP_CCM2),
	__SUB_MODULE__(TUNING_ISP_GTM1_LUT),
	__SUB_MODULE__(TUNING_ISP_GAMMA),
	__SUB_MODULE__(TUNING_ISP_GTM2_LUT),
	__SUB_MODULE__(TUNING_ISP_3DLUT),
	__SUB_MODULE__(TUNING_ISP_RGB2YUV),
	__SUB_MODULE__(TUNING_ISP_CM),
	__SUB_MODULE__(TUNING_ISP_CM2),
	__SUB_MODULE__(TUNING_ISP_CNF),
	__SUB_MODULE__(TUNING_ISP_LNR_LEE),
	__SUB_MODULE__(TUNING_ISP_DITHER),
	__SUB_MODULE__(TUNING_ISP_DE3D),
	__SUB_MODULE__(TUNING_ISP_ALGO_AEC),
	__SUB_MODULE__(TUNING_ISP_ALGO_AWB),
	__SUB_MODULE__(TUNING_ISP_ALGO_AF),
	__SUB_MODULE__(TUNING_ISP_HDR_LSC),
};

static inline STRU_SUB_MODULE_LIST_T *get_sub_module_list()
{
   return g_list;
}
static inline int get_sub_module_list_count()
{
   return sizeof(g_list)/sizeof(STRU_SUB_MODULE_LIST_T);
}

static int show_all_sub_module()
{
	STRU_SUB_MODULE_LIST_T *p_list=get_sub_module_list();
	for(int i=0;i<get_sub_module_list_count();i++)
	{
	   ar_printf("%d==>%s \n",p_list[i].sub_module,p_list[i].sub_module_name);
	}
	return 0;
}

static int sub_moudle_name_2_sub_module(char *sub_moudle_name)
{
	STRU_SUB_MODULE_LIST_T *p_list=get_sub_module_list();
	for(int i=0;i<get_sub_module_list_count();i++)
	{
	   if(!strcmp(sub_moudle_name,p_list[i].sub_module_name))
	   {
	      return p_list[i].sub_module;
	   }
	}
	return -1;
}
static int api_set_isp_submodule_state(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     int dev_id=0;
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 AR_S32  dev_num=0;
		 int ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
		 ar_always("dev_num=%d",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
			//dump_dev_info(p_info);
		 }
		 free(p_info);

		 dev_id=0;
		 index=camera_find_cmd_pra_index_by_name("-dev_id",p_cmd);
		 if(index>0)
		 {
			 dev_id=atoi(p_cmd->args[index+1]);
		 }
		 int opend=0;
		 ret=ar_hal_vin_dev_is_opend(vin_fd,dev_id,&opend);
		 if(ret<0 || opend)
		 {
			ar_always("the device have started ret=%d opend=%d",ret,opend);
			if(opend)
			{
			   int pipe_id=0;
			   int flag=0;
			   ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(vin_fd,dev_id,&pipe_id);
			   ar_printf("dev id =%d pipe_id=%d\n",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0)
			   {
					int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);

					index=camera_find_cmd_pra_index_by_name("-list",p_cmd);
					if(index>0)
					{
                        show_all_sub_module();
						flag++;
					}
					index=camera_find_cmd_pra_index_by_name("-set_state",p_cmd);
					if(index > 0)
					{
					   int sub_module=0;
					   int enable=1;
                       char *sub_moudle_name=p_cmd->args[index+1];
					   enable=atoi(p_cmd->args[index+2]);
					   sub_module=sub_moudle_name_2_sub_module(sub_moudle_name);
					   if(sub_module>=0){
					     ar_hal_vin_set_isp_submodule_state(fd_pipe,pipe_id,sub_module,enable);
					   }else
					   {
					       ar_err("not find the submodule, please sel following ...............................");
					       show_all_sub_module();
					   }
					   flag++;
					}
					index=camera_find_cmd_pra_index_by_name("-get_state",p_cmd);
				    if(index> 0)
					{
					   int sub_module=0;
					   int enable=0;
                       char *sub_moudle_name=p_cmd->args[index+1];
					   sub_module=sub_moudle_name_2_sub_module(sub_moudle_name);
					   if(sub_module>=0){
					     ar_hal_vin_get_isp_submodule_state(fd_pipe,pipe_id,sub_module,&enable);
                         ar_printf("the module %s sub_moudle_name status is :%d\n",sub_moudle_name,enable);
					   }else
					   {
					       ar_err("not find the submodule, please sel following ...............................");
					       show_all_sub_module();
					   }
					   flag++;
					}
					if(!flag)
					{
					    show_all_sub_module();
					}
					ar_hal_vin_close_pipe_dev(fd_pipe);
			   }
			   else
			   {
			      ar_err("can not get bind pipid by devid");
			   }
			}
		  }else
		  {
			  ar_err("the dev not opend, can not do this test, so open the dev, to do later");
		  }
		 ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(isp_submodule_state)=
{
   "--isp_submodule_state",
   api_set_isp_submodule_state,
   {
      "test the api ar_hal_vin_set_isp_submodule_state, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-set_state [module] [1/0]",
	  "-get_state [module]",
	  "-list",
	  NULL,
   },
};
