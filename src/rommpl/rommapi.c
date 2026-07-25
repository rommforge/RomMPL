#include "rommpl/rommapi.h"
#include "rommpl/http.h"
#include <stdio.h>
#include <string.h>

typedef struct { RomEntryCallback cb; void *user; int emitted; } Relay;

static int relay(const RomEntry *e, void *u) {
    Relay *r = (Relay *)u; r->emitted++; return r->cb(e, r->user);
}

int rommapi_list_platform_roms(RommApi *api, int platform_id, int page_size,
                               RomEntryCallback cb, void *user) {
    Relay r = { cb, user, 0 };
    long total = -1;
    int offset = 0;

    for (;;) {
        char path[256];
        int pn = snprintf(path, sizeof path,
            "/api/roms?platform_id=%d&limit=%d&offset=%d",
            platform_id, page_size, offset);
        if (pn <= 0 || (size_t)pn >= sizeof path) return -1;

        if (api->transport->connect(api->transport, api->host, api->port) < 0) return -1;
        HttpResponse resp;
        int gr = http_get(api->transport, api->host, path, api->token, &resp);
        api->transport->close(api->transport);
        if (gr != 0) return -1;
        if (resp.status != 200) { http_response_free(&resp); return -1; }

        RomPageInfo info;
        int pr = rom_parse_page(resp.body, resp.body_len, relay, &r, &info);
        http_response_free(&resp);
        if (pr != 0) return -1;

        if (info.total >= 0) total = info.total;
        offset += (info.count > 0 ? info.count : page_size);

        if (info.count == 0) break;                       /* empty page */
        if (info.count < page_size) break;                /* last partial page */
        if (total >= 0 && r.emitted >= total) break;      /* got them all */
    }
    return r.emitted;
}
