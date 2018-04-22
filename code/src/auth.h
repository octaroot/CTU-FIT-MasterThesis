#ifndef CODE_AUTH_H
#define CODE_AUTH_H

#include <stdint.h>
#include <stdbool.h>
#include "keyfile.h"

#define AUTH_CHALLENGE_LENGTH	32

extern struct keyfile globalKeyfile;

typedef struct auth_context
{
	char challenge[AUTH_CHALLENGE_LENGTH];
};

void initializeContext(struct auth_context * ctx);
bool checkResponse(struct auth_context *ctx, char *response, int responseLength);

#endif //CODE_AUTH_H
