#ifndef ROMMPL_TRANSPORT_LWIP_H
#define ROMMPL_TRANSPORT_LWIP_H

#include "rommpl/transport.h"

RommplTransport *lwip_transport_new(void);
void lwip_transport_free(RommplTransport *t);

#endif
