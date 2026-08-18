
#ifndef __RTSP_LOG_H__
#define __RTSP_LOG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <stdbool.h>

extern const char* LOG_strerror(uint32_t errno);

#if LOG_DEBUG
#define LOG_SYNTAX "1;32"
#define LOG_TRACE(fmt, arg...) \
	do{\
		const char* const bname = __FILE__;\
		printf("\033["LOG_SYNTAX"m[%12s:%4d]\033[0m ", bname, __LINE__);\
		printf(fmt, ##arg);\
		printf("\r\n");\
	}while(0)

#define LOG_ASSERT(exp, fmt, arg...) \
	do{\
		if(!(exp)){\
			const char* const bname = __FILE__;\
			printf("\033["LOG_SYNTAX"m[%12s:%4d]\033[0m assert(\"%s\") ", bname, __LINE__, #exp);\
			printf(fmt, ##arg);\
			printf("\r\n");\
		}\
	}while(0)

#define LOG_CHECK(exp, fmt...) \
	do{\
		uint32_t ret = exp;\
		if(GK_SUCCESS != ret){\
			const char* const bname = __FILE__;\
			printf("\033["LOG_SYNTAX"m");\
			printf("%s @ [%s: %d] err: 0x%08x <%s>", #exp, bname, __LINE__, ret, LOG_strerror(ret));\
			printf("\033[0m\r\n");\
		}\
	}while(0)

#else
#define LOG_TRACE(fmt...)
#define LOG_ASSERT(exp, fmt, arg...)
#define LOG_CHECK(exp, fmt...)
#endif
/*=====================================*/

#define _DBG_PRINTF_ 1
#if _DBG_PRINTF_
#define DBG_PRINTF(format, ...) printf("\033[0;0m[RTSP DBG] FILE: "__FILE__", LINE: %d: "format"\033[0;0m\n", __LINE__, ##__VA_ARGS__)
#else
#define DBG_PRINTF(format, ...)
#endif

#define _WARN_PRINTF_   1
#if _WARN_PRINTF_
#define WARN_PRINTF(format, ...) printf("\033[0;32m[RTSP WARN] FILE: "__FILE__", LINE: %d: "format"\033[0;0m\n", __LINE__, ##__VA_ARGS__)
#else
#define WARN_PRINTF(format, ...)
#endif

#define _ERR_PRINTF_    1
#if _ERR_PRINTF_
#define ERR_PRINTF(format, ...) printf("\033[0;31m[RTSP ERR] FILE: "__FILE__", LINE: %d: "format"\033[0;0m\n", __LINE__, ##__VA_ARGS__)
#else
#define ERR_PRINTF(format, ...)
#endif


#ifdef __cplusplus
};
#endif
#endif //__RTSP_LOG_H__

