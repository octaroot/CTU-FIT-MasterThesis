#ifndef CODE_PACKET_H
#define CODE_PACKET_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "../../src/tun-device.h"
#include "tcp.h"

#define TCP_SOCKET_MTU  (4096 + sizeof(struct TCPPacketHeader))

typedef enum TCP_PACKET_TYPE
{
	TCP_CONNECTION_REQUEST,
	TCP_AUTH_CHALLENGE,
	TCP_AUTH_RESPONSE,
	TCP_CONNECTION_ACCEPT,
	TCP_CONNECTION_REJECT,
	TCP_KEEPALIVE,
	TCP_DATA

} TCP_PACKET_TYPE;

typedef struct TCPPacketHeader
{
	uint8_t type; /* TCP_PACKET_TYPE */
	uint16_t length;
} TCPPacketHeader;

typedef struct TCPMessage
{
	int size;
	TCP_PACKET_TYPE packetType;
	char buffer[TCP_SOCKET_MTU];
} TCPMessage;


int TCPSocketOpen();

int TCPSendMsg(struct TCPPluginState * pluginStateTCP, struct TCPMessage *msg);

int TCPReceiveMsg(struct TCPPluginState * pluginStateTCP, struct TCPMessage *msg);

void TCPSocketClose(int socketFD);

#endif //CODE_PACKET_H
