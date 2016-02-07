# List of all the SAM3X8E platform files.
ifeq ($(USE_SMART_BUILD),yes)
HALCONF := $(strip $(shell cat halconf.h | egrep -e "define"))

PLATFORMSRC := $(CHIBIOS)/os/hal/ports/SAM3X8E/hal_lld.c \
               $(CHIBIOS)/os/hal/ports/SAM3X8E/st_lld.c
ifneq ($(findstring HAL_USE_PAL TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/SAM3X8E/pal_lld.c
endif
ifneq ($(findstring HAL_USE_SERIAL TRUE,$(HALCONF)),)
PLATFORMSRC += $(CHIBIOS)/os/hal/ports/SAM3X8E/serial_lld.c
endif
else
PLATFORMSRC := $(CHIBIOS)/os/hal/ports/SAM3X8E/hal_lld.c \
               $(CHIBIOS)/os/hal/ports/SAM3X8E/st_lld.c \
               $(CHIBIOS)/os/hal/ports/SAM3X8E/pal_lld.c \
               $(CHIBIOS)/os/hal/ports/SAM3X8E/serial_lld.c
endif

# Required include directories
PLATFORMINC := $(CHIBIOS)/os/hal/ports/SAM3X8E
