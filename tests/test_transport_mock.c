#include "rommpl/transport.h"
#include "support/mock_transport.h"
#include "test.h"
#include <string.h>

int main(void) {
    const char *responses[] = { "HELLO", "WORLD", "OK" };
    RommplTransport *t = mock_transport_new(responses, 3);

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

    /* (a) write larger than capture buffer returns actual captured byte count */
    ASSERT_INT_EQ(t->connect(t, "host", 80), 0);
    char big_write[5000];
    memset(big_write, 'X', sizeof big_write);
    int written = t->write(t, big_write, sizeof big_write);
    ASSERT_INT_EQ(written, 4095);  /* not 5000: truncated to buffer size */
    t->close(t);

    /* (b) read on out-of-range/closed connection returns negative */
    ASSERT_INT_EQ(t->connect(t, "host", 80), -1);  /* no third response */
    int read_result = t->read(t, buf, sizeof buf);
    ASSERT(read_result < 0);  /* should be -1 on bad connection */

    mock_transport_free(t);
    return TEST_SUMMARY();
}
