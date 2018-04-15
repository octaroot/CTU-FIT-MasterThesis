#include <memory.h>
#include <stdlib.h>
#include "client-functions.h"
#include "packet.h"

void ICMPClientInitialize(uint32_t endpoint)
{
	ICMPSequenceNumber = rand();
	ICMPIDNumber = rand();

	ICMPsendConnectionRequest(_ICMPSocketFD, endpoint);
}

void ICMPsendConnectionRequest(int socketFD, uint32_t endpoint)
{
	struct ICMPEchoMessage msg;
	msg.size = 0;
	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_CONNECTION_REQUEST;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(socketFD, endpoint, &msg);
}