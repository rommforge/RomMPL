#ifndef ROMMPL_PLATFORM_PARSER_H
#define ROMMPL_PLATFORM_PARSER_H

#include <stddef.h>
#include <stdint.h>

#define ROMMPL_SLUG_MAX 48

typedef struct {
    uint32_t id;
    char     slug[ROMMPL_SLUG_MAX];
} PlatformEntry;

typedef int (*PlatformEntryCallback)(const PlatformEntry *entry, void *user);

/* Parse a top-level JSON array of platform objects (GET /api/platforms),
 * emitting {id, slug} per element. Returns count emitted, or negative on
 * malformed JSON. */
int platform_parse_list(const char *json, size_t len,
                        PlatformEntryCallback cb, void *user);

#endif
