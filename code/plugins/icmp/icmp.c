#include <stdio.h>
#include <stdbool.h>
#include "icmp.h"


bool _ICMPTestAvailability(unsigned int endpoint)
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

void _ICMPStart(unsigned int endpoint)
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

