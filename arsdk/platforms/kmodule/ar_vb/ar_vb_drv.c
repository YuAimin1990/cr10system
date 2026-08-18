#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/sort.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>

#include "osal_mmz.h"
#include "ar_vb_drv.h"

#define ALIGN_TO(size, align)   (((size) + (align) -1L) & ~((align) -1L))
#define BLOCK_OFFSET            (64 * 1024)
#define ISP_BAYER_CHN           (4)
#define CFACOLORPLANE           (3)
#define DNG_NP_SIZE             (6)



//#pragma pack(push)
//#pragma pack(1)

typedef struct hiDNG_RATIONAL_S
{
    AR_U32 u32Numerator;/*represents the numerator of a fraction,*/
    AR_U32 u32Denominator;/* the denominator. */
} DNG_RATIONAL_S;

/*
Defines the structure of DNG image dynamic infomation
*/
typedef struct hiDNG_IMAGE_DYNAMIC_INFO_S
{
    AR_U32 au32BlackLevel[ISP_BAYER_CHN];           /* RO;Range: [0x0, 0xFFFF]; black level*/
    DNG_RATIONAL_S astAsShotNeutral[CFACOLORPLANE]; /* specifies the selected white balance at time of capture, encoded as the coordinates of a perfectly neutral color in linear reference space values.*/
    AR_DOUBLE adNoiseProfile[DNG_NP_SIZE];          /* RO;describes the amount of noise in a raw image*/
} DNG_IMAGE_DYNAMIC_INFO_S;


typedef struct
{
    unsigned int            block_id;
    unsigned int            ref_cnt;
    unsigned int            map_cnt;
    unsigned int            user[AR_VB_UID_BUTT];
    unsigned long long      block_size;
    unsigned long long      block_phy_addr;
    STRU_VIDEO_SUPPLEMENT_S supplement_info;
} STRU_AR_VB_BLOCK_INFO;

typedef struct
{
    unsigned int            pool_id;
    unsigned int            pool_version;
    unsigned int            is_comm_pool;
    unsigned int            is_valid;
    int                     pool_owner;
    unsigned int            free_blk_cnt;
    unsigned int            mini_free_blk_cnt;
    unsigned int            blk_cnt;
    unsigned long long      pre_blk_size;
    unsigned long long      pool_size;
    ENUM_VB_REMAP_MODE_E    e_remap_mode;
    char                    mmz_name[AR_MAX_MMZ_NAME_LEN + 1];
    hil_mmb_t               * mmb;
    hil_mmb_t               * mmb_sup_nc;
    hil_mmb_t               * mmb_sup_c;
    void AR_ATTRIBUTE       * p_pool_vir_addr;
    void AR_ATTRIBUTE       * p_sup_nc_vir_addr;
    void AR_ATTRIBUTE       * p_sup_c_vir_addr;
    STRU_AR_VB_BLOCK_INFO   * block;
    unsigned long long      * v2_pa_list;
} STRU_AR_VB_POOL_INFO;

typedef struct
{
    unsigned long long      u64_blk_size;
    unsigned int            index;
} STRU_AR_VB_LUT;

typedef struct
{
    unsigned int            is_inited;
    unsigned int            pool_cnt;

    unsigned int            is_cfged;
    STRU_VB_CONFIG_S        st_vb_pool_cfg;
    STRU_AR_VB_LUT          st_vb_lut[AR_VB_MAX_COMM_POOLS];
    unsigned int            st_vb_lut_num;

    unsigned int            is_mod_com_inited[AR_VB_UID_BUTT];
    unsigned int            is_mod_com_cfged[AR_VB_UID_BUTT];
    STRU_VB_CONFIG_S        st_vb_mod_com_pool_cfg[AR_VB_UID_BUTT];
    STRU_AR_VB_LUT          st_vb_mod_lut[AR_VB_UID_BUTT][AR_VB_MAX_COMM_POOLS];
    unsigned int            st_vb_mod_lut_num[AR_VB_UID_BUTT];

    STRU_AR_VB_POOL_INFO    pools[AR_VB_MAX_POOLS];

    unsigned int            is_supplement_cfged;
    unsigned int            supplement_blk_cnt;
    unsigned int            supplement_cache_size;
    unsigned int            supplement_noncache_size;
    STRU_VB_SUPPLEMENT_CONFIG_S     supplement_cfg;
} STRU_AR_VB_INFO;

typedef struct ar_vb_user_info {
    pid_t                   pid;
    pid_t                   mmap_pid;
    struct list_head        list;
}STRU_AR_VB_USER_INFO;

//#pragma pack(pop)


hil_mmb_t                   * g_mmb         = NULL;
STRU_AR_VB_INFO             * ar_vb_info    = NULL;
spinlock_t                  g_vb_spin_lock;
struct mutex                g_vb_ioctl_lock;
unsigned int                g_align         = 256;


unsigned int g_ioctl_cnt = 0;
unsigned int g_ioctl_sub_cnt[VB_USERSUB_PA + 1] = {0};

extern void *hil_mmb_map2kern_cached(hil_mmb_t *mmb);
extern void *hil_mmb_map2kern(hil_mmb_t *mmb);
extern hil_mmb_t *hil_mmb_alloc(const char *name, unsigned long size, unsigned long align,
                                unsigned long gfp, const char *mmz_name);
extern int hil_mmb_unmap(hil_mmb_t *mmb);
extern int hil_mmb_free(hil_mmb_t *mmb);

static void ar_vb_print_one_pool(STRU_AR_VB_POOL_INFO * pool_info);

int vb_pool_cmp(const void * a, const void * b)
{
    STRU_AR_VB_LUT * pool_a = (STRU_AR_VB_LUT *)a;
    STRU_AR_VB_LUT * pool_b = (STRU_AR_VB_LUT *)b;

    if(pool_a->u64_blk_size > pool_b->u64_blk_size)
        return 1;
    else if(pool_a->u64_blk_size < pool_b->u64_blk_size)
        return -1;
    else
        return 0;
}

void vb_pool_swap(void * a, void * b, int size)
{
    STRU_AR_VB_LUT * pool_a = (STRU_AR_VB_LUT *)a;
    STRU_AR_VB_LUT * pool_b = (STRU_AR_VB_LUT *)b;
    STRU_AR_VB_LUT pool_tmp = *pool_a ;
    *pool_a = *pool_b;
    *pool_b = pool_tmp;
}

/*****************************************************************************
 prototype    : vb_handle_to_pool_id
 Description  : convert handle to pool ID
 input        : vb_blk_handle   : handle of the buffer
 output       :
 return value : hi_u32
*****************************************************************************/
static unsigned int ar_vb_handle_to_pool_id(unsigned int block_id)
{
    return block_id / BLOCK_OFFSET;
}

static unsigned int ar_vb_handle_to_blk_idx(unsigned int block_id)
{
    return block_id - block_id / BLOCK_OFFSET * BLOCK_OFFSET;
}

static int ar_vb_phy_to_handle(unsigned long long phy_addr)
{
    int pool_id = 0;
    int ret = AR_VB_INVALID_HANDLE;
    for(pool_id = 0; pool_id < AR_VB_MAX_POOLS; ++pool_id) {
        if(0 == ar_vb_info->pools[pool_id].is_valid)
            continue;
        
        if(0 == ar_vb_info->pools[pool_id].pool_version) {
            if(phy_addr >= ar_vb_info->pools[pool_id].mmb->phys_addr &&
               phy_addr < ar_vb_info->pools[pool_id].mmb->phys_addr + ar_vb_info->pools[pool_id].pool_size) {
                ret = ((phy_addr - ar_vb_info->pools[pool_id].mmb->phys_addr) / ar_vb_info->pools[pool_id].pre_blk_size)
                      + BLOCK_OFFSET * pool_id;
                break;
            }
        } else {
            int i = 0;
            for(i = 0; i < ar_vb_info->pools[pool_id].blk_cnt; ++i) {
                if(phy_addr == ar_vb_info->pools[pool_id].v2_pa_list[i]) {
                    ret = i + BLOCK_OFFSET * pool_id;
                    break;
                }
            }
        }
    }
    return ret;
}

static unsigned long ar_vb_handle_to_phy(unsigned int block_id)
{
    int pool_id = ar_vb_handle_to_pool_id(block_id);
    int index = ar_vb_handle_to_blk_idx(block_id);

    return ar_vb_info->pools[pool_id].block[index].block_phy_addr;
}


static int ar_vb_block_is_used(unsigned int      block_id)
{
    unsigned int pool_id = ar_vb_handle_to_pool_id(block_id);
    if(NULL == ar_vb_info->pools[pool_id].block)
        return 0;

    return (!!ar_vb_info->pools[pool_id].block[block_id].ref_cnt);
}

static int ar_vb_pool_is_used(unsigned int      pool_id)
{
    int i = 0, ret = 0;
    if(NULL == ar_vb_info->pools[pool_id].block)
        return 0;

    for(i = 0; i < ar_vb_info->pools[pool_id].blk_cnt; ++i) {
        ret += ar_vb_info->pools[pool_id].block[i].ref_cnt;
        ret += ar_vb_info->pools[pool_id].block[i].map_cnt;
    }
    return !!ret;
}

static int ar_vb_inquire_blk_cnt(unsigned int block_id)
{
    unsigned int pool_id = ar_vb_handle_to_pool_id(block_id);
    int index = ar_vb_handle_to_blk_idx(block_id);

    return ar_vb_info->pools[pool_id].block[index].ref_cnt;
}

static STRU_VIDEO_SUPPLEMENT_S * ar_vb_handle_to_supplement(unsigned int block_id)
{
    unsigned int pool_id = ar_vb_handle_to_pool_id(block_id);
    int index = ar_vb_handle_to_blk_idx(block_id);

    if(!ar_vb_info->pools[pool_id].block[index].ref_cnt)
        return NULL;

    return &(ar_vb_info->pools[pool_id].block[index].supplement_info);
}

