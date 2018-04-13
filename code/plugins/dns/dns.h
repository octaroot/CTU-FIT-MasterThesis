#ifndef CODE_DNS_H
#define CODE_DNS_H

#include <stdint.h>

#define DNS_PLUGIN_VERSION    "1.0.0-57"

extern int tunDeviceFD;

bool _DNSRunning;

void _DNSCleanup();

bool _DNSTestAvailability(uint32_t endpoint);

const char *_DNSGetVersion();

void _DNSStart(uint32_t endpoint, bool serverMode);

void _DNSStop();

#endif //CODE_DNS_H
