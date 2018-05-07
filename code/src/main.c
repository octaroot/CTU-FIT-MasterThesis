#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#include "common.h"
#include "tun-device.h"
#include "mux.h"
#include "resolve.h"
#include "keyfile.h"
#include "plugin-parser.h"

int tunDeviceFD;

static void printHelp(char *programName)
{
	fprintf(stderr, "%s v%s\n", programName, PROGRAM_VERSION);
	fprintf(stderr, "  -v                     print program version and exit\n");
	fprintf(stderr, "  -k                     keyfile filename (required for auth)\n");
	fprintf(stderr, "  -l                     list compiled plugins and exit\n");
	fprintf(stderr, "  -h                     print help and exit\n");
	fprintf(stderr, "  -t                     test client connectivity to server and exit\n");
	fprintf(stderr, "  -p [<name[:port]>,...] use plugin on port, use comma to specify more plugins\n");
	fprintf(stderr, "  -s                     run in server mode\n");
	fprintf(stderr, "  -c <server>            run in client mode, connect to server ip/hostname\n");
	exit(EXIT_SUCCESS);
}

static void printVersion()
{
	fprintf(stderr, "v%s, built on %s %s\n", PROGRAM_VERSION, __DATE__, __TIME__);
	exit(EXIT_SUCCESS);
}

static void handleSignal(int sig)
{
	fprintf(stderr, "Detected SIGTERM/SIGINT, closing connection\n");

	muxStop();
}

int main(int argc, char **argv)
{

	bool justTestConnectivity = false;
	bool serverMode = false;
	bool clientMode = false;
	char * keyfileFilename = NULL;
	int parameter;

	char *serverName = "localhost";
	uint32_t endpoint;

	struct pluginOptions plugins[MAX_PLUGINS];
	char * requiredPlugins = NULL;

	while ((parameter = getopt(argc, argv, "lvhtsc:k:p:")) != -1)
	{
		switch (parameter)
		{
			case 'l':
				muxListPlugins();
				exit(EXIT_SUCCESS);
			case 'v':
				printVersion();
				break;
			case 'h':
				printHelp(argv[0]);
				break;
			case 't':
				justTestConnectivity = true;
				break;
			case 'k':
				keyfileFilename = optarg;
				break;
			case 's':
				serverMode = true;
				break;
			case 'c':
				clientMode = true;
				serverName = optarg;
				break;
			case 'p':
				requiredPlugins = optarg;
				break;
			default:
				fprintf(stderr, "Unknown or missing operand. Refer to program help (-h)\n");
				exit(EXIT_FAILURE);
		}
	}


	if (optind < argc)
	{
		for (int i = optind; i < argc; ++i)
		{
			fprintf(stderr, "Unknown operand \"%s\". Refer to program help (-h)\n", argv[i]);
		}
		exit(EXIT_FAILURE);
	}

	if (requiredPlugins == NULL)
	{
		fprintf(stderr, "You need to specify at least one plugin, e.g. -p udp:4444\n");
		exit(EXIT_FAILURE);
	}

	int parsedPlugins = parsePluginOptions(requiredPlugins, plugins);

	if (parsedPlugins < 1)
	{
		fprintf(stderr, "You need to specify at least one plugin, e.g. -p udp:4444\n");
		exit(EXIT_FAILURE);
	}

	if (clientMode ^ serverMode == false)
	{
		fprintf(stderr, "Select either server (-s) mode or client (-c <server>) mode\n");
		exit(EXIT_FAILURE);
	}

	if (justTestConnectivity && !clientMode)
	{
		fprintf(stderr, "Test mode is only available in client (-c <server>) mode\n");
		exit(EXIT_FAILURE);
	}

	endpoint = resolve(serverName);

	if (!endpoint)
	{
		fprintf(stderr, "Unable to resolve domain name: %s\n", serverName);
		exit(EXIT_FAILURE);
	}

	if (keyfileFilename && !keyfileRead(keyfileFilename))
	{
		fprintf(stderr, "Keyfile specified, but couldn't be read\n");
		exit(EXIT_FAILURE);
	}

	if (geteuid() != 0)
	{
		fprintf(stderr, "This program must be run as root\n");
		exit(EXIT_FAILURE);
	}


	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);

	srand(time(NULL));

	if (justTestConnectivity)
	{
		printf("Testing connectivity options to %s ...\n", serverName);

		muxTestPlugins(endpoint, plugins, parsedPlugins);
		exit(EXIT_SUCCESS);
	}

	tunDeviceFD = tunOpen();

	if (serverMode)
	{
		printf("Running in server mode ...\n");
	}
	else
	{
		printf("Connecting to server %s ...\n", serverName);
	}

	muxStart(serverMode, endpoint, plugins, parsedPlugins);
	//muxStart(endpoint, serverMode);

	tunClose(tunDeviceFD);
	keyfileClose();

	exit(EXIT_SUCCESS);
}