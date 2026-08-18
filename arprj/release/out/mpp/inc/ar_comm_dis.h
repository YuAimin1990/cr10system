/**
 * \file
 * \brief 描述dis相关的通用数据结构
 */

#ifndef __AR_COMM_DIS_H__
#define __AR_COMM_DIS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hal_type.h"
#include "ar_common.h"
#include "hal_errno.h"
#include "ar_comm_video.h"

/* failure caused by malloc buffer */
#define AR_ERR_DIS_NOBUF                AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define AR_ERR_DIS_BUF_EMPTY            AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
#define AR_ERR_DIS_NULL_PTR             AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_DIS_ILLEGAL_PARAM        AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_DIS_BUF_FULL             AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
#define AR_ERR_DIS_SYS_NOTREADY         AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define AR_ERR_DIS_NOT_SUPPORT          AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
#define AR_ERR_DIS_NOT_PERMITTED        AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define AR_ERR_DIS_BUSY                 AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define AR_ERR_DIS_INVALID_CHNID        AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
#define AR_ERR_DIS_CHN_UNEXIST          AR_MPP_DEF_ERR(AR_ID_DIS, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)


/* Different mode of DIS */
typedef enum arDIS_MODE_E
{
    DIS_MODE_4_DOF_GME      = 0,    /* Only use with GME in 4 dof  */
    DIS_MODE_6_DOF_GME,             /* Only use with GME in 6 dof  */
    DIS_MODE_GYRO,                  /* Only use with gryo in 6 dof  */
    DIS_MODE_HYBRID,                /* Both use with GME and gyro in 6 dof */
    DIS_MODE_DOF_BUTT,
} DIS_MODE_E;

/* The motion level of camera */
typedef enum arDIS_MOTION_LEVEL_E
{
    DIS_MOTION_LEVEL_LOW    = 0,    /* Low motion level*/
    DIS_MOTION_LEVEL_NORMAL,        /* Normal motion level */
    DIS_MOTION_LEVEL_HIGH,          /* High motion level */
    DIS_MOTION_LEVEL_BUTT
}DIS_MOTION_LEVEL_E;


/* Different product type used DIS */
typedef enum arDIS_PDT_TYPE_E
{
    DIS_PDT_TYPE_IPC        = 0,    /* IPC product type */
    DIS_PDT_TYPE_DV,                /* DV product type */
    DIS_PDT_TYPE_DRONE,             /* DRONE product type */
    DIS_PDT_TYPE_BUTT
} DIS_PDT_TYPE_E;

/* The Attribute of DIS */
typedef struct arDIS_ATTR_S
{
    AR_BOOL     bEnable;                /* RW; DIS enable */
    AR_BOOL     bGdcBypass;             /* RW; gdc correction process , DIS = GME&GDC correction*/
    AR_U32      u32MovingSubjectLevel;  /* RW; Range:[0,6]; Moving Subject level */
    AR_S32      s32RollingShutterCoef;  /* RW; Range:[0,1000]; Rolling shutter coefficients */
    AR_U32      u32Timelag;             /* RW; Range:[0,200000]; Timestamp delay between Gyro and Frame PTS */
    AR_U32      u32ViewAngle;           /* Reserved */
    AR_U32      u32HorizontalLimit;     /* RW; Range:[0,1000]; Parameter to limit horizontal drift by large foreground */
    AR_U32      u32VerticalLimit;       /* RW; Range:[0,1000]; Parameter to limit vertical drift by large foreground */
    AR_BOOL     bStillCrop;             /* RW; The stabilization will be not working ,but the output image still be cropped */
}DIS_ATTR_S;

/* The Config of DIS */
typedef struct arDIS_CONFIG_S
{
    DIS_MODE_E              enMode;                             /* RW; DIS Mode */
    DIS_MOTION_LEVEL_E      enMotionLevel;                      /* RW; DIS Motion level of the camera */
    DIS_PDT_TYPE_E          enPdtType;                          /* RW; DIS product type*/
    AR_U32                  u32BufNum;                          /* RW; Range:[5,10]; Buf num for DIS */
    AR_U32                  u32CropRatio;                       /* RW; Range:[50,98]; Crop ratio of output image */
    AR_U32                  u32FrameRate;                       /* RW; Range:[25,120]; The input framerate */
    AR_U32                  u32GyroOutputRange;                 /* RW; Range:[0,360]; The range of Gyro output in degree */
    AR_U32                  u32GyroDataBitWidth;                /* RW; Range:[0,32]; The bits used for gyro angular velocity output */
    AR_BOOL                 bCameraSteady;                      /* RW; The camera is steady or not */
    AR_BOOL                 bScale;                             /* RW; Scale output image or not*/
}DIS_CONFIG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __AR_COMM_DIS_H__ */