static STRU_AR_VB_BLOCK_INFO * ar_vb_block_cfg(unsigned int pool_id, unsigned int blk_cnt,
                                                   unsigned long long blk_size, unsigned long long pool_phy_addr,
                                                   STRU_VB_SUPPLEMENT_CONFIG_S supplement_info,
                                                   STRU_AR_VB_POOL_INFO * pool)
{
    STRU_AR_VB_BLOCK_INFO   * p_block_info  = NULL;
    unsigned int            i               = 0;

    p_block_info = (STRU_AR_VB_BLOCK_INFO *)kzalloc(blk_cnt * sizeof(STRU_AR_VB_BLOCK_INFO), GFP_KERNEL);
    if(NULL == p_block_info) {
        printk(KERN_ERR "ar_vb_block_cfg: kzalloc failed!\n");
        return NULL;
    }

    for(i = 0; i < blk_cnt; ++i) {
        p_block_info[i].block_id        = (BLOCK_OFFSET * pool_id + i);
        p_block_info[i].ref_cnt         = 0;
        p_block_info[i].block_size      = blk_size;
        p_block_info[i].block_phy_addr  = pool_phy_addr + i * blk_size;
        if(ar_vb_info->is_supplement_cfged) {
            unsigned int offset = ar_vb_info->supplement_noncache_size * i;
            ar_vb_info->supplement_blk_cnt++;
            p_block_info[i].supplement_info.u64_low_delay_phy_addr  = pool->mmb_sup_nc->phys_addr;
            p_block_info[i].supplement_info.p_low_delay_vir_addr    = pool->p_sup_nc_vir_addr;



            offset = ar_vb_info->supplement_cache_size * i;
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_JPEG_MASK) {
                p_block_info[i].supplement_info.u64_jpeg_DCF_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_jpeg_DCF_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += 0xcc;
            }
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_ISPINFO_MASK) {
                p_block_info[i].supplement_info.u64_isp_info_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_isp_info_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += 0x38;
            }
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_MOTION_DATA_MASK) {
                p_block_info[i].supplement_info.u64_motion_data_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_motion_data_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += 0xf0;
            }
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_DNG_MASK) {
                p_block_info[i].supplement_info.u64_frame_DNG_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_frame_DNG_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += sizeof(DNG_IMAGE_DYNAMIC_INFO_S);
            }
        }
    }

    return p_block_info;
}

static STRU_AR_VB_BLOCK_INFO * ar_vb_block_cfg_v2(unsigned int pool_id, unsigned int blk_cnt,
                                                   unsigned long long blk_size, unsigned long long * pool_phy_addr,
                                                   STRU_VB_SUPPLEMENT_CONFIG_S supplement_info,
                                                   STRU_AR_VB_POOL_INFO * pool)
{
    STRU_AR_VB_BLOCK_INFO   * p_block_info  = NULL;
    unsigned int            i               = 0;

    p_block_info = (STRU_AR_VB_BLOCK_INFO *)kzalloc(blk_cnt * sizeof(STRU_AR_VB_BLOCK_INFO), GFP_KERNEL);
    if(NULL == p_block_info) {
        printk(KERN_ERR "ar_vb_block_cfg_v2: kzalloc failed!\n");
        return NULL;
    }

    for(i = 0; i < blk_cnt; ++i) {
        p_block_info[i].block_id        = (BLOCK_OFFSET * pool_id + i);
        p_block_info[i].ref_cnt         = 0;
        p_block_info[i].block_size      = blk_size;
        p_block_info[i].block_phy_addr  = pool_phy_addr[i];
        if(ar_vb_info->is_supplement_cfged) {
            unsigned int offset = ar_vb_info->supplement_noncache_size * i;
            ar_vb_info->supplement_blk_cnt++;
            p_block_info[i].supplement_info.u64_low_delay_phy_addr  = pool->mmb_sup_nc->phys_addr;
            p_block_info[i].supplement_info.p_low_delay_vir_addr    = pool->p_sup_nc_vir_addr;



            offset = ar_vb_info->supplement_cache_size * i;
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_JPEG_MASK) {
                p_block_info[i].supplement_info.u64_jpeg_DCF_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_jpeg_DCF_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += 0xcc;
            }
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_ISPINFO_MASK) {
                p_block_info[i].supplement_info.u64_isp_info_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_isp_info_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += 0x38;
            }
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_MOTION_DATA_MASK) {
                p_block_info[i].supplement_info.u64_motion_data_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_motion_data_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += 0xf0;
            }
            if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_DNG_MASK) {
                p_block_info[i].supplement_info.u64_frame_DNG_phy_addr   =
                            pool->mmb_sup_c->phys_addr + offset;
                p_block_info[i].supplement_info.p_frame_DNG_vir_addr     =
                            pool->p_sup_c_vir_addr + offset;
                offset += sizeof(DNG_IMAGE_DYNAMIC_INFO_S);
            }
        }
    }

    return p_block_info;
}


/*****************************************************************************
 prototype    : vb_create_pool
 Description  : create a private VB pool
 input        : blk_cnt      : count of buffer blocks
                blk_cnt      : size of buffer blocks
 output       : pool_id     : pointer to the ID of pool created
 return value : hi_s32         : 0 -- success; !0 : failure
*****************************************************************************/
static int ar_vb_create_pool(unsigned int pool_id, unsigned int blk_cnt, unsigned long long blk_size,
                                  char * p_mmz_name, char * p_mmb_name, ENUM_VB_REMAP_MODE_E e_remap_mode,
                                  int pool_owner)
{
    int ret = 0;
    int supplement_cache_size = 0;
    int supplement_noncache_size = 0;

    ar_vb_info->pools[pool_id].pool_id        = pool_id;
    ar_vb_info->pools[pool_id].is_comm_pool   = (AR_POOL_OWNER_PRIVATE == pool_owner) ? 0 : 1;
    ar_vb_info->pools[pool_id].pool_owner     = pool_owner;
    ar_vb_info->pools[pool_id].free_blk_cnt   = blk_cnt;
    ar_vb_info->pools[pool_id].mini_free_blk_cnt   = blk_cnt;
    ar_vb_info->pools[pool_id].blk_cnt        = blk_cnt;
    ar_vb_info->pools[pool_id].pre_blk_size   = ALIGN_TO(blk_size, g_align);
    ar_vb_info->pools[pool_id].pool_size      = ar_vb_info->pools[pool_id].blk_cnt * ar_vb_info->pools[pool_id].pre_blk_size;
    ar_vb_info->pools[pool_id].e_remap_mode    = e_remap_mode;
    ar_vb_info->pools[pool_id].mmb            = hil_mmb_alloc(p_mmb_name,
                                                              ar_vb_info->pools[pool_id].pool_size,
                                                              g_align,
                                                              0,
                                                              p_mmz_name);

    if(!ar_vb_info->pools[pool_id].mmb) {
        printk(KERN_ERR "hil_mmb_alloc failed!\n");
        ret = -ENOMEM;
        goto FAIL_0;
    }
    printk(KERN_INFO "hil_mmb_alloc pa:0x%lx len:%ld!\n", ar_vb_info->pools[pool_id].mmb->phys_addr,
                                                         ar_vb_info->pools[pool_id].mmb->length);

    strncpy(ar_vb_info->pools[pool_id].mmz_name, p_mmz_name, AR_MAX_MMZ_NAME_LEN);

    if(AR_VB_REMAP_MODE_NOCACHE == e_remap_mode) {
        ar_vb_info->pools[pool_id].p_pool_vir_addr = hil_mmb_map2kern(ar_vb_info->pools[pool_id].mmb);
        if(ar_vb_info->pools[pool_id].p_pool_vir_addr) {
            printk(KERN_ERR "hil_mmb_map2kern failed!\n");
            ret = -EINVAL;
            goto FAIL_1;
        }
    }
    else if(AR_VB_REMAP_MODE_CACHED == e_remap_mode) {
        ar_vb_info->pools[pool_id].p_pool_vir_addr = hil_mmb_map2kern_cached(ar_vb_info->pools[pool_id].mmb);
        if(ar_vb_info->pools[pool_id].p_pool_vir_addr) {
            printk(KERN_ERR "hil_mmb_map2kern_cached failed!\n");
            ret = -EINVAL;
            goto FAIL_1;
        }
    }
    else if(AR_VB_REMAP_MODE_BUTT <= e_remap_mode) {
        printk(KERN_ERR "e_remap_mode val out of range! e_remapMode:%d.\n",
                        e_remap_mode);
        ret = -EINVAL;
        goto FAIL_1;
    }
    else {
        ar_vb_info->pools[pool_id].p_pool_vir_addr = NULL;
    }

    //cfg supplement
    if(ar_vb_info->is_supplement_cfged) {
        char sup_name[16];

        //alloc noncache supplement
        supplement_noncache_size += 256;
        if(supplement_noncache_size) {
            snprintf(sup_name, 16, "%s_sup_nc", p_mmb_name);
            ar_vb_info->supplement_noncache_size = supplement_noncache_size;
            ar_vb_info->pools[pool_id].mmb_sup_nc = hil_mmb_alloc(sup_name,
                                                                  supplement_noncache_size * blk_cnt,
                                                                  g_align,
                                                                  0,
                                                                  p_mmz_name);
            if(!ar_vb_info->pools[pool_id].mmb_sup_nc) {
                printk(KERN_ERR "hil_mmb_alloc failed!\n");
                ret = -ENOMEM;
                goto FAIL_2;
            }

            ar_vb_info->pools[pool_id].p_sup_nc_vir_addr = hil_mmb_map2kern(ar_vb_info->pools[pool_id].mmb_sup_nc);
            if(!ar_vb_info->pools[pool_id].p_sup_nc_vir_addr) {
                printk(KERN_ERR "hil_mmb_map2kern failed!\n");
                ret = -EINVAL;
                goto FAIL_3;
            }
        }

        //alloc cache supplement
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_JPEG_MASK)
            supplement_cache_size += 0xcc;
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_ISPINFO_MASK)
            supplement_cache_size += 0x38;
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_MOTION_DATA_MASK)
            supplement_cache_size += 0xf0;
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_DNG_MASK)
            supplement_cache_size += sizeof(DNG_IMAGE_DYNAMIC_INFO_S);

        supplement_cache_size = ALIGN_TO(supplement_cache_size, g_align);
        if(supplement_cache_size) {
            snprintf(sup_name, 16, "%s_sup_c", p_mmb_name);
            ar_vb_info->supplement_cache_size = supplement_cache_size;
            ar_vb_info->pools[pool_id].mmb_sup_c = hil_mmb_alloc(sup_name,
                                                                 supplement_cache_size * blk_cnt,
                                                                 g_align,
                                                                 0,
                                                                 p_mmz_name);
            if(!ar_vb_info->pools[pool_id].mmb_sup_c) {
                printk(KERN_ERR "2 hil_mmb_alloc failed!\n");
                ret = -ENOMEM;
                goto FAIL_4;
            }

            ar_vb_info->pools[pool_id].p_sup_c_vir_addr = hil_mmb_map2kern_cached(ar_vb_info->pools[pool_id].mmb_sup_c);
            if(!ar_vb_info->pools[pool_id].p_sup_c_vir_addr) {
                printk(KERN_ERR "2 hil_mmb_map2kern failed!\n");
                ret = -EINVAL;
                goto FAIL_5;
            }
        }
    }

    //cfg block
    ar_vb_info->pools[pool_id].block = ar_vb_block_cfg(ar_vb_info->pools[pool_id].pool_id,
                                                       ar_vb_info->pools[pool_id].blk_cnt,
                                                       ar_vb_info->pools[pool_id].pre_blk_size,
                                                       ar_vb_info->pools[pool_id].mmb->phys_addr,
                                                       ar_vb_info->supplement_cfg,
                                                       &ar_vb_info->pools[pool_id]);
    if(NULL == ar_vb_info->pools[pool_id].block) {
        ret = -EINVAL;
        goto FAIL_6;
    }

    ar_vb_info->pools[pool_id].is_valid       = 1;
    (ar_vb_info->pool_cnt)++;

    return ret;

