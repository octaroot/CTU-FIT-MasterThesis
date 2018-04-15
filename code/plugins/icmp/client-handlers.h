#ifndef CODE_CLIENT_HANDLERS_H
#define CODE_CLIENT_HANDLERS_H

#include <stdint.h>

extern int tunDeviceFD;

void ICMPClientInitialize(uint32_t endpoint);

void ICMPClientCheckHealth(uint32_t endpoint);

void ICMPClientICMPData(uint32_t endpoint);

void ICMPClientTunnelData(uint32_t endpoint);

#endif //CODE_CLIENT_HANDLERS_H
