#include "gman_priv.h"
#include "di_platform.h"

#ifdef SUPPORT_DI

fbo_t * gman_createfbo(struct gman_device *device, struct gman_bo *bo, unsigned int width, unsigned int height, unsigned int pitch, int bpp) {
    di_fbo_priv_t *fbo;
    struct gman_bo_priv *priv = (struct gman_bo_priv*) bo;
    
    fbo = malloc(sizeof(*fbo));
    if(!fbo)
        return NULL;

    fbo->base.width = width;
    fbo->base.height = height;
    fbo->base.pitch = pitch;
    fbo->base.bpp = bpp;

    fbo->phy_addr = (phy_addr_t) priv->paddr;

    return (fbo_t*)&fbo->base;
}

void gman_deletefbo(struct gman_device *device, fbo_t *fbo) {
    di_fbo_priv_t *priv = (di_fbo_priv_t*) fbo;
    free(priv);
}

#endif /* SUPPORT_DI */
