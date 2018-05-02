#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

#include "sctp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

struct SCTPPluginState pluginState;

bool _SCTPTestAvailability(uint32_t endpoint)
{
	//TODO

	_SCTPCleanup();
}

void _SCTPCleanup()
{
	pluginState.connected = false;
	pluginState.noReplyCount = 0;
	SCTPSocketClose(pluginState.socket);
	SCTPSocketClose(pluginState.listener);
}

const char *_SCTPGetVersion()
{
	return SCTP_PLUGIN_VERSION;
}

void _SCTPStart(uint32_t address, bool serverMode)
{
	pluginState.noReplyCount = 0;
	pluginState.connected = false;
	pluginState.endpoint = malloc(sizeof(struct sockaddr_in));


	SCTPHandlers handlers[] = {
			{SCTPClientInitialize, SCTPClientAcceptClient, SCTPClientCheckHealth, SCTPClientSCTPData, SCTPClientTunnelData},
			{SCTPServerInitialize, SCTPServerAcceptClient, SCTPServerCheckHealth, SCTPServerSCTPData, SCTPServerTunnelData},
	};

	_SCTPRunning = true;

	pluginState.listener = SCTPSocketOpen();

	if (!pluginState.listener)
	{
		_SCTPRunning = false;
		return;
	}

	SCTPHandlers *handler = &(handlers[serverMode]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(5060);

	handler->initialize(&endpoint);

	while (_SCTPRunning)
	{
		handler->acceptClient();

		int maxFD = MAX(pluginState.socket, tunDeviceFD);
		struct timeval timeout;

		while (_SCTPRunning && pluginState.connected)
		{
			fd_set fs;

			FD_ZERO(&fs);
			FD_SET(pluginState.socket, &fs);
			FD_SET(tunDeviceFD, &fs);

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			int lenAvailable = select(maxFD + 1, &fs, NULL, NULL, &timeout);

			if (lenAvailable < 0)
			{
				_SCTPStopClient();
				_SCTPStop();
				break;
			}

			if (lenAvailable == 0)
			{
				handler->checkHealth(pluginState.endpoint);
				continue;
			}

			if (pluginState.auth && FD_ISSET(tunDeviceFD, &fs))
			{
				handler->tunnelData(pluginState.endpoint);
			}

			if (FD_ISSET(pluginState.socket, &fs))
			{
				handler->SCTPData(pluginState.endpoint);
			}
		}
	}
	_SCTPCleanup();
}

void _SCTPStop()
{
	_SCTPRunning = false;
}

void _SCTPStopClient()
{
	pluginState.connected = false;
	pluginState.auth = false;
	close(pluginState.socket);
}