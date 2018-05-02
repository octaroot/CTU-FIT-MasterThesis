#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"


void TCPServerInitialize(struct sockaddr_in *endpoint)
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
		fprintf(stderr, "Unable to bind TCP socket to port %d\n", ntohs(endpoint->sin_port));
		_TCPStop();
	}

	if (listen(pluginState.listener, 10) < 0)
	{
		fprintf(stderr, "Unable to listen on TCP socket: %s\n", strerror(errno));
		_TCPStop();
	}
}

void TCPServerAcceptClient()
{
	socklen_t endpointLen;
	memset(&pluginState.endpoint, 0, sizeof(pluginState.endpoint));
	if ((pluginState.socket = accept(pluginState.listener, (struct sockaddr*)pluginState.endpoint, &endpointLen)) < 0) {
		fprintf(stderr, "Unable to accept a TCP client: %s\n", strerror(errno));
		_TCPStop();
	}
}

void TCPServerCheckHealth(struct sockaddr_in *endpoint)
{
	if (!pluginState.connected)
		return;

	if (pluginState.noReplyCount++ > TCP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginState.connected = false;
		return;
	}
}

void TCPServerTCPData(struct sockaddr_in *endpoint)
{
	TCPMessage msg;
	struct sockaddr_in sender;

	if (TCPReceiveMsg(pluginState.socket, &sender, &msg))
		return;

	if (pluginState.connected && !TCPequalSockaddr(&sender, pluginState.endpoint))
		return;

	//add port check ??

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case TCP_CONNECTION_REQUEST:
			TCPHandleConnectionRequest(pluginState.socket, &sender, &msg);
			break;
		case TCP_AUTH_RESPONSE:
			TCPHandleAuthResponse(pluginState.socket, &sender, &msg);
			break;
		case TCP_DATA:
			TCPHandleTCPData(&msg);
			break;
		case TCP_KEEPALIVE:
			TCPHandleKeepAlive(pluginState.socket, &sender, &msg);
			break;
	}
}

void TCPServerTunnelData(struct sockaddr_in *endpoint)
{
	TCPMessage msg;
	tunRead(tunDeviceFD, (char *) &(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = TCP_DATA;

	TCPSendMsg(pluginState.socket, pluginState.endpoint, &msg);
}