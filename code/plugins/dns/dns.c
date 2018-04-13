#include <stdio.h>
#include <stdbool.h>
#include "dns.h"


bool _DNSTestAvailability(unsigned int endpoint)
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

void _DNSStart(unsigned int endpoint)
{
	_DNSRunning = true;
	while (_DNSRunning)
	{
		//TODO
	}

	_DNSCleanup();
}

void _DNSStop()
{
	_DNSRunning = false;
}

