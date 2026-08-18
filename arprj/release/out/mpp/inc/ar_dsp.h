#ifndef _AR_DSP_H_
#define _AR_DSP_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "ar_comm_svp.h"
//if sdk environment,include ar_common.h,else typedef AR_S32 SVP_DSP_HANDLE
#include "ar_common.h"

/**************************************SVP_DSP Error Code***********************************************************************/
typedef enum arEN_SVP_DSP_ERR_CODE_E
{
    ERR_SVP_DSP_SYS_TIMEOUT    = 0x40,   /* SVP_DSP process timeout */
    ERR_SVP_DSP_QUERY_TIMEOUT  = 0x41,   /* SVP_DSP query timeout */
    ERR_SVP_DSP_OPEN_FILE      = 0x42,   /* SVP_DSP open file error*/
    ERR_SVP_DSP_READ_FILE      = 0x43,   /* SVP_DSP read file error*/

    ERR_SVP_DSP_BUTT
}EN_SVP_DSP_ERR_CODE_E;
/*Invalid device ID*/
#define AR_ERR_SVP_DSP_INVALID_DEVID     AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
/*Invalid channel ID*/
#define AR_ERR_SVP_DSP_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/*At least one parameter is illegal. For example, an illegal enumeration value exists.*/
#define AR_ERR_SVP_DSP_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/*The channel exists.*/
#define AR_ERR_SVP_DSP_EXIST             AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/*The UN exists.*/
#define AR_ERR_SVP_DSP_UNEXIST           AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/*A null point is used.*/
#define AR_ERR_SVP_DSP_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/*Try to enable or initialize the system, device, or channel before configuring attributes.*/
#define AR_ERR_SVP_DSP_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/*The operation is not supported currently.*/
#define AR_ERR_SVP_DSP_NOT_SURPPORT      AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/*The operation, changing static attributes for example, is not permitted.*/
#define AR_ERR_SVP_DSP_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/*A failure caused by the malloc memory occurs.*/
#define AR_ERR_SVP_DSP_NOMEM             AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/*A failure caused by the malloc buffer occurs.*/
#define AR_ERR_SVP_DSP_NOBUF             AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/*The buffer is empty.*/
#define AR_ERR_SVP_DSP_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/*No buffer is provided for storing new data.*/
#define AR_ERR_SVP_DSP_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/*The system is not ready because it may be not initialized or loaded.
 *The error code is returned when a device file fails to be opened. */
#define AR_ERR_SVP_DSP_NOTREADY          AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/*The source address or target address is incorrect during the operations such as calling copy_from_user or copy_to_user.*/
#define AR_ERR_SVP_DSP_BADADDR           AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_BADADDR)
/*The resource is busy during the operations such as destroying a VENC channel without deregistering it.*/
#define AR_ERR_SVP_DSP_BUSY              AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
/*SVP_DSP process timeout*/
#define AR_ERR_SVP_DSP_SYS_TIMEOUT       AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, ERR_SVP_DSP_SYS_TIMEOUT)
/*SVP_DSP query timeout*/
#define AR_ERR_SVP_DSP_QUERY_TIMEOUT     AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, ERR_SVP_DSP_QUERY_TIMEOUT)
/*SVP_DSP open file error*/
#define AR_ERR_SVP_DSP_OPEN_FILE         AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, ERR_SVP_DSP_OPEN_FILE)
/*SVP_DSP read file error*/
#define AR_ERR_SVP_DSP_READ_FILE         AR_MPP_DEF_ERR(AR_ID_SVP_DSP, HAL_ERR_LEVEL_ERROR, ERR_SVP_DSP_READ_FILE)


/*SVP_DSP core id*/
typedef enum arSVP_DSP_ID_E
{
    SVP_DSP_ID_0 = 0x0,

    SVP_DSP_ID_BUTT
}SVP_DSP_ID_E;

/*SVP_DSP  priority*/
typedef enum arSVP_DSP_PRI_E
{
    SVP_DSP_PRI_0 = 0x0,
    SVP_DSP_PRI_1 = 0x1,
    SVP_DSP_PRI_2 = 0x2,

    SVP_DSP_PRI_BUTT
}SVP_DSP_PRI_E;

/*SVP_DSP memory type*/
typedef enum arSVP_DSP_MEM_TYPE_E
{
    SVP_DSP_MEM_TYPE_SYS_DDR_DSP_0 = 0x0,
    SVP_DSP_MEM_TYPE_IRAM_DSP_0    = 0x1,
    SVP_DSP_MEM_TYPE_DRAM_0_DSP_0  = 0x2,
    SVP_DSP_MEM_TYPE_DRAM_1_DSP_0  = 0x3,

    SVP_DSP_MEM_TYPE_BUTT
}SVP_DSP_MEM_TYPE_E;

/*SVP_DSP  cmd*/
typedef enum arSVP_DSP_CMD_E
{
    SVP_DSP_CMD_INIT       = 0x0,
	SVP_DSP_CMD_EXIT       = 0x1,
	SVP_DSP_CMD_ERODE_3X3  = 0x2,
	SVP_DSP_CMD_DILATE_3X3 = 0x3,

#ifdef CONFIG_AR_PHOTO_SUPPORT
    SVP_DSP_CMD_PHOTO_PROC,
#endif

	SVP_DSP_CMD_BUTT
}SVP_DSP_CMD_E;

/*SVP_DSP ARM->DSP request message*/
typedef struct arSVP_DSP_MESSAGE_S
{
    AR_U32      u32CMD;      /*CMD ID, user-defined SVP_DSP_CMD_BUTT + */
    AR_U32      u32MsgId;    /*Message ID*/
    AR_U64      u64Body;     /*Message body*/
    AR_U32      u32BodyLen;  /*Length of pBody*/
} SVP_DSP_MESSAGE_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_AR_DSP_H_*/
