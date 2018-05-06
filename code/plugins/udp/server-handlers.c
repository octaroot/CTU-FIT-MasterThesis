#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"


void UDPServerInitialize(struct sockaddr_in *endpoint, struct UDPPluginState * pluginState)
{
	struct sockaddr_in sock;

	// zero out the structure
	memset(&sock, 0, sizeof(sock));

	sock.sin_family = AF_INET;
	//sock.sin_port = htons(PORT);
	sock.sin_port = endpoint->sin_port;
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(pluginState->socket, (struct sockaddr*)(&sock), sizeof(sock)) < 0)
	{
		fprintf(stderr, "Unable to bind UDP socket to port %d\n", ntohs(endpoint->sin_port));
		_UDPStop();
	}
}

void UDPServerCheckHealth(struct UDPPluginState * pluginState)
{
	if (!pluginState->connected)
		return;

	if (pluginState->noReplyCount++ > UDP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginState->connected = false;
		return;
	}
}

void UDPServerUDPData(struct UDPPluginState * pluginState)
{
	UDPMessage msg;
	struct sockaddr_in sender;

	if (UDPReceiveMsg(pluginState->socket, &sender, &msg))
		return;

	if (pluginState->connected && !UDPequalSockaddr(&sender, pluginState->endpoint))
		return;

	//add port check ??

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case UDP_CONNECTION_REQUEST:
			UDPHandleConnectionRequest(pluginState, &sender, &msg);
			break;
		case UDP_AUTH_RESPONSE:
			UDPHandleAuthResponse(pluginState, &sender, &msg);
			break;
		case UDP_DATA:
			UDPHandleUDPData(&msg);
			break;
		case UDP_KEEPALIVE:
			UDPHandleKeepAlive(pluginState, &sender, &msg);
			break;
	}
}

void UDPServerTunnelData(struct UDPPluginState * pluginState)
{
	UDPMessage msg;
	tunRead(tunDeviceFD, (char *) &(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = UDP_DATA;

	UDPSendMsg(pluginState->socket, pluginState->endpoint, &msg);
}