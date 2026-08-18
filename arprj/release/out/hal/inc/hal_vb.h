/**
 * @file hal_vb.h
 * @brief  hal 视频流缓冲区API和数据结构
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/04/28
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef __HAL_VB_H__
#define __HAL_VB_H__

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_type.h"
#include "hal_errno.h"
#include "hal_sys.h"

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

/**
* @note    当前支持4种VB 内存附加信息
*/
#define AR_VB_SUPPLEMENT_JPEG_MASK         (0x1)
#define AR_VB_SUPPLEMENT_ISPINFO_MASK      (0x2)
#define AR_VB_SUPPLEMENT_MOTION_DATA_MASK  (0x4)
#define AR_VB_SUPPLEMENT_DNG_MASK          (0x8)

#define HAL_ERR_VB_NULL_PTR             AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define HAL_ERR_VB_NOMEM                AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
#define HAL_ERR_VB_NOBUF                AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define HAL_ERR_VB_UNEXIST              AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
#define HAL_ERR_VB_ILLEGAL_PARAM        AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define HAL_ERR_VB_NOTREADY             AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define HAL_ERR_VB_BUSY                 AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define HAL_ERR_VB_NOT_PERM             AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define HAL_ERR_VB_SIZE_NOT_ENOUGH      AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_SIZE_NOT_ENOUGH)

#define HAL_ERR_VB_2MPOOLS              AR_HAL_DEF_ERR(AR_SYS_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUTT + 1)

typedef AR_U32 AR_VB_POOL;
typedef AR_U32 AR_VB_BLK;

typedef enum
{
    AR_VB_REMAP_MODE_NONE      = 0, /* no remap */
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
    AR_VB_UID_BUTT             = 24,
} ENUM_VB_UID_E;

//#pragma pack(push)
//#pragma pack(1)

/**
* @note  定义视频缓存池属性结构体。
* @note  每个缓存块的大小u64_blk_size 应根据当前图像宽高、像素格式、数据位宽、是否压缩等来计算。
* @note  该缓存池是从空闲MMZ内存中分配，一个缓存池包含若干个大小相同的缓存块，如果该缓存池的大小超过了保留内存中的空闲空间，则创建缓存池会失败。
* @note  用户需保证输入的DDR名字已经存在，如果输入不存在DDR的名字，会造成分不到内存。如果数组str_mmz_name 被memset为0则表示在没有命名的DDR中创建缓存池。
*/
typedef struct
{
    AR_U64                  u64_blk_size;               /* 缓存块大小，以Byte位单位. */
    AR_U32                  u32_blk_cnt;                /* 每个缓存池的缓存块个数，取值范围：(0, 10240]. */
    ENUM_VB_REMAP_MODE_E    e_remap_mode;               /* VB 的内核态虚拟地址映射模式 */
    AR_CHAR                 str_mmz_name[AR_MAX_MMZ_NAME_LEN + 1];     /* 当前缓存池从哪个MMZ区域分配内存 */
} STRU_VB_POOL_CONFIG_S;

typedef struct
{
    AR_U64                  u64_blk_size;               /* 缓存块大小，以Byte位单位. */
    AR_U32                  u32_blk_cnt;                /* 每个缓存池的缓存块个数，取值范围：(0, 10240]. */
    AR_U64* AR_ATTRIBUTE    u64_blk_addr;              /* 每个缓存池的缓存块的物理地址. */
    AR_VOID** AR_ATTRIBUTE  p_blk_vaddr;              /* 每个缓存池的缓存块的虚拟地址. */
    ENUM_VB_REMAP_MODE_E    e_remap_mode;               /* VB 的内核态虚拟地址映射模式 */
    AR_CHAR                 str_mmz_name[AR_MAX_MMZ_NAME_LEN + 1];     /* 当前缓存池从哪个MMZ区域分配内存 */
} STRU_VB_POOL_CONFIG_V2_S;


