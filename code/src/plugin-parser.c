#include <string.h>
#include <stdio.h>
#include "plugin-parser.h"


int parsePluginOptions(char * input, struct pluginOptions * plugins)
{
	int count = 0;
	char * token = strtok(input, ",");

	while (token != NULL)
	{
		if (sscanf(token, "%15[a-zA-Z]", plugins[count].pluginName))
		{
			plugins[count].pluginName[15] = 0;
			plugins[count].port = 0;
			sscanf(token, "%*15[a-zA-Z]:%d", &plugins[count].port);
			++count;
		}

		token = strtok(NULL, ",");
	}

	return count;
}