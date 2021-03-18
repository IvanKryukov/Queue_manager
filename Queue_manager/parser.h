#if !(defined PARSER_H)
#define PARSER_H

#include "stdint.h"

int	get_command(  const char* cmd_string);
int get_argument( const char* arg_string);

#endif
