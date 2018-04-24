#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>

#include "udp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

int _UDPSocketFD;

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
	UDPSocketClose(_UDPSocketFD);
}

const char *_UDPGetVersion()
{
	return UDP_PLUGIN_VERSION;
}

void _UDPStart(uint32_t endpoint, bool serverMode)
{
	pluginState.noReplyCount = 0;
	pluginState.connected = false;
	UDPHandlers handlers[] = {
			{UDPClientInitialize, UDPClientCheckHealth, UDPClientUDPData, UDPClientTunnelData},
			{UDPServerInitialize, UDPServerCheckHealth, UDPServerUDPData, UDPServerTunnelData},
	};

	_UDPRunning = true;

	_UDPSocketFD = UDPSocketOpen();

	if (!_UDPSocketFD)
	{
		_UDPRunning = false;
		return;
	}

	UDPHandlers *handler = &(handlers[serverMode]);

	handler->initialize(endpoint);

	int maxFD = MAX(_UDPSocketFD, tunDeviceFD);
	struct timeval timeout;

	while (_UDPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(_UDPSocketFD, &fs);
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

		if (FD_ISSET(_UDPSocketFD, &fs))
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

