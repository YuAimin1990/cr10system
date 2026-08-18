#include"camera.h"

static int api_test_get_pipe_run_infor(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
			   ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(vin_fd,dev_id,&pipe_id);
			   ar_printf("dev id =%d pipe_id=%d\n",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0){
			   	   STRU_CAM_PRA_GET_PIPE_RUN_INFO_PRA_T info;
				   int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				   if(fd_pipe>=0)
				   {
				        info.stream_id=0;
				        ar_hal_vin_get_pipe_run_info(fd_pipe,pipe_id,&info);
						ar_hal_vin_close_pipe_dev(fd_pipe);

                        ar_printf("stream_id=%d\n",info.stream_id);
                        ar_printf("isp_irq:\n");

                       for(int i=0;i<8;i++)
                       {
                          int offset=i*8;
                          for(int j=0;j<8;j++)
                          {
                             int index=offset+j;
							 if(index<32)
							 {
                                ar_printf("%-2d(%-2d):%-8d",index,index,info.isp_irq[offset+j]);
							 }else
							 {
                                ar_printf("%-2d(%-2d):%-8d",index,index-32,info.isp_irq[offset+j]);
							 }
                          }
						  ar_printf("\n");
                       }
					   ar_printf("vif_irq:\n");
					   for(int i=0;i<8;i++)
                       {
                          int offset=i*8;
                          for(int j=0;j<8;j++)
                          {
                             int index=offset+j;
							 if(index<32)
							 {
                                ar_printf("%-2d(%-2d):%-8d",index,index,info.vif_irq[offset+j]);
							 }else
							 {
                                ar_printf("%-2d(%-2d):%-8d",index,index-32,info.vif_irq[offset+j]);
							 }
                          }
						  ar_printf("\n");
                       }
					   ar_printf("work_mode=%d\n",info.work_mode);
					   ar_printf("sensor_fps=%d\n",info.sensor_fps);
					   ar_printf("current_res=%f\n",info.current_res);
					   ar_printf("isp_used=%d\n",info.isp_used);
					   ar_printf("isp_wait_on_sch=%d\n",info.isp_wait_on_sch);
					   ar_printf("isp_wait_on_input_queue=%d\n",info.isp_wait_on_input_queue);
					   ar_printf("isp_sch_trigger_count=%d\n",info.isp_sch_trigger_count);
					   ar_printf("isp_fetch_comp_count=%d\n",info.isp_fetch_comp_count);
					   ar_printf("isp_wait_on_sof=%d\n",info.isp_wait_on_sof);
					   ar_printf("isp_wait_on_all_isp_done=%d\n",info.isp_wait_on_all_isp_done);
					   ar_printf("isp_trigger_count=%d\n",info.isp_trigger_count);
					   ar_printf("vif_loss_count=%d\n",info.vif_loss_count);
					   ar_printf("vif_done_count=%d\n",info.vif_done_count);
					   ar_printf("vif_frame_count=%d\n",info.vif_frame_count);
					   ar_printf("buffer_sink_fps_stats=%f\n",info.buffer_sink_fps_stats);
					   ar_printf("isp_frame_count=%d\n",info.isp_frame_count);
					   ar_printf("isp_loss_frame_count=%d\n",info.isp_loss_frame_count);
					   ar_printf("isp_vsync_count=%d\n",info.isp_vsync_count);
					   ar_printf("isp_done_count=%d\n",info.isp_done_count);
					   ar_printf("buffer_count_inqueue=%d\n",info.buffer_count_inqueue);

				   }else
				   {
                        ar_err("can not opened the pipe fd");
				   }
			   }else
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

__test_camera_case__(get_pipe_run_infor)=
{
   "--get_pipe_run_infor",
   api_test_get_pipe_run_infor,
   {
      "test the api api_test_get_pipe_run_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  NULL,
   },
};
