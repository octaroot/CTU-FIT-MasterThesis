#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "udp.h"
#include "../../src/auth.h"

struct auth_context * authCtxs[UDP_MAX_AUTH_REQUESTS];
struct sockaddr_in UDPauthUDPIds[UDP_MAX_AUTH_REQUESTS];
int UDPauthCtxIdx = 0;

void UDPHandleConnectionRequest(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage *request)
{
	struct UDPMessage msg;

	if (pluginState.connected)
	{
		msg.size = 1;
		msg.packetType = UDP_CONNECTION_REJECT;

		UDPSendMsg(socketFD, endpoint, &msg);
		return;
	}

	UDPauthCtxIdx = (UDPauthCtxIdx + 1) % UDP_MAX_AUTH_REQUESTS;

	if (authCtxs[UDPauthCtxIdx] != NULL)
	{
		free(authCtxs[UDPauthCtxIdx]);
	}

	memcpy(&(UDPauthUDPIds[UDPauthCtxIdx]), endpoint, sizeof(struct sockaddr_in));

	authCtxs[UDPauthCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(authCtxs[UDPauthCtxIdx]);

	msg.packetType = UDP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, authCtxs[UDPauthCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	UDPSendMsg(socketFD, endpoint, &msg);
}

void UDPHandleAuthResponse(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage * request)
{
	struct UDPMessage msg;
	msg.size = 1;

	if (!pluginState.connected && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < UDP_MAX_AUTH_REQUESTS; ++i)
		{
			if (authCtxs[i] != NULL && equalSockaddr(endpoint, &(UDPauthUDPIds[i])))
			{
				if (checkResponse(authCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = UDP_CONNECTION_ACCEPT;

					pluginState.connected = true;
					memcpy(pluginState.endpoint, endpoint, sizeof(struct sockaddr_in));

					UDPSendMsg(socketFD, pluginState.endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < UDP_MAX_AUTH_REQUESTS; ++i)
					{
						memset(&(UDPauthUDPIds[i]), 0, sizeof(struct sockaddr_in));
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

	msg.packetType = UDP_CONNECTION_REJECT;

	UDPSendMsg(socketFD, endpoint, &msg);
}

void UDPHandleKeepAlive(int socketFD, struct sockaddr_in * endpoint, struct UDPMessage * request)
{
	if (!pluginState.connected || !equalSockaddr(endpoint, pluginState.endpoint))
		return;

	pluginState.noReplyCount = 0;

	UDPSendMsg(socketFD, pluginState.endpoint, request);
}