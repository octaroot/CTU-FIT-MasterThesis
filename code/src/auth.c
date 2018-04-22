#include <memory.h>
#include <stdio.h>
#include "auth.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdlib.h>

void initializeContext(struct auth_context *ctx)
{
	RAND_pseudo_bytes(ctx->challenge, AUTH_CHALLENGE_LENGTH);
}

bool checkResponse(struct auth_context *ctx, char *response, int responseLength)
{
	const int totalLen = AUTH_CHALLENGE_LENGTH + globalKeyfile.length;

	char * buffer = malloc(totalLen);
	memcpy(buffer, ctx->challenge, AUTH_CHALLENGE_LENGTH);
	memcpy(buffer + AUTH_CHALLENGE_LENGTH, totalLen, globalKeyfile.data);

	unsigned char hash[SHA256_DIGEST_LENGTH];

	SHA256(buffer, totalLen, hash);

	bool result = memcmp(buffer, hash, totalLen) == 0;

	free(buffer);

	return result;
}