#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "icmp.h"

extern uint16_t NATSequenceNumberIdx,NATSequenceNumbers[ICMP_NAT_PACKET_COUNT];

void ICMPServerInitialize(uint32_t endpoint);

void ICMPServerCheckHealth(uint32_t endpoint);

void ICMPServerICMPData(uint32_t endpoint);

void ICMPServerTunnelData(uint32_t endpoint);

#endif //CODE_SERVER_HANDLERS_H
