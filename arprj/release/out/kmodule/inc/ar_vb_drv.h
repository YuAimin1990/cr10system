#ifndef _AR_VB_DRV_H_
#define _AR_VB_DRV_H_

#include <linux/ioctl.h>
#include "hal_type.h"
#include "hal_errno.h"


/////////////////////////////////////////////////////////////////////////////
#define IOC_TYPE_TEST                   't'

#define IOCTEST_0                       _IO(IOC_TYPE_TEST, 0)
#define IOCTEST_1                       _IO(IOC_TYPE_TEST, 1)

#define VB_SETCONF                      (0)
#define VB_GETCONF                      (1)
#define VB_INIT                         (2)
#define VB_EXIT                         (3)
#define VB_CRTPL                        (4)
#define VB_DESTPL                       (5)
#define VB_GETBLK                       (6)
#define VB_RLSBLK                       (7)
#define VB_PA2HL                        (8)
#define VB_HL2PA                        (9)
#define VB_HL2PID                       (10)
#define VB_GETPLINFO                    (11)
#define VB_MUMAPPL                      (12)
#define VB_GETBLKVA                     (13)
#define VB_INITMCPL                     (14)
#define VB_EXITMCPL                     (15)
#define VB_SETMCPLC                     (16)
#define VB_GETMCPLC                     (17)
#define VB_INQUCNT                      (18)
#define VB_GETSUPPLEADDR                (19)
#define VB_SETSUPPLECFG                 (20)
#define VB_GETSUPPLECFG                 (21)
#define VB_USERADD                      (22)
#define VB_USERSUB                      (23)
#define VB_CRTPL_V2                     (24)
#define VB_DESTPL_V2                    (25)
#define VB_GETPLINFO_V2                 (26)
#define VB_USERADD_PA                   (27)
#define VB_USERSUB_PA                   (28)

