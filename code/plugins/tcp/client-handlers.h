#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>
#include "packet.h"

void TCPClientInitialize(struct sockaddr_in * endpoint, struct TCPPluginState * pluginStateTCP);

void TCPClientAcceptClient(struct TCPPluginState * pluginStateTCP);

void TCPClientCheckHealth(struct TCPPluginState * pluginStateTCP);

void TCPClientTCPData(struct TCPPluginState * pluginStateTCP);

void TCPClientTunnelData(struct TCPPluginState * pluginStateTCP);

#endif //CODE_CLIENT_HANDLERS_H
