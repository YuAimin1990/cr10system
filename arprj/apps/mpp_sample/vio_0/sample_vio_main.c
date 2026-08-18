#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ar_common.h"
#include "sample_vio.h"
#include "hal_vin_log.h"

AR_S32 g_s32LoadVinDriver = 0;

extern AR_VOID SAMPLE_AR_MPI_VIN_CloseDev();

/******************************************************************************
* function : show usage
******************************************************************************/

void SAMPLE_VIO_Usage(char *sPrgNm)
{
    printf("Usage : %s -index [n] -vo [n]\n", sPrgNm);
    printf("index:\n");
    printf("\t 12)SAMPLE_VIO_Only.\n");
    printf("\t 13)SAMPLE_VIO_Only_Hdr.\n");
    printf("\t 14)SAMPLE_VIO_Only_2Ch.\n");
    printf("\t 15)SAMPLE_VIO_Only_For_Bind.\n");
    printf("\t 16)SAMPLE_VIO_multi_process_get_ch_frame\n");
    printf("\t 17)SAMPLE_VIO_Only_Bind_Get_Pipe_Raw\n");
    printf("\t 18)SAMPLE_VIO_Only_For_Bind_ISP_IN \n");
    printf("\t 19)SAMPLE_VIO_Only_For_Bind_imx307_raw_process \n");
    printf("\t 20)SAMPLE_VIO_Only_Two_Sensor \n");
    printf("\t 21)SAMPLE_VIO_Only_TP9930_DVP_24Ch \n");
#ifdef AR9341
    printf("\t 22)SAMPLE_VIO_Only_For_Bind_With_IMX415.\n");
    printf("\t 23)SAMPLE_VIO_Only_For_Bind_With_IMX415_60Fps\n");
#endif
    printf("\t 24)SAMPLE_VIO_Only_AR_MPI_ISP_GetVDTimeOut.\n");
    printf("\t 25)SAMPLE_VIO_Only_Get3aStats.\n");
    printf("\t 26)SAMPLE_VIO_Only_mipi_2.\n");
    printf("\t 27)SAMPLE_VIO_Dual_Sensor_With_Hdr_Nohdr.\n");
    printf("\t 28)SAMPLE_VIO_Only_run_200_frame_exit.\n");
    printf("\t 29)SAMPLE_VIO_Three_Sensor.\n");
    printf("\t 30)SAMPLE_VIO_Only_YUV420SP.\n");
    printf("\t 31)SAMPLE_VIO_Only_IMX307_MIPI_1Ch_AND_TP9930_DVP_4Ch.\n");
    printf("\t 32)SAMPLE_VIO_Only_For_Bind_With_OS04A10.\n");
    printf("\t 33)SAMPLE_VIO_Only_Hdr_For_Bind_With_OS04A10.\n");
    printf("\t 34)SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Start_Stop.\n");
    printf("\t 35)SAMPLE_VIO_Only_For_Bind_GC2093.\n");
    printf("\t 36)SAMPLE_VIO_Only_For_Bind_GC2093_Hdr.\n");
    printf("\t 37)SAMPLE_VIO_imx307_No_Buffer_test.\n");
    printf("\t 38)SAMPLE_VIO_Only_For_Bind_With_SC530AI.\n");
    printf("\t 39)SAMPLE_VIO_SetFpsRange.\n");
    printf("\t 40)SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug.\n");
    printf("\t 41)SAMPLE_VIO_Only_imx307_crop_api_test.\n");
    printf("\t 42)SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug_UserPic.\n");
	printf("\t 43)SAMPLE_VIO_Only_Hdr_imx307_mulit_mode.\n");
	printf("\t 44)SAMPLE_VIO_Only_Two_Sensor_60fps_30fps.\n");
	printf("\t 45)SAMPLE_VIO_Only_imx307_af_fullsweep_test.\n");
	printf("\t 46)SAMPLE_VIO_Big_Pic_Demo.\n");
	printf("\t 48)SAMPLE_VIO_Vpss_imx307.\n");
	printf("\t 49)SAMPLE_VIO_Bind_imx307_dig_gain_test.\n");
	printf("\t 50)SAMPLE_VIO_Only_Test_Interlace.\n");
    printf("\t 51)SAMPLE_VI_Only.\n");
	printf("\t 52)SAMPLE_VIO_Only_Two_imx307_Sensor_4ch_stop_one_ch \n");
	#ifdef AR_FEAT_EIS
	printf("\t 53)SAMPLE_VIO_Vpss_Eis_imx415 \n");
	#endif
	printf("\t 54)SAMPLE_VIO_VI_DeMuxVCRaw_imx464 \n");
#ifdef PROXIMA
	printf("\t 55)SAMPLE_VIO_VI_Three_Chns_Stream_for_9311 \n");
#endif
	printf("\t 56)SAMPLE_VIO_Only_set_sence \n");
	printf("\t 57)SAMPLE_VIO_Only_For_Bind_reset_vin \n");
	printf("\t 58)SAMPLE_VIO_Vpss_imx307_reset_vi \n");
	printf("\t 59)SAMPLE_VIO_imx307_For_Bind_suspend_and_resume \n");
	printf("\t 60)SAMPLE_VIO_Only_For_Bind_With_IMX415_suspend_resueme \n");
	printf("\t 61)SAMPLE_VIO_Bind_imx347 \n");
#ifdef AR_FEAT_EIS
	printf("\t 62)SAMPLE_VIO_Eis_imx415 \n");
#endif
    printf("\t 63)SAMPLE_VIO_Only_imx307_mipi1 \n");
    printf("\t 64)SAMPLE_VIO_Only_Hdr_imx307_mipi1 \n");
    printf("\t 65)SAMPLE_VI_1080p_2200x1125_power_performance_test \n");
#ifdef PROXIMA
    printf("\t 66)SAMPLE_VIO_Only_For_9311_mipi_High_speed_With_IMX415 \n");
    printf("\t 67)SAMPLE_VIO_Only_For_9311_hdr_vc_With_IMX415 \n");
#endif
    printf("\t 69)SAMPLE_VIO_Only_For_Bind_With_SC2210 \n");
    printf("\t 70)SAMPLE_VIO_Only_imx307_with_ISP_FRAME_INFO_S \n");
    printf("\t 71)SAMPLE_VIO_Only_For_Bind_With_OS05A20 \n");
    printf("\t 72)SAMPLE_VIO_Only_For_Bind_With_OS05A20_4M \n");
    printf("\t 73)SAMPLE_VIO_dma_cpy_test \n");
    printf("\t 74)SAMPLE_VIO_multi_processr_two_sensor_p0ch0_p1ch0 \n");
    printf("\t 75)SAMPLE_VIO_Only_imx307_ircutter_led \n");
    printf("\t 76)SAMPLE_VIO_Only_Bind_Get_Raw_from_pleco_sensor \n");
    printf("\t 77)SAMPLE_VIO_Only_Bind_Get_ebd_raw_from_imx307 \n");
    printf("\t 78)SAMPLE_VIO_Only_exp_policy_test: test_mpp_vio -index 78 -exp [0/1/2/3] \n");
    printf("\t 79)SAMPLE_VIO_Only_exp_back_light_test: test_mpp_vio -index 79 -back [0:off/1:compasate/2:suspress] \n");
    printf("\t 80)SAMPLE_VIO_Only_DWDR_test \n");
    printf("\t 81)SAMPLE_VIO_Only_For_Bind_With_OS05A20_4M_3ch: test_mpp_vio -index 81 -size1 [w,h] -size2 [w.h] -dis_anti [1:disable anti/0:enable anti]\n");
	printf("\t 82)SAMPLE_VIO_Only_For_Bind_ISP_IN_use_default_attr \n");
	printf("\t 83)SAMPLE_VIO_Only_use_default_attr \n");
	printf("\t 84)SAMPLE_VIO_Only_start_with_new_tuning_pra_night \n");
	printf("\t 85)SAMPLE_VIO_Only_use_default_attr_cf50 \n");
	printf("\t 86)SAMPLE_VIO_Only_two_sensor_use_default_attr \n");
	printf("\t 87)SAMPLE_VIO_Only_two_ispin_default_attr \n");
	printf("\t 88)SAMPLE_VIO_Only_stop_start_use_default_attr \n");
	printf("\t 89)SAMPLE_VIO_Only_get_3a_info_default_attr \n");
	printf("\t 90)SAMPLE_VIO_gamma_tindy_default_attr \n");
	printf("\t 91)SAMPLE_VIO_ispblank_test_use_default_attr \n");
	printf("\t 92)SAMPLE_VIO_continue_yuvmem_default_attr \n");
	printf("\t 96)SAMPLE_VIO_VI_Three_Chns_Stream_for_9311_crop_ch3 \n");
	printf("\t 97)SAMPLE_VIO_Only_resizech0_default_attr \n");
	printf("\t 98)SAMPLE_VIO_Only_resize_ispout1_default_attr \n");
	printf("\t 99)SAMPLE_VIO_Only_For_Bind_GC2093_suspend_resueme \n");
	printf("\t 100)SAMPLE_VIO_Only_For_Bind_With_OS04A10_suspend_resume \n");
	printf("\t 101)SAMPLE_VIO_Only_For_Bind_With_imx307_suspend_resume \n");

    printf("-vo:\n");
    printf("\t 0) vo HDMI output.default\n");
    printf("\t 1) vo mipi output,\n");

    printf("-no_sig:\n");
    printf("-nframes [n]:\n");
    printf("-cam_mode [n]: 0:online 1:offline 2:multi other:auto\n");
    printf("-dpcm [0/1/2]:0:default 1:enable 2 disable \n");
    printf("--fre -mipi [ipi] -vif [vif] -isp [isp/hdr] -pcs [pcs]\n");
    printf("-settle [settle]: \n");
    printf("-feature_mask [mask:FEA_AR_MPI_ISP_GetVDTimeOut=1<<0,FEA_AR_MPI_ISP_GetAEStatistics=1<<1,FEA_AR_MPI_ISP_GetWBStatistics=1<<2,FEA_AR_MPI_ISP_GetFocusStatistics=1<<3]: \n");
    printf("-load_vi_driver [1/0]:\n");

    return;
}

