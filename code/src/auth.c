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

void generateResponse(struct auth_context *ctx)
{
	size_t totalLen = AUTH_CHALLENGE_LENGTH + globalKeyfile.length;

	unsigned char *buffer = malloc(totalLen);
	memcpy(buffer, ctx->challenge, AUTH_CHALLENGE_LENGTH);
	memcpy(buffer + AUTH_CHALLENGE_LENGTH, globalKeyfile.data, globalKeyfile.length);

	unsigned char hash[AUTH_RESPONSE_LENGTH];

	SHA256(buffer, totalLen, ctx->hash);

	free(buffer);
}

bool checkResponse(struct auth_context *ctx, unsigned char * response, int responseLength)
{
	if (responseLength != AUTH_RESPONSE_LENGTH)
		return false;

	return memcmp(ctx->hash, response, AUTH_RESPONSE_LENGTH) == 0;
}