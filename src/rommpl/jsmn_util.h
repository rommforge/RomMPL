#ifndef ROMMPL_JSMN_UTIL_H
#define ROMMPL_JSMN_UTIL_H

/* jsmn.h is a header-only library: including it normally emits a full
 * implementation of jsmn_init/jsmn_parse into every translation unit that
 * does so. jsmn_util.h is pulled into more than one .c file, so it asks for
 * declarations only (JSMN_HEADER); jsmn_util.c provides the single real
 * implementation by including plain jsmn.h before this header. */
#define JSMN_HEADER
#include "jsmn.h"
#include <stddef.h>

int jsmn_tok_is(const char *js, const jsmntok_t *t, const char *key);
int jsmn_skip(const jsmntok_t *t, int i);
int jsmn_field(const char *js, const jsmntok_t *t, int obj, const char *key);
void jsmn_copy_str(const char *js, const jsmntok_t *v, char *dst, size_t cap);
long jsmn_prim_long(const char *js, const jsmntok_t *v, long dflt);
unsigned long long jsmn_prim_u64(const char *js, const jsmntok_t *v);
int jsmn_prim_is_true(const char *js, const jsmntok_t *v);

#endif
