#ifndef CODE_MUX_H
#define CODE_MUX_H

#include <stdbool.h>

typedef struct plugin
{
	const char *(*getVersion)();

	bool (*testAvailability)(unsigned int endpoint);

	void (*start)(unsigned int endpoint);

	void (*stop)();

} plugin;

void mux_start();

void mux_stop();

#endif //CODE_MUX_H