/**
* @note  定义视频缓存池属性结构体。
* @note  u64_blk_size 等于0或u32_blk_cnt等于0，则对应的缓存池不会被创建。
* @note  建议整个结构体先memset 为0 再按需赋值。
*/
typedef struct
{
    AR_U32                  u32_max_pool_cnt;           /* 整个系统中可容纳的缓存池个数，取值范围：(0, VB_MAX_POOLS]，静态属性，保留，目前内部固定取值VB_MAX_POOLS. */
    STRU_VB_POOL_CONFIG_S   st_comm_pool[AR_VB_MAX_COMM_POOLS];   /* 公共缓存池属性结构体，静态属性. */
} STRU_VB_CONFIG_S;

typedef struct
{
    AR_U32                  u32_is_comm_pool;
    AR_U32                  u32_blk_cnt;
    AR_U32                  u32_free_blk_cnt;
} STRU_VB_POOL_STATUS_S;

/**
* @note  定义VB 附加信息结构体。
* @note  当前支持4种附加信息，具体请参考ar_hal_vb_set_supplementconfig 接口描述。
*/
typedef struct
{
    AR_U32                  u32_supplement_config;      /* 附加信息控制. */
} STRU_VB_SUPPLEMENT_CONFIG_S;

typedef struct {
    AR_U64* AR_ATTRIBUTE      p_pool_blk_pa_list;
    AR_VOID** AR_ATTRIBUTE    p_pool_blk_va_list;
} STRU_AR_VB_POOL_V2_INFO;

/**
* @note  定义ioctl pool信息交换结构体。
*/
typedef struct {
    AR_U32                  u32_pool_id;
    AR_S32                  s32_pool_owner;
    AR_U32                  u32_blk_cnt;
#ifdef CONFIG_BIT_32BITS
    AR_U32                  u32_pool_version;
#endif
    AR_U64                  u64_blk_size;
    AR_U64                  u64_pool_size;
    AR_U64                  u64_pool_phy_addr;
    AR_VOID* AR_ATTRIBUTE   p_pool_vir_addr;
    AR_VOID* AR_ATTRIBUTE   p_pool_vir_addr_cache;
#ifndef CONFIG_BIT_32BITS
    AR_U32                  u32_pool_version;
#endif
    STRU_AR_VB_POOL_V2_INFO v2_info;
} STRU_AR_VB_GET_POOL_INFO;

/**
* @note  定义ioctl信息交换结构体。
*/
typedef struct {
    AR_VB_POOL              u32_pool_id;
    AR_VB_BLK               u32_block_id;
    AR_U64                  u64_blk_size;
    AR_U64                  u64_phy_addr;
    ENUM_VB_UID_E           e_vb_uid;
    AR_CHAR* AR_ATTRIBUTE   pstr_mmz_name;
    STRU_VB_CONFIG_S* AR_ATTRIBUTE pst_vb_cfg;
    STRU_VB_POOL_CONFIG_S* AR_ATTRIBUTE pst_vb_pool_cfg;
    STRU_VIDEO_SUPPLEMENT_S* AR_ATTRIBUTE pst_supplement;
}STRU_AR_VB_MSG;


//#pragma pack(pop)

/**
* @brief  创建一个视频缓冲池.
* @param  pst_vb_pool_cfg 缓存池配置属性参数指针.
* @retval 非VB_INVALID_POOLID 有效的缓存池ID 号, VB_INVALID_POOLID 创建缓存池失败，可能是参数非法或者保留内存不够.
* @note   必须先初始化缓存池，否则会失败
*/
AR_VB_POOL ar_hal_vb_create_pool(STRU_VB_POOL_CONFIG_S *pst_vb_pool_cfg);

