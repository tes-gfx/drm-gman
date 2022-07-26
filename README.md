# libdrm-gman
GMAN (Gfx Manager) user space library for TES EvalKits. Do NOT use in
production. EvalKit only!

This library implements the GMAN DRM module user space interface for user
space applications.
Its purpose is to create, map and get the physical address of simple BOs
(Buffer Objects).

## Example
A simple example that opens the device and allocates 4096 bytes:

```
struct gman_device *gman;
int fd;
struct gman_bo *bo;
void *map;

fd = drmOpen("tes-gman", NULL);
gman = gman_device_new(fd);

bo = gman_bo_new(gman, 4096, GMAN_BO_UNCACHED);
map = gman_bo_map(bo);

drmClose(gman_device_fd(gman));
```
