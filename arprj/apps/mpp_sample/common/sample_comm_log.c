#include "sample_comm_log.h"

static int app_log_id = -1;

int ar_get_app_log_id()
{
	int ret = 0;

	if(-1 != app_log_id)
		return app_log_id;

	STRU_MOD_INFO log_info;
	//使用用户tag注册log id; 数量有限，建议不要大量注册。
	char *osal_tag = "APP_SAMP";
	memset(&log_info, 0, sizeof(STRU_MOD_INFO));
	memcpy(log_info.tag, osal_tag, strlen(osal_tag));
	log_info.id = -1;
	ret = ar_log_register(&log_info);

	if(!ret ){
		app_log_id = log_info.id;
		ret = log_info.id;
	}

	return ret;
}

