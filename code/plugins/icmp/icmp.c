#include <stdio.h>
#include <stdbool.h>
#include <memory.h>

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
	_ICMPRunning = true;

	_ICMPSocketFD = ICMPSocketOpen();

	if (!_ICMPSocketFD)
	{
		_ICMPRunning = false;
		return;
	}

	if (serverMode)
	{
		ICMPRunServer(endpoint);
	}
	else
	{
		ICMPRunClient(endpoint);
	}

	_ICMPCleanup();
}

void ICMPRunClient(uint32_t endpoint)
{
	clientInitialize();
	sendConnectionRequest(_ICMPSocketFD, endpoint);

	while (_ICMPRunning)
	{
		//TODO
	}
}

void ICMPRunServer(uint32_t endpoint)
{
	while (_ICMPRunning)
	{
		//TODO
	}
}

void _ICMPStop()
{
	_ICMPRunning = false;
}

