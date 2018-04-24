#ifndef CODE_UDP_H
#define CODE_UDP_H

#include <stdint.h>
#include <stdbool.h>

#define UDP_PLUGIN_VERSION "1.0.0-udp"
#define UDP_NAT_PACKET_COUNT	10
#define UDP_KEEPALIVE_TIMEOUT	10
#define UDP_MAX_AUTH_REQUESTS	10


extern int tunDeviceFD;

bool _UDPRunning;

void _UDPCleanup();

bool _UDPTestAvailability(uint32_t endpoint);

const char *_UDPGetVersion();

void _UDPStart(uint32_t endpoint, bool serverMode);

void _UDPStop();

typedef struct UDPHandlers
{
	void (*initialize)(uint32_t endpoint);

	void (*checkHealth)(uint32_t endpoint);

	void (*UDPData)(uint32_t endpoint);

	void (*tunnelData)(uint32_t endpoint);

} UDPHandlers;

typedef struct UDPPluginState
{
	bool connected;
	uint32_t endpoint;
	int noReplyCount;

} UDPPluginState;

#endif //CODE_UDP_H
