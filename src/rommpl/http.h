#ifndef ROMMPL_HTTP_H
#define ROMMPL_HTTP_H

#include "rommpl/transport.h"
#include <stddef.h>

typedef struct {
    int    status;      /* HTTP status code */
    char  *body;        /* malloc'd, NUL-terminated */
    size_t body_len;    /* body length excluding the terminator */
} HttpResponse;

/* GET `path` over an already-connected transport `t`. Sends Host and, if
 * bearer_token is non-NULL, an Authorization: Bearer header. Fills *resp.
 * Returns 0 on success, negative on transport or protocol error. On a
 * negative return, *resp is left untouched and must NOT be passed to
 * http_response_free; only a 0 return yields a valid, freeable resp. */
int http_get(RommplTransport *t, const char *host, const char *path,
             const char *bearer_token, HttpResponse *resp);

void http_response_free(HttpResponse *resp);

#endif
