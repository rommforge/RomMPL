/* lwIP (PS2SDK ps2ip) socket implementation of the transport vtable. Mirrors
 * transport_posix.c's contract exactly (connect/write-all-loop/read/close),
 * just over the EE target's BSD-socket-shaped ps2ip API instead of the host
 * OS's sockets.
 *
 * This is built ONLY for the PS2 EE target. The ps2sdk build system always
 * compiles with -D_EE (see $PS2SDK/samples/Makefile.eeglobal), so that macro
 * gates the real implementation below. This file lives in src/rommpl/ next
 * to the rest of the shared harness, which means the top-level harness
 * Makefile's source wildcard (src/rommpl dot-c) also picks it up for the PC
 * `make test` build; on that host build _EE is undefined, so this
 * translation unit compiles to a deliberate no-op instead of pulling in
 * PS2-only network headers (ps2ip.h, netman.h, ...) that do not exist
 * there. */
#if defined(_EE)

#include "rommpl/transport_lwip.h"

#include <ps2ip.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int fd;
} LwipState;

static int lw_connect(RommplTransport *t, const char *host, int port) {
    LwipState *s = (LwipState *)t->impl;
    char portstr[16];
    snprintf(portstr, sizeof portstr, "%d", port);

    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, portstr, &hints, &res) != 0) return -1;

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) {
        freeaddrinfo(res);
        return -1;
    }
    if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
        close(fd);
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);
    s->fd = fd;
    return 0;
}

static int lw_write(RommplTransport *t, const char *buf, size_t len) {
    LwipState *s = (LwipState *)t->impl;
    size_t off = 0;
    while (off < len) {
        ssize_t n = send(s->fd, buf + off, len - off, 0);
        if (n <= 0) return -1;
        off += (size_t)n;
    }
    return (int)len;
}

static int lw_read(RommplTransport *t, char *buf, size_t len) {
    LwipState *s = (LwipState *)t->impl;
    ssize_t n = recv(s->fd, buf, len, 0);
    return (int)n;
}

static void lw_close(RommplTransport *t) {
    LwipState *s = (LwipState *)t->impl;
    if (s->fd >= 0) {
        close(s->fd);
        s->fd = -1;
    }
}

RommplTransport *lwip_transport_new(void) {
    RommplTransport *t = (RommplTransport *)calloc(1, sizeof *t);
    LwipState *s = (LwipState *)calloc(1, sizeof *s);
    s->fd = -1;
    t->connect = lw_connect;
    t->write = lw_write;
    t->read = lw_read;
    t->close = lw_close;
    t->impl = s;
    return t;
}

void lwip_transport_free(RommplTransport *t) {
    free(t->impl);
    free(t);
}

#else /* !_EE: host build (harness `make test`); intentionally no code here */

typedef int rommpl_transport_lwip_host_placeholder;

#endif /* _EE */
