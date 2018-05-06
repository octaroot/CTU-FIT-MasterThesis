#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>
#include "packet.h"

void UDPClientInitialize(struct sockaddr_in * endpoint, struct UDPPluginState * pluginState);

void UDPClientCheckHealth(struct UDPPluginState * pluginState);

void UDPClientUDPData(struct UDPPluginState * pluginState);

void UDPClientTunnelData(struct UDPPluginState * pluginState);

#endif //CODE_CLIENT_HANDLERS_H
