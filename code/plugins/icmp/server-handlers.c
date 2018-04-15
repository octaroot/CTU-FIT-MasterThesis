#include <stdlib.h>
#include "server-handlers.h"
#include "server-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"


void ICMPServerInitialize(uint32_t endpoint)
{
	ICMPSequenceNumber = 0;
	ICMPIDNumber = 0;
}

void ICMPServerCheckHealth(uint32_t endpoint)
{

}

void ICMPServerICMPData(uint32_t endpoint)
{
	ICMPEchoMessage msg;
	uint32_t sender;

	if (ICMPReceiveEcho(_ICMPSocketFD, &sender, &msg))
		return;

	if (sender != endpoint || !msg.type != ICMP_ECHO_REPLY || !msg.size)
		return;

	switch (msg.packetType)
	{
		case ICMP_CONNECTION_REQUEST:
			ICMPHandlConnectionRequest(_ICMPSocketFD, endpoint, &msg);
			break;
		case ICMP_DATA:
			ICMPHandleICMPData(&msg);
			break;
		case ICMP_NATPACKET:
			ICMPHandleNATPacket(_ICMPSocketFD, endpoint, &msg);
			break;
		case ICMP_KEEPALIVE:
			//TODO
			break;
	}
}

void ICMPServerTunnelData(uint32_t endpoint)
{
	ICMPEchoMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.type = ICMP_ECHO_REQUEST;
	msg.packetType = ICMP_DATA;
	msg.seq = NATSequenceNumbers[NATSequenceNumberIdx++];
	NATSequenceNumberIdx %= ICMP_NAT_PACKET_COUNT;

	ICMPSendEcho(_ICMPSocketFD, endpoint, &msg);
}