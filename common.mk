VERSION = 1.0
REVISION = 1

OUT = bin

DEPS += src/structs.h
DEPS += src/common.h
DEPS += src/defs.h

GAME_SOURCES := $(shell find src/ -mindepth 2 -name '*.c' -not -path 'src/plat/*' | sort)

OBJS := $(addprefix $(OUT)/,$(GAME_SOURCES:%.c=%.o))

GAME_OBJS = $(OBJS) $(OUT)/src/main.o

MAP_OBJS = $(OBJS) $(OUT)/src/mapEditor.o

all: $(PROG) $(MAP_PROG)

$(OUT)/%.o: %.c %.h $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) -rf $(OUT) $(PROG) $(MAP_PROG) $(LOCALE_MO)
