#ifndef CODE_MUX_H
#define CODE_MUX_H

#include <stdbool.h>
#include <stdint.h>

#include "plugin-parser.h"

typedef struct plugin
{
	const char *(*getVersion)();

	const char *(*getName)();

	bool (*testAvailability)(uint32_t endpoint);

	void (*start)(uint32_t endpoint, int port, bool serverMode);

	void (*stop)();

} plugin;

void muxListPlugins();

void muxTestPlugins(uint32_t endpoint);

void muxStart(bool serverMode, uint32_t address, struct pluginOptions * requiredPlugins, int count);

void muxStartPlugin(bool serverMode, uint32_t address, int port, const char * pluginName);

void muxStop();

#endif //CODE_MUX_H
