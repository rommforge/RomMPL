#include "rommpl/http.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* Drain the whole response off the transport into one growable buffer. */
static char *slurp(RommplTransport *t, size_t *out_len) {
    size_t cap = 4096, len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) return NULL;
    for (;;) {
        if (len + 1024 > cap) { cap *= 2; char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; } buf = nb; }
        int n = t->read(t, buf + len, cap - len);
        if (n < 0) { free(buf); return NULL; }
        if (n == 0) break;
        len += (size_t)n;
    }
    *out_len = len;
    return buf;
}

static int parse_dechunk(const char *body, size_t body_len,
                         char **out, size_t *out_len) {
    char *dst = (char *)malloc(body_len + 1);
    if (!dst) return -1;
    size_t di = 0, i = 0;
    while (i < body_len) {
        /* Parse hex chunk size manually, bounded by body_len */
        size_t sz = 0;
        size_t hex_start = i;
        while (i < body_len && isxdigit((unsigned char)body[i])) {
            char c = body[i];
            int digit = (c >= '0' && c <= '9') ? (c - '0') :
                        (c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
                        (c >= 'A' && c <= 'F') ? (c - 'A' + 10) : -1;
            sz = sz * 16 + digit;
            i++;
        }
        /* Error if no hex digits found or end of buffer reached */
        if (i == hex_start || i >= body_len) { free(dst); return -1; }
        /* Skip CRLF after hex size */
        while (i < body_len && (body[i] == '\r' || body[i] == '\n')) i++;
        /* Stop on zero chunk or if we hit end of buffer without proper data */
        if (sz == 0) break;
        if (i + (size_t)sz > body_len) { free(dst); return -1; }
        memcpy(dst + di, body + i, (size_t)sz);
        di += (size_t)sz; i += (size_t)sz;
        /* Skip CRLF after chunk data */
        while (i < body_len && (body[i] == '\r' || body[i] == '\n')) i++;
    }
    dst[di] = '\0';
    *out = dst; *out_len = di;
    return 0;
}

int http_get(RommplTransport *t, const char *host, const char *path,
             const char *bearer_token, HttpResponse *resp) {
    char req[1024];
    int rn;
    if (bearer_token) {
        rn = snprintf(req, sizeof req,
            "GET %s HTTP/1.1\r\nHost: %s\r\nAuthorization: Bearer %s\r\n"
            "Accept: application/json\r\nConnection: close\r\n\r\n",
            path, host, bearer_token);
    } else {
        rn = snprintf(req, sizeof req,
            "GET %s HTTP/1.1\r\nHost: %s\r\n"
            "Accept: application/json\r\nConnection: close\r\n\r\n",
            path, host);
    }
    if (rn <= 0 || (size_t)rn >= sizeof req) return -1;
    /* Write request in a loop until all bytes are sent; handle partial writes */
    size_t written = 0;
    while (written < (size_t)rn) {
        int n = t->write(t, req + written, (size_t)rn - written);
        if (n <= 0) return -1;  /* Error or no progress on write */
        written += (size_t)n;
    }

    size_t raw_len = 0;
    char *raw = slurp(t, &raw_len);
    if (!raw) return -1;

    /* status line */
    if (raw_len < 12 || strncmp(raw, "HTTP/1.", 7) != 0) { free(raw); return -1; }
    resp->status = (int)strtol(raw + 9, NULL, 10);

    /* find header/body split */
    char *sep = NULL;
    for (size_t i = 0; i + 3 < raw_len; i++) {
        if (raw[i]=='\r'&&raw[i+1]=='\n'&&raw[i+2]=='\r'&&raw[i+3]=='\n') {
            sep = raw + i; break;
        }
    }
    if (!sep) { free(raw); return -1; }
    size_t header_len = (size_t)(sep - raw);
    const char *body = sep + 4;
    size_t body_len = raw_len - header_len - 4;

    /* chunked? (case-insensitive scan of the header block) */
    int chunked = 0;
    for (size_t i = 0; i + 6 < header_len; i++) {
        if (strncasecmp(raw + i, "chunked", 7) == 0) { chunked = 1; break; }
    }

    if (chunked) {
        char *de = NULL; size_t de_len = 0;
        if (parse_dechunk(body, body_len, &de, &de_len) != 0) { free(raw); return -1; }
        resp->body = de; resp->body_len = de_len;
        free(raw);
    } else {
        char *b = (char *)malloc(body_len + 1);
        if (!b) { free(raw); return -1; }
        memcpy(b, body, body_len); b[body_len] = '\0';
        resp->body = b; resp->body_len = body_len;
        free(raw);
    }
    return 0;
}

void http_response_free(HttpResponse *resp) {
    if (resp && resp->body) { free(resp->body); resp->body = NULL; resp->body_len = 0; }
}
