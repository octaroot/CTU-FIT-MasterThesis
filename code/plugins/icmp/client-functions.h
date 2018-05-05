#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern uint16_t ICMPSequenceNumber, ICMPIDNumber;

extern int _ICMPSocketFD;

extern struct ICMPPluginState pluginStateICMP;

void ICMPSendConnectionRequest(int socketFD, uint32_t endpoint);

void ICMPSendKeepAlive(int socketFD, uint32_t endpoint);

void ICMPHandleConnectionAccept(int socketFD, uint32_t endpoint);

void ICMPHandleAuthChallenge(int socketFD, uint32_t endpoint, struct ICMPEchoMessage *origMsg);

void ICMPHandleConnectionReject(int socketFD, uint32_t endpoint);

void ICMPHandleICMPData(struct ICMPEchoMessage *msg);

void ICMPHandleKeepAliveResponse(int socketFD, uint32_t endpoint);

#endif //CODE_CLIENT_FUNCTIONS_H
