#ifndef ROMMPL_ROM_ENTRY_H
#define ROMMPL_ROM_ENTRY_H

#include <stdint.h>

#define ROMMPL_NAME_MAX   128
#define ROMMPL_FSNAME_MAX 128
#define ROMMPL_FSPATH_MAX 160
#define ROMMPL_COVER_MAX  160

enum {
    ROMMPL_FLAG_MULTI_FILE      = 1 << 0,
    ROMMPL_FLAG_MISSING_FROM_FS = 1 << 1
};

/* Compact, fixed-size view of a RomM rom. Everything the console needs; nothing
 * it does not. Roughly 600 bytes, so a 500-game library is about 300 KB. */
typedef struct {
    uint32_t id;                          /* RomM rom id */
    char     name[ROMMPL_NAME_MAX];       /* display name (fallback: fs_name_no_ext) */
    char     fs_name[ROMMPL_FSNAME_MAX];  /* on-disk filename, the Neutrino boot target */
    char     fs_path[ROMMPL_FSPATH_MAX];  /* relative path within the served volume */
    uint64_t fs_size_bytes;               /* for display */
    char     cover_small[ROMMPL_COVER_MAX]; /* path_cover_small, fetched lazily */
    uint8_t  flags;                       /* ROMMPL_FLAG_* */
} RomEntry;

#endif
