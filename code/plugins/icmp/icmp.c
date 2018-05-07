#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>

#include "icmp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

int _ICMPSocketFD;

struct ICMPPluginState pluginStateICMP;

bool _ICMPTestAvailability(uint32_t endpoint, int port)
{
	pluginStateICMP.noReplyCount = 0;
	pluginStateICMP.connected = false;
	ICMPHandlers handlers[] = {
			{ICMPClientInitialize, ICMPClientCheckHealth, ICMPClientICMPData, ICMPClientTunnelData},
	};

	_ICMPRunning = true;

	_ICMPSocketFD = ICMPSocketOpen();

	if (!_ICMPSocketFD)
	{
		_ICMPRunning = false;
		return false;
	}

	ICMPHandlers *handler = &(handlers[0]);

	handler->initialize(endpoint);

	struct timeval timeout;

	while (_ICMPRunning && !pluginStateICMP.connected && pluginStateICMP.noReplyCount < ICMP_KEEPALIVE_TIMEOUT)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(_ICMPSocketFD, &fs);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int lenAvailable = select(_ICMPSocketFD + 1, &fs, NULL, NULL, &timeout);

		if (lenAvailable < 0)
		{
			if (!_ICMPRunning)
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
			++pluginStateICMP.noReplyCount;
			continue;
		}

		handler->ICMPData(pluginStateICMP.endpoint);
	}

	bool success = pluginStateICMP.connected;

	_ICMPCleanup();

	return success;
}

void _ICMPCleanup()
{
	pluginStateICMP.connected = false;
	pluginStateICMP.noReplyCount = 0;
	ICMPSocketClose(_ICMPSocketFD);
}

const char *_ICMPGetVersion()
{
	return ICMP_PLUGIN_VERSION;
}

const char *_ICMPGetName()
{
	return ICMP_PLUGIN_NAME;
}

void _ICMPStart(uint32_t endpoint, int port, bool serverMode)
{
	pluginStateICMP.noReplyCount = 0;
	pluginStateICMP.connected = false;
	ICMPHandlers handlers[] = {
			{ICMPClientInitialize, ICMPClientCheckHealth, ICMPClientICMPData, ICMPClientTunnelData},
			{ICMPServerInitialize, ICMPServerCheckHealth, ICMPServerICMPData, ICMPServerTunnelData},
	};

	_ICMPRunning = true;

	_ICMPSocketFD = ICMPSocketOpen();

	if (!_ICMPSocketFD)
	{
		_ICMPRunning = false;
		return;
	}

	ICMPHandlers *handler = &(handlers[serverMode]);

	handler->initialize(endpoint);

	int maxFD = MAX(_ICMPSocketFD, tunDeviceFD);
	struct timeval timeout;

	while (_ICMPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(_ICMPSocketFD, &fs);

		if (pluginStateICMP.connected)
			FD_SET(tunDeviceFD, &fs);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int lenAvailable = select(maxFD + 1, &fs, NULL, NULL, &timeout);

		if (lenAvailable < 0)
		{
			if (!_ICMPRunning)
			{
				break;
			}

			fprintf(stderr, "Unable to select() on sockets: %s\n", strerror(errno));
			return;
		}

		if (lenAvailable == 0)
		{
			handler->checkHealth(pluginStateICMP.endpoint);
		}

		if (FD_ISSET(tunDeviceFD, &fs))
		{
			handler->tunnelData(pluginStateICMP.endpoint);
		}

		if (FD_ISSET(_ICMPSocketFD, &fs))
		{
			handler->ICMPData(pluginStateICMP.endpoint);
		}
	}

	_ICMPCleanup();
}

void _ICMPStop()
{
	_ICMPRunning = false;
}

