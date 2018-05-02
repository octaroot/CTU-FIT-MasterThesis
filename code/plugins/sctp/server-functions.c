#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "sctp.h"
#include "../../src/auth.h"

struct auth_context * authCtxs[SCTP_MAX_AUTH_REQUESTS];
int SCTPauthCtxIdx = 0;

void SCTPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage *request)
{
	struct SCTPMessage msg;

	if (pluginState.auth)
	{
		msg.size = 1;
		msg.packetType = SCTP_CONNECTION_REJECT;

		SCTPSendControl(socketFD, endpoint, &msg);
		return;
	}

	SCTPauthCtxIdx = (SCTPauthCtxIdx + 1) % SCTP_MAX_AUTH_REQUESTS;

	if (authCtxs[SCTPauthCtxIdx] != NULL)
	{
		free(authCtxs[SCTPauthCtxIdx]);
	}

	authCtxs[SCTPauthCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(authCtxs[SCTPauthCtxIdx]);

	msg.packetType = SCTP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, authCtxs[SCTPauthCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	SCTPSendControl(socketFD, endpoint, &msg);
}

void SCTPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage * request)
{
	struct SCTPMessage msg;
	msg.size = 1;

	if (!pluginState.auth && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < SCTP_MAX_AUTH_REQUESTS; ++i)
		{
			if (authCtxs[i] != NULL)
			{
				if (checkResponse(authCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = SCTP_CONNECTION_ACCEPT;

					pluginState.auth = true;

					SCTPSendControl(socketFD, pluginState.endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < SCTP_MAX_AUTH_REQUESTS; ++i)
					{
						if (authCtxs[i])
						{
							free(authCtxs[i]);
							authCtxs[i] = NULL;
						}
					}

					return;
				}
			}
		}
	}

	msg.packetType = SCTP_CONNECTION_REJECT;

	SCTPSendControl(socketFD, endpoint, &msg);
}

void SCTPHandleKeepAlive(int socketFD, struct sockaddr_in * endpoint, struct SCTPMessage * request)
{
	if (!pluginState.connected)
		return;

	if (pluginState.auth)
		pluginState.noReplyCount = 0;

	SCTPSendControl(socketFD, pluginState.endpoint, request);
}