/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_FAILURE;
    AR_S32 s32pra_index = 0;
    AR_S32 s32Index = 12;
	SAMPLE_VIO_PRA vio_pra = {0};

	vio_pra.s32nframes = -1;
	vio_pra.u32votype = 0;
    ar_log_init();
    ar_always("test test process entry");

    if (argc < 2)
    {
        SAMPLE_VIO_Usage(argv[0]);
        return AR_FAILURE;
    }

	g_s32LoadVinDriver = ar_hal_sys_mpp_service_is_lib();

    vio_pra.argc = argc;
    vio_pra.argv = argv;

	s32pra_index = sample_find_pra_by_name("-no_sig", argc, argv);
    if (s32pra_index > 0){

       ar_printf("not register singal\n");
	}else{

	  ar_printf("register singal SIGINT SIGTERM\n");
      signal(SIGINT, SAMPLE_VIO_HandleSig);
      signal(SIGTERM, SAMPLE_VIO_HandleSig);
	}

	s32pra_index = sample_find_pra_by_name("-index", argc, argv);
	if (s32pra_index > 0)
	{
		s32Index = atoi(argv[s32pra_index+1]);
	}

	s32pra_index = sample_find_pra_by_name("-sensor", argc, argv);
	if (s32pra_index > 0)
	{
		 vio_pra.u32Sensor = atoi(argv[s32pra_index+1]);
	}

	s32pra_index = sample_find_pra_by_name("-vo", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.u32votype = atoi(argv[s32pra_index+1]);
	}

     s32pra_index = sample_find_pra_by_name("-num", argc, argv);
     if (s32pra_index > 0)
     {
         vio_pra.s32Num = atoi(argv[s32pra_index+1]);
     }
     s32pra_index = sample_find_pra_by_name("-cnt", argc, argv);
     if (s32pra_index > 0)
     {
         vio_pra.s32Count = atoi(argv[s32pra_index+1]);
     }
	s32pra_index = sample_find_pra_by_name("-nframes", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.s32nframes = atoi(argv[s32pra_index+1]);
	}
	vio_pra.cam_mode=0xff;
	s32pra_index = sample_find_pra_by_name("-cam_mode", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.cam_mode = atoi(argv[s32pra_index+1]);
	}
	vio_pra.dpcm=0;
	s32pra_index = sample_find_pra_by_name("-dpcm", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.dpcm = atoi(argv[s32pra_index+1]);
	}

   s32pra_index =sample_find_pra_by_name("--fre", argc, argv);
	if (s32pra_index > 0)
	{
		s32pra_index =sample_find_pra_by_name("-mipi", argc, argv);
		if(s32pra_index >0){
		   vio_pra.mipi_fre= atoi(argv[s32pra_index+1]);
		}
	    s32pra_index =sample_find_pra_by_name("-vif", argc, argv);
		if(s32pra_index >0){
		   vio_pra.vif_fre= atoi(argv[s32pra_index+1]);
		}
	    s32pra_index =sample_find_pra_by_name("-isp", argc, argv);
		if(s32pra_index >0){
		   vio_pra.isp_fre= atoi(argv[s32pra_index+1]);
		}
	    s32pra_index =sample_find_pra_by_name("-pcs", argc, argv);
		if(s32pra_index >0){
		   vio_pra.pcs_fre= atoi(argv[s32pra_index+1]);
		}

	}

	vio_pra.settle=0;
	s32pra_index = sample_find_pra_by_name("-settle", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.settle =strtoul(argv[s32pra_index+1],NULL,0);
	}

	vio_pra.feature_mask=0;
	s32pra_index = sample_find_pra_by_name("-feature_mask", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.feature_mask =strtoul(argv[s32pra_index+1],NULL,0);
	}

	s32pra_index = sample_find_pra_by_name("-suspend", argc, argv);
	if (s32pra_index > 0)
	{
		vio_pra.sus_mode = atoi(argv[s32pra_index+1]);
	}

    int load_vi_driver=0;

	s32pra_index = sample_find_pra_by_name("-load_vi_driver", argc, argv);
	if (s32pra_index > 0)
	{
		g_s32LoadVinDriver = atoi(argv[s32pra_index+1]);
	}

    SAMPLE_VIO_MsgInit();

    switch (s32Index)
    {
		case 12:
            s32Ret = SAMPLE_VIO_Only(&vio_pra);
            break;
		case 13:
			s32Ret = SAMPLE_VIO_Only_Hdr(&vio_pra);
            break;
		case 14:
			s32Ret = SAMPLE_VIO_Only_2Ch(&vio_pra);
			break;
		case 15:
			s32Ret = SAMPLE_VIO_Only_For_Bind(&vio_pra);
			break;
		case 16:
			s32Ret = SAMPLE_VIO_multi_process_get_ch_frame(&vio_pra);
			break;
		case 17:
			s32Ret = SAMPLE_VIO_Only_Bind_Get_Pipe_Raw(&vio_pra);
			break;
		case 18:
			s32Ret = SAMPLE_VIO_Only_For_Bind_ISP_IN(&vio_pra);
			break;
		case 19:
			s32Ret = SAMPLE_VIO_Only_For_Bind_imx307_raw_process(&vio_pra);
			break;
		case 20:
		    s32Ret = SAMPLE_VIO_Only_Two_Sensor(&vio_pra);
		    break;
        case 21:
            s32Ret = SAMPLE_VIO_Only_TP9930_DVP_24Ch(&vio_pra);
			break;
#ifdef AR9341
        case 22:
            s32Ret = SAMPLE_VIO_Only_For_Bind_With_IMX415(&vio_pra);
            break;
        case 23:
            s32Ret = SAMPLE_VIO_Only_For_Bind_With_IMX415_60Fps(&vio_pra);
            break;
#endif
	    case 24:
            s32Ret = SAMPLE_VIO_Only_AR_MPI_ISP_GetVDTimeOut(&vio_pra);
            break;
		case 25:
			s32Ret = SAMPLE_VIO_Only_Get3aStats(&vio_pra);
			break;
		case 26:
			s32Ret=SAMPLE_VIO_Only_mipi_2(&vio_pra);
			break;
		case 27:
		    s32Ret = SAMPLE_VIO_Dual_Sensor_With_Hdr_Nohdr(&vio_pra);
		    break;
		case 28:
            s32Ret = SAMPLE_VIO_Only_run_200_frame_exit(&vio_pra);
            break;
	    case 29:
		    s32Ret = SAMPLE_VIO_Three_Sensor_oneimx464_twoimx307(&vio_pra);
		    break;
		case 30:
		    s32Ret = SAMPLE_VIO_Only_YUV420SP(&vio_pra);
		    break;
		case 31:
		    s32Ret = SAMPLE_VIO_Only_IMX307_MIPI_1Ch_AND_TP9930_DVP_4Ch(&vio_pra);
		    break;
		case 32:
			s32Ret = SAMPLE_VIO_Only_For_Bind_With_OS04A10(&vio_pra);
			break;
		case 33:
			s32Ret = SAMPLE_VIO_Only_Hdr_For_Bind_With_OS04A10(&vio_pra);
			break;
        case 34:
            s32Ret = SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Start_Stop(&vio_pra);
			break;
        case 35:
            s32Ret = SAMPLE_VIO_Only_For_Bind_GC2093(&vio_pra);
            break;
        case 36:
            s32Ret = SAMPLE_VIO_Only_For_Bind_GC2093_Hdr(&vio_pra);
            break;
		case 37:
			s32Ret=SAMPLE_VIO_imx307_No_Buffer_test(&vio_pra);
			break;
		case 38:
			s32Ret=SAMPLE_VIO_Only_For_Bind_With_SC530AI(&vio_pra);
			break;
		case 39:
			s32Ret = SAMPLE_VIO_SetFpsRange(&vio_pra);
			break;
        case 40:
            s32Ret = SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug(&vio_pra);
            break;
		case 41:
			s32Ret = SAMPLE_VIO_Only_imx307_crop_api_test(&vio_pra);
			break;
        case 42:
            s32Ret = SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug_UserPic(&vio_pra);
            break;
		case 43:
		    s32Ret = SAMPLE_VIO_Only_Hdr_imx307_mulit_mode(&vio_pra);
			break;
		case 44:
		    s32Ret = SAMPLE_VIO_Only_Two_Sensor_60fps_30fps(&vio_pra);
            break;
        case 45:
            s32Ret = SAMPLE_VIO_Only_imx307_af_fullsweep_test(&vio_pra);
			break;
        case 46:
            s32pra_index = sample_find_pra_by_name("-video", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32CameraMode = atoi(argv[s32pra_index+1]);
            }
            s32pra_index = sample_find_pra_by_name("-res", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32SensorRes = atoi(argv[s32pra_index+1]);
            }
            s32pra_index = sample_find_pra_by_name("-hdr", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32IsHdrOn = 1;
            }
            s32pra_index = sample_find_pra_by_name("-dir", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32IsFromFile = 1;
                vio_pra.pstrDirName = argv[s32pra_index+1];
            }
            s32pra_index = sample_find_pra_by_name("-bigpic", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32Cmd = 5;
            }
            s32pra_index = sample_find_pra_by_name("-preview", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32Cmd = 4;
            }
            s32pra_index = sample_find_pra_by_name("-take", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32Cmd = 3;
                vio_pra.s32nframes = atoi(argv[s32pra_index+1]);
            }
            s32pra_index = sample_find_pra_by_name("-stop", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32Cmd = 2;
            }
            s32pra_index = sample_find_pra_by_name("-server", argc, argv);
            if (s32pra_index > 0)
            {
                vio_pra.s32Cmd = 1;
            }

            s32Ret = SAMPLE_VIO_Big_Pic_Demo(&vio_pra);
            break;
		case 48:
		    s32Ret = SAMPLE_VIO_Vpss_imx307(&vio_pra);
			break;
	    case 49:
			s32Ret = SAMPLE_VIO_Bind_imx307_dig_gain_test(&vio_pra);
			break;
	    case 50:
			s32Ret = SAMPLE_VIO_Only_Test_Interlace(&vio_pra);
			break;
		case 51:
		    s32Ret = SAMPLE_VI_Only(&vio_pra);
			break;
		case 52:
			s32Ret = SAMPLE_VIO_Only_Two_imx307_Sensor_4ch_stop_one_ch(&vio_pra);
			break;

       #ifdef AR_FEAT_EIS
		case 53:
			s32Ret = SAMPLE_VIO_Vpss_Eis_imx415(&vio_pra);
			break;
		#endif
		case 54:
			s32Ret = SAMPLE_VIO_VI_DeMuxVCRaw_imx464(&vio_pra);
			break;
#ifdef PROXIMA
		case 55:
			s32Ret = SAMPLE_VIO_VI_Three_Chns_Stream_for_9311(&vio_pra);
			break;
#endif
		case 56:
			s32Ret = SAMPLE_VIO_Only_set_sence(&vio_pra);
			break;
	    case 57:
			s32Ret = SAMPLE_VIO_Only_For_Bind_reset_vin(&vio_pra);
			break;
		case 58:
			s32Ret=SAMPLE_VIO_Vpss_imx307_reset_vi(&vio_pra);
			break;
		case 59:
			s32Ret = SAMPLE_VIO_imx307_For_Bind_suspend_and_resume(&vio_pra);
			break;
		case 60:
			s32Ret = SAMPLE_VIO_Only_For_Bind_With_IMX415_suspend_resueme(&vio_pra);
			break;
		case 61:
			s32Ret = SAMPLE_VIO_Bind_imx347(&vio_pra);
			break;
		#ifdef AR_FEAT_EIS
		case 62:
		    s32Ret =  SAMPLE_VIO_Eis_imx415(&vio_pra);
			break;
		#endif
		case 63:
			s32Ret = SAMPLE_VIO_Only_imx307_mipi1(&vio_pra);
			break;
	    case 64:
			s32Ret = SAMPLE_VIO_Only_Hdr_imx307_mipi1(&vio_pra);
			break;
		case 65:
			s32Ret = SAMPLE_VI_1080p_2200x1125_power_performance_test(&vio_pra);
			break;
#ifdef PROXIMA
		case 66:
			s32Ret = SAMPLE_VIO_Only_For_9311_mipi_High_speed_With_IMX415(&vio_pra);
			break;
		case 67:
			s32Ret = SAMPLE_VIO_Only_For_9311_hdr_vc_With_IMX415(&vio_pra);
			break;
#endif
		case 68:
			s32Ret = SAMPLE_VIO_Only_2Ch_Plus_Raw(&vio_pra);
			break;
		case 69:
			s32Ret = SAMPLE_VIO_Only_For_Bind_With_SC2210(&vio_pra);
			break;
		case 70:
			s32Ret = SAMPLE_VIO_Only_imx307_with_ISP_FRAME_INFO_S(&vio_pra);
			break;
        case 71:
            s32Ret = SAMPLE_VIO_Only_For_Bind_With_OS05A20(&vio_pra);
            break;
        case 72:
            s32Ret = SAMPLE_VIO_Only_For_Bind_With_OS05A20_4M(&vio_pra);
            break;
	    case 73:
			s32Ret = SAMPLE_VIO_dma_cpy_test(&vio_pra);
			break;
        case 74:
			s32Ret=SAMPLE_VIO_multi_processr_two_sensor_p0ch0_p1ch0(&vio_pra);
			break;
		case 75:
			s32Ret=SAMPLE_VIO_Only_imx307_ircutter_led(&vio_pra);
			break;
		case 76:
			s32Ret = SAMPLE_VIO_Only_Bind_Get_Raw_from_pleco_sensor(&vio_pra);
			break;
		case 77:
			s32Ret = SAMPLE_VIO_Only_Bind_Get_ebd_raw_from_imx307(&vio_pra);
			break;
		case 78:
			s32Ret = SAMPLE_VIO_Only_exp_policy_test(&vio_pra);
			break;
        case 79:
			s32Ret = SAMPLE_VIO_Only_exp_back_light_test(&vio_pra);
			break;
		case 80:
			s32Ret = SAMPLE_VIO_Only_DWDR_test(&vio_pra);
			break;
		case 81:
			s32Ret = SAMPLE_VIO_Only_For_Bind_With_OS05A20_4M_3ch(&vio_pra);
			break;
		case 82:
			s32Ret = SAMPLE_VIO_Only_For_Bind_ISP_IN_use_default_attr(&vio_pra);
			break;
		case 83:
			s32Ret = SAMPLE_VIO_Only_use_default_attr(&vio_pra);
			break;
		case 84:
			s32Ret = SAMPLE_VIO_Only_start_with_new_tuning_pra_night(&vio_pra);
			break;
		case 85:
			s32Ret = SAMPLE_VIO_Only_use_default_attr_cf50(&vio_pra);
			break;
		case 86:
		    s32Ret = SAMPLE_VIO_Only_two_sensor_use_default_attr(&vio_pra);
			break;
        case 87:
			s32Ret = SAMPLE_VIO_Only_two_ispin_default_attr(&vio_pra);
			break;
	    case 88:
			s32Ret = SAMPLE_VIO_Only_stop_start_use_default_attr(&vio_pra);
			break;
	    case 89:
			s32Ret = SAMPLE_VIO_Only_get_3a_info_default_attr(&vio_pra);
			break;
        case 90:
		    s32Ret = SAMPLE_VIO_gamma_tindy_default_attr(&vio_pra);
			break;
		case 91:
			s32Ret = SAMPLE_VIO_ispblank_test_use_default_attr(&vio_pra);
			break;
		case 92:
			s32Ret = SAMPLE_VIO_continue_yuvmem_default_attr(&vio_pra);
			break;
		case 93:
			s32Ret = SAMPLE_VIO_GC2093_Hdr_with_imx307_line(&vio_pra);
			break;
		case 94:
			s32Ret = SAMPLE_VIO_Bind_Get_ebd_raw_from_imx307_use_default_attr(&vio_pra);
			break;
		case 95:
			s32Ret = SAMPLE_VIO_gc2053_linear_and_gc2093_hdr(&vio_pra);
			break;
		case 96:
			s32Ret = SAMPLE_VIO_VI_Three_Chns_Stream_for_9311_crop_ch3(&vio_pra);
			break;
		case 97:
		    s32Ret =  SAMPLE_VIO_Only_resizech0_default_attr(&vio_pra);
		       break;
		case 201:
			s32Ret = sample_vio_imx307_dvpcolorbar(&vio_pra);
			break;
		case 98:
		    s32Ret =  SAMPLE_VIO_Only_resize_ispout1_default_attr(&vio_pra);
		    break;
		case 99:
			s32Ret = SAMPLE_VIO_Only_For_Bind_GC2093_suspend_resueme(&vio_pra);
			break;
		case 100:
			s32Ret = SAMPLE_VIO_Only_For_Bind_With_OS04A10_suspend_resume(&vio_pra);
			break;
	    case 101:
			s32Ret = SAMPLE_VIO_imx307_For_ddr_retention_suspend_and_resume(&vio_pra);
			break;
        case 102:			
		    s32Ret = SAMPLE_VIO_Only_Dual_IMX307_RTD6122C(&vio_pra);
		    break;

        case 103:			
		    s32Ret = SAMPLE_VIO_Only_raw_startsotp_use_default_attr(&vio_pra);
			break;

		case 104:
			s32Ret = SAMPLE_VIO_zoom_use_default_attr(&vio_pra);
			break;
		case 105:
			s32Ret = SAMPLE_VIO_Only_hdmi_raw_LowDelay(&vio_pra);
			break;	
		case 106:
			s32Ret = SAMPLE_VIO_sof_lowdelay(&vio_pra);
		case 107:
			s32Ret = SAMPLE_VI_Only_gn2(&vio_pra);

			break;
        default:
            SAMPLE_PRT("the index %d is invaild!\n",s32Index);
            SAMPLE_VIO_Usage(argv[0]);
            SAMPLE_VIO_MsgExit();
            return AR_FAILURE;
    }

    SAMPLE_VIO_MsgExit();
	//SAMPLE_AR_MPI_VIN_CloseDev();
    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
