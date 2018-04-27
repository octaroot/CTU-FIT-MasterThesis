#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>

#include "udp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

struct UDPPluginState pluginState;

bool _UDPTestAvailability(uint32_t endpoint)
{
	//TODO

	_UDPCleanup();
}

void _UDPCleanup()
{
	pluginState.connected = false;
	pluginState.noReplyCount = 0;
	UDPSocketClose(pluginState.socket);
}

const char *_UDPGetVersion()
{
	return UDP_PLUGIN_VERSION;
}

void _UDPStart(uint32_t address, bool serverMode)
{
	pluginState.noReplyCount = 0;
	pluginState.connected = false;
	UDPHandlers handlers[] = {
			{UDPClientInitialize, UDPClientCheckHealth, UDPClientUDPData, UDPClientTunnelData},
			{UDPServerInitialize, UDPServerCheckHealth, UDPServerUDPData, UDPServerTunnelData},
	};

	_UDPRunning = true;

	pluginState.socket = UDPSocketOpen();

	if (!pluginState.socket)
	{
		_UDPRunning = false;
		return;
	}

	UDPHandlers *handler = &(handlers[serverMode]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(5060);

	handler->initialize(&endpoint);

	int maxFD = MAX(pluginState.socket, tunDeviceFD);
	struct timeval timeout;

	while (_UDPRunning)
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
			if (!_UDPRunning)
			{
				break;
			}

			fprintf(stderr, "Unable to select() on sockets: %s\n", strerror(errno));
			return;
		}

		if (lenAvailable == 0)
		{
			handler->checkHealth(pluginState.endpoint);
		}

		if (pluginState.connected && FD_ISSET(tunDeviceFD, &fs))
		{
			handler->tunnelData(pluginState.endpoint);
		}

		if (FD_ISSET(pluginState.socket, &fs))
		{
			handler->UDPData(pluginState.endpoint);
		}
	}

	_UDPCleanup();
}

void _UDPStop()
{
	_UDPRunning = false;
}

