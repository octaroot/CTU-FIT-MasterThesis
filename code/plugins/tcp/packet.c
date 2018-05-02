#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <sys/param.h>

#include "packet.h"
#include "tcp.h"

int TCPReceiveMsg(int socketFD, struct sockaddr_in *from, struct TCPMessage *msg)
{
	TCPPacketHeader customHeader;

	unsigned char headerBuffer[3];

	int readSize;

	readSize = read(socketFD, headerBuffer, 3);
	if (readSize < 0)
	{
		_TCPStopClient();
	}
	else if (readSize != 3)
	{
		fprintf(stderr, "Received malformed TCP packet: Unable to read packet header: %s\n", strerror(errno));
		return 1;
	}

	customHeader.type = headerBuffer[0];
	customHeader.length = headerBuffer[1] | (headerBuffer[2] << 8);

	if (customHeader.length < 0)
	{
		fprintf(stderr, "Unable to receive an TCP packet: %s\n", strerror(errno));
		return 1;
	}

	if (customHeader.length > TCP_SOCKET_MTU)
	{
		fprintf(stderr, "Received malformed TCP packet: Size is too large (%d)\n", customHeader.length);
		return 1;
	}

	msg->size = customHeader.length;
	msg->packetType = customHeader.type;

	readSize = read(socketFD, &(msg->buffer), customHeader.length);

	if (readSize < 0)
	{
		_TCPStopClient();
	}
	else if (readSize != customHeader.length)
	{
		fprintf(stderr, "Received malformed TCP packet: Bad size (no data left to read)\n");
		return 1;
	}

	return 0;
}

void TCPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int TCPSendMsg(int socketFD, struct sockaddr_in *to, struct TCPMessage *msg)
{
	char buffer[TCP_SOCKET_MTU];
	buffer[0] = msg->packetType;
	buffer[1] = msg->size & 0xFF;
	buffer[2] = msg->size >> 8;

	memcpy(buffer + 3, msg->buffer, msg->size);

	int sentSize = write(socketFD, buffer, msg->size + 3);

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an TCP packet: %s\n", strerror(errno));
		_TCPStopClient();
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


	int optval = 1;
	if (setsockopt(TCPSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) < 0)
	{
		fprintf(stderr, "Unable to set SO_REUSEADDR on TCP socket: %s\n", strerror(errno));
		return 1;
	}


	return TCPSocket;
}