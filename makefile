PROG = waterCloset
MAP_PROG = mapEditor

CC = gcc

include common.mk

GAME_OBJS += $(OUT)/src/plat/unix/unixInit.o
MAP_OBJS += $(OUT)/src/plat/unix/unixInit.o

NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

CXXFLAGS += `sdl2-config --cflags` -DVERSION=$(VERSION) -DREVISION=$(REVISION) -DDATA_DIR=\"$(DATA_DIR)\"
CXXFLAGS += -Wall -Wempty-body -Werror -Wstrict-prototypes -Werror=maybe-uninitialized -Warray-bounds
CXXFLAGS += -g -lefence

LDFLAGS += `sdl2-config --libs` -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lm

$(PROG): $(GAME_OBJS)
	$(CC) -o $@ $(GAME_OBJS) $(LDFLAGS)

$(MAP_PROG): $(MAP_OBJS)
	$(CC) -o $@ $(MAP_OBJS) $(LDFLAGS)
