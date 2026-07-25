#include "rommpl/platform_parser.h"
#include "support/fixtures.h"
#include "test.h"
#include <string.h>

typedef struct { PlatformEntry rows[8]; int n; } Bag;
static int collect(const PlatformEntry *p, void *u) {
    Bag *b = (Bag *)u; b->rows[b->n++] = *p; return 0;
}

int main(void) {
    Bag b = {0};
    const char *js = FIXTURE_PLATFORMS;
    int n = platform_parse_list(js, strlen(js), collect, &b);
    ASSERT_INT_EQ(n, 3);
    ASSERT_INT_EQ(b.n, 3);
    ASSERT_INT_EQ(b.rows[0].id, 12);
    ASSERT_STR_EQ(b.rows[0].slug, "ps2");
    ASSERT_INT_EQ(b.rows[1].id, 5);
    ASSERT_STR_EQ(b.rows[1].slug, "snes");
    ASSERT_STR_EQ(b.rows[2].slug, "gc");
    return TEST_SUMMARY();
}
