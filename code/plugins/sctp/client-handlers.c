#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "sctp.h"


void SCTPClientInitialize(struct sockaddr_in * endpoint)
{
	memcpy(pluginState.endpoint, endpoint, sizeof(struct sockaddr_in));


	if (connect(pluginState.listener, (struct sockaddr*) endpoint, sizeof(*endpoint)) < 0) {
		fprintf(stderr,"Unable to connect to server (SCTP): %s\n", strerror(errno));
		_SCTPStop();
		return;
	}

	SCTPSetEvents(pluginState.listener);

	pluginState.socket = pluginState.listener;

	SCTPSetInitMsg(pluginState.socket);
	//SCTPSetEvents(pluginState.socket);

	SCTPSendConnectionRequest(pluginState.socket, pluginState.endpoint);
}

void SCTPClientAcceptClient()
{
	pluginState.connected = true;
}

void SCTPClientCheckHealth(struct sockaddr_in * endpoint)
{
	if (!pluginState.connected)
		return;

	if (pluginState.noReplyCount++ > SCTP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_SCTPStop();
		return;
	}

	SCTPSendKeepAlive(pluginState.socket, pluginState.endpoint);
}

void SCTPClientSCTPData(struct sockaddr_in * endpoint)
{
	SCTPMessage msg;
	struct sockaddr_in sender;

	if (SCTPReceiveMsg(pluginState.socket, &msg))
		return;


	if (!pluginState.connected)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case SCTP_AUTH_CHALLENGE:
			SCTPHandleAuthChallenge(pluginState.socket, &sender, &msg);
			break;
		case SCTP_CONNECTION_ACCEPT:
			SCTPHandleConnectionAccept(&sender);
			break;
		case SCTP_CONNECTION_REJECT:
			SCTPHandleConnectionReject(pluginState.socket, &sender);
			break;
		case SCTP_KEEPALIVE:
			SCTPHandleKeepAliveResponse();
			break;
	}
}

void SCTPClientTunnelData(struct sockaddr_in * endpoint)
{
	SCTPMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	SCTPSendData(pluginState.socket, &msg);
}