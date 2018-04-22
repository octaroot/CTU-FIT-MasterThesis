#include <stdio.h>

#include "keyfile.h"

struct keyfile globalKeyfile = {NULL, 0};

bool keyfileRead(const char *filename)
{
	FILE *keyfileHandle = fopen(filename, "rb");

	if (!keyfileHandle)
	{
		return false;
	}

	fseek(keyfileHandle, 0, SEEK_END);
	long keyfileSize = ftell(keyfileHandle);
	rewind(keyfileHandle);

	if (keyfileSize > 2048)
	{
		fprintf(stderr, "Keyfile too large (%li bytes), max %d\n", keyfileSize, KEYFILE_MAX_SIZE);
		return false;
	}
	else if (keyfileSize < 8)
	{
		fprintf(stderr, "Keyfile too small (%li bytes), min %d\n", keyfileSize, KEYFILE_MIN_SIZE);
		return false;
	}

	globalKeyfile.data = malloc(keyfileSize);
	fread(globalKeyfile.data, keyfileSize, 1, keyfileHandle);
	fclose(keyfileHandle);

	return true;
}


void keyfileClose()
{
	if (globalKeyfile.data && globalKeyfile.length)
	{
		free(globalKeyfile.data);
		struct keyfile asdf = {NULL, 0};;
		globalKeyfile = asdf;
	}
}