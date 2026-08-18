#ifdef AR_FEAT_EIS

#ifndef __SAMPLE_EIS_H__
#define __SAMPLE_EIS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define EIS_MAX_NAME_LEN					(32)

typedef struct
{
	AR_VOID* data;
	AR_U32 len;
} EIS_CONFIG_T;

typedef struct
{
	AR_S32 s32_dev_id;
	AR_S32 s32_chn_id;
	AR_U32 u32_walign;
	AR_U32 u32_halign;

	AR_VOID* resv;
} STRU_EIS_PROCESS_PARA;

struct STRU_EIS_INFO_S_TAG;
typedef struct STRU_EIS_INFO_S_TAG STRU_EIS_INFO_S;

typedef struct
{
	AR_S32 (*init)(STRU_EIS_INFO_S* eis_info, AR_CHAR* algo_name, STRU_EIS_TUNING_T* tuning);
	AR_S32 (*exit)(STRU_EIS_INFO_S* eis_info);
	AR_S32 (*start)(STRU_EIS_INFO_S* eis_info);
	AR_S32 (*stop)(STRU_EIS_INFO_S* eis_info);
	AR_S32 (*config)(STRU_EIS_INFO_S* eis_info, EIS_CONFIG_T* config);
	AR_S32 (*process)(STRU_EIS_INFO_S* eis_info, VIDEO_FRAME_INFO_S* frame, STRU_EIS_PROCESS_PARA* para);
}  STRU_EIS_HIGH_INTERFACE_S;

typedef struct
{
	AR_S32 (*open)(EIS_CONFIG_T* config);
	AR_S32 (*read)(AR_S32 fd, STRU_EIS_IMMU_DATA_T *data);
	AR_S32 (*close)(AR_S32 fd);
	AR_S32 (*reset)(AR_S32 fd);
	AR_S32 (*ctrl_fifo)(AR_S32 fd, AR_BOOL gyro_fifo_en, AR_BOOL accl_fifo_en);
	AR_S32 (*set_config)(AR_S32 fd, EIS_CONFIG_T* config);
	AR_S32 (*get_config)(AR_S32 fd, EIS_CONFIG_T* config);
} STRU_EIS_LOW_INTERFACE_S;

typedef struct
{
	AR_VOID (*init)(STRU_EIS_INFO_S* eis_info);
	AR_VOID (*exit)(STRU_EIS_INFO_S* eis_info);
} STRU_EIS_DEBUG_INTERFACE_S;

typedef struct
{
	STRU_DUMP_OBJ_T *dump;
	STRU_DUMP_OBJ_T *dump_after;
	BINDER_CMD_T *binder_cmd;
	STRU_EIS_DEBUG_INTERFACE_S* intf;
} STRU_EIS_DEBUG_CNTX_S;

struct STRU_EIS_INFO_S_TAG
{
	STRU_EIS_ALGO_LIB_T* lib;
	AR_CHAR algo_name[EIS_MAX_NAME_LEN];
	ar_thread_id_t thread;
	ar_lock_id_t lock;
	AR_BOOL stop;

	AR_S32 dev_fd;
	STRU_EIS_TUNING_T tuning;
	EIS_CONFIG_T config;

	AR_BOOL is_first;
	VIDEO_FRAME_INFO_S last_frame;
	AR_S32 no_imu_data_match_ref;

	STRU_EIS_HIGH_INTERFACE_S* high_intf;
	STRU_EIS_LOW_INTERFACE_S* low_intf;
	STRU_EIS_DEBUG_CNTX_S* dbg;

	AR_VOID* priv;
};

AR_S32 sample_eis_register(STRU_EIS_INFO_S* eis_info, const STRU_EIS_LOW_INTERFACE_S* low_intf, const STRU_EIS_DEBUG_INTERFACE_S* dbg_intf);
AR_S32 sample_eis_unregister(STRU_EIS_INFO_S* eis_info);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_EIS_H__ */
#endif
