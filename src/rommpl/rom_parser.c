#include "rommpl/rom_parser.h"
#include "jsmn.h"
#include <stdlib.h>
#include <string.h>

static int tok_is(const char *js, const jsmntok_t *t, const char *key) {
    int len = t->end - t->start;
    return t->type == JSMN_STRING &&
           (int)strlen(key) == len &&
           strncmp(js + t->start, key, (size_t)len) == 0;
}

/* Index just past the subtree rooted at token i. */
static int skip(const jsmntok_t *t, int i) {
    if (t[i].type == JSMN_OBJECT) {
        int idx = i + 1;
        for (int k = 0; k < t[i].size; k++) { idx = skip(t, idx); idx = skip(t, idx); }
        return idx;
    }
    if (t[i].type == JSMN_ARRAY) {
        int idx = i + 1;
        for (int k = 0; k < t[i].size; k++) idx = skip(t, idx);
        return idx;
    }
    return i + 1;
}

/* Token index of the value for `key` within object token `obj`, or -1. */
static int field(const char *js, const jsmntok_t *t, int obj, const char *key) {
    int idx = obj + 1;
    for (int k = 0; k < t[obj].size; k++) {
        int kt = idx, vt = idx + 1;
        if (tok_is(js, &t[kt], key)) return vt;
        idx = skip(t, vt);
    }
    return -1;
}

static void copy_str(const char *js, const jsmntok_t *v, char *dst, size_t cap) {
    if (v->type != JSMN_STRING) { dst[0] = '\0'; return; }
    size_t n = (size_t)(v->end - v->start);
    if (n >= cap) n = cap - 1;
    memcpy(dst, js + v->start, n);
    dst[n] = '\0';
}

static long prim_long(const char *js, const jsmntok_t *v, long dflt) {
    if (v == NULL || v->type != JSMN_PRIMITIVE) return dflt;
    char buf[32]; size_t n = (size_t)(v->end - v->start);
    if (n >= sizeof buf) n = sizeof buf - 1;
    memcpy(buf, js + v->start, n); buf[n] = '\0';
    return strtol(buf, NULL, 10);
}

static unsigned long long prim_u64(const char *js, const jsmntok_t *v) {
    if (v == NULL || v->type != JSMN_PRIMITIVE) return 0;
    char buf[32]; size_t n = (size_t)(v->end - v->start);
    if (n >= sizeof buf) n = sizeof buf - 1;
    memcpy(buf, js + v->start, n); buf[n] = '\0';
    return strtoull(buf, NULL, 10);
}

static int is_true(const char *js, const jsmntok_t *v) {
    return v && v->type == JSMN_PRIMITIVE && js[v->start] == 't';
}
static int is_null(const char *js, const jsmntok_t *v) {
    return v && v->type == JSMN_PRIMITIVE && js[v->start] == 'n';
}

int rom_parse_page(const char *json, size_t len,
                   RomEntryCallback cb, void *user, RomPageInfo *info) {
    jsmn_parser p; jsmn_init(&p);
    int need = jsmn_parse(&p, json, len, NULL, 0);
    if (need < 1) return -1;
    jsmntok_t *tok = (jsmntok_t *)malloc(sizeof(jsmntok_t) * (size_t)need);
    if (!tok) return -1;
    jsmn_init(&p);
    int nt = jsmn_parse(&p, json, len, tok, (unsigned int)need);
    if (nt < 1 || tok[0].type != JSMN_OBJECT) { free(tok); return -1; }

    if (info) { info->total = info->limit = info->offset = -1; info->count = 0; }
    if (info) {
        int ft = field(json, tok, 0, "total");
        int fl = field(json, tok, 0, "limit");
        int fo = field(json, tok, 0, "offset");
        info->total  = prim_long(json, tok + (ft >= 0 ? ft : 0), -1);
        info->limit  = prim_long(json, tok + (fl >= 0 ? fl : 0), -1);
        info->offset = prim_long(json, tok + (fo >= 0 ? fo : 0), -1);
    }

    int items = field(json, tok, 0, "items");
    if (items < 0 || tok[items].type != JSMN_ARRAY) { free(tok); return -1; }

    int idx = items + 1;
    int rc = 0;
    for (int i = 0; i < tok[items].size; i++) {
        int obj = idx;
        if (tok[obj].type == JSMN_OBJECT) {
            RomEntry e; memset(&e, 0, sizeof e);
            int f;
            f = field(json, tok, obj, "id");            e.id = (uint32_t)prim_long(json, f>=0?tok+f:NULL, 0);
            f = field(json, tok, obj, "fs_name");       if (f>=0) copy_str(json, tok+f, e.fs_name, ROMMPL_FSNAME_MAX);
            f = field(json, tok, obj, "fs_path");       if (f>=0) copy_str(json, tok+f, e.fs_path, ROMMPL_FSPATH_MAX);
            f = field(json, tok, obj, "path_cover_small"); if (f>=0) copy_str(json, tok+f, e.cover_small, ROMMPL_COVER_MAX);
            f = field(json, tok, obj, "fs_size_bytes"); e.fs_size_bytes = prim_u64(json, f>=0?tok+f:NULL);
            f = field(json, tok, obj, "name");
            if (f >= 0 && tok[f].type == JSMN_STRING) {
                copy_str(json, tok+f, e.name, ROMMPL_NAME_MAX);
            } else {
                int g = field(json, tok, obj, "fs_name_no_ext");
                if (g >= 0) copy_str(json, tok+g, e.name, ROMMPL_NAME_MAX);
            }
            f = field(json, tok, obj, "has_multiple_files"); if (is_true(json, f>=0?tok+f:NULL)) e.flags |= ROMMPL_FLAG_MULTI_FILE;
            f = field(json, tok, obj, "missing_from_fs");    if (is_true(json, f>=0?tok+f:NULL)) e.flags |= ROMMPL_FLAG_MISSING_FROM_FS;

            (void)is_null;
            if (info) info->count++;
            rc = cb(&e, user);
            if (rc < 0) break;
        }
        idx = skip(tok, idx);
    }
    free(tok);
    return rc < 0 ? rc : 0;
}
