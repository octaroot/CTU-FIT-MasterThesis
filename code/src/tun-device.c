#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdlib.h>
#include <unistd.h>

#include "tun-device.h"

int tunOpen()
{
	int baseTunDev = open(TUN_DEVICE_BASE, O_RDWR);

	if (baseTunDev < 0)
	{
		fprintf(stderr, "Unable to open base TUN device (%s): %s\n", TUN_DEVICE_BASE, strerror(errno));
		return 1;
	}

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_NO_PI | IFF_TUN;

	if (ioctl(baseTunDev, TUNSETIFF, &ifr) < 0)
	{
		fprintf(stderr, "Unable to create a TUN device: %s\n", strerror(errno));
		return 1;
	}

	printf("TUN interface created: %s\n", ifr.ifr_name);

	return baseTunDev;
}

int tunWrite(int device, const char *buffer, int size)
{
	int bytesWritten = write(device, buffer, size);

	if (bytesWritten != size && bytesWritten > 0)
	{
		fprintf(stderr, "Unable to write to TUN device: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int tunRead(int device, char *buffer, int *size)
{
	if ((*size = read(device, buffer, TUN_DEVICE_MTU)) < 0)
	{
		fprintf(stderr, "unable to read from tunnel device: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

void tunClose(int device)
{
	if (device >= 0)
	{
		close(device);
	}
}