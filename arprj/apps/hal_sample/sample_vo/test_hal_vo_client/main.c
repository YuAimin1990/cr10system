#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include "votest_client_handler.h"
//#include "binder_ipc_common.h"
#include "binder_ipc.h"

char set_dev_attr_help[] =
"\nYou can set some parameters, and the rest use the current parameters.\n"
"set_dev_attr\n"
"-dev_dev_id                   set device id\n"
"-dev_bg_color                 set background color of a device\n"
"-dev_interface                set vo interface type\n"
"-dev_sub_interface            set vo interface type\n"
"-dev_timing_template          set timing tempalate type\n"
"-dev_dpi_format               set dpi format\n"
"-dev_color_mode               set color mode\n"
"-dev_sync_mode                set sync mode\n"
"-dev_pin_mode                 set pin mode\n"
"-dev_csc                      set device css\n"
"-dev_bit_count_per_channel    set bit count per channel\n"
"If e_timing_template is AR_HAL_VO_OUTPUT_USER, you can set the following parameters:\n"
"-syn_interlace_mod            set interlace mod\n"
"-syn_hpw                      set hpw\n"
"-syn_hbp                      set hbp\n"
"-syn_hdp                      set hdp\n"
"-syn_hfp                      set hfp\n"
"-syn_vpw                      set vpw\n"
"-syn_vbp                      set vpw\n"
"-syn_vdp                      set vdp\n"
"-syn_vfp                      set vfp\n"
"-syn_fps                      set sync fps\n"
"-syn_de_polarity              set de_polarity\n"
"-syn_da_polarity              set da_polarity\n"
"-syn_clock_polarity           set clock_polarity\n"
"-syn_hsync_polarity           set hsync_polarity\n"
"-syn_vsync_polarity           set vsync_polarity\n";

char set_layer_attr_help[] =
"\nYou can set some parameters, and the rest use the current parameters.\n"
"set_layer_attr\n"
"-dev_dev_id                   set device id\n"
"-layer_id                     set layer id\n"
"-layer_width                  set layer width\n"
"-layer_height                 set layer height\n"
"-layer_luma_stride            set luma stride\n"
"-layer_chroma_stride          set chroma stride\n"
"-layer_format                 set layer format\n"
"-layer_fps                    set layer fps\n"
"-layer_rotation               set rotation\n"
"-layer_csc_in                 set layer csc of input\n"
"-layer_csc_out                set layer csc of output\n"
"-overlay_posx                 set overlay posx\n"
"-overlay_posy                 set overlay posy\n";

char set_chn_attr_help[] =
"\nYou can set some parameters, and the rest use the current parameters.\n"
"set_chn_attr\n"
"-dev_dev_id                   set device id\n"
"-layer_id                     set layer id\n"
"-ch_id                        set channel id\n"
"-ch_priority                  set channel priority\n"
"-ch_width                     set channel width\n"
"-ch_height                    set channel height\n"
"-ch_pos_x                     set channel positon of x\n"
"-ch_pos_y                     set channel positon of y\n"
"-ch_bg_color                  set background color\n"
"-ch_aspect_ratio_mode         set aspect ratio mode\n"
"-ch_aspect_ratio_width        set aspect ratio width\n"
"-ch_aspect_ratio_height       set aspect ratio height\n"
"-ch_aspect_ratio_pos_x        set aspect ratio pos of x\n"
"-ch_aspect_ratio_pos_y        set aspect ratio pos of y\n";

