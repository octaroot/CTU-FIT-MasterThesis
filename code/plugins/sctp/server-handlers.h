#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "sctp.h"

void SCTPServerInitialize(struct sockaddr_in * endpoint, struct SCTPPluginState * pluginStateSCTP);

void SCTPServerAcceptClient(struct SCTPPluginState * pluginStateSCTP);

void SCTPServerCheckHealth(struct SCTPPluginState * pluginStateSCTP);

void SCTPServerSCTPData(struct SCTPPluginState * pluginStateSCTP);

void SCTPServerTunnelData(struct SCTPPluginState * pluginStateSCTP);

#endif //CODE_SERVER_HANDLERS_H
