#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "sctp.h"

void SCTPSendConnectionRequest(int socketFD, struct sockaddr_in * endpoint)
{
	struct SCTPMessage msg;
	msg.size = 1;
	msg.packetType = SCTP_CONNECTION_REQUEST;

	SCTPSendControl(socketFD, &msg);
}

void SCTPSendKeepAlive(int socketFD, struct sockaddr_in * endpoint)
{
	struct SCTPMessage msg;
	msg.size = 1;
	msg.packetType = SCTP_KEEPALIVE;

	SCTPSendControl(socketFD, &msg);
}

void SCTPHandleConnectionAccept(struct sockaddr_in * endpoint)
{
	pluginState.auth = true;
}

void SCTPHandleAuthChallenge(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage *origMsg)
{
	if (origMsg->size != AUTH_CHALLENGE_LENGTH)
		return;

	struct SCTPMessage msg;

	struct auth_context auth;
	memcpy(auth.challenge, origMsg->buffer, AUTH_CHALLENGE_LENGTH);
	generateResponse(&auth);
	memcpy(msg.buffer, auth.response, AUTH_RESPONSE_LENGTH);

	msg.size = AUTH_RESPONSE_LENGTH;
	msg.packetType = SCTP_AUTH_RESPONSE;

	SCTPSendControl(socketFD, &msg);
}

void SCTPHandleConnectionReject(int socketFD, struct sockaddr_in * endpoint)
{
	pluginState.connected = false;
	_SCTPStop();
}

void SCTPHandleSCTPData(struct SCTPMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void SCTPHandleKeepAliveResponse()
{
	pluginState.noReplyCount = 0;
}