#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

void TCPHandleConnectionRequest(struct TCPPluginState * pluginStateTCP, struct TCPMessage *request);

void TCPHandleTCPData(struct TCPMessage *msg);

void TCPHandleKeepAlive(struct TCPPluginState * pluginStateTCP, struct TCPMessage * request);

void TCPHandleAuthResponse(struct TCPPluginState * pluginStateTCP, struct TCPMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
