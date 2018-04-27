#ifndef CODE_UDP_H
#define CODE_UDP_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/udp.h>

#define UDP_PLUGIN_VERSION "1.0.0-udp"
#define UDP_KEEPALIVE_TIMEOUT	10
#define UDP_MAX_AUTH_REQUESTS	10


extern int tunDeviceFD;

bool _UDPRunning;

void _UDPCleanup();

bool _UDPTestAvailability(uint32_t address);

const char *_UDPGetVersion();

void _UDPStart(uint32_t address, bool serverMode);

void _UDPStop();

typedef struct UDPHandlers
{
	void (*initialize)(struct sockaddr_in * endpoint);

	void (*checkHealth)(struct sockaddr_in * endpoint);

	void (*UDPData)(struct sockaddr_in * endpoint);

	void (*tunnelData)(struct sockaddr_in * endpoint);

} UDPHandlers;

typedef struct UDPPluginState
{
	bool connected;
	struct sockaddr_in * endpoint;
	int socket;
	int noReplyCount;

} UDPPluginState;

#endif //CODE_UDP_H
