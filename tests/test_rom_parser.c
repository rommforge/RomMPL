#include "rommpl/rom_parser.h"
#include "support/fixtures.h"
#include "test.h"
#include <string.h>

typedef struct { RomEntry rows[8]; int n; } Collector;

static int collect(const RomEntry *e, void *user) {
    Collector *c = (Collector *)user;
    c->rows[c->n++] = *e;
    return 0;
}

int main(void) {
    Collector c = {0};
    RomPageInfo info;
    const char *page = FIXTURE_PAGE_2;
    ASSERT_INT_EQ(rom_parse_page(page, strlen(page), collect, &c, &info), 0);

    ASSERT_INT_EQ(c.n, 2);
    ASSERT_INT_EQ(info.count, 2);
    ASSERT_INT_EQ(info.total, 3);
    ASSERT_INT_EQ(info.limit, 2);
    ASSERT_INT_EQ(info.offset, 0);

    /* first item: fat rom, metadata ignored */
    ASSERT_INT_EQ(c.rows[0].id, 42);
    ASSERT_STR_EQ(c.rows[0].name, "Rez");
    ASSERT_STR_EQ(c.rows[0].fs_name, "Rez.iso");
    ASSERT_STR_EQ(c.rows[0].fs_path, "ps2/Rez.iso");
    ASSERT(c.rows[0].fs_size_bytes == 734003200ULL);
    ASSERT_STR_EQ(c.rows[0].cover_small, "assets/covers/42/small.png");
    ASSERT_INT_EQ(c.rows[0].flags, 0);

    /* second item: name null -> fs_name_no_ext, multi-file flag set */
    ASSERT_INT_EQ(c.rows[1].id, 7);
    ASSERT_STR_EQ(c.rows[1].name, "Ico");
    ASSERT(c.rows[1].flags & ROMMPL_FLAG_MULTI_FILE);
    return TEST_SUMMARY();
}
