#include <stdlib.h>
#include <stdio.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"


void UDPServerInitialize(uint32_t endpoint)
{
	UDPSequenceNumber = 0;
	UDPIDNumber = 0;
}

void UDPServerCheckHealth(uint32_t endpoint)
{
	if (!pluginState.connected)
		return;

	if (pluginState.noReplyCount++ > UDP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		pluginState.connected = false;
		return;
	}
}

void UDPServerUDPData(uint32_t endpoint)
{
	UDPEchoMessage msg;
	uint32_t sender;

	if (UDPReceiveEcho(_UDPSocketFD, &sender, &msg))
		return;

	if (pluginState.connected && sender != pluginState.endpoint)
		return;

	if (msg.type != UDP_ECHO_REQUEST)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case UDP_CONNECTION_REQUEST:
			UDPHandlConnectionRequest(_UDPSocketFD, sender, &msg);
			break;
		case UDP_AUTH_RESPONSE:
			UDPHandleAuthResponse(_UDPSocketFD, sender, &msg);
			break;
		case UDP_DATA:
			UDPHandleUDPData(&msg);
			break;
		case UDP_NATPACKET:
			UDPHandleNATPacket(_UDPSocketFD, sender, &msg);
			break;
		case UDP_KEEPALIVE:
			UDPHandleKeepAlive(_UDPSocketFD, sender, &msg);
			break;
	}
}

void UDPServerTunnelData(uint32_t endpoint)
{
	UDPEchoMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.type = UDP_ECHO_REPLY;
	msg.packetType = UDP_DATA;
	msg.seq = NATSequenceNumbers[NATSequenceNumberIdx++];
	NATSequenceNumberIdx %= UDP_NAT_PACKET_COUNT;

	UDPSendEcho(_UDPSocketFD, pluginState.endpoint, &msg);
}