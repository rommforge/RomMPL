#include "rommpl/rom_entry.h"
#include "test.h"
#include <string.h>

int main(void) {
    RomEntry e;
    memset(&e, 0, sizeof e);
    e.id = 42;
    e.fs_size_bytes = 4700000000ULL;      /* > 4 GB, must not overflow */
    strcpy(e.fs_name, "SLUS_200.00.iso");
    e.flags = ROMMPL_FLAG_MULTI_FILE;

    ASSERT(e.id == 42u);
    ASSERT(e.fs_size_bytes == 4700000000ULL);
    ASSERT_STR_EQ(e.fs_name, "SLUS_200.00.iso");
    ASSERT(e.flags & ROMMPL_FLAG_MULTI_FILE);
    ASSERT(sizeof(RomEntry) < 800);       /* stays compact */
    return TEST_SUMMARY();
}