/**
* @brief  销毁一个视频缓冲池.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @retval 0 成功 , 其它 失败.
* @note   销毁一个不存在的缓存池，则返回HAL_ERR_VB_UNEXIST
* @note   在去初始化视频缓存池时，所有的缓存池都将被销毁，包括用户态的缓存池
* @note   退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
* @note   [0, VB_MAX_POOLS)范围内的缓存池ID 号，包括公共缓存池、模块公共缓存
          池、模块私有缓存池等的ID 号。请确保pool_id为ar_hal_vb_create_pool所创建的
          缓存池的ID 号，否则会返回失败
* @note   如果当前缓存池有通过ar_hal_vb_mmap_pool接口映射虚拟地址，则必须先通过
          ar_hal_vb_munmap_pool接口解除映射，然后才能销毁缓存池
*/
AR_S32 ar_hal_vb_destroy_pool(AR_VB_POOL pool_id);

/**
* @brief  hal lib 层中获取一个缓存块.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @param  u64_blk_size 缓存块大小，取值范围：数据类型全范围，以byte为单位.
* @param  pstr_mmz_name 缓存池所在DDR 的名字.
* @param  e_vb_uid 申请缓存使用模块.
* @retval 非VB_INVALID_HANDLE 有效的缓存块句柄, VB_INVALID_HANDLE 获取缓存块失败，
* @note   用户可以在创建一个缓存池之后，调用本接口从该缓存池中获取一个缓存块；即
          将第1个参数pool_id设置为创建的缓存池ID；第2个参数u64_blk_size 须小于或等
          于创建该缓存池时指定的缓存块大小。从指定缓存池获取缓存块时，参数
          pstr_mmz_name 无效
* @note   如果用户需要从任意一个公共缓存池中获取一块指定大小的缓存块，则可以将第1
          个参数pool_id设置为无效ID 号（VB_INVALID_POOLID），将第2个参数
          u64_blk_size 设置为需要的缓存块大小，并指定要从哪个DDR上的公共缓存池获取
          缓存块。如果指定的DDR上并没有公共缓存池，那么将获取不到缓存块。如果
          pstr_mmz_name 等于NULL，则表示在没有命名的DDR上的公共缓存池获取缓存
          块
* @note   该函数主要用于hal lib 各个module 申请VB缓存块，应用程序请使用ar_hal_vb_get_block
*/
AR_VB_BLK ar_hal_vb_get_block_ex(AR_VB_POOL pool_id, AR_U64 u64_blk_size, const AR_CHAR *pstr_mmz_name, ENUM_VB_UID_E e_vb_uid);

/**
* @brief  用户态获取一个缓存块.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @param  u64_blk_size 缓存块大小，取值范围：数据类型全范围，以byte为单位.
* @param  pstr_mmz_name 缓存池所在DDR 的名字.
* @retval 非VB_INVALID_HANDLE 有效的缓存块句柄, VB_INVALID_HANDLE 获取缓存块失败，
* @note   用户可以在创建一个缓存池之后，调用本接口从该缓存池中获取一个缓存块；即
          将第1个参数pool_id设置为创建的缓存池ID；第2个参数u64_blk_size 须小于或等
          于创建该缓存池时指定的缓存块大小。从指定缓存池获取缓存块时，参数
          pstr_mmz_name 无效
* @note   如果用户需要从任意一个公共缓存池中获取一块指定大小的缓存块，则可以将第1
          个参数pool_id设置为无效ID 号（VB_INVALID_POOLID），将第2个参数
          u64_blk_size 设置为需要的缓存块大小，并指定要从哪个DDR上的公共缓存池获取
          缓存块。如果指定的DDR上并没有公共缓存池，那么将获取不到缓存块。如果
          pstr_mmz_name 等于NULL，则表示在没有命名的DDR上的公共缓存池获取缓存
          块
*/
AR_VB_BLK ar_hal_vb_get_block(AR_VB_POOL pool_id, AR_U64 u64_blk_size, const AR_CHAR *pstr_mmz_name);

