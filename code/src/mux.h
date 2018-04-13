#ifndef CODE_MUX_H
#define CODE_MUX_H

#include <stdbool.h>
#include <stdint.h>

typedef struct plugin
{
	const char *(*getVersion)();

	bool (*testAvailability)(uint32_t endpoint);

	void (*start)(uint32_t endpoint);

	void (*stop)();

} plugin;

void mux_start(uint32_t);

void mux_stop();

#endif //CODE_MUX_H
