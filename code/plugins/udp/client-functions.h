#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern struct UDPPluginState pluginState;

void UDPSendConnectionRequest(int socketFD, struct sockaddr_in * endpoint);

void UDPSendKeepAlive(int socketFD, struct sockaddr_in * endpoint);

void UDPHandleConnectionAccept(struct sockaddr_in * endpoint);

void UDPHandleAuthChallenge(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage *origMsg);

void UDPHandleConnectionReject(int socketFD, struct sockaddr_in * endpoint);

void UDPHandleUDPData(struct UDPMessage *msg);

void UDPHandleKeepAliveResponse();

#endif //CODE_CLIENT_FUNCTIONS_H
