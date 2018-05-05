#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern struct TCPPluginState pluginStateTCP;

void TCPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage *request);

void TCPHandleTCPData(struct TCPMessage *msg);

void TCPHandleKeepAlive(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage * request);

void TCPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
