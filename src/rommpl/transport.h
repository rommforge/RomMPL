#ifndef ROMMPL_TRANSPORT_H
#define ROMMPL_TRANSPORT_H

#include <stddef.h>

typedef struct RommplTransport RommplTransport;

/* Byte-level connection abstraction. One implementation per platform:
 * mock (tests), POSIX sockets (dev machine), lwIP (PS2, later plan). */
struct RommplTransport {
    int  (*connect)(RommplTransport *t, const char *host, int port); /* 0 ok, <0 error */
    int  (*write)(RommplTransport *t, const char *buf, size_t len);  /* bytes written or <0 */
    int  (*read)(RommplTransport *t, char *buf, size_t len);         /* bytes read, 0 EOF, <0 error */
    void (*close)(RommplTransport *t);
    void *impl;                                                      /* implementation-private */
};

#endif
