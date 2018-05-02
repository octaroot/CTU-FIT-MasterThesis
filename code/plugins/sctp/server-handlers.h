#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "sctp.h"

void SCTPServerInitialize(struct sockaddr_in * endpoint);

void SCTPServerAcceptClient();

void SCTPServerCheckHealth(struct sockaddr_in * endpoint);

void SCTPServerSCTPData(struct sockaddr_in * endpoint);

void SCTPServerTunnelData(struct sockaddr_in * endpoint);

#endif //CODE_SERVER_HANDLERS_H
