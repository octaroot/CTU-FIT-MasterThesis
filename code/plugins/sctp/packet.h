#ifndef CODE_PACKET_H
#define CODE_PACKET_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/ip.h>
#include <netinet/sctp.h>

#include "../../src/tun-device.h"
#include "sctp.h"

#define SCTP_SOCKET_MTU  (4096 + sizeof(struct SCTPPacketHeader))

typedef enum SCTP_PACKET_TYPE
{
	SCTP_CONNECTION_REQUEST,
	SCTP_AUTH_CHALLENGE,
	SCTP_AUTH_RESPONSE,
	SCTP_CONNECTION_ACCEPT,
	SCTP_CONNECTION_REJECT,
	SCTP_KEEPALIVE

} SCTP_PACKET_TYPE;

typedef struct SCTPPacketHeader
{
	uint8_t type; /* SCTP_PACKET_TYPE */
	uint16_t length;
} SCTPPacketHeader;

typedef struct SCTPMessage
{
	int size;
	SCTP_PACKET_TYPE packetType;
	char buffer[SCTP_SOCKET_MTU];
} SCTPMessage;


int SCTPSocketOpen();

void SCTPSetInitMsg(int socketFD);

void SCTPSetEvents(int socketFD);

int SCTPSendControl(struct SCTPPluginState * pluginState, struct SCTPMessage *msg);

int SCTPSendData(struct SCTPPluginState * pluginState, struct SCTPMessage *msg);

int SCTPReceiveMsg(struct SCTPPluginState * pluginState, struct SCTPMessage *msg);

void SCTPSocketClose(int socketFD);

#endif //CODE_PACKET_H
