#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>
#include "packet.h"

extern int tunDeviceFD;

void SCTPClientInitialize(struct sockaddr_in * endpoint, struct SCTPPluginState * pluginStateSCTP);

void SCTPClientAcceptClient(struct SCTPPluginState * pluginStateSCTP);

void SCTPClientCheckHealth(struct SCTPPluginState * pluginStateSCTP);

void SCTPClientSCTPData(struct SCTPPluginState * pluginStateSCTP);

void SCTPClientTunnelData(struct SCTPPluginState * pluginStateSCTP);

#endif //CODE_CLIENT_HANDLERS_H
