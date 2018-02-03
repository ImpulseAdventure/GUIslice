##
## Makefile for GUIslice library - LINUX
## - Calvin Hass
## - https://www.impulseadventure.com/elec/guislice-gui.html
## - https://github.com/ImpulseAdventure/GUIslice
##
## NOTE: This Makefile handles both SDL1.2 and SDL2 compilation.
##       A simpler Makefile can be used if this flexibility is
##       not required.

DEBUG = -O3
CC = gcc
INCLUDE = -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g
LDFLAGS = -L/usr/local/lib

GSLC_CORE = ../src/GUIslice.c ../src/GUIslice_ex.c ../src/GUIslice_config_linux.h

# ---------------------------------------------------------------------------
# Define compiler flags for GUIslice
# - Select the graphics driver by uncommenting one of the following lines
#   and edit GUIslice_config.h to set the appropriate #define DRV_DISP_*
#   Example for SDL1:
#     Makefile:            GSLC_DRV := SDL1
#     GUIslice_config.h:   #define DRV_DISP_SDL1
#
# NOTE: One can also override this from the command-line as follows:
#       make gslc-ex04 GSLC_DRV=SDL2
#
GSLC_DRV := SDL1
#GSLC_DRV := SDL2

# Define any extra linker settings
# - For tslib (GUIslice_config.h has #define DRV_TOUCH_TSLIB), add "-lts"
#GSLC_LDLIB_EXTRA := -lm
GSLC_LDLIB_EXTRA := -lm -lts

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
	gslc-ex11.c

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
