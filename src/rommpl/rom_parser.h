#ifndef ROMMPL_ROM_PARSER_H
#define ROMMPL_ROM_PARSER_H

#include "rommpl/rom_entry.h"
#include <stddef.h>

/* Invoked once per rom. Return 0 to continue, negative to abort parsing. */
typedef int (*RomEntryCallback)(const RomEntry *entry, void *user);

typedef struct {
    long total;   /* "total" from the page envelope, -1 if absent */
    long limit;   /* echoed "limit", -1 if absent */
    long offset;  /* echoed "offset", -1 if absent */
    int  count;   /* items emitted from this page */
} RomPageInfo;

/* Parse one LimitOffsetPage JSON buffer, emitting one RomEntry per item.
 * Returns 0 on success, negative on malformed JSON. info may be NULL. */
int rom_parse_page(const char *json, size_t len,
                   RomEntryCallback cb, void *user, RomPageInfo *info);

#endif
