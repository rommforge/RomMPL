#include "rommpl/rommapi.h"
#include "rommpl/transport_posix.h"
#include <stdio.h>
#include <stdlib.h>

static int print_rom(const RomEntry *e, void *u) {
    int *n = (int *)u;
    (*n)++;
    printf("  [%u] %-40s -> %s (%llu bytes)\n",
           e->id, e->name, e->fs_name, (unsigned long long)e->fs_size_bytes);
    return 0;
}

int main(void) {
    const char *host = getenv("ROMM_HOST");
    const char *token = getenv("ROMM_TOKEN");
    const char *pid = getenv("ROMM_PS2_PLATFORM_ID");
    const char *port = getenv("ROMM_PORT");
    if (!host || !pid) {
        printf("SKIP: set ROMM_HOST, ROMM_PS2_PLATFORM_ID (and ROMM_TOKEN, ROMM_PORT)\n");
        return 0;
    }
    RommplTransport *t = posix_transport_new();
    RommApi api = { t, host, port ? atoi(port) : 8080, token };
    int n = 0;
    int total = rommapi_list_platform_roms(&api, atoi(pid), 25, print_rom, &n);
    posix_transport_free(t);
    printf("listed %d roms (returned %d)\n", n, total);
    return total < 0 ? 1 : 0;
}
