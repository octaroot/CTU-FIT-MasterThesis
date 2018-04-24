#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>

extern int tunDeviceFD;

void UDPClientInitialize(uint32_t endpoint);

void UDPClientCheckHealth(uint32_t endpoint);

void UDPClientUDPData(uint32_t endpoint);

void UDPClientTunnelData(uint32_t endpoint);

#endif //CODE_CLIENT_HANDLERS_H
