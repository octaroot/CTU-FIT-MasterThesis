#ifndef CODE_MUX_H
#define CODE_MUX_H

#include <stdbool.h>

typedef struct plugin
{
	const char *(*getVersion)();
} plugin;

void mux_start();

#endif //CODE_MUX_H
