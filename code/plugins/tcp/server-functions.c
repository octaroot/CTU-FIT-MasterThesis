#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "tcp.h"
#include "../../src/auth.h"

struct auth_context * authCtxs[TCP_MAX_AUTH_REQUESTS];
struct sockaddr_in TCPauthTCPIds[TCP_MAX_AUTH_REQUESTS];
int TCPauthCtxIdx = 0;

void TCPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage *request)
{
	struct TCPMessage msg;

	if (pluginState.connected)
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

	memcpy(&(TCPauthTCPIds[TCPauthCtxIdx]), endpoint, sizeof(struct sockaddr_in));

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

	if (!pluginState.connected && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < TCP_MAX_AUTH_REQUESTS; ++i)
		{
			if (authCtxs[i] != NULL && TCPequalSockaddr(endpoint, &(TCPauthTCPIds[i])))
			{
				if (checkResponse(authCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = TCP_CONNECTION_ACCEPT;

					pluginState.connected = true;
					memcpy(pluginState.endpoint, endpoint, sizeof(struct sockaddr_in));

					TCPSendMsg(socketFD, pluginState.endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < TCP_MAX_AUTH_REQUESTS; ++i)
					{
						memset(&(TCPauthTCPIds[i]), 0, sizeof(struct sockaddr_in));
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
	if (!pluginState.connected || !TCPequalSockaddr(endpoint, pluginState.endpoint))
		return;

	pluginState.noReplyCount = 0;

	TCPSendMsg(socketFD, pluginState.endpoint, request);
}