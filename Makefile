MAJOR := 0
MINOR := 1
NAME := drm_gman
VERSION := $(MAJOR).$(MINOR)

LIBDRM_GMAN_FILES := \
	gman_device.c \
	gman_bo.c  \

LIBDRM_GMAN_OBJ_FILES := $(LIBDRM_GMAN_FILES:%.c=%.o)

UAPI_HEADER := gman_drm.h
UAPIIF_HEADER := gman_drmif.h

CFLAGS += -shared -fPIC -MMD -Wall -I../../../../interface/src
CFLAGS += $(shell pkg-config --cflags libdrm)

.PHONY:
all : lib$(NAME).so

lib$(NAME).so : lib$(NAME).so.$(MAJOR)
	ln -fs $^ $@

lib$(NAME).so.$(MAJOR) : lib$(NAME).so.$(VERSION)
	ln -fs $^ $@

lib$(NAME).so.$(VERSION) : $(LIBDRM_GMAN_OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-soname,lib$(NAME).so.$(MAJOR) -o $@ $^

.PHONY:
clean :
	-rm *.o *.d lib$(NAME).so*

.PHONY:
deploy : all
#	cp lib$(NAME).so.$(VERSION) $(SDKTARGETSYSROOT)/usr/lib/
#	cp $(UAPI_HEADER) $(SDKTARGETSYSROOT)/usr/include/
#	cp $(UAPIIF_HEADER) $(SDKTARGETSYSROOT)/usr/include/
#	cp $(UAPI_HEADER) $(SDKTARGETSYSROOT)/usr/src/kernel/include/uapi/drm/
#	cp ../../../../interface/src/*.h $(SDKTARGETSYSROOT)/usr/include/
#	cp ../../../../interface/src/*.h $(SDKTARGETSYSROOT)/usr/src/kernel/include/
	scp  lib$(NAME).so.$(VERSION) root@$(BOARD_IP):/usr/lib/

.PHONY:
scp: lib$(NAME).so.$(VERSION)
	$(STRIP) lib$(NAME).so.$(VERSION)
	scp lib$(NAME).so.$(VERSION) $(SCP_USER)@$(SCP_HOST):$(SCP_PATH)
