#include "rommpl/rom_parser.h"
#include "rommpl/jsmn_util.h"
#include "jsmn.h"
#include <stdlib.h>
#include <string.h>

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
        int ft = jsmn_field(json, tok, 0, "total");
        int fl = jsmn_field(json, tok, 0, "limit");
        int fo = jsmn_field(json, tok, 0, "offset");
        info->total  = jsmn_prim_long(json, tok + (ft >= 0 ? ft : 0), -1);
        info->limit  = jsmn_prim_long(json, tok + (fl >= 0 ? fl : 0), -1);
        info->offset = jsmn_prim_long(json, tok + (fo >= 0 ? fo : 0), -1);
    }

    int items = jsmn_field(json, tok, 0, "items");
    if (items < 0 || tok[items].type != JSMN_ARRAY) { free(tok); return -1; }

    int idx = items + 1;
    int rc = 0;
    for (int i = 0; i < tok[items].size; i++) {
        int obj = idx;
        if (tok[obj].type == JSMN_OBJECT) {
            RomEntry e; memset(&e, 0, sizeof e);
            int f;
            f = jsmn_field(json, tok, obj, "id");            e.id = (uint32_t)jsmn_prim_long(json, f>=0?tok+f:NULL, 0);
            f = jsmn_field(json, tok, obj, "fs_name");       if (f>=0) jsmn_copy_str(json, tok+f, e.fs_name, ROMMPL_FSNAME_MAX);
            f = jsmn_field(json, tok, obj, "fs_path");       if (f>=0) jsmn_copy_str(json, tok+f, e.fs_path, ROMMPL_FSPATH_MAX);
            f = jsmn_field(json, tok, obj, "path_cover_small"); if (f>=0) jsmn_copy_str(json, tok+f, e.cover_small, ROMMPL_COVER_MAX);
            f = jsmn_field(json, tok, obj, "fs_size_bytes"); e.fs_size_bytes = jsmn_prim_u64(json, f>=0?tok+f:NULL);
            f = jsmn_field(json, tok, obj, "name");
            if (f >= 0 && tok[f].type == JSMN_STRING) {
                jsmn_copy_str(json, tok+f, e.name, ROMMPL_NAME_MAX);
            } else {
                int g = jsmn_field(json, tok, obj, "fs_name_no_ext");
                if (g >= 0) jsmn_copy_str(json, tok+g, e.name, ROMMPL_NAME_MAX);
            }
            f = jsmn_field(json, tok, obj, "has_multiple_files"); if (jsmn_prim_is_true(json, f>=0?tok+f:NULL)) e.flags |= ROMMPL_FLAG_MULTI_FILE;
            f = jsmn_field(json, tok, obj, "missing_from_fs");    if (jsmn_prim_is_true(json, f>=0?tok+f:NULL)) e.flags |= ROMMPL_FLAG_MISSING_FROM_FS;

            if (info) info->count++;
            rc = cb(&e, user);
            if (rc < 0) break;
        }
        idx = jsmn_skip(tok, idx);
    }
    free(tok);
    return rc < 0 ? rc : 0;
}
