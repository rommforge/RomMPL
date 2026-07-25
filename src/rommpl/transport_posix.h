#ifndef ROMMPL_TRANSPORT_POSIX_H
#define ROMMPL_TRANSPORT_POSIX_H

#include "rommpl/transport.h"

RommplTransport *posix_transport_new(void);
void posix_transport_free(RommplTransport *t);

#endif
