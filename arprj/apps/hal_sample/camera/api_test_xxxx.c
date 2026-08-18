#include"camera.h"

static int api_test_xxxxx(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
	   ar_printf("%s %d test pig remote, send the cmd to remote server, then server to call hal api......................\n",__FUNCTION__,__LINE__);
   }else
   {
	   ar_printf("%s %d test pig local ,call the hal api in local......................\n",__FUNCTION__,__LINE__);
   }
   return 0;
}

__test_camera_case__(api_test_xxxxx)=
{
   "--api_test_xxxxx",
   api_test_xxxxx,
   {
      "test the api xxxxxxx, -remote [remote binder server name]: this indicate the api run on remote app,",
	  NULL,
   },
};

