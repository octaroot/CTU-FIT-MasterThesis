#ifndef CODE_ICMP_H
#define CODE_ICMP_H

#define ICMP_PLUGIN_VERSION "1.0.0-icmp"
#define ICMP_PACKET_MAGIC "CRNC"

extern int tunDeviceFD;

bool _ICMPRunning;

void _ICMPCleanup();

bool _ICMPTestAvailability(unsigned int endpoint);

const char *_ICMPGetVersion();

void _ICMPStart(unsigned int endpoint);

void _ICMPStop();

#endif //CODE_ICMP_H
