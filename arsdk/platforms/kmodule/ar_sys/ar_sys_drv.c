#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/sched_clock.h>
#include <linux/mutex.h>

#include "osal_mmz.h"
#include "ar_sys_drv.h"

#define				__MIN_TIME_ZONE_OFFSET__		(-86400)
#define				__MAX_TIME_ZONE_OFFSET__		(86400)

spinlock_t			g_sys_spin_lock;
struct mutex		g_sys_ioctl_lock;
int					g_time_zone = 0;
STRU_SYS_GPS_INFO	g_str_gps_info = {0};

unsigned long long	g_u64GlobalPTSBase = 0;
unsigned long long	g_u64GlobalPTSBaseLast = 0;
unsigned long long	g_u64LocalPTSBase = 0;
unsigned long long	g_u64TimeLast = 0;
long long	        g_u64GlobalPTSOffset = 0;

static int s_log_level_core[LOG_RSV_ID_START] = {0};
static int s_log_level_hal[LOG_RSV_ID_START]  = {0};
static int s_log_level_mpp[LOG_RSV_ID_START]  = {0};
static int s_log_level_rsv[LOG_RSV_NUM]      = {0};


static char s_base_mod_info[LOG_MOD_MAX_NUM][LOG_TAG_MAX_LEN];

static const char *s_layer_info[3] = {
    [0]  = "_CORE",
    [1]  = "_HAL",
    [2]  = "_MPP",
};

static STRU_QUERY_INFO s_log_query_info;

typedef struct
{
	int args_num;
	char args[2][16];
}args_t;

static int loglevel = 2;
module_param(loglevel, int, S_IRUGO | S_IWUSR);
#define AR_SYS_MPP_SERVICE_TYPE_LEN   8
static char mpp_service_type[AR_SYS_MPP_SERVICE_TYPE_LEN] = "app";
module_param_string(mpp_service, mpp_service_type, AR_SYS_MPP_SERVICE_TYPE_LEN, 0600);

static int ar_sys_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct osal_vm osal_vm;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

	osal_vm.vm = vma;

	if(hil_is_phys_in_mmz(offset, vma->vm_end - vma->vm_start)) {
		if(hil_map_mmz_check_phys(offset, vma->vm_end - vma->vm_start)) {
			printk(KERN_INFO "hil_map_mmz_check_phys failed!\n");
			return -EPERM;
		}
	}

	osal_pgprot_cached(&osal_vm);
	if (osal_remap_pfn_range(&osal_vm, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start)) {
		printk(KERN_INFO "osal_remap_pfn_range failed!\n");
		return -EAGAIN;
	}

	return 0;
}

