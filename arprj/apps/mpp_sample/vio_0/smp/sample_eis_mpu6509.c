#ifdef AR_FEAT_EIS
#include "ar_comm_video.h"
#include "hal_type.h"
#include "hal_sys.h"
#include "hal_eis_algo.h"
#include "inv_mpu_ioctl.h"
#include "mpi_dbglog.h"
#include "sample_eis.h"
#include "sample_eis_mpu.h"

#define SAMPLE_EIS_MPU_LOG_TAG		MPP_TAG_ID(AR_ID_VPSS)

enum inv_mpu6509_fifo_wm_prefer_e
{
	INV_MPU6509_ODR_RESV_0_FIFO_WM = 0,
	INV_MPU6509_ODR_32K_FIFO_WM = 16,
	INV_MPU6509_ODR_16K_FIFO_WM = 16,
	INV_MPU6509_ODR_8K_FIFO_WM = 16,
	INV_MPU6509_ODR_4K_FIFO_WM = 16,
	INV_MPU6509_ODR_2K_FIFO_WM = 16,
	INV_MPU6509_ODR_1K_FIFO_WM = 16,
	INV_MPU6509_ODR_200_FIFO_WM = 1,
	INV_MPU6509_ODR_100_FIFO_WM = 1,
	INV_MPU6509_ODR_50_FIFO_WM = 1,
	INV_MPU6509_ODR_25_FIFO_WM = 1,
	INV_MPU6509_ODR_12_5_FIFO_WM = 1,
	INV_MPU6509_ODR_6_25_FIFO_WM = 1,
	INV_MPU6509_ODR_3_125_FIFO_WM = 1,
	INV_MPU6509_ODR_1_5625_FIFO_WM = 1,
	INV_MPU6509_ODR_500_FIFO_WM = 8,
	INV_MPU6509_ODR_FIFO_WM_MAX
};

enum inv_mpu6509_fifo_packet_error_e
{
	INV_MPU6509_FIFO_ERROR_HDR_HW_TMST_UNEXIST = -7,
	INV_MPU6509_FIFO_ERROR_HDR_GYRO_WRONG_UNEXIST_BUT_ENABLED = -6,
	INV_MPU6509_FIFO_ERROR_HDR_GYRO_WRONG_EXIST_BUT_DISABLED = -5,
	INV_MPU6509_FIFO_ERROR_HDR_ACCL_WRONG_UNEXIST_BUT_ENABLED = -4,
	INV_MPU6509_FIFO_ERROR_HDR_ACCL_WRONG_EXIST_BUT_DISABLED = -3,
	INV_MPU6509_FIFO_ERROR_HDR_FMT_WRONG_BOTH = -2,
	INV_MPU6509_FIFO_ERROR_FIFO_EMPTY = -1
};

// fifo header
enum inv_mpu6509_fifo_hdr_odr_e {
	INV_MPU6509_FIFO_HDR_ODR_DIFFERENT,
	INV_MPU6509_FIFO_HDR_ODR_SAME
};

#define INV_MPU6509_BIT_OFFSET_FIFO_HEADER_ORD_GYRO			0
#define INV_MPU6509_BIT_OFFSET_FIFO_HEADER_ORD_ACCEL		1

enum inv_mpu6509_fifo_hdr_timestamp_fsync_e {
	INV_MPU6509_FIFO_HDR_TIMESTAMP_FSYNC_NONE,
	INV_MPU6509_FIFO_HDR_TIMESTAMP_FSYNC_RESV,
	INV_MPU6509_FIFO_HDR_TIMESTAMP_FSYNC_TIMESTAMP,
	INV_MPU6509_FIFO_HDR_TIMESTAMP_FSYNC_FSYNC
};
#define INV_MPU6509_BIT_OFFSET_FIFO_HEADER_TIMESTAMP_FSYNC	2
#define INV_MPU6509_FIFO_HEADER_TIMESTAMP_FSYNC_MASK		0x3

enum inv_mpu6509_fifo_hdr_sensor_data_flag_e {
	INV_MPU6509_FIFO_HDR_SENSOR_DATA_UNEXIST,
	INV_MPU6509_FIFO_HDR_SENSOR_DATA_EXIST
};