#define IOC_TYPE_VB                     'b'
#define IOC_VB_SETCONF                  _IOW(IOC_TYPE_VB, VB_SETCONF, STRU_VB_CONFIG_S)
#define IOC_VB_GETCONF                  _IOR(IOC_TYPE_VB, VB_GETCONF, STRU_VB_CONFIG_S)
#define IOC_VB_INIT                     _IO(IOC_TYPE_VB, VB_INIT)
#define IOC_VB_EXIT                     _IO(IOC_TYPE_VB, VB_EXIT)
#define IOC_VB_CRTPL                    _IOW(IOC_TYPE_VB, VB_CRTPL, STRU_VB_POOL_CONFIG_S)
#define IOC_VB_DESTPL                   _IOW(IOC_TYPE_VB, VB_DESTPL, AR_VB_POOL)
#define IOC_VB_GETBLK                   _IOWR(IOC_TYPE_VB, VB_GETBLK, STRU_AR_VB_MSG)
#define IOC_VB_RLSBLK                   _IOW(IOC_TYPE_VB, VB_RLSBLK, STRU_AR_VB_MSG)
#define IOC_VB_PA2HL                    _IOW(IOC_TYPE_VB, VB_PA2HL, AR_U64)
#define IOC_VB_HL2PA                    _IOWR(IOC_TYPE_VB, VB_HL2PA, STRU_AR_VB_MSG)
#define IOC_VB_HL2PID                   _IOW(IOC_TYPE_VB, VB_HL2PID, AR_VB_BLK)
#define IOC_VB_GETPLINFO                _IOWR(IOC_TYPE_VB, VB_GETPLINFO, STRU_AR_VB_GET_POOL_INFO)
//#define IOC_VB_MUMAPPL                  _IO(IOC_TYPE_VB, VB_MUMAPPL)
//#define IOC_VB_GETBLKVA                 _IO(IOC_TYPE_VB, VB_GETBLKVA)
#define IOC_VB_INITMCPL                 _IOW(IOC_TYPE_VB, VB_INITMCPL, ENMU_VB_UID_E)
#define IOC_VB_EXITMCPL                 _IOW(IOC_TYPE_VB, VB_EXITMCPL, ENMU_VB_UID_E)
#define IOC_VB_SETMCPLC                 _IOW(IOC_TYPE_VB, VB_SETMCPLC, STRU_AR_VB_MSG)
#define IOC_VB_GETMCPLC                 _IOW(IOC_TYPE_VB, VB_GETMCPLC, STRU_AR_VB_MSG)
#define IOC_VB_INQUCNT                  _IOW(IOC_TYPE_VB, VB_INQUCNT, AR_VB_BLK)
#define IOC_VB_GETSUPPLEADDR            _IOW(IOC_TYPE_VB, VB_GETSUPPLEADDR, STRU_AR_VB_MSG)
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
#define IOC_VB_SETSUPPLECFG             _IOW(IOC_TYPE_VB, VB_SETSUPPLECFG, AR_U64)
#define IOC_VB_GETSUPPLECFG             _IOW(IOC_TYPE_VB, VB_GETSUPPLECFG, AR_U64)
#else
#define IOC_VB_SETSUPPLECFG             _IOW(IOC_TYPE_VB, VB_SETSUPPLECFG, STRU_VB_SUPPLEMENT_CONFIG_S *)
#define IOC_VB_GETSUPPLECFG             _IOW(IOC_TYPE_VB, VB_GETSUPPLECFG, STRU_VB_SUPPLEMENT_CONFIG_S *)
#endif
#define IOC_VB_USERADD                  _IOW(IOC_TYPE_VB, VB_USERADD, STRU_AR_VB_MSG)
#define IOC_VB_USERSUB                  _IOW(IOC_TYPE_VB, VB_USERSUB, STRU_AR_VB_MSG)
#define IOC_VB_CRTPL_V2                 _IOW(IOC_TYPE_VB, VB_CRTPL_V2, STRU_VB_POOL_CONFIG_V2_S)
#define IOC_VB_DESTPL_V2                _IOW(IOC_TYPE_VB, VB_DESTPL_V2, AR_VB_POOL)
#define IOC_VB_GETPLINFO_V2             _IOWR(IOC_TYPE_VB, VB_GETPLINFO_V2, STRU_AR_VB_GET_POOL_INFO)
#define IOC_VB_USERADD_PA               _IOW(IOC_TYPE_VB, VB_USERADD_PA, STRU_AR_VB_MSG)
#define IOC_VB_USERSUB_PA               _IOW(IOC_TYPE_VB, VB_USERSUB_PA, STRU_AR_VB_MSG)

#define AR_ALIGN_NUM                       8
#define AR_ATTRIBUTE                       __attribute__((aligned(AR_ALIGN_NUM)))

#define AR_ID_VB                           (1)//tmp for test

/* Generall common pool use this owner id, module common pool use VB_UID as owner id */
#define AR_POOL_OWNER_COMMON               (-1)

/* Private pool use this owner id */
#define AR_POOL_OWNER_PRIVATE              (-2)


#define AR_MAX_MMZ_NAME_LEN                (16)

#define AR_VB_INVALID_POOLID               (-1U)
#define AR_VB_INVALID_HANDLE               (-1U)

#define AR_VB_MAX_COMM_POOLS               (16)
#define AR_VB_MAX_MOD_COMM_POOLS           (16)

/* user ID for VB */
#define AR_VB_MAX_USER                     AR_VB_UID_BUTT

#define AR_VB_MAX_POOLS                    (512)

#define AR_VB_SUPPLEMENT_JPEG_MASK         (0x1)   //256byte
#define AR_VB_SUPPLEMENT_ISPINFO_MASK      (0x2)   //256byte
#define AR_VB_SUPPLEMENT_MOTION_DATA_MASK  (0x4)
#define AR_VB_SUPPLEMENT_DNG_MASK          (0x8)   //256byte

