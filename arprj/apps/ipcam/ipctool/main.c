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
#include "ipctool_client_handler.h"
//#include "binder_ipc_common.h"
#include "binder_ipc.h"

IPCTOOL_SERVICE_S g_current_client_service[] =
{
    {"setvires", "setvires -c [channel] -w [weight] -h [height]", AR_IPCTOOL_HandlerViRes},
    {"setvifps",  "setvifps -c [channel] -f [fps]", AR_IPCTOOL_HandlerViFps},
    {"setviflicker",  "setviflicker -c [channel] -f [freq 0:auto 1:50Hz 2:60Hz]", AR_IPCTOOL_HandlerViAntiFlicker},
    {"getvide3d",  "getvide3d -c [channel]", AR_IPCTOOL_HandlerGetDenoise3D},
    {"setvide3d",  "setvide3d -c [channel] -s [strength]", AR_IPCTOOL_HandlerSetDenoise3D},
    {"getvide2d",  "getvide2d -c [channel]", AR_IPCTOOL_HandlerGetDenoise2D},
    {"setvide2d",  "setvide2d -c [channel] -s [strength]", AR_IPCTOOL_HandlerSetDenoise2D},
    {"getviexplmt",  "getviexplmt -c [channel]", AR_IPCTOOL_HandlerGetExpLimit},
    {"setviexplmt",  "setviexplmt -c [channel] -e [us]", AR_IPCTOOL_HandlerSetExpLimit},
    {"setviwdr",  "setviwdr -c [channel] -en [0/1]", AR_IPCTOOL_HandlerViWdr},
    {"getvi3a", "getvi3a -c [channel]", AR_IPCTOOL_HandlerVi3aInfo},
    {"setvencbrc", "setvencbrc -c [channel] -s [stream] -m [mode] -max [value] -min [value] -avg [value] -stattime [1-60]", AR_IPCTOOL_HandlerVencBrc},
    {"setvencgop", "setvencgop -c [channel] -s [stream] -v [value]", AR_IPCTOOL_HandlerVencGop},
    {"setvenctype", "setvenctype -c [channel] -s [stream] -t [type]", AR_IPCTOOL_HandlerVencType},
    {"setvencfps", "setvencfps -c [channel] -s [stream] -f [fps]", AR_IPCTOOL_HandlerVencFps},
    {"setvencqp", "setvencqp -c [channel] -s [stream] -minQpI [qp] -maxQpI [qp] -minQpP [qp] -maxQpP [qp]", AR_IPCTOOL_HandlerVencQp},
    {"vrecord", "startvrecord -c [channel] -s [stream] -f [format] -t [1start/0stop] -m [1manul/2sched]", AR_IPCTOOL_HandlerVrecord},
    {"arecord", "arecord -path [path] -start [start|stop : 1|0] -stream [stream]", AR_IPCTOOL_HandlerArecord},
    {"playback", "playback -f [audio|vedio : 0|1], -path [path], -start [start|stop : 1|0]", AR_IPCTOOL_HandlerPlayback},
    {"setflip", "setflip -c [channel] -en [0/1]", AR_IPCTOOL_HandlerFlip},
    {"setmirror", "setmirror -c [channel] -en [0/1]", AR_IPCTOOL_HandlerMirror},
    {"snapshot", "snapshot -g [vpss grp] -c [vpss chn] -cnt [value] -path [string]", AR_IPCTOOL_HandlerSnapshot},
    {"vofullscreen", "vofullscreen  -c [vo chn] -en [1 on/0 off]", AR_IPCTOOL_HandlerVoFullScreen},
    {"setimsgescene", "setimsgescene -c [channel] -m [0day/1night] -f [tunning name]", AR_IPCTOOL_HandlerImageScene},
    {"setimagestyle", "setimagestyle -c [channel] -m [0normal/1bright]", AR_IPCTOOL_HandlerImageStyle},
    {"setsaturation", "setsaturation -c [channel] -v [value]", AR_IPCTOOL_HandlerSetSaturation},
    {"setbright", "setbright -c [channel] -v [value]", AR_IPCTOOL_HandlerSetBright},
    {"setcontrast", "setcontrast -c [channel] -v [value]", AR_IPCTOOL_HandlerSetContrast},
    {"setsharpness", "setsharpness -c [channel] -v [value]", AR_IPCTOOL_HandlerSetSharpness},
    {"sethue", "sethue -c [channel] -v [value]", AR_IPCTOOL_HandlerSetHue},
    {"getsaturation", "getsaturation -c [channel]", AR_IPCTOOL_HandlerGetSaturation},
    {"getbright", "getbright -c [channel]", AR_IPCTOOL_HandlerGetBright},
    {"getcontrast", "getcontrast -c [channel]", AR_IPCTOOL_HandlerGetContrast},
    {"getsharpness", "getsharpness -c [channel]", AR_IPCTOOL_HandlerGetSharpness},
    {"gethue", "gethue -c [channel]", AR_IPCTOOL_HandlerGetHue},
    {"setwdt", "setwdt -power [power_status : 1on/0off]", AR_IPCTOOL_HandlerWdt},
    {"setgpio", "setgpio -n [A0_0] -d [0in/1out] -v [0/1]", AR_IPCTOOL_HandlerSetGpio},
    {"getgpio", "getgpio -n [A0_0]", AR_IPCTOOL_HandlerGetGpio},
    {"setsvp", "setsvp  -power [power_status : 1on/0off]", AR_IPCTOOL_HandlerSvp},
    {"sysreboot", "sysreboot -reboot [flag]", AR_IPCTOOL_HandlerReboot},
    {"syspoweroff", "syspoweroff -power [flag]", AR_IPCTOOL_HandlerPowerOff},
    {"setwifimode", "setwifimode -m [mode]", AR_IPCTOOL_HandlerWifiMode},
    {"dumpyuv", "dumpyuv -c [channel]", AR_IPCTOOL_HandlerDumpYuv},
    {"getvencparam", "getvencparam -c [channel] -s [streamid]", AR_IPCTOOL_HandlerGetVencParam},
    {"drawosd", "drawosd -g [vpss grp] -iw [img width] -ih [img height] -s [string] -sc [RGB] -rect -rc [RGB]", AR_IPCTOOL_HandlerDrawOsd},
    {"clearosd", "clearosd -g [vpss grp]", AR_IPCTOOL_HandlerClearOsd},
    {"setstreamosd", "setstreamosd -c [channel] -s [stream] -name [string] -r [RGB] -ce [0/1] -te [0/1] -tf [0/1/2/3/4/5] -df [0/1] - we [0/1] -f [fontid]", AR_IPCTOOL_HandlerSetStreamOsd},
    {"setvpssflip", "setvpssflip -g [group] -c [channel] -en [0/1]", AR_IPCTOOL_HandlerVpssFlip},
    {"setvpssmirror", "setvpssmirror -g [group] -c [channel] -en [0/1]", AR_IPCTOOL_HandlerVpssMirror},
    {"start", "start elements from cfg_control.json", AR_IPCTOOL_HandlerStart},
    {"stop", "stop elements from cfg_control.json", AR_IPCTOOL_HandlerStop},
    {"create", "create<use for test IPC_AV_Init>", AR_IPCTOOL_HandlerCreate},
    {"destory", "destory<use for test IPC_AV_UnInit>", AR_IPCTOOL_HandlerDestory},
    {"setmd", "setmd -en [0/1]", AR_IPCTOOL_HandlerMd},
    {"setod", "setod -en [0/1]", AR_IPCTOOL_HandlerOd},
    {{0}, {0}, NULL}
};

