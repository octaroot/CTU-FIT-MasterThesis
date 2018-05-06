#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

#include "udp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

bool _UDPTestAvailability(uint32_t endpoint)
{
	//TODO
	struct UDPPluginState pluginStateUDP;

	_UDPCleanup(&pluginStateUDP);
}

void _UDPCleanup(struct UDPPluginState * pluginStateUDP)
{
	pluginStateUDP->connected = false;
	pluginStateUDP->noReplyCount = 0;
	UDPSocketClose(pluginStateUDP->socket);
}

const char *_UDPGetVersion()
{
	return UDP_PLUGIN_VERSION;
}

void _UDPStart(uint32_t address, bool serverMode)
{
	struct UDPPluginState pluginStateUDP;

	pluginStateUDP.noReplyCount = 0;
	pluginStateUDP.connected = false;
	pluginStateUDP.endpoint = malloc(sizeof(struct sockaddr_in));


	UDPHandlers handlers[] = {
			{UDPClientInitialize, UDPClientCheckHealth, UDPClientUDPData, UDPClientTunnelData},
			{UDPServerInitialize, UDPServerCheckHealth, UDPServerUDPData, UDPServerTunnelData},
	};

	_UDPRunning = true;

	pluginStateUDP.socket = UDPSocketOpen();

	if (!pluginStateUDP.socket)
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

	handler->initialize(&endpoint, &pluginStateUDP);

	int maxFD = MAX(pluginStateUDP.socket, tunDeviceFD);
	struct timeval timeout;

	while (_UDPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(pluginStateUDP.socket, &fs);
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
			handler->checkHealth(&pluginStateUDP);
			continue;
		}

		if (pluginStateUDP.connected && FD_ISSET(tunDeviceFD, &fs))
		{
			handler->tunnelData(&pluginStateUDP);
		}

		if (FD_ISSET(pluginStateUDP.socket, &fs))
		{
			handler->UDPData(&pluginStateUDP);
		}
	}

	_UDPCleanup(&pluginStateUDP);
}

void _UDPStop()
{
	_UDPRunning = false;
}

