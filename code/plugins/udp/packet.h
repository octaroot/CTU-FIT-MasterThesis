#ifndef CODE_PACKET_H
#define CODE_PACKET_H

#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/ip_udp.h>

#include "../../src/tun-device.h"

#define UDP_SOCKET_MTU  (TUN_DEVICE_MTU + sizeof(struct UDPPacketHeader) + sizeof(struct iphdr) + sizeof(struct udphdr))

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

typedef struct UDPEchoMessage
{
	int size;
	UDP_TYPE type;
	UDP_PACKET_TYPE packetType;
	char buffer[TUN_DEVICE_MTU];
	uint16_t id;
	uint16_t seq;
} UDPEchoMessage;

#define UDP_PACKET_MAGIC "CVUT"


int UDPSocketOpen();

int UDPSendEcho(int socketFD, uint32_t to, struct UDPEchoMessage *msg);

int UDPReceiveEcho(int socketFD, uint32_t *from, struct UDPEchoMessage *msg);

void UDPSocketClose(int socketFD);

uint16_t UDPPacketChecksum(const char *buffer, int size);

#endif //CODE_PACKET_H