static void usage(void)
{
    int i = 0;

	printf("\n");

    for(i = 0; i < sizeof(g_current_client_service)/sizeof(g_current_client_service[0]); i++)
    {
        if(strlen(g_current_client_service[i].s8ServiceName))
        {
            printf("%s  %s\n", g_current_client_service[i].s8ServiceName, g_current_client_service[i].s8Help);
        }
    }
	printf("rtcmd <use for send general command>\r\n=>");

    printf("\n");
}

int main(int argc, char *argv[])
{
	AR_S32 s32Ret = 0;
	char *ps8Line = NULL;
	ssize_t read;
	size_t len;

	s32Ret = AR_IPCTOOL_BinderInit();
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

		if(strstr(ps8Line, "rtcmd")){
			s32Ret = AR_IPCTOOL_Send_Cmd(ps8Line);
			if(s32Ret){
				printf("ipctool send rtcmd failed\r\n");
			}

			if(ps8Line){
				free(ps8Line);
				ps8Line = NULL;
			}

			continue;
		}

		if(strstr(ps8Line, "help")){
			usage();
			free(ps8Line);
            ps8Line = NULL;
			continue;
		}

		if(strstr(ps8Line, "exit")){
			free(ps8Line);
            ps8Line = NULL;
			return 0;
		}
		IPCTOOL_CMD_ARGS_S * pstCmdArgs = AR_IPCTOOL_GetArgs(ps8Line);
		if(!pstCmdArgs){
			printf("ipctool get args failed\r\n");
			printf("\n=>");
           		free(ps8Line);
            		ps8Line = NULL;
			continue;
		}

		AR_S32 s32ServiceNum = sizeof(g_current_client_service)/sizeof(g_current_client_service[0]);
		int i = 0;
		for(i = 0; i < s32ServiceNum; i++)
		{
			if(!strcmp(pstCmdArgs->s8Args[0], g_current_client_service[i].s8ServiceName)){
				if(!strcmp(pstCmdArgs->s8Args[1], "help")){
					printf("%s\r\n",g_current_client_service[i].s8Help);
				}else{
					g_current_client_service[i].ServiceHandler(pstCmdArgs);
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

