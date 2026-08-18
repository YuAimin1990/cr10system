#ifndef __COMMON_H__
#define __COMMON_H__

#define _LARGEFILE64_SOURCE

#include <linux/types.h>
#include <stdint.h>
#include <stddef.h>

typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __u64 u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef uint64_t lbaint_t;

#define LBAF  "%llx"
#define LBAFU "%llu"

#define __packed __attribute__((packed))

#endif
