#ifndef CODE_ICMP_H
#define CODE_ICMP_H

#include <stdint.h>
#include <stdbool.h>

#define ICMP_PLUGIN_VERSION "1.0.0-icmp"

#define ICMP_NAT_PACKET_COUNT	10

extern int tunDeviceFD;

bool _ICMPRunning;

void _ICMPCleanup();

bool _ICMPTestAvailability(uint32_t endpoint);

const char *_ICMPGetVersion();

void _ICMPStart(uint32_t endpoint, bool serverMode);

void _ICMPStop();

typedef struct ICMPHandlers
{
	void (*initialize)(uint32_t endpoint);

	void (*checkHealth)(uint32_t endpoint);

	void (*ICMPData)(uint32_t endpoint);

	void (*tunnelData)(uint32_t endpoint);

} ICMPHandlers;

typedef struct ICMPPluginState
{
	bool connected;
	uint32_t endpoint;

} ICMPPluginState;

#endif //CODE_ICMP_H
