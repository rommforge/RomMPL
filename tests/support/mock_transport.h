#ifndef ROMMPL_MOCK_TRANSPORT_H
#define ROMMPL_MOCK_TRANSPORT_H

#include "rommpl/transport.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char **responses;
    int          n;
    int          cur;        /* index of response for the active connection */
    size_t       read_pos;   /* read cursor within current response */
    char         written[4096];
    size_t       written_len;
    int          write_limit;  /* 0 = no limit, >0 = max bytes per write (for testing short writes) */
} MockState;

static int mock_connect(RommplTransport *t, const char *host, int port) {
    (void)host; (void)port;
    MockState *s = (MockState *)t->impl;
    if (s->cur >= s->n) return -1;
    s->read_pos = 0;
    s->written_len = 0;
    return 0;
}

static int mock_write(RommplTransport *t, const char *buf, size_t len) {
    MockState *s = (MockState *)t->impl;
    size_t room = sizeof s->written - 1 - s->written_len;
    size_t cp = len < room ? len : room;
    /* If write_limit is set, cap the copy to that many bytes (for testing short writes) */
    if (s->write_limit > 0 && cp > (size_t)s->write_limit) {
        cp = (size_t)s->write_limit;
    }
    memcpy(s->written + s->written_len, buf, cp);
    s->written_len += cp;
    s->written[s->written_len] = '\0';
    return (int)cp;
}

static int mock_read(RommplTransport *t, char *buf, size_t len) {
    MockState *s = (MockState *)t->impl;
    if (s->cur >= s->n || s->read_pos > strlen(s->responses[s->cur])) return -1;
    const char *resp = s->responses[s->cur];
    size_t avail = strlen(resp) - s->read_pos;
    size_t cp = len < avail ? len : avail;
    memcpy(buf, resp + s->read_pos, cp);
    s->read_pos += cp;
    return (int)cp;
}

static void mock_close(RommplTransport *t) {
    MockState *s = (MockState *)t->impl;
    s->cur++;            /* next connect serves the next response */
    s->read_pos = 0;     /* reset read cursor for safety */
}

static RommplTransport *mock_transport_new(const char **responses, int n) {
    RommplTransport *t = (RommplTransport *)calloc(1, sizeof *t);
    MockState *s = (MockState *)calloc(1, sizeof *s);
    s->responses = responses; s->n = n; s->cur = 0; s->write_limit = 0;
    t->connect = mock_connect; t->write = mock_write;
    t->read = mock_read; t->close = mock_close; t->impl = s;
    return t;
}

/* Create a mock transport with a write limit (for testing partial writes) */
static __attribute__((unused)) RommplTransport *mock_transport_new_with_write_limit(const char **responses, int n, int limit) {
    RommplTransport *t = mock_transport_new(responses, n);
    MockState *s = (MockState *)t->impl;
    s->write_limit = limit;
    return t;
}

static const char *mock_transport_written(RommplTransport *t) {
    return ((MockState *)t->impl)->written;
}

static void mock_transport_free(RommplTransport *t) {
    free(t->impl); free(t);
}

#endif
