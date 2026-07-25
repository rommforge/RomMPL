#include "rommpl/http.h"
#include "support/mock_transport.h"
#include "test.h"
#include <string.h>

int main(void) {
    /* Content-Length response */
    const char *cl =
        "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
        "Content-Length: 13\r\n\r\n{\"ok\":true}\r\n";
    const char *r1[] = { cl };
    RommplTransport *t = mock_transport_new(r1, 1);
    t->connect(t, "h", 80);
    HttpResponse resp;
    ASSERT_INT_EQ(http_get(t, "h", "/api/roms", "tok123", &resp), 0);
    ASSERT_INT_EQ(resp.status, 200);
    ASSERT_INT_EQ(resp.body_len, 13);
    ASSERT(strncmp(resp.body, "{\"ok\":true}\r\n", 13) == 0);
    ASSERT(strstr(mock_transport_written(t), "GET /api/roms HTTP/1.1") != NULL);
    ASSERT(strstr(mock_transport_written(t), "Authorization: Bearer tok123") != NULL);
    ASSERT(strstr(mock_transport_written(t), "Host: h") != NULL);
    http_response_free(&resp);
    t->close(t);
    mock_transport_free(t);

    /* chunked response: "abc" + "de" */
    const char *ch =
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "3\r\nabc\r\n2\r\nde\r\n0\r\n\r\n";
    const char *r2[] = { ch };
    RommplTransport *t2 = mock_transport_new(r2, 1);
    t2->connect(t2, "h", 80);
    HttpResponse resp2;
    ASSERT_INT_EQ(http_get(t2, "h", "/x", NULL, &resp2), 0);
    ASSERT_INT_EQ(resp2.body_len, 5);
    ASSERT(strncmp(resp2.body, "abcde", 5) == 0);
    http_response_free(&resp2);
    t2->close(t2);
    mock_transport_free(t2);

    /* malformed chunked: ends on hex digits, no CRLF terminator */
    const char *malformed =
        "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
        "3\r\nabc\r\n2";  /* ends on '2' with no CRLF after */
    const char *r3[] = { malformed };
    RommplTransport *t3 = mock_transport_new(r3, 1);
    t3->connect(t3, "h", 80);
    HttpResponse resp3;
    /* http_get should fail due to malformed chunked body */
    ASSERT_INT_EQ(http_get(t3, "h", "/x", NULL, &resp3), -1);
    t3->close(t3);
    mock_transport_free(t3);

    /* short write: mock that limits write returns to force multiple calls */
    const char *normal =
        "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
    const char *r4[] = { normal };
    /* Create mock with write_limit of 15 bytes per call to force multiple writes */
    RommplTransport *t4 = mock_transport_new_with_write_limit(r4, 1, 15);
    t4->connect(t4, "h", 80);
    HttpResponse resp4;
    /* http_get should handle multiple writes and succeed */
    ASSERT_INT_EQ(http_get(t4, "h", "/test", NULL, &resp4), 0);
    ASSERT_INT_EQ(resp4.status, 200);
    ASSERT_INT_EQ(resp4.body_len, 2);
    ASSERT(strncmp(resp4.body, "OK", 2) == 0);
    /* Verify the full request was written (write loop worked): request ends with CRLF CRLF */
    const char *written = mock_transport_written(t4);
    ASSERT(strstr(written, "Connection: close\r\n\r\n") != NULL);
    http_response_free(&resp4);
    t4->close(t4);
    mock_transport_free(t4);

    return TEST_SUMMARY();
}
