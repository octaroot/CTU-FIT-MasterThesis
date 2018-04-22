#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
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

void ICMPHandleAuthChallenge(int socketFD, uint32_t endpoint, struct ICMPEchoMessage *origMsg)
{
	if (origMsg->size != AUTH_CHALLENGE_LENGTH)
		return;

	struct ICMPEchoMessage msg;

	struct auth_context auth;
	memcpy(auth.challenge, origMsg->buffer, AUTH_CHALLENGE_LENGTH);
	generateResponse(&auth);
	memcpy(msg.buffer, auth.response, AUTH_RESPONSE_LENGTH);

	msg.size = AUTH_RESPONSE_LENGTH;
	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_AUTH_RESPONSE;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(socketFD, endpoint, &msg);
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
	pluginState.noReplyCount = 0;
}