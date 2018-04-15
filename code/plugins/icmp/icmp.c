#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>

#include "icmp.h"
#include "client.h"
#include "packet.h"

#include "client-functions.h"

int _ICMPSocketFD;

bool _ICMPTestAvailability(uint32_t endpoint)
{
	//TODO

	_ICMPCleanup();
}

void _ICMPCleanup()
{
	ICMPSocketClose(_ICMPSocketFD);
}

const char *_ICMPGetVersion()
{
	return ICMP_PLUGIN_VERSION;
}

void _ICMPStart(uint32_t endpoint, bool serverMode)
{
	ICMPHandlers handlers[] = {
			{ICMPclientInitialize}
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
		else if (lenAvailable == 0)
		{
			handler->checkHealth(endpoint);
		}

		if (FD_ISSET(tunDeviceFD, &fs))
		{
			handler->tunnelData(endpoint);
		}

		if (FD_ISSET(_ICMPSocketFD, &fs))
		{
			handler->ICMPData(endpoint);
		}
	}

	_ICMPCleanup();
}

void _ICMPStop()
{
	_ICMPRunning = false;
}

