#ifdef AR_FEAT_EIS
#include "ar_comm_video.h"
#include "hal_type.h"
#include "hal_errno.h"
#include "hal_eis_algo.h"
#include "hal_sys.h"
#include "mpi_dbglog.h"
#include "inv_mpu_ioctl.h"
#include "sample_eis.h"
#include "sample_eis_mpu.h"
#include "mpi_gdc_api.h"
#include "ar_math.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#define SAMPLE_EIS_MPU_LOG_TAG		MPP_TAG_ID(AR_ID_VPSS)

#define SAMPLE_EIS_ALGO_NAME			"artosyn"

#define SAMPLE_EIS_DEV_NAME			"/dev/"INV_MPU_DEV_NAME
#define SAMPLE_EIS_DEV_FLAG			O_RDWR

static STRU_SAMPLE_EIS_INFO_S g_sample_eis;

#ifdef __SAMPLE_EIS_SIMU__
AR_S32 sample_eis_read_data(AR_S32 fd, STRU_EIS_IMMU_DATA_T *data, AR_U32 len)
{
	(AR_VOID)fd;
	(AR_VOID)len;

	data->x = 0;
	data->y = 0;
	data->z = 0;
	data->time_stamps = ar_get_timestamp();
	ar_delay(10);

	return 0;
}
#else
AR_S32 sample_eis_read_data(AR_S32 fd, STRU_EIS_IMMU_DATA_T *data, AR_U32 len)
{
	AR_S32 ret = AR_OK;
	AR_U8* hwdata = NULL;
	AR_S32 read_len = 0;

	hwdata = ar_malloc(len);
	if(!hwdata)
	{
		ret = AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOMEM);
		return ret;
	}

	read_len = read(fd, hwdata, len);

	g_sample_eis.total_read_len += read_len;
	if(read_len < len)
	{
		AR_LOG_DBG(SAMPLE_EIS_MPU_LOG_TAG, "read %d get %d < %d err %d %s",
					fd, read_len, len, errno, strerror(errno));
		ar_free(hwdata);
		return read_len;
	}

	sample_eis_conv_data(&g_sample_eis, hwdata, data, len);

	ar_free(hwdata);
	return read_len;
}
#endif

#define __SAMPLE_EIS_DEBUG_BDCMD__

#ifdef __SAMPLE_EIS_DEBUG_BDCMD__
static AR_VOID eis_binder_cmd_process_dump(STRU_CMD_ARG_T *args, STRU_EIS_INFO_S *eis_info)
{
	char *file_name = NULL;
	int num_frames = 0;
	char after_name[128] = {0};

	int sub_index = ar_hal_sys_find_pra_index_by_name(args, "-name");
	if(sub_index > 0)
	{
		file_name = args->args[sub_index+1];
	}

	sub_index = ar_hal_sys_find_pra_index_by_name(args, "-num");
	if(sub_index > 0)
	{
		num_frames = atoi(args->args[sub_index+1]);
	}

	sub_index = ar_hal_sys_find_pra_index_by_name(args, "-after");
	if(sub_index > 0)
	{
		if(file_name)
		{
			sprintf(after_name, "%s_after", file_name);
		}
		eis_info->dbg->dump_after->dump_req(eis_info->dbg->dump_after, after_name, num_frames);
	}
	else
	{
		eis_info->dbg->dump->dump_req(eis_info->dbg->dump, file_name, num_frames);

		sub_index = ar_hal_sys_find_pra_index_by_name(args, "-both");
		if(sub_index > 0)
		{
			if(file_name)
			{
				sprintf(after_name, "%s_after", file_name);
			}
			eis_info->dbg->dump_after->dump_req(eis_info->dbg->dump_after, after_name, num_frames);
		}
	}
}