FAIL_6:
    hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_c);

FAIL_5:
    hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_c);

FAIL_4:
    hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_nc);

FAIL_3:
    hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_nc);

FAIL_2:
    hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb);

FAIL_1:
    hil_mmb_free(ar_vb_info->pools[pool_id].mmb);
    memset(&ar_vb_info->pools[pool_id], 0, sizeof(ar_vb_info->pools[pool_id]));

FAIL_0:
    return ret;
}

static int ar_vb_destory_pool(unsigned int pool_id)
{
    int ret = 0;
    if(!ar_vb_info->pools[pool_id].is_valid)
        return HAL_ERR_VB_UNEXIST;

    if(ar_vb_pool_is_used(pool_id))
        return HAL_ERR_VB_BUSY;

    //if(POOL_OWNER_PRIVATE != ar_vb_info->pools[pool_id].pool_owner)
    //    return HAL_ERR_VB_NOT_PERM;

    if(NULL != ar_vb_info->pools[pool_id].block) {
        kfree(ar_vb_info->pools[pool_id].block);
        ar_vb_info->pools[pool_id].block = NULL;
        //ar_vb_info->pools[pool_id].blk_cnt = 0;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_c &&
       NULL != ar_vb_info->pools[pool_id].p_sup_c_vir_addr) {
        hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_c);
        ar_vb_info->pools[pool_id].p_sup_c_vir_addr = NULL;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_c) {
        hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_c);
        ar_vb_info->pools[pool_id].mmb_sup_c = NULL;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_nc &&
       NULL != ar_vb_info->pools[pool_id].p_sup_nc_vir_addr) {
        hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_nc);
        ar_vb_info->pools[pool_id].p_sup_nc_vir_addr = NULL;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_nc) {
        hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_nc);
        ar_vb_info->pools[pool_id].mmb_sup_nc = NULL;
    }

    if(AR_VB_REMAP_MODE_NOCACHE == ar_vb_info->pools[pool_id].e_remap_mode ||
       AR_VB_REMAP_MODE_CACHED == ar_vb_info->pools[pool_id].e_remap_mode) {
        ret = hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb);
        if(0 != ret) {
            return HAL_ERR_VB_BUSY;
        }
    }

    if(ar_vb_info->is_supplement_cfged)
        ar_vb_info->supplement_blk_cnt -= ar_vb_info->pools[pool_id].blk_cnt;

    if(NULL != ar_vb_info->pools[pool_id].mmb) {
        ret = hil_mmb_free(ar_vb_info->pools[pool_id].mmb);
        if(0 != ret) {
            return HAL_ERR_VB_BUSY;
        }
    }

    if(ar_vb_info->pools[pool_id].is_comm_pool && ar_vb_info->st_vb_lut_num) {
        int i = 0;
        for(i = 0; i < ar_vb_info->st_vb_lut_num; ++i) {
            if(ar_vb_info->st_vb_lut[i].index == pool_id)
                break;
        }

        if(i != ar_vb_info->st_vb_lut_num){
            ar_vb_info->st_vb_lut_num--;
            memcpy(&ar_vb_info->st_vb_lut[i], &ar_vb_info->st_vb_lut[i + 1], ar_vb_info->st_vb_lut_num - i);
        }
    }

    if(0 == ar_vb_info->pools[pool_id].is_comm_pool && 
       ar_vb_info->pools[pool_id].pool_owner != AR_POOL_OWNER_PRIVATE && 
       ar_vb_info->st_vb_mod_lut_num[ar_vb_info->pools[pool_id].pool_owner]) {
        int i = 0;
        int uid = ar_vb_info->pools[pool_id].pool_owner;
        for(i = 0; i < ar_vb_info->st_vb_mod_lut_num[uid]; ++i) {
            if(ar_vb_info->st_vb_mod_lut[uid][i].index == pool_id)
                break;
        }

        if(i != ar_vb_info->st_vb_mod_lut_num[uid]){
            ar_vb_info->st_vb_mod_lut_num[uid]--;
            memcpy(&ar_vb_info->st_vb_mod_lut[uid][i], 
                   &ar_vb_info->st_vb_mod_lut[uid][i + 1], 
                   ar_vb_info->st_vb_mod_lut_num[uid] - i);
        }
    }

    memset(&ar_vb_info->pools[pool_id], 0, sizeof(ar_vb_info->pools[pool_id]));

    (ar_vb_info->pool_cnt)--;

    return ret;
}

/*****************************************************************************
 prototype    : vb_create_pool
 Description  : create a private VB pool
 input        : blk_cnt      : count of buffer blocks
                blk_cnt      : size of buffer blocks
 output       : pool_id     : pointer to the ID of pool created
 return value : hi_s32         : 0 -- success; !0 : failure
*****************************************************************************/
static int ar_vb_create_pool_v2(unsigned int pool_id, unsigned int blk_cnt, unsigned long long blk_size,
                                  char * p_mmz_name, char * p_mmb_name, ENUM_VB_REMAP_MODE_E e_remap_mode,
                                  int pool_owner, unsigned long long * pa_list)
{
    int ret = 0;
    int supplement_cache_size = 0;
    int supplement_noncache_size = 0;

    ar_vb_info->pools[pool_id].pool_id        = pool_id;
    ar_vb_info->pools[pool_id].is_comm_pool   = (AR_POOL_OWNER_PRIVATE == pool_owner) ? 0 : 1;
    ar_vb_info->pools[pool_id].pool_owner     = pool_owner;
    ar_vb_info->pools[pool_id].free_blk_cnt   = blk_cnt;
    ar_vb_info->pools[pool_id].mini_free_blk_cnt   = blk_cnt;
    ar_vb_info->pools[pool_id].blk_cnt        = blk_cnt;
    ar_vb_info->pools[pool_id].pre_blk_size   = ALIGN_TO(blk_size, g_align);
    ar_vb_info->pools[pool_id].pool_size      = ar_vb_info->pools[pool_id].blk_cnt * ar_vb_info->pools[pool_id].pre_blk_size;
    ar_vb_info->pools[pool_id].e_remap_mode    = e_remap_mode;
    ar_vb_info->pools[pool_id].v2_pa_list     = (unsigned long long *)vmalloc(blk_cnt * sizeof(unsigned long long));
    ar_vb_info->pools[pool_id].pool_version   = 1;
    
    if(NULL == ar_vb_info->pools[pool_id].v2_pa_list || NULL == pa_list)
    {
        printk(KERN_ERR "ar_vb_info->pools[pool_id].v2_pa_list or pa_list is NULL!\n");
        ret = -ENOMEM;
        goto FAIL_1;
    }

    memcpy(ar_vb_info->pools[pool_id].v2_pa_list, pa_list, blk_cnt * sizeof(unsigned long long));

    //cfg supplement
    if(ar_vb_info->is_supplement_cfged) {
        char sup_name[16];

        //alloc noncache supplement
        supplement_noncache_size += 256;
        if(supplement_noncache_size) {
            snprintf(sup_name, 16, "%s_sup_nc", p_mmb_name);
            ar_vb_info->supplement_noncache_size = supplement_noncache_size;
            ar_vb_info->pools[pool_id].mmb_sup_nc = hil_mmb_alloc(sup_name,
                                                                  supplement_noncache_size * blk_cnt,
                                                                  g_align,
                                                                  0,
                                                                  p_mmz_name);
            if(!ar_vb_info->pools[pool_id].mmb_sup_nc) {
                printk(KERN_ERR "hil_mmb_alloc failed!\n");
                ret = -ENOMEM;
                goto FAIL_2;
            }

            ar_vb_info->pools[pool_id].p_sup_nc_vir_addr = hil_mmb_map2kern(ar_vb_info->pools[pool_id].mmb_sup_nc);
            if(!ar_vb_info->pools[pool_id].p_sup_nc_vir_addr) {
                printk(KERN_ERR "hil_mmb_map2kern failed!\n");
                ret = -EINVAL;
                goto FAIL_3;
            }
        }

        //alloc cache supplement
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_JPEG_MASK)
            supplement_cache_size += 0xcc;
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_ISPINFO_MASK)
            supplement_cache_size += 0x38;
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_MOTION_DATA_MASK)
            supplement_cache_size += 0xf0;
        if(ar_vb_info->supplement_cfg.u32_supplement_config & AR_VB_SUPPLEMENT_DNG_MASK)
            supplement_cache_size += sizeof(DNG_IMAGE_DYNAMIC_INFO_S);

        supplement_cache_size = ALIGN_TO(supplement_cache_size, g_align);
        if(supplement_cache_size) {
            snprintf(sup_name, 16, "%s_sup_c", p_mmb_name);
            ar_vb_info->supplement_cache_size = supplement_cache_size;
            ar_vb_info->pools[pool_id].mmb_sup_c = hil_mmb_alloc(sup_name,
                                                                 supplement_cache_size * blk_cnt,
                                                                 g_align,
                                                                 0,
                                                                 p_mmz_name);
            if(!ar_vb_info->pools[pool_id].mmb_sup_c) {
                printk(KERN_ERR "2 hil_mmb_alloc failed!\n");
                ret = -ENOMEM;
                goto FAIL_4;
            }

            ar_vb_info->pools[pool_id].p_sup_c_vir_addr = hil_mmb_map2kern_cached(ar_vb_info->pools[pool_id].mmb_sup_c);
            if(!ar_vb_info->pools[pool_id].p_sup_c_vir_addr) {
                printk(KERN_ERR "2 hil_mmb_map2kern failed!\n");
                ret = -EINVAL;
                goto FAIL_5;
            }
        }
    }

    //cfg block
    ar_vb_info->pools[pool_id].block = ar_vb_block_cfg_v2(ar_vb_info->pools[pool_id].pool_id,
                                                       ar_vb_info->pools[pool_id].blk_cnt,
                                                       ar_vb_info->pools[pool_id].pre_blk_size,
                                                       ar_vb_info->pools[pool_id].v2_pa_list,
                                                       ar_vb_info->supplement_cfg,
                                                       &ar_vb_info->pools[pool_id]);
    if(NULL == ar_vb_info->pools[pool_id].block) {
        ret = -EINVAL;
        goto FAIL_6;
    }

    ar_vb_info->pools[pool_id].is_valid       = 1;
    (ar_vb_info->pool_cnt)++;

    return ret;

