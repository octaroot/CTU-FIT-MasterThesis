#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

void UDPSendConnectionRequest(struct UDPPluginState * pluginState);

void UDPSendKeepAlive(struct UDPPluginState * pluginState);

void UDPHandleConnectionAccept(struct UDPPluginState * pluginState);

void UDPHandleAuthChallenge(struct UDPPluginState * pluginState, struct UDPMessage *origMsg);

void UDPHandleConnectionReject(struct UDPPluginState * pluginState);

void UDPHandleUDPData(struct UDPMessage *msg);

void UDPHandleKeepAliveResponse(struct UDPPluginState * pluginState);

#endif //CODE_CLIENT_FUNCTIONS_H
