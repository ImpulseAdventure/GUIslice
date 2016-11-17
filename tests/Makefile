##
## Makefile for GUIslice library
## - Calvin Hass
## - http://www.impulseadventure.com/elec/
##

DEBUG = -O3
CC = gcc
INCLUDE = -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g

# Define compiler flags for GUIslice
GSLC_FLAGS = -DINC_TS -DVT_WRK_EN
GSLC_CORE = ../src/GUIslice.c ../src/GUIslice_ex.c

# Select display driver for GUIslice
GSLC_DRV = ../src/GUIslice_drv_sdl1.c

LDFLAGS = -L/usr/local/lib
LDLIBS = -lSDL -lSDL_ttf -lSDL_image -lts -lm

SRC = test-sdl.c \
	gslc-ex01.c \
	gslc-ex02.c \
	gslc-ex03.c \
	gslc-ex04.c \
	gslc-ex05.c \
	gslc-ex06.c \
	gslc-ex07.c
	

OBJ = $(SRC:.c=.o)

BINS = $(SRC:.c=)

all: $(BINS)
 
clean:
	@echo "Cleaning directory..."
	$(RM) $(BINS)


test-sdl: test-sdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ test-sdl.c $(LDFLAGS) $(LDLIBS)

gslc-ex01: gslc-ex01.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex01.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex02: gslc-ex02.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex02.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex03: gslc-ex03.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex03.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex04: gslc-ex04.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex04.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex05: gslc-ex05.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex05.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex06: gslc-ex06.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex06.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

gslc-ex07: gslc-ex07.c $(GSLC_CORE) $(GSLC_DRV)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ gslc-ex07.c $(GSLC_CORE) $(GSLC_DRV) $(GSLC_FLAGS) $(LDFLAGS) $(LDLIBS) -I . -I ../src

