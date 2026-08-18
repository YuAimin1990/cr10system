#include"camera.h"

static void dump_tuning_prop(STRU_TUNING_REQ_UPDATE_PROP_T  *update_prop)
{
	ar_always("disable_update=%d",update_prop->disable_update);
	ar_always("only_force_update=%d",update_prop->only_force_update);
	ar_always("update_src=%d",update_prop->update_src);
	ar_always("force_update=%d",update_prop->force_update);
	ar_always("trigger_from=%d",update_prop->trigger_from);
	ar_always("aec_up_index=%d",update_prop->aec_up_index);
	ar_always("aec_down_index=%d",update_prop->aec_down_index);
	ar_always("aec_weight_down=%f",update_prop->aec_weight_down);
	ar_always("awb_up_index=%d",update_prop->awb_up_index);
	ar_always("awb_down_index=%d",update_prop->awb_down_index);
	ar_always("awb_weight_down=%f",update_prop->awb_weight_down);
}
static int api_test_gamma_tuning_prop(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 AR_S32  dev_num=0;
		 int ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
		 ar_always("dev_num=%d",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
			dump_dev_info(p_info);
		 }
		 free(p_info);
         int dev_id=0;
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
			   ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(vin_fd,dev_id,&pipe_id);
			   ar_always("dev id =%d pipe_id=%d",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0)
			   {
				  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				  if(fd_pipe>=0)
				  {
				       STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA *p_gamma_pra=malloc(sizeof(STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA));
					   //gen a line gamma
					   for(int i=0;i<4096;i++)
					   {
                          p_gamma_pra->k[i]=i;
					   }
					   //get current gamma update prop
					   int fd_mm=ar_hal_vin_open_dev_mem();
					   if(fd_mm<0)
					   {
						  ar_err("err open dev/mem \n");
						  return -1;
					   }
					   STRU_PIPE_SET_TUNING_REQUEST_T *req_v=NULL;

					   STRU_PIPE_SET_TUNING_REQUEST_T *req=NULL;
					   STRU_VIN_MEM_T tuning_req_mem;
					   ar_hal_vin_get_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);
					   req=tuning_req_mem.remote_pa;

					   AR_S32 offset_ret=0;
					   void *req_addr=ar_hal_vin_map_phy_addr(fd_mm,(void *)req,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T),&offset_ret);
					   req_v=(STRU_PIPE_SET_TUNING_REQUEST_T *)(req_addr+offset_ret);

					   req_v->req_pra.update_prop.moudle=TUNING_ISP_GAMMA;
					   req_v->req_type = TUNING_REQ_GET_UPDATE_PROP;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

					   //get update prop completed
					   dump_tuning_prop(&req_v->req_pra.update_prop);

					   req_v->req_pra.read_tuning_pra.tuning_type=TUNING_ISP_GAMMA;
					   req_v->req_type = TUNING_REQ_READ_TUNING_PRA;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

					   ar_always("curremt gamma have %d group trigger",req_v->req_pra.read_tuning_pra.tuning.isp_sub_module_gamma_lut_tuning.count_ae);

                       //read current gamma
					   req_v->req_pra.read_cur_isp_pra.tuning_type=TUNING_ISP_GAMMA;
					   req_v->req_type=TUNING_REQ_READ_CUR_ISP_PRA;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

					   STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA *p_current_gamma_pra=malloc(sizeof(STRU_ISP_SUB_MODULE_GAMMA_LUT_PRA));
					   *p_current_gamma_pra=req_v->req_pra.read_cur_isp_pra.tuning.isp_sub_module_gamma_lut_tuning;


					   //lock the tuning
					   ar_always("lock the tuning");
					   req_v->req_type = TUNING_REQ_LOCK_TUNING_PRA;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

					   //modify tuing prop
					   ar_always("modify the prop");
					   int force_type=1;
					   req_v->req_pra.update_prop.disable_update=0;
					   memset(&req_v->req_pra.update_prop,0,sizeof(req_v->req_pra.update_prop));

					   index=camera_find_cmd_pra_index_by_name("-user_mode",p_cmd);
					   if(index >=0)
					   {
					        if(!strcmp(p_cmd->args[index+1],"auto"))
					        {
					            memset(&req_v->req_pra.update_prop,0,sizeof(req_v->req_pra.update_prop));
					        }else if(!strcmp(p_cmd->args[index+1],"user_write"))
					        {
								req_v->req_pra.update_prop.only_force_update=1;
								req_v->req_pra.update_prop.force_update=1;
								req_v->req_pra.update_prop.update_src=1;
								req_v->req_pra.update_prop.trigger_from=0;
								force_type=2;
					        }else if(!strcmp(p_cmd->args[index+1],"user_sel"))
					        {
								index=camera_find_cmd_pra_index_by_name("-index",p_cmd);
								if(index > 0)
								{
								    int ae_index=atoi(p_cmd->args[index+1]);
									req_v->req_pra.update_prop.only_force_update=0;
									req_v->req_pra.update_prop.force_update=0;
									req_v->req_pra.update_prop.update_src=0;
									req_v->req_pra.update_prop.trigger_from=1;
									req_v->req_pra.update_prop.aec_up_index=ae_index;
									req_v->req_pra.update_prop.aec_down_index=ae_index;
									req_v->req_pra.update_prop.aec_weight_down=0.5;
								}
					        }else
					        {
					            //auto
					            memset(&req_v->req_pra.update_prop,0,sizeof(req_v->req_pra.update_prop));
					        }
					   }else
					   {
					       //auto
					       memset(&req_v->req_pra.update_prop,0,sizeof(req_v->req_pra.update_prop));
					   }
					   req_v->req_pra.update_prop.moudle=TUNING_ISP_GAMMA;
					   req_v->req_type = TUNING_REQ_SET_UPDATE_PROP;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);
                       ar_always("get update prop after set");
					   req_v->req_pra.update_prop.moudle=TUNING_ISP_GAMMA;
					   req_v->req_type = TUNING_REQ_GET_UPDATE_PROP;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

					   dump_tuning_prop(&req_v->req_pra.update_prop);

                       ar_always("write the gamma par to tuning");
					   req_v->req_pra.read_cur_isp_pra.tuning_type=TUNING_ISP_GAMMA;
					   req_v->req_type = TUNING_REQ_SET_CUR_ISP_TUNING_PRA;

                       req_v->req_pra.read_cur_isp_pra.tuning.isp_sub_module_gamma_lut_tuning=*p_gamma_pra;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

                       ar_always("unlock the tuning pra,to trigger tuning update");
					   req_v->req_type = TUNING_REQ_UNLOCK_TUNING_PRA;
					   req_v->req_pra.unlock_tuning_pra.force_type=force_type;
					   ar_hal_vin_process_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);

					   ar_hal_vin_release_tuning_req(fd_pipe,pipe_id,&tuning_req_mem);
					   ar_hal_vin_ummap_vaddr(req_addr,sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
					   ar_hal_vin_close_dev_mem(fd_mm);

				  }else
				  {
				      ar_err("open fd pipe failed");
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

__test_camera_case__(gamma_tuning_prop)=
{
   "--gamma_tuning_prop",
   api_test_gamma_tuning_prop,
   {
      "test gamma tuing update prop, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-user_mode [mode:auto/user_write/user_sel]",
	  "-index [index]: which group will be use if user_mode is user_sel",
	  NULL,
   },
};
