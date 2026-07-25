# Reproducible cross-compile image for the RomMPL PS2 EE target (ps2/).
#
# The upstream ps2dev/ps2dev:latest image (Alpine 3.24) ships the full
# PS2SDK plus the mips64r5900el-ps2-elf / mipsel-none-elf toolchains, but
# does not include GNU make, so a bare `docker run ps2dev/ps2dev:latest
# make ...` fails with "make: not found". This image adds only `make` on
# top of the base image; the toolchain, PS2SDK, headers, and libraries are
# untouched from upstream.
#
# Build once:
#   docker build -t rommpl-ps2build docker/ps2-build
#
# Then cross-compile ps2/ (see ps2/README.md for the full flow):
#   docker run --rm --user $(id -u):$(id -g) \
#     -v /path/to/RomMPL:/src -w /src/ps2 \
#     rommpl-ps2build sh -c 'make clean && make ROMMPL_HOST=<ip> ROMMPL_PORT=<port>'
#
# Use `sh -c`, not `sh -lc`: a login shell (`-l`) re-execs /etc/profile,
# which resets PATH and drops the toolchain directories baked into this
# image's Docker ENV.
FROM ps2dev/ps2dev:latest

RUN apk add --no-cache make
