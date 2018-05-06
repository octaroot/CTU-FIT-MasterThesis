#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "udp.h"

void UDPSendConnectionRequest(struct UDPPluginState * pluginState)
{
	struct UDPMessage msg;
	msg.size = 1;
	msg.packetType = UDP_CONNECTION_REQUEST;

	UDPSendMsg(pluginState->socket, pluginState->endpoint, &msg);
}

void UDPSendKeepAlive(struct UDPPluginState * pluginState)
{
	struct UDPMessage msg;
	msg.size = 1;
	msg.packetType = UDP_KEEPALIVE;

	UDPSendMsg(pluginState->socket, pluginState->endpoint, &msg);
}

void UDPHandleConnectionAccept(struct UDPPluginState * pluginState)
{
	pluginState->connected = true;
}

void UDPHandleAuthChallenge(struct UDPPluginState * pluginState, struct UDPMessage *origMsg)
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

	UDPSendMsg(pluginState->socket, pluginState->endpoint, &msg);
}

void UDPHandleConnectionReject(struct UDPPluginState * pluginState)
{
	pluginState->connected = false;
	_UDPStop();
}

void UDPHandleUDPData(struct UDPMessage *msg)
{
	if (msg->size <= 0)
		return;

	tunWrite(tunDeviceFD, msg->buffer, msg->size);
}

void UDPHandleKeepAliveResponse(struct UDPPluginState * pluginState)
{
	pluginState->noReplyCount = 0;
}