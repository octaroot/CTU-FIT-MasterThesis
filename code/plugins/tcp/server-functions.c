#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "tcp.h"
#include "../../src/auth.h"

struct auth_context * TCPauthCtxs[TCP_MAX_AUTH_REQUESTS];
int TCPauthCtxIdx = 0;

void TCPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage *request)
{
	struct TCPMessage msg;

	if (pluginStateTCP.auth)
	{
		msg.size = 1;
		msg.packetType = TCP_CONNECTION_REJECT;

		TCPSendMsg(socketFD, endpoint, &msg);
		return;
	}

	TCPauthCtxIdx = (TCPauthCtxIdx + 1) % TCP_MAX_AUTH_REQUESTS;

	if (TCPauthCtxs[TCPauthCtxIdx] != NULL)
	{
		free(TCPauthCtxs[TCPauthCtxIdx]);
	}

	TCPauthCtxs[TCPauthCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(TCPauthCtxs[TCPauthCtxIdx]);

	msg.packetType = TCP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, TCPauthCtxs[TCPauthCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	TCPSendMsg(socketFD, endpoint, &msg);
}

void TCPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct TCPMessage * request)
{
	struct TCPMessage msg;
	msg.size = 1;

	if (!pluginStateTCP.auth && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < TCP_MAX_AUTH_REQUESTS; ++i)
		{
			if (TCPauthCtxs[i] != NULL)
			{
				if (checkResponse(TCPauthCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = TCP_CONNECTION_ACCEPT;

					pluginStateTCP.auth = true;

					TCPSendMsg(socketFD, pluginStateTCP.endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < TCP_MAX_AUTH_REQUESTS; ++i)
					{
						if (TCPauthCtxs[i])
						{
							free(TCPauthCtxs[i]);
							TCPauthCtxs[i] = NULL;
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
	if (!pluginStateTCP.connected)
		return;

	if (pluginStateTCP.auth)
		pluginStateTCP.noReplyCount = 0;

	TCPSendMsg(socketFD, pluginStateTCP.endpoint, request);
}