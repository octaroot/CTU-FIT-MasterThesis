#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "server-functions.h"
#include "packet.h"
#include "icmp.h"
#include "../../src/auth.h"

uint16_t NATSequenceNumbers[ICMP_NAT_PACKET_COUNT];
int NATSequenceNumberIdx = 0;

struct auth_context * authCtxs[ICMP_MAX_AUTH_REQUESTS];
uint16_t authICMPIds[ICMP_MAX_AUTH_REQUESTS];
int authCtxIdx = 0;

void ICMPHandleConnectionRequest(int socketFD, uint32_t endpoint, struct ICMPEchoMessage *request)
{
	struct ICMPEchoMessage msg;
	msg.type = ICMP_ECHO_REPLY;
	msg.seq = request->seq;

	if (pluginState.connected)
	{
		msg.size = 1;
		msg.packetType = ICMP_CONNECTION_REJECT;

		uint16_t oldID = ICMPIDNumber;
		ICMPSendEcho(socketFD, endpoint, &msg);
		ICMPIDNumber = oldID;
		return;
	}

	authCtxIdx = (authCtxIdx + 1) % ICMP_MAX_AUTH_REQUESTS;

	if (authCtxs[authCtxIdx] != NULL)
	{
		free(authCtxs[authCtxIdx]);
	}

	authICMPIds[authCtxIdx] = request->id;
	authCtxs[authCtxIdx] = malloc(sizeof(struct auth_context));
	initializeContext(authCtxs[authCtxIdx]);

	msg.packetType = ICMP_AUTH_CHALLENGE;
	msg.size = AUTH_CHALLENGE_LENGTH;
	memcpy(msg.buffer, authCtxs[authCtxIdx]->challenge, AUTH_CHALLENGE_LENGTH);

	ICMPSequenceNumber = request->seq;
	ICMPIDNumber = request->id;

	ICMPSendEcho(socketFD, endpoint, &msg);
}

void ICMPHandleAuthResponse(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request)
{
	struct ICMPEchoMessage msg;
	msg.size = 1;
	msg.type = ICMP_ECHO_REPLY;
	msg.seq = request->seq;

	if (!pluginState.connected && request->size == AUTH_RESPONSE_LENGTH)
	{
		for (int i = 0; i < ICMP_MAX_AUTH_REQUESTS; ++i)
		{
			if (request->id == authICMPIds[i] && authCtxs[i] != NULL)
			{
				printf("found req in server memory\n");
				if (checkResponse(authCtxs[i], (unsigned char *) request->buffer, AUTH_RESPONSE_LENGTH))
				{
					msg.packetType = ICMP_CONNECTION_ACCEPT;

					ICMPSequenceNumber = request->seq;
					ICMPIDNumber = request->id;

					pluginState.connected = true;
					pluginState.endpoint = endpoint;

					ICMPSendEcho(socketFD, endpoint, &msg);

					// zruseni vsech ostatnich challenge-response pozadavku
					memset(authICMPIds, 0, ICMP_MAX_AUTH_REQUESTS);
					for (i = 0; i < ICMP_MAX_AUTH_REQUESTS; ++i)
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

	msg.packetType = ICMP_CONNECTION_REJECT;

	uint16_t oldID = ICMPIDNumber;
	ICMPSendEcho(socketFD, endpoint, &msg);
	ICMPIDNumber = oldID;
}

void ICMPHandleNATPacket(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request)
{
	if (!pluginState.connected || pluginState.endpoint != endpoint)
		return;

	NATSequenceNumbers[NATSequenceNumberIdx++] = request->seq;
	NATSequenceNumberIdx %= ICMP_NAT_PACKET_COUNT;
}

void ICMPHandleKeepAlive(int socketFD, uint32_t endpoint, struct ICMPEchoMessage * request)
{
	if (!pluginState.connected || pluginState.endpoint != endpoint)
		return;

	pluginState.noReplyCount = 0;

	request->type = ICMP_ECHO_REPLY;

	ICMPSendEcho(socketFD, endpoint, request);
}