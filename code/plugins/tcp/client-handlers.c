#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "tcp.h"


void TCPClientInitialize(struct sockaddr_in * endpoint)
{
	memcpy(pluginState.endpoint, endpoint, sizeof(struct sockaddr_in));

	if (connect(pluginState.listener, (struct sockaddr*) endpoint, sizeof(*endpoint)) < 0) {
		fprintf(stderr,"Unable to connect to server (TCP): %s\n", strerror(errno));
		_TCPStop();
		return;
	}

	pluginState.socket = pluginState.listener;

	TCPSendConnectionRequest(pluginState.socket, pluginState.endpoint);
}

void TCPClientAcceptClient()
{
	pluginState.connected = true;
}

void TCPClientCheckHealth(struct sockaddr_in * endpoint)
{
	if (!pluginState.connected)
		return;

	if (pluginState.noReplyCount++ > TCP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_TCPStop();
		return;
	}

	TCPSendKeepAlive(pluginState.socket, pluginState.endpoint);
}

void TCPClientTCPData(struct sockaddr_in * endpoint)
{
	TCPMessage msg;
	struct sockaddr_in sender;

	if (TCPReceiveMsg(pluginState.socket, &sender, &msg))
		return;

	if (!pluginState.connected)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case TCP_AUTH_CHALLENGE:
			TCPHandleAuthChallenge(pluginState.socket, &sender, &msg);
			break;
		case TCP_CONNECTION_ACCEPT:
			TCPHandleConnectionAccept(&sender);
			break;
		case TCP_CONNECTION_REJECT:
			TCPHandleConnectionReject(pluginState.socket, &sender);
			break;
		case TCP_KEEPALIVE:
			TCPHandleKeepAliveResponse();
			break;
		case TCP_DATA:
			TCPHandleTCPData(&msg);
			break;
	}
}

void TCPClientTunnelData(struct sockaddr_in * endpoint)
{
	TCPMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = TCP_DATA;

	TCPSendMsg(pluginState.socket, pluginState.endpoint, &msg);
}