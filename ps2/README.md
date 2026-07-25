# ps2/ -- RomMPL PS2 netcheck

Cross-compiled PS2 EE homebrew that:

1. Brings up the SMAP/DEV9 Ethernet adapter via DHCP (`net.c`).
2. Resolves the RomM `ps2` platform id (`GET /api/platforms`).
3. Lists every rom for that platform (`GET /api/roms?platform_id=...`, paged).
4. Prints each rom's id, display name, on-disk filename, and size to the EE
   console.

This build is **console output only**: there is no on-screen UI, menu, or
game launcher here. It is the network + parsing pipeline (the shared
`src/rommpl` harness, cross-compiled) proven out end to end on real PS2
target code, gated by a clean compile. It has not been run against a real
RomM server or PS2 hardware/emulator in this environment (no PS2 hardware or
DEV9-capable emulator was available); see "What's verified" below.

RomM must be reachable over **plain HTTP** (no TLS) at the configured
host/port -- the transport is a bare TCP socket plus a hand-rolled HTTP/1.1
GET, with no TLS support.

## Build

The build needs the `mips64r5900el-ps2-elf-*` toolchain, PS2SDK, and GNU
`make`. `ps2dev/ps2dev:latest` has the toolchain and PS2SDK but not `make`,
so this repo ships a one-line derived image that adds it.

### 1. Build the build image (once)

```bash
docker build -t rommpl-ps2build docker/ps2-build
```

This only adds the `make` package on top of `ps2dev/ps2dev:latest`; the
toolchain, PS2SDK, headers, and libraries are exactly what ps2dev publishes.

### 2. Cross-compile the ELF

```bash
docker run --rm --user $(id -u):$(id -g) \
  -v /absolute/path/to/RomMPL:/src -w /src/ps2 \
  rommpl-ps2build sh -c 'make clean && make ROMMPL_HOST=192.168.1.10 ROMMPL_PORT=8080'
```

Notes:

- Use `sh -c`, not `sh -lc`. A login shell (`-l`) re-sources `/etc/profile`,
  which resets `PATH` and drops the toolchain directories baked into the
  image's `ENV PATH`; `sh -c` (non-login) preserves it.
- `--user $(id -u):$(id -g)` keeps output files owned by you instead of
  root, since the container's default user is root.
- Output is `ps2/rommpl_netcheck.elf`. Build artifacts (`*.elf`, `*.o`,
  `*_irx.c`) are gitignored; only source and the Makefile are committed.

### Config: `ROMMPL_HOST` / `ROMMPL_PORT` / `ROMMPL_TOKEN`

RomM connection details are supplied **only** as `make` variables at build
time, compiled in as `-D` defines. They are never hardcoded in source and
never committed:

```bash
make ROMMPL_HOST=192.168.1.10 ROMMPL_PORT=8080 ROMMPL_TOKEN=your-token-here
```

- `ROMMPL_HOST` / `ROMMPL_PORT`: address of the RomM server on your LAN.
- `ROMMPL_TOKEN`: optional bearer token; omit it if your RomM instance
  does not require auth for the API routes used here.
- All three are optional. `make` with none of them set still compiles
  cleanly (`main.c` falls back to a placeholder host/port and a `NULL`
  token) -- the build does **not** require a token to succeed.

Rebuilding with different values requires `make clean` first, since the
values are baked in as preprocessor defines, not read at runtime.

## Running it

### PCSX2 / ARMSX2 (emulator)

1. In emulator settings, enable the DEV9 Ethernet device and bind it to
   your host's network adapter (PCSX2: Settings -> DEV9 Ethernet -> Ethernet
   Device -> select your active NIC; mode: typically "Auto"/TAP so the PS2
   client gets a DHCP lease from your LAN, not a virtual/host-only network,
   since it needs to actually reach the RomM server's LAN address).
2. Load `ps2/rommpl_netcheck.elf` directly as the boot ELF (File -> Boot ELF
   in PCSX2, or the equivalent in ARMSX2).
3. Watch the EE console/log output. Expected sequence: bringing up the LAN,
   DHCP IP assignment, the resolved `ps2` platform id, then one line per rom
   with a final count, or an explicit `FAILED` line if link/DHCP/HTTP
   negotiation didn't succeed.

### Real hardware

1. Get `rommpl_netcheck.elf` onto the memory card or a USB/network-visible
   location the console can boot from, using your existing homebrew loader
   of choice (FreeMcBoot / PS2BBL / wLaunchELF, or an ELF launched from
   Neutrino/OPL's homebrew menu).
2. Ensure the PS2's Ethernet port is wired into the same LAN segment as the
   RomM server, and that the RomM server is reachable over plain HTTP at
   the host/port baked into this build.
3. Launch the ELF. It DHCPs on `sm0` (the SMAP/DEV9 interface), then talks
   to RomM. Console output (EE `printf`, i.e. whatever your loader/BIOS
   routes stdout to -- typically visible over a debug/serial link, or
   `ps2link`/`ps2client` if you loaded it that way) shows the same sequence
   as the emulator case above.

## What's verified

- **Compile-clean**: this build is gated by a zero-warning, zero-error
  cross-compile (`-Wall -Werror`) linking the full shared harness
  (`http.c`, `rom_parser.c`, `rommapi.c`, `platform_parser.c`,
  `jsmn_util.c`), `transport_lwip.c`, `net.c`, and `main.c`, against the
  real PS2SDK headers and libraries, plus the embedded `DEV9`/`NETMAN`/
  `SMAP` IRX modules. See the parent repo's task reports for the exact
  build command and output.
- **Not verified here**: actual DHCP lease acquisition, actual HTTP
  round-trips against a live RomM instance, and rom listing output, since
  no PS2 hardware or DEV9-capable emulator was available in this
  environment. The PC-side harness (`src/rommpl`, built via the top-level
  `Makefile`) exercises the same HTTP/JSON logic against a real or mocked
  RomM server on the dev machine (`make test`, `make itest`); this PS2
  build is the identical logic, cross-compiled and linked against a
  different transport (`transport_lwip.c` instead of `transport_posix.c`).
  Confirming an actual game list on hardware/emulator is the operator's
  next step, not part of this build-verification gate.
