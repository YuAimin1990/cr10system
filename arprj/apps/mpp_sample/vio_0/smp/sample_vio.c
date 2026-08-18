#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* End of #ifdef __cplusplus */

#include "../sample_vio.h"
#include "sample_lcd.h"

#include "hal_sys.h"
#include "mpi_sysctl.h"

	// 声明驱动的外部变量，从而可以使用驱动的对象
	extern AR_S32 g_s32LoadVinDriver;
	extern ISP_SNS_OBJ_S stSnsImx290Obj;
	extern ISP_SNS_OBJ_S stSnsIspIn0Obj;
	extern ISP_SNS_OBJ_S stSnsImx290Obj1;
	extern ISP_SNS_OBJ_S stSnsTp9930Obj;
	extern ISP_SNS_OBJ_S stSnsImx415Obj;
	extern ISP_SNS_OBJ_S stSnsImx464Obj;
	extern ISP_SNS_OBJ_S stSnsOs04a10Obj;
	extern ISP_SNS_OBJ_S stSnsGc2093Obj;
	extern ISP_SNS_OBJ_S stSnssc530aiObj;
	extern ISP_SNS_OBJ_S stSnsIspInLObj;
	extern ISP_SNS_OBJ_S stSnsIspInRObj;
	extern ISP_SNS_OBJ_S stSnsMicroiiiObj;
	extern ISP_SNS_OBJ_S stSnsImx347Obj;
	extern ISP_SNS_OBJ_S stSnsGst417wObj;
	extern ISP_SNS_OBJ_S stSnsGst412cObj;
	extern ISP_SNS_OBJ_S stSnsGst612cObj;
	extern ISP_SNS_OBJ_S stSnsGst212w4Obj;
	extern ISP_SNS_OBJ_S stSnsH3812c1sgObj;
	extern ISP_SNS_OBJ_S stSnsRtd6122cObj;
	extern ISP_SNS_OBJ_S stSnsRtd3172cObj;
	extern ISP_SNS_OBJ_S stSnsIspInInfThermalObj;
	extern ISP_SNS_OBJ_S stSnsIspInInfObj;
	extern ISP_SNS_OBJ_S stSnsIspInInfObj1;
	extern ISP_SNS_OBJ_S stSnsSc2210Obj;
	extern ISP_SNS_OBJ_S stSnsOs05a20Obj;
	extern ISP_SNS_OBJ_S stSnsPlecoObj;

	STRU_DISPLAY_OBJ_T g_vo_obj = {0};
	volatile AR_S8 g_exit = 0;

#define AR_ALIGN4(_x) (((_x) + 0x03) & ~0x03)

	typedef struct
	{
		unsigned char type[2];
		unsigned char size[4];
		unsigned char retain[4];
		unsigned char offset[4];
		unsigned char head_len[4];
		unsigned char width[4];
		unsigned char height[4];
		unsigned char planes[2];
		unsigned char bitCount[2];
	} bmp_head_t;

	extern SAMPLE_SNS_TYPE_E g_enSnsType[];

	static STRU_AR_HAL_VO_DSI_ATTR g_dsi_cfg =
		{
			.dev_no = 0,
			.lane = {1, 1, 1, 1},
			.bits_per_pixel = 24,
			.sync_info = {
				.hsa = 6,
				.hbp = 120,
				.hact = 1024,
				.hfp = 120,

				.vsa = 10,
				.vbp = 23,
				.vact = 600,
				.vfp = 12},
			.dphy_clk_mhz = 313.9344,
			.pll_freq_reg_2c0 = 0x32F74FD1,
			.pll_freq_reg_38c = 0x2800};

	AR_S32 sample_find_pra_by_name(AR_CHAR *name, AR_S32 argc, AR_CHAR *argv[])
	{
		AR_S32 i = 0;

		for (i = 0; i < argc; i++)
		{
			if (!strcmp(name, argv[i]))
			{
				SAMPLE_PRT("find pra %s @ %d\n", name, i);
				return i;
			}
		}

		return -1;
	}

	AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode, AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq, AR_S32 mipi_freq)
	{
		VI_DEV_PROP_S Prop = {0};
		// for no need here now, move to sys init

		if (mode == 0)
		{
			Prop.cam_mode = VIN_CAMERA_NORMAL;
		}
		else if (mode == 1)
		{
			Prop.cam_mode = VIN_CAMERA_OFFLINE;
		}
		else if (mode == 2)
		{
			Prop.cam_mode = VIN_CMAERA_MULTI_MODE;
		}
		else
		{
			Prop.cam_mode = VIN_CAMERA_OFFLINE;
		}
		// cfg the fre
		Prop.hdr_fre_mod = 1;
		Prop.hdr_fre_hz = hdr_freq;
		Prop.vif_fre_mod = 1;
		Prop.vif_fre_hz = vif_freq;
		Prop.isp_fre_mod = 1;
		Prop.isp_fre_hz = isp_freq;
		Prop.mipi_fre_mod = 1;
		Prop.mipi_fre_hz = mipi_freq;

		AR_MPI_VIN_OpenDev(&Prop);
		return;
	}

	AR_VOID SAMPLE_AR_MPI_VIN_CloseDev()
	{
		return;
	}

	AR_VOID SAMPLE_VIO_MsgInit(AR_VOID)
	{
	}

	AR_VOID SAMPLE_VIO_MsgExit(AR_VOID)
	{
	}

	void SAMPLE_VIO_HandleSig(AR_S32 signo)
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);

		if (SIGINT == signo || SIGTERM == signo)
		{
			printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
			g_exit = 1;
		}
	}

	static void *algo_get_3a_stats_run(void *param)
	{
		// AR_S32 ref=0;

		STRU_VIO_SAMPLE_GET_3A_STATS_T *run_inf = (STRU_VIO_SAMPLE_GET_3A_STATS_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		uint64_t pts_last[3] = {0};

		ISP_AE_STATISTICS_S *pstAeStat = malloc(sizeof(ISP_AE_STATISTICS_S));
		ISP_WB_STATISTICS_S *pstWBStat = malloc(sizeof(ISP_WB_STATISTICS_S));
		ISP_AF_STATISTICS_S *pstAfStat = malloc(sizeof(ISP_AF_STATISTICS_S));

		if (!pstAeStat || !pstWBStat || !pstAfStat)
		{
			goto end;
		}

		while (1)
		{

			if (run_inf->algo_type == 0)
			{
				AR_MPI_ISP_GetAEStatistics(run_inf->pipe_id, pstAeStat);
				// we sum the aec rro stats
				float sum_r = 0;
				float sum_gr = 0;
				float sum_gb = 0;
				float sum_b = 0;
				int count = pstAeStat->stAecStats.stats_data.mesh_grid_stats_w * pstAeStat->stAecStats.stats_data.mesh_grid_stats_h;
				for (int i = 0; i < count; i++)
				{
					sum_r += pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_r_sum[i] / (float)pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_r_num[i];
					sum_gr += pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_gr_sum[i] / (float)pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_gr_num[i];
					sum_gb += pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_gb_sum[i] / (float)pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_gb_num[i];
					sum_b += pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_b_sum[i] / (float)pstAeStat->stAecStats.stats_data.p_mesh_grid_stats_b_num[i];
				}
				float sum = ((sum_r + sum_gr + sum_gb + sum_b) / 4) / count;
				ar_printf("get aec stats ok ,the luma average is %f pts:%lld deata_pts=%lld \n", sum, pstAeStat->stAecStats.pts_us, pstAeStat->stAecStats.pts_us - pts_last[run_inf->algo_type]);
				pts_last[run_inf->algo_type] = pstAeStat->stAecStats.pts_us;
			}
			else if (run_inf->algo_type == 1)
			{
				AR_MPI_ISP_GetWBStatistics(run_inf->pipe_id, pstWBStat);
				float sum_rg = 0;
				float sum_bg = 0;
				int count = pstWBStat->stAwbStats.mesh_grid_w * pstWBStat->stAwbStats.mesh_grid_h;
				for (int i = 0; i < count; i++)
				{
					float r_avg = pstWBStat->stAwbStats.p_mesh_grid_stats_r_sum[i] / (float)pstWBStat->stAwbStats.p_mesh_grid_stats_num[i];
					float g_avg = pstWBStat->stAwbStats.p_mesh_grid_stats_g_sum[i] / (float)pstWBStat->stAwbStats.p_mesh_grid_stats_num[i];
					float b_avg = pstWBStat->stAwbStats.p_mesh_grid_stats_b_sum[i] / (float)pstWBStat->stAwbStats.p_mesh_grid_stats_num[i];
					sum_rg += r_avg / g_avg;
					sum_bg += b_avg / g_avg;
				}
				ar_printf("get awb stats ok ,rg,bg ==>(%f %f) pts:%lld deta_pts:%lld\n", sum_rg / count, sum_bg / count, pstWBStat->stAwbStats.pts_us, pstWBStat->stAwbStats.pts_us - pts_last[run_inf->algo_type]);
				pts_last[run_inf->algo_type] = pstWBStat->stAwbStats.pts_us;
			}
			else if (run_inf->algo_type == 2)
			{
				AR_MPI_ISP_GetFocusStatistics(run_inf->pipe_id, pstAfStat);
				float sharp_sum = 0;
				float iir_sum = 0;
				float fir_sum = 0;
				int count = pstAfStat->stAfStats.af_stats_data.af_stats_data_total_vertical_block_num * pstAfStat->stAfStats.af_stats_data.af_stats_data_total_horizontal_block_num;
				for (int i = 0; i < count; i++)
				{
					sharp_sum += pstAfStat->stAfStats.af_stats_data.p_af_stats_data_sharpness_addr[i];
					fir_sum += pstAfStat->stAfStats.af_stats_data.p_af_stats_data_fir_addr[i];
					iir_sum += pstAfStat->stAfStats.af_stats_data.p_af_stats_data_iir_addr[i];
				}
				ar_printf("get af stats ok  sharp=%f fir=%f iir=%f\n", sharp_sum / count, fir_sum / count, iir_sum / count);
			}

			if (s32nframes >= 0)
			{
				s32cnt = run_inf->nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					g_exit = 1;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
	end:
		if (pstAeStat)
		{
			free(pstAeStat);
		}
		if (pstWBStat)
		{
			free(pstWBStat);
		}
		if (pstAfStat)
		{
			free(pstAfStat);
		}
		return NULL;
	}

	static void *calibration_thread(void *param)
	{
#define NLANES (4)
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 lane_status[NLANES] = {0}, upper[127] = {0}, lower[127] = {0};
		AR_S32 step[NLANES] = {0}, current[NLANES] = {0}, next[NLANES] = {0}; // 0: OK, 1 not OK
		ar_always("enter calibration thread \n");
		int upper_idx = 0, lower_idx = 0, ref = 0;
		// lane 0
		for (int k = 0; k < 128; k++)
		{
			// for (int i = 0; i < 2; i++)
			// int i = 0;
			int i = run_inf->lane_id;
			{
				// first check status
				if (k == 0)
				{
					do
					{
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						/*if (ref == 5) {
							ref = 0;
							break;
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}

						usleep(16000);*/
					} while (0);
					printf("get lane %d, deskew status = %d \n", i, lane_status[i]);
					if (lane_status[i] >= 2)
					{
						current[i] = 1; // skew
					}
					else
					{
						if (!lane_status[i]) // mask interrupt: no interrupt status
							current[i] = 0;	 // so set it to be   0 as no skew,shoult be 1
						else
							current[i] = 0; // no skew
					}
				}
				// stop the pipe
				AR_MPI_VI_PauseChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);

				// set deskew new value
				AR_MPI_VI_Set_MIPI_Deskew(run_inf->pipe_id, i, step[i]);
				step[i] = (step[i] + 1) % 128;
				// resume the pipe
				usleep(16000);
				AR_MPI_VI_RecoverChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				// check status again
				// need delay some time
				// usleep(60000);
				lane_status[i] = 0;

				do
				{
					ref++;
					AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
					lane_status[i] = lane_status[i] & 0x03;
					// printf("@@deskew status: %d\n", lane_status[i]);
					/*if (ref == 5)
					{
						ref = 0;
						break;
					}
					if (!lane_status[i])
					{
						usleep(16000);
						continue;
					}
					else {
						break;
					}

					usleep(16000);	*/
				} while (0);
				printf("[k = %d]resume get lane %d, deskew status = %d \n", k, i, lane_status[i]);
				if (lane_status[i] >= 2)
					next[i] = 1;
				else
				{
					if (!lane_status[i])
						next[i] = 0;
					else
						next[i] = 0;
				}
				if (current[i] == 0 && next[i] == 1)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i, &upper[upper_idx]);
					upper_idx++;
				}
				else if (current[i] == 1 && next[i] == 0)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i, &lower[lower_idx]);
					lower_idx++;
				}

				current[i] = next[i];
			}
		}

		printf("lane%d lower:\n", run_inf->lane_id);
		// printf("lane%d lower:\n", 0);
		for (int i = 0; i < lower_idx; i++)
			printf("%d ", lower[i]);
		printf("\n");
		// printf("lane%d upper:\n", 0);
		printf("lane%d upper:\n", run_inf->lane_id);
		for (int i = 0; i < upper_idx; i++)
			printf("%d ", upper[i]);
		printf("\n");

		upper_idx = 0;
		lower_idx = 0;
		// lane1
#if 0	
	for(int k = 0; k < 128; k++)
		{
			//for (int i = 0; i < 2; i++)
			int i = 1;
			{
				//first check status
				if (k == 0)
				{
					do {
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						/*if (ref == 5) {
							ref = 0;
							break;				
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}					
						
						usleep(16000);*/
					}while(0);
					printf("get lane %d, deskew status = %d \n", i, lane_status[i]);
					if (lane_status[i] >= 2)
					{
						current[i] = 1; //skew
					}
					else
					{
						if (!lane_status[i])
							current[i] = 1;
						else
							current[i] = 0; // no skew
					}
				}
				//stop the pipe
				AR_MPI_VI_PauseChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				
				//set deskew new value
				AR_MPI_VI_Set_MIPI_Deskew(run_inf->pipe_id, i, step[i]);
				step[i] = (step[i] + 1)%128;
				//resume the pipe
				
				AR_MPI_VI_RecoverChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				//check status again
				//need delay some time 
				//usleep(60000);
				lane_status[i] = 0;
				
				do {
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						//printf("@@deskew status: %d\n", lane_status[i]);
						/*if (ref == 5)
						{
							ref = 0;
							break;
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}
						
						usleep(16000);*/				
				}while(0);
				printf("[k = %d]resume get lane %d, deskew status = %d \n", k, i, lane_status[i]);
				if (lane_status[i] >= 2)
					next[i]=1;
				else {
					if (!lane_status[i])	
						next[i]=1;
					else
						next[i]=0;
				}
				if (current[i] ==0 && next[i] ==1)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i,	&upper[upper_idx]); 							
					upper_idx++;
				}
				else if (current[i] == 1 && next[i] == 0)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i,	&lower[lower_idx]); 						
					lower_idx++;
				}
	
				current[i] = next[i];
			}
		}
	printf("lane%d lower:\n", 1);
	for (int i = 0 ; i < lower_idx; i++)
		printf("%d ", lower[i]);
	printf("\n");
	printf("lane%d upper:\n", 1);
	for (int i = 0 ; i < upper_idx; i++)
		printf("%d ", upper[i]);
	printf("\n");
#endif

		upper_idx = 0;
		lower_idx = 0;
		// lane2
#if 0
	
	for(int k = 0; k < 128; k++)
		{
			//for (int i = 0; i < 2; i++)
			int i = 2;
			{
				//first check status
				if (k == 0)
				{
					do {
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						/*if (ref == 5) {
							ref = 0;
							break;				
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}					
						
						usleep(16000);*/
					}while(0);
					printf("get lane %d, deskew status = %d \n", i, lane_status[i]);
					if (lane_status[i] >= 2)
					{
						current[i] = 1; //skew
					}
					else
					{
						if (!lane_status[i])
							current[i] = 1;
						else
							current[i] = 0; // no skew
					}
				}
				//stop the pipe
				AR_MPI_VI_PauseChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				
				//set deskew new value
				AR_MPI_VI_Set_MIPI_Deskew(run_inf->pipe_id, i, step[i]);
				step[i] = (step[i] + 1)%128;
				//resume the pipe
				
				AR_MPI_VI_RecoverChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				//check status again
				//need delay some time 
				//usleep(60000);
				lane_status[i] = 0;
				
				do {
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						//printf("@@deskew status: %d\n", lane_status[i]);
						/*if (ref == 5)
						{
							ref = 0;
							break;
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}
						
						usleep(16000);*/				
				}while(0);
				printf("[k = %d]resume get lane %d, deskew status = %d \n", k, i, lane_status[i]);
				if (lane_status[i] >= 2)
					next[i]=1;
				else {
					if (!lane_status[i])	
						next[i]=1;
					else
						next[i]=0;
				}
				if (current[i] ==0 && next[i] ==1)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i,	&upper[upper_idx]); 							
					upper_idx++;
				}
				else if (current[i] == 1 && next[i] == 0)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i,	&lower[lower_idx]); 						
					lower_idx++;
				}
	
				current[i] = next[i];
			}
		}
	printf("lane%d lower:\n", 2);
	for (int i = 0 ; i < lower_idx; i++)
		printf("%d ", lower[i]);
	printf("\n");
	printf("lane%d upper:\n", 2);
	for (int i = 0 ; i < upper_idx; i++)
		printf("%d ", upper[i]);
	printf("\n");
#endif
		upper_idx = 0;
		lower_idx = 0;
		// lane3
#if 0 

	for(int k = 0; k < 128; k++)
		{
			//for (int i = 0; i < 2; i++)
			int i = 3;
			{
				//first check status
				if (k == 0)
				{
					do {
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						/*if (ref == 5) {
							ref = 0;
							break;				
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}					
						
						usleep(16000);*/
					}while(0);
					printf("get lane %d, deskew status = %d \n", i, lane_status[i]);
					if (lane_status[i] >= 2)
					{
						current[i] = 1; //skew
					}
					else
					{
						if (!lane_status[i])
							current[i] = 1;
						else
							current[i] = 0; // no skew
					}
				}
				//stop the pipe
				AR_MPI_VI_PauseChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				
				//set deskew new value
				AR_MPI_VI_Set_MIPI_Deskew(run_inf->pipe_id, i, step[i]);
				step[i] = (step[i] + 1)%128;
				//resume the pipe
				
				AR_MPI_VI_RecoverChn(run_inf->pipe_id, run_inf->ch_id, ISP_SUS_SHALLOW_MODE);
				//check status again
				//need delay some time 
				//usleep(60000);
				lane_status[i] = 0;
				
				do {
						ref++;
						AR_MPI_VI_Get_MIPI_Deskew_status(run_inf->pipe_id, i, &lane_status[i]);
						lane_status[i] = lane_status[i] & 0x03;
						//printf("@@deskew status: %d\n", lane_status[i]);
						/*if (ref == 5)
						{
							ref = 0;
							break;
						}
						if (!lane_status[i])
						{
							usleep(16000);
							continue;
						}
						else {
							break;
						}
						
						usleep(16000);	*/			
				}while(0);
				printf("[k = %d]resume get lane %d, deskew status = %d \n", k, i, lane_status[i]);
				if (lane_status[i] >= 2)
					next[i]=1;
				else {
					if (!lane_status[i])	
						next[i]=1;
					else
						next[i]=0;
				}
				if (current[i] ==0 && next[i] ==1)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i,	&upper[upper_idx]); 							
					upper_idx++;
				}
				else if (current[i] == 1 && next[i] == 0)
				{
					AR_MPI_VI_Get_MIPI_Deskew(run_inf->pipe_id, i,	&lower[lower_idx]); 						
					lower_idx++;
				}
	
				current[i] = next[i];
			}
		}
	
	printf("lane%d lower:\n", 3);
	for (int i = 0 ; i < lower_idx; i++)
		printf("%d ", lower[i]);
	printf("\n");
	printf("lane%d upper:\n", 3);
	for (int i = 0 ; i < upper_idx; i++)
		printf("%d ", upper[i]);
	printf("\n");
#endif
		return NULL;
	}

	void *frame_run(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		int fd = -1;
		char name[256];
		if (run_inf->vo_ch_id < 0)
		{
			memset(name, 0, sizeof(name));
			sprintf(name, "mipi_vc%d_pipe%d_chn%d.raw", run_inf->pipe_id, run_inf->pipe_id, run_inf->ch_id);
			fd = open(name, O_RDWR | O_CREAT);
		}
		while (1)
		{
			// SAMPLE_PRT("get frame = %d : pipe_id = %d, ch_id = %d, vo_ch_id = %d\n", ref, run_inf->pipe_id, run_inf->ch_id, run_inf->vo_ch_id);
			int status = AR_MPI_VI_GetChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo, 5000);

			ISP_FRAME_INFO_S *info = (ISP_FRAME_INFO_S *)FrameInfo.stVFrame.stSupplement.pIspInfoVirAddr;

			// ar_printf("pipe_id %d iso:%d \n",run_inf->pipe_id,info->u32ISO);

			if (status)
			{

				if (g_exit)
				{
					break;
				}

				continue;
			}
			if (ref > 1000)
			{
				// SAMPLE_PRT("get frame %lx \n",FrameInfo.stVFrame.u64PhyAddr[0]);
				// ref=0;
			}
			ref++;

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",run_inf->pipe_id,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			// push to display
			if (run_inf->vo_ch_id >= 0)
				send_to_fw_display(&g_vo_obj, run_inf->vo_ch_id, &FrameInfo);
			else
			{
				if (fd > 0 && ref > 30 && ref <= 30 + 1)
				{
					ret = write(fd, FrameInfo.stVFrame.u64VirAddr[0], FrameInfo.stVFrame.u32Len[0]);
					if (ret < 0)
						SAMPLE_PRT("write raw to file error, fd = %d", fd);
					SAMPLE_PRT("pipe_id=%d, frame_len = %d, u64VirAddr = 0x%x", run_inf->pipe_id, FrameInfo.stVFrame.u32Len[0], FrameInfo.stVFrame.u64VirAddr[0]);
				}
			}
			AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = run_inf->nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		ar_printf("frame run pipe_id=%d run_inf=%d exit", run_inf->pipe_id, run_inf->ch_id);
		if (fd > 0)
			close(fd);
		return NULL;
	}

	static void *get_3a_info_run(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		int fd = -1;
		ISP_3A_INFO_TIDY_ATTR_S st3aInfoTidyAttr;
		prctl(PR_SET_NAME, "Get3aInfoRun");

		while (1)
		{

			AR_MPI_ISP_Get3aInfoTidyAttr(run_inf->pipe_id, &st3aInfoTidyAttr);

			ar_delay(1000);

			ar_printf("lux_index=%f luma_avg_r %f %f %f \n", st3aInfoTidyAttr.st3aInfo.aec_info.lux_index,
					  st3aInfoTidyAttr.st3aInfo.aec_info.luma_avg_r, st3aInfoTidyAttr.st3aInfo.aec_info.luma_avg_g, st3aInfoTidyAttr.st3aInfo.aec_info.luma_avg_b);

			if (s32nframes >= 0)
			{
				s32cnt = run_inf->nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		ar_printf("frame run pipe_id=%d run_inf=%d exit", run_inf->pipe_id, run_inf->ch_id);
		return NULL;
	}

	static void *fetch_run(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		VI_PIPE ViPipe = run_inf->pipe_id;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		// read a raw
		int fd = -1;
		ISP_SNS_OBJ_S *p_obj[2] = {&stSnsIspIn0Obj, &stSnsIspIn0Obj};

		int dng_multi = 2;

		if (run_inf->p_attr->stPipeAttr.enBitWidth == DATA_BITWIDTH_8)
		{
			dng_multi = 1;
		}

		int stride = CAM_ALIGNE_TO(run_inf->p_attr->stPubAttr.stSnsSize.u32Width * dng_multi, 256);
		int raw_size = stride * run_inf->p_attr->stPubAttr.stSnsSize.u32Height;

		fd = open(run_inf->name, O_RDONLY);
		if (fd < 0)
		{
			SAMPLE_PRT("raw file not exist %s\n", run_inf->name);
			return NULL;
		}
		int vb_blk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, raw_size, NULL);
		AR_MPI_VB_MmapPool(AR_MPI_VB_Handle2PoolId(vb_blk));
		AR_U64 phy_addr = AR_MPI_VB_Handle2PhysAddr(vb_blk);
		void *p_vaddr = NULL;
		AR_MPI_VB_GetBlockVirAddr(AR_MPI_VB_Handle2PoolId(vb_blk), AR_MPI_VB_Handle2PhysAddr(vb_blk), &p_vaddr);

		SAMPLE_PRT("vb_blk=%d phy_addr=%p  p_vaddr=%p raw_size=%d\n", vb_blk, (void *)phy_addr, p_vaddr, raw_size);

		if (!phy_addr || !p_vaddr)
		{
			ar_err("vb too small");
			return NULL;
			;
		}
		void *p_vaddr_align = NULL;
		AR_U64 phy_addr_align = 0;
		p_vaddr_align = (void *)CAM_ALIGNE_TO((AR_U64)p_vaddr, 256);
		phy_addr_align = CAM_ALIGNE_TO(phy_addr, 256);
		// fill the mem by file
		int read_size = read(fd, p_vaddr_align, raw_size);
		SAMPLE_PRT("fill the raw men with file read_size=0x%x,raw_size=0x%x \n", read_size, raw_size);
		close(fd);
		while (1)
		{
			// send the raw to isp
			VIDEO_FRAME_INFO_S VideoFrame;
			VIDEO_FRAME_INFO_S *pstVideoFrame = &VideoFrame;
			pstVideoFrame->stVFrame.u64PhyAddr[0] = phy_addr_align;
			pstVideoFrame->stVFrame.u64PhyAddr[1] = 0;
			pstVideoFrame->stVFrame.u64PhyAddr[2] = 0;
			pstVideoFrame->stVFrame.u64VirAddr[0] = (AR_U64)p_vaddr_align;
			pstVideoFrame->stVFrame.u64VirAddr[1] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u64VirAddr[2] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u32Stride[0] = stride;
			pstVideoFrame->stVFrame.u32Stride[1] = 0;
			pstVideoFrame->stVFrame.u32Stride[2] = 0;

			pstVideoFrame->stVFrame.u32Width = run_inf->p_attr->stPubAttr.stSnsSize.u32Width;
			pstVideoFrame->stVFrame.u32Height = run_inf->p_attr->stPubAttr.stSnsSize.u32Height;
			pstVideoFrame->stVFrame.u64PTS = 0;
			pstVideoFrame->stVFrame.u64PrivateData = (AR_U64)(NULL);
			pstVideoFrame->u32PoolId = VB_INVALID_POOLID;

			int ret1 = AR_MPI_VI_SendPipeRaw(ViPipe, pstVideoFrame, 200);
			if (ret1 < 0)
			{
				ar_err("send pipe raw failded");
			}
			if (s32nframes >= 0)
			{
				s32cnt = s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
			usleep(30000);
		}
		AR_MPI_VB_ReleaseBlock(vb_blk);
		return NULL;
	}

	AR_S32 SAMPLE_VIO_Only(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{

			if (ref > 60)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// MPLE_COMM_VI_StopVi(&stViConfig);
				// MPLE_COMM_VI_StartVi(&stViConfig);

				// break;
			}
			ref++;

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;
			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_exp_policy_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		int exp_policy_mode = -1;
		int s32ArgIdx = sample_find_pra_by_name("-exp", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			exp_policy_mode = strtol(ppra->argv[s32ArgIdx + 1], NULL, 0);
		}

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 gamma = 0;

		ISP_EXP_POLICY_TIDY_ATTR_S exp_policy;

		if (exp_policy_mode == 0)
		{

#if 1
			exp_policy.stExpPolicy.b_gain_first[0] = 0;
			exp_policy.stExpPolicy.b_gain_first[1] = 1;
			exp_policy.stExpPolicy.b_gain_first[2] = 0;
			exp_policy.stExpPolicy.b_gain_first[3] = 1;
			exp_policy.stExpPolicy.seg_num = 4;

			exp_policy.stExpPolicy.gain[0] = 1.0;
			exp_policy.stExpPolicy.gain[1] = 10;
			exp_policy.stExpPolicy.gain[2] = 20;
			exp_policy.stExpPolicy.gain[3] = 50;
			exp_policy.stExpPolicy.gain[4] = 100;

			exp_policy.stExpPolicy.shutter[0] = 100;
			exp_policy.stExpPolicy.shutter[1] = 1000;
			exp_policy.stExpPolicy.shutter[2] = 10000;
			exp_policy.stExpPolicy.shutter[3] = 30000;
			exp_policy.stExpPolicy.shutter[4] = 40000;
#else
		exp_policy.stExpPolicy.b_gain_first[0] = 1;
		exp_policy.stExpPolicy.seg_num = 1;
		exp_policy.stExpPolicy.gain[0] = 0;
		exp_policy.stExpPolicy.gain[1] = 50;
		exp_policy.stExpPolicy.shutter[0] = 100;
		exp_policy.stExpPolicy.shutter[1] = 40000;
#endif
		}
		else if (exp_policy_mode == 1)
		{
			exp_policy.stExpPolicy.b_gain_first[0] = 0;
			exp_policy.stExpPolicy.seg_num = 1;
			exp_policy.stExpPolicy.gain[0] = 1.0;
			exp_policy.stExpPolicy.gain[1] = 100;

			exp_policy.stExpPolicy.shutter[0] = 10000;
			exp_policy.stExpPolicy.shutter[1] = 10000;
		}
		else if (exp_policy_mode == 2)
		{

			exp_policy.stExpPolicy.b_gain_first[0] = 0;
			exp_policy.stExpPolicy.seg_num = 1;
			exp_policy.stExpPolicy.gain[0] = 10;
			exp_policy.stExpPolicy.gain[1] = 10;

			exp_policy.stExpPolicy.shutter[0] = 1000;
			exp_policy.stExpPolicy.shutter[1] = 40000;
		}
		else if (exp_policy_mode == 3)
		{
			exp_policy.stExpPolicy.seg_num = 1;
			exp_policy.stExpPolicy.b_gain_first[0] = 0;
			exp_policy.stExpPolicy.gain[0] = 10;
			exp_policy.stExpPolicy.gain[1] = 10;
			exp_policy.stExpPolicy.shutter[0] = 10000;
			exp_policy.stExpPolicy.shutter[1] = 10000;
		}
		else
		{
			exp_policy.stExpPolicy.seg_num = 0;
		}

		AR_MPI_ISP_SetExpPolicyTidyAttr(ViPipe, &exp_policy);

		ar_delay(300);

		ISP_EXP_POLICY_TIDY_ATTR_S get_exp_policy;
		AR_MPI_ISP_GetExpPolicyTidyAttr(ViPipe, &get_exp_policy);

		ar_printf("seg =%d \n", get_exp_policy.stExpPolicy.seg_num);
		for (int i = 0; i < get_exp_policy.stExpPolicy.seg_num; i++)
		{
			ar_printf("b_gain_first[%d]=%d \n", i, get_exp_policy.stExpPolicy.b_gain_first[i]);
		}
		for (int i = 0; i < get_exp_policy.stExpPolicy.seg_num + 1; i++)
		{
			ar_printf("gain[%d]=%f \n", i, get_exp_policy.stExpPolicy.gain[i]);
		}
		for (int i = 0; i < get_exp_policy.stExpPolicy.seg_num + 1; i++)
		{
			ar_printf("shutter[%d]=%d \n", i, get_exp_policy.stExpPolicy.shutter[i]);
		}

		ISP_EXPOSURE_ATTR_S *pstExposureAttr = malloc(sizeof(ISP_EXPOSURE_ATTR_S));
		pstExposureAttr->bEnable = 1;
		pstExposureAttr->bManual = 0;

		AR_MPI_ISP_GetExposureAttr(ViPipe, pstExposureAttr);

		for (int i = 0; i < pstExposureAttr->stAutoExposure.exp_table_para.count; i++)
		{
			ar_printf("%d(%f),%d \n", pstExposureAttr->stAutoExposure.exp_table_para.exp_table[i].gain,
					  pstExposureAttr->stAutoExposure.exp_table_para.exp_table[i].gain / 256.0,
					  pstExposureAttr->stAutoExposure.exp_table_para.exp_table[i].line_count);
		}

		free(pstExposureAttr);

		ISP_PIPE_LIMIT_TIDY_ATTR_S ExpLimitTidyAttr = {0};
		AR_MPI_ISP_GetPipeLimitAttr(ViPipe, &ExpLimitTidyAttr);

		ar_printf("gain %f %f shutter %d %d \n", ExpLimitTidyAttr.stLimit.min_gain[0], ExpLimitTidyAttr.stLimit.max_gain[0], ExpLimitTidyAttr.stLimit.min_exp_times_us[0], ExpLimitTidyAttr.stLimit.max_exp_times_us[0]);

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		int bright = 50;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_exp_back_light_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		int exp_policy_mode = -1;
		int s32ArgIdx = sample_find_pra_by_name("-back", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			exp_policy_mode = strtol(ppra->argv[s32ArgIdx + 1], NULL, 0);
		}

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 gamma = 0;

		ISP_EXPOSURE_ATTR_S *pstExposureAttr = malloc(sizeof(ISP_EXPOSURE_ATTR_S));
		pstExposureAttr->bEnable = 1;
		pstExposureAttr->bManual = 0;

		AR_MPI_ISP_GetExposureAttr(ViPipe, pstExposureAttr);

		ar_printf("basic bright dark en info bright_dark_en:%d bright_en:%d dark_en:%d \n",
				  pstExposureAttr->stAutoExposure.pra.bright_dark.bright_dark_en,
				  pstExposureAttr->stAutoExposure.pra.bright_dark.bright_en,
				  pstExposureAttr->stAutoExposure.pra.bright_dark.dark_en);

		STRU_BRIGHT_DRAK_PRA_T black_light_compenstate = {
			.lux_index = 0,
			.dark_th_low = 10,
			.dark_th_high = 20,
			.bright_th_low = 150,
			.bright_th_high = 220,

			.dark_th_low_w = 4,
			.dark_th_high_w = 4,
			.bright_th_low_w = 4,
			.bright_th_high_w = 4,
		};
		STRU_BRIGHT_DRAK_PRA_T strong_light_suppression = {
			.lux_index = 0,
			.dark_th_low = 10,
			.dark_th_high = 20,
			.bright_th_low = 150,
			.bright_th_high = 220,

			.dark_th_low_w = 4,
			.dark_th_high_w = 4,
			.bright_th_low_w = 4,
			.bright_th_high_w = 4,
		};

		pstExposureAttr->stAutoExposure.pra.black_light_compenstate = black_light_compenstate;
		pstExposureAttr->stAutoExposure.pra.strong_light_suppression = strong_light_suppression;
		pstExposureAttr->stAutoExposure.pra.aec_weight_max = 20;
		AR_MPI_ISP_SetExposureAttr(ViPipe, pstExposureAttr);

		free(pstExposureAttr);

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		int strength = 50;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				if (exp_policy_mode == 0)
				{
					ISP_BackLight_TIDY_ATTR_S stBackLightTidyAttr;
					stBackLightTidyAttr.stBackLight.mode = BLACK_LIGHT_MODE_OFF;
					stBackLightTidyAttr.stBackLight.strength = 50;
					AR_MPI_ISP_SetBackLightTidyAttr(ViPipe, &stBackLightTidyAttr);
				}
				else if (exp_policy_mode == 1)
				{
					ISP_BackLight_TIDY_ATTR_S stBackLightTidyAttr;
					stBackLightTidyAttr.stBackLight.mode = BLACK_LIGHT_COMPENSTATE;
					stBackLightTidyAttr.stBackLight.strength = strength++;
					if (strength > 100)
					{
						strength = 0;
					}
					AR_MPI_ISP_SetBackLightTidyAttr(ViPipe, &stBackLightTidyAttr);
				}
				else if (exp_policy_mode == 2)
				{
					ISP_BackLight_TIDY_ATTR_S stBackLightTidyAttr;
					stBackLightTidyAttr.stBackLight.mode = STRONG_LIGHT_SUPPRESSION;
					stBackLightTidyAttr.stBackLight.strength = strength++;
					if (strength > 100)
					{
						strength = 0;
					}
					AR_MPI_ISP_SetBackLightTidyAttr(ViPipe, &stBackLightTidyAttr);
				}

				ISP_BackLight_TIDY_ATTR_S stBackLightTidyAttr;
				AR_MPI_ISP_GetBackLightTidyAttr(ViPipe, &stBackLightTidyAttr);
				ar_printf("%d %d\n", stBackLightTidyAttr.stBackLight.mode, stBackLightTidyAttr.stBackLight.strength);
				ref = 0;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_DWDR_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 gamma = 0;

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		int strength = 50;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ISP_DWDR_STRENGTH_TIDY_ATTR_S StrengthTidyAttr;
				StrengthTidyAttr.u32Strength = strength++;

				if (strength > 100)
					strength = 0;

				AR_MPI_ISP_SetDWDRStrengthTidyAttr(ViPipe, &StrengthTidyAttr);

				ISP_DWDR_STRENGTH_TIDY_ATTR_S get_StrengthTidyAttr;
				AR_MPI_ISP_GetDWDRStrengthTidyAttr(ViPipe, &get_StrengthTidyAttr);

				ar_printf("get u32Strength %d\n", get_StrengthTidyAttr.u32Strength);

				ref = 0;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_imx307_ircutter_led(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		int current = 0;
		int ir_status = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				ISP_IR_CUTTER_STATE_TIDY_ATTR_S ircutter;
				ircutter.u32IrCutterState.status = ir_status;
				ir_status = !ir_status;
				AR_MPI_ISP_SetIrCutterStateTidyAttr(ViPipe, &ircutter);

				ISP_IR_CUTTER_STATE_TIDY_ATTR_S ircutter_get;
				AR_MPI_ISP_GetIrCutterStateTidyAttr(ViPipe, &ircutter_get);
				ar_printf("ir cutrer set to %d and get is %d\n", ircutter.u32IrCutterState.status, ircutter_get.u32IrCutterState.status);

				ISP_LED_STATE_TIDY_ATTR_S led;
				led.u32LedState.status = 1;
				led.u32LedState.led_current[0] = current++;
				AR_MPI_ISP_SetLedStateTidyAttr(ViPipe, &led);
				ISP_LED_STATE_TIDY_ATTR_S led_get;
				AR_MPI_ISP_GetLedStateTidyAttr(ViPipe, &led_get);
				ar_printf("led set current to %d and get is %d\n", led.u32LedState.led_current[0], led_get.u32LedState.led_current[0]);
				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_imx307_with_ISP_FRAME_INFO_S(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		ISP_FRAME_INFO_S *p_isp_info = NULL;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);

				p_isp_info = (ISP_FRAME_INFO_S *)FrameInfo.stVFrame.stSupplement.pIspInfoVirAddr;
				if (p_isp_info)
				{
					ar_printf("u32ExposureTime:%d \n", p_isp_info->u32ExposureTime);
					ar_printf("u32ISO:%d \n", p_isp_info->u32ISO);
					ar_printf("u32HmaxTimes:%d \n", p_isp_info->u32HmaxTimes);
					ar_printf("u64PTS:%lld us\n", FrameInfo.stVFrame.u64PTS);
				}

				ref = 0;

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_imx307_mipi1(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = 1;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = SENSOR0_TYPE;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;

		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Hdr_imx307_mipi1(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		if (SONY_IMX290_MIPI_2M_30FPS_12BIT == stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType)
		{
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1;
		}

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 1;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = 1;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 1;

		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);
		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				ref = 0;

				// break;
			}
			ref++;
			// push to display
			send_to_fw_display(&g_vo_obj, 0, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_set_sence(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 black = 0;

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		ISP_SENSCE_TIDY_ATTR_S sensce = {0};

		ISP_SENSCE_TIDY_ATTR_S sensce_orgi = {0};
		AR_MPI_ISP_GetSensceTidyAttr(ViPipe, &sensce_orgi);
		int switch_ref = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 100)
			{
				SAMPLE_PRT("get frame %lx id=%d \n", FrameInfo.stVFrame.u64PhyAddr[0], FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				if (black == 0)
				{
					switch_ref++;
					ar_printf("switch to night  %d\n", switch_ref);
					strcpy(sensce.stSensce.tuning_name, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_night_dwt_80.bin");
					AR_MPI_ISP_SetSensceTidyAttr(ViPipe, &sensce);
					black = 1;
				}
				else
				{
					switch_ref++;
					ar_printf("switch to night normal %d\n", switch_ref);
					strcpy(sensce.stSensce.tuning_name, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_preview_dwt_80.bin");
					AR_MPI_ISP_SetSensceTidyAttr(ViPipe, &sensce);
					black = 0;
				}

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		AR_MPI_ISP_SetSensceTidyAttr(ViPipe, &sensce_orgi);

		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VI_Only(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				SAMPLE_PRT("get frame %lx id=%d \n", FrameInfo.stVFrame.u64PhyAddr[0], FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// break;
			}
			ref++;

			// push to display

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index;
		AR_S8 s8I2cDev;
		if (ppra->u32Sensor == 0)
		{
			mipi_index = 0;
			s8I2cDev = 0;
		}
		else if (ppra->u32Sensor == 1)
		{
			mipi_index = 1;
			s8I2cDev = 1;
		}
		else
		{
			ar_err("Invalid semsor id");
			return -1;
		}

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			if (ppra->u32Sensor == 0)
			{
				p_obj->pfnGetDefaultAttr(0, &default_attr);
			}
			else if (ppra->u32Sensor == 1)
			{
				p_obj->pfnGetDefaultAttr(1, &default_attr);
			}
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 20;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 20;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		default_attr.stChnAttr.stFrameRate.s32DstFrameRate = 22;
		default_attr.stChnAttr.stFrameRate.s32SrcFrameRate = 22;
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// init display
		if (ppra->u32votype)
		{
			// ar_vo_dev_init_for_mipi(&g_vo_obj,1920,1080,-1,-1, 25, ppra->u32votype, 0);
			ar_vo_dev_init_for_mipi_720_1440(&g_vo_obj, 720, 1440, -1, -1, 60, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		// four layout
		//  AR_S32 mesh_w=1920/2;
		//  AR_S32 mesh_h=1080/2;
		AR_S32 mesh_w = 720;
		AR_S32 mesh_h = 1440;
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		// AR_S32 gamma=0;
		// ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));
		// ISP_MIRROR_STATE_TIDY_ATTR_S *pstMirrorState = malloc(sizeof(ISP_MIRROR_STATE_TIDY_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		// ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthTidyAttr;
		// stDe3dStrengthTidyAttr.u32De3dStrength = 100;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// if(gamma==0)
				//{
				// pstGammaAttr->bEnable=gamma;
				// pstMirrorState->u32MirrorState = gamma;
				// AR_MPI_ISP_SetGammaAttr(ViPipe,pstGammaAttr);
				// AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, pstMirrorState);
				// stDe3dStrengthTidyAttr.u32De3dStrength -=5;
				// AR_MPI_ISP_SetDe3dStrengthTidyAttr(ViPipe, &stDe3dStrengthTidyAttr);
				// gamma=1;
				//}else
				//{
				// pstGammaAttr->bEnable=gamma;
				// pstMirrorState->u32MirrorState = gamma;
				// AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, pstMirrorState);
				// AR_MPI_ISP_SetGammaAttr(ViPipe,pstGammaAttr);
				// ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthTidyAttr1;
				// AR_MPI_ISP_GetDe3dStrengthTidyAttr(ViPipe, &stDe3dStrengthTidyAttr1);
				// printf("de3dstrengh = %d \n", stDe3dStrengthTidyAttr1.u32De3dStrength);
				// gamma=0;
				//}

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_zoom_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		default_attr.stChnAttr.stFrameRate.s32DstFrameRate = 25;
		default_attr.stChnAttr.stFrameRate.s32SrcFrameRate = 25;
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		// set the ch 1

		VI_CHN_ATTR_S stChnAttr = default_attr.stChnAttr;
		stChnAttr.stSize.u32Width = default_attr.stChnAttr.stSize.u32Width / 3;
		stChnAttr.stSize.u32Height = default_attr.stChnAttr.stSize.u32Height / 3;
		AR_MPI_VI_SetChnAttr(ViPipe, 1, &stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, 1);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr = {0};
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = PIC_1080P;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
		stVoConfig.enVoMode = VO_MODE_4MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
		}

		// bind the ch1
		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, 1, 0, 1);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
		}

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;

		int dir = 0;
		float zoom = 1.0;
		float zoom_imp = 1.0;
		while (1)
		{

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			// zoom the image
			RECT_S stCrop;
			stCrop.u32Width = default_attr.stChnAttr.stSize.u32Width / zoom_imp;
			stCrop.u32Height = default_attr.stChnAttr.stSize.u32Height / zoom_imp;
			stCrop.s32X = (default_attr.stChnAttr.stSize.u32Width - stCrop.u32Width) / 2;
			stCrop.s32Y = (default_attr.stChnAttr.stSize.u32Height - stCrop.u32Height) / 2;

			AR_MPI_VI_ReShapeCh(ViPipe, 1, stChnAttr.stSize, stCrop);

			/// usleep(10000);

			if (dir == 0)
			{
				zoom *= 1.001;
				if (zoom >= 36.0)
				{
					zoom = 36.0;
					dir = 1;
				}
			}
			else
			{
				zoom /= 1.001;
				if (zoom <= 1.0)
				{
					zoom = 1.0;
					dir = 0;
				}
			}
			zoom_imp = sqrt(zoom);

			ar_printf("%f \n", zoom_imp);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (g_exit)
			{
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_resizech0_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);

		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr = {0};
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920;
		AR_S32 mesh_h = 1080;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;

		// here we set cnf prop to zoom pra as 4
		ISP_CNF_ATTR_S *pstCnfAttr = malloc(sizeof(ISP_CNF_ATTR_S));
		if (pstCnfAttr)
		{
			pstCnfAttr->bEnable = 1;
			pstCnfAttr->bManual = 0;
			AR_MPI_ISP_GetCnfAttr(ViPipe, pstCnfAttr);

			pstCnfAttr->bEnable = 1;
			pstCnfAttr->bManual = 0;
			pstCnfAttr->stAutoCNF.count_ae = 1;
			pstCnfAttr->stAutoCNF.pra[0].zoom_par = 4;

			AR_MPI_ISP_SetCnfAttr(ViPipe, pstCnfAttr);

			free(pstCnfAttr);
		}

		SIZE_S size_list[] =
			{
				{1920, 1080},
				{1280, 720},
				{960, 540},
				{480, 270},
				{240, 135},
			};

		RECT_S roi_list[] =
			{
				{0, 0, 1920, 1080},
				{44, 180, 1280, 720},
				{44, 270, 960, 540},
				{44, 405, 480, 270},
				{44, 472, 240, 135},
			};

		int size_index = 1;
		int restart_ch = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 200)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				ar_printf("resize to %d %d \n", size_list[size_index].u32Width, size_list[size_index].u32Height);
				AR_MPI_VI_ReShapeCh(ViPipe, ViChn, size_list[size_index], roi_list[size_index]);
				size_index++;
				if (size_index >= sizeof(size_list) / sizeof(size_list[0]))
					size_index = 0;
				// break;
			}
			ref++;

			FrameInfo.stVFrame.u32Width = 1920;
			FrameInfo.stVFrame.u32Height = 1080;
			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (restart_ch++ == 1000)
			{
				AR_MPI_VI_DisableChn(ViPipe, ViChn);
				AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
				AR_MPI_VI_EnableChn(ViPipe, ViChn);
			}

			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_resize_ispout1_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);

		default_attr.stChnAttr.stSize.u32Width = 1916;
		default_attr.stChnAttr.stSize.u32Height = 1080;

		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr = {0};
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920;
		AR_S32 mesh_h = 1080;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;

		SIZE_S size_list[] =
			{
				{1916, 1080},
				{1280, 720},
				{960, 540},
				{640, 480},
				{320, 240},
			};

		RECT_S roi = {0};

		int size_index = 1;

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 200)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				ar_printf("resize to %d %d \n", size_list[size_index].u32Width, size_list[size_index].u32Height);
				AR_MPI_VI_ReShapeCh(ViPipe, ViChn, size_list[size_index], roi);
				size_index++;
				if (size_index >= sizeof(size_list) / sizeof(size_list[0]))
					size_index = 0;
				// break;
			}
			ref++;

			FrameInfo.stVFrame.u32Width = 1920;
			FrameInfo.stVFrame.u32Height = 1080;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_continue_yuvmem_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		default_attr.stChnAttr.s32ContinueBuffer = 1;
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// write the yuv to file by only y
				int fd = open("continue_mem_test.yuv", O_RDWR | O_CREAT);
				if (fd > 0)
				{
					write(fd, FrameInfo.stVFrame.u64VirAddr[0], FrameInfo.stVFrame.u32Stride[0] * default_attr.stChnAttr.stSize.u32Height * 3 / 2);
				}
				else
				{
					ar_err("open file failed");
				}
				close(fd);
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_ispblank_test_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		default_attr.stChnAttr.stFrameRate.s32DstFrameRate = 22;
		default_attr.stChnAttr.stFrameRate.s32SrcFrameRate = 22;
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		default_attr.stPubAttr.stTiming.hblank = default_attr.stPubAttr.stSnsSize.u32Width;
		default_attr.stPubAttr.stTiming.vblank = default_attr.stPubAttr.stSnsSize.u32Height;
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		// AR_S32 gamma=0;
		// ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));
		/// ISP_MIRROR_STATE_TIDY_ATTR_S *pstMirrorState = malloc(sizeof(ISP_MIRROR_STATE_TIDY_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		// ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthTidyAttr;
		// stDe3dStrengthTidyAttr.u32De3dStrength = 100;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// if(gamma==0)
				//{
				// pstGammaAttr->bEnable=gamma;
				// pstMirrorState->u32MirrorState = gamma;
				// AR_MPI_ISP_SetGammaAttr(ViPipe,pstGammaAttr);
				// AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, pstMirrorState);
				// stDe3dStrengthTidyAttr.u32De3dStrength -=5;
				// AR_MPI_ISP_SetDe3dStrengthTidyAttr(ViPipe, &stDe3dStrengthTidyAttr);
				// gamma=1;
				//}else
				//{
				// pstGammaAttr->bEnable=gamma;
				// pstMirrorState->u32MirrorState = gamma;
				// AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, pstMirrorState);
				// AR_MPI_ISP_SetGammaAttr(ViPipe,pstGammaAttr);
				// ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthTidyAttr1;
				// AR_MPI_ISP_GetDe3dStrengthTidyAttr(ViPipe, &stDe3dStrengthTidyAttr1);
				// printf("de3dstrengh = %d \n", stDe3dStrengthTidyAttr1.u32De3dStrength);
				// gamma=0;
				//}

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_gamma_tindy_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		ISP_GAMMA_TIDY_ATTR_S gamma_attr;
		ISP_GAMMA_TIDY_ATTR_S get_gamma_attr;

		gamma_attr.u32Gamma = 50;

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;

				AR_MPI_ISP_GetGammaTidyAttr(ViPipe, &get_gamma_attr);
				ar_printf("get gamma=%d \n", get_gamma_attr.u32Gamma);
				gamma_attr.u32Gamma++;
				if (gamma_attr.u32Gamma > 100)
					gamma_attr.u32Gamma = 0;
				AR_MPI_ISP_SetGammaTidyAttr(ViPipe, &gamma_attr);

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_get_3a_info_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		// here to creat a thread to get 3a info every 1s

		pthread_t ch_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, get_3a_info_run, &pipe0_ch0);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);

		// wait get 3a thead exit
		pthread_join(ch_thread, NULL);

		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_stop_start_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		int first_start = 1;
		STRU_AEC_OUT_T AecOut;
		STRU_AWB_OUT_T AwbOut;
		int stop_start_ref = 0;
		while (1)
		{
			/*start vi*/
			stop_start_ref++;
			ar_printf("stop_start_ref=%d \n", stop_start_ref);
			AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
			default_attr.stComboAttr.devno = mipi_index;
			AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
			AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
			AR_MPI_VI_EnableDev(ViDev);
			VI_DEV_BIND_PIPE_S stDevBindPipe;
			stDevBindPipe.u32Num = 1;
			stDevBindPipe.PipeId[0] = ViPipe;
			AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);

			// set to trigger mode
			default_attr.stPipeAttr.s32Trigger = 1;

			AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
			AR_MPI_VI_StartPipe(ViPipe);
			AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
			AR_MPI_VI_EnableChn(ViPipe, ViChn);

			ALG_LIB_S stAeLib;
			ALG_LIB_S stAwbLib;
			stAeLib.s32Id = ViPipe;
			stAwbLib.s32Id = ViPipe;
			strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
			strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

			if (p_obj->pfnRegisterCallback != AR_NULL)
			{
				s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("sensor_register_callback failed with AR_NULL!\n");
				goto EXIT;
			}

			if (AR_NULL != p_obj->pfnSetBusInfo)
			{
				ISP_SNS_COMMBUS_U uSnsBusInfo;
				uSnsBusInfo.s8I2cDev = s8I2cDev;
				s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("set sensor bus info failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("not support set sensor bus info!\n");
				goto EXIT;
			}
#if 0
        //register vsync and aec update callback to pipe
        ISP_NOTIFY_REGISTER_S NotifyRegister={0};
    	NotifyRegister.stNotify.aec_update=aec_update;
    	NotifyRegister.stNotify.vsync_update=vsync_update;
        AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

			AR_MPI_ISP_MemInit(ViPipe);
			AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
			VI_PIPE_EXT_ATTR_S stPipeAttr;
			AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
			stPipeAttr.bFoucs = 0;
			AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

			AR_MPI_ISP_Init(ViPipe);
			AR_MPI_ISP_Run(ViPipe);

			if (!first_start)
			{
				AR_MPI_ISP_AecUpdate(ViPipe, &AecOut);
				AR_MPI_ISP_AwbUpdate(ViPipe, &AwbOut);
				AR_MPI_ISP_FlushTuningPra(ViPipe);
			}
			first_start = 0;
			// PAUSE();
			AR_MPI_VI_TriggerPipe(ViPipe, 1);
			// get frame buffer
			// usleep(5000000);
			VIDEO_FRAME_INFO_S FrameInfo;
			AR_S32 ref = 0;

			AR_S32 s32cnt = 0;
			AR_S32 s32nframes = 100;
			AR_U64 last_pts = 0;
			s32cnt = s32nframes;
			ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthTidyAttr;
			stDe3dStrengthTidyAttr.u32De3dStrength = 100;
			while (1)
			{
				int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

				if (status)
				{

					if (g_exit)
					{
						break;
					}

					continue;
				}
				ref++;

				// push to display
				send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

				AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
				if (s32cnt > 0)
				{
					if (--s32cnt == 0)
					{
						s32cnt = 0;
						break;
					}
				}
				if (g_exit)
				{
					break;
				}
			}
			// before exit the vin, we save the last aec and awb output,so next start, we wiil the last stop aec awb out as init aec awb and decided the tuning pra
			AR_MPI_ISP_GetAecUpdate(ViPipe, &AecOut);
			AR_MPI_ISP_GetAwbUpdate(ViPipe, &AwbOut);

			// PAUSE();

			AR_MPI_ISP_Exit(ViPipe);
			if (p_obj->pfnUnRegisterCallback != AR_NULL)
			{
				s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				}
			}
			AR_MPI_VI_DisableChn(ViPipe, ViChn);
			AR_MPI_VI_StopPipe(ViPipe);
			AR_MPI_VI_DestroyPipe(ViPipe);
			AR_MPI_VI_DisableDev(ViDev);

			if (g_exit)
			{
				break;
			}
		}

		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_use_default_attr_cf50(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 1;

		int cf50_size = 0;
		default_attr.stChnAttr.enCompressMode = COMPRESS_MODE_SEG;
		AR_MPI_VI_GetChnBufferSize(ViPipe, ViChn, &default_attr.stChnAttr, &cf50_size);

		stVbConf.astCommPool[0].u64BlkSize = cf50_size; // for cf520,more buffer will be malloc
		stVbConf.astCommPool[0].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);

		default_attr.stChnAttr.enCompressMode = COMPRESS_MODE_SEG;
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;

		VI_CH_CF50_CMP_ATTR_T pstChnCmpAttr = {0};
		pstChnCmpAttr.stChCmpAttr.s32OsdCount = 1;
		pstChnCmpAttr.stChCmpAttr.stOsd[0].s32X = 0;
		pstChnCmpAttr.stChCmpAttr.stOsd[0].s32Y = 0;
		pstChnCmpAttr.stChCmpAttr.stOsd[0].u32Width = 4 * 128;
		pstChnCmpAttr.stChCmpAttr.stOsd[0].u32Height = 16 * 16;

		AR_MPI_VI_SetChnCmpAttr(ViPipe, ViChn, &pstChnCmpAttr);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_start_with_new_tuning_pra_night(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
#if 0
    //register vsync and aec update callback to pipe
    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
	NotifyRegister.stNotify.aec_update=aec_update;
	NotifyRegister.stNotify.vsync_update=vsync_update;
    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

		AR_MPI_ISP_MemInit(ViPipe);
		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		strcpy(stPipeAttr.TuningPraBinName, "/usrdata/local/factory/tunning/cam_imx307/imx307_tuning_night_dwt_80.bin");
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				// SAMPLE_PRT("get frame %lx id=%d \n",FrameInfo.stVFrame.u64PhyAddr[0],FrameInfo.stVFrame.u32FrameId);
				ref = 0;

				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		AR_MPI_ISP_Exit(ViPipe);
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_imx307_af_fullsweep_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32FocusModule = 1;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		AR_S32 ref = 0;

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		// set foucs to fullsweep
		ISP_FOCUS_ATTR_S stFocusAttr;
		stFocusAttr.bEnable = 1;
		stFocusAttr.bManual = 0;
		AR_MPI_ISP_GetFocusAttr(ViPipe, &stFocusAttr);

		SAMPLE_PRT("foucus=%d", stFocusAttr.stAutoFocus.enable);

		stFocusAttr.bEnable = 1;
		stFocusAttr.bManual = 0;
		stFocusAttr.stAutoFocus.algo_type = FOCUS_FULL_SWEEP;

		AR_MPI_ISP_SetFocusAttr(ViPipe, &stFocusAttr);

		// trigger focus
		AR_MPI_ISP_Trigger_Focus(ViPipe);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx id=%d \n", FrameInfo.stVFrame.u64PhyAddr[0], FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_imx307_crop_api_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 1;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Width = 1280;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Height = 720;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Width,
									stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Height, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Width,
						   stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Height, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, ViChn, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;

		// first get the crop info
		VI_CROP_INFO_S CropInfo;

		AR_MPI_VI_GetChnCrop(ViPipe, ViChn, &CropInfo);

		ar_always("bEnable=%d enCropCoordinate=%d  roi:%d %d %d %d", CropInfo.bEnable, CropInfo.enCropCoordinate,
				  CropInfo.stCropRect.s32X, CropInfo.stCropRect.s32Y, CropInfo.stCropRect.u32Width, CropInfo.stCropRect.u32Height);

		// set the crop to crop

		CropInfo.bEnable = 1;
		CropInfo.enCropCoordinate = VI_CROP_ABS_COOR;
		CropInfo.stCropRect.s32X = 960 / 2;
		CropInfo.stCropRect.s32Y = 540 / 2,
		CropInfo.stCropRect.u32Width = 960;
		CropInfo.stCropRect.u32Height = 540;

		AR_MPI_VI_SetChnCrop(ViPipe, ViChn, &CropInfo);

		AR_MPI_VI_GetChnCrop(ViPipe, ViChn, &CropInfo);

		ar_always("bEnable=%d enCropCoordinate=%d  roi:%d %d %d %d", CropInfo.bEnable, CropInfo.enCropCoordinate,
				  CropInfo.stCropRect.s32X, CropInfo.stCropRect.s32Y, CropInfo.stCropRect.u32Width, CropInfo.stCropRect.u32Height);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",ViPipe,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			if (ref > 10)
			{
				SAMPLE_PRT("get frame %lx id=%d \n", FrameInfo.stVFrame.u64PhyAddr[0], FrameInfo.stVFrame.u32FrameId);
				ref = 0;
				// break;s
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, ViChn);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_SetFpsRange(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		int mod = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 10)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				mod++;

				ref = 0;
				// break;
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_imx307_No_Buffer_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		VIDEO_FRAME_INFO_S saveFrameInfo[15];
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		if (s32nframes == -1)
		{
			s32nframes = 200;
		}
		AR_S32 id = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					break;
				}
				continue;
			}
			else // success get frame
			{
				// push to display
				SAMPLE_PRT("get frame successfully \n");
				send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);
				saveFrameInfo[ref] = FrameInfo;
				ref++;
			}

			SAMPLE_PRT("get frame id = %d \n", id);
			if (s32nframes >= 0)
			{
				s32cnt = s32nframes;
				s32nframes = -1;
			}
			id++;
			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					g_exit = 1;
				}
			}
			else
			{
				g_exit = 1;
			}

			if (g_exit)
			{
				break;
			}
		}
		g_exit = 0;
		for (int k = 0; k < ref; k++)
		{
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &saveFrameInfo[k]);
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 write_yuv_data_to_file(FILE *fp, VIDEO_FRAME_INFO_S *pstFrameInfo, int format)
	{

		AR_S32 ret = 0;
		if (fp == NULL)
		{
			SAMPLE_PRT("file point null\n");
		}

		if (format == PIXEL_FORMAT_YVU_SEMIPLANAR_420)
		{
			// two pannel
			int i, j = 0;
			// SAMPLE_PRT("frame w = %d, h = %d\n", pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height);
			// SAMPLE_PRT("pannel[0] stride = %d,  pannel[1] stride= %d\n", pstFrameInfo->stVFrame.u32Stride[0], pstFrameInfo->stVFrame.u32Stride[1]);
			// write y
			for (i = 0; i < pstFrameInfo->stVFrame.u32Height; i++)
			{
				ret = fwrite((void *)pstFrameInfo->stVFrame.u64VirAddr[0] + i * pstFrameInfo->stVFrame.u32Stride[0], pstFrameInfo->stVFrame.u32Stride[0], 1, fp);
				if (ret < 1)
					ar_err("fwrite Y line error");
			}
			// write uv
			for (j = 0; j < pstFrameInfo->stVFrame.u32Height / 2; j++)
			{
				ret = fwrite((void *)pstFrameInfo->stVFrame.u64VirAddr[1] + j * pstFrameInfo->stVFrame.u32Stride[1], pstFrameInfo->stVFrame.u32Stride[1], 1, fp);
				if (ret < 1)
					ar_err("fwrite uv error");
			}
		}
		else
		{
			// todo
		}
		return ret;
	}

	AR_S32 SAMPLE_VIO_Only_YUV420SP(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420; // yuv420 sp
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
		s32WorkSnsId = 0;
		g_enSnsType[s32WorkSnsId] = SENSOR12_TYPE;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR12_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;
#if 1
		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
#if 0
    //four layout
    AR_S32 mesh_w=1920;
    AR_S32 mesh_h=1080;
    //init display
	ar_vo_dev_init(&g_vo_obj,1920,1080,-1,-1);
	STRU_AR_HAL_VO_RECT chan_pos={0};
	chan_pos.x=0;
	chan_pos.y=0;
	chan_pos.w=mesh_w;
	chan_pos.h=mesh_h;
	ar_vo_surface_init(&g_vo_obj,0,&chan_pos);
#endif
		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 gamma = 0;
		ISP_GAMMA_ATTR_S *pstGammaAttr = malloc(sizeof(ISP_GAMMA_ATTR_S));
		ISP_MIRROR_STATE_TIDY_ATTR_S *pstMirrorState = malloc(sizeof(ISP_MIRROR_STATE_TIDY_ATTR_S));

		AR_MPI_ISP_GetGammaAttr(ViPipe, pstGammaAttr);
		FILE *fp = NULL;
		fp = fopen("./isp_yuv420sp.yuv", "w");
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		if (s32nframes == -1)
		{
			s32nframes = 2;
		}
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					break;
				}
				continue;
			}
			if (ref > 100)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// ref=0;
				if (gamma == 0)
				{
					pstGammaAttr->bEnable = gamma;
					pstMirrorState->u32MirrorState = gamma;
					// AR_MPI_ISP_SetGammaAttr(ViPipe,pstGammaAttr);
					// AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, pstMirrorState);
					gamma = 1;
				}
				else
				{
					pstGammaAttr->bEnable = gamma;
					pstMirrorState->u32MirrorState = gamma;
					// AR_MPI_ISP_SetMirrorStateTidyAttr(ViPipe, pstMirrorState);
					// AR_MPI_ISP_SetGammaAttr(ViPipe,pstGammaAttr);
					gamma = 0;
				}
				// break;
			}
			ref++;
// push to display
#if 0
		 send_to_fw_display(&g_vo_obj,ViChn,&FrameInfo);
#else
			if (ref > 1)
				write_yuv_data_to_file(fp, &FrameInfo, PIXEL_FORMAT_YVU_SEMIPLANAR_420);
#endif

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
#if 0
		 if (ref >= 10)
		 {
            SAMPLE_PRT("capture %d frame\n", ref);
		    break;
		 }
#endif
			if (s32nframes >= 0)
			{
				s32cnt = s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					g_exit = 1;
				}
			}
			else
			{
				g_exit = 1;
			}

			if (g_exit)
			{
				break;
			}
		}
		g_exit = 0;
#if 0
	close_display_surface(&g_vo_obj,0);
	close_display(&g_vo_obj);
#endif
		SAMPLE_COMM_VI_StopVi(&stViConfig);
		fclose(fp);
#else
	while (!g_exit)
	{
		sleep(1);
	}
#endif
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_run_200_frame_exit(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN			   VoChn		  = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes = 200;

		while (1)
		{

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 100)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// ref=0;

				// break;
			}
			ref++;
			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_mipi_2(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 1;
		VI_PIPE ViPipe = 2;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 2;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 3;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;

		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 100)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				ref = 0;
				// break;
			}
			ref++;
			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	static int SAMPLE_VIO_bmp_image_data_get(char *file_name, char **vrt_addr,
											 int *width, int *height, int *bitCnt)
	{
		bmp_head_t *info = NULL;
		char *bmp_tmp_data = NULL;
		int ret;
		int bitCount, offset;
		ssize_t cnt;
		int stride = 0;

		printf("bmp_image_data_get\r\n");
		int fd = open(file_name, O_RDWR);
		if (fd <= 0)
		{
			printf("open img file fail\r\n");
			ret = -1;
			goto End;
		}

		size_t len = sizeof(bmp_head_t);
		info = (bmp_head_t *)malloc(len);
		if (NULL == info)
		{
			printf("dma malloc info fail\r\n");
			ret = -1;
			goto End_Fd;
		}

		//    printf("malloc buffer, len=%d\r\n", len);
		cnt = read(fd, (char *)info, len);
		if (cnt != len)
		{
			printf("read bmp header error (%ld, %ld)\r\n", cnt, len);
			ret = -2;
			goto End_Mem;
		}

		*width = info->width[0] + (info->width[1] << 8) + (info->width[2] << 16) + (info->width[3] << 24);
		*height = info->height[0] + (info->height[1] << 8) + (info->height[2] << 16) + (info->height[3] << 24);
		offset = info->offset[0] + (info->offset[1] << 8) + (info->offset[2] << 16) + (info->offset[3] << 24);
		bitCount = info->bitCount[0] + (info->bitCount[1] << 8);
		*bitCnt = bitCount;
		printf("bmp info width = %d, height = %d, offset = %d, bitCount = %d\r\n",
			   *width, *height, offset, bitCount);

		if ((24 != bitCount) && (32 != bitCount))
		{
			printf("format wrong, only support RGB888 format\r\n");
			ret = -3;
			goto End_Mem;
		}

		bmp_tmp_data = (char *)malloc(offset - sizeof(bmp_head_t));
		if (NULL == bmp_tmp_data)
		{
			printf("ar_malloc bmp tmp fail\r\n");
			ret = -4;
			goto End_Mem;
		}

		len = offset - sizeof(bmp_head_t);
		cnt = read(fd, bmp_tmp_data, len);
		if (cnt != len)
		{
			printf("read tmp data error (%ld, %ld)\r\n", cnt, len);
			ret = -5;
			goto End_Mem;
		}

		stride = AR_ALIGN4(bitCount / 8 * (*width));
		len = stride * (*height);

		printf("bmp data stride: %d, len: %ld\r\n", stride, len);
		*vrt_addr = (char *)malloc(len);
		if (NULL == *vrt_addr)
		{
			printf("ar_malloc bmp data fail, len = %ld\r\n", len);
			ret = -6;
			goto End_Mem;
		}

		cnt = read(fd, *vrt_addr, len);
		if (cnt != len)
		{
			printf("read image data error (%ld, %ld)\r\n", cnt, len);
			ret = -7;
			free((unsigned char *)*vrt_addr);
			goto End_Mem;
		}

		//    printf("phy addr:%p, vrt addr: %p", *phy_addr, *vrt_addr);

		ret = 0;

	End_Mem:
		if (info)
			free((unsigned char *)info);

		if (bmp_tmp_data)
			free((unsigned char *)bmp_tmp_data);

	End_Fd:
		close(fd);

	End:
		return ret;
	}

	int SAMPLE_VIO_region_vi_attach_test(char *file_name, AR_U32 rgn_id)
	{
		RGN_ATTR_S attr = {0};
		int ret;
		printf("[%s,%d] mpi region_%d init\r\n", __func__, __LINE__, rgn_id);

		// get image data
		char *vrt_addr = NULL;
		int width = 0, height = 0, bitCnt = 0;
		SAMPLE_VIO_bmp_image_data_get(file_name, &vrt_addr, &width, &height, &bitCnt);
		/* init region 0 format gray 1*/
		attr.enType = OVERLAY_RGN;
		attr.unAttr.stOverlay.u32BgColor = 0;
		attr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_888;
		attr.unAttr.stOverlay.stSize.u32Width = width;
		attr.unAttr.stOverlay.stSize.u32Height = height;
		attr.unAttr.stOverlay.u32CanvasNum = MAX_REGION_BUFFER;
		ret = AR_MPI_RGN_Create(rgn_id, &attr);
		if (ret != 0)
		{
			printf("[%s,%d] region_%d create failed!", __func__, __LINE__, rgn_id);
			goto End;
		}

		MPP_CHN_S chn_id;
		AR_S32 ViPipe = 0;
		chn_id.s32ChnId = 0;
		chn_id.s32DevId = ViPipe;
		chn_id.enModId = AR_ID_VI;

		RGN_CHN_ATTR_S chn_attr;
		memset(&chn_attr, 0, sizeof(RGN_CHN_ATTR_S));
		chn_attr.bShow = AR_TRUE;
		chn_attr.enType = OVERLAY_RGN;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = AR_TRUE;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 0;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 0;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 0;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = LESSTHAN_LUM_THRESH;
		chn_attr.unChnAttr.stOverlayChn.u32Layer = 0;
		chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = 0;
		chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
		chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = 0 + rgn_id * width;
		;
		chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = 0 + rgn_id * height;
		ret = AR_MPI_RGN_AttachToChn(rgn_id, &chn_id, &chn_attr);
		if (ret != 0)
		{
			printf("[%s,%d] can't update region_%d's canvas!", __func__, __LINE__, rgn_id);
			goto End;
		}

		RGN_CANVAS_INFO_S canvas = {0};
		ret = AR_MPI_RGN_GetCanvasInfo(rgn_id, &canvas);
		if (ret != 0)
		{
			printf("[%s,%d] region create failed!", __func__, __LINE__);
			goto End;
		}

		// copy the rgb888 image to canvas
		for (int i = 0; i < height; i++)
		{
			memcpy((void *)(canvas.u64VirtAddr + canvas.u32Stride * i), vrt_addr + i * width * 3, width);
		}
		/* update canvas */
		ret = AR_MPI_RGN_UpdateCanvas(rgn_id);
		if (ret != 0)
		{
			printf("[%s,%d] can't update the canvas!\r\n", __func__, __LINE__);
			goto End;
		}
	End:
		free(vrt_addr);
		return ret;
	}

	AR_S32 SAMPLE_VIO_VI_Three_Chns_Stream_for_9311(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn[3] = {0, 1, 3};
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn[0];
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = ViChn;

		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, ViChn[0], &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		for (int i = 1; i < 3; i++)
		{
			if (i == 1)
			{
				stChnAttr.stSize.u32Width = stSize.u32Width / 2;
				stChnAttr.stSize.u32Height = stSize.u32Height / 2;
			}
			else
			{
				stChnAttr.stSize.u32Width = 640;
				stChnAttr.stSize.u32Height = 480;
			}

			s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn[i], &stChnAttr);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT;
			}

			s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn[i]);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT;
			}
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_4MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		for (int i = 0; i < 3; i++)
		{
			s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn[i], 0, i);

			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
				goto EXIT2;
			}
		}
		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn[0], &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn[0], &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		PAUSE();
		for (int i = 0; i < 3; i++)
			SAMPLE_COMM_VI_UnBind_VO(0, ViChn[i], 0, i);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
	AR_S32 SAMPLE_VIO_VI_Three_Chns_Stream_for_9311_crop_ch3(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn[3] = {0, 1, 3};
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn[0];
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = ViChn;

		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, ViChn[0], &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		for (int i = 1; i < 3; i++)
		{
			if (i == 1)
			{
				stChnAttr.stSize.u32Width = stSize.u32Width / 2;
				stChnAttr.stSize.u32Height = stSize.u32Height / 2;
			}
			else
			{
				stChnAttr.stSize.u32Width = 640;
				stChnAttr.stSize.u32Height = 480;
			}

			s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn[i], &stChnAttr);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT;
			}

			s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn[i]);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT;
			}
		}

		// first get the crop info
		VI_CROP_INFO_S CropInfo;

		AR_MPI_VI_GetChnCrop(ViPipe, 3, &CropInfo);

		ar_always("bEnable=%d enCropCoordinate=%d  roi:%d %d %d %d", CropInfo.bEnable, CropInfo.enCropCoordinate,
				  CropInfo.stCropRect.s32X, CropInfo.stCropRect.s32Y, CropInfo.stCropRect.u32Width, CropInfo.stCropRect.u32Height);

		// set the crop to crop

		CropInfo.bEnable = 1;
		CropInfo.enCropCoordinate = VI_CROP_ABS_COOR;
		CropInfo.stCropRect.s32X = 960 / 2;
		CropInfo.stCropRect.s32Y = 540 / 2,
		CropInfo.stCropRect.u32Width = 960;
		CropInfo.stCropRect.u32Height = 540;

		AR_MPI_VI_SetChnCrop(ViPipe, 3, &CropInfo);

		AR_MPI_VI_GetChnCrop(ViPipe, 3, &CropInfo);

		ar_always("bEnable=%d enCropCoordinate=%d  roi:%d %d %d %d", CropInfo.bEnable, CropInfo.enCropCoordinate,
				  CropInfo.stCropRect.s32X, CropInfo.stCropRect.s32Y, CropInfo.stCropRect.u32Width, CropInfo.stCropRect.u32Height);

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_4MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		for (int i = 0; i < 3; i++)
		{
			s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn[i], 0, i);

			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
				goto EXIT2;
			}
		}
		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn[0], &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn[0], &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		PAUSE();
		for (int i = 0; i < 3; i++)
			SAMPLE_COMM_VI_UnBind_VO(0, ViChn[i], 0, i);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			uint64_t start = ar_get_timestamp_us();
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			// ar_printf("deta:%d \n",(ar_get_timestamp_us()-start)/1000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);
	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_reset_vin(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		int reset_vin_ref = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
			reset_vin_ref++;
			if (reset_vin_ref > 200)
			{
				SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);
				SAMPLE_COMM_VI_StopVi(&stViConfig);
				SAMPLE_COMM_VI_StartVi(&stViConfig);
				SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);
				reset_vin_ref = 0;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Bind_Get_ebd_raw_from_imx307_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsImx290Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;
		VI_PIPE ViPipe[4] = {0, 1, -1, -1};
		VI_CHN ViChn[4] = {2, 2, 0, 0};
		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;
		struct arPipeInfo
		{
			AR_U32 u32VCNum[4];
			AR_U32 u32Width[4];
			AR_U32 u32Height[4];
			AR_U32 u32BitWidth[4];
			AR_U32 u32VCDTNum[4];
			AR_U32 u32enPixFormat[4];
			AR_FLOAT f32Fps[4];
			AR_BOOL bVcNumCfg;
			AR_BOOL bVcDTCfg;
		} stPipeInfo;

		struct arChnInfo
		{
			PIXEL_FORMAT_E enPixFormat[5];
			VIDEO_FORMAT_E enVideoFormat;
			COMPRESS_MODE_E enCompressMode;
			AR_U32 u32Width[5];
			AR_U32 u32Height[5];
		} stChnInfo;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr.stPubAttr.stSnsSize;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[0].u32BlkCnt = 5;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		VI_DEV_PROP_S prop = {0};
		/*if(ppra->cam_mode>2){
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);
		}else{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);
		}*/

		prop.cam_mode = ppra->cam_mode;
		if (prop.cam_mode > VIN_CMAERA_TOOL_SIMULATION)
			prop.cam_mode = VIN_CAMERA_OFFLINE;
		prop.hdr_fre_mod = 1;
		prop.hdr_fre_hz = ppra->isp_fre;
		prop.vif_fre_mod = 1;
		prop.vif_fre_hz = ppra->vif_fre;
		prop.isp_fre_mod = 1;
		prop.isp_fre_hz = ppra->isp_fre;
		prop.mipi_fre_mod = 1;
		prop.mipi_fre_hz = ppra->pcs_fre;

		AR_MPI_VIN_OpenDev(&prop);

		if (ppra->settle > 0)
		{
			default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		default_attr.stComboAttr.devno = mipi_index;
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		// one device two pipe,
		memset(&stDevBindPipe, 0, sizeof(VI_DEV_BIND_PIPE_S));
		for (int i = 0; i < 2; i++)
		{
			stDevBindPipe.u32Num++;
			stDevBindPipe.PipeId[i] = ViPipe[i];
		}
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);

		stPipeInfo.u32BitWidth[0] = DATA_BITWIDTH_12;
		stPipeInfo.u32BitWidth[1] = DATA_BITWIDTH_8;
		stPipeInfo.u32enPixFormat[0] = PIXEL_FORMAT_RGB_BAYER_12BPP;
		stPipeInfo.u32enPixFormat[1] = PIXEL_FORMAT_RGB_BAYER_8BPP;
		stPipeInfo.u32Width[0] = stSize.u32Width;
		stPipeInfo.u32Height[0] = stSize.u32Height;
		stPipeInfo.u32Width[1] = 230;
		stPipeInfo.u32Height[1] = 1;
		stPipeInfo.u32VCDTNum[0] = 0x002c;
		stPipeInfo.u32VCDTNum[1] = 0x0012;
		stPipeInfo.u32VCNum[0] = 0;
		stPipeInfo.u32VCNum[1] = 1;
		stPipeInfo.bVcDTCfg = AR_TRUE;
		stPipeInfo.bVcNumCfg = AR_TRUE;

		// pipe
		for (int i = 0; i < 2; i++)
		{
			default_attr.stPipeAttr.enBitWidth = stPipeInfo.u32BitWidth[i];
			default_attr.stPipeAttr.enPixFmt = stPipeInfo.u32enPixFormat[i];
			default_attr.stPipeAttr.u32MaxH = stPipeInfo.u32Height[i];
			default_attr.stPipeAttr.u32MaxW = stPipeInfo.u32Width[i];
			if (ppra->dpcm == 1)
			{
				default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
				default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			}
			else if (ppra->dpcm == 2)
			{
				default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
				default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
			}

			if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
			{
				default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
			}

			if (ppra->feature_mask)
			{
				default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
			}

			AR_MPI_VI_CreatePipe(ViPipe[i], &default_attr.stPipeAttr);
			if (AR_TRUE == stPipeInfo.bVcNumCfg)
			{
				s32Ret = AR_MPI_VI_SetPipeVCNumber(ViPipe[i], stPipeInfo.u32VCNum[i]);
				if (s32Ret != AR_SUCCESS)
				{
					AR_MPI_VI_DestroyPipe(ViPipe[i]);
					SAMPLE_PRT("AR_MPI_VI_SetPipeVCNumber failed with %#x!\n", s32Ret);
					return AR_FAILURE;
				}
			}

			if (AR_TRUE == stPipeInfo.bVcDTCfg)
			{
				s32Ret = AR_MPI_VI_SetPipeDTNumber(ViPipe[i], stPipeInfo.u32VCDTNum[i]);
				if (s32Ret != AR_SUCCESS)
				{
					AR_MPI_VI_DestroyPipe(ViPipe[i]);
					SAMPLE_PRT("AR_MPI_VI_SetPipeDTNumber failed with %#x!\n", s32Ret);
					return AR_FAILURE;
				}
			}

			s32Ret = AR_MPI_VI_StartPipe(ViPipe[i]);
			if (s32Ret != AR_SUCCESS)
			{
				AR_MPI_VI_DestroyPipe(ViPipe[i]);
				SAMPLE_PRT("AR_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
				return AR_FAILURE;
			}
		}
		// chn
		stChnInfo.enPixFormat[0] = stPipeInfo.u32enPixFormat[0];
		stChnInfo.u32Width[0] = stPipeInfo.u32Width[0];
		stChnInfo.u32Height[0] = stPipeInfo.u32Height[0];
		stChnInfo.enPixFormat[1] = stPipeInfo.u32enPixFormat[1];
		stChnInfo.u32Width[1] = stPipeInfo.u32Width[1];
		stChnInfo.u32Height[1] = stPipeInfo.u32Height[1];

		for (int i = 0; i < 2; i++)
		{
			default_attr.stChnAttr.stFrameRate.s32DstFrameRate = 30;
			default_attr.stChnAttr.stFrameRate.s32SrcFrameRate = 30;
			default_attr.stChnAttr.enPixelFormat = stChnInfo.enPixFormat[i];
			default_attr.stChnAttr.stSize.u32Width = stChnInfo.u32Width[i];
			default_attr.stChnAttr.stSize.u32Height = stChnInfo.u32Height[i];

			AR_MPI_VI_SetChnAttr(ViPipe[i], ViChn[i], &default_attr.stChnAttr);
			AR_MPI_VI_EnableChn(ViPipe[i], ViChn[i]);
		}

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		for (int i = 0; i < 2; i++)
		{
			stAeLib.s32Id = ViPipe[i];
			stAwbLib.s32Id = ViPipe[i];
			strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
			strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

			if (p_obj->pfnRegisterCallback != AR_NULL)
			{
				s32Ret = p_obj->pfnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("sensor_register_callback failed with AR_NULL!\n");
				goto EXIT;
			}

			if (AR_NULL != p_obj->pfnSetBusInfo)
			{
				ISP_SNS_COMMBUS_U uSnsBusInfo;
				uSnsBusInfo.s8I2cDev = s8I2cDev;
				s32Ret = p_obj->pfnSetBusInfo(ViPipe[i], uSnsBusInfo);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("set sensor bus info failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("not support set sensor bus info!\n");
				goto EXIT;
			}
#if 0
	    //register vsync and aec update callback to pipe
	    ISP_NOTIFY_REGISTER_S NotifyRegister={0};
		NotifyRegister.stNotify.aec_update=aec_update;
		NotifyRegister.stNotify.vsync_update=vsync_update;
	    AR_MPI_ISP_RegisterNotifyCallBack(ViPipe,&NotifyRegister);
#endif

			AR_MPI_ISP_MemInit(ViPipe[i]);
			default_attr.stPubAttr.stSnsSize.u32Width = stPipeInfo.u32Width[i];
			default_attr.stPubAttr.stSnsSize.u32Height = stPipeInfo.u32Height[i];
			default_attr.stPubAttr.u8SnsMode = 75; // SONY_IMX290_MIPI_2M_30FPS_12BIT_VC_RAW
			AR_MPI_ISP_SetPubAttr(ViPipe[i], &default_attr.stPubAttr);
			VI_PIPE_EXT_ATTR_S stPipeAttr;
			AR_MPI_VI_GetPipeExtAttr(ViPipe[i], &stPipeAttr);
			stPipeAttr.bFoucs = 0;
			AR_MPI_VI_SetPipeExtAttr(ViPipe[i], &stPipeAttr);

			AR_MPI_ISP_Init(ViPipe[i]);
			AR_MPI_ISP_Run(ViPipe[i]);
		}

		// get frame buffer
		pthread_t ch_thread = {0};
		pthread_t ch1_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 2;
		pipe0_ch0.vo_ch_id = -1;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);
#if 1
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
		pipe0_ch1.pipe_id = 1;
		pipe0_ch1.ch_id = 2;
		pipe0_ch1.vo_ch_id = -1;
		pipe0_ch1.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe0_ch1);
#endif
		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Bind_Get_ebd_raw_from_imx307(SAMPLE_VIO_PRA *ppra)
	{
		SAMPLE_VIO_Bind_Get_ebd_raw_from_imx307_use_default_attr(ppra);
		return 0;
	}

	AR_S32 SAMPLE_VIO_Only_Bind_Get_Raw_from_pleco_sensor(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 2;
		VI_PIPE ViPipe[4] = {0, 1, -1, -1};
		VI_CHN ViChn[4] = {2, 2, 0, 0};
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN			   VoChn		  = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_RGB_BAYER_10BPP;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		ar_err("this case only for pleco_sensor of aobi custormer,sdk test may be failed becuase we have no test sensor");

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
		g_enSnsType[s32WorkSnsId] = SENSOR27_TYPE;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[0];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = ViPipe[1];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = ViPipe[2];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = ViPipe[3];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_TRUE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_TRUE;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 3;

		for (int i = 0; i < 2; i++)
		{
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32VCNum[i] = 0;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32VCDTNum[i] = i;
		}

		{
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32BitWidth[0] = DATA_BITWIDTH_10;
			// stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32BitWidth[0] = DATA_BITWIDTH_8;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32BitWidth[1] = DATA_BITWIDTH_8;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32enPixFormat[0] = PIXEL_FORMAT_RGB_BAYER_10BPP;
			// stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32enPixFormat[0] = PIXEL_FORMAT_RGB_BAYER_8BPP;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32enPixFormat[1] = PIXEL_FORMAT_RGB_BAYER_8BPP;
		}
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn[0];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR27_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			printf("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			printf("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[0].u32BlkCnt = 5;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		// set two pipe w, h
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Width[0] = stSize.u32Width;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Width[1] = stSize.u32Width;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Height[0] = stSize.u32Height;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Height[1] = 2 * 6;
		// stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Height[0]= 2 * 6;
		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

// AR_MPI_VI_Set_MIPI_Deskew(0, 0, 30);
#if 0
    VI_CHN_ATTR_S      stChnAttr;
    /*start vi chn1*/
    s32Ret = AR_MPI_VI_GetChnAttr(ViPipe[1], ViChn[1], &stChnAttr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT;
    }
    stChnAttr.stSize.u32Width = stSize.u32Width;
    stChnAttr.stSize.u32Height = 12;
    stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_8BPP;
    s32Ret = AR_MPI_VI_SetChnAttr(ViPipe[1], ViChn[1], &stChnAttr);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT;
    }

    s32Ret = AR_MPI_VI_EnableChn(ViPipe[1], ViChn[1]);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT;
    }
#endif
		// SAMPLE_COMM_ISP_Run(ViPipe[1]);
		pthread_t ch_thread = {0};
		pthread_t ch1_thread = {0};
		pthread_t ch2_thread = {0};
		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 2;
		pipe0_ch0.vo_ch_id = -1;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);
#if 0
	STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
	pipe0_ch1.pipe_id=1;
	pipe0_ch1.ch_id=2;
	pipe0_ch1.vo_ch_id=-1;
	pipe0_ch1.nframes = ppra->s32nframes;
	pthread_create(&ch1_thread, pstAttr, frame_run, &pipe0_ch1);
#endif
		if (ppra->u32votype)
		{
			sleep(20);
			pthread_create(&ch2_thread, pstAttr, calibration_thread, &pipe0_ch0);
			pthread_join(ch2_thread, NULL);
		}
		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		SAMPLE_COMM_ISP_Stop(ViPipe[1]);
		SAMPLE_COMM_VI_StopVi(&stViConfig);

	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	static void *suspend_resume_thread(AR_CHAR *cmd, AR_S32 len, void *arg)
	{
		STRU_VIO_SAMPLE_FRAME_RUN_T *info = (STRU_VIO_SAMPLE_FRAME_RUN_T *)arg;
		STRU_CMD_ARG_T *cmd_arg = ar_hal_sys_get_args(cmd);
		char ch;
		struct timeval start;
		struct timeval end;
		int s32pra_index = 0;
		s32pra_index = ar_hal_sys_find_pra_index_by_name(cmd_arg, "-suspend");

		if (s32pra_index > 0)
		{
			// gettimeofday(&start, NULL);
			AR_MPI_VI_PauseChn(info->pipe_id, info->ch_id, info->suspend);
			// ar_always("resume spend time = %dms \n",
			// end.tv_sec * 1000 - start.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_usec / 1000);
		}

		s32pra_index = ar_hal_sys_find_pra_index_by_name(cmd_arg, "-resume");

		if (s32pra_index > 0)
		{
			AR_MPI_VI_RecoverChn(info->pipe_id, info->ch_id, info->suspend);
		}
	}

	MPI_SYSCTL_HANDLE g_phandle = NULL;
	void ddr_retention_suspend_and_resume_cb(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
	{
		int ret;
		int flag = -1;
		AR_U64 u64Tic = 0;
		struct timeval tm_start;
		struct timeval tm_end;
		STRU_VIO_SAMPLE_FRAME_RUN_T info;
		info.pipe_id = 0;
		info.ch_id = 0;
		info.suspend = 2;

		if (SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
			SYSCTL_EVENT_SUSPEND == eSysctlEvent)
		{
			gettimeofday(&tm_start, NULL);
			ret = AR_MPI_RGN_Suspend();
			ret = AR_MPI_VO_Suspend(0);
			ret = AR_MPI_VI_PauseChn(info.pipe_id, info.ch_id, info.suspend);
			gettimeofday(&tm_end, NULL);
			if (ret)
				printf("suspend failed, ret:%d!\n", ret);
			else
				printf("suspend success!\n");

			flag = 1;
		}
		else if (SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
				 SYSCTL_EVENT_RESUME == eSysctlEvent)
		{
			// for gpio power level switch by set register, no need now
			AR_MPI_ISP_SetRegister(info.pipe_id, 0x0a10a000, 0x46);
			gettimeofday(&tm_start, NULL);
			ret = AR_MPI_VO_Resume(0);
			ret = AR_MPI_RGN_Resume();

			AR_MPI_VI_RecoverChn(info.pipe_id, info.ch_id, info.suspend);
			gettimeofday(&tm_end, NULL);
			if (ret)
				printf("resume failed, ret:%d!\n", ret);
			else
				printf("resume success!\n");

			flag = 0;
		}
		else
		{
			printf("fault sysctl event:%d\n", eSysctlEvent);
			return;
		}

		ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
		if (ret)
			printf("event SYSCTL_Event_done fail!\n");
		else
			printf("event SYSCTL_Event_done!\n");

		if (-1 != flag)
		{
			u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
			printf(" %s takes %f ms\n", flag == 1 ? "SUSPEND" : "RESUME", u64Tic * 1.0 / 1000);
		}
	}

	AR_S32 SAMPLE_VIO_imx307_For_ddr_retention_suspend_and_resume(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enTrigger = 1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		AR_MPI_VI_TriggerPipe(ViPipe, 1);
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pipe0_ch0.suspend = ppra->sus_mode;
		g_phandle = AR_MPI_SYSCTL_Register("vin_ddr_retention", 0, ddr_retention_suspend_and_resume_cb);
		if (!g_phandle)
		{
			printf("register sysctl failed!\n");
			return -1;
		}
		printf("register sysctl success!\n");
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);
		s32Ret = AR_MPI_SYSCTL_Unregister(g_phandle);
		if (s32Ret)
		{
			printf("unregister sysctl  failed!\n");
		}
	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_imx307_For_Bind_suspend_and_resume(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enTrigger = 1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		AR_MPI_VI_TriggerPipe(ViPipe, 1);

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pipe0_ch0.suspend = ppra->sus_mode;
		BINDER_CMD_T *binder_cmd = NULL;
		binder_cmd = ar_hal_sys_register_binder_cmd("resume_cmd", suspend_resume_thread, &pipe0_ch0);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		ar_hal_sys_unregister_binder_cmd(binder_cmd);

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_AR_MPI_ISP_GetVDTimeOut(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = 0xff;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			s32Ret = AR_MPI_ISP_GetVDTimeOut(ViPipe, ISP_VD_FE_START, 200);

			if (s32Ret)
			{
				ar_printf("ISP_VD_FE_START timeout \n");
			}
			else
			{
				ar_printf("ISP_VD_FE_START ok \n");
			}

			AR_MPI_ISP_GetVDTimeOut(ViPipe, ISP_VD_FE_END, 200);
			if (s32Ret)
			{
				ar_printf("ISP_VD_FE_END timeout \n");
			}
			else
			{
				ar_printf("ISP_VD_FE_END ok \n");
			}

			AR_MPI_ISP_GetVDTimeOut(ViPipe, ISP_VD_BE_END, 200);
			if (s32Ret)
			{
				ar_printf("ISP_VD_BE_END timeout \n");
			}
			else
			{
				ar_printf("ISP_VD_BE_END ok \n");
			}

			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

#define AF_OPEN

	AR_S32 SAMPLE_VIO_Only_Get3aStats(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32FocusModule = 1;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}

		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		ISP_FOCUS_ATTR_S stFocusAttr;
		stFocusAttr.bEnable = 1;
		stFocusAttr.bManual = 0;
		AR_MPI_ISP_GetFocusAttr(ViPipe, &stFocusAttr);

		SAMPLE_PRT("foucus=%d", stFocusAttr.stAutoFocus.enable);

		stFocusAttr.bEnable = 1;
		stFocusAttr.bManual = 0;
		stFocusAttr.stAutoFocus.algo_type = FOCUS_FULL_SWEEP;

		AR_MPI_ISP_SetFocusAttr(ViPipe, &stFocusAttr);

		pthread_t aec_thread;
		pthread_t awb_thread;
		pthread_t af_thread;

		STRU_VIO_SAMPLE_GET_3A_STATS_T aec_obj;
		STRU_VIO_SAMPLE_GET_3A_STATS_T awb_obj;
		STRU_VIO_SAMPLE_GET_3A_STATS_T af_obj;

		aec_obj.pipe_id = ViPipe;

		pthread_attr_t *pstAttr = NULL;
		aec_obj.pipe_id = ViPipe;
		aec_obj.algo_type = 0;
		aec_obj.nframes = ppra->s32nframes;
		pthread_create(&aec_thread, pstAttr, algo_get_3a_stats_run, &aec_obj);

		awb_obj.pipe_id = ViPipe;
		awb_obj.algo_type = 1;
		awb_obj.nframes = ppra->s32nframes;
		pthread_create(&awb_thread, pstAttr, algo_get_3a_stats_run, &awb_obj);
#ifdef AF_OPEN
		af_obj.pipe_id = ViPipe;
		af_obj.algo_type = 2;
		af_obj.nframes = ppra->s32nframes;
		pthread_create(&af_thread, pstAttr, algo_get_3a_stats_run, &af_obj);
#endif
		VIDEO_FRAME_INFO_S FrameInfo;
		int s32nframes = ppra->s32nframes;
		int s32cnt = 0;
		while (1)
		{

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					break;
				}
				continue;
			}
			ar_printf("frame pts=%lld \n", FrameInfo.stVFrame.u64PTS);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}

		pthread_join(aec_thread, NULL);
		pthread_join(awb_thread, NULL);
#ifdef AF_OPEN
		pthread_join(af_thread, NULL);

#endif

		g_exit = 0;

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	static AR_VOID vsync_update(VI_PIPE ViPipe)
	{
		SAMPLE_PRT("ViPipe =%d \n", ViPipe);
	}
	static AR_VOID aec_update(VI_PIPE ViPipe, STRU_AEC_OUT_T *aec_out)
	{
		SAMPLE_PRT("ViPipe =%d lux_index=%d\n", ViPipe, aec_out->lux_index);
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_ISP_IN(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 4;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR1_TYPE;

		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		SAMPLE_PRT("enSnsType=%d \n", stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		SAMPLE_PRT("pool0 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[0].u64BlkSize, stVbConf.astCommPool[0].u32BlkCnt);

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		SAMPLE_PRT("pool1 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[1].u64BlkSize, stVbConf.astCommPool[1].u32BlkCnt);

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi_Step1(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// register vsync and aec update callback to pipe
		ISP_NOTIFY_REGISTER_S NotifyRegister = {0};
		NotifyRegister.stNotify.aec_update = aec_update;
		NotifyRegister.stNotify.vsync_update = vsync_update;
		AR_MPI_ISP_RegisterNotifyCallBack(ViPipe, &NotifyRegister);

		s32Ret = SAMPLE_COMM_VI_StartVi_Step2(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		SAMPLE_PRT("push a raw to isp \n");

		// read a raw
		int fd = open("/local/imx307.raw", O_RDONLY);
		int raw_size = 1920 * 2 * 1080;
		if (fd < 0)
		{
			SAMPLE_PRT("raw file not exist\n");
			goto EXIT2;
		}
		int vb_blk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, raw_size, NULL);
		AR_MPI_VB_MmapPool(AR_MPI_VB_Handle2PoolId(vb_blk));
		AR_U64 phy_addr = AR_MPI_VB_Handle2PhysAddr(vb_blk);
		void *p_vaddr = NULL;
		AR_MPI_VB_GetBlockVirAddr(AR_MPI_VB_Handle2PoolId(vb_blk), AR_MPI_VB_Handle2PhysAddr(vb_blk), &p_vaddr);

		SAMPLE_PRT("vb_blk=%d phy_addr=%p  p_vaddr=%p raw_size=%d\n", vb_blk, (void *)phy_addr, p_vaddr, raw_size);

		if (!phy_addr || !p_vaddr)
		{
			ar_err("vb too small");
			goto EXIT2;
			;
		}
		void *p_vaddr_align = NULL;
		AR_U64 phy_addr_align = 0;
		p_vaddr_align = (void *)CAM_ALIGNE_TO((AR_U64)p_vaddr, 256);
		phy_addr_align = CAM_ALIGNE_TO(phy_addr, 256);
		// fill the mem by file
		int read_size = read(fd, p_vaddr_align, raw_size);
		SAMPLE_PRT("fill the raw men with file read_size=0x%x,raw_size=0x%x \n", read_size, raw_size);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			// send the raw to isp
			VIDEO_FRAME_INFO_S VideoFrame;
			VIDEO_FRAME_INFO_S *pstVideoFrame = &VideoFrame;
			pstVideoFrame->stVFrame.u64PhyAddr[0] = phy_addr_align;
			pstVideoFrame->stVFrame.u64PhyAddr[1] = 0;
			pstVideoFrame->stVFrame.u64PhyAddr[2] = 0;
			pstVideoFrame->stVFrame.u64VirAddr[0] = (AR_U64)p_vaddr_align;
			pstVideoFrame->stVFrame.u64VirAddr[1] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u64VirAddr[2] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u32Stride[0] = 1920 * 2;
			pstVideoFrame->stVFrame.u32Stride[1] = 0;
			pstVideoFrame->stVFrame.u32Stride[2] = 0;

			pstVideoFrame->stVFrame.u32Width = 1920;
			pstVideoFrame->stVFrame.u32Height = 1080;
			pstVideoFrame->stVFrame.u64PTS = 0;
			pstVideoFrame->stVFrame.u64PrivateData = (AR_U64)(NULL);
			pstVideoFrame->u32PoolId = VB_INVALID_POOLID;

			int ret1 = AR_MPI_VI_SendPipeRaw(ViPipe, pstVideoFrame, 200);
			if (ret1 < 0)
			{
				while (1)
				{
					usleep(3000000000);
				}
			}
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
			usleep(30000);
		}

		PAUSE();

		SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, 0, VoChn);
		if (fd > 0)
			close(fd);
		if (vb_blk > 0)
			AR_MPI_VB_ReleaseBlock(vb_blk);
	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_ISP_IN_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj = &stSnsIspIn0Obj;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 4;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 mipi_index = 0;
		AR_S8 s8I2cDev = 1;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		int w = 1920;
		int h = 1080;

		if (p_obj->pfnGetDefaultAttr)
		{
			p_obj->pfnGetDefaultAttr(0, &default_attr);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		default_attr.stPubAttr.stSnsSize.u32Width = w;
		default_attr.stPubAttr.stSnsSize.u32Height = h;

		stSize = default_attr.stPubAttr.stSnsSize;
		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		SAMPLE_PRT("pool0 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[0].u64BlkSize, stVbConf.astCommPool[0].u32BlkCnt);

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		SAMPLE_PRT("pool1 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[1].u64BlkSize, stVbConf.astCommPool[1].u32BlkCnt);

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		default_attr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		default_attr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		if (ppra->dpcm == 1)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}
		else if (ppra->dpcm == 3)
		{
			default_attr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			default_attr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}

		if (default_attr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			default_attr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			default_attr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		/*start vi*/
		AR_MPI_VI_SetMipiBindDev(ViDev, mipi_index);
		AR_MPI_VI_SetComboDevAttr(&default_attr.stComboAttr);
		AR_MPI_VI_SetDevAttr(ViDev, &default_attr.stDevAttr);
		AR_MPI_VI_EnableDev(ViDev);
		VI_DEV_BIND_PIPE_S stDevBindPipe;
		stDevBindPipe.u32Num = 1;
		stDevBindPipe.PipeId[0] = ViPipe;
		AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
		AR_MPI_VI_CreatePipe(ViPipe, &default_attr.stPipeAttr);
		AR_MPI_VI_StartPipe(ViPipe);
		default_attr.stChnAttr.stSize.u32Width = w;
		default_attr.stChnAttr.stSize.u32Height = h;
		AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &default_attr.stChnAttr);
		AR_MPI_VI_EnableChn(ViPipe, ViChn);

		ALG_LIB_S stAeLib;
		ALG_LIB_S stAwbLib;
		stAeLib.s32Id = ViPipe;
		stAwbLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
		strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

		if (p_obj->pfnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("sensor_register_callback failed with AR_NULL!\n");
			goto EXIT;
		}

		if (AR_NULL != p_obj->pfnSetBusInfo)
		{
			ISP_SNS_COMMBUS_U uSnsBusInfo;
			uSnsBusInfo.s8I2cDev = s8I2cDev;
			s32Ret = p_obj->pfnSetBusInfo(ViPipe, uSnsBusInfo);

			if (s32Ret != AR_SUCCESS)
			{
				ar_err("set sensor bus info failed with %#x!\n", s32Ret);
				goto EXIT;
			}
		}
		else
		{
			ar_err("not support set sensor bus info!\n");
			goto EXIT;
		}
		// register vsync and aec update callback to pipe
		ISP_NOTIFY_REGISTER_S NotifyRegister = {0};
		NotifyRegister.stNotify.aec_update = aec_update;
		NotifyRegister.stNotify.vsync_update = vsync_update;
		AR_MPI_ISP_RegisterNotifyCallBack(ViPipe, &NotifyRegister);

		AR_MPI_ISP_MemInit(ViPipe);
		default_attr.stPubAttr.stSnsSize.u32Width = w;
		default_attr.stPubAttr.stSnsSize.u32Height = h;
		default_attr.stPubAttr.stWndRect.s32X = 0;
		default_attr.stPubAttr.stWndRect.s32Y = 0;
		default_attr.stPubAttr.stWndRect.u32Width = w;
		default_attr.stPubAttr.stWndRect.u32Height = h;
		default_attr.stPubAttr.stTiming.hblank = 200;
		default_attr.stPubAttr.stTiming.vblank = 100;
		default_attr.stPubAttr.enBayer = AR_BAYER_GBRG;

		AR_MPI_ISP_SetPubAttr(ViPipe, &default_attr.stPubAttr);
		VI_PIPE_EXT_ATTR_S stPipeAttr;
		AR_MPI_VI_GetPipeExtAttr(ViPipe, &stPipeAttr);
		stPipeAttr.bFoucs = 0;
		AR_MPI_VI_SetPipeExtAttr(ViPipe, &stPipeAttr);

		AR_MPI_ISP_Init(ViPipe);
		AR_MPI_ISP_Run(ViPipe);

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = PIC_1080P;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		SAMPLE_PRT("push a raw to isp \n");

		// read a raw
		int fd = open("/local/imx307.raw", O_RDONLY);
		int raw_stride = CAM_ALIGNE_TO(w * 2, 256);
		int raw_size = raw_stride * h;
		if (fd < 0)
		{
			SAMPLE_PRT("raw file not exist\n");
			goto EXIT2;
		}
		int vb_blk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, raw_size, NULL);
		AR_MPI_VB_MmapPool(AR_MPI_VB_Handle2PoolId(vb_blk));
		AR_U64 phy_addr = AR_MPI_VB_Handle2PhysAddr(vb_blk);
		void *p_vaddr = NULL;
		AR_MPI_VB_GetBlockVirAddr(AR_MPI_VB_Handle2PoolId(vb_blk), AR_MPI_VB_Handle2PhysAddr(vb_blk), &p_vaddr);

		SAMPLE_PRT("vb_blk=%d phy_addr=%p  p_vaddr=%p raw_size=%d\n", vb_blk, (void *)phy_addr, p_vaddr, raw_size);

		if (!phy_addr || !p_vaddr)
		{
			ar_err("vb too small");
			goto EXIT2;
			;
		}
		void *p_vaddr_align = NULL;
		AR_U64 phy_addr_align = 0;
		p_vaddr_align = (void *)CAM_ALIGNE_TO((AR_U64)p_vaddr, 256);
		phy_addr_align = CAM_ALIGNE_TO(phy_addr, 256);
		// fill the mem by file
		int read_size = read(fd, p_vaddr_align, raw_size);
		SAMPLE_PRT("fill the raw men with file read_size=0x%x,raw_size=0x%x \n", read_size, raw_size);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			// send the raw to isp
			VIDEO_FRAME_INFO_S VideoFrame;
			VIDEO_FRAME_INFO_S *pstVideoFrame = &VideoFrame;
			pstVideoFrame->stVFrame.u64PhyAddr[0] = phy_addr_align;
			pstVideoFrame->stVFrame.u64PhyAddr[1] = 0;
			pstVideoFrame->stVFrame.u64PhyAddr[2] = 0;
			pstVideoFrame->stVFrame.u64VirAddr[0] = (AR_U64)p_vaddr_align;
			pstVideoFrame->stVFrame.u64VirAddr[1] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u64VirAddr[2] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u32Stride[0] = raw_stride;
			pstVideoFrame->stVFrame.u32Stride[1] = 0;
			pstVideoFrame->stVFrame.u32Stride[2] = 0;

			pstVideoFrame->stVFrame.u32Width = w;
			pstVideoFrame->stVFrame.u32Height = h;
			pstVideoFrame->stVFrame.u64PTS = 0;
			pstVideoFrame->stVFrame.u64PrivateData = (AR_U64)(NULL);
			pstVideoFrame->u32PoolId = VB_INVALID_POOLID;

			int ret1 = AR_MPI_VI_SendPipeRaw(ViPipe, pstVideoFrame, 200);
			if (ret1 < 0)
			{
				while (1)
				{
					usleep(3000000000);
				}
			}
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
			usleep(30000);
		}
		SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, 0, VoChn);
		if (fd > 0)
			close(fd);
		if (vb_blk > 0)
			AR_MPI_VB_ReleaseBlock(vb_blk);
	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		AR_MPI_ISP_Exit(ViPipe);
		if (p_obj->pfnUnRegisterCallback != AR_NULL)
		{
			s32Ret = p_obj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
			}
		}
		AR_MPI_VI_DisableChn(ViPipe, ViChn);
		AR_MPI_VI_StopPipe(ViPipe);
		AR_MPI_VI_DestroyPipe(ViPipe);
		AR_MPI_VI_DisableDev(ViDev);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_two_ispin_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj[2] = {&stSnsIspIn0Obj, &stSnsIspIn0Obj};
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr[2];
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};
		VI_CHN ViChn[2] = {0, 0};
		AR_S32 mipi_index[2] = {0, 1};
		AR_S8 s8I2cDev[2] = {1, 2};

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj[0]->pfnGetDefaultAttr)
		{
			p_obj[0]->pfnGetDefaultAttr(0, &default_attr[0]);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		// 按照需求重新设置sensor的分辨率，bayer 格式
		default_attr[0].stPubAttr.enBayer = AR_BAYER_RGGB;
		default_attr[0].stPubAttr.stSnsSize.u32Width = 1920;
		default_attr[0].stPubAttr.stSnsSize.u32Height = 1080;
		// 修改bit depth
		default_attr[0].stPipeAttr.enBitWidth = DATA_BITWIDTH_12;

		stSize = default_attr[0].stPubAttr.stSnsSize;
		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 0;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 16 * 1024;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 4;
		stVbConf.u32MaxPoolCnt++;

		// malloc camera 2 vb
		if (p_obj[1]->pfnGetDefaultAttr)
		{
			p_obj[1]->pfnGetDefaultAttr(1, &default_attr[1]);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		// 按照需求重新设置sensor的分辨率，bayer 格式
		default_attr[1].stPubAttr.enBayer = AR_BAYER_BGGR;
		default_attr[1].stPubAttr.stSnsSize.u32Width = 960;
		default_attr[1].stPubAttr.stSnsSize.u32Height = 1280;
		// 修改bit depth
		default_attr[1].stPipeAttr.enBitWidth = DATA_BITWIDTH_8;

		stSize = default_attr[1].stPubAttr.stSnsSize;
		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 16 * 1024;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 4;
		stVbConf.u32MaxPoolCnt++;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 300000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		for (int i = 0; i < 2; i++)
		{
			if (ppra->settle > 0)
			{
				default_attr[i].stComboAttr.mipi_attr.settle_count = ppra->settle;
			}

			if (ppra->mipi_fre > 0)
			{
				default_attr[i].stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
			}
			if (ppra->dpcm == 1)
			{
				default_attr[i].stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
				default_attr[i].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			}
			else if (ppra->dpcm == 2)
			{
				default_attr[i].stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
				default_attr[i].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
			}

			if (default_attr[i].stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
			{
				default_attr[i].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
			}

			if (ppra->feature_mask)
			{
				default_attr[i].stPipeAttr.u32FeatureMask = ppra->feature_mask;
			}

			/*start vi*/
			AR_MPI_VI_SetMipiBindDev(ViDev[i], mipi_index[i]);
			default_attr[i].stComboAttr.devno = mipi_index[i];
			AR_MPI_VI_SetComboDevAttr(&default_attr[i].stComboAttr);
			AR_MPI_VI_SetDevAttr(ViDev[i], &default_attr[i].stDevAttr);
			AR_MPI_VI_EnableDev(ViDev[i]);
			VI_DEV_BIND_PIPE_S stDevBindPipe;
			stDevBindPipe.u32Num = 1;
			stDevBindPipe.PipeId[0] = ViPipe[i];
			AR_MPI_VI_SetDevBindPipe(ViDev[i], &stDevBindPipe);
			AR_MPI_VI_CreatePipe(ViPipe[i], &default_attr[i].stPipeAttr);
			AR_MPI_VI_StartPipe(ViPipe[i]);

			if (i == 0)
			{
				default_attr[i].stChnAttr.stSize.u32Width = 1280;
				default_attr[i].stChnAttr.stSize.u32Height = 720;
			}
			else if (i == 1)
			{
				default_attr[i].stChnAttr.stSize.u32Width = 960;
				default_attr[i].stChnAttr.stSize.u32Height = 1080;
			}

			AR_MPI_VI_SetChnAttr(ViPipe[i], ViChn[i], &default_attr[i].stChnAttr);
			AR_MPI_VI_EnableChn(ViPipe[i], ViChn[i]);

			ALG_LIB_S stAeLib;
			ALG_LIB_S stAwbLib;
			stAeLib.s32Id = ViPipe[i];
			stAwbLib.s32Id = ViPipe[i];
			strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
			strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

			if (p_obj[i]->pfnRegisterCallback != AR_NULL)
			{
				s32Ret = p_obj[i]->pfnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("sensor_register_callback failed with AR_NULL!\n");
				goto EXIT;
			}

			if (AR_NULL != p_obj[i]->pfnSetBusInfo)
			{
				ISP_SNS_COMMBUS_U uSnsBusInfo;
				uSnsBusInfo.s8I2cDev = s8I2cDev[i];
				s32Ret = p_obj[i]->pfnSetBusInfo(ViPipe[i], uSnsBusInfo);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("set sensor bus info failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("not support set sensor bus info!\n");
				goto EXIT;
			}
			AR_MPI_ISP_MemInit(ViPipe[i]);

			if (i == 0)
			{
				default_attr[i].stPubAttr.stWndRect.u32Width = 1920;
				default_attr[i].stPubAttr.stWndRect.u32Height = 1080;
				default_attr[i].stPubAttr.stWndRect.s32X = (default_attr[i].stPubAttr.stSnsSize.u32Width - default_attr[i].stPubAttr.stWndRect.u32Width) / 2;
				default_attr[i].stPubAttr.stWndRect.s32Y = (default_attr[i].stPubAttr.stSnsSize.u32Height - default_attr[i].stPubAttr.stWndRect.u32Height) / 2;
			}
			else if (i == 1)
			{
				default_attr[i].stPubAttr.stWndRect.u32Width = 960;
				default_attr[i].stPubAttr.stWndRect.u32Height = 1280;
				default_attr[i].stPubAttr.stWndRect.s32X = (default_attr[i].stPubAttr.stSnsSize.u32Width - default_attr[i].stPubAttr.stWndRect.u32Width) / 2;
				default_attr[i].stPubAttr.stWndRect.s32Y = (default_attr[i].stPubAttr.stSnsSize.u32Height - default_attr[i].stPubAttr.stWndRect.u32Height) / 2;
			}

			AR_MPI_ISP_SetPubAttr(ViPipe[i], &default_attr[i].stPubAttr);
			VI_PIPE_EXT_ATTR_S stPipeAttr;
			AR_MPI_VI_GetPipeExtAttr(ViPipe[i], &stPipeAttr);
			stPipeAttr.bFoucs = 0;
			stPipeAttr.s32EnableExtAttr = 1;
			// 修改tuning 参数,hmax vmax
			if (i == 0)
			{
				strcpy(stPipeAttr.TuningPraBinName, "/usrdata/imx307_tuning_preview_fam30.bin");
			}
			else if (i == 1)
			{
				strcpy(stPipeAttr.TuningPraBinName, "/usrdata/sc132gs_tuning_preview_fam30.bin");
			}
			stPipeAttr.u32Hmax = default_attr[i].stPubAttr.stSnsSize.u32Width + 200;
			stPipeAttr.u32FullLinesStd = default_attr[i].stPubAttr.stSnsSize.u32Height + 100;
			stPipeAttr.u32FullLinesMax = 0xffff;
			AR_MPI_VI_SetPipeExtAttr(ViPipe[i], &stPipeAttr);

			AR_MPI_ISP_Init(ViPipe[i]);
			AR_MPI_ISP_Run(ViPipe[i]);

			// before push buffer ,set the manula aec and manual awb
			if (i == 0)
			{
				ISP_AEC_MANU_TIDY_ATTR_S aec;
				aec.stAecManu.aec_mode = AEC_MANUAL;
				aec.stAecManu.gain = 1.003906;
				aec.stAecManu.exp_time_us = 4444;
				AR_MPI_ISP_SetAecManuTidyAttr(ViPipe[i], &aec);

				ISP_AWB_MANU_TIDY_ATTR_S awb;
				awb.stAwbManu.awb_mode = AWB_MANUAL;
				awb.stAwbManu.cct = 6860;
				awb.stAwbManu.r_gain = 2.148636;
				awb.stAwbManu.g_gain = 1;
				awb.stAwbManu.b_gain = 1.466090;
				AR_MPI_ISP_SetAwbManuTidyAttr(ViPipe[i], &awb);
			}
			else if (i == 2)
			{
				ISP_AEC_MANU_TIDY_ATTR_S aec;
				aec.stAecManu.aec_mode = AEC_MANUAL;
				aec.stAecManu.gain = 16.578125;
				aec.stAecManu.exp_time_us = 2000;
				AR_MPI_ISP_SetAecManuTidyAttr(ViPipe[i], &aec);

				ISP_AWB_MANU_TIDY_ATTR_S awb;
				awb.stAwbManu.awb_mode = AWB_MANUAL;
				awb.stAwbManu.cct = 5000;
				awb.stAwbManu.r_gain = 1;
				awb.stAwbManu.b_gain = 1;
				awb.stAwbManu.g_gain = 1;
				AR_MPI_ISP_SetAwbManuTidyAttr(ViPipe[i], &awb);
			}
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		pthread_t ch_thread;
		pthread_t ch1_thread;
		pthread_t pipe0_thread;
		pthread_t pipe1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		// start thread to fetch raw file
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_fetch;
		pipe0_fetch.pipe_id = 0;
		pipe0_fetch.ch_id = 0;
		pipe0_fetch.vo_ch_id = 0;
		pipe0_fetch.nframes = ppra->s32nframes;
		pipe0_fetch.name = "/usrdata/sensor_imx307.raw";
		pipe0_fetch.p_attr = &default_attr[0];
		pthread_create(&pipe0_thread, pstAttr, fetch_run, &pipe0_fetch);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_fetch;
		pipe1_fetch.pipe_id = 1;
		pipe1_fetch.ch_id = 0;
		pipe1_fetch.vo_ch_id = 0;
		pipe1_fetch.nframes = ppra->s32nframes;
		pipe1_fetch.name = "/usrdata/sensor_sc132_gs.raw";
		pipe1_fetch.p_attr = &default_attr[1];
		pthread_create(&pipe1_thread, pstAttr, fetch_run, &pipe1_fetch);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);
		pthread_join(pipe0_thread, NULL);
		pthread_join(pipe1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);

		for (int i = 0; i < 2; i++)
		{
			AR_MPI_ISP_Exit(ViPipe[i]);
			if (p_obj[i]->pfnUnRegisterCallback != AR_NULL)
			{
				ALG_LIB_S stAeLib;
				ALG_LIB_S stAwbLib;
				stAeLib.s32Id = ViPipe[i];
				stAwbLib.s32Id = ViPipe[i];
				strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
				strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
				s32Ret = p_obj[i]->pfnUnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);
				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				}
			}
			AR_MPI_VI_DisableChn(ViPipe[i], ViChn[i]);
			AR_MPI_VI_StopPipe(ViPipe[i]);
			AR_MPI_VI_DestroyPipe(ViPipe[i]);
			AR_MPI_VI_DisableDev(ViDev[i]);
		}
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	static void *frame_run_performance(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		int fd = -1;
		char name[256];

		while (1)
		{
			if (g_exit)
			{
				break;
			}

			// SAMPLE_PRT("get frame = %d : pipe_id = %d, ch_id = %d, vo_ch_id = %d\n", ref, run_inf->pipe_id, run_inf->ch_id, run_inf->vo_ch_id);
			int status = AR_MPI_VI_GetChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo, 5000);
			if (status)
			{
				continue;
			}
			if (ref > 1000)
			{
				system("cat /proc/umap/vin/pipe4/stats");
				ref = 0;
			}
			ref++;

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",run_inf->pipe_id,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = run_inf->nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					g_exit = 1;
				}
			}
		}
		ar_printf("frame run pipe_id=%d run_inf=%d exit", run_inf->pipe_id, run_inf->ch_id);
		return NULL;
	}

	AR_S32 SAMPLE_VI_1080p_2200x1125_power_performance_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 4;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR1_TYPE;

		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		SAMPLE_PRT("enSnsType=%d \n", stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		SAMPLE_PRT("pool0 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[0].u64BlkSize, stVbConf.astCommPool[0].u32BlkCnt);

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		SAMPLE_PRT("pool1 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[1].u64BlkSize, stVbConf.astCommPool[1].u32BlkCnt);

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi_Step1(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		s32Ret = SAMPLE_COMM_VI_StartVi_Step2(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		pthread_t ch_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 4;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run_performance, &pipe0_ch0);

		SAMPLE_PRT("push a raw to isp \n");

		// read a raw
		int fd = open("/local/imx307.raw", O_RDONLY);
		int raw_size = 1920 * 2 * 1080;
		if (fd < 0)
		{
			ar_err(" !!!!!!raw file not exist,/local/imx307.raw  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			goto EXIT2;
		}
		int vb_blk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, raw_size, NULL);
		AR_MPI_VB_MmapPool(AR_MPI_VB_Handle2PoolId(vb_blk));
		AR_U64 phy_addr = AR_MPI_VB_Handle2PhysAddr(vb_blk);
		void *p_vaddr = NULL;
		AR_MPI_VB_GetBlockVirAddr(AR_MPI_VB_Handle2PoolId(vb_blk), AR_MPI_VB_Handle2PhysAddr(vb_blk), &p_vaddr);

		SAMPLE_PRT("vb_blk=%d phy_addr=%p  p_vaddr=%p raw_size=%d\n", vb_blk, (void *)phy_addr, p_vaddr, raw_size);

		if (!phy_addr || !p_vaddr)
		{
			ar_err("vb too small");
			goto EXIT2;
			;
		}
		void *p_vaddr_align = NULL;
		AR_U64 phy_addr_align = 0;
		p_vaddr_align = (void *)CAM_ALIGNE_TO((AR_U64)p_vaddr, 256);
		phy_addr_align = CAM_ALIGNE_TO(phy_addr, 256);
		// fill the mem by file
		int read_size = read(fd, p_vaddr_align, raw_size);
		SAMPLE_PRT("fill the raw men with file read_size=0x%x,raw_size=0x%x \n", read_size, raw_size);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			// send the raw to isp
			VIDEO_FRAME_INFO_S VideoFrame;
			VIDEO_FRAME_INFO_S *pstVideoFrame = &VideoFrame;
			pstVideoFrame->stVFrame.u64PhyAddr[0] = phy_addr_align;
			pstVideoFrame->stVFrame.u64PhyAddr[1] = 0;
			pstVideoFrame->stVFrame.u64PhyAddr[2] = 0;
			pstVideoFrame->stVFrame.u64VirAddr[0] = (AR_U64)p_vaddr_align;
			pstVideoFrame->stVFrame.u64VirAddr[1] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u64VirAddr[2] = (AR_U64)(NULL);
			pstVideoFrame->stVFrame.u32Stride[0] = 1920 * 2;
			pstVideoFrame->stVFrame.u32Stride[1] = 0;
			pstVideoFrame->stVFrame.u32Stride[2] = 0;

			pstVideoFrame->stVFrame.u32Width = 1920;
			pstVideoFrame->stVFrame.u32Height = 1080;
			pstVideoFrame->stVFrame.u64PTS = 0;
			pstVideoFrame->stVFrame.u64PrivateData = (AR_U64)(NULL);
			pstVideoFrame->u32PoolId = VB_INVALID_POOLID;

			int ret1 = AR_MPI_VI_SendPipeRaw(ViPipe, pstVideoFrame, -1);
			if (ret1 < 0)
			{
				while (1)
				{
					usleep(3000000000);
				}
			}
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					g_exit = 1;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		pthread_join(ch_thread, NULL);
		g_exit = 0;

		if (fd > 0)
			close(fd);
		if (vb_blk > 0)
			AR_MPI_VB_ReleaseBlock(vb_blk);
	EXIT2:

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	static AR_VOID vsync_update_for_imx307_process(VI_PIPE ViPipe)
	{
		// SAMPLE_PRT("ViPipe =%d \n",ViPipe);
		// send the vsync to raw dev
		int ViPipe_raw = 0;
		STRU_AR_CAMERA_SOF_PRO_T sof_prop;
		sof_prop.sof_mode = 1;
		sof_prop.to_trigger = 1;
		AR_MPI_ISP_VsyncUpdate(ViPipe_raw, &sof_prop);
	}
	static AR_VOID aec_update_for_imx307_process(VI_PIPE ViPipe, STRU_AEC_OUT_T *aec_out)
	{
		// SAMPLE_PRT("ViPipe =%d lux_index=%d\n",ViPipe,aec_out->lux_index);
		int ViPipe_raw = 0;
		AR_MPI_ISP_AecUpdate(ViPipe_raw, aec_out);
	}
	static AR_VOID awb_update_for_imx307_process(VI_PIPE ViPipe, STRU_AWB_OUT_T *awb_out)
	{
		// SAMPLE_PRT("ViPipe =%d lux_index=%d\n",ViPipe,aec_out->lux_index);
		// ar_printf("awb_update raw \n");
		int ViPipe_raw = 0;
		AR_MPI_ISP_AwbUpdate(ViPipe_raw, awb_out);
	}

	static void *run_loop_imx307_process_raw(void *param)
	{
		// AR_S32 ref=0;
		VIDEO_FRAME_INFO_S *pstVideoFrame = NULL;

		STRU_VIO_SAMPLE_RAW_PROCESS_RUN_T *p_raw_prcess = (STRU_VIO_SAMPLE_RAW_PROCESS_RUN_T *)param;

		while (1)
		{
			if (g_exit)
			{
				break;
			}

			p_raw_prcess->p_queue->queue_pop(p_raw_prcess->p_queue, (void **)&pstVideoFrame);
			if (pstVideoFrame == NULL)
			{
				// SAMPLE_PRT("wait raw frames\n");
				ar_signal_wait(p_raw_prcess->signal);
			}
			else
			{
				// SAMPLE_PRT("send a raw frame\n");
				int ret1 = AR_MPI_VI_SendPipeRaw(p_raw_prcess->pipe_id, pstVideoFrame, -1);
				if (ret1 < 0)
				{
					SAMPLE_PRT("AR_MPI_VI_SendPipeRaw failed exit the thread\n");
					break;
				}
				else
				{
					// SAMPLE_PRT("release to raw dev ViPipe_raw=%d ViChn_raw=%d\n",p_raw_prcess->ViPipe_raw,p_raw_prcess->ViChn_raw);
					AR_MPI_VI_ReleaseChnFrame(p_raw_prcess->ViPipe_raw, p_raw_prcess->ViChn_raw, pstVideoFrame);
					free(pstVideoFrame);
				}
			}
		}
		return NULL;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_imx307_raw_process(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev = 4;
		VI_PIPE ViPipe = ViDev;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.as32WorkingViId[1] = 1;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR1_TYPE;

		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR1_TYPE;

		SAMPLE_PRT("enSnsType=%d \n", stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

		AR_S32 s32WorkSnsId_raw = 1;
		VI_DEV ViDev_raw = 0;
		VI_PIPE ViPipe_raw = ViDev_raw;
		VI_CHN ViChn_raw = 2;

		stViConfig.astViInfo[s32WorkSnsId_raw].stSnsInfo.s32SnsId = 0;
		stViConfig.astViInfo[s32WorkSnsId_raw].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId_raw].stSnsInfo.MipiDev = 0;
		stViConfig.astViInfo[s32WorkSnsId_raw].stSnsInfo.enSnsType = SENSOR0_TYPE;

		stViConfig.astViInfo[s32WorkSnsId_raw].stDevInfo.ViDev = ViDev_raw;
		stViConfig.astViInfo[s32WorkSnsId_raw].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId_raw].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId_raw].stPipeInfo.aPipe[0] = ViPipe_raw;
		stViConfig.astViInfo[s32WorkSnsId_raw].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId_raw].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId_raw].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId_raw].stChnInfo.ViChn = ViChn_raw;
		stViConfig.astViInfo[s32WorkSnsId_raw].stChnInfo.enPixFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
		stViConfig.astViInfo[s32WorkSnsId_raw].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId_raw].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId_raw].stChnInfo.enCompressMode = enCompressMode;

		SAMPLE_PRT("s32WorkSnsId_raw enSnsType=%d \n", stViConfig.astViInfo[s32WorkSnsId_raw].stSnsInfo.enSnsType);

		/*get picture size use s32WorkSnsId*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		SAMPLE_PRT("pool0 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[0].u64BlkSize, stVbConf.astCommPool[0].u32BlkCnt);

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 8;

		SAMPLE_PRT("pool1 u64BlkSize=%ld u32BlkCnt=%d\n", stVbConf.astCommPool[1].u64BlkSize, stVbConf.astCommPool[1].u32BlkCnt);

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi_Step1(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// register vsync and aec update callback to pipe
		ISP_NOTIFY_REGISTER_S NotifyRegister = {0};
		NotifyRegister.stNotify.aec_update = aec_update_for_imx307_process;
		NotifyRegister.stNotify.vsync_update = vsync_update_for_imx307_process;
		NotifyRegister.stNotify.awb_update = awb_update_for_imx307_process;
		AR_MPI_ISP_RegisterNotifyCallBack(ViPipe, &NotifyRegister);

		s32Ret = SAMPLE_COMM_VI_StartVi_Step2(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}

		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		SAMPLE_PRT("creat a thread to \n");

		// creat a thread to receive raw frame
		pthread_t ch_thread = {0};
		// pthread_t	ch1_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_RAW_PROCESS_RUN_T raw_prcess;
		raw_prcess.p_queue = ar_creat_queue(16, "raw process");
		raw_prcess.signal = ar_create_signal();
		raw_prcess.pipe_id = ViPipe;
		raw_prcess.ViPipe_raw = ViPipe_raw;
		raw_prcess.ViChn_raw = ViChn_raw;
		pthread_create(&ch_thread, pstAttr, run_loop_imx307_process_raw, &raw_prcess);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			// get a raw from raw ch and send to isp in dev
			VIDEO_FRAME_INFO_S *pstFrameInfo = malloc(sizeof(VIDEO_FRAME_INFO_S));
			int status = AR_MPI_VI_GetChnFrame(ViPipe_raw, ViChn_raw, pstFrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					break;
				}
				continue;
			}
			// SAMPLE_PRT("get a raw frames \n");
			raw_prcess.p_queue->queue_insert(raw_prcess.p_queue, pstFrameInfo);
			ar_signal(raw_prcess.signal);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					g_exit = 1;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		ar_signal(raw_prcess.signal);
		pthread_join(ch_thread, NULL);
		g_exit = 0;

		while (get_queue_size(raw_prcess.p_queue))
		{
			VIDEO_FRAME_INFO_S *pstVideoFrame = NULL;
			raw_prcess.p_queue->queue_pop(raw_prcess.p_queue, (void **)&pstVideoFrame);
			AR_MPI_VI_ReleaseChnFrame(ViPipe_raw, ViChn_raw, pstVideoFrame);
			free(pstVideoFrame);
		}

		SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Bind_Get_Pipe_Raw(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		VIDEO_FRAME_INFO_S VideoFrame;

		AR_MPI_VI_GetPipeFrame(ViPipe, &VideoFrame, -1);

		// write the buffer to file
		char *name_file = "pipe_raw.raw";
		int fd = open(name_file, O_CREAT | O_TRUNC | O_RDWR);
		int len = VideoFrame.stVFrame.u32Stride[0] * VideoFrame.stVFrame.u32Height;
		SAMPLE_PRT("len=%d \n", len);
		write(fd, (void *)VideoFrame.stVFrame.u64VirAddr[0], len);
		close(fd);

		AR_MPI_VI_ReleasePipeFrame(ViPipe, &VideoFrame);

		AR_S32 s32cnt = 0, ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		VIDEO_FRAME_INFO_S FrameInfo;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				ref = 0;
			}
			ref++;
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_multi_process_get_ch_frame(SAMPLE_VIO_PRA *ppra)
	{
		// creat ch client
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		int client_id = 0;
		int ref = 0;
		AR_MPI_VI_CreatChn_Client(ViPipe, ViChn, &client_id);
		if (client_id < 0)
		{
			SAMPLE_PRT("SAMPLE_VIO_multi_get_ch_frame, creat client err \n");
			return -1;
		}
		ar_always("client_id=%d", client_id);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			VIDEO_FRAME_INFO_S FrameInfo;

			int ret = AR_MPI_VI_GetChnFrame_ByClient(ViPipe, ViChn, client_id, &FrameInfo, -1);

			ref++;
			if (ref >= 100)
			{
				ar_printf("SAMPLE_VIO_multi_get_ch_frame %x \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				ref = 0;
			}
			if (ret == AR_SUCCESS)
			{
				AR_MPI_VI_ReleaseChnFrame_ByClient(ViPipe, ViChn, client_id, &FrameInfo);
			}
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		AR_MPI_VI_DeleteChn_Client(ViPipe, ViChn, client_id);
		return 0;
	}

	static void *frame_run_multi(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		int client_id = 0;

		while (1)
		{

			do
			{
				if (g_exit)
				{
					goto exit;
				}

				ret = AR_MPI_VI_CreatChn_Client(run_inf->pipe_id, run_inf->ch_id, &client_id);

				sleep(1);

				if (ret)
				{
					ar_printf("try to creat ch client pipe_id=%d ch_id=%d\n", run_inf->pipe_id, run_inf->ch_id);
				}
			} while (ret);

			ar_printf("creat client id=%d pipe_id=%d ch_id=%d sucdess\n", client_id, run_inf->pipe_id, run_inf->ch_id);

			while (1)
			{

				VIDEO_FRAME_INFO_S FrameInfo;

				int ret = AR_MPI_VI_GetChnFrame_ByClient(run_inf->pipe_id, run_inf->ch_id, client_id, &FrameInfo, -1);

				if (ret)
				{
					ar_printf("get frame failed,may be main process exit,try delete client and try creat client\n");
					AR_MPI_VI_DeleteChn_Client(run_inf->pipe_id, run_inf->ch_id, client_id);
					break;
				}

				ref++;
				if (ref >= 100)
				{
					ar_printf("SAMPLE_VIO_multi_get_ch_frame %x %d %d\n", FrameInfo.stVFrame.u64PhyAddr[0], run_inf->pipe_id, run_inf->ch_id);
					ref = 0;
				}
				if (ret == AR_SUCCESS)
				{
					ret = AR_MPI_VI_ReleaseChnFrame_ByClient(run_inf->pipe_id, run_inf->ch_id, client_id, &FrameInfo);
					if (ret)
					{
						ar_printf("release frame failed,may be main process exit,try delete client and try creat client\n");
						AR_MPI_VI_DeleteChn_Client(run_inf->pipe_id, run_inf->ch_id, client_id);
						break;
					}
				}

				if (g_exit)
				{
					goto exit;
				}
			}
		}
	exit:
		return NULL;
	}

	AR_S32 SAMPLE_VIO_multi_processr_two_sensor_p0ch0_p1ch0(SAMPLE_VIO_PRA *ppra)
	{
		pthread_t ch_thread;
		pthread_t ch1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run_multi, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run_multi, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);
		return 0;
	}

	// dual imx307
	AR_S32 SAMPLE_VIO_Dual_Sensor_With_Hdr_Nohdr(SAMPLE_VIO_PRA *ppra)
	{

		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		// WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		s32WorkSnsId = 0;
		g_enSnsType[s32WorkSnsId] = SENSOR9_TYPE;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR9_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		s32WorkSnsId = 1;
		g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 0
	s32WorkSnsId=1;
    g_enSnsType[s32WorkSnsId] = SENSOR9_TYPE;
    stViConfig.as32WorkingViId[s32WorkSnsId]                     = 1;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev[s32WorkSnsId];
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev[s32WorkSnsId];
    stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = WDR_MODE_2To1_LINE;
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2;     //i2c channel id
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = ViDev[s32WorkSnsId];
    stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR9_TYPE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe[s32WorkSnsId];
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm=ppra->dpcm;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
#endif
		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d \n", enPicSize0, enPicSize1);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}
		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4 * s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		if (ppra->u32votype)
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		else
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t ch_thread;
		pthread_t ch1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return 0;
	}

	// dual imx307 + imx464
	AR_S32 SAMPLE_VIO_Three_Sensor_oneimx464_twoimx307(SAMPLE_VIO_PRA *ppra)
	{

		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[3] = {0, 1, 2};
		VI_PIPE ViPipe[3] = {0, 1, 2};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		// WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
		if (ppra->u32votype == 1) // one pipe
		{
			s32ViCnt = 1;
			s32WorkSnsId = 0;
			ViDev[s32WorkSnsId] = 0;
			g_enSnsType[s32WorkSnsId] = SENSOR10_TYPE;
			stViConfig.s32WorkingViNum = s32ViCnt;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 3; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 2;  // mipi index = 2
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR10_TYPE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}
		else if (ppra->u32votype == 2) // two pipe
		{

			s32WorkSnsId = 0;
			s32ViCnt = 2;
			g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
			stViConfig.s32WorkingViNum = s32ViCnt;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 0
    	s32WorkSnsId=1;
        g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
        stViConfig.as32WorkingViId[s32WorkSnsId]                     = 1;
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev[s32WorkSnsId];
        stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev[s32WorkSnsId];
        stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = WDR_MODE_NONE;
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2;     //i2c channel id
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev  = ViDev[s32WorkSnsId];
        stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
        stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
        stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe[s32WorkSnsId];
        stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
        stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
        stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
        stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
        stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
        stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
        stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
        stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

#endif
			s32WorkSnsId = 1;
			g_enSnsType[s32WorkSnsId] = SENSOR10_TYPE;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 3; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 2;  // ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR10_TYPE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}
		else // three pipe
		{
			// todo
			s32WorkSnsId = 0;
			s32ViCnt = 3;
			g_enSnsType[s32WorkSnsId] = SENSOR9_TYPE;
			stViConfig.s32WorkingViNum = s32ViCnt;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR9_TYPE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 1
			s32WorkSnsId = 1;
			g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#endif
			s32WorkSnsId = 2;
			g_enSnsType[s32WorkSnsId] = SENSOR10_TYPE;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 2;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 3; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR10_TYPE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}

		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0 = 0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 2*/
		PIC_SIZE_E enPicSize2 = 0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[2].stSnsInfo.enSnsType, &enPicSize2);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d, enPicSiee2 = %d \n", enPicSize0, enPicSize1, enPicSize2);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}

		if (enPicSize < enPicSize2)
		{
			enPicSize = enPicSize2;
		}
		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);

		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 5 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 1*s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 333000000;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, 200000000);

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// ar_vo_dev_init(&g_vo_obj,1920,1080,-1,-1);

		// init display
		if (ppra->u32votype < 0)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		chan_pos.x = 0;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 2, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t ch_thread;
		pthread_t ch1_thread;
		pthread_t ch2_thread;
		pthread_attr_t *pstAttr = NULL;
		if (ppra->u32votype == 1)
		{
			STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
			pipe0_ch0.pipe_id = 0;
			pipe0_ch0.ch_id = 0;
			pipe0_ch0.vo_ch_id = 0;
			pipe0_ch0.nframes = ppra->s32nframes;
			pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);
			pthread_join(ch_thread, NULL);
		}
		else if (ppra->u32votype == 2)
		{
			STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
			pipe0_ch0.pipe_id = 0;
			pipe0_ch0.ch_id = 0;
			pipe0_ch0.vo_ch_id = 0;
			pipe0_ch0.nframes = ppra->s32nframes;
			pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

			STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
			pipe1_ch0.pipe_id = 1;
			pipe1_ch0.ch_id = 0;
			pipe1_ch0.vo_ch_id = 1;
			pipe1_ch0.nframes = ppra->s32nframes;
			pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

			pthread_join(ch_thread, NULL);
			pthread_join(ch1_thread, NULL);
		}
		else
		{
			// todo
			STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
			pipe0_ch0.pipe_id = 0;
			pipe0_ch0.ch_id = 0;
			pipe0_ch0.vo_ch_id = 0;
			pipe0_ch0.nframes = ppra->s32nframes;
			pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

			STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
			pipe1_ch0.pipe_id = 1;
			pipe1_ch0.ch_id = 0;
			pipe1_ch0.vo_ch_id = 1;
			pipe1_ch0.nframes = ppra->s32nframes;
			pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);
#if 1
			STRU_VIO_SAMPLE_FRAME_RUN_T pipe2_ch0;
			pipe2_ch0.pipe_id = 2;
			pipe2_ch0.ch_id = 0;
			pipe2_ch0.vo_ch_id = 2;
			pipe2_ch0.nframes = ppra->s32nframes;
			pthread_create(&ch2_thread, pstAttr, frame_run, &pipe2_ch0);
			pthread_join(ch_thread, NULL);
			pthread_join(ch1_thread, NULL);
			pthread_join(ch2_thread, NULL);
#endif
		}

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display_surface(&g_vo_obj, 2);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return 0;
	}

	AR_S32 SAMPLE_VIO_Only_Two_Sensor(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		s32WorkSnsId = 0;
		g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		s32WorkSnsId = 1;
		g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d \n", enPicSize0, enPicSize1);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}
		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4 * s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t ch_thread;
		pthread_t ch1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_two_sensor_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj[2] = {&stSnsImx290Obj, &stSnsImx290Obj1};
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr[2];
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};
		VI_CHN ViChn[2] = {0, 0};
		AR_S32 mipi_index[2] = {0, 1};
		AR_S8 s8I2cDev[2] = {1, 2};

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj[0]->pfnGetDefaultAttr)
		{
			p_obj[0]->pfnGetDefaultAttr(0, &default_attr[0]);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr[0].stPubAttr.stSnsSize;
		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 0;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt++].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt++].u32BlkCnt = 4;

		stVbConf.u32MaxPoolCnt++;

		// malloc camera 2 vb
		if (p_obj[1]->pfnGetDefaultAttr)
		{
			p_obj[1]->pfnGetDefaultAttr(1, &default_attr[1]);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr[1].stPubAttr.stSnsSize;
		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 300000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		for (int i = 0; i < 2; i++)
		{
			if (ppra->settle > 0)
			{
				default_attr[i].stComboAttr.mipi_attr.settle_count = ppra->settle;
			}

			if (ppra->mipi_fre > 0)
			{
				default_attr[i].stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
			}
			if (ppra->dpcm == 1)
			{
				default_attr[i].stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
				default_attr[i].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			}
			else if (ppra->dpcm == 2)
			{
				default_attr[i].stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
				default_attr[i].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
			}

			if (default_attr[i].stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
			{
				default_attr[i].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
			}

			if (ppra->feature_mask)
			{
				default_attr[i].stPipeAttr.u32FeatureMask = ppra->feature_mask;
			}

			/*start vi*/
			AR_MPI_VI_SetMipiBindDev(ViDev[i], mipi_index[i]);
			default_attr[i].stComboAttr.devno = mipi_index[i];
			AR_MPI_VI_SetComboDevAttr(&default_attr[i].stComboAttr);
			AR_MPI_VI_SetDevAttr(ViDev[i], &default_attr[i].stDevAttr);
			AR_MPI_VI_EnableDev(ViDev[i]);
			VI_DEV_BIND_PIPE_S stDevBindPipe;
			stDevBindPipe.u32Num = 1;
			stDevBindPipe.PipeId[0] = ViPipe[i];
			AR_MPI_VI_SetDevBindPipe(ViDev[i], &stDevBindPipe);
			AR_MPI_VI_CreatePipe(ViPipe[i], &default_attr[i].stPipeAttr);
			AR_MPI_VI_StartPipe(ViPipe[i]);

			if (i == 0)
			{
				default_attr[i].stChnAttr.stSize.u32Width = 640;
				default_attr[i].stChnAttr.stSize.u32Height = 360;
			}
			else if (i == 1)
			{
				default_attr[i].stChnAttr.stSize.u32Width = 360;
				default_attr[i].stChnAttr.stSize.u32Height = 640;
			}

			AR_MPI_VI_SetChnAttr(ViPipe[i], ViChn[i], &default_attr[i].stChnAttr);
			AR_MPI_VI_EnableChn(ViPipe[i], ViChn[i]);

			ALG_LIB_S stAeLib;
			ALG_LIB_S stAwbLib;
			stAeLib.s32Id = ViPipe[i];
			stAwbLib.s32Id = ViPipe[i];
			strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
			strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

			if (p_obj[i]->pfnRegisterCallback != AR_NULL)
			{
				s32Ret = p_obj[i]->pfnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("sensor_register_callback failed with AR_NULL!\n");
				goto EXIT;
			}

			if (AR_NULL != p_obj[i]->pfnSetBusInfo)
			{
				ISP_SNS_COMMBUS_U uSnsBusInfo;
				uSnsBusInfo.s8I2cDev = s8I2cDev[i];
				s32Ret = p_obj[i]->pfnSetBusInfo(ViPipe[i], uSnsBusInfo);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("set sensor bus info failed with %#x!\n", s32Ret);
					goto EXIT;
				}
			}
			else
			{
				ar_err("not support set sensor bus info!\n");
				goto EXIT;
			}
			AR_MPI_ISP_MemInit(ViPipe[i]);

			if (i == 0)
			{
				default_attr[i].stPubAttr.stWndRect.u32Width = 960;
				default_attr[i].stPubAttr.stWndRect.u32Height = 540;
				default_attr[i].stPubAttr.stWndRect.s32X = (default_attr[i].stPubAttr.stSnsSize.u32Width - default_attr[i].stPubAttr.stWndRect.u32Width) / 2;
				default_attr[i].stPubAttr.stWndRect.s32Y = (default_attr[i].stPubAttr.stSnsSize.u32Height - default_attr[i].stPubAttr.stWndRect.u32Height) / 2;
			}
			else if (i == 1)
			{
				default_attr[i].stPubAttr.stWndRect.u32Width = 480;
				default_attr[i].stPubAttr.stWndRect.u32Height = 640;
				default_attr[i].stPubAttr.stWndRect.s32X = (default_attr[i].stPubAttr.stSnsSize.u32Width - default_attr[i].stPubAttr.stWndRect.u32Width) / 2;
				default_attr[i].stPubAttr.stWndRect.s32Y = (default_attr[i].stPubAttr.stSnsSize.u32Height - default_attr[i].stPubAttr.stWndRect.u32Height) / 2;
			}

			AR_MPI_ISP_SetPubAttr(ViPipe[i], &default_attr[i].stPubAttr);
			VI_PIPE_EXT_ATTR_S stPipeAttr;
			AR_MPI_VI_GetPipeExtAttr(ViPipe[i], &stPipeAttr);
			stPipeAttr.bFoucs = 0;
			AR_MPI_VI_SetPipeExtAttr(ViPipe[i], &stPipeAttr);

			AR_MPI_ISP_Init(ViPipe[i]);
			AR_MPI_ISP_Run(ViPipe[i]);
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = 640;
		chan_pos.h = 360;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = 480;
		chan_pos.h = 640;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		pthread_t ch_thread;
		pthread_t ch1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);

		for (int i = 0; i < 2; i++)
		{
			AR_MPI_ISP_Exit(ViPipe[i]);
			if (p_obj[i]->pfnUnRegisterCallback != AR_NULL)
			{
				ALG_LIB_S stAeLib;
				ALG_LIB_S stAwbLib;
				stAeLib.s32Id = ViPipe[i];
				stAwbLib.s32Id = ViPipe[i];
				strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
				strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
				s32Ret = p_obj[i]->pfnUnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);
				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				}
			}
			AR_MPI_VI_DisableChn(ViPipe[i], ViChn[i]);
			AR_MPI_VI_StopPipe(ViPipe[i]);
			AR_MPI_VI_DestroyPipe(ViPipe[i]);
			AR_MPI_VI_DisableDev(ViDev[i]);
		}
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_gc2053_linear_and_gc2093_hdr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		// WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
		if (ppra->u32votype == 1)
		{
			s32WorkSnsId = 0;
			g_enSnsType[s32WorkSnsId] = GC_GC2053_MIPI_1080P_30FPS_10BIT;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 1;  // ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = GC_GC2053_MIPI_1080P_30FPS_10BIT;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

			s32WorkSnsId = 1;

			g_enSnsType[s32WorkSnsId] = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
			;
			stViConfig.s32WorkingViNum = s32ViCnt;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}
		else
		{
			s32WorkSnsId = 0;

			g_enSnsType[s32WorkSnsId] = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
			;
			stViConfig.s32WorkingViNum = s32ViCnt;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
			s32WorkSnsId = 1;
			g_enSnsType[s32WorkSnsId] = GC_GC2053_MIPI_1080P_30FPS_10BIT;
			stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 1;  // ViDev[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = GC_GC2053_MIPI_1080P_30FPS_10BIT;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}
		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0 = 0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d\n", enPicSize0, enPicSize1);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}

		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);

		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 5 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 1*s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		// SAMPLE_AR_MPI_VIN_OpenDev(2,400000000,400000000,500000000,333000000);
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 333000000;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// ar_vo_dev_init(&g_vo_obj,1920,1080,-1,-1);

		// init display
		if (ppra->u32votype < 0)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		chan_pos.x = 0;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 2, &chan_pos);

		pthread_t ch_thread;
		pthread_t ch1_thread;
		pthread_t ch2_thread;
		pthread_attr_t *pstAttr = NULL;

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display_surface(&g_vo_obj, 2);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return 0;
	}

	AR_S32 SAMPLE_VIO_Only_Two_imx307_Sensor_4ch_stop_one_ch(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		s32WorkSnsId = 0;
		g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		s32WorkSnsId = 1;
		g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d \n", enPicSize0, enPicSize1);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}
		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4 * s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = 1;

		/*start vi chn1 of pipe 0*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe[0], ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width / 2;
		stChnAttr.stSize.u32Height = stSize.u32Height / 2;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe[0], ViChn1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		/*start vi chn1 of pipe 1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe[1], ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width / 2;
		stChnAttr.stSize.u32Height = stSize.u32Height / 2;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe[1], ViChn1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		if (ppra->u32votype)
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		else
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		chan_pos.x = 0;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 2, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 3, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t pipe0_ch0_thread;
		pthread_t pipe0_ch1_thread;
		pthread_t pipe1_ch0_thread;
		pthread_t pipe1_ch1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch1;

		s32Ret = AR_MPI_VI_EnableChn(ViPipe[0], ViChn1);
		s32Ret = AR_MPI_VI_EnableChn(ViPipe[1], ViChn1);

		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = 200;
		pthread_create(&pipe0_ch0_thread, pstAttr, frame_run, &pipe0_ch0);

		pipe0_ch1.pipe_id = 0;
		pipe0_ch1.ch_id = 1;
		pipe0_ch1.vo_ch_id = 1;
		pipe0_ch1.nframes = (ppra->s32nframes + 200) * 2;
		pthread_create(&pipe0_ch1_thread, pstAttr, frame_run, &pipe0_ch1);

		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 2;
		pipe1_ch0.nframes = (ppra->s32nframes + 200) * 4;
		pthread_create(&pipe1_ch0_thread, pstAttr, frame_run, &pipe1_ch0);

		pipe1_ch1.pipe_id = 1;
		pipe1_ch1.ch_id = 1;
		pipe1_ch1.vo_ch_id = 3;
		pipe1_ch1.nframes = (ppra->s32nframes + 200) * 8;
		pthread_create(&pipe1_ch1_thread, pstAttr, frame_run, &pipe1_ch1);

		for (int i = 0; i < (((ppra->s32nframes + 200) * 8) / 200); i++)
		{
			pthread_join(pipe0_ch0_thread, NULL);
			s32Ret = AR_MPI_VI_DisableChn(ViPipe[0], ViChn);
			close_display_surface(&g_vo_obj, 0);
			ar_delay(30 * 200);
			s32Ret = AR_MPI_VI_EnableChn(ViPipe[0], ViChn);
			chan_pos.x = 0;
			chan_pos.y = 0;
			chan_pos.w = mesh_w;
			chan_pos.h = mesh_h;
			ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

			pipe0_ch0.pipe_id = 0;
			pipe0_ch0.ch_id = 0;
			pipe0_ch0.vo_ch_id = 0;
			pipe0_ch0.nframes = 200;
			pthread_create(&pipe0_ch0_thread, pstAttr, frame_run, &pipe0_ch0);
			ar_printf("pipe0 ch0 start stop test i=%d\n", i);
			if (g_exit)
			{
				break;
			}
		}
		pthread_join(pipe0_ch1_thread, NULL);
		s32Ret = AR_MPI_VI_DisableChn(ViPipe[0], ViChn1);
		pthread_join(pipe1_ch0_thread, NULL);
		s32Ret = AR_MPI_VI_DisableChn(ViPipe[1], ViChn);
		pthread_join(pipe1_ch1_thread, NULL);
		s32Ret = AR_MPI_VI_DisableChn(ViPipe[1], ViChn1);

		// restart the ch
		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display_surface(&g_vo_obj, 2);
		close_display_surface(&g_vo_obj, 3);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Two_Sensor_60fps_30fps(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		s32WorkSnsId = 0;
		g_enSnsType[s32WorkSnsId] = SONY_IMX290_MIPI_2M_60FPS_12BIT;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.f32Fps[0] = 60;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		s32WorkSnsId = 1;
		g_enSnsType[s32WorkSnsId] = SONY_IMX290_MIPI_2M_30FPS_12BIT;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 2; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_30FPS_12BIT;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.f32Fps[0] = 30;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d \n", enPicSize0, enPicSize1);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}
		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4 * s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 300000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t ch_thread;
		pthread_t ch1_thread;

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Hdr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		if (SONY_IMX290_MIPI_2M_30FPS_12BIT == stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType)
		{
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1;
		}

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);
		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 gamma = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				ref = 0;

				// break;
			}
			ref++;
			// push to display
			send_to_fw_display(&g_vo_obj, 0, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
	AR_S32 SAMPLE_VIO_Only_Hdr_imx307_mulit_mode(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		if (SONY_IMX290_MIPI_2M_30FPS_12BIT == stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType)
		{
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1;
		}

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);
		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
				ref = 0;
				// break;
			}
			ref++;
			// push to display
			send_to_fw_display(&g_vo_obj, 0, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_2Ch(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN			   VoChn		  = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = 1;

		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width / 2;
		stChnAttr.stSize.u32Height = stSize.u32Height / 2;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t ch_thread = {0};
		pthread_t ch1_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
		pipe0_ch1.pipe_id = 0;
		pipe0_ch1.ch_id = 1;
		pipe0_ch1.vo_ch_id = 1;
		pipe0_ch1.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe0_ch1);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_2Ch_Plus_Raw(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN			   VoChn		  = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 16;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[1].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = 1;

		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width / 2;
		stChnAttr.stSize.u32Height = stSize.u32Height / 2;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		VI_CHN ViChn2 = 2;
		stChnAttr.stSize.u32Width = stSize.u32Width;
		stChnAttr.stSize.u32Height = stSize.u32Height;
		stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_16BPP;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn2, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn2);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		// get frame buffer
		// usleep(5000000);
		//  VIDEO_FRAME_INFO_S FrameInfo;
		//  AR_S32 ref=0;
		//  AR_S32 gamma=0;
		//  ISP_GAMMA_ATTR_S * pstGammaAttr=malloc(sizeof(ISP_GAMMA_ATTR_S));

		// AR_MPI_ISP_GetGammaAttr(ViPipe,pstGammaAttr);

		pthread_t ch_thread = {0};
		pthread_t ch1_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
		pipe0_ch1.pipe_id = 0;
		pipe0_ch1.ch_id = 1;
		pipe0_ch1.vo_ch_id = 1;
		pipe0_ch1.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe0_ch1);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
		return 0;
	}

	static void *frame_run2(void *param)
	{
		AR_S32 ret = 0;
		AR_S32 ref = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;

		while (!run_inf->stop)
		{
			ret = AR_MPI_VI_GetChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo, 5000);
			if (ret)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}

				continue;
			}
			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, run_inf->vo_ch_id, &FrameInfo);
			AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo);

			if (run_inf->nframes > 0)
			{
				run_inf->nframes--;
				if (run_inf->nframes == 0)
					run_inf->stop = AR_TRUE;
			}

			if (g_exit)
			{
				break;
			}
		}

		close_display_surface(&g_vo_obj, run_inf->vo_ch_id);
		AR_MPI_VI_DisableChn(run_inf->pipe_id, run_inf->ch_id);
		return NULL;
	}

	static void *frame_run3(void *param)
	{
		AR_S32 ret = 0;
		AR_S32 ref = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;

		while (!run_inf->stop)
		{
			ret = AR_MPI_VI_GetChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo, 5000);
			if (ret)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}

				continue;
			}
			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			// push to display
			send_to_fw_display(&g_vo_obj, run_inf->vo_ch_id, &FrameInfo);
			AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo);

			if (run_inf->nframes > 0)
			{
				run_inf->nframes--;
				if (run_inf->nframes == 0)
					run_inf->stop = AR_TRUE;
			}
		}

		close_display_surface(&g_vo_obj, run_inf->vo_ch_id);
		AR_MPI_VI_DisableChn(run_inf->pipe_id, run_inf->ch_id);
		return NULL;
	}

	static AR_VOID TP9930_Randomly_Start_Stop(SAMPLE_VI_CONFIG_S *stViConfig, STRU_AR_HAL_VO_RECT chn_pos[],
											  STRU_VIO_SAMPLE_FRAME_RUN_T pipe_ch[], pthread_t ch_thread[], AR_S32 s32nframes)
	{
		AR_S32 ret = AR_SUCCESS;
		AR_U32 i = 0;
		AR_U32 j = 0;
		AR_U32 rand_num = 0;
		AR_U32 sleep_time = 0;
		AR_U32 cur_chn_dis = 0;
		AR_U32 last_chn_dis = 0;
		AR_U32 total_chns = stViConfig->s32WorkingViNum * 6;
		AR_BOOL dev_stop_flag = AR_TRUE;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		pthread_attr_t *pstAttr = NULL;
		AR_U32 total_time = 0;
		AR_BOOL bQuit = AR_FALSE;

		srand((unsigned int)time(NULL));

		while (!bQuit)
		{
			rand_num = (unsigned int)rand();
			sleep_time = ((rand_num >> total_chns) % 5) + 1;
			cur_chn_dis = (rand_num & ((1 << total_chns) - 1));

			total_time += sleep_time;

			if (s32nframes > 0 && total_time * 25 >= s32nframes)
			{
				bQuit = AR_TRUE;
				if ((i & 1) == 0)
				{
					cur_chn_dis = (1 << total_chns) - 1;
				}
				else
				{
					cur_chn_dis = (((1 << total_chns) - 1) ^ last_chn_dis);
				}
				sleep_time = (s32nframes - (total_time - sleep_time) * 25) / 25;
			}
			else
			{
				if (i == 10 && !dev_stop_flag)
				{
					AR_U32 temp = 0;
					for (j = 0; j < stViConfig->s32WorkingViNum; j++)
					{
						if (cur_chn_dis & (1 << j))
							temp |= (0x3f << (j * 6));
					}
					cur_chn_dis = temp;
				}
			}

		EXIT:
			if (g_exit)
			{
				g_exit = 0;
				bQuit = AR_TRUE;
				if ((i & 1) == 0)
				{
					cur_chn_dis = (1 << total_chns) - 1;
				}
				else
				{
					cur_chn_dis = (((1 << total_chns) - 1) ^ last_chn_dis);
				}
			}
			else
			{
				sleep(sleep_time);
				if (g_exit)
					goto EXIT;
			}

			SAMPLE_PRT("rand_num[0x%x] sleep_time[%u] cur_chn_dis[0x%x] last_chn_dis[0x%x]\n",
					   rand_num, sleep_time, cur_chn_dis, last_chn_dis);

			if ((i & 1) == 0 || bQuit)
			{
				for (j = 0; j < total_chns; j++)
				{
					if (cur_chn_dis & (1 << j))
					{
						pipe_ch[j].stop = AR_TRUE;
						pthread_join(ch_thread[j], NULL);
						ch_thread[j] = 0;
						if ((j == total_chns - 1 && cur_chn_dis == ((1 << total_chns) - 1)) || (bQuit && (cur_chn_dis >> (j + 1)) == 0))
						{
							close_display(&g_vo_obj);
						}
						SAMPLE_PRT("stop pipe[%d]'s chn[%d]\n", pipe_ch[j].pipe_id, pipe_ch[j].ch_id);
					}

					// Stop a pipe, if all 3 channels of this pipe have been disabled
					if ((j % 3) == 2)
					{
						if (((cur_chn_dis >> (j - 2)) & 0x7) == 0x7 || (bQuit && ((cur_chn_dis >> (j - 2)) & 0x7)))
						{
							AR_MPI_ISP_Exit(pipe_ch[j].pipe_id);
							SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(pipe_ch[j].pipe_id);
							ret = SAMPLE_COMM_VI_StopSingleViPipe(pipe_ch[j].pipe_id);
							if (ret != AR_SUCCESS)
							{
								SAMPLE_PRT("SAMPLE_COMM_VI_StopSingleViPipe failed. pipe[%d] ret[%d]\n",
										   pipe_ch[j].pipe_id, ret);
								// return ret;
							}
							SAMPLE_PRT("stop pipe[%d]\n", pipe_ch[j].pipe_id);
						}
					}

					// Stop a dev, if all 6 channels of this dev have been disabled
					if ((j % 6) == 5)
					{
						if (((cur_chn_dis >> (j - 5)) & 0x3f) == 0x3f || (bQuit && ((cur_chn_dis >> (j - 5)) & 0x3f)))
						{
							ViDev = (j - 5) / 6;
							ret = SAMPLE_COMM_VI_StopDev(&stViConfig->astViInfo[ViDev]);
							if (ret != AR_SUCCESS)
							{
								SAMPLE_PRT("SAMPLE_COMM_VI_StopDev failed. dev[%d] ret[%d]\n", ViDev, ret);
								// return ret;
							}
							dev_stop_flag = AR_TRUE;
							SAMPLE_PRT("stop dev[%d]\n", ViDev);
						}
					}
				}
			}
			else
			{
				for (j = 0; j < total_chns; j++)
				{
					if (j == 0 && last_chn_dis == ((1 << total_chns) - 1))
					{
						ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
					}

					// Start a dev, if all 6 channels of this dev were disabled before
					if ((j % 6) == 0 && ((last_chn_dis >> j) & 0x3f) == 0x3f)
					{
						ViDev = j / 6;
						ret = SAMPLE_COMM_VI_StartDev(&stViConfig->astViInfo[ViDev]);
						if (ret != AR_SUCCESS)
						{
							SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed. dev[%d] ret[%d]\n", ViDev, ret);
							// return ret;
						}
						ret = SAMPLE_COMM_VI_BindPipeDev(&stViConfig->astViInfo[ViDev]);
						if (ret != AR_SUCCESS)
						{
							SAMPLE_PRT("SAMPLE_COMM_VI_BindPipeDev failed. dev[%d] ret[%d]\n", ViDev, ret);
							// return ret;
						}
						SAMPLE_PRT("start dev[%d]\n", ViDev);
					}

					// Start a pipe, if all 3 channels of this pipe were disabled before
					if ((j % 3) == 0 && ((last_chn_dis >> j) & 0x7) == 0x7)
					{
						ViPipe = j / 3;
						ViDev = ViPipe / 2;
						ret = SAMPLE_COMM_VI_StartSingleViPipe(&stViConfig->astViInfo[ViDev], (ViPipe & 1));
						if (ret != AR_SUCCESS)
						{
							SAMPLE_PRT("SAMPLE_COMM_VI_StartSingleViPipe failed. dev[%d] pipe[%d] ret[%d]\n",
									   ViDev, ViPipe, ret);
							// return ret;
						}
						ret = SAMPLE_COMM_VI_StartSingleIsp(&stViConfig->astViInfo[ViDev], (ViPipe & 1));
						if (ret != AR_SUCCESS)
						{
							SAMPLE_PRT("SAMPLE_COMM_VI_StartSingleIsp failed: dev[%d] pipe[%d] ret[%d]\n",
									   ViDev, ViPipe, ret);
							// return ret;
						}
						SAMPLE_PRT("start pipe[%d]\n", ViPipe);
					}

					if (last_chn_dis & (1 << j))
					{
						ret = AR_MPI_VI_EnableChn(pipe_ch[j].pipe_id, pipe_ch[j].ch_id);
						if (ret != AR_SUCCESS)
						{
							SAMPLE_PRT("AR_MPI_VI_EnableChn failed. pipe[%d] chn[%d] ret[%d]\n",
									   pipe_ch[j].pipe_id, pipe_ch[j].ch_id, ret);
							// return ret;
						}
						else
						{
							ar_vo_surface_init(&g_vo_obj, j, &chn_pos[j]);
							pipe_ch[j].stop = AR_FALSE;
							pthread_create(&ch_thread[j], pstAttr, frame_run3, &pipe_ch[j]);
						}
						SAMPLE_PRT("start pipe[%d]'s chn[%d]\n", pipe_ch[j].pipe_id, pipe_ch[j].ch_id);
					}
				}
			}

			last_chn_dis = cur_chn_dis;
			i++;
			if (i == 20)
			{
				i = 0;
				dev_stop_flag = AR_FALSE;
			}
		}

		return;
	}

	static AR_S32 TP9930_DVP_24Ch(SAMPLE_VIO_PRA *ppra, AR_BOOL randomly_start_stop)
	{
		AR_S32 s32Ret;
		SAMPLE_VI_CONFIG_S stViConfig;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;
		SAMPLE_VI_INFO_S *pstViInfo = NULL;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VI_PIPE ViPipe = 0;
		VI_PIPE ViChn = 0;
		AR_U32 u32PipeWidth[8] = {1920, 1920, 1920, 1920, 1920, 1920, 1920, 1920};
		AR_U32 u32PipeHeight[8] = {1080, 1080, 1080, 1080, 1080, 1080, 1080, 1080};
		AR_U32 f32PipeFps[8] = {25, 25, 25, 25, 25, 30, 30, 25};
		AR_U32 u32PipeVCNum[8] = {0, 1, 2, 3, 4, 5, 6, 7};
		AR_U32 u32ChnWidthScaler[8] = {384, 384, 384, 384, 384, 384, 384, 384};
		AR_U32 u32ChnHeightScaler[8] = {216, 216, 216, 216, 216, 216, 216, 216};
		int total_chns = 0;
		int i = 0;
		int j = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = 4;
		total_chns = stViConfig.s32WorkingViNum * 6;

		for (i = 0; i < stViConfig.s32WorkingViNum; i++)
		{
			g_enSnsType[5 + i] = TP9930_DVP_MULTIPLEX_2CH;
			stViConfig.as32WorkingViId[i] = i;
			pstViInfo = &stViConfig.astViInfo[i];

			pstViInfo->stSnsInfo.s32SnsId = 5 + i;
			pstViInfo->stSnsInfo.enSnsType = TP9930_DVP_MULTIPLEX_2CH;
			pstViInfo->stSnsInfo.s32BusId = 3;
			pstViInfo->stSnsInfo.MipiDev = i;

			pstViInfo->stDevInfo.ViDev = i;
			pstViInfo->stDevInfo.enWDRMode = WDR_MODE_NONE;

			pstViInfo->stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			pstViInfo->stPipeInfo.bMultiPipe = AR_TRUE;
			pstViInfo->stPipeInfo.bVcNumCfged = AR_TRUE;

			for (j = 0; j < 2; j++)
			{
				pstViInfo->stPipeInfo.aPipe[j] = ViPipe;
				pstViInfo->stPipeInfo.u32VCNum[j] = u32PipeVCNum[ViPipe];
				pstViInfo->stPipeInfo.u32Width[j] = u32PipeWidth[ViPipe];
				pstViInfo->stPipeInfo.u32Height[j] = u32PipeHeight[ViPipe];
				pstViInfo->stPipeInfo.f32Fps[j] = f32PipeFps[ViPipe];
				ViPipe++;
			}

			pstViInfo->stPipeInfo.aPipe[2] = -1;
			pstViInfo->stPipeInfo.aPipe[3] = -1;

			pstViInfo->stChnInfo.ViChn = ViChn;
			pstViInfo->stChnInfo.enPixFormat = enPixFormat;
			pstViInfo->stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
			pstViInfo->stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
			pstViInfo->stChnInfo.enCompressMode = COMPRESS_MODE_NONE;
		}

		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		for (ViPipe = 0; ViPipe < stViConfig.s32WorkingViNum * 2; ViPipe++)
		{
			u32BlkSize = COMMON_GetPicBufferSize(u32ChnWidthScaler[ViPipe],
												 u32ChnHeightScaler[ViPipe], enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
			stVbConf.u32MaxPoolCnt++;
		}

		for (ViPipe = 0; ViPipe < stViConfig.s32WorkingViNum * 2; ViPipe++)
		{
			u32BlkSize = COMMON_GetPicBufferSize(u32PipeWidth[ViPipe], u32PipeHeight[ViPipe],
												 enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5 * 2;
			stVbConf.u32MaxPoolCnt++;
		}

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		for (ViPipe = 0; ViPipe < stViConfig.s32WorkingViNum * 2; ViPipe++)
		{
			VI_CHN_ATTR_S stChnAttr;

			s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. Pipe: %d, Chn: 0, s32Ret: 0x%x !\n",
						   ViPipe, s32Ret);
				goto EXIT2;
			}

			for (ViChn = 1; ViChn < 3; ViChn++)
			{
				stChnAttr.stSize.u32Width = (ViChn == 1 ? u32ChnWidthScaler[ViPipe] : u32PipeWidth[ViPipe]);
				stChnAttr.stSize.u32Height = (ViChn == 1 ? u32ChnHeightScaler[ViPipe] : u32PipeHeight[ViPipe]);

				s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
				if (AR_SUCCESS != s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VI_SetChnAttr failed. Pipe: %d, Chn: %d, s32Ret: 0x%x !\n",
							   ViPipe, ViChn, s32Ret);
					goto EXIT2;
				}

				s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);
				if (AR_SUCCESS != s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VI_EnableChn failed. Pipe: %d, Chn: %d, s32Ret: 0x%x !\n",
							   ViPipe, ViChn, s32Ret);
					goto EXIT2;
				}
			}
		}

		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chn_pos[24];
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe_ch[24];
		pthread_t ch_thread[24];
		int grid = 6;

		AR_S32 disp_w = 1920 / grid;
		AR_S32 disp_h = 1080 / grid;
		for (i = 0; i < total_chns; i++)
		{
			AR_S32 row = i / grid;
			AR_S32 column = i % grid;
			chn_pos[i].x = column * disp_w;
			chn_pos[i].y = row * disp_h;
			chn_pos[i].w = disp_w;
			chn_pos[i].h = disp_h;
		}

		for (i = 0; i < total_chns; i++)
		{
			ar_vo_surface_init(&g_vo_obj, i, &chn_pos[i]);

			pthread_attr_t *pstAttr = NULL;
			pipe_ch[i].pipe_id = i / 3;
			pipe_ch[i].ch_id = i % 3;
			pipe_ch[i].vo_ch_id = i;
			pipe_ch[i].stop = AR_FALSE;
			pipe_ch[i].nframes = (randomly_start_stop ? -1 : ppra->s32nframes);
			pthread_create(&ch_thread[i], pstAttr, (randomly_start_stop ? frame_run3 : frame_run2), &pipe_ch[i]);
		}

		if (randomly_start_stop)
		{
			TP9930_Randomly_Start_Stop(&stViConfig, chn_pos, pipe_ch, ch_thread, ppra->s32nframes);
			SAMPLE_COMM_SYS_Exit();
			return 0;
		}
		else
		{
			for (i = 0; i < total_chns; i++)
				pthread_join(ch_thread[i], NULL);
			close_display(&g_vo_obj);
			if (g_exit)
				g_exit = 0;
		}

	EXIT2:
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch(SAMPLE_VIO_PRA *ppra)
	{
		return TP9930_DVP_24Ch(ppra, AR_FALSE);
	}

	AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Start_Stop(SAMPLE_VIO_PRA *ppra)
	{
		return TP9930_DVP_24Ch(ppra, AR_TRUE);
	}

	typedef struct
	{
		SAMPLE_VI_INFO_S *pstViInfo;
		AR_S32 s32PipeIdx;
		STRU_AR_HAL_VO_RECT stVoChnPos[3];
		AR_S32 s32VoChnId[3];
		AR_U32 u32ScalerWidth;
		AR_U32 u32ScalerHeight;
		PIXEL_FORMAT_E enChnPixelFormat;
		pthread_mutex_t *pDispMutex;
		AR_S32 *ps32DispRef;
		VI_USERPIC_ATTR_S stUsrPic;
		AR_U64 u64UsrPicPhy;
		AR_VOID *pUsrPicVirt;
		volatile AR_BOOL bStop;
		AR_U32 u32VoType;
		AR_BOOL bUsrPic;
		AR_BOOL bNoDvpScaler;
		AR_BOOL bCf50En;
		VI_CH_CF50_CMP_ATTR_T stCf50Attr;
	} PIPE_THREAD_INFO_S;

	typedef struct
	{
		VI_PIPE ViPipe;
		VI_CHN ViChn;
		AR_S32 s32VoChnId;
		STRU_AR_HAL_VO_RECT stVoChnPos;
		AR_U32 u32Width;
		AR_U32 u32Height;
		PIXEL_FORMAT_E enPixelFormat;
		AR_FLOAT f32Fps;
		pthread_mutex_t *pDispMutex;
		AR_S32 *ps32DispRef;
		volatile AR_BOOL bStop;
		AR_U32 u32VoType;
		AR_BOOL bCf50En;
		VI_CH_CF50_CMP_ATTR_T stCf50Attr;
	} CHN_THREAD_INFO_S;

	static AR_BOOL TP9930CompareSnsDetectInfo(ISP_SNS_DETECT_INFO_S *pstInfo1,
											  ISP_SNS_DETECT_INFO_S *pstInfo2)
	{
		if (!pstInfo1->bIsConnected && !pstInfo2->bIsConnected)
		{
			return AR_TRUE;
		}
		else
		{
			if (pstInfo1->u16Width == pstInfo2->u16Width && pstInfo1->u16Height == pstInfo2->u16Height && pstInfo1->f32Fps == pstInfo2->f32Fps && pstInfo1->bIsInterlace == pstInfo2->bIsInterlace && pstInfo1->bIsConnected == pstInfo2->bIsConnected)
			{
				return AR_TRUE;
			}
			else
			{
				return AR_FALSE;
			}
		}
	}

	static void *TP9930ChnThread(void *param)
	{
		AR_S32 s32Ret = 0;
		AR_U32 u32Ref = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		CHN_THREAD_INFO_S *pstChnThdInfo = (CHN_THREAD_INFO_S *)param;
		VI_CHN_ATTR_S stChnAttr = {
			.stSize = {1920, 1080},
			.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420,
			.enDynamicRange = DYNAMIC_RANGE_SDR8,
			.enVideoFormat = VIDEO_FORMAT_LINEAR,
			.enCompressMode = COMPRESS_MODE_NONE,
			.bMirror = 0,
			.bFlip = 0,
			.u32Depth = 0,
			.stFrameRate = {
				.s32SrcFrameRate = -1,
				.s32DstFrameRate = -1,
			},
		};

		SAMPLE_PRT("Pipe[%d] Chn[%d] thread start\n", pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn);

		stChnAttr.stSize.u32Width = pstChnThdInfo->u32Width;
		stChnAttr.stSize.u32Height = pstChnThdInfo->u32Height;
		stChnAttr.stFrameRate.s32SrcFrameRate = pstChnThdInfo->f32Fps;
		stChnAttr.stFrameRate.s32DstFrameRate = pstChnThdInfo->f32Fps;
		stChnAttr.enPixelFormat = pstChnThdInfo->enPixelFormat;
		if (pstChnThdInfo->bCf50En)
		{
			stChnAttr.enCompressMode = COMPRESS_MODE_SEG;
		}

		s32Ret = AR_MPI_VI_SetChnAttr(pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			ar_err("AR_MPI_VI_SetChnAttr failed. pipe[%d] chn[%d] ret[%d]",
				   pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, s32Ret);
			goto EXIT;
		}

		if (pstChnThdInfo->bCf50En)
		{
			s32Ret = AR_MPI_VI_SetChnCmpAttr(pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, &pstChnThdInfo->stCf50Attr);
			if (AR_SUCCESS != s32Ret)
			{
				ar_err("AR_MPI_VI_SetChnCmpAttr failed. pipe[%d] chn[%d] ret[%d]",
					   pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, s32Ret);
				goto EXIT;
			}
		}

		s32Ret = AR_MPI_VI_EnableChn(pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn);
		if (AR_SUCCESS != s32Ret)
		{
			ar_err("AR_MPI_VI_EnableChn failed. pipe[%d] chn[%d] ret[%d]",
				   pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, s32Ret);
			goto EXIT;
		}

		if (stChnAttr.enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_420 && stChnAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			pthread_mutex_lock(pstChnThdInfo->pDispMutex);
			if (*(pstChnThdInfo->ps32DispRef) == 0)
			{
				if (pstChnThdInfo->u32VoType)
				{
					ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, pstChnThdInfo->u32VoType, 0);
				}
				else
				{
					ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
				}
			}
			(*(pstChnThdInfo->ps32DispRef))++;
			pthread_mutex_unlock(pstChnThdInfo->pDispMutex);
			ar_vo_surface_init(&g_vo_obj, pstChnThdInfo->s32VoChnId, &pstChnThdInfo->stVoChnPos);
		}

		while (!pstChnThdInfo->bStop)
		{
			s32Ret = AR_MPI_VI_GetChnFrame(pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, &FrameInfo, 5000);
			if (s32Ret)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}

				continue;
			}
			if (u32Ref % 1024 == 0)
			{
				SAMPLE_PRT("get frame %lx \n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			u32Ref++;

			if (stChnAttr.enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_420 && stChnAttr.enCompressMode == COMPRESS_MODE_NONE)
			{
				send_to_fw_display(&g_vo_obj, pstChnThdInfo->s32VoChnId, &FrameInfo);
			}

			AR_MPI_VI_ReleaseChnFrame(pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn, &FrameInfo);
		}

		if (stChnAttr.enPixelFormat == PIXEL_FORMAT_YVU_PLANAR_420 && stChnAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			close_display_surface(&g_vo_obj, pstChnThdInfo->s32VoChnId);
			pthread_mutex_lock(pstChnThdInfo->pDispMutex);
			(*(pstChnThdInfo->ps32DispRef))--;
			if (*(pstChnThdInfo->ps32DispRef) == 0)
			{
				close_display(&g_vo_obj);
			}
			pthread_mutex_unlock(pstChnThdInfo->pDispMutex);
		}

		AR_MPI_VI_DisableChn(pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn);

	EXIT:
		SAMPLE_PRT("Pipe[%d] Chn[%d] thread stop\n", pstChnThdInfo->ViPipe, pstChnThdInfo->ViChn);
		return NULL;
	}

	static void *TP9930PipeThread(void *param)
	{
		AR_S32 s32Ret = 0;
		PIPE_THREAD_INFO_S *pstPipeThdInfo = (PIPE_THREAD_INFO_S *)param;
		VI_PIPE ViPipe = pstPipeThdInfo->pstViInfo->stPipeInfo.aPipe[pstPipeThdInfo->s32PipeIdx];
		ISP_SNS_DETECT_INFO_S stSnsDetectInfo[2];
		CHN_THREAD_INFO_S stChnThdInfo[3];
		pthread_t chThread[3] = {0};
		ISP_PUB_ATTR_S stIspPubAttr = {
			.stWndRect = {0, 0, 1920, 1080},
			.stSnsSize = {1920, 1080},
			.f32FrameRate = -1,
			.enBayer = AR_BAYER_RGGB,
			.enWDRMode = WDR_MODE_NONE,
			.u8SnsMode = 0,
		};
		AR_BOOL bPipeStart = AR_FALSE;
		AR_U32 u32PlugStableTimes = 0;
		AR_BOOL bAction = AR_FALSE;
		AR_BOOL bChnValid[3] = {AR_TRUE, AR_TRUE, AR_TRUE};
		AR_S32 i = 0;

#define TP9930_POLL_INTERVAL_US (100 * 1000)
#define TP9930_POLL_STABLE_TH 5

		SAMPLE_PRT("Pipe[%d] thread start\n", ViPipe);
		stSnsDetectInfo[0].bIsConnected = AR_FALSE;
		stSnsDetectInfo[1].bIsConnected = AR_FALSE;

		for (i = 0; i < 3; i++)
		{
			stChnThdInfo[i].ViPipe = ViPipe;
			stChnThdInfo[i].ViChn = i;
			stChnThdInfo[i].enPixelFormat = pstPipeThdInfo->enChnPixelFormat;
			stChnThdInfo[i].s32VoChnId = pstPipeThdInfo->s32VoChnId[i];
			stChnThdInfo[i].stVoChnPos = pstPipeThdInfo->stVoChnPos[i];
			stChnThdInfo[i].pDispMutex = pstPipeThdInfo->pDispMutex;
			stChnThdInfo[i].ps32DispRef = pstPipeThdInfo->ps32DispRef;
		}

		// Only Channel 2 supports YUV422, Channle 0 & 1 does not support.
		if (pstPipeThdInfo->enChnPixelFormat == PIXEL_FORMAT_YVU_PLANAR_422 || pstPipeThdInfo->bNoDvpScaler)
		{
			bChnValid[0] = AR_FALSE;
			bChnValid[1] = AR_FALSE;
		}

		s32Ret = SAMPLE_COMM_VI_StartSingleViPipe(pstPipeThdInfo->pstViInfo, pstPipeThdInfo->s32PipeIdx);
		if (s32Ret)
		{
			ar_err("SAMPLE_COMM_VI_StartSingleViPipe failed! Pipe[%d] Ret[%d]", ViPipe, s32Ret);
			goto EXIT1;
		}

		s32Ret = SAMPLE_COMM_VI_BindSensor(pstPipeThdInfo->pstViInfo, pstPipeThdInfo->s32PipeIdx);
		if (s32Ret)
		{
			ar_err("SAMPLE_COMM_VI_BindSensor failed! Pipe[%d]", ViPipe);
			goto EXIT2;
		}

		s32Ret = AR_MPI_ISP_StartSnsDetect(ViPipe);
		if (s32Ret)
		{
			ar_err("AR_MPI_ISP_StartSnsDetectRes failed! Pipe[%d]", ViPipe);
			goto EXIT3;
		}

		if (pstPipeThdInfo->bUsrPic)
		{
			s32Ret = AR_MPI_VI_SetUserPic(ViPipe, &pstPipeThdInfo->stUsrPic);
			if (s32Ret)
			{
				ar_err("AR_MPI_VI_SetUserPic failed! Pipe[%d]", ViPipe);
				goto EXIT3;
			}
		}

		while (!pstPipeThdInfo->bStop)
		{
			s32Ret = AR_MPI_ISP_GetSnsDetectInfo(ViPipe, &stSnsDetectInfo[0]);
			if (s32Ret)
				goto CONTINUE_POLL;

			if (TP9930CompareSnsDetectInfo(&stSnsDetectInfo[0], &stSnsDetectInfo[1]))
			{
				if (u32PlugStableTimes < TP9930_POLL_STABLE_TH)
				{
					u32PlugStableTimes++;
					if (u32PlugStableTimes == TP9930_POLL_STABLE_TH)
						bAction = AR_TRUE;
				}
			}
			else
			{
				u32PlugStableTimes = 0;
			}

			if (!bAction)
				goto CONTINUE_POLL;

			SAMPLE_PRT("Sensor detect info changed: pipe[%d] connect[%d] w[%u] h[%u] fps[%f]\n",
					   ViPipe, stSnsDetectInfo[0].bIsConnected, stSnsDetectInfo[0].u16Width,
					   stSnsDetectInfo[0].u16Height, stSnsDetectInfo[0].f32Fps);

			if (pstPipeThdInfo->bUsrPic)
			{
				if (bPipeStart && stSnsDetectInfo[0].bIsConnected)
				{
					if (stSnsDetectInfo[0].u16Width != stIspPubAttr.stSnsSize.u32Width || stSnsDetectInfo[0].u16Height != stIspPubAttr.stSnsSize.u32Height || stSnsDetectInfo[0].f32Fps != stIspPubAttr.f32FrameRate)
					{
						for (i = 0; i < 3; i++)
						{
							if (bChnValid[i])
								stChnThdInfo[i].bStop = AR_TRUE;
						}
						for (i = 0; i < 3; i++)
						{
							if (bChnValid[i])
							{
								pthread_join(chThread[i], NULL);
								chThread[i] = 0;
							}
						}
						AR_MPI_ISP_Exit(ViPipe);
						bPipeStart = AR_FALSE;
					}
				}

				if (stSnsDetectInfo[0].bIsConnected)
				{
					s32Ret = AR_MPI_VI_DisableUserPic(ViPipe);
					if (s32Ret)
					{
						ar_err("AR_MPI_VI_DisableUserPic failed! Pipe[%d]", ViPipe);
						goto CONTINUE_POLL;
					}
				}
				else
				{
					s32Ret = AR_MPI_VI_EnableUserPic(ViPipe);
					if (s32Ret)
					{
						ar_err("AR_MPI_VI_EnableUserPic failed! Pipe[%d]", ViPipe);
						goto CONTINUE_POLL;
					}
				}

				if (!bPipeStart)
				{
					if (stSnsDetectInfo[0].bIsConnected)
					{
						stIspPubAttr.stSnsSize.u32Width = stSnsDetectInfo[0].u16Width;
						stIspPubAttr.stSnsSize.u32Height = stSnsDetectInfo[0].u16Height;
						stIspPubAttr.stWndRect.u32Width = stSnsDetectInfo[0].u16Width;
						stIspPubAttr.stWndRect.u32Height = stSnsDetectInfo[0].u16Height;
						stIspPubAttr.f32FrameRate = stSnsDetectInfo[0].f32Fps;
					}
					else
					{
						stIspPubAttr.stSnsSize.u32Width = 1920;
						stIspPubAttr.stSnsSize.u32Height = 1080;
						stIspPubAttr.stWndRect.u32Width = 1920;
						stIspPubAttr.stWndRect.u32Height = 1080;
						stIspPubAttr.f32FrameRate = 30;
					}

					s32Ret = AR_MPI_ISP_MemInit(ViPipe);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("Init Ext memory failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stIspPubAttr);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("SetPubAttr failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					s32Ret = AR_MPI_ISP_Init(ViPipe);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("ISP Init failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("ISP Run failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					bPipeStart = AR_TRUE;

					for (i = 0; i < 3; i++)
					{
						if (bChnValid[i])
						{
							stChnThdInfo[i].u32Width = (i == 1 ? pstPipeThdInfo->u32ScalerWidth : stIspPubAttr.stSnsSize.u32Width);
							stChnThdInfo[i].u32Height = (i == 1 ? pstPipeThdInfo->u32ScalerHeight : stIspPubAttr.stSnsSize.u32Height);
							stChnThdInfo[i].f32Fps = stIspPubAttr.f32FrameRate;
							stChnThdInfo[i].bStop = AR_FALSE;
							stChnThdInfo[i].u32VoType = pstPipeThdInfo->u32VoType;
							stChnThdInfo[i].bCf50En = (i == 0 ? pstPipeThdInfo->bCf50En : AR_FALSE); // only chn-0 allows cf50
							if (stChnThdInfo[i].bCf50En)
							{
								stChnThdInfo[i].stCf50Attr = pstPipeThdInfo->stCf50Attr;
							}
							pthread_create(&chThread[i], NULL, TP9930ChnThread, &stChnThdInfo[i]);
						}
					}
				}
			}
			else
			{
				if (bPipeStart)
				{
					for (i = 0; i < 3; i++)
					{
						if (bChnValid[i])
							stChnThdInfo[i].bStop = AR_TRUE;
					}
					for (i = 0; i < 3; i++)
					{
						if (bChnValid[i])
						{
							pthread_join(chThread[i], NULL);
							chThread[i] = 0;
						}
					}
					AR_MPI_ISP_Exit(ViPipe);
					bPipeStart = AR_FALSE;
				}

				if (stSnsDetectInfo[0].bIsConnected)
				{
					stIspPubAttr.stSnsSize.u32Width = stSnsDetectInfo[0].u16Width;
					stIspPubAttr.stSnsSize.u32Height = stSnsDetectInfo[0].u16Height;
					stIspPubAttr.stWndRect.u32Width = stSnsDetectInfo[0].u16Width;
					stIspPubAttr.stWndRect.u32Height = stSnsDetectInfo[0].u16Height;
					stIspPubAttr.f32FrameRate = stSnsDetectInfo[0].f32Fps;

					s32Ret = AR_MPI_ISP_MemInit(ViPipe);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("Init Ext memory failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stIspPubAttr);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("SetPubAttr failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					s32Ret = AR_MPI_ISP_Init(ViPipe);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("ISP Init failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("ISP Run failed with %#x! Pipe[%d]", s32Ret, ViPipe);
						goto CONTINUE_POLL;
					}

					bPipeStart = AR_TRUE;

					for (i = 0; i < 3; i++)
					{
						if (bChnValid[i])
						{
							stChnThdInfo[i].u32Width = (i == 1 ? pstPipeThdInfo->u32ScalerWidth : stSnsDetectInfo[0].u16Width);
							stChnThdInfo[i].u32Height = (i == 1 ? pstPipeThdInfo->u32ScalerHeight : stSnsDetectInfo[0].u16Height);
							stChnThdInfo[i].f32Fps = stSnsDetectInfo[0].f32Fps;
							stChnThdInfo[i].bStop = AR_FALSE;
							stChnThdInfo[i].u32VoType = pstPipeThdInfo->u32VoType;
							stChnThdInfo[i].bCf50En = (i == 0 ? pstPipeThdInfo->bCf50En : AR_FALSE); // only chn-0 allows cf50
							if (stChnThdInfo[i].bCf50En)
							{
								stChnThdInfo[i].stCf50Attr = pstPipeThdInfo->stCf50Attr;
							}
							pthread_create(&chThread[i], NULL, TP9930ChnThread, &stChnThdInfo[i]);
						}
					}
				}
			}

		CONTINUE_POLL:
			bAction = AR_FALSE;
			stSnsDetectInfo[1] = stSnsDetectInfo[0];
			usleep(TP9930_POLL_INTERVAL_US);
		}

		AR_MPI_ISP_StopSnsDetect(ViPipe);

		if (bPipeStart)
		{
			for (i = 0; i < 3; i++)
			{
				if (bChnValid[i])
					stChnThdInfo[i].bStop = AR_TRUE;
			}
			for (i = 0; i < 3; i++)
			{
				if (bChnValid[i])
				{
					pthread_join(chThread[i], NULL);
					chThread[i] = 0;
				}
			}
			AR_MPI_ISP_Exit(ViPipe);
			bPipeStart = AR_FALSE;
		}

		if (pstPipeThdInfo->bUsrPic)
		{
			AR_MPI_VI_DisableUserPic(ViPipe);
		}

	EXIT3:
		SAMPLE_COMM_VI_UnbindSensor(pstPipeThdInfo->pstViInfo, pstPipeThdInfo->s32PipeIdx);
	EXIT2:
		SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
	EXIT1:
		SAMPLE_PRT("Pipe[%d] thread stop\n", ViPipe);
		return NULL;
	}

	static AR_S32 LoadUserPic(AR_CHAR *strFname, VIDEO_FRAME_S *pstFrame)
	{
		AR_S32 ret = 0;
		FILE *fp = NULL;
		AR_U32 size = 0;
		AR_U32 read_size = 0;
		AR_S32 i = 0;

		fp = fopen(strFname, "rb");
		if (fp == NULL)
		{
			ar_err("open user pic file [%s] failed!", strFname);
			return -1;
		}

		for (i = 0; i < 3; i++)
		{
			size = pstFrame->u32Stride[i] * (i == 0 ? pstFrame->u32Height : pstFrame->u32Height / 2);
			read_size = fread((AR_VOID *)pstFrame->u64VirAddr[i], 1, size, fp);
			if (read_size != size)
			{
				ar_err("read user pic file [%s] failed! size[%u] read_size[%u]",
					   strFname, size, read_size);
				ret = -1;
				goto exit;
			}
		}

	exit:
		fclose(fp);
		return ret;
	}

	static AR_S32 InitPipeUserPic(PIPE_THREAD_INFO_S *pstThdInfo, AR_CHAR *strFname)
	{
		AR_S32 ret = 0;
		static const AR_U32 au32BgColor[8] = {
			0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0x00ffff, 0xff00ff, 0xff8000, 0x8000ff};
		AR_U32 u32Width = pstThdInfo->pstViInfo->stPipeInfo.u32Width[pstThdInfo->s32PipeIdx];
		AR_U32 u32Height = pstThdInfo->pstViInfo->stPipeInfo.u32Height[pstThdInfo->s32PipeIdx];
		VI_PIPE ViPipe = pstThdInfo->pstViInfo->stPipeInfo.aPipe[pstThdInfo->s32PipeIdx];
		VIDEO_FRAME_S *pstVFrame = &pstThdInfo->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame;
		AR_U32 u32BufSize = 0;
		AR_U64 u64PAddr = 0;
		AR_U64 u64VAddr = 0;
		AR_S32 i = 0;

		// u32BufSize = COMMON_GetPicBufferSize(u32Width, u32Height, PIXEL_FORMAT_YVU_PLANAR_420,
		//     DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		u32BufSize = u32Width * u32Height * 3 / 2;
		u32BufSize += 512;

		ret = AR_MPI_SYS_MmzAlloc_Cached(&pstThdInfo->u64UsrPicPhy,
										 &pstThdInfo->pUsrPicVirt, NULL, NULL, u32BufSize);
		if (ret)
		{
			ar_err("Pipe[%d] allocate user pic buffer failed", ViPipe);
			pstThdInfo->u64UsrPicPhy = 0;
			pstThdInfo->pUsrPicVirt = NULL;
			ret = 0;
			goto PURE_COLOR;
		}

		u64PAddr = CAM_ALIGNE_TO(pstThdInfo->u64UsrPicPhy, 512);
		u64VAddr = CAM_ALIGNE_TO((AR_U64)pstThdInfo->pUsrPicVirt, 512);

		pstVFrame->u32Width = u32Width;
		pstVFrame->u32Height = u32Height;
		pstVFrame->enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		pstVFrame->enVideoFormat = VIDEO_FORMAT_LINEAR;
		pstVFrame->enCompressMode = COMPRESS_MODE_NONE;
		pstVFrame->enDynamicRange = DYNAMIC_RANGE_SDR8;
		for (i = 0; i < 3; i++)
		{
			pstVFrame->u32Stride[i] = (i == 0 ? u32Width : u32Width / 2);
			pstVFrame->u64PhyAddr[i] = u64PAddr;
			pstVFrame->u64VirAddr[i] = u64VAddr;
			u64PAddr += (pstVFrame->u32Stride[i] * (i == 0 ? u32Height : u32Height / 2));
			u64VAddr += (pstVFrame->u32Stride[i] * (i == 0 ? u32Height : u32Height / 2));
		}

		SAMPLE_PRT("Pipe[%d] w[%u] h[%u] user_pic_buf_size[%u] addr_orig[0x%llx %p] p0[%u 0x%llx 0x%llx] p1[%u 0x%llx 0x%llx] p2[%u 0x%llx 0x%llx]\n",
				   ViPipe, u32Width, u32Height, u32BufSize, pstThdInfo->u64UsrPicPhy, pstThdInfo->pUsrPicVirt,
				   pstVFrame->u32Stride[0], pstVFrame->u64PhyAddr[0], pstVFrame->u64VirAddr[0],
				   pstVFrame->u32Stride[1], pstVFrame->u64PhyAddr[1], pstVFrame->u64VirAddr[1],
				   pstVFrame->u32Stride[2], pstVFrame->u64PhyAddr[2], pstVFrame->u64VirAddr[2]);

		ret = LoadUserPic(strFname, pstVFrame);
		if (ret)
		{
			ar_err("Pipe[%d] load user pic [%s] failed", ViPipe, strFname);
			ret = 0;
			goto PURE_COLOR;
		}

		ret = AR_MPI_SYS_MmzFlushCache(pstThdInfo->u64UsrPicPhy, pstThdInfo->pUsrPicVirt, u32BufSize);
		if (ret)
		{
			ar_err("Pipe[%d] flush user pic memory failed! ret[%d]", ViPipe, ret);
			ret = 0;
			goto PURE_COLOR;
		}

		SAMPLE_PRT("Pipe[%d] load user pic [%s] successfully\n", ViPipe, strFname);
		return ret;

	PURE_COLOR:
		if (pstThdInfo->u64UsrPicPhy)
		{
			AR_MPI_SYS_MmzFree(pstThdInfo->u64UsrPicPhy, pstThdInfo->pUsrPicVirt);
			pstThdInfo->u64UsrPicPhy = 0;
			pstThdInfo->pUsrPicVirt = NULL;
		}
		pstThdInfo->stUsrPic.enUsrPicMode = VI_USERPIC_MODE_BGC;
		pstThdInfo->stUsrPic.unUsrPic.stUsrPicBg.u32BgColor = au32BgColor[ViPipe];
		SAMPLE_PRT("Pipe[%d] set user pic pure color [0x%08x] successfully\n", ViPipe, au32BgColor[ViPipe]);
		return ret;
	}

	static AR_VOID UninitPipeUserPic(PIPE_THREAD_INFO_S *pstThdInfo)
	{
		if (pstThdInfo->u64UsrPicPhy)
		{
			AR_MPI_SYS_MmzFree(pstThdInfo->u64UsrPicPhy, pstThdInfo->pUsrPicVirt);
			pstThdInfo->u64UsrPicPhy = 0;
			pstThdInfo->pUsrPicVirt = NULL;
		}
		memset(&pstThdInfo->stUsrPic, 0, sizeof(pstThdInfo->stUsrPic));
	}

	AR_S32 TP9930_Plug_Test(SAMPLE_VIO_PRA *ppra, AR_BOOL bUsrPic)
	{
		AR_S32 s32Ret = 0;
		SAMPLE_VI_CONFIG_S stViConfig;
		SAMPLE_VI_INFO_S *pstViInfo = NULL;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		const PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		const AR_U32 u32MaxWidth = 1920;
		const AR_U32 u32MaxHeight = 1080;
		const AR_FLOAT f32MaxFps = 30;
		const AR_U32 u32ScalerWidth = 640;
		const AR_U32 u32ScalerHeight = 360;
		const AR_U32 u32PipeVCNum[8] = {0, 1, 2, 3, 4, 5, 6, 7};
		const AR_U32 u32DvpIdx[4] = {0, 1, 2, 3};
		SAMPLE_SNS_TYPE_E enSnsType = TP9930_DVP_MULTIPLEX_2CH;
		AR_U32 u32PipeValid = 0xff;
		AR_S32 s32TotalPipes = 0;
		VB_CONFIG_S stVbConf;
		PIPE_THREAD_INFO_S stPipeThdInfo[8];
		AR_U32 u32DispGrid = 0;
		AR_S32 s32DispW = 0;
		AR_S32 s32DispH = 0;
		pthread_t pipeThread[8] = {0};
		pthread_mutex_t dispMutex;
		AR_S32 s32DispRef = 0;
		AR_S32 s32ViDevRef = 0;
		AR_CHAR achUsrPicFname[64] = {0};
		AR_S32 s32ArgIdx = -1;
		AR_S32 s32Temp = 0;
		AR_BOOL bDvpHighLowByteSwap = AR_FALSE;
		AR_BOOL bNoDvpScaler = AR_FALSE;
		AR_BOOL bCf50En = AR_FALSE;
		VI_CH_CF50_CMP_ATTR_T stCf50Attr = {0};

		AR_S32 i = 0;
		AR_S32 j = 0;
		AR_S32 k = 0;

		s32ArgIdx = sample_find_pra_by_name("-sns_type", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			s32Temp = strtol(ppra->argv[s32ArgIdx + 1], NULL, 0);
			if (s32Temp >= 0 && s32Temp <= 3)
			{
				enSnsType = TP9930_DVP_SINGLE_CH + s32Temp;
			}
		}

		s32ArgIdx = sample_find_pra_by_name("-pipe_valid", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			s32Temp = strtol(ppra->argv[s32ArgIdx + 1], NULL, 0);
			if (s32Temp >= 0 && s32Temp < 255)
			{
				u32PipeValid = s32Temp;
			}
		}

		s32ArgIdx = sample_find_pra_by_name("-byte_swap", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			s32Temp = strtol(ppra->argv[s32ArgIdx + 1], NULL, 0);
			bDvpHighLowByteSwap = s32Temp;
		}

		s32ArgIdx = sample_find_pra_by_name("-no_dvp_scaler", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			bNoDvpScaler = AR_TRUE;
		}

		s32ArgIdx = sample_find_pra_by_name("-cf50_loss", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			bCf50En = AR_TRUE;
			stCf50Attr.stChCmpAttr.enMode = CF50_CMP_MODE_LOSS;
			stCf50Attr.stChCmpAttr.enCmpRate = CF50_CMP_RATE_HIGH;
			stCf50Attr.stChCmpAttr.s32OsdCount = 0;

			s32ArgIdx = sample_find_pra_by_name("-cf50_rate", ppra->argc, ppra->argv);
			if (s32ArgIdx >= 0)
			{
				stCf50Attr.stChCmpAttr.enCmpRate = atoi(ppra->argv[s32ArgIdx + 1]);
			}
		}

		s32ArgIdx = sample_find_pra_by_name("-cf50_lossless", ppra->argc, ppra->argv);
		if (s32ArgIdx >= 0)
		{
			bCf50En = AR_TRUE;
			stCf50Attr.stChCmpAttr.enMode = CF50_CMP_MODE_LOSSLESS;
			stCf50Attr.stChCmpAttr.s32OsdCount = 0;

			s32ArgIdx = sample_find_pra_by_name("-cf50_osd0", ppra->argc, ppra->argv);
			if (s32ArgIdx >= 0)
			{
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].s32X = atoi(ppra->argv[s32ArgIdx + 1]);
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].s32Y = atoi(ppra->argv[s32ArgIdx + 2]);
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].u32Width = atoi(ppra->argv[s32ArgIdx + 3]);
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].u32Height = atoi(ppra->argv[s32ArgIdx + 4]);
				stCf50Attr.stChCmpAttr.s32OsdCount++;
			}

			s32ArgIdx = sample_find_pra_by_name("-cf50_osd1", ppra->argc, ppra->argv);
			if (s32ArgIdx >= 0)
			{
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].s32X = atoi(ppra->argv[s32ArgIdx + 1]);
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].s32Y = atoi(ppra->argv[s32ArgIdx + 2]);
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].u32Width = atoi(ppra->argv[s32ArgIdx + 3]);
				stCf50Attr.stChCmpAttr.stOsd[stCf50Attr.stChCmpAttr.s32OsdCount].u32Height = atoi(ppra->argv[s32ArgIdx + 4]);
				stCf50Attr.stChCmpAttr.s32OsdCount++;
			}
		}

		SAMPLE_PRT("Start TP9930 Test: SensorType[%d] PipeValid[0x%x] DvpByteSwap[%d] NoDvpScaler[%d] Cf50[%d] UsrPic[%d]\n",
				   enSnsType - TP9930_DVP_SINGLE_CH, u32PipeValid, bDvpHighLowByteSwap, bNoDvpScaler, bCf50En, bUsrPic);

		memset(stPipeThdInfo, 0, sizeof(stPipeThdInfo));

		pthread_mutex_init(&dispMutex, NULL);

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		if (enSnsType == TP9930_DVP_MULTIPLEX_2CH)
		{
			for (i = 0; i < 4; i++)
			{
				if (((u32PipeValid >> (i * 2)) & 0x3) == 0)
					continue;

				g_enSnsType[5 + ViDev] = enSnsType;
				stViConfig.as32WorkingViId[ViDev] = ViDev;
				pstViInfo = &stViConfig.astViInfo[ViDev];

				pstViInfo->stSnsInfo.s32SnsId = 5 + ViDev;
				pstViInfo->stSnsInfo.enSnsType = enSnsType;
				pstViInfo->stSnsInfo.s32BusId = 3;
				pstViInfo->stSnsInfo.MipiDev = u32DvpIdx[i];

				pstViInfo->stDevInfo.ViDev = ViDev;
				pstViInfo->stDevInfo.enWDRMode = WDR_MODE_NONE;
				pstViInfo->stDevInfo.bDvpHighLowByteSwap = bDvpHighLowByteSwap;

				pstViInfo->stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
				pstViInfo->stPipeInfo.bMultiPipe = AR_TRUE;
				pstViInfo->stPipeInfo.bVcNumCfged = AR_TRUE;
				for (j = 0; j < 2; j++)
				{
					if ((u32PipeValid >> (i * 2)) & (1 << j))
					{
						pstViInfo->stPipeInfo.aPipe[j] = ViPipe;
						pstViInfo->stPipeInfo.u32VCNum[j] = u32PipeVCNum[ViPipe];
						pstViInfo->stPipeInfo.u32Width[j] = u32MaxWidth;
						pstViInfo->stPipeInfo.u32Height[j] = u32MaxHeight;
						pstViInfo->stPipeInfo.f32Fps[j] = f32MaxFps;

						stPipeThdInfo[ViPipe].pstViInfo = pstViInfo;
						stPipeThdInfo[ViPipe].s32PipeIdx = j;
						stPipeThdInfo[ViPipe].u32ScalerWidth = u32ScalerWidth;
						stPipeThdInfo[ViPipe].u32ScalerHeight = u32ScalerHeight;
						stPipeThdInfo[ViPipe].enChnPixelFormat = enPixFormat;
						stPipeThdInfo[ViPipe].pDispMutex = &dispMutex;
						stPipeThdInfo[ViPipe].ps32DispRef = &s32DispRef;
						stPipeThdInfo[ViPipe].bStop = AR_FALSE;
						stPipeThdInfo[ViPipe].bNoDvpScaler = bNoDvpScaler;

						ViPipe++;
					}
					else
					{
						pstViInfo->stPipeInfo.aPipe[j] = -1;
					}
				}
				pstViInfo->stPipeInfo.aPipe[2] = -1;
				pstViInfo->stPipeInfo.aPipe[3] = -1;

				pstViInfo->stChnInfo.ViChn = 0;
				pstViInfo->stChnInfo.enPixFormat = enPixFormat;
				pstViInfo->stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
				pstViInfo->stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
				pstViInfo->stChnInfo.enCompressMode = COMPRESS_MODE_NONE;

				ViDev++;
			}
		}
		else if (enSnsType == TP9930_DVP_MULTIPLEX_4CH || enSnsType == TP9930_DVP_MULTIPLEX_4CH_BT656)
		{
			for (i = 0; i < 2; i++)
			{
				if (((u32PipeValid >> (i * 4)) & 0xf) == 0)
					continue;

				g_enSnsType[5 + ViDev] = enSnsType;
				stViConfig.as32WorkingViId[ViDev] = ViDev;
				pstViInfo = &stViConfig.astViInfo[ViDev];

				pstViInfo->stSnsInfo.s32SnsId = 5 + ViDev;
				pstViInfo->stSnsInfo.enSnsType = enSnsType;
				pstViInfo->stSnsInfo.s32BusId = 3;
				pstViInfo->stSnsInfo.MipiDev = u32DvpIdx[i * 2];

				pstViInfo->stDevInfo.ViDev = ViDev;
				pstViInfo->stDevInfo.enWDRMode = WDR_MODE_NONE;
				pstViInfo->stDevInfo.bDvpHighLowByteSwap = bDvpHighLowByteSwap;

				pstViInfo->stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
				pstViInfo->stPipeInfo.bMultiPipe = AR_TRUE;
				pstViInfo->stPipeInfo.bVcNumCfged = AR_TRUE;
				for (j = 0; j < 4; j++)
				{
					if ((u32PipeValid >> (i * 4)) & (1 << j))
					{
						pstViInfo->stPipeInfo.aPipe[j] = ViPipe;
						pstViInfo->stPipeInfo.u32VCNum[j] = u32PipeVCNum[ViPipe];
						pstViInfo->stPipeInfo.u32Width[j] = u32MaxWidth;
						pstViInfo->stPipeInfo.u32Height[j] = u32MaxHeight;
						pstViInfo->stPipeInfo.f32Fps[j] = f32MaxFps;

						stPipeThdInfo[ViPipe].pstViInfo = pstViInfo;
						stPipeThdInfo[ViPipe].s32PipeIdx = j;
						stPipeThdInfo[ViPipe].u32ScalerWidth = u32ScalerWidth;
						stPipeThdInfo[ViPipe].u32ScalerHeight = u32ScalerHeight;
						stPipeThdInfo[ViPipe].enChnPixelFormat = enPixFormat;
						stPipeThdInfo[ViPipe].pDispMutex = &dispMutex;
						stPipeThdInfo[ViPipe].ps32DispRef = &s32DispRef;
						stPipeThdInfo[ViPipe].bStop = AR_FALSE;
						stPipeThdInfo[ViPipe].bNoDvpScaler = bNoDvpScaler;

						ViPipe++;
					}
					else
					{
						pstViInfo->stPipeInfo.aPipe[j] = -1;
					}
				}

				pstViInfo->stChnInfo.ViChn = 0;
				pstViInfo->stChnInfo.enPixFormat = enPixFormat;
				pstViInfo->stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
				pstViInfo->stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
				pstViInfo->stChnInfo.enCompressMode = COMPRESS_MODE_NONE;

				ViDev++;
			}
		}
		else if (enSnsType == TP9930_DVP_SINGLE_CH)
		{
			for (i = 0; i < 4; i++)
			{
				if (((u32PipeValid >> i) & 0x1) == 0)
					continue;

				g_enSnsType[5 + ViDev] = enSnsType;
				stViConfig.as32WorkingViId[ViDev] = ViDev;
				pstViInfo = &stViConfig.astViInfo[ViDev];

				pstViInfo->stSnsInfo.s32SnsId = 5 + ViDev;
				pstViInfo->stSnsInfo.enSnsType = enSnsType;
				pstViInfo->stSnsInfo.s32BusId = 3;
				pstViInfo->stSnsInfo.MipiDev = u32DvpIdx[i];

				pstViInfo->stDevInfo.ViDev = ViDev;
				pstViInfo->stDevInfo.enWDRMode = WDR_MODE_NONE;
				pstViInfo->stDevInfo.bDvpHighLowByteSwap = bDvpHighLowByteSwap;

				pstViInfo->stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
				pstViInfo->stPipeInfo.bMultiPipe = AR_FALSE;
				pstViInfo->stPipeInfo.bVcNumCfged = AR_FALSE;

				pstViInfo->stPipeInfo.aPipe[0] = ViPipe;
				pstViInfo->stPipeInfo.u32Width[0] = u32MaxWidth;
				pstViInfo->stPipeInfo.u32Height[0] = u32MaxHeight;
				pstViInfo->stPipeInfo.f32Fps[0] = f32MaxFps;

				stPipeThdInfo[ViPipe].pstViInfo = pstViInfo;
				stPipeThdInfo[ViPipe].s32PipeIdx = 0;
				stPipeThdInfo[ViPipe].u32ScalerWidth = u32ScalerWidth;
				stPipeThdInfo[ViPipe].u32ScalerHeight = u32ScalerHeight;
				stPipeThdInfo[ViPipe].enChnPixelFormat = enPixFormat;
				stPipeThdInfo[ViPipe].pDispMutex = &dispMutex;
				stPipeThdInfo[ViPipe].ps32DispRef = &s32DispRef;
				stPipeThdInfo[ViPipe].bStop = AR_FALSE;
				stPipeThdInfo[ViPipe].bNoDvpScaler = bNoDvpScaler;

				ViPipe++;

				pstViInfo->stPipeInfo.aPipe[1] = -1;
				pstViInfo->stPipeInfo.aPipe[2] = -1;
				pstViInfo->stPipeInfo.aPipe[3] = -1;

				pstViInfo->stChnInfo.ViChn = 0;
				pstViInfo->stChnInfo.enPixFormat = enPixFormat;
				pstViInfo->stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
				pstViInfo->stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
				pstViInfo->stChnInfo.enCompressMode = COMPRESS_MODE_NONE;

				ViDev++;
			}
		}

		stViConfig.s32WorkingViNum = ViDev;
		s32TotalPipes = ViPipe;

		if (enPixFormat == PIXEL_FORMAT_YVU_PLANAR_422 || bNoDvpScaler)
		{
			if (s32TotalPipes <= 1)
			{
				u32DispGrid = 1;
			}
			else if (s32TotalPipes <= 4)
			{
				u32DispGrid = 2;
			}
			else
			{
				u32DispGrid = 3;
			}

			s32DispW = 1920 / u32DispGrid;
			s32DispH = 1080 / u32DispGrid;

			for (i = 0; i < s32TotalPipes; i++)
			{
				stPipeThdInfo[i].s32VoChnId[2] = i;
				stPipeThdInfo[i].stVoChnPos[2].x = (i % u32DispGrid) * s32DispW;
				stPipeThdInfo[i].stVoChnPos[2].y = (i / u32DispGrid) * s32DispH;
				stPipeThdInfo[i].stVoChnPos[2].w = s32DispW;
				stPipeThdInfo[i].stVoChnPos[2].h = s32DispH;
			}
		}
		else
		{
			u32DispGrid = 6;
			s32DispW = 1920 / u32DispGrid;
			s32DispH = 1080 / u32DispGrid;
			for (i = 0; i < s32TotalPipes; i++)
			{
				for (k = 0; k < 3; k++)
				{
					stPipeThdInfo[i].s32VoChnId[k] = i * 3 + k;
					stPipeThdInfo[i].stVoChnPos[k].x = ((i * 3 + k) % u32DispGrid) * s32DispW;
					stPipeThdInfo[i].stVoChnPos[k].y = ((i * 3 + k) / u32DispGrid) * s32DispH;
					stPipeThdInfo[i].stVoChnPos[k].w = s32DispW;
					stPipeThdInfo[i].stVoChnPos[k].h = s32DispH;
				}
			}
		}

		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		if (enPixFormat == PIXEL_FORMAT_YVU_PLANAR_420)
		{
			if (bCf50En)
			{
				AR_U32 u32PadSize = 512 * 16;
				AR_U32 u32Cf50WidthY = CAM_ALIGNE_TO(u32MaxWidth, 128);
				AR_U32 u32Cf50WidthUV = CAM_ALIGNE_TO(u32MaxWidth / 2, 128);
				AR_U32 u32Cf50HeadLineLen = 64;
				AR_U32 u32Cf50BufSize = 0;

				u32Cf50BufSize += (CAM_ALIGNE_TO(u32Cf50WidthY * u32MaxHeight, 512) + u32PadSize);
				u32Cf50BufSize += ((CAM_ALIGNE_TO(u32Cf50WidthUV * u32MaxHeight / 2, 512) + u32PadSize) * 2);
				u32Cf50BufSize += (CAM_ALIGNE_TO(u32Cf50HeadLineLen * u32MaxHeight, 512) + u32PadSize);
				u32Cf50BufSize += ((CAM_ALIGNE_TO(u32Cf50HeadLineLen * u32MaxHeight / 2, 512) + u32PadSize) * 2);

				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32Cf50BufSize;
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5 * s32TotalPipes;
				stVbConf.u32MaxPoolCnt++;

				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = COMMON_GetPicBufferSize(u32MaxWidth,
																								  u32MaxHeight, enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5 * s32TotalPipes;
				stVbConf.u32MaxPoolCnt++;
			}
			else
			{
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = COMMON_GetPicBufferSize(u32MaxWidth,
																								  u32MaxHeight, enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = (bNoDvpScaler ? (5 * s32TotalPipes) : (5 * 2 * s32TotalPipes));
				stVbConf.u32MaxPoolCnt++;
			}

			if (!bNoDvpScaler)
			{
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = COMMON_GetPicBufferSize(u32ScalerWidth,
																								  u32ScalerHeight, enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5 * s32TotalPipes;
				stVbConf.u32MaxPoolCnt++;
			}
		}
		else if (enPixFormat == PIXEL_FORMAT_YVU_PLANAR_422)
		{
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = COMMON_GetPicBufferSize(u32MaxWidth,
																							  u32MaxHeight, enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
			stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5 * s32TotalPipes;
			stVbConf.u32MaxPoolCnt++;
		}

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			ar_err("system init failed with %d!", s32Ret);
			return s32Ret;
		}

#if defined(AR9341)
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);
#elif defined(PROXIMA)
	SAMPLE_AR_MPI_VIN_OpenDev(0, 300000000, 300000000, 300000000, 200000000);
#else
	SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);
#endif

		s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
		if (s32Ret != AR_SUCCESS)
		{
			ar_err("SAMPLE_COMM_VI_StartMIPI failed!");
			goto EXIT1;
		}

		s32Ret = SAMPLE_COMM_VI_SetParam(&stViConfig);
		if (s32Ret != AR_SUCCESS)
		{
			ar_err("SAMPLE_COMM_VI_SetParam failed!");
			goto EXIT2;
		}

		for (i = 0; i < stViConfig.s32WorkingViNum; i++)
		{
			pstViInfo = &stViConfig.astViInfo[i];

			s32Ret = SAMPLE_COMM_VI_StartDev(pstViInfo);
			if (s32Ret != AR_SUCCESS)
			{
				ar_err("SAMPLE_COMM_VI_StartDev failed! Dev[%d]", pstViInfo->stDevInfo.ViDev);
				goto EXIT3;
			}

			s32Ret = SAMPLE_COMM_VI_BindPipeDev(pstViInfo);
			if (s32Ret != AR_SUCCESS)
			{
				SAMPLE_COMM_VI_StopDev(pstViInfo);
				ar_err("SAMPLE_COMM_VI_BindPipeDev failed! Dev[%d]", pstViInfo->stDevInfo.ViDev);
				goto EXIT3;
			}

			s32ViDevRef++;
		}

		if (bUsrPic)
		{
			k = 0;
			for (i = 0; i < s32TotalPipes; i++)
			{
				while (k < 8)
				{
					if (u32PipeValid & (1 << k))
						break;
					k++;
				}

				ViPipe = stPipeThdInfo[i].pstViInfo->stPipeInfo.aPipe[stPipeThdInfo[i].s32PipeIdx];
				memset(&stPipeThdInfo[i].stUsrPic, 0, sizeof(stPipeThdInfo[i].stUsrPic));
				strcpy(achUsrPicFname, "/usrdata/local/nosignal.yuv");
				s32Ret = InitPipeUserPic(&stPipeThdInfo[i], achUsrPicFname);
				if (s32Ret)
				{
					ar_err("Pipe[%d] init user pic [%s] failed!", ViPipe, achUsrPicFname);
					for (j = 0; j < i; j++)
						UninitPipeUserPic(&stPipeThdInfo[j]);
					goto EXIT3;
				}
			}
		}

		for (i = 0; i < s32TotalPipes; i++)
		{
			stPipeThdInfo[i].bUsrPic = bUsrPic;
			stPipeThdInfo[i].u32VoType = ppra->u32votype;
			stPipeThdInfo[i].bCf50En = bCf50En;
			stPipeThdInfo[i].stCf50Attr = stCf50Attr;
			pthread_create(&pipeThread[i], NULL, TP9930PipeThread, &stPipeThdInfo[i]);
		}

		while (g_exit == 0)
		{
			usleep(500000);
		}

		for (i = 0; i < s32TotalPipes; i++)
		{
			stPipeThdInfo[i].bStop = AR_TRUE;
		}

		for (i = 0; i < s32TotalPipes; i++)
		{
			pthread_join(pipeThread[i], NULL);
			pipeThread[i] = 0;
		}

		if (bUsrPic)
		{
			for (i = 0; i < s32TotalPipes; i++)
				UninitPipeUserPic(&stPipeThdInfo[i]);
		}

	EXIT3:
		for (i = 0; i < s32ViDevRef; i++)
		{
			SAMPLE_COMM_VI_StopDev(&stViConfig.astViInfo[i]);
		}
	EXIT2:
		SAMPLE_COMM_VI_StopMIPI(&stViConfig);
	EXIT1:
		SAMPLE_COMM_SYS_Exit();
		pthread_mutex_destroy(&dispMutex);
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug(SAMPLE_VIO_PRA *ppra)
	{
		return TP9930_Plug_Test(ppra, AR_FALSE);
	}

	AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug_UserPic(SAMPLE_VIO_PRA *ppra)
	{
		return TP9930_Plug_Test(ppra, AR_TRUE);
	}

	AR_S32 SAMPLE_VIO_Only_IMX307_MIPI_1Ch_AND_TP9930_DVP_4Ch(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		SAMPLE_VI_CONFIG_S stViConfig;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;
		SAMPLE_VI_INFO_S *pstViInfo = NULL;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VI_PIPE ViDev[3] = {0, 1, 2};
		VI_PIPE ViPipe[5] = {0, 1, 2, 3, 4};
		int dev_idx = 0;
		int pipe_idx = 0;
		AR_U32 u32TP9930PipeWidth[4] = {1920, 1920, 1920, 1920};
		AR_U32 u32TP9930PipeHeight[4] = {1080, 1080, 1080, 1080};
		AR_U32 f32TP9930PipeFps[4] = {25, 25, 25, 25};
		AR_U32 u32TP9930PipeVCNum[4] = {0, 1, 2, 3};
		int i = 0;
		int j = 0;

		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
		stViConfig.s32WorkingViNum = 0;

		// IMX307
		g_enSnsType[0] = SENSOR0_TYPE;
		stViConfig.as32WorkingViId[dev_idx] = dev_idx;
		pstViInfo = &stViConfig.astViInfo[dev_idx];
		pstViInfo->stSnsInfo.s32SnsId = 0;
		pstViInfo->stSnsInfo.enSnsType = SENSOR0_TYPE;
		pstViInfo->stSnsInfo.s32BusId = 1;
		pstViInfo->stSnsInfo.MipiDev = 0;
		pstViInfo->stDevInfo.ViDev = ViDev[dev_idx];
		pstViInfo->stDevInfo.enWDRMode = WDR_MODE_NONE;
		pstViInfo->stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		pstViInfo->stPipeInfo.bMultiPipe = AR_FALSE;
		pstViInfo->stPipeInfo.bVcNumCfged = AR_FALSE;
		pstViInfo->stPipeInfo.aPipe[0] = ViPipe[pipe_idx];
		pstViInfo->stPipeInfo.aPipe[1] = -1;
		pstViInfo->stPipeInfo.aPipe[2] = -1;
		pstViInfo->stPipeInfo.aPipe[3] = -1;
		pstViInfo->stChnInfo.ViChn = 0;
		pstViInfo->stChnInfo.enPixFormat = enPixFormat;
		pstViInfo->stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
		pstViInfo->stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
		pstViInfo->stChnInfo.enCompressMode = COMPRESS_MODE_NONE;
		stViConfig.s32WorkingViNum++;
		pipe_idx++;
		dev_idx++;

		u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, enPixFormat,
											 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
		stVbConf.u32MaxPoolCnt++;

		// TP9930
		for (i = 0; i < 2; i++)
		{
			g_enSnsType[5 + i] = TP9930_DVP_MULTIPLEX_2CH;
			stViConfig.as32WorkingViId[dev_idx] = dev_idx;
			pstViInfo = &stViConfig.astViInfo[dev_idx];

			pstViInfo->stSnsInfo.s32SnsId = 5 + i;
			pstViInfo->stSnsInfo.enSnsType = TP9930_DVP_MULTIPLEX_2CH;
			pstViInfo->stSnsInfo.s32BusId = 3;
			pstViInfo->stSnsInfo.MipiDev = i;

			pstViInfo->stDevInfo.ViDev = ViDev[dev_idx];
			pstViInfo->stDevInfo.enWDRMode = WDR_MODE_NONE;

			pstViInfo->stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			pstViInfo->stPipeInfo.bMultiPipe = AR_TRUE;
			pstViInfo->stPipeInfo.bVcNumCfged = AR_TRUE;

			for (j = 0; j < 2; j++)
			{
				pstViInfo->stPipeInfo.aPipe[j] = ViPipe[pipe_idx];
				pstViInfo->stPipeInfo.u32VCNum[j] = u32TP9930PipeVCNum[i * 2 + j];
				pstViInfo->stPipeInfo.u32Width[j] = u32TP9930PipeWidth[i * 2 + j];
				pstViInfo->stPipeInfo.u32Height[j] = u32TP9930PipeHeight[i * 2 + j];
				pstViInfo->stPipeInfo.f32Fps[j] = f32TP9930PipeFps[i * 2 + j];
				pipe_idx++;

				u32BlkSize = COMMON_GetPicBufferSize(u32TP9930PipeWidth[i * 2 + j],
													 u32TP9930PipeHeight[i * 2 + j], enPixFormat, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
				stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 5;
				stVbConf.u32MaxPoolCnt++;
			}

			pstViInfo->stPipeInfo.aPipe[2] = -1;
			pstViInfo->stPipeInfo.aPipe[3] = -1;

			pstViInfo->stChnInfo.ViChn = 2;
			pstViInfo->stChnInfo.enPixFormat = enPixFormat;
			pstViInfo->stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
			pstViInfo->stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
			pstViInfo->stChnInfo.enCompressMode = COMPRESS_MODE_NONE;

			stViConfig.s32WorkingViNum++;
			dev_idx++;
		}

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chn_pos[5];
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe_ch[5];
		pthread_t ch_thread[5];
		int grid = 3;

		AR_S32 disp_w = 1920 / grid;
		AR_S32 disp_h = 1080 / grid;
		for (i = 0; i < 5; i++)
		{
			AR_S32 row = i / grid;
			AR_S32 column = i % grid;
			chn_pos[i].x = column * disp_w;
			chn_pos[i].y = row * disp_h;
			chn_pos[i].w = disp_w;
			chn_pos[i].h = disp_h;
		}

		for (i = 0; i < pipe_idx; i++)
		{
			ar_vo_surface_init(&g_vo_obj, i, &chn_pos[i]);

			pthread_attr_t *pstAttr = NULL;
			pipe_ch[i].pipe_id = ViPipe[i];
			pipe_ch[i].ch_id = (i < 1 ? 0 : 2);
			pipe_ch[i].vo_ch_id = i;
			pipe_ch[i].stop = AR_FALSE;
			pipe_ch[i].nframes = ppra->s32nframes;
			pthread_create(&ch_thread[i], pstAttr, frame_run2, &pipe_ch[i]);
		}

		for (i = 0; i < pipe_idx; i++)
			pthread_join(ch_thread[i], NULL);
		close_display(&g_vo_obj);

		// EXIT2:
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_9311_hdr_vc_With_IMX415(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR26_TYPE;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test work mode = %d\n", ppra->cam_mode);
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}
		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}
		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}
		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_9311_mipi_High_speed_With_IMX415(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR25_TYPE;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test work mode = %d\n", ppra->cam_mode);
		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_IMX415(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR7_TYPE;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(1, 400000000, 400000000, 600000000, 333000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_IMX415_suspend_resueme(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR7_TYPE;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enTrigger = 1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(1, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		AR_MPI_VI_TriggerPipe(ViPipe, 1);

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		stVoConfig.stImageSize.u32Width = 3840;
		stVoConfig.stImageSize.u32Height = 2160;

		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}

		// set display enginner clock for 4k scaler to 1080p
		AR_U32 pu32Value = 0;
		AR_MPI_ISP_GetRegister(0, 0x01070000, &pu32Value);
		printf("0x01070000 = 0x%x\n", pu32Value);
		if (pu32Value != 0xfffff1f2)
			AR_MPI_ISP_SetRegister(0, 0x01070000, 0xfffff1f2);

		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pipe0_ch0.suspend = ppra->sus_mode;
		ar_hal_sys_register_binder_cmd("resume_cmd", suspend_resume_thread, &pipe0_ch0);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_IMX415_60Fps(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR8_TYPE;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(0, 600000000, 400000000, 600000000, 333000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_OS04A10_suspend_resume(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		// SAMPLE_SNS_TYPE_E  enSnsType      = SENSOR12_TYPE; //12bit linear
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR13_TYPE; // 10bit hdr
		// SAMPLE_SNS_TYPE_E  enSnsType      = SENSOR14_TYPE; //10bit linear
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		// WDR_MODE_E         enWDRMode      = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_S32 ret = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enTrigger = 1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 0
    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }
#endif
		stSize.u32Width = 2560;
		stSize.u32Height = 1440;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		AR_MPI_VI_TriggerPipe(ViPipe, 1);
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pipe0_ch0.suspend = ppra->sus_mode;
		ar_hal_sys_register_binder_cmd("resume_cmd", suspend_resume_thread, &pipe0_ch0);

		while (1)
		{
			ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 500000);
			if (ret)
			{
				continue;
			}

			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes > 0)
			{
				s32nframes--;
				if (s32nframes == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_OS04A10(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		// SAMPLE_SNS_TYPE_E  enSnsType      = SENSOR12_TYPE; //12bit linear
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR13_TYPE; // 10bit hdr
		// SAMPLE_SNS_TYPE_E  enSnsType      = SENSOR14_TYPE; //10bit linear
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		// WDR_MODE_E         enWDRMode      = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_S32 ret = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 0
    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }
#endif
		stSize.u32Width = 2560;
		stSize.u32Height = 1440;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 300000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 100000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 166000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		while (1)
		{
			ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (ret)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes > 0)
			{
				s32nframes--;
				if (s32nframes == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Hdr_For_Bind_With_OS04A10(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		SAMPLE_SNS_TYPE_E enSnsType = OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1; // 10bit hdr
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_S32 ret = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;

		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 0
    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }
#endif
		stSize.u32Width = 2560;
		stSize.u32Height = 1440;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		while (1)
		{
			ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (ret)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes > 0)
			{
				s32nframes--;
				if (s32nframes == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_GC2093(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		SAMPLE_SNS_TYPE_E enSnsType = GC_GC2093_MIPI_1080P_30FPS_10BIT;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_S32 ret = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		while (1)
		{
			ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (ret)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes > 0)
			{
				s32nframes--;
				if (s32nframes == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_GC2093_suspend_resueme(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		SAMPLE_SNS_TYPE_E enSnsType = GC_GC2093_MIPI_1080P_30FPS_10BIT;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_S32 ret = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enTrigger = 1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		AR_MPI_VI_TriggerPipe(ViPipe, 1);
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pipe0_ch0.suspend = ppra->sus_mode;
		ar_hal_sys_register_binder_cmd("resume_cmd", suspend_resume_thread, &pipe0_ch0);
		while (1)
		{
			ret = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 500000);
			if (ret)
			{
				continue;
			}

			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes > 0)
			{
				s32nframes--;
				if (s32nframes == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_GC2093_Hdr_with_imx307_line(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};

		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN             VoChn          = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		// WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		s32WorkSnsId = 0;
		s32ViCnt = 2;
		g_enSnsType[s32WorkSnsId] = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
		;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		s32WorkSnsId = 1;
		g_enSnsType[s32WorkSnsId] = SENSOR0_TYPE;
		stViConfig.as32WorkingViId[s32WorkSnsId] = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 3; // i2c channel id
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 2;  // ViDev[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR0_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[s32WorkSnsId];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size for sensor 0*/
		PIC_SIZE_E enPicSize0 = 0;
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize0);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		/*get picture size for sensor 1*/
		PIC_SIZE_E enPicSize1 = 0;

		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[1].stSnsInfo.enSnsType, &enPicSize1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		SAMPLE_PRT("enPicSize0 = %d, enPicSize1 = %d\n", enPicSize0, enPicSize1);
		if (enPicSize0 >= enPicSize1)
		{
			enPicSize = enPicSize0;
		}
		else
		{
			enPicSize = enPicSize1;
		}

		SAMPLE_PRT("enPicSize =%d\n", enPicSize);

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}
		SAMPLE_PRT("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);

		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 5 * s32ViCnt;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 1*s32ViCnt;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		// SAMPLE_AR_MPI_VIN_OpenDev(2,400000000,400000000,500000000,333000000);
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 333000000;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// ar_vo_dev_init(&g_vo_obj,1920,1080,-1,-1);

		// init display
		if (ppra->u32votype < 0)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		chan_pos.x = 0;
		chan_pos.y = mesh_h;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 2, &chan_pos);

		pthread_t ch_thread;
		pthread_t ch1_thread;
		pthread_t ch2_thread;
		pthread_attr_t *pstAttr = NULL;

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id = 1;
		pipe1_ch0.ch_id = 0;
		pipe1_ch0.vo_ch_id = 1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display_surface(&g_vo_obj, 2);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return 0;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_GC2093_Hdr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		SAMPLE_SNS_TYPE_E enSnsType = GC_GC2093_MIPI_1080P_30FPS_10BIT_WDR2TO1;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_S32 ret = 0;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			if (ref % 1000 == 0)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
			}
			ref++;

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

			if (s32nframes > 0)
			{
				s32nframes--;
				if (s32nframes == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_VI_DeMuxVCRaw_imx464(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe[4] = {0, 1, -1, -1};
		VI_CHN ViChn[4] = {2, 2, 0, 0};
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		// VO_CHN			   VoChn		  = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_2To1_LINE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
		g_enSnsType[s32WorkSnsId] = SENSOR20_TYPE;
		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe[0];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = ViPipe[1];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = ViPipe[2];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = ViPipe[3];
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_TRUE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_TRUE;

		for (int i = 0; i < 4; i++)
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32VCNum[i] = i;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn[0];
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SENSOR20_TYPE;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			printf("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			printf("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[0].u32BlkCnt = 5;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 5;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		// set two pipe w, h
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Width[0] = stSize.u32Width;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Width[1] = stSize.u32Width;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Height[0] = stSize.u32Height;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.u32Height[1] = stSize.u32Height;
		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

#if 1
		VI_CHN_ATTR_S stChnAttr;
		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe[1], ViChn[1], &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width;
		stChnAttr.stSize.u32Height = stSize.u32Height;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe[1], ViChn[1], &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe[1], ViChn[1]);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
#endif
		SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe[1], stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId);
		SAMPLE_COMM_ISP_Run(ViPipe[1]);
		pthread_t ch_thread = {0};
		pthread_t ch1_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 2;
		pipe0_ch0.vo_ch_id = -1;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);
#if 1
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
		pipe0_ch1.pipe_id = 1;
		pipe0_ch1.ch_id = 2;
		pipe0_ch1.vo_ch_id = -1;
		pipe0_ch1.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe0_ch1);
#endif
		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);
		SAMPLE_COMM_ISP_Stop(ViPipe[1]);
		SAMPLE_COMM_VI_StopVi(&stViConfig);

	EXIT:
		SAMPLE_COMM_SYS_Exit();

		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_SC530AI(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		SAMPLE_SNS_TYPE_E enSnsType = SC_SC530AI_MIPI_5M_30FPS_10BIT;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

#if 0
    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (AR_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }
#endif
		stSize.u32Width = 2880;
		stSize.u32Height = 1616;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		while (1)
		{
			usleep(500000000);
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
	typedef enum arSAMPLE_VPSS_TEST_MASK_E
	{
		VPSS_TEST_CROP = 1 << 0,
		VPSS_TEST_ROTATION = 1 << 1,
		VPSS_TEST_MIRROR = 1 << 2,
		VPSS_TEST_FLIP = 1 << 3,
		VPSS_TEST_EXTCHAN = 1 << 4,
		VPSS_TEST_LDC = 1 << 5,
		VPSS_TEST_REGION_LUMA = 1 << 6,
		VPSS_TEST_EIS = 1 << 7
	} SAMPLE_VPSS_TEST_MASK_E;

	AR_S32 SAMPLE_VIO_VPSS_Start(VPSS_GRP VpssGrp, AR_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr, VPSS_CHN_ATTR_S *pastVpssChnAttr, int mask)
	{
		VPSS_CHN VpssChn;
		AR_S32 s32Ret;
		AR_S32 j;

		s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);

		if (s32Ret != AR_SUCCESS)
		{
			SAMPLE_PRT("AR_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
			return AR_FAILURE;
		}

		s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);

		if (s32Ret != AR_SUCCESS)
		{
			SAMPLE_PRT("AR_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
			return AR_FAILURE;
		}

		for (j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
		{
			if (AR_TRUE == pabChnEnable[j])
			{
				VpssChn = j;
				s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastVpssChnAttr[VpssChn]);

				if (s32Ret != AR_SUCCESS)
				{
					SAMPLE_PRT("AR_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
					return AR_FAILURE;
				}

				AR_U32 u32Align = 64;
				s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
				if (s32Ret != AR_SUCCESS)
				{
					SAMPLE_PRT("AR_MPI_VPSS_SetChnAlign failed with %#x\n", s32Ret);
					return AR_FAILURE;
				}

				VPSS_MPU_INFO_S stMpuInfo = {
					.bEnable = 0,
				};

				if (mask & VPSS_TEST_EIS)
				{
					stMpuInfo.bEnable = 1;
				}

				s32Ret = AR_MPI_VPSS_SetChnMpuInfo(VpssGrp, VpssChn, &stMpuInfo);
				if (AR_SUCCESS != s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VPSS_SetChnMpuInfo failed. s32Ret: 0x%x !\n", s32Ret);
					return s32Ret;
				}

				s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);

				if (s32Ret != AR_SUCCESS)
				{
					SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
					return AR_FAILURE;
				}
			}
		}

		return AR_SUCCESS;
	}

	AR_S32 SAMPLE_VPSS_Config_Test(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN VpssExtChn, AR_BOOL *abChnEnable, AR_S32 w, AR_S32 h, AR_U32 mask)
	{
		AR_S32 s32Ret = 0;

		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
		VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

		VPSS_EXT_CHN_ATTR_S stVpssExtChnAttr = {0};

		/*config vpss*/
		stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
		stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
		stVpssGrpAttr.u32Width = w;
		stVpssGrpAttr.u32Height = h;

		stVpssGrpAttr.enCompressMode = enCompressMode;
		stVpssGrpAttr.enDynamicRange = enDynamicRange;
		stVpssGrpAttr.enVideoFormat = enVideoFormat;
		stVpssGrpAttr.enPixelFormat = enPixFormat;

		/*stVpssGrpAttr.u32MaxW 					 = stSize.u32Width;
		stVpssGrpAttr.u32MaxH						 = stSize.u32Height;
		stVpssGrpAttr.bNrEn 						 = AR_TRUE;
		stVpssGrpAttr.stNrAttr.enCompressMode		 = COMPRESS_MODE_FRAME;
		stVpssGrpAttr.stNrAttr.enNrMotionMode		 = NR_MOTION_MODE_NORMAL;*/

		astVpssChnAttr[VpssChn].u32Width = w;  // stSize.u32Width;
		astVpssChnAttr[VpssChn].u32Height = h; // stSize.u32Height;
		astVpssChnAttr[VpssChn].enChnMode = VPSS_CHN_MODE_USER;
		astVpssChnAttr[VpssChn].enCompressMode = enCompressMode;
		astVpssChnAttr[VpssChn].enDynamicRange = enDynamicRange;
		astVpssChnAttr[VpssChn].enVideoFormat = enVideoFormat;
		astVpssChnAttr[VpssChn].enPixelFormat = enPixFormat;
		astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
		astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
		astVpssChnAttr[VpssChn].u32Depth = 1;
		// add mirror test functioni
		if (mask & VPSS_TEST_MIRROR)
		{
			astVpssChnAttr[VpssChn].bMirror = AR_TRUE;
		}
		// add flip test function
		if (mask & VPSS_TEST_FLIP)
		{
			astVpssChnAttr[VpssChn].bFlip = AR_TRUE;
		}
		astVpssChnAttr[VpssChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;
		abChnEnable[VpssChn] = AR_TRUE;

		// set scale mode
		if (mask & VPSS_TEST_CROP)
		{
			astVpssChnAttr[VpssChn].u32ScaleMode = 0;
			SAMPLE_PRT("set scale mode %d === !\n", astVpssChnAttr[VpssChn].u32ScaleMode);
		}

		s32Ret = SAMPLE_VIO_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr, mask);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}

		// add ext channel
		if (mask & VPSS_TEST_EXTCHAN)
		{
			stVpssExtChnAttr.s32BindChn = VpssChn;
			memcpy(&stVpssExtChnAttr.enChnMode, (const VPSS_CHN_ATTR_S *)&astVpssChnAttr[VpssChn].enChnMode, sizeof(VPSS_CHN_ATTR_S));
			s32Ret = AR_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssExtChn, &stVpssExtChnAttr);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VPSS_SetExtChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
				return s32Ret;
			}

			s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssExtChn);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
				return AR_FAILURE;
			}
		}

		// add crop test function
		if (mask & VPSS_TEST_CROP)
		{
			VPSS_CROP_INFO_S stCropInfo = {
				.bEnable = AR_TRUE,
				.enCropCoordinate = VPSS_CROP_ABS_COOR,
				.stCropRect = {
					.s32X = 0,
					.s32Y = 0,
					.u32Width = 640,
					.u32Height = 480,
				},
			};

			s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stCropInfo);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VPSS_SetChnCrop failed. s32Ret: 0x%x !\n", s32Ret);
				return s32Ret;
			}

			SAMPLE_PRT("AR_MPI_VPSS_SetGrpCrop pass.=== !\n");
		}

		// add rotation test function
		if (mask & VPSS_TEST_ROTATION)
		{
			ROTATION_E rotation = ROTATION_90;

			s32Ret = AR_MPI_VPSS_SetChnRotation(VpssGrp, VpssChn, rotation);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("AR_MPI_VPSS_SetChnRotation failed. s32Ret: 0x%x !\n", s32Ret);
				return s32Ret;
			}

			SAMPLE_PRT("AR_MPI_VPSS_SetChnRotation %d pass.=== !\n", rotation);
		}

		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Vpss_imx307(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		AR_S32 u32UseVpss = 1;
		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		RECT_S stDefDispRect = {0, 0, 1920, 1080};
		SIZE_S stDefImageSize = {1920, 1080};
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VPSS_GRP VpssGrp = 0;
		VPSS_CHN VpssChn = VPSS_CHN0;
		AR_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
		AR_U32 VpssTestMask = VPSS_TEST_CROP | VPSS_TEST_ROTATION | VPSS_TEST_MIRROR | VPSS_TEST_FLIP | VPSS_TEST_EXTCHAN | VPSS_TEST_REGION_LUMA;
		VPSS_CHN VpssExtChn = 5;

		VIDEO_REGION_INFO_S stRegionInfo = {0};
		AR_U64 *pu64LumaData = NULL;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 10;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		/*start vi*/
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		if (u32UseVpss)
		{
			/* config & start vpss */
			s32Ret = SAMPLE_VPSS_Config_Test(VpssGrp, VpssChn, VpssExtChn, abChnEnable, 1920, 1080, VpssTestMask);
			if (AR_SUCCESS != s32Ret)
			{
				goto EXIT1;
			}

			s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT2;
			}
		}

		/*config vo*/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);

		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		stVoConfig.stDispRect = stDefDispRect;
		stVoConfig.stImageSize = stDefImageSize;

		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		else
		{
			stVoConfig.enVoIntfType = VO_INTF_HDMI;
		}
		/*start vo*/
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT5;
		}

		if (u32UseVpss)
		{
			/*vpss bind vo*/
			if (VpssTestMask & VPSS_TEST_EXTCHAN)
			{
				s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
			}
			else
			{
				s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
			}
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT6;
			}

			if (VpssTestMask & VPSS_TEST_REGION_LUMA)
			{
				stRegionInfo.u32RegionNum = 2;
				stRegionInfo.pstRegion = malloc(stRegionInfo.u32RegionNum * sizeof(*stRegionInfo.pstRegion));
				if (stRegionInfo.pstRegion == NULL)
				{
					SAMPLE_PRT("ar_malloc failed!\n");
					goto EXIT7;
				}

				stRegionInfo.pstRegion[0].s32X = 32;
				stRegionInfo.pstRegion[0].s32Y = 32;
				stRegionInfo.pstRegion[0].u32Width = 128;
				stRegionInfo.pstRegion[0].u32Height = 128;

				stRegionInfo.pstRegion[1].s32X = 320;
				stRegionInfo.pstRegion[1].s32Y = 320;
				stRegionInfo.pstRegion[1].u32Width = 160;
				stRegionInfo.pstRegion[1].u32Height = 160;

				pu64LumaData = malloc(stRegionInfo.u32RegionNum * sizeof(AR_U64));
				if (pu64LumaData == NULL)
				{
					SAMPLE_PRT("ar_malloc failed!\n");
					goto EXIT8;
				}
				memset(pu64LumaData, 0, stRegionInfo.u32RegionNum * sizeof(*pu64LumaData));
				s32Ret = AR_MPI_VPSS_GetRegionLuma(VpssGrp, VpssChn, &stRegionInfo, pu64LumaData, 1000);
				if (AR_SUCCESS != s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VPSS_GetRegionLuma failed. s32Ret: 0x%x !\n", s32Ret);
					goto EXIT9;
				}
			}
		}
		else
		{
			s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
		}

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

	EXIT9:
		if (pu64LumaData)
		{
			free(pu64LumaData);
		}

	EXIT8:
		if (stRegionInfo.pstRegion)
		{
			free(stRegionInfo.pstRegion);
		}

	EXIT7:
		if (u32UseVpss)
		{
			if (VpssTestMask & VPSS_TEST_EXTCHAN)
			{
				SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
			}
			else
			{
				SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
			}
		}
		else
		{
			SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
		}
	EXIT6:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);
	EXIT5:
		if (u32UseVpss)
		{
			SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
		}
	EXIT2:
		if (u32UseVpss)
		{
			SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
		}
	EXIT1:
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
	AR_S32 SAMPLE_VIO_Vpss_imx307_reset_vi(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		AR_S32 u32UseVpss = 1;
		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		RECT_S stDefDispRect = {0, 0, 1920, 1080};
		SIZE_S stDefImageSize = {1920, 1080};
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VPSS_GRP VpssGrp = 0;
		VPSS_CHN VpssChn = VPSS_CHN0;
		AR_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
		AR_U32 VpssTestMask = 0;
		VPSS_CHN VpssExtChn = 5;

		VIDEO_REGION_INFO_S stRegionInfo = {0};
		AR_U64 *pu64LumaData = NULL;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 10;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		/*start vi*/
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		if (u32UseVpss)
		{
			/* config & start vpss */
			s32Ret = SAMPLE_VPSS_Config_Test(VpssGrp, VpssChn, VpssExtChn, abChnEnable, 1920, 1080, VpssTestMask);
			if (AR_SUCCESS != s32Ret)
			{
				goto EXIT1;
			}

			s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT2;
			}
		}

		/*config vo*/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);

		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		stVoConfig.stDispRect = stDefDispRect;
		stVoConfig.stImageSize = stDefImageSize;

		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		else
		{
			stVoConfig.enVoIntfType = VO_INTF_HDMI;
		}
		/*start vo*/
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT5;
		}

		if (u32UseVpss)
		{
			/*vpss bind vo*/
			if (VpssTestMask & VPSS_TEST_EXTCHAN)
			{
				s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
			}
			else
			{
				s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
			}
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT6;
			}

			if (VpssTestMask & VPSS_TEST_REGION_LUMA)
			{
				stRegionInfo.u32RegionNum = 2;
				stRegionInfo.pstRegion = malloc(stRegionInfo.u32RegionNum * sizeof(*stRegionInfo.pstRegion));
				if (stRegionInfo.pstRegion == NULL)
				{
					SAMPLE_PRT("ar_malloc failed!\n");
					goto EXIT7;
				}

				stRegionInfo.pstRegion[0].s32X = 32;
				stRegionInfo.pstRegion[0].s32Y = 32;
				stRegionInfo.pstRegion[0].u32Width = 128;
				stRegionInfo.pstRegion[0].u32Height = 128;

				stRegionInfo.pstRegion[1].s32X = 320;
				stRegionInfo.pstRegion[1].s32Y = 320;
				stRegionInfo.pstRegion[1].u32Width = 160;
				stRegionInfo.pstRegion[1].u32Height = 160;

				pu64LumaData = malloc(stRegionInfo.u32RegionNum * sizeof(AR_U64));
				if (pu64LumaData == NULL)
				{
					SAMPLE_PRT("ar_malloc failed!\n");
					goto EXIT8;
				}
				memset(pu64LumaData, 0, stRegionInfo.u32RegionNum * sizeof(*pu64LumaData));
				s32Ret = AR_MPI_VPSS_GetRegionLuma(VpssGrp, VpssChn, &stRegionInfo, pu64LumaData, 1000);
				if (AR_SUCCESS != s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VPSS_GetRegionLuma failed. s32Ret: 0x%x !\n", s32Ret);
					goto EXIT9;
				}
			}
		}
		else
		{
			s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
		}

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		int reset_ref = 0;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
			reset_ref++;
			if (reset_ref > 200)
			{
				SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
				SAMPLE_COMM_VI_StopVi(&stViConfig);
				SAMPLE_COMM_VI_StartVi(&stViConfig);
				SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
				reset_ref = 0;
			}
		}

	EXIT9:
		if (pu64LumaData)
		{
			free(pu64LumaData);
		}

	EXIT8:
		if (stRegionInfo.pstRegion)
		{
			free(stRegionInfo.pstRegion);
		}

	EXIT7:
		if (u32UseVpss)
		{
			if (VpssTestMask & VPSS_TEST_EXTCHAN)
			{
				SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
			}
			else
			{
				SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
			}
		}
		else
		{
			SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
		}
	EXIT6:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);
	EXIT5:
		if (u32UseVpss)
		{
			SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
		}
	EXIT2:
		if (u32UseVpss)
		{
			SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
		}
	EXIT1:
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
#ifdef AR_FEAT_EIS
	AR_S32 SAMPLE_VIO_Vpss_Eis_imx415(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		AR_S32 u32UseVpss = 1;
		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		RECT_S stDefDispRect = {0, 0, 1920, 1080};
		SIZE_S stDefImageSize = {1920, 1080};
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VPSS_GRP VpssGrp = 0;
		VPSS_CHN VpssChn = VPSS_CHN0;
		AR_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
		AR_U32 VpssTestMask = VPSS_TEST_EIS;
		VPSS_CHN VpssExtChn = 5;

		VIDEO_REGION_INFO_S stRegionInfo = {0};
		AR_U64 *pu64LumaData = NULL;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR7_TYPE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 3;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height + 100, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		// for vin ch1
		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width / 2, (stSize.u32Height + 100) / 2, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 10;
		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[2].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[2].u32BlkCnt = 10;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		/*start vi*/
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 400000000, 200000000);
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// start the vi ch1
		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = 1;

		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width / 2;
		stChnAttr.stSize.u32Height = stSize.u32Height / 2;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		if (u32UseVpss)
		{
			/* config & start vpss */
			s32Ret = SAMPLE_VPSS_Config_Test(VpssGrp, VpssChn, VpssExtChn, abChnEnable, 1920, 1080, VpssTestMask);
			if (AR_SUCCESS != s32Ret)
			{
				goto EXIT1;
			}

			s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT2;
			}
		}

		/*config vo*/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);

		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_2MUX;
		stVoConfig.stDispRect = stDefDispRect;
		stVoConfig.stImageSize = stDefImageSize;

		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		else
		{
			stVoConfig.enVoIntfType = VO_INTF_HDMI;
		}
		/*start vo*/
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT5;
		}

		if (u32UseVpss)
		{
			/*vpss bind vo*/
			if (VpssTestMask & VPSS_TEST_EXTCHAN)
			{
				s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
			}
			else
			{
				s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
			}
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
				goto EXIT6;
			}

			if (VpssTestMask & VPSS_TEST_REGION_LUMA)
			{
				stRegionInfo.u32RegionNum = 2;
				stRegionInfo.pstRegion = malloc(stRegionInfo.u32RegionNum * sizeof(*stRegionInfo.pstRegion));
				if (stRegionInfo.pstRegion == NULL)
				{
					SAMPLE_PRT("ar_malloc failed!\n");
					goto EXIT7;
				}

				stRegionInfo.pstRegion[0].s32X = 32;
				stRegionInfo.pstRegion[0].s32Y = 32;
				stRegionInfo.pstRegion[0].u32Width = 128;
				stRegionInfo.pstRegion[0].u32Height = 128;

				stRegionInfo.pstRegion[1].s32X = 320;
				stRegionInfo.pstRegion[1].s32Y = 320;
				stRegionInfo.pstRegion[1].u32Width = 160;
				stRegionInfo.pstRegion[1].u32Height = 160;

				pu64LumaData = malloc(stRegionInfo.u32RegionNum * sizeof(AR_U64));
				if (pu64LumaData == NULL)
				{
					SAMPLE_PRT("ar_malloc failed!\n");
					goto EXIT8;
				}
				memset(pu64LumaData, 0, stRegionInfo.u32RegionNum * sizeof(*pu64LumaData));
				s32Ret = AR_MPI_VPSS_GetRegionLuma(VpssGrp, VpssChn, &stRegionInfo, pu64LumaData, 1000);
				if (AR_SUCCESS != s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VPSS_GetRegionLuma failed. s32Ret: 0x%x !\n", s32Ret);
					goto EXIT9;
				}
			}
		}
		else
		{
			s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
		}
		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn1, stVoConfig.VoDev, 1);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

	EXIT9:
		if (pu64LumaData)
		{
			free(pu64LumaData);
		}

	EXIT8:
		if (stRegionInfo.pstRegion)
		{
			free(stRegionInfo.pstRegion);
		}

	EXIT7:
		if (u32UseVpss)
		{
			if (VpssTestMask & VPSS_TEST_EXTCHAN)
			{
				SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssExtChn, stVoConfig.VoDev, VoChn);
			}
			else
			{
				SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
			}
		}
		else
		{
			SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, stVoConfig.VoDev, VoChn);
		}
		SAMPLE_COMM_VI_UnBind_VO(ViPipe, ViChn, stVoConfig.VoDev, 1);
	EXIT6:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);
	EXIT5:
		if (u32UseVpss)
		{
			SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
		}
	EXIT2:
		if (u32UseVpss)
		{
			SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
		}
	EXIT1:
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	static void *frame_run_eis(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		int fd = -1;
		char name[256];
		while (1)
		{
			// SAMPLE_PRT("get frame = %d : pipe_id = %d, ch_id = %d, vo_ch_id = %d\n", ref, run_inf->pipe_id, run_inf->ch_id, run_inf->vo_ch_id);
			int status = AR_MPI_VI_GetChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}

			if (ref > 1000)
			{
				// SAMPLE_PRT("get frame %lx \n",FrameInfo.stVFrame.u64PhyAddr[0]);
				// ref=0;
			}
			ref++;

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",run_inf->pipe_id,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			STRU_EIS_PROCESS_PARA para =
				{
					.s32_dev_id = 0,
					.s32_chn_id = 0,
					.u32_walign = 64,
					.u32_halign = 64};
			// call the eis to process
			ar_hal_vb_user_add_by_frameinfo(&FrameInfo, AR_VB_UID_VPSS);
			AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo);

			sample_eis_process_frame(&FrameInfo, para);
			// push to display
			send_to_fw_display(&g_vo_obj, run_inf->vo_ch_id, &FrameInfo);

			ar_hal_vb_user_sub_by_frameinfo(&FrameInfo, AR_VB_UID_VPSS);

			if (s32nframes >= 0)
			{
				s32cnt = run_inf->nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		ar_printf("frame run pipe_id=%d run_inf=%d exit", run_inf->pipe_id, run_inf->ch_id);
		if (fd > 0)
			close(fd);
		return NULL;
	}

	static int eis_pra_init(STRU_EIS_TUNING_T *eis_algo_tuning)
	{

		memset(eis_algo_tuning, 0, sizeof(STRU_EIS_TUNING_T));
		eis_algo_tuning->enable = 1;
		eis_algo_tuning->eis_enable = 1;
		eis_algo_tuning->use_current_frame = 0;
		eis_algo_tuning->angle_th = 10 * 32768;
		eis_algo_tuning->zero_cali_frames = 100;
		eis_algo_tuning->zero_cali_th[0] = 0.00001;
		eis_algo_tuning->zero_cali_th[1] = 0.00001;
		eis_algo_tuning->zero_cali_th[2] = 0.00001;
		eis_algo_tuning->imu_pts_offset_start_ms = 33 / 2;
		eis_algo_tuning->imu_pts_offset_end_ms = 33 / 2;
		eis_algo_tuning->gain = 1.0;
		eis_algo_tuning->crop_pix_x = 128;
		eis_algo_tuning->crop_pix_y = 128;
		// 几遍矫正系数
		eis_algo_tuning->dist_coeff[0] = -0.5989 * (1 << 14);
		eis_algo_tuning->dist_coeff[1] = 0.3942 * (1 << 14);
		eis_algo_tuning->dist_coeff[2] = -0.0 * (1 << 14);

		// 陀螺仪与sensor 位置关系矫正系数
		eis_algo_tuning->A[0] = 0;
		eis_algo_tuning->A[1] = -1;
		eis_algo_tuning->A[2] = 0;
		eis_algo_tuning->A[3] = -1;
		eis_algo_tuning->A[4] = 0;
		eis_algo_tuning->A[5] = 0;
		eis_algo_tuning->A[6] = 0;
		eis_algo_tuning->A[7] = 0;
		eis_algo_tuning->A[8] = -1;

#if 0
    //相机内参
    eis_algo_tuning->k[0]=4436;
    eis_algo_tuning->k[1]=0;
    eis_algo_tuning->k[2]=1824;
    eis_algo_tuning->k[3]=0;
    eis_algo_tuning->k[4]=4436;
    eis_algo_tuning->k[5]=1114;
    eis_algo_tuning->k[6]=0;
    eis_algo_tuning->k[7]=0;
    eis_algo_tuning->k[8]=1;

    //相机内参的妮
    eis_algo_tuning->k_inv[0]=0.0002*(1<<20)-1;
    eis_algo_tuning->k_inv[1]=0;
    eis_algo_tuning->k_inv[2]=-0.4113*(1<<20)-1;
    eis_algo_tuning->k_inv[3]=0;
    eis_algo_tuning->k_inv[4]=0.0002*(1<<20)-1;
    eis_algo_tuning->k_inv[5]=-0.2512*(1<<20)-1;
    eis_algo_tuning->k_inv[6]=0;
    eis_algo_tuning->k_inv[7]=0;
    eis_algo_tuning->k_inv[8]=1*(1<<20)-1;
#else
		// 相机内参
		eis_algo_tuning->k[0] = 2268;
		eis_algo_tuning->k[1] = 0;
		eis_algo_tuning->k[2] = 923;
		eis_algo_tuning->k[3] = 0;
		eis_algo_tuning->k[4] = 2269;
		eis_algo_tuning->k[5] = 552;
		eis_algo_tuning->k[6] = 0;
		eis_algo_tuning->k[7] = 0;
		eis_algo_tuning->k[8] = 1;

// 相机内参的妮
#if 0
    eis_algo_tuning->k_inv[0]=0.00044*(1<<20)-1;
    eis_algo_tuning->k_inv[1]=0;
    eis_algo_tuning->k_inv[2]=-0.40696*(1<<20)-1;
    eis_algo_tuning->k_inv[3]=0;
    eis_algo_tuning->k_inv[4]=0.00044*(1<<20)-1;
    eis_algo_tuning->k_inv[5]=-0.24327*(1<<20)-1;
    eis_algo_tuning->k_inv[6]=0;
    eis_algo_tuning->k_inv[7]=0;
    eis_algo_tuning->k_inv[8]=1*(1<<20)-1;
#endif
		eis_algo_tuning->k_inv[0] = 462;
		eis_algo_tuning->k_inv[1] = 0;
		eis_algo_tuning->k_inv[2] = -426735;
		eis_algo_tuning->k_inv[3] = 0;
		eis_algo_tuning->k_inv[4] = 462;
		eis_algo_tuning->k_inv[5] = -255096;
		eis_algo_tuning->k_inv[6] = 0;
		eis_algo_tuning->k_inv[7] = 0;
		eis_algo_tuning->k_inv[8] = 1048576;

#endif

		return 0;
	}

	AR_S32 SAMPLE_VIO_Eis_imx415(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;
		AR_S32 u32UseVpss = 1;
		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		RECT_S stDefDispRect = {0, 0, 1920, 1080};
		SIZE_S stDefImageSize = {1920, 1080};
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		VPSS_GRP VpssGrp = 0;
		VPSS_CHN VpssChn = VPSS_CHN0;
		AR_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
		AR_U32 VpssTestMask = 0;
		VPSS_CHN VpssExtChn = 5;

		SAMPLE_SNS_TYPE_E enSnsType = SENSOR7_TYPE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 3;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height + 100, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		// for vin ch1
		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width / 2, (stSize.u32Height + 100) / 2, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 10;
		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[2].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[2].u32BlkCnt = 10;
		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		/*start vi*/
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 400000000, 200000000);
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		// start the vi ch1
		VI_CHN_ATTR_S stChnAttr;
		VI_CHN ViChn1 = 1;

		/*start vi chn1*/
		s32Ret = AR_MPI_VI_GetChnAttr(ViPipe, ViChn, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}
		stChnAttr.stSize.u32Width = stSize.u32Width / 2;
		stChnAttr.stSize.u32Height = stSize.u32Height / 2;

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		// we init the eis
		STRU_EIS_TUNING_T eis_algo_tuning = {0};
		eis_pra_init(&eis_algo_tuning);
		sample_eis_init(&eis_algo_tuning);

		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

		pthread_t ch_thread = {0};
		pthread_t ch1_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id = 0;
		pipe0_ch0.ch_id = 0;
		pipe0_ch0.vo_ch_id = 0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);

		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
		pipe0_ch1.pipe_id = 0;
		pipe0_ch1.ch_id = 1;
		pipe0_ch1.vo_ch_id = 1;
		pipe0_ch1.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run_eis, &pipe0_ch1);

		pthread_join(ch_thread, NULL);
		pthread_join(ch1_thread, NULL);

		sample_eis_reset();
		sample_eis_stop();
		sample_eis_exit();

		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
#endif

	AR_S32 SAMPLE_VIO_Bind_imx307_dig_gain_test(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(0, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		// set the max sensor gain to 16
		ISP_EXPOSURE_ATTR_S *pstExposureAttr = malloc(sizeof(ISP_EXPOSURE_ATTR_S));
		pstExposureAttr->bEnable = 1;
		pstExposureAttr->bManual = 0;
		AR_MPI_ISP_GetExposureAttr(ViPipe, pstExposureAttr);
		pstExposureAttr->stAutoExposure.exp_table_para.max_sensor_gain = 16;
		pstExposureAttr->stAutoExposure.exp_table_para.enable_isp_digital_gain = 1;
		pstExposureAttr->stAutoExposure.exp_table_para.max_isp_gain1 = 31.9;
		pstExposureAttr->bEnable = 1;
		pstExposureAttr->bManual = 0;
		AR_MPI_ISP_SetExposureAttr(ViPipe, pstExposureAttr);
		free(pstExposureAttr);

		ISP_3A_INFO_EXT_TIDY_ATTR_S *pst3aInfoExtTidyAttr = malloc(sizeof(ISP_3A_INFO_EXT_TIDY_ATTR_S));

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 300)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);

				AR_MPI_ISP_Get3aInfoExtTidyAttr(ViPipe, pst3aInfoExtTidyAttr);

				ar_printf("gain=%f ", pst3aInfoExtTidyAttr->st3aInfoExt.aec_info.gain);
				ar_printf("sensor_gain=%f ", pst3aInfoExtTidyAttr->st3aInfoExt.aec_info.sensor_gain);
				ar_printf("isp_gain1=%f ", pst3aInfoExtTidyAttr->st3aInfoExt.aec_info.isp_gain1);
				ar_printf("isp_gain2=%f \n", pst3aInfoExtTidyAttr->st3aInfoExt.aec_info.isp_gain2);
				// break;
				ref = 0;
			}

			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		free(pst3aInfoExtTidyAttr);
		PAUSE();

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Test_Interlace(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 1;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 2;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = IRAY_PAL_DVP_SINGLE_CH;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Width = 720;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.u32Height = 288;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		stSize.u32Width = 720;
		stSize.u32Height = 288;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_AR_MPI_VIN_OpenDev(1, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		system("devmem 0x010f0008 32 0x00000041"); // bit6 //hardware reason

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 300)
			{
				ar_printf("get frame %lx enField=%d \n", FrameInfo.stVFrame.u64PhyAddr[0], FrameInfo.stVFrame.enField);

				// break;
				ref = 0;
			}

			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Bind_imx347(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_SNS_TYPE_E enSnsType = SENSOR24_TYPE;
		SAMPLE_VI_CONFIG_S stViConfig;

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		g_enSnsType[s32WorkSnsId] = enSnsType;

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		ar_memset(&stViConfig.astViInfo[s32WorkSnsId].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bMultiPipe = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.bVcNumCfged = AR_FALSE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[0].u32BlkCnt = 6; // 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[1].u32BlkCnt = 1; // 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		SAMPLE_PRT("test test ddr mode\n", s32Ret);
		SAMPLE_AR_MPI_VIN_OpenDev(0, 400000000, 400000000, 600000000, 200000000);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_1MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, ViChn, 0, VoChn);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);

		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;

		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, VoChn);

	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_SC2210(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig = {0};

		VO_CHN VoChn = 0;

		SIZE_S stSize = {1920, 1080};
		VB_CONFIG_S stVbConf = {0};
		PIC_SIZE_E enPicSize = 0;
		AR_U32 u32BlkSize = 0;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		// config vi
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SC_SC2210_1080P;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		// config vb
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.u32MaxPoolCnt = 2;
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		// start vi
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = 1920;
		chan_pos.h = 1080;
		ar_vo_surface_init(&g_vo_obj, VoChn, &chan_pos);

		// get frame buffer
		VIDEO_FRAME_INFO_S FrameInfo = {0};
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}

				continue;
			}

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		// deinit display
		close_display_surface(&g_vo_obj, VoChn);
		close_display(&g_vo_obj);

		// stop vi
		SAMPLE_COMM_VI_StopVi(&stViConfig);

	EXIT:
		SAMPLE_COMM_SYS_Exit();

		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_OS05A20(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig = {0};

		VO_CHN VoChn = 0;

		SIZE_S stSize = {2688, 1944};
		VB_CONFIG_S stVbConf = {0};
		PIC_SIZE_E enPicSize = 0;
		AR_U32 u32BlkSize = 0;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		// config vi
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = OV_OS05A20_2688x1944;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		// config vb
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.u32MaxPoolCnt = 2;
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}
		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 100000000;
		}
		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}
		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		// start vi
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = 1920;
		chan_pos.h = 1080;
		ar_vo_surface_init(&g_vo_obj, VoChn, &chan_pos);

		// get frame buffer
		VIDEO_FRAME_INFO_S FrameInfo = {0};
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}

				continue;
			}

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		// deinit display
		close_display_surface(&g_vo_obj, VoChn);
		close_display(&g_vo_obj);

		// stop vi
		SAMPLE_COMM_VI_StopVi(&stViConfig);

	EXIT:
		SAMPLE_COMM_SYS_Exit();

		return s32Ret;
	}

	static void *dma_cpy_process_thread(void *arg)
	{
		ar_signal_t af_signal = (ar_signal_t)arg;
		int ref = 0;
		while (1)
		{

			printf("malloc free %d \n", ref);
			ref++;
			int ret = 0;
			AR_U32 size = 128 * 2 * 8 * 8 * 1024;
			AR_U64 src_phy = 0;
			AR_VOID *src_virt = NULL;
			ret = ar_hal_sys_mmz_alloc(&src_phy, &src_virt, NULL, NULL, size);
			if (ret)
			{
				return NULL;
			}
			AR_U64 dst_phy = 0;
			AR_VOID *dst_virt = NULL;
			ret = ar_hal_sys_mmz_alloc_cached(&dst_phy, &dst_virt, NULL, NULL, size);
			if (ret)
			{
				return NULL;
			}
			ar_hal_sys_mmz_free(src_phy, src_virt);
			ar_hal_sys_mmz_free(dst_phy, dst_virt);
		}
#if 0
    while(1)
    {
        ar_signal_wait_timeout_binary(af_signal,2000);

        if(g_exit)
        {
            break;
        }
		ar_hal_sys_memcpy_pa((AR_U64)dst_phy,(AR_U64)src_phy,size);
    }
#endif
		return NULL;
	}

#define SENDSIGTIME 10

	pthread_cond_t g_cond;
	pthread_mutex_t g_mutex;

	void thread1(void *arg)
	{
		int inArg = (int)arg;
		int ret = 0;
		struct timeval now;
		struct timespec outtime;
		unsigned int times = 0;
		prctl(PR_SET_NAME, "thread1");

		printf("in thread1\n");
		while (1)
		{
			pthread_mutex_lock(&g_mutex);

			gettimeofday(&now, NULL);
			outtime.tv_sec = now.tv_sec + 5;
			outtime.tv_nsec = now.tv_usec * 1000;

			ret = pthread_cond_timedwait(&g_cond, &g_mutex, &outtime);
			// ret = pthread_cond_wait(&g_cond, &g_mutex);
			pthread_mutex_unlock(&g_mutex);

			// printf("thread 1 ret: %d\n", ret);
			times++;
			if ((times & 0x3fff) == 0)
			{
				printf("times: %d\n", times);
			}
		}
	}

	int main_my(void)
	{
		pthread_t id1;
		int ret;

		pthread_cond_init(&g_cond, NULL);
		pthread_mutex_init(&g_mutex, NULL);

		ret = pthread_create(&id1, NULL, (void *)thread1, (void *)1);
		if (0 != ret)
		{
			printf("thread 1 create failed!\n");
			return 1;
		}

		printf("in main\n");
		while (1)
		{
#if 0
    printf("等待%ds发送信号!\n", SENDSIGTIME);
    sleep(SENDSIGTIME);
    printf("正在发送信号....\n");
#endif
			pthread_mutex_lock(&g_mutex);
			pthread_cond_signal(&g_cond);
			pthread_mutex_unlock(&g_mutex);
		}

		pthread_join(id1, NULL);
		pthread_cond_destroy(&g_cond);
		pthread_mutex_destroy(&g_mutex);

		return 0;
	}

	AR_S32 SAMPLE_VIO_dma_cpy_test(SAMPLE_VIO_PRA *ppra)
	{

		main_my();
#if 0
    VB_CONFIG_S        stVbConf   = {0};
    ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

    uint32_t u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
    stVbConf.u32MaxPoolCnt              = 2;
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize = VI_GetRawBufferSize(1920, 1080, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 4;


	SAMPLE_COMM_SYS_Init(&stVbConf);

	ar_signal_t signal;

	signal=ar_create_signal_binary();

	ar_os_thread_attr_t ctrl_attr = {0};
	ctrl_attr.name = "dma_cpy";
	ctrl_attr.stack_size = 16 * 1024;
	ar_os_thread_id_t event_thread_id;
	event_thread_id = ar_osal_thread_new(dma_cpy_process_thread, signal, &ctrl_attr);

    while(1)
    {
        if(g_exit)
        {
            break;
        }
		ar_delay(40);
		ar_signal_binary(signal);
    }
	ar_osal_thread_join(event_thread_id);
#endif
	}
	AR_S32 SAMPLE_VIO_Only_For_Bind_With_OS05A20_4M(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig = {0};

		VO_CHN VoChn = 0;

		SIZE_S stSize = {2336, 1752};
		VB_CONFIG_S stVbConf = {0};
		PIC_SIZE_E enPicSize = 0;
		AR_U32 u32BlkSize = 0;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		// config vi
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = OV_OS05A20_2336x1752;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		// config vb
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.u32MaxPoolCnt = 2;
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}
		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 100000000;
		}
		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}
		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		// start vi
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		SAMPLE_COMM_VI_StopVi(&stViConfig);

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = 1920;
		chan_pos.h = 1080;
		ar_vo_surface_init(&g_vo_obj, VoChn, &chan_pos);

		// get frame buffer
		VIDEO_FRAME_INFO_S FrameInfo = {0};
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{

			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);
			if (status)
			{
				if (g_exit)
				{
					g_exit = 0;
					break;
				}

				continue;
			}

			// push to display
			send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}

		// deinit display
		close_display_surface(&g_vo_obj, VoChn);
		close_display(&g_vo_obj);

		// stop vi
		SAMPLE_COMM_VI_StopVi(&stViConfig);

	EXIT:
		SAMPLE_COMM_SYS_Exit();

		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_For_Bind_With_OS05A20_4M_3ch(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig = {0};

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		SIZE_S stSize = {2336, 1752};
		VB_CONFIG_S stVbConf = {0};
		PIC_SIZE_E enPicSize = 0;
		AR_U32 u32BlkSize = 0;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		// config vi
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = OV_OS05A20_2336x1752;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		// config vb
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.u32MaxPoolCnt = 2;
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 15;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 300000000;
		}
		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}
		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}
		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		// start vi
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		int s32pra_index = 0;
		VI_CHN_ATTR_S stChnAttr;
		AR_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr);

		s32pra_index = sample_find_pra_by_name("-dis_anti", ppra->argc, ppra->argv);

		if (s32pra_index > 0)
		{
			stChnAttr.s32AntiDisable = atoi(ppra->argv[s32pra_index + 1]);
		}

		stChnAttr.stSize.u32Width = 1280;
		stChnAttr.stSize.u32Height = 720;

		s32pra_index = sample_find_pra_by_name("-size1", ppra->argc, ppra->argv);

		if (s32pra_index > 0)
		{
			stChnAttr.stSize.u32Width = atoi(ppra->argv[s32pra_index + 1]);
			stChnAttr.stSize.u32Height = atoi(ppra->argv[s32pra_index + 2]);
		}

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, 1, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, 1);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		stChnAttr.stSize.u32Width = 1920;
		stChnAttr.stSize.u32Height = 1080;

		s32pra_index = sample_find_pra_by_name("-size2", ppra->argc, ppra->argv);

		if (s32pra_index > 0)
		{
			stChnAttr.stSize.u32Width = atoi(ppra->argv[s32pra_index + 1]);
			stChnAttr.stSize.u32Height = atoi(ppra->argv[s32pra_index + 2]);
		}

		s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, 3, &stChnAttr);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_GetChnAttr failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		s32Ret = AR_MPI_VI_EnableChn(ViPipe, 3);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("AR_MPI_VI_EnableChn failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT;
		}

		/************************************************
		 start V0
		*************************************************/
		SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
		stVoConfig.VoDev = SAMPLE_VO_DEV_DHD0;
		stVoConfig.enVoIntfType = VO_INTF_HDMI;
		stVoConfig.enIntfSync = VO_OUTPUT_1080P60;
		stVoConfig.enPicSize = enPicSize;
		stVoConfig.u32DisBufLen = 3;
		stVoConfig.enDstDynamicRange = enDynamicRange;
		stVoConfig.enVoMode = VO_MODE_4MUX;
		if (ppra->u32votype)
		{
			stVoConfig.enVoIntfType = VO_INTF_MIPI;
			stVoConfig.enIntfSync = VO_OUTPUT_USER;
		}
		s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
			goto EXIT2;
		}
		/************************************************
		step 8:  Vi bind VO
		*************************************************/

		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO \n");

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, 0, 0, 0);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, 1, 0, 1);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		s32Ret = SAMPLE_COMM_VI_Bind_VO(ViPipe, 3, 0, 2);

		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VO failed with %d!\n", s32Ret);
			goto EXIT2;
		}

		// SAMPLE_VIO_region_vi_attach_test("/local/logo.bmp",0);
		VIDEO_FRAME_INFO_S FrameInfo;
		int ref;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		while (1)
		{
			int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 5000);

			if (status)
			{

				if (g_exit)
				{
					g_exit = 0;
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				SAMPLE_PRT("get frame %lx\n", FrameInfo.stVFrame.u64PhyAddr[0]);
				// break;
				ref = 0;
			}
			ref++;
			// push to display
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = ppra->s32nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					s32cnt = 0;
					break;
				}
			}

			if (g_exit)
			{
				g_exit = 0;
				break;
			}
		}
		SAMPLE_COMM_VI_UnBind_VO(0, 0, 0, 0);
		SAMPLE_COMM_VI_UnBind_VO(0, 1, 0, 1);
		SAMPLE_COMM_VI_UnBind_VO(0, 3, 0, 2);
	EXIT2:
		SAMPLE_COMM_VO_StopVO(&stVoConfig);
		// stop vi
		SAMPLE_COMM_VI_StopVi(&stViConfig);

	EXIT:
		SAMPLE_COMM_SYS_Exit();

		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_Dual_IMX307_RTD6122C(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		AR_S32 s32ViCnt = 2;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;

		SAMPLE_VI_CONFIG_S stViConfig = {0};

		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize;
		AR_U32 u32BlkSize;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

		/*config vi*/
		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.as32WorkingViId[1] = 1;
		stViConfig.as32WorkingViId[2] = 2;
		stViConfig.as32WorkingViId[3] = 3;

		stSize.u32Width = 1920;
		stSize.u32Height = 1080;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[0].u32BlkCnt = 10;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt = 4;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		// AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq
		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 100000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

#if 0
	 if(ppra->cam_mode>2){
		SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);
	 }else{ 	   
		SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre,ppra->vif_fre,ppra->isp_fre,ppra->pcs_fre);
	}
#endif

		SAMPLE_AR_MPI_VIN_OpenDev(2, 100000000, 100000000, 300000000, 100000000);

		s32WorkSnsId = 0;
		VI_DEV ViDev_vis = 0;
		VI_PIPE ViPipe_vis = ViDev_vis;
		VI_CHN ViChn_vis = 0;
		{
			DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
			PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
			VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
			COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

			g_enSnsType[s32WorkSnsId] = SONY_IMX290_MIPI_2M_30FPS_12BIT;

			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_30FPS_12BIT;

			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev_vis;
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe_vis;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn_vis;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}
		SAMPLE_PRT("%d---------------enSnsType=%d \n", s32WorkSnsId, stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

		s32WorkSnsId = 1;
		VI_DEV ViDev_raw = 1;
		VI_PIPE ViPipe_raw = ViDev_raw;
		VI_CHN ViChn_raw = 0;
		{
			DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
			PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
			VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
			COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

			g_enSnsType[s32WorkSnsId] = RTD6122C_IR_656X522;

			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = s32WorkSnsId;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = 0;
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = RTD6122C_IR_656X522;

			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev_raw;
			stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe_raw;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn_raw;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;
		}
		SAMPLE_PRT("%d---------------enSnsType=%d \n", s32WorkSnsId, stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType);

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}
		sleep(5);
		while (1)
		{

			if (g_exit)
				break;
#if 1
			ar_err("--------------------stop vi\n");
			s32Ret = SAMPLE_COMM_VI_StopSingleVi(&stViConfig.astViInfo[0]);
			if (s32Ret != AR_SUCCESS)
			{
				SAMPLE_PRT("SAMPLE_COMM_VI_StopIsp failed !\n");
				break;
			}
			sleep(2);
			ar_err("---------------------start vi\n");
			s32Ret = SAMPLE_COMM_VI_StartSingleVi(&stViConfig.astViInfo[0]);
			if (AR_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
				goto EXIT;
			}
			sleep(2);
#endif
		}

		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();

		return s32Ret;
	}

	AR_S32 SAMPLE_VIO_Only_raw_startsotp_use_default_attr(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret;

		ISP_SNS_OBJ_S *p_obj[2] = {&stSnsImx290Obj, &stSnsImx290Obj1};
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr[2];
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;

		VI_DEV ViDev[2] = {0, 1};
		VI_PIPE ViPipe[2] = {0, 1};
		VI_CHN ViChn[2] = {0, 2};
		AR_S32 mipi_index[2] = {0, 1};
		AR_S8 s8I2cDev[2] = {1, 2};

		VO_CHN VoChn = 0;
		SAMPLE_VO_CONFIG_S stVoConfig;

		if (p_obj[0]->pfnGetDefaultAttr)
		{
			p_obj[0]->pfnGetDefaultAttr(0, &default_attr[0]);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr[0].stPubAttr.stSnsSize;
		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 0;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 12;

		stVbConf.u32MaxPoolCnt++;

		// malloc camera 2 vb
		if (p_obj[1]->pfnGetDefaultAttr)
		{
			p_obj[1]->pfnGetDefaultAttr(1, &default_attr[1]);
		}
		else
		{
			ar_err("pfnGetDefaultAttr is null, exit the test");
			return -1;
		}
		stSize = default_attr[1].stPubAttr.stSnsSize;
		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed with %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 150000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 100000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(2, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		for (int i = 0; i < 2; i++)
		{
			if (ppra->settle > 0)
			{
				default_attr[i].stComboAttr.mipi_attr.settle_count = ppra->settle;
			}

			if (ppra->mipi_fre > 0)
			{
				default_attr[i].stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
			}
			if (ppra->dpcm == 1)
			{
				default_attr[i].stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
				default_attr[i].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			}
			else if (ppra->dpcm == 2)
			{
				default_attr[i].stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
				default_attr[i].stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
			}

			if (default_attr[i].stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
			{
				default_attr[i].stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
			}

			if (ppra->feature_mask)
			{
				default_attr[i].stPipeAttr.u32FeatureMask = ppra->feature_mask;
			}

			/*start vi*/
			AR_MPI_VI_SetMipiBindDev(ViDev[i], mipi_index[i]);
			default_attr[i].stComboAttr.devno = mipi_index[i];
			AR_MPI_VI_SetComboDevAttr(&default_attr[i].stComboAttr);
			AR_MPI_VI_SetDevAttr(ViDev[i], &default_attr[i].stDevAttr);
			AR_MPI_VI_EnableDev(ViDev[i]);
			VI_DEV_BIND_PIPE_S stDevBindPipe;
			stDevBindPipe.u32Num = 1;
			stDevBindPipe.PipeId[0] = ViPipe[i];
			AR_MPI_VI_SetDevBindPipe(ViDev[i], &stDevBindPipe);
			AR_MPI_VI_CreatePipe(ViPipe[i], &default_attr[i].stPipeAttr);
			AR_MPI_VI_StartPipe(ViPipe[i]);

			if (i == 1)
			{
				default_attr[i].stChnAttr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
			}

			AR_MPI_VI_SetChnAttr(ViPipe[i], ViChn[i], &default_attr[i].stChnAttr);
			AR_MPI_VI_EnableChn(ViPipe[i], ViChn[i]);

			ALG_LIB_S stAeLib;
			ALG_LIB_S stAwbLib;
			stAeLib.s32Id = ViPipe[i];
			stAwbLib.s32Id = ViPipe[i];
			strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
			strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

			if (p_obj[i]->pfnRegisterCallback != AR_NULL)
			{
				s32Ret = p_obj[i]->pfnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
					// goto EXIT;
				}
			}
			else
			{
				ar_err("sensor_register_callback failed with AR_NULL!\n");
				// goto EXIT;
			}

			if (AR_NULL != p_obj[i]->pfnSetBusInfo)
			{
				ISP_SNS_COMMBUS_U uSnsBusInfo;
				uSnsBusInfo.s8I2cDev = s8I2cDev[i];
				s32Ret = p_obj[i]->pfnSetBusInfo(ViPipe[i], uSnsBusInfo);

				if (s32Ret != AR_SUCCESS)
				{
					ar_err("set sensor bus info failed with %#x!\n", s32Ret);
					// goto EXIT;
				}
			}
			else
			{
				ar_err("not support set sensor bus info!\n");
				// goto EXIT;
			}
			AR_MPI_ISP_MemInit(ViPipe[i]);
			AR_MPI_ISP_SetPubAttr(ViPipe[i], &default_attr[i].stPubAttr);
			VI_PIPE_EXT_ATTR_S stPipeAttr = {0};
			AR_MPI_VI_GetPipeExtAttr(ViPipe[i], &stPipeAttr);
			stPipeAttr.bFoucs = 0;
			AR_MPI_VI_SetPipeExtAttr(ViPipe[i], &stPipeAttr);

			AR_MPI_ISP_Init(ViPipe[i]);
			AR_MPI_ISP_Run(ViPipe[i]);
		}
		// four layout
		AR_S32 mesh_w = 1920 / 2;
		AR_S32 mesh_h = 1080 / 2;
		// init display
		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}
		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = 640;
		chan_pos.h = 360;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		chan_pos.x = mesh_w;
		chan_pos.y = 0;
		chan_pos.w = 480;
		chan_pos.h = 640;
		ar_vo_surface_init(&g_vo_obj, 1, &chan_pos);

#if 0
		pthread_t	ch_thread;
		pthread_t	ch1_thread;
	
		pthread_attr_t* pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;
		pipe0_ch0.pipe_id=0;
		pipe0_ch0.ch_id=0;
		pipe0_ch0.vo_ch_id=0;
		pipe0_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch_thread, pstAttr, frame_run, &pipe0_ch0);
	
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe1_ch0;
		pipe1_ch0.pipe_id=1;
		pipe1_ch0.ch_id=0;
		pipe1_ch0.vo_ch_id=1;
		pipe1_ch0.nframes = ppra->s32nframes;
		pthread_create(&ch1_thread, pstAttr, frame_run, &pipe1_ch0);
	
		pthread_join(ch_thread,NULL);
		pthread_join(ch1_thread,NULL);
#else
		sleep(5);
		while (1)
		{
			sleep(2);
			for (int i = 0; i < 1; i++)
			{
				AR_MPI_ISP_Exit(ViPipe[i]);
				if (p_obj[i]->pfnUnRegisterCallback != AR_NULL)
				{
					ALG_LIB_S stAeLib;
					ALG_LIB_S stAwbLib;
					stAeLib.s32Id = ViPipe[i];
					stAwbLib.s32Id = ViPipe[i];
					strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
					strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
					s32Ret = p_obj[i]->pfnUnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);
					if (s32Ret != AR_SUCCESS)
					{
						ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
					}
				}
				AR_MPI_VI_DisableChn(ViPipe[i], ViChn[i]);
				AR_MPI_VI_StopPipe(ViPipe[i]);
				AR_MPI_VI_DestroyPipe(ViPipe[i]);
				AR_MPI_VI_DisableDev(ViDev[i]);
			}
			for (int i = 0; i < 1; i++)
			{
				/*start vi*/
				AR_MPI_VI_SetMipiBindDev(ViDev[i], mipi_index[i]);
				default_attr[i].stComboAttr.devno = mipi_index[i];
				AR_MPI_VI_SetComboDevAttr(&default_attr[i].stComboAttr);
				AR_MPI_VI_SetDevAttr(ViDev[i], &default_attr[i].stDevAttr);
				AR_MPI_VI_EnableDev(ViDev[i]);
				VI_DEV_BIND_PIPE_S stDevBindPipe;
				stDevBindPipe.u32Num = 1;
				stDevBindPipe.PipeId[0] = ViPipe[i];
				AR_MPI_VI_SetDevBindPipe(ViDev[i], &stDevBindPipe);
				AR_MPI_VI_CreatePipe(ViPipe[i], &default_attr[i].stPipeAttr);
				AR_MPI_VI_StartPipe(ViPipe[i]);

				AR_MPI_VI_SetChnAttr(ViPipe[i], ViChn[i], &default_attr[i].stChnAttr);
				AR_MPI_VI_EnableChn(ViPipe[i], ViChn[i]);

				ALG_LIB_S stAeLib;
				ALG_LIB_S stAwbLib;
				stAeLib.s32Id = ViPipe[i];
				stAwbLib.s32Id = ViPipe[i];
				strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
				strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));

				if (p_obj[i]->pfnRegisterCallback != AR_NULL)
				{
					s32Ret = p_obj[i]->pfnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);

					if (s32Ret != AR_SUCCESS)
					{
						ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
						goto EXIT;
					}
				}
				else
				{
					ar_err("sensor_register_callback failed with AR_NULL!\n");
					goto EXIT;
				}

				if (AR_NULL != p_obj[i]->pfnSetBusInfo)
				{
					ISP_SNS_COMMBUS_U uSnsBusInfo;
					uSnsBusInfo.s8I2cDev = s8I2cDev[i];
					s32Ret = p_obj[i]->pfnSetBusInfo(ViPipe[i], uSnsBusInfo);

					if (s32Ret != AR_SUCCESS)
					{
						ar_err("set sensor bus info failed with %#x!\n", s32Ret);
						goto EXIT;
					}
				}
				else
				{
					ar_err("not support set sensor bus info!\n");
					goto EXIT;
				}
				AR_MPI_ISP_MemInit(ViPipe[i]);

				AR_MPI_ISP_SetPubAttr(ViPipe[i], &default_attr[i].stPubAttr);
				VI_PIPE_EXT_ATTR_S stPipeAttr = {0};
				AR_MPI_VI_GetPipeExtAttr(ViPipe[i], &stPipeAttr);
				stPipeAttr.bFoucs = 0;
				AR_MPI_VI_SetPipeExtAttr(ViPipe[i], &stPipeAttr);

				AR_MPI_ISP_Init(ViPipe[i]);
				AR_MPI_ISP_Run(ViPipe[i]);
			}
		}
#endif
		close_display_surface(&g_vo_obj, 0);
		close_display_surface(&g_vo_obj, 1);
		close_display(&g_vo_obj);

		for (int i = 0; i < 2; i++)
		{
			AR_MPI_ISP_Exit(ViPipe[i]);
			if (p_obj[i]->pfnUnRegisterCallback != AR_NULL)
			{
				ALG_LIB_S stAeLib;
				ALG_LIB_S stAwbLib;
				stAeLib.s32Id = ViPipe[i];
				stAwbLib.s32Id = ViPipe[i];
				strncpy(stAeLib.acLibName, "artosyn", sizeof("artosyn"));
				strncpy(stAwbLib.acLibName, "artosyn", sizeof("artosyn"));
				s32Ret = p_obj[i]->pfnUnRegisterCallback(ViPipe[i], &stAeLib, &stAwbLib);
				if (s32Ret != AR_SUCCESS)
				{
					ar_err("sensor_register_callback failed with %#x!\n", s32Ret);
				}
			}
			AR_MPI_VI_DisableChn(ViPipe[i], ViChn[i]);
			AR_MPI_VI_StopPipe(ViPipe[i]);
			AR_MPI_VI_DestroyPipe(ViPipe[i]);
			AR_MPI_VI_DisableDev(ViDev[i]);
		}
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
	static volatile int g_hdmi_frame_run = 1;
	static void *hdmi_frame_run(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = run_inf->nframes;
		AR_U64 last_pts = 0;
		int fd = -1;
		char name[256];

		while (g_hdmi_frame_run)
		{
			// SAMPLE_PRT("get frame = %d : pipe_id = %d, ch_id = %d, vo_ch_id = %d\n", ref, run_inf->pipe_id, run_inf->ch_id, run_inf->vo_ch_id);
			int status = AR_MPI_VI_GetChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo, 50);

			ISP_FRAME_INFO_S *info = (ISP_FRAME_INFO_S *)FrameInfo.stVFrame.stSupplement.pIspInfoVirAddr;

			// ar_always("enField %d \n",FrameInfo.stVFrame.enField);

			if (status)
			{

				if (g_exit)
				{
					break;
				}
				continue;
			}
			if (ref > 1000)
			{
				// SAMPLE_PRT("get frame %lx \n",FrameInfo.stVFrame.u64PhyAddr[0]);
				// ref=0;
			}
			ref++;

			// SAMPLE_PRT("pipe_id=%d %ld deta=%ld\n",run_inf->pipe_id,FrameInfo.stVFrame.u64PTS,FrameInfo.stVFrame.u64PTS-last_pts);
			last_pts = FrameInfo.stVFrame.u64PTS;

			// push to display
			send_to_fw_display(&g_vo_obj, run_inf->vo_ch_id, &FrameInfo);

			AR_MPI_VI_ReleaseChnFrame(run_inf->pipe_id, run_inf->ch_id, &FrameInfo);
			if (s32nframes >= 0)
			{
				s32cnt = run_inf->nframes;
				s32nframes = -1;
			}

			if (s32cnt > 0)
			{
				if (--s32cnt == 0)
				{
					break;
				}
			}

			if (g_exit)
			{
				break;
			}
		}
		ar_printf("frame run pipe_id=%d run_inf=%d exit", run_inf->pipe_id, run_inf->ch_id);
		return NULL;
	}
	extern ISP_SNS_OBJ_S stSnsITE66021Obj;
	static void *plug_in(void *param)
	{
		AR_S32 ref = 0, ret = 0;
		VIDEO_FRAME_INFO_S FrameInfo;
		SAMPLE_VIO_PRA *ppra = (SAMPLE_VIO_PRA *)param;
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;
		int fd = -1;
		char name[256];

		AR_S32 s32Ret = 0;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;
		STRU_AR_HAL_VO_RECT vo_rect;
		VB_BLK vbBlk = VB_INVALID_HANDLE;
		VB_POOL vbPool = VB_INVALID_POOLID;
		AR_U64 u64BlkPhy = 0;
		AR_VOID *pBlkVirt = NULL;
		VIDEO_FRAME_INFO_S stFrmInfo;
		SRTU_SENSOR_DEFAULT_ATTR_T stDefaultViAttr;
		VI_CHN_EXT_ATTR_S stChnExtAttr = {0};
		VI_DEV_BIND_PIPE_S stDevBindPipe = {0};
		ALG_LIB_S stAeLib = {0};
		ALG_LIB_S stAwbLib = {0};
		ISP_SNS_COMMBUS_U uSnsBusInfo = {0};
		int i = 0;
		int j = 0;
		const ISP_SNS_OBJ_S *pstSnsObj = &stSnsITE66021Obj;
		const VI_DEV ViDev = 0;
		const VI_PIPE ViPipe = 0;
		const VI_CHN ViChn = 2;
		const int MipiDev = 0;
		AR_S8 s8I2cDev = 1;
		SIZE_S stOutSize = {0};
		int use_lowdelay = 0;
		int isp_run = 0;
		pthread_t ch_thread = {0};

		if (pstSnsObj->pfnGetDefaultAttr)
		{
			s32Ret = pstSnsObj->pfnGetDefaultAttr(0, &stDefaultViAttr);
		}

		if (ppra->settle > 0)
		{
			stDefaultViAttr.stComboAttr.mipi_attr.settle_count = ppra->settle;
		}

		if (ppra->mipi_fre > 0)
		{
			stDefaultViAttr.stComboAttr.mipi_attr.mipi_pix_clk = ppra->mipi_fre / 1000 / 1000;
		}
		if (ppra->dpcm == 1)
		{
			stDefaultViAttr.stPipeAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
			stDefaultViAttr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_DPCM_6BITS;
		}
		else if (ppra->dpcm == 2)
		{
			stDefaultViAttr.stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
			stDefaultViAttr.stPipeAttr.stNrAttr.enCompressMode = COMPRESS_MODE_NONE;
		}

		if (stDefaultViAttr.stPipeAttr.enCompressMode == COMPRESS_MODE_NONE)
		{
			stDefaultViAttr.stPipeAttr.enEnableNoCmp = VIN_PIPE_NOCPM_ENABLE;
		}

		if (ppra->feature_mask)
		{
			stDefaultViAttr.stPipeAttr.u32FeatureMask = ppra->feature_mask;
		}

		int s32pra_index = sample_find_pra_by_name("-lowdealy", ppra->argc, ppra->argv);
		if (s32pra_index > 0)
		{
			use_lowdelay = atoi(ppra->argv[s32pra_index + 1]);
		}

		u32BlkSize = COMMON_GetPicBufferSize(stDefaultViAttr.stPipeAttr.u32MaxW, stDefaultViAttr.stPipeAttr.u32MaxH, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);

		if (use_lowdelay)
		{
			// lowdeay 只用一个buffer，手动的申请这个buffer，并配置到每个pannle 中
			vbBlk = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, u32BlkSize, NULL);
			if (vbBlk == VB_INVALID_HANDLE)
			{
				ar_err("AR_MPI_VB_GetBlock err!");
			}

			vbPool = AR_MPI_VB_Handle2PoolId(vbBlk);
			if (vbPool == VB_INVALID_POOLID)
			{
				ar_err("AR_MPI_VB_Handle2PoolId err!");
			}

			u64BlkPhy = AR_MPI_VB_Handle2PhysAddr(vbBlk);
			if (u64BlkPhy == 0)
			{
				ar_err("AR_MPI_VB_Handle2PhysAddr err!");
			}

			pBlkVirt = AR_MPI_SYS_Mmap(u64BlkPhy, u32BlkSize);
			if (pBlkVirt == NULL)
			{
				ar_err("AR_MPI_SYS_Mmap err!");
			}

			ar_always("frame buffer: pool[%d] blk[%d] phy[0x%x] virt[%p]",
					  vbPool, vbBlk, u64BlkPhy, pBlkVirt);
		}
		ISP_SNS_DETECT_INFO_S stSnsDetectInfo = {0};

		while (1)
		{

			int status = AR_MPI_ISP_GetSnsDetectInfo(ViPipe, &stSnsDetectInfo);

			if (status)
			{
				break;
			}

			ar_printf("get sensordetected info, bIsConnected %d bIsChanged %d \n", stSnsDetectInfo.bIsConnected, stSnsDetectInfo.bIsChanged);
			if (stSnsDetectInfo.bIsConnected)
			{

				if (isp_run && !stSnsDetectInfo.bIsChanged)
				{
					continue;
				}

				if (isp_run && stSnsDetectInfo.bIsChanged)
				{
					g_hdmi_frame_run = 0;
					pthread_join(ch_thread, NULL);
					AR_MPI_ISP_Exit(ViPipe);
					AR_MPI_VI_DisableChn(ViPipe, ViChn);
					AR_MPI_VI_StopPipe(ViPipe);
					AR_MPI_VI_DestroyPipe(ViPipe);
					AR_MPI_VI_DisableDev(ViDev);
					isp_run = 0;
				}

				stDefaultViAttr.stPubAttr.stSnsSize.u32Width = stSnsDetectInfo.u16Width;
				stDefaultViAttr.stPubAttr.stSnsSize.u32Height = stSnsDetectInfo.u16Height;
				stDefaultViAttr.stPubAttr.f32FrameRate = stSnsDetectInfo.f32Fps;
				stDefaultViAttr.stPubAttr.stWndRect.s32X = 0;
				stDefaultViAttr.stPubAttr.stWndRect.s32Y = 0;
				stDefaultViAttr.stPubAttr.stWndRect.u32Width = stSnsDetectInfo.u16Width;
				stDefaultViAttr.stPubAttr.stWndRect.u32Height = stSnsDetectInfo.u16Height;
				if (stSnsDetectInfo.bIsInterlace)
				{
					stDefaultViAttr.stDevAttr.enScanMode = VI_SCAN_INTERLACED;
				}
				else
				{
					stDefaultViAttr.stDevAttr.enScanMode = VI_SCAN_PROGRESSIVE;
				}
				// stDefaultViAttr.stDevAttr.enDataSeq=VI_DATA_SEQ_YUYV;

				stDefaultViAttr.stChnAttr.stSize.u32Width = stSnsDetectInfo.u16Width;
				stDefaultViAttr.stChnAttr.stSize.u32Height = stSnsDetectInfo.u16Height;
				stDefaultViAttr.stChnAttr.s32Timeout = (1000 / stDefaultViAttr.stPubAttr.f32FrameRate) / 2;
				stDefaultViAttr.stChnAttr.u32BufCount = 8;

				stOutSize.u32Width = stDefaultViAttr.stChnAttr.stSize.u32Width;
				stOutSize.u32Height = stDefaultViAttr.stChnAttr.stSize.u32Height;

				if (use_lowdelay)
				{
					memset(&stFrmInfo, 0, sizeof(stFrmInfo));
					stFrmInfo.stVFrame.u32Stride[0] = CAM_ALIGNE_TO(stOutSize.u32Width, 512);
					stFrmInfo.stVFrame.u32Stride[1] = CAM_ALIGNE_TO(stOutSize.u32Width / 2, 256);
					stFrmInfo.stVFrame.u32Stride[2] = CAM_ALIGNE_TO(stOutSize.u32Width / 2, 256);
					stFrmInfo.stVFrame.u64PhyAddr[0] = u64BlkPhy;
					stFrmInfo.stVFrame.u64PhyAddr[1] = stFrmInfo.stVFrame.u64PhyAddr[0] + stFrmInfo.stVFrame.u32Stride[0] * stOutSize.u32Height + 8192;
					stFrmInfo.stVFrame.u64PhyAddr[2] = stFrmInfo.stVFrame.u64PhyAddr[1] + stFrmInfo.stVFrame.u32Stride[1] * stOutSize.u32Height / 2 + 8192;
					stFrmInfo.stVFrame.u64VirAddr[0] = (AR_U64)pBlkVirt;
					stFrmInfo.stVFrame.u64VirAddr[1] = stFrmInfo.stVFrame.u64VirAddr[0] + stFrmInfo.stVFrame.u32Stride[0] * stOutSize.u32Height + 8192;
					stFrmInfo.stVFrame.u64VirAddr[2] = stFrmInfo.stVFrame.u64VirAddr[1] + stFrmInfo.stVFrame.u32Stride[1] * stOutSize.u32Height / 2 + 8192;
					stFrmInfo.stVFrame.u32Len[0] = stFrmInfo.stVFrame.u32Stride[0] * stOutSize.u32Height;
					stFrmInfo.stVFrame.u32Len[1] = stFrmInfo.stVFrame.u32Stride[1] * stOutSize.u32Height / 2;
					stFrmInfo.stVFrame.u32Len[2] = stFrmInfo.stVFrame.u32Stride[2] * stOutSize.u32Height / 2;
					stFrmInfo.stVFrame.u32Width = stOutSize.u32Width;
					stFrmInfo.stVFrame.u32Height = stOutSize.u32Height;
					stFrmInfo.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
					stFrmInfo.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
					stFrmInfo.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
					stFrmInfo.stVFrame.enField = VIDEO_FIELD_FRAME;
					stFrmInfo.u32PoolId = vbPool;

					// send_to_fw_display(&g_vo_obj, 0, &stFrmInfo);
					stChnExtAttr.enLowDelayMode = VI_CH_LOW_DELAY_DDR;
					stChnExtAttr.enLowDelaySink = LOW_DELAY_SINK_HEVC_H265;
				}
				else
				{
					stChnExtAttr.enLowDelayMode = VI_CH_LOW_DELAY_OFF;
					stChnExtAttr.enLowDelaySink = LOW_DELAY_SINK_HEVC_H265;
				}

				s32Ret = AR_MPI_VI_SetMipiBindDev(ViDev, MipiDev);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_SetMipiBindDev failed");
				}

				s32Ret = AR_MPI_VI_SetComboDevAttr(&stDefaultViAttr.stComboAttr);
				if (s32Ret)
				{
					ar_err("MIPI_SET_DEV_ATTR failed");
				}

				s32Ret = AR_MPI_VI_SetDevAttr(ViDev, &stDefaultViAttr.stDevAttr);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_SetDevAttr failed");
				}

				s32Ret = AR_MPI_VI_EnableDev(ViDev);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_EnableDev failed");
				}

				stDevBindPipe.u32Num = 1;
				stDevBindPipe.PipeId[0] = ViPipe;
				s32Ret = AR_MPI_VI_SetDevBindPipe(ViDev, &stDevBindPipe);
				if (s32Ret)
				{
					SAMPLE_PRT("AR_MPI_VI_SetDevBindPipe");
				}

				s32Ret = AR_MPI_VI_CreatePipe(ViPipe, &stDefaultViAttr.stPipeAttr);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_CreatePipe");
				}

				s32Ret = AR_MPI_VI_StartPipe(ViPipe);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_StartPipe");
				}

				s32Ret = AR_MPI_VI_SetChnAttr(ViPipe, ViChn, &stDefaultViAttr.stChnAttr);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_SetChnAttr failed");
				}

				s32Ret = AR_MPI_VI_SetChnExtAttr(ViPipe, ViChn, &stChnExtAttr);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_SetChnExtAttr failed");
				}

				if (use_lowdelay)
				{
					for (i = 0; i < 5; i++)
					{
						for (j = 0; j < 3; j++)
						{
							s32Ret = AR_MPI_VI_SetChnPannelAddr(ViPipe, ViChn, i, j,
																stFrmInfo.stVFrame.u64PhyAddr[j], (AR_VOID *)stFrmInfo.stVFrame.u64VirAddr[j]);
							if (s32Ret)
							{
								ar_err("AR_MPI_VI_SetChnPannelAddr failed");
							}
						}
					}
				}

				s32Ret = AR_MPI_VI_EnableChn(ViPipe, ViChn);
				if (s32Ret)
				{
					ar_err("AR_MPI_VI_EnableChn failed");
				}

				if (s8I2cDev >= 0)
				{
					uSnsBusInfo.s8I2cDev = s8I2cDev;
					if (pstSnsObj->pfnSetBusInfo)
					{
						s32Ret = pstSnsObj->pfnSetBusInfo(ViPipe, uSnsBusInfo);
					}
					if (s32Ret || pstSnsObj->pfnSetBusInfo == NULL)
					{
						ar_err("set sensor bus info failed");
					}
				}

				s32Ret = AR_MPI_ISP_SetPubAttr(ViPipe, &stDefaultViAttr.stPubAttr);
				if (s32Ret)
				{
					ar_err("AR_MPI_ISP_SetPubAttr failed");
				}

				s32Ret = AR_MPI_ISP_Init(ViPipe);
				if (s32Ret)
				{
					ar_err("AR_MPI_ISP_Init failed");
				}

				s32Ret = AR_MPI_ISP_Run(ViPipe);
				if (s32Ret)
				{
					ar_err("AR_MPI_ISP_Run failed");
				}
				isp_run = 1;
				// start a thread to display
				STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch1;
				pipe0_ch1.pipe_id = 0;
				pipe0_ch1.ch_id = 2;
				pipe0_ch1.vo_ch_id = 0;
				pipe0_ch1.nframes = ppra->s32nframes;
				g_hdmi_frame_run = 1;
				pthread_create(&ch_thread, NULL, hdmi_frame_run, &pipe0_ch1);
			}
			else
			{
				g_hdmi_frame_run = 0;
				pthread_join(ch_thread, NULL);
				AR_MPI_ISP_Exit(ViPipe);
				AR_MPI_VI_DisableChn(ViPipe, ViChn);
				AR_MPI_VI_StopPipe(ViPipe);
				AR_MPI_VI_DestroyPipe(ViPipe);
				AR_MPI_VI_DisableDev(ViDev);
				isp_run = 0;
			}
		}

		if (isp_run)
		{
			g_hdmi_frame_run = 0;
			pthread_join(ch_thread, NULL);
			AR_MPI_ISP_Exit(ViPipe);
			AR_MPI_VI_DisableChn(ViPipe, ViChn);
			AR_MPI_VI_StopPipe(ViPipe);
			AR_MPI_VI_DestroyPipe(ViPipe);
			AR_MPI_VI_DisableDev(ViDev);
			isp_run = 0;
		}

	EXIT3:
		if (use_lowdelay)
		{
			AR_MPI_SYS_Munmap(pBlkVirt, u32BlkSize);
		}
	EXIT2:
		if (use_lowdelay)
		{
			AR_MPI_VB_ReleaseBlock(vbBlk);
		}
		return NULL;
	}

	AR_S32 SAMPLE_VIO_Only_hdmi_raw_LowDelay(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;
		VB_CONFIG_S stVbConf;
		AR_U32 u32BlkSize;
		STRU_AR_HAL_VO_RECT vo_rect;
		VB_BLK vbBlk = VB_INVALID_HANDLE;
		VB_POOL vbPool = VB_INVALID_POOLID;
		AR_U64 u64BlkPhy = 0;
		AR_VOID *pBlkVirt = NULL;
		VIDEO_FRAME_INFO_S stFrmInfo;

		SRTU_SENSOR_DEFAULT_ATTR_T stDefaultViAttr;
		VI_CHN_EXT_ATTR_S stChnExtAttr = {0};
		VI_DEV_BIND_PIPE_S stDevBindPipe = {0};
		ALG_LIB_S stAeLib = {0};
		ALG_LIB_S stAwbLib = {0};
		ISP_SNS_COMMBUS_U uSnsBusInfo = {0};
		const VI_PIPE ViPipe = 0;

		const ISP_SNS_OBJ_S *pstSnsObj = &stSnsITE66021Obj;

		if (pstSnsObj->pfnGetDefaultAttr)
		{
			s32Ret = pstSnsObj->pfnGetDefaultAttr(0, &stDefaultViAttr);
		}

		// config vb
		SIZE_S stSize = stDefaultViAttr.stPubAttr.stSnsSize;
		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 0;

		u32BlkSize = COMMON_GetPicBufferSize(stDefaultViAttr.stPipeAttr.u32MaxW, stDefaultViAttr.stPipeAttr.u32MaxH, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 10;
		stVbConf.u32MaxPoolCnt++;

		u32BlkSize = VI_GetRawBufferSize(stDefaultViAttr.stPipeAttr.u32MaxW, stDefaultViAttr.stPipeAttr.u32MaxH, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u64BlkSize = u32BlkSize + 8192;
		stVbConf.astCommPool[stVbConf.u32MaxPoolCnt].u32BlkCnt = 12;

		stVbConf.u32MaxPoolCnt++;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			ar_err("system init failed with %d!", s32Ret);
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 300000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 300000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 100000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		// four layout
		AR_S32 mesh_w = 1920;
		AR_S32 mesh_h = 1080;

		// init display
		if (ppra->u32votype)
		{
			ar_vo_dev_init_for_mipi(&g_vo_obj, 1920, 1080, -1, -1, 25, ppra->u32votype, 0);
		}
		else
		{
			ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);
		}

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);

		// 为了使用热插拔检测，打开vin 设备后，首先关联pipe 和 sensor
		stAeLib.s32Id = ViPipe;
		strncpy(stAeLib.acLibName, AR_AE_LIB_NAME, sizeof(AR_AE_LIB_NAME));
		stAwbLib.s32Id = ViPipe;
		strncpy(stAwbLib.acLibName, AR_AWB_LIB_NAME, sizeof(AR_AWB_LIB_NAME));

		if (pstSnsObj->pfnRegisterCallback)
		{
			s32Ret = pstSnsObj->pfnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
		}

		// 启动自动探测，本api 会把hdmi 驱动启动起来，后续就可以通过api 获取热插拔消息
		AR_MPI_ISP_StartSnsDetect(ViPipe);

		// 启动一个线程，等待热插拔消息,并处理热插拔
		pthread_t ch_thread = {0};

		pthread_attr_t *pstAttr = NULL;
		pthread_create(&ch_thread, pstAttr, plug_in, ppra);

		while (1)
		{
			if (g_exit)
			{
				break;
			}
			ar_delay(30);
		}
		// 停止探测会导致plug_in 线程退出
		AR_MPI_ISP_StopSnsDetect(ViPipe);

		pthread_join(ch_thread, NULL);

	EXIT8:
		if (pstSnsObj->pfnUnRegisterCallback)
		{
			pstSnsObj->pfnUnRegisterCallback(ViPipe, &stAeLib, &stAwbLib);
		}
	EXIT1:
		close_display_surface(&g_vo_obj, 0);
		close_display(&g_vo_obj);
		SAMPLE_COMM_SYS_Exit();
	EXIT0:
		return s32Ret;
	}

	static int random_scan_deskew(STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf)
	{
		int check_flg = 0, ref_ok = 0;
		int lane0_deskew, lane1_deskew, lane2_deskew, lane3_deskew;
		int index = 0;
		int region0[2][2] = {{6, 21}, {6, 21}};
		srand(time(NULL));
		int ViPipe = run_inf->pipe_id;
		int ViChn = run_inf->ch_id;
		VIDEO_FRAME_INFO_S FrameInfo;
#define TIME_OUT (500)
#define REF_FRAME_OK (50000)
		int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, TIME_OUT);

		if (status < 0)
		{
			index = rand() % 2;
			for (lane0_deskew = region0[index][0]; lane0_deskew <= region0[index][1]; lane0_deskew++)
			{
				status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, TIME_OUT);
				if (status < 0)
				{
					index = rand() % 2;
					for (lane1_deskew = region0[index][0]; lane1_deskew <= region0[index][1]; lane1_deskew++)
					{
						status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, TIME_OUT);
						if (status < 0)
						{
							index = rand() % 2;
							for (lane2_deskew = region0[index][0]; lane2_deskew <= region0[index][1]; lane2_deskew++)
							{
								status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, TIME_OUT);
								if (status < 0)
								{
									index = rand() % 2;
									for (lane3_deskew = region0[index][0]; lane3_deskew <= region0[index][1]; lane3_deskew++)
									{
										printf("check: lane0_deskew = %d, lane1_deskew = %d, lane2_deskew = %d, lane3_deskew = %d\n",
											   lane0_deskew, lane1_deskew, lane2_deskew, lane3_deskew);
										AR_MPI_VI_PauseChn(ViPipe, ViChn, ISP_SUS_SHALLOW_MODE);

										// set deskew new value
										AR_MPI_VI_Set_MIPI_Deskew(ViPipe, 0, lane0_deskew);
										AR_MPI_VI_Set_MIPI_Deskew(ViPipe, 1, lane1_deskew);
										AR_MPI_VI_Set_MIPI_Deskew(ViPipe, 2, lane2_deskew);
										AR_MPI_VI_Set_MIPI_Deskew(ViPipe, 3, lane3_deskew);
										// resume the pipe

										AR_MPI_VI_RecoverChn(ViPipe, ViChn, ISP_SUS_SHALLOW_MODE);
										// usleep(16000);
										status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, TIME_OUT);
										if (status < 0)
										{
											continue;
										}
										else if (status == 0)
										{
											printf("get chn frame OK !, lane0_deskew = %d, lane1_deskew = %d, lane2_deskew = %d, lane3_deskew = %d\n",
												   lane0_deskew, lane1_deskew, lane2_deskew, lane3_deskew);
											AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
											// check frame is really ok
											int ref_ok = 0;
											int flg = 0;
											do
											{
												status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, TIME_OUT);
												if (status == 0)
												{
													AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
													ref_ok++;
													printf("get chn ok frame id = %d\n", ref_ok + 1);
												}
												else
												{
													break;
												}
												if (ref_ok >= REF_FRAME_OK)
												{
													flg = 1;
													break;
												}
											} while (1);
											if (flg)
											{
												break;
											}
										}
										if (g_exit)
										{
											break;
										}
									}
								}
								else
								{
									AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
								}
								if (g_exit)
								{
									break;
								}
							}
						}
						else
						{
							AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
						}
						if (g_exit)
						{
							break;
						}
					}
				}
				else
				{
					AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
				}
				if (g_exit)
				{
					break;
				}
			}
		}
		else
		{
			// ref_ok++;
			// send_to_fw_display(&g_vo_obj,ViChn,&FrameInfo);
			printf("pipe_id=%d vichn = %d pts=%ld\n", ViPipe, ViChn, FrameInfo.stVFrame.u64PTS);
			AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);
		}
		return 0;
	}

	static void *loop_set_vif_register_thread(void *param)
	{
		STRU_VIO_SAMPLE_FRAME_RUN_T *run_inf = (STRU_VIO_SAMPLE_FRAME_RUN_T *)param;
		int ViPipe = run_inf->pipe_id;
		int value = 0, addr_y, addr_u, addr_v;
		while (1)
		{
			AR_MPI_ISP_GetRegister(ViPipe, 0x01100020, &addr_y);
			// printf("get view0 addr y: 0x%x\n", addr_y);
			AR_MPI_ISP_SetRegister(ViPipe, 0x01100020, addr_y);
			// AR_MPI_ISP_SetRegister(ViPipe, 0x01100040, 0X0C010000);
			AR_MPI_ISP_GetRegister(ViPipe, 0x01100040, &addr_u);
			// printf("get view0 addr u: 0x%x\n", addr_u);
			AR_MPI_ISP_SetRegister(ViPipe, 0x01100040, addr_u);
			AR_MPI_ISP_GetRegister(ViPipe, 0x01100060, &addr_v);
			// printf("get view0 addr v: 0x%x\n", addr_v);
			AR_MPI_ISP_SetRegister(ViPipe, 0x01100060, addr_v);
			AR_MPI_ISP_GetRegister(ViPipe, 0x0110017c, &value);
			// printf("get vif view int reg: 0x%x\n", value);
			value = value | 0xff;
			AR_MPI_ISP_SetRegister(ViPipe, 0x0110017c, value);
			sleep(3);
		}

		return NULL;
	}

	AR_S32 SAMPLE_VI_Only_gn2(SAMPLE_VIO_PRA *ppra)
	{
		AR_S32 s32Ret = 0;

		AR_S32 s32ViCnt = 1;
		VI_DEV ViDev = 0;
		VI_PIPE ViPipe = 0;
		VI_CHN ViChn = 0;
		AR_S32 s32WorkSnsId = 0;
		SAMPLE_VI_CONFIG_S stViConfig;
		SAMPLE_SNS_TYPE_E enSnsType;
		if (ppra->u32votype == 1)
			enSnsType = SAMSUNG_S5KGN2SP_MIPI_4K_30FPS_10BIT; // SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT;
		else
			enSnsType = SAMSUNG_S5KGN2SP_MIPI_4K_60FPS_10BIT;
		SRTU_SENSOR_DEFAULT_ATTR_T default_attr;
		SIZE_S stSize;
		VB_CONFIG_S stVbConf;
		PIC_SIZE_E enPicSize = PIC_1080P;
		AR_U32 u32BlkSize;

		VO_CHN VoChn = 0;
		// SAMPLE_VO_CONFIG_S stVoConfig;

		WDR_MODE_E enWDRMode = WDR_MODE_NONE;
		DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
		PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
		VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
		COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;
		/*
		if(p_obj->pfnGetDefaultAttr)
		{
		   p_obj->pfnGetDefaultAttr(0,&default_attr);
		}else
		{
		  ar_err("pfnGetDefaultAttr is null, exit the test");
		  return -1;
		}
	*/
		// sleep(20);
		//*(int*)(NULL) = 0;
		/*config vi*/
		int scan_skew = 0;
		int lane_id = 0;
		int s32praidx = sample_find_pra_by_name("-scan_skew", ppra->argc, ppra->argv);
		if (s32praidx >= 0)
		{
			scan_skew = atoi(ppra->argv[s32praidx + 1]);
		}
		s32praidx = sample_find_pra_by_name("-lane", ppra->argc, ppra->argv);
		if (s32praidx >= 0)
		{
			lane_id = atoi(ppra->argv[s32praidx + 1]);
		}
		int random_deskew = 0;
		s32praidx = sample_find_pra_by_name("-random_deskew", ppra->argc, ppra->argv);
		if (s32praidx >= 0)
		{
			random_deskew = atoi(ppra->argv[s32praidx + 1]);
		}

		SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

		stViConfig.s32WorkingViNum = s32ViCnt;
		stViConfig.as32WorkingViId[0] = 0;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = enSnsType;
		g_enSnsType[stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId] = stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId = 1; // 2; //for evb 1
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev = ViDev;
		stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = enWDRMode;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0] = ViPipe;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3] = -1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn = ViChn;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = enCompressMode;

		/*get picture size*/
		s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get1 picture size by sensor failed!\n");
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("get picture size failed!\n");
			return s32Ret;
		}

		// stSize.u32Width = 4096;
		// stSize.u32Height = 3120;

		/*config vb*/
		ar_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
		stVbConf.u32MaxPoolCnt = 2;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[0].u64BlkSize = u32BlkSize + 1024 * 1024 + 8 * 1024;
		stVbConf.astCommPool[0].u32BlkCnt = 5;

		/*u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
		stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
		stVbConf.astCommPool[1].u32BlkCnt   = 6;*/

		u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 256);
		stVbConf.astCommPool[2].u64BlkSize = u32BlkSize + 8192 * 3;
		stVbConf.astCommPool[2].u32BlkCnt = 0;

		s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("system init failed wit1h %d!\n", s32Ret);
			return s32Ret;
		}

		if (ppra->isp_fre == 0)
		{
			ppra->isp_fre = 400000000;
		}

		if (ppra->vif_fre == 0)
		{
			ppra->vif_fre = 400000000;
		}

		if (ppra->pcs_fre == 0)
		{
			ppra->pcs_fre = 333000000;
		}

		if (ppra->cam_mode > 2)
		{
			SAMPLE_AR_MPI_VIN_OpenDev(1, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}
		else
		{
			SAMPLE_AR_MPI_VIN_OpenDev(ppra->cam_mode, ppra->isp_fre, ppra->vif_fre, ppra->isp_fre, ppra->pcs_fre);
		}

		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.settle = ppra->settle;
		stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.mipi_ipi_fre = ppra->mipi_fre;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.dpcm = ppra->dpcm;
		stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.feature_mask = ppra->feature_mask;

		/*start vi*/
		s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
		if (AR_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start 1vi failed.s32Ret:0x%x !\n", s32Ret);
			goto EXIT;
		}

		// four layout
		AR_S32 mesh_w = 1920;
		AR_S32 mesh_h = 1080;
		// init display
#if 1
		ar_vo_dev_init(&g_vo_obj, 1920, 1080, -1, -1);

		STRU_AR_HAL_VO_RECT chan_pos = {0};
		chan_pos.x = 0;
		chan_pos.y = 0;
		chan_pos.w = mesh_w;
		chan_pos.h = mesh_h;
		ar_vo_surface_init(&g_vo_obj, 0, &chan_pos);
#endif
		// get frame buffer
		// usleep(5000000);
		VIDEO_FRAME_INFO_S FrameInfo;
		AR_S32 ref = 0;
		AR_S32 gamma = 0;
		ISP_GAMMA_ATTR_S *pstGammaAttr = malloc(sizeof(ISP_GAMMA_ATTR_S));
		ISP_MIRROR_STATE_TIDY_ATTR_S *pstMirrorState = malloc(sizeof(ISP_MIRROR_STATE_TIDY_ATTR_S));

		AR_MPI_ISP_GetGammaAttr(ViPipe, pstGammaAttr);
		AR_S32 s32cnt = 0;
		AR_S32 s32nframes = ppra->s32nframes;
		AR_U64 last_pts = 0;

		ISP_DE3D_STRENGTH_TIDY_ATTR_S stDe3dStrengthTidyAttr;
		stDe3dStrengthTidyAttr.u32De3dStrength = 100;
		pthread_t ch_thread = {0};
		pthread_attr_t *pstAttr = NULL;
		STRU_VIO_SAMPLE_FRAME_RUN_T pipe0_ch0;

		pthread_t ch_thread1 = {0};
		pipe0_ch0.pipe_id = ViPipe;
		pipe0_ch0.ch_id = ViChn;
		pipe0_ch0.vo_ch_id = VoChn;
		pipe0_ch0.nframes = ppra->s32nframes;
		pipe0_ch0.lane_id = lane_id;
		if (scan_skew)
		{
			STRU_VIN_DRIVER_START_OPT_T p_opt;
			AR_MPI_VIN_get_driver_opt(&p_opt);
			p_opt.use_mipi_only_skew_int = 1;
			AR_MPI_VIN_set_driver_opt(&p_opt);
			sleep(5);
			pthread_create(&ch_thread1, pstAttr, calibration_thread, &pipe0_ch0);
			pthread_join(ch_thread1, NULL);
		}
		STRU_VIO_SAMPLE_FRAME_RUN_T run_inf;
		run_inf.ch_id = ViChn;
		run_inf.pipe_id = ViPipe;
		while (1)
		{
			if (random_deskew)
			{
				pthread_create(&ch_thread, pstAttr, loop_set_vif_register_thread, &pipe0_ch0);
				random_scan_deskew(&run_inf);

				if (g_exit)
				{
					break;
				}
			}
			else
			{
				int status = AR_MPI_VI_GetChnFrame(ViPipe, ViChn, &FrameInfo, 1000);
				if (ref % 2000 == 0)
					printf("pipe_id=%d %ld deta=%ld\n", ViPipe, FrameInfo.stVFrame.u64PTS, FrameInfo.stVFrame.u64PTS - last_pts);
				last_pts = FrameInfo.stVFrame.u64PTS;
				// push to display
				send_to_fw_display(&g_vo_obj, ViChn, &FrameInfo);
				AR_MPI_VI_ReleaseChnFrame(ViPipe, ViChn, &FrameInfo);

				if (g_exit)
				{
					break;
				}
				ref++;
			}
		}
	EXIT2:
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	EXIT:
		SAMPLE_COMM_SYS_Exit();
		return s32Ret;
	}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
