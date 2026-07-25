#include "rommpl/transport.h"
#include "support/mock_transport.h"
#include "test.h"
#include <string.h>

int main(void) {
    const char *responses[] = { "HELLO", "WORLD" };
    RommplTransport *t = mock_transport_new(responses, 2);

    char buf[16];
    ASSERT_INT_EQ(t->connect(t, "host", 80), 0);
    ASSERT_INT_EQ(t->write(t, "GET / HTTP/1.1\r\n", 16), 16);
    int n = t->read(t, buf, sizeof buf);
    buf[n] = '\0';
    ASSERT_STR_EQ(buf, "HELLO");                     /* first connect serves first response */
    ASSERT_INT_EQ(t->read(t, buf, sizeof buf), 0);   /* then EOF */
    t->close(t);
    ASSERT(strncmp(mock_transport_written(t), "GET / HTTP/1.1", 14) == 0);

    ASSERT_INT_EQ(t->connect(t, "host", 80), 0);     /* second connect serves second response */
    n = t->read(t, buf, sizeof buf); buf[n] = '\0';
    ASSERT_STR_EQ(buf, "WORLD");
    t->close(t);

    mock_transport_free(t);
    return TEST_SUMMARY();
}
