#ifndef CODE_CLIENT_FUNCTIONS_H
#define CODE_CLIENT_FUNCTIONS_H

#include <stdint.h>

extern uint16_t ICMPSequenceNumber, ICMPIDNumber;

extern int _ICMPSocketFD;

void ICMPClientInitialize(uint32_t endpoint);

void ICMPsendConnectionRequest(int socketFD, uint32_t endpoint);

#endif //CODE_CLIENT_FUNCTIONS_H
