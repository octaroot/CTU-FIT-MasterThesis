#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "tcp.h"

void TCPServerInitialize(struct sockaddr_in * endpoint);

void TCPServerCheckHealth(struct sockaddr_in * endpoint);

void TCPServerTCPData(struct sockaddr_in * endpoint);

void TCPServerTunnelData(struct sockaddr_in * endpoint);

#endif //CODE_SERVER_HANDLERS_H
