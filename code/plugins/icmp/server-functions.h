#ifndef CODE_SERVER_FUNCTIONS_H
#define CODE_SERVER_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

extern uint16_t ICMPSequenceNumber, ICMPIDNumber;

extern int _ICMPSocketFD;

extern struct ICMPPluginState pluginState;

void ICMPHandlConnectionRequest(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request);

void ICMPHandleNATPacket(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request);

void ICMPHandleICMPData(struct ICMPEchoMessage *msg);

void ICMPHandleKeepAlive(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request);
#endif //CODE_SERVER_FUNCTIONS_H