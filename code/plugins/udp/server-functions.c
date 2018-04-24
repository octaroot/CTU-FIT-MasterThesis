#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "udp.h"
#include "../../src/auth.h"

uint16_t NATSequenceNumbers[UDP_NAT_PACKET_COUNT];
int NATSequenceNumberIdx = 0;

struct auth_context * authCtxs[UDP_MAX_AUTH_REQUESTS];
uint16_t authUDPIds[UDP_MAX_AUTH_REQUESTS];
int authCtxIdx = 0;

void UDPHandlConnectionRequest(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request)
{
	struct UDPEchoMessage msg;
	msg.type = UDP_ECHO_REPLY;
	msg.seq = request->seq;

	if (pluginState.connected)
	{
		msg.size = 1;
		msg.packetType = UDP_CONNECTION_REJECT;

		uint16_t oldID = UDPIDNumber;
		UDPSendEcho(socketFD, endpoint, &msg);
		UDPIDNumber = oldID;
		return;
	}

	authCtxIdx = (authCtxIdx + 1) % UDP_MAX_AUTH_REQUESTS;

	if (authCtxs[authCtxIdx] != NULL)
	{
		free(authCtxs[authCtxIdx]);
	}

	authUDPIds[authCtxIdx] = request->id;
	authCtxs[authCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(authCtxs[authCtxIdx]);

	msg.packetType = UDP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, authCtxs[authCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	UDPSequenceNumber = request->seq;
	UDPIDNumber = request->id;

	UDPSendEcho(socketFD, endpoint, &msg);
}

void UDPHandleAuthResponse(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request)
{
	struct UDPEchoMessage msg;
	msg.size = 1;
	msg.type = UDP_ECHO_REPLY;
	msg.seq = request->seq;

	if (!pluginState.connected && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < UDP_MAX_AUTH_REQUESTS; ++i)
		{
			if (request->id == authUDPIds[i] && authCtxs[i] != NULL)
			{
				printf("found req in server memory\n");
				if (checkResponse(authCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = UDP_CONNECTION_ACCEPT;

					UDPSequenceNumber = request->seq;
					UDPIDNumber = request->id;

					pluginState.connected = true;
					pluginState.endpoint = endpoint;

					UDPSendEcho(socketFD, endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					memset(authUDPIds, 0, UDP_MAX_AUTH_REQUESTS);
					for (i = 0; i < UDP_MAX_AUTH_REQUESTS; ++i)
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

	msg.packetType = UDP_CONNECTION_REJECT;

	uint16_t oldID = UDPIDNumber;
	UDPSendEcho(socketFD, endpoint, &msg);
	UDPIDNumber = oldID;
}

void UDPHandleNATPacket(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request)
{
	if (!pluginState.connected || pluginState.endpoint != endpoint)
		return;

	NATSequenceNumbers[NATSequenceNumberIdx++] = request->seq;
	NATSequenceNumberIdx %= UDP_NAT_PACKET_COUNT;
}

void UDPHandleKeepAlive(int socketFD, uint32_t endpoint, struct UDPEchoMessage * request)
{
	if (!pluginState.connected || pluginState.endpoint != endpoint)
		return;

	pluginState.noReplyCount = 0;

	request->type = UDP_ECHO_REPLY;

	UDPSendEcho(socketFD, endpoint, request);
}