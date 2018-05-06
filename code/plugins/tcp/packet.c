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

int TCPReceiveMsg(struct TCPPluginState * pluginStateTCP, struct TCPMessage *msg)
{
	TCPPacketHeader customHeader;

	unsigned char headerBuffer[3];

	unsigned int readSize;

	readSize = read(pluginStateTCP->socket, headerBuffer, 3);
	if (readSize < 0)
	{
		_TCPStopClient(pluginStateTCP);
		return 1;
	}
	else if (readSize != 3)
	{
		fprintf(stderr, "Received malformed TCP packet: Unable to read packet header: %s (%d)\n", strerror(errno), errno);
		_TCPStopClient(pluginStateTCP);
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

	readSize = 0;
	while (readSize != customHeader.length)
	{
		int result = read(pluginStateTCP->socket, msg->buffer, customHeader.length - readSize);
		if (result < 0)
		{
			_TCPStopClient(pluginStateTCP);
			fprintf(stderr, "Unable to read from TCP socket: %s (%d)\n", strerror(errno), errno);
			return 1;
		}
		readSize += result;
		//fprintf(stderr, "Read %d bytes out of required %d, still waiting for %d\n", readSize, customHeader.length, customHeader.length - readSize);
		if (readSize < 0)
		{
			_TCPStopClient(pluginStateTCP);
		}
	}

	return 0;
}

void TCPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int TCPSendMsg(struct TCPPluginState * pluginStateTCP, struct TCPMessage *msg)
{
	char buffer[TCP_SOCKET_MTU];
	buffer[0] = msg->packetType;
	buffer[1] = msg->size & 0xFF;
	buffer[2] = msg->size >> 8;

	memcpy(buffer + 3, msg->buffer, msg->size);

	int sentSize = write(pluginStateTCP->socket, buffer, msg->size + 3);

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an TCP packet: %s\n", strerror(errno));
		_TCPStopClient(pluginStateTCP);
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