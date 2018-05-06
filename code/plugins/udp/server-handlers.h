#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "udp.h"

void UDPServerInitialize(struct sockaddr_in * endpoint, struct UDPPluginState * pluginState);

void UDPServerCheckHealth(struct UDPPluginState * pluginState);

void UDPServerUDPData(struct UDPPluginState * pluginState);

void UDPServerTunnelData(struct UDPPluginState * pluginState);

#endif //CODE_SERVER_HANDLERS_H
