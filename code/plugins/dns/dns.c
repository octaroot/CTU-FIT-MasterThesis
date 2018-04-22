#include <stdio.h>
#include <stdbool.h>
#include "dns.h"


bool _DNSTestAvailability(uint32_t endpoint)
{
	//TODO

	_DNSCleanup();
}

void _DNSCleanup()
{
	//close the socket etc
}

const char *_DNSGetVersion()
{
	return DNS_PLUGIN_VERSION;
}

void _DNSStart(uint32_t endpoint, bool serverMode)
{
	_DNSRunning = true;
	while (_DNSRunning)
	{
		//TODO
		_DNSRunning = false;
	}

	_DNSCleanup();
}

void _DNSStop()
{
	_DNSRunning = false;
}

