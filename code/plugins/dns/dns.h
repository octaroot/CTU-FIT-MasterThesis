#ifndef CODE_DNS_H
#define CODE_DNS_H

#define DNS_PLUGIN_VERSION    "1.0.0-57"

extern int tunDeviceFD;

bool _DNSRunning;

void _DNSCleanup();

bool _DNSTestAvailability(unsigned int endpoint);

const char *_DNSGetVersion();

void _DNSStart(unsigned int endpoint);

void _DNSStop();

#endif //CODE_DNS_H
