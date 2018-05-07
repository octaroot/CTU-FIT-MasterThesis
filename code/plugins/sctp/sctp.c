#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

#include "sctp.h"
#include "packet.h"

#include "client-handlers.h"
#include "server-handlers.h"

bool _SCTPTestAvailability(uint32_t address, int port)
{
	struct SCTPPluginState pluginStateSCTP;

	pluginStateSCTP.noReplyCount = 0;
	pluginStateSCTP.connected = false;
	pluginStateSCTP.endpoint = malloc(sizeof(struct sockaddr_in));


	SCTPHandlers handlers[] = {
			{SCTPClientInitialize, SCTPClientAcceptClient, SCTPClientCheckHealth, SCTPClientSCTPData, SCTPClientTunnelData},
	};

	_SCTPRunning = true;

	pluginStateSCTP.listener = SCTPSocketOpen();

	if (!pluginStateSCTP.listener)
	{
		_SCTPRunning = false;
		return false;
	}

	SCTPHandlers *handler = &(handlers[0]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(port);

	handler->initialize(&endpoint, &pluginStateSCTP);

	while (_SCTPRunning && !pluginStateSCTP.auth && pluginStateSCTP.noReplyCount < SCTP_KEEPALIVE_TIMEOUT)
	{
		handler->acceptClient(&pluginStateSCTP);

		struct timeval timeout;

		while (_SCTPRunning && pluginStateSCTP.connected && !pluginStateSCTP.auth && pluginStateSCTP.noReplyCount < SCTP_KEEPALIVE_TIMEOUT)
		{
			fd_set fs;

			FD_ZERO(&fs);
			FD_SET(pluginStateSCTP.socket, &fs);

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			int lenAvailable = select(pluginStateSCTP.socket + 1, &fs, NULL, NULL, &timeout);

			if (lenAvailable < 0)
			{
				_SCTPStopClient(&pluginStateSCTP);
				_SCTPStop();
				break;
			}

			if (lenAvailable == 0)
			{
				printf(".");
				fflush(stdout);
				++pluginStateSCTP.noReplyCount;
				continue;
			}

			handler->SCTPData(&pluginStateSCTP);
		}
	}

	bool success = pluginStateSCTP.auth;

	_SCTPCleanup(&pluginStateSCTP);

	return success;
}

void _SCTPCleanup(struct SCTPPluginState* pluginStateSCTP)
{
	pluginStateSCTP->connected = false;
	pluginStateSCTP->noReplyCount = 0;
	SCTPSocketClose(pluginStateSCTP->socket);
	SCTPSocketClose(pluginStateSCTP->listener);
}

const char *_SCTPGetVersion()
{
	return SCTP_PLUGIN_VERSION;
}

const char *_SCTPGetName()
{
	return SCTP_PLUGIN_NAME;
}

void _SCTPStart(uint32_t address, int port,  bool serverMode)
{
	struct SCTPPluginState pluginStateSCTP;
	
	pluginStateSCTP.noReplyCount = 0;
	pluginStateSCTP.connected = false;
	pluginStateSCTP.endpoint = malloc(sizeof(struct sockaddr_in));


	SCTPHandlers handlers[] = {
			{SCTPClientInitialize, SCTPClientAcceptClient, SCTPClientCheckHealth, SCTPClientSCTPData, SCTPClientTunnelData},
			{SCTPServerInitialize, SCTPServerAcceptClient, SCTPServerCheckHealth, SCTPServerSCTPData, SCTPServerTunnelData},
	};

	_SCTPRunning = true;

	pluginStateSCTP.listener = SCTPSocketOpen();

	if (!pluginStateSCTP.listener)
	{
		_SCTPRunning = false;
		return;
	}

	SCTPHandlers *handler = &(handlers[serverMode]);

	struct sockaddr_in endpoint;
	memset(&endpoint, 0, sizeof(struct sockaddr_in));


	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = htonl(address);
	endpoint.sin_port = htons(port);

	handler->initialize(&endpoint, &pluginStateSCTP);

	while (_SCTPRunning)
	{
		handler->acceptClient(&pluginStateSCTP);

		int maxFD = MAX(pluginStateSCTP.socket, tunDeviceFD);
		struct timeval timeout;

		while (_SCTPRunning && pluginStateSCTP.connected)
		{
			fd_set fs;

			FD_ZERO(&fs);
			FD_SET(pluginStateSCTP.socket, &fs);

			if (pluginStateSCTP.auth)
				FD_SET(tunDeviceFD, &fs);

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			int lenAvailable = select(maxFD + 1, &fs, NULL, NULL, &timeout);

			if (lenAvailable < 0)
			{
				_SCTPStopClient(&pluginStateSCTP);
				_SCTPStop();
				break;
			}

			if (lenAvailable == 0)
			{
				handler->checkHealth(&pluginStateSCTP);
				continue;
			}

			if (FD_ISSET(tunDeviceFD, &fs))
			{
				handler->tunnelData(&pluginStateSCTP);
			}

			if (FD_ISSET(pluginStateSCTP.socket, &fs))
			{
				handler->SCTPData(&pluginStateSCTP);
			}
		}
	}
	_SCTPCleanup(&pluginStateSCTP);
}

void _SCTPStop()
{
	_SCTPRunning = false;
}

void _SCTPStopClient(struct SCTPPluginState* pluginStateSCTP)
{
	pluginStateSCTP->connected = false;
	pluginStateSCTP->auth = false;
	close(pluginStateSCTP->socket);
}