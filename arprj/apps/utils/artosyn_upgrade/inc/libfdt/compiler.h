/*
 * Keep all the ugly #ifdef for system stuff here
 */

#ifndef __COMPILER_H__
#define __COMPILER_H__


/* Provided by gcc. */
#include <stdint.h>

#include <little_endian.h>


#if __SIZEOF_LONG__ == 8
# define __WORDSIZE	64
#elif __SIZEOF_LONG__ == 4
# define __WORDSIZE	32
#else
/*
 * Assume 32-bit for now - only newer toolchains support this feature and
 * this is only required for sandbox support at present.
 */
#define __WORDSIZE	32
#endif


#endif
