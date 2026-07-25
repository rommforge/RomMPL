EE_BIN = rommpl_netcheck.elf

ROMMPL_SRC = ../src/rommpl
EE_INCS += -I$(ROMMPL_SRC)/.. -I../third_party

EE_OBJS = main.o net.o \
          rommpl_http.o rommpl_rom_parser.o rommpl_rommapi.o \
          rommpl_platform_parser.o rommpl_jsmn_util.o rommpl_transport_lwip.o \
          DEV9_irx.o NETMAN_irx.o SMAP_irx.o

EE_LIBS = -lnetman -lps2ip -ldebug -lpatches

# Enforce a zero-warning build: Makefile.eeglobal only sets -Wall by default,
# so without -Werror a warning would not fail the build.
EE_WARNFLAGS = -Wall -Werror

# RomM connection config, supplied as make variables (never committed):
#   make ROMMPL_HOST=192.168.1.10 ROMMPL_PORT=8080 ROMMPL_TOKEN=xxxxx
# All three are optional; the build compiles clean with none of them set
# (main.c falls back to a placeholder host/port and a NULL token).
ROMMPL_HOST ?=
ROMMPL_PORT ?=
ROMMPL_TOKEN ?=
ifneq ($(ROMMPL_HOST),)
EE_CFLAGS += -DROMMPL_HOST=\"$(ROMMPL_HOST)\"
endif
ifneq ($(ROMMPL_PORT),)
EE_CFLAGS += -DROMMPL_PORT=$(ROMMPL_PORT)
endif
ifneq ($(ROMMPL_TOKEN),)
EE_CFLAGS += -DROMMPL_TOKEN=\"$(ROMMPL_TOKEN)\"
endif

all: $(EE_BIN)

# Compile the shared harness sources straight from ../src/rommpl, with
# prefixed object names (rommpl_*.o) so they can't collide with main.o/net.o
# and so this pattern rule is tried (and its prerequisite found) before the
# generic %.o: %.c rule that Makefile.eeglobal defines below.
rommpl_%.o: $(ROMMPL_SRC)/%.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

DEV9_irx.c: $(PS2SDK)/iop/irx/ps2dev9.irx
	bin2c $< DEV9_irx.c DEV9_irx

NETMAN_irx.c: $(PS2SDK)/iop/irx/netman.irx
	bin2c $< NETMAN_irx.c NETMAN_irx

SMAP_irx.c: $(PS2SDK)/iop/irx/smap.irx
	bin2c $< SMAP_irx.c SMAP_irx

clean:
	rm -f $(EE_BIN) $(EE_OBJS) DEV9_irx.c NETMAN_irx.c SMAP_irx.c

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
