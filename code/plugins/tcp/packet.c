#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdbool.h>

#include "packet.h"

int TCPReceiveMsg(int socketFD, struct sockaddr_in *from, struct TCPMessage *msg)
{
	char buffer[TCP_SOCKET_MTU];

	socklen_t serverSize = sizeof(*from);

	int receivedSize = recvfrom(socketFD, buffer, TCP_SOCKET_MTU, 0, (struct sockaddr*)from, &serverSize);

	if (receivedSize < 0)
	{
		fprintf(stderr, "Unable to receive an TCP packet: %s\n", strerror(errno));
		return 1;
	}

	if (receivedSize < (int) sizeof(struct TCPPacketHeader))
	{
		fprintf(stderr, "Received malformed TCP packet: %s\n", strerror(errno));
		return 1;
	}

	struct TCPPacketHeader *customHeader = (struct TCPPacketHeader *) buffer;

	if (memcmp(customHeader->magic, TCP_PACKET_MAGIC, sizeof(customHeader->magic)) != 0)
		return 1;

	int offset = sizeof(struct TCPPacketHeader);

	msg->size = receivedSize - offset;
	msg->packetType = customHeader->type;
	memcpy(msg->buffer, buffer + offset, receivedSize - offset);

	return 0;
}

void TCPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int TCPSendMsg(int socketFD, struct sockaddr_in * to, struct TCPMessage *msg)
{
	char buffer[TCP_SOCKET_MTU];

	struct TCPPacketHeader *customHeader = (struct TCPPacketHeader *) (buffer);
	memcpy(customHeader->magic, TCP_PACKET_MAGIC, sizeof(customHeader->magic));
	customHeader->type = msg->packetType;

	memcpy(buffer + sizeof(struct TCPPacketHeader), msg->buffer, msg->size);

	int sentSize = sendto(socketFD, buffer, msg->size + sizeof(struct TCPPacketHeader), 0, (struct sockaddr*)to, sizeof(*to));

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an TCP packet: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int TCPSocketOpen()
{
	int TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (TCPSocket < 0)
	{
		fprintf(stderr, "Unable to open an TCP socket: %s\n", strerror(errno));
		return 1;
	}

	return TCPSocket;
}

bool TCPequalSockaddr(struct sockaddr_in * a, struct sockaddr_in * b)
{
	return a && b && a->sin_port == b->sin_port && a->sin_addr.s_addr == b->sin_addr.s_addr;
}