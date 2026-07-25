#ifndef ROMMPL_FIXTURES_H
#define ROMMPL_FIXTURES_H

/* One fat rom object, trimmed to representative shape. The parser must pull the
 * compact fields and ignore igdb_metadata, moby_metadata, siblings, etc. */
#define FIXTURE_FAT_ROM \
    "{\"id\":42,\"name\":\"Rez\",\"fs_name\":\"Rez.iso\"," \
    "\"fs_name_no_ext\":\"Rez\",\"fs_path\":\"ps2/Rez.iso\"," \
    "\"fs_size_bytes\":734003200,\"platform_slug\":\"ps2\"," \
    "\"path_cover_small\":\"assets/covers/42/small.png\"," \
    "\"has_multiple_files\":false,\"missing_from_fs\":false," \
    "\"igdb_metadata\":{\"summary\":\"ignore me\",\"genres\":[\"Shooter\"]}," \
    "\"moby_metadata\":{\"x\":1},\"siblings\":[],\"alternative_names\":[\"Rezz\"]}"

/* A LimitOffsetPage envelope with two items, one of them name:null. */
#define FIXTURE_PAGE_2 \
    "{\"items\":[" FIXTURE_FAT_ROM "," \
    "{\"id\":7,\"name\":null,\"fs_name\":\"Ico.iso\",\"fs_name_no_ext\":\"Ico\"," \
    "\"fs_path\":\"ps2/Ico.iso\",\"fs_size_bytes\":1200000000," \
    "\"path_cover_small\":\"assets/covers/7/small.png\"," \
    "\"has_multiple_files\":true,\"missing_from_fs\":false}" \
    "],\"total\":3,\"limit\":2,\"offset\":0}"

/* Second page, one remaining item, total still 3. */
#define FIXTURE_PAGE_LAST \
    "{\"items\":[" \
    "{\"id\":9,\"name\":\"Katamari\",\"fs_name\":\"Katamari.zso\"," \
    "\"fs_name_no_ext\":\"Katamari\",\"fs_path\":\"ps2/Katamari.zso\"," \
    "\"fs_size_bytes\":900000000,\"path_cover_small\":\"assets/covers/9/small.png\"," \
    "\"has_multiple_files\":false,\"missing_from_fs\":false}" \
    "],\"total\":3,\"limit\":2,\"offset\":2}"

/* A GET /api/platforms response: a top-level array of platform objects, each
 * carrying more than we need. The parser must pull only id + slug. */
#define FIXTURE_PLATFORMS \
    "[" \
    "{\"id\":12,\"slug\":\"ps2\",\"name\":\"PlayStation 2\",\"rom_count\":4213}," \
    "{\"id\":5,\"slug\":\"snes\",\"name\":\"Super Nintendo\",\"rom_count\":900}," \
    "{\"id\":9,\"slug\":\"gc\",\"name\":\"GameCube\",\"rom_count\":50}" \
    "]"

#endif
