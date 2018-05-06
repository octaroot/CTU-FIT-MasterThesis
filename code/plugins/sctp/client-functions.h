#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

void SCTPSendConnectionRequest(struct SCTPPluginState * pluginStateSCTP);

void SCTPSendKeepAlive(struct SCTPPluginState * pluginStateSCTP);

void SCTPHandleConnectionAccept(struct SCTPPluginState * pluginStateSCTP);

void SCTPHandleAuthChallenge(struct SCTPPluginState * pluginStateSCTP, struct SCTPMessage *origMsg);

void SCTPHandleConnectionReject(struct SCTPPluginState * pluginStateSCTP);

void SCTPHandleKeepAliveResponse(struct SCTPPluginState * pluginStateSCTP);

#endif //CODE_CLIENT_FUNCTIONS_H
