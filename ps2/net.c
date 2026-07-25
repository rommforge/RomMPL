#include "net.h"

#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <sbv_patches.h>
#include <netman.h>
#include <ps2ip.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern unsigned char DEV9_irx[];   extern unsigned int size_DEV9_irx;
extern unsigned char NETMAN_irx[]; extern unsigned int size_NETMAN_irx;
extern unsigned char SMAP_irx[];   extern unsigned int size_SMAP_irx;

static int wait_link(void) {
    for (int i = 0; i < 10; i++) {
        if (NetManIoctl(NETMAN_NETIF_IOCTL_GET_LINK_STATUS, NULL, 0, NULL, 0) == NETMAN_NETIF_ETH_LINK_STATE_UP)
            return 0;
        sleep(1);
    }
    return -1;
}

static int wait_dhcp(t_ip_info *info) {
    for (int i = 0; i < 10; i++) {
        if (ps2ip_getconfig("sm0", info) >= 0 &&
            (info->dhcp_enabled == 0 || info->dhcp_status == DHCP_STATE_BOUND))
            return 0;
        sleep(1);
    }
    return -1;
}

int ps2_net_up(char ip_out[16]) {
    struct ip4_addr ip, nm, gw;
    t_ip_info info;

    SifInitRpc(0);
    while (!SifIopReset("", 0)) {}
    while (!SifIopSync()) {}
    SifInitRpc(0);
    SifLoadFileInit();
    SifInitIopHeap();
    sbv_patch_enable_lmb();

    if (SifExecModuleBuffer(DEV9_irx, size_DEV9_irx, 0, NULL, NULL) < 0) return -1;
    if (SifExecModuleBuffer(NETMAN_irx, size_NETMAN_irx, 0, NULL, NULL) < 0) return -1;
    if (SifExecModuleBuffer(SMAP_irx, size_SMAP_irx, 0, NULL, NULL) < 0) return -1;

    if (NetManInit() != 0) return -1;

    ip4_addr_set_zero(&ip);
    ip4_addr_set_zero(&nm);
    ip4_addr_set_zero(&gw);
    ps2ipInit(&ip, &nm, &gw);

    /* enable DHCP */
    ps2ip_getconfig("sm0", &info);
    info.dhcp_enabled = 1;
    ps2ip_setconfig(&info);

    if (wait_link() != 0) return -1;
    if (wait_dhcp(&info) != 0) return -1;

    ps2ip_getconfig("sm0", &info);
    {
        /* info.ipaddr is a struct in_addr; ip4_addrN() expects a struct
         * ip4_addr. Both are a single u32, but reinterpreting the pointer
         * (as the tcpip-dhcp sample does) violates strict-aliasing under
         * -O2 -Wall. Copy the bits into a real struct ip4_addr instead. */
        struct ip4_addr addr;
        memcpy(&addr, &info.ipaddr, sizeof(addr));
        snprintf(ip_out, 16, "%u.%u.%u.%u",
                 ip4_addr1(&addr), ip4_addr2(&addr),
                 ip4_addr3(&addr), ip4_addr4(&addr));
    }
    return 0;
}

void ps2_net_down(void) {
    ps2ipDeinit();
    NetManDeinit();
    SifExitRpc();
}
