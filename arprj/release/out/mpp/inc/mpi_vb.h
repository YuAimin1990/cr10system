#ifndef __MPI_VB_H__
#define __MPI_VB_H__

#include "ar_comm_vb.h"
#include "ar_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/**
\brief      创建一个视频缓冲池.
\param[in]  pst_vb_pool_cfg 缓存池配置属性参数指针.
\retvall    非VB_INVALID_POOLID 有效的缓存池ID 号, VB_INVALID_POOLID 创建缓存池失败，可能是参数非法或者保留内存不够.
\remarks    必须先初始化缓存池，否则会失败
*/
VB_POOL AR_MPI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);

/**
\brief      销毁一个视频缓冲池.
\param[in]  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
\retvall    0 成功 , 其它 失败.
\remarks    销毁一个不存在的缓存池，则返回HAL_ERR_VB_UNEXIST
\remarks    在去初始化视频缓存池时，所有的缓存池都将被销毁，包括用户态的缓存池
\remarks    退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
\remarks    [0, VB_MAX_POOLS)范围内的缓存池ID 号，包括公共缓存池、模块公共缓存
            池、模块私有缓存池等的ID 号。请确保pool_id为ar_hal_vb_create_pool所创建的
            缓存池的ID 号，否则会返回失败
\remarks    如果当前缓存池有通过ar_hal_vb_mmap_pool接口映射虚拟地址，则必须先通过
            ar_hal_vb_munmap_pool接口解除映射，然后才能销毁缓存池
*/
AR_S32 AR_MPI_VB_DestroyPool(VB_POOL Pool);

/**
\brief      用户态获取一个缓存块.
\param[in]  pool_id 缓存池ID 号，取值范围：[0, VB_MAX_POOLS).
\param[in]  u64_blk_size 缓存块大小，取值范围：数据类型全范围，以byte为单位.
\param[in]  pstr_mmz_name 缓存池所在DDR 的名字.
\retvall    非VB_INVALID_HANDLE 有效的缓存块句柄, VB_INVALID_HANDLE 获取缓存块失败，
\remarks    用户可以在创建一个缓存池之后，调用本接口从该缓存池中获取一个缓存块；即
            将第1个参数pool_id设置为创建的缓存池ID；第2个参数u64_blk_size 须小于或等
            于创建该缓存池时指定的缓存块大小。从指定缓存池获取缓存块时，参数
            pstr_mmz_name 无效
\remarks    如果用户需要从任意一个公共缓存池中获取一块指定大小的缓存块，则可以将第1
            个参数pool_id设置为无效ID 号（VB_INVALID_POOLID），将第2个参数
            u64_blk_size 设置为需要的缓存块大小，并指定要从哪个DDR上的公共缓存池获取
            缓存块。如果指定的DDR上并没有公共缓存池，那么将获取不到缓存块。如果
            pstr_mmz_name 等于NULL，则表示在没有命名的DDR上的公共缓存池获取缓存
            块
*/
VB_BLK AR_MPI_VB_GetBlock(VB_POOL Pool, AR_U64 u64BlkSize,const AR_CHAR *pcMmzName);

/**
\brief      用户态释放一个已经获取的缓存块.
\param[in]  block_id 缓存块句柄.
\retvall 0  成功 , 其它 失败.
\remarks    获取的缓存块使用完后，应该调用此接口释放缓存块
*/
AR_S32 AR_MPI_VB_ReleaseBlock(VB_BLK Block);

/**
\brief      用户态通过缓存块的物理地址获取其句柄.
\param[in]  u64_phy_addr 缓存块物理地址.
\retvall    非负数 有效的缓存池句柄 , 负数 无效的缓存池句柄.
\remarks    物理地址应该是从视频缓存池中获取的有效缓存块的地址
*/
VB_BLK AR_MPI_VB_PhysAddr2Handle(AR_U64 u64PhyAddr);

/**
\brief      用户态获取一个缓存块的物理地址.
\param[in]  block_id 缓存块句柄.
\retvall    0 无效返回值 , 其它 有效物理地址.
\remarks    指定的缓存块应该是从视频缓存池中获取的有效缓存块
*/
AR_U64 AR_MPI_VB_Handle2PhysAddr(VB_BLK Block);

