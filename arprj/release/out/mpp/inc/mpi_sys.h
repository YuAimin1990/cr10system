/**
 * \file
 * \brief 描述系统相关的数据结构和接口.
 */

#ifndef __MPI_SYS_H__
#define __MPI_SYS_H__

#include "hal_type.h"
#include "ar_common.h"
#include "ar_comm_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/**
\addtogroup MPI_SYS
 * @brief 系统控制根据芯片特性，完成硬件各个部件的复位、基本初始化工作，同时负责完成系统
 各个业务模块的初始化、去初始化以及管理系统各个业务模块的工作状态、提供当前系统的版本信
息、提供大块物理内存管理等功能。
 * @{
*/


/**
\brief      初始化 MPP 系统。包括音频输入输出、视频输入输出、视频编解码、视频叠加区域、视频处理、图形处理等模块都会被初始化.
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_Init(AR_VOID);

/**
\brief      去初始化 MPP 系统。包括音频输入输出、视频输入输出、视频编解码、视频叠加区 域、视频处理、图形处理等模块都会被销毁或者禁用.
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_Exit(AR_VOID);

/**
\brief      配置系统控制参数.
@note       未实现
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_SetConfig(const MPP_SYS_CONFIG_S* pstSysConfig);

/**
\brief      配置系统控制参数.
@note       未实现
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetConfig(MPP_SYS_CONFIG_S* pstSysConfig);

/**
\brief     数据源到数据接收者绑定接口.
\param[in] pstSrcChn  源通道指针
\param[in] pstDestChn  目的通道指针
\retval     0 成功 , 其它 失败.
*/
AR_S32  AR_MPI_SYS_Bind(const MPP_CHN_S* pstSrcChn, const MPP_CHN_S* pstDestChn);

/**
\brief     数据源到数据接收者解绑定接口.
\param[in] pstSrcChn  源通道指针
\param[in] pstDestChn  目的通道指针
\retval     0 成功 , 其它 失败.
*/
AR_S32  AR_MPI_SYS_UnBind(const MPP_CHN_S* pstSrcChn, const MPP_CHN_S* pstDestChn);

/**
\brief     获取此通道上绑定的源通道的信息.
\param[in] pstSrcChn  源通道指针
\param[in] pstDestChn 目的通道指针
\retval     0 成功 , 其它 失败.
*/
AR_S32  AR_MPI_SYS_GetBindbyDest(const MPP_CHN_S* pstDestChn, MPP_CHN_S* pstSrcChn);

/**
\brief     获取此通道上绑定的源通道的信息.
\param[in] pstSrcChn  源通道指针
\param[in] pstBindDest  绑定的目的指针
\retval     0 成功 , 其它 失败.
*/
AR_S32  AR_MPI_SYS_GetBindbySrc(const MPP_CHN_S *pstSrcChn, MPP_BIND_DEST_S *pstBindDest);

/**
\brief      获取MPP版本号.
\param[out] pstVersion  版本号描述指针
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetVersion(MPP_VERSION_S* pstVersion);

/**
\brief      获取当前芯片的 ID.
\param[out] pu32ChipId  芯片 ID 指针
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetChipId(AR_U32 *pu32ChipId);

/**
\brief      获取当前芯片的 CustomCode.
@note       未实现
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetCustomCode(AR_U32 *pu32CustomCode);

/**
\brief      获取当前的时间戳.
\param[out] pu64CurPTS  当前时间戳指针
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetCurPTS(AR_U64* pu64CurPTS);

/**
\brief     初始化 MPP 的时间戳基准.
\param[in] u64_pts_base 时间戳基准。单位：微秒.
\retval    0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_InitPTSBase(AR_U64 u64PTSBase);

/**
\brief 同步 MPP 的时间戳.
\param[in] u64_pts_base 时间戳基准。单位：微秒.
\retval    0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_SyncPTS(AR_U64 u64PTSBase);

/**
\brief      在用户态分配MMZ内存.
\param[out] pu64PhyAddr 分配的物理地址指针.
\param[out] ppVirAddr 指向分配的虚拟地址指针的指针.
\param[in]  strMmb Mmb 名称的字符串指针.
\param[in]  strZone MMZ zone 名称的字符串指针.
\param[in]  u32Len 内存块大小.
\retval     0 成功 , 其它 失败.
\remarks    MMZ分为许多区域（Zone），每个区域下有多个Mmb，调用此接口在MMZ的名为
      *pstr_zone 的区域中分配一个名为*pstr_mmb 的内存块，大小为u32_len，并返回物理地
      址和用户态虚拟地址指针。如果MMZ中有名为anonymous的区域，*pstr_zone可设为
      NULL。如果*pstr_mmb 设为NULL，创建的Mmb 分块名为“<null>”
*/
AR_S32 AR_MPI_SYS_MmzAlloc(AR_U64* pu64PhyAddr, AR_VOID** ppVirAddr,
                           const AR_CHAR* strMmb, const AR_CHAR* strZone, AR_U32 u32Len);

/**
\brief      在用户态分配MMZ内存，该内存支持cache缓存.
\param[out] pu64PhyAddr 分配的物理地址指针.
\param[out] ppVirAddr 指向分配的虚拟地址指针的指针.
\param[in]  pstrMmb Mmb 名称的字符串指针.
\param[in]  pstrZone MMZ zone 名称的字符串指针.
\param[in]  u32Len 内存块大小.
\retval     0 成功 , 其它 失败.
\remarks    本接口与ar_hal_sys_mmz_alloc接口的区别：通过本接口分配的内存支持cache
      缓存，对于频繁使用的内存，最好使用本接口分配内存，这样可以提高cpu读写
      的效率，提升系统性能，如用户在使用ive算子时，就存在大量数据频繁读写，此
      时使用此接口来分配内存，就能很好的提高cpu 的效率
   	  当cpu访问此接口分配的内存时，会将内存中的数据放在cache 中，而硬件设备
      只能访问物理内存，不能访问cache 的内容，对于这种cpu和硬件会共同
      操作的内存，需调用ar_hal_sys_mmz_flush_cache做好数据同步
*/
AR_S32 AR_MPI_SYS_MmzAlloc_Cached(AR_U64* pu64PhyAddr, AR_VOID** ppVirAddr,
                                  const AR_CHAR* pstrMmb, const AR_CHAR* pstrZone, AR_U32 u32Len);

/**
\brief      在用户态释放MMZ内存.
\param[in]  u64PhyAddr 物理地址.
\param[in]  pVirAddr 虚拟地址指针..
\retval     0 成功 , 其它 失败.
\remarks    输入的地址必须为有效的物理地址，虚拟地址指针可以置为NULL
      不能释放正在进行flush操作的内存，否则会引发不可预知的异常
*/
AR_S32 AR_MPI_SYS_MmzFree(AR_U64 u64PhyAddr, AR_VOID* pVirAddr);

/**
\brief     刷新 cache里的内容到内存并且使cache里的内容无效.
\param[in] u64PhyAddr 待操作数据的起始物理地址.
\param[in] pVirAddr 待操作数据的起始虚拟地址指针，不能传NULL.
\param[in] u32Size 待操作数据的大小.
\retval    0 成功 , 其它 失败.
\remarks   当 cache里的数据为最新数据时，为了保证不能直接访问cache 的硬件在访
      问内存时能够得到正确的数据，此时需要先调用此接口将cache里的内容更新到
      内存，这样，当硬件访问内存时，保证了数据的一致性和正确性
      此接口应与ar_hal_sys_mmz_alloc_cached接口配套使用
      若将u32PhyAddr 设为0，则表示操作整个cache 区域
      用户需要确保传入参数的合法性
      用户必须保证进行flush操作时不能调用ar_hal_sys_mmz_free接口释放当前
      flush 的内存，否则会引发不可预知的异常
*/
AR_S32 AR_MPI_SYS_MmzFlushCache(AR_U64 u64PhyAddr, AR_VOID* pVirAddr, AR_U32 u32Size);

/**
\brief     memory存储映射接口.
\param[in] u64PhyAddr 需映射的内存单元起始地址.
\param[in] u32Size 映射的字节数。取值范围为（0, 0xFFFFF000）.
\retval    0 无效地址 , 其它 有效地址.
\remarks   open的节点是dev/mmz_userdev，在MMZ中实现映射非device类型（不可cache、可reorder）的DDR 地址
	  输入的地址需为合法的物理地址,对应的解映射接口为ar_hal_sys_munmap
*/
AR_VOID* AR_MPI_SYS_Mmap(AR_U64 u64PhyAddr, AR_U32 u32Size);

/**
\brief     存储映射接口，映射成cache属性.
\param[in] u64PhyAddr 需映射的内存单元起始地址.
\param[in] u32Size 映射的字节数。取值范围为（0, 0xFFFFF000）.
\retval    0 无效地址 , 其它 有效地址.
\remarks 输入的地址需为合法的物理地址
*/
AR_VOID* AR_MPI_SYS_MmapCache(AR_U64 u64PhyAddr, AR_U32 u32Size);

/**
\brief     存储反映射接口.
\param[in] u64_phy_addr 需映射的内存单元起始地址.
\param[in] u32Size 映射的字节数。取值范围为（0, 0xFFFFF000）.
\retval 0 无效地址 , 其它 有效地址.
\remarks  等同于系统munmap 函数
*/
AR_S32 AR_MPI_SYS_Munmap(AR_VOID* pVirAddr, AR_U32 u32Size);

/**
\brief     刷新 cache里的内容到内存并且使cache里的内容无效.
\param[in] u64PhyAddr 待操作数据的起始物理地址，需要 4字节对齐.
\param[in] pVirAddr 待操作数据的起始虚拟地址指针，必须为用户态地址，
           且不能为空，虚拟地址必须为物理地址映射的地址.
\param[in] u32Size 待操作数据的大小，不能为0.
\retval    0 无效地址 , 其它 有效地址.
\remarks   此接口应与ar_hal_sys_mmap_cache接口配套使用,用户需要确保传入参数的合法性
*/
AR_S32 AR_MPI_SYS_MflushCache(AR_U64 u64PhyAddr, AR_VOID *pVirAddr, AR_U32 u32Size);

/**
\brief      配置内存参数，设置模块设备通道使用的 MMZ 区域名称.
@note       未实现
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_SetMemConfig(const MPP_CHN_S* pstMppChn, const AR_CHAR* pcMmzName);

/**
\brief      获取模块设备通道使用的 MMZ 区域名称.
@note       未实现
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetMemConfig(const MPP_CHN_S* pstMppChn, AR_CHAR* pcMmzName);

/**
\brief      关闭所有 SYS 打开的日志、系统、内存 Fd.
@note       未实现
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_CloseFd(AR_VOID);

/**
\brief      根据虚拟地址获取对应的内存信息，包括物理地址及 cached 属性.
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetVirMemInfo(const void* pVirAddr, SYS_VIRMEM_INFO_S* pstMemInfo);

/* Set/get Scale coefficient level for VPSS/VGS*/
AR_S32 AR_MPI_SYS_SetScaleCoefLevel(const SCALE_RANGE_S *pstScaleRange, const SCALE_COEFF_LEVEL_S *pstScaleCoeffLevel);
AR_S32 AR_MPI_SYS_GetScaleCoefLevel(const SCALE_RANGE_S *pstScaleRange, SCALE_COEFF_LEVEL_S *pstScaleCoeffLevel);

/**
\brief      设置时区信息.(Set/Get local timezone, range: [-86400, 86400] seconds (that is: [-24, 24] hours))
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_SetTimeZone(AR_S32 s32TimeZone);

/**
\brief      获取时区信息.
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetTimeZone(AR_S32 *ps32TimeZone);

/**
\brief      设置 GPS 信息.
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_SetGPSInfo(const GPS_INFO_S *pstGPSInfo);

/**
\brief      获取 GPS 信息.
\retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_SYS_GetGPSInfo(GPS_INFO_S *pstGPSInfo);

AR_S32 AR_MPI_SYS_SetTuningConnect(AR_S32 s32Connect);
AR_S32 AR_MPI_SYS_GetTuningConnect(AR_S32* ps32Connect);


AR_S32 AR_MPI_SYS_SetVIVPSSMode(const VI_VPSS_MODE_S* pstVIVPSSMode);
AR_S32 AR_MPI_SYS_GetVIVPSSMode(VI_VPSS_MODE_S* pstVIVPSSMode);

/** @} */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_SYS_H__ */

