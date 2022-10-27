/**
 * Title: GMAN - Graphics Manager
 *
 * A graphics memory manager for baremetal drivers.
 */
#ifndef __GMAN_DRMIF_H__
#define __GMAN_DRMIF_H__


#include <xf86drm.h>
#include <stdint.h>

/**
 * Group: Types
 */

/**
 * Type: gman_device
 * 
 * GMAN device handle.
 */
struct gman_device;

/**
 * Type: gman_bo
 * 
 * Buffer object handle.
 */
struct gman_bo {
	int dummy;
};

/**
 * Type: gman_dma_addr_t
 * 
 * Physical address as used by HW devices.
 */
typedef uint64_t gman_dma_addr_t;

/* Group: Device functions
 */

/**
 * function: gman_device_new
 *
 * Create new GMAN device handle.
 *
 * parameters:
 *  fd - file descriptor of GMAN device
 *
 * The file descriptor fd identifies the GMAN device. It is a file descriptor
 * of an opened GMAN device.
 *
 * returns:
 *  GMAN device handle
 */
struct gman_device *gman_device_new(int fd);

/**
 * function: gman_device_del
 *
 * Delete GMAN device handle.
 *
 * parameters:
 *  dev - GMAN device handle created with <gman_device_new>
 *
 * Gracefully shut down the GMAN device and free the device handle.
 */
void gman_device_del(struct gman_device *dev);

/**
 * function: gman_device_fd
 *
 * Return the file descriptor used to create the given GMAN handle.
 *
 * parameters:
 *  dev - GMAN device handle created with <gman_device_new>
 *
 * Depending on the platform, the file descriptor used during creation is
 * also required during clean up (e.g. calling close()). For convenience,
 * this function returns the file descriptor used to create the given handle.
 *
 * returns:
 *  file decriptor used to create the given handle
 */
int gman_device_fd(struct gman_device *dev);
uint32_t *gman_device_mem_io(struct gman_device *dev);


/* Group: Buffer-Object functions:
 */

/**
 * Function: gman_bo_new
 *
 * Create a new BO of the given size.
 *
 * Parameters:
 *  dev - GMAN device handle created with <gman_device_new>
 *
 * Depending on the platform, the file descriptor used during creation is
 * also required during clean up (e.g. calling close()). For convenience,
 * this function returns the file descriptor used to create the given handle.
 *
 * Returns:
 *  File decriptor used to create the given handle
 */
/* allocate a new (un-tiled) buffer object */
struct gman_bo *gman_bo_new(struct gman_device *dev,
		uint32_t size, uint32_t flags);

/**
 * Function: gman_bo_map
 *
 * Maps a BO to CPU address space.
 *
 * Parameters:
 *  bo - BO that will be mapped
 *
 * returns:
 *  Pointer to the mapped BO data or NULL
 */
void *gman_bo_map(struct gman_bo *bo);

/**
 * Function: gman_bo_dma_addr
 *
 * Get a DMA address for the given BO.
 * 
 * The DMA address is platform dependent. No assumptions can be made.
 *
 * Parameters:
 *  bo - the BO whose DMA address will be a acquired
 *
 * returns:
 *  A DMA address for the given BO.
 */
gman_dma_addr_t gman_bo_dma_addr(struct gman_bo *bo);

/**
 * Function: gman_bo_del
 *
 * Delete a BO created with <gman_bo_new>.
 * 
 * The memory used by the BO is freed.
 *
 * Parameters:
 *  p - the BO to delete.
 */
void gman_bo_del(struct gman_bo *bo);

/**
 * Function: gman_bo_from_dmabuf
 *
 * Create a BO from a dmabuf.
 * 
 * The dmabuf is referenced by a file descriptor. This can be used on Linux
 * platforms to integrate devices with an actual Linux driver.
 *
 * Parameters:
 *  dev - the GMAN device handle
 *  fd - the file descriptor of the dmabuf
 *
 * returns:
 *  a BO handle or NULL.
 */
struct gman_bo *gman_bo_from_dmabuf(struct gman_device *dev, int fd);


/* Debug functions are not supported yet.
 */
int gman_debug_selftest(struct gman_device *dev);
void gman_debug_reset(struct gman_device *dev);


/* Streams are not supported yet. */
struct gman_stream {
	void *stream;           /* ptr to stream data */
	void *jump;             /* ptr to jump address at stream's end */
};

struct gman_stream *gman_stream_new(struct gman_device *dev);
void gman_stream_del(struct gman_stream *stream);
void gman_stream_append_bo(struct gman_device *dev, struct gman_bo *bo);
void gman_stream_flush(struct gman_stream *stream);
int gman_stream_finish(struct gman_stream *stream);
uint32_t gman_stream_timestamp(struct gman_stream *stream);

/* Group: Display Interface functions
 */
#ifndef fbo_t
/* Anonymous declaration if needed. fbo_t is platform dependent and defined by
 * the used DI implementation. */
typedef struct _fbo_t fbo_t;
#endif

/**
 * Function: gman_createfbo
 *
 * Create an FBO for use with the DI display interface.
 *
 * Parameters:
 *  dev - a GMAN device handle
 *  bo - a BO used as frame buffer.
 *  width - frame buffer's width
 *  height - frame buffer's height
 *  pitch - frame buffer's pitch in pixels
 * 	bpp - bytes per pixel
 *
 * returns:
 *  fbo handle or NULL
 */
fbo_t * gman_createfbo(struct gman_device *dev, struct gman_bo *bo, unsigned int width, unsigned int height, unsigned int pitch, int bpp);

/**
 * Function: gman_deletefbo
 *
 * Delete an FBO created with <gman_createfbo>.
 *
 * Parameters:
 *  dev - a GMAN device handle
 *  fbo - the FBO to delete
 */
void gman_deletefbo(struct gman_device *dev, fbo_t *fbo);

#endif
