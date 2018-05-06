#ifndef CODE_PLUGIN_PARSER_H
#define CODE_PLUGIN_PARSER_H

#define MAX_PLUGINS	32

typedef struct pluginOptions
{
	char pluginName[16];
	int port;
} pluginOptions;

int parsePluginOptions(char * input, struct pluginOptions* plugins);

#endif //CODE_PLUGIN_PARSER_H
