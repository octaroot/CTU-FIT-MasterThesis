#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "tcp.h"


void TCPClientInitialize(struct sockaddr_in * endpoint, struct TCPPluginState * pluginStateTCP)
{
	memcpy(pluginStateTCP->endpoint, endpoint, sizeof(struct sockaddr_in));

	if (connect(pluginStateTCP->listener, (struct sockaddr*) endpoint, sizeof(*endpoint)) < 0) {
		fprintf(stderr,"Unable to connect to server (TCP): %s\n", strerror(errno));
		_TCPStop();
		return;
	}

	pluginStateTCP->socket = pluginStateTCP->listener;

	TCPSendConnectionRequest(pluginStateTCP);
}

void TCPClientAcceptClient(struct TCPPluginState * pluginStateTCP)
{
	pluginStateTCP->connected = true;
}

void TCPClientCheckHealth(struct TCPPluginState * pluginStateTCP)
{
	if (!pluginStateTCP->connected)
		return;

	if (pluginStateTCP->noReplyCount++ > TCP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_TCPStop();
		return;
	}

	TCPSendKeepAlive(pluginStateTCP);
}

void TCPClientTCPData(struct TCPPluginState * pluginStateTCP)
{
	TCPMessage msg;
	struct sockaddr_in sender;

	if (TCPReceiveMsg(pluginStateTCP, &msg))
		return;

	if (!pluginStateTCP->connected)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case TCP_AUTH_CHALLENGE:
			TCPHandleAuthChallenge(pluginStateTCP, &msg);
			break;
		case TCP_CONNECTION_ACCEPT:
			TCPHandleConnectionAccept(pluginStateTCP);
			break;
		case TCP_CONNECTION_REJECT:
			TCPHandleConnectionReject(pluginStateTCP);
			break;
		case TCP_KEEPALIVE:
			TCPHandleKeepAliveResponse(pluginStateTCP);
			break;
		case TCP_DATA:
			TCPHandleTCPData(&msg);
			break;
	}
}

void TCPClientTunnelData(struct TCPPluginState * pluginStateTCP)
{
	TCPMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = TCP_DATA;

	TCPSendMsg(pluginStateTCP,&msg);
}