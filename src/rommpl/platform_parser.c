#include "rommpl/platform_parser.h"
#include "rommpl/jsmn_util.h"
#include "jsmn.h"
#include <stdlib.h>
#include <string.h>

int platform_parse_list(const char *json, size_t len,
                        PlatformEntryCallback cb, void *user) {
    jsmn_parser p; jsmn_init(&p);
    int need = jsmn_parse(&p, json, len, NULL, 0);
    if (need < 1) return -1;
    jsmntok_t *tok = (jsmntok_t *)malloc(sizeof(jsmntok_t) * (size_t)need);
    if (!tok) return -1;
    jsmn_init(&p);
    int nt = jsmn_parse(&p, json, len, tok, (unsigned int)need);
    if (nt < 1 || tok[0].type != JSMN_ARRAY) { free(tok); return -1; }

    int idx = 1;
    int count = 0, rc = 0;
    for (int i = 0; i < tok[0].size; i++) {
        int obj = idx;
        if (tok[obj].type == JSMN_OBJECT) {
            PlatformEntry e; memset(&e, 0, sizeof e);
            int f;
            f = jsmn_field(json, tok, obj, "id");
            e.id = (uint32_t)jsmn_prim_long(json, f >= 0 ? tok + f : NULL, 0);
            f = jsmn_field(json, tok, obj, "slug");
            if (f >= 0) jsmn_copy_str(json, tok + f, e.slug, ROMMPL_SLUG_MAX);
            count++;
            rc = cb(&e, user);
            if (rc < 0) break;
        }
        idx = jsmn_skip(tok, idx);
    }
    free(tok);
    return rc < 0 ? rc : count;
}