/**
* @brief  hal lib 层中释放一个已经获取的缓存块.
* @param  block_id 缓存块句柄.
* @param  e_vb_uid 申请缓存使用模块.
* @retval 0 成功 , 其它 失败.
* @note   获取的缓存块使用完后，应该调用此接口释放缓存块
* @note   该函数主要用于hal lib 各个module 申请VB缓存块，应用程序请使用ar_hal_vb_release_block
*/
AR_S32 ar_hal_vb_release_block_ex(AR_VB_BLK block_id, ENUM_VB_UID_E e_vb_uid);

/**
* @brief  用户态释放一个已经获取的缓存块.
* @param  block_id 缓存块句柄.
* @retval 0 成功 , 其它 失败.
* @note   获取的缓存块使用完后，应该调用此接口释放缓存块
*/
AR_S32 ar_hal_vb_release_block(AR_VB_BLK block_id);

/**
* @brief  用户态通过缓存块的物理地址获取其句柄.
* @param  u64_phy_addr 缓存块物理地址.
* @retval 非负数 有效的缓存池句柄 , 负数 无效的缓存池句柄.
* @note   物理地址应该是从视频缓存池中获取的有效缓存块的地址
*/
AR_VB_BLK ar_hal_vb_physaddr2handle(AR_U64 u64_phy_addr);

/**
* @brief  用户态获取一个缓存块的物理地址.
* @param  block_id 缓存块句柄.
* @retval 0 无效返回值 , 其它 有效物理地址.
* @note   指定的缓存块应该是从视频缓存池中获取的有效缓存块
*/
AR_U64 ar_hal_vb_handle2physaddr(AR_VB_BLK block_id);

/**
* @brief  用户态获取一个帧缓存块所在缓存池的ID.
* @param  block_id 缓存块句柄.
* @retval 非负数 有效的缓存池ID 号 , 负数 无效的缓存池ID 号.
* @note   指定的缓存块应该是从视频缓存池中获取的有效缓存块
*/
AR_VB_POOL ar_hal_vb_handle2poolid(AR_VB_BLK block_id);

/**
* @brief  hal lib 中增加一个VB 的引用计数,请不要在应用程序中使用.
* @param  block_id 缓存块句柄.
* @param  e_uid 使用者的ID .
* @retval 0 成功 , 其它 失败.
* @note   指定的缓存块应该是从视频缓存池中获取的有效缓存块
* @note   该函数主要用于hal lib 各个module callback 增加对应VB 的引用计数，
          请不要在应用程序中使用
*/
AR_S32 ar_hal_vb_user_add(AR_VB_BLK block_id, ENUM_VB_UID_E e_uid);

/**
* @brief  hal lib 减少一个VB 的引用计数,请不要在应用程序中使用.
* @param  block_id 缓存块句柄.
* @param  e_uid 使用者的ID .
* @retval 0 成功 , 其它 失败.
* @note   指定的缓存块应该是从视频缓存池中获取的有效缓存块
* @note   该函数主要用于hal lib 各个module callback 减少对应VB 的引用计数，
          请不要在应用程序中使用
*/
AR_S32 ar_hal_vb_user_sub(AR_VB_BLK block_id, ENUM_VB_UID_E e_uid);


/**
* @brief  hal lib 中增加一个VB 的引用计数,请不要在应用程序中使用.
* @param  u64_phy_addr 缓存块物理地址.
* @param  e_uid 使用者的ID .
* @retval 0 成功 , 其它 失败.
* @note   指定的缓存块应该是从视频缓存池中获取的有效缓存块
* @note   该函数主要用于hal lib 各个module callback 增加对应VB 的引用计数，
          请不要在应用程序中使用
*/
AR_S32 ar_hal_vb_user_add_by_pa(AR_U64 u64_phy_addr, ENUM_VB_UID_E e_uid);