#define INV_MPU6509_BIT_OFFSET_FIFO_HEADER_GYRO				5
#define INV_MPU6509_BIT_OFFSET_FIFO_HEADER_ACCEL			6

enum inv_mpu6509_fifo_hdr_data_flag_e {
	INV_MPU6509_FIFO_HDR_NOT_EMPTY,
	INV_MPU6509_FIFO_HDR_EMPTY
};

#define INV_MPU6509_BIT_OFFSET_FIFO_HEADER_MSG				7

typedef struct
{
	AR_BOOL empty;
	AR_BOOL accl_exist;
	AR_BOOL gyro_exist;
	AR_U32 tmst_fsync;
	AR_BOOL accl_odr_changed;
	AR_BOOL gyro_odr_changed;
} INV_MPU6509_FIFO_HEADER_T;

static const AR_FLOAT gyro_scale[INV_MPU6509_GYRO_FS_SEL_MAX] =
{
	0.001064724, 0.000532362, 0.000266181, 0.000133090, 0.000066545, 0.000033272, 0.000016636, 0.000008318
};

static const AR_FLOAT accl_scale[INV_MPU6509_ACCEL_FS_SEL_MAX] =
{
	0.009570, 0.004785, 0.002392, 0.001196
};

static MPU_CONFIG_T sample_mpu_config_default =
{
	.odr = INV_MPU6509_GYRO_ODR_100,
	.fifo_wm = INV_MPU6509_ODR_100_FIFO_WM,

	.gyro_enable = AR_TRUE,
	.gyro_fsr = INV_MPU6509_GYRO_FS_SEL_2000,
	.gyro_aaf_enable = AR_TRUE,
	.gyro_aaf_delt = 6,
	.gyro_fifo_enable = AR_TRUE,

	.accl_enable = AR_TRUE,
	.accl_fsr = INV_MPU6509_ACCEL_FS_SEL_4,
	.accl_aaf_enable = AR_TRUE,
	.accl_aaf_delt = 1,
	.accl_fifo_enable = AR_TRUE,

	.ui_filt_ord = INV_MPU6509_GYRO_UI_FILT_ORD_1,
	.ui_filt_bw = INV_MPU6509_GYRO_UI_FILT_BW_MAX_400_ODR_DIV_BY_5,

	.use_tmst = AR_TRUE,
	.tmst_res = 16
};

static AR_VOID sample_eis_dump_data(STRU_SAMPLE_EIS_INFO_S* info, AR_U8* hwdata, STRU_EIS_IMMU_DATA_T* data, AR_U32 len)
{
	AR_U32 pkt_len = sample_eis_get_fifo_bytes_per_pkt(&info->mpu_cfg);
	AR_U32 i = pkt_len - INV_MPU6509_BYTES_FIFO_TEMP;
	AR_FLOAT temperature;
	AR_U64 ar_tmst = ar_get_timestamp();

	if (info->mpu_cfg.accl_fifo_enable && info->mpu_cfg.gyro_fifo_enable)
	{
		i -= INV_MPU6509_BYTES_FIFO_TMST;
	}

	temperature = (AR_S8)(hwdata[i]);
	temperature = temperature / 2.07 + 25;

	AR_LOG_RAW("raw\t: [%lld] ts=%lld ", ar_tmst, data->time_stamps);
	for(i=0; i<len; i++)
	{
		AR_LOG_RAW("%02x ", hwdata[i]);
	}
	AR_LOG_RAW("\n");

	AR_LOG_RAW("frmt\t: [%lld] ts=%lld ", ar_tmst, data->time_stamps);
	if (info->mpu_cfg.accl_fifo_enable)
	{
		AR_LOG_RAW("accl=(%f, %f, %f) ", data->x_acce, data->y_acce, data->z_acce);
	}
	if (info->mpu_cfg.gyro_fifo_enable)
	{
		AR_LOG_RAW("gyro=(%f, %f, %f) ", data->x, data->y, data->z);
	}
	AR_LOG_RAW("temp=%f \n", temperature);

	AR_LOG_RAW("delta\t: [%lld] prev_kt=%lld curr_kt=%lld dkt=%lld ", ar_tmst, info->prev_kt, data->time_stamps, (AR_S64)(data->time_stamps - info->prev_kt));
	AR_LOG_RAW("total_read=%lld\n", info->total_read_len);

	info->prev_kt = data->time_stamps;
}

