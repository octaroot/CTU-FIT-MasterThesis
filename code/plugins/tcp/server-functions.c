#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "tcp.h"
#include "../../src/auth.h"

struct auth_context * authCtxs[TCP_MAX_AUTH_REQUESTS];
int TCPauthCtxIdx = 0;

void TCPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage *request)
{
	struct TCPMessage msg;

	if (pluginState.auth)
	{
		msg.size = 1;
		msg.packetType = TCP_CONNECTION_REJECT;

		TCPSendMsg(socketFD, endpoint, &msg);
		return;
	}

	TCPauthCtxIdx = (TCPauthCtxIdx + 1) % TCP_MAX_AUTH_REQUESTS;

	if (authCtxs[TCPauthCtxIdx] != NULL)
	{
		free(authCtxs[TCPauthCtxIdx]);
	}

	authCtxs[TCPauthCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(authCtxs[TCPauthCtxIdx]);

	msg.packetType = TCP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, authCtxs[TCPauthCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	TCPSendMsg(socketFD, endpoint, &msg);
}

void TCPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage * request)
{
	struct TCPMessage msg;
	msg.size = 1;

	if (!pluginState.auth && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < TCP_MAX_AUTH_REQUESTS; ++i)
		{
			if (authCtxs[i] != NULL)
			{
				if (checkResponse(authCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = TCP_CONNECTION_ACCEPT;

					pluginState.auth = true;

					TCPSendMsg(socketFD, pluginState.endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < TCP_MAX_AUTH_REQUESTS; ++i)
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

	msg.packetType = TCP_CONNECTION_REJECT;

	TCPSendMsg(socketFD, endpoint, &msg);
}

void TCPHandleKeepAlive(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage * request)
{
	if (!pluginState.connected)
		return;

	if (pluginState.auth)
		pluginState.noReplyCount = 0;

	TCPSendMsg(socketFD, pluginState.endpoint, request);
}