/**
* @brief  hal lib 减少一个VB 的引用计数,请不要在应用程序中使用.
* @param  u64_phy_addr 缓存块物理地址.
* @param  e_uid 使用者的ID .
* @retval 0 成功 , 其它 失败.
* @note   指定的缓存块物理地址应该是从视频缓存池中获取的有效缓存块
* @note   该函数主要用于hal lib 各个module callback 减少对应VB 的引用计数，
          请不要在应用程序中使用
*/
AR_S32 ar_hal_vb_user_sub_by_pa(AR_U64 u64_phy_addr, ENUM_VB_UID_E e_uid);

/**
* @brief  hal lib 中增加一个VB 的引用计数,请不要在应用程序中使用.
* @param  STRU_SYS_VIDEO_FRAME_INFO 使用VB分配的物理地址的帧信息.
* @param  e_uid 使用者的ID .
* @retval 0 成功 , 其它 失败.
* @note   帧信息的物理地址应该是从视频缓存池中获取的有效缓存块的物理地址
* @note   该函数主要用于hal lib 各个module callback 增加对应VB 的引用计数，
          请不要在应用程序中使用
*/
AR_S32 ar_hal_vb_user_add_by_frameinfo(STRU_SYS_VIDEO_FRAME_INFO * pst_frame_info, ENUM_VB_UID_E e_uid);

/**
* @brief  hal lib 减少一个VB 的引用计数,请不要在应用程序中使用.
* @param  STRU_SYS_VIDEO_FRAME_INFO 使用VB分配的物理地址的帧信息.
* @param  e_uid 使用者的ID .
* @retval 0 成功 , 其它 失败.
* @note   帧信息的物理地址应该是从视频缓存池中获取的有效缓存块的物理地址
* @note   该函数主要用于hal lib 各个module callback 减少对应VB 的引用计数，
          请不要在应用程序中使用
*/
AR_S32 ar_hal_vb_user_sub_by_frameinfo(STRU_SYS_VIDEO_FRAME_INFO * pst_frame_info, ENUM_VB_UID_E e_uid);


/**
* @brief  查询缓存块使用计数信息.
* @param  block_id 缓存块句柄.
* @retval 非VB_INVALID_HANDLE 有效的缓存块句柄, VB_INVALID_HANDLE 缓存块使用计数值.
* @note   指定的缓存块应该是从视频缓存池中获取的有效缓存块
*/
AR_S32 ar_hal_vb_inquire_usercnt(AR_VB_BLK block_id);

/**
* @brief  获取VB Block 内存的辅助信息.
* @param  block_id 缓存块句柄.
* @param  pst_supplement  VB Block 内存的辅助信息，如闪光帧类型、DCF信息等.
* @retval 0 成功 , 其它 失败.
* @note   pstSupplement保存的DCF地址信息为内核态虚拟地址
*/
AR_S32 ar_hal_vb_get_supplementaddr(AR_VB_BLK block_id, STRU_VIDEO_SUPPLEMENT_S *pst_supplement);

/**
* @brief  设置VB 内存的附加信息.
* @param  pst_supplement_config VB 内存附加信息控制结构体，用于附加信息分配内存.
* @retval 0 成功 , 其它 失败.
* @note   当前支持4种VB 内存附加信息，分别为
*           - DCF信息，对应结构体JPEG_DCF_S，对应的MASK为
*             VB_SUPPLEMENT_JPEG_MASK
*           − ISP 实时信息，对应结构体为ISP_FRAME_INFO_S，对应的MASK为
*             VB_SUPPLEMENT_ISPINFO_MASK
*           − 图像运动信息。无对应的结构体，对应的MASK为
*             VB_SUPPLEMENT_MOTION_DATA_MASK
*           − DNG 信息。对应结构体为DNG_IMAGE_DYNAMIC_INFO_S，对应的MASK
*             为VB_SUPPLEMENT_DNG_MASK
* @note   需要在ar_hal_vb_init前调用此接口，辅助信息才能生效
*/
AR_S32 ar_hal_vb_set_supplementconfig(const STRU_VB_SUPPLEMENT_CONFIG_S *pst_supplement_config);

