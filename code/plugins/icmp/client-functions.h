#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>

extern uint16_t ICMPSequenceNumber, ICMPIDNumber;

void clientInitialize();

void sendConnectionRequest(int socketFD, uint32_t endpoint);

#endif //CODE_CLIENT_FUNCTIONS_H
