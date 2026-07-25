#include "net.h"
#include "rommpl/rommapi.h"
#include "rommpl/transport_lwip.h"
#include <stdio.h>
#include <stdint.h>

/* RomM connection config. Supplied at build time via ps2/Makefile's
 * ROMMPL_HOST / ROMMPL_PORT / ROMMPL_TOKEN make variables (never committed);
 * these #ifndef defaults just keep the build self-contained (compiles with
 * no make vars at all, no token required). */
#ifndef ROMMPL_HOST
#define ROMMPL_HOST "192.168.1.10"
#endif
#ifndef ROMMPL_PORT
#define ROMMPL_PORT 8080
#endif
#ifndef ROMMPL_TOKEN
#define ROMMPL_TOKEN NULL
#endif

static int print_rom(const RomEntry *e, void *u) {
    int *n = (int *)u;
    (*n)++;
    /* uint32_t is a 32-bit `long` on the EE MIPS ABI, so %u (which expects
     * plain unsigned int) mismatches under -Wformat -Werror; cast instead
     * of switching to %lu so this stays portable across ABIs where
     * uint32_t is a plain unsigned int (e.g. the PC harness, if it ever
     * printed these). */
    printf("  [%u] %s -> %s (%llu bytes)\n",
           (unsigned int)e->id, e->name, e->fs_name,
           (unsigned long long)e->fs_size_bytes);
    return 0;
}

int main(void) {
    char ip[16];
    int n = 0;
    int total = 0;
    uint32_t ps2_id = 0;
    RommplTransport *t;
    RommApi api;
    int rr;

    printf("RomMPL: bringing up LAN...\n");
    if (ps2_net_up(ip) != 0) {
        printf("RomMPL: network bring-up FAILED\n");
        return 1;
    }
    printf("RomMPL: DHCP up, IP = %s, talking to %s:%d\n", ip, ROMMPL_HOST, ROMMPL_PORT);

    t = lwip_transport_new();
    api.transport = t;
    api.host = ROMMPL_HOST;
    api.port = ROMMPL_PORT;
    api.token = ROMMPL_TOKEN;

    rr = rommapi_resolve_platform_id(&api, "ps2", &ps2_id);
    if (rr != 0) {
        printf("RomMPL: could not resolve ps2 platform (rc=%d)\n", rr);
        goto done;
    }
    printf("RomMPL: ps2 platform_id = %u\n", (unsigned int)ps2_id);

    total = rommapi_list_platform_roms(&api, (int)ps2_id, 50, print_rom, &n);
    printf("RomMPL: listed %d roms (rc=%d)\n", n, total);

done:
    lwip_transport_free(t);
    ps2_net_down();
    printf("RomMPL: done\n");
    return 0;
}