static AR_VOID eis_binder_cmd_process_ldc(STRU_CMD_ARG_T *args, STRU_EIS_INFO_S *eis_info)
{
	STRU_EIS_TUNING_T tuning;
	STRU_EIS_CTL_UPDATE_TUNING_T pra =
	{
		.is_init = 0,
		.p_algo_tuning = &tuning
	};
	hal_eis_get_tuning_pra(eis_info->lib, &pra);

	int sub_index = ar_hal_sys_find_pra_index_by_name(args, "-set");
	if(sub_index > 0)
	{
		AR_LOG_RAW("before: ldc_enable=%d\n", pra.p_algo_tuning->ldc_enable);
		pra.p_algo_tuning->ldc_enable = atoi(args->args[sub_index+1]);
		hal_eis_set_tuning_pra(eis_info->lib, &pra);

		hal_eis_get_tuning_pra(eis_info->lib, &pra);
		AR_LOG_RAW("after: ldc_enable=%d\n", pra.p_algo_tuning->ldc_enable);
	}
	else
	{
		AR_LOG_RAW("ldc_enable=%d\n", pra.p_algo_tuning->ldc_enable);
	}
}

static AR_VOID eis_binder_cmd_process_eis(STRU_CMD_ARG_T *args, STRU_EIS_INFO_S *eis_info)
{
	STRU_EIS_TUNING_T tuning;
	STRU_EIS_CTL_UPDATE_TUNING_T pra =
	{
		.is_init = 0,
		.p_algo_tuning = &tuning
	};
	hal_eis_get_tuning_pra(eis_info->lib, &pra);

	int sub_index = ar_hal_sys_find_pra_index_by_name(args, "-set");
	if(sub_index > 0)
	{
		AR_LOG_RAW("before: eis_enable=%d\n", pra.p_algo_tuning->eis_enable);
		pra.p_algo_tuning->eis_enable = atoi(args->args[sub_index+1]);
		hal_eis_set_tuning_pra(eis_info->lib, &pra);

		hal_eis_get_tuning_pra(eis_info->lib, &pra);
		AR_LOG_RAW("after: eis_enable=%d\n", pra.p_algo_tuning->eis_enable);
	}
	else
	{
		AR_LOG_RAW("eis_enable=%d\n", pra.p_algo_tuning->eis_enable);
	}
}

static AR_S32 eis_binder_cmd_process(AR_CHAR *cmd_str, AR_S32 len, void *data)
{
	AR_LOG_RAW("%s: cmd_str:%s len:%d\n", __func__, cmd_str, len);

	STRU_CMD_ARG_T *args = ar_hal_sys_get_args(cmd_str);
	STRU_EIS_INFO_S *eis_info = (STRU_EIS_INFO_S*)data;
	(void)len;

	int index = ar_hal_sys_find_pra_index_by_name(args, "--help");
	if(index > 0)
	{
		AR_LOG_RAW("--help: show this info\n"
					"--dump -name [name] -num [no] -after -both\n"
					"--eis -set [1/0]\n"
					"--ldc -set [0/1/2/3]\n"
					"--stats\n"
					"--gain [float gain]\n"
					"--offset_ms [val, val]\n");
		

		free(args);
		return 0;
	}

	index = ar_hal_sys_find_pra_index_by_name(args, "--dump");
	if(index > 0)
	{
		eis_binder_cmd_process_dump(args, eis_info);
	}

	index = ar_hal_sys_find_pra_index_by_name(args, "--eis");
	if(index > 0)
	{
		eis_binder_cmd_process_eis(args, eis_info);
	}

	index = ar_hal_sys_find_pra_index_by_name(args, "--ldc");
	if(index > 0)
	{
		eis_binder_cmd_process_ldc(args, eis_info);
	}

	index = ar_hal_sys_find_pra_index_by_name(args, "--stats");
	if(index > 0)
	{
	    AR_LOG_RAW("no_imu_data_match_ref=%d\n", eis_info->no_imu_data_match_ref);
	}
	index = ar_hal_sys_find_pra_index_by_name(args, "--offset_ms");
	if(index > 0)
	{
	    int valule=atoi(args->args[index+1]);
		int valule1=atoi(args->args[index+1]);
	    AR_LOG_RAW("offset_ms=%d %d\n",valule,valule1);

    	STRU_EIS_TUNING_T tuning;
    	STRU_EIS_CTL_UPDATE_TUNING_T pra =
    	{
    		.is_init = 0,
    		.p_algo_tuning = &tuning
    	};
    	hal_eis_get_tuning_pra(eis_info->lib, &pra);
        pra.p_algo_tuning->imu_pts_offset_start_ms=valule;				
        pra.p_algo_tuning->imu_pts_offset_end_ms=valule1;		
	    hal_eis_set_tuning_pra(eis_info->lib, &pra);
	}

	index = ar_hal_sys_find_pra_index_by_name(args, "--gain");
	if(index > 0)
	{
	    float valule=atof(args->args[index+1]);
	    AR_LOG_RAW("offset_ms=%f\n",valule);

    	STRU_EIS_TUNING_T tuning;
    	STRU_EIS_CTL_UPDATE_TUNING_T pra =
    	{
    		.is_init = 0,
    		.p_algo_tuning = &tuning
    	};
    	hal_eis_get_tuning_pra(eis_info->lib, &pra);
        pra.p_algo_tuning->gain=valule;		
	    hal_eis_set_tuning_pra(eis_info->lib, &pra);
	}

	
	free(args);
	return 0;
}

