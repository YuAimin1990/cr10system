/*
 * libfdt - Flat Device Tree manipulation (build/run environment adaptation)
 * Copyright (C) 2007 Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com
 * Original version written by David Gibson, IBM Corporation.
 *
 * SPDX-License-Identifier:	LGPL-2.1+
 */

#ifndef _LIBFDT_ENV_H
#define _LIBFDT_ENV_H

#include "compiler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fdt_types.h"
#include "generic.h"

extern struct fdt_header *working_fdt;  /* Pointer to the working fdt */

typedef __be16 fdt16_t;
typedef __be32 fdt32_t;
typedef __be64 fdt64_t;

#define fdt32_to_cpu(x)		be32_to_cpu(x)
#define cpu_to_fdt32(x)		cpu_to_be32(x)
#define fdt64_to_cpu(x)		be64_to_cpu(x)
#define cpu_to_fdt64(x)		cpu_to_be64(x)
#if 0
unsigned long long simple_strtoull(const char *cp, char **endp,
					unsigned int base);

unsigned long simple_strtoul(const char *cp, char **endp,
				unsigned int base);

int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);

long simple_strtol(const char *cp, char **endp, unsigned int base);

unsigned long ustrtoul(const char *cp, char **endp, unsigned int base);


unsigned long long ustrtoull(const char *cp, char **endp, unsigned int base);

long trailing_strtol(const char *str);
#endif

/* adding a ramdisk needs 0x44 bytes in version 2008.10 */
#define FDT_RAMDISK_OVERHEAD	0x80

#endif /* _LIBFDT_ENV_H */
