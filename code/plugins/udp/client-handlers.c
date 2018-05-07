#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "udp.h"


void UDPClientInitialize(struct sockaddr_in * endpoint, struct UDPPluginState * pluginState)
{
	memcpy(pluginState->endpoint, endpoint, sizeof(struct sockaddr_in));
	UDPSendConnectionRequest(pluginState);
}

void UDPClientCheckHealth(struct UDPPluginState * pluginState)
{
	if (!pluginState->connected)
		return;

	if (pluginState->noReplyCount++ > UDP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_UDPStop();
		return;
	}

	UDPSendKeepAlive(pluginState);
}

void UDPClientUDPData(struct UDPPluginState * pluginState)
{
	UDPMessage msg;
	struct sockaddr_in sender;

	if (UDPReceiveMsg(pluginState->socket, &sender, &msg))
		return;

	if (pluginState->connected && !UDPequalSockaddr(&sender, pluginState->endpoint))
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case UDP_AUTH_CHALLENGE:
			UDPHandleAuthChallenge(pluginState, &msg);
			break;
		case UDP_CONNECTION_ACCEPT:
			UDPHandleConnectionAccept(pluginState);
			break;
		case UDP_CONNECTION_REJECT:
			UDPHandleConnectionReject(pluginState);
			break;
		case UDP_KEEPALIVE:
			UDPHandleKeepAliveResponse(pluginState);
			break;
		case UDP_DATA:
			UDPHandleUDPData(&msg);
			break;
	}
}

void UDPClientTunnelData(struct UDPPluginState * pluginState)
{
	UDPMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = UDP_DATA;

	UDPSendMsg(pluginState->socket, pluginState->endpoint, &msg);
}