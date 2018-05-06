#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

void SCTPHandleConnectionRequest(struct SCTPPluginState* pluginStateSCTP);

void SCTPHandleKeepAlive(struct SCTPPluginState* pluginStateSCTP, struct SCTPMessage * request);

void SCTPHandleAuthResponse(struct SCTPPluginState* pluginStateSCTP, struct SCTPMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
