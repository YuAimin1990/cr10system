/*
 * This code was taken from the linux kernel. The license is GPL Version 2.
 */

#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>

typedef unsigned char Bytef;
typedef unsigned int  uInt;

/* Return a 32-bit CRC of the contents of the buffer */
uint32_t crc32 (uint32_t crc, const Bytef *p, uInt len);

#endif /* __CRC32_H__ */
