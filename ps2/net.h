#ifndef ROMMPL_PS2_NET_H
#define ROMMPL_PS2_NET_H

/* Bring the SMAP/DEV9 LAN up via DHCP. On success returns 0 and writes the
 * assigned IPv4 dotted-quad into ip_out (at least 16 bytes). Negative on
 * failure (module load, link, or DHCP timeout). */
int ps2_net_up(char ip_out[16]);
void ps2_net_down(void);

#endif