/**
* @brief  获取VB 内存的附加信息.
* @param  pst_supplement_config VB 内存附加信息控制结构体，用于附加信息分配内存.
* @retval 0 成功 , 其它 失败.
* @note   无
*/
AR_S32 ar_hal_vb_get_supplementconfig(STRU_VB_SUPPLEMENT_CONFIG_S *pst_supplement_config);

/**
* @brief  初始化视频缓冲池.
* @param  无.
* @retval 0 成功 , 其它 失败.
* @note   必须先调用ar_hal_vb_set_config 设置视频缓存池属性，再初始化缓存池，否则会失败
* @note   可反复初始化，不返回失败
*/
AR_S32 ar_hal_vb_init(AR_VOID);

/**
* @brief  去初始化视频缓冲池.
* @param  无.
* @retval 0 成功 , 其它 失败.
* @note   可以反复去初始化，不返回失败
* @note   去初始化不会清除先前对缓存池的配置
* @note   退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
*/
AR_S32 ar_hal_vb_exit(AR_VOID);

/**
* @brief  配置视频缓冲池的属性.
* @param  pst_vb_config  视频缓冲池属性指针（静态属性）.
* @retval 0 成功 , 其它 失败.
* @note   只能在系统处于未初始化的状态下，才可以设置缓存池属性，否则会返回失败
* @note   video buf根据不同的应用场景需要不同的配置
*/
AR_S32 ar_hal_vb_set_config(const STRU_VB_CONFIG_S *pst_vb_config);

/**
* @brief  获取视频缓冲池的属性.
* @param  pst_vb_config  视频缓冲池属性指针（静态属性）.
* @retval 0 成功 , 其它 失败.
* @note   必须先调用ar_hal_vb_set_config 设置视频缓存池属性，再获取属性
*/
AR_S32 ar_hal_vb_get_config(STRU_VB_CONFIG_S *pst_vb_config);

/**
* @brief  获取一个视频缓存池信息.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @retval 非NULL 成功 , 其它 失败.
* @note   必须输入合法的缓存池ID
*/
STRU_AR_VB_GET_POOL_INFO * ar_hal_vb_get_pool_info(AR_VB_POOL pool_id);

/**
* @brief  为一个视频缓存池映射用户态虚拟地址.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @retval 0 成功 , 其它 失败.
* @note   必须输入合法的缓存池ID
* @note   重复映射视为成功
*/
AR_S32 ar_hal_vb_mmap_pool(AR_VB_POOL pool_id);

/**
* @brief  为一个视频缓存池解除用户态映射.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @retval 0 成功 , 其它 失败.
* @note   必须输入合法的缓存池ID
* @note   视频缓存池必须已经映射过
* @note   如果视频缓存池未映射，则直接返回成功
* @note   缓存池中的缓存块没有被占用，如果被占用，视为正在使用映射的
          用户态虚拟地址，返回失败
* @note   必须先释放虚拟地址，然后再销毁缓存池
*/
AR_S32 ar_hal_vb_munmap_pool(AR_VB_POOL pool_id);

/**
* @brief  获取一个视频缓存池中的缓存块的用户态虚拟地址.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @param  u64_phy_addr 缓存块的物理地址.
* @param  p_vir_addr 用户态虚拟地址.
* @retval 0 成功 , 其它 失败.
* @note   必须输入合法的缓存池ID、合法的缓存块物理地址
* @note   必须已调用ar_hal_vb_mmap_pool
* @note   如果物理地址不在当前VB 池范围内，则返回HAL_ERR_VB_ILLEGAL_PARAM
*/
AR_S32 ar_hal_vb_get_blockviraddr(AR_VB_POOL pool_id, AR_U64 u64_phy_addr, AR_VOID **p_vir_addr);

