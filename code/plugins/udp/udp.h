#ifndef CODE_UDP_H
#define CODE_UDP_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/udp.h>


#define UDP_PLUGIN_VERSION "1.0.0-udp"
#define UDP_PLUGIN_NAME "udp"
#define UDP_KEEPALIVE_TIMEOUT    10
#define UDP_MAX_AUTH_REQUESTS    10


extern int tunDeviceFD;


typedef struct UDPPluginState
{
	bool connected;
	struct sockaddr_in *endpoint;
	int socket;
	int noReplyCount;

} UDPPluginState;

typedef struct UDPHandlers
{
	void (*initialize)(struct sockaddr_in *endpoint, struct UDPPluginState *pluginState);

	void (*checkHealth)(struct UDPPluginState *pluginState);

	void (*UDPData)(struct UDPPluginState *pluginState);

	void (*tunnelData)(struct UDPPluginState *pluginState);

} UDPHandlers;


bool _UDPRunning;


void _UDPCleanup(struct UDPPluginState *pluginStateUDP);

bool _UDPTestAvailability(uint32_t address);

const char *_UDPGetVersion();

const char *_UDPGetName();

void _UDPStart(uint32_t address, int port, bool serverMode);

void _UDPStop();


#endif //CODE_UDP_H