static AR_VOID eis_binder_cmd_init(STRU_EIS_INFO_S* eis_info)
{
	eis_info->dbg->dump = ar_hal_sys_creat_dump_server();
	eis_info->dbg->dump_after = ar_hal_sys_creat_dump_server();
	eis_info->dbg->binder_cmd = ar_hal_sys_register_binder_cmd("sample_eis", eis_binder_cmd_process, eis_info);
}

static AR_VOID eis_binder_cmd_exit(STRU_EIS_INFO_S* eis_info)
{
	ar_hal_sys_delete_dump_server(eis_info->dbg->dump);
	eis_info->dbg->dump = NULL;
	ar_hal_sys_delete_dump_server(eis_info->dbg->dump_after);
	eis_info->dbg->dump_after = NULL;
	ar_hal_sys_unregister_binder_cmd(eis_info->dbg->binder_cmd);
	eis_info->dbg->binder_cmd = NULL;
}
#else
static AR_VOID eis_binder_cmd_process_dump(STRU_CMD_ARG_T *args, STRU_EIS_INFO_S *eis_info)
{
	(void)args;
	(void)eis_info;
}

static AR_VOID eis_binder_cmd_process_ldc(STRU_CMD_ARG_T *args, STRU_EIS_INFO_S *eis_info)
{
	(void)args;
	(void)eis_info;
}

static AR_VOID eis_binder_cmd_process_eis(STRU_CMD_ARG_T *args, STRU_EIS_INFO_S *eis_info)
{
	(void)args;
	(void)eis_info;
}

static AR_S32 eis_binder_cmd_process(AR_CHAR *cmd_str, AR_S32 len, void *data)
{
	(void)cmd_str;
	(void)len;
	(void)data;
}

static AR_VOID eis_binder_cmd_init(STRU_EIS_INFO_S* eis_info)
{
	(void)eis_info;
}

static AR_VOID eis_binder_cmd_exit(STRU_EIS_INFO_S* eis_info)
{
	(void)eis_info;
}
#endif

static AR_VOID eis_sig_handler(AR_S32 signo)
{
	pthread_t tid = pthread_self();

	if(signo == SIGUSR1 && tid == ((ar_thread_t*)g_sample_eis.eis_info.thread)->tid)
	{
		ar_thread_exit(g_sample_eis.eis_info.thread);
		AR_LOG_DBG(SAMPLE_EIS_MPU_LOG_TAG, "thread %d exit", tid);
	}
}

