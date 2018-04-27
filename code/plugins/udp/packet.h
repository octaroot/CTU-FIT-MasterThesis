#ifndef CODE_PACKET_H
#define CODE_PACKET_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "../../src/tun-device.h"

#define UDP_SOCKET_MTU  (TUN_DEVICE_MTU + sizeof(struct UDPPacketHeader))

typedef enum UDP_PACKET_TYPE
{
	UDP_CONNECTION_REQUEST,
	UDP_AUTH_CHALLENGE,
	UDP_AUTH_RESPONSE,
	UDP_CONNECTION_ACCEPT,
	UDP_CONNECTION_REJECT,
	UDP_KEEPALIVE,
	UDP_DATA

} UDP_PACKET_TYPE;

typedef struct UDPPacketHeader
{
	uint8_t magic[4];
	uint8_t type; /* UDP_PACKET_TYPE */
} UDPPacketHeader;

typedef struct UDPMessage
{
	int size;
	UDP_PACKET_TYPE packetType;
	char buffer[TUN_DEVICE_MTU];
} UDPMessage;

#define UDP_PACKET_MAGIC "CVUT"


int UDPSocketOpen();

int UDPSendMsg(int socketFD, struct sockaddr_in * to, struct UDPMessage *msg);

int UDPReceiveMsg(int socketFD, struct sockaddr_in *from, struct UDPMessage *msg);

void UDPSocketClose(int socketFD);

bool equalSockaddr(struct sockaddr_in * a, struct sockaddr_in * b);

#endif //CODE_PACKET_H
