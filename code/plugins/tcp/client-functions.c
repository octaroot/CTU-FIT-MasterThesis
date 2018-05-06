#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "tcp.h"

void TCPSendConnectionRequest(struct TCPPluginState * pluginStateTCP)
{
	struct TCPMessage msg;
	msg.size = 1;
	msg.packetType = TCP_CONNECTION_REQUEST;

	TCPSendMsg(pluginStateTCP, &msg);
}

void TCPSendKeepAlive(struct TCPPluginState * pluginStateTCP)
{
	struct TCPMessage msg;
	msg.size = 1;
	msg.packetType = TCP_KEEPALIVE;

	TCPSendMsg(pluginStateTCP, &msg);
}

void TCPHandleConnectionAccept(struct TCPPluginState * pluginStateTCP)
{
	pluginStateTCP->auth = true;
}

void TCPHandleAuthChallenge(struct TCPPluginState * pluginStateTCP, struct TCPMessage *origMsg)
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

	TCPSendMsg(pluginStateTCP, &msg);
}

void TCPHandleConnectionReject(struct TCPPluginState * pluginStateTCP)
{
	pluginStateTCP->connected = false;
	_TCPStop();
}

void TCPHandleTCPData(struct TCPMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void TCPHandleKeepAliveResponse(struct TCPPluginState * pluginStateTCP)
{
	pluginStateTCP->noReplyCount = 0;
}