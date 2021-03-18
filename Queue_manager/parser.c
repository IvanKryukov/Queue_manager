#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define MAX_CMD_LENGTH (uint32_t)(3)
#define MAX_ARG_LENGTH (uint32_t)(5)

int get_command( const char * cmd_string)
{
	int cmd_length = 0;

	while (*cmd_string != '\0')
		cmd_length++;

	if (cmd_length > MAX_CMD_LENGTH)
		return -1;

	return 0;
}

int get_argument( const char * arg_string)
{
	int arg_length = 0;

	while (*arg_string != '\0')
		arg_length++;

	if (arg_length > MAX_ARG_LENGTH)
		return -1;

	return 0;
}