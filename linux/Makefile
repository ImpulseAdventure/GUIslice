##
## Makefile for GUIslice library - LINUX
## - Calvin Hass
## - https://www.impulseadventure.com/elec/guislice-gui.html
## - https://github.com/ImpulseAdventure/GUIslice
##
## NOTE: This Makefile handles both SDL1.2 and SDL2 compilation.
##       A simpler Makefile can be used if this flexibility is
##       not required.

# ---------------------------------------------------------------------------
# Define compiler flags for GUIslice
# - In some cases, the makefile compilation options must match
#   settings in the GUIslice_config_linux.h configuration file.
#
# GRAPHICS DRIVER
# - make parameter: GSLC_DRV=(SDL1,SDL2)
# - On the makefile command-line, pass the GSLC_DRV parameter to select
#   the graphics driver matching the #define DRV_DISP_* setting in
#   GUIslice_config_linux.h, eg:
#     GUIslice_config_linux.h:  #define DRV_DISP_SDL1
#     Makefile:                 make <target> GSLC_DRV=SDL1
#
#
# TOUCH DRIVER
# - make parameter: GSLC_TOUCH=(TSLIB,SDL,NONE)
# - On the makefile command-line, pass the GSLC_TOUCH parameter to
#   indicate whether or not DRV_TOUCH_TSLIB is #defined in the
#   GUIslice_config_linux.h, eg:
#     GUIslice_config_linux.h:  #define DRV_TOUCH_TSLIB
#     Makefile:                 make <target> GSLC_TOUCH=TSLIB
#
#     GUIslice_config_linux.h:  #define DRV_TOUCH_SDL (or DRV_TOUCH_NONE)
#     Makefile:                 make <target> GSLC_TOUCH=NONE
# - Note that the makefile only checks to see if DRV_TOUCH=TSLIB and
#   will ignore any other values.
#
# NOTES:
# - Note that multiple parameters may be passed, for example:
#     make gslc-ex04
#     make gslc-ex06 GSLC_TOUCH=TSLIB GSLC_DRV=SDL2
#     make all GSLC_TOUCH=TSLIB
#


DEBUG = -O3
CC = gcc
INCLUDE = -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g
LDFLAGS = -L/usr/local/lib

GSLC_CORE = ../src/GUIslice.c ../src/GUIslice_ex.c ../src/GUIslice_config_linux.h


# Define default compiler flags which may be overridden by command line

# Define graphics driver (overridden by command line GSLC_DRV)
GSLC_DRV := SDL1
#GSLC_DRV := SDL2

# Define any extra linker settings (overridden by command line GSLC_TOUCH_TSLIB)
#GSLC_LDLIB_EXTRA := -lm
#GSLC_LDLIB_EXTRA := -lm -lts

ifndef GSLC_TOUCH
  GSLC_LDLIB_EXTRA := -lm -lts
  $(info GUIslice touch mode: tslib enabled by default)
else
  ifeq (TSLIB,${GSLC_TOUCH})
    $(info GUIslice touch mode: tslib enabled)
    GSLC_LDLIB_EXTRA := -lm -lts
  else
    $(info GUIslice touch mode: tslib not enabled)
    GSLC_LDLIB_EXTRA := -lm
  endif
endif

# === SDL1.2 ===
ifeq (SDL1,${GSLC_DRV})
  $(info GUIslice driver mode: SDL1)
  GSLC_SRCS = ../src/GUIslice_drv_sdl.c
  # - Add extra linker libraries if needed
  LDLIBS = -lSDL -lSDL_ttf ${GSLC_LDLIB_EXTRA}
endif

# === SDL2.0 ===
ifeq (SDL2,${GSLC_DRV})
  $(info GUIslice driver mode: SDL2)
  GSLC_SRCS = ../src/GUIslice_drv_sdl.c
  # - Add extra linker libraries if needed
  LDLIBS = -lSDL2 -lSDL2_ttf ${GSLC_LDLIB_EXTRA}
endif

# === Adafruit-GFX ===
# No makefile for Arduino as most users will use the IDE for compilation

# ---------------------------------------------------------------------------

# Examples that operate in all driver modes
SRC =   gslc-ex01.c \
	gslc-ex02.c \
	gslc-ex03.c \
	gslc-ex04.c \
	gslc-ex05.c \
	gslc-ex06.c \
	gslc-ex07.c \
	gslc-ex09.c \
	gslc-ex10.c \
	gslc-ex11.c \
	gslc-ex18.c

# Add simple example for specific driver modes
ifeq (SDL1,${GSLC_DRV})
  SRC += test-sdl1.c
endif
ifeq (SDL2,${GSLC_DRV})
  SRC += test-sdl2.c
endif


OBJ = $(SRC:.c=.o)

BINS = $(SRC:.c=)

all: $(BINS)

clean:
	@echo "Cleaning directory..."
	$(RM) $(BINS)


test-sdl1: test-sdl1.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ test-sdl1.c $(LDFLAGS) -lSDL

test-sdl2: test-sdl2.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ test-sdl2.c $(LDFLAGS) -lSDL2

gslc-ex01: gslc-ex01.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex01.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex02: gslc-ex02.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex02.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex03: gslc-ex03.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex03.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex04: gslc-ex04.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex04.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex05: gslc-ex05.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex05.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex06: gslc-ex06.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex06.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -lm -I . -I ../src

gslc-ex07: gslc-ex07.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex07.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex09: gslc-ex09.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex09.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex10: gslc-ex10.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex10.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex11: gslc-ex11.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex11.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex18: gslc-ex18.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex18.c $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../src