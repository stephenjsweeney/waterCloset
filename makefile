PROG = waterCloset
MAP_PROG = mapEditor

CC = gcc
PREFIX ?= /usr
BIN_DIR ?= $(PREFIX)/bin
DATA_DIR ?= /opt/$(PROG)
LOCALE_DIR = $(PREFIX)/share/locale
ICON_DIR = $(PREFIX)/share/icons/hicolor
DESKTOP_DIR = $(PREFIX)/share/applications

DESTDIR ?=
INST_BIN_DIR = $(DESTDIR)$(BIN_DIR)
INST_DATA_DIR = $(DESTDIR)$(DATA_DIR)
INST_LOCALE_DIR = $(DESTDIR)$(LOCALE_DIR)
INST_ICON_DIR = $(DESTDIR)$(ICON_DIR)
INST_DESKTOP_DIR = $(DESTDIR)$(DESKTOP_DIR)

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

install:
	mkdir -p $(INST_BIN_DIR)
	install -m 0755 $(PROG) $(INST_BIN_DIR)
	mkdir -p $(INST_DATA_DIR)
	cp -r data $(INST_DATA_DIR)
	cp -r gfx $(INST_DATA_DIR)
	cp -r music $(INST_DATA_DIR)
	cp -r sound $(INST_DATA_DIR)
	cp -r fonts $(INST_DATA_DIR)
	mkdir -p $(INST_ICON_DIR)/16x16/apps
	mkdir -p $(INST_ICON_DIR)/32x32/apps
	mkdir -p $(INST_ICON_DIR)/64x64/apps
	mkdir -p $(INST_ICON_DIR)/128x128/apps
	cp -p icons/$(PROG)-16x16.png $(INST_ICON_DIR)/16x16/apps/$(PROG).png
	cp -p icons/$(PROG)-32x32.png $(INST_ICON_DIR)/32x32/apps/$(PROG).png
	cp -p icons/$(PROG)-64x64.png $(INST_ICON_DIR)/64x64/apps/$(PROG).png
	cp -p icons/$(PROG)-128x128.png $(INST_ICON_DIR)/128x128/apps/$(PROG).png
	mkdir -p $(INST_DESKTOP_DIR)
	cp -p icons/$(PROG).desktop $(INST_DESKTOP_DIR)

uninstall:
	$(RM) $(BIN_DIR)/$(PROG)
	$(RM) -rf $(DATA_DIR)
	$(RM) $(ICON_DIR)/16x16/apps/$(PROG).png
	$(RM) $(ICON_DIR)/32x32/apps/$(PROG).png
	$(RM) $(ICON_DIR)/64x64/apps/$(PROG).png
	$(RM) $(ICON_DIR)/128x128/apps/$(PROG).png
	$(RM) $(DESKTOP_DIR)/$(PROG).desktop

.PHONY: dist
