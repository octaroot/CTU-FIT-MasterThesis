#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "tcp.h"

void TCPSendConnectionRequest(int socketFD, struct sockaddr_in * endpoint)
{
	struct TCPMessage msg;
	msg.size = 1;
	msg.packetType = TCP_CONNECTION_REQUEST;

	TCPSendMsg(socketFD, endpoint, &msg);
}

void TCPSendKeepAlive(int socketFD, struct sockaddr_in * endpoint)
{
	struct TCPMessage msg;
	msg.size = 1;
	msg.packetType = TCP_KEEPALIVE;

	TCPSendMsg(socketFD, pluginState.endpoint, &msg);
}

void TCPHandleConnectionAccept(struct sockaddr_in * endpoint)
{
	pluginState.connected = true;
}

void TCPHandleAuthChallenge(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage *origMsg)
{
	if (origMsg->size != AUTH_CHALLENGE_LENGTH)
		return;

	struct TCPMessage msg;

	struct auth_context auth;
	memcpy(auth.challenge, origMsg->buffer, AUTH_CHALLENGE_LENGTH);
	generateResponse(&auth);
	memcpy(msg.buffer, auth.response, AUTH_RESPONSE_LENGTH);

	msg.size = AUTH_RESPONSE_LENGTH;
	msg.packetType = TCP_AUTH_RESPONSE;

	TCPSendMsg(socketFD, pluginState.endpoint, &msg);
}

void TCPHandleConnectionReject(int socketFD, struct sockaddr_in * endpoint)
{
	pluginState.connected = false;
	_TCPStop();
}

void TCPHandleTCPData(struct TCPMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void TCPHandleKeepAliveResponse()
{
	pluginState.noReplyCount = 0;
}