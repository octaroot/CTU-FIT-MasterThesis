#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "udp.h"


void UDPClientInitialize(struct sockaddr_in * endpoint)
{
	memcpy(pluginStateUDP.endpoint, endpoint, sizeof(struct sockaddr_in));
	UDPSendConnectionRequest(pluginStateUDP.socket, pluginStateUDP.endpoint);
}

void UDPClientCheckHealth(struct sockaddr_in * endpoint)
{
	if (!pluginStateUDP.connected)
		return;

	if (pluginStateUDP.noReplyCount++ > UDP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_UDPStop();
		return;
	}

	UDPSendKeepAlive(pluginStateUDP.socket, pluginStateUDP.endpoint);
}

void UDPClientUDPData(struct sockaddr_in * endpoint)
{
	UDPMessage msg;
	struct sockaddr_in sender;

	if (UDPReceiveMsg(pluginStateUDP.socket, &sender, &msg))
		return;

	if (pluginStateUDP.connected && !UDPequalSockaddr(&sender, pluginStateUDP.endpoint))
		return;

	//TODO: add port check??

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case UDP_AUTH_CHALLENGE:
			UDPHandleAuthChallenge(pluginStateUDP.socket, &sender, &msg);
			break;
		case UDP_CONNECTION_ACCEPT:
			UDPHandleConnectionAccept(&sender);
			break;
		case UDP_CONNECTION_REJECT:
			UDPHandleConnectionReject(pluginStateUDP.socket, &sender);
			break;
		case UDP_KEEPALIVE:
			UDPHandleKeepAliveResponse();
			break;
		case UDP_DATA:
			UDPHandleUDPData(&msg);
			break;
	}
}

void UDPClientTunnelData(struct sockaddr_in * endpoint)
{
	UDPMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = UDP_DATA;

	UDPSendMsg(pluginStateUDP.socket, pluginStateUDP.endpoint, &msg);
}