static AR_S32 sample_eis_validate_fifo_pkt_hdr(MPU_CONFIG_T* mpu_cfg, INV_MPU6509_FIFO_HEADER_T *hdr)
{
	if(hdr->empty)
	{
		return INV_MPU6509_FIFO_ERROR_FIFO_EMPTY;
	}

	if(!hdr->accl_exist && !hdr->gyro_exist)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "fifo not empty but no data exist\n");
		return INV_MPU6509_FIFO_ERROR_HDR_FMT_WRONG_BOTH;
	}

	if(hdr->accl_exist && !mpu_cfg->accl_enable)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "TO CHECK: accel data exist but not enabled in config\n");
		return INV_MPU6509_FIFO_ERROR_HDR_ACCL_WRONG_EXIST_BUT_DISABLED;
	}

	if(!hdr->accl_exist && mpu_cfg->accl_enable)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "TO CHECK: accel data not exist but enabled in config\n");
		return INV_MPU6509_FIFO_ERROR_HDR_ACCL_WRONG_UNEXIST_BUT_ENABLED;
	}

	if(hdr->gyro_exist && !mpu_cfg->gyro_enable)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "TO CHECK: gyro data exist but not enabled in config\n");
		return INV_MPU6509_FIFO_ERROR_HDR_GYRO_WRONG_EXIST_BUT_DISABLED;
	}

	if(!hdr->gyro_exist && mpu_cfg->gyro_enable)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "TO CHECK: gyro data not exist but enabled in config\n");
		return INV_MPU6509_FIFO_ERROR_HDR_GYRO_WRONG_UNEXIST_BUT_ENABLED;
	}

	// fsync not used
	if(mpu_cfg->gyro_enable && mpu_cfg->accl_enable
		&& hdr->tmst_fsync != INV_MPU6509_FIFO_HDR_TIMESTAMP_FSYNC_TIMESTAMP)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "TO CHECK: both accel and gyro data enabled in config but no odr tmst %d\n", hdr->tmst_fsync);
		return INV_MPU6509_FIFO_ERROR_HDR_HW_TMST_UNEXIST;
	}

	if(hdr->accl_odr_changed)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "accel data odr changed\n");
	}

	if(hdr->gyro_odr_changed)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "gyro data odr changed\n");
	}

	return AR_OK;
}

AR_S32 sample_eis_conv_data(STRU_SAMPLE_EIS_INFO_S* info, AR_U8* hwdata, STRU_EIS_IMMU_DATA_T* data, AR_U32 output_data_size)
{
	AR_S32 result;
	AR_U16 idx = 0;
	AR_S16 tmp;
	INV_MPU6509_FIFO_HEADER_T hdr;

	hdr.empty = (hwdata[idx] >> INV_MPU6509_BIT_OFFSET_FIFO_HEADER_MSG) & 0x1;
	hdr.accl_exist = (hwdata[idx] >> INV_MPU6509_BIT_OFFSET_FIFO_HEADER_ACCEL) & 0x1;
	hdr.gyro_exist = (hwdata[idx] >> INV_MPU6509_BIT_OFFSET_FIFO_HEADER_GYRO) & 0x1;
	hdr.tmst_fsync = (hwdata[idx] >> INV_MPU6509_BIT_OFFSET_FIFO_HEADER_TIMESTAMP_FSYNC) & INV_MPU6509_FIFO_HEADER_TIMESTAMP_FSYNC_MASK;
	hdr.accl_odr_changed = (hwdata[idx] >> INV_MPU6509_BIT_OFFSET_FIFO_HEADER_ORD_ACCEL) & 0x1;
	hdr.gyro_odr_changed = (hwdata[idx] >> INV_MPU6509_BIT_OFFSET_FIFO_HEADER_ORD_GYRO) & 0x1;

	result = sample_eis_validate_fifo_pkt_hdr(&info->mpu_cfg, &hdr);
	if(result)
		return result;
	idx += INV_MPU6509_BYTES_FIFO_HDR;

	if(hdr.accl_exist)
	{
		tmp = (AR_S8)hwdata[idx++];
		tmp = (tmp << 8) | hwdata[idx++];
		data->x_acce = tmp * accl_scale[info->mpu_cfg.accl_fsr];

		tmp = (AR_S8)hwdata[idx++];
		tmp = (tmp << 8) | hwdata[idx++];
		data->y_acce = tmp * accl_scale[info->mpu_cfg.accl_fsr];

		tmp = (AR_S8)hwdata[idx++];
		tmp = (tmp << 8) | hwdata[idx++];
		data->z_acce = tmp * accl_scale[info->mpu_cfg.accl_fsr];
	}

	if(hdr.gyro_exist)
	{
		tmp = (AR_S8)hwdata[idx++];
		tmp = (tmp << 8) | hwdata[idx++];
		data->x = tmp * gyro_scale[info->mpu_cfg.gyro_fsr];

		tmp = (AR_S8)hwdata[idx++];
		tmp = (tmp << 8) | hwdata[idx++];
		data->y = tmp * gyro_scale[info->mpu_cfg.gyro_fsr];

		tmp = (AR_S8)hwdata[idx++];
		tmp = (tmp << 8) | hwdata[idx++];
		data->z = tmp * gyro_scale[info->mpu_cfg.gyro_fsr];
	}

	if(info->mpu_cfg.use_tmst)
	{
		data->time_stamps = *((int64_t *)&hwdata[output_data_size - INV_MPU6509_SOFT_TIMESTATMP_LEN]);
		data->time_stamps /= 1000000;
	}
	else
	{
		data->time_stamps = 0;
	}

//	eis_dump_data(info, hwdata, data, output_data_size);

	return 0;
}