VOTEST_SERVICE_S g_vo_client_service[] =
{
    {"pause_chan",       "pause_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",    AR_VOTEST_ClientPauseChannel},
    {"resume_chan",      "resume_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",   AR_VOTEST_ClientResumeChannel},
    {"hide_chan",        "hide_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",     AR_VOTEST_ClientHideChannel},
    {"show_chan",        "show_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",     AR_VOTEST_ClientShowChannel},
    {"step_chan",        "step_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",     AR_VOTEST_ClientStepChannel},
    {"dump_layer_frame", "dump_layer_frame -layer [layer_id] -name [file_name]",                 AR_VOTEST_ClientDumpLayerFrame},
    {"enable_dev",       "enable_dev -dev [device_id]",                                      AR_VOTEST_ClientEnableDev},
    {"disable_dev",      "disable_dev -dev [device_id]",                                     AR_VOTEST_ClientDisableDev},
    {"enable_layer",     "enable_layer -dev [device_id] -layer [layer_id]",                  AR_VOTEST_ClientEnableLayer },
    {"disable_layer",    "disable_layer-dev [device_id] -layer [layer_id]",                  AR_VOTEST_ClientDisableLayer},
    {"enable_chan",      "enable_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",   AR_VOTEST_ClientEnableChn},
    {"disable_chan",     "disable_chan -dev [device_id] -layer [layer_id] -chan [chan_id]",  AR_VOTEST_ClientDisableChn},
    {"set_dev_attr",     set_dev_attr_help,                                                  AR_VOTEST_ClientSetDevAttr},
    {"set_layer_attr",   set_layer_attr_help,                                                AR_VOTEST_ClientSetLayerAttr},
    {"set_chn_attr",     set_chn_attr_help,                                                  AR_VOTEST_ClientSetChnAttr}
 //   {NULL,NULL,NULL}
};

static void usage(void)
{
    int i = 0;

	printf("\n");

    for(i = 0; i < sizeof(g_vo_client_service)/sizeof(g_vo_client_service[0]); i++)
    {
        if(g_vo_client_service[i].s8ServiceName)
        {
            printf("%s  %s\n", g_vo_client_service[i].s8ServiceName, g_vo_client_service[i].s8Help);
        }
    }

    printf("\n");
}

int main(int argc, char *argv[])
{
	AR_S32 s32Ret = 0;
	char *ps8Line = NULL;
	ssize_t read;
	size_t len;

	s32Ret = AR_VOTEST_BinderInit();
	if(s32Ret < 0){
		printf("binder create failed\r\n");
		return -1;
	}
	printf("use 'help' to list supported cmd, 'exit' to quit\r\n");
	printf("\r\n=>");
	while((read = getline(&ps8Line, &len, stdin)) != -1){

		 if(strcmp(ps8Line, "\n") == 0){
            		printf("\n=>");
           		free(ps8Line);
            		ps8Line = NULL;
            		continue;
        	}
		ps8Line[read - 1 ] = '\0';
		if(strstr(ps8Line, "help")){
			usage();
			printf("\r\n=>");
			free(ps8Line);
            ps8Line = NULL;
			continue;
		}
        if(strstr(ps8Line, "exit")){
			free(ps8Line);
            ps8Line = NULL;
			return 0;
		}
		VOTEST_CMD_ARGS_S * pstCmdArgs = AR_VOTEST_GetArgs(ps8Line);
		if(!pstCmdArgs){
			printf("VOTEST get args failed\r\n");
			printf("\n=>");
           		free(ps8Line);
            		ps8Line = NULL;
			continue;
		}
		AR_S32 s32ServiceNum = sizeof(g_vo_client_service)/sizeof(g_vo_client_service[0]);
		int i = 0;
		for(i = 0; i < s32ServiceNum; i++)
		{
			if(!strcmp(pstCmdArgs->s8Args[0], g_vo_client_service[i].s8ServiceName)){
				if(!strcmp(pstCmdArgs->s8Args[1], "help")){
					printf("%s\r\n",g_vo_client_service[i].s8Help);
				}else{
					g_vo_client_service[i].ServiceHandler(pstCmdArgs);
				}
				break;
			}
		}
   		//printf("i = %d,s32ServiceNum = %d\r\n",i,s32ServiceNum);
		if(i == s32ServiceNum){
			printf("can't find input cmd\r\n");
		}
		printf("\r\n=>");
		if(pstCmdArgs){
			free(pstCmdArgs);
		}
		if(ps8Line){
			free(ps8Line);
			ps8Line = NULL;
		}
	}

}

