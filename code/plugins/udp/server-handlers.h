#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "udp.h"

void UDPServerInitialize(struct sockaddr_in * endpoint);

void UDPServerCheckHealth(struct sockaddr_in * endpoint);

void UDPServerUDPData(struct sockaddr_in * endpoint);

void UDPServerTunnelData(struct sockaddr_in * endpoint);

#endif //CODE_SERVER_HANDLERS_H