FAIL_6:
    hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_c);

FAIL_5:
    hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_c);

FAIL_4:
    hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_nc);

FAIL_3:
    hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_nc);

FAIL_2:
    vfree(ar_vb_info->pools[pool_id].v2_pa_list);

FAIL_1:
    memset(&ar_vb_info->pools[pool_id], 0, sizeof(ar_vb_info->pools[pool_id]));

//FAIL_0:
    return ret;
}

static int ar_vb_destory_pool_v2(unsigned int pool_id)
{
    int ret = 0;
    if(!ar_vb_info->pools[pool_id].is_valid)
        return HAL_ERR_VB_UNEXIST;

    if(1 != ar_vb_info->pools[pool_id].pool_version)
        return HAL_ERR_VB_UNEXIST;

    if(ar_vb_pool_is_used(pool_id))
        return HAL_ERR_VB_BUSY;

    //if(POOL_OWNER_PRIVATE != ar_vb_info->pools[pool_id].pool_owner)
    //    return HAL_ERR_VB_NOT_PERM;

    if(NULL != ar_vb_info->pools[pool_id].block) {
        kfree(ar_vb_info->pools[pool_id].block);
        ar_vb_info->pools[pool_id].block = NULL;
        //ar_vb_info->pools[pool_id].blk_cnt = 0;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_c &&
       NULL != ar_vb_info->pools[pool_id].p_sup_c_vir_addr) {
        hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_c);
        ar_vb_info->pools[pool_id].p_sup_c_vir_addr = NULL;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_c) {
        hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_c);
        ar_vb_info->pools[pool_id].mmb_sup_c = NULL;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_nc &&
       NULL != ar_vb_info->pools[pool_id].p_sup_nc_vir_addr) {
        hil_mmb_unmap(ar_vb_info->pools[pool_id].mmb_sup_nc);
        ar_vb_info->pools[pool_id].p_sup_nc_vir_addr = NULL;
    }

    if(NULL != ar_vb_info->pools[pool_id].mmb_sup_nc) {
        hil_mmb_free(ar_vb_info->pools[pool_id].mmb_sup_nc);
        ar_vb_info->pools[pool_id].mmb_sup_nc = NULL;
    }

    if(ar_vb_info->is_supplement_cfged)
        ar_vb_info->supplement_blk_cnt -= ar_vb_info->pools[pool_id].blk_cnt;

    if(NULL != ar_vb_info->pools[pool_id].v2_pa_list) {
        vfree(ar_vb_info->pools[pool_id].v2_pa_list);
    }

    memset(&ar_vb_info->pools[pool_id], 0, sizeof(ar_vb_info->pools[pool_id]));

    (ar_vb_info->pool_cnt)--;

    return ret;
}

static int ar_vb_get_blk(AR_VB_POOL pool_id, unsigned int index, ENMU_VB_UID_E uid)
{
    ar_vb_info->pools[pool_id].block[index].ref_cnt++;
    ar_vb_info->pools[pool_id].block[index].user[uid]++;
    ar_vb_info->pools[pool_id].free_blk_cnt--;

    if(ar_vb_info->pools[pool_id].free_blk_cnt < ar_vb_info->pools[pool_id].mini_free_blk_cnt)
        ar_vb_info->pools[pool_id].mini_free_blk_cnt = ar_vb_info->pools[pool_id].free_blk_cnt;

    return ar_vb_info->pools[pool_id].block[index].block_id;
}

static int ar_vb_release_blk(AR_VB_POOL pool_id, unsigned int index, ENMU_VB_UID_E uid)
{
    if(!ar_vb_info->pools[pool_id].block[index].ref_cnt ||
       !ar_vb_info->pools[pool_id].block[index].user[uid])
        return HAL_ERR_VB_UNEXIST;

    ar_vb_info->pools[pool_id].block[index].ref_cnt--;
    ar_vb_info->pools[pool_id].block[index].user[uid]--;
    if(0 == ar_vb_info->pools[pool_id].block[index].ref_cnt)
        ar_vb_info->pools[pool_id].free_blk_cnt++;
    return AR_SUCCESS;
}

static int ar_vb_user_add(AR_VB_POOL pool_id, unsigned int index, ENMU_VB_UID_E uid)
{
    if(0 == ar_vb_info->pools[pool_id].block[index].ref_cnt)
    {
         ar_vb_print_one_pool(&ar_vb_info->pools[pool_id]);
        return HAL_ERR_VB_UNEXIST;
    }

    ar_vb_info->pools[pool_id].block[index].ref_cnt++;
    ar_vb_info->pools[pool_id].block[index].user[uid]++;
    return AR_SUCCESS;
}

static int ar_vb_user_sub(AR_VB_POOL pool_id, unsigned int index, ENMU_VB_UID_E uid)
{
    if(0 == ar_vb_info->pools[pool_id].block[index].ref_cnt)
        return HAL_ERR_VB_UNEXIST;

    if(1 == ar_vb_info->pools[pool_id].block[index].ref_cnt)
        return ar_vb_release_blk(pool_id, index, uid);

    if(!ar_vb_info->pools[pool_id].block[index].user[uid])
        return HAL_ERR_VB_UNEXIST;

    ar_vb_info->pools[pool_id].block[index].ref_cnt--;
    ar_vb_info->pools[pool_id].block[index].user[uid]--;
    if(0 == ar_vb_info->pools[pool_id].block[index].ref_cnt)
        ar_vb_info->pools[pool_id].free_blk_cnt++;
    return AR_SUCCESS;
}


/*****************************************************************************
 prototype    : vb_get_blk_by_pool_id
 Description  : get a buffer block from a specified pool
 input        : pool_id      : ID of a pool
                uid         : ID of user getting the buffer
 output       :
 return value : vb_blk_handle   :
                success : not VB_INVALID_HANDLE
                failure : VB_INVALID_HANDLE
*****************************************************************************/
static AR_VB_BLK ar_vb_get_blk_by_pool_id(AR_VB_POOL pool_id, ENMU_VB_UID_E uid)
{
    int i = 0;

    if(!ar_vb_info->pools[pool_id].is_valid)
        return AR_VB_INVALID_HANDLE;

    if(ar_vb_info->pools[pool_id].pool_owner > 0 &&
       ar_vb_info->pools[pool_id].pool_owner != uid)
        return AR_VB_INVALID_HANDLE;

    if(!ar_vb_info->pools[pool_id].free_blk_cnt)
        return AR_VB_INVALID_HANDLE;

    for(i = 0; i < ar_vb_info->pools[pool_id].blk_cnt; ++i) {
        if(!ar_vb_info->pools[pool_id].block[i].ref_cnt)
            break;
    }

    return ar_vb_get_blk(pool_id, i, uid);
}

/*****************************************************************************
 prototype    : vb_get_blk_by_size
 Description  : get a block in the first fit common pool
 input        : blk_size : size of buffer block
                uid     : ID of user who will use the buffer
 output       : NONE
 return value : vb_blk_handle
                success : not VB_INVALID_HANDLE
                failure : VB_INVALID_HANDLE
*****************************************************************************/
static AR_VB_BLK ar_vb_get_blk_by_size(AR_U64 blk_size, ENMU_VB_UID_E uid, AR_CHAR *pc_mmz_name)
{
    AR_VB_POOL pool_id     = 0;
    AR_VB_POOL pool_id_true   = 0;
    AR_VB_BLK  block_id    = AR_VB_INVALID_HANDLE;
    int     i           = 0;

    for(pool_id = 0; pool_id < ar_vb_info->st_vb_lut_num; ++pool_id) {
        pool_id_true = ar_vb_info->st_vb_lut[pool_id].index;
        if(ar_vb_info->pools[pool_id_true].is_valid &&
            AR_POOL_OWNER_COMMON == ar_vb_info->pools[pool_id_true].pool_owner &&
            blk_size <= ar_vb_info->pools[pool_id_true].pre_blk_size &&
            0 == strncmp(pc_mmz_name, ar_vb_info->pools[pool_id_true].mmz_name, AR_MAX_MMZ_NAME_LEN)) {
            if(!ar_vb_info->pools[pool_id_true].free_blk_cnt)
                continue;

            for(i = 0; i < ar_vb_info->pools[pool_id_true].blk_cnt; ++i) {
                if(!ar_vb_info->pools[pool_id_true].block[i].ref_cnt)
                    break;
            }

            block_id = ar_vb_get_blk(pool_id_true, i, uid);
            break;
        }
    }

    return block_id;
}

/*****************************************************************************
 prototype    : vb_get_blk_by_size_and_module
 Description  : get a block in the module common pool
 input        : blk_size : size of buffer block
                uid     : ID of user who build the buffer
 output       : NONE
 return value : vb_blk_handle
                success : not VB_INVALID_HANDLE
                failure : VB_INVALID_HANDLE
*****************************************************************************/
static AR_VB_BLK ar_vb_get_blk_by_size_and_module(AR_U64 blk_size, ENMU_VB_UID_E uid, AR_CHAR *pc_mmz_name)
{
    AR_VB_POOL pool_id     = 0;
    AR_VB_POOL pool_id_true   = 0;
    AR_VB_BLK  block_id    = AR_VB_INVALID_HANDLE;
    int     i           = 0;

    for(pool_id = 0; pool_id < ar_vb_info->st_vb_mod_lut_num[uid]; ++pool_id) {
        pool_id_true = ar_vb_info->st_vb_mod_lut[uid][pool_id].index;
        if(ar_vb_info->pools[pool_id_true].is_valid &&
            uid == ar_vb_info->pools[pool_id_true].pool_owner &&
            blk_size <= ar_vb_info->pools[pool_id_true].pre_blk_size &&
            0 == strncmp(pc_mmz_name, ar_vb_info->pools[pool_id_true].mmz_name, AR_MAX_MMZ_NAME_LEN)) {

            if(!ar_vb_info->pools[pool_id_true].free_blk_cnt)
                continue;

            for(i = 0; i < ar_vb_info->pools[pool_id_true].blk_cnt; ++i) {
                if(!ar_vb_info->pools[pool_id_true].block[i].ref_cnt)
                    break;
            }

            block_id = ar_vb_get_blk(pool_id_true, i, uid);
            break;
        }
    }

    return block_id;

}

