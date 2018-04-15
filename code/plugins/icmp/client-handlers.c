#include <stdlib.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"


void ICMPClientInitialize(uint32_t endpoint)
{
	ICMPSequenceNumber = rand();
	ICMPIDNumber = rand();

	ICMPSendConnectionRequest(_ICMPSocketFD, endpoint);
}

void ICMPClientCheckHealth(uint32_t endpoint)
{
}

void ICMPClientICMPData(uint32_t endpoint)
{
	ICMPEchoMessage msg;
	uint32_t sender;

	if (ICMPReceiveEcho(_ICMPSocketFD, &sender, &msg))
		return;

	if (sender != endpoint || !msg.type != ICMP_ECHO_REPLY || !msg.size)
		return;

	switch (msg.packetType)
	{
		case ICMP_CONNECTION_ACCEPT:
			ICMPHandleConnectionAccept(_ICMPSocketFD, endpoint);
			break;
		case ICMP_CONNECTION_REJECT:
			ICMPHandleConnectionReject(_ICMPSocketFD, endpoint);
			break;
		case ICMP_KEEPALIVE:
			ICMPHandleKeepAliveResponse(_ICMPSocketFD, endpoint);
			break;
		case ICMP_DATA:
			ICMPHandleICMPData(&msg);
			break;
	}
}

void ICMPClientTunnelData(uint32_t endpoint)
{
	ICMPEchoMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_DATA;
	msg.seq = ICMPSequenceNumber++;

	ICMPSendEcho(_ICMPSocketFD, endpoint, &msg);
}