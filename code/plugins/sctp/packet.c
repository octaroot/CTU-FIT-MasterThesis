#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/sctp.h>
#include <stdbool.h>
#include <sys/param.h>
#include <stdlib.h>

#include "packet.h"
#include "sctp.h"
#include "client-functions.h"

int SCTPReceiveMsg(int socketFD, struct SCTPMessage *msg)
{
	int flags;
	struct sctp_sndrcvinfo sndrcvinfo;
	char buffer[SCTP_SOCKET_MTU];

	int readSize = sctp_recvmsg(socketFD, buffer, sizeof(buffer), NULL, 0, &sndrcvinfo, &flags);

	if (readSize < 0)
	{
		fprintf(stderr, "Unable to receive an SCTP packet: %s (%d)\n", strerror(errno), errno);
		return 1;
	}

	if (readSize < 1)
		return 1;

	switch (sndrcvinfo.sinfo_stream)
	{
		case SCTP_STREAM_CONTROL:
			msg->packetType = buffer[0];
			msg->size = readSize - 1;
			buffer[0] = msg->packetType;
			memcpy(msg->buffer, buffer + 1, msg->size);
			break;
		case SCTP_STREAM_DATA:
			if (pluginState.auth)
			{
				tunWrite(tunDeviceFD, buffer, readSize);
			}
			break;
		default:
			return 1;
	}

	return 0;
}

void SCTPSocketClose(int socketFD)
{
	if (socketFD > 0)
		close(socketFD);
}

int SCTPSendControl(int socketFD, struct SCTPMessage *msg)
{
	char buffer[SCTP_SOCKET_MTU];

	buffer[0] = msg->packetType;
	memcpy(buffer + 1, msg->buffer, msg->size);

	int sentSize = sctp_sendmsg(socketFD, buffer, msg->size + 1, NULL, 0, 0, 0, SCTP_STREAM_CONTROL, 0, 0);

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an SCTP packet: %s\n", strerror(errno));
		_SCTPStopClient();
		return 1;
	}

	return 0;
}

int SCTPSendData(int socketFD, struct SCTPMessage *msg)
{
	int sentSize = sctp_sendmsg(socketFD, msg->buffer, msg->size, NULL, 0, 0, 0, SCTP_STREAM_DATA, 0, 0);

	if (sentSize < 0)
	{
		fprintf(stderr, "Unable to send an SCTP packet: %s\n", strerror(errno));
		_SCTPStopClient();
		return 1;
	}

	return 0;
}


void SCTPSetInitMsg(int socketFD)
{
	struct sctp_initmsg initmsg;

	memset(&initmsg, 0, sizeof(initmsg));
	initmsg.sinit_num_ostreams = 2;
	initmsg.sinit_max_instreams = 2;
	initmsg.sinit_max_attempts = 5;

	if (setsockopt(socketFD, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg)) < 0)
	{
		fprintf(stderr, "Unable to initialize SCTP socket (initmsg): %s\n", strerror(errno));
		_SCTPStop();
	}
}

void SCTPSetEvents(int socketFD)
{
	struct sctp_event_subscribe events;

	memset(&events, 0, sizeof(events));
	events.sctp_data_io_event = 1;
	if (setsockopt(socketFD, SOL_SCTP, SCTP_EVENTS, &events, sizeof(events)) < 0) {
		fprintf(stderr, "Unable to initialize SCTP socket (SCTP events): %s\n", strerror(errno));
		_SCTPStop();
	}
}


int SCTPSocketOpen()
{
	int SCTPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	if (SCTPSocket < 0)
	{
		fprintf(stderr, "Unable to open an SCTP socket: %s\n", strerror(errno));
		return 1;
	}

	return SCTPSocket;
}