#include <stdlib.h>
#include <errno.h>

#if !defined(_WIN32) && !defined(_WIN64) && !defined(LXSS)

#include <sys/mman.h>
#include <xf86drm.h>

#include "gman_priv.h"


struct gman_device *gman_device_new(int fd)
{
	struct gman_device *dev = calloc(sizeof(*dev), 1);

	if(!dev) {
		ERROR_MSG("could not create device object");
		return NULL;
	}

	dev->fd = fd;

	return dev;
}


void gman_device_del(struct gman_device *dev)
{
	free(dev);
}


int gman_device_fd(struct gman_device *dev)
{
	return dev->fd;
}

#endif
