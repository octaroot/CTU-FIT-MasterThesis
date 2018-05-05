#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern struct SCTPPluginState pluginStateSCTP;

void SCTPSendConnectionRequest(int socketFD, struct sockaddr_in * endpoint);

void SCTPSendKeepAlive(int socketFD, struct sockaddr_in * endpoint);

void SCTPHandleConnectionAccept(struct sockaddr_in * endpoint);

void SCTPHandleAuthChallenge(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage *origMsg);

void SCTPHandleConnectionReject(int socketFD, struct sockaddr_in * endpoint);

void SCTPHandleSCTPData(struct SCTPMessage *msg);

void SCTPHandleKeepAliveResponse();

#endif //CODE_CLIENT_FUNCTIONS_H
