/*****************************************************************************
Copyright: 2021-2025, Artosyn. Co., Ltd.
File name: ar_mpi_dbglog.h
Description: The external debug log APIs.
Author: Artosyn Software Team
Version: 0.0.1
Date: 2021/04/21
History:
        0.0.1    2021/04/21    The initial version of ar_hal_dbglog.h
*****************************************************************************/

#ifndef __AR_MPI_DBGLOG_H__
#define __AR_MPI_DBGLOG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


//=============================================================================
// Include files
//=============================================================================
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ar_sys_drv.h"
#include "utils_dbglog.h"
//=============================================================================
// Data type definition
//=============================================================================
#define MPP_TAG_ID(mod_id) ( mod_id | LOG_LAYER_MPP)

/*
*list all log module.
*please add your own module here and add default modeName in ar_debug_log.c
*/
typedef enum
{
	AR_MPI_MOD_ID_LOG    = 0,
	AR_MPI_MOD_ID_TEST   = 1,
	AR_MPI_MOD_ID_CMD    = 2,
} ENUM_MPI_MOD_ID;

typedef struct
{
    ENUM_MPI_MOD_ID  mod_id;
    int    level;
    char   mode_name[16];
} STRU_MPI_LOG_LEVEL_CONF;

//=============================================================================
// Macro definition
//=============================================================================
#define FILENAME(x) strrchr(x, '/') ?  strrchr(x, '/') + 1 : x

/* output log's level */
#define AR_MPI_LOG_LEVEL_ASSERT                      0
#define AR_MPI_LOG_LEVEL_ERROR                       1
#define AR_MPI_LOG_LEVEL_WARN                        2
#define AR_MPI_LOG_LEVEL_INFO                        3
#define AR_MPI_LOG_LEVEL_DEBUG                       4
#define AR_MPI_LOG_LEVEL_VERBOSE                     5

//=============================================================================
// Global function definition
//=============================================================================
/**
* @brief  Initialize the log.
* @param  NONE.
* @retval 0    means the function is well done.
* @note   This function must be called when use log.
*/
int AR_MPI_LOG_Init();

/**
* @brief  Close the log.
* @param  NONE.
* @retval NONE.
* @note   This function must be called when close log.
*/
void AR_MPI_LOG_Close();

/**
* @brief  Set log module level.
* @param  pstConf	the info for log module.
* @retval 0    means the function is well done.
* @note   When pstConf->ModName is "", func will use default ModName,
*	when pstConf->ModName is "all", the log level for all modules will be set.
*/
int AR_MPI_LOG_SetLevelConf(STRU_MPI_LOG_LEVEL_CONF *pst_conf);

/**
* @brief  Get log module level.
* @param  pstConf	the info of log module.
* @retval 0    means the function is well done.
*/
int AR_MPI_LOG_GetLevelConf(STRU_MPI_LOG_LEVEL_CONF *pst_conf);

//TODO
int AR_MPI_LOG_SetWaitFlag(int s32_bWait);

/**
* @brief  Read log.
* @param  pBuf	buffer to fill read content.
* @param  u32Size	log size want to read.
* @retval 0    means the function is well done.
*/
int AR_MPI_LOG_Read(char *ps8_buf, size_t u32_size);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /*__AR_MPI_DBGLOG_H__ */



