#ifndef CODE_PACKET_H
#define CODE_PACKET_H

#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "../../src/tun-device.h"

#define ICMP_SOCKET_MTU  (TUN_DEVICE_MTU + sizeof(struct ICMPPacketHeader) + sizeof(struct iphdr) + sizeof(struct icmphdr))

typedef enum ICMP_PACKET_TYPE
{
	ICMP_NATPACKET,
	ICMP_CONNECTION_REQUEST,
	ICMP_CONNECTION_ACCEPT,
	ICMP_CONNECTION_REJECT,
	ICMP_KEEPALIVE,
	ICMP_DATA

} ICMP_PACKET_TYPE;

typedef enum ICMP_TYPE
{
	ICMP_ECHO_REQUEST = 0x08,
	ICMP_ECHO_REPLY = 0x0,

} ICMP_TYPE;

typedef struct ICMPPacketHeader
{
	uint8_t magic[4];
	uint8_t type; /* ICMP_PACKET_TYPE */
} ICMPPacketHeader;

typedef struct ICMPEchoMessage
{
	int size;
	ICMP_TYPE type;
	ICMP_PACKET_TYPE packetType;
	char buffer[TUN_DEVICE_MTU];
	uint16_t id;
	uint16_t seq;
} ICMPEchoMessage;

#define ICMP_PACKET_MAGIC "CVUT"


int ICMPSocketOpen();

int ICMPSendEcho(int socketFD, uint32_t to, struct ICMPEchoMessage *msg);

int ICMPReceiveEcho(int socketFD, uint32_t *from, struct ICMPEchoMessage *msg);

void ICMPSocketClose(int socketFD);

#endif //CODE_PACKET_H
