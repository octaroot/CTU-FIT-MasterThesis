#ifndef CODE_ICMP_H
#define CODE_ICMP_H

#include <stdint.h>

#define ICMP_PLUGIN_VERSION "1.0.0-icmp"

extern int tunDeviceFD;

bool _ICMPRunning;

void _ICMPCleanup();

bool _ICMPTestAvailability(uint32_t endpoint);

const char *_ICMPGetVersion();

void _ICMPStart(uint32_t endpoint, bool serverMode);

void _ICMPStop();

#endif //CODE_ICMP_H