/**
* @brief  初始化模块公共视频缓存池.
* @param  e_vb_uid 使用模块公共视频缓冲池的模块ID.
* @retval 0 成功 , 其它 失败.
* @note   当前公共视频缓冲池仅适用于VDEC模块
* @note   必须先调用ar_hal_vb_init进行公共视频缓冲池初始化
* @note   必须先调用ar_hal_vb_set_modpoolconfig配置缓存池属性，再初始化缓存池，否
          则会失败
* @note   可反复初始化，不返回失败
* @note   VDEC 模块公共池仅在解码帧存分配方式使用模块公共VB 池时才需要创建
*/
AR_S32 ar_hal_vb_init_modcommpool(ENUM_VB_UID_E e_vb_uid);

/**
* @brief  去初始化视频缓冲池.
* @param  无.
* @retval 0 成功 , 其它 失败.
* @note   可以反复去初始化，不返回失败
* @note   去初始化不会清除先前对缓存池的配置
* @note   退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
*/
AR_S32 ar_hal_vb_exit_modcommpool(ENUM_VB_UID_E e_vb_uid);

/**
* @brief  设置模块公共视频缓存池属性.
* @param  e_vb_uid 使用模块公共视频缓冲池的模块ID.
* @param  pst_vb_config 模块公共视频缓存池属性指针（静态属性）.
* @retval 0 成功 , 其它 失败.
* @note   模块公共视频缓冲区的配置根据实际需要配置，否则会造成内存浪费
* @note   如果模块VB 已创建，再次配置返回错误
*/
AR_S32 ar_hal_vb_set_modpoolconfig(ENUM_VB_UID_E e_vb_uid, const STRU_VB_CONFIG_S *pst_vb_config);

/**
* @brief  获取模块公共视频缓存池属性.
* @param  e_vb_uid 使用模块公共视频缓冲池的模块ID.
* @param  pst_vb_config 模块公共视频缓存池属性指针（静态属性）.
* @retval 0 成功 , 其它 失败.
* @note   必须先调用ar_hal_vb_set_modpoolconfig设置模块公共视频缓存池属性，再获取属性
*/
AR_S32 ar_hal_vb_get_modpoolconfig(ENUM_VB_UID_E e_vb_uid, STRU_VB_CONFIG_S *pst_vb_config);

/**
* @brief  创建一个大图模式下专用的视频缓冲池.
* @param  pst_vb_pool_cfg 缓存池配置属性参数指针，需要传入分配好的物理地址列表和虚拟地址列表.
* @retval 非VB_INVALID_POOLID 有效的缓存池ID 号, VB_INVALID_POOLID 创建缓存池失败，可能是参数非法或者保留内存不够.
* @note   必须先初始化缓存池，否则会失败，改池分配的block无法跨进程。
*/
AR_VB_POOL ar_hal_vb_create_pool_v2(STRU_VB_POOL_CONFIG_V2_S *pst_vb_pool_cfg);

/**
* @brief  销毁一个大图模式下专用的视频缓冲池.
* @param  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
* @retval 0 成功 , 其它 失败.
* @note   销毁一个不存在的缓存池，则返回HAL_ERR_VB_UNEXIST
* @note   在去初始化视频缓存池时，所有的缓存池都将被销毁，包括用户态的缓存池
* @note   退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
* @note   [0, VB_MAX_POOLS)范围内的缓存池ID 号，包括公共缓存池、模块公共缓存
          池、模块私有缓存池等的ID 号。请确保pool_id为ar_hal_vb_create_pool所创建的
          缓存池的ID 号，否则会返回失败
* @note   如果当前缓存池有通过ar_hal_vb_mmap_pool接口映射虚拟地址，则必须先通过
          ar_hal_vb_munmap_pool接口解除映射，然后才能销毁缓存池
*/
AR_S32 ar_hal_vb_destroy_pool_v2(AR_VB_POOL pool_id);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __HAL_VB_H__ */

