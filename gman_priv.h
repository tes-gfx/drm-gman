#ifndef __GMAN_PRIV_H__
#define __GMAN_PRIV_H__


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/ioctl.h>
#include <xf86drm.h>

#include "gman_drmif.h"
#include "gman_drm.h"


#define VOID2U64(x) ((uint64_t)(unsigned long)(x))
#define U642VOID(x) ((void*)(unsigned long)(x))


struct gman_device;


struct gman_device {
	int fd;
};


/* a GEM buffer object allocated from the DRM driver */
struct gman_bo_priv {
	struct gman_bo     base;
	struct gman_device *dev;
	gman_dma_addr_t    paddr;
	void               *map;           /* userspace mmap'ing (if there is one) */
	uint32_t           size;
	uint32_t           handle;
	uint32_t           flags;
	uint64_t           offset;         /* offset to mmap() */
};

/* debug stuff:
 */

#define gman_enable_debug 1

#define INFO_MSG(fmt, ...) \
		do { drmMsg("[I] "fmt " (%s:%d)\n", \
				##__VA_ARGS__, __FUNCTION__, __LINE__); } while (0)
#define DEBUG_MSG(fmt, ...) \
		do if (gman_enable_debug) { drmMsg("[D] "fmt " (%s:%d)\n", \
				##__VA_ARGS__, __FUNCTION__, __LINE__); } while (0)
#define WARN_MSG(fmt, ...) \
		do { drmMsg("[W] "fmt " (%s:%d)\n", \
				##__VA_ARGS__, __FUNCTION__, __LINE__); } while (0)
#define ERROR_MSG(fmt, ...) \
		do { drmMsg("[E] " fmt " (%s:%d)\n", \
				##__VA_ARGS__, __FUNCTION__, __LINE__); } while (0)

#endif
