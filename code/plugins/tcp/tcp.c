#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

#include "tcp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

bool _TCPTestAvailability(uint32_t endpoint)
{
	struct TCPPluginState pluginStateTCP;
	//TODO

	_TCPCleanup(&pluginStateTCP);
}

void _TCPCleanup(struct TCPPluginState * pluginStateTCP)
{
	pluginStateTCP->connected = false;
	pluginStateTCP->noReplyCount = 0;
	TCPSocketClose(pluginStateTCP->socket);
	TCPSocketClose(pluginStateTCP->listener);
}

const char *_TCPGetVersion()
{
	return TCP_PLUGIN_VERSION;
}

const char *_TCPGetName()
{
	return TCP_PLUGIN_NAME;
}

void _TCPStart(uint32_t address, int port,  bool serverMode)
{
	struct TCPPluginState pluginStateTCP;

	pluginStateTCP.noReplyCount = 0;
	pluginStateTCP.connected = false;
	pluginStateTCP.endpoint = malloc(sizeof(struct sockaddr_in));


	TCPHandlers handlers[] = {
			{TCPClientInitialize, TCPClientAcceptClient, TCPClientCheckHealth, TCPClientTCPData, TCPClientTunnelData},
			{TCPServerInitialize, TCPServerAcceptClient, TCPServerCheckHealth, TCPServerTCPData, TCPServerTunnelData},
	};

	_TCPRunning = true;

	pluginStateTCP.listener = TCPSocketOpen();

	if (!pluginStateTCP.listener)
	{
		_TCPRunning = false;
		return;
	}

	TCPHandlers *handler = &(handlers[serverMode]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(port);

	handler->initialize(&endpoint, &pluginStateTCP);

	while (_TCPRunning)
	{
		handler->acceptClient(&pluginStateTCP);

		int maxFD = MAX(pluginStateTCP.socket, tunDeviceFD);
		struct timeval timeout;

		while (_TCPRunning && pluginStateTCP.connected)
		{
			fd_set fs;

			FD_ZERO(&fs);
			FD_SET(pluginStateTCP.socket, &fs);

			if (pluginStateTCP.auth)
				FD_SET(tunDeviceFD, &fs);

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			int lenAvailable = select(maxFD + 1, &fs, NULL, NULL, &timeout);

			if (lenAvailable < 0)
			{
				_TCPStopClient(&pluginStateTCP);
				_TCPStop();
				break;
			}

			if (lenAvailable == 0)
			{
				handler->checkHealth(&pluginStateTCP);
				continue;
			}

			if (FD_ISSET(tunDeviceFD, &fs))
			{
				handler->tunnelData(&pluginStateTCP);
			}

			if (FD_ISSET(pluginStateTCP.socket, &fs))
			{
				handler->TCPData(&pluginStateTCP);
			}
		}
	}
	_TCPCleanup(&pluginStateTCP);
}

void _TCPStop()
{
	_TCPRunning = false;
}

void _TCPStopClient(struct TCPPluginState * pluginStateTCP)
{
	pluginStateTCP->connected = false;
	pluginStateTCP->auth = false;
	close(pluginStateTCP->socket);
}