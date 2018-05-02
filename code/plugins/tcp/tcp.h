#ifndef CODE_TCP_H
#define CODE_TCP_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define TCP_PLUGIN_VERSION "1.0.0-tcp"
#define TCP_KEEPALIVE_TIMEOUT	10
#define TCP_MAX_AUTH_REQUESTS	10


extern int tunDeviceFD;

bool _TCPRunning;

void _TCPCleanup();

bool _TCPTestAvailability(uint32_t address);

const char *_TCPGetVersion();

void _TCPStart(uint32_t address, bool serverMode);

void _TCPStop();

void _TCPStopClient();

typedef struct TCPHandlers
{
	void (*initialize)(struct sockaddr_in * endpoint);

	void (*acceptClient)();

	void (*checkHealth)(struct sockaddr_in * endpoint);

	void (*TCPData)(struct sockaddr_in * endpoint);

	void (*tunnelData)(struct sockaddr_in * endpoint);

} TCPHandlers;

typedef struct TCPPluginState
{
	bool connected;
	bool auth;
	struct sockaddr_in * endpoint;
	int socket;
	int listener;
	int noReplyCount;

} TCPPluginState;

#endif //CODE_TCP_H
