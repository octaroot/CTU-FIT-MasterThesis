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

struct TCPPluginState pluginStateTCP;

bool _TCPTestAvailability(uint32_t endpoint)
{
	//TODO

	_TCPCleanup();
}

void _TCPCleanup()
{
	pluginStateTCP.connected = false;
	pluginStateTCP.noReplyCount = 0;
	TCPSocketClose(pluginStateTCP.socket);
	TCPSocketClose(pluginStateTCP.listener);
}

const char *_TCPGetVersion()
{
	return TCP_PLUGIN_VERSION;
}

void _TCPStart(uint32_t address, bool serverMode)
{
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
	endpoint.sin_port = htons(5060);

	handler->initialize(&endpoint);

	while (_TCPRunning)
	{
		handler->acceptClient();

		int maxFD = MAX(pluginStateTCP.socket, tunDeviceFD);
		struct timeval timeout;

		while (_TCPRunning && pluginStateTCP.connected)
		{
			fd_set fs;

			FD_ZERO(&fs);
			FD_SET(pluginStateTCP.socket, &fs);
			FD_SET(tunDeviceFD, &fs);

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			int lenAvailable = select(maxFD + 1, &fs, NULL, NULL, &timeout);

			if (lenAvailable < 0)
			{
				_TCPStopClient();
				_TCPStop();
				break;
			}

			if (lenAvailable == 0)
			{
				handler->checkHealth(pluginStateTCP.endpoint);
				continue;
			}

			if (pluginStateTCP.auth && FD_ISSET(tunDeviceFD, &fs))
			{
				handler->tunnelData(pluginStateTCP.endpoint);
			}

			if (FD_ISSET(pluginStateTCP.socket, &fs))
			{
				handler->TCPData(pluginStateTCP.endpoint);
			}
		}
	}
	_TCPCleanup();
}

void _TCPStop()
{
	_TCPRunning = false;
}

void _TCPStopClient()
{
	pluginStateTCP.connected = false;
	pluginStateTCP.auth = false;
	close(pluginStateTCP.socket);
}