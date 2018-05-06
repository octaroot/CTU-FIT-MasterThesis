#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include <unistd.h>
#include "../../src/tun-device.h"


void SCTPServerInitialize(struct sockaddr_in *endpoint, struct SCTPPluginState * pluginStateSCTP)
{
	struct sockaddr_in sock;

	// zero out the structure
	memset(&sock, 0, sizeof(sock));

	sock.sin_family = AF_INET;
	//sock.sin_port = htons(PORT);
	sock.sin_port = endpoint->sin_port;
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(pluginStateSCTP->listener, (struct sockaddr*)(&sock), sizeof(sock)) < 0)
	{
		fprintf(stderr, "Unable to bind SCTP socket to port %d\n", ntohs(endpoint->sin_port));
		_SCTPStop();
	}
	SCTPSetEvents(pluginStateSCTP->listener);

	SCTPSetInitMsg(pluginStateSCTP->listener);

	if (listen(pluginStateSCTP->listener, 10) < 0)
	{
		fprintf(stderr, "Unable to listen on SCTP socket: %s\n", strerror(errno));
		_SCTPStop();
	}
}

void SCTPServerAcceptClient(struct SCTPPluginState * pluginStateSCTP)
{
	struct timeval timeout;

	while (_SCTPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(pluginStateSCTP->listener, &fs);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int lenAvailable = select(pluginStateSCTP->listener + 1, &fs, NULL, NULL, &timeout);

		if (lenAvailable < 0)
		{
			if (!_SCTPRunning)
			{
				_SCTPStop();
				return;
			}

			fprintf(stderr, "Unable to select() on SCTP listening socket: %s\n", strerror(errno));
			_SCTPStop();
			return;
		}

		if (lenAvailable > 0)
		{
			socklen_t endpointLen;
			memset(&pluginStateSCTP->endpoint, 0, sizeof(pluginStateSCTP->endpoint));
			if ((pluginStateSCTP->socket = accept(pluginStateSCTP->listener, (struct sockaddr*)pluginStateSCTP->endpoint, &endpointLen)) < 0) {
				fprintf(stderr, "Unable to accept a SCTP client: %s\n", strerror(errno));
				_SCTPStop();
			}

			pluginStateSCTP->noReplyCount = 0;
			pluginStateSCTP->auth = false;
			pluginStateSCTP->connected = true;
			return;
		}
	}

}

void SCTPServerCheckHealth(struct SCTPPluginState * pluginStateSCTP)
{
	if (pluginStateSCTP->noReplyCount++ > SCTP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginStateSCTP->connected = false;
		pluginStateSCTP->auth = false;
		close(pluginStateSCTP->socket);
		return;
	}
}

void SCTPServerSCTPData(struct SCTPPluginState * pluginStateSCTP)
{
	SCTPMessage msg;

	if (SCTPReceiveMsg(pluginStateSCTP, &msg))
		return;

	if (!pluginStateSCTP->connected)
		return;

	if (!msg.size)
		return;


	switch (msg.packetType)
	{
		case SCTP_CONNECTION_REQUEST:
			SCTPHandleConnectionRequest(pluginStateSCTP);
			break;
		case SCTP_AUTH_RESPONSE:
			SCTPHandleAuthResponse(pluginStateSCTP, &msg);
			break;
		case SCTP_KEEPALIVE:
			SCTPHandleKeepAlive(pluginStateSCTP, &msg);
			break;
	}
}

void SCTPServerTunnelData(struct SCTPPluginState * pluginStateSCTP)
{
	SCTPMessage msg;
	tunRead(tunDeviceFD, (char *) &(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	SCTPSendData(pluginStateSCTP, &msg);
}