#ifndef CODE_SERVER_HANDLERS_H
#define CODE_SERVER_HANDLERS_H

#include <stdint.h>
#include "udp.h"

extern uint16_t NATSequenceNumberIdx,NATSequenceNumbers[UDP_NAT_PACKET_COUNT];

void UDPServerInitialize(uint32_t endpoint);

void UDPServerCheckHealth(uint32_t endpoint);

void UDPServerUDPData(uint32_t endpoint);

void UDPServerTunnelData(uint32_t endpoint);

#endif //CODE_SERVER_HANDLERS_H