static AR_S32 sample_eis_open_device(EIS_CONFIG_T* config)
{
	AR_S32 dev_fd = open(SAMPLE_EIS_DEV_NAME, SAMPLE_EIS_DEV_FLAG);

	if(dev_fd < 0)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOT_SUPPORT);
	}

	MPU_INIT_T mpu_init =
	{
		.config = g_sample_eis.mpu_cfg
	};
	AR_S32 ret = ioctl(dev_fd, MPU_REQ_INIT, (unsigned long)&mpu_init);
	if (ret < 0)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "init %d failed %d: %s\n", 
					dev_fd, ret, strerror(errno));
		close(dev_fd);
		dev_fd = -1;
	}

	return dev_fd;
}

static AR_S32 sample_eis_read_device(AR_S32 fd, STRU_EIS_IMMU_DATA_T* data)
{
	if(fd < 0)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	AR_S32 ret = 0;
	AR_U32 output_data_size = sample_eis_get_fifo_bytes_per_pkt(&g_sample_eis.mpu_cfg);

	if(g_sample_eis.mpu_cfg.use_tmst)
	{
		output_data_size += INV_MPU6509_SOFT_TIMESTATMP_LEN;
	}

	// error might be: hdr error, nomem, uncomplete pkt
	ret = sample_eis_read_data(fd, data, output_data_size);
	if(ret < output_data_size)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_SIZE_NOT_ENOUGH);
	}

	return AR_OK;
}

static AR_S32 sample_eis_close_device(AR_S32 fd)
{
	if(fd < 0)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	return close(fd);
}

static AR_S32 sample_eis_reset_device(AR_S32 fd)
{
	if(fd < 0)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	AR_S32 ret = 0;
	AR_S32 dummy;

	ret = ioctl(fd, MPU_REQ_RESET, (unsigned long)&dummy);
	if (ret < 0)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "reset %d failed %d: %s\n",
					fd, ret, strerror(errno));
	}
	return ret;
}

static AR_S32 sample_eis_ctrl_fifo(AR_S32 fd, AR_BOOL gyro_fifo_en, AR_BOOL accl_fifo_en)
{
	AR_S32 ret = 0;
	MPU_CTRL_FIFO_T ctrl =
	{
		.gyro_fifo_enable = gyro_fifo_en,
		.accl_fifo_enable = accl_fifo_en
	};

	ret = ioctl(fd, MPU_REQ_CTRL_FIFO, (unsigned long)&ctrl);
	if (ret < 0)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "send ctrl %d failed %d: %s\n",
							 fd, ret, strerror(errno));
	}
	else
	{
		g_sample_eis.mpu_cfg.accl_fifo_enable = accl_fifo_en;
		g_sample_eis.mpu_cfg.gyro_fifo_enable = gyro_fifo_en;
	}

	return ret;
}

static AR_S32 sample_eis_set_device_config(AR_S32 fd, EIS_CONFIG_T* config)
{
	if(fd < 0)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	AR_S32 ret = AR_OK;
	MPU_CONFIG_T* mpu_cfg = (MPU_CONFIG_T*)(config->data);

	ret = sample_eis_validate_config(mpu_cfg);
	if(ret)
	{
		return ret;
	}

	MPU_SET_CONFIG_T config_set =
	{
		.config = *mpu_cfg
	};
	ret = ioctl(fd, MPU_REQ_SET_CONFIG, (unsigned long)&config_set);
	if (ret < 0)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "set config %d failed %d: %s\n",
							 fd, ret, strerror(errno));
	}
	else
	{
		g_sample_eis.mpu_cfg = *mpu_cfg;
	}

	return ret;
}