AR_S32 sample_eis_validate_config(MPU_CONFIG_T *cfg)
{
	if(cfg->odr >= INV_MPU6509_GYRO_ODR_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config odr %d", cfg->odr);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(!cfg->gyro_enable && !cfg->accl_enable)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config sensor enable gyro %d accel %d", cfg->gyro_enable, cfg->accl_enable);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->gyro_enable && cfg->gyro_fsr >= INV_MPU6509_GYRO_FS_SEL_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config gyro fsr %d", cfg->gyro_fsr);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->gyro_enable && cfg->gyro_aaf_delt >= INV_MPU6509_GYRO_AAF_DELT_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config gyro aaf delt %d", cfg->gyro_aaf_delt);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->accl_enable && cfg->accl_fsr >= INV_MPU6509_ACCEL_FS_SEL_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config gyro fsr %d", cfg->accl_fsr);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->accl_enable && cfg->accl_aaf_delt >= INV_MPU6509_ACCL_AAF_DELT_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config accel aaf delt %d", cfg->accl_aaf_delt);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->ui_filt_ord >= INV_MPU6509_GYRO_UI_FILT_ORD_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config ui filt ord %d", cfg->ui_filt_ord);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->ui_filt_bw >= INV_MPU6509_GYRO_UI_FILT_BW_MAX)
	{
		AR_LOG_ERR(SAMPLE_EIS_MPU_LOG_TAG, "invalid config ui filt bw %d", cfg->ui_filt_bw);
		return AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM);
	}

	if(cfg->fifo_wm >> (INV_MPU6509_FIFO_WM_BIT_HIGHEST+1))
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "invalid config fifo watermark %x can only upto %d bits", cfg->fifo_wm, (INV_MPU6509_FIFO_WM_BIT_HIGHEST+1));
	}

	if(cfg->tmst_res != 1 || cfg->tmst_res != 16)
	{
		AR_LOG_WARN(SAMPLE_EIS_MPU_LOG_TAG, "invalid config tmst resolution %d can only be 1 or 16", cfg->tmst_res);
	}

	return AR_OK;
}

MPU_CONFIG_T* sample_eis_get_default_config(AR_VOID)
{
	return &sample_mpu_config_default;
}

AR_S32 sample_eis_get_fifo_bytes_per_pkt(MPU_CONFIG_T* mpu_cfg)
{
	if (mpu_cfg->accl_fifo_enable && mpu_cfg->gyro_fifo_enable) {
		return INV_MPU6509_FIFO_PACKETS_LEN_MODE3;
	} else if (mpu_cfg->gyro_fifo_enable) {
		return INV_MPU6509_FIFO_PACKETS_LEN_MODE2;
	} else if (mpu_cfg->accl_fifo_enable) {
		return INV_MPU6509_FIFO_PACKETS_LEN_MODE1;
	}

	return 0;
}
#endif
