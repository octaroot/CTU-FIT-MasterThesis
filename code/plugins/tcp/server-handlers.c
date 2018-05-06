#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include <unistd.h>
#include "../../src/tun-device.h"


void TCPServerInitialize(struct sockaddr_in *endpoint, struct TCPPluginState * pluginStateTCP)
{
	struct sockaddr_in sock;

	// zero out the structure
	memset(&sock, 0, sizeof(sock));

	sock.sin_family = AF_INET;
	//sock.sin_port = htons(PORT);
	sock.sin_port = endpoint->sin_port;
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(pluginStateTCP->listener, (struct sockaddr*)(&sock), sizeof(sock)) < 0)
	{
		fprintf(stderr, "Unable to bind TCP socket to port %d\n", ntohs(endpoint->sin_port));
		_TCPStop();
	}

	if (listen(pluginStateTCP->listener, 10) < 0)
	{
		fprintf(stderr, "Unable to listen on TCP socket: %s\n", strerror(errno));
		_TCPStop();
	}
}

void TCPServerAcceptClient(struct TCPPluginState * pluginStateTCP)
{
	struct timeval timeout;

	while (_TCPRunning)
	{
		fd_set fs;

		FD_ZERO(&fs);
		FD_SET(pluginStateTCP->listener, &fs);

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int lenAvailable = select(pluginStateTCP->listener + 1, &fs, NULL, NULL, &timeout);

		if (lenAvailable < 0)
		{
			if (!_TCPRunning)
			{
				_TCPStop();
				return;
			}

			fprintf(stderr, "Unable to select() on TCP listening socket: %s\n", strerror(errno));
			_TCPStop();
			return;
		}

		if (lenAvailable > 0)
		{
			socklen_t endpointLen;
			memset(&pluginStateTCP->endpoint, 0, sizeof(pluginStateTCP->endpoint));
			if ((pluginStateTCP->socket = accept(pluginStateTCP->listener, (struct sockaddr*)pluginStateTCP->endpoint, &endpointLen)) < 0) {
				fprintf(stderr, "Unable to accept a TCP client: %s\n", strerror(errno));
				_TCPStop();
			}

			pluginStateTCP->noReplyCount = 0;
			pluginStateTCP->auth = false;
			pluginStateTCP->connected = true;
			return;
		}
	}

}

void TCPServerCheckHealth(struct TCPPluginState * pluginStateTCP)
{
	if (pluginStateTCP->noReplyCount++ > TCP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginStateTCP->connected = false;
		pluginStateTCP->auth = false;
		close(pluginStateTCP->socket);
		return;
	}
}

void TCPServerTCPData(struct TCPPluginState * pluginStateTCP)
{
	TCPMessage msg;

	if (TCPReceiveMsg(pluginStateTCP, &msg))
		return;

	if (!pluginStateTCP->connected)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case TCP_CONNECTION_REQUEST:
			TCPHandleConnectionRequest(pluginStateTCP, &msg);
			break;
		case TCP_AUTH_RESPONSE:
			TCPHandleAuthResponse(pluginStateTCP, &msg);
			break;
		case TCP_DATA:
			TCPHandleTCPData(&msg);
			break;
		case TCP_KEEPALIVE:
			TCPHandleKeepAlive(pluginStateTCP, &msg);
			break;
	}
}

void TCPServerTunnelData(struct TCPPluginState * pluginStateTCP)
{
	TCPMessage msg;
	tunRead(tunDeviceFD, (char *) &(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = TCP_DATA;

	TCPSendMsg(pluginStateTCP, &msg);
}