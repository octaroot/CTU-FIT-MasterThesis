#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "udp.h"

void UDPSendConnectionRequest(int socketFD, uint32_t endpoint)
{
	struct UDPEchoMessage msg;
	msg.size = 1;
	msg.type = UDP_ECHO_REQUEST;
	msg.packetType = UDP_CONNECTION_REQUEST;
	msg.seq = UDPSequenceNumber++;

	UDPSendEcho(socketFD, endpoint, &msg);
}

void UDPSendNATPacket(int socketFD, uint32_t endpoint)
{
	struct UDPEchoMessage msg;
	msg.size = 1;
	msg.type = UDP_ECHO_REQUEST;
	msg.packetType = UDP_NATPACKET;
	msg.seq = UDPSequenceNumber++;

	UDPSendEcho(socketFD, endpoint, &msg);
}

void UDPSendKeepAlive(int socketFD, uint32_t endpoint)
{
	struct UDPEchoMessage msg;
	msg.size = 1;
	msg.type = UDP_ECHO_REQUEST;
	msg.packetType = UDP_KEEPALIVE;
	msg.seq = UDPSequenceNumber++;

	UDPSendEcho(socketFD, endpoint, &msg);
}

void UDPHandleConnectionAccept(int socketFD, uint32_t endpoint)
{
	pluginState.connected = true;
	pluginState.endpoint = endpoint;
	for (int i = 0; i < UDP_NAT_PACKET_COUNT; ++i)
	{
		UDPSendNATPacket(socketFD, endpoint);
	}
}

void UDPHandleAuthChallenge(int socketFD, uint32_t endpoint, struct UDPEchoMessage *origMsg)
{
	if (origMsg->size != AUTH_CHALLENGE_LENGTH)
		return;

	struct UDPEchoMessage msg;

	struct auth_context auth;
	memcpy(auth.challenge, origMsg->buffer, AUTH_CHALLENGE_LENGTH);
	generateResponse(&auth);
	memcpy(msg.buffer, auth.response, AUTH_RESPONSE_LENGTH);

	msg.size = AUTH_RESPONSE_LENGTH;
	msg.type = UDP_ECHO_REQUEST;
	msg.packetType = UDP_AUTH_RESPONSE;
	msg.seq = UDPSequenceNumber++;

	UDPSendEcho(socketFD, endpoint, &msg);
}

void UDPHandleConnectionReject(int socketFD, uint32_t endpoint)
{
	pluginState.connected = false;
	_UDPStop();
}

void UDPHandleUDPData(struct UDPEchoMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void UDPHandleKeepAliveResponse(int socketFD, uint32_t endpoint)
{
	pluginState.noReplyCount = 0;
}