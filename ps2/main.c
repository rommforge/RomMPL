#include "net.h"
#include <stdio.h>

int main(void) {
    char ip[16];
    printf("RomMPL net check: bringing up LAN...\n");
    if (ps2_net_up(ip) != 0) {
        printf("RomMPL net check: FAILED to bring up network\n");
        return 1;
    }
    printf("RomMPL net check: DHCP up, IP = %s\n", ip);
    ps2_net_down();
    printf("RomMPL net check: done\n");
    return 0;
}
