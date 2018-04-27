#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "udp.h"

void UDPSendConnectionRequest(int socketFD, struct sockaddr_in * endpoint)
{
	struct UDPMessage msg;
	msg.size = 1;
	msg.packetType = UDP_CONNECTION_REQUEST;

	UDPSendMsg(socketFD, endpoint, &msg);
}

void UDPSendKeepAlive(int socketFD, struct sockaddr_in * endpoint)
{
	struct UDPMessage msg;
	msg.size = 1;
	msg.packetType = UDP_KEEPALIVE;

	UDPSendMsg(socketFD, pluginState.endpoint, &msg);
}

void UDPHandleConnectionAccept(struct sockaddr_in * endpoint)
{
	pluginState.connected = true;
}

void UDPHandleAuthChallenge(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage *origMsg)
{
	if (origMsg->size != AUTH_CHALLENGE_LENGTH)
		return;

	struct UDPMessage msg;

	struct auth_context auth;
	memcpy(auth.challenge, origMsg->buffer, AUTH_CHALLENGE_LENGTH);
	generateResponse(&auth);
	memcpy(msg.buffer, auth.response, AUTH_RESPONSE_LENGTH);

	msg.size = AUTH_RESPONSE_LENGTH;
	msg.packetType = UDP_AUTH_RESPONSE;

	UDPSendMsg(socketFD, pluginState.endpoint, &msg);
}

void UDPHandleConnectionReject(int socketFD, struct sockaddr_in * endpoint)
{
	pluginState.connected = false;
	_UDPStop();
}

void UDPHandleUDPData(struct UDPMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void UDPHandleKeepAliveResponse()
{
	pluginState.noReplyCount = 0;
}