/**
\brief      用户态获取一个帧缓存块所在缓存池的ID.
\param[in]  block_id 缓存块句柄.
\retvall    非负数 有效的缓存池ID 号 , 负数 无效的缓存池ID 号.
\remarks    指定的缓存块应该是从视频缓存池中获取的有效缓存块
*/
VB_POOL AR_MPI_VB_Handle2PoolId(VB_BLK Block);

/**
\brief      查询缓存块使用计数信息.
\param[in]  block_id 缓存块句柄.
\retvall    非VB_INVALID_HANDLE 有效的缓存块句柄, VB_INVALID_HANDLE 缓存块使用计数值.
\remarks    指定的缓存块应该是从视频缓存池中获取的有效缓存块
*/
AR_S32 AR_MPI_VB_InquireUserCnt(VB_BLK Block);

/**
\brief      获取VB Block 内存的辅助信息.
\param[in]  block_id 缓存块句柄.
\param[out] pst_supplement  VB Block 内存的辅助信息，如闪光帧类型、DCF信息等.
\retvall    0 成功 , 其它 失败.
\remarks    pstSupplement保存的DCF地址信息为内核态虚拟地址
*/
AR_S32 AR_MPI_VB_GetSupplementAddr(VB_BLK Block, VIDEO_SUPPLEMENT_S *pstSupplement);

/**
\brief  设置VB 内存的附加信息.
\param[in]  pst_supplement_config VB 内存附加信息控制结构体，用于附加信息分配内存.
\retvall 0 成功 , 其它 失败.
\remarks   当前支持4种VB 内存附加信息，分别为
\           - DCF信息，对应结构体JPEG_DCF_S，对应的MASK为
\             VB_SUPPLEMENT_JPEG_MASK
\           − ISP 实时信息，对应结构体为ISP_FRAME_INFO_S，对应的MASK为
\             VB_SUPPLEMENT_ISPINFO_MASK
\           − 图像运动信息。无对应的结构体，对应的MASK为
\             VB_SUPPLEMENT_MOTION_DATA_MASK
\           − DNG 信息。对应结构体为DNG_IMAGE_DYNAMIC_INFO_S，对应的MASK
\             为VB_SUPPLEMENT_DNG_MASK
\remarks   需要在ar_hal_vb_init前调用此接口，辅助信息才能生效
*/
AR_S32 AR_MPI_VB_SetSupplementConfig(const VB_SUPPLEMENT_CONFIG_S *pstSupplementConfig);

/**
\brief      获取VB 内存的附加信息.
\param[out] pst_supplement_config VB 内存附加信息控制结构体，用于附加信息分配内存.
\retvall    0 成功 , 其它 失败.
\remarks    无
*/
AR_S32 AR_MPI_VB_GetSupplementConfig(VB_SUPPLEMENT_CONFIG_S *pstSupplementConfig);

/**
\brief      初始化视频缓冲池.
\param      无.
\retvall    0 成功 , 其它 失败.
\remarks    必须先调用ar_hal_vb_set_config 设置视频缓存池属性，再初始化缓存池，否则会失败
\remarks    可反复初始化，不返回失败
*/
AR_S32 AR_MPI_VB_Init(AR_VOID);

/**
\brief      去初始化视频缓冲池.
\param      无.
\retvall    0 成功 , 其它 失败.
\remarks    可以反复去初始化，不返回失败
\remarks    去初始化不会清除先前对缓存池的配置
\remarks    退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
*/
AR_S32 AR_MPI_VB_Exit(AR_VOID);

/**
\brief     设置 MPP 视频缓存池属性.
\param[in] pstVbPoolCfg 视频缓存池属性指针.静态属性.
\retval     0 成功 , 其它 失败.
\remarks   只能在系统处于未初始化的状态下，才可以设置缓存池属性，否则会返回失败
\remarks   video buf 根据不同的应用场景需要不同的配置
\remarks   公共缓存池中每个缓存块的大小应根据当前图像像素格式以及图像是否压缩而有所不同
*/
AR_S32 AR_MPI_VB_SetConfig(const VB_CONFIG_S *pstVbConfig);

/**
\brief     获取 MPP 视频缓存池属性.
\param[in] pstVbPoolCfg 视频缓存池属性指针.静态属性.
\retval     0 成功 , 其它 失败.
\remarks   必须先调用 AR_MPI_VB_SetConfig 设置 MPP 视频缓存池属性，再获取属性
*/
AR_S32 AR_MPI_VB_GetConfig(VB_CONFIG_S *pstVbConfig);