static int ar_sys_flush_cache(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct dirty_area	   da;
	unsigned long		   pa;
	unsigned long		   va_tmp, pa_tmp;
	unsigned long		   offset;

	if(sizeof(da) != _IOC_SIZE(cmd)) {
		printk(KERN_ERR "%s: struct length error.\n", __func__);
		return -EINVAL;
	}

	if (copy_from_user(&da, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
		printk(KERN_ERR "%s: copy_from_user error.\n", __func__);
		return -EFAULT;
	}

	if(0 != (pa = usr_virt_to_phys((unsigned long)da.dirty_virt_start))) {
		printk(KERN_ERR "%s: 0. usr_virt_to_phys error.\n", __func__);
		return -EFAULT;
	}

	pa &= (~0x01);
	if(pa != da.dirty_phys_start) {
		printk(KERN_ERR "%s: dirty_phys_start error.\n", __func__);
		return -EFAULT;
	}

	offset = da.dirty_size - 0x40;
	va_tmp = (unsigned long)da.dirty_virt_start + offset;
	if(0 != (pa_tmp = usr_virt_to_phys((unsigned long)da.dirty_virt_start))) {
		printk(KERN_ERR "%s: 1. usr_virt_to_phys error.\n", __func__);
		return -EFAULT;
	}

	pa_tmp &= (~0x01);
	if(pa + offset != pa_tmp) {
		printk(KERN_ERR "%s: pa error.\n", __func__);
		return -EFAULT;
	}

	return hil_mmb_flush_dcache_byaddr_safe(da.dirty_virt_start, da.dirty_phys_start, da.dirty_size);
}

static int ar_sys_set_time_zone(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int tmp_time_offset = 0;
	if(sizeof(tmp_time_offset) != _IOC_SIZE(cmd)) {
		printk(KERN_ERR "%s: struct length error.\n", __func__);
		return -EINVAL;
	}

	if (copy_from_user(&tmp_time_offset, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
		printk(KERN_ERR "%s: copy_from_user error.\n", __func__);
		return -EFAULT;
	}

	if(tmp_time_offset < __MIN_TIME_ZONE_OFFSET__ || tmp_time_offset > __MAX_TIME_ZONE_OFFSET__) {
		printk(KERN_ERR "%s: tmp_time_offset %d out of range[%d, %d].\n", __func__,
			tmp_time_offset, __MIN_TIME_ZONE_OFFSET__, __MAX_TIME_ZONE_OFFSET__);
		return -EINVAL;
	}

	g_time_zone = tmp_time_offset;

	return 0;
}

static int ar_sys_get_time_zone(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if(sizeof(g_time_zone) != _IOC_SIZE(cmd)) {
		printk(KERN_ERR "%s: struct length error.\n", __func__);
		return -EINVAL;
	}

	if (copy_to_user((void *)(uintptr_t)arg, &g_time_zone, _IOC_SIZE(cmd))) {
		printk(KERN_ERR "%s: copy_to_user error.\n", __func__);
		return -EFAULT;
	}

	return 0;
}

static int ar_sys_set_gps_info(struct file *filp, unsigned int cmd, unsigned long arg)
{
	STRU_SYS_GPS_INFO tmp_gps_info = {0};
	int i = 0;

	if(sizeof(tmp_gps_info) != _IOC_SIZE(cmd)) {
		printk(KERN_ERR "%s: struct length error.\n", __func__);
		return -EINVAL;
	}

	if (copy_from_user(&tmp_gps_info, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
		printk(KERN_ERR "%s: copy_from_user error.\n", __func__);
		return -EFAULT;
	}

	if(tmp_gps_info.ch_gps_latitude_ref != 'N' && tmp_gps_info.ch_gps_latitude_ref != 'S') {
		printk(KERN_ERR "%s:ch_gps_latitude_ref error.\n", __func__);
		return -EINVAL;
	}

	if(tmp_gps_info.ch_gps_longitude_ref != 'E' && tmp_gps_info.ch_gps_longitude_ref != 'W') {
		printk(KERN_ERR "%s:ch_gps_longitude_ref error.\n", __func__);
		return -EINVAL;
	}

	if(tmp_gps_info.u8_gps_altitude_ref != 0 && tmp_gps_info.u8_gps_altitude_ref != 1) {
		printk(KERN_ERR "%s:u8_gps_altitude_ref error.\n", __func__);
		return -EINVAL;
	}

	for(i = 0; i < 3; ++i)
	{
		if(0 == tmp_gps_info.au32_gps_latitude[i][1]) {
			printk(KERN_ERR "%s:au32_gps_latitude[%d][1] can not be zero.\n", __func__, i);
			return -EINVAL;
		}

		if(0 == tmp_gps_info.au32_gps_longitude[i][1]) {
			printk(KERN_ERR "%s:au32_gps_longitude[%d][1] can not be zero.\n", __func__, i);
			return -EINVAL;
		}
	}

	memcpy(&g_str_gps_info, &tmp_gps_info, _IOC_SIZE(cmd));

	return 0;
}

static int ar_sys_get_gps_info(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if(sizeof(g_str_gps_info) != _IOC_SIZE(cmd)) {
		printk(KERN_ERR "%s: struct length error.\n", __func__);
		return -EINVAL;
	}

	if (copy_to_user((void *)(uintptr_t)arg, &g_str_gps_info, _IOC_SIZE(cmd))) {
		printk(KERN_ERR "%s: copy_to_user error.\n", __func__);
		return -EFAULT;
	}

	return 0;
}

static int ar_sys_ioctl_y(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	switch(_IOC_NR(cmd)) {
		case _IOC_NR(IOC_SYS_FLUSH_DCACHE_DIRTY):
			ret = ar_sys_flush_cache(filp, cmd, arg);
			break;
		case _IOC_NR(IOC_SYS_SET_TIME_ZONE):
			ret = ar_sys_set_time_zone(filp, cmd, arg);
			break;
		case _IOC_NR(IOC_SYS_GET_TIME_ZONE):
			ret = ar_sys_get_time_zone(filp, cmd, arg);
			break;
		case _IOC_NR(IOC_SYS_SET_GPS_INFO):
			ret = ar_sys_set_gps_info(filp, cmd, arg);
			break;
		case _IOC_NR(IOC_SYS_GET_GPS_INFO):
			ret = ar_sys_get_gps_info(filp, cmd, arg);
			break;
		default:
			ret = -1;
            break;
	}
	return ret;
}

static unsigned long long ar_sys_get_local_curPTs(void)
{
	unsigned long long u64_time_now = sched_clock();

	if( u64_time_now < g_u64TimeLast){

		printk(KERN_ERR "get sched_clock error, u64_time_now:%llu， u64_time_last:%llu.\n", u64_time_now, g_u64TimeLast);
		return -EFAULT;
	}

	g_u64TimeLast = u64_time_now;

	//printk(KERN_INFO "get_local_curPTs:%llu.\n", div_u64(u64_time_now, 1000LL));
	return div_u64(u64_time_now, 1000LL);
}

static unsigned long long ar_sys_get_time_stamp(void)
{
	unsigned long long u64_time_stamp = -1;
	int flag = -1;
	u64_time_stamp = g_u64GlobalPTSBase - g_u64LocalPTSBase + ar_sys_get_local_curPTs();

	if(g_u64GlobalPTSBaseLast){
		flag = u64_time_stamp >= g_u64GlobalPTSBaseLast;
	}else{
		flag = 1;
	}

	if(!flag)
		u64_time_stamp = g_u64GlobalPTSBaseLast + 10;

	g_u64GlobalPTSBaseLast = u64_time_stamp;

	return u64_time_stamp;
}

static int ar_sys_sync_time_stamp(unsigned long long pts_base, int init_flag)
{
	g_u64GlobalPTSBase = pts_base;
	g_u64LocalPTSBase = ar_sys_get_local_curPTs();
	if(init_flag)
		g_u64GlobalPTSBaseLast = 0;

    g_u64GlobalPTSOffset = pts_base*1000 - ktime_get_raw_ns();

	return 0;
}

static int parse_string(const char *pMsgString, const char	*split_char, args_t* args)
{
	if(!args || !split_char || !args){
		return -1;
	}

	char *p_str =  NULL;
	char p_cmd[512] = {0};
	char *p = p_cmd;
	//int str_len = strlen(pMsgString);

	memset(args, 0, sizeof(args_t));
	strcpy(p_cmd, pMsgString);

	p_str=strsep(&p, split_char);

	if(p){
		printk(KERN_INFO "### %s %d p_str:%s p:%s\n", __func__,  __LINE__, p_str, p);
		strncpy(args->args[0], p_str, sizeof(args->args[0])-1);
		strncpy(args->args[1], p, sizeof(args->args[1])-1);
	    args->args_num = 2;
	}

	return 0;
}

static void set_all_level(int level)
{
	int i;
	for(i=0; i<LOG_RSV_ID_START; ++i){
		s_log_level_core[i] = level;
		s_log_level_hal[i] = level;
		s_log_level_mpp[i] = level;
	}

	for(i=LOG_RSV_ID_START; i<LOG_MOD_MAX_NUM; ++i){
		s_log_level_rsv[i-LOG_RSV_ID_START] = level;
	}
}

static int ar_sys_ioctl_p(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	switch(cmd) {
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
		case IOC_SYS_INIT_PTS_BASE:{
			unsigned long long pts_base = 0;
			if (copy_from_user(&pts_base, (void __user *)arg, sizeof(pts_base))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}

			ret = ar_sys_sync_time_stamp(pts_base, 1);
			break;
		}

		case IOC_SYS_GET_CUR_PTS:{
			unsigned long long get_cur_pts = 0;
			unsigned long long u64_cur_pts = ar_sys_get_time_stamp();
			if (copy_from_user(&get_cur_pts, (void __user *)arg, _IOC_SIZE(cmd))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}
			get_cur_pts &= 0xFFFFFFFF;
			ret = copy_to_user((void __user *)get_cur_pts, &u64_cur_pts, sizeof(unsigned long long));
			break;
		}

		case IOC_SYS_SYNC_PTS:{
			unsigned long long pts_base = 0;
			if (copy_from_user(&pts_base, (void __user *)arg, _IOC_SIZE(cmd))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}
			ret = ar_sys_sync_time_stamp(pts_base, 0);
			break;
		}

        case IOC_SYS_GET_PTS_OFFSET:{
            unsigned long long pts_offset = 0;
			if (copy_from_user(&pts_offset, (void __user *)arg, _IOC_SIZE(cmd))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}
			pts_offset &= 0xFFFFFFFF;
            ret = copy_to_user((void __user *)pts_offset, &g_u64GlobalPTSOffset, sizeof(long long));
            break;
        }
#else
		case IOC_SYS_INIT_PTS_BASE:{
				unsigned long long *pts_base = (unsigned long long *)arg;
				ret = ar_sys_sync_time_stamp(*pts_base, 1);
				break;
			}

		case IOC_SYS_GET_CUR_PTS:{
				unsigned long long *get_cur_pts = (unsigned long long *)arg;
				unsigned long long u64_cur_pts = ar_sys_get_time_stamp();
				ret = copy_to_user(get_cur_pts, &u64_cur_pts, sizeof(unsigned long long));
				break;
			}

		case IOC_SYS_SYNC_PTS:{
				unsigned long long *pts_base = (unsigned long long *)arg;
				ret = ar_sys_sync_time_stamp(*pts_base, 0);
				break;
			}
	
		case IOC_SYS_GET_PTS_OFFSET:{
				long long *pts_offset = (long long *)arg;
				ret = copy_to_user(pts_offset, &g_u64GlobalPTSOffset, sizeof(long long));
	            break;
	        }
#endif

		case IOC_SYS_GET_LOG_LEVEL:{
			STRU_MOD_INFO log_mod_info;
			STRU_MOD_INFO *mod_info;
			int log_mask_id;
			int log_layer;
			int log_mod_id;

			if (copy_from_user(&log_mod_info, (struct STRU_MOD_INFO __user *)arg, sizeof(log_mod_info))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}

			log_mask_id = log_mod_info.id;
			log_mod_id = log_mask_id & LOG_MOD_ID_MASK;
			if(log_mod_id>LOG_MOD_MAX_NUM-1 || log_mod_id<0 || 0 == strlen(s_base_mod_info[log_mod_id])){
				ret = -EFAULT;
				//pr_err("bad log_mod_id = %d \n", log_mod_id);
				break;
			}

			log_layer = log_mask_id  & LOG_LAYER_ID_MASK;

			if(LOG_LAYER_CORE == log_layer) {
				if(log_mod_id >= LOG_RSV_ID_START) {
					ret = -EINVAL;
					break;
				}
				log_mod_info.level = s_log_level_core[log_mod_id];
				snprintf(log_mod_info.tag, LOG_TAG_MAX_LEN-1, "%s%s", s_base_mod_info[log_mod_id], s_layer_info[0]);
			} else if(LOG_LAYER_HAL == log_layer) {
				if(log_mod_id >= LOG_RSV_ID_START) {
					ret = -EINVAL;
					break;
				}
				log_mod_info.level = s_log_level_hal[log_mod_id];
				snprintf(log_mod_info.tag, LOG_TAG_MAX_LEN-1, "%s%s", s_base_mod_info[log_mod_id], s_layer_info[1]);
			} else if(LOG_LAYER_MPP == log_layer) {
				if(log_mod_id >= LOG_RSV_ID_START) {
					ret = -EINVAL;
					break;
				}
				log_mod_info.level = s_log_level_mpp[log_mod_id];
				snprintf(log_mod_info.tag, LOG_TAG_MAX_LEN-1, "%s%s", s_base_mod_info[log_mod_id], s_layer_info[2]);
			} else {
				if(log_mod_id < LOG_RSV_ID_START) {
					ret = -EINVAL;
					break;
				}
				log_mod_info.level = s_log_level_rsv[log_mod_id - LOG_RSV_ID_START];
				snprintf(log_mod_info.tag, LOG_TAG_MAX_LEN-1, "%s", s_base_mod_info[log_mod_id]);
			}

			mod_info = (STRU_MOD_INFO __user *)arg;
			ret = copy_to_user(mod_info, &log_mod_info, sizeof(STRU_MOD_INFO));

			break;
		}

		case IOC_SYS_SET_LOG_LEVEL:{
			STRU_MOD_INFO log_mod_info;
			int log_mask_id = 0;
			int log_layer = 0;
			int log_mod_id = -1;
			args_t args;
			int i;

			if (copy_from_user(&log_mod_info, (struct STRU_MOD_INFO __user *)arg, sizeof(log_mod_info))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}
			printk(KERN_INFO "!!!!! %s %d %s %02x %d\n", __func__,  __LINE__, log_mod_info.tag, log_mod_info.id, log_mod_info.level);

			if(log_mod_info.level < 0 || log_mod_info.level > 5){
				pr_err("bad log_mod_level = %d \n", log_mod_info.level);
				break;
			}

			//1. set log level by id
			if(log_mod_info.id != -1){
				log_mask_id = log_mod_info.id;
				log_layer = log_mask_id  & LOG_LAYER_ID_MASK;
				log_mod_id = log_mask_id & LOG_MOD_ID_MASK;

				if(log_mod_id > LOG_MOD_MAX_NUM-1 ){
					ret = -EFAULT;
					pr_err("bad log_mod_id= %d \n", log_mod_id);
					break;
				}

				if(!strlen(s_base_mod_info[log_mod_id])){
					ret = -EFAULT;
					pr_err("log_mod_id %d not register\n", log_mod_id);
					break;
				}

			}else{  //2. set log level by tag
				if(log_mod_info.tag[0] == 0)
					break ;

				//2.1 split tag  (_)
				parse_string(log_mod_info.tag, "_", &args);

				//args.args[0]: base mod tag
				//args.args[1]: layer tag
				if(2 == args.args_num){
					for(i=0; i<LOG_RSV_ID_START; ++i){
						if(!strcmp(s_base_mod_info[i], args.args[0])){
							log_mod_id = i;
							break ;
						}
					}

					if(i == LOG_RSV_ID_START)
						goto rsv_log_tag;

					if(!strcmp(args.args[1], "CORE")){
						log_layer = LOG_LAYER_CORE;
					}else if(!strcmp(args.args[1], "HAL")){
						log_layer = LOG_LAYER_HAL;
					}else if(!strcmp(args.args[1], "MPP")){
						log_layer = LOG_LAYER_MPP;
					}else{
						printk("wrong layer:%s\n", args.args[1]);
						break;
					}
				}else{ // no "_", find in RSV tag

					if(!strcmp("all", log_mod_info.tag)){
						set_all_level(log_mod_info.level);
						break;
					}

rsv_log_tag:
					for(i=0; i<LOG_RSV_ID_START; ++i){
						if(!strcmp(s_base_mod_info[i], log_mod_info.tag)){
							log_mod_id = i;
							log_layer = -1;
							break ;
						}
					}

					for(i=0; i< LOG_RSV_NUM; ++i){
						if(!strcmp(s_base_mod_info[i+LOG_RSV_ID_START], log_mod_info.tag)){
							log_mod_id = i+LOG_RSV_ID_START;
							break ;
						}
					}
				}

			}

			if(log_mod_id > LOG_MOD_MAX_NUM-1 ){
				ret = -EFAULT;
				pr_err("bad log_mod_id= %d \n", log_mod_id);
				break;
			}

			if(!strlen(s_base_mod_info[log_mod_id])){
				ret = -EFAULT;
				pr_err("log_mod_id %d not register\n", log_mod_id);
				break;
			}

			if(log_mod_id >= LOG_RSV_ID_START) {
				s_log_level_rsv[log_mod_id - LOG_RSV_ID_START] = log_mod_info.level;
			} else if(LOG_LAYER_CORE == log_layer){
				s_log_level_core[log_mod_id] = log_mod_info.level;
			}else if(LOG_LAYER_HAL == log_layer){
				s_log_level_hal[log_mod_id] = log_mod_info.level;
			}else if(LOG_LAYER_MPP == log_layer){
				s_log_level_mpp[log_mod_id] = log_mod_info.level;
			}else if(-1 == log_layer){
				s_log_level_core[log_mod_id] = log_mod_info.level;
				s_log_level_hal[log_mod_id] = log_mod_info.level;
				s_log_level_mpp[log_mod_id] = log_mod_info.level;
			}

			break;
		}

		case IOC_SYS_REGISTER_LOG_TAG:{
			STRU_MOD_INFO log_mod_info;
			STRU_MOD_INFO *mod_info;

			if (copy_from_user(&log_mod_info, (struct STRU_MOD_INFO __user *)arg, sizeof(log_mod_info))) {
				ret = -EFAULT;
				pr_err("copy_from_user = %d \n", ret);
				break;
			}

			//printk(KERN_INFO "### %s %d %s %02x %d\n", __func__,  __LINE__, log_mod_info.tag, log_mod_info.id, log_mod_info.level);

			if(strlen(log_mod_info.tag)){
				if(-1 == log_mod_info.id){ //register base mod rsv id by tag
					int i;
					for(i=LOG_RSV_ID_START; i<LOG_MOD_MAX_NUM; ++i){
						if(strlen(s_base_mod_info[i]) > 0){
							if(!strcmp(s_base_mod_info[i], log_mod_info.tag)){
								log_mod_info.id = i;
								break;
							}
						}else if(0 == strlen(s_base_mod_info[i])){
							log_mod_info.id = i;
							strncpy(s_base_mod_info[i], log_mod_info.tag, LOG_TAG_MAX_LEN-1);
							s_base_mod_info[i][LOG_TAG_MAX_LEN-1] = 0;
							break;
							//s_log_level_rsv[i-LOG_RSV_ID_START] = log_mod_info.level;
						}
					}

					if(i == LOG_MOD_MAX_NUM) {
						ret = -EINVAL;
						pr_err("no mod info matched\n");
						break;
					}
				}else{
					int base_id = log_mod_info.id & LOG_MOD_ID_MASK;
					//int log_layer = log_mod_info.id  & LOG_LAYER_ID_MASK;
					if(-1 < base_id && base_id < LOG_MOD_MAX_NUM ){
						strncpy(s_base_mod_info[base_id], log_mod_info.tag, LOG_TAG_MAX_LEN-1);
						s_base_mod_info[base_id][LOG_TAG_MAX_LEN-1] = 0;
					}else{
						ret = -EINVAL;
						pr_err("err mod id %d \n", base_id);
						break;
					}
				}
			}else{
				ret = -EINVAL;
				pr_err("empty mod tag\n");
				break;
			}

			mod_info = (STRU_MOD_INFO __user *)arg;
			ret = copy_to_user(mod_info, &log_mod_info, sizeof(STRU_MOD_INFO));

			break;
		}

		case IOC_SYS_QUERY_LOG_TAG:{
			STRU_QUERY_INFO *query_info;
			int i = 0;
			int count = 0;
			int log_layer;

			memset(&s_log_query_info, 0, sizeof(STRU_QUERY_INFO));

			for(i=0; i<LOG_RSV_ID_START; ++i){
				for(log_layer=1; log_layer<4; ++log_layer)
				{
					if(strlen(s_base_mod_info[i])){
						if(LOG_LAYER_CORE>>16 == log_layer) {
							s_log_query_info.log_info[count].level = s_log_level_core[i];
							snprintf(s_log_query_info.log_info[count].tag, LOG_TAG_MAX_LEN-1, "%s%s", s_base_mod_info[i], s_layer_info[0]);
						} else if(LOG_LAYER_HAL>>16 == log_layer) {
							s_log_query_info.log_info[count].level = s_log_level_hal[i];
							snprintf(s_log_query_info.log_info[count].tag, LOG_TAG_MAX_LEN-1, "%s%s", s_base_mod_info[i], s_layer_info[1]);
						} else if(LOG_LAYER_MPP>>16 == log_layer) {
							s_log_query_info.log_info[count].level = s_log_level_mpp[i];
							snprintf(s_log_query_info.log_info[count].tag, LOG_TAG_MAX_LEN-1, "%s%s", s_base_mod_info[i], s_layer_info[2]);
						} else {
							snprintf(s_log_query_info.log_info[count].tag, LOG_TAG_MAX_LEN-1, "%s", s_base_mod_info[i]);
						}
						count++;
					}

				}
			}

			for(i=LOG_RSV_ID_START; i<LOG_MOD_MAX_NUM; ++i){
				if(strlen(s_base_mod_info[i])){
					strncpy(s_log_query_info.log_info[count].tag, s_base_mod_info[i], strlen(s_base_mod_info[i]));
					s_log_query_info.log_info[count].level = s_log_level_rsv[i - LOG_RSV_ID_START];
					count++;
				}
			}

			s_log_query_info.num = count;

			printk(KERN_INFO "### %s %d %d\n", __func__, __LINE__, s_log_query_info.num);

			query_info = (STRU_QUERY_INFO __user *)arg;
			ret = copy_to_user(query_info, &s_log_query_info, sizeof(STRU_QUERY_INFO));

			break;
		}

		case IOC_SYS_GET_MPP_SERVICE_TYPE: {
			int mpp_service_is_lib = 0;

			if(sizeof(mpp_service_is_lib) != _IOC_SIZE(cmd)) {
				printk(KERN_ERR "%s: length error.\n", __func__);
				return -EINVAL;
			}

			if(!strcmp(mpp_service_type, "lib"))
				mpp_service_is_lib = 1;

			ret = copy_to_user((void __user *)arg, &mpp_service_is_lib, _IOC_SIZE(cmd));
			if(ret)
				return -EFAULT;

			break;
		}

		default:
			ret = -1;
            break;
	}
	return ret;
}

static long ar_sys_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -1;

	mutex_lock(&g_sys_ioctl_lock);

	if (_IOC_TYPE(cmd) == 'y') {
		ret = ar_sys_ioctl_y(filp, cmd, arg);
	}else if(_IOC_TYPE(cmd) == 'p'){
		ret = ar_sys_ioctl_p(filp, cmd, arg);
	}

	mutex_unlock(&g_sys_ioctl_lock);
	return ret;
}


static struct file_operations ar_sys_fops =
{
	.owner		        = THIS_MODULE,
	.mmap	            = ar_sys_mmap,
	.unlocked_ioctl     = ar_sys_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl       = ar_sys_ioctl,
#endif
};

static struct miscdevice ar_sys =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ar_sys",
	.fops = &ar_sys_fops,
};

static int __init ar_sys_init(void)
{
	int ret = -1;

	ret = misc_register(&ar_sys);
	if(ret < 0) {
		goto out;
	}
	if(loglevel < 0 || loglevel > 5) {
		printk(KERN_INFO "bad loglevel %d\n", loglevel);
		loglevel = 2;
	}
	set_all_level(loglevel);

	memset(s_base_mod_info, 0, sizeof(s_base_mod_info));

	spin_lock_init(&g_sys_spin_lock);
	mutex_init(&g_sys_ioctl_lock);

out:
	printk(KERN_INFO "ar_sys_init %d\n", ret);
	return ret;
}

static void __exit ar_sys_exit(void)
{
	misc_deregister(&ar_sys);

	printk(KERN_INFO "ar_sys_exit\n");
}

module_init(ar_sys_init);
module_exit(ar_sys_exit);

MODULE_AUTHOR("Artosyn");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

