EE_BIN = net_check.elf
EE_OBJS = main.o net.o DEV9_irx.o NETMAN_irx.o SMAP_irx.o
EE_LIBS = -lnetman -lps2ip -ldebug -lpatches

# Enforce a zero-warning build: Makefile.eeglobal only sets -Wall by default,
# so without -Werror a warning would not fail the build.
EE_WARNFLAGS = -Wall -Werror

all: $(EE_BIN)

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
