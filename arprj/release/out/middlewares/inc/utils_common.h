#ifndef __UTILS_COMMON_H__
#define __UTILS_COMMON_H__

#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MAX_LEN 32

typedef enum {
  AR_OK                      =  0,         //
  AR_ERROR                   = -1,         //
  AR_ERROR_TIMEOUT           = -2,         //
  AR_ERROR_QUEUE_FULL        = -3,         //
  AR_ERROR_QUEUE_EMPTY       = -4,         //
  AR_ERROR_NO_TIME_FUNCTION  = -5,         //
  AR_ERROR_MAX               = 0x7FFFFFFF  //
} ar_status_t;

uint64_t ar_get_linux_time();
uint64_t ar_get_linux_time_us();

 /**
* @Description:The ar_memset function copies the value of c (converted to an unsigned char) into each of the first count characters of the object pointed to by dest.
* @param dest - destination  address
* @param destMax -The maximum length of destination buffer
* @param c - the value to be copied
* @param count -copies fisrt count characters of  dest
* @return  EOK if there was no runtime-constraint violation
*/
int ar_memset(void* dest, size_t destMax, int c, size_t count);

/**
* @Description:The ar_memcpy function copies n characters from the object pointed to by src into the object pointed to by dest.
* @param dest - destination  address
* @param destMax -The maximum length of destination buffer
* @param src -source  address
* @param count -copies count  characters from the  src
* @return  EOK if there was no runtime-constraint violation
*/
int ar_memcpy(void* dest, size_t destMax, const void* src, size_t count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //_AR_COMMON_H_
