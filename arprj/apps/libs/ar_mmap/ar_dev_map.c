
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "ar_mmap.h"

#define	REG_ADDR_START			(0x00000000)
#define	REG_ADDR_END			(0x10000000)

/* Compute the number of elements in the given array */
#define ARRAY_SIZE(a)				\
	(sizeof(a) / sizeof((a)[0]))

typedef struct
{
	unsigned long start;
	unsigned long end;
	void		*va;
	void		*map_hdl;
} va_map;


static va_map ar_dev_addr_map[] = {
	{REG_ADDR_START,  REG_ADDR_END, NULL, NULL},
};

int ar_dev_map_deinit(void);

void ar_dev_map_print(void)
{
	int i;
	for (i = 0; i< ARRAY_SIZE(ar_dev_addr_map); i++){
		printf("seciotn%d: 0x%08lx => 0x%08lx\r\n",
				i, ar_dev_addr_map[i].start, ar_dev_addr_map[i].end);
	}
}

int ar_dev_map_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar_dev_addr_map); i++) {

		AR_MMAP_ATTR_NONCACHEABLE(attr);

		ar_dev_addr_map[i].va = ar_mmap(ar_dev_addr_map[i].start,
									(ar_dev_addr_map[i].end - ar_dev_addr_map[i].start + 1),
									attr, &ar_dev_addr_map[i].map_hdl);

		if (ar_dev_addr_map[i].va == NULL)
			goto err_mmap;
	}

	return AR_SUCCESS;

err_mmap:
	ar_dev_map_deinit();

	return AR_FAILURE;
}

int ar_dev_map_deinit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar_dev_addr_map); i++) {
		if (!ar_dev_addr_map[i].va)
			break;

		ar_munmap(ar_dev_addr_map[i].map_hdl);
		ar_dev_addr_map[i].va = NULL;
	}

	return 0;
}


void * ar_dev_pa2va(AR_PHYS_ADDR pa)
{
	for (int i = 0; i<ARRAY_SIZE(ar_dev_addr_map); i++) {
		if ((pa >= ar_dev_addr_map[i].start) && (pa <= ar_dev_addr_map[i].end))
			return ar_dev_addr_map[i].va + (pa-ar_dev_addr_map[i].start);
	}

	return NULL;
}
