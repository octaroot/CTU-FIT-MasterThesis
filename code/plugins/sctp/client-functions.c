#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "client-functions.h"
#include "packet.h"
#include "../../src/auth.h"
#include "sctp.h"

void SCTPSendConnectionRequest(struct SCTPPluginState * pluginStateSCTP)
{
	struct SCTPMessage msg;
	msg.size = 1;
	msg.packetType = SCTP_CONNECTION_REQUEST;

	SCTPSendControl(pluginStateSCTP, &msg);
}

void SCTPSendKeepAlive(struct SCTPPluginState * pluginStateSCTP)
{
	struct SCTPMessage msg;
	msg.size = 1;
	msg.packetType = SCTP_KEEPALIVE;

	SCTPSendControl(pluginStateSCTP, &msg);
}

void SCTPHandleConnectionAccept(struct SCTPPluginState * pluginStateSCTP)
{
	pluginStateSCTP->auth = true;
}

void SCTPHandleAuthChallenge(struct SCTPPluginState * pluginStateSCTP, struct SCTPMessage *origMsg)
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

	SCTPSendControl(pluginStateSCTP, &msg);
}

void SCTPHandleConnectionReject(struct SCTPPluginState * pluginStateSCTP)
{
	pluginStateSCTP->connected = false;
	_SCTPStop();
}

void SCTPHandleKeepAliveResponse(struct SCTPPluginState * pluginStateSCTP)
{
	pluginStateSCTP->noReplyCount = 0;
}