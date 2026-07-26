#include "rommpl/rommapi.h"
#include "support/mock_transport.h"
#include "support/fixtures.h"
#include "test.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

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
    ASSERT(strstr(mock_transport_written(t), "platform_ids=22") != NULL);
    ASSERT(strstr(mock_transport_written(t), "offset=2") != NULL);  /* second page requested */

    mock_transport_free(t);

    /* --- resolve_platform_id --- */
    static char resp[1024];
    snprintf(resp, sizeof resp,
        "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s",
        strlen(FIXTURE_PLATFORMS), FIXTURE_PLATFORMS);
    const char *responses_resolve[] = { resp };
    RommplTransport *t_resolve = mock_transport_new(responses_resolve, 1);
    RommApi api_resolve = { t_resolve, "h", 8080, "tok" };
    uint32_t id = 0;
    int rc = rommapi_resolve_platform_id(&api_resolve, "ps2", &id);
    ASSERT_INT_EQ(rc, 0);
    ASSERT_INT_EQ(id, 12);
    ASSERT(strstr(mock_transport_written(t_resolve), "GET /api/platforms") != NULL);
    mock_transport_free(t_resolve);

    const char *responses_resolve2[] = { resp };
    RommplTransport *t_resolve2 = mock_transport_new(responses_resolve2, 1);
    RommApi api_resolve2 = { t_resolve2, "h", 8080, "tok" };
    uint32_t id2 = 999;
    int rc2 = rommapi_resolve_platform_id(&api_resolve2, "dreamcast", &id2);
    ASSERT_INT_EQ(rc2, 1);          /* request ok, no match */
    mock_transport_free(t_resolve2);

    return TEST_SUMMARY();
}
