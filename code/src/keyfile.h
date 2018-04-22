#ifndef CODE_KEYFILE_H
#define CODE_KEYFILE_H

#define KEYFILE_MAX_SIZE    1024
#define KEYFILE_MIN_SIZE    16

#include <stdlib.h>
#include <stdbool.h>

typedef struct keyfile
{
	char *data;
	size_t length;
} keyfile;


bool keyfileRead(const char *filename);

void keyfileClose();

#endif //CODE_KEYFILE_H
