#ifndef __OSAL_LOG_H__
#define __OSAL_LOG_H__

#include <sys/syscall.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "utils_dbglog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

int ar_get_osal_log_id();

#define OSAL_TAG_ID ar_get_osal_log_id()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
