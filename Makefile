MAJOR := 0
MINOR := 1
NAME := drm_gman
VERSION := $(MAJOR).$(MINOR)

LIBDRM_GMAN_FILES := \
	gman_device.c \
	gman_bo.c  \
	gman_fbo.c  \

LIBDRM_GMAN_OBJ_FILES := $(LIBDRM_GMAN_FILES:%.c=%.o)

UAPI_HEADER := gman_drm.h
UAPIIF_HEADER := gman_drmif.h

CFLAGS += -shared -fPIC -MMD -Wall -I../../../../interface/src
CFLAGS += $(shell pkg-config --cflags libdrm)
CFLAGS += -ldrm

# Optional support for DI FBOs
CFLAGS += -DSUPPORT_DI
CFLAGS += -I$(DI_PATH)/inc -I$(DI_PATH)/src/linux_genip

LDFLAGS += \
	-Wl,--no-undefined \
	-Wl,-ldrm

.PHONY:
all : lib$(NAME).so

lib$(NAME).so : lib$(NAME).so.$(MAJOR)
	ln -fs $^ $@

lib$(NAME).so.$(MAJOR) : lib$(NAME).so.$(VERSION)
	ln -fs $^ $@

lib$(NAME).so.$(VERSION) : $(LIBDRM_GMAN_OBJ_FILES)
	$(CC) $(CFLAGS) -Wl,-soname,lib$(NAME).so.$(MAJOR) -o $@ $^ $(LDFLAGS)

.PHONY:
clean :
	-rm *.o *.d lib$(NAME).so*

.PHONY:
deploy : all
	scp  lib$(NAME).so.$(VERSION) root@$(BOARD_IP):/usr/lib/
