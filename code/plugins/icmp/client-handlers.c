#include <stdlib.h>
#include <stdio.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "icmp.h"


void ICMPClientInitialize(uint32_t endpoint)
{
	ICMPSequenceNumber = rand();
	ICMPIDNumber = rand();

	ICMPSendConnectionRequest(_ICMPSocketFD, endpoint);
}

void ICMPClientCheckHealth(uint32_t endpoint)
{
	if (pluginState.noReplyCount++ > ICMP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_ICMPStop();
		return;
	}

	ICMPSendKeepAlive(_ICMPSocketFD, endpoint);
}

void ICMPClientICMPData(uint32_t endpoint)
{
	ICMPEchoMessage msg;
	uint32_t sender;

	if (ICMPReceiveEcho(_ICMPSocketFD, &sender, &msg))
		return;

	if (pluginState.connected && pluginState.endpoint && sender != pluginState.endpoint)
		return;

	if (msg.type != ICMP_ECHO_REPLY)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case ICMP_CONNECTION_ACCEPT:
			ICMPHandleConnectionAccept(_ICMPSocketFD, sender);
			break;
		case ICMP_CONNECTION_REJECT:
			ICMPHandleConnectionReject(_ICMPSocketFD, sender);
			break;
		case ICMP_KEEPALIVE:
			ICMPHandleKeepAliveResponse(_ICMPSocketFD, sender);
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