#ifndef __GMAN_DRMIF_H__
#define __GMAN_DRMIF_H__


#include <xf86drm.h>
#include <stdint.h>


struct gman_device;
struct gman_stream;
struct gman_bo;


/* device functions:
 */

struct gman_device *gman_device_new(int fd);
void gman_device_del(struct gman_device *dev);
int gman_device_fd(struct gman_device *dev);
uint32_t *gman_device_mem_io(struct gman_device *dev);


/* buffer-object functions:
 */

struct gman_bo *gman_bo_new(struct gman_device *dev,
		uint32_t size, uint32_t flags);
void *gman_bo_map(struct gman_bo *bo);
void gman_bo_del(struct gman_bo *bo);
struct gman_bo *gman_bo_from_dmabuf(struct gman_device *dev, int fd);


/* debug functions:
 */

int gman_debug_selftest(struct gman_device *dev);
void gman_debug_reset(struct gman_device *dev);


/* stream functions:
 */

struct gman_stream {
	void *stream;           /* ptr to stream data */
	void *jump;             /* ptr to jump address at stream's end */
};

/* todo: integrate with gman_priv_bo and hide everything to user side,
 * use functions instead to get values */
struct gman_bo {
	void *paddr;
	void *map;
	uint32_t handle;
};

struct gman_stream *gman_stream_new(struct gman_device *device);
void gman_stream_del(struct gman_stream *stream);
void gman_stream_append_bo(struct gman_device *device, struct gman_bo *bo);
void gman_stream_flush(struct gman_stream *stream);
int gman_stream_finish(struct gman_stream *stream);
uint32_t gman_stream_timestamp(struct gman_stream *stream);


#endif
