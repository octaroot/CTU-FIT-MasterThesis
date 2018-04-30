#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdbool.h>

#include "packet.h"

int UDPReceiveMsg(int socketFD, struct sockaddr_in *from, struct UDPMessage *msg)
{
	char buffer[UDP_SOCKET_MTU];

	socklen_t serverSize = sizeof(*from);

	int receivedSize = recvfrom(socketFD, buffer, UDP_SOCKET_MTU, 0, (struct sockaddr*)from, &serverSize);

	if (receivedSize < 0)
	{
		fprintf(stderr, "Unable to receive an UDP packet: %s\n", strerror(errno));
		return 1;
	}

	if (receivedSize < (int) sizeof(struct UDPPacketHeader))
	{
		fprintf(stderr, "Received malformed UDP packet: %s\n", strerror(errno));
		return 1;
	}

	struct UDPPacketHeader *customHeader = (struct UDPPacketHeader *) buffer;

	if (memcmp(customHeader->magic, UDP_PACKET_MAGIC, sizeof(customHeader->magic)) != 0)
		return 1;

	int offset = sizeof(struct UDPPacketHeader);

	msg->size = receivedSize - offset;
	msg->packetType = customHeader->type;
	memcpy(msg->buffer, buffer + offset, receivedSize - offset);

	return 0;
}

void UDPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int UDPSendMsg(int socketFD, struct sockaddr_in * to, struct UDPMessage *msg)
{
	char buffer[UDP_SOCKET_MTU];

	struct UDPPacketHeader *customHeader = (struct UDPPacketHeader *) (buffer);
	memcpy(customHeader->magic, UDP_PACKET_MAGIC, sizeof(customHeader->magic));
	customHeader->type = msg->packetType;

	memcpy(buffer + sizeof(struct UDPPacketHeader), msg->buffer, msg->size);

	int sentSize = sendto(socketFD, buffer, msg->size + sizeof(struct UDPPacketHeader), 0, (struct sockaddr*)to, sizeof(*to));

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an UDP packet: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int UDPSocketOpen()
{
	int UDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (UDPSocket < 0)
	{
		fprintf(stderr, "Unable to open an UDP socket: %s\n", strerror(errno));
		return 1;
	}

	return UDPSocket;
}

bool UDPequalSockaddr(struct sockaddr_in * a, struct sockaddr_in * b)
{
	return a && b && a->sin_port == b->sin_port && a->sin_addr.s_addr == b->sin_addr.s_addr;
}