#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>
#include "packet.h"

extern int tunDeviceFD;

void SCTPClientInitialize(struct sockaddr_in * endpoint);

void SCTPClientAcceptClient();

void SCTPClientCheckHealth(struct sockaddr_in * endpoint);

void SCTPClientSCTPData(struct sockaddr_in * endpoint);

void SCTPClientTunnelData(struct sockaddr_in * endpoint);

#endif //CODE_CLIENT_HANDLERS_H
