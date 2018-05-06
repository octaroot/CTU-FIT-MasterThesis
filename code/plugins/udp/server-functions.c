#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "udp.h"
#include "../../src/auth.h"

struct auth_context * UDPauthCtxs[UDP_MAX_AUTH_REQUESTS];
struct sockaddr_in UDPauthUDPIds[UDP_MAX_AUTH_REQUESTS];
int UDPauthCtxIdx = 0;

void UDPHandleConnectionRequest(struct UDPPluginState * pluginState, struct sockaddr_in * endpoint, struct UDPMessage *request)
{
	struct UDPMessage msg;

	if (pluginState->connected)
	{
		msg.size = 1;
		msg.packetType = UDP_CONNECTION_REJECT;

		UDPSendMsg(pluginState->socket, endpoint, &msg);
		return;
	}

	UDPauthCtxIdx = (UDPauthCtxIdx + 1) % UDP_MAX_AUTH_REQUESTS;

	if (UDPauthCtxs[UDPauthCtxIdx] != NULL)
	{
		free(UDPauthCtxs[UDPauthCtxIdx]);
	}

	memcpy(&(UDPauthUDPIds[UDPauthCtxIdx]), endpoint, sizeof(struct sockaddr_in));

	UDPauthCtxs[UDPauthCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(UDPauthCtxs[UDPauthCtxIdx]);

	msg.packetType = UDP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, UDPauthCtxs[UDPauthCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	UDPSendMsg(pluginState->socket, endpoint, &msg);
}

void UDPHandleAuthResponse(struct UDPPluginState * pluginState, struct sockaddr_in * endpoint, struct UDPMessage * request)
{
	struct UDPMessage msg;
	msg.size = 1;

	if (!pluginState->connected && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < UDP_MAX_AUTH_REQUESTS; ++i)
		{
			if (UDPauthCtxs[i] != NULL && UDPequalSockaddr(endpoint, &(UDPauthUDPIds[i])))
			{
				if (checkResponse(UDPauthCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = UDP_CONNECTION_ACCEPT;

					pluginState->connected = true;
					memcpy(pluginState->endpoint, endpoint, sizeof(struct sockaddr_in));

					UDPSendMsg(pluginState->socket, pluginState->endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					for (i = 0; i < UDP_MAX_AUTH_REQUESTS; ++i)
					{
						memset(&(UDPauthUDPIds[i]), 0, sizeof(struct sockaddr_in));
						if (UDPauthCtxs[i])
						{
							free(UDPauthCtxs[i]);
							UDPauthCtxs[i] = NULL;
						}
					}

					return;
				}
			}
		}
	}

	msg.packetType = UDP_CONNECTION_REJECT;

	UDPSendMsg(pluginState->socket, endpoint, &msg);
}

void UDPHandleKeepAlive(struct UDPPluginState * pluginState, struct sockaddr_in * endpoint, struct UDPMessage * request)
{
	if (!pluginState->connected || !UDPequalSockaddr(endpoint, pluginState->endpoint))
		return;

	pluginState->noReplyCount = 0;

	UDPSendMsg(pluginState->socket, pluginState->endpoint, request);
}