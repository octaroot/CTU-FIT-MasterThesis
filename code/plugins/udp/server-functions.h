#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern uint16_t UDPSequenceNumber, UDPIDNumber;

extern int _UDPSocketFD;

extern struct UDPPluginState pluginState;

void UDPHandlConnectionRequest(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request);

void UDPHandleNATPacket(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request);

void UDPHandleUDPData(struct UDPEchoMessage *msg);

void UDPHandleKeepAlive(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request);

void UDPHandleAuthResponse(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H
