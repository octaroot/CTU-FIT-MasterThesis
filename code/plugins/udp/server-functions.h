#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"
#include "udp.h"

void UDPHandleConnectionRequest(struct UDPPluginState * pluginState, struct sockaddr_in * endpoint, struct UDPMessage *request);

void UDPHandleUDPData(struct UDPMessage *msg);

void UDPHandleKeepAlive(struct UDPPluginState * pluginState, struct sockaddr_in * endpoint, struct UDPMessage * request);

void UDPHandleAuthResponse(struct UDPPluginState * pluginState, struct sockaddr_in * endpoint, struct UDPMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
