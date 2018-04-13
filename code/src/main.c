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

int tunDeviceFD;

static void pritnHelp(char *programName) {
    fprintf(stderr, "%s v%s\n", programName, PROGRAM_VERSION);
    fprintf(stderr, "  -v               print program version and exit\n");
    fprintf(stderr, "  -l               list compiled plugins and exit\n");
    fprintf(stderr, "  -h               print help and exit\n");
    fprintf(stderr, "  -t <server>      test connectivity to server and exit\n");
    fprintf(stderr, "  -s               run in server mode\n");
    fprintf(stderr, "  -c <server>      run in client mode, connect to server ip/hostname\n");
    exit(EXIT_SUCCESS);
}

static void printVersion() {
    fprintf(stderr, "v%s, built on %s %s\n", PROGRAM_VERSION, __DATE__, __TIME__);
    exit(EXIT_SUCCESS);
}

static void handleSignal(int sig) {
    fprintf(stderr, "Detected SIGTERM/SIGINT, closing connection\n");

    mux_stop();
}

int main(int argc, char **argv) {

    bool justTestConnectivity = false;
    bool serverMode = false;
    bool clientMode = false;
    int parameter;

    char *serverName = NULL;
    uint32_t endpoint;

    while ((parameter = getopt(argc, argv, "lvht:sc:")) != -1) {
        switch (parameter) {
            case 'l':
                mux_list();
                exit(EXIT_SUCCESS);
            case 'v':
                printVersion();
                break;
            case 'h':
                pritnHelp(argv[0]);
                break;
            case 't':
                justTestConnectivity = true;
                serverName = optarg;
                break;
            case 's':
                serverMode = true;
                break;
            case 'c':
                clientMode = true;
                serverName = optarg;
                break;
            default:
                fprintf(stderr, "Unknown or missing operand. Refer to program help (-h)\n");
                exit(EXIT_FAILURE);
        }
    }


    if (optind < argc) {
        for (int i = optind; i < argc; ++i) {
            fprintf(stderr, "Unknown operand \"%s\". Refer to program help (-h)\n", argv[i]);
        }
        exit(EXIT_FAILURE);
    }

    if (clientMode ^ serverMode == false && !justTestConnectivity) {
        fprintf(stderr, "Select either server (-s) mode or client (-c <server>) mode\n");
        exit(EXIT_FAILURE);
    }

	endpoint = resolve(serverName);

	if (!endpoint)
	{
		fprintf(stderr, "Unable to resolve domain name: %s\n", serverName);
		exit(EXIT_FAILURE);
	}

    if (geteuid() != 0) {
        fprintf(stderr, "This program must be run as root\n");
        exit(EXIT_FAILURE);
    }


    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    srand(time(NULL));

    if (justTestConnectivity) {
        printf("Testing connectivity options to %s ...\n", serverName);

        mux_test(endpoint);
        exit(EXIT_SUCCESS);
    }

    tunDeviceFD = tunOpen();

    if (serverMode) {
        printf("Running in server mode ...\n");
        //TODO
    } else {
        printf("Connecting to server %s ...\n", serverName);

        //TODO
    }

    tunClose(tunDeviceFD);

    exit(EXIT_SUCCESS);
}