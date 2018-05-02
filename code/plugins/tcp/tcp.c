#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

#include "tcp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

struct TCPPluginState pluginState;

bool _TCPTestAvailability(uint32_t endpoint)
{
	//TODO

	_TCPCleanup();
}

void _TCPCleanup()
{
	pluginState.connected = false;
	pluginState.noReplyCount = 0;
	TCPSocketClose(pluginState.socket);
	TCPSocketClose(pluginState.listener);
}

const char *_TCPGetVersion()
{
	return TCP_PLUGIN_VERSION;
}

void _TCPStart(uint32_t address, bool serverMode)
{
	pluginState.noReplyCount = 0;
	pluginState.connected = false;
	pluginState.endpoint = malloc(sizeof(struct sockaddr_in));


	TCPHandlers handlers[] = {
			{TCPClientInitialize, TCPClientAcceptClient, TCPClientCheckHealth, TCPClientTCPData, TCPClientTunnelData},
			{TCPServerInitialize, TCPServerAcceptClient, TCPServerCheckHealth, TCPServerTCPData, TCPServerTunnelData},
	};

	_TCPRunning = true;

	pluginState.listener = TCPSocketOpen();

	if (!pluginState.listener)
	{
		_TCPRunning = false;
		return;
	}

	TCPHandlers *handler = &(handlers[serverMode]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(5060);

	handler->initialize(&endpoint);

	while (_TCPRunning)
	{
		handler->acceptClient();

		int maxFD = MAX(pluginState.socket, tunDeviceFD);
		struct timeval timeout;

		while (_TCPRunning && pluginState.connected)
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
				if (!_TCPRunning)
				{
					break;
				}

				fprintf(stderr, "Unable to select() on sockets: %s\n", strerror(errno));
				return;
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
				handler->TCPData(pluginState.endpoint);
			}
		}
	}
	_TCPCleanup();
}

void _TCPStop()
{
	_TCPRunning = false;
}

