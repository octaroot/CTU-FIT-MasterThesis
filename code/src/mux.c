#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <strings.h>
#include "mux.h"

#include "../plugins/icmp/icmp.h"
#include "../plugins/udp/udp.h"
#include "../plugins/tcp/tcp.h"
#include "../plugins/sctp/sctp.h"
#include "common.h"
#include "plugin-parser.h"

#define PLUGIN_COUNT (sizeof(plugins) / sizeof(plugin))
#define PLUGIN_MAX_RESTART_COUNT	10

bool stop;

plugin plugins[] = {
		{_ICMPGetVersion, _ICMPGetName, _ICMPTestAvailability, _ICMPStart, _ICMPStop},
		{_UDPGetVersion, _UDPGetName,  _UDPTestAvailability,  _UDPStart,  _UDPStop},
		{_TCPGetVersion, _TCPGetName,  _TCPTestAvailability,  _TCPStart,  _TCPStop},
		{_SCTPGetVersion, _SCTPGetName,  _SCTPTestAvailability,  _SCTPStart,  _SCTPStop},
};

void muxStart(bool serverMode, uint32_t address, struct pluginOptions * requiredPlugins, int count)
{
	stop = false;
#pragma omp parallel num_threads(count)
#pragma omp single nowait
	for (int i = 0; i < count; ++i)
	{
		muxStartPlugin(serverMode, address, requiredPlugins[i].port, requiredPlugins[i].pluginName);
	}
}

void muxStartPlugin(bool serverMode, uint32_t address, int port, const char * pluginName)
{
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
		if (strcasecmp(pluginName, plugins[i].getName()) != 0)
		{
			continue;
		}

		#pragma omp task shared(stop)
		{
			int restarts = 0;
			log_verbose("[%s:%d] Starting worker thread (%s)\n", plugins[i].getName(), port, plugins[i].getVersion());
			while (!stop && restarts < PLUGIN_MAX_RESTART_COUNT)
			{
				//debug print
				plugins[i].start(address, port, serverMode);
				++restarts;
			}
		}
	}
}


void muxStop()
{
	stop = true;

	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
		plugins[i].stop();
	}
}

void muxListPlugins()
{
	printf("There are a total of %zu plugins:\nNAME\tVERSION\n", PLUGIN_COUNT);
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
		printf("%s\t%s\n", plugins[i].getName(), plugins[i].getVersion());
	}
}

void muxTestPlugins(uint32_t address, struct pluginOptions * requiredPlugins, int count)
{
	printf("Testing availability of %zu plugins:\n", PLUGIN_COUNT);
	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < PLUGIN_COUNT; ++j)
		{
			if (strcasecmp(requiredPlugins[i].pluginName, plugins[j].getName()) != 0)
			{
				continue;
			}

			printf("Testing %s:%d ", plugins[j].getVersion(), requiredPlugins[i].port);

			fflush(stdout);

			bool available = plugins[j].testAvailability(address, requiredPlugins[i].port);

			if (available)
			{
				printf(" OK\n");
			}
			else
			{
				printf(" unavailable or already connected\n");
			}

		}
	}
}