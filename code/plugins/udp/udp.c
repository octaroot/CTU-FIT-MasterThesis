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

bool _UDPTestAvailability(uint32_t address, int port)
{
	struct UDPPluginState pluginStateUDP;

	pluginStateUDP.noReplyCount = 0;
	pluginStateUDP.connected = false;
	pluginStateUDP.endpoint = malloc(sizeof(struct sockaddr_in));


	UDPHandlers handlers[] = {
			{UDPClientInitialize, UDPClientCheckHealth, UDPClientUDPData, UDPClientTunnelData},
	};

	_UDPRunning = true;

	pluginStateUDP.socket = UDPSocketOpen();

	if (!pluginStateUDP.socket)
	{
		_UDPRunning = false;
		return false;
	}

	UDPHandlers *handler = &(handlers[0]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(port);

	handler->initialize(&endpoint, &pluginStateUDP);

	struct timeval timeout;

	while (_UDPRunning && !pluginStateUDP.connected && pluginStateUDP.noReplyCount < UDP_KEEPALIVE_TIMEOUT)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(pluginStateUDP.socket, &fs);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int lenAvailable = select(pluginStateUDP.socket + 1, &fs, NULL, NULL, &timeout);

		if (lenAvailable < 0)
		{
			if (!_UDPRunning)
			{
				break;
			}

			fprintf(stderr, "Unable to select() on sockets: %s\n", strerror(errno));
			return false;
		}

		if (lenAvailable == 0)
		{
			printf(".");
			fflush(stdout);
			++pluginStateUDP.noReplyCount;
			continue;
		}

		handler->UDPData(&pluginStateUDP);

	}

	bool success = pluginStateUDP.connected;

	_UDPCleanup(&pluginStateUDP);

	return success;
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

const char *_UDPGetName()
{
	return UDP_PLUGIN_NAME;
}

void _UDPStart(uint32_t address, int port,  bool serverMode)
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
	endpoint.sin_port = htons(port);

	handler->initialize(&endpoint, &pluginStateUDP);

	int maxFD = MAX(pluginStateUDP.socket, tunDeviceFD);
	struct timeval timeout;

	while (_UDPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(pluginStateUDP.socket, &fs);

		if (pluginStateUDP.connected)
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

		if (FD_ISSET(tunDeviceFD, &fs))
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

