#ifndef CODE_TCP_H
#define CODE_TCP_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define TCP_PLUGIN_VERSION "1.0.0-tcp"
#define TCP_PLUGIN_NAME "tcp"
#define TCP_KEEPALIVE_TIMEOUT	5
#define TCP_MAX_AUTH_REQUESTS	10


extern int tunDeviceFD;

bool _TCPRunning;

typedef struct TCPPluginState
{
	bool connected;
	bool auth;
	struct sockaddr_in * endpoint;
	int socket;
	int listener;
	int noReplyCount;

} TCPPluginState;

void _TCPCleanup(struct TCPPluginState * pluginStateTCP);

bool _TCPTestAvailability(uint32_t address, int port);

const char *_TCPGetVersion();

const char *_TCPGetName();

void _TCPStart(uint32_t address, int port,  bool serverMode);

void _TCPStop();

void _TCPStopClient(struct TCPPluginState * pluginStateTCP);

typedef struct TCPHandlers
{
	void (*initialize)(struct sockaddr_in * endpoint, struct TCPPluginState * pluginStateTCP);

	void (*acceptClient)(struct TCPPluginState * pluginStateTCP);

	void (*checkHealth)(struct TCPPluginState * pluginStateTCP);

	void (*TCPData)(struct TCPPluginState * pluginStateTCP);

	void (*tunnelData)(struct TCPPluginState * pluginStateTCP);

} TCPHandlers;

#endif //CODE_TCP_H
