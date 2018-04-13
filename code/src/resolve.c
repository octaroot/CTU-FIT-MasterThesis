#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "resolve.h"

uint32_t resolve(const char *hostname)
{
	uint32_t address = ntohl(inet_addr(hostname));

	if (address == INADDR_NONE)
	{
		struct hostent *tmp = gethostbyname(hostname);

		if (!tmp)
		{
			return 0;
		}

		return ntohl(*(uint32_t *) tmp->h_addr_list[0]);
	}

	return 0;
}