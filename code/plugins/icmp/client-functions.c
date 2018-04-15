#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "icmp.h"

void ICMPSendConnectionRequest(int socketFD, uint32_t endpoint)
{
	struct ICMPEchoMessage msg;
	msg.size = 1;
	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_CONNECTION_REQUEST;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(socketFD, endpoint, &msg);
}

void ICMPSendNATPacket(int socketFD, uint32_t endpoint)
{
	struct ICMPEchoMessage msg;
	msg.size = 1;
	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_NATPACKET;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(socketFD, endpoint, &msg);
}

void ICMPSendKeepAlive(int socketFD, uint32_t endpoint)
{
	struct ICMPEchoMessage msg;
	msg.size = 1;
	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_KEEPALIVE;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(socketFD, endpoint, &msg);
}

void ICMPHandleConnectionAccept(int socketFD, uint32_t endpoint)
{
	pluginState.connected = true;
	pluginState.endpoint = endpoint;
	for (int i = 0; i < ICMP_NAT_PACKET_COUNT; ++i)
	{
		ICMPSendNATPacket(socketFD, endpoint);
	}
}

void ICMPHandleConnectionReject(int socketFD, uint32_t endpoint)
{
	pluginState.connected = false;
	_ICMPStop();
}

void ICMPHandleICMPData(struct ICMPEchoMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void ICMPHandleKeepAliveResponse(int socketFD, uint32_t endpoint)
{
	// TODO
}