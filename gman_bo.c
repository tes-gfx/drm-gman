#define _FILE_OFFSET_BITS 64
#include "libdrm_macros.h"

#include "gman_priv.h"
#include "gman_drmif.h"

#include <unistd.h>


/* todo: Implement a caching for bos. Reuse deleted bos to reduce kernel calls */


static inline struct gman_bo_priv *
gman_bo_priv(struct gman_bo *bo)
{
    return (struct gman_bo_priv *) bo;
}


/* allocate a new buffer object, call w/ table_lock held */
static struct gman_bo *bo_from_handle(struct gman_device *dev,
		uint32_t size, uint32_t handle, uint32_t flags, gman_dma_addr_t paddr)
{
	struct gman_bo_priv *bo = calloc(sizeof(*bo), 1);

	if (!bo) {
		struct drm_gem_close req = {
			.handle = handle,
		};

		drmIoctl(dev->fd, DRM_IOCTL_GEM_CLOSE, &req);

		return NULL;
	}

	bo->dev = dev;
	bo->size = size;
	bo->handle = handle;
	bo->flags = flags;
	bo->paddr = paddr;

	return &bo->base;
}


/* allocate a new (un-tiled) buffer object */
struct gman_bo *gman_bo_new(struct gman_device *dev, uint32_t size,
		uint32_t flags)
{
	struct gman_bo *bo;
	int ret;
	struct drm_gman_gem_new req = {
			.flags = flags,
			.size = size,
	};

	ret = drmCommandWriteRead(dev->fd, DRM_GMAN_GEM_NEW,
			&req, sizeof(req));
	if (ret)
		return NULL;

	bo = bo_from_handle(dev, size, req.handle, flags, req.paddr);

	return bo;
}


/* get buffer info */
static int get_buffer_info(struct gman_bo_priv *bo)
{
	int ret;
	struct drm_gman_gem_info req = {
		.handle = bo->handle,
	};

	ret = drmCommandWriteRead(bo->dev->fd, DRM_GMAN_GEM_INFO,
			&req, sizeof(req));
	if (ret) {
		return ret;
	}

	/* really all we need for now is mmap offset */
	bo->offset = req.offset;

	return 0;
}


/* map a bo to user space */
void *gman_bo_map(struct gman_bo *bo)
{
	struct gman_bo_priv *priv = gman_bo_priv(bo);

	if (!priv->map) {
		if (!priv->offset) {
			get_buffer_info(priv);
		}

		priv->map = drm_mmap(0, priv->size, PROT_READ | PROT_WRITE,
				MAP_SHARED, priv->dev->fd, priv->offset);
		if (priv->map == MAP_FAILED) {
			ERROR_MSG("mmap failed: %s", strerror(errno));
			priv->map = NULL;
		}
	}

	return priv->map;
}

gman_dma_addr_t gman_bo_dma_addr(struct gman_bo *bo)
{	
	struct gman_bo_priv *priv = gman_bo_priv(bo);
	return priv->paddr;
}


/* get buffer info required by user space driver */
static int get_buffer_user(struct gman_device *dev, uint32_t handle, gman_dma_addr_t* paddr)
{
	int ret;
	struct drm_gman_gem_user req = {
		.handle = handle,
	};

	ret = drmCommandWriteRead(dev->fd, DRM_GMAN_GEM_USER,
			&req, sizeof(req));
	if (ret) {
		return ret;
	}

	*paddr = req.paddr;

	return 0;
}


/* import a buffer from dmabuf fd, does not take ownership of the
 * fd so caller should close() the fd when it is otherwise done
 * with it (even if it is still using the 'struct gman_bo *')
 */
struct gman_bo *gman_bo_from_dmabuf(struct gman_device *dev, int fd)
{
	struct gman_bo *bo;
	int ret, size;
	uint32_t handle;
	gman_dma_addr_t paddr = 0;

	ret = drmPrimeFDToHandle(dev->fd, fd, &handle);
	if (ret) {
		return NULL;
	}

	/* lseek() to get bo size */
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_CUR);

	get_buffer_user(dev, handle, &paddr);
	bo = bo_from_handle(dev, size, handle, 0, paddr);

	return bo;
}


drm_private void bo_del(struct gman_bo_priv *bo)
{
	if (bo->map)
		drm_munmap(bo->map, bo->size);

	if (bo->handle) {
		struct drm_gem_close req = {
			.handle = bo->handle,
		};

		drmIoctl(bo->dev->fd, DRM_IOCTL_GEM_CLOSE, &req);
	}

	free(bo);
}


/* destroy a buffer object */
void gman_bo_del(struct gman_bo *bo)
{
	struct gman_bo_priv *priv = gman_bo_priv(bo);

	if (!priv)
		return;

	bo_del(priv);
}
