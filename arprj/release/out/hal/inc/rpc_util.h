/*
 * ossp-util - OSS Proxy: Common utilities
 *
 * Copyright (C) 2008-2010  SUSE Linux Products GmbH
 * Copyright (C) 2008-2010  Tejun Heo <tj@kernel.org>
 *
 * This file is released under the GPLv2.
 */

#ifndef _OSSP_UTIL_H
#define _OSSP_UTIL_H

#define OSSP_LOG_NAME_LEN	128

#define BITS_PER_BYTE		8
#define BITS_PER_LONG		(BITS_PER_BYTE * sizeof(long))
#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

/*
 * Bitops lifted from linux asm-generic implementation.
 */
unsigned long find_next_zero_bit(const unsigned long *addr, unsigned
				 long size, unsigned long offset);
#define find_first_zero_bit(addr, size) find_next_zero_bit((addr), (size), 0)
extern void __set_bit(int nr, volatile unsigned long *addr);
extern void __clear_bit(int nr, volatile unsigned long *addr);

#endif /*_OSSP_UTIL_H*/