/*****************************************************************************
 prototype    : vb_get_blk_by_size_and_pool_id
 Description  : get a block in the pool with id
 input        : blk_size : size of buffer block
                pool_id  : ID of block
                uid     : ID of user who build the buffer
 output       : NONE
 return value : vb_blk_handle
                success : not VB_INVALID_HANDLE
                failure : VB_INVALID_HANDLE
*****************************************************************************/
static AR_VB_BLK ar_vb_get_blk_by_size_and_pool_id(AR_VB_POOL pool_id, AR_U64 blk_size, ENMU_VB_UID_E uid)
{
    int i = 0;

    if(!ar_vb_info->pools[pool_id].is_valid)
        return AR_VB_INVALID_HANDLE;

    if(ar_vb_info->pools[pool_id].pool_owner > 0 &&
       ar_vb_info->pools[pool_id].pool_owner != uid)
        return AR_VB_INVALID_HANDLE;

    if(blk_size > ar_vb_info->pools[pool_id].pre_blk_size)
        return AR_VB_INVALID_HANDLE;

    if(!ar_vb_info->pools[pool_id].free_blk_cnt)
        return AR_VB_INVALID_HANDLE;

    for(i = 0; i < ar_vb_info->pools[pool_id].blk_cnt; ++i) {
        if(!ar_vb_info->pools[pool_id].block[i].ref_cnt)
            break;
    }

    return ar_vb_get_blk(pool_id, i, uid);
}


static long ar_vb_ioctl_t(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(_IOC_NR(cmd))
    {
        case 0:
            printk(KERN_INFO "%s %x malloc\n", __func__, _IOC_NR(cmd));
            g_mmb = hil_mmb_alloc("test", 0x1000, 8, 0, "");
            break;

        case 1:
            printk(KERN_INFO "%s %x free\n", __func__, _IOC_NR(cmd));
            hil_mmb_free(g_mmb);
            break;

        default:
            break;
    }
    return 0;
}

static long ar_vb_ioctl_b(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret         = 0;
    int i           = 0;
    int j           = 0;
    AR_U32 pool_id  = 0;
    AR_U32 block_id  = 0;
    AR_U64 phy_addr = 0;
    STRU_VB_CONFIG_S        tmp_vb_config;
    STRU_VB_POOL_CONFIG_S   tmp_pool_config;
    STRU_VB_POOL_CONFIG_V2_S   tmp_pool_config_v2;
    STRU_AR_VB_MSG          tmp_vb_msg = {0};
    char                    mmz_name[16] = "";
    ENMU_VB_UID_E           tmp_uid;
    ENMU_VB_UID_E           tmp_block;
    STRU_VIDEO_SUPPLEMENT_S *tmp_supplement;
    //STRU_VB_SUPPLEMENT_CONFIG_S *tmp_supplement_cfg;
    STRU_AR_VB_GET_POOL_INFO tmp_pool_info = {0};
    AR_U64 * phy_addr_list = NULL;

    ++g_ioctl_sub_cnt[_IOC_NR(cmd)];
    switch(_IOC_NR(cmd))
    {
        case VB_SETCONF:
            //printk(KERN_INFO "%s %x malloc\n", __func__, _IOC_NR(cmd));
            if(ar_vb_info->is_inited) {
                ret = -EINVAL;
                break;
            }

            if(_IOC_SIZE(cmd) != sizeof(STRU_VB_CONFIG_S)) {
                printk(KERN_ERR "VB_SETCONF: struct length error(%d %ld).\n", _IOC_SIZE(cmd), sizeof(STRU_VB_CONFIG_S));
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_config, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_SETCONF: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            for(i = 0; i < AR_VB_MAX_COMM_POOLS; ++i) {
                if(tmp_vb_config.st_comm_pool[i].u32_blk_cnt != 0 && tmp_vb_config.st_comm_pool[i].u64_blk_size != 0) {
                    if(tmp_vb_config.st_comm_pool[i].u32_blk_cnt > BLOCK_OFFSET) {
                        printk(KERN_ERR "VB_SETCONF: u32_blk_cnt too big.\n");
                        ret = -EINVAL;
                        break;
                    }

                    if(AR_MAX_MMZ_NAME_LEN == strnlen(tmp_vb_config.st_comm_pool[i].str_mmz_name, AR_MAX_MMZ_NAME_LEN)) {
                        printk(KERN_ERR "VB_SETCONF: str_mmz_name name too long.\n");
                        ret = -EINVAL;
                        break;
                    }
                }
            }

            if(0 == ret) {
                memcpy(&ar_vb_info->st_vb_pool_cfg, &tmp_vb_config, _IOC_SIZE(cmd));
                ar_vb_info->st_vb_pool_cfg.u32_max_pool_cnt = AR_VB_MAX_POOLS;
                ar_vb_info->is_cfged = 1;
            }

            break;

        case VB_GETCONF:
            printk(KERN_INFO "%s %x free\n", __func__, _IOC_NR(cmd));
            if (copy_to_user((void *)(uintptr_t)arg, &ar_vb_info->st_vb_pool_cfg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_GETCONF: copy_to_user error.\n");
                ret = -EFAULT;
            }
            break;

        case VB_INIT:

            if(ar_vb_info->is_inited) {
                ret = 0;
                break;
            }

            if(!ar_vb_info->is_cfged) {
                ret = -EINVAL;
                break;
            }

            memset(ar_vb_info->st_vb_lut, 0, sizeof(ar_vb_info->st_vb_lut));
            j = 0;

            for(i = 0; i < AR_VB_MAX_COMM_POOLS; ++i) {

                if(0 == ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].u32_blk_cnt ||
                   0 == ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].u64_blk_size)
                    continue;

                ret = ar_vb_create_pool(ar_vb_info->pool_cnt, ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].u32_blk_cnt,
                                                   ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].u64_blk_size,
                                                   ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].str_mmz_name,
                                                   "VbPool",
                                                   ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].e_remap_mode,
                                                   AR_POOL_OWNER_COMMON);
                if(0 != ret) {
                    break;
                }
                ar_vb_info->st_vb_lut[j].u64_blk_size = ar_vb_info->st_vb_pool_cfg.st_comm_pool[i].u64_blk_size;
                ar_vb_info->st_vb_lut[j].index = ar_vb_info->pool_cnt - 1;
                j++;
            }

            if(0 == ret) {
                ar_vb_info->is_inited = 1;
                printk(KERN_INFO "Create %d common pools\n", ar_vb_info->pool_cnt);
                sort(ar_vb_info->st_vb_lut, j, sizeof(STRU_AR_VB_LUT), vb_pool_cmp, vb_pool_swap);
                ar_vb_info->st_vb_lut_num = j;
            }
            else {
                goto VB_INIT_FAILED;
            }

            break;

        case VB_EXIT:
            if(!ar_vb_info->is_inited) {
                break;
            }

            for(i = 0; i < AR_VB_UID_BUTT; ++i) {
                if(ar_vb_info->is_mod_com_inited[i] || ar_vb_info->is_mod_com_cfged[i]) {
                    ret = -EINVAL;
                    break;
                }
            }

            if(0 != ret)
                break;

            for(i = 0; i < AR_VB_MAX_POOLS; ++i)
            {
                if(ar_vb_info->pools[i].free_blk_cnt != ar_vb_info->pools[i].blk_cnt)
                {
                    ret = -EINVAL;
                    break;
                }
            }

            if(0 != ret)
                break;

