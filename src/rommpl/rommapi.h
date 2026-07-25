#ifndef ROMMPL_ROMMAPI_H
#define ROMMPL_ROMMAPI_H

#include "rommpl/transport.h"
#include "rommpl/rom_parser.h"

typedef struct {
    RommplTransport *transport;  /* reconnected per page */
    const char      *host;       /* e.g. "192.168.1.10" */
    int              port;       /* e.g. 8080 */
    const char      *token;      /* client token, may be NULL */
} RommApi;

/* Emit every rom for `platform_id`, paging by `page_size`. Returns the count
 * emitted, or negative on error. */
int rommapi_list_platform_roms(RommApi *api, int platform_id, int page_size,
                               RomEntryCallback cb, void *user);

#endif
