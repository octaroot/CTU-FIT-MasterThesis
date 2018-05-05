#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "mux.h"

#include "../plugins/icmp/icmp.h"
#include "../plugins/udp/udp.h"
#include "../plugins/tcp/tcp.h"
#include "../plugins/sctp/sctp.h"
#include "common.h"

#define PLUGIN_COUNT (sizeof(plugins) / sizeof(plugin))

plugin plugins[] = {
		//{_ICMPGetVersion, _ICMPTestAvailability, _ICMPStart, _ICMPStop},
		//{_UDPGetVersion,  _UDPTestAvailability,  _UDPStart,  _UDPStop},
		//{_TCPGetVersion,  _TCPTestAvailability,  _TCPStart,  _TCPStop},
		{_SCTPGetVersion,  _SCTPTestAvailability,  _SCTPStart,  _SCTPStop},
};

void muxStart(uint32_t endpoint, bool serverMode)
{
#pragma omp parallel num_threads(PLUGIN_COUNT)
#pragma omp single nowait
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
#pragma omp task
		{
			//debug print
			log_verbose("[LOG] %d, thread: %d, %s\n", i, omp_get_thread_num(), plugins[i].getVersion());

			plugins[i].start(endpoint, serverMode);
		}
	}
}


void muxStop()
{
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
		plugins[i].stop();
	}
}

void muxListPlugins()
{
	printf("There are a total of %zu plugins:\n", PLUGIN_COUNT);
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
		printf("\t%s\n", plugins[i].getVersion());
	}
}

void muxTestPlugins(uint32_t endpoint)
{
	printf("Testing availability of %zu plugins:\n", PLUGIN_COUNT);
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
		printf("Testing %s ... ", plugins[i].getVersion());

		bool available = plugins[i].testAvailability(endpoint);

		if (available)
		{
			printf("OK\n");
		}
		else
		{
			printf("unavailable\n");
		}
	}
}