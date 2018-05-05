#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern struct UDPPluginState pluginStateUDP;

void UDPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage *request);

void UDPHandleUDPData(struct UDPMessage *msg);

void UDPHandleKeepAlive(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage * request);

void UDPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
