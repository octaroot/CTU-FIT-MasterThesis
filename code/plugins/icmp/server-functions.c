#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "icmp.h"

uint16_t NATSequenceNumbers[ICMP_NAT_PACKET_COUNT];
int NATSequenceNumberIdx = 0;

void ICMPHandlConnectionRequest(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request)
{
	struct ICMPEchoMessage msg;
	msg.size = 1;
	msg.type = ICMP_ECHO_REPLY;
	msg.seq = request->seq;

	if (pluginState.connected)
	{
		msg.packetType = ICMP_CONNECTION_REJECT;

		uint16_t oldID = ICMPIDNumber;
		ICMPSendEcho(socketFD, endpoint, &msg);
		ICMPIDNumber = oldID;
	}
	else
	{
		msg.packetType = ICMP_CONNECTION_ACCEPT;

		ICMPSequenceNumber = request->seq;
		ICMPIDNumber = request->id;

		pluginState.connected = true;
		pluginState.endpoint = endpoint;
	}

	msg.seq = ICMPSequenceNumber++;


}

void ICMPHandleNATPacket(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request)
{
	if (!pluginState.connected || pluginState.endpoint != endpoint)
		return;

	NATSequenceNumbers[NATSequenceNumberIdx++] = request->seq;
	NATSequenceNumberIdx %= ICMP_NAT_PACKET_COUNT;
}

void ICMPHandleKeepAlive(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request)
{
	if (!pluginState.connected || pluginState.endpoint != endpoint)
		return;

	request->type = ICMP_ECHO_REPLY;

	ICMPSendEcho(socketFD, endpoint, request);
}