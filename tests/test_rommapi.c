#include "rommpl/rommapi.h"
#include "support/mock_transport.h"
#include "support/fixtures.h"
#include "test.h"
#include <string.h>
#include <stdio.h>

typedef struct { int count; uint32_t ids[8]; } Sink;
static int sink(const RomEntry *e, void *u) {
    Sink *s = (Sink *)u; s->ids[s->count++] = e->id; return 0;
}

int main(void) {
    static char page1[4096], page2[4096];
    snprintf(page1, sizeof page1,
        "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s",
        strlen(FIXTURE_PAGE_2), FIXTURE_PAGE_2);
    snprintf(page2, sizeof page2,
        "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s",
        strlen(FIXTURE_PAGE_LAST), FIXTURE_PAGE_LAST);
    const char *responses[] = { page1, page2 };

    RommplTransport *t = mock_transport_new(responses, 2);
    RommApi api = { t, "192.168.1.10", 8080, "tok" };
    Sink s = {0};
    int n = rommapi_list_platform_roms(&api, 22, 2, sink, &s);

    ASSERT_INT_EQ(n, 3);                 /* total from envelope */
    ASSERT_INT_EQ(s.count, 3);
    ASSERT_INT_EQ(s.ids[0], 42);
    ASSERT_INT_EQ(s.ids[1], 7);
    ASSERT_INT_EQ(s.ids[2], 9);
    ASSERT(strstr(mock_transport_written(t), "platform_id=22") != NULL);
    ASSERT(strstr(mock_transport_written(t), "offset=2") != NULL);  /* second page requested */

    mock_transport_free(t);
    return TEST_SUMMARY();
}
