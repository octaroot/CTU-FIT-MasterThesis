#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "sctp.h"
#include "../../src/auth.h"

struct auth_context * SCTPauthCtxs[SCTP_MAX_AUTH_REQUESTS];
int SCTPauthCtxIdx = 0;

void SCTPHandleConnectionRequest(struct SCTPPluginState* pluginStateSCTP)
{
	struct SCTPMessage msg;

	if (pluginStateSCTP->auth)
	{
		msg.size = 1;
		msg.packetType = SCTP_CONNECTION_REJECT;

		SCTPSendControl(pluginStateSCTP, &msg);
		return;
	}

	SCTPauthCtxIdx = (SCTPauthCtxIdx + 1) % SCTP_MAX_AUTH_REQUESTS;

	if (SCTPauthCtxs[SCTPauthCtxIdx] != NULL)
	{
		free(SCTPauthCtxs[SCTPauthCtxIdx]);
	}

	SCTPauthCtxs[SCTPauthCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(SCTPauthCtxs[SCTPauthCtxIdx]);

	msg.packetType = SCTP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, SCTPauthCtxs[SCTPauthCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	SCTPSendControl(pluginStateSCTP, &msg);
}

void SCTPHandleAuthResponse(struct SCTPPluginState* pluginStateSCTP, struct SCTPMessage * request)
{
	struct SCTPMessage msg;
	msg.size = 1;

	if (!pluginStateSCTP->auth && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < SCTP_MAX_AUTH_REQUESTS; ++i)
		{
			if (SCTPauthCtxs[i] != NULL)
			{
				if (checkResponse(SCTPauthCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = SCTP_CONNECTION_ACCEPT;

					pluginStateSCTP->auth = true;

					SCTPSendControl(pluginStateSCTP, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < SCTP_MAX_AUTH_REQUESTS; ++i)
					{
						if (SCTPauthCtxs[i])
						{
							free(SCTPauthCtxs[i]);
							SCTPauthCtxs[i] = NULL;
						}
					}

					return;
				}
			}
		}
	}

	msg.packetType = SCTP_CONNECTION_REJECT;

	SCTPSendControl(pluginStateSCTP, &msg);
}

void SCTPHandleKeepAlive(struct SCTPPluginState* pluginStateSCTP, struct SCTPMessage * request)
{
	if (!pluginStateSCTP->connected)
		return;

	if (pluginStateSCTP->auth)
		pluginStateSCTP->noReplyCount = 0;

	SCTPSendControl(pluginStateSCTP, request);
}