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
	memcpy(pluginStateSCTP.endpoint, endpoint, sizeof(struct sockaddr_in));


	if (connect(pluginStateSCTP.listener, (struct sockaddr*) endpoint, sizeof(*endpoint)) < 0) {
		fprintf(stderr,"Unable to connect to server (SCTP): %s\n", strerror(errno));
		_SCTPStop();
		return;
	}

	SCTPSetEvents(pluginStateSCTP.listener);

	pluginStateSCTP.socket = pluginStateSCTP.listener;

	SCTPSetInitMsg(pluginStateSCTP.socket);
	//SCTPSetEvents(pluginStateSCTP.socket);

	SCTPSendConnectionRequest(pluginStateSCTP.socket, pluginStateSCTP.endpoint);
}

void SCTPClientAcceptClient()
{
	pluginStateSCTP.connected = true;
}

void SCTPClientCheckHealth(struct sockaddr_in * endpoint)
{
	if (!pluginStateSCTP.connected)
		return;

	if (pluginStateSCTP.noReplyCount++ > SCTP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_SCTPStop();
		return;
	}

	SCTPSendKeepAlive(pluginStateSCTP.socket, pluginStateSCTP.endpoint);
}

void SCTPClientSCTPData(struct sockaddr_in * endpoint)
{
	SCTPMessage msg;
	struct sockaddr_in sender;

	if (SCTPReceiveMsg(pluginStateSCTP.socket, &msg))
		return;


	if (!pluginStateSCTP.connected)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case SCTP_AUTH_CHALLENGE:
			SCTPHandleAuthChallenge(pluginStateSCTP.socket, &sender, &msg);
			break;
		case SCTP_CONNECTION_ACCEPT:
			SCTPHandleConnectionAccept(&sender);
			break;
		case SCTP_CONNECTION_REJECT:
			SCTPHandleConnectionReject(pluginStateSCTP.socket, &sender);
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

	SCTPSendData(pluginStateSCTP.socket, &msg);
}