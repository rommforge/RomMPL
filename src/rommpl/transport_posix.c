/* POSIX socket implementation of the transport vtable. This is the only file
 * in the project allowed to include POSIX network headers. Under -std=c11 the
 * compiler defines __STRICT_ANSI__, which hides POSIX declarations (getaddrinfo,
 * socket, ...). Request them explicitly: full BSD surface on Apple, POSIX.1-2001
 * elsewhere. A later PS2SDK build replaces this file with an lwIP transport. */
#if defined(__APPLE__)
#define _DARWIN_C_SOURCE
#else
#define _POSIX_C_SOURCE 200112L
#endif

#include "rommpl/transport_posix.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

typedef struct {
    int fd;
} PosixState;

static int px_connect(RommplTransport *t, const char *host, int port) {
    PosixState *s = (PosixState *)t->impl;
    char portstr[16];
    snprintf(portstr, sizeof portstr, "%d", port);

    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, portstr, &hints, &res) != 0) return -1;

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) { freeaddrinfo(res); return -1; }
    if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
        close(fd);
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);
    s->fd = fd;
    return 0;
}

static int px_write(RommplTransport *t, const char *buf, size_t len) {
    PosixState *s = (PosixState *)t->impl;
    size_t off = 0;
    while (off < len) {
        ssize_t n = write(s->fd, buf + off, len - off);
        if (n <= 0) return -1;
        off += (size_t)n;
    }
    return (int)len;
}

static int px_read(RommplTransport *t, char *buf, size_t len) {
    PosixState *s = (PosixState *)t->impl;
    ssize_t n = read(s->fd, buf, len);
    return (int)n;
}

static void px_close(RommplTransport *t) {
    PosixState *s = (PosixState *)t->impl;
    if (s->fd >= 0) {
        close(s->fd);
        s->fd = -1;
    }
}

RommplTransport *posix_transport_new(void) {
    RommplTransport *t = (RommplTransport *)calloc(1, sizeof *t);
    PosixState *s = (PosixState *)calloc(1, sizeof *s);
    s->fd = -1;
    t->connect = px_connect;
    t->write = px_write;
    t->read = px_read;
    t->close = px_close;
    t->impl = s;
    return t;
}

void posix_transport_free(RommplTransport *t) {
    free(t->impl);
    free(t);
}
