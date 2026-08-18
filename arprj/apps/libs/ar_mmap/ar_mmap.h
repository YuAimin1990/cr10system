#ifndef __AR_MPP_MMAP_H__
#define __AR_MPP_MMAP_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/mman.h>
typedef unsigned long AR_PHYS_ADDR;
#define AR_SUCCESS             0
#define AR_FAILURE             -1

enum ar_mmp_cache_attr
{
	CACHEABLE		= 0,
	NON_CACHEABLE	= 1,
};


/*
 * prot:	argument describes the desired memory protection of the mapping:
 *			PROT_EXEC,PROT_READ, PROT_WRITE, PROT_NONE
 * flags:	argument determines whether updates to the mapping are visible to other processes mapping the
 *			same region: MAP_SHARED, MAP_PRIVATE,
 * more of the attr check the mmap help
 */
struct ar_mmp_attr
{
	int		prot;
	int		flags;
	enum	ar_mmp_cache_attr cache;
};

#define AR_MMAP_ATTR_CACHEABLE(name)				\
	struct ar_mmp_attr name = {						\
	.prot	=	PROT_READ | PROT_WRITE,				\
	.flags	=	MAP_SHARED,							\
	.cache = CACHEABLE, 							\
};


#define AR_MMAP_ATTR_NONCACHEABLE(name) 			\
	struct ar_mmp_attr name = {						\
	.prot	=	PROT_READ | PROT_WRITE,				\
	.flags	=	MAP_SHARED,							\
	.cache	=	NON_CACHEABLE,						\
};


int ar_mmap_init(void);

void *ar_mmap(AR_PHYS_ADDR pa, size_t length, struct ar_mmp_attr attr, void **hdl);

int ar_munmap(void *hdl);

int ar_mmap_clear(void);


int ar_dev_map_init();

void *ar_dev_pa2va(AR_PHYS_ADDR pa);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AR_MPP_MMAP_H__ */


