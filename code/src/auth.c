#include <memory.h>
#include <stdio.h>
#include "auth.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdlib.h>

void initializeContext(struct auth_context *ctx)
{
	RAND_pseudo_bytes(ctx->challenge, AUTH_CHALLENGE_LENGTH);
	generateResponse(ctx);
}

void generateResponse(struct auth_context *ctx)
{
	size_t totalLen = AUTH_CHALLENGE_LENGTH + globalKeyfile.length;

	unsigned char *buffer = malloc(totalLen);
	memcpy(buffer, ctx->challenge, AUTH_CHALLENGE_LENGTH);
	memcpy(buffer + AUTH_CHALLENGE_LENGTH, globalKeyfile.data, globalKeyfile.length);

	SHA256(buffer, totalLen, ctx->response);

	printf("\nVSTU:");
	for (int i = 0; i < AUTH_CHALLENGE_LENGTH; ++i)
	{
		printf("%x ", ctx->challenge[i]);
	}
	printf("\n out:");
	for (int i = 0; i < AUTH_RESPONSE_LENGTH; ++i)
	{
		printf("%x ", ctx->response[i]);
	}
	puts("");

	free(buffer);
}

bool checkResponse(struct auth_context *ctx, unsigned char * response, int responseLength)
{
	if (responseLength != AUTH_RESPONSE_LENGTH)
		return false;

printf("\norig:");
	for (int i = 0; i < AUTH_RESPONSE_LENGTH; ++i)
	{
		printf("%x ", ctx->response[i]);
	}
printf("\ntest:");
	for (int i = 0; i < AUTH_RESPONSE_LENGTH; ++i)
	{
		printf("%x ", response[i]);
	}
	puts("");

	return memcmp(ctx->response, response, AUTH_RESPONSE_LENGTH) == 0;
}