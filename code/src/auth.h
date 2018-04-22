#ifndef CODE_AUTH_H
#define CODE_AUTH_H

#include <stdint.h>
#include <stdbool.h>
#include <openssl/sha.h>
#include "keyfile.h"

#define AUTH_CHALLENGE_LENGTH    32
#define AUTH_RESPONSE_LENGTH    SHA256_DIGEST_LENGTH

extern struct keyfile globalKeyfile;

typedef struct auth_context
{
	char challenge[AUTH_CHALLENGE_LENGTH];
	unsigned char hash[AUTH_RESPONSE_LENGTH];
};

void initializeContext(struct auth_context *ctx);

void generateResponse(struct auth_context *ctx);

bool checkResponse(struct auth_context *ctx, unsigned char *response, int responseLength);

#endif //CODE_AUTH_H
