#include <stdlib.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"


void ICMPClientInitialize(uint32_t endpoint)
{
	ICMPSequenceNumber = rand();
	ICMPIDNumber = rand();

	ICMPSendConnectionRequest(_ICMPSocketFD, endpoint);
}

void ICMPClientCheckHealth(uint32_t endpoint)
{
	ICMPEchoMessage msg;
	uint32_t sender;

	if (ICMPReceiveEcho(_ICMPSocketFD, &sender, &msg))
		return;

	if (sender != endpoint || !msg.type != ICMP_ECHO_REPLY || !msg.size)
		return;

}

void ICMPClientICMPData(uint32_t endpoint)
{

}

void ICMPClientTunnelData(uint32_t endpoint)
{

}