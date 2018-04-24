#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern uint16_t UDPSequenceNumber, UDPIDNumber;

extern int _UDPSocketFD;

extern struct UDPPluginState pluginState;

void UDPSendConnectionRequest(int socketFD, uint32_t endpoint);

void UDPSendKeepAlive(int socketFD, uint32_t endpoint);

void UDPHandleConnectionAccept(int socketFD, uint32_t endpoint);

void UDPHandleAuthChallenge(int socketFD, uint32_t endpoint, struct UDPEchoMessage *origMsg);

void UDPHandleConnectionReject(int socketFD, uint32_t endpoint);

void UDPHandleUDPData(struct UDPEchoMessage *msg);

void UDPHandleKeepAliveResponse(int socketFD, uint32_t endpoint);

#endif //CODE_CLIENT_FUNCTIONS_H
