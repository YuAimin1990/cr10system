/**
 * \file
 * \brief 描述系统控制模块相关的通用数据结构
 */

#ifndef __AR_COMM_SYS_H__
#define __AR_COMM_SYS_H__

#include "hal_type.h"
#include "hal_errno.h"
#include "ar_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define BIND_DEST_MAXNUM 128

#define AR_TRACE_SYS(level, fmt, ...)\
do{ \
    AR_TRACE(level, AR_ID_SYS,"[Func]:%s [Line]:%d [Info]:"fmt,__FUNCTION__, __LINE__,##__VA_ARGS__);\
}while(0)


typedef struct arMPP_SYS_CONFIG_S
{
    AR_U32 u32Align;

} MPP_SYS_CONFIG_S;

typedef struct arSYS_VIRMEM_INFO_S
{
    AR_U64  u64PhyAddr;
    AR_BOOL bCached;
} SYS_VIRMEM_INFO_S;

typedef struct arMPP_BIND_DEST_S
{
    AR_U32    u32Num;
    MPP_CHN_S astMppChn[BIND_DEST_MAXNUM];
}MPP_BIND_DEST_S;


typedef enum arSCALE_RANGE_E
{
    SCALE_RANGE_0 = 0,      /* scale range <   8/64 */
    SCALE_RANGE_1,          /* scale range >=  8/64 */
    SCALE_RANGE_2,          /* scale range >= 10/64 */
    SCALE_RANGE_3,          /* scale range >= 15/64 */
    SCALE_RANGE_4,          /* scale range >= 19/64 */
    SCALE_RANGE_5,          /* scale range >= 24/64 */
    SCALE_RANGE_6,          /* scale range >= 29/64 */
    SCALE_RANGE_7,          /* scale range >= 33/64 */
    SCALE_RANGE_8,          /* scale range >= 35/64 */
    SCALE_RANGE_9,          /* scale range >= 38/64 */
    SCALE_RANGE_10,         /* scale range >= 42/64 */
    SCALE_RANGE_11,         /* scale range >= 45/64 */
    SCALE_RANGE_12,         /* scale range >= 48/64 */
    SCALE_RANGE_13,         /* scale range >= 51/64 */
    SCALE_RANGE_14,         /* scale range >= 53/64 */
    SCALE_RANGE_15,         /* scale range >= 55/64 */
    SCALE_RANGE_16,         /* scale range >= 57/64 */
    SCALE_RANGE_17,         /* scale range >= 60/64 */
    SCALE_RANGE_18,         /* scale range >  1     */
    SCALE_RANGE_BUTT,
} SCALE_RANGE_E;


typedef enum arCOEFF_LEVEL_E
{
    COEFF_LEVEL_0 = 0,      /* coefficient level 0 */
    COEFF_LEVEL_1,          /* coefficient level 1 */
    COEFF_LEVEL_2,          /* coefficient level 2 */
    COEFF_LEVEL_3,          /* coefficient level 3 */
    COEFF_LEVEL_4,          /* coefficient level 4 */
    COEFF_LEVEL_5,          /* coefficient level 5 */
    COEFF_LEVEL_6,          /* coefficient level 6 */
    COEFF_LEVEL_7,          /* coefficient level 7 */
    COEFF_LEVEL_8,          /* coefficient level 8 */
    COEFF_LEVEL_9,          /* coefficient level 9 */
    COEFF_LEVEL_10,         /* coefficient level 10 */
    COEFF_LEVEL_11,         /* coefficient level 11 */
    COEFF_LEVEL_12,         /* coefficient level 12 */
    COEFF_LEVEL_13,         /* coefficient level 13 */
    COEFF_LEVEL_14,         /* coefficient level 14 */
    COEFF_LEVEL_15,         /* coefficient level 15 */
    COEFF_LEVEL_16,         /* coefficient level 16 */
    COEFF_LEVEL_17,         /* coefficient level 17 */
    COEFF_LEVEL_18,         /* coefficient level 18 */
    COEFF_LEVEL_BUTT,
} COEFF_LEVEL_E;



typedef struct arSCALE_COEFF_LEVEL_S
{
    COEFF_LEVEL_E enHorLum; /* horizontal luminance   coefficient level */
    COEFF_LEVEL_E enHorChr; /* horizontal chrominance coefficient level */
    COEFF_LEVEL_E enVerLum; /* vertical   luminance   coefficient level */
    COEFF_LEVEL_E enVerChr; /* vertical   chrominance coefficient level */
} SCALE_COEFF_LEVEL_S;

typedef struct arSCALE_RANGE_S
{
    SCALE_RANGE_E enHorizontal;
    SCALE_RANGE_E enVertical;
} SCALE_RANGE_S;

typedef struct arSCALE_COEFF_INFO_S
{
    SCALE_RANGE_S stScaleRange;
    SCALE_COEFF_LEVEL_S stScaleCoeffLevel;
} SCALE_COEFF_INFO_S;

typedef struct arGPS_INFO_S
{
    AR_CHAR  chGPSLatitudeRef;         /* GPS LatitudeRef Indicates whether the latitude is north or south latitude,
                                    * 'N'/'S', default 'N' */
    AR_U32   au32GPSLatitude[3][2];    /* GPS Latitude is expressed as degrees, minutes and seconds,a typical format like "dd/1,mm/1,ss/1",
                                    *  default 0/0, 0/0, 0/0 */
    AR_CHAR  chGPSLongitudeRef;        /* GPS LongitudeRef Indicates whether the longitude is east or west longitude,
                                    * 'E'/'W', default 'E' */
    AR_U32   au32GPSLongitude[3][2];   /* GPS Longitude is expressed as degrees, minutes and seconds,a typical format like "dd/1,mm/1,ss/1",
                                    *  default 0/0, 0/0, 0/0 */
    AR_U8    u8GPSAltitudeRef;         /* GPS AltitudeRef Indicates the reference altitude used, 0 - above sea level, 1 - below sea level
                                    * default 0 */
    AR_U32   au32GPSAltitude[2];       /* GPS AltitudeRef Indicates the altitude based on the reference u8GPSAltitudeRef, the reference unit is meters,
                                    * default 0/0 */
} GPS_INFO_S;




typedef enum arVI_VPSS_MODE_E
{
    VI_OFFLINE_VPSS_OFFLINE = 0,
    VI_OFFLINE_VPSS_ONLINE,
    VI_ONLINE_VPSS_OFFLINE,
    VI_ONLINE_VPSS_ONLINE,
    VI_PARALLEL_VPSS_OFFLINE,
    VI_PARALLEL_VPSS_PARALLEL,
    VI_VPSS_MODE_BUTT
} VI_VPSS_MODE_E;


typedef struct arVI_VPSS_MODE_S
{
    VI_VPSS_MODE_E aenMode[VI_MAX_PIPE_NUM];
}VI_VPSS_MODE_S;

typedef struct arRAW_FRAME_COMPRESS_PARAM_S
{
    AR_U32 u32CompRatio_10Bit;/* RW, Range[1000, 4470]; compression ratio of 10 bit */
    AR_U32 u32CompRatio_12Bit;/* RW, Range[1000, 3700]; compression ratio of 12 bit */
    AR_U32 u32CompRatio_14Bit;/* RW, Range[1000, 3300]; compression ratio of 14 bit */
} RAW_FRAME_COMPRESS_PARAM_S;

#define AR_ERR_SYS_NULL_PTR         AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_SYS_NOTREADY         AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define AR_ERR_SYS_NOT_PERM         AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define AR_ERR_SYS_NOMEM            AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
#define AR_ERR_SYS_ILLEGAL_PARAM    AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_SYS_BUSY             AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define AR_ERR_SYS_NOT_SUPPORT      AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __AR_COMM_SYS_H__ */

