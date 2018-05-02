#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>
#include "packet.h"

extern int tunDeviceFD;

void TCPClientInitialize(struct sockaddr_in * endpoint);

void TCPClientAcceptClient();

void TCPClientCheckHealth(struct sockaddr_in * endpoint);

void TCPClientTCPData(struct sockaddr_in * endpoint);

void TCPClientTunnelData(struct sockaddr_in * endpoint);

#endif //CODE_CLIENT_HANDLERS_H
