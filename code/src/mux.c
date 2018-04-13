//tady budou nejak zadratovany odkazy na jednotlive pluginy, takovy "interface" v C.
//taky se tu budou resit vlakna

#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include "mux.h"

#include "../plugins/icmp/icmp.h"
#include "../plugins/dns/dns.h"

#define PLUGIN_COUNT (sizeof(plugins) / sizeof(plugin))

plugin plugins[] = {
		{_ICMPGetVersion, _ICMPTestAvailability, _ICMPStart, _ICMPStop},
		{_DNSGetVersion,  _DNSTestAvailability,  _DNSStart,  _DNSStop},
};

void mux_start()
{
#pragma omp parallel num_threads(PLUGIN_COUNT)
#pragma omp single nowait
	for (int i = 0; i < PLUGIN_COUNT; ++i)
	{
#pragma omp task
		{

			//debug print
			printf("%d, thread: %d, %s\n", i, omp_get_thread_num(), plugins[i].getVersion());
		};
	}
}


void mux_stop()
{

}