# udpfsd sidecar for RomMPL

This dockerizes [pcm720/udpfsd](https://github.com/pcm720/udpfsd), a Go UDPFS /
UDPBD server, to serve RomM's PS2 library over the LAN. Neutrino on the PS2
mounts it and boots games by filename, so RomMPL never downloads an ISO; the
console streams disk blocks on demand.

RomMPL writes no serving code of its own. This is stock udpfsd in a container.

## Run

Point it at the same directory RomM stores your PS2 images in, read-only:

```
ROMM_PS2_DIR=/path/to/romm/library/ps2 \
  docker compose -f docker/udpfsd/docker-compose.yml up --build -d
```

The container listens on UDP port **62966** and serves `/fsroot` read-only.
Check it started with `docker compose -f docker/udpfsd/docker-compose.yml logs`.

## Host networking

The compose file uses `network_mode: host` so the PS2 can find udpfsd via UDP on
the LAN. This works on Linux hosts. On Docker Desktop (macOS / Windows) host
networking does not bridge to the physical LAN, so there you run the udpfsd
binary natively instead of in Docker.

## Formats

The default image builds the CSO/ZSO path with `CGO_ENABLED=0` (static, no
dependencies). ISO and ZSO boot with no host-side decoding. udpfsd can also do
on-the-fly CSO/CHD decompression, but **CHD support needs a CGO-enabled build
with libchdr** per the upstream README; swap the build stage in the `Dockerfile`
if your library is CHD.

## Verify (needs a PS2 or emulator)

With Neutrino (udpfs/udpbd backend) on real hardware or PCSX2/ARMSX2, confirm the
served folder is visible and a known filename boots. Record the working Neutrino
argv in `docs/spikes/2026-07-24-filename-parity.md` (the Task 0 spike).
