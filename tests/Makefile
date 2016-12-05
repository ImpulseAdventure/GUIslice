##
## Makefile for GUIslice library
## - Calvin Hass
## - http://www.impulseadventure.com/elec/
##

DEBUG = -O3
CC = gcc
INCLUDE = -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g
LDFLAGS = -L/usr/local/lib

GSLC_CORE = ../src/GUIslice.c ../src/GUIslice_ex.c

# ---------------------------------------------------------------------------
# Define compiler flags for GUIslice
# - Select the graphics driver by uncommenting one of the following lines
#
# NOTE: One can override this from the command-line as follows:
#       make gslc-ex04 GSLC_DRV=SDL2
#
GSLC_DRV := SDL1
#GSLC_DRV := SDL2

# === SDL1.2 ===
ifeq (SDL1,${GSLC_DRV})
  # - DRV_INC_TS is optional (and -lts)
  # - DRV_SDL_FIX_START is optional
  $(info GUIslice driver mode: SDL1)
  GSLC_SRCS = ../src/GUIslice_drv_sdl.c
  GSLC_FLAGS = -DDRV_TYPE_SDL1 -DDRV_INC_TS -DDRV_SDL_FIX_START
  LDLIBS = -lSDL -lSDL_ttf -lts -lm
endif

# === SDL2.0 ===
ifeq (SDL2,${GSLC_DRV})
  # - DRV_INC_TS is optional
  $(info GUIslice driver mode: SDL2)
  GSLC_SRCS = ../src/GUIslice_drv_sdl.c
  GSLC_FLAGS = -DDRV_TYPE_SDL2 -DDRV_INC_TS
  LDLIBS = -lSDL2 -lSDL2_ttf -lts -lm
endif

# === Adafruit-GFX ===
# TBD

# ---------------------------------------------------------------------------

# Examples that operate in all driver modes
SRC =   gslc-ex01.c \
	gslc-ex02.c \
	gslc-ex03.c \
	gslc-ex04.c \
	gslc-ex05.c \
	gslc-ex06.c \
	gslc-ex07.c

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
	@$(CC) $(CFLAGS) -o $@ gslc-ex01.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex02: gslc-ex02.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex02.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex03: gslc-ex03.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex03.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex04: gslc-ex04.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex04.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex05: gslc-ex05.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex05.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex06: gslc-ex06.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex06.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex07: gslc-ex07.c $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex07.c $(GSLC_CORE) $(GSLC_SRCS) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

