/* Pull in jsmn.h's actual implementation here, before jsmn_util.h requests
 * declarations only (JSMN_HEADER). This keeps jsmn_init/jsmn_parse defined
 * exactly once across the program, in this translation unit. */
#include "jsmn.h"
#include "rommpl/jsmn_util.h"
#include <stdlib.h>
#include <string.h>

int jsmn_tok_is(const char *js, const jsmntok_t *t, const char *key) {
    int len = t->end - t->start;
    return t->type == JSMN_STRING &&
           (int)strlen(key) == len &&
           strncmp(js + t->start, key, (size_t)len) == 0;
}

int jsmn_skip(const jsmntok_t *t, int i) {
    if (t[i].type == JSMN_OBJECT) {
        int idx = i + 1;
        for (int k = 0; k < t[i].size; k++) { idx = jsmn_skip(t, idx); idx = jsmn_skip(t, idx); }
        return idx;
    }
    if (t[i].type == JSMN_ARRAY) {
        int idx = i + 1;
        for (int k = 0; k < t[i].size; k++) idx = jsmn_skip(t, idx);
        return idx;
    }
    return i + 1;
}

int jsmn_field(const char *js, const jsmntok_t *t, int obj, const char *key) {
    int idx = obj + 1;
    for (int k = 0; k < t[obj].size; k++) {
        int kt = idx, vt = idx + 1;
        if (jsmn_tok_is(js, &t[kt], key)) return vt;
        idx = jsmn_skip(t, vt);
    }
    return -1;
}

void jsmn_copy_str(const char *js, const jsmntok_t *v, char *dst, size_t cap) {
    if (v->type != JSMN_STRING) { dst[0] = '\0'; return; }
    size_t n = (size_t)(v->end - v->start);
    if (n >= cap) n = cap - 1;
    memcpy(dst, js + v->start, n);
    dst[n] = '\0';
}

long jsmn_prim_long(const char *js, const jsmntok_t *v, long dflt) {
    if (v == NULL || v->type != JSMN_PRIMITIVE) return dflt;
    char buf[32]; size_t n = (size_t)(v->end - v->start);
    if (n >= sizeof buf) n = sizeof buf - 1;
    memcpy(buf, js + v->start, n); buf[n] = '\0';
    return strtol(buf, NULL, 10);
}

unsigned long long jsmn_prim_u64(const char *js, const jsmntok_t *v) {
    if (v == NULL || v->type != JSMN_PRIMITIVE) return 0;
    char buf[32]; size_t n = (size_t)(v->end - v->start);
    if (n >= sizeof buf) n = sizeof buf - 1;
    memcpy(buf, js + v->start, n); buf[n] = '\0';
    return strtoull(buf, NULL, 10);
}

int jsmn_prim_is_true(const char *js, const jsmntok_t *v) {
    return v && v->type == JSMN_PRIMITIVE && js[v->start] == 't';
}