VB_INIT_FAILED:
            for(i = 0; i < AR_VB_MAX_POOLS; ++i)
            {
                if(!ar_vb_info->pools[i].is_valid)
                    continue;//break;

                if(0 == ar_vb_info->pools[i].pool_version) {
                    ret = ar_vb_destory_pool(i);
                    if(0 != ret)
                        break;
                }
                else {
                    ret = ar_vb_destory_pool_v2(i);
                    if(0 != ret)
                        break;
                }
            }

            if(0 == ret) {
                ar_vb_info->st_vb_lut_num = 0;
                memset(ar_vb_info->st_vb_lut, 0, sizeof(ar_vb_info->st_vb_lut));
                ar_vb_info->st_vb_pool_cfg.u32_max_pool_cnt = 0;
                ar_vb_info->is_supplement_cfged = 0;
                ar_vb_info->supplement_cache_size = 0;
                ar_vb_info->supplement_noncache_size = 0;
                //memset(&ar_vb_info->is_mod_com_inited, 0, sizeof(ar_vb_info->is_mod_com_inited));
                //memset(&ar_vb_info->is_mod_com_cfged, 0, sizeof(ar_vb_info->is_mod_com_cfged));
                memset(&ar_vb_info->supplement_cfg, 0, sizeof(ar_vb_info->supplement_cfg));
                ar_vb_info->is_cfged = 0;
                ar_vb_info->is_inited = 0;
                printk(KERN_INFO "VB exit\n");
            }
            break;

        case VB_CRTPL:

            if(_IOC_SIZE(cmd) != sizeof(STRU_VB_POOL_CONFIG_S)) {
                printk(KERN_ERR "VB_CRTPL: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_pool_config, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_CRTPL: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            for(i = 0; i < AR_VB_MAX_POOLS; ++i) {
                if(ar_vb_info->pools[i].is_valid)
                    continue;

                ret = ar_vb_create_pool(i, tmp_pool_config.u32_blk_cnt,
                                           tmp_pool_config.u64_blk_size,
                                           tmp_pool_config.str_mmz_name,
                                           "UserPool",
                                           tmp_pool_config.e_remap_mode,
                                           AR_POOL_OWNER_PRIVATE);
                if(0 == ret)
                    ret = i;
                break;
            }
            break;

        case VB_DESTPL:

            if(_IOC_SIZE(cmd) != sizeof(AR_VB_POOL)) {
                printk(KERN_ERR "VB_DESTPL: struct length error.\n");
                ret = -HAL_ERR_VB_ILLEGAL_PARAM;
                break;
            }

            if (copy_from_user(&pool_id, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_DESTPL: copy_from_user error.\n");
                ret = -HAL_ERR_VB_NULL_PTR;
                break;
            }

            if(pool_id >= AR_VB_MAX_POOLS) {
                printk(KERN_ERR "VB_DESTPL: invalid pool_id.\n");
                ret = -HAL_ERR_VB_ILLEGAL_PARAM;
                break;
            }

            if(ar_vb_info->pools[pool_id].is_comm_pool) {
                printk(KERN_ERR "VB_DESTPL: is_comm_pool.\n");
                ret = -HAL_ERR_VB_ILLEGAL_PARAM;
                break;
            }

            ret = ar_vb_destory_pool(pool_id);
            break;

        case VB_GETBLK:

            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_GETBLK: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_GETBLK: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(NULL != tmp_vb_msg.pstr_mmz_name) {
                if (copy_from_user(mmz_name, tmp_vb_msg.pstr_mmz_name, AR_MAX_MMZ_NAME_LEN)) {
                    printk(KERN_ERR "VB_GETBLK: copy_from_user 2 error.\n");
                    ret = -EFAULT;
                    break;
                }
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_GETBLK: e_vb_uid out of range. %d\n", tmp_vb_msg.e_vb_uid);
                ret = -EFAULT;
                break;
            }

            /*printk(KERN_INFO "VB_GETBLK %d %lld %s\n", tmp_vb_msg.u32_pool_id,
                                                    tmp_vb_msg.u64_blk_size,
                                                    mmz_name);*/

            if(AR_VB_INVALID_HANDLE == tmp_vb_msg.u32_pool_id)
                ret = ar_vb_get_blk_by_size(tmp_vb_msg.u64_blk_size, tmp_vb_msg.e_vb_uid, mmz_name);
            else if(tmp_vb_msg.u32_pool_id < AR_VB_MAX_POOLS)
                ret = ar_vb_get_blk_by_size_and_pool_id(tmp_vb_msg.u32_pool_id, tmp_vb_msg.u64_blk_size, tmp_vb_msg.e_vb_uid);
            else
                ret = AR_VB_INVALID_HANDLE;

/*            if(AR_VB_INVALID_HANDLE == ret)
                printk(KERN_DEBUG "VB_GETBLK failed! %d %lld %s\n", tmp_vb_msg.u32_pool_id,
                                                    tmp_vb_msg.u64_blk_size,
                                                    mmz_name);*/

            break;

        case VB_RLSBLK:

            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_RLSBLK: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_RLSBLK: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id) >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_RLSBLK: e_vb_uid out of range. %d\n", tmp_vb_msg.e_vb_uid);
                ret = -EFAULT;
                break;
            }

            ret = ar_vb_release_blk(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id),
                                ar_vb_handle_to_blk_idx(tmp_vb_msg.u32_block_id),
                                tmp_vb_msg.e_vb_uid);
            break;

        case VB_PA2HL:
            if(_IOC_SIZE(cmd) != sizeof(AR_U64)) {
                printk(KERN_ERR "VB_PA2HL: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&phy_addr, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_PA2HL: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            ret = ar_vb_phy_to_handle(phy_addr);

            break;

        case VB_HL2PA:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_HL2PA: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_HL2PA: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id) >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            tmp_vb_msg.u64_phy_addr = ar_vb_handle_to_phy(tmp_vb_msg.u32_block_id);
            if (copy_to_user((void *)(uintptr_t)arg, &tmp_vb_msg, _IOC_SIZE(cmd))) {
                ret = -EINVAL;
                break;
            }

            ret = 0;

            break;

        case VB_HL2PID:

            if(_IOC_SIZE(cmd) != sizeof(AR_VB_BLK)) {
                printk(KERN_ERR "VB_HL2PID: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&block_id, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_HL2PID: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(block_id) >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            ret = ar_vb_handle_to_pool_id(block_id);

            break;

        case VB_GETPLINFO:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_GET_POOL_INFO)) {
                printk(KERN_ERR "VB_GETPLINFO: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_pool_info, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_GETPLINFO: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_pool_info.u32_pool_id >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            if(!ar_vb_info->pools[tmp_pool_info.u32_pool_id].is_valid) {
                ret = -EINVAL;
                break;
            }

            if(0 == ar_vb_info->pools[tmp_pool_info.u32_pool_id].pool_version)
                tmp_pool_info.u64_pool_phy_addr = ar_vb_info->pools[tmp_pool_info.u32_pool_id].mmb->phys_addr;
            tmp_pool_info.s32_pool_owner = ar_vb_info->pools[tmp_pool_info.u32_pool_id].pool_owner;
            tmp_pool_info.u32_blk_cnt = ar_vb_info->pools[tmp_pool_info.u32_pool_id].blk_cnt;
            tmp_pool_info.u64_blk_size = ar_vb_info->pools[tmp_pool_info.u32_pool_id].pre_blk_size;
            tmp_pool_info.u64_pool_size = ar_vb_info->pools[tmp_pool_info.u32_pool_id].pool_size;
            tmp_pool_info.u32_pool_version = ar_vb_info->pools[tmp_pool_info.u32_pool_id].pool_version;

            if (copy_to_user((void *)(uintptr_t)arg, &tmp_pool_info, _IOC_SIZE(cmd))) {
                ret = -EINVAL;
                break;
            }
            break;

        case VB_INITMCPL:

            if(_IOC_SIZE(cmd) != sizeof(ENMU_VB_UID_E)) {
                printk(KERN_ERR "VB_INITMCPL: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_uid, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_INITMCPL: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_uid < AR_VB_UID_VI || tmp_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_INITMCPL: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_info->is_mod_com_inited[tmp_uid]) {
                ret = 0;
                break;
            }

            if(!ar_vb_info->is_inited) {
                printk(KERN_ERR "VB_INITMCPL: vb must be inited first.\n");
                ret = -EBUSY;
                break;
            }

            if(!ar_vb_info->is_mod_com_cfged[tmp_uid]) {
                printk(KERN_ERR "VB_INITMCPL: must be configure first.\n");
                ret = -EBUSY;
                break;
            }

            memset(ar_vb_info->st_vb_mod_lut[tmp_uid], 0, sizeof(STRU_AR_VB_LUT) * AR_VB_MAX_COMM_POOLS);
            j = 0;

            for(i = 0; i < AR_VB_MAX_COMM_POOLS; ++i) {
                unsigned int free_pool_index = 0;

                if(ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].u32_blk_cnt == 0 ||
                   ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].u64_blk_size == 0)
                    continue;

                while(ar_vb_info->pools[free_pool_index].is_valid) {
                    free_pool_index++;
                }

                ret = ar_vb_create_pool(free_pool_index, ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].u32_blk_cnt,
                                           ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].u64_blk_size,
                                           ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].str_mmz_name,
                                           "ModVb",
                                           ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].e_remap_mode,
                                           tmp_uid);

                ar_vb_info->st_vb_mod_lut[tmp_uid][j].u64_blk_size = ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid].st_comm_pool[i].u64_blk_size;
                ar_vb_info->st_vb_mod_lut[tmp_uid][j].index = free_pool_index;
                j++;
            }

            if(0 == ret) {
                ar_vb_info->is_mod_com_inited[tmp_uid] = 1;
                printk(KERN_INFO "Create %d mod common pools\n", j);
                sort(ar_vb_info->st_vb_mod_lut[tmp_uid], j, sizeof(STRU_AR_VB_LUT), vb_pool_cmp, vb_pool_swap);
                ar_vb_info->st_vb_mod_lut_num[tmp_uid] = j;
            }
            else {
                goto VB_COM_MOD_INIT_FAILED;
            }

            break;

        case VB_EXITMCPL:

            if(_IOC_SIZE(cmd) != sizeof(ENMU_VB_UID_E)) {
                printk(KERN_ERR "VB_EXITMCPL: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_uid, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_EXITMCPL: copy_from_user error\n");
                ret = -EFAULT;
                break;
            }

            if(!ar_vb_info->is_inited) {
                printk(KERN_ERR "VB_EXITMCPL: not is_inited yet\n");
                ret = -EINVAL;
                break;
            }

            if(tmp_uid < AR_VB_UID_VI || tmp_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_EXITMCPL: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }

            if(!ar_vb_info->is_mod_com_inited[tmp_uid]) {
                break;
            }
