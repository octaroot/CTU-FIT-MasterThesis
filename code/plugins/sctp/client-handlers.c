#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "sctp.h"


void SCTPClientInitialize(struct sockaddr_in * endpoint, struct SCTPPluginState * pluginStateSCTP)
{
	memcpy(pluginStateSCTP->endpoint, endpoint, sizeof(struct sockaddr_in));


	if (connect(pluginStateSCTP->listener, (struct sockaddr*) endpoint, sizeof(*endpoint)) < 0) {
		//fprintf(stderr,"Unable to connect to server (SCTP): %s\n", strerror(errno));
		_SCTPStop();
		return;
	}

	SCTPSetEvents(pluginStateSCTP->listener);

	pluginStateSCTP->socket = pluginStateSCTP->listener;

	SCTPSetInitMsg(pluginStateSCTP->socket);
	//SCTPSetEvents(pluginStateSCTP->socket);

	SCTPSendConnectionRequest(pluginStateSCTP);
}

void SCTPClientAcceptClient(struct SCTPPluginState * pluginStateSCTP)
{
	pluginStateSCTP->connected = true;
}

void SCTPClientCheckHealth(struct SCTPPluginState * pluginStateSCTP)
{
	if (!pluginStateSCTP->connected)
		return;

	if (pluginStateSCTP->noReplyCount++ > SCTP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_SCTPStop();
		return;
	}

	SCTPSendKeepAlive(pluginStateSCTP);
}

void SCTPClientSCTPData(struct SCTPPluginState * pluginStateSCTP)
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
		case SCTP_AUTH_CHALLENGE:
			SCTPHandleAuthChallenge(pluginStateSCTP, &msg);
			break;
		case SCTP_CONNECTION_ACCEPT:
			SCTPHandleConnectionAccept(pluginStateSCTP);
			break;
		case SCTP_CONNECTION_REJECT:
			SCTPHandleConnectionReject(pluginStateSCTP);
			break;
		case SCTP_KEEPALIVE:
			SCTPHandleKeepAliveResponse(pluginStateSCTP);
			break;
	}
}

void SCTPClientTunnelData(struct SCTPPluginState * pluginStateSCTP)
{
	SCTPMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	SCTPSendData(pluginStateSCTP, &msg);
}