#define HAL_ERR_VB_NULL_PTR             AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define HAL_ERR_VB_NOMEM                AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
#define HAL_ERR_VB_NOBUF                AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define HAL_ERR_VB_UNEXIST              AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
#define HAL_ERR_VB_ILLEGAL_PARAM        AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define HAL_ERR_VB_NOTREADY             AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define HAL_ERR_VB_BUSY                 AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define HAL_ERR_VB_NOT_PERM             AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define HAL_ERR_VB_SIZE_NOT_ENOUGH      AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_SIZE_NOT_ENOUGH)
#define HAL_ERR_VB_2MPOOLS              AR_KMODULE_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUTT + 1)

typedef AR_U32 AR_VB_POOL;
typedef AR_U32 AR_VB_BLK;

typedef enum
{
    AR_VB_REMAP_MODE_NONE = 0, /* no remap */
    AR_VB_REMAP_MODE_NOCACHE   = 1, /* no cache remap */
    AR_VB_REMAP_MODE_CACHED    = 2, /* cache remap, if you use this mode, you should flush cache by yourself */
    AR_VB_REMAP_MODE_BUTT      = 3,
} ENUM_VB_REMAP_MODE_E;

typedef enum
{
    AR_VB_UID_VI               = 0,
    AR_VB_UID_VO               = 1,
    AR_VB_UID_VGS              = 2,
    AR_VB_UID_VENC             = 3,
    AR_VB_UID_VDEC             = 4,
    AR_VB_UID_H265E            = 5,
    AR_VB_UID_H264E            = 6,
    AR_VB_UID_JPEGE            = 7,
    AR_VB_UID_H264D            = 8,
    AR_VB_UID_JPEGD            = 9,
    AR_VB_UID_VPSS             = 10,
    AR_VB_UID_DIS              = 11,
    AR_VB_UID_USER             = 12,
    AR_VB_UID_PCIV             = 13,
    AR_VB_UID_AI               = 14,
    AR_VB_UID_AENC             = 15,
    AR_VB_UID_RC               = 16,
    AR_VB_UID_VFMW             = 17,
    AR_VB_UID_GDC              = 18,
    AR_VB_UID_AVS              = 19,
    AR_VB_UID_DPU_RECT         = 20,
    AR_VB_UID_DPU_MATCH        = 21,
    AR_VB_UID_MCF              = 22,
    AR_VB_UID_RGN              = 23,
    AR_VB_UID_BUTT             = 24
} ENMU_VB_UID_E;


//#pragma pack(push)
//#pragma pack(1)

typedef struct
{
    AR_U64                  u64_blk_size;
    AR_U32                  u32_blk_cnt;
    ENUM_VB_REMAP_MODE_E    e_remap_mode;
    AR_CHAR                 str_mmz_name[AR_MAX_MMZ_NAME_LEN + 1];//add one for '\0'
} STRU_VB_POOL_CONFIG_S;

typedef struct
{
    AR_U64                  u64_blk_size;               /* 缓存块大小，以Byte位单位. */
    AR_U32                  u32_blk_cnt;                /* 每个缓存池的缓存块个数，取值范围：(0, 10240]. */
    AR_U64                  *u64_blk_addr;              /* 每个缓存池的缓存块的物理地址. */
    AR_VOID                 **p_blk_vaddr;              /* 每个缓存池的缓存块的虚拟地址. */
    ENUM_VB_REMAP_MODE_E    e_remap_mode;               /* VB 的内核态虚拟地址映射模式 */
    AR_CHAR                 str_mmz_name[AR_MAX_MMZ_NAME_LEN + 1];     /* 当前缓存池从哪个MMZ区域分配内存 */
} STRU_VB_POOL_CONFIG_V2_S;

typedef struct
{
    AR_U32                  u32_max_pool_cnt;
    STRU_VB_POOL_CONFIG_S   st_comm_pool[AR_VB_MAX_COMM_POOLS];
} STRU_VB_CONFIG_S;

