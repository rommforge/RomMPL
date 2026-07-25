# RomMPL, the RomM PS2 Loader

A native PlayStation 2 homebrew client for [RomM](https://github.com/rommapp/romm).

Browse your self-hosted RomM PS2 library on-console with box art, pick a game, and
boot it **over the network** via [Neutrino](https://github.com/rickgaiser/neutrino)
plus UDPBD. No downloading; the console streams disk blocks on demand.

- RomM stays the source of truth for library plus metadata (plain HTTP on the LAN).
- A stock `udpbd-server` serves the PS2 folder; Neutrino boots it.
- The only new software is the on-console ELF.
