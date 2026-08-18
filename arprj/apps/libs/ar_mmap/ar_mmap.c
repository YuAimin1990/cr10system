#include "ar_mmap.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "utils_debug.h"
#include "osal.h"

#define AR_DEV_MEM		("/dev/mem")
#define AR_MMAP_MAGIC	(0x53535353U)

/*
 * The round_up() macro rounds up a value to the given boundary in a
 * type-agnostic yet type-safe manner. The boundary must be a power of two.
 * In other words, it computes the smallest multiple of boundary which is
 * greater than or equal to value.
 *
 * round_down() is similar but rounds the value down instead.
 */
#define round_boundary(value, boundary)		\
	((__typeof__(value))((boundary) - 1))

#define round_up(value, boundary)		\
	((((value) - 1) | round_boundary(value, boundary)) + 1)

#define round_down(value, boundary)		\
	((value) & ~round_boundary(value, boundary))

struct _ar_mmap_module_info
{
	int cache_fd;
	int noncache_fd;
};


struct _ar_mmaped_info
{
	void 			*real_start_va;
	size_t			real_sz;
	unsigned int	magic;
};

static struct _ar_mmap_module_info _mmap_inf = {
	.cache_fd		= -1,
	.noncache_fd	= -1
};


/*
 *
 */
int ar_mmap_init(void)
{
	if ((_mmap_inf.noncache_fd = open("/dev/mem", O_RDWR | O_SYNC)) <= 0) {
		log_tag_err("open noncache_fd = %d\n", _mmap_inf.noncache_fd);
		goto err_open;
	}

	if ((_mmap_inf.cache_fd = open("/dev/mem", O_RDWR)) <= 0) {
		log_tag_err("open cache_fd = %d\n", _mmap_inf.cache_fd);
		goto err_open;
	}

	return AR_SUCCESS;

err_open:

	(void)ar_mmap_clear();
	return AR_FAILURE;
}


/*
 *
*/
void *ar_mmap(AR_PHYS_ADDR pa, size_t length, struct ar_mmp_attr attr, void **hdl)
{
	assert(_mmap_inf.cache_fd > 0 && _mmap_inf.noncache_fd > 0);

	unsigned int page_size = sysconf(_SC_PAGE_SIZE);

	off_t offset  =  round_down(pa, page_size);

	length += (pa & (page_size - 1));
	void *real_start_va = mmap(NULL, length, attr.prot, attr.flags,
						((attr.cache == NON_CACHEABLE) ? _mmap_inf.noncache_fd :
						 								 _mmap_inf.cache_fd),
						offset);
	if (real_start_va != MAP_FAILED) {
		struct _ar_mmaped_info *mapped = ar_malloc(sizeof(*mapped));
		if (mapped) {
			mapped->real_start_va	= real_start_va;
			mapped->real_sz			= length;
			mapped->magic			= AR_MMAP_MAGIC;

			*hdl = mapped;
			return (real_start_va + (pa & (page_size - 1)));

		} else {
			log_tag_err("ar_malloc fail\n");
			munmap(real_start_va, length);
		}
	}

	return NULL;
}



int ar_munmap(void *hdl)
{
	int ret;
	struct _ar_mmaped_info *info = (struct _ar_mmaped_info *)hdl;

	if (info->magic != AR_MMAP_MAGIC) {
		log_tag_err("wrong magic: 0x%x\n", info->magic);
		return AR_FAILURE;
	}

	ret = munmap(info->real_start_va, info->real_sz);
	if (ret)
		log_tag_err("munmap fail ret=%d errono:%d\n", ret, errno);

	ar_free(hdl);

	return ret;
}

int ar_mmap_clear(void)
{
	int ret = -1;
	if (_mmap_inf.noncache_fd > 0)
		ret = close(_mmap_inf.noncache_fd);

	if (_mmap_inf.cache_fd > 0)
		ret = close(_mmap_inf.cache_fd);

	return ret;
}

