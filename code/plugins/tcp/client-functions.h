#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

void TCPSendConnectionRequest(struct TCPPluginState * pluginStateTCP);

void TCPSendKeepAlive(struct TCPPluginState * pluginStateTCP);

void TCPHandleConnectionAccept(struct TCPPluginState * pluginStateTCP);

void TCPHandleAuthChallenge(struct TCPPluginState * pluginStateTCP, struct TCPMessage *origMsg);

void TCPHandleConnectionReject(struct TCPPluginState * pluginStateTCP);

void TCPHandleTCPData(struct TCPMessage *msg);

void TCPHandleKeepAliveResponse(struct TCPPluginState * pluginStateTCP);

#endif //CODE_CLIENT_FUNCTIONS_H
