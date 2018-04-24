#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_udp.h>

#include "packet.h"

uint16_t UDPSequenceNumber, UDPIDNumber;

int UDPReceiveEcho(int socketFD, uint32_t *from, struct UDPEchoMessage *msg)
{
	char buffer[UDP_SOCKET_MTU];

	struct sockaddr_in server;
	socklen_t serverSize = sizeof(struct sockaddr_in);

	int receivedSize = recvfrom(socketFD, buffer, UDP_SOCKET_MTU, 0, (struct sockaddr *) &server, &serverSize);

	if (receivedSize < 0)
	{
		fprintf(stderr, "Unable to receive an UDP packet: %s\n", strerror(errno));
		return 1;
	}

	if (receivedSize < (int) sizeof(struct iphdr) + (int) sizeof(struct udphdr))
	{
		fprintf(stderr, "Received malformed UDP packet: %s\n", strerror(errno));
		return 1;
	}

	struct udphdr *header = (struct udphdr *) (buffer + sizeof(struct iphdr));

	if ((header->type != UDP_ECHO_REQUEST && header->type != UDP_ECHO_REPLY) || header->code != 0)
		return 1;

	int offset = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct UDPPacketHeader);

	struct UDPPacketHeader *customHeader = (struct UDPPacketHeader *) (buffer + sizeof(struct iphdr) +
																		 sizeof(struct udphdr));

	if (memcmp(customHeader->magic, UDP_PACKET_MAGIC, sizeof(customHeader->magic)) != 0)
		return 1;

	msg->size = receivedSize - offset;
	msg->type = header->type;
	msg->packetType = customHeader->type;
	msg->id = ntohs(header->un.echo.id);
	msg->seq = ntohs(header->un.echo.sequence);
	memcpy(msg->buffer, buffer + offset, receivedSize - offset);

	*from = ntohl(server.sin_addr.s_addr);

	return 0;
}

void UDPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int UDPSendEcho(int socketFD, uint32_t to, struct UDPEchoMessage *msg)
{
	char buffer[UDP_SOCKET_MTU];

	struct udphdr *header = (struct udphdr *) (buffer);
	header->type = msg->type;
	header->code = 0;
	header->un.echo.id = htons(UDPIDNumber);
	header->un.echo.sequence = htons(msg->seq);
	header->checksum = 0;

	struct UDPPacketHeader *customHeader = (struct UDPPacketHeader *) (buffer + sizeof(struct udphdr));
	memcpy(customHeader->magic, UDP_PACKET_MAGIC, sizeof(customHeader->magic));
	customHeader->type = msg->packetType;

	memcpy(buffer + sizeof(struct udphdr) + sizeof(struct UDPPacketHeader), msg->buffer, msg->size);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(to);

	header->checksum = UDPPacketChecksum(buffer, sizeof(struct udphdr) + sizeof(struct UDPPacketHeader) + msg->size);

	int sentSize = sendto(socketFD, buffer, msg->size + sizeof(struct UDPPacketHeader) + sizeof(struct udphdr), 0,
						  (struct sockaddr *) &server, sizeof(server));

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an UDP packet: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int UDPSocketOpen()
{
	int UDPSocket = socket(AF_INET, SOCK_RAW, 0);

	if (UDPSocket < 0)
	{
		fprintf(stderr, "Unable to open an UDP socket: %s\n", strerror(errno));
		return 1;
	}

	return UDPSocket;
}