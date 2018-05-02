#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include <unistd.h>
#include "../../src/tun-device.h"


void SCTPServerInitialize(struct sockaddr_in *endpoint)
{
	struct sockaddr_in sock;

	// zero out the structure
	memset(&sock, 0, sizeof(sock));

	sock.sin_family = AF_INET;
	//sock.sin_port = htons(PORT);
	sock.sin_port = endpoint->sin_port;
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(pluginState.listener, (struct sockaddr*)(&sock), sizeof(sock)) < 0)
	{
		fprintf(stderr, "Unable to bind SCTP socket to port %d\n", ntohs(endpoint->sin_port));
		_SCTPStop();
	}

	SCTPSetInitMsg(pluginState.listener);

	if (listen(pluginState.listener, 10) < 0)
	{
		fprintf(stderr, "Unable to listen on SCTP socket: %s\n", strerror(errno));
		_SCTPStop();
	}
}

void SCTPServerAcceptClient()
{
	struct timeval timeout;

	while (_SCTPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(pluginState.listener, &fs);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int lenAvailable = select(pluginState.listener + 1, &fs, NULL, NULL, &timeout);

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
			memset(&pluginState.endpoint, 0, sizeof(pluginState.endpoint));
			if ((pluginState.socket = accept(pluginState.listener, (struct sockaddr*)pluginState.endpoint, &endpointLen)) < 0) {
				fprintf(stderr, "Unable to accept a SCTP client: %s\n", strerror(errno));
				_SCTPStop();
			}

			pluginState.noReplyCount = 0;
			pluginState.auth = false;
			pluginState.connected = true;
			return;
		}
	}

}

void SCTPServerCheckHealth(struct sockaddr_in *endpoint)
{
	if (pluginState.noReplyCount++ > SCTP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginState.connected = false;
		pluginState.auth = false;
		close(pluginState.socket);
		return;
	}
}

void SCTPServerSCTPData(struct sockaddr_in *endpoint)
{
	SCTPMessage msg;
	struct sockaddr_in sender;

	if (SCTPReceiveMsg(pluginState.socket, &msg))
		return;
	fprintf(stderr, "prochozi zprava\n");

	if (!pluginState.connected)
		return;

	if (!msg.size)
		return;


	switch (msg.packetType)
	{
		case SCTP_CONNECTION_REQUEST:
			SCTPHandleConnectionRequest(pluginState.socket, &sender, &msg);
			break;
		case SCTP_AUTH_RESPONSE:
			SCTPHandleAuthResponse(pluginState.socket, &sender, &msg);
			break;
		case SCTP_DATA:
			SCTPHandleSCTPData(&msg);
			break;
		case SCTP_KEEPALIVE:
			SCTPHandleKeepAlive(pluginState.socket, &sender, &msg);
			break;
	}
}

void SCTPServerTunnelData(struct sockaddr_in *endpoint)
{
	SCTPMessage msg;
	tunRead(tunDeviceFD, (char *) &(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = SCTP_DATA;

	SCTPSendData(pluginState.socket, &msg);
}