/**
\brief     为一个视频缓存池映射用户态虚拟地址.
\param[in] Pool 缓存池 ID 号.取值范围：[0, VB_MAX_POOLS).
\retval     0 成功 , 其它 失败.
\remarks   必须输入合法的缓存池 ID
\remarks   重复映射视为成功
*/
AR_S32 AR_MPI_VB_MmapPool(VB_POOL Pool);

/**
\brief     为一个视频缓存池解除用户态映射.
\param[in] Pool 缓存池 ID 号.取值范围：[0, VB_MAX_POOLS).
\retval     0 成功 , 其它 失败.
\remarks   必须输入合法的缓存池 ID
\remarks   视频缓存池必须已经映射过
\remarks   如果视频缓存池未映射，则直接返回成功
\remarks   缓存池中的缓存块没有被 MPI 层占用，如果被 MPI 层占用，视为正在使用映射的用户态虚拟地址，返回失败
\remarks   必须先释放虚拟地址，然后再销毁缓存池
*/
AR_S32 AR_MPI_VB_MunmapPool(VB_POOL Pool);

/**
\brief     为一个视频缓存池映射用户态虚拟地址.
\param[in] Pool 缓存池 ID 号.取值范围：[0, VB_MAX_POOLS).
\param[in] u64PhyAddr 缓存块的物理地址.
\param[out] ppVirAddr 用户态虚拟地址.
\retval     0 成功 , 其它 失败.
\remarks   必须输入合法的缓存池 ID
\remarks   必须已调用 AR_MPI_VB_MmapPool
\remarks   如果物理地址不在当前 VB 池范围内，则返回失败
*/
AR_S32 AR_MPI_VB_GetBlockVirAddr(VB_POOL Pool, AR_U64 u64PhyAddr, AR_VOID **ppVirAddr);

/**
\brief      初始化模块公共视频缓存池.
\param[in]  e_vb_uid 使用模块公共视频缓冲池的模块ID.
\retval     0 成功 , 其它 失败.
\remarks    当前公共视频缓冲池仅适用于VDEC模块
\remarks    必须先调用AR_MPI_VB_Init进行公共视频缓冲池初始化
\remarks    必须先调用AR_MPI_VB_SetModPoolConfig配置缓存池属性，再初始化缓存池，否          则会失败
\remarks    可反复初始化，不返回失败
\remarks    VDEC 模块公共池仅在解码帧存分配方式使用模块公共VB 池时才需要创建
*/
AR_S32 AR_MPI_VB_InitModCommPool(VB_UID_E enVbUid);

/**
\brief      去初始化视频缓冲池.
\param[in]  e_vb_uid 使用模块公共视频缓冲池的模块ID.
\retval     0 成功 , 其它 失败.
\remarks    可以反复去初始化，不返回失败
\remarks    去初始化不会清除先前对缓存池的配置
\remarks    退出VB 池之前请确保VB 池里的任何VB 都没有被占用，否则无法退出
*/
AR_S32 AR_MPI_VB_ExitModCommPool(VB_UID_E enVbUid);

/**
\brief      设置模块公共视频缓存池属性.
\param[in]  e_vb_uid 使用模块公共视频缓冲池的模块ID.
\param[in]  pst_vb_config 模块公共视频缓存池属性指针（静态属性）.
\retval     0 成功 , 其它 失败.
\remarks    模块公共视频缓冲区的配置根据实际需要配置，否则会造成内存浪费
\remarks    如果模块VB 已创建，再次配置返回错误
*/
AR_S32 AR_MPI_VB_SetModPoolConfig(VB_UID_E enVbUid, const VB_CONFIG_S *pstVbConfig);

/**
\brief      获取模块公共视频缓存池属性.
\param[in]  e_vb_uid 使用模块公共视频缓冲池的模块ID.
\param[out] pst_vb_config 模块公共视频缓存池属性指针（静态属性）.
\retval     0 成功 , 其它 失败.
\remarks    必须先调用AR_MPI_VB_SetModPoolConfig设置模块公共视频缓存池属性，再获取属性
*/
AR_S32 AR_MPI_VB_GetModPoolConfig(VB_UID_E enVbUid, VB_CONFIG_S *pstVbConfig);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_VI_H__ */