typedef struct
{
    AR_U32                  u32_is_comm_pool;
    AR_U32                  u32_blk_cnt;
    AR_U32                  u32_free_blk_cnt;
} STRU_VB_POOL_STATUS_S;

typedef struct
{
    AR_U64                  u64_jpeg_DCF_phy_addr;
    AR_U64                  u64_isp_info_phy_addr;
    AR_U64                  u64_low_delay_phy_addr;
    AR_U64                  u64_motion_data_phy_addr;
    AR_U64                  u64_frame_DNG_phy_addr;

    AR_VOID* AR_ATTRIBUTE      p_jpeg_DCF_vir_addr;        /* JPEG_DCF_S, used in JPEG DCF */
    AR_VOID* AR_ATTRIBUTE      p_isp_info_vir_addr;        /* ISP_FRAME_INFO_S, used in ISP debug, when get raw and send raw */
    AR_VOID* AR_ATTRIBUTE      p_low_delay_vir_addr;       /* used in low delay */
    AR_VOID* AR_ATTRIBUTE      p_motion_data_vir_addr;     /* vpss 3dnr use: gme motion data, Filter motion data, gyro data.*/
    AR_VOID* AR_ATTRIBUTE      p_frame_DNG_vir_addr;
} STRU_VIDEO_SUPPLEMENT_S;

typedef struct
{
    AR_U32                  u32_supplement_config;
} STRU_VB_SUPPLEMENT_CONFIG_S;

typedef struct {
    AR_U64* AR_ATTRIBUTE p_pool_blk_pa_list;
    AR_VOID** AR_ATTRIBUTE p_pool_blk_va_list;
} STRU_AR_VB_POOL_V2_INFO;

typedef struct {
    AR_U32                  u32_pool_id;
    AR_S32                  s32_pool_owner;
    AR_U32                  u32_blk_cnt;
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
    AR_U32                  u32_pool_version;
#endif
    AR_U64                  u64_blk_size;
    AR_U64                  u64_pool_size;
    AR_U64                  u64_pool_phy_addr;
    AR_VOID* AR_ATTRIBUTE   p_pool_vir_addr;
    AR_VOID* AR_ATTRIBUTE   p_pool_vir_addr_cache;
#if !(defined(KERNEL_BIT_64) && defined(USER_BIT_32))
	AR_U32					u32_pool_version;
#endif
    STRU_AR_VB_POOL_V2_INFO v2_info;
} STRU_AR_VB_GET_POOL_INFO;

typedef struct {
    AR_VB_POOL              u32_pool_id;
    AR_VB_BLK               u32_block_id;
    AR_U64                  u64_blk_size;
    AR_U64                  u64_phy_addr;
    ENMU_VB_UID_E           e_vb_uid;
    AR_CHAR* AR_ATTRIBUTE   pstr_mmz_name;
    STRU_VB_CONFIG_S* AR_ATTRIBUTE pst_vb_cfg;
    STRU_VB_POOL_CONFIG_S* AR_ATTRIBUTE pst_vb_pool_cfg;
    STRU_VIDEO_SUPPLEMENT_S* AR_ATTRIBUTE pst_supplement;
}STRU_AR_VB_MSG;

/*typedef struct
{
    unsigned int            u32_pool_id;
    unsigned int            u32_blk_cnt;
    unsigned long long      u64_blk_size;
    unsigned long long      u64_pool_size;
    unsigned long long      u64_pool_phy_addr;
    void ATTRIBUTE          *p_pool_vir_addr;
    unsigned int            u32_is_comm_pool;
    unsigned int            u32_blk_cnt;
    unsigned int            u32_free_blk_cnt;
} STRU_AR_VB_POOL_STATUS_S;


typedef struct
{
    STRU_VB_CONFIG_S        st_vb_pool_cfg;
    STRU_VB_POOL_STATUS_S   st_vb_pool_status[VB_MAX_POOLS];
    unsigned int            u32_is_inited;
} STRU_AR_VB_POOL_S;*/

//#pragma pack(pop)

#endif
