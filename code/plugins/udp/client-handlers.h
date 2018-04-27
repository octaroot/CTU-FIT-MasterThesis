#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>
#include "packet.h"

extern int tunDeviceFD;

void UDPClientInitialize(struct sockaddr_in * endpoint);

void UDPClientCheckHealth(struct sockaddr_in * endpoint);

void UDPClientUDPData(struct sockaddr_in * endpoint);

void UDPClientTunnelData(struct sockaddr_in * endpoint);

#endif //CODE_CLIENT_HANDLERS_H
