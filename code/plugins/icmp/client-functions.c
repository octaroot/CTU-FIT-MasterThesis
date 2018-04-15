#include <memory.h>
#include <stdlib.h>
#include "client-functions.h"
#include "packet.h"

void clientInitialize()
{
	ICMPSequenceNumber = rand();
	ICMPIDNumber = rand();
}

void sendConnectionRequest(int socketFD, uint32_t endpoint)
{
	struct ICMPEchoMessage msg;
	msg.size = 0;
	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_CONNECTION_REQUEST;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(socketFD, endpoint, &msg);
}