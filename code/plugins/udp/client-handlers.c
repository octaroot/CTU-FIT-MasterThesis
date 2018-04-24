#include <stdlib.h>
#include <stdio.h>
#include "client-handlers.h"
#include "client-functions.h"
#include "packet.h"
#include "../../src/tun-device.h"
#include "udp.h"


void UDPClientInitialize(uint32_t endpoint)
{
	UDPSequenceNumber = rand();
	UDPIDNumber = rand();

	UDPSendConnectionRequest(_UDPSocketFD, endpoint);
}

void UDPClientCheckHealth(uint32_t endpoint)
{
	if (!pluginState.connected)
		return;

	if (pluginState.noReplyCount++ > UDP_KEEPALIVE_TIMEOUT)
	{
		// timed out, close connection
		_UDPStop();
		return;
	}

	UDPSendKeepAlive(_UDPSocketFD, endpoint);
}

void UDPClientUDPData(uint32_t endpoint)
{
	UDPEchoMessage msg;
	uint32_t sender;

	if (UDPReceiveEcho(_UDPSocketFD, &sender, &msg))
		return;

	if (pluginState.connected && pluginState.endpoint && sender != pluginState.endpoint)
		return;

	if (msg.type != UDP_ECHO_REPLY)
		return;

	if (!msg.size)
		return;

	switch (msg.packetType)
	{
		case UDP_AUTH_CHALLENGE:
			UDPHandleAuthChallenge(_UDPSocketFD, sender, &msg);
			break;
		case UDP_CONNECTION_ACCEPT:
			UDPHandleConnectionAccept(_UDPSocketFD, sender);
			break;
		case UDP_CONNECTION_REJECT:
			UDPHandleConnectionReject(_UDPSocketFD, sender);
			break;
		case UDP_KEEPALIVE:
			UDPHandleKeepAliveResponse(_UDPSocketFD, sender);
			break;
		case UDP_DATA:
			UDPHandleUDPData(&msg);
			break;
	}
}

void UDPClientTunnelData(uint32_t endpoint)
{
	UDPEchoMessage msg;
	tunRead(tunDeviceFD, (char*)&(msg.buffer), &(msg.size));

	if (!msg.size)
		return;

	msg.type = UDP_ECHO_REQUEST;
	msg.packetType = UDP_DATA;
	msg.seq = UDPSequenceNumber++;

	UDPSendEcho(_UDPSocketFD, endpoint, &msg);
}