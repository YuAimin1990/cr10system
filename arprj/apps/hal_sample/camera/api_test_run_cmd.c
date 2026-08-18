#include"camera.h"

static int api_test_run_cmd(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int n=10;
   int index=camera_find_cmd_pra_index_by_name("-times",p_cmd);
   if(index > 0)
   {
      n=atoi(p_cmd->args[index+1]);
   }
   int delay=10;
   index=camera_find_cmd_pra_index_by_name("-delay",p_cmd);
   if(index > 0)
   {
      delay=atoi(p_cmd->args[index+1]);
   }
   int test_case=1;
   index=camera_find_cmd_pra_index_by_name("-case",p_cmd);
   if(index > 0)
   {
      test_case=atoi(p_cmd->args[index+1]);
   }
   int delay_after_stop=5;
   index=camera_find_cmd_pra_index_by_name("-delay_after_stop",p_cmd);
   if(index > 0)
   {
      delay_after_stop=atoi(p_cmd->args[index+1]);
   }

   for(int i=0;i<n;i++){

       ar_always("========================run camera %d times=========================================\n\n\n\n",i);
	   sleep(1);
	   switch(test_case)
	   {
          case 0:
          //system("camera --start_camera -dev_id 0 -server camera -scaler1_to surface  -surface1 0 0 1920 1080 -scaler1 1920 1080 0 0 1920 1080 2048 -trigger -scaler2 1920 1080 0 0 1920 1080 2048 -max_wh 1920 1080 25 &");
          system("camera --start_camera -dev_id 0 -server camera -scaler1_to surface  -surface1 0 0 1920 1080 -scaler1 640 480 0 0 1920 1080 2048 -trigger  -max_wh 1920 1080 25 &");
		  break;
		  case 1:
		  system("camera --start_camera -dev_id 0 -server camera -scaler1 1920 1080 0 0 1920 1080 2048 -scaler1_to surface -trigger -scaler2 640 480 0 0 1920 1080 2048 \
			  -surface1 0 0 640 480 -surface2 680 0 640 480 -scaler2_to surface -max_wh 1920 1080 25 &");

		  break;
		  default:
		  ar_always("===============================exit test, not supported case==========================================================\n\n\n\n");
		  return 0;
		  break;
	   }
	   sleep(delay);
	   system("camera --stop -remote camera_0 -exit");
	   sleep(delay_after_stop);
   }
   ar_always("===============================exit test==========================================================\n\n\n\n");
   return 0;
}

__test_camera_case__(run_cmd_test)=
{
   "--run_cmd",
   api_test_run_cmd,
   {
      "run cmd,",
	  "-times [0]",
	  "-delay [s]",
	  "-delay_after_stop [s]",
	  "-case [n]",
	  NULL,
   },
};
