#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"


void UDPServerInitialize(struct sockaddr_in *endpoint)
{
	struct sockaddr_in sock;

	// zero out the structure
	memset(&sock, 0, sizeof(sock));

	sock.sin_family = AF_INET;
	//sock.sin_port = htons(PORT);
	sock.sin_port = endpoint->sin_port;
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(pluginStateUDP.socket, (struct sockaddr*)(&sock), sizeof(sock)) < 0)
	{
		fprintf(stderr, "Unable to bind UDP socket to port %d\n", ntohs(endpoint->sin_port));
		_UDPStop();
	}
}

void UDPServerCheckHealth(struct sockaddr_in *endpoint)
{
	if (!pluginStateUDP.connected)
		return;

	if (pluginStateUDP.noReplyCount++ > UDP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginStateUDP.connected = false;
		return;
	}
}

void UDPServerUDPData(struct sockaddr_in *endpoint)
{
	UDPMessage msg;
	struct sockaddr_in sender;

	if (UDPReceiveMsg(pluginStateUDP.socket, &sender, &msg))
		return;

	if (pluginStateUDP.connected && !UDPequalSockaddr(&sender, pluginStateUDP.endpoint))
		return;

	//add port check ??

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case UDP_CONNECTION_REQUEST:
			UDPHandleConnectionRequest(pluginStateUDP.socket, &sender, &msg);
			break;
		case UDP_AUTH_RESPONSE:
			UDPHandleAuthResponse(pluginStateUDP.socket, &sender, &msg);
			break;
		case UDP_DATA:
			UDPHandleUDPData(&msg);
			break;
		case UDP_KEEPALIVE:
			UDPHandleKeepAlive(pluginStateUDP.socket, &sender, &msg);
			break;
	}
}

void UDPServerTunnelData(struct sockaddr_in *endpoint)
{
	UDPMessage msg;
	tunRead(tunDeviceFD, (char *) &(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.packetType = UDP_DATA;

	UDPSendMsg(pluginStateUDP.socket, pluginStateUDP.endpoint, &msg);
}