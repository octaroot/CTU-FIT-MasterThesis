#ifndef CODE_SCTP_H
#define CODE_SCTP_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#define SCTP_PLUGIN_VERSION "1.0.0-sctp"
#define SCTP_KEEPALIVE_TIMEOUT	10
#define SCTP_MAX_AUTH_REQUESTS	10

#define SCTP_STREAM_CONTROL     0
#define SCTP_STREAM_DATA        1

extern int tunDeviceFD;

typedef struct SCTPPluginState
{
	bool connected;
	bool auth;
	struct sockaddr_in * endpoint;
	int socket;
	int listener;
	int noReplyCount;

} SCTPPluginState;


bool _SCTPRunning;

void _SCTPCleanup(struct SCTPPluginState* pluginStateSCTP);

bool _SCTPTestAvailability(uint32_t address);

const char *_SCTPGetVersion();

void _SCTPStart(uint32_t address, bool serverMode);

void _SCTPStop();

void _SCTPStopClient(struct SCTPPluginState* pluginStateSCTP);


typedef struct SCTPHandlers
{
	void (*initialize)(struct sockaddr_in * endpoint, struct SCTPPluginState* pluginStateSCTP);

	void (*acceptClient)(struct SCTPPluginState* pluginStateSCTP);

	void (*checkHealth)(struct SCTPPluginState* pluginStateSCTP);

	void (*SCTPData)(struct SCTPPluginState* pluginStateSCTP);

	void (*tunnelData)(struct SCTPPluginState* pluginStateSCTP);

} SCTPHandlers;

#endif //CODE_SCTP_H
