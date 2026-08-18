#ifdef AR_FEAT_EIS

#ifndef __SAMPLE_EIS_MPU_H__
#define __SAMPLE_EIS_MPU_H__

typedef struct
{
	STRU_EIS_INFO_S eis_info;
	MPU_CONFIG_T mpu_cfg;
	AR_S64 prev_kt;
	AR_U64 total_read_len;
} STRU_SAMPLE_EIS_INFO_S;

/* internal interface for various imu chips */
MPU_CONFIG_T* sample_eis_get_default_config(AR_VOID);
AR_S32 sample_eis_get_fifo_bytes_per_pkt(MPU_CONFIG_T* mpu_cfg);
AR_S32 sample_eis_conv_data(STRU_SAMPLE_EIS_INFO_S* info, AR_U8* hwdata, STRU_EIS_IMMU_DATA_T* data, AR_U32 output_data_size);
AR_S32 sample_eis_validate_config(MPU_CONFIG_T *cfg);

/* exernal eis interfaces */
AR_S32 sample_eis_init(STRU_EIS_TUNING_T* p_algo_tuning);
AR_S32 sample_eis_exit(AR_VOID);
AR_S32 sample_eis_mpu_start(AR_VOID);
AR_S32 sample_eis_stop(AR_VOID);
AR_S32 sample_eis_process_frame(VIDEO_FRAME_INFO_S *p_src, STRU_EIS_PROCESS_PARA* para);
AR_S32 sample_eis_reset(AR_VOID);

#endif
#endif
