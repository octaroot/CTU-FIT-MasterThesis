#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "packet.h"

uint16_t ICMPSequenceNumber, ICMPIDNumber;

int ICMPReceiveEcho(int socketFD, uint32_t *from, struct ICMPEchoMessage *msg)
{
	char buffer[ICMP_SOCKET_MTU];

	struct sockaddr_in server;
	socklen_t serverSize = sizeof(struct sockaddr_in);

	int receivedSize = recvfrom(socketFD, buffer, ICMP_SOCKET_MTU, 0, (struct sockaddr *) &server, &serverSize);

	if (receivedSize < 0)
	{
		fprintf(stderr, "Unable to receive an ICMP packet: %s\n", strerror(errno));
		return 1;
	}

	if (receivedSize < (int) sizeof(struct iphdr) + (int) sizeof(struct icmphdr))
	{
		fprintf(stderr, "Received malformed ICMP packet: %s\n", strerror(errno));
		return 1;
	}

	struct icmphdr *header = (struct icmphdr *) (buffer + sizeof(struct iphdr));

	if ((header->type != ICMP_ECHO_REQUEST && header->type != ICMP_ECHO_REPLY) || header->code != 0)
		return 1;

	int offset = sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct ICMPPacketHeader);

	struct ICMPPacketHeader *customHeader = (struct ICMPPacketHeader *) (buffer + sizeof(struct iphdr) +
																		 sizeof(struct icmphdr));

	if (memcmp(customHeader->magic, ICMP_PACKET_MAGIC, sizeof(customHeader->magic)) != 0)
		return 1;

	msg->size = receivedSize - offset;
	msg->type = header->type;
	msg->id = ntohs(header->un.echo.id);
	msg->seq = ntohs(header->un.echo.sequence);
	memcpy(msg->buffer, buffer + offset, receivedSize - offset);

	*from = ntohl(server.sin_addr.s_addr);

	return 0;
}

void ICMPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int ICMPSendEcho(int socketFD, uint32_t to, struct ICMPEchoMessage *msg)
{
	char buffer[ICMP_SOCKET_MTU];

	struct icmphdr *header = (struct icmphdr *) (buffer);
	header->type = msg->type;
	header->code = 0;
	header->un.echo.id = htons(ICMPIDNumber);
	header->un.echo.sequence = htons(msg->seq);
	header->checksum = 0;

	struct ICMPPacketHeader *customHeader = (struct ICMPPacketHeader *) (buffer + sizeof(struct icmphdr));
	memcpy(customHeader->magic, ICMP_PACKET_MAGIC, sizeof(customHeader->magic));
	customHeader->type = ICMP_CONNECTION_REQUEST;

	memcpy(buffer + sizeof(struct icmphdr) + sizeof(struct ICMPPacketHeader), msg->buffer, msg->size);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(to);

	int sentSize = sendto(socketFD, buffer, msg->size + sizeof(struct ICMPPacketHeader) + sizeof(struct icmphdr), 0,
						  (struct sockaddr *) &server, sizeof(server));

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an ICMP packet: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int ICMPSocketOpen()
{
	int ICMPSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (ICMPSocket < 0)
	{
		fprintf(stderr, "Unable to open an ICMP socket: %s\n", strerror(errno));
		return 1;
	}

	return ICMPSocket;
}

