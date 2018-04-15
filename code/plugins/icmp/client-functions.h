#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern uint16_t ICMPSequenceNumber, ICMPIDNumber;

extern int _ICMPSocketFD;

void ICMPSendConnectionRequest(int socketFD, uint32_t endpoint);

void ICMPSendKeepAlive(int socketFD, uint32_t endpoint);

void ICMPHandleConnectionAccept(int socketFD, uint32_t endpoint);

void ICMPHandleConnectionReject(int socketFD, uint32_t endpoint);

void ICMPHandleICMPData(struct ICMPEchoMessage *msg);

void ICMPHandleKeepAliveResponse();

#endif //CODE_CLIENT_FUNCTIONS_H
