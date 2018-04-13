//
// Created by martin on 13.4.18.
//

#ifndef CODE_TUN_DEVICE_H
#define CODE_TUN_DEVICE_H

#define TUN_DEVICE_BASE    "/dev/net/tun"
#define TUN_DEVICE_MTU    1500

int tunOpen();

int tunWrite(int device, const char *buffer, int size);

int tunRead(int device, char *buffer, int *size);

void tunClose(int device);


#endif //CODE_TUN_DEVICE_H
