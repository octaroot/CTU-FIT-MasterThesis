#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern struct TCPPluginState pluginStateTCP;

void TCPSendConnectionRequest(int socketFD, struct sockaddr_in * endpoint);

void TCPSendKeepAlive(int socketFD, struct sockaddr_in * endpoint);

void TCPHandleConnectionAccept(struct sockaddr_in * endpoint);

void TCPHandleAuthChallenge(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage *origMsg);

void TCPHandleConnectionReject(int socketFD, struct sockaddr_in * endpoint);

void TCPHandleTCPData(struct TCPMessage *msg);

void TCPHandleKeepAliveResponse();

#endif //CODE_CLIENT_FUNCTIONS_H
