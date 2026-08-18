#ifndef __UTIL_PROFILE_H__
#define __UTIL_PROFILE_H__

#include "mpi_sys.h"

#ifdef __cplusplus
extern "C"
{
#endif

AR_S32 EnableProfile();
AR_S32 DisbleProfile();

AR_S32 ProfileStart(const AR_CHAR * pchName);
AR_S32 ProfileEnd(const AR_CHAR * pchName);

#ifdef __cplusplus
}
#endif
#endif

