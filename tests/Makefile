##
## Makefile for microSDL library
## - Calvin Hass
## - http://www.impulseadventure.com/elec/
##

DEBUG = -O3
CC = g++
INCLUDE = -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS = -L/usr/local/lib
LDLIBS = -lSDL -lSDL_ttf -lSDL_image -lts

SRC = test-sdl.c \
	sdl-ex01.c \
	sdl-ex02.c \
	sdl-ex03.c \
	sdl-ex04.c \
	sdl-ex05.c \
	sdl-ex06.c

OBJ = $(SRC:.c=.o)

BINS = $(SRC:.c=)

all: $(BINS)
 
clean:
	@echo "Cleaning directory..."
	$(RM) $(BINS)


test-sdl: test-sdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ test-sdl.c $(LDFLAGS) $(LDLIBS)

sdl-ex01: sdl-ex01.c ../src/microsdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ sdl-ex01.c ../src/microsdl.c $(LDFLAGS) $(LDLIBS) -I . -I ../src

sdl-ex02: sdl-ex02.c ../src/microsdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ sdl-ex02.c ../src/microsdl.c -DINC_TS $(LDFLAGS) $(LDLIBS) -I . -I ../src

sdl-ex03: sdl-ex03.c ../src/microsdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ sdl-ex03.c ../src/microsdl.c -DINC_TS $(LDFLAGS) $(LDLIBS) -I . -I ../src

sdl-ex04: sdl-ex04.c ../src/microsdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ sdl-ex04.c ../src/microsdl.c -DINC_TS $(LDFLAGS) $(LDLIBS) -I . -I ../src

sdl-ex05: sdl-ex05.c ../src/microsdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ sdl-ex05.c ../src/microsdl.c -DINC_TS $(LDFLAGS) $(LDLIBS) -I . -I ../src

sdl-ex06: sdl-ex06.c ../src/microsdl.c
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ sdl-ex06.c ../src/microsdl.c -DINC_TS $(LDFLAGS) $(LDLIBS) -I . -I ../src