VB_COM_MOD_INIT_FAILED:

            for(i = 0; i < AR_VB_MAX_POOLS; ++i)
            {
                if(ar_vb_info->pools[i].is_valid && tmp_uid == ar_vb_info->pools[i].pool_owner) {
                    ret = ar_vb_destory_pool(i);
                    if(0 != ret)
                        break;
                }
            }

            if(0 == ret) {
                ar_vb_info->st_vb_mod_lut_num[tmp_uid] = 0;
                memset(ar_vb_info->st_vb_mod_lut[tmp_uid], 0, sizeof(STRU_AR_VB_LUT) * AR_VB_MAX_COMM_POOLS);

                ar_vb_info->is_mod_com_inited[tmp_uid] = 0;
                memset(&ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid],
                        0,
                        sizeof(ar_vb_info->st_vb_mod_com_pool_cfg[tmp_uid]));
                ar_vb_info->is_mod_com_cfged[tmp_uid] = 0;
                printk(KERN_INFO "MOD VB exit\n");
            }
            break;

        case VB_SETMCPLC:

            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_SETMCPLC: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_SETMCPLC: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_SETMCPLC: e_vb_uid out of range. %d\n", tmp_vb_msg.e_vb_uid);
                ret = -EFAULT;
                break;
            }

            if(ar_vb_info->is_mod_com_inited[tmp_vb_msg.e_vb_uid]) {
                printk(KERN_ERR "VB_SETMCPLC: is_mod_com_inited has been inited.\n");
                ret = -EBUSY;
                break;
            }

            if (copy_from_user(&tmp_vb_config, tmp_vb_msg.pst_vb_cfg, sizeof(tmp_vb_config))) {
                printk(KERN_ERR "VB_SETMCPLC: copy_from_user 2 error.\n");
                ret = -EFAULT;
                break;
            }

            for(i = 0; i < AR_VB_MAX_COMM_POOLS; ++i) {
                if(tmp_vb_config.st_comm_pool[i].u32_blk_cnt != 0 && tmp_vb_config.st_comm_pool[i].u64_blk_size != 0) {
                    if(tmp_vb_config.st_comm_pool[i].u32_blk_cnt > BLOCK_OFFSET) {
                        printk(KERN_ERR "VB_SETMCPLC: u32_blk_cnt too big.\n");
                        ret = -EINVAL;
                        break;
                    }

                    if(AR_MAX_MMZ_NAME_LEN == strnlen(tmp_vb_config.st_comm_pool[i].str_mmz_name, AR_MAX_MMZ_NAME_LEN)) {
                        printk(KERN_ERR "VB_SETMCPLC: str_mmz_name name too long.\n");
                        ret = -EINVAL;
                        break;
                    }
                }
            }

            if(0 == ret) {
                memcpy(&ar_vb_info->st_vb_mod_com_pool_cfg[tmp_vb_msg.e_vb_uid], &tmp_vb_config, sizeof(tmp_vb_config));
                ar_vb_info->st_vb_mod_com_pool_cfg[tmp_vb_msg.e_vb_uid].u32_max_pool_cnt = AR_VB_MAX_POOLS;
                ar_vb_info->is_mod_com_cfged[tmp_vb_msg.e_vb_uid] = 1;
            }

            break;

        case VB_GETMCPLC:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_GETMCPLC: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_GETMCPLC: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_GETMCPLC: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }

            if (copy_to_user((void *)(uintptr_t)tmp_vb_msg.pst_vb_cfg,
                &ar_vb_info->st_vb_mod_com_pool_cfg[tmp_vb_msg.e_vb_uid], sizeof(STRU_VB_CONFIG_S))) {
                printk(KERN_ERR "VB_GETMCPLC: copy_to_user error.\n");
                ret = -EFAULT;
            }
            break;

        case VB_USERADD:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_USERADD: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_USERADD: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_USERADD: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id) >= AR_VB_MAX_POOLS) {
                printk(KERN_ERR "VB_USERADD: u32_block_id out of range.\n");
                ret = -EINVAL;
                break;
            }

            ret = ar_vb_user_add(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id),
                                 ar_vb_handle_to_blk_idx(tmp_vb_msg.u32_block_id),
                                 tmp_vb_msg.e_vb_uid);


            break;

        case VB_USERSUB:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_USERSUB: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_USERSUB: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_USERSUB: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }


            if(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id) >= AR_VB_MAX_POOLS) {
                printk(KERN_ERR "VB_USERADD: u32_block_id out of range.\n");
                ret = -EINVAL;
                break;
            }

            ret = ar_vb_user_sub(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id),
                                 ar_vb_handle_to_blk_idx(tmp_vb_msg.u32_block_id),
                                 tmp_vb_msg.e_vb_uid);


            break;

        case VB_INQUCNT:
            if(_IOC_SIZE(cmd) != sizeof(AR_VB_BLK)) {
                printk(KERN_ERR "VB_INQUCNT: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_block, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_INQUCNT: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(tmp_block) >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            if(!ar_vb_info->pools[ar_vb_handle_to_pool_id(tmp_block)].is_valid) {
                ret = -EINVAL;
                break;
            }

            ret = ar_vb_inquire_blk_cnt(tmp_block);
            break;

        case VB_GETSUPPLEADDR:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_GETSUPPLEADDR: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_GETSUPPLEADDR: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(tmp_vb_msg.u32_block_id) >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            tmp_supplement = ar_vb_handle_to_supplement(tmp_vb_msg.u32_block_id);
            if(NULL == tmp_supplement) {
                printk(KERN_ERR "VB_GETSUPPLEADDR: copy_from_user error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_to_user((void *)(uintptr_t)tmp_vb_msg.pst_supplement,
                tmp_supplement, sizeof(STRU_VIDEO_SUPPLEMENT_S))) {
                printk(KERN_ERR "VB_GETSUPPLEADDR: copy_to_user error.\n");
                ret = -EFAULT;
            }

            break;

        case VB_SETSUPPLECFG:
            if(_IOC_SIZE(cmd) != sizeof(STRU_VB_SUPPLEMENT_CONFIG_S *)) {
                printk(KERN_ERR "VB_SETSUPPLECFG: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if(ar_vb_info->is_inited) {
                printk(KERN_ERR "VB_SETSUPPLECFG: already inited\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&ar_vb_info->supplement_cfg,
                (void *)(uintptr_t)arg, sizeof(STRU_VB_SUPPLEMENT_CONFIG_S))) {
                printk(KERN_ERR "VB_SETSUPPLECFG: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            ar_vb_info->is_supplement_cfged = 1;

            break;

        case VB_GETSUPPLECFG:
            if(_IOC_SIZE(cmd) != sizeof(STRU_VB_SUPPLEMENT_CONFIG_S *)) {
                printk(KERN_ERR "VB_GETSUPPLECFG: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_to_user((void *)(uintptr_t)arg,
                &ar_vb_info->supplement_cfg, sizeof(STRU_VB_SUPPLEMENT_CONFIG_S))) {
                printk(KERN_ERR "VB_GETSUPPLECFG: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }
            break;
        
        case VB_CRTPL_V2:
            if(_IOC_SIZE(cmd) != sizeof(STRU_VB_POOL_CONFIG_V2_S)) {
                printk(KERN_ERR "VB_CRTPL: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_pool_config_v2, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_CRTPL: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            phy_addr_list = (AR_U64 *)vmalloc(sizeof(AR_U64) * tmp_pool_config_v2.u32_blk_cnt);
            if(NULL == phy_addr_list) {
                printk(KERN_ERR "VB_CRTPL_V2: vmalloc failed.\n");
                ret = -EFAULT;
                break;
            }
            
            if (copy_from_user(phy_addr_list, 
                (void *)(uintptr_t)tmp_pool_config_v2.u64_blk_addr, 
                sizeof(AR_U64) * tmp_pool_config_v2.u32_blk_cnt)) {
                vfree(phy_addr_list);
                printk(KERN_ERR "VB_CRTPL_V2: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            for(i = 0; i < AR_VB_MAX_POOLS; ++i) {
                if(ar_vb_info->pools[i].is_valid)
                    continue;

                ret = ar_vb_create_pool_v2(i, tmp_pool_config_v2.u32_blk_cnt,
                                           tmp_pool_config_v2.u64_blk_size,
                                           tmp_pool_config_v2.str_mmz_name,
                                           "UserPool",
                                           tmp_pool_config_v2.e_remap_mode,
                                           AR_POOL_OWNER_PRIVATE,
                                           phy_addr_list);
                if(0 == ret)
                    ret = i;
                break;
            }
            vfree(phy_addr_list);
            phy_addr_list = NULL;
            break;
        
        case VB_DESTPL_V2:
            if(_IOC_SIZE(cmd) != sizeof(AR_VB_POOL)) {
                printk(KERN_ERR "VB_DESTPL: struct length error.\n");
                ret = -HAL_ERR_VB_ILLEGAL_PARAM;
                break;
            }

            if (copy_from_user(&pool_id, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_DESTPL: copy_from_user error.\n");
                ret = -HAL_ERR_VB_NULL_PTR;
                break;
            }

            if(pool_id >= AR_VB_MAX_POOLS) {
                printk(KERN_ERR "VB_DESTPL: invalid pool_id.\n");
                ret = -HAL_ERR_VB_ILLEGAL_PARAM;
                break;
            }

            if(ar_vb_info->pools[pool_id].is_comm_pool) {
                printk(KERN_ERR "VB_DESTPL: is_comm_pool.\n");
                ret = -HAL_ERR_VB_ILLEGAL_PARAM;
                break;
            }

            ret = ar_vb_destory_pool_v2(pool_id);
            break;

        case VB_GETPLINFO_V2:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_GET_POOL_INFO)) {
                printk(KERN_ERR "VB_GETPLINFO_V2: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_pool_info, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_GETPLINFO_V2: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(1 != tmp_pool_info.u32_pool_version) {
                ret = -EINVAL;
                break;
            }

            if(tmp_pool_info.u32_pool_id >= AR_VB_MAX_POOLS) {
                ret = -EINVAL;
                break;
            }

            if(NULL == tmp_pool_info.v2_info.p_pool_blk_pa_list) {
                ret = -EINVAL;
                break;
            }

            if(!ar_vb_info->pools[tmp_pool_info.u32_pool_id].is_valid) {
                ret = -EINVAL;
                break;
            }

            if(!ar_vb_info->pools[tmp_pool_info.u32_pool_id].v2_pa_list) {
                ret = -EINVAL;
                break;
            }

            if (copy_to_user((void *)(uintptr_t)tmp_pool_info.v2_info.p_pool_blk_pa_list, 
                ar_vb_info->pools[tmp_pool_info.u32_pool_id].v2_pa_list, 
                tmp_pool_info.u32_blk_cnt * sizeof(unsigned long long))) {
                ret = -EINVAL;
                break;
            }

            break;

        case VB_USERADD_PA:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_USERADD_PA: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_USERADD_PA: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_USERADD_PA: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }

            ret = ar_vb_phy_to_handle(tmp_vb_msg.u64_phy_addr);
            if(ret < 0) {
                printk(KERN_ERR "VB_USERADD_PA: phy_addr out of range.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(ret) >= AR_VB_MAX_POOLS) {
                printk(KERN_ERR "VB_USERADD_PA: u32_block_id out of range.\n");
                ret = -EINVAL;
                break;
            }

            ret = ar_vb_user_add(ar_vb_handle_to_pool_id(ret),
                                 ar_vb_handle_to_blk_idx(ret),
                                 tmp_vb_msg.e_vb_uid);

            break;

        case VB_USERSUB_PA:
            if(_IOC_SIZE(cmd) != sizeof(STRU_AR_VB_MSG)) {
                printk(KERN_ERR "VB_USERSUB_PA: struct length error.\n");
                ret = -EINVAL;
                break;
            }

            if (copy_from_user(&tmp_vb_msg, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                printk(KERN_ERR "VB_USERSUB_PA: copy_from_user error.\n");
                ret = -EFAULT;
                break;
            }

            if(tmp_vb_msg.e_vb_uid < AR_VB_UID_VI || tmp_vb_msg.e_vb_uid >= AR_VB_UID_BUTT) {
                printk(KERN_ERR "VB_USERSUB_PA: e_vb_uid out of range.\n");
                ret = -EFAULT;
                break;
            }

            ret = ar_vb_phy_to_handle(tmp_vb_msg.u64_phy_addr);
            if(ret < 0) {
                printk(KERN_ERR "VB_USERSUB_PA: phy_addr out of range.\n");
                ret = -EFAULT;
                break;
            }

            if(ar_vb_handle_to_pool_id(ret) >= AR_VB_MAX_POOLS) {
                printk(KERN_ERR "VB_USERSUB_PA: u32_block_id out of range.\n");
                ret = -EINVAL;
                break;
            }

            ret = ar_vb_user_sub(ar_vb_handle_to_pool_id(ret),
                                 ar_vb_handle_to_blk_idx(ret),
                                 tmp_vb_msg.e_vb_uid);

            break;

        default:
            break;
    }

    return ret;
}

static int ar_vb_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "%s\n", __func__);
    return 0;
}

static int ar_vb_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "%s\n", __func__);
    return 0;
}

static long ar_vb_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    mutex_lock(&g_vb_ioctl_lock);
    //printk(KERN_INFO "%s %x\n", __func__, cmd);
    if (_IOC_TYPE(cmd) == 't') {
        printk(KERN_INFO "%s: %c %d\n", __func__, _IOC_TYPE(cmd), _IOC_NR(cmd));
        ret = ar_vb_ioctl_t(filp, cmd, arg);
    }
    else if(_IOC_TYPE(cmd) == 'b') {
        //printk(KERN_INFO "%s: %c %d\n", __func__, _IOC_TYPE(cmd), _IOC_NR(cmd));
        ret = ar_vb_ioctl_b(filp, cmd, arg);
    }
    ++g_ioctl_cnt;
    mutex_unlock(&g_vb_ioctl_lock);
    return ret;
}

static void ar_vb_print_one_pool(STRU_AR_VB_POOL_INFO * pool_info)
{
    unsigned int sum[AR_VB_UID_BUTT] = {0};
    unsigned int i, j;
    printk(KERN_CONT KERN_INFO "\n----------------------------------------------------------------------------\n");
    printk(KERN_CONT KERN_INFO "PoolId\tPA\t\t\tVA\t\tIsComm\tOwner\tBlkSz\tBlkCnt\tFree\tMiniFree\n");
    printk(KERN_CONT KERN_INFO "%d\t0x%lx\t%p\t%d\t%d\t%lld\t%d\t%d\t%d\t\n",
        pool_info->pool_id,
        (0 == pool_info->pool_version) ? pool_info->mmb->phys_addr : (unsigned long)pool_info->v2_pa_list[0],
        pool_info->p_pool_vir_addr,
        pool_info->is_comm_pool,
        pool_info->pool_owner,
        pool_info->pre_blk_size,
        pool_info->blk_cnt,
        pool_info->free_blk_cnt,
        pool_info->mini_free_blk_cnt);

    printk(KERN_CONT KERN_INFO "BLK\tVI\tVO\tVGS\tVENC\tVDEC\tH265E\tH264E\tJPFGE\tH264D\tJPEGD\tVPSS\tDIS\tUSER\tPCIV\t"\
        "AI\tAENC\tRC\tVFMW\tGDC\tAVS\tRECT\tMATCH\tMCF\tRGN\n");
    for(i = 0; i < pool_info->blk_cnt; ++i) {
        if(pool_info->block[i].ref_cnt) {
            printk(KERN_CONT KERN_INFO "%d", i);
            for(j = 0; j < AR_VB_UID_BUTT; ++j) {
                printk(KERN_CONT KERN_INFO "\t%d", pool_info->block[i].user[j]);
                sum[j] += pool_info->block[i].user[j];
            }
            printk(KERN_CONT KERN_INFO "\n");
        }
    }

    printk(KERN_CONT KERN_INFO "Sum");
    for(j = 0; j < AR_VB_UID_BUTT; ++j) {
        printk(KERN_CONT KERN_INFO "\t%d", sum[j]);
    }
    printk(KERN_CONT KERN_INFO "\n");
}

static void ar_vb_print_one_cfg(STRU_VB_POOL_CONFIG_S * pool_cfg)
{
    int i;
    printk(KERN_CONT KERN_INFO "PoolId");
    for(i = 0; i < 16; i++) {
        printk(KERN_CONT KERN_INFO "\t%d", i);
    }
    printk(KERN_CONT KERN_INFO "\n");

    printk(KERN_CONT KERN_INFO "Size");
    for(i = 0; i < 16; i++) {
        printk(KERN_CONT KERN_INFO "\t%lld", pool_cfg[i].u64_blk_size);
    }
    printk(KERN_CONT KERN_INFO "\n");

    printk(KERN_CONT KERN_INFO "Count");
    for(i = 0; i < 16; i++) {
        printk(KERN_CONT KERN_INFO "\t%d", pool_cfg[i].u32_blk_cnt);
    }
    printk(KERN_CONT KERN_INFO "\n");
}

static void ar_vb_print_pool_cfgs(void)
{
    ENMU_VB_UID_E i;
    if(ar_vb_info->is_cfged && ar_vb_info->is_inited) {
        printk(KERN_CONT KERN_INFO "-----COMMON POOL CONFIG----------------------------------\n");
        ar_vb_print_one_cfg(ar_vb_info->st_vb_pool_cfg.st_comm_pool);
    }

    for(i = AR_VB_UID_VI; i < AR_VB_UID_BUTT; ++i) {
        if(ar_vb_info->is_mod_com_cfged[i] && ar_vb_info->is_mod_com_inited[i]) {
            printk(KERN_CONT KERN_INFO "-----MODULE COMMON POOL CONFIG OF VB_UID <%d>--------\n", i);
            ar_vb_print_one_cfg(ar_vb_info->st_vb_mod_com_pool_cfg[i].st_comm_pool);
        }
    }

    printk(KERN_CONT KERN_INFO "g_ioctl_cnt %d", g_ioctl_cnt);
    printk(KERN_CONT KERN_INFO "\n");
    for(i = 0; i < (VB_USERSUB_PA + 1); i++) {
        printk(KERN_CONT KERN_INFO "\t%d", g_ioctl_sub_cnt[i]);
    }
    printk(KERN_CONT KERN_INFO "\n");
}

static ssize_t ar_vb_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int i;
    printk(KERN_CONT KERN_INFO "%s\n", __func__);
    printk(KERN_CONT KERN_INFO "-----AR VB PUB CONFIG--------------------\n");
    printk(KERN_CONT KERN_INFO "MAX POOL\n");
    printk(KERN_CONT KERN_INFO "\t%d\n", ar_vb_info->st_vb_pool_cfg.u32_max_pool_cnt);
    printk(KERN_CONT KERN_INFO "-----AR VB SUPPLEMENT ATTR---------------\n");
    printk(KERN_CONT KERN_INFO "\tConfig\tSize\tVbcnt\n");
    printk(KERN_CONT KERN_INFO "\t%6d\t%4d\t%4d\n", ar_vb_info->supplement_cfg.u32_supplement_config,
                                          ar_vb_info->supplement_cache_size,
                                          ar_vb_info->supplement_blk_cnt);

    ar_vb_print_pool_cfgs();

    printk(KERN_CONT KERN_INFO "-----------------------------------------------------------\n");
    for(i = 0; i < AR_VB_MAX_POOLS; ++i) {
        if(ar_vb_info->pools[i].is_valid) {
            ar_vb_print_one_pool(&ar_vb_info->pools[i]);
        }
    }
    return count;
}

static ssize_t ar_vb_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int i;
    printk(KERN_CONT KERN_INFO "%s\n", __func__);
    printk(KERN_CONT KERN_INFO "-----AR VB PUB CONFIG--------------------\n");
    printk(KERN_CONT KERN_INFO "MAX POOL\n");
    printk(KERN_CONT KERN_INFO "\t%d\n", ar_vb_info->st_vb_pool_cfg.u32_max_pool_cnt);
    printk(KERN_CONT KERN_INFO "-----AR VB SUPPLEMENT ATTR---------------\n");
    printk(KERN_CONT KERN_INFO "\tConfig\tSize\tVbcnt\n");
    printk(KERN_CONT KERN_INFO "\t%6d\t%4d\t%4d\n", ar_vb_info->supplement_cfg.u32_supplement_config,
                                          ar_vb_info->supplement_cache_size,
                                          ar_vb_info->supplement_blk_cnt);

    ar_vb_print_pool_cfgs();

    printk(KERN_CONT KERN_INFO "-----------------------------------------------------------\n");
    for(i = 0; i < AR_VB_MAX_POOLS; ++i) {
        if(ar_vb_info->pools[i].is_valid) {
            ar_vb_print_one_pool(&ar_vb_info->pools[i]);
        }
    }
    /*void* data_buf = kmalloc(count+1, GFP_KERNEL);
    memset(data_buf, 0, count+1);

    if(copy_from_user(data_buf, buf, count))
    {
        printk(KERN_ERR "%s copy failed!\n", __func__);
        kfree(data_buf);
    }

    printk(KERN_INFO "%s %ld %s\n", __func__, count, (char *)data_buf);
    kfree(data_buf);*/
    return count;
}

static struct file_operations ar_vb_fops =
{
    .owner          = THIS_MODULE,
    .open           = ar_vb_open,
    .release        = ar_vb_release,
    .unlocked_ioctl = ar_vb_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl	= ar_vb_ioctl,
#endif
    .read           = ar_vb_read,
    .write          = ar_vb_write,
};

static struct miscdevice ar_vb =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "ar_vb",
    .fops = &ar_vb_fops,
};

static int __init ar_vb_init(void)
{
    int ret = -1;

    ar_vb_info = kzalloc(sizeof(*ar_vb_info), GFP_KERNEL);
    if (!ar_vb_info)
        return -ENOMEM;

    ret = misc_register(&ar_vb);
    if(ret < 0)
    {
        kfree(ar_vb_info);
        goto out;
    }

    spin_lock_init(&g_vb_spin_lock);
    mutex_init(&g_vb_ioctl_lock);

out:
    printk(KERN_INFO "ar_vb_init %d\n", ret);
    return ret;
}

static void __exit ar_vb_exit(void)
{
    misc_deregister(&ar_vb);

    if(NULL != ar_vb_info) {
        kfree(ar_vb_info);
        ar_vb_info = NULL;
    }

    printk(KERN_INFO "ar_vb_exit\n");
}

module_init(ar_vb_init);
module_exit(ar_vb_exit);

MODULE_AUTHOR("Artosyn");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
