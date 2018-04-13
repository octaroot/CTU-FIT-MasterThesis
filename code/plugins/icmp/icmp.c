#include <stdio.h>
#include <stdbool.h>
#include "icmp.h"
#include "client.h"

bool _ICMPTestAvailability(uint32_t endpoint)
{
	//TODO

	_ICMPCleanup();
}

void _ICMPCleanup()
{
	//close the socket etc
}

const char *_ICMPGetVersion()
{
	return ICMP_PLUGIN_VERSION;
}

void _ICMPStart(uint32_t endpoint, bool serverMode)
{
	_ICMPRunning = true;
	while (_ICMPRunning)
	{
		//TODO
	}

	_ICMPCleanup();
}

void _ICMPStop()
{
	_ICMPRunning = false;
}

