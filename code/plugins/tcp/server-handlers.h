#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "tcp.h"

void TCPServerInitialize(struct sockaddr_in * endpoint, struct TCPPluginState * pluginStateTCP);

void TCPServerAcceptClient(struct TCPPluginState * pluginStateTCP);

void TCPServerCheckHealth(struct TCPPluginState * pluginStateTCP);

void TCPServerTCPData(struct TCPPluginState * pluginStateTCP);

void TCPServerTunnelData(struct TCPPluginState * pluginStateTCP);

#endif //CODE_SERVER_HANDLERS_H
