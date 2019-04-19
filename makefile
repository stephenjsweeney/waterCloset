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

SHARED_FILES = LICENSE README.md data gfx music sound fonts
DIST_FILES = $(SHARED_FILES) $(PROG)
SRC_DIST_FILES = $(SHARED_FILES) src makefile* common.mk

$(PROG): $(GAME_OBJS)
	$(CC) -o $@ $(GAME_OBJS) $(LDFLAGS)

$(MAP_PROG): $(MAP_OBJS)
	$(CC) -o $@ $(MAP_OBJS) $(LDFLAGS)

# prepare an archive for the program
dist:
	$(RM) -rf $(PROG)-$(VERSION).$(REVISION)
	mkdir $(PROG)-$(VERSION).$(REVISION)
	cp -rL $(DIST_FILES) $(PROG)-$(VERSION).$(REVISION)
	tar czf $(PROG)-$(VERSION).$(REVISION).linux-x86.tar.gz $(PROG)-$(VERSION).$(REVISION)
	mkdir -p dist
	mv $(PROG)-$(VERSION).$(REVISION).linux-x86.tar.gz dist
	$(RM) -rf $(PROG)-$(VERSION).$(REVISION)
	
# prepare an archive for the program
src-dist:
	$(RM) -rf $(PROG)-$(VERSION).$(REVISION)
	mkdir $(PROG)-$(VERSION).$(REVISION)
	cp -rL $(SRC_DIST_FILES) $(PROG)-$(VERSION).$(REVISION)
	git log --pretty=format:"%h%x09%an%x09%ad%x09%s" --date=short >$(PROG)-$(VERSION).$(REVISION)/CHANGELOG.raw
	tar czf $(PROG)-$(VERSION).$(REVISION).src.tar.gz $(PROG)-$(VERSION).$(REVISION)
	mkdir -p dist
	mv $(PROG)-$(VERSION).$(REVISION).src.tar.gz dist
	$(RM) -rf $(PROG)-$(VERSION).$(REVISION)