static AR_S32 sample_eis_get_device_config(AR_S32 fd, EIS_CONFIG_T* config)
{
	if(fd < 0)
	{
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	AR_S32 ret = AR_OK;
	MPU_GET_CONFIG_T config_get;

	ret = ioctl(fd, MPU_REQ_GET_CONFIG, (unsigned long)&config_get);
	if (ret < 0)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "get config %d failed %d: %s\n",
							 fd, ret, strerror(errno));
	}

	return ret;
}

static const STRU_EIS_LOW_INTERFACE_S sample_eis_low_intf =
{
	.open = sample_eis_open_device,
	.read = sample_eis_read_device,
	.close = sample_eis_close_device,
	.reset = sample_eis_reset_device,
	.ctrl_fifo = sample_eis_ctrl_fifo,
	.set_config = sample_eis_set_device_config,
	.get_config = sample_eis_get_device_config
};

static const STRU_EIS_DEBUG_INTERFACE_S sample_eis_debug_intf =
{
	.init = eis_binder_cmd_init,
	.exit = eis_binder_cmd_exit
};

AR_S32 sample_eis_init(STRU_EIS_TUNING_T* p_algo_tuning)
{
	AR_S32 ret = AR_OK;

	memset(&g_sample_eis, 0, sizeof(g_sample_eis));
	ret = sample_eis_register(&g_sample_eis.eis_info, &sample_eis_low_intf, &sample_eis_debug_intf);
	if(ret)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "eis register failed");
		return ret;
	}

	ret = g_sample_eis.eis_info.high_intf->init(&g_sample_eis.eis_info, SAMPLE_EIS_ALGO_NAME, p_algo_tuning);
	if(ret)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "eis init failed");
		return ret;
	}

	MPU_CONFIG_T* def_cfg = sample_eis_get_default_config();
	g_sample_eis.mpu_cfg = *def_cfg;
	EIS_CONFIG_T config =
	{
		.data = (AR_VOID*)def_cfg,
		.len = sizeof(*def_cfg)
	};
	return g_sample_eis.eis_info.high_intf->config(&g_sample_eis.eis_info, &config);
}

AR_S32 sample_eis_exit(AR_VOID)
{
	AR_S32 ret = AR_OK;

	if (g_sample_eis.eis_info.high_intf) {
		sample_eis_unregister(&g_sample_eis.eis_info);
    
		ret = g_sample_eis.eis_info.high_intf->exit(&g_sample_eis.eis_info);
		g_sample_eis.eis_info.high_intf = NULL;
		return ret;
	}
	return AR_OK;
}

AR_S32 sample_eis_mpu_start(AR_VOID)
{
	AR_S32 ret = AR_OK;

	ret = g_sample_eis.eis_info.high_intf->start(&g_sample_eis.eis_info);
	if(ret)
	{
		return ret;
	}


	struct sigaction sig_actions = {0};

	sigemptyset(&sig_actions.sa_mask);
	sig_actions.sa_flags = 0;
	sig_actions.sa_handler = eis_sig_handler;

	ret = sigaction(SIGUSR1, &sig_actions, NULL);
	if(ret)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "sigaction ret = %d",  ret);
	}

	return ret;
}

AR_S32 sample_eis_stop(AR_VOID)
{
	if(g_sample_eis.eis_info.thread)
	{
		AR_S32 ret = pthread_kill(((ar_thread_t*)g_sample_eis.eis_info.thread)->tid, SIGUSR1);
		if(ret)
		{
			AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "pthread_kill thread %p failed %d", g_sample_eis.eis_info.thread, ret);
		}
	}

	return g_sample_eis.eis_info.high_intf->stop(&g_sample_eis.eis_info);
}

AR_S32 sample_eis_process_frame(VIDEO_FRAME_INFO_S *p_src, STRU_EIS_PROCESS_PARA* para)
{
	return g_sample_eis.eis_info.high_intf->process(&g_sample_eis.eis_info, p_src, para);
}

AR_S32 sample_eis_reset(AR_VOID)
{
	return sample_eis_reset_device(g_sample_eis.eis_info.dev_fd);
}
#endif
