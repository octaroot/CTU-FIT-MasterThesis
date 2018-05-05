#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern struct SCTPPluginState pluginStateSCTP;

void SCTPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage *request);

void SCTPHandleSCTPData(struct SCTPMessage *msg);

void SCTPHandleKeepAlive(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage * request);

void SCTPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
